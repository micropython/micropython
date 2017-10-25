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

#include "mphalport.h"
#include "hal_qspie.h"

#ifdef HAL_QSPIE_MODULE_ENABLED

#define QSPI_IRQ_NUM QSPI_IRQn
#define QSPI_BASE ((NRF_QSPI_Type *)NRF_QSPI_BASE)

// frequency, 32 MHz / (SCKFREQ + 1)
static const uint32_t hal_qspi_frequency_lookup[] = {
    (15 << QSPI_IFCONFIG1_SCKFREQ_Pos), // 2 Mbps
    (7  << QSPI_IFCONFIG1_SCKFREQ_Pos), // 4 Mbps
    (3  << QSPI_IFCONFIG1_SCKFREQ_Pos), // 8 Mbps
    (1  << QSPI_IFCONFIG1_SCKFREQ_Pos), // 16 Mbps
    (0  << QSPI_IFCONFIG1_SCKFREQ_Pos), // 32 Mbps
};

void hal_qspi_master_init(NRF_QSPI_Type * p_instance, hal_qspi_init_t const * p_qspi_init)
{
	// configure SCK
    p_instance->PSEL.SCK = (p_qspi_init->clk_pin << QSPI_PSEL_SCK_PIN_Pos)
                         | (p_qspi_init->clk_pin_port << QSPI_PSEL_SCK_PORT_Pos)
                         | (QSPI_PSEL_SCK_CONNECT_Connected << QSPI_PSEL_SCK_CONNECT_Pos);

    // configure CS
    if (p_qspi_init->use_csn) {
        p_instance->PSEL.CSN = (p_qspi_init->clk_pin << QSPI_PSEL_CSN_PIN_Pos)
                             | (p_qspi_init->clk_pin_port << QSPI_PSEL_CSN_PORT_Pos)
                             | (QSPI_PSEL_CSN_CONNECT_Connected << QSPI_PSEL_CSN_CONNECT_Pos);
    } else {
        p_instance->PSEL.CSN = (QSPI_PSEL_CSN_CONNECT_Disconnected << QSPI_PSEL_CSN_CONNECT_Pos);
    }

    // configure MOSI/IO0, valid for all configurations
    p_instance->PSEL.IO0 = (p_qspi_init->d0_mosi_pin << QSPI_PSEL_IO0_PIN_Pos)
                        | (p_qspi_init->d0_mosi_pin_port << QSPI_PSEL_IO0_PORT_Pos)
                        | (QSPI_PSEL_IO0_CONNECT_Connected << QSPI_PSEL_IO0_CONNECT_Pos);

    if (p_qspi_init->data_line != HAL_QSPI_DATA_LINE_SINGLE) {
        // configure MISO/IO1
        p_instance->PSEL.IO1 = (p_qspi_init->d1_miso_pin << QSPI_PSEL_IO1_PIN_Pos)
                            | (p_qspi_init->d1_miso_pin_port << QSPI_PSEL_IO1_PORT_Pos)
                            | (QSPI_PSEL_IO1_CONNECT_Connected << QSPI_PSEL_IO1_CONNECT_Pos);

        if (p_qspi_init->data_line == HAL_QSPI_DATA_LINE_QUAD) {
            // configure IO2
            p_instance->PSEL.IO2 = (p_qspi_init->d2_pin << QSPI_PSEL_IO2_PIN_Pos)
                                | (p_qspi_init->d2_pin_port << QSPI_PSEL_IO2_PORT_Pos)
                                | (QSPI_PSEL_IO2_CONNECT_Connected << QSPI_PSEL_IO2_CONNECT_Pos);

            // configure IO3
            p_instance->PSEL.IO3 = (p_qspi_init->d3_pin << QSPI_PSEL_IO3_PIN_Pos)
                                | (p_qspi_init->d3_pin_port << QSPI_PSEL_IO3_PORT_Pos)
                                | (QSPI_PSEL_IO3_CONNECT_Connected << QSPI_PSEL_IO3_CONNECT_Pos);
        }
    }

    uint32_t mode;
    switch (p_qspi_init->mode) {
        case HAL_SPI_MODE_CPOL0_CPHA0:
            mode = (QSPI_IFCONFIG1_SPIMODE_MODE0 << QSPI_IFCONFIG1_SPIMODE_Pos);
            break;
        case HAL_SPI_MODE_CPOL1_CPHA1:
            mode = (QSPI_IFCONFIG1_SPIMODE_MODE3 << QSPI_IFCONFIG1_SPIMODE_Pos);
            break;
        default:
            mode = 0;
            break;
    }

    // interface config1
    p_instance->IFCONFIG1 = hal_qspi_frequency_lookup[p_qspi_init->freq]
                          | mode
                          | (1 << QSPI_IFCONFIG1_SCKDELAY_Pos); // number of 16 MHz periods (62.5 ns)

    p_instance->ENABLE = 1;
}

void hal_qspi_master_tx_rx(NRF_QSPI_Type  * p_instance,
                           uint16_t         transfer_size,
                           const uint8_t  * tx_data,
                           uint8_t        * rx_data)
{
	p_instance->READ.DST = (uint32_t)rx_data;
	p_instance->READ.CNT = transfer_size;
	p_instance->READ.SRC = (uint32_t)tx_data;
	p_instance->READ.CNT = transfer_size;
	p_instance->TASKS_ACTIVATE = 1;
	while (p_instance->EVENTS_READY == 0) {
		;
	}

	p_instance->TASKS_ACTIVATE = 0;
}

#endif // HAL_QSPIE_MODULE_ENABLED
