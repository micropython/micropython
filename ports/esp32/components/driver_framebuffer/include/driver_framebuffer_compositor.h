#pragma once

#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "esp_system.h"
#include "driver_framebuffer_orientation_internal.h"
#include "driver_framebuffer_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t depth_buffer_type_t;

#define RENDERMODE_LINE 0x00			// (uint32_t color) Renders a colored line accoring to the first two points.
#define RENDERMODE_SOLID 0x01			// (uint32_t color) Renders a colored triangle.
#define RENDERMODE_TEXTURED 0x02		// (texture_2d* tx) Renders a textured triangle.
#define RENDERMODE_WIRE 0x03			// (uint32_t color) Renders a colored outline of a triangle.

typedef struct triangle_3d_buffered_t {
	float x0, y0, z0;
	float x1, y1, z1;
	float x2, y2, z2;
	uint32_t param;				// Rendering parameter: type is specified by the mode.
	uint8_t mode;               // Triangle rendering mode.
} triangle_3d_buffered;

typedef struct triangle_buffer_3d_t {
	triangle_3d_buffered *triangles;   // Circular buffer of triangles.
	int providedIndex;                 // The index of the provided triangles; more triangles can be given if (providedIndex + 1) % CONFIG_LIB3D_TRI_BUFFER_SIZE != usedIndex.
	int usedIndex;                     // The index of the used triangles.
} triangle_buffer_3d;

typedef struct depth_buffer_3d_t {
    depth_buffer_type_t *buffer;
    uint16_t width;
    uint16_t height;
    bool is_clear;
} depth_buffer_3d;

typedef struct Window_t {
	/* Linked list */
	struct Window_t* _prevWindow;
	struct Window_t* _nextWindow;
	
	/* Properties */
	char* name;                     //The name of the window
	int16_t x, y;                   // Position (x, y)
	uint16_t width, height;         // Buffer size (width, height)
	enum Orientation orientation;   // Current orientation
	bool enableTransparentColor;    // Enable transparency
	uint32_t transparentColor;      // Which color is made transparent
	
	/* Automatic rendering settings */
	bool visible;                   // Visible or hidden
	int16_t hOffset, vOffset;       // Drawing offset (x, y)
	uint16_t drawWidth, drawHeight; // Drawing size (width, height)
	
	/* Buffer */
	uint8_t* buffer;
	depth_buffer_3d* depth_buffer;  // 3D depth buffer

	/* Graphics */
	bool is_3d;
	matrix_stack_2d* stack_2d;      // 2D matrix stack
	matrix_stack_3d* stack_3d;      // 3D matrix stack
} Window;

extern matrix_stack_2d stack_2d_global;

Window* driver_framebuffer_window_create(const char* name, uint16_t width, uint16_t height);
/* Create a window */

void driver_framebuffer_window_remove(Window* window);
/* Delete a window */

bool driver_framebuffer_window_rename(Window* window, const char* newName);
/* Rename a window */

Window* driver_framebuffer_window_find(const char* name);
/* Find an existing window by name */

Window* driver_framebuffer_window_first();
/* Find the first window */

Window* driver_framebuffer_window_next(Window* currentWindow);
/* Find the next window */

Window* driver_framebuffer_window_last();
/* Find the last window */

void driver_framebuffer_window_focus(Window* window);
/* Move a window to the end of the list */

void driver_framebuffer_window_getSize(Window* window, int16_t* width, int16_t* height);
/* Get the width and height of a window */

#ifdef __cplusplus
}
#endif
