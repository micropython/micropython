/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 * and Mnemote Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2017 Nick Moore @mnemote
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
 * Copyright (c) 2025 "Trent Walraven" <trwbox@gmail.com>
 *
 * Based on esp8266/modnetwork.c which is Copyright (c) 2015 Paul Sokolovsky
 * And the ESP IDF example code which is Public Domain / CC0
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

#include <string.h>

#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modnetwork.h"
#include "modnetwork.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_psram.h"

#ifndef NO_QSTR
#include "mdns.h"
#endif

#if MICROPY_PY_NETWORK_WLAN

#if (WIFI_MODE_STA & WIFI_MODE_AP != WIFI_MODE_NULL || WIFI_MODE_STA | WIFI_MODE_AP != WIFI_MODE_APSTA)
#error WIFI_MODE_STA and WIFI_MODE_AP are supposed to be bitfields!
#endif

typedef base_if_obj_t wlan_if_obj_t;

static wlan_if_obj_t wlan_sta_obj;
static wlan_if_obj_t wlan_ap_obj;

// Set to "true" if esp_wifi_start() was called
static bool wifi_started = false;

// Set to "true" if the STA interface is requested to be connected by the
// user, used for automatic reassociation.
static bool wifi_sta_connect_requested = false;

// Set to "true" if the STA interface is connected to wifi and has IP address.
static bool wifi_sta_connected = false;

// Store the current status. 0 means None here, safe to do so as first enum value is WIFI_REASON_UNSPECIFIED=1.
static uint8_t wifi_sta_disconn_reason = 0;

#if MICROPY_HW_ENABLE_MDNS_QUERIES || MICROPY_HW_ENABLE_MDNS_RESPONDER
// Whether mDNS has been initialised or not
static bool mdns_initialised = false;
#endif

static uint8_t conf_wifi_sta_reconnects = 0;
static uint8_t wifi_sta_reconnects;

// The rules for this default are defined in the documentation of esp_wifi_set_protocol()
// rather than in code, so we have to recreate them here.
#if CONFIG_SOC_WIFI_HE_SUPPORT
// Note: No Explicit support for 5GHz here, yet
#define WIFI_PROTOCOL_DEFAULT (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX)
#else
#define WIFI_PROTOCOL_DEFAULT (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N)
#endif

// TODO: Can/should these be dynamically allocated?
static uint8_t scan_config_ssid[33]; // The maximum length of an SSID is 32 bytes, plus a null terminator
static uint8_t scan_config_bssid[6];

static wifi_event_sta_scan_done_t scan_event_data;

// Set to "true" while there is an in-progress scan.
static bool scan_in_progress = false;
// If the scan in progress is a background scan
static bool scan_in_background = false;

// If a soft reset happens between the time a scan has started, and the scan is complete,
// then the callback function provided by the user is no longer valid and should not be called.
// So track the case of a soft-reboot
static bool soft_reboot_happened = false;

// Gets the wifi scan results and returns them as a list of tuples in the expected format.
static mp_obj_t read_wifi_scan_results() {
    ESP_LOGI("wifi", "Reading wifi scan results");

    // Create the list the results will be added to
    mp_obj_t list = mp_obj_new_list(0, NULL);

    uint16_t count = 0;
    esp_exceptions(esp_wifi_scan_get_ap_num(&count));
    if (count == 0) {
        // esp_wifi_scan_get_ap_records must be called to free internal buffers from the scan.
        // But it returns an error if wifi_ap_records==NULL. So allocate at least 1 AP entry.
        // esp_wifi_scan_get_ap_records will then return the actual number of APs in count.
        count = 1;
    }
    // The array to hold the scan results
    wifi_ap_record_t *wifi_ap_records = calloc(count, sizeof(wifi_ap_record_t));
    esp_exceptions(esp_wifi_scan_get_ap_records(&count, wifi_ap_records));
    for (uint16_t i = 0; i < count; i++) {
        // The new tuple for the record with the values (ssid, bssid, channel, rssi, security, hidden)
        mp_obj_tuple_t *t = mp_obj_new_tuple(6, NULL);
        uint8_t *x = memchr(wifi_ap_records[i].ssid, 0, sizeof(wifi_ap_records[i].ssid));
        int ssid_len = x ? x - wifi_ap_records[i].ssid : sizeof(wifi_ap_records[i].ssid);
        t->items[0] = mp_obj_new_bytes(wifi_ap_records[i].ssid, ssid_len);
        t->items[1] = mp_obj_new_bytes(wifi_ap_records[i].bssid, sizeof(wifi_ap_records[i].bssid));
        t->items[2] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].primary);
        t->items[3] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].rssi);
        t->items[4] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].authmode);
        // Hidden is not a direct field in ESP-IDF wifi, but setting show_hidden=False will cause all networks
        //       with no SSID to be removed from the list even if the BSSID is explicitly set.
        if (ssid_len == 0) {
            // If the SSID length is 0, then it is a hidden network
            t->items[5] = mp_const_true;
        } else {
            // Otherwise, it is not a hidden network
            t->items[5] = mp_const_false;
        }
        mp_obj_list_append(list, MP_OBJ_FROM_PTR(t));
    }
    // Free the array of scan results
    free(wifi_ap_records);

    ESP_LOGI("wifi", "Read %d scan results into the list", count);
    return list;
}

