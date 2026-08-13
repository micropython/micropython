/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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
 *
 * MMHAL implementation for MicroPython: the SD-over-SPI transport to the Morse
 * Micro transceiver, plus the firmware and board-configuration blobs.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_HALOW

#include <string.h>

#include "py/runtime.h"
#include "extmod/modmachine.h"

#include "mmhal.h"
#include "mmhal_wlan.h"
#include "mmosal.h"
#include "halow.h"
#include "halow_osal.h"
#include "halow_sched.h"

// A request, not a setting: stm32 and mimxrt round down to the nearest rate,
// but alif truncates clk / speed and DesignWare SSI forces the divider even, so
// an unevenly-dividing request yields a *faster* bus (40MHz becomes 50MHz on a
// 200MHz AHB).  Check the rate you got.
#ifndef MICROPY_HW_HALOW_SPI_BAUDRATE
#define MICROPY_HW_HALOW_SPI_BAUDRATE   (12 * 1000 * 1000)
#endif

// Bytes clocked out with MOSI held high to stabilise the transceiver's SD-over-SPI
// state machine.  Must be at least 74 bits, see section 6.4.1.1 of "SD Physical
// Layer Simplified Specification Version 9.10".
#define HALOW_TRAINING_BYTES    (16)

// Size of the scratch buffer used to drive MOSI high during read transfers.
// Reads are chunked through it so that the driver never allocates per transfer.
#define HALOW_READ_CHUNK        (128)

// Set by boards that invert the transceiver's BUSY output before it reaches the
// MCU.  RESET_N is active low at the transceiver and is not configurable.
#ifndef MICROPY_HW_HALOW_BUSY_INVERTED
#define MICROPY_HW_HALOW_BUSY_INVERTED (0)
#endif

static mmhal_irq_handler_t halow_spi_irq_handler;
static mmhal_irq_handler_t halow_busy_irq_handler;
static volatile bool halow_spi_irq_enabled;
static volatile bool halow_busy_irq_enabled;

// Filled with 0xff at init and never written again: SD-over-SPI needs MOSI held
// high while reading, and clocking out of a fixed buffer keeps reads allocation
// free.
static uint8_t halow_spi_ones[HALOW_READ_CHUNK];

// The firmware image and, if the board supplies one, the board configuration
// file.  Both are linked in as binary blobs, see extmod.mk.
extern uint8_t halow_firmware_start;
extern uint8_t halow_firmware_end;
#ifdef MICROPY_HW_HALOW_BCF
extern uint8_t halow_bcf_start;
extern uint8_t halow_bcf_end;
#endif

/*******************************************************************************/
// Bus

static void halow_spi_transfer(size_t len, const uint8_t *src, uint8_t *dest) {
    mp_obj_t spi = MP_STATE_PORT(mp_halow_spi);
    if (spi == MP_OBJ_NULL) {
        return;
    }
    const mp_machine_spi_p_t *spi_proto = MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, protocol);
    spi_proto->transfer(spi, len, src, dest);
}

void mmhal_wlan_spi_cs_assert(void) {
    // The gap before a transaction starts is the one point in the bus path
    // where nothing is in flight, so it is where a task that has overrun its
    // turn is made to give one up.  A retry loop re-asserts CS every time round,
    // so this is always reached however long morselib intends to keep trying.
    if (halow_sched_over_budget() && !halow_osal_in_critical()) {
        halow_sched_yield();
    }
    mp_hal_pin_write(MICROPY_HW_HALOW_CS, 0);
}

void mmhal_wlan_spi_cs_deassert(void) {
    mp_hal_pin_write(MICROPY_HW_HALOW_CS, 1);
}

uint8_t mmhal_wlan_spi_rw(uint8_t data) {
    uint8_t rx = 0xff;
    halow_spi_transfer(1, &data, &rx);
    return rx;
}

void mmhal_wlan_spi_read_buf(uint8_t *buf, unsigned len) {
    // SD-over-SPI requires MOSI to be held high while reading, so clock out ones
    // from a fixed scratch buffer rather than doing a receive-only transfer.
    while (len > 0) {
        size_t chunk = MIN(len, HALOW_READ_CHUNK);
        halow_spi_transfer(chunk, halow_spi_ones, buf);
        buf += chunk;
        len -= chunk;
    }
}

void mmhal_wlan_spi_write_buf(const uint8_t *buf, unsigned len) {
    halow_spi_transfer(len, buf, NULL);
}

void mmhal_wlan_send_training_seq(void) {
    mmhal_wlan_spi_cs_deassert();
    halow_spi_transfer(HALOW_TRAINING_BYTES, halow_spi_ones, NULL);
}

/*******************************************************************************/
// Control lines

void mmhal_wlan_assert_reset(bool assert_reset) {
    mp_hal_pin_write(MICROPY_HW_HALOW_RESET, assert_reset ? 0 : 1);
}

void mmhal_wlan_hard_reset(void) {
    mmhal_wlan_assert_reset(true);
    mmosal_task_sleep(5);
    mmhal_wlan_assert_reset(false);
    mmosal_task_sleep(20);
}

