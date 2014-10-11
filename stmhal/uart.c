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
#include <errno.h>

#include "stm32f4xx_hal.h"

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "bufhelper.h"
#include "uart.h"
#include "pybioctl.h"

/// \moduleref pyb
/// \class UART - duplex serial communication bus
///
/// UART implements the standard UART/USART duplex serial communications protocol.  At
/// the physical level it consists of 2 lines: RX and TX.
///
/// See usage model of I2C.  UART is very similar.  Main difference is
/// parameters to init the UART bus:
///
///     from pyb import UART
///
///     uart = UART(1, 9600)                         # init with given baudrate
///     uart.init(9600, bits=8, stop=1, parity=None) # init with given parameters
///
/// Bits can be 8 or 9, stop can be 1 or 2, parity can be None, 0 (even), 1 (odd).
///
/// Extra method:
///
///     uart.any()               # returns True if any characters waiting

struct _pyb_uart_obj_t {
    mp_obj_base_t base;
    pyb_uart_t uart_id;
    bool is_enabled;
    UART_HandleTypeDef uart;
    IRQn_Type irqn;
    uint16_t recv_buf_len;              // buf can hold len-1 chars
    volatile uint16_t recv_buf_head;    // indexes first empty slot
    uint16_t recv_buf_tail;             // indexes first full slot (not full if equals head)
    byte *recv_buf;
};

STATIC pyb_uart_obj_t *pyb_uart_obj_all[6];

STATIC mp_obj_t pyb_uart_deinit(mp_obj_t self_in);

void uart_init0(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(pyb_uart_obj_all); i++) {
        pyb_uart_obj_all[i] = NULL;
    }
}

// unregister all interrupt sources
void uart_deinit(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(pyb_uart_obj_all); i++) {
        pyb_uart_obj_t *uart_obj = pyb_uart_obj_all[i];
        if (uart_obj != NULL) {
            pyb_uart_deinit(uart_obj);
        }
    }
}

// assumes Init parameters have been set up correctly
bool uart_init2(pyb_uart_obj_t *uart_obj) {
    USART_TypeDef *UARTx;
    IRQn_Type irqn;
    uint32_t GPIO_Pin;
    uint8_t GPIO_AF_UARTx = 0;
    GPIO_TypeDef* GPIO_Port = NULL;

    switch (uart_obj->uart_id) {
        // USART1 is on PA9/PA10 (CK on PA8), PB6/PB7
        case PYB_UART_1:
            UARTx = USART1;
            irqn = USART1_IRQn;
            GPIO_AF_UARTx = GPIO_AF7_USART1;

#if defined (PYBV4) || defined(PYBV10)
            GPIO_Port = GPIOB;
            GPIO_Pin = GPIO_PIN_6 | GPIO_PIN_7;
#else
            GPIO_Port = GPIOA;
            GPIO_Pin = GPIO_PIN_9 | GPIO_PIN_10;
#endif

            __USART1_CLK_ENABLE();
            break;

        // USART2 is on PA2/PA3 (CK on PA4), PD5/PD6 (CK on PD7)
        case PYB_UART_2:
            UARTx = USART2;
            irqn = USART2_IRQn;
            GPIO_AF_UARTx = GPIO_AF7_USART2;

            GPIO_Port = GPIOA;
            GPIO_Pin = GPIO_PIN_2 | GPIO_PIN_3;

            __USART2_CLK_ENABLE();
            break;

        // USART3 is on PB10/PB11 (CK on PB12), PC10/PC11 (CK on PC12), PD8/PD9 (CK on PD10)
        case PYB_UART_3:
            UARTx = USART3;
            irqn = USART3_IRQn;
            GPIO_AF_UARTx = GPIO_AF7_USART3;

#if defined(PYBV3) || defined(PYBV4) | defined(PYBV10)
            GPIO_Port = GPIOB;
            GPIO_Pin = GPIO_PIN_10 | GPIO_PIN_11;
#else
            GPIO_Port = GPIOD;
            GPIO_Pin = GPIO_PIN_8 | GPIO_PIN_9;
#endif
            __USART3_CLK_ENABLE();
            break;

        // UART4 is on PA0/PA1, PC10/PC11
        case PYB_UART_4:
            UARTx = UART4;
            irqn = UART4_IRQn;
            GPIO_AF_UARTx = GPIO_AF8_UART4;

            GPIO_Port = GPIOA;
            GPIO_Pin = GPIO_PIN_0 | GPIO_PIN_1;

            __UART4_CLK_ENABLE();
            break;

        // USART6 is on PC6/PC7 (CK on PC8)
        case PYB_UART_6:
            UARTx = USART6;
            irqn = USART6_IRQn;
            GPIO_AF_UARTx = GPIO_AF8_USART6;

            GPIO_Port = GPIOC;
            GPIO_Pin = GPIO_PIN_6 | GPIO_PIN_7;

            __USART6_CLK_ENABLE();
            break;

        default:
            return false;
    }

    uart_obj->irqn = irqn;
    uart_obj->uart.Instance = UARTx;

    // init GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_Pin;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = GPIO_AF_UARTx;
    HAL_GPIO_Init(GPIO_Port, &GPIO_InitStructure);

    // init UARTx
    HAL_UART_Init(&uart_obj->uart);

    uart_obj->is_enabled = true;

    return true;
}

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

