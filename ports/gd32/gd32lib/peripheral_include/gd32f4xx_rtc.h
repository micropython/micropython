/*!
    \file    gd32f4xx_rtc.c
    \brief   definitions for the RTC

    \version 2026-02-05, V3.3.3, firmware for GD32F4xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/


#ifndef GD32F4XX_RTC_H
#define GD32F4XX_RTC_H

#include "gd32f4xx.h"

/* RTC definitions */
#define RTC                                RTC_BASE

/* registers definitions */
#define RTC_TIME                           REG32((RTC) + 0x00U)                 /*!< RTC time of day register */
#define RTC_DATE                           REG32((RTC) + 0x04U)                 /*!< RTC date register */
#define RTC_CTL                            REG32((RTC) + 0x08U)                 /*!< RTC control register */
#define RTC_STAT                           REG32((RTC) + 0x0CU)                 /*!< RTC status register */
#define RTC_PSC                            REG32((RTC) + 0x10U)                 /*!< RTC time prescaler register */
#define RTC_WUT                            REG32((RTC) + 0x14U)                 /*!< RTC wakeup timer regiser */
#define RTC_COSC                           REG32((RTC) + 0x18U)                 /*!< RTC coarse calibration register */
#define RTC_ALRM0TD                        REG32((RTC) + 0x1CU)                 /*!< RTC alarm 0 time and date register */
#define RTC_ALRM1TD                        REG32((RTC) + 0x20U)                 /*!< RTC alarm 1 time and date register */
#define RTC_WPK                            REG32((RTC) + 0x24U)                 /*!< RTC write protection key register */
#define RTC_SS                             REG32((RTC) + 0x28U)                 /*!< RTC sub second register */
#define RTC_SHIFTCTL                       REG32((RTC) + 0x2CU)                 /*!< RTC shift function control register */
#define RTC_TTS                            REG32((RTC) + 0x30U)                 /*!< RTC time of timestamp register */
#define RTC_DTS                            REG32((RTC) + 0x34U)                 /*!< RTC date of timestamp register */
#define RTC_SSTS                           REG32((RTC) + 0x38U)                 /*!< RTC sub second of timestamp register */
#define RTC_HRFC                           REG32((RTC) + 0x3CU)                 /*!< RTC high resolution frequency compensation registor */
#define RTC_TAMP                           REG32((RTC) + 0x40U)                 /*!< RTC tamper register */
#define RTC_ALRM0SS                        REG32((RTC) + 0x44U)                 /*!< RTC alarm 0 sub second register */
#define RTC_ALRM1SS                        REG32((RTC) + 0x48U)                 /*!< RTC alarm 1 sub second register */
#define RTC_BKP0                           REG32((RTC) + 0x50U)                 /*!< RTC backup register */
#define RTC_BKP1                           REG32((RTC) + 0x54U)                 /*!< RTC backup register */
#define RTC_BKP2                           REG32((RTC) + 0x58U)                 /*!< RTC backup register */
#define RTC_BKP3                           REG32((RTC) + 0x5CU)                 /*!< RTC backup register */
#define RTC_BKP4                           REG32((RTC) + 0x60U)                 /*!< RTC backup register */
#define RTC_BKP5                           REG32((RTC) + 0x64U)                 /*!< RTC backup register */
#define RTC_BKP6                           REG32((RTC) + 0x68U)                 /*!< RTC backup register */
#define RTC_BKP7                           REG32((RTC) + 0x6CU)                 /*!< RTC backup register */
#define RTC_BKP8                           REG32((RTC) + 0x70U)                 /*!< RTC backup register */
#define RTC_BKP9                           REG32((RTC) + 0x74U)                 /*!< RTC backup register */
#define RTC_BKP10                          REG32((RTC) + 0x78U)                 /*!< RTC backup register */
#define RTC_BKP11                          REG32((RTC) + 0x7CU)                 /*!< RTC backup register */
#define RTC_BKP12                          REG32((RTC) + 0x80U)                 /*!< RTC backup register */
#define RTC_BKP13                          REG32((RTC) + 0x84U)                 /*!< RTC backup register */
#define RTC_BKP14                          REG32((RTC) + 0x88U)                 /*!< RTC backup register */
#define RTC_BKP15                          REG32((RTC) + 0x8CU)                 /*!< RTC backup register */
#define RTC_BKP16                          REG32((RTC) + 0x90U)                 /*!< RTC backup register */
#define RTC_BKP17                          REG32((RTC) + 0x94U)                 /*!< RTC backup register */
#define RTC_BKP18                          REG32((RTC) + 0x98U)                 /*!< RTC backup register */
#define RTC_BKP19                          REG32((RTC) + 0x9CU)                 /*!< RTC backup register */

/* bits definitions */
/* RTC_TIME */
#define RTC_TIME_SCU                       BITS(0,3)                            /*!< second units in BCD code */
#define RTC_TIME_SCT                       BITS(4,6)                            /*!< second tens in BCD code */
#define RTC_TIME_MNU                       BITS(8,11)                           /*!< minute units in BCD code */
#define RTC_TIME_MNT                       BITS(12,14)                          /*!< minute tens in BCD code */
#define RTC_TIME_HRU                       BITS(16,19)                          /*!< hour units in BCD code */
#define RTC_TIME_HRT                       BITS(20,21)                          /*!< hour tens in BCD code */
#define RTC_TIME_PM                        BIT(22)                              /*!< AM/PM notation */

/* RTC_DATE */
#define RTC_DATE_DAYU                      BITS(0,3)                            /*!< date units in BCD code */
#define RTC_DATE_DAYT                      BITS(4,5)                            /*!< date tens in BCD code */
#define RTC_DATE_MONU                      BITS(8,11)                           /*!< month units in BCD code */
#define RTC_DATE_MONT                      BIT(12)                              /*!< month tens in BCD code */
#define RTC_DATE_DOW                       BITS(13,15)                          /*!< day of week units */
#define RTC_DATE_YRU                       BITS(16,19)                          /*!< year units in BCD code */
#define RTC_DATE_YRT                       BITS(20,23)                          /*!< year tens in BCD code */

