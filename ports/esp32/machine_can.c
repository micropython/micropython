/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Matt Trentini
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

// ESP32 implementation of the shared machine.CAN port interface
// (extmod/machine_can_port.h), backed by the ESP-IDF node-based TWAI driver
// (driver/twai_onchip.h, available from ESP-IDF v5.4). This file is included
// textually into extmod/machine_can.c via MICROPY_PY_MACHINE_CAN_INCLUDEFILE.
//
// The node driver delivers all events (rx, tx-done, state change, error) via
// ISR callbacks. We:
//   - drain received frames in on_rx_done into a software ring that recv() pops;
//   - track transmits with a FIFO head/tail ring of persistent slots so send()
//     can return an accurate buffer index and on_tx_done can report it;
//   - schedule the MicroPython irq handler from the callbacks.
//
// Hardware notes (classic ESP32/S3 TWAI controller): one mask filter, one TX
// hardware buffer (driver provides a FIFO software queue), classic CAN only (no
// FD). Filter/timing must be configured while the node is stopped, so changing
// filters at runtime disables and re-enables the node.

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/runtime/mpirq.h"
#include "modmachine.h"

#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "esp_clk_tree.h"
#include "soc/soc_caps.h"

// Classic-CAN timing limits. brp comes from soc_caps; tseg/sjw are the classic
// SJA1000 controller limits. These bound the timing maths in extmod.
#define CAN_BRP_MIN SOC_TWAI_BRP_MIN
#define CAN_BRP_MAX SOC_TWAI_BRP_MAX
#define CAN_TSEG1_MIN 1
#define CAN_TSEG1_MAX 16
#define CAN_TSEG2_MIN 1
#define CAN_TSEG2_MAX 8
#define CAN_SJW_MIN 1
#define CAN_SJW_MAX 4

// Let extmod compute brp/tseg1/tseg2 from bitrate + sample_point; we apply them
// via the advanced timing config so get_timings() stays accurate.
#define CAN_USE_UPSTREAM_TIMING (1)
#define CAN_PORT_PRINT_FUNCTION (0)

// The TWAI source clock (TWAI_CLK_SRC_DEFAULT) frequency differs across chips
// (80 MHz APB on ESP32/S3, 40 MHz XTAL on ESP32-P4, ...), so it is queried at
// runtime in machine_can_port_f_clock() rather than hard-coded.

// Depth of the driver's software transmit queue, also the number of send()
// buffer indexes reported to Python.
#define CAN_TX_QUEUE_LEN (8)

// Only a single acceptance filter is implemented, which matches every current
// single-filter controller. Some controllers expose more (e.g. ESP32-C5 has 3
// mask filters); clamp to 1 rather than over-report SOC_TWAI_MASK_FILTER_NUM
// until multi-filter support is implemented and tested. See machine_can.md.
#define CAN_HW_MAX_FILTER (1)

// Software receive ring depth.
#define CAN_RX_RING_LEN (16)

// TX/RX pins per controller, indexed by can_idx (0-based). A board defines
// MICROPY_HW_CANn_TX / MICROPY_HW_CANn_RX (n is the 1-based controller id) and
// sets MICROPY_HW_NUM_CAN to the number of controllers it exposes. The node
// driver auto-allocates a free hardware controller from these pins, so no
// chip-specific selection is needed.
typedef struct {
    gpio_num_t tx;
    gpio_num_t rx;
} machine_can_pins_t;

static const machine_can_pins_t machine_can_pins[] = {
    #if defined(MICROPY_HW_CAN1_TX)
    { MICROPY_HW_CAN1_TX, MICROPY_HW_CAN1_RX },
    #endif
    #if defined(MICROPY_HW_CAN2_TX)
    { MICROPY_HW_CAN2_TX, MICROPY_HW_CAN2_RX },
    #endif
    #if defined(MICROPY_HW_CAN3_TX)
    { MICROPY_HW_CAN3_TX, MICROPY_HW_CAN3_RX },
    #endif
};

