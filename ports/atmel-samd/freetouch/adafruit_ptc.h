/*
 * FreeTouch, a QTouch-compatible library - tested on ATSAMD21 only!
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Limor 'ladyada' Fried for Adafruit Industries
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

// This is similar to the drivers found in sam0/drivers but for the PTC.
#ifndef ADAFRUIT_FREETOUCH_ADAFRUIT_PTC_H
#define ADAFRUIT_FREETOUCH_ADAFRUIT_PTC_H

#include <stdint.h>

#include "samd21_ptc_component.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Touch library oversampling (filter) setting */
typedef enum tag_oversample_level_t {
	OVERSAMPLE_1,
	OVERSAMPLE_2,
	OVERSAMPLE_4,
	OVERSAMPLE_8,
	OVERSAMPLE_16,
	OVERSAMPLE_32,
	OVERSAMPLE_64
}
oversample_t;

/* Touch library series resistor setting */
typedef enum tag_series_resistor_t {
	RESISTOR_0,
	RESISTOR_20K,
	RESISTOR_50K,
	RESISTOR_100K,
}
series_resistor_t;

typedef enum tag_freq_mode_t {
	FREQ_MODE_NONE,
	FREQ_MODE_HOP,
	FREQ_MODE_SPREAD,
	FREQ_MODE_SPREAD_MEDIAN
}
freq_mode_t;

typedef enum tag_freq_hop_t {
	FREQ_HOP_1,
	FREQ_HOP_2,
	FREQ_HOP_3,
	FREQ_HOP_4,
	FREQ_HOP_5,
	FREQ_HOP_6,
	FREQ_HOP_7,
	FREQ_HOP_8,
	FREQ_HOP_9,
	FREQ_HOP_10,
	FREQ_HOP_11,
	FREQ_HOP_12,
	FREQ_HOP_13,
	FREQ_HOP_14,
	FREQ_HOP_15,
	FREQ_HOP_16
}
freq_hop_t;

struct adafruit_ptc_config {
    uint8_t pin;           // ASF pin #
    int8_t yline;      // the Y select line (see datasheet)
    oversample_t oversample;
    series_resistor_t seriesres;
    freq_mode_t freqhop;
    freq_hop_t hops;
    uint16_t compcap;
    uint8_t  intcap;
};

void adafruit_ptc_get_config_default(struct adafruit_ptc_config *config);
void adafruit_ptc_init(Ptc* module_inst, struct adafruit_ptc_config const* config);
void adafruit_ptc_start_conversion(Ptc* module_inst, struct adafruit_ptc_config const* config);

bool adafruit_ptc_is_conversion_finished(Ptc* module_inst);
uint16_t adafruit_ptc_get_conversion_result(Ptc* module_inst);

#ifdef __cplusplus
}
#endif

#endif  // ADAFRUIT_FREETOUCH_ADAFRUIT_PTC_H
