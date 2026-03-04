/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_can.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.
#include <stdbool.h>
#include "extmod/machine_can_port.h"
#include "can.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/gc.h"

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

#define TX_EMPTY UINT32_MAX

struct machine_can_port {
    CAN_HandleTypeDef h;
    uint32_t tx[CAN_TX_QUEUE_LEN];  // ID stored in each hardware tx buffer, or TX_EMPTY if empty
    bool irq_state_pending;
    bool error_passive;
};

// Convert the port agnostic CAN mode to the ST mode
static uint32_t can_port_mode(machine_can_mode_t mode) {
    switch (mode) {
        case MP_CAN_MODE_NORMAL:
            return CAN_MODE_NORMAL;
        case MP_CAN_MODE_SLEEP:
            return CAN_MODE_SILENT; // Sleep is not an operating mode for ST's peripheral
        case MP_CAN_MODE_LOOPBACK:
            return CAN_MODE_LOOPBACK;
        case MP_CAN_MODE_SILENT:
            return CAN_MODE_SILENT;
        case MP_CAN_MODE_SILENT_LOOPBACK:
            return CAN_MODE_SILENT_LOOPBACK;
        default:
            assert(0); // Mode should have been checked already
            return CAN_MODE_NORMAL;
    }
}

static int machine_can_port_f_clock(const machine_can_obj_t *self) {
    return (int)can_get_source_freq();
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

static void machine_can_port_init(machine_can_obj_t *self) {
    if (!self->port) {
        self->port = m_new0(struct machine_can_port, 1);
        for (int i = 0; i < CAN_TX_QUEUE_LEN; i++) {
            self->port->tx[i] = TX_EMPTY;
        }
    }
    bool res = can_init(&self->port->h,
        self->can_idx + 1,
        CAN_TX_QUEUE,
        can_port_mode(self->mode),
        self->brp,
        self->sjw,
        self->tseg1,
        self->tseg2,
        self->auto_retransmit);

    if (!res) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("CAN init failed"));
    }
}

static void machine_can_port_cancel_all_tx(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;
    can_disable_tx_interrupts(&port->h);
    for (int i = 0; i < CAN_TX_QUEUE_LEN; i++) {
        can_cancel_transmit(&port->h, i);
        port->tx[i] = TX_EMPTY;
    }
}

static void machine_can_port_deinit(machine_can_obj_t *self) {
    machine_can_port_cancel_all_tx(self);
    can_deinit(&self->port->h);
}

static mp_int_t machine_can_port_send(machine_can_obj_t *self, mp_uint_t id, const byte *data, size_t data_len, mp_uint_t flags) {
    int idx_empty = -1; // Empty transmit buffer, where no later index has the same ID message in it

    // Scan through the current transmit queue to find an eligible buffer for transmit
    for (int i = 0; i < CAN_TX_QUEUE_LEN; i++) {
        uint32_t tx_id = self->port->tx[i];
        if (tx_id == TX_EMPTY) {
            // This slot is empty
            if (idx_empty == -1) {
                // Still have to keep scanning as we might see a later message with the same ID,
                idx_empty = i;
            }
        } else if (tx_id == id && !(flags & CAN_MSG_FLAG_UNORDERED)) {
            // Can't queue a second message with the same ID and guarantee order

            // (Undocumented hardware limitation - CANFD reference suggests
            // messages with the same ID are sent in buffer index order but
            // testing shows not always the case at least on STM32H7! Unsure if
            // also a limitation of bxCAN or STM32G4, but these only have 3 TX
            // buffers so inserting in buffer index order is likely to run out
            // of buffers relatively quickly anyway...)

            // Note: currently the driver considers a Standard and an Extended
            // ID with the same numeric value to be the same ID... could fix
            // this, although it's a relatively uncommon case.
            return -1;
        }
    }

    if (idx_empty == -1) {
        // No space in transmit queue
        return -1;
    }

    CanTxMsgTypeDef tx = {
        #if MICROPY_HW_ENABLE_FDCAN
        .MessageMarker = 0,
        .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
        .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
        .Identifier = id, // Range checked by caller
        .IdType = (flags & CAN_MSG_FLAG_EXT_ID) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID,
        .TxFrameType = (flags & CAN_MSG_FLAG_RTR) ? FDCAN_REMOTE_FRAME : FDCAN_DATA_FRAME,
        .FDFormat = (flags & CAN_MSG_FLAG_FD_F) ? FDCAN_FD_CAN : FDCAN_CLASSIC_CAN,
        .BitRateSwitch = (flags & CAN_MSG_FLAG_BRS) ? FDCAN_BRS_ON : FDCAN_BRS_OFF,
        .DataLength = data_len, // Converted inside can_transmit_buf_index
        #else // Classic
        .StdId = (flags & CAN_MSG_FLAG_EXT_ID) ? 0 : id,
        .ExtId = (flags & CAN_MSG_FLAG_EXT_ID) ? id : 0,
        .IDE = (flags & CAN_MSG_FLAG_EXT_ID) ? CAN_ID_EXT : CAN_ID_STD,
        .RTR = (flags & CAN_MSG_FLAG_RTR) ? CAN_RTR_REMOTE : CAN_RTR_DATA,
        .DLC = data_len,
        #endif
    };
    #if !MICROPY_HW_ENABLE_FDCAN
    assert(data_len <= sizeof(tx.Data)); // Also checked by caller
    memcpy(tx.Data, data, data_len);
    #endif

    HAL_StatusTypeDef err = can_transmit_buf_index(&self->port->h, idx_empty, &tx, data);
    if (err != HAL_OK) {
        return -1;
    }
    self->port->tx[idx_empty] = id;

    return idx_empty;
}

