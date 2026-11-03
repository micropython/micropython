// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/espidf/__init__.h"
#include "common-hal/wifi/__init__.h"
#include "shared-bindings/wifi/__init__.h"

#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/wifi/Monitor.h"
#include "shared-bindings/wifi/Radio.h"
#include "common-hal/socketpool/__init__.h"

#include "py/gc.h"
#include "py/mpstate.h"
#include "py/runtime.h"

#include "esp_wifi.h"

#include "components/heap/include/esp_heap_caps.h"

wifi_radio_obj_t common_hal_wifi_radio_obj;

#include "components/log/include/esp_log.h"

#include "supervisor/port.h"
#include "supervisor/workflow.h"

#include "lwip/sockets.h"

#if CIRCUITPY_SETTINGS_TOML
#include "supervisor/shared/settings.h"
#endif

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

#include "esp_ipc.h"

#ifdef CONFIG_ESP_HOSTED_ENABLED
#include "esp_hosted.h"
#include "common-hal/microcontroller/Pin.h"
#endif

#ifdef CONFIG_IDF_TARGET_ESP32
#include "nvs_flash.h"
#endif

#define MAC_ADDRESS_LENGTH 6

static const char *TAG = "CP wifi";

static void schedule_background_on_cp_core(void *arg) {
    #if CIRCUITPY_STATUS_BAR
    supervisor_status_bar_request_update(false);
    #endif

    // CircuitPython's VM is run in a separate FreeRTOS task from wifi callbacks. So, we have to
    // notify the main task every time in case it's waiting for us.
    port_wake_main_task();
}

