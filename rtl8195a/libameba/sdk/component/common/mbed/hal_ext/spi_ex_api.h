/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_SPI_EXT_API_H
#define MBED_SPI_EXT_API_H

#include "device.h"

#if DEVICE_SPI

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_DMA_RX_EN           (1<<0)
#define SPI_DMA_TX_EN           (1<<1)

enum {
    SPI_SCLK_IDLE_LOW=0,        // the SCLK is Low when SPI is inactive
    SPI_SCLK_IDLE_HIGH=2        // the SCLK is High when SPI is inactive
};

// SPI Master mode: for continuous transfer, how the CS toggle:
enum {
    SPI_CS_TOGGLE_EVERY_FRAME=0,       // let SCPH=0 then the CS toggle every frame
    SPI_CS_TOGGLE_START_STOP=1         // let SCPH=1 the CS toggle at start and stop
};

enum {
    SPI_SCLK_TOGGLE_MIDDLE=0,      // Serial Clk toggle at middle of 1st data bit and latch data at 1st Clk edge
    SPI_SCLK_TOGGLE_START=1        // Serial Clk toggle at start of 1st data bit and latch data at 2nd Clk edge
};

typedef enum {
    CS_0 = 0,
    CS_1 = 1,
    CS_2 = 2,
    CS_3 = 3,
    CS_4 = 4,
    CS_5 = 5,
    CS_6 = 6,
    CS_7 = 7
}ChipSelect;


#define SPI_STATE_READY     0x00
#define SPI_STATE_RX_BUSY   (1<<1)
#define SPI_STATE_TX_BUSY   (1<<2)

typedef enum {
    SpiRxIrq,
    SpiTxIrq
} SpiIrq;

typedef void (*spi_irq_handler)(uint32_t id, SpiIrq event);

void spi_irq_hook(spi_t *obj, spi_irq_handler handler, uint32_t id);
int32_t spi_slave_read_stream(spi_t *obj, char *rx_buffer, uint32_t length);
int32_t spi_slave_write_stream(spi_t *obj, char *tx_buffer, uint32_t length);
int32_t spi_master_read_stream(spi_t *obj, char *rx_buffer, uint32_t length);
int32_t spi_master_write_stream(spi_t *obj, char *tx_buffer, uint32_t length);
int32_t spi_master_write_read_stream(spi_t *obj, char *tx_buffer, 
        char *rx_buffer, uint32_t length);
#ifdef CONFIG_GDMA_EN    
int32_t spi_slave_read_stream_dma(spi_t *obj, char *rx_buffer, uint32_t length);
int32_t spi_slave_write_stream_dma(spi_t *obj, char *tx_buffer, uint32_t length);
int32_t spi_master_read_stream_dma(spi_t *obj, char *rx_buffer, uint32_t length);
int32_t spi_master_write_stream_dma(spi_t *obj, char *tx_buffer, uint32_t length);
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
