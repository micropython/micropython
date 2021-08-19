/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Armink (armink.ztl@gmail.com)
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
#include <stdint.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include "shared/runtime/interrupt_char.h"
#include "mpgetcharport.h"

#define UART_FIFO_SIZE 256

static struct rt_ringbuffer *rx_fifo = NULL;
static rt_err_t (*odev_rx_ind)(rt_device_t dev, rt_size_t size) = NULL;

static rt_err_t getchar_rx_ind(rt_device_t dev, rt_size_t size)
{
    uint8_t ch;
    rt_size_t i;
    rt_base_t int_lvl;

    for (i = 0; i < size; i++)
    {
        /* read a char */
        if (rt_device_read(dev, 0, &ch, 1))
        {
            if (ch == mp_interrupt_char)
            {
                mp_sched_keyboard_interrupt();
            }
            else
            {
                int_lvl = rt_hw_interrupt_disable();
                rt_ringbuffer_put_force(rx_fifo, &ch, 1);
                rt_hw_interrupt_enable(int_lvl);
            }
        }
    }
    return RT_EOK;
}

void mp_getchar_init(void)
{
    rt_base_t int_lvl;
    rt_device_t console;

    /* create RX FIFO */
    rx_fifo = rt_ringbuffer_create(UART_FIFO_SIZE);
    /* created must success */
    RT_ASSERT(rx_fifo);

    int_lvl = rt_hw_interrupt_disable();
    console = rt_console_get_device();
    if (console)
    {
        /* backup RX indicate */
        odev_rx_ind = console->rx_indicate;
        rt_device_set_rx_indicate(console, getchar_rx_ind);
    }
    rt_hw_interrupt_enable(int_lvl);

}

void mp_getchar_deinit(void)
{
    rt_base_t int_lvl;
    rt_device_t console;

    rt_ringbuffer_destroy(rx_fifo);

    int_lvl = rt_hw_interrupt_disable();
    console = rt_console_get_device();
    if (console && odev_rx_ind)
    {
        /* restore RX indicate */
        rt_device_set_rx_indicate(console, odev_rx_ind);
    }
    rt_hw_interrupt_enable(int_lvl);
}

int mp_getchar(void)
{
    uint8_t ch;
    rt_base_t int_lvl;

    int_lvl = rt_hw_interrupt_disable();
    if (!rt_ringbuffer_getchar(rx_fifo, &ch))
    {
        ch = 0xFF;
    }
    rt_hw_interrupt_enable(int_lvl);

    return ch;

}
