#include "RTOSmanager.h"

static StaticSemaphore_t lcd_mutex_buf;

SemaphoreHandle_t lcd_bus_mutex;

void RTOS_init(void){
	lcd_bus_mutex = xSemaphoreCreateMutexStatic(&lcd_mutex_buf);
}

