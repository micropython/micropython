/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Kwabena W. Agyeman
 * Copyright (c) 2024-2026 Angus Gratton
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

#include <stdbool.h>
#include "extmod/machine_can_port.h"
#include "canfd.h"
#include "sys_ctrl_canfd.h"
#include "RTE_Device.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/gc.h"
#include "shared/runtime/mpirq.h"

#define CAN_BRP_MIN         2
#define CAN_BRP_MAX         255

#define CAN_TSEG1_MIN       2
#define CAN_TSEG1_MAX       255
#define CAN_TSEG2_MIN       1
#define CAN_TSEG2_MAX       255
#define CAN_SJW_MIN         1
#define CAN_SJW_MAX         127
#define CAN_TX_QUEUE_LEN    18
#define CAN_HW_MAX_FILTER   64

#if RTE_CANFD_CLK_SOURCE
    #define CANFD_CLK_DIVISOR (CANFD_CLK_SRC_160MHZ_CLK / RTE_CANFD_CLK_SPEED)
    #if ((CANFD_CLK_DIVISOR < 2U) || (CANFD_CLK_DIVISOR > 255U))
        #error "Incorrect CANFD Clock speed"
    #endif
#else
    #define CANFD_CLK_DIVISOR (CANFD_CLK_SRC_38P4MHZ_CLK / RTE_CANFD_CLK_SPEED)
    #if ((CANFD_CLK_DIVISOR < 2U) || (CANFD_CLK_DIVISOR > 255U))
        #error "Incorrect CANFD Clock speed"
    #endif
#endif

/****** CAN ID Frame Format codes *****/
#define ARM_CAN_ID_IDE_Pos            31UL
#define ARM_CAN_ID_IDE_Msk            (1UL << ARM_CAN_ID_IDE_Pos)

/****** CAN Identifier encoding *****/
#define ARM_CAN_STANDARD_ID(id)       (id & 0x000007FFUL)                         // < CAN identifier in standard format (11-bits)
#define ARM_CAN_EXTENDED_ID(id)       ((id & 0x1FFFFFFFUL) | ARM_CAN_ID_IDE_Msk)  // < CAN identifier in extended format (29-bits)

typedef struct machine_can_port {
    uint8_t can_hw_id;
    CANFD_Type *canfd_base;
    CANFD_CNT_Type *canfd_cnt_base;
    machine_can_state_t can_state;
    machine_can_mode_t can_mode;
    bool is_enabled;
    mp_int_t irq_flags;
    uint16_t num_error_warning;
    uint16_t num_error_passive;
    uint16_t num_bus_off;
    uint16_t num_rx_overrun;
    canfd_transfer_t data_transfer;
    canfd_acpt_fltr_t filter_config[CANFD_MAX_ACCEPTANCE_FILTERS];
    int num_canfd_acpt_fltr;
} machine_can_port_t;

// Just one CAN device.
static machine_can_port_t canfd_port = {
    .can_hw_id = 1,
    .canfd_base = (CANFD_Type *)CANFD_BASE,
    .canfd_cnt_base = (CANFD_CNT_Type *)CANFD_CNT_BASE,
    .can_state = MP_CAN_STATE_STOPPED,
};

