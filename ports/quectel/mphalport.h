/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#ifndef __MPHAL_PORT_H
#define __MPHAL_PORT_H

#include "helios_os.h"
#include "helios_uart.h"

#define HAL_TICK1S      32.768
#define QPY_REPL_UART   HELIOS_UART3
#define HELIOS_UART_BAUD   HELIOS_UART_BAUD_115200

// mia.zhong @20220308 input接口多线程调用导致dump问题
typedef struct Input_ListNode
{
    // int id;
    int mthread_sleep_flag;
    Helios_MsgQ_t msg_q;
    void *next_node;
} Input_ListNode_t;

void _add_list_node();
void _delete_list_node();
mp_uint_t mp_hal_ticks_cpu(void);

void mp_mthread_sleep_deal_init(void);
int mp_mthread_sleep(uint32_t ms);
void mp_mthread_wakeup(void);
int mp_mthread_sleep_child(uint32_t ms);
void mp_mthread_wakeup_child(void);

int mp_hal_stdio_init(void);

int mp_hal_stdin_rx_chr(void);

void mp_hal_port_open(uint8_t state);

void mp_hal_set_interrupt_char(int c);

static void mp_hal_random_init(void);
void mp_hal_get_random(size_t n, uint8_t *buf);

#endif
