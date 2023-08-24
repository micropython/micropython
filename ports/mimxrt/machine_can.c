/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Kwabena W. Agyeman
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

#include "py/obj.h"
#include "py/objarray.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/binary.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/runtime/mpirq.h"
#include "modmachine.h"
#include CLOCK_CONFIG_H

#include "fsl_iomuxc.h"

#if MICROPY_HW_ENABLE_CAN
#include "fsl_flexcan.h"

#define CAN_MAX_DATA_FRAME              (8)

#define CAN_NORMAL_MODE                 (0)
#define CAN_LOOPBACK_FLAG               (1)
#define CAN_SILENT_FLAG                 (2)

#define CAN_DUAL                        (0)

// matches fsl_flexcan.c enum _flexcan_mb_code_tx
#define kFLEXCAN_TxMbDataOrRemote       (0xC)

enum {
    CAN_STATE_STOPPED,
    CAN_STATE_ERROR_ACTIVE,
    CAN_STATE_ERROR_WARNING,
    CAN_STATE_ERROR_PASSIVE,
    CAN_STATE_BUS_OFF,
};

#define MICROPY_HW_CAN_NUM MP_ARRAY_SIZE(can_index_table)

#define CTX (iomux_table[index])
#define CRX (iomux_table[index + 1])

typedef struct _machine_can_obj_t {
    mp_obj_base_t base;
    uint8_t can_id;
    uint8_t can_hw_id;
    CAN_Type *can_inst;
    flexcan_config_t *flexcan_config;
    flexcan_rx_fifo_config_t *flexcan_rx_fifo_config;
    uint8_t flexcan_txmb_start;
    uint8_t flexcan_txmb_count;
    mp_obj_t callback;
    bool is_enabled;
    uint16_t num_error_warning;
    uint16_t num_error_passive;
    uint16_t num_bus_off;
} machine_can_obj_t;

typedef struct _iomux_table_t {
    uint32_t muxRegister;
    uint32_t muxMode;
    uint32_t inputRegister;
    uint32_t inputDaisy;
    uint32_t configRegister;
} iomux_table_t;

STATIC const uint8_t can_index_table[] = MICROPY_HW_CAN_INDEX;
#ifdef MIMXRT117x_SERIES
STATIC const uint32_t can_clock_index_table[] = {
    BOARD_BOOTCLOCKRUN_CAN1_CLK_ROOT,
    BOARD_BOOTCLOCKRUN_CAN2_CLK_ROOT,
    BOARD_BOOTCLOCKRUN_CAN3_CLK_ROOT
};
#endif
STATIC CAN_Type *can_base_ptr_table[] = CAN_BASE_PTRS;
static const iomux_table_t iomux_table[] = {
    IOMUX_TABLE_CAN
};

bool can_set_iomux(int8_t can) {
    int index = (can - 1) * 2;

    if (CTX.muxRegister != 0) {
        IOMUXC_SetPinMux(CTX.muxRegister, CTX.muxMode, CTX.inputRegister, CTX.inputDaisy, CTX.configRegister, 0U);
        IOMUXC_SetPinConfig(CTX.muxRegister, CTX.muxMode, CTX.inputRegister, CTX.inputDaisy, CTX.configRegister,
            pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_OUT, PIN_DRIVE_6, CTX.configRegister));

        IOMUXC_SetPinMux(CRX.muxRegister, CRX.muxMode, CRX.inputRegister, CRX.inputDaisy, CRX.configRegister, 0U);
        IOMUXC_SetPinConfig(CRX.muxRegister, CRX.muxMode, CRX.inputRegister, CRX.inputDaisy, CRX.configRegister,
            pin_generate_config(PIN_PULL_UP_100K, PIN_MODE_IN, PIN_DRIVE_6, CRX.configRegister));

        return true;
    } else {
        return false;
    }
}

