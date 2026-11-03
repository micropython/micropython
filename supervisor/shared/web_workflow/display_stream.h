// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mpconfig.h"

#if CIRCUITPY_WEB_WORKFLOW && CIRCUITPY_DISPLAYIO

#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "shared-module/displayio/area.h"
#include "shared-module/displayio/display_core.h"

// Serves the first display with the Remote Framebuffer protocol (RFB 3.8,
// RFC 6143), so VNC viewers can show it. Framebuffer displays are read from
// their framebuffer. Bus displays have none, so changed areas are rendered
// again from the display's group.
//
// It is reached two ways:
// - a websocket at /cp/display/ carrying RFB in binary messages, as noVNC and
//   the /cp/display/ page expect. The HTTP request was already authenticated,
//   so it offers the None security type.
// - plain TCP on CIRCUITPY_WEB_VNC_PORT for VNC viewers. It is off unless
//   settings.toml sets the port. It has no password, offering only the None
//   security type, so anyone who can reach it can see the display. Keys are
//   ignored there because the console can run code, unless settings.toml sets
//   CIRCUITPY_WEB_VNC_KEYS to true. Viewers that insist on a password can't use
//   it. Each listener takes one of the six sockets AirLift's co-processor has,
//   and can't be closed.
//
// The encodings are Raw and Hextile, which needs no compressor and little code.
// DesktopSize tells the viewer when the display changes size.
// Keys typed into a websocket viewer go to the serial console.
//
// There is one viewer at a time: a new one takes over. A replaced websocket
// viewer is closed with code 4000 (WEBSOCKET_CLOSE_REPLACED).

void display_stream_init(void);
// Sets the VNC port, 0 for none, and whether keys typed there reach the console.
void display_stream_set_vnc(mp_int_t port, bool allow_keys);
// Opens the VNC port if there is one and it isn't open.
void display_stream_listen(socketpool_socketpool_obj_t *pool);
// Takes a connected socket: a websocket upgrade request for /cp/display/, or a
// connection to the VNC port.
void display_stream_handoff(socketpool_socket_obj_t *socket, bool websocket);
void display_stream_background(void);
// Called every tick while the stream has the tick enabled.
void display_stream_tick(void);
// Called by a display after it refreshed area.
void display_stream_area_updated(displayio_display_core_t *core, const displayio_area_t *area);

// Serial console input typed into the viewer.
bool display_stream_connected(void);
uint32_t display_stream_available(void);
char display_stream_read_char(void);

#endif
