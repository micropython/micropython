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

#include <stdint.h>
#include <stdbool.h>
#include "std.h"

#include "simplelink.h"
#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "netutils.h"
#include "modnetwork.h"
#include "modusocket.h"
#include "modwlan.h"
#include "pybioctl.h"
#include "debug.h"
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
#include "serverstask.h"
#endif
#include "mpexception.h"
#include "mpcallback.h"
#include "pybsleep.h"
#include "antenna.h"


/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
// Status bits - These are used to set/reset the corresponding bits in a given variable
typedef enum{
    STATUS_BIT_NWP_INIT = 0,        // If this bit is set: Network Processor is
                                    // powered up

    STATUS_BIT_CONNECTION,          // If this bit is set: the device is connected to
                                    // the AP or client is connected to device (AP)

    STATUS_BIT_IP_LEASED,           // If this bit is set: the device has leased IP to
                                    // any connected client

    STATUS_BIT_IP_ACQUIRED,          // If this bit is set: the device has acquired an IP

    STATUS_BIT_SMARTCONFIG_START,   // If this bit is set: the SmartConfiguration
                                    // process is started from SmartConfig app

    STATUS_BIT_P2P_DEV_FOUND,       // If this bit is set: the device (P2P mode)
                                    // found any p2p-device in scan

    STATUS_BIT_P2P_REQ_RECEIVED,    // If this bit is set: the device (P2P mode)
                                    // found any p2p-negotiation request

    STATUS_BIT_CONNECTION_FAILED,   // If this bit is set: the device(P2P mode)
                                    // connection to client(or reverse way) is failed

    STATUS_BIT_PING_DONE            // If this bit is set: the device has completed
                                    // the ping operation
}e_StatusBits;

typedef struct _wlan_obj_t {
    mp_obj_base_t       base;
    SlWlanMode_t        mode;
    uint32_t            status;

    uint32_t            ip;
    uint32_t            gateway;
    uint32_t            dns;

#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    bool                servers_enabled;
#endif
    uint8_t             security;

    // my own name and mac
    uint8_t             ssid[33];
    uint8_t             mac[SL_MAC_ADDR_LEN];

    // the name and mac of the other device
    uint8_t             ssid_o[33];
    uint8_t             bssid[6];

    bool                staconnected;

} wlan_obj_t;

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define CLR_STATUS_BIT_ALL(status)      (status = 0)
#define SET_STATUS_BIT(status, bit)     (status |= ( 1 << (bit)))
#define CLR_STATUS_BIT(status, bit)     (status &= ~(1 << (bit)))
#define GET_STATUS_BIT(status, bit)     (0 != (status & (1 << (bit))))

#define IS_NW_PROCSR_ON(status)         GET_STATUS_BIT(status, STATUS_BIT_NWP_INIT)
#define IS_CONNECTED(status)            GET_STATUS_BIT(status, STATUS_BIT_CONNECTION)
#define IS_IP_LEASED(status)            GET_STATUS_BIT(status, STATUS_BIT_IP_LEASED)
#define IS_IP_ACQUIRED(status)          GET_STATUS_BIT(status, STATUS_BIT_IP_ACQUIRED)
#define IS_SMART_CFG_START(status)      GET_STATUS_BIT(status, STATUS_BIT_SMARTCONFIG_START)
#define IS_P2P_DEV_FOUND(status)        GET_STATUS_BIT(status, STATUS_BIT_P2P_DEV_FOUND)
#define IS_P2P_REQ_RCVD(status)         GET_STATUS_BIT(status, STATUS_BIT_P2P_REQ_RECEIVED)
#define IS_CONNECT_FAILED(status)       GET_STATUS_BIT(status, STATUS_BIT_CONNECTION_FAILED)
#define IS_PING_DONE(status)            GET_STATUS_BIT(status, STATUS_BIT_PING_DONE)

#define MODWLAN_SL_SCAN_ENABLE          1
#define MODWLAN_SL_SCAN_DISABLE         0
#define MODWLAN_SL_MAX_NETWORKS         20

#define MODWLAN_TIMEOUT_MS              5000
#define MODWLAN_MAX_NETWORKS            20
#define MODWLAN_SCAN_PERIOD_S           3600     // 1 hour
#define MODWLAN_WAIT_FOR_SCAN_MS        1050

#define ASSERT_ON_ERROR( x )            ASSERT((x) >= 0 )

#define IPV4_ADDR_STR_LEN_MAX           (16)

#define WLAN_MAX_RX_SIZE                16000
#define WLAN_MAX_TX_SIZE                1476

#define MODWLAN_IP_MODE_DYNAMIC         0
#define MODWLAN_IP_MODE_STATIC          1

#define MAKE_SOCKADDR(addr, ip, port)       sockaddr addr; \
                                            addr.sa_family = AF_INET; \
                                            addr.sa_data[0] = port >> 8; \
                                            addr.sa_data[1] = port; \
                                            addr.sa_data[2] = ip[3]; \
                                            addr.sa_data[3] = ip[2]; \
                                            addr.sa_data[4] = ip[1]; \
                                            addr.sa_data[5] = ip[0];

#define UNPACK_SOCKADDR(addr, ip, port)     port = (addr.sa_data[0] << 8) | addr.sa_data[1]; \
                                            ip[0] = addr.sa_data[5]; \
                                            ip[1] = addr.sa_data[4]; \
                                            ip[2] = addr.sa_data[3]; \
                                            ip[3] = addr.sa_data[2];

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC wlan_obj_t wlan_obj = {
        .mode = -1,
        .status = 0,
        .ip = 0,
        .gateway = 0,
        .dns = 0,
    #if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
        .servers_enabled = false,
    #endif
        .security = SL_SEC_TYPE_OPEN,
        .ssid = {0},
        .bssid = {0},
        .mac = {0},
        .ssid_o = {0},
        .staconnected = false
};

