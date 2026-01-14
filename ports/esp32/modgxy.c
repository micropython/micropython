/* mod _gxy 15/10/2025 creation*/
#include "py/obj.h"
#include "py/runtime.h"

#include <lwip/netif.h>
#include <lwip/dhcp.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_eap_client.h>

static struct dhcp *_dhcp_get(uint32_t *ip_srv){
    struct netif *nif = netif_list;
    for (int i = 0; i < 5 && nif; i++) {
        if (nif->name[0] == 's' && nif->name[1] == 't') { // station
            struct dhcp *d = netif_dhcp_data(nif);
            if (d) {
                uint32_t srv = ip_addr_get_ip4_u32(&d->server_ip_addr);
                if (srv != 0) {
                    if (ip_srv)
                        *ip_srv = srv;
                    return d;
                }
            }
        }
        nif = nif->next;
    }
    return 0;
}
/*
bool dhcp_get_info(dhcp_info_t *nfo)
{
    struct dhcp *d = _dhcp_get(&nfo->srv);
    if (d)
    {
        nfo->ip = d->offered_ip_addr.addr;
        nfo->nm = d->offered_sn_mask.addr;
        nfo->gw = d->offered_gw_addr.addr;
        const ip_addr_t *dns1 = dns_getserver(0);
        nfo->dns1 = dns1 ? *(int *)dns1 : 0;
        const ip_addr_t *dns2 = dns_getserver(1);
        nfo->dns2 = dns2 ? *(int *)dns2 : 0;

        // t0_timeout and lease_used : u16 minutes (ESP32 u32_t in seconds)
        int rem = d->t0_timeout - d->lease_used;
        nfo->remain = rem * DHCP_COARSE_TIMER_SECS / 60; // in minutes
        trace_n("dhcp rem:%d/%d", rem, d->t0_timeout);
        return true;
    }
    return false;
} */
static bool dhcp_get_rem(int *minutes) {
    struct dhcp *d = _dhcp_get(0);
    if (d) {
        int rem = d->t0_timeout - d->lease_used;
        if (minutes)
            *minutes = rem * DHCP_COARSE_TIMER_SECS / 60;
        return true;
    }
    return false;
}
static mp_obj_t gxy_dhcp_rem(void) {
    int minutes;
    return mp_obj_new_int( dhcp_get_rem(&minutes) ? minutes : -1);
}
static MP_DEFINE_CONST_FUN_OBJ_0(gxy_dhcp_rem_obj, gxy_dhcp_rem);

static mp_obj_t gxy_wifi_set_storage(const mp_obj_t storage) {
    int e = (int) esp_wifi_set_storage(mp_obj_get_int(storage));
    return mp_obj_new_int(e);
}
static MP_DEFINE_CONST_FUN_OBJ_1(gxy_wifi_set_storage_obj, gxy_wifi_set_storage);

static mp_obj_t gxy_wifi_eap_connect(const mp_obj_t ssid, const mp_obj_t user, const mp_obj_t pass) {
    wifi_config_t wi_conf;
    memset(&wi_conf, 0, sizeof(wifi_config_t));
    const char * tag = "_gxy";
    esp_log_level_set(tag, ESP_LOG_INFO);
    ESP_LOGI(tag, "EAP connecting ...");
    
    size_t len;
    const char *p = mp_obj_str_get_data(ssid, &len);
    ESP_LOGI(tag, "ssid:%s len:%d", p, len);
    if (len > sizeof(wi_conf.sta.ssid))
        len = sizeof(wi_conf.sta.ssid);
    memcpy((char *)wi_conf.sta.ssid, p, len);

    p = mp_obj_str_get_data(user, &len);
    ESP_LOGI(tag, "user:%s len:%d", p, len);
    ESP_ERROR_CHECK(esp_eap_client_set_username((uint8_t*) p, len));
    ESP_ERROR_CHECK(esp_eap_client_set_identity((uint8_t*) p, len));
    p = mp_obj_str_get_data(pass, &len);
    ESP_LOGI(tag, "pass:%s len:%d", p, len);
    ESP_ERROR_CHECK(esp_eap_client_set_password((uint8_t*) p, len));

    ESP_ERROR_CHECK(esp_wifi_sta_enterprise_enable());

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wi_conf));
    ESP_ERROR_CHECK(esp_wifi_start()); 
    ESP_ERROR_CHECK(esp_wifi_connect());
    return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_3(gxy_wifi_eap_connect_obj, gxy_wifi_eap_connect);

static const mp_rom_map_elem_t gxy_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__gxy) },

    { MP_ROM_QSTR(MP_QSTR_VERSION), MP_ROM_INT(3) }, // 3 pour espnow  V2
    { MP_ROM_QSTR(MP_QSTR_dhcp_rem), MP_ROM_PTR(&gxy_dhcp_rem_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_set_storage), MP_ROM_PTR(&gxy_wifi_set_storage_obj) },
    { MP_ROM_QSTR(MP_QSTR_wifi_eap_connect), MP_ROM_PTR(&gxy_wifi_eap_connect_obj) },
};

static MP_DEFINE_CONST_DICT(gxy_module_globals, gxy_module_globals_table);
const mp_obj_module_t gxy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&gxy_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__gxy, gxy_module);
