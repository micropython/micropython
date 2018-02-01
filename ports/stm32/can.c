/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include "py/objtuple.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "bufhelper.h"
#include "can.h"
#include "irq.h"

#if MICROPY_HW_ENABLE_CAN

#define MASK16 (0)
#define LIST16 (1)
#define MASK32 (2)
#define LIST32 (3)

/// \moduleref pyb
/// \class CAN - controller area network communication bus
///
/// CAN implements the standard CAN communications protocol.  At
/// the physical level it consists of 2 lines: RX and TX.  Note that
/// to connect the pyboard to a CAN bus you must use a CAN transceiver
/// to convert the CAN logic signals from the pyboard to the correct
/// voltage levels on the bus.
///
/// Note that this driver does not yet support filter configuration
/// (it defaults to a single filter that lets through all messages),
/// or bus timing configuration (except for setting the prescaler).
///
/// Example usage (works without anything connected):
///
///     from pyb import CAN
///     can = pyb.CAN(1, pyb.CAN.LOOPBACK)
///     can.send('message!', 123)   # send message with id 123
///     can.recv(0)                 # receive message on FIFO 0

typedef enum _rx_state_t {
    RX_STATE_FIFO_EMPTY = 0,
    RX_STATE_MESSAGE_PENDING,
    RX_STATE_FIFO_FULL,
    RX_STATE_FIFO_OVERFLOW,
} rx_state_t;

typedef struct _pyb_can_obj_t {
    mp_obj_base_t base;
    mp_obj_t rxcallback0;
    mp_obj_t rxcallback1;
    mp_uint_t can_id : 8;
    bool is_enabled : 1;
    bool extframe : 1;
    byte rx_state0;
    byte rx_state1;
    CAN_HandleTypeDef can;
} pyb_can_obj_t;

STATIC mp_obj_t pyb_can_deinit(mp_obj_t self_in);

STATIC uint8_t can2_start_bank = 14;

// assumes Init parameters have been set up correctly
STATIC bool can_init(pyb_can_obj_t *can_obj) {
    CAN_TypeDef *CANx = NULL;

    uint32_t GPIO_Pin = 0;
    uint8_t  GPIO_AF_CANx = 0;
    GPIO_TypeDef* GPIO_Port = NULL;

    switch (can_obj->can_id) {
        // CAN1 is on RX,TX = Y3,Y4 = PB9,PB9
        case PYB_CAN_1:
            CANx = CAN1;
            GPIO_AF_CANx = GPIO_AF9_CAN1;
            GPIO_Port = GPIOB;
            GPIO_Pin = GPIO_PIN_8 | GPIO_PIN_9;
            __CAN1_CLK_ENABLE();
            break;

        #if defined(CAN2)
        // CAN2 is on RX,TX = Y5,Y6 = PB12,PB13
        case PYB_CAN_2:
            CANx = CAN2;
            GPIO_AF_CANx = GPIO_AF9_CAN2;
            GPIO_Port = GPIOB;
            GPIO_Pin = GPIO_PIN_12 | GPIO_PIN_13;
            __CAN1_CLK_ENABLE(); // CAN2 is a "slave" and needs CAN1 enabled as well
            __CAN2_CLK_ENABLE();
            break;
        #endif

        default:
            return false;
    }

    // init GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_Pin;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = GPIO_AF_CANx;
    HAL_GPIO_Init(GPIO_Port, &GPIO_InitStructure);

    // init CANx
    can_obj->can.Instance = CANx;
    HAL_CAN_Init(&can_obj->can);

    can_obj->is_enabled = true;

    return true;
}

void can_init0(void) {
    for (uint i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_can_obj_all)); i++) {
        MP_STATE_PORT(pyb_can_obj_all)[i] = NULL;
    }
}

void can_deinit(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(pyb_can_obj_all)); i++) {
        pyb_can_obj_t *can_obj = MP_STATE_PORT(pyb_can_obj_all)[i];
        if (can_obj != NULL) {
            pyb_can_deinit(can_obj);
        }
    }
}

STATIC void can_clearfilter(uint32_t f) {
    CAN_FilterConfTypeDef filter;

    filter.FilterIdHigh         = 0;
    filter.FilterIdLow          = 0;
    filter.FilterMaskIdHigh     = 0;
    filter.FilterMaskIdLow      = 0;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterNumber         = f;
    filter.FilterMode           = CAN_FILTERMODE_IDMASK;
    filter.FilterScale          = CAN_FILTERSCALE_16BIT;
    filter.FilterActivation     = DISABLE;
    filter.BankNumber           = can2_start_bank;

    HAL_CAN_ConfigFilter(NULL, &filter);
}

