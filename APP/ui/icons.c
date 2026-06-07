#include "icons.h"

#define HOR_RES      480     
#define VER_RES      320     

// static const char * btnm_map[] = {" ", " ", " ", " ", "\n",
// 								" ", " ", " ", " ", ""};


static void btnm_press_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    icons_t * my_icons = lv_event_get_user_data(e);
    if(lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj) + ICONS_PER_PAGE*(my_icons->curr_page);
        printf("app %d was pressed\n", id);

        // 0. if id more than registered app, return since not possible
        if(id>=app_mgr_inst()->app_registered){
            printf("Larger than total apps registered!\r\n");
            return;
        }

        // 1. find app
        u8 tmp_app_id = 0;
        app_t* tmp_app = app_mgr_inst()->app_head;
        while(id != tmp_app_id){    // no need to check id out of bound since checked
            tmp_app = tmp_app->next_app;
            tmp_app_id++;
        }

        // 2. look up in tab (resource)
        tab_t* tmp = get_app_tv_inst()->tab_head->next_tab;
        while(NULL != tmp->app_ptr){
            if(NULL == tmp->next_tab){
                // 2.1 no free tab, return
                printf("Exceed max running app available!\r\n");
                return;
            } else if (tmp->app_ptr==tmp_app){
                // 2.2 app already running, switch to it
                printf("App already running, switch to it!\r\n");
                lv_tabview_set_act(get_app_tv_inst()->tabview, tmp->id, LV_ANIM_OFF);
                return;
            } else {
                tmp = tmp->next_tab;
            }
        }
        // 2.3 free tab, clear hidden flag
        show_tab(get_app_tv_inst(), tmp);
        // 3. select the tab 
        lv_tabview_set_act(get_app_tv_inst()->tabview, tmp->id, LV_ANIM_OFF);
        // 4. create_app on tab ptr
        tmp_app->app_t_load(tmp_app,tmp->tab);
        tmp->app_ptr = tmp_app; 
        // 5. change tab button name to app name
        rename_tab(get_app_tv_inst(), tmp, tmp_app->app_name);
    }
}

static void btnm_draw_event_cb(lv_event_t * e)
{
	if(lv_event_get_code(e) == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
		if(dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
            /*Change the draw descriptor of the 2nd button*/
            // if(dsc->label_dsc != NULL && dsc->id != ICONS_PER_PAGE/2 && dsc->id != ICONS_PER_PAGE+1) {
            dsc->label_dsc->font = &awesomesyms;
            dsc->rect_dsc->bg_opa = LV_OPA_30;
		}
	}
}

static void update_button(icons_t * btnm, uint8_t i, const char * lbl_txt) {

    if(lbl_txt == NULL){
        printf("Invalid label text to update!\r\n");
        return;
    }

    if(strlen(lbl_txt) > BYTE_PER_ICON){
        printf("Label text too long to update!\r\n");
        return;
    }

    if(i<ICONS_PER_PAGE/2){
        strcpy(btnm->btnm_buf[i], lbl_txt);
    } else if(i>ICONS_PER_PAGE/2 && i<ICONS_PER_PAGE){
        strcpy(btnm->btnm_buf[i+1], lbl_txt);
    } else {
        printf("Invalid button id to update!\r\n");
        return;
    }
        
    // lv_btnmatrix_set_map(btnm->icons_btnm, btnm->btnm_map); once hooked to ptr, no need to set map again
}

static void update_page(icons_t * btnm) {
    for(uint8_t i = 0; i < ICONS_PER_PAGE; i++) {
        if(btnm->curr_page * ICONS_PER_PAGE + i >= app_mgr_inst()->app_registered) {
            lv_btnmatrix_set_btn_ctrl(btnm->icons_btnm, i, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED);
            // set btn label to empty
            update_button(btnm, i, " ");
        } else {
            lv_btnmatrix_clear_btn_ctrl(btnm->icons_btnm, i, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED);
            // set btn label to app icon
            app_t* tmp_app = app_mgr_inst()->app_head;
            uint8_t j = 0;
            while(j < btnm->curr_page * ICONS_PER_PAGE + i) {
                tmp_app = tmp_app->next_app;
                j++;
            }
            update_button(btnm, i, tmp_app->app_icon);
        }
    }
    lv_obj_invalidate(btnm->icons_btnm);
}

static void turn_page_left(lv_event_t * e){
    if(lv_event_get_code(e) == LV_EVENT_PRESSED) {
        icons_t * btnm = (icons_t * )lv_event_get_user_data(e);
        if(btnm->curr_page>0){
            btnm->curr_page--;
            if(btnm->curr_page==0){
                // lv_obj_add_flag(btnm->btn_l,LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_state(btnm->btn_l, LV_STATE_DISABLED); 
            }else if(btnm->curr_page>0){
                // lv_obj_clear_flag(btnm->btn_l,LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_state(btnm->btn_l, LV_STATE_DISABLED); 
            }
            if((btnm->curr_page+1)*ICONS_PER_PAGE < app_mgr_inst()->app_registered){
                // lv_obj_clear_flag(btnm->btn_r,LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_state(btnm->btn_r, LV_STATE_DISABLED);
            }
        }
        update_page(btnm);
    }
}