static bool machine_can_port_cancel_send(machine_can_obj_t *self, mp_uint_t idx) {
    return can_cancel_transmit(&self->port->h, idx);
}

static bool machine_can_port_recv(machine_can_obj_t *self, void *data, size_t *dlen, mp_uint_t *id, mp_uint_t *flags, mp_uint_t *errors) {
    CAN_HandleTypeDef *can = &self->port->h;
    CanRxMsgTypeDef msg;

    for (can_rx_fifo_t fifo = CAN_RX_FIFO0; fifo <= CAN_RX_FIFO1; fifo++) {
        if (can_receive(can, fifo, &msg, data, 0) == 0) {
            // CanRxMsgTypeDef is different for Classic vs FD
            #if MICROPY_HW_ENABLE_FDCAN
            *flags = ((msg.IdType == FDCAN_EXTENDED_ID) ? CAN_MSG_FLAG_EXT_ID : 0) |
                ((msg.RxFrameType == FDCAN_REMOTE_FRAME) ? CAN_MSG_FLAG_RTR : 0);
            *id = msg.Identifier;
            *dlen = msg.DataLength; // Lower layer has converted to bytes already
            #else
            *flags = (msg.IDE ? CAN_MSG_FLAG_EXT_ID : 0) |
                (msg.RTR ? CAN_MSG_FLAG_RTR : 0);
            *id = msg.IDE ? msg.ExtId : msg.StdId;
            *dlen = msg.DLC;
            #endif

            *errors = self->rx_error_flags;
            self->rx_error_flags = 0;

            // Re-enable any interrupts that were disabled in RX IRQ handlers
            can_enable_rx_interrupts(can, fifo, self->mp_irq_trigger & MP_CAN_IRQ_RX);

            return true;
        }
    }
    return false;
}

static void machine_can_update_irqs(machine_can_obj_t *self) {
    uint16_t triggers = self->mp_irq_trigger;

    for (can_rx_fifo_t fifo = CAN_RX_FIFO0; fifo <= CAN_RX_FIFO1; fifo++) {
        if (triggers & MP_CAN_IRQ_RX) {
            can_enable_rx_interrupts(&self->port->h, fifo, true);
        } else {
            can_disable_rx_interrupts(&self->port->h, fifo);
        }
    }

    // Note: TX complete interrupt is always enabled to manage the internal queue state
}

static void machine_can_port_clear_filters(machine_can_obj_t *self) {
    #if MICROPY_HW_ENABLE_FDCAN
    for (int f = 0; f < CAN_HW_MAX_STD_FILTER; f++) {
        can_clearfilter(&self->port->h, f, false);
    }
    for (int f = 0; f < CAN_HW_MAX_EXT_FILTER; f++) {
        can_clearfilter(&self->port->h, f, true);
    }
    #else
    for (int f = 0; f < CAN_HW_MAX_FILTER; f++) {
        can_clearfilter(&self->port->h, f, CAN_HW_MAX_FILTER);
    }
    #endif
}

