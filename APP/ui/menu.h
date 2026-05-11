#ifndef __MENU_H
#define __MENU_H

#include <lvgl/lvgl.h>
#include "lv_app_conf.h"

//	cross reference
// typedef struct {
// 	lv_obj_t *cont_obj;

// 	lv_obj_t *cont_label;	// content label
// 	page_t *cont_subpage;
// } cont_t;

// typedef struct {
// 	lv_obj_t *section_obj;

// 	uint8_t cont_len;	// number of contents
// 	cont_t **section_conts;		// array of contents
// } section_t;

// typedef struct{
// 	lv_obj_t *page_obj;	// page
	
// 	uint8_t section_len;	// number of sections
// 	section_t **menu_sections;	// array of ptrs of sections
// } page_t;

// typedef struct{
// 	lv_obj_t *menu_obj;
// 	page_t *root_page_obj;
// } menu_t;

typedef struct cont_t    cont_t;
typedef struct section_t section_t;
typedef struct page_t    page_t;
typedef struct menu_t    menu_t;
 
struct cont_t {
	lv_obj_t *cont_obj;
 
	lv_obj_t *cont_attach;	// content obj
	page_t   *cont_subpage;
};
 
struct section_t {
	lv_obj_t  *section_obj;
 
	uint8_t    cont_len;		// number of contents
	cont_t   **section_conts;	// array of contents
};
 
struct page_t {
	lv_obj_t   *page_obj;		// page
 
	uint8_t     section_len;	// number of sections
	section_t **menu_sections;	// array of ptrs of sections
};
 
struct menu_t {
	lv_obj_t *menu_obj;
	page_t   *root_page_obj;
};


/* ------------------------------------------------------------------ */
/*  Factory functions                                                   */
/* ------------------------------------------------------------------ */
 
/**
 * Allocate and initialise a menu widget.
 *
 * @param parent   LVGL parent object
 * @param menu_x   X position
 * @param menu_y   Y position
 * @param menu_w   Width
 * @param menu_h   Height
 * @return         Pointer to the new menu_t, or NULL on failure.
 */
menu_t* create_menu(lv_obj_t *parent,
                    uint16_t menu_x, uint16_t menu_y,
                    uint16_t menu_w, uint16_t menu_h);
 
/**
 * Allocate and initialise a page, bound to @p my_menu.
 * Sets my_menu->root_page_obj when it is the first page created.
 *
 * @param my_menu      Owner menu
 * @param section_len  Maximum number of sections this page will hold
 * @return             Pointer to the new page_t, or NULL on failure.
 */
page_t* create_page(menu_t *parent_menu, cont_t *parent_cont, uint8_t section_len);
 
/**
 * Allocate and initialise a section inside @p parent_page.
 *
 * @param parent_page  Owner page
 * @param cont_len     Maximum number of contents this section will hold
 * @return             Pointer to the new section_t, or NULL on failure.
 */
section_t* create_section(page_t *parent_page, uint8_t cont_len);

/**
 * Allocate and initialise a content row inside @p parent_sect.
 *
 * @param parent_sect  Owner section
 * @param my_cont_l    Label text
 * @return             Pointer to the new cont_t, or NULL on failure.
 */
cont_t* create_content(section_t *parent_sect, uint8_t cont_h);

/**
 * Create a label for content to use.
 *
 * @param cont_l		Label text
 * @return             	Pointer to label.
 */
lv_obj_t* create_cont_lbl(cont_t* parent_cont, const char* cont_l);

/**
 * Create a button for content to use.
 *
 * @param cont_bl		Label text on button
 * @return             	Pointer to button.
 */
lv_obj_t* create_cont_btn(cont_t* parent_cont, const char* cont_bl, void(*lv_callback)(lv_event_t* e), void* lv_data);


/* ------------------------------------------------------------------ */
/*  Cleanup                                                             */
/* ------------------------------------------------------------------ */
 
/** Recursively free a cont_t (does NOT free its sub-page). */
void free_content(cont_t *cont);
 
/** Recursively free a section_t and all its contents. */
void free_section(section_t *section);
 
/** Recursively free a page_t, all its sections, and their contents. */
void free_page(page_t *page);
 
/** Free the entire menu tree. */
void free_menu(menu_t *my_menu);
 
#endif /* __MENU_H */
 
