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
#include "py/mperrno.h"
#include "extmod/vfs_fat.h"

#if MICROPY_HW_ENABLE_SDCARD

#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

#define DEBUG 0
#if DEBUG
#define DEBUG_printf(...) ESP_LOGI("modsdcard", __VA_ARGS__)
#else
#define DEBUG_printf(...) (void)0
#endif

//
// There are three layers of abstraction: host, slot and card.
// Creating an SD Card object will initialise the host and slot.
// Cards gets initialised by ioctl op==1 and de-inited by ioctl 2
// Hosts are de-inited in __del__. Slots do not need de-initing.
//

// Forward declaration
const mp_obj_type_t machine_sdcard_type;

typedef struct _sdcard_obj_t {
    mp_obj_base_t base;
    mp_int_t flags;
    sdmmc_host_t host;
    // The card structure duplicates the host. It's not clear if we
    // can avoid this given the way that it is copied.
    sdmmc_card_t card;
} sdcard_card_obj_t;

#define SDCARD_CARD_FLAGS_HOST_INIT_DONE 0x01
#define SDCARD_CARD_FLAGS_CARD_INIT_DONE 0x02

#define _SECTOR_SIZE(self) (self->card.csd.sector_size)

STATIC gpio_num_t pin_or_int(const mp_obj_t arg) {
    if (mp_obj_is_small_int(arg)) {
        return MP_OBJ_SMALL_INT_VALUE(arg);
    } else {
        // This raises a value error if the argument is not a Pin.
        return machine_pin_get_id(arg);
    }
}

#define SET_CONFIG_PIN(config, pin_var, arg_id) \
    if (arg_vals[arg_id].u_obj != mp_const_none) \
    config.pin_var = pin_or_int(arg_vals[arg_id].u_obj)

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

// Create a new SDCard object
// The driver supports either the host SD/MMC controller (default) or SPI mode
// In both cases there are two "slots". Slot 0 on the SD/MMC controller is
// typically tied up with the flash interface in most ESP32 modules but in
// theory supports 1, 4 or 8-bit transfers. Slot 1 supports only 1 and 4-bit
// transfers. Only 1-bit is supported on the SPI interfaces.
// card = SDCard(slot=1, width=None, present_pin=None, wp_pin=None)

