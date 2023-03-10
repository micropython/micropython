
#include "stdlib.h"

#include "shared-module/vectorio/__init__.h"
#include "shared-bindings/vectorio/VectorShape.h"

#include "py/misc.h"
#include "py/runtime.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Palette.h"

#include "shared-bindings/vectorio/Circle.h"
#include "shared-bindings/vectorio/Polygon.h"
#include "shared-bindings/vectorio/Rectangle.h"

// Lifecycle actions.
#define VECTORIO_SHAPE_DEBUG(...) (void)0
// #define VECTORIO_SHAPE_DEBUG(...) mp_printf(&mp_plat_print, __VA_ARGS__)


// Used in both logging and ifdefs, for extra variables
// #define VECTORIO_PERF(...) mp_printf(&mp_plat_print, __VA_ARGS__)


// Really verbose.
#define VECTORIO_SHAPE_PIXEL_DEBUG(...) (void)0
// #define VECTORIO_SHAPE_PIXEL_DEBUG(...) mp_printf(&mp_plat_print, __VA_ARGS__)

#define U32_TO_BINARY_FMT "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define U32_TO_BINARY(u32)  \
    (u32 & 0x80000000 ? '1' : '0'), \
    (u32 & 0x40000000 ? '1' : '0'), \
    (u32 & 0x20000000 ? '1' : '0'), \
    (u32 & 0x10000000 ? '1' : '0'), \
    (u32 & 0x8000000  ? '1' : '0'), \
    (u32 & 0x4000000  ? '1' : '0'), \
    (u32 & 0x2000000  ? '1' : '0'), \
    (u32 & 0x1000000  ? '1' : '0'), \
    (u32 & 0x800000   ? '1' : '0'), \
    (u32 & 0x400000   ? '1' : '0'), \
    (u32 & 0x200000   ? '1' : '0'), \
    (u32 & 0x100000   ? '1' : '0'), \
    (u32 & 0x80000    ? '1' : '0'), \
    (u32 & 0x40000    ? '1' : '0'), \
    (u32 & 0x20000    ? '1' : '0'), \
    (u32 & 0x10000    ? '1' : '0'), \
    (u32 & 0x8000     ? '1' : '0'), \
    (u32 & 0x4000     ? '1' : '0'), \
    (u32 & 0x2000     ? '1' : '0'), \
    (u32 & 0x1000     ? '1' : '0'), \
    (u32 & 0x800      ? '1' : '0'), \
    (u32 & 0x400      ? '1' : '0'), \
    (u32 & 0x200      ? '1' : '0'), \
    (u32 & 0x100      ? '1' : '0'), \
    (u32 & 0x80       ? '1' : '0'), \
    (u32 & 0x40       ? '1' : '0'), \
    (u32 & 0x20       ? '1' : '0'), \
    (u32 & 0x10       ? '1' : '0'), \
    (u32 & 0x8        ? '1' : '0'), \
    (u32 & 0x4        ? '1' : '0'), \
    (u32 & 0x2        ? '1' : '0'), \
    (u32 & 0x1        ? '1' : '0')

static void short_bound_check(mp_int_t i, qstr name) {
    mp_arg_validate_int_range(i, SHRT_MIN, SHRT_MAX, name);
}

inline __attribute__((always_inline))
static void area_transpose(displayio_area_t *to_transpose) {
    int16_t swap = to_transpose->y1;
    to_transpose->y1 = to_transpose->x1;
    to_transpose->x1 = swap;
    swap = to_transpose->y2;
    to_transpose->y2 = to_transpose->x2;
    to_transpose->x2 = swap;
}

