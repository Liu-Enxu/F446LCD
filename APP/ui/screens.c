#include <string.h>

#include "screens.h"
#include "images.h"
#include "ui.h"
//#include "lain_gaze_fix.h"
//#include "sys.h"
//#include "lcd.h"

objects_t objects;
lv_style_t my_style0;
lv_style_t my_style1;

lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;



void create_screen_load() {
		
		// 	screen obj
    lv_obj_t *obj = lv_obj_create(0);
    objects.load = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
		
		// 	label obj
		lv_obj_t *L_obj = lv_label_create(obj);
		objects.obj0 = L_obj;
		lv_obj_set_pos(L_obj, 279, 70);
		lv_obj_set_size(L_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_style_init(&my_style0);
		lv_style_set_text_font(&my_style0, &lv_font_montserrat_48);
		lv_style_set_text_color(&my_style0, lv_color_hex(0xffc1b492));
		lv_style_set_transform_pivot_x(&my_style0, 101);
		lv_style_set_transform_pivot_y(&my_style0, 52);
		lv_style_set_transform_rotation(&my_style0, 900);
		lv_label_set_text(L_obj, "LAIN OS\nv0.2");
		lv_obj_add_style(L_obj,&my_style0,LV_PART_MAIN | LV_STATE_DEFAULT);
	
		//	button obj
		lv_obj_t *b_obj = lv_button_create(obj);
		objects.obj1 = b_obj;
		lv_obj_set_pos(b_obj , 335, 240);
		lv_obj_set_size(b_obj , 100, 50);
		//	apply style to button
//		lv_obj_set_style_bg_color(b_obj, lv_color_hex(0xffd2738a),LV_PART_MAIN | LV_STATE_DEFAULT);
//		lv_obj_set_style_text_color(b_obj, lv_color_hex(0xffc1b492),LV_PART_MAIN | LV_STATE_DEFAULT);
//		lv_obj_set_style_text_font(b_obj, &lv_font_montserrat_22,LV_PART_MAIN | LV_STATE_DEFAULT);
//		lv_obj_set_style_text_align(b_obj, LV_TEXT_ALIGN_CENTER,LV_PART_MAIN | LV_STATE_DEFAULT);
		
		lv_style_init(&my_style1);
		lv_style_set_bg_color(&my_style1, lv_color_hex(0xffd2738a));
		lv_style_set_text_color(&my_style1, lv_color_hex(0xffc1b492));
		lv_style_set_text_font(&my_style1, &lv_font_montserrat_22);
		lv_style_set_text_align(&my_style1, LV_TEXT_ALIGN_CENTER);
		lv_obj_add_style(b_obj,&my_style1,LV_PART_MAIN | LV_STATE_DEFAULT);
		
		//	label obj on button obj
		lv_obj_t *l_obj = lv_label_create(b_obj);
		objects.obj2 = l_obj;
		lv_obj_set_pos(l_obj, 0, 0);
		lv_obj_set_size(l_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_align(l_obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_label_set_text(l_obj, "MAIN\nMENU");
		
		// 	img obj
//		lv_obj_t * img_obj = lv_image_create(obj);
//		lv_obj_set_pos(obj, 40, 40);
//		lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
//		lv_image_set_src(obj, &          );
		
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
