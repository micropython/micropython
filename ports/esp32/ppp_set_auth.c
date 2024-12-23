/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The pppapi_set_auth function was made static in the ESP-IDF, so it's re-added here.
// See ESP-IDF commit c67f4c2b4c2bb4b7740f988fc0f8a3e911e56afe

#include "ppp_set_auth.h"

#if defined(CONFIG_ESP_NETIF_TCPIP_LWIP) && defined(CONFIG_LWIP_PPP_SUPPORT)

#include "netif/ppp/pppapi.h"

typedef struct {
    struct tcpip_api_call_data call;
    ppp_pcb *ppp;
    u8_t authtype;
    const char *user;
    const char *passwd;
} set_auth_msg_t;

static err_t pppapi_do_ppp_set_auth(struct tcpip_api_call_data *m) {
    set_auth_msg_t *msg = (set_auth_msg_t *)m;
    ppp_set_auth(msg->ppp, msg->authtype, msg->user, msg->passwd);
    return ERR_OK;
}

void pppapi_set_auth(ppp_pcb *pcb, u8_t authtype, const char *user, const char *passwd) {
    set_auth_msg_t msg = { .ppp = pcb, .authtype = authtype, .user = user, .passwd = passwd};
    tcpip_api_call(pppapi_do_ppp_set_auth, &msg.call);
}

#endif
