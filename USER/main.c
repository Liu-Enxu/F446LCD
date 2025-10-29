//#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "timer.h"
#include "spi.h"
#include "sd.h"
#include "malloc.h"
#include "ff.h"  
#include "exfuns.h" 
#include "adc.h"
#include "piclib.h"


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
		u32 total,free;
		u8 res;
		// pic
		u8 frame_idx = 0;
		// adc
		u16 adc;
		u16 adcY, adcX, adcZ;
		
		vu16 mouseX;
		vu16 mouseY;
		vu16 last_mX = 0;
		vu16 last_mY = 0;
		vu8 mouse_moved = 0;
	
	
	
//		while(1);
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
	
	
// ADC test----------------------------------------------------------------------	
//	while(1){
//		if(times%100==0){
//			adc = Get_Adc_Average(ADC_Channel_1, 10);
//			printf("The adc reading: %u",adc);
//		}
//		times++;
//		delay_ms(10);
//	}



// pure SD----------------------------------------------------------------------	
// 	while(SD_Initialize())//检测不到SD卡
//	{
//		LCD_ShowString(30,100,200,24,24,"SD Card Error!");
//		delay_ms(500);					
//		LCD_ShowString(30,130,200,24,24,"Please Check!");
//		delay_ms(500);
//	}

//	//检测SD卡成功 											    
//	LCD_ShowString(30,100,200,24,24,"SD Card OK!");
//	sd_size=SD_GetSectorCount();//得到扇区数
//	sprintf((char*)stringBuf,"SD Card Size: %u MB",sd_size>>11);
//	LCD_ShowString(30,130,300,24,24,(char*)stringBuf);


//	buf = mymalloc(512);
//	mymemset(buf,0,512);
//	SD_WriteDisk(buf,0/*start sector*/,1/*num of sec*/);
//	mymemset(buf,0,512);
//	delay_ms(10);
//	if(SD_ReadDisk(buf,0,1)==0){
//		buf_temp = mymalloc(512);
//		for(t=0;t<512;t++){
//			(buf_temp[t]) = (char)(buf[t] + (u8)('0'));
//		}
//		LCD_ShowString(30,280,260,180,12,(char*)buf_temp);
//		myfree(buf_temp);
//	}	

// FATFS----------------------------------------------------------------------
	exfuns_init();
	res = f_mount(fs[0],"0:",1);
	if(res){
		LCD_ShowString(30,20,200,24,24,"Mount err!");	
		LCD_ShowNum(30+12*10,20,res,5,24);
		while(1);
	}

	LCD_ShowString(30,20,200,24,24,"Program start");	

	while(exf_getfree("0",&total,&free))	//得到SD卡的总容量和剩余容量
	{
		LCD_ShowString(30,50,200,24,24,"SD Card Fatfs Error!");
		delay_ms(200);
		LCD_ShowString(30,50,200,24,24,"Re-initializing!    ");	//清除显示			  
		delay_ms(200);
	}
	LCD_ShowString(30,50,200,24,24,"FATFS OK!");	 
	LCD_ShowString(30,80,300,24,24,"SD Total Size:     MB");	 
	LCD_ShowString(30,110,300,24,24,"SD  Free Size:     MB"); 	    
 	LCD_ShowNum(30+12*14,80,total>>10,5,24);				//显示SD卡总容量 MB
 	LCD_ShowNum(30+12*14,110,free>>10,5,24);					//显示SD卡剩余容量 MB
	
	LCD_ShowString(30,140,200,16,16,"SRAM USED:   %");
	LCD_ShowString(30,160,300,16,16,"The message you sent is:");
	LCD_ShowString(30,200,300,16,16,"The message you stored is:");
	LCD_ShowString(30,280,260,16,16,"Your touch location XY is:");
	
	// create and write
	if(f_stat("0:/test_folder",&fileinfo)){
		res = f_mkdir("0:/test_folder");
		if(res){
			LCD_ShowString(30,240,300,16,16,"Folder creation failure!");
			while(1);
		}
	}
		
	if(f_stat("0:/test_folder/hello.txt",&fileinfo)){
		res = f_open(file, "0:/test_folder/hello.txt", FA_CREATE_NEW | FA_WRITE);
	}else{
		res = f_open(file, "0:/test_folder/hello.txt", FA_WRITE);
	}
	if(res){
		LCD_ShowString(30,240,300,16,16,"File creation/write failure!");
		while(1);
	}
	
	f_write(file, "Hello, World!\r\n", 15, &bw);
	if (bw != 15){
		LCD_ShowString(30,240,300,16,16,"File writing failure!");
		while(1);
	}
	f_close(file);
	
	mouseX = lcddev.width-220;
	mouseY = 0;
	
	piclib_init();
	res = ai_load_picfile("0:/listen.gif",mouseX, mouseY, 220, 160, 1);
	if(res)printf("error:%04X",res);
	
	for(t=0;t<4;t++){
		LCD_Fill(280+t*50,280,280+t*50+40,320,WHITE);
	}
  while(1) 
	{		 
		if(times%10==0)
		{
			
			enableZ();
			adcX = Get_Adc(ADC_Channel_4);
			adcY = Get_Adc(ADC_Channel_1);
			adcZ = 1023 - (adcY-adcX);
			if(MIN_PRES<=adcZ && adcZ<=MAX_PRES){ // disregard the movement of longpress I.E. if Z>min_pres continues, do not update mouse
				if(!mouse_moved){
					disableY();
					enableX();
					adcX = Get_Adc_Average(ADC_Channel_4,5);
					disableX();
			
					enableY();
					adcY = Get_Adc_Average(ADC_Channel_1,5);
					disableY();
					
					last_mX = mouseX;last_mY = mouseY;
					mouseX = (vu16)(((adcX-70)*lcddev.width)/(930-70));
					mouseY = lcddev.height-(vu16)((adcY-100)*lcddev.height/(925-100));
//					LCD_Draw_Circle(mouseX, mouseY, 50);
					
					LCD_ShowNum(27*8+30,280,mouseX,3,16);
					LCD_ShowNum(31*8+30,280,mouseY,3,16);
					LCD_ShowNum(35*8+30,280,adcZ,3,16);
					
					mouse_moved = 1;	
				}
			}else{mouse_moved = 0;}
			
			LCD_DATA_OUT();
			
		}
		
//		if(times%30==0)
//		{
//			LCD_draw_raw(lcddev.width-128,0,128,128,idle_Frames[frame_idx++]);
//			frame_idx%=idle_framesNum;
//		}
		
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
			if(times%100==0){
				if(!p){
					p = mymalloc(2048);
					mymemcpy(p,(u8*)("Mem alloc!"), 10);
				} else {
					myfree(p);
					p=NULL;
				}
				
				LCD_ShowNum(30+80,140,mem_perused(),3,16);
				LCD_ShowString(30,220,300,16,16,"              ");
				LCD_ShowString(30,220,300,16,16,(char*)p);
				
				// read and print
				res = f_open(file, "0:/test_folder/hello.txt", FA_READ);
				if(res){
					LCD_ShowString(30,240,300,16,16,"Read file open failure!");
					while(1);
				}
				res = f_read(file,fatbuf,15,&br);
				if(br!=15){
					LCD_ShowString(30,240,300,16,16,"File reading failure!");
					while(1);
				}
				f_close(file);
				LCD_ShowString(30,240,260,16,16,"The file content is:");
				LCD_ShowString(30,260,260,40,16,fatbuf);
			}
			
		}
		delay_ms(10);   
	} 
}