void mmhal_wlan_wake_assert(void) {
    mp_hal_pin_write(MICROPY_HW_HALOW_WAKE, 1);
}

void mmhal_wlan_wake_deassert(void) {
    mp_hal_pin_write(MICROPY_HW_HALOW_WAKE, 0);
}

bool mmhal_wlan_busy_is_asserted(void) {
    // The transceiver drives BUSY high, but a board may invert it on the way to
    // the MCU, for example to share the line with a wake-up input.
    #if MICROPY_HW_HALOW_BUSY_INVERTED
    return mp_hal_pin_read(MICROPY_HW_HALOW_BUSY) == 0;
    #else
    return mp_hal_pin_read(MICROPY_HW_HALOW_BUSY) != 0;
    #endif
}

void mmhal_wlan_register_busy_irq_handler(mmhal_irq_handler_t handler) {
    halow_busy_irq_handler = handler;
}

void mmhal_wlan_set_busy_irq_enabled(bool enabled) {
    halow_busy_irq_enabled = enabled;
}

bool mmhal_wlan_spi_irq_is_asserted(void) {
    // The transceiver drives the line low while it has data pending.
    return mp_hal_pin_read(MICROPY_HW_HALOW_IRQ) == 0;
}

void mmhal_wlan_clear_spi_irq(void) {
    // The line is level driven by the transceiver, so there is nothing to clear.
}

void mmhal_wlan_register_spi_irq_handler(mmhal_irq_handler_t handler) {
    halow_spi_irq_handler = handler;
}

void mmhal_wlan_set_spi_irq_enabled(bool enabled) {
    halow_spi_irq_enabled = enabled;
    // The line is level, not edge, driven: if it is already asserted when the
    // interrupt is enabled there will be no further edge to trigger on.
    if (enabled && mmhal_wlan_spi_irq_is_asserted() && halow_spi_irq_handler != NULL) {
        halow_spi_irq_handler();
    }
}

#if MICROPY_PY_NETWORK_HALOW_PIN_IRQ
// The transceiver holds IRQ low until it is serviced, so the falling edge is the
// assert.  Waking the driver from the edge rather than waiting for the next
// network poll shaves a poll period off a round trip, which crosses that wait
// twice.  Optional: mp_hal_pin_interrupt() is not available on every port, and
// halow_hal_poll_irqs() reads the line by level anyway, so a port without it
// just waits for the next poll.
static mp_obj_t halow_irq_callback(mp_obj_t arg) {
    (void)arg;
    // Coalesce: one poll drains everything the transceiver has, so further edges
    // until then are pure overhead.  Re-armed by halow_hal_irq_rearm().
    mp_hal_pin_interrupt_enable(MICROPY_HW_HALOW_IRQ, false);
    halow_schedule_poll();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(halow_irq_callback_obj, halow_irq_callback);

static void halow_config_irq_pin(bool enabled) {
    mp_hal_pin_interrupt(MICROPY_HW_HALOW_IRQ,
        MP_OBJ_FROM_PTR(&halow_irq_callback_obj),
        enabled ? MP_HAL_PIN_TRIGGER_FALL : MP_HAL_PIN_TRIGGER_NONE,
        true);
}
#else
static void halow_config_irq_pin(bool enabled) {
    (void)enabled;
}
#endif

#if MICROPY_PY_NETWORK_HALOW_PIN_IRQ
void halow_hal_irq_rearm(void) {
    mp_hal_pin_interrupt_enable(MICROPY_HW_HALOW_IRQ, true);
}
#else
void halow_hal_irq_rearm(void) {
}
#endif

// Called from halow_poll() to pick up transceiver interrupts.  Level-checking
// here rather than relying purely on a pin interrupt keeps the driver correct on
// boards where the IRQ line is not wired to an interrupt-capable pin.
void halow_hal_poll_irqs(void) {
    if (halow_spi_irq_enabled && halow_spi_irq_handler != NULL && mmhal_wlan_spi_irq_is_asserted()) {
        halow_spi_irq_handler();
    }
    if (halow_busy_irq_enabled && halow_busy_irq_handler != NULL && mmhal_wlan_busy_is_asserted()) {
        halow_busy_irq_handler();
    }
}

/*******************************************************************************/
// Init

void mmhal_wlan_init(void) {
    memset(halow_spi_ones, 0xff, sizeof(halow_spi_ones));

    mp_hal_pin_output(MICROPY_HW_HALOW_RESET);
    mp_hal_pin_write(MICROPY_HW_HALOW_RESET, 0);
    mp_hal_pin_output(MICROPY_HW_HALOW_WAKE);
    mp_hal_pin_write(MICROPY_HW_HALOW_WAKE, 0);
    mp_hal_pin_output(MICROPY_HW_HALOW_CS);
    mp_hal_pin_write(MICROPY_HW_HALOW_CS, 1);
    mp_hal_pin_input(MICROPY_HW_HALOW_BUSY);
    mp_hal_pin_input(MICROPY_HW_HALOW_IRQ);
    halow_config_irq_pin(true);

    // Mode 0, MSB first: the transceiver's SD-over-SPI interface samples on the
    // rising edge with the clock idling low.
    mp_obj_t args[] = {
        MP_OBJ_NEW_SMALL_INT(MICROPY_HW_HALOW_SPI_ID),
        MP_OBJ_NEW_SMALL_INT(MICROPY_HW_HALOW_SPI_BAUDRATE),
        MP_OBJ_NEW_QSTR(MP_QSTR_polarity), MP_OBJ_NEW_SMALL_INT(0),
        MP_OBJ_NEW_QSTR(MP_QSTR_phase), MP_OBJ_NEW_SMALL_INT(0),
    };
    MP_STATE_PORT(mp_halow_spi) =
        MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, make_new)((mp_obj_t)&machine_spi_type, 2, 2, args);

    // Initialising the SPI peripheral may have reclaimed the CS pin.
    mp_hal_pin_output(MICROPY_HW_HALOW_CS);
    mp_hal_pin_write(MICROPY_HW_HALOW_CS, 1);

    mmhal_wlan_assert_reset(false);
}

