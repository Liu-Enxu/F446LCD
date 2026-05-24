#ifndef __TAB_H
#define __TAB_H

#include <lvgl/lvgl.h>
#include "sys.h"
#include "app.h"

typedef struct tabview_t    tabview_t;
typedef struct tab_t        tab_t;
struct tabview_t {
    lv_obj_t *tabview;
    app_mgr_t* app_mgr;

    u8 tab_line_hidden;
    lv_draw_rect_dsc_t tab_line;
    
    u8 tab_cnt;
    tab_t *tab_head; // array of content objects for each tab
};

struct tab_t{
    u8 id;
    lv_obj_t *tab;
    app_t* app_ptr;
    tab_t *next_tab;
};

tabview_t* get_app_tv_inst(void);
void set_app_tv_inst(tabview_t* my_tv);
tabview_t* create_tabview(lv_obj_t *parent, uint16_t tabview_x, uint16_t tabview_y, uint16_t tabview_w, uint16_t tabview_h, uint8_t head_h);
tab_t* create_tab(tabview_t* my_tabview, const char* tab_name);
void hide_tab(tabview_t* my_tabview, tab_t* my_tab);
void show_tab(tabview_t* my_tabview, tab_t* my_tab);

void free_tab(tab_t *tab);
void free_tabview(tabview_t *tabview);

#endif /* __TAB_H */

