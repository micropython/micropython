/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 by kvc0/WarriorOfWire
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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

MP_REGISTER_MODULE(MP_QSTR_vectorio, vectorio_module);
