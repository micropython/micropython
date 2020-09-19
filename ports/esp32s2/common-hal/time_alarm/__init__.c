#include "esp_sleep.h"

#include "shared-bindings/time_alarm/__init__.h"

void common_hal_time_alarm_duration (uint32_t ms) {
    if (esp_sleep_enable_timer_wakeup((ms) * 1000) == ESP_ERR_INVALID_ARG) {
        mp_raise_ValueError(translate("time out of range"));
    }   
}  

