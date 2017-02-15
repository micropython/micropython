/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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
#include <stdbool.h>

#include "py/runtime.h"
#include "softdevice.h"
#include "mpconfigport.h"
#include "nrf_sdm.h"
#include "ble_gap.h"
#include "ble.h" // sd_ble_uuid_encode


#define SD_TEST_OR_ENABLE() \
if (sd_enabled() == 0) { \
    (void)sd_enable(); \
}

static bool m_adv_in_progress = false;

static ubluepy_evt_callback_t ubluepy_event_handler;
static mp_obj_t               mp_observer;

#if (BLUETOOTH_SD != 100) && (BLUETOOTH_SD != 110)
#include "nrf_nvic.h"

#if NRF51
nrf_nvic_state_t nrf_nvic_state;;
#else
nrf_nvic_state_t nrf_nvic_state;
#endif // NRF51

#endif // (BLUETOOTH_SD != 100)

#if (BLUETOOTH_SD == 100 ) || (BLUETOOTH_SD == 110)
void softdevice_assert_handler(uint32_t pc, uint16_t line_number, const uint8_t * p_file_name) {
    printf("ERROR: SoftDevice assert!!!");
}
#else
void softdevice_assert_handler(uint32_t id, uint32_t pc, uint32_t info) {
    printf("ERROR: SoftDevice assert!!!");
}
#endif
uint32_t sd_enable(void) {
#if (BLUETOOTH_SD != 100) && (BLUETOOTH_SD != 110)
    memset(&nrf_nvic_state, 0, sizeof(nrf_nvic_state_t));
#endif

#if (BLUETOOTH_SD == 100) || (BLUETOOTH_SD == 110)
    uint32_t err_code = sd_softdevice_enable(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM,
                                             softdevice_assert_handler);
#else
    nrf_clock_lf_cfg_t clock_config = {
        .source = NRF_CLOCK_LF_SRC_XTAL,
        .rc_ctiv = 0,
        .rc_temp_ctiv = 0,
        .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM
    };

    uint32_t err_code = sd_softdevice_enable(&clock_config,
                                             softdevice_assert_handler);
#endif

    printf("SoftDevice enable status: " UINT_FMT "\n", (uint16_t)err_code);

#if NRF51
    err_code = sd_nvic_EnableIRQ(SWI2_IRQn);
#else
    err_code = sd_nvic_EnableIRQ(SWI2_EGU2_IRQn);
#endif

    printf("IRQ enable status: " UINT_FMT "\n", (uint16_t)err_code);
    
    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0x00, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.attr_tab_size = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
    ble_enable_params.gatts_enable_params.service_changed  = 0;
    ble_enable_params.gap_enable_params.periph_conn_count  = 1;
    ble_enable_params.gap_enable_params.central_conn_count = 1;



#if (BLUETOOTH_SD == 100) || (BLUETOOTH_SD == 110)
    err_code = sd_ble_enable(&ble_enable_params);
#else

#if (BLUETOOTH_SD == 132)
    uint32_t app_ram_start = 0x200039c0;
    err_code = sd_ble_enable(&ble_enable_params, &app_ram_start); // 8K SD headroom from linker script.
    printf("BLE ram size: " UINT_FMT "\n", (uint16_t)app_ram_start);
#else
    err_code = sd_ble_enable(&ble_enable_params, (uint32_t *)0x20001870);
#endif

#endif

    printf("BLE enable status: " UINT_FMT "\n", (uint16_t)err_code);

    return err_code;
}

void sd_disable(void) {
    sd_softdevice_disable();
}

uint8_t sd_enabled(void) {
    uint8_t is_enabled;
    uint32_t err_code = sd_softdevice_is_enabled(&is_enabled);

#if BLUETOOTH_SD_DEBUG
    printf("Is enabled status: " UINT_FMT "\n", (uint16_t)err_code);
#endif

    return is_enabled;
}

