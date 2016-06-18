/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <stdarg.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "uart.h"
#include "pybioctl.h"
#include "irq.h"

//TODO: Add UART7/8 support for MCU_SERIES_F7

/// \moduleref pyb
/// \class UART - duplex serial communication bus
///
/// UART implements the standard UART/USART duplex serial communications protocol.  At
/// the physical level it consists of 2 lines: RX and TX.  The unit of communication
/// is a character (not to be confused with a string character) which can be 8 or 9
/// bits wide.
///
/// UART objects can be created and initialised using:
///
///     from pyb import UART
///
///     uart = UART(1, 9600)                         # init with given baudrate
///     uart.init(9600, bits=8, parity=None, stop=1) # init with given parameters
///
/// Bits can be 8 or 9.  Parity can be None, 0 (even) or 1 (odd).  Stop can be 1 or 2.
///
/// A UART object acts like a stream object and reading and writing is done
/// using the standard stream methods:
///
///     uart.read(10)       # read 10 characters, returns a bytes object
///     uart.readall()      # read all available characters
///     uart.readline()     # read a line
///     uart.readinto(buf)  # read and store into the given buffer
///     uart.write('abc')   # write the 3 characters
///
/// Individual characters can be read/written using:
///
///     uart.readchar()     # read 1 character and returns it as an integer
///     uart.writechar(42)  # write 1 character
///
/// To check if there is anything to be read, use:
///
///     uart.any()               # returns True if any characters waiting

#define CHAR_WIDTH_8BIT (0)
#define CHAR_WIDTH_9BIT (1)

struct _pyb_uart_obj_t {
    mp_obj_base_t base;
    UART_HandleTypeDef uart;            // this is 17 words big
    IRQn_Type irqn;
    pyb_uart_t uart_id : 8;
    bool is_enabled : 1;
    byte char_width;                    // 0 for 7,8 bit chars, 1 for 9 bit chars
    uint16_t char_mask;                 // 0x7f for 7 bit, 0xff for 8 bit, 0x1ff for 9 bit
    uint16_t timeout;                   // timeout waiting for first char
    uint16_t timeout_char;              // timeout waiting between chars
    uint16_t read_buf_len;              // len in chars; buf can hold len-1 chars
    volatile uint16_t read_buf_head;    // indexes first empty slot
    uint16_t read_buf_tail;             // indexes first full slot (not full if equals head)
    byte *read_buf;                     // byte or uint16_t, depending on char size
};

STATIC mp_obj_t pyb_uart_deinit(mp_obj_t self_in);

void uart_init0(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_uart_obj_all)); i++) {
        MP_STATE_PORT(pyb_uart_obj_all)[i] = NULL;
    }
}

// unregister all interrupt sources
void uart_deinit(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_uart_obj_all)); i++) {
        pyb_uart_obj_t *uart_obj = MP_STATE_PORT(pyb_uart_obj_all)[i];
        if (uart_obj != NULL) {
            pyb_uart_deinit(uart_obj);
        }
    }
}

