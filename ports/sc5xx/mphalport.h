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
#include "py/obj.h"
#include <sys/platform.h>

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
#define GPIO_MODE_IT_RISING ((uint32_t)1)
#define GPIO_MODE_IT_FALLING ((uint32_t)2)

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

static inline void mp_hal_set_interrupt_char(char c) {}

void mp_hal_gpio_init(pin_gpio_t *gpio, uint32_t pin, uint32_t mode, uint32_t af);
void extint_register_pin(const void *pin, uint32_t mode, int hard_irq, mp_obj_t callback_obj);

#define mp_hal_ticks_cpu() (0)

struct _pin_obj_t;
#define mp_hal_pin_obj_t const struct _pin_obj_t*
#define mp_hal_pin_high(p) (((p)->gpio->DATA_SET) = (p)->pin_mask)
#define mp_hal_pin_low(p)  (((p)->gpio->DATA_CLR) = (p)->pin_mask)
#define mp_hal_pin_read(p) ((((p)->gpio->DATA) >> (p)->pin) & 1)
#define mp_hal_pin_write(p, v)  do { if (v) { mp_hal_pin_high(p); } else { mp_hal_pin_low(p); } } while (0)

#endif
