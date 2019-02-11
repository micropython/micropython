/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Analog Devices Inc.
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
#ifndef __MPHALPORT_H__
#define __MPHALPORT_H__

#define asm __asm
#include <sys/platform.h>
#include "py/obj.h"

#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)

#define GPIOA   ((pin_gpio_t *)REG_PORTA_FER)
#define GPIOB   ((pin_gpio_t *)REG_PORTB_FER)
#define GPIOC   ((pin_gpio_t *)REG_PORTC_FER)
#define GPIOD   ((pin_gpio_t *)REG_PORTD_FER)
#define GPIOE   ((pin_gpio_t *)REG_PORTE_FER)
#define GPIOF   ((pin_gpio_t *)REG_PORTF_FER)
#define GPIOG   ((pin_gpio_t *)REG_PORTG_FER)

typedef struct {
    volatile uint32_t FER;      // AF enable register
    volatile uint32_t FER_SET;  // AF enable set register
    volatile uint32_t FER_CLR;  // AF enable clear register
    volatile uint32_t DATA;     // Data register
    volatile uint32_t DATA_SET; // Set output register
    volatile uint32_t DATA_CLR; // Clear output register
    volatile uint32_t DIR;      // Direction register
    volatile uint32_t DIR_SET;  // Direction set register
    volatile uint32_t DIR_CLR;  // Direction clear register
    volatile uint32_t INEN;     // Input enable register
    volatile uint32_t INEN_SET; // Input enable set register
    volatile uint32_t INEN_CLR; // Input enable clear register
    volatile uint32_t MUX;      // MUX control register
    volatile uint32_t DATA_TGL; // Toggle output register
    volatile uint32_t POL;      // Polarity register
    volatile uint32_t POL_SET;  // Polarity set register
    volatile uint32_t POL_CLR;  // Polarity clear register
    volatile uint32_t LOCK;     // Lock register
} pin_gpio_t;

#define GPIO_OUTPUT_TYPE    ((uint32_t)0x00000010)

#define GPIO_MODE_INPUT     ((uint32_t)0x00000000)
#define GPIO_MODE_OUTPUT_PP ((uint32_t)0x00000001)
#define GPIO_MODE_AF_PP     ((uint32_t)0x00000002)
#define GPIO_MODE_INVALID   ((uint32_t)0xFFFFFFFF)
#define GPIO_MODE_IT_RISING    ((uint32_t)0)
#define GPIO_MODE_IT_FALLING   ((uint32_t)1)
#define GPIO_MODE_IT_LOW_LEVEL ((uint32_t)2)

#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_MODE_INPUT)              ||\
                            ((MODE) == GPIO_MODE_OUTPUT_PP)          ||\
                            ((MODE) == GPIO_MODE_AF_PP))

#define GPIO_NOPULL         ((uint32_t)0)
#define GPIO_PULLUP         ((uint32_t)1)
#define GPIO_PULLDOWN       ((uint32_t)2)

#define IS_GPIO_PULL(PULL) (((PULL) == GPIO_NOPULL) || ((PULL) == GPIO_PULLUP) || \
                            ((PULL) == GPIO_PULLDOWN))

#define GPIO_SPEED_FREQ_LOW       ((uint32_t)0)
#define GPIO_SPEED_FREQ_MEDIUM    ((uint32_t)1)
#define GPIO_SPEED_FREQ_HIGH      ((uint32_t)2)
#define GPIO_SPEED_FREQ_VERY_HIGH ((uint32_t)3)

#define IS_GPIO_AF(af)      ((af) >= 0 && (af) <= 3)

__attribute__(( always_inline )) static inline void __WFI(void) {
  __asm volatile ("wfi");
}

#include "lib/utils/interrupt_char.h"

void mp_hal_uart_interrupt(unsigned char c);

#include "pin.h"

#define MP_HAL_PIN_FMT                  "%q"
#define MP_HAL_PIN_MODE_INPUT           GPIO_MODE_INPUT
#define MP_HAL_PIN_MODE_OUTPUT          GPIO_MODE_OUTPUT_PP
#define MP_HAL_PIN_MODE_ALT             GPIO_MODE_AF_PP
#define MP_HAL_PIN_MODE_INVALID         GPIO_MODE_INVALID

#define MP_HAL_PIN_PULL_NONE            0
#define MP_HAL_PIN_PULL_UP              0
#define MP_HAL_PIN_PULL_DOWN            0

#define mp_hal_pin_obj_t const pin_obj_t*
#define mp_hal_get_pin_obj(o)   pin_find(o)
#define mp_hal_pin_name(p)      ((p)->name)
#define mp_hal_pin_input(p)     mp_hal_pin_config((p), MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_output(p)    mp_hal_pin_config((p), MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0)
#define mp_hal_pin_open_drain(p) mp_hal_pin_config((p), MP_HAL_PIN_MODE_INVALID, MP_HAL_PIN_PULL_NONE, 0)

void mp_hal_gpio_init(pin_gpio_t *gpio, uint32_t pin, uint32_t mode, uint32_t af);
void mp_hal_pin_config(mp_hal_pin_obj_t pin_obj, uint32_t mode, uint32_t pull, uint32_t alt);
bool mp_hal_pin_config_alt(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint8_t fn, uint8_t unit);

#define mp_hal_pin_config_alt_static(pin_obj, mode, pull, fn_type) \
        mp_hal_pin_config(pin_obj, mode, pull, fn_type(pin_obj)) /* Overflow Error => alt func not found */

#define mp_hal_ticks_cpu() (0)

#define mp_hal_pin_high(p) (((p)->gpio->DATA_SET) = (p)->pin_mask)
#define mp_hal_pin_low(p)  (((p)->gpio->DATA_CLR) = (p)->pin_mask)
// This is required by the soft-i2c extmod outside of the sc5xx port
#define mp_hal_pin_od_low(p)    mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p)   mp_hal_pin_high(p)
#define mp_hal_pin_read(p) ((((p)->gpio->DATA) >> (p)->pin) & 1)
#define mp_hal_pin_write(p, v)  do { if (v) { mp_hal_pin_high(p); } else { mp_hal_pin_low(p); } } while (0)

#endif
