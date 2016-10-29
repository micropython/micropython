#ifndef SAMD_NEOPIXEL_WRITE_H
#define SAMD_NEOPIXEL_WRITE_H

#include <stdint.h>
#include <stdbool.h>

extern void new_status_color(uint8_t r, uint8_t g, uint8_t b);
extern void temp_status_color(uint8_t r, uint8_t g, uint8_t b);
extern void clear_temp_status();

void samd_neopixel_write(uint32_t pin, uint8_t *pixels, uint32_t numBytes, bool is800KHz);

#endif
