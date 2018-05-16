/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_I2C_H
#define MICROPY_INCLUDED_STM32_I2C_H

#include "dma.h"

// use this for OwnAddress1 to configure I2C in master mode
#define PYB_I2C_MASTER_ADDRESS (0xfe)

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    I2C_HandleTypeDef *i2c;
    const dma_descr_t *tx_dma_descr;
    const dma_descr_t *rx_dma_descr;
    bool *use_dma;
} pyb_i2c_obj_t;

extern I2C_HandleTypeDef I2CHandle1;
extern I2C_HandleTypeDef I2CHandle2;
extern I2C_HandleTypeDef I2CHandle3;
extern I2C_HandleTypeDef I2CHandle4;
extern const mp_obj_type_t pyb_i2c_type;
extern const pyb_i2c_obj_t pyb_i2c_obj[4];

void i2c_init0(void);
void pyb_i2c_init(I2C_HandleTypeDef *i2c);
void pyb_i2c_init_freq(const pyb_i2c_obj_t *self, mp_int_t freq);
uint32_t pyb_i2c_get_baudrate(I2C_HandleTypeDef *i2c);
void i2c_ev_irq_handler(mp_uint_t i2c_id);
void i2c_er_irq_handler(mp_uint_t i2c_id);

typedef I2C_TypeDef i2c_t;

int i2c_init(i2c_t *i2c, mp_hal_pin_obj_t scl, mp_hal_pin_obj_t sda, uint32_t freq);
int i2c_start_addr(i2c_t *i2c, int rd_wrn, uint16_t addr, size_t len, bool stop);
int i2c_read(i2c_t *i2c, uint8_t *dest, size_t len, size_t next_len);
int i2c_write(i2c_t *i2c, const uint8_t *src, size_t len, size_t next_len);
int i2c_readfrom(i2c_t *i2c, uint16_t addr, uint8_t *dest, size_t len, bool stop);
int i2c_writeto(i2c_t *i2c, uint16_t addr, const uint8_t *src, size_t len, bool stop);

#endif // MICROPY_INCLUDED_STM32_I2C_H
