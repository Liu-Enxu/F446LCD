#ifndef __TAB_H
#define __TAB_H

#include <lvgl/lvgl.h>
#include "sys.h"

typedef struct tabview_t    tabview_t;
typedef struct tab_t        tab_t;

struct tabview_t {
    lv_obj_t *tabview;
    
    u8 tab_cnt;
    lv_obj_t **tabs; // array of content objects for each tab
};

struct tab_t{
    u8 id;
    lv_obj_t *tab;
};

tabview_t* create_tabview(lv_obj_t *parent, uint16_t tabview_x, uint16_t tabview_y, uint16_t tabview_w, uint16_t tabview_h, uint8_t head_h);

tab_t* create_tab(tabview_t* my_tabview, const char* tab_name);


#endif /* __TAB_H */