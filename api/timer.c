#include "s5pv210.h"
#include "stdio.h"
#include "all_headers.h"
#define up 1
#define down 2
#define keep 0

/*定时器0中断函数*/
static void timer0_irq_handler(void)
{
	//printf("timer0 irq happend\r\n");		
	timer0_irq();
	
	//rGPJ2DAT ^= (0x01<<0);
	 /*1.清空定时器标志位*/
	rTINT_CSTAT|=1<<5;
	 
	 /*2.告诉CPU我已经完成中断*/
	rVIC0ADDRESS &= ~(1<<21);
}
/*定时器1中断函数*/
static void timer1_irq_handler(void)
{
	//printf("timer1 irq happend\r\n");
	 
	timer1_irq();
	 
	 /*1.清空定时器标志位*/
	rTINT_CSTAT|=1<<6;
	 
	 /*2.告诉CPU我已经完成中断*/
	rVIC0ADDRESS &= ~(1<<22);
}

/*定时器2中断函数*/
static void timer2_irq_handler(void)
{
	//printf("timer2 irq happend\r\n");
	 
	timer2_irq();
	 
	 /*1.清空定时器标志位*/
	rTINT_CSTAT|=1<<7;
	 
	 /*2.告诉CPU我已经完成中断*/
	 rVIC0ADDRESS &= ~(1<<23);


}
/*定时器3中断函数*/
static void timer3_irq_handler(void)
{
	 //printf("timer3 irq happend\r\n");
	 
	timer3_irq();
	 
	 /*1.清空定时器标志位*/
	 rTINT_CSTAT|=1<<8;
	 
	 /*2.告诉CPU我已经完成中断*/
	 rVIC0ADDRESS &= ~(1<<24);
}

/*定时器初始化*/
void timer_init(void)
{
	/*1.设置定时器输入频率=PCLK/(预分频值+1)/分频值 
	                      =66000000/(65+1)/16
						  =62500Hz
						  TCNTB0进行减1计数为62500HZ的时候就代表是1秒的时间
	*/
	rTCFG0 = (65<<0) | (65<<8);
	rTCFG1 =  (4 <<0) 
			| (4 <<4) 
			| (4 <<8)
			| (4 <<12);
	
	/*2.设置定时器计数值 
	rTCNTB0=62500;//1S/5
	rTCNTB1=62500;//1S/2
	rTCNTB2=62500;//1S/1
	rTCNTB3=62500;//1S*2
	
	//3.设置定时器比较值为0，因为我当前不需要进行PWM功能，即不需要产生电平变化
	rTCMPB0=0
	rTCMPB1=0;
	rTCMPB2=0;
	rTCMPB3=0;
	*/
	/*4.安装定时器0中断服务函数*/
	rVIC0VECTADDR21 = (unsigned int)timer0_irq_handler;
	rVIC0VECTADDR22 = (unsigned int)timer1_irq_handler;
	rVIC0VECTADDR23 = (unsigned int)timer2_irq_handler;
	rVIC0VECTADDR24 = (unsigned int)timer3_irq_handler;
	
	/*5.使能当前VIC0第21通道为IRQ中断*/
	//rVIC0INTSELECT &=~(0xF<<21);
	
	/*6.允许当前VIC0第21通道中断*/
	//rVIC0INTENABLE |= (0xF<<21);
	
	/*7.当前没有有效的中断服务函数，也代表说清零*/
	rVIC0ADDRESS = 0;
	
	/*8.启动定时器
	    1)先手动更新TCNTB0与TCMPB0的值
		2)启动定时器并自动重载
	*/
	//rTCON|=  1<<1;
		//	|1<<9
		///	|1<<13
		//	|1<<17;
			
	//rTCON =  (1<<3)|(1<<0);
		//	|(1<<11)|(1<<8)
		//	|(1<<15)|(1<<12)
		//	|(1<<19)|(1<<16);
	
	/*9.使能定时器中断*/
	rTINT_CSTAT =(0xF<<5)|(0xF<<0);

}
/*定时器使能*/
void timer_start(unsigned int num,unsigned int tcn, unsigned int tcm)
{
	switch(num)
	{
		case 0: 
			rTCNTB0=62500/tcn;//1S/5
			if(tcm == 0)
				rTCMPB0=tcm;
			else
				rTCMPB0=rTCNTB0/tcm;
	
			rVIC0INTSELECT &=~(0x1<<21);
			rVIC0INTENABLE |= (0x1<<21);
			
			rTCON |=  1<<1;
			rTCON &= ~(1<<1);
			rTCON |=  (1<<3)|(1<<0);
			break;
		case 1: 
			rTCNTB1=62500/tcn;//1S/5
			if(tcm == 0)
				rTCMPB1=tcm;
			else
				rTCMPB1=rTCNTB1/tcm;
			
			rVIC0INTSELECT &=~(0x1<<22);
			rVIC0INTENABLE |= (0x1<<22);
			
			rTCON |=  1<<9;
			rTCON &= ~(1<<9);
			rTCON |=  (1<<11)|(1<<8);
			break;
		case 2: 
			rTCNTB2=62500/tcn;//1S/5
			if(tcm == 0)
				rTCMPB2=tcm;
			else
				rTCMPB2=rTCNTB2/tcm;
			
			rVIC0INTSELECT &=~(0x1<<23);
			rVIC0INTENABLE |= (0x1<<23);
			
			rTCON |=  1<<13;
			rTCON &= ~(1<<13);
			rTCON |=  (1<<15)|(1<<12);
			break;
		case 3:
			rTCNTB3=62500/tcn;//1S/5
			if(tcm == 0)
				rTCMPB3=tcm;
			else
				rTCMPB3=rTCNTB3/tcm;
			
			rVIC0INTSELECT &=~(0x1<<24);
			rVIC0INTENABLE |= (0x1<<24);
			
			rTCON |=  1<<17;
			rTCON &= ~(1<<17);
			rTCON |=  (1<<19)|(1<<16);
	}
}

