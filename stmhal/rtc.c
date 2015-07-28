/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>

#include STM32_HAL_H

#include "py/runtime.h"
#include "rtc.h"

/// \moduleref pyb
/// \class RTC - real time clock
///
/// The RTC is and independent clock that keeps track of the date
/// and time.
///
/// Example usage:
///
///     rtc = pyb.RTC()
///     rtc.datetime((2014, 5, 1, 4, 13, 0, 0, 0))
///     print(rtc.datetime())

RTC_HandleTypeDef RTCHandle;

// rtc_info indicates various things about RTC startup
// it's a bit of a hack at the moment
static mp_uint_t rtc_info;

// Note: LSI is around (32KHz), these dividers should work either way
// ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)
#define RTC_ASYNCH_PREDIV (0x7f)
#define RTC_SYNCH_PREDIV  (0x00ff)

#if 0
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
    mp_uint_t sys_tick = sys_tick_counter;
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
#endif

STATIC void RTC_CalendarConfig(void);

void rtc_init(void) {
    RTCHandle.Instance = RTC;

    /* Configure RTC prescaler and RTC data registers */
    /* RTC configured as follow:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
    RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    mp_uint_t tick = HAL_GetTick();

    if (HAL_RTC_Init(&RTCHandle) != HAL_OK) {
        // init error
        rtc_info = 0xffff; // indicate error
        return;
    }

    // record how long it took for the RTC to start up
    rtc_info = HAL_GetTick() - tick;

    // check data stored in BackUp register0
    if (HAL_RTCEx_BKUPRead(&RTCHandle, RTC_BKP_DR0) != 0x32f2) {
        // fresh reset; configure RTC Calendar
        RTC_CalendarConfig();
    } else {
        // RTC was previously set, so leave it alone
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET) {
            // power on reset occurred
            rtc_info |= 0x10000;
        }
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
            // external reset occurred
            rtc_info |= 0x20000;
        }
        // Clear source Reset Flag
        __HAL_RCC_CLEAR_RESET_FLAGS();
    }
}

STATIC void RTC_CalendarConfig(void) {
    // set the date to 1st Jan 2014
    RTC_DateTypeDef date;
    date.Year = 14;
    date.Month = 1;
    date.Date = 1;
    date.WeekDay = RTC_WEEKDAY_WEDNESDAY;

    if(HAL_RTC_SetDate(&RTCHandle, &date, FORMAT_BIN) != HAL_OK) {
        // init error
        return;
    }

    // set the time to 00:00:00
    RTC_TimeTypeDef time;
    time.Hours = 0;
    time.Minutes = 0;
    time.Seconds = 0;
    time.TimeFormat = RTC_HOURFORMAT12_AM;
    time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    time.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&RTCHandle, &time, FORMAT_BIN) != HAL_OK) {
        // init error
        return;
    }

    // write data to indicate the RTC has been set
    HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, 0x32f2);
}

/*
  Note: Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  the RTC clock source; in this case the Backup domain will be reset in
  order to modify the RTC Clock source, as consequence RTC registers (including
  the backup registers) and RCC_BDCR register are set to their reset values.
*/
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc) {
    RCC_OscInitTypeDef        RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

    /* To change the source clock of the RTC feature (LSE, LSI), You have to:
       - Enable the power clock using __PWR_CLK_ENABLE()
       - Enable write access using HAL_PWR_EnableBkUpAccess() function before to
         configure the RTC clock source (to be done once after reset).
       - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and
         __HAL_RCC_BACKUPRESET_RELEASE().
       - Configure the needed RTc clock source */

    // RTC clock source uses LSE (external crystal) only if relevant
    // configuration variable is set.  Otherwise it uses LSI (internal osc).

    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    #if defined(MICROPY_HW_RTC_USE_LSE) && MICROPY_HW_RTC_USE_LSE
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    #else
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    #endif
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        //Error_Handler();
        return;
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    #if defined(MICROPY_HW_RTC_USE_LSE) && MICROPY_HW_RTC_USE_LSE
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    #else
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    #endif
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        //Error_Handler();
        return;
    }

    // enable RTC peripheral clock
    __HAL_RCC_RTC_ENABLE();
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc) {
    __HAL_RCC_RTC_DISABLE();
}

/******************************************************************************/
// Micro Python bindings

