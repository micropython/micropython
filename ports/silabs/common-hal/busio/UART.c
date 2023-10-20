/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/UART.h"

#include "mpconfigport.h"
#include "shared/readline/readline.h"
#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"

#define UARTDRV_USART_BUFFER_SIZE 6

// Define RX and TX buffer queues
DEFINE_BUF_QUEUE(UARTDRV_USART_BUFFER_SIZE, uartdrv_usart_rx_buffer);
DEFINE_BUF_QUEUE(UARTDRV_USART_BUFFER_SIZE, uartdrv_usart_tx_buffer);

STATIC UARTDRV_HandleData_t uartdrv_usart_handle;
STATIC UARTDRV_InitUart_t uartdrv_usart_init;
STATIC bool in_used = false;
STATIC bool never_reset = false;
busio_uart_obj_t *context;
volatile Ecode_t errflag; // Used to restart read halts

// Reset uart peripheral
void uart_reset(void) {
    if ((!never_reset) && in_used) {
        if (UARTDRV_DeInit(&uartdrv_usart_handle) != ECODE_EMDRV_UARTDRV_OK) {
            mp_raise_ValueError(translate("UART Deinit fail"));
        }
        in_used = false;
    }
}

// Construct uart obj
void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx,
    const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts,
    const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir,
    bool rs485_invert,
    uint32_t baudrate,
    uint8_t bits,
    busio_uart_parity_t parity,
    uint8_t stop,
    mp_float_t timeout,
    uint16_t receiver_buffer_size,
    byte *receiver_buffer,
    bool sigint_enabled) {

    if ((rts != NULL) || (cts != NULL) || (rs485_dir != NULL)
        || (rs485_invert == true)) {
        mp_raise_NotImplementedError(translate("RS485"));
    }

    if ((tx != NULL) && (rx != NULL)) {
        if (tx->function_list[FN_USART0_TX] == 1
            && tx->function_list[FN_USART0_RX] == 1) {

            self->handle = &uartdrv_usart_handle;
            self->baudrate = baudrate;
            self->tx = tx;
            self->rx = rx;
            self->sigint_enabled = sigint_enabled;
            self->timeout_ms = timeout * 1000;
            uartdrv_usart_init.port = USART0;
            uartdrv_usart_init.baudRate = baudrate;
            uartdrv_usart_init.txPort = tx->port;
            uartdrv_usart_init.rxPort = rx->port;
            uartdrv_usart_init.txPin = tx->number;
            uartdrv_usart_init.rxPin = rx->number;
            uartdrv_usart_init.uartNum = 0;
            uartdrv_usart_init.stopBits = (stop >= 2) ? usartStopbits2
                                                       :usartStopbits1;
            uartdrv_usart_init.parity = (USART_Parity_TypeDef)parity;
            uartdrv_usart_init.oversampling = usartOVS4;
            uartdrv_usart_init.mvdis = false;
            uartdrv_usart_init.fcType = uartdrvFlowControlNone;

            uartdrv_usart_init.rxQueue = (UARTDRV_Buffer_FifoQueue_t *)
                &uartdrv_usart_rx_buffer;
            uartdrv_usart_init.txQueue = (UARTDRV_Buffer_FifoQueue_t *)
                &uartdrv_usart_tx_buffer;

            if (UARTDRV_InitUart(self->handle, &uartdrv_usart_init)
                != ECODE_EMDRV_UARTDRV_OK) {
                mp_raise_RuntimeError(translate("UART init"));
            }
            common_hal_mcu_pin_claim(tx);
            common_hal_mcu_pin_claim(rx);
            in_used = true;

            // Use the provided buffer when given.
            if (receiver_buffer != NULL) {
                ringbuf_init(&self->ringbuf, receiver_buffer, receiver_buffer_size);
            } else {
                if (!ringbuf_alloc(&self->ringbuf, receiver_buffer_size)) {
                    m_malloc_fail(receiver_buffer_size);
                }
            }
            errflag = ECODE_EMDRV_UARTDRV_OK;
            context = self;

        } else {
            mp_raise_ValueError(translate("Hardware in use, try alternative pins"));
        }

    } else {
        raise_ValueError_invalid_pins();
    }
}

// Never reset UART obj when reload
void common_hal_busio_uart_never_reset(busio_uart_obj_t *self) {
    never_reset = true;
    common_hal_never_reset_pin(self->tx);
    common_hal_never_reset_pin(self->rx);
    return;
}

