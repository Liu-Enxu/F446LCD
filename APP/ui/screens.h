#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *load;
    lv_obj_t *main;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
		lv_obj_t *obj2;
} objects_t;

extern objects_t objects;
extern lv_style_t my_style0;
extern lv_style_t my_style1;

enum ScreensEnum {
    SCREEN_ID_LOAD = 1,
    SCREEN_ID_MAIN = 2,
};

void create_screen_load();

void create_screen_main();

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/