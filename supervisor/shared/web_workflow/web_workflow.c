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

#include <string.h>

#include "shared-bindings/wifi/Radio.h"
#include "supervisor/shared/translate/translate.h"
#include "supervisor/shared/web_workflow/web_workflow.h"

#include "shared-bindings/socketpool/__init__.h"
#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/socketpool/SocketPool.h"

#if CIRCUITPY_WIFI
#include "shared-bindings/wifi/__init__.h"
#endif

#if CIRCUITPY_DOTENV
#include "shared-module/dotenv/__init__.h"
#endif

#include "esp_log.h"

static const char *TAG = "CP webserver";

enum request_state {
    STATE_METHOD,
    STATE_PATH,
    STATE_VERSION,
    STATE_HEADER_KEY,
    STATE_HEADER_VALUE,
    STATE_BODY
};

typedef struct {
    enum request_state state;
    char method[6];
    char path[256];
    uint32_t content_length;
    size_t offset;
} _request;

static wifi_radio_error_t wifi_status = WIFI_RADIO_ERROR_NONE;

static socketpool_socketpool_obj_t pool;
static socketpool_socket_obj_t listening;
static socketpool_socket_obj_t active;

static _request active_request;


void supervisor_web_workflow_status(void) {
    serial_write_compressed(translate("Wi-Fi: "));
    if (common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj)) {
        uint32_t ipv4_address = wifi_radio_get_ipv4_address(&common_hal_wifi_radio_obj);
        if (wifi_status == WIFI_RADIO_ERROR_AUTH_EXPIRE ||
            wifi_status == WIFI_RADIO_ERROR_AUTH_FAIL) {
            serial_write_compressed(translate("Authentication failure"));
        } else if (wifi_status != WIFI_RADIO_ERROR_NONE) {
            mp_printf(&mp_plat_print, "%d", wifi_status);
        } else if (ipv4_address == 0) {
            serial_write_compressed(translate("No IP"));
        } else {
            uint8_t *octets = (uint8_t *)&ipv4_address;
            mp_printf(&mp_plat_print, "%d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
            // TODO: Use these unicode to show signal strength: ▂▄▆█
        }
    } else {
        serial_write_compressed(translate("off"));
    }
}

void supervisor_start_web_workflow(void) {
    #if CIRCUITPY_WEB_WORKFLOW && CIRCUITPY_WIFI

    if (common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj) &&
        wifi_radio_get_ipv4_address(&common_hal_wifi_radio_obj) != 0) {
        // Already started.
        return;
    }

    char ssid[33];
    char password[64];
    mp_int_t ssid_len = 0;
    mp_int_t password_len = 0;

    #if CIRCUITPY_DOTENV
    ssid_len = dotenv_get_key("/.env", "CIRCUITPY_WIFI_SSID", ssid, sizeof(ssid) - 1);
    password_len = dotenv_get_key("/.env", "CIRCUITPY_WIFI_PASSWORD", password, sizeof(password) - 1);
    #endif
    if (ssid_len <= 0 || (size_t)ssid_len >= sizeof(ssid) ||
        password_len <= 0 || (size_t)password_len >= sizeof(password)) {
        return;
    }
    common_hal_wifi_init(false);
    common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, true);

    // TODO: Do our own scan so that we can find the channel we want before calling connect.
    // Otherwise, connect will do a full slow scan to pick the best AP.

    // NUL terminate the strings because dotenv doesn't.
    ssid[ssid_len] = '\0';
    password[password_len] = '\0';
    wifi_status = common_hal_wifi_radio_connect(
        &common_hal_wifi_radio_obj, (uint8_t *)ssid, ssid_len, (uint8_t *)password, password_len,
        0, 0.1, NULL, 0);

    if (wifi_status != WIFI_RADIO_ERROR_NONE) {
        common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, false);
        return;
    }

    pool.base.type = &socketpool_socketpool_type;
    common_hal_socketpool_socketpool_construct(&pool, &common_hal_wifi_radio_obj);

    listening.base.type = &socketpool_socket_type;
    socketpool_socket(&pool, SOCKETPOOL_AF_INET, SOCKETPOOL_SOCK_STREAM, &listening);
    common_hal_socketpool_socket_settimeout(&listening, 0);
    // Bind to any ip.
    // TODO: Make this port .env configurable.
    const char *ip = "192.168.1.94";
    common_hal_socketpool_socket_bind(&listening, ip, strlen(ip), 80);
    common_hal_socketpool_socket_listen(&listening, 1);

    ESP_LOGW(TAG, "listening on socket %d", listening.num);

    active.base.type = &socketpool_socket_type;
    active.num = -1;
    active.connected = false;

    // Accept a connection and start parsing:
    // * HTTP method
    // * HTTP path
    // * Headers we care about:
    //   * Authentication
    //     * Must match CIRCUITPY_WEB_AUTH
    //   * Host
    //     * IP - ok
    //     * cpy-mac.local - ok
    //     * circuitpython.local - redirect
    //   * Content-Length
    //
    // PUT /fs/<filename>
    // GET /fs/<filename>
    //   - File contents
    //   - JSON directory representation
    // GET /cp/devices.json
    //   - JSON list of MDNS results
    // GET /cp/version.json
    //   - JSON version info
    // GET /
    //   - Super basic editor
    // GET /ws/circuitpython
    // GET /ws/user
    //   - WebSockets
    #endif
}

