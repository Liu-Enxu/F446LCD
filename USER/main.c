//#include "led.h"
#include "delay.h"
//#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"

#include "spi.h"
//#include "sd.h"
#include "malloc.h"
//#include "ff.h"  
//#include "exfuns.h" 
//#include "adc.h"
//#include "piclib.h"

#include "fatsd.h"
//#include "gui.h"

#include "timer.h"
#include "touch.h"
#include "lvgl/lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"

#include "screens.h"


int main(void)
{	  
//		u16 t; //vu8 pressed;
//		vu16 len;	
//		u16 times=0;
//		u8 stringBuf[50];
		
//		// SD
//		u8* buf;u8* buf_temp;
//		u32 sd_size;
//		u8 *p=0;
//		// FATFS
//		u16 res;
//		// pic
//		u8 frame_idx = 0;
	
	// must haves ------------------------
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
//	TIM2_Int_Init(999,89); // 90M/(89+1)=1Mhz --> 1us, 9999+1-->10ms	//若使用RTOS则不需要TIMx！！！！
	printf("begin!\n");	
	
	lv_log_register_print_cb(my_log_cb);
	lv_init();
	
	lv_port_disp_init();       
//  lv_display_set_buffers(disp, buf_1_1, NULL, sizeof(buf_1_1), LV_DISPLAY_RENDER_MODE_PARTIAL); // MUST CALL THIS IN MAIN.C!!! WHY?? --- STACK SIZE NOT ENOUGH!!!!!!!!!!!!!!	
	lv_port_indev_init();
	
	create_screen_load();
//	ui_init();	
	
	printf("loop!\n");	
	 while(1) 
	{	
		lv_timer_handler();
		delay_ms(5);
	}
	

	
//	SPI1_Init();
//	mem_init();
	
// FATFS----------------------------------------------------------------------
//	fatsd_init();
//	sd_info();
//	debug_lines();
	
	
//	test_file();
	
//	piclib_init();
//	res = ai_load_picfile("0:/listen.gif",lcddev.width-220, 0, 220, 160, 1);
	
//	gui_init();gui_draw();

//	lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
//	lv_obj_set_size(switch_obj,50,25);
//	lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);
//  while(1) 
//	{			
//		if(times%10==0)get_touchXY();
//		
//		if(USART_RX_STA&0x8000){					   
//			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
//			LCD_ShowString(30,200,300,16,16,USART_RX_BUF);
//			for(t=0;t<len;t++)
//			{
//				USART_SendData(USART2, USART_RX_BUF[t]);
//				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
//			}
//			USART_RX_STA=0;
//		}else{
//			times++;
//		}
//		delay_ms(10);   
//	} 
}
