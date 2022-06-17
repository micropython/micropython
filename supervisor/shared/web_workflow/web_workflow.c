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

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "genhdr/mpversion.h"
#include "py/mpstate.h"

#include "shared-bindings/wifi/Radio.h"
#include "supervisor/shared/translate/translate.h"
#include "supervisor/shared/web_workflow/web_workflow.h"

#include "shared-bindings/mdns/RemoteService.h"
#include "shared-bindings/mdns/Server.h"
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
    char header_key[64];
    char header_value[64];
    uint32_t content_length;
    size_t offset;
    bool redirect;
    bool done;
    bool authenticated;
} _request;

static wifi_radio_error_t wifi_status = WIFI_RADIO_ERROR_NONE;

static mdns_server_obj_t mdns;

static socketpool_socketpool_obj_t pool;
static socketpool_socket_obj_t listening;
static socketpool_socket_obj_t active;

static _request active_request;

static char _api_password[64];

// in_len is the number of bytes to encode. out_len is the number of bytes we
// have to do it.
static bool _base64_in_place(char *buf, size_t in_len, size_t out_len) {
    size_t triples = (((in_len - 1) / 3) + 1);
    size_t encoded_len = triples * 4;
    if (encoded_len + 1 > out_len) {
        return false;
    }
    ESP_LOGI(TAG, "triples %d", triples);

    // First pass, we convert input buffer to numeric base 64 values
    char *in = buf + (triples - 1) * 3;
    char *out = buf + (triples - 1) * 4;
    int r = in_len % 3;
    int partial = 0;
    if (r != 0) {
        out[0] = (in[0] & 0xFC) >> 2;
        out[1] = (in[0] & 0x03) << 4;
        if (r == 2) {
            out[1] |= (in[1] & 0xF0) >> 4;
            out[2] = (in[1] & 0x0F) << 2;
        } else {
            out[2] = 64;
        }
        out[3] = 64;
        in -= 3;
        out -= 4;
        partial = 1;
    } else {
        ESP_LOGI(TAG, "no partial");
    }
    buf[encoded_len] = '\0';
    for (size_t i = 0; i < triples - partial; i++) {
        ESP_LOGI(TAG, "in %d %d %d", in[0], in[1], in[2]);
        out[0] = (in[0] & 0xFC) >> 2;
        out[1] = (in[0] & 0x03) << 4 | (in[1] & 0xF0) >> 4;
        out[2] = (in[1] & 0x0F) << 2 | (in[2] & 0xC0) >> 6;
        out[3] = in[2] & 0x3F;
        ESP_LOGI(TAG, "out %d %d %d %d", out[0], out[1], out[2], out[3]);
        in -= 3;
        out -= 4;
    }

    // Second pass, we convert number base 64 values to actual base64 ascii encoding
    out = buf;
    for (mp_uint_t j = 0; j < encoded_len; j++) {
        if (*out < 26) {
            *out += 'A';
        } else if (*out < 52) {
            *out += 'a' - 26;
        } else if (*out < 62) {
            *out += '0' - 52;
        } else if (*out == 62) {
            *out = '+';
        } else if (*out == 63) {
            *out = '/';
        } else {
            *out = '=';
        }
        out++;
    }
    return true;
}


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

    mdns_server_construct(&mdns, true);
    common_hal_mdns_server_set_instance_name(&mdns, MICROPY_HW_BOARD_NAME);
    common_hal_mdns_server_advertise_service(&mdns, "_circuitpython", "_tcp", 80);

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


    mp_int_t api_password_len = dotenv_get_key("/.env", "CIRCUITPY_WEB_API_PASSWORD", _api_password + 1, sizeof(_api_password) - 2);
    if (api_password_len > 0) {
        _api_password[0] = ':';
        _api_password[api_password_len + 1] = '\0';
        ESP_LOGW(TAG, "password before: %s", _api_password);
        _base64_in_place(_api_password, api_password_len + 1, sizeof(_api_password));
        ESP_LOGW(TAG, "password encoded: %s", _api_password);
    }

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
    // GET /cp/serial.txt
    //   - Most recent 1k of serial output.
    // GET /
    //   - Super basic editor
    // GET /ws/circuitpython
    // GET /ws/user
    //   - WebSockets
    #endif
}