STATIC const mp_cb_methods_t wlan_cb_methods;

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
OsiLockObj_t wlan_LockObj;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void wlan_initialize_data (void);
STATIC void wlan_reenable (SlWlanMode_t mode);
STATIC void wlan_servers_start (void);
STATIC void wlan_servers_stop (void);
STATIC void wlan_get_sl_mac (void);
STATIC modwlan_Status_t wlan_do_connect (const char* ssid, uint32_t ssid_len, const char* bssid, uint8_t sec,
                                         const char* key, uint32_t key_len, uint32_t timeout);
STATIC void wlan_lpds_callback_enable (mp_obj_t self_in);
STATIC void wlan_lpds_callback_disable (mp_obj_t self_in);
STATIC bool wlan_scan_result_is_unique (const mp_obj_list_t *nets, _u8 *bssid);

//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent) {
    if (!pWlanEvent) {
        return;
    }

    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t *pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            // copy the new connection data
            memcpy(wlan_obj.bssid, pEventData->bssid, SL_BSSID_LENGTH);
            memcpy(wlan_obj.ssid_o, pEventData->ssid_name, pEventData->ssid_len);
            wlan_obj.ssid_o[pEventData->ssid_len] = '\0';
            SET_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION);
        #if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
            // we must reset the servers in case that the last connection
            // was lost without any notification being received
            servers_reset();
        #endif
        }
            break;
        case SL_WLAN_DISCONNECT_EVENT:
            CLR_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(wlan_obj.status, STATUS_BIT_IP_ACQUIRED);
        #if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
            servers_reset();
        #endif
            break;
        case SL_WLAN_STA_CONNECTED_EVENT:
        {
            slPeerInfoAsyncResponse_t *pEventData = &pWlanEvent->EventData.APModeStaConnected;
            // get the mac address and name of the connected device
            memcpy(wlan_obj.bssid, pEventData->mac, SL_BSSID_LENGTH);
            memcpy(wlan_obj.ssid_o, pEventData->go_peer_device_name, pEventData->go_peer_device_name_len);
            wlan_obj.ssid_o[pEventData->go_peer_device_name_len] = '\0';
            wlan_obj.staconnected = true;
        #if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
            // we must reset the servers in case that the last connection
            // was lost without any notification being received
            servers_reset();
        #endif
        }
            break;
        case SL_WLAN_STA_DISCONNECTED_EVENT:
            wlan_obj.staconnected = false;
        #if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
            servers_reset();
        #endif
            break;
        case SL_WLAN_P2P_DEV_FOUND_EVENT:
            // TODO
            break;
        case SL_WLAN_P2P_NEG_REQ_RECEIVED_EVENT:
            // TODO
            break;
        case SL_WLAN_CONNECTION_FAILED_EVENT:
            // TODO
            break;
        default:
            break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent) {
    if(!pNetAppEvent) {
        return;
    }

    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(wlan_obj.status, STATUS_BIT_IP_ACQUIRED);

            // Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            // Get ip, gateway and dns
            wlan_obj.gateway = pEventData->gateway;
            wlan_obj.ip      = pEventData->ip;
            wlan_obj.dns     = pEventData->dns;
        }
            break;
        case SL_NETAPP_IPV6_IPACQUIRED_EVENT:
            break;
        case SL_NETAPP_IP_LEASED_EVENT:
            break;
        case SL_NETAPP_IP_RELEASED_EVENT:
            break;
        default:
            break;
    }
}

//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent, SlHttpServerResponse_t *pHttpResponse) {
    if (!pHttpEvent) {
        return;
    }

    switch (pHttpEvent->Event) {
    case SL_NETAPP_HTTPGETTOKENVALUE_EVENT:
        break;
    case SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT:
        break;
    default:
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent) {
    if (!pDevEvent) {
        return;
    }
}

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock) {
    if (!pSock) {
        return;
    }

    switch( pSock->Event ) {
    case SL_SOCKET_TX_FAILED_EVENT:
        switch( pSock->socketAsyncEvent.SockTxFailData.status) {
        case SL_ECLOSE:
            break;
        default:
          break;
        }
        break;
    case SL_SOCKET_ASYNC_EVENT:
         switch(pSock->socketAsyncEvent.SockAsyncData.type) {
         case SSL_ACCEPT:
             break;
         case RX_FRAGMENTATION_TOO_BIG:
             break;
         case OTHER_SIDE_CLOSE_SSL_DATA_NOT_ENCRYPTED:
             break;
         default:
             break;
         }
        break;
    default:
      break;
    }
}

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************

__attribute__ ((section (".boot")))
void wlan_pre_init (void) {
    // create the wlan lock
    ASSERT(OSI_OK == sl_LockObjCreate(&wlan_LockObj, "WlanLock"));
}

void wlan_first_start (void) {
    if (wlan_obj.mode < 0) {
        wlan_initialize_data();
        wlan_obj.mode = sl_Start(0, 0, 0);
        sl_LockObjUnlock (&wlan_LockObj);
    }

    // get the mac address
    wlan_get_sl_mac();
}

