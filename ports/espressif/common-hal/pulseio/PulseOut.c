/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "common-hal/pulseio/PulseOut.h"

#include "shared-bindings/pwmio/PWMOut.h"
#include "py/runtime.h"

// Requires rmt.c void esp32s2_peripherals_reset_all(void) to reset

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle) {

    rmt_channel_t channel = esp32s2_peripherals_find_and_reserve_rmt();
    if (channel == RMT_CHANNEL_MAX) {
        mp_raise_RuntimeError(translate("All timers in use"));
    }

    // Configure Channel
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(pin->number, channel);
    config.tx_config.carrier_en = true;
    config.tx_config.carrier_duty_percent = (duty_cycle * 100) / (1 << 16);
    config.tx_config.carrier_freq_hz = frequency;
    config.clk_div = 80;

    rmt_config(&config);
    rmt_driver_install(channel, 0, 0);

    self->channel = channel;
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self) {
    return self->channel == RMT_CHANNEL_MAX;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self) {
    esp32s2_peripherals_free_rmt(self->channel);
    self->channel = RMT_CHANNEL_MAX;

}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t length) {
    rmt_item32_t items[length];

    // Circuitpython allows 16 bit pulse values, while ESP32 only allows 15 bits
    // Thus, we use entire items for one pulse, rather than switching inside each item
    for (size_t i = 0; i < length; i++) {
        // Setting the RMT duration to 0 has undefined behavior, so avoid that pre-emptively.
        if (pulses[i] == 0) {
            pulses[i] = 1;
        }
        uint32_t level = (i % 2) ? 0 : 1;
        const rmt_item32_t item = {{{ (pulses[i] & 0x8000 ? 0x7FFF : 1), level, (pulses[i] & 0x7FFF), level}}};
        items[i] = item;
    }

    rmt_write_items(self->channel, items, length, true);
    while (rmt_wait_tx_done(self->channel, 0) != ESP_OK) {
        RUN_BACKGROUND_TASKS;
    }
}
