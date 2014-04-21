#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "bufhelper.h"
#include "usart.h"

// Usage model:
//
// See usage model of I2C in i2c.c.  USART is very similar.  Main difference is
// parameters to init the USART bus:
//
//     from pyb import USART
//     usart = USART(1, 9600)                        # init with given baudrate
//     usart.init(9600, bits=8, stop=1, parity=None) # init with given parameters
//
// Bits can be 8 or 9, stop can be 1 or 2, parity can be None, 0 (even), 1 (odd).
//
// Extra method:
//
//     usart.any()              # returns True if any characters waiting

struct _pyb_usart_obj_t {
    mp_obj_base_t base;
    pyb_usart_t usart_id;
    bool is_enabled;
    UART_HandleTypeDef uart;
};

pyb_usart_obj_t *pyb_usart_global_debug = NULL;

// assumes Init parameters have been set up correctly
bool usart_init2(pyb_usart_obj_t *usart_obj) {
    USART_TypeDef *USARTx = NULL;

    uint32_t GPIO_Pin = 0;
    uint8_t  GPIO_AF_USARTx = 0;
    GPIO_TypeDef* GPIO_Port = NULL;

    switch (usart_obj->usart_id) {
        // USART1 is on PA9/PA10, PB6/PB7
        case PYB_USART_1:
            USARTx = USART1;
            GPIO_AF_USARTx = GPIO_AF7_USART1;

#if defined(PYBV10)
            GPIO_Port = GPIOB;
            GPIO_Pin = GPIO_PIN_6 | GPIO_PIN_7;
#else
            GPIO_Port = GPIOA;
            GPIO_Pin = GPIO_PIN_9 | GPIO_PIN_10;
#endif

            __USART1_CLK_ENABLE();
            break;

#if !defined(PYBV10)
        // USART2 is on PA2/PA3, PD5/PD6
        case PYB_USART_2:
            USARTx = USART2;
            GPIO_AF_USARTx = GPIO_AF7_USART2;

            GPIO_Port = GPIOD;
            GPIO_Pin = GPIO_PIN_5 | GPIO_PIN_6;

            __USART2_CLK_ENABLE();
            break;
#endif

        // USART3 is on PB10/PB11, PC10/PC11, PD8/PD9
        case PYB_USART_3:
            USARTx = USART3;
            GPIO_AF_USARTx = GPIO_AF7_USART3;

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
        case PYB_USART_4:
            USARTx = UART4;
            GPIO_AF_USARTx = GPIO_AF8_UART4;

            GPIO_Port = GPIOA;
            GPIO_Pin = GPIO_PIN_0 | GPIO_PIN_1;

            __UART4_CLK_ENABLE();
            break;

        // USART6 is on PC6/PC7
        case PYB_USART_6:
            USARTx = USART6;
            GPIO_AF_USARTx = GPIO_AF8_USART6;

            GPIO_Port = GPIOC;
            GPIO_Pin = GPIO_PIN_6 | GPIO_PIN_7;

            __USART6_CLK_ENABLE();
            break;

        default:
            return false;
    }

    // init GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_Pin;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = GPIO_AF_USARTx;
    HAL_GPIO_Init(GPIO_Port, &GPIO_InitStructure);

    // init USARTx
    usart_obj->uart.Instance = USARTx;
    HAL_UART_Init(&usart_obj->uart);

    usart_obj->is_enabled = true;

    return true;
}

bool usart_init(pyb_usart_obj_t *usart_obj, uint32_t baudrate) {
    UART_HandleTypeDef *uh = &usart_obj->uart;
    memset(uh, 0, sizeof(*uh));
    uh->Init.BaudRate = baudrate;
    uh->Init.WordLength = UART_WORDLENGTH_8B;
    uh->Init.StopBits = UART_STOPBITS_1;
    uh->Init.Parity = UART_PARITY_NONE;
    uh->Init.Mode = UART_MODE_TX_RX;
    uh->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uh->Init.OverSampling = UART_OVERSAMPLING_16;
    return usart_init2(usart_obj);
}