// assumes Init parameters have been set up correctly
STATIC bool uart_init2(pyb_uart_obj_t *uart_obj) {
    USART_TypeDef *UARTx;
    IRQn_Type irqn;
    uint32_t GPIO_Pin, GPIO_Pin2 = 0;
    uint8_t GPIO_AF_UARTx = 0;
    GPIO_TypeDef* GPIO_Port = NULL;
    GPIO_TypeDef* GPIO_Port2 = NULL;

    switch (uart_obj->uart_id) {
        #if defined(MICROPY_HW_UART1_PORT) && defined(MICROPY_HW_UART1_PINS)
        // USART1 is on PA9/PA10 (CK on PA8), PB6/PB7
        case PYB_UART_1:
            UARTx = USART1;
            irqn = USART1_IRQn;
            GPIO_AF_UARTx = GPIO_AF7_USART1;
            GPIO_Port = MICROPY_HW_UART1_PORT;
            GPIO_Pin = MICROPY_HW_UART1_PINS;
            __USART1_CLK_ENABLE();
            break;
        #endif

        #if defined(MICROPY_HW_UART1_TX_PORT) && \
            defined(MICROPY_HW_UART1_TX_PIN) && \
            defined(MICROPY_HW_UART1_RX_PORT) && \
            defined(MICROPY_HW_UART1_RX_PIN)
        case PYB_UART_1:
            UARTx = USART1;
            irqn = USART1_IRQn;
            GPIO_AF_UARTx = GPIO_AF7_USART1;
            GPIO_Port  = MICROPY_HW_UART1_TX_PORT;
            GPIO_Pin   = MICROPY_HW_UART1_TX_PIN;
            GPIO_Port2 = MICROPY_HW_UART1_RX_PORT;
            GPIO_Pin2  = MICROPY_HW_UART1_RX_PIN;
            __USART1_CLK_ENABLE();
            break;
        #endif

        #if defined(MICROPY_HW_UART2_PORT) && defined(MICROPY_HW_UART2_PINS)
        case PYB_UART_2:
            UARTx = USART2;
            irqn = USART2_IRQn;
            GPIO_AF_UARTx = GPIO_AF7_USART2;
            GPIO_Port = MICROPY_HW_UART2_PORT;
            GPIO_Pin = MICROPY_HW_UART2_PINS;
            #if defined(MICROPY_HW_UART2_RTS)
            if (uart_obj->uart.Init.HwFlowCtl & UART_HWCONTROL_RTS) {
                GPIO_Pin |= MICROPY_HW_UART2_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART2_CTS)
            if (uart_obj->uart.Init.HwFlowCtl & UART_HWCONTROL_CTS) {
                GPIO_Pin |= MICROPY_HW_UART2_CTS;
            }
            #endif
            __USART2_CLK_ENABLE();
            break;
        #endif

        #if defined(USART3) && defined(MICROPY_HW_UART3_PORT) && defined(MICROPY_HW_UART3_PINS)
        // USART3 is on PB10/PB11 (CK,CTS,RTS on PB12,PB13,PB14), PC10/PC11 (CK on PC12), PD8/PD9 (CK on PD10)
        case PYB_UART_3:
            UARTx = USART3;
            irqn = USART3_IRQn;
            GPIO_AF_UARTx = GPIO_AF7_USART3;
            GPIO_Port = MICROPY_HW_UART3_PORT;
            GPIO_Pin = MICROPY_HW_UART3_PINS;
            #if defined(MICROPY_HW_UART3_RTS)
            if (uart_obj->uart.Init.HwFlowCtl & UART_HWCONTROL_RTS) {
                GPIO_Pin |= MICROPY_HW_UART3_RTS;
            }
            #endif
            #if defined(MICROPY_HW_UART3_CTS)
            if (uart_obj->uart.Init.HwFlowCtl & UART_HWCONTROL_CTS) {
                GPIO_Pin |= MICROPY_HW_UART3_CTS;
            }
            #endif
            __USART3_CLK_ENABLE();
            break;
        #endif

        #if defined(UART4) && defined(MICROPY_HW_UART4_PORT) && defined(MICROPY_HW_UART4_PINS)
        // UART4 is on PA0/PA1, PC10/PC11
        case PYB_UART_4:
            UARTx = UART4;
            irqn = UART4_IRQn;
            GPIO_AF_UARTx = GPIO_AF8_UART4;
            GPIO_Port = MICROPY_HW_UART4_PORT;
            GPIO_Pin = MICROPY_HW_UART4_PINS;
            __UART4_CLK_ENABLE();
            break;
        #endif

        #if defined(UART5) && \
            defined(MICROPY_HW_UART5_TX_PORT) && \
            defined(MICROPY_HW_UART5_TX_PIN) && \
            defined(MICROPY_HW_UART5_RX_PORT) && \
            defined(MICROPY_HW_UART5_RX_PIN)
        case PYB_UART_5:
            UARTx = UART5;
            irqn = UART5_IRQn;
            GPIO_AF_UARTx = GPIO_AF8_UART5;
            GPIO_Port = MICROPY_HW_UART5_TX_PORT;
            GPIO_Port2 = MICROPY_HW_UART5_RX_PORT;
            GPIO_Pin = MICROPY_HW_UART5_TX_PIN;
            GPIO_Pin2 = MICROPY_HW_UART5_RX_PIN;
            __UART5_CLK_ENABLE();
            break;
        #endif

        #if defined(MICROPY_HW_UART6_PORT) && defined(MICROPY_HW_UART6_PINS)
        // USART6 is on PC6/PC7 (CK on PC8)
        case PYB_UART_6:
            UARTx = USART6;
            irqn = USART6_IRQn;
            GPIO_AF_UARTx = GPIO_AF8_USART6;
            GPIO_Port = MICROPY_HW_UART6_PORT;
            GPIO_Pin = MICROPY_HW_UART6_PINS;
            __USART6_CLK_ENABLE();
            break;
        #endif

        default:
            // UART does not exist or is not configured for this board
            return false;
    }

    uart_obj->irqn = irqn;
    uart_obj->uart.Instance = UARTx;

    // init GPIO
    mp_hal_gpio_clock_enable(GPIO_Port);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_Pin;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = GPIO_AF_UARTx;
    HAL_GPIO_Init(GPIO_Port, &GPIO_InitStructure);

    // init GPIO for second pin if needed
    if (GPIO_Port2 != NULL) {
        mp_hal_gpio_clock_enable(GPIO_Port2);
        GPIO_InitStructure.Pin = GPIO_Pin2;
        HAL_GPIO_Init(GPIO_Port2, &GPIO_InitStructure);
    }

    // init UARTx
    HAL_UART_Init(&uart_obj->uart);

    uart_obj->is_enabled = true;

    return true;
}

