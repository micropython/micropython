/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Damien P. George
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

#include "py/obj.h"
#include "py/mphal.h"
#include "systick.h"
#include "dma.h"
#include "irq.h"

#define DMA_IDLE_ENABLED()  (dma_idle.enabled != 0)
#define DMA_SYSTICK_LOG2    (3)
#define DMA_SYSTICK_MASK    ((1 << DMA_SYSTICK_LOG2) - 1)
#define DMA_IDLE_TICK_MAX   (8) // 8*8 = 64 msec
#define DMA_IDLE_TICK(tick) (((tick) & ~(SYSTICK_DISPATCH_NUM_SLOTS - 1) & DMA_SYSTICK_MASK) == 0)

#define ENABLE_SDIO (MICROPY_HW_ENABLE_SDCARD || MICROPY_HW_ENABLE_MMCARD || MICROPY_PY_NETWORK_CYW43)

typedef enum {
    dma_id_not_defined=-1,
    dma_id_0,
    dma_id_1,
    dma_id_2,
    dma_id_3,
    dma_id_4,
    dma_id_5,
    dma_id_6,
    dma_id_7,
    dma_id_8,
    dma_id_9,
    dma_id_10,
    dma_id_11,
    dma_id_12,
    dma_id_13,
    dma_id_14,
    dma_id_15,
} dma_id_t;

typedef union {
    uint16_t enabled; // Used to test if both counters are == 0
    uint8_t counter[2];
} dma_idle_count_t;

struct _dma_descr_t {
    #if defined(STM32F4) || defined(STM32F7) || defined(STM32H7)
    DMA_Stream_TypeDef *instance;
    #elif defined(STM32F0) || defined(STM32L0) || defined(STM32L4) || defined(STM32WB)
    DMA_Channel_TypeDef *instance;
    #else
    #error "Unsupported Processor"
    #endif
    uint32_t sub_instance;
    dma_id_t id;
    const DMA_InitTypeDef *init;
};

// Default parameters to dma_init() shared by spi and i2c; Channel and Direction
// vary depending on the peripheral instance so they get passed separately
static const DMA_InitTypeDef dma_init_struct_spi_i2c = {
    #if defined(STM32F4) || defined(STM32F7)
    .Channel = 0,
    #elif defined(STM32H7) || defined(STM32L0) || defined(STM32L4) || defined(STM32WB)
    .Request = 0,
    #endif
    .Direction = 0,
    .PeriphInc = DMA_PINC_DISABLE,
    .MemInc = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
    .MemDataAlignment = DMA_MDATAALIGN_BYTE,
    .Mode = DMA_NORMAL,
    .Priority = DMA_PRIORITY_LOW,
    #if defined(STM32F4) || defined(STM32F7) || defined(STM32H7)
    .FIFOMode = DMA_FIFOMODE_DISABLE,
    .FIFOThreshold = DMA_FIFO_THRESHOLD_FULL,
    .MemBurst = DMA_MBURST_INC4,
    .PeriphBurst = DMA_PBURST_INC4
    #endif
};

#if MICROPY_HW_ENABLE_I2S
// Default parameters to dma_init() for i2s; Channel and Direction
// vary depending on the peripheral instance so they get passed separately
static const DMA_InitTypeDef dma_init_struct_i2s = {
    #if defined(STM32F4) || defined(STM32F7)
    .Channel = 0,
    #elif defined(STM32H7) || defined(STM32L0) || defined(STM32L4)
    .Request = 0,
    #endif
    .Direction = DMA_MEMORY_TO_PERIPH,
    .PeriphInc = DMA_PINC_DISABLE,
    .MemInc = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD,
    .MemDataAlignment = DMA_MDATAALIGN_HALFWORD,
    .Mode = DMA_CIRCULAR,
    .Priority = DMA_PRIORITY_LOW,
    #if defined(STM32F4) || defined(STM32F7) || defined(STM32H7)
    .FIFOMode = DMA_FIFOMODE_DISABLE,
    .FIFOThreshold = DMA_FIFO_THRESHOLD_FULL,
    .MemBurst = DMA_MBURST_SINGLE,
    .PeriphBurst = DMA_PBURST_SINGLE
    #endif
};
#endif

#if ENABLE_SDIO && !defined(STM32H7)
// Parameters to dma_init() for SDIO tx and rx.
static const DMA_InitTypeDef dma_init_struct_sdio = {
    #if defined(STM32F4) || defined(STM32F7)
    .Channel = 0,
    #elif defined(STM32L0) || defined(STM32L4) || defined(STM32WB)
    .Request = 0,
    #endif
    .Direction = 0,
    .PeriphInc = DMA_PINC_DISABLE,
    .MemInc = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_WORD,
    .MemDataAlignment = DMA_MDATAALIGN_WORD,
    #if defined(STM32F4) || defined(STM32F7)
    .Mode = DMA_PFCTRL,
    #elif defined(STM32L0) || defined(STM32L4) || defined(STM32WB)
    .Mode = DMA_NORMAL,
    #endif
    .Priority = DMA_PRIORITY_VERY_HIGH,
    #if defined(STM32F4) || defined(STM32F7)
    .FIFOMode = DMA_FIFOMODE_ENABLE,
    .FIFOThreshold = DMA_FIFO_THRESHOLD_FULL,
    .MemBurst = DMA_MBURST_INC4,
    .PeriphBurst = DMA_PBURST_INC4,
    #endif
};
#endif

#if defined(MICROPY_HW_ENABLE_DAC) && MICROPY_HW_ENABLE_DAC
// Default parameters to dma_init() for DAC tx
static const DMA_InitTypeDef dma_init_struct_dac = {
    #if defined(STM32F4) || defined(STM32F7)
    .Channel = 0,
    #elif defined(STM32H7) || defined(STM32L0) || defined(STM32L4) || defined(STM32WB)
    .Request = 0,
    #endif
    .Direction = 0,
    .PeriphInc = DMA_PINC_DISABLE,
    .MemInc = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
    .MemDataAlignment = DMA_MDATAALIGN_BYTE,
    .Mode = DMA_NORMAL,
    .Priority = DMA_PRIORITY_HIGH,
    #if defined(STM32F4) || defined(STM32F7) || defined(STM32H7)
    .FIFOMode = DMA_FIFOMODE_DISABLE,
    .FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL,
    .MemBurst = DMA_MBURST_SINGLE,
    .PeriphBurst = DMA_PBURST_SINGLE,
    #endif
};
#endif

#if MICROPY_HW_ENABLE_DCMI
static const DMA_InitTypeDef dma_init_struct_dcmi = {
    #if defined(STM32H7)
    .Request = DMA_REQUEST_DCMI,
    #else
    .Channel = DMA_CHANNEL_1,
    #endif
    .Direction = DMA_PERIPH_TO_MEMORY,
    .PeriphInc = DMA_PINC_DISABLE,
    .MemInc = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_WORD,
    .MemDataAlignment = DMA_MDATAALIGN_WORD,
    .Mode = DMA_NORMAL,
    .Priority = DMA_PRIORITY_HIGH,
    .FIFOMode = DMA_FIFOMODE_ENABLE,
    .FIFOThreshold = DMA_FIFO_THRESHOLD_FULL,
    .MemBurst = DMA_MBURST_INC4,
    .PeriphBurst = DMA_PBURST_SINGLE
};
#endif

