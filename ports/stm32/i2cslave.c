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

// The hardware triggers the following IRQs for the given scenarios:
// - scan (0-length write):                 ADDR STOPF
// - write of n bytes:                      ADDR RXNE*n STOPF
// - read of n bytes:                       ADDR TXE*(n+1) AF
// - write of n bytes then read of m bytes: ADDR RXNE*n ADDR TXE*(m+1) AF

void i2c_slave_init_helper(i2c_slave_t *i2c, int addr) {
    i2c->CR2 = I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN | 4 << I2C_CR2_FREQ_Pos | I2C_CR2_ITERREN;
    i2c->OAR1 = 1 << 14 | addr << 1;
    i2c->OAR2 = 0;
    i2c->CR1 = I2C_CR1_ACK | I2C_CR1_PE;
}

void i2c_slave_irq_handler(i2c_slave_t *i2c) {
    uint32_t sr1 = i2c->SR1;

    // Clear all error flags.
    i2c->SR1 &= ~(I2C_SR1_SMBALERT | I2C_SR1_TIMEOUT | I2C_SR1_PECERR | I2C_SR1_OVR | I2C_SR1_AF | I2C_SR1_ARLO | I2C_SR1_BERR);

    if (sr1 & I2C_SR1_AF) {
        // A NACK in TX mode, which is a stop condition.
        i2c_slave_process_tx_end(i2c);
    }
    if (sr1 & I2C_SR1_ADDR) {
        // Address matched
        // Read of SR1, SR2 needed to clear ADDR bit
        sr1 = i2c->SR1;
        uint32_t sr2 = i2c->SR2;
        i2c_slave_process_addr_match(i2c, (sr2 >> I2C_SR2_TRA_Pos) & 1);
    }
    if (sr1 & I2C_SR1_TXE) {
        // This callback must call i2c_slave_write_byte.
        i2c_slave_process_tx_byte(i2c);
    }
    if (sr1 & I2C_SR1_RXNE) {
        // This callback must call i2c_slave_read_byte.
        i2c_slave_process_rx_byte(i2c);
    }
    if (sr1 & I2C_SR1_STOPF) {
        // STOPF only set at end of RX mode (in TX mode AF is set on NACK)
        // Read of SR1, write CR1 needed to clear STOPF bit
        sr1 = i2c->SR1;
        i2c->CR1 &= ~I2C_CR1_ACK;
        i2c_slave_process_rx_end(i2c);
        i2c->CR1 |= I2C_CR1_ACK;
    }
}

#elif defined(STM32F7) || defined(STM32H7) || defined(STM32L4) || defined(STM32WB)

void i2c_slave_init_helper(i2c_slave_t *i2c, int addr) {
    i2c->CR1 = I2C_CR1_STOPIE | I2C_CR1_ADDRIE | I2C_CR1_RXIE | I2C_CR1_TXIE;
    i2c->CR2 = 0;
    i2c->OAR1 = I2C_OAR1_OA1EN | addr << 1;
    i2c->OAR2 = 0;
    i2c->CR1 |= I2C_CR1_PE;
}

void i2c_slave_irq_handler(i2c_slave_t *i2c) {
    uint32_t isr = i2c->ISR;
    if (isr & I2C_ISR_ADDR) {
        // Address matched
        // Set TXE so that TXDR is flushed and ready for the first byte
        i2c->ISR = I2C_ISR_TXE;
        i2c->ICR = I2C_ICR_ADDRCF;
        i2c_slave_process_addr_match(i2c, (i2c->ISR >> I2C_ISR_DIR_Pos) & 1);
        // Re-read ISR in case i2c_slave_process_addr_match() took some time
        // to process and TXIS/RXNE was set in the meantime.
        isr = i2c->ISR;
    }
    if (isr & I2C_ISR_TXIS) {
        // This callback must call i2c_slave_write_byte.
        i2c_slave_process_tx_byte(i2c);
    }
    if (isr & I2C_ISR_RXNE) {
        // This callback must call i2c_slave_read_byte.
        i2c_slave_process_rx_byte(i2c);
    }
    if (isr & I2C_ISR_STOPF) {
        // STOPF only set for STOP condition, not a repeated START
        i2c->ICR = I2C_ICR_STOPCF;
        i2c->OAR1 &= ~I2C_OAR1_OA1EN;
        if (i2c->ISR & I2C_ISR_DIR) {
            i2c_slave_process_tx_end(i2c);
        } else {
            i2c_slave_process_rx_end(i2c);
        }
        i2c->OAR1 |= I2C_OAR1_OA1EN;
    }
}

#endif
