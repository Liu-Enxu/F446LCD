#include <string.h>

#include "screens.h"
#include "lv_app_conf.h"

#include "FreeRTOS.h"
#include "task.h"

#include "LCD.h"



/*
 * private variable
 */ 
// calibration circle positions
static u16 circle_pos[6][2] = {
	{30,30},{HOR_RESOLUTION/2,30},{HOR_RESOLUTION,30},
	{30,VER_RESOLUTION},{HOR_RESOLUTION/2,VER_RESOLUTION},{HOR_RESOLUTION,VER_RESOLUTION}
};

/*
 * private functions forward declarations
 */ 
static scrn_load_t*	create_screen_load(void);
static scrn_main_t* create_screen_main(void);
static scrn_cali_t* create_screen_cali(void);

/* ================================================================== */
/*  Screen Manager				   									*/
/* ================================================================== */

static void scrn_manager_switch(scrn_t *next) {	// use this in all exiting callback
    scrn_t *curr = scrn_manager_inst()->curr_scrn_ptr;
    next->scrn_t_enter(next);	// create new scrn before clear old due to stupid logic inside lv_disp_load_scr which asks for old screen (root obj)
    if (curr) curr->scrn_t_exit(curr);
    scrn_manager_inst()->curr_scrn_ptr = next;
}

scrn_manager_t* scrn_manager_inst(void){
	static scrn_manager_t scrn_manager;
	if (!scrn_manager.scrn_manager_inited){
		scrn_manager.curr_scrn_ptr = NULL;
		scrn_manager.scrn_manager_inited = 1;
	}
	return &scrn_manager;
}

void start_scrn_manager(void){
	static u8 started = 0;
	if (!started){
		scrn_manager_switch((scrn_t*)create_screen_load());
		started = 1;
	} else {
		printf("Screen manager already started!\r\n");
	}
}

/* ================================================================== */
/*  General Screen (some kind of abstract base class?)					*/
/* ================================================================== */

static void scrn_t_enter_t(scrn_t* self){
	(void)self; // for now, nothing to do when entering a screen. could add animation later?
	// this should init all lvgl objects for specific screen
}

static void scrn_t_exit_t(scrn_t* self){
	lv_obj_clean(self->screen);	//	lv_obj_del should del all children...lv_obj_clean?
	lv_refr_now(lv_obj_get_disp(lv_scr_act()));
	// scrn_t* next_scrn = next->scrn_t_enter(next);
	lv_obj_del(self->screen);
	lv_mem_free(self);	// free memory explicitly
}

static void create_scrn_t(scrn_t* self){
	self->scrn_t_enter = scrn_t_enter_t;
	self->scrn_t_exit = scrn_t_exit_t;
}
/* ================================================================== */
/*  Calibration Screen                                                */
/* ================================================================== */

