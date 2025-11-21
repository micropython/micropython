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

// GPIO support for Linux using sysfs or gpiochip character device
// This provides machine.Pin functionality for Raspberry Pi and other Linux SBCs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"

// GPIO chip device (default for Raspberry Pi 5 is gpiochip4, Pi 4 is gpiochip0)
#ifndef MICROPY_HW_GPIO_CHIP
#define MICROPY_HW_GPIO_CHIP "/dev/gpiochip4"
#endif

#define GPIO_MODE_IN  (0)
#define GPIO_MODE_OUT (1)
#define GPIO_PULL_NONE (0)
#define GPIO_PULL_UP   (1)
#define GPIO_PULL_DOWN (2)

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint32_t id;
    int fd;          // file descriptor for this pin's line
    uint8_t mode;
    uint8_t pull;
    uint8_t value;
} machine_pin_obj_t;

static machine_pin_obj_t *machine_pin_obj_all[64] = {0};  // Support up to 64 GPIO pins

static int gpio_chip_fd = -1;

// Open the GPIO chip device
static int open_gpio_chip(void) {
    if (gpio_chip_fd < 0) {
        gpio_chip_fd = open(MICROPY_HW_GPIO_CHIP, O_RDWR);
        if (gpio_chip_fd < 0) {
            // Try alternate chip for Raspberry Pi 4 and earlier
            gpio_chip_fd = open("/dev/gpiochip0", O_RDWR);
        }
    }
    return gpio_chip_fd;
}

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Pin(%u, mode=%s, pull=%s, value=%u)",
        self->id,
        self->mode == GPIO_MODE_IN ? "IN" : "OUT",
        self->pull == GPIO_PULL_UP ? "PULL_UP" :
            (self->pull == GPIO_PULL_DOWN ? "PULL_DOWN" : "NONE"),
        self->value);
}

static mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_mode].u_obj != mp_const_none) {
        self->mode = mp_obj_get_int(args[ARG_mode].u_obj);
    }

    if (args[ARG_pull].u_obj != mp_const_none) {
        self->pull = mp_obj_get_int(args[ARG_pull].u_obj);
    }

    // Request the GPIO line from the kernel
    int chip_fd = open_gpio_chip();
    if (chip_fd < 0) {
        mp_raise_OSError(errno);
    }

    // Close existing line if already open
    if (self->fd >= 0) {
        close(self->fd);
        self->fd = -1;
    }

    struct gpiohandle_request req;
    memset(&req, 0, sizeof(req));
    req.lineoffsets[0] = self->id;
    req.lines = 1;

    if (self->mode == GPIO_MODE_IN) {
        req.flags = GPIOHANDLE_REQUEST_INPUT;
    } else {
        req.flags = GPIOHANDLE_REQUEST_OUTPUT;
        if (args[ARG_value].u_obj != mp_const_none) {
            self->value = mp_obj_is_true(args[ARG_value].u_obj);
            req.default_values[0] = self->value;
        }
    }

    // Note: Pull up/down configuration via GPIO character device requires newer kernel (5.5+)
    // For older kernels, this would need to be done via device tree or other means

    snprintf(req.consumer_label, sizeof(req.consumer_label), "micropython");

    if (ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0) {
        mp_raise_OSError(errno);
    }

    self->fd = req.fd;

    return mp_const_none;
}

static mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get pin id
    int pin_id = mp_obj_get_int(args[0]);
    if (pin_id < 0 || pin_id >= 64) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }

    // Get or create pin object
    machine_pin_obj_t *self = machine_pin_obj_all[pin_id];
    if (self == NULL) {
        self = mp_obj_malloc(machine_pin_obj_t, &machine_pin_type);
        self->id = pin_id;
        self->fd = -1;
        self->mode = GPIO_MODE_IN;
        self->pull = GPIO_PULL_NONE;
        self->value = 0;
        machine_pin_obj_all[pin_id] = self;
    }

    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_pin_obj_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (n_args == 0) {
        // Get pin value
        struct gpiohandle_data data;
        if (ioctl(self->fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
            mp_raise_OSError(errno);
        }
        self->value = data.values[0];
        return MP_OBJ_NEW_SMALL_INT(self->value);
    } else {
        // Set pin value
        self->value = mp_obj_is_true(args[0]);
        struct gpiohandle_data data;
        data.values[0] = self->value;
        if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
            mp_raise_OSError(errno);
        }
        return mp_const_none;
    }
}

static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_obj_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->value = 1;
    struct gpiohandle_data data;
    data.values[0] = 1;
    if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->value = 0;
    struct gpiohandle_data data;
    data.values[0] = 0;
    if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Read current value
    struct gpiohandle_data data;
    if (ioctl(self->fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
        mp_raise_OSError(errno);
    }

    // Toggle it
    self->value = !data.values[0];
    data.values[0] = self->value;

    if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        mp_raise_OSError(errno);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // Instance methods
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&machine_pin_toggle_obj) },

    // Class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(GPIO_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(GPIO_MODE_OUT) },
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE), MP_ROM_INT(GPIO_PULL_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULL_DOWN) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    switch (request) {
        case MP_PIN_READ: {
            struct gpiohandle_data data;
            if (ioctl(self->fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
                return -1;
            }
            return data.values[0];
        }
        case MP_PIN_WRITE: {
            struct gpiohandle_data data;
            data.values[0] = arg;
            if (ioctl(self->fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
                return -1;
            }
            return 0;
        }
    }
    return -1;
}

static const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_make_new,
    print, machine_pin_print,
    call, machine_pin_obj_call,
    protocol, &pin_pin_p,
    locals_dict, &machine_pin_locals_dict
);
