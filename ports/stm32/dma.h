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

typedef struct _dma_descr_t dma_descr_t;

#if defined(STM32F4) || defined(STM32F7)

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
extern const dma_descr_t dma_SDMMC_2_RX;
extern const dma_descr_t dma_SPI_1_RX;
extern const dma_descr_t dma_SPI_5_RX;
extern const dma_descr_t dma_SDIO_0_RX;
extern const dma_descr_t dma_SPI_4_RX;
extern const dma_descr_t dma_SPI_5_TX;
extern const dma_descr_t dma_SPI_4_TX;
extern const dma_descr_t dma_SPI_6_TX;
extern const dma_descr_t dma_SPI_1_TX;
extern const dma_descr_t dma_SDMMC_2_TX;
extern const dma_descr_t dma_SPI_6_RX;
extern const dma_descr_t dma_SDIO_0_TX;

#elif defined(STM32L4)

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
extern const dma_descr_t dma_SDIO_0_TX;
extern const dma_descr_t dma_SDIO_0_RX;

#endif

typedef union {
    uint16_t    enabled;    // Used to test if both counters are == 0
    uint8_t     counter[2];
} dma_idle_count_t;
extern volatile dma_idle_count_t dma_idle;
#define DMA_IDLE_ENABLED()  (dma_idle.enabled != 0)

#define DMA_SYSTICK_MASK            0x0e
#define DMA_MSECS_PER_SYSTICK       (DMA_SYSTICK_MASK + 1)
#define DMA_IDLE_TICK_MAX           (8)     // 128 msec
#define DMA_IDLE_TICK(tick)         (((tick) & DMA_SYSTICK_MASK) == 0)


void dma_init(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, void *data);
void dma_init_handle(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, void *data);
void dma_deinit(const dma_descr_t *dma_descr);
void dma_invalidate_channel(const dma_descr_t *dma_descr);
void dma_idle_handler(int controller);

#endif // MICROPY_INCLUDED_STM32_DMA_H
