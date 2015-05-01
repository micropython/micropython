/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#ifndef PYBSLEEP_H_
#define PYBSLEEP_H_

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
 DECLARE EXPORTED VARIABLES
 ******************************************************************************/
extern const mp_obj_base_t pyb_sleep_obj;

/******************************************************************************
 DECLARE FUNCTIONS
 ******************************************************************************/
void pybsleep_pre_init (void);
void pybsleep_init0 (void);
void pybsleep_signal_soft_reset (void);
void pybsleep_add (const mp_obj_t obj, WakeUpCB_t wakeup);
void pybsleep_remove (const mp_obj_t obj);
void pybsleep_set_wlan_lpds_callback (mp_obj_t cb_obj);
void pybsleep_set_gpio_lpds_callback (mp_obj_t cb_obj);
void pybsleep_set_timer_lpds_callback (mp_obj_t cb_obj);
void pybsleep_configure_timer_wakeup (uint pwrmode);
pybsleep_reset_cause_t pybsleep_get_reset_cause (void);

#endif /* PYBSLEEP_H_ */
