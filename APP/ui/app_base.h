#ifndef __APP_BASE_H
#define __APP_BASE_H

#include <lvgl/lvgl.h>
#include "sys.h"

typedef struct app_t app_t;
struct app_t{
	char app_name[10];
	char app_icon[4];
	u8 is_registered;
	app_t* next_app;
	void (*app_t_load)(app_t* self, lv_obj_t* parent);
	void (*app_t_exit)(app_t* self);
};

#endif /*__APP_BASE_H*/

