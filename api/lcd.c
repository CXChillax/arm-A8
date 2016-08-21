#include "stdio.h"		//printf scanf ....
#include "s5pv210.h"	//寄存器

#include "pc02.h"	    //图片
#include "image_me.h"	//图片
#include "pic_s.h"	    //图片
#include "adc.h"	    //图片
#include "pwm_pic.h"    //图片
#include "led_pic.h"	//图片
#include "nand_pic.h"	//图片
#include "wo_pic.h"  	//图片
#include "all_headers.h"


#include "font.h"	    //字库

#define CLKVAL_F 4      //vclk=(HCKL_DSYS)/(CLKVAL_F+1)=33.3M
#define VSPW 9
#define VBPD 12
#define VFPD 21
#define HSPW 19
#define HBPD 25
#define HFPD 209
#define XSIZE 800
#define YSIZE 480

#define ROW	(480)
#define COL	(800)

#define LINEVAL (YSIZE-1)
#define HOZVAL (XSIZE-1)
#define LEFTTOPX 0
#define LEFTTOPY 0
#define RIGHTBOTX (XSIZE-1)
#define RIGHTBOTY (YSIZE-1)
#define FRAMEBUFFER 0X42000000
#define FB_ADDR	   (0x42000000)

/*颜色列表*/
enum {WHITE=0xFFFFFF, RED=0XFF0000,YELLOW=0XFFFF00,BLUE=0X312288,BLACK=0X0, GREEN=0X7CFC00, LIGHTBLUE=0x39ecf7};

/*功能列表*/
enum {MAIN, INF, LED, PWM, PIC, NAND, ADC};  

/*色块结构体*/
struct color_rect
{
	int x;
	int y;
	int with;
	int heigth;
	int color;
}inf_rect ={80,  110, 270, 155, LIGHTBLUE},         //自我介绍
 led_rect ={360, 110, 175, 155, LIGHTBLUE},       	//LED测试
 pwm_rect ={545, 110, 175, 155, LIGHTBLUE},			//pwm测试
 pic_rect ={80,  275, 175, 155, LIGHTBLUE},		    //相册
 nand_rect={265, 275, 270, 155, LIGHTBLUE},	        //nand flash测试/i2c
 adc_rect ={545, 275, 175, 155, LIGHTBLUE};		    //adc测试

/*获取屏幕指针*/
//volatile unsigned (*p_buf)[480][800]=(volatile unsigned (*)[480][800])FRAMEBUFFER; 

/*颜色数组*/
const int colors[]={WHITE, RED, YELLOW, BLUE, BLACK};
/*功能模块数组*/
volatile struct color_rect home_rect[7];

/*当前工作位置*/
volatile int flag = MAIN;

/*游标指向*/
volatile int cur = INF;

/*延时*/
static void lcd_delay(volatile unsigned int i)
{	
	while(i--);	
}

//////////////////////////////
/*画像素*/
void lcd_draw_pixel(int x, int y, int color)
{
	unsigned long * pixel = (unsigned long  *)FB_ADDR;
	*(pixel + y * COL + x) = color; 

}

/*画圈*/
void lcd_draw_circle(int centerX, int centerY, int radius, int color)
{
	int x, y;
	for(y = 0; y < ROW; y++)
	{
		for(x = 0; x < COL; x++)
		{
			if(((x-centerX)*(x-centerX) + (y-centerY)*(y-centerY))>=radius*radius &&((x-centerX)*(x-centerX) + (y-centerY)*(y-centerY))<=(radius+2)*(radius+2))
				lcd_draw_pixel(x, y, color);
		}
	}
}
/*画圆*/
void lcd_draw_circle_block(int centerX, int centerY, int radius, int color)
{
	int x, y;
	for(y = 0; y < ROW ; y++)
	{
		for(x = 0; x < COL; x++)
		{
			if(((x-centerX)*(x-centerX) + (y-centerY)*(y-centerY))<=radius*radius)
				lcd_draw_pixel(x, y, color);
		}
	}
}

/*任意线*/

void lcd_draw_line_free(int startx,int starty,int endx,int endy,int with, int color)
{
	  int x=0, y=0, w=with;
	  int k=(endy-starty)/(endx-startx);
	  for(x=startx; x<endx; x++)
	  {
		  for(y=starty; y<endy; y++)
		  {
			   if(y <= (k*x + w/2) && y >= (k*x - w/2))
					lcd_draw_pixel(x, y, color);
		  }
	  }
}
////////////////////////////////////////////////////////

/*清屏*/
void lcd_clear_screen(int color)
{
	int x=0,y=0;
	for(x=0;x<XSIZE;x++)
	{
		for(y=0;y<YSIZE;y++)
		{
			 lcd_draw_pixel(x, y, color);
		}
		lcd_delay(0x500);
	}
}

/*百叶窗清屏*/
void lcd_baiyue_screen(int color)
{
	int x=0,y=0;
	int i=0;
	int h=YSIZE/5;
	for(y=0;y<h;y++)
	{
		for(x=0;x<XSIZE;x++)
		{			
			for(i=0; i<5; i++)
			{
				lcd_draw_pixel(x, y+i*h, color);
			}
		}
		lcd_delay(0xA0000);
	}
}

