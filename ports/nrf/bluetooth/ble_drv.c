/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#include <stdbool.h>
#include <stdio.h>

#include "ble.h"
#include "ble_drv.h"
#include "nrf_nvic.h"
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "nrfx_power.h"
#include "py/misc.h"
#include "py/mpstate.h"

nrf_nvic_state_t nrf_nvic_state = { 0 };

// Flag indicating progress of internal flash operation.
volatile sd_flash_operation_status_t sd_flash_operation_status;

__attribute__((aligned(4)))
static uint8_t m_ble_evt_buf[sizeof(ble_evt_t) + (BLE_GATTS_VAR_ATTR_LEN_MAX)];

void ble_drv_reset() {
    // Linked list items will be gc'd.
    MP_STATE_VM(ble_drv_evt_handler_entries) = NULL;
    sd_flash_operation_status = SD_FLASH_OPERATION_DONE;
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
    ble_drv_evt_handler_entry_t *handler = m_new_ll(ble_drv_evt_handler_entry_t, 1);
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
            #if CIRCUITPY_USB
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

    while (1) {
        uint16_t evt_len = sizeof(m_ble_evt_buf);
        const uint32_t err_code = sd_ble_evt_get(m_ble_evt_buf, &evt_len);
        if (err_code != NRF_SUCCESS) {
            if (err_code == NRF_ERROR_DATA_SIZE) {
                printf("NRF_ERROR_DATA_SIZE\n");
            }

            break;
        }

        ble_evt_t *event = (ble_evt_t *)m_ble_evt_buf;
        #if CIRCUITPY_VERBOSE_BLE
        size_t eid = event->header.evt_id;
        if (eid != 0x1d) {
            if (BLE_GAP_EVT_BASE <= eid && eid <= BLE_GAP_EVT_LAST) {
                mp_printf(&mp_plat_print, "BLE GAP event: %d\n", eid - BLE_GAP_EVT_BASE);
            } else {
                mp_printf(&mp_plat_print, "BLE event: 0x%04x\n", event->header.evt_id);
            }
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
}
