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

//| class SharpMemoryFramebuffer:
//|     """A framebuffer for a memory-in-pixel display. Sharp makes monochrome displays and JDI used
//|     to make 8-color displays.
//|
//|     This initializes a display and connects it into CircuitPython. Unlike other
//|     objects in CircuitPython, Display objects live until `displayio.release_displays()`
//|     is called. This is done so that CircuitPython can use the display itself."""
//|
//|     def __init__(
//|         self,
//|         spi_bus: busio.SPI,
//|         chip_select: microcontroller.Pin,
//|         width: int,
//|         height: int,
//|         baudrate: int = 2000000,
//|         jdi_display: bool = False,
//|     ) -> None:
//|         """Create a framebuffer for the memory-in-pixel display.
//|
//|         :param busio.SPI spi_bus: The SPI bus that the display is connected to
//|         :param microcontroller.Pin chip_select: The pin connect to the display's chip select line
//|         :param int width: The width of the display in pixels
//|         :param int height: The height of the display in pixels
//|         :param int baudrate: The baudrate to communicate with the screen at
//|         :param bool jdi_display: When True, work with an 8-color JDI display. Otherwise, a monochrome Sharp display.
//|         """
//|         ...
STATIC mp_obj_t sharpdisplay_framebuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_spi_bus, ARG_chip_select, ARG_width, ARG_height, ARG_baudrate, ARG_jdi_display, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi_bus, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_chip_select, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 2000000} },
        { MP_QSTR_jdi_display, MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);

    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *chip_select = validate_obj_is_free_pin(args[ARG_chip_select].u_obj, MP_QSTR_chip_select);
    busio_spi_obj_t *spi = validate_obj_is_spi_bus(args[ARG_spi_bus].u_obj, MP_QSTR_spi_bus);

    sharpdisplay_framebuffer_obj_t *self = &allocate_display_bus_or_raise()->sharpdisplay;
    self->base.type = &sharpdisplay_framebuffer_type;

    common_hal_sharpdisplay_framebuffer_construct(self, spi, chip_select, args[ARG_baudrate].u_int, args[ARG_width].u_int, args[ARG_height].u_int, args[ARG_jdi_display].u_bool);

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

//|     def deinit(self) -> None:
//|         """Free the resources (pins, timers, etc.) associated with this
//|         SharpMemoryFramebuffer instance.  After deinitialization, no further operations
//|         may be performed."""
//|         ...
//|
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

MP_DEFINE_CONST_OBJ_TYPE(
    sharpdisplay_framebuffer_type,
    MP_QSTR_SharpMemoryFramebuffer,
    MP_TYPE_FLAG_NONE,
    make_new, sharpdisplay_framebuffer_make_new,
    locals_dict, &sharpdisplay_framebuffer_locals_dict,
    buffer, sharpdisplay_framebuffer_get_buffer,
    protocol, &sharpdisplay_framebuffer_proto
    );
