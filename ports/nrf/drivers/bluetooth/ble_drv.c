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

#if BLUETOOTH_SD

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#if (BLUETOOTH_SD == 132)
#define NRF52 // Needed for SD132 v2
#endif

#include "shared-bindings/bleio/ScanEntry.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"
#include "shared-module/bleio/Device.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"
#include "ble_drv.h"
#include "mpconfigport.h"
#include "nrf_sdm.h"
#include "nrfx_power.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "ble.h" // sd_ble_uuid_encode

#define BLE_DRIVER_LOG printf

#define BLE_ADV_LENGTH_FIELD_SIZE   1
#define BLE_ADV_AD_TYPE_FIELD_SIZE  1
#define BLE_AD_TYPE_FLAGS_DATA_SIZE 1

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))
#define UNIT_0_625_MS (625)
#define UNIT_10_MS    (10000)
#define APP_CFG_NON_CONN_ADV_TIMEOUT 0 // Disable timeout.
#define NON_CONNECTABLE_ADV_INTERVAL MSEC_TO_UNITS(100, UNIT_0_625_MS)

#define BLE_MIN_CONN_INTERVAL        MSEC_TO_UNITS(15, UNIT_0_625_MS)
#define BLE_MAX_CONN_INTERVAL        MSEC_TO_UNITS(300, UNIT_0_625_MS)
#define BLE_SLAVE_LATENCY            0
#define BLE_CONN_SUP_TIMEOUT         MSEC_TO_UNITS(4000, UNIT_10_MS)

#ifndef BLE_GAP_ADV_MAX_SIZE
#define BLE_GAP_ADV_MAX_SIZE            31
#endif

#define SD_TEST_OR_ENABLE() \
if (ble_drv_stack_enabled() == 0) { \
    (void)ble_drv_stack_enable(); \
}

static volatile bool m_adv_in_progress;
static volatile bool m_tx_in_progress;

static ble_drv_gap_evt_callback_t          gap_event_handler;
static ble_drv_gatts_evt_callback_t        gatts_event_handler;

static bleio_device_obj_t *mp_gap_observer;
static bleio_device_obj_t *mp_gatts_observer;

static volatile bool m_primary_service_found;
static volatile bool m_characteristic_found;
static volatile bool m_write_done;

static volatile ble_drv_adv_evt_callback_t          adv_event_handler;
static volatile ble_drv_gattc_evt_callback_t        gattc_event_handler;
static volatile ble_drv_disc_add_char_callback_t    disc_add_char_handler;
static volatile ble_drv_gattc_char_data_callback_t  gattc_char_data_handle;

static bleio_scanner_obj_t *mp_adv_observer;
static bleio_device_obj_t *mp_gattc_observer;
static bleio_device_obj_t *mp_gattc_disc_service_observer;
static bleio_service_obj_t *mp_gattc_disc_char_observer;
static bleio_characteristic_obj_t *mp_gattc_char_data_observer;
static bleio_address_obj_t *mp_connect_address;

#if (BLUETOOTH_SD == 140)
static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
static uint8_t m_scan_buffer_data[BLE_GAP_SCAN_BUFFER_MIN];

static ble_data_t m_scan_buffer =
{
    m_scan_buffer_data,
    BLE_GAP_SCAN_BUFFER_MIN
};
#endif

#include "nrf_nvic.h"

nrf_nvic_state_t nrf_nvic_state = {0};

void softdevice_assert_handler(uint32_t id, uint32_t pc, uint32_t info) {
    BLE_DRIVER_LOG("ERROR: SoftDevice assert!!!");
}

