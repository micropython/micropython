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

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "py/obj.h"
#include "dma.h"
#include "irq.h"

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

typedef struct _dma_descr_t {
    #if defined(STM32F4) || defined(STM32F7) || defined(STM32H7)
    DMA_Stream_TypeDef *instance;
    #elif defined(STM32L4)
    DMA_Channel_TypeDef *instance;
    #else
    #error "Unsupported Processor"
    #endif
    uint32_t sub_instance;
    uint32_t transfer_direction; // periph to memory or vice-versa
    dma_id_t id;
    const DMA_InitTypeDef *init;
} dma_descr_t;

// Default parameters to dma_init() shared by spi and i2c; Channel and Direction
// vary depending on the peripheral instance so they get passed separately
static const DMA_InitTypeDef dma_init_struct_spi_i2c = {
    #if defined(STM32F4) || defined(STM32F7)
    .Channel             = 0,
    #elif defined(STM32L4)
    .Request             = 0,
    #endif
    .Direction           = 0,
    .PeriphInc           = DMA_PINC_DISABLE,
    .MemInc              = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
    .MemDataAlignment    = DMA_MDATAALIGN_BYTE,
    .Mode                = DMA_NORMAL,
    .Priority            = DMA_PRIORITY_LOW,
    #if defined(STM32F4) || defined(STM32F7)
    .FIFOMode            = DMA_FIFOMODE_DISABLE,
    .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
    .MemBurst            = DMA_MBURST_INC4,
    .PeriphBurst         = DMA_PBURST_INC4
    #endif
};

#if defined(MICROPY_HW_HAS_SDCARD) && MICROPY_HW_HAS_SDCARD && !defined(STM32H7)
// Parameters to dma_init() for SDIO tx and rx.
static const DMA_InitTypeDef dma_init_struct_sdio = {
    #if defined(STM32F4) || defined(STM32F7)
    .Channel             = 0,
    #elif defined(STM32L4)
    .Request             = 0,
    #endif
    .Direction           = 0,
    .PeriphInc           = DMA_PINC_DISABLE,
    .MemInc              = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_WORD,
    .MemDataAlignment    = DMA_MDATAALIGN_WORD,
    #if defined(STM32F4) || defined(STM32F7)
    .Mode                = DMA_PFCTRL,
    #elif defined(STM32L4)
    .Mode                = DMA_NORMAL,
    #endif
    .Priority            = DMA_PRIORITY_VERY_HIGH,
    #if defined(STM32F4) || defined(STM32F7)
    .FIFOMode            = DMA_FIFOMODE_ENABLE,
    .FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
    .MemBurst            = DMA_MBURST_INC4,
    .PeriphBurst         = DMA_PBURST_INC4,
    #endif
};
#endif

#if defined(MICROPY_HW_ENABLE_DAC) && MICROPY_HW_ENABLE_DAC
// Default parameters to dma_init() for DAC tx
static const DMA_InitTypeDef dma_init_struct_dac = {
    #if defined(STM32F4) || defined(STM32F7)
    .Channel             = 0,
    #elif defined(STM32L4)
    .Request             = 0,
    #endif
    .Direction           = 0,
    .PeriphInc           = DMA_PINC_DISABLE,
    .MemInc              = DMA_MINC_ENABLE,
    .PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
    .MemDataAlignment    = DMA_MDATAALIGN_BYTE,
    .Mode                = DMA_NORMAL,
    .Priority            = DMA_PRIORITY_HIGH,
    #if defined(STM32F4) || defined(STM32F7)
    .FIFOMode            = DMA_FIFOMODE_DISABLE,
    .FIFOThreshold       = DMA_FIFO_THRESHOLD_HALFFULL,
    .MemBurst            = DMA_MBURST_SINGLE,
    .PeriphBurst         = DMA_PBURST_SINGLE,
    #endif
};
#endif

