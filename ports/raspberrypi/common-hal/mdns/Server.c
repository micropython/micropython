// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mdns/Server.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/mdns/RemoteService.h"
#include "shared-bindings/mdns/Server.h"
#include "shared-bindings/wifi/__init__.h"
#include "supervisor/shared/tick.h"

#include "lwip/apps/mdns.h"
#include "lwip/prot/dns.h"

// Track if we've inited the LWIP MDNS at all. It expects to only init once.
// Subsequent times, we restart it.
static bool inited = false;
// Track if we are globally inited. This essentially forces one inited MDNS
// object at a time. (But ignores MDNS objects that are deinited.)
static bool object_inited = false;

#define NETIF_STA (&cyw43_state.netif[CYW43_ITF_STA])
#define NETIF_AP (&cyw43_state.netif[CYW43_ITF_AP])

void mdns_server_construct(mdns_server_obj_t *self, bool workflow) {
    if (object_inited) {
        // Mark the object as deinit since another is already using MDNS.
        self->inited = false;
        return;
    }

    if (!inited) {
        mdns_resp_init();
        inited = true;
    } else {
        mdns_resp_restart(NETIF_STA);
    }
    self->inited = true;

    uint8_t mac[6];
    wifi_radio_get_mac_address(&common_hal_wifi_radio_obj, mac);
    snprintf(self->default_hostname, sizeof(self->default_hostname), "cpy-%02x%02x%02x", mac[3], mac[4], mac[5]);
    common_hal_mdns_server_set_hostname(self, self->default_hostname);

    if (workflow) {
        // Add a second host entry to respond to "circuitpython.local" queries as well.

        #if MDNS_MAX_SECONDARY_HOSTNAMES > 0
        mdns_resp_add_secondary_hostname(NETIF_STA, "circuitpython");
        #endif
    }
}

void common_hal_mdns_server_construct(mdns_server_obj_t *self, mp_obj_t network_interface) {
    if (network_interface != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(MP_ERROR_TEXT("mDNS only works with built-in WiFi"));
        return;
    }
    if (object_inited) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("mDNS already initialized"));
    }
    mdns_server_construct(self, false);
}

void common_hal_mdns_server_deinit(mdns_server_obj_t *self) {
    if (common_hal_mdns_server_deinited(self)) {
        return;
    }
    self->inited = false;
    object_inited = false;
    mdns_resp_remove_netif(NETIF_STA);
}

bool common_hal_mdns_server_deinited(mdns_server_obj_t *self) {
    return !self->inited;
}

const char *common_hal_mdns_server_get_hostname(mdns_server_obj_t *self) {
    return self->hostname;
}

void common_hal_mdns_server_set_hostname(mdns_server_obj_t *self, const char *hostname) {
    if (mdns_resp_netif_active(NETIF_STA)) {
        mdns_resp_rename_netif(NETIF_STA, hostname);
    } else {
        mdns_resp_add_netif(NETIF_STA, hostname);
    }

    self->hostname = hostname;
}

const char *common_hal_mdns_server_get_instance_name(mdns_server_obj_t *self) {
    return self->instance_name;
}

void common_hal_mdns_server_set_instance_name(mdns_server_obj_t *self, const char *instance_name) {
    self->instance_name = instance_name;
}

typedef struct {
    uint8_t request_id;
    size_t i;
    mdns_remoteservice_obj_t *out;
    size_t out_len;
} nonalloc_search_state_t;

static void copy_data_into_remote_service(struct mdns_answer *answer, const char *varpart, int varlen, mdns_remoteservice_obj_t *out) {
    if (varlen > 0) {
        if (answer->info.type == DNS_RRTYPE_A) {
            char *hostname = out->hostname;
            size_t len = MIN(63, answer->info.domain.name[0]);
            memcpy(hostname, answer->info.domain.name + 1, len);
            hostname[len] = '\0';
            out->ipv4_address = varpart[0] | varpart[1] << 8 | varpart[2] << 16 | varpart[3] << 24;
        }
        if (answer->info.type == DNS_RRTYPE_SRV) {
            // This isn't a null terminated string. Its length encoded.
            uint8_t *domain = answer->info.domain.name;
            char *instance_name = out->instance_name;
            size_t offset = 0;
            uint8_t iname_len = domain[offset++];
            size_t len = MIN(63, iname_len);
            memcpy(instance_name, domain + offset, len);
            offset += iname_len;
            instance_name[len] = '\0';

            uint8_t sn_len = domain[offset++];
            char *service_name = out->service_name;
            len = MIN(16, sn_len);
            memcpy(service_name, domain + offset, len);
            offset += sn_len;
            service_name[len] = '\0';

            uint8_t proto_len = domain[offset++];
            char *protocol = out->protocol;
            len = MIN(4, proto_len);
            memcpy(protocol, domain + offset, len);
            offset += proto_len;
            protocol[len] = '\0';

            out->port = varpart[4] << 8 | varpart[5];
        }
    }
}

static void search_result_cb(struct mdns_answer *answer, const char *varpart, int varlen, int flags, void *arg) {
    nonalloc_search_state_t *state = arg;
    state->out[state->i].base.type = &mdns_remoteservice_type;

    copy_data_into_remote_service(answer, varpart, varlen, &state->out[state->i]);

    if ((flags & MDNS_SEARCH_RESULT_LAST) != 0) {
        state->i += 1;
    }

    if (state->i == state->out_len) {
        mdns_search_stop(state->request_id);
        state->request_id = MDNS_MAX_REQUESTS;
    }
}

