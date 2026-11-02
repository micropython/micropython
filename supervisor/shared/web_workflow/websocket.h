// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "shared-bindings/socketpool/Socket.h"

typedef struct {
    socketpool_socket_obj_t socket;
    uint8_t opcode;
    uint8_t frame_len;
    uint8_t payload_len_size;
    bool masked;
    uint8_t mask[4];
    int frame_index;
    size_t payload_remaining;
} websocket_t;

// Close code sent to a client when a newer one takes over its connection.
#define WEBSOCKET_CLOSE_REPLACED (4000)

// Sends a CLOSE frame with code if the socket can take it without waiting. The
// caller closes the socket.
void websocket_send_close(websocket_t *ws, uint16_t code);
// Moves an upgraded socket into ws, closing the one it had. A connected client
// is sent WEBSOCKET_CLOSE_REPLACED first.
void websocket_take_socket(websocket_t *ws, socketpool_socket_obj_t *socket);
// Reads the next byte of a text or binary message. PINGs are answered and a CLOSE closes
// the socket. Returns false when no byte is available yet.
bool websocket_read_payload_byte(websocket_t *ws, uint8_t *c);

void websocket_init(void);
void websocket_handoff(socketpool_socket_obj_t *socket);
bool websocket_connected(void);
uint32_t websocket_available(void);
char websocket_read_char(void);
void websocket_background(void);
void websocket_write(const char *text, size_t len);
