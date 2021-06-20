
#include "stdlib.h"

#include "shared-module/vectorio/__init__.h"
#include "shared-bindings/vectorio/VectorShape.h"

#include "py/runtime.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Palette.h"

#include "shared-bindings/vectorio/Circle.h"
#include "shared-bindings/vectorio/Polygon.h"
#include "shared-bindings/vectorio/Rectangle.h"

// Lifecycle actions.
#define VECTORIO_SHAPE_DEBUG(...) (void)0
// #define VECTORIO_SHAPE_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)


// Used in both logging and ifdefs, for extra variables
// #define VECTORIO_PERF(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)


// Really verbose.
#define VECTORIO_SHAPE_PIXEL_DEBUG(...) (void)0
// #define VECTORIO_SHAPE_PIXEL_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)


inline __attribute__((always_inline))
static int32_t max(int32_t a, int32_t b) {
    return a > b ? a : b;
}


inline __attribute__((always_inline))
static void _get_screen_area(vectorio_vector_shape_t *self, displayio_area_t *out_area) {
    VECTORIO_SHAPE_DEBUG("%p get_screen_area tform:{x:%d y:%d dx:%d dy:%d scl:%d w:%d h:%d mx:%d my:%d tr:%d}", self,
        self->absolute_transform->x, self->absolute_transform->y, self->absolute_transform->dx, self->absolute_transform->dy, self->absolute_transform->scale,
        self->absolute_transform->width, self->absolute_transform->height, self->absolute_transform->mirror_x, self->absolute_transform->mirror_y, self->absolute_transform->transpose_xy
        );
    self->ishape.get_area(self->ishape.shape, out_area);
    VECTORIO_SHAPE_DEBUG(" in:{(%5d,%5d), (%5d,%5d)}", out_area->x1, out_area->y1, out_area->x2, out_area->y2);
    if (self->absolute_transform->transpose_xy) {
        int16_t swap = out_area->x1;
        out_area->x1 = (out_area->y1 + self->y) * self->absolute_transform->dx + self->absolute_transform->x;
        out_area->y1 = (swap + self->x) * self->absolute_transform->dy + self->absolute_transform->y;
        swap = out_area->x2;
        out_area->x2 = (out_area->y2 + self->y) * self->absolute_transform->dx + self->absolute_transform->x;
        out_area->y2 = (swap + self->x) * self->absolute_transform->dy + self->absolute_transform->y;
    } else {
        out_area->x1 = (out_area->x1 + self->x) * self->absolute_transform->dx + self->absolute_transform->x;
        out_area->y1 = (out_area->y1 + self->y) * self->absolute_transform->dy + self->absolute_transform->y;
        out_area->x2 = (out_area->x2 + self->x) * self->absolute_transform->dx + self->absolute_transform->x;
        out_area->y2 = (out_area->y2 + self->y) * self->absolute_transform->dy + self->absolute_transform->y;
    }
    // We might have mirrored due to dx
    displayio_area_canon(out_area);
    VECTORIO_SHAPE_DEBUG(" out:{(%5d,%5d), (%5d,%5d)}\n", out_area->x1, out_area->y1, out_area->x2, out_area->y2);
}


// For use by Group to know where it needs to redraw on layer removal.
bool vectorio_vector_shape_get_dirty_area(vectorio_vector_shape_t *self, displayio_area_t *out_area) {
    displayio_area_copy(&self->ephemeral_dirty_area, out_area);
    return true; // For now just always redraw.
}


// This must be invoked each time a shape changes its position or its shape in any way.
void common_hal_vectorio_vector_shape_set_dirty(void *vector_shape) {
    vectorio_vector_shape_t *self = vector_shape;
    // In screen space.  Need to offset the shape space.
    displayio_area_t current_area;
    _get_screen_area(self, &current_area);
    VECTORIO_SHAPE_DEBUG("%p shape_dirty current:{(%3d,%3d), (%3d,%3d)} dirty:{(%3d,%3d), (%3d,%3d)}",
        self,
        current_area.x1, current_area.y1, current_area.x2, current_area.y2,
        self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);
    self->dirty = true;
    // Dirty area tracks the shape's footprint between draws.  It's reset on refresh finish,
    displayio_area_union(&self->ephemeral_dirty_area, &current_area, &self->ephemeral_dirty_area);
    VECTORIO_SHAPE_DEBUG(" -> expanded:{(%3d,%3d), (%3d,%3d)}\n", self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);
}