#if defined(STM32F0)

#define NCONTROLLERS            (2)
#define NSTREAMS_PER_CONTROLLER (7)
#define NSTREAM                 (NCONTROLLERS * NSTREAMS_PER_CONTROLLER)

#define DMA_SUB_INSTANCE_AS_UINT8(dma_channel) ((dma_channel) >> ((dma_channel >> 28) * 4))

#define DMA1_ENABLE_MASK (0x007f) // Bits in dma_enable_mask corresponding to DMA1 (7 channels)
#define DMA2_ENABLE_MASK (0x0f80) // Bits in dma_enable_mask corresponding to DMA2 (only 5 channels)

// DMA1 streams
#if MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_1_TX = { DMA1_Channel3, HAL_DMA1_CH3_DAC_CH1, dma_id_2, &dma_init_struct_dac };
const dma_descr_t dma_DAC_2_TX = { DMA1_Channel4, HAL_DMA1_CH4_DAC_CH2, dma_id_3, &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_2_TX = { DMA1_Channel5, HAL_DMA1_CH5_SPI2_TX, dma_id_4, &dma_init_struct_spi_i2c};
const dma_descr_t dma_SPI_2_RX = { DMA1_Channel6, HAL_DMA1_CH6_SPI2_RX, dma_id_5, &dma_init_struct_spi_i2c};
const dma_descr_t dma_SPI_1_RX = { DMA2_Channel3, HAL_DMA2_CH3_SPI1_RX, dma_id_9, &dma_init_struct_spi_i2c};
const dma_descr_t dma_SPI_1_TX = { DMA2_Channel4, HAL_DMA2_CH4_SPI1_TX, dma_id_10, &dma_init_struct_spi_i2c};

static const uint8_t dma_irqn[NSTREAM] = {
    DMA1_Ch1_IRQn,
    DMA1_Ch2_3_DMA2_Ch1_2_IRQn,
    DMA1_Ch2_3_DMA2_Ch1_2_IRQn,
    DMA1_Ch4_7_DMA2_Ch3_5_IRQn,
    DMA1_Ch4_7_DMA2_Ch3_5_IRQn,
    DMA1_Ch4_7_DMA2_Ch3_5_IRQn,
    DMA1_Ch4_7_DMA2_Ch3_5_IRQn,

    DMA1_Ch2_3_DMA2_Ch1_2_IRQn,
    DMA1_Ch2_3_DMA2_Ch1_2_IRQn,
    DMA1_Ch4_7_DMA2_Ch3_5_IRQn,
    DMA1_Ch4_7_DMA2_Ch3_5_IRQn,
    DMA1_Ch4_7_DMA2_Ch3_5_IRQn,
    0,
    0,
};

#elif defined(STM32F4) || defined(STM32F7)

#define NCONTROLLERS            (2)
#define NSTREAMS_PER_CONTROLLER (8)
#define NSTREAM                 (NCONTROLLERS * NSTREAMS_PER_CONTROLLER)

#define DMA_SUB_INSTANCE_AS_UINT8(dma_channel) (((dma_channel) & DMA_SxCR_CHSEL) >> 25)

#define DMA1_ENABLE_MASK (0x00ff) // Bits in dma_enable_mask corresponding to DMA1
#define DMA2_ENABLE_MASK (0xff00) // Bits in dma_enable_mask corresponding to DMA2

// These descriptors are ordered by DMAx_Stream number, and within a stream by channel
// number. The duplicate streams are ok as long as they aren't used at the same time.
//
// Currently I2C and SPI are synchronous and they call dma_init/dma_deinit
// around each transfer.

// DMA1 streams
const dma_descr_t dma_I2C_1_RX = { DMA1_Stream0, DMA_CHANNEL_1, dma_id_0,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_3_RX = { DMA1_Stream2, DMA_CHANNEL_0, dma_id_2,   &dma_init_struct_spi_i2c };
#if defined(STM32F7)
const dma_descr_t dma_I2C_4_RX = { DMA1_Stream2, DMA_CHANNEL_2, dma_id_2,   &dma_init_struct_spi_i2c };
#endif
const dma_descr_t dma_I2C_3_RX = { DMA1_Stream2, DMA_CHANNEL_3, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_RX = { DMA1_Stream2, DMA_CHANNEL_7, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_RX = { DMA1_Stream3, DMA_CHANNEL_0, dma_id_3,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_TX = { DMA1_Stream4, DMA_CHANNEL_0, dma_id_4,   &dma_init_struct_spi_i2c };
#if MICROPY_HW_ENABLE_I2S
const dma_descr_t dma_I2S_2_RX = { DMA1_Stream3, DMA_CHANNEL_0, dma_id_3,   &dma_init_struct_i2s };
const dma_descr_t dma_I2S_2_TX = { DMA1_Stream4, DMA_CHANNEL_0, dma_id_4,   &dma_init_struct_i2s };
#endif
const dma_descr_t dma_I2C_3_TX = { DMA1_Stream4, DMA_CHANNEL_3, dma_id_4,   &dma_init_struct_spi_i2c };
#if defined(STM32F7)
const dma_descr_t dma_I2C_4_TX = { DMA1_Stream5, DMA_CHANNEL_2, dma_id_5,   &dma_init_struct_spi_i2c };
#endif
#if defined(MICROPY_HW_ENABLE_DAC) && MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_1_TX = { DMA1_Stream5, DMA_CHANNEL_7, dma_id_5,   &dma_init_struct_dac };
const dma_descr_t dma_DAC_2_TX = { DMA1_Stream6, DMA_CHANNEL_7, dma_id_6,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_3_TX = { DMA1_Stream7, DMA_CHANNEL_0, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Stream7, DMA_CHANNEL_1, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_TX = { DMA1_Stream7, DMA_CHANNEL_7, dma_id_7,   &dma_init_struct_spi_i2c };
/* not preferred streams
const dma_descr_t dma_SPI_3_RX = { DMA1_Stream0, DMA_CHANNEL_0, dma_id_0,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Stream6, DMA_CHANNEL_1, dma_id_6,   &dma_init_struct_spi_i2c };
*/

// DMA2 streams
#if defined(STM32F7) && defined(SDMMC2) && ENABLE_SDIO
const dma_descr_t dma_SDMMC_2 = { DMA2_Stream0, DMA_CHANNEL_11, dma_id_8,  &dma_init_struct_sdio };
#endif
#if MICROPY_HW_ENABLE_DCMI
const dma_descr_t dma_DCMI_0 = { DMA2_Stream1, DMA_CHANNEL_1, dma_id_9,  &dma_init_struct_dcmi };
#endif
const dma_descr_t dma_SPI_1_RX = { DMA2_Stream2, DMA_CHANNEL_3, dma_id_10,  &dma_init_struct_spi_i2c };
#if MICROPY_HW_ENABLE_I2S
const dma_descr_t dma_I2S_1_RX = { DMA2_Stream2, DMA_CHANNEL_3, dma_id_10,  &dma_init_struct_i2s };
#endif
const dma_descr_t dma_SPI_5_RX = { DMA2_Stream3, DMA_CHANNEL_2, dma_id_11,  &dma_init_struct_spi_i2c };
#if ENABLE_SDIO
const dma_descr_t dma_SDIO_0 = { DMA2_Stream3, DMA_CHANNEL_4, dma_id_11,  &dma_init_struct_sdio };
#endif
const dma_descr_t dma_SPI_4_RX = { DMA2_Stream3, DMA_CHANNEL_5, dma_id_11,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_TX = { DMA2_Stream4, DMA_CHANNEL_2, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_TX = { DMA2_Stream4, DMA_CHANNEL_5, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_6_TX = { DMA2_Stream5, DMA_CHANNEL_1, dma_id_13,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_1_TX = { DMA2_Stream5, DMA_CHANNEL_3, dma_id_13,  &dma_init_struct_spi_i2c };
#if MICROPY_HW_ENABLE_I2S
const dma_descr_t dma_I2S_1_TX = { DMA2_Stream5, DMA_CHANNEL_3, dma_id_13,  &dma_init_struct_i2s };
#endif
// #if defined(STM32F7) && defined(SDMMC2) && ENABLE_SDIO
// const dma_descr_t dma_SDMMC_2 = { DMA2_Stream5, DMA_CHANNEL_11, dma_id_13,  &dma_init_struct_sdio };
// #endif
const dma_descr_t dma_SPI_6_RX = { DMA2_Stream6, DMA_CHANNEL_1, dma_id_14,  &dma_init_struct_spi_i2c };
// #if ENABLE_SDIO
// const dma_descr_t dma_SDIO_0 = { DMA2_Stream6, DMA_CHANNEL_4, dma_id_14,  &dma_init_struct_sdio };
// #endif
/* not preferred streams
const dma_descr_t dma_SPI_1_TX = { DMA2_Stream3, DMA_CHANNEL_3, dma_id_11,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_1_RX = { DMA2_Stream0, DMA_CHANNEL_3, dma_id_8,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_RX = { DMA2_Stream0, DMA_CHANNEL_4, dma_id_8,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_TX = { DMA2_Stream1, DMA_CHANNEL_4, dma_id_9,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_RX = { DMA2_Stream5, DMA_CHANNEL_7, dma_id_13,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_TX = { DMA2_Stream6, DMA_CHANNEL_7, dma_id_14,  &dma_init_struct_spi_i2c };
*/

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

#elif defined(STM32L0)

#define NCONTROLLERS            (1)
#define NSTREAMS_PER_CONTROLLER (7)
#define NSTREAM                 (NCONTROLLERS * NSTREAMS_PER_CONTROLLER)

#define DMA_SUB_INSTANCE_AS_UINT8(dma_request) (dma_request)

#define DMA1_ENABLE_MASK (0x007f) // Bits in dma_enable_mask corresponding to DMA1

// These descriptors are ordered by DMAx_Channel number, and within a channel by request
// number. The duplicate streams are ok as long as they aren't used at the same time.

// DMA1 streams
const dma_descr_t dma_SPI_1_RX = { DMA1_Channel2, DMA_REQUEST_1,  dma_id_1,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_TX = { DMA1_Channel2, DMA_REQUEST_14, dma_id_1,   &dma_init_struct_spi_i2c };
#if MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_1_TX = { DMA1_Channel2, DMA_REQUEST_9,  dma_id_1,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_1_TX = { DMA1_Channel3, DMA_REQUEST_1,  dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_RX = { DMA1_Channel3, DMA_REQUEST_14, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_RX = { DMA1_Channel4, DMA_REQUEST_2,  dma_id_3,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_TX = { DMA1_Channel4, DMA_REQUEST_7,  dma_id_3,   &dma_init_struct_spi_i2c };
#if MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_2_TX = { DMA1_Channel4, DMA_REQUEST_15, dma_id_3,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_2_TX = { DMA1_Channel5, DMA_REQUEST_2,  dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_RX = { DMA1_Channel5, DMA_REQUEST_7,  dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Channel6, DMA_REQUEST_6,  dma_id_5,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_RX = { DMA1_Channel7, DMA_REQUEST_6,  dma_id_6,   &dma_init_struct_spi_i2c };

static const uint8_t dma_irqn[NSTREAM] = {
    DMA1_Channel1_IRQn,
    DMA1_Channel2_3_IRQn,
    DMA1_Channel2_3_IRQn,
    DMA1_Channel4_5_6_7_IRQn,
    DMA1_Channel4_5_6_7_IRQn,
    DMA1_Channel4_5_6_7_IRQn,
    DMA1_Channel4_5_6_7_IRQn,
};

#elif defined(STM32L4)

#define NCONTROLLERS            (2)
#define NSTREAMS_PER_CONTROLLER (7)
#define NSTREAM                 (NCONTROLLERS * NSTREAMS_PER_CONTROLLER)

#define DMA_SUB_INSTANCE_AS_UINT8(dma_request) (dma_request)

#define DMA1_ENABLE_MASK (0x007f) // Bits in dma_enable_mask corresponding to DMA1
#define DMA2_ENABLE_MASK (0x3f80) // Bits in dma_enable_mask corresponding to DMA2

// These descriptors are ordered by DMAx_Channel number, and within a channel by request
// number. The duplicate streams are ok as long as they aren't used at the same time.

// DMA1 streams
// const dma_descr_t dma_ADC_1_RX = { DMA1_Channel1, DMA_REQUEST_0, dma_id_0,   NULL }; // unused
// const dma_descr_t dma_ADC_2_RX = { DMA1_Channel2, DMA_REQUEST_0, dma_id_1,   NULL }; // unused
const dma_descr_t dma_SPI_1_RX = { DMA1_Channel2, DMA_REQUEST_1, dma_id_1,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_TX = { DMA1_Channel2, DMA_REQUEST_3, dma_id_1,   &dma_init_struct_spi_i2c };
// const dma_descr_t dma_ADC_3_RX = { DMA1_Channel3, DMA_REQUEST_0, dma_id_2,   NULL }; // unused
const dma_descr_t dma_SPI_1_TX = { DMA1_Channel3, DMA_REQUEST_1, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_RX = { DMA1_Channel3, DMA_REQUEST_3, dma_id_2,   &dma_init_struct_spi_i2c };
#if MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_1_TX = { DMA1_Channel3, DMA_REQUEST_6, dma_id_2,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_2_RX = { DMA1_Channel4, DMA_REQUEST_1, dma_id_3,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_TX = { DMA1_Channel4, DMA_REQUEST_3, dma_id_3,   &dma_init_struct_spi_i2c };
#if MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_2_TX = { DMA1_Channel4, DMA_REQUEST_5, dma_id_3,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_2_TX = { DMA1_Channel5, DMA_REQUEST_1, dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_RX = { DMA1_Channel5, DMA_REQUEST_3, dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Channel6, DMA_REQUEST_3, dma_id_5,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_RX = { DMA1_Channel7, DMA_REQUEST_3, dma_id_6,   &dma_init_struct_spi_i2c };

// DMA2 streams
const dma_descr_t dma_I2C_4_RX = { DMA2_Channel1, DMA_REQUEST_0, dma_id_0,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_3_RX = { DMA2_Channel1, DMA_REQUEST_3, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_4_TX = { DMA2_Channel2, DMA_REQUEST_0, dma_id_1,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_3_TX = { DMA2_Channel2, DMA_REQUEST_3, dma_id_8,   &dma_init_struct_spi_i2c };
/* not preferred streams
const dma_descr_t dma_ADC_1_RX = { DMA2_Channel3, DMA_REQUEST_0, dma_id_9,   NULL };
const dma_descr_t dma_SPI_1_RX = { DMA2_Channel3, DMA_REQUEST_4, dma_id_9,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_ADC_2_RX = { DMA2_Channel4, DMA_REQUEST_0, dma_id_10,  NULL };
const dma_descr_t dma_DAC_1_TX = { DMA2_Channel4, DMA_REQUEST_3, dma_id_10,  &dma_init_struct_dac };
const dma_descr_t dma_SPI_1_TX = { DMA2_Channel4, DMA_REQUEST_4, dma_id_10,  &dma_init_struct_spi_i2c };
*/
#if ENABLE_SDIO
const dma_descr_t dma_SDIO_0 = { DMA2_Channel4, DMA_REQUEST_7, dma_id_10,  &dma_init_struct_sdio };
#endif
/* not preferred streams
const dma_descr_t dma_ADC_3_RX = { DMA2_Channel5, DMA_REQUEST_0, dma_id_11,  NULL };
const dma_descr_t dma_DAC_2_TX = { DMA2_Channel5, DMA_REQUEST_3, dma_id_11,  &dma_init_struct_dac };
const dma_descr_t dma_SDIO_0_TX= { DMA2_Channel5, DMA_REQUEST_7, dma_id_11,  &dma_init_struct_sdio };
const dma_descr_t dma_I2C_1_RX = { DMA2_Channel6, DMA_REQUEST_5, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA2_Channel7, DMA_REQUEST_5, dma_id_13,  &dma_init_struct_spi_i2c };
*/

static const uint8_t dma_irqn[NSTREAM] = {
    DMA1_Channel1_IRQn,
    DMA1_Channel2_IRQn,
    DMA1_Channel3_IRQn,
    DMA1_Channel4_IRQn,
    DMA1_Channel5_IRQn,
    DMA1_Channel6_IRQn,
    DMA1_Channel7_IRQn,
    DMA2_Channel1_IRQn,
    DMA2_Channel2_IRQn,
    DMA2_Channel3_IRQn,
    DMA2_Channel4_IRQn,
    DMA2_Channel5_IRQn,
    DMA2_Channel6_IRQn,
    DMA2_Channel7_IRQn,
};

#elif defined(STM32WB)

#define NCONTROLLERS            (2)
#define NSTREAMS_PER_CONTROLLER (7)
#define NSTREAM                 (NCONTROLLERS * NSTREAMS_PER_CONTROLLER)

#define DMA_SUB_INSTANCE_AS_UINT8(dma_request) (dma_request)

#define DMA1_ENABLE_MASK (0x007f) // Bits in dma_enable_mask corresponding to DMA1
#define DMA2_ENABLE_MASK (0x3f80) // Bits in dma_enable_mask corresponding to DMA2

// DMA1 streams
const dma_descr_t dma_SPI_1_RX = { DMA1_Channel1, DMA_REQUEST_SPI1_RX, dma_id_0, &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_1_TX = { DMA1_Channel2, DMA_REQUEST_SPI1_TX, dma_id_1, &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_RX = { DMA1_Channel3, DMA_REQUEST_SPI2_RX, dma_id_2, &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_TX = { DMA1_Channel4, DMA_REQUEST_SPI2_TX, dma_id_3, &dma_init_struct_spi_i2c };

static const uint8_t dma_irqn[NSTREAM] = {
    DMA1_Channel1_IRQn,
    DMA1_Channel2_IRQn,
    DMA1_Channel3_IRQn,
    DMA1_Channel4_IRQn,
    DMA1_Channel5_IRQn,
    DMA1_Channel6_IRQn,
    DMA1_Channel7_IRQn,
    DMA2_Channel1_IRQn,
    DMA2_Channel2_IRQn,
    DMA2_Channel3_IRQn,
    DMA2_Channel4_IRQn,
    DMA2_Channel5_IRQn,
    DMA2_Channel6_IRQn,
    DMA2_Channel7_IRQn,
};

#elif defined(STM32H7)

#define NCONTROLLERS            (2)
#define NSTREAMS_PER_CONTROLLER (8)
#define NSTREAM                 (NCONTROLLERS * NSTREAMS_PER_CONTROLLER)

#define DMA_SUB_INSTANCE_AS_UINT8(dma_channel) (dma_channel)

#define DMA1_ENABLE_MASK (0x00ff) // Bits in dma_enable_mask corresponding to DMA1
#define DMA2_ENABLE_MASK (0xff00) // Bits in dma_enable_mask corresponding to DMA2

// These descriptors are ordered by DMAx_Stream number, and within a stream by channel
// number. The duplicate streams are ok as long as they aren't used at the same time.
//
// Currently I2C and SPI are synchronous and they call dma_init/dma_deinit
// around each transfer.

// DMA1 streams
const dma_descr_t dma_I2C_1_RX = { DMA1_Stream0, DMA_REQUEST_I2C1_RX, dma_id_0,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_3_RX = { DMA1_Stream2, DMA_REQUEST_SPI3_RX, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_4_RX = { DMA1_Stream2, BDMA_REQUEST_I2C4_RX, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_RX = { DMA1_Stream2, DMA_REQUEST_I2C3_RX, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_RX = { DMA1_Stream2, DMA_REQUEST_I2C2_RX, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_RX = { DMA1_Stream3, DMA_REQUEST_SPI2_RX, dma_id_3,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_TX = { DMA1_Stream4, DMA_REQUEST_SPI2_TX, dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_TX = { DMA1_Stream4, DMA_REQUEST_I2C3_TX, dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_4_TX = { DMA1_Stream5, BDMA_REQUEST_I2C4_TX, dma_id_5,   &dma_init_struct_spi_i2c };
#if defined(MICROPY_HW_ENABLE_DAC) && MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_1_TX = { DMA1_Stream5, DMA_REQUEST_DAC1_CH1, dma_id_5,   &dma_init_struct_dac };
const dma_descr_t dma_DAC_2_TX = { DMA1_Stream6, DMA_REQUEST_DAC1_CH2, dma_id_6,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_3_TX = { DMA1_Stream7, DMA_REQUEST_SPI3_TX, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Stream7, DMA_REQUEST_I2C1_TX, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_TX = { DMA1_Stream7, DMA_REQUEST_I2C2_TX, dma_id_7,   &dma_init_struct_spi_i2c };

// DMA2 streams
#if MICROPY_HW_ENABLE_DCMI
const dma_descr_t dma_DCMI_0 = { DMA2_Stream1, DMA_REQUEST_DCMI, dma_id_9,  &dma_init_struct_dcmi };
#endif
const dma_descr_t dma_SPI_1_RX = { DMA2_Stream2, DMA_REQUEST_SPI1_RX, dma_id_10,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_RX = { DMA2_Stream3, DMA_REQUEST_SPI5_RX, dma_id_11,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_RX = { DMA2_Stream3, DMA_REQUEST_SPI4_RX, dma_id_11,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_TX = { DMA2_Stream4, DMA_REQUEST_SPI5_TX, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_TX = { DMA2_Stream4, DMA_REQUEST_SPI4_TX, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_6_TX = { DMA2_Stream5, BDMA_REQUEST_SPI6_TX, dma_id_13,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_1_TX = { DMA2_Stream5, DMA_REQUEST_SPI1_TX, dma_id_13,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_6_RX = { DMA2_Stream6, BDMA_REQUEST_SPI6_RX, dma_id_14,  &dma_init_struct_spi_i2c };

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

#endif

static DMA_HandleTypeDef *dma_handle[NSTREAM] = {NULL};
static uint8_t dma_last_sub_instance[NSTREAM];
static volatile uint32_t dma_enable_mask = 0;
volatile dma_idle_count_t dma_idle;

#define DMA_INVALID_CHANNEL 0xff    // Value stored in dma_last_channel which means invalid

#if defined(STM32F0) || defined(STM32L0)
#define DMA1_IS_CLK_ENABLED()   ((RCC->AHBENR & RCC_AHBENR_DMA1EN) != 0)
#if defined(DMA2)
#define DMA2_IS_CLK_ENABLED()   ((RCC->AHBENR & RCC_AHBENR_DMA2EN) != 0)
#endif
#else
#define DMA1_IS_CLK_ENABLED()   ((RCC->AHB1ENR & RCC_AHB1ENR_DMA1EN) != 0)
#define DMA2_IS_CLK_ENABLED()   ((RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN) != 0)
#endif

#if defined(STM32F0)

void DMA1_Ch1_IRQHandler(void) {
    IRQ_ENTER(DMA1_Ch1_IRQn);
    if (dma_handle[dma_id_0] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_0]);
    }
}

void DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler(void) {
    IRQ_ENTER(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
    if (dma_handle[dma_id_1] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_1]);
    }
    if (dma_handle[dma_id_2] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_2]);
    }
    if (dma_handle[dma_id_7] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_7]);
    }
    if (dma_handle[dma_id_8] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_8]);
    }
    IRQ_EXIT(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
}

void DMA1_Ch4_7_DMA2_Ch3_5_IRQHandler(void) {
    IRQ_ENTER(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
    for (unsigned int i = 0; i < 4; ++i) {
        if (dma_handle[dma_id_3 + i] != NULL) {
            HAL_DMA_IRQHandler(dma_handle[dma_id_3 + i]);
        }
        // When i==3 this will check an invalid handle, but it will always be NULL
        if (dma_handle[dma_id_9 + i] != NULL) {
            HAL_DMA_IRQHandler(dma_handle[dma_id_9 + i]);
        }
    }
    IRQ_EXIT(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
}

#elif defined(STM32F4) || defined(STM32F7) || defined(STM32H7)

void DMA1_Stream0_IRQHandler(void) {
    IRQ_ENTER(DMA1_Stream0_IRQn);
    if (dma_handle[dma_id_0] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_0]);
    }
    IRQ_EXIT(DMA1_Stream0_IRQn);
}
void DMA1_Stream1_IRQHandler(void) {
    IRQ_ENTER(DMA1_Stream1_IRQn);
    if (dma_handle[dma_id_1] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_1]);
    }
    IRQ_EXIT(DMA1_Stream1_IRQn);
}
void DMA1_Stream2_IRQHandler(void) {
    IRQ_ENTER(DMA1_Stream2_IRQn);
    if (dma_handle[dma_id_2] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_2]);
    }
    IRQ_EXIT(DMA1_Stream2_IRQn);
}
void DMA1_Stream3_IRQHandler(void) {
    IRQ_ENTER(DMA1_Stream3_IRQn);
    if (dma_handle[dma_id_3] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_3]);
    }
    IRQ_EXIT(DMA1_Stream3_IRQn);
}
void DMA1_Stream4_IRQHandler(void) {
    IRQ_ENTER(DMA1_Stream4_IRQn);
    if (dma_handle[dma_id_4] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_4]);
    }
    IRQ_EXIT(DMA1_Stream4_IRQn);
}
void DMA1_Stream5_IRQHandler(void) {
    IRQ_ENTER(DMA1_Stream5_IRQn);
    if (dma_handle[dma_id_5] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_5]);
    }
    IRQ_EXIT(DMA1_Stream5_IRQn);
}
void DMA1_Stream6_IRQHandler(void) {
    IRQ_ENTER(DMA1_Stream6_IRQn);
    if (dma_handle[dma_id_6] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_6]);
    }
    IRQ_EXIT(DMA1_Stream6_IRQn);
}
void DMA1_Stream7_IRQHandler(void) {
    IRQ_ENTER(DMA1_Stream7_IRQn);
    if (dma_handle[dma_id_7] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_7]);
    }
    IRQ_EXIT(DMA1_Stream7_IRQn);
}
void DMA2_Stream0_IRQHandler(void) {
    IRQ_ENTER(DMA2_Stream0_IRQn);
    if (dma_handle[dma_id_8] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_8]);
    }
    IRQ_EXIT(DMA2_Stream0_IRQn);
}
void DMA2_Stream1_IRQHandler(void) {
    IRQ_ENTER(DMA2_Stream1_IRQn);
    if (dma_handle[dma_id_9] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_9]);
    }
    IRQ_EXIT(DMA2_Stream1_IRQn);
}
void DMA2_Stream2_IRQHandler(void) {
    IRQ_ENTER(DMA2_Stream2_IRQn);
    if (dma_handle[dma_id_10] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_10]);
    }
    IRQ_EXIT(DMA2_Stream2_IRQn);
}
void DMA2_Stream3_IRQHandler(void) {
    IRQ_ENTER(DMA2_Stream3_IRQn);
    if (dma_handle[dma_id_11] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_11]);
    }
    IRQ_EXIT(DMA2_Stream3_IRQn);
}
void DMA2_Stream4_IRQHandler(void) {
    IRQ_ENTER(DMA2_Stream4_IRQn);
    if (dma_handle[dma_id_12] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_12]);
    }
    IRQ_EXIT(DMA2_Stream4_IRQn);
}
void DMA2_Stream5_IRQHandler(void) {
    IRQ_ENTER(DMA2_Stream5_IRQn);
    if (dma_handle[dma_id_13] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_13]);
    }
    IRQ_EXIT(DMA2_Stream5_IRQn);
}
void DMA2_Stream6_IRQHandler(void) {
    IRQ_ENTER(DMA2_Stream6_IRQn);
    if (dma_handle[dma_id_14] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_14]);
    }
    IRQ_EXIT(DMA2_Stream6_IRQn);
}
void DMA2_Stream7_IRQHandler(void) {
    IRQ_ENTER(DMA2_Stream7_IRQn);
    if (dma_handle[dma_id_15] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_15]);
    }
    IRQ_EXIT(DMA2_Stream7_IRQn);
}