modwlan_Status_t wlan_sl_enable (SlWlanMode_t mode, const char *ssid, uint8_t ssid_len, uint8_t sec,
                                 const char *key, uint8_t key_len, uint8_t channel) {

    if (mode == ROLE_STA || mode == ROLE_AP || mode == ROLE_P2P) {
        // stop the servers
        wlan_servers_stop();

        // do a basic start
        wlan_first_start();

        // Device in station-mode. Disconnect previous connection if any
        // The function returns 0 if 'Disconnected done', negative number if already
        // disconnected Wait for 'disconnection' event if 0 is returned, Ignore
        // other return-codes
        if (0 == sl_WlanDisconnect()) {
            while (IS_CONNECTED (wlan_obj.status)) {
                HAL_Delay (5);
                wlan_update();
            }
        }

        // Remove all profiles
        ASSERT_ON_ERROR(sl_WlanProfileDel(0xFF));

        // Enable the DHCP client
        uint8_t value = 1;
        ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &value));

        // Set PM policy to normal
        ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0));

        // Unregister mDNS services
        ASSERT_ON_ERROR(sl_NetAppMDNSUnRegisterService(0, 0));

        // Stop the internal HTTP server
        sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);

        // Remove all 64 filters (8 * 8)
        _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask;
        memset ((void *)&RxFilterIdMask, 0 ,sizeof(RxFilterIdMask));
        memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
        ASSERT_ON_ERROR(sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask, sizeof(_WlanRxFilterOperationCommandBuff_t)));

        // Set Tx power level for station or AP mode
        // Number between 0-15, as dB offset from max power - 0 will set max power
        uint8_t ucPower = 0;
        if (mode == ROLE_AP) {
            // Switch to AP mode
            ASSERT_ON_ERROR(sl_WlanSetMode(mode));
            ASSERT (ssid != NULL && key != NULL);
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_AP_TX_POWER, sizeof(ucPower),
                                       (unsigned char *)&ucPower));
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, ssid_len, (unsigned char *)ssid));
            memcpy(wlan_obj.ssid, (unsigned char *)ssid, ssid_len);
            wlan_obj.ssid[ssid_len] = '\0';
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SECURITY_TYPE, sizeof(uint8_t), &sec));
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_PASSWORD, key_len, (unsigned char *)key));
            _u8*  country = (_u8*)"EU";
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_COUNTRY_CODE, 2, country));
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_CHANNEL, 1, (_u8 *)&channel));

            // stop and start again
            wlan_reenable(mode);
            ASSERT (wlan_obj.mode == mode);

            SlNetCfgIpV4Args_t ipV4;
            ipV4.ipV4          = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 IP address
            ipV4.ipV4Mask      = (_u32)SL_IPV4_VAL(255,255,255,0);          // _u32 Subnet mask for this AP
            ipV4.ipV4Gateway   = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 Default gateway address
            ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 DNS server address
            ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4,
                                         sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4));

            // stop and start again
            wlan_reenable(mode);

            SlNetAppDhcpServerBasicOpt_t dhcpParams;
            dhcpParams.lease_time      =  4096;                             // lease time (in seconds) of the IP Address
            dhcpParams.ipv4_addr_start =  SL_IPV4_VAL(192,168,1,2);         // first IP Address for allocation.
            dhcpParams.ipv4_addr_last  =  SL_IPV4_VAL(192,168,1,254);       // last IP Address for allocation.
            ASSERT_ON_ERROR(sl_NetAppStop(SL_NET_APP_DHCP_SERVER_ID));      // Stop DHCP server before settings
            ASSERT_ON_ERROR(sl_NetAppSet(SL_NET_APP_DHCP_SERVER_ID, NETAPP_SET_DHCP_SRV_BASIC_OPT,
                                         sizeof(SlNetAppDhcpServerBasicOpt_t), (_u8* )&dhcpParams));  // set parameters
            ASSERT_ON_ERROR(sl_NetAppStart(SL_NET_APP_DHCP_SERVER_ID));     // Start DHCP server with new settings

            // stop and start again
            wlan_reenable(mode);

            // save the security type
            wlan_obj.security = sec;
        }
        // STA and P2P modes
        else {
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER,
                                       sizeof(ucPower), (unsigned char *)&ucPower));
            ASSERT_ON_ERROR(sl_WlanSetMode(mode));
            // stop and start again
            wlan_reenable(mode);
            // set connection policy to Auto + Fast (tries to connect to the last connected AP)
            ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_CONNECTION,SL_CONNECTION_POLICY(1, 1, 0, 0, 0), NULL, 0));
        }
        // start the servers before returning
        wlan_servers_start();
        return MODWLAN_OK;
    }
    return MODWLAN_ERROR_INVALID_PARAMS;
}

void wlan_update(void) {
#ifndef SL_PLATFORM_MULTI_THREADED
    _SlTaskEntry();
#endif
}

void wlan_stop (uint32_t timeout) {
    wlan_servers_stop();
    sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
    sl_Stop(timeout);
    wlan_obj.mode = -1;
}

void wlan_start (void) {
    wlan_obj.mode = sl_Start(0, 0, 0);
    sl_LockObjUnlock (&wlan_LockObj);
    wlan_servers_start();
}

void wlan_get_mac (uint8_t *macAddress) {
    if (macAddress) {
        memcpy (macAddress, wlan_obj.mac, SL_MAC_ADDR_LEN);
    }
}

void wlan_get_ip (uint32_t *ip) {
    if (ip) {
        *ip = IS_IP_ACQUIRED(wlan_obj.status) ? wlan_obj.ip : 0;
    }
}

bool wlan_is_connected (void) {
    return ((GET_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION) &&
             GET_STATUS_BIT(wlan_obj.status, STATUS_BIT_IP_ACQUIRED)) || wlan_obj.staconnected);
}

