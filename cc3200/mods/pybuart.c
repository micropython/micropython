/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "uart.h"
#include "pybuart.h"
#include "pybioctl.h"
#include "mpexception.h"
#include "py/mpstate.h"
#include "osi.h"

/// \moduleref pyb
/// \class UART - duplex serial communication bus
///
/// UART implements the standard UART/USART duplex serial communications protocol.  At
/// the physical level it consists of 2 lines: RX and TX.
///
/// UART objects can be created and initialised using:
///
///     from pyb import UART
///
///     uart = UART(0, 9600)                         # init with given baudrate
///     uart.init(9600, bits=8, stop=1, parity=None) # init with given parameters
///
/// Bits can be 5, 6, 7, 8, parity can be None, 0 (even), 1 (odd). Stop can be 1 or 2.
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
///     uart.any()          # returns True if any characters waiting

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define PYBUART_TX_WAIT_MS              1
#define PYBUART_TX_MAX_TIMEOUT_MS       5

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC pyb_uart_obj_t* pyb_uart_add (pyb_uart_id_t uart_id);
STATIC pyb_uart_obj_t* pyb_uart_find (pyb_uart_id_t uart_id);
STATIC void UARTGenericIntHandler(uint32_t uart_id);
STATIC void UART0IntHandler(void);
STATIC void UART1IntHandler(void);
STATIC mp_obj_t pyb_uart_deinit(mp_obj_t self_in);

/******************************************************************************
 DEFINE PRIVATE TYPES
 ******************************************************************************/

struct _pyb_uart_obj_t {
    mp_obj_base_t base;
    pyb_uart_id_t uart_id;
    uint reg;
    uint baudrate;
    uint config;
    uint flowcontrol;
    byte *read_buf;                     // read buffer pointer
    uint16_t timeout;                   // timeout waiting for first char
    uint16_t timeout_char;              // timeout waiting between chars
    uint16_t read_buf_len;              // len in chars; buf can hold len-1 chars
    volatile uint16_t read_buf_head;    // indexes first empty slot
    uint16_t read_buf_tail;             // indexes first full slot (not full if equals head)
    bool enabled;
};

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void uart_init0 (void) {
    mp_obj_list_init(&MP_STATE_PORT(pyb_uart_list), 0);
}

// unregister all interrupt sources
void uart_deinit(void) {
    for (int i = PYB_UART_0; i < PYB_NUM_UARTS; i++) {
        pyb_uart_obj_t *self;
        if ((self = pyb_uart_find (i))) {
            pyb_uart_deinit(self);
        }
    }
}

// assumes init parameters have been set up correctly
bool uart_init2(pyb_uart_obj_t *self) {
    uint uartPerh;

    switch (self->uart_id) {
    case PYB_UART_0:
        self->reg = UARTA0_BASE;
        uartPerh = PRCM_UARTA0;
        MAP_UARTIntRegister(UARTA0_BASE, UART0IntHandler);
        MAP_IntPrioritySet(INT_UARTA0, INT_PRIORITY_LVL_3);
        break;
    case PYB_UART_1:
        self->reg = UARTA1_BASE;
        uartPerh = PRCM_UARTA1;
        MAP_UARTIntRegister(UARTA1_BASE, UART1IntHandler);
        MAP_IntPrioritySet(INT_UARTA1, INT_PRIORITY_LVL_3);
        break;
    default:
        return false;
    }

    // Enable the peripheral clock
    MAP_PRCMPeripheralClkEnable(uartPerh, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);

    // Reset the uart
    MAP_PRCMPeripheralReset(uartPerh);

    // Initialize the UART
    MAP_UARTConfigSetExpClk(self->reg, MAP_PRCMPeripheralClockGet(uartPerh),
                            self->baudrate, self->config);

    // Enbale the FIFO
    MAP_UARTFIFOEnable(self->reg);

    // Configure the FIFO interrupt levels
    MAP_UARTFIFOLevelSet(self->reg, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
    
    // Configure the flow control mode
    UARTFlowControlSet(self->reg, self->flowcontrol);

    // Enable the RX and RX timeout interrupts
    MAP_UARTIntEnable(self->reg, UART_INT_RX | UART_INT_RT);

    self->enabled = true;

    return true;
}

bool uart_init(pyb_uart_obj_t *self, uint baudrate) {
    self->baudrate = baudrate;
    self->config = UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE;
    self->flowcontrol = UART_FLOWCONTROL_NONE;
    return uart_init2(self);
}

bool uart_rx_any(pyb_uart_obj_t *self) {
    return (self->read_buf_tail != self->read_buf_head || MAP_UARTCharsAvail(self->reg));
}

// Waits at most timeout milliseconds for at least 1 char to become ready for
// reading (from buf or for direct reading).
// Returns true if something available, false if not.
STATIC bool uart_rx_wait(pyb_uart_obj_t *self, uint32_t timeout) {
    for (;;) {
        if (uart_rx_any(self)) {
            return true; // have at least 1 char ready for reading
        }
        if (timeout > 0) {
            HAL_Delay (1);
            timeout--;
        }
        else {
            return false;
        }
    }
}

int uart_rx_char(pyb_uart_obj_t *self) {
    if (self->read_buf_tail != self->read_buf_head) {
        // buffering via IRQ
        int data = self->read_buf[self->read_buf_tail];
        self->read_buf_tail = (self->read_buf_tail + 1) % self->read_buf_len;
        return data;
    } else {
        // no buffering
        return MAP_UARTCharGetNonBlocking(self->reg);
    }
}

bool uart_tx_char(pyb_uart_obj_t *self, int c) {
    uint32_t timeout = 0;

    while (!MAP_UARTCharPutNonBlocking(self->reg, c)) {
        if (timeout++ > (PYBUART_TX_MAX_TIMEOUT_MS / PYBUART_TX_WAIT_MS)) {
            return false;
        }
        HAL_Delay (PYBUART_TX_WAIT_MS);
    }
    return true;
}

bool uart_tx_strn(pyb_uart_obj_t *self, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (!uart_tx_char(self, *str)) {
            return false;
        }
    }
    return true;
}

