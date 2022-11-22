#ifndef MICROPY_INCLUDED_ESP32_MODNETWORK2_H
#define MICROPY_INCLUDED_ESP32_MODNETWORK2_H


#include "py/runtime.h"
#include "py/obj.h"

#include "modnetwork.h"

#if ESP_IDF_VERSION_MINOR >= 3

// enum { PHY_LAN8720, PHY_IP101, PHY_RTL8201, PHY_DP83848, PHY_KSZ8041 };

typedef struct _wifi_network_if_obj_t {
    mp_obj_base_t base;
    uint8_t if_mode;
    esp_netif_t *netif;
    // EventGroupHandle_t event_group_handle;
    bool active;                // true, if interface is started
    bool connected;             // sta: true, if interface is connected, ap: always false
    bool scan_done;             // true, if scan results are ready
    bool auto_reconnect;        // sta: true, if interface trys to reconnect after disconnect, ap: always false
    bool try_reconnect;         // helper for auto_reconnect
    bool wps_probing;           // true, if wps probing is active
    bool sniffer_running;       // true, if wifi is running in sniffermode
    uint8_t max_connect_retry;  // max number of auto reconnect attempts, -1 = try forever
    uint8_t num_connect_retry;  // current reconnect attempt
    mp_obj_t ssid;
    mp_obj_t bssid;
    mp_obj_t password;
    mp_obj_t last_scan_results; // list of scan results or None, if results not ready
    uint8_t last_wifi_event;    // for debugging, readable for user
    uint8_t last_ip_event;      // for debugging, readable for user
    uint8_t disconnect_reason;  // from esp-idf
    uint32_t ip_addr;
    uint32_t gw_addr;           // gateway address
    uint32_t subnetmask;
    uint8_t num_sta;            // ap: number of STAs connected
    mp_obj_tuple_t *sniffer_arg;
    mp_obj_t sniffer_handler;
} wifi_network_if_obj_t;

const mp_obj_type_t wifi_network_if_type;

typedef struct _eth_network_if_obj_t {
    mp_obj_base_t base;
    uint8_t if_mode;
    esp_netif_t *netif;
    bool active;                // true, if interface is started
    bool connected;             // true, if interface is connected
    uint8_t pin_mdc;            // number of mdc-pin, default 23
    uint8_t pin_mdio;           // number of mdio-pin, default 18
    uint8_t pin_phy_power;      // number of power-pin, default -1 (not used)
    esp_eth_phy_t *phy;
    esp_eth_handle_t eth_handle;
    uint8_t phy_addr;           // phy address
    uint8_t phy_type;           // type of phy chip: PHY_LAN8720, PHY_IP101, PHY_RTL8201, PHY_DP83848, PHY_KSZ8041
    uint8_t last_eth_event;     // for debugging, readable for user
    uint8_t last_ip_event;      // for debugging, readable for user
    uint32_t ip_addr;
    uint32_t gw_addr;           // gateway address
    uint32_t subnetmask;
} eth_network_if_obj_t;

const mp_obj_type_t eth_network_if_type;


#endif
#endif
