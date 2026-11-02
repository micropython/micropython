// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mpconfig.h"

#if CIRCUITPY_SOCKETPOOL

#include <string.h>

#include "shared-module/_airlift/socketpool.h"
#include "shared-module/_airlift/network.h"
#include "shared-module/_airlift/transport.h"

#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/socketpool/__init__.h"
#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "supervisor/workflow.h"

// Background pass intervals, while a client is connected and while only the
// listening socket is open.
#define POLL_ACTIVE_MS (20)
#define POLL_IDLE_MS (100)
// The idle interval when nina-fw signals data ready on GPIO0. A pass then only
// keeps the WiFi status current.
#define POLL_IDLE_DATA_READY_MS (500)
// Shortest time between passes when data ready rises.
#define POLL_MIN_MS (5)
// How long a socket's connection state is trusted before asking the ESP32 again.
// Web workflow asks several times per pass.
#define CONNECTED_CACHE_MS (10)
// How long to wait before asking the ESP32 for data again, after it had none.
// Without this, a loop polling the serial websocket would keep the bus busy.
#define AVAIL_RETRY_MS (2)
// How long close() waits without progress for the ESP32 to take the rest of the
// TX buffer.
#define DRAIN_TIMEOUT_MS (1000)

#define TCP_MODE (0)
#define SOCKET_CLOSED (0)
#define SOCKET_ESTABLISHED (4)

const mp_obj_base_t airlift_socketpool_interface = { &mp_type_object };

// The open socket object for each ESP32 socket number.
static socketpool_socket_obj_t *by_num[CIRCUITPY_AIRLIFT_MAX_SOCKETS];
// Sockets closed when the ESP32 couldn't be reached. They are stopped in the
// next background pass.
static uint8_t stop_pending;
static uint32_t stop_pending_generation;
// A listener the core closed. nina-fw can't stop a listener, so it is left
// running and handed back to the next listen() on the same port.
static uint8_t parked_num = AIRLIFT_NO_SOCKET;
static uint16_t parked_port;
static uint32_t parked_generation;

static volatile bool poll_wanted;
static volatile uint32_t poll_countdown;
static volatile uint32_t ms_since_pass;
static bool last_data_ready;
static bool tick_enabled;

static bool is_open(const socketpool_socket_obj_t *self) {
    return self->num != AIRLIFT_NO_SOCKET && airlift_transport_active() &&
           self->generation == airlift_transport_generation();
}

// A command can be sent now. It can't while another one is in flight, which
// happens when this is called from a background task run during its waits.
static bool can_transact(void) {
    return airlift_transport_active() && !airlift_transport_busy();
}

static void set_num(socketpool_socket_obj_t *self, uint8_t num) {
    self->num = num;
    self->generation = airlift_transport_generation();
    by_num[num] = self;
    airlift_transport_set_owned(num, true);
}

// Forgets the socket number without telling the ESP32.
static void forget(socketpool_socket_obj_t *self) {
    if (self->num < CIRCUITPY_AIRLIFT_MAX_SOCKETS && by_num[self->num] == self) {
        by_num[self->num] = NULL;
        // After a hard reset the number may belong to someone else.
        if (self->generation == airlift_transport_generation()) {
            airlift_transport_set_owned(self->num, false);
        }
    }
    self->num = AIRLIFT_NO_SOCKET;
    self->listening = false;
    self->connected = false;
    self->connected_valid = false;
    self->rx_head = 0;
    self->rx_len = 0;
    self->tx_len = 0;
}

// Sends a command about one socket and reads its one-parameter reply. *reply_len
// is the reply's length.
static airlift_result_t command(uint8_t cmd, const airlift_param_t *params, size_t n_params,
    bool param_len_16, uint8_t *buf, size_t buf_len, bool reply_len_16, size_t *reply_len) {
    uint16_t lens[1];
    airlift_reply_t reply = { .buf = buf, .buf_len = buf_len, .lens = lens, .max_count = 1 };
    airlift_result_t result = airlift_transact(cmd, params, n_params, param_len_16, &reply, 1,
        reply_len_16, false);
    *reply_len = reply.count == 1 ? lens[0] : 0;
    return result;
}

