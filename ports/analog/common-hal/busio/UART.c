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

#if CIRCUITPY_BUSIO_UART

#include "mpconfigport.h"
#include "supervisor/shared/tick.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/UART.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared/readline/readline.h"
#include "shared/runtime/interrupt_char.h"

#include "py/gc.h"
#include "py/ringbuf.h"
#include "py/mperrno.h"
#include "py/mpprint.h"
#include "py/runtime.h"

#include "max32_port.h"
#include "UART.h"
#include "nvic_table.h"

// UART IRQ Priority
#define UART_PRIORITY 1

typedef enum {
    UART_9600 = 9600,
    UART_14400 = 14400,
    UART_19200 = 19200,
    UART_38400 = 38400,
    UART_57600 = 57600,
    UART_115200 = 115200,
    UART_230400 = 230400,
    UART_460800 = 460800,
    UART_921600 = 921600,
} uart_valid_baudrates;

typedef enum {
    UART_FREE = 0,
    UART_BUSY,
} uart_status_t;

static uint32_t timeout_ms = 0;

// Set each bit to indicate an active UART
// will be checked by ISR Handler for which ones to call
static uint8_t uarts_active = 0;
static uart_status_t uart_status[NUM_UARTS];
static volatile int uart_err;
static busio_uart_obj_t *context;

static bool isValidBaudrate(uint32_t baudrate) {
    switch (baudrate) {
        case UART_9600:
            return true;
            break;
        case UART_14400:
            return true;
            break;
        case UART_19200:
            return true;
            break;
        case UART_38400:
            return true;
            break;
        case UART_57600:
            return true;
            break;
        case UART_115200:
            return true;
            break;
        case UART_230400:
            return true;
            break;
        case UART_460800:
            return true;
            break;
        case UART_921600:
            return true;
            break;
        default:
            return false;
            break;
    }
}

static mxc_uart_parity_t convertParity(busio_uart_parity_t busio_parity) {
    switch (busio_parity) {
        case BUSIO_UART_PARITY_NONE:
            return MXC_UART_PARITY_DISABLE;
        case BUSIO_UART_PARITY_EVEN:
            return MXC_UART_PARITY_EVEN_0;
        case BUSIO_UART_PARITY_ODD:
            return MXC_UART_PARITY_ODD_0;
        default:
            // not reachable due to validation in shared-bindings/busio/SPI.c
            return MXC_UART_PARITY_DISABLE;
    }
}

void uart_isr(void) {
    for (int i = 0; i < NUM_UARTS; i++) {
        if (uarts_active & (1 << i)) {
            MXC_UART_AsyncHandler(MXC_UART_GET_UART(i));
        }
    }
}

// Callback gets called when AsyncRequest is COMPLETE
// (e.g. txLen == txCnt)
static volatile void uartCallback(mxc_uart_req_t *req, int error) {
    uart_status[MXC_UART_GET_IDX(req->uart)] = UART_FREE;
    uart_err = error;

    MXC_SYS_Crit_Enter();
    ringbuf_put_n(&context->ringbuf, req->rxData, req->rxLen);
    MXC_SYS_Crit_Exit();
}

