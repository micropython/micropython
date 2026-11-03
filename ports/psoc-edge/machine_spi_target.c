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
#include "py/mphal.h"
#include "py/mperrno.h"
#include <string.h>
#include "cybsp.h"
#include "genhdr/pins_af.h"
#include "modmachine.h"
#include "spi.h"

#if MICROPY_PY_MACHINE_SPI_TARGET

#define DEFAULT_SPI_TARGET_POLARITY    (0)
#define DEFAULT_SPI_TARGET_PHASE       (0)
#define DEFAULT_SPI_TARGET_BITS        (SPI_SUPPORTED_BITS)
#define SPI_TARGET_TICKS_WRAP_MS       (15000U) // from mp_hal_ticks_ms source timer period in modtime.c
#define DEFAULT_SPI_TARGET_TIMEOUT_MS  (5000U)  // must stay below SPI_TARGET_TICKS_WRAP_MS

// mp_hal_ticks_ms() on this port wraps every SPI_TARGET_TICKS_WRAP_MS.
// This computes elapsed time across one wrap; caller keeps timeout < wrap period.
static inline uint32_t spi_target_elapsed_ms(uint32_t start_ms) {
    uint32_t now = mp_hal_ticks_ms();
    return (now >= start_ms) ? (now - start_ms) : (SPI_TARGET_TICKS_WRAP_MS - start_ms + now);
}

typedef struct _machine_spi_target_obj_t {
    mp_obj_base_t base;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t mosi;
    mp_hal_pin_obj_t miso;
    mp_hal_pin_obj_t ssel;
    uint32_t timeout;
    spi_obj_t spi_obj;
} machine_spi_target_obj_t;

MP_REGISTER_ROOT_POINTER(struct _machine_spi_target_obj_t *machine_spi_target_obj[MICROPY_PY_MACHINE_SPI_TARGET_MAX]);

static inline machine_spi_target_obj_t *machine_spi_target_obj_get(machine_pin_af_unit_t id) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_TARGET_MAX; i++) {
        if (MP_STATE_PORT(machine_spi_target_obj[i]) != NULL
            && spi_get_id(&MP_STATE_PORT(machine_spi_target_obj[i])->spi_obj) == id) {
            return MP_STATE_PORT(machine_spi_target_obj[i]);
        }
    }
    return NULL;
}

static inline machine_spi_target_obj_t *machine_spi_target_obj_alloc(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_TARGET_MAX; i++) {
        if (MP_STATE_PORT(machine_spi_target_obj[i]) == NULL) {
            machine_spi_target_obj_t *self = mp_obj_malloc(machine_spi_target_obj_t, &machine_spi_target_type);
            self->sck = NULL;
            self->mosi = NULL;
            self->miso = NULL;
            self->ssel = NULL;
            self->timeout = DEFAULT_SPI_TARGET_TIMEOUT_MS;
            memset(&self->spi_obj, 0, sizeof(spi_obj_t));
            self->spi_obj.id = MACHINE_PIN_AF_UNIT_NONE;
            MP_STATE_PORT(machine_spi_target_obj[i]) = self;
            return self;
        }
    }
    return NULL;
}

static inline void machine_spi_target_obj_free(machine_spi_target_obj_t *obj) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_TARGET_MAX; i++) {
        if (MP_STATE_PORT(machine_spi_target_obj[i]) == obj) {
            MP_STATE_PORT(machine_spi_target_obj[i]) = NULL;
        }
    }
}

static void machine_spi_target_scb_isr(mp_obj_t obj) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(obj);
    spi_hw_irq(&self->spi_obj);
}

enum { ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso, ARG_ssel };

