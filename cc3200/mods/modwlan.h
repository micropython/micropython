/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_CC3200_MODS_MODWLAN_H
#define MICROPY_INCLUDED_CC3200_MODS_MODWLAN_H

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define SIMPLELINK_SPAWN_TASK_PRIORITY              3
#define SIMPLELINK_TASK_STACK_SIZE                  2048
#define SL_STOP_TIMEOUT                             35
#define SL_STOP_TIMEOUT_LONG                        575

#define MODWLAN_WIFI_EVENT_ANY                      0x01

#define MODWLAN_SSID_LEN_MAX                        32

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef enum {
    MODWLAN_OK = 0,
    MODWLAN_ERROR_INVALID_PARAMS = -1,
    MODWLAN_ERROR_TIMEOUT = -2,
    MODWLAN_ERROR_UNKNOWN = -3,
} modwlan_Status_t;

typedef struct _wlan_obj_t {
    mp_obj_base_t       base;
    mp_obj_t            irq_obj;
    uint32_t            status;

    uint32_t            ip;

    int8_t              mode;
    uint8_t             auth;
    uint8_t             channel;
    uint8_t             antenna;

    // my own ssid, key and mac
    uint8_t             ssid[(MODWLAN_SSID_LEN_MAX + 1)];
    uint8_t             key[65];
    uint8_t             mac[SL_MAC_ADDR_LEN];

    // the sssid (or name) and mac of the other device
    uint8_t             ssid_o[33];
    uint8_t             bssid[6];
    uint8_t             irq_flags;
    bool                irq_enabled;

#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    bool                servers_enabled;
#endif
} wlan_obj_t;

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
extern _SlLockObj_t wlan_LockObj;

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/
extern void wlan_pre_init (void);
extern void wlan_sl_init (int8_t mode, const char *ssid, uint8_t ssid_len, uint8_t auth, const char *key, uint8_t key_len,
                          uint8_t channel, uint8_t antenna, bool add_mac);
extern void wlan_first_start (void);
extern void wlan_update(void);
extern void wlan_stop (uint32_t timeout);
extern void wlan_get_mac (uint8_t *macAddress);
extern void wlan_get_ip (uint32_t *ip);
extern bool wlan_is_connected (void);
extern void wlan_set_current_time (uint32_t seconds_since_2000);
extern void wlan_off_on (void);

#endif // MICROPY_INCLUDED_CC3200_MODS_MODWLAN_H
