#pragma once

#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"

#include "driver_framebuffer_font.h"
#include "driver_framebuffer_orientation_internal.h"
#include "driver_framebuffer_dirty.h"
#include "driver_framebuffer_compositor.h"
#include "driver_framebuffer_orientation.h"
#include "driver_framebuffer_drawing.h"
#include "driver_framebuffer_text.h"
#include "driver_framebuffer_matrix.h"

//PNG library
#include "../png/mem_reader.h"
#include "../png/file_reader.h"
#include "../png/png_reader.h"

/* Flags */
#define FB_FLAG_FORCE          1
#define FB_FLAG_FULL           2
#define FB_FLAG_LUT_GREYSCALE  4
#define FB_FLAG_LUT_NORMAL     8
#define FB_FLAG_LUT_FAST      16
#define FB_FLAG_LUT_FASTEST   32
#define FB_FLAG_CLEAR_DEPTH   64

/* Colors */
#define COLOR_BLACK   0x000000
#define COLOR_WHITE   0xFFFFFF
#define COLOR_RED     0xFF0000
#define COLOR_GREEN   0x00FF00
#define COLOR_BLUE    0x0000FF
#define COLOR_YELLOW  0xFFFF00
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_CYAN    0x00FFFF

#ifdef __cplusplus
extern "C" {
#endif

extern matrix_stack_2d stack_2d_global;
extern matrix_stack_3d stack_3d_global;
extern depth_buffer_3d depth_buffer_global;
extern bool is_3d_global;

esp_err_t driver_framebuffer_init();
/* Initialize the framebuffer driver (called once at system boot from platform.c) */

bool driver_framebuffer_flush(uint32_t flags);
/* Flush the framebuffer to the display */

void driver_framebuffer_fill(Window* window, uint32_t value);
/* Fill the framebuffer or the provided frame with a single color */

void driver_framebuffer_setPixel(Window* window, int16_t x, int16_t y, uint32_t value);
/* Set a pixel in the framebuffer or the provided frame to a color */

uint32_t driver_framebuffer_getPixel(Window* window, int16_t x, int16_t y);
/* Get the color of a pixel in the framebuffer or the provided frame */

uint16_t driver_framebuffer_getWidth(Window* window);
/* Get the width of the framebuffer or the provided window */

uint16_t driver_framebuffer_getHeight(Window* window);
/* Get the height of the framebuffer or the provided window */

esp_err_t driver_framebuffer_png(Window* window, int16_t x, int16_t y, lib_reader_read_t reader, void* reader_p);
/* Draw a PNG image to the framebuffer of the provided window */

void driver_framebuffer_blit(Window* source, Window* target);
/* Blit a window to the framebuffer of another window or the main framebuffer */

esp_err_t driver_framebuffer_setBacklight(uint8_t level);
/* Set the brightness of the backlight (0-255) */

uint8_t driver_framebuffer_getBacklight();
/* Get the brightness of the backlight */

#ifdef __cplusplus
}
#endif