// This function is used to get execution back into the main MicroPython thread during the wifi event handler
// this is done to allow this function have MicroPython exceptions raised and handled, when setting up and calling
// the user's callback function after a background wifi scan has completed.
static mp_obj_t scan_done_cb(mp_obj_t arg) {
    ESP_LOGI("wifi_callback", "Scan status: %lu", scan_event_data.status);

    bool skip_callback = false;
    // Check if a soft reboot happened, making the user callback invalid
    if (soft_reboot_happened) {
        ESP_LOGI("wifi_callback", "Soft reboot happened, skipping user callback");
        // Reset the flag so that it doesn't affect future scans
        soft_reboot_happened = false;
        skip_callback = true;
    }

    // We always need to save the results from the scan, even if the user callback is not made
    // as that is the only way that the results are freed from memory.
    mp_obj_t results_list = read_wifi_scan_results();

    // The scan is done, so set the pair of flags to false
    scan_in_progress = false;
    scan_in_background = false;

    if (!skip_callback) {
        // Get the user's callback function
        mp_obj_t user_callback = MP_STATE_VM(user_scan_callback);
        // Create a new bool object to indicate the status of the scan
        mp_obj_t status = mp_obj_new_bool(scan_event_data.status == 0);

        // Setup the tuple for the user callback
        mp_obj_t callback_args[2] = {
            status, // The status of the scan
            results_list // The list of results from the scan
        };
        mp_obj_t callback_args_tuple = mp_obj_new_tuple(2, callback_args);

        ESP_LOGI("wifi", "Results gathered, calling user provided callback function.");
        // Make the protected call to the user's callback function with the arguments.
        mp_call_function_2_protected(user_callback, MP_OBJ_FROM_PTR(&wlan_sta_obj), callback_args_tuple);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(scan_done_cb_obj, scan_done_cb);

// This function is called by the system-event task and so runs in a different
// thread to the main MicroPython task.  It must not raise any Python exceptions.
static void network_wlan_wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI("wifi", "STA_START");
            wlan_sta_obj.active = true;
            wifi_sta_reconnects = 0;
            break;

        case WIFI_EVENT_STA_STOP:
            wlan_sta_obj.active = false;
            break;

        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI("network", "CONNECTED");
            break;

        case WIFI_EVENT_STA_DISCONNECTED: {
            // This is a workaround as ESP32 WiFi libs don't currently
            // auto-reassociate.

            wifi_event_sta_disconnected_t *disconn = event_data;
            char *message = "";
            wifi_sta_disconn_reason = disconn->reason;
            switch (disconn->reason) {
                case WIFI_REASON_BEACON_TIMEOUT:
                    // AP has dropped out; try to reconnect.
                    message = "beacon timeout";
                    break;
                case WIFI_REASON_NO_AP_FOUND:
                    // AP may not exist, or it may have momentarily dropped out; try to reconnect.
                    message = "no AP found";
                    break;
                case WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD:
                    // No AP with RSSI within given threshold exists, or it may have momentarily dropped out; try to reconnect.
                    message = "no AP with RSSI within threshold found";
                    break;
                case WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD:
                    // No AP with authmode within given threshold exists, or it may have momentarily dropped out; try to reconnect.
                    message = "no AP with authmode within threshold found";
                    break;
                case WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY:
                    // No AP with compatible security exists, or it may have momentarily dropped out; try to reconnect.
                    message = "no AP with compatible security found";
                    break;
                case WIFI_REASON_AUTH_FAIL:
                    // Password may be wrong, or it just failed to connect; try to reconnect.
                    message = "authentication failed";
                    break;
                default:
                    // Let other errors through and try to reconnect.
                    break;
            }
            ESP_LOGI("wifi", "STA_DISCONNECTED, reason:%d:%s", disconn->reason, message);

            wifi_sta_connected = false;
            if (wifi_sta_connect_requested) {
                wifi_mode_t mode;
                if (esp_wifi_get_mode(&mode) != ESP_OK) {
                    break;
                }
                if (!(mode & WIFI_MODE_STA)) {
                    break;
                }
                if (conf_wifi_sta_reconnects) {
                    ESP_LOGI("wifi", "reconnect counter=%d, max=%d",
                        wifi_sta_reconnects, conf_wifi_sta_reconnects);
                    if (++wifi_sta_reconnects >= conf_wifi_sta_reconnects) {
                        break;
                    }
                }
                esp_err_t e = esp_wifi_connect();
                if (e != ESP_OK) {
                    ESP_LOGI("wifi", "error attempting to reconnect: 0x%04x", e);
                }
            }
            break;
        }

        case WIFI_EVENT_AP_START:
            wlan_ap_obj.active = true;
            break;

        case WIFI_EVENT_AP_STOP:
            wlan_ap_obj.active = false;
            break;

        case WIFI_EVENT_SCAN_DONE:
            // If the scan was a background scan we need to call the user callback
            if (scan_in_background) {
                ESP_LOGI("wifi", "Scan completed when non-blocking scan was requested. Running callback");
                // This event is triggered whenever a wifi scan completes
                wifi_event_sta_scan_done_t *scan_done = event_data;
                // Copy the scan data to a global so it doesn't get removed before the callback is called
                memcpy(&scan_event_data, scan_done, sizeof(wifi_event_sta_scan_done_t));

                // Since the system thread can't raise exceptions, schedule our handler in the micropython thread
                if (!mp_sched_schedule(MP_OBJ_FROM_PTR(&scan_done_cb_obj), mp_const_none)) {
                    ESP_LOGE("wifi", "Failed to schedule internal scan_done callback");
                }
            } else if (soft_reboot_happened) {
                // Since this method is always called after a scan, it can be used to reset the soft_reset flag
                // if the reset occurred when there was a scan happening, but it was not a non-blocking scan.
                // TODO: Not sure that this branch is ever take, but in the event it is, I think it might need to run
                //      read_wifi_scan_results() to free the scan results from memory?
                soft_reboot_happened = false;
            }
            break;
        default:
            break;
    }
}

// This is called from the Ctrl-D soft reboot to handle when there is an in-progress scan.
void network_wlan_deinit(void) {
    if (scan_in_progress) {
        ESP_LOGI("wifi", "soft reboot is happening, while a scan is in progress, setting the flag to ignore scan callback");
        soft_reboot_happened = true;
    }
}

static void network_wlan_ip_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI("network", "GOT_IP");
            wifi_sta_connected = true;
            wifi_sta_disconn_reason = 0; // Success so clear error. (in case of new error will be replaced anyway)
            #if MICROPY_HW_ENABLE_MDNS_QUERIES || MICROPY_HW_ENABLE_MDNS_RESPONDER
            if (!mdns_initialised) {
                mdns_init();
                #if MICROPY_HW_ENABLE_MDNS_RESPONDER
                mdns_hostname_set(mod_network_hostname_data);
                mdns_instance_name_set(mod_network_hostname_data);
                #endif
                mdns_initialised = true;
            }
            #endif
            break;

        default:
            break;
    }
}