static void event_handler(void *arg, esp_event_base_t event_base,
    int32_t event_id, void *event_data) {
    // This runs on the PRO CORE! It cannot share CP interrupt enable/disable
    // directly.
    wifi_radio_obj_t *radio = arg;
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_SCAN_DONE:
                ESP_LOGW(TAG, "scan");
                xEventGroupSetBits(radio->event_group_handle, WIFI_SCAN_DONE_BIT);
                break;
            case WIFI_EVENT_AP_START:
                ESP_LOGW(TAG, "ap start");
                break;
            case WIFI_EVENT_AP_STOP:
                ESP_LOGW(TAG, "ap stop");
                break;
            case WIFI_EVENT_AP_STACONNECTED:
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                break;
            case WIFI_EVENT_STA_START:
                ESP_LOGW(TAG, "sta start");
                break;
            case WIFI_EVENT_STA_STOP:
                ESP_LOGW(TAG, "sta stop");
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGW(TAG, "connected");
                break;
            case WIFI_EVENT_STA_DISCONNECTED: {
                ESP_LOGW(TAG, "disconnected");
                wifi_event_sta_disconnected_t *d = (wifi_event_sta_disconnected_t *)event_data;
                uint8_t reason = d->reason;
                const char *reason_str = "unknown";
                switch (reason) {
                    case WIFI_REASON_UNSPECIFIED:
                        reason_str = "unspecified";
                        break;
                    case WIFI_REASON_AUTH_EXPIRE:
                        reason_str = "auth expire";
                        break;
                    case WIFI_REASON_AUTH_LEAVE:
                        reason_str = "auth leave";
                        break;
                    case WIFI_REASON_DISASSOC_DUE_TO_INACTIVITY:
                        reason_str = "disassoc inactivity";
                        break;
                    case WIFI_REASON_ASSOC_TOOMANY:
                        reason_str = "assoc toomany";
                        break;
                    case WIFI_REASON_CLASS2_FRAME_FROM_NONAUTH_STA:
                        reason_str = "class2 from nonauth";
                        break;
                    case WIFI_REASON_CLASS3_FRAME_FROM_NONASSOC_STA:
                        reason_str = "class3 from nonassoc";
                        break;
                    case WIFI_REASON_ASSOC_LEAVE:
                        reason_str = "assoc leave";
                        break;
                    case WIFI_REASON_ASSOC_NOT_AUTHED:
                        reason_str = "assoc not authed";
                        break;
                    case WIFI_REASON_DISASSOC_PWRCAP_BAD:
                        reason_str = "disassoc pwrcap bad";
                        break;
                    case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
                        reason_str = "disassoc supchan bad";
                        break;
                    case WIFI_REASON_BSS_TRANSITION_DISASSOC:
                        reason_str = "bss transition disassoc";
                        break;
                    case WIFI_REASON_IE_INVALID:
                        reason_str = "ie invalid";
                        break;
                    case WIFI_REASON_MIC_FAILURE:
                        reason_str = "mic failure";
                        break;
                    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
                        reason_str = "4way handshake timeout";
                        break;
                    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
                        reason_str = "group key update timeout";
                        break;
                    case WIFI_REASON_IE_IN_4WAY_DIFFERS:
                        reason_str = "ie in 4way differs";
                        break;
                    case WIFI_REASON_GROUP_CIPHER_INVALID:
                        reason_str = "group cipher invalid";
                        break;
                    case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
                        reason_str = "pairwise cipher invalid";
                        break;
                    case WIFI_REASON_AKMP_INVALID:
                        reason_str = "akmp invalid";
                        break;
                    case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
                        reason_str = "unsupp rsn ie version";
                        break;
                    case WIFI_REASON_INVALID_RSN_IE_CAP:
                        reason_str = "invalid rsn ie cap";
                        break;
                    case WIFI_REASON_802_1X_AUTH_FAILED:
                        reason_str = "802.1x auth failed";
                        break;
                    case WIFI_REASON_CIPHER_SUITE_REJECTED:
                        reason_str = "cipher suite rejected";
                        break;
                    case WIFI_REASON_TDLS_PEER_UNREACHABLE:
                        reason_str = "tdls peer unreachable";
                        break;
                    case WIFI_REASON_TDLS_UNSPECIFIED:
                        reason_str = "tdls unspecified";
                        break;
                    case WIFI_REASON_SSP_REQUESTED_DISASSOC:
                        reason_str = "ssp requested disassoc";
                        break;
                    case WIFI_REASON_NO_SSP_ROAMING_AGREEMENT:
                        reason_str = "no ssp roaming agreement";
                        break;
                    case WIFI_REASON_BAD_CIPHER_OR_AKM:
                        reason_str = "bad cipher or akm";
                        break;
                    case WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION:
                        reason_str = "not authorized this location";
                        break;
                    case WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS:
                        reason_str = "service change precludes ts";
                        break;
                    case WIFI_REASON_UNSPECIFIED_QOS:
                        reason_str = "unspecified qos";
                        break;
                    case WIFI_REASON_NOT_ENOUGH_BANDWIDTH:
                        reason_str = "not enough bandwidth";
                        break;
                    case WIFI_REASON_MISSING_ACKS:
                        reason_str = "missing acks";
                        break;
                    case WIFI_REASON_EXCEEDED_TXOP:
                        reason_str = "exceeded txop";
                        break;
                    case WIFI_REASON_STA_LEAVING:
                        reason_str = "sta leaving";
                        break;
                    case WIFI_REASON_END_BA:
                        reason_str = "end ba";
                        break;
                    case WIFI_REASON_UNKNOWN_BA:
                        reason_str = "unknown ba";
                        break;
                    case WIFI_REASON_TIMEOUT:
                        reason_str = "timeout";
                        break;
                    case WIFI_REASON_PEER_INITIATED:
                        reason_str = "peer initiated";
                        break;
                    case WIFI_REASON_AP_INITIATED:
                        reason_str = "ap initiated";
                        break;
                    case WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT:
                        reason_str = "invalid ft action frame count";
                        break;
                    case WIFI_REASON_INVALID_PMKID:
                        reason_str = "invalid pmkid";
                        break;
                    case WIFI_REASON_INVALID_MDE:
                        reason_str = "invalid mde";
                        break;
                    case WIFI_REASON_INVALID_FTE:
                        reason_str = "invalid fte";
                        break;
                    case WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED:
                        reason_str = "transmission link establish failed";
                        break;
                    case WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED:
                        reason_str = "alternative channel occupied";
                        break;
                    case WIFI_REASON_BEACON_TIMEOUT:
                        reason_str = "beacon timeout";
                        break;
                    case WIFI_REASON_NO_AP_FOUND:
                        reason_str = "no ap found";
                        break;
                    case WIFI_REASON_AUTH_FAIL:
                        reason_str = "auth fail";
                        break;
                    case WIFI_REASON_ASSOC_FAIL:
                        reason_str = "assoc fail";
                        break;
                    case WIFI_REASON_HANDSHAKE_TIMEOUT:
                        reason_str = "handshake timeout";
                        break;
                    case WIFI_REASON_CONNECTION_FAIL:
                        reason_str = "connection fail";
                        break;
                    case WIFI_REASON_AP_TSF_RESET:
                        reason_str = "ap tsf reset";
                        break;
                    case WIFI_REASON_ROAMING:
                        reason_str = "roaming";
                        break;
                    case WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG:
                        reason_str = "assoc comeback time too long";
                        break;
                    case WIFI_REASON_SA_QUERY_TIMEOUT:
                        reason_str = "sa query timeout";
                        break;
                    case WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY:
                        reason_str = "no ap found w compatible security";
                        break;
                    case WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD:
                        reason_str = "no ap found in authmode threshold";
                        break;
                    case WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD:
                        reason_str = "no ap found in rssi threshold";
                        break;
                    default:
                        break;
                }
                ESP_LOGW(TAG, "reason %d 0x%02x %s", reason, reason, reason_str);
                if (radio->retries_left > 0 &&
                    reason != WIFI_REASON_AUTH_FAIL &&
                    reason != WIFI_REASON_NO_AP_FOUND &&
                    reason != WIFI_REASON_ASSOC_LEAVE) {
                    radio->retries_left--;
                    ESP_LOGI(TAG, "Retrying connect. %d retries remaining", radio->retries_left);
                    esp_wifi_connect();
                    return;
                }

                radio->last_disconnect_reason = reason;
                xEventGroupSetBits(radio->event_group_handle, WIFI_DISCONNECTED_BIT);
                break;
            }

            case WIFI_EVENT_WIFI_READY:
                ESP_LOGW(TAG, "wifi ready");
                break;
            case WIFI_EVENT_STA_AUTHMODE_CHANGE:
                ESP_LOGW(TAG, "sta authmode change");
                break;
            case WIFI_EVENT_STA_WPS_ER_SUCCESS:
                ESP_LOGW(TAG, "sta wps er success");
                break;
            case WIFI_EVENT_STA_WPS_ER_FAILED:
                ESP_LOGW(TAG, "sta wps er failed");
                break;
            case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
                ESP_LOGW(TAG, "sta wps er timeout");
                break;
            case WIFI_EVENT_STA_WPS_ER_PIN:
                ESP_LOGW(TAG, "sta wps er pin");
                break;
            case WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP:
                ESP_LOGW(TAG, "sta wps er pbc overlap");
                break;
            case WIFI_EVENT_AP_PROBEREQRECVED:
                ESP_LOGW(TAG, "ap probereqrecved");
                break;
            case WIFI_EVENT_FTM_REPORT:
                ESP_LOGW(TAG, "ftm report");
                break;
            case WIFI_EVENT_STA_BSS_RSSI_LOW:
                ESP_LOGW(TAG, "sta bss rssi low");
                break;
            case WIFI_EVENT_ACTION_TX_STATUS:
                ESP_LOGW(TAG, "action tx status");
                break;
            case WIFI_EVENT_ROC_DONE:
                ESP_LOGW(TAG, "roc done");
                break;
            case WIFI_EVENT_STA_BEACON_TIMEOUT:
                ESP_LOGW(TAG, "sta beacon timeout");
                break;
            case WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START:
                ESP_LOGW(TAG, "connectionless module wake interval start");
                break;
            case WIFI_EVENT_AP_WPS_RG_SUCCESS:
                ESP_LOGW(TAG, "ap wps rg success");
                break;
            case WIFI_EVENT_AP_WPS_RG_FAILED:
                ESP_LOGW(TAG, "ap wps rg failed");
                break;
            case WIFI_EVENT_AP_WPS_RG_TIMEOUT:
                ESP_LOGW(TAG, "ap wps rg timeout");
                break;
            case WIFI_EVENT_AP_WPS_RG_PIN:
                ESP_LOGW(TAG, "ap wps rg pin");
                break;
            case WIFI_EVENT_AP_WPS_RG_PBC_OVERLAP:
                ESP_LOGW(TAG, "ap wps rg pbc overlap");
                break;
            case WIFI_EVENT_ITWT_SETUP:
                ESP_LOGW(TAG, "itwt setup");
                break;
            case WIFI_EVENT_ITWT_TEARDOWN:
                ESP_LOGW(TAG, "itwt teardown");
                break;
            case WIFI_EVENT_ITWT_PROBE:
                ESP_LOGW(TAG, "itwt probe");
                break;
            case WIFI_EVENT_ITWT_SUSPEND:
                ESP_LOGW(TAG, "itwt suspend");
                break;
            case WIFI_EVENT_TWT_WAKEUP:
                ESP_LOGW(TAG, "twt wakeup");
                break;
            case WIFI_EVENT_BTWT_SETUP:
                ESP_LOGW(TAG, "btwt setup");
                break;
            case WIFI_EVENT_BTWT_TEARDOWN:
                ESP_LOGW(TAG, "btwt teardown");
                break;
            case WIFI_EVENT_NAN_SYNC_STARTED:
                ESP_LOGW(TAG, "nan sync started");
                break;
            case WIFI_EVENT_NAN_SYNC_STOPPED:
                ESP_LOGW(TAG, "nan sync stopped");
                break;
            case WIFI_EVENT_NAN_SVC_MATCH:
                ESP_LOGW(TAG, "nan svc match");
                break;
            case WIFI_EVENT_NAN_REPLIED:
                ESP_LOGW(TAG, "nan replied");
                break;
            case WIFI_EVENT_NAN_RECEIVE:
                ESP_LOGW(TAG, "nan receive");
                break;
            case WIFI_EVENT_NDP_INDICATION:
                ESP_LOGW(TAG, "ndp indication");
                break;
            case WIFI_EVENT_NDP_CONFIRM:
                ESP_LOGW(TAG, "ndp confirm");
                break;
            case WIFI_EVENT_NDP_TERMINATED:
                ESP_LOGW(TAG, "ndp terminated");
                break;
            case WIFI_EVENT_HOME_CHANNEL_CHANGE:
                ESP_LOGW(TAG, "home channel change");
                break;
            case WIFI_EVENT_STA_NEIGHBOR_REP:
                ESP_LOGW(TAG, "sta neighbor rep");
                break;
            case WIFI_EVENT_AP_WRONG_PASSWORD:
                ESP_LOGW(TAG, "ap wrong password");
                break;
            case WIFI_EVENT_STA_BEACON_OFFSET_UNSTABLE:
                ESP_LOGW(TAG, "sta beacon offset unstable");
                break;
            case WIFI_EVENT_DPP_URI_READY:
                ESP_LOGW(TAG, "dpp uri ready");
                break;
            case WIFI_EVENT_DPP_CFG_RECVD:
                ESP_LOGW(TAG, "dpp cfg recvd");
                break;
            case WIFI_EVENT_DPP_FAILED:
                ESP_LOGW(TAG, "dpp failed");
                break;
            default:
                ESP_LOGW(TAG, "unknown event %ld", event_id);
                break;
        }
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGW(TAG, "got ip");
        radio->retries_left = radio->starting_retries;
        xEventGroupSetBits(radio->event_group_handle, WIFI_CONNECTED_BIT);
    }
    // Use IPC to ensure we run schedule background on the same core as CircuitPython.
    #if defined(CONFIG_FREERTOS_UNICORE) && CONFIG_FREERTOS_UNICORE
    schedule_background_on_cp_core(NULL);
    #else
    // This only blocks until the start of the function. That's ok since the PRO
    // core shouldn't care what we do.
    esp_ipc_call(CONFIG_ESP_MAIN_TASK_AFFINITY, schedule_background_on_cp_core, NULL);
    #endif
}

