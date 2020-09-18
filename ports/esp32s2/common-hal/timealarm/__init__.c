#include "esp_sleep.h"

#include "shared-bindings/timealarm/__init__.h"

void common_hal_timealarm_duration (uint32_t ms) {
    if (esp_sleep_enable_timer_wakeup((ms) * 1000) == ESP_ERR_INVALID_ARG) {
        mp_raise_ValueError(translate("time out of range"));
    }   
}  

