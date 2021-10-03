#include <stdint.h>
#include <stddef.h>

#include "uart.h"

#if defined(QEMU_SOC_STM32)

typedef struct _UART_t {
    volatile uint32_t SR;
    volatile uint32_t DR;
} UART_t;

#define UART0 ((UART_t *)(0x40011000))

void uart_init(void) {
}

void uart_tx_strn(const char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        UART0->DR = buf[i];
    }
}

#elif defined(QEMU_SOC_NRF51)

typedef struct _UART_t {
    volatile uint32_t r0[2];
    volatile uint32_t STARTTX;  // 0x008
    volatile uint32_t r1[(0x500 - 0x008) / 4 - 1];
    volatile uint32_t ENABLE;   // 0x500
    volatile uint32_t r2[(0x51c - 0x500) / 4 - 1];
    volatile uint32_t TXD;      // 0x51c
} UART_t;

#define UART0 ((UART_t *)(0x40002000))

void uart_init(void) {
    UART0->ENABLE = 4;
    UART0->STARTTX = 1;
}

void uart_tx_strn(const char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        UART0->TXD = buf[i];
    }
}

#elif defined(QEMU_SOC_MPS2)

#define UART_STATE_TXFULL (1 << 0)

#define UART_CTRL_TX_EN (1 << 0)
#define UART_CTRL_RX_EN (1 << 1)

typedef struct _UART_t {
    volatile uint32_t DATA;
    volatile uint32_t STATE;
    volatile uint32_t CTRL;
    volatile uint32_t INTSTATUS;
    volatile uint32_t BAUDDIV;
} UART_t;

#define UART0 ((UART_t *)(0x40004000))

void uart_init(void) {
    UART0->BAUDDIV = 16;
    UART0->CTRL = UART_CTRL_TX_EN;
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
#define UART_UCR2_TXEN (1 << 2)

typedef struct _UART_t {
    volatile uint32_t URXD; // 0x00
    volatile uint32_t r0[15];
    volatile uint32_t UTXD; // 0x40
    volatile uint32_t r1[15];
    volatile uint32_t UCR1; // 0x80
    volatile uint32_t UCR2; // 0x84
} UART_t;

#define UART1 ((UART_t *)(0x02020000))

void uart_init(void) {
    UART1->UCR1 = UART_UCR1_UARTEN;
    UART1->UCR2 = UART_UCR2_TXEN;
}

void uart_tx_strn(const char *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        UART1->UTXD = buf[i];
    }
}

#endif