#elif defined(STM32L0)

void DMA1_Channel1_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel1_IRQn);
    if (dma_handle[dma_id_0] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_0]);
    }
    IRQ_EXIT(DMA1_Channel1_IRQn);
}

void DMA1_Channel2_3_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel2_3_IRQn);
    if (dma_handle[dma_id_1] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_1]);
    }
    if (dma_handle[dma_id_2] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_2]);
    }
    IRQ_EXIT(DMA1_Channel2_3_IRQn);
}

void DMA1_Channel4_5_6_7_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel4_5_6_7_IRQn);
    if (dma_handle[dma_id_3] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_3]);
    }
    if (dma_handle[dma_id_4] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_4]);
    }
    if (dma_handle[dma_id_5] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_5]);
    }
    if (dma_handle[dma_id_6] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_6]);
    }
    IRQ_EXIT(DMA1_Channel4_5_6_7_IRQn);
}

#elif defined(STM32L4) || defined(STM32WB)

void DMA1_Channel1_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel1_IRQn);
    if (dma_handle[dma_id_0] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_0]);
    }
    IRQ_EXIT(DMA1_Channel1_IRQn);
}
void DMA1_Channel2_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel2_IRQn);
    if (dma_handle[dma_id_1] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_1]);
    }
    IRQ_EXIT(DMA1_Channel2_IRQn);
}
void DMA1_Channel3_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel3_IRQn);
    if (dma_handle[dma_id_2] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_2]);
    }
    IRQ_EXIT(DMA1_Channel3_IRQn);
}
void DMA1_Channel4_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel4_IRQn);
    if (dma_handle[dma_id_3] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_3]);
    }
    IRQ_EXIT(DMA1_Channel4_IRQn);
}
void DMA1_Channel5_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel5_IRQn);
    if (dma_handle[dma_id_4] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_4]);
    }
    IRQ_EXIT(DMA1_Channel5_IRQn);
}
void DMA1_Channel6_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel6_IRQn);
    if (dma_handle[dma_id_5] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_5]);
    }
    IRQ_EXIT(DMA1_Channel6_IRQn);
}
void DMA1_Channel7_IRQHandler(void) {
    IRQ_ENTER(DMA1_Channel7_IRQn);
    if (dma_handle[dma_id_6] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_6]);
    }
    IRQ_EXIT(DMA1_Channel7_IRQn);
}
void DMA2_Channel1_IRQHandler(void) {
    IRQ_ENTER(DMA2_Channel1_IRQn);
    if (dma_handle[dma_id_7] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_7]);
    }
    IRQ_EXIT(DMA2_Channel1_IRQn);
}
void DMA2_Channel2_IRQHandler(void) {
    IRQ_ENTER(DMA2_Channel2_IRQn);
    if (dma_handle[dma_id_8] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_8]);
    }
    IRQ_EXIT(DMA2_Channel2_IRQn);
}
void DMA2_Channel3_IRQHandler(void) {
    IRQ_ENTER(DMA2_Channel3_IRQn);
    if (dma_handle[dma_id_9] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_9]);
    }
    IRQ_EXIT(DMA2_Channel3_IRQn);
}
void DMA2_Channel4_IRQHandler(void) {
    IRQ_ENTER(DMA2_Channel4_IRQn);
    if (dma_handle[dma_id_10] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_10]);
    }
    IRQ_EXIT(DMA2_Channel4_IRQn);
}
void DMA2_Channel5_IRQHandler(void) {
    IRQ_ENTER(DMA2_Channel5_IRQn);
    if (dma_handle[dma_id_11] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_11]);
    }
    IRQ_EXIT(DMA2_Channel5_IRQn);
}
void DMA2_Channel6_IRQHandler(void) {
    IRQ_ENTER(DMA2_Channel6_IRQn);
    if (dma_handle[dma_id_12] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_12]);
    }
    IRQ_EXIT(DMA2_Channel6_IRQn);
}
void DMA2_Channel7_IRQHandler(void) {
    IRQ_ENTER(DMA2_Channel7_IRQn);
    if (dma_handle[dma_id_13] != NULL) {
        HAL_DMA_IRQHandler(dma_handle[dma_id_13]);
    }
    IRQ_EXIT(DMA2_Channel7_IRQn);
}

