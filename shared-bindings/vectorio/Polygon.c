#include "shared-bindings/vectorio/__init__.h"
#include "shared-module/vectorio/__init__.h"
#include "shared-bindings/vectorio/Polygon.h"
#include "shared-bindings/vectorio/VectorShape.h"

#include <stdint.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "supervisor/shared/translate/translate.h"


#define VECTORIO_POLYGON_DEBUG(...) (void)0
// #define VECTORIO_POLYGON_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)


//| class Polygon:
//|     def __init__(
//|         self,
//|         pixel_shader: Union[displayio.ColorConverter, displayio.Palette],
//|         points: List[Tuple[int, int]],
//|         x: int,
//|         y: int,
//|     ) -> None:
//|         """Represents a closed shape by ordered vertices. The path will be treated as
//|         'closed', the last point will connect to the first point.
//|
//|         :param Union[~displayio.ColorConverter,~displayio.Palette] pixel_shader: The pixel
//|             shader that produces colors from values
//|         :param List[Tuple[int,int]] points: Vertices for the polygon
//|         :param int x: Initial screen x position of the 0,0 origin in the points list.
//|         :param int y: Initial screen y position of the 0,0 origin in the points list.
//|         :param int color_index: Initial color_index to use when selecting color from the palette.
//|         """
static mp_obj_t vectorio_polygon_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pixel_shader, ARG_points_list, ARG_x, ARG_y, ARG_color_index };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pixel_shader, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_points, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_x, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_color_index, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t points_list = mp_arg_validate_type(args[ARG_points_list].u_obj, &mp_type_list, MP_QSTR_points);

    vectorio_polygon_t *self = mp_obj_malloc(vectorio_polygon_t, &vectorio_polygon_type);

    uint16_t color_index = args[ARG_color_index].u_int;
    common_hal_vectorio_polygon_construct(self, points_list, color_index);

    // VectorShape parts
    mp_obj_t pixel_shader = args[ARG_pixel_shader].u_obj;
    int32_t x = args[ARG_x].u_int;
    int32_t y = args[ARG_y].u_int;
    mp_obj_t vector_shape = vectorio_vector_shape_make_new(self, pixel_shader, x, y);
    self->draw_protocol_instance = vector_shape;

    return MP_OBJ_FROM_PTR(self);
}

STATIC const vectorio_draw_protocol_t polygon_draw_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_draw)
    .draw_get_protocol_self = (draw_get_protocol_self_fun)common_hal_vectorio_polygon_get_draw_protocol,
    .draw_protocol_impl = &vectorio_vector_shape_draw_protocol_impl
};


//|     points: List[Tuple[int, int]]
//|     """Vertices for the polygon."""
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

MP_PROPERTY_GETSET(vectorio_polygon_points_obj,
    (mp_obj_t)&vectorio_polygon_get_points_obj,
    (mp_obj_t)&vectorio_polygon_set_points_obj);

//|     color_index: int
//|     """The color_index of the polygon as 0 based index of the palette."""
STATIC mp_obj_t vectorio_polygon_obj_get_color_index(mp_obj_t self_in) {
    vectorio_polygon_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_vectorio_polygon_get_color_index(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(vectorio_polygon_get_color_index_obj, vectorio_polygon_obj_get_color_index);

STATIC mp_obj_t vectorio_polygon_obj_set_color_index(mp_obj_t self_in, mp_obj_t color_index) {
    vectorio_polygon_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_vectorio_polygon_set_color_index(self, mp_obj_get_int(color_index));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(vectorio_polygon_set_color_index_obj, vectorio_polygon_obj_set_color_index);

MP_PROPERTY_GETSET(vectorio_polygon_color_index_obj,
    (mp_obj_t)&vectorio_polygon_get_color_index_obj,
    (mp_obj_t)&vectorio_polygon_set_color_index_obj);


// Documentation for properties inherited from VectorShape.

//|     x: int
//|     """X position of the 0,0 origin in the points list."""
//|
//|     y: int
//|     """Y position of the 0,0 origin in the points list."""
//|
//|     hidden: bool
//|     """Hide the polygon or not."""
//|
//|     location: Tuple[int, int]
//|     """(X,Y) position of the 0,0 origin in the points list."""
//|
//|     pixel_shader: Union[displayio.ColorConverter, displayio.Palette]
//|     """The pixel shader of the polygon."""
//|

STATIC const mp_rom_map_elem_t vectorio_polygon_locals_dict_table[] = {
    // Functions
    { MP_ROM_QSTR(MP_QSTR_contains), MP_ROM_PTR(&vectorio_vector_shape_contains_obj) },
    // Properties
    { MP_ROM_QSTR(MP_QSTR_points), MP_ROM_PTR(&vectorio_polygon_points_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&vectorio_vector_shape_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&vectorio_vector_shape_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_hidden), MP_ROM_PTR(&vectorio_vector_shape_hidden_obj) },
    { MP_ROM_QSTR(MP_QSTR_color_index), MP_ROM_PTR(&vectorio_polygon_color_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_location), MP_ROM_PTR(&vectorio_vector_shape_location_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel_shader), MP_ROM_PTR(&vectorio_vector_shape_pixel_shader_obj) },
};
STATIC MP_DEFINE_CONST_DICT(vectorio_polygon_locals_dict, vectorio_polygon_locals_dict_table);

const mp_obj_type_t vectorio_polygon_type = {
    { &mp_type_type },
    .name = MP_QSTR_Polygon,
    .flags = MP_TYPE_FLAG_EXTENDED,
    .make_new = vectorio_polygon_make_new,
    .locals_dict = (mp_obj_dict_t *)&vectorio_polygon_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &polygon_draw_protocol,
        ),
};
