/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The pppapi_set_auth function was made static in the ESP-IDF, so it's re-added here.
// See ESP-IDF commit c67f4c2b4c2bb4b7740f988fc0f8a3e911e56afe

#pragma once

#include "esp_netif.h"

#ifdef CONFIG_ESP_NETIF_TCPIP_LWIP

#include "lwip/netif.h"

typedef struct ppp_pcb_s ppp_pcb;

void pppapi_set_auth(ppp_pcb *pcb, u8_t authtype, const char *user, const char *passwd);

#endif
