#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h"
#include "delay.h"
#include "timer.h"

//LCD的画笔颜色和背景色
u16 POINT_COLOR=WHITE;         //画笔颜色
u16 BACK_COLOR=BLACK;          //背景色 
  
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;
//LCDdataOut_TypeDef LCDdataOut;
//LCDdataIn_TypeDef LCDdataIn;

//static volatile unsigned long  * LCDdataOut[8] = {
//	LCD_D0_o_addr,
//	LCD_D1_o_addr,
//	LCD_D2_o_addr,
//	LCD_D3_o_addr,
//	LCD_D4_o_addr,
//	LCD_D5_o_addr,
//	LCD_D6_o_addr,
//	LCD_D7_o_addr
//};
//static volatile unsigned long  * LCDdataIn[8] = {
//	LCD_D0_i_addr,
//	LCD_D1_i_addr,
//	LCD_D2_i_addr,
//	LCD_D3_i_addr,
//	LCD_D4_i_addr,
//	LCD_D5_i_addr,
//	LCD_D6_i_addr,
//	LCD_D7_i_addr
//};

static inline void DatabusWrite(u8 data){
	LCD_D0_out = (data&0X01);data>>=1;
	LCD_D1_out = (data&0X01);data>>=1;
	LCD_D2_out = (data&0X01);data>>=1;
	LCD_D3_out = (data&0X01);data>>=1;
	LCD_D4_out = (data&0X01);data>>=1;
	LCD_D5_out = (data&0X01);data>>=1;
	LCD_D6_out = (data&0X01);data>>=1;
	LCD_D7_out = (data&0X01);
	
//	*LCD_D0_o_addr = (data&0X01);data>>=1;
//	*LCD_D1_o_addr = (data&0X01);data>>=1;
//	*LCD_D2_o_addr = (data&0X01);data>>=1;
//	*LCD_D3_o_addr = (data&0X01);data>>=1;
//	*LCD_D4_o_addr = (data&0X01);data>>=1;
//	*LCD_D5_o_addr = (data&0X01);data>>=1;
//	*LCD_D6_o_addr = (data&0X01);data>>=1;
//	*LCD_D7_o_addr = (data&0X01);
	
//	volatile unsigned long ** p = &LCDdataOut.addr_D0;
//	for(u8 i=0;i<8;i++){
//		*(p[i]) = (data&0X01);
//		data>>=1;
//	}
	
//	for(u8 i=0;i<8;i++){
//		*(LCDdataOut[i]) = (data&0X01);
//		data>>=1;
//	}
	
}

static inline uint8_t DatabusRead(){
	return (uint8_t)(LCD_D7_in<<7 | LCD_D6_in<<6 | LCD_D5_in<<5 | LCD_D4_in<<4 | LCD_D3_in<<3 | LCD_D2_in<<2 | LCD_D1_in<<1 | LCD_D0_in);
}

// 重置
void LCD_RESET(void){
	LCD_CS = 0;
	
	LCD_RST = 1;
	delay_ms(1);
	LCD_RST = 0;
	delay_ms(10);
	LCD_RST = 1;
	delay_ms(120);

	LCD_CS = 1;
}

void LCD_WR_CMD(u8 cmd){
//	LCD_DATA_OUT()
	
	LCD_CS = 0;
	
	DatabusWrite(cmd);
		
	LCD_RS = 0;
	LCD_WR = 0;
	LCD_WR = 1;
	LCD_RS = 1;
	
	LCD_CS = 1;
}

void LCD_WR_DATA(u8 data){
//	LCD_DATA_OUT()
	
	LCD_CS = 0;
	
	DatabusWrite(data);
		
//	LCD_RS = 1;
	LCD_WR = 0;
	LCD_WR = 1;
//	LCD_RS = 1;
	
	LCD_CS = 1;
}