STATIC mp_obj_t machine_sdcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    enum {
        ARG_slot,
        ARG_width,
        ARG_cd,
        ARG_wp,
        ARG_miso,
        ARG_mosi,
        ARG_sck,
        ARG_cs,
        ARG_freq,
    };
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_slot,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_width,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_cd,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_wp,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        // These are only needed if using SPI mode
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_cs,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        // freq is valid for both SPI and SDMMC interfaces
        { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 20000000} },
    };
    mp_arg_val_t arg_vals[MP_ARRAY_SIZE(allowed_args)];
    mp_map_t kw_args;

    DEBUG_printf("Making new SDCard:n");
    DEBUG_printf("  Unpacking arguments");

    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);

    mp_arg_parse_all(n_args, args, &kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, arg_vals);

    DEBUG_printf("  slot=%d, width=%d, cd=%p, wp=%p",
        arg_vals[ARG_slot].u_int, arg_vals[ARG_width].u_int,
        arg_vals[ARG_cd].u_obj, arg_vals[ARG_wp].u_obj);

    DEBUG_printf("  miso=%p, mosi=%p, sck=%p, cs=%p",
        arg_vals[ARG_miso].u_obj, arg_vals[ARG_mosi].u_obj,
        arg_vals[ARG_sck].u_obj, arg_vals[ARG_cs].u_obj);

    int slot_num = arg_vals[ARG_slot].u_int;
    if (slot_num < 0 || slot_num > 3) {
        mp_raise_ValueError(MP_ERROR_TEXT("slot number must be between 0 and 3 inclusive"));
    }

    // Slots 0 and 1 are native SD/MMC, slots 2 and 3 are SPI
    bool is_spi = (slot_num >= 2);
    if (is_spi) {
        slot_num -= 2;
    }

    DEBUG_printf("  Setting up host configuration");

    sdcard_card_obj_t *self = m_new_obj_with_finaliser(sdcard_card_obj_t);
    self->base.type = &machine_sdcard_type;
    self->flags = 0;
    // Note that these defaults are macros that expand to structure
    // constants so we can't directly assign them to fields.
    int freq = arg_vals[ARG_freq].u_int;
    if (is_spi) {
        sdmmc_host_t _temp_host = SDSPI_HOST_DEFAULT();
        _temp_host.max_freq_khz = freq / 1000;
        self->host = _temp_host;
    } else {
        sdmmc_host_t _temp_host = SDMMC_HOST_DEFAULT();
        _temp_host.max_freq_khz = freq / 1000;
        self->host = _temp_host;
    }

    if (is_spi) {
        self->host.slot = slot_num ? HSPI_HOST : VSPI_HOST;
    }

    DEBUG_printf("  Calling host.init()");

    check_esp_err(self->host.init());
    self->flags |= SDCARD_CARD_FLAGS_HOST_INIT_DONE;

    if (is_spi) {
        // SPI interface
        STATIC const sdspi_slot_config_t slot_defaults[2] = {
            {
                .gpio_miso = GPIO_NUM_19,
                .gpio_mosi = GPIO_NUM_23,
                .gpio_sck = GPIO_NUM_18,
                .gpio_cs = GPIO_NUM_5,
                .gpio_cd = SDSPI_SLOT_NO_CD,
                .gpio_wp = SDSPI_SLOT_NO_WP,
                .dma_channel = 2
            },
            SDSPI_SLOT_CONFIG_DEFAULT()
        };

        DEBUG_printf("  Setting up SPI slot configuration");
        sdspi_slot_config_t slot_config = slot_defaults[slot_num];

        SET_CONFIG_PIN(slot_config, gpio_cd, ARG_cd);
        SET_CONFIG_PIN(slot_config, gpio_wp, ARG_wp);
        SET_CONFIG_PIN(slot_config, gpio_miso, ARG_miso);
        SET_CONFIG_PIN(slot_config, gpio_mosi, ARG_mosi);
        SET_CONFIG_PIN(slot_config, gpio_sck, ARG_sck);
        SET_CONFIG_PIN(slot_config, gpio_cs, ARG_cs);

        DEBUG_printf("  Calling init_slot()");
        check_esp_err(sdspi_host_init_slot(self->host.slot, &slot_config));
    } else {
        // SD/MMC interface
        DEBUG_printf("  Setting up SDMMC slot configuration");
        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

        // Stronger external pull-ups are still needed but apparently
        // it is a good idea to set the internal pull-ups anyway.
        // slot_config.flags = SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

        SET_CONFIG_PIN(slot_config, gpio_cd, ARG_cd);
        SET_CONFIG_PIN(slot_config, gpio_wp, ARG_wp);

        int width = arg_vals[ARG_width].u_int;
        if (width == 1 || width == 4 || (width == 8 && slot_num == 0)) {
            slot_config.width = width;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("width must be 1 or 4 (or 8 on slot 0)"));
        }

        DEBUG_printf("  Calling init_slot()");
        check_esp_err(sdmmc_host_init_slot(self->host.slot, &slot_config));
    }

    DEBUG_printf("  Returning new card object: %p", self);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t sd_deinit(mp_obj_t self_in) {
    sdcard_card_obj_t *self = self_in;

    DEBUG_printf("De-init host\n");

    if (self->flags & SDCARD_CARD_FLAGS_HOST_INIT_DONE) {
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
        if (self->host.flags & SDMMC_HOST_FLAG_DEINIT_ARG) {
            self->host.deinit_p(self->host.slot);
        } else
        #endif
        {
            self->host.deinit();
        }
        self->flags &= ~SDCARD_CARD_FLAGS_HOST_INIT_DONE;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_deinit_obj, sd_deinit);

STATIC mp_obj_t sd_info(mp_obj_t self_in) {
    sdcard_card_obj_t *self = self_in;
    // We could potential return a great deal more SD card data but it
    // is not clear that it is worth the extra code space to do
    // so. For the most part people only care about the card size and
    // block size.

    check_esp_err(sdcard_ensure_card_init((sdcard_card_obj_t *)self, false));

    uint32_t log_block_nbr = self->card.csd.capacity;
    uint32_t log_block_size = _SECTOR_SIZE(self);

    mp_obj_t tuple[2] = {
        mp_obj_new_int_from_ull((uint64_t)log_block_nbr * (uint64_t)log_block_size),
        mp_obj_new_int_from_uint(log_block_size),
    };
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sd_info_obj, sd_info);

STATIC mp_obj_t machine_sdcard_readblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    sdcard_card_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    esp_err_t err;

    err = sdcard_ensure_card_init((sdcard_card_obj_t *)self, false);
    if (err != ESP_OK) {
        return false;
    }

    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    err = sdmmc_read_sectors(&(self->card), bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / _SECTOR_SIZE(self));

    return mp_obj_new_bool(err == ESP_OK);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_readblocks_obj, machine_sdcard_readblocks);

