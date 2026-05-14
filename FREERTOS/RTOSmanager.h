#ifndef __RTOSMANAGER_H
#define __RTOSMANAGER_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "lv_app_conf.h"

//	lcd
extern SemaphoreHandle_t lcd_bus_mutex;
//	noti q
#define NOTI_Q_LEN 10
extern QueueHandle_t notifyQueue;
	
	
void RTOS_init(void);

#endif

