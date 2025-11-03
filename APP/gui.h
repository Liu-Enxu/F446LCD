#ifndef __GUI_H
#define __GUI_H
#include "lcd.h"


typedef struct{
	u16 sx,sy,ex,ey;
	u16 color;
} BUTTON;

extern BUTTON* buttons;

void debug_lines(void);
void gui_init(void);
void gui_draw(void);

#endif