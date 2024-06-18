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

#if !defined(I2C2_BASE)
// This MCU doesn't have I2C2_BASE, define it so that the i2c_idx calculation works.
#define I2C2_BASE (I2C1_BASE + ((I2C3_BASE - I2C1_BASE) / 2))
#endif

typedef I2C_TypeDef i2c_slave_t;

void i2c_slave_init_helper(i2c_slave_t *i2c, int addr);

static inline void i2c_slave_init(i2c_slave_t *i2c, int irqn, int irq_pri, int addr) {
    int i2c_idx = ((uintptr_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    #if defined(STM32F4) || defined(STM32F7)
    RCC->APB1ENR |= 1 << (RCC_APB1ENR_I2C1EN_Pos + i2c_idx);
    volatile uint32_t tmp = RCC->APB1ENR; // Delay after enabling clock
    (void)tmp;
    #elif defined(STM32G0)
    RCC->APBENR1 |= 1 << (RCC_APBENR1_I2C1EN_Pos + i2c_idx);
    volatile uint32_t tmp = RCC->APBENR1; // Delay after enabling clock
    (void)tmp;
    #elif defined(STM32H5) || defined(STM32H7)
    RCC->APB1LENR |= 1 << (RCC_APB1LENR_I2C1EN_Pos + i2c_idx);
    volatile uint32_t tmp = RCC->APB1LENR; // Delay after enabling clock
    (void)tmp;
    #elif defined(STM32N6)
    if (i2c_idx == 3) {
        RCC->APB4ENR1 |= RCC_APB4ENR1_I2C4EN;
        volatile uint32_t tmp = RCC->APB4ENR1; // Delay after enabling clock
        (void)tmp;
    } else {
        RCC->APB1ENR1 |= 1 << (RCC_APB1ENR1_I2C1EN_Pos + i2c_idx);
        volatile uint32_t tmp = RCC->APB1ENR1; // Delay after enabling clock
        (void)tmp;
    }
    #elif defined(STM32WB)
    RCC->APB1ENR1 |= 1 << (RCC_APB1ENR1_I2C1EN_Pos + i2c_idx);
    volatile uint32_t tmp = RCC->APB1ENR1; // Delay after enabling clock
    (void)tmp;
    #endif

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
int i2c_slave_process_addr_match(i2c_slave_t *i2c, int rw);
int i2c_slave_process_rx_byte(i2c_slave_t *i2c, uint8_t val);
void i2c_slave_process_rx_end(i2c_slave_t *i2c);
uint8_t i2c_slave_process_tx_byte(i2c_slave_t *i2c);
void i2c_slave_process_tx_end(i2c_slave_t *i2c);

#endif // MICROPY_INCLUDED_STM32_I2CSLAVE_H
