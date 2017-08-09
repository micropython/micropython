/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/runtime.h"
#include "rtc.h"
#include "irq.h"

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
// modify RTC_ASYNCH_PREDIV & RTC_SYNCH_PREDIV in board/<BN>/mpconfigport.h to change sub-second ticks
// default is 3906.25 µs, min is ~30.52 µs (will increas Ivbat by ~500nA)
#ifndef RTC_ASYNCH_PREDIV
#define RTC_ASYNCH_PREDIV (0x7f)
#endif
#ifndef RTC_SYNCH_PREDIV
#define RTC_SYNCH_PREDIV  (0x00ff)
#endif

STATIC HAL_StatusTypeDef PYB_RTC_Init(RTC_HandleTypeDef *hrtc);
STATIC void PYB_RTC_MspInit_Kick(RTC_HandleTypeDef *hrtc, bool rtc_use_lse);
STATIC HAL_StatusTypeDef PYB_RTC_MspInit_Finalise(RTC_HandleTypeDef *hrtc);
STATIC void RTC_CalendarConfig(void);

#if defined(MICROPY_HW_RTC_USE_LSE) && MICROPY_HW_RTC_USE_LSE
STATIC bool rtc_use_lse = true;
#else
STATIC bool rtc_use_lse = false;
#endif
STATIC uint32_t rtc_startup_tick;
STATIC bool rtc_need_init_finalise = false;

// check if LSE exists
// not well tested, should probably be removed
STATIC bool lse_magic(void) {
#if 0
    uint32_t mode_in = GPIOC->MODER & 0x3fffffff;
    uint32_t mode_out = mode_in | 0x40000000;
    GPIOC->MODER = mode_out;
    GPIOC->OTYPER &= 0x7fff;
    GPIOC->BSRRH = 0x8000;
    GPIOC->OSPEEDR &= 0x3fffffff;
    GPIOC->PUPDR &= 0x3fffffff;
    int i = 0xff0;
    __IO int d = 0;
    uint32_t tc = 0;
    __IO uint32_t j;
    while (i) {
        GPIOC->MODER = mode_out;
        GPIOC->MODER = mode_in;
        for (j = 0; j < d; j++) ;
        i--;
        if ((GPIOC->IDR & 0x8000) == 0) {
            tc++;
        }
    }
    return (tc < 0xff0)?true:false;
#else
    return false;
#endif
}

void rtc_init_start(bool force_init) {
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

    rtc_need_init_finalise = false;

    if (!force_init) {
        if ((RCC->BDCR & (RCC_BDCR_LSEON | RCC_BDCR_LSERDY)) == (RCC_BDCR_LSEON | RCC_BDCR_LSERDY)) {
            // LSE is enabled & ready --> no need to (re-)init RTC
            // remove Backup Domain write protection
            HAL_PWR_EnableBkUpAccess();
            // Clear source Reset Flag
            __HAL_RCC_CLEAR_RESET_FLAGS();
            // provide some status information
            rtc_info |= 0x40000 | (RCC->BDCR & 7) | (RCC->CSR & 3) << 8;
            return;
        } else if (((RCC->BDCR & RCC_BDCR_RTCSEL) == RCC_BDCR_RTCSEL_1) && ((RCC->CSR & 3) == 3)) {
            // LSI configured & enabled & ready --> no need to (re-)init RTC
            // remove Backup Domain write protection
            HAL_PWR_EnableBkUpAccess();
            // Clear source Reset Flag
            __HAL_RCC_CLEAR_RESET_FLAGS();
            RCC->CSR |= 1;
            // provide some status information
            rtc_info |= 0x80000 | (RCC->BDCR & 7) | (RCC->CSR & 3) << 8;
            return;
        }
    }

    rtc_startup_tick = HAL_GetTick();
    rtc_info = 0x3f000000 | (rtc_startup_tick & 0xffffff);
    if (rtc_use_lse) {
        if (lse_magic()) {
            // don't even try LSE
            rtc_use_lse = false;
            rtc_info &= ~0x01000000;
        }
    }
    PYB_RTC_MspInit_Kick(&RTCHandle, rtc_use_lse);
}

