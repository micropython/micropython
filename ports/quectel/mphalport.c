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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "py/stream.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "py/builtin.h"
#include "py/parse.h"
#include "py/ringbuf.h"
#include "py/mpthread.h"
#include "mphalport.h"

#include "mpconfigport.h"
#include "helios_uart.h"
#include "helios_rtc.h"
#if MICROPY_PY_KBD_EXCEPTION
#include "shared/runtime/interrupt_char.h"
#include "py/runtime.h"
#endif

#define QPY_MTHREAD_SLEEP_DEAL_MSG_MAX_NUM (2 * MICROPY_SCHEDULER_DEPTH)
static Helios_MsgQ_t qpy_mthread_sleep_queue = 0;
static int mthread_sleep_flag = 0;

#define MTHREAD_SLEEP_ENTER()   (mthread_sleep_flag = 1)
#define MTHREAD_SLEEP_EXIT()    (mthread_sleep_flag = 0)
#define IS_MTHREAD_IN_SLEEP()   (1 == mthread_sleep_flag)
#define MP_HAL_PORT_CHECK_OPEN  (mp_hal_cdcPort_State == 1)

static uint8_t stdin_ringbuf_array[256];
ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0};

//mia.zhong @20220308 fix input of multi thread dump problem.
Input_ListNode_t *Head_node = NULL;
static uint8_t mp_hal_cdcPort_State = 1;


static bool mp_mthread_sleep_deal_is_inited(void);
static void mp_hal_stdin_cb(uint64_t ind_type, Helios_UARTNum port, uint64_t size);


void mp_hal_port_open(uint8_t state)
{
	mp_hal_cdcPort_State = state;
	Helios_UARTConfig UARTConfig = {
        HELIOS_UART_BAUD,
        HELIOS_UART_DATABIT_8,
        HELIOS_UART_STOP_1,
        HELIOS_UART_PARITY_NONE,
        HELIOS_UART_FC_NONE
    };
	Helios_UARTInitStruct UARTInitStruct = {&UARTConfig, mp_hal_stdin_cb};
	
	if(state == 1) {
		Helios_UART_Deinit(QPY_REPL_UART);
		if (Helios_UART_Init(QPY_REPL_UART, &UARTInitStruct)) {
			mp_hal_cdcPort_State = 0;
	    }
	}
}


//mia.zhong @20220308 fix input of multi thread dump problem.
static bool mp_mthread_sleep_deal_is_inited_child(Input_ListNode_t *node)
{
    return (0 != node->msg_q);
}

//mia.zhong @20220308 fix input of multi thread dump problem.
int mp_mthread_sleep_child(uint32_t ms)
{
    mp_uint_t msg;
    int ret = -1;
    if(Head_node) {
        if(mp_mthread_sleep_deal_is_inited_child((Input_ListNode_t *)Head_node)) {
            Head_node->mthread_sleep_flag = 1;
            ret = Helios_MsgQ_Get(Head_node->msg_q, (mp_uint_t*)&msg, sizeof(msg), ms);
            Head_node->mthread_sleep_flag = 0;
        }
    }
    return ret;
}
//mia.zhong @20220308 fix input of multi thread dump problem.
void mp_mthread_wakeup_child(void)
{
    mp_uint_t msg = 0;
    if(Head_node) {
        if(mp_mthread_sleep_deal_is_inited_child((Input_ListNode_t *)Head_node) && (Head_node->mthread_sleep_flag == 1)) {
            Helios_MsgQ_Put(Head_node->msg_q, (const void*)(&msg), sizeof(mp_uint_t), 0);
        }
    }
}

//main thread sleep
int mp_mthread_sleep(uint32_t ms)
{
    mp_uint_t msg;
    int ret = -1;
    if(mp_mthread_sleep_deal_is_inited()) {
        MTHREAD_SLEEP_ENTER();
        ret = Helios_MsgQ_Get(qpy_mthread_sleep_queue, (mp_uint_t*)&msg, sizeof(msg), ms);
        MTHREAD_SLEEP_EXIT();
    }
    return ret;
}