// The board must provide exactly one pin pair per controller, and the chip must
// have at least that many TWAI controllers.
MP_STATIC_ASSERT(MP_ARRAY_SIZE(machine_can_pins) == MICROPY_HW_NUM_CAN);
#if MICROPY_HW_NUM_CAN > SOC_TWAI_CONTROLLER_NUM
#error "MICROPY_HW_NUM_CAN exceeds the number of TWAI controllers on this chip"
#endif

typedef struct {
    uint32_t id;
    uint16_t flags;
    uint8_t dlc;
    uint8_t data[8];
} can_rx_slot_t;

typedef struct {
    twai_frame_t frame;
    uint8_t data[8];
} can_tx_slot_t;

// Staged filter intent (applied at init / set_filter_done).
enum {
    CAN_FILTER_ACCEPT_ALL,
    CAN_FILTER_ACCEPT_NONE,
    CAN_FILTER_REAL,
};

typedef struct machine_can_port {
    twai_node_handle_t node;
    gpio_num_t tx_io;
    gpio_num_t rx_io;
    bool enabled;

    // Staged filter (set between clear_filters() and set_filter_done()).
    uint8_t filter_mode;
    twai_mask_filter_config_t filter;

    // RX software ring (filled in on_rx_done ISR, drained by recv()).
    can_rx_slot_t rx_ring[CAN_RX_RING_LEN];
    volatile uint16_t rx_head;
    volatile uint16_t rx_tail;
    volatile uint16_t rx_overruns;

    // TX FIFO ring of persistent slots; index returned by send().
    can_tx_slot_t tx_slots[CAN_TX_QUEUE_LEN];
    volatile uint16_t tx_head; // next index to assign
    volatile uint16_t tx_tail; // next index to complete

    // Bus-state-transition counters (maintained in on_state_change).
    volatile uint16_t num_warning;
    volatile uint16_t num_passive;
    volatile uint16_t num_bus_off;

    // IRQ events, consumed one-at-a-time by machine_can_port_irq_flags() (the
    // handler calls flags() repeatedly until it returns 0). TX completions carry
    // an index so they need a queue; RX and STATE are single pending flags.
    volatile uint8_t txd_head;
    volatile uint8_t txd_tail;
    struct {
        uint8_t idx;
        bool failed;
    } txd_ring[CAN_TX_QUEUE_LEN];
    volatile bool rx_pending;
    volatile bool state_pending;
} machine_can_port_t;

static void can_hw_create(machine_can_obj_t *self);
static void can_hw_apply_filter(machine_can_obj_t *self);

static void can_check_esp_err(esp_err_t err) {
    if (err != ESP_OK) {
        mp_raise_OSError(MP_EIO);
    }
}

// -------------------------------------------------------------------------
// Mode / clock / capability helpers
// -------------------------------------------------------------------------

static int machine_can_port_f_clock(const machine_can_obj_t *self) {
    (void)self;
    uint32_t freq_hz = 0;
    esp_clk_tree_src_get_freq_hz((soc_module_clk_t)TWAI_CLK_SRC_DEFAULT,
        ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &freq_hz);
    return freq_hz;
}

static bool machine_can_port_supports_mode(const machine_can_obj_t *self, machine_can_mode_t mode) {
    (void)self;
    return mode < MP_CAN_MODE_MAX && mode != MP_CAN_MODE_SLEEP;
}

static mp_uint_t machine_can_port_max_data_len(mp_uint_t flags) {
    (void)flags;
    return 8;
}

// -------------------------------------------------------------------------
// ISR event callbacks (run in interrupt context)
// -------------------------------------------------------------------------

// Wake the MicroPython irq handler if the event type is enabled. The handler
// reads the actual event(s) back via machine_can_port_irq_flags().
static void can_sched_irq(machine_can_obj_t *self, mp_uint_t trigger_bit) {
    if ((trigger_bit & self->mp_irq_trigger) && self->mp_irq_obj != NULL &&
        self->mp_irq_obj->handler != mp_const_none) {
        mp_irq_handler(self->mp_irq_obj);
    }
}