static airlift_result_t socket_command(uint8_t cmd, uint8_t num, uint8_t *buf, size_t buf_len,
    size_t *reply_len) {
    airlift_param_t param = { .buf = &num, .len = 1 };
    return command(cmd, &param, 1, false, buf, buf_len, false, reply_len);
}

static uint16_t le16(const uint8_t *buf, size_t len) {
    return len >= 2 ? buf[0] | (buf[1] << 8) : (len == 1 ? buf[0] : 0);
}

static bool stop(uint8_t num) {
    uint8_t buf[1];
    size_t len;
    return socket_command(AIRLIFT_STOP_CLIENT_TCP, num, buf, sizeof(buf), &len) == AIRLIFT_OK;
}

// Stops the connection and forgets the number. If the ESP32 can't be reached,
// it is stopped in the next background pass.
static void stop_and_forget(socketpool_socket_obj_t *self) {
    uint8_t num = self->num;
    bool stopped = can_transact() && stop(num);
    forget(self);
    if (!stopped) {
        // Until then it stays the core's, so airlift_user_reset() doesn't stop it
        // after someone else is given it.
        airlift_transport_set_owned(num, true);
        if (stop_pending == 0) {
            stop_pending_generation = airlift_transport_generation();
        }
        stop_pending |= 1 << num;
    }
}

// Sends the TX buffer. Returns 0 when it's empty, -MP_EAGAIN if the ESP32 can't
// be reached right now, -MP_ENOBUFS if the ESP32 had no room for all of it (the
// rest stays here), or -MP_ENOTCONN if the connection is gone or was ended
// because the send failed.
static int flush(socketpool_socket_obj_t *self) {
    if (self->tx_len == 0) {
        return 0;
    }
    if (!can_transact()) {
        return -MP_EAGAIN;
    }
    airlift_param_t params[2] = {
        { .buf = &self->num, .len = 1 },
        { .buf = self->tx, .len = self->tx_len },
    };
    uint8_t buf[2];
    size_t len;
    airlift_result_t result = command(AIRLIFT_SEND_DATA_TCP, params, 2, true, buf, sizeof(buf), false, &len);
    if (result == AIRLIFT_BUSY) {
        return -MP_EAGAIN;
    }
    // The reply is the count sent. DATA_SENT_TCP isn't asked: it says yes even
    // when nothing went out.
    size_t sent = result == AIRLIFT_OK ? MIN(le16(buf, len), self->tx_len) : 0;
    if (sent == self->tx_len) {
        self->tx_len = 0;
        return 0;
    }
    // nina-fw's send doesn't wait for room in its TCP send buffer.
    if (sent > 0) {
        memmove(self->tx, self->tx + sent, self->tx_len - sent);
        self->tx_len -= sent;
        return -MP_ENOBUFS;
    }
    if (result != AIRLIFT_OK) {
        // Whether the ESP32 sent any of it is unknown, so the stream may have a
        // hole now. End the connection rather than carry on.
        self->tx_len = 0;
        stop_and_forget(self);
        return -MP_ENOTCONN;
    }
    // Nothing went out: the ESP32 has no room, or the connection is gone.
    self->connected_valid = false;
    if (common_hal_socketpool_socket_get_connected(self)) {
        return -MP_ENOBUFS;
    }
    self->tx_len = 0;
    return -MP_ENOTCONN;
}

// Sends all of the TX buffer, waiting while the ESP32 has no room for it.
static void drain(socketpool_socket_obj_t *self) {
    uint64_t progress_ms = supervisor_ticks_ms64();
    uint16_t left = self->tx_len;
    while (flush(self) == -MP_ENOBUFS) {
        uint64_t now = supervisor_ticks_ms64();
        if (self->tx_len < left) {
            left = self->tx_len;
            progress_ms = now;
        } else if (now - progress_ms >= DRAIN_TIMEOUT_MS) {
            break;
        }
        port_task_sleep_ms(1);
    }
}

