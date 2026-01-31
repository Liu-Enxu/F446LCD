#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>



typedef struct _objects_t {
    lv_obj_t *load;
    lv_obj_t *main;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
		lv_obj_t *obj2;
		lv_obj_t *obj3;
//		lv_obj_t *lain_os_1;
//		lv_obj_t *lain_os_2;
//		lv_obj_t *lain_os_3;
} objects_t;
extern objects_t objects;

extern lv_style_t char_color1;
extern lv_style_t char_color2;
extern lv_style_t color_combo;

void create_screen_load();
//void create_screen_main();



#endif /*EEZ_LVGL_UI_SCREENS_H*/