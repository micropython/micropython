/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: uart.c
 *
 * Description: Two UART mode configration and interrupt handler.
 *              Check your hardware connection while use this mode.
 *
 * Modification history:
 *     2014/3/12, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "uart.h"
#include "osapi.h"
#include "uart_register.h"
#include "etshal.h"
#include "c_types.h"
#include "user_interface.h"
#include "esp_mphal.h"

// seems that this is missing in the Espressif SDK
#define FUNC_U0RXD 0

#define UART_REPL UART0

// UartDev is defined and initialized in rom code.
extern UartDevice UartDev;

// the uart to which OS messages go; -1 to disable
static int uart_os = UART_OS;

#if MICROPY_REPL_EVENT_DRIVEN
static os_event_t uart_evt_queue[16];
#endif

// A small, static ring buffer for incoming chars
// This will only be populated if the UART is not attached to dupterm
uint8 uart_ringbuf_array[UART0_STATIC_RXBUF_LEN];
static ringbuf_t uart_ringbuf = {uart_ringbuf_array, sizeof(uart_ringbuf_array), 0, 0};

static void uart0_rx_intr_handler(void *para);

void soft_reset(void);
void mp_keyboard_interrupt(void);

/******************************************************************************
 * FunctionName : uart_config
 * Description  : Internal used function
 *                UART0 used for data TX/RX, RX buffer size is 0x100, interrupt enabled
 *                UART1 just used for debug output
 * Parameters   : uart_no, use UART0 or UART1 defined ahead
 * Returns      : NONE
*******************************************************************************/
static void ICACHE_FLASH_ATTR uart_config(uint8 uart_no) {
    if (uart_no == UART1) {
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);
    } else {
        ETS_UART_INTR_ATTACH(uart0_rx_intr_handler, NULL);
        PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD);
    }

    uart_div_modify(uart_no, UART_CLK_FREQ / (UartDev.baut_rate));

    WRITE_PERI_REG(UART_CONF0(uart_no), UartDev.exist_parity
        | UartDev.parity
        | (UartDev.stop_bits << UART_STOP_BIT_NUM_S)
        | (UartDev.data_bits << UART_BIT_NUM_S));

    // clear rx and tx fifo,not ready
    SET_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);
    CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);

    if (uart_no == UART0) {
        // set rx fifo trigger
        WRITE_PERI_REG(UART_CONF1(uart_no),
            ((0x10 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S) |
            ((0x10 & UART_RX_FLOW_THRHD) << UART_RX_FLOW_THRHD_S) |
            UART_RX_FLOW_EN |
            (0x02 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S |
                UART_RX_TOUT_EN);
        SET_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_TOUT_INT_ENA |
            UART_FRM_ERR_INT_ENA);
    } else {
        WRITE_PERI_REG(UART_CONF1(uart_no),
            ((UartDev.rcv_buff.TrigLvl & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S));
    }

    // clear all interrupt
    WRITE_PERI_REG(UART_INT_CLR(uart_no), 0xffff);
    // enable rx_interrupt
    SET_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_FULL_INT_ENA);
}

/******************************************************************************
 * FunctionName : uart1_tx_one_char
 * Description  : Internal used function
 *                Use uart1 interface to transfer one char
 * Parameters   : uint8 TxChar - character to tx
 * Returns      : OK
*******************************************************************************/
void uart_tx_one_char(uint8 uart, uint8 TxChar) {
    while (true) {
        uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);
        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
            break;
        }
    }
    WRITE_PERI_REG(UART_FIFO(uart), TxChar);
}

void uart_flush(uint8 uart) {
    while (true) {
        uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);
        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) == 0) {
            break;
        }
    }
}

/******************************************************************************
 * FunctionName : uart1_write_char
 * Description  : Internal used function
 *                Do some special deal while tx char is '\r' or '\n'
 * Parameters   : char c - character to tx
 * Returns      : NONE
*******************************************************************************/
static void ICACHE_FLASH_ATTR
uart_os_write_char(char c) {
    if (uart_os == -1) {
        return;
    }
    if (c == '\n') {
        uart_tx_one_char(uart_os, '\r');
        uart_tx_one_char(uart_os, '\n');
    } else if (c == '\r') {
    } else {
        uart_tx_one_char(uart_os, c);
    }
}

void ICACHE_FLASH_ATTR
uart_os_config(int uart) {
    uart_os = uart;
}

/******************************************************************************
 * FunctionName : uart0_rx_intr_handler
 * Description  : Internal used function
 *                UART0 interrupt handler, add self handle code inside
 * Parameters   : void *para - point to ETS_UART_INTR_ATTACH's arg
 * Returns      : NONE
*******************************************************************************/

