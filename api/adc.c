#include "stdio.h"		//printf scanf ....
#include "s5pv210.h"	//寄存器
#include "font.h"	    //字库
#include "adc_pic2.h"	//图片
#include "adc_pic01.h"	//图片
#include "all_headers.h"

/*颜色列表*/
enum {WHITE=0xFFFFFF, RED=0XFF0000,YELLOW=0XFFFF00,BLUE=0X0000CD,BLACK=0X0, GREEN=0X7CFC00, LIGHTBLUE=0x39ecf7};
/*本页背景颜色*/
const unsigned int back_color_adc = 0x82e42a;

static unsigned int older = 0;

/*adc电压读取*/
static int adc_read(void)
{
		 /* 设置读取结果之前先启动A/D转换
			 rTSADCCON[1]=0
		 */	
		rTSADCCON0 &= ~(1<<1);

		 /* 使能A/D转换
			 rTSADCCON[0]=1 
		 */	
		rTSADCCON0 |= 1<<0;

		 /* 检查rTSADCCON[0]是否清零，使用while进行判断
		 */	

		 while(rTSADCCON0 &(1<<0));
		  
		 /* 检查rTSADCCON[15]是否置1，使用while进行判断
		 */
		//  printf("--------6\r\n");
		 while((rTSADCCON0 & (1<<15)) == 0);
		
		 /* 从rTSDATX0读取结果值，并打印出来
		 */		
		 
		 printf("ADC read successfully\r\n");
		
		return (3300*(rTSDATX0 & 0xFFF))/0x1000;
		
		 			 
}

/*电压显示*/
static void adc_power_show(unsigned int num, unsigned int color)
{
	unsigned int bit4 = 0;
	unsigned int bit3 = 0;
	unsigned int bit2 = 0;
	unsigned int bit1 = 0;
	
	if(num > 1000)
	{
		bit4 = num/1000;
		bit3 = (num%1000)/100;
		bit2 = (num%100)/10;
	}
	else if(num > 100)
	{
		bit3 = num/100;
		bit2 = (num%100)/10;
	}		
	else if(num > 10)
	{
		bit2 = num/10;
	}
	else if(num > 0)
	{
		bit1 = (num%10);
	}
	if(bit4 > 0)
		number_show(475, 153, font_number_32, bit4, 16, color);
	if(bit3 > 0 || bit4 > 0)
		number_show(493, 153, font_number_32, bit3, 16, color);
	if(bit2 > 0 || bit3 > 0 || bit4 > 0)
		number_show(511, 153, font_number_32, bit2, 16, color);
	
	number_show(529, 153, font_number_32,     bit1, 16, color);
}

/*延时*/
static void adc_delay()
{
	unsigned int i = 0x50000;
	while(i--);
}

/*数据读取和显示*/
void adc_running()
{
	
	unsigned int power = 0;
	power = adc_read();
	adc_delay();
	adc_delay();
	adc_delay();
	if(power != older)
	{
		adc_power_show(older, BLACK);
		adc_power_show(power, WHITE);
	}
		
	if(power-older > 100)
	{
		power_change(older, back_color_adc);
		
		if(power < 800 )
		{
			power_change(power, RED);
		}
		else if(power - older > 100)
		{
			power_change(power, YELLOW);
		}
	}
	older = power;
	adc_delay();
	adc_delay();	
	
}
/*adc初始化*/
void adc_init()
{
		rADCMUX = 0;
		/* 使能ADC的预分频功能，设置它的频率=PCLK/(65+1)=1MHz
		   设置它的精度为12bit
		   rTSADCCON[16]=1
		   rTSADCCON[14]=1
		   rTSADCCON[13:6]=65
		 */
		 rTSADCCON0 &= ~(1<<0);
		 rTSADCCON0 = 1<<16 | 1<<14 | 65<<6;
		 	 
		 /* 设置当前的ADC为普通转换模式 
			 rTSADCCON[2]=0 
		 */
		 rTSADCCON0 &= ~(1<<2);
}

/*电池变化显示*/
void power_change(int num, unsigned int color)
{
	if(num > 100)
		lcd_draw_color_rect(44, 319-(num/100)*6, 109, (num/100)*6, color);
}

/*test*/
void adc_test(void)
{
	lcd_baiyue_screen(back_color_adc);
	lcd_draw_pic(0 ,0, 800, 480, gImage_adc_pic01);
	
	adc_init();
	older = 0;
	timer_start(1, 1, 0);
}