void usart_deinit(pyb_usart_obj_t *usart_obj) {
    usart_obj->is_enabled = false;
    UART_HandleTypeDef *uart = &usart_obj->uart;
    HAL_UART_DeInit(uart);
    if (uart->Instance == USART1) {
        __USART1_FORCE_RESET();
        __USART1_RELEASE_RESET();
        __USART1_CLK_DISABLE();
    } else if (uart->Instance == USART2) {
        __USART2_FORCE_RESET();
        __USART2_RELEASE_RESET();
        __USART2_CLK_DISABLE();
    } else if (uart->Instance == USART3) {
        __USART3_FORCE_RESET();
        __USART3_RELEASE_RESET();
        __USART3_CLK_DISABLE();
    } else if (uart->Instance == UART4) {
        __UART4_FORCE_RESET();
        __UART4_RELEASE_RESET();
        __UART4_CLK_DISABLE();
    } else if (uart->Instance == USART6) {
        __USART6_FORCE_RESET();
        __USART6_RELEASE_RESET();
        __USART6_CLK_DISABLE();
    }
}

bool usart_rx_any(pyb_usart_obj_t *usart_obj) {
    return __HAL_UART_GET_FLAG(&usart_obj->uart, UART_FLAG_RXNE);
}

int usart_rx_char(pyb_usart_obj_t *usart_obj) {
    uint8_t ch;
    if (HAL_UART_Receive(&usart_obj->uart, &ch, 1, 0) != HAL_OK) {
        ch = 0;
    }
    return ch;
}

void usart_tx_char(pyb_usart_obj_t *usart_obj, int c) {
    uint8_t ch = c;
    HAL_UART_Transmit(&usart_obj->uart, &ch, 1, 100000);
}

void usart_tx_str(pyb_usart_obj_t *usart_obj, const char *str) {
    HAL_UART_Transmit(&usart_obj->uart, (uint8_t*)str, strlen(str), 100000);
}

void usart_tx_strn(pyb_usart_obj_t *usart_obj, const char *str, uint len) {
    HAL_UART_Transmit(&usart_obj->uart, (uint8_t*)str, len, 100000);
}

void usart_tx_strn_cooked(pyb_usart_obj_t *usart_obj, const char *str, uint len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            usart_tx_char(usart_obj, '\r');
        }
        usart_tx_char(usart_obj, *str);
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

STATIC void pyb_usart_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_usart_obj_t *self = self_in;
    if (!self->is_enabled) {
        print(env, "USART(%lu)", self->usart_id);
    } else {
        print(env, "USART(%lu, baudrate=%u, bits=%u, stop=%u",
            self->usart_id, self->uart.Init.BaudRate,
            self->uart.Init.WordLength == UART_WORDLENGTH_8B ? 8 : 9,
            self->uart.Init.StopBits == UART_STOPBITS_1 ? 1 : 2);
        if (self->uart.Init.Parity == UART_PARITY_NONE) {
            print(env, ", parity=None)");
        } else {
            print(env, ", parity=%u)", self->uart.Init.Parity == UART_PARITY_EVEN ? 0 : 1);
        }
    }
}

STATIC const mp_arg_parse_t pyb_usart_init_accepted_args[] = {
    { MP_QSTR_baudrate, MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_INT, {.u_int = 9600} },
    { MP_QSTR_bits,     MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT,  {.u_int = 8} },
    { MP_QSTR_stop,     MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT,  {.u_int = 1} },
    { MP_QSTR_parity,   MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_OBJ,  {.u_obj = mp_const_none} },
};
#define PYB_USART_INIT_NUM_ARGS (sizeof(pyb_usart_init_accepted_args) / sizeof(pyb_usart_init_accepted_args[0]))

STATIC mp_obj_t pyb_usart_init_helper(pyb_usart_obj_t *self, uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_parse_val_t vals[PYB_USART_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_USART_INIT_NUM_ARGS, pyb_usart_init_accepted_args, vals);

    // set the USART configuration values
    memset(&self->uart, 0, sizeof(self->uart));
    UART_InitTypeDef *init = &self->uart.Init;
    init->BaudRate = vals[0].u_int;
    init->WordLength = vals[1].u_int == 8 ? UART_WORDLENGTH_8B : UART_WORDLENGTH_9B;
    switch (vals[2].u_int) {
        case 1: init->StopBits = UART_STOPBITS_1; break;
        default: init->StopBits = UART_STOPBITS_2; break;
    }
    if (vals[3].u_obj == mp_const_none) {
        init->Parity = UART_PARITY_NONE;
    } else {
        machine_int_t parity = mp_obj_get_int(vals[3].u_obj);
        init->Parity = (parity & 1) ? UART_PARITY_ODD : UART_PARITY_EVEN;
    }
    init->Mode = UART_MODE_TX_RX;
    init->HwFlowCtl = UART_HWCONTROL_NONE;
    init->OverSampling = UART_OVERSAMPLING_16;

    // init USART (if it fails, it's because the port doesn't exist)
    if (!usart_init2(self)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "USART port %d does not exist", self->usart_id));
    }

    return mp_const_none;
}