inline __attribute__((always_inline))
static void _get_screen_area(vectorio_vector_shape_t *self, displayio_area_t *out_area) {
    VECTORIO_SHAPE_DEBUG("%p get_screen_area (%3d,%3d) tform:{x:%d y:%d dx:%d dy:%d scl:%d w:%d h:%d mx:%d my:%d tr:%d}", self, self->x, self->y,
        self->absolute_transform->x, self->absolute_transform->y, self->absolute_transform->dx, self->absolute_transform->dy, self->absolute_transform->scale,
        self->absolute_transform->width, self->absolute_transform->height, self->absolute_transform->mirror_x, self->absolute_transform->mirror_y, self->absolute_transform->transpose_xy
        );
    self->ishape.get_area(self->ishape.shape, out_area);
    VECTORIO_SHAPE_DEBUG(" in:{(%5d,%5d), (%5d,%5d)}", out_area->x1, out_area->y1, out_area->x2, out_area->y2);

    int16_t x;
    int16_t y;
    if (self->absolute_transform->transpose_xy) {
        x = self->absolute_transform->x + self->absolute_transform->dx * self->y;
        y = self->absolute_transform->y + self->absolute_transform->dy * self->x;
        if (self->absolute_transform->dx < 1) {
            out_area->y1 = out_area->y1 * -1 + 1;
            out_area->y2 = out_area->y2 * -1 + 1;
        }
        if (self->absolute_transform->dy < 1) {
            out_area->x1 = out_area->x1 * -1 + 1;
            out_area->x2 = out_area->x2 * -1 + 1;
        }
        area_transpose(out_area);
    } else {
        x = self->absolute_transform->x + self->absolute_transform->dx * self->x;
        y = self->absolute_transform->y + self->absolute_transform->dy * self->y;

        if (self->absolute_transform->dx < 1) {
            out_area->x1 = out_area->x1 * -1 + 1;
            out_area->x2 = out_area->x2 * -1 + 1;
        }
        if (self->absolute_transform->dy < 1) {
            out_area->y1 = out_area->y1 * -1 + 1;
            out_area->y2 = out_area->y2 * -1 + 1;
        }
    }
    displayio_area_canon(out_area);
    displayio_area_shift(out_area, x, y);

    VECTORIO_SHAPE_DEBUG(" out:{(%5d,%5d), (%5d,%5d)}\n", out_area->x1, out_area->y1, out_area->x2, out_area->y2);
}

// Get the target pixel based on the shape's coordinate space
static void screen_to_shape_coordinates(vectorio_vector_shape_t *self, uint16_t x, uint16_t y, int16_t *out_shape_x, int16_t *out_shape_y) {
    if (self->absolute_transform->transpose_xy) {
        *out_shape_x = y - self->absolute_transform->y - self->absolute_transform->dy * self->x;
        *out_shape_y = x - self->absolute_transform->x - self->absolute_transform->dx * self->y;

        VECTORIO_SHAPE_PIXEL_DEBUG(" a(%3d, %3d)", *out_shape_x, *out_shape_y);
        if (self->absolute_transform->dx < 1) {
            *out_shape_y *= -1;
        }
        if (self->absolute_transform->dy < 1) {
            *out_shape_x *= -1;
        }
        VECTORIO_SHAPE_PIXEL_DEBUG(" b(%3d, %3d)", *out_shape_x, *out_shape_y);
    } else {
        *out_shape_x = x - self->absolute_transform->x - self->absolute_transform->dx * self->x;
        *out_shape_y = y - self->absolute_transform->y - self->absolute_transform->dy * self->y;

        VECTORIO_SHAPE_PIXEL_DEBUG(" a(%3d, %3d)", *out_shape_x, *out_shape_y);
        if (self->absolute_transform->dx < 1) {
            *out_shape_x *= -1;
        }
        if (self->absolute_transform->dy < 1) {
            *out_shape_y *= -1;
        }
        VECTORIO_SHAPE_PIXEL_DEBUG(" b(%3d, %3d)", *out_shape_x, *out_shape_y);

        // It's mirrored via dx. Maybe we need to add support for also separately mirroring?
        // if (self->absolute_transform->mirror_x) {
        //     pixel_to_get_x = (shape_area.x2 - shape_area.x1) - (pixel_to_get_x - shape_area.x1) + shape_area.x1 - 1;
        // }
        // if (self->absolute_transform->mirror_y) {
        //     pixel_to_get_y = (shape_area.y2 - shape_area.y1) - (pixel_to_get_y - shape_area.y1) + +shape_area.y1 - 1;
        // }
    }
}

static void check_bounds_and_set_x(vectorio_vector_shape_t *self, mp_int_t x) {
    short_bound_check(x, MP_QSTR_x);
    self->x = x;
}

static void check_bounds_and_set_y(vectorio_vector_shape_t *self, mp_int_t y) {
    short_bound_check(y, MP_QSTR_y);
    self->y = y;
}