static void _send_chunk(socketpool_socket_obj_t *socket, const char *chunk) {
    char encoded_len[sizeof(size_t) * 2 + 1];
    int len = snprintf(encoded_len, sizeof(encoded_len), "%X", strlen(chunk));
    int sent = socketpool_socket_send(socket, (const uint8_t *)encoded_len, len);
    if (sent < len) {
        ESP_LOGE(TAG, "short send %d %d", sent, len);
    }
    sent = socketpool_socket_send(socket, (const uint8_t *)"\r\n", 2);
    if (sent < 2) {
        ESP_LOGE(TAG, "short send %d %d", sent, 2);
    }
    sent = socketpool_socket_send(socket, (const uint8_t *)chunk, strlen(chunk));
    if (sent < (int)strlen(chunk)) {
        ESP_LOGE(TAG, "short send %d %d", sent, strlen(chunk));
    }
    sent = socketpool_socket_send(socket, (const uint8_t *)"\r\n", 2);
    if (sent < 2) {
        ESP_LOGE(TAG, "short send %d %d", sent, 2);
    }
}

static void _send_str(socketpool_socket_obj_t *socket, const char *str) {
    int sent = socketpool_socket_send(socket, (const uint8_t *)str, strlen(str));
    if (sent < (int)strlen(str)) {
        ESP_LOGE(TAG, "short send %d %d", sent, strlen(str));
    }
}

static bool _endswith(const char *str, const char *suffix) {
    if (str == NULL || suffix == NULL) {
        return false;
    }
    if (strlen(suffix) > strlen(str)) {
        return false;
    }
    return strcmp(str + (strlen(str) - strlen(suffix)), suffix) == 0;
}

