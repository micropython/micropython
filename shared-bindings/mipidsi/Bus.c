// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/mipidsi/Bus.h"
#include "shared-bindings/util.h"
#include "shared/runtime/context_manager_helpers.h"

//| class Bus:
//|     def __init__(
//|         self,
//|         *,
//|         minimum_frequency: int = 500_000_000,
//|         num_lanes: int = 2,
//|     ) -> None:
//|         """Create a MIPI DSI Bus object.
//|
//|         This creates a DSI bus interface. The specific pins used are determined by the board.
//|         DSI supports 1-4 data lanes.
//|
//|         The lane bit rate is a bandwidth floor, not an exact clock. The bus runs at a
//|         rate the PHY can generate that is at least ``minimum_frequency``, choosing one
//|         close to it. Rates within 1 kHz of the minimum count, since the PHY and the
//|         reported pixel clock are quantized. The actual rate can be read back from
//|         `frequency`.
//|
//|         :param int minimum_frequency: the minimum lane bit rate in Hz (default 500 MHz)
//|         :param int num_lanes: the number of data lanes to use (default 2, range 1-4)
//|         :raises ValueError: if the PHY cannot reach ``minimum_frequency``
//|         """
//|
//
//
// All MCUs we support only have one DSI bus but it can be shared between multiple displays. One
// display may live longer than the VM, so we need to allocate the bus outside the VM. To simplify
// memory tracking, we use a global object for the bus.
//
static mipidsi_bus_obj_t _mipidsi_bus_obj;

static mp_obj_t mipidsi_bus_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_minimum_frequency, ARG_num_lanes };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_minimum_frequency, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 500000000} },
        { MP_QSTR_num_lanes, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 2} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    _mipidsi_bus_obj.base.type = &mipidsi_bus_type;
    mipidsi_bus_obj_t *self = &_mipidsi_bus_obj;

    mp_uint_t minimum_frequency = (mp_uint_t)mp_arg_validate_int_min(args[ARG_minimum_frequency].u_int, 1, MP_QSTR_minimum_frequency);
    uint8_t num_lanes = (uint8_t)mp_arg_validate_int_range(args[ARG_num_lanes].u_int, 1, 4, MP_QSTR_num_lanes);

    common_hal_mipidsi_bus_construct(self, minimum_frequency, num_lanes);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Free the resources (pins, timers, etc.) associated with this
//|         `mipidsi.Bus` instance.  After deinitialization, no further operations
//|         may be performed."""
//|         ...
//|
static mp_obj_t mipidsi_bus_deinit(mp_obj_t self_in) {
    mipidsi_bus_obj_t *self = (mipidsi_bus_obj_t *)self_in;
    common_hal_mipidsi_bus_deinit(self);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(mipidsi_bus_deinit_obj, mipidsi_bus_deinit);

static void check_for_deinit(mipidsi_bus_obj_t *self) {
    if (common_hal_mipidsi_bus_deinited(self)) {
        raise_deinited_error();
    }
}

//|     frequency: int
//|     """The lane bit rate the bus actually runs at, in Hz. This is at least the
//|     ``minimum_frequency`` passed to the constructor, up to the 1 kHz rounding
//|     tolerance, but can be higher because the PHY PLL only produces certain rates."""
static mp_obj_t mipidsi_bus_get_frequency(mp_obj_t self_in) {
    mipidsi_bus_obj_t *self = (mipidsi_bus_obj_t *)self_in;
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(common_hal_mipidsi_bus_get_frequency(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(mipidsi_bus_get_frequency_obj, mipidsi_bus_get_frequency);
MP_PROPERTY_GETTER(mipidsi_bus_frequency_obj,
    (mp_obj_t)&mipidsi_bus_get_frequency_obj);

//|     num_lanes: int
//|     """The number of data lanes the bus uses."""
//|
static mp_obj_t mipidsi_bus_get_num_lanes(mp_obj_t self_in) {
    mipidsi_bus_obj_t *self = (mipidsi_bus_obj_t *)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_mipidsi_bus_get_num_lanes(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(mipidsi_bus_get_num_lanes_obj, mipidsi_bus_get_num_lanes);
MP_PROPERTY_GETTER(mipidsi_bus_num_lanes_obj,
    (mp_obj_t)&mipidsi_bus_get_num_lanes_obj);

static const mp_rom_map_elem_t mipidsi_bus_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mipidsi_bus_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mipidsi_bus_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&mipidsi_bus_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_num_lanes), MP_ROM_PTR(&mipidsi_bus_num_lanes_obj) },
};
static MP_DEFINE_CONST_DICT(mipidsi_bus_locals_dict, mipidsi_bus_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mipidsi_bus_type,
    MP_QSTR_Bus,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, mipidsi_bus_make_new,
    locals_dict, &mipidsi_bus_locals_dict
    );
