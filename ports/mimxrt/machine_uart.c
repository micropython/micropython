/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_uart.c via MICROPY_PY_MACHINE_UART_INCLUDEFILE.

#include "py/mphal.h"
#include "ticks.h"
#include "fsl_common.h"
#include "fsl_lpuart.h"
#include "fsl_iomuxc.h"
#include CLOCK_CONFIG_H
#include "modmachine.h"
#include "pin.h"

#define DEFAULT_UART_ID  (1)
#define DEFAULT_UART_BAUDRATE (115200)
#define DEFAULT_BUFFER_SIZE (256)
#define MIN_BUFFER_SIZE  (32)
#define MAX_BUFFER_SIZE  (32766)

#define UART_HWCONTROL_RTS  (1)
#define UART_HWCONTROL_CTS  (2)
#define UART_HWCONTROL_MASK (UART_HWCONTROL_RTS | UART_HWCONTROL_CTS)

#define UART_INVERT_TX (1)
#define UART_INVERT_RX (2)
#define UART_INVERT_MASK (UART_INVERT_TX | UART_INVERT_RX)

#define UART_IRQ_RXIDLE  (1)
#define UART_IRQ_TXIDLE  (2)
#define MP_UART_ALLOWED_FLAGS (UART_IRQ_RXIDLE | UART_IRQ_TXIDLE)

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    struct _lpuart_handle handle;
    lpuart_config_t config;
    LPUART_Type *lpuart;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    uint8_t id;
    uint8_t invert;
    uint16_t tx_status;
    uint8_t *txbuf;
    uint16_t txbuf_len;
    uint8_t *rxbuf;
    uint16_t rxbuf_len;
    bool new;
    uint16_t mp_irq_trigger;   // user IRQ trigger mask
    uint16_t mp_irq_flags;     // user IRQ active IRQ flags
    mp_irq_obj_t *mp_irq_obj;  // user IRQ object
} machine_uart_obj_t;

typedef struct _iomux_table_t {
    uint32_t muxRegister;
    uint32_t muxMode;
    uint32_t inputRegister;
    uint32_t inputDaisy;
    uint32_t configRegister;
} iomux_table_t;

static const uint8_t uart_index_table[] = MICROPY_HW_UART_INDEX;
static LPUART_Type *uart_base_ptr_table[] = LPUART_BASE_PTRS;
static const iomux_table_t iomux_table_uart[] = {
    IOMUX_TABLE_UART
};
static const iomux_table_t iomux_table_uart_cts_rts[] = {
    IOMUX_TABLE_UART_CTS_RTS
};

static const char *_parity_name[] = {"None", "", "0", "1"};  // Is defined as 0, 2, 3
static const char *_invert_name[] = {"None", "INV_TX", "INV_RX", "INV_TX|INV_RX"};
static const char *_flow_name[] = {"None", "RTS", "CTS", "RTS|CTS"};

#define RX (iomux_table_uart[index + 1])
#define TX (iomux_table_uart[index])
#define RTS (iomux_table_uart_cts_rts[index + 1])
#define CTS (iomux_table_uart_cts_rts[index])

bool lpuart_set_iomux(int8_t uart) {
    int index = (uart - 1) * 2;

    if (TX.muxRegister != 0) {
        IOMUXC_SetPinMux(TX.muxRegister, TX.muxMode, TX.inputRegister, TX.inputDaisy, TX.configRegister, 0U);
        IOMUXC_SetPinConfig(TX.muxRegister, TX.muxMode, TX.inputRegister, TX.inputDaisy, TX.configRegister,
            pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_OUT, PIN_DRIVE_6, TX.configRegister));

        IOMUXC_SetPinMux(RX.muxRegister, RX.muxMode, RX.inputRegister, RX.inputDaisy, RX.configRegister, 0U);
        IOMUXC_SetPinConfig(RX.muxRegister, RX.muxMode, RX.inputRegister, RX.inputDaisy, RX.configRegister,
            pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_IN, PIN_DRIVE_6, RX.configRegister));
        return true;
    } else {
        return false;
    }
}

bool lpuart_set_iomux_rts(int8_t uart) {
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(iomux_table_uart) == MP_ARRAY_SIZE(iomux_table_uart_cts_rts));
    int index = (uart - 1) * 2;

    if (RTS.muxRegister != 0) {
        IOMUXC_SetPinMux(RTS.muxRegister, RTS.muxMode, RTS.inputRegister, RTS.inputDaisy, RTS.configRegister, 0U);
        IOMUXC_SetPinConfig(RTS.muxRegister, RTS.muxMode, RTS.inputRegister, RTS.inputDaisy, RTS.configRegister,
            pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_OUT, PIN_DRIVE_6, RTS.configRegister));
        return true;
    } else {
        return false;
    }
}

