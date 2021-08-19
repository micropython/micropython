/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Armink (armink.ztl@gmail.com)
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

#include <rtthread.h>
#include <rtdevice.h>

static rt_device_t console_dev = NULL;
static struct rt_device dummy_console = { 0 };
static rt_uint16_t console_open_flag;

void mp_putsn(const char *str, size_t len)
{
    if (console_dev)
    {
        rt_device_write(console_dev, 0, str, len);
    }
}

void mp_putsn_stream(const char *str, size_t len)
{
    if (console_dev)
    {
        rt_uint16_t old_flag = console_dev->open_flag;

        console_dev->open_flag |= RT_DEVICE_FLAG_STREAM;
        rt_device_write(console_dev, 0, str, len);
        console_dev->open_flag = old_flag;
    }
}

void mp_putsn_init(void)
{
    {
        /* register dummy console device */
#ifdef RT_USING_DEVICE_OPS
        static struct rt_device_ops _ops = {0};
        dummy_console.ops = &_ops;
#endif

        dummy_console.type = RT_Device_Class_Char;

        rt_device_register(&dummy_console, "dummy", RT_DEVICE_FLAG_RDWR);
    }

    /* backup the console device */
    console_dev = rt_console_get_device();
    console_open_flag = console_dev->open_flag;
    console_dev->open_flag = 0;

    /* set the new console device to dummy console */
    rt_console_set_device(dummy_console.parent.name);
    /* reopen the old console device for mp_putsn */
    rt_device_open(console_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
}

void mp_putsn_deinit(void)
{
    /* close the old console, it's already in mp_putsn_init */
    rt_device_close(console_dev);
    /* restore the old console device */
    rt_console_set_device(console_dev->parent.name);
    console_dev->open_flag = console_open_flag;

    rt_device_unregister(&dummy_console);
}
