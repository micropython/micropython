// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdio.h>

#include "ble.h"
#include "ble_drv.h"
#include "nrf_nvic.h"
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "nrfx_power.h"
#include "py/gc.h"
#include "py/misc.h"
#include "py/mpstate.h"
#include "mpconfigport.h"

#if CIRCUITPY_SERIAL_BLE && CIRCUITPY_VERBOSE_BLE
#include "supervisor/shared/bluetooth/serial.h"
#endif

#if CIRCUITPY_VERBOSE_BLE
const char *ble_drv_evt_name(uint32_t evt) {
    switch (evt) {
        case BLE_GAP_EVT_CONNECTED:
            return "BLE_GAP_EVT_CONNECTED";
        case BLE_GAP_EVT_DISCONNECTED:
            return "BLE_GAP_EVT_DISCONNECTED";
        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            return "BLE_GAP_EVT_CONN_PARAM_UPDATE";
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            return "BLE_GAP_EVT_SEC_PARAMS_REQUEST";
        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            return "BLE_GAP_EVT_SEC_INFO_REQUEST";
        case BLE_GAP_EVT_PASSKEY_DISPLAY:
            return "BLE_GAP_EVT_PASSKEY_DISPLAY";
        case BLE_GAP_EVT_KEY_PRESSED:
            return "BLE_GAP_EVT_KEY_PRESSED";
        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            return "BLE_GAP_EVT_AUTH_KEY_REQUEST";
        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            return "BLE_GAP_EVT_LESC_DHKEY_REQUEST";
        case BLE_GAP_EVT_AUTH_STATUS:
            return "BLE_GAP_EVT_AUTH_STATUS";
        case BLE_GAP_EVT_CONN_SEC_UPDATE:
            return "BLE_GAP_EVT_CONN_SEC_UPDATE";
        case BLE_GAP_EVT_TIMEOUT:
            return "BLE_GAP_EVT_TIMEOUT";
        case BLE_GAP_EVT_RSSI_CHANGED:
            return "BLE_GAP_EVT_RSSI_CHANGED";
        case BLE_GAP_EVT_ADV_REPORT:
            return "BLE_GAP_EVT_ADV_REPORT";
        case BLE_GAP_EVT_SEC_REQUEST:
            return "BLE_GAP_EVT_SEC_REQUEST";
        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
            return "BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST";
        case BLE_GAP_EVT_SCAN_REQ_REPORT:
            return "BLE_GAP_EVT_SCAN_REQ_REPORT";
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
            return "BLE_GAP_EVT_PHY_UPDATE_REQUEST";
        case BLE_GAP_EVT_PHY_UPDATE:
            return "BLE_GAP_EVT_PHY_UPDATE";
        case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
            return "BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST";
        case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
            return "BLE_GAP_EVT_DATA_LENGTH_UPDATE";
        case BLE_GAP_EVT_QOS_CHANNEL_SURVEY_REPORT:
            return "BLE_GAP_EVT_QOS_CHANNEL_SURVEY_REPORT";
        case BLE_GAP_EVT_ADV_SET_TERMINATED:
            return "BLE_GAP_EVT_ADV_SET_TERMINATED";

        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
            return "BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP";
        case BLE_GATTC_EVT_REL_DISC_RSP:
            return "BLE_GATTC_EVT_REL_DISC_RSP";
        case BLE_GATTC_EVT_CHAR_DISC_RSP:
            return "BLE_GATTC_EVT_CHAR_DISC_RSP";
        case BLE_GATTC_EVT_DESC_DISC_RSP:
            return "BLE_GATTC_EVT_DESC_DISC_RSP";
        case BLE_GATTC_EVT_ATTR_INFO_DISC_RSP:
            return "BLE_GATTC_EVT_ATTR_INFO_DISC_RSP";
        case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
            return "BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP";
        case BLE_GATTC_EVT_READ_RSP:
            return "BLE_GATTC_EVT_READ_RSP";
        case BLE_GATTC_EVT_CHAR_VALS_READ_RSP:
            return "BLE_GATTC_EVT_CHAR_VALS_READ_RSP";
        case BLE_GATTC_EVT_WRITE_RSP:
            return "BLE_GATTC_EVT_WRITE_RSP";
        case BLE_GATTC_EVT_HVX:
            return "BLE_GATTC_EVT_HVX";
        case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
            return "BLE_GATTC_EVT_EXCHANGE_MTU_RSP";
        case BLE_GATTC_EVT_TIMEOUT:
            return "BLE_GATTC_EVT_TIMEOUT";
        case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:
            return "BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE";

        case BLE_GATTS_EVT_WRITE:
            return "BLE_GATTS_EVT_WRITE";
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            return "BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST";
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            return "BLE_GATTS_EVT_SYS_ATTR_MISSING";
        case BLE_GATTS_EVT_HVC:
            return "BLE_GATTS_EVT_HVC";
        case BLE_GATTS_EVT_SC_CONFIRM:
            return "BLE_GATTS_EVT_SC_CONFIRM";
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            return "BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST";
        case BLE_GATTS_EVT_TIMEOUT:
            return "BLE_GATTS_EVT_TIMEOUT";
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            return "BLE_GATTS_EVT_HVN_TX_COMPLETE";

        default:
            return "unknown EVT";
    }
};
#endif

