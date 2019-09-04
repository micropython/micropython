/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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
#include "py/objstr.h"
#include "py/runtime.h"
#include "supervisor/usb.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Address.h"

#define BLE_MIN_CONN_INTERVAL        MSEC_TO_UNITS(15, UNIT_0_625_MS)
#define BLE_MAX_CONN_INTERVAL        MSEC_TO_UNITS(15, UNIT_0_625_MS)
#define BLE_SLAVE_LATENCY            0
#define BLE_CONN_SUP_TIMEOUT         MSEC_TO_UNITS(4000, UNIT_10_MS)

STATIC void softdevice_assert_handler(uint32_t id, uint32_t pc, uint32_t info) {
    mp_raise_msg_varg(&mp_type_AssertionError,
                      translate("Soft device assert, id: 0x%08lX, pc: 0x%08lX"), id, pc);
}

STATIC uint32_t ble_stack_enable(void) {
    nrf_clock_lf_cfg_t clock_config = {
#if BOARD_HAS_32KHZ_XTAL
        .source       = NRF_CLOCK_LF_SRC_XTAL,
        .rc_ctiv      = 0,
        .rc_temp_ctiv = 0,
        .accuracy     = NRF_CLOCK_LF_ACCURACY_20_PPM,
#else
        .source       = NRF_CLOCK_LF_SRC_RC,
        .rc_ctiv      = 16,
        .rc_temp_ctiv = 2,
        .accuracy     = NRF_CLOCK_LF_ACCURACY_250_PPM,
#endif
    };

    uint32_t err_code = sd_softdevice_enable(&clock_config, softdevice_assert_handler);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    err_code = sd_nvic_EnableIRQ(SD_EVT_IRQn);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    // Start with no event handlers, etc.
    ble_drv_reset();

    uint32_t app_ram_start;
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
    if (err_code != NRF_SUCCESS)
        return err_code;

    ble_gap_conn_params_t gap_conn_params = {
        .min_conn_interval = BLE_MIN_CONN_INTERVAL,
        .max_conn_interval = BLE_MAX_CONN_INTERVAL,
        .slave_latency     = BLE_SLAVE_LATENCY,
        .conn_sup_timeout  = BLE_CONN_SUP_TIMEOUT,
    };
   err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
   if (err_code != NRF_SUCCESS) {
       return err_code;
   }

   err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
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
        // The SD takes over the POWER module and will fail if the module is already in use.
        // Occurs when USB is initialized previously
        nrfx_power_uninit();

        err_code = ble_stack_enable();

        // Re-init USB hardware
        init_usb_hardware();
    } else {
        err_code = sd_softdevice_disable();

        // Re-init USB hardware
        init_usb_hardware();
    }

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to change softdevice state"));
    }
}

bool common_hal_bleio_adapter_get_enabled(void) {
    uint8_t is_enabled;

    const uint32_t err_code = sd_softdevice_is_enabled(&is_enabled);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to get softdevice state"));
    }

    return is_enabled;
}

void get_address(ble_gap_addr_t *address) {
    uint32_t err_code;

    common_hal_bleio_adapter_set_enabled(true);
    err_code = sd_ble_gap_addr_get(address);

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to get local address"));
    }
}

bleio_address_obj_t *common_hal_bleio_adapter_get_address(void) {
    common_hal_bleio_adapter_set_enabled(true);

    ble_gap_addr_t local_address;
    get_address(&local_address);

    bleio_address_obj_t *address = m_new_obj(bleio_address_obj_t);
    address->base.type = &bleio_address_type;

    common_hal_bleio_address_construct(address, local_address.addr, local_address.addr_type);
    return address;
}

mp_obj_t common_hal_bleio_adapter_get_default_name(void) {
   common_hal_bleio_adapter_set_enabled(true);

    ble_gap_addr_t local_address;
    get_address(&local_address);

    char name[] = { 'C', 'I', 'R', 'C', 'U', 'I', 'T', 'P', 'Y', 0, 0, 0, 0 };

    name[sizeof(name) - 4] = nibble_to_hex_lower[local_address.addr[1] >> 4 & 0xf];
    name[sizeof(name) - 3] = nibble_to_hex_lower[local_address.addr[1] & 0xf];
    name[sizeof(name) - 2] = nibble_to_hex_lower[local_address.addr[0] >> 4 & 0xf];
    name[sizeof(name) - 1] = nibble_to_hex_lower[local_address.addr[0] & 0xf];

    return mp_obj_new_str(name, sizeof(name));
}