static bool can_on_rx_done(twai_node_handle_t node, const twai_rx_done_event_data_t *edata, void *user) {
    (void)node;
    (void)edata;
    machine_can_obj_t *self = user;
    machine_can_port_t *port = self->port;

    uint8_t buf[8];
    twai_frame_t rx = { .buffer = buf, .buffer_len = sizeof(buf) };
    if (twai_node_receive_from_isr(port->node, &rx) != ESP_OK) {
        return false;
    }

    uint16_t next = (port->rx_head + 1) % CAN_RX_RING_LEN;
    if (next == port->rx_tail) {
        port->rx_overruns++; // ring full, drop oldest-blocking: drop this frame
        return false;
    }
    can_rx_slot_t *slot = &port->rx_ring[port->rx_head];
    slot->id = rx.header.id;
    slot->flags = (rx.header.ide ? CAN_MSG_FLAG_EXT_ID : 0) | (rx.header.rtr ? CAN_MSG_FLAG_RTR : 0);
    mp_uint_t len = twaifd_dlc2len(rx.header.dlc);
    if (len > 8) {
        len = 8;
    }
    slot->dlc = len;
    for (mp_uint_t i = 0; i < 8; i++) {
        slot->data[i] = (i < len) ? buf[i] : 0;
    }
    port->rx_head = next;

    port->rx_pending = true;
    can_sched_irq(self, MP_CAN_IRQ_RX);
    return false;
}

static bool can_on_tx_done(twai_node_handle_t node, const twai_tx_done_event_data_t *edata, void *user) {
    (void)node;
    machine_can_obj_t *self = user;
    machine_can_port_t *port = self->port;

    // Transmits complete FIFO; the oldest in-flight slot is the one that's done.
    mp_uint_t idx = port->tx_tail % CAN_TX_QUEUE_LEN;
    if (port->tx_tail != port->tx_head) {
        port->tx_tail++;
    }
    // Queue the completion so each one is reported individually by flags().
    uint8_t next = (port->txd_head + 1) % CAN_TX_QUEUE_LEN;
    if (next != port->txd_tail) {
        port->txd_ring[port->txd_head].idx = idx;
        port->txd_ring[port->txd_head].failed = !edata->is_tx_success;
        port->txd_head = next;
    }
    can_sched_irq(self, MP_CAN_IRQ_TX);
    return false;
}

static bool can_on_state_change(twai_node_handle_t node, const twai_state_change_event_data_t *edata, void *user) {
    (void)node;
    machine_can_obj_t *self = user;
    machine_can_port_t *port = self->port;

    switch (edata->new_sta) {
        case TWAI_ERROR_WARNING:
            port->num_warning++;
            break;
        case TWAI_ERROR_PASSIVE:
            port->num_passive++;
            break;
        case TWAI_ERROR_BUS_OFF:
            port->num_bus_off++;
            break;
        default:
            break;
    }
    // Only report transitions to a more severe state, matching the docs.
    if (edata->new_sta > edata->old_sta) {
        port->state_pending = true;
        can_sched_irq(self, MP_CAN_IRQ_STATE);
    }
    return false;
}

// -------------------------------------------------------------------------
// irq() integration
// -------------------------------------------------------------------------

static void machine_can_update_irqs(machine_can_obj_t *self) {
    // Callbacks are always registered (on_rx_done must run to fill the ring);
    // the trigger mask in self->mp_irq_trigger gates whether the handler runs.
    (void)self;
}

// Return one pending event per call (TX completions first, each with its index),
// returning 0 when drained. The handler is expected to loop until 0.
static mp_uint_t machine_can_port_irq_flags(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    if (port->txd_tail != port->txd_head) {
        mp_uint_t idx = port->txd_ring[port->txd_tail].idx;
        bool failed = port->txd_ring[port->txd_tail].failed;
        port->txd_tail = (port->txd_tail + 1) % CAN_TX_QUEUE_LEN;
        return MP_CAN_IRQ_TX | (idx << MP_CAN_IRQ_IDX_SHIFT) | (failed ? MP_CAN_IRQ_TX_FAILED : 0);
    }
    if (port->rx_pending) {
        port->rx_pending = false;
        return MP_CAN_IRQ_RX;
    }
    if (port->state_pending) {
        port->state_pending = false;
        return MP_CAN_IRQ_STATE;
    }
    return 0;
}

