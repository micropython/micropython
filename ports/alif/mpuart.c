/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include <string.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "mpuart.h"

#if MICROPY_HW_ENABLE_UART_REPL

#include "pinconf.h"
#include "sys_ctrl_uart.h"
#include "uart.h"

#define TX_PIN pin_P12_2
#define RX_PIN pin_P12_1
#define UART_ID 4
#define UART_IRQN UART4_IRQ_IRQn
#define UART_PTR ((UART_Type *)UART4_BASE)
#define BAUDRATE 115200
#define SYST_PCLK 100000000

static UART_TRANSFER transfer;

void mp_uart_init(void) {
    mp_hal_pin_config(TX_PIN, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT_UART, false);
    mp_hal_pin_config(RX_PIN, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT_UART, true);
    select_uart_clock_syst_pclk(UART_ID);
    enable_uart_clock(UART_ID);
    uart_software_reset(UART_PTR);
    uart_enable_fifo(UART_PTR);
    uart_disable_tx_irq(UART_PTR);
    uart_disable_rx_irq(UART_PTR);
    uart_set_baudrate(UART_PTR, SYST_PCLK, BAUDRATE);
    uart_set_data_parity_stop_bits(UART_PTR, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);
    uart_set_flow_control(UART_PTR, UART_FLOW_CONTROL_NONE);
    NVIC_ClearPendingIRQ(UART_IRQN);
    NVIC_SetPriority(UART_IRQN, IRQ_PRI_UART_REPL);
    NVIC_EnableIRQ(UART_IRQN);
    uart_set_tx_trigger(UART_PTR, UART_TX_FIFO_EMPTY);
    uart_set_rx_trigger(UART_PTR, UART_RX_ONE_CHAR_IN_FIFO);
    uart_enable_rx_irq(UART_PTR);
}

void mp_uart_write_strn(const char *str, size_t len) {
    memset(&transfer, 0, sizeof(transfer));
    transfer.tx_buf = (uint8_t *)str;
    transfer.tx_total_num = len;
    transfer.tx_curr_cnt = 0U;
    transfer.status = UART_TRANSFER_STATUS_NONE;

    uart_enable_tx_irq(UART_PTR);

    uint32_t start = mp_hal_ticks_ms();
    while (transfer.status == UART_TRANSFER_STATUS_NONE) {
        if (mp_hal_ticks_ms() - start > 10 * len) {
            break;
        }
        __WFE();
    }
    uart_disable_tx_irq(UART_PTR);
}

void UART4_IRQHandler(void) {
    if (UART_PTR->UART_RFL) {
        for (;;) {
            uint32_t rx_fifo_available_cnt = UART_PTR->UART_RFL;
            if (rx_fifo_available_cnt == 0) {
                break;
            }
            for (uint32_t i = 0; i < rx_fifo_available_cnt; ++i) {
                int c = UART_PTR->UART_RBR;
                #if MICROPY_KBD_EXCEPTION
                if (c == mp_interrupt_char) {
                    mp_sched_keyboard_interrupt();
                    continue;
                }
                #endif
                ringbuf_put(&stdin_ringbuf, c);
            }
        }
    } else {
        uart_irq_handler(UART_PTR, &transfer);
    }
    __SEV();
}

#endif
