#include <string.h>
#include "driver_rtcmem.h"
#include "esp_log.h"
#include "esp_attr.h"

#define TAG "rtcmem"

#define STATIC static

#define RTC_MEM_INT_SIZE 64
#define RTC_MEM_STR_SIZE 64

// ---------------------------------------------------------------------------
// RTC memory storage (modern ESP-IDF 5.x method)
// ---------------------------------------------------------------------------

RTC_SLOW_ATTR static int  rtc_mem_int[RTC_MEM_INT_SIZE];
RTC_SLOW_ATTR static char rtc_mem_str[RTC_MEM_STR_SIZE];

// ---------------------------------------------------------------------------
// Initialization (kept for API compatibility)
// ---------------------------------------------------------------------------

esp_err_t driver_rtcmem_init(void) {
    // Nothing to initialize in IDF 5.x, but keep API stable
    return ESP_OK;
}

// ---------------------------------------------------------------------------
// Integer API
// ---------------------------------------------------------------------------

esp_err_t driver_rtcmem_int_write(int pos, int val) {
    if (pos < 0 || pos >= RTC_MEM_INT_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }
    rtc_mem_int[pos] = val;
    return ESP_OK;
}

esp_err_t driver_rtcmem_int_read(int pos, int *val) {
    if (!val) {
        return ESP_ERR_INVALID_ARG;
    }
    if (pos < 0 || pos >= RTC_MEM_INT_SIZE) {
        *val = 0;
        return ESP_ERR_INVALID_ARG;
    }
    *val = rtc_mem_int[pos];
    return ESP_OK;
}

// ---------------------------------------------------------------------------
// String API
// ---------------------------------------------------------------------------

esp_err_t driver_rtcmem_string_write(const char *str) {
    if (!str) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t len = strlen(str);
    if (len > RTC_MEM_STR_SIZE) {
        len = RTC_MEM_STR_SIZE;
    }

    memset(rtc_mem_str, 0, RTC_MEM_STR_SIZE);
    memcpy(rtc_mem_str, str, len);

    return ESP_OK;
}

esp_err_t driver_rtcmem_string_read(const char **str) {
    if (!str) {
        return ESP_ERR_INVALID_ARG;
    }

    // Return pointer directly into RTC memory
    *str = rtc_mem_str;
    return ESP_OK;
}

// ---------------------------------------------------------------------------
// Clear API
// ---------------------------------------------------------------------------

esp_err_t driver_rtcmem_clear(void) {
    memset(rtc_mem_int, 0, sizeof(rtc_mem_int));
    memset(rtc_mem_str, 0, sizeof(rtc_mem_str));
    return ESP_OK;
}

// ---------------------------------------------------------------------------
// Optional debug dump
// ---------------------------------------------------------------------------

void driver_rtcmem_dump(void) {
    ESP_LOGI(TAG, "RTC int values:");
    for (int i = 0; i < RTC_MEM_INT_SIZE; i++) {
        ESP_LOGI(TAG, "  [%02d] = %d", i, rtc_mem_int[i]);
    }

    ESP_LOGI(TAG, "RTC string:");
    ESP_LOG_BUFFER_HEX(TAG, rtc_mem_str, RTC_MEM_STR_SIZE);
}
