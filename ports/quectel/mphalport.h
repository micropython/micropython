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

#if defined(PLAT_RDA)
#define QPY_REPL_UART   HELIOS_UART0
#define		HAL_TICK1S	16.384
/*Due to low power consumption of BC25 series, the baud rate needs to be set to 57600, please do not modify.*/
#define HELIOS_UART_BAUD   HELIOS_UART_BAUD_57600 
#elif defined(PLAT_Qualcomm)
#define		HAL_TICK1S	32.768
#define QPY_REPL_UART   HELIOS_UART3
#define HELIOS_UART_BAUD   HELIOS_UART_BAUD_115200
#elif defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)
#define		HAL_TICK1S	3.25
#define QPY_REPL_UART   HELIOS_UART3
#define HELIOS_UART_BAUD   HELIOS_UART_BAUD_115200
#elif defined(PLAT_ECR6600)
#define	HAL_TICK1S	0.5
#define QPY_REPL_UART   HELIOS_UART2
#define HELIOS_UART_BAUD   HELIOS_UART_BAUD_115200
#elif defined(PLAT_aic8800m40)
#define		HAL_TICK1S	32.768
#define QPY_REPL_UART   HELIOS_UART1
#define HELIOS_UART_BAUD   HELIOS_UART_BAUD_115200
#else
#define		HAL_TICK1S	32.768
#define QPY_REPL_UART   HELIOS_UART3
#define HELIOS_UART_BAUD   HELIOS_UART_BAUD_115200
#endif

//mia.zhong @20220308 input接口多线程调用导致dump问题
typedef struct Input_ListNode
{
    //int id;
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

uint64_t mp_hal_time_ns(void);
#endif