/* RTC_CTL */
#define RTC_CTL_WTCS                       BITS(0,2)                            /*!< auto wakeup timer clock selection */
#define RTC_CTL_TSEG                       BIT(3)                               /*!< valid event edge of time-stamp */
#define RTC_CTL_REFEN                      BIT(4)                               /*!< reference clock detection function enable */
#define RTC_CTL_BPSHAD                     BIT(5)                               /*!< shadow registers bypass control */
#define RTC_CTL_CS                         BIT(6)                               /*!< display format of clock system */
#define RTC_CTL_CCEN                       BIT(7)                               /*!< coarse calibration function enable */
#define RTC_CTL_ALRM0EN                    BIT(8)                               /*!< alarm0 function enable */
#define RTC_CTL_ALRM1EN                    BIT(9)                               /*!< alarm1 function enable */
#define RTC_CTL_WTEN                       BIT(10)                              /*!< auto wakeup timer function enable */
#define RTC_CTL_TSEN                       BIT(11)                              /*!< time-stamp function enable */
#define RTC_CTL_ALRM0IE                    BIT(12)                              /*!< RTC alarm0 interrupt enable */
#define RTC_CTL_ALRM1IE                    BIT(13)                              /*!< RTC alarm1 interrupt enable */
#define RTC_CTL_WTIE                       BIT(14)                              /*!< auto wakeup timer interrupt enable */
#define RTC_CTL_TSIE                       BIT(15)                              /*!< time-stamp interrupt enable */
#define RTC_CTL_A1H                        BIT(16)                              /*!< add 1 hour(summer time change) */
#define RTC_CTL_S1H                        BIT(17)                              /*!< subtract 1 hour(winter time change) */
#define RTC_CTL_DSM                        BIT(18)                              /*!< daylight saving mark */
#define RTC_CTL_COS                        BIT(19)                              /*!< calibration output selection */
#define RTC_CTL_OPOL                       BIT(20)                              /*!< output polarity */
#define RTC_CTL_OS                         BITS(21,22)                          /*!< output selection */
#define RTC_CTL_COEN                       BIT(23)                              /*!< calibration output enable */

/* RTC_STAT */
#define RTC_STAT_ALRM0WF                   BIT(0)                               /*!< alarm0 configuration can be write flag */
#define RTC_STAT_ALRM1WF                   BIT(1)                               /*!< alarm1 configuration can be write flag */
#define RTC_STAT_WTWF                      BIT(2)                               /*!< wakeup timer can be write flag */
#define RTC_STAT_SOPF                      BIT(3)                               /*!< shift function operation pending flag */
#define RTC_STAT_YCM                       BIT(4)                               /*!< year configuration mark status flag */
#define RTC_STAT_RSYNF                     BIT(5)                               /*!< register synchronization flag */
#define RTC_STAT_INITF                     BIT(6)                               /*!< initialization state flag */
#define RTC_STAT_INITM                     BIT(7)                               /*!< enter initialization mode */
#define RTC_STAT_ALRM0F                    BIT(8)                               /*!< alarm0 occurs flag */
#define RTC_STAT_ALRM1F                    BIT(9)                               /*!< alarm1 occurs flag */
#define RTC_STAT_WTF                       BIT(10)                              /*!< wakeup timer occurs flag */
#define RTC_STAT_TSF                       BIT(11)                              /*!< time-stamp flag */
#define RTC_STAT_TSOVRF                    BIT(12)                              /*!< time-stamp overflow flag */
#define RTC_STAT_TP0F                      BIT(13)                              /*!< RTC tamper 0 detected flag */
#define RTC_STAT_TP1F                      BIT(14)                              /*!< RTC tamper 1 detected flag */
#define RTC_STAT_SCPF                      BIT(16)                              /*!< smooth calibration pending flag */

/* RTC_PSC */
#define RTC_PSC_FACTOR_S                   BITS(0,14)                           /*!< synchronous prescaler factor */
#define RTC_PSC_FACTOR_A                   BITS(16,22)                          /*!< asynchronous prescaler factor */

/* RTC_WUT */
#define RTC_WUT_WTRV                       BITS(0,15)                           /*!< auto wakeup timer reloads value */

/* RTC_COSC */
#define RTC_COSC_COSS                      BITS(0,4)                            /*!< coarse calibration step */
#define RTC_COSC_COSD                      BIT(7)                               /*!< coarse calibration direction */

/* RTC_ALRMxTD */
#define RTC_ALRMXTD_SCU                    BITS(0,3)                            /*!< second units in BCD code */
#define RTC_ALRMXTD_SCT                    BITS(4,6)                            /*!< second tens in BCD code */
#define RTC_ALRMXTD_MSKS                   BIT(7)                               /*!< alarm second mask bit */
#define RTC_ALRMXTD_MNU                    BITS(8,11)                           /*!< minutes units in BCD code */
#define RTC_ALRMXTD_MNT                    BITS(12,14)                          /*!< minutes tens in BCD code */
#define RTC_ALRMXTD_MSKM                   BIT(15)                              /*!< alarm minutes mask bit */
#define RTC_ALRMXTD_HRU                    BITS(16,19)                          /*!< hour units in BCD code */
#define RTC_ALRMXTD_HRT                    BITS(20,21)                          /*!< hour units in BCD code */
#define RTC_ALRMXTD_PM                     BIT(22)                              /*!< AM/PM flag */
#define RTC_ALRMXTD_MSKH                   BIT(23)                              /*!< alarm hour mask bit */
#define RTC_ALRMXTD_DAYU                   BITS(24,27)                          /*!< date units or week day in BCD code */
#define RTC_ALRMXTD_DAYT                   BITS(28,29)                          /*!< date tens in BCD code */
#define RTC_ALRMXTD_DOWS                   BIT(30)                              /*!< day of week  selection */
#define RTC_ALRMXTD_MSKD                   BIT(31)                              /*!< alarm date mask bit */

/* RTC_WPK */
#define RTC_WPK_WPK                        BITS(0,7)                            /*!< key for write protection */

/* RTC_SS */
#define RTC_SS_SSC                         BITS(0,15)                           /*!< sub second value */

/* RTC_SHIFTCTL */
#define RTC_SHIFTCTL_SFS                   BITS(0,14)                           /*!< subtract a fraction of a second */
#define RTC_SHIFTCTL_A1S                   BIT(31)                              /*!< one second add */

/* RTC_TTS */
#define RTC_TTS_SCU                        BITS(0,3)                            /*!< second units in BCD code */
#define RTC_TTS_SCT                        BITS(4,6)                            /*!< second units in BCD code */
#define RTC_TTS_MNU                        BITS(8,11)                           /*!< minute units in BCD code */
#define RTC_TTS_MNT                        BITS(12,14)                          /*!< minute tens in BCD code */
#define RTC_TTS_HRU                        BITS(16,19)                          /*!< hour units in BCD code */
#define RTC_TTS_HRT                        BITS(20,21)                          /*!< hour tens in BCD code */
#define RTC_TTS_PM                         BIT(22)                              /*!< AM/PM notation */

