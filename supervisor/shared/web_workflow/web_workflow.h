// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "shared-bindings/mdns/Server.h"
#include "shared-bindings/socketpool/Socket.h"

// This background function should be called repeatedly. It cannot be done based
// on events.
void supervisor_web_workflow_background(void *data);
bool supervisor_web_workflow_status_dirty(void);
void supervisor_web_workflow_status(void);
bool supervisor_start_web_workflow(void);
void supervisor_stop_web_workflow(void);

// Share the MDNS object with user code.
mdns_server_obj_t *supervisor_web_workflow_mdns(mp_obj_t network_interface);

// To share with websocket.
void web_workflow_send_raw(socketpool_socket_obj_t *socket, bool flush, const uint8_t *buf, int len);