// -------------------------------------------------------------------------
// Node create / enable / disable
// -------------------------------------------------------------------------

static void can_mode_flags(machine_can_mode_t mode, twai_onchip_node_config_t *cfg) {
    switch (mode) {
        case MP_CAN_MODE_SILENT:
            cfg->flags.enable_listen_only = 1;
            break;
        case MP_CAN_MODE_LOOPBACK:
        case MP_CAN_MODE_SILENT_LOOPBACK:
            // The controller receives its own frames and doesn't require an ACK.
            cfg->flags.enable_loopback = 1;
            cfg->flags.enable_self_test = 1;
            break;
        default: // MP_CAN_MODE_NORMAL
            break;
    }
}

// Create the node and configure it (callbacks, timing, filter) while stopped,
// then enable it.
static void can_hw_create(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;

    twai_onchip_node_config_t cfg = {
        .io_cfg = {
            .tx = port->tx_io,
            .rx = port->rx_io,
            .quanta_clk_out = -1,
            .bus_off_indicator = -1,
        },
        .bit_timing = { .bitrate = self->bitrate },
        .fail_retry_cnt = -1, // standard CAN automatic retransmission
        .tx_queue_depth = CAN_TX_QUEUE_LEN,
    };
    can_mode_flags(self->mode, &cfg);

    esp_err_t err = twai_new_node_onchip(&cfg, &port->node);
    if (err == ESP_ERR_INVALID_ARG) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid CAN config"));
    }
    can_check_esp_err(err);

    const twai_event_callbacks_t cbs = {
        .on_rx_done = can_on_rx_done,
        .on_tx_done = can_on_tx_done,
        .on_state_change = can_on_state_change,
    };
    can_check_esp_err(twai_node_register_event_callbacks(port->node, &cbs, self));

    // Apply the exact timing computed by extmod (must be done while stopped).
    twai_timing_advanced_config_t timing = {
        .brp = self->brp,
        .prop_seg = 0,
        .tseg_1 = self->tseg1,
        .tseg_2 = self->tseg2,
        .sjw = self->sjw,
    };
    err = twai_node_reconfig_timing(port->node, &timing, NULL);
    if (err == ESP_ERR_INVALID_ARG) {
        twai_node_delete(port->node);
        port->node = NULL;
        mp_raise_ValueError(MP_ERROR_TEXT("invalid CAN timing"));
    }
    can_check_esp_err(err);

    can_hw_apply_filter(self);

    can_check_esp_err(twai_node_enable(port->node));
    port->enabled = true;
}

static void can_hw_delete(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    if (port->node != NULL) {
        if (port->enabled) {
            twai_node_disable(port->node);
            port->enabled = false;
        }
        twai_node_delete(port->node);
        port->node = NULL;
    }
}

static void machine_can_port_init(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;

    if (port == NULL) {
        port = m_new_obj(machine_can_port_t);
        memset(port, 0, sizeof(*port));
        self->port = port;
        // Select this controller's pins by index (see machine_can_pins).
        port->tx_io = machine_can_pins[self->can_idx].tx;
        port->rx_io = machine_can_pins[self->can_idx].rx;
        port->filter_mode = CAN_FILTER_ACCEPT_ALL;
    } else {
        // Reconfiguring: tear down the existing node first.
        can_hw_delete(self);
    }
    // Reset queues/counters for the fresh node.
    port->rx_head = port->rx_tail = port->rx_overruns = 0;
    port->tx_head = port->tx_tail = 0;
    port->txd_head = port->txd_tail = 0;
    port->rx_pending = port->state_pending = false;
    port->num_warning = port->num_passive = port->num_bus_off = 0;

    can_hw_create(self);
}

