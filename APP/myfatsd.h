#ifndef __FATSD_H
#define __FATSD_H

#include "exfuns.h"
#include "lcd.h"
#include "delay.h"
#include "malloc.h"
#include "sys.h"

#include "stdio.h"
#include "stdlib.h"




extern u8 file_cnt;
extern char* pathName;

void fatsd_init(void);

u16 fatsd_open(const TCHAR *path);

u16 fatsd_read(UINT length);

u16 fatsd_dir(const TCHAR* path, u8 CHK0CRT1);

u16 fatsd_file(const TCHAR* path, u8 CHK0CRT1);

void fatsd_stringAppend(char* chars1,char* chars2);

void sd_info(void);

u16 test_file(void);
#endif