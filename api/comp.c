#include "comp.h"	        
#include "all_headers.h"	        
#define   TRUE  1
#define   FALSE 0     
#define  nowork -1

/*�жϱ�ʶ��*/
enum {timer0, timer1, timer2, timer3, key2, key3, key4, key5, key6, key7, key8, key9};

/*��¼���жϹ���״̬*/
volatile int work;
/*�����ȴ����*/
volatile unsigned int  wait;
//volatile unsigned int  in_or_set;
//extern volatile unsigned int settime_flag;


/*��ɫ�б�*/
enum {WHITE=0xFFFFFF, RED=0XFF0000,YELLOW=0XFFFF00,BLUE=0X312288,BLACK=0X0, GREEN=0X7CFC00, LIGHTBLUE=0x39ecf7};
/*����ģ���ʶ��*/
enum {MAIN, INF, LED, PWM, PIC, NAND, ADC};

/*����ģ����*/
extern int flag;
/*�����α�*/
extern int cur;
/*������������*/
const unsigned char *gImage_starting[104];
/*��������֡*/
volatile unsigned int gif_num;

/*��ʱ*/
void delay(volatile unsigned int i)
{	
	while(i--);	
}

/*������͸����ı��뺯��*/
void raise(void)
{
	
}

/*
void runing_point(unsigned int d)
{
	unsigned int x = 0, y = 240, r = 5;
	unsigned int save_x=0;
	unsigned int times = 40;
	unsigned int mid = 800/2-40-d;

	while(times--)
	{	
		lcd_draw_circle_block(x,  y,r,     WHITE);
		if(x == 0) x -= d;
		save_x = x;
		x += 20;
		delay(0x5000000);
		if(x == mid )
		{
			delay(0xFFF0000);	
		}
		lcd_draw_circle_block(save_x, y,r, LIGHTBLUE);
	}
}
*/
/*��ʱ��0���ú���*/
void timer0_irq(void)
{
	if(flag != PWM)
		work = timer0;
	
}	
/*��ʱ��0��������*/
void timer0_run(void)
{
	if(flag == MAIN)
		lcd_run_select_rect();
	work_stop();
	
}

/*��ʱ��1���ú���*/
void timer1_irq(void)
{
	work = timer1;
	
}
/*��ʱ��1��������*/
void timer1_run(void)
{
	switch(flag)
	{
		case LED: 
			run_led();
			break;
		case PWM:
			pwm_running();
			break;
		case ADC:
			adc_running();
			break;
		case INF:
			play();
			break;
		case PIC:
			play_pic();
			break;
	}
	work_stop();
}

/*��ʱ��2���ú���*/
void timer2_irq(void)
{
	work = timer2;
}
/*��ʱ��2��������*/
void timer2_run(void)
{
	if(flag == PWM)
		shengbo();
	work_stop();
}

/*��ʱ��3���ú���*/
void timer3_irq(void)
{
	if(flag == -1)
	{
		lcd_draw_pic(317, 131, 160, 120, gImage_starting[gif_num]);
		gif_num++;
	}
	work = timer3;
}

void timer3_run(void)
{
	if(flag == MAIN)
	{
		rtc_lcd_show( 600, 450,font_number_16, 8, WHITE, BLUE);
	}
	work_stop();
}
/*ֹͣ����״̬*/
void work_stop()
{
	work = nowork;
}

/*�ⲿ�ж�״̬*/
void key_int_set(int i)
{
	switch(i)
	{
		case 2:
			work = key2;
			break;
		case 3:
			work = key3;
			break;
		case 4:
			work = key4;
			break;
		case 5:
			work = key5;
			break;
		case 6:
			work = key6;
			break;
		case 7:
			work = key7;
			break;
		case 8:
			work = key8;
			break;
		case 9:
			work = key9;
			break;
	}
}

/*��ʾ����*/
void number_show(unsigned int x, unsigned int y, unsigned char *numbers ,unsigned int num, unsigned font_with,unsigned int color)
{
	unsigned int offset = num*font_with*font_with*2/8;
	lcd_show_font(x, y, numbers+offset, font_with, 0, color);
}

/*��ӭ����*/
void welcome()
{
	unsigned char *numbers = font_number_32;
	unsigned int offset = 3*16*32/8;
	int i;
	
	lcd_draw_fonts(330,300, font_welcome_64, 64, 1, WHITE, sizeof(font_welcome_64));
	lcd_draw_pic(300, 10, 200,264,gImage_tv_pic);
	
	for(i=3; i>0; i--)
	{
		lcd_show_font(393, 182, numbers+offset, 16, 0, BLACK);
		offset = i*16*32/8;
		lcd_show_font(393, 182, numbers+offset, 16, 0, WHITE);
		delay(0xFFFFFF0);
	}
	delay(0xFFFFFF0);
	starting();
}

