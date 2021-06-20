#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/vectorio/Circle.h"
#include "shared-bindings/vectorio/Polygon.h"
#include "shared-bindings/vectorio/Rectangle.h"
#include "shared-bindings/vectorio/VectorShape.h"

//| """Lightweight 2d shapes for displays"""
//|

STATIC const mp_rom_map_elem_t vectorio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_vectorio) },
    { MP_ROM_QSTR(MP_QSTR_Circle), MP_ROM_PTR(&vectorio_circle_type) },
    { MP_ROM_QSTR(MP_QSTR_Polygon), MP_ROM_PTR(&vectorio_polygon_type) },
    { MP_ROM_QSTR(MP_QSTR_Rectangle), MP_ROM_PTR(&vectorio_rectangle_type) },
    { MP_ROM_QSTR(MP_QSTR_VectorShape), MP_ROM_PTR(&vectorio_vector_shape_type) },
};

STATIC MP_DEFINE_CONST_DICT(vectorio_module_globals, vectorio_module_globals_table);

const mp_obj_module_t vectorio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&vectorio_module_globals,
};