void CANFD_IRQHandler(void) {
    // Just one CAN device at index 0
    machine_can_obj_t *self = MP_STATE_PORT(machine_can_objs[0]);
    if (self != NULL) {
        machine_can_port_t *port = self->port;
        mp_int_t irq_flags = 0;

        // Get all Interrupt flags at once.
        uint32_t irq_event = (port->canfd_base->CANFD_RTIF & CANFD_RTIF_REG_Msk)
            | ((port->canfd_base->CANFD_ERRINT & CANFD_ERRINT_REG_Msk) << 8U);

        // State & Error interrupts
        if (irq_event & CANFD_RBUF_OVERRUN_EVENT) {
            port->num_rx_overrun++;
        }
        machine_can_state_t temp_state = port->can_state;
        if (canfd_error_passive_mode(port->canfd_base)) {
            temp_state = MP_CAN_STATE_PASSIVE;
            // Count only at the transition into PASSIVE from below.
            if (port->can_state < MP_CAN_STATE_PASSIVE) {
                port->num_error_passive++;
            }
        } else {
            if (canfd_err_warn_limit_reached(port->canfd_base)) {
                temp_state = MP_CAN_STATE_WARNING;
                // Count only at the transition ACTIVE -> WARNING
                if (port->can_state == MP_CAN_STATE_ACTIVE) {
                    port->num_error_warning++;
                }
            } else {
                temp_state = MP_CAN_STATE_ACTIVE;
            }
        }
        if (canfd_get_bus_status(port->canfd_base) == CANFD_BUS_STATUS_OFF) {
            temp_state = MP_CAN_STATE_BUS_OFF;
            if (port->can_state != MP_CAN_STATE_BUS_OFF) {
                port->num_bus_off++;
            }
        }
        if (temp_state > port->can_state) {
            irq_flags |= MP_CAN_IRQ_STATE;
        }
        port->can_state = temp_state;

        // Nothing else to do, return.
        if (irq_event == 0) {
            return;
        }

        // Data RX and TX interrupts.
        if (irq_event & CANFD_RBUF_AVAILABLE_EVENT) {
            irq_flags |= MP_CAN_IRQ_RX;
        }
        if (irq_event & (CANFD_SECONDARY_BUF_TX_COMPLETE_EVENT | CANFD_PRIMARY_BUF_TX_COMPLETE_EVENT)) {
            irq_flags |= MP_CAN_IRQ_TX;
        }
        if (irq_event & CANFD_TX_ABORT_EVENT) {
            irq_flags |= (MP_CAN_IRQ_TX_FAILED | MP_CAN_IRQ_TX);
        }

        port->irq_flags = irq_flags;

        // Call the MP callback if the events match the trigger.
        if (irq_flags & self->mp_irq_trigger) {
            mp_irq_handler(self->mp_irq_obj);
        }
        // Clear data and error interrupt flags
        port->canfd_base->CANFD_RTIF |= irq_event & 0xff;
        port->canfd_base->CANFD_ERRINT |= irq_event >> 8;
    }
}

static int machine_can_port_f_clock(const machine_can_obj_t *self) {
    return RTE_CANFD_CLK_SPEED;
}

static bool machine_can_port_supports_mode(const machine_can_obj_t *self, machine_can_mode_t mode) {
    return mode < MP_CAN_MODE_MAX;
}

static void machine_can_port_clear_filters(machine_can_obj_t *self) {
    canfd_reset_acpt_fltrs(self->port->canfd_base);
    self->port->num_canfd_acpt_fltr = 0;
}

static mp_uint_t machine_can_port_max_data_len(mp_uint_t flags) {
    #if MICROPY_HW_ENABLE_FDCAN
    if (flags & CAN_MSG_FLAG_FD_F) {
        return 64;
    }
    #endif
    return 8;
}

// The extmod layer calls this function in a loop with incrementing filter_idx
// values. It's up to the port how to apply the filters from here, and to raise
// an exception if there are too many.
// Filters can only be configured in Reset state, so the filter setting are collected
// here and later applied by forcing a re-init.
// machine_can_port_set_filter() will be called with increasing values for filter_idx,
// allowing using it as the total number of filters.
static void machine_can_port_set_filter(machine_can_obj_t *self, int filter_idx, mp_uint_t can_id, mp_uint_t mask, mp_uint_t flags) {

    if (filter_idx < CANFD_MAX_ACCEPTANCE_FILTERS) {
        canfd_acpt_fltr_t *filter_config = &self->port->filter_config[filter_idx];

        if (flags & CAN_MSG_FLAG_EXT_ID) {
            filter_config->frame_type = CANFD_ACPT_FILTER_CFG_EXT_FRAMES;
        } else {
            filter_config->frame_type = CANFD_ACPT_FILTER_CFG_STD_FRAMES;
        }

        filter_config->ac_code = can_id;
        filter_config->ac_mask = mask;  // Inverting is done in canfd_enable_acpt_fltr().
        filter_config->op_code = CANFD_ACPT_FLTR_OP_ADD_MASKABLE_ID;
        filter_config->filter = filter_idx;
        self->port->num_canfd_acpt_fltr = filter_idx + 1;
    }
}