#endif

static void dma_idle_handler(uint32_t tick);

// Resets the idle counter for the DMA controller associated with dma_id.
static void dma_tickle(dma_id_t dma_id) {
    dma_idle.counter[(dma_id < NSTREAMS_PER_CONTROLLER) ? 0 : 1] = 1;
    systick_enable_dispatch(SYSTICK_DISPATCH_DMA, dma_idle_handler);
}

static void dma_enable_clock(dma_id_t dma_id) {
    // We don't want dma_tick_handler() to turn off the clock right after we
    // enable it, so we need to mark the channel in use in an atomic fashion.
    mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint32_t old_enable_mask = dma_enable_mask;
    dma_enable_mask |= (1 << dma_id);
    MICROPY_END_ATOMIC_SECTION(irq_state);

    #if defined(STM32WB)
    // This MCU has a DMAMUX peripheral which needs to be enabled to multiplex the channels.
    if (!__HAL_RCC_DMAMUX1_IS_CLK_ENABLED()) {
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
    }
    #endif

    if (dma_id < NSTREAMS_PER_CONTROLLER) {
        if (((old_enable_mask & DMA1_ENABLE_MASK) == 0) && !DMA1_IS_CLK_ENABLED()) {
            __HAL_RCC_DMA1_CLK_ENABLE();

            // We just turned on the clock. This means that anything stored
            // in dma_last_channel (for DMA1) needs to be invalidated.

            for (int channel = 0; channel < NSTREAMS_PER_CONTROLLER; channel++) {
                dma_last_sub_instance[channel] = DMA_INVALID_CHANNEL;
            }
        }
    }
    #if defined(DMA2)
    else {
        if (((old_enable_mask & DMA2_ENABLE_MASK) == 0) && !DMA2_IS_CLK_ENABLED()) {
            __HAL_RCC_DMA2_CLK_ENABLE();

            // We just turned on the clock. This means that anything stored
            // in dma_last_channel (for DMA2) needs to be invalidated.

            for (int channel = NSTREAMS_PER_CONTROLLER; channel < NSTREAM; channel++) {
                dma_last_sub_instance[channel] = DMA_INVALID_CHANNEL;
            }
        }
    }
    #endif
}

