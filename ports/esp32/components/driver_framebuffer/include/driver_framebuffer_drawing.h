#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver_framebuffer_matrix_drawing.h"

#ifdef __cplusplus
extern "C" {
#endif

void driver_framebuffer_line(Window* window, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);
/* Draw a line from point (x0, y0) to point (x1, y1) */

void driver_framebuffer_rect(Window* window, int16_t x, int16_t y, uint16_t w, uint16_t h, bool fill, uint32_t color);
/* Draw a rectangle (filled or only the outline) from point (x, y) to point (x+w, y+h) */

void driver_framebuffer_circle(Window* window, int16_t x0, int16_t y0, uint16_t r, uint16_t a0, uint16_t a1, bool fill, uint32_t color);
/* Draw a circle (filled or only the outline) at center point (x0, y0) with a radius r starting at angle a0 and ending at angle a1 */

#ifdef __cplusplus
}
#endif