//wake up main sleep
void mp_mthread_wakeup(void)
{
    mp_uint_t msg = 0;
    if(mp_mthread_sleep_deal_is_inited() && (IS_MTHREAD_IN_SLEEP())) {
        Helios_MsgQ_Put(qpy_mthread_sleep_queue, (const void*)(&msg), sizeof(mp_uint_t), 0);
    }
}

void mp_main_thread_wakeup()
{
    //mia.zhong @20220308 fix input of multi thread dump problem.
    if (Head_node != NULL) {
        mp_mthread_wakeup_child();
    } else {
        mp_mthread_wakeup();//forrest.liu@20210809 add for quecpython task repl using waiting msg
    }
}

static void mp_hal_stdin_cb(uint64_t ind_type, Helios_UARTNum port, uint64_t size)
{
	if(MP_HAL_PORT_CHECK_OPEN) {
#if MICROPY_PY_KBD_EXCEPTION
	    if(IS_MAINPY_RUNNING_FLAG_TRUE() && Head_node == NULL)
	    {
	        uint64_t i = 0;
	        volatile unsigned char c = 0;
	        for(i= 0; i < size; i++)
	        {
	            if(Helios_UART_Read(QPY_REPL_UART, (void *)&c, sizeof(unsigned char)) >0 ) {
        			if (!IS_REPL_REFUSED() && (c == mp_interrupt_char)) {
                        // Signal keyboard interrupt to be raised as soon as the VM resumes
                        mp_sched_keyboard_interrupt();
                        break;
                    }
                    continue;
        		}
        		break;
            }
            if(i < size)
            {
                for(; i < size; i++)
                {
                    if(Helios_UART_Read(QPY_REPL_UART, (void *)&c, sizeof(unsigned char)) >0 ) {
                        continue;
        		    }
        		    break;
                }
            }
        }
        else
#endif
        {
            mp_main_thread_wakeup();
    	    
        }
	}
}

int mp_hal_stdio_init(void)
{
    Helios_UARTConfig UARTConfig = {
        HELIOS_UART_BAUD_115200,
        HELIOS_UART_DATABIT_8,
        HELIOS_UART_STOP_1,
        HELIOS_UART_PARITY_NONE,
        HELIOS_UART_FC_NONE
    };
    
    Helios_UARTInitStruct UARTInitStruct = {&UARTConfig, mp_hal_stdin_cb};
    int ret = (int)Helios_UART_Init(QPY_REPL_UART, &UARTInitStruct);
    if(ret)
    {
        return -1;
    }
    return 0;
}

int mp_hal_stdin_rx_chr(void)
{
    while(1)
    {
    	volatile unsigned char c = 0;
		
		if(MP_HAL_PORT_CHECK_OPEN && Helios_UART_Read(QPY_REPL_UART, (void *)&c, sizeof(unsigned char)) >0 ) {
			return c;
		}

        //forrest.liu@20210809 add for quecpython task repl using waiting msg
        MP_THREAD_GIL_EXIT();
        //mia.zhong @20220308 fix input of multi thread dump problem.
        if (Head_node != NULL)
            mp_mthread_sleep_child(HELIOS_WAIT_FOREVER);
        else
		    mp_mthread_sleep(HELIOS_WAIT_FOREVER);
        MP_THREAD_GIL_ENTER();
		
        MICROPY_EVENT_POLL_HOOK
    }
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len)
{
	if (!str|| !len) return 0;
    if(MP_HAL_PORT_CHECK_OPEN) {
    	Helios_UART_Write(QPY_REPL_UART, (void *)str, len);
    }
    return len;
}

mp_uint_t mp_hal_ticks_ms(void) 
{
	return (mp_uint_t)Helios_RTC_TicksToMs();
}

