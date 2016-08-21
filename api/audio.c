

#define GPICON  	(*(volatile unsigned int *)0xE0200220)	//IIS Signals


#define IISCON  	(*(volatile unsigned int *)0xEEE30000)	//IIS Control
#define IISMOD  	(*(volatile unsigned int *)0xEEE30004)	//IIS Mode
#define IISFIC  	(*(volatile unsigned int *)0xEEE30008)	//IIS FIFO Control
#define IISPSR  	(*(volatile unsigned int *)0xEEE3000C)	//IIS Prescaler
#define IISTXD		(*(volatile unsigned int *)0xEEE30010)	//IIS TXD DATA
#define IISRXD 		(*(volatile unsigned int *)0xEEE30014)	//IIS RXD DATA
#define IISFICS  	(*(volatile unsigned int *)0xEEE30018)	//IIS FIFO Control

#define EPLL_CON0  	(*(volatile unsigned int *)0xe0100110)
#define EPLL_CON1  	(*(volatile unsigned int *)0xe0100114)

#define CLK_SRC0  	(*(volatile unsigned int *)0xE0100200)
#define CLK_CON  	(*(volatile unsigned int *)0xEEE10000)


#define GPD1CON    	(*(volatile unsigned *)0xE02000C0) 
#define GPD1DAT    	(*(volatile unsigned *)0xE02000C4) 
#define GPD1PUD    	(*(volatile unsigned *)0xE02000C8) 


#define I2CCON0  	(*(volatile unsigned *)0xE1800000) 
#define I2CSTAT0 	(*(volatile unsigned *)0xE1800004) 
#define I2CADD0  	(*(volatile unsigned *)0xE1800008) 
#define I2CDS0   	(*(volatile unsigned *)0xE180000C)
#define I2CLC0   	(*(volatile unsigned *)0xE1800010)


#define IICCON		I2CCON0
#define IICSTAT		I2CSTAT0
#define IICADD		I2CADD0
#define IICDS		I2CDS0

#define WM8960_DEVICE_ADDR		0x34

#define IISTXD		(*(volatile unsigned int *)0xEEE30010)
#define IISCON  	(*(volatile unsigned int *)0xEEE30000)

static void i2s_init(void)
{
    int N;
	
    /* 配置引脚用于i2s功能 */
    GPICON = 0x22222222;

    /*  设置i2s相关时钟，配置EPLL输出频率为67.7MHz(see p361 of s5pv210.pdf) 
		FOUT = (MDIV+K/65536) X FIN / (PDIV X 2SDIV)
		Fout = (0x43+0.7)*24M / (3*2^3) = 80*24M/24 = 67.7Mhz
	*/

    EPLL_CON0 = 0xA8430303; 	// MPLL_FOUT = 67.7Mhz
    EPLL_CON1 = 0xbcee; 		// from linux kernel setting

    /* 设置 MUX_EPLL = 1 -> SCLK_EPLL = 67.7Mhz (see p361 of s5pv210.pdf)
       EPLL_SEL  [8]  Control MUXEPLL (0:FINPLL, 1:FOUTEPLL)
	*/

    CLK_SRC0 = 0x10001111;

    /* 设置Mux_I2S	AUDIO subsystem clock selection (see P1868 P1875 of s5pv210.pdf) */

    CLK_CON = 0x1;		// 1 = FOUT_EPLL MUXI2S_A 00 = Main CLK


    /* 设置i2s控制器
		Divider of IIS (67.7 -> 11.289Mhz)
		N + 1 = (67.7Mhz) / (256 * 44.1Khz) = 5.99
		IISCDCLK  11.289Mhz = 44.1K * 256fs
		IISSCLK	  1.4112Mhz = 44.1K * 32fs
		IISLRCLK   44.1Khz
	*/
    N = 5;
    IISPSR = 1<<15 | N<<8;

    /* IIS interface active (start operation) 
	   [0]:1 	使能I2S接口
	   [31]:1 	不复位I2S硬件模块
	
	*/
    IISCON |= (1<<0) | (1<<31);

    /* [9:8]:10  发送与接收数据能够同时进行(播放音乐与录音)
	   [10] :1   使用I2SCLK，时钟源来自EPLL
	*/
    IISMOD = (1<<9) | (0<<8) | (1<<10);
}