uint32_t ble_drv_stack_enable(void) {
    m_adv_in_progress = false;
    m_tx_in_progress  = false;

    nrf_clock_lf_cfg_t clock_config = {
        .source = NRF_CLOCK_LF_SRC_XTAL,
        .rc_ctiv = 0,
        .rc_temp_ctiv = 0,
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
        BLE_DRIVER_LOG("SoftDevice enable status: " UINT_FMT "\n", (uint16_t)err_code);

    err_code = sd_nvic_EnableIRQ(SD_EVT_IRQn);
    if (err_code != NRF_SUCCESS)
        BLE_DRIVER_LOG("IRQ enable status: " UINT_FMT "\n", (uint16_t)err_code);

    // Enable BLE stack.
    uint32_t app_ram_start;
#if (BLE_API_VERSION == 2)
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0x00, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.attr_tab_size = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
    ble_enable_params.gatts_enable_params.service_changed  = 0;
    ble_enable_params.gap_enable_params.periph_conn_count  = 1;
    ble_enable_params.gap_enable_params.central_conn_count = 1;

    app_ram_start = 0x200039c0;
    err_code = sd_ble_enable(&ble_enable_params, &app_ram_start); // 8K SD headroom from linker script.
#else
    app_ram_start = 0x20004000;
    err_code = sd_ble_enable(&app_ram_start);
#endif

    if (err_code != NRF_SUCCESS) {
        BLE_DRIVER_LOG("BLE ram size: " UINT_FMT "\n", (uint16_t)app_ram_start);
        BLE_DRIVER_LOG("BLE enable status: " UINT_FMT "\n", (uint16_t)err_code);
    }

    return err_code;
}

void ble_drv_stack_disable(void) {
    sd_softdevice_disable();
}

uint8_t ble_drv_stack_enabled(void) {
    uint8_t is_enabled;
    uint32_t err_code = sd_softdevice_is_enabled(&is_enabled);

    if (err_code != NRF_SUCCESS) {
        BLE_DRIVER_LOG("Is enabled status: " UINT_FMT "\n", (uint16_t)err_code);
    }

    return is_enabled;
}

void ble_drv_address_get(bleio_address_obj_t *address) {
    SD_TEST_OR_ENABLE();

    ble_gap_addr_t local_ble_addr;
#if (BLE_API_VERSION == 2)
    uint32_t err_code = sd_ble_gap_address_get(&local_ble_addr);
#else
    uint32_t err_code = sd_ble_gap_addr_get(&local_ble_addr);
#endif

    if (err_code != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not query for the device address.")));
    }

    BLE_DRIVER_LOG("ble address, type: " HEX2_FMT ", " \
                   "address: " HEX2_FMT ":" HEX2_FMT ":" HEX2_FMT ":" \
                               HEX2_FMT ":" HEX2_FMT ":" HEX2_FMT "\n", \
                   local_ble_addr.addr_type, \
                   local_ble_addr.addr[5], local_ble_addr.addr[4], local_ble_addr.addr[3], \
                   local_ble_addr.addr[2], local_ble_addr.addr[1], local_ble_addr.addr[0]);

    address->type = local_ble_addr.addr_type;
    memcpy(address->value, local_ble_addr.addr, BLEIO_ADDRESS_BYTES);
}

bool ble_drv_uuid_add_vs(uint8_t *uuid, uint8_t *idx) {
    SD_TEST_OR_ENABLE();

    if (sd_ble_uuid_vs_add((ble_uuid128_t const *)uuid, idx) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not add Vendor Specific 128-bit UUID.")));
    }

    return true;
}

void ble_drv_service_add(bleio_service_obj_t *service) {
    SD_TEST_OR_ENABLE();

    ble_uuid_t uuid = {
        .type = BLE_UUID_TYPE_BLE,
        .uuid = service->uuid->value[0] | (service->uuid->value[1] << 8)
    };

    if (service->uuid->type == UUID_TYPE_128BIT) {
        uuid.type = service->uuid->uuid_vs_idx;
    }

    uint8_t service_type = BLE_GATTS_SRVC_TYPE_PRIMARY;
    if (service->is_secondary) {
        service_type = BLE_GATTS_SRVC_TYPE_SECONDARY;
    }

    if (sd_ble_gatts_service_add(service_type, &uuid, &service->handle) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not add Service.")));
    }
}

bool ble_drv_characteristic_add(bleio_characteristic_obj_t *characteristic) {
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.broadcast      = characteristic->props.broadcast;
    char_md.char_props.read           = characteristic->props.read;
    char_md.char_props.write_wo_resp  = characteristic->props.write_wo_resp;
    char_md.char_props.write          = characteristic->props.write;
    char_md.char_props.notify         = characteristic->props.notify;
    char_md.char_props.indicate       = characteristic->props.indicate;

    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_sccd_md         = NULL;

    if (characteristic->props.notify || characteristic->props.notify) {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
        char_md.p_cccd_md = &cccd_md;
    } else {
        char_md.p_cccd_md = NULL;
    }

    uuid.type = BLE_UUID_TYPE_BLE;
    if (characteristic->uuid->type == UUID_TYPE_128BIT)
        uuid.type = characteristic->uuid->uuid_vs_idx;

    uuid.uuid  = characteristic->uuid->value[0];
    uuid.uuid += characteristic->uuid->value[1] << 8;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
#if (BLE_API_VERSION == 2)
    attr_char_value.max_len   = (GATT_MTU_SIZE_DEFAULT - 3);
#else
    attr_char_value.max_len   = (BLE_GATT_ATT_MTU_DEFAULT - 3);
#endif

    ble_gatts_char_handles_t handles;

    uint32_t err_code = sd_ble_gatts_characteristic_add(characteristic->service_handle,
                                                        &char_md,
                                                        &attr_char_value,
                                                        &handles);
    if (err_code != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not add Characteristic. status: 0x%08lX"), err_code));
    }

    // apply handles to object instance
    characteristic->handle           = handles.value_handle;
    characteristic->user_desc_handle = handles.user_desc_handle;
    characteristic->cccd_handle      = handles.cccd_handle;
    characteristic->sccd_handle      = handles.sccd_handle;

    return true;
}

// TODO: Replace with just bleio_device_obj_t + data
bool ble_drv_advertise_data(bleio_advertisement_data_t *adv_params) {
    SD_TEST_OR_ENABLE();

    uint8_t byte_pos = 0;
    uint8_t adv_data[BLE_GAP_ADV_MAX_SIZE];

    GET_STR_DATA_LEN(adv_params->device_name, name_data, name_len);

    if (name_len > 0) {
        ble_gap_conn_sec_mode_t sec_mode;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

        if (sd_ble_gap_device_name_set(&sec_mode,
                                       name_data,
                                       name_len) != 0) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
	              translate("Can not apply device name in the stack.")));
        }

        adv_data[byte_pos] = (BLE_ADV_AD_TYPE_FIELD_SIZE + name_len);
        byte_pos += BLE_ADV_LENGTH_FIELD_SIZE;

        // TODO: Shorten if too long
        adv_data[byte_pos] = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
        byte_pos += BLE_ADV_AD_TYPE_FIELD_SIZE;

        memcpy(&adv_data[byte_pos], name_data, name_len);

        byte_pos += name_len;
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(adv_params->data, &bufinfo, MP_BUFFER_WRITE);

    // set flags, default to disc mode
    if (bufinfo.len == 0) {
        adv_data[byte_pos] = (BLE_ADV_AD_TYPE_FIELD_SIZE + BLE_AD_TYPE_FLAGS_DATA_SIZE);
        byte_pos += BLE_ADV_LENGTH_FIELD_SIZE;

        adv_data[byte_pos] = BLE_GAP_AD_TYPE_FLAGS;
        byte_pos += BLE_AD_TYPE_FLAGS_DATA_SIZE;

        adv_data[byte_pos] = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
        byte_pos += 1;
    }

    const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(adv_params->services);
    if (service_list->len > 0) {
        bool type_16bit_present  = false;
        bool type_128bit_present = false;

        for (size_t i = 0; i < service_list->len; ++i) {
            const bleio_service_obj_t *service = MP_OBJ_TO_PTR(service_list->items[i]);

            if (service->uuid->type == UUID_TYPE_16BIT) {
                type_16bit_present = true;
            }

            if (service->uuid->type == UUID_TYPE_128BIT) {
                type_128bit_present = true;
            }
        }

        if (type_16bit_present) {
            uint8_t size_byte_pos = byte_pos;

            // skip length byte for now, apply total length post calculation
            byte_pos += BLE_ADV_LENGTH_FIELD_SIZE;

            adv_data[byte_pos] = BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE;
            byte_pos += BLE_ADV_AD_TYPE_FIELD_SIZE;

            uint8_t uuid_total_size = 0;
            uint8_t encoded_size    = 0;

            for (size_t i = 0; i < service_list->len; ++i) {
                const bleio_service_obj_t *service = MP_OBJ_TO_PTR(service_list->items[i]);

                if (service->uuid->type != UUID_TYPE_16BIT) {
                    continue;
                }

                ble_uuid_t uuid;
                uuid.type = BLE_UUID_TYPE_BLE;
                uuid.uuid = service->uuid->value[0] | (service->uuid->value[1] << 8);

                // calculate total size of uuids
                if (sd_ble_uuid_encode(&uuid, &encoded_size, NULL) != 0) {
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                              translate("Can not encode UUID, to check length.")));
                }

                // do encoding into the adv buffer
                if (sd_ble_uuid_encode(&uuid, &encoded_size, &adv_data[byte_pos]) != 0) {
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                              translate("Can encode UUID into the advertisement packet.")));
                }

                uuid_total_size += encoded_size; // size of entry
                byte_pos        += encoded_size; // relative to adv data packet
            }

            adv_data[size_byte_pos] = (BLE_ADV_AD_TYPE_FIELD_SIZE + uuid_total_size);
        }

        if (type_128bit_present) {
            uint8_t size_byte_pos = byte_pos;

            // skip length byte for now, apply total length post calculation
            byte_pos += BLE_ADV_LENGTH_FIELD_SIZE;

            adv_data[byte_pos] = BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE;
            byte_pos += BLE_ADV_AD_TYPE_FIELD_SIZE;

            uint8_t uuid_total_size = 0;
            uint8_t encoded_size    = 0;

            for (size_t i = 0; i < service_list->len; ++i) {
                const bleio_service_obj_t *service = MP_OBJ_TO_PTR(service_list->items[i]);

                if (service->uuid->type != UUID_TYPE_128BIT) {
                    continue;
                }

                ble_uuid_t uuid;
                uuid.type = service->uuid->uuid_vs_idx;
                uuid.uuid = service->uuid->value[0] | (service->uuid->value[1] << 8);

                // calculate total size of uuids
                if (sd_ble_uuid_encode(&uuid, &encoded_size, NULL) != 0) {
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                              translate("Can not encode UUID, to check length.")));
                }

                // do encoding into the adv buffer
                if (sd_ble_uuid_encode(&uuid, &encoded_size, &adv_data[byte_pos]) != 0) {
                    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                              translate("Can encode UUID into the advertisement packet.")));
                }

                uuid_total_size += encoded_size; // size of entry
                byte_pos        += encoded_size; // relative to adv data packet
            }

            adv_data[size_byte_pos] = (BLE_ADV_AD_TYPE_FIELD_SIZE + uuid_total_size);
        }
    }

    if (bufinfo.len > 0) {
        if (byte_pos + bufinfo.len > BLE_GAP_ADV_MAX_SIZE) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                      translate("Can not fit data into the advertisement packet.")));
        }

        memcpy(adv_data, bufinfo.buf, bufinfo.len);
        byte_pos += bufinfo.len;
    }

    uint32_t err_code;
