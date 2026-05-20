#ifndef __ICONS_H
#define __ICONS_H

#include <lvgl/lvgl.h>
#include "sys.h"
#include "lv_app_conf.h"


typedef struct icons_t icons_t;
typedef struct icn_t icn_t;

struct icons_t {
    u8 app_registered;  // no need to go above 256
    u8 curr_page;

    lv_obj_t * icons_btnm;
    lv_obj_t * btn_l;
    lv_obj_t * btn_l_lbl;
    lv_obj_t * btn_r;
    lv_obj_t * btn_r_lbl;
};

struct icn_t {
    u8 page;
    u8 idx;
};

icons_t* create_icons(lv_obj_t * parent);

#endif // __ICONS_H

