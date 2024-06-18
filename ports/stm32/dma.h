/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_DMA_H
#define MICROPY_INCLUDED_STM32_DMA_H

#include "py/mpconfig.h"

typedef struct _dma_descr_t dma_descr_t;

#if defined(STM32H5)
// STM32H5 GPDMA doesn't feature circular mode directly, so define doesn't exist in
// stm32 driver header. Define it here to make users like DAC driver happy.
#define DMA_CIRCULAR 0x00000001
#endif

#if defined(STM32F0) || defined(STM32F4) || defined(STM32F7) || defined(STM32G0) || defined(STM32H5) || defined(STM32H7)

extern const dma_descr_t dma_I2C_1_RX;
extern const dma_descr_t dma_SPI_3_RX;
extern const dma_descr_t dma_I2C_4_RX;
extern const dma_descr_t dma_I2C_3_RX;
extern const dma_descr_t dma_I2C_2_RX;
extern const dma_descr_t dma_SPI_2_RX;
extern const dma_descr_t dma_SPI_2_TX;
extern const dma_descr_t dma_I2C_3_TX;
extern const dma_descr_t dma_I2C_4_TX;
extern const dma_descr_t dma_DAC_1_TX;
extern const dma_descr_t dma_DAC_2_TX;
extern const dma_descr_t dma_SPI_3_TX;
extern const dma_descr_t dma_I2C_1_TX;
extern const dma_descr_t dma_I2C_2_TX;
extern const dma_descr_t dma_SDMMC_2;
extern const dma_descr_t dma_SPI_1_RX;
extern const dma_descr_t dma_SPI_5_RX;
extern const dma_descr_t dma_SDIO_0;
extern const dma_descr_t dma_SPI_4_RX;
extern const dma_descr_t dma_SPI_5_TX;
extern const dma_descr_t dma_SPI_4_TX;
extern const dma_descr_t dma_SPI_6_TX;
extern const dma_descr_t dma_SPI_1_TX;
extern const dma_descr_t dma_SDMMC_2;
extern const dma_descr_t dma_SPI_6_RX;
extern const dma_descr_t dma_SDIO_0;
extern const dma_descr_t dma_DCMI_0;
extern const dma_descr_t dma_I2S_1_RX;
extern const dma_descr_t dma_I2S_1_TX;
extern const dma_descr_t dma_I2S_2_RX;
extern const dma_descr_t dma_I2S_2_TX;

#elif defined(STM32G4)

extern const dma_descr_t dma_SPI_1_RX;
extern const dma_descr_t dma_SPI_1_TX;
extern const dma_descr_t dma_SPI_2_RX;
extern const dma_descr_t dma_SPI_2_TX;
extern const dma_descr_t dma_I2C_1_RX;
extern const dma_descr_t dma_I2C_1_TX;
extern const dma_descr_t dma_I2C_2_RX;
extern const dma_descr_t dma_I2C_2_TX;
extern const dma_descr_t dma_I2C_3_RX;
extern const dma_descr_t dma_I2C_3_TX;
extern const dma_descr_t dma_UART_3_RX;
extern const dma_descr_t dma_UART_3_TX;
extern const dma_descr_t dma_DAC_1_TX;
extern const dma_descr_t dma_DAC_2_TX;
extern const dma_descr_t dma_UART_1_RX;
extern const dma_descr_t dma_UART_1_TX;
extern const dma_descr_t dma_LPUART_1_RX;
extern const dma_descr_t dma_LPUART_1_TX;
extern const dma_descr_t dma_ADC_1;
extern const dma_descr_t dma_MEM_2_MEM;

#elif defined(STM32L0)

extern const dma_descr_t dma_SPI_1_RX;
extern const dma_descr_t dma_I2C_3_TX;
extern const dma_descr_t dma_SPI_1_TX;
extern const dma_descr_t dma_I2C_3_RX;
extern const dma_descr_t dma_DAC_1_TX;
extern const dma_descr_t dma_SPI_2_RX;
extern const dma_descr_t dma_I2C_2_TX;
extern const dma_descr_t dma_DAC_2_TX;
extern const dma_descr_t dma_SPI_2_TX;
extern const dma_descr_t dma_I2C_2_RX;
extern const dma_descr_t dma_I2C_1_TX;
extern const dma_descr_t dma_I2C_1_RX;

