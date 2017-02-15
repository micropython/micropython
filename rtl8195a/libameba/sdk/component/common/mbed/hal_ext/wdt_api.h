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
#ifndef MBED_WATCHDOG_API_H
#define MBED_WATCHDOG_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*wdt_irq_handler)(uint32_t id);

/** Initial the watch dog time setting
 *
 * This function will initial and enable the watchdog timer with a given timeout value.
 * When the watchdog timer timeout event is triggered, the system will be reset. User also can
 * register a callback function to handle the watchdog timer timeout event.
 */
void watchdog_init(uint32_t timeout_ms);

/** Start the watchdog counting
 *
 * This function will active the watchdog timer down counting. When the watchdog timer count down
 * to 0, a callback function will be called or the system will be reset.
 */
void watchdog_start(void);

/** Stop the watchdog counting
 *
 * This function will stop the watchdog timer down counting. If a user application aware a
 * procedure may takes too long and cause the watchdog timer timeout, the application use this
 * function to stop the watchdog timer to prevent the watchdog timer timeout.
 */
void watchdog_stop(void);

/** Refresh the watchdog counting
 *
 * This function will reload the watchdog timer counting value. Usually a application do the watchdog
 * timer reflash in the main loop to prevent the watchdog timer timeout.
 */
void watchdog_refresh(void);

#ifdef __cplusplus
}
#endif

#endif
