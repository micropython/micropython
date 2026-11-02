// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-module/_airlift/transport.h"

#include "py/mpconfig.h"
#include "py/obj.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/tick.h"

#if CIRCUITPY_TINYUSB
#include "supervisor/usb.h"
#endif

#ifndef CIRCUITPY_AIRLIFT_SPI_INDEX
#define CIRCUITPY_AIRLIFT_SPI_INDEX (0)
#endif

#define START_CMD (0xE0)
#define END_CMD (0xEE)
#define ERR_CMD (0xEF)
#define REPLY_FLAG (0x80)

// The same values as ESP32SPI.
#define SPI_BAUDRATE (8000000)
#define READY_TIMEOUT_MS (10000)
#define SELECT_TIMEOUT_MS (1000)
#define START_TRIES (10)
#define START_RETRY_MS (10)

// How long a VM caller waits for someone else to unlock the SPI bus.
#define BUS_LOCK_TIMEOUT_MS (1000)
// How long a VM GET_SOCKET holds off the core from taking a socket.
#define VM_ALLOCATING_TIMEOUT_MS (1000)
// How many core commands in a row may get a garbled reply before the ESP is
// reset.
#define CORE_FAILURES_BEFORE_RESET (3)
// How many times a command is sent when the ESP says it arrived garbled.
#define COMMAND_TRIES (2)

static busio_spi_obj_t *spi;
static digitalio_digitalinout_obj_t cs;
static digitalio_digitalinout_obj_t ready;
static digitalio_digitalinout_obj_t reset;
#ifdef CIRCUITPY_AIRLIFT_GPIO0
static digitalio_digitalinout_obj_t gpio0;
#endif
static bool active;
static volatile bool busy;
static uint32_t generation;
// Sockets that VM code started a server on since the last hard reset.
static uint8_t vm_listening;
// Sockets the core has open. airlift_user_reset() leaves these alone.
static uint8_t core_owned;
// When VM code last did GET_SOCKET without using the number yet.
static bool vm_allocating;
static uint64_t vm_allocating_ms;
static uint32_t vm_network_commands;
static uint8_t core_failures;

// Commands are built here and sent with one SPI write, as ESP32SPI does.
static uint8_t tx_buf[AIRLIFT_MAX_TRANSFER];

// Web workflow sends its commands from a background task, where
// RUN_BACKGROUND_TASKS does nothing because background tasks don't nest. Run
// USB directly too, so the host doesn't see the board stop answering while the
// ESP takes up to READY_TIMEOUT_MS to reply.
static void run_background_tasks(void) {
    RUN_BACKGROUND_TASKS;
    #if CIRCUITPY_TINYUSB
    usb_background();
    #endif
}

// Waits without checking for Ctrl-C, running background tasks.
static void pause_ms(uint32_t ms) {
    uint64_t start = supervisor_ticks_ms64();
    while (supervisor_ticks_ms64() - start < ms) {
        run_background_tasks();
    }
}

// Waits for the ESP's BUSY/READY line to read `level`. Background tasks run at
// most once per millisecond, so a quick handshake isn't slowed down by them.
static bool wait_ready(bool level, uint32_t timeout_ms, bool interruptible, bool *interrupted) {
    uint64_t start = supervisor_ticks_ms64();
    uint64_t last = start;
    while (common_hal_digitalio_digitalinout_get_value(&ready) != level) {
        uint64_t now = supervisor_ticks_ms64();
        if (now - start >= timeout_ms) {
            return false;
        }
        if (now != last) {
            last = now;
            if (interruptible && mp_hal_is_interrupted()) {
                *interrupted = true;
                return false;
            }
            run_background_tasks();
        }
    }
    return true;
}

