// machine_i2c_target_port.c - machine.I2CTarget for GD32F4xx
//
// Included by extmod/machine_i2c_target.c via
// MICROPY_PY_MACHINE_I2C_TARGET_INCLUDEFILE.
//
// GD32F4 I2C works as a bus target when its own address is programmed and the
// peripheral is enabled in I2C mode.  Address match, byte transfer and STOP
// detection are driven from the event interrupt, which forwards them to the
// extmod helpers (machine_i2c_target_data_addr_match / _read_request /
// _write_request / _stop) exactly as the STM32 port does.

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "gd32f4xx.h"
#include "pin_defs.h"

#ifndef MICROPY_PY_MACHINE_I2C_TARGET_MAX
#define MICROPY_PY_MACHINE_I2C_TARGET_MAX (2)
#endif

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    uint32_t i2c;          // I2C0/I2C1/I2C2
    uint32_t rcu;
    uint8_t  idx;
    uint8_t  addr;         // 7-bit target address
    int16_t  scl_pin;      // encoded pin or -1
    int16_t  sda_pin;
    bool     active;
    bool     master_reads; // direction of the current transfer
} machine_i2c_target_obj_t;

static const struct {
    uint32_t i2c;
    uint32_t rcu;
} i2c_target_hw[] = {
    { I2C0, RCU_I2C0 },
    { I2C1, RCU_I2C1 },
    { I2C2, RCU_I2C2 },
};

#define NUM_I2C_TARGET MP_ARRAY_SIZE(i2c_target_hw)

static machine_i2c_target_obj_t machine_i2c_target_obj[NUM_I2C_TARGET];

// ---- helpers ----
static machine_i2c_target_obj_t *i2c_target_lookup(uint32_t i2c) {
    for (size_t i = 0; i < NUM_I2C_TARGET; i++) {
        if (machine_i2c_target_obj[i].i2c == i2c && machine_i2c_target_obj[i].active) {
            return &machine_i2c_target_obj[i];
        }
    }
    return NULL;
}

static machine_i2c_target_irq_obj_t *i2c_target_irq(machine_i2c_target_obj_t *self) {
    return MP_STATE_PORT(machine_i2c_target_irq_obj)[self->idx];
}

static void i2c_target_pin_config(int16_t enc_pin) {
    if (enc_pin < 0) {
        return;
    }
    uint8_t port = PIN_PORT((uint8_t)enc_pin);
    uint8_t num = PIN_NUM((uint8_t)enc_pin);
    rcu_periph_clock_enable(pin_port_rcu(port));
    // I2C pins are AF4; PB3 used as I2C1_SDA is AF9 on GD32F4.
    uint32_t af = (port == 1 && num == 3) ? GPIO_AF_9 : GPIO_AF_4;
    gpio_af_set(pin_port_base(port), af, pin_hal_mask(num));
    gpio_mode_set(pin_port_base(port), GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin_hal_mask(num));
    gpio_output_options_set(pin_port_base(port), GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, pin_hal_mask(num));
}

// ---- extmod hooks ----
static inline size_t mp_machine_i2c_target_get_index(machine_i2c_target_obj_t *self) {
    return self->idx;
}

static inline void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mp_irq_handler(&irq->base);
}

// The extmod layer asks us to hand one byte to the master.  "read_bytes"
// here means "produce the next byte the master will read", so the byte comes
// from the extmod-controlled buffer and goes into the data register.
static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    if (len == 0) {
        return 0;
    }
    // Wait for the transmit buffer to drain; the controller stretches SCL
    // until a byte is supplied.
    mp_uint_t start = mp_hal_ticks_ms();
    while (i2c_flag_get(self->i2c, I2C_FLAG_TBE) == RESET) {
        if ((mp_hal_ticks_ms() - start) > 100) {
            return 0;
        }
    }
    i2c_data_transmit(self->i2c, buf[0]);
    return 1;
}

// The extmod layer pushes a byte that the master has just written.  The byte
// already sits in our data register, so drain it and hand it back through the
// const buffer pointer (same convention as the STM32 port).
static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    (void)buf;
    if (len == 0) {
        return 0;
    }
    mp_uint_t start = mp_hal_ticks_ms();
    while (i2c_flag_get(self->i2c, I2C_FLAG_RBNE) == RESET) {
        if ((mp_hal_ticks_ms() - start) > 100) {
            return 0;
        }
    }
    // Discard: the caller already has the value in buf.
    (void)i2c_data_receive(self->i2c);
    return 1;
}

