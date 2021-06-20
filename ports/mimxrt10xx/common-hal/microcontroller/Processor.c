/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Dan Halbert for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

#include "fsl_tempmon.h"
#include "fsl_ocotp.h"
#include "clocks.h"

float common_hal_mcu_processor_get_temperature(void) {
    tempmon_config_t config;
    TEMPMON_GetDefaultConfig(&config);

    OCOTP_Init(OCOTP, CLOCK_GetFreq(kCLOCK_IpgClk));
    TEMPMON_Init(TEMPMON, &config);
    TEMPMON_StartMeasure(TEMPMON);

    const float temp = TEMPMON_GetCurrentTemperature(TEMPMON);
    TEMPMON_Deinit(TEMPMON);
    OCOTP_Deinit(OCOTP);

    return temp;
}

float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return SystemCoreClock;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    OCOTP_Init(OCOTP, CLOCK_GetFreq(kCLOCK_IpgClk));

    // Reads shadow registers 0x01 - 0x04 (Configuration and Manufacturing Info)
    // into 8 bit wide destination, avoiding punning.
    for (int i = 0; i < 4; ++i) {
        uint32_t wr = OCOTP_ReadFuseShadowRegister(OCOTP, i + 1);
        for (int j = 0; j < 4; j++) {
            raw_id[i * 4 + j] = wr & 0xff;
            wr >>= 8;
        }
    }
    OCOTP_Deinit(OCOTP);
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    return RESET_REASON_UNKNOWN;
}