static const mp_arg_t allowed_args[] = {
    { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_TARGET_POLARITY} },
    { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_TARGET_PHASE} },
    { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_TARGET_BITS} },
    { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
    { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_ssel,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
};

#define SPI_TARGET_NUM_PINS (4)

// Resolves the id and the pin AF config from the pins given (None if not given).
static machine_pin_af_unit_t machine_spi_target_pins_resolve(mp_hal_pin_af_config_t *spi_pins_af_config, machine_pin_af_unit_t spi_id, const mp_obj_t *pins) {
    /* -- Pins: SCK, MOSI and SSEL are inputs, MISO is an output -- */
    spi_pins_af_config[0] = MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_HIGHZ, 0, MACHINE_PIN_AF_SIGNAL_SPI_CLK);
    spi_pins_af_config[1] = MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_HIGHZ, 0, MACHINE_PIN_AF_SIGNAL_SPI_MOSI);
    spi_pins_af_config[2] = MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_STRONG_IN_OFF, 1, MACHINE_PIN_AF_SIGNAL_SPI_MISO);
    spi_pins_af_config[3] = MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_HIGHZ, 0, MACHINE_PIN_AF_SIGNAL_SPI_SELECT0);
    return mp_hal_periph_pins_af_resolve(spi_pins_af_config, SPI_TARGET_NUM_PINS, MACHINE_PIN_AF_FN_SPI, spi_id, pins);
}

// (Re)configures the object. Arguments not given are reset to their defaults.
static void machine_spi_target_init_impl(machine_spi_target_obj_t *self, machine_pin_af_unit_t spi_id, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t pins[SPI_TARGET_NUM_PINS] = { args[ARG_sck].u_obj, args[ARG_mosi].u_obj, args[ARG_miso].u_obj, args[ARG_ssel].u_obj };
    mp_hal_pin_af_config_t spi_pins_af_config[SPI_TARGET_NUM_PINS];
    machine_pin_af_unit_t fn_unit = machine_spi_target_pins_resolve(spi_pins_af_config, spi_id, pins);

    /* -- Validate the remaining parameters before touching the hardware -- */
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

    /* -- SPITarget params init -- */
    self->sck = spi_pins_af_config[0].pin;
    self->mosi = spi_pins_af_config[1].pin;
    self->miso = spi_pins_af_config[2].pin;
    self->ssel = spi_pins_af_config[3].pin;
    self->spi_obj.polarity = (uint8_t)args[ARG_polarity].u_int;
    self->spi_obj.phase = (uint8_t)args[ARG_phase].u_int;
    self->spi_obj.bits = (uint8_t)args[ARG_bits].u_int;
    self->spi_obj.firstbit = (uint8_t)args[ARG_firstbit].u_int;

    /* -- Initialise hardware -- */
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        bool is_new;
        spi_alloc(&self->spi_obj, fn_unit, MP_OBJ_FROM_PTR(self), machine_spi_target_scb_isr, &is_new);
        if (!is_new) {
            spi_hw_deinit(&self->spi_obj);
        }
        mp_hal_periph_pins_af_init(spi_pins_af_config, SPI_TARGET_NUM_PINS);
        spi_hw_target_init(&self->spi_obj);
        nlr_pop();
    } else {
        // Leave the SCB claimed so that init() can be retried on this object.
        spi_hw_deinit(&self->spi_obj);
        nlr_raise(nlr.ret_val);
    }
}

/******************************************************************************/
// MicroPython bindings

static void machine_spi_target_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPITarget(id=%u, sck='%q', mosi='%q', miso='%q', ssel='%q', polarity=%u, phase=%u, bits=%u, firstbit=%s)",
        spi_get_id(&self->spi_obj), self->sck->name, self->mosi->name, self->miso->name, self->ssel->name,
        self->spi_obj.polarity, self->spi_obj.phase, self->spi_obj.bits,
        self->spi_obj.firstbit == MICROPY_PY_MACHINE_SPI_MSB ? "MSB" : "LSB");
}

static mp_obj_t machine_spi_target_deinit(mp_obj_t self_in) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    spi_hw_deinit(&self->spi_obj);
    spi_free(&self->spi_obj);
    machine_spi_target_obj_free(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_spi_target_deinit_obj, machine_spi_target_deinit);