static void require_if(mp_obj_t wlan_if, int if_no) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(wlan_if);
    if (self->if_id != if_no) {
        mp_raise_msg(&mp_type_OSError, if_no == ESP_IF_WIFI_STA ? MP_ERROR_TEXT("STA required") : MP_ERROR_TEXT("AP required"));
    }
}

void esp_initialise_wifi(void) {
    static int wifi_initialized = 0;
    if (!wifi_initialized) {
        esp_exceptions(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, network_wlan_wifi_event_handler, NULL, NULL));
        esp_exceptions(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, network_wlan_ip_event_handler, NULL, NULL));

        wlan_sta_obj.base.type = &esp_network_wlan_type;
        wlan_sta_obj.if_id = ESP_IF_WIFI_STA;
        wlan_sta_obj.netif = esp_netif_create_default_wifi_sta();
        wlan_sta_obj.active = false;

        wlan_ap_obj.base.type = &esp_network_wlan_type;
        wlan_ap_obj.if_id = ESP_IF_WIFI_AP;
        wlan_ap_obj.netif = esp_netif_create_default_wifi_ap();
        wlan_ap_obj.active = false;

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        #if CONFIG_SPIRAM_IGNORE_NOTFOUND
        if (!esp_psram_is_initialized()) {
            // If PSRAM failed to initialize, disable "Wi-Fi Cache TX Buffers"
            // (default SPIRAM config ESP32_WIFI_CACHE_TX_BUFFER_NUM==32, this is 54,400 bytes of heap)
            cfg.cache_tx_buf_num = 0;
            cfg.feature_caps &= ~CONFIG_FEATURE_CACHE_TX_BUF_BIT;

            // Set some other options back to the non-SPIRAM default values
            // to save more RAM.
            //
            // These can be determined from ESP-IDF components/esp_wifi/Kconfig and the
            // WIFI_INIT_CONFIG_DEFAULT macro
            cfg.tx_buf_type = 1; // Dynamic, this "magic number" is defined in IDF KConfig
            cfg.static_tx_buf_num = 0; // Probably don't need, due to tx_buf_type
            cfg.dynamic_tx_buf_num = 32; // ESP-IDF default value (maximum)
        }
        #endif
        ESP_LOGD("modnetwork", "Initializing WiFi");
        esp_exceptions(esp_wifi_init(&cfg));
        esp_exceptions(esp_wifi_set_storage(WIFI_STORAGE_RAM));

        ESP_LOGD("modnetwork", "Initialized");
        wifi_initialized = 1;
    }
}

static mp_obj_t network_wlan_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    esp_initialise_wifi();

    int idx = (n_args > 0) ? mp_obj_get_int(args[0]) : ESP_IF_WIFI_STA;
    if (idx == ESP_IF_WIFI_STA) {
        return MP_OBJ_FROM_PTR(&wlan_sta_obj);
    } else if (idx == ESP_IF_WIFI_AP) {
        return MP_OBJ_FROM_PTR(&wlan_ap_obj);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid WLAN interface identifier"));
    }
}

static mp_obj_t network_wlan_active(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    wifi_mode_t mode;
    if (!wifi_started) {
        mode = WIFI_MODE_NULL;
    } else {
        esp_exceptions(esp_wifi_get_mode(&mode));
    }

    int bit = (self->if_id == ESP_IF_WIFI_STA) ? WIFI_MODE_STA : WIFI_MODE_AP;

    if (n_args > 1) {
        bool active = mp_obj_is_true(args[1]);
        mode = active ? (mode | bit) : (mode & ~bit);
        if (mode == WIFI_MODE_NULL) {
            if (wifi_started) {
                esp_exceptions(esp_wifi_stop());
                wifi_started = false;
            }
        } else {
            esp_exceptions(esp_wifi_set_mode(mode));
            if (!wifi_started) {
                esp_exceptions(esp_wifi_start());
                wifi_started = true;
            }
        }

        // Wait for the interface to be in the correct state.
        while (self->active != active) {
            MICROPY_EVENT_POLL_HOOK;
        }
    }

    return (mode & bit) ? mp_const_true : mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_wlan_active_obj, 1, 2, network_wlan_active);

static mp_obj_t network_wlan_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_key, ARG_bssid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    wifi_config_t wifi_sta_config = {0};

    // configure any parameters that are given
    if (n_args > 1) {
        size_t len;
        const char *p;
        if (args[ARG_ssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
            memcpy(wifi_sta_config.sta.ssid, p, MIN(len, sizeof(wifi_sta_config.sta.ssid)));
        }
        if (args[ARG_key].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_key].u_obj, &len);
            memcpy(wifi_sta_config.sta.password, p, MIN(len, sizeof(wifi_sta_config.sta.password)));
        }
        if (args[ARG_bssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_bssid].u_obj, &len);
            if (len != sizeof(wifi_sta_config.sta.bssid)) {
                mp_raise_ValueError(NULL);
            }
            wifi_sta_config.sta.bssid_set = 1;
            memcpy(wifi_sta_config.sta.bssid, p, sizeof(wifi_sta_config.sta.bssid));
        }
        esp_exceptions(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_sta_config));
    }

    esp_exceptions(esp_netif_set_hostname(wlan_sta_obj.netif, mod_network_hostname_data));

    wifi_sta_reconnects = 0;
    // connect to the WiFi AP
    MP_THREAD_GIL_EXIT();
    esp_exceptions(esp_wifi_connect());
    MP_THREAD_GIL_ENTER();
    wifi_sta_connect_requested = true;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_wlan_connect_obj, 1, network_wlan_connect);

static mp_obj_t network_wlan_disconnect(mp_obj_t self_in) {
    wifi_sta_connect_requested = false;
    esp_exceptions(esp_wifi_disconnect());
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_disconnect_obj, network_wlan_disconnect);

