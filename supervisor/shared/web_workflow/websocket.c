// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/web_workflow/websocket.h"

#include "py/ringbuf.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "supervisor/shared/web_workflow/web_workflow.h"

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

// Buffer the incoming serial data in the background so that we can look for the
// interrupt character.
static ringbuf_t _incoming_ringbuf;
static uint8_t _buf[16];
// make sure background is not called recursively
static bool in_web_background = false;

static websocket_t cp_serial;

void websocket_init(void) {
    socketpool_socket_reset(&cp_serial.socket);

    ringbuf_init(&_incoming_ringbuf, _buf, sizeof(_buf));
}

void websocket_send_close(websocket_t *ws, uint16_t code) {
    if (common_hal_socketpool_socket_get_closed(&ws->socket) ||
        !common_hal_socketpool_socket_get_connected(&ws->socket)) {
        return;
    }
    const uint8_t frame[4] = { 1 << 7 | 0x8, 2, code >> 8, code & 0xff };
    // Only what the socket takes now: a client that stopped reading must not
    // hold everything up.
    socketpool_socket_send(&ws->socket, frame, sizeof(frame));
}

void websocket_take_socket(websocket_t *ws, socketpool_socket_obj_t *socket) {
    if (!common_hal_socketpool_socket_get_closed(&ws->socket)) {
        // Tell the page it was replaced, so it doesn't reconnect and take the
        // connection back.
        websocket_send_close(ws, WEBSOCKET_CLOSE_REPLACED);
        common_hal_socketpool_socket_close(&ws->socket);
    }

    socketpool_socket_move(socket, &ws->socket);
    ws->opcode = 0;
    ws->frame_index = 0;
    ws->frame_len = 2;
}

void websocket_handoff(socketpool_socket_obj_t *socket) {
    websocket_take_socket(&cp_serial, socket);

    #if CIRCUITPY_STATUS_BAR
    // Send the title bar for the new client.
    supervisor_status_bar_request_update(true);
    #endif
}

bool websocket_connected(void) {
    return _incoming_ringbuf.size > 0 &&
           !common_hal_socketpool_socket_get_closed(&cp_serial.socket) &&
           common_hal_socketpool_socket_get_connected(&cp_serial.socket);
}

static bool _read_byte(websocket_t *ws, uint8_t *c) {
    int len = socketpool_socket_recv_into(&ws->socket, c, 1);
    if (len < 1) {
        return false;
    }
    return true;
}

static void _read_next_frame_header(websocket_t *ws) {
    uint8_t h = 0;
    if (ws->frame_index == 0 && _read_byte(ws, &h)) {
        ws->frame_index++;
        ws->opcode = h & 0xf;
    }
    if (ws->frame_index == 1 && _read_byte(ws, &h)) {
        ws->frame_index++;
        uint8_t len = h & 0x7f;
        ws->masked = (h >> 7) == 1;
        if (len <= 125) {
            ws->payload_remaining = len;
            ws->payload_len_size = 0;
        } else if (len == 126) { // 16 bit length
            ws->payload_len_size = 2;
        } else if (len == 127) { // 64 bit length
            ws->payload_len_size = 8;
        }
        ws->frame_len = 2 + ws->payload_len_size;
        if (ws->masked) {
            ws->frame_len += 4;
        }
    }
    while (ws->frame_index >= 2 &&
           ws->frame_index < (ws->payload_len_size + 2) &&
           _read_byte(ws, &h)) {
        ws->frame_index++;
        ws->payload_remaining = ws->payload_remaining << 8 | h;
    }
    int mask_start = ws->payload_len_size + 2;
    while (ws->frame_index >= mask_start &&
           ws->frame_index < ws->frame_len &&
           _read_byte(ws, &h)) {
        size_t mask_offset = ws->frame_index - mask_start;
        ws->mask[mask_offset] = h;
        ws->frame_index++;
    }
    // Reply to PINGs and CLOSE. Replying can find the client gone, which closes
    // the socket and drops the payload still to be read.
    while ((ws->opcode == 0x8 ||
            ws->opcode == 0x9) &&
           ws->frame_index >= ws->frame_len &&
           !common_hal_socketpool_socket_get_closed(&ws->socket)) {

        if (ws->frame_index == ws->frame_len) {
            uint8_t opcode = 0x8; // CLOSE
            if (ws->opcode == 0x9) {
                opcode = 0xA; // PONG
            }
            uint8_t frame_header[2];
            frame_header[0] = 1 << 7 | opcode;
            frame_header[1] = ws->payload_remaining;
            web_workflow_send_raw(&ws->socket, ws->opcode != 0x9, (const uint8_t *)frame_header, 2);
        }

        if (ws->payload_remaining > 0 && _read_byte(ws, &h)) {
            // Send the payload back to the client.
            ws->frame_index++;
            ws->payload_remaining--;
            web_workflow_send_raw(&ws->socket, false, &h, 1);
        }

        if (ws->payload_remaining == 0) {
            ws->frame_index = 0;
            if (ws->opcode == 0x8) {
                common_hal_socketpool_socket_close(&ws->socket);
            }
        }
    }
}

