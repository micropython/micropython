
#include "shared-module/vectorio/__init__.h"
#include "shared-bindings/vectorio/VectorShape.h"
#include "shared-bindings/vectorio/Circle.h"
#include "shared-bindings/vectorio/Polygon.h"
#include "shared-bindings/vectorio/Rectangle.h"

#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Palette.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"

#include "py/binary.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"


//| class VectorShape:
//|     def __init__(self, shape: Union[Polygon, Rectangle, Circle], pixel_shader: Union[displayio.ColorConverter, displayio.Palette], x: int=0, y: int=0) -> None:
//|         """Binds a vector shape to a location and pixel color
//|
//|            :param shape: The shape to draw.
//|            :param pixel_shader: The pixel shader that produces colors from values
//|            :param x: Initial x position of the center axis of the shape within the parent.
//|            :param y: Initial y position of the center axis of the shape within the parent."""
//|         ...
//|
STATIC mp_obj_t vectorio_vector_shape_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_shape, ARG_pixel_shader, ARG_x, ARG_y };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_shape, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_pixel_shader, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_x, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t pixel_shader = args[ARG_pixel_shader].u_obj;
    if (!mp_obj_is_type(pixel_shader, &displayio_colorconverter_type) &&
        !mp_obj_is_type(pixel_shader, &displayio_palette_type)) {
        mp_raise_TypeError_varg(translate("unsupported %q type"), MP_QSTR_pixel_shader);
    }

    int16_t x = args[ARG_x].u_int;
    int16_t y = args[ARG_y].u_int;

    mp_obj_t shape = args[ARG_shape].u_obj;
    vectorio_ishape_t ishape;
    // Wire up shape functions
    if (mp_obj_is_type(shape, &vectorio_polygon_type)) {
        ishape.shape = shape;
        ishape.get_area = &common_hal_vectorio_polygon_get_area;
        ishape.get_pixel = &common_hal_vectorio_polygon_get_pixel;
    } else if (mp_obj_is_type(shape, &vectorio_rectangle_type)) {
        ishape.shape = shape;
        ishape.get_area = &common_hal_vectorio_rectangle_get_area;
        ishape.get_pixel = &common_hal_vectorio_rectangle_get_pixel;
    } else if (mp_obj_is_type(shape, &vectorio_circle_type)) {
        ishape.shape = shape;
        ishape.get_area = &common_hal_vectorio_circle_get_area;
        ishape.get_pixel = &common_hal_vectorio_circle_get_pixel;
    } else {
        mp_raise_TypeError_varg(translate("unsupported %q type"), MP_QSTR_shape);
    }

    vectorio_vector_shape_t *self = m_new_obj(vectorio_vector_shape_t);
    self->base.type = &vectorio_vector_shape_type;
    common_hal_vectorio_vector_shape_construct(self,
        ishape, pixel_shader, x, y
        );

    // Wire up event callbacks
    vectorio_event_t on_dirty = {
        .obj = self,
        .event = &common_hal_vectorio_vector_shape_set_dirty
    };

    if (mp_obj_is_type(shape, &vectorio_polygon_type)) {
        common_hal_vectorio_polygon_set_on_dirty(self->ishape.shape, on_dirty);
    } else if (mp_obj_is_type(shape, &vectorio_rectangle_type)) {
    } else if (mp_obj_is_type(shape, &vectorio_circle_type)) {
        common_hal_vectorio_circle_set_on_dirty(self->ishape.shape, on_dirty);
    } else {
        mp_raise_TypeError_varg(translate("unsupported %q type"), MP_QSTR_shape);
    }

    return MP_OBJ_FROM_PTR(self);
}