static void flush_all(void) {
    for (uint8_t num = 0; num < CIRCUITPY_AIRLIFT_MAX_SOCKETS; num++) {
        socketpool_socket_obj_t *sock = by_num[num];
        if (sock != NULL && is_open(sock) && sock->tx_len > 0) {
            flush(sock);
        }
    }
}

static bool any_client_open(void) {
    for (size_t i = 0; i < CIRCUITPY_AIRLIFT_MAX_SOCKETS; i++) {
        if (by_num[i] != NULL && is_open(by_num[i]) && !by_num[i]->listening) {
            return true;
        }
    }
    return false;
}

void airlift_socketpool_poll_resume(void) {
    if (!tick_enabled) {
        supervisor_enable_tick();
        tick_enabled = true;
    }
    if (any_client_open()) {
        poll_countdown = POLL_ACTIVE_MS;
    } else if (airlift_transport_has_data_ready()) {
        poll_countdown = POLL_IDLE_DATA_READY_MS;
    } else {
        poll_countdown = POLL_IDLE_MS;
    }
    ms_since_pass = 0;
    poll_wanted = true;
}

void airlift_socketpool_tick(void) {
    if (!poll_wanted) {
        return;
    }
    ms_since_pass++;
    if (airlift_transport_has_data_ready()) {
        // A pass runs soon after data ready rises. While it stays high, for
        // example because user code hasn't read its socket, the usual intervals
        // apply, but never the long idle one.
        bool ready = airlift_transport_data_ready();
        if (ready && !last_data_ready) {
            uint32_t soonest = ms_since_pass >= POLL_MIN_MS ? 1 : POLL_MIN_MS - ms_since_pass;
            poll_countdown = MIN(poll_countdown, soonest);
        } else if (ready) {
            poll_countdown = MIN(poll_countdown, POLL_IDLE_MS);
        }
        last_data_ready = ready;
    }
    if (--poll_countdown == 0) {
        poll_wanted = false;
        supervisor_workflow_request_background();
    }
}

bool airlift_socketpool_background(void) {
    if (!can_transact() || !airlift_transport_bus_free()) {
        return false;
    }
    airlift_network_maintain();
    if (stop_pending_generation != airlift_transport_generation()) {
        stop_pending = 0;
    }
    for (uint8_t num = 0; num < CIRCUITPY_AIRLIFT_MAX_SOCKETS; num++) {
        if ((stop_pending & (1 << num)) && stop(num)) {
            stop_pending &= ~(1 << num);
            airlift_transport_set_owned(num, false);
        }
    }
    // Serial output written by the VM since the last pass.
    flush_all();
    return true;
}

void airlift_socketpool_background_done(void) {
    // TCP_NODELAY doesn't send right away (most replies would go out in many
    // small pieces), so this sends what the pass left.
    if (can_transact()) {
        flush_all();
    }
}

void socketpool_user_reset(void) {
    // Python can't make AirLift sockets yet, so there are none to close.
}

void common_hal_socketpool_socketpool_construct(socketpool_socketpool_obj_t *self, mp_obj_t network_interface) {
    if (network_interface != MP_OBJ_FROM_PTR(&airlift_socketpool_interface)) {
        mp_arg_error_invalid(MP_QSTR_radio);
    }
}

bool socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type,
    int proto, socketpool_socket_obj_t *sock) {
    if (family != SOCKETPOOL_AF_INET || type != SOCKETPOOL_SOCK_STREAM) {
        return false;
    }
    sock->base.type = &socketpool_socket_type;
    sock->pool = self;
    sock->num = AIRLIFT_NO_SOCKET;
    sock->type = type;
    sock->timeout_ms = (uint32_t)-1;
    sock->port = 0;
    sock->next_avail_ms = 0;
    forget(sock);
    return true;
}

