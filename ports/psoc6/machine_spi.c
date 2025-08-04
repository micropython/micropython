/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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
#include "py/runtime.h"
#include "extmod/modmachine.h"
#include "py/mperrno.h"

// MTB includes
#include "cybsp.h"
#include "cyhal.h"


// port-specific includes
#include "modmachine.h"
#include "machine_pin_phy.h"
#include "mplogger.h"

#define DEFAULT_SPI_BAUDRATE    (1000000)
#define DEFAULT_SPI_POLARITY    (0)
#define DEFAULT_SPI_PHASE       (0)
#define DEFAULT_SPI_BITS        (8)
#define DEFAULT_SPI_FIRSTBIT    (0) // msb
#define DEFAULT_SPI_SSEL_PIN    (MP_ROM_QSTR(MP_QSTR_NC))
#define MASTER_MODE (0)
#define SLAVE_MODE  (1)


#define spi_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

#define spi_alloc_msg(pin_name, pin_obj, msg)   if (pin_obj == NULL) { \
        size_t slen; \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), mp_obj_str_get_data(pin_name, &slen)); \
}

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    cyhal_spi_t spi_obj;
    int id; // This parameter is unused and added for compliance with reference API
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
    uint32_t ssel;
    uint32_t sck;
    uint32_t mosi;
    uint32_t miso;
    uint32_t baudrate;
} machine_spi_obj_t;

machine_spi_obj_t *spi_obj[MAX_SPI] = { NULL };

// Function to select the mode
static cyhal_spi_mode_t spi_mode_select(uint8_t firstbit, uint8_t polarity, uint8_t phase) {

    cyhal_spi_mode_t mode;
    if (firstbit == 1) {
        if (polarity == 0) {
            if (phase == 0) {
                mode = CYHAL_SPI_MODE_00_LSB;
            } else {
                mode = CYHAL_SPI_MODE_01_LSB;
            }
        } else {
            if (phase == 0) {
                mode = CYHAL_SPI_MODE_10_LSB;
            } else {
                mode = CYHAL_SPI_MODE_11_LSB;
            }
        }
    } else {
        if (polarity == 0) {
            if (phase == 0) {
                mode = CYHAL_SPI_MODE_00_MSB;
            } else {
                mode = CYHAL_SPI_MODE_01_MSB;
            }
        } else {
            if (phase == 0) {
                mode = CYHAL_SPI_MODE_10_MSB;
            } else {
                mode = CYHAL_SPI_MODE_11_MSB;
            }
        }
    }
    return mode;
}

static inline machine_spi_obj_t *spi_obj_alloc(bool is_slave) {
    for (uint8_t i = 0; i < MAX_SPI; i++)
    {
        if (spi_obj[i] == NULL) {

            const mp_obj_type_t *obj_type = &machine_spi_type;
            #if MICROPY_PY_MACHINE_SPI_SLAVE
            if (is_slave) {
                obj_type = &machine_spi_slave_type;
            }
            #endif
            spi_obj[i] = mp_obj_malloc(machine_spi_obj_t, obj_type);
            return spi_obj[i];
        }
    }

    return NULL;
}

static inline void spi_obj_free(machine_spi_obj_t *spi_obj_ptr) {
    for (uint8_t i = 0; i < MAX_SPI; i++)
    {
        if (spi_obj[i] == spi_obj_ptr) {
            spi_obj[i] = NULL;
        }
    }
}

static inline void spi_init(machine_spi_obj_t *machine_spi_obj, int spi_mode) {
    cyhal_spi_mode_t mode = spi_mode_select(machine_spi_obj->firstbit, machine_spi_obj->polarity, machine_spi_obj->phase);
    // set the baudrate
    cyhal_spi_set_frequency(&machine_spi_obj->spi_obj, machine_spi_obj->baudrate);
    // Initialise the SPI peripheral if any arguments given, or it was not initialised previously.
    cy_rslt_t result = cyhal_spi_init(&machine_spi_obj->spi_obj, machine_spi_obj->mosi, machine_spi_obj->miso, machine_spi_obj->sck, machine_spi_obj->ssel, NULL, machine_spi_obj->bits, mode, spi_mode);
    assert_pin_phy_used(result);
    spi_assert_raise_val("SPI initialisation failed with return code %x !", result);
}

static void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%u, ssel=%d, sck=%d, mosi=%d, miso=%d)",
        self->baudrate, self->polarity,
        self->phase, self->bits, self->firstbit,
        self->ssel, self->sck, self->mosi, self->miso);
}

mp_obj_t machine_spi_init_helper(machine_spi_obj_t *self, int spi_mode, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_ssel, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_BAUDRATE} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_POLARITY} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_PHASE} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_BITS} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_FIRSTBIT} },
        { MP_QSTR_ssel,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = DEFAULT_SPI_SSEL_PIN}},
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // set id if provided
    if (args[ARG_id].u_int != -1) {
        self->id = args[ARG_id].u_int;
        mp_printf(&mp_plat_print, "machine.SPI: ID parameter is ignored in this port.\n");
    }
    // set baudrate if provided
    if (args[ARG_baudrate].u_int != -1) {
        self->baudrate = args[ARG_baudrate].u_int;
    }

    // set polarity(CPOL) if provided
    if (args[ARG_polarity].u_int != -1) {
        self->polarity = args[ARG_polarity].u_int;
    }

    // set phase(CPHA) if provided
    if (args[ARG_phase].u_int != -1) {
        self->phase = args[ARG_phase].u_int;
    }

    // set bits if provided
    if (args[ARG_bits].u_int != -1) {
        self->bits = args[ARG_bits].u_int;
    }

    // set firstbit if provided(LSB or MSB first)
    if (args[ARG_firstbit].u_int != -1) {
        self->firstbit = args[ARG_firstbit].u_int;
    }

    // Set SSEL/SCK/MOSI/MISO pins if configured.
    if (spi_mode == MASTER_MODE) {
        if (args[ARG_ssel].u_obj != DEFAULT_SPI_SSEL_PIN) {
            mp_raise_TypeError(MP_ERROR_TEXT("SSEL pin cannot be provided in master constructor!"));
        } else {
            self->ssel = pin_addr_by_name(DEFAULT_SPI_SSEL_PIN);
        }
    } else if (spi_mode == SLAVE_MODE) {
        if ((args[ARG_ssel].u_obj != DEFAULT_SPI_SSEL_PIN)) {
            self->ssel = pin_addr_by_name(args[ARG_ssel].u_obj);
        } else {
            mp_raise_TypeError(MP_ERROR_TEXT("SSEL pin must be provided in slave mode"));
        }
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("SPI should either be in master or slave mode!"));
    }

    if (args[ARG_sck].u_obj != mp_const_none) {
        self->sck = pin_addr_by_name(args[ARG_sck].u_obj);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("SCK pin must be provided"));
    }

    if (args[ARG_mosi].u_obj != mp_const_none) {
        self->mosi = pin_addr_by_name(args[ARG_mosi].u_obj);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("MOSI pin must be provided"));
    }

    if (args[ARG_miso].u_obj != mp_const_none) {
        self->miso = pin_addr_by_name(args[ARG_miso].u_obj);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("MISO pin must be provided"));
    }

    if (n_args > 1 || n_kw > 0) {
        spi_init(self, spi_mode);
    }
    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t machine_spi_master_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    machine_spi_obj_t *self = spi_obj_alloc(false);
    if (n_kw > 1) {
        machine_spi_init_helper(self, MASTER_MODE, n_args, n_kw, all_args);
    } else {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Master init failed as all required arguments not passed!"));
    }
    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t machine_spi_slave_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    machine_spi_obj_t *self = spi_obj_alloc(true);
    if (n_kw > 1) {
        machine_spi_init_helper(self, SLAVE_MODE, n_args, n_kw, all_args);
    } else {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Slave init failed as all required arguments not passed!"));
    }
    return MP_OBJ_FROM_PTR(self);
}

static void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Init not supported. Use the constructor to initialize.\n"));
}

static void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *tx, uint8_t *rx) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    cy_rslt_t result;
    const uint8_t *tx_buf;
    uint8_t *rx_buf;
    uint8_t tx_temp_buf[len];
    uint8_t rx_temp_buf[len];
    uint8_t write_fill = 0xFF;

    cyhal_spi_clear(&self->spi_obj);

    // write
    if (rx == NULL) {
        tx_buf = tx;
        memset(rx_temp_buf, 0x01, len * sizeof(uint8_t));
        rx_buf = rx_temp_buf;
    } else {
        // read(), readinto() and write_readinto() with tx and rx same buffers
        if (tx == rx || tx == NULL) {
            memcpy(tx_temp_buf, tx, len * sizeof(uint8_t));
            tx_buf = tx_temp_buf;
            rx_buf = rx;
            write_fill = tx_temp_buf[0];
        }
        // write_readinto() with tx and rx different buffers
        else {
            tx_buf = tx;
            rx_buf = rx;
        }
    }
    result = cyhal_spi_transfer(&self->spi_obj, tx_buf, len, rx_buf, len, write_fill);
    spi_assert_raise_val("SPI read failed with return code %lx !", result);
}


static void machine_spi_deinit(mp_obj_base_t *self_in) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    cyhal_spi_clear(&self->spi_obj);
    cyhal_spi_free(&self->spi_obj);
    spi_obj_free(self);
}

static const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .deinit = machine_spi_deinit,
    .transfer = machine_spi_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_master_make_new,
    print, machine_spi_print,
    protocol, &machine_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );

#if MICROPY_PY_MACHINE_SPI_SLAVE

static void machine_spi_slave_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPISlave(baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%u, ssel=%d, sck=%d, mosi=%d, miso=%d)",
        self->baudrate, self->polarity,
        self->phase, self->bits, self->firstbit,
        self->ssel, self->sck, self->mosi, self->miso);
}

static mp_obj_t machine_spi_slave_deinit(mp_obj_t self_in) {
    mp_obj_base_t *self = MP_OBJ_TO_PTR(self_in);
    machine_spi_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_spi_slave_deinit_obj, machine_spi_slave_deinit);

static mp_obj_t machine_spi_slave_read(mp_obj_t self_in, mp_obj_t buf_in) {
    cy_rslt_t result;
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    uint16_t len = bufinfo.len;
    uint8_t tx_dummy[len];
    memset(tx_dummy, 0x02, len * sizeof(uint8_t));
    result = cyhal_spi_transfer(&self->spi_obj, tx_dummy, len, bufinfo.buf, len, 0xFF);
    spi_assert_raise_val("SPI slave read failed with return code %lx !", result);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_slave_read_obj, machine_spi_slave_read);

static mp_obj_t machine_spi_slave_write(mp_obj_t self_in, mp_obj_t buf_in) {
    cy_rslt_t result;
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    uint16_t len = bufinfo.len;
    uint8_t rx_dummy[len];
    memset(rx_dummy, 0x01, len * sizeof(uint8_t));
    result = cyhal_spi_transfer(&self->spi_obj, bufinfo.buf, len, rx_dummy, len, 0xFF);
    spi_assert_raise_val("SPI slave write failed with return code %lx !", result);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_slave_write_obj, machine_spi_slave_write);

static mp_obj_t machine_spi_slave_write_readinto(mp_obj_t self_in, mp_obj_t tx_buf_in, mp_obj_t rx_buf_in) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t tx_bufinfo;
    mp_buffer_info_t rx_bufinfo;
    mp_get_buffer_raise(tx_buf_in, &tx_bufinfo, MP_BUFFER_READ);
    mp_get_buffer_raise(rx_buf_in, &rx_bufinfo, MP_BUFFER_WRITE);
    uint16_t len = tx_bufinfo.len;
    if (tx_bufinfo.len != rx_bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffers must be the same length"));
    }
    cy_rslt_t result = cyhal_spi_transfer(&self->spi_obj, tx_bufinfo.buf, len, rx_bufinfo.buf, len, 0xFF);
    spi_assert_raise_val("SPI slave write failed with return code %lx !", result);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_spi_slave_write_readinto_obj, machine_spi_slave_write_readinto);


static const mp_rom_map_elem_t machine_spi_slave_locals_dict_table[] = {
    // Functions
    { MP_ROM_QSTR(MP_QSTR_read),           MP_ROM_PTR(&machine_spi_slave_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),          MP_ROM_PTR(&machine_spi_slave_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&machine_spi_slave_write_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),         MP_ROM_PTR(&machine_spi_slave_deinit_obj) },
    // constants
    { MP_ROM_QSTR(MP_QSTR_MSB),            MP_ROM_INT(MICROPY_PY_MACHINE_SPISLAVE_MSB) },
    { MP_ROM_QSTR(MP_QSTR_LSB),            MP_ROM_INT(MICROPY_PY_MACHINE_SPISLAVE_LSB) },
};
static MP_DEFINE_CONST_DICT(machine_spi_slave_locals_dict, machine_spi_slave_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_slave_type,
    MP_QSTR_SPISlave,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_slave_make_new,
    print, machine_spi_slave_print,
    locals_dict, &machine_spi_slave_locals_dict
    );

#endif

void machine_spi_deinit_all() {
    for (uint8_t i = 0; i < MAX_SPI; i++) {
        if (spi_obj[i] != NULL) {
            machine_spi_deinit((mp_obj_base_t *)(spi_obj[i]));
        }
    }
}
