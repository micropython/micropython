/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2026 Infineon Technologies AG
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

// std includes
#include <stdio.h>
#include <string.h>

// mpy includes
#include "extmod/modmachine.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"

// MTB includes
#include "cybsp.h"
#include "cy_scb_i2c.h"

// port-specific includes
#include "clk.h"
#include "genhdr/pins_af.h"
#include "modmachine.h"
#include "i2c.h"

#define DEBUG_printf(...) // printf(__VA_ARGS__)

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
    uint32_t freq;
    uint32_t timeout;
    i2c_obj_t i2c_obj;
} machine_i2c_obj_t;

MP_REGISTER_ROOT_POINTER(struct _machine_i2c_obj_t *machine_i2c_obj[MICROPY_PY_MACHINE_I2C_NUM_ENTRIES]);

static inline machine_i2c_obj_t *machine_i2c_obj_get(uint8_t id) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_I2C_NUM_ENTRIES; i++)
    {
        if (MP_STATE_PORT(machine_i2c_obj[i]) != NULL
            && i2c_get_id(&MP_STATE_PORT(machine_i2c_obj[i])->i2c_obj) == id) {
            return MP_STATE_PORT(machine_i2c_obj[i]);
        }
    }
    return NULL;
}

static inline machine_i2c_obj_t *machine_i2c_obj_alloc(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_I2C_NUM_ENTRIES; i++)
    {
        if (MP_STATE_PORT(machine_i2c_obj[i]) == NULL) {
            MP_STATE_PORT(machine_i2c_obj[i]) = mp_obj_malloc(machine_i2c_obj_t, &machine_i2c_type);
            memset(&MP_STATE_PORT(machine_i2c_obj[i])->i2c_obj, 0, sizeof(i2c_obj_t));
            return MP_STATE_PORT(machine_i2c_obj[i]);
        }
    }
    return NULL;
}

static inline void machine_i2c_obj_free(machine_i2c_obj_t *i2c_obj_ptr) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_I2C_NUM_ENTRIES; i++)
    {
        if (MP_STATE_PORT(machine_i2c_obj[i]) == i2c_obj_ptr) {
            MP_STATE_PORT(machine_i2c_obj[i]) = NULL;
        }
    }
}

static void machine_i2c_scb_isr(mp_obj_t hw_i2c_obj) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(hw_i2c_obj);
    i2c_hw_controller_irq(&self->i2c_obj);
}

#define DEFAULT_I2C_FREQ     (400000)

enum { ARG_scl, ARG_sda, ARG_freq, ARG_timeout };

static const mp_arg_t allowed_args[] = {
    { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_I2C_FREQ} },
    { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 50000} }  // Default 50000us (50ms)
};

static void machine_i2c_init_impl(machine_i2c_obj_t **self_ptr, int i2c_id, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    /* -- SDA / RX pins -- */
    mp_hal_pin_af_config_t i2c_pins_af_config[] = {
        /* SCL */ MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_OD_DRIVESLOW, 1, MACHINE_PIN_AF_SIGNAL_I2C_SCL),
        /* SDA */ MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_OD_DRIVESLOW, 1, MACHINE_PIN_AF_SIGNAL_I2C_SDA),
    };

    /* -- Resolve ID - pin match and not provided AF pins -- */
    const mp_obj_t i2c_pins[] = { args[ARG_scl].u_obj, args[ARG_sda].u_obj };
    machine_pin_af_unit_t fn_unit = mp_hal_periph_pins_af_resolve(i2c_pins_af_config, I2C_NUM_PINS, MACHINE_PIN_AF_FN_I2C, (machine_pin_af_unit_t)i2c_id, i2c_pins);

    /* -- Frequency -- */
    if (args[ARG_freq].u_int != 100000 && args[ARG_freq].u_int != 400000) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq must be 100000 or 400000"));
    }
    uint32_t freq_hz = (uint32_t)args[ARG_freq].u_int;

    /* -- Timeout -- */
    if (args[ARG_timeout].u_int <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("machine.I2C: timeout must be > 0"));
    }
    uint32_t timeout = (uint32_t)args[ARG_timeout].u_int;

    machine_i2c_obj_t *self = *self_ptr;

    /* -- Object allocation -- */
    bool is_new = false;
    bool is_make_obj_required = (*self_ptr == NULL);
    if (is_make_obj_required) {
        *self_ptr = machine_i2c_obj_get(fn_unit);
        if (*self_ptr == NULL) {
            *self_ptr = machine_i2c_obj_alloc();
            if (*self_ptr == NULL) {
                mp_raise_ValueError(MP_ERROR_TEXT("failed to allocate I2C object"));
            }
            is_new = true;
        }
        self = *self_ptr;
    }

    /* -- Reinitialization reset -- */
    /* For reused I2C() object or init() path */
    if (!is_new) {
        i2c_hw_deinit(&self->i2c_obj);
    }

    /* -- I2C params init -- */
    self->scl = i2c_pins_af_config[0].pin;
    self->sda = i2c_pins_af_config[1].pin;
    self->freq = freq_hz;
    self->timeout = timeout;

    /* -- Initialise hardware -- */
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_hal_periph_pins_af_init(i2c_pins_af_config, I2C_NUM_PINS);
        bool is_new;
        i2c_alloc(&self->i2c_obj, fn_unit, self, machine_i2c_scb_isr, &is_new);
        if (!is_new) {
            i2c_hw_deinit(&self->i2c_obj);
        }
        i2c_hw_controller_init(&self->i2c_obj, self->freq);
        nlr_pop();
    } else {
        // Ensure partially-initialized instances are fully released on init failure.
        i2c_hw_deinit(&self->i2c_obj);
        nlr_raise(nlr.ret_val);
    }
}

