/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "py/objarray.h"
#include "py/runtime.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/sharpdisplay/SharpMemoryFramebuffer.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/sharpdisplay/SharpMemoryFramebuffer.h"

STATIC mp_obj_t sharpdisplay_framebuffer_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_spi_bus, ARG_chip_select, ARG_width, ARG_height, ARG_baudrate, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi_bus, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_chip_select, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 2000000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mcu_pin_obj_t *chip_select = validate_obj_is_free_pin(args[ARG_chip_select].u_obj);
    busio_spi_obj_t *spi = validate_obj_is_spi_bus(args[ARG_spi_bus].u_obj);

    sharpdisplay_framebuffer_obj_t *self = &allocate_display_bus_or_raise()->sharpdisplay;
    self->base.type = &sharpdisplay_framebuffer_type;

    common_hal_sharpdisplay_framebuffer_construct(self, spi, chip_select, args[ARG_baudrate].u_int, args[ARG_width].u_int, args[ARG_height].u_int);

    return MP_OBJ_FROM_PTR(self);
}


STATIC mp_int_t sharpdisplay_framebuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    sharpdisplay_framebuffer_obj_t *self = (sharpdisplay_framebuffer_obj_t *)self_in;
    // a readonly framebuffer would be unusual but not impossible
    if ((flags & MP_BUFFER_WRITE) && !(self->bufinfo.typecode & MP_OBJ_ARRAY_TYPECODE_FLAG_RW)) {
        return 1;
    }
    *bufinfo = self->bufinfo;
    return 0;
}

STATIC mp_obj_t sharpdisplay_framebuffer_deinit(mp_obj_t self_in) {
    sharpdisplay_framebuffer_obj_t *self = (sharpdisplay_framebuffer_obj_t *)self_in;
    common_hal_sharpdisplay_framebuffer_deinit(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sharpdisplay_framebuffer_deinit_obj, sharpdisplay_framebuffer_deinit);

STATIC const mp_rom_map_elem_t sharpdisplay_framebuffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&sharpdisplay_framebuffer_deinit_obj) },
};
STATIC MP_DEFINE_CONST_DICT(sharpdisplay_framebuffer_locals_dict, sharpdisplay_framebuffer_locals_dict_table);

const mp_obj_type_t sharpdisplay_framebuffer_type = {
    { &mp_type_type },
    .name = MP_QSTR_SharpMemoryFramebuffer,
    .flags = MP_TYPE_FLAG_EXTENDED,
    .make_new = sharpdisplay_framebuffer_make_new,
    .locals_dict = (mp_obj_dict_t *)&sharpdisplay_framebuffer_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .buffer_p = { .get_buffer = sharpdisplay_framebuffer_get_buffer, },
        .protocol = &sharpdisplay_framebuffer_proto,
        ),
};