bool lpuart_set_iomux_cts(int8_t uart) {
    int index = (uart - 1) * 2;

    if (CTS.muxRegister != 0) {
        IOMUXC_SetPinMux(CTS.muxRegister, CTS.muxMode, CTS.inputRegister, CTS.inputDaisy, CTS.configRegister, 0U);
        IOMUXC_SetPinConfig(CTS.muxRegister, CTS.muxMode, CTS.inputRegister, CTS.inputDaisy, CTS.configRegister,
            pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_IN, PIN_DRIVE_6, CTS.configRegister));
        return true;
    } else {
        return false;
    }
}

void LPUART_UserCallback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData) {
    machine_uart_obj_t *self = userData;

    uint16_t mp_irq_flags = 0;
    if (status == kStatus_LPUART_TxIdle) {
        self->tx_status = kStatus_LPUART_TxIdle;
        mp_irq_flags = UART_IRQ_TXIDLE;
    } else if (status == kStatus_LPUART_IdleLineDetected) {
        mp_irq_flags = UART_IRQ_RXIDLE;
    }
    // Check the flags to see if the user handler should be called
    if (self->mp_irq_trigger & mp_irq_flags) {
        self->mp_irq_flags = mp_irq_flags;
        mp_irq_handler(self->mp_irq_obj);
    }

    if (status == kStatus_LPUART_RxRingBufferOverrun) {
        ; // Ringbuffer full, deassert RTS if flow control is enabled
    }
}

static void machine_uart_ensure_active(machine_uart_obj_t *uart) {
    if (uart->lpuart->CTRL == 0) {
        mp_raise_OSError(EIO);
    }
}

void machine_uart_set_baudrate(mp_obj_t uart_in, uint32_t baudrate) {
    machine_uart_obj_t *uart = MP_OBJ_TO_PTR(uart_in);
    #if defined(MIMXRT117x_SERIES)
    // Use the Lpuart1 clock value, which is set for All UART devices.
    LPUART_SetBaudRate(uart->lpuart, baudrate, CLOCK_GetRootClockFreq(kCLOCK_Root_Lpuart1));
    #else
    LPUART_SetBaudRate(uart->lpuart, baudrate, CLOCK_GetClockRootFreq(kCLOCK_UartClkRoot));
    #endif
}