u8 LCD_RD_DATA(void){
	u8 result = 0;
	
	LCD_DATA_IN();
	LCD_CS = 0;	
	
	LCD_RD = 0;	
	delay_us(1);
	result = DatabusRead();
	LCD_RD = 1;
	
	LCD_CS = 1;
	LCD_DATA_OUT();
	return result;
}

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_CMD(lcddev.wramcmd);
}

//续写GRAM
void LCD_WriteRAM_Continue(void)
{
	LCD_WR_CMD(lcddev.wramcontd);
}
//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(u16 color)
{
    LCD_WR_DATA(color>>8);
		LCD_WR_DATA(color & 0XFF);
}

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
    u16  r, g, b, rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return (rgb);
}

//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
    while (i--);
}

//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
u16 LCD_ReadPoint(u16 x, u16 y)
{
    u16 data1,data2;

    if (x >= lcddev.width || y >= lcddev.height)return 0;   //超过了范围,直接返回

    LCD_SetCursor(x, y);

		LCD_WR_CMD(0X2E);

    data1 = LCD_RD_DATA();          //假读
    data1 = LCD_RD_DATA();          //实际坐标颜色
    data2 = LCD_RD_DATA();
  
    return ( (data1 << 8) | data2 ); 
}

//LCD开启显示
void LCD_DisplayOn(void)
{
    LCD_WR_CMD(0X29);       //开启显示
}

//LCD关闭显示
void LCD_DisplayOff(void)
{
    LCD_WR_CMD(0X28);       //关闭显示

}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	LCD_WR_CMD(lcddev.setxcmd);
	LCD_WR_DATA(Xpos >> 8);
	LCD_WR_DATA(Xpos & 0XFF);
	LCD_WR_DATA(lcddev.winwend >> 8);
	LCD_WR_DATA(lcddev.winwend & 0XFF);
	
	LCD_WR_CMD(lcddev.setycmd);
	LCD_WR_DATA(Ypos >> 8);
	LCD_WR_DATA(Ypos & 0XFF);
	LCD_WR_DATA(lcddev.winhend >> 8);
	LCD_WR_DATA(lcddev.winhend & 0XFF);
}

//设置LCD的自动扫描方向
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9341/5310/5510/1963/7789等IC已经实际测试
//注意:其他函数可能会受到此函数设置的影响(尤其是9341),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
    u16 temp;
		
		if (lcddev.dir == 1)
    {
			switch (dir)   //方向转换
			{
					case 0:
							dir = 6;
							break;

					case 1:
							dir = 7;
							break;

					case 2:
							dir = 4;
							break;

					case 3:
							dir = 5;
							break;

					case 4:
							dir = 1;
							break;

					case 5:
							dir = 0;
							break;

					case 6:
							dir = 3;
							break;

					case 7:
							dir = 2;
							break;
			}
		}
    

    switch (dir)
    {
        case L2R_U2D://从左到右,从上到下
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U://从左到右,从下到上
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D://从右到左,从上到下
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U://从右到左,从下到上
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R://从上到下,从左到右
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L://从上到下,从右到左
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R://从下到上,从左到右
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L://从下到上,从右到左
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    if (lcddev.id == 0x9486){
			dirreg = 0X36;
			regval |= 0X08;
		}

    LCD_WR_CMD(dirreg);
		LCD_WR_DATA(regval);

		if (regval & 0X20)
		{
			if (lcddev.width < lcddev.height)   //交换X,Y
			{
					temp = lcddev.width;
					lcddev.width = lcddev.height;
					lcddev.height = temp;
			}
		}else{
			if (lcddev.width > lcddev.height)   //交换X,Y
			{
					temp = lcddev.width;
					lcddev.width = lcddev.height;
					lcddev.height = temp;
			}
		}
    

    //设置显示区域(开窗)大小
		lcddev.winwsta = 0;
		lcddev.winhsta = 0;
		lcddev.winwend = lcddev.width;
		lcddev.winhend = lcddev.height;
		lcddev.workw = lcddev.width;
		lcddev.workh = lcddev.height;
		LCD_WR_CMD(lcddev.setxcmd);		
		LCD_WR_DATA(0);															// SP high
		LCD_WR_DATA(0);															// SP low
		LCD_WR_DATA((lcddev.width - 1) >> 8);				// EP high
		LCD_WR_DATA((lcddev.width - 1) & 0XFF);			// EP low
		
		LCD_WR_CMD(lcddev.setycmd);									
		LCD_WR_DATA(0);															// SP high
		LCD_WR_DATA(0);															// SP low
		LCD_WR_DATA((lcddev.height - 1) >> 8);			// EP high
		LCD_WR_DATA((lcddev.height - 1) & 0XFF);		// EP low
		

}

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x, u16 y)
{
		LCD_WriteRAM_Continue();
		LCD_SetCursor(x, y);        //设置光标位置
    LCD_WriteRAM_Prepare();
    LCD_WR_DATA(POINT_COLOR>>8);
		LCD_WR_DATA(POINT_COLOR & 0XFF); 
}