//*****************************************************************************
// DEFINE STATIC FUNCTIONS
//*****************************************************************************

STATIC void wlan_initialize_data (void) {
    CLR_STATUS_BIT_ALL(wlan_obj.status);
    wlan_obj.dns = 0;
    wlan_obj.gateway = 0;
    wlan_obj.ip = 0;
    wlan_obj.security = SL_SEC_TYPE_OPEN;
    wlan_obj.staconnected = false;
    memset(wlan_obj.ssid, 0, sizeof(wlan_obj.ssid));
    memset(wlan_obj.ssid_o, 0, sizeof(wlan_obj.ssid));
    memset(wlan_obj.bssid, 0, sizeof(wlan_obj.bssid));
}

STATIC void wlan_reenable (SlWlanMode_t mode) {
    // stop and start again
    sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
    sl_Stop(SL_STOP_TIMEOUT);
    CLR_STATUS_BIT_ALL(wlan_obj.status);
    wlan_obj.staconnected = false;
    wlan_obj.mode = sl_Start(0, 0, 0);
    sl_LockObjUnlock (&wlan_LockObj);
    ASSERT (wlan_obj.mode == mode);
}

STATIC void wlan_servers_start (void) {
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    // start the servers if they were enabled before
    if (wlan_obj.servers_enabled) {
        servers_start();
    }
#endif
}

STATIC void wlan_servers_stop (void) {
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    // Stop all other processes using the wlan engine
    if ((wlan_obj.servers_enabled = servers_are_enabled())) {
        servers_stop();
    }
#endif
}

STATIC modwlan_Status_t wlan_do_connect (const char* ssid, uint32_t ssid_len, const char* bssid, uint8_t sec,
                                         const char* key, uint32_t key_len, uint32_t timeout) {
    SlSecParams_t secParams;
    secParams.Key = (_i8*)key;
    secParams.KeyLen = ((key != NULL) ? key_len : 0);
    secParams.Type = sec;

    if (0 == sl_WlanConnect((_i8*)ssid, ssid_len, (_u8*)bssid, &secParams, NULL)) {
        // wait for the WLAN Event
        uint32_t waitForConnectionMs = 0;
        while (!IS_CONNECTED(wlan_obj.status)) {
            HAL_Delay (5);
            waitForConnectionMs += 5;
            if (waitForConnectionMs > timeout) {
                return MODWLAN_ERROR_TIMEOUT;
            }
            wlan_update();
        }
        return MODWLAN_OK;
    }
    return MODWLAN_ERROR_INVALID_PARAMS;
}

STATIC void wlan_get_sl_mac (void) {
    // Get the MAC address
    uint8_t macAddrLen = SL_MAC_ADDR_LEN;
    sl_NetCfgGet(SL_MAC_ADDRESS_GET, NULL, &macAddrLen, wlan_obj.mac);
}

/// \method init(mode, ssid=None, *, security=wlan.OPEN, key=None, channel=5)
///
/// Initialise the WLAN engine with the given parameters:
///
///   - `mode` can be ROLE_AP, ROLE_STA and ROLE_P2P.
///   - `ssid` is the network ssid in case of AP mode
///   - `security` is the security type for AP mode
///   - `key` is the key when in AP mode
///   - `channel` is the channel to use for the AP network
STATIC const mp_arg_t wlan_init_args[] = {
    { MP_QSTR_mode,         MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = ROLE_STA} },
    { MP_QSTR_ssid,                           MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_security,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = SL_SEC_TYPE_OPEN} },
    { MP_QSTR_key,          MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_channel,      MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 5} },
};

STATIC mp_obj_t wlan_init_helper(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(wlan_init_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(wlan_init_args), wlan_init_args, args);

    // get the ssid
    mp_uint_t ssid_len;
    const char *ssid = mp_obj_str_get_data(args[1].u_obj, &ssid_len);

    // get the key
    mp_uint_t key_len;
    const char *key = mp_obj_str_get_data(args[3].u_obj, &key_len);

    if (key_len < 8) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    // force the channel to be between 1-11
    uint8_t channel = args[4].u_int;
    channel = (channel > 0 && channel != 12) ? channel % 12 : 1;

    if (MODWLAN_OK != wlan_sl_enable (args[0].u_int, ssid, ssid_len, args[2].u_int, key, key_len, channel)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }

    return mp_const_none;
}

STATIC void wlan_lpds_callback_enable (mp_obj_t self_in) {
    mp_obj_t _callback = mpcallback_find(self_in);
    pybsleep_set_wlan_lpds_callback (_callback);
}

STATIC void wlan_lpds_callback_disable (mp_obj_t self_in) {
    pybsleep_set_wlan_lpds_callback (NULL);
}

STATIC bool wlan_scan_result_is_unique (const mp_obj_list_t *nets, _u8 *bssid) {
    for (int i = 0; i < nets->len; i++) {
        // index 1 in the list is the bssid
        mp_obj_str_t *_bssid = (mp_obj_str_t *)((mp_obj_tuple_t *)nets->items[i])->items[1];
        if (!memcmp (_bssid->data, bssid, SL_BSSID_LENGTH)) {
            return false;
        }
    }
    return true;
}

/******************************************************************************/
// Micro Python bindings; WLAN class

/// \class WLAN - driver for the WLAN functionality of the SoC