static airlift_result_t lock_bus(bool wait, bool interruptible) {
    if (common_hal_busio_spi_deinited(spi)) {
        return AIRLIFT_NOT_ACTIVE;
    }
    uint64_t start = supervisor_ticks_ms64();
    while (!common_hal_busio_spi_try_lock(spi)) {
        if (!wait || supervisor_ticks_ms64() - start >= BUS_LOCK_TIMEOUT_MS) {
            return AIRLIFT_BUSY;
        }
        if (interruptible && mp_hal_is_interrupted()) {
            return AIRLIFT_INTERRUPTED;
        }
        run_background_tasks();
    }
    // Other devices on the bus may have changed its settings.
    common_hal_busio_spi_configure(spi, SPI_BAUDRATE, 0, 0, 8);
    common_hal_digitalio_digitalinout_set_value(&cs, false);
    return AIRLIFT_OK;
}

static void unlock_bus(void) {
    common_hal_digitalio_digitalinout_set_value(&cs, true);
    common_hal_busio_spi_unlock(spi);
}

static uint8_t read_byte(void) {
    uint8_t b = 0;
    common_hal_busio_spi_read(spi, &b, 1, 0);
    return b;
}

static bool check_byte(uint8_t expected) {
    return read_byte() == expected;
}

static void discard(size_t len) {
    uint8_t scratch[32];
    while (len > 0) {
        size_t n = MIN(len, sizeof(scratch));
        common_hal_busio_spi_read(spi, scratch, n, 0);
        len -= n;
    }
}

// Which parameter holds the socket number, or -1 for commands without one.
static int socket_param(uint8_t cmd, size_t n_params) {
    switch (cmd) {
        case 0x28: // START_SERVER_TCP: [ip,] port, socket, mode
        case 0x2D: // START_CLIENT_TCP: [host,] ip, port, socket, mode
            return n_params >= 2 ? (int)n_params - 2 : -1;
        case 0x29: // GET_STATE_TCP
        case 0x2A: // DATA_SENT_TCP
        case 0x2B: // AVAIL_DATA_TCP
        case 0x2C: // GET_DATA_TCP
        case 0x2E: // STOP_CLIENT_TCP
        case 0x2F: // GET_CLIENT_STATE_TCP
        case 0x39: // SEND_UDP_DATA
        case 0x3A: // GET_REMOTE_DATA
        case 0x44: // SEND_DATA_TCP
        case 0x45: // GET_DATABUF_TCP
        case 0x46: // INSERT_DATABUF_TCP
            return 0;
        default:
            return -1;
    }
}

// Checks the command and builds it in tx_buf. Returns its padded length in *len.
static airlift_result_t build_command(uint8_t cmd, const airlift_param_t *params, size_t n_params,
    bool param_len_16, size_t *len) {
    int sock = socket_param(cmd, n_params);
    if (sock >= 0 && (params[sock].len < 1 || params[sock].buf[0] >= CIRCUITPY_AIRLIFT_MAX_SOCKETS)) {
        return AIRLIFT_BAD_SOCKET;
    }
    if (cmd == AIRLIFT_GET_DATABUF_TCP && n_params >= 2 && params[1].len == 2 &&
        (params[1].buf[0] | (params[1].buf[1] << 8)) > AIRLIFT_MAX_READ) {
        return AIRLIFT_TOO_BIG;
    }
    if (n_params > 255) {
        return AIRLIFT_TOO_BIG;
    }
    // START, cmd, count, the params, END.
    size_t packet_len = 4;
    for (size_t i = 0; i < n_params; i++) {
        if (!param_len_16 && params[i].len > 255) {
            return AIRLIFT_TOO_BIG;
        }
        packet_len += params[i].len + (param_len_16 ? 2 : 1);
    }
    if (packet_len > AIRLIFT_MAX_TRANSFER) {
        return AIRLIFT_TOO_BIG;
    }
    uint8_t *p = tx_buf;
    *p++ = START_CMD;
    *p++ = cmd & ~REPLY_FLAG;
    *p++ = n_params;
    for (size_t i = 0; i < n_params; i++) {
        if (param_len_16) {
            *p++ = params[i].len >> 8;
        }
        *p++ = params[i].len & 0xff;
        memcpy(p, params[i].buf, params[i].len);
        p += params[i].len;
    }
    *p++ = END_CMD;
    // Pad to a multiple of 4. AIRLIFT_MAX_TRANSFER is one, so this still fits.
    while ((p - tx_buf) % 4 != 0) {
        *p++ = 0;
    }
    *len = p - tx_buf;
    return AIRLIFT_OK;
}

