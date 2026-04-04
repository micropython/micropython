/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Kwabena W. Agyeman
 * Copyright (c) 2026 Robert Hammelrath
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

#define CAN_PORT_PRINT_FUNCTION         (1)

// Port-specific IRQ flags
#define MP_CAN_IRQ_RX_OVERFLOW      (1 << 4)
#define MP_CAN_IRQ_RX_WARNING       (1 << 5)

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
#define kFLEXCAN_TxMbInactive           (0x8)
#define kFLEXCAN_TxMbAbort              (0x9)
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

typedef struct machine_can_port {
    uint8_t can_hw_id;
    CAN_Type *can_inst;
    flexcan_config_t *flexcan_config;
    flexcan_rx_fifo_config_t *flexcan_rx_fifo_config;
    uint8_t flexcan_txmb_start;
    uint8_t flexcan_txmb_count;
    uint16_t mp_irq_flags;
    bool is_enabled;
    uint16_t num_error_warning;
    uint16_t num_error_passive;
    uint16_t num_bus_off;
} machine_can_port_t;

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
        struct machine_can_port *port = self->port;
        uint32_t result = FLEXCAN_GetStatusFlags(port->can_inst);
        if (result & FLEXCAN_ERROR_AND_STATUS_INIT_FLAG) {
            uint32_t flt_conf = (result & CAN_ESR1_FLTCONF_MASK) >> CAN_ESR1_FLTCONF_SHIFT;
            if (flt_conf > 1) {
                ++port->num_bus_off;
            } else if (flt_conf == 1) {
                ++port->num_error_passive;
            } else if ((result & CAN_ESR1_RXWRN_MASK) || (result & CAN_ESR1_TXWRN_MASK)) {
                ++port->num_error_warning;
            }
            FLEXCAN_ClearStatusFlags(port->can_inst, FLEXCAN_ERROR_AND_STATUS_INIT_FLAG);
        }

        mp_int_t irq_flags = 0;

        // Maybe not disable the interrupts.
        if (FLEXCAN_GetMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoOverflowFlag)) {
            FLEXCAN_DisableMbInterrupts(port->can_inst,
                kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
            irq_flags |= MP_CAN_IRQ_RX_OVERFLOW;
        }
        if (FLEXCAN_GetMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoWarningFlag)) {
            FLEXCAN_DisableMbInterrupts(port->can_inst, kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
            irq_flags |= MP_CAN_IRQ_RX_WARNING;
        }
        if (FLEXCAN_GetMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag)) {
            FLEXCAN_DisableMbInterrupts(port->can_inst, kFLEXCAN_RxFifoFrameAvlFlag);
            irq_flags |= MP_CAN_IRQ_RX;
        }
        port->mp_irq_flags = irq_flags;

        if (irq_flags & self->mp_irq_trigger) {
            mp_irq_handler(self->mp_irq_obj);
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

static void machine_can_port_deinit(machine_can_obj_t *self) {
    if (MP_STATE_PORT(machine_can_objs[self->can_idx]) != NULL) {
        struct machine_can_port *port = self->port;
        mp_uint_t instance = FLEXCAN_GetInstance(port->can_inst);
        DisableIRQ(((IRQn_Type [])CAN_Rx_Warning_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_Tx_Warning_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_Error_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_Bus_Off_IRQS)[instance]);
        DisableIRQ(((IRQn_Type [])CAN_ORed_Message_buffer_IRQS)[instance]);
        FLEXCAN_DisableInterrupts(port->can_inst,
            kFLEXCAN_BusOffInterruptEnable | kFLEXCAN_ErrorInterruptEnable |
            kFLEXCAN_RxWarningInterruptEnable | kFLEXCAN_TxWarningInterruptEnable);
        FLEXCAN_DisableMbInterrupts(port->can_inst,
            kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
        MP_STATE_PORT(machine_can_objs[self->can_idx]) = NULL;
        port->is_enabled = false;
        FLEXCAN_Deinit(port->can_inst);
    }
}

// Deinit all can IRQ handlers.
void machine_can_irq_deinit(void) {
    for (int i = 0; i < MICROPY_HW_NUM_CAN_IRQS; ++i) {
        machine_can_obj_t *machine_can_obj = MP_STATE_PORT(machine_can_objs[i]);
        if (machine_can_obj != NULL) {
            machine_can_port_deinit(machine_can_obj);
        }
    }
}

#if CAN_PORT_PRINT_FUNCTION
static void machine_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct machine_can_port *port = self->port;

    if (!port->is_enabled) {
        mp_printf(print, "CAN(%u)", self->can_idx + 1);
    } else {
        qstr mode = MP_QSTR_MODE_NORMAL;
        if (port->flexcan_config->enableLoopBack) {
            if (port->flexcan_config->enableListenOnlyMode) {
                mode = MP_QSTR_MODE_SILENT_LOOPBACK;
            } else {
                mode = MP_QSTR_MODE_LOOPBACK;
            }
        } else if (port->flexcan_config->enableListenOnlyMode) {
            mode = MP_QSTR_MODE_SILENT;
        }
        mp_printf(print, "CAN(%u, bitrate=%u, mode=CAN.%q, auto_restart=%q)",
            self->can_idx + 1,
            port->flexcan_config->bitRate,
            mode,
            (port->can_inst->CTRL1 & CAN_CTRL1_BOFFREC_MASK) ? MP_QSTR_False : MP_QSTR_True);
    }
}
#endif

// Convert the port agnostic CAN mode to the ST mode
static uint32_t can_port_mode(machine_can_mode_t mode) {
    switch (mode) {
        case MP_CAN_MODE_NORMAL:
            return CAN_NORMAL_MODE;
        case MP_CAN_MODE_SLEEP:
            return CAN_SILENT_FLAG; // Sleep is not an operating mode for ST's peripheral
        case MP_CAN_MODE_LOOPBACK:
            return CAN_LOOPBACK_FLAG;
        case MP_CAN_MODE_SILENT:
            return CAN_SILENT_FLAG;
        case MP_CAN_MODE_SILENT_LOOPBACK:
            return CAN_LOOPBACK_FLAG | CAN_SILENT_FLAG;
        default:
            assert(0); // Mode should have been checked already
            return CAN_NORMAL_MODE;
    }
}
static void machine_can_port_init(machine_can_obj_t *self) {
    // Get peripheral object and do the first level config.
    struct machine_can_port *port = self->port;

    if (port == NULL) {
        port = m_new_obj(machine_can_port_t);
        self->port = port;
        mp_uint_t can_hw_id = can_index_table[self->can_idx];  // the hw can number 1..n
        port->can_inst = can_base_ptr_table[can_hw_id];
        port->can_hw_id = can_hw_id;
        port->flexcan_config = m_new_obj(flexcan_config_t);
        FLEXCAN_GetDefaultConfig(port->flexcan_config);
        mp_uint_t maxMbNum = FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(port->can_inst);
        port->flexcan_config->maxMbNum = maxMbNum;
        port->flexcan_config->disableSelfReception = true;

        port->flexcan_rx_fifo_config = m_new_obj(flexcan_rx_fifo_config_t);
        mp_uint_t idFilterNum = maxMbNum * 2;
        port->flexcan_rx_fifo_config->idFilterNum = idFilterNum;
        port->flexcan_rx_fifo_config->idFilterType = kFLEXCAN_RxFifoFilterTypeA;
        port->flexcan_rx_fifo_config->priority = kFLEXCAN_RxFifoPrioHigh;
        port->flexcan_rx_fifo_config->idFilterTable = m_new0(uint32_t, idFilterNum);

        // Configure board-specific pin MUX based on the hardware device number.
        can_set_iomux(can_hw_id);

        // When selecting the CCM CAN clock source with CAN_CLK_SEL set to 2, the UART clock gate
        // will not open and CAN_CLK_ROOT will be off. To avoid this issue, set CAN_CLK_SEL to 0 or
        // 1 for CAN clock selection, or open the UART clock gate by configuring the CCM_CCGRx register.
        // There are two workarounds:
        // Set CAN_CLK_SEL to 0 or 1 for CAN clock selection, or if CAN_CLK_SEL is set to 2,
        // then the CCM must open any of UART clock gate by configuring the CCM_CCGRx register.
        #if (defined(FSL_FEATURE_CCM_HAS_ERRATA_50235) && FSL_FEATURE_CCM_HAS_ERRATA_50235)
        CLOCK_EnableClock(kCLOCK_Lpuart1);
        #endif // FSL_FEATURE_CCM_HAS_ERRATA_50235
    }

    // (Re-)configuration after init().
    port->flexcan_config->enableLoopBack = can_port_mode(self->mode) & CAN_LOOPBACK_FLAG;
    port->flexcan_config->enableListenOnlyMode = can_port_mode(self->mode) & CAN_SILENT_FLAG;
    port->flexcan_config->bitRate = self->bitrate;
    port->flexcan_config->enableIndividMask = true;

    uint32_t sourceClock_Hz = machine_can_port_f_clock(self);

    // Initialise the CAN peripheral.
    FLEXCAN_Init(port->can_inst, port->flexcan_config, sourceClock_Hz);
    memset(port->flexcan_rx_fifo_config->idFilterTable, 0,
        sizeof(uint32_t) * port->flexcan_rx_fifo_config->idFilterNum);
    FLEXCAN_SetRxFifoConfig(port->can_inst, port->flexcan_rx_fifo_config, true);

    // Calculate the Number of Mailboxes occupied by RX Legacy FIFO and the filter.
    mp_uint_t rffn = (uint8_t)((port->can_inst->CTRL2 & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
    port->flexcan_txmb_start = 6U + (rffn + 1U) * 2U;
    #if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
    (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
    // the first valid MB should be occupied by ERRATA 5461 or 5829.
    port->flexcan_txmb_start += 1U;
    #endif
    port->flexcan_txmb_count = port->flexcan_config->maxMbNum - port->flexcan_txmb_start;

    for (mp_uint_t i = 0; i < port->flexcan_txmb_count; i++) {
        FLEXCAN_SetTxMbConfig(port->can_inst, port->flexcan_txmb_start + i, true);
    }

    port->is_enabled = true;
    port->num_error_warning = 0;
    port->num_error_passive = 0;
    port->num_bus_off = 0;

    FLEXCAN_EnableMbInterrupts(port->can_inst,
        kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);

    FLEXCAN_EnableInterrupts(port->can_inst,
        kFLEXCAN_BusOffInterruptEnable | kFLEXCAN_ErrorInterruptEnable |
        kFLEXCAN_RxWarningInterruptEnable | kFLEXCAN_TxWarningInterruptEnable);

    mp_uint_t instance = FLEXCAN_GetInstance(port->can_inst);
    EnableIRQ(((IRQn_Type [])CAN_Rx_Warning_IRQS)[instance]);
    EnableIRQ(((IRQn_Type [])CAN_Tx_Warning_IRQS)[instance]);
    EnableIRQ(((IRQn_Type [])CAN_Error_IRQS)[instance]);
    EnableIRQ(((IRQn_Type [])CAN_Bus_Off_IRQS)[instance]);
    EnableIRQ(((IRQn_Type [])CAN_ORed_Message_buffer_IRQS)[instance]);

    // Store the configured timing parameters in the CAN object for reporting.
    self->brp = port->flexcan_config->timingConfig.preDivider;
    self->sjw = port->flexcan_config->timingConfig.rJumpwidth;
    self->tseg1 = port->flexcan_config->timingConfig.phaseSeg1 + port->flexcan_config->timingConfig.propSeg + 2;
    self->tseg2 = port->flexcan_config->timingConfig.phaseSeg2 + 1;

}

// The port must provide implementations of these low-level CAN functions
static int machine_can_port_f_clock(const machine_can_obj_t *self) {
    uint32_t sourceClock_Hz;
    #ifdef MIMXRT117x_SERIES
    sourceClock_Hz = can_clock_index_table[self->can_hw_id];
    #else
    sourceClock_Hz = BOARD_BOOTCLOCKRUN_CAN_CLK_ROOT;
    #endif
    return sourceClock_Hz;
}

static bool machine_can_port_supports_mode(const machine_can_obj_t *self, machine_can_mode_t mode) {
    return mode < MP_CAN_MODE_MAX;
}

static mp_uint_t machine_can_port_max_data_len(mp_uint_t flags) {
    #if MICROPY_HW_ENABLE_FDCAN
    if (flags & CAN_MSG_FLAG_FD_F) {
        return 64;
    }
    #endif
    return 8;
}

// Clear all filters
static void machine_can_port_clear_filters(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;
    mp_uint_t bank;
    for (bank = 0; bank < port->flexcan_config->maxMbNum; bank++) {
        port->flexcan_rx_fifo_config->idFilterTable[bank * 2] = 0;
        port->flexcan_rx_fifo_config->idFilterTable[(bank * 2) + 1] = 0;
    }
    FLEXCAN_SetRxFifoConfig(port->can_inst, port->flexcan_rx_fifo_config, true);
}

// The extmod layer calls this function in a loop with incrementing filter_idx
// values. It's up to the port how to apply the filters from here, and to raise
// an exception if there are too many.
//
// If the CAN_FILTERS_STD_EXT_SEPARATE flag is set to 1, filter_idx will
// enumerate standard id filters separately to extended id filters (the
// CAN_MSG_FLAG_EXT_ID bit in 'flags' differentiates the type).
static void machine_can_port_set_filter(machine_can_obj_t *self, int filter_idx, mp_uint_t can_id, mp_uint_t mask, mp_uint_t flags) {
    struct machine_can_port *port = self->port;

    if (filter_idx >= port->flexcan_config->maxMbNum) {
        return;
    }
    if (flags & CAN_MSG_FLAG_EXT_ID) {
        can_id = FLEXCAN_ID_EXT(can_id) | (1 << 29);
        mask = FLEXCAN_ID_EXT(mask) | (1 << 29);
    } else {
        can_id = FLEXCAN_ID_STD(can_id);
        mask = FLEXCAN_ID_STD(mask);
    }
    if (flags & CAN_MSG_FLAG_RTR) {
        can_id |= 1 << 30;
        mask |= 1 << 30;
    }
    port->flexcan_rx_fifo_config->idFilterTable[filter_idx] = can_id << 1;
    if (filter_idx < 64) {
        FLEXCAN_EnterFreezeMode(port->can_inst);
        port->can_inst->RXIMR[filter_idx] = mask << 1;
        FLEXCAN_ExitFreezeMode(port->can_inst);
    }
    FLEXCAN_SetRxFifoConfig(port->can_inst, port->flexcan_rx_fifo_config, true);
}

// Update interrupt configuration based on the new contents of 'self'
static void machine_can_update_irqs(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;
    uint16_t triggers = self->mp_irq_trigger;
    mp_uint_t enable_flags = 0;
    if (triggers & MP_CAN_IRQ_RX) {
        enable_flags |= kFLEXCAN_RxFifoFrameAvlFlag;
    }
    if (triggers & MP_CAN_IRQ_RX_OVERFLOW) {
        enable_flags |= kFLEXCAN_RxFifoOverflowFlag;
    }
    if (triggers & MP_CAN_IRQ_RX_WARNING) {
        enable_flags |= kFLEXCAN_RxFifoWarningFlag;
    }
    FLEXCAN_DisableMbInterrupts(port->can_inst,
        ~enable_flags & (kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag));
    FLEXCAN_EnableMbInterrupts(port->can_inst, enable_flags);
}

// Return the irq().flags() result. Calling this function may also update the hardware state machine.
static mp_uint_t machine_can_port_irq_flags(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;
    mp_int_t irq_flags = 0;

    // Maybe not disable the interrupts.
    if (FLEXCAN_GetMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoOverflowFlag)) {
        irq_flags |= MP_CAN_IRQ_RX_OVERFLOW;
    }
    if (FLEXCAN_GetMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoWarningFlag)) {
        irq_flags |= MP_CAN_IRQ_RX_WARNING;
    }
    if (FLEXCAN_GetMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag)) {
        irq_flags |= MP_CAN_IRQ_RX;
    }
    return irq_flags;
}

static mp_uint_t can_find_txmb(machine_can_obj_t *self, flexcan_frame_t *frame) {
    struct machine_can_port *port = self->port;

    // See if this frame id has been used before. If so, reuse the same mailbox to keep message ordering.
    for (mp_uint_t i = 0; i < port->flexcan_txmb_count; i++) {
        uint32_t cs_temp = port->can_inst->MB[port->flexcan_txmb_start + i].CS;
        if (((frame->format == kFLEXCAN_FrameFormatStandard) || (cs_temp & CAN_CS_IDE_MASK))
            && ((frame->type == kFLEXCAN_FrameTypeData) || (cs_temp & CAN_CS_RTR_MASK))
            && (port->can_inst->MB[port->flexcan_txmb_start + i].ID == frame->id)) {
            if ((cs_temp & CAN_CS_CODE_MASK) != CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote)) {
                return port->flexcan_txmb_start + i;
            } else {
                return 0;
            }
        }
    }
    // Frame id has never been used before so just pick the first empty mailbox.
    for (mp_uint_t i = 0; i < port->flexcan_txmb_count; i++) {
        uint32_t cs_temp = port->can_inst->MB[port->flexcan_txmb_start + i].CS;
        if ((cs_temp & CAN_CS_CODE_MASK) != CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote)) {
            return port->flexcan_txmb_start + i;
        }
    }
    return 0;
}

