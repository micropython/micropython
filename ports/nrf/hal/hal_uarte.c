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

#include "hal_uart.h"
#include "hal_irq.h"

#ifdef HAL_UARTE_MODULE_ENABLED

#include "nrf.h"

#ifndef NRF52
#error "Device not supported."
#endif

#define UART_BASE(x) ((NRF_UART_Type *)UART_BASE_POINTERS[x])
#define UART_IRQ_NUM(x) (UART_IRQ_VALUES[x])

#define TX_BUF_SIZE 1
#define RX_BUF_SIZE 1

static const uint32_t hal_uart_baudrate_lookup[] = {
    UARTE_BAUDRATE_BAUDRATE_Baud1200,   ///< 1200 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud2400,   ///< 2400 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud4800,   ///< 4800 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud9600,   ///< 9600 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud14400,  ///< 14400 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud19200,  ///< 19200 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud28800,  ///< 28800 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud38400,  ///< 38400 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud57600,  ///< 57600 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud76800,  ///< 76800 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud115200, ///< 115200 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud230400, ///< 230400 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud250000, ///< 250000 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud460800, ///< 460800 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud921600, ///< 921600 baud.
    UARTE_BAUDRATE_BAUDRATE_Baud1M,     ///< 1000000 baud.
};

void nrf_sendchar(NRF_UART_Type * p_instance, int ch) {
    hal_uart_char_write(p_instance, ch);
}

void hal_uart_init(NRF_UART_Type * p_instance, hal_uart_init_t const * p_uart_init) {

    NRF_UARTE_Type * uarte_instance = (NRF_UARTE_Type *)p_instance;

    hal_gpio_cfg_pin(p_uart_init->tx_pin->port, p_uart_init->tx_pin->pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
    hal_gpio_pin_set(p_uart_init->tx_pin->port, p_uart_init->tx_pin->pin);
    hal_gpio_cfg_pin(p_uart_init->tx_pin->port, p_uart_init->rx_pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);

    uarte_instance->BAUDRATE = (hal_uart_baudrate_lookup[p_uart_init->baud_rate]);

    uint32_t hwfc   = (p_uart_init->flow_control) 
                      ? (UARTE_CONFIG_HWFC_Enabled << UARTE_CONFIG_HWFC_Pos)
                      : (UARTE_CONFIG_HWFC_Disabled << UARTE_CONFIG_HWFC_Pos);

    uint32_t parity = (p_uart_init->use_parity)
                      ? (UARTE_CONFIG_PARITY_Included << UARTE_CONFIG_PARITY_Pos)
                      : (UARTE_CONFIG_PARITY_Excluded << UARTE_CONFIG_PARITY_Pos);

    uarte_instance->CONFIG = (uint32_t)hwfc | (uint32_t)parity;

    uarte_instance->PSEL.RXD = p_uart_init->rx_pin->pin;
    uarte_instance->PSEL.TXD = p_uart_init->tx_pin->pin;

#if NRF52840_XXAA
    uarte_instance->PSEL.RXD |= (p_uart_init->rx_pin->port << UARTE_PSEL_RXD_PORT_Pos);
    uarte_instance->PSEL.TXD |= (p_uart_init->tx_pin->port << UARTE_PSEL_TXD_PORT_Pos);
#endif

    if (hwfc) {
        hal_gpio_cfg_pin(p_uart_init->cts_pin->port, p_uart_init->cts_pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);
        hal_gpio_cfg_pin(p_uart_init->rts_pin->port, p_uart_init->rts_pin->pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
        hal_gpio_pin_set(p_uart_init->rts_pin->port, p_uart_init->rts_pin->pin);

        uarte_instance->PSEL.RTS = p_uart_init->rts_pin->pin;
        uarte_instance->PSEL.CTS = p_uart_init->cts_pin->pin;

#if NRF52840_XXAA
        uarte_instance->PSEL.RTS |= (p_uart_init->rx_pin->port << UARTE_PSEL_RTS_PORT_Pos);
        uarte_instance->PSEL.CTS |= (p_uart_init->rx_pin->port << UARTE_PSEL_CTS_PORT_Pos);
#endif
    }

    hal_irq_priority(p_uart_init->irq_num, p_uart_init->irq_priority);
    hal_irq_enable(p_uart_init->irq_num);

    uarte_instance->INTENSET = (UARTE_INTENSET_ENDRX_Set << UARTE_INTENSET_ENDRX_Pos);
    uarte_instance->INTENSET = (UARTE_INTENSET_ENDTX_Set << UARTE_INTENSET_ENDTX_Pos);

    uarte_instance->ENABLE = (UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos);

    uarte_instance->EVENTS_ENDTX  = 0;
    uarte_instance->EVENTS_ENDRX  = 0;
}

hal_uart_error_t hal_uart_char_write(NRF_UART_Type * p_instance, uint8_t ch) {

    NRF_UARTE_Type * uarte_instance = (NRF_UARTE_Type *)p_instance;

    uarte_instance->ERRORSRC = 0;


    static volatile uint8_t m_tx_buf[TX_BUF_SIZE];
    (void)m_tx_buf;

    uarte_instance->INTENCLR = (UARTE_INTENSET_ENDTX_Set << UARTE_INTENSET_ENDTX_Pos);

    m_tx_buf[0] = ch;

    uarte_instance->TXD.PTR = (uint32_t)((uint8_t *)m_tx_buf);
    uarte_instance->TXD.MAXCNT = (uint32_t)sizeof(m_tx_buf);

    uarte_instance->TASKS_STARTTX = 1;

    while((0 == uarte_instance->EVENTS_ENDTX));

    uarte_instance->EVENTS_ENDTX  = 0;
    uarte_instance->TASKS_STOPTX  = 1;

    uarte_instance->INTENSET = (UARTE_INTENSET_ENDTX_Set << UARTE_INTENSET_ENDTX_Pos);

    return uarte_instance->ERRORSRC;
}

hal_uart_error_t hal_uart_char_read(NRF_UART_Type * p_instance, uint8_t * ch) {

    NRF_UARTE_Type * uarte_instance = (NRF_UARTE_Type *)p_instance;

    uarte_instance->ERRORSRC = 0;

    static volatile uint8_t m_rx_buf[RX_BUF_SIZE];

    uarte_instance->INTENCLR = (UARTE_INTENSET_ENDRX_Set << UARTE_INTENSET_ENDRX_Pos);

    uarte_instance->RXD.PTR = (uint32_t)((uint8_t *)m_rx_buf);
    uarte_instance->RXD.MAXCNT = (uint32_t)sizeof(m_rx_buf);

    uarte_instance->TASKS_STARTRX = 1;

    while ((0 == uarte_instance->EVENTS_ENDRX));

    uarte_instance->EVENTS_ENDRX  = 0;
    uarte_instance->TASKS_STOPRX  = 1;

    uarte_instance->INTENSET = (UARTE_INTENSET_ENDRX_Set << UARTE_INTENSET_ENDRX_Pos);
    *ch = (uint8_t)m_rx_buf[0];

    return uarte_instance->ERRORSRC;
}

#endif // HAL_UARTE_MODULE_ENABLED