static void uart0_rx_intr_handler(void *para) {
    /* uart0 and uart1 intr combine togther, when interrupt occur, see reg 0x3ff20020, bit2, bit0 represents
      * uart1 and uart0 respectively
      */

    uint8 uart_no = UART_REPL;

    if (UART_FRM_ERR_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_FRM_ERR_INT_ST)) {
        // frame error
        WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_FRM_ERR_INT_CLR);
    }

    if (UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_FULL_INT_ST)) {
        // fifo full
        goto read_chars;
    } else if (UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_TOUT_INT_ST)) {
    read_chars:
        ETS_UART_INTR_DISABLE();

        while (READ_PERI_REG(UART_STATUS(uart_no)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
            uint8 RcvChar = READ_PERI_REG(UART_FIFO(uart_no)) & 0xff;
            // For efficiency, when connected to dupterm we put incoming chars
            // directly on stdin_ringbuf, rather than going via uart_ringbuf
            if (uart_attached_to_dupterm) {
                if (RcvChar == mp_interrupt_char) {
                    mp_keyboard_interrupt();
                } else {
                    ringbuf_put(&stdin_ringbuf, RcvChar);
                }
            } else {
                ringbuf_put(&uart_ringbuf, RcvChar);
            }
        }

        // Clear pending FIFO interrupts
        WRITE_PERI_REG(UART_INT_CLR(UART_REPL), UART_RXFIFO_TOUT_INT_CLR | UART_RXFIFO_FULL_INT_ST);
        ETS_UART_INTR_ENABLE();

        if (uart_attached_to_dupterm) {
            mp_hal_signal_input();
        }
    }
}

// Waits at most timeout microseconds for at least 1 char to become ready for reading.
// Returns true if something available, false if not.
bool ICACHE_FLASH_ATTR uart_rx_wait(uint32_t timeout_us) {
    uint32_t start = system_get_time();
    for (;;) {
        if (uart_ringbuf.iget != uart_ringbuf.iput) {
            return true; // have at least 1 char ready for reading
        }
        if (system_get_time() - start >= timeout_us) {
            return false; // timeout
        }
        ets_event_poll();
    }
}

int uart_rx_any(uint8 uart) {
    if (uart_ringbuf.iget != uart_ringbuf.iput) {
        return true; // have at least 1 char ready for reading
    }
    return false;
}

int uart_tx_any_room(uint8 uart) {
    uint32_t fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);
    if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) >= 126) {
        return false;
    }
    return true;
}

// Returns char from the input buffer, else -1 if buffer is empty.
int uart_rx_char(void) {
    return ringbuf_get(&uart_ringbuf);
}

int uart_rx_one_char(uint8 uart_no) {
    if (READ_PERI_REG(UART_STATUS(uart_no)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
        return READ_PERI_REG(UART_FIFO(uart_no)) & 0xff;
    }
    return -1;
}

/******************************************************************************
 * FunctionName : uart_init
 * Description  : user interface for init uart
 * Parameters   : UartBautRate uart0_br - uart0 bautrate
 *                UartBautRate uart1_br - uart1 bautrate
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR uart_init(UartBautRate uart0_br, UartBautRate uart1_br) {
    // rom use 74880 baut_rate, here reinitialize
    UartDev.baut_rate = uart0_br;
    uart_config(UART0);
    UartDev.baut_rate = uart1_br;
    uart_config(UART1);
    ETS_UART_INTR_ENABLE();

    // install handler for "os" messages
    os_install_putc1((void *)uart_os_write_char);
}

void ICACHE_FLASH_ATTR uart_reattach() {
    uart_init(UART_BIT_RATE_74880, UART_BIT_RATE_74880);
}

void ICACHE_FLASH_ATTR uart_setup(uint8 uart) {
    ETS_UART_INTR_DISABLE();
    uart_config(uart);
    ETS_UART_INTR_ENABLE();
}

int ICACHE_FLASH_ATTR uart0_get_rxbuf_len(void) {
    return uart_ringbuf.size;
}

void ICACHE_FLASH_ATTR uart0_set_rxbuf(uint8 *buf, int len) {
    ETS_UART_INTR_DISABLE();
    uart_ringbuf.buf = buf;
    uart_ringbuf.size = len;
    uart_ringbuf.iget = 0;
    uart_ringbuf.iput = 0;
    ETS_UART_INTR_ENABLE();
}

// Task-based UART interface

#include "py/obj.h"
#include "lib/utils/pyexec.h"

#if MICROPY_REPL_EVENT_DRIVEN
void ICACHE_FLASH_ATTR uart_task_handler(os_event_t *evt) {
    if (pyexec_repl_active) {
        // TODO: Just returning here isn't exactly right.
        // What really should be done is something like
        // enquing delayed event to itself, for another
        // chance to feed data to REPL. Otherwise, there
        // can be situation when buffer has bunch of data,
        // and sits unprocessed, because we consumed all
        // processing signals like this.
        return;
    }

    int c, ret = 0;
    while ((c = ringbuf_get(&stdin_ringbuf)) >= 0) {
        if (c == mp_interrupt_char) {
            mp_keyboard_interrupt();
        }
        ret = pyexec_event_repl_process_char(c);
        if (ret & PYEXEC_FORCED_EXIT) {
            break;
        }
    }

    if (ret & PYEXEC_FORCED_EXIT) {
        soft_reset();
    }
}

void uart_task_init() {
    system_os_task(uart_task_handler, UART_TASK_ID, uart_evt_queue, sizeof(uart_evt_queue) / sizeof(*uart_evt_queue));
}
#endif
