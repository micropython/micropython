
#include "shared-bindings/vectorio/Rectangle.h"

#include <stdint.h>

#include "py/objtype.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| class Rectangle:
//|     def __init__(self, width: int, height: int) -> None:
//|         """Represents a rectangle by defining its bounds
//|
//|            :param width: The number of pixels wide
//|            :param height: The number of pixels high"""
//|
static mp_obj_t vectorio_rectangle_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_width, ARG_height };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t width = args[ARG_width].u_int;
    if (width < 1) {
        mp_raise_ValueError_varg(translate("%q must be >= 1"), MP_QSTR_width);
    }
    mp_int_t height = args[ARG_height].u_int;
    if (height < 1) {
        mp_raise_ValueError_varg(translate("%q must be >= 1"), MP_QSTR_height);
    }

    vectorio_rectangle_t *self = m_new_obj(vectorio_rectangle_t);
    self->base.type = &vectorio_rectangle_type;
    common_hal_vectorio_rectangle_construct(self, width, height);

    return MP_OBJ_FROM_PTR(self);
}


STATIC const mp_rom_map_elem_t vectorio_rectangle_locals_dict_table[] = {
};
STATIC MP_DEFINE_CONST_DICT(vectorio_rectangle_locals_dict, vectorio_rectangle_locals_dict_table);

const mp_obj_type_t vectorio_rectangle_type = {
    { &mp_type_type },
    .name = MP_QSTR_Rectangle,
    .make_new = vectorio_rectangle_make_new,
    .locals_dict = (mp_obj_dict_t *)&vectorio_rectangle_locals_dict,
};
