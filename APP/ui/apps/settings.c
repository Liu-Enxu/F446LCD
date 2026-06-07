#include "settings.h"

#define hor_res 480
#define ver_res 320

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

