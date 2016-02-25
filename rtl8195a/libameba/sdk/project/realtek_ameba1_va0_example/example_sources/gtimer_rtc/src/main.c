/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include <time.h>
#include "timer_api.h"
#include "main.h"


#define SW_RTC_TIMER_ID        TIMER5

static gtimer_t sw_rtc;
static volatile struct tm rtc_timeinfo;

const static u8 dim[14] = { 
	31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28 };

static inline bool is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

	
static u8 days_in_month (u8 month, u8 year)
{
	u8 ret = dim [ month - 1 ];
	if (ret == 0)
		ret = is_leap_year (year) ? 29 : 28;
	return ret;
}	

static void sw_rtc_tick_handler(uint32_t id)
{
    if(++rtc_timeinfo.tm_sec > 59) {                               // Increment seconds, check for overflow
        rtc_timeinfo.tm_sec = 0;                                   // Reset seconds
        if(++rtc_timeinfo.tm_min > 59) {                           // Increment minutes, check for overflow
            rtc_timeinfo.tm_min = 0;                               // Reset minutes
            if(++rtc_timeinfo.tm_hour > 23) {                      // Increment hours, check for overflow
                rtc_timeinfo.tm_hour = 0;                          // Reset hours
                ++rtc_timeinfo.tm_yday;                            // Increment day of year
                if(++rtc_timeinfo.tm_wday > 6)                     // Increment day of week, check for overflow
                    rtc_timeinfo.tm_wday = 0;                      // Reset day of week
                                                        // Increment day of month, check for overflow
                if(++rtc_timeinfo.tm_mday >
                    days_in_month(rtc_timeinfo.tm_mon, rtc_timeinfo.tm_year)) {
                    rtc_timeinfo.tm_mday = 1;                      // Reset day of month
                    if(++rtc_timeinfo.tm_mon > 11) {               // Increment month, check for overflow
                        rtc_timeinfo.tm_mon = 0;                   // Reset month
                        rtc_timeinfo.tm_yday = 0;                  // Reset day of year
                        ++rtc_timeinfo.tm_year;                    // Increment year
                    }                                   // - year       
                }                                       // - month
            }                                           // - day
        }                                               // - hour
    }                                     
}

static void rtc_init(void)
{
    // Initial a periodical timer
    gtimer_init(&sw_rtc, SW_RTC_TIMER_ID);
    
    // Tick every 1 sec
    gtimer_start_periodical(&sw_rtc, 1000000, (void*)sw_rtc_tick_handler, (uint32_t)&sw_rtc);
}

static void rtc_deinit(void)
{
    gtimer_stop(&sw_rtc);
    gtimer_deinit(&sw_rtc);
}

static void rtc_set_time(uint32_t year, uint8_t mon, uint8_t mday, uint8_t wday, 
    uint8_t hour, uint8_t min, uint8_t sec)
{
    int i;
    
    gtimer_stop(&sw_rtc);
    rtc_timeinfo.tm_sec = sec;
    rtc_timeinfo.tm_min = min;
    rtc_timeinfo.tm_hour = hour;
    rtc_timeinfo.tm_mday = mday-1;  
    rtc_timeinfo.tm_wday = wday-1;
    rtc_timeinfo.tm_yday = 0;
    for (i=0;i<(mon-1);i++) {
        rtc_timeinfo.tm_yday += days_in_month(i,year);
    }
    rtc_timeinfo.tm_yday += (mday-1);
    rtc_timeinfo.tm_mon = mon-1;
    rtc_timeinfo.tm_year = year;
    gtimer_start(&sw_rtc);
}

static void rtc_read_time(struct tm *timeinfo)
{
    _memcpy((void*)timeinfo, (void*)&rtc_timeinfo, sizeof(struct tm));
    timeinfo->tm_mon++;
    timeinfo->tm_mday++;
    timeinfo->tm_wday++;
    timeinfo->tm_yday++;
}

void main(void)
{
    struct tm timeinfo;

    rtc_init();
    
    // Give RTC a initial value: 2015/4/15 (Wed) 12:00:00
    rtc_set_time(2015, 4, 15, 3, 12, 0, 0);

    while (1) {
        rtc_read_time(&timeinfo);
        DBG_8195A("%d-%d-%d[%d] %d:%d:%d\r\n", timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday,
            timeinfo.tm_wday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        wait_ms(1000);
    }
    rtc_deinit();
}