static void turn_page_right(lv_event_t * e){
    if(lv_event_get_code(e) == LV_EVENT_PRESSED) {
        icons_t * btnm = (icons_t * )lv_event_get_user_data(e);
        if((btnm->curr_page+1)*ICONS_PER_PAGE < app_mgr_inst()->app_registered){
            btnm->curr_page++;
            if((btnm->curr_page+1)*ICONS_PER_PAGE >= app_mgr_inst()->app_registered){
                // lv_obj_add_flag(btnm->btn_r,LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_state(btnm->btn_r, LV_STATE_DISABLED);
            } else if((btnm->curr_page+1)*ICONS_PER_PAGE < app_mgr_inst()->app_registered){
                // lv_obj_clear_flag(btnm->btn_r,LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_state(btnm->btn_r, LV_STATE_DISABLED);
            }
            if(btnm->curr_page>0){
                // lv_obj_clear_flag(btnm->btn_l,LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_state(btnm->btn_l, LV_STATE_DISABLED);
            }
        }
        update_page(btnm);
    }
}

icons_t* create_icons(lv_obj_t * parent){
    // struct malloc
    icons_t* my_icons = lv_mem_alloc(sizeof(icons_t));
    if (my_icons == NULL) {
        printf("Error allocating memory for icons_t!!\r\n");
        return NULL;
    }
    // curr_page
    my_icons->curr_page = 0;

    // btnm_map init with first 8 app icons, rest empty
    for(uint8_t i = 0; i < ICONS_PER_PAGE+2; i++) {
        if(i==ICONS_PER_PAGE/2){
            strcpy(my_icons->btnm_buf[i], "\n");
        } else if(i==ICONS_PER_PAGE+1){
            strcpy(my_icons->btnm_buf[i], "");
        } else {
            strcpy(my_icons->btnm_buf[i], " ");
        }
        my_icons->btnm_map[i] = my_icons->btnm_buf[i];
    }
    app_t* tmp_app = app_mgr_inst()->app_head;
    uint8_t idx=0;
    while(idx<ICONS_PER_PAGE+2 && tmp_app != NULL){   // only show first 8 apps
        // 1. skip number 4 which is for newline
        if(idx==ICONS_PER_PAGE/2)idx++;
        // 2. update btnm_map with app icons
        update_button(my_icons, idx, tmp_app->app_icon);
        // 3. next app
        tmp_app = tmp_app->next_app;
        idx++;
    }
    
    // icons_btnm obj
    my_icons->icons_btnm = lv_btnmatrix_create(parent);
    lv_obj_add_style(my_icons->icons_btnm,&lv_app_styles.color_combo1,LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_btnmatrix_set_map(my_icons->icons_btnm, my_icons->btnm_map);
	lv_obj_set_size(my_icons->icons_btnm, HOR_RES-80, VER_RES-50);
	lv_obj_align( my_icons->icons_btnm, LV_ALIGN_CENTER, 0, 0);

	lv_obj_set_style_bg_opa( my_icons->icons_btnm, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width( my_icons->icons_btnm, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_event_cb( my_icons->icons_btnm, btnm_press_event_cb, LV_EVENT_VALUE_CHANGED, my_icons);
	lv_obj_clear_flag( my_icons->icons_btnm, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag( my_icons->icons_btnm, LV_OBJ_FLAG_EVENT_BUBBLE);
	lv_obj_add_event_cb( my_icons->icons_btnm, btnm_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

    // left and right button to change page
	my_icons->btn_l = lv_btn_create(parent);
	lv_obj_set_size(my_icons->btn_l,30,VER_RES-250);
	lv_obj_align(my_icons->btn_l, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_add_style(my_icons->btn_l,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(my_icons->btn_l, LV_OPA_30, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_add_flag(my_icons->btn_l,LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(my_icons->btn_l, turn_page_left, LV_EVENT_PRESSED, my_icons);
	my_icons->btn_l_lbl = lv_label_create(my_icons->btn_l);
	lv_label_set_text(my_icons->btn_l_lbl,"<");
	lv_obj_align(my_icons->btn_l_lbl, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_flag(my_icons->btn_l_lbl,LV_OBJ_FLAG_EVENT_BUBBLE);
    // lv_obj_add_flag(my_icons->btn_l,LV_OBJ_FLAG_HIDDEN); 
    lv_obj_add_state(my_icons->btn_l, LV_STATE_DISABLED);   // disabled at start


	my_icons->btn_r = lv_btn_create(parent);
	lv_obj_set_size(my_icons->btn_r,30,VER_RES-250);
	lv_obj_align(my_icons->btn_r, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_add_style(my_icons->btn_r,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(my_icons->btn_r, LV_OPA_30, LV_PART_MAIN | LV_STATE_DISABLED);
	lv_obj_add_flag(my_icons->btn_r,LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(my_icons->btn_r, turn_page_right, LV_EVENT_PRESSED, my_icons);
	my_icons->btn_r_lbl = lv_label_create(my_icons->btn_r);
	lv_label_set_text(my_icons->btn_r_lbl,">");
	lv_obj_align(my_icons->btn_r_lbl, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_flag(my_icons->btn_r_lbl,LV_OBJ_FLAG_EVENT_BUBBLE);
    if((my_icons->curr_page+1)*ICONS_PER_PAGE >= app_mgr_inst()->app_registered){    // not only hide right button, also hide icons
        // lv_obj_add_flag(my_icons->btn_r,LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_state(my_icons->btn_r, LV_STATE_DISABLED);   // disabled if less
        u8 i=app_mgr_inst()->app_registered;
        while(i<ICONS_PER_PAGE){
            lv_btnmatrix_set_btn_ctrl(my_icons->icons_btnm, i, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED);
            i++;
        }
    }
    return my_icons;
}

void free_icons(icons_t *icons)
{
    if (icons == NULL) return;
    // lv_obj_del not needed, parent tabview owns and destroys all lv_obj children
    lv_mem_free(icons);
}


