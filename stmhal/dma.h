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

//TODO: Put stream/channel defs for i2c/spi/can, etc here
#define DMA_STREAM_SDIO_RX      DMA2_Stream3
#define DMA_CHANNEL_SDIO_RX     DMA_CHANNEL_4

#define DMA_STREAM_SDIO_TX      DMA2_Stream6
#define DMA_CHANNEL_SDIO_TX     DMA_CHANNEL_4

extern const DMA_InitTypeDef dma_init_struct_spi_i2c;

void dma_init(DMA_HandleTypeDef *dma, DMA_Stream_TypeDef *dma_stream, const DMA_InitTypeDef *dma_init, uint32_t dma_channel, uint32_t direction, void *data);
void dma_deinit(DMA_HandleTypeDef *dma);
void dma_invalidate_channel(DMA_Stream_TypeDef *dma_stream, uint32_t dma_channel);
