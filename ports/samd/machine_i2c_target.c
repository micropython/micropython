/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022-2025 Robert Hammelrath
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

#include "py/runtime.h"
#include "samd_soc.h"
#include "pin_af.h"
#include "genhdr/pins.h"

#define TRANSMIT          (1)
#define RECEIVE           (0)
#define NACK_RECVD        (i2c->I2CS.STATUS.bit.RXNACK == 1)
#define IRQ_AMATCH        (i2c->I2CS.INTFLAG.bit.AMATCH == 1)
#define IRQ_DRDY          (i2c->I2CS.INTFLAG.bit.DRDY == 1)
#define IRQ_STOP          (i2c->I2CS.INTFLAG.bit.PREC == 1)

#define PREPARE_ACK       i2c->I2CS.CTRLB.bit.ACKACT = 0
#define PREPARE_NACK      i2c->I2CS.CTRLB.bit.ACKACT = 1

typedef struct _machine_i2c_target_obj_t {
    mp_obj_base_t base;
    Sercom *instance;
    uint8_t id;
    uint8_t scl;
    uint8_t sda;
    uint8_t addr;
    uint8_t direction;
} machine_i2c_target_obj_t;

void common_i2c_target_irq_handler(int i2c_id) {
    // Handle Sercom I2C IRQ for target memory mode.
    machine_i2c_target_obj_t *self = MP_STATE_PORT(sercom_table[i2c_id]);
    machine_i2c_target_data_t *data = &machine_i2c_target_data[i2c_id];

    if (self != NULL) {
        Sercom *i2c = self->instance;

        if (IRQ_AMATCH) {
            // Address match.
            self->direction = i2c->I2CS.STATUS.bit.DIR;
            machine_i2c_target_data_addr_match(data, self->direction);
            // Send ACK
            i2c->I2CS.CTRLB.bit.CMD = 3;

        } else if (IRQ_DRDY) {
            // Data to be handled, depending in the direction
            if (self->direction == TRANSMIT) {
                machine_i2c_target_data_read_request(self, data);
            } else {
                machine_i2c_target_data_write_request(self, data);
            }
            // ACK will be sent in mp_machine_i2c_target_read_bytes/mp_machine_i2c_target_write_bytes.
        } else if (IRQ_STOP) {
            // Stop detected. Just reset the data machine.
            machine_i2c_target_data_stop(data);
            i2c->I2CS.INTFLAG.reg |= SERCOM_I2CS_INTFLAG_PREC;

        } else { // On any error clear the interrupts and reset the data state.
            machine_i2c_target_data_stop(data);
            i2c->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_ERROR | SERCOM_I2CS_INTFLAG_AMATCH |
                SERCOM_I2CS_INTFLAG_DRDY | SERCOM_I2CS_INTFLAG_PREC;
        }
    }
}

/******************************************************************************/
// I2CTarget port implementation

static inline size_t mp_machine_i2c_target_get_index(machine_i2c_target_obj_t *self) {
    return self->id;
}

static void mp_machine_i2c_target_event_callback(machine_i2c_target_irq_obj_t *irq) {
    mp_irq_handler(&irq->base);
}

static size_t mp_machine_i2c_target_read_bytes(machine_i2c_target_obj_t *self, size_t len, uint8_t *buf) {
    Sercom *i2c = self->instance;
    buf[0] = i2c->I2CS.DATA.reg;
    i2c->I2CS.CTRLB.bit.CMD = 3; // send ACK
    return 1;
}

static size_t mp_machine_i2c_target_write_bytes(machine_i2c_target_obj_t *self, size_t len, const uint8_t *buf) {
    Sercom *i2c = self->instance;
    i2c->I2CS.DATA.reg = buf[0];
    i2c->I2CS.CTRLB.bit.CMD = 3; // send ACK
    return 1;
}

static inline void mp_machine_i2c_target_irq_config(machine_i2c_target_obj_t *self, unsigned int trigger) {
    (void)self;
    (void)trigger;
}

mp_obj_t mp_machine_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_addr, ARG_addrsize, ARG_mem, ARG_mem_addrsize, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        #if MICROPY_HW_DEFAULT_I2C_ID < 0
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
        #else
        { MP_QSTR_id, MP_ARG_INT, {.u_int = MICROPY_HW_DEFAULT_I2C_ID} },
        #endif
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 7} },
        { MP_QSTR_mem, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mem_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        #if defined(pin_SCL) && defined(pin_SDA)
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = pin_SCL} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = pin_SDA} },
        #else
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        #endif
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus.
    int id = args[ARG_id].u_int;
    if (id < 0 || id >= SERCOM_INST_NUM) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), id);
    }

    // Get the peripheral object.
    machine_i2c_target_obj_t *self = mp_obj_malloc_with_finaliser(machine_i2c_target_obj_t, &machine_i2c_target_type);
    self->id = id;
    self->instance = sercom_instance[id];

    // Set SCL/SDA pins.
    self->sda = pin_config_for_i2c(args[ARG_sda].u_obj, id, 0);
    self->scl = pin_config_for_i2c(args[ARG_scl].u_obj, id, 1);

    MP_STATE_PORT(sercom_table[id]) = self;

    // Get the address and initialise data.
    self->addr = args[ARG_addr].u_int;
    MP_STATE_PORT(machine_i2c_target_mem_obj)[id] = args[ARG_mem].u_obj;
    machine_i2c_target_data_t *data = &machine_i2c_target_data[id];
    machine_i2c_target_data_init(data, args[ARG_mem].u_obj, args[ARG_mem_addrsize].u_int);

    // Set up the clocks
    enable_sercom_clock(id);

    // Initialise the I2C peripheral
    Sercom *i2c = self->instance;
    // Reset the device
    i2c->I2CS.CTRLA.reg = SERCOM_I2CM_CTRLA_SWRST;
    while (i2c->I2CS.SYNCBUSY.bit.SWRST == 1) {
    }

    // Set to slave mode, enable SCl timeout, set the address
    i2c->I2CS.CTRLA.reg = SERCOM_I2CS_CTRLA_MODE(0x04)
        | SERCOM_I2CS_CTRLA_SEXTTOEN | SERCOM_I2CS_CTRLA_LOWTOUTEN;
    i2c->I2CS.ADDR.reg = self->addr << 1;

    // Enable interrupts
    sercom_register_irq(id, &common_i2c_target_irq_handler);
    #if defined(MCU_SAMD21)
    NVIC_EnableIRQ(SERCOM0_IRQn + id);
    #elif defined(MCU_SAMD51)
    NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * id);
    NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * id + 1);
    NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * id + 2);
    NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * id + 3);
    #endif
    i2c->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_DRDY | SERCOM_I2CS_INTENSET_AMATCH |
        SERCOM_I2CS_INTENSET_PREC | SERCOM_I2CS_INTENSET_ERROR;

    // Now enable I2C.
    sercom_enable(i2c, 1);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_i2c_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, scl=\"%q\", sda=\"%q\", addr=%u)",
        self->id, pin_find_by_id(self->scl)->name, pin_find_by_id(self->sda)->name,
        self->addr);
}

// Stop the Slave transfer and free the memory objects.
static void mp_machine_i2c_target_deinit(machine_i2c_target_obj_t *self) {
    // Disable I2C
    sercom_enable(self->instance, 0);
    MP_STATE_PORT(sercom_table[self->id]) = NULL;
}
