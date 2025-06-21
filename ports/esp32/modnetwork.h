/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
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
#ifndef MICROPY_INCLUDED_ESP32_MODNETWORK_H
#define MICROPY_INCLUDED_ESP32_MODNETWORK_H

#include "esp_netif.h"

// lan867x component requires newer IDF version
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0) && CONFIG_IDF_TARGET_ESP32
#define PHY_LAN867X_ENABLED (1)
#else
#define PHY_LAN867X_ENABLED (0)
#endif

// PHY_GENERIC support requires newer IDF version
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0) && CONFIG_IDF_TARGET_ESP32
#define PHY_GENERIC_ENABLED (1)
#else
#define PHY_GENERIC_ENABLED (0)
#endif

enum {
    // PHYs supported by the internal Ethernet MAC:
    PHY_LAN8710, PHY_LAN8720, PHY_IP101, PHY_RTL8201, PHY_DP83848, PHY_KSZ8041, PHY_KSZ8081,
    #if PHY_LAN867X_ENABLED
    PHY_LAN8670,
    #endif
    #if PHY_GENERIC_ENABLED
    PHY_GENERIC,
    #endif
    // PHYs which are actually SPI Ethernet MAC+PHY chips:
    PHY_KSZ8851SNL = 100, PHY_DM9051, PHY_W5500
};
#define IS_SPI_PHY(NUM) (NUM >= 100)
enum { ETH_INITIALIZED, ETH_STARTED, ETH_STOPPED, ETH_CONNECTED, ETH_DISCONNECTED, ETH_GOT_IP };

// Cases similar to ESP8266 user_interface.h
// Error cases are referenced from wifi_err_reason_t in ESP-IDF
enum {
    STAT_IDLE       = 1000,
    STAT_CONNECTING = 1001,
    STAT_GOT_IP     = 1010,
};

typedef struct _base_if_obj_t {
    mp_obj_base_t base;
    esp_interface_t if_id;
    esp_netif_t *netif;
    volatile bool active;
} base_if_obj_t;

extern const mp_obj_type_t esp_network_wlan_type;

MP_DECLARE_CONST_FUN_OBJ_0(esp_network_initialize_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(esp_network_get_wlan_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(esp_network_get_lan_obj);
extern const struct _mp_obj_type_t esp_network_ppp_lwip_type;
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(esp_network_ifconfig_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(esp_network_ipconfig_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(esp_nic_ipconfig_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(esp_network_config_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(esp_network_phy_mode_obj);

mp_obj_t esp_ifname(esp_netif_t *netif);

MP_NORETURN void esp_exceptions_helper(esp_err_t e);

static inline void esp_exceptions(esp_err_t e) {
    if (e != ESP_OK) {
        esp_exceptions_helper(e);
    }
}

void socket_events_deinit(void);
void esp_initialise_wifi(void);

#endif
