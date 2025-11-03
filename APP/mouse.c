#include "mouse.h"


u16 adc;
u16 adcY, adcX, adcZ;

vu16 mouseX = 0;
vu16 mouseY = 0;
vu16 last_mX = 0;
vu16 last_mY = 0;
vu8 mouse_moved = 0;

void get_touchXY(void){
	u8 i;
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
			
			LCD_ShowNum(27*8+30,190,mouseX,3,16);
			LCD_ShowNum(31*8+30,190,mouseY,3,16);
			LCD_ShowNum(35*8+30,190,adcZ,3,16);
			
			mouse_moved = 1;	
			
			for(i=0;i<4;i++){
				if(buttons[i].sx<=mouseX && mouseX<=buttons[i].ex && buttons[i].sy<=mouseY && mouseY<=buttons[i].ey){
					switch(i){
						case 0:
							LCD_ShowNum(27*8+30,210,0,1,16);
							fatsd_dir("0:/test_folder",1);
//							fatsd_file("0:/test_folder/hello.txt", 1);
							break;
						case 1:
							LCD_ShowNum(27*8+30,210,1,1,16);
							fatsd_stringAppend("0:/hello","1");
							break;
						case 2:
							LCD_ShowNum(27*8+30,210,2,1,16);
							break;
						case 3:
							LCD_ShowNum(27*8+30,210,3,1,16);
							break;
					}
				}
			}
		}
	}else{mouse_moved = 0;}
	
	LCD_DATA_OUT();
}