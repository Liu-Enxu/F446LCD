#include "menu.h"
#include <stdio.h>

/* ================================================================== */
/*  Internal helpers                                                    */
/* ================================================================== */
 
/** Return the first NULL slot index in a pointer array, or -1 if full. */
static int find_free_slot(void **arr, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        if (arr[i] == NULL) return (int)i;
    }
    printf("Error: array full (capacity %d)", len);
    return -1;
}


/* ================================================================== */
/*  create_menu                                                         */
/* ================================================================== */

menu_t* create_menu(lv_obj_t *parent, 
                    uint16_t menu_x, uint16_t menu_y, 
                    uint16_t menu_w, uint16_t menu_h)
{
    //  check for valid params
    if(parent==NULL || menu_w == 0 || menu_h == 0){
        printf("Invalid args in create_menu!\r\n");
        return NULL;
    }
    //  struct malloc
    menu_t* my_menu = lv_mem_alloc(sizeof(menu_t));
    if (my_menu == NULL){
        printf("Error allocating memory for menu_t!!\r\n");
        return NULL;
    }
    //  menu_obj
    my_menu->menu_obj = lv_menu_create(parent);
    lv_obj_set_pos(my_menu->menu_obj, menu_x, menu_y);
    lv_obj_set_size(my_menu->menu_obj, menu_w, menu_h);
    lv_obj_set_style_bg_opa(my_menu->menu_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(my_menu->menu_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    //  root_page_obj
    my_menu->root_page_obj = NULL;
    
    //  config menu properties
//    lv_obj_set_width(((lv_menu_t*)(my_menu->menu_obj))->sidebar, LV_PCT(50));
    // lv_obj_add_flag(lv_menu_get_sidebar_header(my_menu->menu_obj), LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lv_menu_get_main_header(my_menu->menu_obj), LV_OBJ_FLAG_HIDDEN);
    //  hide menu by default
    lv_obj_add_flag(my_menu->menu_obj, LV_OBJ_FLAG_HIDDEN);
    
    return my_menu;
}

/* ================================================================== */
/*  create_page                                                         */
/* ================================================================== */

page_t* create_page(menu_t *parent_menu, cont_t *parent_cont, uint8_t section_len)
{
    // check for valid args
    if (parent_menu == NULL || section_len == 0) {
        printf("Invalid args in create_page!!\r\n");
        return NULL;
    }
    // valid page vacancy
    if (parent_menu->root_page_obj != NULL && (parent_cont == NULL || parent_cont->cont_subpage != NULL)){
        printf("Neither root page or content subpage is empty!");
        return NULL;
    }
    // struct malloc
    page_t *my_page = lv_mem_alloc(sizeof(page_t));
    if (my_page == NULL) {
        printf("Error allocating memory for page_t!!\r\n");
        return NULL;
    }

    /* page_obj — created as a child of the menu widget */
    my_page->page_obj = lv_menu_page_create(parent_menu->menu_obj, NULL);
    // lv_obj_set_style_pad_hor(my_page->page_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);    //  ?
    lv_obj_set_style_bg_opa(my_page->page_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(my_page->page_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(my_page->page_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    /* section_len */
    my_page->section_len = section_len;
 
    /* menu_sections — array of section_t pointers, all NULL initially */
    my_page->menu_sections = lv_mem_alloc(section_len * sizeof(section_t *));
    if (my_page->menu_sections == NULL) {
        lv_obj_del(my_page->page_obj);
        lv_mem_free(my_page);
        printf("Error allocating memory for menu_sections in page_t!!\r\n");
        return NULL;
    }
    for (uint8_t i = 0; i < section_len; i++) {
        my_page->menu_sections[i] = NULL;
    }
    
    // nothing goes wrong, add page to menu
    /* If this is the first page, make it the root */
    if (parent_menu->root_page_obj == NULL) {
        //  set root page and sidebar
        parent_menu->root_page_obj = my_page;
        // sidebar setting
        lv_menu_set_sidebar_page(parent_menu->menu_obj, my_page->page_obj);
		lv_menu_set_page(parent_menu->menu_obj, NULL);
		lv_obj_set_width(((lv_menu_t*)(parent_menu->menu_obj))->sidebar, LV_PCT(50));
        lv_obj_add_flag(lv_menu_get_sidebar_header(parent_menu->menu_obj), LV_OBJ_FLAG_HIDDEN);
    } else if (parent_cont != NULL && parent_cont->cont_subpage == NULL) {
        parent_cont->cont_subpage = my_page;
        /* Wire up LVGL navigation so tapping this cont opens the sub-page */
        lv_menu_set_load_page_event(
            parent_menu->menu_obj,
            parent_cont->cont_obj,
            my_page->page_obj
        );
    }
 
    return my_page;
}

/* ================================================================== */
/*  create_section                                                      */
/* ================================================================== */
 
section_t* create_section(page_t *parent_page, uint8_t cont_len){
    
    /* check for valid params */
    if (parent_page == NULL || cont_len == 0) {
        printf("Invalid args in create_section!\r\n");
        return NULL;
    }

    // check for vacancy
    int slot = find_free_slot((void **)parent_page->menu_sections, parent_page->section_len);
    if (slot < 0) {
        printf("Error: page section array is full (capacity %d)!!\r\n",
               parent_page->section_len);
        return NULL;
    }

    // struct malloc
    section_t* my_section = lv_mem_alloc(sizeof(section_t));
    if (my_section == NULL){
        printf("Error allocating memory for section_t!!\r\n");
        return NULL;
    } 

    //  section_obj
    my_section->section_obj = lv_menu_section_create(parent_page->page_obj);
    lv_obj_add_flag(my_section->section_obj, LV_OBJ_FLAG_EVENT_BUBBLE);

    //  cont_len
    my_section->cont_len = cont_len;
    //  section_conts
    my_section->section_conts = lv_mem_alloc(cont_len*sizeof(cont_t*));
    if (my_section->section_conts == NULL){
        lv_obj_del(my_section->section_obj);
        lv_mem_free(my_section);
        printf("Error allocating memory for section_conts in certain section_t!!\r\n");
        return NULL;
    } 
    for (uint8_t i = 0; i < cont_len; i++) {
        my_section->section_conts[i] = NULL;
    }

    // nothing goes wrong, add section to page
    parent_page->menu_sections[slot] = my_section;

    return my_section;
}

/* ================================================================== */
/*  create_content                                                      */
/* ================================================================== */

cont_t* create_content(section_t *parent_sect, uint8_t cont_h){
    
    /* check for valid params */
    if (parent_sect == NULL) {
        printf("Invalid args in create_content!\r\n");
        return NULL;
    }

    //  check for vacancy
    int slot = find_free_slot((void **)parent_sect->section_conts, parent_sect->cont_len);
    if (slot < 0) {
        printf("Error: section cont array is full (capacity %d)!!\r\n",
               parent_sect->cont_len);
        return NULL;
    }

    //  struct malloc
    cont_t *my_cont = lv_mem_alloc(sizeof(cont_t)); // need to be freed lv_mem_free()...
    if (my_cont == NULL){
        printf("Error allocating memory for my_cont!!\r\n");
        return NULL;
    }

    //  cont_obj create
    my_cont->cont_obj = lv_menu_cont_create(parent_sect->section_obj);

    // cont_obj properties
    lv_obj_set_height(my_cont->cont_obj, cont_h);
    lv_obj_set_flex_align(my_cont->cont_obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_style(my_cont->cont_obj,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(my_cont->cont_obj,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(my_cont->cont_obj,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(my_cont->cont_obj,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_PRESSED | LV_STATE_CHECKED);
	// lv_obj_add_style(my_cont->cont_obj,&lv_app_styles.color_combo1, LV_PART_MAIN | LV_STATE_ANY);
    lv_obj_set_style_bg_opa(my_cont->cont_obj, LV_OPA_0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_clear_flag(my_cont->cont_obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(my_cont->cont_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    //  cont_attach
    my_cont->cont_attach = NULL;
    //  cont_subpage
    my_cont->cont_subpage = NULL;

    // nothing goes wrong, add cont_obj to section
    parent_sect->section_conts[slot] = my_cont;

    return my_cont;
}
 

/* ================================================================== */
/*  create_cont_lbl                                                   */
/* ================================================================== */

lv_obj_t* create_cont_lbl(cont_t* parent_cont, const char* cont_l){
    /* check for valid params */
    if (parent_cont == NULL || cont_l == NULL) {
        printf("Invalid args in create_cont_lbl!\r\n");
        return NULL;
    }

    //  check for vacancy
    if (parent_cont->cont_attach != NULL){
        printf("Content already has attachment!");
        return NULL;
    }
        
    // create label
    lv_obj_t *my_cont_label = lv_label_create(parent_cont->cont_obj);
    lv_label_set_long_mode(my_cont_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(my_cont_label, 1);   //  expand to fill the remaining horizontal space in the flex
    // lv_obj_set_flex_align(my_cont_label, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_label_set_text(my_cont_label, cont_l);
    lv_obj_add_style(my_cont_label,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(my_cont_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(my_cont_label, LV_OBJ_FLAG_EVENT_BUBBLE);

    return my_cont_label;
}

/* ================================================================== */
/*  create_cont_btn                                                   */
/* ================================================================== */

lv_obj_t* create_cont_btn(cont_t* parent_cont, const char* cont_bl, void(*lv_callback)(lv_event_t* e), void* lv_data){
    /* check for valid params */
    if (parent_cont == NULL || cont_bl == NULL) {
        printf("Invalid args in create_cont_lbl!\r\n");
        return NULL;
    }

    //  check for vacancy
    if (parent_cont->cont_attach != NULL){
        printf("Content already has attachment!");
        return NULL;
    }

    // create button
    lv_obj_t *my_cont_btn = lv_btn_create(parent_cont->cont_obj);
    lv_obj_add_style(my_cont_btn,&lv_app_styles.color_combo1,LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(my_cont_btn, LV_OBJ_FLAG_EVENT_BUBBLE);

    // create button label
    lv_obj_t *my_cont_btn_l = lv_label_create(my_cont_btn);
    lv_label_set_long_mode(my_cont_btn_l, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(my_cont_btn_l, 1);   //  expand to fill the remaining horizontal space in the flex
    // lv_obj_set_flex_align(my_cont_btn_l, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_label_set_text(my_cont_btn_l, cont_bl);
    // lv_obj_add_style(my_cont_btn_l,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(my_cont_btn_l, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(my_cont_btn_l, LV_OBJ_FLAG_EVENT_BUBBLE);

    // attach button callback
    lv_obj_add_event_cb(my_cont_btn, lv_callback, LV_EVENT_ALL, lv_data);
    return my_cont_btn;


}



/* ================================================================== */
/*  Cleanup                                                             */
/* ================================================================== */
 
void free_content(cont_t *cont)
{
    if (cont == NULL) return;
    /* LVGL objects are deleted with the parent; only free our struct */
    if (cont->cont_subpage != NULL) {
        free_page(cont->cont_subpage);  // recursively free subpage tree
    }
    lv_mem_free(cont);
}
 
void free_section(section_t *section)
{
    if (section == NULL) return;
    for (uint8_t i = 0; i < section->cont_len; i++) {
        free_content(section->section_conts[i]);
    }
    lv_mem_free(section->section_conts);
    lv_mem_free(section);
}
 
void free_page(page_t *page)
{
    if (page == NULL) return;
    for (uint8_t i = 0; i < page->section_len; i++) {
        free_section(page->menu_sections[i]);
    }
    lv_mem_free(page->menu_sections);
    lv_mem_free(page);
}
 
void free_menu(menu_t *my_menu)
{
    if (my_menu == NULL) return;
    /* Deleting the LVGL menu object cascades to all child LVGL objects */
    lv_obj_del(my_menu->menu_obj);
    free_page(my_menu->root_page_obj);
    lv_mem_free(my_menu);
}

