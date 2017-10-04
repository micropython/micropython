/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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
#ifndef MICROPY_INCLUDED_CC3200_MODS_PYBSLEEP_H
#define MICROPY_INCLUDED_CC3200_MODS_PYBSLEEP_H

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define PYB_PWR_MODE_ACTIVE                     (0x01)
#define PYB_PWR_MODE_LPDS                       (0x02)
#define PYB_PWR_MODE_HIBERNATE                  (0x04)

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef enum {
    PYB_SLP_PWRON_RESET = 0,
    PYB_SLP_HARD_RESET,
    PYB_SLP_WDT_RESET,
    PYB_SLP_HIB_RESET,
    PYB_SLP_SOFT_RESET
} pybsleep_reset_cause_t;

typedef enum {
    PYB_SLP_WAKED_PWRON = 0,
    PYB_SLP_WAKED_BY_WLAN,
    PYB_SLP_WAKED_BY_GPIO,
    PYB_SLP_WAKED_BY_RTC
} pybsleep_wake_reason_t;

typedef void (*WakeUpCB_t)(const mp_obj_t self);

/******************************************************************************
 DECLARE FUNCTIONS
 ******************************************************************************/
void pyb_sleep_pre_init (void);
void pyb_sleep_init0 (void);
void pyb_sleep_signal_soft_reset (void);
void pyb_sleep_add (const mp_obj_t obj, WakeUpCB_t wakeup);
void pyb_sleep_remove (const mp_obj_t obj);
void pyb_sleep_set_gpio_lpds_callback (mp_obj_t cb_obj);
void pyb_sleep_set_wlan_obj (mp_obj_t wlan_obj);
void pyb_sleep_set_rtc_obj (mp_obj_t rtc_obj);
void pyb_sleep_sleep (void);
void pyb_sleep_deepsleep (void);
pybsleep_reset_cause_t pyb_sleep_get_reset_cause (void);
pybsleep_wake_reason_t pyb_sleep_get_wake_reason (void);

#endif // MICROPY_INCLUDED_CC3200_MODS_PYBSLEEP_H
