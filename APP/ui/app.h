#ifndef __APP_H
#define __APP_H

#include <lvgl/lvgl.h>
#include "sys.h"
#include "menu.h"

typedef struct app_mgr_t app_mgr_t;
typedef struct app_t app_t;
typedef struct settings_app_t settings_app_t;

struct app_mgr_t{
	u8 app_registered;
	app_t* app_head;
	u8 app_mgr_inited;
};

app_mgr_t* app_mgr_inst(void);
void app_mgr_register(app_t* my_app);
void app_mgr_unreg(app_t* my_app);

struct app_t{
	lv_obj_t * app_lbl;
	app_t* next_app;
	void (*app_t_load)(app_t* self, lv_obj_t* parent);
	void (*app_t_exit)(app_t* self);
};

struct settings_app_t{
	app_t app_base;
	menu_t* settings_menu;
};
settings_app_t* create_settings_app(void);


#endif // __APP_H