bool uart_rx_any(pyb_uart_obj_t *self) {
    return self->recv_buf_tail != self->recv_buf_head
        || __HAL_UART_GET_FLAG(&self->uart, UART_FLAG_RXNE) != RESET;
}

// Waits at most timeout milliseconds for at least 1 char to become ready for
// reading (from buf or for direct reading).
// Returns true if something available, false if not.
STATIC bool uart_rx_wait(pyb_uart_obj_t *self, uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    for (;;) {
        if (self->recv_buf_tail != self->recv_buf_head || __HAL_UART_GET_FLAG(&self->uart, UART_FLAG_RXNE) != RESET) {
            return true; // have at least 1 char ready for reading
        }
        if (HAL_GetTick() - start >= timeout) {
            return false; // timeout
        }
        __WFI();
    }
}

// assumes there is a character available
// TODO assumes transmission size is 8-bits wide
int uart_rx_char(pyb_uart_obj_t *self) {
    if (self->recv_buf_tail != self->recv_buf_head) {
        // buffering via IRQ
        int data = self->recv_buf[self->recv_buf_tail];
        self->recv_buf_tail = (self->recv_buf_tail + 1) % self->recv_buf_len;
        return data;
    } else {
        // no buffering
        return self->uart.Instance->DR;
    }
}

void uart_tx_char(pyb_uart_obj_t *uart_obj, int c) {
    uint8_t ch = c;
    HAL_UART_Transmit(&uart_obj->uart, &ch, 1, 100000);
}

void uart_tx_strn(pyb_uart_obj_t *uart_obj, const char *str, uint len) {
    HAL_UART_Transmit(&uart_obj->uart, (uint8_t*)str, len, 100000);
}

void uart_tx_strn_cooked(pyb_uart_obj_t *uart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            uart_tx_char(uart_obj, '\r');
        }
        uart_tx_char(uart_obj, *str);
    }
}

