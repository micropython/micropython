/*
 * Copyright 2023 Cisco Systems, Inc. and its affiliates
 * Author: Wyrdsec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */
#include "py/objarray.h"
#include "py/runtime.h"
#include "py/objproperty.h"


#include "shared-bindings/aurora_epaper/aurora_framebuffer.h"
#include "shared-module/aurora_epaper/aurora_framebuffer.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"

//| class AuroraMemoryFramebuffer:
//|     """A framebuffer for Pervasive Displays Aurora E-paper displays.
//|
//|     These displays are 2 color only.
//|
//|     This initializes a display and connects it to CircuitPython.
//|
//|     For Example::
//|
//|         import busio
//|         import framebufferio
//|         from aurora_epaper import AuroraMemoryFramebuffer
//|         spi = busio.SPI(EINK_CLKS, EINK_MOSI, EINK_MISO)
//|         aurora = AuroraMemoryFramebuffer(spi, EINK_CS, EINK_RST, EINK_BUSY, EINK_DISCHARGE, HEIGHT, WIDTH)
//|         display = framebufferio.FramebufferDisplay(t, auto_refresh=False)
//|         display.refresh()
//|
//|     For more information on how these displays are driven see:
//|     https://www.pervasivedisplays.com/wp-content/uploads/2023/02/4P018-00_04_G2_Aurora-Mb_COG_Driver_Interface_Timing_for_small-size_20231107.pdf
//|     """
//|
//|     def __init__(
//|         self,
//|         spi_bus: busio.SPI,
//|         chip_select: microcontroller.Pin,
//|         reset: microcontroller.Pin,
//|         busy: microcontroller.Pin,
//|         discharge: microcontroller.Pin,
//|         width: int,
//|         height: int,
//|         power: Optional[microcontroller.Pin] = None,
//|         free_bus: Optional[bool] = True,
//|     ) -> None:
//|         """Create a framebuffer for the Aurora CoG display.
//|
//|         .. note:: Displays of size 1.9" and 2.6" are not tested, and may exibit unexpected behavior.
//|
//|         :param busio.SPI spi_bus: The SPI bus that the display is connected to
//|         :param microcontroller.Pin chip_select: The pin connected to the displays chip select input
//|         :param microcontroller.Pin reset: The pin connected to the displays reset input
//|         :param microcontroller.Pin busy: The pin connected to the displays busy output
//|         :param microcontroller.Pin discharge: The pin connected to the displays discharge input
//|         :param int width: The width of the display in pixels
//|         :param int height: The height of the display in pixels
//|         :param microcontroller.Pin power: The pin that controls power to the display (optional).
//|         :param bool free_bus: Determines whether the SPI bus passed in will be freed when the frame buffer is freed (optional).
//|         """
//|         ...
static mp_obj_t aurora_epaper_framebuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_spi_bus, ARG_chip_select, ARG_reset, ARG_busy, ARG_discharge, ARG_width, ARG_height, ARG_power, ARG_free_bus, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi_bus, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_chip_select, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_reset, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_busy, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_discharge, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_power, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_free_bus, MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);

    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Pins
    const mcu_pin_obj_t *chip_select = validate_obj_is_free_pin(args[ARG_chip_select].u_obj, MP_QSTR_chip_select);
    const mcu_pin_obj_t *reset = validate_obj_is_free_pin(args[ARG_reset].u_obj, MP_QSTR_reset);
    const mcu_pin_obj_t *busy = validate_obj_is_free_pin(args[ARG_busy].u_obj, MP_QSTR_busy);
    const mcu_pin_obj_t *discharge = validate_obj_is_free_pin(args[ARG_discharge].u_obj, MP_QSTR_discharge);
    const mcu_pin_obj_t *power = validate_obj_is_free_pin_or_none(args[ARG_power].u_obj, MP_QSTR_power);

    busio_spi_obj_t *spi = validate_obj_is_spi_bus(args[ARG_spi_bus].u_obj, MP_QSTR_spi_bus);
    aurora_epaper_framebuffer_obj_t *self = &allocate_display_bus_or_raise()->aurora_epaper;
    self->base.type = &aurora_framebuffer_type;

    common_hal_aurora_epaper_framebuffer_construct(self, spi, chip_select, reset, busy, discharge, power, args[ARG_width].u_int, args[ARG_height].u_int, args[ARG_free_bus].u_bool);
    return MP_OBJ_FROM_PTR(self);
}

