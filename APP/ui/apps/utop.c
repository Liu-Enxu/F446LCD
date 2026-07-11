#include "utop.h"
#include "lv_app_conf.h"
#include "tab.h"

#define HOR_RES         480
#define VER_RES         320
#define FLASH_TOTAL_KB  512
#define ROW_H           24
#define TOP_OFFSET      4
#define TOP_H           ((VER_RES-50) / 2 - 2)
#define BOT_H           ((VER_RES-50) / 2 - 2)
#define BOT_Y           ((VER_RES-50) / 2 + 2)

 
/* ================================================================== */
/*  Helpers                                                             */
/* ================================================================== */
 
static void make_bar(char* buf, uint8_t len, uint8_t pct) {
    uint8_t filled = pct / 10;
    char bar[11];
    for (uint8_t i = 0; i < 10; i++)
        bar[i] = (i < filled) ? '#' : '-';
    bar[10] = '\0';
    snprintf(buf, len, "[%s] %3u%%", bar, pct);
}
 
/* ================================================================== */
/*  Update functions                                                    */
/* ================================================================== */
 
static void update_cpu(utop_app_t* utop) {
    uint32_t total_runtime = 0;
    TaskStatus_t tasks[8];
    UBaseType_t count = uxTaskGetSystemState(tasks, 8, &total_runtime);
 
    uint32_t idle_runtime = 0;
    for (UBaseType_t i = 0; i < count; i++) {
        if (strncmp(tasks[i].pcTaskName, "IDLE", 4) == 0) {
            idle_runtime = tasks[i].ulRunTimeCounter;
            break;
        }
    }
 
    uint32_t delta_total = total_runtime - utop->last_total_runtime;
    uint8_t cpu_pct = (delta_total > 0) ?
        (uint8_t)(100 - (idle_runtime * 100 / total_runtime)) : 0;
    if (cpu_pct > 100) cpu_pct = 100;
    utop->last_total_runtime = total_runtime;
 
    char buf[32];
    make_bar(buf, sizeof(buf), cpu_pct);
    lv_label_set_text_fmt(utop->lbl_cpu_bar, "CPU %s", buf);
}
 
static void update_mem(utop_app_t* utop) {
    /* SRAM heap */
    uint32_t free_b  = xPortGetFreeHeapSize();
    uint32_t total_b = configTOTAL_HEAP_SIZE;
    uint32_t used_b  = total_b - free_b;
    uint8_t  ram_pct = (uint8_t)(used_b * 100 / total_b);
 
    char buf[32];
    make_bar(buf, sizeof(buf), ram_pct);
    lv_label_set_text_fmt(utop->lbl_ram_bar, "RAM %s", buf);
    lv_label_set_text_fmt(utop->lbl_ram_val, "%lu/%luB",
                          (unsigned long)used_b, (unsigned long)total_b);
 
    /* Flash hardcoded total */
    extern uint32_t Image$$ER_IROM1$$Length;
    uint32_t flash_kb = (uint32_t)&Image$$ER_IROM1$$Length / 1024;
    if (flash_kb > FLASH_TOTAL_KB) flash_kb = FLASH_TOTAL_KB;
    uint8_t fls_pct = (uint8_t)(flash_kb * 100 / FLASH_TOTAL_KB);
 
    make_bar(buf, sizeof(buf), fls_pct);
    lv_label_set_text_fmt(utop->lbl_fls_bar, "FLS %s", buf);
    lv_label_set_text_fmt(utop->lbl_fls_val, "%lu/%uKB",
                          (unsigned long)flash_kb, FLASH_TOTAL_KB);

    uint32_t sd_total_kb = 0;
    uint32_t sd_free_kb = 0;
    uint32_t sd_used_kb = 0;
    uint8_t sd_pct = 0;
    if (peri_status.SD){
        // sd_total_kb = SD_GetSectorCount() / 2; // SD_GetSectorCount() -> 512-byte sectors, divide by 2 to get KB
        if(exf_getfree((u8*)"0:", &sd_total_kb, &sd_free_kb)){
            sd_used_kb = sd_total_kb - sd_free_kb;
            sd_pct = (uint8_t)(sd_used_kb * 100 / sd_total_kb);
        } 
    }
    make_bar(buf, sizeof(buf), sd_pct);
    lv_label_set_text_fmt(utop->lbl_drv_bar, "DRV %s", buf);
    lv_label_set_text_fmt(utop->lbl_drv_val, "%lu/%uKB", (unsigned long)sd_used_kb, (unsigned long)sd_total_kb);
}