typedef struct _pyb_rtc_obj_t {
    mp_obj_base_t base;
} pyb_rtc_obj_t;

STATIC const pyb_rtc_obj_t pyb_rtc_obj = {{&pyb_rtc_type}};

/// \classmethod \constructor()
/// Create an RTC object.
STATIC mp_obj_t pyb_rtc_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return (mp_obj_t)&pyb_rtc_obj;
}

/// \method info()
/// Get information about the startup time and reset source.
///
///  - The lower 0xffff are the number of milliseconds the RTC took to
///    start up.
///  - Bit 0x10000 is set if a power-on reset occurred.
///  - Bit 0x20000 is set if an external reset occurred
mp_obj_t pyb_rtc_info(mp_obj_t self_in) {
    return mp_obj_new_int(rtc_info);
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_rtc_info_obj, pyb_rtc_info);

/// \method datetime([datetimetuple])
/// Get or set the date and time of the RTC.
///
/// With no arguments, this method returns an 8-tuple with the current
/// date and time.  With 1 argument (being an 8-tuple) it sets the date
/// and time.
///
/// The 8-tuple has the following format:
///
///     (year, month, day, weekday, hours, minutes, seconds, subseconds)
///
/// `weekday` is 1-7 for Monday through Sunday.
///
/// `subseconds` counts down from 255 to 0
mp_obj_t pyb_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        // get date and time
        // note: need to call get time then get date to correctly access the registers
        RTC_DateTypeDef date;
        RTC_TimeTypeDef time;
        HAL_RTC_GetTime(&RTCHandle, &time, FORMAT_BIN);
        HAL_RTC_GetDate(&RTCHandle, &date, FORMAT_BIN);
        mp_obj_t tuple[8] = {
            mp_obj_new_int(2000 + date.Year),
            mp_obj_new_int(date.Month),
            mp_obj_new_int(date.Date),
            mp_obj_new_int(date.WeekDay),
            mp_obj_new_int(time.Hours),
            mp_obj_new_int(time.Minutes),
            mp_obj_new_int(time.Seconds),
            mp_obj_new_int(time.SubSeconds),
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        // set date and time
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 8, &items);

        RTC_DateTypeDef date;
        date.Year = mp_obj_get_int(items[0]) - 2000;
        date.Month = mp_obj_get_int(items[1]);
        date.Date = mp_obj_get_int(items[2]);
        date.WeekDay = mp_obj_get_int(items[3]);
        HAL_RTC_SetDate(&RTCHandle, &date, FORMAT_BIN);

        RTC_TimeTypeDef time;
        time.Hours = mp_obj_get_int(items[4]);
        time.Minutes = mp_obj_get_int(items[5]);
        time.Seconds = mp_obj_get_int(items[6]);
        time.SubSeconds = mp_obj_get_int(items[7]);
        time.TimeFormat = RTC_HOURFORMAT12_AM;
        time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        time.StoreOperation = RTC_STOREOPERATION_SET;
        HAL_RTC_SetTime(&RTCHandle, &time, FORMAT_BIN);

        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_datetime_obj, 1, 2, pyb_rtc_datetime);