#if (BLUETOOTH_SD == 132)
    if ((err_code = sd_ble_gap_adv_data_set(adv_data, byte_pos, NULL, 0)) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not apply advertisement data. status: 0x%02x"), (uint16_t)err_code));
    }
#endif

    static ble_gap_adv_params_t m_adv_params;

    // initialize advertising params
    memset(&m_adv_params, 0, sizeof(m_adv_params));
    if (adv_params->connectable) {
#if (BLUETOOTH_SD == 140)
        m_adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
#else
        m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
#endif
    } else {
#if (BLUETOOTH_SD == 140)
        m_adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
#else
        m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
#endif
    }

    m_adv_params.p_peer_addr = NULL;                                // undirected advertisement
    m_adv_params.interval    = MSEC_TO_UNITS(100, UNIT_0_625_MS);   // approx 8 ms
#if (BLUETOOTH_SD == 140)
    m_adv_params.duration              = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED;
    m_adv_params.filter_policy         = BLE_GAP_ADV_FP_ANY;
    m_adv_params.primary_phy           = BLE_GAP_PHY_1MBPS;
#else
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.timeout     = 0;                                   // infinite advertisement
#endif

    ble_drv_advertise_stop();

#if (BLUETOOTH_SD == 140)
    const ble_gap_adv_data_t ble_gap_adv_data = {
        .adv_data = {
            .p_data = adv_data,
            .len = byte_pos
        }
    };

    if ((err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &ble_gap_adv_data, &m_adv_params)) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not apply advertisement data. status: 0x%02x"), (uint16_t)err_code));
    }
    err_code = sd_ble_gap_adv_start(m_adv_handle, BLE_CONN_CFG_TAG_DEFAULT);