void sd_address_get(void) {
    ble_gap_addr_t local_ble_addr;
#if (BLUETOOTH_SD != 132)
    uint32_t err_code = sd_ble_gap_address_get(&local_ble_addr);
#else
    uint32_t err_code = sd_ble_gap_addr_get(&local_ble_addr);
#endif
    printf("ble address, type: " HEX2_FMT ", " \
           "address: " HEX2_FMT ":" HEX2_FMT ":" HEX2_FMT ":" \
                       HEX2_FMT ":" HEX2_FMT ":" HEX2_FMT "\n", \
            local_ble_addr.addr_type, \
            local_ble_addr.addr[5], local_ble_addr.addr[4], local_ble_addr.addr[3], \
            local_ble_addr.addr[2], local_ble_addr.addr[1], local_ble_addr.addr[0]);

    (void)err_code;
}

#define EDDYSTONE_UUID 0xFEAA // UUID for Eddystone beacons, Big Endian.

// URL Frame Type, fixed at 0x10.
// RSSI, 0xEE = -18 dB is the approximate signal strength at 0 m.
// URL prefix, 0x00 = "http://www".
// URL
// URL suffix, 0x01 = ".com"
#define EDDYSTONE_DATA 0x10, 0xEE, 0x00, 'm', 'i', 'c', 'r', 'o', 'p', 'y', 't', 'h', 'o', 'n', 0x01
#define BLE_ADV_LENGTH_FIELD_SIZE   1
#define BLE_ADV_AD_TYPE_FIELD_SIZE  1
#define BLE_AD_TYPE_FLAGS_DATA_SIZE 1

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))
#define UNIT_0_625_MS (625)
#define APP_CFG_NON_CONN_ADV_TIMEOUT 0 // Disable timeout.
#define NON_CONNECTABLE_ADV_INTERVAL MSEC_TO_UNITS(100, UNIT_0_625_MS)

void sd_advertise(void) {
    ble_uuid_t adv_uuids[] = {{.uuid = EDDYSTONE_UUID, .type = BLE_UUID_TYPE_BLE}};
    uint8_t encoded_size;
    uint8_t uuid_encoded[2];
    uint32_t err_code = sd_ble_uuid_encode(&adv_uuids[0], &encoded_size, uuid_encoded);

    printf("Encoded UUID size: " UINT_FMT ": result: " HEX2_FMT "\n", encoded_size, (uint16_t)err_code);
    printf("Encoded UUID: " HEX2_FMT " " HEX2_FMT "\n", uuid_encoded[0], uuid_encoded[1]);

    uint8_t eddystone_data[] = {EDDYSTONE_DATA}; // Temp buffer to calculate the size.

    uint8_t adv_data[] = {
        (uint8_t)(BLE_ADV_AD_TYPE_FIELD_SIZE + BLE_AD_TYPE_FLAGS_DATA_SIZE),
        BLE_GAP_AD_TYPE_FLAGS,
        BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,
        3,
        BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE,
        uuid_encoded[0], uuid_encoded[1],
        (uint8_t)(BLE_ADV_AD_TYPE_FIELD_SIZE + sizeof(eddystone_data) + 2),
        BLE_GAP_AD_TYPE_SERVICE_DATA,
        uuid_encoded[0], uuid_encoded[1],
        EDDYSTONE_DATA
    };

    // Scan response data not set.
    err_code = sd_ble_gap_adv_data_set(adv_data, sizeof(adv_data), NULL, 0);
    printf("Set Adv data status: " UINT_FMT ", size: " UINT_FMT "\n", (uint16_t)err_code, sizeof(adv_data));

    ble_gap_adv_params_t m_adv_params;

    // Initialize advertising params.
    memset(&m_adv_params, 0, sizeof(m_adv_params));
    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    m_adv_params.p_peer_addr = NULL;                                // Undirected advertisement.
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;

    err_code = sd_ble_gap_adv_start(&m_adv_params);

    printf("Advertisment start status: " UINT_FMT "\n", (uint16_t)err_code);
}

bool sd_uuid_add_vs(uint8_t * p_uuid, uint8_t * idx) {
    SD_TEST_OR_ENABLE();

    if (sd_ble_uuid_vs_add((ble_uuid128_t const *)p_uuid, idx) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Can not add Vendor Specific 128-bit UUID."));
    }

    return true;
}

