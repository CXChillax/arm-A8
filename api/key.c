#include "stdio.h"		//printf scanf ....
#include "s5pv210.h"	//寄存器
#include "font.h"	    //字库
#include "all_headers.h"

#define   TRUE  1
#define   FALSE 0 

/*颜色列表*/
enum {WHITE=0xFFFFFF, RED=0XFF0000,YELLOW=0XFFFF00,BLUE=0X312288,BLACK=0X0, GREEN=0X7CFC00, LIGHTBLUE=0x39ecf7};
/*功能模块标识符*/
enum {MAIN, INF, LED, PWM, PIC, NAND, ADC};
/*功能模块标记*/
extern volatile int flag;
/*桌面游标*/
extern volatile int cur;
extern const int colors[5];
extern volatile int led_runing;
/*开机等待标记*/
extern volatile unsigned int  wait;
//extern volatile unsigned int  in_or_set;
//extern volatile unsigned int settime_flag;
/*pwm的自动和手动标记*/
extern volatile int auto_or_hand;

const int block_color[]={BLACK, WHITE, BLUE, RED, RED, YELLOW, BLUE};

/*桌面游标向左*/
static void turn_left()
{

	lcd_select_rect(cur, BLUE);
	
	switch(cur)
	{
		case INF:
			cur=ADC;
			break;
		case LED:
			cur=INF;
			break;
		case PIC:
			cur=PWM;
			break;
		case PWM:
			cur=LED;
			break;
		case NAND:
			cur=PIC;
			break;
		case ADC:
			cur=NAND;
			break;
	}
	lcd_select_rect(cur, GREEN);
	
	
	
}
/*桌面游标向右*/
static void turn_right()
{
	lcd_select_rect(cur, BLUE);
			
	switch(cur)
	{
		case INF:
			cur=LED;
			break;
		case LED:
			cur=PWM;
			break;
		case PIC:
			cur=NAND;
			break;
		case PWM:
			cur=PIC;
			break;
		case NAND:
			cur=ADC;
			break;
		case ADC:
			cur=INF;
			break;
	}
	lcd_select_rect(cur, GREEN);
}
/*桌面游标向上*/
static void turn_up()
{
	lcd_select_rect(cur, BLUE);
		
	switch(cur)
	{
	case INF:
		cur=PIC;
		break;
	case LED:
		cur=NAND;
		break;
	case PIC:
		cur=INF;
		break;
	case PWM:
		cur=ADC;
		break;
	case NAND:
		cur=LED;
		break;
	case ADC:
		cur=PWM;
		break;
	}
	lcd_select_rect(cur, GREEN);
}
/*桌面游标向下*/
static void turn_down()
{
	lcd_select_rect(cur, BLUE);
			
		
	switch(cur)
	{
		case INF:
			cur=PIC;
			break;
		case LED:
			cur=NAND;
			break;
		case PIC:
			cur=LED;
			break;
		case PWM:
			cur=ADC;
			break;
		case NAND:
			cur=LED;
			break;
		case ADC:
			cur=PWM;
			break;
	}
	
	lcd_select_rect(cur, GREEN);
}

/*进入功能模块工作*/
static void cur_in(unsigned int i)
{
	timer_stop(0);
	
	switch(i)
	{
		case LED: 
			led_runing = 0;
			led_test();
			break;
		case INF:
			myself_test();
			break;
		case PWM: 
			pwm_test();
			break;
		case PIC:
			pic_test();
			break;
		case NAND: 
			nand_test();
			break;
		case ADC:
			adc_test();
		
	}
}

/*回到桌面*/
static void cur_out()
{
	timer_stop(1);
	timer_stop(2);
	if(flag != MAIN)
			lcd_main();
	flag = MAIN;
}

