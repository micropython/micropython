#include "modnetwork2.h"

#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wps.h"
#include "esp_log.h"
#include "py/gc.h"
#include "py/objarray.h"
#include "shared/netutils/netutils.h"

#include <string.h>

#if ESP_IDF_VERSION_MINOR >= 3

#define SNIFFER_PAYLOAD_FCS_LEN (4)

static char wifi_mode_names[][5] = {"NULL", "STA", "AP", "APSTA"};
static wifi_network_if_obj_t *wifi_if = NULL;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
    int32_t event_id, void *event_data) {
    wifi_network_if_obj_t *self = (wifi_network_if_obj_t *)arg;

    if (event_base == WIFI_EVENT) {
        self->last_wifi_event = event_id;

        switch (event_id) {
            case WIFI_EVENT_STA_START:
                self->num_connect_retry = 0;
                break;
            case WIFI_EVENT_STA_CONNECTED:
                self->disconnect_reason = 0;
                self->connected = true;
                self->num_connect_retry = 0;
                break;
            case WIFI_EVENT_STA_DISCONNECTED: {
                system_event_sta_disconnected_t *event = (system_event_sta_disconnected_t *)event_data;
                self->disconnect_reason = event->reason;
                self->connected = false;
                if (self->if_mode == WIFI_MODE_STA) {
                    if ((self->try_reconnect) && (
                        (self->num_connect_retry < self->max_connect_retry || (self->num_connect_retry == -1)))) {
                        self->num_connect_retry += 1;
                        esp_wifi_connect();
                    }
                }
            }
            break;
            case WIFI_EVENT_SCAN_DONE: {
                self->scan_done = true;
            }
            break;
            case WIFI_EVENT_AP_STACONNECTED:
                self->num_sta += 1;
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                self->num_sta -= 1;
                break;
            case WIFI_EVENT_STA_WPS_ER_SUCCESS: {
                esp_wifi_wps_disable();
                self->wps_probing = false;
                self->num_connect_retry = 0;
                esp_wifi_connect();
            }
            break;
            case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
            case WIFI_EVENT_STA_WPS_ER_FAILED: {
                esp_wifi_wps_disable();
                self->wps_probing = false;
            }
            break;
            default:
                break;
        }
    }

    if (event_base == IP_EVENT) {
        self->last_ip_event = event_id;
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP: {
                ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
                self->ip_addr = event->ip_info.ip.addr;
                self->gw_addr = event->ip_info.gw.addr;
                self->subnetmask = event->ip_info.netmask.addr;
            }
            break;
            case IP_EVENT_STA_LOST_IP: {
                self->ip_addr = 0;
                self->gw_addr = 0;
                self->subnetmask = 0;
            }
            break;
            default:
                break;
        }
    }
}


STATIC mp_obj_t wifi_network_if_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *in_args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, true);

    enum { ARG_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = WIFI_MODE_STA}},
    };

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, in_args + n_args);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, in_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (wifi_if) {
        mp_raise_ValueError(MP_ERROR_TEXT("Wifi Interface already initialized. Only one is allowed."));
    }

    wifi_network_if_obj_t *self = m_new_obj(wifi_network_if_obj_t);
    self->base.type = &wifi_network_if_type;
    self->if_mode = args[ARG_mode].u_int;
    self->active = false;
    self->sniffer_running = false;
    wifi_if = self;

    esp_exceptions(esp_netif_init());
    esp_exceptions(esp_event_loop_create_default());

    if (args[ARG_mode].u_int == WIFI_MODE_STA) {
        self->netif = esp_netif_create_default_wifi_sta();
    } else if (args[ARG_mode].u_int == WIFI_MODE_AP) {
        self->netif = esp_netif_create_default_wifi_ap();
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_exceptions(esp_wifi_init(&cfg));

    if (esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, self, NULL) != ESP_OK) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("esp_event_handler_register failed"));
    }
    if (esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, self, NULL) != ESP_OK) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("esp_event_handler_register failed"));
    }

    esp_wifi_set_mode(args[ARG_mode].u_int);

    return MP_OBJ_FROM_PTR(self);
};


