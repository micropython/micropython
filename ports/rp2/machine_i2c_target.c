/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
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
// extmod/machine_i2c_target.c via MICROPY_PY_MACHINE_I2C_TARGET_INCLUDEFILE.

#include "machine_i2c.h"
#include "hardware/i2c.h"

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    i2c_inst_t *const i2c_inst;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
    uint8_t state;
    bool stop_pending;
    bool irq_active;
} machine_i2c_target_obj_t;

static machine_i2c_target_obj_t machine_i2c_target_obj[] = {
    {{&machine_i2c_target_type}, i2c0, MICROPY_HW_I2C0_SCL, MICROPY_HW_I2C0_SDA},
    {{&machine_i2c_target_type}, i2c1, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA},
};

/******************************************************************************/
// RP2xxx hardware bindings

static void check_stop_pending(machine_i2c_target_obj_t *self) {
    if (self->irq_active) {
        return;
    }
    if (self->stop_pending && !(self->i2c_inst->hw->status & I2C_IC_STATUS_RFNE_BITS)) {
        unsigned int i2c_id = self - &machine_i2c_target_obj[0];
        machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id];
        self->stop_pending = false;
        self->state = STATE_IDLE;
        machine_i2c_target_data_restart_or_stop(data);
    }
}

static void i2c_target_handler(i2c_inst_t *i2c) {
    unsigned int i2c_id = i2c == i2c0 ? 0 : 1;
    machine_i2c_target_obj_t *self = &machine_i2c_target_obj[i2c_id];
    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id];

    self->irq_active = true;

    // Get the interrupt status.
    uint32_t intr_stat = i2c->hw->intr_stat;

    if (intr_stat & I2C_IC_INTR_STAT_R_TX_ABRT_BITS) {
        // Clear the TX_ABRT condition.
        (void)i2c->hw->clr_tx_abrt;
    }

    if (intr_stat & I2C_IC_INTR_STAT_R_START_DET_BITS) {
        // Controller sent a start condition.
        // Reset all state machines in case something went wrong.
        (void)i2c->hw->clr_start_det;
        if (self->state != STATE_IDLE) {
            machine_i2c_target_data_reset_helper(data);
            self->state = STATE_IDLE;
        }
    }

    if (intr_stat & I2C_IC_INTR_STAT_R_RX_FULL_BITS) {
        // Data from controller is available for reading.
        // Mask interrupt until I2C_DATA_CMD is read from.
        i2c->hw->intr_mask &= ~I2C_IC_INTR_MASK_M_RX_FULL_BITS;
        if (self->state != STATE_WRITING) {
            machine_i2c_target_data_addr_match(data, false);
        }
        machine_i2c_target_data_write_request(self, data);
        self->state = STATE_WRITING;
    }

    if (intr_stat & (I2C_IC_INTR_STAT_R_RD_REQ_BITS | I2C_IC_INTR_STAT_R_RX_DONE_BITS)) {
        // Controller is requesting data.
        (void)i2c->hw->clr_rx_done;
        (void)i2c->hw->clr_rd_req;
        i2c->hw->intr_mask &= ~I2C_IC_INTR_MASK_M_RD_REQ_BITS;
        if (self->state != STATE_READING) {
            machine_i2c_target_data_addr_match(data, true);
        }
        machine_i2c_target_data_read_request(self, data);
        self->state = STATE_READING;
    }

    if (intr_stat & I2C_IC_INTR_STAT_R_STOP_DET_BITS) {
        // Controller has generated a stop condition.
        (void)i2c->hw->clr_stop_det;
        if (self->state == STATE_IDLE) {
            machine_i2c_target_data_addr_match(data, false);
        }
        if (i2c->hw->status & I2C_IC_STATUS_RFNE_BITS) {
            self->stop_pending = true;
        } else {
            machine_i2c_target_data_restart_or_stop(data);
            self->state = STATE_IDLE;
        }
    }

    self->irq_active = false;
    check_stop_pending(self);
}

static void i2c_target_irq_handler(void) {
    uint i2c_index = __get_current_exception() - VTABLE_FIRST_IRQ - I2C0_IRQ;
    i2c_inst_t *i2c = i2c_get_instance(i2c_index);
    i2c_target_handler(i2c);
}

