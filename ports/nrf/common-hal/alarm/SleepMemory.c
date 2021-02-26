/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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
#include "py/runtime.h"
#include "common-hal/alarm/SleepMemory.h"
#include "nrf_power.h"

#ifdef NRF_DEBUG_PRINT
extern void dbg_dump_RAMreg(void);
#endif

#define RTC_DATA_ATTR __attribute__((section(".uninitialized")))
static RTC_DATA_ATTR uint8_t _sleep_mem[SLEEP_MEMORY_LENGTH];
static RTC_DATA_ATTR uint32_t _sleep_mem_magicnum;
#define SLEEP_MEMORY_DATA_GUARD 0xad0000af
#define SLEEP_MEMORY_DATA_GUARD_MASK 0xff0000ff

static int is_sleep_memory_valid(void) {
    if ((_sleep_mem_magicnum & SLEEP_MEMORY_DATA_GUARD_MASK)
         == SLEEP_MEMORY_DATA_GUARD) {
      return 1;
    }
    return 0;
}

void set_memory_retention(void) {
    // set RAM[n].POWER register for RAM retention
    // nRF52840 has RAM[0..7].Section[0..1] and RAM[8].Section[0..5]
    // nRF52833 has RAM[0..7].Section[0..1] and RAM[8].Section[0,1]
    for(int block = 0; block <= 7; ++block) {
        nrf_power_rampower_mask_on(NRF_POWER, block,
				   NRF_POWER_RAMPOWER_S0RETENTION_MASK |
				   NRF_POWER_RAMPOWER_S1RETENTION_MASK);
    };
#ifdef NRF52840
    nrf_power_rampower_mask_on(NRF_POWER, 8,
			       NRF_POWER_RAMPOWER_S0RETENTION_MASK |
			       NRF_POWER_RAMPOWER_S1RETENTION_MASK |
			       NRF_POWER_RAMPOWER_S2RETENTION_MASK |
			       NRF_POWER_RAMPOWER_S3RETENTION_MASK |
			       NRF_POWER_RAMPOWER_S4RETENTION_MASK |
			       NRF_POWER_RAMPOWER_S5RETENTION_MASK);
#endif
#ifdef NRF52833
    nrf_power_rampower_mask_on(NRF_POWER, 8,
			       NRF_POWER_RAMPOWER_S0RETENTION_MASK |
			       NRF_POWER_RAMPOWER_S1RETENTION_MASK);
#endif
}

static void initialize_sleep_memory(void) {
    memset((uint8_t *)_sleep_mem, 0, SLEEP_MEMORY_LENGTH);

    set_memory_retention();
#ifdef NRF_DEBUG_PRINT
    dbg_dump_RAMreg();
#endif

    _sleep_mem_magicnum = SLEEP_MEMORY_DATA_GUARD;
}

void alarm_sleep_memory_reset(void) {
    if (!is_sleep_memory_valid()) {
        initialize_sleep_memory();
    }
}

uint32_t common_hal_alarm_sleep_memory_get_length(alarm_sleep_memory_obj_t *self) {
    return sizeof(_sleep_mem);
}

bool common_hal_alarm_sleep_memory_set_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, const uint8_t* values, uint32_t len) {
    if (start_index + len > sizeof(_sleep_mem)) {
        return false;
    }

    memcpy((uint8_t *) (_sleep_mem + start_index), values, len);
    return true;
}

void common_hal_alarm_sleep_memory_get_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, uint8_t* values, uint32_t len) {
    if (start_index + len > sizeof(_sleep_mem)) {
        return;
    }
    memcpy(values, (uint8_t *) (_sleep_mem + start_index), len);
}
