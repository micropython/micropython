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

// Audio support for Linux using ALSA (simple PCM interface)
// Provides machine.Audio functionality for microphone/speaker

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

// Tentative declaration
extern const mp_obj_type_t machine_audio_type;

// Note: For full ALSA support, link with -lasound
// This is a simplified version using direct device access

#define DEFAULT_RATE 44100
#define DEFAULT_CHANNELS 2
#define DEFAULT_FORMAT 16  // 16-bit PCM

typedef struct _machine_audio_obj_t {
    mp_obj_base_t base;
    int fd;
    uint32_t rate;
    uint8_t channels;
    uint8_t bits;
    bool is_input;  // true=microphone, false=speaker
    bool is_open;
} machine_audio_obj_t;

static void machine_audio_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_audio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Audio(%s, rate=%u, channels=%u, bits=%u)",
        self->is_input ? "INPUT" : "OUTPUT",
        self->rate, self->channels, self->bits);
}

static mp_obj_t machine_audio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    // Parse arguments
    enum { ARG_mode, ARG_rate, ARG_channels, ARG_bits };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },  // 0=output, 1=input
        { MP_QSTR_rate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_RATE} },
        { MP_QSTR_channels, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_CHANNELS} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_FORMAT} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    // Create audio object
    machine_audio_obj_t *self = mp_obj_malloc(machine_audio_obj_t, &machine_audio_type);
    self->is_input = (parsed_args[ARG_mode].u_int == 1);
    self->rate = parsed_args[ARG_rate].u_int;
    self->channels = parsed_args[ARG_channels].u_int;
    self->bits = parsed_args[ARG_bits].u_int;
    self->is_open = false;

    // Open audio device (simplified - real implementation would use ALSA API)
    const char *device = self->is_input ? "/dev/dsp" : "/dev/dsp";  // OSS device
    self->fd = open(device, self->is_input ? O_RDONLY : O_WRONLY);

    if (self->fd < 0) {
        // If OSS not available, use arecord/aplay as fallback
        mp_printf(&mp_plat_print, "Note: Direct audio device access not available.\n");
        mp_printf(&mp_plat_print, "Use .record()/.play() methods which use arecord/aplay.\n");
        self->fd = -1;
    }

    self->is_open = (self->fd >= 0);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_audio_record(size_t n_args, const mp_obj_t *args) {
    machine_audio_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int duration_ms = mp_obj_get_int(args[1]);  // Duration in milliseconds

    if (!self->is_input) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Audio not configured for input"));
    }

    // Calculate buffer size
    size_t samples = (self->rate * duration_ms) / 1000;
    size_t buffer_size = samples * self->channels * (self->bits / 8);

    // Use arecord command as fallback
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
        "arecord -d %d -f S16_LE -c %d -r %d -t raw 2>/dev/null",
        duration_ms / 1000 + 1, self->channels, self->rate);

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        mp_raise_OSError(errno);
    }

    vstr_t vstr;
    vstr_init(&vstr, buffer_size);

    size_t bytes_read = fread(vstr.buf, 1, buffer_size, fp);
    vstr.len = bytes_read;

    pclose(fp);

    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_audio_record_obj, 2, 2, machine_audio_record);

static mp_obj_t machine_audio_play(mp_obj_t self_in, mp_obj_t data_in) {
    machine_audio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->is_input) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Audio not configured for output"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

    // Use aplay command as fallback
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
        "aplay -f S16_LE -c %d -r %d -t raw 2>/dev/null",
        self->channels, self->rate);

    FILE *fp = popen(cmd, "w");
    if (!fp) {
        mp_raise_OSError(errno);
    }

    fwrite(bufinfo.buf, 1, bufinfo.len, fp);
    pclose(fp);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_audio_play_obj, machine_audio_play);

static mp_obj_t machine_audio_deinit(mp_obj_t self_in) {
    machine_audio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->is_open && self->fd >= 0) {
        close(self->fd);
        self->fd = -1;
        self->is_open = false;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_audio_deinit_obj, machine_audio_deinit);

static const mp_rom_map_elem_t machine_audio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_record), MP_ROM_PTR(&machine_audio_record_obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&machine_audio_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_audio_deinit_obj) },

    // Mode constants
    { MP_ROM_QSTR(MP_QSTR_OUTPUT), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_INPUT), MP_ROM_INT(1) },
};
static MP_DEFINE_CONST_DICT(machine_audio_locals_dict, machine_audio_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_audio_type,
    MP_QSTR_Audio,
    MP_TYPE_FLAG_NONE,
    make_new, machine_audio_make_new,
    print, machine_audio_print,
    locals_dict, &machine_audio_locals_dict
);
