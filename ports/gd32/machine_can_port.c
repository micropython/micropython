// machine_can_port.c - machine.CAN for GD32F4xx (bxCAN-compatible)
// Included by extmod/machine_can.c via MICROPY_PY_MACHINE_CAN_INCLUDEFILE.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/machine_can_port.h"

#include "gd32f4xx.h"

// GD32 CAN0 / CAN1 (bxCAN), 3 TX mailboxes, 2 RX FIFOs with 3 slots each.
#define CAN_NUM_INSTANCES   (2)
#define CAN_NUM_TX_MAILBOX  (3)
#define CAN_NUM_RX_FIFO     (2)

struct machine_can_port {
    uint16_t tx_in_use;      // bitmask of busy mailboxes
    uint16_t tx_id[CAN_NUM_TX_MAILBOX];
    uint8_t  filter_count;
};

static machine_can_obj_t *can_obj_by_idx[CAN_NUM_INSTANCES];

static uint32_t can_periph(uint8_t idx) {
    return (idx == 0) ? CAN0 : CAN1;
}

static uint32_t can_rcu(uint8_t idx) {
    return (idx == 0) ? RCU_CAN0 : RCU_CAN1;
}

// ---- required port hooks ----
static int machine_can_port_f_clock(const machine_can_obj_t *self) {
    (void)self;
    return (int)rcu_clock_freq_get(CK_APB1);
}

static bool machine_can_port_supports_mode(const machine_can_obj_t *self, machine_can_mode_t mode) {
    (void)self;
    switch (mode) {
        case MP_CAN_MODE_NORMAL:
        case MP_CAN_MODE_SLEEP:
        case MP_CAN_MODE_LOOPBACK:
        case MP_CAN_MODE_SILENT:
        case MP_CAN_MODE_SILENT_LOOPBACK:
            return true;
        default:
            return false;
    }
}

static mp_uint_t machine_can_port_max_data_len(mp_uint_t flags) {
    (void)flags;
    return 8;    // classic CAN
}

static void machine_can_port_clear_filters(machine_can_obj_t *self) {
    uint32_t can = can_periph((uint8_t)self->can_idx);
    can_deinit(can);
    self->port->filter_count = 0;
}

static void machine_can_port_set_filter(machine_can_obj_t *self, int filter_idx,
                                        mp_uint_t can_id, mp_uint_t mask, mp_uint_t flags) {
    uint32_t can = can_periph((uint8_t)self->can_idx);
    bool ext = (flags & CAN_MSG_FLAG_EXT_ID) != 0;

    if (self->port->filter_count >= 14) {
        mp_raise_ValueError(MP_ERROR_TEXT("too many filters"));
    }

    can_filter_parameter_struct f;
    can_struct_para_init(CAN_FILTER_STRUCT, &f);
    f.filter_number = (uint32_t)filter_idx;
    f.filter_mode = CAN_FILTERMODE_MASK;
    f.filter_bits = ext ? CAN_FILTERBITS_32BIT : CAN_FILTERBITS_32BIT;
    f.filter_list_high = (uint16_t)((ext ? (can_id >> 13) : (can_id << 5)) & 0xFFFF);
    f.filter_list_low  = (uint16_t)((ext ? ((can_id << 3) | (flags & CAN_MSG_FLAG_EXT_ID ? 0x4 : 0)) : 0) & 0xFFFF);
    f.filter_mask_high = (uint16_t)((ext ? (mask >> 13) : (mask << 5)) & 0xFFFF);
    f.filter_mask_low  = (uint16_t)(ext ? ((mask << 3) & 0xFFFF) : 0);
    f.filter_fifo_number = CAN_FIFO0;
    f.filter_enable = ENABLE;

    can_filter_init(&f);
    self->port->filter_count++;
    (void)can;
}

static void machine_can_port_set_filter_done(machine_can_obj_t *self) {
    (void)self;
}

