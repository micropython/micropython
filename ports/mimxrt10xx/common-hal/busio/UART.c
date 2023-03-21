/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George
 * Copyright (c) 2019 Artur Pacholec
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

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/busio/UART.h"

#include "mpconfigport.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/shared/tick.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "periph.h"

#include "sdk/drivers/lpuart/fsl_lpuart.h"
#include "sdk/drivers/igpio/fsl_gpio.h"
// ==========================================================
// Debug code
// ==========================================================
#define ENABLE_DEBUG_PRINTING 0
#if ENABLE_DEBUG_PRINTING
#define DBGPrintf mp_printf
#else
#define DBGPrintf(p,...)
#endif


// arrays use 0 based numbering: UART1 is stored at index 0
#define MAX_UART 8
STATIC bool reserved_uart[MAX_UART];
STATIC bool never_reset_uart[MAX_UART];

#define UART_CLOCK_FREQ (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U)

static void config_periph_pin(const mcu_periph_obj_t *periph) {
    IOMUXC_SetPinMux(
        periph->pin->mux_reg, periph->mux_mode,
        periph->input_reg, periph->input_idx,
        0,
        0);

    IOMUXC_SetPinConfig(0, 0, 0, 0,
        periph->pin->cfg_reg,
        IOMUXC_SW_PAD_CTL_PAD_HYS(0)
        | IOMUXC_SW_PAD_CTL_PAD_PUS(1)
        | IOMUXC_SW_PAD_CTL_PAD_PUE(1)
        | IOMUXC_SW_PAD_CTL_PAD_PKE(1)
        | IOMUXC_SW_PAD_CTL_PAD_ODE(0)
        | IOMUXC_SW_PAD_CTL_PAD_SPEED(1)
        | IOMUXC_SW_PAD_CTL_PAD_DSE(6)
        | IOMUXC_SW_PAD_CTL_PAD_SRE(0));
}

STATIC void LPUART_UserCallback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *user_data) {
    busio_uart_obj_t *self = (busio_uart_obj_t *)user_data;

    if (status == kStatus_LPUART_RxIdle) {
        self->rx_ongoing = false;
    }
}

void uart_reset(void) {
    for (uint i = 0; i < MP_ARRAY_SIZE(mcu_uart_banks); i++) {
        if (never_reset_uart[i]) {
            continue;
        }
        reserved_uart[i] = false;
        LPUART_Deinit(mcu_uart_banks[i]);
    }
}

