#include "icons.h"

#define HOR_RES      480     //LCDˮƽ�ֱ���
#define VER_RES      320     //LCD��ֱ�ֱ���

static const char * btnm_map[] = {" ", " ", " ", " ", "\n",
								" ", " ", " ", " ", ""};

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);

        LV_LOG_USER("%s was pressed\n", txt);
    }
}

static void btnm_draw_event_cb(lv_event_t * e)
{
	static u8 is_hidden[8] = {0};
	if(lv_event_get_code(e) == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
		if(dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
            /*Change the draw descriptor of the 2nd button*/
            dsc->rect_dsc->bg_opa = LV_OPA_30;
		}
	}
}

static void update_page(icons_t * btnm) {
    for(uint8_t i = 0; i < 8; i++) {
        if(btnm->curr_page * 8 + i >= btnm->app_registered) {
            lv_btnmatrix_set_btn_ctrl(btnm->icons_btnm, i, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED);
        } else {
            lv_btnmatrix_clear_btn_ctrl(btnm->icons_btnm, i, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED);
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
                lv_obj_add_flag(btnm->btn_l,LV_OBJ_FLAG_HIDDEN);
            }else if(btnm->curr_page>0){
                lv_obj_clear_flag(btnm->btn_l,LV_OBJ_FLAG_HIDDEN);
            }
            if((btnm->curr_page+1)*8 < btnm->app_registered){
                lv_obj_clear_flag(btnm->btn_r,LV_OBJ_FLAG_HIDDEN);
            }
        }
        update_page(btnm);
    }
}

static void turn_page_right(lv_event_t * e){
    if(lv_event_get_code(e) == LV_EVENT_PRESSED) {
        icons_t * btnm = (icons_t * )lv_event_get_user_data(e);
        if((btnm->curr_page+1)*8 < btnm->app_registered){
            btnm->curr_page++;
            if((btnm->curr_page+1)*8 >= btnm->app_registered){
                lv_obj_add_flag(btnm->btn_r,LV_OBJ_FLAG_HIDDEN);
            } else if((btnm->curr_page+1)*8 < btnm->app_registered){
                lv_obj_clear_flag(btnm->btn_r,LV_OBJ_FLAG_HIDDEN);
            }
            if(btnm->curr_page>0){
                lv_obj_clear_flag(btnm->btn_l,LV_OBJ_FLAG_HIDDEN);
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
    // app_registered
    my_icons->app_registered = 10;
    // curr_page
    my_icons->curr_page = 0;
    
    // icons_btnm obj
    my_icons->icons_btnm = lv_btnmatrix_create(parent);
    lv_btnmatrix_set_map(my_icons->icons_btnm, btnm_map);
	lv_obj_set_size(my_icons->icons_btnm, HOR_RES-80, VER_RES-50);
    lv_obj_add_style(my_icons->icons_btnm,&lv_app_styles.color_combo1,LV_PART_ITEMS | LV_STATE_DEFAULT);
	lv_obj_align( my_icons->icons_btnm, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_bg_opa( my_icons->icons_btnm, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width( my_icons->icons_btnm, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	// lv_obj_add_event_cb( my_icons->icons_btnm, event_handler, LV_EVENT_ALL, NULL);
	lv_obj_clear_flag( my_icons->icons_btnm, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag( my_icons->icons_btnm, LV_OBJ_FLAG_EVENT_BUBBLE);
	lv_obj_add_event_cb( my_icons->icons_btnm, btnm_draw_event_cb, LV_EVENT_ALL, NULL);

    // left and right button to change page
	my_icons->btn_l = lv_btn_create(parent);
	lv_obj_set_size(my_icons->btn_l,30,VER_RES-250);
	lv_obj_align(my_icons->btn_l, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_add_style(my_icons->btn_l,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(my_icons->btn_l,LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(my_icons->btn_l, turn_page_left, LV_EVENT_ALL, my_icons);
	my_icons->btn_l_lbl = lv_label_create(my_icons->btn_l);
	lv_label_set_text(my_icons->btn_l_lbl,"<");
	lv_obj_align(my_icons->btn_l_lbl, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_flag(my_icons->btn_l_lbl,LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(my_icons->btn_l,LV_OBJ_FLAG_HIDDEN); // hidden at start

	my_icons->btn_r = lv_btn_create(parent);
	lv_obj_set_size(my_icons->btn_r,30,VER_RES-250);
	lv_obj_align(my_icons->btn_r, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_add_style(my_icons->btn_r,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(my_icons->btn_r,LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(my_icons->btn_r, turn_page_right, LV_EVENT_ALL, my_icons);
	my_icons->btn_r_lbl = lv_label_create(my_icons->btn_r);
	lv_label_set_text(my_icons->btn_r_lbl,">");
	lv_obj_align(my_icons->btn_r_lbl, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_flag(my_icons->btn_r_lbl,LV_OBJ_FLAG_EVENT_BUBBLE);

    return my_icons;
}

void set_btn_lbl_txt(icons_t* my_icons, u8 id, const char* lbl_txt){
    
}
