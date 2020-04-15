/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "common-hal/protomatter/Protomatter.h"
#include "shared-bindings/protomatter/Protomatter.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/framebufferio/__init__.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

//| .. currentmodule:: protomatter
//|
//| :class:`Protomatter` --  Driver for HUB75-style RGB LED matrices
//| ================================================================
//|

extern Protomatter_core *_PM_protoPtr;

STATIC uint8_t validate_pin(mp_obj_t obj) {
    mcu_pin_obj_t *result = validate_obj_is_free_pin(obj);
    return common_hal_mcu_pin_number(result);
}

STATIC void validate_pins(qstr what, uint8_t* pin_nos, mp_int_t max_pins, mp_obj_t seq, uint8_t *count_out) {
    mp_int_t len = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(seq));
    if (len > max_pins) {
        mp_raise_ValueError_varg(translate("At most %d %q may be specified (not %d)"), max_pins, what, len);
    }
    *count_out = len;
    for (mp_int_t i=0; i<len; i++) {
        pin_nos[i] = validate_pin(mp_obj_subscr(seq, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));
    }
}

STATIC void claim_and_never_reset_pin(mp_obj_t pin) {
    common_hal_mcu_pin_claim(pin);
    common_hal_never_reset_pin(pin);
}

STATIC void claim_and_never_reset_pins(mp_obj_t seq) {
    mp_int_t len = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(seq));
    for (mp_int_t i=0; i<len; i++) {
        claim_and_never_reset_pin(mp_obj_subscr(seq, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));
    }
}

STATIC void preflight_pins_or_throw(uint8_t clock_pin, uint8_t *rgb_pins, uint8_t rgb_pin_count, bool allow_inefficient) {
    uint32_t port = clock_pin / 32;
    uint32_t bit_mask = 1 << (clock_pin % 32);

    for (uint8_t i = 0; i < rgb_pin_count; i++) {
        uint32_t pin_port = rgb_pins[i] / 32;

        if (pin_port != port) {
            mp_raise_ValueError_varg(
                translate("rgb_pins[%d] is not on the same port as clock"), i);
        }

        uint32_t pin_mask = 1 << (rgb_pins[i] % 32);
        if (pin_mask & bit_mask) {
            mp_raise_ValueError_varg(
                translate("rgb_pins[%d] duplicates another pin assignment"), i);
        }

        bit_mask |= pin_mask;
    }

    if (allow_inefficient) {
        return;
    }

    uint8_t byte_mask = 0;
    if (bit_mask & 0x000000FF) byte_mask |= 0b0001;
    if (bit_mask & 0x0000FF00) byte_mask |= 0b0010;
    if (bit_mask & 0x00FF0000) byte_mask |= 0b0100;
    if (bit_mask & 0xFF000000) byte_mask |= 0b1000;

    uint8_t bytes_per_element = 0xff;
    uint8_t ideal_bytes_per_element = (rgb_pin_count + 7) / 8;

    switch(byte_mask) {
        case 0b0001:
        case 0b0010:
        case 0b0100:
        case 0b1000:
            bytes_per_element = 1;
            break;

        case 0b0011:
        case 0b1100:
            bytes_per_element = 2;
            break;

        default:
            bytes_per_element = 4;
            break;
    }

    if (bytes_per_element != ideal_bytes_per_element) {
        mp_raise_ValueError_varg(
            translate("Pinout uses %d bytes per element, which consumes more than the ideal %d bytes.  If this cannot be avoided, pass allow_inefficient=True to the constructor"),
            bytes_per_element, ideal_bytes_per_element);
    }
}