// For use by Group to know where it needs to redraw on layer removal.
bool vectorio_vector_shape_get_dirty_area(vectorio_vector_shape_t *self, displayio_area_t *out_area) {
    out_area->x1 = out_area->x2;
    displayio_area_union(
        &self->ephemeral_dirty_area,
        &self->current_area,
        out_area
        );
    return true; // For now just always redraw.
}


// This must be invoked after each time a shape changes its position, shape or appearance in any way.
void common_hal_vectorio_vector_shape_set_dirty(void *vector_shape) {
    vectorio_vector_shape_t *self = vector_shape;
    // In screen space.  Need to offset the shape space.
    displayio_area_t current_area;
    _get_screen_area(self, &current_area);
    VECTORIO_SHAPE_DEBUG("%p shape_dirty new:{(%3d,%3d), (%3d,%3d)} dirty:{(%3d,%3d), (%3d,%3d)}",
        self,
        current_area.x1, current_area.y1, current_area.x2, current_area.y2,
        self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);

    bool moved = !displayio_area_equal(&current_area, &self->current_area);
    if (moved) {
        displayio_area_union(&self->current_area, &self->ephemeral_dirty_area, &self->ephemeral_dirty_area);
        VECTORIO_SHAPE_DEBUG(" stale:{(%3d,%3d), (%3d,%3d)} -> expanded:{(%3d,%3d), (%3d,%3d)}\n",
            self->current_area.x1, self->current_area.y1, self->current_area.x2, self->current_area.y2,
            self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);

        // Dirty area tracks the shape's footprint between draws.  It's reset on refresh finish.
        displayio_area_copy(&current_area, &self->current_area);
    }
    self->current_area_dirty = true;
}


void common_hal_vectorio_vector_shape_construct(vectorio_vector_shape_t *self,
    vectorio_ishape_t ishape,
    mp_obj_t pixel_shader, int32_t x, int32_t y) {
    VECTORIO_SHAPE_DEBUG("%p vector_shape_construct x:%3d, y:%3d\n", self, x, y);
    check_bounds_and_set_x(self, x);
    check_bounds_and_set_y(self, y);
    self->pixel_shader = pixel_shader;
    self->ishape = ishape;
    self->absolute_transform = &null_transform; // Critical to have a valid transform before getting screen area.
    self->ephemeral_dirty_area.x1 = self->ephemeral_dirty_area.x2; // Cheat to set area to 0
    self->ephemeral_dirty_area.next = NULL;
    self->current_area_dirty = true;
    _get_screen_area(self, &self->current_area);
}

bool common_hal_vectorio_vector_shape_contains(vectorio_vector_shape_t *self, mp_int_t x, mp_int_t y) {
    VECTORIO_SHAPE_DEBUG("%p contains(%d, %d)", self);
    short_bound_check(x, MP_QSTR_x);
    short_bound_check(y, MP_QSTR_y);
    int16_t shape_x;
    int16_t shape_y;
    screen_to_shape_coordinates(self, x, y, &shape_x, &shape_y);
    bool shape_contains_coordinates = 0 != self->ishape.get_pixel(self->ishape.shape, shape_x, shape_y);
    return shape_contains_coordinates;
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
    check_bounds_and_set_x(self, x);
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
    check_bounds_and_set_y(self, y);
    common_hal_vectorio_vector_shape_set_dirty(self);
}

mp_obj_tuple_t *common_hal_vectorio_vector_shape_get_location(vectorio_vector_shape_t *self) {
    VECTORIO_SHAPE_DEBUG("%p get_location\n", self);
    mp_obj_tuple_t *pair = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    pair->items[0] = mp_obj_new_int((mp_int_t)self->x);
    pair->items[1] = mp_obj_new_int((mp_int_t)self->y);
    return pair;
}


void common_hal_vectorio_vector_shape_set_location(vectorio_vector_shape_t *self, mp_obj_t xy) {
    VECTORIO_SHAPE_DEBUG("%p set_location\n", self);
    size_t tuple_len = 0;
    mp_obj_t *tuple_items;
    mp_obj_tuple_get(xy, &tuple_len, &tuple_items);
    mp_arg_validate_length(tuple_len, 2, MP_QSTR_location);

    mp_int_t x = mp_arg_validate_type_int(tuple_items[0], MP_QSTR_x);
    mp_int_t y = mp_arg_validate_type_int(tuple_items[0], MP_QSTR_y);
    bool dirty = false;
    if (self->x != x) {
        check_bounds_and_set_x(self, x);
        dirty = true;
    }
    if (self->y != y) {
        check_bounds_and_set_y(self, y);
        dirty = true;
    }
    if (dirty) {
        common_hal_vectorio_vector_shape_set_dirty(self);
    }
}

