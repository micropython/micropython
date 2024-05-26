// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include "py/runtime.h"

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

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