void machine_can_handler(CAN_Type *base) {
    machine_can_obj_t *self = NULL;
    for (int i = 0; i < MICROPY_HW_NUM_CAN_IRQS; ++i) {
        machine_can_obj_t *machine_can_obj = MP_STATE_PORT(machine_can_objects[i]);
        if ((machine_can_obj != NULL) && (machine_can_obj->can_inst == base)) {
            self = machine_can_obj;
            break;
        }
    }
    if (self != NULL) {
        uint32_t result = FLEXCAN_GetStatusFlags(self->can_inst);
        if (result & FLEXCAN_ERROR_AND_STATUS_INIT_FLAG) {
            uint32_t flt_conf = (result & CAN_ESR1_FLTCONF_MASK) >> CAN_ESR1_FLTCONF_SHIFT;
            if (flt_conf > 1) {
                ++self->num_bus_off;
            } else if (flt_conf == 1) {
                ++self->num_error_passive;
            } else if ((result & CAN_ESR1_RXWRN_MASK) || (result & CAN_ESR1_TXWRN_MASK)) {
                ++self->num_error_warning;
            }
            FLEXCAN_ClearStatusFlags(self->can_inst, FLEXCAN_ERROR_AND_STATUS_INIT_FLAG);
        }

        mp_int_t irq_reason = -1;

        if (FLEXCAN_GetMbStatusFlags(self->can_inst, (uint32_t)kFLEXCAN_RxFifoOverflowFlag)) {
            FLEXCAN_DisableMbInterrupts(self->can_inst,
                kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
            irq_reason = 2;
        } else if (FLEXCAN_GetMbStatusFlags(self->can_inst, (uint32_t)kFLEXCAN_RxFifoWarningFlag)) {
            FLEXCAN_DisableMbInterrupts(self->can_inst, kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
            irq_reason = 1;
        } else if (FLEXCAN_GetMbStatusFlags(self->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag)) {
            FLEXCAN_DisableMbInterrupts(self->can_inst, kFLEXCAN_RxFifoFrameAvlFlag);
            irq_reason = 0;
        }

        if (irq_reason != -1 && self->callback != mp_const_none) {
            mp_sched_lock();
            gc_lock();
            nlr_buf_t nlr;
            if (nlr_push(&nlr) == 0) {
                mp_call_function_2(self->callback, MP_OBJ_FROM_PTR(self), MP_OBJ_NEW_SMALL_INT(irq_reason));
                nlr_pop();
            } else {
                // Uncaught exception; disable the callback so it doesn't run again.
                self->callback = mp_const_none;
                mp_printf(MICROPY_ERROR_PRINTER, "uncaught exception in CAN(%u) rx interrupt handler\n", self->can_id);
                mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
            }
            gc_unlock();
            mp_sched_unlock();
        }
    }
}

#if defined(CAN1)
void CAN1_IRQHandler(void) {
    machine_can_handler(CAN1);
}
#endif

#if defined(CAN2)
void CAN2_IRQHandler(void) {
    machine_can_handler(CAN2);
}
#endif

#if defined(CAN3)
void CAN3_IRQHandler(void) {
    machine_can_handler(CAN3);
}
#endif

static void machine_flexcan_deinit(machine_can_obj_t *self) {
    if (MP_STATE_PORT(machine_can_objects[self->can_id]) != NULL) {
        mp_uint_t instance = FLEXCAN_GetInstance(self->can_inst);
        DisableIRQ(((IRQn_Type [])CAN_Rx_Warning_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_Tx_Warning_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_Error_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_Bus_Off_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_ORed_Message_buffer_IRQS)[instance]);
        FLEXCAN_DisableInterrupts(self->can_inst,
            kFLEXCAN_BusOffInterruptEnable | kFLEXCAN_ErrorInterruptEnable |
            kFLEXCAN_RxWarningInterruptEnable | kFLEXCAN_TxWarningInterruptEnable);
        FLEXCAN_DisableMbInterrupts(self->can_inst,
            kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
        MP_STATE_PORT(machine_can_objects[self->can_id]) = NULL;
        self->is_enabled = false;
        FLEXCAN_Deinit(self->can_inst);
    }
}

// Deinit all can IRQ handlers.
void machine_can_irq_deinit(void) {
    for (int i = 0; i < MICROPY_HW_NUM_CAN_IRQS; ++i) {
        machine_can_obj_t *machine_can_obj = MP_STATE_PORT(machine_can_objects[i]);
        if (machine_can_obj != NULL) {
            machine_flexcan_deinit(machine_can_obj);
        }
    }
}

STATIC void machine_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_enabled) {
        mp_printf(print, "CAN(%u)", self->can_id);
    } else {
        qstr mode = MP_QSTR_NORMAL;
        if (self->flexcan_config->enableLoopBack) {
            if (self->flexcan_config->enableListenOnlyMode) {
                mode = MP_QSTR_SILENT_LOOPBACK;
            } else {
                mode = MP_QSTR_LOOPBACK;
            }
        } else if (self->flexcan_config->enableListenOnlyMode) {
            mode = MP_QSTR_SILENT;
        }
        mp_printf(print, "CAN(%u, mode=CAN.%q, auto_restart=%q, baudrate=%u)",
            self->can_id,
            mode,
            (self->can_inst->CTRL1 & CAN_CTRL1_BOFFREC_MASK) ? MP_QSTR_False : MP_QSTR_True,
            self->flexcan_config->bitRate);
    }
}

STATIC mp_obj_t machine_can_init_helper(machine_can_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_auto_restart, ARG_baudrate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = CAN_NORMAL_MODE} },
        { MP_QSTR_auto_restart, MP_ARG_KW_ONLY | MP_ARG_BOOL,   {.u_bool = false} },
        { MP_QSTR_baudrate,     MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int = 0} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_flexcan_deinit(self);

    // Initialise the CAN peripheral.
    self->flexcan_config->enableLoopBack = args[ARG_mode].u_int & CAN_LOOPBACK_FLAG;
    self->flexcan_config->enableListenOnlyMode = args[ARG_mode].u_int & CAN_SILENT_FLAG;
    self->flexcan_config->bitRate = args[ARG_baudrate].u_int;

    // When selecting the CCM CAN clock source with CAN_CLK_SEL set to 2, the UART clock gate
    // will not open and CAN_CLK_ROOT will be off. To avoid this issue, set CAN_CLK_SEL to 0 or
    // 1 for CAN clock selection, or open the UART clock gate by configuring the CCM_CCGRx register.
    // There are two workarounds:
    // Set CAN_CLK_SEL to 0 or 1 for CAN clock selection, or if CAN_CLK_SEL is set to 2,
    // then the CCM must open any of UART clock gate by configuring the CCM_CCGRx register.
    #if (defined(FSL_FEATURE_CCM_HAS_ERRATA_50235) && FSL_FEATURE_CCM_HAS_ERRATA_50235)
    CLOCK_EnableClock(kCLOCK_Lpuart1);
    #endif // FSL_FEATURE_CCM_HAS_ERRATA_50235

    uint32_t sourceClock_Hz;
    #ifdef MIMXRT117x_SERIES
    sourceClock_Hz = can_clock_index_table[self->can_hw_id];
    #else
    sourceClock_Hz = BOARD_BOOTCLOCKRUN_CAN_CLK_ROOT;
    #endif

    FLEXCAN_Init(self->can_inst, self->flexcan_config, sourceClock_Hz);
    memset(self->flexcan_rx_fifo_config->idFilterTable, 0,
        sizeof(uint32_t) * self->flexcan_rx_fifo_config->idFilterNum);
    FLEXCAN_SetRxFifoConfig(self->can_inst, self->flexcan_rx_fifo_config, true);

    // Calculate the Number of Mailboxes occupied by RX Legacy FIFO and the filter.
    mp_uint_t rffn = (uint8_t)((self->can_inst->CTRL2 & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
    self->flexcan_txmb_start = 6U + (rffn + 1U) * 2U;
    #if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
    (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
    // the first valid MB should be occupied by ERRATA 5461 or 5829.
    self->flexcan_txmb_start += 1U;
    #endif
    self->flexcan_txmb_count = self->flexcan_config->maxMbNum - self->flexcan_txmb_start;

    for (mp_uint_t i = 0; i < self->flexcan_txmb_count; i++) {
        FLEXCAN_SetTxMbConfig(self->can_inst, self->flexcan_txmb_start + i, true);
    }

    if (!args[ARG_auto_restart].u_bool) {
        self->can_inst->CTRL1 |= CAN_CTRL1_BOFFREC_MASK;
    } else {
        self->can_inst->CTRL1 &= ~CAN_CTRL1_BOFFREC_MASK;
    }

    self->callback = mp_const_none;
    self->is_enabled = true;
    self->num_error_warning = 0;
    self->num_error_passive = 0;
    self->num_bus_off = 0;

    MP_STATE_PORT(machine_can_objects[self->can_id]) = self;

    FLEXCAN_EnableMbInterrupts(self->can_inst,
        kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);

    FLEXCAN_EnableInterrupts(self->can_inst,
        kFLEXCAN_BusOffInterruptEnable | kFLEXCAN_ErrorInterruptEnable |
        kFLEXCAN_RxWarningInterruptEnable | kFLEXCAN_TxWarningInterruptEnable);

    mp_uint_t instance = FLEXCAN_GetInstance(self->can_inst);
    EnableIRQ(((IRQn_Type [])CAN_Rx_Warning_IRQS)[instance]);
    EnableIRQ(((IRQn_Type [])CAN_Tx_Warning_IRQS)[instance]);
    EnableIRQ(((IRQn_Type [])CAN_Error_IRQS)[instance]);
    EnableIRQ(((IRQn_Type [])CAN_Bus_Off_IRQS)[instance]);
    EnableIRQ(((IRQn_Type [])CAN_ORed_Message_buffer_IRQS)[instance]);

    return MP_OBJ_FROM_PTR(self);
}

// CAN(bus, ...)
mp_obj_t machine_can_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get the CAN bus id.
    int can_id = mp_obj_get_int(args[0]);
    if (can_id < 0 || can_id >= MP_ARRAY_SIZE(can_index_table) || can_index_table[can_id] == 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("CAN(%d) doesn't exist"), can_id);
    }

    // Get peripheral object.
    mp_uint_t can_hw_id = can_index_table[can_id];  // the hw can number 1..n
    machine_can_obj_t *self = mp_obj_malloc(machine_can_obj_t, &machine_can_type);
    self->can_id = can_id;
    self->can_inst = can_base_ptr_table[can_hw_id];
    self->can_hw_id = can_hw_id;
    self->flexcan_config = m_new_obj(flexcan_config_t);
    FLEXCAN_GetDefaultConfig(self->flexcan_config);
    mp_uint_t maxMbNum = FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(self->can_inst);
    self->flexcan_config->maxMbNum = maxMbNum;
    self->flexcan_config->disableSelfReception = true;

    self->flexcan_rx_fifo_config = m_new_obj(flexcan_rx_fifo_config_t);
    mp_uint_t idFilterNum = maxMbNum * 2;
    self->flexcan_rx_fifo_config->idFilterNum = idFilterNum;
    self->flexcan_rx_fifo_config->idFilterType = kFLEXCAN_RxFifoFilterTypeA;
    self->flexcan_rx_fifo_config->priority = kFLEXCAN_RxFifoPrioHigh;
    self->flexcan_rx_fifo_config->idFilterTable = m_new0(uint32_t, idFilterNum);

    // Configure board-specific pin MUX based on the hardware device number.
    bool can_present = can_set_iomux(can_hw_id);

    if (can_present) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        return machine_can_init_helper(self, n_args - 1, args + 1, &kw_args);
    } else {
        return mp_const_none;
    }
}