#if MICROPY_HW_ENABLE_FDCAN
static void machine_can_port_set_filter(machine_can_obj_t *self, int filter_idx, mp_uint_t can_id, mp_uint_t mask, mp_uint_t flags) {
    int max_idx = (flags & CAN_MSG_FLAG_EXT_ID) ? CAN_HW_MAX_EXT_FILTER : CAN_HW_MAX_STD_FILTER;
    if (filter_idx >= max_idx) {
        mp_raise_ValueError(MP_ERROR_TEXT("Too many filters for this ID type"));
    }
    if (flags & ~CAN_MSG_FLAG_EXT_ID) {
        mp_raise_ValueError(MP_ERROR_TEXT("flags")); // Only supported flag is for extended ID
    }

    FDCAN_FilterTypeDef filter = {
        .IdType = (flags & CAN_MSG_FLAG_EXT_ID) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID,
        // FDCAN counts standard and extended id filters separately, but this is
        // already accounted for in filter_idx due to CAN_FILTERS_STD_EXT_SEPARATE.
        .FilterIndex = filter_idx,
        .FilterType = FDCAN_FILTER_MASK,
        // Round-robin between FIFO1 and FIFO0
        .FilterConfig = (filter_idx & 1) ? FDCAN_FILTER_TO_RXFIFO1 : FDCAN_FILTER_TO_RXFIFO0,
        .FilterID1 = can_id,
        .FilterID2 = mask,
    };

    int r = HAL_FDCAN_ConfigFilter(&self->port->h, &filter);
    assert(r == HAL_OK);
    (void)r;
}
#else
static void machine_can_port_set_filter(machine_can_obj_t *self, int filter_idx, mp_uint_t can_id, mp_uint_t mask, mp_uint_t flags) {
    uint32_t rtr_id_flag = 0; // Filtering on RTR flag specifically is not supported
    uint32_t rtr_mask_flag = 0;
    uint32_t id_type_bit = (flags & CAN_MSG_FLAG_EXT_ID) ? 4 : 0;
    CAN_FilterConfTypeDef filter = {
        .FilterActivation = ENABLE,
        .FilterScale = CAN_FILTERSCALE_32BIT,
        .FilterMode = CAN_FILTERMODE_IDMASK,
        // MSB of 32-bit identifier, see RM Figure 342 "Filter bank scale configuration"
        .FilterIdHigh = ((can_id & 0x7FF) << 5) | ((can_id >> 13) & 0x3f),
        // LSB of 32-bit identifier
        .FilterIdLow = ((can_id & 0x1fff) << 3) | id_type_bit | rtr_id_flag,
        // MSB of 32-bit mask
        .FilterMaskIdHigh = ((mask & 0x7FF) << 5) | ((mask >> 13) & 0x3f),
        .FilterMaskIdLow = ((mask & 0x1fff) << 3) | id_type_bit | rtr_mask_flag,
        .BankNumber = self->can_idx == 1 ? CAN_HW_MAX_FILTER : 0, // CAN2 needs BankNumber set
    };

    if (filter_idx >= CAN_HW_MAX_FILTER) {
        mp_raise_ValueError(MP_ERROR_TEXT("Too many filters"));
    }
    if (flags & ~CAN_MSG_FLAG_EXT_ID) {
        mp_raise_ValueError(MP_ERROR_TEXT("flags")); // Only supported flag is for extended ID
    }

    // Apply the filters round-robin to each FIFO, as each filter in bxCAN is
    // associated with only one FIFO.
    int fifo = filter_idx % 2;

    filter.FilterNumber = filter_idx;
    filter.FilterFIFOAssignment = fifo;
    int r = HAL_CAN_ConfigFilter(&self->port->h, &filter);
    assert(r == HAL_OK); // Params should be verified before passing to HAL
    (void)r;
}
#endif // MICROPY_HW_ENABLE_FDCAN

static machine_can_state_t machine_can_port_get_state(machine_can_obj_t *self) {
    // machine_can_port.h defines MP_CAN_STATE_xxx enums, verify they all match
    // numerically with stm32 can.h CAN_STATE_xxx enums
    MP_STATIC_ASSERT((int)MP_CAN_STATE_STOPPED == (int)CAN_STATE_STOPPED);
    MP_STATIC_ASSERT((int)MP_CAN_STATE_ACTIVE == (int)CAN_STATE_ERROR_ACTIVE);
    MP_STATIC_ASSERT((int)MP_CAN_STATE_WARNING == (int)CAN_STATE_ERROR_WARNING);
    MP_STATIC_ASSERT((int)MP_CAN_STATE_PASSIVE == (int)CAN_STATE_ERROR_PASSIVE);
    MP_STATIC_ASSERT((int)MP_CAN_STATE_BUS_OFF == (int)CAN_STATE_BUS_OFF);
    return (machine_can_state_t)can_get_state(&self->port->h);
}