STATIC void wifi_network_if_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    wifi_network_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Wifi(mode=%s)",
        wifi_mode_names[self->if_mode]);
};


STATIC mp_obj_t wifi_network_if_active(size_t n_args, const mp_obj_t *args) {
    wifi_network_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args > 1) {
        bool active = mp_obj_is_true(args[1]);
        if (active && !self->active) {
            esp_exceptions(esp_wifi_start());
            self->active = true;
        } else if (!active && self->active) {
            esp_exceptions(esp_wifi_stop());
            self->active = false;
        }
    }
    return self->active ? mp_const_true : mp_const_false;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wifi_network_if_active_obj, 1, 2, wifi_network_if_active);


STATIC mp_obj_t wifi_network_if_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password, ARG_bssid, ARG_auto_reconnect, ARG_max_reconnect, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = mp_const_none} },
        { MP_QSTR_, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_auto_reconnect, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = mp_const_true} },
        { MP_QSTR_max_reconnects, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    wifi_network_if_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    if (self->if_mode == WIFI_MODE_AP) {
        mp_raise_ValueError(MP_ERROR_TEXT("Interface must be in STA mode"));
    }

    wifi_config_t wifi_config = {0};
    size_t len;
    const char *p;

    if (args[ARG_ssid].u_obj == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("SSID cannot be None"));
    } else {
        p = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
        memcpy(wifi_config.sta.ssid, p, MIN(len, sizeof(wifi_config.sta.ssid)));
        self->ssid = args[ARG_ssid].u_obj;
    }

    if (args[ARG_password].u_obj == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("Password cannot be None"));
    } else {
        p = mp_obj_str_get_data(args[ARG_password].u_obj, &len);
        memcpy(wifi_config.sta.password, p, MIN(len, sizeof(wifi_config.sta.password)));
        self->password = args[ARG_password].u_obj;
    }

    if (args[ARG_bssid].u_obj != mp_const_none) {
        p = mp_obj_str_get_data(args[ARG_bssid].u_obj, &len);
        memcpy(wifi_config.sta.bssid, p, MIN(len, sizeof(wifi_config.sta.password)));
        wifi_config.sta.bssid_set = 1;
        self->bssid = args[ARG_bssid].u_obj;
    }

    self->auto_reconnect = args[ARG_auto_reconnect].u_bool;
    self->max_connect_retry = args[ARG_max_reconnect].u_int;
    self->num_connect_retry = 0;
    self->try_reconnect = self->auto_reconnect;

    esp_exceptions(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_wifi_connect();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_network_if_connect_obj, 3, wifi_network_if_connect);


STATIC mp_obj_t wifi_network_if_start_ap(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password, ARG_authmode, ARG_channel, ARG_max_connection, ARG_hidden };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = mp_const_none} },
        { MP_QSTR_, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = mp_const_none} },
        { MP_QSTR_authmode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = WIFI_AUTH_WPA_WPA2_PSK} },
        { MP_QSTR_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_max_connection, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5} },
        { MP_QSTR_hidden, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    wifi_network_if_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);


    if (self->if_mode != WIFI_MODE_AP) {
        mp_raise_ValueError(MP_ERROR_TEXT("Interface must be in AP mode"));
    }

    wifi_config_t wifi_config = {0};
    size_t len;
    const char *p;

    if (args[ARG_ssid].u_obj == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("SSID cannot be None"));
    } else {
        p = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
        memcpy(wifi_config.ap.ssid, p, MIN(len, sizeof(wifi_config.ap.ssid)));
        wifi_config.ap.ssid_len = sizeof(wifi_config.ap.ssid);
        self->ssid = args[ARG_ssid].u_obj;
    }

    if (args[ARG_password].u_obj == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("Password cannot be None"));
    } else {
        p = mp_obj_str_get_data(args[ARG_password].u_obj, &len);
        memcpy(wifi_config.ap.password, p, MIN(len, sizeof(wifi_config.ap.password)));
        self->password = args[ARG_password].u_obj;
    }

    wifi_config.ap.authmode = args[ARG_authmode].u_int;
    wifi_config.ap.channel = args[ARG_channel].u_int;
    wifi_config.ap.max_connection = args[ARG_max_connection].u_int;
    wifi_config.ap.ssid_hidden = args[ARG_hidden].u_bool;

    esp_exceptions(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    esp_wifi_start();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_network_if_start_ap_obj, 3, wifi_network_if_start_ap);


