#include "esp_sleep.h"

#include "shared-bindings/alarm_time/__init__.h"

void common_hal_alarm_time_duration (uint32_t ms) {
    if (esp_sleep_enable_timer_wakeup((ms) * 1000) == ESP_ERR_INVALID_ARG) {
        mp_raise_ValueError(translate("time out of range"));
    }   
}  

void common_hal_alarm_time_disable (void) {
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
}
