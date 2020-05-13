
#include "shared-module/vectorio/__init__.h"
#include "shared-bindings/vectorio/Polygon.h"
#include "shared-module/displayio/area.h"

#include "py/runtime.h"
#include "stdlib.h"
#include <stdio.h>


#define VECTORIO_POLYGON_DEBUG(...) (void)0
// #define VECTORIO_POLYGON_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)


void common_hal_vectorio_polygon_construct(vectorio_polygon_t *self, mp_obj_t points_list) {
    VECTORIO_POLYGON_DEBUG("%p polygon_construct\n", self);
    self->points_list = points_list;
    self->on_dirty.obj = NULL;
}


mp_obj_t common_hal_vectorio_polygon_get_points(vectorio_polygon_t *self) {
    return self->points_list;
}
void common_hal_vectorio_polygon_set_points(vectorio_polygon_t *self, mp_obj_t points_list) {
    self->points_list = points_list;
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

void common_hal_vectorio_polygon_set_on_dirty(vectorio_polygon_t *self, vectorio_event_t notification) {
    if ( self->on_dirty.obj != NULL ) {
        mp_raise_TypeError(translate("polygon can only be registered in one parent"));
    }
    self->on_dirty = notification;
}


void common_hal_vectorio_polygon_get_area(void *polygon, displayio_area_t *area) {
    VECTORIO_POLYGON_DEBUG("%p polygon get_area", polygon);
    vectorio_polygon_t *self = polygon;
    size_t len;
    mp_obj_t *points;
    mp_obj_list_get(self->points_list, &len, &points);
    VECTORIO_POLYGON_DEBUG(" len: %2d, points: %d\n", len, len/2);

    area->x1 = SHRT_MAX;
    area->y1 = SHRT_MAX;
    area->x2 = SHRT_MIN;
    area->y2 = SHRT_MIN;
    for (size_t i=0; i < len; ++i) {
        mp_int_t x = mp_obj_get_int(points[i]);
        ++i;
        mp_int_t y = mp_obj_get_int(points[i]);
        if (x <= area->x1) area->x1 = x-1;
        if (y <= area->y1) area->y1 = y-1;
        if (x >= area->x2) area->x2 = x+1;
        if (y >= area->y2) area->y2 = y+1;
    }
}


// <0 if the point is to the left of the line vector
//  0 if the point is on the line
// >0 if the point is to the right of the line vector
__attribute__((always_inline)) static inline int line_side( mp_int_t x1, mp_int_t y1, mp_int_t x2, mp_int_t y2, int16_t px, int16_t py ) {
    return (px - x1) * (y2 - y1)
         - (py - y1) * (x2 - x1);
}


uint32_t common_hal_vectorio_polygon_get_pixel(void *obj, int16_t x, int16_t y) {
    VECTORIO_POLYGON_DEBUG("%p polygon get_pixel %d, %d\n", obj, x, y);
    vectorio_polygon_t *self = obj;
    size_t len;
    mp_obj_t *points;
    mp_obj_list_get(self->points_list, &len, &points);

    if (len == 0) {
        return 0;
    }

    int winding_number = 0;
    mp_int_t x1 = mp_obj_get_int(points[0]);
    mp_int_t y1 = mp_obj_get_int(points[1]);
    for (size_t i=2; i <= len + 1; ++i) {
        VECTORIO_POLYGON_DEBUG("  {(%3d, %3d),", x1, y1);
        mp_int_t x2 = mp_obj_get_int(points[i % len]);
        ++i;
        mp_int_t y2 = mp_obj_get_int(points[i % len]);
        VECTORIO_POLYGON_DEBUG(" (%3d, %3d)}\n", x2, y2);
        if ( y1 <= y ) {
            if ( y2 > y && line_side(x1, y1, x2, y2, x, y) > 0 ) {
                // Wind up, point is to the right of the edge vector
                ++winding_number;
                VECTORIO_POLYGON_DEBUG("    wind:%2d winding_number:%2d\n", 1, winding_number);
            }
        } else if ( y2 <= y && line_side(x1, y1, x2, y2, x, y) < 0 ) {
            // Wind down, point is to the left of the edge vector
            --winding_number;
            VECTORIO_POLYGON_DEBUG("    wind:%2d winding_number:%2d\n", -1, winding_number); 
        }

        x1 = x2;
        y1 = y2;
    }
    return winding_number == 0 ? 0 : 1;
}