//| :class:`~protomatter.Protomatter` displays an in-memory framebuffer to an LED matrix.
//|
//| .. class:: Protomatter(width, bit_depth, rgb_pins, addr_pins, clock_pin, latch_pin, output_enable_pin, *, doublebuffer=True, framebuffer=None)
//|
//|   Create a Protomatter object with the given attributes.  The height of
//|   the display is determined by the number of rgb and address pins:
//|   len(rgb_pins) // 3 * 2 ** len(address_pins).  With 6 RGB pins and 4
//|   address lines, the display will be 32 pixels tall.
//|
//|   Up to 30 RGB pins and 8 address pins are supported.
//|
//|   The RGB pins must be within a single "port" and performance and memory
//|   usage are best when they are all within "close by" bits of the port.
//|   The clock pin must also be on the same port as the RGB pins.  See the
//|   documentation of the underlying protomatter C library for more
//|   information.  Generally, Adafruit's interface boards are designed so
//|   that these requirements are met when matched with the intended
//|   microcontroller board.  For instance, the Feather M4 Express works
//|   together with the RGB Matrix Feather.
//|
//|   The framebuffer is in "RGB565" format.
//|
//|   "RGB565" means that it is organized as a series of 16-bit numbers
//|   where the highest 5 bits are interpreted as red, the next 6 as
//|   green, and the final 5 as blue.  The object can be any buffer, but
//|   `array.array` and `ulab.array` objects are most often useful.
//|   To update the content, modify the framebuffer and call refresh.
//|
//|   If a framebuffer is not passed in, one is allocated and initialized
//|   to all black.  In any case, the framebuffer can be retrieved
//|   by passing the protomatter object to memoryview().
//|
//|   If doublebuffer is False, some memory is saved, but the display may
//|   flicker during updates.
//|
//|   If a framebuffer is not passed in, one is allocated internally.  To
//|   retrieve it, pass the protomatter object to memoryview().
//|
//|   A Protomatter framebuffer is often used in conjunction with a
//|   `framebufferio.FramebufferDisplay`.
//|

STATIC mp_obj_t protomatter_protomatter_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_width, ARG_bit_depth, ARG_rgb_list, ARG_addr_list,
        ARG_clock_pin, ARG_latch_pin, ARG_output_enable_pin, ARG_doublebuffer, ARG_framebuffer, ARG_height };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_bit_depth, MP_ARG_INT | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_rgb_pins, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_addr_pins, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_clock_pin, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_latch_pin, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_output_enable_pin, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_doublebuffer, MP_ARG_BOOL | MP_ARG_KW_ONLY, { .u_bool = true } },
        { MP_QSTR_framebuffer, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = mp_const_none } },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 0 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Because interrupt handlers point directly at protomatter objects,
    // it is NOT okay to move them to the long-lived pool later.  Allocate
    // them there to begin with, since generally they'll be long-lived anyway.
    protomatter_protomatter_obj_t *self = &allocate_display_bus_or_raise()->protomatter;
    self->base.type = &protomatter_Protomatter_type;

    uint8_t rgb_count, addr_count;
    uint8_t rgb_pins[MP_ARRAY_SIZE(self->rgb_pins)];
    uint8_t addr_pins[MP_ARRAY_SIZE(self->addr_pins)];
    uint8_t clock_pin = validate_pin(args[ARG_clock_pin].u_obj);
    uint8_t latch_pin = validate_pin(args[ARG_latch_pin].u_obj);
    uint8_t output_enable_pin = validate_pin(args[ARG_output_enable_pin].u_obj);

    validate_pins(MP_QSTR_rgb_pins, rgb_pins, MP_ARRAY_SIZE(self->rgb_pins), args[ARG_rgb_list].u_obj, &rgb_count);
    validate_pins(MP_QSTR_addr_pins, addr_pins, MP_ARRAY_SIZE(self->addr_pins), args[ARG_addr_list].u_obj, &addr_count);

    if (rgb_count % 6) {
        mp_raise_ValueError_varg(translate("Must use a multiple of 6 rgb pins, not %d"), rgb_count);
    }

    // TODO(@jepler) Use fewer than all rows of pixels if height < computed_height
    if (args[ARG_height].u_int != 0) {
        int computed_height = (rgb_count / 3) << (addr_count);
        if (computed_height != args[ARG_height].u_int) {
            mp_raise_ValueError_varg(
                translate("%d address pins and %d rgb pins indicate a height of %d, not %d"), addr_count, rgb_count, computed_height, args[ARG_height].u_int);
        }
    }

    preflight_pins_or_throw(clock_pin, rgb_pins, rgb_count, true);

    mp_obj_t framebuffer = args[ARG_framebuffer].u_obj;
    if (framebuffer == mp_const_none) {
        int width = args[ARG_width].u_int;
        int bufsize = 2 * width * rgb_count / 3 * (1 << addr_count);
        framebuffer = mp_obj_new_bytearray_of_zeros(bufsize);
    }

    common_hal_protomatter_protomatter_construct(self,
        args[ARG_width].u_int,
        args[ARG_bit_depth].u_int,
        rgb_count, rgb_pins,
        addr_count, addr_pins,
        clock_pin, latch_pin, output_enable_pin,
        args[ARG_doublebuffer].u_bool,
        framebuffer, NULL);

    claim_and_never_reset_pins(args[ARG_rgb_list].u_obj);
    claim_and_never_reset_pins(args[ARG_addr_list].u_obj);
    claim_and_never_reset_pin(args[ARG_clock_pin].u_obj);
    claim_and_never_reset_pin(args[ARG_output_enable_pin].u_obj);
    claim_and_never_reset_pin(args[ARG_latch_pin].u_obj);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: deinit
