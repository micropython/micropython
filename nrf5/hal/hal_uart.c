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

#include "nrf.h"
#include "mphalport.h"
#include "hal_uart.h"

#ifdef HAL_UART_MODULE_ENABLED

#ifdef NRF51
#define UART_BASE   ((NRF_UART_Type *) NRF_UART0_BASE)
#define UART_IRQ_NUM UART0_IRQn
#else
#define UART_BASE   ((NRF_UART_Type *) NRF_UART0_BASE)
#define UART_IRQ_NUM UARTE0_UART0_IRQn
#endif

uint32_t hal_uart_baudrate_lookup[] = {
    UART_BAUDRATE_BAUDRATE_Baud1200,   ///< 1200 baud.
    UART_BAUDRATE_BAUDRATE_Baud2400,   ///< 2400 baud.
    UART_BAUDRATE_BAUDRATE_Baud4800,   ///< 4800 baud.
    UART_BAUDRATE_BAUDRATE_Baud9600,   ///< 9600 baud.
    UART_BAUDRATE_BAUDRATE_Baud14400,  ///< 14400 baud.
    UART_BAUDRATE_BAUDRATE_Baud19200,  ///< 19200 baud.
    UART_BAUDRATE_BAUDRATE_Baud28800,  ///< 28800 baud.
    UART_BAUDRATE_BAUDRATE_Baud38400,  ///< 38400 baud.
    UART_BAUDRATE_BAUDRATE_Baud57600,  ///< 57600 baud.
    UART_BAUDRATE_BAUDRATE_Baud76800,  ///< 76800 baud.
    UART_BAUDRATE_BAUDRATE_Baud115200, ///< 115200 baud.
    UART_BAUDRATE_BAUDRATE_Baud230400, ///< 230400 baud.
    UART_BAUDRATE_BAUDRATE_Baud250000, ///< 250000 baud.
    UART_BAUDRATE_BAUDRATE_Baud460800, ///< 460800 baud.
    UART_BAUDRATE_BAUDRATE_Baud921600, ///< 921600 baud.
    UART_BAUDRATE_BAUDRATE_Baud1M,     ///< 1000000 baud.
};

void nrf_uart_char_write(uint8_t ch) {
    UART_BASE->TXD = (uint8_t)ch;
    while (UART_BASE->EVENTS_TXDRDY != 1) {
        // Blocking wait.
    }

    // Clear the TX flag.
    UART_BASE->EVENTS_TXDRDY = 0;
}

uint8_t nrf_uart_char_read(void) {
    while (UART_BASE->EVENTS_RXDRDY != 1) {
        // Wait for RXD data.
    }

    UART_BASE->EVENTS_RXDRDY = 0;
    return (uint8_t)UART_BASE->RXD;
}

void nrf_uart_buffer_write(uint8_t * p_buffer, uint32_t num_of_bytes, uart_complete_cb cb) {
    int i = 0;
    uint8_t ch = p_buffer[i++];
    while (i < num_of_bytes) {
        nrf_uart_char_write(ch);
        ch = p_buffer[i++];
    }
    cb();
}

void nrf_uart_buffer_read(uint8_t * p_buffer, uint32_t num_of_bytes, uart_complete_cb cb) {
    int i = 0;
    while (i < num_of_bytes) {
        uint8_t ch = nrf_uart_char_read();
        p_buffer[i] = ch;
        i++;
    }
    cb();
}

void nrf_uart_init(hal_uart_init_t const * p_uart_init) {
    hal_gpio_cfg_pin(p_uart_init->tx_pin->port, p_uart_init->tx_pin->pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
    hal_gpio_cfg_pin(p_uart_init->tx_pin->port, p_uart_init->rx_pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);

    hal_gpio_pin_clear(p_uart_init->tx_pin->port, p_uart_init->tx_pin->pin);

    UART_BASE->PSELTXD = p_uart_init->tx_pin->pin;
    UART_BASE->PSELRXD = p_uart_init->rx_pin->pin;

#if NRF52840_XXAA
    UART_BASE->PSELTXD |= (p_uart_init->tx_pin->port << UARTE_PSEL_TXD_PORT_Pos);
    UART_BASE->PSELRXD |= (p_uart_init->rx_pin->port << UARTE_PSEL_RXD_PORT_Pos);
#endif

    if (p_uart_init->flow_control) {
        hal_gpio_cfg_pin(p_uart_init->rts_pin->port, p_uart_init->rts_pin->pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
        hal_gpio_cfg_pin(p_uart_init->cts_pin->port, p_uart_init->cts_pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);

        UART_BASE->PSELCTS = p_uart_init->cts_pin->pin;
        UART_BASE->PSELRTS = p_uart_init->rts_pin->pin;

#if NRF52840_XXAA
        UART_BASE->PSELCTS |= (p_uart_init->cts_pin->port << UARTE_PSEL_CTS_PORT_Pos);
        UART_BASE->PSELRTS |= (p_uart_init->rts_pin->port << UARTE_PSEL_RTS_PORT_Pos);
#endif

        UART_BASE->CONFIG  = (UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos);
    }

    UART_BASE->BAUDRATE      = (hal_uart_baudrate_lookup[p_uart_init->baud_rate]);
    UART_BASE->ENABLE        = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    UART_BASE->EVENTS_TXDRDY = 0;
    UART_BASE->EVENTS_RXDRDY = 0;
    UART_BASE->TASKS_STARTTX = 1;
    UART_BASE->TASKS_STARTRX = 1;
}

#endif // HAL_UART_MODULE_ENABLED
