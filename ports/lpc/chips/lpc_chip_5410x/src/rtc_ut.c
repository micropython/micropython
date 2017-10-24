/*
 * @brief RTC tick to (a more) Universal Time
 * Adds conversion functions to use an RTC that only provides a
 * seconds capability to provide "struct tm" support.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "rtc_ut.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define SECSPERMIN      (60)
#define MINSPERHOUR     (60)
#define SECSPERHOUR     (SECSPERMIN * MINSPERHOUR)
#define HOURSPERDAY     (24)
#define SECSPERDAY      (SECSPERMIN * MINSPERHOUR * HOURSPERDAY)
#define DAYSPERWEEK     (7)
#define MONETHSPERYEAR  (12)
#define DAYSPERYEAR     (365)
#define DAYSPERLEAPYEAR (366)

/* Days per month, LY is special */
static uint8_t daysPerMonth[2][MONETHSPERYEAR] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},	/* Normal year */
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},	/* Leap year */
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Converts the number of days offset from the start year to real year
   data accounting for leap years */
static void GetDMLY(int dayOff, struct tm *pTime)
{
	bool YearFound = false;
	int daysYear = dayOff;
	int leapYear, curLeapYear, year = TM_YEAR_BASE, monthYear = 0;
	bool MonthFound = false;

	/* Leap year check for less than 1 year time */
	if ( ((year % 4) == 0) && (year != 2000) ) {
		curLeapYear = 1;
	}
	else {
		curLeapYear = 0;
	}

	/* Determine offset of years from days offset */
	while (YearFound == false) {
		if ( ((year % 4) == 0) && (year != 2000) ) {
			/* Leap year, 366 days */
			daysYear = DAYSPERLEAPYEAR;
			leapYear = 1;
		}
		else {
			/* Leap year, 365 days */
			daysYear = DAYSPERYEAR;
			leapYear = 0;
		}

		if (dayOff >= daysYear) {
			dayOff -= daysYear;
			year++;
		}
		else {
			YearFound = true;
			curLeapYear = leapYear;	/* In a leap year */
		}

	}

	/* Save relative year and day into year */
	pTime->tm_year = year - TM_YEAR_BASE;	/* Base year relative */
	pTime->tm_yday = dayOff;/* 0 relative */

	/* Determine offset of months from days offset */
	while (MonthFound == false) {
		if ((dayOff + 1) > daysPerMonth[curLeapYear][monthYear]) {
			/* Next month */
			dayOff -= daysPerMonth[curLeapYear][monthYear];
			monthYear++;
		}
		else {
			/* Month found */
			MonthFound = true;
		}
	}

	pTime->tm_mday = dayOff + 1;/* 1 relative */
	pTime->tm_mon = monthYear;	/* 0 relative */
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Converts an RTC tick time to Universal time */
void ConvertRtcTime(uint32_t rtcTick, struct tm *pTime)
{
	int daySeconds, dayNum;

	/* Get day offset and seconds since start */
	dayNum = (int) (rtcTick / SECSPERDAY);
	daySeconds = (int) (rtcTick % SECSPERDAY);

	/* Fill in secs, min, hours */
	pTime->tm_sec = daySeconds % 60;
	pTime->tm_hour = daySeconds / SECSPERHOUR;
	pTime->tm_min = (daySeconds - (pTime->tm_hour * SECSPERHOUR)) / SECSPERMIN;

	/* Weekday, 0 = Sunday, 6 = Saturday */
	pTime->tm_wday = (dayNum + TM_DAYOFWEEK) % DAYSPERWEEK;

	/* Get year, month, day of month, and day of year */
	GetDMLY(dayNum, pTime);

	/* Not supported in this driver */
	pTime->tm_isdst = 0;
}

/* Converts a Universal time to RTC tick time */
void ConvertTimeRtc(struct tm *pTime, uint32_t *rtcTick)
{
	int leapYear, year = TM_YEAR_BASE;
	uint32_t dayOff, monthOff, monthCur, rtcTicks = 0;

	/* Add days for each year and leap year */
	while (year < pTime->tm_year + TM_YEAR_BASE) {
		if ( ((year % 4) == 0) && (year != 2000) ) {
			/* Leap year, 366 days */
			rtcTicks += DAYSPERLEAPYEAR * SECSPERDAY;
		}
		else {
			/* Leap year, 365 days */
			rtcTicks += DAYSPERYEAR * SECSPERDAY;
		}
		year++;
	}

	/* Day and month are 0 relative offsets since day and month start at 1 */
	dayOff = (uint32_t) pTime->tm_mday - 1;
	monthOff = (uint32_t) pTime->tm_mon;

	/* Add in seconds for passed months */
	if ( ((year % 4) == 0) && (year != 2000) )
		leapYear = 1;
	else
		leapYear = 0;

	for (monthCur = 0; monthCur < monthOff; monthCur++) {
		rtcTicks += (uint32_t) (daysPerMonth[leapYear][monthCur] * SECSPERDAY);
	}

	/* Add in seconds for day offset into the current month */
	rtcTicks += (dayOff * SECSPERDAY);

	/* Add in seconds for hours, minutes, and seconds */
	rtcTicks += (uint32_t) (pTime->tm_hour * SECSPERHOUR);
	rtcTicks += (uint32_t) (pTime->tm_min * SECSPERMIN);
	rtcTicks += (uint32_t) pTime->tm_sec;

	*rtcTick = rtcTicks;
}
