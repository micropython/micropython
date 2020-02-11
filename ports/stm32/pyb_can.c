/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 Damien P. George
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

#include <string.h>

#include "py/objarray.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/binary.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "bufhelper.h"
#include "can.h"
#include "irq.h"

#if MICROPY_HW_ENABLE_CAN

#if MICROPY_HW_ENABLE_FDCAN

#define CAN_MAX_FILTER              (64)

#define CAN_FIFO0                   FDCAN_RX_FIFO0
#define CAN_FIFO1                   FDCAN_RX_FIFO1
#define CAN_FILTER_FIFO0            (0)

// Default timings; 125Kbps assuming 48MHz clock
#define CAN_DEFAULT_PRESCALER       (32)
#define CAN_DEFAULT_SJW             (1)
#define CAN_DEFAULT_BS1             (8)
#define CAN_DEFAULT_BS2             (3)

#define CAN_MODE_NORMAL             FDCAN_MODE_NORMAL
#define CAN_MODE_LOOPBACK           FDCAN_MODE_EXTERNAL_LOOPBACK
#define CAN_MODE_SILENT             FDCAN_MODE_BUS_MONITORING
#define CAN_MODE_SILENT_LOOPBACK    FDCAN_MODE_INTERNAL_LOOPBACK

#define CAN1_RX0_IRQn               FDCAN1_IT0_IRQn
#define CAN1_RX1_IRQn               FDCAN1_IT1_IRQn
#define CAN2_RX0_IRQn               FDCAN2_IT0_IRQn
#define CAN2_RX1_IRQn               FDCAN2_IT1_IRQn

#define CAN_IT_FIFO0_FULL           FDCAN_IT_RX_FIFO0_FULL
#define CAN_IT_FIFO1_FULL           FDCAN_IT_RX_FIFO1_FULL
#define CAN_IT_FIFO0_OVRF           FDCAN_IT_RX_FIFO0_MESSAGE_LOST
#define CAN_IT_FIFO1_OVRF           FDCAN_IT_RX_FIFO1_MESSAGE_LOST
#define CAN_IT_FIFO0_PENDING        FDCAN_IT_RX_FIFO0_NEW_MESSAGE
#define CAN_IT_FIFO1_PENDING        FDCAN_IT_RX_FIFO1_NEW_MESSAGE
#define CAN_FLAG_FIFO0_FULL         FDCAN_FLAG_RX_FIFO0_FULL
#define CAN_FLAG_FIFO1_FULL         FDCAN_FLAG_RX_FIFO1_FULL
#define CAN_FLAG_FIFO0_OVRF         FDCAN_FLAG_RX_FIFO0_MESSAGE_LOST
#define CAN_FLAG_FIFO1_OVRF         FDCAN_FLAG_RX_FIFO1_MESSAGE_LOST

#define __HAL_CAN_ENABLE_IT         __HAL_FDCAN_ENABLE_IT
#define __HAL_CAN_DISABLE_IT        __HAL_FDCAN_DISABLE_IT
#define __HAL_CAN_CLEAR_FLAG        __HAL_FDCAN_CLEAR_FLAG
#define __HAL_CAN_MSG_PENDING       HAL_FDCAN_GetRxFifoFillLevel

// Both banks start at 0
STATIC uint8_t can2_start_bank = 0;

#else

#define CAN_MAX_FILTER              (28)

#define CAN_DEFAULT_PRESCALER       (100)
#define CAN_DEFAULT_SJW             (1)
#define CAN_DEFAULT_BS1             (6)
#define CAN_DEFAULT_BS2             (8)

#define CAN_IT_FIFO0_FULL           CAN_IT_FF0
#define CAN_IT_FIFO1_FULL           CAN_IT_FF1
#define CAN_IT_FIFO0_OVRF           CAN_IT_FOV0
#define CAN_IT_FIFO1_OVRF           CAN_IT_FOV1
#define CAN_IT_FIFO0_PENDING        CAN_IT_FMP0
#define CAN_IT_FIFO1_PENDING        CAN_IT_FMP1
#define CAN_FLAG_FIFO0_FULL         CAN_FLAG_FF0
#define CAN_FLAG_FIFO1_FULL         CAN_FLAG_FF1
#define CAN_FLAG_FIFO0_OVRF         CAN_FLAG_FOV0
#define CAN_FLAG_FIFO1_OVRF         CAN_FLAG_FOV1