/******************************************************************************/
// MicroPython bindings for UART

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_INV_TX), MP_ROM_INT(UART_INVERT_TX) }, \
    { MP_ROM_QSTR(MP_QSTR_INV_RX), MP_ROM_INT(UART_INVERT_RX) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HWCONTROL_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HWCONTROL_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RXIDLE), MP_ROM_INT(UART_IRQ_RXIDLE) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_TXIDLE), MP_ROM_INT(UART_IRQ_TXIDLE) }, \

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, flow=%s, "
        "rxbuf=%d, txbuf=%d, timeout=%u, timeout_char=%u, invert=%s, irq=%d)",
        self->id, self->config.baudRate_Bps, 8 - self->config.dataBitsCount,
        _parity_name[self->config.parityMode], self->config.stopBitCount + 1,
        _flow_name[(self->config.enableTxCTS << 1) | self->config.enableRxRTS],
        self->rxbuf_len, self->txbuf_len, self->timeout, self->timeout_char,
        _invert_name[self->invert], self->mp_irq_trigger);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_flow,
           ARG_timeout, ARG_timeout_char, ARG_invert, ARG_rxbuf, ARG_txbuf};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1 } },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Set baudrate if configured.
    if (args[ARG_baudrate].u_int > 0) {
        self->config.baudRate_Bps = args[ARG_baudrate].u_int;
    }

    // Set bits if configured.
    if (args[ARG_bits].u_int > 0) {
        self->config.dataBitsCount = 8 - args[ARG_bits].u_int;
    }

    // Set parity if configured.
    if (args[ARG_parity].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            self->config.parityMode = kLPUART_ParityDisabled;
        } else if (mp_obj_get_int(args[ARG_parity].u_obj) & 1) {
            self->config.parityMode = kLPUART_ParityOdd;
        } else {
            self->config.parityMode = kLPUART_ParityEven;
        }
    }

    // Set stop bits if configured.
    if (args[ARG_stop].u_int > 0) {
        self->config.stopBitCount = args[ARG_stop].u_int - 1;
    }

    // Set flow if configured.
    if (args[ARG_flow].u_int >= 0) {
        if (args[ARG_flow].u_int & ~UART_HWCONTROL_MASK) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad flow mask"));
        }

        if (args[ARG_flow].u_int & UART_HWCONTROL_RTS) {
            if (!lpuart_set_iomux_rts(uart_index_table[self->id])) {
                mp_raise_ValueError(MP_ERROR_TEXT("rts not available"));
            }
            self->config.enableRxRTS = true;
        }

        if (args[ARG_flow].u_int & UART_HWCONTROL_CTS) {
            if (!lpuart_set_iomux_cts(uart_index_table[self->id])) {
                mp_raise_ValueError(MP_ERROR_TEXT("cts not available"));
            }
            self->config.enableTxCTS = true;
        }
    }

    // Set timeout if configured.
    if (args[ARG_timeout].u_int >= 0) {
        self->timeout = args[ARG_timeout].u_int;
    }

    // Set timeout_char if configured.
    if (args[ARG_timeout_char].u_int >= 0) {
        self->timeout_char = args[ARG_timeout_char].u_int;
    }

    // Set line inversion if configured.
    if (args[ARG_invert].u_int >= 0) {
        if (args[ARG_invert].u_int & ~UART_INVERT_MASK) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad inversion mask"));
        }
        self->invert = args[ARG_invert].u_int;
    }

    self->tx_status = kStatus_LPUART_TxIdle;
    self->config.enableTx = true;
    self->config.enableRx = true;

    // Set the RX buffer size if configured.
    if (args[ARG_rxbuf].u_int > 0) {
        size_t rxbuf_len = args[ARG_rxbuf].u_int;
        if (rxbuf_len < MIN_BUFFER_SIZE) {
            rxbuf_len = MIN_BUFFER_SIZE;
        } else if (rxbuf_len > MAX_BUFFER_SIZE) {
            mp_raise_ValueError(MP_ERROR_TEXT("rxbuf too large"));
        }
        // Force re-allocting of the buffer if the size changed
        if (rxbuf_len != self->rxbuf_len) {
            self->rxbuf = NULL;
            self->rxbuf_len = rxbuf_len;
        }
    }

    // Set the TX buffer size if configured.
    if (args[ARG_txbuf].u_int > 0) {
        size_t txbuf_len = args[ARG_txbuf].u_int;
        if (txbuf_len < MIN_BUFFER_SIZE) {
            txbuf_len = MIN_BUFFER_SIZE;
        } else if (txbuf_len > MAX_BUFFER_SIZE) {
            mp_raise_ValueError(MP_ERROR_TEXT("txbuf too large"));
        }
        // Force re-allocting of the buffer if the size is changed
        if (txbuf_len != self->txbuf_len) {
            self->txbuf = NULL;
            self->txbuf_len = txbuf_len;
        }
    }

    // Initialise the UART peripheral if any arguments given, or it was not initialised previously.
    if (n_args > 0 || kw_args->used > 0 || self->new) {
        self->new = false;
        // may be obsolete
        if (self->config.baudRate_Bps == 0) {
            self->config.baudRate_Bps = DEFAULT_UART_BAUDRATE;
        }

        // Make sure timeout_char is at least as long as a whole character (13 bits to be safe).
        uint32_t min_timeout_char = 13000 / self->config.baudRate_Bps + 1;
        if (self->timeout_char < min_timeout_char) {
            self->timeout_char = min_timeout_char;
        }

        self->config.rxIdleType = kLPUART_IdleTypeStartBit;
        self->config.rxIdleConfig = kLPUART_IdleCharacter4;
        #if defined(MIMXRT117x_SERIES)
        // Use the Lpuart1 clock value, which is set for All UART devices.
        LPUART_Init(self->lpuart, &self->config, CLOCK_GetRootClockFreq(kCLOCK_Root_Lpuart1));
        #else
        LPUART_Init(self->lpuart, &self->config, CLOCK_GetClockRootFreq(kCLOCK_UartClkRoot));
        #endif
        LPUART_TransferCreateHandle(self->lpuart, &self->handle,  LPUART_UserCallback, self);
        if (self->rxbuf == NULL) {
            self->rxbuf = m_new(uint8_t, self->rxbuf_len + 1);
        }
        LPUART_TransferStartRingBuffer(self->lpuart, &self->handle, self->rxbuf, self->rxbuf_len);
        if (self->txbuf == NULL) {
            self->txbuf = m_new(uint8_t, self->txbuf_len); // Allocate the TX buffer.
        }

        #if MICROPY_PY_MACHINE_UART_IRQ
        LPUART_EnableInterrupts(self->lpuart, kLPUART_IdleLineInterruptEnable);
        #endif

        // The Uart supports inverting, but not the fsl API, so it has to coded directly
        // And it has to be done after LPUART_Init.
        if (self->invert & UART_INVERT_RX) {
            LPUART_EnableRx(self->lpuart, false);
            self->lpuart->STAT |= 1 << LPUART_STAT_RXINV_SHIFT;
            LPUART_EnableRx(self->lpuart, true);
        }
        if (self->invert & UART_INVERT_TX) {
            LPUART_EnableTx(self->lpuart, false);
            self->lpuart->CTRL |= 1 << LPUART_CTRL_TXINV_SHIFT;
            LPUART_EnableTx(self->lpuart, true);
        }
        // Send long break; drop that code for a shorter break duration
        LPUART_EnableTx(self->lpuart, false);
        self->lpuart->STAT |= 1 << LPUART_STAT_BRK13_SHIFT;
        LPUART_EnableTx(self->lpuart, true);
    }
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    // Get UART bus.
    int uart_id;
    if (n_args > 0) {
        uart_id = mp_obj_get_int(args[0]);
        n_args--;
        args++;
    } else {
        uart_id = DEFAULT_UART_ID;
    }
    if (uart_id < 0 || uart_id > MICROPY_HW_UART_NUM || uart_index_table[uart_id] == 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), uart_id);
    }

    // Create the UART object and fill it with defaults.
    uint8_t uart_hw_id = uart_index_table[uart_id];  // the hw uart number 1..n
    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
    self->id = uart_id;
    self->lpuart = uart_base_ptr_table[uart_hw_id];
    self->invert = false;
    self->timeout = 1;
    self->timeout_char = 1;
    self->rxbuf = NULL;
    self->rxbuf_len = DEFAULT_BUFFER_SIZE;
    self->txbuf = NULL;
    self->txbuf_len = DEFAULT_BUFFER_SIZE;
    self->new = true;
    self->mp_irq_obj = NULL;
    self->mp_irq_trigger = 0;

    LPUART_GetDefaultConfig(&self->config);

    // Configure board-specific pin MUX based on the hardware device number.
    bool uart_present = lpuart_set_iomux(uart_hw_id);

    if (uart_present) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        mp_machine_uart_init_helper(self, n_args, args, &kw_args);
        return MP_OBJ_FROM_PTR(self);
    } else {
        return mp_const_none;
    }
}

