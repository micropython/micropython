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

#ifndef __MOD_MISC_H_
#define __MOD_MISC_H_

#include "py/obj.h"


extern const mp_obj_type_t misc_power_type;
extern const mp_obj_type_t misc_pwm_type;
extern const mp_obj_type_t misc_pwm_v2_type;
extern const mp_obj_type_t misc_adc_type;
extern const mp_obj_type_t misc_usb_type;
#if MICROPY_QPY_MISC_USBNET
extern const mp_obj_module_t misc_usbnet_module;
#endif
extern const mp_obj_type_t misc_powerkey_type;

#if defined(PLAT_RDA)
extern const mp_obj_module_t machine_temperature_type;
#endif

#endif /* __MOD_MISC_H_ */

