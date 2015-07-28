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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include STM32_HAL_H

#include "dma.h"

#define NSTREAM (16)

static const uint8_t dma_irqn[NSTREAM] = {
    DMA1_Stream0_IRQn,
    DMA1_Stream1_IRQn,
    DMA1_Stream2_IRQn,
    DMA1_Stream3_IRQn,
    DMA1_Stream4_IRQn,
    DMA1_Stream5_IRQn,
    DMA1_Stream6_IRQn,
    DMA1_Stream7_IRQn,
    DMA2_Stream0_IRQn,
    DMA2_Stream1_IRQn,
    DMA2_Stream2_IRQn,
    DMA2_Stream3_IRQn,
    DMA2_Stream4_IRQn,
    DMA2_Stream5_IRQn,
    DMA2_Stream6_IRQn,
    DMA2_Stream7_IRQn,
};

// Default parameters to dma_init() shared by spi and i2c; Channel and Direction
// vary depending on the peripheral instance so they get passed separately
const DMA_InitTypeDef dma_init_struct_spi_i2c = {
    .Channel             = 0,
    .Direction           = 0,
    .PeriphInc           = DMA_PINC_DISABLE,
    .MemInc              = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
    .MemDataAlignment    = DMA_MDATAALIGN_BYTE,
    .Mode                = DMA_NORMAL,
    .Priority            = DMA_PRIORITY_LOW,
    .FIFOMode            = DMA_FIFOMODE_DISABLE,
    .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
    .MemBurst            = DMA_MBURST_INC4,
    .PeriphBurst         = DMA_PBURST_INC4
};

static DMA_HandleTypeDef *dma_handle[NSTREAM] = {NULL};
static uint32_t dma_last_channel[NSTREAM];

void DMA1_Stream0_IRQHandler(void) { if (dma_handle[0] != NULL) { HAL_DMA_IRQHandler(dma_handle[0]); } }
void DMA1_Stream1_IRQHandler(void) { if (dma_handle[1] != NULL) { HAL_DMA_IRQHandler(dma_handle[1]); } }
void DMA1_Stream2_IRQHandler(void) { if (dma_handle[2] != NULL) { HAL_DMA_IRQHandler(dma_handle[2]); } }
void DMA1_Stream3_IRQHandler(void) { if (dma_handle[3] != NULL) { HAL_DMA_IRQHandler(dma_handle[3]); } }
void DMA1_Stream4_IRQHandler(void) { if (dma_handle[4] != NULL) { HAL_DMA_IRQHandler(dma_handle[4]); } }
void DMA1_Stream5_IRQHandler(void) { if (dma_handle[5] != NULL) { HAL_DMA_IRQHandler(dma_handle[5]); } }
void DMA1_Stream6_IRQHandler(void) { if (dma_handle[6] != NULL) { HAL_DMA_IRQHandler(dma_handle[6]); } }
void DMA1_Stream7_IRQHandler(void) { if (dma_handle[7] != NULL) { HAL_DMA_IRQHandler(dma_handle[7]); } }
void DMA2_Stream0_IRQHandler(void) { if (dma_handle[8] != NULL) { HAL_DMA_IRQHandler(dma_handle[8]); } }
void DMA2_Stream1_IRQHandler(void) { if (dma_handle[9] != NULL) { HAL_DMA_IRQHandler(dma_handle[9]); } }
void DMA2_Stream2_IRQHandler(void) { if (dma_handle[10] != NULL) { HAL_DMA_IRQHandler(dma_handle[10]); } }
void DMA2_Stream3_IRQHandler(void) { if (dma_handle[11] != NULL) { HAL_DMA_IRQHandler(dma_handle[11]); } }
void DMA2_Stream4_IRQHandler(void) { if (dma_handle[12] != NULL) { HAL_DMA_IRQHandler(dma_handle[12]); } }
void DMA2_Stream5_IRQHandler(void) { if (dma_handle[13] != NULL) { HAL_DMA_IRQHandler(dma_handle[13]); } }
void DMA2_Stream6_IRQHandler(void) { if (dma_handle[14] != NULL) { HAL_DMA_IRQHandler(dma_handle[14]); } }
void DMA2_Stream7_IRQHandler(void) { if (dma_handle[15] != NULL) { HAL_DMA_IRQHandler(dma_handle[15]); } }

static int get_dma_id(DMA_Stream_TypeDef *dma_stream) {
    if ((uint32_t)dma_stream < DMA2_BASE) {
        return ((uint32_t)dma_stream - DMA1_Stream0_BASE) / 0x18;
    } else {
        return (NSTREAM / 2) + ((uint32_t)dma_stream - DMA2_Stream0_BASE) / 0x18;
    }
}

void dma_init(DMA_HandleTypeDef *dma, DMA_Stream_TypeDef *dma_stream, const DMA_InitTypeDef *dma_init, uint32_t dma_channel, uint32_t direction, void *data) {
    int dma_id = get_dma_id(dma_stream);
    //printf("dma_init(%p, %p(%d), 0x%x, 0x%x, %p)\n", dma, dma_stream, dma_id, (uint)dma_channel, (uint)direction, data);

    // TODO possibly don't need to clear the entire structure
    memset(dma, 0, sizeof(*dma));

    // set global pointer for IRQ handler
    dma_handle[dma_id] = dma;

    // initialise parameters
    dma->Instance = dma_stream;
    dma->Init = *dma_init;
    dma->Init.Direction = direction;
    dma->Init.Channel = dma_channel;

    // half of __HAL_LINKDMA(data, xxx, *dma)
    // caller must implement other half by doing: data->xxx = dma
    dma->Parent = data;

    // if this stream was previously configured for this channel then we
    // can skip most of the initialisation
    if (dma_last_channel[dma_id] == dma_channel) {
        goto same_channel;
    }
    dma_last_channel[dma_id] = dma_channel;

    // enable clock for needed DMA peripheral
    if (dma_id <= 7) {
        __DMA1_CLK_ENABLE();
    } else {
        __DMA2_CLK_ENABLE();
    }

    // reset and configure DMA peripheral
    HAL_DMA_DeInit(dma);
    HAL_DMA_Init(dma);
    HAL_NVIC_SetPriority(dma_irqn[dma_id], 6, 0);

same_channel:
    HAL_NVIC_EnableIRQ(dma_irqn[dma_id]);
}

void dma_deinit(DMA_HandleTypeDef *dma) {
    int dma_id = get_dma_id(dma->Instance);
    HAL_NVIC_DisableIRQ(dma_irqn[dma_id]);
    dma_handle[dma_id] = NULL;
}

void dma_invalidate_channel(DMA_Stream_TypeDef *dma_stream, uint32_t dma_channel) {
    int dma_id = get_dma_id(dma_stream);
    if (dma_last_channel[dma_id] == dma_channel) {
        dma_last_channel[dma_id] = 0xffffffff;
    }
}
