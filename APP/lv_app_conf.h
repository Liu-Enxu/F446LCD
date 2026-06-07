#ifndef __LV_APP_CONF_H
#define __LV_APP_CONF_H
#include <lvgl/lvgl.h>
#include "FreeRTOS.h"
//#include "screens.h"

// system wide
typedef struct {
	// colors
	lv_color_t color1;
	lv_color_t color2;

	lv_style_t char_color1;
	lv_style_t char_color2;
	lv_style_t color_combo1;
	lv_style_t color_combo2;

	// fonts
	lv_style_t sym_font;	
	lv_style_t txt_font;

	// common styles
	lv_style_t no_deco;
	lv_style_t btnm_main;
	lv_style_t btn_lbl;
	lv_style_t tab_item_default;
	lv_style_t tab_item_checked;
	lv_style_t btn_disabled;

}lv_app_styles_t;
extern lv_app_styles_t lv_app_styles;

typedef struct {
	// raw ADC readings at each calibration point
	u16 tl_x; u16 tl_y;
	u16 tm_x; u16 tm_y;
	u16 tr_x; u16 tr_y;
	u16 bl_x; u16 bl_y;
	u16 bm_x; u16 bm_y;
	u16 br_x; u16 br_y;
	// calibration var for calculation
	u8  id;
	u8  cali_sub;
	u32 cali_acc_x;
	u32 cali_acc_y;
	// individual circle valid range
	u16 valid_x_min; u16 valid_x_max;
	u16 valid_y_min; u16 valid_y_max;
	u8 isValidLimSet; u8 isInvalid;
	// computed boundaries (averaged from the 6 points above)
	u16 adc_x_left;
	u16 adc_x_right;
	u16 adc_y_top;
	u16 adc_y_bot;
	u8  calibrated;
}calibration_param_t;
extern calibration_param_t calibrat_t;

typedef struct {
	lv_coord_t cursor_x;
	lv_coord_t cursor_y;
	u16        raw_x;
	u16        raw_y;
}lv_cursor_pos_t;
extern lv_cursor_pos_t lv_cursor_pos;

// app wide
typedef struct {
	u8 WIFI;
	u8 volume;
	u8 signal;
	u8 SD;
} peri_status_t;
extern peri_status_t peri_status;

typedef struct {
	u8 isMenuOpen;
//	scrn_t *currScrn;
} disp_status_t;
extern disp_status_t disp_status;

typedef struct {
	char notiSource[5];
	char notiContent[20];
} noti_t;

extern uint32_t LV_NOTI_NEW;

void lv_app_init(void);

#endif //__LV_APP_CONF_H
