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
static uint8_t bluetooth_adv_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
static uint8_t bluetooth_sr_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
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
    switch (err_code) {
    case 0:
        return 0; // no error
    case NRF_ERROR_INVALID_PARAM:
    case NRF_ERROR_INVALID_LENGTH:
        return MP_EINVAL;
    case NRF_ERROR_NO_MEM:
    #if NRF51
    case BLE_ERROR_NO_TX_BUFFERS:
    #else
    case NRF_ERROR_RESOURCES:
    #endif
        return MP_ENOMEM;
    case NRF_ERROR_INVALID_ADDR:
        return MP_EFAULT; // bad address
    case NRF_ERROR_NOT_FOUND:
        return MP_ENOENT;
    case NRF_ERROR_DATA_SIZE:
        return MP_E2BIG;
    case NRF_ERROR_FORBIDDEN:
        return MP_EACCES;
    default:
        return MP_EPERM; // catch-all
    }
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

int mp_bt_advertise_start(mp_bt_adv_type_t type, uint16_t interval, const uint8_t *adv_data, size_t adv_data_len, const uint8_t *sr_data, size_t sr_data_len) {
    mp_bt_advertise_stop(); // restart if already started

#if NRF51
    sd_ble_gap_adv_data_set(adv_data, adv_data_len, sr_data, sr_data_len);
    bluetooth_adv_type = type;
    bluetooth_adv_interval = interval;
    uint32_t err_code = mp_bt_advertise_start_internal();
    return mp_bt_errno(err_code);

#elif NRF52
    if (adv_data_len > sizeof(bluetooth_adv_data) || sr_data_len > sizeof(bluetooth_sr_data)) {
        return MP_EINVAL;
    }
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
        case BLE_GAP_EVT_CONNECTED:
            mp_bt_connected(p_ble_evt->evt.gap_evt.conn_handle);
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            mp_bt_disconnected(p_ble_evt->evt.gap_evt.conn_handle);
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
        case BLE_GATTS_EVT_WRITE:
            mp_bt_characteristic_on_write(p_ble_evt->evt.gatts_evt.params.write.handle, &p_ble_evt->evt.gatts_evt.params.write.data, p_ble_evt->evt.gatts_evt.params.write.len);
            break;
    }
}

int mp_bt_add_service(mp_bt_service_t *service, size_t num_characteristics, mp_bt_characteristic_t **characteristics) {
    uint32_t err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service->uuid, &service->handle);
    if (err_code != 0) {
        return mp_bt_errno(err_code);
    }

    // Add each characteristic.
    for (size_t i = 0; i < num_characteristics; i++) {
        mp_bt_characteristic_t *characteristic = characteristics[i];

        // Create characteristic metadata.
        ble_gatts_char_md_t char_md = {0};
        char_md.char_props.read = (characteristic->flags & MP_BLE_FLAG_READ) ? 1 : 0;
        char_md.char_props.write = (characteristic->flags & MP_BLE_FLAG_WRITE) ? 1 : 0;
        char_md.char_props.notify = (characteristic->flags & MP_BLE_FLAG_NOTIFY) ? 1 : 0;

        // Create attribute metadata.
        ble_gatts_attr_md_t attr_md = {0};
        attr_md.vlen = 1;
        attr_md.vloc = BLE_GATTS_VLOC_STACK;
        attr_md.rd_auth = 0;
        attr_md.wr_auth = 0;
        if (characteristic->flags & MP_BLE_FLAG_READ) {
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
        }
        if (characteristic->flags & MP_BLE_FLAG_WRITE) {
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
        }

        // Create characteristic value.
        ble_gatts_attr_t attr_char_value = {0};
        attr_char_value.p_uuid = &characteristic->uuid;
        attr_char_value.p_attr_md = &attr_md;
        attr_char_value.init_len = 0;
        attr_char_value.init_offs = 0;
        attr_char_value.max_len = MP_BT_MAX_ATTR_SIZE;
        attr_char_value.p_value = NULL;

        // Output handles.
        ble_gatts_char_handles_t handles;

        // BLE_GATT_HANDLE_INVALID: add to previously added service.
        uint32_t err_code = sd_ble_gatts_characteristic_add(BLE_GATT_HANDLE_INVALID, &char_md, &attr_char_value, &handles);
        if (err_code != 0) {
            return mp_bt_errno(err_code);
        }

        // Now that the characteristic has been added successfully to the
        // service, update the characteristic's service.
        // Note that the caller has already ensured that
        // characteristic->service is NULL.
        characteristic->service = service;
        characteristic->value_handle = handles.value_handle;
    }

    return 0;
}

int mp_bt_characteristic_value_set(mp_bt_characteristic_t *characteristic, const void *value, size_t value_len) {
    ble_gatts_value_t data = {0};
    data.len = value_len;
    data.offset = 0;
    data.p_value = (void*)value; // value is only read so we can discard const
    uint32_t err_code = sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, characteristic->value_handle, &data);
    return mp_bt_errno(err_code);
}

int mp_bt_characteristic_value_notify(mp_bt_characteristic_t *characteristic, uint16_t conn_handle, const void *value, size_t value_len) {
    uint16_t len = value_len;
    ble_gatts_hvx_params_t hvx_params = {0};
    hvx_params.handle = characteristic->value_handle;
    hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.p_len = &len;
    hvx_params.p_data = (void*)value;
    uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
    if (err_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING) {
        // may happen when not subscribed
        err_code = 0;
    }
    return mp_bt_errno(err_code);
}

int mp_bt_characteristic_value_get(mp_bt_characteristic_t *characteristic, void *value, size_t *value_len) {
    ble_gatts_value_t data = {0};
    data.len = *value_len;
    data.offset = 0;
    data.p_value = value;
    uint32_t err_code = sd_ble_gatts_value_get(BLE_CONN_HANDLE_INVALID, characteristic->value_handle, &data);
    *value_len = data.len;
    return mp_bt_errno(err_code);
}

// Parse a UUID object from the caller.
void mp_bt_parse_uuid(mp_obj_t obj, mp_bt_uuid_t *uuid) {
    if (MP_OBJ_IS_SMALL_INT(obj) && MP_OBJ_SMALL_INT_VALUE(obj) == (uint32_t)(uint16_t)MP_OBJ_SMALL_INT_VALUE(obj)) {
        // Integer fits inside 16 bits.
        uuid->type = BLE_UUID_TYPE_BLE;
        uuid->uuid = MP_OBJ_SMALL_INT_VALUE(obj);
    } else if (mp_obj_is_str(obj)) {
        // Guessing this is a 128-bit (proprietary) UUID.
        ble_uuid128_t buf;
        mp_bt_parse_uuid_str(obj, &buf.uuid128[0]);
        uint32_t err_code = sd_ble_uuid_vs_add(&buf, &uuid->type);
        if (err_code != 0) {
            mp_raise_OSError(mp_bt_errno(err_code));
        }
        uuid->uuid = (uint16_t)(buf.uuid128[12]) | ((uint16_t)(buf.uuid128[13]) << 8);
    } else {
        mp_raise_ValueError("cannot parse UUID");
    }
}

mp_obj_t mp_bt_format_uuid(mp_bt_uuid_t *uuid) {
    uint8_t raw[16];
    uint8_t raw_len;
    if (sd_ble_uuid_encode(uuid, &raw_len, raw) != 0) {
        return mp_const_none;
    }
    switch (raw_len) {
    case 2:
        return MP_OBJ_NEW_SMALL_INT((int)(raw[0]) | ((int)(raw[1]) << 8));
    case 16:
        return mp_bt_format_uuid_str(raw);
    default:
        return mp_const_none;
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
