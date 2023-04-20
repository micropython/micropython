/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbert for Adafruit Industries
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

#include "py/obj.h"
#include "py/enum.h"
#include "py/runtime.h"

#include "shared-bindings/countio/Edge.h"

MAKE_ENUM_VALUE(countio_edge_type, edge, RISE, EDGE_RISE);
MAKE_ENUM_VALUE(countio_edge_type, edge, FALL, EDGE_FALL);
MAKE_ENUM_VALUE(countio_edge_type, edge, RISE_AND_FALL, EDGE_RISE_AND_FALL);

//| class Edge:
//|     """Enumerates which signal transitions can be counted."""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define which signal transitions to count."""
//|         ...
//|     RISE: Edge
//|     """Count the rising edges."""
//|
//|     FALL: Edge
//|     """Count the falling edges."""
//|
//|     RISE_AND_FALL: Edge
//|     """Count the rising and falling edges.
//|
//|     **Limitations:** ``RISE_AND_FALL`` is not available to RP2040 due to hardware limitations.
//|     """
//|
MAKE_ENUM_MAP(countio_edge) {
    MAKE_ENUM_MAP_ENTRY(edge, RISE),
    MAKE_ENUM_MAP_ENTRY(edge, FALL),
    MAKE_ENUM_MAP_ENTRY(edge, RISE_AND_FALL),
};

STATIC MP_DEFINE_CONST_DICT(countio_edge_locals_dict, countio_edge_locals_table);

MAKE_PRINTER(countio, countio_edge);

MAKE_ENUM_TYPE(countio, Edge, countio_edge);

countio_edge_t validate_edge(mp_obj_t obj, qstr arg_name) {
    return cp_enum_value(&countio_edge_type, obj, arg_name);
}
