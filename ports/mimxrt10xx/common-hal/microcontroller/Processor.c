// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include <math.h>

#include "py/runtime.h"

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

#if CIRCUITPY_ANALOGIO
#include "sdk/drivers/tempmon/fsl_tempmon.h"
#endif
#include "sdk/drivers/ocotp/fsl_ocotp.h"
#include "clocks.h"

float common_hal_mcu_processor_get_temperature(void) {
    #if CIRCUITPY_ANALOGIO
    tempmon_config_t config;
    TEMPMON_GetDefaultConfig(&config);

    OCOTP_Init(OCOTP, CLOCK_GetFreq(kCLOCK_IpgClk));
    TEMPMON_Init(TEMPMON, &config);
    TEMPMON_StartMeasure(TEMPMON);

    const float temp = TEMPMON_GetCurrentTemperature(TEMPMON);
    TEMPMON_Deinit(TEMPMON);
    OCOTP_Deinit(OCOTP);

    return temp;
    #else
    return NAN;
    #endif
}

void common_hal_mcu_processor_set_frequency(mcu_processor_obj_t *self,
    uint32_t frequency) {
    uint32_t freq = frequency / 1000000;
    if (freq != 24 && freq != 150 && freq != 396 && freq != 450 && freq != 528 && freq != 600 &&
        freq != 720 && freq != 816 && freq != 912 && freq != 960 && freq != 1008) {
        mp_raise_ValueError(MP_ERROR_TEXT("Frequency must be 24, 150, 396, 450, 528, 600, 720, 816, 912, 960 or 1008 Mhz"));
    }
    SystemCoreClock = setarmclock(frequency);
}


float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return SystemCoreClock;
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    #if IMXRT11XX
    OCOTP_Init(OCOTP, CLOCK_GetFreq(kCLOCK_Ocotp));
    #else
    OCOTP_Init(OCOTP, CLOCK_GetFreq(kCLOCK_IpgClk));
    #endif

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
