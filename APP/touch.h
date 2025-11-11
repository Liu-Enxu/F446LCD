#ifndef __TOUCH_H
#define __TOUCH_H
#include "sys.h"
#include "lcd.h"
#include "adc.h"
#include "fatsd.h"
#include <stdbool.h>

extern u16 adc;
extern u16 adcY, adcX, adcZ;

extern vu16 mouseX;
extern vu16 mouseY;
extern vu16 last_mX;
extern vu16 last_mY;
extern vu8 mouse_moved;

bool get_touchXY(void);

#endif