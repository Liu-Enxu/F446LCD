#ifndef __APP_H
#define __APP_H

#include <lvgl/lvgl.h>
#include "sys.h"

// app tabs -  in developement
typedef struct{	// need revise
	u8 id;
	lv_obj_t *tab;
	void* other_obj;
} app_tab_t;

#endif // __APP_H