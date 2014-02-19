#include <stdio.h>
#include <stm32f4xx.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "systick.h"
#include "rtc.h"

machine_uint_t rtc_info;

#define RTC_INFO_USE_EXISTING (0)
#define RTC_INFO_USE_LSE (1)
#define RTC_INFO_USE_LSI (3)

void rtc_init(void) {
    // Enable the PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Allow access to RTC
    PWR_BackupAccessCmd(ENABLE);

    if (RTC_ReadBackupRegister(RTC_BKP_DR0) == 0x32F2) {
        // RTC still alive, so don't re-init it
        // wait for RTC APB register synchronisation
        RTC_WaitForSynchro();
        rtc_info = RTC_INFO_USE_EXISTING;
        return;
    }

    uint32_t timeout = 10000000;

    // Enable the PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    // Allow access to RTC
    PWR_BackupAccessCmd(ENABLE);

    // Enable the LSE OSC
    RCC_LSEConfig(RCC_LSE_ON);

    // Wait till LSE is ready
    machine_uint_t sys_tick = sys_tick_counter;
    while((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (--timeout > 0)) {
    }

    // record how long it took for the RTC to start up
    rtc_info = (sys_tick_counter - sys_tick) << 2;

    // If LSE timed out, use LSI instead
    if (timeout == 0) {
        // Disable the LSE OSC
        RCC_LSEConfig(RCC_LSE_OFF);

        // Enable the LSI OSC
        RCC_LSICmd(ENABLE);

        // Wait till LSI is ready
        while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
        }

        // Use LSI as the RTC Clock Source
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

        // record that we are using the LSI
        rtc_info |= RTC_INFO_USE_LSI;
    } else {
        // Use LSE as the RTC Clock Source
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        // record that we are using the LSE
        rtc_info |= RTC_INFO_USE_LSE;
    }

    // Note: LSI is around (32KHz), these dividers should work either way
    // ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
    uint32_t uwSynchPrediv = 0xFF;
    uint32_t uwAsynchPrediv = 0x7F;

    // Enable the RTC Clock
    RCC_RTCCLKCmd(ENABLE);

    // Wait for RTC APB registers synchronisation
    RTC_WaitForSynchro();

    // Configure the RTC data register and RTC prescaler
    RTC_InitTypeDef RTC_InitStructure;
    RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    // Set the date (BCD)
    RTC_DateTypeDef RTC_DateStructure;
    RTC_DateStructure.RTC_Year = 0x13;
    RTC_DateStructure.RTC_Month = RTC_Month_October;
    RTC_DateStructure.RTC_Date = 0x26;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

    // Set the time (BCD)
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x01;
    RTC_TimeStructure.RTC_Minutes = 0x53;
    RTC_TimeStructure.RTC_Seconds = 0x00;
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

    // Indicator for the RTC configuration
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
}

/******************************************************************************/
// Micro Python bindings

mp_obj_t pyb_rtc_info(void) {
    return mp_obj_new_int(rtc_info);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_rtc_info_obj, pyb_rtc_info);

mp_obj_t pyb_rtc_read(void) {
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    printf("%02d:%02d:%02d\n", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_rtc_read_obj, pyb_rtc_read);
