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

#include <unistd.h>

#include <sys/ioctl.h>
#include <arch/chip/pin.h>
#include <arch/chip/adc.h>
#include <arch/board/board.h>

#include "shared-bindings/microcontroller/Pin.h"

typedef struct {
    const mcu_pin_obj_t *pin;
    bool reset;
    bool free;
} pin_status_t;

const mcu_pin_obj_t pin_D0 = PIN(PIN_UART2_RXD, false);
const mcu_pin_obj_t pin_D1 = PIN(PIN_UART2_TXD, false);
const mcu_pin_obj_t pin_D2 = PIN(PIN_HIF_IRQ_OUT, false);
const mcu_pin_obj_t pin_D3 = PIN(PIN_PWM3, false);
const mcu_pin_obj_t pin_D4 = PIN(PIN_SPI2_MOSI, false);
const mcu_pin_obj_t pin_D5 = PIN(PIN_PWM1, false);
const mcu_pin_obj_t pin_D6 = PIN(PIN_PWM0, false);
const mcu_pin_obj_t pin_D7 = PIN(PIN_SPI3_CS1_X, false);
const mcu_pin_obj_t pin_D8 = PIN(PIN_SPI2_MISO, false);
const mcu_pin_obj_t pin_D9 = PIN(PIN_PWM2, false);
const mcu_pin_obj_t pin_D10 = PIN(PIN_SPI4_CS_X, false);
const mcu_pin_obj_t pin_D11 = PIN(PIN_SPI4_MOSI, false);
const mcu_pin_obj_t pin_D12 = PIN(PIN_SPI4_MISO, false);
const mcu_pin_obj_t pin_D13 = PIN(PIN_SPI4_SCK, false);
const mcu_pin_obj_t pin_D14 = PIN(PIN_I2C0_BDT, false);
const mcu_pin_obj_t pin_D15 = PIN(PIN_I2C0_BCK, false);
const mcu_pin_obj_t pin_D16 = PIN(PIN_EMMC_DATA0, false);
const mcu_pin_obj_t pin_D17 = PIN(PIN_EMMC_DATA1, false);
const mcu_pin_obj_t pin_D18 = PIN(PIN_I2S0_DATA_OUT, false);
const mcu_pin_obj_t pin_D19 = PIN(PIN_I2S0_DATA_IN, false);
const mcu_pin_obj_t pin_D20 = PIN(PIN_EMMC_DATA2, false);
const mcu_pin_obj_t pin_D21 = PIN(PIN_EMMC_DATA3, false);
const mcu_pin_obj_t pin_D22 = PIN(PIN_SEN_IRQ_IN, false);
const mcu_pin_obj_t pin_D23 = PIN(PIN_EMMC_CLK, false);
const mcu_pin_obj_t pin_D24 = PIN(PIN_EMMC_CMD, false);
const mcu_pin_obj_t pin_D25 = PIN(PIN_I2S0_LRCK, false);
const mcu_pin_obj_t pin_D26 = PIN(PIN_I2S0_BCK, false);
const mcu_pin_obj_t pin_D27 = PIN(PIN_UART2_CTS, false);
const mcu_pin_obj_t pin_D28 = PIN(PIN_UART2_RTS, false);
const mcu_pin_obj_t pin_LED0 = PIN(PIN_I2S1_BCK, false);
const mcu_pin_obj_t pin_LED1 = PIN(PIN_I2S1_LRCK, false);
const mcu_pin_obj_t pin_LED2 = PIN(PIN_I2S1_DATA_IN, false);
const mcu_pin_obj_t pin_LED3 = PIN(PIN_I2S1_DATA_OUT, false);
const mcu_pin_obj_t pin_A0 = PIN(0, true);
const mcu_pin_obj_t pin_A1 = PIN(1, true);
const mcu_pin_obj_t pin_A2 = PIN(2, true);
const mcu_pin_obj_t pin_A3 = PIN(3, true);
const mcu_pin_obj_t pin_A4 = PIN(4, true);
const mcu_pin_obj_t pin_A5 = PIN(5, true);

STATIC pin_status_t pins[] = {
    { &pin_D0, true, true },
    { &pin_D1, true, true },
    { &pin_D2, true, true },
    { &pin_D3, true, true },
    { &pin_D4, true, true },
    { &pin_D5, true, true },
    { &pin_D6, true, true },
    { &pin_D7, true, true },
    { &pin_D8, true, true },
    { &pin_D9, true, true },
    { &pin_D10, true, true },
    { &pin_D11, true, true },
    { &pin_D12, true, true },
    { &pin_D13, true, true },
    { &pin_D14, true, true },
    { &pin_D15, true, true },
    { &pin_D16, true, true },
    { &pin_D17, true, true },
    { &pin_D18, true, true },
    { &pin_D19, true, true },
    { &pin_D20, true, true },
    { &pin_D21, true, true },
    { &pin_D22, true, true },
    { &pin_D23, true, true },
    { &pin_D24, true, true },
    { &pin_D25, true, true },
    { &pin_D26, true, true },
    { &pin_D27, true, true },
    { &pin_D28, true, true },
    { &pin_LED0, true, true },
    { &pin_LED1, true, true },
    { &pin_LED2, true, true },
    { &pin_LED3, true, true },
};

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    for (int i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (pins[i].pin->number == pin->number) {
            return pins[i].free;
        }
    }

    return true;
}

void never_reset_pin_number(uint8_t pin_number) {
    for (int i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (pins[i].pin->number == pin_number) {
            pins[i].reset = false;
        }
    }
}

void reset_pin_number(uint8_t pin_number) {
    for (int i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (pins[i].pin->number == pin_number) {
            pins[i].free = true;
        }
    }
}

void reset_all_pins(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (!pins[i].free && pins[i].reset) {
            board_gpio_write(pins[i].pin->number, -1);
            board_gpio_config(pins[i].pin->number, 0, false, true, PIN_FLOAT);
            board_gpio_int(pins[i].pin->number, false);
            board_gpio_intconfig(pins[i].pin->number, 0, false, NULL);
            pins[i].free = true;
        }
    }
}

void claim_pin(const mcu_pin_obj_t *pin) {
    for (int i = 0; i < MP_ARRAY_SIZE(pins); i++) {
        if (pins[i].pin->number == pin->number) {
            pins[i].free = false;
        }
    }
}