static void dma_disable_clock(dma_id_t dma_id) {
    // We just mark the clock as disabled here, but we don't actually disable it.
    // We wait for the timer to expire first, which means that back-to-back
    // transfers don't have to initialize as much.
    dma_tickle(dma_id);
    dma_enable_mask &= ~(1 << dma_id);
}

void dma_init_handle(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, uint32_t dir, void *data) {
    // initialise parameters
    dma->Instance = dma_descr->instance;
    dma->Init = *dma_descr->init;
    dma->Init.Direction = dir;
    #if defined(STM32L0) || defined(STM32L4) || defined(STM32H7) || defined(STM32WB)
    dma->Init.Request = dma_descr->sub_instance;
    #else
    #if !defined(STM32F0)
    dma->Init.Channel = dma_descr->sub_instance;
    #endif
    #endif
    // half of __HAL_LINKDMA(data, xxx, *dma)
    // caller must implement other half by doing: data->xxx = dma
    dma->Parent = data;
}

void dma_init(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, uint32_t dir, void *data) {
    // Some drivers allocate the DMA_HandleTypeDef from the stack
    // (i.e. dac, i2c, spi) and for those cases we need to clear the
    // structure so we don't get random values from the stack)
    memset(dma, 0, sizeof(*dma));

    if (dma_descr != NULL) {
        dma_id_t dma_id = dma_descr->id;

        dma_init_handle(dma, dma_descr, dir, data);
        // set global pointer for IRQ handler
        dma_handle[dma_id] = dma;

        dma_enable_clock(dma_id);

        #if defined(STM32H7) || defined(STM32L0) || defined(STM32L4) || defined(STM32WB)
        // Always reset and configure the H7 and L0/L4 DMA peripheral
        // (dma->State is set to HAL_DMA_STATE_RESET by memset above)
        // TODO: understand how L0/L4 DMA works so this is not needed
        HAL_DMA_DeInit(dma);
        HAL_DMA_Init(dma);
        NVIC_SetPriority(IRQn_NONNEG(dma_irqn[dma_id]), IRQ_PRI_DMA);
        #else
        // if this stream was previously configured for this channel/request and direction then we
        // can skip most of the initialisation
        uint8_t sub_inst = DMA_SUB_INSTANCE_AS_UINT8(dma_descr->sub_instance) | (dir == DMA_PERIPH_TO_MEMORY) << 7;
        if (dma_last_sub_instance[dma_id] != sub_inst) {
            dma_last_sub_instance[dma_id] = sub_inst;

            // reset and configure DMA peripheral
            // (dma->State is set to HAL_DMA_STATE_RESET by memset above)
            HAL_DMA_DeInit(dma);
            HAL_DMA_Init(dma);
            NVIC_SetPriority(IRQn_NONNEG(dma_irqn[dma_id]), IRQ_PRI_DMA);
            #if defined(STM32F0)
            if (dma->Instance < DMA2_Channel1) {
                __HAL_DMA1_REMAP(dma_descr->sub_instance);
            } else {
                __HAL_DMA2_REMAP(dma_descr->sub_instance);
            }
            #endif
        } else {
            // only necessary initialization
            dma->State = HAL_DMA_STATE_READY;
            #if defined(STM32F0)
            // These variables are used to access the relevant 4 bits in ISR and IFCR
            if (dma_id < NSTREAMS_PER_CONTROLLER) {
                dma->DmaBaseAddress = DMA1;
                dma->ChannelIndex = dma_id * 4;
            } else {
                dma->DmaBaseAddress = DMA2;
                dma->ChannelIndex = (dma_id - NSTREAMS_PER_CONTROLLER) * 4;
            }
            #elif defined(STM32F4) || defined(STM32F7)
            // calculate DMA base address and bitshift to be used in IRQ handler
            extern uint32_t DMA_CalcBaseAndBitshift(DMA_HandleTypeDef *hdma);
            DMA_CalcBaseAndBitshift(dma);
            #endif
        }
        #endif

        HAL_NVIC_EnableIRQ(dma_irqn[dma_id]);
    }
}