// Report that the set of filters is complete for now.
// Since filters can only be enabled in reset state, a call init()
// to do so..
static void machine_can_port_set_filter_done(machine_can_obj_t *self) {
    if (self->port->num_canfd_acpt_fltr > 0) {
        machine_can_port_init(self);
    }
}

// Update interrupt configuration based on the new contents of 'self'
static void machine_can_update_irqs(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;
    uint16_t triggers = self->mp_irq_trigger;

    if (triggers & MP_CAN_IRQ_RX) {
        port->canfd_base->CANFD_RTIE |= CANFD_RTIE_RIE;
    } else {
        port->canfd_base->CANFD_RTIE &= ~CANFD_RTIE_RIE;
    }
    if (triggers & MP_CAN_IRQ_TX) {
        canfd_enable_tx_interrupts(port->canfd_base);
    }
}

// Return the irq().flags() result.
static mp_uint_t machine_can_port_irq_flags(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    mp_int_t irq_flags = port->irq_flags;

    // Check if meanwhile messages arrived.
    if (canfd_rx_msg_available(port->canfd_base)) {
        irq_flags |= MP_CAN_IRQ_RX;
    }
    port->irq_flags = 0;

    return irq_flags;
}

// Initialize the hardware
static void machine_can_port_init(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;

    if (port == NULL) {
        port = &canfd_port;
        self->port = port;

        // port is in static RAM and not cleared unless there is a
        // power cycle. Thus clearing the filter is required.
        memset(port, 0, sizeof(machine_can_port_t));
        port->can_hw_id = 1;
        port->canfd_base = (CANFD_Type *)CANFD_BASE;
        port->canfd_cnt_base = (CANFD_CNT_Type *)CANFD_CNT_BASE;
        port->can_state = MP_CAN_STATE_STOPPED;

        // Both 160M and HFOSC clocks must be enabled even if only one
        // of them is used.
        enable_cgu_clk38p4m();
        enable_cgu_clk160m();

        // Enable the CANFD clock, Source 160 MHz, CANFD clock 20 MHz.
        canfd_clock_enable(RTE_CANFD_CLK_SOURCE, CANFD_CLK_DIVISOR);
        // Configure the RX/TX pins.
        mp_hal_pin_config(pin_CAN_RXD, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP,
            MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_2MA, MP_HAL_PIN_ALT(CAN_RXD, 1), true);
        mp_hal_pin_config(pin_CAN_TXD, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
            MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_2MA, MP_HAL_PIN_ALT(CAN_TXD, 1), false);
    }

    // Clear counters
    port->num_error_warning = 0;
    port->num_error_passive = 0;
    port->num_bus_off = 0;
    port->num_rx_overrun = 0;
    // Clear the TEC/REC error counters & leave a bus-off state
    port->canfd_base->CANFD_CFG_STAT |= CANFD_CFG_STAT_BUSOFF;
    port->can_state = MP_CAN_STATE_ACTIVE;

    // Configure the Controller
    canfd_reset(port->canfd_base);

    // Set the bit timing
    port->canfd_base->CANFD_S_SEG_1 = self->tseg1 - 1;
    port->canfd_base->CANFD_S_SEG_2 = self->tseg2 - 1;
    port->canfd_base->CANFD_S_SJW = self->sjw - 1;
    port->canfd_base->CANFD_S_PRESC = self->brp - 1;

    // Clear all filters and set the,m again.
    canfd_reset_acpt_fltrs(self->port->canfd_base);
    for (int i = 0; i < port->num_canfd_acpt_fltr; i++) {
        canfd_enable_acpt_fltr(self->port->canfd_base, port->filter_config[i]);
    }

    // Switch to the requested mode. Normal mode as default.
    if (self->mode == MP_CAN_MODE_LOOPBACK) {
        canfd_enable_external_loop_back_mode(port->canfd_base);
    } else if (self->mode == MP_CAN_MODE_SILENT_LOOPBACK) {
        canfd_enable_internal_loop_back_mode(port->canfd_base);
    } else if (self->mode == MP_CAN_MODE_SILENT) {
        canfd_enable_listen_only_mode(port->canfd_base);
    } else {
        canfd_enable_normal_mode(port->canfd_base);
    }
    // Configure other RX/TX modes
    canfd_set_stb_mode(port->canfd_base, CANFD_SECONDARY_BUF_MODE_PRIORITY);
    canfd_set_rbuf_overflow_mode(port->canfd_base, CANFD_RBUF_OVF_MODE_DISCARD_NEW_MSG);
    canfd_set_rbuf_storage_format(port->canfd_base, CANFD_RBUF_STORE_NORMAL_MSG);
    canfd_set_rbuf_almost_full_warn_limit(port->canfd_base, 14);
    canfd_set_err_warn_limit(port->canfd_base, 96); // This is the default

    // Enable/Disable the CANFD interrupt events.
    // Enable RX interrupt except CANFD_RTIE_RIE
    port->canfd_base->CANFD_RTIE =
        (CANFD_RTIE_ROIE | CANFD_RTIE_RFIE | CANFD_RTIE_RAFIE);
    if (self->mp_irq_trigger & MP_CAN_IRQ_RX) {
        port->canfd_base->CANFD_RTIE |= CANFD_RTIE_RIE;
    }
    canfd_disable_tx_interrupts(port->canfd_base);
    canfd_enable_error_interrupts(port->canfd_base);

    // Enable the MCU interrupts for CANFD.
    NVIC_ClearPendingIRQ(CANFD_IRQ_IRQn);
    NVIC_SetPriority(CANFD_IRQ_IRQn, RTE_CANFD_IRQ_PRIORITY);
    NVIC_EnableIRQ(CANFD_IRQ_IRQn);

    port->is_enabled = true;
}