// SPITarget([id], ...): the constructor sets the identity, everything else is delegated to init.
static mp_obj_t machine_spi_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    machine_pin_af_unit_t spi_id = MACHINE_PIN_AF_UNIT_NONE;
    size_t init_n_args = n_args;
    const mp_obj_t *init_args = all_args;
    if (n_args > 0 && mp_obj_is_int(all_args[0])) {
        mp_int_t id = mp_obj_get_int(all_args[0]);
        if (id < 0 || id >= MICROPY_PY_SCB_NUM_ENTRIES) {
            mp_raise_ValueError(MP_ERROR_TEXT("SPITarget id out of range"));
        }
        spi_id = (machine_pin_af_unit_t)id;
        init_n_args = n_args - 1;
        init_args = all_args + 1;
    } else if (n_args > 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("SPITarget id must be an integer"));
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);

    // No id given: resolve it from the pins, so that a live object for that id is reused.
    if (spi_id == MACHINE_PIN_AF_UNIT_NONE) {
        const qstr pin_names[SPI_TARGET_NUM_PINS] = { MP_QSTR_sck, MP_QSTR_mosi, MP_QSTR_miso, MP_QSTR_ssel };
        mp_obj_t pins[SPI_TARGET_NUM_PINS];
        mp_hal_periph_pins_get_from_kw(&kw_args, pin_names, SPI_TARGET_NUM_PINS, pins);
        mp_hal_pin_af_config_t spi_pins_af_config[SPI_TARGET_NUM_PINS];
        spi_id = machine_spi_target_pins_resolve(spi_pins_af_config, spi_id, pins);
    }

    machine_spi_target_obj_t *self = machine_spi_target_obj_get(spi_id);
    bool is_new_obj = (self == NULL);
    if (is_new_obj) {
        self = machine_spi_target_obj_alloc();
        if (self == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("failed to allocate SPITarget object"));
        }
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        machine_spi_target_init_impl(self, spi_id, init_n_args, init_args, &kw_args);
        nlr_pop();
    } else {
        // Only release an object created here. A live object is kept, as on a failed init().
        if (is_new_obj) {
            machine_spi_target_deinit(MP_OBJ_FROM_PTR(self));
        }
        nlr_raise(nlr.ret_val);
    }

    return MP_OBJ_FROM_PTR(self);
}

// SPITarget.init(): reconfigures the object, keeping the id set by the constructor.
static mp_obj_t machine_spi_target_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    machine_spi_target_init_impl(self, spi_get_id(&self->spi_obj), n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_target_init_obj, 1, machine_spi_target_init);

static mp_obj_t machine_spi_target_read(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);

    uint32_t start = mp_hal_ticks_ms();
    size_t received = 0;
    uint8_t *rx_buf = bufinfo.buf;

    while (received < bufinfo.len) {
        uint32_t num_available = spi_hw_target_get_num_in_rx_fifo(&self->spi_obj);
        while (num_available > 0 && received < bufinfo.len) {
            rx_buf[received++] = spi_hw_target_read(&self->spi_obj);
            num_available--;
        }

        if (spi_target_elapsed_ms(start) > self->timeout) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPITarget read timeout"));
        }
        mp_event_handle_nowait();
    }

    return mp_obj_new_int(received);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_target_read_obj, machine_spi_target_read);

static mp_obj_t machine_spi_target_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    spi_hw_target_clear_tx_fifo(&self->spi_obj);

    uint32_t start = mp_hal_ticks_ms();
    size_t written = 0;
    uint8_t *tx_buf = bufinfo.buf;

    while (written < bufinfo.len) {
        if (spi_hw_target_tx_fifo_has_space(&self->spi_obj)) {
            spi_hw_target_write(&self->spi_obj, tx_buf[written++]);
            // Timeout tracks FIFO stall time, not total transfer time.
            start = mp_hal_ticks_ms();
        } else {
            if (spi_target_elapsed_ms(start) > self->timeout) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPITarget write timeout"));
            }
            mp_event_handle_nowait();
        }
    }

    start = mp_hal_ticks_ms();
    while (!spi_hw_target_is_tx_complete(&self->spi_obj)) {
        if (spi_target_elapsed_ms(start) > self->timeout) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPITarget write timeout"));
        }
        mp_event_handle_nowait();
    }

    return mp_obj_new_int(written);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_target_write_obj, machine_spi_target_write);

static mp_obj_t machine_spi_target_write_readinto(mp_obj_t self_in, mp_obj_t tx_buf_in, mp_obj_t rx_buf_in) {
    machine_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t tx_bufinfo;
    mp_buffer_info_t rx_bufinfo;
    mp_get_buffer_raise(tx_buf_in, &tx_bufinfo, MP_BUFFER_READ);
    mp_get_buffer_raise(rx_buf_in, &rx_bufinfo, MP_BUFFER_WRITE);

    if (tx_bufinfo.len != rx_bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffers must be the same length"));
    }

    spi_hw_target_clear_tx_fifo(&self->spi_obj);
    spi_hw_target_clear_rx_fifo(&self->spi_obj);

    uint8_t *tx_buf = tx_bufinfo.buf;
    uint8_t *rx_buf = rx_bufinfo.buf;
    size_t len = tx_bufinfo.len;
    size_t tx_idx = 0;
    size_t rx_idx = 0;
    uint32_t start = mp_hal_ticks_ms();

    while (rx_idx < len) {
        bool progressed = false;

        // Load TX FIFO
        while (tx_idx < len && spi_hw_target_tx_fifo_has_space(&self->spi_obj)) {
            spi_hw_target_write(&self->spi_obj, tx_buf[tx_idx++]);
            progressed = true;
        }

        // Read RX FIFO
        while (rx_idx < len && spi_hw_target_get_num_in_rx_fifo(&self->spi_obj) > 0) {
            rx_buf[rx_idx++] = spi_hw_target_read(&self->spi_obj);
            progressed = true;
        }

        if (progressed) {
            // Timeout tracks no-progress stalls (e.g. FIFO-full with no clocking).
            start = mp_hal_ticks_ms();
        }

        if (spi_target_elapsed_ms(start) > self->timeout) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPITarget transfer timeout"));
        }
        mp_event_handle_nowait();
    }

    return mp_obj_new_int(len);
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_spi_target_write_readinto_obj, machine_spi_target_write_readinto);

void machine_spi_target_deinit_all(void) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_SPI_TARGET_MAX; i++) {
        if (MP_STATE_PORT(machine_spi_target_obj[i]) != NULL) {
            machine_spi_target_deinit(MP_OBJ_FROM_PTR(MP_STATE_PORT(machine_spi_target_obj[i])));
        }
    }
}

static const mp_rom_map_elem_t machine_spi_target_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),           MP_ROM_PTR(&machine_spi_target_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),       MP_ROM_PTR(&machine_spi_target_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),          MP_ROM_PTR(&machine_spi_target_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&machine_spi_target_write_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),         MP_ROM_PTR(&machine_spi_target_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_MSB),            MP_ROM_INT(MICROPY_PY_MACHINE_SPI_MSB) },
    { MP_ROM_QSTR(MP_QSTR_LSB),            MP_ROM_INT(MICROPY_PY_MACHINE_SPI_LSB) },
};
static MP_DEFINE_CONST_DICT(machine_spi_target_locals_dict, machine_spi_target_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_target_type,
    MP_QSTR_SPITarget,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_target_make_new,
    print, machine_spi_target_print,
    locals_dict, &machine_spi_target_locals_dict
    );

#endif // MICROPY_PY_MACHINE_SPI_TARGET