//|
//|     Free the resources (pins, timers, etc.) associated with this
//|     protomatter instance.  After deinitialization, no further operations
//|     may be performed.
//|
STATIC mp_obj_t protomatter_protomatter_deinit(mp_obj_t self_in) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    common_hal_protomatter_protomatter_deinit(self);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(protomatter_protomatter_deinit_obj, protomatter_protomatter_deinit);

static void check_for_deinit(protomatter_protomatter_obj_t *self) {
    if (!self->core.rgbPins) {
        raise_deinited_error();
    }
}

//|   .. attribute:: brightness
//|
//|     In the current implementation, 0.0 turns the display off entirely
//|     and any other value up to 1.0 turns the display on fully.
//|
STATIC mp_obj_t protomatter_protomatter_get_brightness(mp_obj_t self_in) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_protomatter_protomatter_get_paused(self)? 0.0f : 1.0f);
}
MP_DEFINE_CONST_FUN_OBJ_1(protomatter_protomatter_get_brightness_obj, protomatter_protomatter_get_brightness);

STATIC mp_obj_t protomatter_protomatter_set_brightness(mp_obj_t self_in, mp_obj_t value_in)  {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    check_for_deinit(self);
    mp_float_t brightness = mp_obj_get_float(value_in);
    if (brightness < 0.0f || brightness > 1.0f) {
        mp_raise_ValueError(translate("Brightness must be 0-1.0"));
    }
    common_hal_protomatter_protomatter_set_paused(self_in, brightness <= 0);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(protomatter_protomatter_set_brightness_obj, protomatter_protomatter_set_brightness);

const mp_obj_property_t protomatter_protomatter_brightness_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&protomatter_protomatter_get_brightness_obj,
              (mp_obj_t)&protomatter_protomatter_set_brightness_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. method:: refresh()
//|
//|     Transmits the color data in the buffer to the pixels so that
//|     they are shown.
//|
STATIC mp_obj_t protomatter_protomatter_refresh(mp_obj_t self_in) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    check_for_deinit(self);
    common_hal_protomatter_protomatter_refresh(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(protomatter_protomatter_refresh_obj, protomatter_protomatter_refresh);

//|   .. attribute:: width
//|
//|     The width of the display, in pixels
//|
STATIC mp_obj_t protomatter_protomatter_get_width(mp_obj_t self_in) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_protomatter_protomatter_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(protomatter_protomatter_get_width_obj, protomatter_protomatter_get_width);
const mp_obj_property_t protomatter_protomatter_width_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&protomatter_protomatter_get_width_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: height
//|
//|     The height of the display, in pixels
//|
STATIC mp_obj_t protomatter_protomatter_get_height(mp_obj_t self_in) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_protomatter_protomatter_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(protomatter_protomatter_get_height_obj, protomatter_protomatter_get_height);

const mp_obj_property_t protomatter_protomatter_height_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&protomatter_protomatter_get_height_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: bytes_per_cell
//|
//|     The bytes_per_cell of the display, in pixels.  Always equal to 1.
//|
STATIC mp_obj_t protomatter_protomatter_get_bytes_per_cell(mp_obj_t self_in) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(1);
}
MP_DEFINE_CONST_FUN_OBJ_1(protomatter_protomatter_get_bytes_per_cell_obj, protomatter_protomatter_get_bytes_per_cell);

const mp_obj_property_t protomatter_protomatter_bytes_per_cell_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&protomatter_protomatter_get_bytes_per_cell_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: color_depth
//|
//|     The color_depth of the framebuffer, in bits.  Always equal to 16. This
//|     is different than the constructor's "bit_depth".
//|
STATIC mp_obj_t protomatter_protomatter_get_color_depth(mp_obj_t self_in) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(16);
}
MP_DEFINE_CONST_FUN_OBJ_1(protomatter_protomatter_get_color_depth_obj, protomatter_protomatter_get_color_depth);

