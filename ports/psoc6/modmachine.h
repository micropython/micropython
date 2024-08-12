/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#ifndef MICROPY_INCLUDED_PSOC6_MODMACHINE_H
#define MICROPY_INCLUDED_PSOC6_MODMACHINE_H


// micropython includes
#include "py/obj.h"


extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_type_t machine_i2c_slave_type;
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_rtc_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_spi_slave_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_wdt_type;
extern const mp_obj_type_t machine_adcblock_type;
extern const mp_obj_type_t machine_i2s_type;
// extern const mp_obj_type_t machine_uart_type;
#ifdef MICROPY_PY_SD_CARD
extern const mp_obj_type_t machine_sdcard_type;
#endif

// Add all mod machine deinits
void mod_pin_phy_deinit(void);
void mod_pin_deinit(void);
void mod_adc_deinit(void);
void mod_adcblock_deinit(void);
void mod_i2c_deinit(void);
void mod_pwm_deinit(void);
void mod_rtc_deinit(void);
void mod_spi_deinit(void);
void mod_timer_deinit(void);
void mod_wdt_deinit(void);
void mod_uart_deinit(void);
void mod_sdcard_deinit(void);
/* Note: the static functions' prototypes in the .c file cannot be declared here
since they are static. The static type in those functions come from MPY hence
should stay that way.  */


// functions to be called from other .c files. Hence they are not static.
void machine_init(void);
void machine_deinit(void);
mp_obj_t system_reset_cause(void);

#endif // MICROPY_INCLUDED_PSOC6_MODMACHINE_H