void common_hal_busio_uart_never_reset(busio_uart_obj_t *self) {
    never_reset_uart[self->index] = true;
    common_hal_never_reset_pin(self->tx);
    common_hal_never_reset_pin(self->rx);
    common_hal_never_reset_pin(self->rts);
    common_hal_never_reset_pin(self->cts);
    common_hal_never_reset_pin(self->rs485_dir);
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {

    self->baudrate = baudrate;
    self->character_bits = (uint8_t)mp_arg_validate_int_range(bits, 7, 8, MP_QSTR_bits);
    self->timeout_ms = timeout * 1000;


    DBGPrintf(&mp_plat_print, "uart_construct: tx:%p rx:%p rts:%p cts:%p rs485:%p\n", tx, rx, rts, cts, rs485_dir);

    // We are transmitting one direction if one pin is NULL and the other isn't.
    bool is_onedirection = (rx == NULL) != (tx == NULL);
    bool uart_taken = false;
    bool use_rts_for_rs485 = false;

    const uint32_t rx_count = MP_ARRAY_SIZE(mcu_uart_rx_list);
    const uint32_t tx_count = MP_ARRAY_SIZE(mcu_uart_tx_list);

    const mcu_periph_obj_t *tx_config = NULL;
    const mcu_periph_obj_t *rx_config = NULL;
    const mcu_periph_obj_t *rts_config = NULL;
    const mcu_periph_obj_t *cts_config = NULL;

    // RX loop handles rx only, or both rx and tx
    if (rx != NULL) {
        for (uint32_t i = 0; i < rx_count; ++i) {
            if (mcu_uart_rx_list[i].pin != rx) {
                continue;
            }
            // If TX is on, keep looking, else stop
            if (tx != NULL) {
                for (uint32_t j = 0; j < tx_count; ++j) {
                    if (mcu_uart_tx_list[j].pin != tx ||
                        mcu_uart_tx_list[j].bank_idx != mcu_uart_rx_list[i].bank_idx) {
                        continue;
                    }
                    // If UART is taken, break (pins never have >1 periph)
                    if (reserved_uart[mcu_uart_rx_list[i].bank_idx - 1]) {
                        uart_taken = true;
                        break;
                    }
                    rx_config = &mcu_uart_rx_list[i];
                    tx_config = &mcu_uart_tx_list[j];
                    break;
                }
                if (tx_config != NULL || uart_taken) {
                    break;
                }
            } else {
                if (reserved_uart[mcu_uart_rx_list[i].bank_idx - 1]) {
                    uart_taken = true;
                    break;
                }
                rx_config = &mcu_uart_rx_list[i];
            }
        }
    } else if (tx != NULL) {
        // TX only case
        for (uint32_t i = 0; i < tx_count; ++i) {
            if (mcu_uart_tx_list[i].pin != tx) {
                continue;
            }
            if (reserved_uart[mcu_uart_tx_list[i].bank_idx - 1]) {
                uart_taken = true;
                break;
            }
            tx_config = &mcu_uart_tx_list[i];
            break;
        }
    } else {
        // TX and RX are both None. But this is already handled in shared-bindings, so
        // we won't get here.
    }

    if (rx && !rx_config) {
        raise_ValueError_invalid_pin_name(MP_QSTR_rx);
    }
    if (tx && !tx_config) {
        raise_ValueError_invalid_pin_name(MP_QSTR_tx);
    }

    if (uart_taken) {
        mp_raise_ValueError(translate("Hardware in use, try alternative pins"));
    }

    if (is_onedirection && ((rts != NULL) || (cts != NULL))) {
        mp_raise_ValueError(translate("Both RX and TX required for flow control"));
    }

    // Filter for sane settings for RS485
    if (rs485_dir != NULL) {
        DBGPrintf(&mp_plat_print, "\t(485 pin): gpio:%p #:%x Mux: %x %x cfg:%x reset:%x %x\n",
            rs485_dir->gpio, rs485_dir->number, rs485_dir->mux_idx, rs485_dir->mux_reg, rs485_dir->cfg_reg,
            rs485_dir->mux_reset, rs485_dir->pad_reset);
        if ((rts != NULL) || (cts != NULL)) {
            mp_raise_ValueError(translate("Cannot specify RTS or CTS in RS485 mode"));
        }
        // For IMXRT the RTS pin is used for RS485 direction ???? - Can be will try
        // it if this is an rts pin.
    } else {
        if (rs485_invert) {
            mp_raise_ValueError(translate("RS485 inversion specified when not in RS485 mode"));
        }
    }

    // Now check for RTS/CTS (or overloaded RS485 direction) pin(s)
    const uint32_t rts_count = MP_ARRAY_SIZE(mcu_uart_rts_list);
    const uint32_t cts_count = MP_ARRAY_SIZE(mcu_uart_cts_list);

    if ((rts != NULL) || (rs485_dir != NULL)) {
        for (uint32_t i = 0; i < rts_count; ++i) {
            if (mcu_uart_rts_list[i].bank_idx == rx_config->bank_idx) {
                if (mcu_uart_rts_list[i].pin == rts) {
                    rts_config = &mcu_uart_rts_list[i];
                    break;
                } else if (mcu_uart_rts_list[i].pin == rs485_dir) {
                    rts_config = &mcu_uart_rts_list[i];
                    use_rts_for_rs485 = true;
                    rts = rs485_dir;
                    rs485_dir = NULL;
                    break;
                }
            }
        }
        if ((rts != NULL) && (rts_config == NULL)) {
            raise_ValueError_invalid_pin_name(MP_QSTR_rts);
        }
    }

    if (cts != NULL) {
        for (uint32_t i = 0; i < cts_count; ++i) {
            if (mcu_uart_cts_list[i].bank_idx == tx_config->bank_idx) {
                if (mcu_uart_cts_list[i].pin == cts) {
                    cts_config = &mcu_uart_cts_list[i];
                    break;
                }
            }
        }
        if (cts_config == NULL) {
            raise_ValueError_invalid_pin_name(MP_QSTR_cts);
        }
    }


    if (self->rx) {
        self->index = rx_config->bank_idx - 1;
    } else {
        assert(self->tx);
        self->index = tx_config->bank_idx - 1;
    }
    self->uart = mcu_uart_banks[self->index];

    assert(self->uart);

    if (rx_config) {
        config_periph_pin(rx_config);
        self->rx = rx;
    }
    if (tx_config) {
        config_periph_pin(tx_config);
        self->tx = tx;
    }
    if (rts_config) {
        config_periph_pin(rts_config);
        self->rts = rts;
    }
    if (cts_config) {
        config_periph_pin(cts_config);
        self->cts = cts;
    }
    if (rs485_dir) {
        DBGPrintf(&mp_plat_print, "\tInit rs485 pin\n");
        // lets configure this pin as standard GPIO output pin.
        claim_pin(rs485_dir);

        #define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5 5U
        IOMUXC_SetPinMux(rs485_dir->mux_reg, IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_ALT5, 0, 0, 0, 0);
        DBGPrintf(&mp_plat_print, "\tAfter IOMUXC_SetPinMux\n");
        IOMUXC_SetPinConfig(0, 0, 0, 0, rs485_dir->cfg_reg,
            IOMUXC_SW_PAD_CTL_PAD_HYS(1)
            | IOMUXC_SW_PAD_CTL_PAD_PUS(0)
            | IOMUXC_SW_PAD_CTL_PAD_PUE(0)
            | IOMUXC_SW_PAD_CTL_PAD_PKE(1)
            | IOMUXC_SW_PAD_CTL_PAD_ODE(0)
            | IOMUXC_SW_PAD_CTL_PAD_SPEED(2)
            | IOMUXC_SW_PAD_CTL_PAD_DSE(1)
            | IOMUXC_SW_PAD_CTL_PAD_SRE(0));
        DBGPrintf(&mp_plat_print, "\tAfter IOMUXC_SetPinConfig\n");

        const gpio_pin_config_t config = { kGPIO_DigitalOutput, rs485_invert, kGPIO_NoIntmode };
        GPIO_PinInit(rs485_dir->gpio, rs485_dir->number, &config);
        DBGPrintf(&mp_plat_print, "\tAfter GPIO_PinInit\n");
        self->rs485_dir = rs485_dir;
        self->rs485_invert = rs485_invert;

    }
    lpuart_config_t config = { 0 };
    LPUART_GetDefaultConfig(&config);

    config.dataBitsCount = self->character_bits == 8 ? kLPUART_EightDataBits : kLPUART_SevenDataBits;
    config.baudRate_Bps = self->baudrate;
    config.enableTx = self->tx != NULL;
    config.enableRx = self->rx != NULL;
    config.enableRxRTS = self->rts != NULL;
    config.enableTxCTS = self->cts != NULL;
    if (self->rts != NULL) {
        claim_pin(self->rts);
    }
    if (self->cts != NULL) {
        claim_pin(self->cts);
    }

    LPUART_Init(self->uart, &config, UART_CLOCK_FREQ);

    // Before we init, setup RS485 direction pin
    // ..unfortunately this isn't done by the driver library
    uint32_t modir = (self->uart->MODIR) & ~(LPUART_MODIR_TXRTSPOL_MASK | LPUART_MODIR_TXRTSE_MASK);
    if (use_rts_for_rs485) {
        modir |= LPUART_MODIR_TXRTSE_MASK;
        if (rs485_invert) {
            modir |= LPUART_MODIR_TXRTSPOL_MASK;
        }
    }
    self->uart->MODIR = modir;

    if (self->tx != NULL) {
        claim_pin(self->tx);
    }

    if (self->rx != NULL) {
        if (receiver_buffer == NULL) {
            self->ringbuf = gc_alloc(receiver_buffer_size, false, true /*long-lived*/);
        } else {
            self->ringbuf = receiver_buffer;
        }


        if (!self->ringbuf) {
            LPUART_Deinit(self->uart);
            m_malloc_fail(receiver_buffer_size);
        }

        LPUART_TransferCreateHandle(self->uart, &self->handle, LPUART_UserCallback, self);
        // Pass actual allocated size; the LPUART routines are cognizant that
        // the capacity is one less than the size.
        LPUART_TransferStartRingBuffer(self->uart, &self->handle, self->ringbuf, receiver_buffer_size);

        claim_pin(self->rx);
    }
    DBGPrintf(&mp_plat_print, "\t<< Init completed >>\n");
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->rx == NULL && self->tx == NULL;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }

    reserved_uart[self->index] = false;
    never_reset_uart[self->index] = false;

    LPUART_Deinit(self->uart);
    gc_free(self->ringbuf);

    common_hal_reset_pin(self->rx);
    common_hal_reset_pin(self->tx);
    common_hal_reset_pin(self->cts);
    common_hal_reset_pin(self->rts);
    common_hal_reset_pin(self->rs485_dir);

    self->rx = NULL;
    self->tx = NULL;
    self->cts = NULL;
    self->rts = NULL;
    self->rs485_dir = NULL;

}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (self->rx == NULL) {
        mp_raise_ValueError(translate("No RX pin"));
    }

    if (len == 0) {
        // Nothing to read.
        return 0;
    }

    lpuart_transfer_t xfer = {
        .data = data,
        .dataSize = len,
    };

    self->rx_ongoing = true;
    LPUART_TransferReceiveNonBlocking(self->uart, &self->handle, &xfer, NULL);

    uint64_t start_ticks = supervisor_ticks_ms64();

    // Wait for all bytes received or timeout
    while (self->rx_ongoing && (supervisor_ticks_ms64() - start_ticks < self->timeout_ms)) {
        RUN_BACKGROUND_TASKS;

        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            break;
        }
    }

    // if we timed out, stop the transfer
    if (self->rx_ongoing) {
        uint32_t recvd = 0;
        LPUART_TransferGetReceiveCount(self->uart, &self->handle, &recvd);
        LPUART_TransferAbortReceive(self->uart, &self->handle);
        if (recvd == 0) {
            *errcode = EAGAIN;
            return MP_STREAM_ERROR;
        }
        return recvd;
    }

    // No data left, we got it all
    if (self->handle.rxData == NULL) {
        return len;
    }

    // The only place we can reliably tell how many bytes have been received is from the current
    // wp in the handle (because the abort nukes rxDataSize, and reading it before abort is a race.)
    if (self->handle.rxData > data) {
        return self->handle.rxData - data;
    } else {
        return len;
    }
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (self->tx == NULL) {
        mp_raise_ValueError(translate("No TX pin"));
    }
    if (self->rs485_dir && len) {
        GPIO_PinWrite(self->rs485_dir->gpio, self->rs485_dir->number, !self->rs485_invert);
        LPUART_WriteBlocking(self->uart, data, len);
        // Probably need to verify we have completed output.
        uint32_t dont_hang_count = 0xffff;
        while (dont_hang_count--) {
            if (LPUART_GetStatusFlags(self->uart) & kLPUART_TransmissionCompleteFlag) {
                break; // hardware says it completed.
            }
        }
        GPIO_PinWrite(self->rs485_dir->gpio, self->rs485_dir->number, self->rs485_invert);
    } else {
        // could combine with above but would go through two ifs
        LPUART_WriteBlocking(self->uart, data, len);
    }

    return len;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return self->baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    if (LPUART_SetBaudRate(self->uart, baudrate, UART_CLOCK_FREQ) == kStatus_Success) {
        self->baudrate = baudrate;
    }
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t)(self->timeout_ms / 1000.0f);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return LPUART_TransferGetRxRingBufferLength(self->uart, &self->handle);
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    self->handle.rxRingBufferHead = self->handle.rxRingBufferTail;
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    if (self->tx == NULL) {
        return false;
    }

    return LPUART_GetStatusFlags(self->uart) & kLPUART_TxDataRegEmptyFlag;
}
