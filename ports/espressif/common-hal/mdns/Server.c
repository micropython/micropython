/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/mdns/Server.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/mdns/RemoteService.h"
#include "shared-bindings/wifi/__init__.h"

#include "mdns.h"

// Track whether the underlying IDF mdns has been started so that we only
// create a single inited MDNS object to CircuitPython. (After deinit, another
// could be created.)
STATIC mdns_server_obj_t *_active_object = NULL;

void mdns_server_construct(mdns_server_obj_t *self, bool workflow) {
    if (_active_object != NULL) {
        if (self == _active_object) {
            return;
        }
        // Mark this object as deinited because another is already using MDNS.
        self->inited = false;
        return;
    }
    esp_err_t ret = mdns_init();
    if (ret != ESP_OK) {
        return;
    }
    _active_object = self;

    uint8_t mac[6];
    esp_netif_get_mac(common_hal_wifi_radio_obj.netif, mac);
    snprintf(self->default_hostname, sizeof(self->default_hostname), "cpy-%02x%02x%02x", mac[3], mac[4], mac[5]);
    common_hal_mdns_server_set_hostname(self, self->default_hostname);

    self->inited = true;

    if (workflow) {
        // Set a delegated entry to ourselves. This allows us to respond to "circuitpython.local"
        // queries as well.
        mdns_ip_addr_t our_ip;
        esp_netif_get_ip_info(common_hal_wifi_radio_obj.netif, &common_hal_wifi_radio_obj.ip_info);
        our_ip.next = NULL;
        our_ip.addr.type = ESP_IPADDR_TYPE_V4;
        our_ip.addr.u_addr.ip4 = common_hal_wifi_radio_obj.ip_info.ip;
        our_ip.addr.u_addr.ip6.addr[1] = 0;
        our_ip.addr.u_addr.ip6.addr[2] = 0;
        our_ip.addr.u_addr.ip6.addr[3] = 0;
        our_ip.addr.u_addr.ip6.zone = 0;
        mdns_delegate_hostname_add("circuitpython", &our_ip);
    }
}

void common_hal_mdns_server_construct(mdns_server_obj_t *self, mp_obj_t network_interface) {
    if (network_interface != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(MP_ERROR_TEXT("mDNS only works with built-in WiFi"));
        return;
    }
    mdns_server_construct(self, false);
    if (common_hal_mdns_server_deinited(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("mDNS already initialized"));
    }
}

void common_hal_mdns_server_deinit(mdns_server_obj_t *self) {
    if (common_hal_mdns_server_deinited(self)) {
        return;
    }
    self->inited = false;
    _active_object = NULL;
    mdns_free();
}

void mdns_server_deinit_singleton(void) {
    if (_active_object != NULL) {
        common_hal_mdns_server_deinit(_active_object);
    }
}

bool common_hal_mdns_server_deinited(mdns_server_obj_t *self) {
    return !self->inited;
}

const char *common_hal_mdns_server_get_hostname(mdns_server_obj_t *self) {
    return self->hostname;
}

void common_hal_mdns_server_set_hostname(mdns_server_obj_t *self, const char *hostname) {
    mdns_hostname_set(hostname);
    // Wait for the mdns task to set the new hostname.
    while (!mdns_hostname_exists(hostname)) {
        RUN_BACKGROUND_TASKS;
    }
    self->hostname = hostname;
}

const char *common_hal_mdns_server_get_instance_name(mdns_server_obj_t *self) {
    if (self->instance_name == NULL) {
        return self->hostname;
    }
    return self->instance_name;
}

void common_hal_mdns_server_set_instance_name(mdns_server_obj_t *self, const char *instance_name) {
    mdns_instance_name_set(instance_name);
    self->instance_name = instance_name;
}

