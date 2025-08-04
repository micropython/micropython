/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_PSOC6_MODMACHINE_H
#define MICROPY_INCLUDED_PSOC6_MODMACHINE_H


// micropython includes
#include "py/obj.h"


extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_type_t machine_i2c_slave_type;
extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_rtc_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_spi_slave_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_wdt_type;
extern const mp_obj_type_t machine_adcblock_type;
extern const mp_obj_type_t machine_pdm_pcm_type;
extern const mp_obj_type_t machine_i2s_type;
// extern const mp_obj_type_t machine_uart_type;
#ifdef MICROPY_PY_SD_CARD
extern const mp_obj_type_t machine_sdcard_type;
#endif

// Add all mod machine deinits
void machine_pin_deinit_all(void);
void machine_adcblock_deinit_all(void);
void machine_i2c_deinit_all(void);
void machine_pwm_deinit_all(void);
void machine_rtc_deinit_all(void);
void machine_spi_deinit_all(void);
void machine_timer_deinit_all(void);
void machine_wdt_deinit(void);
void machine_uart_deinit_all(void);
void machine_sdcard_deinit_all(void);
/* Note: the static functions' prototypes in the .c file cannot be declared here
since they are static. The static type in those functions come from MPY hence
should stay that way.  */


// functions to be called from other .c files. Hence they are not static.
void machine_init(void);
void machine_deinit(void);
mp_obj_t system_reset_cause(void);

extern void audio_i2s_set_frequency(uint32_t audio_clock_freq_hz);
extern cyhal_clock_t audio_clock;
extern cyhal_clock_t pdm_pcm_audio_clock;
extern bool clock_set_i2s;
extern bool clock_set_pdm;

// enums to hold the frequency constants
enum clock_freq_type {
    AUDIO_I2S_98_MHZ =  98000000,
    AUDIO_I2S_90_MHZ = 90000000,
    AUDIO_PDM_24_576_000_HZ = 24576000,
    AUDIO_PDM_22_579_000_HZ = 22579000,
    CM4,
    CM4_FLL
};

extern enum clock_freq_type PLL0_freq;

#endif // MICROPY_INCLUDED_PSOC6_MODMACHINE_H