/* RTC_DTS */
#define RTC_DTS_DAYU                       BITS(0,3)                            /*!< date units in BCD code */
#define RTC_DTS_DAYT                       BITS(4,5)                            /*!< date tens in BCD code */
#define RTC_DTS_MONU                       BITS(8,11)                           /*!< month units in BCD code */
#define RTC_DTS_MONT                       BIT(12)                              /*!< month tens in BCD code */
#define RTC_DTS_DOW                        BITS(13,15)                          /*!< day of week units */

/* RTC_SSTS */
#define RTC_SSTS_SSC                       BITS(0,15)                           /*!< timestamp sub second units */

/* RTC_HRFC */
#define RTC_HRFC_CMSK                      BITS(0,8)                            /*!< calibration mask number */
#define RTC_HRFC_CWND16                    BIT(13)                              /*!< calibration window select 16 seconds */
#define RTC_HRFC_CWND8                     BIT(14)                              /*!< calibration window select 16 seconds */
#define RTC_HRFC_FREQI                     BIT(15)                              /*!< increase RTC frequency by 488.5ppm */

/* RTC_TAMP */
#define RTC_TAMP_TP0EN                     BIT(0)                               /*!< tamper 0 detection enable */
#define RTC_TAMP_TP0EG                     BIT(1)                               /*!< tamper 0 event trigger edge for RTC tamp 0 input */
#define RTC_TAMP_TPIE                      BIT(2)                               /*!< tamper detection interrupt enable */
#define RTC_TAMP_TP1EN                     BIT(3)                               /*!< tamper 1 detection enable */
#define RTC_TAMP_TP1EG                     BIT(4)                               /*!< Tamper 1 event trigger edge for RTC tamp 1 input */
#define RTC_TAMP_TPTS                      BIT(7)                               /*!< make tamper function used for timestamp function */
#define RTC_TAMP_FREQ                      BITS(8,10)                           /*!< sample frequency of tamper event detection */
#define RTC_TAMP_FLT                       BITS(11,12)                          /*!< RTC tamp x filter count setting */
#define RTC_TAMP_PRCH                      BITS(13,14)                          /*!< precharge duration time of RTC tamp x */
#define RTC_TAMP_DISPU                     BIT(15)                              /*!< RTC tamp x pull up disable bit */
#define RTC_TAMP_TP0SEL                    BIT(16)                              /*!< Tamper 0 function input mapping selection */
#define RTC_TAMP_TSSEL                     BIT(17)                              /*!< Timestamp input mapping selection */
#define RTC_TAMP_AOT                       BIT(18)                              /*!< RTC_ALARM output Type */

/* RTC_ALRM0SS */
#define RTC_ALRM0SS_SSC                    BITS(0,14)                           /*!< alarm0 sub second value */
#define RTC_ALRM0SS_MASKSSC                BITS(24,27)                          /*!< mask control bit of SS */

/* RTC_ALRM1SS */
#define RTC_ALRM1SS_SSC                    BITS(0,14)                           /*!< alarm1 sub second value */
#define RTC_ALRM1SS_MASKSSC                BITS(24,27)                          /*!< mask control bit of SS */

/* constants definitions */
/* structure for initialization of the RTC */
typedef struct
{
    uint8_t year;                                                               /*!< RTC year value: 0x0 - 0x99(BCD format) */
    uint8_t month;                                                              /*!< RTC month value */
    uint8_t date;                                                               /*!< RTC date value: 0x1 - 0x31(BCD format) */
    uint8_t day_of_week;                                                        /*!< RTC weekday value */
    uint8_t hour;                                                               /*!< RTC hour value */
    uint8_t minute;                                                             /*!< RTC minute value: 0x0 - 0x59(BCD format) */
    uint8_t second;                                                             /*!< RTC second value: 0x0 - 0x59(BCD format) */
    uint16_t factor_asyn;                                                       /*!< RTC asynchronous prescaler value: 0x0 - 0x7F */
    uint16_t factor_syn;                                                        /*!< RTC synchronous prescaler value: 0x0 - 0x7FFF */
    uint32_t am_pm;                                                             /*!< RTC AM/PM value */
    uint32_t display_format;                                                    /*!< RTC time notation */
}rtc_parameter_struct;

/* structure for RTC alarm configuration */
typedef struct
{
    uint32_t alarm_mask;                                                        /*!< RTC alarm mask */
    uint32_t weekday_or_date;                                                   /*!< specify RTC alarm is on date or weekday */
    uint8_t alarm_day;                                                          /*!< RTC alarm date or weekday value*/
    uint8_t alarm_hour;                                                         /*!< RTC alarm hour value */
    uint8_t alarm_minute;                                                       /*!< RTC alarm minute value: 0x0 - 0x59(BCD format) */
    uint8_t alarm_second;                                                       /*!< RTC alarm second value: 0x0 - 0x59(BCD format) */
    uint32_t am_pm;                                                             /*!< RTC alarm AM/PM value */
}rtc_alarm_struct;

/* structure for RTC time-stamp configuration */
typedef struct
{
    uint8_t timestamp_month;                                                    /*!< RTC time-stamp month value */
    uint8_t timestamp_date;                                                     /*!< RTC time-stamp date value: 0x1 - 0x31(BCD format) */
    uint8_t timestamp_day;                                                      /*!< RTC time-stamp weekday value */
    uint8_t timestamp_hour;                                                     /*!< RTC time-stamp hour value */
    uint8_t timestamp_minute;                                                   /*!< RTC time-stamp minute value: 0x0 - 0x59(BCD format) */
    uint8_t timestamp_second;                                                   /*!< RTC time-stamp second value: 0x0 - 0x59(BCD format) */
    uint32_t am_pm;                                                             /*!< RTC time-stamp AM/PM value */
}rtc_timestamp_struct;

