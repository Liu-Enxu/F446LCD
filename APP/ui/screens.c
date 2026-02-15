#include <string.h>

#include "screens.h"
#include "lv_app_conf.h"

#include "FreeRTOS.h"
#include "task.h"

#include "LCD.h"

static void touch_load_cb(lv_event_t* e){
	lv_event_code_t code = lv_event_get_code(e);
	scrn_load_t *load_scrn = lv_event_get_user_data(e);
	if(load_scrn->obj5 != NULL && (code == LV_EVENT_PRESSED || code==LV_EVENT_RELEASED)){
//		printf("RELEASED\n");
		lv_label_set_text_fmt(load_scrn->obj5, "%u\n%u", lv_cursor_pos.cursor_x,lv_cursor_pos.cursor_y);
	}
}

static lv_obj_t* root;

static void button_pressed_load_cb(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	scrn_load_t* load_scrn= lv_event_get_user_data(e);
//	if(code == LV_EVENT_PRESSED){
//		printf("PRESSED");
//	}else 
	if (code == LV_EVENT_RELEASED){
//		printf("RELEASED");
		lv_obj_clean(root);	//	lv_obj_del should del all children...
		create_screen_main();
		lv_mem_free(load_scrn);	// free memory explicitly
	}
}

void create_screen(){
	root = lv_obj_create(NULL);
	lv_obj_set_pos(root, 0, 0);
	lv_obj_set_size(root, HOR_RESOLUTION, VER_RESOLUTION);
	lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
//    lv_obj_set_style_border_opa(obj, LV_OPA_TRANSP, 0);	// optional??
	lv_obj_add_event_cb(root, touch_load_cb, LV_EVENT_ALL, root);
}

