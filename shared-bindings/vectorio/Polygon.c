
#include "shared-module/vectorio/__init__.h"
#include "shared-bindings/vectorio/Polygon.h"

#include <stdint.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"


#define VECTORIO_POLYGON_DEBUG(...) (void)0
// #define VECTORIO_POLYGON_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)


//| class Polygon:
//|     def __init__(self, points: List[Tuple[int, int]]) -> None:
//|         """Represents a closed shape by ordered vertices
//|
//|            :param points: Vertices for the polygon"""
//|
static mp_obj_t vectorio_polygon_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_points_list };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_points, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!mp_obj_is_type(args[ARG_points_list].u_obj, &mp_type_list)) {
        mp_raise_TypeError_varg(translate("%q list must be a list"), MP_QSTR_point);
    }

    vectorio_polygon_t *self = m_new_obj(vectorio_polygon_t);
    self->base.type = &vectorio_polygon_type;

    common_hal_vectorio_polygon_construct(self, args[ARG_points_list].u_obj);

    return MP_OBJ_FROM_PTR(self);
}


//|     points: List[Tuple[int, int]]
//|     """Set a new look and shape for this polygon"""
//|
STATIC mp_obj_t vectorio_polygon_obj_get_points(mp_obj_t self_in) {
    vectorio_polygon_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_vectorio_polygon_get_points(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_polygon_get_points_obj, vectorio_polygon_obj_get_points);

STATIC mp_obj_t vectorio_polygon_obj_set_points(mp_obj_t self_in, mp_obj_t points) {
    vectorio_polygon_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_vectorio_polygon_set_points(self, points);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_polygon_set_points_obj, vectorio_polygon_obj_set_points);

const mp_obj_property_t vectorio_polygon_points_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&vectorio_polygon_get_points_obj,
              (mp_obj_t)&vectorio_polygon_set_points_obj,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t vectorio_polygon_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_points), MP_ROM_PTR(&vectorio_polygon_points_obj) },
};
STATIC MP_DEFINE_CONST_DICT(vectorio_polygon_locals_dict, vectorio_polygon_locals_dict_table);

const mp_obj_type_t vectorio_polygon_type = {
    { &mp_type_type },
    .name = MP_QSTR_Polygon,
    .make_new = vectorio_polygon_make_new,
    .locals_dict = (mp_obj_dict_t *)&vectorio_polygon_locals_dict,
};