void common_hal_vectorio_vector_shape_construct(vectorio_vector_shape_t *self,
    vectorio_ishape_t ishape,
    mp_obj_t pixel_shader, uint16_t x, uint16_t y) {
    VECTORIO_SHAPE_DEBUG("%p vector_shape_construct x:%3d, y:%3d\n", self, x, y);
    self->x = x;
    self->y = y;
    self->pixel_shader = pixel_shader;
    self->ishape = ishape;
    self->dirty = true;
    self->absolute_transform = &null_transform; // Critical to have a valid transform before getting screen area.
    _get_screen_area(self, &self->ephemeral_dirty_area);
    self->ephemeral_dirty_area.next = NULL;
}


mp_int_t common_hal_vectorio_vector_shape_get_x(vectorio_vector_shape_t *self) {
    VECTORIO_SHAPE_DEBUG("%p get_x\n", self);
    return self->x;
}


void common_hal_vectorio_vector_shape_set_x(vectorio_vector_shape_t *self, mp_int_t x) {
    VECTORIO_SHAPE_DEBUG("%p set_x %d\n", self, x);
    if (self->x == x) {
        return;
    }
    self->x = x;
    common_hal_vectorio_vector_shape_set_dirty(self);
}


mp_int_t common_hal_vectorio_vector_shape_get_y(vectorio_vector_shape_t *self) {
    VECTORIO_SHAPE_DEBUG("%p get_y\n", self);
    return self->y;
}


void common_hal_vectorio_vector_shape_set_y(vectorio_vector_shape_t *self, mp_int_t y) {
    VECTORIO_SHAPE_DEBUG("%p set_y %d\n", self, y);
    if (self->y == y) {
        return;
    }
    self->y = y;
    common_hal_vectorio_vector_shape_set_dirty(self);
}


mp_obj_t common_hal_vectorio_vector_shape_get_pixel_shader(vectorio_vector_shape_t *self) {
    VECTORIO_SHAPE_DEBUG("%p get_pixel_shader\n", self);
    return self->pixel_shader;
}

void common_hal_vectorio_vector_shape_set_pixel_shader(vectorio_vector_shape_t *self, mp_obj_t pixel_shader) {
    VECTORIO_SHAPE_DEBUG("%p set_pixel_shader\n", self);
    self->pixel_shader = pixel_shader;
    common_hal_vectorio_vector_shape_set_dirty(self);
}