STATIC mp_obj_t machine_sdcard_writeblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf) {
    sdcard_card_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    esp_err_t err;

    err = sdcard_ensure_card_init((sdcard_card_obj_t *)self, false);
    if (err != ESP_OK) {
        return false;
    }

    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    err = sdmmc_write_sectors(&(self->card), bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / _SECTOR_SIZE(self));

    return mp_obj_new_bool(err == ESP_OK);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_writeblocks_obj, machine_sdcard_writeblocks);

STATIC mp_obj_t machine_sdcard_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    sdcard_card_obj_t *self = self_in;
    esp_err_t err = ESP_OK;
    mp_int_t cmd = mp_obj_get_int(cmd_in);

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            err = sdcard_ensure_card_init(self, false);
            return MP_OBJ_NEW_SMALL_INT((err == ESP_OK) ? 0 : -1);

        case MP_BLOCKDEV_IOCTL_DEINIT:
            // Ensure that future attempts to look at info re-read the card
            self->flags &= ~SDCARD_CARD_FLAGS_CARD_INIT_DONE;
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case MP_BLOCKDEV_IOCTL_SYNC:
            // nothing to do
            return MP_OBJ_NEW_SMALL_INT(0); // success

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            err = sdcard_ensure_card_init(self, false);
            if (err != ESP_OK) {
                return MP_OBJ_NEW_SMALL_INT(-1);
            }
            return MP_OBJ_NEW_SMALL_INT(self->card.csd.capacity);

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            err = sdcard_ensure_card_init(self, false);
            if (err != ESP_OK) {
                return MP_OBJ_NEW_SMALL_INT(-1);
            }
            return MP_OBJ_NEW_SMALL_INT(_SECTOR_SIZE(self));

        default: // unknown command
            return MP_OBJ_NEW_SMALL_INT(-1); // error
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_sdcard_ioctl_obj, machine_sdcard_ioctl);

STATIC const mp_rom_map_elem_t machine_sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&sd_info_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&sd_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&sd_deinit_obj) },
    // block device protocol
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&machine_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&machine_sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&machine_sdcard_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_sdcard_locals_dict, machine_sdcard_locals_dict_table);

const mp_obj_type_t machine_sdcard_type = {
    { &mp_type_type },
    .name = MP_QSTR_SDCard,
    .make_new = machine_sdcard_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_sdcard_locals_dict,
};

#endif // MICROPY_HW_ENABLE_SDCARD
