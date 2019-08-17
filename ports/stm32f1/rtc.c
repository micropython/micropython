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
#include "extint.h"
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

// Note: LSI is around (40KHz), these dividers should work either way
// ck_spre(1Hz) = RTCCLK(LSE) /(uwPrediv + 1) where uwPrediv in range [0, 0xFFFFF]
#ifndef RTC_PREDIV
    #if defined(MICROPY_HW_RTC_USE_LSE) || defined(MICROPY_HW_RTC_USE_BYPASS)
        #define RTC_PREDIV        LSE_VALUE
    #else
        #define RTC_PREDIV        LSI_VALUE
    #endif
#endif

STATIC HAL_StatusTypeDef PYB_RTC_Init(RTC_HandleTypeDef *hrtc);
STATIC void PYB_RTC_MspInit_Kick(RTC_HandleTypeDef *hrtc, bool rtc_use_lse, bool rtc_use_byp);
STATIC HAL_StatusTypeDef PYB_RTC_MspInit_Finalise(RTC_HandleTypeDef *hrtc);
STATIC void RTC_CalendarConfig(void);

STATIC HAL_StatusTypeDef RTC_EnterInitMode();

#if MICROPY_HW_RTC_USE_LSE || MICROPY_HW_RTC_USE_BYPASS
STATIC bool rtc_use_lse = true;
#else
STATIC bool rtc_use_lse = false;
#endif

STATIC uint32_t rtc_startup_tick;
STATIC bool rtc_need_init_finalise = false;

void rtc_init_start(bool force_init) {
    RTCHandle.Instance = RTC;

    /* Configure RTC prescaler and RTC data registers */
    /* RTC configured as follow:
      - Asynch Prediv  = Value according to source clock
      - OutPut         = Output Disable
     */
    RTCHandle.Init.AsynchPrediv = RTC_PREDIV;
    #if defined(MICROPY_HW_RTC_USE_CALOUT) && MICROPY_HW_RTC_USE_CALOUT
    RTCHandle.Init.OutPut = RTC_OUTPUTSOURCE_SECOND;
    #else
    RTCHandle.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
    #endif
    rtc_need_init_finalise = false;

    if (!force_init) {
        uint32_t bdcr = RCC->BDCR; /* BDCR => Backup domain control register */
        if ((bdcr & (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL | RCC_BDCR_LSEON | RCC_BDCR_LSERDY))
            == (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_0 | RCC_BDCR_LSEON | RCC_BDCR_LSERDY)
        ) {
            // LSE is enabled & ready --> no need to (re-)init RTC
            // remove Backup Domain write protection
            HAL_PWR_EnableBkUpAccess();
            // Clear source Reset Flag
            __HAL_RCC_CLEAR_RESET_FLAGS();
            // provide some status information
            rtc_info |= 0x40000 | (RCC->BDCR & 7) | (RCC->CSR & 3) << 8;
            return;
        } else if (
            (bdcr & (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL)) == (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_1)
        ) {
            // LSI configured as the RTC clock source --> no need to (re-)init RTC
            // remove Backup Domain write protection
            HAL_PWR_EnableBkUpAccess();
            // Clear source Reset Flag
            __HAL_RCC_CLEAR_RESET_FLAGS();
            // Turn the LSI on (it may need this even if the RTC is running)
            RCC->CSR |= RCC_CSR_LSION;
            // provide some status information
            rtc_info |= 0x80000 | (RCC->BDCR & 7) | (RCC->CSR & 3) << 8;
            return;
        }
    }

    rtc_startup_tick = HAL_GetTick();
    rtc_info = 0x3f000000 | (rtc_startup_tick & 0xffffff);
    PYB_RTC_MspInit_Kick(&RTCHandle, rtc_use_lse, MICROPY_HW_RTC_USE_BYPASS);
}