static mp_int_t machine_can_port_send(machine_can_obj_t *self, mp_uint_t id, const byte *data, size_t data_len, mp_uint_t flags) {

    flexcan_frame_t tx_msg;

    tx_msg.dataWord0 = 0;
    tx_msg.dataWord1 = 0;

    if (data_len > 0) {
        tx_msg.dataByte0 = data[0];
    }
    if (data_len > 1) {
        tx_msg.dataByte1 = data[1];
    }
    if (data_len > 2) {
        tx_msg.dataByte2 = data[2];
    }
    if (data_len > 3) {
        tx_msg.dataByte3 = data[3];
    }
    if (data_len > 4) {
        tx_msg.dataByte4 = data[4];
    }
    if (data_len > 5) {
        tx_msg.dataByte5 = data[5];
    }
    if (data_len > 6) {
        tx_msg.dataByte6 = data[6];
    }
    if (data_len > 7) {
        tx_msg.dataByte7 = data[7];
    }
    tx_msg.length = data_len;
    tx_msg.type = (flags & CAN_MSG_FLAG_RTR) != 0;
    tx_msg.format = (flags & CAN_MSG_FLAG_EXT_ID) != 0;

    if (tx_msg.format) {
        tx_msg.id = FLEXCAN_ID_EXT(id);
    } else {
        tx_msg.id = FLEXCAN_ID_STD(id);
    }

    uint32_t timeout_ms = 1000;
    uint32_t start = mp_hal_ticks_ms();
    mp_uint_t mbIdx;

    while (true) {
        mbIdx = can_find_txmb(self, &tx_msg);
        if (mbIdx && (FLEXCAN_WriteTxMb(self->port->can_inst, mbIdx, &tx_msg) == kStatus_Success)) {
            break;
        }
        if (timeout_ms == 0) {
            return -1;
        }
        // Check for the Timeout
        if (timeout_ms != UINT32_MAX) {
            if (mp_hal_ticks_ms() - start >= timeout_ms) {
                return -1;
            }
        }
        MICROPY_EVENT_POLL_HOOK
    }
    return mbIdx - self->port->flexcan_txmb_start;
}

