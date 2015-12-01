/*
 * This file is part of the Micro Python project, http://micropython.org/
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

// These are ordered by DMAx_Stream number, and within a stream by channel
// number. The duplicate streams are ok as long as they aren't used at the
// same time.
//
// Currently I2C and SPI are synchronous and they call dma_init/dma_deinit
// around each transfer.

// DMA1 streams

#define DMA_STREAM_I2C1_RX      DMA1_Stream0
#define DMA_CHANNEL_I2C1_RX                     DMA_CHANNEL_1

#define DMA_STREAM_SPI3_RX      DMA1_Stream2
#define DMA_CHANNEL_SPI3_RX                     DMA_CHANNEL_0

#define DMA_STREAM_I2C3_RX      DMA1_Stream2
#define DMA_CHANNEL_I2C3_RX                     DMA_CHANNEL_3

#define DMA_STREAM_I2C2_RX      DMA1_Stream2
#define DMA_CHANNEL_I2C2_RX                     DMA_CHANNEL_7

#define DMA_STREAM_SPI2_RX      DMA1_Stream3
#define DMA_CHANNEL_SPI2_RX                     DMA_CHANNEL_0

#define DMA_STREAM_SPI2_TX      DMA1_Stream4
#define DMA_CHANNEL_SPI2_TX                     DMA_CHANNEL_0

#define DMA_STREAM_I2C3_TX      DMA1_Stream4
#define DMA_CHANNEL_I2C3_TX                     DMA_CHANNEL_3

#define DMA_STREAM_DAC1         DMA1_Stream5
#define DMA_CHANNEL_DAC1                        DMA_CHANNEL_7

#define DMA_STREAM_DAC2         DMA1_Stream6
#define DMA_CHANNEL_DAC2                        DMA_CHANNEL_7

#define DMA_STREAM_SPI3_TX      DMA1_Stream7
#define DMA_CHANNEL_SPI3_TX                     DMA_CHANNEL_0

#define DMA_STREAM_I2C1_TX      DMA1_Stream7
#define DMA_CHANNEL_I2C1_TX                     DMA_CHANNEL_1

#define DMA_STREAM_I2C2_TX      DMA1_Stream7
#define DMA_CHANNEL_I2C2_TX                     DMA_CHANNEL_7

// DMA2 streams

#define DMA_STREAM_SPI1_RX      DMA2_Stream2
#define DMA_CHANNEL_SPI1_RX                     DMA_CHANNEL_3

#define DMA_STREAM_SDIO_RX      DMA2_Stream3
#define DMA_CHANNEL_SDIO_RX                     DMA_CHANNEL_4

#define DMA_STREAM_SPI1_TX      DMA2_Stream5
#define DMA_CHANNEL_SPI1_TX                     DMA_CHANNEL_3

#define DMA_STREAM_SDIO_TX      DMA2_Stream6
#define DMA_CHANNEL_SDIO_TX                     DMA_CHANNEL_4

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

extern const DMA_InitTypeDef dma_init_struct_spi_i2c;

void dma_init(DMA_HandleTypeDef *dma, DMA_Stream_TypeDef *dma_stream, const DMA_InitTypeDef *dma_init, uint32_t dma_channel, uint32_t direction, void *data);
void dma_deinit(DMA_HandleTypeDef *dma);
void dma_invalidate_channel(DMA_Stream_TypeDef *dma_stream, uint32_t dma_channel);
void dma_idle_handler(int controller);
