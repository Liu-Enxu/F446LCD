#ifndef __APP_H
#define __APP_H

//#include <math.h>
#include <lvgl/lvgl.h>
#include "sys.h"

#include "app_base.h"
#include "settings.h"
#include "calc.h"
#include "utop.h"
#include "filesys.h"

typedef struct app_mgr_t app_mgr_t;

struct app_mgr_t{
	u8 app_registered;
	app_t* app_head;
	u8 app_mgr_inited;
};

app_mgr_t* app_mgr_inst(void);
void start_app_mgr(void);
void app_mgr_register(app_t* my_app);
void app_mgr_unreg(app_t* my_app);


#endif // __APP_H