static void machine_can_port_deinit(machine_can_obj_t *self) {
    if (self->port != NULL) {
        can_hw_delete(self);
    }
}

// -------------------------------------------------------------------------
// Filters
//
// One mask filter. The driver defaults to accept-all and performs software
// std/ext discrimination for configured filters. Filter config requires the
// node to be stopped, so a runtime change disables and re-enables it.
// -------------------------------------------------------------------------

static void can_hw_apply_filter(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    twai_mask_filter_config_t cfg;
    switch (port->filter_mode) {
        case CAN_FILTER_ACCEPT_NONE:
            cfg = (twai_mask_filter_config_t) { .id = UINT32_MAX, .mask = UINT32_MAX };
            break;
        case CAN_FILTER_REAL:
            cfg = port->filter;
            break;
        default: // CAN_FILTER_ACCEPT_ALL
            cfg = (twai_mask_filter_config_t) { .id = 0, .mask = 0 };
            break;
    }
    can_check_esp_err(twai_node_config_mask_filter(port->node, 0, &cfg));
}

static void machine_can_port_clear_filters(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    // Baseline is "accept none"; set_filters(None) raises this to accept-all via
    // zero-mask set_filter() calls, a real tuple sets CAN_FILTER_REAL, and an
    // empty iterable leaves it at accept-none.
    port->filter_mode = CAN_FILTER_ACCEPT_NONE;
}

static void machine_can_port_set_filter(machine_can_obj_t *self, int filter_idx, mp_uint_t can_id, mp_uint_t mask, mp_uint_t flags) {
    machine_can_port_t *port = self->port;

    // A zero bit-mask means "match every ID" (accept all). extmod uses this
    // twice (std + ext) for set_filters(None).
    if (mask == 0) {
        port->filter_mode = CAN_FILTER_ACCEPT_ALL;
        return;
    }
    // The mask filter cannot match on the RTR bit.
    if (flags & CAN_MSG_FLAG_RTR) {
        mp_raise_ValueError(MP_ERROR_TEXT("RTR filtering not supported"));
    }
    // Only a single hardware filter is available.
    if (filter_idx >= CAN_HW_MAX_FILTER) {
        mp_raise_ValueError(MP_ERROR_TEXT("too many filters"));
    }
    // The node driver's mask uses 1=match, matching machine.CAN's bit_mask, so
    // no inversion is needed. The driver discriminates std vs ext in software.
    port->filter = (twai_mask_filter_config_t) {
        .id = can_id,
        .mask = mask,
        .is_ext = (flags & CAN_MSG_FLAG_EXT_ID) ? 1 : 0,
    };
    port->filter_mode = CAN_FILTER_REAL;
}

static void machine_can_port_set_filter_done(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    if (port->node == NULL) {
        return;
    }
    // Filter config requires the node stopped; bounce it if running.
    bool was_enabled = port->enabled;
    if (was_enabled) {
        can_check_esp_err(twai_node_disable(port->node));
        port->enabled = false;
    }
    can_hw_apply_filter(self);
    if (was_enabled) {
        can_check_esp_err(twai_node_enable(port->node));
        port->enabled = true;
    }
}

// -------------------------------------------------------------------------
// Transmit / receive
// -------------------------------------------------------------------------

