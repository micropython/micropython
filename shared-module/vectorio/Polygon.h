#ifndef MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_POLYGON_H
#define MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_POLYGON_H

#include <stdint.h>

#include "py/obj.h"
#include "shared-module/vectorio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    // A micropython List[ x, y, ... ]
    mp_obj_t points_list;
    vectorio_event_t on_dirty;
} vectorio_polygon_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_POLYGON_H
