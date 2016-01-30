/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "rtc_api.h"
     
 int main() 
 {
    time_t seconds;
    struct tm *timeinfo;
    
    rtc_init();
    rtc_write(1256729737);  // Set RTC time to Wed, 28 Oct 2009 11:35:37

    while(1) {
        seconds = rtc_read();
        timeinfo = localtime(&seconds);

        DBG_8195A("Time as seconds since January 1, 1970 = %d\n", seconds);

        DBG_8195A("Time as a basic string = %s", ctime(&seconds));

        DBG_8195A("Time as a custom formatted string = %d-%d-%d %d:%d:%d ", 
            timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour,
            timeinfo->tm_min,timeinfo->tm_sec);

        wait(1);
    }
 }

