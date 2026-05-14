#ifndef __SCREENS_H
#define __SCREENS_H

#include <lvgl/lvgl.h>
#include "sys.h"
#include "menu.h"

// forward declarations
typedef struct scrn_t scrn_t;
typedef struct scrn_cali_t scrn_cali_t;
typedef struct scrn_load_t scrn_load_t;
typedef struct scrn_main_t scrn_main_t;

//	screen manager -------------------------------------------------
typedef enum {
	SCREEN_LOAD = 0,
	SCREEN_MAIN,
	SCREEN_CALIBRATE,
} scrn_type_t;

typedef struct {
	// scrn_type_t curr_scrn_type;
	u8 scrn_manager_inited;
	scrn_t* curr_scrn_ptr;
} scrn_manager_t;
scrn_manager_t* scrn_manager_inst(void);
void start_scrn_manager(void);

//	father class/general screen ---------------------------------
struct scrn_t {
	// member
	lv_obj_t *screen;
	lv_obj_t *pos_obj;
	lv_obj_t *posX_obj;
	lv_obj_t *posY_obj;
	
	// operation funcs
	void (*scrn_t_enter)(scrn_t* self);
	void (*scrn_t_exit)(scrn_t* self);
};

//	children classes ---------------------------------------------
struct scrn_cali_t {	// inherit from scrn_t
	scrn_t	 scrn_base;

	lv_obj_t *circle;
	lv_obj_t *circle_pos_obj;
	lv_obj_t *lbl_pref_obj;
	lv_obj_t *lbl_sep_obj;
	lv_obj_t *lbl_comma_obj;
	
};

struct scrn_load_t{	// inherit from scrn_t
	scrn_t	 scrn_base;

    lv_obj_t *L_obj;
    lv_obj_t *b_obj;
	lv_obj_t *l_obj;
	lv_obj_t *ascii_obj;
	lv_obj_t *welcome_obj;
};

struct scrn_main_t {	// inherit from scrn_t
	scrn_t	 scrn_base;

	// header
	lv_obj_t *header_obj;
	lv_obj_t *menu_b_obj;
	lv_obj_t *menu_l_obj;
	menu_t *menu;

	lv_obj_t *folder_obj;
	lv_obj_t *folder_l_obj;
	lv_obj_t *terminal_obj;
	lv_obj_t *terminal_l_obj;

	// footer/status bar
	lv_obj_t *bar_obj;
	lv_obj_t *ret_obj;
	lv_obj_t *ret_val_obj;
	lv_obj_t *ucHeap_l_obj;
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
};


// app tabs -  in developement
typedef struct{	// need revise
	u8 id;
	lv_obj_t *tab;
	void* other_obj;
} app_tab_t;

// constructor funcs
// should be private now since scrn_manager already handles screen switching?
scrn_load_t* create_screen_load(void);
scrn_main_t* create_screen_main(void);
scrn_cali_t* create_screen_cali(void);



#endif /*__SCREENS_H*/