static void machine_can_port_deinit(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;

    if (port != NULL) {
        // Clears Pending IRQs and disables it.
        NVIC_ClearPendingIRQ(CANFD_IRQ_IRQn);
        NVIC_DisableIRQ(CANFD_IRQ_IRQn);

        // Disable and clear all the interrupts
        canfd_disable_tx_interrupts(port->canfd_base);
        canfd_disable_rx_interrupts(port->canfd_base);
        canfd_disable_error_interrupts(port->canfd_base);
        canfd_clear_interrupts(port->canfd_base);

        // Disable CANFD Clock
        canfd_clock_disable();
        port->is_enabled = false;
        port->can_state = MP_CAN_STATE_STOPPED;
        self->port = NULL;
    }
}

static mp_int_t machine_can_port_send(machine_can_obj_t *self, mp_uint_t id, const byte *data, size_t data_len, mp_uint_t flags) {
    struct machine_can_port *port = self->port;

    /* If the node is in other than below modes, returns an error */
    if ((self->mode != MP_CAN_MODE_NORMAL) &&
        (self->mode != MP_CAN_MODE_LOOPBACK) &&
        (self->mode != MP_CAN_MODE_SILENT_LOOPBACK)) {
        return -1;
    }

    memset(&port->data_transfer.tx_header, 0x0, sizeof(canfd_tx_info_t));

    /* Perform below if secondary Tx buf chosen */
    if (canfd_stb_free(port->canfd_base)) {
        port->data_transfer.tx_header.buf_type = CANFD_BUF_TYPE_SECONDARY;
    } else {
        return -1;
    }

    /* Stores the message id based on message frame ID type */
    port->data_transfer.tx_header.frame_type = !!(flags & CAN_MSG_FLAG_EXT_ID);

    if (port->data_transfer.tx_header.frame_type) {
        port->data_transfer.tx_header.id = (ARM_CAN_EXTENDED_ID(id) & (~ARM_CAN_ID_IDE_Msk));
    } else {
        port->data_transfer.tx_header.id = ARM_CAN_STANDARD_ID(id);
    }

    /* Copies the message header */
    port->data_transfer.tx_header.edl = 0;
    port->data_transfer.tx_header.brs = !!(flags & CAN_MSG_FLAG_BRS);
    port->data_transfer.tx_header.dlc = data_len;
    port->data_transfer.tx_header.rtr = !!(flags & CAN_MSG_FLAG_RTR);

    /* Invokes the low level functions to prepare and send the message */
    canfd_select_tx_buf(port->canfd_base, port->data_transfer.tx_header.buf_type);
    /* Invokes interrupt mode send function */
    canfd_send(port->canfd_base, port->data_transfer.tx_header, data, data_len);

    // Enable TX interrupt.
    canfd_enable_tx_interrupts(port->canfd_base);

    // The alif CAN controller provides no information about the slot where the
    // message is stored. So the return value is always 0.

    return 0;
}

