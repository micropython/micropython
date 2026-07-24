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
//   - hold transmits in a software priority queue of persistent slots, handing
//     the single hardware TX buffer the highest-priority frame at a time (the
//     next is submitted from on_tx_done) so send() returns a stable buffer index
//     and priority ordering is honoured despite the hardware having no arbiter;
//   - schedule the MicroPython irq handler from the callbacks.
//
// Hardware notes (classic ESP32/S3 TWAI controller): one mask filter, one TX
// hardware buffer (no priority arbitration), classic CAN only (no FD).
// Filter/timing must be configured while the node is stopped, so changing
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
// The TWAI controller only supports an even prescaler.
#define CAN_BRP_GRANULARITY 2
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

// RX filtering is done in software (in the on_rx_done ISR) rather than with the
// single hardware acceptance filter, so multiple standard and extended filters
// can be supported. This is the number of filters accepted by set_filters().
#define CAN_HW_MAX_FILTER (4)

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
    uint8_t err; // CAN_RECV_ERR_* describing the ring state when this frame arrived
    uint8_t data[8];
} can_rx_slot_t;

// TX slot lifecycle: FREE -> PENDING (queued by send) -> INFLIGHT (handed to the
// driver's single TX buffer) -> FREE (on completion). Only one slot is INFLIGHT
// at a time; the driver has a single hardware TX buffer with no priority
// arbitration, so priority ordering is done here in software.
enum {
    CAN_TX_FREE,
    CAN_TX_PENDING,
    CAN_TX_INFLIGHT,
};

typedef struct {
    twai_frame_t frame;
    uint8_t data[8];
    uint32_t prio; // arbitration priority key, lower = higher priority
    uint8_t state;
} can_tx_slot_t;

// Filter mode: accept everything, nothing, or match against the filter list.
enum {
    CAN_FILTER_ACCEPT_ALL,
    CAN_FILTER_ACCEPT_NONE,
    CAN_FILTER_LIST,
};

// A single software RX filter: a frame matches if it is the same frame format
// (standard/extended), its masked id equals the filter's masked id, and — if the
// filter requests remote frames — it is a remote frame.
typedef struct {
    uint32_t id;
    uint32_t mask;
    bool ext;
    bool rtr;
} can_filter_t;