STATIC uint8_t can2_start_bank = 14;

#endif

STATIC void pyb_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_enabled) {
        mp_printf(print, "CAN(%u)", self->can_id);
    } else {
        qstr mode;
        switch (self->can.Init.Mode) {
            case CAN_MODE_NORMAL: mode = MP_QSTR_NORMAL; break;
            case CAN_MODE_LOOPBACK: mode = MP_QSTR_LOOPBACK; break;
            case CAN_MODE_SILENT: mode = MP_QSTR_SILENT; break;
            case CAN_MODE_SILENT_LOOPBACK: default: mode = MP_QSTR_SILENT_LOOPBACK; break;
        }
        mp_printf(print, "CAN(%u, CAN.%q, extframe=%q, auto_restart=%q)",
            self->can_id,
            mode,
            self->extframe ? MP_QSTR_True : MP_QSTR_False,
            #if MICROPY_HW_ENABLE_FDCAN
            (self->can.Instance->CCCR & FDCAN_CCCR_DAR) ? MP_QSTR_True : MP_QSTR_False
            #else
            (self->can.Instance->MCR & CAN_MCR_ABOM) ? MP_QSTR_True : MP_QSTR_False
            #endif
        );
    }
}

// init(mode, extframe=False, prescaler=100, *, sjw=1, bs1=6, bs2=8)
STATIC mp_obj_t pyb_can_init_helper(pyb_can_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_extframe, ARG_prescaler, ARG_sjw, ARG_bs1, ARG_bs2, ARG_auto_restart };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int  = CAN_MODE_NORMAL} },
        { MP_QSTR_extframe,     MP_ARG_BOOL,                    {.u_bool = false} },
        { MP_QSTR_prescaler,    MP_ARG_INT,                     {.u_int  = CAN_DEFAULT_PRESCALER} },
        { MP_QSTR_sjw,          MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int  = CAN_DEFAULT_SJW} },
        { MP_QSTR_bs1,          MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int  = CAN_DEFAULT_BS1} },
        { MP_QSTR_bs2,          MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int  = CAN_DEFAULT_BS2} },
        { MP_QSTR_auto_restart, MP_ARG_KW_ONLY | MP_ARG_BOOL,   {.u_bool = false} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->extframe = args[ARG_extframe].u_bool;

    // set the CAN configuration values
    memset(&self->can, 0, sizeof(self->can));

    // init CAN (if it fails, it's because the port doesn't exist)
    if (!can_init(self, args[ARG_mode].u_int, args[ARG_prescaler].u_int, args[ARG_sjw].u_int,
        args[ARG_bs1].u_int, args[ARG_bs2].u_int, args[ARG_auto_restart].u_bool)) {
        mp_raise_msg_varg(&mp_type_ValueError, "CAN(%d) doesn't exist", self->can_id);
    }

    return mp_const_none;
}

// CAN(bus, ...)
STATIC mp_obj_t pyb_can_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out port
    mp_uint_t can_idx;
    if (mp_obj_is_str(args[0])) {
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
        #ifdef MICROPY_HW_CAN3_NAME
        } else if (strcmp(port, MICROPY_HW_CAN3_NAME) == 0) {
            can_idx = PYB_CAN_3;
        #endif
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, "CAN(%s) doesn't exist", port);
        }
    } else {
        can_idx = mp_obj_get_int(args[0]);
    }
    if (can_idx < 1 || can_idx > MP_ARRAY_SIZE(MP_STATE_PORT(pyb_can_obj_all))) {
        mp_raise_msg_varg(&mp_type_ValueError, "CAN(%d) doesn't exist", can_idx);
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
            can_deinit(self);
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

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_can_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_can_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_init_obj, 1, pyb_can_init);

