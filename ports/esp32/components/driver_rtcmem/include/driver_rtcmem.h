#ifndef DRIVER_RTCMEM_H
#define DRIVER_RTCMEM_H

#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize RTC memory subsystem (optional for IDF 5.x)
esp_err_t driver_rtcmem_init(void);

// Integer API
esp_err_t driver_rtcmem_int_write(int pos, int val);
esp_err_t driver_rtcmem_int_read(int pos, int *val);

// String API
esp_err_t driver_rtcmem_string_write(const char *str);
esp_err_t driver_rtcmem_string_read(const char **str);

// Clear all RTC memory regions used by this driver
esp_err_t driver_rtcmem_clear(void);

#ifdef __cplusplus
}
#endif

#endif // DRIVER_RTCMEM_H
