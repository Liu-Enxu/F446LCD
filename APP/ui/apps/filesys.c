#include "filesys.h"
#include "lv_app_conf.h"

u8 file_cnt = 0;
char* pathName;

u8 fatsd_init(void)
{
	u16 res;
	exfuns_init();
	res = f_mount(fs[0],"0:",1);
	if(res){
		printf("f_mount failed: %d\r\n", res);
	}else{
		res = f_chdir("/");
		if(res){
			printf("f_chdir failed: %d\r\n", res);
		}
	}
	peri_status.SD = (res==0)?1:0;
	return res;
}

// Open a file for reading
static u16 fatsd_open(const TCHAR *path)
{
	u8 res;
	res = f_open(file, path, FA_READ);
	// LCD_ShowString(0,300,200,16,16,"                      ");
	// LCD_ShowNum(0,300,res,2,16);
	if(res){	
		// LCD_ShowString(20,300,200,16,16,"File open failure!");
	}else{
		// LCD_ShowString(20,300,200,16,16,"File open success!");
	}
	return res;
}

// Read a specified number of bytes from the file into the buffer
static u16 fatsd_read(UINT length)
{
	u16 res;
	res = f_read(file,fatbuf,length,&br);
	// LCD_ShowString(0,300,200,16,16,"                      ");
	if(res){	
		// LCD_ShowString(20,300,200,16,16,"File reading failure!");
	}else if(br!=length){
		// LCD_ShowString(20,300,200,16,16,"File read incomplete!");
		res = 20;
	}else{
		// LCD_ShowString(20,300,200,16,16,"File read success!");
	}
	// LCD_ShowNum(0,300,res,2,16);
	return res;
}

// check or create folder
static u16 fatsd_dir(const TCHAR* path, u8 CHK0CRT1DEL2)
{
	u16 res;
	if(CHK0CRT1DEL2 > 2) return FR_INVALID_PARAMETER;

	res = f_stat(path,&fileinfo);
	// LCD_ShowString(0,300,200,16,16,"                      ");
	// LCD_ShowNum(0,300,res,2,16);
	if(!res){
		if(CHK0CRT1DEL2 == 2){
			res = f_unlink(path);
		}
	}else{
		if(CHK0CRT1DEL2 == 0){
			// LCD_ShowString(20,300,200,16,16,"No such folder!");
		}else if(CHK0CRT1DEL2 == 1){
			res = f_mkdir(path);
			if(res){
				// LCD_ShowString(20,300,200,16,16,"Folder creation failure!");
			}else{
				// LCD_ShowString(20,300,200,16,16,"Folder creation success!");
			}
		}
	}
	return res;
}

// check or create file
static u16 fatsd_file(const TCHAR* path, u8 CHK0CRT1DEL2)
{
	u16 res;
	if(CHK0CRT1DEL2 > 2) return FR_INVALID_PARAMETER;

	res = f_stat(path,&fileinfo);
	// LCD_ShowString(0,300,200,16,16,"                      ");
	// LCD_ShowNum(0,300,res,2,16);
	if(!res){
		if(CHK0CRT1DEL2 == 2){
			res = f_unlink(path);
		}
	}else{
		if(CHK0CRT1DEL2 == 0){
			// LCD_ShowString(20,300,200,16,16,"No such file!");
		}else if(CHK0CRT1DEL2 == 1){
			res = f_open(file, path, FA_CREATE_NEW);
			// LCD_ShowString(0,300,200,16,16,"                      ");
			// LCD_ShowNum(0,300,20,2,16);
			if(res){	
				// LCD_ShowString(20,300,200,16,16,"File creation failure!");
			}else{
				// LCD_ShowString(20,300,200,16,16,"File creation success!");
				res = f_close(file);
			}
		}
	}
	return res;
}

static void fatsd_stringAppend(char* chars1,char* chars2)
{
	u8 len1 = (u8)strlen(chars1);
	u8 len2 = (u8)strlen(chars2);
	u8 len3 = len1+len2;
	pathName = (char*)pvPortRealloc(chars1,len3+1);
	memcpy(pathName+len1,chars2,len2);
	pathName[len3] = '\0';	
}