void uart_tx_strn_cooked(pyb_uart_obj_t *self, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            uart_tx_char(self, '\r');
        }
        uart_tx_char(self, *str);
    }
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC pyb_uart_obj_t* pyb_uart_add (pyb_uart_id_t uart_id) {
    // create a new uart object
    pyb_uart_obj_t *self = m_new_obj(pyb_uart_obj_t);
    self->base.type = &pyb_uart_type;
    self->uart_id = uart_id;
    self->read_buf = NULL;
    self->enabled = false;
    // add it to the list
    mp_obj_list_append(&MP_STATE_PORT(pyb_uart_list), self);
    return self;
}

STATIC pyb_uart_obj_t* pyb_uart_find (pyb_uart_id_t uart_id) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(pyb_uart_list).len; i++) {
        pyb_uart_obj_t *self = (pyb_uart_obj_t *)MP_STATE_PORT(pyb_uart_list).items[i];
        if (self->uart_id == uart_id) {
            return self;
        }
    }
    return NULL;
}

STATIC void UARTGenericIntHandler(uint32_t uart_id) {
    pyb_uart_obj_t *self;
    uint32_t status;

    if ((self = pyb_uart_find(uart_id))) {
        status = MAP_UARTIntStatus(self->reg, true);
        // receive interrupt
        if (status & (UART_INT_RX | UART_INT_RT)) {
            MAP_UARTIntClear(self->reg, UART_INT_RX | UART_INT_RT);
            while (UARTCharsAvail(self->reg)) {
                int data = MAP_UARTCharGetNonBlocking(self->reg);
                if (MICROPY_STDIO_UART == self->uart_id && data == user_interrupt_char) {
                    // raise exception when interrupts are finished
                    mpexception_keyboard_nlr_jump();
                }
                else if (self->read_buf_len != 0) {
                    uint16_t next_head = (self->read_buf_head + 1) % self->read_buf_len;
                    if (next_head != self->read_buf_tail) {
                        // only store data if room in buf
                        self->read_buf[self->read_buf_head] = data;
                        self->read_buf_head = next_head;
                    }
                }
            }
        }
    }
}

STATIC void UART0IntHandler(void) {
    UARTGenericIntHandler(0);
}

STATIC void UART1IntHandler(void) {
    UARTGenericIntHandler(1);
}

/******************************************************************************/
/* Micro Python bindings                                                      */

STATIC void pyb_uart_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_uart_obj_t *self = self_in;
    if (!self->enabled) {
        print(env, "<UART%u>", self->uart_id);
    } else {
        print(env, "<UART%u baudrate=%u, bits=", self->uart_id, self->baudrate);
        switch (self->config & UART_CONFIG_WLEN_MASK) {
        case UART_CONFIG_WLEN_5:
            print(env, "5");
            break;
        case UART_CONFIG_WLEN_6:
            print(env, "6");
            break;
        case UART_CONFIG_WLEN_7:
            print(env, "7");
            break;
        case UART_CONFIG_WLEN_8:
            print(env, "8");
            break;
        default:
            break;
        }
        if ((self->config & UART_CONFIG_PAR_MASK) == UART_CONFIG_PAR_NONE) {
            print(env, ", parity=None");
        } else {
            print(env, ", parity=%u", (self->config & UART_CONFIG_PAR_MASK) == UART_CONFIG_PAR_EVEN ? 0 : 1);
        }
        print(env, ", stop=%u, timeout=%u, timeout_char=%u, read_buf_len=%u>",
              (self->config & UART_CONFIG_STOP_MASK) == UART_CONFIG_STOP_ONE ? 1 : 2,
               self->timeout, self->timeout_char, self->read_buf_len);
    }
}