void rtc_init_finalise() {
    if (!rtc_need_init_finalise) {
        return;
    }

    rtc_info = 0x20000000 | (rtc_use_lse << 28);
    if (PYB_RTC_Init(&RTCHandle) != HAL_OK) {
        if (rtc_use_lse) {
            // fall back to LSI...
            rtc_use_lse = false;
            rtc_startup_tick = HAL_GetTick();
            PYB_RTC_MspInit_Kick(&RTCHandle, rtc_use_lse);
            HAL_PWR_EnableBkUpAccess();
            RTCHandle.State = HAL_RTC_STATE_RESET;
            if (PYB_RTC_Init(&RTCHandle) != HAL_OK) {
                rtc_info = 0x0100ffff; // indicate error
                return;
            }
        } else {
            // init error
            rtc_info = 0xffff; // indicate error
            return;
        }
    }

    // record how long it took for the RTC to start up
    rtc_info |= (HAL_GetTick() - rtc_startup_tick) & 0xffff;

    // fresh reset; configure RTC Calendar
    RTC_CalendarConfig();
    #if defined(MCU_SERIES_L4)
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET) {
    #else
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET) {
    #endif
        // power on reset occurred
        rtc_info |= 0x10000;
    }
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
        // external reset occurred
        rtc_info |= 0x20000;
    }
    // Clear source Reset Flag
    __HAL_RCC_CLEAR_RESET_FLAGS();
    rtc_need_init_finalise = false;
}

