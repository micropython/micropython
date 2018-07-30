#ifndef MICROPY_INCLUDED_ESP8266_ESPNEOPIXEL_H
#define MICROPY_INCLUDED_ESP8266_ESPNEOPIXEL_H

void esp_neopixel_write(uint8_t pin, uint8_t *pixels, uint32_t numBytes, bool is800KHz);

#endif // MICROPY_INCLUDED_ESP8266_ESPNEOPIXEL_H