void dma_deinit(const dma_descr_t *dma_descr) {
    if (dma_descr != NULL) {
        #if !defined(STM32F0)
        HAL_NVIC_DisableIRQ(dma_irqn[dma_descr->id]);
        #endif
        dma_handle[dma_descr->id] = NULL;

        dma_disable_clock(dma_descr->id);
    }
}

void dma_invalidate_channel(const dma_descr_t *dma_descr) {
    if (dma_descr != NULL) {
        dma_id_t dma_id = dma_descr->id;
        // Only compare the sub-instance, not the direction bit (MSB)
        if ((dma_last_sub_instance[dma_id] & 0x7f) == DMA_SUB_INSTANCE_AS_UINT8(dma_descr->sub_instance)) {
            dma_last_sub_instance[dma_id] = DMA_INVALID_CHANNEL;
        }
    }
}

// Called from the SysTick handler
// We use LSB of tick to select which controller to process
static void dma_idle_handler(uint32_t tick) {
    if (!DMA_IDLE_ENABLED() || !DMA_IDLE_TICK(tick)) {
        return;
    }

    static const uint32_t controller_mask[] = {
        DMA1_ENABLE_MASK,
        #if defined(DMA2)
        DMA2_ENABLE_MASK,
        #endif
    };
    {
        int controller = (tick >> DMA_SYSTICK_LOG2) & 1;
        if (dma_idle.counter[controller] == 0) {
            return;
        }
        if (++dma_idle.counter[controller] > DMA_IDLE_TICK_MAX) {
            if ((dma_enable_mask & controller_mask[controller]) == 0) {
                // Nothing is active and we've reached our idle timeout,
                // Now we'll really disable the clock.
                dma_idle.counter[controller] = 0;
                if (controller == 0) {
                    __HAL_RCC_DMA1_CLK_DISABLE();
                }
                #if defined(DMA2)
                else {
                    __HAL_RCC_DMA2_CLK_DISABLE();
                }
                #endif
            } else {
                // Something is still active, but the counter never got
                // reset, so we'll reset the counter here.
                dma_idle.counter[controller] = 1;
            }
        }
    }
}