/// \classmethod \constructor()
/// Create a wlan obecjt and initialise the simplelink engine
//
STATIC mp_obj_t wlan_make_new (mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, MP_ARRAY_SIZE(wlan_init_args), true);

    if (n_args > 0) {
        // get the mode
        SlWlanMode_t mode = mp_obj_get_int(args[0]);
        if (mode == ROLE_AP) {
            // start the peripheral
            mp_map_t kw_args;
            mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
            wlan_init_helper(n_args, args, &kw_args);
        }
        // TODO only STA mode supported for the moment. What if P2P?
        else if (n_args == 1) {
            if (MODWLAN_OK != wlan_sl_enable (mode, NULL, 0, 0, NULL, 0, 0)) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
            }
        }
        else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, mpexception_num_type_invalid_arguments));
        }
    }

    wlan_obj.base.type = (mp_obj_type_t*)&mod_network_nic_type_wlan;
    return &wlan_obj;
}

/// \method connect(ssid, *, security=OPEN, key=None, bssid=None, timeout=5000)
//          if security is WPA/WPA2, the key must be a string
///         if security is WEP, the key must be binary
STATIC mp_obj_t wlan_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid,     MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_security, MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = SL_SEC_TYPE_OPEN} },
        { MP_QSTR_key,      MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid,    MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_timeout,  MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = MODWLAN_TIMEOUT_MS} },
    };

    // check for correct wlan mode
    if (wlan_obj.mode != ROLE_STA && wlan_obj.mode != ROLE_P2P) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the ssid
    mp_uint_t ssid_len;
    const char *ssid = mp_obj_str_get_data(args[0].u_obj, &ssid_len);

    // get the security type
    mp_uint_t sec = args[1].u_int;

    // get key and its len
    mp_uint_t key_len = 0;
    const char *key = NULL;
    mp_buffer_info_t wepkey;
    mp_obj_t key_o = args[2].u_obj;
    if (key_o != mp_const_none) {
        // wep key must be given as raw bytes
        if (sec == SL_SEC_TYPE_WEP) {
            mp_get_buffer_raise(key_o, &wepkey, MP_BUFFER_READ);
            key = wepkey.buf;
            key_len = wepkey.len;
        } else {
            key = mp_obj_str_get_data(key_o, &key_len);
        }
    }

    // get bssid
    const char *bssid = NULL;
    if (args[3].u_obj != mp_const_none) {
        bssid = mp_obj_str_get_str(args[3].u_obj);
    }

    // get the timeout
    uint32_t timeout = MAX(args[4].u_int, 0);

    if (GET_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION)) {
        if (0 == sl_WlanDisconnect()) {
            while (IS_CONNECTED(wlan_obj.status)) {
                HAL_Delay (5);
                wlan_update();
            }
        }
    }

    // connect to the requested access point
    modwlan_Status_t status;
    status = wlan_do_connect (ssid, ssid_len, bssid, sec, key, key_len, timeout);
    if (status == MODWLAN_ERROR_TIMEOUT) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }
    else if (status == MODWLAN_ERROR_INVALID_PARAMS) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
    wlan_obj.security = sec;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wlan_connect_obj, 1, wlan_connect);

/// \method wlan_disconnect()
/// Close the current WLAN connection
STATIC mp_obj_t wlan_disconnect(mp_obj_t self_in) {
    sl_WlanDisconnect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_disconnect_obj, wlan_disconnect);

/// \method is_connected()
/// Return true if connected to the AP and an IP address has been assigned. Also true if there's any station connected.
///        false otherwise.
STATIC mp_obj_t wlan_isconnected(mp_obj_t self_in) {
    if (wlan_is_connected()) {
        return mp_const_true;
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_isconnected_obj, wlan_isconnected);

STATIC mp_obj_t wlan_ifconfig (mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        // get
        unsigned char len = sizeof(SlNetCfgIpV4Args_t);
        unsigned char dhcpIsOn;
        SlNetCfgIpV4Args_t ipV4;
        sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &len, (uint8_t *)&ipV4);

        mp_obj_t ifconfig[4] = {
                netutils_format_ipv4_addr((uint8_t *)&ipV4.ipV4, NETUTILS_LITTLE),
                netutils_format_ipv4_addr((uint8_t *)&ipV4.ipV4Mask, NETUTILS_LITTLE),
                netutils_format_ipv4_addr((uint8_t *)&ipV4.ipV4Gateway, NETUTILS_LITTLE),
                netutils_format_ipv4_addr((uint8_t *)&ipV4.ipV4DnsServer, NETUTILS_LITTLE)
        };
        return mp_obj_new_tuple(4, ifconfig);
    }
    else {
        if (mp_obj_get_type(args[1]) == &mp_type_tuple) {
            // set a static ip
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(args[1], 4, &items);

            SlNetCfgIpV4Args_t ipV4;
            netutils_parse_ipv4_addr(items[0], (uint8_t *)&ipV4.ipV4, NETUTILS_LITTLE);
            netutils_parse_ipv4_addr(items[1], (uint8_t *)&ipV4.ipV4Mask, NETUTILS_LITTLE);
            netutils_parse_ipv4_addr(items[2], (uint8_t *)&ipV4.ipV4Gateway, NETUTILS_LITTLE);
            netutils_parse_ipv4_addr(items[3], (uint8_t *)&ipV4.ipV4DnsServer, NETUTILS_LITTLE);

            // stop the servers
            wlan_servers_stop();
            if (wlan_obj.mode == ROLE_AP) {
                ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4));
                // stop and start again
                wlan_reenable(wlan_obj.mode);

                SlNetAppDhcpServerBasicOpt_t dhcpParams;
                dhcpParams.lease_time      =  4096;                             // lease time (in seconds) of the IP Address
                dhcpParams.ipv4_addr_start =  ipV4.ipV4 + 1;                    // first IP Address for allocation.
                dhcpParams.ipv4_addr_last  =  (ipV4.ipV4 & 0xFFFFFF00) + 254;   // last IP Address for allocation.
                ASSERT_ON_ERROR(sl_NetAppStop(SL_NET_APP_DHCP_SERVER_ID));      // stop DHCP server before settings
                ASSERT_ON_ERROR(sl_NetAppSet(SL_NET_APP_DHCP_SERVER_ID, NETAPP_SET_DHCP_SRV_BASIC_OPT,
                        sizeof(SlNetAppDhcpServerBasicOpt_t), (_u8* )&dhcpParams));  // set parameters
                ASSERT_ON_ERROR(sl_NetAppStart(SL_NET_APP_DHCP_SERVER_ID));     // start DHCP server with new settings
            }
            else {
                ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4));
            }
            // re-enable WLAN and start the servers again
            wlan_reenable (wlan_obj.mode);
            wlan_servers_start();
        }
        else {
            // check for the correct string
            const char *mode = mp_obj_str_get_str(args[1]);
            if (strcmp("dhcp", mode)) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
            }

            // only if we are not in AP mode
            if (wlan_obj.mode != ROLE_AP) {
                _u8 val = 1;
                wlan_servers_stop();
                sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, 1, &val);
                wlan_reenable (wlan_obj.mode);
                wlan_servers_start();
            }
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_ifconfig_obj, 1, 2, wlan_ifconfig);