static void machine_i2c_deinit(mp_obj_base_t *self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    i2c_hw_deinit(&self->i2c_obj);
    i2c_free(&self->i2c_obj);
    machine_i2c_obj_free(self);
}

static int machine_i2c_transfer(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cy_rslt_t result;

    DEBUG_printf("I2C Transfer: addr=0x%02X, len=%u, flags=0x%02X (%s)\n",
        addr, len, flags, (flags & MP_MACHINE_I2C_FLAG_READ) ? "READ" : "WRITE");

    cy_stc_scb_i2c_master_xfer_config_t transfer;
    transfer.slaveAddress = addr;
    transfer.buffer = buf;
    transfer.bufferSize = len;
    // Generate Stop condition if MP_MACHINE_I2C_FLAG_STOP is set
    transfer.xferPending = !(flags & MP_MACHINE_I2C_FLAG_STOP);

    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        result = i2c_hw_controller_read(&self->i2c_obj, &transfer);
    } else {
        result = i2c_hw_controller_write(&self->i2c_obj, &transfer);
    }

    if (result != CY_RSLT_SUCCESS) {
        DEBUG_printf("I2C Transfer start failed: 0x%lx\n", result);
        return -MP_EIO;  // I/O error
    }

    DEBUG_printf("I2C Transfer started, waiting for completion...\n");

    uint32_t start_time = mp_hal_ticks_us();
    uint32_t timeout_end = start_time + self->timeout;  // Both in microseconds

    while (0UL != (CY_SCB_I2C_MASTER_BUSY & i2c_hw_controller_get_status(&self->i2c_obj))) {
        // Yield to allow other tasks/interrupts to run
        mp_event_handle_nowait();

        // Check for timeout using actual elapsed time
        if (mp_hal_ticks_us() >= timeout_end) {
            DEBUG_printf("I2C Transfer timeout after %u us!\n", self->timeout);
            return -MP_ETIMEDOUT;
        }
    }

    uint32_t master_status = i2c_hw_controller_get_status(&self->i2c_obj);

    DEBUG_printf("I2C Transfer complete, status=0x%08lX\n", master_status);

    if (master_status & CY_SCB_I2C_MASTER_ERR) {
        DEBUG_printf("I2C Transfer error detected in status\n");
        return -MP_EIO;  // I/O error
    }

    return len;
}

void machine_i2c_deinit_all(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_I2C_NUM_ENTRIES; i++)
    {
        if (MP_STATE_PORT(machine_i2c_obj[i]) != NULL) {
            machine_i2c_deinit((mp_obj_base_t *)MP_STATE_PORT(machine_i2c_obj[i]));
        }
    }
}

/******************************************************************************/
// MicroPython bindings for machine API

static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "I2C(id=%u, scl='%q', sda='%q', freq=%u, timeout=%u)",
        i2c_get_id(&self->i2c_obj),
        self->scl->name,
        self->sda->name,
        self->freq,
        self->timeout);
}

static mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    /**
     * Only the constructor takes the id.
     * Its validation together with the rest of the arguments is
     * delegated to machine_i2c_init_impl() which also allocates
     * the object self = NULL.
    */
    int i2c_id = MACHINE_PIN_AF_UNIT_NONE;
    size_t init_n_args = n_args;
    const mp_obj_t *init_args = args;
    if (n_args > 0 && mp_obj_is_int(args[0])) {
        i2c_id = mp_obj_get_int(args[0]);
        if (i2c_id < 0 || i2c_id >= MICROPY_PY_SCB_NUM_ENTRIES) {
            mp_raise_ValueError(MP_ERROR_TEXT("I2C id out of range"));
        }
        init_n_args = n_args - 1;
        init_args = args + 1;
    } else if (n_args > 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("I2C id must be an integer"));
    }

    machine_i2c_obj_t *self = NULL;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        machine_i2c_init_impl(&self, i2c_id, init_n_args, init_args, &kw_args);
        nlr_pop();
    } else {
        if (self != NULL) {
            machine_i2c_deinit(MP_OBJ_FROM_PTR(self));
        }
        nlr_raise(nlr.ret_val);
    }

    return MP_OBJ_FROM_PTR(self);
}

static void machine_i2c_init(mp_obj_base_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_i2c_obj_t *self_in = MP_OBJ_TO_PTR(self);
    machine_i2c_init_impl(&self_in, i2c_get_id(&self_in->i2c_obj), n_args, pos_args, kw_args);
}

static const mp_machine_i2c_p_t machine_i2c_p = {
    .init = machine_i2c_init,
    .deinit = machine_i2c_deinit,
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_i2c_transfer,
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