#if defined(STM32F0) || defined(STM32L0) || defined(STM32L4)

void dma_nohal_init(const dma_descr_t *descr, uint32_t config) {
    DMA_Channel_TypeDef *dma = descr->instance;

    // Enable the DMA peripheral
    dma_enable_clock(descr->id);

    // Set main configuration register
    dma->CCR =
        descr->init->Priority       // PL
        | descr->init->MemInc       // MINC
        | descr->init->PeriphInc    // PINC
        | config                    // MSIZE | PSIZE | CIRC | DIR
    ;

    // Select channel that the DMA stream uses
    #if defined(STM32F0)
    if (dma < DMA2_Channel1) {
        __HAL_DMA1_REMAP(descr->sub_instance);
    } else {
        __HAL_DMA2_REMAP(descr->sub_instance);
    }
    #else
    DMA_Request_TypeDef *dma_ctrl = (void *)(((uint32_t)dma & ~0xff) + (DMA1_CSELR_BASE - DMA1_BASE)); // DMA1_CSELR or DMA2_CSELR
    uint32_t channel_number = (((uint32_t)dma & 0xff) - 0x08) / 20; // 0 through 6
    uint32_t channel_pos = channel_number * 4;
    dma_ctrl->CSELR = (dma_ctrl->CSELR & ~(0xf << channel_pos)) | (descr->sub_instance << channel_pos);
    #endif
}

