#include "stdio.h"
#include "s5pv210.h"
#include "rtc.h"
#include "font.h"	    //字库
#include "all_headers.h"



#define 		RTC_BASE	   (0xE2800000)
#define		INTP      	 ( *((volatile unsigned long *)(RTC_BASE + 0x30)) )
#define		RTCCON    	 ( *((volatile unsigned long *)(RTC_BASE + 0x40)) )
#define		TICCNT    	 ( *((volatile unsigned long *)(RTC_BASE + 0x44)) )
#define		RTCALM    	 ( *((volatile unsigned long *)(RTC_BASE + 0x50)) )
#define		ALMSEC    	 ( *((volatile unsigned long *)(RTC_BASE + 0x54)) )
#define		ALMMIN    	 ( *((volatile unsigned long *)(RTC_BASE + 0x58)) )
#define		ALMHOUR  	 	 ( *((volatile unsigned long *)(RTC_BASE + 0x5c)) )
#define		ALMDAY    	 ( *((volatile unsigned long *)(RTC_BASE + 0x60)) )
#define		ALMMON    	 ( *((volatile unsigned long *)(RTC_BASE + 0x64)) )
#define		ALMYEAR  	   ( *((volatile unsigned long *)(RTC_BASE + 0x68)) )
#define		RTCRST     	 ( *((volatile unsigned long *)(RTC_BASE + 0x6c)) )
#define		BCDSEC    	 ( *((volatile unsigned long *)(RTC_BASE + 0x70)) )
#define		BCDMIN   	   ( *((volatile unsigned long *)(RTC_BASE + 0x74)) )
#define		BCDHOUR    	 ( *((volatile unsigned long *)(RTC_BASE + 0x78)) )
#define		BCDDAY    	 ( *((volatile unsigned long *)(RTC_BASE + 0x7c)) )
#define		BCDWEEK     ( *((volatile unsigned long *)(RTC_BASE + 0x80)) )
#define		BCDMON     	 ( *((volatile unsigned long *)(RTC_BASE + 0x84)) )
#define		BCDYEAR      ( *((volatile unsigned long *)(RTC_BASE + 0x88)) )
#define		CURTICCNT    ( *((volatile unsigned long *)(RTC_BASE + 0x90)) )
#define		RTCLVD    	 ( *((volatile unsigned long *)(RTC_BASE + 0x94)) )

#define    rVIC0VECTADDR28 ( *((volatile unsigned long *)(0xF2000170 )) )

#define   TRUE  1
#define   FALSE 0



char *days[7] = {	"Sunday",
					"Monday",
					"Tuesday",
					"Wednesday",
					"Thursday",
					"Friday",
					"Saturday"};
	/* 初始值为重置值 */
volatile unsigned int year = 15;
volatile unsigned int month = 8;
volatile unsigned int day = 24;
volatile unsigned int hour = 14;
volatile unsigned int min = 50;
volatile unsigned int sec = 0;	
volatile unsigned int weekday= 1;

volatile unsigned int settime_flag;
				

void time_enable(unsigned char bdata)
{
	
	/* 使能rtc控制器 */
	rtc_enable(bdata);
	
	/* 使能rtc tick timer */
	rtc_ticktime_enable(bdata);
}

// 使能/关闭rtc控制器
void rtc_enable(unsigned char bdata)
{
	unsigned int uread;

	uread = RTCCON;
	RTCCON = (uread&~(1<<0))|(bdata);
}

// 使能/关闭Tick timer
void rtc_ticktime_enable(unsigned char bdata)
{
	unsigned int uread;
	uread = RTCCON;
	RTCCON = ( (uread&~(1<<8)) | (bdata<<8) );
}



/*初始化*/
void rtc_init()
{
	settime_flag = 0;
	
	year = 15;
	month = 8;
	day = 26;
	hour = 14;
	min = 10;
	sec = 0;	
	weekday= 1;
	
	rtc_settime();
	time_enable(TRUE);
	
}

