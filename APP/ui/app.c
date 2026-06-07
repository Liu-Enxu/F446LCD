#include "app.h"

#define hor_res 480
#define ver_res 320

app_mgr_t* app_mgr_inst(void){
	static app_mgr_t app_mgr;
	if (!app_mgr.app_mgr_inited){
		app_mgr.app_registered = 0;
		app_mgr.app_head = NULL;
		app_mgr.app_mgr_inited = 1;
	}
	return &app_mgr;
}

void start_app_mgr(void){
	static u8 started = 0;
	if (!started){
		app_mgr_register((app_t*)create_settings_app());
		app_mgr_register((app_t*)create_calc_app());
		started = 1;
	} else {
		printf("App manager already started!\r\n");
	}
}

void app_mgr_register(app_t* my_app){
	if(my_app == NULL){
		printf("Cannot register NULL app!\r\n");
		return;
	}
	if(my_app->is_registered){
		printf("App already registered!\r\n");
		return;
	}
	my_app->is_registered = 1;
	(app_mgr_inst()->app_registered)++;

	app_t* tmp = app_mgr_inst()->app_head;
	if(NULL == tmp) {
		app_mgr_inst()->app_head = my_app;
		return;
	}
	while(NULL != tmp->next_app){
		tmp = tmp->next_app;
	}
	tmp->next_app = my_app;
	
}

void app_mgr_unreg(app_t* my_app){
	// find app
	app_t* tmp = app_mgr_inst()->app_head;
	app_t* prev = NULL;
	while(my_app != tmp){
		prev = tmp;
		tmp = tmp->next_app;
	}
	// list remove
	if(NULL == prev){
		app_mgr_inst()->app_head = tmp->next_app;
	} else {
		prev->next_app = tmp->next_app;
	}
	tmp->next_app = NULL;
}













static void app_t_exit_t(app_t* self){

}