/* obsolete and unused
bool uart_init(pyb_uart_obj_t *uart_obj, uint32_t baudrate) {
    UART_HandleTypeDef *uh = &uart_obj->uart;
    memset(uh, 0, sizeof(*uh));
    uh->Init.BaudRate = baudrate;
    uh->Init.WordLength = UART_WORDLENGTH_8B;
    uh->Init.StopBits = UART_STOPBITS_1;
    uh->Init.Parity = UART_PARITY_NONE;
    uh->Init.Mode = UART_MODE_TX_RX;
    uh->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uh->Init.OverSampling = UART_OVERSAMPLING_16;
    return uart_init2(uart_obj);
}
*/

mp_uint_t uart_rx_any(pyb_uart_obj_t *self) {
    int buffer_bytes = self->read_buf_head - self->read_buf_tail;
    if (buffer_bytes < 0) {
        return buffer_bytes + self->read_buf_len;
    } else if (buffer_bytes > 0) {
        return buffer_bytes;
    } else {
        return __HAL_UART_GET_FLAG(&self->uart, UART_FLAG_RXNE) != RESET;
    }
}

// Waits at most timeout milliseconds for at least 1 char to become ready for
// reading (from buf or for direct reading).
// Returns true if something available, false if not.
STATIC bool uart_rx_wait(pyb_uart_obj_t *self, uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    for (;;) {
        if (self->read_buf_tail != self->read_buf_head || __HAL_UART_GET_FLAG(&self->uart, UART_FLAG_RXNE) != RESET) {
            return true; // have at least 1 char ready for reading
        }
        if (HAL_GetTick() - start >= timeout) {
            return false; // timeout
        }
        __WFI();
    }
}

// assumes there is a character available
int uart_rx_char(pyb_uart_obj_t *self) {
    if (self->read_buf_tail != self->read_buf_head) {
        // buffering via IRQ
        int data;
        if (self->char_width == CHAR_WIDTH_9BIT) {
            data = ((uint16_t*)self->read_buf)[self->read_buf_tail];
        } else {
            data = self->read_buf[self->read_buf_tail];
        }
        self->read_buf_tail = (self->read_buf_tail + 1) % self->read_buf_len;
        if (__HAL_UART_GET_FLAG(&self->uart, UART_FLAG_RXNE) != RESET) {
            // UART was stalled by flow ctrl: re-enable IRQ now we have room in buffer
            __HAL_UART_ENABLE_IT(&self->uart, UART_IT_RXNE);
        }
        return data;
    } else {
        // no buffering
        #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_L4)
        return self->uart.Instance->RDR & self->char_mask;
        #else
        return self->uart.Instance->DR & self->char_mask;
        #endif
    }
}

