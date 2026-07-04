#ifndef __FILESYS_H
#define __FILESYS_H

#include "app_base.h"
// #include "FreeRTOS.h"
// #include "task.h"
#include <lvgl/lvgl.h>
#include "sys.h"

#include "exfuns.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"

typedef struct filesys_app_t filesys_app_t;

typedef struct filesys_app_t {
    app_t           app_base;
    
} filesys_app_t;
 
filesys_app_t* create_filesys_app(void);

extern u8 file_cnt;
extern char* pathName;

u8 fatsd_init(void);

u16 fatsd_open(const TCHAR *path);

u16 fatsd_read(UINT length);

u16 fatsd_dir(const TCHAR* path, u8 CHK0CRT1);

u16 fatsd_file(const TCHAR* path, u8 CHK0CRT1);

void fatsd_stringAppend(char* chars1,char* chars2);

void sd_info(void);

u16 test_file(void);

#endif // __FILESYS_H
