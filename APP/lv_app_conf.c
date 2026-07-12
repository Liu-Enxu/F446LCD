#include "lv_app_conf.h"

lv_app_styles_t lv_app_styles;
lv_cursor_pos_t lv_cursor_pos;
calibration_param_t calibrat_t = {0};

peri_status_t peri_status = {0};
disp_status_t disp_status = {0};

uint32_t LV_NOTI_NEW;

void lv_app_init(void){
	// color
	lv_app_styles.color1 = lv_color_hex(0xffc4a656);
	lv_app_styles.color2 = lv_color_hex(0xffa83450);
	// color
	lv_style_init(&lv_app_styles.char_color1);
	lv_style_set_text_color(&lv_app_styles.char_color1, lv_app_styles.color1);
	lv_style_set_text_font(&lv_app_styles.char_color1,&fixedsys);
	
	lv_style_init(&lv_app_styles.char_color2);
	lv_style_set_text_color(&lv_app_styles.char_color2, lv_app_styles.color2);
	lv_style_set_text_font(&lv_app_styles.char_color2,&fixedsys);
	
	lv_style_init(&lv_app_styles.color_combo1);
	lv_style_set_bg_color(&lv_app_styles.color_combo1, lv_app_styles.color2);
	lv_style_set_text_color(&lv_app_styles.color_combo1, lv_app_styles.color1);
	lv_style_set_text_align(&lv_app_styles.color_combo1, LV_TEXT_ALIGN_CENTER);

	lv_style_init(&lv_app_styles.color_combo2);
	lv_style_set_bg_color(&lv_app_styles.color_combo2, lv_app_styles.color1);
	lv_style_set_text_color(&lv_app_styles.color_combo2, lv_app_styles.color2);
	lv_style_set_text_align(&lv_app_styles.color_combo2, LV_TEXT_ALIGN_CENTER);
	
	// font
	lv_style_init(&lv_app_styles.sym_font);
	lv_style_set_text_font(&lv_app_styles.sym_font,&awesomesyms);
	
	lv_style_init(&lv_app_styles.txt_font);
	lv_style_set_text_font(&lv_app_styles.txt_font,&fixedsys);
	
	// common styles
	lv_style_init(&lv_app_styles.no_deco);
	lv_style_set_bg_opa(&lv_app_styles.no_deco, LV_OPA_TRANSP);
	lv_style_set_border_width(&lv_app_styles.no_deco, 0);
	lv_style_set_radius(&lv_app_styles.no_deco, 0);
	lv_style_set_pad_all(&lv_app_styles.no_deco, 0);

	lv_style_init(&lv_app_styles.btnm_main);
	lv_style_set_bg_opa(&lv_app_styles.btnm_main, LV_OPA_TRANSP);
	lv_style_set_border_width(&lv_app_styles.btnm_main, 0);

	lv_style_init(&lv_app_styles.btn_lbl);
	lv_style_set_align(&lv_app_styles.btn_lbl, LV_ALIGN_CENTER);

	/* Fixed-height-list button: equal share, centered content, app colors/font. */
	lv_style_init(&lv_app_styles.list_btn);
	lv_style_set_flex_main_place(&lv_app_styles.list_btn, LV_FLEX_ALIGN_CENTER);
	lv_style_set_flex_cross_place(&lv_app_styles.list_btn, LV_FLEX_ALIGN_CENTER);
	lv_style_set_flex_track_place(&lv_app_styles.list_btn, LV_FLEX_ALIGN_CENTER);
	lv_style_set_bg_color(&lv_app_styles.list_btn, lv_app_styles.color2);
	lv_style_set_text_color(&lv_app_styles.list_btn, lv_app_styles.color1);
	lv_style_set_text_font(&lv_app_styles.list_btn, &awesomesyms);

	/* Cancel the default theme's list-item growth while pressed. */
	lv_style_init(&lv_app_styles.list_btn_pressed);
	lv_style_set_transform_width(&lv_app_styles.list_btn_pressed, 0);
	lv_style_set_transform_height(&lv_app_styles.list_btn_pressed, 0);

	lv_style_init(&lv_app_styles.tab_item_default);
	lv_style_set_bg_opa(&lv_app_styles.tab_item_default, LV_OPA_10);
	lv_style_set_border_opa(&lv_app_styles.tab_item_default, LV_OPA_TRANSP);

	lv_style_init(&lv_app_styles.tab_item_checked);
	lv_style_set_border_opa(&lv_app_styles.tab_item_checked, LV_OPA_COVER);
	lv_style_set_border_color(&lv_app_styles.tab_item_checked, lv_app_styles.color1);

	lv_style_init(&lv_app_styles.btn_disabled);
	lv_style_set_bg_opa(&lv_app_styles.btn_disabled, LV_OPA_30);
	
	// event
	LV_NOTI_NEW = lv_event_register_id();
	
}
	