void dma_nohal_deinit(const dma_descr_t *descr) {
    DMA_Channel_TypeDef *dma = descr->instance;
    dma->CCR &= ~DMA_CCR_EN;
    dma->CCR = 0;
    dma->CNDTR = 0;
    dma_deinit(descr);
}

void dma_nohal_start(const dma_descr_t *descr, uint32_t src_addr, uint32_t dst_addr, uint16_t len) {
    DMA_Channel_TypeDef *dma = descr->instance;
    dma->CNDTR = len;
    dma->CPAR = dst_addr;
    dma->CMAR = src_addr;
    dma->CCR |= DMA_CCR_EN;
}

#elif defined(STM32WB)

// These functions are currently not implemented or needed for this MCU.

#else

void dma_nohal_init(const dma_descr_t *descr, uint32_t config) {
    DMA_Stream_TypeDef *dma = descr->instance;

    // Enable the DMA peripheral
    dma_enable_clock(descr->id);

    // Set main configuration register
    const DMA_InitTypeDef *init = descr->init;
    dma->CR =
        descr->sub_instance         // CHSEL
        | init->MemBurst            // MBURST
        | init->PeriphBurst         // PBURST
        | init->Priority            // PL
        | init->MemInc              // MINC
        | init->PeriphInc           // PINC
        | config                    // MSIZE | PSIZE | CIRC | DIR
    ;

    // Set FIFO control register
    dma->FCR =
        init->FIFOMode              // DMDIS
        | init->FIFOThreshold       // FTH
    ;

    #if defined(STM32H7)
    // Configure DMAMUX
    uint32_t request = descr->sub_instance & DMAMUX_CxCR_DMAREQ_ID;
    if ((dma >= DMA1_Stream0 && dma <= DMA1_Stream7) || (dma >= DMA2_Stream0 && dma <= DMA2_Stream7)) {
        // DMA1/2 channels 0 to 7 are hardwired to DMAMUX1 channels 0-15.
        uint32_t stream = (((uint32_t)dma & 0xff) - 16) / 24 + ((dma >= DMA2_Stream0) ? 8 : 0);
        // Set DMAMUX channel peripheral request.
        ((DMAMUX_Channel_TypeDef *)(((uint32_t)DMAMUX1_Channel0) + (stream * 4)))->CCR = request;
        DMAMUX1_ChannelStatus->CFR = 1 << (stream & 0x1F);
        // Configure DMAMUX request generator if needed.
        if (request >= DMA_REQUEST_GENERATOR0 && request <= DMA_REQUEST_GENERATOR7) {
            ((DMAMUX_RequestGen_TypeDef *)(((uint32_t)DMAMUX1_RequestGenerator0) + ((request - 1) * 4)))->RGCR = 0;
            DMAMUX1_RequestGenStatus->RGCFR = 1 << (request - 1);
        }
    #if ENABLE_BDMA // TODO: BDMA is Not currently supported by this driver.
    } else if (dma >= BDMA_Channel0 && dma <= BDMA_Channel7) {
        // BDMA channels 0 to 7 are hardwired to DMAMUX2 channels 0-7.
        uint32_t stream = (((uint32_t)dma & 0xff) - 8) / 20;
        // Set DMAMUX channel peripheral request.
        ((DMAMUX_Channel_TypeDef *)(((uint32_t)DMAMUX2_Channel0) + (stream * 4)))->CCR = request;
        DMAMUX2_ChannelStatus->CFR = 1 << (stream & 0x1F);
        // Configure DMAMUX request generator if needed.
        if (request >= DMA_REQUEST_GENERATOR0 && request <= DMA_REQUEST_GENERATOR7) {
            ((DMAMUX_RequestGen_TypeDef *)(((uint32_t)DMAMUX2_RequestGenerator0) + ((request - 1) * 4)))->RGCR = 0;
            DMAMUX2_RequestGenStatus->RGCFR = 1 << (request - 1);
        }
    #endif // ENABLE_BDMA
    }
    #endif // STM32H7
}

void dma_nohal_deinit(const dma_descr_t *descr) {
    DMA_Stream_TypeDef *dma = descr->instance;
    dma->CR &= ~DMA_SxCR_EN;
    uint32_t t0 = mp_hal_ticks_ms();
    while ((dma->CR & DMA_SxCR_EN) && mp_hal_ticks_ms() - t0 < 100) {
    }
    dma->CR = 0;
    dma->NDTR = 0;
    dma->FCR = 0x21;
    dma_deinit(descr);
}

void dma_nohal_start(const dma_descr_t *descr, uint32_t src_addr, uint32_t dst_addr, uint16_t len) {
    // Must clear all event flags for this stream before enabling it
    DMA_TypeDef *dma_ctrl;
    uint32_t ch = descr->id;
    if (ch < NSTREAMS_PER_CONTROLLER) {
        dma_ctrl = DMA1;
    } else {
        dma_ctrl = DMA2;
        ch -= NSTREAMS_PER_CONTROLLER;
    }
    __IO uint32_t *ifcr;
    if (ch <= 3) {
        ifcr = &dma_ctrl->LIFCR;
    } else {
        ifcr = &dma_ctrl->HIFCR;
        ch -= 4;
    }
    if (ch <= 1) {
        ch = ch * 6;
    } else {
        ch = 4 + ch * 6;
    }
    *ifcr = 0x3d << ch;

    // Configure and enable stream
    DMA_Stream_TypeDef *dma = descr->instance;
    dma->CR &= ~DMA_SxCR_DBM;
    dma->NDTR = len;
    dma->PAR = dst_addr;
    dma->M0AR = src_addr;
    dma->CR |= DMA_SxCR_EN;
}

#endif

#define DMA_ID_FROM_CONTROLLER_STREAM(c, s) ((s) + (c) * NSTREAMS_PER_CONTROLLER)

void dma_external_acquire(uint32_t controller, uint32_t stream) {
    dma_enable_clock(DMA_ID_FROM_CONTROLLER_STREAM(controller, stream));
}

void dma_external_release(uint32_t controller, uint32_t stream) {
    dma_disable_clock(DMA_ID_FROM_CONTROLLER_STREAM(controller, stream));
}