STATIC mp_obj_t wifi_network_if_disconnect(mp_obj_t self_in) {
    wifi_network_if_obj_t *self = self_in;

    if (self->if_mode == WIFI_MODE_AP) {
        mp_raise_ValueError(MP_ERROR_TEXT("Interface must be in STA mode"));
    }

    self->try_reconnect = false;
    esp_exceptions(esp_wifi_disconnect());

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_if_disconnect_obj, wifi_network_if_disconnect);


STATIC mp_obj_t wifi_network_if_stop_ap(mp_obj_t self_in) {
    wifi_network_if_obj_t *self = self_in;

    if (self->if_mode == WIFI_MODE_STA) {
        mp_raise_ValueError(MP_ERROR_TEXT("Interface must be in STA mode"));
    }

    self->try_reconnect = false;
    esp_exceptions(esp_wifi_disconnect());

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_if_stop_ap_obj, wifi_network_if_stop_ap);


STATIC mp_obj_t wifi_network_if_start_wps(mp_obj_t self_in) {
    wifi_network_if_obj_t *self = self_in;

    if (self->if_mode == WIFI_MODE_AP) {
        mp_raise_ValueError(MP_ERROR_TEXT("Interface must be in STA mode"));
    }

    esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);
    esp_exceptions(esp_wifi_wps_enable(&config));
    esp_exceptions(esp_wifi_wps_start(0));
    self->wps_probing = true;

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_if_start_wps_obj, wifi_network_if_start_wps);

STATIC mp_obj_t wifi_network_if_get_scan_results(mp_obj_t self_in) {
    wifi_network_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->scan_done) {
        return mp_const_none;
    }
    if (self->last_scan_results != mp_const_none) {
        return self->last_scan_results;
    }
    mp_obj_t list = mp_obj_new_list(0, NULL);
    uint16_t count = 0;
    esp_exceptions(esp_wifi_scan_get_ap_num(&count));
    wifi_ap_record_t *wifi_ap_records = calloc(count, sizeof(wifi_ap_record_t));
    esp_exceptions(esp_wifi_scan_get_ap_records(&count, wifi_ap_records));
    for (uint16_t i = 0; i < count; i++) {
        mp_obj_tuple_t *t = mp_obj_new_tuple(12, NULL);
        uint8_t *x = memchr(wifi_ap_records[i].ssid, 0, sizeof(wifi_ap_records[i].ssid));
        int ssid_len = x ? x - wifi_ap_records[i].ssid : sizeof(wifi_ap_records[i].ssid);
        t->items[0] = mp_obj_new_str((char *)wifi_ap_records[i].ssid, ssid_len);
        t->items[1] = mp_obj_new_bytes(wifi_ap_records[i].bssid, sizeof(wifi_ap_records[i].bssid));
        t->items[2] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].primary);
        t->items[3] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].rssi);
        t->items[4] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].authmode);
        t->items[5] = mp_const_false; // hidden (for compatibility)
        t->items[6] = mp_obj_new_bool(wifi_ap_records[i].phy_11b);
        t->items[7] = mp_obj_new_bool(wifi_ap_records[i].phy_11g);
        t->items[8] = mp_obj_new_bool(wifi_ap_records[i].phy_11n);
        t->items[9] = mp_obj_new_bool(wifi_ap_records[i].wps);
        t->items[10] = mp_obj_new_bool(wifi_ap_records[i].ftm_initiator);
        t->items[11] = mp_obj_new_bool(wifi_ap_records[i].ftm_responder);
        mp_obj_list_append(list, MP_OBJ_FROM_PTR(t));
    }
    free(wifi_ap_records);
    self->last_scan_results = list;
    return list;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_if_get_scan_results_obj, wifi_network_if_get_scan_results);

