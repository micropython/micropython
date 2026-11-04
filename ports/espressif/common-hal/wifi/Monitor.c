// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/mpstate.h"
#include "py/runtime.h"

#include "bindings/espidf/__init__.h"
#include "shared-bindings/wifi/Monitor.h"
#include "shared-bindings/wifi/Packet.h"
#include "supervisor/port_heap.h"

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_memory_utils.h"

#define MONITOR_PAYLOAD_FCS_LEN     (4)
#define MONITOR_QUEUE_TIMEOUT_TICK  (0)
#define MONITOR_INTERNAL_RAM_FLOOR  (32 * 1024)

typedef struct {
    void *payload;
    unsigned channel;
    uint32_t length;
    signed rssi;
} monitor_packet_t;

static const char *TAG = "monitor";

// port_malloc() tries PSRAM first. Keep a frame in internal RAM only while
// enough stays free for WiFi, DMA bounce buffers and the like.
static void *alloc_payload(size_t length) {
    void *payload = port_malloc(length, false);
    if (payload != NULL && esp_ptr_internal(payload) &&
        heap_caps_get_free_size(MALLOC_CAP_INTERNAL) < MONITOR_INTERNAL_RAM_FLOOR) {
        port_free(payload);
        payload = NULL;
    }
    return payload;
}

static void wifi_monitor_cb(void *recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)recv_buf;

    // Construct enables promiscuous mode before make_new sets the singleton.
    wifi_monitor_obj_t *self = MP_STATE_VM(wifi_monitor_singleton);
    if (self == NULL || common_hal_wifi_monitor_deinited(self)) {
        return;
    }

    // prepare packet
    monitor_packet_t packet = {
        .channel = pkt->rx_ctrl.channel,
        .length = pkt->rx_ctrl.sig_len,
        .rssi = pkt->rx_ctrl.rssi,
    };

    // for now, the monitor only dumps the length of the MISC type frame
    if (type != WIFI_PKT_MISC && !pkt->rx_ctrl.rx_state) {
        packet.length -= MONITOR_PAYLOAD_FCS_LEN;
        packet.payload = alloc_payload(packet.length);
        if (packet.payload) {
            memcpy(packet.payload, pkt->payload, packet.length);
            // send packet
            if (xQueueSendFromISR(self->queue, &packet, NULL) != pdTRUE) {
                self->lost++;
                port_free(packet.payload);
                ESP_LOGE(TAG, "packet queue full");
            }
        } else {
            self->lost++;
            ESP_LOGE(TAG, "not enough memory for packet");
        }
    }
}

// Tune the radio and return the channel it is on afterwards. Raises
// ValueError for a channel this radio cannot use. Any other failure, for
// example while the station is associated, leaves the radio where it was.
static uint8_t set_channel(uint8_t channel) {
    esp_err_t result = esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    if (result == ESP_ERR_INVALID_ARG) {
        mp_arg_error_invalid(MP_QSTR_channel);
    } else if (result != ESP_OK) {
        wifi_second_chan_t second;
        CHECK_ESP_RESULT(esp_wifi_get_channel(&channel, &second));
    }
    return channel;
}

void common_hal_wifi_monitor_construct(wifi_monitor_obj_t *self, uint8_t channel, size_t queue) {
    mp_rom_error_text_t monitor_mode_init_error = MP_ERROR_TEXT("monitor init failed");

    // Before anything needs undoing on failure.
    channel = set_channel(channel);

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

    self->channel = channel;
    self->queue_length = queue;
}

bool common_hal_wifi_monitor_deinited(wifi_monitor_obj_t *self) {
    return self->queue == NULL;
}

void common_hal_wifi_monitor_deinit(wifi_monitor_obj_t *self) {
    if (common_hal_wifi_monitor_deinited(self)) {
        return;
    }

    // disable wifi promiscuous mode
    esp_wifi_set_promiscuous(false);

    // make sure to free all resources in the left items
    UBaseType_t left_items = uxQueueMessagesWaiting(self->queue);
    monitor_packet_t packet;
    while (left_items--) {
        xQueueReceive(self->queue, &packet, MONITOR_QUEUE_TIMEOUT_TICK);
        port_free(packet.payload);
    }
    vQueueDelete(self->queue);
    self->queue = NULL;
    // Root pointers outlive the VM heap.
    MP_STATE_VM(wifi_monitor_singleton) = NULL;
}

void common_hal_wifi_monitor_set_channel(wifi_monitor_obj_t *self, uint8_t channel) {
    self->channel = set_channel(channel);
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

    // Peek, so that if building the dict raises MemoryError, the packet stays
    // queued for the next call instead of being lost and its payload leaked.
    // Only this task removes packets, so the one received below is this one.
    if (xQueuePeek(self->queue, &packet, MONITOR_QUEUE_TIMEOUT_TICK) != pdTRUE) {
        return (mp_obj_t)&mp_const_empty_dict_obj;
    }

    mp_obj_dict_t *dict = MP_OBJ_TO_PTR(mp_obj_new_dict(4));

    mp_obj_dict_store(dict, cp_enum_find(&wifi_packet_type, PACKET_CH), MP_OBJ_NEW_SMALL_INT(packet.channel));

    mp_obj_dict_store(dict, cp_enum_find(&wifi_packet_type, PACKET_LEN), MP_OBJ_NEW_SMALL_INT(packet.length));

    mp_obj_dict_store(dict, cp_enum_find(&wifi_packet_type, PACKET_RAW), mp_obj_new_bytes(packet.payload, packet.length));

    mp_obj_dict_store(dict, cp_enum_find(&wifi_packet_type, PACKET_RSSI), MP_OBJ_NEW_SMALL_INT(packet.rssi));

    xQueueReceive(self->queue, &packet, MONITOR_QUEUE_TIMEOUT_TICK);
    port_free(packet.payload);

    return MP_OBJ_FROM_PTR(dict);
}
