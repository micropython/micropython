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

#include "std.h"
#include <stdint.h>
#include <stdbool.h>

#include "simplelink.h"
#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "py/runtime.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "pybioctl.h"
#include "debug.h"
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
#include "serverstask.h"
#endif
#include "mpexception.h"


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
    mp_obj_base_t   base;
    SlWlanMode_t    mode;
    uint32_t        status;

    uint32_t        ip;
    uint32_t        gateway;
    uint32_t        dns;

#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    bool            servers_enabled;
#endif
    uint8_t         security;
    uint8_t         mac[SL_MAC_ADDR_LEN];
    uint8_t         ssid[33];
    uint8_t         bssid[6];

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

#define ASSERT_ON_ERROR( x )            ASSERT((x) >= 0 )

#define IPV4_ADDR_STR_LEN_MAX           (16)
#define SL_STOP_TIMEOUT                 250

#define WLAN_MAX_RX_SIZE                16000
#define WLAN_MAX_TX_SIZE                1476


#define MAKE_SOCKADDR(addr, ip, port)       sockaddr addr; \
                                            addr.sa_family = AF_INET; \
                                            addr.sa_data[0] = port >> 8; \
                                            addr.sa_data[1] = port; \
                                            addr.sa_data[2] = ip[0]; \
                                            addr.sa_data[3] = ip[1]; \
                                            addr.sa_data[4] = ip[2]; \
                                            addr.sa_data[5] = ip[3];

#define UNPACK_SOCKADDR(addr, ip, port)     port = (addr.sa_data[0] << 8) | addr.sa_data[1]; \
                                            ip[0] = addr.sa_data[2]; \
                                            ip[1] = addr.sa_data[3]; \
                                            ip[2] = addr.sa_data[4]; \
                                            ip[3] = addr.sa_data[5];

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC wlan_obj_t wlan_obj;

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
OsiLockObj_t wlan_LockObj;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void wlan_initialize_data (void);
STATIC void wlan_reenable (SlWlanMode_t mode);
STATIC void wlan_get_sl_mac (void);
STATIC modwlan_Status_t wlan_do_connect (const char* ssid, uint32_t ssid_len, const char* bssid, uint8_t sec,
                                         const char* key, uint32_t key_len);


//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(!pWlanEvent) {
        return;
    }

    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION);
            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'-Applications
            // can use it if required
            //
            slWlanConnectAsyncResponse_t *pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;

            // Copy new connection SSID and BSSID to global parameters
            memcpy(wlan_obj.ssid, pEventData->ssid_name, pEventData->ssid_len);
            memcpy(wlan_obj.bssid, pEventData->bssid, SL_BSSID_LENGTH);
        }
            break;
        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(wlan_obj.status, STATUS_BIT_IP_ACQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated the 'Disconnect' request,
            //'reason_code' is SL_USER_INITIATED_DISCONNECTION
            if (SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code) {
                // TODO ...
            }
            else {
                // TODO: Maybe trow an exception?
            }
            memset(wlan_obj.ssid, 0, sizeof(wlan_obj.ssid));
            memset(wlan_obj.bssid, 0, sizeof(wlan_obj.bssid));
        }
            break;
        case SL_WLAN_STA_CONNECTED_EVENT:
            break;
        case SL_WLAN_STA_DISCONNECTED_EVENT:
            break;
        case SL_WLAN_P2P_DEV_FOUND_EVENT:
            break;
        case SL_WLAN_P2P_NEG_REQ_RECEIVED_EVENT:
            break;
        case SL_WLAN_CONNECTION_FAILED_EVENT:
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
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
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
            wlan_obj.gateway = ntohl(pEventData->gateway);
            wlan_obj.ip      = ntohl(pEventData->ip);
            wlan_obj.dns     = ntohl(pEventData->dns);
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
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent, SlHttpServerResponse_t *pHttpResponse)
{
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
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    if (!pDevEvent) {
        return;
    }

    ASSERT (false);
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
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if (!pSock) {
        return;
    }

    switch( pSock->Event ) {
    case SL_SOCKET_TX_FAILED_EVENT:
        break;
    default:
        break;
    }
}

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************

void wlan_init0 (void) {
    // Set the mode to an invalid one
    wlan_obj.mode = -1;
    wlan_obj.base.type = NULL;
    memset (wlan_obj.mac, 0, SL_MAC_ADDR_LEN);
    ASSERT(OSI_OK == sl_LockObjCreate(&wlan_LockObj, "WlanLock"));
    wlan_initialize_data ();
}