// this IRQ handler is set up to receive RXNE interrupts only
void uart_irq_handler(mp_uint_t uart_id) {
    // get the uart object
    pyb_uart_obj_t *self = pyb_uart_obj_all[uart_id - 1];

    if (self == NULL) {
        // UART object has not been set, so we can't do anything, not
        // even disable the IRQ.  This should never happen.
        return;
    }

    if (__HAL_UART_GET_FLAG(&self->uart, UART_FLAG_RXNE) != RESET) {
        uint8_t data = self->uart.Instance->DR; // clears UART_FLAG_RXNE
        if (self->recv_buf_len != 0) {
            uint16_t next_head = (self->recv_buf_head + 1) % self->recv_buf_len;
            if (next_head != self->recv_buf_tail) {
                // only store data if room in buf
                self->recv_buf[self->recv_buf_head] = data;
                self->recv_buf_head = next_head;
            }
        }
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

STATIC void pyb_uart_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_uart_obj_t *self = self_in;
    if (!self->is_enabled) {
        print(env, "UART(%u)", self->uart_id);
    } else {
        print(env, "UART(%u, baudrate=%u, bits=%u, stop=%u",
            self->uart_id, self->uart.Init.BaudRate,
            self->uart.Init.WordLength == UART_WORDLENGTH_8B ? 8 : 9,
            self->uart.Init.StopBits == UART_STOPBITS_1 ? 1 : 2);
        if (self->uart.Init.Parity == UART_PARITY_NONE) {
            print(env, ", parity=None");
        } else {
            print(env, ", parity=%u", self->uart.Init.Parity == UART_PARITY_EVEN ? 0 : 1);
        }
        print(env, ", recv_buf_len=%d)", self->recv_buf_len);
    }
}

/// \method init(baudrate, *, bits=8, stop=1, parity=None, recv_buf_len=64)
///
/// Initialise the UART bus with the given parameters:
///
///   - `baudrate` is the clock rate.
///   - `bits` is the number of bits per byte, 8 or 9.
///   - `stop` is the number of stop bits, 1 or 2.
///   - `parity` is the parity, `None`, 0 (even) or 1 (odd).
///   - `recv_buf_len` is the character length of the receive buffer (0 to disable).
STATIC mp_obj_t pyb_uart_init_helper(pyb_uart_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 8} },
        { MP_QSTR_stop,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 1} },
        { MP_QSTR_parity,   MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_recv_buf_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // set the UART configuration values
    memset(&self->uart, 0, sizeof(self->uart));
    UART_InitTypeDef *init = &self->uart.Init;
    init->BaudRate = args[0].u_int;
    init->WordLength = args[1].u_int == 8 ? UART_WORDLENGTH_8B : UART_WORDLENGTH_9B;
    switch (args[2].u_int) {
        case 1: init->StopBits = UART_STOPBITS_1; break;
        default: init->StopBits = UART_STOPBITS_2; break;
    }
    if (args[3].u_obj == mp_const_none) {
        init->Parity = UART_PARITY_NONE;
    } else {
        mp_int_t parity = mp_obj_get_int(args[3].u_obj);
        init->Parity = (parity & 1) ? UART_PARITY_ODD : UART_PARITY_EVEN;
    }
    init->Mode = UART_MODE_TX_RX;
    init->HwFlowCtl = UART_HWCONTROL_NONE;
    init->OverSampling = UART_OVERSAMPLING_16;

    // init UART (if it fails, it's because the port doesn't exist)
    if (!uart_init2(self)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%d) does not exist", self->uart_id));
    }

    // setup the receive buffer
    m_del(byte, self->recv_buf, self->recv_buf_len);
    self->recv_buf_head = 0;
    self->recv_buf_tail = 0;
    if (args[4].u_int <= 0) {
        // no receive buffer
        self->recv_buf_len = 0;
        self->recv_buf = NULL;
        HAL_NVIC_DisableIRQ(self->irqn);
        __HAL_UART_DISABLE_IT(&self->uart, UART_IT_RXNE);
    } else {
        // receive buffer using interrupts
        self->recv_buf_len = args[4].u_int;
        self->recv_buf = m_new(byte, args[4].u_int);
        __HAL_UART_ENABLE_IT(&self->uart, UART_IT_RXNE);
        HAL_NVIC_SetPriority(self->irqn, 0xe, 0xe); // next-to lowest priority
        HAL_NVIC_EnableIRQ(self->irqn);
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
STATIC mp_obj_t pyb_uart_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out port
    int uart_id = 0;
    if (MP_OBJ_IS_STR(args[0])) {
        const char *port = mp_obj_str_get_str(args[0]);
        if (0) {
#if defined(PYBV10)
        } else if (strcmp(port, "XA") == 0) {
            uart_id = PYB_UART_XA;
        } else if (strcmp(port, "XB") == 0) {
            uart_id = PYB_UART_XB;
        } else if (strcmp(port, "YA") == 0) {
            uart_id = PYB_UART_YA;
        } else if (strcmp(port, "YB") == 0) {
            uart_id = PYB_UART_YB;
#endif
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%s) does not exist", port));
        }
    } else {
        uart_id = mp_obj_get_int(args[0]);
        if (uart_id < 1 || uart_id > MP_ARRAY_SIZE(pyb_uart_obj_all)) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "UART(%d) does not exist", uart_id));
        }
    }

    pyb_uart_obj_t *self;
    if (pyb_uart_obj_all[uart_id - 1] == NULL) {
        // create new UART object
        self = m_new_obj(pyb_uart_obj_t);
        self->base.type = &pyb_uart_type;
        self->uart_id = uart_id;
        self->is_enabled = false;
        self->recv_buf_len = 0;
        self->recv_buf_head = 0;
        self->recv_buf_tail = 0;
        self->recv_buf = NULL;
        pyb_uart_obj_all[uart_id - 1] = self;
    } else {
        // reference existing UART object
        self = pyb_uart_obj_all[uart_id - 1];
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
    } else if (uart->Instance == USART3) {
        HAL_NVIC_DisableIRQ(USART3_IRQn);
        __USART3_FORCE_RESET();
        __USART3_RELEASE_RESET();
        __USART3_CLK_DISABLE();
    } else if (uart->Instance == UART4) {
        HAL_NVIC_DisableIRQ(UART4_IRQn);
        __UART4_FORCE_RESET();
        __UART4_RELEASE_RESET();
        __UART4_CLK_DISABLE();
    } else if (uart->Instance == USART6) {
        HAL_NVIC_DisableIRQ(USART6_IRQn);
        __USART6_FORCE_RESET();
        __USART6_RELEASE_RESET();
        __USART6_CLK_DISABLE();
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_deinit_obj, pyb_uart_deinit);