static void machine_can_port_update_counters(machine_can_obj_t *self) {
    can_counters_t hw_counters;
    struct machine_can_port *port = self->port;
    machine_can_counters_t *counters = &self->counters;

    can_get_counters(&port->h, &hw_counters);

    counters->tec = hw_counters.tec;
    counters->rec = hw_counters.rec;
    counters->tx_pending = hw_counters.tx_pending;
    counters->rx_pending = hw_counters.rx_fifo0_pending + hw_counters.rx_fifo1_pending;

    // Other fields in 'counters' are updated from ISR directly
}

static mp_obj_t machine_can_port_get_additional_timings(machine_can_obj_t *self, mp_obj_t optional_arg) {
    return mp_const_none;
}

static void machine_can_port_restart(machine_can_obj_t *self) {
    // extmod layer has already checked CAN is initialised
    struct machine_can_port *port = self->port;
    machine_can_port_cancel_all_tx(self);
    can_restart(&port->h);
    port->irq_state_pending = false;
}

static bool clear_complete_transfer(machine_can_obj_t *self, int *index, bool *is_success) {
    *index = can_get_transmit_finished(&self->port->h, is_success);
    if (*index == -1) {
        return false;
    }
    self->port->tx[*index] = TX_EMPTY;

    return true;
}

static mp_uint_t machine_can_port_irq_flags(machine_can_obj_t *self) {
    mp_uint_t flags = 0;
    CAN_HandleTypeDef *can = &self->port->h;

    if (self->mp_irq_trigger & MP_CAN_IRQ_STATE && self->port->irq_state_pending) {
        flags |= MP_CAN_IRQ_STATE;
        self->port->irq_state_pending = false;
    }

    // Check for RX
    if (self->mp_irq_trigger & MP_CAN_IRQ_RX) {
        for (can_rx_fifo_t fifo = CAN_RX_FIFO0; fifo <= CAN_RX_FIFO1; fifo++) {
            if (can_is_rx_pending(can, fifo)) {
                flags |= MP_CAN_IRQ_RX;
            }
        }
    }

    // Check for TX done
    if (self->mp_irq_trigger & MP_CAN_IRQ_TX) {
        bool is_success = false;
        int index;
        if (clear_complete_transfer(self, &index, &is_success)) {
            flags |= (mp_uint_t)(index << MP_CAN_IRQ_IDX_SHIFT) | MP_CAN_IRQ_TX;
            if (!is_success) {
                flags |= MP_CAN_IRQ_TX_FAILED;
            }
        }
    }

    return flags;
}

void machine_can_irq_handler(uint can_id,  can_int_t interrupt) {
    assert(can_id > 0);
    machine_can_obj_t *self = MP_STATE_PORT(machine_can_objs)[can_id - 1];
    struct machine_can_port *port = self->port;
    machine_can_counters_t *counters;
    bool call_irq = false;
    bool irq_state = false;
    if (self == NULL) {
        return;
    }
    counters = &self->counters;

    switch (interrupt) {
        // RX
        case CAN_INT_FIFO_FULL:
            self->rx_error_flags |= CAN_RECV_ERR_FULL;
            break;
        case CAN_INT_FIFO_OVERFLOW:
            self->rx_error_flags |= CAN_RECV_ERR_OVERRUN;
            counters->rx_overruns++;
            break;
        case CAN_INT_MESSAGE_RECEIVED:
            call_irq = call_irq || (self->mp_irq_trigger & MP_CAN_IRQ_RX);
            break;

        // Error states
        case CAN_INT_ERR_WARNING:
            if (!port->error_passive) {
                // Only count entering warning state, not leaving it
                counters->num_warning++;
                irq_state = true;
            }
            port->error_passive = false;
            break;
        case CAN_INT_ERR_PASSIVE:
            counters->num_passive++;
            port->error_passive = true;
            irq_state = true;
            break;
        case CAN_INT_ERR_BUS_OFF:
            counters->num_bus_off++;
            irq_state = true;
            port->error_passive = false;
            break;

        // TX
        case CAN_INT_TX_COMPLETE:
            if (!(self->mp_irq_trigger & MP_CAN_IRQ_TX)) {
                // No TX IRQ, so mark this buffer as free and move on
                int index;
                bool is_success = false;
                clear_complete_transfer(self, &index, &is_success);
            } else {
                // Otherwise, the slot is marked empty after the irq calls flags()
                call_irq = true;
            }
            break;

        default:
            assert(0); // Should be unreachable
    }

    if (irq_state && (self->mp_irq_trigger & MP_CAN_IRQ_STATE)) {
        self->port->irq_state_pending = true;
        call_irq = true;
    }

    if (call_irq) {
        assert(self->mp_irq_obj != NULL); // Can't set mp_irq_trigger otherwise
        mp_irq_handler(self->mp_irq_obj);
    }
}
