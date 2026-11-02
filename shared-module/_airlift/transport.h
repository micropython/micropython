// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Driver for an ESP32 co-processor running nina-fw, spoken to over SPI. The
// framing matches ESP32SPI's _send_command() and _wait_response_cmd(). Only one
// command is in flight at a time; the board SPI lock is held only during each
// half of a command, so other devices on the bus can use it in between.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "shared-bindings/busio/SPI.h"

// Number of sockets in nina-fw (CONFIG_LWIP_MAX_SOCKETS). A command naming a
// higher socket hangs nina-fw 3.1.0 until a hard reset.
#ifndef CIRCUITPY_AIRLIFT_MAX_SOCKETS
#define CIRCUITPY_AIRLIFT_MAX_SOCKETS (6)
#endif

// The ESP32 receives and sends at most this many bytes per SPI transfer.
// Longer commands are silently truncated, and longer replies hang it.
#define AIRLIFT_MAX_TRANSFER (4092)
// GET_DATABUF_TCP reply: START, cmd, 1 param, 16-bit length, data, END.
#define AIRLIFT_MAX_READ (AIRLIFT_MAX_TRANSFER - 6)
// SEND_DATA_TCP command: START, cmd, 2 params, 16-bit length + socket,
// 16-bit length + data, END.
#define AIRLIFT_MAX_SEND (AIRLIFT_MAX_TRANSFER - 9)

#define AIRLIFT_SET_NET (0x10)
#define AIRLIFT_SET_PASSPHRASE (0x11)
#define AIRLIFT_SET_AP_NET (0x18)
#define AIRLIFT_SET_AP_PASSPHRASE (0x19)
#define AIRLIFT_GET_CONN_STATUS (0x20)
#define AIRLIFT_GET_IPADDR (0x21)
#define AIRLIFT_GET_CURR_SSID (0x23)
#define AIRLIFT_START_SERVER_TCP (0x28)
#define AIRLIFT_AVAIL_DATA_TCP (0x2B)
#define AIRLIFT_START_CLIENT_TCP (0x2D)
#define AIRLIFT_STOP_CLIENT_TCP (0x2E)
#define AIRLIFT_GET_CLIENT_STATE_TCP (0x2F)
#define AIRLIFT_DISCONNECT (0x30)
#define AIRLIFT_GET_SOCKET (0x3F)
#define AIRLIFT_SEND_DATA_TCP (0x44)
#define AIRLIFT_GET_DATABUF_TCP (0x45)
#define AIRLIFT_SET_ENT_ENABLE (0x4F)

// The socket number GET_SOCKET returns when all are in use, and AVAIL_DATA_TCP
// returns for a listening socket with nobody waiting.
#define AIRLIFT_NO_SOCKET (255)

typedef struct {
    const uint8_t *buf;
    size_t len;
} airlift_param_t;

typedef struct {
    uint8_t *buf;       // The reply parameters, back to back.
    size_t buf_len;
    uint16_t *lens;     // Length of each reply parameter.
    size_t max_count;
    size_t count;       // Set by airlift_transact().
} airlift_reply_t;

typedef enum {
    AIRLIFT_OK,
    AIRLIFT_NOT_ACTIVE,
    // Another command is in flight, or the SPI bus is locked by someone else.
    AIRLIFT_BUSY,
    // A VM caller was interrupted before anything was sent.
    AIRLIFT_INTERRUPTED,
    // The command was rejected before sending it.
    AIRLIFT_BAD_SOCKET,
    AIRLIFT_TOO_BIG,
    // BUSY never went low.
    AIRLIFT_TIMEOUT_READY,
    // BUSY never went high after CS went low.
    AIRLIFT_TIMEOUT_SELECT,
    // No START_CMD at the start of the reply.
    AIRLIFT_TIMEOUT_START,
    // The ESP32 replied ERR_CMD.
    AIRLIFT_ERR_RESPONSE,
    // A framing byte was wrong.
    AIRLIFT_BAD_REPLY,
    // The reply didn't fit in the airlift_reply_t. It was read and dropped.
    AIRLIFT_REPLY_TOO_BIG,
} airlift_result_t;

// Claims the ESP pins and the board SPI bus, then hard-resets the ESP32.
// Returns false if a pin or the bus is already in use. Never raises.
bool airlift_transport_init(void);
// Releases the pins. The board SPI bus is left to board.SPI().
void airlift_transport_deinit(void);
// Hard reset through ESP_RESET. Takes about 760 ms. Changes the generation.
void airlift_transport_reset(void);
bool airlift_transport_active(void);
// True from the start of a command's send to the end of its reply.
bool airlift_transport_busy(void);
// False while someone else holds the SPI bus lock. Background callers don't
// wait for it.
bool airlift_transport_bus_free(void);
// Changes on every hard reset, so sockets can tell they no longer exist.
uint32_t airlift_transport_generation(void);
// Whether the board connects GPIO0, which nina-fw drives high while a socket
// has data to read or a listener has a client waiting.
bool airlift_transport_has_data_ready(void);
// The level of that signal; true when the board doesn't connect it. Safe to
// call from an interrupt.
bool airlift_transport_data_ready(void);
bool airlift_transport_using_spi(const busio_spi_obj_t *spi);
// At the end of a VM run, closes the sockets user code left open. nina-fw can't
// close a listening socket, so if user code started a server, the ESP32 is
// hard-reset instead.
void airlift_user_reset(void);

// Marks a socket number as the core's, so airlift_user_reset() leaves it open. A
// hard reset clears them all.
void airlift_transport_set_owned(uint8_t sock, bool owned);
// Records that the core left a listener running on this socket. nina-fw can't
// stop it, so the ESP32 is hard-reset at the end of the VM run.
void airlift_transport_leaked_listener(uint8_t sock);
// True while VM code has done GET_SOCKET but not yet used the number. GET_SOCKET
// doesn't reserve it, so the core must not take a socket in the meantime.
bool airlift_transport_vm_allocating(void);
// Counts the WiFi join and disconnect commands VM code has sent, so the core
// can tell whether user code is managing the connection itself.
uint32_t airlift_transport_vm_network_commands(void);

// Sends one command and reads its reply. expected_replies < 0 means take the
// count from the reply. from_vm callers wait for a locked bus and can be
// interrupted, but only before anything is sent. When the ESP32 stops answering
// the core's commands, or garbles three replies in a row, it is hard-reset.
airlift_result_t airlift_transact(uint8_t cmd, const airlift_param_t *params, size_t n_params,
    bool param_len_16, airlift_reply_t *reply, int expected_replies, bool reply_len_16,
    bool from_vm);