socketpool_socket_obj_t *common_hal_socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type, int proto) {
    socketpool_socket_obj_t *sock = mp_obj_malloc_with_finaliser(socketpool_socket_obj_t, &socketpool_socket_type);
    if (!socketpool_socket(self, family, type, proto, sock)) {
        mp_raise_NotImplementedError(NULL);
    }
    return sock;
}

mp_obj_t common_hal_socketpool_getaddrinfo_raise(socketpool_socketpool_obj_t *self, const char *host, int port, int family, int type, int proto, int flags) {
    mp_raise_NotImplementedError(NULL);
}

// Waits for a VM caller with a timeout. Returns false when it's time to give up.
static bool keep_waiting(const socketpool_socket_obj_t *self, uint64_t start) {
    if (self->timeout_ms == 0 || mp_hal_is_interrupted()) {
        return false;
    }
    if (self->timeout_ms != (uint32_t)-1 && supervisor_ticks_ms64() - start >= self->timeout_ms) {
        return false;
    }
    RUN_BACKGROUND_TASKS;
    return true;
}

static int try_accept(socketpool_socket_obj_t *self, socketpool_socket_obj_t *accepted) {
    // A VM GET_SOCKET is about to use the lowest free number, which is the one
    // nina-fw would give a new client.
    if (!can_transact() || airlift_transport_vm_allocating()) {
        return -MP_EAGAIN;
    }
    uint8_t buf[2];
    size_t len;
    // On a listening socket, AVAIL_DATA_TCP accepts a waiting client, and
    // replies with its socket number.
    if (socket_command(AIRLIFT_AVAIL_DATA_TCP, self->num, buf, sizeof(buf), &len) != AIRLIFT_OK) {
        return -MP_EAGAIN;
    }
    uint16_t num = le16(buf, len);
    if (num >= CIRCUITPY_AIRLIFT_MAX_SOCKETS) {
        return -MP_EAGAIN;
    }
    if (!common_hal_socketpool_socket_get_closed(accepted)) {
        common_hal_socketpool_socket_close(accepted);
    }
    accepted->base.type = &socketpool_socket_type;
    accepted->pool = self->pool;
    accepted->type = SOCKETPOOL_SOCK_STREAM;
    accepted->timeout_ms = self->timeout_ms;
    accepted->port = self->port;
    accepted->next_avail_ms = 0;
    forget(accepted);
    set_num(accepted, num);
    accepted->connected = true;
    accepted->connected_valid = true;
    accepted->connected_ms = supervisor_ticks_ms64();
    return 1;
}

int socketpool_socket_accept(socketpool_socket_obj_t *self, mp_obj_t *peer_out, socketpool_socket_obj_t *accepted) {
    if (!is_open(self) || !self->listening) {
        return -MP_EBADF;
    }
    uint64_t start = supervisor_ticks_ms64();
    int result;
    while ((result = try_accept(self, accepted)) == -MP_EAGAIN && keep_waiting(self, start)) {
    }
    // nina-fw doesn't say who the peer is.
    if (result > 0 && peer_out != NULL) {
        *peer_out = mp_const_none;
    }
    return result;
}

socketpool_socket_obj_t *common_hal_socketpool_socket_accept(socketpool_socket_obj_t *self, mp_obj_t *peer_out) {
    socketpool_socket_obj_t *accepted = mp_obj_malloc_with_finaliser(socketpool_socket_obj_t, &socketpool_socket_type);
    accepted->num = AIRLIFT_NO_SOCKET;
    int result = socketpool_socket_accept(self, peer_out, accepted);
    if (result < 0) {
        m_del_obj(socketpool_socket_obj_t, accepted);
        mp_raise_OSError(-result);
    }
    return accepted;
}

