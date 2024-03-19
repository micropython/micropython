/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#ifndef MP_PORT_MCX_DRV_UART_H
#define MP_PORT_MCX_DRV_UART_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    DRV_Uart7DataBits,
    DRV_Uart8DataBits,
} drv_uart_data_bits_t;

typedef enum {
    DRV_Uart1StopBits,
    DRV_Uart2StopBits,
} drv_uart_stop_bits_t;

typedef enum {
    DRV_UartNoParity,
    DRV_UartEvenParity,
    DRV_UartOddParity,
} drv_uart_parity_t;

typedef enum {
    DRV_UartNoFlowControl,
    DRV_UartCtsFlowControl,
    DRV_UartRtsFlowControl,
    DRV_UartCtsRtsFlowControl,
} drv_uart_flow_control_t;

typedef enum {
    DRV_UartStatusIdle,
    DRV_UartStatusBusy,
    DRV_UartStatusAsyncRx,
} drv_uart_status_t;

typedef void (*drv_uart_async_fn_t)(void *uart, void *param, uint8_t len);

typedef struct drv_uart_type {
    uint8_t id;
    void *instance;
    drv_uart_status_t status;
    drv_uart_async_fn_t async_fn;
    void *async_fn_param;
} drv_uart_t;

typedef struct drv_uart_config_type {
    uint32_t baud_rate;
    bool i2c_shared;
    drv_uart_data_bits_t data_bits;
    drv_uart_stop_bits_t stop_bits;
    drv_uart_parity_t parity;
    drv_uart_flow_control_t flow_control;
} drv_uart_config_t;

int drv_uart_init(drv_uart_t *uart, uint8_t id, drv_uart_config_t *cfg);
int drv_uart_deinit(drv_uart_t *uart);
int drv_uart_write(drv_uart_t *uart, const uint8_t *data, int len, mp_uint_t timeout);
int drv_uart_send_break(drv_uart_t *uart);
int drv_uart_read(drv_uart_t *uart, uint8_t *data, int len, mp_uint_t timeout);
int drv_uart_async_start(drv_uart_t *uart, drv_uart_async_fn_t fn, void *param);
int drv_uart_async_read(drv_uart_t *uart, uint8_t *data, uint8_t len);
int drv_uart_async_cancel(drv_uart_t *uart);

#endif