// uart.deinit()
static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    LPUART_SoftwareReset(self->lpuart);
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    machine_uart_ensure_active(self);
    size_t count = LPUART_TransferGetRxRingBufferLength(self->lpuart, &self->handle);
    return count;
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return self->tx_status == kStatus_LPUART_TxIdle;
}

static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    machine_uart_ensure_active(self);
    self->lpuart->CTRL |= 1 << LPUART_CTRL_SBK_SHIFT; // Set SBK bit
    self->lpuart->CTRL &= ~LPUART_CTRL_SBK_MASK; // Clear SBK bit
}

// Reset all defined UARTs
void machine_uart_deinit_all(void) {
    for (int i = 0; i < MICROPY_HW_UART_NUM; i++) {
        if (uart_index_table[i] != 0) {
            LPUART_SoftwareReset(uart_base_ptr_table[uart_index_table[i]]);
        }
    }
}

#if MICROPY_PY_MACHINE_UART_IRQ
static mp_uint_t uart_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->mp_irq_trigger = new_trigger;
    return 0;
}

static mp_uint_t uart_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return self->mp_irq_flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return self->mp_irq_trigger;
    }
    return 0;
}

static const mp_irq_methods_t uart_irq_methods = {
    .trigger = uart_irq_trigger,
    .info = uart_irq_info,
};

