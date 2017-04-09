/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#ifndef HAL_GPIO_H__
#define HAL_GPIO_H__

#if NRF51
  #define POINTERS (const uint32_t[]){NRF_GPIO_BASE}
#endif

#if NRF52
  #ifdef NRF52832_XXAA
    #define POINTERS (const uint32_t[]){NRF_P0_BASE}
  #endif

  #ifdef NRF52840_XXAA
    #define POINTERS (const uint32_t[]){NRF_P0_BASE, NRF_P1_BASE}
  #endif
#endif

#define GPIO_BASE(x) ((NRF_GPIO_Type *)POINTERS[x])

#define hal_gpio_pin_high(p)       (((NRF_GPIO_Type *)(GPIO_BASE((p)->port)))->OUTSET = (p)->pin_mask)
#define hal_gpio_pin_low(p)        (((NRF_GPIO_Type *)(GPIO_BASE((p)->port)))->OUTCLR = (p)->pin_mask)
#define hal_gpio_pin_read(p)       (((NRF_GPIO_Type *)(GPIO_BASE((p)->port)))->IN >> ((p)->pin) & 1)

typedef enum {
    HAL_GPIO_PULL_DISABLED = (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos),
    HAL_GPIO_PULL_DOWN = (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos),
    HAL_GPIO_PULL_UP = (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
} hal_gpio_pull_t;

typedef enum {
    HAL_GPIO_MODE_OUTPUT = (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos),
    HAL_GPIO_MODE_INPUT = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos),
} hal_gpio_mode_t;

static inline void hal_gpio_cfg_pin(uint8_t port, uint32_t pin_number, hal_gpio_mode_t mode, hal_gpio_pull_t pull) {
    GPIO_BASE(port)->PIN_CNF[pin_number] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                         | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                         | pull
                                         | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                         | mode;
}

static inline void hal_gpio_out_set(uint8_t port, uint32_t pin_mask) {
    GPIO_BASE(port)->OUTSET = pin_mask;
}

static inline void hal_gpio_pin_set(uint8_t port, uint32_t pin) {
    GPIO_BASE(port)->OUTSET = (1 << pin);
}

static inline void hal_gpio_pin_clear(uint8_t port, uint32_t pin) {
    GPIO_BASE(port)->OUTCLR = (1 << pin);
}

static inline void hal_gpio_pin_toggle(uint8_t port, uint32_t pin) {
    uint32_t pin_mask = (1 << pin);

    if (GPIO_BASE(port)->OUT ^ pin_mask) {
        GPIO_BASE(port)->OUTSET = pin_mask;
    } else {
        GPIO_BASE(port)->OUTCLR = pin_mask;
    }
}

#endif // HAL_GPIO_H__
