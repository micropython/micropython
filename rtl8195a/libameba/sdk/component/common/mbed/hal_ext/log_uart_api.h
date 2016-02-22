/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef LOG_UART_API_H
#define LOG_UART_API_H

#include "device.h"
#include "serial_api.h"
#include "hal_log_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*loguart_irq_handler)(uint32_t id, LOG_UART_INT_ID event);

typedef struct log_uart_s log_uart_t;

int32_t log_uart_init (log_uart_t *obj, int baudrate, int data_bits, SerialParity parity, int stop_bits);
void log_uart_free(log_uart_t *obj);
void log_uart_baud(log_uart_t *obj, int baudrate);
void log_uart_format(log_uart_t *obj, int data_bits, SerialParity parity, int stop_bits);
void log_uart_irq_handler(log_uart_t *obj, loguart_irq_handler handler, uint32_t id);
void log_uart_irq_set(log_uart_t *obj, LOG_UART_INT_ID irq, uint32_t enable);
char log_uart_getc(log_uart_t *obj);
void log_uart_putc(log_uart_t *obj, char c);
int log_uart_readable(log_uart_t *obj);
int log_uart_writable(log_uart_t *obj);
void log_uart_clear(log_uart_t *obj);
void log_uart_clear_tx(log_uart_t *obj);
void log_uart_clear_rx(log_uart_t *obj);
void log_uart_break_set(log_uart_t *obj);
void log_uart_break_clear(log_uart_t *obj);
void log_uart_tx_comp_handler(log_uart_t *obj, void *handler, uint32_t id);
void log_uart_rx_comp_handler(log_uart_t *obj, void *handler, uint32_t id);
void log_uart_line_status_handler(log_uart_t *obj, void *handler, uint32_t id);
int32_t log_uart_recv (log_uart_t *obj, char *prxbuf, uint32_t len, uint32_t timeout_ms);
int32_t log_uart_send (log_uart_t *obj, char *ptxbuf, uint32_t len, uint32_t timeout_ms);
int32_t log_uart_recv_stream (log_uart_t *obj, char *prxbuf, uint32_t len);
int32_t log_uart_send_stream (log_uart_t *obj, char *ptxbuf, uint32_t len);
int32_t log_uart_recv_stream_timeout (log_uart_t *obj, char *prxbuf, uint32_t len,
                                        uint32_t timeout_ms, void *force_cs);
int32_t log_uart_send_stream_abort (log_uart_t *obj);
int32_t log_uart_recv_stream_abort (log_uart_t *obj);
void log_uart_disable (log_uart_t *obj);
void log_uart_enable (log_uart_t *obj);
uint8_t log_uart_raed_lsr(log_uart_t *obj);
uint8_t log_uart_raed_msr(log_uart_t *obj);

#ifdef __cplusplus
}
#endif

#endif  // end of "#ifndef LOG_UART_API_H"
