#include "adc.h"
#include "delay.h"

void Adc_init(void){
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;    //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1/*WR - PA1 (A1) - ANALOG ADC123_IN1*/ | GPIO_Pin_4/*RS - PA4 (A2) - ANALOG ADC12_IN4*/;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_DeInit();
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_Cmd(ADC1,ENABLE);  
}

u16 Get_Adc(u8 ch){
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_15Cycles);
	ADC_SoftwareStartConv(ADC1);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}

u16 Get_Adc_Average(u8 ch, u8 times){
	u32 tmp = 0;
	u8 t;
	for(t=0;t<times;t++){
		tmp += Get_Adc(ch);
		delay_ms(5);
	}
	return tmp/times;
}