// We have our own version of CAN transmit so we can handle Timeout=0 correctly.
STATIC HAL_StatusTypeDef CAN_Transmit(CAN_HandleTypeDef *hcan, uint32_t Timeout) {
    uint32_t transmitmailbox;
    uint32_t tickstart;
    uint32_t rqcpflag;
    uint32_t txokflag;

    // Check the parameters
    assert_param(IS_CAN_IDTYPE(hcan->pTxMsg->IDE));
    assert_param(IS_CAN_RTR(hcan->pTxMsg->RTR));
    assert_param(IS_CAN_DLC(hcan->pTxMsg->DLC));

    // Select one empty transmit mailbox
    if ((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0) {
        transmitmailbox = CAN_TXMAILBOX_0;
        rqcpflag = CAN_FLAG_RQCP0;
        txokflag = CAN_FLAG_TXOK0;
    } else if ((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1) {
        transmitmailbox = CAN_TXMAILBOX_1;
        rqcpflag = CAN_FLAG_RQCP1;
        txokflag = CAN_FLAG_TXOK1;
    } else if ((hcan->Instance->TSR&CAN_TSR_TME2) == CAN_TSR_TME2) {
        transmitmailbox = CAN_TXMAILBOX_2;
        rqcpflag = CAN_FLAG_RQCP2;
        txokflag = CAN_FLAG_TXOK2;
    } else {
        transmitmailbox = CAN_TXSTATUS_NOMAILBOX;
    }

    if (transmitmailbox != CAN_TXSTATUS_NOMAILBOX) {
        // Set up the Id
        hcan->Instance->sTxMailBox[transmitmailbox].TIR &= CAN_TI0R_TXRQ;
        if (hcan->pTxMsg->IDE == CAN_ID_STD) {
            assert_param(IS_CAN_STDID(hcan->pTxMsg->StdId));
            hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->StdId << 21) | \
                                                        hcan->pTxMsg->RTR);
        } else {
            assert_param(IS_CAN_EXTID(hcan->pTxMsg->ExtId));
            hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->ExtId << 3) | \
                                                        hcan->pTxMsg->IDE | \
                                                        hcan->pTxMsg->RTR);
        }

        // Set up the DLC
        hcan->pTxMsg->DLC &= (uint8_t)0x0000000F;
        hcan->Instance->sTxMailBox[transmitmailbox].TDTR &= (uint32_t)0xFFFFFFF0;
        hcan->Instance->sTxMailBox[transmitmailbox].TDTR |= hcan->pTxMsg->DLC;

        // Set up the data field
        hcan->Instance->sTxMailBox[transmitmailbox].TDLR = (((uint32_t)hcan->pTxMsg->Data[3] << 24) |
                                                ((uint32_t)hcan->pTxMsg->Data[2] << 16) |
                                                ((uint32_t)hcan->pTxMsg->Data[1] << 8) |
                                                ((uint32_t)hcan->pTxMsg->Data[0]));
        hcan->Instance->sTxMailBox[transmitmailbox].TDHR = (((uint32_t)hcan->pTxMsg->Data[7] << 24) |
                                                ((uint32_t)hcan->pTxMsg->Data[6] << 16) |
                                                ((uint32_t)hcan->pTxMsg->Data[5] << 8) |
                                                ((uint32_t)hcan->pTxMsg->Data[4]));
        // Request transmission
        hcan->Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;

        if (Timeout == 0) {
            return HAL_OK;
        }

        // Get tick
        tickstart = HAL_GetTick();
        // Check End of transmission flag
        while (!(__HAL_CAN_TRANSMIT_STATUS(hcan, transmitmailbox))) {
            // Check for the Timeout
            if (Timeout != HAL_MAX_DELAY) {
                if ((HAL_GetTick() - tickstart) > Timeout) {
                    // When the timeout expires, we try to abort the transmission of the packet
                    __HAL_CAN_CANCEL_TRANSMIT(hcan, transmitmailbox);
                    while (!__HAL_CAN_GET_FLAG(hcan, rqcpflag)) {
                    }
                    if (__HAL_CAN_GET_FLAG(hcan, txokflag)) {
                        // The abort attempt failed and the message was sent properly
                        return HAL_OK;
                    } else {
                        return HAL_TIMEOUT;
                    }
                }
            }
        }
        return HAL_OK;
    } else {
        return HAL_BUSY;
    }
}

/******************************************************************************/
// MicroPython bindings

