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

#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "extint.h"
#include "rtc.h"
#include "irq.h"

#if defined(STM32WB)
#define RCC_CSR_LSION RCC_CSR_LSI1ON
#define RCC_FLAG_LSIRDY RCC_FLAG_LSI1RDY
#define RCC_OSCILLATORTYPE_LSI RCC_OSCILLATORTYPE_LSI1
#define __HAL_RCC_LSI_ENABLE __HAL_RCC_LSI1_ENABLE
#define __HAL_RCC_LSI_DISABLE __HAL_RCC_LSI1_DISABLE
#endif

/// \moduleref pyb
/// \class RTC - real time clock
///
/// The RTC is an independent clock that keeps track of the date
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
// default is 3906.25 us, min is ~30.52 us (will increase Ivbat by ~500nA)
#ifndef RTC_ASYNCH_PREDIV
#define RTC_ASYNCH_PREDIV (0x7f)
#endif
#ifndef RTC_SYNCH_PREDIV
#define RTC_SYNCH_PREDIV  (0x00ff)
#endif

static HAL_StatusTypeDef PYB_RTC_Init(RTC_HandleTypeDef *hrtc);
static void PYB_RTC_MspInit_Kick(RTC_HandleTypeDef *hrtc, bool rtc_use_lse, bool rtc_use_byp);
static HAL_StatusTypeDef PYB_RTC_MspInit_Finalise(RTC_HandleTypeDef *hrtc);
static void RTC_CalendarConfig(void);

#if MICROPY_HW_RTC_USE_LSE || MICROPY_HW_RTC_USE_BYPASS
static bool rtc_use_lse = true;
#else
static bool rtc_use_lse = false;
#endif
static uint32_t rtc_startup_tick;
static bool rtc_need_init_finalise = false;

#if defined(STM32L0)
#define BDCR CSR
#define RCC_BDCR_RTCEN RCC_CSR_RTCEN
#define RCC_BDCR_RTCSEL RCC_CSR_RTCSEL
#define RCC_BDCR_RTCSEL_0 RCC_CSR_RTCSEL_0
#define RCC_BDCR_RTCSEL_1 RCC_CSR_RTCSEL_1
#define RCC_BDCR_LSEON RCC_CSR_LSEON
#define RCC_BDCR_LSERDY RCC_CSR_LSERDY
#define RCC_BDCR_LSEBYP RCC_CSR_LSEBYP
#elif defined(STM32L1)
#define BDCR CR
#define RCC_BDCR_RTCEN RCC_CSR_RTCEN
#define RCC_BDCR_RTCSEL RCC_CSR_RTCSEL
#define RCC_BDCR_RTCSEL_0 RCC_CSR_RTCSEL_0
#define RCC_BDCR_RTCSEL_1 RCC_CSR_RTCSEL_1
#define RCC_BDCR_LSEON RCC_CSR_LSEON
#define RCC_BDCR_LSERDY RCC_CSR_LSERDY
#define RCC_BDCR_LSEBYP RCC_CSR_LSEBYP
#endif

