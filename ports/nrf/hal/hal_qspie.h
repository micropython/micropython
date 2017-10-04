/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#ifndef HAL_QSPIE_H__
#define HAL_QSPIE_H__

#ifdef HAL_QSPIE_MODULE_ENABLED

#if NRF52840_XXAA

#include <nrf.h>

#else
#error "Device not supported."
#endif

/**
  * @brief  Quad SPI clock frequency type definition
  */
typedef enum {
    HAL_FREQ_2_Mbps,
    HAL_FREQ_4_Mbps,
    HAL_FREQ_8_Mbps,
    HAL_FREQ_16_Mbps,
    HAL_FREQ_32_Mbps
} hal_qspi_clk_freq_t;

/**
  * @brief  Quad SPI mode type definition
  */
typedef enum {
    HAL_SPI_MODE_CPOL0_CPHA0 = 0, // CPOL = 0, CPHA = 0 (data on leading edge)
    HAL_SPI_MODE_CPOL1_CPHA1 = 3  // CPOL = 1, CPHA = 1 (data on trailing edge)
} hal_qspi_mode_t;

/**
  * @brief  Quad SPI data line configuration type definition
  */
typedef enum {
    HAL_QSPI_DATA_LINE_SINGLE,
    HAL_QSPI_DATA_LINE_DUAL,
    HAL_QSPI_DATA_LINE_QUAD
} hal_qspi_data_line_t;



/**
  * @brief  Quad SPI Configuration Structure definition
  */
typedef struct {
    uint8_t d0_mosi_pin;
    uint8_t d1_miso_pin;
    uint8_t d2_pin;
    uint8_t d3_pin;
    uint8_t clk_pin;
    uint8_t csn_pin;
    uint8_t d0_mosi_pin_port;
    uint8_t d1_miso_pin_port;
    uint8_t d2_pin_port;
    uint8_t d3_pin_port;
    uint8_t clk_pin_port;
    uint8_t csn_pin_port;
    bool use_csn;
    hal_qspi_mode_t mode;
    hal_qspi_data_line_t data_line;
    hal_qspi_clk_freq_t freq;
} hal_qspi_init_t;

/**
  * @brief  Quad SPI handle Structure definition
  */
typedef struct __QSPI_HandleTypeDef
{
    NRF_QSPI_Type     *instance;    /* QSPI registers base address */
    hal_qspi_init_t   init;         /* QSPI initialization parameters */
} QSPI_HandleTypeDef;

void hal_qspi_master_init(NRF_QSPI_Type * p_instance, hal_qspi_init_t const * p_qspi_init);

void hal_qspi_master_tx_rx(NRF_QSPI_Type  * p_instance,
                           uint16_t         transfer_size,
                           const uint8_t  * tx_data,
                           uint8_t        * rx_data);

#endif // HAL_QSPIE_MODULE_ENABLED

#endif // HAL_QSPIE_H__
