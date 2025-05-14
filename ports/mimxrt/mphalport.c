/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2020 Jim Mussared
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

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "shared/timeutils/timeutils.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/tinyusb/mp_usbd_cdc.h"
#include "extmod/misc.h"
#include "ticks.h"
#include "tusb.h"
#include "fsl_snvs_lp.h"

#ifndef MICROPY_HW_STDIN_BUFFER_LEN
#define MICROPY_HW_STDIN_BUFFER_LEN 512
#endif

#include CPU_HEADER_H

static uint8_t stdin_ringbuf_array[MICROPY_HW_STDIN_BUFFER_LEN];
ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0};

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    ret |= mp_usbd_cdc_poll_interfaces(poll_flags);
    #if MICROPY_PY_OS_DUPTERM
    ret |= mp_os_dupterm_poll(poll_flags);
    #endif
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        mp_usbd_cdc_poll_interfaces(0);
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        #if MICROPY_PY_OS_DUPTERM
        int dupterm_c = mp_os_dupterm_rx_chr();
        if (dupterm_c >= 0) {
            return dupterm_c;
        }
        #endif
        MICROPY_EVENT_POLL_HOOK
    }
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t ret = len;
    bool did_write = false;
    mp_uint_t cdc_res = mp_usbd_cdc_tx_strn(str, len);
    if (cdc_res > 0) {
        did_write = true;
        ret = MIN(cdc_res, ret);
    }
    #if MICROPY_PY_OS_DUPTERM
    int dupterm_res = mp_os_dupterm_tx_strn(str, len);
    if (dupterm_res >= 0) {
        did_write = true;
        ret = MIN((mp_uint_t)dupterm_res, ret);
    }
    #endif
    return did_write ? ret : 0;
}

uint64_t mp_hal_time_ns(void) {
    snvs_lp_srtc_datetime_t t;
    SNVS_LP_SRTC_GetDatetime(SNVS, &t);
    uint64_t s = timeutils_seconds_since_epoch(t.year, t.month, t.day, t.hour, t.minute, t.second);
    return s * 1000000000ULL;
}

/*******************************************************************************/
// MAC address

void mp_hal_get_unique_id(uint8_t id[]) {
    #if defined CPU_MIMXRT1176_cm7
    *(uint32_t *)id = OCOTP->FUSEN[0x10].FUSE;
    *(uint32_t *)(id + 4) = OCOTP->FUSEN[0x11].FUSE;
    #else
    *(uint32_t *)id = OCOTP->CFG0;
    *(uint32_t *)(id + 4) = OCOTP->CFG1;
    #endif
}

// Generate a random locally administered MAC address (LAA)
void mp_hal_generate_laa_mac(int idx, uint8_t buf[6]) {
    // Take the MAC addr from the OTP's Configuration and Manufacturing Info
    unsigned char id[8];
    mp_hal_get_unique_id(id);

    uint32_t *pt1 = (uint32_t *)id;
    uint32_t *pt2 = (uint32_t *)(id + 4);

    buf[0] = 0x02; // Locally Administered MAC
    *(uint32_t *)&buf[1] = *pt1 ^ (*pt1 >> 8);
    *(uint16_t *)&buf[4] = (uint16_t)(*pt2 ^ *pt2 >> 16);
    buf[5] ^= (uint8_t)idx;
}

// A board can override this if needed
MP_WEAK void mp_hal_get_mac(int idx, uint8_t buf[6]) {
    mp_hal_generate_laa_mac(idx, buf);
}

void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest) {
    static const char hexchr[16] = "0123456789ABCDEF";
    uint8_t mac[6];
    mp_hal_get_mac(idx, mac);
    for (; chr_len; ++chr_off, --chr_len) {
        *dest++ = hexchr[mac[chr_off >> 1] >> (4 * (1 - (chr_off & 1))) & 0xf];
    }
}