STATIC void pyb_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_can_obj_t *self = self_in;
    if (!self->is_enabled) {
        mp_printf(print, "CAN(%u)", self->can_id);
    } else {
        mp_printf(print, "CAN(%u, CAN.", self->can_id);
        qstr mode;
        switch (self->can.Init.Mode) {
            case CAN_MODE_NORMAL: mode = MP_QSTR_NORMAL; break;
            case CAN_MODE_LOOPBACK: mode = MP_QSTR_LOOPBACK; break;
            case CAN_MODE_SILENT: mode = MP_QSTR_SILENT; break;
            case CAN_MODE_SILENT_LOOPBACK: default: mode = MP_QSTR_SILENT_LOOPBACK; break;
        }
        mp_printf(print, "%q, extframe=", mode);
        if (self->extframe) {
            mode = MP_QSTR_True;
        } else {
            mode = MP_QSTR_False;
        }
        mp_printf(print, "%q)", mode);
    }
}

// init(mode, extframe=False, prescaler=100, *, sjw=1, bs1=6, bs2=8)
STATIC mp_obj_t pyb_can_init_helper(pyb_can_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int  = CAN_MODE_NORMAL} },
        { MP_QSTR_extframe,     MP_ARG_BOOL,                    {.u_bool = false} },
        { MP_QSTR_prescaler,    MP_ARG_INT,                     {.u_int  = 100} },
        { MP_QSTR_sjw,          MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int = 1} },
        { MP_QSTR_bs1,          MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int = 6} },
        { MP_QSTR_bs2,          MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int = 8} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->extframe = args[1].u_bool;

    // set the CAN configuration values
    memset(&self->can, 0, sizeof(self->can));
    CAN_InitTypeDef *init = &self->can.Init;
    init->Mode = args[0].u_int << 4; // shift-left so modes fit in a small-int
    init->Prescaler = args[2].u_int;
    init->SJW = ((args[3].u_int - 1) & 3) << 24;
    init->BS1 = ((args[4].u_int - 1) & 0xf) << 16;
    init->BS2 = ((args[5].u_int - 1) & 7) << 20;
    init->TTCM = DISABLE;
    init->ABOM = DISABLE;
    init->AWUM = DISABLE;
    init->NART = DISABLE;
    init->RFLM = DISABLE;
    init->TXFP = DISABLE;

    // init CAN (if it fails, it's because the port doesn't exist)
    if (!can_init(self)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "CAN(%d) doesn't exist", self->can_id));
    }

    return mp_const_none;
}

/// \classmethod \constructor(bus, ...)
///
/// Construct a CAN object on the given bus.  `bus` can be 1-2, or 'YA' or 'YB'.
/// With no additional parameters, the CAN object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).  If extra arguments are given, the bus is initialised.
/// See `init` for parameters of initialisation.
///
/// The physical pins of the CAN busses are:
///
///   - `CAN(1)` is on `YA`: `(RX, TX) = (Y3, Y4) = (PB8, PB9)`
///   - `CAN(2)` is on `YB`: `(RX, TX) = (Y5, Y6) = (PB12, PB13)`
STATIC mp_obj_t pyb_can_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out port
    mp_uint_t can_idx;
    if (MP_OBJ_IS_STR(args[0])) {
        const char *port = mp_obj_str_get_str(args[0]);
        if (0) {
        #ifdef MICROPY_HW_CAN1_NAME
        } else if (strcmp(port, MICROPY_HW_CAN1_NAME) == 0) {
            can_idx = PYB_CAN_1;
        #endif
        #ifdef MICROPY_HW_CAN2_NAME
        } else if (strcmp(port, MICROPY_HW_CAN2_NAME) == 0) {
            can_idx = PYB_CAN_2;
        #endif
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "CAN(%s) doesn't exist", port));
        }
    } else {
        can_idx = mp_obj_get_int(args[0]);
    }
    if (can_idx < 1 || can_idx > MP_ARRAY_SIZE(MP_STATE_PORT(pyb_can_obj_all))) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "CAN(%d) doesn't exist", can_idx));
    }

    pyb_can_obj_t *self;
    if (MP_STATE_PORT(pyb_can_obj_all)[can_idx - 1] == NULL) {
        self = m_new_obj(pyb_can_obj_t);
        self->base.type = &pyb_can_type;
        self->can_id = can_idx;
        self->is_enabled = false;
        MP_STATE_PORT(pyb_can_obj_all)[can_idx - 1] = self;
    } else {
        self = MP_STATE_PORT(pyb_can_obj_all)[can_idx - 1];
    }

    if (!self->is_enabled || n_args > 1) {
        if (self->is_enabled) {
            // The caller is requesting a reconfiguration of the hardware
            // this can only be done if the hardware is in init mode
            pyb_can_deinit(self);
        }

        self->rxcallback0 = mp_const_none;
        self->rxcallback1 = mp_const_none;
        self->rx_state0 = RX_STATE_FIFO_EMPTY;
        self->rx_state1 = RX_STATE_FIFO_EMPTY;

        if (n_args > 1 || n_kw > 0) {
            // start the peripheral
            mp_map_t kw_args;
            mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
            pyb_can_init_helper(self, n_args - 1, args + 1, &kw_args);
        }
    }

    return self;
}

