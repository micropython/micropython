// machine_i2c_port.c - machine.I2C (hardware) for GD32F4xx

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

#include "gd32f4xx.h"
#include "pin_defs.h"

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    uint32_t i2c;
    uint32_t rcu;
    uint8_t  idx;
    uint32_t freq;
    int16_t  scl_pin;
    int16_t  sda_pin;
} machine_i2c_obj_t;

static const struct {
    uint32_t i2c;
    uint32_t rcu;
} i2c_hw[] = {
    { I2C0, RCU_I2C0 },
    { I2C1, RCU_I2C1 },
    { I2C2, RCU_I2C2 },
};

#define NUM_I2C MP_ARRAY_SIZE(i2c_hw)

static machine_i2c_obj_t machine_i2c_obj[NUM_I2C];


// Look up the alternate function for an I2C pin. Most I2C pins use AF4,
// but a few (notably PB3 as I2C1_SDA) are mapped to AF9 on GD32F4.
static uint32_t i2c_pin_af(uint8_t pin_enc) {
    const uint8_t port = PIN_PORT(pin_enc);
    const uint8_t num = PIN_NUM(pin_enc);

    // PB3 -> I2C1_SDA via AF9
    if (port == 1 && num == 3) {
        return GPIO_AF_9;
    }
    // Everything else (PB6/PB7, PB8/PB9, PB10/PB11, PA8, PC9) is AF4.
    return GPIO_AF_4;
}

static void i2c_cfg_pin(int16_t enc_pin) {
    if (enc_pin < 0) {
        return;
    }
    uint8_t port = PIN_PORT((uint8_t)enc_pin);
    uint8_t num = PIN_NUM((uint8_t)enc_pin);
    uint32_t base = pin_port_base(port);
    uint32_t mask = pin_hal_mask(num);

    rcu_periph_clock_enable(pin_port_rcu(port));
    gpio_af_set(base, i2c_pin_af((uint8_t)enc_pin), mask);
    gpio_mode_set(base, GPIO_MODE_AF, GPIO_PUPD_PULLUP, mask);
    gpio_output_options_set(base, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, mask);
}

// ---- basic master transfer ----
static int i2c_wait_flag(uint32_t i2c, uint32_t flag, FlagStatus status, uint32_t timeout_ms) {
    mp_uint_t start = mp_hal_ticks_ms();
    while (i2c_flag_get(i2c, flag) != status) {
        if ((mp_hal_ticks_ms() - start) > timeout_ms) {
            return -MP_ETIMEDOUT;
        }
    }
    return 0;
}

static int i2c_start_addr(machine_i2c_obj_t *self, uint16_t addr, bool read) {
    // wait for bus idle
    int ret = i2c_wait_flag(self->i2c, I2C_FLAG_I2CBSY, RESET, 100);
    if (ret) {
        return ret;
    }

    i2c_start_on_bus(self->i2c);
    ret = i2c_wait_flag(self->i2c, I2C_FLAG_SBSEND, SET, 100);
    if (ret) {
        return ret;
    }

    i2c_master_addressing(self->i2c, addr << 1, read ? I2C_RECEIVER : I2C_TRANSMITTER);
    ret = i2c_wait_flag(self->i2c, I2C_FLAG_ADDSEND, SET, 100);
    if (ret) {
        return ret;
    }
    i2c_flag_clear(self->i2c, I2C_FLAG_ADDSEND);
    return 0;
}

static int i2c_write_bytes(machine_i2c_obj_t *self, const uint8_t *src, size_t len) {
    for (size_t i = 0; i < len; i++) {
        int ret = i2c_wait_flag(self->i2c, I2C_FLAG_TBE, SET, 100);
        if (ret) {
            return ret;
        }
        i2c_data_transmit(self->i2c, src[i]);
    }
    return i2c_wait_flag(self->i2c, I2C_FLAG_BTC, SET, 100);
}

static int i2c_read_bytes(machine_i2c_obj_t *self, uint8_t *dest, size_t len) {
    if (len == 0) {
        return 0;
    }
    // enable ACK for all but last byte
    i2c_ack_config(self->i2c, I2C_ACK_ENABLE);
    for (size_t i = 0; i < len; i++) {
        if (i == len - 1) {
            i2c_ack_config(self->i2c, I2C_ACK_DISABLE);
        }
        int ret = i2c_wait_flag(self->i2c, I2C_FLAG_RBNE, SET, 100);
        if (ret) {
            return ret;
        }
        dest[i] = i2c_data_receive(self->i2c);
    }
    i2c_ack_config(self->i2c, I2C_ACK_ENABLE);
    return 0;
}

