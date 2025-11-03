//#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "timer.h"
#include "spi.h"
//#include "sd.h"
#include "malloc.h"
//#include "ff.h"  
//#include "exfuns.h" 
//#include "adc.h"
#include "piclib.h"

#include "fatsd.h"
#include "gui.h"
#include "mouse.h"

#include "stdio.h"
#include "stdlib.h"

int main(void)
{	  
		u16 t; //vu8 pressed;
		vu16 len;	
		u16 times=0;
		u8 stringBuf[50];
		
		// SD
		u8* buf;u8* buf_temp;
		u32 sd_size;
		u8 *p=0;
		// FATFS
		u16 res;
		// pic
		u8 frame_idx = 0;
		// adc
		

	// must haves ------------------------
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	
	// tests ------------------------
//	LED_Init();			     //LED被占用！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
//	KEY_Init();
//	TIM2_Int_Init(4999,179); // 180M/180=1Mhz --> 1us, 5000-->5ms
	
	// in use ------------------------
	Adc_init(); // MUST BEFORE LCD_INIT!!!!!!!!!!!!!!
	LCD_Init();
	SPI1_Init();
	mem_init();
	
// FATFS----------------------------------------------------------------------
	fatsd_init();
	sd_info();
	debug_lines();
	
	
	test_file();
	
	piclib_init();
	res = ai_load_picfile("0:/listen.gif",lcddev.width-220, 0, 220, 160, 1);
	
	gui_init();gui_draw();
  while(1) 
	{		 
		if(times%10==0)get_touchXY();
		
		if(USART_RX_STA&0x8000){					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			LCD_ShowString(30,200,300,16,16,USART_RX_BUF);
			for(t=0;t<len;t++)
			{
				USART_SendData(USART2, USART_RX_BUF[t]);
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
			}
			USART_RX_STA=0;
		}else{
			times++;
//			if(times%100==0){				
//				// read and print
//				fatsd_open("0:/test_folder/hello.txt");
//				fatsd_read(15);
//				f_close(file);
//				LCD_ShowString(30,150,260,16,16,"The file content is:");
//				LCD_ShowString(30,170,260,40,16,fatbuf);
//			}
			
		}
		delay_ms(10);   
	} 
}
