#include "stdio.h"		//printf scanf ....
#include "s5pv210.h"	//寄存器
#include "font.h"	    //字库
#include "nand_pic2.h"	 //图片
#include "all_headers.h"

#define MP0_1CON 	( *((volatile unsigned long *)0xE02002E0) )
#define MP0_2CON 	( *((volatile unsigned long *)0xE0200300) )
#define MP0_3CON 	( *((volatile unsigned long *)0xE0200320) )
#define MP0_6CON 	( *((volatile unsigned long *)0xE0200380) )
#define TACLS    					  2				
#define TWRPH0   					  4
#define TWRPH1   					  2


#define NFCONF 		( *((volatile unsigned long *)0xB0E00000) )
#define NFCONT 		( *((volatile unsigned long *)0xB0E00004) )
#define NFCMMD 		( *((volatile unsigned char *)0xB0E00008) )
#define NFADDR 		( *((volatile unsigned char *)0xB0E0000C) )
#define NFDATA 		( *((volatile unsigned char *)0xB0E00010) )
#define NFSTAT 		( *((volatile unsigned long *)0xB0E00028) )

#define MAX_NAND_BLOCK    4096 //blocks in chip			
#define NAND_PAGE_SIZE    2048 //size of page			
#define NAND_BLOCK_NUM    64   //pages in a block
#define NAND_BLOCK_SIZE   (NAND_PAGE_SIZE*NAND_BLOCK_NUM) //size of block			

#define NAND_CMD_READ_1st             0x00 
#define NAND_CMD_READ_2st             0x30
#define NAND_CMD_RANDOM_WRITE         0x85
#define NAND_CMD_RANDOM_READ_1st      0x05
#define NAND_CMD_RANDOM_READ_2st      0xe0
#define NAND_CMD_READ_CB_1st          0x00
#define NAND_CMD_READ_CB_2st          0x35
#define NAND_CMD_READ_ID              0x90
#define NAND_CMD_RES                  0xff //reset command
#define NAND_CMD_WRITE_PAGE_1st       0x80
#define NAND_CMD_WRITE_PAGE_2st       0x10
#define NAND_CMD_BLOCK_ERASE_1st      0x60
#define NAND_CMD_BLOCK_ERASE_2st      0xd0
#define NAND_CMD_READ_STATUS          0x70

#define ECC_EN						 (1<<4)
#define CHIP_SELECTED   			(~(1<<1))
#define CHIP_DESELECTED 			(1<<1)
#define CONTROL_EN					(1<<0)

#define W 1
#define R 0

#define NAND_ADDR					460*1024*1024
/*颜色列表*/
enum {WHITE=0xFFFFFF, RED=0XFF0000,YELLOW=0XFFFF00,BLUE=0X0000CD,BLACK=0X0, GREEN=0X7CFC00, LIGHTBLUE=0x39ecf7};
/*本页背景颜色*/
const unsigned int back_color_nand = 0x3d60f9;
/*数据buff*/
const unsigned char data[]={'A','B','C','D','E','F','G','H','I','J','K','L',
							'M','N','O','P','Q','R','S','T','U','V','W','X',
							'Y','Z'};
volatile unsigned int k = 10;////


/*延时*/
static void nand_delay(void)
{
	   volatile unsigned int i=0x200;
		while(i--);
}

void success()
{
	lcd_draw_fonts(300, 350, font_successfully_16, 8, 0, BLACK, sizeof(font_successfully_16));
	nand_delay();
	nand_delay();
	nand_delay();
	nand_delay();
	nand_delay();
	nand_delay();
	lcd_draw_fonts(300, 350, font_successfully_16, 8, 0, back_color_nand, sizeof(font_successfully_16));
}