/*横线与竖线*/
void lcd_draw_line_xy(int startx,int starty,int endx,int endy,int with, int color)
{
	  int x=0, y=0, w=with;
	  for(x=startx-w/2; x<endx+w/2; x++)
	  {
		  for(y=starty-w/2; y<endy+w/2; y++)
		  {
			  lcd_draw_pixel(x, y, color);
		  }
		  lcd_delay(0x7000);
	  }
}

/*实心矩形*/
void lcd_draw_color_rect(int startx,int starty,int with,int heigth, int color)
{
	int x=0,y=0;
	for(x=startx; x<startx+with; x++)
	{
		for(y=starty; y<starty+heigth; y++)
		{
			 lcd_draw_pixel(x, y, color);
		}
	}
}

/*空心矩形*/
void lcd_draw_rect(int startx, int starty, int with, int heigth, int linewith, int color)
{
	lcd_draw_line_xy(startx, starty, startx+with, starty, linewith, color);
	lcd_draw_line_xy(startx, starty, startx, starty+heigth, linewith, color);
	lcd_draw_line_xy(startx, starty+heigth, startx+with, starty+heigth, linewith, color);
	lcd_draw_line_xy(startx+with, starty, startx+with, starty+heigth, linewith, color);
}

//选择框
void lcd_select_rect(int s, int color)
{
	struct color_rect rect = home_rect[s];	
	//lcd_draw_rect(rect.x-2,rect.y-2,rect.with+4,rect.heigth+4,3,color+5);
	lcd_draw_rect(rect.x,rect.y,rect.with,rect.heigth,5,color);
}

/*选择框闪烁*/
void lcd_run_select_rect()
{
	lcd_select_rect(cur, GREEN+0x00FF);
	lcd_select_rect(cur, GREEN);
}

/*LCD初始化*/
void lcd_init()
{
	/*设置LCD RGB接口的24个管脚为lcd工作模式*/
	rGPF0CON=0x22222222;
	rGPF1CON=0x22222222;
	rGPF2CON=0x22222222;
	rGPF3CON &=~((0xF<<12)|(0xF<<8)|(0xF<<4)|(0xF<<0));	
	rGPF3CON=0x22222222;

	/**/
	rDISPLAY_CONTROL=0x3;

	rVIDCON0=(CLKVAL_F<<6)|(1<<4)|(0X3<<0);
	rVIDCON1=(1<<6)|(1<<5);
	rVIDCON2=0x0;
	rVIDCON3=0x0;

	rVIDTCON0=(VBPD<<16)|(VFPD<<8)|(VSPW<<0);
	rVIDTCON1=(HBPD<<16)|(HFPD<<8)|(HSPW<<0);
	rVIDTCON2= (LINEVAL<<11)|(HOZVAL<<0);

	rVIDTCON3=0X0;

	rWINCON0=(1<<15)|(0XB<<2)|(1<<0);
	rWINCON2&=~1;
	rVIDOSD0A=(LEFTTOPX<<11)|LEFTTOPY;
	rVIDOSD0B=(RIGHTBOTX<<11)|RIGHTBOTY;
	rVIDOSD0C=XSIZE*YSIZE;

	rVIDW00ADD0B0=FRAMEBUFFER;
	rVIDW00ADD1B0=FRAMEBUFFER+XSIZE*YSIZE*4;

	rSHADOWCON=1; 
	
	///主界面分布数组
	home_rect[1] = inf_rect;
	home_rect[2] = led_rect;
	home_rect[3] = pwm_rect;
	home_rect[4] = pic_rect;
	home_rect[5] = nand_rect;
	home_rect[6] = adc_rect;
	
	flag = MAIN;
	cur = INF;

}


/*图片显示*/
void lcd_draw_pic(int x_s, int y_s, int pc_w, int pc_h, const unsigned char *picture_buf)
{
	int x=0, y=0;
	const unsigned char *pic = picture_buf;
	int red, blue, green, color;
	for(y=y_s; y<y_s+pc_h; y++)
	{
		for(x=x_s; x<x_s+pc_w; x++)
		{
			blue = *pic++;
			green = *pic++;
			red = *pic++;
			
			color = red<<16 | green<<8 | blue<<0;
			lcd_draw_pixel(x, y, color);
		}
		
	}
	
}


/****************************************************
 *函数名称:lcd_show_font
 *说    明:每次只能显示一个汉字或一个数字、字母
 *****************************************************/
