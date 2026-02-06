#ifndef __LV_APP_CONF_H
#define __LV_APP_CONF_H
#include <lvgl/lvgl.h>
#include "FreeRTOS.h"

typedef struct {
	lv_style_t char_color1;
	lv_style_t char_color2;
	lv_style_t color_combo; 
}lv_app_styles_t;
extern lv_app_styles_t lv_app_styles;

typedef struct {
	lv_coord_t cursor_x;
	lv_coord_t cursor_y;
}lv_cursor_pos_t;
extern lv_cursor_pos_t lv_cursor_pos;

void lv_app_init(void);


#endif
