#ifndef __UTOP_H
#define __UTOP_H

#include "app_base.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>

#define UTOP_MAX_APPS   8
#define UTOP_TASK_BUF   256   /* buffer for vTaskList output */
 
/* top half view */
typedef enum {
    UTOP_TOP_MEM = 0,
    UTOP_TOP_NET,
} utop_top_view_t;
 
/* bottom half view */
typedef enum {
    UTOP_BOT_TASK = 0,
    UTOP_BOT_APP,
} utop_bot_view_t;
 
typedef struct utop_app_t {
    app_t           app_base;
 
    utop_top_view_t top_view;
    utop_bot_view_t bot_view;
 
    /* top section */
    lv_obj_t       *top_container;
    lv_obj_t       *lbl_top_toggle_mem;
    lv_obj_t       *lbl_top_toggle_net;
    lv_obj_t       *lbl_cpu_bar;
 
    /* mem view */
    lv_obj_t       *mem_container;
    lv_obj_t       *lbl_ram_bar;
    lv_obj_t       *lbl_ram_val;
    lv_obj_t       *lbl_fls_bar;
    lv_obj_t       *lbl_fls_val;
 
    /* net view */
    lv_obj_t       *net_container;
    lv_obj_t       *lbl_net;
 
    /* bottom section */
    lv_obj_t       *bot_container;
    lv_obj_t       *lbl_bot_toggle_tsk;
    lv_obj_t       *lbl_bot_toggle_app;
 
    /* task view */
    lv_obj_t       *tsk_container;
    lv_obj_t       *lbl_tsk_header;
    lv_obj_t       *lbl_tsk_list;      /* single label fed by vTaskList */
 
    /* app view */
    lv_obj_t       *app_container;
    lv_obj_t       *lbl_app_header;
    lv_obj_t       *lbl_app_rows[UTOP_MAX_APPS];
 
    /* refresh timer */
    lv_timer_t     *refresh_timer;
 
    /* CPU delta */
    uint32_t        last_total_runtime;
} utop_app_t;
 
utop_app_t* create_utop_app(void);
 
#endif // __UTOP_APP_H
