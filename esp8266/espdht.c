/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Seong-Woo Kim
 * Copyright (c) 2015-2016 Damien P. George
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

#include <stdint.h>

#include "etshal.h"
#include "user_interface.h"
#include "modpyb.h"
#include "gpio.h"
#include "espdht.h"

#define LOW  0
#define HIGH 1
#define TIMEOUT 200
#define DHT_PULSES 40

static double dht_humidity;
static double dht_temperature;
static uint8_t dht_bytes[5];

static uint32_t disable_irq(void) {
    ets_intr_lock();
    return 0;
}

static void enable_irq(uint32_t i) {
    ets_intr_unlock();
}

static void mp_hal_delay_us_no_irq(uint32_t us) {
    uint32_t start = system_get_time();
    while (system_get_time() - start < us) {
    }
}

#define DELAY_US mp_hal_delay_us_no_irq

int esp_dht_detect_edge(uint pin, uint value, int timeout) {
    int counter = 0;
    while (pin_get(pin) == value && counter < timeout) {
        DELAY_US(1);
        ++counter;
    }
    if (counter > timeout) {
        return -1;
    }
    return counter;
}

int esp_dht_readSensor(uint pin) {
    uint8_t n = 0;

    // Pin LOW
    pin_set(pin, 0);
    DELAY_US(20000);
    uint32_t i = disable_irq();
    pin_set(pin, 1);
    DELAY_US(40);
    // Set Pin INPUT
    gpio_output_set(0, 0, 0, 1 << pin);

    // GET Acknowledge or Timeout
    if (esp_dht_detect_edge(pin, LOW, TIMEOUT) == -1)
        return DHT_ERROR_TIMEOUT;

    // GET HIGH
    if (esp_dht_detect_edge(pin, HIGH, TIMEOUT) == -1)
        return DHT_ERROR_TIMEOUT;

    // Read output - 40 bits : 5 bytes
    for (n = 0; n < DHT_PULSES; n++) {
        if (esp_dht_detect_edge(pin, LOW, TIMEOUT) == -1)
            return DHT_ERROR_TIMEOUT;
        uint32_t start = system_get_time();
        if (esp_dht_detect_edge(pin, HIGH, TIMEOUT) == -1)
            return DHT_ERROR_TIMEOUT;
        dht_bytes[n/8] <<= 1;
        if ((system_get_time() - start) > 40) {
            dht_bytes[n/8] |= 1;
        }
    }
    enable_irq(i);
    pin_set(pin, 1);

    return DHT_OK;
}

int esp_dht_read(uint pin) {
    int value = esp_dht_readSensor(pin);
    if (value != DHT_OK) {
        dht_humidity = DHT_INVALID_VALUE;
        dht_temperature = DHT_INVALID_VALUE;
        return value;
    }
    // DHT11
    if (dht_bytes[1] == 0 && dht_bytes[3] == 0) {
        dht_humidity = (float)dht_bytes[0];
        dht_temperature = (float)dht_bytes[2];
    } else {
        dht_humidity = (float)((dht_bytes[0] << 8 | dht_bytes[1])*0.1);
        dht_temperature = (float)((dht_bytes[2] << 8 | dht_bytes[3])*0.1);
    }

    uint8_t sum = dht_bytes[0] + dht_bytes[1] + dht_bytes[2] + dht_bytes[3];
    if (dht_bytes[4] != sum)
    {
        return DHT_ERROR_CHECKSUM;
    }
    return DHT_OK;
}

float esp_dht_readhumi(void) {
    return dht_humidity;
}

float esp_dht_readtemp(void) {
    return dht_temperature;
}