static bool machine_can_port_cancel_send(machine_can_obj_t *self, mp_uint_t idx) {
    struct machine_can_port *port = self->port;

    if (idx >= port->flexcan_txmb_count) {
        return false;
    }
    port->can_inst->MB[idx + port->flexcan_txmb_start].CS = CAN_CS_CODE(kFLEXCAN_TxMbAbort);
    // Missing: Check that the transmit was aborted as defined in RM 40.7.7.1
    return true;
}

static bool machine_can_port_recv(machine_can_obj_t *self, void *data, size_t *dlen, mp_uint_t *id, mp_uint_t *flags, mp_uint_t *errors) {
    struct machine_can_port *port = self->port;

    if (!FLEXCAN_GetMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag)) {
        return false;
    }

    flexcan_frame_t rx_frame;
    status_t status = FLEXCAN_ReadRxFifo(port->can_inst, &rx_frame);
    FLEXCAN_ClearMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag);
    *errors = status;

    FLEXCAN_EnableMbInterrupts(port->can_inst,
        kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);

    if (status != kStatus_Success) {
        return false;
    }

    *dlen = rx_frame.length;
    uint8_t *rx_data = data;

    if (*dlen > 0) {
        rx_data[0] = rx_frame.dataByte0;
    }
    if (*dlen > 1) {
        rx_data[1] = rx_frame.dataByte1;
    }
    if (*dlen > 2) {
        rx_data[2] = rx_frame.dataByte2;
    }
    if (*dlen > 3) {
        rx_data[3] = rx_frame.dataByte3;
    }
    if (*dlen > 4) {
        rx_data[4] = rx_frame.dataByte4;
    }
    if (*dlen > 5) {
        rx_data[5] = rx_frame.dataByte5;
    }
    if (*dlen > 6) {
        rx_data[6] = rx_frame.dataByte6;
    }
    if (*dlen > 7) {
        rx_data[7] = rx_frame.dataByte7;
    }

    *flags = (rx_frame.format ? CAN_MSG_FLAG_EXT_ID : 0) |
        (rx_frame.type ? CAN_MSG_FLAG_RTR : 0);
    *id = (rx_frame.id & 0x3fffffff) >> (rx_frame.format ? 0 : 18);

    return true;
}

