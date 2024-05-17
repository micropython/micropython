// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/Processor.h"

#include "common-hal/alarm/__init__.h"
#include "shared-bindings/microcontroller/ResetReason.h"

#include "nrfx_saadc.h"
#ifdef BLUETOOTH_SD
#include "nrf_sdm.h"
#endif

#include "nrf.h"

float common_hal_mcu_processor_get_temperature(void) {
    int32_t temp = 0;

    #ifdef BLUETOOTH_SD
    uint8_t sd_en = 0;

    (void)sd_softdevice_is_enabled(&sd_en);

    if (sd_en) {
        uint32_t err_code = sd_temp_get(&temp);
        if (err_code != NRF_SUCCESS) {
            mp_raise_OSError_msg(MP_ERROR_TEXT("Cannot get temperature"));
        }
        return temp / 4.0f;
    } // Fall through if SD not enabled.
    #endif
    NRF_TEMP->TASKS_START = 1;
    while (NRF_TEMP->EVENTS_DATARDY == 0) {
    }
    NRF_TEMP->EVENTS_DATARDY = 0;
    temp = NRF_TEMP->TEMP;
    NRF_TEMP->TASKS_STOP = 1;
    return temp / 4.0f;
}



uint32_t common_hal_mcu_processor_get_frequency(void) {
    return 64000000ul;
}

float common_hal_mcu_processor_get_voltage(void) {
    nrf_saadc_value_t value = -1;

    const nrf_saadc_channel_config_t config = {
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
        .gain = NRF_SAADC_GAIN1_6,
        .reference = NRF_SAADC_REFERENCE_INTERNAL,
        .acq_time = NRF_SAADC_ACQTIME_10US,
        .mode = NRF_SAADC_MODE_SINGLE_ENDED,
        .burst = NRF_SAADC_BURST_DISABLED
    };

    nrf_saadc_resolution_set(NRF_SAADC, NRF_SAADC_RESOLUTION_14BIT);
    nrf_saadc_oversample_set(NRF_SAADC, NRF_SAADC_OVERSAMPLE_DISABLED);
    nrf_saadc_enable(NRF_SAADC);

    for (uint32_t i = 0; i < SAADC_CH_NUM; i++) {
        nrf_saadc_channel_input_set(NRF_SAADC, i, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED);
    }

    nrf_saadc_channel_init(NRF_SAADC, 0, &config);
    nrf_saadc_channel_input_set(NRF_SAADC, 0, NRF_SAADC_INPUT_VDD, NRF_SAADC_INPUT_VDD);
    nrf_saadc_buffer_init(NRF_SAADC, &value, 1);

    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_START);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_STARTED) == 0) {
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_STARTED);

    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_SAMPLE);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_END) == 0) {
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_END);

    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_STOP);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_STOPPED) == 0) {
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_STOPPED);

    nrf_saadc_disable(NRF_SAADC);

    if (value < 0) {
        value = 0;
    }

// The ADC reading we expect if VDD is 3.3V.
#define NOMINAL_VALUE_3_3 (((3.3f / 6) / 0.6f) * 16383)
    return (value / NOMINAL_VALUE_3_3) * 3.3f;
}


void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    for (int i = 0; i < 2; i++) {
        ((uint32_t *)raw_id)[i] = NRF_FICR->DEVICEID[i];
    }
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    mcu_reset_reason_t r = RESET_REASON_UNKNOWN;
    if (reset_reason_saved == 0) {
        r = RESET_REASON_POWER_ON;
    } else if (reset_reason_saved & POWER_RESETREAS_RESETPIN_Msk) {
        r = RESET_REASON_RESET_PIN;
    } else if (reset_reason_saved & POWER_RESETREAS_DOG_Msk) {
        r = RESET_REASON_WATCHDOG;
    } else if (reset_reason_saved & POWER_RESETREAS_SREQ_Msk) {
        r = RESET_REASON_SOFTWARE;
        #if CIRCUITPY_ALARM
        // Our "deep sleep" is still actually light sleep followed by a software
        // reset. Adding this check here ensures we treat it as-if we're waking
        // from deep sleep.
        if (sleepmem_wakeup_event != SLEEPMEM_WAKEUP_BY_NONE) {
            r = RESET_REASON_DEEP_SLEEP_ALARM;
        }
        #endif
    } else if ((reset_reason_saved & POWER_RESETREAS_OFF_Msk) ||
               (reset_reason_saved & POWER_RESETREAS_LPCOMP_Msk) ||
               (reset_reason_saved & POWER_RESETREAS_NFC_Msk) ||
               (reset_reason_saved & POWER_RESETREAS_VBUS_Msk)) {
        r = RESET_REASON_DEEP_SLEEP_ALARM;
    }
    return r;
}
