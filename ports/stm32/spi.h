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
#ifndef MICROPY_INCLUDED_STMHAL_SPI_H
#define MICROPY_INCLUDED_STMHAL_SPI_H

#include "dma.h"

typedef struct _spi_t {
    SPI_HandleTypeDef *spi;
    const dma_descr_t *tx_dma_descr;
    const dma_descr_t *rx_dma_descr;
} spi_t;

extern SPI_HandleTypeDef SPIHandle1;
extern SPI_HandleTypeDef SPIHandle2;
extern SPI_HandleTypeDef SPIHandle3;
extern SPI_HandleTypeDef SPIHandle4;
extern SPI_HandleTypeDef SPIHandle5;
extern SPI_HandleTypeDef SPIHandle6;

extern const spi_t spi_obj[6];

extern const mp_obj_type_t pyb_spi_type;
extern const mp_obj_type_t machine_soft_spi_type;
extern const mp_obj_type_t machine_hard_spi_type;

void spi_init0(void);
void spi_init(const spi_t *spi, bool enable_nss_pin);
const spi_t *spi_from_mp_obj(mp_obj_t o);

#endif // MICROPY_INCLUDED_STMHAL_SPI_H
