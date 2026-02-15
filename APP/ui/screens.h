#ifndef __SCREENS_H
#define __SCREENS_H

#include <lvgl/lvgl.h>

typedef struct {
    lv_obj_t *load;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
		lv_obj_t *obj2;
		lv_obj_t *obj3;
		lv_obj_t *obj4;
		lv_obj_t *obj5;
} scrn_load_t;


typedef struct {
    lv_obj_t *main;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
		lv_obj_t *obj2;
		lv_obj_t *obj3;
		lv_obj_t *obj4;
		lv_obj_t *obj5;
		lv_obj_t *obj6;
		lv_obj_t *obj7;
		lv_obj_t *obj8;
		lv_obj_t *obj9;
		lv_obj_t *obj10;
		lv_obj_t *obj11;
		lv_obj_t *obj12;
} scrn_main_t;


void create_screen_load();
void create_screen_main();



#endif /*EEZ_LVGL_UI_SCREENS_H*/