static void machine_can_port_init(machine_can_obj_t *self) {
    uint8_t idx = (uint8_t)self->can_idx;
    uint32_t can = can_periph(idx);

    rcu_periph_clock_enable(can_rcu(idx));

    // Configure the CAN pins for AF9 as required by the GD32F4xx datasheet:
    //   CAN0: PB8 = RX, PB9  = TX
    //   CAN1: PB12 = RX, PB13 = TX
    rcu_periph_clock_enable(RCU_GPIOB);
    if (idx == 0) {
        gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_8 | GPIO_PIN_9);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                                GPIO_PIN_8 | GPIO_PIN_9);
    } else {
        gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_12 | GPIO_PIN_13);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                                GPIO_PIN_12 | GPIO_PIN_13);
    }

    can_deinit(can);

    can_parameter_struct p;
    can_struct_para_init(CAN_INIT_STRUCT, &p);
    p.working_mode = CAN_NORMAL_MODE;
    p.resync_jump_width = CAN_BT_SJW_1TQ;
    p.time_segment_1 = CAN_BT_BS1_8TQ;
    p.time_segment_2 = CAN_BT_BS2_3TQ;
    // 500 kbit/s on 42 MHz APB1: 42e6 / (1+8+3) / 500e3 = 7 -> prescaler 7
    p.prescaler = 7;
    p.time_triggered = DISABLE;
    p.auto_bus_off_recovery = ENABLE;
    p.auto_wake_up = DISABLE;
    p.auto_retrans = ENABLE;
    p.rec_fifo_overwrite = DISABLE;
    p.trans_fifo_order = DISABLE;
    can_init(can, &p);

    // accept all by default
    can_filter_parameter_struct f;
    can_struct_para_init(CAN_FILTER_STRUCT, &f);
    f.filter_number = 0;
    f.filter_mode = CAN_FILTERMODE_MASK;
    f.filter_bits = CAN_FILTERBITS_32BIT;
    f.filter_list_high = 0;
    f.filter_list_low = 0;
    f.filter_mask_high = 0;
    f.filter_mask_low = 0;
    f.filter_fifo_number = CAN_FIFO0;
    f.filter_enable = ENABLE;
    can_filter_init(&f);

    self->port->filter_count = 1;
    self->port->tx_in_use = 0;

    can_obj_by_idx[idx] = self;

    NVIC_SetPriority(idx == 0 ? CAN0_RX0_IRQn : CAN1_RX0_IRQn, 2);
    NVIC_EnableIRQ(idx == 0 ? CAN0_RX0_IRQn : CAN1_RX0_IRQn);
}

static void machine_can_port_deinit(machine_can_obj_t *self) {
    uint8_t idx = (uint8_t)self->can_idx;
    NVIC_DisableIRQ(idx == 0 ? CAN0_RX0_IRQn : CAN1_RX0_IRQn);
    can_deinit(can_periph(idx));
    rcu_periph_clock_disable(can_rcu(idx));
    can_obj_by_idx[idx] = NULL;
}

static mp_int_t machine_can_port_send(machine_can_obj_t *self, mp_uint_t id,
                                      const byte *data, size_t data_len, mp_uint_t flags) {
    uint32_t can = can_periph((uint8_t)self->can_idx);

    can_transmit_message_struct tx;
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &tx);
    tx.tx_sfid = (flags & CAN_MSG_FLAG_EXT_ID) ? 0 : (uint16_t)(id & CAN_STD_ID_MASK);
    tx.tx_efid = (flags & CAN_MSG_FLAG_EXT_ID) ? (uint32_t)(id & CAN_EXT_ID_MASK) : 0;
    tx.tx_ff = (flags & CAN_MSG_FLAG_EXT_ID) ? CAN_FF_EXTENDED : CAN_FF_STANDARD;
    tx.tx_ft = (flags & CAN_MSG_FLAG_RTR) ? CAN_FT_REMOTE : CAN_FT_DATA;
    tx.tx_dlen = (uint8_t)(data_len > 8 ? 8 : data_len);
    for (size_t i = 0; i < tx.tx_dlen; i++) {
        tx.tx_data[i] = data[i];
    }

    uint8_t mb = can_message_transmit(can, &tx);
    if (mb == CAN_NOMAILBOX) {
        return -1;
    }

    self->port->tx_id[mb] = (uint16_t)(id & 0xFFFF);
    self->port->tx_in_use |= (uint16_t)(1u << mb);
    return (mp_int_t)mb;
}

static bool machine_can_port_cancel_send(machine_can_obj_t *self, mp_uint_t idx) {
    uint32_t can = can_periph((uint8_t)self->can_idx);
    if (idx >= CAN_NUM_TX_MAILBOX) {
        return false;
    }
    if (!(self->port->tx_in_use & (1u << idx))) {
        return false;
    }
    if (can_transmission_stop(can, (uint8_t)idx) == SUCCESS) {
        self->port->tx_in_use &= (uint16_t)~(1u << idx);
        return true;
    }
    return false;
}