// Waits at most timeout milliseconds for TX register to become empty.
// Returns true if can write, false if can't.
STATIC bool uart_tx_wait(pyb_uart_obj_t *self, uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    for (;;) {
        if (__HAL_UART_GET_FLAG(&self->uart, UART_FLAG_TXE)) {
            return true; // tx register is empty
        }
        if (HAL_GetTick() - start >= timeout) {
            return false; // timeout
        }
        __WFI();
    }
}

STATIC HAL_StatusTypeDef uart_tx_data(pyb_uart_obj_t *self, uint8_t *data, uint16_t len) {
    if (self->uart.Init.HwFlowCtl & UART_HWCONTROL_CTS) {
        // CTS can hold off transmission for an arbitrarily long time. Apply
        // the overall timeout rather than the character timeout.
        return HAL_UART_Transmit(&self->uart, data, len, self->timeout);
    }
    // The timeout specified here is for waiting for the TX data register to
    // become empty (ie between chars), as well as for the final char to be
    // completely transferred.  The default value for timeout_char is long
    // enough for 1 char, but we need to double it to wait for the last char
    // to be transferred to the data register, and then to be transmitted.
    return HAL_UART_Transmit(&self->uart, data, len, 2 * self->timeout_char);
}

STATIC void uart_tx_char(pyb_uart_obj_t *uart_obj, int c) {
    uint8_t ch = c;
    uart_tx_data(uart_obj, &ch, 1);
}

void uart_tx_strn(pyb_uart_obj_t *uart_obj, const char *str, uint len) {
    uart_tx_data(uart_obj, (uint8_t*)str, len);
}

void uart_tx_strn_cooked(pyb_uart_obj_t *uart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            uart_tx_char(uart_obj, '\r');
        }
        uart_tx_char(uart_obj, *str);
    }
}

// this IRQ handler is set up to handle RXNE interrupts only
void uart_irq_handler(mp_uint_t uart_id) {
    // get the uart object
    pyb_uart_obj_t *self = MP_STATE_PORT(pyb_uart_obj_all)[uart_id - 1];

    if (self == NULL) {
        // UART object has not been set, so we can't do anything, not
        // even disable the IRQ.  This should never happen.
        return;
    }

    if (__HAL_UART_GET_FLAG(&self->uart, UART_FLAG_RXNE) != RESET) {
        if (self->read_buf_len != 0) {
            uint16_t next_head = (self->read_buf_head + 1) % self->read_buf_len;
            if (next_head != self->read_buf_tail) {
                // only read data if room in buf
                #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_L4)
                int data = self->uart.Instance->RDR; // clears UART_FLAG_RXNE
                #else
                int data = self->uart.Instance->DR; // clears UART_FLAG_RXNE
                #endif
                data &= self->char_mask;
                if (self->char_width == CHAR_WIDTH_9BIT) {
                    ((uint16_t*)self->read_buf)[self->read_buf_head] = data;
                } else {
                    self->read_buf[self->read_buf_head] = data;
                }
                self->read_buf_head = next_head;
            } else { // No room: leave char in buf, disable interrupt
                __HAL_UART_DISABLE_IT(&self->uart, UART_IT_RXNE);
            }
        }
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

STATIC void pyb_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_uart_obj_t *self = self_in;
    if (!self->is_enabled) {
        mp_printf(print, "UART(%u)", self->uart_id);
    } else {
        mp_int_t bits = (self->uart.Init.WordLength == UART_WORDLENGTH_8B ? 8 : 9);
        if (self->uart.Init.Parity != UART_PARITY_NONE) {
            bits -= 1;
        }
        mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=",
            self->uart_id, self->uart.Init.BaudRate, bits);
        if (self->uart.Init.Parity == UART_PARITY_NONE) {
            mp_print_str(print, "None");
        } else {
            mp_printf(print, "%u", self->uart.Init.Parity == UART_PARITY_EVEN ? 0 : 1);
        }
        if (self->uart.Init.HwFlowCtl) {
            mp_printf(print, ", flow=");
            if (self->uart.Init.HwFlowCtl & UART_HWCONTROL_RTS) {
                mp_printf(print, "RTS%s", self->uart.Init.HwFlowCtl & UART_HWCONTROL_CTS ? "|" : "");
            }
            if (self->uart.Init.HwFlowCtl & UART_HWCONTROL_CTS) {
                mp_printf(print, "CTS");
            }
        }
        mp_printf(print, ", stop=%u, timeout=%u, timeout_char=%u, read_buf_len=%u)",
            self->uart.Init.StopBits == UART_STOPBITS_1 ? 1 : 2,
            self->timeout, self->timeout_char,
            self->read_buf_len == 0 ? 0 : self->read_buf_len - 1); // -1 to adjust for usable length of buffer
    }
}

