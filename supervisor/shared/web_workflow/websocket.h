// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "shared-bindings/socketpool/Socket.h"

void websocket_init(void);
void websocket_handoff(socketpool_socket_obj_t *socket);
bool websocket_connected(void);
uint32_t websocket_available(void);
char websocket_read_char(void);
void websocket_background(void);
void websocket_write(const char *text, size_t len);
