#ifndef __LCD_H
#define __LCD_H
#include "sys.h"
#include "stdlib.h"


// LCD --------------------------------------------------------------------------
//LCD重要参数集
typedef struct
{
    u16 width;      //LCD 宽度
    u16 height;     //LCD 高度
    u16 id;         //LCD ID
    u8  dir;        //横屏还是竖屏控制：0，竖屏；1，横屏。
    u16	wramcmd;    //开始写gram指令
		u16 wramcontd;
    u16 setxcmd;    //设置x坐标指令
    u16 setycmd;   //设置y坐标指令
		
		u16 winwsta;
		u16 winhsta;
		u16 winwend;		//开窗宽度极值绝对位置
		u16 winhend;		//开窗高度极值绝对位置
		
		u16 workw;
		u16 workh;
//		u16 workwsta;
//		u16 workhsta;
//		u16 workwend;
//		u16 workhend;
	
} _lcd_dev;

//LCD参数
extern _lcd_dev lcddev;     //管理LCD重要参数
//LCD的画笔颜色和背景色
extern u16  POINT_COLOR;    //默认红色
extern u16  BACK_COLOR;     //背景颜色.默认为白色

#define LCD_DATA_OUT() 	{GPIOA->MODER&=0XFFC0FCF3;GPIOA->MODER|=0X00150104;GPIOB->MODER&=0XFFCFF03F;GPIOB->MODER|=0X00100540;GPIOC->MODER&=0XFFFF3FFF;GPIOC->MODER|=0X00004000;} // A:8 9 10; B:3 4 5 10; C:7
#define LCD_DATA_IN() 	{GPIOA->MODER&=0XFFC0FFFF;							 					 GPIOB->MODER&=0XFFCFF03F;												 GPIOC->MODER&=0XFFFF3FFF;}

// output
#define	LCD_D0_o_addr PAo_addr(9)
#define	LCD_D1_o_addr PCo_addr(7)
#define LCD_D2_o_addr PAo_addr(10)
#define LCD_D3_o_addr PBo_addr(3)
#define LCD_D4_o_addr PBo_addr(5)
#define LCD_D5_o_addr PBo_addr(4)
#define LCD_D6_o_addr PBo_addr(10)
#define LCD_D7_o_addr PAo_addr(8)

#define	LCD_D0_out PAout(9)
#define	LCD_D1_out PCout(7)
#define LCD_D2_out PAout(10)
#define LCD_D3_out PBout(3)
#define LCD_D4_out PBout(5)
#define LCD_D5_out PBout(4)
#define LCD_D6_out PBout(10)
#define LCD_D7_out PAout(8)

// input
#define	LCD_D0_i_addr PAi_addr(9)
#define	LCD_D1_i_addr PCi_addr(7)
#define LCD_D2_i_addr PAi_addr(10)
#define LCD_D3_i_addr PBi_addr(3)
#define LCD_D4_i_addr PBi_addr(5)
#define LCD_D5_i_addr PBi_addr(4)
#define LCD_D6_i_addr PBi_addr(10)
#define LCD_D7_i_addr PAi_addr(8)

#define	LCD_D0_in PAin(9)
#define	LCD_D1_in PCin(7)
#define LCD_D2_in PAin(10)
#define LCD_D3_in PBin(3)
#define LCD_D4_in PBin(5)
#define LCD_D5_in PBin(4)
#define LCD_D6_in PBin(10)
#define LCD_D7_in PAin(8)

// ctrl
#define LCD_RD PAout(0)
#define LCD_WR PAout(1)
#define LCD_RS PAout(4) // D/CX
#define LCD_CS PBout(0)
#define LCD_RST PCout(1)
	
//typedef struct{
//	volatile unsigned long * addr_D0;
//	volatile unsigned long * addr_D1;
//	volatile unsigned long * addr_D2;
//	volatile unsigned long * addr_D3;
//	volatile unsigned long * addr_D4;
//	volatile unsigned long * addr_D5;
//	volatile unsigned long * addr_D6;
//	volatile unsigned long * addr_D7;
//} LCDdataOut_TypeDef;


//typedef struct{
//	volatile unsigned long * addr_D0;
//	volatile unsigned long * addr_D1;
//	volatile unsigned long * addr_D2;
//	volatile unsigned long * addr_D3;
//	volatile unsigned long * addr_D4;
//	volatile unsigned long * addr_D5;
//	volatile unsigned long * addr_D6;
//	volatile unsigned long * addr_D7;
//} LCDdataIn_TypeDef;

//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左