static void i2c_write(int slave_addr, int addr, int data)
{
	I2CDS0 = slave_addr;

	/* 	bit[7:6]: 主机发送器
		bit[5]:发出启动信号和I2CDS0里的从机地址
		bit[4]:使能tx/rx
	*/
	I2CSTAT0 = 0xf0;

	/* 等待数据发送 */
	while ((I2CCON0 & 0x10) == 0);			

	/* 等待从机发来ACK */
	while ((I2CSTAT0 & 0x1));						


	/* 发7bit地址和9bit数据 */
	I2CDS0 = (addr<<1) | ((data>>8) & 0x0001);
	
	/* 清中断 */
	I2CCON0 &= ~(1<<4);	
	
	/* 等待数据发送 */
	while ((I2CCON0 & 0x10) == 0);
	
	/* 等待从机发来ACK */
	while ((I2CSTAT0 & 0x1));	
	
	I2CDS0 = (data & 0x00FF);
	
	/* 清中断 */
	I2CCON0 &= ~(1<<4);		
	
	/* 等待数据发送 */
	while ((I2CCON0 & 0x10) == 0);	
	
	/* 等待从机发来ACK */
	while ((I2CSTAT0 & 0x1));						
	
	/* 	bit[7:6]: 主机发送器
		bit[5]: 发出停止信号
		bit[4]: 使能tx/rx
	*/
	
	I2CSTAT0 = 0xd0;

	/* 清中断 */
	I2CCON0 &= ~(1<<4);								
	
	// 延时等待
	volatile int i=0;
	for(i=0; i<50; i++);
}

static void i2c_init(void)
{
	/* 设置GPD1_0引脚为IICSDA功能，GPD1_1引脚为IICSCL功能 */
    GPD1CON &= ~(0xff);
    GPD1CON |= (0x22); 
	
	/* 取消上下拉电阻，因为硬件外部已经连接了上拉电阻 */
    GPD1PUD &= ~(0xf); 

    I2CCON0  =(1<<7) 	//使能应答位，在读数据的时候提供应答
			 |(1<<6) 	//IICPCLK=PCLK/16=66MHz/16=4.125MHz
			 |(1<<5) 	//使能中断标志位
			 |(0xf);	//由于IICPCLK时钟频率4.125MHz过高,再次二次分频，得出最后的频率为约等于260KHz.

    /* bit[4] = 1: 使能接收和发送功能 */
    I2CSTAT0 = 0x10;

}