static machine_can_state_t machine_can_port_get_state(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;

    if (port->is_enabled) {
        uint32_t result = FLEXCAN_GetStatusFlags(port->can_inst);
        uint32_t flt_conf = (result & CAN_ESR1_FLTCONF_MASK) >> CAN_ESR1_FLTCONF_SHIFT;
        if (flt_conf > 1) {
            return MP_CAN_STATE_BUS_OFF;
        }
        if (flt_conf == 1) {
            return MP_CAN_STATE_PASSIVE;
        }
        if ((result & CAN_ESR1_RXWRN_MASK) || (result & CAN_ESR1_TXWRN_MASK)) {
            return MP_CAN_STATE_WARNING;
        }
        // flt_conf == 0
        return MP_CAN_STATE_ACTIVE;
    }
    return MP_CAN_STATE_STOPPED;
}

static void machine_can_port_restart(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;
    // Disable FLEXCAN MB interrupts
    FLEXCAN_DisableMbInterrupts(port->can_inst,
        kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
    // Disable the MP IRQ
    self->mp_irq_obj->handler = NULL;
    self->mp_irq_trigger = 0;
    // Clear counts
    port->num_error_warning = 0;
    port->num_error_passive = 0;
    port->num_bus_off = 0;
    // Cancel all pending TX MBs
    for (mp_uint_t idx = 0; idx < port->flexcan_txmb_count; idx++) {
        port->can_inst->MB[idx + port->flexcan_txmb_start].CS = CAN_CS_CODE(kFLEXCAN_TxMbAbort);
    }
    // Drain the FIFO
    flexcan_frame_t rx_frame;
    while (FLEXCAN_GetMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag) != 0) {
        FLEXCAN_ReadRxFifo(port->can_inst, &rx_frame);
        FLEXCAN_ClearMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag);
    }
    // Clear all filters
    // machine_can_port_clear_filters(self);
    // Re-enable FLEXCAN MB interrupts
    FLEXCAN_EnableMbInterrupts(port->can_inst,
        kFLEXCAN_RxFifoOverflowFlag | kFLEXCAN_RxFifoWarningFlag | kFLEXCAN_RxFifoFrameAvlFlag);
}