/// \method init(baudrate, bits=8, parity=None, stop=1, *, timeout=1000, timeout_char=0, flow=0, read_buf_len=64)
///
/// Initialise the UART bus with the given parameters:
///
///   - `baudrate` is the clock rate.
///   - `bits` is the number of bits per byte, 7, 8 or 9.
///   - `parity` is the parity, `None`, 0 (even) or 1 (odd).
///   - `stop` is the number of stop bits, 1 or 2.
///   - `timeout` is the timeout in milliseconds to wait for the first character.
///   - `timeout_char` is the timeout in milliseconds to wait between characters.
///   - `flow` is RTS | CTS where RTS == 256, CTS == 512
///   - `read_buf_len` is the character length of the read buffer (0 to disable).
STATIC mp_obj_t pyb_uart_init_helper(pyb_uart_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_HWCONTROL_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_read_buf_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };

    // parse args
    struct {
        mp_arg_val_t baudrate, bits, parity, stop, flow, timeout, timeout_char, read_buf_len;
    } args;
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t*)&args);

    // set the UART configuration values
    memset(&self->uart, 0, sizeof(self->uart));
    UART_InitTypeDef *init = &self->uart.Init;

    // baudrate
    init->BaudRate = args.baudrate.u_int;

    // parity
    mp_int_t bits = args.bits.u_int;
    if (args.parity.u_obj == mp_const_none) {
        init->Parity = UART_PARITY_NONE;
    } else {
        mp_int_t parity = mp_obj_get_int(args.parity.u_obj);
        init->Parity = (parity & 1) ? UART_PARITY_ODD : UART_PARITY_EVEN;
        bits += 1; // STs convention has bits including parity
    }

    // number of bits
    if (bits == 8) {
        init->WordLength = UART_WORDLENGTH_8B;
    } else if (bits == 9) {
        init->WordLength = UART_WORDLENGTH_9B;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "unsupported combination of bits and parity"));
    }

    // stop bits
    switch (args.stop.u_int) {
        case 1: init->StopBits = UART_STOPBITS_1; break;
        default: init->StopBits = UART_STOPBITS_2; break;
    }

    // flow control
    init->HwFlowCtl = args.flow.u_int;

    // extra config (not yet configurable)
    init->Mode = UART_MODE_TX_RX;
    init->OverSampling = UART_OVERSAMPLING_16;

    // init UART (if it fails, it's because the port doesn't exist)
    if (!uart_init2(self)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%d) does not exist", self->uart_id));
    }

    // set timeout
    self->timeout = args.timeout.u_int;

    // set timeout_char
    // make sure it is at least as long as a whole character (13 bits to be safe)
    self->timeout_char = args.timeout_char.u_int;
    uint32_t min_timeout_char = 13000 / init->BaudRate + 1;
    if (self->timeout_char < min_timeout_char) {
        self->timeout_char = min_timeout_char;
    }

    // setup the read buffer
    m_del(byte, self->read_buf, self->read_buf_len << self->char_width);
    if (init->WordLength == UART_WORDLENGTH_9B && init->Parity == UART_PARITY_NONE) {
        self->char_mask = 0x1ff;
        self->char_width = CHAR_WIDTH_9BIT;
    } else {
        if (init->WordLength == UART_WORDLENGTH_9B || init->Parity == UART_PARITY_NONE) {
            self->char_mask = 0xff;
        } else {
            self->char_mask = 0x7f;
        }
        self->char_width = CHAR_WIDTH_8BIT;
    }
    self->read_buf_head = 0;
    self->read_buf_tail = 0;
    if (args.read_buf_len.u_int <= 0) {
        // no read buffer
        self->read_buf_len = 0;
        self->read_buf = NULL;
        HAL_NVIC_DisableIRQ(self->irqn);
        __HAL_UART_DISABLE_IT(&self->uart, UART_IT_RXNE);
    } else {
        // read buffer using interrupts
        self->read_buf_len = args.read_buf_len.u_int + 1; // +1 to adjust for usable length of buffer
        self->read_buf = m_new(byte, self->read_buf_len << self->char_width);
        __HAL_UART_ENABLE_IT(&self->uart, UART_IT_RXNE);
        HAL_NVIC_SetPriority(self->irqn, IRQ_PRI_UART, IRQ_SUBPRI_UART); 
        HAL_NVIC_EnableIRQ(self->irqn);
    }

    // compute actual baudrate that was configured
    // (this formula assumes UART_OVERSAMPLING_16)
    uint32_t actual_baudrate;
    if (self->uart.Instance == USART1
        #if defined(USART6)
        || self->uart.Instance == USART6
        #endif
        ) {
        actual_baudrate = HAL_RCC_GetPCLK2Freq();
    } else {
        actual_baudrate = HAL_RCC_GetPCLK1Freq();
    }
    actual_baudrate /= self->uart.Instance->BRR;

    // check we could set the baudrate within 5%
    uint32_t baudrate_diff;
    if (actual_baudrate > init->BaudRate) {
        baudrate_diff = actual_baudrate - init->BaudRate;
    } else {
        baudrate_diff = init->BaudRate - actual_baudrate;
    }
    init->BaudRate = actual_baudrate; // remember actual baudrate for printing
    if (20 * baudrate_diff > init->BaudRate) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "set baudrate %d is not within 5%% of desired value", actual_baudrate));
    }

    return mp_const_none;
}

