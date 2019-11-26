/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Emil Renner Berthing
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

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "py/mpconfig.h"

#include "gd32vf103/rcu.h"
#include "gd32vf103/usart.h"

#include "lib/eclic.h"
#include "lib/gpio.h"

#include "uart0.h"

int mp_hal_stdin_rx_chr(void) {
    while (!(USART0->STAT & USART_STAT_RBNE)) {
        /* wait */
    }

    return USART0->DATA;
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    for (; len > 0; len--, str++) {
        while (!(USART0->STAT & USART_STAT_TBE)) {
            /* wait */
        }
        USART0->DATA = *str;
    }
}

void uart0_init(uint32_t pclk, uint32_t target) {
    /* enable GPIO clock */
    RCU->APB2EN |= RCU_APB2EN_PAEN;
    /* enable USART clock */
    RCU->APB2EN |= RCU_APB2EN_USART0EN;

    gpio_pin_config(GPIO_PA9,  GPIO_MODE_AF_PP_50MHZ);
    gpio_pin_config(GPIO_PA10, GPIO_MODE_IN_FLOAT);

    /* reset usart0 */
    RCU->APB2RST |= RCU_APB2RST_USART0RST;
    RCU->APB2RST &= ~RCU_APB2RST_USART0RST;

    /* set baudrate */
    USART0->BAUD = (2 * pclk + target) / (2 * target);
    /* set 1 stop bit */
    USART0->CTL1 = USART_CTL1_STB_1;
    /* enable rx and tx */
    USART0->CTL0 = USART_CTL0_TEN | USART_CTL0_REN;
    /* enable usart0 */
    USART0->CTL0 |= USART_CTL0_UEN;
}
