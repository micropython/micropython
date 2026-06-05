/*
 * The functions in this file serve as a simple way of
 * storing which area of the framebuffer needs to be
 * sent to the display during the next flush
 * 
 * (This only applies to the main framebuffer and not to the compositor frames!)
 */

#include "include/driver_framebuffer_internal.h"

#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE

/* Variables */
int16_t dirty_x0 = 0,          dirty_y0 = 0;           // Top-left corner of the "dirty" area
int16_t dirty_x1 = FB_WIDTH-1, dirty_y1 = FB_HEIGHT-1; // Bottom-right corner of the "dirty" area

/* Public functions */
bool driver_framebuffer_is_dirty()
{
	return (dirty_x1 >= dirty_x0) || (dirty_y1 >= dirty_y0);
}

void driver_framebuffer_set_dirty_area(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool force)
{
	if (force) {
		//Just set the dirty area
		dirty_x0 = x0;
		dirty_y0 = y0;
		dirty_x1 = x1;
		dirty_y1 = y1;
	} else {
		//Only increase the dirty area
		if (x0 < dirty_x0) dirty_x0 = x0;
		if (y0 < dirty_y0) dirty_y0 = y0;
		if (x1 > dirty_x1) dirty_x1 = x1;
		if (y1 > dirty_y1) dirty_y1 = y1;
	}
	if (dirty_x0 > FB_WIDTH-1)  dirty_x0 = FB_WIDTH  - 1;
	if (dirty_y0 > FB_HEIGHT-1) dirty_y0 = FB_HEIGHT - 1;
	if (dirty_x1 > FB_WIDTH-1)  dirty_x1 = FB_WIDTH  - 1;
	if (dirty_y1 > FB_HEIGHT-1) dirty_y1 = FB_HEIGHT - 1;
	if (dirty_x0 < 0) dirty_x0 = 0;
	if (dirty_y0 < 0) dirty_y0 = 0;
	if (dirty_x1 < 0) dirty_x1 = 0;
	if (dirty_y1 < 0) dirty_y1 = 0;
}

void driver_framebuffer_get_dirty_area(int16_t* x0, int16_t* y0, int16_t* x1, int16_t* y1)
{
	*x0 = dirty_x0;
	*y0 = dirty_y0;
	*x1 = dirty_x1;
	*y1 = dirty_y1;
}

#endif /* CONFIG_DRIVER_FRAMEBUFFER_ENABLE */
