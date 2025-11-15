/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#ifndef __MOD_MACHINE_H_
#define __MOD_MACHINE_H_

#include "py/obj.h"

extern const mp_obj_type_t machine_lcd_type;
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_extint_type;
extern const mp_obj_type_t machine_rtc_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_hard_i2c_type;
extern const mp_obj_type_t machine_simulation_i2c_type;
extern const mp_obj_type_t machine_hard_spi_type;
extern const mp_obj_type_t machine_wdt_type;
extern const mp_obj_type_t machine_nandflash_type;
#if defined(PLAT_ASR) || defined(PLAT_Unisoc) ||defined(PLAT_ASR_1606)
extern const mp_obj_type_t machine_keypad_type;
#endif



// void machine_timer_deinit_all(void);

#endif /* __MOD_MACHINE_H_ */


