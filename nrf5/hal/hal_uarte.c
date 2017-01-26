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

#include "hal_uart.h"

#ifdef HAL_UARTE_MODULE_ENABLED

#include "nrf.h"

#if NRF52

#define UARTE_BASE   ((NRF_UARTE_Type *) NRF_UARTE0_BASE)
#define UART_IRQ_NUM UARTE0_UART0_IRQn

#else
#error "Device not supported."
#endif

#define TX_BUF_SIZE 1
#define RX_BUF_SIZE 1

static uart_complete_cb dma_read_cb  = NULL;
static uart_complete_cb dma_write_cb = NULL;

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

__STATIC_INLINE void nrf_uart_irq_clear(void) {
    NVIC_ClearPendingIRQ(UART_IRQ_NUM);
}

__STATIC_INLINE void nrf_uart_irq_enable(uint8_t priority) {
    NVIC_SetPriority(UART_IRQ_NUM, priority);
    nrf_uart_irq_clear();
    NVIC_EnableIRQ(UART_IRQ_NUM);
}

void nrf_sendchar(int ch) {
    nrf_uart_char_write(ch);
}

void nrf_uart_init(hal_uart_init_t const * p_uart_init) {
    hal_gpio_cfg_pin(p_uart_init->tx_pin->port, p_uart_init->tx_pin->pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
    hal_gpio_pin_set(p_uart_init->tx_pin->port, p_uart_init->tx_pin->pin);
    hal_gpio_cfg_pin(p_uart_init->tx_pin->port, p_uart_init->rx_pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);

    UARTE_BASE->BAUDRATE = (hal_uart_baudrate_lookup[p_uart_init->baud_rate]);

    uint32_t hwfc   = (p_uart_init->flow_control) 
                      ? (UARTE_CONFIG_HWFC_Enabled << UARTE_CONFIG_HWFC_Pos)
                      : (UARTE_CONFIG_HWFC_Disabled << UARTE_CONFIG_HWFC_Pos);

    uint32_t parity = (p_uart_init->use_parity)
                      ? (UARTE_CONFIG_PARITY_Included << UARTE_CONFIG_PARITY_Pos)
                      : (UARTE_CONFIG_PARITY_Excluded << UARTE_CONFIG_PARITY_Pos);

    UARTE_BASE->CONFIG = (uint32_t)hwfc | (uint32_t)parity;

    UARTE_BASE->PSEL.RXD = p_uart_init->rx_pin->pin;
    UARTE_BASE->PSEL.TXD = p_uart_init->tx_pin->pin;

#if NRF52840_XXAA
    UARTE_BASE->PSEL.RXD |= (p_uart_init->rx_pin->port << UARTE_PSEL_RXD_PORT_Pos);
    UARTE_BASE->PSEL.TXD |= (p_uart_init->tx_pin->port << UARTE_PSEL_TXD_PORT_Pos);
#endif

    if (hwfc) {
        hal_gpio_cfg_pin(p_uart_init->cts_pin->port, p_uart_init->cts_pin->pin, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_DISABLED);
        hal_gpio_cfg_pin(p_uart_init->rts_pin->port, p_uart_init->rts_pin->pin, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DISABLED);
        hal_gpio_pin_set(p_uart_init->rts_pin->port, p_uart_init->rts_pin->pin);

        UARTE_BASE->PSEL.RTS = p_uart_init->rts_pin->pin;
        UARTE_BASE->PSEL.CTS = p_uart_init->cts_pin->pin;

#if NRF52840_XXAA
        UARTE_BASE->PSEL.RTS |= (p_uart_init->rx_pin->port << UARTE_PSEL_RTS_PORT_Pos);
        UARTE_BASE->PSEL.CTS |= (p_uart_init->rx_pin->port << UARTE_PSEL_CTS_PORT_Pos);
#endif
    }

    nrf_uart_irq_enable(p_uart_init->irq_priority);

    UARTE_BASE->INTENSET = (UARTE_INTENSET_ENDRX_Set << UARTE_INTENSET_ENDRX_Pos);
    UARTE_BASE->INTENSET = (UARTE_INTENSET_ENDTX_Set << UARTE_INTENSET_ENDTX_Pos);

    UARTE_BASE->ENABLE = (UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos);

    UARTE_BASE->EVENTS_ENDTX  = 0;
    UARTE_BASE->EVENTS_ENDRX  = 0;
}

void nrf_uart_char_write(uint8_t ch) {
    static volatile uint8_t m_tx_buf[TX_BUF_SIZE];
    (void)m_tx_buf;

    UARTE_BASE->INTENCLR = (UARTE_INTENSET_ENDTX_Set << UARTE_INTENSET_ENDTX_Pos);

    m_tx_buf[0] = ch;

    UARTE_BASE->TXD.PTR = (uint32_t)((uint8_t *)m_tx_buf);
    UARTE_BASE->TXD.MAXCNT = (uint32_t)sizeof(m_tx_buf);

    UARTE_BASE->TASKS_STARTTX = 1;

    while((0 == UARTE_BASE->EVENTS_ENDTX));

    UARTE_BASE->EVENTS_ENDTX  = 0;
    UARTE_BASE->TASKS_STOPTX  = 1;

    UARTE_BASE->INTENSET = (UARTE_INTENSET_ENDTX_Set << UARTE_INTENSET_ENDTX_Pos);
}

uint8_t nrf_uart_char_read(void) {
    static volatile uint8_t m_rx_buf[RX_BUF_SIZE];

    UARTE_BASE->INTENCLR = (UARTE_INTENSET_ENDRX_Set << UARTE_INTENSET_ENDRX_Pos);

    UARTE_BASE->RXD.PTR = (uint32_t)((uint8_t *)m_rx_buf);
    UARTE_BASE->RXD.MAXCNT = (uint32_t)sizeof(m_rx_buf);

    UARTE_BASE->TASKS_STARTRX = 1;

    while ((0 == UARTE_BASE->EVENTS_ENDRX));

    UARTE_BASE->EVENTS_ENDRX  = 0;
    UARTE_BASE->TASKS_STOPRX  = 1;

    UARTE_BASE->INTENSET = (UARTE_INTENSET_ENDRX_Set << UARTE_INTENSET_ENDRX_Pos);

    return (uint8_t)m_rx_buf[0];
}

void nrf_uart_buffer_write(uint8_t * p_buffer, uint32_t num_of_bytes, uart_complete_cb cb) {
    dma_write_cb = cb;

    UARTE_BASE->TXD.PTR       = (uint32_t)p_buffer;
    UARTE_BASE->TXD.MAXCNT    = num_of_bytes;
    UARTE_BASE->TASKS_STARTTX = 1;

    while((0 == UARTE_BASE->EVENTS_ENDTX));

    UARTE_BASE->EVENTS_ENDTX  = 0;
    UARTE_BASE->TASKS_STOPTX  = 1;

    UARTE_BASE->INTENSET = (UARTE_INTENSET_ENDTX_Set << UARTE_INTENSET_ENDTX_Pos);

}

void nrf_uart_buffer_read(uint8_t * p_buffer, uint32_t num_of_bytes, uart_complete_cb cb) {
    dma_read_cb = cb;

    UARTE_BASE->RXD.PTR       = (uint32_t)(p_buffer);
    UARTE_BASE->RXD.MAXCNT    = num_of_bytes;
    UARTE_BASE->TASKS_STARTRX = 1;

    while ((0 == UARTE_BASE->EVENTS_ENDRX));

    UARTE_BASE->EVENTS_ENDRX  = 0;
    UARTE_BASE->TASKS_STOPRX  = 1;

    UARTE_BASE->INTENSET = (UARTE_INTENSET_ENDRX_Set << UARTE_INTENSET_ENDRX_Pos);

}

static void dma_read_complete(void) {
    UARTE_BASE->TASKS_STOPRX  = 1;

    if (dma_read_cb != NULL) {
        uart_complete_cb temp_cb = dma_read_cb;
        dma_read_cb = NULL;
        temp_cb();
    }
}

static void dma_write_complete(void) {
    UARTE_BASE->TASKS_STOPTX  = 1;

    if (dma_write_cb != NULL) {
        uart_complete_cb temp_cb = dma_write_cb;
        dma_write_cb = NULL;
        temp_cb();
    }
}

void UARTE0_UART0_IRQHandler(void) {
    if ((UARTE_BASE->EVENTS_ENDRX) 
        && (UARTE_BASE->INTEN & UARTE_INTENSET_ENDRX_Msk)) {
        
        UARTE_BASE->EVENTS_ENDRX = 0;
        dma_read_complete();

    } else if ((UARTE_BASE->EVENTS_ENDTX) 
        && (UARTE_BASE->INTEN & UARTE_INTENSET_ENDTX_Msk)) {

        UARTE_BASE->EVENTS_ENDTX = 0;
        dma_write_complete();
    }
}

#endif // HAL_UARTE_MODULE_ENABLED
