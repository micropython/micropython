#ifndef MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_CIRCLE_H
#define MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_CIRCLE_H

#include <stdint.h>

#include "py/obj.h"

#include "shared-module/vectorio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    uint16_t radius;
    vectorio_event_t on_dirty;
} vectorio_circle_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_CIRCLE_H
