/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#ifdef HAL_SPIE_MODULE_ENABLED

#if NRF52

#define SPIM0 ((NRF_SPI_Type *) NRF_SPIM0_BASE)
#define SPIM0_IRQ_NUM SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn
#define SPIM1 ((NRF_SPI_Type *) NRF_SPIM1_BASE)
#define SPIM1_IRQ_NUM SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn
#define SPIM2 ((NRF_SPI_Type *) NRF_SPIM2_BASE)
#define SPIM2_IRQ_NUM SPIM2_SPIS2_SPI2_IRQn

#else
#error "Device not supported."
#endif

static uint32_t m_ss_pin;

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
	// cast to master type
	NRF_SPIM_Type * spim_instance = (NRF_SPIM_Type *)p_instance;

    hal_gpio_pin_set(p_spi_init->enable_pin);
    m_ss_pin = p_spi_init->enable_pin;

    hal_gpio_cfg_pin(p_spi_init->clk_pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
    hal_gpio_cfg_pin(p_spi_init->mosi_pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
    hal_gpio_cfg_pin(p_spi_init->miso_pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);
    hal_gpio_cfg_pin(p_spi_init->enable_pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);

#if NRF51
    spim_instance->PSELSCK  = p_spi_init->clk_pin;
    spim_instance->PSELMOSI = p_spi_init->mosi_pin;
    spim_instance->PSELMISO = p_spi_init->miso_pin;
#else
    spim_instance->PSEL.SCK  = p_spi_init->clk_pin;
    spim_instance->PSEL.MOSI = p_spi_init->mosi_pin;
    spim_instance->PSEL.MISO = p_spi_init->miso_pin;
#endif

    spim_instance->FREQUENCY = hal_spi_frequency_lookup[p_spi_init->freq];

    uint32_t mode;
    switch (p_spi_init->mode) {
        case HAL_SPI_MODE_CPOL0_CPHA0:
            mode = (SPIM_CONFIG_CPHA_Leading << SPIM_CONFIG_CPHA_Pos) | (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos);
            break;
        case HAL_SPI_MODE_CPOL0_CPHA1:
            mode = (SPIM_CONFIG_CPHA_Trailing << SPIM_CONFIG_CPHA_Pos) | (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos);
            break;
        case HAL_SPI_MODE_CPOL1_CPHA0:
            mode = (SPIM_CONFIG_CPHA_Leading << SPIM_CONFIG_CPHA_Pos) | (SPIM_CONFIG_CPOL_ActiveLow << SPIM_CONFIG_CPOL_Pos);
            break;
        case HAL_SPI_MODE_CPOL1_CPHA1:
            mode = (SPIM_CONFIG_CPHA_Trailing << SPIM_CONFIG_CPHA_Pos) | (SPIM_CONFIG_CPOL_ActiveLow << SPIM_CONFIG_CPOL_Pos);
            break;
        default:
            mode = 0;
            break;
    }

    if (p_spi_init->lsb_first) {
    	spim_instance->CONFIG = (mode | (SPIM_CONFIG_ORDER_LsbFirst << SPIM_CONFIG_ORDER_Pos));
    } else {
    	spim_instance->CONFIG = (mode | (SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos));
    }

    spim_instance->EVENTS_END = 0;
    spim_instance->ENABLE = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);
}

void hal_spi_master_tx_rx(NRF_SPI_Type * p_instance, uint16_t transfer_size, const uint8_t * tx_data, uint8_t * rx_data) {

	// cast to master type
	NRF_SPIM_Type * spim_instance = (NRF_SPIM_Type *)p_instance;

    hal_gpio_pin_clear(m_ss_pin);

    spim_instance->TXD.PTR = (uint32_t)(tx_data);
    spim_instance->TXD.MAXCNT = transfer_size;
    spim_instance->RXD.PTR = (uint32_t)(rx_data);
    spim_instance->RXD.MAXCNT = transfer_size;

    spim_instance->TASKS_START = 1;

	while((0 == spim_instance->EVENTS_END));

	spim_instance->EVENTS_END  = 0;
	spim_instance->TASKS_STOP  = 1;

    hal_gpio_pin_set(m_ss_pin);
}

#endif // HAL_SPIE_MODULE_ENABLED