void rtc_init_finalise() {
    if (!rtc_need_init_finalise) {
        return;
    }

    rtc_info = 0;
    while (PYB_RTC_Init(&RTCHandle) != HAL_OK) {
        if (rtc_use_lse) {
            #if MICROPY_HW_RTC_USE_BYPASS
            if (RCC->BDCR & RCC_BDCR_LSEBYP) {
                // LSEBYP failed, fallback to LSE non-bypass
                rtc_info |= 0x02000000;
            } else
            #endif
            {
                // LSE failed, fallback to LSI
                rtc_use_lse = false;
                rtc_info |= 0x01000000;
            }
            rtc_startup_tick = HAL_GetTick();
            PYB_RTC_MspInit_Kick(&RTCHandle, rtc_use_lse, false);
            HAL_PWR_EnableBkUpAccess();
            RTCHandle.State = HAL_RTC_STATE_RESET;
        } else {
            // init error
            rtc_info |= 0xffff; // indicate error
            return;
        }
    }

    // RTC started successfully
    rtc_info = 0x20000000;

    // record if LSE or LSI is used
    rtc_info |= (rtc_use_lse << 28);

    // record how long it took for the RTC to start up
    rtc_info |= (HAL_GetTick() - rtc_startup_tick) & 0xffff;

    // fresh reset; configure RTC Calendar
    RTC_CalendarConfig();
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
    rtc_need_init_finalise = false;
}

STATIC HAL_StatusTypeDef PYB_RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct) {
    /*------------------------------ LSI Configuration -------------------------*/
    if ((RCC_OscInitStruct->OscillatorType & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI) {
        // Check the LSI State
        if (RCC_OscInitStruct->LSIState != RCC_LSI_OFF) {
            __HAL_RCC_LSI_ENABLE();
        } else {
            __HAL_RCC_LSI_DISABLE();
        }
    }

    /*------------------------------ LSE Configuration -------------------------*/
    if ((RCC_OscInitStruct->OscillatorType & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE) {
        // Enable Power Clock
        __HAL_RCC_PWR_CLK_ENABLE();

        // Enable access to the backup domain
        HAL_PWR_EnableBkUpAccess();
        uint32_t tickstart = HAL_GetTick();

        // Wait for Backup domain Write protection disable
        while (READ_BIT(PWR->CR, PWR_CR_DBP) == RESET) {
            if (HAL_GetTick() - tickstart > RCC_DBP_TIMEOUT_VALUE) {
                return HAL_TIMEOUT;
            }
        }

        #if MICROPY_HW_RTC_USE_BYPASS
        // If LSEBYP is enabled and new state is non-bypass then disable LSEBYP
        if (RCC_OscInitStruct->LSEState == RCC_LSE_ON && (RCC->BDCR & RCC_BDCR_LSEBYP)) {
            CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);
            while (RCC->BDCR & RCC_BDCR_LSERDY);
            CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);
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

        uint32_t prescaler = 0U;

        /* Clear Flags Bits */
        CLEAR_BIT(hrtc->Instance->CRL, (RTC_FLAG_OW | RTC_FLAG_ALRAF | RTC_FLAG_SEC));
        
        if (hrtc->Init.OutPut != RTC_OUTPUTSOURCE_NONE) {
            /* Disable the selected Tamper pin */
            // CLEAR_BIT(BKP->CR, BKP_CR_TPE);
            BIT_BAND_CLEAR(BKP->CR, BKP_CR_TPE_Pos);
        }

        /* Set the signal which will be routed to RTC Tamper pin*/
        MODIFY_REG(BKP->RTCCR, (BKP_RTCCR_CCO | BKP_RTCCR_ASOE | BKP_RTCCR_ASOS), hrtc->Init.OutPut);

        // Specifies the RTC Asynchronous Predivider value[0x00,0xFFFFF] OR RTC_AUTO_1_SECOND.
        if (hrtc->Init.AsynchPrediv == RTC_AUTO_1_SECOND) {
            prescaler = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC);
        } else {
            prescaler = hrtc->Init.AsynchPrediv;
        }
        
        hrtc->Instance->PRLH = (uint32_t)(prescaler >> 16U);
        hrtc->Instance->PRLL = (uint32_t)(prescaler & 0xffff);
    
        // Exit Initialization mode
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
        
        /* Initialize date to 1st of January 2000 */
        hrtc->DateToUpdate.Year = 0x00U;
        hrtc->DateToUpdate.Month = RTC_MONTH_JANUARY;
        hrtc->DateToUpdate.Date = 0x01U;

        // Set RTC state
        hrtc->State = HAL_RTC_STATE_READY;

        return HAL_OK;
    }
}