STATIC HAL_StatusTypeDef PYB_RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct) {
    /*------------------------------ LSI Configuration -------------------------*/
    if ((RCC_OscInitStruct->OscillatorType & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI) {
        // Check the LSI State
        if (RCC_OscInitStruct->LSIState != RCC_LSI_OFF) {
            // Enable the Internal Low Speed oscillator (LSI).
            __HAL_RCC_LSI_ENABLE();
        } else {
            // Disable the Internal Low Speed oscillator (LSI).
            __HAL_RCC_LSI_DISABLE();
        }
    }

    /*------------------------------ LSE Configuration -------------------------*/
    if ((RCC_OscInitStruct->OscillatorType & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE) {
        // Enable Power Clock
        __PWR_CLK_ENABLE();
        HAL_PWR_EnableBkUpAccess();
        uint32_t tickstart = HAL_GetTick();

        #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_L4)
        //__HAL_RCC_PWR_CLK_ENABLE();
        // Enable write access to Backup domain
        //PWR->CR1 |= PWR_CR1_DBP;
        // Wait for Backup domain Write protection disable
        while ((PWR->CR1 & PWR_CR1_DBP) == RESET) {
            if (HAL_GetTick() - tickstart > RCC_DBP_TIMEOUT_VALUE) {
                return HAL_TIMEOUT;
            }
        }
        #else
        // Enable write access to Backup domain
        //PWR->CR |= PWR_CR_DBP;
        // Wait for Backup domain Write protection disable
        while ((PWR->CR & PWR_CR_DBP) == RESET) {
            if (HAL_GetTick() - tickstart > DBP_TIMEOUT_VALUE) {
                return HAL_TIMEOUT;
            }
        }
        #endif

        // Set the new LSE configuration
        __HAL_RCC_LSE_CONFIG(RCC_OscInitStruct->LSEState);
    }

    return HAL_OK;
}

STATIC HAL_StatusTypeDef PYB_RTC_Init(RTC_HandleTypeDef *hrtc) {
    // Check the RTC peripheral state
    if (hrtc == NULL) {
        return HAL_ERROR;
    }
    if (hrtc->State == HAL_RTC_STATE_RESET) {
        // Allocate lock resource and initialize it
        hrtc->Lock = HAL_UNLOCKED;
        // Initialize RTC MSP
        if (PYB_RTC_MspInit_Finalise(hrtc) != HAL_OK) {
            return HAL_ERROR;
        }
    }

    // Set RTC state
    hrtc->State = HAL_RTC_STATE_BUSY;

    // Disable the write protection for RTC registers
    __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

    // Set Initialization mode
    if (RTC_EnterInitMode(hrtc) != HAL_OK) {
        // Enable the write protection for RTC registers
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        // Set RTC state
        hrtc->State = HAL_RTC_STATE_ERROR;

        return HAL_ERROR;
    } else {
        // Clear RTC_CR FMT, OSEL and POL Bits
        hrtc->Instance->CR &= ((uint32_t)~(RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL));
        // Set RTC_CR register
        hrtc->Instance->CR |= (uint32_t)(hrtc->Init.HourFormat | hrtc->Init.OutPut | hrtc->Init.OutPutPolarity);

        // Configure the RTC PRER
        hrtc->Instance->PRER = (uint32_t)(hrtc->Init.SynchPrediv);
        hrtc->Instance->PRER |= (uint32_t)(hrtc->Init.AsynchPrediv << 16);

        // Exit Initialization mode
        hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT;

        #if defined(MCU_SERIES_L4)
        hrtc->Instance->OR &= (uint32_t)~RTC_OR_ALARMOUTTYPE;
        hrtc->Instance->OR |= (uint32_t)(hrtc->Init.OutPutType);
        #elif defined(MCU_SERIES_F7)
        hrtc->Instance->OR &= (uint32_t)~RTC_OR_ALARMTYPE;
        hrtc->Instance->OR |= (uint32_t)(hrtc->Init.OutPutType);
        #else
        hrtc->Instance->TAFCR &= (uint32_t)~RTC_TAFCR_ALARMOUTTYPE;
        hrtc->Instance->TAFCR |= (uint32_t)(hrtc->Init.OutPutType);
        #endif

        // Enable the write protection for RTC registers
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        // Set RTC state
        hrtc->State = HAL_RTC_STATE_READY;

        return HAL_OK;
    }
}

STATIC void PYB_RTC_MspInit_Kick(RTC_HandleTypeDef *hrtc, bool rtc_use_lse) {
    /* To change the source clock of the RTC feature (LSE, LSI), You have to:
       - Enable the power clock using __PWR_CLK_ENABLE()
       - Enable write access using HAL_PWR_EnableBkUpAccess() function before to
         configure the RTC clock source (to be done once after reset).
       - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and
         __HAL_RCC_BACKUPRESET_RELEASE().
       - Configure the needed RTc clock source */

    // RTC clock source uses LSE (external crystal) only if relevant
    // configuration variable is set.  Otherwise it uses LSI (internal osc).

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (rtc_use_lse) {
        RCC_OscInitStruct.LSEState = RCC_LSE_ON;
        RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    } else {
        RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
        RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    }
    PYB_RCC_OscConfig(&RCC_OscInitStruct);

    // now ramp up osc. in background and flag calendear init needed
    rtc_need_init_finalise = true;
}

#define PYB_LSE_TIMEOUT_VALUE 1000  // ST docs spec 2000 ms LSE startup, seems to be too pessimistic
#define PYB_LSI_TIMEOUT_VALUE 500   // this is way too pessimistic, typ. < 1ms

STATIC HAL_StatusTypeDef PYB_RTC_MspInit_Finalise(RTC_HandleTypeDef *hrtc) {
    // we already had a kick so now wait for the corresponding ready state...
    if (rtc_use_lse) {
        // we now have to wait for LSE ready or timeout
        uint32_t tickstart = rtc_startup_tick;
        while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET) {
            if ((HAL_GetTick() - tickstart ) > PYB_LSE_TIMEOUT_VALUE) {
                return HAL_TIMEOUT;
            }
        }
    } else {
        // we now have to wait for LSI ready or timeout
        uint32_t tickstart = rtc_startup_tick;
        while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET) {
            if ((HAL_GetTick() - tickstart ) > PYB_LSI_TIMEOUT_VALUE) {
                return HAL_TIMEOUT;
            }
        }
    }

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    if (rtc_use_lse) {
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    } else {
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    }
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        //Error_Handler();
        return HAL_ERROR;
    }

    // enable RTC peripheral clock
    __HAL_RCC_RTC_ENABLE();
    return HAL_OK;
}

STATIC void RTC_CalendarConfig(void) {
    // set the date to 1st Jan 2015
    RTC_DateTypeDef date;
    date.Year = 15;
    date.Month = 1;
    date.Date = 1;
    date.WeekDay = RTC_WEEKDAY_THURSDAY;

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
}

/******************************************************************************/
// MicroPython bindings

typedef struct _pyb_rtc_obj_t {
    mp_obj_base_t base;
} pyb_rtc_obj_t;

STATIC const pyb_rtc_obj_t pyb_rtc_obj = {{&pyb_rtc_type}};

/// \classmethod \constructor()
/// Create an RTC object.
STATIC mp_obj_t pyb_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return constant object
    return (mp_obj_t)&pyb_rtc_obj;
}

// force rtc to re-initialise
mp_obj_t pyb_rtc_init(mp_obj_t self_in) {
    rtc_init_start(true);
    rtc_init_finalise();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_rtc_init_obj, pyb_rtc_init);

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

#define MEG_DIV_64 (1000000 / 64)
#define MEG_DIV_SCALE ((RTC_SYNCH_PREDIV + 1) / 64)

