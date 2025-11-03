#include "gui.h"
#include "malloc.h"

BUTTON* buttons;

void gui_init(void){
	u8 i;
	buttons = (BUTTON*)mymalloc(sizeof(BUTTON)*4);
	for(i=0;i<4;i++){
		buttons[i] = (BUTTON){
			.sx = 280+i*50,
			.sy = 280,
			.ex = 280+i*50+40,
			.ey = 320,
			.color = WHITE
		};
	}
}

void gui_draw(void){
	u8 t;
	for(t=0;t<4;t++){
		LCD_Fill(buttons[t].sx,buttons[t].sy,buttons[t].ex,buttons[t].ey,buttons[t].color);
	}
}

void debug_lines(void){
	LCD_ShowString(30,90,200,16,16,"SRAM USED:   %");
	LCD_ShowString(30,110,300,16,16,"The message you sent is:");
	LCD_ShowString(30,150,260,16,16,"The file content is:");
	LCD_ShowString(30,190,240,16,16,"Your touch location XY is:");
	LCD_ShowString(30,210,420,16,16,"The button you pressed is: ");
}