// wakeup(None)
// wakeup(ms, callback=None)
// wakeup(wucksel, wut, callback)
mp_obj_t pyb_rtc_wakeup(mp_uint_t n_args, const mp_obj_t *args) {
    // wut is wakeup counter start value, wucksel is clock source
    // counter is decremented at wucksel rate, and wakes the MCU when it gets to 0
    // wucksel=0b000 is RTC/16 (RTC runs at 32768Hz)
    // wucksel=0b001 is RTC/8
    // wucksel=0b010 is RTC/4
    // wucksel=0b011 is RTC/2
    // wucksel=0b100 is 1Hz clock
    // wucksel=0b110 is 1Hz clock with 0x10000 added to wut
    // so a 1 second wakeup could be wut=2047, wucksel=0b000, or wut=4095, wucksel=0b001, etc

    // disable wakeup IRQ while we configure it
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);

    bool enable = false;
    mp_int_t wucksel;
    mp_int_t wut;
    mp_obj_t callback = mp_const_none;
    if (n_args <= 3) {
        if (args[1] == mp_const_none) {
            // disable wakeup
        } else {
            // time given in ms
            mp_int_t ms = mp_obj_get_int(args[1]);
            mp_int_t div = 2;
            wucksel = 3;
            while (div <= 16 && ms > 2000 * div) {
                div *= 2;
                wucksel -= 1;
            }
            if (div <= 16) {
                wut = 32768 / div * ms / 1000;
            } else {
                wucksel = 4;
                wut = ms / 1000;
                if (ms > 0x10000) {
                    wucksel = 5;
                    ms -= 0x10000;
                    if (ms > 0x10000) {
                        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "wakeup value too large"));
                    }
                }
            }
            wut -= 1;
            enable = true;
        }
        if (n_args == 3) {
            callback = args[2];
        }
    } else {
        // config values given directly
        wucksel = mp_obj_get_int(args[1]);
        wut = mp_obj_get_int(args[2]);
        callback = args[3];
        enable = true;
    }

    // set the callback
    MP_STATE_PORT(pyb_extint_callback)[22] = callback;

    // disable register write protection
    RTC->WPR = 0xca;
    RTC->WPR = 0x53;

    // clear WUTE
    RTC->CR &= ~(1 << 10);

    // wait until WUTWF is set
    while (!(RTC->ISR & (1 << 2))) {
    }

    if (enable) {
        // program WUT
        RTC->WUTR = wut;

        // set WUTIE to enable wakeup interrupts
        // set WUTE to enable wakeup
        // program WUCKSEL
        RTC->CR |= (1 << 14) | (1 << 10) | (wucksel & 7);

        // enable register write protection
        RTC->WPR = 0xff;

        // enable external interrupts on line 22
        EXTI->IMR |= 1 << 22;
        EXTI->RTSR |= 1 << 22;

        // clear interrupt flags
        RTC->ISR &= ~(1 << 10);
        EXTI->PR = 1 << 22;

        HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0f, 0x0f);
        HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

        //printf("wut=%d wucksel=%d\n", wut, wucksel);
    } else {
        // clear WUTIE to disable interrupts
        RTC->CR &= ~(1 << 14);

        // enable register write protection
        RTC->WPR = 0xff;

        // disable external interrupts on line 22
        EXTI->IMR &= ~(1 << 22);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_wakeup_obj, 2, 4, pyb_rtc_wakeup);

// calibration(None)
// calibration(cal)
// When an integer argument is provided, check that it falls in the range [-511 to 512]
// and set the calibration value; otherwise return calibration value
mp_obj_t pyb_rtc_calibration(mp_uint_t n_args, const mp_obj_t *args) {
    mp_int_t cal;
    if (n_args == 2) {
	cal = mp_obj_get_int(args[1]);
	mp_uint_t cal_p, cal_m;
	if (cal < -511 || cal > 512) {
	    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
					       "calibration value out of range"));
	}
	if (cal > 0) {
	    cal_p = RTC_SMOOTHCALIB_PLUSPULSES_SET;
	    cal_m = 512 - cal;
	} else {
	    cal_p = RTC_SMOOTHCALIB_PLUSPULSES_RESET;
	    cal_m = -cal;
	}
	HAL_RTCEx_SetSmoothCalib(&RTCHandle, RTC_SMOOTHCALIB_PERIOD_32SEC, cal_p, cal_m);
	return mp_const_none;
    } else {
        // printf("CALR = 0x%x\n", (mp_uint_t) RTCHandle.Instance->CALR); // DEBUG
	// Test if CALP bit is set in CALR:
	if (RTCHandle.Instance->CALR & 0x8000) {
	    cal = 512 - (RTCHandle.Instance->CALR & 0x1ff);
	} else {
	    cal = -(RTCHandle.Instance->CALR & 0x1ff);
	}
	return mp_obj_new_int(cal);
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_calibration_obj, 1, 2, pyb_rtc_calibration);
    
STATIC const mp_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_info), (mp_obj_t)&pyb_rtc_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_datetime), (mp_obj_t)&pyb_rtc_datetime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_wakeup), (mp_obj_t)&pyb_rtc_wakeup_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_calibration), (mp_obj_t)&pyb_rtc_calibration_obj },
};
STATIC MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);

const mp_obj_type_t pyb_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = pyb_rtc_make_new,
    .locals_dict = (mp_obj_t)&pyb_rtc_locals_dict,
};
