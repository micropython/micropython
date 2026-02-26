/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022-2024 Infineon Technologies AG
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
#ifndef MICROPY_INCLUDED_PSOC6_MODPSOC6_H
#define MICROPY_INCLUDED_PSOC6_MODPSOC6_H


// micropython includes
#include "py/obj.h"

typedef enum
{
    SYSTEM_RESET_NONE,                  /**< No cause */
    SYSTEM_RESET_WDT,                   /**< A watchdog timer (WDT) reset has occurred */
    SYSTEM_RESET_ACTIVE_FAULT,          /**< The fault logging system requested a reset from its Active logic. */
    SYSTEM_RESET_DEEPSLEEP_FAULT,       /**< The fault logging system requested a reset from its Deep-Sleep logic. */
    SYSTEM_RESET_SOFT,                  /**< The CPU requested a system reset through it's SYSRESETREQ. */
    SYSTEM_RESET_HIB_WAKEUP,            /**< A reset has occurred due to a a wakeup from hibernate power mode. */
    SYSTEM_RESET_WCO_ERR,               /**< A reset has occurred due to a watch-crystal clock error */
    SYSTEM_RESET_SYS_CLK_ERR,           /**< A reset has occurred due to a system clock error */
    SYSTEM_RESET_PROTECTION,            /**< A reset has occurred due to a protection violation */
    SYSTEM_RESET_WARMBOOT,              /**< A reset has occurred due wake up from DSRAM, which is a Warm Boot */
    SYSTEM_RESET_MP_SOFT                /**< A reset has occurred due to a soft reset from micropython side*/
} system_reset_reason_t;

extern const mp_obj_type_t psoc6_flash_type;
extern const mp_obj_type_t psoc6_qspi_flash_type;
extern const mp_obj_type_t machine_sdcard_type;

#endif // MICROPY_INCLUDED_PSOC6_MODPSOC6_H
