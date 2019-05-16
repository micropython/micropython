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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "py/objtuple.h"
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

#if MICROPY_HW_ENABLE_FDCAN

#define FDCAN_ELEMENT_MASK_STDID ((uint32_t)0x1FFC0000U) /* Standard Identifier         */
#define FDCAN_ELEMENT_MASK_EXTID ((uint32_t)0x1FFFFFFFU) /* Extended Identifier         */
#define FDCAN_ELEMENT_MASK_RTR   ((uint32_t)0x20000000U) /* Remote Transmission Request */
#define FDCAN_ELEMENT_MASK_XTD   ((uint32_t)0x40000000U) /* Extended Identifier         */
#define FDCAN_ELEMENT_MASK_ESI   ((uint32_t)0x80000000U) /* Error State Indicator       */
#define FDCAN_ELEMENT_MASK_TS    ((uint32_t)0x0000FFFFU) /* Timestamp                   */
#define FDCAN_ELEMENT_MASK_DLC   ((uint32_t)0x000F0000U) /* Data Length Code            */
#define FDCAN_ELEMENT_MASK_BRS   ((uint32_t)0x00100000U) /* Bit Rate Switch             */
#define FDCAN_ELEMENT_MASK_FDF   ((uint32_t)0x00200000U) /* FD Format                   */
#define FDCAN_ELEMENT_MASK_EFC   ((uint32_t)0x00800000U) /* Event FIFO Control          */
#define FDCAN_ELEMENT_MASK_MM    ((uint32_t)0xFF000000U) /* Message Marker              */
#define FDCAN_ELEMENT_MASK_FIDX  ((uint32_t)0x7F000000U) /* Filter Index                */
#define FDCAN_ELEMENT_MASK_ANMF  ((uint32_t)0x80000000U) /* Accepted Non-matching Frame */
#define FDCAN_ELEMENT_MASK_ET    ((uint32_t)0x00C00000U) /* Event type                  */

enum {
    CAN_STATE_STOPPED,
    CAN_STATE_ERROR_ACTIVE,
    CAN_STATE_ERROR_WARNING,
    CAN_STATE_ERROR_PASSIVE,
    CAN_STATE_BUS_OFF,
};

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
///     import pyb
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
    mp_uint_t fdcan_id : 8;
    bool is_enabled : 1;
    bool extframe : 1;
    byte rx_state0;
    byte rx_state1;
    uint16_t num_error_warning;
    uint16_t num_error_passive;
    uint16_t num_bus_off;
    FDCAN_HandleTypeDef fdcan;
} pyb_can_obj_t;

STATIC mp_obj_t pyb_can_deinit(mp_obj_t self_in);
STATIC void can_clearfilter(FDCAN_HandleTypeDef *fdcan, uint32_t f);

