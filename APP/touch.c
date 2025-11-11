#include "touch.h"


u16 adc;
u16 adcY, adcX, adcZ;

vu16 mouseX = 0;
vu16 mouseY = 0;
vu16 last_mX = 0;
vu16 last_mY = 0;
vu8 mouse_moved = 0;

bool get_touchXY(void){
	u8 i;
	enableZ();
	adcX = Get_Adc(ADC_Channel_4);
	adcY = Get_Adc(ADC_Channel_1);
	adcZ = 1023 - (adcY-adcX);
	if(MIN_PRES<=adcZ && adcZ<=MAX_PRES){ // disregard the movement of longpress I.E. if Z>min_pres continues, do not update mouse
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
			
//			LCD_DATA_OUT();
			return true;
	}
//	LCD_DATA_OUT();
	return false;

}