size_t common_hal_socketpool_socket_bind(socketpool_socket_obj_t *self, const char *host, size_t hostlen, uint32_t port) {
    // nina-fw listens on every interface.
    self->port = port;
    return 0;
}

void socketpool_socket_close(socketpool_socket_obj_t *self) {
    if (!is_open(self)) {
        forget(self);
        return;
    }
    uint8_t num = self->num;
    if (self->listening) {
        // STOP_CLIENT_TCP would free the number but leave the listener
        // running, so keep it, still owned, for the next listen() on this port.
        if (parked_num != AIRLIFT_NO_SOCKET && parked_generation == airlift_transport_generation()) {
            // Only one is kept. The other is stopped at the end of the VM run.
            airlift_transport_set_owned(parked_num, false);
            airlift_transport_leaked_listener(parked_num);
        }
        parked_num = num;
        parked_port = self->port;
        parked_generation = airlift_transport_generation();
        by_num[num] = NULL;
        self->num = AIRLIFT_NO_SOCKET;
        self->listening = false;
        return;
    }
    // Web workflow closes as soon as a reply is written, so this is often the
    // end of it.
    drain(self);
    // A failed send may have ended it already.
    if (is_open(self)) {
        stop_and_forget(self);
    }
}

void common_hal_socketpool_socket_close(socketpool_socket_obj_t *self) {
    socketpool_socket_close(self);
}

void common_hal_socketpool_socket_connect(socketpool_socket_obj_t *self, const char *host, size_t hostlen, uint32_t port) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_socketpool_socket_get_closed(socketpool_socket_obj_t *self) {
    return !is_open(self);
}

bool common_hal_socketpool_socket_get_connected(socketpool_socket_obj_t *self) {
    if (!is_open(self) || self->listening) {
        return false;
    }
    uint64_t now = supervisor_ticks_ms64();
    if ((self->connected_valid && now - self->connected_ms < CONNECTED_CACHE_MS) || !can_transact()) {
        return self->connected;
    }
    uint8_t buf[1];
    size_t len;
    if (socket_command(AIRLIFT_GET_CLIENT_STATE_TCP, self->num, buf, sizeof(buf), &len) != AIRLIFT_OK ||
        len != 1) {
        return self->connected;
    }
    self->connected_ms = now;
    self->connected_valid = true;
    self->connected = buf[0] == SOCKET_ESTABLISHED;
    if (buf[0] == SOCKET_CLOSED) {
        // nina-fw 3.1.0 to 3.3.0 don't free the number of a client a server
        // accepted when asked its state (3.3.0 frees only clients it connected
        // itself), so each client that hangs up first would keep its number
        // until a hard reset. Once all six are kept, every new connection is
        // reset. STOP_CLIENT_TCP frees it, and does nothing to a free number.
        stop_and_forget(self);
    }
    return self->connected;
}

mp_uint_t common_hal_socketpool_socket_get_timeout(socketpool_socket_obj_t *self) {
    return self->timeout_ms;
}

mp_int_t common_hal_socketpool_socket_get_type(socketpool_socket_obj_t *self) {
    return self->type;
}