#elif (BLUETOOTH_SD == 132 && BLE_API_VERSION == 4)
    err_code = sd_ble_gap_adv_start(&m_adv_params, BLE_CONN_CFG_TAG_DEFAULT);
#else
    err_code = sd_ble_gap_adv_start(&m_adv_params);
#endif
    if (err_code != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not start advertisement. status: 0x%02x"), (uint16_t)err_code));
    }

    m_adv_in_progress = true;

    return true;
}

void ble_drv_advertise_stop(void) {
    if (m_adv_in_progress == true) {
        uint32_t err_code;
#if (BLUETOOTH_SD == 140)
        if ((err_code = sd_ble_gap_adv_stop(m_adv_handle)) != 0) {
#else
        if ((err_code = sd_ble_gap_adv_stop()) != 0) {
#endif
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                      translate("Can not stop advertisement. status: 0x%02x"), (uint16_t)err_code));
        }
    }
    m_adv_in_progress = false;
}

void ble_drv_attr_s_read(uint16_t conn_handle, uint16_t handle, uint16_t len, uint8_t * p_data) {
    ble_gatts_value_t gatts_value;
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = len;
    gatts_value.offset  = 0;
    gatts_value.p_value = p_data;

    uint32_t err_code = sd_ble_gatts_value_get(conn_handle,
                                               handle,
                                               &gatts_value);
    if (err_code != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not read attribute value. status: 0x%02x"), (uint16_t)err_code));
    }

}