static void settings_app_load(app_t* self, lv_obj_t* parent){
	settings_app_t* settings = (settings_app_t*)self;

	// test settings app
	settings->settings_menu = create_menu(parent, 0, 0, hor_res, ver_res-50, 30);
	lv_obj_set_style_bg_opa(settings->settings_menu->menu_obj, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(settings->settings_menu->menu_obj, lv_color_darken(lv_obj_get_style_bg_color(settings->settings_menu->menu_obj, 0), 20), 0);

		page_t    *settings_root_page     = create_page(settings->settings_menu, NULL, 1);
		section_t *settings_main_section  = create_section(settings_root_page, 3);
			cont_t *test1_cont = create_content(settings_main_section, 30);
			create_cont_lbl(test1_cont, "test1_cont");

			cont_t *test2_cont = create_content(settings_main_section, 30);
			create_cont_lbl(test2_cont, "test2_cont");

			cont_t *test3_cont = create_content(settings_main_section, 30);
			create_cont_lbl(test3_cont, "test3_cont");
				
			// splash subpage
				page_t    *test3_cont_page    = create_page(settings->settings_menu, test3_cont, 1);
				section_t *test3_cont_section = create_section(test3_cont_page, 2);
				cont_t    *test3_cont_item    = create_content(test3_cont_section, 30);
				create_cont_lbl(test3_cont_item, "go to splash screen");
				cont_t    *test3_cont_item2    = create_content(test3_cont_section, 60);
				create_cont_lbl(test3_cont_item2, "GO");
}

static void settings_app_exit(app_t* self){
	free_menu(((settings_app_t*)self)->settings_menu);
	// lv_mem_free((settings_app_t*)self);	// DON'T free self here, it's registered in app manager!!!
}

settings_app_t* create_settings_app(void){
	settings_app_t *settings = lv_mem_alloc(sizeof(settings_app_t));
	if(settings == NULL){
		printf("Failed to allocate memory for settings_app_t\r\n");
		return NULL;
	}
	lv_memset_00(settings, sizeof(settings_app_t));

	assert_param(strlen("Settings") < sizeof(settings->app_base.app_name)-1);	// ensure name fits in buffer
	assert_param(strlen("\xEF\x80\x93") < sizeof(settings->app_base.app_icon)-1);	// ensure icon fits in buffer
	strcpy(settings->app_base.app_name, "Settings");
	memcpy(settings->app_base.app_icon, "\xEF\x80\x93", sizeof("\xEF\x80\x93"));

	settings->app_base.app_t_load = settings_app_load;
	settings->app_base.app_t_exit = settings_app_exit;

	return settings;
}

static const char * btnm_map[] = {"(", ")", "C", "CE", "\n",
								"7", "8", "9", "/", "\n",
								"4", "5", "6", "*", "\n",
								"1", "2", "3", "-", "\n",
								"0", ".", "=", "+", ""};

static void calc_app_load(app_t* self, lv_obj_t* parent){
	calc_app_t* calc = (calc_app_t*)self;

	// calc->calc_btnm = lv_btnmatrix_create(parent);
	// lv_obj_add_style(calc->calc_btnm,&lv_app_styles.color_combo1,LV_PART_ITEMS | LV_STATE_DEFAULT);
    // lv_btnmatrix_set_map(calc->calc_btnm, calc->btnm_map);
	// lv_obj_set_size(calc->calc_btnm, HOR_RES-80, VER_RES-50);
	// lv_obj_align( calc->calc_btnm, LV_ALIGN_CENTER, 0, 0);

	// lv_obj_set_style_bg_opa( calc->calc_btnm, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
	// lv_obj_set_style_border_width( calc->calc_btnm, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	// lv_obj_add_event_cb( calc->calc_btnm, btnm_press_event_cb, LV_EVENT_VALUE_CHANGED, calc);
	// lv_obj_clear_flag( calc->calc_btnm, LV_OBJ_FLAG_SCROLLABLE);
	// lv_obj_add_flag( calc->calc_btnm, LV_OBJ_FLAG_EVENT_BUBBLE);
	// lv_obj_add_event_cb( calc->calc_btnm, btnm_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

	// last expression label
	lv_obj_t* last_expr = lv_label_create(parent);
	int val = (int)(cos(0.2) * 1000);  // shift decimal by 3 places
	lv_label_set_text_fmt(last_expr, "%d.%03d", val/1000, val%1000);
	lv_label_set_long_mode(last_expr, LV_LABEL_LONG_CLIP);
	
	lv_obj_set_size(last_expr, hor_res, LV_SIZE_CONTENT);   // width grows with content
	lv_obj_set_style_text_align(last_expr, LV_TEXT_ALIGN_RIGHT, 0);
	lv_obj_set_style_align(last_expr, LV_ALIGN_TOP_RIGHT, 0);
	lv_obj_set_y(last_expr, 5);

	lv_obj_add_style(last_expr,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(last_expr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(last_expr, LV_OBJ_FLAG_EVENT_BUBBLE);

	// current expression label
	lv_obj_t* curr_expr = lv_label_create(parent);
	val = (int)(cos(0.2) * 100);
	// lv_label_set_text_fmt(curr_expr, "%d.%02d", val/100, val%100);
	lv_label_set_text(curr_expr, "this is a very long text that should be clipped, let's see what happens");
	lv_label_set_long_mode(curr_expr, LV_LABEL_LONG_CLIP);
	
	lv_obj_set_size(curr_expr, hor_res, 20);   // width grows with content
	lv_obj_set_style_text_align(curr_expr, LV_TEXT_ALIGN_RIGHT, 0);
	lv_obj_set_style_align(curr_expr, LV_ALIGN_TOP_RIGHT, 0);
	lv_obj_set_y(curr_expr, 30);

	lv_obj_add_style(curr_expr,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(curr_expr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(curr_expr, LV_OBJ_FLAG_EVENT_BUBBLE);

	// snapshot, works really well but takes 7.5kb
	// lv_obj_t * curr_expr_img = lv_img_create(parent);
	// lv_img_dsc_t * snapshot = lv_snapshot_take(curr_expr, LV_IMG_CF_TRUE_COLOR_ALPHA);
	// lv_img_set_src(curr_expr_img, snapshot);
	// lv_obj_set_style_align(curr_expr_img, LV_ALIGN_TOP_RIGHT, 0);
	// lv_obj_set_y(curr_expr_img, 50);
	// lv_obj_set_style_transform_zoom(curr_expr_img, 512, LV_PART_MAIN | LV_STATE_DEFAULT);  // 2x zoom (256 = 1x)
	// lv_obj_set_style_transform_pivot_x(curr_expr_img, (snapshot->header.w), LV_PART_MAIN | LV_STATE_DEFAULT);
	// lv_obj_set_style_transform_pivot_y(curr_expr_img, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	// btnm
	lv_obj_t* btnm = lv_btnmatrix_create(parent);
	lv_obj_add_style(btnm,&lv_app_styles.color_combo1,LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_btnmatrix_set_map(btnm, btnm_map);
	lv_obj_set_size(btnm, hor_res-10, ver_res-50-40);
	lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 30);

	lv_obj_set_style_bg_opa( btnm, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width( btnm, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	// lv_obj_add_event_cb( btnm, btnm_press_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
	lv_obj_clear_flag( btnm, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag( btnm, LV_OBJ_FLAG_EVENT_BUBBLE);
	// lv_obj_add_event_cb( btnm, btnm_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

}

static void calc_app_exit(app_t* self){
	// TODO
}

calc_app_t* create_calc_app(void){
	calc_app_t *calc = lv_mem_alloc(sizeof(calc_app_t));
	if(calc == NULL){
		printf("Failed to allocate memory for calc_app_t\r\n");
		return NULL;
	}
	lv_memset_00(calc, sizeof(calc_app_t));

	assert_param(strlen("Calculator") < sizeof(calc->app_base.app_name)-1);	// ensure name fits in buffer
	assert_param(strlen("\xEF\x87\xAC") < sizeof(calc->app_base.app_icon)-1);	// ensure icon fits in buffer
	strcpy(calc->app_base.app_name, "Calculator");
	memcpy(calc->app_base.app_icon, "\xEF\x87\xAC", sizeof("\xEF\x87\xAC"));

	calc->app_base.app_t_load = calc_app_load;
	calc->app_base.app_t_exit = calc_app_exit;

	return calc;
}

