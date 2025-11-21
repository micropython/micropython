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

// Display support for Linux using framebuffer
// Provides machine.Display functionality for Raspberry Pi and other Linux SBCs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

// Tentative declaration
extern const mp_obj_type_t machine_display_type;

typedef struct _machine_display_obj_t {
    mp_obj_base_t base;
    int fd;
    uint8_t *framebuffer;
    size_t fb_size;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;  // bits per pixel
    bool is_open;
} machine_display_obj_t;

static void machine_display_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Display(%ux%u, %ubpp, open=%s)",
        self->width, self->height, self->bpp, self->is_open ? "True" : "False");
}

static mp_obj_t machine_display_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    // Parse arguments
    enum { ARG_device };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    // Create display object
    machine_display_obj_t *self = mp_obj_malloc(machine_display_obj_t, &machine_display_type);
    self->framebuffer = NULL;
    self->is_open = false;

    // Open framebuffer device
    char device[32];
    snprintf(device, sizeof(device), "/dev/fb%ld", (long)parsed_args[ARG_device].u_int);
    self->fd = open(device, O_RDWR);
    if (self->fd < 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Cannot open %s: %s"), device, strerror(errno));
    }

    // Get variable screen info
    struct fb_var_screeninfo vinfo;
    if (ioctl(self->fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    // Get fixed screen info
    struct fb_fix_screeninfo finfo;
    if (ioctl(self->fd, FBIOGET_FSCREENINFO, &finfo) < 0) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    self->width = vinfo.xres;
    self->height = vinfo.yres;
    self->bpp = vinfo.bits_per_pixel;
    self->fb_size = finfo.smem_len;

    // Map framebuffer to memory
    self->framebuffer = (uint8_t *)mmap(0, self->fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, self->fd, 0);
    if (self->framebuffer == MAP_FAILED) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    self->is_open = true;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_display_fill(mp_obj_t self_in, mp_obj_t color_in) {
    machine_display_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t color = mp_obj_get_int(color_in);

    if (!self->is_open) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Display not open"));
    }

    // Fill framebuffer with color
    // Assumes 32bpp (RGBA) or 24bpp (RGB)
    if (self->bpp == 32) {
        uint32_t *fb32 = (uint32_t *)self->framebuffer;
        size_t pixels = self->fb_size / 4;
        for (size_t i = 0; i < pixels; i++) {
            fb32[i] = color;
        }
    } else if (self->bpp == 24) {
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        for (size_t i = 0; i < self->fb_size; i += 3) {
            self->framebuffer[i] = b;
            self->framebuffer[i + 1] = g;
            self->framebuffer[i + 2] = r;
        }
    } else if (self->bpp == 16) {
        uint16_t *fb16 = (uint16_t *)self->framebuffer;
        uint16_t color16 = ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
        size_t pixels = self->fb_size / 2;
        for (size_t i = 0; i < pixels; i++) {
            fb16[i] = color16;
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_display_fill_obj, machine_display_fill);

static mp_obj_t machine_display_pixel(size_t n_args, const mp_obj_t *args) {
    machine_display_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);
    uint32_t color = mp_obj_get_int(args[3]);

    if (!self->is_open) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Display not open"));
    }

    if (x < 0 || x >= (int)self->width || y < 0 || y >= (int)self->height) {
        return mp_const_none;  // Ignore out of bounds
    }

    size_t offset = (y * self->width + x) * (self->bpp / 8);

    if (self->bpp == 32) {
        *((uint32_t *)(self->framebuffer + offset)) = color;
    } else if (self->bpp == 24) {
        self->framebuffer[offset] = color & 0xFF;
        self->framebuffer[offset + 1] = (color >> 8) & 0xFF;
        self->framebuffer[offset + 2] = (color >> 16) & 0xFF;
    } else if (self->bpp == 16) {
        uint16_t color16 = ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
        *((uint16_t *)(self->framebuffer + offset)) = color16;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_display_pixel_obj, 4, 4, machine_display_pixel);

static mp_obj_t machine_display_blit(size_t n_args, const mp_obj_t *args) {
    machine_display_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    int x = mp_obj_get_int(args[2]);
    int y = mp_obj_get_int(args[3]);
    int w = mp_obj_get_int(args[4]);
    int h = mp_obj_get_int(args[5]);

    if (!self->is_open) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Display not open"));
    }

    // Simple blit implementation
    size_t bytes_per_pixel = self->bpp / 8;
    for (int row = 0; row < h; row++) {
        if (y + row < 0 || y + row >= (int)self->height) continue;

        for (int col = 0; col < w; col++) {
            if (x + col < 0 || x + col >= (int)self->width) continue;

            size_t src_offset = (row * w + col) * bytes_per_pixel;
            size_t dst_offset = ((y + row) * self->width + (x + col)) * bytes_per_pixel;

            if (src_offset + bytes_per_pixel <= bufinfo.len && dst_offset + bytes_per_pixel <= self->fb_size) {
                memcpy(self->framebuffer + dst_offset, (uint8_t *)bufinfo.buf + src_offset, bytes_per_pixel);
            }
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_display_blit_obj, 6, 6, machine_display_blit);

static mp_obj_t machine_display_deinit(mp_obj_t self_in) {
    machine_display_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->framebuffer && self->framebuffer != MAP_FAILED) {
        munmap(self->framebuffer, self->fb_size);
        self->framebuffer = NULL;
    }

    if (self->is_open) {
        close(self->fd);
        self->is_open = false;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_display_deinit_obj, machine_display_deinit);

static const mp_rom_map_elem_t machine_display_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&machine_display_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&machine_display_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&machine_display_blit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_display_deinit_obj) },
};
static MP_DEFINE_CONST_DICT(machine_display_locals_dict, machine_display_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_display_type,
    MP_QSTR_Display,
    MP_TYPE_FLAG_NONE,
    make_new, machine_display_make_new,
    print, machine_display_print,
    locals_dict, &machine_display_locals_dict
);
