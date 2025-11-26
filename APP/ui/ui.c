#include "ui.h"
#include "screens.h"
#include "images.h"
//#include "actions.h"
//#include "vars.h"
#include "delay.h"

static int16_t currentScreen = -1;

void ui_init() {
    create_screens();
		lv_obj_t *screen = ((lv_obj_t **)&objects)[SCREEN_ID_LOAD-1];
    lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
}