void rtc_init_start(bool force_init) {
    // Enable the RTC APB bus clock, to communicate with the RTC.
    #if defined(STM32H5)
    __HAL_RCC_RTC_CLK_ENABLE();
    #elif defined(STM32WL)
    __HAL_RCC_RTCAPB_CLK_ENABLE();
    #endif

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
        bool rtc_running = false;
        uint32_t bdcr = RCC->BDCR;
        if ((bdcr & (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL | RCC_BDCR_LSEON | RCC_BDCR_LSERDY))
            == (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_0 | RCC_BDCR_LSEON | RCC_BDCR_LSERDY)) {
            // LSE is enabled & ready --> no need to (re-)init RTC
            rtc_running = true;
            // remove Backup Domain write protection
            HAL_PWR_EnableBkUpAccess();
            // Clear source Reset Flag
            __HAL_RCC_CLEAR_RESET_FLAGS();
            // provide some status information
            rtc_info |= 0x40000;
        } else if ((bdcr & (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL))
                   == (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_1)) {
            // LSI configured as the RTC clock source --> no need to (re-)init RTC
            rtc_running = true;
            // remove Backup Domain write protection
            HAL_PWR_EnableBkUpAccess();
            // Clear source Reset Flag
            __HAL_RCC_CLEAR_RESET_FLAGS();
            // Turn the LSI on (it may need this even if the RTC is running)
            #if defined(STM32H5)
            RCC->BDCR |= RCC_BDCR_LSION;
            #else
            RCC->CSR |= RCC_CSR_LSION;
            #endif
            // provide some status information
            rtc_info |= 0x80000;
        }

        if (rtc_running) {
            // Provide information about the registers that indicated the RTC is running.
            // Bits are (MSB first): LSIRDY LSION LSEBYP LSERDY LSEON
            #if defined(STM32H5)
            rtc_info |= (RCC->BDCR >> RCC_BDCR_LSEON_Pos & 7) | (RCC->BDCR >> RCC_BDCR_LSION_Pos & 3) << 8;
            #else
            rtc_info |= (RCC->BDCR & 7) | (RCC->CSR & 3) << 8;
            #endif

            // Check that the sync and async prescaler values are correct.  If the RTC
            // gets into a state where they are wrong then it will run slow or fast and
            // never be corrected.  In such a situation, attempt to reconfigure the values
            // without changing the data/time.
            if (LL_RTC_GetSynchPrescaler(RTC) != RTC_SYNCH_PREDIV
                || LL_RTC_GetAsynchPrescaler(RTC) != RTC_ASYNCH_PREDIV) {
                // Values are wrong, attempt to enter RTC init mode and change them.
                LL_RTC_DisableWriteProtection(RTC);
                LL_RTC_EnableInitMode(RTC);
                uint32_t ticks_ms = HAL_GetTick();
                while (HAL_GetTick() - ticks_ms < RTC_TIMEOUT_VALUE) {
                    if (LL_RTC_IsActiveFlag_INIT(RTC)) {
                        // Reconfigure the RTC prescaler register PRER.
                        LL_RTC_SetSynchPrescaler(RTC, RTC_SYNCH_PREDIV);
                        LL_RTC_SetAsynchPrescaler(RTC, RTC_ASYNCH_PREDIV);
                        LL_RTC_DisableInitMode(RTC);
                        break;
                    }
                }
                LL_RTC_EnableWriteProtection(RTC);

                // Provide information that the prescaler was changed.
                rtc_info |= 0x100000;
            }

            // The RTC is up and running, so return without any further configuration.
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
    #if defined(STM32G0)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PWRRST) != RESET) {
    #elif defined(STM32G4) || defined(STM32H5) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET) {
    #else
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET) {
        #endif
        // power on reset occurred
        rtc_info |= 0x10000;
    }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
        // external reset occurred
        rtc_info |= 0x20000;
    }
    // Clear source Reset Flag
    __HAL_RCC_CLEAR_RESET_FLAGS();
    rtc_need_init_finalise = false;
}

