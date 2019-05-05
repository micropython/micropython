/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nicko van Someren
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"

#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

#define DEBUG 1
#if DEBUG
#define DEBUG_printf(...) ESP_LOGI("modsdcard", __VA_ARGS__)
#else
#define DEBUG_printf(...) (0)
#endif

//
// There are three layers of abstraction: host, slot and card.
// Creating an SD Card object will initialise the host and slot.
// The card gets initialised by ioctl op==1 and de-inited by ioctl 2
// The host is de-inited in __del__
//

// Currently the ESP32 Library doesn't support MM cards, so
// we don't enable on MICROPY_HW_ENABLE_MMCARD.
#if MICROPY_HW_ENABLE_SDCARD

// Forward declaration
const mp_obj_type_t esp32_sdcard_type;

typedef struct _sdcard_obj_t {
    mp_obj_base_t base;
    mp_int_t flags;
    sdmmc_host_t host;
    // The card structure duplicates the host. It's not clear if we can avoid this
    sdmmc_card_t card;
} sdcard_card_obj_t;

#define SDCARD_CARD_FLAGS_HOST_INIT_DONE 0x01
#define SDCARD_CARD_FLAGS_CARD_INIT_DONE 0x02

#define _SECTOR_SIZE(self) (self->card.csd.sector_size)

STATIC esp_err_t check_esp_err(esp_err_t code) {
    const mp_obj_type_t *ex_type = NULL;

    switch(code) {
    case ESP_OK:
        return ESP_OK;
    case ESP_ERR_NO_MEM:
        ex_type = &mp_type_MemoryError;
        break;
    case ESP_ERR_TIMEOUT:
        ex_type = &mp_type_TimeoutError;
        break;
    case ESP_ERR_NOT_SUPPORTED:
        ex_type = &mp_type_NotImplementedError;
        break;
    default:
        mp_raise_OSError(code);
    }

    mp_raise_msg(ex_type, "From ESP SDK");
}

