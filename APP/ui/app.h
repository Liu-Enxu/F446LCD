#ifndef __APP_H
#define __APP_H

#include <lvgl/lvgl.h>
#include "sys.h"
#include "tab.h"

// app tabs -  in developement
typedef struct app_t app_t;
struct app_t{	// need revise
	u8 id;
	lv_obj_t *tab;
	tab_t* app_tab;
};

#endif // __APP_H