#ifndef __FRAMEBUFFER_DISPLAY_H__
#define __FRAMEBUFFER_DISPLAU_H__

#include "driver_ili9341.h"

//This header file containes the defines used by the framebuffer driver.

#define FB_SIZE ILI9341_BUFFER_SIZE
#define FB_WIDTH ILI9341_WIDTH
#define FB_HEIGHT ILI9341_HEIGHT
#ifdef CONFIG_DRIVER_ILI9341_8C
        #define FB_TYPE_8CBPP
#else
        #define FB_TYPE_16BPP
#endif
#define FB_ALPHA_ENABLED
#define FB_FLUSH(buffer,eink_flags,x0,y0,x1,y1) driver_ili9341_write_partial(buffer, x0, y0, x1, y1)
#define FB_SET_BACKLIGHT(brightness) driver_ili9341_set_backlight(brightness > 127)
#define COLOR_FILL_DEFAULT 0x000000
#define COLOR_TEXT_DEFAULT 0xFFFFFF
#endif