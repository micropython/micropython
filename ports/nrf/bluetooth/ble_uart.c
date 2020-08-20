/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include <string.h>

#include "ble.h"
#include "ble_uart.h"
#include "ringbuffer.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "lib/utils/interrupt_char.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Device.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"

#if CIRCUITPY_SERIAL_BLE

static const char default_name[] = "CP-REPL"; // max 8 chars or uuid won't fit in adv data
static const char NUS_UUID[] = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";

#define NUS_RX_UUID 0x0002
#define NUS_TX_UUID 0x0003
#define BUFFER_SIZE 128

ringBuffer_typedef(uint8_t, ringbuffer_t);

static bleio_device_obj_t m_device;
static bleio_service_obj_t *m_nus;
static bleio_characteristic_obj_t *m_tx_chara;
static bleio_characteristic_obj_t *m_rx_chara;

static volatile bool m_cccd_enabled;

static uint8_t m_rx_ring_buffer_data[BUFFER_SIZE];
static ringbuffer_t m_rx_ring_buffer = {
    .size = sizeof(m_rx_ring_buffer_data) + 1,
    .elems = m_rx_ring_buffer_data,
};

STATIC void on_ble_evt(ble_evt_t *ble_evt, void *param) {
    switch (ble_evt->header.evt_id) {
        case BLE_GAP_EVT_DISCONNECTED:
        {
            mp_obj_t device_obj = MP_OBJ_FROM_PTR(&m_device);
            mp_call_function_0(mp_load_attr(device_obj, qstr_from_str("start_advertising")));
            break;
        }

        case BLE_GATTS_EVT_WRITE:
        {
            ble_gatts_evt_write_t *write = &ble_evt->evt.gatts_evt.params.write;

            if (write->handle == m_tx_chara->cccd_handle) {
                m_cccd_enabled = true;
            } else if (write->handle == m_rx_chara->handle) {
                for (size_t i = 0; i < write->len; ++i) {
#if MICROPY_KBD_EXCEPTION
                    if (write->data[i] == mp_interrupt_char) {
                        mp_keyboard_interrupt();
                    } else
#endif
                    {
                        bufferWrite(&m_rx_ring_buffer, write->data[i]);
                    }
                }
            }
        }
    }
}

void ble_uart_init(void) {
    mp_obj_t device_obj = MP_OBJ_FROM_PTR(&m_device);
    m_device.base.type = &bleio_device_type;
    m_device.service_list = mp_obj_new_list(0, NULL);
    m_device.notif_handler = mp_const_none;
    m_device.conn_handler = mp_const_none;
    m_device.conn_handle = 0xFFFF;
    m_device.is_peripheral = true;
    m_device.name = mp_obj_new_str(default_name, strlen(default_name), false);
    common_hal_bleio_adapter_get_address(&m_device.address);

    mp_obj_t nus_uuid_str = mp_obj_new_str(NUS_UUID, strlen(NUS_UUID), false);
    mp_obj_t nus_uuid_obj = bleio_uuid_type.make_new(&bleio_uuid_type, 1, 0, &nus_uuid_str);
    mp_obj_t nus_obj = bleio_service_type.make_new(&bleio_service_type, 1, 0, &nus_uuid_obj);
    m_nus = MP_OBJ_TO_PTR(nus_obj);
    mp_call_function_1(mp_load_attr(device_obj, qstr_from_str("add_service")), nus_obj);

    mp_obj_t tx_uuid_int = mp_obj_new_int(NUS_TX_UUID);
    mp_obj_t tx_uuid_obj = bleio_uuid_type.make_new(&bleio_uuid_type, 1, 0, &tx_uuid_int);
    mp_obj_t tx_obj = bleio_characteristic_type.make_new(&bleio_characteristic_type, 1, 0, &tx_uuid_obj);
    m_tx_chara = MP_OBJ_TO_PTR(tx_obj);
    m_tx_chara->uuid->type = UUID_TYPE_128BIT;
    m_tx_chara->uuid->uuid_vs_idx = m_nus->uuid->uuid_vs_idx;
    m_tx_chara->props.notify = true;
    mp_call_function_1(mp_load_attr(nus_obj, qstr_from_str("add_characteristic")), tx_obj);

    mp_obj_t rx_uuid_int = mp_obj_new_int(NUS_RX_UUID);
    mp_obj_t rx_uuid_obj = bleio_uuid_type.make_new(&bleio_uuid_type, 1, 0, &rx_uuid_int);
    mp_obj_t rx_obj = bleio_characteristic_type.make_new(&bleio_characteristic_type, 1, 0, &rx_uuid_obj);
    m_rx_chara = MP_OBJ_TO_PTR(rx_obj);
    m_rx_chara->uuid->type = UUID_TYPE_128BIT;
    m_rx_chara->uuid->uuid_vs_idx = m_nus->uuid->uuid_vs_idx;
    m_rx_chara->props.write = true;
    m_rx_chara->props.write_wo_resp = true;
    mp_call_function_1(mp_load_attr(nus_obj, qstr_from_str("add_characteristic")), rx_obj);

    mp_call_function_0(mp_load_attr(device_obj, qstr_from_str("start_advertising")));

    ble_drv_add_event_handler(on_ble_evt, &m_device);

    m_cccd_enabled = false;

    while (!m_cccd_enabled) {
        RUN_BACKGROUND_TASKS;
    }
}

bool ble_uart_connected(void) {
    return (m_device.conn_handle != BLE_CONN_HANDLE_INVALID);
}

char ble_uart_rx_chr(void) {
    while (isBufferEmpty(&m_rx_ring_buffer)) {
        RUN_BACKGROUND_TASKS;
    }

    uint8_t byte;
    bufferRead(&m_rx_ring_buffer, byte);
    return (int)byte;
}

bool ble_uart_stdin_any(void) {
    return !isBufferEmpty(&m_rx_ring_buffer);
}

void ble_uart_stdout_tx_str(const char *text) {
    mp_hal_stdout_tx_strn(text, strlen(text));
}

int mp_hal_stdin_rx_chr(void) {
    return ble_uart_rx_chr();
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    size_t send_len;

    while (len > 0) {
        if (len >= BLE_GATT_ATT_MTU_DEFAULT - 3) {
            send_len = (BLE_GATT_ATT_MTU_DEFAULT - 3);
        } else {
            send_len = len;
        }

        mp_buffer_info_t bufinfo = {
            .buf = (uint8_t*)str,
            .len = send_len,
        };

        common_hal_bleio_characteristic_write_value(m_tx_chara, &bufinfo);

        len -= send_len;
        str += send_len;
    }
}

#endif // CIRCUITPY_SERIAL_BLE