static bool machine_can_port_recv(machine_can_obj_t *self, void *data, size_t *dlen,
                                  mp_uint_t *id, mp_uint_t *flags, mp_uint_t *errors) {
    uint32_t can = can_periph((uint8_t)self->can_idx);

    // check both FIFOs
    for (uint8_t fifo = 0; fifo < CAN_NUM_RX_FIFO; fifo++) {
        if (can_receive_message_length_get(can, fifo) == 0) {
            continue;
        }
        can_receive_message_struct rx;
        can_message_receive(can, fifo, &rx);
        can_fifo_release(can, fifo);

        size_t len = rx.rx_dlen > 8 ? 8 : rx.rx_dlen;
        if (len > *dlen) {
            len = *dlen;
        }
        for (size_t i = 0; i < len; i++) {
            ((byte *)data)[i] = rx.rx_data[i];
        }
        *dlen = len;

        mp_uint_t f = 0;
        if (rx.rx_ff == CAN_FF_EXTENDED) {
            f |= CAN_MSG_FLAG_EXT_ID;
            *id = rx.rx_efid & CAN_EXT_ID_MASK;
        } else {
            *id = rx.rx_sfid & CAN_STD_ID_MASK;
        }
        if (rx.rx_ft == CAN_FT_REMOTE) {
            f |= CAN_MSG_FLAG_RTR;
        }
        *flags = f;
        *errors = 0;
        return true;
    }
    return false;
}

static void machine_can_update_irqs(machine_can_obj_t *self) {
    uint32_t can = can_periph((uint8_t)self->can_idx);
    if (self->mp_irq_trigger == 0) {
        can_interrupt_disable(can, CAN_INT_RFNE0);
        can_interrupt_disable(can, CAN_INT_RFNE1);
    } else {
        if (self->mp_irq_trigger & MP_CAN_IRQ_RX) {
            can_interrupt_enable(can, CAN_INT_RFNE0);
            can_interrupt_enable(can, CAN_INT_RFNE1);
        }
    }
}

static machine_can_state_t machine_can_port_get_state(machine_can_obj_t *self) {
    uint32_t can = can_periph((uint8_t)self->can_idx);
    // GD32 bxCAN: use error counters as a proxy
    uint8_t tec = can_transmit_error_number_get(can);
    uint8_t rec = can_receive_error_number_get(can);
    if (tec > 255 || rec > 255) {
        return MP_CAN_STATE_BUS_OFF;
    }
    if (tec > 127 || rec > 127) {
        return MP_CAN_STATE_PASSIVE;
    }
    if (tec > 95 || rec > 95) {
        return MP_CAN_STATE_WARNING;
    }
    return MP_CAN_STATE_ACTIVE;
}

static void machine_can_port_restart(machine_can_obj_t *self) {
    machine_can_port_deinit(self);
    machine_can_port_init(self);
}

static void machine_can_port_update_counters(machine_can_obj_t *self) {
    uint32_t can = can_periph((uint8_t)self->can_idx);
    self->counters.tec = can_transmit_error_number_get(can);
    self->counters.rec = can_receive_error_number_get(can);
    uint8_t pending = 0;
    for (uint8_t i = 0; i < CAN_NUM_TX_MAILBOX; i++) {
        if (self->port->tx_in_use & (1u << i)) {
            pending++;
        }
    }
    self->counters.tx_pending = pending;
    self->counters.rx_pending =
        can_receive_message_length_get(can, CAN_FIFO0) +
        can_receive_message_length_get(can, CAN_FIFO1);
}

static mp_obj_t machine_can_port_get_additional_timings(machine_can_obj_t *self, mp_obj_t optional_arg) {
    (void)self;
    (void)optional_arg;
    return mp_const_none;
}

static mp_uint_t machine_can_port_irq_flags(machine_can_obj_t *self) {
    uint32_t can = can_periph((uint8_t)self->can_idx);
    mp_uint_t flags = 0;

    if (self->port->tx_in_use) {
        // check completed transmissions
        for (uint8_t i = 0; i < CAN_NUM_TX_MAILBOX; i++) {
            if (!(self->port->tx_in_use & (1u << i))) {
                continue;
            }
            if (can_flag_get(can, (can_flag_enum)(CAN_FLAG_MTF0 << i)) != RESET) {
                can_flag_clear(can, (can_flag_enum)(CAN_FLAG_MTF0 << i));
                self->port->tx_in_use &= (uint16_t)~(1u << i);
                flags |= MP_CAN_IRQ_TX | ((mp_uint_t)i << MP_CAN_IRQ_IDX_SHIFT);
            }
        }
    }

    if (can_receive_message_length_get(can, CAN_FIFO0) > 0 ||
        can_receive_message_length_get(can, CAN_FIFO1) > 0) {
        flags |= MP_CAN_IRQ_RX;
    }

    return flags;
}

// ---- IRQ handlers ----
void CAN0_RX0_IRQHandler(void) {
    if (can_obj_by_idx[0] && can_obj_by_idx[0]->mp_irq_obj) {
        mp_irq_handler(can_obj_by_idx[0]->mp_irq_obj);
    }
}

void CAN1_RX0_IRQHandler(void) {
    if (can_obj_by_idx[1] && can_obj_by_idx[1]->mp_irq_obj) {
        mp_irq_handler(can_obj_by_idx[1]->mp_irq_obj);
    }
}