modwlan_Status_t wlan_sl_enable (SlWlanMode_t mode, const char *ssid, uint8_t ssid_len, uint8_t sec,
                                 const char *key, uint8_t key_len, uint8_t channel) {

    if (mode == ROLE_STA || mode == ROLE_AP || mode == ROLE_P2P) {
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
        // Stop all other processes using the wlan engine
        if ((wlan_obj.servers_enabled = servers_are_enabled())) {
            wlan_stop_servers();
        }
#endif
        if (wlan_obj.mode < 0) {
            wlan_obj.mode = sl_Start(0, 0, 0);
            sl_LockObjUnlock (&wlan_LockObj);
        }

        // get the mac address
        wlan_get_sl_mac();

        // stop the device if it's not in station mode
        if (wlan_obj.mode != ROLE_STA) {
            if (ROLE_AP == wlan_obj.mode) {
                // if the device is in AP mode, we need to wait for this event
                // before doing anything
                while (!IS_IP_ACQUIRED(wlan_obj.status)) {
                #ifndef SL_PLATFORM_MULTI_THREADED
                    _SlTaskEntry();
                #endif
                    HAL_Delay (5);
                }
            }
            // switch to STA mode
            ASSERT_ON_ERROR(sl_WlanSetMode(ROLE_STA));
            // stop and start again
            wlan_reenable(ROLE_STA);
        }

        // Device in station-mode. Disconnect previous connection if any
        // The function returns 0 if 'Disconnected done', negative number if already
        // disconnected Wait for 'disconnection' event if 0 is returned, Ignore
        // other return-codes
        if (0 == sl_WlanDisconnect()) {
            while (IS_CONNECTED (wlan_obj.status)) {
            #ifndef SL_PLATFORM_MULTI_THREADED
                _SlTaskEntry();
            #endif
                HAL_Delay (5);
            }
        }

        // clear wlan data after checking any of the status flags
        wlan_initialize_data ();
        wlan_obj.security = sec;

        // Set connection policy to Auto + SmartConfig (Device's default connection policy)
        ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0));

        // Remove all profiles
        ASSERT_ON_ERROR(sl_WlanProfileDel(0xFF));

        // Enable the DHCP client
        uint8_t value = 1;
        ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &value));

        // Set PM policy to normal
        ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0));

        // Unregister mDNS services
        ASSERT_ON_ERROR(sl_NetAppMDNSUnRegisterService(0, 0));

        // Remove all 64 filters (8 * 8)
        _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask;
        memset ((void *)&RxFilterIdMask, 0 ,sizeof(RxFilterIdMask));
        memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
        ASSERT_ON_ERROR(sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask, sizeof(_WlanRxFilterOperationCommandBuff_t)));

        // Set Tx power level for station or AP mode
        // Number between 0-15, as dB offset from max power - 0 will set max power
        uint8_t ucPower = 0;
        if (mode == ROLE_AP) {
            // Disable the scanning
            ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_SCAN, MODWLAN_SL_SCAN_DISABLE, NULL, 0));

            // Switch to AP mode
            ASSERT_ON_ERROR(sl_WlanSetMode(mode));
            ASSERT (ssid != NULL && key != NULL);
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_AP_TX_POWER, sizeof(ucPower),
                                       (unsigned char *)&ucPower));
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, ssid_len, (unsigned char *)ssid));
            memcpy(wlan_obj.ssid, (unsigned char *)ssid, ssid_len);
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SECURITY_TYPE, sizeof(uint8_t), &sec));
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_PASSWORD, key_len, (unsigned char *)key));
            _u8*  country = (_u8*)"EU";
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_COUNTRY_CODE, 2, country));
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_CHANNEL, 1, (_u8 *)&channel));

            // Stop and start again
            wlan_reenable(mode);
            ASSERT (wlan_obj.mode == mode);

            SlNetAppDhcpServerBasicOpt_t dhcpParams;
            dhcpParams.lease_time      =  4096;                             // lease time (in seconds) of the IP Address
            dhcpParams.ipv4_addr_start =  SL_IPV4_VAL(192,168,1,2);         // first IP Address for allocation.
            dhcpParams.ipv4_addr_last  =  SL_IPV4_VAL(192,168,1,254);       // last IP Address for allocation.
            ASSERT_ON_ERROR(sl_NetAppStop(SL_NET_APP_DHCP_SERVER_ID));      // Stop DHCP server before settings
            ASSERT_ON_ERROR(sl_NetAppSet(SL_NET_APP_DHCP_SERVER_ID, NETAPP_SET_DHCP_SRV_BASIC_OPT,
                                         sizeof(SlNetAppDhcpServerBasicOpt_t), (_u8* )&dhcpParams));  // set parameters
            ASSERT_ON_ERROR(sl_NetAppStart(SL_NET_APP_DHCP_SERVER_ID));     // Start DHCP server with new settings

            SlNetCfgIpV4Args_t ipV4;
            ipV4.ipV4          = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 IP address
            ipV4.ipV4Mask      = (_u32)SL_IPV4_VAL(255,255,255,0);          // _u32 Subnet mask for this AP
            ipV4.ipV4Gateway   = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 Default gateway address
            ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 DNS server address
            ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4,
                                         sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4));

            // Stop and start again
            wlan_reenable(mode);
        }
        // STA and P2P modes
        else {
            ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER,
                                       sizeof(ucPower), (unsigned char *)&ucPower));
            // Enable scanning every 60 seconds
            uint32_t scanSeconds = 60;
            ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_SCAN , MODWLAN_SL_SCAN_ENABLE, (_u8 *)&scanSeconds, sizeof(scanSeconds)));

            if (mode == ROLE_P2P) {
                // Switch to P2P mode
                ASSERT_ON_ERROR(sl_WlanSetMode(mode));
                // Stop and start again
                wlan_reenable(mode);
            }
        }
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
        // Start the servers again
        if (wlan_obj.servers_enabled) {
            servers_enable();
        }
