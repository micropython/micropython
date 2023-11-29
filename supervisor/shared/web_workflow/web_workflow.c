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

// Include strchrnul()
#define _GNU_SOURCE

#include <stdarg.h>
#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "genhdr/mpversion.h"
#include "py/mperrno.h"
#include "py/mpstate.h"
#include "py/stackctrl.h"

#include "shared-bindings/wifi/Radio.h"
#include "shared-module/storage/__init__.h"
#include "shared/timeutils/timeutils.h"
#include "supervisor/fatfs.h"
#include "supervisor/filesystem.h"
#include "supervisor/port.h"
#include "supervisor/shared/reload.h"
#include "supervisor/shared/translate/translate.h"
#include "supervisor/shared/web_workflow/web_workflow.h"
#include "supervisor/shared/web_workflow/websocket.h"
#include "supervisor/shared/workflow.h"
#include "supervisor/usb.h"

#include "shared-bindings/hashlib/__init__.h"
#include "shared-bindings/hashlib/Hash.h"
#include "lib/oofatfs/diskio.h"

#if CIRCUITPY_MDNS
#include "shared-bindings/mdns/RemoteService.h"
#include "shared-bindings/mdns/Server.h"
#endif

#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/socketpool/__init__.h"
#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/socketpool/SocketPool.h"

#if CIRCUITPY_WIFI
#include "shared-bindings/wifi/__init__.h"
#endif

#if CIRCUITPY_OS_GETENV
#include "shared-module/os/__init__.h"
#endif

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
    char destination[256];
    char header_key[64];
    char header_value[256];
    char origin[64];        // We store the origin so we can reply back with it.
    char host[64];          // We store the host to check against origin.
    size_t content_length;
    size_t offset;
    uint64_t timestamp_ms;
    bool redirect;
    bool done;
    bool in_progress;
    bool authenticated;
    bool expect;
    bool json;
    bool websocket;
    bool new_socket;
    uint32_t websocket_version;
    // RFC6455 for websockets says this header should be 24 base64 characters long.
    char websocket_key[24 + 1];
} _request;

static wifi_radio_error_t _wifi_status = WIFI_RADIO_ERROR_NONE;

#if CIRCUITPY_STATUS_BAR
// Store various last states to compute if status bar needs an update.
static bool _last_enabled = false;
static uint32_t _last_ip = 0;
static wifi_radio_error_t _last_wifi_status = WIFI_RADIO_ERROR_NONE;
#endif

#if CIRCUITPY_MDNS
static mdns_server_obj_t mdns;
#endif

static mp_int_t web_api_port = 80;

static socketpool_socketpool_obj_t pool;
static socketpool_socket_obj_t listening;
static socketpool_socket_obj_t active;

static _request active_request;

static char _api_password[64];
static char web_instance_name[50];

// Store the encoded IP so we don't duplicate work.
static uint32_t _encoded_ip = 0;
static char _our_ip_encoded[4 * 4];

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

STATIC void _update_encoded_ip(void) {
    uint32_t ipv4_address = 0;
    if (common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj)) {
        ipv4_address = wifi_radio_get_ipv4_address(&common_hal_wifi_radio_obj);
    }
    if (_encoded_ip != ipv4_address) {
        uint8_t *octets = (uint8_t *)&ipv4_address;
        snprintf(_our_ip_encoded, sizeof(_our_ip_encoded), "%d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
        _encoded_ip = ipv4_address;
    }
}

mdns_server_obj_t *supervisor_web_workflow_mdns(mp_obj_t network_interface) {
    #if CIRCUITPY_MDNS
    if (network_interface == &common_hal_wifi_radio_obj &&
        mdns.base.type == &mdns_server_type) {
        return &mdns;
    }
    #endif
    return NULL;
}

#if CIRCUITPY_STATUS_BAR
bool supervisor_web_workflow_status_dirty(void) {
    return common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj) != _last_enabled ||
           _encoded_ip != _last_ip ||
           _last_wifi_status != _wifi_status;
}
#endif

#if CIRCUITPY_STATUS_BAR
void supervisor_web_workflow_status(void) {
    _last_enabled = common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj);
    if (_last_enabled) {
        uint32_t ipv4_address = wifi_radio_get_ipv4_address(&common_hal_wifi_radio_obj);
        if (ipv4_address != 0) {
            _update_encoded_ip();
            _last_ip = _encoded_ip;
            mp_printf(&mp_plat_print, "%s", _our_ip_encoded);
            if (web_api_port != 80) {
                mp_printf(&mp_plat_print, ":%d", web_api_port);
            }
            // TODO: Use these unicode to show signal strength: ▂▄▆█
            return;
        }
        serial_write_compressed(MP_ERROR_TEXT("Wi-Fi: "));
        _last_wifi_status = _wifi_status;
        if (_wifi_status == WIFI_RADIO_ERROR_AUTH_EXPIRE ||
            _wifi_status == WIFI_RADIO_ERROR_AUTH_FAIL) {
            serial_write_compressed(MP_ERROR_TEXT("Authentication failure"));
        } else if (_wifi_status != WIFI_RADIO_ERROR_NONE) {
            mp_printf(&mp_plat_print, "%d", _wifi_status);
        } else if (ipv4_address == 0) {
            _last_ip = 0;
            serial_write_compressed(MP_ERROR_TEXT("No IP"));
        } else {
        }
    } else {
        // Keep Wi-Fi print separate so its data can be matched with the one above.
        serial_write_compressed(MP_ERROR_TEXT("Wi-Fi: "));
        serial_write_compressed(MP_ERROR_TEXT("off"));
    }
}
#endif

