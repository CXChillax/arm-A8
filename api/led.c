#include "stdio.h"		//printf scanf ....
#include "s5pv210.h"	//寄存器

#include "light_on.h"
#include "light_off.h"

enum {WHITE=0xFFFFFF, RED=0XFF0000,YELLOW=0XFFFF00,BLUE=0X0000CD,BLACK=0X0, GREEN=0X7CFC00};
volatile unsigned int light_on[4];
const int back_color = 0xd47c1a;
volatile int led_runing;
volatile int run_i = 0;

//gImage_lingt_on

//延时
void led_delay(volatile unsigned int i)
{	
	while(i--);	
}

void draw_light(unsigned int x, unsigned int y, unsigned int with, unsigned int height,const unsigned char *gImage_light)
{
	lcd_draw_pic(x, y, with, height, gImage_light);
}

void light_on_off(unsigned int num, unsigned int on)
{
	volatile int x = 30, y = 0, w =200, h=400 ;
	volatile int d = 175;
	switch(num)
	{
		
		case 0:
			if(on)
				 draw_light(x, y, w, h, gImage_light_on);
			else
				 draw_light(x, y, w, h, gImage_light_off);
			break;
		case 1:
			if(on)
				draw_light(x+d, y, w, h, gImage_light_on);
			else
				draw_light(x+d, y, w, h, gImage_light_off);
			break;
		case 2:
			if(on)
				draw_light(x+2*d, y, w, h,  gImage_light_on);
			else
				draw_light(x+2*d, y, w, h, gImage_light_off);
			break;
		case 3:
			if(on)
				draw_light(x+3*d, y, w, h,  gImage_light_on);
			else
				draw_light(x+3*d, y, w, h, gImage_light_off);

	}
}

//点亮LEDi
void one_led(int i)
{
	rGPJ2DAT ^= (1<<i);	
	if(light_on[i] == 0)
		light_on[i] = 1;
	else
		light_on[i] = 0;
	
	light_on_off(i, light_on[i]);
}

//流水灯
void run_led()
{
	
	/*检测前一盏灯有没有亮，如果亮就熄灭*/
	if(run_i > 0)
	{
		if(light_on[run_i-1] == 1)
		{
			light_on[run_i-1] = 0;
			light_on_off(run_i-1, light_on[run_i-1]);
		}
	}
	else
	{
		if(light_on[3] == 1)
		{
			light_on[3] = 0;
			light_on_off(3, light_on[3]);
		}
	}
	
	/* 点亮GPJ2_0引脚控制的LED灯 */
	rGPJ2DAT = ~(1<<run_i);
	
	
	if(light_on[run_i] == 0)
		light_on[run_i] = 1;
	else
		light_on[run_i] = 0;
	
	light_on_off(run_i, light_on[run_i]);
	
	run_i++;
	if(run_i == 4)
		run_i = 0;	
	
	printf("led run\r\n---------");
	

	//rGPJ2DAT |= (1<<3);
}

void led_init(void)
{
		/* 1.初始化GPJ2_0引脚为输出模式 */
	rGPJ2CON &= ~(0xFFFF<<0);
	//rGPJ2CON |=  (0x1<<0);
	rGPJ2CON |=  (0x1111<<0);
	
	led_runing = 0;
	
	/*led状态初始化*/
	light_on[0] = 0;
	light_on[1] = 0;
	light_on[2] = 0;
	light_on[3] = 0;
	run_i = 0;
}


/*led测试*/
void led_test(void)
{
	unsigned int i=0;
	
	led_init();
	/*刷背景色*/
	lcd_baiyue_screen(back_color);
	
	for(i=0; i<4; i++)
		light_on_off(i, light_on[i]);
	//draw_light(50,50,40,WHITE);
}