bool common_hal_socketpool_socket_listen(socketpool_socket_obj_t *self, int backlog) {
    if (self->type != SOCKETPOOL_SOCK_STREAM || is_open(self) || !can_transact() ||
        airlift_transport_vm_allocating()) {
        return false;
    }
    if (parked_num != AIRLIFT_NO_SOCKET && parked_generation == airlift_transport_generation() &&
        parked_port == self->port) {
        // Still listening.
        set_num(self, parked_num);
        self->listening = true;
        parked_num = AIRLIFT_NO_SOCKET;
        return true;
    }
    // START_SERVER_TCP before WiFi is up hangs nina-fw until a hard reset.
    airlift_network_refresh(false);
    if (airlift_network_status() != AIRLIFT_WL_CONNECTED) {
        return false;
    }
    // GET_SOCKET doesn't reserve the number. START_SERVER_TCP does.
    uint8_t buf[1];
    size_t len;
    if (command(AIRLIFT_GET_SOCKET, NULL, 0, false, buf, sizeof(buf), false, &len) != AIRLIFT_OK ||
        len != 1 || buf[0] >= CIRCUITPY_AIRLIFT_MAX_SOCKETS) {
        return false;
    }
    uint8_t num = buf[0];
    uint8_t port[2] = { self->port >> 8, self->port & 0xff };
    uint8_t mode = TCP_MODE;
    airlift_param_t params[3] = {
        { .buf = port, .len = 2 },
        { .buf = &num, .len = 1 },
        { .buf = &mode, .len = 1 },
    };
    // A reply of 0 means the port is in use.
    if (command(AIRLIFT_START_SERVER_TCP, params, 3, false, buf, sizeof(buf), false, &len) != AIRLIFT_OK ||
        len != 1 || buf[0] != 1) {
        return false;
    }
    set_num(self, num);
    self->listening = true;
    return true;
}

mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t *self,
    uint8_t *buf, uint32_t len, mp_obj_t *peer_out) {
    mp_raise_NotImplementedError(NULL);
}

// What a read returns when a command fails. Only a busy transport is worth
// trying again; other failures would repeat, and once they reset the ESP32 the
// socket is gone.
static int recv_failed(socketpool_socket_obj_t *self, airlift_result_t result) {
    if (result == AIRLIFT_BUSY || result == AIRLIFT_INTERRUPTED) {
        return -MP_EAGAIN;
    }
    return is_open(self) ? -MP_EIO : -MP_ENOTCONN;
}

// Reads from the ESP32. Large reads go straight into buf; small ones fill the
// RX buffer first.
static int try_recv(socketpool_socket_obj_t *self, uint8_t *buf, uint32_t len) {
    if (self->rx_len > 0) {
        size_t n = MIN(len, self->rx_len);
        memcpy(buf, self->rx + self->rx_head, n);
        self->rx_head += n;
        self->rx_len -= n;
        return n;
    }
    if (!can_transact()) {
        return -MP_EAGAIN;
    }
    // The peer may be waiting for what's buffered before it sends more.
    flush(self);
    uint64_t now = supervisor_ticks_ms64();
    if (now < self->next_avail_ms) {
        return -MP_EAGAIN;
    }
    uint8_t avail_buf[2];
    size_t reply_len;
    airlift_result_t result = socket_command(AIRLIFT_AVAIL_DATA_TCP, self->num, avail_buf, sizeof(avail_buf), &reply_len);
    if (result != AIRLIFT_OK) {
        return recv_failed(self, result);
    }
    uint16_t avail = le16(avail_buf, reply_len);
    if (avail == 0) {
        self->next_avail_ms = now + AVAIL_RETRY_MS;
        return common_hal_socketpool_socket_get_connected(self) ? -MP_EAGAIN : -MP_ENOTCONN;
    }
    bool direct = len >= AIRLIFT_SOCKET_RX_SIZE;
    uint8_t *dest = direct ? buf : self->rx;
    uint16_t want = MIN(avail, direct ? MIN(len, AIRLIFT_MAX_READ) : AIRLIFT_SOCKET_RX_SIZE);
    uint8_t want_le[2] = { want & 0xff, want >> 8 };
    airlift_param_t params[2] = {
        { .buf = &self->num, .len = 1 },
        { .buf = want_le, .len = 2 },
    };
    result = command(AIRLIFT_GET_DATABUF_TCP, params, 2, true, dest, want, true, &reply_len);
    if (result != AIRLIFT_OK) {
        return recv_failed(self, result);
    }
    if (reply_len == 0) {
        return -MP_EAGAIN;
    }
    if (direct) {
        return reply_len;
    }
    self->rx_head = 0;
    self->rx_len = reply_len;
    return try_recv(self, buf, len);
}

