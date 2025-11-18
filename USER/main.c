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
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "ui.h"

//void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf)
//{
//    /* Show the rendered image on the display */
//        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
//				printf("Flush: x1=%d y1=%d x2=%d y2=%d\n", 
//           area->x1, area->y1, area->x2, area->y2);
//        int32_t x;
//        int32_t y;
//        for(y = area->y1; y <= area->y2; y++) {
//            for(x = area->x1; x <= area->x2; x++) {
//                /*Put a pixel to the display. For example:*/
//                LCD_Fast_DrawPoint(x, y, (u16)(*(px_buf+1)<<8|*px_buf));
//                px_buf+=2;
//            }
//        }

//    /* Indicate that the buffer is available.
//     * If DMA were used, call in the DMA complete interrupt. */
//    lv_display_flush_ready(disp);
//}

//int main(void)
//{
//		u8 i;
//		delay_init();NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//		uart_init(115200);TIM2_Int_Init(999,89);printf("begin!!\n");
//		LCD_Init();
//		
//		lv_log_register_print_cb(my_log_cb);
//    lv_init();
//		
//		/* physical display */
//    lv_display_t * display = lv_display_create(480, 320);

//    /* LVGL will render to this 1/10 screen sized buffer for 2 bytes/pixel */
//    static uint8_t buf[32*1024];
//    lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

//    /* This callback will display the rendered image */
//    lv_display_set_flush_cb(display, my_flush_cb);
//		
//		// create style
//		static lv_style_t st;
//		lv_style_init(&st);
//		//lv_obj_add_style(label,&st,LV_STATE_DEFAULT);
//		
//    /* Create widgets */
//    lv_obj_t * label = lv_label_create(lv_screen_active());
//    lv_label_set_text(label, "Hello LVGL!");
//		lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
//	
//		ui_init();
//		/* Make LVGL periodically execute its tasks */
//    while(1) {
//      lv_timer_handler();
//			delay_ms(5);
//    }
//}



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
	TIM2_Int_Init(999,89); // 180M/180=1Mhz --> 1us, 9999+1-->10ms
	printf("begin!\n");	
	
	lv_log_register_print_cb(my_log_cb);
	lv_init();
	
	lv_port_disp_init();       
//  lv_display_set_buffers(disp, buf_1_1, NULL, sizeof(buf_1_1), LV_DISPLAY_RENDER_MODE_PARTIAL); // MUST CALL THIS IN MAIN.C!!! WHY?? --- STACK SIZE NOT ENOUGH!!!!!!!!!!!!!!	
	lv_port_indev_init();
	
//	ui_init();

	lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
	lv_obj_set_size(switch_obj, 100,50);
	lv_obj_align(switch_obj, LV_ALIGN_LEFT_MID, 0, 0);

	lv_obj_t* spinner_obj = lv_spinner_create(lv_scr_act());
	lv_obj_set_size(spinner_obj, 100, 50);
	lv_obj_align(spinner_obj, LV_ALIGN_RIGHT_MID,0,0);
	
	
	
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