static mp_obj_t network_wlan_status(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        if (self->if_id == ESP_IF_WIFI_STA) {
            // Case of no arg is only for the STA interface
            if (wifi_sta_connected) {
                // Happy path, connected with IP
                return MP_OBJ_NEW_SMALL_INT(STAT_GOT_IP);
            } else if (wifi_sta_disconn_reason == WIFI_REASON_NO_AP_FOUND) {
                return MP_OBJ_NEW_SMALL_INT(WIFI_REASON_NO_AP_FOUND);
            } else if (wifi_sta_disconn_reason == WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD) {
                return MP_OBJ_NEW_SMALL_INT(WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD);
            } else if (wifi_sta_disconn_reason == WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD) {
                return MP_OBJ_NEW_SMALL_INT(WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD);
            } else if (wifi_sta_disconn_reason == WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY) {
                return MP_OBJ_NEW_SMALL_INT(WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY);
            } else if ((wifi_sta_disconn_reason == WIFI_REASON_AUTH_FAIL) || (wifi_sta_disconn_reason == WIFI_REASON_CONNECTION_FAIL)) {
                // wrong password
                return MP_OBJ_NEW_SMALL_INT(WIFI_REASON_AUTH_FAIL);
            } else if (wifi_sta_disconn_reason == WIFI_REASON_ASSOC_LEAVE) {
                // After wlan.disconnect()
                return MP_OBJ_NEW_SMALL_INT(STAT_IDLE);
            } else if (wifi_sta_connect_requested
                       && (conf_wifi_sta_reconnects == 0
                           || wifi_sta_reconnects < conf_wifi_sta_reconnects)) {
                // No connection or error, but is requested = Still connecting
                return MP_OBJ_NEW_SMALL_INT(STAT_CONNECTING);
            } else if (wifi_sta_disconn_reason == 0) {
                // No activity, No error = Idle
                return MP_OBJ_NEW_SMALL_INT(STAT_IDLE);
            } else {
                // Simply pass the error through from ESP-identifier
                return MP_OBJ_NEW_SMALL_INT(wifi_sta_disconn_reason);
            }
        }
        return mp_const_none;
    }

    // one argument: return status based on query parameter
    switch ((uintptr_t)args[1]) {
        case (uintptr_t)MP_OBJ_NEW_QSTR(MP_QSTR_stations): {
            // return list of connected stations, only if in soft-AP mode
            require_if(args[0], ESP_IF_WIFI_AP);
            wifi_sta_list_t station_list;
            esp_exceptions(esp_wifi_ap_get_sta_list(&station_list));
            wifi_sta_info_t *stations = (wifi_sta_info_t *)station_list.sta;
            mp_obj_t list = mp_obj_new_list(0, NULL);
            for (int i = 0; i < station_list.num; ++i) {
                mp_obj_tuple_t *t = mp_obj_new_tuple(1, NULL);
                t->items[0] = mp_obj_new_bytes(stations[i].mac, sizeof(stations[i].mac));
                mp_obj_list_append(list, t);
            }
            return list;
        }
        case (uintptr_t)MP_OBJ_NEW_QSTR(MP_QSTR_rssi): {
            // return signal of AP, only in STA mode
            require_if(args[0], ESP_IF_WIFI_STA);

            wifi_ap_record_t info;
            esp_exceptions(esp_wifi_sta_get_ap_info(&info));
            return MP_OBJ_NEW_SMALL_INT(info.rssi);
        }
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_wlan_status_obj, 1, 2, network_wlan_status);

