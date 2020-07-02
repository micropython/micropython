#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_RECTANGLE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_RECTANGLE_H

#include "shared-module/vectorio/Rectangle.h"
#include "shared-module/displayio/area.h"

extern const mp_obj_type_t vectorio_rectangle_type;

void common_hal_vectorio_rectangle_construct(vectorio_rectangle_t *self, uint32_t width, uint32_t height);

uint32_t common_hal_vectorio_rectangle_get_pixel(void *rectangle, int16_t x, int16_t y);

void common_hal_vectorio_rectangle_get_area(void *rectangle, displayio_area_t *out_area);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_RECTANGLE_H