static mp_irq_obj_t *mp_machine_uart_irq(machine_uart_obj_t *self, bool any_args, mp_arg_val_t *args) {
    if (self->mp_irq_obj == NULL) {
        self->mp_irq_trigger = 0;
        self->mp_irq_obj = mp_irq_new(&uart_irq_methods, MP_OBJ_FROM_PTR(self));
    }

    if (any_args) {
        // Check the handler
        mp_obj_t handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
        }

        // Check the trigger
        mp_uint_t trigger = args[MP_IRQ_ARG_INIT_trigger].u_int;
        mp_uint_t not_supported = trigger & ~MP_UART_ALLOWED_FLAGS;
        if (trigger != 0 && not_supported) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("trigger 0x%04x unsupported"), not_supported);
        }

        self->mp_irq_obj->handler = handler;
        self->mp_irq_obj->ishard = args[MP_IRQ_ARG_INIT_hard].u_bool;
        self->mp_irq_trigger = trigger;
    }

    return self->mp_irq_obj;
}
#endif

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint64_t t = ticks_us64() + (uint64_t)self->timeout * 1000;
    uint64_t timeout_char_us = (uint64_t)self->timeout_char * 1000;
    lpuart_transfer_t xfer;
    uint8_t *dest = buf_in;
    size_t avail;
    size_t nget;

    machine_uart_ensure_active(self);

    for (size_t received = 0; received < size;) {
        // Wait for the first/next character.
        while ((avail = LPUART_TransferGetRxRingBufferLength(self->lpuart, &self->handle)) <= 0) {
            if (ticks_us64() > t) { // timed out
                if (received <= 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return received;
                }
            }
            MICROPY_EVENT_POLL_HOOK
        }
        // Get as many bytes as possible to meet the need.
        nget = avail < (size - received) ? avail : size - received;
        xfer.data = dest + received;
        xfer.dataSize = nget;
        LPUART_TransferReceiveNonBlocking(self->lpuart, &self->handle, &xfer, NULL);
        received += nget;
        t = ticks_us64() + timeout_char_us;
    }
    return size;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    lpuart_transfer_t xfer;
    uint64_t t;
    size_t remaining = size;
    size_t offset = 0;
    uint8_t fifo_size = FSL_FEATURE_LPUART_FIFO_SIZEn(0);

    machine_uart_ensure_active(self);

    // First check if a previous transfer is still ongoing,
    // then wait at least the number of remaining character times.
    t = ticks_us64() + (uint64_t)(self->handle.txDataSize + fifo_size) * (13000000 / self->config.baudRate_Bps + 1000);
    while (self->tx_status != kStatus_LPUART_TxIdle) {
        if (ticks_us64() > t) { // timed out, hard error
            *errcode = MP_ETIMEDOUT;
            return MP_STREAM_ERROR;
        }
        MICROPY_EVENT_POLL_HOOK
    }

    // Check if the first part has to be sent semi-blocking.
    if (size > self->txbuf_len) {
        // Send the first block.
        xfer.data = (uint8_t *)buf_in;
        offset = xfer.dataSize = size - self->txbuf_len;
        self->tx_status = kStatus_LPUART_TxBusy;
        LPUART_TransferSendNonBlocking(self->lpuart, &self->handle, &xfer);

        // Wait at least the number of character times for this chunk.
        t = ticks_us64() + (uint64_t)xfer.dataSize * (13000000 / self->config.baudRate_Bps + 1000);
        while (self->tx_status != kStatus_LPUART_TxIdle) {
            // Wait for the first/next character to be sent.
            if (ticks_us64() > t) { // timed out
                if (self->handle.txDataSize >= size) {
                    *errcode = MP_ETIMEDOUT;
                    return MP_STREAM_ERROR;
                } else {
                    return size - self->handle.txDataSize;
                }
            }
            MICROPY_EVENT_POLL_HOOK
        }
        remaining = self->txbuf_len;
    } else {
        // The data fits into the tx buffer.
        offset = 0;
        remaining = size;
    }

    // Send the remaining data without waiting for completion.
    memcpy(self->txbuf, (uint8_t *)buf_in + offset, remaining);
    xfer.data = self->txbuf;
    xfer.dataSize = remaining;
    self->tx_status = kStatus_LPUART_TxBusy;
    LPUART_TransferSendNonBlocking(self->lpuart, &self->handle, &xfer);

    return size;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        machine_uart_ensure_active(self);
        uintptr_t flags = arg;
        ret = 0;
        if (flags & MP_STREAM_POLL_RD) {
            uint32_t count;
            count = LPUART_TransferGetRxRingBufferLength(self->lpuart, &self->handle);
            if (count > 0) {
                ret |= MP_STREAM_POLL_RD;
            }
        }
        if ((flags & MP_STREAM_POLL_WR) && (self->tx_status == kStatus_LPUART_TxIdle)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        // The timeout is estimated using the buffer size and the baudrate.
        // Take the worst case assumptions at 13 bit symbol size times 2.
        uint64_t timeout = (uint64_t)(3 + self->txbuf_len) * 13000000ll * 2 /
            self->config.baudRate_Bps + ticks_us64();

        do {
            if (mp_machine_uart_txdone(self)) {
                return 0;
            }
            MICROPY_EVENT_POLL_HOOK
        } while (ticks_us64() < timeout);

        *errcode = MP_ETIMEDOUT;
        ret = MP_STREAM_ERROR;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}