static HAL_StatusTypeDef PYB_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct) {
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
        #if !defined(STM32H5) && !defined(STM32H7) && !defined(STM32WB) && !defined(STM32WL)
        // Enable Power Clock
        __HAL_RCC_PWR_CLK_ENABLE();
        #endif

        // Enable access to the backup domain
        HAL_PWR_EnableBkUpAccess();
        uint32_t tickstart = HAL_GetTick();

        #if defined(STM32F7) || defined(STM32G0) || defined(STM32G4) || defined(STM32H7) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)
        // __HAL_RCC_PWR_CLK_ENABLE();
        // Enable write access to Backup domain
        // PWR->CR1 |= PWR_CR1_DBP;
        // Wait for Backup domain Write protection disable
        while ((PWR->CR1 & PWR_CR1_DBP) == RESET) {
            if (HAL_GetTick() - tickstart > RCC_DBP_TIMEOUT_VALUE) {
                return HAL_TIMEOUT;
            }
        }
        #elif defined(STM32H5)
        // Wait for Backup domain Write protection disable
        while (!LL_PWR_IsEnabledBkUpAccess()) {
            if (HAL_GetTick() - tickstart > RCC_DBP_TIMEOUT_VALUE) {
                return HAL_TIMEOUT;
            }
        }
        #else
        // Enable write access to Backup domain
        // PWR->CR |= PWR_CR_DBP;
        // Wait for Backup domain Write protection disable
        while ((PWR->CR & PWR_CR_DBP) == RESET) {
            if (HAL_GetTick() - tickstart > RCC_DBP_TIMEOUT_VALUE) {
                return HAL_TIMEOUT;
            }
        }
        #endif

        #if MICROPY_HW_RTC_USE_BYPASS
        // If LSEBYP is enabled and new state is non-bypass then disable LSEBYP
        if (RCC_OscInitStruct->LSEState == RCC_LSE_ON && (RCC->BDCR & RCC_BDCR_LSEBYP)) {
            CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEON);
            while (RCC->BDCR & RCC_BDCR_LSERDY) {
            }
            CLEAR_BIT(RCC->BDCR, RCC_BDCR_LSEBYP);
        }
        #endif

        // Set the new LSE configuration
        __HAL_RCC_LSE_CONFIG(RCC_OscInitStruct->LSEState);
    }

    return HAL_OK;
}

static HAL_StatusTypeDef PYB_RTC_Init(RTC_HandleTypeDef *hrtc) {
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
        hrtc->Instance->CR &= ((uint32_t) ~(RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL));
        // Set RTC_CR register
        hrtc->Instance->CR |= (uint32_t)(hrtc->Init.HourFormat | hrtc->Init.OutPut | hrtc->Init.OutPutPolarity);

        // Configure the RTC PRER
        hrtc->Instance->PRER = (uint32_t)(hrtc->Init.SynchPrediv);
        hrtc->Instance->PRER |= (uint32_t)(hrtc->Init.AsynchPrediv << 16);

        // Exit Initialization mode
        LL_RTC_DisableInitMode(hrtc->Instance);

        #if defined(STM32H7A3xx) || defined(STM32H7A3xxQ) || defined(STM32H7B3xx) || defined(STM32H7B3xxQ)
        // do nothing
        #elif defined(STM32L0) || defined(STM32L4) || defined(STM32H7) || defined(STM32WB)
        hrtc->Instance->OR &= (uint32_t) ~RTC_OR_ALARMOUTTYPE;
        hrtc->Instance->OR |= (uint32_t)(hrtc->Init.OutPutType);
        #elif defined(STM32F7)
        hrtc->Instance->OR &= (uint32_t) ~RTC_OR_ALARMTYPE;
        hrtc->Instance->OR |= (uint32_t)(hrtc->Init.OutPutType);
        #elif defined(STM32G0) || defined(STM32G4) || defined(STM32H5) || defined(STM32WL)
        hrtc->Instance->CR &= (uint32_t) ~RTC_CR_TAMPALRM_TYPE_Msk;
        hrtc->Instance->CR |= (uint32_t)(hrtc->Init.OutPutType);
        #else
        hrtc->Instance->TAFCR &= (uint32_t) ~RTC_TAFCR_ALARMOUTTYPE;
        hrtc->Instance->TAFCR |= (uint32_t)(hrtc->Init.OutPutType);
        #endif

        // Enable the write protection for RTC registers
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

        // Set RTC state
        hrtc->State = HAL_RTC_STATE_READY;

        return HAL_OK;
    }
}