/// \classmethod \constructor(bus, ...)
///
/// Construct a UART object on the given bus.  `bus` can be 1-6, or 'XA', 'XB', 'YA', or 'YB'.
/// With no additional parameters, the UART object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).  If extra arguments are given, the bus is initialised.
/// See `init` for parameters of initialisation.
///
/// The physical pins of the UART busses are:
///
///   - `UART(4)` is on `XA`: `(TX, RX) = (X1, X2) = (PA0, PA1)`
///   - `UART(1)` is on `XB`: `(TX, RX) = (X9, X10) = (PB6, PB7)`
///   - `UART(6)` is on `YA`: `(TX, RX) = (Y1, Y2) = (PC6, PC7)`
///   - `UART(3)` is on `YB`: `(TX, RX) = (Y9, Y10) = (PB10, PB11)`
///   - `UART(2)` is on: `(TX, RX) = (X3, X4) = (PA2, PA3)`
STATIC mp_obj_t pyb_uart_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out port
    int uart_id = 0;
    if (MP_OBJ_IS_STR(args[0])) {
        const char *port = mp_obj_str_get_str(args[0]);
        if (0) {
        #ifdef MICROPY_HW_UART1_NAME
        } else if (strcmp(port, MICROPY_HW_UART1_NAME) == 0) {
            uart_id = PYB_UART_1;
        #endif
        #ifdef MICROPY_HW_UART2_NAME
        } else if (strcmp(port, MICROPY_HW_UART2_NAME) == 0) {
            uart_id = PYB_UART_2;
        #endif
        #ifdef MICROPY_HW_UART3_NAME
        } else if (strcmp(port, MICROPY_HW_UART3_NAME) == 0) {
            uart_id = PYB_UART_3;
        #endif
        #ifdef MICROPY_HW_UART4_NAME
        } else if (strcmp(port, MICROPY_HW_UART4_NAME) == 0) {
            uart_id = PYB_UART_4;
        #endif
        #ifdef MICROPY_HW_UART5_NAME
        } else if (strcmp(port, MICROPY_HW_UART5_NAME) == 0) {
            uart_id = PYB_UART_5;
        #endif
        #ifdef MICROPY_HW_UART6_NAME
        } else if (strcmp(port, MICROPY_HW_UART6_NAME) == 0) {
            uart_id = PYB_UART_6;
        #endif
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%s) does not exist", port));
        }
    } else {
        uart_id = mp_obj_get_int(args[0]);
        if (uart_id < 1 || uart_id > MP_ARRAY_SIZE(MP_STATE_PORT(pyb_uart_obj_all))) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%d) does not exist", uart_id));
        }
    }

    pyb_uart_obj_t *self;
    if (MP_STATE_PORT(pyb_uart_obj_all)[uart_id - 1] == NULL) {
        // create new UART object
        self = m_new0(pyb_uart_obj_t, 1);
        self->base.type = &pyb_uart_type;
        self->uart_id = uart_id;
        MP_STATE_PORT(pyb_uart_obj_all)[uart_id - 1] = self;
    } else {
        // reference existing UART object
        self = MP_STATE_PORT(pyb_uart_obj_all)[uart_id - 1];
    }

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_uart_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return self;
}

