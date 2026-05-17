/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
 * Copyright (c) 2022 David Lechner <david@pybricks.com>
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

#include "py/mphal.h"
#include "hub_display.h"

#include STM32_HAL_H

#define I2C_ADDR 0x28

// Registers
#define DEVICE_CONFIG0          0x00
#define DEVICE_CONFIG1          0x01
#define LED_CONFIG0             0x02
#define BANK_BRIGHTNESS         0x03
#define BANK_A_COLOR            0x04
#define BANK_B_COLOR            0x05
#define BANK_C_COLOR            0x06
#define LED0_BRIGHTNESS         0x07
#define LED1_BRIGHTNESS         0x08
#define LED2_BRIGHTNESS         0x09
#define LED3_BRIGHTNESS         0x0A
#define OUT0_COLOR              0x0B
#define OUT1_COLOR              0x0C
#define OUT2_COLOR              0x0D
#define OUT3_COLOR              0x0E
#define OUT4_COLOR              0x0F
#define OUT5_COLOR              0x10
#define OUT6_COLOR              0x11
#define OUT7_COLOR              0x12
#define OUT8_COLOR              0x13
#define OUT9_COLOR              0x14
#define OUT10_COLOR             0x15
#define OUT11_COLOR             0x16
#define RESET                   0x17

// Flags
#define DEVICE_CONFIG0_CHIP_EN              (1 << 6)
#define DEVICE_CONFIG1_LOG_SCALE_EN         (1 << 5)
#define DEVICE_CONFIG1_POWER_SAVE_EN        (1 << 4)
#define DEVICE_CONFIG1_AUTO_INCR_EN         (1 << 3)
#define DEVICE_CONFIG1_PWM_DITHERING_EN     (1 << 2)
#define DEVICE_CONFIG1_MAX_CURRENT_OPTION   (1 << 1)
#define DEVICE_CONFIG1_LED_GLOBAL_OFF       (1 << 0)
#define LED_CONFIG0_LED3_BANK_EN            (1 << 3)
#define LED_CONFIG0_LED2_BANK_EN            (1 << 2)
#define LED_CONFIG0_LED1_BANK_EN            (1 << 1)
#define LED_CONFIG0_LED0_BANK_EN            (1 << 0)

#define LP50XX_NUM_CH 6

// channel mapping:
// CH 0 = battery LED - red
// CH 1 = battery LED - green
// CH 2 = battery LED - blue
// CH 3 = status LED - red
// CH 4 = status LED - green
// CH 5 = status LED - blue

#define FMPI2C_CONVERT_TIMINGS(PRESC, SCLDEL, SDADEL, SCLH, SCLL) \
    (((PRESC) << FMPI2C_TIMINGR_PRESC_Pos) | \
    ((SCLDEL) << FMPI2C_TIMINGR_SCLDEL_Pos) | \
    ((SDADEL) << FMPI2C_TIMINGR_SDADEL_Pos) | \
    ((SCLH) << FMPI2C_TIMINGR_SCLH_Pos) | \
    ((SCLL) << FMPI2C_TIMINGR_SCLL_Pos))

static FMPI2C_HandleTypeDef hub_display_i2c;
static bool hub_display_init;

static struct {
    uint8_t reg;
    uint8_t values[LP50XX_NUM_CH];
} __attribute__((packed)) hub_display_data = {
    .reg = OUT0_COLOR,
};

void HAL_FMPI2C_MspInit(FMPI2C_HandleTypeDef *hfmpi2c) {
    __HAL_RCC_FMPI2C1_CLK_ENABLE();
    mp_hal_pin_config(pyb_pin_LED_SCL, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, 4);
    mp_hal_pin_config(pyb_pin_LED_SDA, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_NONE, 4);
}

void HAL_FMPI2C_MspDeInit(FMPI2C_HandleTypeDef *hfmpi2c) {
    __HAL_RCC_FMPI2C1_CLK_DISABLE();
    __HAL_RCC_FMPI2C1_FORCE_RESET();
    __HAL_RCC_FMPI2C1_RELEASE_RESET();
    mp_hal_pin_config(pyb_pin_LED_SCL, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config(pyb_pin_LED_SDA, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);
}

static void hub_display_i2c_init(void) {
    hub_display_i2c.Instance = FMPI2C1;
    hub_display_i2c.Init.Timing = FMPI2C_CONVERT_TIMINGS(0, 4, 0, 19, 28);
    hub_display_i2c.Init.OwnAddress1 = 0;
    hub_display_i2c.Init.AddressingMode = FMPI2C_ADDRESSINGMODE_7BIT;
    hub_display_i2c.Init.DualAddressMode = FMPI2C_DUALADDRESS_DISABLE;
    hub_display_i2c.Init.OwnAddress2 = 0;
    hub_display_i2c.Init.OwnAddress2Masks = FMPI2C_OA2_NOMASK;
    hub_display_i2c.Init.GeneralCallMode = FMPI2C_GENERALCALL_DISABLE;
    hub_display_i2c.Init.NoStretchMode = FMPI2C_NOSTRETCH_DISABLE;
    HAL_FMPI2C_Init(&hub_display_i2c);
}

void hub_display_set(uint8_t led, uint16_t value) {
    if (led >= LP50XX_NUM_CH) {
        return;
    }

    hub_display_data.values[led] = value >> 8;
}

void hub_display_update(void) {
    if (!hub_display_init) {
        return;
    }

    HAL_FMPI2C_Master_Transmit(&hub_display_i2c, I2C_ADDR, (uint8_t *)&hub_display_data,
        sizeof(hub_display_data), HAL_MAX_DELAY);
}

void hub_display_on(void) {
    if (hub_display_init) {
        return;
    }

    hub_display_i2c_init();
    mp_hal_pin_output(pyb_pin_LED_EN);
    mp_hal_pin_high(pyb_pin_LED_EN);

    static const struct {
        uint8_t reg;
        uint8_t values[11];
    } __attribute__((packed)) init_data = {
        .reg = DEVICE_CONFIG0,
        .values = {
            [DEVICE_CONFIG0] = DEVICE_CONFIG0_CHIP_EN,
            [DEVICE_CONFIG1] = DEVICE_CONFIG1_POWER_SAVE_EN | DEVICE_CONFIG1_PWM_DITHERING_EN | DEVICE_CONFIG1_AUTO_INCR_EN,
            [LED_CONFIG0] = 0,
            [BANK_BRIGHTNESS] = 0,
            [BANK_A_COLOR] = 0,
            [BANK_B_COLOR] = 0,
            [BANK_C_COLOR] = 0,
            [LED0_BRIGHTNESS] = 51, // battery LED
            [LED1_BRIGHTNESS] = 38, // status LED
            [LED2_BRIGHTNESS] = 0,
            [LED3_BRIGHTNESS] = 0,
        }
    };

    HAL_FMPI2C_Master_Transmit(&hub_display_i2c, I2C_ADDR, (uint8_t *)&init_data,
        sizeof(init_data), HAL_MAX_DELAY);

    hub_display_init = true;
}

void hub_display_off(void) {
    if (!hub_display_init) {
        return;
    }

    HAL_FMPI2C_DeInit(&hub_display_i2c);

    mp_hal_pin_config(pyb_pin_LED_EN, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);

    hub_display_init = false;
}