static bool wifi_inited;
static bool wifi_ever_inited;
static bool wifi_user_initiated;

#ifdef CONFIG_ESP_HOSTED_ENABLED
// Set when the co-processor didn't answer as ESP-Hosted firmware. Its firmware can't change
// without resetting this chip too, so don't wait for it again until then.
static bool hosted_unavailable;

static const uint8_t hosted_pins[] = {
    CONFIG_ESP_HOSTED_SDIO_PIN_CLK, CONFIG_ESP_HOSTED_SDIO_PIN_CMD,
    CONFIG_ESP_HOSTED_SDIO_PIN_D0, CONFIG_ESP_HOSTED_SDIO_PIN_D1,
    CONFIG_ESP_HOSTED_SDIO_PIN_D2, CONFIG_ESP_HOSTED_SDIO_PIN_D3,
    CONFIG_ESP_HOSTED_SDIO_GPIO_RESET_SLAVE,
};

// Start ESP-Hosted and check that the co-processor answers as ESP-Hosted firmware. Other firmware
// (one that doesn't use SDIO, or speaks another protocol over it) never brings the transport up.
// Every Wi-Fi call then fails, and connect() would wait forever for an event, which hangs boot when
// web workflow is configured. So if it doesn't answer, undo the start and release the pins.
static bool start_hosted(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(hosted_pins); i++) {
        claim_pin_number(hosted_pins[i]);
        never_reset_pin_number(hosted_pins[i]);
    }
    esp_hosted_coprocessor_fwver_t version;
    // esp_hosted_connect_to_slave() waits for the transport but always returns ESP_OK. The version
    // request fails at once if the transport isn't up, and otherwise needs a working RPC reply.
    if (esp_hosted_init() == ESP_OK) {
        esp_hosted_connect_to_slave();
        if (esp_hosted_get_coprocessor_fwversion(&version) == ESP_OK) {
            return true;
        }
    }
    ESP_LOGE(TAG, "Co-processor is not answering as ESP-Hosted; Wi-Fi is unavailable");
    esp_hosted_deinit();
    for (size_t i = 0; i < MP_ARRAY_SIZE(hosted_pins); i++) {
        reset_pin_number(hosted_pins[i]);
    }
    hosted_unavailable = true;
    return false;
}