size_t mdns_server_find(mdns_server_obj_t *self, const char *service_type, const char *protocol,
    mp_float_t timeout, mdns_remoteservice_obj_t *out, size_t out_len) {
    mdns_search_once_t *search = mdns_query_async_new(NULL, service_type, protocol, MDNS_TYPE_PTR, timeout * 1000, 255, NULL);
    if (search == NULL) {
        return 0;
    }
    uint8_t num_results;
    mdns_result_t *results;
    while (!mdns_query_async_get_results(search, 1, &results, &num_results)) {
        RUN_BACKGROUND_TASKS;
    }
    mdns_query_async_delete(search);
    mdns_result_t *next = results;
    // Don't error if we're out of memory. Instead, truncate the tuple.
    uint8_t added = 0;
    while (next != NULL && added < out_len) {
        mdns_remoteservice_obj_t *service = &out[added];

        service->result = next;
        service->base.type = &mdns_remoteservice_type;
        next = next->next;
        // Break the linked list so we free each result separately.
        service->result->next = NULL;
        added++;
    }
    if (added < out_len) {
        // Free the remaining results from the IDF because we don't have
        // enough space in Python.
        mdns_query_results_free(next);
    }
    return num_results;
}

mp_obj_t common_hal_mdns_server_find(mdns_server_obj_t *self, const char *service_type, const char *protocol, mp_float_t timeout) {
    mdns_search_once_t *search = mdns_query_async_new(NULL, service_type, protocol, MDNS_TYPE_PTR, timeout * 1000, 255, NULL);
    if (search == NULL) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Unable to start mDNS query"));
    }
    uint8_t num_results;
    mdns_result_t *results;
    while (!mdns_query_async_get_results(search, 1, &results, &num_results)) {
        RUN_BACKGROUND_TASKS;
    }
    mdns_query_async_delete(search);
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(num_results, NULL));
    // The empty tuple object is shared and stored in flash so return early if
    // we got it. Without this we'll crash when trying to set len below.
    if (num_results == 0) {
        return MP_OBJ_FROM_PTR(tuple);
    }
    mdns_result_t *next = results;
    // Don't error if we're out of memory. Instead, truncate the tuple.
    uint8_t added = 0;
    while (next != NULL) {
        mdns_remoteservice_obj_t *service = gc_alloc(sizeof(mdns_remoteservice_obj_t), GC_ALLOC_FLAG_HAS_FINALISER);
        if (service == NULL) {
            if (added == 0) {
                m_malloc_fail(sizeof(mdns_remoteservice_obj_t));
            }
            // Free the remaining results from the IDF because we don't have
            // enough space in Python.
            mdns_query_results_free(next);
            break;
        }
        service->result = next;
        service->base.type = &mdns_remoteservice_type;
        next = next->next;
        // Break the linked list so we free each result separately.
        service->result->next = NULL;
        tuple->items[added] = MP_OBJ_FROM_PTR(service);
        added++;
    }
    tuple->len = added;

    return MP_OBJ_FROM_PTR(tuple);
}

void common_hal_mdns_server_advertise_service(mdns_server_obj_t *self, const char *service_type, const char *protocol, mp_int_t port, const char *txt_records[], size_t num_txt_records) {
    if (mdns_service_exists(service_type, protocol, NULL)) {
        mdns_service_port_set(service_type, protocol, port);
    } else {
        // TODO: Add support for TXT record
        /* NOTE: The `mdns_txt_item_t *txt` argument of mdns_service_add uses a struct
         * that splits out the TXT record into keys and values, though it seems little
         * is done with those fields aside from concatenating them with an optional
         * equals sign and calculating the total length of the concatenated string.
         *
         * There should be little issue with the underlying implementation to populate
         * the mdns_txt_item_t struct with only a key containing exactly the desired TXT
         * record. As long as the underlying implementation calculates the length of the
         * key + NULL value correctly, it should work.
         *
         * Ref: RFC 6763, section 6.1:
         * > The format of each constituent string within the DNS TXT record is a single
         * > length byte, followed by 0-255 bytes of text data.
         */
        mdns_service_add(NULL, service_type, protocol, port, NULL, 0);
    }
}
