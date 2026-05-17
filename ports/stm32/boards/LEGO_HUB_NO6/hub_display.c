/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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

// Map pixel number 0-24, and 25-36 to TLC bit number.
static const uint8_t hub_display_pixel_map[] = {
    // 5x5 display
    9,  11,   6,   1,  14,
    10,  19,   8,   0,  26,
    23,  18,   3,   2,  24,
    21,  20,  15,  13,  25,
    22,   7,  17,  12,  38,
    // RGB Bluetooth button
    27, 28, 29,
    // RGB middle button (left and right)
    39, 40, 41, 42, 43, 44,
    // RGB battery indicator
    45, 46, 47,
};

static bool hub_display_init;
static uint8_t hub_display_gs_state[96];

static void hub_display_tim_init(void) {
    // TLC maximum GSCLK freq: 33MHz

    // tim12, ch2, pwm

    TIM_TypeDef *tim = TIM12;

    __HAL_RCC_TIM12_CLK_ENABLE();
    tim->CR1 = TIM_CR1_ARPE;

    // Configure PWM mode.
    uint32_t ch = 1; // ch2
    uint32_t reg = 6 << TIM_CCMR1_OC1M_Pos // PWM1 mode
        | 1 << TIM_CCMR1_OC1PE_Pos // preload enabled
        | 0 << TIM_CCMR1_CC1S_Pos // output mode
    ;
    uint32_t shift = 8 * (ch & 1);
    tim->CCMR1 = (tim->CCMR1 & ~(0xff << shift)) | reg << shift;

    // Enable output on pin, active high for normal channel.
    reg = TIM_CCER_CC1E;
    shift = 4 * ch;
    tim->CCER = (tim->CCER & ~(0xf << shift)) | reg << shift;

    // Enable the timer if it's not already running.
    tim->CR1 |= TIM_CR1_CEN;

    // fast
    tim->PSC = 0;
    tim->ARR = 2;

    // 50% duty
    tim->CCR2 = 2;
    tim->EGR = 1; // UG

    mp_hal_pin_config(pyb_pin_TLC_GS_CLK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, 9);
}

static void hub_display_spi_init(void) {
    // TLC maximum SPI freq: 25MHz

    SPI_TypeDef *spi = SPI1;

    __HAL_RCC_SPI1_CLK_ENABLE();
    spi->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | 0 << SPI_CR1_BR_Pos | SPI_CR1_MSTR;
    spi->CR1 |= SPI_CR1_SPE;

    mp_hal_pin_config(pyb_pin_TLC_SCLK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, 5);
    mp_hal_pin_config(pyb_pin_TLC_SOUT, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, 5);
    mp_hal_pin_config(pyb_pin_TLC_SIN, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, 5);
}

static void hub_display_spi_write(uint8_t value) {
    SPI_TypeDef *spi = SPI1;
    spi->DR = value;
    while (!(spi->SR & SPI_SR_TXE)) {
    }
}

// dc: dot control
// mc: maximum current
// bc: global brightness control
// fc: function control
static void hub_display_latch_ctrl(uint8_t dc, uint32_t mc, uint32_t bc, uint8_t fc) {
    hub_display_spi_write(1);                   // bit 768
    hub_display_spi_write(0x96);                // bits 760-767
    for (int i = 0; i < 48; ++i) {
        hub_display_spi_write(0);
    }
    hub_display_spi_write(fc >> 2);             // bits 368-375
    hub_display_spi_write(fc << 6 | bc >> 15);  // bits 360-367
    hub_display_spi_write(bc >> 7);             // bits 352-359
    hub_display_spi_write(bc << 1 | mc >> 8);   // bits 344-351
    hub_display_spi_write(mc);                  // bits 336-343
    for (int i = 0; i < 42; ++i) {
        hub_display_spi_write(dc);
    }
    mp_hal_pin_high(pyb_pin_TLC_LAT);
    mp_hal_delay_us(1);
    mp_hal_pin_low(pyb_pin_TLC_LAT);
}

void hub_display_set(uint8_t led, uint16_t value) {
    led = hub_display_pixel_map[led];
    hub_display_gs_state[led * 2] = value;
    hub_display_gs_state[led * 2 + 1] = value >> 8;
}

void hub_display_update(void) {
    if (!hub_display_init) {
        return;
    }
    hub_display_spi_write(0);
    for (int i = 0; i < 96; ++i) {
        hub_display_spi_write(hub_display_gs_state[95 - i]);
    }
    mp_hal_pin_high(pyb_pin_TLC_LAT);
    mp_hal_delay_us(1);
    mp_hal_pin_low(pyb_pin_TLC_LAT);
}

void hub_display_on(void) {
    if (hub_display_init) {
        return;
    }
    mp_hal_pin_output(pyb_pin_TLC_LAT);
    mp_hal_pin_low(pyb_pin_TLC_LAT);
    hub_display_spi_init();
    for (int i = 0; i < 2; ++i) {
        hub_display_latch_ctrl(0xff, 0, 0x1fffff, 0x11);
    }
    hub_display_tim_init();
    hub_display_init = true;
}

void hub_display_off(void) {
    if (!hub_display_init) {
        return;
    }
    __HAL_RCC_TIM12_CLK_DISABLE();
    __HAL_RCC_TIM12_FORCE_RESET();
    __HAL_RCC_TIM12_RELEASE_RESET();
    __HAL_RCC_SPI1_CLK_DISABLE();
    __HAL_RCC_SPI1_FORCE_RESET();
    __HAL_RCC_SPI1_RELEASE_RESET();
    mp_hal_pin_config(pyb_pin_TLC_SCLK, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config(pyb_pin_TLC_SOUT, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config(pyb_pin_TLC_SIN, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config(pyb_pin_TLC_LAT, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_config(pyb_pin_TLC_GS_CLK, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);
    hub_display_init = false;
}
