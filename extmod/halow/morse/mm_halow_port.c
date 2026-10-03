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
 * The mm_halow_port_* functions the mm-halow driver requires from its host:
 * the SPI bus via machine.SPI, the driver's private heap backed by the
 * MicroPython heap (held live through a GC root pointer), the hardware RNG,
 * the fallback MAC address, and the optional edge interrupt on the IRQ line.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_HALOW_MORSE_MICRO

#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "py/runtime.h"
#include "extmod/modmachine.h"

#include "mm_halow.h"

/*******************************************************************************/
// SPI bus

void mm_halow_port_spi_transfer(size_t len, const uint8_t *src, uint8_t *dest) {
    mp_obj_t spi = MP_STATE_PORT(mm_halow_spi);
    if (spi == MP_OBJ_NULL) {
        return;
    }
    const mp_machine_spi_p_t *spi_proto = MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, protocol);
    spi_proto->transfer(spi, len, src, dest);
}

void mm_halow_port_spi_init(void) {
    mp_obj_t args[] = {
        MP_OBJ_NEW_SMALL_INT(MM_HALOW_SPI_ID),
        MP_OBJ_NEW_SMALL_INT(MM_HALOW_SPI_BAUDRATE),
        MP_OBJ_NEW_QSTR(MP_QSTR_polarity), MP_OBJ_NEW_SMALL_INT(0),
        MP_OBJ_NEW_QSTR(MP_QSTR_phase), MP_OBJ_NEW_SMALL_INT(0),
    };
    MP_STATE_PORT(mm_halow_spi) =
        MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, make_new)((mp_obj_t)&machine_spi_type, 2, 2, args);
}

void mm_halow_port_spi_deinit(void) {
    MP_STATE_PORT(mm_halow_spi) = MP_OBJ_NULL;
}

/*******************************************************************************/
// Private heap backing

uint8_t *mm_halow_port_heap_alloc(size_t size) {
    // Held by a root pointer so the GC keeps it alive: morselib allocates out
    // of this block from PendSV context, where the GC must not run.
    uint8_t *heap = m_malloc_maybe(size);
    MP_STATE_PORT(mm_halow_heap) = heap;
    return heap;
}

void mm_halow_port_heap_free(uint8_t *ptr) {
    m_free(ptr);
    MP_STATE_PORT(mm_halow_heap) = NULL;
}

/*******************************************************************************/
// Entropy and identity

uint32_t mm_halow_port_random_u32(void) {
    #ifdef MICROPY_PY_RANDOM_SEED_INIT_FUNC
    return MICROPY_PY_RANDOM_SEED_INIT_FUNC;
    #else
    #error "network.HALOW requires a hardware random number generator"
    #endif
}

void mm_halow_port_get_mac(uint8_t mac_addr[6]) {
    mp_hal_get_mac(MP_HAL_MAC_WLAN0, mac_addr);
}

/*******************************************************************************/
// Optional edge interrupt on the IRQ line

#if MICROPY_PY_NETWORK_HALOW_PIN_IRQ
static mp_obj_t mm_halow_irq_callback(mp_obj_t arg) {
    (void)arg;
    mm_halow_port_irq_handler();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mm_halow_irq_callback_obj, mm_halow_irq_callback);

void mm_halow_port_irq_config(bool enabled) {
    mp_hal_pin_interrupt(MM_HALOW_IRQ,
        MP_OBJ_FROM_PTR(&mm_halow_irq_callback_obj),
        enabled ? MP_HAL_PIN_TRIGGER_FALL : MP_HAL_PIN_TRIGGER_NONE,
        true);
}

void mm_halow_port_irq_enable(bool enabled) {
    mp_hal_pin_interrupt_enable(MM_HALOW_IRQ, enabled);
}
#endif // MICROPY_PY_NETWORK_HALOW_PIN_IRQ

/*******************************************************************************/
// Fatal error

void mm_halow_port_assert_fail(void) {
    mp_printf(&mp_plat_print, "halow: assertion failed\n");
    #ifdef MICROPY_BOARD_FATAL_ERROR
    MICROPY_BOARD_FATAL_ERROR("halow assertion failed");
    #endif
    for (;;) {
    }
}

/*******************************************************************************/
// newlib back-end stubs for the prebuilt morselib.
//
// morselib is compiled against newlib and pulls in a few C library functions
// (sscanf, qsort, setjmp, ...) that transitively reference newlib's syscall
// back-end.  MicroPython links -nostdlib and uses its own heap, so nothing else
// provides these.  morselib never calls them at run time, so the stubs only
// exist to satisfy the link; _sbrk deliberately fails rather than hand out any
// of MicroPython's memory.  They are weak so a port with real implementations
// takes precedence.

MP_WEAK void *_sbrk(ptrdiff_t incr) {
    (void)incr;
    errno = ENOMEM;
    return (void *)-1;
}

MP_WEAK int _write(int fd, const char *buf, int len) {
    (void)fd;
    (void)buf;
    (void)len;
    errno = ENOSYS;
    return -1;
}

MP_WEAK int _read(int fd, char *buf, int len) {
    (void)fd;
    (void)buf;
    (void)len;
    errno = ENOSYS;
    return -1;
}

MP_WEAK int _close(int fd) {
    (void)fd;
    errno = ENOSYS;
    return -1;
}

MP_WEAK int _lseek(int fd, int ptr, int dir) {
    (void)fd;
    (void)ptr;
    (void)dir;
    errno = ENOSYS;
    return -1;
}

MP_WEAK int _fstat(int fd, struct stat *st) {
    (void)fd;
    (void)st;
    errno = ENOSYS;
    return -1;
}

MP_WEAK int _isatty(int fd) {
    (void)fd;
    errno = ENOSYS;
    return 0;
}

MP_WEAK int _getpid(void) {
    return 1;
}

MP_WEAK int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    errno = ENOSYS;
    return -1;
}

// A port without a real time-of-day back-end (e.g. alif) otherwise links
// newlib's stub, which warns and fails; morselib only needs a monotonic value
// here, so synthesise one from the millisecond tick.
MP_WEAK int _gettimeofday(struct timeval *tv, void *tz) {
    (void)tz;
    if (tv != NULL) {
        mp_uint_t ms = mp_hal_ticks_ms();
        tv->tv_sec = ms / 1000;
        tv->tv_usec = (ms % 1000) * 1000;
    }
    return 0;
}

MP_REGISTER_ROOT_POINTER(struct _machine_spi_obj_t *mm_halow_spi);
MP_REGISTER_ROOT_POINTER(void *mm_halow_heap);

#endif // MICROPY_PY_NETWORK_HALOW_MORSE_MICRO