// Raises when called from the VM. Outside it (web workflow at boot) the caller sees the radio off.
static void hosted_failed(void) {
    if (gc_alloc_possible()) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("WiFi is not enabled"));
    }
}
#endif

void common_hal_wifi_init(bool user_initiated) {
    wifi_radio_obj_t *self = &common_hal_wifi_radio_obj;

    if (wifi_inited) {
        if (user_initiated && !wifi_user_initiated) {
            common_hal_wifi_radio_set_enabled(self, true);
        }
        return;
    }
    common_hal_wifi_radio_obj.base.type = &wifi_radio_type;

    if (!wifi_ever_inited) {
        #ifdef CONFIG_ESP_HOSTED_ENABLED
        if (hosted_unavailable) {
            hosted_failed();
            return;
        }
        #endif
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        ESP_ERROR_CHECK(esp_netif_init());
        #ifdef CONFIG_ESP_HOSTED_ENABLED
        if (!start_hosted()) {
            hosted_failed();
            return;
        }
        #endif
        wifi_ever_inited = true;
    }
    wifi_inited = true;
    wifi_user_initiated = user_initiated;

    self->netif = esp_netif_create_default_wifi_sta();
    self->ap_netif = esp_netif_create_default_wifi_ap();
    self->started = false;

    // Even though we just called esp_netif_create_default_wifi_sta,
    //   station mode isn't actually ready for use until esp_wifi_set_mode()
    //   is called and the configuration is loaded via esp_wifi_set_config().
    // Set both convenience flags to false so it's not forgotten.
    self->sta_mode = 0;
    self->ap_mode = 0;

    self->event_group_handle = xEventGroupCreateStatic(&self->event_group);
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        self,
        &self->handler_instance_all_wifi));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        self,
        &self->handler_instance_got_ip));

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t result = esp_wifi_init(&config);
    #ifdef CONFIG_ESP32_WIFI_NVS_ENABLED
    // Generally we don't use this because we store ssid and passwords ourselves in the filesystem.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    #else
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    #endif
    if (result == ESP_ERR_NO_MEM) {
        if (gc_alloc_possible()) {
            mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Failed to allocate Wifi memory"));
        }
        ESP_LOGE(TAG, "Failed to allocate Wifi memory");
    } else if (result != ESP_OK) {
        if (gc_alloc_possible()) {
            raise_esp_error(result);
        }
        ESP_LOGE(TAG, "WiFi error code: %x", result);
        return;
    }

    #if CIRCUITPY_SETTINGS_TOML
    char hostname[CONFIG_LWIP_DHCPS_MAX_HOSTNAME_LEN];
    if (settings_get_str("CIRCUITPY_WIFI_HOSTNAME", hostname, sizeof(hostname)) == SETTINGS_OK) {
        ESP_ERROR_CHECK(esp_netif_set_hostname(self->netif, hostname));
    }
    #else
    if (false) {
    }
    #endif
    else {
        // Set the default lwip_local_hostname capped at 32 characters. We trim off
        // the start of the board name (likely manufacturer) because the end is
        // often more unique to the board.
        size_t board_len = MIN(32 - ((MAC_ADDRESS_LENGTH * 2) + 6), strlen(CIRCUITPY_BOARD_ID));
        size_t board_trim = strlen(CIRCUITPY_BOARD_ID) - board_len;
        // Avoid double _ in the hostname.
        if (CIRCUITPY_BOARD_ID[board_trim] == '_') {
            board_trim++;
        }

        char cpy_default_hostname[board_len + (MAC_ADDRESS_LENGTH * 2) + 6];
        uint8_t mac[MAC_ADDRESS_LENGTH];
        esp_wifi_get_mac(WIFI_IF_STA, mac);
        snprintf(cpy_default_hostname, sizeof(cpy_default_hostname), "cpy-%s-%02x%02x%02x%02x%02x%02x", CIRCUITPY_BOARD_ID + board_trim, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        const char *default_lwip_local_hostname = cpy_default_hostname;
        ESP_ERROR_CHECK(esp_netif_set_hostname(self->netif, default_lwip_local_hostname));
    }

    // set station mode to avoid the default SoftAP
    common_hal_wifi_radio_start_station(self);
    // start wifi
    common_hal_wifi_radio_set_enabled(self, true);
}