#endif
        return MODWLAN_OK;
    }
    return MODWLAN_ERROR_INVALID_PARAMS;
}

void wlan_update(void) {
#ifndef SL_PLATFORM_MULTI_THREADED
    _SlTaskEntry();
#endif
}

// call this function to disable the complete WLAN subsystem in order to save power
void wlan_stop (void) {
    if (wlan_obj.mode >= 0) {
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
        // Stop all other processes using the wlan engine
        if ((wlan_obj.servers_enabled = servers_are_enabled())) {
            wlan_stop_servers();
        }
#endif
        sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
        wlan_obj.mode = -1;
        sl_Stop(SL_STOP_TIMEOUT);
    }
}

// cal this function to reenable the WLAN susbsystem after a previous call to wlan_sl_disable()
// WLAN will remain configured as it was before being disabled
void wlan_start (void) {
    if (wlan_obj.mode < 0) {
        wlan_obj.mode = sl_Start(0, 0, 0);
        sl_LockObjUnlock (&wlan_LockObj);
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
        // Start the servers again
        if (wlan_obj.servers_enabled) {
            servers_enable();
        }
#endif
    }
}

SlWlanMode_t wlan_get_mode (void) {
    return wlan_obj.mode;
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

void wlan_set_pm_policy (uint8_t policy) {
    ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_PM, policy, NULL, 0));
}

void wlan_stop_servers (void) {
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    servers_disable();
    do {
        HAL_Delay (5);
    } while (servers_are_enabled());
#endif
}

//*****************************************************************************
// DEFINE STATIC FUNCTIONS
//*****************************************************************************

STATIC void wlan_initialize_data (void) {
    wlan_obj.status = 0;
    wlan_obj.dns = 0;
    wlan_obj.gateway = 0;
    wlan_obj.ip = 0;
    wlan_obj.security = SL_SEC_TYPE_OPEN;
    memset(wlan_obj.ssid, 0, sizeof(wlan_obj.ssid));
    memset(wlan_obj.bssid, 0, sizeof(wlan_obj.bssid));
}

STATIC void wlan_reenable (SlWlanMode_t mode) {
    // Stop and start again
    wlan_obj.mode = -1;
    sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
    sl_Stop(SL_STOP_TIMEOUT);
    wlan_obj.mode = sl_Start(0, 0, 0);
    sl_LockObjUnlock (&wlan_LockObj);
    ASSERT (wlan_obj.mode == mode);
}