/*��תҳ��*/
void turning()
{
	timer_start(2, 1, 0);
	//delay(0x50000);
	//timer_start(3, 1, 0);
	delay(0xFFF0000);
	timer_stop(2);	
	//timer_stop(3);	
}

/*�����ȴ�*/
void waiting()
{
	wait = 1;
	//in_or_set=1;
	flag = -1;
	/*rtc��ʼ��*/
	rtc_init();
	/*��������*/
	lcd_draw_pic(0, 0, 800, 480, gImage_start);

	while(wait)
	{
		working();
		//if(settime_flag == 0)
		//{	
			rtc_lcd_show(220, 200,font_number_32, 16, WHITE, LIGHTBLUE);
		//}
		delay(0x5000000);
		delay(0x5000000);
		/*if(flag < 0 && wait == 1)
		{
			if(settime_flag == 0)
			{	
				if(in_or_set  == 1)
				{
					lcd_draw_rect(255, 268, 100, 39, 3, WHITE);
					lcd_draw_rect(442, 268, 100, 39, 3, LIGHTBLUE);	
				}	
				else if(in_or_set  == 0)
				{
					lcd_draw_rect(442, 268,  100, 39, 3, WHITE);
					lcd_draw_rect(255, 268,  100, 39, 3, LIGHTBLUE);
				}
			}
		
		}*/
	
	};
}

/*���򵼺�*/
void fangxiang()
{
	if(flag == MAIN)
	{
		lcd_draw_pic(115 ,444, 15, 30, gImage_daohang_01);
		lcd_draw_pic(155 ,444, 15, 30, gImage_daohang_02);
		lcd_draw_pic(200 ,455, 30, 15, gImage_daohang_03);
		lcd_draw_pic(245 ,455, 30, 15, gImage_daohang_04);
	}
	else
	{
		lcd_draw_pic(200 ,455, 30, 15,  gImage_daohang_003);
		lcd_draw_pic(245 ,455, 30, 15,  gImage_daohang_004);
	}
}

/*��������*/
void working()
{
	switch(work)
	{
		case timer0:
			timer0_run();
			break;
		case timer1:
			timer1_run();
			break;
		case timer2: 
			timer2_run();
			break;
		case timer3:
			timer3_run();
			break;
	
		case key2:
			key_int_run(2);
			break;
		case key3: 
			key_int_run(3);
			break;
		case key4: 
			key_int_run(4);
			break;
		case key5: 
			key_int_run(5);
			break;
		case key6:
			key_int_run(6);
			break;
		case key7:
			key_int_run(7);
			break;
		case key8: 
			key_int_run(8);
			break;
		case key9: 
			key_int_run(9);
			break;
	}
}

/*�������*/
void comp_Test(void)
{
	printf("This is  Test\r\n");
	
	/*lcd��ʼ��*/
	lcd_init();
	/*��ʱ����ʼ��*/
	timer_init();
	/*led��ʼ��*/
	led_init();
	/*�ⲿ�жϳ�ʼ��*/
	key_init();
	
	/*�����ȴ�*/
	waiting();
	/*����*/
	lcd_clear_screen(LIGHTBLUE);
	/*��ӭ����*/
	welcome();
//	turning();
	
	/*������ʾ*/
	lcd_main();
	/*������ʱ��3*/
	timer_start(3, 1, 0);
	
	work = nowork;
	while(1)
	{
		working();
	};
}

