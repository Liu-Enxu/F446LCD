#ifndef __FILESYS_H
#define __FILESYS_H

#include "app_base.h"
// #include "FreeRTOS.h"
// #include "task.h"
#include <lvgl/lvgl.h>
#include "sys.h"

#include "exfuns.h"
#include <stdint.h>
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

#define FILESYS_PATH_MAX 128
#define FILESYS_VISIBLE_ROWS 7
#define FILESYS_ROW_HEIGHT 35

typedef enum {
	FILESYS_OP_BACK = 0,
	FILESYS_OP_UP,
	FILESYS_OP_SELECT,
	FILESYS_OP_DOWN,
	FILESYS_OP_ADD_FOLDER,
	FILESYS_OP_ADD_FILE,
	FILESYS_OP_DELETE
} filesys_op_t;

typedef struct filesys_app_t filesys_app_t;

typedef struct filesys_app_t {
    app_t           app_base;
    char current_path[FILESYS_PATH_MAX];
    lv_obj_t *path_label;
    lv_obj_t *fs_list_obj;
    lv_obj_t *list_op_btn;
    lv_obj_t *selected_item;
    u8 selected_is_dir;
} filesys_app_t;
 
filesys_app_t* create_filesys_app(void);

// lv_obj_t* filesys_list_add_item(filesys_app_t* filesys,
                                // const char* name,
                                // u8 is_dir);
// FRESULT filesys_list_update(filesys_app_t* filesys);

extern u8 file_cnt;
extern char* pathName;

u8 fatsd_init(void);

#endif // __FILESYS_H