#if defined(STM32F4) || defined(STM32F7)

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
const dma_descr_t dma_I2C_1_RX = { DMA1_Stream0, DMA_CHANNEL_1, DMA_PERIPH_TO_MEMORY, dma_id_0,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_3_RX = { DMA1_Stream2, DMA_CHANNEL_0, DMA_PERIPH_TO_MEMORY, dma_id_2,   &dma_init_struct_spi_i2c };
#if defined(STM32F7)
const dma_descr_t dma_I2C_4_RX = { DMA1_Stream2, DMA_CHANNEL_2, DMA_PERIPH_TO_MEMORY, dma_id_2,   &dma_init_struct_spi_i2c };
#endif
const dma_descr_t dma_I2C_3_RX = { DMA1_Stream2, DMA_CHANNEL_3, DMA_PERIPH_TO_MEMORY, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_RX = { DMA1_Stream2, DMA_CHANNEL_7, DMA_PERIPH_TO_MEMORY, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_RX = { DMA1_Stream3, DMA_CHANNEL_0, DMA_PERIPH_TO_MEMORY, dma_id_3,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_TX = { DMA1_Stream4, DMA_CHANNEL_0, DMA_MEMORY_TO_PERIPH, dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_TX = { DMA1_Stream4, DMA_CHANNEL_3, DMA_MEMORY_TO_PERIPH, dma_id_4,   &dma_init_struct_spi_i2c };
#if defined(STM32F7)
const dma_descr_t dma_I2C_4_TX = { DMA1_Stream5, DMA_CHANNEL_2, DMA_MEMORY_TO_PERIPH, dma_id_5,   &dma_init_struct_spi_i2c };
#endif
#if defined(MICROPY_HW_ENABLE_DAC) && MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_1_TX = { DMA1_Stream5, DMA_CHANNEL_7, DMA_MEMORY_TO_PERIPH, dma_id_5,   &dma_init_struct_dac };
const dma_descr_t dma_DAC_2_TX = { DMA1_Stream6, DMA_CHANNEL_7, DMA_MEMORY_TO_PERIPH, dma_id_6,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_3_TX = { DMA1_Stream7, DMA_CHANNEL_0, DMA_MEMORY_TO_PERIPH, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Stream7, DMA_CHANNEL_1, DMA_MEMORY_TO_PERIPH, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_TX = { DMA1_Stream7, DMA_CHANNEL_7, DMA_MEMORY_TO_PERIPH, dma_id_7,   &dma_init_struct_spi_i2c };
/* not preferred streams
const dma_descr_t dma_SPI_3_RX = { DMA1_Stream0, DMA_CHANNEL_0, DMA_PERIPH_TO_MEMORY, dma_id_0,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Stream6, DMA_CHANNEL_1, DMA_MEMORY_TO_PERIPH, dma_id_6,   &dma_init_struct_spi_i2c };
*/

// DMA2 streams
#if defined(STM32F7) && defined(SDMMC2) && MICROPY_HW_HAS_SDCARD
const dma_descr_t dma_SDMMC_2_RX= { DMA2_Stream0, DMA_CHANNEL_11, DMA_PERIPH_TO_MEMORY, dma_id_8,  &dma_init_struct_sdio };
#endif
const dma_descr_t dma_SPI_1_RX = { DMA2_Stream2, DMA_CHANNEL_3, DMA_PERIPH_TO_MEMORY, dma_id_10,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_RX = { DMA2_Stream3, DMA_CHANNEL_2, DMA_PERIPH_TO_MEMORY, dma_id_11,  &dma_init_struct_spi_i2c };
#if defined(MICROPY_HW_HAS_SDCARD) && MICROPY_HW_HAS_SDCARD
const dma_descr_t dma_SDIO_0_RX= { DMA2_Stream3, DMA_CHANNEL_4, DMA_PERIPH_TO_MEMORY, dma_id_11,  &dma_init_struct_sdio };
#endif
const dma_descr_t dma_SPI_4_RX = { DMA2_Stream3, DMA_CHANNEL_5, DMA_PERIPH_TO_MEMORY, dma_id_11,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_TX = { DMA2_Stream4, DMA_CHANNEL_2, DMA_MEMORY_TO_PERIPH, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_TX = { DMA2_Stream4, DMA_CHANNEL_5, DMA_MEMORY_TO_PERIPH, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_6_TX = { DMA2_Stream5, DMA_CHANNEL_1, DMA_MEMORY_TO_PERIPH, dma_id_13,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_1_TX = { DMA2_Stream5, DMA_CHANNEL_3, DMA_MEMORY_TO_PERIPH, dma_id_13,  &dma_init_struct_spi_i2c };
#if defined(STM32F7) && defined(SDMMC2) && MICROPY_HW_HAS_SDCARD
const dma_descr_t dma_SDMMC_2_TX= { DMA2_Stream5, DMA_CHANNEL_11, DMA_MEMORY_TO_PERIPH, dma_id_13,  &dma_init_struct_sdio };
#endif
const dma_descr_t dma_SPI_6_RX = { DMA2_Stream6, DMA_CHANNEL_1, DMA_PERIPH_TO_MEMORY, dma_id_14,  &dma_init_struct_spi_i2c };
#if defined(MICROPY_HW_HAS_SDCARD) && MICROPY_HW_HAS_SDCARD
const dma_descr_t dma_SDIO_0_TX= { DMA2_Stream6, DMA_CHANNEL_4, DMA_MEMORY_TO_PERIPH, dma_id_14,  &dma_init_struct_sdio };
#endif
/* not preferred streams
const dma_descr_t dma_SPI_1_TX = { DMA2_Stream3, DMA_CHANNEL_3, DMA_MEMORY_TO_PERIPH, dma_id_11,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_1_RX = { DMA2_Stream0, DMA_CHANNEL_3, DMA_PERIPH_TO_MEMORY, dma_id_8,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_RX = { DMA2_Stream0, DMA_CHANNEL_4, DMA_PERIPH_TO_MEMORY, dma_id_8,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_TX = { DMA2_Stream1, DMA_CHANNEL_4, DMA_MEMORY_TO_PERIPH, dma_id_9,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_RX = { DMA2_Stream5, DMA_CHANNEL_7, DMA_PERIPH_TO_MEMORY, dma_id_13,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_TX = { DMA2_Stream6, DMA_CHANNEL_7, DMA_MEMORY_TO_PERIPH, dma_id_14,  &dma_init_struct_spi_i2c };
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

#elif defined(STM32L4)

#define NCONTROLLERS            (2)
#define NSTREAMS_PER_CONTROLLER (7)
#define NSTREAM                 (NCONTROLLERS * NSTREAMS_PER_CONTROLLER)

#define DMA_SUB_INSTANCE_AS_UINT8(dma_request) (dma_request)

#define DMA1_ENABLE_MASK (0x007f) // Bits in dma_enable_mask corresponfing to DMA1
#define DMA2_ENABLE_MASK (0x3f80) // Bits in dma_enable_mask corresponding to DMA2

// These descriptors are ordered by DMAx_Channel number, and within a channel by request
// number. The duplicate streams are ok as long as they aren't used at the same time.

// DMA1 streams
//const dma_descr_t dma_ADC_1_RX = { DMA1_Channel1, DMA_REQUEST_0, DMA_PERIPH_TO_MEMORY, dma_id_0,   NULL }; // unused
//const dma_descr_t dma_ADC_2_RX = { DMA1_Channel2, DMA_REQUEST_0, DMA_PERIPH_TO_MEMORY, dma_id_1,   NULL }; // unused
const dma_descr_t dma_SPI_1_RX = { DMA1_Channel2, DMA_REQUEST_1, DMA_PERIPH_TO_MEMORY, dma_id_1,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_TX = { DMA1_Channel2, DMA_REQUEST_3, DMA_MEMORY_TO_PERIPH, dma_id_1,   &dma_init_struct_spi_i2c };
//const dma_descr_t dma_ADC_3_RX = { DMA1_Channel3, DMA_REQUEST_0, DMA_PERIPH_TO_MEMORY, dma_id_2,   NULL }; // unused
const dma_descr_t dma_SPI_1_TX = { DMA1_Channel3, DMA_REQUEST_1, DMA_MEMORY_TO_PERIPH, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_RX = { DMA1_Channel3, DMA_REQUEST_3, DMA_PERIPH_TO_MEMORY, dma_id_2,   &dma_init_struct_spi_i2c };
#if MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_1_TX = { DMA1_Channel3, DMA_REQUEST_6, DMA_MEMORY_TO_PERIPH, dma_id_2,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_2_RX = { DMA1_Channel4, DMA_REQUEST_1, DMA_PERIPH_TO_MEMORY, dma_id_3,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_TX = { DMA1_Channel4, DMA_REQUEST_3, DMA_MEMORY_TO_PERIPH, dma_id_3,   &dma_init_struct_spi_i2c };
#if MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_2_TX = { DMA1_Channel4, DMA_REQUEST_5, DMA_MEMORY_TO_PERIPH, dma_id_3,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_2_TX = { DMA1_Channel5, DMA_REQUEST_1, DMA_MEMORY_TO_PERIPH, dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_RX = { DMA1_Channel5, DMA_REQUEST_3, DMA_PERIPH_TO_MEMORY, dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Channel6, DMA_REQUEST_3, DMA_MEMORY_TO_PERIPH, dma_id_5,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_RX = { DMA1_Channel7, DMA_REQUEST_3, DMA_PERIPH_TO_MEMORY, dma_id_6,   &dma_init_struct_spi_i2c };

// DMA2 streams
const dma_descr_t dma_SPI_3_RX = { DMA2_Channel1, DMA_REQUEST_3, DMA_PERIPH_TO_MEMORY, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_3_TX = { DMA2_Channel2, DMA_REQUEST_3, DMA_MEMORY_TO_PERIPH, dma_id_8,   &dma_init_struct_spi_i2c };
/* not preferred streams
const dma_descr_t dma_ADC_1_RX = { DMA2_Channel3, DMA_REQUEST_0, DMA_PERIPH_TO_MEMORY, dma_id_9,   NULL };
const dma_descr_t dma_SPI_1_RX = { DMA2_Channel3, DMA_REQUEST_4, DMA_PERIPH_TO_MEMORY, dma_id_9,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_ADC_2_RX = { DMA2_Channel4, DMA_REQUEST_0, DMA_PERIPH_TO_MEMORY, dma_id_10,  NULL };
const dma_descr_t dma_DAC_1_TX = { DMA2_Channel4, DMA_REQUEST_3, DMA_MEMORY_TO_PERIPH, dma_id_10,  &dma_init_struct_dac };
const dma_descr_t dma_SPI_1_TX = { DMA2_Channel4, DMA_REQUEST_4, DMA_MEMORY_TO_PERIPH, dma_id_10,  &dma_init_struct_spi_i2c };
*/
#if defined(MICROPY_HW_HAS_SDCARD) && MICROPY_HW_HAS_SDCARD
// defined twice as L4 HAL only needs one channel and can correctly switch direction but sdcard.c needs two channels
const dma_descr_t dma_SDIO_0_TX= { DMA2_Channel4, DMA_REQUEST_7, DMA_MEMORY_TO_PERIPH, dma_id_10,  &dma_init_struct_sdio };
const dma_descr_t dma_SDIO_0_RX= { DMA2_Channel4, DMA_REQUEST_7, DMA_PERIPH_TO_MEMORY, dma_id_10,  &dma_init_struct_sdio };
#endif
/* not preferred streams
const dma_descr_t dma_ADC_3_RX = { DMA2_Channel5, DMA_REQUEST_0, DMA_PERIPH_TO_MEMORY, dma_id_11,  NULL };
const dma_descr_t dma_DAC_2_TX = { DMA2_Channel5, DMA_REQUEST_3, DMA_MEMORY_TO_PERIPH, dma_id_11,  &dma_init_struct_dac };
const dma_descr_t dma_SDIO_0_TX= { DMA2_Channel5, DMA_REQUEST_7, DMA_MEMORY_TO_PERIPH, dma_id_11,  &dma_init_struct_sdio };
const dma_descr_t dma_I2C_1_RX = { DMA2_Channel6, DMA_REQUEST_5, DMA_PERIPH_TO_MEMORY, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA2_Channel7, DMA_REQUEST_5, DMA_MEMORY_TO_PERIPH, dma_id_13,  &dma_init_struct_spi_i2c };
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
const dma_descr_t dma_I2C_1_RX = { DMA1_Stream0, DMA_REQUEST_I2C1_RX, DMA_PERIPH_TO_MEMORY, dma_id_0,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_3_RX = { DMA1_Stream2, DMA_REQUEST_SPI3_RX, DMA_PERIPH_TO_MEMORY, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_4_RX = { DMA1_Stream2, BDMA_REQUEST_I2C4_RX, DMA_PERIPH_TO_MEMORY, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_RX = { DMA1_Stream2, DMA_REQUEST_I2C3_RX, DMA_PERIPH_TO_MEMORY, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_RX = { DMA1_Stream2, DMA_REQUEST_I2C2_RX, DMA_PERIPH_TO_MEMORY, dma_id_2,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_RX = { DMA1_Stream3, DMA_REQUEST_SPI2_RX, DMA_PERIPH_TO_MEMORY, dma_id_3,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_2_TX = { DMA1_Stream4, DMA_REQUEST_SPI2_TX, DMA_MEMORY_TO_PERIPH, dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_3_TX = { DMA1_Stream4, DMA_REQUEST_I2C3_TX, DMA_MEMORY_TO_PERIPH, dma_id_4,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_4_TX = { DMA1_Stream5, BDMA_REQUEST_I2C4_TX, DMA_MEMORY_TO_PERIPH, dma_id_5,   &dma_init_struct_spi_i2c };
#if defined(MICROPY_HW_ENABLE_DAC) && MICROPY_HW_ENABLE_DAC
const dma_descr_t dma_DAC_1_TX = { DMA1_Stream5, DMA_REQUEST_DAC1_CH1, DMA_MEMORY_TO_PERIPH, dma_id_5,   &dma_init_struct_dac };
const dma_descr_t dma_DAC_2_TX = { DMA1_Stream6, DMA_REQUEST_DAC1_CH2, DMA_MEMORY_TO_PERIPH, dma_id_6,   &dma_init_struct_dac };
#endif
const dma_descr_t dma_SPI_3_TX = { DMA1_Stream7, DMA_REQUEST_SPI3_TX, DMA_MEMORY_TO_PERIPH, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_1_TX = { DMA1_Stream7, DMA_REQUEST_I2C1_TX, DMA_MEMORY_TO_PERIPH, dma_id_7,   &dma_init_struct_spi_i2c };
const dma_descr_t dma_I2C_2_TX = { DMA1_Stream7, DMA_REQUEST_I2C2_TX, DMA_MEMORY_TO_PERIPH, dma_id_7,   &dma_init_struct_spi_i2c };

// DMA2 streams
const dma_descr_t dma_SPI_1_RX = { DMA2_Stream2, DMA_REQUEST_SPI1_RX, DMA_PERIPH_TO_MEMORY, dma_id_10,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_RX = { DMA2_Stream3, DMA_REQUEST_SPI5_RX, DMA_PERIPH_TO_MEMORY, dma_id_11,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_RX = { DMA2_Stream3, DMA_REQUEST_SPI4_RX, DMA_PERIPH_TO_MEMORY, dma_id_11,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_5_TX = { DMA2_Stream4, DMA_REQUEST_SPI5_TX, DMA_MEMORY_TO_PERIPH, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_4_TX = { DMA2_Stream4, DMA_REQUEST_SPI4_TX, DMA_MEMORY_TO_PERIPH, dma_id_12,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_6_TX = { DMA2_Stream5, BDMA_REQUEST_SPI6_TX, DMA_MEMORY_TO_PERIPH, dma_id_13,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_1_TX = { DMA2_Stream5, DMA_REQUEST_SPI1_TX, DMA_MEMORY_TO_PERIPH, dma_id_13,  &dma_init_struct_spi_i2c };
const dma_descr_t dma_SPI_6_RX = { DMA2_Stream6, BDMA_REQUEST_SPI6_RX, DMA_PERIPH_TO_MEMORY, dma_id_14,  &dma_init_struct_spi_i2c };

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

#define DMA1_IS_CLK_ENABLED()   ((RCC->AHB1ENR & RCC_AHB1ENR_DMA1EN) != 0)
#define DMA2_IS_CLK_ENABLED()   ((RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN) != 0)

#if defined(STM32F4) || defined(STM32F7) || defined(STM32H7)

void DMA1_Stream0_IRQHandler(void) { IRQ_ENTER(DMA1_Stream0_IRQn); if (dma_handle[dma_id_0] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_0]); } IRQ_EXIT(DMA1_Stream0_IRQn); }
void DMA1_Stream1_IRQHandler(void) { IRQ_ENTER(DMA1_Stream1_IRQn); if (dma_handle[dma_id_1] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_1]); } IRQ_EXIT(DMA1_Stream1_IRQn); }
void DMA1_Stream2_IRQHandler(void) { IRQ_ENTER(DMA1_Stream2_IRQn); if (dma_handle[dma_id_2] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_2]); } IRQ_EXIT(DMA1_Stream2_IRQn); }
void DMA1_Stream3_IRQHandler(void) { IRQ_ENTER(DMA1_Stream3_IRQn); if (dma_handle[dma_id_3] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_3]); } IRQ_EXIT(DMA1_Stream3_IRQn); }
void DMA1_Stream4_IRQHandler(void) { IRQ_ENTER(DMA1_Stream4_IRQn); if (dma_handle[dma_id_4] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_4]); } IRQ_EXIT(DMA1_Stream4_IRQn); }
void DMA1_Stream5_IRQHandler(void) { IRQ_ENTER(DMA1_Stream5_IRQn); if (dma_handle[dma_id_5] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_5]); } IRQ_EXIT(DMA1_Stream5_IRQn); }
void DMA1_Stream6_IRQHandler(void) { IRQ_ENTER(DMA1_Stream6_IRQn); if (dma_handle[dma_id_6] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_6]); } IRQ_EXIT(DMA1_Stream6_IRQn); }
void DMA1_Stream7_IRQHandler(void) { IRQ_ENTER(DMA1_Stream7_IRQn); if (dma_handle[dma_id_7] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_7]); } IRQ_EXIT(DMA1_Stream7_IRQn); }
void DMA2_Stream0_IRQHandler(void) { IRQ_ENTER(DMA2_Stream0_IRQn); if (dma_handle[dma_id_8] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_8]); } IRQ_EXIT(DMA2_Stream0_IRQn); }
void DMA2_Stream1_IRQHandler(void) { IRQ_ENTER(DMA2_Stream1_IRQn); if (dma_handle[dma_id_9] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_9]); } IRQ_EXIT(DMA2_Stream1_IRQn); }
void DMA2_Stream2_IRQHandler(void) { IRQ_ENTER(DMA2_Stream2_IRQn); if (dma_handle[dma_id_10] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_10]); } IRQ_EXIT(DMA2_Stream2_IRQn); }
void DMA2_Stream3_IRQHandler(void) { IRQ_ENTER(DMA2_Stream3_IRQn); if (dma_handle[dma_id_11] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_11]); } IRQ_EXIT(DMA2_Stream3_IRQn); }
void DMA2_Stream4_IRQHandler(void) { IRQ_ENTER(DMA2_Stream4_IRQn); if (dma_handle[dma_id_12] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_12]); } IRQ_EXIT(DMA2_Stream4_IRQn); }
void DMA2_Stream5_IRQHandler(void) { IRQ_ENTER(DMA2_Stream5_IRQn); if (dma_handle[dma_id_13] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_13]); } IRQ_EXIT(DMA2_Stream5_IRQn); }
void DMA2_Stream6_IRQHandler(void) { IRQ_ENTER(DMA2_Stream6_IRQn); if (dma_handle[dma_id_14] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_14]); } IRQ_EXIT(DMA2_Stream6_IRQn); }
void DMA2_Stream7_IRQHandler(void) { IRQ_ENTER(DMA2_Stream7_IRQn); if (dma_handle[dma_id_15] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_15]); } IRQ_EXIT(DMA2_Stream7_IRQn); }

#elif defined(STM32L4)

void DMA1_Channel1_IRQHandler(void) { IRQ_ENTER(DMA1_Channel1_IRQn); if (dma_handle[dma_id_0] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_0]); } IRQ_EXIT(DMA1_Channel1_IRQn); }
void DMA1_Channel2_IRQHandler(void) { IRQ_ENTER(DMA1_Channel2_IRQn); if (dma_handle[dma_id_1] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_1]); } IRQ_EXIT(DMA1_Channel2_IRQn); }
void DMA1_Channel3_IRQHandler(void) { IRQ_ENTER(DMA1_Channel3_IRQn); if (dma_handle[dma_id_2] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_2]); } IRQ_EXIT(DMA1_Channel3_IRQn); }
void DMA1_Channel4_IRQHandler(void) { IRQ_ENTER(DMA1_Channel4_IRQn); if (dma_handle[dma_id_3] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_3]); } IRQ_EXIT(DMA1_Channel4_IRQn); }
void DMA1_Channel5_IRQHandler(void) { IRQ_ENTER(DMA1_Channel5_IRQn); if (dma_handle[dma_id_4] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_4]); } IRQ_EXIT(DMA1_Channel5_IRQn); }
void DMA1_Channel6_IRQHandler(void) { IRQ_ENTER(DMA1_Channel6_IRQn); if (dma_handle[dma_id_5] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_5]); } IRQ_EXIT(DMA1_Channel6_IRQn); }
void DMA1_Channel7_IRQHandler(void) { IRQ_ENTER(DMA1_Channel7_IRQn); if (dma_handle[dma_id_6] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_6]); } IRQ_EXIT(DMA1_Channel7_IRQn); }
void DMA2_Channel1_IRQHandler(void) { IRQ_ENTER(DMA2_Channel1_IRQn); if (dma_handle[dma_id_7] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_7]); } IRQ_EXIT(DMA2_Channel1_IRQn); }
void DMA2_Channel2_IRQHandler(void) { IRQ_ENTER(DMA2_Channel2_IRQn); if (dma_handle[dma_id_8] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_8]); } IRQ_EXIT(DMA2_Channel2_IRQn); }
void DMA2_Channel3_IRQHandler(void) { IRQ_ENTER(DMA2_Channel3_IRQn); if (dma_handle[dma_id_9] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_9]); } IRQ_EXIT(DMA2_Channel3_IRQn); }
void DMA2_Channel4_IRQHandler(void) { IRQ_ENTER(DMA2_Channel4_IRQn); if (dma_handle[dma_id_10] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_10]);} IRQ_EXIT(DMA2_Channel4_IRQn); }
void DMA2_Channel5_IRQHandler(void) { IRQ_ENTER(DMA2_Channel5_IRQn); if (dma_handle[dma_id_11] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_11]);} IRQ_EXIT(DMA2_Channel5_IRQn); }
void DMA2_Channel6_IRQHandler(void) { IRQ_ENTER(DMA2_Channel6_IRQn); if (dma_handle[dma_id_12] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_12]);} IRQ_EXIT(DMA2_Channel6_IRQn); }
void DMA2_Channel7_IRQHandler(void) { IRQ_ENTER(DMA2_Channel7_IRQn); if (dma_handle[dma_id_13] != NULL) { HAL_DMA_IRQHandler(dma_handle[dma_id_13]);} IRQ_EXIT(DMA2_Channel7_IRQn); }

#endif

// Resets the idle counter for the DMA controller associated with dma_id.
static void dma_tickle(dma_id_t dma_id) {
    dma_idle.counter[(dma_id < NSTREAMS_PER_CONTROLLER) ? 0 : 1] = 1;
}

static void dma_enable_clock(dma_id_t dma_id) {
    // We don't want dma_tick_handler() to turn off the clock right after we
    // enable it, so we need to mark the channel in use in an atomic fashion.
    mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint32_t old_enable_mask = dma_enable_mask;
    dma_enable_mask |= (1 << dma_id);
    MICROPY_END_ATOMIC_SECTION(irq_state);

    if (dma_id < NSTREAMS_PER_CONTROLLER) {
        if (((old_enable_mask & DMA1_ENABLE_MASK) == 0) && !DMA1_IS_CLK_ENABLED()) {
            __HAL_RCC_DMA1_CLK_ENABLE();

            // We just turned on the clock. This means that anything stored
            // in dma_last_channel (for DMA1) needs to be invalidated.

            for (int channel = 0; channel < NSTREAMS_PER_CONTROLLER; channel++) {
                dma_last_sub_instance[channel] = DMA_INVALID_CHANNEL;
            }
        }
    } else {
        if (((old_enable_mask & DMA2_ENABLE_MASK) == 0) && !DMA2_IS_CLK_ENABLED()) {
            __HAL_RCC_DMA2_CLK_ENABLE();

            // We just turned on the clock. This means that anything stored
            // in dma_last_channel (for DMA1) needs to be invalidated.

            for (int channel = NSTREAMS_PER_CONTROLLER; channel < NSTREAM; channel++) {
                dma_last_sub_instance[channel] = DMA_INVALID_CHANNEL;
            }
        }
    }
}

static void dma_disable_clock(dma_id_t dma_id) {
    // We just mark the clock as disabled here, but we don't actually disable it.
    // We wait for the timer to expire first, which means that back-to-back
    // transfers don't have to initialize as much.
    dma_tickle(dma_id);
    dma_enable_mask &= ~(1 << dma_id);
}

void dma_init_handle(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, void *data) {
    // initialise parameters
    dma->Instance = dma_descr->instance;
    dma->Init = *dma_descr->init;
    dma->Init.Direction = dma_descr->transfer_direction;
    #if defined(STM32L4) || defined(STM32H7)
    dma->Init.Request = dma_descr->sub_instance;
    #else
    dma->Init.Channel = dma_descr->sub_instance;
    #endif
    // half of __HAL_LINKDMA(data, xxx, *dma)
    // caller must implement other half by doing: data->xxx = dma
    dma->Parent = data;
}

void dma_init(DMA_HandleTypeDef *dma, const dma_descr_t *dma_descr, void *data){
    // Some drivers allocate the DMA_HandleTypeDef from the stack
    // (i.e. dac, i2c, spi) and for those cases we need to clear the
    // structure so we don't get random values from the stack)
    memset(dma, 0, sizeof(*dma));

    if (dma_descr != NULL) {
        dma_id_t dma_id  = dma_descr->id;

        dma_init_handle(dma, dma_descr, data);
        // set global pointer for IRQ handler
        dma_handle[dma_id] = dma;

        dma_enable_clock(dma_id);

        // if this stream was previously configured for this channel/request then we
        // can skip most of the initialisation
        uint8_t sub_inst = DMA_SUB_INSTANCE_AS_UINT8(dma_descr->sub_instance);
        if (dma_last_sub_instance[dma_id] != sub_inst) {
            dma_last_sub_instance[dma_id] = sub_inst;

            // reset and configure DMA peripheral
            // (dma->State is set to HAL_DMA_STATE_RESET by memset above)
            HAL_DMA_DeInit(dma);
            HAL_DMA_Init(dma);
            HAL_NVIC_SetPriority(dma_irqn[dma_id], IRQ_PRI_DMA, IRQ_SUBPRI_DMA);
        } else {
            // only necessary initialization
            dma->State = HAL_DMA_STATE_READY;
#if defined(STM32F4) || defined(STM32F7)
            // calculate DMA base address and bitshift to be used in IRQ handler
            extern uint32_t DMA_CalcBaseAndBitshift(DMA_HandleTypeDef *hdma);
            DMA_CalcBaseAndBitshift(dma);
#endif
        }

        HAL_NVIC_EnableIRQ(dma_irqn[dma_id]);
    }
}

void dma_deinit(const dma_descr_t *dma_descr) {
    if (dma_descr != NULL) {
        HAL_NVIC_DisableIRQ(dma_irqn[dma_descr->id]);
        dma_handle[dma_descr->id] = NULL;

        dma_disable_clock(dma_descr->id);
    }
}

void dma_invalidate_channel(const dma_descr_t *dma_descr) {
    if (dma_descr != NULL) {
        dma_id_t dma_id = dma_descr->id;
        if (dma_last_sub_instance[dma_id] == DMA_SUB_INSTANCE_AS_UINT8(dma_descr->sub_instance) ) {
            dma_last_sub_instance[dma_id] = DMA_INVALID_CHANNEL;
        }
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
                    __HAL_RCC_DMA1_CLK_DISABLE();
                } else {
                    __HAL_RCC_DMA2_CLK_DISABLE();
                }
            } else {
                // Something is still active, but the counter never got
                // reset, so we'll reset the counter here.
                dma_idle.counter[controller] = 1;
            }
        }
    }
}
