#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "usart.h"

struct _pyb_usart_obj_t {
    mp_obj_base_t base;
    pyb_usart_t usart_id;
    bool is_enabled;
    UART_HandleTypeDef handle;
};

pyb_usart_obj_t *pyb_usart_global_debug = NULL;

bool usart_init(pyb_usart_obj_t *usart_obj, uint32_t baudrate) {
    USART_TypeDef *USARTx=NULL;

    uint32_t GPIO_Pin=0;
    uint8_t  GPIO_AF_USARTx=0;
    GPIO_TypeDef* GPIO_Port=NULL;

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

        // USART4 is on PA0/PA1, PC10/PC11
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

    // Initialize USARTx

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_Pin;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = GPIO_AF_USARTx;
    HAL_GPIO_Init(GPIO_Port, &GPIO_InitStructure);

    UART_HandleTypeDef *uh = &usart_obj->handle;
    memset(uh, 0, sizeof(*uh));
    uh->Instance = USARTx;
    uh->Init.BaudRate = baudrate;
    uh->Init.WordLength = USART_WORDLENGTH_8B;
    uh->Init.StopBits = USART_STOPBITS_1;
    uh->Init.Parity = USART_PARITY_NONE;
    uh->Init.Mode = USART_MODE_TX_RX;
    uh->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uh->Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(uh);

    return true;
}

bool usart_rx_any(pyb_usart_obj_t *usart_obj) {
    return __HAL_UART_GET_FLAG(&usart_obj->handle, USART_FLAG_RXNE);
}

int usart_rx_char(pyb_usart_obj_t *usart_obj) {
    uint8_t ch;
    if (HAL_UART_Receive(&usart_obj->handle, &ch, 1, 0) != HAL_OK) {
        ch = 0;
    }
    return ch;
}

void usart_tx_char(pyb_usart_obj_t *usart_obj, int c) {
    uint8_t ch = c;
    HAL_UART_Transmit(&usart_obj->handle, &ch, 1, 100000);
}

void usart_tx_str(pyb_usart_obj_t *usart_obj, const char *str) {
    for (; *str; str++) {
        usart_tx_char(usart_obj, *str);
    }
}

void usart_tx_strn(pyb_usart_obj_t *usart_obj, const char *str, uint len) {
    for (; len > 0; str++, len--) {
        usart_tx_char(usart_obj, *str);
    }
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

STATIC void usart_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_usart_obj_t *self = self_in;
    print(env, "<Usart %lu>", self->usart_id);
}

STATIC mp_obj_t usart_obj_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    if (!(n_args == 2 && n_kw == 0)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Usart accepts 2 arguments"));
    }

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
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Usart port %s does not exist", port));
        }
    } else {
        o->usart_id = mp_obj_get_int(args[0]);
    }

    // init USART (if it fails, it's because the port doesn't exist)
    if (!usart_init(o, mp_obj_get_int(args[1]))) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Usart port %d does not exist", o->usart_id));
    }

    o->is_enabled = true;

    return o;
}

STATIC mp_obj_t usart_obj_status(mp_obj_t self_in) {
    pyb_usart_obj_t *self = self_in;
    if (usart_rx_any(self)) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

STATIC mp_obj_t usart_obj_rx_char(mp_obj_t self_in) {
    mp_obj_t ret = mp_const_none;
    pyb_usart_obj_t *self = self_in;

    if (self->is_enabled) {
        ret =  mp_obj_new_int(usart_rx_char(self));
    }
    return ret;
}

STATIC mp_obj_t usart_obj_tx_char(mp_obj_t self_in, mp_obj_t c) {
    pyb_usart_obj_t *self = self_in;
    uint len;
    const char *str = mp_obj_str_get_data(c, &len);
    if (len == 1 && self->is_enabled) {
        usart_tx_char(self, str[0]);
    }
    return mp_const_none;
}

STATIC mp_obj_t usart_obj_tx_str(mp_obj_t self_in, mp_obj_t s) {
    pyb_usart_obj_t *self = self_in;
    if (self->is_enabled) {
        if (MP_OBJ_IS_STR(s)) {
            uint len;
            const char *data = mp_obj_str_get_data(s, &len);
            usart_tx_strn(self, data, len);
        }
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(usart_obj_status_obj, usart_obj_status);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(usart_obj_rx_char_obj, usart_obj_rx_char);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(usart_obj_tx_char_obj, usart_obj_tx_char);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(usart_obj_tx_str_obj, usart_obj_tx_str);

STATIC const mp_map_elem_t usart_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_status), (mp_obj_t)&usart_obj_status_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv_chr), (mp_obj_t)&usart_obj_rx_char_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_chr), (mp_obj_t)&usart_obj_tx_char_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&usart_obj_tx_str_obj },
};

STATIC MP_DEFINE_CONST_DICT(usart_locals_dict, usart_locals_dict_table);

const mp_obj_type_t pyb_usart_type = {
    { &mp_type_type },
    .name = MP_QSTR_Usart,
    .print = usart_obj_print,
    .make_new = usart_obj_make_new,
    .locals_dict = (mp_obj_t)&usart_locals_dict,
};
