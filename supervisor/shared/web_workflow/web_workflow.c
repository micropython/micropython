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
#include "py/stackctrl.h"

#include "shared-bindings/wifi/Radio.h"
#include "shared-module/storage/__init__.h"
#include "shared/timeutils/timeutils.h"
#include "supervisor/shared/translate/translate.h"
#include "supervisor/shared/web_workflow/web_workflow.h"
#include "supervisor/usb.h"

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
    char method[8];
    char path[256];
    char header_key[64];
    char header_value[64];
    size_t content_length;
    size_t offset;
    bool redirect;
    bool done;
    bool authenticated;
    bool expect;
    bool json;
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

    // First pass, we convert input buffer to numeric base 64 values
    char *in = buf + (triples - 1) * 3;
    char *out = buf + (triples - 1) * 4;
    int r = in_len % 3;
    int partial = 0;
    if (r != 0) {
        out[3] = 64;
        if (r == 2) {
            out[2] = (in[1] & 0x0F) << 2;
            out[1] = (in[0] & 0x03) << 4 | (in[1] & 0xF0) >> 4;
        } else {
            out[2] = 64;
            out[1] = (in[0] & 0x03) << 4;
        }
        out[0] = (in[0] & 0xFC) >> 2;
        in -= 3;
        out -= 4;
        partial = 1;
    }
    buf[encoded_len] = '\0';
    for (size_t i = 0; i < triples - partial; i++) {
        out[3] = in[2] & 0x3F;
        out[2] = (in[1] & 0x0F) << 2 | (in[2] & 0xC0) >> 6;
        out[1] = (in[0] & 0x03) << 4 | (in[1] & 0xF0) >> 4;
        out[0] = (in[0] & 0xFC) >> 2;
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
    common_hal_socketpool_socket_bind(&listening, "", 0, 80);
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

static void _reply_continue(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 100 Continue\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_created(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 201 Created\r\n"
        "Content-Length: 0\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_no_content(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 204 No Content\r\n"
        "Content-Length: 0\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_missing(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 404 Not Found\r\n"
        "Content-Length: 0\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_forbidden(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 403 Forbidden\r\n"
        "Content-Length: 0\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_conflict(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 409 Conflict\r\n"
        "Content-Length: 19\r\n\r\nUSB storage active.";
    _send_str(socket, response);
}

static void _reply_payload_too_large(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 413 Payload Too Large\r\n"
        "Content-Length: 0\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_expectation_failed(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 417 Expectation Failed\r\n"
        "Content-Length: 0\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_unauthorized(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 401 Unauthorized\r\n"
        "Content-Length: 0\r\n"
        "WWW-Authenticate: Basic realm=\"CircuitPython\"\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_server_error(socketpool_socket_obj_t *socket) {
    const char *response = "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Length: 0\r\n\r\n";
    _send_str(socket, response);
}

static void _reply_redirect(socketpool_socket_obj_t *socket, const char *path) {
    const char *redirect_response = "HTTP/1.1 301 Moved Permanently\r\nConnection: close\r\nContent-Length: 0\r\nLocation: http://";
    int nodelay = 1;
    lwip_setsockopt(socket->num, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
    _send_str(socket, redirect_response);
    _send_str(socket, common_hal_mdns_server_get_hostname(&mdns));
    _send_str(socket, ".local");
    _send_str(socket, path);
    _send_str(socket, "\r\n\r\n");
}

static void _reply_directory_json(socketpool_socket_obj_t *socket, FF_DIR *dir, const char *request_path, const char *path) {
    const char *ok_response = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n"
        "Content-Type: application/json\r\n\r\n";
    socketpool_socket_send(socket, (const uint8_t *)ok_response, strlen(ok_response));
    _send_chunk(socket, "[");
    bool first = true;

    FILINFO file_info;
    char *fn = file_info.fname;
    FRESULT res = f_readdir(dir, &file_info);
    while (res == FR_OK && fn[0] != 0) {
        if (!first) {
            _send_chunk(socket, ",");
        }
        _send_chunk(socket, "{\"name\": \"");
        _send_chunk(socket, file_info.fname);
        _send_chunk(socket, "\", \"directory\": ");
        if ((file_info.fattrib & AM_DIR) != 0) {
            _send_chunk(socket, "true");
        } else {
            _send_chunk(socket, "false");
        }
        // We use nanoseconds past Jan 1, 1970 for consistency with BLE API and
        // LittleFS.
        _send_chunk(socket, ", \"modified_ns\": ");

        uint64_t truncated_time = timeutils_mktime(1980 + (file_info.fdate >> 9),
            (file_info.fdate >> 5) & 0xf,
            file_info.fdate & 0x1f,
            file_info.ftime >> 11,
            (file_info.ftime >> 5) & 0x1f,
            (file_info.ftime & 0x1f) * 2) * 1000000000ULL;

        char encoded_number[32];
        snprintf(encoded_number, sizeof(encoded_number), "%lld", truncated_time);
        _send_chunk(socket, encoded_number);

        _send_chunk(socket, ", \"file_size\": ");
        size_t file_size = 0;
        if ((file_info.fattrib & AM_DIR) == 0) {
            file_size = file_info.fsize;
        }
        snprintf(encoded_number, sizeof(encoded_number), "%d", file_size);
        _send_chunk(socket, encoded_number);

        _send_chunk(socket, "}");
        first = false;
        res = f_readdir(dir, &file_info);
    }
    _send_chunk(socket, "]");
    _send_chunk(socket, "");
}

static void _reply_directory_html(socketpool_socket_obj_t *socket, FF_DIR *dir, const char *request_path, const char *path) {
    const char *ok_response = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n"
        "Content-Type: text/html\r\n\r\n";
    socketpool_socket_send(socket, (const uint8_t *)ok_response, strlen(ok_response));
    _send_chunk(socket, "<!DOCTYPE html><html><head><title>");
    _send_chunk(socket, path);
    _send_chunk(socket, "</title><meta charset=\"UTF-8\"></head>");
    _send_chunk(socket, "<script src=\"http://127.0.0.1:8000/circuitpython.js\" defer=true></script>");
    _send_chunk(socket, "<body><h1>");
    _send_chunk(socket, path);
    _send_chunk(socket, "</h1><pre>");
    if (strlen(path) > 1) {
        _send_chunk(socket, "🗀\t<a href=\"..\">..</a><br/>");
    }

    FILINFO file_info;
    char *fn = file_info.fname;
    FRESULT res = f_readdir(dir, &file_info);
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
                   _endswith(file_info.fname, ".py") ||
                   _endswith(file_info.fname, ".js") ||
                   _endswith(file_info.fname, ".json")) {
            _send_chunk(socket, "🖹\t");
        } else if (_endswith(file_info.fname, ".html")) {
            _send_chunk(socket, "🌐\t");
        } else {
            _send_chunk(socket, "⬇\t");
        }

        _send_chunk(socket, "<a href=\"");
        _send_chunk(socket, request_path);
        _send_chunk(socket, file_info.fname);
        if ((file_info.fattrib & AM_DIR) != 0) {
            _send_chunk(socket, "/");
        }
        _send_chunk(socket, "\">");

        _send_chunk(socket, file_info.fname);
        _send_chunk(socket, "</a>");
        _send_chunk(socket, "<button value=\"");
        _send_chunk(socket, file_info.fname);
        _send_chunk(socket, "\" class=\"delete\">🗑️</button><br/>");
        res = f_readdir(dir, &file_info);
    }
    _send_chunk(socket, "</pre><hr><input type=\"file\" id=\"files\" multiple><button type=\"submit\" id=\"upload\">Upload</button>");

    _send_chunk(socket, "<hr>+🗀<input type=\"text\" id=\"name\"><button type=\"submit\" id=\"mkdir\">Create Directory</button>");
    _send_chunk(socket, "</body></html>");
    _send_chunk(socket, "");
}

static void _reply_with_file(socketpool_socket_obj_t *socket, const char *filename, FIL *active_file) {
    uint32_t total_length = f_size(active_file);
    char encoded_len[10];
    snprintf(encoded_len, sizeof(encoded_len), "%d", total_length);

    _send_str(socket, "HTTP/1.1 200 OK\r\nContent-Length: ");
    _send_str(socket, encoded_len);
    _send_str(socket, "\r\n");
    if (_endswith(filename, ".txt") || _endswith(filename, ".py")) {
        _send_str(socket, "Content-Type: text/plain\r\n");
    } else if (_endswith(filename, ".js")) {
        _send_str(socket, "Content-Type: text/javascript\r\n");
    } else if (_endswith(filename, ".html")) {
        _send_str(socket, "Content-Type: text/html\r\n");
    } else if (_endswith(filename, ".json")) {
        _send_str(socket, "Content-Type: application/json\r\n");
    } else {
        _send_str(socket, "Content-Type: application/octet-stream\r\n");
    }
    _send_str(socket, "\r\n");

    uint32_t total_read = 0;
    while (total_read < total_length) {
        uint8_t data_buffer[64];
        size_t quantity_read;
        f_read(active_file, data_buffer, 64, &quantity_read);
        total_read += quantity_read;
        uint32_t send_offset = 0;
        while (send_offset < quantity_read) {
            int sent = socketpool_socket_send(socket, data_buffer + send_offset, quantity_read - send_offset);
            if (sent < 0) {
                if (sent == -EAGAIN) {
                    sent = 0;
                } else {
                    ESP_LOGE(TAG, "file send error %d", sent);
                    break;
                }
            }
            send_offset += sent;
        }
    }
    ESP_LOGW(TAG, "file return done");
    if (total_read < total_length) {
        socketpool_socket_close(socket);
    }
}

static void _reply_with_devices_json(socketpool_socket_obj_t *socket) {
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
}

static void _reply_with_version_json(socketpool_socket_obj_t *socket) {
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

STATIC bool _usb_active(void) {
    // Check to see if USB has already been mounted. If not, then we "eject" from USB until we're done.
    #if CIRCUITPY_USB && CIRCUITPY_USB_MSC
    if (storage_usb_enabled() && !usb_msc_lock()) {
        return true;
    }
    #endif
    return false;
}

// Copied from ble file_transfer.c. We should share it.
STATIC FRESULT _delete_directory_contents(FATFS *fs, const TCHAR *path) {
    FF_DIR dir;
    FRESULT res = f_opendir(fs, &dir, path);
    FILINFO file_info;
    // Check the stack since we're putting paths on it.
    if (mp_stack_usage() >= MP_STATE_THREAD(stack_limit)) {
        return FR_INT_ERR;
    }
    while (res == FR_OK) {
        res = f_readdir(&dir, &file_info);
        if (res != FR_OK || file_info.fname[0] == '\0') {
            break;
        }
        size_t pathlen = strlen(path);
        size_t fnlen = strlen(file_info.fname);
        TCHAR full_path[pathlen + 1 + fnlen];
        memcpy(full_path, path, pathlen);
        full_path[pathlen] = '/';
        size_t full_pathlen = pathlen + 1 + fnlen;
        memcpy(full_path + pathlen + 1, file_info.fname, fnlen);
        full_path[full_pathlen] = '\0';
        if ((file_info.fattrib & AM_DIR) != 0) {
            res = _delete_directory_contents(fs, full_path);
        }
        if (res != FR_OK) {
            break;
        }
        res = f_unlink(fs, full_path);
    }
    f_closedir(&dir);
    return res;
}

static void _write_file_and_reply(socketpool_socket_obj_t *socket, _request *request, FATFS *fs, const TCHAR *path) {
    FIL active_file;

    if (_usb_active()) {
        _reply_conflict(socket);
        return;
    }
    FRESULT result = f_open(fs, &active_file, path, FA_WRITE);
    bool new_file = false;
    if (result == FR_NO_FILE) {
        new_file = true;
        result = f_open(fs, &active_file, path, FA_WRITE | FA_OPEN_ALWAYS);
    }

    if (result != FR_OK) {
        ESP_LOGW(TAG, "file write error %d %s", result, path);
        _reply_server_error(socket);
    } else if (request->expect) {
        _reply_continue(socket);
    }

    // Change the file size to start.
    f_lseek(&active_file, request->content_length);
    if (f_tell(&active_file) < request->content_length) {
        f_close(&active_file);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        // Too large.
        if (request->expect) {
            _reply_expectation_failed(socket);
        } else {
            _reply_payload_too_large(socket);
        }

        return;
    }
    f_truncate(&active_file);
    f_rewind(&active_file);

    size_t total_read = 0;
    bool error = false;
    while (total_read < request->content_length && !error) {
        uint8_t bytes[64];
        size_t read_len = MIN(sizeof(bytes), request->content_length - total_read);
        int len = socketpool_socket_recv_into(socket, bytes, read_len);
        if (len < 0) {
            if (len == -EAGAIN) {
                continue;
            } else {
                ESP_LOGW(TAG, "other error %d", len);
            }
            error = true;
            break;
        }
        UINT actual;
        f_write(&active_file, bytes, len, &actual);
        if (actual != (UINT)len) {
            ESP_LOGE(TAG, "didn't write whole file");
        }
        total_read += len;
    }

    f_close(&active_file);
    #if CIRCUITPY_USB_MSC
    usb_msc_unlock();
    #endif
    if (new_file) {
        _reply_created(socket);
    } else {
        _reply_no_content(socket);
    }
}

static void _reply(socketpool_socket_obj_t *socket, _request *request) {
    if (request->redirect) {
        _reply_redirect(socket, request->path);
    } else if (memcmp(request->path, "/fs/", 4) == 0) {
        if (!request->authenticated) {
            ESP_LOGW(TAG, "not authed");
            if (_api_password[0] != '\0') {
                _reply_unauthorized(socket);
            } else {
                _reply_forbidden(socket);
            }
        } else {
            ESP_LOGW(TAG, "filesystem %s %s", request->method, request->path + 3);
            char *path = request->path + 3;
            size_t pathlen = strlen(path);
            FATFS *fs = &((fs_user_mount_t *)MP_STATE_VM(vfs_mount_table)->obj)->fatfs;
            // Trailing / is a directory.
            bool directory = false;
            if (path[pathlen - 1] == '/') {
                // FATFS lib doesn't like a trailing / for paths besides root.
                if (pathlen > 1) {
                    path[pathlen - 1] = '\0';
                }
                directory = true;
            }
            // Delete is almost identical for files and directories so share the
            // implementation.
            if (strcmp(request->method, "DELETE") == 0) {
                if (_usb_active()) {
                    _reply_conflict(socket);
                    return;
                }

                FILINFO file;
                FRESULT result = f_stat(fs, path, &file);
                if (result == FR_OK) {
                    if ((file.fattrib & AM_DIR) != 0) {
                        result = _delete_directory_contents(fs, path);
                    }
                    if (result == FR_OK) {
                        result = f_unlink(fs, path);
                    }
                }

                if (result == FR_NO_PATH || result == FR_NO_FILE) {
                    _reply_missing(socket);
                } else if (result != FR_OK) {
                    ESP_LOGW(TAG, "rm error %d %s", result, path);
                    _reply_server_error(socket);
                } else {
                    _reply_no_content(socket);
                }
            } else if (directory) {
                if (strcmp(request->method, "GET") == 0) {
                    FF_DIR dir;
                    FRESULT res = f_opendir(fs, &dir, path);
                    // Put the / back for replies.
                    if (pathlen > 1) {
                        path[pathlen - 1] = '/';
                    }
                    if (res != FR_OK) {
                        ESP_LOGW(TAG, "unable to open %d %s", res, path);
                        _reply_missing(socket);
                        return;
                    }
                    if (request->json) {
                        _reply_directory_json(socket, &dir, request->path, path);
                    } else {
                        _reply_directory_html(socket, &dir, request->path, path);
                    }

                    f_closedir(&dir);
                } else if (strcmp(request->method, "PUT") == 0) {
                    if (_usb_active()) {
                        _reply_conflict(socket);
                        return;
                    }

                    FRESULT result = f_mkdir(fs, path);
                    #if CIRCUITPY_USB_MSC
                    usb_msc_unlock();
                    #endif
                    if (result == FR_EXIST) {
                        _reply_no_content(socket);
                    } else if (result == FR_NO_PATH) {
                        _reply_missing(socket);
                    } else if (result != FR_OK) {
                        ESP_LOGE(TAG, "mkdir error %d %s", result, path);
                        _reply_server_error(socket);
                    } else {
                        _reply_created(socket);
                    }
                }
            } else { // Dealing with a file.
                if (strcmp(request->method, "GET") == 0) {
                    FIL active_file;
                    FRESULT result = f_open(fs, &active_file, path, FA_READ);

                    if (result != FR_OK) {
                        // TODO: 404
                        _reply_missing(socket);
                    } else {
                        _reply_with_file(socket, path, &active_file);
                    }

                    f_close(&active_file);
                } else if (strcmp(request->method, "PUT") == 0) {
                    _write_file_and_reply(socket, request, fs, path);
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
            _reply_with_devices_json(socket);
        } else if (strcmp(path, "/version.json") == 0) {
            _reply_with_version_json(socket);
        }
    } else {
        const char *ok_response = "HTTP/1.1 200 OK\r\nContent-Length: 11\r\nContent-Type: text/plain\r\n\r\nHello World";
        int sent = socketpool_socket_send(socket, (const uint8_t *)ok_response, strlen(ok_response));
        ESP_LOGW(TAG, "sent ok %d", sent);
    }
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
                    request->state = STATE_HEADER_KEY;
                    request->offset = 0;
                }
                break;
            }
            case STATE_HEADER_KEY: {
                if (c == '\r') {
                    request->state = STATE_BODY;
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
                        const char *prefix = "Basic ";
                        request->authenticated = memcmp(request->header_value, prefix, strlen(prefix)) == 0 &&
                            strcmp(_api_password, request->header_value + strlen(prefix)) == 0;
                    } else if (strcmp(request->header_key, "Host") == 0) {
                        request->redirect = strcmp(request->header_value, "circuitpython.local") == 0;
                    } else if (strcmp(request->header_key, "Content-Length") == 0) {
                        request->content_length = strtoul(request->header_value, NULL, 10);
                    } else if (strcmp(request->header_key, "Expect") == 0) {
                        request->expect = strcmp(request->header_value, "100-continue") == 0;
                    } else if (strcmp(request->header_key, "Accept") == 0) {
                        request->json = strcmp(request->header_value, "application/json") == 0;
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
    _reply(socket, request);
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
        if (newsoc == -EBADF) {
            common_hal_socketpool_socket_close(&listening);
            return;
        }
        if (newsoc != -EAGAIN) {
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
