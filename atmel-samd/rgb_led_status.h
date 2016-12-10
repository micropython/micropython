#ifndef __MICROPY_INCLUDED_ATMEL_SAMD_RGB_LED_STATUS_H
#define __MICROPY_INCLUDED_ATMEL_SAMD_RGB_LED_STATUS_H

#include <stdint.h>
#include <stdbool.h>

#include "rgb_led_colors.h"

extern void rgb_led_status_init(void);
extern void new_status_color(uint32_t rgb);
extern void temp_status_color(uint32_t rgb);
extern void clear_temp_status(void);

uint32_t color_brightness(uint32_t color, uint8_t brightness);

#endif  // __MICROPY_INCLUDED_ATMEL_SAMD_RGB_LED_STATUS_H