STATIC mp_obj_t wifi_network_if_scan(size_t n_args, const mp_obj_t *args) {
    wifi_network_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (!self->active) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Interface must be active"));
    }
    self->scan_done = false;
    self->last_scan_results = mp_const_none;
    bool block = true;
    if (n_args > 1) {
        block = mp_obj_is_true(args[1]);
    }

    wifi_scan_config_t config = { 0 };
    config.show_hidden = true;

    if (block) {
        esp_err_t status = esp_wifi_scan_start(&config, true);
        self->scan_done = true;
        if (status == 0) {
            return wifi_network_if_get_scan_results(args[0]);
        }
    } else {
        esp_exceptions(esp_wifi_scan_start(&config, false));
    }

    return mp_const_none;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wifi_network_if_scan_obj, 1, 2, wifi_network_if_scan);


static void wifi_sniffer_cb(void *recv_buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t *sniffer = (wifi_promiscuous_pkt_t *)recv_buf;
    uint length = sniffer->rx_ctrl.sig_len;

    if (type != WIFI_PKT_MISC && !sniffer->rx_ctrl.rx_state) {
        length -= SNIFFER_PAYLOAD_FCS_LEN;

        mp_obj_array_t *b = (mp_obj_array_t *)wifi_if->sniffer_arg->items[1];
        memcpy(b->items, sniffer->payload, MIN(length, 1024));
        b->len = MIN(length, 1024);

        wifi_if->sniffer_arg->items[0] = mp_obj_new_int(sniffer->rx_ctrl.timestamp);
        if (wifi_if->sniffer_handler != mp_const_none) {
            mp_sched_schedule(wifi_if->sniffer_handler, wifi_if->sniffer_arg);
        }
    }
}


STATIC mp_obj_t wifi_network_if_start_sniffer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_filter, ARG_channel, ARG_handler };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_filter, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = WIFI_PROMIS_FILTER_MASK_ALL} },
        { MP_QSTR_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 6} },
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    wifi_network_if_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    if (self->sniffer_running) {
        mp_raise_OSError(MP_ERROR_TEXT("Sniffer already running"));
    }

    if (self->if_mode != WIFI_MODE_NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("Interface must be in NULL mode"));
    }

    if (args[ARG_handler].u_obj != mp_const_none) {
        self->sniffer_handler = args[ARG_handler].u_obj;
        if (!mp_obj_is_callable(self->sniffer_handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be a function."));
        }

        char _buffer[1024] = {0};
        self->sniffer_arg = mp_obj_new_tuple(2, NULL);
        self->sniffer_arg->items[0] = mp_obj_new_int(0);
        self->sniffer_arg->items[1] = mp_obj_new_bytearray(1024, _buffer);
    }

    wifi_promiscuous_filter_t filter;
    filter.filter_mask = args[ARG_filter].u_int;

    esp_wifi_set_promiscuous_filter(&filter);

    esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);

    esp_wifi_set_promiscuous(true);
    self->sniffer_running = true;

    esp_wifi_set_channel(args[ARG_channel].u_int, WIFI_SECOND_CHAN_NONE);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_network_if_start_sniffer_obj, 1, wifi_network_if_start_sniffer);

STATIC mp_obj_t wifi_network_if_stop_sniffer(mp_obj_t self_in) {
    wifi_network_if_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->sniffer_running) {
        mp_raise_OSError(MP_ERROR_TEXT("Sniffer is not running"));
    }

    esp_wifi_set_promiscuous(false);
    self->sniffer_running = false;

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_if_stop_sniffer_obj, wifi_network_if_stop_sniffer);


