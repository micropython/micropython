/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

#ifndef MICROPY_INCLUDED_MIMXRT10XX_PERIPHERALS_PERIPH_H
#define MICROPY_INCLUDED_MIMXRT10XX_PERIPHERALS_PERIPH_H

#include "pins.h"

typedef struct {
    uint8_t bank_idx : 4;
    uint8_t mux_mode : 4;
    uint32_t input_reg;
    uint8_t input_idx;
    const mcu_pin_obj_t *pin;
} mcu_periph_obj_t;

#define PERIPH_PIN(p_bank_idx, p_mux_mode, p_input_reg, p_input_idx, p_pin) \
    { \
        .bank_idx = p_bank_idx, \
        .mux_mode = p_mux_mode, \
        .input_reg = p_input_reg == 0 ? 0 : (uint32_t)&(IOMUXC->SELECT_INPUT[p_input_reg]), \
        .input_idx = p_input_idx, \
        .pin = p_pin, \
    }

typedef struct {
    PWM_Type *pwm;
    pwm_submodule_t submodule : 4;
    pwm_channels_t channel : 4;
    uint8_t mux_mode;
    uint8_t input_idx;
    uint32_t input_reg;
    const mcu_pin_obj_t *pin;
} mcu_pwm_obj_t;

#define PWM_PIN(p_pwm, p_submodule, p_channel, p_iomuxc, p_pin) \
    PWM_PIN_(p_pwm, p_submodule, p_channel, p_iomuxc, p_pin)
// ----------------------------------------------------------//
// supplied by the expansion of p_iomuxc into multiple args //
#define PWM_PIN_(p_pwm, p_submodule, p_channel, p_mux_reg, p_mux_mode, p_input_reg, p_input_idx, p_config_reg, p_pin) \
    { \
        .pwm = p_pwm, \
        .submodule = p_submodule, \
        .channel = p_channel, \
        .mux_mode = p_mux_mode, \
        .input_reg = p_input_reg, \
        .input_idx = p_input_idx, \
        .pin = p_pin, \
    }

extern LPI2C_Type *mcu_i2c_banks[];
extern LPSPI_Type *mcu_spi_banks[];
extern LPUART_Type *mcu_uart_banks[];

#ifdef MIMXRT1011_SERIES
#include "MIMXRT1011/periph.h"
#elif defined(MIMXRT1021_SERIES)
#include "MIMXRT1021/periph.h"
#elif defined(MIMXRT1062_SERIES)
#include "MIMXRT1062/periph.h"
#endif

#endif // MICROPY_INCLUDED_MIMXRT10XX_PERIPHERALS_PERIPH_H