void lcd_show_font(unsigned int   x,         	//x坐标
                   unsigned int   y,         	//y坐标
				   const unsigned char *font_buf,    	//点阵缓冲区
				   unsigned int   font_width, 	//点阵宽度
				   unsigned int   asc_or_hz,  	//0:asc  1:汉字
				   unsigned int   font_color) 	//颜色值
{
	unsigned int    i,j;         	//执行for循环用到的变量
	unsigned int    x_bak=x;       	//用于备份原来X坐标
	unsigned char  *pt=font_buf;   	//指向点阵数组缓冲区  
	
	unsigned int    point_total=0; 	//像素点总数
	
	/*1. 判断当前是否显示汉字 */
	if(asc_or_hz)
	{
	   //当前为汉字
	   point_total = font_width * font_width;
	}
	else
	{
	   //当前为数字或英文
	   point_total = font_width * font_width *2;
	}
	
	/*2. 显示像素点 */
	for(j = 0; j<point_total/8; j++)
	{
		for(i=0; i<8; i++)
		{
			/*3.检测要显示的点*/
			if((*pt & (1<<(7-i)))!=0)
			{
				/*检测要显示的点的颜色*/
				lcd_draw_pixel(x, y, font_color);
			}
			
			/*4.显示点偏移一个像素点*/
			x++;
			/*5.若显示完一行要进行换行处理*/
			if((x-x_bak)==font_width)
			{
				x=x_bak;
				y++;
			}
	
		}
		/*偏移一个字节*/
		pt++;
	}
}

/*文字显示*/
void lcd_draw_fonts(unsigned int   x,         	//x坐标
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
			
			font_offset +=(add_off/8);//16*16/8=32
			//printf("------\r\n");
	}
	
}

/*主界面显示*/
void lcd_main()
{	
	unsigned int i;
	/*背景颜色*/
	lcd_clear_screen(BLUE);
	
	/*各功能色块*/
	for(i = 1; i < 7; i++)
	{
		lcd_draw_color_rect(home_rect[i].x,home_rect[i].y,home_rect[i].with,home_rect[i].heigth,home_rect[i].color);
	}
	
	/*选择框*/
	lcd_select_rect(cur, GREEN);
	/*选择框闪烁定时器*/
	timer_start(0, 5, 0);
	
	/*图片显示*/
	lcd_draw_pic(home_rect[1].x,home_rect[1].y,home_rect[1].with,home_rect[1].heigth, gImage_image_me);
	lcd_draw_pic(home_rect[4].x+46,home_rect[4].y +27, 80, 96,  gImage_pic_s);
	lcd_draw_pic(home_rect[6].x+46,home_rect[6].y +27, 80, 96,    gImage_adc);
	lcd_draw_pic(home_rect[3].x+46,home_rect[3].y +27, 80, 96,gImage_pwm_pic);
	lcd_draw_pic(home_rect[2].x+46,home_rect[2].y +27, 80, 96,gImage_led_pic);
	lcd_draw_pic(home_rect[5].x+75,home_rect[5].y +33, 120,80,gImage_nand_pic);
	lcd_draw_color_rect(home_rect[1].x,home_rect[1].y+120,home_rect[1].with,home_rect[1].heigth-120,home_rect[1].color);
	
	
	
	/*文字显示*/
	lcd_draw_fonts(home_rect[1].x+5, home_rect[1].y+home_rect[1].heigth-30, font_introduce_24, 24, 1, WHITE, sizeof(font_introduce_24));
	lcd_draw_fonts(home_rect[2].x+5, home_rect[2].y+home_rect[2].heigth-23, font_led_16, 8, 0, WHITE, sizeof(font_led_16));
	lcd_draw_fonts(home_rect[3].x+5, home_rect[3].y+home_rect[3].heigth-23, font_pwm_16, 8, 0, WHITE, sizeof(font_pwm_16));
	lcd_draw_fonts(home_rect[2].x+5+9*3, home_rect[2].y+home_rect[2].heigth-30, font_check_24, 24, 1, WHITE, sizeof(font_check_24));
	lcd_draw_fonts(home_rect[3].x+5+9*3, home_rect[3].y+home_rect[3].heigth-30, font_check_24, 24, 1, WHITE, sizeof(font_check_24));
	
	lcd_draw_fonts(home_rect[4].x+15, home_rect[4].y+home_rect[4].heigth-30, font_pic_24, 24, 1, WHITE, sizeof(font_pic_24));
	lcd_draw_fonts(home_rect[5].x+15, home_rect[5].y+home_rect[5].heigth-23, font_nand_16, 8, 0, WHITE, sizeof(font_nand_16));
	lcd_draw_fonts(home_rect[5].x+5+11*4, home_rect[5].y+home_rect[5].heigth-30, font_check_24, 24, 1, WHITE, sizeof(font_check_24));
	
	lcd_draw_fonts(home_rect[6].x+15, home_rect[6].y+home_rect[6].heigth-30, font_power_24, 24, 1, WHITE, sizeof(font_power_24));
	
	
	//开始
	lcd_draw_fonts(home_rect[1].x, home_rect[1].y-80, font_start_40, 40, 1, WHITE, sizeof(font_start_40));
	/*吴玉波*/
	lcd_draw_fonts(home_rect[3].x+15, home_rect[3].y-55, font_wuyubo_24, 24, 1, WHITE, sizeof(font_wuyubo_24));
	lcd_draw_pic(home_rect[3].x+110,home_rect[3].y-80, 60,60,gImage_wo_pic);

	flag = MAIN;
	/*方向导航*/
	fangxiang();
}
