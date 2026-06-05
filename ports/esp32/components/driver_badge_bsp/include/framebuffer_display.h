#ifndef __FRAMEBUFFER_DISPLAY_H__
#define __FRAMEBUFFER_DISPLAU_H__

#include "driver_badge_bsp.h"

//This header file containes the defines used by the framebuffer driver.

#define FB_SIZE driver_badge_bsp_get_size()
#define FB_WIDTH driver_badge_bsp_get_width()
#define FB_HEIGHT driver_badge_bsp_get_height()
#define FB_TYPE_16BPP
#define FB_ALPHA_ENABLED
#define FB_FLUSH(buffer,eink_flags,x0,y0,x1,y1) driver_badge_bsp_flush(buffer, x0, y0, x1, y1);
#define FB_SET_BACKLIGHT(brightness) driver_badge_bsp_set_backlight_brightness(brightness)
#define COLOR_FILL_DEFAULT 0x000000
#define COLOR_TEXT_DEFAULT 0xFFFFFF
#endif