/// \method any()
/// Return `True` if any characters waiting, else `False`.
STATIC mp_obj_t pyb_uart_any(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;
    if (uart_rx_any(self)) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_any_obj, pyb_uart_any);

/// \method send_chr(send, *, timeout=5000)
/// Send data on the bus:
///
///   - `send` is an integer to send.
///   - `timeout` is the timeout in milliseconds to wait for the send.
///
/// Return value: `None`.
STATIC mp_obj_t pyb_uart_send_chr(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    pyb_uart_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to send from
    uint8_t data = args[0].u_int;

    // send the data
    HAL_StatusTypeDef status = HAL_UART_Transmit(&self->uart, &data, 1, args[1].u_int);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_UART_Transmit failed with code %d", status));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_uart_send_chr_obj, 1, pyb_uart_send_chr);

/// \method send(buf, *, timeout=5000)
/// Send data on the bus:
///
///   - `buf` is the data to send (a buffer object).
///   - `timeout` is the timeout in milliseconds to wait for the send.
///
/// Return value: `None`.
STATIC mp_obj_t pyb_uart_send(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    pyb_uart_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0].u_obj, &bufinfo, MP_BUFFER_READ);

    // send the data
    HAL_StatusTypeDef status = HAL_UART_Transmit(&self->uart, bufinfo.buf, bufinfo.len, args[1].u_int);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_UART_Transmit failed with code %d", status));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_uart_send_obj, 1, pyb_uart_send);

/// \method recv_chr(*, timeout=5000)
///
/// Receive a single character on the bus:
///
///   - `timeout` is the timeout in milliseconds to wait for the receive.
///
/// Return value: The character received, as an integer.  Returns -1 on timeout.
STATIC mp_obj_t pyb_uart_recv_chr(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    pyb_uart_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // receive the data
    if (uart_rx_wait(self, args[0].u_int)) {
        return MP_OBJ_NEW_SMALL_INT(uart_rx_char(self));
    } else {
        // return -1 on timeout
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_uart_recv_chr_obj, 1, pyb_uart_recv_chr);

/// \method recv(len, *, timeout=5000, timeout_chr=5)
///
/// Receive data on the bus:
///
///   - `len` is the maximum number of characters to receive.
///   - `timeout` is the timeout in milliseconds to wait for the first character.
///   - `timeout_chr` is the timeout in milliseconds to wait between characters.
///
/// Return value: a bytearray of the characters received.
STATIC mp_obj_t pyb_uart_recv(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_len, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
        { MP_QSTR_timeout_chr, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5} },
    };

    pyb_uart_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_int_t len = args[0].u_int;

    // make sure we want at least 1 char, and wait for it to become available
    if (len <= 0 || !uart_rx_wait(self, args[1].u_int)) {
        return mp_const_empty_bytes;
    }

    // allocate a bytes object to hold the result
    byte *buf;
    mp_obj_t buf_obj = mp_obj_str_builder_start(&mp_type_bytes, len, &buf);

    // receive the data
    byte *orig_buf = buf;
    for (;;) {
        *buf++ = uart_rx_char(self);
        if (len-- == 0) {
            // return the received chars
            return mp_obj_str_builder_end(buf_obj);
        }
        if (!uart_rx_wait(self, args[2].u_int)) {
            // timeout, so truncate the bytes object
            return mp_obj_str_builder_end_with_len(buf_obj, buf - orig_buf);
        }
    }

}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_uart_recv_obj, 1, pyb_uart_recv);