#if MICROPY_PORT_WLAN_URN
STATIC mp_obj_t wlan_urn (uint n_args, const mp_obj_t *args) {
    char urn[MAX_DEVICE_URN_LEN];
    uint8_t len = MAX_DEVICE_URN_LEN;

    // an URN is given, so set it
    if (n_args == 2) {
        const char *p = mp_obj_str_get_str(args[1]);
        uint8_t len = strlen(p);

        // the call to sl_NetAppSet corrupts the input string URN=args[1], so we copy into a local buffer
        if (len > MAX_DEVICE_URN_LEN) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
        }
        strcpy(urn, p);

        if (sl_NetAppSet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, len, (unsigned char *)urn) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
        }
    }
    else {
        // get the URN
        if (sl_NetAppGet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, &len, (uint8_t *)urn) < 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
        }
        return mp_obj_new_str(urn, (len - 1), false);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_urn_obj, 1, 2, wlan_urn);
#endif

/// \method wlan_netlist()
/// Return a list of tuples with all the acces points within range
STATIC mp_obj_t wlan_scan(mp_obj_t self_in) {
    STATIC const qstr wlan_scan_info_fields[] = {
        MP_QSTR_ssid, MP_QSTR_bssid,
        MP_QSTR_security, MP_QSTR_channel, MP_QSTR_rssi
    };

    Sl_WlanNetworkEntry_t wlanEntry;
    mp_obj_t nets = mp_obj_new_list(0, NULL);
    uint8_t _index = 0;

    // trigger a new network scan
    uint32_t scanSeconds = MODWLAN_SCAN_PERIOD_S;
    ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_SCAN , MODWLAN_SL_SCAN_ENABLE, (_u8 *)&scanSeconds, sizeof(scanSeconds)));

    // wait for the scan to complete
    HAL_Delay (MODWLAN_WAIT_FOR_SCAN_MS);

    do {
        if (sl_WlanGetNetworkList(_index++, 1, &wlanEntry) <= 0) {
            break;
        }

        // we must skip any duplicated results
        if (!wlan_scan_result_is_unique(nets, wlanEntry.bssid)) {
            continue;
        }

        mp_obj_t tuple[5];
        tuple[0] = mp_obj_new_str((const char *)wlanEntry.ssid, wlanEntry.ssid_len, false);
        tuple[1] = mp_obj_new_bytes((const byte *)wlanEntry.bssid, SL_BSSID_LENGTH);
        // 'normalize' the security type
        if (wlanEntry.sec_type > 2) {
            wlanEntry.sec_type = 2;
        }
        tuple[2] = mp_obj_new_int(wlanEntry.sec_type);
        tuple[3] = mp_const_none;
        tuple[4] = mp_obj_new_int(wlanEntry.rssi);

        // add the network to the list
        mp_obj_list_append(nets, mp_obj_new_attrtuple(wlan_scan_info_fields, 5, tuple));

    } while (_index < MODWLAN_SL_MAX_NETWORKS);

    return nets;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_scan_obj, wlan_scan);

/// \method callback(handler, pwrmode)
/// Create a callback object associated with WLAN
/// min num of arguments is 1 (pwrmode)
STATIC mp_obj_t wlan_callback (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[mpcallback_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, mpcallback_INIT_NUM_ARGS, mpcallback_init_args, args);

    wlan_obj_t *self = pos_args[0];
    mp_obj_t _callback = mpcallback_find(self);
    // check if any parameters were passed
    if (kw_args->used > 0 || !_callback) {
        // check the power mode
        if (args[4].u_int != PYB_PWR_MODE_LPDS) {
            // throw an exception since WLAN only supports LPDS mode
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
        }

        // create the callback
        _callback = mpcallback_new (self, args[1].u_obj, &wlan_cb_methods);

        // enable network wakeup
        pybsleep_set_wlan_lpds_callback (_callback);
    }
    return _callback;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wlan_callback_obj, 1, wlan_callback);