/// \method init(baudrate, bits=8, parity=None, stop=1, *, timeout=1000, timeout_char=0, read_buf_len=128)
///
/// Initialise the UART bus with the given parameters:
///
///   - `baudrate` is the clock rate.
///   - `bits` is the number of bits per byte, 7, 8 or 9.
///   - `parity` is the parity, `None`, 0 (even) or 1 (odd).
///   - `stop` is the number of stop bits, 1 or 2.
///   - `flowcontrol` is the flow control mode, `None`, `UART.FLOW_TX`, 
///     `UART.FLOW_RX', 'UART.FLOW_TXRX`.
///   - `timeout` is the timeout in milliseconds to wait for the first character.
///   - `timeout_char` is the timeout in milliseconds to wait between characters.
///   - `read_buf_len` is the character length of the read buffer (0 to disable).
STATIC const mp_arg_t pyb_uart_init_args[] = {
    { MP_QSTR_baudrate,     MP_ARG_REQUIRED | MP_ARG_INT,  },
    { MP_QSTR_bits,         MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 8} },
    { MP_QSTR_parity,       MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_stop,         MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 1} },
    { MP_QSTR_flow,         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_int = UART_FLOWCONTROL_NONE} },
    { MP_QSTR_timeout,      MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 1000} },
    { MP_QSTR_timeout_char, MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 1} },
    { MP_QSTR_read_buf_len, MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 128} },
};

STATIC mp_obj_t pyb_uart_init_helper(pyb_uart_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    bool success;
    
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_uart_init_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(pyb_uart_init_args), pyb_uart_init_args, args);

    // set the UART configuration values
    if (n_args > 1) {
        self->baudrate = args[0].u_int;
        switch (args[1].u_int) {
        case 5:
            self->config = UART_CONFIG_WLEN_5;
            break;
        case 6:
            self->config = UART_CONFIG_WLEN_6;
            break;
        case 7:
            self->config = UART_CONFIG_WLEN_7;
            break;
        case 8:
            self->config = UART_CONFIG_WLEN_8;
            break;
        default:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
            break;
        }
        // Parity
        if (args[2].u_obj == mp_const_none) {
            self->config |= UART_CONFIG_PAR_NONE;
        } else {
            self->config |= ((mp_obj_get_int(args[2].u_obj) & 1) ? UART_CONFIG_PAR_ODD : UART_CONFIG_PAR_EVEN);
        }
        // Stop bits
        self->config |= (args[3].u_int == 1 ? UART_CONFIG_STOP_ONE : UART_CONFIG_STOP_TWO);
        
        // Flow control
        self->flowcontrol = args[4].u_int;
        success = uart_init2(self);        
    } else {
        success = uart_init(self, args[0].u_int);
    }
    
    // init UART (if it fails, something weird happened)
    if (!success) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }

    // set timeouts
    self->timeout = args[5].u_int;
    self->timeout_char = args[6].u_int;

    // setup the read buffer
    m_del(byte, self->read_buf, self->read_buf_len);
    self->read_buf_head = 0;
    self->read_buf_tail = 0;

    if (args[7].u_int <= 0) {
        // no read buffer
        self->read_buf_len = 0;
        self->read_buf = NULL;
        MAP_UARTIntDisable(self->reg, UART_INT_RX | UART_INT_RT);
    } else {
        // read buffer using interrupts
        self->read_buf_len = args[7].u_int;
        self->read_buf = m_new(byte, args[7].u_int);
    }
   
    return mp_const_none;
}

