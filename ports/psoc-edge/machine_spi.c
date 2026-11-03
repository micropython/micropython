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

#include "py/runtime.h"
#include "extmod/modmachine.h"
#include <string.h>
#include "py/mphal.h"
#include "py/mperrno.h"
#include "cybsp.h"
#include "genhdr/pins_af.h"
#include "modmachine.h"
#include "spi.h"

#define DEFAULT_SPI_BAUDRATE    (1000000)
#define DEFAULT_SPI_POLARITY    (0)
#define DEFAULT_SPI_PHASE       (0)
#define DEFAULT_SPI_BITS        (SPI_SUPPORTED_BITS)
#define DEFAULT_SPI_TIMEOUT     (50000)

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t mosi;
    mp_hal_pin_obj_t miso;
    uint32_t baudrate;
    uint32_t timeout;
    spi_obj_t spi_obj;
} machine_spi_obj_t;

MP_REGISTER_ROOT_POINTER(struct _machine_spi_obj_t *machine_spi_obj[MICROPY_PY_MACHINE_SPI_NUM_ENTRIES]);

static inline machine_spi_obj_t *machine_spi_obj_get(machine_pin_af_unit_t id) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_spi_obj[i]) != NULL
            && spi_get_id(&MP_STATE_PORT(machine_spi_obj[i])->spi_obj) == id) {
            return MP_STATE_PORT(machine_spi_obj[i]);
        }
    }
    return NULL;
}

static inline machine_spi_obj_t *machine_spi_obj_alloc(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_spi_obj[i]) == NULL) {
            machine_spi_obj_t *self = mp_obj_malloc(machine_spi_obj_t, &machine_spi_type);
            self->sck = NULL;
            self->mosi = NULL;
            self->miso = NULL;
            self->timeout = DEFAULT_SPI_TIMEOUT;
            memset(&self->spi_obj, 0, sizeof(spi_obj_t));
            self->spi_obj.id = MACHINE_PIN_AF_UNIT_NONE;
            MP_STATE_PORT(machine_spi_obj[i]) = self;
            return self;
        }
    }
    return NULL;
}

static inline void machine_spi_obj_free(machine_spi_obj_t *spi_obj) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_spi_obj[i]) == spi_obj) {
            MP_STATE_PORT(machine_spi_obj[i]) = NULL;
        }
    }
}

static void machine_spi_scb_isr(mp_obj_t hw_spi_obj) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(hw_spi_obj);
    spi_hw_irq(&self->spi_obj);
}

enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };

static const mp_arg_t allowed_args[] = {
    { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = DEFAULT_SPI_BAUDRATE} },
    { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_POLARITY} },
    { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_PHASE} },
    { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_BITS} },
    { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
    { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
};

#define SPI_NUM_PINS (3)

// Resolves the id and the pin AF config from the pins given (None if not given).
static machine_pin_af_unit_t machine_spi_pins_resolve(mp_hal_pin_af_config_t *spi_pins_af_config, machine_pin_af_unit_t spi_id, const mp_obj_t *pins) {
    /* -- Pins (CS/SSEL is managed by the user via machine.Pin) -- */
    spi_pins_af_config[0] = MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_STRONG_IN_OFF, 1, MACHINE_PIN_AF_SIGNAL_SPI_CLK);
    spi_pins_af_config[1] = MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_STRONG_IN_OFF, 1, MACHINE_PIN_AF_SIGNAL_SPI_MOSI);
    spi_pins_af_config[2] = MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_HIGHZ, 0, MACHINE_PIN_AF_SIGNAL_SPI_MISO);
    return mp_hal_periph_pins_af_resolve(spi_pins_af_config, SPI_NUM_PINS, MACHINE_PIN_AF_FN_SPI, spi_id, pins);
}

// (Re)configures the object. Arguments not given are reset to their defaults.
static void machine_spi_init_impl(machine_spi_obj_t *self, machine_pin_af_unit_t spi_id, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t pins[SPI_NUM_PINS] = { args[ARG_sck].u_obj, args[ARG_mosi].u_obj, args[ARG_miso].u_obj };
    mp_hal_pin_af_config_t spi_pins_af_config[SPI_NUM_PINS];
    machine_pin_af_unit_t fn_unit = machine_spi_pins_resolve(spi_pins_af_config, spi_id, pins);

    /* -- Validate the remaining parameters before touching the hardware -- */
    if (args[ARG_baudrate].u_int <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("baudrate must be > 0"));
    }
    if ((args[ARG_polarity].u_int & ~1) != 0 || (args[ARG_phase].u_int & ~1) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("polarity/phase must be 0 or 1"));
    }
    if (args[ARG_bits].u_int != SPI_SUPPORTED_BITS) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits must be 8"));
    }
    if ((args[ARG_firstbit].u_int != MICROPY_PY_MACHINE_SPI_MSB) &&
        (args[ARG_firstbit].u_int != MICROPY_PY_MACHINE_SPI_LSB)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid firstbit"));
    }

    /* -- SPI params init -- */
    self->sck = spi_pins_af_config[0].pin;
    self->mosi = spi_pins_af_config[1].pin;
    self->miso = spi_pins_af_config[2].pin;
    self->baudrate = (uint32_t)args[ARG_baudrate].u_int;
    self->spi_obj.polarity = (uint8_t)args[ARG_polarity].u_int;
    self->spi_obj.phase = (uint8_t)args[ARG_phase].u_int;
    self->spi_obj.bits = (uint8_t)args[ARG_bits].u_int;
    self->spi_obj.firstbit = (uint8_t)args[ARG_firstbit].u_int;

    /* -- Initialise hardware -- */
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        bool is_new;
        spi_alloc(&self->spi_obj, fn_unit, MP_OBJ_FROM_PTR(self), machine_spi_scb_isr, &is_new);
        if (!is_new) {
            spi_hw_deinit(&self->spi_obj);
        }
        mp_hal_periph_pins_af_init(spi_pins_af_config, SPI_NUM_PINS);
        spi_hw_controller_init(&self->spi_obj, self->baudrate);
        nlr_pop();
    } else {
        // Leave the SCB claimed so that init() can be retried on this object.
        spi_hw_deinit(&self->spi_obj);
        nlr_raise(nlr.ret_val);
    }
}