void wifi_user_reset(void) {
    if (wifi_user_initiated) {
        wifi_reset();
        wifi_user_initiated = false;
    }
}

void wifi_reset(void) {
    if (!wifi_inited) {
        return;
    }
    if (MP_STATE_VM(wifi_monitor_singleton) != NULL) {
        common_hal_wifi_monitor_deinit(MP_STATE_VM(wifi_monitor_singleton));
    }
    wifi_radio_obj_t *radio = &common_hal_wifi_radio_obj;
    common_hal_wifi_radio_set_enabled(radio, false);
    #ifndef CONFIG_IDF_TARGET_ESP32
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        radio->handler_instance_all_wifi));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        radio->handler_instance_got_ip));
    ESP_ERROR_CHECK(esp_wifi_deinit());
    esp_netif_destroy(radio->netif);
    radio->netif = NULL;
    esp_netif_destroy(radio->ap_netif);
    radio->ap_netif = NULL;
    wifi_inited = false;
    #endif
    supervisor_workflow_request_background();
}

void ipaddress_ipaddress_to_esp_idf(mp_obj_t ip_address, ip_addr_t *esp_ip_address) {
    if (mp_obj_is_type(ip_address, &ipaddress_ipv4address_type)) {
        ipaddress_ipaddress_to_esp_idf_ip4(ip_address, (esp_ip4_addr_t *)esp_ip_address);
        #if LWIP_IPV6
        esp_ip_address->type = IPADDR_TYPE_V4;
        #endif
    } else {
        struct sockaddr_storage addr_storage;
        socketpool_resolve_host_or_throw(AF_UNSPEC, SOCK_STREAM, mp_obj_str_get_str(ip_address), &addr_storage, 1);
        sockaddr_to_espaddr(&addr_storage, (esp_ip_addr_t *)esp_ip_address);
    }
}