static void PYB_RTC_MspInit_Kick(RTC_HandleTypeDef *hrtc, bool rtc_use_lse, bool rtc_use_byp) {
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
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
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

static HAL_StatusTypeDef PYB_RTC_MspInit_Finalise(RTC_HandleTypeDef *hrtc) {
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
            if ((HAL_GetTick() - tickstart) > timeout) {
                return HAL_TIMEOUT;
            }
        }
    } else {
        // we now have to wait for LSI ready or timeout
        uint32_t tickstart = rtc_startup_tick;
        while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET) {
            if ((HAL_GetTick() - tickstart) > MICROPY_HW_RTC_LSI_TIMEOUT_MS) {
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
        // Error_Handler();
        return HAL_ERROR;
    }

    // enable RTC peripheral clock
    __HAL_RCC_RTC_ENABLE();
    return HAL_OK;
}

static void RTC_CalendarConfig(void) {
    // set the date to 1st Jan 2015
    RTC_DateTypeDef date;
    date.Year = 15;
    date.Month = 1;
    date.Date = 1;
    date.WeekDay = RTC_WEEKDAY_THURSDAY;

    if (HAL_RTC_SetDate(&RTCHandle, &date, RTC_FORMAT_BIN) != HAL_OK) {
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

    if (HAL_RTC_SetTime(&RTCHandle, &time, RTC_FORMAT_BIN) != HAL_OK) {
        // init error
        return;
    }
}

uint64_t mp_hal_time_ns(void) {
    uint64_t ns = 0;
    #if MICROPY_HW_ENABLE_RTC
    // Get current according to the RTC.
    rtc_init_finalise();
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
    ns = timeutils_seconds_since_epoch(2000 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
    ns *= 1000000000ULL;
    uint32_t usec = ((RTC_SYNCH_PREDIV - time.SubSeconds) * (1000000 / 64)) / ((RTC_SYNCH_PREDIV + 1) / 64);
    ns += usec * 1000;
    #endif
    return ns;
}

/******************************************************************************/
// MicroPython bindings

typedef struct _pyb_rtc_obj_t {
    mp_obj_base_t base;
} pyb_rtc_obj_t;

static const pyb_rtc_obj_t pyb_rtc_obj = {{&pyb_rtc_type}};

/// \classmethod \constructor()
/// Create an RTC object.
static mp_obj_t pyb_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
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

mp_obj_t pyb_rtc_datetime(size_t n_args, const mp_obj_t *args) {
    rtc_init_finalise();
    if (n_args == 1) {
        // get date and time
        // note: need to call get time then get date to correctly access the registers
        RTC_DateTypeDef date;
        RTC_TimeTypeDef time;
        HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
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
        HAL_RTC_SetDate(&RTCHandle, &date, RTC_FORMAT_BIN);

        RTC_TimeTypeDef time;
        time.Hours = mp_obj_get_int(items[4]);
        time.Minutes = mp_obj_get_int(items[5]);
        time.Seconds = mp_obj_get_int(items[6]);
        time.TimeFormat = RTC_HOURFORMAT12_AM;
        time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        time.StoreOperation = RTC_STOREOPERATION_SET;
        HAL_RTC_SetTime(&RTCHandle, &time, RTC_FORMAT_BIN);

        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_datetime_obj, 1, 2, pyb_rtc_datetime);

#if defined(STM32F0) || defined(STM32H5) || defined(STM32L0)
#define RTC_WKUP_IRQn RTC_IRQn
#elif defined(STM32G0)
#define RTC_WKUP_IRQn RTC_TAMP_IRQn
#endif

// wakeup(None)
// wakeup(ms, callback=None)
// wakeup(wucksel, wut, callback)
mp_obj_t pyb_rtc_wakeup(size_t n_args, const mp_obj_t *args) {
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
                #if defined(STM32L1)
                if (rtc_use_lse) {
                    wut = LSE_VALUE / div * ms / 1000;
                } else {
                    wut = LSI_VALUE / div * ms / 1000;
                }
                #else
                wut = 32768 / div * ms / 1000;
                #endif
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
                        mp_raise_ValueError(MP_ERROR_TEXT("wakeup value too large"));
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
    MP_STATE_PORT(pyb_extint_callback)[EXTI_RTC_WAKEUP] = callback;

    // disable register write protection
    RTC->WPR = 0xca;
    RTC->WPR = 0x53;

    // clear WUTE
    RTC->CR &= ~RTC_CR_WUTE;

    // wait until WUTWF is set
    while (!LL_RTC_IsActiveFlag_WUTW(RTC)) {
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

        // enable external interrupts on line EXTI_RTC_WAKEUP
        #if defined(STM32G0) || defined(STM32G4) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)
        EXTI->IMR1 |= 1 << EXTI_RTC_WAKEUP;
        EXTI->RTSR1 |= 1 << EXTI_RTC_WAKEUP;
        #elif defined(STM32H5)
        EXTI->IMR1 |= 1 << EXTI_RTC_WAKEUP;
        #elif defined(STM32H7)
        EXTI_D1->IMR1 |= 1 << EXTI_RTC_WAKEUP;
        EXTI->RTSR1 |= 1 << EXTI_RTC_WAKEUP;
        #else
        EXTI->IMR |= 1 << EXTI_RTC_WAKEUP;
        EXTI->RTSR |= 1 << EXTI_RTC_WAKEUP;
        #endif

        // clear interrupt flags
        #if defined(STM32G0) || defined(STM32G4) || defined(STM32WL)
        RTC->ICSR &= ~RTC_ICSR_WUTWF;
        #elif defined(STM32H5)
        RTC->SCR = RTC_SCR_CWUTF;
        #elif defined(STM32H7A3xx) || defined(STM32H7A3xxQ) || defined(STM32H7B3xx) || defined(STM32H7B3xxQ)
        RTC->SR &= ~RTC_SR_WUTF;
        #else
        RTC->ISR &= ~RTC_ISR_WUTF;
        #endif
        #if defined(STM32G4) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)
        EXTI->PR1 = 1 << EXTI_RTC_WAKEUP;
        #elif defined(STM32H7)
        EXTI_D1->PR1 = 1 << EXTI_RTC_WAKEUP;
        #elif defined(STM32G0) || defined(STM32H5)
        // Do nothing
        #else
        EXTI->PR = 1 << EXTI_RTC_WAKEUP;
        #endif

        NVIC_SetPriority(RTC_WKUP_IRQn, IRQ_PRI_RTC_WKUP);
        HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    } else {
        // clear WUTIE to disable interrupts
        RTC->CR &= ~RTC_CR_WUTIE;

        // enable register write protection
        RTC->WPR = 0xff;

        // disable external interrupts on line EXTI_RTC_WAKEUP
        #if defined(STM32G0) || defined(STM32G4) || defined(STM32H5) || defined(STM32L4) || defined(STM32WB) || defined(STM32WL)
        EXTI->IMR1 &= ~(1 << EXTI_RTC_WAKEUP);
        #elif defined(STM32H7)
        EXTI_D1->IMR1 |= 1 << EXTI_RTC_WAKEUP;
        #else
        EXTI->IMR &= ~(1 << EXTI_RTC_WAKEUP);
        #endif
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_rtc_wakeup_obj, 2, 4, pyb_rtc_wakeup);

// calibration(None)
// calibration(cal)
// When an integer argument is provided, check that it falls in the range [-511 to 512]
// and set the calibration value; otherwise return calibration value
mp_obj_t pyb_rtc_calibration(size_t n_args, const mp_obj_t *args) {
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
                mp_raise_ValueError(MP_ERROR_TEXT("calibration value out of range"));
            }
            #else
            mp_raise_ValueError(MP_ERROR_TEXT("calibration value out of range"));
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

static const mp_rom_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_rtc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&pyb_rtc_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&pyb_rtc_datetime_obj) },
    { MP_ROM_QSTR(MP_QSTR_wakeup), MP_ROM_PTR(&pyb_rtc_wakeup_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibration), MP_ROM_PTR(&pyb_rtc_calibration_obj) },
};
static MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_rtc_type,
    MP_QSTR_RTC,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_rtc_make_new,
    locals_dict, &pyb_rtc_locals_dict
    );