// SPI.init(): reconfigures the object, keeping the id set by the constructor.
static void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_spi_init_impl(self, spi_get_id(&self->spi_obj), n_args, pos_args, kw_args);
}

static void machine_spi_deinit(mp_obj_base_t *self_in) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    spi_hw_deinit(&self->spi_obj);
    spi_free(&self->spi_obj);
    machine_spi_obj_free(self);
}

static void machine_spi_transfer(mp_obj_base_t *self_in,
    size_t len,
    const uint8_t *src,
    uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;

    if (len == 0) {
        return;
    }

    // Prepare TX buffer: if src is NULL (read-only), send 0xFF fill bytes
    uint8_t tx_fill[len];
    const uint8_t *tx_buf = src;
    if (tx_buf == NULL) {
        memset(tx_fill, 0xFF, len);
        tx_buf = tx_fill;
    }

    // Prepare RX buffer: if dest is NULL (write-only), use a dummy buffer
    uint8_t rx_dummy[len];
    uint8_t *rx_buf = dest;
    if (rx_buf == NULL) {
        rx_buf = rx_dummy;
    }

    // Start full-duplex transfer (interrupt-driven via ISR)
    cy_en_scb_spi_status_t status = spi_hw_controller_transfer(&self->spi_obj, tx_buf, rx_buf, len);

    if (status != CY_SCB_SPI_SUCCESS) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("SPI transfer failed: 0x%lx"), (uint32_t)status);
    }

    // Wait for transfer to complete
    uint32_t start = mp_hal_ticks_us();
    while (spi_hw_controller_is_transfer_active(&self->spi_obj)) {
        if (mp_hal_ticks_us() - start > self->timeout) {
            spi_hw_controller_abort_transfer(&self->spi_obj);
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPI transfer timeout"));
        }
        mp_event_handle_nowait();
    }
}

static const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .deinit = machine_spi_deinit,
    .transfer = machine_spi_transfer,
};

static void machine_spi_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(id=%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%s, sck='%q', mosi='%q', miso='%q')",
        spi_get_id(&self->spi_obj), self->baudrate, self->spi_obj.polarity, self->spi_obj.phase, self->spi_obj.bits,
        self->spi_obj.firstbit == MICROPY_PY_MACHINE_SPI_MSB ? "MSB" : "LSB",
        self->sck->name, self->mosi->name, self->miso->name);
}

// SPI([id], ...): the constructor sets the identity, everything else is delegated to init.
mp_obj_t mp_machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    machine_pin_af_unit_t spi_id = MACHINE_PIN_AF_UNIT_NONE;
    size_t init_n_args = n_args;
    const mp_obj_t *init_args = all_args;
    if (n_args > 0 && mp_obj_is_int(all_args[0])) {
        mp_int_t id = mp_obj_get_int(all_args[0]);
        if (id < 0 || id >= MICROPY_PY_SCB_NUM_ENTRIES) {
            mp_raise_ValueError(MP_ERROR_TEXT("SPI id out of range"));
        }
        spi_id = (machine_pin_af_unit_t)id;
        init_n_args = n_args - 1;
        init_args = all_args + 1;
    } else if (n_args > 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("SPI id must be an integer"));
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);

    // No id given: resolve it from the pins, so that a live object for that id is reused.
    if (spi_id == MACHINE_PIN_AF_UNIT_NONE) {
        const qstr pin_names[SPI_NUM_PINS] = { MP_QSTR_sck, MP_QSTR_mosi, MP_QSTR_miso };
        mp_obj_t pins[SPI_NUM_PINS];
        mp_hal_periph_pins_get_from_kw(&kw_args, pin_names, SPI_NUM_PINS, pins);
        mp_hal_pin_af_config_t spi_pins_af_config[SPI_NUM_PINS];
        spi_id = machine_spi_pins_resolve(spi_pins_af_config, spi_id, pins);
    }

    machine_spi_obj_t *self = machine_spi_obj_get(spi_id);
    bool is_new_obj = (self == NULL);
    if (is_new_obj) {
        self = machine_spi_obj_alloc();
        if (self == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("failed to allocate SPI object"));
        }
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        machine_spi_init_impl(self, spi_id, init_n_args, init_args, &kw_args);
        nlr_pop();
    } else {
        // Only release an object created here. A live object is kept, as on a failed init().
        if (is_new_obj) {
            machine_spi_deinit(&self->base);
        }
        nlr_raise(nlr.ret_val);
    }

    return MP_OBJ_FROM_PTR(self);
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, mp_machine_spi_make_new,
    print, machine_spi_print,
    protocol, &machine_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );

void machine_spi_deinit_all(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_spi_obj[i]) != NULL) {
            machine_spi_deinit((mp_obj_base_t *)MP_STATE_PORT(machine_spi_obj[i]));
        }
    }
}