static mp_obj_t network_wlan_scan(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_self, ARG_blocking, ARG_callback, ARG_channel, ARG_ssid, ARG_bssid, ARG_show_hidden,
           ARG_active_scan, ARG_scan_time_passive, ARG_scan_time_active_min, ARG_scan_time_active_max,
           ARG_home_chan_dwell_time};

    static const mp_arg_t allowed_args[] = {
        // Create the list of optional elements that can be given, starting with the self object that
        // will always exist
        { MP_QSTR_self, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none }},
        // If the scan should be sent to the blocking
        { MP_QSTR_blocking, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_bool = true }},
        // Callback function that will be called with the results from the scan after the non-blocking scan has completed
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none }},
        // The channel, or list of channels that should be scanned
        { MP_QSTR_channel, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none }},
        // The ssid string that should be scanned for, default of None
        { MP_QSTR_ssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none }},
        // The mac address bssid as bytes that should be scanned for, default is None
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none }},
        // If hidden networks should be shown, default of true based on the original implementation
        { MP_QSTR_show_hidden, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_bool = true }},
        // If the scan should be active with WIFI_SCAN_TYPE_ACTIVE, default of true based on the ESP-IDF defaults
        { MP_QSTR_active_scan, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_bool = true }},
        // Time to passive scan, default 360 ms based on the ESP-IDF defaults
        { MP_QSTR_scan_time_passive, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 360 }},
        // Min time to spend actively scanning, default of 0 ms based on the ESP-IDF defaults
        { MP_QSTR_scan_time_active_min, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 0 }},
        // Max time to spend actively scanning, default of 120 ms based on the ESP-IDF defaults
        { MP_QSTR_scan_time_active_max, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 120 }},
        // Time to spend on the home channel between hops if connected to a wifi network to not lose connection,
        // defaults to 30ms based on the ESP-IDF defaults
        { MP_QSTR_home_chan_dwell_time, MP_ARG_KW_ONLY | MP_ARG_INT, { .u_int = 30 }}
    };

    // Get the args to the function based on the keywords
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // check that STA mode is active
    wifi_mode_t mode;
    esp_exceptions(esp_wifi_get_mode(&mode));
    if ((mode & WIFI_MODE_STA) == 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("STA must be active"));
    }

    // Raise an error if the scan is already happening
    if (scan_in_progress) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("WiFi scan already in progress, wait for it to finish"));
    } else if (!scan_in_progress && soft_reboot_happened) {
        // This is a weird state to be in, since the soft_reboot flag should only be set when scan_in_progress is set
        ESP_LOGW("wifi", "Scan not in progress with soft_reboot_happened set, resetting the flag");
        // Reset the flag if somehow this is the case
        soft_reboot_happened = false;
    }

    // Create the scan config object to be filled in with the scan parameters
    wifi_scan_config_t config = { 0 };

    config.show_hidden = args[ARG_show_hidden].u_bool;

    config.scan_type = args[ARG_active_scan].u_bool ? WIFI_SCAN_TYPE_ACTIVE : WIFI_SCAN_TYPE_PASSIVE;

    if (args[ARG_home_chan_dwell_time].u_int < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("home_chan_dwell_time must be a positive integer"));
    } else if (args[ARG_home_chan_dwell_time].u_int > 255) {
        mp_raise_ValueError(MP_ERROR_TEXT("home_chan_dwell_time must be less than 256"));
    }
    config.home_chan_dwell_time = (uint8_t)args[ARG_home_chan_dwell_time].u_int;

    // Set the scan times
    if (args[ARG_scan_time_passive].u_int < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("scan_time_passive must be a positive integer"));
    } else if (args[ARG_scan_time_passive].u_int > 1500) {
        // TODO: It appears that the ESP-IDF sets a hard limit of 1500 ms for the passive scan time,
        //       but the documentation reads more like a recommendation than a hard limit.
        //       So, should this be a warning instead of an error?
        mp_raise_ValueError(MP_ERROR_TEXT("scan_time_passive must be less than or equal to 1500"));
    }
    config.scan_time.passive = args[ARG_scan_time_passive].u_int;

    if (args[ARG_scan_time_active_min].u_int < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("scan_time_active_min must be a positive integer"));
    } else if (args[ARG_scan_time_active_min].u_int > 1500) {
        // TODO: There is no note in the documentation about a limit for the min scan time, but logically
        //       it would also not be allowed over the 1500 ms limit if the max is 1500ms?
        mp_raise_ValueError(MP_ERROR_TEXT("scan_time_active_min must be less than or equal to 1500"));
    }

    if (args[ARG_scan_time_active_max].u_int < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("scan_time_active_max must be a positive integer"));
    } else if (args[ARG_scan_time_active_max].u_int > 1500) {
        // TODO: There appears to be a hard limit of 1500 ms for the active scan time in the ESP-IDF,
        //       but the documentation reads more like a recommendation than a hard limit.
        //       So, should this be a warning instead of an error?
        mp_raise_ValueError(MP_ERROR_TEXT("scan_time_active_max must be less than or equal to 1500"));
    }
    if (args[ARG_scan_time_active_min].u_int > args[ARG_scan_time_active_max].u_int) {
        mp_raise_ValueError(MP_ERROR_TEXT("scan_time_active_min must be less than or equal to scan_time_active_max"));
    }
    config.scan_time.active.min = args[ARG_scan_time_active_min].u_int;
    config.scan_time.active.max = args[ARG_scan_time_active_max].u_int;


    if (args[ARG_ssid].u_obj != mp_const_none) {
        size_t len;
        const char *ssid_str;

        ssid_str = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
        // Maximum length of the SSID is 32 bytes, array is 33 bytes to include the null terminator
        if (len >= sizeof(scan_config_ssid)) {
            mp_raise_ValueError(MP_ERROR_TEXT("SSID too long"));
        }
        // Clear the SSID array before copying the new SSID into it
        memset(scan_config_ssid, 0, sizeof(scan_config_ssid));
        // TODO: This is technically an *uint8_t for the SSID in the ESP-IDF, does signedness of char matter?
        // Copy the SSID into the scan_config_ssid array
        memcpy(scan_config_ssid, ssid_str, len);
        config.ssid = scan_config_ssid;
    }

    if (args[ARG_bssid].u_obj != mp_const_none) {
        size_t len;
        const char *addr_bytes;

        addr_bytes = mp_obj_str_get_data(args[ARG_bssid].u_obj, &len);
        if (len != sizeof(scan_config_bssid)) {
            mp_raise_ValueError(MP_ERROR_TEXT("BSSID must be 6 bytes"));
        }
        // Clear the BSSID array before copying the new BSSID into it.
        // This probably isn't strictly required since it will always be 6 bytes, but a good practice
        memset(scan_config_bssid, 0, sizeof(scan_config_bssid));

        // TODO: This is technically an *uint8_t for the BSSID in the ESP-IDF, does signedness of char matter?
        memcpy(scan_config_bssid, addr_bytes, sizeof(scan_config_bssid));
        config.bssid = scan_config_bssid;
    }

    if (mp_obj_is_int(args[ARG_channel].u_obj)) {
        // If the channel argument is a single channel, set the channel in the config
        // NOTE: This check won't be good enough if support for 5GHz is added with the esp32-c5
        // TODO: Should this allow for an int 0 which would cause a scan of all channels?
        if (mp_obj_get_int(args[ARG_channel].u_obj) < 1 || mp_obj_get_int(args[ARG_channel].u_obj) > 14) {
            mp_raise_ValueError(MP_ERROR_TEXT("Unsupported channel number"));
        }

        config.channel = mp_obj_get_int(args[ARG_channel].u_obj);
    } else if (mp_obj_is_type(args[ARG_channel].u_obj, &mp_type_list) || mp_obj_is_type(args[ARG_channel].u_obj, &mp_type_tuple)) {
        // If channel argument is a list or tuple, try to use the channel bitmap to scan them all
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
        // The channel bitmap only supports ESP-IDF versions 5.3.0 and above
        // Start with the empty bitmap
        wifi_scan_channel_bitmap_t channel_bitmap = {0};

        // Get the list of channels
        mp_obj_t channel_list = args[ARG_channel].u_obj;
        int channel_count = mp_obj_get_int(mp_obj_len(channel_list));
        ESP_LOGI("wifi", "There were %d channels given in the channel list", channel_count);

        if (channel_count == 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("No channels given in channel list"));
        }

        // Get the channels themselves
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(channel_list, channel_count, &items);
        for (int i = 0; i < channel_count; i++) {
            if (!mp_obj_is_int(items[i])) {
                mp_raise_TypeError(MP_ERROR_TEXT("The wifi channels must be specified as integers"));
            }

            // Check that it is a valid channel
            int temp_channel_number = mp_obj_get_int(items[i]);
            // NOTE: This check won't be good enough if support for 5GHz is added with the esp32-c5
            if (temp_channel_number < 1 || temp_channel_number > 14) {
                mp_raise_ValueError(MP_ERROR_TEXT("Unsupported channel number in channel list"));
            }

            // Set the channel in the bitmap
            channel_bitmap.ghz_2_channels |= 1 << (temp_channel_number);
        }

        // Set the channel bitmap, and clear the channel number since 0 is required for the bitmap
        config.channel = 0;
        config.channel_bitmap = channel_bitmap;
        #else
        // If the version is less than 5.3.0, raise an error when trying to use a list or tuple of channels
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Using a list of channels is not supported on this version of ESP-IDF"));
        #endif
    } else if (args[ARG_channel].u_obj == mp_const_none) {
        // If no channel is given, set the channel to 0 to scan all channels
        config.channel = 0;
    } else {
        // If it is neither of those, raise an error
        mp_raise_msg(&mp_type_TypeError, MP_ERROR_TEXT("Channel must be an integer or a list of integers"));
    }

    // If the scan is non-blocking, run checks on the callback
    if (args[ARG_blocking].u_bool == false) {
        // Check that a callback is given
        if (args[ARG_callback].u_obj == mp_const_none) {
            mp_raise_TypeError(MP_ERROR_TEXT("Callback must be given for non-blocking scan"));
        }

        // Check that the callback is a callable object
        if (!mp_obj_is_callable(args[ARG_callback].u_obj)) {
            mp_raise_TypeError(MP_ERROR_TEXT("Callback must be a callable object"));
        }

        // Set the callback to the global callback object
        MP_STATE_VM(user_scan_callback) = args[ARG_callback].u_obj;
    } else if (args[ARG_callback].u_obj != mp_const_none) {
        // TODO: Should the callback be allowed for blocking scans too if desired?
        //      I don't think so, since that could lead to backwards compatibility issues.
        // If the scan is blocking, but a callback is given, raise an error
        mp_raise_TypeError(MP_ERROR_TEXT("Callback cannot be given for blocking scan"));
    }

    MP_THREAD_GIL_EXIT();
    // Set the flags for the scan that is going to happen
    scan_in_progress = true;
    scan_in_background = !args[ARG_blocking].u_bool;
    esp_err_t status = esp_wifi_scan_start(&config, args[ARG_blocking].u_bool);
    MP_THREAD_GIL_ENTER();

    if (!args[ARG_blocking].u_bool) {
        // If this is a non-blocking scan check the error code
        // NOTE: Choosing not to use esp_exceptions here to make it more clear it was a non-blocking scan,
        //       but unsure if that is the correct approach.
        if (status != ESP_OK) {
            // Clear the scan in flags if there was an error
            scan_in_progress = false;
            scan_in_background = false;
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Non-blocking WiFi scan failed: %s"), esp_err_to_name(status));
        }
        ESP_LOGI("wifi", "The non-blocking scan started successfully, with a callback upon completion.");

        // Then return None immediately since the callback will handle the rest
        return mp_const_none;
    }

    // If the scan was blacking, it will have completed or errored by reaching this code
    // Verify the status of the scan
    esp_exceptions(status);
    // If the scan was successful, read the results and return them
    mp_obj_t list = read_wifi_scan_results();
    // Clear the scan in progress flag
    scan_in_progress = false;
    ESP_LOGI("wifi", "The blocking scan completed successfully, returning the results.");
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_wlan_scan_obj, 1, network_wlan_scan);