//快速画点(自定义颜色)
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{

    LCD_WR_CMD(lcddev.setxcmd);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0XFF);
		LCD_WR_DATA(lcddev.winwend >> 8);
		LCD_WR_DATA(lcddev.winwend & 0XFF);
	
    LCD_WR_CMD(lcddev.setycmd);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0XFF);
		LCD_WR_DATA(lcddev.winhend >> 8);
		LCD_WR_DATA(lcddev.winhend & 0XFF);

    LCD_WR_CMD(lcddev.wramcmd);
    LCD_WR_DATA(color>>8);
		LCD_WR_DATA(color & 0XFF); 
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
    lcddev.dir = dir;       //竖屏/横屏
		
		lcddev.wramcmd = 0X2C;
		lcddev.wramcontd = 0x3C;
		lcddev.setxcmd = 0X2A;
		lcddev.setycmd = 0X2B;
		
    if (dir == 0)           //竖屏
    {
			lcddev.width = 320;
      lcddev.height = 480;    
    }else{     //横屏
			lcddev.width = 480;
      lcddev.height = 320;
    }

    LCD_Scan_Dir(DFT_SCAN_DIR);     //默认扫描方向
}

//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height)
{
		lcddev.workw = width;
		lcddev.workh = height;
		lcddev.winwsta = sx;
		lcddev.winhsta = sy;
		lcddev.winwend = sx + width - 1;
    lcddev.winhend = sy + height - 1;

    LCD_WR_CMD(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0XFF);
    LCD_WR_DATA(lcddev.winwend >> 8);
    LCD_WR_DATA(lcddev.winwend & 0XFF);
    LCD_WR_CMD(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0XFF);
    LCD_WR_DATA(lcddev.winwend >> 8);
    LCD_WR_DATA(lcddev.winwend & 0XFF);
}

//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试!
void LCD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE); //使能PORTA,B,C
		// Default GPIO settings
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    //推挽输出
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		
		//	Data pins ----------------------------------------------
    //	PORTA Data pins
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0/*LCD_RD*/|GPIO_Pin_1/*LCD_WR*/|GPIO_Pin_4/*LCD_RS*/|GPIO_Pin_8/*LCD_D7 or YM*/|GPIO_Pin_9/*LCD_D0*/|GPIO_Pin_10/*LCD_D2*/;		//PORTA
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
    //	PORTB复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0/*LCD_CS*/|GPIO_Pin_3/*LCD_D3*/|GPIO_Pin_4/*LCD_D5*/|GPIO_Pin_5/*LCD_D4*/|GPIO_Pin_10/*LCD_D6 or XP*/;    //PORTB复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
     
    //	PORTC复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1/*LCD_RST*/|GPIO_Pin_7/*LCD_D1*/; //PORTC复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		//	Ctrl pins ----------------------------------------------
		// PORTA Ctrl pins
		GPIO_SetBits(GPIOA,GPIO_Pin_0/*LCD_RD*/|GPIO_Pin_1/*LCD_WR*/|GPIO_Pin_4/*LCD_RS*/);
		// PORTB Ctrl pin
