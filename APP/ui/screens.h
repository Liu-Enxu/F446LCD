#ifndef __SCREENS_H
#define __SCREENS_H

#include <lvgl/lvgl.h>
#include "sys.h"

typedef struct {
	lv_obj_t *screen;
	lv_obj_t *circle;
	lv_obj_t *pos_obj;
	lv_obj_t *circle_pos_obj;
	lv_obj_t *posX_obj;
	lv_obj_t *posY_obj;
	lv_obj_t *lbl_pref_obj;
	lv_obj_t *lbl_sep_obj;
	lv_obj_t *lbl_comma_obj;
	
} scrn_cali_t;

typedef struct {
	lv_obj_t *screen;
    lv_obj_t *L_obj;
    lv_obj_t *b_obj;
	lv_obj_t *l_obj;
	lv_obj_t *ascii_obj;
	lv_obj_t *pos_obj;
	lv_obj_t *posX_obj;
	lv_obj_t *posY_obj;
	lv_obj_t *welcome_obj;
} scrn_load_t;


typedef struct {
		lv_obj_t *screen;
		// header
		lv_obj_t *header_obj;
		lv_obj_t *menu_b_obj;
		lv_obj_t *menu_l_obj;
		lv_obj_t *menu_obj;
			lv_obj_t *menu_page_obj;
				lv_obj_t *menu_splash_obj;
					lv_obj_t *menu_splash_b_obj;
						lv_obj_t *menu_splash_l_obj;
				lv_obj_t *menu_calibrate_obj;
					lv_obj_t *menu_calibrate_b_obj;
						lv_obj_t *menu_calibrate_l_obj;
				lv_obj_t *menu_settings_obj;
					lv_obj_t *menu_settings_b_obj;
						lv_obj_t *menu_settings_l_obj;
	
		lv_obj_t *folder_obj;
		lv_obj_t *folder_l_obj;
		lv_obj_t *terminal_obj;
		lv_obj_t *terminal_l_obj;
	
		// footer/status bar
    	lv_obj_t *bar_obj;
    	lv_obj_t *ret_obj;
		lv_obj_t *ret_val_obj;
		lv_obj_t *pos_obj;
		lv_obj_t *posX_obj;
		lv_obj_t *posY_obj;
		lv_obj_t *tray_obj;
		lv_obj_t *end_obj;
		lv_obj_t *wifi_obj;
		lv_obj_t *wifix_obj;
		lv_obj_t *sound_obj;
		lv_obj_t *soundx_obj;
		lv_obj_t *battery_obj;
		lv_obj_t *batteryx_obj;
		lv_obj_t *SD_obj;
		lv_obj_t *SDx_obj;
		lv_obj_t *sta_obj;
} scrn_main_t;


typedef struct{
	lv_obj_t *menu_obj;
	lv_obj_t *menu_page_obj;	// root page
	
	uint8_t section_len;	// number of sections
	section_t **menu_sections;	// array of ptrs of sections
} menu_t;

typedef struct {
	lv_obj_t *section_obj;
	uint8_t cont_len;	// number of contents
	cont_t **section_conts;		// array of contents
} section_t;

typedef struct {
	lv_obj_t *cont_obj;
	lv_obj_t *cont_label;	// content label
	submenu_t *cont_submenu;
} cont_t;

typedef struct {
	lv_obj_t *submenu_page_obj;	// lv_obj_t * sub_about_page = lv_menu_page_create(menu, NULL);
	
	uint8_t section_len;	// number of sections
	section_t **submenu_sections;	// array of ptrs of sections

} submenu_t;	



typedef struct{
	u8 id;
	lv_obj_t *tab;
	void* other_obj;
} app_t;
	
void create_screen_cali(void);
void create_screen_load(void);
void create_screen_main(void);



#endif /*__SCREENS_H*/