STATIC modwlan_Status_t wlan_do_connect (const char* ssid, uint32_t ssid_len, const char* bssid, uint8_t sec,
                                         const char* key, uint32_t key_len) {
    SlSecParams_t secParams;

    secParams.Key = (_i8*)key;
    secParams.KeyLen = ((key != NULL) ? key_len : 0);
    secParams.Type = sec;

    if (0 == sl_WlanConnect((_i8*)ssid, ssid_len, (_u8*)bssid, &secParams, NULL)) {

        // Wait for the WLAN Event
        uint32_t waitForConnectionMs = 0;
        while (!IS_CONNECTED(wlan_obj.status)) {
        #ifndef SL_PLATFORM_MULTI_THREADED
            _SlTaskEntry();
        #endif
            HAL_Delay (5);
            if (++waitForConnectionMs >= MODWLAN_TIMEOUT_MS) {
                return MODWLAN_ERROR_TIMEOUT;
            }
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

/// \method init(mode, ssid=myWlan, security=wlan.WPA_WPA2, key=myWlanKey)
///
/// Initialise the UART bus with the given parameters:
///
///   - `mode` can be ROLE_AP, ROLE_STA and ROLE_P2P.
///   - `ssid` is the network ssid in case of AP mode
///   - `security` is the security type for AP mode
///   - `key` is the key when in AP mode
///   - `channel` is the channel to use for the AP network
STATIC const mp_arg_t wlan_init_args[] = {
    { MP_QSTR_mode,         MP_ARG_REQUIRED | MP_ARG_INT,  {.u_int = ROLE_STA} },
    { MP_QSTR_ssid,         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
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
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, mpexception_value_invalid_arguments));
    }

    // Force the channel to be between 1-11
    uint8_t channel = args[4].u_int > 0 ? args[4].u_int % 12 : 1;

    if (MODWLAN_OK != wlan_sl_enable (args[0].u_int, ssid, ssid_len, args[2].u_int, key, key_len, channel)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }

    return mp_const_none;
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
        // Get the mode
        SlWlanMode_t mode = mp_obj_get_int(args[0]);
        if (mode == ROLE_AP) {
            // start the peripheral
            mp_map_t kw_args;
            mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
            wlan_init_helper(n_args, args, &kw_args);
        }
        // TODO: Only STA mode supported for the moment. What if P2P?
        else if (n_args == 1) {
            if (MODWLAN_OK != wlan_sl_enable (mode, NULL, 0, 0, NULL, 0, 0)) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
            }
        }
        else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, mpexception_num_type_invalid_arguments));
        }
    } else if (wlan_obj.mode < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, mpexception_num_type_invalid_arguments));
    }

    wlan_obj.base.type = (mp_obj_type_t*)&mod_network_nic_type_wlan;
    // register with the network module
    mod_network_register_nic(&wlan_obj);

    return &wlan_obj;
}

STATIC void wlan_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    wlan_obj_t *self = self_in;
    print(env, "<WLAN mode=%u", self->mode);

    // only print the bssid if in station mode
    if (self->mode != ROLE_AP && GET_STATUS_BIT(self->status, STATUS_BIT_CONNECTION)) {
        print(env, ", connected to: ssid=%s, bssid=%02x:%02x:%02x:%02x:%02x:%02x", self->ssid,
              self->bssid[0], self->bssid[1], self->bssid[2], self->bssid[3], self->bssid[4], self->bssid[5]);
    }
    else {
        print(env, ", ssid=%s", self->ssid);
    }
    print(env, ", security=%u>", self->security);
}

/// \method mode()
/// Get the wlan mode:
///
///   - Returns the current wlan mode. Possible values are:
///     ROLE_STA, ROLE_AP and ROLE_P2P
///
STATIC mp_obj_t wlan_getmode(mp_obj_t self_in) {
    wlan_obj_t* self = self_in;
    return mp_obj_new_int(self->mode);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_getmode_obj, wlan_getmode);

STATIC mp_obj_t wlan_setpm(mp_obj_t self_in, mp_obj_t pm_mode) {
    mp_int_t mode = mp_obj_get_int(pm_mode);
    if (mode < SL_NORMAL_POLICY || mode > SL_LONG_SLEEP_INTERVAL_POLICY) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
    wlan_set_pm_policy((uint8_t)mode);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wlan_setpm_obj, wlan_setpm);

