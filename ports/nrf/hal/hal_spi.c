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

#include <stdint.h>
#include <stdio.h>

#include "mphalport.h"
#include "hal_spi.h"

#ifdef HAL_SPI_MODULE_ENABLED

static const uint32_t hal_spi_frequency_lookup[] = {
    SPI_FREQUENCY_FREQUENCY_K125, // 125 kbps
    SPI_FREQUENCY_FREQUENCY_K250, // 250 kbps
    SPI_FREQUENCY_FREQUENCY_K500, // 500 kbps
    SPI_FREQUENCY_FREQUENCY_M1, // 1 Mbps
    SPI_FREQUENCY_FREQUENCY_M2, // 2 Mbps
    SPI_FREQUENCY_FREQUENCY_M4, // 4 Mbps
    SPI_FREQUENCY_FREQUENCY_M8 // 8 Mbps
};

void hal_spi_master_init(NRF_SPI_Type * p_instance, hal_spi_init_t const * p_spi_init) {
    hal_gpio_cfg_pin(p_spi_init->clk_pin->port, p_spi_init->clk_pin->pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
    hal_gpio_cfg_pin(p_spi_init->mosi_pin->port, p_spi_init->mosi_pin->pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
    hal_gpio_cfg_pin(p_spi_init->miso_pin->port, p_spi_init->miso_pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);

#if NRF51
    p_instance->PSELSCK  = p_spi_init->clk_pin->pin;
    p_instance->PSELMOSI = p_spi_init->mosi_pin->pin;
    p_instance->PSELMISO = p_spi_init->miso_pin->pin;
#else
    p_instance->PSEL.SCK  = p_spi_init->clk_pin->pin;
    p_instance->PSEL.MOSI = p_spi_init->mosi_pin->pin;
    p_instance->PSEL.MISO = p_spi_init->miso_pin->pin;

#if NRF52840_XXAA
    p_instance->PSEL.SCK  |= (p_spi_init->clk_pin->port << SPI_PSEL_SCK_PORT_Pos);
    p_instance->PSEL.MOSI |= (p_spi_init->mosi_pin->port << SPI_PSEL_MOSI_PORT_Pos);
    p_instance->PSEL.MISO |= (p_spi_init->miso_pin->port << SPI_PSEL_MISO_PORT_Pos);
#endif

#endif

    p_instance->FREQUENCY = hal_spi_frequency_lookup[p_spi_init->freq];

    uint32_t mode;
    switch (p_spi_init->mode) {
        case HAL_SPI_MODE_CPOL0_CPHA0:
            mode = (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
            break;
        case HAL_SPI_MODE_CPOL0_CPHA1:
            mode = (SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
            break;
        case HAL_SPI_MODE_CPOL1_CPHA0:
            mode = (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
            break;
        case HAL_SPI_MODE_CPOL1_CPHA1:
            mode = (SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
            break;
        default:
            mode = 0;
            break;
    }

    if (p_spi_init->firstbit == HAL_SPI_LSB_FIRST) {
        p_instance->CONFIG = (mode | (SPI_CONFIG_ORDER_LsbFirst << SPI_CONFIG_ORDER_Pos));
    } else {
        p_instance->CONFIG = (mode | (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos));
    }

    p_instance->EVENTS_READY = 0U;
    p_instance->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
}

void hal_spi_master_tx_rx(NRF_SPI_Type  * p_instance,
                          uint16_t        transfer_size,
                          const uint8_t * tx_data,
                          uint8_t       * rx_data) {

    uint16_t number_of_txd_bytes = 0;

    p_instance->EVENTS_READY = 0;

    while (number_of_txd_bytes < transfer_size) {
        p_instance->TXD = (uint32_t)(tx_data[number_of_txd_bytes]);

        // wait for the transaction complete or timeout (about 10ms - 20 ms)
        while (p_instance->EVENTS_READY == 0) {
            ;
        }

        p_instance->EVENTS_READY = 0;

        uint8_t in_byte = (uint8_t)p_instance->RXD;

        if (rx_data != NULL) {
            rx_data[number_of_txd_bytes] = in_byte;
        }

        number_of_txd_bytes++;
    };
}

#endif // HAL_SPI_MODULE_ENABLED
