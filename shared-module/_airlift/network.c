// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-module/_airlift/network.h"
#include "shared-module/_airlift/transport.h"

#include "py/mpconfig.h"
#include "supervisor/shared/tick.h"

#define JOIN_TIMEOUT_MS (10000)
#define JOIN_POLL_MS (100)
#define REFRESH_MS (1000)
// How often the background tries to rejoin while disconnected.
#define REJOIN_MS (30000)

static uint8_t status = AIRLIFT_WL_NO_SHIELD;
static uint32_t ipv4_address;
static uint32_t generation;
static uint64_t refreshed_ms;
static bool refreshed;

// The network from settings.toml.
static char join_ssid[33];
static char join_password[64];
// The core's last join attempt: when, in which generation, and how many WiFi
// commands VM code had sent by then.
static uint64_t join_ms;
static uint32_t join_generation;
static uint32_t join_vm_commands;
static bool join_tried;
// A background join was sent and hasn't finished yet.
static bool joining;

// Sends a command with 8-bit parameter lengths from background context, and
// reads a reply of `expected` parameters into buf.
static airlift_result_t command(uint8_t cmd, const airlift_param_t *params, size_t n_params,
    uint8_t *buf, size_t buf_len, uint16_t *lens, int expected) {
    airlift_reply_t reply = {
        .buf = buf, .buf_len = buf_len, .lens = lens, .max_count = expected,
    };
    airlift_result_t result = airlift_transact(cmd, params, n_params, false, &reply, expected, false, false);
    if (result == AIRLIFT_OK && (int)reply.count != expected) {
        return AIRLIFT_BAD_REPLY;
    }
    return result;
}

static const uint8_t ff = 0xff;
static const airlift_param_t ff_param = { .buf = &ff, .len = 1 };

static bool read_status(void) {
    uint8_t buf[1];
    uint16_t lens[1];
    if (command(AIRLIFT_GET_CONN_STATUS, NULL, 0, buf, sizeof(buf), lens, 1) != AIRLIFT_OK || lens[0] != 1) {
        return false;
    }
    status = buf[0];
    return true;
}

static void read_address(void) {
    uint8_t buf[12];
    uint16_t lens[3];
    ipv4_address = 0;
    if (status == AIRLIFT_WL_CONNECTED &&
        command(AIRLIFT_GET_IPADDR, &ff_param, 1, buf, sizeof(buf), lens, 3) == AIRLIFT_OK && lens[0] == 4) {
        memcpy(&ipv4_address, buf, 4);
    }
}

void airlift_network_refresh(bool force) {
    if (!airlift_transport_active()) {
        status = AIRLIFT_WL_NO_SHIELD;
        ipv4_address = 0;
        return;
    }
    uint64_t now = supervisor_ticks_ms64();
    // A hard reset drops the connection, so the cache is stale.
    if (!force && refreshed && generation == airlift_transport_generation() &&
        now - refreshed_ms < REFRESH_MS) {
        return;
    }
    uint32_t gen = airlift_transport_generation();
    if (!read_status()) {
        // Leave the cache alone and try again next time.
        return;
    }
    read_address();
    generation = gen;
    refreshed_ms = now;
    refreshed = true;
}

uint8_t airlift_network_status(void) {
    if (!airlift_transport_active() || generation != airlift_transport_generation()) {
        return airlift_transport_active() ? AIRLIFT_WL_IDLE_STATUS : AIRLIFT_WL_NO_SHIELD;
    }
    return status;
}

uint32_t airlift_network_ipv4_address(void) {
    return airlift_network_status() == AIRLIFT_WL_CONNECTED ? ipv4_address : 0;
}

static bool connected_to(const char *ssid) {
    if (!read_status() || status != AIRLIFT_WL_CONNECTED) {
        return false;
    }
    uint8_t buf[33];
    uint16_t lens[1];
    if (command(AIRLIFT_GET_CURR_SSID, &ff_param, 1, buf, sizeof(buf), lens, 1) != AIRLIFT_OK) {
        return false;
    }
    return lens[0] == strlen(ssid) && memcmp(buf, ssid, lens[0]) == 0;
}

// Waits without checking for Ctrl-C, running background tasks.
static void pause_ms(uint32_t ms) {
    uint64_t start = supervisor_ticks_ms64();
    while (supervisor_ticks_ms64() - start < ms) {
        RUN_BACKGROUND_TASKS;
    }
}

// Sends SET_PASSPHRASE (or SET_NET for an open network). Doesn't wait.
static bool start_join(void) {
    join_ms = supervisor_ticks_ms64();
    join_generation = airlift_transport_generation();
    join_vm_commands = airlift_transport_vm_network_commands();
    join_tried = true;
    airlift_param_t params[2] = {
        { .buf = (const uint8_t *)join_ssid, .len = strlen(join_ssid) },
        { .buf = (const uint8_t *)join_password, .len = strlen(join_password) },
    };
    bool open = join_password[0] == '\0';
    uint8_t buf[1];
    uint16_t lens[1];
    return command(open ? AIRLIFT_SET_NET : AIRLIFT_SET_PASSPHRASE, params, open ? 1 : 2,
        buf, sizeof(buf), lens, 1) == AIRLIFT_OK && buf[0] == 1;
}

bool airlift_network_join(const char *ssid, const char *password) {
    if (!airlift_transport_active()) {
        return false;
    }
    strncpy(join_ssid, ssid, sizeof(join_ssid) - 1);
    strncpy(join_password, password, sizeof(join_password) - 1);
    // Joining again, even the same network, drops every open connection.
    if (connected_to(ssid)) {
        joining = false;
        airlift_network_refresh(true);
        return true;
    }
    if (join_tried && join_generation == airlift_transport_generation()) {
        // This already failed once since the last reset. Don't hold up every
        // VM start for it; let the background try again soon, even if the last
        // VM run managed the connection itself.
        join_vm_commands = airlift_transport_vm_network_commands();
        join_ms = supervisor_ticks_ms64() - REJOIN_MS;
        joining = false;
        airlift_network_refresh(true);
        return false;
    }
    joining = false;
    if (start_join()) {
        // Like ESP32SPI's connect_AP(), wait out failures too: the status passes
        // through other values on the way to connected.
        uint64_t start = supervisor_ticks_ms64();
        while (supervisor_ticks_ms64() - start < JOIN_TIMEOUT_MS) {
            pause_ms(JOIN_POLL_MS);
            if (read_status() && status == AIRLIFT_WL_CONNECTED) {
                break;
            }
        }
    }
    airlift_network_refresh(true);
    return airlift_network_status() == AIRLIFT_WL_CONNECTED;
}

void airlift_network_maintain(void) {
    if (!airlift_transport_active() || join_ssid[0] == '\0') {
        return;
    }
    airlift_network_refresh(false);
    if (airlift_network_status() == AIRLIFT_WL_CONNECTED) {
        joining = false;
        return;
    }
    uint64_t now = supervisor_ticks_ms64();
    bool was_reset = join_generation != airlift_transport_generation();
    if (joining && !was_reset && now - join_ms < JOIN_TIMEOUT_MS) {
        return;
    }
    joining = false;
    // Leave the connection to user code that joined or left a network itself,
    // until it resets the ESP32 or the VM ends.
    bool vm_managed = join_vm_commands != airlift_transport_vm_network_commands();
    if (was_reset || (!vm_managed && now - join_ms >= REJOIN_MS)) {
        joining = start_join();
    }
}
