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

#include "i2cslave.h"

#if defined(STM32F4)

void i2c_slave_init_helper(i2c_slave_t *i2c, int addr) {
    i2c->CR2 = I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN | 4 << I2C_CR2_FREQ_Pos;
    i2c->OAR1 = 1 << 14 | addr << 1;
    i2c->OAR2 = 0;
    i2c->CR1 = I2C_CR1_ACK | I2C_CR1_PE;
}

void i2c_slave_ev_irq_handler(i2c_slave_t *i2c) {
    uint32_t sr1 = i2c->SR1;
    if (sr1 & I2C_SR1_ADDR) {
        // Address matched
        // Read of SR1, SR2 needed to clear ADDR bit
        sr1 = i2c->SR1;
        uint32_t sr2 = i2c->SR2;
        i2c_slave_process_addr_match((sr2 >> I2C_SR2_TRA_Pos) & 1);
    }
    if (sr1 & I2C_SR1_TXE) {
        i2c->DR = i2c_slave_process_tx_byte();
    }
    if (sr1 & I2C_SR1_RXNE) {
        i2c_slave_process_rx_byte(i2c->DR);
    }
    if (sr1 & I2C_SR1_STOPF) {
        // STOPF only set at end of RX mode (in TX mode AF is set on NACK)
        // Read of SR1, write CR1 needed to clear STOPF bit
        sr1 = i2c->SR1;
        i2c->CR1 &= ~I2C_CR1_ACK;
        i2c_slave_process_rx_end();
        i2c->CR1 |= I2C_CR1_ACK;
    }
}

#elif defined(STM32F7)

void i2c_slave_init_helper(i2c_slave_t *i2c, int addr) {
    i2c->CR1 = I2C_CR1_STOPIE | I2C_CR1_ADDRIE | I2C_CR1_RXIE | I2C_CR1_TXIE;
    i2c->CR2 = 0;
    i2c->OAR1 = I2C_OAR1_OA1EN | addr << 1;
    i2c->OAR2 = 0;
    i2c->CR1 |= I2C_CR1_PE;
}

void i2c_slave_ev_irq_handler(i2c_slave_t *i2c) {
    uint32_t isr = i2c->ISR;
    if (isr & I2C_ISR_ADDR) {
        // Address matched
        // Set TXE so that TXDR is flushed and ready for the first byte
        i2c->ISR = I2C_ISR_TXE;
        i2c->ICR = I2C_ICR_ADDRCF;
        i2c_slave_process_addr_match(0);
    }
    if (isr & I2C_ISR_TXIS) {
        i2c->TXDR = i2c_slave_process_tx_byte();
    }
    if (isr & I2C_ISR_RXNE) {
        i2c_slave_process_rx_byte(i2c->RXDR);
    }
    if (isr & I2C_ISR_STOPF) {
        // STOPF only set for STOP condition, not a repeated START
        i2c->ICR = I2C_ICR_STOPCF;
        i2c->OAR1 &= ~I2C_OAR1_OA1EN;
        if (i2c->ISR & I2C_ISR_DIR) {
            //i2c_slave_process_tx_end();
        } else {
            i2c_slave_process_rx_end();
        }
        i2c->OAR1 |= I2C_OAR1_OA1EN;
    }
}

#endif
