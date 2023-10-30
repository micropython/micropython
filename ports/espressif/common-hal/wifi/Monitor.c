/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#include "py/mpstate.h"
#include "py/runtime.h"

#include "shared-bindings/wifi/Monitor.h"
#include "shared-bindings/wifi/Packet.h"

#include "esp_log.h"

#define MONITOR_PAYLOAD_FCS_LEN     (4)
#define MONITOR_QUEUE_TIMEOUT_TICK  (0)

typedef struct {
    void *payload;
    unsigned channel;
    uint32_t length;
    signed rssi;
} monitor_packet_t;

static const char *TAG = "monitor";

static void wifi_monitor_cb(void *recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)recv_buf;

    // prepare packet
    monitor_packet_t packet = {
        .channel = pkt->rx_ctrl.channel,
        .length = pkt->rx_ctrl.sig_len,
        .rssi = pkt->rx_ctrl.rssi,
    };

    // for now, the monitor only dumps the length of the MISC type frame
    if (type != WIFI_PKT_MISC && !pkt->rx_ctrl.rx_state) {
        packet.length -= MONITOR_PAYLOAD_FCS_LEN;
        packet.payload = malloc(packet.length);
        if (packet.payload) {
            memcpy(packet.payload, pkt->payload, packet.length);
            wifi_monitor_obj_t *self = MP_STATE_VM(wifi_monitor_singleton);
            if (self->queue) {
                // send packet
                if (xQueueSendFromISR(self->queue, &packet, NULL) != pdTRUE) {
                    self->lost++;
                    free(packet.payload);
                    ESP_LOGE(TAG, "packet queue full");
                }
            }
        } else {
            ESP_LOGE(TAG, "not enough memory for packet");
        }
    }
}

void common_hal_wifi_monitor_construct(wifi_monitor_obj_t *self, uint8_t channel, size_t queue) {
    mp_rom_error_text_t monitor_mode_init_error = MP_ERROR_TEXT("monitor init failed");

    self->queue = xQueueCreate(queue, sizeof(monitor_packet_t));
    if (!self->queue) {
        mp_raise_RuntimeError(monitor_mode_init_error);
    }

    // start wifi promicuous mode
    wifi_promiscuous_filter_t wifi_filter = {
        .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT,
    };
    esp_wifi_set_promiscuous_filter(&wifi_filter);
    esp_wifi_set_promiscuous_rx_cb(wifi_monitor_cb);
    if (esp_wifi_set_promiscuous(true) != ESP_OK) {
        mp_raise_RuntimeError(monitor_mode_init_error);
    }
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    self->channel = channel;
    self->queue_length = queue;
}

bool common_hal_wifi_monitor_deinited(void) {
    bool enabled;
    return (esp_wifi_get_promiscuous(&enabled) == ESP_ERR_WIFI_NOT_INIT) ? true : !enabled;
}

void common_hal_wifi_monitor_deinit(wifi_monitor_obj_t *self) {
    if (common_hal_wifi_monitor_deinited()) {
        return;
    }

    // disable wifi promiscuous mode
    esp_wifi_set_promiscuous(false);

    // make sure to free all resources in the left items
    UBaseType_t left_items = uxQueueMessagesWaiting(self->queue);
    monitor_packet_t packet;
    while (left_items--) {
        xQueueReceive(self->queue, &packet, MONITOR_QUEUE_TIMEOUT_TICK);
        free(packet.payload);
    }
    vQueueDelete(self->queue);
    self->queue = NULL;
}

void common_hal_wifi_monitor_set_channel(wifi_monitor_obj_t *self, uint8_t channel) {
    self->channel = channel;
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

mp_obj_t common_hal_wifi_monitor_get_channel(wifi_monitor_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->channel);
}

mp_obj_t common_hal_wifi_monitor_get_queue(wifi_monitor_obj_t *self) {
    return mp_obj_new_int_from_uint(self->queue_length);
}

mp_obj_t common_hal_wifi_monitor_get_lost(wifi_monitor_obj_t *self) {
    size_t lost = self->lost;
    self->lost = 0;
    return mp_obj_new_int_from_uint(lost);
}

mp_obj_t common_hal_wifi_monitor_get_queued(wifi_monitor_obj_t *self) {
    return mp_obj_new_int_from_uint(uxQueueMessagesWaiting(self->queue));
}

mp_obj_t common_hal_wifi_monitor_get_packet(wifi_monitor_obj_t *self) {
    monitor_packet_t packet;

    if (xQueueReceive(self->queue, &packet, MONITOR_QUEUE_TIMEOUT_TICK) != pdTRUE) {
        return (mp_obj_t)&mp_const_empty_dict_obj;
    }

    mp_obj_dict_t *dict = MP_OBJ_TO_PTR(mp_obj_new_dict(4));

    mp_obj_dict_store(dict, cp_enum_find(&wifi_packet_type, PACKET_CH), MP_OBJ_NEW_SMALL_INT(packet.channel));

    mp_obj_dict_store(dict, cp_enum_find(&wifi_packet_type, PACKET_LEN), MP_OBJ_NEW_SMALL_INT(packet.length));

    mp_obj_dict_store(dict, cp_enum_find(&wifi_packet_type, PACKET_RAW), mp_obj_new_bytes(packet.payload, packet.length));
    free(packet.payload);

    mp_obj_dict_store(dict, cp_enum_find(&wifi_packet_type, PACKET_RSSI), MP_OBJ_NEW_SMALL_INT(packet.rssi));

    return MP_OBJ_FROM_PTR(dict);
}