// deinit()
STATIC mp_obj_t pyb_can_deinit(mp_obj_t self_in) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    can_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_can_deinit_obj, pyb_can_deinit);

// Force a software restart of the controller, to allow transmission after a bus error
STATIC mp_obj_t pyb_can_restart(mp_obj_t self_in) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_enabled) {
        mp_raise_ValueError(NULL);
    }
    CAN_TypeDef *can = self->can.Instance;
    #if MICROPY_HW_ENABLE_FDCAN
    can->CCCR |= FDCAN_CCCR_INIT;
    while ((can->CCCR & FDCAN_CCCR_INIT) == 0) {
    }
    can->CCCR &= ~FDCAN_CCCR_INIT;
    while ((can->CCCR & FDCAN_CCCR_INIT)) {
    }
    #else
    can->MCR |= CAN_MCR_INRQ;
    while ((can->MSR & CAN_MSR_INAK) == 0) {
    }
    can->MCR &= ~CAN_MCR_INRQ;
    while ((can->MSR & CAN_MSR_INAK)) {
    }
    #endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_can_restart_obj, pyb_can_restart);

// Get the state of the controller
STATIC mp_obj_t pyb_can_state(mp_obj_t self_in) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t state = CAN_STATE_STOPPED;
    if (self->is_enabled) {
        CAN_TypeDef *can = self->can.Instance;
        #if MICROPY_HW_ENABLE_FDCAN
        if (can->PSR & FDCAN_PSR_BO) {
            state = CAN_STATE_BUS_OFF;
        } else if (can->PSR & FDCAN_PSR_EP) {
            state = CAN_STATE_ERROR_PASSIVE;
        } else if (can->PSR & FDCAN_PSR_EW) {
            state = CAN_STATE_ERROR_WARNING;
        } else {
            state = CAN_STATE_ERROR_ACTIVE;
        }
        #else
        if (can->ESR & CAN_ESR_BOFF) {
            state = CAN_STATE_BUS_OFF;
        } else if (can->ESR & CAN_ESR_EPVF) {
            state = CAN_STATE_ERROR_PASSIVE;
        } else if (can->ESR & CAN_ESR_EWGF) {
            state = CAN_STATE_ERROR_WARNING;
        } else {
            state = CAN_STATE_ERROR_ACTIVE;
        }
        #endif
    }
    return MP_OBJ_NEW_SMALL_INT(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_can_state_obj, pyb_can_state);