static void key_delay()
{
	unsigned int i= 0x5000;
	while(i--);
}
/*中断16函数*/
static void eint16_31_irq_handler(void)
{
		
		//beack
		if(rEXT_INT_2_PEND & (1<<0))
		{
			printf("KEY2 Down\r\n");
			key_int_set(2);
			key_delay();
			
			/*中断标志位挂起*/
			rEXT_INT_2_PEND |= 1<<0;
		}
		///HOME
		else if(rEXT_INT_2_PEND & (1<<1))
		{
			printf("KEY3 Down\r\n");
			key_int_set(3);
			key_delay();
			
			/*中断标志位挂起*/
			rEXT_INT_2_PEND |= 1<<1;
		}		
		////MENU/确定
		else if(rEXT_INT_2_PEND & (1<<2))
		{
			printf("KEY4 Down\r\n");
			key_int_set(4);
			key_delay();
		
			/*中断标志位挂起*/
			rEXT_INT_2_PEND |= 1<<2;
		}	
		/*KEY5*/
		else if(rEXT_INT_2_PEND & (1<<3))
		{
			printf("KEY5 Down\r\n");
			key_int_set(5);
			key_delay();
			
			/*中断标志位挂起*/
			rEXT_INT_2_PEND |= 1<<3;
		}	
		
		
		/////////////////////////////////////
		/*上移KEY6*/
		if(rEXT_INT_3_PEND & (1<<0))
		{
			printf("KEY6 Down\r\n");
			key_int_set(6);
			key_delay();
			
			/*中断标志位挂起*/
			rEXT_INT_3_PEND |= 1<<0;
		}
		/*下移KEY7*/
		else if(rEXT_INT_3_PEND & (1<<1))
		{
			printf("KEY7 Down\r\n");
			key_int_set(7);
			key_delay();
			
			/*中断标志位挂起*/
			rEXT_INT_3_PEND |= 1<<1;
		}		
		/*左移KEY8*/
		else if(rEXT_INT_3_PEND & (1<<2))
		{
			printf("KEY8 Down\r\n");
			key_int_set(8);
			key_delay();
			
			/*中断标志位挂起*/
			rEXT_INT_3_PEND |= 1<<2;
		}	
		/*右移KEY9*/
		else if(rEXT_INT_3_PEND & (1<<3))
		{
			printf("KEY9 Down\r\n");
			key_int_set(9);
			key_delay();
			
			/*中断标志位挂起*/
			rEXT_INT_3_PEND |= 1<<3;
		}
	
		
		/* 完成中断处理的时候，清零所有的VICADDRESS */
		rVIC0ADDRESS = 0;
}

/*外部中断初始化*/
void key_init(void)
{
	/* 1.初始化GPH2_0/1/2引脚为外部中断16模式 */
	rGPH2CON |= (0xFFFF<<0);
	rGPH3CON |= (0xFFFF<<0);
	
	/* 1.初始化GPJ2_0引脚为输出模式LED */
	rGPJ2CON &= ~(0xFFFF<<0);
	rGPJ2CON |=  (0x1111<<0);
	rGPJ2DAT = ~0;
	
	/* 2.配置外部中断16为下降沿触发 */
	rEXT_INT_2_CON &=~(0x7777<<0);
	rEXT_INT_2_CON |= (0x2222<<0);
	rEXT_INT_3_CON &=~(0x7777<<0);
	rEXT_INT_3_CON |= (0x2222<<0);
	
	/* 3.使能外部中断16中断 */
	rEXT_INT_2_MASK &= ~(0xF<<0);
	rEXT_INT_3_MASK &= ~(0xF<<0);

	/* 4.清除外部中断16标志位 */	
	rEXT_INT_2_MASK &= ~(0xF<<0);
	rEXT_INT_3_MASK &= ~(0xF<<0);
	
	/* 5.配置VIC0的通道16为IRQ中断 */
	rVIC0INTSELECT &=~(1<<16);
	
	/* 6.允许VIC0的通道16为IRQ产生中断 */
	rVIC0INTENABLE |= (1<<16);	
	
	/* 7.VIC0通道16安装中断服务函数 */
	rVIC0VECTADDR16 = (unsigned int )eint16_31_irq_handler;
}