static inline void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
    (void)self;
    (void)trigger;
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args,
                                               size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,           MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addr,         MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_scl,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t i2c_id = args[ARG_id].u_int;
    if (i2c_id < 0 || (mp_uint_t)i2c_id >= NUM_I2C_TARGET) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid I2C bus"));
    }

    machine_i2c_target_obj_t *self = &machine_i2c_target_obj[i2c_id];
    if (self->active) {
        mp_raise_ValueError(MP_ERROR_TEXT("I2CTarget already active"));
    }

    self->base.type = type;
    self->i2c = i2c_target_hw[i2c_id].i2c;
    self->rcu = i2c_target_hw[i2c_id].rcu;
    self->idx = (uint8_t)i2c_id;
    self->addr = (uint8_t)(args[ARG_addr].u_int & 0x7F);
    self->active = false;
    self->master_reads = false;
    self->scl_pin = (args[ARG_scl].u_obj == mp_const_none)
        ? -1 : (int16_t)mp_obj_get_int(args[ARG_scl].u_obj);
    self->sda_pin = (args[ARG_sda].u_obj == mp_const_none)
        ? -1 : (int16_t)mp_obj_get_int(args[ARG_sda].u_obj);

    // Seed the memory object for the extmod layer.
    size_t data_idx = mp_machine_i2c_target_get_index(self);
    MP_STATE_PORT(machine_i2c_target_mem_obj)[data_idx] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[data_idx];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    rcu_periph_clock_enable(self->rcu);
    i2c_target_pin_config(self->scl_pin);
    i2c_target_pin_config(self->sda_pin);

    i2c_deinit(self->i2c);
    // Program the target address (SADDR0 expects the 8-bit shifted form).
    i2c_mode_addr_config(self->i2c, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS,
                         (uint32_t)(self->addr << 1));
    i2c_ack_config(self->i2c, I2C_ACK_ENABLE);
    i2c_enable(self->i2c);

    i2c_interrupt_enable(self->i2c, I2C_INT_EV | I2C_INT_ERR | I2C_INT_BUF);

    IRQn_Type irqn = (i2c_id == 0) ? I2C0_EV_IRQn
                   : (i2c_id == 1) ? I2C1_EV_IRQn : I2C2_EV_IRQn;
    NVIC_SetPriority(irqn, 2);
    NVIC_EnableIRQ(irqn);

    self->active = true;
    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%u, addr=0x%02x)", (unsigned)self->idx, (unsigned)self->addr);
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    IRQn_Type irqn = (self->idx == 0) ? I2C0_EV_IRQn
                   : (self->idx == 1) ? I2C1_EV_IRQn : I2C2_EV_IRQn;
    NVIC_DisableIRQ(irqn);
    i2c_interrupt_disable(self->i2c, I2C_INT_EV | I2C_INT_ERR | I2C_INT_BUF);
    i2c_disable(self->i2c);
    i2c_deinit(self->i2c);
    rcu_periph_clock_disable(self->rcu);
    self->active = false;
}

// ---- event service ----
// Translate the GD32 peripheral state into the extmod helper calls.  This runs
// inside the event interrupt, so it must not block.
static void i2c_target_ev_handler(uint32_t i2c) {
    machine_i2c_target_obj_t *self = i2c_target_lookup(i2c);
    if (self == NULL) {
        return;
    }
    machine_i2c_target_data_t *data = &machine_i2c_target_data[self->idx];
    machine_i2c_target_irq_obj_t *irq = i2c_target_irq(self);

    // Address matched: the master has selected us.
    if (i2c_interrupt_flag_get(i2c, I2C_INT_FLAG_ADDSEND) != RESET) {
        i2c_interrupt_flag_clear(i2c, I2C_INT_FLAG_ADDSEND);
        // I2C_FLAG_TR set means the master wants to read from us.
        self->master_reads = (i2c_flag_get(i2c, I2C_FLAG_TR) != RESET);
        machine_i2c_target_data_addr_match(data, self->master_reads);
        if (irq != NULL) {
            mp_irq_handler(&irq->base);
        }
        return;
    }

    // STOP detected: the master ended the transfer.
    if (i2c_interrupt_flag_get(i2c, I2C_INT_FLAG_STPDET) != RESET) {
        i2c_interrupt_flag_clear(i2c, I2C_INT_FLAG_STPDET);
        machine_i2c_target_data_stop(data);
        if (irq != NULL) {
            mp_irq_handler(&irq->base);
        }
        return;
    }

    // Master wrote a byte to us.
    if (i2c_interrupt_flag_get(i2c, I2C_INT_FLAG_RBNE) != RESET) {
        if (!self->master_reads) {
            machine_i2c_target_data_write_request(self, data);
        } else {
            (void)i2c_data_receive(i2c);
        }
        return;
    }

    // Master wants to read a byte from us.
    if (i2c_interrupt_flag_get(i2c, I2C_INT_FLAG_TBE) != RESET) {
        if (self->master_reads) {
            machine_i2c_target_data_read_request(self, data);
        }
        return;
    }

    // Clear error flags so the peripheral keeps serving.
    if (i2c_interrupt_flag_get(i2c, I2C_INT_FLAG_AERR) != RESET) {
        i2c_interrupt_flag_clear(i2c, I2C_INT_FLAG_AERR);
    }
    if (i2c_interrupt_flag_get(i2c, I2C_INT_FLAG_BERR) != RESET) {
        i2c_interrupt_flag_clear(i2c, I2C_INT_FLAG_BERR);
    }
}

void I2C0_EV_IRQHandler(void) {
    i2c_target_ev_handler(I2C0);
}

void I2C1_EV_IRQHandler(void) {
    i2c_target_ev_handler(I2C1);
}

void I2C2_EV_IRQHandler(void) {
    i2c_target_ev_handler(I2C2);
}