void ipaddress_ipaddress_to_esp_idf_ip4(mp_obj_t ip_address, esp_ip4_addr_t *esp_ip_address) {
    if (!mp_obj_is_type(ip_address, &ipaddress_ipv4address_type)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Only IPv4 addresses supported"));
    }
    mp_obj_t packed = common_hal_ipaddress_ipv4address_get_packed(ip_address);
    size_t len;
    const char *bytes = mp_obj_str_get_data(packed, &len);
    esp_netif_set_ip4_addr(esp_ip_address, bytes[0], bytes[1], bytes[2], bytes[3]);
}

void common_hal_wifi_gc_collect(void) {
    common_hal_wifi_radio_gc_collect(&common_hal_wifi_radio_obj);
}

static mp_obj_t espaddrx_to_str(const void *espaddr, uint8_t esptype) {
    char buf[IPADDR_STRLEN_MAX];
    inet_ntop(esptype == ESP_IPADDR_TYPE_V6 ? AF_INET6 : AF_INET, espaddr, buf, sizeof(buf));
    return mp_obj_new_str(buf, strlen(buf));
}

mp_obj_t espaddr_to_str(const esp_ip_addr_t *espaddr) {
    return espaddrx_to_str(espaddr, espaddr->type);
}

mp_obj_t espaddr4_to_str(const esp_ip4_addr_t *espaddr) {
    return espaddrx_to_str(espaddr, ESP_IPADDR_TYPE_V4);
}

