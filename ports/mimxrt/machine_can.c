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
#include "py/gc.h"
#include "py/binary.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/runtime/mpirq.h"
#include "modmachine.h"
#include CLOCK_CONFIG_H

#include "fsl_iomuxc.h"
#include "fsl_flexcan.h"

#define CAN_MAX_DATA_FRAME              (8)

#define CAN_NORMAL_MODE                 (0)
#define CAN_LOOPBACK_FLAG               (1)
#define CAN_SILENT_FLAG                 (2)

#define CAN_DUAL                        (0)

#define CAN_PORT_PRINT_FUNCTION         (1)

#if MICROPY_HW_ENABLE_FDCAN
#define CAN_BRP_MIN 1
#define CAN_BRP_MAX 512
#define CAN_FD_BRS_BRP_MIN 1
#define CAN_FD_BRS_BRP_MAX 32
#define CAN_FILTERS_STD_EXT_SEPARATE 1

#else // Classic bxCAN
#define CAN_BRP_MIN 1
#define CAN_BRP_MAX 1024
#define CAN_FILTERS_STD_EXT_SEPARATE 0
#endif

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

struct machine_can_port {
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
};

typedef struct _iomux_table_t {
    uint32_t muxRegister;
    uint32_t muxMode;
    uint32_t inputRegister;
    uint32_t inputDaisy;
    uint32_t configRegister;
} iomux_table_t;

