// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// socketpool common-hal on an ESP32 co-processor running nina-fw. Ports include
// this from common-hal/socketpool/{Socket,SocketPool}.h when CIRCUITPY_AIRLIFT is on.
// Only TCP is supported, and only the core uses it for now (web workflow):
// SocketPool() accepts only airlift_socketpool_interface, which Python can't reach.

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/_airlift/transport.h"

// Bytes read from the ESP32 at a time. Web workflow reads headers a byte at a time.
#ifndef AIRLIFT_SOCKET_RX_SIZE
#define AIRLIFT_SOCKET_RX_SIZE (512)
#endif
// Small sends are collected here and sent together when full, before a read,
// on close, and at the end of each web workflow background pass.
#ifndef AIRLIFT_SOCKET_TX_SIZE
#define AIRLIFT_SOCKET_TX_SIZE (2048)
#endif

typedef struct {
    mp_obj_base_t base;
} socketpool_socketpool_obj_t;

typedef struct {
    mp_obj_base_t base;
    socketpool_socketpool_obj_t *pool;
    // When `connected` was last read from the ESP32.
    uint64_t connected_ms;
    // Don't ask the ESP32 for data again before this, after it last had none.
    uint64_t next_avail_ms;
    // airlift_transport_generation() when the socket was opened. After a hard
    // reset the socket no longer exists.
    uint32_t generation;
    uint32_t timeout_ms;
    uint16_t port;
    uint16_t rx_head;
    uint16_t rx_len;
    uint16_t tx_len;
    // The ESP32's socket number, or AIRLIFT_NO_SOCKET when closed.
    uint8_t num;
    uint8_t type;
    bool listening;
    bool connected;
    bool connected_valid;
    uint8_t rx[AIRLIFT_SOCKET_RX_SIZE];
    uint8_t tx[AIRLIFT_SOCKET_TX_SIZE];
} socketpool_socket_obj_t;

// The network interface object the core passes to SocketPool's constructor.
extern const mp_obj_base_t airlift_socketpool_interface;

// AirLift has no callbacks, so web workflow is polled: this schedules its next
// background pass, sooner while a client is connected.
void airlift_socketpool_poll_resume(void);
#define socketpool_socket_poll_resume() airlift_socketpool_poll_resume()
// Called from supervisor_tick() every millisecond.
void airlift_socketpool_tick(void);
// Called at the start of each web workflow background pass. Refreshes the WiFi
// status, sends what's waiting in the TX buffers, and returns false if the ESP32
// can't be used right now, so the pass should be skipped.
bool airlift_socketpool_background(void);
// Called at the end of each web workflow background pass. Sends what's waiting
// in the TX buffers.
void airlift_socketpool_background_done(void);