//		GPIO_SetBits(GPIOB,GPIO_Pin_0);/*LCD_CS*/
		GPIO_ResetBits(GPIOB,GPIO_Pin_10);
		// PORTC Ctrl pin
		GPIO_SetBits(GPIOC,GPIO_Pin_1);/*LCD_RST*/
		
//		LCDdataOut.addr_D0 = LCD_D0_o_addr;
//		LCDdataOut.addr_D1 = LCD_D1_o_addr;
//		LCDdataOut.addr_D2 = LCD_D2_o_addr;
//		LCDdataOut.addr_D3 = LCD_D3_o_addr;
//		LCDdataOut.addr_D4 = LCD_D4_o_addr;
//		LCDdataOut.addr_D5 = LCD_D5_o_addr;
//		LCDdataOut.addr_D6 = LCD_D6_o_addr;
//		LCDdataOut.addr_D7 = LCD_D7_o_addr;
//		
//		LCDdataIn.addr_D0 = LCD_D0_i_addr;
//		LCDdataIn.addr_D1 = LCD_D1_i_addr;
//		LCDdataIn.addr_D2 = LCD_D2_i_addr;
//		LCDdataIn.addr_D3 = LCD_D3_i_addr;
//		LCDdataIn.addr_D4 = LCD_D4_i_addr;
//		LCDdataIn.addr_D5 = LCD_D5_i_addr;
//		LCDdataIn.addr_D6 = LCD_D6_i_addr;
//		LCDdataIn.addr_D7 = LCD_D7_i_addr;
		
		LCD_RESET();	
		delay_ms(50);       // delay 50 ms		
		
    //尝试9486 ID的读取
    LCD_WR_CMD(0XD3);
		delay_ms(50);
    lcddev.id = LCD_RD_DATA();          //dummy read
    lcddev.id = LCD_RD_DATA();          //读到0X00
    lcddev.id = LCD_RD_DATA();          //读取0X94
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA();         //读取0X86
		
		printf(" LCD ID:%x\r\n", lcddev.id);
		
		vu8 temp;
		LCD_WR_CMD(0X04);
		temp = LCD_RD_DATA();		// dummy data
		temp = LCD_RD_DATA();		// manufacturer ID
		printf(" Manufacturer ID:%x\r\n", temp);
		temp = LCD_RD_DATA();		// module/driver version ID
		printf(" Module/driver version ID:%x\r\n", temp);
		temp = LCD_RD_DATA();		// module/driver ID
		printf(" Module/driver ID:%x\r\n", temp);
		
		LCD_WR_CMD(0X0C);
		temp = LCD_RD_DATA();		// dummy data
		temp = LCD_RD_DATA();		// pixel format
		printf("Pixel format and CPU format ID:%x before set\r\n", temp);
		
		LCD_WR_CMD(0X3A);
		LCD_WR_DATA(0x55);		// pixel format
		LCD_WR_CMD(0X0C);
		temp = LCD_RD_DATA();		// dummy data
		temp = LCD_RD_DATA();		// pixel format
		printf("Pixel format and CPU format ID:%x after set\r\n", temp);
		
		if(lcddev.id!=0X9486){
			return;
		} else {
			LCD_WR_CMD(0XF2);    
			LCD_WR_DATA(0x18); 
			LCD_WR_DATA(0xA3); 
			LCD_WR_DATA(0x12); 
			LCD_WR_DATA(0x02); 
			LCD_WR_DATA(0XB2); 
			LCD_WR_DATA(0x12); 
			LCD_WR_DATA(0xFF); 
			LCD_WR_DATA(0x10); 
			LCD_WR_DATA(0x00);
			
			LCD_WR_CMD(0XF8);    
			LCD_WR_DATA(0x21); 
			LCD_WR_DATA(0x04);
			
			LCD_WR_CMD(0XF9);    
			LCD_WR_DATA(0x00); 
			LCD_WR_DATA(0x08);
			
			LCD_WR_CMD(0x36);    
			LCD_WR_DATA(0x08);
			
			LCD_WR_CMD(0xB4); 
			LCD_WR_DATA(0x00);
			
			LCD_WR_CMD(0xC1);    
			LCD_WR_DATA(0x41);

			LCD_WR_CMD(0xC5);    
			LCD_WR_DATA(0x00); 
			LCD_WR_DATA(0x53); 
			LCD_WR_CMD(0xE0);    
			LCD_WR_DATA(0x0F); 
			LCD_WR_DATA(0x1B); 
			LCD_WR_DATA(0x18); 
			LCD_WR_DATA(0x0B); 
			LCD_WR_DATA(0x0E); 
			LCD_WR_DATA(0x09); 
			LCD_WR_DATA(0x47); 
			LCD_WR_DATA(0x94); 
			LCD_WR_DATA(0x35); 
			LCD_WR_DATA(0x0A); 
			LCD_WR_DATA(0x13); 
			LCD_WR_DATA(0x05); 
			LCD_WR_DATA(0x08); 
			LCD_WR_DATA(0x03); 
			LCD_WR_DATA(0x00);
			
			LCD_WR_CMD(0XE1);    
			LCD_WR_DATA(0x0F); 
			LCD_WR_DATA(0x3A); 
			LCD_WR_DATA(0x37); 
			LCD_WR_DATA(0x0B); 
			LCD_WR_DATA(0x0C); 
			LCD_WR_DATA(0x05); 
			LCD_WR_DATA(0x4A); 
			LCD_WR_DATA(0x24); 
			LCD_WR_DATA(0x39); 
			LCD_WR_DATA(0x07); 
			LCD_WR_DATA(0x10); 
			LCD_WR_DATA(0x04); 
			LCD_WR_DATA(0x27); 
			LCD_WR_DATA(0x25); 
			LCD_WR_DATA(0x00);
			
			LCD_WR_CMD(0x11);
			delay_ms(120); 
			LCD_WR_CMD(0x29);
		}
		
		
		
		
		LCD_Display_Dir(1);         //默认为竖屏
    
