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

#include "i2c.h"

#define I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL (1 << 9)
#define I2C_IC_CON_TX_EMPTY_CTRL (1 << 8)
#define I2C_IC_CON_STOP_DET_IFADDRESSED (1 << 7)

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    I2C_Type *i2c;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
    uint8_t state;
    bool stop_pending;
    bool irq_active;
} machine_i2c_target_obj_t;

static machine_i2c_target_obj_t machine_i2c_target_obj[] = {
    #if defined(MICROPY_HW_I2C0_SCL)
    [0] = {{&machine_i2c_target_type}, (I2C_Type *)I2C0_BASE, MICROPY_HW_I2C0_SCL, MICROPY_HW_I2C0_SDA},
    #endif
    #if defined(MICROPY_HW_I2C1_SCL)
    [1] = {{&machine_i2c_target_type}, (I2C_Type *)I2C1_BASE, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA},
    #endif
    #if defined(MICROPY_HW_I2C2_SCL)
    [2] = {{&machine_i2c_target_type}, (I2C_Type *)I2C2_BASE, MICROPY_HW_I2C2_SCL, MICROPY_HW_I2C2_SDA},
    #endif
    #if defined(MICROPY_HW_I2C3_SCL)
    [3] = {{&machine_i2c_target_type}, (I2C_Type *)I2C3_BASE, MICROPY_HW_I2C3_SCL, MICROPY_HW_I2C3_SDA},
    #endif
};

/******************************************************************************/
// Alif I2C hardware bindings
//
// The hardware triggers the following IRQs for the given scenarios:
// - scan (0-byte write) of another target: START_DET
// - scan (0-byte write) of us:             START_DET STOP_DET
// - write of n bytes:                      START_DET RX_FULL*n STOP_DET
// - write of n bytes then read of m bytes: START_DET RX_FULL*n START_DET RD_REQ*m RX_DONE STOP_DET

static inline unsigned int i2c_reg_base_to_index(I2C_Type *i2c) {
    return ((uintptr_t)i2c - I2C0_BASE) / (I2C1_BASE - I2C0_BASE);
}

static const uint32_t i2c_irq_num[] = { I2C0_IRQ_IRQn, I2C1_IRQ_IRQn, I2C2_IRQ_IRQn, I2C3_IRQ_IRQn };

static void check_stop_pending(machine_i2c_target_obj_t *self) {
    if (self->irq_active) {
        return;
    }
    if (self->stop_pending && !(self->i2c->I2C_STATUS & I2C_IC_STATUS_RECEIVE_FIFO_NOT_EMPTY)) {
        unsigned int i2c_id = self - &machine_i2c_target_obj[0];
        machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id];
        self->stop_pending = false;
        self->state = STATE_IDLE;
        machine_i2c_target_data_restart_or_stop(data);
    }
}

static void i2c_target_irq_handler(machine_i2c_target_obj_t *self) {
    unsigned int i2c_id = self - &machine_i2c_target_obj[0];
    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id];
    I2C_Type *i2c = self->i2c;

    self->irq_active = true;

    // Get the interrupt status.
    uint32_t intr_stat = i2c->I2C_RAW_INTR_STAT;

    if (intr_stat & I2C_IC_INTR_STAT_TX_ABRT) {
        // Clear the TX_ABRT condition.
        (void)i2c->I2C_CLR_TX_ABRT;
    }

    if (intr_stat & I2C_IC_INTR_STAT_START_DET) {
        // Controller sent a start condition.
        // Reset all state machines in case something went wrong.
        (void)i2c->I2C_CLR_START_DET;
        if (self->state != STATE_IDLE) {
            machine_i2c_target_data_reset_helper(data);
            self->state = STATE_IDLE;
        }
    }

    if (intr_stat & I2C_IC_INTR_STAT_RX_FULL) {
        // Data from controller is available for reading.
        // Mask interrupt until I2C_DATA_CMD is read from.
        i2c->I2C_INTR_MASK &= ~I2C_IC_INTR_STAT_RX_FULL;
        if (self->state != STATE_WRITING) {
            machine_i2c_target_data_addr_match(data, false);
        }
        machine_i2c_target_data_write_request(self, data);
        self->state = STATE_WRITING;
    }

    if (intr_stat & (I2C_IC_INTR_STAT_RD_REQ | I2C_IC_INTR_STAT_RX_DONE)) {
        // Controller is requesting data.
        // Note: for RX_DONE interrupt, no data needs to be written but this event is
        // needed to match the expected I2CTarget event behaviour.  A TX_ABTR interrupt
        // will be fired after the unused byte is written to I2C_DATA_CMD, and clearing
        // that abort is required to reset the hardware I2C state machine.
        (void)i2c->I2C_CLR_RX_DONE;
        (void)i2c->I2C_CLR_RD_REQ;
        i2c->I2C_INTR_MASK &= ~I2C_IC_INTR_STAT_RD_REQ;
        if (self->state != STATE_READING) {
            machine_i2c_target_data_addr_match(data, true);
        }
        machine_i2c_target_data_read_request(self, data);
        self->state = STATE_READING;
    }

    if (intr_stat & I2C_IC_INTR_STAT_STOP_DET) {
        // Controller has generated a stop condition.
        (void)i2c->I2C_CLR_STOP_DET;
        if (self->state == STATE_IDLE) {
            machine_i2c_target_data_addr_match(data, false);
        }
        if (i2c->I2C_STATUS & I2C_IC_STATUS_RECEIVE_FIFO_NOT_EMPTY) {
            self->stop_pending = true;
        } else {
            machine_i2c_target_data_restart_or_stop(data);
            self->state = STATE_IDLE;
        }
    }

    self->irq_active = false;
    check_stop_pending(self);
}