STATIC mp_obj_t pyb_usart_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // create object
    pyb_usart_obj_t *o = m_new_obj(pyb_usart_obj_t);
    o->base.type = &pyb_usart_type;

    // work out port
    o->usart_id = 0;
    if (MP_OBJ_IS_STR(args[0])) {
        const char *port = mp_obj_str_get_str(args[0]);
        if (0) {
#if defined(PYBV10)
        } else if (strcmp(port, "XA") == 0) {
            o->usart_id = PYB_USART_XA;
        } else if (strcmp(port, "XB") == 0) {
            o->usart_id = PYB_USART_XB;
        } else if (strcmp(port, "YA") == 0) {
            o->usart_id = PYB_USART_YA;
        } else if (strcmp(port, "YB") == 0) {
            o->usart_id = PYB_USART_YB;
#endif
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "USART port %s does not exist", port));
        }
    } else {
        o->usart_id = mp_obj_get_int(args[0]);
    }

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_usart_init_helper(o, n_args - 1, args + 1, &kw_args);
    }

    return o;
}

STATIC mp_obj_t pyb_usart_init(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_usart_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usart_init_obj, 1, pyb_usart_init);

STATIC mp_obj_t pyb_usart_deinit(mp_obj_t self_in) {
    pyb_usart_obj_t *self = self_in;
    usart_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usart_deinit_obj, pyb_usart_deinit);

STATIC mp_obj_t pyb_usart_any(mp_obj_t self_in) {
    pyb_usart_obj_t *self = self_in;
    if (usart_rx_any(self)) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usart_any_obj, pyb_usart_any);

STATIC const mp_arg_parse_t pyb_usart_send_accepted_args[] = {
    { MP_QSTR_send,    MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_timeout, MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT, {.u_int = 5000} },
};
#define PYB_USART_SEND_NUM_ARGS (sizeof(pyb_usart_send_accepted_args) / sizeof(pyb_usart_send_accepted_args[0]))

STATIC mp_obj_t pyb_usart_send(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    pyb_usart_obj_t *self = args[0];

    // parse args
    mp_arg_parse_val_t vals[PYB_USART_SEND_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_USART_SEND_NUM_ARGS, pyb_usart_send_accepted_args, vals);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);

    // send the data
    HAL_StatusTypeDef status = HAL_UART_Transmit(&self->uart, bufinfo.buf, bufinfo.len, vals[1].u_int);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_UART_Transmit failed with code %d", status));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usart_send_obj, 1, pyb_usart_send);

STATIC const mp_arg_parse_t pyb_usart_recv_accepted_args[] = {
    { MP_QSTR_recv,    MP_ARG_PARSE_REQUIRED | MP_ARG_PARSE_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_timeout, MP_ARG_PARSE_KW_ONLY | MP_ARG_PARSE_INT, {.u_int = 5000} },
};
#define PYB_USART_RECV_NUM_ARGS (sizeof(pyb_usart_recv_accepted_args) / sizeof(pyb_usart_recv_accepted_args[0]))

STATIC mp_obj_t pyb_usart_recv(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    pyb_usart_obj_t *self = args[0];

    // parse args
    mp_arg_parse_val_t vals[PYB_USART_RECV_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_USART_RECV_NUM_ARGS, pyb_usart_recv_accepted_args, vals);

    // get the buffer to receive into
    mp_buffer_info_t bufinfo;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &bufinfo);

    // receive the data
    HAL_StatusTypeDef status = HAL_UART_Receive(&self->uart, bufinfo.buf, bufinfo.len, vals[1].u_int);

    if (status != HAL_OK) {
        // TODO really need a HardwareError object, or something
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, "HAL_UART_Receive failed with code %d", status));
    }

    // return the received data
    if (o_ret == MP_OBJ_NULL) {
        return vals[0].u_obj;
    } else {
        return mp_obj_str_builder_end(o_ret);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usart_recv_obj, 1, pyb_usart_recv);

STATIC const mp_map_elem_t pyb_usart_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_usart_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_usart_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_any), (mp_obj_t)&pyb_usart_any_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&pyb_usart_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&pyb_usart_recv_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_usart_locals_dict, pyb_usart_locals_dict_table);

const mp_obj_type_t pyb_usart_type = {
    { &mp_type_type },
    .name = MP_QSTR_USART,
    .print = pyb_usart_print,
    .make_new = pyb_usart_make_new,
    .locals_dict = (mp_obj_t)&pyb_usart_locals_dict,
};
