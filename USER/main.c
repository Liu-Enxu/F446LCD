//  BSP layer ----------------------------------------
//#include "led.h"
#include "delay.h"
//#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"

//#include "piclib.h"

#include "timer.h"
#include "touch.h"

#include "screens.h"

//  Middleware layer ----------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "RTOSmanager.h"

//	App layer ----------------------------------------
#include "lv_app_conf.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"

#include "diskio.h"

// ----------------------------------------
// |----------- dummy task -------------|
// ----------------------------------------
TaskHandle_t dummyTaskHandle;
void dummy_task(void *pvParameters){
	pvParameters = pvParameters;
	while(1){
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

// ----------------------------------------
// |----------- filesys task -------------|
// ---------------------------------------- 
// #define FILESYS_Q_LEN 10
// QueueHandle_t xfilesysQueue = NULL;

#define FILESYS_STACK 2*1024
TaskHandle_t filesysTaskHandle;
void filesys_task(void *pvParameters){
	static u8 fail_cnt = 0;
	static int res = 0; 
	static DSTATUS dStatus = 0;

	u32 total, free;
	pvParameters = pvParameters;
	while(1){
		if(0==peri_status.SD){   
			if(0==fatsd_init()){
				peri_status.SD = 1;
				dStatus = 0;
			};
		} else {
			if(exf_getfree("0:", &total, &free) == 0) {
                  fail_cnt = 0;
				  dStatus = 0;
            } else {
                fail_cnt++;
                if(fail_cnt >= 3) {
                    peri_status.SD = 0;
                    fail_cnt = 0;
                    f_mount(NULL, "0:", 0);
                    dStatus = STA_NODISK;
                }
            }
		}

		// if(xfilesysQueue){
		// 	xQueueSend(xfilesysQueue, &dStatus, pdMS_TO_TICKS(10));
		// }
		vTaskDelay(1000);
	}
}

// ----------------------------------------
// |----------- display task -------------|
// ---------------------------------------- 
// QueueSetHandle_t XDispQueueSet = NULL;
#define DISP_STACK 6*1024	// LOWER MIGHT CAUSE SCREEN NOT FOUND
TaskHandle_t dispTaskHandle;
void disp_task(void *pvParameters){
	pvParameters = pvParameters;
	// QueueSetMemberHandle_t xActivatedMember;
	start_scrn_manager();
	while(1){
		// if(xQueueReceive(ui_evt_queue, &evt, 0) == pdTRUE){
		// 	scrn_manager_ret_event_handler(&evt);
		// }
		lv_timer_handler();
		vTaskDelay(5);
	}
}

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
	
	// must haves ------------------------
	delay_init();	    	 //
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //
	uart_init(115200);	 	//Ϊ115200
//	TIM2_Int_Init(999,89); // 90M/(89+1)=1Mhz --> 1us, 9999+1-->10ms
	printf("begin!\n");	
	
	RTOS_init();
	lv_app_init();
	
	lv_log_register_print_cb(my_log_cb);
	lv_init();
	
	lv_port_disp_init();       
	lv_port_indev_init();
	
	printf("loop!\n");	
	
		
	taskENTER_CRITICAL();
	// xfilesysQueue = xQueueCreate(FILESYS_Q_LEN, sizeof(DSTATUS));
	xTaskCreate(filesys_task,
				"filesysTask",
				FILESYS_STACK,
				(void*)NULL,
				2,
				&filesysTaskHandle);

	// XDispQueueSet = xQueueCreateSet(FILESYS_Q_LEN);
	// xQueueAddToSet(xfilesysQueue, XDispQueueSet);
	xTaskCreate(disp_task,
				"dispTask",
            	DISP_STACK,
            	(void*)NULL,
            	3,
           	 	&dispTaskHandle);
	
	xTaskCreate(dummy_task,
				"dummyTask",
				512,
				(void*)NULL,
				1,
		   	 	&dummyTaskHandle);

	taskEXIT_CRITICAL();
							
	vTaskStartScheduler();

//	 while(1) 
//	{	
//		lv_timer_handler();
//		vTaskDelay(5);
//	}

// FATFS----------------------------------------------------------------------
//	fatsd_init();
//	sd_info();
	
	
//	test_file();
	
//	piclib_init();
//	res = ai_load_picfile("0:/listen.gif",lcddev.width-220, 0, 220, 160, 1);
	
//	gui_init();gui_draw();

//  while(1) 
//	{			
//		if(times%10==0)get_touchXY();
//		
//		if(USART_RX_STA&0x8000){					   
//			len=USART_RX_STA&0x3fff;
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