int socketpool_socket_recv_into(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    if (!is_open(self) || self->listening) {
        return -MP_ENOTCONN;
    }
    if (len == 0) {
        return 0;
    }
    uint64_t start = supervisor_ticks_ms64();
    int result;
    while ((result = try_recv(self, (uint8_t *)buf, len)) == -MP_EAGAIN && keep_waiting(self, start)) {
    }
    return result;
}

mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int received = socketpool_socket_recv_into(self, buf, len);
    if (received < 0) {
        mp_raise_OSError(-received);
    }
    return received;
}

int socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    if (!is_open(self) || self->listening) {
        return -MP_ENOTCONN;
    }
    size_t total = 0;
    while (total < len) {
        if (self->tx_len == AIRLIFT_SOCKET_TX_SIZE) {
            int result = flush(self);
            if (result == -MP_EAGAIN) {
                // Another command is in flight (this is a background task run
                // during its waits) or another device holds the bus. Waiting
                // here would never end, so the rest is dropped.
                return len;
            }
            if (result == -MP_ENOTCONN) {
                return result;
            }
            if (self->tx_len == AIRLIFT_SOCKET_TX_SIZE) {
                // The ESP32 has no room yet. The caller tries again.
                return total > 0 ? (int)total : -MP_EAGAIN;
            }
        }
        size_t n = MIN(len - total, (size_t)(AIRLIFT_SOCKET_TX_SIZE - self->tx_len));
        memcpy(self->tx + self->tx_len, buf + total, n);
        self->tx_len += n;
        total += n;
    }
    return len;
}

mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int sent = socketpool_socket_send(self, buf, len);
    if (sent < 0) {
        mp_raise_OSError(-sent);
    }
    return sent;
}

mp_uint_t common_hal_socketpool_socket_sendto(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port, const uint8_t *buf, uint32_t len) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t *self, uint32_t timeout_ms) {
    self->timeout_ms = timeout_ms;
}

int common_hal_socketpool_socket_setsockopt(socketpool_socket_obj_t *self, int level, int optname, const void *value, size_t optlen) {
    if (level != SOCKETPOOL_IPPROTO_TCP || optname != SOCKETPOOL_TCP_NODELAY || optlen != sizeof(int)) {
        return -MP_EOPNOTSUPP;
    }
    // Accepted, but nothing is sent right away: the buffer goes out before
    // the next read and at the end of the background pass.
    return 0;
}

bool common_hal_socketpool_readable(socketpool_socket_obj_t *self) {
    if (!is_open(self) || self->listening) {
        return false;
    }
    if (self->rx_len > 0) {
        return true;
    }
    flush(self);
    uint8_t buf[2];
    size_t len;
    return can_transact() &&
           socket_command(AIRLIFT_AVAIL_DATA_TCP, self->num, buf, sizeof(buf), &len) == AIRLIFT_OK &&
           le16(buf, len) > 0;
}

bool common_hal_socketpool_writable(socketpool_socket_obj_t *self) {
    return common_hal_socketpool_socket_get_connected(self);
}

void socketpool_socket_move(socketpool_socket_obj_t *self, socketpool_socket_obj_t *sock) {
    *sock = *self;
    if (sock->num < CIRCUITPY_AIRLIFT_MAX_SOCKETS && by_num[sock->num] == self) {
        by_num[sock->num] = sock;
    }
    self->num = AIRLIFT_NO_SOCKET;
    self->tx_len = 0;
    self->rx_len = 0;
}

void socketpool_socket_reset(socketpool_socket_obj_t *self) {
    if (self->base.type == &socketpool_socket_type) {
        return;
    }
    self->base.type = &socketpool_socket_type;
    self->num = AIRLIFT_NO_SOCKET;
    self->listening = false;
    self->connected = false;
    self->connected_valid = false;
    self->rx_len = 0;
    self->tx_len = 0;
}

#endif // CIRCUITPY_SOCKETPOOL