// callbacks for calibration screen - calibraiton touch
static void cali_cb(lv_event_t* e){
	lv_event_code_t code = lv_event_get_code(e);
	scrn_cali_t *cali_scrn = lv_event_get_user_data(e);
	if(code != LV_EVENT_RELEASED) return;
	
	// id 0-5: accumulate taps
	if (0==calibrat_t.isValidLimSet){
		if (circle_pos[calibrat_t.id][0] < HOR_RESOLUTION/4) {
			calibrat_t.valid_x_min = 0;
			calibrat_t.valid_x_max = 200; 
		} else if (circle_pos[calibrat_t.id][0] < 3*HOR_RESOLUTION/4) {
			calibrat_t.valid_x_min = 1024/2-100;
			calibrat_t.valid_x_max = 1024/2+100; 
		} else {
			calibrat_t.valid_x_min = 1024-200;
			calibrat_t.valid_x_max = 1024;
		}

		if (circle_pos[calibrat_t.id][1] < VER_RESOLUTION/4) {
			calibrat_t.valid_y_min = 1024-200;
			calibrat_t.valid_y_max = 1024;	
		} else if (circle_pos[calibrat_t.id][1] < 3*VER_RESOLUTION/4) {
			calibrat_t.valid_y_min = 1024/2-100;
			calibrat_t.valid_y_max = 1024/2+100; 
		} else {
			calibrat_t.valid_y_min = 0;
			calibrat_t.valid_y_max = 200; 
		}
	}

	if ( (lv_cursor_pos.raw_x < calibrat_t.valid_x_min) || (lv_cursor_pos.raw_x > calibrat_t.valid_x_max)){
		if(cali_scrn->scrn_base.posX_obj) lv_label_set_text_fmt(cali_scrn->scrn_base.posX_obj, "%s", "INV");
		calibrat_t.isInvalid = 1;
	} else {
		if(cali_scrn->scrn_base.posX_obj) lv_label_set_text_fmt(cali_scrn->scrn_base.posX_obj, "%u", lv_cursor_pos.raw_x);
	}
	if ( (lv_cursor_pos.raw_y < calibrat_t.valid_y_min) || (lv_cursor_pos.raw_y > calibrat_t.valid_y_max)){
		if(cali_scrn->scrn_base.posY_obj) lv_label_set_text_fmt(cali_scrn->scrn_base.posY_obj, "%s", "INV");
		calibrat_t.isInvalid = 1;
	} else {
		if(cali_scrn->scrn_base.posY_obj) lv_label_set_text_fmt(cali_scrn->scrn_base.posY_obj, "%u", lv_cursor_pos.raw_y);
	}
	if(calibrat_t.isInvalid) {
		calibrat_t.isInvalid = 0;
		return;
	} else {
		calibrat_t.cali_acc_x += lv_cursor_pos.raw_x;
		calibrat_t.cali_acc_y += lv_cursor_pos.raw_y;
		calibrat_t.cali_sub++;
	}

	if(calibrat_t.cali_sub < 3){
		lv_label_set_text_fmt(cali_scrn->circle_pos_obj, "%u,%u (%u/3)", circle_pos[calibrat_t.id][0], circle_pos[calibrat_t.id][1], calibrat_t.cali_sub);
		return;
	} else {
		// 3rd tap: store average and advance
		u16 avg_x = calibrat_t.cali_acc_x / 3;
		u16 avg_y = calibrat_t.cali_acc_y / 3;
		switch(calibrat_t.id){
			case 0: calibrat_t.tl_x = avg_x; calibrat_t.tl_y = avg_y; break;
			case 1: calibrat_t.tm_x = avg_x; calibrat_t.tm_y = avg_y; break;
			case 2: calibrat_t.tr_x = avg_x; calibrat_t.tr_y = avg_y; break;
			case 3: calibrat_t.bl_x = avg_x; calibrat_t.bl_y = avg_y; break;
			case 4: calibrat_t.bm_x = avg_x; calibrat_t.bm_y = avg_y; break;
			case 5: calibrat_t.br_x = avg_x; calibrat_t.br_y = avg_y; break;
		}
		calibrat_t.cali_acc_x = 0; 
		calibrat_t.cali_acc_y = 0; 
		calibrat_t.cali_sub = 0; 
		calibrat_t.isValidLimSet = 0; 
		calibrat_t.isInvalid = 0;
		calibrat_t.id++;
		
		if(6 > calibrat_t.id){
			lv_obj_set_pos(cali_scrn->circle, circle_pos[calibrat_t.id][0]-30, circle_pos[calibrat_t.id][1]-30);
			lv_label_set_text_fmt(cali_scrn->circle_pos_obj, "%u,%u (0/3)", circle_pos[calibrat_t.id][0], circle_pos[calibrat_t.id][1]);
		} else {
			calibrat_t.adc_x_left  = (calibrat_t.tl_x + calibrat_t.bl_x) / 2;
			calibrat_t.adc_x_right = (calibrat_t.tr_x + calibrat_t.br_x) / 2;
			calibrat_t.adc_y_top   = (calibrat_t.tl_y + calibrat_t.tm_y + calibrat_t.tr_y) / 3;
			calibrat_t.adc_y_bot   = (calibrat_t.bl_y + calibrat_t.bm_y + calibrat_t.br_y) / 3;
			calibrat_t.calibrated  = 1;
			calibrat_t.id = 0; calibrat_t.cali_acc_x = 0; calibrat_t.cali_acc_y = 0; calibrat_t.cali_sub = 0;
			lv_obj_add_flag(cali_scrn->circle, LV_OBJ_FLAG_HIDDEN);
			printf("cal: xl=%u xr=%u yt=%u yb=%u\r\n",
				calibrat_t.adc_x_left, calibrat_t.adc_x_right,
				calibrat_t.adc_y_top,  calibrat_t.adc_y_bot);
			printf("raw: tl=(%u,%u) tm=(%u,%u) tr=(%u,%u)\r\n",
				calibrat_t.tl_x, calibrat_t.tl_y,
				calibrat_t.tm_x, calibrat_t.tm_y,
				calibrat_t.tr_x, calibrat_t.tr_y);
			printf("raw: bl=(%u,%u) bm=(%u,%u) br=(%u,%u)\r\n",
				calibrat_t.bl_x, calibrat_t.bl_y,
				calibrat_t.bm_x, calibrat_t.bm_y,
				calibrat_t.br_x, calibrat_t.br_y);
			lv_label_set_text(cali_scrn->circle_pos_obj, "Done! Exiting..");
			lv_refr_now(lv_obj_get_disp(cali_scrn->scrn_base.screen));
			vTaskDelay(pdMS_TO_TICKS(1000));
			// load actual screen
			scrn_manager_switch((scrn_t*)create_screen_main());
			return;
		}
	}
}

