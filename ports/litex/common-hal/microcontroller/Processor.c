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
#include "py/runtime.h"

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"
#include "supervisor/shared/translate.h"

#include "csr.h"
#include "generated/soc.h"

float common_hal_mcu_processor_get_temperature(void) {
    return NAN;
}

float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return CONFIG_CLOCK_FREQUENCY;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    raw_id[0] = csr_readl(CSR_VERSION_MAJOR_ADDR);
    raw_id[1] = csr_readl(CSR_VERSION_MINOR_ADDR);
    raw_id[2] = csr_readl(CSR_VERSION_REVISION_ADDR);
    raw_id[3] = csr_readl(CSR_VERSION_GITREV_ADDR + 0);
    raw_id[4] = csr_readl(CSR_VERSION_GITREV_ADDR + 4);
    raw_id[5] = csr_readl(CSR_VERSION_GITREV_ADDR + 8);
    raw_id[6] = csr_readl(CSR_VERSION_GITREV_ADDR + 12);
    raw_id[7] = csr_readl(CSR_VERSION_GITEXTRA_ADDR + 0);
    raw_id[8] = csr_readl(CSR_VERSION_GITEXTRA_ADDR + 4);
    raw_id[9] = csr_readl(CSR_VERSION_DIRTY_ADDR);
    raw_id[10] = csr_readl(CSR_VERSION_MODEL_ADDR);
    raw_id[11] = csr_readl(CSR_VERSION_SEED_ADDR + 0);
    raw_id[12] = csr_readl(CSR_VERSION_SEED_ADDR + 4);
    raw_id[13] = csr_readl(CSR_VERSION_SEED_ADDR + 8);
    raw_id[14] = csr_readl(CSR_VERSION_SEED_ADDR + 12);
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    return RESET_REASON_UNKNOWN;
}