void ble_drv_attr_s_write(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo) {
    bleio_device_obj_t *device = MP_OBJ_TO_PTR(characteristic->service->device);
    uint16_t conn_handle = device->conn_handle;
    ble_gatts_value_t gatts_value;

    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = bufinfo->len;
    gatts_value.offset  = 0;
    gatts_value.p_value = bufinfo->buf;

    uint32_t err_code = sd_ble_gatts_value_set(conn_handle, characteristic->handle, &gatts_value);

    if (err_code != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not write attribute value. status: 0x%02x"), (uint16_t)err_code));
    }
}

void ble_drv_attr_s_notify(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo) {
    bleio_device_obj_t *device = MP_OBJ_TO_PTR(characteristic->service->device);
    uint16_t conn_handle = device->conn_handle;
    ble_gatts_hvx_params_t hvx_params;
    uint16_t hvx_len = bufinfo->len;

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = characteristic->handle;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset = 0;
    hvx_params.p_len  = &hvx_len;
    hvx_params.p_data = bufinfo->buf;

    while (m_tx_in_progress) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }

    m_tx_in_progress = true;
    uint32_t err_code;
    if ((err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params)) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not notify attribute value. status: 0x%02x"), (uint16_t)err_code));
    }
}

void ble_drv_gap_event_handler_set(bleio_device_obj_t *device, ble_drv_gap_evt_callback_t evt_handler) {
    mp_gap_observer = device;
    gap_event_handler = evt_handler;
}

void ble_drv_gatts_event_handler_set(bleio_device_obj_t *device, ble_drv_gatts_evt_callback_t evt_handler) {
    mp_gatts_observer = device;
    gatts_event_handler = evt_handler;
}

void ble_drv_gattc_event_handler_set(bleio_device_obj_t *device, ble_drv_gattc_evt_callback_t evt_handler) {
    mp_gattc_observer = device;
    gattc_event_handler = evt_handler;
}

void ble_drv_adv_report_handler_set(bleio_scanner_obj_t *device, ble_drv_adv_evt_callback_t evt_handler) {
    mp_adv_observer = device;
    adv_event_handler = evt_handler;
}

void ble_drv_attr_c_read(bleio_characteristic_obj_t *characteristic, ble_drv_gattc_char_data_callback_t cb) {
    bleio_service_obj_t *service = characteristic->service;
    bleio_device_obj_t *device = MP_OBJ_TO_PTR(service->device);

    mp_gattc_char_data_observer = characteristic;
    gattc_char_data_handle = cb;

    const uint32_t err_code = sd_ble_gattc_read(device->conn_handle, characteristic->handle, 0);
    if (err_code != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not read attribute value. status: 0x%02x"), (uint16_t)err_code));
    }

    while (gattc_char_data_handle != NULL) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }
}

void ble_drv_attr_c_write(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo) {
    bleio_device_obj_t *device = MP_OBJ_TO_PTR(characteristic->service->device);
    uint16_t conn_handle = device->conn_handle;

    ble_gattc_write_params_t write_params;
    write_params.write_op = BLE_GATT_OP_WRITE_REQ;

    if (characteristic->props.write_wo_resp) {
        write_params.write_op = BLE_GATT_OP_WRITE_CMD;
    }

    write_params.flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_CANCEL;
    write_params.handle   = characteristic->handle;
    write_params.offset   = 0;
    write_params.len      = bufinfo->len;
    write_params.p_value  = bufinfo->buf;

    m_write_done = (write_params.write_op == BLE_GATT_OP_WRITE_CMD);

    uint32_t err_code = sd_ble_gattc_write(conn_handle, &write_params);
    if (err_code != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            translate("Can not write attribute value. status: 0x%02x"), (uint16_t)err_code));
    }

    while (m_write_done != true) {   
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }
}

void ble_drv_scan_start(uint16_t interval, uint16_t window) {
    SD_TEST_OR_ENABLE();

    ble_gap_scan_params_t scan_params;
    memset(&scan_params, 0, sizeof(ble_gap_scan_params_t));

    scan_params.active   = 1;
    scan_params.interval = MSEC_TO_UNITS(interval, UNIT_0_625_MS);
    scan_params.window   = MSEC_TO_UNITS(window, UNIT_0_625_MS);
#if (BLUETOOTH_SD == 140)
    scan_params.scan_phys          = BLE_GAP_PHY_1MBPS;
#endif
    scan_params.timeout  = 0; // Infinite

    uint32_t err_code;
#if (BLUETOOTH_SD == 140)
    if ((err_code = sd_ble_gap_scan_start(&scan_params, &m_scan_buffer)) != 0) {
#else
    if ((err_code = sd_ble_gap_scan_start(&scan_params)) != 0) {
#endif
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  translate("Can not start scanning. status: 0x%02x"), (uint16_t)err_code));
    }
}

