#include <stdio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "usart.h"

pyb_usart_t pyb_usart_global_debug = PYB_USART_NONE;

static USART_TypeDef *usart_get_base(pyb_usart_t usart_id) {
    USART_TypeDef *USARTx=NULL;

    switch (usart_id) {
        case PYB_USART_NONE:
            break;
        case PYB_USART_1:
            USARTx = USART1;
            break;
        case PYB_USART_2:
            USARTx = USART2;
            break;
        case PYB_USART_3:
            USARTx = USART3;
            break;
        case PYB_USART_6:
            USARTx = USART6;
            break;
    }

    return USARTx;
}

void usart_init(pyb_usart_t usart_id, uint32_t baudrate) {
    USART_TypeDef *USARTx=NULL;

    uint32_t GPIO_Pin=0;
    uint8_t  GPIO_AF_USARTx=0;
    GPIO_TypeDef* GPIO_Port=NULL;
    uint16_t GPIO_PinSource_TX=0;
    uint16_t GPIO_PinSource_RX=0;

    uint32_t RCC_APBxPeriph=0;
    void (*RCC_APBxPeriphClockCmd)(uint32_t, FunctionalState)=NULL;

    switch (usart_id) {
        case PYB_USART_NONE:
            return;

        case PYB_USART_1:
            USARTx = USART1;

            GPIO_Port = GPIOA;
            GPIO_AF_USARTx = GPIO_AF_USART1;
            GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
            GPIO_PinSource_TX = GPIO_PinSource9;
            GPIO_PinSource_RX = GPIO_PinSource10;

            RCC_APBxPeriph = RCC_APB2Periph_USART1;
            RCC_APBxPeriphClockCmd =RCC_APB2PeriphClockCmd;
            break;
        case PYB_USART_2:
            USARTx = USART2;

            GPIO_Port = GPIOD;
            GPIO_AF_USARTx = GPIO_AF_USART2;
            GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
            GPIO_PinSource_TX = GPIO_PinSource5;
            GPIO_PinSource_RX = GPIO_PinSource6;

            RCC_APBxPeriph = RCC_APB1Periph_USART2;
            RCC_APBxPeriphClockCmd =RCC_APB1PeriphClockCmd;
            break;
        case PYB_USART_3:
            USARTx = USART3;

#if defined(PYBOARD3) || defined(PYBOARD4)
            GPIO_Port = GPIOB;
            GPIO_AF_USARTx = GPIO_AF_USART3;
            GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
            GPIO_PinSource_TX = GPIO_PinSource10;
            GPIO_PinSource_RX = GPIO_PinSource11;
#else
            GPIO_Port = GPIOD;
            GPIO_AF_USARTx = GPIO_AF_USART3;
            GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
            GPIO_PinSource_TX = GPIO_PinSource8;
            GPIO_PinSource_RX = GPIO_PinSource9;
#endif

            RCC_APBxPeriph = RCC_APB1Periph_USART3;
            RCC_APBxPeriphClockCmd =RCC_APB1PeriphClockCmd;
            break;
        case PYB_USART_6:
            USARTx = USART6;

            GPIO_Port = GPIOC;
            GPIO_AF_USARTx = GPIO_AF_USART6;
            GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
            GPIO_PinSource_TX = GPIO_PinSource6;
            GPIO_PinSource_RX = GPIO_PinSource7;

            RCC_APBxPeriph = RCC_APB2Periph_USART6;
            RCC_APBxPeriphClockCmd =RCC_APB2PeriphClockCmd;
            break;
    }

    /* Initialize USARTx */
    RCC_APBxPeriphClockCmd(RCC_APBxPeriph, ENABLE);

    GPIO_PinAFConfig(GPIO_Port, GPIO_PinSource_TX, GPIO_AF_USARTx);
    GPIO_PinAFConfig(GPIO_Port, GPIO_PinSource_RX, GPIO_AF_USARTx);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIO_Port, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStructure);

    USART_Cmd(USARTx, ENABLE);
}

bool usart_rx_any(pyb_usart_t usart_id) {
    USART_TypeDef *USARTx = usart_get_base(usart_id);
    return USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == SET;
}

int usart_rx_char(pyb_usart_t usart_id) {
    USART_TypeDef *USARTx = usart_get_base(usart_id);
    return USART_ReceiveData(USARTx);
}

