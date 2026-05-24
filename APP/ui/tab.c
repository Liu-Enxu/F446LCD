#include "tab.h"
#include "lv_app_conf.h"
#include "lcd.h"

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

    // tab id
    my_tab->id = my_tabview->tab_cnt++;
    // tab app_ptr
    my_tab->app_ptr = NULL;
    // tab obj
    my_tab->tab = lv_tabview_add_tab(my_tabview->tabview, tab_name);
    lv_obj_set_style_pad_all(my_tab->tab, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(my_tab->tab, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(my_tab->tab, LV_OBJ_FLAG_EVENT_BUBBLE);
    // next tab
    my_tab->next_tab = NULL;

    // add to previous tab_t
    if(my_tabview->tab_head != NULL) {
        tab_t *tail = my_tabview->tab_head;
        while(tail->next_tab != NULL) {
            tail = tail->next_tab;
        }
        tail->next_tab = my_tab;
        lv_btnmatrix_set_btn_width(lv_tabview_get_tab_btns(my_tabview->tabview), 0, my_tab->id);
    }

    return my_tab;
}

void hide_tab(tabview_t* my_tabview, tab_t* my_tab){    // you cant really delete tab
    // check for valid args
    if (my_tabview == NULL || my_tab == NULL) {
        printf("Invalid args in hide_tab!\r\n");
        return;
    }

    // hide tab obj
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(my_tabview->tabview);
    lv_btnmatrix_set_btn_ctrl(tab_btns, my_tab->id, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED);
    // lv_btnmatrix_set_btn_width(tab_btns, 0, (++(my_tabview->tab_cnt)-1));   // counter intuitive but works

}

void show_tab(tabview_t* my_tabview, tab_t* my_tab){
    // check for valid args
    if (my_tabview == NULL || my_tab == NULL) {
        printf("Invalid args in show_tab!\r\n");
        return;
    }

    // hide tab obj
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(my_tabview->tabview);
    lv_btnmatrix_clear_btn_ctrl(tab_btns, my_tab->id, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED);
    // lv_btnmatrix_set_btn_width(tab_btns, 0, (--(my_tabview->tab_cnt)-1));   // counter intuitive but works
}

static void tab_draw_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
		if(dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
            /*Change the draw descriptor of the 1st button*/
            if(dsc->id == 0) {
				dsc->rect_dsc->bg_opa = LV_OPA_0;
				dsc->rect_dsc->border_opa = LV_OPA_0;
			}
		}
	}
}

static void tab_changed_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;
    tabview_t *my_tabview = (tabview_t *)lv_event_get_user_data(e);
    uint16_t active = lv_tabview_get_tab_act(my_tabview->tabview);
    my_tabview->tab_line_hidden = (active == 0);
    lv_obj_invalidate(lv_event_get_target(e));  // trigger redraw only
}

static void tab_line_draw_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) != LV_EVENT_DRAW_POST) return;
    tabview_t *my_tabview = (tabview_t *)lv_event_get_user_data(e);
    if (my_tabview->tab_line_hidden) return;
    lv_draw_ctx_t *draw_ctx = lv_event_get_draw_ctx(e);
    lv_area_t area = { .x1 = 0, .x2 = HOR_RESOLUTION, .y1 = 30, .y2 = 34 };
    lv_draw_rect(draw_ctx, &my_tabview->tab_line, &area);
}

static tabview_t *app_tabview = NULL;
tabview_t* get_app_tv_inst(void){
    if(NULL == app_tabview) printf("NULL app tabview!");
    return app_tabview;
}

void set_app_tv_inst(tabview_t* my_tv){    // could have set in create_, but potentially other tabviews
    if(my_tv == NULL){
        printf("tabview is NULL!\r\n");
        return;
    }
    app_tabview = my_tv;
}

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

    // tab_line
    my_tabview->tab_line_hidden = 1; // initially hidden since first tab is active
    lv_draw_rect_dsc_init(&my_tabview->tab_line);
    my_tabview->tab_line.bg_color     = lv_app_styles.color1;
    my_tabview->tab_line.bg_opa       = LV_OPA_COVER;
    my_tabview->tab_line.radius       = 0;
    my_tabview->tab_line.border_width = 0;

    // tabs array
    my_tabview->tab_cnt = 0;
    my_tabview->tab_head = create_tab(my_tabview, "     "); // create first tab as head

    // tab array content
    lv_obj_clear_flag(lv_tabview_get_content(my_tabview->tabview), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(lv_tabview_get_content(my_tabview->tabview), LV_OBJ_FLAG_EVENT_BUBBLE);
    // tab btns style
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(my_tabview->tabview);
	lv_obj_add_style(tab_btns,&lv_app_styles.color_combo1,LV_PART_ITEMS | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(tab_btns, LV_OPA_10, LV_PART_ITEMS | LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(tab_btns, LV_OPA_TRANSP, LV_PART_ITEMS | LV_STATE_DEFAULT);
	
	lv_obj_add_style(tab_btns,&lv_app_styles.color_combo1,LV_PART_ITEMS | LV_STATE_CHECKED);
	lv_obj_set_style_border_opa(tab_btns, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_CHECKED);
	lv_obj_set_style_border_color(tab_btns, lv_app_styles.color1, LV_PART_ITEMS | LV_STATE_CHECKED);

    lv_btnmatrix_set_btn_width(tab_btns, 0, 1);
    lv_obj_add_event_cb(tab_btns, tab_draw_event_cb, LV_EVENT_ALL, NULL);
	lv_obj_set_style_bg_opa(tab_btns, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    // event cb for tab change and tab line redraw
    lv_obj_add_event_cb(my_tabview->tabview, tab_changed_cb, LV_EVENT_ALL, my_tabview);
    lv_obj_add_event_cb(parent, tab_line_draw_event_cb, LV_EVENT_DRAW_POST, my_tabview);

    return my_tabview;
}


void free_tab(tab_t *tab)
{
    if (tab == NULL) return;
    // don't lv_obj_del tab->tab here — tabview owns and destroys it
    // just walk and free the linked list nodes
    free_tab(tab->next_tab);  // recurse to end of list
    lv_mem_free(tab);
}

void free_tabview(tabview_t *tabview)
{
    if (tabview == NULL) return;
    // deleting the tabview LVGL obj cascades to all tab lv_obj children
    lv_obj_del(tabview->tabview);
    // then free our linked list of tab_t structs
    free_tab(tabview->tab_head);
    lv_mem_free(tabview);
}
