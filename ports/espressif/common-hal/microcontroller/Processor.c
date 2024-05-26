// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include <string.h>

#include "py/runtime.h"

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

#include "esp_sleep.h"
#include "esp_system.h"

#include "soc/efuse_reg.h"

#if !defined(CONFIG_IDF_TARGET_ESP32)
#include "driver/temperature_sensor.h"
#endif

float common_hal_mcu_processor_get_temperature(void) {
    float tsens_value;
    #if defined(CONFIG_IDF_TARGET_ESP32)
    return NAN;
    #else
    temperature_sensor_handle_t temp_sensor = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80); // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temperature_sensor_install(&temp_sensor_config, &temp_sensor);
    temperature_sensor_enable(temp_sensor);
    temperature_sensor_get_celsius(temp_sensor, &tsens_value);
    temperature_sensor_disable(temp_sensor);
    temperature_sensor_uninstall(temp_sensor);
    #endif
    return tsens_value;
}

float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 1000000;
}

static uint8_t swap_nibbles(uint8_t v) {
    return ((v << 4) | (v >> 4)) & 0xff;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    memset(raw_id, 0, COMMON_HAL_MCU_PROCESSOR_UID_LENGTH);

    uint8_t *ptr = &raw_id[COMMON_HAL_MCU_PROCESSOR_UID_LENGTH - 1];
    // MAC address contains 48 bits (6 bytes), 32 in the low order word

    #if defined(CONFIG_IDF_TARGET_ESP32)
    uint32_t mac_address_part = REG_READ(EFUSE_BLK0_RDATA1_REG);
    #elif defined(CONFIG_IDF_TARGET_ESP32H2)
    uint32_t mac_address_part = REG_READ(EFUSE_RD_MAC_SYS_0_REG);
    #elif defined(CONFIG_IDF_TARGET_ESP32C2)
    uint32_t mac_address_part = REG_READ(EFUSE_RD_BLK2_DATA0_REG);
    #else
    uint32_t mac_address_part = REG_READ(EFUSE_RD_MAC_SPI_SYS_0_REG);
    #endif

    *ptr-- = swap_nibbles(mac_address_part & 0xff);
    mac_address_part >>= 8;
    *ptr-- = swap_nibbles(mac_address_part & 0xff);
    mac_address_part >>= 8;
    *ptr-- = swap_nibbles(mac_address_part & 0xff);
    mac_address_part >>= 8;
    *ptr-- = swap_nibbles(mac_address_part & 0xff);

    // and 16 in the high order word
    #if defined(CONFIG_IDF_TARGET_ESP32)
    mac_address_part = REG_READ(EFUSE_BLK0_RDATA2_REG);
    #elif defined(CONFIG_IDF_TARGET_ESP32H2)
    mac_address_part = REG_READ(EFUSE_RD_MAC_SYS_1_REG);
    #elif defined(CONFIG_IDF_TARGET_ESP32C2)
    mac_address_part = REG_READ(EFUSE_RD_BLK2_DATA1_REG);
    #else
    mac_address_part = REG_READ(EFUSE_RD_MAC_SPI_SYS_1_REG);
    #endif

    *ptr-- = swap_nibbles(mac_address_part & 0xff);
    mac_address_part >>= 8;
    *ptr-- = swap_nibbles(mac_address_part & 0xff);
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    switch (esp_reset_reason()) {
        case ESP_RST_POWERON:
            return RESET_REASON_POWER_ON;

        case ESP_RST_SW:
        case ESP_RST_PANIC:
            return RESET_REASON_SOFTWARE;

        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
        case ESP_RST_WDT:
            return RESET_REASON_WATCHDOG;

        case ESP_RST_BROWNOUT:
            return RESET_REASON_BROWNOUT;

        case ESP_RST_SDIO:
        case ESP_RST_EXT:
            return RESET_REASON_RESET_PIN;

        case ESP_RST_DEEPSLEEP:
            switch (esp_sleep_get_wakeup_cause()) {
                case ESP_SLEEP_WAKEUP_TIMER:
                case ESP_SLEEP_WAKEUP_EXT0:
                case ESP_SLEEP_WAKEUP_EXT1:
                case ESP_SLEEP_WAKEUP_TOUCHPAD:
                case ESP_SLEEP_WAKEUP_ULP:
                    return RESET_REASON_DEEP_SLEEP_ALARM;

                case ESP_SLEEP_WAKEUP_UNDEFINED:
                default:
                    return RESET_REASON_UNKNOWN;
            }

        case ESP_RST_UNKNOWN:
        default:
            return RESET_REASON_UNKNOWN;

    }
}