STATIC mp_obj_t pyb_can_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_can_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_init_obj, 1, pyb_can_init);

/// \method deinit()
/// Turn off the CAN bus.
STATIC mp_obj_t pyb_can_deinit(mp_obj_t self_in) {
    pyb_can_obj_t *self = self_in;
    self->is_enabled = false;
    HAL_CAN_DeInit(&self->can);
    if (self->can.Instance == CAN1) {
        HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
        HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
        __CAN1_FORCE_RESET();
        __CAN1_RELEASE_RESET();
        __CAN1_CLK_DISABLE();
    #if defined(CAN2)
    } else if (self->can.Instance == CAN2) {
        HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
        HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
        __CAN2_FORCE_RESET();
        __CAN2_RELEASE_RESET();
        __CAN2_CLK_DISABLE();
    #endif
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_can_deinit_obj, pyb_can_deinit);

/// \method any(fifo)
/// Return `True` if any message waiting on the FIFO, else `False`.
STATIC mp_obj_t pyb_can_any(mp_obj_t self_in, mp_obj_t fifo_in) {
    pyb_can_obj_t *self = self_in;
    mp_int_t fifo = mp_obj_get_int(fifo_in);
    if (fifo == 0) {
        if (__HAL_CAN_MSG_PENDING(&self->can, CAN_FIFO0) != 0) {
            return mp_const_true;
        }
    } else {
        if (__HAL_CAN_MSG_PENDING(&self->can, CAN_FIFO1) != 0) {
            return mp_const_true;
        }
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_can_any_obj, pyb_can_any);

/// \method send(send, addr, *, timeout=5000)
/// Send a message on the bus:
///
///   - `send` is the data to send (an integer to send, or a buffer object).
///   - `addr` is the address to send to
///   - `timeout` is the timeout in milliseconds to wait for the send.
///
/// Return value: `None`.
STATIC mp_obj_t pyb_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_id,      MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_rtr,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    pyb_can_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[0].u_obj, &bufinfo, data);

    if (bufinfo.len > 8) {
        mp_raise_ValueError("CAN data field too long");
    }

    // send the data
    CanTxMsgTypeDef tx_msg;
    if (self->extframe) {
        tx_msg.ExtId = args[1].u_int & 0x1FFFFFFF;
        tx_msg.IDE = CAN_ID_EXT;
    } else {
        tx_msg.StdId = args[1].u_int & 0x7FF;
        tx_msg.IDE = CAN_ID_STD;
    }
    if (args[3].u_bool == false) {
        tx_msg.RTR = CAN_RTR_DATA;
    } else  {
        tx_msg.RTR = CAN_RTR_REMOTE;
    }
    tx_msg.DLC = bufinfo.len;
    for (mp_uint_t i = 0; i < bufinfo.len; i++) {
        tx_msg.Data[i] = ((byte*)bufinfo.buf)[i]; // Data is uint32_t but holds only 1 byte
    }

    self->can.pTxMsg = &tx_msg;
    HAL_StatusTypeDef status = CAN_Transmit(&self->can, args[2].u_int);

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_send_obj, 1, pyb_can_send);