/// \method connect(ssid, security=OPEN, key=None, bssid=None)
//          if security is WPA/WPA2, the key must be a string
///         if security is WEP, the key must be binary
STATIC mp_obj_t wlan_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_security, MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = SL_SEC_TYPE_OPEN} },
        { MP_QSTR_key,      MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid,    MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
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
    if (args[2].u_obj != mp_const_none) {
        // wep key must be given as raw bytes
        if (sec == SL_SEC_TYPE_WEP) {
            mp_get_buffer_raise(args[2].u_obj, &wepkey, MP_BUFFER_READ);
            key = wepkey.buf;
            key_len = wepkey.len;
        }
        else {
            key = mp_obj_str_get_data(args[2].u_obj, &key_len);
        }
    }

    // get bssid
    const char *bssid = NULL;
    if (args[3].u_obj != mp_const_none) {
        bssid = mp_obj_str_get_str(args[3].u_obj);
    }

    if (GET_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION)) {
        if (0 == sl_WlanDisconnect()) {
            while (IS_CONNECTED(wlan_obj.status)) {
            #ifndef SL_PLATFORM_MULTI_THREADED
                _SlTaskEntry();
            #endif
                HAL_Delay (5);
            }
        }
    }

    // connect to the requested access point
    modwlan_Status_t status;
    status = wlan_do_connect (ssid, ssid_len, bssid, sec, key, key_len);
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
/// Closes the current WLAN connection
///
STATIC mp_obj_t wlan_disconnect(mp_obj_t self_in) {
    sl_WlanDisconnect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_disconnect_obj, wlan_disconnect);

/// \method is_connected()
/// Returns true if connected to the AP and an IP address has been assigned. False otherwise.
///
STATIC mp_obj_t wlan_isconnected(mp_obj_t self_in) {
    if (GET_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION) &&
        GET_STATUS_BIT(wlan_obj.status, STATUS_BIT_IP_ACQUIRED)) {
        return mp_const_true;
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_isconnected_obj, wlan_isconnected);

STATIC mp_obj_t wlan_ifconfig (mp_obj_t self_in) {

    unsigned char len = sizeof(SlNetCfgIpV4Args_t);
    unsigned char dhcpIsOn;
    SlNetCfgIpV4Args_t ipV4;

    sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &len, (uint8_t *)&ipV4);
    // shift byte order
    ipV4.ipV4Mask = ntohl(ipV4.ipV4Mask);

    mp_obj_t ifconfig = mp_obj_new_dict(0);
    mp_obj_dict_store (ifconfig, mp_obj_new_str("ip", strlen("ip"), false), mod_network_format_ipv4_addr((uint8_t *)&wlan_obj.ip));
    mp_obj_dict_store (ifconfig, mp_obj_new_str("subnet", strlen("subnet"), false), mod_network_format_ipv4_addr((uint8_t *)&ipV4.ipV4Mask));
    mp_obj_dict_store (ifconfig, mp_obj_new_str("gateway", strlen("gateway"), false), mod_network_format_ipv4_addr((uint8_t *)&wlan_obj.gateway));
    mp_obj_dict_store (ifconfig, mp_obj_new_str("dns", strlen("dns"), false), mod_network_format_ipv4_addr((uint8_t *)&wlan_obj.dns));
    char mac_str[18];
    mp_uint_t mac_len = snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x", wlan_obj.mac[0], wlan_obj.mac[1], wlan_obj.mac[2],
                                 wlan_obj.mac[3], wlan_obj.mac[4], wlan_obj.mac[5]);
    mp_obj_dict_store (ifconfig, mp_obj_new_str("mac", strlen("mac"), false), mp_obj_new_str(mac_str, mac_len, false));
    char *mode_str;
    if (wlan_obj.mode == ROLE_STA) {
        mode_str = "station";
    }
    else if (wlan_obj.mode == ROLE_AP) {
        mode_str = "ap";
    }
    else {
        mode_str = "p2p";
    }
    mp_obj_dict_store (ifconfig, mp_obj_new_str("mode", strlen("mode"), false), mp_obj_new_str(mode_str, strlen(mode_str), false));
    mp_obj_dict_store (ifconfig, mp_obj_new_str("ssid", strlen("ssid"), false), mp_obj_new_str((const char *)wlan_obj.ssid, strlen((const char *)wlan_obj.ssid), false));

    return ifconfig;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_ifconfig_obj, wlan_ifconfig);

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

