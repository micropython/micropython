/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "py/runtime.h"

#if CIRCUITPY_ALARM
#include "common-hal/alarm/__init__.h"
#endif
#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"
#include "supervisor/shared/translate/translate.h"
#include "tempdrv.h"
#include "em_system.h"
#include "em_cmu.h"

float common_hal_mcu_processor_get_temperature(void) {
    TEMPDRV_Init();
    return (float)TEMPDRV_GetTemp();
}

float common_hal_mcu_processor_get_voltage(void) {
    // xG24 does not have built-in direct reading of processor voltage
    // Have Only 1 of IADC, already used for analogio module
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return CMU_ClockFreqGet(cmuClock_HCLK);
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {

    size_t byte_index;
    uint64_t serial = SYSTEM_GetUnique();
    for (byte_index = 0; byte_index < sizeof(uint64_t); byte_index++) {
        raw_id[byte_index] = (serial >> (64 - ((byte_index + 1) * 8))) & 0xff;
    }
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    return RESET_REASON_UNKNOWN;
}
