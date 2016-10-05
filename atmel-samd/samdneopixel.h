#ifndef SAMD_NEOPIXEL_WRITE_H
#define SAMD_NEOPIXEL_WRITE_H

#include <stdint.h>
#include <stdbool.h>

void samd_neopixel_write(uint32_t pin, uint8_t *pixels, uint32_t numBytes, bool is800KHz);

#endif
