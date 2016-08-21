#include "stdio.h"		//printf scanf ....
#include "s5pv210.h"	//寄存器
#include "font.h"	    //字库
#include "pwm_pic2.h"    //图片
#include "pwm_au.h"     //图片
#include "pwm_ha.h"     //图片
#include "all_headers.h"


enum {WHITE=0xFFFFFF, RED=0XFF0000,YELLOW=0XFFFF00,BLUE=0X0000CD,BLACK=0X0, GREEN=0X7CFC00};
/*占空比*/
volatile unsigned int score;
volatile unsigned int up;
volatile unsigned int bo;
const unsigned int back_color_pwm = 0xcd10c2;
volatile int au_ha;

/*pwm初始化*/
void pwm_init()
{
	printf("test pwm\r\n");
	rGPD0CON&=~(0xF<<0);
	rGPD0CON|= (0x2<<0);
	
	score=10;
	up = 1;
	bo = 1;
	au_ha ==1;
	pwm_show(score, WHITE);
	lcd_draw_pic(720 ,10, 50, 50, gImage_pwm_au);
	lcd_draw_pic(10 ,10, 50, 50, gImage_pwm_ha);
	
	pwm_auto();
}

/*pwm自动控制*/
void pwm_auto()
{	
	timer_start(0, 100, 10);
	timer_start(1, 1, 0);
	timer_start(2, 10, 0);
	au_ha = 0;
	
}

/*pwm手动控制*/
int pwm_hand()
{
	if(au_ha ==0)
	{
		au_ha = 1;
		timer_stop(1);
		return 0;
	}
	return 1;
}

/*显示*/
void pwm_show(unsigned int num, unsigned int color)
{
	//int low = score%10;
	unsigned char *numbers = font_number_32;
	unsigned int high= num/10;
	unsigned int offset = high*16*32/8;
	lcd_show_font(340, 40, numbers+offset, 16, 0, color);
	lcd_show_font(358, 40, numbers, 16, 0, color);
	
	lcd_show_font(140, 40, numbers+16*32/8, 16, 0, color);
	lcd_show_font(158, 40, numbers, 16, 0, color);
	lcd_show_font(178, 40, numbers, 16, 0, color);
	if(up)
		lcd_show_font(196, 40, numbers, 16, 0, back_color_pwm);
	else
		lcd_show_font(196, 40, numbers, 16, 0, color);
	
	
}

/*pwm 工作*/
void pwm_running()
{
	printf("--%d%--\r\n", score);	
	pwm_show(score, back_color_pwm);
	lcd_draw_color_rect(250,400+80-score*4, 40, 40, back_color_pwm);
	
	if(up == 1)
	{
		if(score == 10)
		{
			timer_update(0, 100, 10, 0);
			//timer_update(2, 4, 0, 3);
			lcd_draw_color_rect(100,80, 40, 400, back_color_pwm);
			lcd_draw_color_rect(100,400+80-40, 40, 40, YELLOW);
		}
			
		else
			timer_update(0, 100, 10, 1);
		
		score += 10;
		if(score >= 90)
			up = 0;
	}
	else
	{
		if(score == 90)
		{
			timer_update(0,1000, 1, 0);
			//timer_update(2, 5, 0, 3);
			lcd_draw_color_rect(100,80, 40, 400, YELLOW);
		}	
		timer_update(0,1000, 10, 2);
		score -= 10;
		if(score <= 10)
			up = 1;
	}
	pwm_show(score, WHITE);
	lcd_draw_color_rect(250,400+80-score*4, 40, score*4, YELLOW);
}

/*声波动态显示*/
void shengbo(void)
{
	lcd_draw_bo(600 ,384, bo*10, back_color_pwm);
	lcd_draw_bo(600 ,384, bo*15, back_color_pwm);
	lcd_draw_bo(600 ,384, bo*20, back_color_pwm);
	
	lcd_draw_pic(600 ,384, 80, 96, gImage_pwm_pic2);
	bo++;
	lcd_draw_bo(600 ,384, bo*10, WHITE);
	lcd_draw_bo(600 ,384, bo*15, WHITE);
	lcd_draw_bo(600 ,384, bo*20, WHITE);
	
	if(bo == 12) bo = 1;
	
	lcd_draw_bo(600 ,384, 12*20, back_color_pwm);
	
}
/*test*/
void pwm_test(void)
{
	
	lcd_baiyue_screen(back_color_pwm);
	lcd_draw_fonts(70, 40, font_pinli_32,   32, 1, WHITE,   sizeof(font_pinli_32));
	lcd_draw_fonts(220, 40, font_zhakubi_32, 32, 1, WHITE, sizeof(font_zhakubi_32));
	lcd_draw_fonts(375, 40, font_percent_32, 16, 0, WHITE, sizeof(font_percent_32));
	
	lcd_draw_pic(600 ,384, 80, 96, gImage_pwm_pic2);
	
	lcd_draw_rect(100, 80, 40, 400, 2, WHITE);
	lcd_draw_rect(250, 80, 40, 400, 2, WHITE);
	
	pwm_init();
}

/*画圈*/
void lcd_draw_bo(int centerX, int centerY, int radius, int color)
{
	int x, y;
	for(y = 0; y < 420; y++)
	{
		for(x = 0; x < 630; x++)
		{
			if(((x-centerX)*(x-centerX) + (y-centerY)*(y-centerY))>=radius*radius &&((x-centerX)*(x-centerX) + (y-centerY)*(y-centerY))<=(radius+2)*(radius+2))
				lcd_draw_pixel(x, y, color);
		}
	}
}