STATIC mp_obj_t pyb_uart_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_uart_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_uart_init_obj, 1, pyb_uart_init);

/// \method deinit()
/// Turn off the UART bus.
STATIC mp_obj_t pyb_uart_deinit(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    self->is_enabled = false;
    UART_HandleTypeDef *uart = &self->uart;
    HAL_UART_DeInit(uart);
    if (uart->Instance == USART1) {
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        __USART1_FORCE_RESET();
        __USART1_RELEASE_RESET();
        __USART1_CLK_DISABLE();
    } else if (uart->Instance == USART2) {
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        __USART2_FORCE_RESET();
        __USART2_RELEASE_RESET();
        __USART2_CLK_DISABLE();
    #if defined(USART3)
    } else if (uart->Instance == USART3) {
        HAL_NVIC_DisableIRQ(USART3_IRQn);
        __USART3_FORCE_RESET();
        __USART3_RELEASE_RESET();
        __USART3_CLK_DISABLE();
    #endif
    #if defined(UART4)
    } else if (uart->Instance == UART4) {
        HAL_NVIC_DisableIRQ(UART4_IRQn);
        __UART4_FORCE_RESET();
        __UART4_RELEASE_RESET();
        __UART4_CLK_DISABLE();
    #endif
    #if defined(UART5)
    } else if (uart->Instance == UART5) {
        HAL_NVIC_DisableIRQ(UART5_IRQn);
        __UART5_FORCE_RESET();
        __UART5_RELEASE_RESET();
        __UART5_CLK_DISABLE();
    #endif
    #if defined(UART6)
    } else if (uart->Instance == USART6) {
        HAL_NVIC_DisableIRQ(USART6_IRQn);
        __USART6_FORCE_RESET();
        __USART6_RELEASE_RESET();
        __USART6_CLK_DISABLE();
    #endif
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_deinit_obj, pyb_uart_deinit);

/// \method any()
/// Return `True` if any characters waiting, else `False`.
STATIC mp_obj_t pyb_uart_any(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(uart_rx_any(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_any_obj, pyb_uart_any);

/// \method writechar(char)
/// Write a single character on the bus.  `char` is an integer to write.
/// Return value: `None`.
STATIC mp_obj_t pyb_uart_writechar(mp_obj_t self_in, mp_obj_t char_in) {
    pyb_uart_obj_t *self = self_in;

    // get the character to write (might be 9 bits)
    uint16_t data = mp_obj_get_int(char_in);

    // write the character
    HAL_StatusTypeDef status;
    if (uart_tx_wait(self, self->timeout)) {
        status = uart_tx_data(self, (uint8_t*)&data, 1);
    } else {
        status = HAL_TIMEOUT;
    }

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_uart_writechar_obj, pyb_uart_writechar);

/// \method readchar()
/// Receive a single character on the bus.
/// Return value: The character read, as an integer.  Returns -1 on timeout.
STATIC mp_obj_t pyb_uart_readchar(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    if (uart_rx_wait(self, self->timeout)) {
        return MP_OBJ_NEW_SMALL_INT(uart_rx_char(self));
    } else {
        // return -1 on timeout
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_readchar_obj, pyb_uart_readchar);

// uart.sendbreak()
STATIC mp_obj_t pyb_uart_sendbreak(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    #if defined(MCU_SERIES_F7) || defined(MCU_SERIES_L4)
    self->uart.Instance->RQR = USART_RQR_SBKRQ; // write-only register
    #else
    self->uart.Instance->CR1 |= USART_CR1_SBK;
    #endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_sendbreak_obj, pyb_uart_sendbreak);

STATIC const mp_map_elem_t pyb_uart_locals_dict_table[] = {
    // instance methods

    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_uart_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_uart_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_any), (mp_obj_t)&pyb_uart_any_obj },

    /// \method read([nbytes])
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    /// \method readall()
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall), (mp_obj_t)&mp_stream_readall_obj },
    /// \method readline()
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    /// \method readinto(buf[, nbytes])
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), (mp_obj_t)&mp_stream_readinto_obj },
    /// \method write(buf)
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_writechar), (mp_obj_t)&pyb_uart_writechar_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readchar), (mp_obj_t)&pyb_uart_readchar_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sendbreak), (mp_obj_t)&pyb_uart_sendbreak_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTS), MP_OBJ_NEW_SMALL_INT(UART_HWCONTROL_RTS) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CTS), MP_OBJ_NEW_SMALL_INT(UART_HWCONTROL_CTS) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_uart_locals_dict, pyb_uart_locals_dict_table);