// assumes Init parameters have been set up correctly
STATIC bool can_init(pyb_can_obj_t *can_obj) {
    FDCAN_GlobalTypeDef *CANx = NULL;
    const pin_obj_t *pins[2];

    switch (can_obj->fdcan_id) {
        #if defined(MICROPY_HW_FDCAN1_TX)
        case PYB_CAN_1:
            CANx = FDCAN1;
            pins[0] = MICROPY_HW_FDCAN1_TX;
            pins[1] = MICROPY_HW_FDCAN1_RX;
            break;
        #endif

        #if defined(MICROPY_HW_FDCAN2_TX)
        case PYB_CAN_2:
            CANx = FDCAN2;
            pins[0] = MICROPY_HW_FDCAN2_TX;
            pins[1] = MICROPY_HW_FDCAN2_RX;
            break;
        #endif

        default:
            return false;
    }

    // Enable FDCAN clock
    __HAL_RCC_FDCAN_CLK_ENABLE();

    // init GPIO
    uint32_t mode = MP_HAL_PIN_MODE_ALT;
    uint32_t pull = MP_HAL_PIN_PULL_UP;
    for (int i = 0; i < 2; i++) {
        if (!mp_hal_pin_config_alt(pins[i], mode, pull, AF_FN_FDCAN, can_obj->fdcan_id)) {
            return false;
        }
    }

    // init CANx
    can_obj->fdcan.Instance = CANx;

    HAL_FDCAN_Init(&can_obj->fdcan);
    // The configuration registers are locked after CAN is started.
    HAL_FDCAN_Start(&can_obj->fdcan);

    // Reset all filters.
    for (int f=0; f<64; f++) {
        can_clearfilter(&can_obj->fdcan, f);
    }

    can_obj->is_enabled = true;
    can_obj->num_error_warning = 0;
    can_obj->num_error_passive = 0;
    can_obj->num_bus_off = 0;

    switch (can_obj->fdcan_id) {
        case PYB_CAN_1:
            NVIC_SetPriority(FDCAN1_IT0_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
            NVIC_SetPriority(FDCAN1_IT1_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
            break;
        case PYB_CAN_2:
            NVIC_SetPriority(FDCAN2_IT0_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
            NVIC_SetPriority(FDCAN2_IT1_IRQn, IRQ_PRI_CAN);
            HAL_NVIC_EnableIRQ(FDCAN2_IT1_IRQn);
            break;
        default:
            return false;
    }

    __HAL_FDCAN_ENABLE_IT(&can_obj->fdcan, FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE);
    __HAL_FDCAN_ENABLE_IT(&can_obj->fdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
    __HAL_FDCAN_ENABLE_IT(&can_obj->fdcan, FDCAN_IT_RX_FIFO0_MESSAGE_LOST | FDCAN_IT_RX_FIFO1_MESSAGE_LOST);
    __HAL_FDCAN_ENABLE_IT(&can_obj->fdcan, FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO1_FULL);
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

STATIC void can_clearfilter(FDCAN_HandleTypeDef *fdcan, uint32_t f) {
    FDCAN_FilterTypeDef filter = {0};
    filter.IdType = FDCAN_STANDARD_ID;
    filter.FilterIndex = f;
    filter.FilterConfig = FDCAN_FILTER_DISABLE;
    HAL_FDCAN_ConfigFilter(fdcan, &filter);
}

STATIC int can_receive(FDCAN_HandleTypeDef *fdcan, int fifo, FDCAN_RxHeaderTypeDef *hdr, uint8_t *data, uint32_t timeout_ms) {
    volatile uint32_t *rxf, *rxa;
    if(fifo == FDCAN_RX_FIFO0) {
        rxf = &fdcan->Instance->RXF0S;
        rxa = &fdcan->Instance->RXF0A;
    } else {
        rxf = &fdcan->Instance->RXF1S;
        rxa = &fdcan->Instance->RXF1A;
    }

    // Wait for a message to become available, with timeout
    uint32_t start = HAL_GetTick();
    while ((*rxf & 7) == 0) {
        MICROPY_EVENT_POLL_HOOK
        if (HAL_GetTick() - start >= timeout_ms) {
            return -MP_ETIMEDOUT;
        }
    }

    // Read message data
    uint32_t index = 0;
    uint32_t *address;

    /* Calculate Rx FIFO 0 element address */
    index = ((fdcan->Instance->RXF0S & FDCAN_RXF0S_F0GI) >> 8);
    address = (uint32_t *)(fdcan->msgRam.RxFIFO0SA + (index * fdcan->Init.RxFifo0ElmtSize * 4));
    hdr->IdType = *address & FDCAN_ELEMENT_MASK_XTD;
    if(hdr->IdType == FDCAN_STANDARD_ID) {
      hdr->Identifier = ((*address & FDCAN_ELEMENT_MASK_STDID) >> 18);
    } else {
      hdr->Identifier = (*address & FDCAN_ELEMENT_MASK_EXTID);
    }
    hdr->RxFrameType = (*address & FDCAN_ELEMENT_MASK_RTR);
    hdr->ErrorStateIndicator = (*address++ & FDCAN_ELEMENT_MASK_ESI);
    hdr->RxTimestamp = (*address & FDCAN_ELEMENT_MASK_TS);
    hdr->DataLength =  ((*address & FDCAN_ELEMENT_MASK_DLC)>>16);
    hdr->BitRateSwitch = (*address & FDCAN_ELEMENT_MASK_BRS);
    hdr->FDFormat = (*address & FDCAN_ELEMENT_MASK_FDF);
    hdr->FilterIndex = ((*address & FDCAN_ELEMENT_MASK_FIDX) >> 24);
    hdr->IsFilterMatchingFrame = ((*address++ & FDCAN_ELEMENT_MASK_ANMF) >> 31);
    // Copy data.
    uint8_t *pdata = (uint8_t *)address;
    for(uint32_t i=0; i<8; i++) { // TODO use DLCtoBytes[hdr->DataLength] for length > 8
      *data++ = *pdata++;
    }

    // Release (free) message from FIFO
    *rxa = index;

    return 0; // success
}

/******************************************************************************/
// MicroPython bindings

STATIC void pyb_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_can_obj_t *self = self_in;
    if (!self->is_enabled) {
        mp_printf(print, "CAN(%u)", self->fdcan_id);
    } else {
        qstr mode;
        switch (self->fdcan.Init.Mode) {
            case FDCAN_MODE_NORMAL: mode = MP_QSTR_NORMAL; break;
            case FDCAN_MODE_INTERNAL_LOOPBACK: mode = MP_QSTR_LOOPBACK; break;
            default: mode = MP_QSTR_SILENT; break;
        }
        mp_printf(print, "CAN(%u, CAN.%q, extframe=%q)",
            self->fdcan_id,
            mode,
            self->extframe ? MP_QSTR_True : MP_QSTR_False);
    }
}

// init(mode, extframe=False, prescaler=100, *, sjw=1, bs1=6, bs2=8)
STATIC mp_obj_t pyb_can_init_helper(pyb_can_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_extframe, ARG_prescaler, ARG_sjw, ARG_bs1, ARG_bs2, ARG_auto_restart };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int  = FDCAN_MODE_NORMAL} },
        { MP_QSTR_extframe,     MP_ARG_BOOL,                    {.u_bool = false} },
        { MP_QSTR_prescaler,    MP_ARG_INT,                     {.u_int = 0x01} },
        { MP_QSTR_sjw,          MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int = 0x08} },
        { MP_QSTR_bs1,          MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int = 0x1F} },
        { MP_QSTR_bs2,          MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int = 0x08} },
        { MP_QSTR_auto_restart, MP_ARG_KW_ONLY | MP_ARG_BOOL,   {.u_bool = false} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->extframe = args[ARG_extframe].u_bool;

    // set the CAN configuration values
    memset(&self->fdcan, 0, sizeof(self->fdcan));

    FDCAN_InitTypeDef *init = &self->fdcan.Init;
    init->FrameFormat = FDCAN_FRAME_CLASSIC;
    init->Mode = args[ARG_mode].u_int;

    init->NominalPrescaler = args[ARG_prescaler].u_int; // tq = NominalPrescaler x (1/fdcan_ker_ck)
    init->NominalSyncJumpWidth = args[ARG_sjw].u_int;
    init->NominalTimeSeg1 = args[ARG_bs1].u_int; // NominalTimeSeg1 = Propagation_segment + Phase_segment_1
    init->NominalTimeSeg2 = args[ARG_bs2].u_int;

    init->AutoRetransmission = ENABLE;
    init->TransmitPause = DISABLE;
    init->ProtocolException = ENABLE;
    // The Message RAM is shared between CAN1 and CAN2. Setting the offset to half
    // the Message RAM for the second CAN and using half the resources for each CAN.
    if (self->fdcan_id == PYB_CAN_1) {
        init->MessageRAMOffset = 0;
    } else {
        init->MessageRAMOffset = 2560/2;
    }

    init->StdFiltersNbr = 64; // 128 / 2
    init->ExtFiltersNbr = 0; // Not used

    init->TxEventsNbr  = 16; // 32 / 2
    init->RxBuffersNbr = 32; // 64 / 2
    init->TxBuffersNbr = 16; // 32 / 2 

    init->RxFifo0ElmtsNbr = 64; // 128 / 2
    init->RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;

    init->RxFifo1ElmtsNbr = 64; // 128 / 2
    init->RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;

    init->TxFifoQueueElmtsNbr = 16; // Tx fifo elements
    init->TxElmtSize = FDCAN_DATA_BYTES_8;
    init->TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

    // init CAN (if it fails, it's because the port doesn't exist)
    if (!can_init(self)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "CAN(%d) doesn't exist", self->fdcan_id));
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
        #ifdef MICROPY_HW_FDCAN1_NAME
        } else if (strcmp(port, MICROPY_HW_FDCAN1_NAME) == 0) {
            can_idx = PYB_CAN_1;
        #endif
        #ifdef MICROPY_HW_FDCAN2_NAME
        } else if (strcmp(port, MICROPY_HW_FDCAN2_NAME) == 0) {
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
        self->fdcan_id = can_idx;
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
    HAL_FDCAN_DeInit(&self->fdcan);
    if (self->fdcan.Instance == FDCAN1) {
        HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
        // TODO check if FDCAN2 is used.
        __HAL_RCC_FDCAN_FORCE_RESET();
        __HAL_RCC_FDCAN_RELEASE_RESET();
        __HAL_RCC_FDCAN_CLK_DISABLE();
    #if defined(MICROPY_HW_FDCAN2_TX)
    } else if (self->fdcan.Instance == FDCAN2) {
        HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN2_IT1_IRQn);
        // TODO check if FDCAN2 is used.
        __HAL_RCC_FDCAN_FORCE_RESET();
        __HAL_RCC_FDCAN_RELEASE_RESET();
        __HAL_RCC_FDCAN_CLK_DISABLE();
    #endif
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_can_deinit_obj, pyb_can_deinit);

