/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#ifndef MODWLAN_H_
#define MODWLAN_H_

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define SIMPLELINK_SPAWN_TASK_PRIORITY              3
#define SIMPLELINK_TASK_STACK_SIZE                  2048

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef enum {
    MODWLAN_OK = 0,
    MODWLAN_ERROR_INVALID_PARAMS = -1,
    MODWLAN_ERROR_TIMEOUT = -2,
    MODWLAN_ERROR_UNKNOWN = -3

}modwlan_Status_t;

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
extern _SlLockObj_t wlan_LockObj;

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/
extern void wlan_init0 (void);
extern modwlan_Status_t wlan_sl_enable (SlWlanMode_t mode, const char *ssid, uint8_t ssid_len, uint8_t sec,
                                        const char *key, uint8_t key_len, uint8_t channel);
extern void wlan_stop (void);
extern void wlan_start (void);
extern SlWlanMode_t wlan_get_mode (void);
extern void wlan_get_mac (uint8_t *macAddress);
extern void wlan_get_ip (uint32_t *ip);
extern void wlan_set_pm_policy (uint8_t policy);
extern void wlan_stop_servers (void);

#endif /* MODWLAN_H_ */
