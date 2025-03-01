/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022 Robert Hammelrath
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

#if MICROPY_PY_MACHINE_I2C

#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"
#include "samd_soc.h"
#include "pin_af.h"
#include "genhdr/pins.h"
#include "clock_config.h"

#define DEFAULT_I2C_FREQ  (400000)
#define RISETIME_NS       (200)
#define I2C_TIMEOUT       (100)

#define IS_BUS_BUSY       (i2c->I2CM.STATUS.bit.BUSSTATE == 3)
#define NACK_RECVD        (i2c->I2CM.STATUS.bit.RXNACK == 1)
#define IRQ_DATA_SENT     (i2c->I2CM.INTFLAG.bit.MB == 1)
#define IRQ_DATA_RECVD    (i2c->I2CM.INTFLAG.bit.SB == 1)
#define READ_MODE         ((flags & MP_MACHINE_I2C_FLAG_READ) != 0)

#define PREPARE_ACK       i2c->I2CM.CTRLB.bit.ACKACT = 0
#define PREPARE_NACK      i2c->I2CM.CTRLB.bit.ACKACT = 1
#define SET_STOP_STATE    i2c_send_command(i2c, 0x03)

enum state_t {
    state_done = 0,
    state_busy,
    state_buserr,
    state_nack
};

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    Sercom *instance;
    uint8_t id;
    uint8_t scl;
    uint8_t sda;
    uint8_t state;
    uint32_t freq;
    uint32_t timeout;
    size_t len;
    uint8_t *buf;
} machine_i2c_obj_t;

static void i2c_send_command(Sercom *i2c, uint8_t command) {
    i2c->I2CM.CTRLB.bit.CMD = command;
    while (i2c->I2CM.SYNCBUSY.bit.SYSOP) {
    }
}

void common_i2c_irq_handler(int i2c_id) {
    // handle Sercom I2C IRQ
    machine_i2c_obj_t *self = MP_STATE_PORT(sercom_table[i2c_id]);
    // Handle IRQ
    if (self != NULL) {
        Sercom *i2c = self->instance;
        // For now, clear all interrupts
        if (IRQ_DATA_RECVD) {
            if (self->len > 0) {
                *(self->buf)++ = i2c->I2CM.DATA.reg;
                self->len--;
                self->timeout = I2C_TIMEOUT;
            }
            if (self->len > 0) { // no ACK at the last byte
                PREPARE_ACK; // Send ACK
                i2c_send_command(i2c, 0x02);
            } else {
                PREPARE_NACK; // Send NACK after the last byte
                self->state = state_done;
                i2c->I2CM.INTFLAG.reg |= SERCOM_I2CM_INTFLAG_SB;
            }
        } else if (IRQ_DATA_SENT) {
            if (NACK_RECVD) { // e.g. NACK after address for both read and write.
                self->state = state_nack; // force stop of transmission
                i2c->I2CM.INTFLAG.reg |= SERCOM_I2CM_INTFLAG_MB;
            } else if (self->len > 0) { // data to be sent
                i2c->I2CM.DATA.bit.DATA = *(self->buf)++;
                self->len--;
                self->timeout = I2C_TIMEOUT;
            } else { // No data left, if there was any.
                self->state = state_done;
                i2c->I2CM.INTFLAG.reg |= SERCOM_I2CM_INTFLAG_MB;
            }
        } else { // On any error, e.g. ARBLOST or BUSERROR, stop the transmission
            self->len = 0;
            self->state = state_buserr;
            i2c->I2CM.INTFLAG.reg |= SERCOM_I2CM_INTFLAG_ERROR;
        }
    }
}

static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, freq=%u, scl=\"%q\", sda=\"%q\")",
        self->id, self->freq, pin_find_by_id(self->scl)->name, pin_find_by_id(self->sda)->name);
}

mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_freq, ARG_scl, ARG_sda };
    static const mp_arg_t allowed_args[] = {
        #if MICROPY_HW_DEFAULT_I2C_ID < 0
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
        #else
        { MP_QSTR_id, MP_ARG_INT, {.u_int = MICROPY_HW_DEFAULT_I2C_ID} },
        #endif
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = DEFAULT_I2C_FREQ} },
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
    machine_i2c_obj_t *self = mp_obj_malloc(machine_i2c_obj_t, &machine_i2c_type);
    self->id = id;
    self->instance = sercom_instance[self->id];

    // Set SCL/SDA pins.
    self->scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
    self->sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj);

    sercom_pad_config_t scl_pad_config = get_sercom_config(self->scl, self->id);
    sercom_pad_config_t sda_pad_config = get_sercom_config(self->sda, self->id);
    if (sda_pad_config.pad_nr != 0 || scl_pad_config.pad_nr != 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid sda/scl pin"));
    }
    MP_STATE_PORT(sercom_table[self->id]) = self;
    self->freq = args[ARG_freq].u_int;

    // Configure the Pin mux.
    mp_hal_set_pin_mux(self->scl, scl_pad_config.alt_fct);
    mp_hal_set_pin_mux(self->sda, sda_pad_config.alt_fct);

    // Set up the clocks
    enable_sercom_clock(self->id);

    // Initialise the I2C peripheral
    Sercom *i2c = self->instance;

    // Reset the device
    i2c->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SWRST;
    while (i2c->I2CM.SYNCBUSY.bit.SWRST == 1) {
    }
    // Set to master mode, inactivity timeout of 20 SCL cycles and speed.
    i2c->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_MODE(0x05)
        | SERCOM_I2CM_CTRLA_INACTOUT(3)
        | SERCOM_I2CM_CTRLA_SPEED(self->freq > 400000 ? 1 : 0);

    // I2C is driven by the clock of GCLK Generator 2, with it's freq in variable bus_freq
    // baud = peripheral_freq / (2 * baudrate) - 5 - (rise_time * peripheral_freq) / 2
    // Just set the minimal configuration for standard and fast mode.
    // Set Baud. Assume ~300ns rise time. Maybe set later by a keyword argument.
    int32_t baud = get_peripheral_freq() / (2 * self->freq) - 5 - (get_peripheral_freq() / 1000000) * RISETIME_NS / 2000;
    if (baud < 0) {
        baud = 0;
    }
    if (baud > 255) {
        baud = 255;
    }
    i2c->I2CM.BAUD.reg = baud;

    // Enable interrupts
    sercom_register_irq(self->id, &common_i2c_irq_handler);
    #if defined(MCU_SAMD21)
    NVIC_EnableIRQ(SERCOM0_IRQn + self->id);
    #elif defined(MCU_SAMD51)
    NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * self->id); // MB interrupt
    NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * self->id + 1); // SB interrupt
    NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * self->id + 3); // ERROR interrupt
    #endif

    // Now enable I2C.
    sercom_enable(i2c, 1);

    // Force the bus state to idle
    i2c->I2CM.STATUS.bit.BUSSTATE = 1;

    return MP_OBJ_FROM_PTR(self);
}

static int machine_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)self_in;
    Sercom *i2c = self->instance;

    self->timeout = I2C_TIMEOUT;
    self->len = len;
    self->buf = buf;
    // Wait a while if the bus is busy
    while (IS_BUS_BUSY && self->timeout) {
        MICROPY_EVENT_POLL_HOOK
        if (--self->timeout == 0) {
            return -MP_ETIMEDOUT;
        }
    }
    // Enable interrupts and set the state
    i2c->I2CM.INTENSET.reg = SERCOM_I2CM_INTENSET_MB | SERCOM_I2CM_INTENSET_SB | SERCOM_I2CM_INTENSET_ERROR;
    self->state = state_busy;

    // Send the address, which kicks off the transfer
    i2c->I2CM.ADDR.bit.ADDR = (addr << 1) | READ_MODE;

    // Transfer the data
    self->timeout = I2C_TIMEOUT;
    while (self->state == state_busy && self->timeout) {
        self->timeout--;
        MICROPY_EVENT_POLL_HOOK
    }
    i2c->I2CM.INTENCLR.reg = SERCOM_I2CM_INTENSET_MB | SERCOM_I2CM_INTENSET_SB | SERCOM_I2CM_INTENSET_ERROR;

    // Check the error states after the transfer is stopped
    if (self->state == state_nack) {
        SET_STOP_STATE;
        return self->len == len ? -MP_ENODEV : -MP_EIO;
    } else if (self->state == state_buserr) {
        SET_STOP_STATE;
        return -MP_EIO;
    } else if (self->timeout == 0) {
        SET_STOP_STATE;
        return -MP_ETIMEDOUT;
    }

    if (flags & MP_MACHINE_I2C_FLAG_STOP) {
        SET_STOP_STATE;
    }

    return len;
}

static const mp_machine_i2c_p_t machine_i2c_p = {
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_i2c_transfer_single,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    print, machine_i2c_print,
    protocol, &machine_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );

#endif