/// \method info()
/// returns (mode, ssid, security, mac)
STATIC mp_obj_t wlan_info (mp_obj_t self_in) {
    STATIC const qstr wlan_info_fields[] = {
        MP_QSTR_mode, MP_QSTR_ssid,
        MP_QSTR_security, MP_QSTR_mac
    };

    mp_obj_t wlan_info[4];
    wlan_info[0] = mp_obj_new_int(wlan_obj.mode);
    wlan_info[1] = wlan_obj.mode != ROLE_STA ?
              mp_obj_new_str((const char *)wlan_obj.ssid, strlen((const char *)wlan_obj.ssid), false) : MP_OBJ_NEW_QSTR(MP_QSTR_);
    wlan_info[2] = mp_obj_new_int(wlan_obj.security);
    wlan_info[3] = mp_obj_new_bytes((const byte *)wlan_obj.mac, SL_BSSID_LENGTH);
    return mp_obj_new_attrtuple(wlan_info_fields, MP_ARRAY_SIZE(wlan_info), wlan_info);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_info_obj, wlan_info);

/// \method connections()
/// returns (ssid/name, bssi), name is the P2P name if in this mode
STATIC mp_obj_t wlan_connections (mp_obj_t self_in) {
    mp_obj_t device[2];
    mp_obj_t connections = mp_obj_new_list(0, NULL);

    if (wlan_is_connected()) {
        device[0] = mp_obj_new_str((const char *)wlan_obj.ssid_o, strlen((const char *)wlan_obj.ssid_o), false);
        device[1] = mp_obj_new_bytes((const byte *)wlan_obj.bssid, SL_BSSID_LENGTH);
        // add the device to the list
        mp_obj_list_append(connections, mp_obj_new_tuple(MP_ARRAY_SIZE(device), device));
    }
    return connections;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_connections_obj, wlan_connections);

#if MICROPY_HW_ANTENNA_DIVERSITY
/// \method antenna()
/// select the antenna type to use (internal or external)
STATIC mp_obj_t wlan_antenna (mp_obj_t self_in, mp_obj_t antenna_o) {
    antenna_type_t _antenna = mp_obj_get_int(antenna_o);

    if (_antenna != ANTENNA_TYPE_INTERNAL && _antenna != ANTENNA_TYPE_EXTERNAL) {
        // invalid antenna type
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    antenna_select (_antenna);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wlan_antenna_obj, wlan_antenna);
#endif

STATIC const mp_map_elem_t wlan_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect),             (mp_obj_t)&wlan_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan),                (mp_obj_t)&wlan_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect),          (mp_obj_t)&wlan_disconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isconnected),         (mp_obj_t)&wlan_isconnected_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifconfig),            (mp_obj_t)&wlan_ifconfig_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_info),                (mp_obj_t)&wlan_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connections),         (mp_obj_t)&wlan_connections_obj },
#if MICROPY_HW_ANTENNA_DIVERSITY
    { MP_OBJ_NEW_QSTR(MP_QSTR_antenna),             (mp_obj_t)&wlan_antenna_obj },
#endif
#if MICROPY_PORT_WLAN_URN
    { MP_OBJ_NEW_QSTR(MP_QSTR_urn),                 (mp_obj_t)&wlan_urn_obj },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback),            (mp_obj_t)&wlan_callback_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_OPEN),                MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_OPEN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WEP),                 MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WEP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPA_WPA2),            MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA_WPA2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPA_ENT),             MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPA_ENT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPS_PBC),             MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPS_PBC) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPS_PIN),             MP_OBJ_NEW_SMALL_INT(SL_SEC_TYPE_WPS_PIN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STA),                 MP_OBJ_NEW_SMALL_INT(ROLE_STA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AP),                  MP_OBJ_NEW_SMALL_INT(ROLE_AP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_P2P),                 MP_OBJ_NEW_SMALL_INT(ROLE_P2P) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_ANTENNA),         MP_OBJ_NEW_SMALL_INT(ANTENNA_TYPE_INTERNAL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EXT_ANTENNA),         MP_OBJ_NEW_SMALL_INT(ANTENNA_TYPE_EXTERNAL) },
};
STATIC MP_DEFINE_CONST_DICT(wlan_locals_dict, wlan_locals_dict_table);

const mod_network_nic_type_t mod_network_nic_type_wlan = {
    .base = {
        { &mp_type_type },
        .name = MP_QSTR_WLAN,
        .make_new = wlan_make_new,
        .locals_dict = (mp_obj_t)&wlan_locals_dict,
    },
};

STATIC const mp_cb_methods_t wlan_cb_methods = {
    .init = wlan_callback,
    .enable = wlan_lpds_callback_enable,
    .disable = wlan_lpds_callback_disable,
};

/******************************************************************************/
// Micro Python bindings; WLAN socket

int wlan_gethostbyname(const char *name, mp_uint_t len, uint8_t *out_ip, uint8_t family) {
    uint32_t ip;
    int result = sl_NetAppDnsGetHostByName((_i8 *)name, (_u16)len, (_u32*)&ip, (_u8)family);

    out_ip[0] = ip;
    out_ip[1] = ip >> 8;
    out_ip[2] = ip >> 16;
    out_ip[3] = ip >> 24;

    return result;
}

int wlan_socket_socket(mod_network_socket_obj_t *s, int *_errno) {
    // open the socket
    int16_t sd = sl_Socket(s->u_param.domain, s->u_param.type, s->u_param.proto);
    // save the socket descriptor
    s->sd = sd;
    if (sd < 0) {
        *_errno = sd;
        return -1;
    }

    // mark the socket not closed
    s->closed = false;

    return 0;
}

