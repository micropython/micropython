/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 - 2018 Glenn Ruben Bakke
 * Copyright (c) 2018 Ayke van Laethem
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

#if MICROPY_PY_BLUETOOTH

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/modbluetooth.h"
#include "drivers/flash.h"

#include "nrf_sdm.h"
#include "ble.h"
#if !NRF51
#include "nrf_nvic.h"
#endif

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))
#define UNIT_0_625_MS (625)
#define UNIT_10_MS    (10000)

#define BLE_MIN_CONN_INTERVAL        MSEC_TO_UNITS(12, UNIT_0_625_MS)
#define BLE_MAX_CONN_INTERVAL        MSEC_TO_UNITS(12, UNIT_0_625_MS)
#define BLE_SLAVE_LATENCY            0
#define BLE_CONN_SUP_TIMEOUT         MSEC_TO_UNITS(4000, UNIT_10_MS)

#if NRF51
  #define MAX_TX_IN_PROGRESS         (6)
#else
  #define MAX_TX_IN_PROGRESS         (10)
#endif
#if !defined(GATT_MTU_SIZE_DEFAULT) && defined(BLE_GATT_ATT_MTU_DEFAULT)
  #define GATT_MTU_SIZE_DEFAULT BLE_GATT_ATT_MTU_DEFAULT
#endif

#if NRF51
STATIC mp_bt_adv_type_t bluetooth_adv_type;
STATIC uint16_t bluetooth_adv_interval;
#else
#include "nrf_nvic.h"
nrf_nvic_state_t nrf_nvic_state = {0};
static uint8_t bluetooth_adv_handle;
static uint8_t bluetooth_adv_data[31];
static uint8_t bluetooth_sr_data[31];
#endif

#if NRF51
void softdevice_assert_handler(uint32_t pc, uint16_t line_number, const uint8_t * p_file_name) {
    printf("ERROR: SoftDevice assert!!!\n");
}
#else
void softdevice_assert_handler(uint32_t id, uint32_t pc, uint32_t info) {
    printf("ERROR: SoftDevice assert!!!\n");
}
#endif

#if !NRF51
#if BLUETOOTH_LFCLK_RC
STATIC const nrf_clock_lf_cfg_t clock_config = {
    .source = NRF_CLOCK_LF_SRC_RC,
    .rc_ctiv = 16,
    .rc_temp_ctiv = 2,
    .accuracy = NRF_CLOCK_LF_ACCURACY_250_PPM
};
#else
STATIC const nrf_clock_lf_cfg_t clock_config = {
    .source = NRF_CLOCK_LF_SRC_XTAL,
    .rc_ctiv = 0,
    .rc_temp_ctiv = 0,
    .accuracy = NRF_CLOCK_LF_ACCURACY_20_PPM
};
#endif // BLUETOOTH_LFCLK_RC
#endif // !NRF51

STATIC const uint8_t ble_default_device_name[] = "MPY";

// Connection params for sd_ble_gap_ppcp_set.
STATIC const ble_gap_conn_params_t gap_conn_params = {
    .min_conn_interval = BLE_MIN_CONN_INTERVAL,
    .max_conn_interval = BLE_MAX_CONN_INTERVAL,
    .slave_latency     = BLE_SLAVE_LATENCY,
    .conn_sup_timeout  = BLE_CONN_SUP_TIMEOUT,
};

STATIC int mp_bt_errno(uint32_t err_code) {
    if (err_code == NRF_ERROR_INVALID_PARAM) {
        return MP_EINVAL;
    } else if (err_code != 0) {
        return MP_EPERM;
    }
    return 0;
}

int mp_bt_enable(void) {
    if (mp_bt_is_enabled()) {
        return 0;
    }

    // initialize our state
#if !NRF51
    bluetooth_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
#endif

#if NRF51
  #if BLUETOOTH_LFCLK_RC
    uint32_t err_code = sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_RC_250_PPM_250MS_CALIBRATION,
                                             softdevice_assert_handler);
  #else
    uint32_t err_code = sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM,
                                             softdevice_assert_handler);
  #endif // BLUETOOTH_LFCLK_RC

#else // NRF52xxx
    uint32_t err_code = sd_softdevice_enable(&clock_config,
                                             softdevice_assert_handler);
#endif

    if (err_code != 0) { // sd_softdevice_enable
        return mp_bt_errno(err_code);
    }

    sd_nvic_EnableIRQ(SD_EVT_IRQn);

#if NRF51
    ble_enable_params_t ble_enable_params = {0};
    ble_enable_params.gatts_enable_params.attr_tab_size = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
    ble_enable_params.gatts_enable_params.service_changed = 0;
    err_code = sd_ble_enable(&ble_enable_params);
#else
    uint32_t app_ram_start_cfg = 0x200039c0;
    err_code = sd_ble_enable(&app_ram_start_cfg); // 8K SD headroom from linker script.
#endif
    if (err_code != 0) { // sd_ble_enable
        return mp_bt_errno(err_code);
    }

    // set up security mode
    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    if ((err_code = sd_ble_gap_device_name_set(&sec_mode,
                    ble_default_device_name,
                    sizeof(ble_default_device_name) - 1)) != 0) {
        return mp_bt_errno(err_code);
    }

    if ((err_code = sd_ble_gap_ppcp_set(&gap_conn_params)) != 0) {
        return mp_bt_errno(err_code);
    }

    return 0; // success
}

