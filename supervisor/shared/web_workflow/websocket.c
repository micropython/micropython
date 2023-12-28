/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "supervisor/shared/web_workflow/websocket.h"

#include "py/ringbuf.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "supervisor/shared/web_workflow/web_workflow.h"

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

typedef struct {
    socketpool_socket_obj_t socket;
    uint8_t opcode;
    uint8_t frame_len;
    uint8_t payload_len_size;
    bool masked;
    uint8_t mask[4];
    int frame_index;
    size_t payload_remaining;
} _websocket;

// Buffer the incoming serial data in the background so that we can look for the
// interrupt character.
STATIC ringbuf_t _incoming_ringbuf;
STATIC uint8_t _buf[16];
// make sure background is not called recursively
STATIC bool in_web_background = false;

static _websocket cp_serial;

void websocket_init(void) {
    socketpool_socket_reset(&cp_serial.socket);

    ringbuf_init(&_incoming_ringbuf, _buf, sizeof(_buf));
}

void websocket_handoff(socketpool_socket_obj_t *socket) {
    if (!common_hal_socketpool_socket_get_closed(&cp_serial.socket)) {
        common_hal_socketpool_socket_close(&cp_serial.socket);
    }

    socketpool_socket_move(socket, &cp_serial.socket);
    cp_serial.opcode = 0;
    cp_serial.frame_index = 0;
    cp_serial.frame_len = 2;

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

static bool _read_byte(uint8_t *c) {
    int len = socketpool_socket_recv_into(&cp_serial.socket, c, 1);
    if (len < 1) {
        return false;
    }
    return true;
}

static void _read_next_frame_header(void) {
    uint8_t h;
    if (cp_serial.frame_index == 0 && _read_byte(&h)) {
        cp_serial.frame_index++;
        cp_serial.opcode = h & 0xf;
    }
    if (cp_serial.frame_index == 1 && _read_byte(&h)) {
        cp_serial.frame_index++;
        uint8_t len = h & 0x7f;
        cp_serial.masked = (h >> 7) == 1;
        if (len <= 125) {
            cp_serial.payload_remaining = len;
            cp_serial.payload_len_size = 0;
        } else if (len == 126) { // 16 bit length
            cp_serial.payload_len_size = 2;
        } else if (len == 127) { // 64 bit length
            cp_serial.payload_len_size = 8;
        }
        cp_serial.frame_len = 2 + cp_serial.payload_len_size;
        if (cp_serial.masked) {
            cp_serial.frame_len += 4;
        }
    }
    while (cp_serial.frame_index >= 2 &&
           cp_serial.frame_index < (cp_serial.payload_len_size + 2) &&
           _read_byte(&h)) {
        cp_serial.frame_index++;
        cp_serial.payload_remaining = cp_serial.payload_remaining << 8 | h;
    }
    int mask_start = cp_serial.payload_len_size + 2;
    while (cp_serial.frame_index >= mask_start &&
           cp_serial.frame_index < cp_serial.frame_len &&
           _read_byte(&h)) {
        size_t mask_offset = cp_serial.frame_index - mask_start;
        cp_serial.mask[mask_offset] = h;
        cp_serial.frame_index++;
    }
    // Reply to PINGs and CLOSE.
    while ((cp_serial.opcode == 0x8 ||
            cp_serial.opcode == 0x9) &&
           cp_serial.frame_index >= cp_serial.frame_len) {

        if (cp_serial.frame_index == cp_serial.frame_len) {
            uint8_t opcode = 0x8; // CLOSE
            if (cp_serial.opcode == 0x9) {
                opcode = 0xA; // PONG
            }
            uint8_t frame_header[2];
            frame_header[0] = 1 << 7 | opcode;
            frame_header[1] = cp_serial.payload_remaining;
            web_workflow_send_raw(&cp_serial.socket, cp_serial.opcode != 0x9, (const uint8_t *)frame_header, 2);
        }

        if (cp_serial.payload_remaining > 0 && _read_byte(&h)) {
            // Send the payload back to the client.
            cp_serial.frame_index++;
            cp_serial.payload_remaining--;
            web_workflow_send_raw(&cp_serial.socket, false, &h, 1);
        }

        if (cp_serial.payload_remaining == 0) {
            cp_serial.frame_index = 0;
            if (cp_serial.opcode == 0x8) {
                common_hal_socketpool_socket_close(&cp_serial.socket);
            }
        }
    }
}

static bool _read_next_payload_byte(uint8_t *c) {
    _read_next_frame_header();
    if (cp_serial.opcode == 0x1 &&
        cp_serial.frame_index >= cp_serial.frame_len &&
        cp_serial.payload_remaining > 0) {
        if (_read_byte(c)) {
            uint8_t mask_offset = (cp_serial.frame_index - cp_serial.frame_len) % 4;
            *c ^= cp_serial.mask[mask_offset];
            cp_serial.frame_index++;
            cp_serial.payload_remaining--;
            if (cp_serial.payload_remaining == 0) {
                cp_serial.frame_index = 0;
            }
            return true;
        }
    }
    return false;
}

bool websocket_available(void) {
    if (!websocket_connected()) {
        return false;
    }
    websocket_background();
    return ringbuf_num_filled(&_incoming_ringbuf) > 0;
}

char websocket_read_char(void) {
    websocket_background();
    if (ringbuf_num_filled(&_incoming_ringbuf) > 0) {
        return ringbuf_get(&_incoming_ringbuf);
    }
    return -1;
}

static void _websocket_send(_websocket *ws, const char *text, size_t len) {
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
           _read_next_payload_byte(&c)) {
        if (c == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
            continue;
        }
        ringbuf_put(&_incoming_ringbuf, c);
    }
    in_web_background = false;
}
