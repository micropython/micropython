/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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

#include <stdio.h>
#include <string.h>

#include "ble.h"
#include "ble_drv.h"
#include "nrfx_power.h"
#include "nrf_nvic.h"
#include "nrf_sdm.h"
#include "py/nlr.h"
#include "shared-bindings/bleio/Adapter.h"

STATIC void softdevice_assert_handler(uint32_t id, uint32_t pc, uint32_t info) {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_AssertionError,
         "Soft device assert, id: 0x%08lX, pc: 0x%08lX", id, pc));
}

STATIC uint32_t ble_stack_enable(void) {
    nrf_clock_lf_cfg_t clock_config = {
        .source = NRF_CLOCK_LF_SRC_XTAL,
#if (BLE_API_VERSION == 4)
        .accuracy = NRF_CLOCK_LF_ACCURACY_20_PPM
#else
        .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM
#endif
    };

#if (BLUETOOTH_SD == 140)
    // The SD takes over the POWER IRQ and will fail if the IRQ is already in use
    nrfx_power_uninit();
#endif

    uint32_t err_code = sd_softdevice_enable(&clock_config, softdevice_assert_handler);
    if (err_code != NRF_SUCCESS)
        return err_code;

    err_code = sd_nvic_EnableIRQ(SD_EVT_IRQn);
    if (err_code != NRF_SUCCESS)
        return err_code;

    uint32_t app_ram_start;
#if (BLE_API_VERSION == 2)
    ble_enable_params_t ble_enable_params = {
        .gatts_enable_params.attr_tab_size = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT,
        .gap_enable_params.central_conn_count = 1,
        .gap_enable_params.periph_conn_count = 1,
    };

    app_ram_start = 0x200039c0;
    err_code = sd_ble_enable(&ble_enable_params, &app_ram_start);
#else
    app_ram_start = 0x20004000;

    ble_cfg_t ble_conf;
    ble_conf.conn_cfg.conn_cfg_tag = BLE_CONN_CFG_TAG_CUSTOM;
    ble_conf.conn_cfg.params.gap_conn_cfg.conn_count = BLE_GAP_CONN_COUNT_DEFAULT;
    ble_conf.conn_cfg.params.gap_conn_cfg.event_length = BLE_GAP_EVENT_LENGTH_DEFAULT;
    err_code = sd_ble_cfg_set(BLE_CONN_CFG_GAP, &ble_conf, app_ram_start);
    if (err_code != NRF_SUCCESS)
        return err_code;

    memset(&ble_conf, 0, sizeof(ble_conf));
    ble_conf.gap_cfg.role_count_cfg.periph_role_count = 1;
    ble_conf.gap_cfg.role_count_cfg.central_role_count = 1;
    err_code = sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &ble_conf, app_ram_start);
    if (err_code != NRF_SUCCESS)
        return err_code;

    memset(&ble_conf, 0, sizeof(ble_conf));
    ble_conf.conn_cfg.conn_cfg_tag = BLE_CONN_CFG_TAG_CUSTOM;
    ble_conf.conn_cfg.params.gatts_conn_cfg.hvn_tx_queue_size = MAX_TX_IN_PROGRESS;
    err_code = sd_ble_cfg_set(BLE_CONN_CFG_GATTS, &ble_conf, app_ram_start);
    if (err_code != NRF_SUCCESS)
        return err_code;

    err_code = sd_ble_enable(&app_ram_start);
#endif

    return err_code;
}

void common_hal_bleio_adapter_set_enabled(bool enabled) {
    const bool is_enabled = common_hal_bleio_adapter_get_enabled();

    // Don't enable or disable twice
    if ((is_enabled && enabled) || (!is_enabled && !enabled)) {
        return;
    }

    uint32_t err_code;
    if (enabled) {
        err_code = ble_stack_enable();
    } else {
        err_code = sd_softdevice_disable();
    }

    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
             "Failed to change softdevice state, error: 0x%08lX", err_code));
    }
}

bool common_hal_bleio_adapter_get_enabled(void) {
    uint8_t is_enabled;

    const uint32_t err_code = sd_softdevice_is_enabled(&is_enabled);
    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "Failed to get softdevice state, error: 0x%08lX", err_code));
    }

    return is_enabled;
}

void common_hal_bleio_adapter_get_address(bleio_address_obj_t *address) {
    ble_gap_addr_t local_address;
    uint32_t err_code;

    common_hal_bleio_adapter_set_enabled(true);

#if (BLE_API_VERSION == 2)
    err_code = sd_ble_gap_address_get(&local_address);
#else
    err_code = sd_ble_gap_addr_get(&local_address);
#endif

    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "Failed to get local address, error: 0x%08lX", err_code));
    }

    address->type = local_address.addr_type;
    memcpy(address->value, local_address.addr, BLEIO_ADDRESS_BYTES);
}