nrf_nvic_state_t nrf_nvic_state = { 0 };

// Flag indicating progress of internal flash operation.
volatile sd_flash_operation_status_t sd_flash_operation_status;

__attribute__((aligned(4)))
static uint8_t m_ble_evt_buf[sizeof(ble_evt_t) + (BLE_GATTS_VAR_ATTR_LEN_MAX)];

void ble_drv_reset(void) {
    // Linked list items will be gc'd.
    MP_STATE_VM(ble_drv_evt_handler_entries) = NULL;
    sd_flash_operation_status = SD_FLASH_OPERATION_DONE;
}

void ble_drv_remove_heap_handlers(void) {
    ble_drv_evt_handler_entry_t *it = MP_STATE_VM(ble_drv_evt_handler_entries);
    while (it != NULL) {
        // If the param is on the heap, then delete the handler.
        if (gc_ptr_on_heap(it->param)) {
            ble_drv_remove_event_handler(it->func, it->param);
        }
        it = it->next;
    }
}

void ble_drv_add_event_handler_entry(ble_drv_evt_handler_entry_t *entry, ble_drv_evt_handler_t func, void *param) {
    ble_drv_evt_handler_entry_t *it = MP_STATE_VM(ble_drv_evt_handler_entries);
    while (it != NULL) {
        // If event handler and its corresponding param are already on the list, don't add again.
        if ((it->func == func) && (it->param == param)) {
            return;
        }
        it = it->next;
    }
    entry->next = MP_STATE_VM(ble_drv_evt_handler_entries);
    entry->param = param;
    entry->func = func;

    MP_STATE_VM(ble_drv_evt_handler_entries) = entry;
}

void ble_drv_add_event_handler(ble_drv_evt_handler_t func, void *param) {
    ble_drv_evt_handler_entry_t *it = MP_STATE_VM(ble_drv_evt_handler_entries);
    while (it != NULL) {
        // If event handler and its corresponding param are already on the list, don't add again.
        if ((it->func == func) && (it->param == param)) {
            return;
        }
        it = it->next;
    }

    // Add a new handler to the front of the list
    ble_drv_evt_handler_entry_t *handler = m_new(ble_drv_evt_handler_entry_t, 1);
    ble_drv_add_event_handler_entry(handler, func, param);
}

void ble_drv_remove_event_handler(ble_drv_evt_handler_t func, void *param) {
    ble_drv_evt_handler_entry_t *it = MP_STATE_VM(ble_drv_evt_handler_entries);
    ble_drv_evt_handler_entry_t **prev = &MP_STATE_VM(ble_drv_evt_handler_entries);
    while (it != NULL) {
        if ((it->func == func) && (it->param == param)) {
            // Splice out the matching handler.
            *prev = it->next;
            // Clear next of the removed node so it's clearly not in a list.
            it->next = NULL;
            return;
        }
        prev = &(it->next);
        it = it->next;
    }
}