#define DFT_SCAN_DIR  R2L_U2D  //默认的扫描方向

//画笔颜色
#define WHITE           0xFFFF
#define BLACK           0x0000
#define BLUE            0x001F
#define BRED            0XF81F
#define GRED            0XFFE0
#define GBLUE           0X07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define GREEN           0x07E0
#define CYAN            0x7FFF
#define YELLOW          0xFFE0
#define BROWN           0XBC40  //棕色
#define BRRED           0XFC07  //棕红色
#define GRAY            0X8430  //灰色
//GUI颜色

#define DARKBLUE        0X01CF  //深蓝色
#define LIGHTBLUE       0X7D7C  //浅蓝色  
#define GRAYBLUE        0X5458  //灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN      0X841F  //浅绿色 
#define LGRAY           0XC618  //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE       0XA651  //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12  //浅棕蓝色(选择条目的反色)

void LCD_Init(void);                                                        //初始化
void LCD_DisplayOn(void);                                                   //开显示
void LCD_DisplayOff(void);                                                  //关显示
void LCD_Clear(u16 Color);void LCD_Fast_Clear(u16 color);                                                  //清屏
void LCD_SetCursor(u16 Xpos, u16 Ypos);                                     //设置光标
void LCD_DrawPoint(u16 x, u16 y);                                           //画点
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color);                           //快速画点
u16  LCD_ReadPoint(u16 x, u16 y);                                           //读点
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r);                                 //画圆
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);                          //画线
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);                     //画矩形
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);                   //填充单色
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color);            //填充指定颜色
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode);                  //显示一个字符
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size);                   //显示一个数字
void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode);         //显示 数字
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p);   //显示一个字符串,12/16字体

void LCD_RESET(void);
void LCD_WR_CMD(u8 cmd);
void LCD_WR_DATA(u8 data);
u8 LCD_RD_DATA(void);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM_Continue(void);
void LCD_WriteRAM(u16 RGB_Code);
void LCD_Scan_Dir(u8 dir);                                  //设置屏扫描方向
void LCD_Display_Dir(u8 dir);                               //设置屏幕显示方向
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height); //设置窗口//LCD分辨率设置

void LCD_draw_raw16(u16 sx, u16 sy, u16 width, u16 height, u16* frame);
void LCD_draw_raw8_LE(u16 sx, u16 sy, u16 width, u16 height, u8* frame);
void LCD_draw_raw8_BE(u16 sx, u16 sy, u16 width, u16 height, u8* frame);

//LCD分辨率设置
#define SSD_HOR_RESOLUTION      480     //LCD水平分辨率
#define SSD_VER_RESOLUTION      320     //LCD垂直分辨率
//LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH     1       //水平脉宽
#define SSD_HOR_BACK_PORCH      46      //水平前廊
#define SSD_HOR_FRONT_PORCH     210     //水平后廊

#define SSD_VER_PULSE_WIDTH     1       //垂直脉宽
#define SSD_VER_BACK_PORCH      23      //垂直前廊
#define SSD_VER_FRONT_PORCH     22      //垂直前廊
//如下几个参数，自动计算
#define SSD_HT  (SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_BACK_PORCH)
#define SSD_VT  (SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

// Touch --------------------------------------------------------------------------
//XP - LCD6 - PB10 - IN
//XM - RS - PA4 (A2) - ANALOG ADC12_IN4
//YP - WR - PA1 (A1) - ANALOG ADC123_IN1
//YM - LCD7 - PA8 - IN
// analog pins: set IN then analog read OR set analog?
#define enableX() {GPIOA->MODER|=	0X00000300;GPIOB->MODER&=0XFFCFFFFF;}
#define disableX() {GPIOA->MODER&=0XFFFFFCFF;GPIOA->MODER|=0X00000100;GPIOB->MODER&=0XFFCFFFFF;GPIOB->MODER|=0X00100000;PBout(10)=1;PAout(4)=0;}
#define enableY() {GPIOA->MODER&=0XFFFCFFF3;GPIOA->MODER|=0X0000000C;}
#define disableY() {GPIOA->MODER&=0XFFFCFFF3;GPIOA->MODER|=0X00010004;PAout(1)=1;PAout(8)=0;}
#define enableZ() {GPIOA->MODER&=0XFFFCFCF3;GPIOB->MODER&=0XFFCFFFFF;GPIOA->MODER|=0X0001030C;GPIOB->MODER|=0X00100000;PBout(10)=0;PAout(8)=1;}

#define MIN_PRES 300
#define MAX_PRES 1000

#endif  

