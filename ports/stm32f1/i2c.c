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

#include "py/mperrno.h"
#include "py/mphal.h"
#include "i2c.h"

#if MICROPY_HW_ENABLE_HW_I2C

#if defined(STM32F4)

STATIC uint16_t i2c_timeout_ms[MICROPY_HW_MAX_I2C];

int i2c_init(i2c_t *i2c, mp_hal_pin_obj_t scl, mp_hal_pin_obj_t sda, uint32_t freq, uint16_t timeout_ms) {
    uint32_t i2c_id = ((uint32_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);

    // Init pins
    if (!mp_hal_pin_config_alt(scl, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_UP, AF_FN_I2C, i2c_id + 1)) {
        return -MP_EPERM;
    }
    if (!mp_hal_pin_config_alt(sda, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_UP, AF_FN_I2C, i2c_id + 1)) {
        return -MP_EPERM;
    }

    // Save timeout value
    i2c_timeout_ms[i2c_id] = timeout_ms;

    // Force reset I2C peripheral
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST << i2c_id;
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST << i2c_id);

    // Enable I2C peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN << i2c_id;
    volatile uint32_t tmp = RCC->APB1ENR; // delay after RCC clock enable
    (void)tmp;

    uint32_t PCLK1 = HAL_RCC_GetPCLK1Freq();

    // Initialise I2C peripheral
    i2c->CR1 = 0;
    i2c->CR2 = PCLK1 / 1000000;
    i2c->OAR1 = 0;
    i2c->OAR2 = 0;

    freq = MIN(freq, 400000);

    // SM: MAX(4, PCLK1 / (F * 2))
    // FM, 16:9 duty: 0xc000 | MAX(1, (PCLK1 / (F * (16 + 9))))
    if (freq <= 100000) {
        i2c->CCR = MAX(4, PCLK1 / (freq * 2));
    } else {
        i2c->CCR = 0xc000 | MAX(1, PCLK1 / (freq * 25));
    }

    // SM: 1000ns / (1/PCLK1) + 1 = PCLK1 * 1e-6 + 1
    // FM: 300ns / (1/PCLK1) + 1 = 300e-3 * PCLK1 * 1e-6 + 1
    if (freq <= 100000) {
        i2c->TRISE = PCLK1 / 1000000 + 1; // 1000ns rise time in SM
    } else {
        i2c->TRISE = PCLK1 / 1000000 * 3 / 10 + 1; // 300ns rise time in FM
    }

    #if defined(I2C_FLTR_ANOFF)
    i2c->FLTR = 0; // analog filter on, digital filter off
    #endif

    return 0;
}