/// \classmethod \constructor(bus, ...)
///
/// Construct a UART object on the given bus id.  `bus id` can be 0-1
/// With no additional parameters, the UART object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).
/// When only the baud rate is given the UART object is created and
/// initialized with the default configuration of: 8 bit transfers,
/// 1 stop bit, no parity and flow control disabled.
/// See `init` for parameters of initialisation.
/// If extra arguments are given, the bus is initialised with these arguments
/// See `init` for parameters of initialisation.
///
STATIC mp_obj_t pyb_uart_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_ARRAY_SIZE(pyb_uart_init_args), true);

    // work out the uart id
    pyb_uart_id_t uart_id = mp_obj_get_int(args[0]);

    if (uart_id < PYB_UART_0 || uart_id > PYB_UART_1) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_resource_not_avaliable));
    }

    // search for an object in the list
    pyb_uart_obj_t *self;
    if (!(self = pyb_uart_find(uart_id))) {
        self = pyb_uart_add(uart_id);
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
    uint uartPerh;

    switch (self->uart_id) {

    case PYB_UART_0:
        uartPerh = PRCM_UARTA0;
        break;

    case PYB_UART_1:
        uartPerh = PRCM_UARTA1;
        break;

    default:
        return mp_const_none;
    }

    self->enabled = false;
    MAP_UARTIntDisable(self->reg, UART_INT_RX | UART_INT_RT);
    MAP_UARTIntClear(self->reg, UART_INT_RX | UART_INT_RT);
    MAP_UARTIntUnregister(self->reg);
    MAP_UARTDisable(self->reg);
    MAP_PRCMPeripheralClkDisable(uartPerh, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_deinit_obj, pyb_uart_deinit);

/// \method delete()
/// Deinits the UART and removes its references so that it can be cleaned by the gc
STATIC mp_obj_t pyb_uart_delete(mp_obj_t self_in) {
    pyb_uart_obj_t *self = self_in;

    // deinit the peripheral
    pyb_uart_deinit(self);
    // remove it from the list
    mp_obj_list_remove(&MP_STATE_PORT(pyb_uart_list), self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_delete_obj, pyb_uart_delete);

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

/// \method writechar(char)
/// Write a single character on the bus.  `char` is an integer to write.
/// Return value: `None`.
STATIC mp_obj_t pyb_uart_writechar(mp_obj_t self_in, mp_obj_t char_in) {
    pyb_uart_obj_t *self = self_in;

    // get the character to write
    uint8_t data = mp_obj_get_int(char_in);

    // send the character
    if (!uart_tx_char(self, data)) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ETIMEDOUT)));
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
        return mp_obj_new_int(uart_rx_char(self));
    } else {
        // return -1 on timeout
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_readchar_obj, pyb_uart_readchar);

STATIC const mp_map_elem_t pyb_uart_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__),     (mp_obj_t)&pyb_uart_delete_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),        (mp_obj_t)&pyb_uart_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),      (mp_obj_t)&pyb_uart_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_any),         (mp_obj_t)&pyb_uart_any_obj },

    /// \method read([nbytes])
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),        (mp_obj_t)&mp_stream_read_obj },
    /// \method readall()
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall),     (mp_obj_t)&mp_stream_readall_obj },
    /// \method readline()
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline),    (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    /// \method readinto(buf[, nbytes])
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto),    (mp_obj_t)&mp_stream_readinto_obj },
    /// \method write(buf)
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),       (mp_obj_t)&mp_stream_write_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_writechar),   (mp_obj_t)&pyb_uart_writechar_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readchar),    (mp_obj_t)&pyb_uart_readchar_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_FLOW_NONE),   MP_OBJ_NEW_SMALL_INT(UART_FLOWCONTROL_NONE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_FLOW_TX),     MP_OBJ_NEW_SMALL_INT(UART_FLOWCONTROL_TX) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_FLOW_RX),     MP_OBJ_NEW_SMALL_INT(UART_FLOWCONTROL_RX) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_FLOW_TXRX),   MP_OBJ_NEW_SMALL_INT(UART_FLOWCONTROL_TX | UART_FLOWCONTROL_RX) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_uart_locals_dict, pyb_uart_locals_dict_table);

STATIC mp_uint_t pyb_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    byte *buf = buf_in;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    // wait for first char to become available
    if (!uart_rx_wait(self, self->timeout)) {
        // we can either return 0 to indicate EOF (then read() method returns b'')
        // or return EAGAIN error to indicate non-blocking (then read() method returns None)
        return 0;
    }

    // read the data
    byte *orig_buf = buf;
    for (;;) {
        *buf++ = uart_rx_char(self);
        if (--size == 0 || !uart_rx_wait(self, self->timeout_char)) {
            // return number of bytes read
            return buf - orig_buf;
        }
    }
}

STATIC mp_uint_t pyb_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    pyb_uart_obj_t *self = self_in;
    const char *buf = buf_in;

    // write the data
    if (!uart_tx_strn(self, buf, size)) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ETIMEDOUT)));
    }
    return size;
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
        if ((flags & MP_IOCTL_POLL_WR) && MAP_UARTSpaceAvail(self->reg)) {
            ret |= MP_IOCTL_POLL_WR;
        }
    } else {
        *errcode = EINVAL;
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
    .stream_p = &uart_stream_p,
    .locals_dict = (mp_obj_t)&pyb_uart_locals_dict,
};