// TODO: Should this be a property somehow instead of a method?
// This allows the user to check if a scan is in progress. This is useful for a
// non-blocking scan, but also in a multi-threaded environment.
static mp_obj_t network_wlan_scan_in_progress(mp_obj_t self_in) {
    return mp_obj_new_bool(scan_in_progress);
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_scan_in_progress_obj, network_wlan_scan_in_progress);

static mp_obj_t network_wlan_isconnected(mp_obj_t self_in) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->if_id == ESP_IF_WIFI_STA) {
        return mp_obj_new_bool(wifi_sta_connected);
    } else {
        wifi_sta_list_t sta;
        esp_wifi_ap_get_sta_list(&sta);
        return mp_obj_new_bool(sta.num != 0);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_wlan_isconnected_obj, network_wlan_isconnected);

static mp_obj_t network_wlan_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (n_args != 1 && kwargs->used != 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("either pos or kw args are allowed"));
    }

    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    bool is_wifi = self->if_id == ESP_IF_WIFI_AP || self->if_id == ESP_IF_WIFI_STA;

    wifi_config_t cfg;
    if (is_wifi) {
        esp_exceptions(esp_wifi_get_config(self->if_id, &cfg));
    }

    if (kwargs->used != 0) {
        if (!is_wifi) {
            goto unknown;
        }

        for (size_t i = 0; i < kwargs->alloc; i++) {
            if (mp_map_slot_is_filled(kwargs, i)) {
                int req_if = -1;

                switch (mp_obj_str_get_qstr(kwargs->table[i].key)) {
                    case MP_QSTR_mac: {
                        mp_buffer_info_t bufinfo;
                        mp_get_buffer_raise(kwargs->table[i].value, &bufinfo, MP_BUFFER_READ);
                        if (bufinfo.len != 6) {
                            mp_raise_ValueError(MP_ERROR_TEXT("invalid buffer length"));
                        }
                        esp_exceptions(esp_wifi_set_mac(self->if_id, bufinfo.buf));
                        break;
                    }
                    case MP_QSTR_ssid:
                    case MP_QSTR_essid: {
                        req_if = ESP_IF_WIFI_AP;
                        size_t len;
                        const char *s = mp_obj_str_get_data(kwargs->table[i].value, &len);
                        len = MIN(len, sizeof(cfg.ap.ssid));
                        memcpy(cfg.ap.ssid, s, len);
                        cfg.ap.ssid_len = len;
                        break;
                    }
                    case MP_QSTR_hidden: {
                        req_if = ESP_IF_WIFI_AP;
                        cfg.ap.ssid_hidden = mp_obj_is_true(kwargs->table[i].value);
                        break;
                    }
                    case MP_QSTR_security:
                    case MP_QSTR_authmode: {
                        req_if = ESP_IF_WIFI_AP;
                        cfg.ap.authmode = mp_obj_get_int(kwargs->table[i].value);
                        break;
                    }
                    case MP_QSTR_key:
                    case MP_QSTR_password: {
                        req_if = ESP_IF_WIFI_AP;
                        size_t len;
                        const char *s = mp_obj_str_get_data(kwargs->table[i].value, &len);
                        len = MIN(len, sizeof(cfg.ap.password) - 1);
                        memcpy(cfg.ap.password, s, len);
                        cfg.ap.password[len] = 0;
                        break;
                    }
                    case MP_QSTR_channel: {
                        uint8_t channel = mp_obj_get_int(kwargs->table[i].value);
                        if (self->if_id == ESP_IF_WIFI_AP) {
                            cfg.ap.channel = channel;
                        } else {
                            // This setting is only used to determine the
                            // starting channel for a scan, so it can result in
                            // slightly faster connection times.
                            cfg.sta.channel = channel;

                            // This additional code to directly set the channel
                            // on the STA interface is only relevant for ESP-NOW
                            // (when there is no STA connection attempt.)
                            uint8_t old_primary;
                            wifi_second_chan_t secondary;
                            // Get the current value of secondary
                            esp_exceptions(esp_wifi_get_channel(&old_primary, &secondary));
                            esp_err_t err = esp_wifi_set_channel(channel, secondary);
                            if (err == ESP_ERR_INVALID_ARG) {
                                // May need to swap secondary channel above to below or below to above
                                secondary = (
                                    (secondary == WIFI_SECOND_CHAN_ABOVE)
                                    ? WIFI_SECOND_CHAN_BELOW
                                    : (secondary == WIFI_SECOND_CHAN_BELOW)
                                    ? WIFI_SECOND_CHAN_ABOVE
                                    : WIFI_SECOND_CHAN_NONE);
                                err = esp_wifi_set_channel(channel, secondary);
                            }
                            esp_exceptions(err);
                            if (channel != old_primary) {
                                // Workaround the ESP-IDF Wi-Fi stack sometimes taking a moment to change channels
                                mp_hal_delay_ms(1);
                            }
                        }
                        break;
                    }
                    case MP_QSTR_hostname:
                    case MP_QSTR_dhcp_hostname: {
                        // TODO: Deprecated. Use network.hostname(name) instead.
                        mod_network_hostname(1, &kwargs->table[i].value);
                        break;
                    }
                    case MP_QSTR_max_clients: {
                        req_if = ESP_IF_WIFI_AP;
                        cfg.ap.max_connection = mp_obj_get_int(kwargs->table[i].value);
                        break;
                    }
                    case MP_QSTR_reconnects: {
                        int reconnects = mp_obj_get_int(kwargs->table[i].value);
                        req_if = ESP_IF_WIFI_STA;
                        // parameter reconnects == -1 means to retry forever.
                        // here means conf_wifi_sta_reconnects == 0 to retry forever.
                        conf_wifi_sta_reconnects = (reconnects == -1) ? 0 : reconnects + 1;
                        break;
                    }
                    case MP_QSTR_txpower: {
                        int8_t power = (mp_obj_get_float(kwargs->table[i].value) * 4);
                        esp_exceptions(esp_wifi_set_max_tx_power(power));
                        break;
                    }
                    case MP_QSTR_protocol: {
                        esp_exceptions(esp_wifi_set_protocol(self->if_id, mp_obj_get_int(kwargs->table[i].value)));
                        break;
                    }
                    case MP_QSTR_pm: {
                        esp_exceptions(esp_wifi_set_ps(mp_obj_get_int(kwargs->table[i].value)));
                        break;
                    }
                    default:
                        goto unknown;
                }

                // We post-check interface requirements to save on code size
                if (req_if >= 0) {
                    require_if(args[0], req_if);
                }
            }
        }

        esp_exceptions(esp_wifi_set_config(self->if_id, &cfg));

        return mp_const_none;
    }

    // Get config

    if (n_args != 2) {
        mp_raise_TypeError(MP_ERROR_TEXT("can query only one param"));
    }

    int req_if = -1;
    mp_obj_t val = mp_const_none;

    switch (mp_obj_str_get_qstr(args[1])) {
        case MP_QSTR_mac: {
            uint8_t mac[6];
            switch (self->if_id) {
                case ESP_IF_WIFI_AP: // fallthrough intentional
                case ESP_IF_WIFI_STA:
                    esp_exceptions(esp_wifi_get_mac(self->if_id, mac));
                    return mp_obj_new_bytes(mac, sizeof(mac));
                default:
                    goto unknown;
            }
        }
        case MP_QSTR_ssid:
        case MP_QSTR_essid:
            switch (self->if_id) {
                case ESP_IF_WIFI_STA:
                    val = mp_obj_new_str_from_cstr((char *)cfg.sta.ssid);
                    break;
                case ESP_IF_WIFI_AP:
                    val = mp_obj_new_str((char *)cfg.ap.ssid, cfg.ap.ssid_len);
                    break;
                default:
                    req_if = ESP_IF_WIFI_AP;
            }
            break;
        case MP_QSTR_hidden:
            req_if = ESP_IF_WIFI_AP;
            val = mp_obj_new_bool(cfg.ap.ssid_hidden);
            break;
        case MP_QSTR_security:
        case MP_QSTR_authmode:
            req_if = ESP_IF_WIFI_AP;
            val = MP_OBJ_NEW_SMALL_INT(cfg.ap.authmode);
            break;
        case MP_QSTR_channel: {
            uint8_t channel;
            wifi_second_chan_t second;
            esp_exceptions(esp_wifi_get_channel(&channel, &second));
            val = MP_OBJ_NEW_SMALL_INT(channel);
            break;
        }
        case MP_QSTR_ifname: {
            val = esp_ifname(self->netif);
            break;
        }
        case MP_QSTR_hostname:
        case MP_QSTR_dhcp_hostname: {
            // TODO: Deprecated. Use network.hostname() instead.
            req_if = ESP_IF_WIFI_STA;
            val = mod_network_hostname(0, NULL);
            break;
        }
        case MP_QSTR_max_clients: {
            val = MP_OBJ_NEW_SMALL_INT(cfg.ap.max_connection);
            break;
        }
        case MP_QSTR_reconnects:
            req_if = ESP_IF_WIFI_STA;
            int rec = conf_wifi_sta_reconnects - 1;
            val = MP_OBJ_NEW_SMALL_INT(rec);
            break;
        case MP_QSTR_txpower: {
            int8_t power;
            esp_exceptions(esp_wifi_get_max_tx_power(&power));
            val = mp_obj_new_float(power * 0.25);
            break;
        }
        case MP_QSTR_protocol: {
            uint8_t protocol_bitmap;
            esp_exceptions(esp_wifi_get_protocol(self->if_id, &protocol_bitmap));
            val = MP_OBJ_NEW_SMALL_INT(protocol_bitmap);
            break;
        }
        case MP_QSTR_pm: {
            wifi_ps_type_t ps_type;
            esp_exceptions(esp_wifi_get_ps(&ps_type));
            val = MP_OBJ_NEW_SMALL_INT(ps_type);
            break;
        }
        default:
            goto unknown;
    }

    // We post-check interface requirements to save on code size
    if (req_if >= 0) {
        require_if(args[0], req_if);
    }

    return val;