/// \method recv(fifo, *, timeout=5000)
///
/// Receive data on the bus:
///
///   - `fifo` is an integer, which is the FIFO to receive on
///   - `timeout` is the timeout in milliseconds to wait for the receive.
///
/// Return value: buffer of data bytes.
STATIC mp_obj_t pyb_can_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_fifo,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_can_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // receive the data
    CanRxMsgTypeDef rx_msg;
    self->can.pRxMsg = self->can.pRx1Msg = &rx_msg;
    HAL_StatusTypeDef status = HAL_CAN_Receive(&self->can, args[0].u_int, args[1].u_int);

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

    // Manage the rx state machine
    if ((args[0].u_int == CAN_FIFO0 && self->rxcallback0 != mp_const_none) ||
        (args[0].u_int == CAN_FIFO1 && self->rxcallback1 != mp_const_none)) {
        byte *state = (args[0].u_int == CAN_FIFO0) ? &self->rx_state0 : &self->rx_state1;

        switch (*state) {
        case RX_STATE_FIFO_EMPTY:
            break;
        case RX_STATE_MESSAGE_PENDING:
            if (__HAL_CAN_MSG_PENDING(&self->can, args[0].u_int) == 0) {
                // Fifo is empty
                __HAL_CAN_ENABLE_IT(&self->can, (args[0].u_int == CAN_FIFO0) ? CAN_IT_FMP0 : CAN_IT_FMP1);
                *state = RX_STATE_FIFO_EMPTY;
            }
            break;
        case RX_STATE_FIFO_FULL:
            __HAL_CAN_ENABLE_IT(&self->can, (args[0].u_int == CAN_FIFO0) ? CAN_IT_FF0 : CAN_IT_FF1);
            *state = RX_STATE_MESSAGE_PENDING;
            break;
        case RX_STATE_FIFO_OVERFLOW:
            __HAL_CAN_ENABLE_IT(&self->can, (args[0].u_int == CAN_FIFO0) ? CAN_IT_FOV0 : CAN_IT_FOV1);
            __HAL_CAN_ENABLE_IT(&self->can, (args[0].u_int == CAN_FIFO0) ? CAN_IT_FF0  : CAN_IT_FF1);
            *state = RX_STATE_MESSAGE_PENDING;
            break;
        }
    }

    // return the received data
    // TODO use a namedtuple (when namedtuple types can be stored in ROM)
    mp_obj_tuple_t *tuple = mp_obj_new_tuple(4, NULL);
    if (rx_msg.IDE == CAN_ID_STD) {
        tuple->items[0] = MP_OBJ_NEW_SMALL_INT(rx_msg.StdId);
    } else {
        tuple->items[0] = MP_OBJ_NEW_SMALL_INT(rx_msg.ExtId);
    }
    tuple->items[1] = rx_msg.RTR == CAN_RTR_REMOTE ? mp_const_true : mp_const_false;
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(rx_msg.FMI);
    vstr_t vstr;
    vstr_init_len(&vstr, rx_msg.DLC);
    for (mp_uint_t i = 0; i < rx_msg.DLC; i++) {
        vstr.buf[i] = rx_msg.Data[i]; // Data is uint32_t but holds only 1 byte
    }
    tuple->items[3] = mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    return tuple;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_recv_obj, 1, pyb_can_recv);

/// \class method initfilterbanks
///
/// Set up the filterbanks. All filter will be disabled and set to their reset states.
///
///   - `banks` is an integer that sets how many filter banks that are reserved for CAN1.
///     0  -> no filters assigned for CAN1
///     28 -> all filters are assigned to CAN1
///     CAN2 will get the rest of the 28 available.
///
/// Return value: none.
STATIC mp_obj_t pyb_can_initfilterbanks(mp_obj_t self, mp_obj_t bank_in) {
    can2_start_bank = mp_obj_get_int(bank_in);

    for (int f = 0; f < 28; f++) {
        can_clearfilter(f);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_can_initfilterbanks_fun_obj, pyb_can_initfilterbanks);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pyb_can_initfilterbanks_obj, (const mp_obj_t)&pyb_can_initfilterbanks_fun_obj);

STATIC mp_obj_t pyb_can_clearfilter(mp_obj_t self_in, mp_obj_t bank_in) {
    pyb_can_obj_t *self = self_in;
    mp_int_t f = mp_obj_get_int(bank_in);
    if (self->can_id == 2) {
        f += can2_start_bank;
    }
    can_clearfilter(f);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_can_clearfilter_obj, pyb_can_clearfilter);

