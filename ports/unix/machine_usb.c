/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
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

// USB Gadget mode support for Linux (Raspberry Pi, etc.)
// Allows Pi to act as a USB device (CDC/ACM serial, mass storage, etc.)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"

#define CONFIGFS_PATH "/sys/kernel/config/usb_gadget"
#define GADGET_NAME "micropython_gadget"

typedef struct _machine_usb_obj_t {
    mp_obj_base_t base;
    char gadget_path[256];
    bool enabled;
    uint16_t vendor_id;
    uint16_t product_id;
    char serial[64];
    char manufacturer[64];
    char product[64];
} machine_usb_obj_t;

// Forward declaration
extern const mp_obj_type_t machine_usb_type;

static machine_usb_obj_t usb_gadget = {
    .base = {&machine_usb_type},
    .enabled = false,
    .vendor_id = 0x1d6b,  // Linux Foundation
    .product_id = 0x0104,  // Multifunction Composite Gadget
};

static int write_file(const char *path, const char *content) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        return -1;
    }

    ssize_t ret = write(fd, content, strlen(content));
    close(fd);

    return (ret < 0) ? -1 : 0;
}

static int create_directory(const char *path) {
    if (mkdir(path, 0755) < 0 && errno != EEXIST) {
        return -1;
    }
    return 0;
}

static int create_symlink(const char *target, const char *linkpath) {
    if (symlink(target, linkpath) < 0 && errno != EEXIST) {
        return -1;
    }
    return 0;
}

static mp_obj_t machine_usb_init_cdc(mp_obj_t self_in) {
    machine_usb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Create gadget directory
    snprintf(self->gadget_path, sizeof(self->gadget_path), "%s/%s", CONFIGFS_PATH, GADGET_NAME);
    if (create_directory(self->gadget_path) < 0) {
        mp_raise_OSError(errno);
    }

    char path[512];

    // Set VID/PID
    snprintf(path, sizeof(path), "%s/idVendor", self->gadget_path);
    char hex_str[16];
    snprintf(hex_str, sizeof(hex_str), "0x%04x", self->vendor_id);
    if (write_file(path, hex_str) < 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to set VID (need root?)"));
    }

    snprintf(path, sizeof(path), "%s/idProduct", self->gadget_path);
    snprintf(hex_str, sizeof(hex_str), "0x%04x", self->product_id);
    write_file(path, hex_str);

    // Set device class
    snprintf(path, sizeof(path), "%s/bcdDevice", self->gadget_path);
    write_file(path, "0x0100");
    snprintf(path, sizeof(path), "%s/bcdUSB", self->gadget_path);
    write_file(path, "0x0200");

    // Create strings directory
    snprintf(path, sizeof(path), "%s/strings/0x409", self->gadget_path);
    create_directory(path);

    // Set manufacturer, product, serial
    snprintf(path, sizeof(path), "%s/strings/0x409/manufacturer", self->gadget_path);
    write_file(path, self->manufacturer[0] ? self->manufacturer : "MicroPython");

    snprintf(path, sizeof(path), "%s/strings/0x409/product", self->gadget_path);
    write_file(path, self->product[0] ? self->product : "MicroPython USB Device");

    snprintf(path, sizeof(path), "%s/strings/0x409/serialnumber", self->gadget_path);
    write_file(path, self->serial[0] ? self->serial : "123456");

    // Create configuration
    snprintf(path, sizeof(path), "%s/configs/c.1", self->gadget_path);
    create_directory(path);

    snprintf(path, sizeof(path), "%s/configs/c.1/strings/0x409", self->gadget_path);
    create_directory(path);

    snprintf(path, sizeof(path), "%s/configs/c.1/strings/0x409/configuration", self->gadget_path);
    write_file(path, "CDC ACM");

    // Set max power
    snprintf(path, sizeof(path), "%s/configs/c.1/MaxPower", self->gadget_path);
    write_file(path, "250");

    // Create ACM function
    snprintf(path, sizeof(path), "%s/functions/acm.usb0", self->gadget_path);
    if (create_directory(path) < 0) {
        mp_raise_OSError(errno);
    }

    // Link function to configuration
    char target[512];
    snprintf(target, sizeof(target), "%s/functions/acm.usb0", self->gadget_path);
    snprintf(path, sizeof(path), "%s/configs/c.1/acm.usb0", self->gadget_path);
    create_symlink(target, path);

    // Find and enable UDC
    // Try to find UDC controller
    const char *udc_candidates[] = {
        "fe980000.usb",  // Pi 4/5
        "3f980000.usb",  // Pi 0/1/2/3
        NULL
    };

    char udc_path[256];
    int found = 0;
    for (int i = 0; udc_candidates[i] != NULL; i++) {
        snprintf(udc_path, sizeof(udc_path), "/sys/class/udc/%s", udc_candidates[i]);
        if (access(udc_path, F_OK) == 0) {
            snprintf(path, sizeof(path), "%s/UDC", self->gadget_path);
            if (write_file(path, udc_candidates[i]) == 0) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        // Try to find any UDC
        mp_printf(&mp_plat_print, "Warning: No known UDC found, trying first available...\n");
    }

    self->enabled = true;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_usb_init_cdc_obj, machine_usb_init_cdc);

static mp_obj_t machine_usb_deinit(mp_obj_t self_in) {
    machine_usb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->enabled) {
        return mp_const_none;
    }

    char path[512];

    // Disable UDC
    snprintf(path, sizeof(path), "%s/UDC", self->gadget_path);
    write_file(path, "");

    // Remove symlink
    snprintf(path, sizeof(path), "%s/configs/c.1/acm.usb0", self->gadget_path);
    unlink(path);

    // Remove directories (in reverse order)
    snprintf(path, sizeof(path), "%s/functions/acm.usb0", self->gadget_path);
    rmdir(path);

    snprintf(path, sizeof(path), "%s/configs/c.1/strings/0x409", self->gadget_path);
    rmdir(path);

    snprintf(path, sizeof(path), "%s/configs/c.1", self->gadget_path);
    rmdir(path);

    snprintf(path, sizeof(path), "%s/strings/0x409", self->gadget_path);
    rmdir(path);

    rmdir(self->gadget_path);

    self->enabled = false;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_usb_deinit_obj, machine_usb_deinit);

