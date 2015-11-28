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
#include "py/obj.h"
#include "irq.h"

#define NSTREAMS_PER_CONTROLLER_LOG2 (3)
#define NSTREAMS_PER_CONTROLLER (1 << NSTREAMS_PER_CONTROLLER_LOG2)
#define NCONTROLLERS            (2)
#define NSTREAM                 (NCONTROLLERS * NSTREAMS_PER_CONTROLLER)

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
static uint8_t  dma_last_channel[NSTREAM];
static volatile uint32_t dma_enable_mask = 0;

volatile dma_idle_count_t dma_idle;

#define DMA1_ENABLE_MASK    0x00ff  // Bits in dma_enable_mask corresponfing to DMA1
#define DMA2_ENABLE_MASK    0xff00  // Bits in dma_enable_mask corresponding to DMA2
#define DMA_INVALID_CHANNEL 0xff    // Value stored in dma_last_channel which means invalid

#define DMA_CHANNEL_AS_UINT8(dma_channel)   (((dma_channel) & DMA_SxCR_CHSEL) >> 24)

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

#define DMA1_IS_CLK_ENABLED()   ((RCC->AHB1ENR & RCC_AHB1ENR_DMA1EN) != 0)
#define DMA2_IS_CLK_ENABLED()   ((RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN) != 0)

static int get_dma_id(DMA_Stream_TypeDef *dma_stream) {
    int dma_id;
    if (dma_stream < DMA2_Stream0) {
        dma_id = dma_stream - DMA1_Stream0;
    } else {
        dma_id =  NSTREAMS_PER_CONTROLLER + (dma_stream - DMA2_Stream0);
    }
    return dma_id;
}

// Resets the idle counter for the DMA controller associated with dma_id.
static void dma_tickle(int dma_id) {
    dma_idle.counter[(dma_id >> NSTREAMS_PER_CONTROLLER_LOG2) & 1] = 1;
}

static void dma_enable_clock(int dma_id) {
    // We don't want dma_tick_handler() to turn off the clock right after we
    // enable it, so we need to mark the channel in use in an atomic fashion.
    mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint32_t old_enable_mask = dma_enable_mask;
    dma_enable_mask |= (1 << dma_id);
    MICROPY_END_ATOMIC_SECTION(irq_state);

    if (dma_id <= 7) {
        if (((old_enable_mask & DMA1_ENABLE_MASK) == 0) && !DMA1_IS_CLK_ENABLED()) {
            __DMA1_CLK_ENABLE();

            // We just turned on the clock. This means that anything stored
            // in dma_last_channel (for DMA1) needs to be invalidated.

            for (int channel = 0; channel < NSTREAMS_PER_CONTROLLER; channel++) {
                dma_last_channel[channel] = DMA_INVALID_CHANNEL;
            }
        }
    } else {
        if (((old_enable_mask & DMA2_ENABLE_MASK) == 0) && !DMA2_IS_CLK_ENABLED()) {
            __DMA2_CLK_ENABLE();

            // We just turned on the clock. This means that anything stored
            // in dma_last_channel (for DMA1) needs to be invalidated.

            for (int channel = NSTREAMS_PER_CONTROLLER; channel < NSTREAM; channel++) {
                dma_last_channel[channel] = DMA_INVALID_CHANNEL;
            }
        }
    }
}

static void dma_disable_clock(int dma_id) {
    // We just mark the clock as disabled here, but we don't actually disable it.
    // We wait for the timer to expire first, which means that back-to-back
    // transfers don't have to initialize as much.
    dma_tickle(dma_id);
    dma_enable_mask &= ~(1 << dma_id);
}

void dma_init(DMA_HandleTypeDef *dma, DMA_Stream_TypeDef *dma_stream, const DMA_InitTypeDef *dma_init, uint32_t dma_channel, uint32_t direction, void *data) {
    int dma_id = get_dma_id(dma_stream);
    //printf("dma_init(%p, %p(%d), 0x%x, 0x%x, %p)\n", dma, dma_stream, dma_id, (uint)dma_channel, (uint)direction, data);

    // Some drivers allocate the DMA_HandleTypeDef from the stack
    // (i.e. dac, i2c, spi) and for those cases we need to clear the
    // structure so we don't get random values from the stack)
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

    dma_enable_clock(dma_id);

    // if this stream was previously configured for this channel then we
    // can skip most of the initialisation
    uint8_t channel_uint8 = DMA_CHANNEL_AS_UINT8(dma_channel);
    if (dma_last_channel[dma_id] == channel_uint8) {
        goto same_channel;
    }
    dma_last_channel[dma_id] = channel_uint8;

    // reset and configure DMA peripheral
    if (HAL_DMA_GetState(dma) != HAL_DMA_STATE_RESET) {
        HAL_DMA_DeInit(dma);
    }
    HAL_DMA_Init(dma);
    HAL_NVIC_SetPriority(dma_irqn[dma_id], IRQ_PRI_DMA, IRQ_SUBPRI_DMA);

same_channel:
    HAL_NVIC_EnableIRQ(dma_irqn[dma_id]);
}

void dma_deinit(DMA_HandleTypeDef *dma) {
    int dma_id = get_dma_id(dma->Instance);
    HAL_NVIC_DisableIRQ(dma_irqn[dma_id]);
    dma_handle[dma_id] = NULL;

    dma_disable_clock(dma_id);
}

void dma_invalidate_channel(DMA_Stream_TypeDef *dma_stream, uint32_t dma_channel) {
    int dma_id = get_dma_id(dma_stream);
    if (dma_last_channel[dma_id] == DMA_CHANNEL_AS_UINT8(dma_channel)) {
        dma_last_channel[dma_id] = DMA_INVALID_CHANNEL;
    }
}

// Called from the SysTick handler
// We use LSB of tick to select which controller to process
void dma_idle_handler(int tick) {
    static const uint32_t   controller_mask[] = {
        DMA1_ENABLE_MASK, DMA2_ENABLE_MASK
    };
    {
        int controller = tick & 1;
        if (dma_idle.counter[controller] == 0) {
            return;
        }
        if (++dma_idle.counter[controller] > DMA_IDLE_TICK_MAX) {
            if ((dma_enable_mask & controller_mask[controller]) == 0) {
                // Nothing is active and we've reached our idle timeout,
                // Now we'll really disable the clock.
                dma_idle.counter[controller] = 0;
                if (controller == 0) {
                    __DMA1_CLK_DISABLE();
                } else {
                    __DMA2_CLK_DISABLE();
                }
            } else {
                // Something is still active, but the counter never got
                // reset, so we'll reset the counter here.
                dma_idle.counter[controller] = 1;
            }
        }
    }
}