STATIC int i2c_wait_sr1_set(i2c_t *i2c, uint32_t mask) {
    uint32_t i2c_id = ((uint32_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    uint32_t t0 = HAL_GetTick();
    while (!(i2c->SR1 & mask)) {
        if (HAL_GetTick() - t0 >= i2c_timeout_ms[i2c_id]) {
            i2c->CR1 &= ~I2C_CR1_PE;
            return -MP_ETIMEDOUT;
        }
    }
    return 0;
}

STATIC int i2c_wait_stop(i2c_t *i2c) {
    uint32_t i2c_id = ((uint32_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    uint32_t t0 = HAL_GetTick();
    while (i2c->CR1 & I2C_CR1_STOP) {
        if (HAL_GetTick() - t0 >= i2c_timeout_ms[i2c_id]) {
            i2c->CR1 &= ~I2C_CR1_PE;
            return -MP_ETIMEDOUT;
        }
    }
    i2c->CR1 &= ~I2C_CR1_PE;
    return 0;
}

// For write: len = 0, 1 or N
// For read: len = 1, 2 or N; stop = true
int i2c_start_addr(i2c_t *i2c, int rd_wrn, uint16_t addr, size_t next_len, bool stop) {
    if (!(i2c->CR1 & I2C_CR1_PE) && (i2c->SR2 & I2C_SR2_MSL)) {
        // The F4 I2C peripheral can sometimes get into a bad state where it's disabled
        // (PE low) but still an active master (MSL high).  It seems the best way to get
        // out of this is a full reset.
        uint32_t i2c_id = ((uint32_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
        RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST << i2c_id;
        RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST << i2c_id);
    }

    // It looks like it's possible to terminate the reading by sending a
    // START condition instead of STOP condition but we don't support that.
    if (rd_wrn) {
        if (!stop) {
            return -MP_EINVAL;
        }
    }

    // Repurpose OAR1 to hold stop flag
    i2c->OAR1 = stop;

    // Enable peripheral and send START condition
    i2c->CR1 |= I2C_CR1_PE;
    i2c->CR1 |= I2C_CR1_START;

    // Wait for START to be sent
    int ret;
    if ((ret = i2c_wait_sr1_set(i2c, I2C_SR1_SB))) {
        return ret;
    }

    // Send the 7-bit address with read/write bit
    i2c->DR = addr << 1 | rd_wrn;

    // Wait for address to be sent
    if ((ret = i2c_wait_sr1_set(i2c, I2C_SR1_AF | I2C_SR1_ADDR))) {
        return ret;
    }

    // Check if the slave responded or not
    if (i2c->SR1 & I2C_SR1_AF) {
        // Got a NACK
        i2c->CR1 |= I2C_CR1_STOP;
        i2c_wait_stop(i2c); // Don't leak errors from this call
        return -MP_ENODEV;
    }

    if (rd_wrn) {
        // For reading, set up ACK/NACK control based on number of bytes to read (at least 1 byte)
        if (next_len <= 1) {
            // NACK next received byte
            i2c->CR1 &= ~I2C_CR1_ACK;
        } else if (next_len <= 2) {
            // NACK second received byte
            i2c->CR1 |= I2C_CR1_POS;
            i2c->CR1 &= ~I2C_CR1_ACK;
        } else {
            // ACK next received byte
            i2c->CR1 |= I2C_CR1_ACK;
        }
    }

    // Read SR2 to clear SR1_ADDR
    uint32_t sr2 = i2c->SR2;
    (void)sr2;

    return 0;
}

// next_len = 0 or N (>=2)
int i2c_read(i2c_t *i2c, uint8_t *dest, size_t len, size_t next_len) {
    if (len == 0) {
        return -MP_EINVAL;
    }
    if (next_len == 1) {
        return -MP_EINVAL;
    }

    size_t remain = len + next_len;
    if (remain == 1) {
        // Special case
        i2c->CR1 |= I2C_CR1_STOP;
        int ret;
        if ((ret = i2c_wait_sr1_set(i2c, I2C_SR1_RXNE))) {
            return ret;
        }
        *dest = i2c->DR;
    } else {
        for (; len; --len) {
            remain = len + next_len;
            int ret;
            if ((ret = i2c_wait_sr1_set(i2c, I2C_SR1_BTF))) {
                return ret;
            }
            if (remain == 2) {
                // In this case next_len == 0 (it's not allowed to be 1)
                i2c->CR1 |= I2C_CR1_STOP;
                *dest++ = i2c->DR;
                *dest = i2c->DR;
                break;
            } else if (remain == 3) {
                // NACK next received byte
                i2c->CR1 &= ~I2C_CR1_ACK;
            }
            *dest++ = i2c->DR;
        }
    }

    if (!next_len) {
        // We sent a stop above, just wait for it to be finished
        return i2c_wait_stop(i2c);
    }

    return 0;
}

// next_len = 0 or N
int i2c_write(i2c_t *i2c, const uint8_t *src, size_t len, size_t next_len) {
    int ret;
    if ((ret = i2c_wait_sr1_set(i2c, I2C_SR1_AF | I2C_SR1_TXE))) {
        return ret;
    }

    // Write out the data
    int num_acks = 0;
    while (len--) {
        i2c->DR = *src++;
        if ((ret = i2c_wait_sr1_set(i2c, I2C_SR1_AF | I2C_SR1_BTF))) {
            return ret;
        }
        if (i2c->SR1 & I2C_SR1_AF) {
            // Slave did not respond to byte so stop sending
            break;
        }
        ++num_acks;
    }

    if (!next_len) {
        if (i2c->OAR1) {
            // Send a STOP and wait for it to finish
            i2c->CR1 |= I2C_CR1_STOP;
            if ((ret = i2c_wait_stop(i2c))) {
                return ret;
            }
        }
    }

    return num_acks;
}

#elif defined(STM32F0) || defined(STM32F7)

STATIC uint16_t i2c_timeout_ms[MICROPY_HW_MAX_I2C];

int i2c_init(i2c_t *i2c, mp_hal_pin_obj_t scl, mp_hal_pin_obj_t sda, uint32_t freq, uint16_t timeout_ms) {
    uint32_t i2c_id = ((uint32_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);

    // Init pins
    if (!mp_hal_pin_config_alt(scl, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_UP, AF_FN_I2C, i2c_id + 1)) {
        return -MP_EPERM;
    }
    if (!mp_hal_pin_config_alt(sda, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_UP, AF_FN_I2C, i2c_id + 1)) {
        return -MP_EPERM;
    }

    // Save timeout value
    i2c_timeout_ms[i2c_id] = timeout_ms;

    // Enable I2C peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN << i2c_id;
    volatile uint32_t tmp = RCC->APB1ENR; // delay after RCC clock enable
    (void)tmp;

    // Initialise I2C peripheral
    i2c->CR1 = 0;
    i2c->CR2 = 0;
    i2c->OAR1 = 0;
    i2c->OAR2 = 0;

    // These timing values are for f_I2CCLK=54MHz and are only approximate
    if (freq >= 1000000) {
        i2c->TIMINGR = 0x50100103;
    } else if (freq >= 400000) {
        i2c->TIMINGR = 0x70330309;
    } else if (freq >= 100000) {
        i2c->TIMINGR = 0xb0420f13;
    } else {
        return -MP_EINVAL;
    }

    i2c->TIMEOUTR = 0;

    return 0;
}

STATIC int i2c_wait_cr2_clear(i2c_t *i2c, uint32_t mask) {
    uint32_t i2c_id = ((uint32_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    uint32_t t0 = HAL_GetTick();
    while (i2c->CR2 & mask) {
        if (HAL_GetTick() - t0 >= i2c_timeout_ms[i2c_id]) {
            i2c->CR1 &= ~I2C_CR1_PE;
            return -MP_ETIMEDOUT;
        }
    }
    return 0;
}

STATIC int i2c_wait_isr_set(i2c_t *i2c, uint32_t mask) {
    uint32_t i2c_id = ((uint32_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);
    uint32_t t0 = HAL_GetTick();
    while (!(i2c->ISR & mask)) {
        if (HAL_GetTick() - t0 >= i2c_timeout_ms[i2c_id]) {
            i2c->CR1 &= ~I2C_CR1_PE;
            return -MP_ETIMEDOUT;
        }
    }
    return 0;
}

// len = 0, 1 or N
int i2c_start_addr(i2c_t *i2c, int rd_wrn, uint16_t addr, size_t len, bool stop) {
    // Enable the peripheral and send the START condition with slave address
    i2c->CR1 |= I2C_CR1_PE;
    i2c->CR2 = stop << I2C_CR2_AUTOEND_Pos
        | (len > 1) << I2C_CR2_RELOAD_Pos
        | (len > 0) << I2C_CR2_NBYTES_Pos
        | rd_wrn << I2C_CR2_RD_WRN_Pos
        | (addr & 0x7f) << 1;
    i2c->CR2 |= I2C_CR2_START;

    // Wait for address to be sent
    int ret;
    if ((ret = i2c_wait_cr2_clear(i2c, I2C_CR2_START))) {
        return ret;
    }

    // Check if the slave responded or not
    if (i2c->ISR & I2C_ISR_NACKF) {
        // If we get a NACK then I2C periph unconditionally sends a STOP
        i2c_wait_isr_set(i2c, I2C_ISR_STOPF); // Don't leak errors from this call
        i2c->CR1 &= ~I2C_CR1_PE;
        return -MP_ENODEV;
    }

    // Repurpose OAR1 to indicate that we loaded CR2
    i2c->OAR1 = 1;

    return 0;
}

STATIC int i2c_check_stop(i2c_t *i2c) {
    if (i2c->CR2 & I2C_CR2_AUTOEND) {
        // Wait for the STOP condition and then disable the peripheral
        int ret;
        if ((ret = i2c_wait_isr_set(i2c, I2C_ISR_STOPF))) {
            return ret;
        }
        i2c->CR1 &= ~I2C_CR1_PE;
    }

    return 0;
}

// next_len = 0 or N
int i2c_read(i2c_t *i2c, uint8_t *dest, size_t len, size_t next_len) {
    if (i2c->OAR1) {
        i2c->OAR1 = 0;
    } else {
        goto load_cr2;
    }

    // Read in the data
    while (len--) {
        int ret;
        if ((ret = i2c_wait_isr_set(i2c, I2C_ISR_RXNE))) {
            return ret;
        }
        *dest++ = i2c->RXDR;
    load_cr2:
        if (len) {
            i2c->CR2 = (i2c->CR2 & I2C_CR2_AUTOEND)
                | (len + next_len > 1) << I2C_CR2_RELOAD_Pos
                | 1 << I2C_CR2_NBYTES_Pos;
        }
    }

    if (!next_len) {
        int ret;
        if ((ret = i2c_check_stop(i2c))) {
            return ret;
        }
    }

    return 0;
}

// next_len = 0 or N
int i2c_write(i2c_t *i2c, const uint8_t *src, size_t len, size_t next_len) {
    int num_acks = 0;

    if (i2c->OAR1) {
        i2c->OAR1 = 0;
    } else {
        goto load_cr2;
    }

    // Write out the data
    while (len--) {
        int ret;
        if ((ret = i2c_wait_isr_set(i2c, I2C_ISR_TXE))) {
            return ret;
        }
        i2c->TXDR = *src++;
        if ((ret = i2c_wait_isr_set(i2c, I2C_ISR_TCR | I2C_ISR_TC | I2C_ISR_STOPF))) {
            return ret;
        }
        uint32_t isr = i2c->ISR;
        if (isr & I2C_ISR_NACKF) {
            // Slave did not respond to byte so stop sending
            if (!(isr & I2C_ISR_TXE)) {
                // The TXDR is still full so the byte previous to that wasn't actually ACK'd
                --num_acks;
            }
            break;
        }
        ++num_acks;
    load_cr2:
        if (len) {
            i2c->CR2 = (i2c->CR2 & I2C_CR2_AUTOEND)
                | (len + next_len > 1) << I2C_CR2_RELOAD_Pos
                | 1 << I2C_CR2_NBYTES_Pos;
        }
    }

    if (!next_len) {
        int ret;
        if ((ret = i2c_check_stop(i2c))) {
            return ret;
        }
    }

    return num_acks;
}

#endif

#if defined(STM32F0) || defined(STM32F4) || defined(STM32F7)

int i2c_readfrom(i2c_t *i2c, uint16_t addr, uint8_t *dest, size_t len, bool stop) {
    int ret;
    if ((ret = i2c_start_addr(i2c, 1, addr, len, stop))) {
        return ret;
    }
    return i2c_read(i2c, dest, len, 0);
}

int i2c_writeto(i2c_t *i2c, uint16_t addr, const uint8_t *src, size_t len, bool stop) {
    int ret;
    if ((ret = i2c_start_addr(i2c, 0, addr, len, stop))) {
        return ret;
    }
    return i2c_write(i2c, src, len, 0);
}

#endif

#endif // MICROPY_HW_ENABLE_HW_I2C
