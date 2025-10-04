#ifndef MICROPY_INCLUDED_ESP32_MODESP32_H
#define MICROPY_INCLUDED_ESP32_MODESP32_H

#include "driver/rmt_tx.h"

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3

    #define RTC_VALID_EXT_PINS \
    ( \
    (1ll << 0) | \
    (1ll << 1) | \
    (1ll << 2) | \
    (1ll << 3) | \
    (1ll << 4) | \
    (1ll << 5) | \
    (1ll << 6) | \
    (1ll << 7) | \
    (1ll << 8) | \
    (1ll << 9) | \
    (1ll << 10) | \
    (1ll << 11) | \
    (1ll << 12) | \
    (1ll << 13) | \
    (1ll << 14) | \
    (1ll << 15) | \
    (1ll << 16) | \
    (1ll << 17) | \
    (1ll << 18) | \
    (1ll << 19) | \
    (1ll << 20) | \
    (1ll << 21)   \
    )
    #define RTC_LAST_EXT_PIN 21

#else

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

#endif

#define RTC_IS_VALID_EXT_PIN(pin_id) ((1ll << (pin_id)) & RTC_VALID_EXT_PINS)

extern bool esp32_rmt_bitstream_enabled;

extern const mp_obj_type_t esp32_nvs_type;
extern const mp_obj_type_t esp32_partition_type;
extern const mp_obj_type_t esp32_rmt_type;
extern const mp_obj_type_t esp32_ulp_type;

#if MICROPY_PY_ESP32_PCNT
extern const mp_obj_type_t esp32_pcnt_type;

void esp32_pcnt_deinit_all(void);
#endif

#endif // MICROPY_INCLUDED_ESP32_MODESP32_H
