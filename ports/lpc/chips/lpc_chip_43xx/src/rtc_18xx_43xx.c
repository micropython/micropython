/*
 * @brief LPC18xx/43xx RTC driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize the RTC peripheral */
void Chip_RTC_Init(LPC_RTC_T *pRTC)
{
	Chip_Clock_RTCEnable();

	/* 2-Second delay after enabling RTC clock */
	LPC_ATIMER->DOWNCOUNTER = 2048;
	while (LPC_ATIMER->DOWNCOUNTER);

	/* Disable RTC */
	Chip_RTC_Enable(pRTC, DISABLE);

	/* Disable Calibration */
	Chip_RTC_CalibCounterCmd(pRTC, DISABLE);

	/* Reset RTC Clock */
	Chip_RTC_ResetClockTickCounter(pRTC);

	/* Clear counter increment and alarm interrupt */
	pRTC->ILR = RTC_IRL_RTCCIF | RTC_IRL_RTCALF;
	while (pRTC->ILR != 0) {}

	/* Clear all register to be default */
	pRTC->CIIR = 0x00;
	pRTC->AMR = 0xFF;
	pRTC->CALIBRATION = 0x00;
}

/*De-initialize the RTC peripheral */
void Chip_RTC_DeInit(LPC_RTC_T *pRTC)
{
	pRTC->CCR = 0x00;
}

/* Reset clock tick counter in the RTC peripheral */
void Chip_RTC_ResetClockTickCounter(LPC_RTC_T *pRTC)
{
	/* Reset RTC clock*/
	pRTC->CCR |= RTC_CCR_CTCRST;
	while (!(pRTC->CCR & RTC_CCR_CTCRST)) {}

	/* Finish resetting RTC clock */
	pRTC->CCR = (pRTC->CCR & ~RTC_CCR_CTCRST) & RTC_CCR_BITMASK;
	while (pRTC->CCR & RTC_CCR_CTCRST) {}
}

/* Start/Stop RTC peripheral */
void Chip_RTC_Enable(LPC_RTC_T *pRTC, FunctionalState NewState)
{
	if (NewState == ENABLE) {
		pRTC->CCR |= RTC_CCR_CLKEN;
	} else {
		pRTC->CCR = (pRTC->CCR & ~RTC_CCR_CLKEN) & RTC_CCR_BITMASK;
	}
}

/* Enable/Disable Counter increment interrupt for a time type in the RTC peripheral */
void Chip_RTC_CntIncrIntConfig(LPC_RTC_T *pRTC, uint32_t cntrMask, FunctionalState NewState)
{
	if (NewState == ENABLE) {
		pRTC->CIIR |= cntrMask;
	}

	else {
		pRTC->CIIR &= (~cntrMask) & RTC_AMR_CIIR_BITMASK;
		while (pRTC->CIIR & cntrMask) {}
	}
}

/* Enable/Disable Alarm interrupt for a time type in the RTC peripheral */
void Chip_RTC_AlarmIntConfig(LPC_RTC_T *pRTC, uint32_t alarmMask, FunctionalState NewState)
{
	if (NewState == ENABLE) {
		pRTC->AMR &= (~alarmMask) & RTC_AMR_CIIR_BITMASK;
	}
	else {
		pRTC->AMR |= (alarmMask);
		while ((pRTC->AMR & alarmMask) == 0) {}
	}
}

/* Set full time in the RTC peripheral */
void Chip_RTC_SetFullTime(LPC_RTC_T *pRTC, RTC_TIME_T *pFullTime)
{
	RTC_TIMEINDEX_T i;
	uint32_t ccr_val = pRTC->CCR;

	/* Temporarily disable */
	if (ccr_val & RTC_CCR_CLKEN) {
		pRTC->CCR = ccr_val & (~RTC_CCR_CLKEN) & RTC_CCR_BITMASK;
	}

	/* Date time setting */
	for (i = RTC_TIMETYPE_SECOND; i < RTC_TIMETYPE_LAST; i++) {
		pRTC->TIME[i] = pFullTime->time[i];
	}

	/* Restore to old setting */
	pRTC->CCR = ccr_val;
}