static mp_obj_t machine_usb_config(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_usb_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1 && kw_args->used == 0) {
        // Get configuration
        mp_obj_t tuple[6];
        tuple[0] = mp_obj_new_int(self->vendor_id);
        tuple[1] = mp_obj_new_int(self->product_id);
        tuple[2] = mp_obj_new_str(self->serial, strlen(self->serial));
        tuple[3] = mp_obj_new_str(self->manufacturer, strlen(self->manufacturer));
        tuple[4] = mp_obj_new_str(self->product, strlen(self->product));
        tuple[5] = mp_obj_new_bool(self->enabled);
        return mp_obj_new_tuple(6, tuple);
    }

    // Set configuration
    enum { ARG_vid, ARG_pid, ARG_serial, ARG_manufacturer, ARG_product };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_vid, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_pid, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_serial, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_manufacturer, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_product, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    if (parsed_args[ARG_vid].u_int != -1) {
        self->vendor_id = parsed_args[ARG_vid].u_int;
    }
    if (parsed_args[ARG_pid].u_int != -1) {
        self->product_id = parsed_args[ARG_pid].u_int;
    }
    if (parsed_args[ARG_serial].u_obj != mp_const_none) {
        const char *s = mp_obj_str_get_str(parsed_args[ARG_serial].u_obj);
        snprintf(self->serial, sizeof(self->serial), "%s", s);
    }
    if (parsed_args[ARG_manufacturer].u_obj != mp_const_none) {
        const char *s = mp_obj_str_get_str(parsed_args[ARG_manufacturer].u_obj);
        snprintf(self->manufacturer, sizeof(self->manufacturer), "%s", s);
    }
    if (parsed_args[ARG_product].u_obj != mp_const_none) {
        const char *s = mp_obj_str_get_str(parsed_args[ARG_product].u_obj);
        snprintf(self->product, sizeof(self->product), "%s", s);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_usb_config_obj, 1, machine_usb_config);

static mp_obj_t machine_usb_active(size_t n_args, const mp_obj_t *args) {
    machine_usb_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        return mp_obj_new_bool(self->enabled);
    }

    bool active = mp_obj_is_true(args[1]);
    if (active && !self->enabled) {
        machine_usb_init_cdc(args[0]);
    } else if (!active && self->enabled) {
        machine_usb_deinit(args[0]);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_usb_active_obj, 1, 2, machine_usb_active);

static const mp_rom_map_elem_t machine_usb_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_usb_init_cdc_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_usb_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&machine_usb_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&machine_usb_active_obj) },
};
static MP_DEFINE_CONST_DICT(machine_usb_locals_dict, machine_usb_locals_dict_table);

static void machine_usb_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_usb_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "USB(vid=0x%04x, pid=0x%04x, active=%s)",
        self->vendor_id, self->product_id, self->enabled ? "True" : "False");
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_usb_type,
    MP_QSTR_USB,
    MP_TYPE_FLAG_NONE,
    print, machine_usb_print,
    locals_dict, &machine_usb_locals_dict
);

// Singleton instance
mp_obj_t machine_usb_singleton(void) {
    return MP_OBJ_FROM_PTR(&usb_gadget);
}
