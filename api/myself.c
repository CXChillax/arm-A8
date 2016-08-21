#include "stdio.h"		//printf scanf ....
#include "s5pv210.h"	//寄存器
#include "font.h"	    //字库
#include "mypic/my_pic01.h"	    //图片
#include "mypic/my_pic04.h"	    //图片  
#include "all_headers.h"
	        

/*颜色列表*/
enum {WHITE=0xFFFFFF, RED=0XFF0000,YELLOW=0XFFFF00,BLUE=0X0000CD,BLACK=0X0, GREEN=0X7CFC00, LIGHTBLUE=0x39ecf7};
/*本页背景颜色*/
const unsigned int back_color_me = 0x711616;
/*自我介绍文字段buff*/
static const unsigned char *introduce_font[8];
/*自我介绍图片buff*/
const unsigned char *introduce_pic[2];
/*字段显示标记*/
static volatile unsigned int font_i;
/*每字段的长度*/
static volatile unsigned int font_num[8];
/*每字段的总大小*/
static volatile unsigned int font_size[8];

/*延时*/
static void my_delay()
{
	volatile unsigned int i = 0x50F00;
	while(i--);
}
/*图片显示*/
void show_me(int i)
{
	my_lcd_draw_pic(0, 0, 800, 480, introduce_pic[i-8]);
}

/*自我介绍文字显示*/
void introduce(const unsigned char fonts[] ,unsigned int with, unsigned int len, unsigned int size,unsigned int color)
{
	my_lcd_draw_fonts(400-with*len/2, 222, fonts, with, 1, color, size);
	
}

/*test函数入口*/
void myself_test() 
{
	lcd_baiyue_screen(back_color_me);
	/*缓存字段*/
	introduce_font[0] =  font_91qi_32;
	introduce_font[1] =  font_wuyubo_32;
	introduce_font[2] =  font_maoming_32;
	introduce_font[3] =  font_school_32;
	introduce_font[4] =  font_gdut_32;
	introduce_font[5] =  font_major_32;
	introduce_font[6] =  font_wuliang_32;
	introduce_font[7] =  font_dream_boy_32;
	
	/*缓存字段长度*/
	font_num[0] = 7;
	font_num[1] = 3;
	font_num[2] = 6;
	font_num[3] = 2;
	font_num[4] = 5;
	font_num[5] = 2;
	font_num[6] = 5;
	font_num[7] = 10;

	/*缓存字段大小*/
	font_size[0] = sizeof(font_91qi_32);
	font_size[1] = sizeof(font_wuyubo_32);
	font_size[2] = sizeof(font_maoming_32);
	font_size[3] = sizeof(font_school_32);
	font_size[4] = sizeof(font_gdut_32);
	font_size[5] = sizeof(font_major_32);
	font_size[6] = sizeof(font_wuliang_32);
	font_size[7] = sizeof(font_dream_boy_32);
	
	/*缓存图片*/
	introduce_pic[0] = gImage_my_pic01;
	introduce_pic[1] = gImage_my_pic04;
	
	font_i = 0;
	/*91*/
	number_show(400-(33*2)/2-15, 222, font_number_32, 9, 16, WHITE);
	number_show(400-(33*2)/2+2, 222, font_number_32, 1, 16, WHITE);
	/*横线*/
	lcd_draw_line_xy(0, 260, 800, 260, 6, WHITE);
	/*开定时器1*/
	timer_start(1, 1, 0);
}

/*文字显示*/
void my_lcd_draw_fonts(unsigned int   x,         	//x坐标
                   unsigned int   y,         	//y坐标
				   const unsigned char  font_buf[],    	//点阵缓冲区
				   unsigned int   font_width, 	//点阵宽度
				   unsigned int   asc_or_hz,  	//0:asc  1:汉字
				   unsigned int   font_color,
				   unsigned int   len)           //显示多少个字
{
	
	unsigned int font_offset=0;
	unsigned int add_off;
	
	
	/*1. 判断当前是否显示汉字 */
	if(asc_or_hz)
	{
	   //当前为汉字
	   add_off = font_width * font_width;
	}
	else
	{
	   //当前为数字或英文
	   add_off = font_width*font_width*2;
	}
	
	for(font_offset=0; font_offset<len;x+=font_width+1)
	{
			
		    lcd_show_font(x,y,&font_buf[font_offset],font_width,asc_or_hz,font_color);
			
			font_offset +=(add_off/8);//
			my_delay();
	}
	
}

/*自右向左刷图片*/
void my_lcd_draw_pic(int x_s, int y_s, int pc_w, int pc_h, const unsigned char *picture_buf)
{
	int x=0, y=0;
	int i = 0;
	int red, blue, green, color;
	const unsigned char *pic = picture_buf;
	const unsigned char *pixel = picture_buf;
	
	for(x=x_s+pc_w; x>y_s; x--)
	{
		for(y=y_s+pc_h; y>x_s; y--)
		{
			pixel =pic+(y*800+x)*3;
			
			blue = *pixel++;
			green = *pixel++;
			red = *pixel++;
			
			i++;
			color = red<<16 | green<<8 | blue<<0;
			lcd_draw_pixel(x, y, color);
		}
		
	}
	
}

/*进入自我介绍*/
void play(void)
{
	if(font_i < 8)
	{
		if(font_i>0)
		{
			introduce(introduce_font[font_i-1], 32, font_num[font_i-1], font_size[font_i-1], back_color_me);
			number_show(400-(33*2)/2-15, 222, font_number_32, 9, 16, back_color_me);
			number_show(400-(33*2)/2+2, 222, font_number_32, 1, 16, back_color_me);
		}
			
		introduce(introduce_font[font_i], 32, font_num[font_i], font_size[font_i],  WHITE);
		font_i++;
	
	}
	else
	{	
		timer_stop(1);
		show_me(font_i);
		font_i++;
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		my_delay();
		show_me(font_i);
		font_i = 0;	
	}
	if(font_i == 10)
	{
		font_i = 0;	
	}
	my_delay();
	my_delay();
	my_delay();
	my_delay();
}