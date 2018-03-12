/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_SHARED_DMA_H
#define MICROPY_INCLUDED_ATMEL_SAMD_SHARED_DMA_H

#include <stdbool.h>
#include <stdint.h>

#include "include/sam.h"

// We allocate DMA resources for the entire lifecycle of the board (not the
// vm) because the general_dma resource will be shared between the REPL and SPI
// flash. Both uses must block each other in order to prevent conflict.
#define AUDIO_DMA_CHANNEL_COUNT 3
#define DMA_CHANNEL_COUNT (AUDIO_DMA_CHANNEL_COUNT + 2)
#define SHARED_TX_CHANNEL (DMA_CHANNEL_COUNT - 2)
#define SHARED_RX_CHANNEL (DMA_CHANNEL_COUNT - 1)

volatile bool audio_dma_in_use;

void init_shared_dma(void);

#ifdef SAMD51
int32_t qspi_dma_write(uint32_t address, const uint8_t* buffer, uint32_t length);
int32_t qspi_dma_read(uint32_t address, uint8_t* buffer, uint32_t length);
#endif

int32_t sercom_dma_write(Sercom* sercom, const uint8_t* buffer, uint32_t length);
int32_t sercom_dma_read(Sercom* sercom, uint8_t* buffer, uint32_t length, uint8_t tx);
int32_t sercom_dma_transfer(Sercom* sercom, const uint8_t* buffer_out, uint8_t* buffer_in, uint32_t length);

void dma_configure(uint8_t channel_number, uint8_t trigsrc, bool output_event);
void dma_enable_channel(uint8_t channel_number);
void dma_disable_channel(uint8_t channel_number);
bool dma_channel_enabled(uint8_t channel_number);
uint8_t dma_transfer_status(uint8_t channel_number);
DmacDescriptor* dma_descriptor(uint8_t channel_number);

#endif  // MICROPY_INCLUDED_ATMEL_SAMD_SHARED_DMA_H