#if defined(MICROPY_HW_RTC_USE_US) && MICROPY_HW_RTC_USE_US
uint32_t rtc_subsec_to_us(uint32_t ss) {
    return ((RTC_SYNCH_PREDIV - ss) * MEG_DIV_64) / MEG_DIV_SCALE;
}

uint32_t rtc_us_to_subsec(uint32_t us) {
    return RTC_SYNCH_PREDIV - (us * MEG_DIV_SCALE / MEG_DIV_64);
}
#else
#define rtc_us_to_subsec
#define rtc_subsec_to_us
#endif

mp_obj_t pyb_rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    rtc_init_finalise();
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
            mp_obj_new_int(rtc_subsec_to_us(time.SubSeconds)),
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
        time.SubSeconds = rtc_us_to_subsec(mp_obj_get_int(items[7]));
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

    rtc_init_finalise();

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
                // use 1Hz clock
                wucksel = 4;
                wut = ms / 1000;
                if (wut > 0x10000) {
                    // wut too large for 16-bit register, try to offset by 0x10000
                    wucksel = 6;
                    wut -= 0x10000;
                    if (wut > 0x10000) {
                        // wut still too large
                        mp_raise_ValueError("wakeup value too large");
                    }
                }
            }
            // wut register should be 1 less than desired value, but guard against wut=0
            if (wut > 0) {
                wut -= 1;
            }
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
        RTC->CR = (RTC->CR & ~7) | (1 << 14) | (1 << 10) | (wucksel & 7);

        // enable register write protection
        RTC->WPR = 0xff;

        // enable external interrupts on line 22
        #if defined(MCU_SERIES_L4)
        EXTI->IMR1 |= 1 << 22;
        EXTI->RTSR1 |= 1 << 22;
        #else
        EXTI->IMR |= 1 << 22;
        EXTI->RTSR |= 1 << 22;
        #endif

        // clear interrupt flags
        RTC->ISR &= ~(1 << 10);
        #if defined(MCU_SERIES_L4)
        EXTI->PR1 = 1 << 22;
        #else
        EXTI->PR = 1 << 22;
        #endif

        HAL_NVIC_SetPriority(RTC_WKUP_IRQn, IRQ_PRI_RTC_WKUP, IRQ_SUBPRI_RTC_WKUP);
        HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

        //printf("wut=%d wucksel=%d\n", wut, wucksel);
    } else {
        // clear WUTIE to disable interrupts
        RTC->CR &= ~(1 << 14);

        // enable register write protection
        RTC->WPR = 0xff;

        // disable external interrupts on line 22
        #if defined(MCU_SERIES_L4)
        EXTI->IMR1 &= ~(1 << 22);
        #else
        EXTI->IMR &= ~(1 << 22);
        #endif
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_wakeup_obj, 2, 4, pyb_rtc_wakeup);

// calibration(None)
// calibration(cal)
// When an integer argument is provided, check that it falls in the range [-511 to 512]
// and set the calibration value; otherwise return calibration value
mp_obj_t pyb_rtc_calibration(mp_uint_t n_args, const mp_obj_t *args) {
    rtc_init_finalise();
    mp_int_t cal;
    if (n_args == 2) {
        cal = mp_obj_get_int(args[1]);
        mp_uint_t cal_p, cal_m;
        if (cal < -511 || cal > 512) {
#if defined(MICROPY_HW_RTC_USE_CALOUT) && MICROPY_HW_RTC_USE_CALOUT
            if ((cal & 0xfffe) == 0x0ffe) {
                // turn on/off X18 (PC13) 512Hz output
                // Note:
                //      Output will stay active even in VBAT mode (and inrease current)
                if (cal & 1) {
                    HAL_RTCEx_SetCalibrationOutPut(&RTCHandle, RTC_CALIBOUTPUT_512HZ);
                } else {
                    HAL_RTCEx_DeactivateCalibrationOutPut(&RTCHandle);
                }
                return mp_obj_new_int(cal & 1);
            } else {
                mp_raise_ValueError("calibration value out of range");
            }
#else
            mp_raise_ValueError("calibration value out of range");
#endif
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

STATIC const mp_rom_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&pyb_rtc_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&pyb_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_wakeup), MP_ROM_PTR(&pyb_rtc_wakeup_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&pyb_rtc_calibration_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);

const mp_obj_type_t pyb_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = pyb_rtc_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_rtc_locals_dict,
};