static void _process_request(socketpool_socket_obj_t *socket, _request *request) {
    bool more = true;
    bool error = false;
    uint8_t c;
    while (more && !error) {
        int len = socketpool_socket_recv_into(socket, &c, 1);
        if (len != 1) {
            if (len != -EAGAIN && len != -EBADF) {
                ESP_LOGW(TAG, "received %d", len);
            }
            more = false;
            break;
        }
        ESP_LOGI(TAG, "%c", c);
        switch (request->state) {
            case STATE_METHOD: {
                if (c == ' ') {
                    request->method[request->offset] = '\0';
                    request->offset = 0;
                    request->state = STATE_PATH;
                } else if (request->offset > sizeof(request->method) - 1) {
                    // Skip methods that are too long.
                } else {
                    request->method[request->offset] = c;
                    request->offset++;
                }
                break;
            }
            case STATE_PATH:  {
                if (c == ' ') {
                    request->path[request->offset] = '\0';
                    request->offset = 0;
                    ESP_LOGW(TAG, "Request %s %s", request->method, request->path);
                    request->state = STATE_VERSION;
                    error = true;
                } else if (request->offset > sizeof(request->path) - 1) {
                    // Skip methods that are too long.
                } else {
                    request->path[request->offset] = c;
                    request->offset++;
                }
                break;
            }
            case STATE_VERSION:
                break;
            case STATE_HEADER_KEY:
                break;
            case STATE_HEADER_VALUE:
                break;
            case STATE_BODY:
                break;
        }
    }
    if (error) {
        const char *error_response = "HTTP/1.1 501 Not Implemented";
        int nodelay = 1;
        int err = lwip_setsockopt(socket->num, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
        int sent = socketpool_socket_send(socket, (const uint8_t *)error_response, strlen(error_response));
        ESP_LOGW(TAG, "sent %d %d", sent, err);
        vTaskDelay(0);
        socketpool_socket_close(socket);
    }
}


void supervisor_web_workflow_background(void) {
    // Otherwise, see if we have another socket to accept.
    if (!common_hal_socketpool_socket_get_connected(&active) &&
        !common_hal_socketpool_socket_get_closed(&listening) &&
        listening.num > 0) {
        uint32_t ip;
        uint32_t port;
        int newsoc = socketpool_socket_accept(&listening, (uint8_t *)&ip, &port);
        if (newsoc != -11) {
            ESP_LOGI(TAG, "newsoc %d", newsoc);
        }
        if (newsoc > 0) {
            ESP_LOGE(TAG, "Accepted socket %d", newsoc);
            // TODO: Don't do this because it uses the private struct layout.
            // Create the socket
            active.num = newsoc;
            active.pool = &pool;
            active.connected = true;


            common_hal_socketpool_socket_settimeout(&active, 0);

            active_request.state = STATE_METHOD;
            active_request.offset = 0;

            lwip_fcntl(newsoc, F_SETFL, O_NONBLOCK);
        }
    }

    // If we have a request in progress, continue working on it.
    if (common_hal_socketpool_socket_get_connected(&active)) {
        _process_request(&active, &active_request);
    }
}

void supervisor_stop_web_workflow(void) {
}