bool websocket_read_payload_byte(websocket_t *ws, uint8_t *c) {
    _read_next_frame_header(ws);
    // Text, binary or a continuation of either.
    if (ws->opcode <= 0x2 &&
        ws->frame_index >= ws->frame_len &&
        ws->payload_remaining > 0) {
        if (_read_byte(ws, c)) {
            uint8_t mask_offset = (ws->frame_index - ws->frame_len) % 4;
            *c ^= ws->mask[mask_offset];
            ws->frame_index++;
            ws->payload_remaining--;
            if (ws->payload_remaining == 0) {
                ws->frame_index = 0;
            }
            return true;
        }
    }
    return false;
}

uint32_t websocket_available(void) {
    if (!websocket_connected()) {
        return 0;
    }
    websocket_background();
    return ringbuf_num_filled(&_incoming_ringbuf);
}

char websocket_read_char(void) {
    websocket_background();
    if (ringbuf_num_filled(&_incoming_ringbuf) > 0) {
        return ringbuf_get(&_incoming_ringbuf);
    }
    return -1;
}

static void _websocket_send(websocket_t *ws, const char *text, size_t len) {
    if (!websocket_connected()) {
        return;
    }
    uint32_t opcode = 1;
    uint8_t frame_header[2];
    frame_header[0] = 1 << 7 | opcode;
    uint8_t payload_len;
    if (len <= 125) {
        payload_len = len;
    } else if (len < (1 << 16)) {
        payload_len = 126;
    } else {
        payload_len = 127;
    }
    frame_header[1] = payload_len;
    web_workflow_send_raw(&ws->socket, false, (const uint8_t *)frame_header, 2);
    uint8_t extended_len[4];
    if (payload_len == 126) {
        extended_len[0] = (len >> 8) & 0xff;
        extended_len[1] = len & 0xff;
        web_workflow_send_raw(&ws->socket, false, extended_len, 2);
    } else if (payload_len == 127) {
        uint32_t zero = 0;
        // 64 bits where top four bytes are zero.
        web_workflow_send_raw(&ws->socket, false, (const uint8_t *)&zero, 4);
        extended_len[0] = (len >> 24) & 0xff;
        extended_len[1] = (len >> 16) & 0xff;
        extended_len[2] = (len >> 8) & 0xff;
        extended_len[3] = len & 0xff;
        web_workflow_send_raw(&ws->socket, false, extended_len, 4);
    }
    web_workflow_send_raw(&ws->socket, false, (const uint8_t *)text, len);
}

void websocket_write(const char *text, size_t len) {
    _websocket_send(&cp_serial, text, len);
}

void websocket_background(void) {
    if (!websocket_connected()) {
        return;
    }
    if (in_web_background) {
        return;
    }
    in_web_background = true;
    uint8_t c;
    while (ringbuf_num_empty(&_incoming_ringbuf) > 0 &&
           websocket_read_payload_byte(&cp_serial, &c)) {
        if (c == mp_interrupt_char) {
            ringbuf_clear(&_incoming_ringbuf);
            mp_sched_keyboard_interrupt();
            continue;
        }
        ringbuf_put(&_incoming_ringbuf, c);
    }
    in_web_background = false;
}
