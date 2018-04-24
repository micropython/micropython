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

#if defined(STM32F7)

#define I2C_POLL_TIMEOUT_MS (50)

int i2c_init(i2c_t *i2c, mp_hal_pin_obj_t scl, mp_hal_pin_obj_t sda, uint32_t freq) {
    uint32_t i2c_id = ((uint32_t)i2c - I2C1_BASE) / (I2C2_BASE - I2C1_BASE);

    // Init pins
    if (!mp_hal_pin_config_alt(scl, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_UP, AF_FN_I2C, i2c_id + 1)) {
        return -MP_EPERM;
    }
    if (!mp_hal_pin_config_alt(sda, MP_HAL_PIN_MODE_ALT_OPEN_DRAIN, MP_HAL_PIN_PULL_UP, AF_FN_I2C, i2c_id + 1)) {
        return -MP_EPERM;
    }

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
    uint32_t t0 = HAL_GetTick();
    while (i2c->CR2 & mask) {
        if (HAL_GetTick() - t0 >= I2C_POLL_TIMEOUT_MS) {
            i2c->CR1 &= ~I2C_CR1_PE;
            return -MP_ETIMEDOUT;
        }
    }
    return 0;
}

STATIC int i2c_wait_isr_set(i2c_t *i2c, uint32_t mask) {
    uint32_t t0 = HAL_GetTick();
    while (!(i2c->ISR & mask)) {
        if (HAL_GetTick() - t0 >= I2C_POLL_TIMEOUT_MS) {
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
        // If we get a NACK then I2C periph releases the bus, so don't send STOP
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
        if (i2c->ISR & I2C_ISR_NACKF) {
            // Slave did not respond to byte so stop sending
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

#endif // defined(STM32F7)

#endif // MICROPY_HW_ENABLE_HW_I2C
