#ifndef __APP_H
#define __APP_H

#include <math.h>
#include <lvgl/lvgl.h>
#include "lv_snapshot.h"
#include "sys.h"
#include "menu.h"

typedef struct app_mgr_t app_mgr_t;
typedef struct app_t app_t;
typedef struct settings_app_t settings_app_t;
typedef struct calc_app_t calc_app_t;

struct app_mgr_t{
	u8 app_registered;
	app_t* app_head;
	u8 app_mgr_inited;
};

app_mgr_t* app_mgr_inst(void);
void start_app_mgr(void);
void app_mgr_register(app_t* my_app);
void app_mgr_unreg(app_t* my_app);

struct app_t{
	char app_name[10];
	char app_icon[4];
	u8 is_registered;
	app_t* next_app;
	void (*app_t_load)(app_t* self, lv_obj_t* parent);
	void (*app_t_exit)(app_t* self);
};

struct settings_app_t{
	app_t app_base;
	menu_t* settings_menu;
};
settings_app_t* create_settings_app(void);

typedef enum {
    CALC_STATE_INPUT = 0,      // normal input
    CALC_STATE_RESULT = 1,     // after = pressed, showing result
    CALC_STATE_ERROR = 2,      // division by zero, overflow etc
} calc_state_t;

#define CALC_EXPR_LEN 32
#define CALC_STACK_SIZE 16
struct calc_app_t{
	app_t app_base;
	char last_expr[CALC_EXPR_LEN];	
	char curr_expr[CALC_EXPR_LEN];	
	uint8_t  curr_len;
    calc_state_t state;

	lv_obj_t* lbl_last;
	lv_obj_t* lbl_curr;
};
calc_app_t* create_calc_app(void);

#endif // __APP_H