// callbacks for calibration screen - exit button
static void cali_exit_cb(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_RELEASED){
		scrn_manager_switch((scrn_t*)create_screen_main());
	}
}

static void scrn_cali_t_enter(scrn_t* self){
	scrn_cali_t *cali = (scrn_cali_t *)self;
	// screen
	cali->scrn_base.screen = lv_obj_create(NULL);
	lv_obj_set_pos(cali->scrn_base.screen, 0, 0);
	lv_obj_set_size(cali->scrn_base.screen, HOR_RESOLUTION, VER_RESOLUTION);
	lv_obj_clear_flag(cali->scrn_base.screen, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_event_cb(cali->scrn_base.screen, cali_cb, LV_EVENT_ALL, cali);
	
	// circle
	cali->circle = lv_obj_create(cali->scrn_base.screen);
	lv_obj_set_size(cali->circle, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_pos(cali->circle,circle_pos[calibrat_t.id][0]-30,circle_pos[calibrat_t.id][1]-30);
	lv_obj_set_style_radius(cali->circle, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(cali->circle, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
  	lv_obj_set_style_bg_opa(cali->circle, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(cali->circle, LV_OBJ_FLAG_EVENT_BUBBLE);	
	
	// pos row container
	cali->scrn_base.pos_obj = lv_obj_create(cali->scrn_base.screen);
	lv_obj_center(cali->scrn_base.pos_obj);
	lv_obj_set_size(cali->scrn_base.pos_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_clear_flag(cali->scrn_base.pos_obj, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_set_layout(cali->scrn_base.pos_obj, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(cali->scrn_base.pos_obj, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_pad_column(cali->scrn_base.pos_obj, 4, LV_PART_MAIN);
	lv_obj_set_style_pad_all(cali->scrn_base.pos_obj, 0, LV_PART_MAIN);
	lv_obj_set_style_radius(cali->scrn_base.pos_obj, 0, LV_PART_MAIN);
	lv_obj_set_style_bg_opa(cali->scrn_base.pos_obj, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_set_style_border_width(cali->scrn_base.pos_obj, 0, LV_PART_MAIN);
	lv_obj_set_flex_align(cali->scrn_base.pos_obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_add_flag(cali->scrn_base.pos_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

	cali->lbl_pref_obj = lv_label_create(cali->scrn_base.pos_obj);
	lv_label_set_text(cali->lbl_pref_obj, "Point (");
	lv_obj_add_flag(cali->lbl_pref_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

	// circle pos
	cali->circle_pos_obj = lv_label_create(cali->scrn_base.pos_obj);
	lv_label_set_text_fmt(cali->circle_pos_obj,"%u,%u (0/3)",circle_pos[calibrat_t.id][0],circle_pos[calibrat_t.id][1]);
	lv_obj_add_flag(cali->circle_pos_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

	cali->lbl_sep_obj = lv_label_create(cali->scrn_base.pos_obj);
	lv_label_set_text(cali->lbl_sep_obj, "):");
	lv_obj_add_flag(cali->lbl_sep_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

	// posX
	cali->scrn_base.posX_obj = lv_label_create(cali->scrn_base.pos_obj); 
	// lv_cursor_pos.label_x = cali->scrn_base.posX_obj;
	lv_label_set_text(cali->scrn_base.posX_obj,"___");
	lv_obj_add_flag(cali->scrn_base.posX_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

	cali->lbl_comma_obj = lv_label_create(cali->scrn_base.pos_obj);
	lv_label_set_text(cali->lbl_comma_obj, ",");
	lv_obj_add_flag(cali->lbl_comma_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

	// posY
	cali->scrn_base.posY_obj = lv_label_create(cali->scrn_base.pos_obj); 
	// lv_cursor_pos.label_y = cali->scrn_base.posY_obj;
	lv_label_set_text(cali->scrn_base.posY_obj,"___");
	lv_obj_add_flag(cali->scrn_base.posY_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	lv_disp_load_scr(cali->scrn_base.screen);
}

static scrn_cali_t* create_screen_cali(void){
	scrn_cali_t *cali = lv_mem_alloc(sizeof(scrn_cali_t));
	if(cali == NULL){
		printf("Failed to allocate memory for scrn_cali_t\r\n");
		return NULL;
	}
	lv_memset_00(cali, sizeof(scrn_cali_t));
	cali->scrn_base.scrn_t_enter = scrn_cali_t_enter;
	cali->scrn_base.scrn_t_exit = scrn_t_exit_t;
	return cali;	
}
	
/* ================================================================== */
/*  Load/Splash Screen                                                */
/* ================================================================== */

// could be deprecated since OOP structure already handles this?
static void touch_cb(lv_event_t* e){	
	lv_event_code_t code = lv_event_get_code(e);
	scrn_t* scrn = (scrn_t*)(lv_event_get_user_data(e));
	if(scrn && scrn->posX_obj && scrn->posY_obj && (code == LV_EVENT_PRESSED || code==LV_EVENT_RELEASED)){
//		printf("RELEASED\n");
		lv_label_set_text_fmt(scrn->posX_obj, "%u", lv_cursor_pos.cursor_x);
		lv_label_set_text_fmt(scrn->posY_obj, "%u", lv_cursor_pos.cursor_y);
	}
}

static void load_exit_cb(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_RELEASED){
		scrn_manager_switch((scrn_t*)create_screen_main());
	}
}

static void scrn_load_t_enter(scrn_t* self){
	scrn_load_t *load = (scrn_load_t *)self;
	// screen
	load->scrn_base.screen = lv_obj_create(NULL);
	lv_obj_set_pos(load->scrn_base.screen, 0, 0);
	lv_obj_set_size(load->scrn_base.screen, HOR_RESOLUTION, VER_RESOLUTION);
	lv_obj_clear_flag(load->scrn_base.screen, LV_OBJ_FLAG_SCROLLABLE);
//    lv_obj_set_style_border_opa(obj, LV_OPA_TRANSP, 0);	// optional??
	lv_obj_add_event_cb(load->scrn_base.screen, touch_cb, LV_EVENT_ALL, load);

	// 	label obj
	load->L_obj = lv_label_create(load->scrn_base.screen);
	lv_obj_set_pos(load->L_obj, 480, 10);
	lv_obj_set_size(load->L_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_text(load->L_obj, " __     __   __  __ _ \n(  )   / _\\ (  )(  ( \\\n/ (_/\\/    \\ )( /    /\n\\____/\\_/\\_/(__)\\_)__)\n  __   ____ \n /  \\ / ___)	V0.2\n(  O )\\___ \\\n \\__/ (____/");
	lv_obj_set_style_transform_angle(load->L_obj, 900, LV_PART_MAIN | LV_STATE_DEFAULT);	
	lv_obj_add_style(load->L_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(load->L_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

	//	button obj
	load->b_obj = lv_btn_create(load->scrn_base.screen);
	lv_obj_set_pos(load->b_obj , 330, 240);
	lv_obj_set_size(load->b_obj , 100, 50);
	lv_obj_add_style(load->b_obj,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_event_cb(load->b_obj, load_exit_cb, LV_EVENT_ALL, load);
	lv_obj_add_flag(load->b_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	//	label obj on button obj
	load->l_obj = lv_label_create(load->b_obj);
	lv_obj_set_pos(load->l_obj, 0, 0);
	lv_obj_set_size(load->l_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_align(load->l_obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_label_set_text(load->l_obj, "MAIN\nMENU");
	lv_obj_add_flag(load->l_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// 	ascii img obj
	load->ascii_obj = lv_label_create(load->scrn_base.screen);
	lv_obj_set_pos(load->ascii_obj, 10, 0);
	lv_obj_set_size(load->ascii_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_text(load->ascii_obj, "         @@@@@@@@@@@     \n      @@@@@@@@@@@@@@@@   \n     @@@@@@@@@@@@@@@@@@@ \n    @@@@@@@@@@@@@@@@@@@@@\n    @@@@@@#%@%+=-=#%@@@@@\n   @@@@@@#=-:-==:-#*%%@@@\n    @@*%#::=*%+:::::-#%@@\n    @@-::::::::::::#%*@@@\n    @@@*:::::::::-::-*   \n     @@*-:::::------+%   \n    %=   ---------=#@@   \n   %%%%%%#***+++*% @@@   \n  %%%%%%%%%%@@@@   @@@   \n%%%%%%%%%%%%%%%%  @@@    ");
	lv_obj_add_style(load->ascii_obj,&lv_app_styles.char_color2,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(load->ascii_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

	//	pointer position obj
	load->scrn_base.pos_obj = lv_label_create(load->scrn_base.screen);
	lv_obj_set_pos(load->scrn_base.pos_obj, 240, 170);
	lv_obj_set_size(load->scrn_base.pos_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_text(load->scrn_base.pos_obj, "[Cursor]\n[X:    ]\n[Y:    ]");
	lv_obj_add_style(load->scrn_base.pos_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(load->scrn_base.pos_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	//	posX val
	load->scrn_base.posX_obj = lv_label_create(load->scrn_base.pos_obj);
	// lv_cursor_pos.label_x=load->scrn_base.posX_obj;
	lv_obj_set_pos(load->scrn_base.posX_obj, 40, 20);
	lv_obj_set_size(load->scrn_base.posX_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_text(load->scrn_base.posX_obj, "_  ");
	lv_obj_add_style(load->scrn_base.posX_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(load->scrn_base.posX_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	//	posY val
	load->scrn_base.posY_obj = lv_label_create(load->scrn_base.pos_obj);
	// lv_cursor_pos.label_y=load->scrn_base.posY_obj;
	lv_obj_set_pos(load->scrn_base.posY_obj, 40, 40);
	lv_obj_set_size(load->scrn_base.posY_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_text(load->scrn_base.posY_obj, "_  ");
	lv_obj_add_style(load->scrn_base.posY_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(load->scrn_base.posY_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// welcome label
	load->welcome_obj = lv_label_create(load->scrn_base.screen);
	lv_obj_set_pos(load->welcome_obj, 0, 300);
	lv_obj_set_size(load->welcome_obj, LV_SIZE_CONTENT, 20);
	lv_label_set_text(load->welcome_obj, "Let's all love Lain! <3<3<3"); // TODO: make it move
	lv_obj_add_style(load->welcome_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(load->welcome_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	lv_disp_load_scr(load->scrn_base.screen);
}

static scrn_load_t* create_screen_load(void) {
	scrn_load_t *load = lv_mem_alloc(sizeof(scrn_load_t));
	if(load == NULL){
		printf("Failed to allocate memory for scrn_load_t\r\n");
		return NULL;
	}
	lv_memset_00(load, sizeof(scrn_load_t));
	load->scrn_base.scrn_t_enter = scrn_load_t_enter;
	load->scrn_base.scrn_t_exit = scrn_t_exit_t;
	return load;
}

/* ================================================================== */
/*  Main Screen	                                                      */
/* ================================================================== */

static void heap_label_cb(lv_timer_t *t) {
    lv_label_set_text_fmt((lv_obj_t *)t->user_data, "][RAM: %u%%", xPortGetFreeHeapSize() * 100 / configTOTAL_HEAP_SIZE);
}

static void menu_toggle_cb(lv_event_t* e) {
	if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
		scrn_main_t *s = lv_event_get_user_data(e);
		if (lv_obj_has_flag(s->menu->menu_obj, LV_OBJ_FLAG_HIDDEN)){
			lv_obj_clear_flag(s->menu->menu_obj, LV_OBJ_FLAG_HIDDEN);
		} else {
			lv_obj_add_flag(s->menu->menu_obj, LV_OBJ_FLAG_HIDDEN);

		}
	}
}

static void menu_to_splash_cb(lv_event_t* e) {
	if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
		scrn_manager_switch((scrn_t*)create_screen_load());
	}
}

static void menu_to_cali_cb(lv_event_t* e) {
	if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
		scrn_manager_switch((scrn_t*)create_screen_cali());
	}
}

static void menu_to_settings_cb(lv_event_t* e) {
	(void)e; // not yet implemented
}

static void btnm_draw_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
		if(dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
            /*Change the draw descriptor of the 2nd button*/
            dsc->rect_dsc->bg_opa = LV_OPA_30;
		}
	}
}

static void scrn_main_t_enter(scrn_t* self){
	scrn_main_t *main = (scrn_main_t *)self;
// screen ----------------------------------------------------------------
	main->scrn_base.screen = lv_obj_create(NULL);
	lv_obj_set_pos(main->scrn_base.screen, 0, 0);
	lv_obj_set_size(main->scrn_base.screen, HOR_RESOLUTION, VER_RESOLUTION);
	lv_obj_clear_flag(main->scrn_base.screen, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_event_cb(main->scrn_base.screen, touch_cb, LV_EVENT_ALL, main);
	

	// apps mgr
	start_app_mgr();

	// tab
	main->app_tab_obj = create_tabview(main->scrn_base.screen, 0, 0, HOR_RESOLUTION, VER_RESOLUTION-20, 30);
	set_app_tv_inst(main->app_tab_obj);
	tab_t* tmp = create_tab(main->app_tab_obj, "App1");
	create_tab(main->app_tab_obj, "App2");
	create_tab(main->app_tab_obj, "App3");
	while (NULL != tmp)
	{
		hide_tab(main->app_tab_obj, tmp);
		tmp = tmp->next_tab;
	}
	
	// app icons, btnm
	main->icons_obj = create_icons(main->app_tab_obj->tab_head->tab);
		

	// header bar ----------------------------------------------------------------
	main->header_obj = lv_obj_create(main->scrn_base.screen);
	lv_obj_set_pos(main->header_obj, 0, 0);
	lv_obj_set_size(main->header_obj, LV_SIZE_CONTENT, 30);
	lv_obj_clear_flag(main->header_obj, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_set_layout(main->header_obj, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(main->header_obj, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_pad_column(main->header_obj, 3, 0); // spacing between items
	lv_obj_set_style_pad_all(main->header_obj, 0, 0);	//	MUST SET 0!! Inner margin of the container. on all 4 sides to children
	lv_obj_set_style_radius(main->header_obj, 0, 0);
	lv_obj_set_style_bg_opa(main->header_obj, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(main->header_obj, 0, 0);
	lv_obj_add_flag(main->header_obj, LV_OBJ_FLAG_FLOATING);
	lv_obj_move_foreground(main->header_obj);
	lv_obj_add_flag(main->header_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	
	// menu button
	main->menu_b_obj = lv_btn_create(main->header_obj);
	lv_obj_set_size(main->menu_b_obj , LV_SIZE_CONTENT, 30);
	lv_obj_add_style(main->menu_b_obj,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(main->menu_b_obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_event_cb(main->menu_b_obj, menu_toggle_cb, LV_EVENT_ALL, main);
	lv_obj_add_flag(main->menu_b_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	// menu label
	main->menu_l_obj = lv_label_create(main->menu_b_obj);
	lv_label_set_text(main->menu_l_obj,"\xEF\x83\x89"); // f0c9
	lv_obj_set_pos(main->menu_l_obj, 0, 0);
	lv_obj_set_size(main->menu_l_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_align(main->menu_l_obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->menu_l_obj, &lv_app_styles.sym_font, LV_PART_MAIN | LV_STATE_DEFAULT);	// explicit, otherwise overwritten
	lv_obj_add_flag(main->menu_l_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// menu (using menu lib)
	main->menu = create_menu(main->scrn_base.screen, 0, 30, 300, 100+6/*+6 for border*/, 50);
	lv_obj_add_flag(main->menu->menu_obj, LV_OBJ_FLAG_HIDDEN);

		// root page: 1 section with 3 contents (splash, calibrate, settings)
		page_t    *root_page     = create_page(main->menu, NULL, 1);page_add_border(root_page);
		section_t *main_section  = create_section(root_page, 3);
		
			// splash
			cont_t *splash_cont = create_content(main_section, 30);
			create_cont_lbl(splash_cont, "splash");

				// splash subpage
				page_t    *splash_page    = create_page(main->menu, splash_cont, 1);page_add_border(splash_page);
				section_t *splash_section = create_section(splash_page, 2);
				cont_t    *splash_item    = create_content(splash_section, 30);
				create_cont_lbl(splash_item, "go to splash screen");
				cont_t    *splash_btn_item    = create_content(splash_section, 60);
				create_cont_btn(splash_btn_item, "GO", menu_to_splash_cb, main);

			// calibrate
			cont_t *calibrate_cont = create_content(main_section, 30);
			create_cont_lbl(calibrate_cont, "calibrate");
			// lv_obj_add_event_cb(calibrate_cont->cont_obj, menu_to_cali_cb, LV_EVENT_ALL, main);

				// calibrate subpage
				page_t    *calibrate_page    = create_page(main->menu, calibrate_cont, 1);page_add_border(calibrate_page);
				section_t *calibrate_section = create_section(calibrate_page, 2);
				cont_t    *calibrate_item    = create_content(calibrate_section, 30);
				create_cont_lbl(calibrate_item, "go to calibration screen");
				cont_t    *calibrate_btn_item    = create_content(calibrate_section, 60);
				create_cont_btn(calibrate_btn_item, "GO", menu_to_cali_cb, main);

			// settings (with subpage)
			cont_t *settings_cont = create_content(main_section, 30);
			create_cont_lbl(settings_cont, "settings");
			// lv_obj_add_event_cb(settings_cont->cont_obj, menu_to_settings_cb, LV_EVENT_ALL, main);

				// settings subpage
				page_t    *settings_page    = create_page(main->menu, settings_cont, 1);page_add_border(settings_page);
				section_t *settings_section = create_section(settings_page, 2);
				cont_t    *settings_item    = create_content(settings_section, 30);
				create_cont_lbl(settings_item, "go to settings app");
				cont_t    *settings_btn_item    = create_content(settings_section, 60);
				create_cont_btn(settings_btn_item, "GO", menu_to_settings_cb, main);


	// folder
	main->folder_obj = lv_btn_create(main->header_obj);
	lv_obj_set_size(main->folder_obj , LV_SIZE_CONTENT, 30);
	lv_obj_add_style(main->folder_obj,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(main->folder_obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->folder_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	// folder label
	main->folder_l_obj = lv_label_create(main->folder_obj);
	lv_label_set_text(main->folder_l_obj,"\xEF\x81\xBC");
	lv_obj_set_pos(main->folder_l_obj, 0, 0);
	lv_obj_set_size(main->folder_l_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_align(main->folder_l_obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->folder_l_obj, &lv_app_styles.sym_font, LV_PART_MAIN | LV_STATE_DEFAULT);	// explicit, otherwise overwritten
	lv_obj_add_flag(main->folder_l_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// terminal
	main->terminal_obj = lv_btn_create(main->header_obj);
	lv_obj_set_size(main->terminal_obj , LV_SIZE_CONTENT, 30);
	lv_obj_add_style(main->terminal_obj,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_radius(main->terminal_obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->terminal_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	// terminal label
	main->terminal_l_obj = lv_label_create(main->terminal_obj);
	// lv_label_set_text(main->terminal_l_obj,"\xEF\x8B\x90"); //f2d0
	lv_label_set_text(main->terminal_l_obj,">_");
	lv_obj_set_pos(main->terminal_l_obj, 0, 0);
	lv_obj_set_size(main->terminal_l_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_set_style_align(main->terminal_l_obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
	// lv_obj_add_style(main->terminal_l_obj, &lv_app_styles.sym_font, LV_PART_MAIN | LV_STATE_DEFAULT);	// explicit, otherwise overwritten
	lv_obj_add_style(main->terminal_l_obj, &lv_app_styles.char_color1, LV_PART_MAIN | LV_STATE_DEFAULT);	// explicit, otherwise overwritten
	lv_obj_add_flag(main->terminal_l_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
	//	footer/status bar obj ----------------------------------------------------------------
	main->bar_obj = lv_obj_create(main->scrn_base.screen);
	lv_obj_set_pos(main->bar_obj, 0, 300);
	lv_obj_set_size(main->bar_obj, HOR_RESOLUTION, 20);
	lv_obj_clear_flag(main->bar_obj, LV_OBJ_FLAG_SCROLLABLE);
	
	lv_obj_set_layout(main->bar_obj, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(main->bar_obj, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_pad_column(main->bar_obj, 1, 0); // spacing between items
	lv_obj_set_style_pad_all(main->bar_obj, 0, 0);	//	MUST SET 0!! Inner margin of the container. on all 4 sides to children
	lv_obj_set_style_radius(main->bar_obj, 0, 0);
	lv_obj_set_style_bg_opa(main->bar_obj, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(main->bar_obj, 0, 0);
	lv_obj_add_flag(main->bar_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

	//	return status label obj
	main->ret_obj = lv_label_create(main->bar_obj);
	lv_label_set_text(main->ret_obj , "[RET:");
	lv_obj_add_style(main->ret_obj ,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->ret_obj , LV_OBJ_FLAG_EVENT_BUBBLE);
	
	//	ret status content obj
	main->ret_val_obj = lv_label_create(main->bar_obj);
	lv_label_set_text(main->ret_val_obj, "NONE");
	lv_obj_add_style(main->ret_val_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->ret_val_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	//	cursor pos label obj
	main->scrn_base.pos_obj = lv_label_create(main->bar_obj);
	lv_label_set_text(main->scrn_base.pos_obj, "][Pos:   ,   ");
	lv_obj_add_style(main->scrn_base.pos_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->scrn_base.pos_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	//	cursor pos val X label obj
	main->scrn_base.posX_obj = lv_label_create(main->scrn_base.pos_obj);
	// lv_cursor_pos.label_x=main->scrn_base.posX_obj;
	lv_label_set_text(main->scrn_base.posX_obj, "___");
	lv_obj_set_pos(main->scrn_base.posX_obj, 60, 0);
	lv_obj_add_style(main->scrn_base.posX_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->scrn_base.posX_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	//	cursor pos val Y label obj
	main->scrn_base.posY_obj = lv_label_create(main->scrn_base.pos_obj);
	// lv_cursor_pos.label_y=main->scrn_base.posY_obj;
	lv_label_set_text(main->scrn_base.posY_obj, "___");
	lv_obj_set_pos(main->scrn_base.posY_obj, 100, 0);
	lv_obj_add_style(main->scrn_base.posY_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->scrn_base.posY_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	//	ucHeap label obj
	main->ucHeap_l_obj = lv_label_create(main->bar_obj);
	lv_label_set_text(main->ucHeap_l_obj, "][RAM: __%");
	lv_obj_add_style(main->ucHeap_l_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->ucHeap_l_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	main->ucHeap_timer = lv_timer_create(heap_label_cb, 1000, main->ucHeap_l_obj);

	//	tray obj
	main->tray_obj = lv_obj_create(main->bar_obj);
	lv_obj_set_flex_grow(main->tray_obj, 1);	//	MUST HAVE: expand to the end!!!
//		lv_obj_set_pos(tray_obj, 0, 300);
	lv_obj_set_size(main->tray_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);	//	MUST HAVE
	lv_obj_clear_flag(main->tray_obj, LV_OBJ_FLAG_SCROLLABLE);
	
	lv_obj_set_layout(main->tray_obj, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(main->tray_obj, LV_FLEX_FLOW_ROW_REVERSE);
	lv_obj_set_style_pad_column(main->tray_obj, 5, 0); // spacing between items
	lv_obj_set_style_pad_all(main->tray_obj, 0, 0);
	lv_obj_set_style_radius(main->tray_obj, 0, 0);
	lv_obj_set_style_border_width(main->tray_obj, 0, 0);
	lv_obj_set_style_bg_opa(main->tray_obj, LV_OPA_TRANSP, 0);

	lv_obj_set_flex_align(main->tray_obj,	//	MUST HAVE
					LV_FLEX_ALIGN_END,
					LV_FLEX_ALIGN_CENTER,
					LV_FLEX_ALIGN_CENTER);
										
	lv_obj_add_flag(main->tray_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// ]
	main->end_obj = lv_label_create(main->tray_obj);
	lv_label_set_text(main->end_obj,"]");
	lv_obj_add_style(main->end_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->end_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// wifi sym
	main->wifi_obj = lv_label_create(main->tray_obj);
	lv_label_set_text(main->wifi_obj,LV_SYMBOL_WIFI);	// cant use any char here, only syms since set by style
	lv_obj_add_style(main->wifi_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->wifi_obj,&lv_app_styles.sym_font,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->wifi_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// wifi disconn slash
	main->wifix_obj = lv_label_create(main->wifi_obj);
	lv_label_set_text(main->wifix_obj,"\xEF\x9C\x95");
	lv_obj_add_style(main->wifix_obj,&lv_app_styles.char_color2, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->wifix_obj, &lv_app_styles.sym_font, LV_PART_MAIN | LV_STATE_DEFAULT);	// explicit, otherwise overwritten
	lv_obj_add_flag(main->wifix_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// sound sym
	main->sound_obj = lv_label_create(main->tray_obj);
	lv_label_set_text(main->sound_obj, LV_SYMBOL_MUTE);
	lv_obj_add_style(main->sound_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->sound_obj,&lv_app_styles.sym_font,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->sound_obj , LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// sound disconn slash
	main->soundx_obj = lv_label_create(main->sound_obj);
	lv_label_set_text(main->soundx_obj,"\xEF\x9C\x95");
	lv_obj_add_style(main->soundx_obj,&lv_app_styles.char_color2, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->soundx_obj, &lv_app_styles.sym_font, LV_PART_MAIN | LV_STATE_DEFAULT);	// explicit, otherwise overwritten
	lv_obj_add_flag(main->soundx_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// battery
	main->battery_obj = lv_label_create(main->tray_obj);
	lv_label_set_text(main->battery_obj, LV_SYMBOL_BATTERY_EMPTY);
	lv_obj_add_style(main->battery_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->battery_obj,&lv_app_styles.sym_font,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->battery_obj , LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// battery disconn slash
	main->batteryx_obj = lv_label_create(main->battery_obj);
	lv_label_set_text(main->batteryx_obj,"\xEF\x9C\x95");
	lv_obj_add_style(main->batteryx_obj,&lv_app_styles.char_color2, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->batteryx_obj, &lv_app_styles.sym_font, LV_PART_MAIN | LV_STATE_DEFAULT);	// explicit, otherwise overwritten
	lv_obj_add_flag(main->batteryx_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	//	SD
	main->SD_obj = lv_label_create(main->tray_obj);
	lv_label_set_text(main->SD_obj, "\xEF\x9F\x82");
	lv_obj_add_style(main->SD_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->SD_obj,&lv_app_styles.sym_font,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->SD_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// SD disconn slash
	main->SDx_obj = lv_label_create(main->SD_obj);
	lv_label_set_text(main->SDx_obj,"\xEF\x9C\x95");
	lv_obj_add_style(main->SDx_obj,&lv_app_styles.char_color2, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(main->SDx_obj, &lv_app_styles.sym_font, LV_PART_MAIN | LV_STATE_DEFAULT);	// explicit, otherwise overwritten
	lv_obj_add_flag(main->SDx_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	// ][
	main->sta_obj = lv_label_create(main->tray_obj);
	lv_label_set_text(main->sta_obj,"][");
	lv_obj_add_style(main->sta_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(main->sta_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
	printf("Free heap main: %d\r\n", xPortGetFreeHeapSize());
	lv_disp_load_scr(main->scrn_base.screen);
}

static void scrn_main_t_exit(scrn_t* self)
{
    scrn_main_t *main = (scrn_main_t *)self;
    // free owned resources before LVGL tree is destroyed
	lv_timer_del(main->ucHeap_timer);    // ADD - kill timer before objects are deleted
    main->ucHeap_timer = NULL;    
    free_menu(main->menu);
	free_icons(main->icons_obj);
    free_tabview(main->app_tab_obj);set_app_tv_inst(NULL);
    scrn_t_exit_t(self);
}

static scrn_main_t* create_screen_main(void){
	scrn_main_t *main = lv_mem_alloc(sizeof(scrn_main_t));
	if(main == NULL){
		printf("Failed to allocate memory for scrn_main_t\r\n");
		return NULL;
	}
	lv_memset_00(main, sizeof(scrn_main_t));
	main->scrn_base.scrn_t_enter = scrn_main_t_enter;
	main->scrn_base.scrn_t_exit = scrn_main_t_exit;
	return main;
}