static mp_int_t aurora_epaper_framebuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    aurora_epaper_framebuffer_obj_t *self = self_in;
    if ((flags & MP_BUFFER_WRITE) && !(self->bufinfo.typecode & MP_OBJ_ARRAY_TYPECODE_FLAG_RW)) {
        return 1;
    }
    *bufinfo = self->bufinfo;
    return 0;
}

//|     def deinit(self) -> None:
//|         """Free the resources (pins, timers, etc.) associated with this
//|         AuroraMemoryFramebuffer instance.  After deinitialization, no further operations
//|         may be performed."""
//|         ...
static mp_obj_t aurora_epaper_framebuffer_deinit(mp_obj_t self_in) {
    aurora_epaper_framebuffer_obj_t *self = (aurora_epaper_framebuffer_obj_t *)self_in;
    common_hal_aurora_epaper_framebuffer_deinit(self);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(aurora_epaper_framebuffer_deinit_obj, aurora_epaper_framebuffer_deinit);

//|     def set_temperature(self, celsius: int) -> None:
//|         """Set the ambient temperature (in celsius) for the display driver.
//|         Higher temperature means faster update speed.
//|         """
//|         ...
static mp_obj_t aurora_epaper_frambuffer_set_temperature(mp_obj_t self_in, mp_obj_t temperature) {
    aurora_epaper_framebuffer_obj_t *self = (aurora_epaper_framebuffer_obj_t *)self_in;

    common_hal_aurora_epaper_framebuffer_set_temperature(self, mp_obj_get_float(temperature));

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(aurora_epaper_frambuffer_set_temperature_obj, aurora_epaper_frambuffer_set_temperature);

//|     free_bus: bool
//|     """When True the spi bus passed into the device will be freed on deinit.
//|     If you have multiple displays this could be used to keep the other active on soft reset."""
//|     ...
//|
static mp_obj_t aurora_epaper_framebuffer_get_free_bus(mp_obj_t self_in) {
    aurora_epaper_framebuffer_obj_t *self = (aurora_epaper_framebuffer_obj_t *)self_in;
    return mp_obj_new_bool(self->free_bus);
}
static MP_DEFINE_CONST_FUN_OBJ_1(aurora_epaper_framebuffer_get_free_bus_obj, aurora_epaper_framebuffer_get_free_bus);


static mp_obj_t aurora_epaper_framebuffer_set_free_bus(mp_obj_t self_in, mp_obj_t free_bus) {
    aurora_epaper_framebuffer_obj_t *self = (aurora_epaper_framebuffer_obj_t *)self_in;
    common_hal_aurora_epaper_framebuffer_set_free_bus(self, mp_obj_is_true(free_bus));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(aurora_epaper_framebuffer_set_free_bus_obj, aurora_epaper_framebuffer_set_free_bus);

MP_PROPERTY_GETSET(aurora_epaper_framebuffer_free_bus_obj,
    (mp_obj_t)&aurora_epaper_framebuffer_get_free_bus_obj,
    (mp_obj_t)&aurora_epaper_framebuffer_set_free_bus_obj);

static const mp_rom_map_elem_t aurora_epaper_framebuffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&aurora_epaper_framebuffer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_temperature), MP_ROM_PTR(&aurora_epaper_frambuffer_set_temperature_obj) },
    { MP_ROM_QSTR(MP_QSTR_free_bus), MP_ROM_PTR(&aurora_epaper_framebuffer_free_bus_obj) },
};
static MP_DEFINE_CONST_DICT(aurora_epaper_framebuffer_locals_dict, aurora_epaper_framebuffer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    aurora_framebuffer_type,
    MP_QSTR_AuroraEpaperFramebuffer,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, aurora_epaper_framebuffer_make_new,
    locals_dict, &aurora_epaper_framebuffer_locals_dict,
    buffer, aurora_epaper_framebuffer_get_buffer,
    protocol, &aurora_epaper_framebuffer_proto
    );