/*定时器停止*/
void timer_stop(unsigned int num)
{
	work_stop();
	switch(num)
	{
		case 0: 
			rTCON &= ~(1<<0);
			break;
		case 1: 
			rTCON &= ~(1<<8);
			break;
		case 2: 
			rTCON &= ~(1<<12);
			break;
		case 3:
			rTCON &= ~(1<<16);
	}
}


/*定时器参数更新*/
void timer_update(unsigned int num, unsigned int tcn, unsigned int tcm, unsigned int state)
{
	switch(num)
	{
		case 0: 
			rTCNTB0=62500/tcn;//1S/5
			switch(state)
			{
				case keep:
					rTCMPB0  =rTCNTB0/tcm;
					break;
				case up:
					rTCMPB0 +=rTCNTB0/tcm;
					break;
				case down:
					rTCMPB0 -=rTCNTB0/tcm;
			}			
			
			rTCON |=  1<<1;
			rTCON &= ~(1<<1);
			rTCON |=  (1<<3)|(1<<0);
			break;
		case 1: 
			rTCNTB1=62500/tcn;//1S/5
			switch(state)
			{
				case keep:
					rTCMPB1  =rTCNTB1/tcm;
					break;
				case up:
					rTCMPB1 +=rTCNTB1/tcm;
					break;
				case down:
					rTCMPB1 -=rTCNTB1/tcm;
			}	
		
			rTCON|=  1<<9;
			rTCON =  (1<<11)|(1<<8);
			break;
		case 2: 
			rTCNTB2=62500/tcn;//1S/5
			switch(state)
			{
				case keep:
					rTCMPB2  =rTCNTB2/tcm;
					break;
				case up:
					rTCMPB2 +=rTCNTB2/tcm;
					break;
				case down:
					rTCMPB2 -=rTCNTB2/tcm;
			}	
			
			rTCON|=  1<<13;
			rTCON =  (1<<15)|(1<<12);
			break;
		case 3:
		
			rTCNTB3=62500/tcn;//1S/5
			switch(state)
			{
				case keep:
					rTCMPB3  =rTCNTB3/tcm;
					break;
				case up:
					rTCMPB3 +=rTCNTB3/tcm;
					break;
				case down:
					rTCMPB3 -=rTCNTB3/tcm;
			}	
			rTCON|=  1<<17;
			rTCON =  (1<<19)|(1<<16);
	}
}





