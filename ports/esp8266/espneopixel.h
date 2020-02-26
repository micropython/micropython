#ifndef MICROPY_INCLUDED_ESP8266_ESPNEOPIXEL_H
#define MICROPY_INCLUDED_ESP8266_ESPNEOPIXEL_H

void esp_neopixel_write(uint8_t pin, uint8_t *pixels, uint32_t numBytes, bool is800KHz);
void esp_my9291_write(uint8_t pinDI, uint8_t pinDCK, uint8_t *data, uint32_t numBytes);

#endif // MICROPY_INCLUDED_ESP8266_ESPNEOPIXEL_H
