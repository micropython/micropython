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

/**
 *  @brief   Turn off the JTAG function
 *
 *  @return  None
 *         
 */
void sys_jtag_off(void);
void sys_clear_ota_signature(void);
void sys_recover_ota_signature(void);
void sys_log_uart_on(void);
void sys_log_uart_off(void);
void sys_adc_calibration(u8 write, u16 *offset, u16 *gain);

/**
 *  @brief   system software reset
 *
 *  @return  None
 *
 */
void sys_reset(void);

#ifdef __cplusplus
}
#endif

#endif