// Construct an underlying UART object.
void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {
    int err, temp;

    // Check for NULL Pointers && valid UART settings
    assert(self);

    // Assign UART ID based on pins
    temp = pinsToUart(tx, rx);
    if (temp == -1) {
        // Error will be indicated by pinsToUart(tx, rx) function
        return;
    } else {
        self->uart_id = temp;
        self->uart_regs = MXC_UART_GET_UART(temp);
    }
    assert((self->uart_id >= 0) && (self->uart_id < NUM_UARTS));

    // Check for size of ringbuffer, desire powers of 2
    // At least use 1 byte if no size is given
    assert((receiver_buffer_size & (receiver_buffer_size - 1)) == 0);
    if (receiver_buffer_size == 0) {
        receiver_buffer_size += 1;
    }

    // Indicate RS485 not implemented
    if ((rs485_dir != NULL) || (rs485_invert)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("RS485"));
    }

    if ((rx != NULL) && (tx != NULL)) {
        err = MXC_UART_Init(self->uart_regs, baudrate, MXC_UART_IBRO_CLK);
        if (err != E_NO_ERROR) {
            mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q init failed"), MP_QSTR_UART);
        }

        // attach & configure pins
        self->tx_pin = tx;
        self->rx_pin = rx;
        common_hal_mcu_pin_claim(self->tx_pin);
        common_hal_mcu_pin_claim(self->rx_pin);
    } else {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("UART needs TX & RX"));
    }

    if ((cts) && (rts)) {
        MXC_UART_SetFlowCtrl(self->uart_regs, MXC_UART_FLOW_EN, 8);
        self->cts_pin = cts;
        self->rts_pin = rts;
        common_hal_mcu_pin_claim(self->cts_pin);
        common_hal_mcu_pin_claim(self->rts_pin);
    } else if (cts || rts) {
        mp_raise_ValueError(MP_ERROR_TEXT("Both RX and TX required for flow control"));
    }

    // Set stop bits & data size
    assert((stop == 1) || (stop == 2));
    mp_arg_validate_int(bits, 8, MP_QSTR_bits);
    MXC_UART_SetDataSize(self->uart_regs, bits);
    MXC_UART_SetStopBits(self->uart_regs, stop);

    // Set parity
    MXC_UART_SetParity(self->uart_regs, convertParity(parity));

    // attach UART parameters
    self->stop_bits = stop; // must be 1 or 2
    self->bits = bits;
    self->parity = parity;
    self->baudrate = baudrate;
    self->error = E_NO_ERROR;

    // Indicate to this module that the UART is active
    uarts_active |= (1 << self->uart_id);

    // Set the timeout to a default value
    if (((timeout < 0.0) || (timeout > 100.0))) {
        self->timeout = 1.0;
    } else {
        self->timeout = timeout;
    }

    // Initialize ringbuffer
    if (receiver_buffer == NULL) {
        if (!ringbuf_alloc(&self->ringbuf, receiver_buffer_size)) {
            m_malloc_fail(receiver_buffer_size);
            mp_raise_RuntimeError_varg(MP_ERROR_TEXT("Failed to allocate %q buffer"),
                MP_QSTR_UART);
        }
    } else {
        if (!(ringbuf_init(&self->ringbuf, receiver_buffer, receiver_buffer_size))) {
            mp_raise_RuntimeError_varg(MP_ERROR_TEXT("Failed to allocate %q buffer"),
                MP_QSTR_UART);
        }
    }

    context = self;

    // Setup UART interrupt
    NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(self->uart_id));
    NVIC_DisableIRQ(MXC_UART_GET_IRQ(self->uart_id));
    NVIC_SetPriority(MXC_UART_GET_IRQ(self->uart_id), UART_PRIORITY);
    NVIC_SetVector(MXC_UART_GET_IRQ(self->uart_id), (uint32_t)uart_isr);

    NVIC_EnableIRQ(MXC_UART_GET_IRQ(self->uart_id));

    return;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    assert(self);

    if (!common_hal_busio_uart_deinited(self)) {
        // First disable the ISR to avoid pre-emption
        NVIC_DisableIRQ(MXC_UART_GET_IRQ(self->uart_id));

        // Shutdown the UART Controller
        MXC_UART_Shutdown(self->uart_regs);
        self->error = E_UNINITIALIZED;

        assert(self->rx_pin && self->tx_pin);
        reset_pin_number(self->rx_pin->port, self->rx_pin->mask);
        reset_pin_number(self->tx_pin->port, self->tx_pin->mask);

        if (self->cts_pin && self->rts_pin) {
            reset_pin_number(self->cts_pin->port, self->cts_pin->mask);
            reset_pin_number(self->rts_pin->port, self->rts_pin->mask);
        }

        self->tx_pin = NULL;
        self->rx_pin = NULL;
        self->cts_pin = NULL;
        self->rts_pin = NULL;

        ringbuf_deinit(&self->ringbuf);

        // Indicate to this module that the UART is not active
        uarts_active &= ~(1 << self->uart_id);
    }
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    if (uarts_active & (1 << self->uart_id)) {
        return false;
    } else {
        return true;
    };
}

