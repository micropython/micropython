#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_POLYGON_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_POLYGON_H

#include "shared-module/vectorio/Polygon.h"
#include "shared-module/displayio/area.h"
#include "shared-module/vectorio/__init__.h"

extern const mp_obj_type_t vectorio_polygon_type;

void common_hal_vectorio_polygon_construct(vectorio_polygon_t *self, mp_obj_t points_list);
void common_hal_vectorio_polygon_set_on_dirty(vectorio_polygon_t *self, vectorio_event_t notification);


uint32_t common_hal_vectorio_polygon_get_pixel(void *polygon, int16_t x, int16_t y);

void common_hal_vectorio_polygon_get_area(void *polygon, displayio_area_t *out_area);



mp_obj_t common_hal_vectorio_polygon_get_points(vectorio_polygon_t *self);
void common_hal_vectorio_polygon_set_points(vectorio_polygon_t *self, mp_obj_t points_list);


#endif // MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_POLYGON_H