// static void update_net(utop_app_t* utop) {
//     lv_label_set_text_fmt(utop->lbl_net, "IP: %s", peri_status.ip);
// }

static void update_tasks(utop_app_t* utop) {
    static char task_buf[UTOP_TASK_BUF];
    char* src = NULL;
    char* dst = NULL;
    uint8_t line_cnt = 0;
    uint8_t curr_page_idx = 0;

    vTaskList(task_buf);
    // first count how many page possible
    // src = task_buf;
    // while (*src) {
    //     if(*src == '\r') {
    //         line_cnt++;
    //         if(line_cnt >= MAX_TASK_ROWS) {
    //             line_cnt = 0;
    //             if(*(src+2) != '\0') curr_page_idx++; // \r\n
    //         }
    //     }
    //     src++;
    // }
    utop->max_task_page_idx = (uint8_t)((unsigned long)uxTaskGetNumberOfTasks()/4); // just to ensure the task list is up to date
    

    // update
    src = task_buf;
    dst = task_buf;
    // line_cnt = 0;
    // curr_page_idx = 0;
    while (*src) {
        if(*src != '\n' && curr_page_idx == utop->task_page_idx) *dst++ = *src;
        if(*src == '\r') {
            line_cnt++;
            if(line_cnt == MAX_TASK_ROWS) {
                line_cnt = 0;
                curr_page_idx++;
                if(curr_page_idx > utop->task_page_idx) break; // we've filled the current page, exit loop
            }
        }
        src++;
    }
    *dst = '\0';

    lv_label_set_text(utop->lbl_tsk_list, task_buf);
}
 
static void update_apps(utop_app_t* utop) {
    app_t*  tmp = app_mgr_inst()->app_head;
    uint8_t row = 0;
    uint8_t app_cnt = 0;
    uint8_t start = utop->app_page_idx * MAX_APP_ROWS;

    // no tabview instance or no tabs, can't determine running apps
    if (get_app_tv_inst() == NULL || get_app_tv_inst()->tab_head == NULL) return; 

    // update max app page index
    utop->max_app_page_idx = app_mgr_inst()->app_registered;
    utop->max_app_page_idx = utop->max_app_page_idx ? (utop->max_app_page_idx - 1) / MAX_APP_ROWS : 0;

    // update app rows for the current page
    for (uint8_t i = 0; i < MAX_APP_ROWS; i++) {
        lv_label_set_text(utop->lbl_app_rows[i], "");
    }

    while (tmp != NULL && row < MAX_APP_ROWS) {
        // get app status
        const char* state = "idle";
        tab_t* tab = get_app_tv_inst()->tab_head->next_tab;
        while (tab != NULL) {
            if (tab->app_ptr == tmp) { state = "running"; break; }  // if match, then it's running
            tab = tab->next_tab;
        }
        // update label for this row
        if(app_cnt >= start){ // only update rows for the current page
            lv_label_set_text_fmt(utop->lbl_app_rows[row],"%-12s %s", tmp->app_name, state);
            row++;
        }
        app_cnt++;
        tmp = tmp->next_app;
    }
    // // clear remaining rows if any
    // if(row < MAX_APP_ROWS) {
    //     for (uint8_t i = row; i < MAX_APP_ROWS; i++) {
    //         if (utop->lbl_app_rows[i])
    //             lv_label_set_text(utop->lbl_app_rows[i], "");
    //     }
    // }
}
 
static void page_up_cb(lv_event_t* e) {
    // printf("Page Up button pressed\r\n");
    utop_app_t* utop = (utop_app_t*)lv_event_get_user_data(e);
    
    if(utop->bot_view == UTOP_BOT_TASK) {
        // check valid page idx
        if(utop->task_page_idx > 0){
            utop->task_page_idx--;
        } else {return;}
        // update task list to reflect new page index
        update_tasks(utop);
    } else if(utop->bot_view == UTOP_BOT_APP) {
        // check valid page idx
        if(utop->app_page_idx > 0){
            utop->app_page_idx--;
        } else {return;}
        // update app list to reflect new page index
        update_apps(utop);
    }
}