static bool machine_can_port_cancel_send(machine_can_obj_t *self, mp_uint_t idx) {
    if (canfd_stb_empty(self->port->canfd_base)) {
        // Nothing to cancel
        return false;
    } else {
        // Cancel ALL queued messages, since a specific message cannot be selected.
        canfd_abort_tx(self->port->canfd_base, CANFD_BUF_TYPE_SECONDARY);
        // tbd: wait until the flag is set?
        return true;
    }
}

static bool machine_can_port_recv(machine_can_obj_t *self, void *data, size_t *dlen, mp_uint_t *id, mp_uint_t *flags, mp_uint_t *errors) {
    struct machine_can_port *port = self->port;

    mp_int_t err_fifo = 0;

    if ((port->canfd_base->CANFD_RCTRL & CANFD_RCTRL_RSTAT_Msk) == 3) {
        err_fifo |= CAN_RECV_ERR_FULL;
    }
    if (port->canfd_base->CANFD_RCTRL & CANFD_RCTRL_ROV) {
        err_fifo |= CAN_RECV_ERR_OVERRUN;
    }

    *errors = err_fifo;

    if (canfd_rx_msg_available(port->canfd_base)) {
        port->data_transfer.rx_count = 8; // Get the full standard msg.
        port->data_transfer.rx_ptr = data;

        // Invokes interrupt mode send function
        canfd_receive(port->canfd_base, &port->data_transfer);

        *id = port->data_transfer.rx_header.id;
        *flags = (port->data_transfer.rx_header.rtr ? CAN_MSG_FLAG_RTR : 0) |
            (port->data_transfer.rx_header.frame_type ? CAN_MSG_FLAG_EXT_ID : 0);
        *dlen = port->data_transfer.rx_header.dlc;

        return true;
    } else {
        return false;
    }
}

// Update the state and return the new value.
static machine_can_state_t machine_can_port_get_state(machine_can_obj_t *self) {
    return self->port->can_state;
}

// For now, just call deinit() and init() to restart.
static void machine_can_port_restart(machine_can_obj_t *self) {
    // Disable IRQ preventing IRQ from cancelling messages.
    canfd_disable_tx_interrupts(self->port->canfd_base);
    // Cancel ALL queued messages, since a specific message cannot be selected.
    canfd_abort_tx(self->port->canfd_base, CANFD_BUF_TYPE_SECONDARY);
    // init() clears the error counters, leave a bus-off state and
    // set the bus state to ACTIVE again.
    machine_can_port_init(self);
}

// Updates values in self->counters (which counters are updated by this
// function versus from ISRs and the like is port specific.
// For tx_pending value only the fact of pending messages is known.
// For rx_pending value only the fact of pending messages is known.

static void machine_can_port_update_counters(machine_can_obj_t *self) {
    struct machine_can_port *port = self->port;
    machine_can_counters_t *counters = &self->counters;

    counters->tec = canfd_get_tx_error_count(port->canfd_base);
    counters->rec = canfd_get_rx_error_count(port->canfd_base);
    counters->num_warning = port->num_error_warning;
    counters->num_passive = port->num_error_passive;
    counters->num_bus_off = port->num_bus_off;
    counters->tx_pending = !canfd_stb_empty(port->canfd_base);
    counters->rx_pending = canfd_rx_msg_available(port->canfd_base);
    counters->rx_overruns = port->num_rx_overrun;
}

// Hook for port to fill in the final item of the get_timings() result list with controller-specific values
static mp_obj_t machine_can_port_get_additional_timings(machine_can_obj_t *self, mp_obj_t optional_arg) {
    return mp_const_none;
}
