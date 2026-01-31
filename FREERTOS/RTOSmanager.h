#ifndef __RTOSMANAGER_H
#define __RTOSMANAGER_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern SemaphoreHandle_t lcd_bus_mutex;
void RTOS_init(void);

#endif