static void wm8960_init(void)
{

    /* 重置所有寄存器
	   [8:0]:0	复位所有寄存器
	*/
    i2c_write(WM8960_DEVICE_ADDR, 0xf, 0x0);

    /* 设置电源管理(1)
	   [6]:		1 	接入参考电压VREF
	   [8:7]	3	左、右声道各自接入5KΩ电阻，用于快速启动
	*/
	i2c_write(WM8960_DEVICE_ADDR, 0x19, 1<<8 | 1<<7 | 1<<6);
	
	/*设置电源管理(2)
	  [3]:1 使能SPKR音量控制
	  [4]:1 使能SPKL音量控制	
	  [5]:1 使能LOUT1输出
	  [6]:1 使能ROUT1输出 	
	  [7]:1 右声道DAC使能
	  [8]:1 左声道DAC使能	  
	 */
	i2c_write(WM8960_DEVICE_ADDR, 0x1A, 1<<8 | 1<<7 | 1<<6 | 1<<5 | 1<<4 | 1<<3);
	
	/*设置电源管理(3)
	  [2]:1	使能右声道混频器输出
	  [3]:1 使能左声道混频器输出
	 */	
	i2c_write(WM8960_DEVICE_ADDR, 0x2F, 1<<3 | 1<<2);


    /* 设置时钟 
	   [0]:		0	SYSCLK为MCLK(11.289MHz)
	   [2:1]:	0	SYSCLK不再次分频
	   [5:3]:	0	DAC默认采样频率为11.289MHz/256=44.1KHz
	   [8:6]:	0	ADC默认采样频率为11.289MHz/256=44.1KHz	
	*/
    i2c_write(WM8960_DEVICE_ADDR, 0x4, 0x0);

    /* 设置ADC-DAC 
	   [6:5]	0	ADC采样极性不取反
	   [7]		0	DAC输出不进行衰减
	
	*/
    i2c_write(WM8960_DEVICE_ADDR, 0x5, 0x0);

    /* 设置audio interface 
	   [1:0] 2	数据格式左对齐
	   [3:2] 0	数据宽度为16bit
	   [4]   0	LRCLK时钟极性正常输出
	   [5]   0	正常输出左右声道数据
	   [6]   0	设置为从机模式
	   [7]   0	BCLK时钟极性正常输出	 
	   [8]   0	正常输出左右声道数据	   
	*/
    i2c_write(WM8960_DEVICE_ADDR, 0x7, 0x2);

    /* 设置LOUT1音量
		[6:0]	7F	输出音量为最大值
		[7]		1	使能过零检测，用于检测增益的变化
		[8]		1	更新当前LOUT1音量
	*/
    i2c_write(WM8960_DEVICE_ADDR, 0x2, 0xFF | 0x100);
	
    /* 设置ROUT1音量
		[6:0]	7F	输出音量为最大值
		[7]		1	使能过零检测，用于检测增益的变化
		[8]		1	更新当前ROUT1音量
	*/	
    i2c_write(WM8960_DEVICE_ADDR, 0x3, 0xFF | 0x100);

    /* 设置LDAC音量
		[7:0]	0	0db输出，无衰减
		[8]		1	更新当前LDAC音量	
	*/
    i2c_write(WM8960_DEVICE_ADDR, 0xa, 0xFF | 0x100);
	
    /* 设置RDAC音量
		[7:0]	0	0db输出，无衰减
		[8]		1	更新当前RDAC音量	
	*/	
    i2c_write(WM8960_DEVICE_ADDR, 0xb, 0xFF | 0x100);

    /* 设置LOUT Mixer 
		[8]		1	LOUT DAC使能混频器输出
		[7]		1	LINPUT3 使能混频器输出		
	*/
    i2c_write(WM8960_DEVICE_ADDR, 0x22, 1<<8 | 1<<7);
	
    /* 设置ROUT Mixer 
		[8]		1	ROUT DAC使能混频器输出
		[7]		1	RINPUT3 使能混频器输出		
	*/	
    i2c_write(WM8960_DEVICE_ADDR, 0x25, 1<<8 | 1<<7);
}




void audio_test(unsigned int addr, unsigned int size)
{
	printf("this is audio Test\r\n");
	
	/* 音频数据开始的地方 */
	int offset = 0x2E;				

	/* 音频文件应该位于的位置 */
	short * p = (short *)addr;		
	
	/* 初始化i2c */
	i2c_init();								

	/* 初始化wm8960 */
	wm8960_init();							
	
	/* 初始化i2s */
	i2s_init();								
	
	/* 循环播放音频文件 */
	while (1)
	{
		/* 等待数据发送成功 */
		while((IISCON & (1<<8)) == (1<<8));
		
		/* 每次发送2byte */
		IISTXD = *(p+offset);				
		
		offset++;
		
		/* 有多少个2byte = (文件大小-偏移)/2 */
		if (offset > (size-0x2e) /2)		
			//offset = 0x2E;
			break;
	
	}
}