void I2C0_IRQHandler(void) {
    i2c_target_irq_handler(&machine_i2c_target_obj[0]);
}

void I2C1_IRQHandler(void) {
    i2c_target_irq_handler(&machine_i2c_target_obj[1]);
}

void I2C2_IRQHandler(void) {
    i2c_target_irq_handler(&machine_i2c_target_obj[2]);
}

void I2C3_IRQHandler(void) {
    i2c_target_irq_handler(&machine_i2c_target_obj[3]);
}

static void i2c_target_init(I2C_Type *i2c, uint16_t addr, bool addr_10bit) {
    i2c_disable(i2c);

    uint32_t ic_con_reg = 0;
    ic_con_reg |= I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL;
    ic_con_reg |= I2C_IC_CON_STOP_DET_IFADDRESSED;
    if (addr_10bit) {
        ic_con_reg |= I2C_SLAVE_10BIT_ADDR_MODE;
    }
    i2c->I2C_CON = ic_con_reg;
    i2c->I2C_SAR = addr & I2C_IC_SAR_10BIT_ADDR_MASK;
    i2c->I2C_TX_TL = 1;
    i2c->I2C_RX_TL = 0; // interrupt when at least 1 byte is available
    i2c_clear_all_interrupt(i2c);

    // Enable interrupts.
    i2c->I2C_INTR_MASK =
        I2C_IC_INTR_STAT_STOP_DET
        | I2C_IC_INTR_STAT_RX_DONE
        | I2C_IC_INTR_STAT_TX_ABRT
        | I2C_IC_INTR_STAT_RD_REQ
        | I2C_IC_INTR_STAT_RX_FULL
    ;

    i2c_enable(i2c);

    // Enable I2C interrupts.
    uint32_t irq_num = i2c_irq_num[i2c_reg_base_to_index(i2c)];
    NVIC_ClearPendingIRQ(irq_num);
    NVIC_SetPriority(irq_num, IRQ_PRI_I2C);
    NVIC_EnableIRQ(irq_num);
}

static void i2c_target_deinit(I2C_Type *i2c) {
    uint32_t irq_num = i2c_irq_num[i2c_reg_base_to_index(i2c)];
    NVIC_DisableIRQ(irq_num);
    i2c_disable(i2c);
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
    I2C_Type *i2c = self->i2c;

    // Read from the RX FIFO.
    size_t i = 0;
    while (i < len && (i2c->I2C_STATUS & I2C_IC_STATUS_RECEIVE_FIFO_NOT_EMPTY)) {
        buf[i++] = i2c->I2C_DATA_CMD;
    }

    // Re-enable RX_FULL interrupt.
    i2c->I2C_INTR_MASK |= I2C_IC_INTR_STAT_RX_FULL;

    check_stop_pending(self);

    return i;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    // Write to the TX FIFO.
    size_t i = 0;
    while (i < len && (self->i2c->I2C_STATUS & I2C_IC_STATUS_TRANSMIT_FIFO_NOT_FULL)) {
        self->i2c->I2C_DATA_CMD = buf[i++];
    }

    // Re-enable RD_REQ interrupt.
    self->i2c->I2C_INTR_MASK |= I2C_IC_INTR_STAT_RD_REQ;

    return 1;
}

static inline void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
    (void)self;
    (void)trigger;
}

static mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int i2c_id = args[ARG_id].u_int;

    // Check if the I2C bus is valid
    if (i2c_id < 0 || i2c_id >= MP_ARRAY_SIZE(machine_i2c_target_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2CTarget(%d) doesn't exist"), i2c_id);
    }

    // Get static peripheral object.
    machine_i2c_target_obj_t *self = &machine_i2c_target_obj[i2c_id];

    // Disable I2C controller.
    i2c_disable(self->i2c);

    // Initialise data.
    self->state = STATE_IDLE;
    self->stop_pending = false;
    self->irq_active = false;
    MP_STATE_PORT(machine_i2c_target_mem_obj)[i2c_id] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    // Set SCL/SDA pins if given.
    if (args[ARG_scl].u_obj != mp_const_none) {
        self->scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
    }
    if (args[ARG_sda].u_obj != mp_const_none) {
        self->sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
    }

    // Configure I2C pins.
    mp_hal_pin_config(self->scl, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP,
        MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT(I2C_SCL, i2c_id), true);
    mp_hal_pin_config(self->sda, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP,
        MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT(I2C_SDA, i2c_id), true);

    // Initialise the I2C target.
    i2c_target_init(self->i2c, args[ARG_addr].u_int, args[ARG_addrsize].u_int == 10);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2CTarget(%u, addr=%u, scl=" MP_HAL_PIN_FMT ", sda=" MP_HAL_PIN_FMT ")",
        self - &machine_i2c_target_obj[0], self->i2c->I2C_SAR, mp_hal_pin_name(self->scl), mp_hal_pin_name(self->sda));
}

static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    i2c_target_deinit(self->i2c);
}
