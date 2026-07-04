#include "filesys.h"
#include "lv_app_conf.h"

u8 file_cnt = 0;
char* pathName;

u8 fatsd_init(void)
{
	u16 res;
	exfuns_init();
	res = f_mount(fs[0],"0:",1);
	peri_status.SD = (res==0)?1:0;
	return res;
}

u16 fatsd_open(const TCHAR *path)
{
	u8 res;
	res = f_open(file, path, FA_READ);
	LCD_ShowString(0,300,200,16,16,"                      ");
	// LCD_ShowNum(0,300,res,2,16);
	if(res){	
		// LCD_ShowString(20,300,200,16,16,"File open failure!");
	}else{
		// LCD_ShowString(20,300,200,16,16,"File open success!");
	}
	return res;
}

u16 fatsd_read(UINT length)
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

u16 fatsd_dir(const TCHAR* path, u8 CHK0CRT1)
{
	u16 res;
	res = f_stat(path,&fileinfo);
	// LCD_ShowString(0,300,200,16,16,"                      ");
	// LCD_ShowNum(0,300,res,2,16);
	if(!res){
		// LCD_ShowString(20,300,200,16,16,"Folder exists!");
	}else{
		if(!CHK0CRT1){
			// LCD_ShowString(20,300,200,16,16,"No such folder!");
		}else{
			res = f_mkdir(path);
			// LCD_ShowString(0,300,200,16,16,"                      ");
			// LCD_ShowNum(0,300,res,2,16);
			if(res){
				// LCD_ShowString(20,300,200,16,16,"Folder creation failure!");
			}else{
				// LCD_ShowString(20,300,200,16,16,"Folder creation success!");
			}
		}
	}
	return res;
}
	
u16 fatsd_file(const TCHAR* path, u8 CHK0CRT1)
{
	u16 res;
	res = f_stat(path,&fileinfo);
	// LCD_ShowString(0,300,200,16,16,"                      ");
	// LCD_ShowNum(0,300,res,2,16);
	if(!res){
		// LCD_ShowString(20,300,200,16,16,"File exists!");
	}else{
		if(!CHK0CRT1){
			// LCD_ShowString(20,300,200,16,16,"No such file!");
		}else{
			res = f_open(file, path, FA_CREATE_NEW);
			// LCD_ShowString(0,300,200,16,16,"                      ");
			// LCD_ShowNum(0,300,20,2,16);
			if(res){	
				// LCD_ShowString(20,300,200,16,16,"File creation failure!");
			}else{
				// LCD_ShowString(20,300,200,16,16,"File creation success!");
			}
		}
	}
	return res;
}

size_t my_strlen(const char* str) 
{
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void fatsd_stringAppend(char* chars1,char* chars2)
{
	u8 len1 = (u8)my_strlen(chars1);
	u8 len2 = (u8)my_strlen(chars2);
	u8 len3 = len1+len2;
	pathName = (char*)myrealloc(chars1,len3+1);
	mymemcpy(pathName+len1,chars2,len2);
	pathName[len3] = '\0';
	// LCD_ShowString(0,300,200,16,16,"                      ");
	// LCD_ShowNum(0,300,len1,2,16);
	// LCD_ShowNum(24,300,len2,2,16);
	// LCD_ShowNum(48,300,len3,2,16);
	// LCD_ShowString(72,300,200,16,16,pathName);
	
}
void sd_info(void)
{
	u32 total,free;

	// while(exf_getfree("0",&total,&free))	//得到SD卡的总容量和剩余容量
	// {
		// LCD_ShowString(0,300,200,16,16,"SD Card Fatfs Error!");
		// delay_ms(200);
		// LCD_ShowString(0,300,200,16,16,"Re-initializing!    ");	//清除显示			  
		// delay_ms(200);
	// }
	// LCD_ShowString(0,300,200,16,16,"FATFS OK!");	 
	// LCD_ShowString(30,30,300,24,24,"SD Total Size:     MB");	 
	// LCD_ShowString(30,60,300,24,24,"SD  Free Size:     MB"); 	    
 	// LCD_ShowNum(30+12*14,30,total>>10,5,24);				//显示SD卡总容量 MB
 	// LCD_ShowNum(30+12*14,60,free>>10,5,24);					//显示SD卡剩余容量 MB
}

u16 test_file(void)
{
	u16 res;
	fatsd_dir("0:/test_folder",1);
		
	if(f_stat("0:/test_folder/hello.txt",&fileinfo)){
		res = f_open(file, "0:/test_folder/hello.txt", FA_CREATE_NEW | FA_WRITE);
	}else{
		res = f_open(file, "0:/test_folder/hello.txt", FA_WRITE);
	}
	if(res){
		// LCD_ShowString(30,190,300,16,16,"File creation/write failure!");
		while(1);
	}
	
	f_write(file, "Hello, World!\r\n", 15, &bw);
	if (bw != 15){
		// LCD_ShowString(30,190,300,16,16,"File writing failure!");
		while(1);
	}
	f_close(file);
}


static void filesys_app_load(app_t* self, lv_obj_t* parent) {
    filesys_app_t* filesys = (filesys_app_t*)self;
    lv_obj_t* label = lv_label_create(parent);
    lv_obj_set_size(label, 480, 270);
    lv_obj_set_pos(label, 0, 0);
    lv_obj_add_style(label, &lv_app_styles.char_color1, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_EVENT_BUBBLE);
    if(0==peri_status.SD){   
        if(0==fatsd_init()){
            peri_status.SD = 1;
            lv_label_set_text(label, "SD card initialized successfully.");
        };
    } else {
        lv_label_set_text(label, "SD card already initialized.");
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