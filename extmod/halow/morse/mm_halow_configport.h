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
 * The mm-halow-driver configuration for MicroPython: maps the driver's
 * integration contract (see lib/mm-halow-driver/src/mm_halow_config.h) onto
 * the MicroPython HAL, and the boards' MICROPY_HW_HALOW_MORSE_MICRO_x settings onto the
 * driver's MM_HALOW_x names.
 */
#ifndef MICROPY_INCLUDED_EXTMOD_MM_HALOW_CONFIGPORT_H
#define MICROPY_INCLUDED_EXTMOD_MM_HALOW_CONFIGPORT_H

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/mpprint.h"
#include "py/runtime.h"
#include "extmod/modnetwork.h"

// Feature selection.
#define MM_HALOW_ENABLED MICROPY_PY_NETWORK_HALOW_MORSE_MICRO
// network.HALOW integrates the driver with lwIP, so enable its lwIP netif and
// DHCP client/server (the driver defaults this off).
#define MM_HALOW_ENABLE_LWIP (1)
#if defined(MICROPY_PY_NETWORK_HALOW_PIN_IRQ) && MICROPY_PY_NETWORK_HALOW_PIN_IRQ
#define MM_HALOW_ENABLE_PIN_IRQ (1)
#endif

// Board configuration, translated from the usual MICROPY_HW_ names.  Values
// without a MICROPY_HW_ setting take the driver's defaults.
#define MM_HALOW_CS             MICROPY_HW_HALOW_MORSE_MICRO_CS
#define MM_HALOW_RESET          MICROPY_HW_HALOW_MORSE_MICRO_RESET
#define MM_HALOW_WAKE           MICROPY_HW_HALOW_MORSE_MICRO_WAKE
#define MM_HALOW_BUSY           MICROPY_HW_HALOW_MORSE_MICRO_BUSY
#define MM_HALOW_IRQ            MICROPY_HW_HALOW_MORSE_MICRO_IRQ
#define MM_HALOW_SPI_ID         MICROPY_HW_HALOW_MORSE_MICRO_SPI_ID
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_SPI_BAUDRATE
#define MM_HALOW_SPI_BAUDRATE   MICROPY_HW_HALOW_MORSE_MICRO_SPI_BAUDRATE
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_BUSY_INVERTED
#define MM_HALOW_BUSY_INVERTED  MICROPY_HW_HALOW_MORSE_MICRO_BUSY_INVERTED
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_SET_MSPLIM
#define MM_HALOW_SCHED_SET_MSPLIM MICROPY_HW_HALOW_MORSE_MICRO_SET_MSPLIM
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_EXT_XTAL_INIT
#define MM_HALOW_EXT_XTAL_INIT  MICROPY_HW_HALOW_MORSE_MICRO_EXT_XTAL_INIT
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_CHIP
#define MM_HALOW_CHIPSET        MICROPY_HW_HALOW_MORSE_MICRO_CHIP
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_HEAP_SIZE
#define MM_HALOW_HEAP_SIZE      MICROPY_HW_HALOW_MORSE_MICRO_HEAP_SIZE
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_TX_BLOCKS
#define MM_HALOW_TX_BLOCKS      MICROPY_HW_HALOW_MORSE_MICRO_TX_BLOCKS
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_RX_BLOCKS
#define MM_HALOW_RX_BLOCKS      MICROPY_HW_HALOW_MORSE_MICRO_RX_BLOCKS
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_MTU
#define MM_HALOW_MTU            MICROPY_HW_HALOW_MORSE_MICRO_MTU
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_MAX_TASKS
#define MM_HALOW_SCHED_MAX_TASKS MICROPY_HW_HALOW_MORSE_MICRO_MAX_TASKS
#endif
#ifdef MICROPY_HW_HALOW_MORSE_MICRO_STA_ADDRESS
#define MM_HALOW_STA_ADDRESS    MICROPY_HW_HALOW_MORSE_MICRO_STA_ADDRESS
#define MM_HALOW_STA_NETMASK    MICROPY_HW_HALOW_MORSE_MICRO_STA_NETMASK
#define MM_HALOW_STA_GATEWAY    MICROPY_HW_HALOW_MORSE_MICRO_STA_GATEWAY
#endif

// Atomic sections and time.
#define MM_HALOW_BEGIN_ATOMIC_SECTION() MICROPY_BEGIN_ATOMIC_SECTION()
#define MM_HALOW_END_ATOMIC_SECTION(st) MICROPY_END_ATOMIC_SECTION(st)
static inline uint32_t mm_halow_ticks_ms(void) {
    return mp_hal_ticks_ms();
}

// Pins.  The MM_HALOW_CS/RESET/WAKE/BUSY/IRQ values are MicroPython pin objects.
typedef mp_hal_pin_obj_t mm_halow_pin_t;
static inline bool mm_halow_hal_pin_read(mm_halow_pin_t pin) {
    return mp_hal_pin_read(pin);
}
static inline void mm_halow_hal_pin_write(mm_halow_pin_t pin, bool value) {
    mp_hal_pin_write(pin, value);
}
static inline void mm_halow_hal_pin_input(mm_halow_pin_t pin) {
    mp_hal_pin_input(pin);
}
static inline void mm_halow_hal_pin_output(mm_halow_pin_t pin) {
    mp_hal_pin_output(pin);
}

// Interrupt context, for the scheduler's owner tracking.
static inline bool mm_halow_in_irq(void) {
    return (__get_IPSR() & IPSR_ISR_Msk) != 0;
}

// Runs while the driver busy-waits.  Not mp_event_wait_ms(): it raises, and a
// KeyboardInterrupt thrown out of morselib's frames is unrecoverable.
#define MM_HALOW_EVENT_POLL_HOOK                                \
    do {                                                        \
        mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_ONLY);    \
        MICROPY_INTERNAL_WFE(1);                                \
    } while (0)

// Diagnostics.
#define MM_HALOW_PRINTF(...)            mp_printf(&mp_plat_print, __VA_ARGS__)
#define MM_HALOW_VPRINTF(fmt, args)     mp_vprintf(&mp_plat_print, fmt, args)
#define MM_HALOW_WEAK                   MP_WEAK

// Error codes.
#define MM_HALOW_EPERM      MP_EPERM
#define MM_HALOW_EIO        MP_EIO
#define MM_HALOW_EINVAL     MP_EINVAL
#define MM_HALOW_EAGAIN     MP_EAGAIN
#define MM_HALOW_ENOMEM     MP_ENOMEM
#define MM_HALOW_ENODEV     MP_ENODEV
#define MM_HALOW_ENOENT     MP_ENOENT
#define MM_HALOW_ENOTCONN   MP_ENOTCONN
#define MM_HALOW_ENXIO      MP_ENXIO
#define MM_HALOW_ETIMEDOUT  MP_ETIMEDOUT
#define MM_HALOW_ERANGE     MP_ERANGE
#define MM_HALOW_EOPNOTSUPP MP_EOPNOTSUPP

// DHCP hostname.
#define MM_HALOW_HOST_NAME  mod_network_hostname_data

#endif // MICROPY_INCLUDED_EXTMOD_MM_HALOW_CONFIGPORT_H