//		LCD_Clear(BACK_COLOR);
		LCD_Fast_Clear(BACK_COLOR);
}  

//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
    u32 index = 0;u32 totalpoint;
		totalpoint = lcddev.workw*lcddev.workh;

//		printf("totalpoint:%u;%u",lcddev.winwend,lcddev.winhend);
    LCD_SetCursor(lcddev.winwsta, lcddev.winhsta);    //设置光标位置
    LCD_WriteRAM_Prepare();         //开始写入GRAM
		
//		TIMER_CNT = TIM2->CNT;
	
    for (index = 0; index < totalpoint; index++)
    {
			LCD_WriteRAM(color);
    }	 
		
//		ELAPSE = (u32)((UPDATE_CNT*(TIM2->ARR+1)+TIM2->CNT-TIMER_CNT));
//		printf("elapse: %u\n",ELAPSE);
//		printf("UPDATE_CNT: %u\n",UPDATE_CNT);UPDATE_CNT=0;
}	


void LCD_Fast_Clear(u16 color)
{
	u32 index = 0;u32 totalpoint;
	totalpoint = lcddev.workw*lcddev.workh;

	LCD_SetCursor(lcddev.winwsta, lcddev.winhsta);    //设置光标位置
  LCD_WriteRAM_Prepare(); 
	
	LCD_CS = 0;
	
	u16 MSB = color>>8;
	u16 LSB = color & 0xFF;
	
//	TIMER_CNT = TIM2->CNT;
	
	for (index = 0; index < totalpoint; index++)
	{
		DatabusWrite(MSB);
		LCD_WR = 0;
		LCD_WR = 1;
		DatabusWrite(LSB);
		LCD_WR = 0;
		LCD_WR = 1;

	}
	
//	ELAPSE = (u32)((UPDATE_CNT*(TIM2->ARR+1)+TIM2->CNT-TIMER_CNT));
//	printf("elapse: %u\n",ELAPSE);
//	printf("UPDATE_CNT: %u\n",UPDATE_CNT);UPDATE_CNT=0;
	
	LCD_CS = 1;
}

