#include "lv_app_conf.h"

lv_app_styles_t lv_app_styles;
lv_cursor_pos_t lv_cursor_pos;
calibration_param_t calibrat_t = {0};

peri_status_t peri_status = {0};
header_status_t header_status = {0};

uint32_t LV_NOTI_NEW;

void lv_app_init(void){
	// color
	lv_app_styles.color1 = lv_color_hex(0xffc4a656);
	lv_app_styles.color2 = lv_color_hex(0xffa83450);
	// styles
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
	
	lv_style_init(&lv_app_styles.sym_font);
	lv_style_set_text_font(&lv_app_styles.sym_font,&awesomesyms);
	
	lv_style_init(&lv_app_styles.txt_font);
	lv_style_set_text_font(&lv_app_styles.txt_font,&fixedsys);
	
//	lv_draw_polygon
	
	// event
	LV_NOTI_NEW = lv_event_register_id();
	
}
	
	
	
	