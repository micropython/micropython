#include "shared-bindings/vectorio/__init__.h"
#include "shared-bindings/vectorio/Circle.h"
#include "shared-bindings/vectorio/VectorShape.h"


#include <stdint.h>

#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"

//| class Circle:
//|     def __init__(
//|         self,
//|         pixel_shader: Union[displayio.ColorConverter, displayio.Palette],
//|         radius: int,
//|         x: int,
//|         y: int,
//|     ) -> None:
//|         """Circle is positioned on screen by its center point.
//|
//|         :param Union[~displayio.ColorConverter,~displayio.Palette] pixel_shader: The pixel shader that produces colors from values
//|         :param int radius: The radius of the circle in pixels
//|         :param int x: Initial x position of the axis.
//|         :param int y: Initial y position of the axis.
//|         :param int color_index: Initial color_index to use when selecting color from the palette.
//|         """
static mp_obj_t vectorio_circle_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pixel_shader, ARG_radius, ARG_x, ARG_y, ARG_color_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pixel_shader, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_radius, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_color_index, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t radius = args[ARG_radius].u_int;
    mp_arg_validate_int_min(radius, 1, MP_QSTR_radius);

    vectorio_circle_t *self = mp_obj_malloc(vectorio_circle_t, &vectorio_circle_type);
    uint16_t color_index = args[ARG_color_index].u_int;
    common_hal_vectorio_circle_construct(self, radius, color_index);

    // VectorShape parts
    mp_obj_t pixel_shader = args[ARG_pixel_shader].u_obj;
    int32_t x = args[ARG_x].u_int;
    int32_t y = args[ARG_y].u_int;
    mp_obj_t vector_shape = vectorio_vector_shape_make_new(self, pixel_shader, x, y);
    self->draw_protocol_instance = vector_shape;

    return MP_OBJ_FROM_PTR(self);
}

STATIC const vectorio_draw_protocol_t circle_draw_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_draw)
    .draw_get_protocol_self = (draw_get_protocol_self_fun)common_hal_vectorio_circle_get_draw_protocol,
    .draw_protocol_impl = &vectorio_vector_shape_draw_protocol_impl
};


//|     radius: int
//|     """The radius of the circle in pixels."""
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

MP_PROPERTY_GETSET(vectorio_circle_radius_obj,
    (mp_obj_t)&vectorio_circle_get_radius_obj,
    (mp_obj_t)&vectorio_circle_set_radius_obj);

//|     color_index: int
//|     """The color_index of the circle as 0 based index of the palette."""
STATIC mp_obj_t vectorio_circle_obj_get_color_index(mp_obj_t self_in) {
    vectorio_circle_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_vectorio_circle_get_color_index(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_circle_get_color_index_obj, vectorio_circle_obj_get_color_index);

STATIC mp_obj_t vectorio_circle_obj_set_color_index(mp_obj_t self_in, mp_obj_t color_index) {
    vectorio_circle_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_vectorio_circle_set_color_index(self, mp_obj_get_int(color_index));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_circle_set_color_index_obj, vectorio_circle_obj_set_color_index);

MP_PROPERTY_GETSET(vectorio_circle_color_index_obj,
    (mp_obj_t)&vectorio_circle_get_color_index_obj,
    (mp_obj_t)&vectorio_circle_set_color_index_obj);


// Documentation for properties inherited from VectorShape.

//|     x: int
//|     """X position of the center point of the circle in the parent."""
//|
//|     y: int
//|     """Y position of the center point of the circle in the parent."""
//|
//|     hidden: bool
//|     """Hide the circle or not."""
//|
//|     location: Tuple[int, int]
//|     """(X,Y) position of the center point of the circle in the parent."""
//|
//|     pixel_shader: Union[displayio.ColorConverter, displayio.Palette]
//|     """The pixel shader of the circle."""
//|

STATIC const mp_rom_map_elem_t vectorio_circle_locals_dict_table[] = {
    // Functions
    { MP_ROM_QSTR(MP_QSTR_contains), MP_ROM_PTR(&vectorio_vector_shape_contains_obj) },
    // Properties
    { MP_ROM_QSTR(MP_QSTR_radius), MP_ROM_PTR(&vectorio_circle_radius_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&vectorio_vector_shape_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&vectorio_vector_shape_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_hidden), MP_ROM_PTR(&vectorio_vector_shape_hidden_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_index), MP_ROM_PTR(&vectorio_circle_color_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_location), MP_ROM_PTR(&vectorio_vector_shape_location_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_shader), MP_ROM_PTR(&vectorio_vector_shape_pixel_shader_obj) },
};
STATIC MP_DEFINE_CONST_DICT(vectorio_circle_locals_dict, vectorio_circle_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    vectorio_circle_type,
    MP_QSTR_Circle,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, vectorio_circle_make_new,
    locals_dict, &vectorio_circle_locals_dict,
    protocol, &circle_draw_protocol
    );