// can.init(mode, [kwargs])
STATIC mp_obj_t machine_can_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_can_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_can_init_obj, 1, machine_can_init);

// deinit()
STATIC mp_obj_t machine_can_deinit(mp_obj_t self_in) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_flexcan_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_can_deinit_obj, machine_can_deinit);

// Force a software restart of the controller, to allow transmission after a bus error
STATIC mp_obj_t machine_can_restart(mp_obj_t self_in) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_enabled) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN bus not enabled"));
    }
    bool boff_rec = self->can_inst->CTRL1 & CAN_CTRL1_BOFFREC_MASK;
    self->can_inst->CTRL1 &= ~CAN_CTRL1_BOFFREC_MASK;
    while (self->can_inst->CTRL1 & CAN_CTRL1_BOFFREC_MASK) {
    }
    if (boff_rec) {
        self->can_inst->CTRL1 |= CAN_CTRL1_BOFFREC_MASK;
        while ((self->can_inst->CTRL1 & CAN_CTRL1_BOFFREC_MASK) == 0) {
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_can_restart_obj, machine_can_restart);

// Get the state of the controller
STATIC mp_obj_t machine_can_state(mp_obj_t self_in) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->is_enabled) {
        uint32_t result = FLEXCAN_GetStatusFlags(self->can_inst);
        uint32_t flt_conf = (result & CAN_ESR1_FLTCONF_MASK) >> CAN_ESR1_FLTCONF_SHIFT;
        if (flt_conf > 1) {
            return MP_OBJ_NEW_SMALL_INT(CAN_STATE_BUS_OFF);
        }
        if (flt_conf == 1) {
            return MP_OBJ_NEW_SMALL_INT(CAN_STATE_ERROR_PASSIVE);
        }
        if ((result & CAN_ESR1_RXWRN_MASK) || (result & CAN_ESR1_TXWRN_MASK)) {
            return MP_OBJ_NEW_SMALL_INT(CAN_STATE_ERROR_WARNING);
        }
        // flt_conf == 0
        return MP_OBJ_NEW_SMALL_INT(CAN_STATE_ERROR_ACTIVE);
    }
    return MP_OBJ_NEW_SMALL_INT(CAN_STATE_STOPPED);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_can_state_obj, machine_can_state);