bool sd_service_add(ubluepy_service_obj_t * p_service_obj) {
    SD_TEST_OR_ENABLE();

    if (p_service_obj->p_uuid->type > BLE_UUID_TYPE_BLE) {

        ble_uuid_t uuid;
        uuid.type = p_service_obj->p_uuid->uuid_vs_idx;
        uuid.uuid = (uint16_t)(*(uint16_t *)&p_service_obj->p_uuid->value[0]);

        if (sd_ble_gatts_service_add(p_service_obj->type,
                                     &uuid,
                                     &p_service_obj->handle) != 0)
        {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                      "Can not add Service."));
        }
    } else if (p_service_obj->p_uuid->type == BLE_UUID_TYPE_BLE) {

        ble_uuid_t uuid;
        uuid.type = p_service_obj->p_uuid->type;
        uuid.uuid = (uint16_t)(*(uint16_t *)&p_service_obj->p_uuid->value[0]);

        if (sd_ble_gatts_service_add(p_service_obj->type,
                                     &uuid,
                                     &p_service_obj->handle) != 0)
        {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                      "Can not add Service."));
        }
    }
    return true;
}

bool sd_characteristic_add(ubluepy_characteristic_obj_t * p_char_obj) {
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;


    uuid.type = p_char_obj->p_uuid->type;
    uuid.uuid = (uint16_t)(*(uint16_t *)&p_char_obj->p_uuid->value[0]);

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
    attr_char_value.max_len   = (GATT_MTU_SIZE_DEFAULT - 3);

    ble_gatts_char_handles_t handles;

    if (sd_ble_gatts_characteristic_add(p_char_obj->service_handle,
                                        &char_md,
                                        &attr_char_value,
                                        &handles) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Can not add Characteristic."));
    }

    // apply handles to object instance
    p_char_obj->handle           = handles.value_handle;
    p_char_obj->user_desc_handle = handles.user_desc_handle;
    p_char_obj->cccd_handle      = handles.cccd_handle;
    p_char_obj->sccd_handle      = handles.sccd_handle;

    return true;
}