/* Get full time from the RTC peripheral */
void Chip_RTC_GetFullTime(LPC_RTC_T *pRTC, RTC_TIME_T *pFullTime)
{
	RTC_TIMEINDEX_T i;
	uint32_t secs = 0xFF;

	/* Read full time, but verify second tick didn't change during the read. If
	   it did, re-read the time again so it will be consistent across all fields. */
	while (secs != pRTC->TIME[RTC_TIMETYPE_SECOND]) {
		secs = pFullTime->time[RTC_TIMETYPE_SECOND] = pRTC->TIME[RTC_TIMETYPE_SECOND];
		for (i = RTC_TIMETYPE_MINUTE; i < RTC_TIMETYPE_LAST; i++) {
			pFullTime->time[i] = pRTC->TIME[i];
		}
	}
}

/* Set full alarm time in the RTC peripheral */
void Chip_RTC_SetFullAlarmTime(LPC_RTC_T *pRTC, RTC_TIME_T *pFullTime)
{
	RTC_TIMEINDEX_T i;

	for (i = RTC_TIMETYPE_SECOND; i < RTC_TIMETYPE_LAST; i++) {
		pRTC->ALRM[i] = pFullTime->time[i];
	}
}

/* Get full alarm time in the RTC peripheral */
void Chip_RTC_GetFullAlarmTime(LPC_RTC_T *pRTC, RTC_TIME_T *pFullTime)
{
	RTC_TIMEINDEX_T i;

	for (i = RTC_TIMETYPE_SECOND; i < RTC_TIMETYPE_LAST; i++) {
		pFullTime->time[i] = pRTC->ALRM[i];
	}
}

/* Enable/Disable calibration counter in the RTC peripheral */
void Chip_RTC_CalibCounterCmd(LPC_RTC_T *pRTC, FunctionalState NewState)
{
	if (NewState == ENABLE) {
		do {
			pRTC->CCR &= (~RTC_CCR_CCALEN) & RTC_CCR_BITMASK;
		} while (pRTC->CCR & RTC_CCR_CCALEN);
	}
	else {
		pRTC->CCR |= RTC_CCR_CCALEN;
	}
}

#if RTC_EV_SUPPORT
/* Get first timestamp value */
void Chip_RTC_EV_GetFirstTimeStamp(LPC_RTC_T *pRTC, RTC_EV_CHANNEL_T ch, RTC_EV_TIMESTAMP_T *pTimeStamp)
{
	pTimeStamp->sec = RTC_ER_TIMESTAMP_SEC(pRTC->ERFIRSTSTAMP[ch]);
	pTimeStamp->min = RTC_ER_TIMESTAMP_MIN(pRTC->ERFIRSTSTAMP[ch]);
	pTimeStamp->hour = RTC_ER_TIMESTAMP_HOUR(pRTC->ERFIRSTSTAMP[ch]);
	pTimeStamp->dayofyear = RTC_ER_TIMESTAMP_DOY(pRTC->ERFIRSTSTAMP[ch]);
}

/* Get last timestamp value */
void Chip_RTC_EV_GetLastTimeStamp(LPC_RTC_T *pRTC, RTC_EV_CHANNEL_T ch, RTC_EV_TIMESTAMP_T *pTimeStamp)
{
	pTimeStamp->sec = RTC_ER_TIMESTAMP_SEC(pRTC->ERLASTSTAMP[ch]);
	pTimeStamp->min = RTC_ER_TIMESTAMP_MIN(pRTC->ERLASTSTAMP[ch]);
	pTimeStamp->hour = RTC_ER_TIMESTAMP_HOUR(pRTC->ERLASTSTAMP[ch]);
	pTimeStamp->dayofyear = RTC_ER_TIMESTAMP_DOY(pRTC->ERLASTSTAMP[ch]);
}

#endif /*RTC_EV_SUPPORT*/