STATIC void PYB_RTC_MspInit_Kick(RTC_HandleTypeDef *hrtc, bool rtc_use_lse, bool rtc_use_byp) {
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
    #if MICROPY_HW_RTC_USE_BYPASS
    if (rtc_use_byp) {
        RCC_OscInitStruct.LSEState = RCC_LSE_BYPASS;
        RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    } else
    #endif
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

#ifndef MICROPY_HW_RTC_LSE_TIMEOUT_MS
#define MICROPY_HW_RTC_LSE_TIMEOUT_MS 1000  // ST docs spec 2000 ms LSE startup, seems to be too pessimistic
#endif
#ifndef MICROPY_HW_RTC_LSI_TIMEOUT_MS
#define MICROPY_HW_RTC_LSI_TIMEOUT_MS 500   // this is way too pessimistic, typ. < 1ms
#endif
#ifndef MICROPY_HW_RTC_BYP_TIMEOUT_MS
#define MICROPY_HW_RTC_BYP_TIMEOUT_MS 150
#endif

STATIC HAL_StatusTypeDef PYB_RTC_MspInit_Finalise(RTC_HandleTypeDef *hrtc) {
    // we already had a kick so now wait for the corresponding ready state...
    if (rtc_use_lse) {
        // we now have to wait for LSE ready or timeout
        uint32_t timeout = MICROPY_HW_RTC_LSE_TIMEOUT_MS;
        #if MICROPY_HW_RTC_USE_BYPASS
        if (RCC->BDCR & RCC_BDCR_LSEBYP) {
            timeout = MICROPY_HW_RTC_BYP_TIMEOUT_MS;
        }
        #endif
        uint32_t tickstart = rtc_startup_tick;
        while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET) {
            if ((HAL_GetTick() - tickstart ) > timeout) {
                return HAL_TIMEOUT;
            }
        }
    } else {
        // we now have to wait for LSI ready or timeout
        uint32_t tickstart = rtc_startup_tick;
        while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET) {
            if ((HAL_GetTick() - tickstart ) > MICROPY_HW_RTC_LSI_TIMEOUT_MS) {
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
        return HAL_ERROR;
    }

    // enable RTC peripheral clock
    __HAL_RCC_RTC_ENABLE();
    return HAL_OK;
}

STATIC void RTC_CalendarConfig(void) {
    // set the date to 1st Jan 2015
    RTC_DateTypeDef date = {
        .Year = 15,
        .Month = 1,
        .Date = 1,
        .WeekDay = RTC_WEEKDAY_THURSDAY,
    };

    if(HAL_RTC_SetDate(&RTCHandle, &date, RTC_FORMAT_BIN) != HAL_OK) {
        return;
    }

    // set the time to 00:00:00
    RTC_TimeTypeDef time = {0};
    if (HAL_RTC_SetTime(&RTCHandle, &time, RTC_FORMAT_BIN) != HAL_OK) {
        return;
    }
}

/**
  * @brief  Enters the RTC Initialization mode.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval HAL status
  */
STATIC HAL_StatusTypeDef RTC_EnterInitMode() {
    uint32_t tickstart = HAL_GetTick();
    /* Wait till RTC is in INIT state and if Time out is reached exit */
    while (BIT_BAND(RTC->CRL, RTC_CRL_RTOFF_Pos) == 0) {
        if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE) {
            return HAL_TIMEOUT;
        }
    }

    /* Disable the write protection for RTC registers */
    BIT_BAND_SET(RTC->CRL, RTC_CRL_CNF_Pos);
    
    return HAL_OK;
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
    return MP_OBJ_FROM_PTR(&pyb_rtc_obj);
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


#if defined(MICROPY_HW_RTC_USE_US) && MICROPY_HW_RTC_USE_US
uint32_t rtc_subsec_to_us(uint32_t ss) {
    uint32_t us_tick = 1000000/( (rtc_use_lse ? LSE_VALUE : LSI_VALUE) + 1);
    return ss * us_tick;
}
#else
uint32_t rtc_subsec_to_us(uint32_t ss) {
    uint32_t prediv = rtc_use_lse ? LSE_VALUE : LSI_VALUE;
    return (prediv - ss) * 255 / ( prediv + 1);
}
#endif

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
mp_obj_t pyb_rtc_datetime(size_t n_args, const mp_obj_t *args) {
    rtc_init_finalise();
    if (n_args == 1) {
        // get date and time
        // note: need to call get time then get date to correctly access the registers
        RTC_DateTypeDef date;
        RTC_TimeTypeDef time;
        register uint16_t hPrescaler = 0, lPrescaler = 0;

        HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
        
        hPrescaler = READ_BIT(RTC->DIVH, 0x0F);
        lPrescaler = READ_BIT(RTC->DIVL, 0xFFFF);

        mp_obj_t tuple[8] = {
            mp_obj_new_int(2000 + date.Year),
            mp_obj_new_int(date.Month),
            mp_obj_new_int(date.Date),
            mp_obj_new_int(date.WeekDay),
            mp_obj_new_int(time.Hours),
            mp_obj_new_int(time.Minutes),
            mp_obj_new_int(time.Seconds),
            mp_obj_new_int(rtc_subsec_to_us(( (uint32_t)(hPrescaler) << 16) | (uint32_t)(lPrescaler))),
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
        
        RTC_TimeTypeDef time;
        time.Hours = mp_obj_get_int(items[4]);
        time.Minutes = mp_obj_get_int(items[5]);
        time.Seconds = mp_obj_get_int(items[6]);

        HAL_RTC_SetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
        HAL_RTC_SetTime(&RTCHandle, &time, RTC_FORMAT_BIN);

        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_datetime_obj, 1, 2, pyb_rtc_datetime);

// wakeup(None)
// wakeup(ms, callback=None)
mp_obj_t pyb_rtc_wakeup(size_t n_args, const mp_obj_t *args) {
    rtc_init_finalise();

    // disable wakeup IRQ while we configure it
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);

    bool enable = false;
    mp_int_t wake_time;
    mp_obj_t callback = mp_const_none;
    
    if (args[1] == mp_const_none) {
        // disable wakeup
    } else {
        // time given in ms
        mp_int_t ms = mp_obj_get_int(args[1]);
        
        // use 1Hz clock
        wake_time = (((uint32_t) ((RTC->CNTH & 0xFFFF) << 16U)) | (RTC->CNTL & 0xFFFF)) + (ms / 1000);
        enable = true;
    }

    if (n_args == 3) {
        callback = args[2];
    }

    // set the callback
    MP_STATE_PORT(pyb_extint_callback)[EXTI_RTC_WAKEUP] = callback;

    // disable register write protection
    RTC_EnterInitMode();

    // clear Alarm Flag
    BIT_BAND_CLEAR(RTC->CRH, RTC_CRH_ALRIE_Pos);

    if (enable) {
        // program alarm
        RTC->ALRH = (uint32_t)(wake_time >> 16);
        RTC->ALRL = (uint32_t)(wake_time & 0xffff);

        // set ALRIE to enable alarm interrupts
        BIT_BAND_SET(RTC->CRH, RTC_CRH_ALRIE_Pos);

        // enable register write protection
        BIT_BAND_CLEAR(RTC->CRL, RTC_CRL_CNF_Pos);

        // enable external interrupts on line EXTI_RTC_WAKEUP
        BIT_BAND_SET(EXTI->IMR, EXTI_RTC_WAKEUP);
        BIT_BAND_SET(EXTI->RTSR, EXTI_RTC_WAKEUP);

        // clear interrupt flags
        EXTI->PR = 1 << EXTI_RTC_WAKEUP;

        NVIC_SetPriority(RTC_Alarm_IRQn, IRQ_PRI_RTC_WKUP);
        HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
    } else {
        // enable register write protection
        BIT_BAND_CLEAR(RTC->CRL, RTC_CRL_CNF_Pos);

        // disable external interrupts on line EXTI_RTC_WAKEUP
        BIT_BAND_CLEAR(EXTI->IMR, EXTI_RTC_WAKEUP);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_wakeup_obj, 2, 3, pyb_rtc_wakeup);

// calibration(None)
// calibration(cal)
// When an integer argument is provided, check that it falls in the range [0 to 0x81]
// if 0x80 is provided then trun off CAL output
// if 0x81 is provided then trun on CAL output
// and set the calibration value; otherwise return calibration value
mp_obj_t pyb_rtc_calibration(size_t n_args, const mp_obj_t *args) {
    rtc_init_finalise();
    mp_int_t cal;
    if (__HAL_RCC_BKP_IS_CLK_DISABLED()) {
        __HAL_RCC_BKP_CLK_ENABLE();
    }

    // enable bkp register access
    BIT_BAND_SET(PWR->CR, PWR_CR_DBP_Pos);

    if (n_args == 2) {
        cal = mp_obj_get_int(args[1]);
        if (cal < 0 || cal > 0x7F) {
            #if defined(MICROPY_HW_RTC_USE_CALOUT) && MICROPY_HW_RTC_USE_CALOUT
            // turn on/off (PC13) 1Hz output
            // Note:
            //      Output will stay active even in VBAT mode (and inrease current)
            switch (cal) {
            case 0x80:
                CLEAR_BIT(BKP->RTCCR, BKP_RTCCR_ASOE | BKP_RTCCR_ASOS);
                break;
            case 0x81:
                BIT_BAND_CLEAR(BKP->RTCCR, BKP_RTCCR_CCO_Pos);
                SET_BIT(BKP->RTCCR,   BKP_RTCCR_ASOE | BKP_RTCCR_ASOS);
                break;
            default:
                BIT_BAND_CLEAR(PWR->CR, PWR_CR_DBP_Pos);
                mp_raise_ValueError("calibration value out of range");
            }
            BIT_BAND_CLEAR(PWR->CR, PWR_CR_DBP_Pos);
            return mp_obj_new_int(cal & 1);
            #else
            BIT_BAND_CLEAR(PWR->CR, PWR_CR_DBP_Pos);
            mp_raise_ValueError("calibration value out of range");
            #endif
        }
        MODIFY_REG(BKP->RTCCR, BKP_RTCCR_CAL, cal);
        BIT_BAND_CLEAR(PWR->CR, PWR_CR_DBP_Pos);
        return mp_const_none;
    } else {
        cal = READ_BIT(BKP->RTCCR, BKP_RTCCR_CAL);
        BIT_BAND_CLEAR(PWR->CR, PWR_CR_DBP_Pos);
        return mp_obj_new_int(cal);
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_calibration_obj, 1, 2, pyb_rtc_calibration);

STATIC const mp_rom_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),        MP_ROM_PTR(&pyb_rtc_init_obj)        },
    { MP_ROM_QSTR(MP_QSTR_info),        MP_ROM_PTR(&pyb_rtc_info_obj)        },
    { MP_ROM_QSTR(MP_QSTR_datetime),    MP_ROM_PTR(&pyb_rtc_datetime_obj)    },
    { MP_ROM_QSTR(MP_QSTR_wakeup),      MP_ROM_PTR(&pyb_rtc_wakeup_obj)      },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&pyb_rtc_calibration_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);

const mp_obj_type_t pyb_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .make_new = pyb_rtc_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_rtc_locals_dict,
};