void usart_tx_char(pyb_usart_t usart_id, int c) {
    USART_TypeDef *USARTx = usart_get_base(usart_id);
    // wait until the end of any previous transmission
    uint32_t timeout = 100000;
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET && --timeout > 0) {
    }
    USART_SendData(USARTx, c);
}

void usart_tx_str(pyb_usart_t usart_id, const char *str) {
    for (; *str; str++) {
        usart_tx_char(usart_id, *str);
    }
}

void usart_tx_bytes(pyb_usart_t usart_id, const char *data, uint len) {
    for (; len > 0; data++, len--) {
        usart_tx_char(usart_id, *data);
    }
}

void usart_tx_strn_cooked(pyb_usart_t usart_id, const char *str, int len) {
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            usart_tx_char(usart_id, '\r');
        }
        usart_tx_char(usart_id, *str);
    }
}


/******************************************************************************/
/* Micro Python bindings                                                      */

typedef struct _pyb_usart_obj_t {
    mp_obj_base_t base;
    pyb_usart_t usart_id;
    bool is_enabled;
} pyb_usart_obj_t;

static mp_obj_t usart_obj_status(mp_obj_t self_in) {
    pyb_usart_obj_t *self = self_in;
    if (usart_rx_any(self->usart_id)) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

static mp_obj_t usart_obj_rx_char(mp_obj_t self_in) {
    mp_obj_t ret = mp_const_none;
    pyb_usart_obj_t *self = self_in;

    if (self->is_enabled) {
        ret =  mp_obj_new_int(usart_rx_char(self->usart_id));
    }
    return ret;
}

static mp_obj_t usart_obj_tx_char(mp_obj_t self_in, mp_obj_t c) {
    pyb_usart_obj_t *self = self_in;
    uint len;
    const char *str = mp_obj_str_get_data(c, &len);
    if (len == 1 && self->is_enabled) {
        usart_tx_char(self->usart_id, str[0]);
    }
    return mp_const_none;
}

static mp_obj_t usart_obj_tx_str(mp_obj_t self_in, mp_obj_t s) {
    pyb_usart_obj_t *self = self_in;
    if (self->is_enabled) {
        if (MP_OBJ_IS_STR(s)) {
            uint len;
            const char *data = mp_obj_str_get_data(s, &len);
            usart_tx_bytes(self->usart_id, data, len);
        }
    }
    return mp_const_none;
}

static void usart_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_usart_obj_t *self = self_in;
    print(env, "<Usart %lu>", self->usart_id);
}

static MP_DEFINE_CONST_FUN_OBJ_1(usart_obj_status_obj, usart_obj_status);
static MP_DEFINE_CONST_FUN_OBJ_1(usart_obj_rx_char_obj, usart_obj_rx_char);
static MP_DEFINE_CONST_FUN_OBJ_2(usart_obj_tx_char_obj, usart_obj_tx_char);
static MP_DEFINE_CONST_FUN_OBJ_2(usart_obj_tx_str_obj, usart_obj_tx_str);

STATIC const mp_map_elem_t usart_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_status), (mp_obj_t)&usart_obj_status_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv_chr), (mp_obj_t)&usart_obj_rx_char_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_chr), (mp_obj_t)&usart_obj_tx_char_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&usart_obj_tx_str_obj },
};

STATIC MP_DEFINE_CONST_DICT(usart_locals_dict, usart_locals_dict_table);

STATIC const mp_obj_type_t usart_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_Usart,
    .print = usart_obj_print,
    .locals_dict = (mp_obj_t)&usart_locals_dict,
};

STATIC mp_obj_t pyb_Usart(mp_obj_t usart_id, mp_obj_t baudrate) {
    if (mp_obj_get_int(usart_id)>PYB_USART_MAX) {
        return mp_const_none;
    }

    /* init USART */
    usart_init(mp_obj_get_int(usart_id), mp_obj_get_int(baudrate));

    pyb_usart_obj_t *o = m_new_obj(pyb_usart_obj_t);
    o->base.type = &usart_obj_type;
    o->usart_id = mp_obj_get_int(usart_id);
    o->is_enabled = true;
    return o;
}

MP_DEFINE_CONST_FUN_OBJ_2(pyb_Usart_obj, pyb_Usart);