static const uint8_t can_index_table[] = MICROPY_HW_CAN_INDEX;
#ifdef MIMXRT117x_SERIES
static const uint32_t can_clock_index_table[] = {
    BOARD_BOOTCLOCKRUN_CAN1_CLK_ROOT,
    BOARD_BOOTCLOCKRUN_CAN2_CLK_ROOT,
    BOARD_BOOTCLOCKRUN_CAN3_CLK_ROOT
};
#endif
static CAN_Type *can_base_ptr_table[] = CAN_BASE_PTRS;
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
        machine_can_obj_t *machine_can_obj = MP_STATE_PORT(machine_can_objs[i]);
        if ((machine_can_obj != NULL) && (machine_can_obj->port->can_inst == base)) {
            self = machine_can_obj;
            break;
        }
    }
    if (self != NULL) {
        uint32_t result = FLEXCAN_GetStatusFlags(self->port->can_inst);
        if (result & FLEXCAN_ERROR_AND_STATUS_INIT_FLAG) {
            uint32_t flt_conf = (result & CAN_ESR1_FLTCONF_MASK) >> CAN_ESR1_FLTCONF_SHIFT;
            if (flt_conf > 1) {
                ++self->port->num_bus_off;
            } else if (flt_conf == 1) {
                ++self->port->num_error_passive;
            } else if ((result & CAN_ESR1_RXWRN_MASK) || (result & CAN_ESR1_TXWRN_MASK)) {
                ++self->port->num_error_warning;
            }
            FLEXCAN_ClearStatusFlags(self->port->can_inst, FLEXCAN_ERROR_AND_STATUS_INIT_FLAG);
        }

        mp_int_t irq_reason = -1;

        if (FLEXCAN_GetMbStatusFlags(self->port->can_inst, (uint32_t)kFLEXCAN_RxFifoOverflowFlag)) {
            FLEXCAN_DisableMbInterrupts(self->port->can_inst,
                kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
            irq_reason = 2;
        } else if (FLEXCAN_GetMbStatusFlags(self->port->can_inst, (uint32_t)kFLEXCAN_RxFifoWarningFlag)) {
            FLEXCAN_DisableMbInterrupts(self->port->can_inst, kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
            irq_reason = 1;
        } else if (FLEXCAN_GetMbStatusFlags(self->port->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag)) {
            FLEXCAN_DisableMbInterrupts(self->port->can_inst, kFLEXCAN_RxFifoFrameAvlFlag);
            irq_reason = 0;
        }

        if (irq_reason != -1 && self->port->callback != mp_const_none) {
            mp_sched_lock();
            gc_lock();
            nlr_buf_t nlr;
            if (nlr_push(&nlr) == 0) {
                mp_call_function_2(self->port->callback, MP_OBJ_FROM_PTR(self), MP_OBJ_NEW_SMALL_INT(irq_reason));
                nlr_pop();
            } else {
                // Uncaught exception; disable the callback so it doesn't run again.
                self->port->callback = mp_const_none;
                mp_printf(MICROPY_ERROR_PRINTER, "uncaught exception in CAN(%u) rx interrupt handler\n", self->can_idx);
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
    if (MP_STATE_PORT(machine_can_objs[self->can_idx]) != NULL) {
        mp_uint_t instance = FLEXCAN_GetInstance(self->port->can_inst);
        DisableIRQ(((IRQn_Type [])CAN_Rx_Warning_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_Tx_Warning_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_Error_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_Bus_Off_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_ORed_Message_buffer_IRQS)[instance]);
        FLEXCAN_DisableInterrupts(self->port->can_inst,
            kFLEXCAN_BusOffInterruptEnable | kFLEXCAN_ErrorInterruptEnable |
            kFLEXCAN_RxWarningInterruptEnable | kFLEXCAN_TxWarningInterruptEnable);
        FLEXCAN_DisableMbInterrupts(self->port->can_inst,
            kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
        MP_STATE_PORT(machine_can_objs[self->can_idx]) = NULL;
        self->port->is_enabled = false;
        FLEXCAN_Deinit(self->port->can_inst);
    }
}

// Deinit all can IRQ handlers.
void machine_can_irq_deinit(void) {
    for (int i = 0; i < MICROPY_HW_NUM_CAN_IRQS; ++i) {
        machine_can_obj_t *machine_can_obj = MP_STATE_PORT(machine_can_objs[i]);
        if (machine_can_obj != NULL) {
            machine_flexcan_deinit(machine_can_obj);
        }
    }
}

static void machine_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->port->is_enabled) {
        mp_printf(print, "CAN(%u)", self->can_idx);
    } else {
        qstr mode = MP_QSTR_MODE_NORMAL;
        if (self->port->flexcan_config->enableLoopBack) {
            if (self->port->flexcan_config->enableListenOnlyMode) {
                mode = MP_QSTR_MODE_SILENT_LOOPBACK;
            } else {
                mode = MP_QSTR_MODE_LOOPBACK;
            }
        } else if (self->port->flexcan_config->enableListenOnlyMode) {
            mode = MP_QSTR_MODE_SILENT;
        }
        mp_printf(print, "CAN(%u, mode=CAN.%q, auto_restart=%q, baudrate=%u)",
            self->can_idx,
            mode,
            (self->port->can_inst->CTRL1 & CAN_CTRL1_BOFFREC_MASK) ? MP_QSTR_False : MP_QSTR_True,
            self->port->flexcan_config->bitRate);
    }
}

// The port must provide implementations of these low-level CAN functions
static int machine_can_port_f_clock(const machine_can_obj_t *self) {
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
    return sourceClock_Hz;
}

static bool machine_can_port_supports_mode(const machine_can_obj_t *self, machine_can_mode_t mode) {
    return 0;
}

static void machine_can_port_clear_filters(machine_can_obj_t *self) {

}

static mp_uint_t machine_can_port_max_data_len(mp_uint_t flags){
    return 0;
}

// The extmod layer calls this function in a loop with incrementing filter_idx
// values. It's up to the port how to apply the filters from here, and to raise
// an exception if there are too many.
//
// If the CAN_FILTERS_STD_EXT_SEPARATE flag is set to 1, filter_idx will
// enumerate standard id filters separately to extended id filters (the
// CAN_MSG_FLAG_EXT_ID bit in 'flags' differentiates the type).
static void machine_can_port_set_filter(machine_can_obj_t *self, int filter_idx, mp_uint_t can_id, mp_uint_t mask, mp_uint_t flags) {

}

// Update interrupt configuration based on the new contents of 'self'
static void machine_can_update_irqs(machine_can_obj_t *self) {

}

// Return the irq().flags() result. Calling this function may also update the hardware state machine.
static mp_uint_t machine_can_port_irq_flags(machine_can_obj_t *self) {
    return 0;
}

static void machine_can_port_init(machine_can_obj_t *self) {
    // Get peripheral object.
    mp_uint_t can_hw_id = can_index_table[self->can_idx];  // the hw can number 1..n
    self->port->can_inst = can_base_ptr_table[can_hw_id];
    self->port->can_hw_id = can_hw_id;
}

static void machine_can_port_deinit(machine_can_obj_t *self) {
    ;
}

static mp_int_t machine_can_port_send(machine_can_obj_t *self, mp_uint_t id, const byte *data, size_t data_len, mp_uint_t flags) {
    return 0;
}

static bool machine_can_port_cancel_send(machine_can_obj_t *self, mp_uint_t idx) {
    return 0;
}

static bool machine_can_port_recv(machine_can_obj_t *self, void *data, size_t *dlen, mp_uint_t *id, mp_uint_t *flags, mp_uint_t *errors) {
    return 0;
}

static machine_can_state_t machine_can_port_get_state(machine_can_obj_t *self) {
    return 0;
}

static void machine_can_port_restart(machine_can_obj_t *self) {

}

// Updates values in self->counters (which counters are updated by this function versus from ISRs and the like
// is port specific
static void machine_can_port_update_counters(machine_can_obj_t *self) {

}

// Hook for port to fill in the final item of the get_timings() result list with controller-specific values
static mp_obj_t machine_can_port_get_additional_timings(machine_can_obj_t *self, mp_obj_t optional_arg) {
    return mp_const_none;
}
