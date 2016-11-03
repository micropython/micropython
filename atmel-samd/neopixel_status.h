#ifndef __MICROPY_INCLUDED_ATMEL_SAMD_NEOPIXEL_STATUS_H
#define __MICROPY_INCLUDED_ATMEL_SAMD_NEOPIXEL_STATUS_H

#include <stdint.h>
#include <stdbool.h>

extern void neopixel_status_init(void);
extern void new_status_color(uint8_t r, uint8_t g, uint8_t b);
extern void temp_status_color(uint8_t r, uint8_t g, uint8_t b);
extern void clear_temp_status(void);

#endif  // __MICROPY_INCLUDED_ATMEL_SAMD_NEOPIXEL_STATUS_H
