#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern u8 UPDATE_CNT;
extern u16 TIMER_CNT;
extern u32 ELAPSE;

void TIM2_Int_Init(u16 arr,u16 psc);
#endif
