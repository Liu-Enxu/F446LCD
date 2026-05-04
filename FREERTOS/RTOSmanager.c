#include "RTOSmanager.h"

// lcd
static StaticSemaphore_t lcd_mutex_buf;
SemaphoreHandle_t lcd_bus_mutex;

// notification system
QueueHandle_t notifyQueue;
static uint8_t notifyQueueStorageBuf[ NOTI_Q_LEN * sizeof(noti_t) ];
static StaticQueue_t notifyQueueBuffer;

void RTOS_init(void){
	lcd_bus_mutex = xSemaphoreCreateMutexStatic(&lcd_mutex_buf);
	notifyQueue = xQueueCreateStatic(NOTI_Q_LEN,sizeof(noti_t),notifyQueueStorageBuf,&notifyQueueBuffer);
}

