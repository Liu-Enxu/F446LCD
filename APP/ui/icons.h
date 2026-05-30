#ifndef __ICONS_H
#define __ICONS_H

#include <lvgl/lvgl.h>
#include "sys.h"
#include "lv_app_conf.h"
#include "tab.h"    // included "app.h"

#define ICONS_PER_PAGE 8
#define BYTE_PER_ICON 3   // for now just 4 chars for icon name, can be used for unicode char or short text

typedef struct icons_t icons_t;
typedef struct icn_t icn_t;

struct icons_t {
    u8 curr_page;

    char btnm_buf[ICONS_PER_PAGE+2][BYTE_PER_ICON+1];   // for now just 4 chars for icon name, can be used for unicode char or short text
    const char* btnm_map[ICONS_PER_PAGE+2];

    lv_obj_t * icons_btnm;
    lv_obj_t * btn_l;
    lv_obj_t * btn_l_lbl;
    lv_obj_t * btn_r;
    lv_obj_t * btn_r_lbl;
};

// struct icn_t {
//     u8 page;
//     u8 idx;
// };

icons_t* create_icons(lv_obj_t * parent);
void free_icons(icons_t *icons);
#endif // __ICONS_H