typedef struct machine_can_port {
    twai_node_handle_t node;
    gpio_num_t tx_io;
    gpio_num_t rx_io;
    bool enabled;

    // Software RX filters. The active copy is read by the on_rx_done ISR; the
    // staged copy is built by set_filter*() and committed in set_filter_done().
    uint8_t stage_mode;
    uint8_t stage_count;
    can_filter_t stage[CAN_HW_MAX_FILTER];
    volatile uint8_t filter_mode;
    volatile uint8_t filter_count;
    can_filter_t filter[CAN_HW_MAX_FILTER];

    // RX software ring (filled in on_rx_done ISR, drained by recv()).
    can_rx_slot_t rx_ring[CAN_RX_RING_LEN];
    volatile uint16_t rx_head;
    volatile uint16_t rx_tail;
    volatile uint16_t rx_overruns; // cumulative dropped-frame count (get_counters)
    volatile bool rx_overrun_pending; // flag the next enqueued frame RECV_ERR_OVERRUN

    // TX priority queue of persistent slots; slot index is returned by send().
    // Guarded by tx_mux because send() (task) and on_tx_done (ISR) both mutate it.
    can_tx_slot_t tx_slots[CAN_TX_QUEUE_LEN];
    volatile int16_t tx_inflight; // slot index handed to the driver, or -1
    portMUX_TYPE tx_mux;

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
static esp_err_t can_tx_kick(machine_can_port_t *port, bool from_isr);
static machine_can_state_t machine_can_port_get_state(machine_can_obj_t *self);

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

// Software RX filter check, run in the on_rx_done ISR.
static bool can_frame_accepted(machine_can_port_t *port, uint32_t id, bool ext, bool rtr) {
    uint8_t mode = port->filter_mode;
    if (mode == CAN_FILTER_ACCEPT_ALL) {
        return true;
    }
    if (mode == CAN_FILTER_ACCEPT_NONE) {
        return false;
    }
    for (uint8_t i = 0; i < port->filter_count; i++) {
        const can_filter_t *f = &port->filter[i];
        if (f->ext == ext && ((id & f->mask) == (f->id & f->mask)) && (!f->rtr || rtr)) {
            return true;
        }
    }
    return false;
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

    // Software filtering: a rejected frame is consumed but not buffered (and
    // doesn't count as an overrun).
    if (!can_frame_accepted(port, rx.header.id, rx.header.ide, rx.header.rtr)) {
        return false;
    }

    uint16_t next = (port->rx_head + 1) % CAN_RX_RING_LEN;
    if (next == port->rx_tail) {
        // Ring full: drop this frame and mark an overrun so the next frame that
        // does get enqueued reports RECV_ERR_OVERRUN.
        port->rx_overruns++;
        port->rx_overrun_pending = true;
        return false;
    }
    can_rx_slot_t *slot = &port->rx_ring[port->rx_head];
    slot->id = rx.header.id;
    slot->flags = (rx.header.ide ? CAN_MSG_FLAG_EXT_ID : 0) | (rx.header.rtr ? CAN_MSG_FLAG_RTR : 0);
    slot->err = 0;
    if (port->rx_overrun_pending) {
        slot->err |= CAN_RECV_ERR_OVERRUN;
        port->rx_overrun_pending = false;
    }
    // Flag RECV_ERR_FULL if enqueuing this frame leaves no room for another.
    if (((next + 1) % CAN_RX_RING_LEN) == port->rx_tail) {
        slot->err |= CAN_RECV_ERR_FULL;
    }
    mp_uint_t len = twaifd_dlc2len(rx.header.dlc);
    if (len > 8) {
        len = 8;
    }
    slot->dlc = len;
    // A remote frame carries no data; machine.CAN reports its payload as zeros.
    bool rtr = rx.header.rtr;
    for (mp_uint_t i = 0; i < 8; i++) {
        slot->data[i] = (!rtr && i < len) ? buf[i] : 0;
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

    // Free the completed in-flight slot and report its index.
    portENTER_CRITICAL_ISR(&port->tx_mux);
    int idx = port->tx_inflight;
    if (idx >= 0) {
        port->tx_slots[idx].state = CAN_TX_FREE;
        port->tx_inflight = -1;
    }
    portEXIT_CRITICAL_ISR(&port->tx_mux);
    if (idx >= 0) {
        // Queue the completion so each one is reported individually by flags().
        uint8_t next = (port->txd_head + 1) % CAN_TX_QUEUE_LEN;
        if (next != port->txd_tail) {
            port->txd_ring[port->txd_head].idx = idx;
            port->txd_ring[port->txd_head].failed = !edata->is_tx_success;
            port->txd_head = next;
        }
    }
    // Hand the next-highest-priority pending frame to the driver (ISR context).
    can_tx_kick(port, true);
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
        // memset can't produce a valid unlocked spinlock (its "free" value is a
        // sentinel, not zero), so initialise tx_mux explicitly.
        port->tx_mux = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
        // Select this controller's pins by index (see machine_can_pins).
        port->tx_io = machine_can_pins[self->can_idx].tx;
        port->rx_io = machine_can_pins[self->can_idx].rx;
        // Accept all until set_filters() is called.
        port->stage_mode = CAN_FILTER_ACCEPT_ALL;
        port->filter_mode = CAN_FILTER_ACCEPT_ALL;
    } else {
        // Reconfiguring: tear down the existing node first.
        can_hw_delete(self);
    }
    // Reset queues/counters for the fresh node.
    port->rx_head = port->rx_tail = port->rx_overruns = 0;
    port->rx_overrun_pending = false;
    port->tx_inflight = -1;
    for (int i = 0; i < CAN_TX_QUEUE_LEN; i++) {
        port->tx_slots[i].state = CAN_TX_FREE;
    }
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
// The hardware has a single mask filter. set_filter_done() uses it directly for
// the common cases (accept-all, accept-none, one non-RTR filter) and otherwise
// leaves it accept-all and matches in the RX ISR (see can_frame_accepted).
// -------------------------------------------------------------------------

static void can_hw_apply_filter(machine_can_obj_t *self) {
    // Initial baseline: accept-all. set_filter_done() reconfigures from here as
    // set_filters() is called.
    twai_mask_filter_config_t cfg = { .id = 0, .mask = 0 };
    can_check_esp_err(twai_node_config_mask_filter(self->port->node, 0, &cfg));
}

static void machine_can_port_clear_filters(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;
    // Baseline is "accept none"; set_filters(None) raises this to accept-all via
    // zero-mask set_filter() calls, filter tuples build the list, and an empty
    // iterable leaves it at accept-none.
    port->stage_mode = CAN_FILTER_ACCEPT_NONE;
    port->stage_count = 0;
}

static void machine_can_port_set_filter(machine_can_obj_t *self, int filter_idx, mp_uint_t can_id, mp_uint_t mask, mp_uint_t flags) {
    machine_can_port_t *port = self->port;

    // A zero bit-mask means "match every ID" (accept all). extmod uses this
    // twice (std + ext) for set_filters(None).
    if (mask == 0) {
        port->stage_mode = CAN_FILTER_ACCEPT_ALL;
        return;
    }
    if (filter_idx >= CAN_HW_MAX_FILTER) {
        mp_raise_ValueError(MP_ERROR_TEXT("too many filters"));
    }
    port->stage[filter_idx] = (can_filter_t) {
        .id = can_id,
        .mask = mask,
        .ext = (flags & CAN_MSG_FLAG_EXT_ID) != 0,
        .rtr = (flags & CAN_MSG_FLAG_RTR) != 0,
    };
    port->stage_count = filter_idx + 1;
    port->stage_mode = CAN_FILTER_LIST;
}

static void machine_can_port_set_filter_done(machine_can_obj_t *self) {
    machine_can_port_t *port = self->port;

    // The single hardware mask filter matches id/mask and std/ext (but not RTR),
    // so it can do the whole job for accept-all, accept-none, and exactly one
    // non-RTR filter -- the common cases, with zero per-frame overhead. Anything
    // else (multiple filters, or an RTR filter) leaves the hardware accept-all
    // and matches in the RX ISR (see can_frame_accepted).
    twai_mask_filter_config_t cfg;
    uint8_t sw_mode;
    if (port->stage_mode == CAN_FILTER_ACCEPT_ALL) {
        cfg = (twai_mask_filter_config_t) { .id = 0, .mask = 0 };
        sw_mode = CAN_FILTER_ACCEPT_ALL;
    } else if (port->stage_mode == CAN_FILTER_ACCEPT_NONE) {
        cfg = (twai_mask_filter_config_t) { .id = UINT32_MAX, .mask = UINT32_MAX };
        sw_mode = CAN_FILTER_ACCEPT_NONE;
    } else if (port->stage_count == 1 && !port->stage[0].rtr) {
        cfg = (twai_mask_filter_config_t) {
            .id = port->stage[0].id,
            .mask = port->stage[0].mask,
            .is_ext = port->stage[0].ext,
        };
        sw_mode = CAN_FILTER_ACCEPT_ALL;
    } else {
        cfg = (twai_mask_filter_config_t) { .id = 0, .mask = 0 };
        sw_mode = CAN_FILTER_LIST;
    }

    // Commit the software filter list first, writing ACCEPT_NONE up front so the
    // ISR never traverses a half-updated table.
    port->filter_mode = CAN_FILTER_ACCEPT_NONE;
    for (uint8_t i = 0; i < port->stage_count; i++) {
        port->filter[i] = port->stage[i];
    }
    port->filter_count = port->stage_count;
    port->filter_mode = sw_mode;

    // The mask filter can only be reconfigured with the node stopped; bounce it
    // if running.
    if (port->node != NULL) {
        bool was_enabled = port->enabled;
        if (was_enabled) {
            can_check_esp_err(twai_node_disable(port->node));
            port->enabled = false;
        }
        can_check_esp_err(twai_node_config_mask_filter(port->node, 0, &cfg));
        if (was_enabled) {
            can_check_esp_err(twai_node_enable(port->node));
            port->enabled = true;
        }
    }
}

// -------------------------------------------------------------------------
// Transmit / receive
// -------------------------------------------------------------------------

// Arbitration priority key: lower value wins the bus first. Matches CAN
// arbitration closely enough for a total order -- compare the 11-bit base, then
// standard-before-extended, then the 18-bit extension, then data-before-remote.
static uint32_t can_tx_prio(uint32_t id, bool ext, bool rtr) {
    uint32_t base = ext ? (id >> 18) : id;
    uint32_t key = (base & 0x7FF) << 21;
    key |= (ext ? 1u : 0u) << 20;
    if (ext) {
        key |= (id & 0x3FFFF) << 2;
    }
    key |= (rtr ? 1u : 0u);
    return key;
}

// If the single TX buffer is free, hand it the highest-priority pending frame.
// Called from send() (task) and on_tx_done (ISR). The slot is claimed under the
// lock but twai_node_transmit() is called outside it (it is ISR-safe, but must
// not be nested inside the critical section).
static esp_err_t can_tx_kick(machine_can_port_t *port, bool from_isr) {
    int idx = -1;
    if (from_isr) {
        portENTER_CRITICAL_ISR(&port->tx_mux);
    } else {
        portENTER_CRITICAL(&port->tx_mux);
    }
    if (port->tx_inflight < 0) {
        uint32_t best = UINT32_MAX;
        for (int i = 0; i < CAN_TX_QUEUE_LEN; i++) {
            if (port->tx_slots[i].state == CAN_TX_PENDING && port->tx_slots[i].prio < best) {
                best = port->tx_slots[i].prio;
                idx = i;
            }
        }
        if (idx >= 0) {
            port->tx_slots[idx].state = CAN_TX_INFLIGHT;
            port->tx_inflight = idx;
        }
    }
    if (from_isr) {
        portEXIT_CRITICAL_ISR(&port->tx_mux);
    } else {
        portEXIT_CRITICAL(&port->tx_mux);
    }
    if (idx < 0) {
        return ESP_OK; // busy or nothing pending
    }
    esp_err_t err = twai_node_transmit(port->node, &port->tx_slots[idx].frame, 0);
    if (err != ESP_OK) {
        // Undo the claim so a later kick can retry this slot.
        if (from_isr) {
            portENTER_CRITICAL_ISR(&port->tx_mux);
        } else {
            portENTER_CRITICAL(&port->tx_mux);
        }
        port->tx_slots[idx].state = CAN_TX_PENDING;
        port->tx_inflight = -1;
        if (from_isr) {
            portEXIT_CRITICAL_ISR(&port->tx_mux);
        } else {
            portEXIT_CRITICAL(&port->tx_mux);
        }
    }
    return err;
}

static mp_int_t machine_can_port_send(machine_can_obj_t *self, mp_uint_t id, const byte *data, size_t data_len, mp_uint_t flags) {
    machine_can_port_t *port = self->port;

    // Can't transmit at all if the bus is off or the node is stopped.
    machine_can_state_t state = machine_can_port_get_state(self);
    if (state == MP_CAN_STATE_BUS_OFF || state == MP_CAN_STATE_STOPPED) {
        mp_raise_OSError(MP_EIO);
    }

    bool ext = (flags & CAN_MSG_FLAG_EXT_ID) != 0;
    bool rtr = (flags & CAN_MSG_FLAG_RTR) != 0;
    uint32_t prio = can_tx_prio(id, ext, rtr);

    // Find a free slot; reject (return None) if the queue is full, or if a
    // pending frame has the same priority and the caller didn't allow reordering
    // (the queue can't guarantee same-priority frames keep their order).
    portENTER_CRITICAL(&port->tx_mux);
    int idx = -1;
    bool collision = false;
    for (int i = 0; i < CAN_TX_QUEUE_LEN; i++) {
        uint8_t st = port->tx_slots[i].state;
        if (st == CAN_TX_FREE) {
            if (idx < 0) {
                idx = i;
            }
        } else if (st == CAN_TX_PENDING && port->tx_slots[i].prio == prio) {
            collision = true;
        }
    }
    if (idx < 0 || (collision && !(flags & CAN_MSG_FLAG_UNORDERED))) {
        portEXIT_CRITICAL(&port->tx_mux);
        return -1;
    }
    can_tx_slot_t *slot = &port->tx_slots[idx];
    for (size_t i = 0; i < data_len && i < 8; i++) {
        slot->data[i] = data[i];
    }
    slot->frame = (twai_frame_t) {
        .header = {
            .id = id,
            .dlc = data_len,
            .ide = ext,
            .rtr = rtr,
        },
        .buffer = slot->data,
        .buffer_len = data_len,
    };
    slot->prio = prio;
    slot->state = CAN_TX_PENDING;
    portEXIT_CRITICAL(&port->tx_mux);

    esp_err_t err = can_tx_kick(port, false);
    if (err != ESP_OK && err != ESP_ERR_TIMEOUT) {
        // Bus-off / disabled / listen-only: the frame cannot be sent at all.
        portENTER_CRITICAL(&port->tx_mux);
        slot->state = CAN_TX_FREE;
        portEXIT_CRITICAL(&port->tx_mux);
        mp_raise_OSError(MP_EIO);
    }
    return idx;
}

static bool machine_can_port_cancel_send(machine_can_obj_t *self, mp_uint_t idx) {
    machine_can_port_t *port = self->port;
    if (idx >= CAN_TX_QUEUE_LEN) {
        return false;
    }
    // Only a still-pending frame can be cancelled; an in-flight one is already
    // committed to the hardware TX buffer and can't be recalled.
    portENTER_CRITICAL(&port->tx_mux);
    bool cancelled = port->tx_slots[idx].state == CAN_TX_PENDING;
    if (cancelled) {
        port->tx_slots[idx].state = CAN_TX_FREE;
    }
    portEXIT_CRITICAL(&port->tx_mux);
    if (cancelled) {
        // Report the cancellation as a failed transmit for this buffer index.
        uint8_t next = (port->txd_head + 1) % CAN_TX_QUEUE_LEN;
        if (next != port->txd_tail) {
            port->txd_ring[port->txd_head].idx = idx;
            port->txd_ring[port->txd_head].failed = true;
            port->txd_head = next;
        }
        can_sched_irq(self, MP_CAN_IRQ_TX);
    }
    return cancelled;
}

static bool machine_can_port_recv(machine_can_obj_t *self, void *data, size_t *dlen, mp_uint_t *id, mp_uint_t *flags, mp_uint_t *errors) {
    machine_can_port_t *port = self->port;

    *errors = 0;
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
    *errors = slot->err;
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
    portENTER_CRITICAL(&port->tx_mux);
    port->tx_inflight = -1;
    for (int i = 0; i < CAN_TX_QUEUE_LEN; i++) {
        port->tx_slots[i].state = CAN_TX_FREE;
    }
    portEXIT_CRITICAL(&port->tx_mux);
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
    uint16_t tx_pending = 0;
    for (int i = 0; i < CAN_TX_QUEUE_LEN; i++) {
        if (port->tx_slots[i].state != CAN_TX_FREE) {
            tx_pending++;
        }
    }
    counters->tx_pending = tx_pending;
    counters->rx_pending = (uint16_t)((port->rx_head - port->rx_tail + CAN_RX_RING_LEN) % CAN_RX_RING_LEN);
    counters->rx_overruns = port->rx_overruns;
}

static mp_obj_t machine_can_port_get_additional_timings(machine_can_obj_t *self, mp_obj_t optional_arg) {
    (void)self;
    (void)optional_arg;
    return mp_const_none;
}
