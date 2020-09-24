#include "esp_sleep.h"

#include "shared-bindings/alarm_touch/__init__.h"

void common_hal_alarm_touch_disable (void) {
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TOUCHPAD);
}