/*nand flash 初始化*/
void nand_init(void)
{
		/* 1.配置IO 
			 MP0_1CON[11:8]=NFCSn[0]
			 MP0_3CON[3:0]=NF_CLE
			 MP0_3CON[7:4]=NF_ALE
			 MP0_3CON[11:8]=NF_FWEn	
			 MP0_3CON[15:12]=NF_FREn		
			 MP0_3CON[19:16]=NF_RnB[0]	
			 MP0_6CON[31:0]=EBI_DATA[0]~EBI_DATA[7]
		*/
		MP0_1CON |= (0x3<<8);
		MP0_3CON =  (0x2<<0) 
				  | (0x2<<4)
				  | (0x2<<8)
				  |(0x2<<12)
				  |(0x2<<16);
		MP0_6CON = 0x22222222;
		
		
		/* 2.配置NFCONF
			[2]:0 2048Bytes/Page ，这个要查看K9F4G08.pdf
			[3]:0 SLC Nand Flash
			[7:4]:1 7.5ns *(1+1)>10ns TWRPH1
			[11:8]: 7.5ns *(3+1)>(25ns+10ns-10ns) TWRPH0
			[12]:   7.5ns *(1+1)>12ns TACLS
			
		*/
		NFCONF |=  (1<<1);
		NFCONF &= ~(1<<2);
		NFCONF &= ~(1<<3);
		
		NFCONF |=  (1<<4);
		NFCONF |=  (3<<8);
		NFCONF |=  (2<<12);
		
					
		/* 3.配置NFCONT
			[0]:1 使能Nand Flash 控制器 
			[1]:1 这里是片选Nand Flash 
			[8]:0 这里设置R/B引脚跳变的状态，上升沿有效，也代表说已经完成操作或准备好	
		*/
		NFCONT |=  (1<<0);
		NFCONT |=  (1<<1);
		NFCONT &= ~(1<<8);
			;
	
}

/*读数据*/
static void nand_read(unsigned int addr, unsigned char buf[], unsigned int len)
{
	unsigned int i = 0;
	unsigned int col = addr%2048;
	unsigned int row = addr/2048;
	
	
	/* 1.片选Nand 
	
		 NFCONT[1]=0
	*/
	NFCONT &= ~(1<<1);
	
	/* 2.发送命令0x00，NFCMMD */
	NFCMMD = 0x00;
	
	 /*3. 发送5个周期地址
	 */		
	NFADDR = col & 0xFF;
	NFADDR = (col>>8) & 0x0F;
	
	NFADDR = row & 0xFF;
	NFADDR = (row>>8) & 0xFF;
	NFADDR = (row>>16) & 0x03;
	
	
	 /*4. 发送读取数据命令
	      NFCMMD = 0x30
	 */	
	NFCMMD = 0x30;
	
	
	/*5. 判断当前是否已经读取数据完成
	 */		
	
	while((NFSTAT &(1<<0))==0);
	
	/* 6.连续读取5个字节数据通过串口打印出来，NFDATA */	
	printf("nand read data:\r\n");
	while(i < len)
	{
		buf[i] = NFDATA;
		transfering(i, R);
		i++;
	}
	
	printf("\r\n");
		
	/* 7.不选择Nand 
		 NFCONT[1]=1
	*/	
	NFCONT |= (1<<1);
	success();
	lcd_draw_fonts(20, 210, font_nand_data_16, 8, 0, WHITE, sizeof(font_nand_data_16));
}
//////////////////////////////////////////////////

/*写数据*/
static void nand_write(unsigned int addr, unsigned char buf[], unsigned int len)
{
	unsigned int i = 0;
	unsigned int col = addr%2048;
	unsigned int row = addr/2048;
	
	printf("nand write data......:\r\n");
	/* 1.片选Nand 
	
		 NFCONT[1]=0
	*/
	NFCONT &= ~(1<<1);
	
	printf("nand write data......:\r\n");
	/* 加一段短暂的延时 */
	delay();
	
	printf("nand write data......:\r\n");
	/* 2.发送命令0x80，NFCMMD */
	NFCMMD = 0x80;
	
	/* 加一段短暂的延时 */
	delay();
	
	printf("nand write data......:\r\n");
	 /*3. 发送5个周期地址
	 */		
	NFADDR = col & 0xFF;
	NFADDR = (col>>8) & 0x0F;
	
	NFADDR = row & 0xFF;
	NFADDR = (row>>8) & 0xFF;
	NFADDR = (row>>16) & 0x03;
	
	/* 加一段短暂的延时 */
	//delay();
	printf("nand write data......:\r\n");
	/* 6.连续写取64个字节数据通过串口打印出来，NFDATA */	
	printf("nand write data:\r\n");
	while(i < len)
	{
		printf("-----writing\r\n");
		NFDATA = buf[i] ;
		transfering(i, W);
		i++;
	}
	
	/*4. 发送写数据命令
	      NFCMMD = 0x10
   */	
	NFCMMD = 0x10;
	
	/*5. 判断当前是否已经写数据完成
	 */		
	
	while((NFSTAT &(1<<0))==0);
	
	NFCMMD = 0x70; 	


	if(NFDATA & 0x01)
		printf("write fail\r\n");
	else
	{
		printf("write success\r\n");
		success();
	}
		

	/* 7.不选择Nand 
		 NFCONT[1]=1
	*/	
	NFCONT |= (1<<1);
	
	lcd_draw_fonts(300, 260, font_nand_data_16, 8, 0, WHITE, sizeof(font_nand_data_16));
	lcd_draw_fonts(20, 210, font_nand_data_16, 8, 0, back_color_nand, sizeof(font_nand_data_16));
	
}

