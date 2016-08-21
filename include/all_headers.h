#ifndef _ALL_HEADERS_H
#define _ALL_HEADERS_H

/*comp*/
extern void timer0_irq(void);
/*定时器2调用函数*/
extern void timer2_irq(void);
/*定时器2工作函数*/
extern void timer2_run(void);
/*定时器3调用函数*/
extern void timer3_irq(void);
extern void timer3_run(void);
/*停止工作状态*/
extern void work_stop();
/*外部中断状态*/
extern void key_int_set(int i);
/*显示数字*/
extern void number_show(unsigned int x, unsigned int y, unsigned char *numbers ,unsigned int num, unsigned font_with,unsigned int color);
/*欢迎动画*/
extern void welcome();
/*跳转页面*/
extern void turning();
/*开机等待*/
extern void waiting();
/*方向导航*/
extern void fangxiang();
/*主程序函数*/
extern void working();
/*函数入口*/
extern void comp_Test(void);
/*动画*/
extern void starting(void);

/*timer*/
/*定时器初始化*/
extern void timer_init(void);
/*定时器使能*/
extern void timer_start(unsigned int num,unsigned int tcn, unsigned int tcm);
/*定时器停止*/
extern void timer_stop(unsigned int num);
/*定时器参数更新*/
extern void timer_update(unsigned int num, unsigned int tcn, unsigned int tcm, unsigned int state);


/*lcd*/

/*画像素*/
extern void lcd_draw_pixel(int x, int y, int color);
/*画圈*/
extern void lcd_draw_circle(int centerX, int centerY, int radius, int color);
/*画圆*/
extern void lcd_draw_circle_block(int centerX, int centerY, int radius, int color);
/*任意线*/
extern void lcd_draw_line_free(int startx,int starty,int endx,int endy,int with, int color);
/*清屏*/
extern void lcd_clear_screen(int color);
/*百叶窗清屏*/
extern void lcd_baiyue_screen(int color);
/*横线与竖线*/
extern void lcd_draw_line_xy(int startx,int starty,int endx,int endy,int with, int color);
/*实心矩形*/
extern void lcd_draw_color_rect(int startx,int starty,int with,int heigth, int color);
/*空心矩形*/
extern void lcd_draw_rect(int startx, int starty, int with, int heigth, int linewith, int color);
//选择框
extern void lcd_select_rect(int s, int color);
/*选择框闪烁*/
extern void lcd_run_select_rect();
/*LCD初始化*/
extern void lcd_init();
/*图片显示*/
extern void lcd_draw_pic(int x_s, int y_s, int pc_w, int pc_h, const unsigned char *picture_buf);

/****************************************************
 *函数名称:lcd_show_font
 *说    明:每次只能显示一个汉字或一个数字、字母
 *****************************************************/
extern void lcd_show_font(unsigned int   x,         	//x坐标
                   unsigned int   y,         	//y坐标
				   const unsigned char *font_buf,    	//点阵缓冲区
				   unsigned int   font_width, 	//点阵宽度
				   unsigned int   asc_or_hz,  	//0:asc  1:汉字
				   unsigned int   font_color); 	//颜色值
/*文字显示*/
extern void lcd_draw_fonts(unsigned int   x,         	//x坐标
                   unsigned int   y,         	//y坐标
				   const unsigned char  font_buf[],    	//点阵缓冲区
				   unsigned int   font_width, 	//点阵宽度
				   unsigned int   asc_or_hz,  	//0:asc  1:汉字
				   unsigned int   font_color,
				   unsigned int   len) ;          //显示多少个字

/*主界面显示*/
extern void lcd_main();




#endif