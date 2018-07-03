#ifndef MICROPY_INCLUDED_ESP8266_ESPNEOPIXEL_H
#define MICROPY_INCLUDED_ESP8266_ESPNEOPIXEL_H

uint32_t esp_neopixel_write(uint8_t pin, uint8_t *pixels, uint32_t numBytes, bool is800KHz, float frac, uint32_t max);

#endif // MICROPY_INCLUDED_ESP8266_ESPNEOPIXEL_H
