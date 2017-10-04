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
#include "hal_twi.h"

#ifdef HAL_TWI_MODULE_ENABLED

static const uint32_t hal_twi_frequency_lookup[] = {
    TWI_FREQUENCY_FREQUENCY_K100, // 100 kbps
    TWI_FREQUENCY_FREQUENCY_K250, // 250 kbps
    TWI_FREQUENCY_FREQUENCY_K400, // 400 kbps
};

void hal_twi_master_init(NRF_TWI_Type * p_instance, hal_twi_init_t const * p_twi_init) {

#if NRF52840_XXAA
    p_instance->PSEL.SCL  = p_twi_init->scl_pin->pin;
    p_instance->PSEL.SDA  = p_twi_init->sda_pin->pin;
    p_instance->PSEL.SCL |= (p_twi_init->scl_pin->port << TWI_PSEL_SCL_PORT_Pos);
    p_instance->PSEL.SDA |= (p_twi_init->sda_pin->port << TWI_PSEL_SDA_PORT_Pos);
#else
    p_instance->PSELSCL  = p_twi_init->scl_pin->pin;
    p_instance->PSELSDA  = p_twi_init->sda_pin->pin;
#endif

    p_instance->FREQUENCY = hal_twi_frequency_lookup[p_twi_init->freq];
    p_instance->ENABLE    = (TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos);
}
#include <stdio.h>
void hal_twi_master_tx(NRF_TWI_Type  * p_instance,
                       uint8_t         addr,
                       uint16_t        transfer_size,
                       const uint8_t * tx_data,
                       bool            stop) {

    uint16_t number_of_txd_bytes = 0;

    p_instance->ADDRESS = addr;

    p_instance->EVENTS_TXDSENT = 0;

    p_instance->TXD            = tx_data[number_of_txd_bytes];
    p_instance->TASKS_STARTTX  = 1;

    while (number_of_txd_bytes < transfer_size) {
        // wait for the transaction complete
        while (p_instance->EVENTS_TXDSENT == 0) {
            ;
        }

        number_of_txd_bytes++;

        // TODO: This could go one byte out of bound.
        p_instance->TXD            = tx_data[number_of_txd_bytes];
        p_instance->EVENTS_TXDSENT = 0;
    }


    if (stop) {
        p_instance->EVENTS_STOPPED   = 0;
        p_instance->TASKS_STOP       = 1;

        while (p_instance->EVENTS_STOPPED == 0) {
            ;
        }
    }
}

void hal_twi_master_rx(NRF_TWI_Type  * p_instance,
                       uint8_t         addr,
                       uint16_t        transfer_size,
                       uint8_t       * rx_data,
                       bool            stop) {

    uint16_t number_of_rxd_bytes = 0;

    p_instance->ADDRESS = addr;

    p_instance->EVENTS_RXDREADY = 0;

    p_instance->TASKS_STARTRX  = 1;

    while (number_of_rxd_bytes < transfer_size) {
        // wait for the transaction complete
        while (p_instance->EVENTS_RXDREADY == 0) {
            ;
        }

        rx_data[number_of_rxd_bytes] = p_instance->RXD;
        p_instance->EVENTS_RXDREADY = 0;

        number_of_rxd_bytes++;
    }

    if (stop) {
        p_instance->EVENTS_STOPPED   = 0;
        p_instance->TASKS_STOP       = 1;

        while (p_instance->EVENTS_STOPPED == 0) {
            ;
        }
    }
}

void hal_twi_slave_init(NRF_TWI_Type * p_instance, hal_twi_init_t const * p_twi_init) {
}

#endif // HAL_TWI_MODULE_ENABLED