static mp_uint_t can_count_txmb_pending(machine_can_port_t *port) {
    mp_uint_t count = 0;
    for (mp_uint_t i = 0; i < port->flexcan_txmb_count; i++) {
        uint32_t cs_temp = port->can_inst->MB[port->flexcan_txmb_start + i].CS;
        if ((cs_temp & CAN_CS_CODE_MASK) == CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote)) {
            count++;
        }
    }
    return count;
}

// Updates values in self->counters (which counters are updated by this function versus from ISRs and the like
// is port specific
static void machine_can_port_update_counters(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;
    machine_can_counters_t *counters = &self->counters;

    uint8_t tec;
    uint8_t rec;
    FLEXCAN_GetBusErrCount(port->can_inst, &tec, &rec);
    counters->tec = tec;
    counters->rec = rec;
    counters->num_warning = port->num_error_warning;
    counters->num_passive = port->num_error_passive;
    counters->num_bus_off = port->num_bus_off;
    counters->tx_pending = can_count_txmb_pending(port);
    counters->rx_pending = FLEXCAN_GetMbStatusFlags(port->can_inst, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag) != 0;
    counters->rx_overruns = 0;
}

// Hook for port to fill in the final item of the get_timings() result list with controller-specific values
static mp_obj_t machine_can_port_get_additional_timings(machine_can_obj_t *self, mp_obj_t optional_arg) {
    return mp_const_none;
}