static airlift_result_t send_command(size_t len, bool from_vm) {
    bool interrupted = false;
    if (!wait_ready(false, READY_TIMEOUT_MS, from_vm, &interrupted)) {
        return interrupted ? AIRLIFT_INTERRUPTED : AIRLIFT_TIMEOUT_READY;
    }
    airlift_result_t result = lock_bus(from_vm, from_vm);
    if (result != AIRLIFT_OK) {
        return result;
    }
    if (!wait_ready(true, SELECT_TIMEOUT_MS, false, NULL)) {
        unlock_bus();
        return AIRLIFT_TIMEOUT_SELECT;
    }
    common_hal_busio_spi_write(spi, tx_buf, len);
    unlock_bus();
    return AIRLIFT_OK;
}

static airlift_result_t read_reply(uint8_t cmd, airlift_reply_t *reply, int expected_replies,
    bool reply_len_16) {
    // Like ESP32SPI's _wait_spi_char(): a few tries for START_CMD.
    for (int tries = 1;; tries++) {
        uint8_t b = read_byte();
        if (b == ERR_CMD) {
            return AIRLIFT_ERR_RESPONSE;
        }
        if (b == START_CMD) {
            break;
        }
        if (tries == START_TRIES) {
            return AIRLIFT_TIMEOUT_START;
        }
        pause_ms(START_RETRY_MS);
    }
    if (!check_byte(cmd | REPLY_FLAG)) {
        return AIRLIFT_BAD_REPLY;
    }
    size_t count;
    if (expected_replies >= 0) {
        if (!check_byte(expected_replies)) {
            return AIRLIFT_BAD_REPLY;
        }
        count = expected_replies;
    } else {
        count = read_byte();
    }
    bool too_big = false;
    size_t used = 0;
    reply->count = 0;
    for (size_t i = 0; i < count; i++) {
        size_t len = read_byte();
        if (reply_len_16) {
            len = (len << 8) | read_byte();
        }
        // Keep reading a reply that doesn't fit, so the framing stays in step.
        if (too_big || i >= reply->max_count || used + len > reply->buf_len) {
            too_big = true;
            discard(len);
            continue;
        }
        if (len > 0) {
            common_hal_busio_spi_read(spi, reply->buf + used, len, 0);
        }
        reply->lens[i] = len;
        used += len;
        reply->count++;
    }
    if (!check_byte(END_CMD)) {
        return AIRLIFT_BAD_REPLY;
    }
    return too_big ? AIRLIFT_REPLY_TOO_BIG : AIRLIFT_OK;
}

static airlift_result_t receive_reply(uint8_t cmd, airlift_reply_t *reply, int expected_replies,
    bool reply_len_16) {
    // The command has been sent, so the reply is read even after Ctrl-C.
    if (!wait_ready(false, READY_TIMEOUT_MS, false, NULL)) {
        return AIRLIFT_TIMEOUT_READY;
    }
    // Wait for the bus even in the background: the ESP has a reply queued.
    airlift_result_t result = lock_bus(true, false);
    if (result == AIRLIFT_BUSY) {
        // The reply can't be read, so the ESP is out of step. Start it over.
        airlift_transport_reset();
    }
    if (result != AIRLIFT_OK) {
        return result;
    }
    if (!wait_ready(true, SELECT_TIMEOUT_MS, false, NULL)) {
        unlock_bus();
        return AIRLIFT_TIMEOUT_SELECT;
    }
    result = read_reply(cmd, reply, expected_replies, reply_len_16);
    unlock_bus();
    return result;
}