mp_int_t common_hal_vectorio_vector_shape_get_hidden(vectorio_vector_shape_t *self) {
    VECTORIO_SHAPE_DEBUG("%p get_hidden\n", self);
    return self->hidden;
}

void common_hal_vectorio_vector_shape_set_hidden(vectorio_vector_shape_t *self, bool hidden) {
    VECTORIO_SHAPE_DEBUG("%p set_hidden %d\n", self, x);
    self->hidden = hidden;
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

    if (self->hidden) {
        return false;
    }

    VECTORIO_SHAPE_DEBUG("%p fill_area: fill: {(%5d,%5d), (%5d,%5d)}",
        self,
        area->x1, area->y1, area->x2, area->y2
        );
    displayio_area_t overlap;
    if (!displayio_area_compute_overlap(area, &self->current_area, &overlap)) {
        VECTORIO_SHAPE_DEBUG(" no overlap\n");
        return false;
    }
    VECTORIO_SHAPE_DEBUG(", overlap: {(%3d,%3d), (%3d,%3d)}", overlap.x1, overlap.y1, overlap.x2, overlap.y2);

    bool full_coverage = displayio_area_equal(area, &overlap);

    uint8_t pixels_per_byte = 8 / colorspace->depth;
    VECTORIO_SHAPE_DEBUG(" xy:(%3d %3d) tform:{x:%d y:%d dx:%d dy:%d scl:%d w:%d h:%d mx:%d my:%d tr:%d}",
        self->x, self->y,
        self->absolute_transform->x, self->absolute_transform->y, self->absolute_transform->dx, self->absolute_transform->dy, self->absolute_transform->scale,
        self->absolute_transform->width, self->absolute_transform->height, self->absolute_transform->mirror_x, self->absolute_transform->mirror_y, self->absolute_transform->transpose_xy
        );

    uint16_t linestride_px = displayio_area_width(area);
    uint16_t line_dirty_offset_px = (overlap.y1 - area->y1) * linestride_px;
    uint16_t column_dirty_offset_px = overlap.x1 - area->x1;
    VECTORIO_SHAPE_DEBUG(", linestride:%3d line_offset:%3d col_offset:%3d depth:%2d ppb:%2d shape:%s",
        linestride_px, line_dirty_offset_px, column_dirty_offset_px, colorspace->depth, pixels_per_byte, mp_obj_get_type_str(self->ishape.shape));

    displayio_input_pixel_t input_pixel;
    displayio_output_pixel_t output_pixel;

    displayio_area_t shape_area;
    self->ishape.get_area(self->ishape.shape, &shape_area);

    uint16_t mask_start_px = line_dirty_offset_px;
    for (input_pixel.y = overlap.y1; input_pixel.y < overlap.y2; ++input_pixel.y) {
        mask_start_px += column_dirty_offset_px;
        for (input_pixel.x = overlap.x1; input_pixel.x < overlap.x2; ++input_pixel.x) {
            // Check the mask first to see if the pixel has already been set.
            uint16_t pixel_index = mask_start_px + (input_pixel.x - overlap.x1);
            uint32_t *mask_doubleword = &(mask[pixel_index / 32]);
            uint8_t mask_bit = pixel_index % 32;
            VECTORIO_SHAPE_PIXEL_DEBUG("\n%p pixel_index: %5u mask_bit: %2u mask: "U32_TO_BINARY_FMT, self, pixel_index, mask_bit, U32_TO_BINARY(*mask_doubleword));
            if ((*mask_doubleword & (1u << mask_bit)) != 0) {
                VECTORIO_SHAPE_PIXEL_DEBUG(" masked");
                continue;
            }
            output_pixel.pixel = 0;

            // Cast input screen coordinates to shape coordinates to pick the pixel to draw
            int16_t pixel_to_get_x;
            int16_t pixel_to_get_y;
            screen_to_shape_coordinates(self, input_pixel.x, input_pixel.y, &pixel_to_get_x, &pixel_to_get_y);

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

            // vectorio shapes use 0 to mean "area is not covered."
            // We can skip all the rest of the work for this pixel if it's not currently covered by the shape.
            if (input_pixel.pixel == 0) {
                VECTORIO_SHAPE_PIXEL_DEBUG(" (encountered transparent pixel; input area is not fully covered)");
                full_coverage = false;
            } else {
                // Pixel is not transparent. Let's pull the pixel value index down to 0-base for more error-resistant palettes.
                input_pixel.pixel -= 1;
                output_pixel.opaque = true;

                if (self->pixel_shader == mp_const_none) {
                    output_pixel.pixel = input_pixel.pixel;
                } else if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
                    displayio_palette_get_color(self->pixel_shader, colorspace, &input_pixel, &output_pixel);
                } else if (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type)) {
                    displayio_colorconverter_convert(self->pixel_shader, colorspace, &input_pixel, &output_pixel);
                }

                // We double-check this to fast-path the case when a pixel is not covered by the shape & not call the color converter unnecessarily.
                if (!output_pixel.opaque) {
                    VECTORIO_SHAPE_PIXEL_DEBUG(" (encountered transparent pixel from colorconverter; input area is not fully covered)");
                    full_coverage = false;
                }

                *mask_doubleword |= 1u << mask_bit;
                if (colorspace->depth == 16) {
                    VECTORIO_SHAPE_PIXEL_DEBUG(" buffer = %04x 16", output_pixel.pixel);
                    *(((uint16_t *)buffer) + pixel_index) = output_pixel.pixel;
                } else if (colorspace->depth == 32) {
                    VECTORIO_SHAPE_PIXEL_DEBUG(" buffer = %04x 32", output_pixel.pixel);
                    *(((uint32_t *)buffer) + pixel_index) = output_pixel.pixel;
                } else if (colorspace->depth == 8) {
                    VECTORIO_SHAPE_PIXEL_DEBUG(" buffer = %02x 8", output_pixel.pixel);
                    *(((uint8_t *)buffer) + pixel_index) = output_pixel.pixel;
                } else if (colorspace->depth < 8) {
                    // Reorder the offsets to pack multiple rows into a byte (meaning they share a column).
                    if (!colorspace->pixels_in_byte_share_row) {
                        uint16_t row = pixel_index / linestride_px;
                        uint16_t col = pixel_index % linestride_px;
                        pixel_index = col * pixels_per_byte + (row / pixels_per_byte) * pixels_per_byte * linestride_px + row % pixels_per_byte;
                    }
                    uint8_t shift = (pixel_index % pixels_per_byte) * colorspace->depth;
                    if (colorspace->reverse_pixels_in_byte) {
                        // Reverse the shift by subtracting it from the leftmost shift.
                        shift = (pixels_per_byte - 1) * colorspace->depth - shift;
                    }
                    VECTORIO_SHAPE_PIXEL_DEBUG(" buffer = %2d %d", output_pixel.pixel, colorspace->depth);
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
        (double)(MAX(1, pixels * (1000000000.0 / (end - start)))),
        (double)(pixel_time / 1000.0),
        (double)(pixel_time / 1000.0 / pixels)
        );
    #endif
    VECTORIO_SHAPE_DEBUG(" -> pixels:%4d\n", (overlap.x2 - overlap.x1) * (overlap.y2 - overlap.y1));
    return full_coverage;
}


void vectorio_vector_shape_finish_refresh(vectorio_vector_shape_t *self) {
    if (displayio_area_empty(&self->ephemeral_dirty_area) && !self->current_area_dirty) {
        return;
    }
    VECTORIO_SHAPE_DEBUG("%p finish_refresh was:{(%3d,%3d), (%3d,%3d)}\n", self, self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);
    // Reset dirty area to nothing
    self->ephemeral_dirty_area.x1 = self->ephemeral_dirty_area.x2; // Cheat to set area to empty
    self->ephemeral_dirty_area.next = NULL;

    self->current_area_dirty = false; // We don't clear current area so we can remember what to clean up if we move
    self->current_area.next = NULL;

    VECTORIO_SHAPE_DEBUG("%p finish_refresh now:{(%3d,%3d), (%3d,%3d)}\n", self, self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);

    if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
        displayio_palette_finish_refresh(self->pixel_shader);
    } else if (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type)) {
        displayio_colorconverter_finish_refresh(self->pixel_shader);
    }
}