static u16 test_file(void)
{
	u16 res;
	static u8 err_cnt = 0;
	res = fatsd_dir("/test_folder",1);
	while(res){
		err_cnt++;
		printf("Failed to create test_folder, retrying...\r\n");
		res = fatsd_dir("/test_folder",1);
		if(err_cnt > 3){
			printf("Failed to create test_folder after 5 attempts, giving up.\r\n");
			return res;
		}
		vTaskDelay(10);
	}
		
	if(f_stat("/test_folder/hello.txt",&fileinfo)){
		res = f_open(file, "/test_folder/hello.txt", FA_CREATE_NEW | FA_WRITE);
	}else{
		res = f_open(file, "/test_folder/hello.txt", FA_WRITE);
	}
	if(res){
		// LCD_ShowString(30,190,300,16,16,"File creation/write failure!");
		return res;
	}
	
	f_write(file, "Hello, World!\r\n", 15, &bw);
	if (bw != 15){
		// LCD_ShowString(30,190,300,16,16,"File writing failure!");
		return 20;
	}
	f_close(file);
	return 0;
}

static void filesys_item_event_handler(lv_event_t* e);

static lv_obj_t* filesys_list_add_item(filesys_app_t* filesys,
								const char* name,
								u8 is_dir)
{
	if (filesys == NULL || filesys->fs_list_obj == NULL || name == NULL) {
		return NULL;
	}

	if (lv_obj_get_child_cnt(filesys->fs_list_obj) >= FILESYS_VISIBLE_ROWS) {
		return NULL;
	}

	const char* icon = is_dir ? "\xEF\x81\xBC" : "\xEF\x85\x9C";
	lv_obj_t* btn = lv_list_add_btn(filesys->fs_list_obj, icon, name);
	lv_obj_add_style(btn, &lv_app_styles.list_btn, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(btn, &lv_app_styles.list_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_add_style(btn, &lv_app_styles.tab_item_checked, LV_PART_MAIN | LV_STATE_CHECKED);
	lv_obj_set_height(btn, FILESYS_ROW_HEIGHT);
	lv_obj_set_user_data(btn, (void*)(uintptr_t)(is_dir != 0));
	lv_obj_add_event_cb(btn, filesys_item_event_handler, LV_EVENT_CLICKED, filesys);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);

	/* The shared button style uses the symbol font; restore the filename font. */
	uint32_t child_count = lv_obj_get_child_cnt(btn);
	if (child_count > 0) {
		lv_obj_t* name_label = lv_obj_get_child(btn, child_count - 1);
		lv_obj_add_style(name_label, &lv_app_styles.char_color1,
						 LV_PART_MAIN | LV_STATE_DEFAULT);
	}

	return btn;
}

static FRESULT filesys_list_update(filesys_app_t* filesys)
{
	if (filesys == NULL || filesys->fs_list_obj == NULL) {
		return FR_INVALID_OBJECT;
	}

	lv_obj_clean(filesys->fs_list_obj);
	filesys->selected_item = NULL;
	filesys->selected_is_dir = 0;
	file_cnt = 0;

	DIR directory;
	FILINFO entry;
#if _USE_LFN
	TCHAR long_name[_MAX_LFN + 1];
	entry.lfname = long_name;
	entry.lfsize = sizeof(long_name) / sizeof(long_name[0]);
#endif
	FRESULT result = f_opendir(&directory, filesys->current_path);
	if (result != FR_OK) {
		return result;
	}

	while (file_cnt < FILESYS_VISIBLE_ROWS) {
		result = f_readdir(&directory, &entry);
		if (result != FR_OK || entry.fname[0] == '\0') {
			break;
		}

		const TCHAR* display_name = entry.fname;
#if _USE_LFN
		if (entry.lfname[0] != '\0') {
			display_name = entry.lfname;
		}
#endif

		if (filesys_list_add_item(filesys, display_name,
								  (entry.fattrib & AM_DIR) != 0) == NULL) {
			break;
		}
		file_cnt++;
	}

	f_closedir(&directory);
	return result;
}

static void filesys_set_selected_item(filesys_app_t* filesys, lv_obj_t* item)
{
	uint32_t child_count = lv_obj_get_child_cnt(filesys->fs_list_obj);
	for (uint32_t i = 0; i < child_count; i++) {
		lv_obj_clear_state(lv_obj_get_child(filesys->fs_list_obj, i), LV_STATE_CHECKED);
	}

	if (item != NULL) {
		lv_obj_add_state(item, LV_STATE_CHECKED);
		filesys->selected_item = item;
		filesys->selected_is_dir = (u8)(uintptr_t)lv_obj_get_user_data(item);
	} else {
		filesys->selected_item = NULL;
		filesys->selected_is_dir = 0;
	}
}

static void filesys_move_selection(filesys_app_t* filesys, int direction)
{
	uint32_t child_count = lv_obj_get_child_cnt(filesys->fs_list_obj);
	if (child_count == 0) {
		filesys_set_selected_item(filesys, NULL);
		return;
	}

	int32_t index;
	if (filesys->selected_item == NULL) {
		index = (direction < 0) ? (int32_t)child_count - 1 : 0;
	} else {
		index = (int32_t)lv_obj_get_index(filesys->selected_item) + direction;
		if (index < 0) index = (int32_t)child_count - 1;
		if (index >= (int32_t)child_count) index = 0;
	}

	filesys_set_selected_item(filesys,
		lv_obj_get_child(filesys->fs_list_obj, index));
}

static void filesys_item_event_handler(lv_event_t* e)
{
	filesys_app_t* filesys = (filesys_app_t*)lv_event_get_user_data(e);
	filesys_set_selected_item(filesys, lv_event_get_target(e));
}

static FRESULT filesys_find_unused_name(const char* prefix,
										char* name,
										size_t name_size)
{
	if (prefix == NULL || name == NULL || name_size == 0) {
		return FR_INVALID_PARAMETER;
	}

	FILINFO info;
	lv_memset_00(&info, sizeof(info));

	for (uint16_t id = 0; id < UINT16_MAX; id++) {
		int length = snprintf(name, name_size, "%s(%u)", prefix, (unsigned int)id);
		if (length < 0 || (size_t)length >= name_size) {
			return FR_INVALID_NAME;
		}

		FRESULT result = f_stat(name, &info);
		if (result == FR_NO_FILE || result == FR_NO_PATH) {
			return FR_OK;
		}
		if (result != FR_OK) {
			return result;
		}
	}

	return FR_EXIST;
}

static FRESULT filesys_find_unused_full_path(filesys_app_t* filesys,
											 const char* prefix,
											 char* path,
											 size_t path_size)
{
	if (filesys == NULL || prefix == NULL || path == NULL || path_size == 0) {
		return FR_INVALID_PARAMETER;
	}

	FILINFO info;
	lv_memset_00(&info, sizeof(info));
	size_t current_len = strlen(filesys->current_path);
	const char* separator = (current_len > 0 &&
		filesys->current_path[current_len - 1] == '/') ? "" : "/";

	for (uint16_t id = 0; id < UINT16_MAX; id++) {
		int length = snprintf(path, path_size, "%s%s%s(%u)",
			filesys->current_path, separator, prefix, (unsigned int)id);
		if (length < 0 || (size_t)length >= path_size) {
			return FR_INVALID_NAME;
		}
		printf("path is: %s\r\n",path);

		FRESULT result = f_stat(path, &info);
		if (result == FR_NO_FILE || result == FR_NO_PATH) {
			return FR_OK;
		}
		if (result != FR_OK) {
			return result;
		}
	}

	return FR_EXIST;
}

static void op_button_event_handler(lv_event_t* e) {
	if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

	lv_obj_t* target = lv_event_get_target(e);
	filesys_app_t* filesys = (filesys_app_t*)lv_event_get_user_data(e);
	filesys_op_t op = (filesys_op_t)lv_obj_get_index(target);

	switch (op) {
		case FILESYS_OP_BACK:
			printf("Back pressed\r\n");
			break;
		case FILESYS_OP_UP:
			filesys_move_selection(filesys, -1);
			break;
		case FILESYS_OP_SELECT:
			if (filesys->selected_item != NULL && filesys->selected_is_dir) {
				const char* name = lv_list_get_btn_text(filesys->fs_list_obj,
					filesys->selected_item);
				FRESULT result = f_chdir(name);
				if (result == FR_OK) {
					result = f_getcwd(filesys->current_path, FILESYS_PATH_MAX);
				}
				if (result == FR_OK) {
					lv_label_set_text(filesys->path_label, filesys->current_path);
					result = filesys_list_update(filesys);
				}
				if (result != FR_OK) {
					printf("Failed to enter folder: %d\r\n", result);
				}
			}
			break;
		case FILESYS_OP_DOWN:
			filesys_move_selection(filesys, 1);
			break;
		case FILESYS_OP_ADD_FOLDER:
		{
			char path[FILESYS_PATH_MAX];
			FRESULT result = filesys_find_unused_full_path(filesys, "new_folder",
													 path, sizeof(path));
			if (result == FR_OK) {
				result = (FRESULT)fatsd_dir(path, 1);
			}
			if (result == FR_OK) {
				filesys_list_update(filesys);
			} else {
				printf("Failed to create folder: %d\r\n", result);
			}
			break;
		}
		case FILESYS_OP_ADD_FILE:
		{
			char name[FILESYS_PATH_MAX];
			FRESULT result = filesys_find_unused_name("new_file", name, sizeof(name));
			if (result == FR_OK) {
				result = (FRESULT)fatsd_file(name, 1);
			}
			if (result == FR_OK) {
				filesys_list_update(filesys);
			} else {
				printf("Failed to create file: %d\r\n", result);
			}
			break;
		}
		case FILESYS_OP_DELETE:
			if (filesys->selected_item != NULL) {
				const char* name = lv_list_get_btn_text(filesys->fs_list_obj,
													 filesys->selected_item);
				FRESULT result = filesys->selected_is_dir
					? (FRESULT)fatsd_dir(name, 2)
					: (FRESULT)fatsd_file(name, 2);

				if (result == FR_OK) {
					filesys_list_update(filesys);
				} else {
					printf("Failed to delete item: %d\r\n", result);
				}
			}
			break;
		default:
			printf("Unknown operation button\r\n");
			break;
	}
}

static void filesys_app_load(app_t* self, lv_obj_t* parent) {
    filesys_app_t* filesys = (filesys_app_t*)self;

	// if(peri_status.SD == 0 || test_file()){
	// 	printf("SD not inited or test_file failed\r\n");
	// }
	
	// curr path
	// strcpy(filesys->current_path, "0:/");
	if(peri_status.SD == 0){
		strcpy(filesys->current_path, "SD not inited");
	} else if(f_getcwd(filesys->current_path, FILESYS_PATH_MAX)){
		strcpy(filesys->current_path, "unknown path error");
	}

    // file path label
	filesys->path_label = lv_label_create(parent);
	lv_label_set_text(filesys->path_label, filesys->current_path);
	lv_obj_set_size(filesys->path_label, 480, 20);
	lv_obj_set_y(filesys->path_label, 2);
	
	lv_obj_add_style(filesys->path_label,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_align(filesys->path_label, LV_ALIGN_TOP_LEFT, 0);
	lv_obj_add_flag(filesys->path_label, LV_OBJ_FLAG_EVENT_BUBBLE);

	// fs operation button list
	filesys->list_op_btn = lv_list_create(parent);
    lv_obj_set_size(filesys->list_op_btn, 100, 245);
    lv_obj_set_flex_flow(filesys->list_op_btn, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_pad_all(filesys->list_op_btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_row(filesys->list_op_btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	
	lv_obj_set_style_align(filesys->path_label, LV_ALIGN_TOP_LEFT, 0);
	lv_obj_align(filesys->list_op_btn, LV_ALIGN_TOP_LEFT, 0, 25);
	lv_obj_add_style(filesys->list_op_btn, &lv_app_styles.color_combo1, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(filesys->list_op_btn, &lv_app_styles.sym_font, LV_PART_MAIN | LV_STATE_DEFAULT);
	// lv_obj_set_style_bg_opa(filesys->list_op_btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT); // by default
	// lv_style_set_border_width(filesys->list_op_btn, 0);

	lv_obj_clear_flag(filesys->list_op_btn, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_flag(filesys->list_op_btn, LV_OBJ_FLAG_EVENT_BUBBLE);

	lv_obj_t* btn = NULL;
	// last folder button
	btn = lv_list_add_btn(filesys->list_op_btn, NULL, "\xEF\x81\xA0"); // left arrow F060
	lv_obj_add_style(btn, &lv_app_styles.list_btn, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(btn, &lv_app_styles.list_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_height(btn, 0);
	lv_obj_set_flex_grow(btn, 1);
	lv_obj_add_event_cb(btn, op_button_event_handler, LV_EVENT_ALL, filesys);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
	// up button
	btn = lv_list_add_btn(filesys->list_op_btn, NULL, "\xEF\x81\xA2"); // up arrow F062
	lv_obj_add_style(btn, &lv_app_styles.list_btn, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(btn, &lv_app_styles.list_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_height(btn, 0);
	lv_obj_set_flex_grow(btn, 1);
    lv_obj_add_event_cb(btn, op_button_event_handler, LV_EVENT_ALL, filesys);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
	// select button
	btn = lv_list_add_btn(filesys->list_op_btn, NULL, "\xEF\x81\xA6"); // F066
	lv_obj_add_style(btn, &lv_app_styles.list_btn, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(btn, &lv_app_styles.list_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_height(btn, 0);
	lv_obj_set_flex_grow(btn, 1);
	lv_obj_add_event_cb(btn, op_button_event_handler, LV_EVENT_ALL, filesys);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
	// down button
	btn = lv_list_add_btn(filesys->list_op_btn, NULL, "\xEF\x81\xA3"); // down arrow F063
	lv_obj_add_style(btn, &lv_app_styles.list_btn, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(btn, &lv_app_styles.list_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_height(btn, 0);
	lv_obj_set_flex_grow(btn, 1);
	lv_obj_add_event_cb(btn, op_button_event_handler, LV_EVENT_ALL, filesys);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
	// add folder button
	btn = lv_list_add_btn(filesys->list_op_btn, NULL, "\xEF\x81\xBC"); // folder icon F07C
	lv_obj_add_style(btn, &lv_app_styles.list_btn, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(btn, &lv_app_styles.list_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_height(btn, 0);
	lv_obj_set_flex_grow(btn, 1);
	lv_obj_add_event_cb(btn, op_button_event_handler, LV_EVENT_ALL, filesys);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
	// add file button
	btn = lv_list_add_btn(filesys->list_op_btn, NULL, "\xEF\x85\x9C"); // file icon F15C
	lv_obj_add_style(btn, &lv_app_styles.list_btn, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(btn, &lv_app_styles.list_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_height(btn, 0);
	lv_obj_set_flex_grow(btn, 1);
	lv_obj_add_event_cb(btn, op_button_event_handler, LV_EVENT_ALL, filesys);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
	// delete button
	btn = lv_list_add_btn(filesys->list_op_btn, NULL, "\xEF\x87\xB8"); // trashcan icon F068
	lv_obj_add_style(btn, &lv_app_styles.list_btn, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(btn, &lv_app_styles.list_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
	lv_obj_set_height(btn, 0);
	lv_obj_set_flex_grow(btn, 1);
	lv_obj_add_event_cb(btn, op_button_event_handler, LV_EVENT_ALL, filesys);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);




	// fs view
	filesys->fs_list_obj = lv_list_create(parent);
	
	lv_obj_set_size(filesys->fs_list_obj, 480-100, 245);
	lv_obj_clear_flag(filesys->fs_list_obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_all(filesys->fs_list_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_row(filesys->fs_list_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_align(filesys->fs_list_obj, LV_ALIGN_TOP_RIGHT, 0);
	lv_obj_align(filesys->fs_list_obj, LV_ALIGN_TOP_RIGHT, 0, 25);

	lv_obj_add_style(filesys->fs_list_obj,&lv_app_styles.char_color1,LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_flag(filesys->fs_list_obj, LV_OBJ_FLAG_EVENT_BUBBLE);
	if(peri_status.SD){
		FRESULT result = filesys_list_update(filesys);
		if(result != FR_OK) {
			printf("Failed to update filesystem list: %d\r\n", result);
		}
    }
}

static void filesys_app_exit(app_t* self) {
    filesys_app_t* filesys = (filesys_app_t*)self;
}

/* ================================================================== */
/*  Constructor                                                         */
/* ================================================================== */

filesys_app_t* create_filesys_app(void) {
    filesys_app_t* filesys = (filesys_app_t*)lv_mem_alloc(sizeof(filesys_app_t));
    if (filesys == NULL) {
        printf("Failed to allocate filesys_app_t\r\n");
        return NULL;
    }
    lv_memset_00(filesys, sizeof(filesys_app_t));

    assert_param(strlen("files") < sizeof(filesys->app_base.app_name));
    assert_param(strlen("\xEF\x81\xBC") < sizeof(filesys->app_base.app_icon));
    strcpy(filesys->app_base.app_name, "files");
    memcpy(filesys->app_base.app_icon, "\xEF\x81\xBC", sizeof("\xEF\x81\xBC"));   /* U+F07C file */
 
    filesys->app_base.app_t_load = filesys_app_load;
    filesys->app_base.app_t_exit = filesys_app_exit;

    return filesys;
}