static mp_uint_t can_count_txmb_pending(machine_can_obj_t *self) {
    mp_uint_t count = 0;
    for (mp_uint_t i = 0; i < self->flexcan_txmb_count; i++) {
        uint32_t cs_temp = self->can_inst->MB[self->flexcan_txmb_start + i].CS;
        if ((cs_temp & CAN_CS_CODE_MASK) == CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote)) {
            count++;
        }
    }
    return count;
}

// Get info about error states and TX/RX buffers
STATIC mp_obj_t machine_can_info(size_t n_args, const mp_obj_t *args) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (!self->is_enabled) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN bus not enabled"));
    }
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

    CAN_Type *can = self->can_inst;
    list->items[0] = MP_OBJ_NEW_SMALL_INT((can->ECR & CAN_ECR_TXERRCNT_MASK) >> CAN_ECR_TXERRCNT_SHIFT);
    list->items[1] = MP_OBJ_NEW_SMALL_INT((can->ECR & CAN_ECR_RXERRCNT_MASK) >> CAN_ECR_RXERRCNT_SHIFT);
    list->items[2] = MP_OBJ_NEW_SMALL_INT(self->num_error_warning);
    list->items[3] = MP_OBJ_NEW_SMALL_INT(self->num_error_passive);
    list->items[4] = MP_OBJ_NEW_SMALL_INT(self->num_bus_off);
    list->items[5] = MP_OBJ_NEW_SMALL_INT(can_count_txmb_pending(self));
    // only 0 or 1
    int n_rx_pending = FLEXCAN_GetMbStatusFlags(can, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag);
    list->items[6] = MP_OBJ_NEW_SMALL_INT(n_rx_pending);
    list->items[7] = MP_OBJ_NEW_SMALL_INT(0);

    return MP_OBJ_FROM_PTR(list);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_can_info_obj, 1, 2, machine_can_info);