/*外部中断*/
void key_int_run(int i)
{
	switch(i)
	{
		case 2:
			key2_run();
			break;
		case 3:
			key3_run();
			break;
		case 4:
			key4_run();
			break;
		case 5:
			key5_run();
			break;
		case 6:
			key6_run();
			break;
		case 7:
			key7_run();
			break;
		case 8:
			key8_run();
			break;
		case 9:
			key9_run();
			break;
	}
	wait = 0;
	work_stop();
}

void key2_run()
{
	/*上一张图片*/
	if(flag == PIC)
		last_pic();
	/*if(in_or_set == 0 && flag < 0 && settime_flag == 1)
	{
		settime_flag = 0;
		time_enable(TRUE);
	}*/
	/*pwm手动控制*/
	if(flag == PWM)
	{
		pwm_hand();
	}
			
}
 void key3_run()
{
	/*回到桌面*/
	if(flag > 0)
		cur_out();
			
}
 void key4_run()
{
	/*下一张图片*/
	if(flag == PIC)
	{
		if(play_start())
			next_pic();
	}
	/*pwm自动控制*/
	if(flag == PWM)
	{
		pwm_auto();
	}
	/*进入功能模块*/		
	if(flag == MAIN)
	{
		flag = cur;
		cur_in(cur);
	}
	/*进入系统*/
	/*if(in_or_set && flag < 0)
	{
				wait = 0;
	}
	else if(in_or_set == 0 && flag < 0 && settime_flag == 0)
	{
		settime_flag = 1;
		time_enable(FALSE);
		rtc_lcd_show(220, 200,font_number_32, 16, WHITE, LIGHTBLUE);
		//lcd_draw_color_rect(220,200, 19*18, 33,LIGHTBLUE);
		//rtc_time_show(220, 200,font_number_32, 16, WHITE, LIGHTBLUE);
	}
	else if(in_or_set == 0 && flag < 0 && settime_flag == 1)
	{
		settime_flag = 0;
		rtc_settime();
		time_enable(TRUE);
	}*/
	
}
void key5_run()
{
	/*LED0*/
	if(flag == LED)
		one_led(0);
}
void key6_run()
{
	if(flag == MAIN)
		turn_up();
	/*LED1*/
	if(flag == LED)
		one_led(1);
}
void key7_run()
{
	if(flag == MAIN)
		turn_down();
	/*LED2*/
	if(flag == LED)
		one_led(2);
	
	if(flag == NAND)
	{
		read_from_nand();
	}	
}
void key8_run()
{
	/*if(flag < 0)
	{
		if(settime_flag == 0)
			in_or_set = 1;
	}*/
	
	if(flag == MAIN)
		turn_left();
	/*LED3*/
	if(flag == LED)
		one_led(3);
	if(flag == NAND)
	{
		erase_nand();
	}
	
	if(flag == PIC)
	{
		play_choose(0);
	}
}
 void key9_run()
{
	/*if(flag < 0)
	{
		if(settime_flag == 0)
		{
			in_or_set = 0;
			
			printf("--------\r\n");
		}
	}*/

	if(flag == MAIN)
		turn_right();
	/*开启流水灯*/
	if(flag == LED)
	{
		if(led_runing == 0)
		{
			timer_start(1, 1, 0);
			led_runing = 1;
		}
		else if(led_runing == 1)
		{
			timer_stop(1);
			led_runing = 0;
		}
		
	}
	if(flag == NAND)
	{
		write_to_nand();
	}
	
	if(flag == PIC)
	{
		play_choose(1);
	}
	
	if(flag == PWM)
	{
		if(pwm_hand())
		{
			pwm_running();
		}
	}
}