bool vectorio_vector_shape_fill_area(vectorio_vector_shape_t *self, const _displayio_colorspace_t *colorspace, const displayio_area_t *area, uint32_t *mask, uint32_t *buffer) {
    // Shape areas are relative to 0,0.  This will allow rotation about a known axis.
    //   The consequence is that the area reported by the shape itself is _relative_ to 0,0.
    //   To make it relative to the VectorShape position, we must shift it.
    // Pixels are drawn on the screen_area (shifted) coordinate space, while pixels are _determined_ from
    //   the shape_area (unshifted) space.
    #ifdef VECTORIO_PERF
    uint64_t start = common_hal_time_monotonic_ns();
    uint64_t pixel_time = 0;
    #endif
    displayio_area_t overlap;
    VECTORIO_SHAPE_DEBUG("%p fill_area dirty:%d fill: {(%5d,%5d), (%5d,%5d)} dirty: {(%5d,%5d), (%5d,%5d)}",
        self, self->dirty,
        area->x1, area->y1, area->x2, area->y2,
        self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2
        );
    if (!displayio_area_compute_overlap(area, &self->ephemeral_dirty_area, &overlap)) {
        VECTORIO_SHAPE_DEBUG(" no overlap\n");
        return false;
    }
    VECTORIO_SHAPE_DEBUG(", overlap: {(%3d,%3d), (%3d,%3d)}", overlap.x1, overlap.y1, overlap.x2, overlap.y2);

    bool full_coverage = displayio_area_equal(area, &overlap);

    uint8_t pixels_per_byte = 8 / colorspace->depth;

    uint32_t linestride_px = displayio_area_width(area);
    uint32_t line_dirty_offset_px = (overlap.y1 - area->y1) * linestride_px;
    uint32_t column_dirty_offset_px = overlap.x1 - area->x1;
    VECTORIO_SHAPE_DEBUG(", linestride:%3d line_offset:%3d col_offset:%3d depth:%2d ppb:%2d shape:%s",
        linestride_px, line_dirty_offset_px, column_dirty_offset_px, colorspace->depth, pixels_per_byte, mp_obj_get_type_str(self->ishape.shape));

    displayio_input_pixel_t input_pixel;
    displayio_output_pixel_t output_pixel;

    uint32_t mask_start_px = line_dirty_offset_px;
    for (input_pixel.y = overlap.y1; input_pixel.y < overlap.y2; ++input_pixel.y) {
        mask_start_px += column_dirty_offset_px;
        for (input_pixel.x = overlap.x1; input_pixel.x < overlap.x2; ++input_pixel.x) {
            // Check the mask first to see if the pixel has already been set.
            uint32_t pixel_index = mask_start_px + (input_pixel.x - overlap.x1);
            uint32_t *mask_doubleword = &(mask[pixel_index / 32]);
            uint8_t mask_bit = pixel_index % 32;
            VECTORIO_SHAPE_PIXEL_DEBUG("%p pixel_index: %5u mask_bit: %2u", self, pixel_index, mask_bit);
            if ((*mask_doubleword & (1u << mask_bit)) != 0) {
                VECTORIO_SHAPE_PIXEL_DEBUG(" masked\n");
                continue;
            }
            output_pixel.pixel = 0;

            // Get the target pixel based on the shape's coordinate space
            int16_t pixel_to_get_x;
            int16_t pixel_to_get_y;
            if (self->absolute_transform->transpose_xy) {
                pixel_to_get_x = (input_pixel.y - self->absolute_transform->dy * self->x - self->absolute_transform->y) / self->absolute_transform->dy;
                pixel_to_get_y = (input_pixel.x - self->absolute_transform->dx * self->y - self->absolute_transform->x) / self->absolute_transform->dx;
            } else {
                pixel_to_get_x = (input_pixel.x - self->absolute_transform->dx * self->x - self->absolute_transform->x) / self->absolute_transform->dx;
                pixel_to_get_y = (input_pixel.y - self->absolute_transform->dy * self->y - self->absolute_transform->y) / self->absolute_transform->dy;
            }
            VECTORIO_SHAPE_PIXEL_DEBUG(" get_pixel %p (%3d, %3d) -> ( %3d, %3d )", self->ishape.shape, input_pixel.x, input_pixel.y, pixel_to_get_x, pixel_to_get_y);
            #ifdef VECTORIO_PERF
            uint64_t pre_pixel = common_hal_time_monotonic_ns();
            #endif
            input_pixel.pixel = self->ishape.get_pixel(self->ishape.shape, pixel_to_get_x, pixel_to_get_y);
            #ifdef VECTORIO_PERF
            uint64_t post_pixel = common_hal_time_monotonic_ns();
            pixel_time += post_pixel - pre_pixel;
            #endif
            VECTORIO_SHAPE_PIXEL_DEBUG(" -> %d", input_pixel.pixel);

            output_pixel.opaque = true;
            if (self->pixel_shader == mp_const_none) {
                output_pixel.pixel = input_pixel.pixel;
            } else if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
                output_pixel.opaque = displayio_palette_get_color(self->pixel_shader, colorspace, input_pixel.pixel, &output_pixel.pixel);
            } else if (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type)) {
                displayio_colorconverter_convert(self->pixel_shader, colorspace, &input_pixel, &output_pixel);
            }
            if (!output_pixel.opaque) {
                VECTORIO_SHAPE_PIXEL_DEBUG(" (encountered transparent pixel; input area is not fully covered)\n");
                full_coverage = false;
            } else {
                *mask_doubleword |= 1u << mask_bit;
                if (colorspace->depth == 16) {
                    VECTORIO_SHAPE_PIXEL_DEBUG(" buffer = %04x 16\n", output_pixel.pixel);
                    *(((uint16_t *)buffer) + pixel_index) = output_pixel.pixel;
                } else if (colorspace->depth == 8) {
                    VECTORIO_SHAPE_PIXEL_DEBUG(" buffer = %02x 8\n", output_pixel.pixel);
                    *(((uint8_t *)buffer) + pixel_index) = output_pixel.pixel;
                } else if (colorspace->depth < 8) {
                    // Reorder the offsets to pack multiple rows into a byte (meaning they share a column).
                    if (!colorspace->pixels_in_byte_share_row) {
                        uint16_t width = linestride_px;
                        uint16_t row = pixel_index / width;
                        uint16_t col = pixel_index % width;
                        pixel_index = col * pixels_per_byte + (row / pixels_per_byte) * pixels_per_byte * width + row % pixels_per_byte;
                    }
                    uint8_t shift = (pixel_index % pixels_per_byte) * colorspace->depth;
                    if (colorspace->reverse_pixels_in_byte) {
                        // Reverse the shift by subtracting it from the leftmost shift.
                        shift = (pixels_per_byte - 1) * colorspace->depth - shift;
                    }
                    VECTORIO_SHAPE_PIXEL_DEBUG(" buffer = %2d %d\n", output_pixel.pixel, colorspace->depth);
                    ((uint8_t *)buffer)[pixel_index / pixels_per_byte] |= output_pixel.pixel << shift;
                }
            }
        }
        mask_start_px += linestride_px - column_dirty_offset_px;
    }
    #ifdef VECTORIO_PERF
    uint64_t end = common_hal_time_monotonic_ns();
    uint32_t pixels = (overlap.x2 - overlap.x1) * (overlap.y2 - overlap.y1);
    VECTORIO_PERF("draw %16s -> shape:{%4dpx, %4.1fms,%9.1fpps fill}  shape_pixels:{%6.1fus total, %4.1fus/px}\n",
        mp_obj_get_type_str(self->ishape.shape),
        (overlap.x2 - overlap.x1) * (overlap.y2 - overlap.y1),
        (double)((end - start) / 1000000.0),
        (double)(max(1, pixels * (1000000000.0 / (end - start)))),
        (double)(pixel_time / 1000.0),
        (double)(pixel_time / 1000.0 / pixels)
        );
    #endif
    VECTORIO_SHAPE_DEBUG(" -> pixels:%4d\n");
    return full_coverage;
}