/// Configures a filterbank
/// Return value: `None`.
#define EXTENDED_ID_TO_16BIT_FILTER(id) (((id & 0xC00000) >> 13) | ((id & 0x38000) >> 15)) | 8
STATIC mp_obj_t pyb_can_setfilter(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bank,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_fifo,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = CAN_FILTER_FIFO0} },
        { MP_QSTR_params,   MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rtr,      MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    pyb_can_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t len;
    size_t rtr_len;
    mp_uint_t rtr_masks[4] = {0, 0, 0, 0};
    mp_obj_t *rtr_flags;
    mp_obj_t *params;
    mp_obj_get_array(args[3].u_obj, &len, &params);
    if (args[4].u_obj != MP_OBJ_NULL){
        mp_obj_get_array(args[4].u_obj, &rtr_len, &rtr_flags);
    }

    CAN_FilterConfTypeDef filter;
    if (args[1].u_int == MASK16 || args[1].u_int == LIST16) {
        if (len != 4) {
            goto error;
        }
        filter.FilterScale = CAN_FILTERSCALE_16BIT;
        if (self->extframe) {
            if (args[4].u_obj != MP_OBJ_NULL) {
                if (args[1].u_int == MASK16) {
                    rtr_masks[0] = mp_obj_get_int(rtr_flags[0]) ? 0x02 : 0;
                    rtr_masks[1] = 0x02;
                    rtr_masks[2] = mp_obj_get_int(rtr_flags[1]) ? 0x02 : 0;
                    rtr_masks[3] = 0x02;
                } else {  // LIST16
                    rtr_masks[0] = mp_obj_get_int(rtr_flags[0]) ? 0x02 : 0;
                    rtr_masks[1] = mp_obj_get_int(rtr_flags[1]) ? 0x02 : 0;
                    rtr_masks[2] = mp_obj_get_int(rtr_flags[2]) ? 0x02 : 0;
                    rtr_masks[3] = mp_obj_get_int(rtr_flags[3]) ? 0x02 : 0;
                }
            }
            filter.FilterIdLow      = EXTENDED_ID_TO_16BIT_FILTER(mp_obj_get_int(params[0])) | rtr_masks[0]; // id1
            filter.FilterMaskIdLow  = EXTENDED_ID_TO_16BIT_FILTER(mp_obj_get_int(params[1])) | rtr_masks[1]; // mask1
            filter.FilterIdHigh     = EXTENDED_ID_TO_16BIT_FILTER(mp_obj_get_int(params[2])) | rtr_masks[2]; // id2
            filter.FilterMaskIdHigh = EXTENDED_ID_TO_16BIT_FILTER(mp_obj_get_int(params[3])) | rtr_masks[3]; // mask2
        } else { // Basic frames
            if (args[4].u_obj != MP_OBJ_NULL) {
                if (args[1].u_int == MASK16) {
                    rtr_masks[0] = mp_obj_get_int(rtr_flags[0]) ? 0x10 : 0;
                    rtr_masks[1] = 0x10;
                    rtr_masks[2] = mp_obj_get_int(rtr_flags[1]) ? 0x10 : 0;
                    rtr_masks[3] = 0x10;
                } else {  // LIST16
                    rtr_masks[0] = mp_obj_get_int(rtr_flags[0]) ? 0x10 : 0;
                    rtr_masks[1] = mp_obj_get_int(rtr_flags[1]) ? 0x10 : 0;
                    rtr_masks[2] = mp_obj_get_int(rtr_flags[2]) ? 0x10 : 0;
                    rtr_masks[3] = mp_obj_get_int(rtr_flags[3]) ? 0x10 : 0;
                }
            }
            filter.FilterIdLow      = (mp_obj_get_int(params[0]) << 5) | rtr_masks[0]; // id1
            filter.FilterMaskIdLow  = (mp_obj_get_int(params[1]) << 5) | rtr_masks[1]; // mask1
            filter.FilterIdHigh     = (mp_obj_get_int(params[2]) << 5) | rtr_masks[2]; // id2
            filter.FilterMaskIdHigh = (mp_obj_get_int(params[3]) << 5) | rtr_masks[3]; // mask2
        }
        if (args[1].u_int == MASK16) {
            filter.FilterMode  = CAN_FILTERMODE_IDMASK;
        }
        if (args[1].u_int == LIST16) {
            filter.FilterMode  = CAN_FILTERMODE_IDLIST;
        }
    }
    else if (args[1].u_int == MASK32 || args[1].u_int == LIST32) {
        if (len != 2) {
            goto error;
        }
        filter.FilterScale = CAN_FILTERSCALE_32BIT;
        if (args[4].u_obj != MP_OBJ_NULL) {
            if (args[1].u_int == MASK32) {
                rtr_masks[0] = mp_obj_get_int(rtr_flags[0]) ? 0x02 : 0;
                rtr_masks[1] = 0x02;
            } else {  // LIST32
                rtr_masks[0] = mp_obj_get_int(rtr_flags[0]) ? 0x02 : 0;
                rtr_masks[1] = mp_obj_get_int(rtr_flags[1]) ? 0x02 : 0;
            }
        }
        filter.FilterIdHigh     = (mp_obj_get_int(params[0]) & 0x1FFFE000)  >> 13;
        filter.FilterIdLow      = (((mp_obj_get_int(params[0]) & 0x00001FFF) << 3) | 4) | rtr_masks[0];
        filter.FilterMaskIdHigh = (mp_obj_get_int(params[1]) & 0x1FFFE000 ) >> 13;
        filter.FilterMaskIdLow  = (((mp_obj_get_int(params[1]) & 0x00001FFF) << 3) | 4) | rtr_masks[1];
        if (args[1].u_int == MASK32) {
            filter.FilterMode  = CAN_FILTERMODE_IDMASK;
        }
        if (args[1].u_int == LIST32) {
            filter.FilterMode  = CAN_FILTERMODE_IDLIST;
        }
    } else {
        goto error;
    }

    filter.FilterFIFOAssignment = args[2].u_int; // fifo
    filter.FilterNumber = args[0].u_int; // bank
    if (self->can_id == 1) {
        if (filter.FilterNumber >= can2_start_bank) {
            goto error;
        }
    } else {
        filter.FilterNumber = filter.FilterNumber + can2_start_bank;
        if (filter.FilterNumber > 27) {
            goto error;
        }
    }
    filter.FilterActivation = ENABLE;
    filter.BankNumber = can2_start_bank;
    HAL_CAN_ConfigFilter(&self->can, &filter);

    return mp_const_none;

error:
    mp_raise_ValueError("CAN filter parameter error");
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_setfilter_obj, 1, pyb_can_setfilter);