mp_uint_t mp_hal_ticks_us(void) 
{
	return (mp_uint_t)Helios_RTC_TicksToUs();
}

uint64_t mp_hal_time_ns(void)
{
    return 0;
}

void mp_hal_delay_ms(mp_uint_t ms) {
	mp_uint_t dt = 0;
	mp_uint_t t0 = 0,t1 = 0;
	Helios_Thread_t taskid = 0;
	extern Helios_Thread_t ql_micropython_task_ref;
	taskid = Helios_Thread_GetID();
	mp_uint_t qpy_mthread_sleep_deal_fun(mp_uint_t ms);

	if(ql_micropython_task_ref == taskid)
	{
		for(;;)
		{
			t0 = mp_hal_ticks_us();
			MP_THREAD_GIL_EXIT();
			mp_uint_t wait_time = qpy_mthread_sleep_deal_fun(ms);
			MP_THREAD_GIL_ENTER();
			if(wait_time >= ms)
			{
				return;
			}
			MICROPY_EVENT_POLL_HOOK;
			t1 = mp_hal_ticks_us();
			dt = t1 - t0;
			if(dt / 1000 >= ms)
			{
				return;
			}
			ms -= dt / 1000;
		}
	}
	else
	{
		MP_THREAD_GIL_EXIT();
		Helios_msleep(ms);
		MP_THREAD_GIL_ENTER();
	}
}

void mp_hal_delay_us(mp_uint_t us) {
	mp_uint_t ms = us / 1000;
    ms = ms ? ms : 1;
	mp_hal_delay_ms(ms);
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return (mp_uint_t)Helios_RTC_GetTicks();
}


uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) && stdin_ringbuf.iget != stdin_ringbuf.iput) {
        ret |= MP_STREAM_POLL_RD;
    }
    return ret;
}

mp_uint_t qpy_mthread_sleep_deal_fun(mp_uint_t ms)
{
	if(mp_mthread_sleep_deal_is_inited())
	{
		mp_uint_t dt;
		mp_uint_t t0 = mp_hal_ticks_us();
		int ret = mp_mthread_sleep(ms);
		if(ret < 0)
		{
			return ms;
		}
		else
		{
			mp_uint_t t1 = mp_hal_ticks_us();
			dt = t1 - t0;
			return dt / 1000;
		}
	}
	else
	{
		Helios_msleep(ms);
		return ms;
	}
}

//mia.zhong @20220308 fix input of multi thread dump problem.
void _add_list_node()
{
    Input_ListNode_t *node = (Input_ListNode_t *)malloc(sizeof(Input_ListNode_t));
    memset(node, 0, sizeof(Input_ListNode_t));
    
    if(0 == node->msg_q) {
        node->msg_q = Helios_MsgQ_Create(QPY_MTHREAD_SLEEP_DEAL_MSG_MAX_NUM, sizeof(mp_uint_t));
    }
    node->next_node = Head_node;
    Head_node = node;
}

//mia.zhong @20220308 fix input of multi thread dump problem.
void _delete_list_node()//delete node
{
    Input_ListNode_t *node = NULL;

    if (Head_node) {
        if(0 != Head_node->msg_q) {
            Helios_MsgQ_Delete(Head_node->msg_q);
        }
        node = Head_node;
        Head_node = (Input_ListNode_t *)Head_node->next_node;
        node->next_node = NULL;
        if (node) {
            free(node);
            node = NULL;
        }
    }
}

void mp_mthread_sleep_deal_init(void)
{
    if(0 == qpy_mthread_sleep_queue) {
        qpy_mthread_sleep_queue = \
        Helios_MsgQ_Create(QPY_MTHREAD_SLEEP_DEAL_MSG_MAX_NUM, sizeof(mp_uint_t));
    }
}

static bool mp_mthread_sleep_deal_is_inited(void)
{
    return (0 != qpy_mthread_sleep_queue);
}

