// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "py/obj.h"
#include "py/objlist.h"

#include "common-hal/_bleio/__init__.h"
#include "shared-module/_bleio/Address.h"
#include "common-hal/_bleio/Service.h"

typedef enum {
    PAIR_NOT_PAIRED,
    PAIR_WAITING,
    PAIR_PAIRED,
} pair_status_t;

// We split the Connection object into two so that the internal mechanics can live outside of the
// VM. If it were one object, then we'd risk user code seeing a connection object of theirs be
// reused.
typedef struct {
    uint16_t conn_handle;
    bool is_central;
    // Remote services discovered when this peripheral is acting as a client.
    mp_obj_list_t *remote_service_list;
    // The advertising data and scan response buffers are held by us, not by the SD, so we must
    // maintain them and not change it. If we need to change the contents during advertising,
    // there are tricks to get the SD to notice (see DevZone - TBS).
    bonding_keys_t bonding_keys;
    // EDIV: Encrypted Diversifier: Identifies LTK during legacy pairing.
    uint16_t ediv;
    volatile pair_status_t pair_status;
    uint8_t sec_status; // Internal security status.
    mp_obj_t connection_obj;
    // REMOVE ble_gap_conn_params_t conn_params;
    volatile bool conn_params_updating;
    uint16_t mtu;
    // Request that CCCD values for this connection be saved, using sys_attr values.
    volatile bool do_bond_cccds;
    // Request that security key info for this connection be saved.
    volatile bool do_bond_keys;
    // Time of setting do_bond_ccds: we delay a bit to consolidate multiple CCCD changes
    // into one write. Time is currently in ticks_ms.
    uint64_t do_bond_cccds_request_time;
    // FIX from att.c
    uint8_t role;
    bt_addr_le_t addr;
} bleio_connection_internal_t;

typedef struct {
    mp_obj_base_t base;
    bleio_connection_internal_t *connection;
    // The HCI disconnect reason.
    uint8_t disconnect_reason;
} bleio_connection_obj_t;

uint16_t bleio_connection_get_conn_handle(bleio_connection_obj_t *self);
mp_obj_t bleio_connection_new_from_internal(bleio_connection_internal_t *connection);
bleio_connection_internal_t *bleio_conn_handle_to_connection(uint16_t conn_handle);
void bleio_connection_clear(bleio_connection_internal_t *self);
