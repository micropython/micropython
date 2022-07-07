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

static _websocket cp_serial;

static const char *TAG = "CP websocket";

void websocket_init(void) {
    cp_serial.socket.num = -1;
    cp_serial.socket.connected = false;
}

void websocket_handoff(socketpool_socket_obj_t *socket) {
    ESP_LOGI(TAG, "socket handed off");
    cp_serial.socket = *socket;
    // Mark the original socket object as closed without telling the lower level.
    socket->connected = false;
    socket->num = -1;
    ESP_LOGI(TAG, "socket hand off done");
}

bool websocket_connected(void) {
    return common_hal_socketpool_socket_get_connected(&cp_serial.socket);
}

static bool _read_byte(uint8_t *c) {
    int len = socketpool_socket_recv_into(&cp_serial.socket, c, 1);
    if (len != 1) {
        if (len != -EAGAIN) {
            ESP_LOGE(TAG, "recv error %d", len);
        }
        return false;
    }
    return true;
}

static void _read_next_frame_header(void) {
    uint8_t h;
    if (cp_serial.frame_index == 0 && _read_byte(&h)) {
        cp_serial.frame_index++;
        cp_serial.opcode = h & 0xf;
        ESP_LOGI(TAG, "fin %d opcode %x", h >> 7, cp_serial.opcode);
    }
    if (cp_serial.frame_index == 1 && _read_byte(&h)) {
        cp_serial.frame_index++;
        uint8_t len = h & 0xf;
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

        ESP_LOGI(TAG, "mask %d length %x", cp_serial.masked, len);
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
        ESP_LOGI(TAG, "mask %08x", (uint32_t)*cp_serial.mask);
    }
}

static bool _read_next_payload_byte(uint8_t *c) {
    _read_next_frame_header();
    if (cp_serial.frame_index >= cp_serial.frame_len &&
        cp_serial.payload_remaining > 0) {
        if (_read_byte(c)) {
            uint8_t mask_offset = (cp_serial.frame_index - cp_serial.frame_len) % 4;
            ESP_LOGI(TAG, "payload byte read %02x mask offset %d", *c, mask_offset);
            *c ^= cp_serial.mask[mask_offset];
            ESP_LOGI(TAG, "byte unmasked %02x", *c);
            cp_serial.frame_index++;
            cp_serial.payload_remaining--;
            ESP_LOGI(TAG, "payload remaining %d", cp_serial.payload_remaining);
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
    _read_next_frame_header();
    return cp_serial.payload_remaining > 0 && cp_serial.frame_index >= cp_serial.frame_len;
}

char websocket_read_char(void) {
    uint8_t c;
    _read_next_payload_byte(&c);
    return c;
}

static void _send_raw(socketpool_socket_obj_t *socket, const uint8_t *buf, int len) {
    int sent = -EAGAIN;
    while (sent == -EAGAIN) {
        sent = socketpool_socket_send(socket, buf, len);
    }
    if (sent < len) {
        ESP_LOGE(TAG, "short send %d %d", sent, len);
    }
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
    _send_raw(&ws->socket, (const uint8_t *)frame_header, 2);
    if (payload_len == 126) {
        _send_raw(&ws->socket, (const uint8_t *)&len, 2);
    } else if (payload_len == 127) {
        uint32_t zero = 0;
        // 64 bits where top four bytes are zero.
        _send_raw(&ws->socket, (const uint8_t *)&zero, 4);
        _send_raw(&ws->socket, (const uint8_t *)&len, 4);
    }
    _send_raw(&ws->socket, (const uint8_t *)text, len);
    char copy[len];
    memcpy(copy, text, len);
    copy[len] = '\0';
    ESP_LOGI(TAG, "sent over websocket: %s", copy);
}

void websocket_write(const char *text, size_t len) {
    _websocket_send(&cp_serial, text, len);
}