STATIC mp_obj_t pyb_can_rxcallback(mp_obj_t self_in, mp_obj_t fifo_in, mp_obj_t callback_in) {
    pyb_can_obj_t *self = self_in;
    mp_int_t fifo = mp_obj_get_int(fifo_in);
    mp_obj_t *callback;

    callback = (fifo == 0) ? &self->rxcallback0 : &self->rxcallback1;
    if (callback_in == mp_const_none) {
        __HAL_CAN_DISABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FMP0 : CAN_IT_FMP1);
        __HAL_CAN_DISABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FF0 : CAN_IT_FF1);
        __HAL_CAN_DISABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FOV0 : CAN_IT_FOV1);
        __HAL_CAN_CLEAR_FLAG(&self->can, (fifo == CAN_FIFO0) ? CAN_FLAG_FF0 : CAN_FLAG_FF1);
        __HAL_CAN_CLEAR_FLAG(&self->can, (fifo == CAN_FIFO0) ? CAN_FLAG_FOV0 : CAN_FLAG_FOV1);
        *callback = mp_const_none;
    } else if (*callback != mp_const_none) {
        // Rx call backs has already been initialized
        // only the callback function should be changed
        *callback = callback_in;
    } else if (mp_obj_is_callable(callback_in)) {
        *callback = callback_in;
        uint32_t irq = 0;
        if (self->can_id == PYB_CAN_1) {
            irq = (fifo == 0) ? CAN1_RX0_IRQn : CAN1_RX1_IRQn;
        #if defined(CAN2)
        } else {
            irq = (fifo == 0) ? CAN2_RX0_IRQn : CAN2_RX1_IRQn;
        #endif
        }
        HAL_NVIC_SetPriority(irq, IRQ_PRI_CAN, IRQ_SUBPRI_CAN);
        HAL_NVIC_EnableIRQ(irq);
        __HAL_CAN_ENABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FMP0 : CAN_IT_FMP1);
        __HAL_CAN_ENABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FF0  : CAN_IT_FF1);
        __HAL_CAN_ENABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FOV0 : CAN_IT_FOV1);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_can_rxcallback_obj, pyb_can_rxcallback);

STATIC const mp_rom_map_elem_t pyb_can_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_can_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&pyb_can_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&pyb_can_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&pyb_can_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&pyb_can_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_initfilterbanks), MP_ROM_PTR(&pyb_can_initfilterbanks_obj) },
    { MP_ROM_QSTR(MP_QSTR_setfilter), MP_ROM_PTR(&pyb_can_setfilter_obj) },
    { MP_ROM_QSTR(MP_QSTR_clearfilter), MP_ROM_PTR(&pyb_can_clearfilter_obj) },
    { MP_ROM_QSTR(MP_QSTR_rxcallback), MP_ROM_PTR(&pyb_can_rxcallback_obj) },

    // class constants
    // Note: we use the ST constants >> 4 so they fit in a small-int.  The
    // right-shift is undone when the constants are used in the init function.
    { MP_ROM_QSTR(MP_QSTR_NORMAL), MP_ROM_INT(CAN_MODE_NORMAL >> 4) },
    { MP_ROM_QSTR(MP_QSTR_LOOPBACK), MP_ROM_INT(CAN_MODE_LOOPBACK >> 4) },
    { MP_ROM_QSTR(MP_QSTR_SILENT), MP_ROM_INT(CAN_MODE_SILENT >> 4) },
    { MP_ROM_QSTR(MP_QSTR_SILENT_LOOPBACK), MP_ROM_INT(CAN_MODE_SILENT_LOOPBACK >> 4) },
    { MP_ROM_QSTR(MP_QSTR_MASK16), MP_ROM_INT(MASK16) },
    { MP_ROM_QSTR(MP_QSTR_LIST16), MP_ROM_INT(LIST16) },
    { MP_ROM_QSTR(MP_QSTR_MASK32), MP_ROM_INT(MASK32) },
    { MP_ROM_QSTR(MP_QSTR_LIST32), MP_ROM_INT(LIST32) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_can_locals_dict, pyb_can_locals_dict_table);

