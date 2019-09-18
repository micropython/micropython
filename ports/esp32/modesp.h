
uint32_t esp_neopixel_write(uint8_t pin, uint8_t *pixels, uint32_t numBytes, uint8_t timing);

uint32_t esp_neopixel_write_timings(uint8_t pin, uint8_t *pixel_bytes, uint32_t numBytes, 
                                      uint32_t t0high, uint32_t t1high, uint32_t t0low, uint32_t t1low, uint32_t t_latch);