void ble_drv_scan_continue(void) {
    SD_TEST_OR_ENABLE();

#if (BLUETOOTH_SD == 140)
    uint32_t err_code;
    if ((err_code = sd_ble_gap_scan_start(NULL, &m_scan_buffer)) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Can not continue scanning. status: 0x" HEX2_FMT, (uint16_t)err_code));
    }
#endif
}

void ble_drv_scan_stop(void) {
    sd_ble_gap_scan_stop();
}

STATIC void ble_drv_connect_scan_callback(bleio_scanner_obj_t *scanner, bleio_scanentry_obj_t *entry) {
    if (memcmp(entry->address.value, mp_connect_address->value, BLEIO_ADDRESS_BYTES) == 0) {
        ble_drv_adv_report_handler_set(NULL, NULL);

        ble_gap_scan_params_t scan_params;
        memset(&scan_params, 0, sizeof(scan_params));

        scan_params.active = 1;
        scan_params.interval = MSEC_TO_UNITS(100, UNIT_0_625_MS);
        scan_params.window = MSEC_TO_UNITS(100, UNIT_0_625_MS);
        scan_params.timeout = 0;

        ble_gap_addr_t addr;
        memset(&addr, 0, sizeof(addr));

        addr.addr_type = entry->address.type;
        memcpy(addr.addr, entry->address.value, BLEIO_ADDRESS_BYTES);

        BLE_DRIVER_LOG("GAP CONNECTING: "HEX2_FMT":"HEX2_FMT":"HEX2_FMT":"HEX2_FMT":"HEX2_FMT":"HEX2_FMT", type: %d\n",
                       addr.addr[5], addr.addr[4], addr.addr[3], addr.addr[2], addr.addr[1], addr.addr[0], addr.addr_type);

        ble_gap_conn_params_t conn_params = {
            .min_conn_interval = BLE_MIN_CONN_INTERVAL,
            .max_conn_interval = BLE_MAX_CONN_INTERVAL,
            .conn_sup_timeout = BLE_CONN_SUP_TIMEOUT,
            .slave_latency = BLE_SLAVE_LATENCY,
        };

        uint32_t err_code;
    #if (BLE_API_VERSION == 2)
        if ((err_code = sd_ble_gap_connect(&addr, &scan_params, &conn_params)) != 0) {
    #else
        if ((err_code = sd_ble_gap_connect(&addr, &scan_params, &conn_params, BLE_CONN_CFG_TAG_DEFAULT)) != 0) {
    #endif
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                      "Can not connect. status: 0x" HEX2_FMT, (uint16_t)err_code));
        }
    }
}

void ble_drv_connect(bleio_device_obj_t *device) {
    SD_TEST_OR_ENABLE();

    mp_connect_address = &device->address;
    ble_drv_adv_report_handler_set(NULL, ble_drv_connect_scan_callback);

    ble_drv_scan_start(100, 100);
}

void ble_drv_disconnect(bleio_device_obj_t *device) {
    sd_ble_gap_disconnect(device->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}

bool ble_drv_discover_services(bleio_device_obj_t *device, uint16_t start_handle) {
    mp_gattc_disc_service_observer = device;

    m_primary_service_found = false;

    uint32_t err_code = sd_ble_gattc_primary_services_discover(device->conn_handle, start_handle, NULL);
    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Failed to discover serivices. status: 0x" HEX2_FMT, (uint16_t)err_code));
    }

    while (mp_gattc_disc_service_observer != NULL) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }

    return m_primary_service_found;
}

bool ble_drv_discover_characteristic(bleio_device_obj_t *device, bleio_service_obj_t *service, uint16_t start_handle, ble_drv_disc_add_char_callback_t cb) {
    BLE_DRIVER_LOG("Discover characteristicts. Conn handle: 0x" HEX2_FMT "\n", device->conn_handle);

    mp_gattc_disc_char_observer = service;
    disc_add_char_handler = cb;

    ble_gattc_handle_range_t handle_range;
    handle_range.start_handle = start_handle;
    handle_range.end_handle = service->end_handle;

    m_characteristic_found = false;

    uint32_t err_code = sd_ble_gattc_characteristics_discover(device->conn_handle, &handle_range);
    if (err_code != 0) {
        return false;
    }

    while (disc_add_char_handler != NULL) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }

    return m_characteristic_found;
}

void ble_drv_discover_descriptors(void) {

}