// Get info about error states and TX/RX buffers
STATIC mp_obj_t pyb_can_info(size_t n_args, const mp_obj_t *args) {
    #if MICROPY_HW_ENABLE_FDCAN
    // TODO implement for FDCAN
    return mp_const_none;
    #else
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_list_t *list;
    if (n_args == 1) {
        list = MP_OBJ_TO_PTR(mp_obj_new_list(8, NULL));
    } else {
        if (!mp_obj_is_type(args[1], &mp_type_list)) {
            mp_raise_TypeError(NULL);
        }
        list = MP_OBJ_TO_PTR(args[1]);
        if (list->len < 8) {
            mp_raise_ValueError(NULL);
        }
    }
    CAN_TypeDef *can = self->can.Instance;
    uint32_t esr = can->ESR;
    list->items[0] = MP_OBJ_NEW_SMALL_INT(esr >> CAN_ESR_TEC_Pos & 0xff);
    list->items[1] = MP_OBJ_NEW_SMALL_INT(esr >> CAN_ESR_REC_Pos & 0xff);
    list->items[2] = MP_OBJ_NEW_SMALL_INT(self->num_error_warning);
    list->items[3] = MP_OBJ_NEW_SMALL_INT(self->num_error_passive);
    list->items[4] = MP_OBJ_NEW_SMALL_INT(self->num_bus_off);
    int n_tx_pending = 0x01121223 >> ((can->TSR >> CAN_TSR_TME_Pos & 7) << 2) & 0xf;
    list->items[5] = MP_OBJ_NEW_SMALL_INT(n_tx_pending);
    list->items[6] = MP_OBJ_NEW_SMALL_INT(can->RF0R >> CAN_RF0R_FMP0_Pos & 3);
    list->items[7] = MP_OBJ_NEW_SMALL_INT(can->RF1R >> CAN_RF1R_FMP1_Pos & 3);
    return MP_OBJ_FROM_PTR(list);
    #endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_can_info_obj, 1, 2, pyb_can_info);

// any(fifo) - return `True` if any message waiting on the FIFO, else `False`
STATIC mp_obj_t pyb_can_any(mp_obj_t self_in, mp_obj_t fifo_in) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
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

// send(send, addr, *, timeout=5000)
STATIC mp_obj_t pyb_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_id, ARG_timeout, ARG_rtr };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_id,      MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_rtr,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[ARG_data].u_obj, &bufinfo, data);

    if (bufinfo.len > 8) {
        mp_raise_ValueError("CAN data field too long");
    }

    // send the data
    CanTxMsgTypeDef tx_msg;

    #if MICROPY_HW_ENABLE_FDCAN
    uint8_t tx_data[8];
    tx_msg.MessageMarker = 0;
    tx_msg.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_msg.BitRateSwitch = FDCAN_BRS_OFF;
    tx_msg.FDFormat = FDCAN_CLASSIC_CAN;
    tx_msg.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_msg.DataLength = (bufinfo.len << 16); // TODO DLC for len > 8

    if (self->extframe) {
        tx_msg.Identifier = args[ARG_id].u_int & 0x1FFFFFFF;
        tx_msg.IdType = FDCAN_EXTENDED_ID;
    } else {
        tx_msg.Identifier = args[ARG_id].u_int & 0x7FF;
        tx_msg.IdType = FDCAN_STANDARD_ID;
    }
    if (args[ARG_rtr].u_bool == false) {
        tx_msg.TxFrameType = FDCAN_DATA_FRAME;
    } else  {
        tx_msg.TxFrameType = FDCAN_REMOTE_FRAME;
    }
    #else
    tx_msg.DLC = bufinfo.len;
    uint8_t *tx_data = tx_msg.Data; // Data is uint32_t but holds only 1 byte

    if (self->extframe) {
        tx_msg.ExtId = args[ARG_id].u_int & 0x1FFFFFFF;
        tx_msg.IDE = CAN_ID_EXT;
    } else {
        tx_msg.StdId = args[ARG_id].u_int & 0x7FF;
        tx_msg.IDE = CAN_ID_STD;
    }
    if (args[ARG_rtr].u_bool == false) {
        tx_msg.RTR = CAN_RTR_DATA;
    } else  {
        tx_msg.RTR = CAN_RTR_REMOTE;
    }
    #endif

    for (mp_uint_t i = 0; i < bufinfo.len; i++) {
        tx_data[i] = ((byte*)bufinfo.buf)[i];
    }

    HAL_StatusTypeDef status;
    #if MICROPY_HW_ENABLE_FDCAN
    status = HAL_FDCAN_AddMessageToTxFifoQ(&self->can, &tx_msg, tx_data);
    #else
    self->can.pTxMsg = &tx_msg;
    status = CAN_Transmit(&self->can, args[ARG_timeout].u_int);
    #endif

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_send_obj, 1, pyb_can_send);