void LCD_draw_raw(u16 sx, u16 sy, u16 width, u16 height, u16* frame)
{
	u32 index;
	u32 totalpoint;
	// set position
	LCD_Set_Window(sx,sy,width,height);
	// fill in bytes
	totalpoint = width*height;
	LCD_WriteRAM_Prepare(); 
	
	LCD_CS = 0;
	
	
	for (index = 0; index < totalpoint; index++)
	{
		DatabusWrite(frame[index]>>8);
		LCD_WR = 0;
		LCD_WR = 1;
		DatabusWrite(frame[index] & 0xFF);
		LCD_WR = 0;
		LCD_WR = 1;

	}
	
	LCD_CS = 1;
	
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);
}
//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
    u16 i, j;
    u16 xlen = 0;

    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);       //设置光标位置
        LCD_WriteRAM_Prepare();     //开始写入GRAM
//				LCD_WriteRAM_Continue();

        for (j = 0; j < xlen; j++)
        {
            LCD_WriteRAM(color);     //设置光标位置
        }
    }
}

//在指定区域内填充指定颜色块(画图？？？)
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    u16 height, width;
    u16 i, j;
    width = ex - sx + 1;            //得到填充的宽度
    height = ey - sy + 1;           //高度

    for (i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i);  //设置光标位置
        LCD_WriteRAM_Prepare();     //开始写入GRAM
        for (j = 0; j < width; j++)
        {
            LCD_WriteRAM(color[i * width + j]);  //写入数据
        }
    }
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;              //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)incx = 1;       //设置单步方向
    else if (delta_x == 0)incx = 0; //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; //水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )    //画线输出
    {
        LCD_DrawPoint(uRow, uCol); //画点
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

//画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       //判断下个点位置的标志

    while (a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b);        //5
        LCD_DrawPoint(x0 + b, y0 - a);        //0
        LCD_DrawPoint(x0 + b, y0 + a);        //4
        LCD_DrawPoint(x0 + a, y0 + b);        //6
        LCD_DrawPoint(x0 - a, y0 + b);        //1
        LCD_DrawPoint(x0 - b, y0 + a);
        LCD_DrawPoint(x0 - a, y0 - b);        //2
        LCD_DrawPoint(x0 - b, y0 - a);        //7
        a++;

        //使用Bresenham算法画圆
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp, t1, t;
    u16 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);  //得到字体一个字符对应点阵集所占的字节数
    num = num - ' ';    //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

    for (t = 0; t < csize; t++)
    {
        if (size == 12)temp = asc2_1206[num][t];        //调用1206字体
        else if (size == 16)temp = asc2_1608[num][t];   //调用1608字体
        else if (size == 24)temp = asc2_2412[num][t];   //调用2412字体
        else return;                                    //没有的字库

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)LCD_Fast_DrawPoint(x, y, BACK_COLOR);

            temp <<= 1;
            y++;

            if (y >= lcddev.height)return;      //超区域了

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= lcddev.width)return;   //超区域了

                break;
            }
        }
    }
}


//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;

    while (n--)result *= m;

    return result;
}

//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + (size / 2)*t, y, ' ', size, 0);
                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, 0);
    }
}

//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)LCD_ShowChar(x + (size / 2)*t, y, '0', size, mode & 0X01);
                else LCD_ShowChar(x + (size / 2)*t, y, ' ', size, mode & 0X01);

                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, mode & 0X01);
    }
}

//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   //判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break; //退出

        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}





