// Answers a VM command about one of the core's sockets without sending it.
// User code can hold a stale number after nina-fw freed it and gave it to the
// core, and stopping or using it would break the core's connection. To user
// code the socket looks closed. Returns false for commands that can be sent.
static bool guard_core_socket(uint8_t cmd, const airlift_param_t *params, size_t n_params,
    airlift_reply_t *reply, airlift_result_t *result) {
    int sock = socket_param(cmd, n_params);
    if (sock < 0 || params[sock].len < 1 || params[sock].buf[0] >= CIRCUITPY_AIRLIFT_MAX_SOCKETS ||
        !(core_owned & (1 << params[sock].buf[0]))) {
        return false;
    }
    uint8_t value;
    switch (cmd) {
        case AIRLIFT_STOP_CLIENT_TCP:
            value = 1; // Stopped.
            break;
        case AIRLIFT_GET_CLIENT_STATE_TCP:
            value = 0; // CLOSED.
            break;
        default:
            *result = AIRLIFT_BAD_SOCKET;
            return true;
    }
    if (reply->max_count < 1 || reply->buf_len < 1) {
        *result = AIRLIFT_REPLY_TOO_BIG;
        return true;
    }
    reply->buf[0] = value;
    reply->lens[0] = 1;
    reply->count = 1;
    *result = AIRLIFT_OK;
    return true;
}

// The core can't raise, so nothing above it notices an ESP that has stopped
// answering or lost step, and every later command fails the same way. A hard
// reset closes the core's sockets, which web workflow then reopens. None of
// the commands the core sends gets ERR_CMD from a working ESP.
static void check_core_result(airlift_result_t result) {
    switch (result) {
        case AIRLIFT_OK:
        case AIRLIFT_REPLY_TOO_BIG:
            core_failures = 0;
            return;
        case AIRLIFT_TIMEOUT_READY:
        case AIRLIFT_TIMEOUT_SELECT:
            // It didn't answer for 1-10 s.
            core_failures = CORE_FAILURES_BEFORE_RESET;
            break;
        case AIRLIFT_TIMEOUT_START:
        case AIRLIFT_ERR_RESPONSE:
        case AIRLIFT_BAD_REPLY:
            core_failures++;
            break;
        default:
            // Nothing was sent.
            return;
    }
    if (core_failures >= CORE_FAILURES_BEFORE_RESET) {
        airlift_transport_reset();
    }
}

airlift_result_t airlift_transact(uint8_t cmd, const airlift_param_t *params, size_t n_params,
    bool param_len_16, airlift_reply_t *reply, int expected_replies, bool reply_len_16,
    bool from_vm) {
    if (!active) {
        return AIRLIFT_NOT_ACTIVE;
    }
    if (busy) {
        return AIRLIFT_BUSY;
    }
    size_t len;
    airlift_result_t result = build_command(cmd, params, n_params, param_len_16, &len);
    if (result != AIRLIFT_OK) {
        return result;
    }
    if (from_vm && guard_core_socket(cmd, params, n_params, reply, &result)) {
        return result;
    }
    busy = true;
    for (int tries = 0; tries < COMMAND_TRIES; tries++) {
        result = send_command(len, from_vm);
        if (result == AIRLIFT_OK) {
            result = receive_reply(cmd, reply, expected_replies, reply_len_16);
        }
        // nina-fw replies ERR_CMD before running anything when the command
        // arrived garbled (seen about once in 15000 commands under load), so it is safe to send
        // again. tx_buf still holds it.
        if (result != AIRLIFT_ERR_RESPONSE) {
            break;
        }
    }
    busy = false;
    if (!from_vm) {
        check_core_result(result);
    }
    if (from_vm && result == AIRLIFT_OK) {
        switch (cmd) {
            case AIRLIFT_GET_SOCKET:
                vm_allocating = true;
                vm_allocating_ms = supervisor_ticks_ms64();
                break;
            case AIRLIFT_START_SERVER_TCP:
                // build_command() checked the socket number, so it is in range here.
                if (n_params >= 2) {
                    vm_listening |= 1 << params[n_params - 2].buf[0];
                }
                MP_FALLTHROUGH;
            case AIRLIFT_START_CLIENT_TCP:
            case AIRLIFT_STOP_CLIENT_TCP:
                vm_allocating = false;
                break;
            case AIRLIFT_SET_NET:
            case AIRLIFT_SET_PASSPHRASE:
            case AIRLIFT_SET_AP_NET:
            case AIRLIFT_SET_AP_PASSPHRASE:
            case AIRLIFT_DISCONNECT:
            case AIRLIFT_SET_ENT_ENABLE:
                vm_network_commands++;
                break;
            default:
                break;
        }
    }
    return result;
}