extern void tusb_hal_nrf_power_event(uint32_t event);

void SD_EVT_IRQHandler(void) {
    uint32_t evt_id;
    while (sd_evt_get(&evt_id) != NRF_ERROR_NOT_FOUND) {
        switch (evt_id) {
            #if CIRCUITPY_USB_DEVICE
            // usb power event
            case NRF_EVT_POWER_USB_DETECTED:
            case NRF_EVT_POWER_USB_POWER_READY:
            case NRF_EVT_POWER_USB_REMOVED: {
                int32_t usbevt = (evt_id == NRF_EVT_POWER_USB_DETECTED) ? NRFX_POWER_USB_EVT_DETECTED:
                    (evt_id == NRF_EVT_POWER_USB_POWER_READY) ? NRFX_POWER_USB_EVT_READY   :
                    (evt_id == NRF_EVT_POWER_USB_REMOVED) ? NRFX_POWER_USB_EVT_REMOVED : -1;

                tusb_hal_nrf_power_event(usbevt);
            }
            break;
            #endif

            // Set flag indicating that a flash operation has finished.
            case NRF_EVT_FLASH_OPERATION_SUCCESS:
                sd_flash_operation_status = SD_FLASH_OPERATION_DONE;
                break;
            case NRF_EVT_FLASH_OPERATION_ERROR:
                sd_flash_operation_status = SD_FLASH_OPERATION_ERROR;
                break;

            default:
                break;
        }
    }

    #if CIRCUITPY_SERIAL_BLE && CIRCUITPY_VERBOSE_BLE
    ble_serial_disable();
    #endif
    while (1) {
        uint16_t evt_len = sizeof(m_ble_evt_buf);
        const uint32_t err_code = sd_ble_evt_get(m_ble_evt_buf, &evt_len);
        if (err_code != NRF_SUCCESS) {
            if (err_code == NRF_ERROR_DATA_SIZE) {
                mp_printf(&mp_plat_print, "NRF_ERROR_DATA_SIZE\n");
            }

            break;
        }

        ble_evt_t *event = (ble_evt_t *)m_ble_evt_buf;

        #if CIRCUITPY_VERBOSE_BLE
        size_t eid = event->header.evt_id;
        if (eid != BLE_GAP_EVT_ADV_REPORT) {
            mp_printf(&mp_plat_print, "BLE event: %s (0x%04x)\n", ble_drv_evt_name(eid), eid);
        }
        #endif

        ble_drv_evt_handler_entry_t *it = MP_STATE_VM(ble_drv_evt_handler_entries);
        bool done = false;
        while (it != NULL) {
            #if CIRCUITPY_VERBOSE_BLE
            // mp_printf(&mp_plat_print, "  calling handler: 0x%08lx, param: 0x%08lx\n", it->func - 1, it->param);
            #endif
            // Capture next before calling the function in case it removes itself from the list.
            ble_drv_evt_handler_entry_t *next = it->next;
            done = it->func(event, it->param) || done;
            it = next;
        }
        #if CIRCUITPY_VERBOSE_BLE
        if (event->header.evt_id == BLE_GATTS_EVT_WRITE) {
            ble_gatts_evt_write_t *write_evt = &event->evt.gatts_evt.params.write;
            mp_printf(&mp_plat_print, "Write to: UUID(0x%04x) handle %x of length %d auth %x\n", write_evt->uuid.uuid, write_evt->handle, write_evt->len, write_evt->auth_required);
        }
        #endif
    }
    #if CIRCUITPY_SERIAL_BLE && CIRCUITPY_VERBOSE_BLE
    ble_serial_enable();
    #endif
}

void ble_drv_gc_collect(void) {
    ble_drv_evt_handler_entry_t *it = MP_STATE_VM(ble_drv_evt_handler_entries);
    while (it != NULL) {
        gc_collect_ptr(it);
        it = it->next;
    }
}

MP_REGISTER_ROOT_POINTER(ble_drv_evt_handler_entry_t * ble_drv_evt_handler_entries);
