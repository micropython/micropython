#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_CIRCLE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_CIRCLE_H

#include "shared-module/vectorio/__init__.h"
#include "shared-module/vectorio/Circle.h"
#include "shared-module/displayio/area.h"

extern const mp_obj_type_t vectorio_circle_type;

void common_hal_vectorio_circle_construct(vectorio_circle_t *self, uint16_t radius);

void common_hal_vectorio_circle_set_on_dirty(vectorio_circle_t *self, vectorio_event_t notification);

uint32_t common_hal_vectorio_circle_get_pixel(void *circle, int16_t x, int16_t y);

void common_hal_vectorio_circle_get_area(void *circle, displayio_area_t *out_area);


int16_t common_hal_vectorio_circle_get_radius(void *circle);
void common_hal_vectorio_circle_set_radius(void *circle, int16_t radius);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_CIRCLE_H
