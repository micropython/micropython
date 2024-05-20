// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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

static MP_DEFINE_CONST_DICT(countio_edge_locals_dict, countio_edge_locals_table);

MAKE_PRINTER(countio, countio_edge);

MAKE_ENUM_TYPE(countio, Edge, countio_edge);

countio_edge_t validate_edge(mp_obj_t obj, qstr arg_name) {
    return cp_enum_value(&countio_edge_type, obj, arg_name);
}
