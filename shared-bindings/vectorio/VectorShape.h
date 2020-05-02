#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_SHAPE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_SHAPE_H

#include "shared-module/vectorio/VectorShape.h"

extern const mp_obj_type_t vectorio_vector_shape_type;

void common_hal_vectorio_vector_shape_construct(vectorio_vector_shape_t *self,
        vectorio_ishape_t ishape,
        mp_obj_t pixel_shader, uint16_t x, uint16_t y, bool transpose_xy);

void common_hal_vectorio_vector_shape_set_dirty(void *self);

mp_int_t common_hal_vectorio_vector_shape_get_x(vectorio_vector_shape_t *self);
void common_hal_vectorio_vector_shape_set_x(vectorio_vector_shape_t *self, mp_int_t x);

mp_int_t common_hal_vectorio_vector_shape_get_y(vectorio_vector_shape_t *self);
void common_hal_vectorio_vector_shape_set_y(vectorio_vector_shape_t *self, mp_int_t y);

bool common_hal_vectorio_vector_shape_get_transpose_xy(vectorio_vector_shape_t *self);

mp_obj_t common_hal_vectorio_vector_shape_get_pixel_shader(vectorio_vector_shape_t *self);
void common_hal_vectorio_vector_shape_set_pixel_shader(vectorio_vector_shape_t *self, mp_obj_t pixel_shader);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_SHAPE_H
