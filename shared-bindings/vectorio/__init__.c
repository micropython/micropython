#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/vectorio/Circle.h"
#include "shared-bindings/vectorio/Polygon.h"
#include "shared-bindings/vectorio/Rectangle.h"

//| """Lightweight 2D shapes for displays
//|
//| The :py:attr:`vectorio` module provide simple filled drawing primitives for
//| use with `displayio`.
//|
//| .. code-block:: python
//|
//|     group = displayio.Group()
//|
//|     palette = displayio.Palette(1)
//|     palette[0] = 0x125690
//|
//|     circle = vectorio.Circle(pixel_shader=palette, radius=25, x=70, y=40)
//|     group.append(circle)
//|
//|     rectangle = vectorio.Rectangle(pixel_shader=palette, width=40, height=30, x=55, y=45)
//|     group.append(rectangle)
//|
//|     points=[(5, 5), (100, 20), (20, 20), (20, 100)]
//|     polygon = vectorio.Polygon(pixel_shader=palette, points=points, x=0, y=0)
//|     group.append(polygon)
//|
//| """

STATIC const mp_rom_map_elem_t vectorio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_vectorio) },
    { MP_ROM_QSTR(MP_QSTR_Circle), MP_ROM_PTR(&vectorio_circle_type) },
    { MP_ROM_QSTR(MP_QSTR_Polygon), MP_ROM_PTR(&vectorio_polygon_type) },
    { MP_ROM_QSTR(MP_QSTR_Rectangle), MP_ROM_PTR(&vectorio_rectangle_type) },
};

STATIC MP_DEFINE_CONST_DICT(vectorio_module_globals, vectorio_module_globals_table);

const mp_obj_module_t vectorio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&vectorio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_vectorio, vectorio_module, CIRCUITPY_VECTORIO);