/* structure for RTC tamper configuration */
typedef struct
{
    uint32_t tamper_source;                                                     /*!< RTC tamper source */
    uint32_t tamper_trigger;                                                    /*!< RTC tamper trigger */
    uint32_t tamper_filter;                                                     /*!< RTC tamper consecutive samples needed during a voltage level detection */
    uint32_t tamper_sample_frequency;                                           /*!< RTC tamper sampling frequency during a voltage level detection */
    ControlStatus tamper_precharge_enable;                                      /*!< RTC tamper precharge feature during a voltage level detection */
    uint32_t tamper_precharge_time;                                             /*!< RTC tamper precharge duration if precharge feature is enabled */
    ControlStatus tamper_with_timestamp;                                        /*!< RTC tamper time-stamp feature */
}rtc_tamper_struct; 

/* time register value */
#define TIME_SC(regval)                    (BITS(0,6) & ((uint32_t)(regval) << 0))    /*!< write value to RTC_TIME_SC bit field */
#define GET_TIME_SC(regval)                GET_BITS((regval),0,6)                     /*!< get value of RTC_TIME_SC bit field */

#define TIME_MN(regval)                    (BITS(8,14) & ((uint32_t)(regval) << 8))   /*!< write value to RTC_TIME_MN bit field */
#define GET_TIME_MN(regval)                GET_BITS((regval),8,14)                    /*!< get value of RTC_TIME_MN bit field */

#define TIME_HR(regval)                    (BITS(16,21) & ((uint32_t)(regval) << 16)) /*!< write value to RTC_TIME_HR bit field */
#define GET_TIME_HR(regval)                GET_BITS((regval),16,21)                   /*!< get value of RTC_TIME_HR bit field */

#define RTC_AM                             ((uint32_t)0x00000000U)                    /*!< AM format */
#define RTC_PM                             RTC_TIME_PM                                /*!< PM format */

/* date register value */
#define DATE_DAY(regval)                   (BITS(0,5) & ((uint32_t)(regval) << 0))    /*!< write value to RTC_DATE_DAY bit field */
#define GET_DATE_DAY(regval)               GET_BITS((regval),0,5)                     /*!< get value of RTC_DATE_DAY bit field */

#define DATE_MON(regval)                   (BITS(8,12) & ((uint32_t)(regval) << 8))   /*!< write value to RTC_DATE_MON bit field */
#define GET_DATE_MON(regval)               GET_BITS((regval),8,12)                    /*!< get value of RTC_DATE_MON bit field */
#define RTC_JAN                            ((uint8_t)0x01U)                           /*!< janurary */
#define RTC_FEB                            ((uint8_t)0x02U)                           /*!< february */
#define RTC_MAR                            ((uint8_t)0x03U)                           /*!< march */
#define RTC_APR                            ((uint8_t)0x04U)                           /*!< april */
#define RTC_MAY                            ((uint8_t)0x05U)                           /*!< may */
#define RTC_JUN                            ((uint8_t)0x06U)                           /*!< june */
#define RTC_JUL                            ((uint8_t)0x07U)                           /*!< july */
#define RTC_AUG                            ((uint8_t)0x08U)                           /*!< august */
#define RTC_SEP                            ((uint8_t)0x09U)                           /*!< september */
#define RTC_OCT                            ((uint8_t)0x10U)                           /*!< october */
#define RTC_NOV                            ((uint8_t)0x11U)                           /*!< november */
#define RTC_DEC                            ((uint8_t)0x12U)                           /*!< december */

#define DATE_DOW(regval)                   (BITS(13,15) & ((uint32_t)(regval) << 13)) /*!< write value to RTC_DATE_DOW bit field */
#define GET_DATE_DOW(regval)               GET_BITS((uint32_t)(regval),13,15)         /*!< get value of RTC_DATE_DOW bit field */
#define RTC_MONDAY                         ((uint8_t)0x01)                            /*!< monday */
#define RTC_TUESDAY                        ((uint8_t)0x02)                            /*!< tuesday */
#define RTC_WEDSDAY                        ((uint8_t)0x03)                            /*!< wednesday */
#define RTC_THURSDAY                       ((uint8_t)0x04)                            /*!< thursday */
#define RTC_FRIDAY                         ((uint8_t)0x05)                            /*!< friday */
#define RTC_SATURDAY                       ((uint8_t)0x06)                            /*!< saturday */
#define RTC_SUNDAY                         ((uint8_t)0x07)                            /*!< sunday */

#define DATE_YR(regval)                    (BITS(16,23) & ((uint32_t)(regval) << 16)) /*!< write value to RTC_DATE_YR bit field */
#define GET_DATE_YR(regval)                GET_BITS((regval),16,23)                   /*!< get value of RTC_DATE_YR bit field */

/* ctl register value */
#define CTL_OS(regval)                     (BITS(21,22) & ((uint32_t)(regval) << 21)) /*!< write value to RTC_CTL_OS bit field */
#define RTC_OS_DISABLE                     CTL_OS(0)                                  /*!< disable output RTC_ALARM */
#define RTC_OS_ALARM0                      CTL_OS(1)                                  /*!< enable alarm0 flag output */
#define RTC_OS_ALARM1                      CTL_OS(2)                                  /*!< enable alarm1 flag output */
#define RTC_OS_WAKEUP                      CTL_OS(3)                                  /*!< enable wakeup flag output */

#define RTC_CALIBRATION_512HZ              RTC_CTL_COEN                               /*!< calibration output of 512Hz is enable */
#define RTC_CALIBRATION_1HZ                (RTC_CTL_COEN | RTC_CTL_COS)               /*!< calibration output of 1Hz is enable */
#define RTC_ALARM0_HIGH                    RTC_OS_ALARM0                              /*!< enable alarm0 flag output with high level */
#define RTC_ALARM0_LOW                     (RTC_OS_ALARM0 | RTC_CTL_OPOL)             /*!< enable alarm0 flag output with low level*/
#define RTC_ALARM1_HIGH                    RTC_OS_ALARM1                              /*!< enable alarm1 flag output with high level */
#define RTC_ALARM1_LOW                     (RTC_OS_ALARM1 | RTC_CTL_OPOL)             /*!< enable alarm1 flag output with low level*/
#define RTC_WAKEUP_HIGH                    RTC_OS_WAKEUP                              /*!< enable wakeup flag output with high level */
#define RTC_WAKEUP_LOW                     (RTC_OS_WAKEUP | RTC_CTL_OPOL)             /*!< enable wakeup flag output with low level*/

#define RTC_24HOUR                         ((uint32_t)0x00000000U)                    /*!< 24-hour format */
#define RTC_12HOUR                         RTC_CTL_CS                                 /*!< 12-hour format */

