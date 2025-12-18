/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2024 Damien P. George
 * Copyright (c) 2023 Alessandro Gatti
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
#include <stddef.h>

#include "uart.h"

#if defined(QEMU_SOC_STM32)

#define UART_SR_RXNE (1 << 5)
#define UART_CR1_UE (1 << 13)
#define UART_CR1_TE (1 << 3)
#define UART_CR1_RE (1 << 2)

typedef struct _UART_t {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
} UART_t;

#define UART0 ((UART_t *)(0x40011000))

void uart_init(void) {
    UART0->CR1 = UART_CR1_UE | UART_CR1_TE | UART_CR1_RE;
}

int uart_rx_chr(void) {
    if (!(UART0->SR & UART_SR_RXNE)) {
        return UART_RX_NO_CHAR;
    }
    return UART0->DR;
}

int uart_rx_any(void) {
    return UART0->SR & UART_SR_RXNE;
}

void uart_tx_strn(const char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        UART0->DR = buf[i];
    }
}

#elif defined(QEMU_SOC_NRF51)

typedef struct _UART_t {
    volatile uint32_t STARTRX;  // 0x000
    volatile uint32_t STOPRX;   // 0x004
    volatile uint32_t STARTTX;  // 0x008
    volatile uint32_t r0[(0x108 - 0x008) / 4 - 1];
    volatile uint32_t RXDRDY;  // 0x108
    volatile uint32_t r1[(0x500 - 0x108) / 4 - 1];
    volatile uint32_t ENABLE;   // 0x500
    volatile uint32_t r2[(0x518 - 0x500) / 4 - 1];
    volatile uint32_t RXD;      // 0x518
    volatile uint32_t TXD;      // 0x51c
} UART_t;

#define UART0 ((UART_t *)(0x40002000))

void uart_init(void) {
    UART0->ENABLE = 4;
    UART0->STARTRX = 1;
    UART0->STARTTX = 1;
}

int uart_rx_chr(void) {
    if (!UART0->RXDRDY) {
        return UART_RX_NO_CHAR;
    }
    UART0->RXDRDY = 0;
    return UART0->RXD;
}

int uart_rx_any(void) {
    return UART0->RXDRDY ? 1 : 0;
}

void uart_tx_strn(const char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        UART0->TXD = buf[i];
    }
}

#elif defined(QEMU_SOC_MPS2) || defined(QEMU_SOC_MPS3)

#define UART_STATE_TXFULL (1 << 0)
#define UART_STATE_RXFULL (1 << 1)

#define UART_CTRL_TX_EN (1 << 0)
#define UART_CTRL_RX_EN (1 << 1)

typedef struct _UART_t {
    volatile uint32_t DATA;
    volatile uint32_t STATE;
    volatile uint32_t CTRL;
    volatile uint32_t INTSTATUS;
    volatile uint32_t BAUDDIV;
} UART_t;

#if defined(QEMU_SOC_MPS3)
#define UART0 ((UART_t *)(0x49303000))
#else
#define UART0 ((UART_t *)(0x40004000))
#endif

void uart_init(void) {
    UART0->BAUDDIV = 16;
    UART0->CTRL = UART_CTRL_TX_EN | UART_CTRL_RX_EN;
}

int uart_rx_chr(void) {
    if (!(UART0->STATE & UART_STATE_RXFULL)) {
        return UART_RX_NO_CHAR;
    }
    return UART0->DATA;
}

int uart_rx_any(void) {
    return UART0->STATE & UART_STATE_RXFULL;
}

void uart_tx_strn(const char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        while (UART0->STATE & UART_STATE_TXFULL) {
        }
        UART0->DATA = buf[i];
    }
}

#elif defined(QEMU_SOC_IMX6)

#define UART_UCR1_UARTEN (1 << 0)
#define UART_UCR2_RXEN (1 << 1)
#define UART_UCR2_TXEN (1 << 2)
#define UART_UTS1_RXEMPTY (1 << 5)

typedef struct _UART_t {
    volatile uint32_t URXD; // 0x00
    volatile uint32_t r0[15];
    volatile uint32_t UTXD; // 0x40
    volatile uint32_t r1[15];
    volatile uint32_t UCR1; // 0x80
    volatile uint32_t UCR2; // 0x84
    volatile uint32_t r2[11];
    volatile uint32_t UTS1; // 0xb4
} UART_t;

#define UART1 ((UART_t *)(0x02020000))

void uart_init(void) {
    UART1->UCR1 = UART_UCR1_UARTEN;
    UART1->UCR2 = UART_UCR2_TXEN | UART_UCR2_RXEN;
}

int uart_rx_chr(void) {
    if (UART1->UTS1 & UART_UTS1_RXEMPTY) {
        return UART_RX_NO_CHAR;
    }
    return UART1->URXD & 0xff;
}

int uart_rx_any(void) {
    return !(UART1->UTS1 & UART_UTS1_RXEMPTY);
}

void uart_tx_strn(const char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        UART1->UTXD = buf[i];
    }
}

#elif defined(QEMU_SOC_VIRT)

// Line status register bits.
#define UART_LSR_THRE (0x20)
#define UART_LSR_DR (0x01)

typedef struct _UART_t {
    volatile uint8_t DR;
    volatile uint8_t r0[4];
    volatile uint8_t LSR;
} UART_t;

#define UART0 ((UART_t *)(0x10000000))

void uart_init(void) {
}

int uart_rx_chr(void) {
    if (UART0->LSR & UART_LSR_DR) {
        return UART0->DR;
    }
    return UART_RX_NO_CHAR;
}

int uart_rx_any(void) {
    return UART0->LSR & UART_LSR_DR;
}

void uart_tx_strn(const char *buffer, size_t length) {
    for (size_t index = 0; index < length; index++) {
        while (!(UART0->LSR & UART_LSR_THRE)) {
        }
        UART0->DR = buffer[index];
    }
}

#endif