bool sd_advertise_data(ubluepy_advertise_data_t * p_adv_params) {
    SD_TEST_OR_ENABLE();

    uint8_t byte_pos = 0;

    uint8_t adv_data[BLE_GAP_ADV_MAX_SIZE];

    if (p_adv_params->device_name_len > 0) {
        ble_gap_conn_sec_mode_t sec_mode;

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

        if (sd_ble_gap_device_name_set(&sec_mode,
                                       p_adv_params->p_device_name,
                                       p_adv_params->device_name_len) != 0) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
	                  "Can not apply device name in the stack."));
        }

        printf("Device name applied\n");

        adv_data[byte_pos] = (BLE_ADV_AD_TYPE_FIELD_SIZE + p_adv_params->device_name_len);
        byte_pos += BLE_ADV_LENGTH_FIELD_SIZE;
        adv_data[byte_pos] = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;
        byte_pos += BLE_ADV_AD_TYPE_FIELD_SIZE;
        memcpy(&adv_data[byte_pos], p_adv_params->p_device_name, p_adv_params->device_name_len);
        // increment position counter to see if it fits, and in case more content should
        // follow in this adv packet.
        byte_pos += p_adv_params->device_name_len;
    }

    // set flags, default to disc mode
    adv_data[byte_pos] = (BLE_ADV_AD_TYPE_FIELD_SIZE + BLE_AD_TYPE_FLAGS_DATA_SIZE);
    byte_pos += BLE_ADV_LENGTH_FIELD_SIZE;
    adv_data[byte_pos] = BLE_GAP_AD_TYPE_FLAGS;
    byte_pos += BLE_AD_TYPE_FLAGS_DATA_SIZE;
    adv_data[byte_pos] = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    byte_pos += 1;

    if (p_adv_params->num_of_services > 0) {

        uint8_t size_byte_pos = byte_pos;

        // skip length byte for now, apply total length post calculation
        byte_pos += BLE_ADV_LENGTH_FIELD_SIZE;

        adv_data[byte_pos] = BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE;
        byte_pos += BLE_ADV_AD_TYPE_FIELD_SIZE;

        uint8_t uuid_total_size = 0;
        uint8_t encoded_size    = 0;

        for (uint8_t i = 0; i < p_adv_params->num_of_services; i++) {
            ubluepy_service_obj_t * p_service = (ubluepy_service_obj_t *)p_adv_params->p_services[i];

            ble_uuid_t uuid;
            uuid.type = p_service->p_uuid->uuid_vs_idx;
            uuid.uuid = (uint16_t)(*(uint16_t *)&p_service->p_uuid->value[0]);

            // calculate total size of uuids
            if (sd_ble_uuid_encode(&uuid, &encoded_size, NULL) != 0) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                          "Can not encode UUID, to check length."));
            }

            // do encoding into the adv buffer
            if (sd_ble_uuid_encode(&uuid, &encoded_size, &adv_data[byte_pos]) != 0) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                          "Can encode UUID into the advertisment packet."));
            }

            printf("encoded uuid for service %u: ", 0);
            for (uint8_t j = 0; j < encoded_size; j++) {
                printf(HEX2_FMT " ", adv_data[byte_pos + j]);
            }
            printf("\n");

            uuid_total_size += encoded_size; // size of entry
            byte_pos        += encoded_size; // relative to adv data packet
            printf("ADV: uuid size: %u, type: %u, uuid: %u, vs_idx: %u\n",
                   encoded_size, p_service->p_uuid->type,
                   (uint16_t)(*(uint16_t *)&p_service->p_uuid->value[0]),
                   p_service->p_uuid->uuid_vs_idx);
        }

        adv_data[size_byte_pos] = (BLE_ADV_AD_TYPE_FIELD_SIZE + uuid_total_size);
    }


    // scan response data not set
    if (sd_ble_gap_adv_data_set(adv_data, byte_pos, NULL, 0) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Can not apply advertisment data."));
    }
    printf("Set Adv data size: " UINT_FMT "\n", byte_pos);

    static ble_gap_adv_params_t m_adv_params;

    // initialize advertising params
    memset(&m_adv_params, 0, sizeof(m_adv_params));
    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.p_peer_addr = NULL;                                // undirected advertisement
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = MSEC_TO_UNITS(100, UNIT_0_625_MS);   // approx 8 ms
    m_adv_params.timeout     = 0;                                   // infinite advertisment

    if (m_adv_in_progress && sd_ble_gap_adv_stop() != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Can not stop advertisment."));
    }

    m_adv_in_progress = false;
    uint32_t err_code = sd_ble_gap_adv_start(&m_adv_params);
    if (err_code != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Can not start advertisment. status: 0x" HEX2_FMT, (uint16_t)err_code));
    }

    m_adv_in_progress = true;

    return true;
}

void sd_event_handler_set(mp_obj_t obj, ubluepy_evt_callback_t evt_handler) {
    mp_observer = obj;
    ubluepy_event_handler = evt_handler;
}

static void ble_evt_handler(ble_evt_t * p_ble_evt) {
// S132 event ranges.
// Common 0x01 -> 0x0F
// GAP    0x10 -> 0x2F
// GATTC  0x30 -> 0x4F
// GATTS  0x50 -> 0x6F
// L2CAP  0x70 -> 0x8F

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            ubluepy_event_handler(mp_observer, BLE_GAP_EVT_CONNECTED, p_ble_evt->header.evt_len - sizeof(uint16_t), NULL);
            printf("GAP CONNECT\n");
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            printf("GAP DISCONNECT\n");
            break;

        case BLE_GATTS_EVT_WRITE:
            printf("GATTS write\n");
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            printf("GAP CONN PARAM UPDATE\n");
            break;

        default:
            printf(">>> unhandled evt: 0x" HEX2_FMT, p_ble_evt->header.evt_id);
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
