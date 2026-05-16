#include "tab.h"

tabview_t* create_tabview(lv_obj_t *parent, uint16_t tabview_x, uint16_t tabview_y, uint16_t tabview_w, uint16_t tabview_h, uint8_t head_h){
    
    // check for valid args
    if (parent == NULL || tabview_w == 0 || tabview_h == 0 || head_h >= tabview_h) {
        printf("Invalid args in create_tabview!\r\n");
        return NULL;
    }

    // struct malloc
    tabview_t* my_tabview = lv_mem_alloc(sizeof(tabview_t));
    if (my_tabview == NULL) {
        printf("Error allocating memory for tabview_t!!\r\n");
        return NULL;
    }

    // tabview obj
    my_tabview->tabview = lv_tabview_create(parent, LV_DIR_TOP, head_h);
    lv_obj_set_pos(my_tabview->tabview, 0, 0);
	lv_obj_set_size(my_tabview->tabview, tabview_w, tabview_h);
	lv_obj_clear_flag(my_tabview->tabview, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(my_tabview->tabview, LV_OBJ_FLAG_EVENT_BUBBLE);

    // tabs array
    my_tabview->tab_cnt = 0;
    my_tabview->tabs = NULL;

    return my_tabview;
}

tab_t* create_tab(tabview_t* my_tabview, const char* tab_name){
    // check for valid args
    if (my_tabview == NULL || tab_name == NULL) {
        printf("Invalid args in create_tab!\r\n");
        return NULL;
    }

    // struct malloc
    tab_t* my_tab = lv_mem_alloc(sizeof(tab_t));
    if (my_tab == NULL) {
        printf("Error allocating memory for tab_t!!\r\n");
        return NULL;
    }

    // tab obj
    my_tab->tab = lv_tabview_add_tab(my_tabview->tabview, tab_name);
    lv_obj_clear_flag(my_tab->tab, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(my_tab->tab, LV_OBJ_FLAG_EVENT_BUBBLE);

    // add to tabview's tabs array
    uint8_t new_cnt = my_tabview->tab_cnt + 1;
    lv_obj_t** new_tabs = lv_mem_alloc(new_cnt * sizeof(lv_obj_t*));
    if (new_tabs == NULL) {
        printf("Error allocating memory for new_tabs array in create_tab!!\r\n");
        lv_mem_free(my_tab);
        return NULL;
    }
    for (uint8_t i = 0; i < my_tabview->tab_cnt; i++) {
        new_tabs[i] = my_tabview->tabs[i];
    }
    new_tabs[my_tabview->tab_cnt] = my_tab->tab;
    if (my_tabview->tabs != NULL) {
        lv_mem_free(my_tabview->tabs);
    }
    my_tabview->tabs = new_tabs;
    my_tabview->tab_cnt = new_cnt;

    return my_tab;
}