static void _reply_missing(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 401 Unauthorized\r\n"
        "Content-Length: 0\r\n"
        "WWW-Authenticate: Basic realm=\"CircuitPython\"\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_forbidden(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 403 Forbidden\r\n"
        "Content-Length: 0\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_unauthorized(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 401 Unauthorized\r\n"
        "Content-Length: 0\r\n"
        "WWW-Authenticate: Basic realm=\"CircuitPython\"\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_redirect(socketpool_socket_obj_t *socket, const char *path) {
    const char *redirect_response = "HTTP/1.1 301 Moved Permanently\r\nConnection: close\r\nContent-Length: 0\r\nLocation: http://";
    int nodelay = 1;
    lwip_setsockopt(socket->num, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
    _send_str(socket, redirect_response);
    _send_str(socket, common_hal_mdns_server_get_instance_name(&mdns));
    _send_str(socket, ".local");
    _send_str(socket, path);
    _send_str(socket, "\r\n\r\n");
}

static void _process_request(socketpool_socket_obj_t *socket, _request *request) {
    bool more = true;
    bool error = false;
    uint8_t c;
    // This code assumes header lines are terminated with \r\n
    while (more && !error) {
        int len = socketpool_socket_recv_into(socket, &c, 1);
        if (len != 1) {
            if (len != -EAGAIN && len != -EBADF) {
                ESP_LOGW(TAG, "received %d", len);
            }
            more = false;
            break;
        }
        // ESP_LOGI(TAG, "%c", c);
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
                } else if (request->offset > sizeof(request->path) - 1) {
                    // Skip methods that are too long.
                } else {
                    request->path[request->offset] = c;
                    request->offset++;
                }
                break;
            }
            case STATE_VERSION: {
                const char *supported_version = "HTTP/1.1\r";
                error = supported_version[request->offset] != c;
                request->offset++;
                if (request->offset == strlen(supported_version)) {
                    ESP_LOGW(TAG, "Version ok");
                    request->state = STATE_HEADER_KEY;
                    request->offset = 0;
                }
                break;
            }
            case STATE_HEADER_KEY: {
                if (c == '\r') {
                    request->state = STATE_BODY;
                    ESP_LOGW(TAG, "Body");
                } else if (c == '\n') {
                    // Consume the \n
                } else if (c == ':') {
                    request->header_key[request->offset] = '\0';
                    request->offset = 0;
                    request->state = STATE_HEADER_VALUE;
                } else if (request->offset > sizeof(request->header_key) - 1) {
                    // Skip methods that are too long.
                } else {
                    request->header_key[request->offset] = c;
                    request->offset++;
                }
                break;
            }
            case STATE_HEADER_VALUE: {
                if (request->offset == 0) {
                    error = c != ' ';
                    request->offset++;
                } else if (c == '\r') {
                    request->header_value[request->offset - 1] = '\0';
                    request->offset = 0;
                    request->state = STATE_HEADER_KEY;
                    if (strcmp(request->header_key, "Authorization") == 0) {
                        ESP_LOGW(TAG, "Authorization");
                    } else if (strcmp(request->header_key, "Host") == 0) {
                        ESP_LOGW(TAG, "Host header check '%s'", request->header_value);
                        request->redirect = strcmp(request->header_value, "circuitpython.local") == 0;
                    }
                    ESP_LOGW(TAG, "Header %s %s", request->header_key, request->header_value);
                } else if (request->offset > sizeof(request->header_value) - 1) {
                    // Skip methods that are too long.
                } else {
                    request->header_value[request->offset - 1] = c;
                    request->offset++;
                }
                break;
            }
            case STATE_BODY:
                ESP_LOGW(TAG, "BODY %c", c);
                request->done = true;
                request->state = STATE_METHOD;
                more = false;
                break;
        }
    }
    if (error) {
        const char *error_response = "HTTP/1.1 501 Not Implemented\r\n\r\n";
        int nodelay = 1;
        int err = lwip_setsockopt(socket->num, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
        int sent = socketpool_socket_send(socket, (const uint8_t *)error_response, strlen(error_response));
        ESP_LOGW(TAG, "sent %d %d", sent, err);
    }
    if (!request->done) {
        return;
    }
    if (request->redirect) {
        _reply_redirect(socket, request->path);
    } else if (memcmp(request->path, "/fs/", 4) == 0) {
        if (!request->authenticated) {
            if (_api_password[0] != '\0') {
                _reply_unauthorized(socket);
            } else {
                _reply_forbidden(socket);
            }
        } else {
            ESP_LOGW(TAG, "filesystem %s %s", request->method, request->path + 3);
            const char *path = request->path + 3;
            size_t pathlen = strlen(path);
            FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
            // Trailing / is a directory.
            if (path[pathlen - 1] == '/') {
                if (strcmp(request->method, "GET") == 0) {
                    FF_DIR dir;
                    FRESULT res = f_opendir(fs, &dir, path);
                    if (res != FR_OK) {
                        _reply_missing(socket);
                    }
                    const char *ok_response = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n"
                        "Content-Type: text/html\r\n\r\n";
                    socketpool_socket_send(socket, (const uint8_t *)ok_response, strlen(ok_response));
                    _send_chunk(socket, "<!DOCTYPE html><html><head><title>");
                    _send_chunk(socket, path);
                    _send_chunk(socket, "</title><meta charset=\"UTF-8\"></head><body><h1>");
                    _send_chunk(socket, path);
                    _send_chunk(socket, "</h1><pre>");
                    if (strlen(path) > 1) {
                        _send_chunk(socket, "🗀\t<a href=\"..\">..</a><br/>");
                    }

                    FILINFO file_info;
                    char *fn = file_info.fname;
                    res = f_readdir(&dir, &file_info);
                    while (res == FR_OK && fn[0] != 0) {
                        // uint64_t truncated_time = timeutils_mktime(1980 + (file_info.fdate >> 9),
                        //     (file_info.fdate >> 5) & 0xf,
                        //     file_info.fdate & 0x1f,
                        //     file_info.ftime >> 11,
                        //     (file_info.ftime >> 5) & 0x1f,
                        //     (file_info.ftime & 0x1f) * 2) * 1000000000ULL;
                        // entry->truncated_time = truncated_time;
                        // if ((file_info.fattrib & AM_DIR) != 0) {
                        //     entry->flags = 1; // Directory
                        //     entry->file_size = 0;
                        // } else {
                        //     entry->flags = 0;
                        //     entry->file_size = file_info.fsize;
                        // }
                        // _send_chunk(socket, "<li>");
                        if ((file_info.fattrib & AM_DIR) != 0) {
                            _send_chunk(socket, "🗀\t");
                        } else if (_endswith(file_info.fname, ".txt") ||
                                   _endswith(file_info.fname, ".py")) {
                            _send_chunk(socket, "🖹\t");
                        } else {
                            _send_chunk(socket, "⬇\t");
                        }
                        _send_chunk(socket, "<a href=\"");
                        _send_chunk(socket, request->path);
                        _send_chunk(socket, file_info.fname);
                        if ((file_info.fattrib & AM_DIR) != 0) {
                            _send_chunk(socket, "/");
                        }
                        _send_chunk(socket, "\">");

                        _send_chunk(socket, file_info.fname);
                        _send_chunk(socket, "</a><a>✏️</a><a>🗑️</a><br/>");
                        res = f_readdir(&dir, &file_info);
                    }
                    _send_chunk(socket, "</pre>Upload:<input type=\"file\" multiple></body></html>");
                    _send_chunk(socket, "");
                    f_closedir(&dir);
                }

            } else { // Dealing with a file.
                if (strcmp(request->method, "GET") == 0) {
                    FIL active_file;
                    FRESULT result = f_open(fs, &active_file, path, FA_READ);

                    if (result != FR_OK) {
                        // TODO: 404
                    }
                    uint32_t total_length = f_size(&active_file);
                    char encoded_len[10];
                    snprintf(encoded_len, sizeof(encoded_len), "%d", total_length);

                    _send_str(socket, "HTTP/1.1 200 OK\r\nContent-Length: ");
                    _send_str(socket, encoded_len);
                    _send_str(socket, "\r\n");
                    if (_endswith(path, ".txt") || _endswith(path, ".py")) {
                        _send_str(socket, "Content-Type: text/plain\r\n");
                    } else {
                        _send_str(socket, "Content-Type: application/octet-stream\r\n");
                    }
                    _send_str(socket, "\r\n");

                    uint32_t total_read = 0;
                    while (total_read < total_length) {
                        uint8_t data_buffer[64];
                        size_t quantity_read;
                        f_read(&active_file, data_buffer, 64, &quantity_read);
                        total_read += quantity_read;
                        uint32_t send_offset = 0;
                        while (send_offset < quantity_read) {
                            int sent = socketpool_socket_send(socket, data_buffer + send_offset, quantity_read - send_offset);
                            if (sent < 0) {
                                ESP_LOGE(TAG, "file send error %d", sent);
                                break;
                            }
                            send_offset += sent;
                        }
                    }
                    ESP_LOGW(TAG, "file return done");

                    f_close(&active_file);
                }
            }
        }
    } else if (memcmp(request->path, "/cp/", 4) == 0) {
        ESP_LOGW(TAG, "circuitpython %s %s", request->method, request->path + 3);
        const char *path = request->path + 3;
        if (strcmp(request->method, "GET") != 0) {
            // Return error if not a GET
        }
        if (strcmp(path, "/devices.json") == 0) {
            mdns_remoteservice_obj_t found_devices[32];
            size_t total_results = mdns_server_find(&mdns, "_circuitpython", "_tcp", 1, found_devices, MP_ARRAY_SIZE(found_devices));
            size_t count = MIN(total_results, MP_ARRAY_SIZE(found_devices));
            const char *ok_response = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nContent-Type: application/json\r\n\r\n";
            socketpool_socket_send(socket, (const uint8_t *)ok_response, strlen(ok_response));
            _send_chunk(socket, "{\"total\": ");
            char total_encoded[4];
            snprintf(total_encoded, sizeof(total_encoded), "%d", total_results);
            _send_chunk(socket, total_encoded);
            _send_chunk(socket, ", \"devices\": [");
            ESP_LOGW(TAG, "%d total devices", total_results);
            for (size_t i = 0; i < count; i++) {
                if (i > 0) {
                    _send_chunk(socket, ",");
                }
                _send_chunk(socket, "{\"hostname\": \"");
                _send_chunk(socket, common_hal_mdns_remoteservice_get_hostname(&found_devices[i]));
                _send_chunk(socket, "\", \"instance_name\": \"");
                _send_chunk(socket, common_hal_mdns_remoteservice_get_instance_name(&found_devices[i]));
                _send_chunk(socket, "\", \"port\": ");
                char port_encoded[4];
                int port = common_hal_mdns_remoteservice_get_port(&found_devices[i]);
                snprintf(port_encoded, sizeof(port_encoded), "%d", port);
                _send_chunk(socket, port_encoded);
                _send_chunk(socket, "}");
                common_hal_mdns_remoteservice_deinit(&found_devices[i]);
            }
            _send_chunk(socket, "]}");
            // Empty chunk signals the end of the response.
            _send_chunk(socket, "");
        } else if (strcmp(path, "/version.json") == 0) {
            const char *ok_response = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nContent-Type: application/json\r\n\r\n";
            socketpool_socket_send(socket, (const uint8_t *)ok_response, strlen(ok_response));
            _send_chunk(socket, "{\"web_api_version\": 1, \"version\": \"");
            _send_chunk(socket, MICROPY_GIT_TAG);
            _send_chunk(socket, "\", \"build_date\": \"");
            _send_chunk(socket, MICROPY_BUILD_DATE);
            _send_chunk(socket, "\", \"board_name\": \"");
            _send_chunk(socket, MICROPY_HW_BOARD_NAME);
            _send_chunk(socket, "\", \"mcu_name\": \"");
            _send_chunk(socket, MICROPY_HW_MCU_NAME);
            _send_chunk(socket, "\", \"board_id\": \"");
            _send_chunk(socket, CIRCUITPY_BOARD_ID);
            _send_chunk(socket, "\", \"creator_id\": ");
            char encoded_id[8];
            snprintf(encoded_id, sizeof(encoded_id), "%d", CIRCUITPY_CREATOR_ID);
            _send_chunk(socket, encoded_id);
            _send_chunk(socket, ", \"creation_id\": ");
            snprintf(encoded_id, sizeof(encoded_id), "%d", CIRCUITPY_CREATION_ID);
            _send_chunk(socket, encoded_id);
            _send_chunk(socket, "}");
            // Empty chunk signals the end of the response.
            _send_chunk(socket, "");
        }
    } else {
        const char *ok_response = "HTTP/1.1 200 OK\r\nContent-Length: 11\r\nContent-Type: text/plain\r\n\r\nHello World";
        int sent = socketpool_socket_send(socket, (const uint8_t *)ok_response, strlen(ok_response));
        ESP_LOGW(TAG, "sent ok %d", sent);
    }
    request->done = false;
}


void supervisor_web_workflow_background(void) {
    // Otherwise, see if we have another socket to accept.
    if (!common_hal_socketpool_socket_get_connected(&active) &&
        !common_hal_socketpool_socket_get_closed(&listening) &&
        listening.num > 0) {
        if (!common_hal_socketpool_socket_get_closed(&active)) {
            common_hal_socketpool_socket_close(&active);
        }
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
            active_request.done = false;
            active_request.redirect = false;

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