const mp_obj_property_t protomatter_protomatter_color_depth_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&protomatter_protomatter_get_color_depth_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: native_frames_per_second
//|
//|     The native_frames_per_second of the display.  Always equal to 250.
//|
STATIC mp_obj_t protomatter_protomatter_get_native_frames_per_second(mp_obj_t self_in) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(250);
}
MP_DEFINE_CONST_FUN_OBJ_1(protomatter_protomatter_get_native_frames_per_second_obj, protomatter_protomatter_get_native_frames_per_second);

const mp_obj_property_t protomatter_protomatter_native_frames_per_second_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&protomatter_protomatter_get_native_frames_per_second_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


STATIC const mp_rom_map_elem_t protomatter_protomatter_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&protomatter_protomatter_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&protomatter_protomatter_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&protomatter_protomatter_refresh_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&protomatter_protomatter_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&protomatter_protomatter_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_depth), MP_ROM_PTR(&protomatter_protomatter_color_depth_obj) },
    { MP_ROM_QSTR(MP_QSTR_bytes_per_cell), MP_ROM_PTR(&protomatter_protomatter_bytes_per_cell_obj) },
    { MP_ROM_QSTR(MP_QSTR_native_frames_per_second), MP_ROM_PTR(&protomatter_protomatter_native_frames_per_second_obj) },
};
STATIC MP_DEFINE_CONST_DICT(protomatter_protomatter_locals_dict, protomatter_protomatter_locals_dict_table);

STATIC void protomatter_protomatter_get_bufinfo(mp_obj_t self_in, mp_buffer_info_t *bufinfo) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    check_for_deinit(self);

    *bufinfo = self->bufinfo;
}

// These version exists so that the prototype matches the protocol,
// avoiding a type cast that can hide errors
STATIC void protomatter_protomatter_swapbuffers(mp_obj_t self_in) {
    common_hal_protomatter_protomatter_refresh(self_in);
}

STATIC void protomatter_protomatter_deinit_proto(mp_obj_t self_in) {
    common_hal_protomatter_protomatter_deinit(self_in);
}

STATIC float protomatter_protomatter_get_brightness_proto(mp_obj_t self_in) {
    return common_hal_protomatter_protomatter_get_paused(self_in) ? 0.0f : 1.0f;
}

STATIC bool protomatter_protomatter_set_brightness_proto(mp_obj_t self_in, mp_float_t value) {
    common_hal_protomatter_protomatter_set_paused(self_in, value <= 0);
    return true;
}

STATIC const framebuffer_p_t protomatter_protomatter_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_framebuffer)
    .get_bufinfo = protomatter_protomatter_get_bufinfo,
    .set_brightness = protomatter_protomatter_set_brightness_proto,
    .get_brightness = protomatter_protomatter_get_brightness_proto,
    .swapbuffers = protomatter_protomatter_swapbuffers,
    .deinit = protomatter_protomatter_deinit_proto,
};

STATIC mp_int_t protomatter_protomatter_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    protomatter_protomatter_obj_t *self = (protomatter_protomatter_obj_t*)self_in;
    // a readonly framebuffer would be unusual but not impossible
    if ((flags & MP_BUFFER_WRITE) && !(self->bufinfo.typecode & MP_OBJ_ARRAY_TYPECODE_FLAG_RW)) {
        return 1;
    }
    *bufinfo = self->bufinfo;
    return 0;
}

const mp_obj_type_t protomatter_Protomatter_type = {
    { &mp_type_type },
    .name = MP_QSTR_Protomatter,
    .buffer_p = { .get_buffer = protomatter_protomatter_get_buffer, },
    .make_new = protomatter_protomatter_make_new,
    .protocol = &protomatter_protomatter_proto,
    .locals_dict = (mp_obj_dict_t*)&protomatter_protomatter_locals_dict,
};
