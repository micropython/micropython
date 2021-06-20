/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#ifndef MICROPY_INCLUDED_CXD56_COMMON_HAL_MICROCONTROLLER_PIN_H
#define MICROPY_INCLUDED_CXD56_COMMON_HAL_MICROCONTROLLER_PIN_H

#include "py/obj.h"

extern const mp_obj_type_t mcu_pin_type;

#define PIN(pin, a) \
    { \
        { &mcu_pin_type }, \
        .number = (pin), \
        .analog = (a) \
    }

typedef struct {
    mp_obj_base_t base;
    uint8_t number;
    bool analog;
} mcu_pin_obj_t;

extern const mcu_pin_obj_t pin_UART2_RXD;
extern const mcu_pin_obj_t pin_UART2_TXD;
extern const mcu_pin_obj_t pin_HIF_IRQ_OUT;
extern const mcu_pin_obj_t pin_PWM3;
extern const mcu_pin_obj_t pin_SPI2_MOSI;
extern const mcu_pin_obj_t pin_PWM1;
extern const mcu_pin_obj_t pin_PWM0;
extern const mcu_pin_obj_t pin_SPI3_CS1_X;
extern const mcu_pin_obj_t pin_SPI2_MISO;
extern const mcu_pin_obj_t pin_PWM2;
extern const mcu_pin_obj_t pin_SPI4_CS_X;
extern const mcu_pin_obj_t pin_SPI4_MOSI;
extern const mcu_pin_obj_t pin_SPI4_MISO;
extern const mcu_pin_obj_t pin_SPI4_SCK;
extern const mcu_pin_obj_t pin_I2C0_BDT;
extern const mcu_pin_obj_t pin_I2C0_BCK;
extern const mcu_pin_obj_t pin_EMMC_DATA0;
extern const mcu_pin_obj_t pin_EMMC_DATA1;
extern const mcu_pin_obj_t pin_I2S0_DATA_OUT;
extern const mcu_pin_obj_t pin_I2S0_DATA_IN;
extern const mcu_pin_obj_t pin_EMMC_DATA2;
extern const mcu_pin_obj_t pin_EMMC_DATA3;
extern const mcu_pin_obj_t pin_SEN_IRQ_IN;
extern const mcu_pin_obj_t pin_EMMC_CLK;
extern const mcu_pin_obj_t pin_EMMC_CMD;
extern const mcu_pin_obj_t pin_I2S0_LRCK;
extern const mcu_pin_obj_t pin_I2S0_BCK;
extern const mcu_pin_obj_t pin_UART2_CTS;
extern const mcu_pin_obj_t pin_UART2_RTS;
extern const mcu_pin_obj_t pin_I2S1_BCK;
extern const mcu_pin_obj_t pin_I2S1_LRCK;
extern const mcu_pin_obj_t pin_I2S1_DATA_IN;
extern const mcu_pin_obj_t pin_I2S1_DATA_OUT;
extern const mcu_pin_obj_t pin_SDIO_CLK;
extern const mcu_pin_obj_t pin_SDIO_CMD;
extern const mcu_pin_obj_t pin_SDIO_DATA0;
extern const mcu_pin_obj_t pin_SDIO_DATA1;
extern const mcu_pin_obj_t pin_SDIO_DATA2;
extern const mcu_pin_obj_t pin_SDIO_DATA3;
extern const mcu_pin_obj_t pin_LPADC0;
extern const mcu_pin_obj_t pin_LPADC1;
extern const mcu_pin_obj_t pin_LPADC2;
extern const mcu_pin_obj_t pin_LPADC3;
extern const mcu_pin_obj_t pin_HPADC0;
extern const mcu_pin_obj_t pin_HPADC1;

void never_reset_pin_number(uint8_t pin_number);
void reset_pin_number(uint8_t pin_number);
void reset_all_pins(void);
void claim_pin(const mcu_pin_obj_t *pin);

#endif // MICROPY_INCLUDED_CXD56_COMMON_HAL_MICROCONTROLLER_PIN_H
