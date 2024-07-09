// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mdns/RemoteService.h"

#include "shared-bindings/ipaddress/IPv4Address.h"

const char *common_hal_mdns_remoteservice_get_service_type(mdns_remoteservice_obj_t *self) {
    return self->service_name;
}

const char *common_hal_mdns_remoteservice_get_protocol(mdns_remoteservice_obj_t *self) {
    return self->protocol;
}

const char *common_hal_mdns_remoteservice_get_instance_name(mdns_remoteservice_obj_t *self) {
    return self->instance_name;
}

const char *common_hal_mdns_remoteservice_get_hostname(mdns_remoteservice_obj_t *self) {
    return self->hostname;
}

mp_int_t common_hal_mdns_remoteservice_get_port(mdns_remoteservice_obj_t *self) {
    return self->port;
}

uint32_t mdns_remoteservice_get_ipv4_address(mdns_remoteservice_obj_t *self) {
    return self->ipv4_address;
}

mp_obj_t common_hal_mdns_remoteservice_get_ipv4_address(mdns_remoteservice_obj_t *self) {
    uint32_t addr = mdns_remoteservice_get_ipv4_address(self);
    if (addr == 0) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(addr);
}

void common_hal_mdns_remoteservice_deinit(mdns_remoteservice_obj_t *self) {
}