mp_uint_t can_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    pyb_can_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD)
            && ((__HAL_CAN_MSG_PENDING(&self->can, CAN_FIFO0) != 0)
                || (__HAL_CAN_MSG_PENDING(&self->can, CAN_FIFO1) != 0))) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && (self->can.Instance->TSR & CAN_TSR_TME)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = -1;
    }
    return ret;
}

void can_rx_irq_handler(uint can_id, uint fifo_id) {
    mp_obj_t callback;
    pyb_can_obj_t *self;
    mp_obj_t irq_reason = MP_OBJ_NEW_SMALL_INT(0);
    byte *state;

    self = MP_STATE_PORT(pyb_can_obj_all)[can_id - 1];

    if (fifo_id == CAN_FIFO0) {
        callback = self->rxcallback0;
        state = &self->rx_state0;
    } else {
        callback = self->rxcallback1;
        state = &self->rx_state1;
    }

    switch (*state) {
        case RX_STATE_FIFO_EMPTY:
            __HAL_CAN_DISABLE_IT(&self->can,  (fifo_id == CAN_FIFO0) ? CAN_IT_FMP0 : CAN_IT_FMP1);
            irq_reason = MP_OBJ_NEW_SMALL_INT(0);
            *state = RX_STATE_MESSAGE_PENDING;
            break;
        case RX_STATE_MESSAGE_PENDING:
            __HAL_CAN_DISABLE_IT(&self->can, (fifo_id == CAN_FIFO0) ? CAN_IT_FF0 : CAN_IT_FF1);
            __HAL_CAN_CLEAR_FLAG(&self->can, (fifo_id == CAN_FIFO0) ? CAN_FLAG_FF0 : CAN_FLAG_FF1);
            irq_reason = MP_OBJ_NEW_SMALL_INT(1);
            *state = RX_STATE_FIFO_FULL;
            break;
        case RX_STATE_FIFO_FULL:
            __HAL_CAN_DISABLE_IT(&self->can, (fifo_id == CAN_FIFO0) ? CAN_IT_FOV0 : CAN_IT_FOV1);
            __HAL_CAN_CLEAR_FLAG(&self->can, (fifo_id == CAN_FIFO0) ? CAN_FLAG_FOV0 : CAN_FLAG_FOV1);
            irq_reason = MP_OBJ_NEW_SMALL_INT(2);
            *state = RX_STATE_FIFO_OVERFLOW;
            break;
        case RX_STATE_FIFO_OVERFLOW:
            // This should never happen
            break;
    }

    if (callback != mp_const_none) {
        mp_sched_lock();
        gc_lock();
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_call_function_2(callback, self, irq_reason);
            nlr_pop();
        } else {
            // Uncaught exception; disable the callback so it doesn't run again.
            pyb_can_rxcallback(self, MP_OBJ_NEW_SMALL_INT(fifo_id), mp_const_none);
            printf("uncaught exception in CAN(%u) rx interrupt handler\n", self->can_id);
            mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
        }
        gc_unlock();
        mp_sched_unlock();
    }
}

STATIC const mp_stream_p_t can_stream_p = {
    //.read = can_read, // is read sensible for CAN?
    //.write = can_write, // is write sensible for CAN?
    .ioctl = can_ioctl,
    .is_text = false,
};

const mp_obj_type_t pyb_can_type = {
    { &mp_type_type },
    .name = MP_QSTR_CAN,
    .print = pyb_can_print,
    .make_new = pyb_can_make_new,
    .protocol = &can_stream_p,
    .locals_dict = (mp_obj_t)&pyb_can_locals_dict,
};

#endif // MICROPY_HW_ENABLE_CAN