// recv(fifo, list=None, *, timeout=5000)
STATIC mp_obj_t pyb_can_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_fifo, ARG_list, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_fifo,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_list,    MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // receive the data
    CanRxMsgTypeDef rx_msg;
    #if MICROPY_HW_ENABLE_FDCAN
    uint8_t rx_data[8];
    #else
    uint8_t *rx_data = rx_msg.Data;
    #endif

    mp_uint_t fifo = args[ARG_fifo].u_int;
    if (fifo == 0) {
        fifo = CAN_FIFO0;
    } else if (fifo == 1) {
        fifo = CAN_FIFO1;
    } else {
        mp_raise_TypeError(NULL);
    }

    int ret = can_receive(&self->can, fifo, &rx_msg, rx_data, args[ARG_timeout].u_int);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    #if MICROPY_HW_ENABLE_FDCAN
    uint32_t rx_dlc = rx_msg.DataLength;
    #else
    uint32_t rx_dlc = rx_msg.DLC;
    #endif

    // Manage the rx state machine
    if ((fifo == CAN_FIFO0 && self->rxcallback0 != mp_const_none) ||
        (fifo == CAN_FIFO1 && self->rxcallback1 != mp_const_none)) {
        byte *state = (fifo == CAN_FIFO0) ? &self->rx_state0 : &self->rx_state1;

        switch (*state) {
        case RX_STATE_FIFO_EMPTY:
            break;
        case RX_STATE_MESSAGE_PENDING:
            if (__HAL_CAN_MSG_PENDING(&self->can, fifo) == 0) {
                // Fifo is empty
                __HAL_CAN_ENABLE_IT(&self->can, (fifo == CAN_FIFO0) ? CAN_IT_FIFO0_PENDING : CAN_IT_FIFO1_PENDING);
                *state = RX_STATE_FIFO_EMPTY;
            }
            break;
        case RX_STATE_FIFO_FULL:
            __HAL_CAN_ENABLE_IT(&self->can, (fifo == CAN_FIFO0) ? CAN_IT_FIFO0_FULL : CAN_IT_FIFO1_FULL);
            *state = RX_STATE_MESSAGE_PENDING;
            break;
        case RX_STATE_FIFO_OVERFLOW:
            __HAL_CAN_ENABLE_IT(&self->can, (fifo == CAN_FIFO0) ? CAN_IT_FIFO0_OVRF : CAN_IT_FIFO1_OVRF);
            __HAL_CAN_ENABLE_IT(&self->can, (fifo == CAN_FIFO0) ? CAN_IT_FIFO0_FULL : CAN_IT_FIFO1_FULL);
            *state = RX_STATE_MESSAGE_PENDING;
            break;
        }
    }

    // Create the tuple, or get the list, that will hold the return values
    // Also populate the fourth element, either a new bytes or reuse existing memoryview
    mp_obj_t ret_obj = args[ARG_list].u_obj;
    mp_obj_t *items;
    if (ret_obj == mp_const_none) {
        ret_obj = mp_obj_new_tuple(4, NULL);
        items = ((mp_obj_tuple_t*)MP_OBJ_TO_PTR(ret_obj))->items;
        items[3] = mp_obj_new_bytes(rx_data, rx_dlc);
    } else {
        // User should provide a list of length at least 4 to hold the values
        if (!mp_obj_is_type(ret_obj, &mp_type_list)) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_list_t *list = MP_OBJ_TO_PTR(ret_obj);
        if (list->len < 4) {
            mp_raise_ValueError(NULL);
        }
        items = list->items;
        // Fourth element must be a memoryview which we assume points to a
        // byte-like array which is large enough, and then we resize it inplace
        if (!mp_obj_is_type(items[3], &mp_type_memoryview)) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_array_t *mv = MP_OBJ_TO_PTR(items[3]);
        if (!(mv->typecode == (MP_OBJ_ARRAY_TYPECODE_FLAG_RW | BYTEARRAY_TYPECODE)
            || (mv->typecode | 0x20) == (MP_OBJ_ARRAY_TYPECODE_FLAG_RW | 'b'))) {
            mp_raise_ValueError(NULL);
        }
        mv->len = rx_dlc;
        memcpy(mv->items, rx_data, rx_dlc);
    }

    // Populate the first 3 values of the tuple/list
    #if MICROPY_HW_ENABLE_FDCAN
    items[0] = MP_OBJ_NEW_SMALL_INT(rx_msg.Identifier);
    items[1] = rx_msg.RxFrameType == FDCAN_REMOTE_FRAME ? mp_const_true : mp_const_false;
    items[2] = MP_OBJ_NEW_SMALL_INT(rx_msg.FilterIndex);
    #else
    items[0] = MP_OBJ_NEW_SMALL_INT((rx_msg.IDE == CAN_ID_STD ? rx_msg.StdId : rx_msg.ExtId));
    items[1] = rx_msg.RTR == CAN_RTR_REMOTE ? mp_const_true : mp_const_false;
    items[2] = MP_OBJ_NEW_SMALL_INT(rx_msg.FMI);
    #endif

    // Return the result
    return ret_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_recv_obj, 1, pyb_can_recv);