void wlan_socket_close(mod_network_socket_obj_t *s) {
    // this is to prevent the finalizer to close a socket that failed when being created
    if (s->sd >= 0) {
        modusocket_socket_delete(s->sd);
        sl_Close(s->sd);
    }
    s->closed = true;
}

int wlan_socket_bind(mod_network_socket_obj_t *s, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_Bind(s->sd, &addr, sizeof(addr));
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

int wlan_socket_listen(mod_network_socket_obj_t *s, mp_int_t backlog, int *_errno) {
    int ret = sl_Listen(s->sd, backlog);
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

int wlan_socket_accept(mod_network_socket_obj_t *s, mod_network_socket_obj_t *s2, byte *ip, mp_uint_t *port, int *_errno) {
    // accept incoming connection
    int16_t sd;
    sockaddr addr;
    socklen_t addr_len = sizeof(addr);

    sd = sl_Accept(s->sd, &addr, &addr_len);
    // save the socket descriptor
    s2->sd = sd;
    if (sd < 0) {
        *_errno = sd;
        return -1;
    }

    // mark the socket not closed
    s2->closed = false;

    // return ip and port
    UNPACK_SOCKADDR(addr, ip, *port);

    return 0;
}

int wlan_socket_connect(mod_network_socket_obj_t *s, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_Connect(s->sd, &addr, sizeof(addr));
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

int wlan_socket_send(mod_network_socket_obj_t *s, const byte *buf, mp_uint_t len, int *_errno) {
    mp_int_t bytes = 0;
    if (len > 0) {
        bytes = sl_Send(s->sd, (const void *)buf, len, 0);
    }
    if (bytes <= 0) {
        *_errno = bytes;
        return -1;
    }

    return bytes;
}

int wlan_socket_recv(mod_network_socket_obj_t *s, byte *buf, mp_uint_t len, int *_errno) {
    // check if the socket is open
    if (s->closed) {
        // socket is closed, but the there might be data remaining in the buffer, so check
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(s->sd, &rfds);
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 2;
        int nfds = sl_Select(s->sd + 1, &rfds, NULL, NULL, &tv);
        if (nfds == -1 || !FD_ISSET(s->sd, &rfds)) {
            // no data waiting, so close the socket and return 0 data
            wlan_socket_close(s);
            return 0;
        }
    }

    // cap length at WLAN_MAX_RX_SIZE
    len = MIN(len, WLAN_MAX_RX_SIZE);

    // do the recv
    int ret = sl_Recv(s->sd, buf, len, 0);
    if (ret < 0) {
        *_errno = ret;
        return -1;
    }

    return ret;
}

int wlan_socket_sendto( mod_network_socket_obj_t *s, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_SendTo(s->sd, (byte*)buf, len, 0, (sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        *_errno = ret;
        return -1;
    }
    return ret;
}

int wlan_socket_recvfrom(mod_network_socket_obj_t *s, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    mp_int_t ret = sl_RecvFrom(s->sd, buf, len, 0, &addr, &addr_len);
    if (ret < 0) {
        *_errno = ret;
        return -1;
    }
    UNPACK_SOCKADDR(addr, ip, *port);
    return ret;
}

int wlan_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    int ret = sl_SetSockOpt(socket->sd, level, opt, optval, optlen);
    if (ret < 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

int wlan_socket_settimeout(mod_network_socket_obj_t *s, mp_uint_t timeout_ms, int *_errno) {
    int ret;
    if (timeout_ms == 0 || timeout_ms == -1) {
        int optval;
        if (timeout_ms == 0) {
            // set non-blocking mode
            optval = 1;
        } else {
            // set blocking mode
            optval = 0;
        }
        ret = sl_SetSockOpt(s->sd, SOL_SOCKET, SO_NONBLOCKING, &optval, sizeof(optval));
    } else {
        // set timeout
        ret = sl_SetSockOpt(s->sd, SOL_SOCKET, SO_RCVTIMEO, &timeout_ms, sizeof(timeout_ms));
    }

    if (ret != 0) {
        *_errno = ret;
        return -1;
    }

    return 0;
}

int wlan_socket_ioctl (mod_network_socket_obj_t *s, mp_uint_t request, mp_uint_t arg, int *_errno) {
    mp_int_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        int32_t sd = s->sd;

        // init fds
        fd_set rfds, wfds, xfds;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&xfds);

        // set fds if needed
        if (flags & MP_IOCTL_POLL_RD) {
            FD_SET(sd, &rfds);

            // A socked that just closed is available for reading.  A call to
            // recv() returns 0 which is consistent with BSD.
            if (s->closed) {
                ret |= MP_IOCTL_POLL_RD;
            }
        }
        if (flags & MP_IOCTL_POLL_WR) {
            FD_SET(sd, &wfds);
        }
        if (flags & MP_IOCTL_POLL_HUP) {
            FD_SET(sd, &xfds);
        }

        // call simplelink select with minimum timeout
        SlTimeval_t tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1;
        int32_t nfds = sl_Select(sd + 1, &rfds, &wfds, &xfds, &tv);

        // check for error
        if (nfds == -1) {
            *_errno = nfds;
            return -1;
        }

        // check return of select
        if (FD_ISSET(sd, &rfds)) {
            ret |= MP_IOCTL_POLL_RD;
        }
        if (FD_ISSET(sd, &wfds)) {
            ret |= MP_IOCTL_POLL_WR;
        }
        if (FD_ISSET(sd, &xfds)) {
            ret |= MP_IOCTL_POLL_HUP;
        }
    } else {
        *_errno = EINVAL;
        ret = -1;
    }
    return ret;
}

