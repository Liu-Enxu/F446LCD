#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "app_base.h"
#include "menu.h"


typedef struct settings_app_t settings_app_t;

struct settings_app_t{
	app_t app_base;
	menu_t* settings_menu;
};
settings_app_t* create_settings_app(void);

#endif /*__SETTINGS_H*/

