#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "driver_framebuffer_compositor.h"

#ifdef __cplusplus
extern "C" {
#endif

enum Orientation driver_framebuffer_get_orientation(Window *window);
/* Get the orientation of the window */

void driver_framebuffer_set_orientation(Window *window, enum Orientation newOrientation);
/* Set the orientation of the window */

uint16_t driver_framebuffer_get_orientation_angle(Window *window);
/* Get the orientation of the window as angle */

void driver_framebuffer_set_orientation_angle(Window *window, uint16_t angle);
/* Set the orientation of the window as angle */

bool driver_framebuffer_orientation_apply(Window *window, int16_t *x, int16_t *y);
/* Apply the orientation of the window to the provided coordinates. (the provided coordinates are from the users perspective) */

void driver_framebuffer_orientation_revert(Window *window, int16_t *x, int16_t *y);
/* Revert the orientation of the window to the provided coordinates. (the provided coordinates are from the internal perspective) */

void driver_framebuffer_orientation_revert_square(Window *window, int16_t* x0, int16_t* y0, int16_t* x1, int16_t* y1);
/* Revert the orientation of the window to the provided square coordinates. */

void driver_framebuffer_get_orientation_size(Window* window, int16_t* width, int16_t* height);
/* Get the width and height of the framebuffer or a window, orientation corrected */

#ifdef __cplusplus
}
#endif