/// \method wlan_netlist()
/// Returns a list of tuples with all the acces points within range
STATIC mp_obj_t wlan_scan(mp_obj_t self_in) {
    Sl_WlanNetworkEntry_t wlanEntry;
    uint8_t _index = 0;
    mp_obj_t nets = NULL;

    do {
        if (sl_WlanGetNetworkList(_index++, 1, &wlanEntry) <= 0) {
            break;
        }
        mp_obj_t tuple[4];

        tuple[0] = mp_obj_new_str((const char *)wlanEntry.ssid, wlanEntry.ssid_len, false);
        tuple[1] = mp_obj_new_str((const char *)wlanEntry.bssid, SL_BSSID_LENGTH, false);
        // 'Normalize' the security type
        if (wlanEntry.sec_type > 2) {
            wlanEntry.sec_type = 2;
        }
        tuple[2] = mp_obj_new_int(wlanEntry.sec_type);
        tuple[3] = mp_obj_new_int(wlanEntry.rssi);

        if (_index == 1) {
            // Initialize the set
            nets = mp_obj_new_set(0, NULL);
        }
        // Add the network found to the list if it's unique
        mp_obj_set_store(nets, mp_obj_new_tuple(4, tuple));

    } while (_index < MODWLAN_SL_MAX_NETWORKS);

    return (nets != NULL) ? nets : mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_scan_obj, wlan_scan);

#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
STATIC mp_obj_t wlan_serversstart(mp_obj_t self_in) {
    servers_enable();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_serversstart_obj, wlan_serversstart);

STATIC mp_obj_t wlan_serversstop(mp_obj_t self_in) {
    wlan_stop_servers();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_serversstop_obj, wlan_serversstop);

STATIC mp_obj_t wlan_serversenabled(mp_obj_t self_in) {
    return MP_BOOL(servers_are_enabled());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_serversenabled_obj, wlan_serversenabled);

STATIC mp_obj_t wlan_serversuserpass(mp_obj_t self_in, mp_obj_t user, mp_obj_t pass) {
    const char *_user = mp_obj_str_get_str(user);
    const char *_pass = mp_obj_str_get_str(pass);
    servers_set_user_pass((char *)_user, (char *)_pass);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(wlan_serversuserpass_obj, wlan_serversuserpass);
#endif

STATIC const mp_map_elem_t wlan_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect),             (mp_obj_t)&wlan_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getmode),             (mp_obj_t)&wlan_getmode_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setpm),               (mp_obj_t)&wlan_setpm_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan),                (mp_obj_t)&wlan_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect),          (mp_obj_t)&wlan_disconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isconnected),         (mp_obj_t)&wlan_isconnected_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifconfig),            (mp_obj_t)&wlan_ifconfig_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_urn),                 (mp_obj_t)&wlan_urn_obj },
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    { MP_OBJ_NEW_QSTR(MP_QSTR_start_servers),       (mp_obj_t)&wlan_serversstart_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stop_servers),        (mp_obj_t)&wlan_serversstop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_servers_enabled),     (mp_obj_t)&wlan_serversenabled_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_servers_userpass),    (mp_obj_t)&wlan_serversuserpass_obj },
#endif

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
    { MP_OBJ_NEW_QSTR(MP_QSTR_NORMAL_PM),           MP_OBJ_NEW_SMALL_INT(SL_NORMAL_POLICY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LOW_LATENCY_PM),      MP_OBJ_NEW_SMALL_INT(SL_LOW_LATENCY_POLICY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LOW_POWER_PM),        MP_OBJ_NEW_SMALL_INT(SL_LOW_POWER_POLICY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ALWAYS_ON_PM),        MP_OBJ_NEW_SMALL_INT(SL_ALWAYS_ON_POLICY) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LONG_SLEEP_PM),       MP_OBJ_NEW_SMALL_INT(SL_LONG_SLEEP_INTERVAL_POLICY) },
};
STATIC MP_DEFINE_CONST_DICT(wlan_locals_dict, wlan_locals_dict_table);


/******************************************************************************/
// Micro Python bindings; WLAN socket

STATIC int wlan_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip, uint8_t family) {
    uint32_t ip;
    int result = sl_NetAppDnsGetHostByName((_i8 *)name, (_u16)len, (_u32*)&ip, (_u8)family);

    out_ip[0] = ip >> 24;
    out_ip[1] = ip >> 16;
    out_ip[2] = ip >> 8;
    out_ip[3] = ip;

    return result;
}

