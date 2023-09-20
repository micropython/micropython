#include "shared-bindings/vectorio/__init__.h"
#include "shared-bindings/vectorio/Rectangle.h"
#include "shared-module/vectorio/VectorShape.h"
#include "shared-bindings/vectorio/VectorShape.h"

#include <stdint.h>

#include "py/objtype.h"
#include "py/runtime.h"

//| class Rectangle:
//|     def __init__(
//|         self,
//|         pixel_shader: Union[displayio.ColorConverter, displayio.Palette],
//|         width: int,
//|         height: int,
//|         x: int,
//|         y: int,
//|     ) -> None:
//|         """Represents a rectangle by defining its bounds
//|
//|         :param Union[~displayio.ColorConverter,~displayio.Palette] pixel_shader: The pixel shader that produces colors from values
//|         :param int width: The number of pixels wide
//|         :param int height: The number of pixels high
//|         :param int x: Initial x position of the top left corner.
//|         :param int y: Initial y position of the top left corner.
//|         :param int color_index: Initial color_index to use when selecting color from the palette.
//|         """
static mp_obj_t vectorio_rectangle_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pixel_shader, ARG_width, ARG_height, ARG_x, ARG_y, ARG_color_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pixel_shader, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_color_index, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t width = args[ARG_width].u_int;
    mp_arg_validate_int_min(width, 1, MP_QSTR_width);
    mp_int_t height = args[ARG_height].u_int;
    mp_arg_validate_int_min(height, 1, MP_QSTR_height);

    vectorio_rectangle_t *self = mp_obj_malloc(vectorio_rectangle_t, &vectorio_rectangle_type);
    uint16_t color_index = args[ARG_color_index].u_int;
    common_hal_vectorio_rectangle_construct(self, width, height, color_index);

    // VectorShape parts
    mp_obj_t pixel_shader = args[ARG_pixel_shader].u_obj;
    int32_t x = args[ARG_x].u_int;
    int32_t y = args[ARG_y].u_int;
    mp_obj_t vector_shape = vectorio_vector_shape_make_new(self, pixel_shader, x, y);
    self->draw_protocol_instance = vector_shape;

    return MP_OBJ_FROM_PTR(self);
}

STATIC const vectorio_draw_protocol_t rectangle_draw_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_draw)
    .draw_get_protocol_self = (draw_get_protocol_self_fun)common_hal_vectorio_rectangle_get_draw_protocol,
    .draw_protocol_impl = &vectorio_vector_shape_draw_protocol_impl
};

//|     width: int
//|     """The width of the rectangle in pixels."""
STATIC mp_obj_t vectorio_rectangle_obj_get_width(mp_obj_t self_in) {
    vectorio_rectangle_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_vectorio_rectangle_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_rectangle_get_width_obj, vectorio_rectangle_obj_get_width);

STATIC mp_obj_t vectorio_rectangle_obj_set_width(mp_obj_t self_in, mp_obj_t width) {
    vectorio_rectangle_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_vectorio_rectangle_set_width(self, mp_obj_get_int(width));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_rectangle_set_width_obj, vectorio_rectangle_obj_set_width);

const mp_obj_property_t vectorio_rectangle_width_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&vectorio_rectangle_get_width_obj,
              (mp_obj_t)&vectorio_rectangle_set_width_obj,
              MP_ROM_NONE},
};

//|     height: int
//|     """The height of the rectangle in pixels."""
STATIC mp_obj_t vectorio_rectangle_obj_get_height(mp_obj_t self_in) {
    vectorio_rectangle_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_vectorio_rectangle_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_rectangle_get_height_obj, vectorio_rectangle_obj_get_height);

STATIC mp_obj_t vectorio_rectangle_obj_set_height(mp_obj_t self_in, mp_obj_t height) {
    vectorio_rectangle_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_vectorio_rectangle_set_height(self, mp_obj_get_int(height));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_rectangle_set_height_obj, vectorio_rectangle_obj_set_height);

const mp_obj_property_t vectorio_rectangle_height_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&vectorio_rectangle_get_height_obj,
              (mp_obj_t)&vectorio_rectangle_set_height_obj,
              MP_ROM_NONE},
};

//|     color_index: int
//|     """The color_index of the rectangle in 1 based index of the palette."""
STATIC mp_obj_t vectorio_rectangle_obj_get_color_index(mp_obj_t self_in) {
    vectorio_rectangle_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_vectorio_rectangle_get_color_index(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_rectangle_get_color_index_obj, vectorio_rectangle_obj_get_color_index);

STATIC mp_obj_t vectorio_rectangle_obj_set_color_index(mp_obj_t self_in, mp_obj_t color_index) {
    vectorio_rectangle_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_vectorio_rectangle_set_color_index(self, mp_obj_get_int(color_index));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_rectangle_set_color_index_obj, vectorio_rectangle_obj_set_color_index);

const mp_obj_property_t vectorio_rectangle_color_index_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&vectorio_rectangle_get_color_index_obj,
              (mp_obj_t)&vectorio_rectangle_set_color_index_obj,
              MP_ROM_NONE},
};

// Documentation for properties inherited from VectorShape.

//|     x: int
//|     """X position of the top left corner of the rectangle in the parent."""
//|
//|     y: int
//|     """Y position of the top left corner of the rectangle in the parent."""
//|
//|     hidden: bool
//|     """Hide the rectangle or not."""
//|
//|     location: Tuple[int, int]
//|     """(X,Y) position of the top left corner of the rectangle in the parent."""
//|
//|     pixel_shader: Union[displayio.ColorConverter, displayio.Palette]
//|     """The pixel shader of the rectangle."""
//|

STATIC const mp_rom_map_elem_t vectorio_rectangle_locals_dict_table[] = {
    // Functions
    { MP_ROM_QSTR(MP_QSTR_contains), MP_ROM_PTR(&vectorio_vector_shape_contains_obj) },
    // Properties
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&vectorio_vector_shape_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&vectorio_vector_shape_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_hidden), MP_ROM_PTR(&vectorio_vector_shape_hidden_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_index), MP_ROM_PTR(&vectorio_rectangle_color_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&vectorio_rectangle_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&vectorio_rectangle_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_location), MP_ROM_PTR(&vectorio_vector_shape_location_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_shader), MP_ROM_PTR(&vectorio_vector_shape_pixel_shader_obj) },
};
STATIC MP_DEFINE_CONST_DICT(vectorio_rectangle_locals_dict, vectorio_rectangle_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    vectorio_rectangle_type,
    MP_QSTR_Rectangle,
    MP_TYPE_FLAG_NONE,
    make_new, vectorio_rectangle_make_new,
    locals_dict, &vectorio_rectangle_locals_dict,
    protocol, &rectangle_draw_protocol
    );