size_t mdns_server_find(mdns_server_obj_t *self, const char *service_type, const char *protocol,
    mp_float_t timeout, mdns_remoteservice_obj_t *out, size_t out_len) {

    enum mdns_sd_proto proto = DNSSD_PROTO_UDP;
    if (strcmp(protocol, "_tcp") == 0) {
        proto = DNSSD_PROTO_TCP;
    }

    nonalloc_search_state_t state;
    state.i = 0;
    state.out = out;
    state.out_len = out_len;

    err_t err = mdns_search_service(NULL, service_type, proto,
        NETIF_STA, &search_result_cb, &state,
        &state.request_id);
    if (err != ERR_OK) {
        return 0;
    }

    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t timeout_ms = timeout * 1000;

    while (state.request_id < MDNS_MAX_REQUESTS &&
           !mp_hal_is_interrupted() &&
           supervisor_ticks_ms64() - start_ticks < timeout_ms) {
        RUN_BACKGROUND_TASKS;
    }
    if (state.request_id < MDNS_MAX_REQUESTS) {
        mdns_search_stop(state.request_id);
        state.request_id = MDNS_MAX_REQUESTS;
    }

    return state.i;
}

typedef struct {
    uint8_t request_id;
    mdns_remoteservice_obj_t *head;
    size_t count;
} alloc_search_state_t;

static void alloc_search_result_cb(struct mdns_answer *answer, const char *varpart, int varlen, int flags, void *arg) {
    alloc_search_state_t *state = arg;

    if ((flags & MDNS_SEARCH_RESULT_FIRST) != 0) {
        // first
        mdns_remoteservice_obj_t *service = m_malloc(sizeof(mdns_remoteservice_obj_t));
        if (service == NULL) {
            // alloc fails
            mdns_search_stop(state->request_id);
            state->request_id = MDNS_MAX_REQUESTS;
            if (state->count == 0) {
                m_malloc_fail(sizeof(mdns_remoteservice_obj_t));
            }
            return;
        }
        service->base.type = &mdns_remoteservice_type;
        state->count++;
        service->next = state->head;
        state->head = service;
    }

    copy_data_into_remote_service(answer, varpart, varlen, state->head);
}

mp_obj_t common_hal_mdns_server_find(mdns_server_obj_t *self, const char *service_type, const char *protocol, mp_float_t timeout) {
    enum mdns_sd_proto proto = DNSSD_PROTO_UDP;
    if (strcmp(protocol, "_tcp") == 0) {
        proto = DNSSD_PROTO_TCP;
    }

    alloc_search_state_t state;
    state.count = 0;
    state.head = NULL;

    err_t err = mdns_search_service(NULL, service_type, proto,
        NETIF_STA, &alloc_search_result_cb, &state,
        &state.request_id);
    if (err != ERR_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Unable to start mDNS query"));
    }

    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t timeout_ms = timeout * 1000;

    while (state.request_id < MDNS_MAX_REQUESTS &&
           !mp_hal_is_interrupted() &&
           supervisor_ticks_ms64() - start_ticks < timeout_ms) {
        RUN_BACKGROUND_TASKS;
    }
    if (state.request_id < MDNS_MAX_REQUESTS) {
        mdns_search_stop(state.request_id);
        state.request_id = MDNS_MAX_REQUESTS;
    }

    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(state.count, NULL));
    mdns_remoteservice_obj_t *next = state.head;
    uint8_t added = 0;
    while (next != NULL) {
        mdns_remoteservice_obj_t *cur = next;
        tuple->items[added] = MP_OBJ_FROM_PTR(cur);
        next = cur->next;
        // Set next back to NULL so that each service object is independently
        // tracked for GC.
        cur->next = NULL;
        added++;
    }

    return MP_OBJ_FROM_PTR(tuple);
}

static void srv_txt_cb(struct mdns_service *service, void *ptr) {
    mdns_server_obj_t *self = ptr;
    err_t res;
    for (size_t i = 0; i < self->num_txt_records; i++) {
        res = mdns_resp_add_service_txtitem(service, self->txt_records[i], strlen(self->txt_records[i]));
        if (res != ERR_OK) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to add service TXT record"));
            return;
        }
    }
}

static void assign_txt_records(mdns_server_obj_t *self, const char *txt_records[], size_t num_txt_records) {
    size_t allowed_num_txt_records = MDNS_MAX_TXT_RECORDS < num_txt_records ? MDNS_MAX_TXT_RECORDS : num_txt_records;
    self->num_txt_records = allowed_num_txt_records;
    for (size_t i = 0; i < allowed_num_txt_records; i++) {
        self->txt_records[i] = txt_records[i];
    }
}

void common_hal_mdns_server_advertise_service(mdns_server_obj_t *self, const char *service_type, const char *protocol, mp_int_t port, const char *txt_records[], size_t num_txt_records) {
    enum mdns_sd_proto proto = DNSSD_PROTO_UDP;
    if (strcmp(protocol, "_tcp") == 0) {
        proto = DNSSD_PROTO_TCP;
    }
    // Remove the existing service if it was already added.
    int8_t existing_slot = MDNS_MAX_SERVICES;
    for (int i = 0; i < MDNS_MAX_SERVICES; i++) {
        if (self->service_type[i] != NULL &&
            (service_type == self->service_type[i] ||
             strcmp(service_type, self->service_type[i]) == 0)) {
            existing_slot = i;
            break;
        }
    }
    if (existing_slot < MDNS_MAX_SERVICES) {
        mdns_resp_del_service(NETIF_STA, existing_slot);
    }

    assign_txt_records(self, txt_records, num_txt_records);
    int8_t slot = mdns_resp_add_service(NETIF_STA, self->instance_name, service_type, proto, port, srv_txt_cb, self);
    if (slot < 0) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Out of MDNS service slots"));
        return;
    }
    self->service_type[slot] = service_type;
}