STATIC int wlan_socket_socket(struct _mod_network_socket_obj_t *s, int *_errno) {
    // open the socket
    int16_t sd = sl_Socket(s->u_param.domain, s->u_param.type, s->u_param.proto);
    if (s->sd < 0) {
        *_errno = s->sd;
        return -1;
    }

    // mark the socket not closed
    s->closed = false;
    // save the socket descriptor
    s->sd = sd;

    // make it blocking by default
    int32_t optval = 0;
    sl_SetSockOpt(sd, SOL_SOCKET, SO_NONBLOCKING, &optval, (SlSocklen_t)sizeof(optval));

    return 0;
}

STATIC void wlan_socket_close(mod_network_socket_obj_t *s) {
    s->closed = true;
    sl_Close(s->sd);
}

STATIC int wlan_socket_bind(mod_network_socket_obj_t *s, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_Bind(s->sd, &addr, sizeof(addr));
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int wlan_socket_listen(mod_network_socket_obj_t *s, mp_int_t backlog, int *_errno) {
    int ret = sl_Listen(s->sd, backlog);
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int wlan_socket_accept(mod_network_socket_obj_t *s, mod_network_socket_obj_t *s2, byte *ip, mp_uint_t *port, int *_errno) {
    // accept incoming connection
    int16_t sd;
    sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    if ((sd = sl_Accept(s->sd, &addr, &addr_len)) < 0) {
        *_errno = sd;
        return -1;
    }

    // Mark the socket not closed and save the new descriptor
    s2->closed = false;
    s2->sd = sd;

    // return ip and port
    UNPACK_SOCKADDR(addr, ip, *port);

    return 0;
}

STATIC int wlan_socket_connect(mod_network_socket_obj_t *s, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_Connect(s->sd, &addr, sizeof(addr));
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int wlan_socket_send(mod_network_socket_obj_t *s, const byte *buf, mp_uint_t len, int *_errno) {
    if (s->closed) {
        sl_Close (s->sd);
        *_errno = EBADF;
        return -1;
    }

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

STATIC int wlan_socket_recv(mod_network_socket_obj_t *s, byte *buf, mp_uint_t len, int *_errno) {
    // check if the socket is open
    if (s->closed) {
        // socket is closed, but the CC3200 may have some data remaining in its buffer, so check
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(s->sd, &rfds);
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 2;
        int nfds = sl_Select(s->sd + 1, &rfds, NULL, NULL, &tv);
        if (nfds == -1 || !FD_ISSET(s->sd, &rfds)) {
            // no data waiting, so close socket and return 0 data
            sl_Close(s->sd);
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

STATIC int wlan_socket_sendto( mod_network_socket_obj_t *s, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = sl_SendTo(s->sd, (byte*)buf, len, 0, (sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        *_errno = ret;
        return -1;
    }
    return ret;
}

STATIC int wlan_socket_recvfrom(mod_network_socket_obj_t *s, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
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

STATIC int wlan_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    int ret = sl_SetSockOpt(socket->sd, level, opt, optval, optlen);
    if (ret < 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int wlan_socket_settimeout(mod_network_socket_obj_t *s, mp_uint_t timeout_ms, int *_errno) {
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

STATIC int wlan_socket_ioctl (mod_network_socket_obj_t *s, mp_uint_t request, mp_uint_t arg, int *_errno) {
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

const mod_network_nic_type_t mod_network_nic_type_wlan = {
    .base = {
        { &mp_type_type },
        .name = MP_QSTR_WLAN,
        .print = wlan_print,
        .make_new = wlan_make_new,
        .locals_dict = (mp_obj_t)&wlan_locals_dict,
    },
    .gethostbyname = wlan_gethostbyname,
    .socket = wlan_socket_socket,
    .close = wlan_socket_close,
    .bind = wlan_socket_bind,
    .listen = wlan_socket_listen,
    .accept = wlan_socket_accept,
    .connect = wlan_socket_connect,
    .send = wlan_socket_send,
    .recv = wlan_socket_recv,
    .sendto = wlan_socket_sendto,
    .recvfrom = wlan_socket_recvfrom,
    .setsockopt = wlan_socket_setsockopt,
    .settimeout = wlan_socket_settimeout,
    .ioctl = wlan_socket_ioctl,
};
