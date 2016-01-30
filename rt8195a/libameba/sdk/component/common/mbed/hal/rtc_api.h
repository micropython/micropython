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
#ifndef MBED_RTC_API_H
#define MBED_RTC_API_H

#include "device.h"

#if DEVICE_RTC

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

void rtc_init(void);
void rtc_free(void);
int rtc_isenabled(void);

time_t rtc_read(void);
void rtc_write(time_t t);

#ifdef __cplusplus
}
#endif

#endif

#endif
