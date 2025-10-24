#include "spi.h"
#include "usart.h"

void SPI1_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	// init gpio
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6/*CS*/;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5/*SCK*/|GPIO_Pin_6/*MISO*/|GPIO_Pin_7/*MOSI*/;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	// init spi
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; // !
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;// !
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;// NSS not connected due to pinout of ILI9486
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;// SYSCLK = 180000000, HCLK = SYSCLK / 1,PCLK2 = HCLK / 2, SPI_CLK = PCLK2 / 256 = 351.562kHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 10;
	SPI_Init(SPI1,&SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE); 
	SPI1_ReadWriteByte(0xFF);// 启动传输
	

//	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;			//SPI1中断配置
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0 
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级0 
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//  NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
}

void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1&=0XFFC7; // 1111 1111 1100 0111 empty the BR(baudrate control) bits
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); 

} 

u8 SPI1_ReadWriteByte(u8 TxData)
{		
	u8 res1;
	u8 retry=0;	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
		
	retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
//	res1 = SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据					    
//	printf("res of SPI: %04X\n",res1);
//	return res1;
		
		return SPI_I2S_ReceiveData(SPI1);
}
