#ifndef MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_RECTANGLE_H
#define MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_RECTANGLE_H

#include <stdint.h>

#include "py/obj.h"
#include "shared-module/vectorio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    uint16_t width;
    uint16_t height;
    uint16_t color_number;
    vectorio_event_t on_dirty;
    mp_obj_t draw_protocol_instance;
} vectorio_rectangle_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_RECTANGLE_H
