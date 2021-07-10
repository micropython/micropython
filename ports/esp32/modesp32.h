#ifndef MICROPY_INCLUDED_ESP32_MODESP32_H
#define MICROPY_INCLUDED_ESP32_MODESP32_H

#define RTC_VALID_EXT_PINS \
    ( \
    (1ll << 0) | \
    (1ll << 2) | \
    (1ll << 4) | \
    (1ll << 12) | \
    (1ll << 13) | \
    (1ll << 14) | \
    (1ll << 15) | \
    (1ll << 25) | \
    (1ll << 26) | \
    (1ll << 27) | \
    (1ll << 32) | \
    (1ll << 33) | \
    (1ll << 34) | \
    (1ll << 35) | \
    (1ll << 36) | \
    (1ll << 37) | \
    (1ll << 38) | \
    (1ll << 39)   \
    )

#define RTC_LAST_EXT_PIN 39
#define RTC_IS_VALID_EXT_PIN(pin_id) ((1ll << (pin_id)) & RTC_VALID_EXT_PINS)

extern const mp_obj_type_t esp32_nvs_type;
extern const mp_obj_type_t esp32_partition_type;
extern const mp_obj_type_t esp32_rmt_type;
extern const mp_obj_type_t esp32_ulp_type;

#endif // MICROPY_INCLUDED_ESP32_MODESP32_H