#define RTC_TIMESTAMP_RISING_EDGE          ((uint32_t)0x00000000U)                    /*!< rising edge is valid event edge for time-stamp event */
#define RTC_TIMESTAMP_FALLING_EDGE         RTC_CTL_TSEG                               /*!< falling edge is valid event edge for time-stamp event */

/* psc register value */
#define PSC_FACTOR_S(regval)               (BITS(0,14) & ((uint32_t)(regval) << 0))   /*!< write value to RTC_PSC_FACTOR_S bit field */
#define GET_PSC_FACTOR_S(regval)           GET_BITS((regval),0,14)                    /*!< get value of RTC_PSC_FACTOR_S bit field */

#define PSC_FACTOR_A(regval)               (BITS(16,22) & ((uint32_t)(regval) << 16)) /*!< write value to RTC_PSC_FACTOR_A bit field */
#define GET_PSC_FACTOR_A(regval)           GET_BITS((regval),16,22)                   /*!< get value of RTC_PSC_FACTOR_A bit field */

/* alrmtd register value */
#define ALRMTD_SC(regval)                  (BITS(0,6) & ((uint32_t)(regval)<< 0))     /*!< write value to RTC_ALRMTD_SC bit field */
#define GET_ALRMTD_SC(regval)              GET_BITS((regval),0,6)                     /*!< get value of RTC_ALRMTD_SC bit field */

#define ALRMTD_MN(regval)                  (BITS(8,14) & ((uint32_t)(regval) << 8))   /*!< write value to RTC_ALRMTD_MN bit field */
#define GET_ALRMTD_MN(regval)              GET_BITS((regval),8,14)                    /*!< get value of RTC_ALRMTD_MN bit field */

#define ALRMTD_HR(regval)                  (BITS(16,21) & ((uint32_t)(regval) << 16)) /*!< write value to RTC_ALRMTD_HR bit field */
#define GET_ALRMTD_HR(regval)              GET_BITS((regval),16,21)                   /*!< get value of RTC_ALRMTD_HR bit field */

#define ALRMTD_DAY(regval)                 (BITS(24,29) & ((uint32_t)(regval) << 24)) /*!< write value to RTC_ALRMTD_DAY bit field */
#define GET_ALRMTD_DAY(regval)             GET_BITS((regval),24,29)                   /*!< get value of RTC_ALRMTD_DAY bit field */

#define RTC_ALARM_NONE_MASK                ((uint32_t)0x00000000U)                    /*!< alarm none mask */
#define RTC_ALARM_DATE_MASK                RTC_ALRMXTD_MSKD                           /*!< alarm date mask */
#define RTC_ALARM_HOUR_MASK                RTC_ALRMXTD_MSKH                           /*!< alarm hour mask */
#define RTC_ALARM_MINUTE_MASK              RTC_ALRMXTD_MSKM                           /*!< alarm minute mask */
#define RTC_ALARM_SECOND_MASK              RTC_ALRMXTD_MSKS                           /*!< alarm second mask */
#define RTC_ALARM_ALL_MASK                 (RTC_ALRMXTD_MSKD|RTC_ALRMXTD_MSKH|RTC_ALRMXTD_MSKM|RTC_ALRMXTD_MSKS)   /*!< alarm all mask */

#define RTC_ALARM_DATE_SELECTED            ((uint32_t)0x00000000U)                    /*!< alarm date format selected */
#define RTC_ALARM_WEEKDAY_SELECTED         RTC_ALRMXTD_DOWS                           /*!< alarm weekday format selected */

/* wpk register value */
#define WPK_WPK(regval)                    (BITS(0,7) & ((uint32_t)(regval) << 0))    /*!< write value to RTC_WPK_WPK bit field */

/* ss register value */
#define SS_SSC(regval)                     (BITS(0,15) & ((uint32_t)(regval) << 0))   /*!< write value to RTC_SS_SSC bit field */

/* shiftctl register value */
#define SHIFTCTL_SFS(regval)               (BITS(0,14) & ((uint32_t)(regval) << 0))   /*!< write value to RTC_SHIFTCTL_SFS bit field */

#define RTC_SHIFT_ADD1S_RESET              ((uint32_t)0x00000000U)                    /*!< not add 1 second */
#define RTC_SHIFT_ADD1S_SET                RTC_SHIFTCTL_A1S                           /*!< add one second to the clock */

/* tts register value */
#define TTS_SC(regval)                     (BITS(0,6) & ((uint32_t)(regval) << 0))    /*!< write value to RTC_TTS_SC bit field */
#define GET_TTS_SC(regval)                 GET_BITS((regval),0,6)                     /*!< get value of RTC_TTS_SC bit field */

#define TTS_MN(regval)                     (BITS(8,14) & ((uint32_t)(regval) << 8))   /*!< write value to RTC_TTS_MN bit field */
#define GET_TTS_MN(regval)                 GET_BITS((regval),8,14)                    /*!< get value of RTC_TTS_MN bit field */

#define TTS_HR(regval)                     (BITS(16,21) & ((uint32_t)(regval) << 16)) /*!< write value to RTC_TTS_HR bit field */
#define GET_TTS_HR(regval)                 GET_BITS((regval),16,21)                   /*!< get value of RTC_TTS_HR bit field */

/* dts register value */
#define DTS_DAY(regval)                    (BITS(0,5) & ((uint32_t)(regval) << 0))    /*!< write value to RTC_DTS_DAY bit field */
#define GET_DTS_DAY(regval)                GET_BITS((regval),0,5)                     /*!< get value of RTC_DTS_DAY bit field */

#define DTS_MON(regval)                    (BITS(8,12) & ((uint32_t)(regval) << 8))   /*!< write value to RTC_DTS_MON bit field */
#define GET_DTS_MON(regval)                GET_BITS((regval),8,12)                    /*!< get value of RTC_DTS_MON bit field */

#define DTS_DOW(regval)                    (BITS(13,15) & ((uint32_t)(regval) << 13)) /*!< write value to RTC_DTS_DOW bit field */
#define GET_DTS_DOW(regval)                GET_BITS((regval),13,15)                   /*!< get value of RTC_DTS_DOW bit field */

/* ssts register value */
#define SSTS_SSC(regval)                   (BITS(0,15) & ((uint32_t)(regval) << 0))   /*!< write value to RTC_SSTS_SSC bit field */

/* hrfc register value */
#define HRFC_CMSK(regval)                  (BITS(0,8) & ((uint32_t)(regval) << 0))    /*!< write value to RTC_HRFC_CMSK bit field */