// any(fifo) - return `True` if any message waiting on the FIFO, else `False`
STATIC mp_obj_t machine_can_any(mp_obj_t self_in, mp_obj_t fifo_in) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_enabled) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN bus not enabled"));
    }
    mp_int_t fifo = mp_obj_get_int(fifo_in);
    if (fifo != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Fifo must be 0"));
    }
    if (FLEXCAN_GetMbStatusFlags(self->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag)) {
        return mp_const_true;
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_can_any_obj, machine_can_any);

static mp_uint_t can_find_txmb(machine_can_obj_t *self, flexcan_frame_t *frame) {
    // See if this frame id has been used before. If so, re-use the same mailbox to keep message ordering.
    for (mp_uint_t i = 0; i < self->flexcan_txmb_count; i++) {
        uint32_t cs_temp = self->can_inst->MB[self->flexcan_txmb_start + i].CS;
        if (((frame->format == kFLEXCAN_FrameFormatStandard) || (cs_temp & CAN_CS_IDE_MASK))
            && ((frame->type == kFLEXCAN_FrameTypeData) || (cs_temp & CAN_CS_RTR_MASK))
            && (self->can_inst->MB[self->flexcan_txmb_start + i].ID == frame->id)) {
            if ((cs_temp & CAN_CS_CODE_MASK) != CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote)) {
                return self->flexcan_txmb_start + i;
            } else {
                return 0;
            }
        }
    }
    // Frame id has never been used before so just pick the first empty mailbox.
    for (mp_uint_t i = 0; i < self->flexcan_txmb_count; i++) {
        uint32_t cs_temp = self->can_inst->MB[self->flexcan_txmb_start + i].CS;
        if ((cs_temp & CAN_CS_CODE_MASK) != CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote)) {
            return self->flexcan_txmb_start + i;
        }
    }
    return 0;
}