// Check Uart status, deinited or not
bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->handle == NULL;
}

// Deinit uart obj
void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }

    if (UARTDRV_DeInit(self->handle) != ECODE_EMDRV_UARTDRV_OK) {
        mp_raise_RuntimeError(translate("UART de-init"));
    }

    common_hal_reset_pin(self->rx);
    common_hal_reset_pin(self->tx);
    self->tx = NULL;
    self->rx = NULL;
    self->handle = NULL;
    in_used = false;
}

// Callback function for UARTDRV_Receive
void UARTDRV_Receive_Callback(UARTDRV_Handle_t *handle,
    Ecode_t transferStatus,
    uint8_t *data,
    UARTDRV_Count_t transferCount) {
    (void)handle;
    (void)transferStatus;
    (void)data;
    (void)transferCount;

    taskENTER_CRITICAL();
    ringbuf_put_n(&context->ringbuf, &context->rx_char, 1);
    taskEXIT_CRITICAL();
    errflag = UARTDRV_Receive(context->handle, &context->rx_char, 1,
        (UARTDRV_Callback_t)UARTDRV_Receive_Callback);
    if (context->sigint_enabled) {
        if (context->rx_char == CHAR_CTRL_C) {
            common_hal_busio_uart_clear_rx_buffer(context);
            mp_sched_keyboard_interrupt();
        }
    }
}

// Read bytes. If nbytes is specified then read at most that many bytes.
// Otherwise, read everything that arrives until the connection times out.
// Providing the number of bytes expected is highly recommended because it will be faster.
// If no bytes are read, return None.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data,
    size_t len, int *errcode) {

    uint64_t start_ticks = supervisor_ticks_ms64();
    if (len == 0) {
        // Nothing to read.
        return 0;
    }
    errflag = ECODE_EMDRV_UARTDRV_WAITING;
    // Wait for all bytes received or timeout, same as nrf
    while ((ringbuf_num_filled(&self->ringbuf) < len) &&
           (supervisor_ticks_ms64() - start_ticks < self->timeout_ms)) {

        RUN_BACKGROUND_TASKS;
        // restart if it failed in the callback
        if (errflag != ECODE_EMDRV_UARTDRV_OK) {
            errflag = UARTDRV_Receive(self->handle, &self->rx_char, 1,
                (UARTDRV_Callback_t)UARTDRV_Receive_Callback);
        }
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            return 0;
        }
    }

    taskENTER_CRITICAL();
    // Copy as much received data as available, up to len bytes.
    size_t rx_bytes = ringbuf_get_n(&self->ringbuf, data, len);
    taskEXIT_CRITICAL();

    if (rx_bytes == 0) {
        *errcode = EAGAIN;
        return MP_STREAM_ERROR;
    }
    return rx_bytes;
}

// Write the buffer of bytes to the bus.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self,
    const uint8_t *data,
    size_t len,
    int *errcode) {

    Ecode_t ret = UARTDRV_TransmitB(self->handle, (uint8_t *)data, len);
    if (ret != ECODE_EMDRV_UARTDRV_OK) {
        mp_raise_RuntimeError(translate("UART write"));
    }
    return len;
}

// Get uart baudrate value
uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return self->baudrate;
}

// Set uart baudrate value
void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self,
    uint32_t baudrate) {

    // Don't reset if it's the same value
    if (baudrate == self->baudrate) {
        return;
    }
    uartdrv_usart_init.baudRate = baudrate;
    if (UARTDRV_InitUart(self->handle, &uartdrv_usart_init)
        != ECODE_EMDRV_UARTDRV_OK) {
        mp_raise_RuntimeError(translate("UART re-init"));
    }
}

// Get timeout for receive
mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t)(self->timeout_ms / 1000.0f);
}

// Set timeout for receive
void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self,
    mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

// Query characters available to read
uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return ringbuf_num_filled(&self->ringbuf);
}

// Clear rx buffer
void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    taskENTER_CRITICAL();
    ringbuf_clear(&self->ringbuf);
    taskEXIT_CRITICAL();
    self->handle->rxQueue->head = 0;
    self->handle->rxQueue->tail = 0;
    self->handle->rxQueue->used = 0;
}

// Check uart bus ready to transmit or not
bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    return UARTDRV_GetTransmitDepth(self->handle) == 0;
}