// Force a software restart of the controller, to allow transmission after a bus error
STATIC mp_obj_t pyb_can_restart(mp_obj_t self_in) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_enabled) {
        mp_raise_ValueError(NULL);
    }
    FDCAN_GlobalTypeDef *can = self->fdcan.Instance;
    can->CCCR |= FDCAN_CCCR_INIT;
    while ((can->CCCR & FDCAN_CCCR_INIT) == 0) {
    }
    can->CCCR &= ~FDCAN_CCCR_INIT;
    while ((can->CCCR & FDCAN_CCCR_INIT)) {
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_can_restart_obj, pyb_can_restart);

// Get the state of the controller
STATIC mp_obj_t pyb_can_state(mp_obj_t self_in) {
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t state = CAN_STATE_STOPPED;
    if (self->is_enabled) {
        FDCAN_GlobalTypeDef *can = self->fdcan.Instance;
        if (can->PSR & FDCAN_PSR_BO) {
            state = CAN_STATE_BUS_OFF;
        } else if (can->PSR & FDCAN_PSR_EP) {
            state = CAN_STATE_ERROR_PASSIVE;
        } else if (can->PSR & FDCAN_PSR_EW) {
            state = CAN_STATE_ERROR_WARNING;
        } else {
            state = CAN_STATE_ERROR_ACTIVE;
        }
    }
    return MP_OBJ_NEW_SMALL_INT(state);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_can_state_obj, pyb_can_state);