#define RTC_CALIBRATION_WINDOW_32S         ((uint32_t)0x00000000U)                    /*!< 2exp20 RTCCLK cycles, 32s if RTCCLK = 32768 Hz */
#define RTC_CALIBRATION_WINDOW_16S         RTC_HRFC_CWND16                            /*!< 2exp19 RTCCLK cycles, 16s if RTCCLK = 32768 Hz */
#define RTC_CALIBRATION_WINDOW_8S          RTC_HRFC_CWND8                             /*!< 2exp18 RTCCLK cycles, 8s if RTCCLK = 32768 Hz */

#define RTC_CALIBRATION_PLUS_SET           RTC_HRFC_FREQI                             /*!< increase RTC frequency by 488.5ppm */
#define RTC_CALIBRATION_PLUS_RESET         ((uint32_t)0x00000000U)                    /*!< no effect */

/* tamp register value */
#define TAMP_FREQ(regval)                  (BITS(8,10) & ((uint32_t)(regval) << 8))  /*!< write value to RTC_TAMP_FREQ bit field */
#define RTC_FREQ_DIV32768                  TAMP_FREQ(0)                               /*!< sample once every 32768 RTCCLK(1Hz if RTCCLK=32.768KHz) */
#define RTC_FREQ_DIV16384                  TAMP_FREQ(1)                               /*!< sample once every 16384 RTCCLK(2Hz if RTCCLK=32.768KHz) */
#define RTC_FREQ_DIV8192                   TAMP_FREQ(2)                               /*!< sample once every 8192 RTCCLK(4Hz if RTCCLK=32.768KHz) */
#define RTC_FREQ_DIV4096                   TAMP_FREQ(3)                               /*!< sample once every 4096 RTCCLK(8Hz if RTCCLK=32.768KHz) */
#define RTC_FREQ_DIV2048                   TAMP_FREQ(4)                               /*!< sample once every 2048 RTCCLK(16Hz if RTCCLK=32.768KHz) */
#define RTC_FREQ_DIV1024                   TAMP_FREQ(5)                               /*!< sample once every 1024 RTCCLK(32Hz if RTCCLK=32.768KHz) */
#define RTC_FREQ_DIV512                    TAMP_FREQ(6)                               /*!< sample once every 512 RTCCLK(64Hz if RTCCLK=32.768KHz) */
#define RTC_FREQ_DIV256                    TAMP_FREQ(7)                               /*!< sample once every 256 RTCCLK(128Hz if RTCCLK=32.768KHz) */

#define TAMP_FLT(regval)                   (BITS(11,12) & ((uint32_t)(regval) << 11)) /*!< write value to RTC_TAMP_FLT bit field */
#define RTC_FLT_EDGE                       TAMP_FLT(0)                                /*!< detecting tamper event using edge mode. precharge duration is disabled automatically */
#define RTC_FLT_2S                         TAMP_FLT(1)                                /*!< detecting tamper event using level mode.2 consecutive valid level samples will make a effective tamper event  */
#define RTC_FLT_4S                         TAMP_FLT(2)                                /*!< detecting tamper event using level mode.4 consecutive valid level samples will make an effective tamper event */
#define RTC_FLT_8S                         TAMP_FLT(3)                                /*!< detecting tamper event using level mode.8 consecutive valid level samples will make a effective tamper event  */

#define TAMP_PRCH(regval)                  (BITS(13,14) & ((uint32_t)(regval) << 13)) /*!< write value to RTC_TAMP_PRCH bit field */
#define RTC_PRCH_1C                        TAMP_PRCH(0)                               /*!< 1 RTC clock prechagre time before each sampling */
#define RTC_PRCH_2C                        TAMP_PRCH(1)                               /*!< 2 RTC clock prechagre time before each sampling  */
#define RTC_PRCH_4C                        TAMP_PRCH(2)                               /*!< 4 RTC clock prechagre time before each sampling */
#define RTC_PRCH_8C                        TAMP_PRCH(3)                               /*!< 8 RTC clock prechagre time before each sampling */

#define RTC_TAMPER0                        RTC_TAMP_TP0EN                             /*!< tamper 0 detection enable */
#define RTC_TAMPER1                        RTC_TAMP_TP1EN                             /*!< tamper 1 detection enable */

#define RTC_TAMPER_TRIGGER_EDGE_RISING     ((uint32_t)0x00000000U)                    /*!< tamper detection is in rising edge mode */
#define RTC_TAMPER_TRIGGER_EDGE_FALLING    RTC_TAMP_TP0EG                             /*!< tamper detection is in falling edge mode */
#define RTC_TAMPER_TRIGGER_LEVEL_LOW       ((uint32_t)0x00000000U)                    /*!< tamper detection is in low level mode */
#define RTC_TAMPER_TRIGGER_LEVEL_HIGH      RTC_TAMP_TP0EG                             /*!< tamper detection is in high level mode */

#define RTC_TAMPER_TRIGGER_POS             ((uint32_t)0x00000001U)                    /* shift position of trigger relative to source */

#define RTC_ALARM_OUTPUT_OD                ((uint32_t)0x00000000U)                    /*!< RTC alarm output open-drain mode */
#define RTC_ALARM_OUTPUT_PP                RTC_TAMP_AOT                               /*!< RTC alarm output push-pull mode */

/* ALRMXSS register value */
#define ALRMXSS_SSC(regval)                (BITS(0,14) & ((uint32_t)(regval)<< 0))    /*!< write value to RTC_ALRMXSS_SSC bit field */