bool supervisor_start_web_workflow(bool reload) {
    #if CIRCUITPY_WEB_WORKFLOW && CIRCUITPY_WIFI && CIRCUITPY_OS_GETENV

    char ssid[33];
    char password[64];

    os_getenv_err_t result = common_hal_os_getenv_str("CIRCUITPY_WIFI_SSID", ssid, sizeof(ssid));
    if (result != GETENV_OK) {
        return false;
    }

    result = common_hal_os_getenv_str("CIRCUITPY_WIFI_PASSWORD", password, sizeof(password));
    if (result == GETENV_ERR_NOT_FOUND) {
        // if password is unspecified, assume an open network
        password[0] = '\0';
    } else if (result != GETENV_OK) {
        return false;
    }

    if (!common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj)) {
        common_hal_wifi_init(false);
        common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, true);
    }

    // TODO: Do our own scan so that we can find the channel we want before calling connect.
    // Otherwise, connect will do a full slow scan to pick the best AP.

    // We can all connect again because it will return early if we're already connected to the
    // network. If we are connected to a different network, then it will disconnect before
    // attempting to connect to the given network.

    _wifi_status = common_hal_wifi_radio_connect(
        &common_hal_wifi_radio_obj, (uint8_t *)ssid, strlen(ssid), (uint8_t *)password, strlen(password),
        0, 8, NULL, 0);

    if (_wifi_status != WIFI_RADIO_ERROR_NONE) {
        common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, false);
        return false;
    }

    // Skip starting the workflow if we're not starting from power on or reset.
    const mcu_reset_reason_t reset_reason = common_hal_mcu_processor_get_reset_reason();
    if (reset_reason != RESET_REASON_POWER_ON &&
        reset_reason != RESET_REASON_RESET_PIN &&
        reset_reason != RESET_REASON_DEEP_SLEEP_ALARM &&
        reset_reason != RESET_REASON_UNKNOWN &&
        reset_reason != RESET_REASON_SOFTWARE) {
        return false;
    }

    bool initialized = pool.base.type == &socketpool_socketpool_type;

    if (!initialized && !reload) {
        result = common_hal_os_getenv_str("CIRCUITPY_WEB_INSTANCE_NAME", web_instance_name, sizeof(web_instance_name));
        if (result != GETENV_OK || web_instance_name[0] == '\0') {
            strcpy(web_instance_name, MICROPY_HW_BOARD_NAME);
        }

        // (leaves new_port unchanged on any failure)
        (void)common_hal_os_getenv_int("CIRCUITPY_WEB_API_PORT", &web_api_port);

        const size_t api_password_len = sizeof(_api_password) - 1;
        result = common_hal_os_getenv_str("CIRCUITPY_WEB_API_PASSWORD", _api_password + 1, api_password_len);
        if (result == GETENV_OK) {
            _api_password[0] = ':';
            _base64_in_place(_api_password, strlen(_api_password), sizeof(_api_password) - 1);
        } else { // Skip starting web-workflow when no password is passed.
            return false;
        }

        pool.base.type = &socketpool_socketpool_type;
        common_hal_socketpool_socketpool_construct(&pool, &common_hal_wifi_radio_obj);

        socketpool_socket_reset(&listening);
        socketpool_socket_reset(&active);

        websocket_init();
    }

    initialized = pool.base.type == &socketpool_socketpool_type;

    if (initialized) {
        if (!common_hal_socketpool_socket_get_closed(&active)) {
            common_hal_socketpool_socket_close(&active);
        }

        #if CIRCUITPY_MDNS
        // Try to start MDNS if the user deinited it.
        if (mdns.base.type != &mdns_server_type ||
            common_hal_mdns_server_deinited(&mdns)) {
            mdns_server_construct(&mdns, true);
            mdns.base.type = &mdns_server_type;
            if (!common_hal_mdns_server_deinited(&mdns)) {
                common_hal_mdns_server_set_instance_name(&mdns, web_instance_name);
            }
        }
        if (!common_hal_mdns_server_deinited(&mdns)) {
            common_hal_mdns_server_advertise_service(&mdns, "_circuitpython", "_tcp", web_api_port, NULL, 0);
        }
        #endif

        if (common_hal_socketpool_socket_get_closed(&listening)) {
            socketpool_socket(&pool, SOCKETPOOL_AF_INET, SOCKETPOOL_SOCK_STREAM, &listening);
            common_hal_socketpool_socket_settimeout(&listening, 0);
            // Bind to any ip. (Not checking for failures)
            common_hal_socketpool_socket_bind(&listening, "", 0, web_api_port);
            common_hal_socketpool_socket_listen(&listening, 1);
        }
        // Wake polling thread (maybe)
        socketpool_socket_poll_resume();
        return true;
    }
    #endif
    return false;
}

void web_workflow_send_raw(socketpool_socket_obj_t *socket, bool flush, const uint8_t *buf, int len) {
    int total_sent = 0;
    int sent = -MP_EAGAIN;
    int nodelay_ok = -1;
    // When flushing, disable Nagle's combining algorithm so that buf is sent immediately.
    if (flush) {
        int nodelay = 1;
        nodelay_ok = common_hal_socketpool_socket_setsockopt(socket, SOCKETPOOL_IPPROTO_TCP, SOCKETPOOL_TCP_NODELAY, &nodelay, sizeof(nodelay));
    }

    while ((sent == -MP_EAGAIN || (sent > 0 && total_sent < len)) &&
           common_hal_socketpool_socket_get_connected(socket)) {
        sent = socketpool_socket_send(socket, buf + total_sent, len - total_sent);
        if (sent > 0) {
            total_sent += sent;
            if (total_sent < len) {
                // Yield so that network code can run.
                port_yield();
            }
        }
    }

    // Re-enable Nagle's algorithm when done sending.
    if (nodelay_ok == 0) {
        int nodelay = 0;
        nodelay_ok = common_hal_socketpool_socket_setsockopt(socket, SOCKETPOOL_IPPROTO_TCP, SOCKETPOOL_TCP_NODELAY, &nodelay, sizeof(nodelay));
    }
}

STATIC void _print_raw(void *env, const char *str, size_t len) {
    web_workflow_send_raw((socketpool_socket_obj_t *)env, false, (const uint8_t *)str, (size_t)len);
}

static void _send_str(socketpool_socket_obj_t *socket, const char *str) {
    web_workflow_send_raw(socket, false, (const uint8_t *)str, strlen(str));
}

static void _send_str_maybe_flush(socketpool_socket_obj_t *socket, bool flush, const char *str) {
    web_workflow_send_raw(socket, flush, (const uint8_t *)str, strlen(str));
}

static void _send_final_str(socketpool_socket_obj_t *socket, const char *str) {
    web_workflow_send_raw(socket, true, (const uint8_t *)str, strlen(str));
}

// The last argument must be NULL! Otherwise, it won't stop.
static __attribute__((sentinel)) void _send_strs(socketpool_socket_obj_t *socket, ...) {
    va_list ap;
    va_start(ap, socket);

    const char *str = va_arg(ap, const char *);
    const char *next_str = va_arg(ap, const char *);
    assert(str != NULL);
    _send_str(socket, str);
    while (next_str != NULL) {
        str = next_str;
        next_str = va_arg(ap, const char *);
        _send_str_maybe_flush(socket, next_str == NULL, str);
    }
    va_end(ap);
}

