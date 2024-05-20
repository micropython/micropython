// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/max3421e/Max3421E.h"

#include "py/runtime.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"

//| class Max3421E:
//|     """Interface with a Max3421E usb host chip."""
//|
//|     def __init__(
//|         self,
//|         spi_bus: busio.SPI,
//|         *,
//|         chip_select: microcontroller.Pin,
//|         irq: microcontroller.Pin,
//|         baudrate: int = 26000000
//|     ) -> None:
//|         """Create a Max3421E object associated with the given pins.
//|
//|         Although this object isn't used directly for USB host (the `usb` module is).
//|         You must keep it alive in memory. When deinit, it will shut down USB host functionality.
//|
//|         :param busio.SPI spi_bus: The SPI bus that make up the clock and data lines
//|         :param microcontroller.Pin chip_select: Chip select pin
//|         :param microcontroller.Pin irq: Interrupt pin
//|         :param int baudrate: Maximum baudrate to talk to the Max chip in Hz"""
//|         ...
static mp_obj_t max3421e_max3421e_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_spi_bus, ARG_chip_select, ARG_irq, ARG_baudrate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_chip_select, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_irq, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_baudrate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 24000000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *chip_select = validate_obj_is_free_pin(args[ARG_chip_select].u_obj, MP_QSTR_chip_select);
    const mcu_pin_obj_t *irq = validate_obj_is_free_pin(args[ARG_irq].u_obj, MP_QSTR_irq);

    mp_obj_t spi = mp_arg_validate_type(args[ARG_spi_bus].u_obj, &busio_spi_type, MP_QSTR_spi_bus);

    max3421e_max3421e_obj_t *self = m_new_obj_with_finaliser(max3421e_max3421e_obj_t);
    self->base.type = &max3421e_max3421e_type;
    common_hal_max3421e_max3421e_construct(self,
        MP_OBJ_TO_PTR(spi), chip_select, irq, args[ARG_baudrate].u_int);
    return self;
}

//|     def deinit(self) -> None:
//|         """Shuts down USB host functionality and releases chip_select and irq pins."""
//|         ...
//|
static mp_obj_t max3421e_max3421e_obj_deinit(mp_obj_t self_in) {
    max3421e_max3421e_obj_t *self = self_in;
    if (common_hal_max3421e_max3421e_deinited(self)) {
        return mp_const_none;
    }
    common_hal_max3421e_max3421e_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(max3421e_max3421e_deinit_obj, max3421e_max3421e_obj_deinit);

static const mp_rom_map_elem_t max3421e_max3421e_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&max3421e_max3421e_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&max3421e_max3421e_deinit_obj) },
};
static MP_DEFINE_CONST_DICT(max3421e_max3421e_locals_dict, max3421e_max3421e_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    max3421e_max3421e_type,
    MP_QSTR_Max3421E,
    MP_TYPE_FLAG_NONE,
    make_new, max3421e_max3421e_make_new,
    locals_dict, &max3421e_max3421e_locals_dict
    );
