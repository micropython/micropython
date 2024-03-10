#include "shared-module/vectorio/__init__.h"
#include "shared-bindings/vectorio/Rectangle.h"
#include "shared-module/displayio/area.h"

#include "py/runtime.h"
#include "stdlib.h"


void common_hal_vectorio_rectangle_construct(vectorio_rectangle_t *self, uint32_t width, uint32_t height, uint16_t color_index) {
    self->width = width;
    self->height = height;
    self->color_index = color_index + 1;
}

void common_hal_vectorio_rectangle_set_on_dirty(vectorio_rectangle_t *self, vectorio_event_t on_dirty) {
    if (self->on_dirty.obj != NULL) {
        mp_raise_TypeError(MP_ERROR_TEXT("can only have one parent"));
    }
    self->on_dirty = on_dirty;
}

uint32_t common_hal_vectorio_rectangle_get_pixel(void *obj, int16_t x, int16_t y) {
    vectorio_rectangle_t *self = obj;
    if (x >= 0 && y >= 0 && x < self->width && y < self->height) {
        return self->color_index;
    }
    return 0;
}


void common_hal_vectorio_rectangle_get_area(void *rectangle, displayio_area_t *out_area) {
    vectorio_rectangle_t *self = rectangle;
    out_area->x1 = 0;
    out_area->y1 = 0;
    out_area->x2 = self->width;
    out_area->y2 = self->height;
}


mp_obj_t common_hal_vectorio_rectangle_get_draw_protocol(void *rectangle) {
    vectorio_rectangle_t *self = rectangle;
    return self->draw_protocol_instance;
}

int16_t common_hal_vectorio_rectangle_get_width(void *obj) {
    vectorio_rectangle_t *self = obj;
    return self->width;
}

void common_hal_vectorio_rectangle_set_width(void *obj, int16_t width) {
    vectorio_rectangle_t *self = obj;
    self->width = abs(width);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

int16_t common_hal_vectorio_rectangle_get_height(void *obj) {
    vectorio_rectangle_t *self = obj;
    return self->height;
}

void common_hal_vectorio_rectangle_set_height(void *obj, int16_t height) {
    vectorio_rectangle_t *self = obj;
    self->height = abs(height);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

uint16_t common_hal_vectorio_rectangle_get_color_index(void *obj) {
    vectorio_rectangle_t *self = obj;
    return self->color_index - 1;
}

void common_hal_vectorio_rectangle_set_color_index(void *obj, uint16_t color_index) {
    vectorio_rectangle_t *self = obj;
    self->color_index = abs(color_index + 1);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}