void mp_bt_disable(void) {
    sd_softdevice_disable();
}

bool mp_bt_is_enabled(void) {
    uint8_t is_enabled;
    sd_softdevice_is_enabled(&is_enabled);
    return is_enabled != 0;
}

#if NRF51
STATIC uint32_t mp_bt_advertise_start_internal(void) {
    ble_gap_adv_params_t adv_params;
    adv_params.type = bluetooth_adv_type;
    adv_params.p_peer_addr = NULL;
    adv_params.fp = BLE_GAP_ADV_FP_ANY; // no filter policy
    adv_params.interval = bluetooth_adv_interval;
    return sd_ble_gap_adv_start(&adv_params);
}
#endif

int mp_bt_advertise_start(mp_bt_adv_type_t type, uint16_t interval, uint8_t *adv_data, size_t adv_data_len, uint8_t *sr_data, size_t sr_data_len) {
    mp_bt_advertise_stop(); // restart if already started

#if NRF51
    sd_ble_gap_adv_data_set(adv_data, adv_data_len, sr_data, sr_data_len);
    bluetooth_adv_type = type;
    bluetooth_adv_interval = interval;
    uint32_t err_code = mp_bt_advertise_start_internal();
    return mp_bt_errno(err_code);

#elif NRF52
    if (adv_data_len) {
        memcpy(bluetooth_adv_data, adv_data, adv_data_len);
    }
    if (sr_data_len) {
        memcpy(bluetooth_sr_data, sr_data, sr_data_len);
    }

    ble_gap_adv_data_t ble_adv_data = {0};
    ble_adv_data.adv_data.p_data = bluetooth_adv_data;
    ble_adv_data.adv_data.len = adv_data_len;
    ble_adv_data.scan_rsp_data.p_data = bluetooth_sr_data;
    ble_adv_data.scan_rsp_data.len = sr_data_len;

    ble_gap_adv_params_t adv_params = {0};
    adv_params.properties.type       = type;
    adv_params.filter_policy         = BLE_GAP_ADV_FP_ANY; // no filter policy
    adv_params.interval              = interval;
    adv_params.max_adv_evts          = 0; // infinite advertisment
    adv_params.primary_phy           = BLE_GAP_PHY_AUTO;
    adv_params.secondary_phy         = BLE_GAP_PHY_AUTO;
    adv_params.scan_req_notification = 0; // Do not raise scan request notifications when scanned.

    uint32_t err_code = sd_ble_gap_adv_set_configure(&bluetooth_adv_handle, &ble_adv_data, &adv_params);
    if (err_code != 0) {
        return mp_bt_errno(err_code);
    }

    err_code = sd_ble_gap_adv_start(bluetooth_adv_handle, BLE_CONN_CFG_TAG_DEFAULT);
    return mp_bt_errno(err_code);
#endif
}

void mp_bt_advertise_stop(void) {
#if NRF51
    sd_ble_gap_adv_stop();
#else
    sd_ble_gap_adv_stop(bluetooth_adv_handle);
#endif
}

static void ble_evt_handler(ble_evt_t * p_ble_evt) {
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_DISCONNECTED:
#if NRF51
            mp_bt_advertise_start_internal();
#else
            sd_ble_gap_adv_start(bluetooth_adv_handle, BLE_CONN_CFG_TAG_DEFAULT);
#endif
            break;

#if NRF52
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle, GATT_MTU_SIZE_DEFAULT);
            break;
#endif
    }
}

static void sd_evt_handler(uint32_t evt_id) {
    switch (evt_id) {
#if MICROPY_MBFS
        case NRF_EVT_FLASH_OPERATION_SUCCESS:
            flash_operation_finished(FLASH_STATE_SUCCESS);
            break;
        case NRF_EVT_FLASH_OPERATION_ERROR:
            flash_operation_finished(FLASH_STATE_ERROR);
            break;
#endif
        default:
            // unhandled event!
            break;
    }
}

static uint8_t m_ble_evt_buf[sizeof(ble_evt_t) + (GATT_MTU_SIZE_DEFAULT)] __attribute__ ((aligned (4)));

#ifdef NRF51
void SWI2_IRQHandler(void) {
#else
void SWI2_EGU2_IRQHandler(void) {
#endif

    uint32_t evt_id;
    while (sd_evt_get(&evt_id) != NRF_ERROR_NOT_FOUND) {
        sd_evt_handler(evt_id);
    }

    while (1) {
        uint16_t evt_len  = sizeof(m_ble_evt_buf);
        uint32_t err_code = sd_ble_evt_get(m_ble_evt_buf, &evt_len);
        if (err_code != NRF_SUCCESS) {
            // Possible error conditions:
            //  * NRF_ERROR_NOT_FOUND: no events left, break
            //  * NRF_ERROR_DATA_SIZE: retry with a bigger data buffer
            //    (currently not handled, TODO)
            //  * NRF_ERROR_INVALID_ADDR: pointer is not aligned, should
            //    not happen.
            // In all cases, it's best to simply stop now.
            break;
        }
        ble_evt_handler((ble_evt_t *)m_ble_evt_buf);
    }
}

#endif // MICROPY_PY_BLUETOOTH