void create_screen_load() {
		scrn_load_t *load_scrn = lv_mem_alloc(sizeof(scrn_load_t));
		lv_memset_00(load_scrn, sizeof(scrn_load_t));
		// 	screen obj
    load_scrn->load = lv_obj_create(root);
    lv_obj_set_pos(load_scrn->load, 0, 0);
    lv_obj_set_size(load_scrn->load, HOR_RESOLUTION, VER_RESOLUTION);
    lv_obj_clear_flag(load_scrn->load, LV_OBJ_FLAG_SCROLLABLE);
//    lv_obj_set_style_border_opa(obj, LV_OPA_TRANSP, 0);	// optional??
		lv_obj_add_event_cb(load_scrn->load, touch_load_cb, LV_EVENT_ALL, load_scrn);
		
		// 	label obj
		lv_obj_t *L_obj = lv_label_create(load_scrn->load);
		load_scrn->obj0 = L_obj;
		lv_obj_set_pos(L_obj, 480, 10);
		lv_obj_set_size(L_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_label_set_text(L_obj, " __     __   __  __ _ \n(  )   / _\\ (  )(  ( \\\n/ (_/\\/    \\ )( /    /\n\\____/\\_/\\_/(__)\\_)__)\n  __   ____ \n /  \\ / ___)	V0.2\n(  O )\\___ \\\n \\__/ (____/");
		lv_obj_set_style_transform_angle(L_obj, 900, LV_PART_MAIN | LV_STATE_DEFAULT);	
		lv_obj_add_style(L_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(L_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

		//	button obj
		lv_obj_t *b_obj = lv_btn_create(obj);
		load_scrn->obj1 = b_obj;
		lv_obj_set_pos(b_obj , 330, 240);
		lv_obj_set_size(b_obj , 100, 50);
		lv_obj_add_style(b_obj,&lv_app_styles.color_combo,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(b_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		lv_obj_add_event_cb(b_obj, button_pressed_load_cb, LV_EVENT_ALL, load_scrn);
		
		//	label obj on button obj
		lv_obj_t *l_obj = lv_label_create(b_obj);
		load_scrn->obj2 = l_obj;
		lv_obj_set_pos(l_obj, 0, 0);
		lv_obj_set_size(l_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_align(l_obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_label_set_text(l_obj, "MAIN\nMENU");
		
		// 	ascii img obj
		lv_obj_t *ascii_obj = lv_label_create(obj);
		load_scrn->obj3 = ascii_obj;
		lv_obj_set_pos(ascii_obj, 10, 0);
		lv_obj_set_size(ascii_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_label_set_text(ascii_obj, "         @@@@@@@@@@@     \n      @@@@@@@@@@@@@@@@   \n     @@@@@@@@@@@@@@@@@@@ \n    @@@@@@@@@@@@@@@@@@@@@\n    @@@@@@#%@%+=-=#%@@@@@\n   @@@@@@#=-:-==:-#*%%@@@\n    @@*%#::=*%+:::::-#%@@\n    @@-::::::::::::#%*@@@\n    @@@*:::::::::-::-*   \n     @@*-:::::------+%   \n    %=   ---------=#@@   \n   %%%%%%#***+++*% @@@   \n  %%%%%%%%%%@@@@   @@@   \n%%%%%%%%%%%%%%%%  @@@    ");
		lv_obj_add_style(ascii_obj,&lv_app_styles.char_color2,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(ascii_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
		//	pointer position obj
		lv_obj_t *pos_obj = lv_label_create(obj);
		load_scrn->obj4 = pos_obj;
		lv_obj_set_pos(pos_obj, 240, 170);
		lv_obj_set_size(pos_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_label_set_text(pos_obj, "[Cursor]\n[X:    ]\n[Y:    ]");
		lv_obj_add_style(pos_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(pos_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
		//	pos val
		lv_obj_t *val_obj = lv_label_create(pos_obj);
		load_scrn->obj5 = val_obj;
		lv_obj_set_pos(val_obj, 40, 20);
		lv_obj_set_size(val_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_label_set_text(val_obj, "_  \n_  ");
		lv_obj_add_style(val_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(val_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
		lv_disp_load_scr(obj);
}

static void touch_main_cb(lv_event_t* e){
	lv_event_code_t code = lv_event_get_code(e);
	scrn_main_t *main_scrn = lv_event_get_user_data(e);
	if(main_scrn->obj5 != NULL && (code == LV_EVENT_PRESSED || code==LV_EVENT_RELEASED)){
//		printf("RELEASED\n");
		lv_label_set_text_fmt(main_scrn->obj5, "%u,%u][", lv_cursor_pos.cursor_x,lv_cursor_pos.cursor_y);
	}
}

void create_screen_main(void){
		scrn_main_t *main_scrn = lv_mem_alloc(sizeof(scrn_main_t));
		lv_memset_00(main_scrn, sizeof(scrn_main_t));
		// 	screen obj
    lv_obj_t *obj = lv_obj_create(NULL);
    main_scrn->main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, HOR_RESOLUTION, VER_RESOLUTION);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_add_event_cb(obj, touch_main_cb, LV_EVENT_ALL, main_scrn);
		
		//	status bar obj
		lv_obj_t *bar_obj = lv_obj_create(obj);
		main_scrn->obj2 = bar_obj;
		lv_obj_set_pos(bar_obj, 0, 300);
		lv_obj_set_size(bar_obj, HOR_RESOLUTION, 20);
		lv_obj_clear_flag(bar_obj, LV_OBJ_FLAG_SCROLLABLE);
		
		lv_obj_set_layout(bar_obj, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(bar_obj, LV_FLEX_FLOW_ROW);
		lv_obj_set_style_pad_column(bar_obj, 10, 0); // spacing between items
		lv_obj_set_style_pad_all(bar_obj, 0, 0);	//	MUST SET 0!! Inner margin of the container. on all 4 sides to children
		lv_obj_set_style_radius(bar_obj, 0, 0);
		lv_obj_set_style_bg_opa(bar_obj, LV_OPA_TRANSP, 0);
		lv_obj_set_style_border_width(bar_obj, 0, 0);
		lv_obj_add_flag(bar_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	
		//	Fatfs status label obj
		lv_obj_t *ff_status_obj = lv_label_create(bar_obj);
		main_scrn->obj3 = ff_status_obj ;
		lv_label_set_text(ff_status_obj , "[FSret:");
		lv_obj_add_style(ff_status_obj ,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(ff_status_obj , LV_OBJ_FLAG_EVENT_BUBBLE);
		
		//	Fatfs status content obj
		lv_obj_t *ff_status_val_obj = lv_label_create(bar_obj);
		main_scrn->obj4 = ff_status_val_obj;
		lv_label_set_text(ff_status_val_obj, "NONE");
		lv_obj_add_style(ff_status_val_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(ff_status_val_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
		//	cursor pos label obj
		lv_obj_t *cursor_obj = lv_label_create(bar_obj);
		main_scrn->obj5 = cursor_obj;
		lv_label_set_text(cursor_obj, "][Pos:");
		lv_obj_add_style(cursor_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(cursor_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
		//	cursor pos val label obj
		lv_obj_t *cursor_val_obj = lv_label_create(bar_obj);
		main_scrn->obj5 = cursor_val_obj;
		lv_label_set_text(cursor_val_obj, "___,___][");
		lv_obj_add_style(cursor_val_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(cursor_val_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
		//	tray obj
		lv_obj_t *tray_obj = lv_obj_create(bar_obj);
		main_scrn->obj6 = tray_obj;
		lv_obj_set_flex_grow(tray_obj, 1);	//	MUST HAVE: expand to the end!!!
//		lv_obj_set_pos(tray_obj, 0, 300);
		lv_obj_set_size(tray_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);	//	MUST HAVE
		lv_obj_clear_flag(tray_obj, LV_OBJ_FLAG_SCROLLABLE);
		
		lv_obj_set_layout(tray_obj, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(tray_obj, LV_FLEX_FLOW_ROW_REVERSE);
		lv_obj_set_style_pad_column(tray_obj, 5, 0); // spacing between items
		lv_obj_set_style_pad_all(tray_obj, 0, 0);
		lv_obj_set_style_radius(tray_obj, 0, 0);
		lv_obj_set_style_border_width(tray_obj, 0, 0);
		lv_obj_set_style_bg_opa(tray_obj, LV_OPA_TRANSP, 0);

		lv_obj_set_flex_align(tray_obj,	//	MUST HAVE
                      LV_FLEX_ALIGN_END,
                      LV_FLEX_ALIGN_CENTER,
                      LV_FLEX_ALIGN_CENTER);
											
		lv_obj_add_flag(tray_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
		// ]
		lv_obj_t *end_obj = lv_label_create(tray_obj);
		main_scrn->obj7 = end_obj;
		lv_label_set_text(end_obj,"]");
		lv_obj_add_style(end_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(end_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
		// wifi sym
		lv_obj_t *wifi_obj = lv_label_create(tray_obj);
		main_scrn->obj8 = wifi_obj;
		lv_label_set_text(wifi_obj,LV_SYMBOL_WIFI);	// cant use any char here, only syms since set by style
		lv_obj_add_style(wifi_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_style(wifi_obj,&lv_app_styles.sym_font,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(wifi_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
		// wifi disconn cross
		lv_obj_t *cross_obj = lv_label_create(wifi_obj);
		main_scrn->obj9 = cross_obj;
		lv_label_set_text(cross_obj,"\xEF\x9C\x95");
		lv_obj_add_style(cross_obj,&lv_app_styles.char_color2, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_style(cross_obj, &lv_app_styles.sym_font, LV_PART_MAIN | LV_STATE_DEFAULT);	// explicit, otherwise overwritten
		lv_obj_add_flag(cross_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
		
		// sound sym
		lv_obj_t *sound_obj = lv_label_create(tray_obj);
		main_scrn->obj10 = sound_obj;
		lv_label_set_text(sound_obj, LV_SYMBOL_MUTE);
		lv_obj_add_style(sound_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_style(sound_obj,&lv_app_styles.sym_font,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(sound_obj , LV_OBJ_FLAG_EVENT_BUBBLE);
		
		// battery
		lv_obj_t *battery_obj = lv_label_create(tray_obj);
		main_scrn->obj11 = battery_obj;
		lv_label_set_text(battery_obj, LV_SYMBOL_BATTERY_EMPTY);
		lv_obj_add_style(battery_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_style(battery_obj,&lv_app_styles.sym_font,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(battery_obj , LV_OBJ_FLAG_EVENT_BUBBLE);
		
		//	SD
		lv_obj_t *SD_obj = lv_label_create(tray_obj);
		main_scrn->obj12 = SD_obj;
		lv_label_set_text(SD_obj, "\xEF\x84\xB3");
		lv_obj_add_style(SD_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_style(SD_obj,&lv_app_styles.sym_font,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(SD_obj , LV_OBJ_FLAG_EVENT_BUBBLE);
		
		//	calendar
		lv_obj_t *calendar_obj = lv_label_create(tray_obj);
		main_scrn->obj12 = calendar_obj;
		lv_label_set_text(calendar_obj, "\xEF\x84\xB3");
		lv_obj_add_style(calendar_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_style(calendar_obj,&lv_app_styles.sym_font,LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_add_flag(calendar_obj , LV_OBJ_FLAG_EVENT_BUBBLE);

		lv_disp_load_scr(obj);
		
}