/// \method recv_into(buf, len=-1, *, timeout=5000, timeout_chr=5)
///
/// Receive data on the bus:
///
///   - `buf` is a mutable buffer which will be filled with received characters.
///   - `len` is the maximum number of characters to receive; if negative, uses len(buf).
///   - `timeout` is the timeout in milliseconds to wait for the first character.
///   - `timeout_chr` is the timeout in milliseconds to wait between characters.
///
/// Return value: number of characters stored in buf.
STATIC mp_obj_t pyb_uart_recv_into(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_len, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
        { MP_QSTR_timeout_chr, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5} },
    };

    pyb_uart_obj_t *self = pos_args[0];

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to receive into
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0].u_obj, &bufinfo, MP_BUFFER_WRITE);

    // get the length
    if (args[1].u_int >= 0 && args[1].u_int < bufinfo.len) {
        bufinfo.len = args[1].u_int;
    }

    // make sure we want at least 1 char, and wait for it to become available
    if (bufinfo.len == 0 || !uart_rx_wait(self, args[2].u_int)) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    // receive the chars
    byte *buf = bufinfo.buf;
    for (;;) {
        *buf++ = uart_rx_char(self);
        if (bufinfo.len-- == 0 || !uart_rx_wait(self, args[3].u_int)) {
            // return the number of chars received
            return mp_obj_new_int(buf - (byte*)bufinfo.buf);
        }
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_uart_recv_into_obj, 1, pyb_uart_recv_into);

STATIC const mp_map_elem_t pyb_uart_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_uart_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_uart_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_any), (mp_obj_t)&pyb_uart_any_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_chr), (mp_obj_t)&pyb_uart_send_chr_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&pyb_uart_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv_chr), (mp_obj_t)&pyb_uart_recv_chr_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&pyb_uart_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv_into), (mp_obj_t)&pyb_uart_recv_into_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_uart_locals_dict, pyb_uart_locals_dict_table);

mp_uint_t uart_ioctl(mp_obj_t self_in, mp_uint_t request, int *errcode, ...) {
    pyb_uart_obj_t *self = self_in;
    va_list vargs;
    va_start(vargs, errcode);
    mp_uint_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = va_arg(vargs, mp_uint_t);
        ret = 0;
        if ((flags & MP_IOCTL_POLL_RD) && uart_rx_any(self)) {
            ret |= MP_IOCTL_POLL_RD;
        }
        if ((flags & MP_IOCTL_POLL_WR) && __HAL_UART_GET_FLAG(&self->uart, UART_FLAG_TXE)) {
            ret |= MP_IOCTL_POLL_WR;
        }
    } else {
        *errcode = EINVAL;
        ret = MP_STREAM_ERROR;
    }
    va_end(vargs);
    return ret;
}

STATIC const mp_stream_p_t uart_stream_p = {
    //.read = uart_read, // TODO
    //.write = uart_write, // TODO
    .ioctl = uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t pyb_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = pyb_uart_print,
    .make_new = pyb_uart_make_new,
    .stream_p = &uart_stream_p,
    .locals_dict = (mp_obj_t)&pyb_uart_locals_dict,
};