void mmhal_wlan_deinit(void) {
    halow_config_irq_pin(false);
    halow_spi_irq_enabled = false;
    halow_busy_irq_enabled = false;
    halow_spi_irq_handler = NULL;
    halow_busy_irq_handler = NULL;

    mmhal_wlan_assert_reset(true);
    mp_hal_pin_write(MICROPY_HW_HALOW_WAKE, 0);
    MP_STATE_PORT(mp_halow_spi) = MP_OBJ_NULL;
}

#if defined(MICROPY_HW_HALOW_EXT_XTAL_INIT) && MICROPY_HW_HALOW_EXT_XTAL_INIT
bool mmhal_wlan_ext_xtal_init_is_required(void) {
    return true;
}
#endif

const struct mmhal_chip *mmhal_get_chip(void) {
    return &MICROPY_HW_HALOW_CHIP;
}

/*******************************************************************************/
// Firmware and board configuration blobs

static void halow_read_blob(const uint8_t *start, const uint8_t *end,
    uint32_t offset, uint32_t requested_len, struct mmhal_robuf *robuf) {
    robuf->buf = NULL;
    robuf->len = 0;
    robuf->free_arg = NULL;
    robuf->free_cb = NULL;

    size_t len = end - start;
    if (offset > len) {
        return;
    }
    robuf->buf = (uint8_t *)start + offset;
    robuf->len = MIN(len - offset, requested_len);
}

void mmhal_wlan_read_fw_file(uint32_t offset, uint32_t requested_len, struct mmhal_robuf *robuf) {
    halow_read_blob(&halow_firmware_start, &halow_firmware_end, offset, requested_len, robuf);
}

void mmhal_wlan_read_bcf_file(uint32_t offset, uint32_t requested_len, struct mmhal_robuf *robuf) {
    #ifdef MICROPY_HW_HALOW_BCF
    halow_read_blob(&halow_bcf_start, &halow_bcf_end, offset, requested_len, robuf);
    #else
    // Without a board configuration file the transceiver falls back to the
    // calibration data in its own OTP.
    (void)offset;
    (void)requested_len;
    robuf->buf = NULL;
    robuf->len = 0;
    robuf->free_arg = NULL;
    robuf->free_cb = NULL;
    #endif
}

/*******************************************************************************/
// Miscellaneous

void mmhal_read_mac_addr(uint8_t *mac_addr) {
    // Leave whatever the transceiver reported from its OTP in place; if that is
    // all zeroes, derive a stable locally administered address from the MCU's
    // unique ID so that the same board always joins with the same address.
    for (int i = 0; i < 6; i++) {
        if (mac_addr[i] != 0) {
            return;
        }
    }

    mp_hal_get_mac(MP_HAL_MAC_WLAN0, mac_addr);
}

uint32_t mmhal_random_u32(uint32_t min, uint32_t max) {
    uint32_t value;
    #ifdef MICROPY_PY_RANDOM_SEED_INIT_FUNC
    value = MICROPY_PY_RANDOM_SEED_INIT_FUNC;
    #else
    #error "halow requires a hardware random number generator"
    #endif
    if (max <= min) {
        return min;
    }
    uint32_t span = max - min;
    if (span == UINT32_MAX) {
        // The whole range: the count of values is 2^32, which does not fit, and
        // computing it wraps to zero.  morselib asks for exactly this when it
        // needs random bytes, and the modulo by zero left every one of them 0.
        return value;
    }
    return min + value % (span + 1);
}

void mmhal_set_deep_sleep_veto(uint8_t veto_id) {
    (void)veto_id;
}

void mmhal_clear_deep_sleep_veto(uint8_t veto_id) {
    (void)veto_id;
}

#endif // MICROPY_PY_NETWORK_HALOW