mp_obj_t espaddr6_to_str(const esp_ip6_addr_t *espaddr) {
    return espaddrx_to_str(espaddr, ESP_IPADDR_TYPE_V6);
}

void sockaddr_to_espaddr(const struct sockaddr_storage *sockaddr, esp_ip_addr_t *espaddr) {
    #if CIRCUITPY_SOCKETPOOL_IPV6
    MP_STATIC_ASSERT(IPADDR_TYPE_V4 == ESP_IPADDR_TYPE_V4);
    MP_STATIC_ASSERT(IPADDR_TYPE_V6 == ESP_IPADDR_TYPE_V6);
    MP_STATIC_ASSERT(sizeof(ip_addr_t) == sizeof(esp_ip_addr_t));
    MP_STATIC_ASSERT(offsetof(ip_addr_t, u_addr) == offsetof(esp_ip_addr_t, u_addr));
    MP_STATIC_ASSERT(offsetof(ip_addr_t, type) == offsetof(esp_ip_addr_t, type));
    if (sockaddr->ss_family == AF_INET6) {
        const struct sockaddr_in6 *addr6 = (const void *)sockaddr;
        MP_STATIC_ASSERT(sizeof(espaddr->u_addr.ip6.addr) == sizeof(addr6->sin6_addr));
        memcpy(&espaddr->u_addr.ip6.addr, &addr6->sin6_addr, sizeof(espaddr->u_addr.ip6.addr));
        espaddr->u_addr.ip6.zone = addr6->sin6_scope_id;
        espaddr->type = ESP_IPADDR_TYPE_V6;
    } else
    #endif
    {
        const struct sockaddr_in *addr = (const void *)sockaddr;
        MP_STATIC_ASSERT(sizeof(espaddr->u_addr.ip4.addr) == sizeof(addr->sin_addr));
        memcpy(&espaddr->u_addr.ip4.addr, &addr->sin_addr, sizeof(espaddr->u_addr.ip4.addr));
        espaddr->type = ESP_IPADDR_TYPE_V4;
    }
}

void espaddr_to_sockaddr(const esp_ip_addr_t *espaddr, struct sockaddr_storage *sockaddr, int port) {
    #if CIRCUITPY_SOCKETPOOL_IPV6
    if (espaddr->type == ESP_IPADDR_TYPE_V6) {
        struct sockaddr_in6 *addr6 = (void *)sockaddr;
        memcpy(&addr6->sin6_addr, &espaddr->u_addr.ip6.addr, sizeof(espaddr->u_addr.ip6.addr));
        addr6->sin6_scope_id = espaddr->u_addr.ip6.zone;
    } else
    #endif
    {
        struct sockaddr_in *addr = (void *)sockaddr;
        memcpy(&addr->sin_addr, &espaddr->u_addr.ip4.addr, sizeof(espaddr->u_addr.ip4.addr));
    }
}
