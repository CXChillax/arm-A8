#ifndef _COMP_H
#define _COMP_H

#include "stdio.h"		//printf scanf ....
#include "s5pv210.h"	//寄存器
#include "font.h"	    //字库

#include "gifh/001.h"	
#include "gifh/002.h"	
#include "gifh/003.h"	
#include "gifh/004.h"	
#include "gifh/005.h"	
#include "gifh/006.h"	
#include "gifh/007.h"	
#include "gifh/008.h"	
#include "gifh/009.h"	
#include "gifh/010.h"	
#include "gifh/011.h"	
#include "gifh/012.h"	
#include "gifh/013.h"	
#include "gifh/014.h"	
#include "gifh/015.h"	
#include "gifh/016.h"	
#include "gifh/017.h"	
#include "gifh/018.h"	
#include "gifh/019.h"	
#include "gifh/020.h"	
#include "gifh/021.h"	
#include "gifh/022.h"	
#include "gifh/023.h"	
#include "gifh/024.h"	
#include "gifh/025.h"	
#include "gifh/026.h"	
#include "gifh/027.h"	
#include "gifh/028.h"	
#include "gifh/029.h"	
#include "gifh/030.h"	
#include "gifh/031.h"	
#include "gifh/032.h"	
#include "gifh/033.h"	
#include "gifh/034.h"	
#include "gifh/035.h"	
#include "gifh/036.h"	
#include "gifh/037.h"	
#include "gifh/038.h"	
#include "gifh/039.h"	
#include "gifh/040.h"	
#include "gifh/041.h"	
#include "gifh/042.h"	
#include "gifh/043.h"	
#include "gifh/044.h"	
#include "gifh/045.h"	
#include "gifh/046.h"	
#include "gifh/047.h"	
#include "gifh/048.h"	
#include "gifh/049.h"	
#include "gifh/050.h"	
#include "gifh/051.h"	
#include "gifh/052.h"	
#include "gifh/053.h"	
#include "gifh/054.h"	
#include "gifh/055.h"	
#include "gifh/056.h"	
#include "gifh/057.h"	
#include "gifh/058.h"	
#include "gifh/059.h"	
#include "gifh/060.h"	
#include "gifh/061.h"	
#include "gifh/062.h"	
#include "gifh/063.h"	
#include "gifh/064.h"	
#include "gifh/065.h"	
#include "gifh/066.h"	
#include "gifh/067.h"	
#include "gifh/068.h"	
#include "gifh/069.h"	
#include "gifh/070.h"	
#include "gifh/071.h"	
#include "gifh/072.h"	
#include "gifh/073.h"	
#include "gifh/074.h"	
#include "gifh/075.h"	
#include "gifh/076.h"	
#include "gifh/077.h"	
#include "gifh/078.h"	
#include "gifh/079.h"	
#include "gifh/080.h"	
#include "gifh/081.h"	
#include "gifh/082.h"	
#include "gifh/083.h"	
#include "gifh/084.h"	
#include "gifh/085.h"	
#include "gifh/086.h"	
#include "gifh/087.h"	
#include "gifh/088.h"	
#include "gifh/089.h"	
#include "gifh/090.h"	
#include "gifh/091.h"	
#include "gifh/092.h"	
#include "gifh/093.h"	
#include "gifh/094.h"	
#include "gifh/095.h"	
#include "gifh/096.h"	
#include "gifh/097.h"	
#include "gifh/098.h"	
#include "gifh/099.h"	
#include "gifh/100.h"	
#include "gifh/101.h"	
#include "gifh/102.h"	
#include "gifh/103.h"	
#include "gifh/104.h"	

#include "tv_pic.h"	      
#include "start.h"	
#include "daohang_01.h"  	//图片 
#include "daohang_02.h"  	//图片 
#include "daohang_03.h"  	//图片 
#include "daohang_04.h"  	//图片 
#include "daohang_001.h"  	//图片 
#include "daohang_002.h"  	//图片 
#include "daohang_003.h"  	//图片 
#include "daohang_004.h"  	//图片

/*延时*/
void delay(volatile unsigned int i);
/*除法库和浮点库的必须函数*/
void raise(void);
/*定时器0调用函数*/
void timer0_irq(void);
/*定时器2调用函数*/
void timer2_irq(void);
/*定时器2工作函数*/
void timer2_run(void);
/*定时器3调用函数*/
void timer3_irq(void);
void timer3_run(void);
/*停止工作状态*/
void work_stop();
/*外部中断状态*/
void key_int_set(int i);
/*显示数字*/
void number_show(unsigned int x, unsigned int y, unsigned char *numbers ,unsigned int num, unsigned font_with,unsigned int color);
/*欢迎动画*/
void welcome();
/*跳转页面*/
void turning();
/*开机等待*/
void waiting();
/*方向导航*/
void fangxiang();
/*主程序函数*/
void working();
/*函数入口*/
void comp_Test(void);
/*动画*/
void starting(void);
 
#endif