STATIC esp_err_t sdcard_ensure_card_init(sdcard_card_obj_t *self, bool force) {
    if (force || !(self->flags & SDCARD_CARD_FLAGS_CARD_INIT_DONE)) {
        DEBUG_printf("  Calling card init");

        esp_err_t err = sdmmc_card_init(&(self->host), &(self->card));
        if (err == ESP_OK) {
            self->flags |= SDCARD_CARD_FLAGS_CARD_INIT_DONE;
        } else {
            self->flags &= ~SDCARD_CARD_FLAGS_CARD_INIT_DONE;
        }
        DEBUG_printf("  Card init returned: %i", err);

        return err;
    } else {
        return ESP_OK;
    }
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the SD card or MMC as an object with the block protocol.

STATIC mp_obj_t esp32_sdcard_init() {
    static int initialized = 0;
    if (!initialized) {
        // It might make sense to set up the host controller when the
        // module is loaded, rather then when we slots are created.

        DEBUG_printf("Initialising sdcard module");

        initialized = 1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp32_sdcard_init_obj, esp32_sdcard_init);


// Create a new SDCard object
// card = SDCard(slot=1, width=None, present_pin=None, wp_pin=None)

STATIC mp_obj_t esp32_sdcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    enum {
        ARG_slot,
        ARG_width,
        ARG_cd_pin,
        ARG_wp_pin,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_slot,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_width,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_cd_pin,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_wp_pin,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
    };
    mp_arg_val_t arg_vals[MP_ARRAY_SIZE(allowed_args)];
    mp_map_t kw_args;

    DEBUG_printf("Making new SDCard:n");
    DEBUG_printf("  Unpacking arguments");

    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    mp_arg_parse_all(n_args, args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, arg_vals);

    DEBUG_printf("  slot=%d, width=%d, cd_pin=%d, wp_pin=%d",
                 arg_vals[ARG_slot].u_int, arg_vals[ARG_width].u_int,
                 arg_vals[ARG_cd_pin].u_int, arg_vals[ARG_wp_pin].u_int);

    DEBUG_printf("  Setting up host configuration");

    sdcard_card_obj_t *self = m_new0(sdcard_card_obj_t, 1);
    self->base.type = &esp32_sdcard_type;
    sdmmc_host_t _temp_host = SDMMC_HOST_DEFAULT();
    self->host = _temp_host;

    int slot_num = arg_vals[ARG_slot].u_int;
    if (slot_num !=0 && slot_num != 1)
        mp_raise_ValueError("Slot number must be 0 or 1");
    self->host.slot = slot_num;

    // Forcing pins into a usable state
    gpio_set_pull_mode(15, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
    gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
    gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
    gpio_set_pull_mode(13, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

    DEBUG_printf("  Setting up slot configuration");

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // TODO: Set up WP and CD pins if needed
    // Do we want to require pin numbers or should we allow Pin objects too?
    if (arg_vals[ARG_cd_pin].u_int != 0xffffffff ||
        arg_vals[ARG_wp_pin].u_int != 0xffffffff)
        mp_raise_NotImplementedError("WP and CD pins not currently supported");

    int width = arg_vals[ARG_width].u_int;
    if (width != 0xffffffff) {
        if (width == 1 || width == 4 || (width == 8 && slot_num == 0))
            slot_config.width = arg_vals[ARG_width].u_int;
        else
            mp_raise_ValueError("Width must be 1 or 4 (or 8 on slot 0)");
    }

    // Stronger external pull-ups are still needed but apparently it is a good idea to
    // set the internal pull-ups anyway.
    slot_config.flags = SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    DEBUG_printf("  Calling host.init()");

    check_esp_err(self->host.init());
    self->flags |= SDCARD_CARD_FLAGS_HOST_INIT_DONE;
    // TODO: Do we need to de-init the host if the slot init fails?

    DEBUG_printf("  Calling init_slot()");

    check_esp_err(sdmmc_host_init_slot(self->host.slot, &slot_config));

    DEBUG_printf("  Returning new card object: %p", self);
    // return object
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t sd_deinit(mp_obj_t self_in) {
    sdcard_card_obj_t *self = self_in;

    DEBUG_printf("De-init host\n");

    if (self->flags & SDCARD_CARD_FLAGS_HOST_INIT_DONE) {
        self->host.deinit();
        self->flags &= ~SDCARD_CARD_FLAGS_HOST_INIT_DONE;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_deinit_obj, sd_deinit);

STATIC mp_obj_t sd_info(mp_obj_t self_in) {
    sdcard_card_obj_t *self = self_in;
    // TODO: Extract SD card data

    check_esp_err(sdcard_ensure_card_init((sdcard_card_obj_t *) self, false));

    uint32_t card_type = 0;
    uint32_t log_block_nbr = self->card.csd.capacity;
    uint32_t log_block_size = _SECTOR_SIZE(self);

    mp_obj_t tuple[3] = {
        mp_obj_new_int_from_ull((uint64_t)log_block_nbr * (uint64_t)log_block_size),
        mp_obj_new_int_from_uint(log_block_size),
        mp_obj_new_int(card_type),
    };
    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_info_obj, sd_info);


STATIC mp_obj_t esp32_sdcard_readblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    sdcard_card_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    esp_err_t err;
    err = sdcard_ensure_card_init((sdcard_card_obj_t *) self, false);
    if (err != ESP_OK)
        return false;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    err = sdmmc_read_sectors(&(self->card), bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / _SECTOR_SIZE(self) );
    return mp_obj_new_bool(err == ESP_OK);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_sdcard_readblocks_obj, esp32_sdcard_readblocks);

STATIC mp_obj_t esp32_sdcard_writeblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    sdcard_card_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    esp_err_t err;
    err = sdcard_ensure_card_init((sdcard_card_obj_t *) self, false);
    if (err != ESP_OK)
        return false;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    err = sdmmc_write_sectors(&(self->card), bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / _SECTOR_SIZE(self) );
    return mp_obj_new_bool(err == ESP_OK);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_sdcard_writeblocks_obj, esp32_sdcard_writeblocks);

STATIC mp_obj_t esp32_sdcard_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    sdcard_card_obj_t *self = self_in;
    esp_err_t err = ESP_OK;
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT:
            err = sdcard_ensure_card_init(self, true);
            return MP_OBJ_NEW_SMALL_INT((err == ESP_OK) ? 0 : -1);

        case BP_IOCTL_DEINIT:
            // Ensure that future attempts to look at info re-read the card
            self->flags &= ~SDCARD_CARD_FLAGS_CARD_INIT_DONE;
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SYNC:
            // nothing to do
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case BP_IOCTL_SEC_COUNT:
            err = sdcard_ensure_card_init(self, false);
            if (err != ESP_OK)
                return MP_OBJ_NEW_SMALL_INT(-1);
            return MP_OBJ_NEW_SMALL_INT(self->card.csd.capacity);

        case BP_IOCTL_SEC_SIZE:
            err = sdcard_ensure_card_init(self, false);
            if (err != ESP_OK)
                return MP_OBJ_NEW_SMALL_INT(-1);
            return MP_OBJ_NEW_SMALL_INT(_SECTOR_SIZE(self));

        default: // unknown command
            return MP_OBJ_NEW_SMALL_INT(-1); // error
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_sdcard_ioctl_obj, esp32_sdcard_ioctl);

STATIC const mp_rom_map_elem_t esp32_sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&sd_info_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&sd_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&sd_deinit_obj) },
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&esp32_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&esp32_sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&esp32_sdcard_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(esp32_sdcard_locals_dict, esp32_sdcard_locals_dict_table);


const mp_obj_type_t esp32_sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDCard,
    .make_new = esp32_sdcard_make_new,
    .locals_dict = (mp_obj_dict_t*)&esp32_sdcard_locals_dict,
};

STATIC const mp_rom_map_elem_t esp32_module_sdcard_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sdcard) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&esp32_sdcard_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_SDCard), MP_ROM_PTR(&esp32_sdcard_type) },
};

STATIC MP_DEFINE_CONST_DICT(esp32_module_sdcard_globals, esp32_module_sdcard_globals_table);

const mp_obj_module_t mp_module_sdcard = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&esp32_module_sdcard_globals,
};


#endif // MICROPY_HW_ENABLE_SDCARD
