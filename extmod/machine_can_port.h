#ifndef MICROPY_INCLUDED_EXTMOD_MACHINE_CAN_PORT_H
#define MICROPY_INCLUDED_EXTMOD_MACHINE_CAN_PORT_H

#include "py/obj.h"
#include "py/objarray.h"
#include "py/runtime.h"
#include "shared/runtime/mpirq.h"

// This header is included into both extmod/machine_can.c and port-specific
// machine_can.c implementations and provides shared (static) function
// declarations to both.
//
// In a MicroPython build including this header from port-specific machine_can.c
// include is a no-op (as the file is included directly into
// extmod/machine_can.c). However, including it anyway means that Language
// Servers and IDEs can correctly analyse the machine_can.c file while the
// developer is writing it.

typedef enum {
    MP_CAN_STATE_STOPPED,
    MP_CAN_STATE_ACTIVE,
    MP_CAN_STATE_WARNING,
    MP_CAN_STATE_PASSIVE,
    MP_CAN_STATE_BUS_OFF,
} machine_can_state_t;

typedef enum {
    MP_CAN_MODE_NORMAL,
    MP_CAN_MODE_SLEEP,
    MP_CAN_MODE_LOOPBACK,
    MP_CAN_MODE_SILENT,
    MP_CAN_MODE_SILENT_LOOPBACK,
    MP_CAN_MODE_MAX,
} machine_can_mode_t;

// CAN IRQ Flags
// (currently the same for all ports)
#define MP_CAN_IRQ_TX         (1 << 0)
#define MP_CAN_IRQ_RX         (1 << 1)
#define MP_CAN_IRQ_TX_FAILED  (1 << 2)
#define MP_CAN_IRQ_STATE      (1 << 3)

// Transmit buffer incex is encoded into the irq().flags() response for MP_CAN_IRQ_TX
#define MP_CAN_IRQ_IDX_SHIFT 16
#define MP_CAN_IRQ_IDX_MASK 0xFF

#if MICROPY_HW_ENABLE_FDCAN
#define MP_CAN_MAX_LEN 64
#else
#define MP_CAN_MAX_LEN 8
#endif

struct machine_can_port;

// These values appear in the same order as the result of CAN.get_counters()
typedef struct {
    mp_uint_t tec;
    mp_uint_t rec;
    mp_uint_t num_warning; // Number of "Error Warning" transitions
    mp_uint_t num_passive; // Number of "Error Passive" transitions
    mp_uint_t num_bus_off; // Number of "Bus-Off" transitions
    mp_uint_t tx_pending;
    mp_uint_t rx_pending;
    mp_uint_t rx_overruns;
} machine_can_counters_t;

typedef struct {
    mp_obj_base_t base;
    mp_uint_t can_idx;

    // Timing register settings
    byte tseg1;
    byte tseg2;
    byte brp;
    byte sjw;

    machine_can_mode_t mode;
    bool auto_retransmit;

    mp_irq_obj_t *mp_irq_obj;
    uint16_t mp_irq_trigger;
    mp_uint_t rx_error_flags;

    // Assumed some of these counters are updated from different port ISRs, etc. and some
    // are updated by calling machine_can_port_update_counters()
    machine_can_counters_t counters;

    struct machine_can_port *port;
} machine_can_obj_t;

// Indexes for recv result list
enum {
    RECV_ARG_ID,
    RECV_ARG_DATA,
    RECV_ARG_FLAGS,
    RECV_ARG_ERRORS,
    RECV_ARG_LEN, // Overall length, not an index
} recv_arg_idx_t;

#define CAN_STD_ID_MASK 0x7FF
#define CAN_EXT_ID_MASK 0x1fffffff

// CAN Message Flags
#define CAN_MSG_FLAG_RTR          (1 << 0)
#define CAN_MSG_FLAG_EXT_ID       (1 << 1)
#define CAN_MSG_FLAG_FD_F         (1 << 2)
#define CAN_MSG_FLAG_BRS          (1 << 3)
#define CAN_MSG_FLAG_UNORDERED (1 << 4)

// CAN recv() Error Flags
#define CAN_RECV_ERR_FULL         (1 << 0)
#define CAN_RECV_ERR_OVERRUN      (1 << 1)
#define CAN_RECV_ERR_ESI          (1 << 2)

// The port must provide implementations of these low-level CAN functions
static int machine_can_port_f_clock(const machine_can_obj_t *self);

static bool machine_can_port_supports_mode(const machine_can_obj_t *self, machine_can_mode_t mode);

static bool machine_can_port_set_mode(machine_can_obj_t *self, machine_can_mode_t mode);

static void machine_can_port_clear_filters(machine_can_obj_t *self);

static mp_uint_t machine_can_port_max_data_len(mp_uint_t flags);

// The extmod layer calls this function in a loop with incrementing filter_idx
// values. It's up to the port how to apply the filters from here, and to raise
// an exception if there are too many.
//
// If the CAN_FILTERS_STD_EXT_SEPARATE flag is set to 1, filter_idx will
// enumerate standard id filters separately to extended id filters (the
// CAN_MSG_FLAG_EXT_ID bit in 'flags' differentiates the type).
static void machine_can_port_set_filter(machine_can_obj_t *self, int filter_idx, mp_uint_t can_id, mp_uint_t mask, mp_uint_t flags);

// Update interrupt configuration based on the new contents of 'self'
static void machine_can_update_irqs(machine_can_obj_t *self);

// Return the irq().flags() result. Calling this function may also update the hardware state machine.
static mp_uint_t machine_can_port_irq_flags(machine_can_obj_t *self);

static void machine_can_port_init(machine_can_obj_t *self);

static void machine_can_port_deinit(machine_can_obj_t *self);

static mp_int_t machine_can_port_send(machine_can_obj_t *self, mp_uint_t id, const byte *data, size_t data_len, mp_uint_t flags);

static bool machine_can_port_cancel_send(machine_can_obj_t *self, mp_uint_t idx);

static bool machine_can_port_recv(machine_can_obj_t *self, void *data, size_t *dlen, mp_uint_t *id, mp_uint_t *flags, mp_uint_t *errors);

static machine_can_state_t machine_can_port_get_state(machine_can_obj_t *self);

static void machine_can_port_restart(machine_can_obj_t *self);

// Updates values in self->counters (which counters are updated by this function versus from ISRs and the like
// is port specific
static void machine_can_port_update_counters(machine_can_obj_t *self);

// Hook for port to fill in the final item of the get_timings() result list with controller-specific values
static mp_obj_t machine_can_port_get_additional_timings(machine_can_obj_t *self, mp_obj_t optional_arg);

// This function is only optionally defined by the port. If macro CAN_PORT_PRINT_FUNCTION is not set
// then a default machine_can_print function will be used.
static void machine_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);

#endif // MICROPY_INCLUDED_EXTMOD_MACHINE_CAN_PORT_H
