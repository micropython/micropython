#ifndef __MICROPY_INCLUDED_ATMEL_SAMD_RGB_LED_STATUS_H
#define __MICROPY_INCLUDED_ATMEL_SAMD_RGB_LED_STATUS_H

#include <stdint.h>
#include <stdbool.h>

#include "mpconfigport.h"
#include "rgb_led_colors.h"

#if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK) && !defined(CIRCUITPY_BITBANG_APA102)
#include "common-hal/busio/SPI.h"
extern busio_spi_obj_t status_apa102;
#endif

void rgb_led_status_init(void);
void reset_status_led(void);
void new_status_color(uint32_t rgb);
void temp_status_color(uint32_t rgb);
void clear_temp_status(void);

uint32_t color_brightness(uint32_t color, uint8_t brightness);

#endif  // __MICROPY_INCLUDED_ATMEL_SAMD_RGB_LED_STATUS_H