STATIC mp_obj_t machine_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_id, ARG_timeout, ARG_rtr, ARG_extframe };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,     MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = 0} },
        { MP_QSTR_timeout,  MP_ARG_KW_ONLY | MP_ARG_INT,    {.u_int = 0} },
        { MP_QSTR_rtr,      MP_ARG_KW_ONLY | MP_ARG_BOOL,   {.u_bool = false} },
        { MP_QSTR_extframe, MP_ARG_KW_ONLY | MP_ARG_BOOL,   {.u_bool = false} },
    };

    // parse args
    machine_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (!self->is_enabled) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN bus not enabled"));
    }
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    if (mp_obj_is_int(args[ARG_data].u_obj)) {
        data[0] = mp_obj_get_int(args[ARG_data].u_obj);
        bufinfo.buf = data;
        bufinfo.len = 1;
        bufinfo.typecode = 'B';
    } else {
        mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);
    }

    if (bufinfo.len > CAN_MAX_DATA_FRAME) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN data field too long"));
    }

    flexcan_frame_t tx_msg;

    tx_msg.dataWord0 = 0;
    tx_msg.dataWord1 = 0;

    if (bufinfo.len > 0) {
        tx_msg.dataByte0 = ((byte *)bufinfo.buf)[0];
    }
    if (bufinfo.len > 1) {
        tx_msg.dataByte1 = ((byte *)bufinfo.buf)[1];
    }
    if (bufinfo.len > 2) {
        tx_msg.dataByte2 = ((byte *)bufinfo.buf)[2];
    }
    if (bufinfo.len > 3) {
        tx_msg.dataByte3 = ((byte *)bufinfo.buf)[3];
    }
    if (bufinfo.len > 4) {
        tx_msg.dataByte4 = ((byte *)bufinfo.buf)[4];
    }
    if (bufinfo.len > 5) {
        tx_msg.dataByte5 = ((byte *)bufinfo.buf)[5];
    }
    if (bufinfo.len > 6) {
        tx_msg.dataByte6 = ((byte *)bufinfo.buf)[6];
    }
    if (bufinfo.len > 7) {
        tx_msg.dataByte7 = ((byte *)bufinfo.buf)[7];
    }

    tx_msg.length = bufinfo.len;
    tx_msg.type = args[ARG_rtr].u_bool;
    tx_msg.format = args[ARG_extframe].u_bool;

    if (tx_msg.format) {
        tx_msg.id = FLEXCAN_ID_EXT(args[ARG_id].u_int);
    } else {
        tx_msg.id = FLEXCAN_ID_STD(args[ARG_id].u_int);
    }

    uint32_t timeout_ms = args[ARG_timeout].u_int;
    uint32_t start = mp_hal_ticks_ms();

    while (true) {
        mp_uint_t mbIdx = can_find_txmb(self, &tx_msg);
        if (mbIdx && (FLEXCAN_WriteTxMb(self->can_inst, mbIdx, &tx_msg) == kStatus_Success)) {
            break;
        }
        if (timeout_ms == 0) {
            mp_raise_OSError(MP_ETIMEDOUT);
        }
        // Check for the Timeout
        if (timeout_ms != UINT32_MAX) {
            if (mp_hal_ticks_ms() - start >= timeout_ms) {
                mp_raise_OSError(MP_ETIMEDOUT);
            }
        }
        MICROPY_EVENT_POLL_HOOK
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_can_send_obj, 1, machine_can_send);

// recv(fifo, list=None, *, timeout=5000)
STATIC mp_obj_t machine_can_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_fifo, ARG_list, ARG_timeout, ARG_fdf };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_fifo,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_list,    MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    machine_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (!self->is_enabled) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN bus not enabled"));
    }
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_uint_t fifo = args[ARG_fifo].u_int;
    if (fifo != 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("fifo must be 0"));
    }

    uint32_t timeout_ms = args[ARG_timeout].u_int;
    uint32_t start = mp_hal_ticks_ms();

    while (!FLEXCAN_GetMbStatusFlags(self->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag)) {
        if (timeout_ms == 0) {
            mp_raise_OSError(MP_ETIMEDOUT);
        }
        // Check for the Timeout
        if (timeout_ms != UINT32_MAX) {
            if (mp_hal_ticks_ms() - start >= timeout_ms) {
                mp_raise_OSError(MP_ETIMEDOUT);
            }
        }
        MICROPY_EVENT_POLL_HOOK
    }

    flexcan_frame_t rx_frame;
    status_t status = FLEXCAN_ReadRxFifo(self->can_inst, &rx_frame);
    FLEXCAN_ClearMbStatusFlags(self->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag);

    FLEXCAN_EnableMbInterrupts(self->can_inst,
        kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);

    if (status != kStatus_Success) {
        mp_raise_OSError(MP_EIO);
    }

    uint32_t rx_dlc = rx_frame.length;
    uint8_t rx_data[CAN_MAX_DATA_FRAME] = {};

    if (rx_dlc > 0) {
        rx_data[0] = rx_frame.dataByte0;
    }
    if (rx_dlc > 1) {
        rx_data[1] = rx_frame.dataByte1;
    }
    if (rx_dlc > 2) {
        rx_data[2] = rx_frame.dataByte2;
    }
    if (rx_dlc > 3) {
        rx_data[3] = rx_frame.dataByte3;
    }
    if (rx_dlc > 4) {
        rx_data[4] = rx_frame.dataByte4;
    }
    if (rx_dlc > 5) {
        rx_data[5] = rx_frame.dataByte5;
    }
    if (rx_dlc > 6) {
        rx_data[6] = rx_frame.dataByte6;
    }
    if (rx_dlc > 7) {
        rx_data[7] = rx_frame.dataByte7;
    }

    // Create the tuple, or get the list, that will hold the return values
    // Also populate the fifth element, either a new bytes or reuse existing memoryview
    mp_obj_t ret_obj = args[ARG_list].u_obj;
    mp_obj_t *items;
    if (ret_obj == mp_const_none) {
        ret_obj = mp_obj_new_tuple(5, NULL);
        items = ((mp_obj_tuple_t *)MP_OBJ_TO_PTR(ret_obj))->items;
        items[4] = mp_obj_new_bytes(rx_data, rx_dlc);
    } else {
        // User should provide a list of length at least 5 to hold the values
        if (!mp_obj_is_type(ret_obj, &mp_type_list)) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_list_t *list = MP_OBJ_TO_PTR(ret_obj);
        if (list->len < 5) {
            mp_raise_ValueError(NULL);
        }
        items = list->items;
        // Fifth element must be a memoryview which we assume points to a
        // byte-like array which is large enough, and then we resize it inplace
        if (!mp_obj_is_type(items[4], &mp_type_memoryview)) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_array_t *mv = MP_OBJ_TO_PTR(items[4]);
        if (!(mv->typecode == (MP_OBJ_ARRAY_TYPECODE_FLAG_RW | BYTEARRAY_TYPECODE)
              || (mv->typecode | 0x20) == (MP_OBJ_ARRAY_TYPECODE_FLAG_RW | 'b'))) {
            mp_raise_ValueError(NULL);
        }
        mv->len = rx_dlc;
        memcpy(mv->items, rx_data, rx_dlc);
    }

    // Populate the first 4 values of the tuple/list
    items[0] = MP_OBJ_NEW_SMALL_INT(rx_frame.id >> (rx_frame.format ? CAN_ID_EXT_SHIFT : CAN_ID_STD_SHIFT));
    items[1] = mp_obj_new_bool(rx_frame.format);
    items[2] = mp_obj_new_bool(rx_frame.type);
    items[3] = MP_OBJ_NEW_SMALL_INT(rx_frame.idhit);

    // Return the result
    return ret_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_can_recv_obj, 1, machine_can_recv);