void rtc_lcd_show(unsigned int x, unsigned int y, unsigned char *numbers , unsigned font_with, unsigned int color, unsigned back_color)
{
	unsigned int uyear,umonth,uday,uweek,uhour,umin,usec;
	static unsigned int ouyear=0,oumonth=0,ouday=0,ouweek=0,ouhour=0,oumin=0,ousec=0;

	uyear = BCDYEAR;
	uyear = 0x2000 + uyear;
	umonth= BCDMON;
	uday  = BCDDAY;
	uhour = BCDHOUR;
	umin  = BCDMIN;
	usec  = BCDSEC;
	uweek  = BCDWEEK;
	
	//hour
	number_show(x, y, numbers, (ouhour>>4)& 0xF, font_with, back_color);
	number_show(x, y, numbers, (uhour>>4)& 0xF, font_with, color);
	number_show(x+font_with+2, y, numbers, ouhour& 0xF, font_with, back_color);
	number_show(x+font_with+2, y, numbers, uhour & 0xF, font_with, color);
	ouhour = uhour;
	
	if(font_with == 8)
		lcd_show_font(x+2*(font_with+2), y, &font_diangan_16[0], 16, 1, color);
	else 
		lcd_draw_line_xy(x+2*(font_with+3), y+16, x+2*(font_with+3)+16, y+16, 3, color);
	//min
	number_show(x+3*(font_with+2), y, numbers, (oumin>>4)& 0xF, font_with, back_color);
	number_show(x+3*(font_with+2), y, numbers, (umin>>4)& 0xF,  font_with, color);
	number_show(x+4*(font_with+2), y, numbers, oumin& 0xF,      font_with, back_color);
	number_show(x+4*(font_with+2), y, numbers, umin & 0xF,      font_with, color);
	oumin = umin;
	if(font_with == 8)
		lcd_show_font(x+5*(font_with+2), y, &font_diangan_16[0], 16, 1, color);
	else
		lcd_draw_line_xy(x+5*(font_with+3)-1, y+16, x+5*(font_with+3)+16, y+16, 3, color);
		//lcd_show_font(x+5*(font_with+2), y, &font_diangan_32[0], 32, 1, color);
	//sec
	number_show(x+6*(font_with+2), y, numbers, (ousec>>4)%10, font_with, back_color);
	number_show(x+6*(font_with+2), y, numbers, (usec>>4)%10,  font_with, color);
	number_show(x+7*(font_with+2), y, numbers, ousec& 0xF,    font_with, back_color);
	number_show(x+7*(font_with+2), y, numbers, usec & 0xF,    font_with, color);
	ousec = usec;
	
	
	//year
	number_show(x+9*(font_with+2), y, numbers, (ouyear>>12)& 0xF, font_with, back_color);
	number_show(x+10*(font_with+2), y, numbers, (ouyear>>8)& 0xF, font_with, back_color);
	number_show(x+11*(font_with+2), y, numbers, (ouyear>>4)& 0xF, font_with, back_color);
	number_show(x+9*(font_with+2), y, numbers, (uyear>>12)& 0xF,  font_with, color);
	number_show(x+10*(font_with+2), y, numbers, (uyear>>8)& 0xF,  font_with, color);
	number_show(x+11*(font_with+2), y, numbers, (uyear>>4)& 0xF,  font_with, color);
	number_show(x+12*(font_with+2), y, numbers, ouyear& 0xF,      font_with, back_color);
	number_show(x+12*(font_with+2), y, numbers, uyear & 0xF,      font_with, color);
	ouyear = uyear;
	
	
	if(font_with == 8)
		lcd_show_font(x+13*(font_with+2), y, &font_diangan_16[16*16/8], 8, 0, color);
	//else
	//	lcd_draw_line_xy(x+13*(font_with+3), y+16, x+13*(font_with)+16, y+16, 3, color);
		//lcd_show_font(x+13*(font_with+2), y, &font_diangan_32[32*32/8], 16, 0, color);
	//month
	number_show(x+14*(font_with+2), y, numbers, (oumonth>>4)%10, font_with, back_color);
	number_show(x+14*(font_with+2), y, numbers, (umonth>>4)%10, font_with, color);
	number_show(x+15*(font_with+2), y, numbers, oumonth& 0xF, font_with, back_color);
	number_show(x+15*(font_with+2), y, numbers, umonth & 0xF, font_with, color);
	oumonth = umonth;
	
	if(font_with == 8)
		lcd_show_font(x+16*(font_with+2), y, &font_diangan_16[16*16/8], 8, 0, color);
	//else
	//	lcd_draw_line_xy(x+16*(font_with+3), y+16, x+16*(font_with+3)+1, y+16, 3, color);
		//lcd_show_font(x+16*(font_with+2), y, &font_diangan_32[32*32/8], 16, 0, color);
	//day
	number_show(x+17*(font_with+2), y, numbers, (ouday>>4)& 0xF, font_with, back_color);
	number_show(x+17*(font_with+2), y, numbers, (uday>>4)& 0xF,  font_with, color);
	number_show(x+18*(font_with+2), y, numbers, ouday& 0xF,      font_with, back_color);
	number_show(x+18*(font_with+2), y, numbers, uday & 0xF,      font_with, color);
	ouday = uday;
}
/*
void rtc_time_show(unsigned int x, unsigned int y, unsigned char *numbers , unsigned font_with, unsigned int color, unsigned back_color)
{
	unsigned int uyear,umonth,uday,uweek,uhour,umin,usec;
	static unsigned int ouyear=0,oumonth=0,ouday=0,ouweek=0,ouhour=0,oumin=0,ousec=0;

	uyear = year;
	uyear = 2000 + uyear;
	umonth= month;
	uday  = day;
	uhour = hour;
	umin  = min;
	usec  = sec;
	uweek  = weekday;
	
	//hour
	number_show(x, y, numbers, ouhour/10, font_with, back_color);
	number_show(x, y, numbers, uhour/10, font_with, color);
	number_show(x+font_with+2, y, numbers, ouhour%10, font_with, back_color);
	number_show(x+font_with+2, y, numbers, uhour %10, font_with, color);
	ouhour = uhour;
	
	lcd_show_font(x+2*(font_with+2), y, &font_diangan_16[0], 16, 1, color);
	//min
	number_show(x+3*(font_with+2), y, numbers, (oumin>>4)/10, font_with, back_color);
	number_show(x+3*(font_with+2), y, numbers, (umin>>4)/10,  font_with, color);
	number_show(x+4*(font_with+2), y, numbers, oumin%10,      font_with, back_color);
	number_show(x+4*(font_with+2), y, numbers, umin %10,      font_with, color);
	oumin = umin;
	
	lcd_show_font(x+5*(font_with+2), y, &font_diangan_16[0], 16, 1, color);
	//sec
	number_show(x+6*(font_with+2), y, numbers, ousec/10, font_with, back_color);
	number_show(x+6*(font_with+2), y, numbers, usec /10,  font_with, color);
	number_show(x+7*(font_with+2), y, numbers, ousec%10,    font_with, back_color);
	number_show(x+7*(font_with+2), y, numbers, usec %10,    font_with, color);
	ousec = usec;
	
	
	//year
	number_show(x+9*(font_with+2), y, numbers, (ouyear>>12)& 0xF, font_with, back_color);
	number_show(x+10*(font_with+2), y, numbers, (ouyear>>8)& 0xF, font_with, back_color);
	number_show(x+11*(font_with+2), y, numbers, (ouyear>>4)& 0xF, font_with, back_color);
	number_show(x+9*(font_with+2), y, numbers, (uyear>>12)& 0xF,  font_with, color);
	number_show(x+10*(font_with+2), y, numbers, (uyear>>8)& 0xF,  font_with, color);
	number_show(x+11*(font_with+2), y, numbers, (uyear>>4)& 0xF,  font_with, color);
	number_show(x+12*(font_with+2), y, numbers, ouyear& 0xF,      font_with, back_color);
	number_show(x+12*(font_with+2), y, numbers, uyear & 0xF,      font_with, color);
	ouyear = uyear;
	
	
	//number_show(x+130, y, font_diangan_16, 1, 8, color);
	lcd_show_font(x+13*(font_with+2), y, &font_diangan_16[16*16/8], 8, 0, color);
	//month
	number_show(x+14*(font_with+2), y, numbers, oumonth/10, font_with, back_color);
	number_show(x+14*(font_with+2), y, numbers, umonth/10, font_with, color);
	number_show(x+15*(font_with+2), y, numbers, oumonth%10, font_with, back_color);
	number_show(x+15*(font_with+2), y, numbers, umonth %10, font_with, color);
	oumonth = umonth;
	
	//number_show(x+160, y, font_diangan_16, 1, 8, color);
	lcd_show_font(x+16*(font_with+2), y, &font_diangan_16[16*16/8], 8, 0, color);
	//day
	number_show(x+17*(font_with+2), y, numbers, ouday/10, font_with, back_color);
	number_show(x+17*(font_with+2), y, numbers, uday/10,  font_with, color);
	number_show(x+18*(font_with+2), y, numbers, ouday%10,      font_with, back_color);
	number_show(x+18*(font_with+2), y, numbers, uday %10,      font_with, color);
	ouday = uday;
}
*/

void rtc_settime(void)
{
	/* 将时间转化为BCD码 */
	year = ( ((year/100)<<8) |(((year/10)%10)<<4)| (year%10)  );
	month  = ( ((month/10)<<4)| (month%10) );
	day = ( ((day/10)<<4) | (day%10) );
	weekday = (weekday%10);							
	hour =( ((hour/10)<<4) | (hour%10) );
	min  =( ((min/10)<<4)  | (min%10) );
	sec  =( ((sec/10)<<4) | (sec%10) );

	rtc_enable(TRUE);
	
	
	/* 保存BCD码 */
	BCDSEC  = sec;
	BCDMIN  = min;
	BCDHOUR = hour;
	BCDDAY = day;
	BCDWEEK  = weekday;
	BCDMON  = month;
	BCDYEAR = year;
	
	/* 禁止rtc */
	rtc_enable(FALSE);
	
	printf("rtc reset success\r\n");
}