
#include "shared-bindings/vectorio/Circle.h"


#include <stdint.h>

#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| class Circle:
//|
//|     def __init__(self, radius: int) -> None:
//|         """Circle is positioned on screen by its center point.
//|
//|            :param radius: The radius of the circle in pixels"""
//|
static mp_obj_t vectorio_circle_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_radius };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_radius, MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t radius = args[ARG_radius].u_int;
    if (radius < 1) {
        mp_raise_ValueError_varg(translate("%q must be >= 1"), MP_QSTR_radius);
    }

    vectorio_circle_t *self = m_new_obj(vectorio_circle_t);
    self->base.type = &vectorio_circle_type;
    common_hal_vectorio_circle_construct(self, radius);

    return MP_OBJ_FROM_PTR(self);
}


//|     radius : int
//|     """The radius of the circle in pixels."""
//|
STATIC mp_obj_t vectorio_circle_obj_get_radius(mp_obj_t self_in) {
    vectorio_circle_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_vectorio_circle_get_radius(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_circle_get_radius_obj, vectorio_circle_obj_get_radius);

STATIC mp_obj_t vectorio_circle_obj_set_radius(mp_obj_t self_in, mp_obj_t radius) {
    vectorio_circle_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_vectorio_circle_set_radius(self, mp_obj_get_int(radius));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_circle_set_radius_obj, vectorio_circle_obj_set_radius);

const mp_obj_property_t vectorio_circle_radius_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&vectorio_circle_get_radius_obj,
              (mp_obj_t)&vectorio_circle_set_radius_obj,
              MP_ROM_NONE},
};


STATIC const mp_rom_map_elem_t vectorio_circle_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_radius), MP_ROM_PTR(&vectorio_circle_radius_obj) },
};
STATIC MP_DEFINE_CONST_DICT(vectorio_circle_locals_dict, vectorio_circle_locals_dict_table);

const mp_obj_type_t vectorio_circle_type = {
    { &mp_type_type },
    .name = MP_QSTR_Circle,
    .make_new = vectorio_circle_make_new,
    .locals_dict = (mp_obj_dict_t *)&vectorio_circle_locals_dict,
};
