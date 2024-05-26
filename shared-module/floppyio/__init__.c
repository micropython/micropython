// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "py/mphal.h"

#include "shared-bindings/time/__init__.h"
#include "shared-bindings/floppyio/__init__.h"
#if CIRCUITPY_DIGITALIO
#include "common-hal/floppyio/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#endif

#include "lib/adafruit_floppy/src/mfm_impl.h"

#if CIRCUITPY_DIGITALIO
MP_WEAK
__attribute__((optimize("O3")))
int common_hal_floppyio_flux_readinto(void *buf, size_t len, digitalio_digitalinout_obj_t *data, digitalio_digitalinout_obj_t *index, mp_int_t index_wait_ms) {
    mp_printf(&mp_plat_print, "common_hal_floppyio_flux_readinto in %s\n", __FILE__);
    uint32_t index_mask;
    volatile uint32_t *index_port = common_hal_digitalio_digitalinout_get_reg(index, DIGITALINOUT_REG_READ, &index_mask);

    uint32_t data_mask;
    volatile uint32_t *data_port = common_hal_digitalio_digitalinout_get_reg(data, DIGITALINOUT_REG_READ, &data_mask);

    uint32_t index_deadline_ms = supervisor_ticks_ms32() + index_wait_ms;
#undef READ_INDEX
#undef READ_DATA
#define READ_INDEX() (!!(*index_port & index_mask))
#define READ_DATA() (!!(*data_port & data_mask))

    memset(buf, 0, len);

    uint8_t *pulses = buf, *pulses_ptr = pulses, *pulses_end = pulses + len;

    common_hal_mcu_disable_interrupts();

    // wait for index pulse low
    while (READ_INDEX()) { /* NOTHING */
        if (supervisor_ticks_ms32() > index_deadline_ms) {
            common_hal_mcu_enable_interrupts();
            mp_raise_RuntimeError(MP_ERROR_TEXT("timeout waiting for index pulse"));
            return 0;
        }
    }


    // if data line is low, wait till it rises
    while (!READ_DATA()) { /* NOTHING */
    }

    // and then till it drops down
    while (READ_DATA()) { /* NOTHING */
    }

    uint32_t index_state = 0;
    while (pulses_ptr < pulses_end) {
        index_state = (index_state << 1) | READ_INDEX();
        if ((index_state & 3) == 2) { // a zero-to-one transition
            break;
        }

        unsigned pulse_count = 3;
        while (!READ_DATA()) {
            pulse_count++;
        }

        while (READ_DATA()) {
            pulse_count++;
        }

        *pulses_ptr++ = MIN(255, pulse_count);
    }
    common_hal_mcu_enable_interrupts();

    return pulses_ptr - pulses;
}
#endif

int common_hal_floppyio_mfm_readinto(const mp_buffer_info_t *buf, const mp_buffer_info_t *flux_buf, uint8_t *validity, size_t t2_max, size_t t3_max) {
    mfm_io_t io = {
        .T2_max = t2_max,
        .T3_max = t3_max,
        .pulses = flux_buf->buf,
        .n_pulses = flux_buf->len,
        .sectors = buf->buf,
        .sector_validity = validity,
        .n_sectors = buf->len / 512,
    };
    int result = decode_track_mfm(&io);

    return result;
}