static void _send_chunk(socketpool_socket_obj_t *socket, const char *chunk) {
    mp_print_t _socket_print = {socket, _print_raw};
    mp_printf(&_socket_print, "%X\r\n", strlen(chunk));
    web_workflow_send_raw(socket, false, (const uint8_t *)chunk, strlen(chunk));
    web_workflow_send_raw(socket, strlen(chunk) == 0, (const uint8_t *)"\r\n", 2);
}

STATIC void _print_chunk(void *env, const char *str, size_t len) {
    mp_print_t _socket_print = {env, _print_raw};
    mp_printf(&_socket_print, "%X\r\n", len);
    web_workflow_send_raw((socketpool_socket_obj_t *)env, false, (const uint8_t *)str, len);
    web_workflow_send_raw((socketpool_socket_obj_t *)env, true, (const uint8_t *)"\r\n", 2);
}

// A bit of a misnomer because it sends all arguments as one chunk.
// The last argument must be NULL! Otherwise, it won't stop.
static void _send_chunks(socketpool_socket_obj_t *socket, ...) {
    va_list strs_to_count;
    va_start(strs_to_count, socket);

    va_list strs_to_send;
    va_copy(strs_to_send, strs_to_count);

    size_t chunk_len = 0;
    const char *str = va_arg(strs_to_count, const char *);
    while (str != NULL) {
        chunk_len += strlen(str);
        str = va_arg(strs_to_count, const char *);
    }
    va_end(strs_to_count);


    mp_print_t _socket_print = {socket, _print_raw};
    mp_printf(&_socket_print, "%X\r\n", chunk_len);

    str = va_arg(strs_to_send, const char *);
    while (str != NULL) {
        _send_str(socket, str);
        str = va_arg(strs_to_send, const char *);
    }
    va_end(strs_to_send);

    _send_str(socket, "\r\n");
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

const char http_scheme[] = "http://";
#define PREFIX_HTTP_LEN (sizeof(http_scheme) - 1)

static bool _origin_ok(_request *request) {
    // Origin may be 'null'
    if (request->origin[0] == '\0') {
        return true;
    }
    // Origin has http prefix?
    if (strncmp(request->origin, http_scheme, PREFIX_HTTP_LEN) != 0) {
        // Not HTTP scheme request - ok
        request->origin[0] = '\0';
        return true;
    }
    // Host given?
    if (request->host[0] != '\0') {
        // OK if host and origin match (fqdn + port #)
        if (strcmp(request->host, &request->origin[PREFIX_HTTP_LEN]) == 0) {
            return true;
        }
        // DEBUG: OK if origin is 'localhost' (ignoring port #)
        char *cptr = strchrnul(&request->origin[PREFIX_HTTP_LEN], ':');
        char csave = *cptr; // NULL or colon
        *cptr = '\0';
        if (strcmp(&request->origin[PREFIX_HTTP_LEN], "localhost") == 0) {
            // Restore removed colon if non-null host terminator
            *cptr = csave;
            return true;
        }
    }
    // Otherwise deny request
    return false;
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


static const char *OK_JSON = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nContent-Type: application/json\r\n";

static void _cors_header(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "Access-Control-Allow-Credentials: true\r\n",
        "Vary: Origin, Accept, Upgrade\r\n",
        "Access-Control-Allow-Origin: ",
        (request->origin[0] == '\0') ? "*" : request->origin, "\r\n", NULL);
}

static void _reply_continue(socketpool_socket_obj_t *socket, _request *request) {
    _send_str(socket, "HTTP/1.1 100 Continue\r\n");
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_created(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 201 Created\r\n",
        "Content-Length: 0\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_no_content(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 204 No Content\r\n",
        "Content-Length: 0\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_access_control(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 204 No Content\r\n",
        "Content-Length: 0\r\n",
        "Access-Control-Expose-Headers: Access-Control-Allow-Methods\r\n",
        "Access-Control-Allow-Headers: X-Timestamp, X-Destination, Content-Type, Authorization\r\n",
        "Access-Control-Allow-Methods:GET, OPTIONS", NULL);
    if (!_usb_active()) {
        _send_str(socket, ", PUT, DELETE, MOVE");
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
    }
    _send_str(socket, "\r\n");
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_missing(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 404 Not Found\r\n",
        "Content-Length: 0\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_method_not_allowed(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 405 Method Not Allowed\r\n",
        "Content-Length: 0\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_forbidden(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 403 Forbidden\r\n",
        "Content-Length: 0\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_conflict(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 409 Conflict\r\n",
        "Content-Length: 19\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\nUSB storage active.");
}


static void _reply_precondition_failed(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 412 Precondition Failed\r\n",
        "Content-Length: 0\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_payload_too_large(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 413 Payload Too Large\r\n",
        "Content-Length: 0\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_expectation_failed(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 417 Expectation Failed\r\n",
        "Content-Length: 0\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_unauthorized(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 401 Unauthorized\r\n",
        "Content-Length: 0\r\n",
        "WWW-Authenticate: Basic realm=\"CircuitPython\"\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

static void _reply_server_error(socketpool_socket_obj_t *socket, _request *request) {
    _send_strs(socket,
        "HTTP/1.1 500 Internal Server Error\r\n",
        "Content-Length: 0\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}

#if CIRCUITPY_MDNS
static void _reply_redirect(socketpool_socket_obj_t *socket, _request *request, const char *path) {
    int nodelay = 1;
    common_hal_socketpool_socket_setsockopt(socket, SOCKETPOOL_IPPROTO_TCP, SOCKETPOOL_TCP_NODELAY, &nodelay, sizeof(nodelay));
    const char *hostname = common_hal_mdns_server_get_hostname(&mdns);
    _send_strs(socket,
        "HTTP/1.1 307 Temporary Redirect\r\n",
        "Connection: close\r\n",
        "Content-Length: 0\r\n",
        "Location: ", NULL);
    if (request->websocket) {
        _send_str(socket, "ws");
    } else {
        _send_str(socket, "http");
    }

    _send_strs(socket, "://", hostname, ".local", NULL);
    if (web_api_port != 80) {
        mp_print_t _socket_print = {socket, _print_raw};
        mp_printf(&_socket_print, ":%d", web_api_port);
    }
    _send_strs(socket, path, "\r\n", NULL);
    _cors_header(socket, request);
    _send_final_str(socket, "\r\n");
}
#endif

static void _reply_directory_json(socketpool_socket_obj_t *socket, _request *request, FF_DIR *dir, const char *request_path, const char *path) {
    socketpool_socket_send(socket, (const uint8_t *)OK_JSON, strlen(OK_JSON));
    _cors_header(socket, request);
    _send_str(socket, "\r\n");
    mp_print_t _socket_print = {socket, _print_chunk};
    _send_chunk(socket, "[");
    bool first = true;

    FILINFO file_info;
    char *fn = file_info.fname;
    FRESULT res = f_readdir(dir, &file_info);
    while (res == FR_OK && fn[0] != 0) {
        if (!first) {
            _send_chunk(socket, ",");
        }
        _send_chunks(socket,
            "{\"name\": \"", file_info.fname, "\",",
            "\"directory\": ", NULL);
        if ((file_info.fattrib & AM_DIR) != 0) {
            _send_chunk(socket, "true");
        } else {
            _send_chunk(socket, "false");
        }
        // We use nanoseconds past Jan 1, 1970 for consistency with BLE API and
        // LittleFS.
        _send_chunk(socket, ", ");

        uint32_t truncated_time = timeutils_mktime(1980 + (file_info.fdate >> 9),
            (file_info.fdate >> 5) & 0xf,
            file_info.fdate & 0x1f,
            file_info.ftime >> 11,
            (file_info.ftime >> 5) & 0x3f,
            (file_info.ftime & 0x1f) * 2);

        // Manually append zeros to make the time nanoseconds. Support for printing 64 bit numbers
        // varies across chipsets.
        mp_printf(&_socket_print, "\"modified_ns\": %lu000000000, ", truncated_time);
        size_t file_size = 0;
        if ((file_info.fattrib & AM_DIR) == 0) {
            file_size = file_info.fsize;
        }
        mp_printf(&_socket_print, "\"file_size\": %d }", file_size);

        first = false;
        res = f_readdir(dir, &file_info);
    }
    _send_chunk(socket, "]");
    _send_chunk(socket, "");
}

static void _reply_with_file(socketpool_socket_obj_t *socket, _request *request, const char *filename, FIL *active_file) {
    uint32_t total_length = f_size(active_file);

    _send_str(socket, "HTTP/1.1 200 OK\r\n");
    mp_print_t _socket_print = {socket, _print_raw};
    mp_printf(&_socket_print, "Content-Length: %d\r\n", total_length);
    // TODO: Make this a table to save space.
    if (_endswith(filename, ".txt") || _endswith(filename, ".py") || _endswith(filename, ".toml")) {
        _send_strs(socket, "Content-Type:", "text/plain", ";charset=UTF-8\r\n", NULL);
    } else if (_endswith(filename, ".js")) {
        _send_strs(socket, "Content-Type:", "text/javascript", ";charset=UTF-8\r\n", NULL);
    } else if (_endswith(filename, ".html")) {
        _send_strs(socket, "Content-Type:", "text/html", ";charset=UTF-8\r\n", NULL);
    } else if (_endswith(filename, ".json")) {
        _send_strs(socket, "Content-Type:", "application/json", ";charset=UTF-8\r\n", NULL);
    } else {
        _send_strs(socket, "Content-Type:", "application/octet-stream\r\n", NULL);
    }
    _cors_header(socket, request);
    _send_str(socket, "\r\n");

    uint32_t total_read = 0;
    int nodelay_ok = -1;
    while (total_read < total_length) {
        uint8_t data_buffer[64];
        size_t quantity_read;
        f_read(active_file, data_buffer, 64, &quantity_read);
        total_read += quantity_read;
        // When getting near the end of the file, disable Nagle's combining algorithm so that
        // data is sent immediately.
        if (total_length - total_read < 64) {
            int nodelay = 1;
            // Returns 0 when it works.
            nodelay_ok = common_hal_socketpool_socket_setsockopt(socket, SOCKETPOOL_IPPROTO_TCP, SOCKETPOOL_TCP_NODELAY, &nodelay, sizeof(nodelay));
        }
        uint32_t send_offset = 0;
        while (send_offset < quantity_read) {
            int sent = socketpool_socket_send(socket, data_buffer + send_offset, quantity_read - send_offset);
            if (sent < 0) {
                if (sent == -MP_EAGAIN) {
                    sent = 0;
                } else {
                    break;
                }
            }
            send_offset += sent;
        }
    }
    if (total_read < total_length) {
        socketpool_socket_close(socket);
    }

    // Re-enable Nagle's algorithm when done sending.
    if (nodelay_ok == 0) {
        int nodelay = 0;
        nodelay_ok = common_hal_socketpool_socket_setsockopt(socket, SOCKETPOOL_IPPROTO_TCP, SOCKETPOOL_TCP_NODELAY, &nodelay, sizeof(nodelay));
    }
}

static void _reply_with_devices_json(socketpool_socket_obj_t *socket, _request *request) {
    size_t total_results = 0;
    #if CIRCUITPY_MDNS
    mdns_remoteservice_obj_t found_devices[32];
    if (!common_hal_mdns_server_deinited(&mdns)) {
        total_results = mdns_server_find(&mdns, "_circuitpython", "_tcp", 1, found_devices, MP_ARRAY_SIZE(found_devices));
    }
    size_t count = MIN(total_results, MP_ARRAY_SIZE(found_devices));
    #endif
    socketpool_socket_send(socket, (const uint8_t *)OK_JSON, strlen(OK_JSON));
    _cors_header(socket, request);
    _send_str(socket, "\r\n");
    mp_print_t _socket_print = {socket, _print_chunk};

    mp_printf(&_socket_print, "{\"total\": %d, \"devices\": [", total_results);
    #if CIRCUITPY_MDNS
    for (size_t i = 0; i < count; i++) {
        if (i > 0) {
            _send_chunk(socket, ",");
        }
        const char *hostname = common_hal_mdns_remoteservice_get_hostname(&found_devices[i]);
        const char *instance_name = common_hal_mdns_remoteservice_get_instance_name(&found_devices[i]);
        int port = common_hal_mdns_remoteservice_get_port(&found_devices[i]);
        uint32_t ipv4_address = mdns_remoteservice_get_ipv4_address(&found_devices[i]);
        uint8_t *octets = (uint8_t *)&ipv4_address;
        mp_printf(&_socket_print,
            "{\"hostname\": \"%s\", "
            "\"instance_name\": \"%s\", "
            "\"port\": %d, "
            "\"ip\": \"%d.%d.%d.%d\"}", hostname, instance_name, port, octets[0], octets[1], octets[2], octets[3]);
        common_hal_mdns_remoteservice_deinit(&found_devices[i]);
    }
    #endif
    _send_chunk(socket, "]}");
    // Empty chunk signals the end of the response.
    _send_chunk(socket, "");
}

static void _reply_with_version_json(socketpool_socket_obj_t *socket, _request *request) {
    _send_str(socket, OK_JSON);
    _cors_header(socket, request);
    _send_str(socket, "\r\n");
    mp_print_t _socket_print = {socket, _print_chunk};

    const char *hostname = "";
    const char *instance_name = "";
    #if CIRCUITPY_MDNS
    if (!common_hal_mdns_server_deinited(&mdns)) {
        hostname = common_hal_mdns_server_get_hostname(&mdns);
        instance_name = common_hal_mdns_server_get_instance_name(&mdns);
    }
    #endif
    _update_encoded_ip();
    // Note: this leverages the fact that C concats consecutive string literals together.
    mp_printf(&_socket_print,
        "{\"web_api_version\": 3, "
        "\"version\": \"" MICROPY_GIT_TAG "\", "
        "\"build_date\": \"" MICROPY_BUILD_DATE "\", "
        "\"board_name\": \"%s\", "
        "\"mcu_name\": \"" MICROPY_HW_MCU_NAME "\", "
        "\"board_id\": \"" CIRCUITPY_BOARD_ID "\", "
        "\"creator_id\": %u, "
        "\"creation_id\": %u, "
        "\"hostname\": \"%s\", "
        "\"port\": %d, ", instance_name, CIRCUITPY_CREATOR_ID, CIRCUITPY_CREATION_ID, hostname, web_api_port, _our_ip_encoded);
    #if CIRCUITPY_MICROCONTROLLER && COMMON_HAL_MCU_PROCESSOR_UID_LENGTH > 0
    uint8_t raw_id[COMMON_HAL_MCU_PROCESSOR_UID_LENGTH];
    common_hal_mcu_processor_get_uid(raw_id);
    mp_printf(&_socket_print, "\"UID\": \"");
    for (uint8_t i = 0; i < COMMON_HAL_MCU_PROCESSOR_UID_LENGTH; i++) {
        mp_printf(&_socket_print, "%02X", raw_id[i]);
    }
    mp_printf(&_socket_print, "\", ");
    #endif
    mp_printf(&_socket_print, "\"ip\": \"%s\"}", _our_ip_encoded);
    // Empty chunk signals the end of the response.
    _send_chunk(socket, "");
}

static void _reply_with_diskinfo_json(socketpool_socket_obj_t *socket, _request *request) {
    _send_str(socket, OK_JSON);
    _cors_header(socket, request);
    _send_str(socket, "\r\n");
    mp_print_t _socket_print = {socket, _print_chunk};

    DWORD free_clusters;
    FATFS *fs = filesystem_circuitpy();
    FRESULT blk_result = f_getfree(fs, &free_clusters);
    uint16_t block_size;
    if (blk_result == FR_OK) {
        disk_ioctl(fs, GET_SECTOR_SIZE, &block_size);
    }

    uint16_t total_size = fs->n_fatent - 2;

    const char *writable = "false";
    if (!_usb_active()) {
        writable = "true";
    }
    mp_printf(&_socket_print,
        "[{\"root\": \"/\", "
        "\"free\": %d, "
        "\"block_size\": %d, "
        "\"writable\": %s, "
        "\"total\": %d}]", free_clusters * block_size, block_size, writable, total_size * block_size);

    // Empty chunk signals the end of the response.
    _send_chunk(socket, "");
}


// FATFS has a two second timestamp resolution but the BLE API allows for nanosecond resolution.
// This function truncates the time the time to a resolution storable by FATFS and fills in the
// FATFS encoded version into fattime.
STATIC uint64_t truncate_time(uint64_t input_time, DWORD *fattime) {
    timeutils_struct_time_t tm;
    uint64_t seconds_since_epoch = timeutils_seconds_since_epoch_from_nanoseconds_since_1970(input_time);
    timeutils_seconds_since_epoch_to_struct_time(seconds_since_epoch, &tm);
    uint64_t truncated_time = timeutils_nanoseconds_since_epoch_to_nanoseconds_since_1970((seconds_since_epoch / 2) * 2 * 1000000000);

    *fattime = ((tm.tm_year - 1980) << 25) | (tm.tm_mon << 21) | (tm.tm_mday << 16) |
        (tm.tm_hour << 11) | (tm.tm_min << 5) | (tm.tm_sec >> 1);
    return truncated_time;
}

STATIC void _discard_incoming(socketpool_socket_obj_t *socket, size_t amount) {
    size_t discarded = 0;
    while (discarded < amount) {
        uint8_t bytes[64];
        size_t read_len = MIN(sizeof(bytes), amount - discarded);
        int len = socketpool_socket_recv_into(socket, bytes, read_len);
        if (len < 0) {
            if (len == -MP_EAGAIN) {
                continue;
            }
            break;
        }
        discarded += len;
    }
}

static void _write_file_and_reply(socketpool_socket_obj_t *socket, _request *request, FATFS *fs, const TCHAR *path) {
    FIL active_file;

    if (_usb_active()) {
        _discard_incoming(socket, request->content_length);
        _reply_conflict(socket, request);
        return;
    }
    if (request->timestamp_ms > 0) {
        DWORD fattime;
        truncate_time(request->timestamp_ms * 1000000, &fattime);
        override_fattime(fattime);
    }

    FRESULT result = f_open(fs, &active_file, path, FA_WRITE);
    bool new_file = false;
    size_t old_length = 0;
    if (result == FR_NO_FILE) {
        new_file = true;
        result = f_open(fs, &active_file, path, FA_WRITE | FA_OPEN_ALWAYS);
    } else {
        old_length = f_size(&active_file);
    }

    if (result == FR_NO_PATH) {
        override_fattime(0);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        _discard_incoming(socket, request->content_length);
        _reply_missing(socket, request);
        return;
    }
    if (result != FR_OK) {
        override_fattime(0);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        _discard_incoming(socket, request->content_length);
        _reply_server_error(socket, request);
        return;
    }

    // Change the file size to start.
    f_lseek(&active_file, request->content_length);
    if (f_tell(&active_file) < request->content_length) {
        if (!new_file) {
            // Truncate the file back to the old length.
            f_lseek(&active_file, old_length);
            f_truncate(&active_file);
        }
        f_close(&active_file);

        if (new_file) {
            f_unlink(fs, path);
        }
        override_fattime(0);
        #if CIRCUITPY_USB_MSC
        usb_msc_unlock();
        #endif
        // Too large.
        if (request->expect) {
            _reply_expectation_failed(socket, request);
        } else {
            _discard_incoming(socket, request->content_length);
            _reply_payload_too_large(socket, request);
        }
        return;
    } else if (request->expect) {
        _reply_continue(socket, request);
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
            if (len == -MP_EAGAIN) {
                continue;
            }
            error = true;
            break;
        }
        total_read += len;
        UINT actual;
        f_write(&active_file, bytes, len, &actual);
        if (actual < (UINT)len) {
            error = true;
            break;
        }
    }

    f_close(&active_file);
    #if CIRCUITPY_USB_MSC
    usb_msc_unlock();
    #endif

    override_fattime(0);
    if (error) {
        _discard_incoming(socket, request->content_length - total_read);
        _reply_server_error(socket, request);
    } else if (new_file) {
        _reply_created(socket, request);
    } else {
        _reply_no_content(socket, request);
    }
}

#define STATIC_FILE(filename) extern uint32_t filename##_length; extern uint8_t filename[]; extern const char *filename##_content_type;

STATIC_FILE(code_html);
STATIC_FILE(directory_html);
STATIC_FILE(directory_js);
STATIC_FILE(welcome_html);
STATIC_FILE(welcome_js);
STATIC_FILE(edit_html);
STATIC_FILE(edit_js);
STATIC_FILE(style_css);
STATIC_FILE(serial_html);
STATIC_FILE(serial_js);
STATIC_FILE(blinka_32x32_ico);

static void _reply_static(socketpool_socket_obj_t *socket, _request *request, const uint8_t *response, size_t response_len, const char *content_type) {
    uint32_t total_length = response_len;
    char encoded_len[10];
    snprintf(encoded_len, sizeof(encoded_len), "%" PRIu32, total_length);

    _send_strs(socket,
        "HTTP/1.1 200 OK\r\n",
        "Content-Encoding: gzip\r\n",
        "Content-Length: ", encoded_len, "\r\n",
        "Content-Type: ", content_type, "\r\n",
        "\r\n", NULL);
    web_workflow_send_raw(socket, true, response, response_len);
}

#define _REPLY_STATIC(socket, request, filename) _reply_static(socket, request, filename, filename##_length, filename##_content_type)

static void _reply_websocket_upgrade(socketpool_socket_obj_t *socket, _request *request) {
    // Compute accept key
    hashlib_hash_obj_t hash;
    common_hal_hashlib_new(&hash, "sha1");
    common_hal_hashlib_hash_update(&hash, (const uint8_t *)request->websocket_key, strlen(request->websocket_key));
    const char *magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    common_hal_hashlib_hash_update(&hash, (const uint8_t *)magic_string, strlen(magic_string));
    size_t digest_size = common_hal_hashlib_hash_get_digest_size(&hash);
    size_t encoded_size = (digest_size + 1) * 4 / 3 + 1;
    uint8_t encoded_accept[encoded_size];
    common_hal_hashlib_hash_digest(&hash, encoded_accept, sizeof(encoded_accept));
    _base64_in_place((char *)encoded_accept, digest_size, encoded_size);

    // Reply with upgrade
    _send_strs(socket, "HTTP/1.1 101 Switching Protocols\r\n",
        "Upgrade: websocket\r\n",
        "Connection: Upgrade\r\n",
        "Sec-WebSocket-Accept: ", encoded_accept, "\r\n",
        "\r\n", NULL);
    websocket_handoff(socket);
    // socket is now closed and "disconnected".
}

static uint8_t _hex2nibble(char h) {
    if ('0' <= h && h <= '9') {
        return h - '0';
    } else if ('A' <= h && h <= 'F') {
        return h - 'A' + 0xa;
    }
    // Shouldn't usually use lower case.
    return h - 'a' + 0xa;
}

// Decode percent encoding in place. Only do this once on a string!
static void _decode_percents(char *str) {
    size_t o = 0;
    size_t i = 0;
    size_t startlen = strlen(str);
    while (i < startlen) {
        if (str[i] == '%') {
            str[o] = _hex2nibble(str[i + 1]) << 4 | _hex2nibble(str[i + 2]);
            i += 3;
        } else {
            if (i != o) {
                str[o] = str[i];
            }
            i += 1;
        }
        o += 1;
    }
    if (o < i) {
        str[o] = '\0';
    }
}

static bool _reply(socketpool_socket_obj_t *socket, _request *request) {
    if (request->redirect) {
        #if CIRCUITPY_MDNS
        if (!common_hal_mdns_server_deinited(&mdns)) {
            _reply_redirect(socket, request, request->path);
        } else {
            _reply_missing(socket, request);
        }
        #else
        _reply_missing(socket, request);
        #endif
    } else if (!_origin_ok(request)) {
        _reply_forbidden(socket, request);
    } else if (strncmp(request->path, "/fs/", 4) == 0) {
        if (strcasecmp(request->method, "OPTIONS") == 0) {
            // OPTIONS is sent for CORS preflight, unauthenticated
            _reply_access_control(socket, request);
        } else if (!request->authenticated) {
            if (_api_password[0] != '\0') {
                _reply_unauthorized(socket, request);
            } else {
                _reply_forbidden(socket, request);
            }
        } else {
            // Decode any percent encoded bytes so that we're left with UTF-8.
            // We only do this on /fs/ paths and after redirect so that any
            // path echoing we do stays encoded.
            _decode_percents(request->path);

            char *path = request->path + 3;
            size_t pathlen = strlen(path);
            FATFS *fs = filesystem_circuitpy();
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
            if (strcasecmp(request->method, "DELETE") == 0) {
                if (_usb_active()) {
                    _reply_conflict(socket, request);
                    return false;
                }

                FILINFO file;
                FRESULT result = f_stat(fs, path, &file);
                if (result == FR_OK) {
                    if ((file.fattrib & AM_DIR) != 0) {
                        result = supervisor_workflow_delete_directory_contents(fs, path);
                    }
                    if (result == FR_OK) {
                        result = f_unlink(fs, path);
                    }
                }

                #if CIRCUITPY_USB_MSC
                usb_msc_unlock();
                #endif
                if (result == FR_NO_PATH || result == FR_NO_FILE) {
                    _reply_missing(socket, request);
                } else if (result != FR_OK) {
                    _reply_server_error(socket, request);
                } else {
                    _reply_no_content(socket, request);
                    return true;
                }
            } else if (strcasecmp(request->method, "MOVE") == 0) {
                if (_usb_active()) {
                    _reply_conflict(socket, request);
                    return false;
                }

                _decode_percents(request->destination);
                char *destination = request->destination + 3;
                size_t destinationlen = strlen(destination);
                if (destination[destinationlen - 1] == '/' && destinationlen > 1) {
                    destination[destinationlen - 1] = '\0';
                }

                FRESULT result = f_rename(fs, path, destination);
                #if CIRCUITPY_USB_MSC
                usb_msc_unlock();
                #endif
                if (result == FR_EXIST) { // File exists and won't be overwritten.
                    _reply_precondition_failed(socket, request);
                } else if (result == FR_NO_PATH || result == FR_NO_FILE) { // Missing higher directories or target file.
                    _reply_missing(socket, request);
                } else if (result != FR_OK) {
                    _reply_server_error(socket, request);
                } else {
                    _reply_created(socket, request);
                    return true;
                }
            } else if (directory) {
                if (strcasecmp(request->method, "GET") == 0) {
                    FF_DIR dir;
                    FRESULT res = f_opendir(fs, &dir, path);
                    // Put the / back for replies.
                    if (pathlen > 1) {
                        path[pathlen - 1] = '/';
                    }
                    if (res != FR_OK) {
                        _reply_missing(socket, request);
                        return false;
                    }
                    if (request->json) {
                        _reply_directory_json(socket, request, &dir, request->path, path);
                    } else if (pathlen == 1) {
                        _REPLY_STATIC(socket, request, directory_html);
                    } else {
                        _reply_missing(socket, request);
                    }

                    f_closedir(&dir);
                } else if (strcasecmp(request->method, "PUT") == 0) {
                    if (_usb_active()) {
                        _reply_conflict(socket, request);
                        return false;
                    }

                    if (request->timestamp_ms > 0) {
                        DWORD fattime;
                        truncate_time(request->timestamp_ms * 1000000, &fattime);
                        override_fattime(fattime);
                    }
                    FRESULT result = supervisor_workflow_mkdir_parents(fs, path);
                    override_fattime(0);
                    #if CIRCUITPY_USB_MSC
                    usb_msc_unlock();
                    #endif
                    if (result == FR_EXIST) {
                        _reply_no_content(socket, request);
                    } else if (result == FR_NO_PATH) {
                        _reply_missing(socket, request);
                    } else if (result != FR_OK) {
                        _reply_server_error(socket, request);
                    } else {
                        _reply_created(socket, request);
                        return true;
                    }
                }
            } else { // Dealing with a file.
                if (strcasecmp(request->method, "GET") == 0) {
                    FIL active_file;
                    FRESULT result = f_open(fs, &active_file, path, FA_READ);

                    if (result != FR_OK) {
                        _reply_missing(socket, request);
                    } else {
                        _reply_with_file(socket, request, path, &active_file);
                    }

                    f_close(&active_file);
                } else if (strcasecmp(request->method, "PUT") == 0) {
                    _write_file_and_reply(socket, request, fs, path);
                    return true;
                }
            }
        }
    } else if (strcmp(request->path, "/edit/") == 0) {
        if (!request->authenticated) {
            if (_api_password[0] != '\0') {
                _reply_unauthorized(socket, request);
            } else {
                _reply_forbidden(socket, request);
            }
        } else {
            _REPLY_STATIC(socket, request, edit_html);
        }
    } else if (strcmp(request->path, "/code/") == 0) {
        _REPLY_STATIC(socket, request, code_html);
    } else if (strncmp(request->path, "/cp/", 4) == 0) {
        const char *path = request->path + 3;
        if (strcasecmp(request->method, "OPTIONS") == 0) {
            // handle preflight requests to /cp/
            _reply_access_control(socket, request);
        } else if (strcasecmp(request->method, "GET") != 0) {
            _reply_method_not_allowed(socket, request);
        } else if (strcmp(path, "/devices.json") == 0) {
            _reply_with_devices_json(socket, request);
        } else if (strcmp(path, "/version.json") == 0) {
            _reply_with_version_json(socket, request);
        } else if (strcmp(path, "/diskinfo.json") == 0) {
            _reply_with_diskinfo_json(socket, request);
        } else if (strcmp(path, "/serial/") == 0) {
            if (!request->authenticated) {
                if (_api_password[0] != '\0') {
                    _reply_unauthorized(socket, request);
                } else {
                    _reply_forbidden(socket, request);
                }
            } else if (request->websocket) {
                _reply_websocket_upgrade(socket, request);
            } else {
                _REPLY_STATIC(socket, request, serial_html);
            }
        } else {
            _reply_missing(socket, request);
        }
    } else if (strcasecmp(request->method, "GET") != 0) {
        _reply_method_not_allowed(socket, request);
    } else {
        if (strcmp(request->path, "/") == 0) {
            // TODO: Autogenerate this based on the blinka bitmap and change the
            // palette based on MAC address.
            _REPLY_STATIC(socket, request, welcome_html);
        } else if (strcmp(request->path, "/directory.js") == 0) {
            _REPLY_STATIC(socket, request, directory_js);
        } else if (strcmp(request->path, "/welcome.js") == 0) {
            _REPLY_STATIC(socket, request, welcome_js);
        } else if (strcmp(request->path, "/serial.js") == 0) {
            _REPLY_STATIC(socket, request, serial_js);
        } else if (strcmp(request->path, "/edit.js") == 0) {
            _REPLY_STATIC(socket, request, edit_js);
        } else if (strcmp(request->path, "/style.css") == 0) {
            _REPLY_STATIC(socket, request, style_css);
        } else if (strcmp(request->path, "/favicon.ico") == 0) {
            // TODO: Autogenerate this based on the blinka bitmap and change the
            // palette based on MAC address.
            _REPLY_STATIC(socket, request, blinka_32x32_ico);
        } else {
            _reply_missing(socket, request);
        }
    }
    return false;
}

static void _reset_request(_request *request) {
    request->state = STATE_METHOD;
    request->origin[0] = '\0';
    request->host[0] = '\0';
    request->content_length = 0;
    request->offset = 0;
    request->timestamp_ms = 0;
    request->redirect = false;
    request->done = false;
    request->in_progress = false;
    request->new_socket = false;
    request->authenticated = false;
    request->expect = false;
    request->json = false;
    request->websocket = false;
}

static void _process_request(socketpool_socket_obj_t *socket, _request *request) {
    bool more = true;
    bool error = false;
    uint8_t c;
    // This code assumes header lines are terminated with \r\n
    while (more && !error) {

        int len = socketpool_socket_recv_into(socket, &c, 1);
        if (len != 1) {
            more = false;
            if (len == 0 || len == -MP_ENOTCONN) {
                // Disconnect - clear 'in-progress'
                _reset_request(request);
                common_hal_socketpool_socket_close(socket);
            }
            break;
        }
        if (!request->in_progress) {
            autoreload_suspend(AUTORELOAD_SUSPEND_WEB);
            request->in_progress = true;
            request->new_socket = false;
        }
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
                    if (strcasecmp(request->header_key, "Authorization") == 0) {
                        const char *prefix = "Basic ";
                        request->authenticated = strncmp(request->header_value, prefix, strlen(prefix)) == 0 &&
                            strcmp(_api_password, request->header_value + strlen(prefix)) == 0;
                    } else if (strcasecmp(request->header_key, "Host") == 0) {
                        // Do a prefix check so that port is ignored. Length must be the same or the
                        // header ends in :.
                        const char *cp_local = "circuitpython.local";
                        request->redirect = strncmp(request->header_value, cp_local, strlen(cp_local)) == 0 &&
                            (strlen(request->header_value) == strlen(cp_local) ||
                                request->header_value[strlen(cp_local)] == ':');
                        strncpy(request->host, request->header_value, sizeof(request->host) - 1);
                        request->host[sizeof(request->host) - 1] = '\0';
                    } else if (strcasecmp(request->header_key, "Content-Length") == 0) {
                        request->content_length = strtoul(request->header_value, NULL, 10);
                    } else if (strcasecmp(request->header_key, "Expect") == 0) {
                        request->expect = strcmp(request->header_value, "100-continue") == 0;
                    } else if (strcasecmp(request->header_key, "Accept") == 0) {
                        request->json = strcasecmp(request->header_value, "application/json") == 0;
                    } else if (strcasecmp(request->header_key, "Origin") == 0) {
                        strncpy(request->origin, request->header_value, sizeof(request->origin) - 1);
                        request->origin[sizeof(request->origin) - 1] = '\0';
                    } else if (strcasecmp(request->header_key, "X-Timestamp") == 0) {
                        request->timestamp_ms = strtoull(request->header_value, NULL, 10);
                    } else if (strcasecmp(request->header_key, "Upgrade") == 0) {
                        request->websocket = strcmp(request->header_value, "websocket") == 0;
                    } else if (strcasecmp(request->header_key, "Sec-WebSocket-Version") == 0) {
                        request->websocket_version = strtoul(request->header_value, NULL, 10);
                    } else if (strcasecmp(request->header_key, "Sec-WebSocket-Key") == 0 &&
                               strlen(request->header_value) == 24) {
                        strcpy(request->websocket_key, request->header_value);
                    } else if (strcasecmp(request->header_key, "X-Destination") == 0) {
                        strcpy(request->destination, request->header_value);
                    }
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
                more = false;
                break;
        }
    }
    if (error) {
        const char *error_response = "HTTP/1.1 501 Not Implemented\r\n\r\n";

        int nodelay = 1;
        common_hal_socketpool_socket_setsockopt(socket, SOCKETPOOL_IPPROTO_TCP, SOCKETPOOL_TCP_NODELAY, &nodelay, sizeof(nodelay));
        socketpool_socket_send(socket, (const uint8_t *)error_response, strlen(error_response));
        request->done = true;
    }
    if (!request->done) {
        return;
    }
    bool reload = _reply(socket, request);
    _reset_request(request);
    common_hal_socketpool_socket_close(socket);
    autoreload_resume(AUTORELOAD_SUSPEND_WEB);
    if (reload) {
        autoreload_trigger();
    }
}


void supervisor_web_workflow_background(void *data) {
    while (true) {
        // If we have a request in progress, continue working on it. Do this first
        // so that we can accept another socket after finishing this request.
        if (common_hal_socketpool_socket_get_connected(&active)) {
            _process_request(&active, &active_request);
            if (active_request.in_progress) {
                break;
            }
        } else {
            // Close the active socket if necessary
            if (!common_hal_socketpool_socket_get_closed(&active)) {
                common_hal_socketpool_socket_close(&active);
            }
        }
        // Otherwise, see if we have another socket to accept.
        if ((!common_hal_socketpool_socket_get_connected(&active) ||
             (!active_request.in_progress && !active_request.new_socket)) &&
            !common_hal_socketpool_socket_get_closed(&listening)) {
            uint32_t ip;
            uint32_t port;
            if (!common_hal_socketpool_socket_get_closed(&active)) {
                common_hal_socketpool_socket_close(&active);
            }
            int newsoc = socketpool_socket_accept(&listening, (uint8_t *)&ip, &port, &active);
            if (newsoc == -EBADF) {
                common_hal_socketpool_socket_close(&listening);
                break;
            }
            if (newsoc > 0) {
                common_hal_socketpool_socket_settimeout(&active, 0);
                _reset_request(&active_request);
                // Mark new sockets, otherwise we may accept another before the first
                // could start its request.
                active_request.new_socket = true;
                continue;
            }
            break;
        }
    }

    // Let the websocket code run.
    websocket_background();

    // Resume polling
    socketpool_socket_poll_resume();

    return;
}

void supervisor_stop_web_workflow(void) {
}
