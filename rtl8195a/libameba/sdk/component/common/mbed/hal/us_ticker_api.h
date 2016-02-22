/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_US_TICKER_API_H
#define MBED_US_TICKER_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t timestamp_t;

uint32_t us_ticker_read(void);

typedef void (*ticker_event_handler)(uint32_t id);
void us_ticker_set_handler(ticker_event_handler handler);

typedef struct ticker_event_s {
    timestamp_t            timestamp;
    uint32_t               id;
    struct ticker_event_s *next;
} ticker_event_t;

void us_ticker_init(void);
void us_ticker_set_interrupt(timestamp_t timestamp);
void us_ticker_disable_interrupt(void);
void us_ticker_clear_interrupt(void);
void us_ticker_irq_handler(void);

void us_ticker_insert_event(ticker_event_t *obj, timestamp_t timestamp, uint32_t id);
void us_ticker_remove_event(ticker_event_t *obj);

#ifdef __cplusplus
}
#endif

#endif
