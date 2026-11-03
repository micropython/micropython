/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_PSOC_EDGE_SPI_H
#define MICROPY_INCLUDED_PSOC_EDGE_SPI_H

#include "sys_int.h"
#include "py/obj.h"
#include "py/mphal.h"

#include "cy_scb_spi.h"

#include "clk.h"
#include "scb.h"

#define SPI_SUPPORTED_BITS (8U)

typedef struct _spi_obj_t {
    uint8_t id;
    scb_obj_t *scb_obj;
    pclk_div_obj_t *pclk_div;
    cy_stc_scb_spi_context_t ctx;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
} spi_obj_t;

uint8_t spi_get_id(spi_obj_t *self);

void spi_alloc(spi_obj_t *self, uint8_t id, mp_obj_t parent, void (*scb_isr)(mp_obj_t), bool *is_new);
void spi_free(spi_obj_t *self);

void spi_hw_irq(spi_obj_t *self);

void spi_hw_controller_init(spi_obj_t *self, uint32_t baudrate);
cy_en_scb_spi_status_t spi_hw_controller_transfer(spi_obj_t *self, const uint8_t *tx_buf, uint8_t *rx_buf, size_t len);
bool spi_hw_controller_is_transfer_active(spi_obj_t *self);
void spi_hw_controller_abort_transfer(spi_obj_t *self);

void spi_hw_deinit(spi_obj_t *self);

#endif // MICROPY_INCLUDED_PSOC_EDGE_SPI_H