unknown:
    mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
}
MP_DEFINE_CONST_FUN_OBJ_KW(network_wlan_config_obj, 1, network_wlan_config);

static const mp_rom_map_elem_t wlan_if_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&network_wlan_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&network_wlan_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&network_wlan_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&network_wlan_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&network_wlan_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan_in_progress), MP_ROM_PTR(&network_wlan_scan_in_progress_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_wlan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&network_wlan_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&esp_network_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&esp_nic_ipconfig_obj) },

    // Constants
    { MP_ROM_QSTR(MP_QSTR_IF_STA), MP_ROM_INT(WIFI_IF_STA)},
    { MP_ROM_QSTR(MP_QSTR_IF_AP), MP_ROM_INT(WIFI_IF_AP)},

    { MP_ROM_QSTR(MP_QSTR_SEC_OPEN), MP_ROM_INT(WIFI_AUTH_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WEP), MP_ROM_INT(WIFI_AUTH_WEP) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA), MP_ROM_INT(WIFI_AUTH_WPA_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA2), MP_ROM_INT(WIFI_AUTH_WPA2_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA_WPA2), MP_ROM_INT(WIFI_AUTH_WPA_WPA2_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA2_ENT), MP_ROM_INT(WIFI_AUTH_WPA2_ENTERPRISE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA3), MP_ROM_INT(WIFI_AUTH_WPA3_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA2_WPA3), MP_ROM_INT(WIFI_AUTH_WPA2_WPA3_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WAPI), MP_ROM_INT(WIFI_AUTH_WAPI_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_OWE), MP_ROM_INT(WIFI_AUTH_OWE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA3_ENT_192), MP_ROM_INT(WIFI_AUTH_WPA3_ENT_192) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA3_EXT_PSK), MP_ROM_INT(WIFI_AUTH_WPA3_EXT_PSK) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA3_EXT_PSK_MIXED_MODE), MP_ROM_INT(WIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE) },
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
    { MP_ROM_QSTR(MP_QSTR_SEC_DPP), MP_ROM_INT(WIFI_AUTH_DPP) },
    #endif
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0)
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA3_ENT), MP_ROM_INT(WIFI_AUTH_WPA3_ENTERPRISE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA2_WPA3_ENT), MP_ROM_INT(WIFI_AUTH_WPA2_WPA3_ENTERPRISE) },
    #endif
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0)
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA_ENT), MP_ROM_INT(WIFI_AUTH_WPA_ENTERPRISE) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_DEFAULT), MP_ROM_INT(WIFI_PROTOCOL_DEFAULT) },
    #if !CONFIG_IDF_TARGET_ESP32C2
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_LR), MP_ROM_INT(WIFI_PROTOCOL_LR) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_PM_NONE), MP_ROM_INT(WIFI_PS_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PM_PERFORMANCE), MP_ROM_INT(WIFI_PS_MIN_MODEM) },
    { MP_ROM_QSTR(MP_QSTR_PM_POWERSAVE), MP_ROM_INT(WIFI_PS_MAX_MODEM) },
};
static MP_DEFINE_CONST_DICT(wlan_if_locals_dict, wlan_if_locals_dict_table);

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0)
_Static_assert(WIFI_AUTH_MAX == 17, "Synchronize WIFI_AUTH_XXX constants with the ESP-IDF. Look at esp-idf/components/esp_wifi/include/esp_wifi_types_generic.h");
#elif ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0)
_Static_assert(WIFI_AUTH_MAX == 16, "Synchronize WIFI_AUTH_XXX constants with the ESP-IDF. Look at esp-idf/components/esp_wifi/include/esp_wifi_types_generic.h");
#elif ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
_Static_assert(WIFI_AUTH_MAX == 14, "Synchronize WIFI_AUTH_XXX constants with the ESP-IDF. Look at esp-idf/components/esp_wifi/include/esp_wifi_types_generic.h");
#elif ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 2, 0)
_Static_assert(WIFI_AUTH_MAX == 13, "Synchronize WIFI_AUTH_XXX constants with the ESP-IDF. Look at esp-idf/components/esp_wifi/include/esp_wifi_types.h");
#else
#error "Error in macro logic, all supported versions should be covered."
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    esp_network_wlan_type,
    MP_QSTR_WLAN,
    MP_TYPE_FLAG_NONE,
    make_new, network_wlan_make_new,
    locals_dict, &wlan_if_locals_dict
    );

MP_REGISTER_ROOT_POINTER(mp_obj_t * user_scan_callback);

#endif // MICROPY_PY_NETWORK_WLAN
