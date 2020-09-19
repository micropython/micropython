#include "shared-bindings/io_alarm/__init__.h"

#include "esp_sleep.h"
#include "driver/rtc_io.h"

void common_hal_io_alarm_pin_state (uint8_t gpio, uint8_t level, bool pull) {
    if (!rtc_gpio_is_valid_gpio(gpio))  {
        mp_raise_ValueError(translate("io must be rtc io"));
        return;        
    }   

    switch(esp_sleep_enable_ext0_wakeup(gpio, level)) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_ValueError(translate("trigger level must be 0 or 1"));
            return;
        case ESP_ERR_INVALID_STATE:
            mp_raise_RuntimeError(translate("wakeup conflict"));
            return;
        default: 
            break;   
    }

    if (pull) {
        (level) ? rtc_gpio_pulldown_en(gpio) : rtc_gpio_pullup_en(gpio);
    }     
}  

