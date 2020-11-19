#include "shared-bindings/alarm_io/__init__.h"

#include "esp_sleep.h"
#include "driver/rtc_io.h"

mp_obj_t common_hal_alarm_io_pin_state (alarm_io_obj_t *self_in) {
    if (!rtc_gpio_is_valid_gpio(self_in->gpio)) {
        mp_raise_ValueError(translate("io must be rtc io"));
    }

    if (self_in->pull && !self_in->level) {
        for (uint8_t i = 0; i<=4; i+=2) {
            if (self_in->gpio == i) {
                mp_raise_ValueError(translate("IOs 0, 2 & 4 do not support internal pullup in sleep"));
            }
        }
    }

    switch(esp_sleep_enable_ext0_wakeup(self_in->gpio, self_in->level)) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_ValueError(translate("trigger level must be 0 or 1"));
        case ESP_ERR_INVALID_STATE:
            mp_raise_RuntimeError(translate("wakeup conflict"));
        default:
            break;
    }

    if (self_in->pull) { (self_in->level) ? rtc_gpio_pulldown_en(self_in->gpio) : rtc_gpio_pullup_en(self_in->gpio); }

    return self_in;
}

void common_hal_alarm_io_disable (void) {
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_EXT0 | ESP_SLEEP_WAKEUP_EXT1);
}