#define ALRMXSS_MASKSSC(regval)            (BITS(24,27) & ((uint32_t)(regval) << 24)) /*!< write value to RTC_ALRMXSS_MASKSSC bit field */
#define RTC_MASKSSC_0_14                   ALRMXSS_MASKSSC(0)                         /*!< mask alarm subsecond configuration */
#define RTC_MASKSSC_1_14                   ALRMXSS_MASKSSC(1)                         /*!< mask RTC_ALRMXSS_SSC[14:1], and RTC_ALRMXSS_SSC[0] is to be compared */
#define RTC_MASKSSC_2_14                   ALRMXSS_MASKSSC(2)                         /*!< mask RTC_ALRMXSS_SSC[14:2], and RTC_ALRMXSS_SSC[1:0] is to be compared */
#define RTC_MASKSSC_3_14                   ALRMXSS_MASKSSC(3)                         /*!< mask RTC_ALRMXSS_SSC[14:3], and RTC_ALRMXSS_SSC[2:0] is to be compared */
#define RTC_MASKSSC_4_14                   ALRMXSS_MASKSSC(4)                         /*!< mask RTC_ALRMXSS_SSC[14:4]], and RTC_ALRMXSS_SSC[3:0] is to be compared */
#define RTC_MASKSSC_5_14                   ALRMXSS_MASKSSC(5)                         /*!< mask RTC_ALRMXSS_SSC[14:5], and RTC_ALRMXSS_SSC[4:0] is to be compared */
#define RTC_MASKSSC_6_14                   ALRMXSS_MASKSSC(6)                         /*!< mask RTC_ALRMXSS_SSC[14:6], and RTC_ALRMXSS_SSC[5:0] is to be compared */
#define RTC_MASKSSC_7_14                   ALRMXSS_MASKSSC(7)                         /*!< mask RTC_ALRMXSS_SSC[14:7], and RTC_ALRMXSS_SSC[6:0] is to be compared */
#define RTC_MASKSSC_8_14                   ALRMXSS_MASKSSC(8)                         /*!< mask RTC_ALRMXSS_SSC[14:7], and RTC_ALRMXSS_SSC[6:0] is to be compared */
#define RTC_MASKSSC_9_14                   ALRMXSS_MASKSSC(9)                         /*!< mask RTC_ALRMXSS_SSC[14:9], and RTC_ALRMXSS_SSC[8:0] is to be compared */
#define RTC_MASKSSC_10_14                  ALRMXSS_MASKSSC(10)                        /*!< mask RTC_ALRMXSS_SSC[14:10], and RTC_ALRMXSS_SSC[9:0] is to be compared */
#define RTC_MASKSSC_11_14                  ALRMXSS_MASKSSC(11)                        /*!< mask RTC_ALRMXSS_SSC[14:11], and RTC_ALRMXSS_SSC[10:0] is to be compared */
#define RTC_MASKSSC_12_14                  ALRMXSS_MASKSSC(12)                        /*!< mask RTC_ALRMXSS_SSC[14:12], and RTC_ALRMXSS_SSC[11:0] is to be compared */
#define RTC_MASKSSC_13_14                  ALRMXSS_MASKSSC(13)                        /*!< mask RTC_ALRMXSS_SSC[14:13], and RTC_ALRMXSS_SSC[12:0] is to be compared */
#define RTC_MASKSSC_14                     ALRMXSS_MASKSSC(14)                        /*!< mask RTC_ALRMXSS_SSC[14], and RTC_ALRMXSS_SSC[13:0] is to be compared */
#define RTC_MASKSSC_NONE                   ALRMXSS_MASKSSC(15)                        /*!< mask none, and RTC_ALRMXSS_SSC[14:0] is to be compared */

/* RTC interrupt source */
#define RTC_INT_TIMESTAMP                  RTC_CTL_TSIE                               /*!< time-stamp interrupt enable */
#define RTC_INT_ALARM0                     RTC_CTL_ALRM0IE                            /*!< RTC alarm0 interrupt enable */
#define RTC_INT_ALARM1                     RTC_CTL_ALRM1IE                            /*!< RTC alarm1 interrupt enable */
#define RTC_INT_TAMP                       RTC_TAMP_TPIE                              /*!< tamper detection interrupt enable */
#define RTC_INT_WAKEUP                     RTC_CTL_WTIE                               /*!< RTC wakeup timer interrupt enable */

/* write protect key */
#define RTC_UNLOCK_KEY1                    ((uint8_t)0xCAU)                           /*!< RTC unlock key1 */
#define RTC_UNLOCK_KEY2                    ((uint8_t)0x53U)                           /*!< RTC unlock key2 */
#define RTC_LOCK_KEY                       ((uint8_t)0xFFU)                           /*!< RTC lock key */

/* registers reset value */
#define RTC_REGISTER_RESET                 ((uint32_t)0x00000000U)                    /*!< RTC common register reset value */
#define RTC_DATE_RESET                     ((uint32_t)0x00002101U)                    /*!< RTC_DATE register reset value */
#define RTC_STAT_RESET                     ((uint32_t)0x00000000U)                    /*!< RTC_STAT register reset value */
#define RTC_PSC_RESET                      ((uint32_t)0x007F00FFU)                    /*!< RTC_PSC register reset value */
#define RTC_WUT_RESET                      ((uint32_t)0x0000FFFFU)                    /*!< RTC_WUT register reset value */

/* RTC alarm */
#define RTC_ALARM0                         ((uint8_t)0x01U)                           /*!< RTC alarm 0 */              
#define RTC_ALARM1                         ((uint8_t)0x02U)                           /*!< RTC alarm 1 */   

/* RTC coarse calibration direction */
#define CALIB_INCREASE                     ((uint8_t)0x01U)                           /*!< RTC coarse calibration increase */  
#define CALIB_DECREASE                     ((uint8_t)0x02U)                           /*!< RTC coarse calibration decrease */  

/* RTC wakeup timer clock */
#define CTL_WTCS(regval)                   (BITS(0,2) & ((regval)<< 0))
#define WAKEUP_RTCCK_DIV16                 CTL_WTCS(0)                                /*!< wakeup timer clock is RTC clock divided by 16 */
#define WAKEUP_RTCCK_DIV8                  CTL_WTCS(1)                                /*!< wakeup timer clock is RTC clock divided by 8 */
#define WAKEUP_RTCCK_DIV4                  CTL_WTCS(2)                                /*!< wakeup timer clock is RTC clock divided by 4 */
#define WAKEUP_RTCCK_DIV2                  CTL_WTCS(3)                                /*!< wakeup timer clock is RTC clock divided by 2 */
#define WAKEUP_CKSPRE                      CTL_WTCS(4)                                /*!< wakeup timer clock is ckapre */
#define WAKEUP_CKSPRE_2EXP16               CTL_WTCS(6)                                /*!< wakeup timer clock is ckapre and wakeup timer add 2exp16 */
 
/* RTC_AF pin */
#define RTC_AF0_TIMESTAMP                  ((uint32_t)0x00000000)                     /*!< RTC_AF0 use for timestamp */
#define RTC_AF1_TIMESTAMP                  RTC_TAMP_TSSEL                             /*!< RTC_AF1 use for timestamp */
#define RTC_AF0_TAMPER0                    ((uint32_t)0x00000000)                     /*!< RTC_AF0 use for tamper0 */
#define RTC_AF1_TAMPER0                    RTC_TAMP_TP0SEL                            /*!< RTC_AF1 use for tamper0 */