static void i2c_stop_bus(machine_i2c_obj_t *self) {
    i2c_stop_on_bus(self->i2c);
    mp_uint_t start = mp_hal_ticks_ms();
    while (i2c_flag_get(self->i2c, I2C_FLAG_STPDET) == RESET) {
        if ((mp_hal_ticks_ms() - start) > 10) {
            break;
        }
    }
    i2c_flag_clear(self->i2c, I2C_FLAG_STPDET);
}

// ---- protocol ----
static void mp_machine_i2c_init(mp_obj_base_t *obj, size_t n_args,
                                const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)obj;
    enum { ARG_freq, ARG_timeout, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 50000} },
        { MP_QSTR_scl,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->freq = args[ARG_freq].u_int;

    rcu_periph_clock_enable(self->rcu);
    rcu_periph_clock_enable(RCU_GPIOB);   // I2C pins typically on B

    if (args[ARG_scl].u_obj != mp_const_none) {
        self->scl_pin = (int16_t)mp_obj_get_int(args[ARG_scl].u_obj);
    }
    if (args[ARG_sda].u_obj != mp_const_none) {
        self->sda_pin = (int16_t)mp_obj_get_int(args[ARG_sda].u_obj);
    }
    i2c_cfg_pin(self->scl_pin);
    i2c_cfg_pin(self->sda_pin);

    i2c_deinit(self->i2c);
    i2c_clock_config(self->i2c, self->freq, I2C_DTCY_2);
    i2c_mode_addr_config(self->i2c, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x00);
    i2c_enable(self->i2c);
    i2c_ack_config(self->i2c, I2C_ACK_ENABLE);
}

static void mp_machine_i2c_deinit(mp_obj_base_t *obj) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)obj;
    i2c_disable(self->i2c);
    i2c_deinit(self->i2c);
    rcu_periph_clock_disable(self->rcu);
}

static int mp_machine_i2c_transfer(mp_obj_base_t *obj, uint16_t addr, size_t n,
                                   mp_machine_i2c_buf_t *bufs, unsigned int flags) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)obj;
    int ret;

    if (!(flags & MP_MACHINE_I2C_FLAG_READ)) {
        // write phase
        ret = i2c_start_addr(self, addr, false);
        if (ret) {
            return ret;
        }
        for (size_t i = 0; i < n; i++) {
            if (bufs[i].len == 0) {
                continue;
            }
            ret = i2c_write_bytes(self, bufs[i].buf, bufs[i].len);
            if (ret) {
                i2c_stop_bus(self);
                return ret;
            }
        }
        if (!(flags & MP_MACHINE_I2C_FLAG_STOP)) {
            return 0;
        }
        i2c_stop_bus(self);
        return 0;
    }

    // read phase
    ret = i2c_start_addr(self, addr, true);
    if (ret) {
        return ret;
    }
    for (size_t i = 0; i < n; i++) {
        if (bufs[i].len == 0) {
            continue;
        }
        bool last = (i == n - 1) && (flags & MP_MACHINE_I2C_FLAG_STOP);
        ret = i2c_read_bytes(self, bufs[i].buf, bufs[i].len);
        if (ret) {
            i2c_stop_bus(self);
            return ret;
        }
        (void)last;
    }
    i2c_stop_bus(self);
    return 0;
}

static const mp_machine_i2c_p_t machine_i2c_p = {
    .init = mp_machine_i2c_init,
    .deinit = mp_machine_i2c_deinit,
    .transfer = mp_machine_i2c_transfer,
};

static mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args,
                                     size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    mp_int_t i2c_id = mp_obj_get_int(all_args[0]);
    if (i2c_id < 0 || (mp_uint_t)i2c_id >= NUM_I2C) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid I2C bus"));
    }

    machine_i2c_obj_t *self = &machine_i2c_obj[i2c_id];
    self->base.type = &machine_i2c_type;
    self->i2c = i2c_hw[i2c_id].i2c;
    self->rcu = i2c_hw[i2c_id].rcu;
    self->idx = (uint8_t)i2c_id;
    self->scl_pin = -1;
    self->sda_pin = -1;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_machine_i2c_init(&self->base, n_args - 1, all_args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    protocol, &machine_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );
