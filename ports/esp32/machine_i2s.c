/*
 * This file is part of the MicroPython ESP32 project
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mike Teachman
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

#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"
#include "driver/i2s.h"
#include "soc/i2s_reg.h"

// Notes on naming conventions:
// 1. "id" versus "port"
//    The MicroPython API identifies instances of a peripheral using "id", while the ESP-IDF uses "port".
//    - for example, the first I2S peripheral on the ESP32 would be indicated by id=0 in MicroPython
//      and port=0 in ESP-IDF
// 2. any C type, macro, or function prefaced by "i2s" is associated with an ESP-IDF I2S interface definition
// 3. any C type, macro, or function prefaced by "machine_i2s" is associated with the MicroPython implementation of I2S

typedef struct _machine_i2s_obj_t {
    mp_obj_base_t base;
    i2s_port_t id;
    i2s_comm_format_t standard;
    uint8_t mode;
    i2s_bits_per_sample_t dataformat;
    i2s_channel_fmt_t channelformat;
    int32_t samplerate;
    int16_t dmacount;
    int16_t dmalen;
    int32_t apllrate;
    int8_t bck;
    int8_t ws;
    int8_t sdout;
    int8_t sdin;
    bool used;
} machine_i2s_obj_t;

// Static object mapping to I2S peripherals
//   note:  I2S implementation makes use of the following mapping between I2S peripheral and I2S object
//      I2S peripheral 1:  machine_i2s_obj[0]
//      I2S peripheral 2:  machine_i2s_obj[1]
STATIC machine_i2s_obj_t machine_i2s_obj[I2S_NUM_MAX];

// this will be called on MicroPython startup and every soft reset
void machine_i2s_init(void) {
    for (uint8_t i = 0; i < I2S_NUM_MAX; i++) {
        machine_i2s_obj[i].used = false;
    }
}

STATIC void machine_i2s_init_helper(machine_i2s_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum {
        ARG_bck,
        ARG_ws,
        ARG_sdout,
        ARG_sdin,
        ARG_standard,
        ARG_mode,
        ARG_dataformat,
        ARG_channelformat,
        ARG_samplerate,
        ARG_dmacount,
        ARG_dmalen,
        ARG_apllrate,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bck,              MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_ws,               MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sdout,            MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sdin,             MP_ARG_KW_ONLY | MP_ARG_OBJ,   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_standard,         MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = I2S_COMM_FORMAT_I2S} },
        { MP_QSTR_mode,             MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_dataformat,       MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_channelformat,    MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_samplerate,       MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT,   {.u_int = -1} },
        { MP_QSTR_dmacount,         MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = 16} },
        { MP_QSTR_dmalen,           MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = 64} },
        { MP_QSTR_apllrate,         MP_ARG_KW_ONLY | MP_ARG_INT,   {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    //
    // ---- Check validity of arguments ----
    //

    // are I2S pin assignments valid?
    int8_t bck = args[ARG_bck].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_bck].u_obj);
    int8_t ws = args[ARG_ws].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_ws].u_obj);
    int8_t sdin = args[ARG_sdin].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_sdin].u_obj);
    int8_t sdout = args[ARG_sdout].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_sdout].u_obj);

    if ((sdin == -1) && (args[ARG_mode].u_int == (I2S_MODE_MASTER | I2S_MODE_RX))) {
        mp_raise_ValueError(MP_ERROR_TEXT("sdin must be specified for RX mode"));
    }

    if ((sdout == -1) && (args[ARG_mode].u_int == (I2S_MODE_MASTER | I2S_MODE_TX))) {
        mp_raise_ValueError(MP_ERROR_TEXT("sdout must be specified for TX mode"));
    }

    if ((sdin != -1) && (sdout != -1)) {
        mp_raise_ValueError(MP_ERROR_TEXT("only one of sdin or sdout can be specified"));
    }

    // is Standard valid?
    i2s_comm_format_t i2s_commformat = args[ARG_standard].u_int;
    if ((i2s_commformat != I2S_COMM_FORMAT_I2S) &&
        (i2s_commformat != (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB))) {
        mp_raise_ValueError(MP_ERROR_TEXT("Standard is not valid"));
    }

    // is Mode valid?
    i2s_mode_t i2s_mode = args[ARG_mode].u_int;
    if ((i2s_mode != (I2S_MODE_MASTER | I2S_MODE_RX)) &&
        (i2s_mode != (I2S_MODE_MASTER | I2S_MODE_TX))) {
        mp_raise_ValueError(MP_ERROR_TEXT("Only Master Rx or Master Tx Modes are supported"));
    }

    // is Data Format valid?
    i2s_bits_per_sample_t i2s_bits_per_sample = args[ARG_dataformat].u_int;
    if ((i2s_bits_per_sample != I2S_BITS_PER_SAMPLE_16BIT) &&
        (i2s_bits_per_sample != I2S_BITS_PER_SAMPLE_24BIT) &&
        (i2s_bits_per_sample != I2S_BITS_PER_SAMPLE_32BIT)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Data Format is not valid"));
    }

    // is Channel Format valid?
    i2s_channel_fmt_t i2s_channelformat = args[ARG_channelformat].u_int;
    if ((i2s_channelformat != I2S_CHANNEL_FMT_RIGHT_LEFT) &&
        (i2s_channelformat != I2S_CHANNEL_FMT_ALL_RIGHT) &&
        (i2s_channelformat != I2S_CHANNEL_FMT_ALL_LEFT) &&
        (i2s_channelformat != I2S_CHANNEL_FMT_ONLY_RIGHT) &&
        (i2s_channelformat != I2S_CHANNEL_FMT_ONLY_LEFT)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Channel Format is not valid"));
    }

    // is Sample Rate valid?
    // No validation done:  ESP-IDF API does not indicate a valid range for sample rate

    // is DMA Buffer Count valid?
    // ESP-IDF API code checks for buffer count in this range:  [2, 128]
    int16_t i2s_dmacount = args[ARG_dmacount].u_int;
    if ((i2s_dmacount < 2) || (i2s_dmacount > 128)) {
        mp_raise_ValueError(MP_ERROR_TEXT("DMA Buffer Count is not valid.  Allowed range is [2, 128]"));
    }

    // is DMA Buffer Length valid?
    // ESP-IDF API code checks for buffer length in this range:  [8, 1024]
    int16_t i2s_dmalen = args[ARG_dmalen].u_int;
    if ((i2s_dmalen < 8) || (i2s_dmalen > 1024)) {
        mp_raise_ValueError(MP_ERROR_TEXT("DMA Buffer Length is not valid.  Allowed range is [8, 1024]"));
    }

    // is APLL Rate valid?
    // No validation done:  ESP-IDF API does not indicate a valid range for APLL rate

    self->bck = bck;
    self->ws = ws;
    self->sdout = sdout;
    self->sdin = sdin;
    self->standard = args[ARG_standard].u_int;
    self->mode = args[ARG_mode].u_int;
    self->dataformat = args[ARG_dataformat].u_int;
    self->channelformat = args[ARG_channelformat].u_int;
    self->samplerate = args[ARG_samplerate].u_int;
    self->dmacount = args[ARG_dmacount].u_int;
    self->dmalen = args[ARG_dmalen].u_int;
    self->apllrate = args[ARG_apllrate].u_int;

    i2s_config_t i2s_config;
    i2s_config.communication_format = self->standard;
    i2s_config.mode = self->mode;
    i2s_config.bits_per_sample = self->dataformat;
    i2s_config.channel_format = self->channelformat;
    i2s_config.sample_rate = self->samplerate;
    i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LOWMED; // allows simultaneous use of both I2S channels
    i2s_config.dma_buf_count = self->dmacount;
    i2s_config.dma_buf_len = self->dmalen;
    if (self->apllrate != 0) {
        i2s_config.use_apll = true;
    } else {
        i2s_config.use_apll = false;
    }
    i2s_config.fixed_mclk = self->apllrate;

    // uninstall I2S driver when changes are being made to an active I2S peripheral
    if (self->used) {
        i2s_driver_uninstall(self->id);
    }

    esp_err_t ret = i2s_driver_install(self->id, &i2s_config, 0, NULL);
    switch (ret) {
        case ESP_OK:
            break;
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S driver install: Parameter error"));
            break;
        case ESP_ERR_NO_MEM:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S driver install: Out of memory"));
            break;
        default:
            // this error not documented in ESP-IDF
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S driver install: Undocumented error"));
            break;
    }

    // apply workaround for bug in some ESP-IDF versions that swap
    // the left and right channels
    // https://github.com/espressif/esp-idf/issues/6625
    REG_SET_BIT(I2S_CONF_REG(self->id), I2S_TX_MSB_RIGHT);
    REG_SET_BIT(I2S_CONF_REG(self->id), I2S_RX_MSB_RIGHT);

    i2s_pin_config_t pin_config;
    pin_config.bck_io_num = self->bck;
    pin_config.ws_io_num = self->ws;
    pin_config.data_out_num = self->sdout;
    pin_config.data_in_num = self->sdin;

    ret = i2s_set_pin(self->id, &pin_config);
    switch (ret) {
        case ESP_OK:
            break;
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S set pin: Parameter error"));
            break;
        case ESP_FAIL:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S set pin: IO error"));
            break;
        default:
            // this error not documented in ESP-IDF
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S set pin: Undocumented error"));
            break;
    }

    self->used = true;
}

/******************************************************************************/
// MicroPython bindings for I2S
STATIC void machine_i2s_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2S(id=%u, bck=%d, ws=%d, sdout=%d, sdin=%d\n"
        "standard=%u, mode=%u,\n"
        "dataformat=%u, channelformat=%u,\n"
        "samplerate=%d,\n"
        "dmacount=%d, dmalen=%d,\n"
        "apllrate=%d)",
        self->id, self->bck, self->ws, self->sdout, self->sdin,
        self->standard, self->mode,
        self->dataformat, self->channelformat,
        self->samplerate,
        self->dmacount, self->dmalen,
        self->apllrate
        );
}

STATIC mp_obj_t machine_i2s_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);

    machine_i2s_obj_t *self;

    // note: it is safe to assume that the arg pointer below references a positional argument because the arg check above
    //       guarantees that at least one positional argument has been provided
    i2s_port_t i2s_id = mp_obj_get_int(args[0]);
    if (i2s_id == I2S_NUM_0) {
        self = &machine_i2s_obj[0];
    } else if (i2s_id == I2S_NUM_1) {
        self = &machine_i2s_obj[1];
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("I2S ID is not valid"));
    }

    self->base.type = &machine_i2s_type;
    self->id = i2s_id;

    // is I2S peripheral already in use?
    if (self->used) {
        mp_raise_ValueError(MP_ERROR_TEXT("I2S port is already in use"));
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    // note:  "args + 1" below has the effect of skipping over the ID argument
    machine_i2s_init_helper(self, n_pos_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_i2s_obj_init(mp_uint_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // note:  "pos_args + 1" below has the effect of skipping over "self"
    machine_i2s_init_helper(pos_args[0], n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_init_obj, 1, machine_i2s_obj_init);

STATIC mp_obj_t machine_i2s_readinto(mp_uint_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buf, ARG_timeout };
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf,                      MP_ARG_REQUIRED | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    machine_i2s_obj_t *self = pos_args[0];

    if (!self->used) {
        mp_raise_ValueError(MP_ERROR_TEXT("I2S port is not initialized"));
    }

    if (self->mode != (I2S_MODE_MASTER | I2S_MODE_RX)) {
        mp_raise_ValueError(MP_ERROR_TEXT("I2S not configured for read method"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_WRITE);

    TickType_t timeout_in_ticks = portMAX_DELAY;
    if (args[ARG_timeout].u_int != -1) {
        timeout_in_ticks = pdMS_TO_TICKS(args[ARG_timeout].u_int);
    }

    uint32_t num_bytes_read = 0;
    esp_err_t ret = i2s_read(self->id, bufinfo.buf, bufinfo.len, &num_bytes_read, timeout_in_ticks);
    switch (ret) {
        case ESP_OK:
            break;
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S read: Parameter error"));
            break;
        default:
            // this error not documented in ESP-IDF
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S read: Undocumented error"));
            break;
    }

    return mp_obj_new_int(num_bytes_read);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_readinto_obj, 2, machine_i2s_readinto);

STATIC mp_obj_t machine_i2s_write(mp_uint_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buf, ARG_timeout };
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf,                      MP_ARG_REQUIRED | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    machine_i2s_obj_t *self = pos_args[0];

    if (!self->used) {
        mp_raise_ValueError(MP_ERROR_TEXT("I2S port is not initialized"));
    }

    if (self->mode != (I2S_MODE_MASTER | I2S_MODE_TX)) {
        mp_raise_ValueError(MP_ERROR_TEXT("I2S not configured for write method"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_WRITE);

    TickType_t timeout_in_ticks = portMAX_DELAY;
    if (args[ARG_timeout].u_int != -1) {
        timeout_in_ticks = pdMS_TO_TICKS(args[ARG_timeout].u_int);
    }

    uint32_t num_bytes_written = 0;
    esp_err_t ret = i2s_write(self->id, bufinfo.buf, bufinfo.len, &num_bytes_written, timeout_in_ticks);
    switch (ret) {
        case ESP_OK:
            break;
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S write: Parameter error"));
            break;
        default:
            // this error not documented in ESP-IDF
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("I2S write: Undocumented error"));
            break;
    }

    return mp_obj_new_int(num_bytes_written);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2s_write_obj, 2, machine_i2s_write);

STATIC mp_obj_t machine_i2s_deinit(mp_obj_t self_in) {
    machine_i2s_obj_t *self = self_in;
    i2s_driver_uninstall(self->id);
    self->used = false;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_i2s_deinit_obj, machine_i2s_deinit);

STATIC const mp_rom_map_elem_t machine_i2s_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_i2s_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&machine_i2s_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),           MP_ROM_PTR(&machine_i2s_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_i2s_deinit_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_NUM0),            MP_ROM_INT(I2S_NUM_0) },
    { MP_ROM_QSTR(MP_QSTR_NUM1),            MP_ROM_INT(I2S_NUM_1) },
    { MP_ROM_QSTR(MP_QSTR_PHILIPS),         MP_ROM_INT(I2S_COMM_FORMAT_I2S) },
    { MP_ROM_QSTR(MP_QSTR_LSB),             MP_ROM_INT(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB) },
    // note:  ESP-IDF does not implement the MSB standard (even though the Macro I2S_COMM_FORMAT_I2S_MSB is defined)
    { MP_ROM_QSTR(MP_QSTR_MASTER_RX),       MP_ROM_INT(I2S_MODE_MASTER | I2S_MODE_RX) },
    { MP_ROM_QSTR(MP_QSTR_MASTER_TX),       MP_ROM_INT(I2S_MODE_MASTER | I2S_MODE_TX) },
    { MP_ROM_QSTR(MP_QSTR_B16),             MP_ROM_INT(I2S_BITS_PER_SAMPLE_16BIT) },
    { MP_ROM_QSTR(MP_QSTR_B24),             MP_ROM_INT(I2S_BITS_PER_SAMPLE_24BIT) },
    { MP_ROM_QSTR(MP_QSTR_B32),             MP_ROM_INT(I2S_BITS_PER_SAMPLE_32BIT) },
    { MP_ROM_QSTR(MP_QSTR_RIGHT_LEFT),      MP_ROM_INT(I2S_CHANNEL_FMT_RIGHT_LEFT) },
    { MP_ROM_QSTR(MP_QSTR_ALL_RIGHT),       MP_ROM_INT(I2S_CHANNEL_FMT_ALL_RIGHT) },
    { MP_ROM_QSTR(MP_QSTR_ALL_LEFT),        MP_ROM_INT(I2S_CHANNEL_FMT_ALL_LEFT) },
    { MP_ROM_QSTR(MP_QSTR_ONLY_RIGHT),      MP_ROM_INT(I2S_CHANNEL_FMT_ONLY_RIGHT) },
    { MP_ROM_QSTR(MP_QSTR_ONLY_LEFT),       MP_ROM_INT(I2S_CHANNEL_FMT_ONLY_LEFT) },
};
MP_DEFINE_CONST_DICT(machine_i2s_locals_dict, machine_i2s_locals_dict_table);

const mp_obj_type_t machine_i2s_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2S,
    .print = machine_i2s_print,
    .make_new = machine_i2s_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_i2s_locals_dict,
};