// initfilterbanks(n)
STATIC mp_obj_t pyb_can_initfilterbanks(mp_obj_t self, mp_obj_t bank_in) {
    #if MICROPY_HW_ENABLE_FDCAN
    can2_start_bank = 0;
    #else
    can2_start_bank = mp_obj_get_int(bank_in);
    #endif

    for (int f = 0; f < CAN_MAX_FILTER; f++) {
        can_clearfilter(MP_OBJ_TO_PTR(self), f, can2_start_bank);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_can_initfilterbanks_fun_obj, pyb_can_initfilterbanks);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pyb_can_initfilterbanks_obj, MP_ROM_PTR(&pyb_can_initfilterbanks_fun_obj));

STATIC mp_obj_t pyb_can_clearfilter(mp_obj_t self_in, mp_obj_t bank_in) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t f = mp_obj_get_int(bank_in);
    if (self->can_id == 2) {
        f += can2_start_bank;
    }
    can_clearfilter(self, f, can2_start_bank);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_can_clearfilter_obj, pyb_can_clearfilter);

// setfilter(bank, mode, fifo, params, *, rtr)
#define EXTENDED_ID_TO_16BIT_FILTER(id) (((id & 0xC00000) >> 13) | ((id & 0x38000) >> 15)) | 8
STATIC mp_obj_t pyb_can_setfilter(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bank, ARG_mode, ARG_fifo, ARG_params, ARG_rtr };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bank,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_fifo,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = CAN_FILTER_FIFO0} },
        { MP_QSTR_params,   MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rtr,      MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if MICROPY_HW_ENABLE_FDCAN
    FDCAN_FilterTypeDef filter = {0};
    filter.IdType = FDCAN_STANDARD_ID;
    // TODO check filter index
    filter.FilterIndex = args[ARG_bank].u_int;

    // Check filter mode
    if (((args[ARG_mode].u_int != FDCAN_FILTER_RANGE) &&
         (args[ARG_mode].u_int != FDCAN_FILTER_DUAL)  &&
         (args[ARG_mode].u_int != FDCAN_FILTER_MASK))) {
        goto error;
    }

    // Check FIFO index.
    if (args[ARG_fifo].u_int == 0) {
        filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    } else if (args[ARG_fifo].u_int == 1) {
        filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
    } else {
        goto error;
    }

    size_t len;
    mp_obj_t *params;
    mp_obj_get_array(args[ARG_params].u_obj, &len, &params);
    if (len != 2) { // Check params len
        goto error;
    }
    filter.FilterID1 = mp_obj_get_int(params[0]);
    filter.FilterID2 = mp_obj_get_int(params[1]);
    filter.FilterType = args[ARG_mode].u_int;
    HAL_FDCAN_ConfigFilter(&self->can, &filter);

    #else

    size_t len;
    size_t rtr_len;
    mp_uint_t rtr_masks[4] = {0, 0, 0, 0};
    mp_obj_t *rtr_flags;
    mp_obj_t *params;
    mp_obj_get_array(args[ARG_params].u_obj, &len, &params);
    if (args[ARG_rtr].u_obj != MP_OBJ_NULL){
        mp_obj_get_array(args[ARG_rtr].u_obj, &rtr_len, &rtr_flags);
    }

    CAN_FilterConfTypeDef filter;
    if (args[ARG_mode].u_int == MASK16 || args[ARG_mode].u_int == LIST16) {
        if (len != 4) {
            goto error;
        }
        filter.FilterScale = CAN_FILTERSCALE_16BIT;
        if (self->extframe) {
            if (args[ARG_rtr].u_obj != MP_OBJ_NULL) {
                if (args[ARG_mode].u_int == MASK16) {
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
            if (args[ARG_rtr].u_obj != MP_OBJ_NULL) {
                if (args[ARG_mode].u_int == MASK16) {
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
        if (args[ARG_mode].u_int == MASK16) {
            filter.FilterMode  = CAN_FILTERMODE_IDMASK;
        }
        if (args[ARG_mode].u_int == LIST16) {
            filter.FilterMode  = CAN_FILTERMODE_IDLIST;
        }
    }
    else if (args[ARG_mode].u_int == MASK32 || args[ARG_mode].u_int == LIST32) {
        if (len != 2) {
            goto error;
        }
        filter.FilterScale = CAN_FILTERSCALE_32BIT;
        if (args[ARG_rtr].u_obj != MP_OBJ_NULL) {
            if (args[ARG_mode].u_int == MASK32) {
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
        if (args[ARG_mode].u_int == MASK32) {
            filter.FilterMode  = CAN_FILTERMODE_IDMASK;
        }
        if (args[ARG_mode].u_int == LIST32) {
            filter.FilterMode  = CAN_FILTERMODE_IDLIST;
        }
    } else {
        goto error;
    }

    filter.FilterFIFOAssignment = args[ARG_fifo].u_int;
    filter.FilterNumber = args[ARG_bank].u_int;
    if (self->can_id == 1) {
        if (filter.FilterNumber >= can2_start_bank) {
            goto error;
        }
    } else if (self->can_id == 2) {
        filter.FilterNumber = filter.FilterNumber + can2_start_bank;
        if (filter.FilterNumber > 27) {
            goto error;
        }
    } else {
        if (filter.FilterNumber > 13) { // CAN3 is independant and has its own 14 filters.
            goto error;
        }
    }
    filter.FilterActivation = ENABLE;
    filter.BankNumber = can2_start_bank;
    HAL_CAN_ConfigFilter(&self->can, &filter);
    #endif

    return mp_const_none;
error:
    mp_raise_ValueError("CAN filter parameter error");
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_setfilter_obj, 1, pyb_can_setfilter);

STATIC mp_obj_t pyb_can_rxcallback(mp_obj_t self_in, mp_obj_t fifo_in, mp_obj_t callback_in) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t fifo = mp_obj_get_int(fifo_in);
    mp_obj_t *callback;

    callback = (fifo == 0) ? &self->rxcallback0 : &self->rxcallback1;
    if (callback_in == mp_const_none) {
        __HAL_CAN_DISABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FIFO0_PENDING : CAN_IT_FIFO1_PENDING);
        __HAL_CAN_DISABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FIFO0_FULL : CAN_IT_FIFO1_FULL);
        __HAL_CAN_DISABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FIFO0_OVRF : CAN_IT_FIFO1_OVRF);
        __HAL_CAN_CLEAR_FLAG(&self->can, (fifo == CAN_FIFO0) ? CAN_FLAG_FIFO0_FULL : CAN_FLAG_FIFO1_FULL);
        __HAL_CAN_CLEAR_FLAG(&self->can, (fifo == CAN_FIFO0) ? CAN_FLAG_FIFO0_OVRF : CAN_FLAG_FIFO1_OVRF);
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
        } else if (self->can_id == PYB_CAN_2) {
            irq = (fifo == 0) ? CAN2_RX0_IRQn : CAN2_RX1_IRQn;
        #endif
        #if defined(CAN3)
        } else {
            irq = (fifo == 0) ? CAN3_RX0_IRQn : CAN3_RX1_IRQn;
        #endif
        }
        NVIC_SetPriority(irq, IRQ_PRI_CAN);
        HAL_NVIC_EnableIRQ(irq);
        __HAL_CAN_ENABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FIFO0_PENDING : CAN_IT_FIFO1_PENDING);
        __HAL_CAN_ENABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FIFO0_FULL : CAN_IT_FIFO1_FULL);
        __HAL_CAN_ENABLE_IT(&self->can, (fifo == 0) ? CAN_IT_FIFO0_OVRF : CAN_IT_FIFO1_OVRF);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_can_rxcallback_obj, pyb_can_rxcallback);

STATIC const mp_rom_map_elem_t pyb_can_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_can_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&pyb_can_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_restart), MP_ROM_PTR(&pyb_can_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_state), MP_ROM_PTR(&pyb_can_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&pyb_can_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&pyb_can_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&pyb_can_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&pyb_can_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_initfilterbanks), MP_ROM_PTR(&pyb_can_initfilterbanks_obj) },
    { MP_ROM_QSTR(MP_QSTR_setfilter), MP_ROM_PTR(&pyb_can_setfilter_obj) },
    { MP_ROM_QSTR(MP_QSTR_clearfilter), MP_ROM_PTR(&pyb_can_clearfilter_obj) },
    { MP_ROM_QSTR(MP_QSTR_rxcallback), MP_ROM_PTR(&pyb_can_rxcallback_obj) },

    #if MICROPY_HW_ENABLE_FDCAN
    { MP_ROM_QSTR(MP_QSTR_NORMAL), MP_ROM_INT(CAN_MODE_NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_LOOPBACK), MP_ROM_INT(CAN_MODE_LOOPBACK) },
    { MP_ROM_QSTR(MP_QSTR_SILENT), MP_ROM_INT(CAN_MODE_SILENT) },
    { MP_ROM_QSTR(MP_QSTR_SILENT_LOOPBACK), MP_ROM_INT(CAN_MODE_SILENT_LOOPBACK) },

    { MP_ROM_QSTR(MP_QSTR_RANGE), MP_ROM_INT(FDCAN_FILTER_RANGE) },
    { MP_ROM_QSTR(MP_QSTR_DUAL), MP_ROM_INT(FDCAN_FILTER_DUAL) },
    { MP_ROM_QSTR(MP_QSTR_MASK), MP_ROM_INT(FDCAN_FILTER_MASK) },
    #else
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
    #endif

    // values for CAN.state()
    { MP_ROM_QSTR(MP_QSTR_STOPPED), MP_ROM_INT(CAN_STATE_STOPPED) },
    { MP_ROM_QSTR(MP_QSTR_ERROR_ACTIVE), MP_ROM_INT(CAN_STATE_ERROR_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_ERROR_WARNING), MP_ROM_INT(CAN_STATE_ERROR_WARNING) },
    { MP_ROM_QSTR(MP_QSTR_ERROR_PASSIVE), MP_ROM_INT(CAN_STATE_ERROR_PASSIVE) },
    { MP_ROM_QSTR(MP_QSTR_BUS_OFF), MP_ROM_INT(CAN_STATE_BUS_OFF) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_can_locals_dict, pyb_can_locals_dict_table);

