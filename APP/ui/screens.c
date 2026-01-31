#include <string.h>

#include "screens.h"
//#include "lain_gaze_fix.h"
//#include "sys.h"
//#include "lcd.h"

objects_t objects;

lv_style_t char_color1;
lv_style_t char_color2;
lv_style_t color_combo;

//lv_obj_t *tick_value_change_obj;
//uint32_t active_theme_index = 0;

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
		lv_obj_set_pos(L_obj, 480, 10);
		lv_obj_set_size(L_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_label_set_text(L_obj, " __     __   __  __ _ \n(  )   / _\\ (  )(  ( \\\n/ (_/\\/    \\ )( /    /\n\\____/\\_/\\_/(__)\\_)__)\n  __   ____ \n /  \\ / ___)	V0.2\n(  O )\\___ \\\n \\__/ (____/");

		lv_style_init(&char_color1);
		lv_style_set_text_font(&char_color1, &fixedsys);
		lv_style_set_text_color(&char_color1, lv_color_hex(0xffc4a656));
		lv_style_set_transform_pivot_x(&char_color1, 0);
		lv_style_set_transform_pivot_y(&char_color1, 0);
		lv_style_set_transform_angle(&char_color1, 900);	
		lv_obj_add_style(L_obj,&char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);

		//	button obj
		lv_obj_t *b_obj = lv_btn_create(obj);
		objects.obj1 = b_obj;
		lv_obj_set_pos(b_obj , 330, 240);
		lv_obj_set_size(b_obj , 100, 50);
		//	apply style to button
//		lv_obj_set_style_bg_color(b_obj, lv_color_hex(0xffd2738a),LV_PART_MAIN | LV_STATE_DEFAULT);
//		lv_obj_set_style_text_color(b_obj, lv_color_hex(0xffc1b492),LV_PART_MAIN | LV_STATE_DEFAULT);
//		lv_obj_set_style_text_font(b_obj, &lv_font_montserrat_16,LV_PART_MAIN | LV_STATE_DEFAULT);
//		lv_obj_set_style_text_align(b_obj, LV_TEXT_ALIGN_CENTER,LV_PART_MAIN | LV_STATE_DEFAULT);
		
		lv_style_init(&color_combo);
		lv_style_set_bg_color(&color_combo, lv_color_hex(0xffa83450));
		lv_style_set_text_color(&color_combo, lv_color_hex(0xffc4a656));
		lv_style_set_text_font(&color_combo, &fixedsys);
		lv_style_set_text_align(&color_combo, LV_TEXT_ALIGN_CENTER);
		lv_obj_add_style(b_obj,&color_combo,LV_PART_MAIN | LV_STATE_DEFAULT);
		
		//	label obj on button obj
		lv_obj_t *l_obj = lv_label_create(b_obj);
		objects.obj2 = l_obj;
		lv_obj_set_pos(l_obj, 0, 0);
		lv_obj_set_size(l_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_align(l_obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_label_set_text(l_obj, "MAIN\nMENU");
		
		// 	ascii img obj
		lv_obj_t *ascii_obj = lv_label_create(obj);
		objects.obj3 = ascii_obj;
		lv_obj_set_pos(ascii_obj, 10, 0);
		lv_obj_set_size(ascii_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_label_set_text(ascii_obj, "         @@@@@@@@@@@     \n      @@@@@@@@@@@@@@@@   \n     @@@@@@@@@@@@@@@@@@@ \n    @@@@@@@@@@@@@@@@@@@@@\n    @@@@@@#%@%+=-=#%@@@@@\n   @@@@@@#=-:-==:-#*%%@@@\n    @@*%#::=*%+:::::-#%@@\n    @@-::::::::::::#%*@@@\n    @@@*:::::::::-::-*   \n     @@*-:::::------+%   \n    %=------------=#@@   \n   %%%%%%#***+++*% @@@   \n  %%%%%%%%%%@@@@   @@@   \n%%%%%%%%%%%%%%%%  @@@    ");
		lv_style_init(&char_color2);
		lv_style_set_text_color(&char_color2, lv_color_hex(0xffa83450));
		lv_obj_add_style(ascii_obj,&char_color2,LV_PART_MAIN | LV_STATE_DEFAULT);

		lv_disp_load_scr(obj);
}
