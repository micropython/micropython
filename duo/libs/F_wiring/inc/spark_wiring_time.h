/**
 ******************************************************************************
 * @file    spark_wiring_time.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    3-March-2014
 * @brief   Header for spark_wiring_time.cpp module
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#ifndef __SPARK_WIRING_TIME_H
#define __SPARK_WIRING_TIME_H

#include "spark_wiring_string.h"
#include <time.h>

extern const char* TIME_FORMAT_DEFAULT;
extern const char* TIME_FORMAT_ISO8601_FULL;


class TimeClass {
public:
	// Arduino time and date functions
	static int     hour();            			// current hour
	static int     hour(time_t t);				// the hour for the given time
	static int     hourFormat12();    			// current hour in 12 hour format
	static int     hourFormat12(time_t t);		// the hour for the given time in 12 hour format
	static uint8_t isAM();            			// returns true if time now is AM
	static uint8_t isAM(time_t t);    			// returns true the given time is AM
	static uint8_t isPM();            			// returns true if time now is PM
	static uint8_t isPM(time_t t);    			// returns true the given time is PM
	static int     minute();          			// current minute
	static int     minute(time_t t);  			// the minute for the given time
	static int     second();          			// current second
	static int     second(time_t t);  			// the second for the given time
	static int     day();             			// current day
	static int     day(time_t t);     			// the day for the given time
	static int     weekday();         			// the current weekday
	static int     weekday(time_t t); 			// the weekday for the given time
	static int     month();           			// current month
	static int     month(time_t t);   			// the month for the given time
	static int     year();            			// current four digit year
	static int     year(time_t t);    			// the year for the given time
	static time_t  now();              			// return the current time as seconds since Jan 1 1970
	static time_t  local();						// return the time as seconds since Jan 1 1970 in the local timezone.
	static void    zone(float GMT_Offset);		// set the time zone (+/-) offset from GMT
	static float	   zone();						// retrieve the current timezone
	static void    setTime(time_t t);			// set the given time as unix/rtc time


        /* return string representation of the current time */
        inline String timeStr()
        {
                return timeStr(now());
        }

        /* return string representation for the given time */
        static String timeStr(time_t t);

        /**
         * Return a string representation of the given time using strftime().
         * This function takes several kilobytes of flash memory so it's kept separate
         * from `timeStr()` to reduce memory footprint for applications that don't use
         * alternative time formats.
         *
         * @param t
         * @param format_spec
         * @return
         */
        String format(time_t t, const char* format_spec=NULL);

        inline String format(const char* format_spec=NULL)
        {
            return format(now(), format_spec);
        }

        void setFormat(const char* format)
        {
            this->format_spec = format;
        }

        const char* getFormat() const { return format_spec; }

private:
    static const char* format_spec;
    static String timeFormatImpl(tm* calendar_time, const char* format, int time_zone);

};

extern TimeClass Time;	//eg. usage: Time.day();

#endif