uint32_t airlift_transport_vm_network_commands(void) {
    return vm_network_commands;
}

bool airlift_transport_vm_allocating(void) {
    if (vm_allocating && supervisor_ticks_ms64() - vm_allocating_ms >= VM_ALLOCATING_TIMEOUT_MS) {
        vm_allocating = false;
    }
    return vm_allocating;
}

void airlift_transport_set_owned(uint8_t sock, bool owned) {
    if (sock >= CIRCUITPY_AIRLIFT_MAX_SOCKETS) {
        return;
    }
    if (owned) {
        core_owned |= 1 << sock;
    } else {
        core_owned &= ~(1 << sock);
    }
}

void airlift_transport_leaked_listener(uint8_t sock) {
    if (sock < CIRCUITPY_AIRLIFT_MAX_SOCKETS) {
        vm_listening |= 1 << sock;
    }
}

static bool claim_esp_pin(digitalio_digitalinout_obj_t *dio, const mcu_pin_obj_t *pin) {
    dio->base.type = &digitalio_digitalinout_type;
    if (common_hal_digitalio_digitalinout_construct(dio, pin) != DIGITALINOUT_OK) {
        return false;
    }
    common_hal_digitalio_digitalinout_never_reset(dio);
    return true;
}

typedef struct {
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *mosi;
    const mcu_pin_obj_t *miso;
} spi_pins_t;

bool airlift_transport_init(void) {
    if (active) {
        return true;
    }
    // board.SPI() may already exist; then the ESP shares it with user code.
    mp_obj_t board_spi = common_hal_board_get_spi(CIRCUITPY_AIRLIFT_SPI_INDEX);
    if (board_spi == NULL || common_hal_busio_spi_deinited(MP_OBJ_TO_PTR(board_spi))) {
        static const spi_pins_t spi_pins[] = CIRCUITPY_BOARD_SPI_PIN;
        const spi_pins_t *p = &spi_pins[CIRCUITPY_AIRLIFT_SPI_INDEX];
        if (!common_hal_mcu_pin_is_free(p->clock) || !common_hal_mcu_pin_is_free(p->mosi) ||
            !common_hal_mcu_pin_is_free(p->miso)) {
            return false;
        }
    }
    if (!common_hal_mcu_pin_is_free(CIRCUITPY_AIRLIFT_CS) ||
        !common_hal_mcu_pin_is_free(CIRCUITPY_AIRLIFT_BUSY) ||
        !common_hal_mcu_pin_is_free(CIRCUITPY_AIRLIFT_RESET)) {
        return false;
    }
    #ifdef CIRCUITPY_AIRLIFT_GPIO0
    if (!common_hal_mcu_pin_is_free(CIRCUITPY_AIRLIFT_GPIO0)) {
        return false;
    }
    #endif

    spi = MP_OBJ_TO_PTR(common_hal_board_create_spi(CIRCUITPY_AIRLIFT_SPI_INDEX));
    common_hal_busio_spi_never_reset(spi);
    claim_esp_pin(&cs, CIRCUITPY_AIRLIFT_CS);
    common_hal_digitalio_digitalinout_switch_to_output(&cs, true, DRIVE_MODE_PUSH_PULL);
    claim_esp_pin(&ready, CIRCUITPY_AIRLIFT_BUSY);
    claim_esp_pin(&reset, CIRCUITPY_AIRLIFT_RESET);
    common_hal_digitalio_digitalinout_switch_to_output(&reset, true, DRIVE_MODE_PUSH_PULL);
    #ifdef CIRCUITPY_AIRLIFT_GPIO0
    claim_esp_pin(&gpio0, CIRCUITPY_AIRLIFT_GPIO0);
    #endif
    active = true;
    airlift_transport_reset();
    return true;
}