STATIC void on_adv_report(ble_gap_evt_adv_report_t *report) {
    bleio_scanentry_obj_t *entry = m_new_obj(bleio_scanentry_obj_t);
    entry->base.type = &bleio_scanentry_type;

    entry->rssi = report->rssi;

    entry->address.type = report->peer_addr.addr_type;
    memcpy(entry->address.value, report->peer_addr.addr, BLEIO_ADDRESS_BYTES);

#if (BLUETOOTH_SD == 140)
    entry->data = mp_obj_new_bytearray(report->data.len, report->data.p_data);
#else
    entry->data = mp_obj_new_bytearray(report->dlen, report->data);
#endif

    if (adv_event_handler != NULL) {
        adv_event_handler(mp_adv_observer, entry);
    }
}

STATIC void on_primary_srv_discovery_rsp(ble_gattc_evt_prim_srvc_disc_rsp_t *response) {
    BLE_DRIVER_LOG(">>> service count: %d\n", response->count);

    for (size_t i = 0; i < response->count; ++i) {
        const ble_gattc_service_t *gattc_service = &response->services[i];

        bleio_service_obj_t *service = m_new_obj(bleio_service_obj_t);
        service->base.type = &bleio_service_type;
        service->char_list = mp_obj_new_list(0, NULL);
        service->start_handle = gattc_service->handle_range.start_handle;
        service->end_handle = gattc_service->handle_range.end_handle;
        service->handle = gattc_service->handle_range.start_handle;
        service->device = mp_gattc_disc_service_observer;

        bleio_uuid_obj_t *uuid = m_new_obj(bleio_uuid_obj_t);
        uuid->base.type = &bleio_uuid_type;
        uuid->type = (gattc_service->uuid.type == BLE_UUID_TYPE_BLE) ? UUID_TYPE_16BIT : UUID_TYPE_128BIT;
        uuid->value[0] = gattc_service->uuid.uuid & 0xFF;
        uuid->value[1] = gattc_service->uuid.uuid >> 8;
        service->uuid = uuid;

        mp_obj_list_append(mp_gattc_disc_service_observer->service_list, service);
    }

    if (response->count > 0) {
        m_primary_service_found = true;
    }

    // mark end of service discovery
    mp_gattc_disc_service_observer = NULL;
}

STATIC void ble_evt_handler(ble_evt_t *p_ble_evt) {
    printf("%s - 0x%02X\r\n", __func__, p_ble_evt->header.evt_id);

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            BLE_DRIVER_LOG("GAP CONNECT\n");
            m_adv_in_progress = false;
            gap_event_handler(mp_gap_observer, p_ble_evt->header.evt_id, p_ble_evt->evt.gap_evt.conn_handle, p_ble_evt->header.evt_len - (2 * sizeof(uint16_t)), NULL);

            ble_gap_conn_params_t conn_params;
            (void)sd_ble_gap_ppcp_get(&conn_params);
            (void)sd_ble_gap_conn_param_update(p_ble_evt->evt.gap_evt.conn_handle, &conn_params);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            BLE_DRIVER_LOG("GAP DISCONNECT\n");
            gap_event_handler(mp_gap_observer, p_ble_evt->header.evt_id, p_ble_evt->evt.gap_evt.conn_handle, p_ble_evt->header.evt_len - (2 * sizeof(uint16_t)), NULL);
            break;

        case BLE_GATTS_EVT_HVC:
            gatts_event_handler(mp_gatts_observer, p_ble_evt->header.evt_id, p_ble_evt->evt.gatts_evt.params.hvc.handle, p_ble_evt->header.evt_len - (2 * sizeof(uint16_t)), NULL);
            break;

        case BLE_GATTS_EVT_WRITE:
            BLE_DRIVER_LOG("GATTS write\n");

            uint16_t  handle   = p_ble_evt->evt.gatts_evt.params.write.handle;
            uint16_t  data_len = p_ble_evt->evt.gatts_evt.params.write.len;
            uint8_t * p_data   = &p_ble_evt->evt.gatts_evt.params.write.data[0];

            gatts_event_handler(mp_gatts_observer, p_ble_evt->header.evt_id, handle, data_len, p_data);
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            BLE_DRIVER_LOG("GAP CONN PARAM UPDATE\n");
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            (void)sd_ble_gatts_sys_attr_set(p_ble_evt->evt.gatts_evt.conn_handle, NULL, 0, 0);
            break;

#if (BLE_API_VERSION == 4)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            BLE_DRIVER_LOG("GATTS EVT EXCHANGE MTU REQUEST\n");
            (void)sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle, 23); // MAX MTU size
            break;
#endif

#if (BLE_API_VERSION == 4)
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
#else
        case BLE_EVT_TX_COMPLETE:
#endif
            BLE_DRIVER_LOG("BLE EVT TX COMPLETE\n");
            m_tx_in_progress = false;
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            BLE_DRIVER_LOG("BLE EVT SEC PARAMS REQUEST\n");
            // pairing not supported
            (void)sd_ble_gap_sec_params_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                              BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                              NULL, NULL);
            break;

        case BLE_GAP_EVT_ADV_REPORT:
            on_adv_report(&p_ble_evt->evt.gap_evt.params.adv_report);
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
            BLE_DRIVER_LOG("BLE EVT CONN PARAM UPDATE REQUEST\n");

            (void)sd_ble_gap_conn_param_update(p_ble_evt->evt.gap_evt.conn_handle,
                                               &p_ble_evt->evt.gap_evt.params.conn_param_update_request.conn_params);
            break;

        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
            on_primary_srv_discovery_rsp(&p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp);
            break;

        case BLE_GATTC_EVT_CHAR_DISC_RSP:
            BLE_DRIVER_LOG("BLE EVT CHAR DISCOVERY RESPONSE\n");
            BLE_DRIVER_LOG(">>> characteristic count: %d\n", p_ble_evt->evt.gattc_evt.params.char_disc_rsp.count);

            for (uint16_t i = 0; i < p_ble_evt->evt.gattc_evt.params.char_disc_rsp.count; i++) {
                ble_gattc_char_t * p_char = &p_ble_evt->evt.gattc_evt.params.char_disc_rsp.chars[i];

                ble_drv_char_data_t char_data;
                char_data.uuid_type    = p_char->uuid.type;
                char_data.uuid         = p_char->uuid.uuid;
                char_data.decl_handle  = p_char->handle_decl;
                char_data.value_handle = p_char->handle_value;

                char_data.props.broadcast = p_char->char_props.broadcast;
                char_data.props.read = p_char->char_props.read;
                char_data.props.write_wo_resp = p_char->char_props.write_wo_resp;
                char_data.props.write = p_char->char_props.write;
                char_data.props.notify = p_char->char_props.notify;
                char_data.props.indicate = p_char->char_props.indicate;

                disc_add_char_handler(mp_gattc_disc_char_observer, &char_data);
            }

            if (p_ble_evt->evt.gattc_evt.params.char_disc_rsp.count > 0) {
                m_characteristic_found = true;
            }

            // mark end of characteristic discovery
            disc_add_char_handler = NULL;

            break;

        case BLE_GATTC_EVT_READ_RSP:
            BLE_DRIVER_LOG("BLE EVT READ RESPONSE, offset: 0x"HEX2_FMT", length: 0x"HEX2_FMT"\n",
                           p_ble_evt->evt.gattc_evt.params.read_rsp.offset,
                           p_ble_evt->evt.gattc_evt.params.read_rsp.len);

            gattc_char_data_handle(mp_gattc_char_data_observer,
                                   p_ble_evt->evt.gattc_evt.params.read_rsp.len,
                                   p_ble_evt->evt.gattc_evt.params.read_rsp.data);

            // mark end of read
            gattc_char_data_handle = NULL;

            break;

        case BLE_GATTC_EVT_WRITE_RSP:
            BLE_DRIVER_LOG("BLE EVT WRITE RESPONSE\n");
            m_write_done = true;
            break;

        case BLE_GATTC_EVT_HVX:
            BLE_DRIVER_LOG("BLE EVT HVX RESPONSE\n");
            break;

        default:
            BLE_DRIVER_LOG(">>> unhandled evt: 0x" HEX2_FMT "\n", p_ble_evt->header.evt_id);
            break;
    }
}

#if (BLE_API_VERSION == 2)
static uint8_t m_ble_evt_buf[sizeof(ble_evt_t) + (GATT_MTU_SIZE_DEFAULT)] __attribute__ ((aligned (4)));
#else
static uint8_t m_ble_evt_buf[sizeof(ble_evt_t) + (BLE_GATT_ATT_MTU_DEFAULT)] __attribute__ ((aligned (4)));
#endif

void SD_EVT_IRQHandler(void) {
    uint32_t evt_id;
    uint32_t err_code;
    do {
        err_code = sd_evt_get(&evt_id);
        // TODO: handle non ble events
    } while (err_code != NRF_ERROR_NOT_FOUND && err_code != NRF_SUCCESS);

    uint16_t evt_len = sizeof(m_ble_evt_buf);
    do {
        err_code = sd_ble_evt_get(m_ble_evt_buf, &evt_len);
        ble_evt_handler((ble_evt_t *)m_ble_evt_buf);
    } while (err_code != NRF_ERROR_NOT_FOUND && err_code != NRF_SUCCESS);
}

#endif // BLUETOOTH_SD
