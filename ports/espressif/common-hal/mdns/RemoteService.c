// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mdns/RemoteService.h"

#include "shared-bindings/ipaddress/IPv4Address.h"

const char *common_hal_mdns_remoteservice_get_service_type(mdns_remoteservice_obj_t *self) {
    if (self->result == NULL) {
        return "";
    }
    return self->result->service_type;
}

const char *common_hal_mdns_remoteservice_get_protocol(mdns_remoteservice_obj_t *self) {
    if (self->result == NULL) {
        return "";
    }
    return self->result->proto;
}

const char *common_hal_mdns_remoteservice_get_instance_name(mdns_remoteservice_obj_t *self) {
    if (self->result == NULL) {
        return "";
    }
    return self->result->instance_name;
}

const char *common_hal_mdns_remoteservice_get_hostname(mdns_remoteservice_obj_t *self) {
    if (self->result == NULL) {
        return "";
    }
    return self->result->hostname;
}

mp_int_t common_hal_mdns_remoteservice_get_port(mdns_remoteservice_obj_t *self) {
    if (self->result == NULL) {
        return 0;
    }
    return self->result->port;
}

uint32_t mdns_remoteservice_get_ipv4_address(mdns_remoteservice_obj_t *self) {
    if (self->result == NULL ||
        self->result->ip_protocol != MDNS_IP_PROTOCOL_V4 ||
        self->result->addr == NULL) {
        return 0;
    }
    mdns_ip_addr_t *cur = self->result->addr;
    while (cur != NULL) {
        if (cur->addr.type == ESP_IPADDR_TYPE_V4) {
            return cur->addr.u_addr.ip4.addr;
        }

        cur = cur->next;
    }

    return 0;
}

mp_obj_t common_hal_mdns_remoteservice_get_ipv4_address(mdns_remoteservice_obj_t *self) {
    uint32_t addr = mdns_remoteservice_get_ipv4_address(self);
    if (addr == 0) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(addr);
}

void common_hal_mdns_remoteservice_deinit(mdns_remoteservice_obj_t *self) {
    mdns_query_results_free(self->result);
    self->result = NULL;
}
