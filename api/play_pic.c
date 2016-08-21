#include "stdio.h"		//printf scanf ....
#include "s5pv210.h"	//寄存器
#include "font.h"	    //字库

#include "showpic01.h"	//图片
#include "showpic02.h"	//图片
#include "showpic03.h"	//图片
#include "showpic04.h"	//图片
#include "play_pic_c.h"	//图片
#include "all_headers.h"

/*颜色列表*/
enum {WHITE=0xFFFFFF, RED=0XFF0000,YELLOW=0XFFFF00,BLUE=0X0000CD,BLACK=0X0, GREEN=0X7CFC00, LIGHTBLUE=0x39ecf7};

/*图片缓存*/
volatile unsigned char *showpic[5];
/*图片播放标记*/
volatile int pic_i;
/*自动还是手动标记*/
volatile int auto_or_hand;
/*自动还是手动选择*/
volatile char chice;


/*延时*/
static void pic_delay()
{
	unsigned int i=0x5000000;
	while(i--);
}
/*test*/
void pic_test(void)
{
	showpic[0] = gImage_showpic01;
	showpic[1] = gImage_showpic02;
	showpic[2] = gImage_showpic03;
	showpic[3] = gImage_showpic04;
	
	pic_i = 0;
	auto_or_hand = -1;
	chice == 0;
	lcd_baiyue_screen(0x635959);
	pic_lcd_draw_pic(0, 0, 800, 480,gImage_play_pic_c);
	fangxiang();
	pic_delay();
	play_choose(0);
	//if(auto_or_hand==1)
	//{
	//timer_start(1, 1, 0);
			
	//}	
}


/*是否放映*/
int play_start()
{
	if(chice == -1)
		return 1;
	if(chice == 0)
		timer_start(1, 1, 0);
	else
	{
		next_pic();
	}
	chice == -1;
	return 0;
}

/*选择框*/
void play_choose(int i)
{
	chice = i;
	if(i  == 0)
	{
		lcd_draw_rect(255, 268, 100, 39, 3, WHITE);
		lcd_draw_rect(442, 268, 100, 39, 3, 0x635959);	
	}	
	else if(i  == 1)
	{
		lcd_draw_rect(442, 268,  100, 39, 3, WHITE);
		lcd_draw_rect(255, 268,  100, 39, 3, 0x635959);
	}
}

/*百叶窗显示图片*/
void pic_lcd_draw_pic(int x_s, int y_s, int pc_w, int pc_h, const unsigned char *picture_buf)
{
	int x=0, y=0;
	int i = 0;
	int h=pc_h/5;
	int red, blue, green, color;
	
	const unsigned char *pic = picture_buf;
	
	const unsigned char *pixel = picture_buf;
	
	for(y=x_s; y<x_s+h; y++)
	{
		for(x=y_s; x<x_s+pc_w; x++)
		{
			for(i=0; i<5; i++)
			{
				pixel =pic+((y+i*h)*800+x)*3;
				
				blue = *pixel++;
				green = *pixel++;
				red = *pixel++;
				
				color = red<<16 | green<<8 | blue<<0;
				lcd_draw_pixel(x, y+i*h, color);
			}
			pic_delay();
			pic_delay();
			pic_delay();
		}
		
	}
	
}

/*下一张图片显示*/
void next_pic()
{
	pic_lcd_draw_pic(0, 0, 800, 480,showpic[pic_i]);
	
	pic_i++;
	if(pic_i == 4)
	{
		pic_i = 0;
	}
}

/*上一张图片显示*/
void last_pic()
{
	pic_lcd_draw_pic(0, 0, 800, 480,showpic[pic_i]);
	
	pic_i--;
	if(pic_i == -1)
	{
		pic_i = 3;
	}
}

/*自动播放*/
void play_pic(void)
{
	next_pic();
}

