#ifndef MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_POLYGON_H
#define MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_POLYGON_H

#include <stdint.h>

#include "py/obj.h"
#include "shared-module/vectorio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    // An int array[ x, y, ... ]
    int16_t *points_list;
    uint16_t len;
    uint16_t color_index;
    vectorio_event_t on_dirty;
    mp_obj_t draw_protocol_instance;
} vectorio_polygon_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_POLYGON_H