STATIC mp_uint_t can_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD)
            && ((__HAL_CAN_MSG_PENDING(&self->can, CAN_FIFO0) != 0)
                || (__HAL_CAN_MSG_PENDING(&self->can, CAN_FIFO1) != 0))) {
            ret |= MP_STREAM_POLL_RD;
        }
        #if MICROPY_HW_ENABLE_FDCAN
        if ((flags & MP_STREAM_POLL_WR) && (self->can.Instance->IR & FDCAN_IR_TFE))
        #else
        if ((flags & MP_STREAM_POLL_WR) && (self->can.Instance->TSR & CAN_TSR_TME))
        #endif
        {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = -1;
    }
    return ret;
}

void pyb_can_handle_callback(pyb_can_obj_t *self, uint fifo_id, mp_obj_t callback, mp_obj_t irq_reason) {
    if (callback != mp_const_none) {
        mp_sched_lock();
        gc_lock();
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_call_function_2(callback, MP_OBJ_FROM_PTR(self), irq_reason);
            nlr_pop();
        } else {
            // Uncaught exception; disable the callback so it doesn't run again.
            pyb_can_rxcallback(MP_OBJ_FROM_PTR(self), MP_OBJ_NEW_SMALL_INT(fifo_id), mp_const_none);
            mp_printf(MICROPY_ERROR_PRINTER, "uncaught exception in CAN(%u) rx interrupt handler\n", self->can_id);
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
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
    .locals_dict = (mp_obj_dict_t*)&pyb_can_locals_dict,
};

#endif // MICROPY_HW_ENABLE_CAN