STATIC mp_uint_t pyb_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    byte *buf = buf_in;

    // check that size is a multiple of character width
    if (size & self->char_width) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    // convert byte size to char size
    size >>= self->char_width;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    // wait for first char to become available
    if (!uart_rx_wait(self, self->timeout)) {
        // return EAGAIN error to indicate non-blocking (then read() method returns None)
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // read the data
    byte *orig_buf = buf;
    for (;;) {
        int data = uart_rx_char(self);
        if (self->char_width == CHAR_WIDTH_9BIT) {
            *(uint16_t*)buf = data;
            buf += 2;
        } else {
            *buf++ = data;
        }
        if (--size == 0 || !uart_rx_wait(self, self->timeout_char)) {
            // return number of bytes read
            return buf - orig_buf;
        }
    }
}

STATIC mp_uint_t pyb_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    const byte *buf = buf_in;

    // check that size is a multiple of character width
    if (size & self->char_width) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    // wait to be able to write the first character. EAGAIN causes write to return None
    if (!uart_tx_wait(self, self->timeout)) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // write the data
    HAL_StatusTypeDef status = uart_tx_data(self, (uint8_t*)buf, size >> self->char_width);

    if (status == HAL_OK) {
        // return number of bytes written
        return size;
    } else if (status == HAL_TIMEOUT) { // UART_WaitOnFlagUntilTimeout() disables RXNE interrupt on timeout
        if (self->read_buf_len > 0) {
            __HAL_UART_ENABLE_IT(&self->uart, UART_IT_RXNE); // re-enable RXNE
        }
        // return number of bytes written
        if (self->char_width == CHAR_WIDTH_8BIT) {
            return size - self->uart.TxXferCount - 1;
        } else {
            int written = self->uart.TxXferCount * 2;
            return size - written - 2;
        }
    } else {
        *errcode = mp_hal_status_to_errno_table[status];
        return MP_STREAM_ERROR;
    }
}

STATIC mp_uint_t pyb_uart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_IOCTL_POLL_RD) && uart_rx_any(self)) {
            ret |= MP_IOCTL_POLL_RD;
        }
        if ((flags & MP_IOCTL_POLL_WR) && __HAL_UART_GET_FLAG(&self->uart, UART_FLAG_TXE)) {
            ret |= MP_IOCTL_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = pyb_uart_read,
    .write = pyb_uart_write,
    .ioctl = pyb_uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t pyb_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = pyb_uart_print,
    .make_new = pyb_uart_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_t)&pyb_uart_locals_dict,
};
