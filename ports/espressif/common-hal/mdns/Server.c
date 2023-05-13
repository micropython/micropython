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

#include "components/mdns/include/mdns.h"

// Track whether the underlying IDF mdns has been started so that we only
// create a single inited MDNS object to CircuitPython. (After deinit, another
// could be created.)
STATIC bool mdns_started = false;

void mdns_server_construct(mdns_server_obj_t *self, bool workflow) {
    if (mdns_started) {
        // Mark this object as deinited because another is already using MDNS.
        self->inited = false;
        return;
    }
    mdns_init();
    mdns_started = true;

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
        mp_raise_ValueError(translate("mDNS only works with built-in WiFi"));
        return;
    }
    mdns_server_construct(self, false);
    if (common_hal_mdns_server_deinited(self)) {
        mp_raise_RuntimeError(translate("mDNS already initialized"));
    }
}

void common_hal_mdns_server_deinit(mdns_server_obj_t *self) {
    if (common_hal_mdns_server_deinited(self)) {
        return;
    }
    self->inited = false;
    mdns_started = false;
    mdns_free();
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
    mdns_result_t *results;
    while (!mdns_query_async_get_results(search, 1, &results)) {
        RUN_BACKGROUND_TASKS;
    }
    mdns_query_async_delete(search);
    // Count how many results we got.
    // TODO: Remove this loop when moving off 4.4. Newer APIs will give us num_results
    // back directly.
    mdns_result_t *next = results;
    uint8_t num_results = 0;
    while (next != NULL) {
        num_results++;
        next = next->next;
    }

    next = results;
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
        mp_raise_RuntimeError(translate("Unable to start mDNS query"));
    }
    mdns_result_t *results;
    while (!mdns_query_async_get_results(search, 1, &results)) {
        RUN_BACKGROUND_TASKS;
    }
    mdns_query_async_delete(search);
    // Count how many results we got.
    // TODO: Remove this loop when moving off 4.4. Newer APIs will give us num_results
    // back directly.
    mdns_result_t *next = results;
    uint8_t num_results = 0;
    while (next != NULL) {
        num_results++;
        next = next->next;
    }
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(num_results, NULL));
    // The empty tuple object is shared and stored in flash so return early if
    // we got it. Without this we'll crash when trying to set len below.
    if (num_results == 0) {
        return MP_OBJ_FROM_PTR(tuple);
    }
    next = results;
    // Don't error if we're out of memory. Instead, truncate the tuple.
    uint8_t added = 0;
    while (next != NULL) {
        mdns_remoteservice_obj_t *service = gc_alloc(sizeof(mdns_remoteservice_obj_t), GC_ALLOC_FLAG_HAS_FINALISER, false);
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

void common_hal_mdns_server_advertise_service(mdns_server_obj_t *self, const char *service_type, const char *protocol, mp_int_t port) {
    if (mdns_service_exists(service_type, protocol, NULL)) {
        mdns_service_port_set(service_type, protocol, port);
    } else {
        mdns_service_add(NULL, service_type, protocol, port, NULL, 0);
    }
}