// Assembles a singly linked list of dirty areas from all components on the display.
displayio_area_t *vectorio_vector_shape_get_refresh_areas(vectorio_vector_shape_t *self, displayio_area_t *tail) {
    if (self->current_area_dirty
        || (mp_obj_is_type(self->pixel_shader, &displayio_palette_type) && displayio_palette_needs_refresh(self->pixel_shader))
        || (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type) && displayio_colorconverter_needs_refresh(self->pixel_shader))
        ) {
        if (!displayio_area_empty(&self->ephemeral_dirty_area)) {
            // Both are dirty, check if we should combine the areas or draw separately
            // Draws as few pixels as possible both when animations move short distances and large distances.
            // The display core implementation currently doesn't combine areas to reduce redrawing of masked areas. If it does,
            // this could be simplified to just return the 2 possibly overlapping areas.
            displayio_area_t area_swap;
            displayio_area_compute_overlap(&self->ephemeral_dirty_area, &self->current_area, &area_swap);
            uint32_t overlap_size = displayio_area_size(&area_swap);
            displayio_area_union(&self->ephemeral_dirty_area, &self->current_area, &area_swap); // Leave area_swap as the union area for later.
            uint32_t union_size = displayio_area_size(&area_swap);
            uint32_t current_size = displayio_area_size(&self->current_area);
            uint32_t dirty_size = displayio_area_size(&self->ephemeral_dirty_area);

            VECTORIO_SHAPE_DEBUG("%p get_refresh_area: dirty{(%3d,%3d), (%3d,%3d)} + current{(%3d,%3d), (%3d,%3d)} = union{(%3d,%3d), (%3d,%3d)}: union%d - dirty%d - curr%d + overlap%d = excluded%d : ", self,
                self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2,
                self->current_area.x1, self->current_area.y1, self->current_area.x2, self->current_area.y2,
                area_swap.x1, area_swap.y1, area_swap.x2, area_swap.y2,
                union_size, dirty_size, current_size, overlap_size, (int32_t)union_size - dirty_size - current_size + overlap_size
                );

            if ((int32_t)union_size - dirty_size - current_size + overlap_size <= MIN(dirty_size, current_size)) {
                // The excluded / non-overlapping area from the disjoint dirty and current areas is smaller
                // than the smallest area we need to draw. Redrawing the overlapping area would cost more
                // than just drawing the union disjoint area once.
                VECTORIO_SHAPE_DEBUG("combining to take disjoint area\n");
                displayio_area_copy(&area_swap, &self->ephemeral_dirty_area);
            } else {
                // The excluded area between the 2 dirty areas is larger than the smallest dirty area. It would be
                // more costly to combine these areas than possibly redraw some overlap.
                VECTORIO_SHAPE_DEBUG("excluded area too large, drawing separate area\n");
                self->current_area.next = tail;
                tail = &self->current_area;
            }

            self->ephemeral_dirty_area.next = tail;
            tail = &self->ephemeral_dirty_area;
        } else {
            self->current_area.next = tail;
            tail = &self->current_area;
            VECTORIO_SHAPE_DEBUG("%p get_refresh_area: redrawing current: {(%3d,%3d), (%3d,%3d)}\n", self, self->current_area.x1, self->current_area.y1, self->current_area.x2, self->current_area.y2);
        }
    } else if (!displayio_area_empty(&self->ephemeral_dirty_area)) {
        self->ephemeral_dirty_area.next = tail;
        tail = &self->ephemeral_dirty_area;
        VECTORIO_SHAPE_DEBUG("%p get_refresh_area redrawing dirty: {(%3d,%3d), (%3d,%3d)}\n", self, self->ephemeral_dirty_area.x1, self->ephemeral_dirty_area.y1, self->ephemeral_dirty_area.x2, self->ephemeral_dirty_area.y2);
    }
    return tail;
}

void vectorio_vector_shape_update_transform(vectorio_vector_shape_t *self, displayio_buffer_transform_t *group_transform) {
    self->absolute_transform = group_transform == NULL ? &null_transform : group_transform;
    common_hal_vectorio_vector_shape_set_dirty(self);
}