static void i2c_target_init(i2c_inst_t *i2c, uint16_t addr, bool addr_10bit) {
    i2c->hw->enable = 0;

    // Configure general settings, target address and FIFO levels.
    i2c->hw->con =
        I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_BITS
        | I2C_IC_CON_STOP_DET_IFADDRESSED_BITS;
    if (addr_10bit) {
        i2c->hw->con |= I2C_IC_CON_IC_10BITADDR_SLAVE_BITS;
    }
    i2c->hw->sar = addr;
    i2c->hw->tx_tl = 1;
    i2c->hw->rx_tl = 0; // interrupt when at least 1 byte is available
    (void)i2c->hw->clr_intr;

    // Enable interrupts.
    i2c->hw->intr_mask =
        I2C_IC_INTR_MASK_M_START_DET_BITS
        | I2C_IC_INTR_MASK_M_STOP_DET_BITS
        | I2C_IC_INTR_MASK_M_RX_DONE_BITS
        | I2C_IC_INTR_MASK_M_TX_ABRT_BITS
        | I2C_IC_INTR_MASK_M_RD_REQ_BITS
        | I2C_IC_INTR_MASK_M_RX_FULL_BITS
    ;

    i2c->hw->enable = 1;

    // Enable interrupt for current core.
    uint i2c_index = i2c_hw_index(i2c);
    uint num = I2C0_IRQ + i2c_index;
    irq_set_exclusive_handler(num, i2c_target_irq_handler);
    irq_set_enabled(num, true);
}

static void i2c_target_deinit(i2c_inst_t *i2c) {
    uint i2c_index = i2c_hw_index(i2c);
    uint num = I2C0_IRQ + i2c_index;
    irq_set_enabled(num, false);
    irq_remove_handler(num, i2c_target_irq_handler);

    i2c->hw->intr_mask = 0;
    i2c->hw->enable = 0;
    i2c->hw->con = I2C_IC_CON_IC_SLAVE_DISABLE_BITS;
}

/******************************************************************************/
// I2CTarget port implementation

static inline size_t mp_machine_i2c_target_get_index(machine_i2c_target_obj_t *self) {
    return self - &machine_i2c_target_obj[0];
}

static inline void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mp_irq_handler(&irq->base);
}

static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    i2c_hw_t *i2c_hw = self->i2c_inst->hw;

    // Read from the RX FIFO.
    size_t i = 0;
    while (i < len && (i2c_hw->status & I2C_IC_STATUS_RFNE_BITS)) {
        buf[i++] = i2c_hw->data_cmd;
    }

    // Re-enable RX_FULL interrupt.
    i2c_hw->intr_mask |= I2C_IC_INTR_MASK_M_RX_FULL_BITS;

    check_stop_pending(self);

    return i;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    i2c_hw_t *i2c_hw = self->i2c_inst->hw;

    // Write to the TX FIFO.
    size_t i = 0;
    while (i < len && (i2c_hw->status & I2C_IC_STATUS_TFNF_BITS)) {
        i2c_hw->data_cmd = buf[i++];
    }

    // Re-enable RD_REQ interrupt.
    i2c_hw->intr_mask |= I2C_IC_INTR_MASK_M_RD_REQ_BITS;

    return i;
}

static inline void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
    (void)self;
    (void)trigger;
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        #ifdef PICO_DEFAULT_I2C
        { MP_QSTR_id, MP_ARG_INT, {.u_int = PICO_DEFAULT_I2C} },
        #else
        { MP_QSTR_id, MP_ARG_INT | MP_ARG_REQUIRED },
        #endif
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_scl, MICROPY_I2C_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MICROPY_I2C_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int i2c_id = args[ARG_id].u_int;

    // Check if the I2C bus is valid
    if (i2c_id < 0 || i2c_id >= MP_ARRAY_SIZE(machine_i2c_target_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2CTarget(%d) doesn't exist"), i2c_id);
    }

    // Get static peripheral object.
    machine_i2c_target_obj_t *self = &machine_i2c_target_obj[i2c_id];

    // Set SCL/SDA pins if configured.
    if (args[ARG_scl].u_obj != mp_const_none) {
        int scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
        if (!IS_VALID_SCL(i2c_id, scl)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SCL pin"));
        }
        self->scl = scl;
    }
    if (args[ARG_sda].u_obj != mp_const_none) {
        int sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
        if (!IS_VALID_SDA(i2c_id, sda)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SDA pin"));
        }
        self->sda = sda;
    }

    // Initialise I2C target state and data.
    self->state = STATE_IDLE;
    self->stop_pending = false;
    self->irq_active = false;
    MP_STATE_PORT(machine_i2c_target_mem_obj)[i2c_id] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    // Initialise I2C target hardware.
    i2c_target_init(self->i2c_inst, args[ARG_addr].u_int, args[ARG_addrsize].u_int == 10);
    gpio_set_function(self->scl, GPIO_FUNC_I2C);
    gpio_set_function(self->sda, GPIO_FUNC_I2C);
    gpio_set_pulls(self->scl, true, 0);
    gpio_set_pulls(self->sda, true, 0);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    i2c_hw_t *i2c_hw = i2c_get_hw(self->i2c_inst);
    mp_printf(print, "I2CTarget(%u, addr=%u, scl=%u, sda=%u)",
        self - &machine_i2c_target_obj[0], i2c_hw->sar, self->scl, self->sda);
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    gpio_set_function(self->scl, GPIO_FUNC_SIO);
    gpio_set_function(self->sda, GPIO_FUNC_SIO);
    i2c_target_deinit(self->i2c_inst);
}