void airlift_transport_deinit(void) {
    if (!active || busy) {
        return;
    }
    active = false;
    common_hal_digitalio_digitalinout_deinit(&cs);
    common_hal_digitalio_digitalinout_deinit(&ready);
    common_hal_digitalio_digitalinout_deinit(&reset);
    #ifdef CIRCUITPY_AIRLIFT_GPIO0
    common_hal_digitalio_digitalinout_deinit(&gpio0);
    #endif
    // The bus stays as board.SPI(). It is released at the next soft reset,
    // because airlift_transport_using_spi() no longer claims it.
    spi = NULL;
    // The core's sockets can't be used any more.
    generation++;
    core_owned = 0;
}

void airlift_transport_reset(void) {
    if (!active) {
        return;
    }
    // A command started from a background task during the reset would see an ESP
    // that isn't running yet.
    bool was_busy = busy;
    busy = true;
    #ifdef CIRCUITPY_AIRLIFT_GPIO0
    // High selects a normal boot rather than the bootloader.
    common_hal_digitalio_digitalinout_switch_to_output(&gpio0, true, DRIVE_MODE_PUSH_PULL);
    #endif
    common_hal_digitalio_digitalinout_set_value(&cs, true);
    common_hal_digitalio_digitalinout_set_value(&reset, false);
    pause_ms(10);
    common_hal_digitalio_digitalinout_set_value(&reset, true);
    pause_ms(750);
    #ifdef CIRCUITPY_AIRLIFT_GPIO0
    common_hal_digitalio_digitalinout_switch_to_input(&gpio0, PULL_NONE);
    #endif
    generation++;
    vm_listening = 0;
    core_owned = 0;
    vm_allocating = false;
    core_failures = 0;
    busy = was_busy;
}

void airlift_user_reset(void) {
    if (!active) {
        return;
    }
    // User code may have exited holding the bus lock.
    common_hal_busio_spi_unlock(spi);
    vm_allocating = false;
    if (vm_listening != 0) {
        airlift_transport_reset();
        return;
    }
    for (uint8_t sock = 0; sock < CIRCUITPY_AIRLIFT_MAX_SOCKETS; sock++) {
        if (core_owned & (1 << sock)) {
            continue;
        }
        airlift_param_t param = { .buf = &sock, .len = 1 };
        uint8_t buf[1];
        uint16_t lens[1];
        airlift_reply_t reply = { .buf = buf, .buf_len = sizeof(buf), .lens = lens, .max_count = 1 };
        if (airlift_transact(AIRLIFT_STOP_CLIENT_TCP, &param, 1, false, &reply, 1, false, false) != AIRLIFT_OK) {
            // Don't wait out a timeout for every socket. A reset closes them all.
            airlift_transport_reset();
            return;
        }
    }
}

bool airlift_transport_active(void) {
    return active;
}

bool airlift_transport_busy(void) {
    return busy;
}

bool airlift_transport_bus_free(void) {
    if (!active || busy || common_hal_busio_spi_deinited(spi) || !common_hal_busio_spi_try_lock(spi)) {
        return false;
    }
    common_hal_busio_spi_unlock(spi);
    return true;
}

bool airlift_transport_has_data_ready(void) {
    #ifdef CIRCUITPY_AIRLIFT_GPIO0
    return active;
    #else
    return false;
    #endif
}

bool airlift_transport_data_ready(void) {
    #ifdef CIRCUITPY_AIRLIFT_GPIO0
    // During a reset GPIO0 is an output, held high.
    return !active || common_hal_digitalio_digitalinout_get_value(&gpio0);
    #else
    return true;
    #endif
}

uint32_t airlift_transport_generation(void) {
    return generation;
}

bool airlift_transport_using_spi(const busio_spi_obj_t *bus) {
    return active && bus == spi;
}