static void page_down_cb(lv_event_t* e) {
    // printf("Page Down button pressed\r\n");
    utop_app_t* utop = (utop_app_t*)lv_event_get_user_data(e);
    if(utop->bot_view == UTOP_BOT_TASK) {
        // check valid page idx
        if(utop->task_page_idx < utop->max_task_page_idx) {
            utop->task_page_idx++;
        } else {return;}
        // update task list to reflect new page index
        update_tasks(utop);
    } else if(utop->bot_view == UTOP_BOT_APP) {
        // check valid page idx
        if(utop->app_page_idx < utop->max_app_page_idx) {
            utop->app_page_idx++;
        } else {return;}
        // update app list to reflect new page index
        update_apps(utop);
    }
}

/* ================================================================== */
/*  Refresh timer                                                       */
/* ================================================================== */
 
static void utop_refresh_cb(lv_timer_t* t) {
    utop_app_t* utop = (utop_app_t*)t->user_data;
    update_cpu(utop);
    if (utop->top_view == UTOP_TOP_MEM) update_mem(utop);
    if (utop->bot_view == UTOP_BOT_TASK) update_tasks(utop);
    else update_apps(utop);
}
 
/* ================================================================== */
/*  Toggle callbacks                                                    */
/* ================================================================== */
 
static void top_toggle_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_RELEASED) return;
    utop_app_t* utop = (utop_app_t*)lv_event_get_user_data(e);
    utop->top_view = (utop->top_view == UTOP_TOP_MEM) ? UTOP_TOP_NET : UTOP_TOP_MEM;
    if (utop->top_view == UTOP_TOP_MEM) {
        lv_obj_clear_flag(utop->mem_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag  (utop->net_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_text_decor(utop->lbl_top_toggle_mem, LV_TEXT_DECOR_UNDERLINE, 0);
        lv_obj_set_style_text_decor(utop->lbl_top_toggle_net, LV_TEXT_DECOR_NONE, 0);
    } else {
        lv_obj_add_flag  (utop->mem_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(utop->net_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_text_decor(utop->lbl_top_toggle_mem, LV_TEXT_DECOR_NONE, 0);
        lv_obj_set_style_text_decor(utop->lbl_top_toggle_net, LV_TEXT_DECOR_UNDERLINE, 0);
    }
}
 
static void bot_toggle_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_RELEASED) return;
    utop_app_t* utop = (utop_app_t*)lv_event_get_user_data(e);
    utop->bot_view = (utop->bot_view == UTOP_BOT_TASK) ? UTOP_BOT_APP : UTOP_BOT_TASK;
    if (utop->bot_view == UTOP_BOT_TASK) {
        lv_obj_clear_flag(utop->tsk_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag  (utop->app_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_text_decor(utop->lbl_bot_toggle_tsk, LV_TEXT_DECOR_UNDERLINE, 0);
        lv_obj_set_style_text_decor(utop->lbl_bot_toggle_app, LV_TEXT_DECOR_NONE, 0);
    } else {
        lv_obj_add_flag  (utop->tsk_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(utop->app_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_text_decor(utop->lbl_bot_toggle_tsk, LV_TEXT_DECOR_NONE, 0);
        lv_obj_set_style_text_decor(utop->lbl_bot_toggle_app, LV_TEXT_DECOR_UNDERLINE, 0);
    }
}
 
/* ================================================================== */
/*  UI helpers                                                          */
/* ================================================================== */
 
static lv_obj_t* make_label(lv_obj_t* parent, int16_t y, const char* txt) {
    lv_obj_t* lbl = lv_label_create(parent);
    lv_label_set_text(lbl, txt);
    lv_obj_set_pos(lbl, 0, y);
    lv_obj_add_style(lbl, &lv_app_styles.char_color1, 0);
    lv_obj_add_flag(lbl, LV_OBJ_FLAG_EVENT_BUBBLE);
    return lbl;
}
 
static lv_obj_t* make_container(lv_obj_t* parent, int16_t y, uint16_t h) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_pos(cont, 0, y);
    lv_obj_set_size(cont, HOR_RES, h);
    lv_obj_add_style(cont, &lv_app_styles.no_deco, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_EVENT_BUBBLE);
    return cont;
}
 
static lv_obj_t* make_toggle_btn(lv_obj_t* parent, int16_t x, int16_t y,
                                  const char* txt, lv_event_cb_t cb, void* ud) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, LV_SIZE_CONTENT, ROW_H);
    lv_obj_add_style(btn, &lv_app_styles.color_combo1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(btn, 4, 0);
    lv_obj_set_style_radius(btn, 3, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_RELEASED, ud);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, txt);
    lv_obj_add_style(lbl, &lv_app_styles.char_color1, 0);
    lv_obj_add_flag(lbl, LV_OBJ_FLAG_EVENT_BUBBLE);
    return lbl;
}
 
/* ================================================================== */
/*  Load / exit                                                         */
/* ================================================================== */
 
static void utop_app_load(app_t* self, lv_obj_t* parent) {
    utop_app_t* utop = (utop_app_t*)self;
 
    /* ---- top container ---- */
    utop->top_container = make_container(parent, TOP_OFFSET, TOP_H);
 
    utop->lbl_top_toggle_mem = make_toggle_btn(utop->top_container,
        HOR_RES - 80, 0, "MEM", top_toggle_cb, utop);
    utop->lbl_top_toggle_net = make_toggle_btn(utop->top_container,
        HOR_RES - 40, 0, "NET", top_toggle_cb, utop);
    lv_obj_set_style_text_decor(utop->lbl_top_toggle_mem, LV_TEXT_DECOR_UNDERLINE, 0);
 
    utop->lbl_cpu_bar = make_label(utop->top_container, ROW_H,
        "CPU [----------]   0%");
 
    /* mem container */
    utop->mem_container = make_container(utop->top_container, ROW_H * 2, ROW_H * 3);
    utop->lbl_ram_bar = make_label(utop->mem_container, 0,        "RAM [----------]   0%");
    utop->lbl_ram_val = make_label(utop->mem_container, 0,        "");
    lv_obj_set_style_align(utop->lbl_ram_val, LV_ALIGN_TOP_RIGHT, 0);
    utop->lbl_fls_bar = make_label(utop->mem_container, ROW_H,    "FLS [----------]   0%");
    utop->lbl_fls_val = make_label(utop->mem_container, ROW_H,    "");
    lv_obj_set_y(utop->lbl_fls_val, ROW_H);
    lv_obj_set_style_align(utop->lbl_fls_val, LV_ALIGN_TOP_RIGHT, 0);
    utop->lbl_drv_bar = make_label(utop->mem_container, ROW_H*2,  "DRV [----------]   0%");
    utop->lbl_drv_val = make_label(utop->mem_container, ROW_H*2,  "");
    lv_obj_set_y(utop->lbl_drv_val, ROW_H*2);
    lv_obj_set_style_align(utop->lbl_drv_val, LV_ALIGN_TOP_RIGHT, 0);

 
    /* net container */
    utop->net_container = make_container(utop->top_container, ROW_H * 2, ROW_H * 3);
    utop->lbl_net = make_label(utop->net_container, 0, "WiFi:  N/A\nMQTT:  N/A");
    lv_obj_add_flag(utop->net_container, LV_OBJ_FLAG_HIDDEN);
 
    /* ---- divider ---- */
    lv_obj_t* div = lv_obj_create(parent);
    lv_obj_set_pos(div, 0, TOP_H+TOP_OFFSET);
    lv_obj_set_size(div, HOR_RES, 2);
    lv_obj_set_style_bg_color(div, lv_app_styles.color1, 0);
    lv_obj_set_style_bg_opa(div, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(div, 0, 0);
    lv_obj_set_style_radius(div, 0, 0);
    lv_obj_add_flag(div, LV_OBJ_FLAG_EVENT_BUBBLE);
 
    /* ---- bot container ---- */
    utop->bot_container = make_container(parent, BOT_Y+TOP_OFFSET, BOT_H);
 
    utop->lbl_bot_toggle_tsk = make_toggle_btn(utop->bot_container,
        HOR_RES - 80, 0, "TSK", bot_toggle_cb, utop);
    utop->lbl_bot_toggle_app = make_toggle_btn(utop->bot_container,
        HOR_RES - 40, 0, "APP", bot_toggle_cb, utop);
    // page up button
    utop->btn_page_up = lv_btn_create(utop->bot_container);
    lv_obj_set_pos(utop->btn_page_up, HOR_RES - 80, ROW_H*1.5);
    lv_obj_set_size(utop->btn_page_up, 80, ROW_H*1.5);
    lv_obj_add_style(utop->btn_page_up, &lv_app_styles.color_combo1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(utop->btn_page_up, page_up_cb, LV_EVENT_RELEASED, utop);
    lv_obj_add_flag(utop->btn_page_up, LV_OBJ_FLAG_EVENT_BUBBLE);
    // page up button label
    lv_obj_t* lbl_page_up = lv_label_create(utop->btn_page_up);
    lv_obj_add_style(lbl_page_up, &lv_app_styles.sym_font, 0);
    lv_obj_set_align(lbl_page_up, LV_ALIGN_CENTER);
    lv_label_set_text(lbl_page_up, "\xEF\x81\xA2");
    lv_obj_add_flag(lbl_page_up, LV_OBJ_FLAG_EVENT_BUBBLE);
    // page down button 
    utop->btn_page_down = lv_btn_create(utop->bot_container);
    lv_obj_set_pos(utop->btn_page_down, HOR_RES - 80, ROW_H*3.5);
    lv_obj_set_size(utop->btn_page_down, 80, ROW_H*1.5);
    lv_obj_add_style(utop->btn_page_down, &lv_app_styles.color_combo1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(utop->btn_page_down, page_down_cb, LV_EVENT_RELEASED, utop);
    lv_obj_add_flag(utop->btn_page_down, LV_OBJ_FLAG_EVENT_BUBBLE);
    // page down button label
    lv_obj_t* lbl_page_down = lv_label_create(utop->btn_page_down);
    lv_obj_add_style(lbl_page_down, &lv_app_styles.sym_font, 0);
    lv_obj_set_align(lbl_page_down, LV_ALIGN_CENTER);
    lv_label_set_text(lbl_page_down, "\xEF\x81\xA3");
    lv_obj_add_flag(lbl_page_down, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_text_decor(utop->lbl_bot_toggle_tsk, LV_TEXT_DECOR_UNDERLINE, 0);
 
    /* task container */
    utop->tsk_container = make_container(utop->bot_container, ROW_H, BOT_H - ROW_H);
    utop->lbl_tsk_header = make_label(utop->tsk_container, 0,
        "Name         State Pri HWM  #");
    utop->lbl_tsk_list = make_label(utop->tsk_container, ROW_H, "");
 
    /* app container */
    utop->app_container = make_container(utop->bot_container, ROW_H, BOT_H - ROW_H);
    utop->lbl_app_header = make_label(utop->app_container, 0, "Name          State");
    for (uint8_t i = 0; i < MAX_APP_ROWS; i++) {
        utop->lbl_app_rows[i] = make_label(utop->app_container, ROW_H * (i + 1)+2, "");
    }
    lv_obj_add_flag(utop->app_container, LV_OBJ_FLAG_HIDDEN);
    
    // move page up/down buttons to foreground to ensure they are clickable
    lv_obj_move_foreground(utop->btn_page_up);
    lv_obj_move_foreground(utop->btn_page_down);
    
    /* refresh timer */
    utop->refresh_timer = lv_timer_create(utop_refresh_cb, 1000, utop);
    lv_timer_ready(utop->refresh_timer);
}
 
static void utop_app_exit(app_t* self) {
    utop_app_t* utop = (utop_app_t*)self;
    if (utop->refresh_timer) {
        lv_timer_del(utop->refresh_timer);
        utop->refresh_timer = NULL;
    }
    // lv_mem_free(self);
}
 
/* ================================================================== */
/*  Constructor                                                         */
/* ================================================================== */
 
utop_app_t* create_utop_app(void) {
    utop_app_t* utop = lv_mem_alloc(sizeof(utop_app_t));
    if (utop == NULL) {
        printf("Failed to allocate utop_app_t\r\n");
        return NULL;
    }
    lv_memset_00(utop, sizeof(utop_app_t));
 
    assert_param(strlen("uTop") < sizeof(utop->app_base.app_name));
    assert_param(strlen("\xEF\x82\x80") < sizeof(utop->app_base.app_icon));
    strcpy(utop->app_base.app_name, "uTop");
    memcpy(utop->app_base.app_icon, "\xEF\x82\x80", sizeof("\xEF\x82\x80"));   /* U+F080 desktop monitor */
 
    utop->app_base.app_t_load = utop_app_load;
    utop->app_base.app_t_exit = utop_app_exit;
    utop->top_view = UTOP_TOP_MEM;
    utop->bot_view = UTOP_BOT_TASK;
    utop->task_page_idx = 0;
    utop->app_page_idx = 0;
 
    return utop;
}