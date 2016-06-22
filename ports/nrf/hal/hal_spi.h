/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#ifndef HAL_SPI_H__
#define HAL_SPI_H__

#include <stdbool.h>
#include "nrf.h"

#if NRF51
  #define SPI_BASE_POINTERS (const uint32_t[]){NRF_SPI0_BASE, NRF_SPI1_BASE}
  #define SPI_IRQ_VALUES (const uint32_t[]){SPI0_TWI0_IRQn, SPI1_TWI1_IRQn}
#endif

#if NRF52
  #ifdef NRF52832_XXAA
    #define SPI_BASE_POINTERS (const uint32_t[]){NRF_SPI0_BASE, \
                                                 NRF_SPI1_BASE, \
                                                 NRF_SPI2_BASE}
    #define SPI_IRQ_VALUES (const uint32_t[]){SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, \
                                              SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, \
                                              SPIM2_SPIS2_SPI2_IRQn}
  #endif

  #ifdef NRF52840_XXAA
    #define SPI_BASE_POINTERS (const uint32_t[]){NRF_SPI0_BASE, \
                                                 NRF_SPI1_BASE, \
                                                 NRF_SPI2_BASE, \
                                                 NRF_SPIM3_BASE}
    #define SPI_IRQ_VALUES (const uint32_t[]){SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, \
                                              SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, \
                                              SPIM2_SPIS2_SPI2_IRQn, \
                                              SPIM3_IRQn}
  #endif
#endif

#define SPI_BASE(x) ((NRF_SPI_Type *)SPI_BASE_POINTERS[x])
#define SPI_IRQ_NUM(x) (SPI_IRQ_VALUES[x])

/**
  * @brief  SPI clock frequency type definition
  */
typedef enum {
    HAL_SPI_FREQ_125_Kbps = 0,
    HAL_SPI_FREQ_250_Kbps,
    HAL_SPI_FREQ_500_Kbps,
    HAL_SPI_FREQ_1_Mbps,
    HAL_SPI_FREQ_2_Mbps,
    HAL_SPI_FREQ_4_Mbps,
    HAL_SPI_FREQ_8_Mbps,
#if NRF52840_XXAA
    HAL_SPI_FREQ_16_Mbps,
    HAL_SPI_FREQ_32_Mbps
#endif
} hal_spi_clk_freq_t;

/**
  * @brief  SPI mode type definition
  */
typedef enum {
    HAL_SPI_MODE_CPOL0_CPHA0 = 0, // CPOL = 0, CPHA = 0 (data on leading edge)
    HAL_SPI_MODE_CPOL0_CPHA1,     // CPOL = 0, CPHA = 1 (data on trailing edge)
    HAL_SPI_MODE_CPOL1_CPHA0,     // CPOL = 1, CPHA = 0 (data on leading edge)
    HAL_SPI_MODE_CPOL1_CPHA1      // CPOL = 1, CPHA = 1 (data on trailing edge)
} hal_spi_mode_t;

/**
 * @brief  SPI firstbit mode definition
 */
typedef enum {
    HAL_SPI_MSB_FIRST = 0,
    HAL_SPI_LSB_FIRST
} hal_spi_firstbit_t;

/**
  * @brief  SPI Configuration Structure definition
  */
typedef struct {
    const pin_obj_t * mosi_pin;
    const pin_obj_t * miso_pin;
    const pin_obj_t * clk_pin;
    hal_spi_firstbit_t firstbit;
    hal_spi_mode_t mode;
    uint32_t irq_priority;
    hal_spi_clk_freq_t freq;
} hal_spi_init_t;

/**
  * @brief  SPI handle Structure definition
  */
typedef struct __SPI_HandleTypeDef
{
    NRF_SPI_Type      *instance;    /* SPI registers base address */
    hal_spi_init_t    init;         /* SPI initialization parameters */
} SPI_HandleTypeDef;

void hal_spi_master_init(NRF_SPI_Type * p_instance, hal_spi_init_t const * p_spi_init);

void hal_spi_master_tx_rx(NRF_SPI_Type  * p_instance,
                          uint16_t        transfer_size,
                          const uint8_t * tx_data,
                          uint8_t       * rx_data);

#endif // HAL_SPI_H__
