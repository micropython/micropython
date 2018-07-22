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

#include <stdbool.h>
#include <stdio.h>

#include "ble.h"
#include "ble_drv.h"
#include "nrf_nvic.h"
#include "nrf_sdm.h"
#include "py/misc.h"

nrf_nvic_state_t nrf_nvic_state = { 0 };

__attribute__((aligned(4)))
static uint8_t m_ble_evt_buf[sizeof(ble_evt_t) + (BLE_GATT_ATT_MTU_DEFAULT)];

typedef struct event_handler {
    struct event_handler *next;
    void *param;
    ble_drv_evt_handler_t func;
} event_handler_t;

static event_handler_t *m_event_handlers;

void ble_drv_add_event_handler(ble_drv_evt_handler_t func, void *param) {
    event_handler_t *handler = m_new_ll(event_handler_t, 1);
    handler->next = NULL;
    handler->param = param;
    handler->func = func;

    if (m_event_handlers == NULL) {
        m_event_handlers = handler;
        return;
    }

    event_handler_t *it = m_event_handlers;
    while (it->next != NULL) {
        if ((it->func == func) && (it->param == param)) {
            m_free(handler);
            return;
        }

        it = it->next;
    }

    it->next = handler;
}

void SD_EVT_IRQHandler(void) {
    uint32_t evt_id;
    while (sd_evt_get(&evt_id) != NRF_ERROR_NOT_FOUND) {
//        sd_evt_handler(evt_id);
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

        event_handler_t *it = m_event_handlers;
        while (it != NULL) {
            it->func((ble_evt_t *)m_ble_evt_buf, it->param);
            it = it->next;
        }
    }
}
