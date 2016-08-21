#ifndef __RTC_H__
#define __RTC_H__

void rtc_ticktime_enable(unsigned char bdata);
void rtc_realtime_display(void);
void rtc_enable(unsigned char bdata);
void rtc_printf(void);
void rtc_settime(void);

#endif