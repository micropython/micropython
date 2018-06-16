/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_I2CSLAVE_H
#define MICROPY_INCLUDED_STM32_I2CSLAVE_H

#include STM32_HAL_H

typedef I2C_TypeDef i2c_slave_t;

void i2c_slave_init_helper(i2c_slave_t *i2c, int addr);

static inline void i2c_slave_init(i2c_slave_t *i2c, int irqn, int irq_pri, int addr) {
    int en_bit = RCC_APB1ENR_I2C1EN_Pos + ((uintptr_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    RCC->APB1ENR |= 1 << en_bit;
    volatile uint32_t tmp = RCC->APB1ENR; // Delay after enabling clock
    (void)tmp;

    i2c_slave_init_helper(i2c, addr);

    NVIC_SetPriority(irqn, irq_pri);
    NVIC_EnableIRQ(irqn);
}

static inline void i2c_slave_shutdown(i2c_slave_t *i2c, int irqn) {
    i2c->CR1 = 0;
    NVIC_DisableIRQ(irqn);
}

void i2c_slave_ev_irq_handler(i2c_slave_t *i2c);

// These should be provided externally
int i2c_slave_process_addr_match(int rw);
int i2c_slave_process_rx_byte(uint8_t val);
void i2c_slave_process_rx_end(void);
uint8_t i2c_slave_process_tx_byte(void);

#endif // MICROPY_INCLUDED_STM32_I2CSLAVE_H
