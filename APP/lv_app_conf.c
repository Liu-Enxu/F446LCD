#include "lv_app_conf.h"

lv_app_styles_t lv_app_styles;
lv_cursor_pos_t lv_cursor_pos;

void lv_app_init(void){
	lv_style_init(&lv_app_styles.char_color1);
	lv_style_set_text_color(&lv_app_styles.char_color1, lv_color_hex(0xffc4a656));
	
	lv_style_init(&lv_app_styles.char_color2);
	lv_style_set_text_color(&lv_app_styles.char_color2, lv_color_hex(0xffa83450));
	
	lv_style_init(&lv_app_styles.color_combo);
	lv_style_set_bg_color(&lv_app_styles.color_combo, lv_color_hex(0xffa83450));
	lv_style_set_text_color(&lv_app_styles.color_combo, lv_color_hex(0xffc4a656));
	lv_style_set_text_align(&lv_app_styles.color_combo, LV_TEXT_ALIGN_CENTER);
	
}
	
	
	
	