STATIC mp_obj_t machine_can_clearfilter(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_extframe };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_extframe, MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    machine_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (!self->is_enabled) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN bus not enabled"));
    }
    mp_uint_t bank = mp_obj_get_int(pos_args[1]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if ((bank < 0) || (bank >= self->flexcan_config->maxMbNum)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%d maximum banks"),
            self->flexcan_config->maxMbNum);
    }

    self->flexcan_rx_fifo_config->idFilterTable[bank * 2] = 0;
    self->flexcan_rx_fifo_config->idFilterTable[(bank * 2) + 1] = 0;

    FLEXCAN_SetRxFifoConfig(self->can_inst, self->flexcan_rx_fifo_config, true);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_can_clearfilter_obj, 2, machine_can_clearfilter);

// setfilter(bank, mode, fifo, params, *, rtr)
STATIC mp_obj_t machine_can_setfilter(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_bank, ARG_mode, ARG_fifo, ARG_params, ARG_rtr, ARG_extframe };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bank,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_fifo,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_params,   MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rtr,      MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_extframe, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    machine_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (!self->is_enabled) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN bus not enabled"));
    }
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_uint_t bank = args[ARG_bank].u_int;
    if ((bank < 0) || (bank >= self->flexcan_config->maxMbNum)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%d maximum banks"),
            self->flexcan_config->maxMbNum);
    }

    if (args[ARG_fifo].u_int != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("fifo must be 0"));
    }

    size_t len;
    size_t rtr_len;
    mp_uint_t rtr_masks[2] = {0, 0};
    mp_obj_t *rtr_flags;
    mp_obj_t *params;
    mp_obj_get_array(args[ARG_params].u_obj, &len, &params);
    if (args[ARG_rtr].u_obj != MP_OBJ_NULL) {
        mp_obj_get_array(args[ARG_rtr].u_obj, &rtr_len, &rtr_flags);
    }

    if (args[ARG_mode].u_int == CAN_DUAL) {
        if (len != 2) {
            goto error;
        }
        if (args[ARG_rtr].u_obj != MP_OBJ_NULL) {
            if (rtr_len != 2) {
                goto error;
            }
            rtr_masks[0] = mp_obj_is_true(rtr_flags[0]);
            rtr_masks[1] = mp_obj_is_true(rtr_flags[1]);
        }
        mp_uint_t id0 = mp_obj_get_int(params[0]);
        mp_uint_t id1 = mp_obj_get_int(params[1]);
        if (args[ARG_extframe].u_bool) {
            id0 = FLEXCAN_ID_EXT(id0) | (1 << 29);
            id1 = FLEXCAN_ID_EXT(id1) | (1 << 29);
        } else {
            id0 = FLEXCAN_ID_STD(id0);
            id1 = FLEXCAN_ID_STD(id1);
        }
        id0 = ((rtr_masks[0] << 30) | id0) << 1;
        id1 = ((rtr_masks[1] << 30) | id1) << 1;
        self->flexcan_rx_fifo_config->idFilterTable[bank * 2] = id0;
        self->flexcan_rx_fifo_config->idFilterTable[(bank * 2) + 1] = id1;
    } else {
        goto error;
    }

    FLEXCAN_SetRxFifoConfig(self->can_inst, self->flexcan_rx_fifo_config, true);

    return mp_const_none;