static mp_int_t machine_can_port_send(machine_can_obj_t *self, mp_uint_t id, const byte *data, size_t data_len, mp_uint_t flags) {
    machine_can_port_t *port = self->port;

    // FIFO ring full?
    if ((uint16_t)(port->tx_head - port->tx_tail) >= CAN_TX_QUEUE_LEN) {
        return -1;
    }
    mp_uint_t idx = port->tx_head % CAN_TX_QUEUE_LEN;
    can_tx_slot_t *slot = &port->tx_slots[idx];

    for (size_t i = 0; i < data_len && i < 8; i++) {
        slot->data[i] = data[i];
    }
    slot->frame = (twai_frame_t) {
        .header = {
            .id = id,
            .dlc = data_len,
            .ide = (flags & CAN_MSG_FLAG_EXT_ID) ? 1 : 0,
            .rtr = (flags & CAN_MSG_FLAG_RTR) ? 1 : 0,
        },
        .buffer = slot->data,
        .buffer_len = data_len,
    };

    // Non-blocking; the slot stays valid until on_tx_done (FIFO completion).
    esp_err_t err = twai_node_transmit(port->node, &slot->frame, 0);
    if (err == ESP_ERR_TIMEOUT) {
        // Driver transmit queue is full: report as "not queued" (recv returns None).
        return -1;
    }
    if (err != ESP_OK) {
        // Bus-off, disabled or listen-only: the message cannot be sent at all.
        // machine.CAN requires this to raise rather than silently drop.
        mp_raise_OSError(MP_EIO);
    }
    port->tx_head++;
    return idx;
}

static bool machine_can_port_cancel_send(machine_can_obj_t *self, mp_uint_t idx) {
    (void)self;
    (void)idx;
    // The node driver does not expose cancellation of an individual queued frame.
    return false;
}

static bool machine_can_port_recv(machine_can_obj_t *self, void *data, size_t *dlen, mp_uint_t *id, mp_uint_t *flags, mp_uint_t *errors) {
    machine_can_port_t *port = self->port;

    *errors = 0;
    if (port->rx_overruns) {
        *errors |= CAN_RECV_ERR_OVERRUN;
        port->rx_overruns = 0;
    }

    if (port->rx_tail == port->rx_head) {
        return false;
    }
    can_rx_slot_t *slot = &port->rx_ring[port->rx_tail];
    uint8_t *rx_data = data;
    for (mp_uint_t i = 0; i < 8; i++) {
        rx_data[i] = slot->data[i];
    }
    *dlen = slot->dlc;
    *id = slot->id;
    *flags = slot->flags;
    port->rx_tail = (port->rx_tail + 1) % CAN_RX_RING_LEN;
    return true;
}

// -------------------------------------------------------------------------
// State / counters / restart
// -------------------------------------------------------------------------

static machine_can_state_t machine_can_port_get_state(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    if (!port->enabled) {
        return MP_CAN_STATE_STOPPED;
    }
    twai_node_status_t status;
    if (twai_node_get_info(port->node, &status, NULL) != ESP_OK) {
        return MP_CAN_STATE_STOPPED;
    }
    switch (status.state) {
        case TWAI_ERROR_WARNING:
            return MP_CAN_STATE_WARNING;
        case TWAI_ERROR_PASSIVE:
            return MP_CAN_STATE_PASSIVE;
        case TWAI_ERROR_BUS_OFF:
            return MP_CAN_STATE_BUS_OFF;
        default:
            return MP_CAN_STATE_ACTIVE;
    }
}

static void machine_can_port_restart(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    // Begin bus-off recovery; pending transmits are dropped.
    twai_node_recover(port->node);
    port->tx_head = port->tx_tail = 0;
    port->num_warning = port->num_passive = port->num_bus_off = 0;
}

static void machine_can_port_update_counters(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    machine_can_counters_t *counters = &self->counters;

    twai_node_status_t status;
    twai_node_record_t record;
    if (twai_node_get_info(port->node, &status, &record) != ESP_OK) {
        return;
    }
    counters->tec = status.tx_error_count;
    counters->rec = status.rx_error_count;
    counters->num_warning = port->num_warning;
    counters->num_passive = port->num_passive;
    counters->num_bus_off = port->num_bus_off;
    counters->tx_pending = (uint16_t)(port->tx_head - port->tx_tail);
    counters->rx_pending = (uint16_t)((port->rx_head - port->rx_tail + CAN_RX_RING_LEN) % CAN_RX_RING_LEN);
    counters->rx_overruns = port->rx_overruns;
}

static mp_obj_t machine_can_port_get_additional_timings(machine_can_obj_t *self, mp_obj_t optional_arg) {
    (void)self;
    (void)optional_arg;
    return mp_const_none;
}
