/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

#ifndef MICROPY_INCLUDED_EXTMOD_MODMACHINE_H
#define MICROPY_INCLUDED_EXTMOD_MODMACHINE_H

#include "py/obj.h"

// Whether to enable the ADC.init() method.
// Requires a port to implement mp_machine_adc_init_helper().
#ifndef MICROPY_PY_MACHINE_ADC_INIT
#define MICROPY_PY_MACHINE_ADC_INIT (0)
#endif

// Whether to enable the ADC.deinit() method.
// Requires a port to implement mp_machine_adc_deinit().
#ifndef MICROPY_PY_MACHINE_ADC_DEINIT
#define MICROPY_PY_MACHINE_ADC_DEINIT (0)
#endif

// Whether to enable the ADC.block() method.
// Requires a port to implement mp_machine_adc_block().
#ifndef MICROPY_PY_MACHINE_ADC_BLOCK
#define MICROPY_PY_MACHINE_ADC_BLOCK (0)
#endif

// Whether to enable the ADC.read_uv() method.
// Requires a port to implement mp_machine_adc_read_uv().
#ifndef MICROPY_PY_MACHINE_ADC_READ_UV
#define MICROPY_PY_MACHINE_ADC_READ_UV (0)
#endif

// Whether to enable the ADC.atten() and ADC.width() methods.
// Note: these are legacy and should not be used on new ports.
#ifndef MICROPY_PY_MACHINE_ADC_ATTEN_WIDTH
#define MICROPY_PY_MACHINE_ADC_ATTEN_WIDTH (0)
#endif

// Whether to enable the ADC.read() method.
// Note: this is legacy and should not be used on new ports.
#ifndef MICROPY_PY_MACHINE_ADC_READ
#define MICROPY_PY_MACHINE_ADC_READ (0)
#endif

// A port must provide these types, but they are otherwise opaque.
typedef struct _machine_adc_obj_t machine_adc_obj_t;
typedef struct _machine_i2s_obj_t machine_i2s_obj_t;
typedef struct _machine_pwm_obj_t machine_pwm_obj_t;
typedef struct _machine_wdt_obj_t machine_wdt_obj_t;

// These classes correspond to machine.Type entries in the machine module.
// Their Python bindings are implemented in extmod, and their implementation
// is provided by a port.
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_type_t machine_i2s_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_wdt_type;

#endif // MICROPY_INCLUDED_EXTMOD_MODMACHINE_H