error:
    mp_raise_ValueError(MP_ERROR_TEXT("CAN filter parameter error"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_can_setfilter_obj, 1, machine_can_setfilter);

STATIC mp_obj_t machine_can_rxcallback(mp_obj_t self_in, mp_obj_t fifo_in, mp_obj_t callback_in) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_enabled) {
        mp_raise_ValueError(MP_ERROR_TEXT("CAN bus not enabled"));
    }
    mp_int_t fifo = mp_obj_get_int(fifo_in);
    if (fifo != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("fifo must be 0"));
    }
    self->callback = callback_in;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_can_rxcallback_obj, machine_can_rxcallback);

STATIC const mp_rom_map_elem_t machine_can_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_can_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_can_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_restart), MP_ROM_PTR(&machine_can_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_state), MP_ROM_PTR(&machine_can_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&machine_can_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_can_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&machine_can_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&machine_can_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_setfilter), MP_ROM_PTR(&machine_can_setfilter_obj) },
    { MP_ROM_QSTR(MP_QSTR_clearfilter), MP_ROM_PTR(&machine_can_clearfilter_obj) },
    { MP_ROM_QSTR(MP_QSTR_rxcallback), MP_ROM_PTR(&machine_can_rxcallback_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_NORMAL), MP_ROM_INT(CAN_NORMAL_MODE) },
    { MP_ROM_QSTR(MP_QSTR_LOOPBACK), MP_ROM_INT(CAN_LOOPBACK_FLAG) },
    { MP_ROM_QSTR(MP_QSTR_SILENT), MP_ROM_INT(CAN_SILENT_FLAG) },
    { MP_ROM_QSTR(MP_QSTR_SILENT_LOOPBACK), MP_ROM_INT(CAN_LOOPBACK_FLAG | CAN_SILENT_FLAG) },
    { MP_ROM_QSTR(MP_QSTR_DUAL), MP_ROM_INT(CAN_DUAL) },
    { MP_ROM_QSTR(MP_QSTR_MASK32), MP_ROM_INT(CAN_DUAL) },

    // values for CAN.state()
    { MP_ROM_QSTR(MP_QSTR_STOPPED), MP_ROM_INT(CAN_STATE_STOPPED) },
    { MP_ROM_QSTR(MP_QSTR_ERROR_ACTIVE), MP_ROM_INT(CAN_STATE_ERROR_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_ERROR_WARNING), MP_ROM_INT(CAN_STATE_ERROR_WARNING) },
    { MP_ROM_QSTR(MP_QSTR_ERROR_PASSIVE), MP_ROM_INT(CAN_STATE_ERROR_PASSIVE) },
    { MP_ROM_QSTR(MP_QSTR_BUS_OFF), MP_ROM_INT(CAN_STATE_BUS_OFF) },
};
STATIC MP_DEFINE_CONST_DICT(machine_can_locals_dict, machine_can_locals_dict_table);

STATIC mp_uint_t can_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (self->is_enabled && request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;

        if ((flags & MP_STREAM_POLL_RD)
            && FLEXCAN_GetMbStatusFlags(self->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag)) {
            ret |= MP_STREAM_POLL_RD;
        }

        if ((flags & MP_STREAM_POLL_WR)
            && can_count_txmb_pending(self)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = -1;
    }
    return ret;
}

STATIC const mp_stream_p_t can_stream_p = {
    .ioctl = can_ioctl,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_can_type,
    MP_QSTR_CAN,
    MP_TYPE_FLAG_NONE,
    make_new, machine_can_make_new,
    print, machine_can_print,
    protocol, &can_stream_p,
    locals_dict, &machine_can_locals_dict
    );

MP_REGISTER_ROOT_POINTER(void *machine_can_objects[MICROPY_HW_NUM_CAN_IRQS]);

#endif // MICROPY_HW_ENABLE_CAN
