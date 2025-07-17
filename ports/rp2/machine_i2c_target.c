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

#include "hardware/i2c.h"

// TODO: should be in a common i2c header
#ifdef MICROPY_HW_I2C_NO_DEFAULT_PINS

// With no default I2C, need to require the pin args.
#define MICROPY_HW_I2C0_SCL (0)
#define MICROPY_HW_I2C0_SDA (0)
#define MICROPY_HW_I2C1_SCL (0)
#define MICROPY_HW_I2C1_SDA (0)
#define MICROPY_I2C_PINS_ARG_OPTS MP_ARG_REQUIRED

#else

// Most boards do not require pin args.
#define MICROPY_I2C_PINS_ARG_OPTS 0

#ifndef MICROPY_HW_I2C0_SCL
#if PICO_DEFAULT_I2C == 0
#define MICROPY_HW_I2C0_SCL (PICO_DEFAULT_I2C_SCL_PIN)
#define MICROPY_HW_I2C0_SDA (PICO_DEFAULT_I2C_SDA_PIN)
#else
#define MICROPY_HW_I2C0_SCL (9)
#define MICROPY_HW_I2C0_SDA (8)
#endif
#endif

#ifndef MICROPY_HW_I2C1_SCL
#if PICO_DEFAULT_I2C == 1
#define MICROPY_HW_I2C1_SCL (PICO_DEFAULT_I2C_SCL_PIN)
#define MICROPY_HW_I2C1_SDA (PICO_DEFAULT_I2C_SDA_PIN)
#else
#define MICROPY_HW_I2C1_SCL (7)
#define MICROPY_HW_I2C1_SDA (6)
#endif
#endif
#endif

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
// TODO: should be in a common i2c header
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    i2c_inst_t *const i2c_inst;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
    uint8_t state;
} machine_i2c_target_obj_t;

static machine_i2c_target_data_t i2c_target_data[4];

static machine_i2c_target_obj_t machine_i2c_target_obj[] = {
    {{&machine_i2c_target_type}, i2c0, MICROPY_HW_I2C0_SCL, MICROPY_HW_I2C0_SDA},
    {{&machine_i2c_target_type}, i2c1, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA},
};

/******************************************************************************/
// RP2xxx hardware bindings

static void i2c_target_handler(i2c_inst_t *i2c) {
    unsigned int i2c_id = i2c == i2c0 ? 0 : 1;
    machine_i2c_target_obj_t *self = &machine_i2c_target_obj[i2c_id];
    machine_i2c_target_data_t *data = &i2c_target_data[i2c_id];

    // Get the interrupt status.
    uint32_t intr_stat = i2c->hw->intr_stat;

    if (intr_stat & I2C_IC_INTR_STAT_R_TX_ABRT_BITS) {
        // Clear the TX_ABRT condition.
        (void)i2c->hw->clr_tx_abrt;
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
        machine_i2c_target_data_restart_or_stop(data);
        self->state = STATE_IDLE;
    }
}

static void i2c_slave_irq_handler(void) {
    uint i2c_index = __get_current_exception() - VTABLE_FIRST_IRQ - I2C0_IRQ;
    i2c_inst_t *i2c = i2c_get_instance(i2c_index);
    i2c_target_handler(i2c);
}

static void i2c_slave_init(i2c_inst_t *i2c, uint16_t addr, bool addr_10bit) {
    i2c->hw->enable = 0;

    // Note: The I2C slave does clock stretching implicitly after a RD_REQ, while the Tx FIFO is empty.
    // Clock stretching while the Rx FIFO is full is also enabled by default.
    i2c->hw->con = I2C_IC_CON_STOP_DET_IFADDRESSED_BITS;
    if (addr_10bit) {
        i2c->hw->con |= I2C_IC_CON_IC_10BITADDR_SLAVE_BITS;
    }
    i2c->hw->sar = addr;
    i2c->hw->tx_tl = 1;
    i2c->hw->rx_tl = 0; // interrupt when at least 1 byte is available

    // Enable interrupts.
    i2c->hw->intr_mask =
        I2C_IC_INTR_MASK_M_STOP_DET_BITS
        | I2C_IC_INTR_MASK_M_RX_DONE_BITS
        | I2C_IC_INTR_MASK_M_TX_ABRT_BITS
        | I2C_IC_INTR_MASK_M_RD_REQ_BITS
        | I2C_IC_INTR_MASK_M_RX_FULL_BITS
    ;

    i2c->hw->enable = 1;

    // Enable interrupt for current core.
    uint i2c_index = i2c_hw_index(i2c);
    uint num = I2C0_IRQ + i2c_index;
    irq_set_exclusive_handler(num, i2c_slave_irq_handler);
    irq_set_enabled(num, true);
}

static void i2c_slave_deinit(i2c_inst_t *i2c) {
    uint i2c_index = i2c_hw_index(i2c);
    uint num = I2C0_IRQ + i2c_index;
    irq_set_enabled(num, false);
    irq_remove_handler(num, i2c_slave_irq_handler);

    i2c->hw->intr_mask = 0;
    i2c->hw->enable = 0;
}

/******************************************************************************/
// I2CTarget port implementation

static void mp_machine_i2c_target_deinit_all_port(void) {
}

static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mp_irq_handler(&irq->base);
}

static mp_int_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    i2c_hw_t *i2c_hw = self->i2c_inst->hw;

    // Read from the RX FIFO.
    mp_int_t i = 0;
    while (i < len && (i2c_hw->status & I2C_IC_STATUS_RFNE_BITS)) {
        buf[i++] = i2c_hw->data_cmd;
    }

    // Re-enable RX_FULL interrupt.
    i2c_hw->intr_mask |= I2C_IC_INTR_MASK_M_RX_FULL_BITS;

    return i;
}

static mp_int_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    i2c_hw_t *i2c_hw = self->i2c_inst->hw;

    // Write to the TX FIFO.
    i2c_hw->data_cmd = buf[0];

    // Re-enable RD_REQ interrupt.
    i2c_hw->intr_mask |= I2C_IC_INTR_MASK_M_RD_REQ_BITS;

    return 1;
}

static void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // TODO: reconsider order of arguments
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

    // Initialise data.
    self->state = STATE_IDLE;
    MP_STATE_PORT(i2c_target_mem_obj)[i2c_id] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &i2c_target_data[i2c_id];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

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

    // Initialise the I2C target.
    // TODO: don't reinitialize if no arguments given.
    i2c_slave_init(self->i2c_inst, args[ARG_addr].u_int, args[ARG_addrsize].u_int == 10);
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
    i2c_slave_deinit(self->i2c_inst);
}