/*����*/
void starting(void)
{
	gImage_starting[0]=gImage_001;
	gImage_starting[1]=gImage_002;
	gImage_starting[2]=gImage_003;
	gImage_starting[3]=gImage_004;
	gImage_starting[4]=gImage_005;
	gImage_starting[5]=gImage_006;
	gImage_starting[6]=gImage_007;
	gImage_starting[7]=gImage_008;
	gImage_starting[8]=gImage_009;
	gImage_starting[9]=gImage_010;
	gImage_starting[10]=gImage_011;
	gImage_starting[11]=gImage_012;
	gImage_starting[12]=gImage_013;
	gImage_starting[13]=gImage_014;
	gImage_starting[14]=gImage_015;
	gImage_starting[15]=gImage_016;
	gImage_starting[16]=gImage_017;
	gImage_starting[17]=gImage_018;
	gImage_starting[18]=gImage_019;
	gImage_starting[19]=gImage_020;
	gImage_starting[20]=gImage_021;
	gImage_starting[21]=gImage_022;
	gImage_starting[22]=gImage_023;
	gImage_starting[23]=gImage_024;
	gImage_starting[24]=gImage_025;
	gImage_starting[25]=gImage_026;
	gImage_starting[26]=gImage_027;
	gImage_starting[27]=gImage_028;
	gImage_starting[28]=gImage_029;
	gImage_starting[29]=gImage_030;
	gImage_starting[30]=gImage_031;
	gImage_starting[31]=gImage_032;
	gImage_starting[32]=gImage_033;
	gImage_starting[33]=gImage_034;
	gImage_starting[34]=gImage_035;
	gImage_starting[35]=gImage_036;
	gImage_starting[36]=gImage_037;
	gImage_starting[37]=gImage_038;
	gImage_starting[38]=gImage_039;
	gImage_starting[39]=gImage_040;
	gImage_starting[40]=gImage_041;
	gImage_starting[41]=gImage_042;
	gImage_starting[42]=gImage_043;
	gImage_starting[43]=gImage_044;
	gImage_starting[44]=gImage_045;
	gImage_starting[45]=gImage_046;
	gImage_starting[46]=gImage_047;
	gImage_starting[47]=gImage_048;
	gImage_starting[48]=gImage_049;
	gImage_starting[49]=gImage_050;
	gImage_starting[50]=gImage_051;
	gImage_starting[51]=gImage_052;
	gImage_starting[52]=gImage_053;
	gImage_starting[53]=gImage_054;
	gImage_starting[54]=gImage_055;
	gImage_starting[55]=gImage_056;
	gImage_starting[56]=gImage_057;
	gImage_starting[57]=gImage_058;
	gImage_starting[58]=gImage_059;
	gImage_starting[59]=gImage_060;
	gImage_starting[60]=gImage_061;
	gImage_starting[61]=gImage_062;
	gImage_starting[62]=gImage_063;
	gImage_starting[63]=gImage_064;
	gImage_starting[64]=gImage_065;
	gImage_starting[65]=gImage_066;
	gImage_starting[66]=gImage_067;
	gImage_starting[67]=gImage_068;
	gImage_starting[68]=gImage_069;
	gImage_starting[69]=gImage_070;
	gImage_starting[70]=gImage_071;
	gImage_starting[71]=gImage_072;
	gImage_starting[72]=gImage_073;
	gImage_starting[73]=gImage_074;
	gImage_starting[74]=gImage_075;
	gImage_starting[75]=gImage_076;
	gImage_starting[76]=gImage_077;
	gImage_starting[77]=gImage_078;
	gImage_starting[78]=gImage_079;
	gImage_starting[79]=gImage_080;
	gImage_starting[80]=gImage_081;
	gImage_starting[81]=gImage_082;
	gImage_starting[82]=gImage_083;
	gImage_starting[83]=gImage_084;
	gImage_starting[84]=gImage_085;
	gImage_starting[85]=gImage_086;
	gImage_starting[86]=gImage_087;
	gImage_starting[87]=gImage_088;
	gImage_starting[88]=gImage_089;
	gImage_starting[89]=gImage_090;
	gImage_starting[90]=gImage_091;
	gImage_starting[91]=gImage_092;
	gImage_starting[92]=gImage_093;
	gImage_starting[93]=gImage_094;
	gImage_starting[94]=gImage_095;
	gImage_starting[95]=gImage_096;
	gImage_starting[96]=gImage_097;
	gImage_starting[97]=gImage_098;
	gImage_starting[98]=gImage_099;
	gImage_starting[99]=gImage_100;
	gImage_starting[100]=gImage_101;
	gImage_starting[101]=gImage_102;
	gImage_starting[102]=gImage_103;
	gImage_starting[103]=gImage_104;
	
	gif_num=0;
	delay(0x5000000);
	flag = -1;
	/*��ʱ��3���ƶ���*/
	timer_start(3, 13, 0);
	
	/*��������*/
	audio_test(0x44000000, 610044);
	while(1)
	{
		if(gif_num == 56)
		{
			audio_test(0x45000000, 420044);
		}
		if(gif_num == 104) 
		{
			timer_stop(3);
			break;
		}
		
		
	}
	delay(0x5000000);
	delay(0x5000000);
	delay(0x5000000);
	flag = MAIN;
	
	timer_start(3, 1, 0);
	
}