#elif defined(STM32L1)
extern const dma_descr_t dma_SPI_1_RX;
extern const dma_descr_t dma_SPI_3_TX;
extern const dma_descr_t dma_SPI_1_TX;
extern const dma_descr_t dma_SPI_3_RX;
extern const dma_descr_t dma_DAC_1_TX;
extern const dma_descr_t dma_SPI_2_RX;
extern const dma_descr_t dma_I2C_2_TX;
extern const dma_descr_t dma_DAC_2_TX;
extern const dma_descr_t dma_SPI_2_TX;
extern const dma_descr_t dma_I2C_2_RX;
extern const dma_descr_t dma_I2C_1_TX;
extern const dma_descr_t dma_I2C_1_RX;

#elif defined(STM32L4) || defined(STM32WB) || defined(STM32WL)

extern const dma_descr_t dma_ADC_1_RX;
extern const dma_descr_t dma_ADC_2_RX;
extern const dma_descr_t dma_SPI_1_RX;
extern const dma_descr_t dma_I2C_3_TX;
extern const dma_descr_t dma_ADC_3_RX;
extern const dma_descr_t dma_SPI_1_TX;
extern const dma_descr_t dma_I2C_3_RX;
extern const dma_descr_t dma_DAC_1_TX;
extern const dma_descr_t dma_SPI_2_RX;
extern const dma_descr_t dma_I2C_2_TX;
extern const dma_descr_t dma_DAC_2_TX;
extern const dma_descr_t dma_SPI_2_TX;
extern const dma_descr_t dma_I2C_2_RX;
extern const dma_descr_t dma_I2C_1_TX;
extern const dma_descr_t dma_I2C_1_RX;
extern const dma_descr_t dma_SPI_3_RX;
extern const dma_descr_t dma_SPI_3_TX;
extern const dma_descr_t dma_SDIO_0;
extern const dma_descr_t dma_I2C_4_TX;
extern const dma_descr_t dma_I2C_4_RX;
extern const dma_descr_t dma_SPI_SUBGHZ_TX;
extern const dma_descr_t dma_SPI_SUBGHZ_RX;

#elif defined(STM32N6)

extern const dma_descr_t dma_SPI_1_RX;
extern const dma_descr_t dma_SPI_1_TX;
extern const dma_descr_t dma_SPI_2_RX;
extern const dma_descr_t dma_SPI_2_TX;
extern const dma_descr_t dma_SPI_3_RX;
extern const dma_descr_t dma_SPI_3_TX;
extern const dma_descr_t dma_SPI_4_RX;
extern const dma_descr_t dma_SPI_4_TX;
extern const dma_descr_t dma_SPI_5_RX;
extern const dma_descr_t dma_SPI_5_TX;

#endif

// API that configures the DMA via the HAL.
void dma_init(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, uint32_t dir, void *data);
void dma_init_handle(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, uint32_t dir, void *data);
void dma_deinit(const dma_descr_t *dma_descr);
void dma_invalidate_channel(const dma_descr_t *dma_descr);

// API that configures the DMA directly and does not use the HAL.
void dma_nohal_init(const dma_descr_t *descr, uint32_t config);
void dma_nohal_deinit(const dma_descr_t *descr);
void dma_nohal_start(const dma_descr_t *descr, uint32_t src_addr, uint32_t dst_addr, uint16_t len);

// API to be used if DMA is controlled externally, to ensure the clock remains enabled.
// - controller: should be 0 or 1, corresponding to DMA1 or DMA2
// - stream: should be 0 to N, corresponding to Stream0..StreamN, or Channel1..ChannelN
void dma_external_acquire(uint32_t controller, uint32_t stream);
void dma_external_release(uint32_t controller, uint32_t stream);

#if __DCACHE_PRESENT
// On chips with D-Cache, it's necessary to call this function before using DMA to read
// into a user-supplied buffer. It does two things:
//
// 1) Cleans this region in the cache.
//
// 2) Temporarily disables caching for the first and last cache lines of the
// buffer. This prevents cache coherency issues if the start or end of the
// buffer don't align to a cache line. (For example, this can happen if the CPU
// accesses unrelated memory adjacent to the buffer and in the same cache
// line.)
//
// Only one region can be protected in this way at a time.
void dma_protect_rx_region(void *dest, size_t len);

// Release the region protected by dma_protect_rx_region().
//
// Call this after the DMA operation completes, before the CPU reads any of the
// data that was written.
//
// As well as restoring data caching, this function invalidates D-cache for the
// entire specified region.
void dma_unprotect_rx_region(void *dest, size_t len);

#else

inline static void dma_protect_rx_region(uint8_t *dest, size_t len) {
    // No-ops on targets without D-Cache.
}

inline static void dma_unprotect_rx_region(void *dest, size_t len) {

}

#endif // __DCACHE_PRESENT

#endif // MICROPY_INCLUDED_STM32_DMA_H
