#include <string.h>

#include "screens.h"
#include "images.h"
//#include "fonts.h"
//#include "actions.h"
//#include "vars.h"
//#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_style_t my_style;

lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;



void create_screen_load() {
		static lv_style_t style1;
		
		// 	screen obj
    lv_obj_t *obj = lv_obj_create(0);
    objects.load = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

		//	button obj
		lv_obj_t *b_obj = lv_button_create(obj);
		objects.obj0 = b_obj;
		lv_obj_set_pos(b_obj , 335, 240);
		lv_obj_set_size(b_obj , 100, 50);
		//	apply style to button
//		lv_obj_set_style_bg_color(b_obj, lv_color_hex(0xffd2738a),LV_PART_MAIN | LV_STATE_DEFAULT);
//		lv_obj_set_style_text_color(b_obj, lv_color_hex(0xffc1b492),LV_PART_MAIN | LV_STATE_DEFAULT);
//		lv_obj_set_style_text_font(b_obj, &lv_font_montserrat_22,LV_PART_MAIN | LV_STATE_DEFAULT);
//		lv_obj_set_style_text_align(b_obj, LV_TEXT_ALIGN_CENTER,LV_PART_MAIN | LV_STATE_DEFAULT);
		
		lv_style_init(&style1);
		lv_style_set_bg_color(&style1, lv_color_hex(0xffd2738a));
		lv_style_set_text_color(&style1, lv_color_hex(0xffc1b492));
		lv_style_set_text_font(&style1, &lv_font_montserrat_22);
		lv_style_set_text_align(&style1, LV_TEXT_ALIGN_CENTER);
		lv_obj_add_style(b_obj,&style1,LV_PART_MAIN | LV_STATE_DEFAULT);
		
		//	label obj on button obj
		lv_obj_t *l_obj = lv_label_create(b_obj);
		objects.obj1 = l_obj;
		lv_obj_set_pos(l_obj, 0, 0);
		lv_obj_set_size(l_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_align(l_obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_label_set_text(l_obj, "MAIN\nMENU");
		
		lv_scr_load_anim(obj, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
		
}

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_spinner_create(parent_obj);
            lv_obj_set_pos(obj, 200, 120);
            lv_obj_set_size(obj, 80, 80);
            lv_spinner_set_anim_params(obj, 1000, 60);
        }
    }
    
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_load();
//    create_screen_main();
}