void vectorio_vector_shape_finish_refresh(vectorio_vector_shape_t *self) {
    if (!self->dirty) {
        return;
    }
    VECTORIO_SHAPE_DEBUG("%p finish_refresh was:{(%3d,%3d), (%3d,%3d)}\n", self, self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);
    self->dirty = false;
    // Reset dirty area tracking to current footprint
    _get_screen_area(self, &self->ephemeral_dirty_area);
    self->ephemeral_dirty_area.next = NULL;
    VECTORIO_SHAPE_DEBUG("%p finish_refresh now:{(%3d,%3d), (%3d,%3d)}\n", self, self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);

    if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
        displayio_palette_finish_refresh(self->pixel_shader);
    } else if (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type)) {
        displayio_colorconverter_finish_refresh(self->pixel_shader);
    }
}


// Assembles a singly linked list of dirty areas from all components on the display.
displayio_area_t *vectorio_vector_shape_get_refresh_areas(vectorio_vector_shape_t *self, displayio_area_t *tail) {
    if (self->dirty
        || (mp_obj_is_type(self->pixel_shader, &displayio_palette_type) && displayio_palette_needs_refresh(self->pixel_shader))
        || (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type) && displayio_colorconverter_needs_refresh(self->pixel_shader))
        ) {
        VECTORIO_SHAPE_DEBUG("%p get_refresh_area dirty:%d {(%3d,%3d), (%3d,%3d)}", self, self->dirty, self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);
        common_hal_vectorio_vector_shape_set_dirty(self);
        // vector.add_to_head
        self->ephemeral_dirty_area.next = tail;
        VECTORIO_SHAPE_DEBUG(" this_area: %p next: %p after: %p\n", &self->ephemeral_dirty_area, tail, tail == NULL ? NULL : tail->next);
        return &self->ephemeral_dirty_area;
    }
    return tail;
}

void vectorio_vector_shape_update_transform(vectorio_vector_shape_t *self, displayio_buffer_transform_t *group_transform) {
    self->absolute_transform = group_transform == NULL ? &null_transform : group_transform;
    common_hal_vectorio_vector_shape_set_dirty(self);
}
