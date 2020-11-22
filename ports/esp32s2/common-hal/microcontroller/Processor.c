/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <math.h>
#include <string.h>

#include "py/runtime.h"

#include "common-hal/microcontroller/Processor.h"
#include "supervisor/shared/translate.h"

#include "soc/efuse_reg.h"

#include "components/driver/esp32s2/include/driver/temp_sensor.h"

float common_hal_mcu_processor_get_temperature(void) {
    float tsens_out;
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT(); // DEFAULT: range:-10℃ ~  80℃, error < 1℃.
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
    temp_sensor_read_celsius(&tsens_out);
    temp_sensor_stop();
    return tsens_out;
}

float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return 0;
}

STATIC uint8_t swap_nibbles(uint8_t v) {
    return ((v << 4) | (v >> 4)) & 0xff;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    memset(raw_id, 0, COMMON_HAL_MCU_PROCESSOR_UID_LENGTH);

    uint8_t *ptr = &raw_id[COMMON_HAL_MCU_PROCESSOR_UID_LENGTH-1];
    // MAC address contains 48 bits (6 bytes), 32 in the low order word
    uint32_t mac_address_part = REG_READ(EFUSE_RD_MAC_SPI_SYS_0_REG);
    *ptr-- = swap_nibbles(mac_address_part & 0xff); mac_address_part >>= 8;
    *ptr-- = swap_nibbles(mac_address_part & 0xff); mac_address_part >>= 8;
    *ptr-- = swap_nibbles(mac_address_part & 0xff); mac_address_part >>= 8;
    *ptr-- = swap_nibbles(mac_address_part & 0xff);

    // and 16 in the high order word
    mac_address_part = REG_READ(EFUSE_RD_MAC_SPI_SYS_1_REG);
    *ptr-- = swap_nibbles(mac_address_part & 0xff); mac_address_part >>= 8;
    *ptr-- = swap_nibbles(mac_address_part & 0xff);
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_TIMER:
            return RESET_REASON_DEEP_SLEEP_ALARM;

        case ESP_SLEEP_WAKEUP_EXT0:
            return RESET_REASON_DEEP_SLEEP_ALARM;

        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            //TODO: implement TouchIO
        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            return RESET_REASON_POWER_APPLIED;
    }
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    return RESET_REASON_POWER_ON;
}