////////////////////////////////////////////////////////////////

/*块擦除*/
static void nand_erase(unsigned int addr)
{
	unsigned int row = addr/2048;
	unsigned int i = 0;
	/* 1.片选Nand 
	
		 NFCONT[1]=0
	*/
	NFCONT &= ~(1<<1);
	
	/* 2.发送命令0x60，NFCMMD */
	NFCMMD = 0x60;
	
	 /*3. 发送5个周期地址
	 */		
	NFADDR = row & 0xFF;
	NFADDR = (row>>8) & 0xFF;
	NFADDR = (row>>16) & 0x03;
	
	while(i < 26)
	{
		transfering(i, R);
		i++;
	}
	
	 /*4. 发送写数据命令
	 */	
	NFCMMD = 0xD0;
	/*5. 判断当前是否已经读取数据完成
	 */		
	while((NFSTAT &(1<<0))==0);	
	
	NFCMMD = 0x70; 	
	if(NFDATA & 0x01)
		printf("erase fail\r\n");
	else
	{
		printf("erase success\r\n");
		success();
	}
		
	/* 7.不选择Nand 
		 NFCONT[1]=1
	*/	
	NFCONT |= (1<<1);
}

/*数据传送动画*/
void transfering(unsigned int i , int state)
{
	int x = 0, oldx = 0;
	unsigned int offset = i*16+3*16;
	unsigned char *nand_data = font_nand_data_16;
	unsigned int start = 0, end = 580;
	
	if(state == R)
	{
		start = 600;
		end = 10;
	}
	
	x = start;
	
	while(1)
	{	
		lcd_draw_color_rect(   x, 233, 10, 17, YELLOW);
		lcd_draw_color_rect(oldx, 233, 10, 17, back_color_nand);
		oldx = x;
		nand_delay();
		nand_delay();
		if(state == W )
		{
			if(i == 10) 
			{
				k++;
			}
			if(i > 9)
			{
				i -= 9;
			}
			lcd_show_font( k*20 , i*20, &font_nand_data_16[offset], 8, 0, WHITE);
			x += 10;
			if(x > end)	break;
		}	
		else
		{
			x -= 10;
			if(x < end)  break;
		}
			
	}
}

/*写数据*/
void write_to_nand()
{
	nand_write(NAND_ADDR, data, 26);
	k++;
}
/*读数据*/
void read_from_nand()
{
	nand_read(NAND_ADDR, data, 26);
}
/*块擦除*/
void erase_nand()
{
	nand_erase(NAND_ADDR);
	lcd_draw_fonts(300, 260, font_nand_data_16, 8, 0, back_color_nand, sizeof(font_nand_data_16));
	nand_test();
}


/*test 入口函数*/
void nand_test()
{
	unsigned char buf[32] = {0};
	
	lcd_baiyue_screen(back_color_nand);
	lcd_draw_line_xy(0, 230, 600, 230, 3, WHITE);
	lcd_draw_line_xy(0, 250, 600, 250, 3, WHITE);
	lcd_draw_pic(600 ,200, 120, 80, gImage_nand_pic2);
	
	lcd_draw_fonts(20, 210, font_nand_data_16, 8, 0, WHITE, sizeof(font_nand_data_16));
	
	nand_init();
	k = 1;
	
}