//|     x: int
//|     """X position of the center point of the shape in the parent."""
//|
STATIC mp_obj_t vectorio_vector_shape_obj_get_x(mp_obj_t self_in) {
    vectorio_vector_shape_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_vectorio_vector_shape_get_x(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_vector_shape_get_x_obj, vectorio_vector_shape_obj_get_x);

STATIC mp_obj_t vectorio_vector_shape_obj_set_x(mp_obj_t self_in, mp_obj_t x_obj) {
    vectorio_vector_shape_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t x = mp_obj_get_int(x_obj);
    common_hal_vectorio_vector_shape_set_x(self, x);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_vector_shape_set_x_obj, vectorio_vector_shape_obj_set_x);

const mp_obj_property_t vectorio_vector_shape_x_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&vectorio_vector_shape_get_x_obj,
              (mp_obj_t)&vectorio_vector_shape_set_x_obj,
              MP_ROM_NONE},
};


//|     y: int
//|     """Y position of the center point of the shape in the parent."""
//|
STATIC mp_obj_t vectorio_vector_shape_obj_get_y(mp_obj_t self_in) {
    vectorio_vector_shape_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_vectorio_vector_shape_get_y(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_vector_shape_get_y_obj, vectorio_vector_shape_obj_get_y);

STATIC mp_obj_t vectorio_vector_shape_obj_set_y(mp_obj_t self_in, mp_obj_t y_obj) {
    vectorio_vector_shape_t *self = MP_OBJ_TO_PTR(self_in);

    mp_int_t y = mp_obj_get_int(y_obj);
    common_hal_vectorio_vector_shape_set_y(self, y);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_vector_shape_set_y_obj, vectorio_vector_shape_obj_set_y);

const mp_obj_property_t vectorio_vector_shape_y_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&vectorio_vector_shape_get_y_obj,
              (mp_obj_t)&vectorio_vector_shape_set_y_obj,
              MP_ROM_NONE},
};


//|     pixel_shader: Union[displayio.ColorConverter, displayio.Palette]
//|     """The pixel shader of the shape."""
//|
STATIC mp_obj_t vectorio_vector_shape_obj_get_pixel_shader(mp_obj_t self_in) {
    vectorio_vector_shape_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_vectorio_vector_shape_get_pixel_shader(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_vector_shape_get_pixel_shader_obj, vectorio_vector_shape_obj_get_pixel_shader);

STATIC mp_obj_t vectorio_vector_shape_obj_set_pixel_shader(mp_obj_t self_in, mp_obj_t pixel_shader) {
    vectorio_vector_shape_t *self = MP_OBJ_TO_PTR(self_in);
    if (!mp_obj_is_type(pixel_shader, &displayio_palette_type) && !mp_obj_is_type(pixel_shader, &displayio_colorconverter_type)) {
        mp_raise_TypeError(translate("pixel_shader must be displayio.Palette or displayio.ColorConverter"));
    }

    common_hal_vectorio_vector_shape_set_pixel_shader(self, pixel_shader);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_vector_shape_set_pixel_shader_obj, vectorio_vector_shape_obj_set_pixel_shader);

const mp_obj_property_t vectorio_vector_shape_pixel_shader_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&vectorio_vector_shape_get_pixel_shader_obj,
              (mp_obj_t)&vectorio_vector_shape_set_pixel_shader_obj,
              MP_ROM_NONE},
};


STATIC const mp_rom_map_elem_t vectorio_vector_shape_locals_dict_table[] = {
    // Properties
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&vectorio_vector_shape_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&vectorio_vector_shape_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_shader), MP_ROM_PTR(&vectorio_vector_shape_pixel_shader_obj) },
};
STATIC MP_DEFINE_CONST_DICT(vectorio_vector_shape_locals_dict, vectorio_vector_shape_locals_dict_table);

const mp_obj_type_t vectorio_vector_shape_type = {
    { &mp_type_type },
    .name = MP_QSTR_VectorShape,
    .make_new = vectorio_vector_shape_make_new,
    .locals_dict = (mp_obj_dict_t *)&vectorio_vector_shape_locals_dict,
};