// Read characters. len is in characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self,
    uint8_t *data, size_t len, int *errcode) {
    int err;
    uint32_t start_time = 0;
    static size_t bytes_remaining;

    // Setup globals & status tracking
    uart_err = E_NO_ERROR;
    uarts_active |= (1 << self->uart_id);
    uart_status[self->uart_id] = UART_BUSY;
    bytes_remaining = len;

    mxc_uart_req_t uart_rd_req;
    uart_rd_req.rxCnt = 0;
    uart_rd_req.txCnt = 0;
    uart_rd_req.rxData = data;
    uart_rd_req.txData = NULL;
    uart_rd_req.rxLen = bytes_remaining;
    uart_rd_req.txLen = 0;
    uart_rd_req.uart = self->uart_regs;
    uart_rd_req.callback = (void *)uartCallback;

    // Initiate the read transaction
    start_time = supervisor_ticks_ms64();
    err = MXC_UART_TransactionAsync(&uart_rd_req);
    if (err != E_NO_ERROR) {
        *errcode = err;
        MXC_UART_AbortAsync(self->uart_regs);
        NVIC_DisableIRQ(MXC_UART_GET_IRQ(self->uart_id));
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("UART read error"));
    }

    // Wait for transaction completion or timeout
    while ((uart_status[self->uart_id] != UART_FREE) &&
           (supervisor_ticks_ms64() - start_time < (self->timeout * 1000))) {
    }

    // If the timeout gets hit, abort and error out
    if (uart_status[self->uart_id] != UART_FREE) {
        MXC_UART_AbortAsync(self->uart_regs);
        NVIC_DisableIRQ(MXC_UART_GET_IRQ(self->uart_id));
        mp_raise_RuntimeError(MP_ERROR_TEXT("UART transaction timeout"));
    }
    // Check for errors from the callback
    else if (uart_err != E_NO_ERROR) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("UART read error"));
        MXC_UART_AbortAsync(self->uart_regs);
    }

    // Copy the data from the ringbuf (or return error)
    MXC_SYS_Crit_Enter();
    err = ringbuf_get_n(&context->ringbuf, data, len);
    MXC_SYS_Crit_Exit();

    return err;
}

// Write characters. len is in characters NOT bytes!
// This function blocks until the timeout finishes
size_t common_hal_busio_uart_write(busio_uart_obj_t *self,
    const uint8_t *data, size_t len, int *errcode) {
    int err;
    static size_t bytes_remaining;

    // Setup globals & status tracking
    uart_err = E_NO_ERROR;
    uarts_active |= (1 << self->uart_id);
    uart_status[self->uart_id] = UART_BUSY;
    bytes_remaining = len;

    mxc_uart_req_t uart_wr_req = {};

    // Setup transaction
    uart_wr_req.rxCnt = 0;
    uart_wr_req.txCnt = 0;
    uart_wr_req.rxData = NULL;
    uart_wr_req.txData = data;
    uart_wr_req.txLen = bytes_remaining;
    uart_wr_req.rxLen = 0;
    uart_wr_req.uart = self->uart_regs;
    uart_wr_req.callback = (void *)uartCallback;

    // Start the transaction
    err = MXC_UART_TransactionAsync(&uart_wr_req);
    if (err != E_NO_ERROR) {
        *errcode = err;
        MXC_UART_AbortAsync(self->uart_regs);
        NVIC_DisableIRQ(MXC_UART_GET_IRQ(self->uart_id));
        mp_raise_ValueError(MP_ERROR_TEXT("All UART peripherals are in use"));
    }

    // Wait for transaction completion
    while (uart_status[self->uart_id] != UART_FREE) {
        // Call the handler and abort if errors
        uart_err = MXC_UART_AsyncHandler(self->uart_regs);
        if (uart_err != E_NO_ERROR) {
            MXC_UART_AbortAsync(self->uart_regs);
        }
    }
    // Check for errors from the callback
    if (uart_err != E_NO_ERROR) {
        MXC_UART_AbortAsync(self->uart_regs);
    }

    return len;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return self->baudrate;
}

// Validate baudrate
void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    if (isValidBaudrate(baudrate)) {
        self->baudrate = baudrate;
    } else {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_baudrate);
    }
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return self->timeout;
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    if (timeout > 100.0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Timeout must be < 100 seconds"));
    }

    timeout_ms = 1000 * (uint32_t)timeout;
    self->timeout = (uint32_t)timeout;

    return;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return ringbuf_num_filled(&self->ringbuf);
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    MXC_UART_ClearRXFIFO(self->uart_regs);
    ringbuf_clear(&self->ringbuf);
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    return !(MXC_UART_GetStatus(self->uart_regs) & (MXC_F_UART_STATUS_TX_BUSY));
}

#endif // CIRCUITPY_BUSIO_UART
