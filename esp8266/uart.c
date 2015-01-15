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

#define RX_BUF_SIZE (256)

// UartDev is defined and initialized in rom code.
extern UartDevice UartDev;

// circular buffer for RX buffering
static uint16_t rx_buf_in;
static uint16_t rx_buf_out;
static uint8_t rx_buf[RX_BUF_SIZE];

static os_event_t uart_evt_queue[16];

static void uart0_rx_intr_handler(void *para);

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
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_U0RTS);
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

    // init RX buffer
    rx_buf_in = 0;
    rx_buf_out = 0;
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
        uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & (UART_TXFIFO_CNT<<UART_TXFIFO_CNT_S);
        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
            break;
        }
    }
    WRITE_PERI_REG(UART_FIFO(uart), TxChar);
}

/******************************************************************************
 * FunctionName : uart1_write_char
 * Description  : Internal used function
 *                Do some special deal while tx char is '\r' or '\n'
 * Parameters   : char c - character to tx
 * Returns      : NONE
*******************************************************************************/
static void ICACHE_FLASH_ATTR
uart1_write_char(char c) {
    if (c == '\n') {
        uart_tx_one_char(UART1, '\r');
        uart_tx_one_char(UART1, '\n');
    } else if (c == '\r') {
    } else {
        uart_tx_one_char(UART1, c);
    }
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

    uint8 RcvChar;
    uint8 uart_no = UART0;

    if (UART_FRM_ERR_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_FRM_ERR_INT_ST)) {
        // frame error
        WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_FRM_ERR_INT_CLR);
    }

    if (UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_FULL_INT_ST)) {
        // fifo full
        WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_FULL_INT_CLR);
        goto read_chars;
    } else if (UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_TOUT_INT_ST)) {
        WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_TOUT_INT_CLR);
        read_chars:
        while (READ_PERI_REG(UART_STATUS(uart_no)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
            RcvChar = READ_PERI_REG(UART_FIFO(uart_no)) & 0xff;
#if 1 //MICROPY_REPL_EVENT_DRIVEN is not available here
            system_os_post(UART_TASK_ID, 0, RcvChar);
#else
            uint16_t rx_buf_in_next = (rx_buf_in + 1) % RX_BUF_SIZE;
            if (rx_buf_in_next != rx_buf_out) {
                rx_buf[rx_buf_in] = RcvChar;
                rx_buf_in = rx_buf_in_next;
            }
#endif
        }
    }
}

int uart0_rx(void) {
  if (rx_buf_out != rx_buf_in) {
      int chr = rx_buf[rx_buf_out];
      rx_buf_out = (rx_buf_out + 1) % RX_BUF_SIZE;
      return chr;
  } else {
      return -1;
  }
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

    // install uart1 putc callback
    os_install_putc1((void *)uart1_write_char);
}

void ICACHE_FLASH_ATTR uart_reattach() {
    uart_init(UART_BIT_RATE_74880, UART_BIT_RATE_74880);
}

// Task-based UART interface

int pyexec_friendly_repl_process_char(int c);

void uart_task_handler(os_event_t *evt) {
    pyexec_friendly_repl_process_char(evt->par);
}

void uart_task_init() {
    system_os_task(uart_task_handler, UART_TASK_ID, uart_evt_queue, sizeof(uart_evt_queue) / sizeof(*uart_evt_queue));
}