STATIC void wifi_network_if_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    wifi_network_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    wifi_config_t cfg;

    if (self->if_mode == WIFI_MODE_STA) {
        esp_exceptions(esp_wifi_get_config(WIFI_IF_STA, &cfg));
    } else if (self->if_mode == WIFI_MODE_AP) {
        esp_exceptions(esp_wifi_get_config(WIFI_IF_AP, &cfg));
    }

    if (dest[0] == MP_OBJ_NULL) {
        // Load
        if (attr == MP_QSTR_mode) {
            dest[0] = mp_obj_new_int(self->if_mode);
        } else if (attr == MP_QSTR_ssid) {
            if (self->if_mode == WIFI_MODE_STA) {
                dest[0] = mp_obj_new_str((char *)cfg.sta.ssid, strlen((char *)cfg.sta.ssid));
            } else if (self->if_mode == WIFI_MODE_AP) {
                dest[0] = mp_obj_new_str((char *)cfg.ap.ssid, cfg.ap.ssid_len);
            }
        } else if (attr == MP_QSTR_bssid) {
            dest[0] = self->bssid;
        } else if (attr == MP_QSTR_password) {
            if (self->if_mode == WIFI_MODE_STA) {
                dest[0] = mp_obj_new_str((char *)cfg.sta.password, strlen((char *)cfg.sta.password));
            } else if (self->if_mode == WIFI_MODE_AP) {
                dest[0] = mp_obj_new_str((char *)cfg.ap.password, strlen((char *)cfg.ap.password));
            }
        } else if (attr == MP_QSTR_last_wifi_event) {
            dest[0] = mp_obj_new_int(self->last_wifi_event);
        } else if (attr == MP_QSTR_last_ip_event) {
            dest[0] = mp_obj_new_int(self->last_ip_event);
        } else if (attr == MP_QSTR_connected) {
            dest[0] = mp_obj_new_bool(self->connected);
        } else if (attr == MP_QSTR_auto_reconnect) {
            dest[0] = mp_obj_new_bool(self->auto_reconnect);
        } else if (attr == MP_QSTR_wps_probing) {
            dest[0] = mp_obj_new_bool(self->wps_probing);
        } else if (attr == MP_QSTR_disconnect_reason) {
            dest[0] = mp_obj_new_int(self->disconnect_reason);
        } else if (attr == MP_QSTR_ip_addr) {
            dest[0] = netutils_format_ipv4_addr((uint8_t *)&(self->ip_addr), NETUTILS_BIG);
        } else if (attr == MP_QSTR_gw_addr) {
            dest[0] = netutils_format_ipv4_addr((uint8_t *)&(self->gw_addr), NETUTILS_BIG);
        } else if (attr == MP_QSTR_subnetmask) {
            dest[0] = netutils_format_ipv4_addr((uint8_t *)&(self->subnetmask), NETUTILS_BIG);
        } else if (attr == MP_QSTR_mac) {
            uint8_t mac[6];
            esp_exceptions(esp_wifi_get_mac(self->if_mode, mac));
            dest[0] = mp_obj_new_bytes(mac, sizeof(mac));
        } else if (attr == MP_QSTR_num_sta) {
            dest[0] = mp_obj_new_int(self->num_sta);
        } else if (attr == MP_QSTR_channel) {
            uint8_t primaryChan;
            wifi_second_chan_t secondChan;
            esp_wifi_get_channel(&primaryChan, &secondChan);
            dest[0] = mp_obj_new_int(primaryChan);
        } else if (attr == MP_QSTR_active) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_active_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_connect) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_connect_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_start_ap) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_start_ap_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_start_wps) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_start_wps_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_stop_ap) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_stop_ap_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_disconnect) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_disconnect_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_scan) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_scan_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_get_scan_results) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_get_scan_results_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_start_sniffer) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_start_sniffer_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_stop_sniffer) {
            dest[0] = MP_OBJ_FROM_PTR(&wifi_network_if_stop_sniffer_obj);
            dest[1] = self_in;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store
        if (attr == MP_QSTR_auto_reconnect) {
            self->auto_reconnect = mp_obj_get_int(dest[1]);
            dest[0] = MP_OBJ_NULL;
        }
        mp_raise_ValueError(MP_ERROR_TEXT("Parameter is Read Only"));
    }
};


const mp_obj_type_t wifi_network_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_Wifi,
    .print = wifi_network_if_print,
    .make_new = wifi_network_if_make_new,
    .attr = wifi_network_if_attr,
};
#endif
