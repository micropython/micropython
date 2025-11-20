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

// PWM support for Linux using sysfs /sys/class/pwm interface
// This provides machine.PWM functionality for Raspberry Pi and other Linux SBCs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    uint8_t chip;
    uint8_t channel;
    uint32_t freq;
    uint16_t duty_u16;
    bool is_exported;
} machine_pwm_obj_t;

static void write_sysfs(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        mp_raise_OSError(errno);
    }
    ssize_t ret = write(fd, value, strlen(value));
    close(fd);
    if (ret < 0) {
        mp_raise_OSError(errno);
    }
}

static void write_sysfs_int(const char *path, int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    write_sysfs(path, buf);
}

static void machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(chip=%u, channel=%u, freq=%u, duty_u16=%u)",
        self->chip, self->channel, self->freq, self->duty_u16);
}

static mp_obj_t machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Parse arguments
    enum { ARG_pin, ARG_freq, ARG_duty_u16, ARG_chip, ARG_channel };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32768} },
        { MP_QSTR_chip, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    // Create PWM object
    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);

    // For now, use chip and channel directly
    // In the future, could map from pin number to chip/channel
    self->chip = parsed_args[ARG_chip].u_int;
    self->channel = parsed_args[ARG_channel].u_int;
    self->freq = parsed_args[ARG_freq].u_int;
    self->duty_u16 = parsed_args[ARG_duty_u16].u_int;
    self->is_exported = false;

    // Export PWM channel
    char path[256];
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/export", self->chip);
    char channel_str[8];
    snprintf(channel_str, sizeof(channel_str), "%d", self->channel);

    // Try to export (may fail if already exported, which is okay)
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, channel_str, strlen(channel_str));
        close(fd);
        usleep(100000);  // Wait for sysfs files to appear
        self->is_exported = true;
    }

    // Set period (1/freq in nanoseconds)
    uint64_t period_ns = 1000000000ULL / self->freq;
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/period", self->chip, self->channel);
    write_sysfs_int(path, period_ns);

    // Set duty cycle
    uint64_t duty_ns = (period_ns * self->duty_u16) / 65535;
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", self->chip, self->channel);
    write_sysfs_int(path, duty_ns);

    // Enable PWM
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/enable", self->chip, self->channel);
    write_sysfs(path, "1");

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_pwm_freq(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // Get frequency
        return MP_OBJ_NEW_SMALL_INT(self->freq);
    } else {
        // Set frequency
        self->freq = mp_obj_get_int(args[1]);

        uint64_t period_ns = 1000000000ULL / self->freq;
        char path[256];
        snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/period", self->chip, self->channel);
        write_sysfs_int(path, period_ns);

        // Update duty cycle to maintain duty ratio
        uint64_t duty_ns = (period_ns * self->duty_u16) / 65535;
        snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", self->chip, self->channel);
        write_sysfs_int(path, duty_ns);

        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_freq_obj, 1, 2, machine_pwm_freq);

static mp_obj_t machine_pwm_duty_u16(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // Get duty cycle
        return MP_OBJ_NEW_SMALL_INT(self->duty_u16);
    } else {
        // Set duty cycle
        self->duty_u16 = mp_obj_get_int(args[1]);

        uint64_t period_ns = 1000000000ULL / self->freq;
        uint64_t duty_ns = (period_ns * self->duty_u16) / 65535;
        char path[256];
        snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", self->chip, self->channel);
        write_sysfs_int(path, duty_ns);

        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_duty_u16_obj, 1, 2, machine_pwm_duty_u16);

static mp_obj_t machine_pwm_deinit(mp_obj_t self_in) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Disable PWM
    char path[256];
    snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/pwm%d/enable", self->chip, self->channel);
    write_sysfs(path, "0");

    // Unexport if we exported it
    if (self->is_exported) {
        snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip%d/unexport", self->chip);
        char channel_str[8];
        snprintf(channel_str, sizeof(channel_str), "%d", self->channel);
        write_sysfs(path, channel_str);
        self->is_exported = false;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pwm_deinit_obj, machine_pwm_deinit);

static const mp_rom_map_elem_t machine_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty_u16), MP_ROM_PTR(&machine_pwm_duty_u16_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_pwm_deinit_obj) },
};
static MP_DEFINE_CONST_DICT(machine_pwm_locals_dict, machine_pwm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pwm_type,
    MP_QSTR_PWM,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pwm_make_new,
    print, machine_pwm_print,
    locals_dict, &machine_pwm_locals_dict
);