// Get info about error states and TX/RX buffers
STATIC mp_obj_t pyb_can_info(size_t n_args, const mp_obj_t *args) {
#if 0
    pyb_can_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_list_t *list;
    if (n_args == 1) {
        list = MP_OBJ_TO_PTR(mp_obj_new_list(8, NULL));
    } else {
        if (!MP_OBJ_IS_TYPE(args[1], &mp_type_list)) {
            mp_raise_TypeError(NULL);
        }
        list = MP_OBJ_TO_PTR(args[1]);
        if (list->len < 8) {
            mp_raise_ValueError(NULL);
        }
    }
    FDCAN_GlobalTypeDef *can = self->fdcan.Instance;
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
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_can_info_obj, 1, 2, pyb_can_info);

/// \method any(fifo)
/// Return `True` if any message waiting on the FIFO, else `False`.
STATIC mp_obj_t pyb_can_any(mp_obj_t self_in, mp_obj_t fifo_in) {
    pyb_can_obj_t *self = self_in;
    mp_int_t fifo = mp_obj_get_int(fifo_in);
    if (HAL_FDCAN_GetRxFifoFillLevel(&self->fdcan, fifo)) { 
        return mp_const_true;
    } else {
        return mp_const_true;
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
    enum { ARG_data, ARG_id, ARG_timeout, ARG_rtr };
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
    pyb_buf_get_for_send(args[ARG_data].u_obj, &bufinfo, data);

    if (bufinfo.len > 8) {
        mp_raise_ValueError("CAN data field too long");
    }

    // send the data
    uint8_t tx_data[8];
    FDCAN_TxHeaderTypeDef tx_hdr;
    if (self->extframe) {
        tx_hdr.IdType = FDCAN_EXTENDED_ID;
        tx_hdr.Identifier = args[ARG_id].u_int & 0x1FFFFFFF;
    } else {
        tx_hdr.IdType = FDCAN_STANDARD_ID;
        tx_hdr.Identifier = args[ARG_id].u_int & 0x7FF;
    }
    if (args[ARG_rtr].u_bool == false) {
        tx_hdr.TxFrameType = FDCAN_DATA_FRAME;
    } else  {
        tx_hdr.TxFrameType = FDCAN_REMOTE_FRAME;
    }
    tx_hdr.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_hdr.BitRateSwitch = FDCAN_BRS_OFF;
    tx_hdr.FDFormat = FDCAN_CLASSIC_CAN;
    tx_hdr.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_hdr.MessageMarker = 0;

    tx_hdr.DataLength = (bufinfo.len << 16); // TODO DLC for len > 8
    for (mp_uint_t i = 0; i < bufinfo.len; i++) {
        tx_data[i] = ((byte*)bufinfo.buf)[i]; // Data is uint32_t but holds only 1 byte
    }

    HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(&self->fdcan, &tx_hdr, tx_data);

    if (status != HAL_OK) {
        printf("fail%ld\n", self->fdcan.ErrorCode);
        mp_hal_raise(status);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_send_obj, 1, pyb_can_send);

/// \method recv(fifo, list=None, *, timeout=5000)
///
/// Receive data on the bus:
///
///   - `fifo` is an integer, which is the FIFO to receive on
///   - `list` if not None is a list with at least 4 elements
///   - `timeout` is the timeout in milliseconds to wait for the receive.
///
/// Return value: buffer of data bytes.
STATIC mp_obj_t pyb_can_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_fifo, ARG_list, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_fifo,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_list,    MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_can_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // receive the data
    uint8_t rx_data[8];
    FDCAN_RxHeaderTypeDef rx_hdr;
    mp_int_t fifo = args[ARG_fifo].u_int;
    if (fifo == 0) {
        fifo = FDCAN_RX_FIFO0;
    } else if (fifo == 1) {
        fifo = FDCAN_RX_FIFO1;
    } else {
        // TODO error
    }
    int ret = can_receive(&self->fdcan, fifo, &rx_hdr, rx_data, args[ARG_timeout].u_int);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    // Manage the rx state machine
    if ((fifo == FDCAN_RX_FIFO0 && self->rxcallback0 != mp_const_none) ||
        (fifo == FDCAN_RX_FIFO1 && self->rxcallback1 != mp_const_none)) {
        byte *state = (fifo == FDCAN_RX_FIFO0) ? &self->rx_state0 : &self->rx_state1;

        switch (*state) {
        case RX_STATE_FIFO_EMPTY:
            break;
        case RX_STATE_MESSAGE_PENDING:
            if (HAL_FDCAN_GetRxFifoFillLevel(&self->fdcan, fifo) == 0) {
                // Fifo is empty
                __HAL_FDCAN_ENABLE_IT(&self->fdcan, (fifo == FDCAN_RX_FIFO0) ?
                        FDCAN_IT_RX_FIFO0_NEW_MESSAGE : FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
                *state = RX_STATE_FIFO_EMPTY;
            }
            break;
        case RX_STATE_FIFO_FULL:
            __HAL_FDCAN_ENABLE_IT(&self->fdcan, (fifo == FDCAN_RX_FIFO0) ?
                    FDCAN_IT_RX_FIFO0_FULL : FDCAN_IT_RX_FIFO1_FULL);
            *state = RX_STATE_MESSAGE_PENDING;
            break;
        case RX_STATE_FIFO_OVERFLOW:
            __HAL_FDCAN_ENABLE_IT(&self->fdcan, (fifo == FDCAN_RX_FIFO0) ?
                    FDCAN_IT_RX_FIFO0_MESSAGE_LOST : FDCAN_IT_RX_FIFO1_MESSAGE_LOST);
            __HAL_FDCAN_ENABLE_IT(&self->fdcan, (fifo == FDCAN_RX_FIFO0) ?
                    FDCAN_IT_RX_FIFO0_FULL  : FDCAN_IT_RX_FIFO1_FULL);
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
        items[3] = mp_obj_new_bytes(rx_data, rx_hdr.DataLength);
    } else {
        // User should provide a list of length at least 4 to hold the values
        if (!MP_OBJ_IS_TYPE(ret_obj, &mp_type_list)) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_list_t *list = MP_OBJ_TO_PTR(ret_obj);
        if (list->len < 4) {
            mp_raise_ValueError(NULL);
        }
        items = list->items;
        // Fourth element must be a memoryview which we assume points to a
        // byte-like array which is large enough, and then we resize it inplace
        if (!MP_OBJ_IS_TYPE(items[3], &mp_type_memoryview)) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_array_t *mv = MP_OBJ_TO_PTR(items[3]);
        if (!(mv->typecode == (0x80 | BYTEARRAY_TYPECODE)
            || (mv->typecode | 0x20) == (0x80 | 'b'))) {
            mp_raise_ValueError(NULL);
        }
        mv->len = rx_hdr.DataLength;
        memcpy(mv->items, rx_data, rx_hdr.DataLength);
    }

    // Populate the first 3 values of the tuple/list
    items[0] = MP_OBJ_NEW_SMALL_INT(rx_hdr.Identifier);
    items[1] = rx_hdr.RxFrameType == FDCAN_REMOTE_FRAME ? mp_const_true : mp_const_false;
    items[2] = MP_OBJ_NEW_SMALL_INT(rx_hdr.FilterIndex);

    // Return the result
    return ret_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_can_recv_obj, 1, pyb_can_recv);

STATIC mp_obj_t pyb_can_clearfilter(mp_obj_t self_in, mp_obj_t filter_in) {
    pyb_can_obj_t *self = self_in;
    mp_int_t f = mp_obj_get_int(filter_in);
    can_clearfilter(&self->fdcan, f);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_can_clearfilter_obj, pyb_can_clearfilter);

/// Configures a filter
/// Return value: `None`.
STATIC mp_obj_t pyb_can_setfilter(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_filter, ARG_fifo, ARG_params };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_filter,   MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_fifo,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_params,   MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    pyb_can_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    FDCAN_FilterTypeDef filter = {0};
    filter.IdType = FDCAN_STANDARD_ID;
    filter.FilterIndex = args[ARG_filter].u_int;

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
    if (len != 2) {
        goto error;
    }
    filter.FilterID1 = mp_obj_get_int(params[0]);
    filter.FilterID2 = mp_obj_get_int(params[1]);
    filter.FilterType = FDCAN_FILTER_RANGE;
    HAL_FDCAN_ConfigFilter(&self->fdcan, &filter);

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
        __HAL_FDCAN_DISABLE_IT(&self->fdcan, (fifo == 0) ? FDCAN_IT_RX_FIFO0_NEW_MESSAGE : FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
        __HAL_FDCAN_DISABLE_IT(&self->fdcan, (fifo == 0) ? FDCAN_IT_RX_FIFO0_FULL : FDCAN_IT_RX_FIFO1_FULL);
        __HAL_FDCAN_DISABLE_IT(&self->fdcan, (fifo == 0) ? FDCAN_IT_RX_FIFO0_MESSAGE_LOST : FDCAN_IT_RX_FIFO1_MESSAGE_LOST);
        __HAL_FDCAN_CLEAR_FLAG(&self->fdcan, (fifo == FDCAN_RX_FIFO0) ? FDCAN_FLAG_RX_FIFO0_FULL : FDCAN_FLAG_RX_FIFO1_FULL);
        __HAL_FDCAN_CLEAR_FLAG(&self->fdcan, (fifo == FDCAN_RX_FIFO0) ? FDCAN_FLAG_RX_FIFO0_MESSAGE_LOST : FDCAN_FLAG_RX_FIFO1_MESSAGE_LOST);
        *callback = mp_const_none;
    } else if (*callback != mp_const_none) {
        // Rx call backs has already been initialized
        // only the callback function should be changed
        *callback = callback_in;
    } else if (mp_obj_is_callable(callback_in)) {
        *callback = callback_in;
        uint32_t irq = 0;
        if (self->fdcan_id == PYB_CAN_1) {
            irq = (fifo == 0) ? FDCAN1_IT0_IRQn : FDCAN1_IT1_IRQn;
        #if defined(MICROPY_HW_FDCAN2_TX)
        } else {
            irq = (fifo == 0) ? FDCAN2_IT0_IRQn : FDCAN2_IT1_IRQn;
        #endif
        }
        NVIC_SetPriority(irq, IRQ_PRI_CAN);
        HAL_NVIC_EnableIRQ(irq);
        __HAL_FDCAN_ENABLE_IT(&self->fdcan, (fifo == 0) ? FDCAN_IT_RX_FIFO0_NEW_MESSAGE : FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
        __HAL_FDCAN_ENABLE_IT(&self->fdcan, (fifo == 0) ? FDCAN_IT_RX_FIFO0_FULL  : FDCAN_IT_RX_FIFO1_FULL);
        __HAL_FDCAN_ENABLE_IT(&self->fdcan, (fifo == 0) ? FDCAN_IT_RX_FIFO0_MESSAGE_LOST : FDCAN_IT_RX_FIFO1_MESSAGE_LOST);
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
    { MP_ROM_QSTR(MP_QSTR_setfilter), MP_ROM_PTR(&pyb_can_setfilter_obj) },
    { MP_ROM_QSTR(MP_QSTR_clearfilter), MP_ROM_PTR(&pyb_can_clearfilter_obj) },
    { MP_ROM_QSTR(MP_QSTR_rxcallback), MP_ROM_PTR(&pyb_can_rxcallback_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_NORMAL), MP_ROM_INT(FDCAN_MODE_NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_LOOPBACK), MP_ROM_INT(FDCAN_MODE_INTERNAL_LOOPBACK) },
    { MP_ROM_QSTR(MP_QSTR_SILENT), MP_ROM_INT(FDCAN_MODE_BUS_MONITORING) },

    // values for CAN.state()
    { MP_ROM_QSTR(MP_QSTR_STOPPED), MP_ROM_INT(CAN_STATE_STOPPED) },
    { MP_ROM_QSTR(MP_QSTR_ERROR_ACTIVE), MP_ROM_INT(CAN_STATE_ERROR_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_ERROR_WARNING), MP_ROM_INT(CAN_STATE_ERROR_WARNING) },
    { MP_ROM_QSTR(MP_QSTR_ERROR_PASSIVE), MP_ROM_INT(CAN_STATE_ERROR_PASSIVE) },
    { MP_ROM_QSTR(MP_QSTR_BUS_OFF), MP_ROM_INT(CAN_STATE_BUS_OFF) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_can_locals_dict, pyb_can_locals_dict_table);

mp_uint_t can_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    pyb_can_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD)
            && ((HAL_FDCAN_GetRxFifoFillLevel(&self->fdcan, FDCAN_RX_FIFO0) != 0)
                || (HAL_FDCAN_GetRxFifoFillLevel(&self->fdcan, FDCAN_RX_FIFO1) != 0))) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && (self->fdcan.Instance->IR & FDCAN_IR_TFE)) {
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

    if (fifo_id == FDCAN_RX_FIFO0) {
        callback = self->rxcallback0;
        state = &self->rx_state0;
    } else {
        callback = self->rxcallback1;
        state = &self->rx_state1;
    }

    switch (*state) {
        case RX_STATE_FIFO_EMPTY:
            __HAL_FDCAN_DISABLE_IT(&self->fdcan,  (fifo_id == FDCAN_RX_FIFO0) ?
                    FDCAN_IT_RX_FIFO0_NEW_MESSAGE : FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
            irq_reason = MP_OBJ_NEW_SMALL_INT(0);
            *state = RX_STATE_MESSAGE_PENDING;
            break;
        case RX_STATE_MESSAGE_PENDING:
            __HAL_FDCAN_DISABLE_IT(&self->fdcan, (fifo_id == FDCAN_RX_FIFO0) ? FDCAN_IT_RX_FIFO0_FULL : FDCAN_IT_RX_FIFO1_FULL);
            __HAL_FDCAN_CLEAR_FLAG(&self->fdcan, (fifo_id == FDCAN_RX_FIFO0) ? FDCAN_FLAG_RX_FIFO0_FULL : FDCAN_FLAG_RX_FIFO1_FULL);
            irq_reason = MP_OBJ_NEW_SMALL_INT(1);
            *state = RX_STATE_FIFO_FULL;
            break;
        case RX_STATE_FIFO_FULL:
            __HAL_FDCAN_DISABLE_IT(&self->fdcan, (fifo_id == FDCAN_RX_FIFO0) ?
                    FDCAN_IT_RX_FIFO0_MESSAGE_LOST : FDCAN_IT_RX_FIFO1_MESSAGE_LOST);
            __HAL_FDCAN_CLEAR_FLAG(&self->fdcan, (fifo_id == FDCAN_RX_FIFO0) ?
                    FDCAN_FLAG_RX_FIFO0_MESSAGE_LOST : FDCAN_FLAG_RX_FIFO1_MESSAGE_LOST);
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
            printf("uncaught exception in CAN(%u) rx interrupt handler\n", self->fdcan_id);
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

#endif // MICROPY_HW_ENABLE_FDCAN