/* RTC flags */
#define RTC_FLAG_ALRM0W                    RTC_STAT_ALRM0WF                           /*!< alarm0 configuration can be write flag */
#define RTC_FLAG_ALRM1W                    RTC_STAT_ALRM1WF                           /*!< alarm1 configuration can be write flag */
#define RTC_FLAG_WTW                       RTC_STAT_WTWF                              /*!< wakeup timer can be write flag */
#define RTC_FLAG_SOP                       RTC_STAT_SOPF                              /*!< shift function operation pending flag */
#define RTC_FLAG_YCM                       RTC_STAT_YCM                               /*!< year configuration mark status flag */
#define RTC_FLAG_RSYN                      RTC_STAT_RSYNF                             /*!< register synchronization flag */
#define RTC_FLAG_INIT                      RTC_STAT_INITF                             /*!< initialization state flag */
#define RTC_FLAG_ALRM0                     RTC_STAT_ALRM0F                            /*!< alarm0 occurs flag */
#define RTC_FLAG_ALRM1                     RTC_STAT_ALRM1F                            /*!< alarm1 occurs flag */
#define RTC_FLAG_WT                        RTC_STAT_WTF                               /*!< wakeup timer occurs flag */
#define RTC_FLAG_TS                        RTC_STAT_TSF                               /*!< time-stamp flag */
#define RTC_FLAG_TSOVR                     RTC_STAT_TSOVRF                            /*!< time-stamp overflow flag */
#define RTC_FLAG_TP0                       RTC_STAT_TP0F                              /*!< RTC tamper 0 detected flag */
#define RTC_FLAG_TP1                       RTC_STAT_TP1F                              /*!< RTC tamper 1 detected flag */
#define RTC_STAT_SCP                       RTC_STAT_SCPF                              /*!< smooth calibration pending flag */

/* function declarations */
/* reset most of the RTC registers */
ErrStatus rtc_deinit(void);
/* initialize RTC registers */
ErrStatus rtc_init(rtc_parameter_struct* rtc_initpara_struct);
/* enter RTC init mode */
ErrStatus rtc_init_mode_enter(void);
/* exit RTC init mode */
void rtc_init_mode_exit(void);
/* wait until RTC_TIME and RTC_DATE registers are synchronized with APB clock, and the shadow registers are updated */
ErrStatus rtc_register_sync_wait(void);

/* get current time and date */
void rtc_current_time_get(rtc_parameter_struct* rtc_initpara_struct);
/* get current subsecond value */
uint32_t rtc_subsecond_get(void);

/* configure RTC alarm */
void rtc_alarm_config(uint8_t rtc_alarm, rtc_alarm_struct* rtc_alarm_time);
/* configure subsecond of RTC alarm */
void rtc_alarm_subsecond_config(uint8_t rtc_alarm, uint32_t mask_subsecond, uint32_t subsecond);
/* get RTC alarm */
void rtc_alarm_get(uint8_t rtc_alarm,rtc_alarm_struct* rtc_alarm_time);
/* get RTC alarm subsecond */
uint32_t rtc_alarm_subsecond_get(uint8_t rtc_alarm);
/* enable RTC alarm */
void rtc_alarm_enable(uint8_t rtc_alarm);
/* disable RTC alarm */
ErrStatus rtc_alarm_disable(uint8_t rtc_alarm);

/* enable RTC time-stamp */
void rtc_timestamp_enable(uint32_t edge);
/* disable RTC time-stamp */
void rtc_timestamp_disable(void);
/* get RTC timestamp time and date */
void rtc_timestamp_get(rtc_timestamp_struct* rtc_timestamp);
/* get RTC time-stamp subsecond */
uint32_t rtc_timestamp_subsecond_get(void);
/* RTC time-stamp pin map */
void rtc_timestamp_pin_map(uint32_t rtc_af);

/* enable RTC tamper */
void rtc_tamper_enable(rtc_tamper_struct* rtc_tamper);
/* disable RTC tamper */
void rtc_tamper_disable(uint32_t source);
/* RTC tamper0 pin map */
void rtc_tamper0_pin_map(uint32_t rtc_af);

/* enable specified RTC interrupt */
void rtc_interrupt_enable(uint32_t interrupt);
/* disble specified RTC interrupt */
void rtc_interrupt_disable(uint32_t interrupt);
/* check specified flag */
FlagStatus rtc_flag_get(uint32_t flag);
/* clear specified flag */
void rtc_flag_clear(uint32_t flag);

/* configure RTC alarm output source */
void rtc_alarm_output_config(uint32_t source, uint32_t mode);
/* configure RTC calibration output source */
void rtc_calibration_output_config(uint32_t source);

/* adjust the daylight saving time by adding or substracting one hour from the current time */
void rtc_hour_adjust(uint32_t operation);
/* adjust RTC second or subsecond value of current time */
ErrStatus rtc_second_adjust(uint32_t add, uint32_t minus);

/* enable RTC bypass shadow registers function */
void rtc_bypass_shadow_enable(void);
/* disable RTC bypass shadow registers function */
void rtc_bypass_shadow_disable(void);

/* enable RTC reference clock detection function */
ErrStatus rtc_refclock_detection_enable(void);
/* disable RTC reference clock detection function */
ErrStatus rtc_refclock_detection_disable(void);

/* enable RTC wakeup timer */
void rtc_wakeup_enable(void);
/* disable RTC wakeup timer */
ErrStatus rtc_wakeup_disable(void);
/* set auto wakeup timer clock */
ErrStatus rtc_wakeup_clock_set(uint8_t wakeup_clock);
/* set auto wakeup timer value */
ErrStatus rtc_wakeup_timer_set(uint16_t wakeup_timer);
/* get auto wakeup timer value */
uint16_t rtc_wakeup_timer_get(void);

/* configure RTC smooth calibration */
ErrStatus rtc_smooth_calibration_config(uint32_t window, uint32_t plus, uint32_t minus);
/* enable RTC coarse calibration */
ErrStatus rtc_coarse_calibration_enable(void);
/* disable RTC coarse calibration */
ErrStatus rtc_coarse_calibration_disable(void);
/* configure RTC coarse calibration direction and step */
ErrStatus rtc_coarse_calibration_config(uint8_t direction, uint8_t step);

#endif /* GD32F4XX_RTC_H */
