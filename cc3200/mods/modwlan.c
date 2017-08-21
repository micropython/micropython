/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include <stdio.h>

#include "simplelink.h"
#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "lib/timeutils/timeutils.h"
#include "lib/netutils/netutils.h"
#include "modnetwork.h"
#include "modusocket.h"
#include "modwlan.h"
#include "pybrtc.h"
#include "debug.h"
#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
#include "serverstask.h"
#endif
#include "mpexception.h"
#include "mpirq.h"
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
} e_StatusBits;

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

#define MODWLAN_MAX_NETWORKS            20
#define MODWLAN_SCAN_PERIOD_S           3600     // 1 hour
#define MODWLAN_WAIT_FOR_SCAN_MS        1050
#define MODWLAN_CONNECTION_WAIT_MS      2

#define ASSERT_ON_ERROR(x)              ASSERT((x) >= 0)

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC wlan_obj_t wlan_obj = {
        .mode = -1,
        .status = 0,
        .ip = 0,
        .auth = MICROPY_PORT_WLAN_AP_SECURITY,
        .channel = MICROPY_PORT_WLAN_AP_CHANNEL,
        .ssid = MICROPY_PORT_WLAN_AP_SSID,
        .key = MICROPY_PORT_WLAN_AP_KEY,
        .mac = {0},
        //.ssid_o = {0},
        //.bssid = {0},
    #if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
        .servers_enabled = false,
    #endif
};

STATIC const mp_irq_methods_t wlan_irq_methods;

/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
#ifdef SL_PLATFORM_MULTI_THREADED
OsiLockObj_t wlan_LockObj;
#endif

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void wlan_clear_data (void);
STATIC void wlan_reenable (SlWlanMode_t mode);
STATIC void wlan_servers_start (void);
STATIC void wlan_servers_stop (void);
STATIC void wlan_reset (void);
STATIC void wlan_validate_mode (uint mode);
STATIC void wlan_set_mode (uint mode);
STATIC void wlan_validate_ssid_len (uint32_t len);
STATIC void wlan_set_ssid (const char *ssid, uint8_t len, bool add_mac);
STATIC void wlan_validate_security (uint8_t auth, const char *key, uint8_t len);
STATIC void wlan_set_security (uint8_t auth, const char *key, uint8_t len);
STATIC void wlan_validate_channel (uint8_t channel);
STATIC void wlan_set_channel (uint8_t channel);
#if MICROPY_HW_ANTENNA_DIVERSITY
STATIC void wlan_validate_antenna (uint8_t antenna);
STATIC void wlan_set_antenna (uint8_t antenna);
#endif
STATIC void wlan_sl_disconnect (void);
STATIC modwlan_Status_t wlan_do_connect (const char* ssid, uint32_t ssid_len, const char* bssid, uint8_t sec,
                                         const char* key, uint32_t key_len, int32_t timeout);
STATIC void wlan_get_sl_mac (void);
STATIC void wlan_wep_key_unhexlify (const char *key, char *key_out);
STATIC void wlan_lpds_irq_enable (mp_obj_t self_in);
STATIC void wlan_lpds_irq_disable (mp_obj_t self_in);
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
            //slWlanConnectAsyncResponse_t *pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            // copy the new connection data
            //memcpy(wlan_obj.bssid, pEventData->bssid, SL_BSSID_LENGTH);
            //memcpy(wlan_obj.ssid_o, pEventData->ssid_name, pEventData->ssid_len);
            //wlan_obj.ssid_o[pEventData->ssid_len] = '\0';
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
            servers_wlan_cycle_power();
        #endif
            break;
        case SL_WLAN_STA_CONNECTED_EVENT:
        {
            //slPeerInfoAsyncResponse_t *pEventData = &pWlanEvent->EventData.APModeStaConnected;
            // get the mac address and name of the connected device
            //memcpy(wlan_obj.bssid, pEventData->mac, SL_BSSID_LENGTH);
            //memcpy(wlan_obj.ssid_o, pEventData->go_peer_device_name, pEventData->go_peer_device_name_len);
            //wlan_obj.ssid_o[pEventData->go_peer_device_name_len] = '\0';
            SET_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION);
        #if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
            // we must reset the servers in case that the last connection
            // was lost without any notification being received
            servers_reset();
        #endif
        }
            break;
        case SL_WLAN_STA_DISCONNECTED_EVENT:
            CLR_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION);
        #if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
            servers_reset();
            servers_wlan_cycle_power();
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

            // Get the ip
            wlan_obj.ip      = pEventData->ip;
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
    #ifdef SL_PLATFORM_MULTI_THREADED
    ASSERT(OSI_OK == sl_LockObjCreate(&wlan_LockObj, "WlanLock"));
    #endif
}

void wlan_first_start (void) {
    if (wlan_obj.mode < 0) {
        CLR_STATUS_BIT_ALL(wlan_obj.status);
        wlan_obj.mode = sl_Start(0, 0, 0);
        #ifdef SL_PLATFORM_MULTI_THREADED
        sl_LockObjUnlock (&wlan_LockObj);
        #endif
    }

    // get the mac address
    wlan_get_sl_mac();
}

void wlan_sl_init (int8_t mode, const char *ssid, uint8_t ssid_len, uint8_t auth, const char *key, uint8_t key_len,
                   uint8_t channel, uint8_t antenna, bool add_mac) {

    // stop the servers
    wlan_servers_stop();

    // do a basic start
    wlan_first_start();

    // close any active connections
    wlan_sl_disconnect();

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

#if MICROPY_HW_ANTENNA_DIVERSITY
    // set the antenna type
    wlan_set_antenna (antenna);
#endif

    // switch to the requested mode
    wlan_set_mode(mode);

    // stop and start again (we need to in the propper mode from now on)
    wlan_reenable(mode);

    // Set Tx power level for station or AP mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    uint8_t ucPower = 0;
    if (mode == ROLE_AP) {
        ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_AP_TX_POWER, sizeof(ucPower),
                                   (unsigned char *)&ucPower));

        // configure all parameters
        wlan_set_ssid (ssid, ssid_len, add_mac);
        wlan_set_security (auth, key, key_len);
        wlan_set_channel (channel);

        // set the country
        _u8*  country = (_u8*)"EU";
        ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_COUNTRY_CODE, 2, country));

        SlNetCfgIpV4Args_t ipV4;
        ipV4.ipV4          = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 IP address
        ipV4.ipV4Mask      = (_u32)SL_IPV4_VAL(255,255,255,0);          // _u32 Subnet mask for this AP
        ipV4.ipV4Gateway   = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 Default gateway address
        ipV4.ipV4DnsServer = (_u32)SL_IPV4_VAL(192,168,1,1);            // _u32 DNS server address
        ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4,
                                     sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4));

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
    } else { // STA and P2P modes
        ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER,
                                   sizeof(ucPower), (unsigned char *)&ucPower));
        // set connection policy to Auto + Fast (tries to connect to the last connected AP)
        ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 1, 0, 0, 0), NULL, 0));
    }

    // set current time and date (needed to validate certificates)
    wlan_set_current_time (pyb_rtc_get_seconds());

    // start the servers before returning
    wlan_servers_start();
}

void wlan_update(void) {
#ifndef SL_PLATFORM_MULTI_THREADED
    _SlTaskEntry();
#endif
}

void wlan_stop (uint32_t timeout) {
    wlan_servers_stop();
    #ifdef SL_PLATFORM_MULTI_THREADED
    sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
    #endif
    sl_Stop(timeout);
    wlan_clear_data();
    wlan_obj.mode = -1;
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
    return (GET_STATUS_BIT(wlan_obj.status, STATUS_BIT_CONNECTION) &&
            (GET_STATUS_BIT(wlan_obj.status, STATUS_BIT_IP_ACQUIRED) || wlan_obj.mode != ROLE_STA));
}

void wlan_set_current_time (uint32_t seconds_since_2000) {
    timeutils_struct_time_t tm;
    timeutils_seconds_since_2000_to_struct_time(seconds_since_2000, &tm);

    SlDateTime_t sl_datetime = {0};
    sl_datetime.sl_tm_day  = tm.tm_mday;
    sl_datetime.sl_tm_mon  = tm.tm_mon;
    sl_datetime.sl_tm_year = tm.tm_year;
    sl_datetime.sl_tm_hour = tm.tm_hour;
    sl_datetime.sl_tm_min  = tm.tm_min;
    sl_datetime.sl_tm_sec  = tm.tm_sec;
    sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION, SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME, sizeof(SlDateTime_t), (_u8 *)(&sl_datetime));
}

void wlan_off_on (void) {
    // no need to lock the WLAN object on every API call since the servers and the MicroPtyhon
    // task have the same priority
    wlan_reenable(wlan_obj.mode);
}

//*****************************************************************************
// DEFINE STATIC FUNCTIONS
//*****************************************************************************

STATIC void wlan_clear_data (void) {
    CLR_STATUS_BIT_ALL(wlan_obj.status);
    wlan_obj.ip = 0;
    //memset(wlan_obj.ssid_o, 0, sizeof(wlan_obj.ssid));
    //memset(wlan_obj.bssid, 0, sizeof(wlan_obj.bssid));
}

STATIC void wlan_reenable (SlWlanMode_t mode) {
    // stop and start again
    #ifdef SL_PLATFORM_MULTI_THREADED
    sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
    #endif
    sl_Stop(SL_STOP_TIMEOUT);
    wlan_clear_data();
    wlan_obj.mode = sl_Start(0, 0, 0);
    #ifdef SL_PLATFORM_MULTI_THREADED
    sl_LockObjUnlock (&wlan_LockObj);
    #endif
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

STATIC void wlan_reset (void) {
    wlan_servers_stop();
    wlan_reenable (wlan_obj.mode);
    wlan_servers_start();
}

STATIC void wlan_validate_mode (uint mode) {
    if (mode != ROLE_STA && mode != ROLE_AP) {
        mp_raise_ValueError(mpexception_value_invalid_arguments);
    }
}

STATIC void wlan_set_mode (uint mode) {
    wlan_obj.mode = mode;
    ASSERT_ON_ERROR(sl_WlanSetMode(mode));
}

STATIC void wlan_validate_ssid_len (uint32_t len) {
    if (len > MODWLAN_SSID_LEN_MAX) {
        mp_raise_ValueError(mpexception_value_invalid_arguments);
    }
}

STATIC void wlan_set_ssid (const char *ssid, uint8_t len, bool add_mac) {
    if (ssid != NULL) {
        // save the ssid
        memcpy(&wlan_obj.ssid, ssid, len);
        // append the last 2 bytes of the MAC address, since the use of this functionality is under our control
        // we can assume that the lenght of the ssid is less than (32 - 5)
        if (add_mac) {
            snprintf((char *)&wlan_obj.ssid[len], sizeof(wlan_obj.ssid) - len, "-%02x%02x", wlan_obj.mac[4], wlan_obj.mac[5]);
            len += 5;
        }
        wlan_obj.ssid[len] = '\0';
        ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, len, (unsigned char *)wlan_obj.ssid));
    }
}

STATIC void wlan_validate_security (uint8_t auth, const char *key, uint8_t len) {
    if (auth != SL_SEC_TYPE_WEP && auth != SL_SEC_TYPE_WPA_WPA2) {
        goto invalid_args;
    }
    if (auth == SL_SEC_TYPE_WEP) {
        for (mp_uint_t i = strlen(key); i > 0; i--) {
            if (!unichar_isxdigit(*key++)) {
                goto invalid_args;
            }
        }
    }
    return;

invalid_args:
    mp_raise_ValueError(mpexception_value_invalid_arguments);
}

STATIC void wlan_set_security (uint8_t auth, const char *key, uint8_t len) {
    wlan_obj.auth = auth;
    ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SECURITY_TYPE, sizeof(uint8_t), &auth));
    if (key != NULL) {
        memcpy(&wlan_obj.key, key, len);
        wlan_obj.key[len] = '\0';
        if (auth == SL_SEC_TYPE_WEP) {
            _u8 wep_key[32];
            wlan_wep_key_unhexlify(key, (char *)&wep_key);
            key = (const char *)&wep_key;
            len /= 2;
        }
        ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_PASSWORD, len, (unsigned char *)key));
    } else {
        wlan_obj.key[0] = '\0';
    }
}

STATIC void wlan_validate_channel (uint8_t channel) {
    if (channel < 1 || channel > 11) {
        mp_raise_ValueError(mpexception_value_invalid_arguments);
    }
}

STATIC void wlan_set_channel (uint8_t channel) {
    wlan_obj.channel = channel;
    ASSERT_ON_ERROR(sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_CHANNEL, 1, &channel));
}

#if MICROPY_HW_ANTENNA_DIVERSITY
STATIC void wlan_validate_antenna (uint8_t antenna) {
    if (antenna != ANTENNA_TYPE_INTERNAL && antenna != ANTENNA_TYPE_EXTERNAL) {
        mp_raise_ValueError(mpexception_value_invalid_arguments);
    }
}

STATIC void wlan_set_antenna (uint8_t antenna) {
    wlan_obj.antenna = antenna;
    antenna_select(antenna);
}
#endif

STATIC void wlan_sl_disconnect (void) {
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore
    // other return-codes
    if (0 == sl_WlanDisconnect()) {
        while (IS_CONNECTED(wlan_obj.status)) {
            mp_hal_delay_ms(MODWLAN_CONNECTION_WAIT_MS);
            wlan_update();
        }
    }
}

STATIC modwlan_Status_t wlan_do_connect (const char* ssid, uint32_t ssid_len, const char* bssid, uint8_t sec,
                                         const char* key, uint32_t key_len, int32_t timeout) {
    SlSecParams_t secParams;
    secParams.Key = (_i8*)key;
    secParams.KeyLen = ((key != NULL) ? key_len : 0);
    secParams.Type = sec;

    // first close any active connections
    wlan_sl_disconnect();

    if (!sl_WlanConnect((_i8*)ssid, ssid_len, (_u8*)bssid, &secParams, NULL)) {
        // wait for the WLAN Event
        uint32_t waitForConnectionMs = 0;
        while (timeout && !IS_CONNECTED(wlan_obj.status)) {
            mp_hal_delay_ms(MODWLAN_CONNECTION_WAIT_MS);
            waitForConnectionMs += MODWLAN_CONNECTION_WAIT_MS;
            if (timeout > 0 && waitForConnectionMs > timeout) {
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

STATIC void wlan_wep_key_unhexlify (const char *key, char *key_out) {
    byte hex_byte = 0;
    for (mp_uint_t i = strlen(key); i > 0 ; i--) {
        hex_byte += unichar_xdigit_value(*key++);
        if (i & 1) {
            hex_byte <<= 4;
        } else {
            *key_out++ = hex_byte;
            hex_byte = 0;
        }
    }
}

STATIC void wlan_lpds_irq_enable (mp_obj_t self_in) {
    wlan_obj_t *self = self_in;
    self->irq_enabled = true;
}

STATIC void wlan_lpds_irq_disable (mp_obj_t self_in) {
    wlan_obj_t *self = self_in;
    self->irq_enabled = false;
}

STATIC int wlan_irq_flags (mp_obj_t self_in) {
    wlan_obj_t *self = self_in;
    return self->irq_flags;
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
// MicroPython bindings; WLAN class

/// \class WLAN - WiFi driver

STATIC mp_obj_t wlan_init_helper(wlan_obj_t *self, const mp_arg_val_t *args) {
    // get the mode
    int8_t mode = args[0].u_int;
    wlan_validate_mode(mode);

    // get the ssid
    size_t ssid_len = 0;
    const char *ssid = NULL;
    if (args[1].u_obj != NULL) {
        ssid = mp_obj_str_get_data(args[1].u_obj, &ssid_len);
        wlan_validate_ssid_len(ssid_len);
    }

    // get the auth config
    uint8_t auth = SL_SEC_TYPE_OPEN;
    size_t key_len = 0;
    const char *key = NULL;
    if (args[2].u_obj != mp_const_none) {
        mp_obj_t *sec;
        mp_obj_get_array_fixed_n(args[2].u_obj, 2, &sec);
        auth = mp_obj_get_int(sec[0]);
        key = mp_obj_str_get_data(sec[1], &key_len);
        wlan_validate_security(auth, key, key_len);
    }

    // get the channel
    uint8_t channel = args[3].u_int;
    wlan_validate_channel(channel);

    // get the antenna type
    uint8_t antenna = 0;
#if MICROPY_HW_ANTENNA_DIVERSITY
    antenna = args[4].u_int;
    wlan_validate_antenna(antenna);
#endif

    // initialize the wlan subsystem
    wlan_sl_init(mode, (const char *)ssid, ssid_len, auth, (const char *)key, key_len, channel, antenna, false);

    return mp_const_none;
}

STATIC const mp_arg_t wlan_init_args[] = {
    { MP_QSTR_id,                             MP_ARG_INT,  {.u_int = 0} },
    { MP_QSTR_mode,                           MP_ARG_INT,  {.u_int = ROLE_STA} },
    { MP_QSTR_ssid,         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_auth,         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    { MP_QSTR_channel,      MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 1} },
    #if MICROPY_HW_ANTENNA_DIVERSITY
    { MP_QSTR_antenna,      MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = ANTENNA_TYPE_INTERNAL} },
    #endif
};
STATIC mp_obj_t wlan_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(wlan_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), wlan_init_args, args);

    // setup the object
    wlan_obj_t *self = &wlan_obj;
    self->base.type = (mp_obj_t)&mod_network_nic_type_wlan;

    // give it to the sleep module
    pyb_sleep_set_wlan_obj(self);

    if (n_args > 1 || n_kw > 0) {
        // check the peripheral id
        if (args[0].u_int != 0) {
            mp_raise_OSError(MP_ENODEV);
        }
        // start the peripheral
        wlan_init_helper(self, &args[1]);
    }

    return (mp_obj_t)self;
}

STATIC mp_obj_t wlan_init(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(wlan_init_args) - 1];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), &wlan_init_args[1], args);
    return wlan_init_helper(pos_args[0], args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wlan_init_obj, 1, wlan_init);

STATIC mp_obj_t wlan_scan(mp_obj_t self_in) {
    STATIC const qstr wlan_scan_info_fields[] = {
        MP_QSTR_ssid, MP_QSTR_bssid, MP_QSTR_sec, MP_QSTR_channel, MP_QSTR_rssi
    };

    // check for correct wlan mode
    if (wlan_obj.mode == ROLE_AP) {
        mp_raise_OSError(MP_EPERM);
    }

    Sl_WlanNetworkEntry_t wlanEntry;
    mp_obj_t nets = mp_obj_new_list(0, NULL);
    uint8_t _index = 0;

    // trigger a new network scan
    uint32_t scanSeconds = MODWLAN_SCAN_PERIOD_S;
    ASSERT_ON_ERROR(sl_WlanPolicySet(SL_POLICY_SCAN , MODWLAN_SL_SCAN_ENABLE, (_u8 *)&scanSeconds, sizeof(scanSeconds)));

    // wait for the scan to complete
    mp_hal_delay_ms(MODWLAN_WAIT_FOR_SCAN_MS);

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

STATIC mp_obj_t wlan_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid,     MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_auth,                       MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid,    MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_timeout,  MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // check for the correct wlan mode
    if (wlan_obj.mode == ROLE_AP) {
        mp_raise_OSError(MP_EPERM);
    }

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the ssid
    size_t ssid_len;
    const char *ssid = mp_obj_str_get_data(args[0].u_obj, &ssid_len);
    wlan_validate_ssid_len(ssid_len);

    // get the auth config
    uint8_t auth = SL_SEC_TYPE_OPEN;
    size_t key_len = 0;
    const char *key = NULL;
    if (args[1].u_obj != mp_const_none) {
        mp_obj_t *sec;
        mp_obj_get_array_fixed_n(args[1].u_obj, 2, &sec);
        auth = mp_obj_get_int(sec[0]);
        key = mp_obj_str_get_data(sec[1], &key_len);
        wlan_validate_security(auth, key, key_len);

        // convert the wep key if needed
        if (auth == SL_SEC_TYPE_WEP) {
            _u8 wep_key[32];
            wlan_wep_key_unhexlify(key, (char *)&wep_key);
            key = (const char *)&wep_key;
            key_len /= 2;
        }
    }

    // get the bssid
    const char *bssid = NULL;
    if (args[2].u_obj != mp_const_none) {
        bssid = mp_obj_str_get_str(args[2].u_obj);
    }

    // get the timeout
    int32_t timeout = -1;
    if (args[3].u_obj != mp_const_none) {
        timeout = mp_obj_get_int(args[3].u_obj);
    }

    // connect to the requested access point
    modwlan_Status_t status;
    status = wlan_do_connect (ssid, ssid_len, bssid, auth, key, key_len, timeout);
    if (status == MODWLAN_ERROR_TIMEOUT) {
        mp_raise_OSError(MP_ETIMEDOUT);
    } else if (status == MODWLAN_ERROR_INVALID_PARAMS) {
        mp_raise_ValueError(mpexception_value_invalid_arguments);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wlan_connect_obj, 1, wlan_connect);

STATIC mp_obj_t wlan_disconnect(mp_obj_t self_in) {
    wlan_sl_disconnect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_disconnect_obj, wlan_disconnect);

STATIC mp_obj_t wlan_isconnected(mp_obj_t self_in) {
    return wlan_is_connected() ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_isconnected_obj, wlan_isconnected);

STATIC mp_obj_t wlan_ifconfig (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t wlan_ifconfig_args[] = {
        { MP_QSTR_id,               MP_ARG_INT,     {.u_int = 0} },
        { MP_QSTR_config,           MP_ARG_OBJ,     {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(wlan_ifconfig_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), wlan_ifconfig_args, args);

    // check the interface id
    if (args[0].u_int != 0) {
        mp_raise_OSError(MP_EPERM);
    }

    // get the configuration
    if (args[1].u_obj == MP_OBJ_NULL) {
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
    } else { // set the configuration
        if (MP_OBJ_IS_TYPE(args[1].u_obj, &mp_type_tuple)) {
            // set a static ip
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(args[1].u_obj, 4, &items);

            SlNetCfgIpV4Args_t ipV4;
            netutils_parse_ipv4_addr(items[0], (uint8_t *)&ipV4.ipV4, NETUTILS_LITTLE);
            netutils_parse_ipv4_addr(items[1], (uint8_t *)&ipV4.ipV4Mask, NETUTILS_LITTLE);
            netutils_parse_ipv4_addr(items[2], (uint8_t *)&ipV4.ipV4Gateway, NETUTILS_LITTLE);
            netutils_parse_ipv4_addr(items[3], (uint8_t *)&ipV4.ipV4DnsServer, NETUTILS_LITTLE);

            if (wlan_obj.mode == ROLE_AP) {
                ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_AP_P2P_GO_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4));
                SlNetAppDhcpServerBasicOpt_t dhcpParams;
                dhcpParams.lease_time      =  4096;                             // lease time (in seconds) of the IP Address
                dhcpParams.ipv4_addr_start =  ipV4.ipV4 + 1;                    // first IP Address for allocation.
                dhcpParams.ipv4_addr_last  =  (ipV4.ipV4 & 0xFFFFFF00) + 254;   // last IP Address for allocation.
                ASSERT_ON_ERROR(sl_NetAppStop(SL_NET_APP_DHCP_SERVER_ID));      // stop DHCP server before settings
                ASSERT_ON_ERROR(sl_NetAppSet(SL_NET_APP_DHCP_SERVER_ID, NETAPP_SET_DHCP_SRV_BASIC_OPT,
                                sizeof(SlNetAppDhcpServerBasicOpt_t), (_u8* )&dhcpParams));  // set parameters
                ASSERT_ON_ERROR(sl_NetAppStart(SL_NET_APP_DHCP_SERVER_ID));     // start DHCP server with new settings
            } else {
                ASSERT_ON_ERROR(sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4));
            }
        } else {
            // check for the correct string
            const char *mode = mp_obj_str_get_str(args[1].u_obj);
            if (strcmp("dhcp", mode)) {
                mp_raise_ValueError(mpexception_value_invalid_arguments);
            }

            // only if we are not in AP mode
            if (wlan_obj.mode != ROLE_AP) {
                _u8 val = 1;
                sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, 1, &val);
            }
        }
        // config values have changed, so reset
        wlan_reset();
        // set current time and date (needed to validate certificates)
        wlan_set_current_time (pyb_rtc_get_seconds());
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wlan_ifconfig_obj, 1, wlan_ifconfig);

STATIC mp_obj_t wlan_mode (mp_uint_t n_args, const mp_obj_t *args) {
    wlan_obj_t *self = args[0];
    if (n_args == 1) {
        return mp_obj_new_int(self->mode);
    } else {
        uint mode = mp_obj_get_int(args[1]);
        wlan_validate_mode(mode);
        wlan_set_mode(mode);
        wlan_reset();
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_mode_obj, 1, 2, wlan_mode);

STATIC mp_obj_t wlan_ssid (mp_uint_t n_args, const mp_obj_t *args) {
    wlan_obj_t *self = args[0];
    if (n_args == 1) {
        return mp_obj_new_str((const char *)self->ssid, strlen((const char *)self->ssid), false);
    } else {
        size_t len;
        const char *ssid = mp_obj_str_get_data(args[1], &len);
        wlan_validate_ssid_len(len);
        wlan_set_ssid(ssid, len, false);
        wlan_reset();
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_ssid_obj, 1, 2, wlan_ssid);

STATIC mp_obj_t wlan_auth (mp_uint_t n_args, const mp_obj_t *args) {
    wlan_obj_t *self = args[0];
    if (n_args == 1) {
        if (self->auth == SL_SEC_TYPE_OPEN) {
            return mp_const_none;
        } else {
            mp_obj_t security[2];
            security[0] = mp_obj_new_int(self->auth);
            security[1] = mp_obj_new_str((const char *)self->key, strlen((const char *)self->key), false);
            return mp_obj_new_tuple(2, security);
        }
    } else {
        // get the auth config
        uint8_t auth = SL_SEC_TYPE_OPEN;
        size_t key_len = 0;
        const char *key = NULL;
        if (args[1] != mp_const_none) {
            mp_obj_t *sec;
            mp_obj_get_array_fixed_n(args[1], 2, &sec);
            auth = mp_obj_get_int(sec[0]);
            key = mp_obj_str_get_data(sec[1], &key_len);
            wlan_validate_security(auth, key, key_len);
        }
        wlan_set_security(auth, key, key_len);
        wlan_reset();
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_auth_obj, 1, 2, wlan_auth);

STATIC mp_obj_t wlan_channel (mp_uint_t n_args, const mp_obj_t *args) {
    wlan_obj_t *self = args[0];
    if (n_args == 1) {
        return mp_obj_new_int(self->channel);
    } else {
        uint8_t channel  = mp_obj_get_int(args[1]);
        wlan_validate_channel(channel);
        wlan_set_channel(channel);
        wlan_reset();
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_channel_obj, 1, 2, wlan_channel);

STATIC mp_obj_t wlan_antenna (mp_uint_t n_args, const mp_obj_t *args) {
    wlan_obj_t *self = args[0];
    if (n_args == 1) {
        return mp_obj_new_int(self->antenna);
    } else {
    #if MICROPY_HW_ANTENNA_DIVERSITY
        uint8_t antenna  = mp_obj_get_int(args[1]);
        wlan_validate_antenna(antenna);
        wlan_set_antenna(antenna);
    #endif
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_antenna_obj, 1, 2, wlan_antenna);

STATIC mp_obj_t wlan_mac (mp_uint_t n_args, const mp_obj_t *args) {
    wlan_obj_t *self = args[0];
    if (n_args == 1) {
        return mp_obj_new_bytes((const byte *)self->mac, SL_BSSID_LENGTH);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
        if (bufinfo.len != 6) {
            mp_raise_ValueError(mpexception_value_invalid_arguments);
        }
        memcpy(self->mac, bufinfo.buf, SL_MAC_ADDR_LEN);
        sl_NetCfgSet(SL_MAC_ADDRESS_SET, 1, SL_MAC_ADDR_LEN, (_u8 *)self->mac);
        wlan_reset();
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_mac_obj, 1, 2, wlan_mac);

STATIC mp_obj_t wlan_irq (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[mp_irq_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, mp_irq_INIT_NUM_ARGS, mp_irq_init_args, args);

    wlan_obj_t *self = pos_args[0];

    // check the trigger, only one type is supported
    if (mp_obj_get_int(args[0].u_obj) != MODWLAN_WIFI_EVENT_ANY) {
        goto invalid_args;
    }

    // check the power mode
    if (mp_obj_get_int(args[3].u_obj) != PYB_PWR_MODE_LPDS) {
        goto invalid_args;
    }

    // create the callback
    mp_obj_t _irq = mp_irq_new (self, args[2].u_obj, &wlan_irq_methods);
    self->irq_obj = _irq;

    // enable the irq just before leaving
    wlan_lpds_irq_enable(self);

    return _irq;

invalid_args:
    mp_raise_ValueError(mpexception_value_invalid_arguments);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wlan_irq_obj, 1, wlan_irq);

//STATIC mp_obj_t wlan_connections (mp_obj_t self_in) {
//    mp_obj_t device[2];
//    mp_obj_t connections = mp_obj_new_list(0, NULL);
//
//    if (wlan_is_connected()) {
//        device[0] = mp_obj_new_str((const char *)wlan_obj.ssid_o, strlen((const char *)wlan_obj.ssid_o), false);
//        device[1] = mp_obj_new_bytes((const byte *)wlan_obj.bssid, SL_BSSID_LENGTH);
//        // add the device to the list
//        mp_obj_list_append(connections, mp_obj_new_tuple(MP_ARRAY_SIZE(device), device));
//    }
//    return connections;
//}
//STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_connections_obj, wlan_connections);

//STATIC mp_obj_t wlan_urn (uint n_args, const mp_obj_t *args) {
//    char urn[MAX_DEVICE_URN_LEN];
//    uint8_t len = MAX_DEVICE_URN_LEN;
//
//    // an URN is given, so set it
//    if (n_args == 2) {
//        const char *p = mp_obj_str_get_str(args[1]);
//        uint8_t len = strlen(p);
//
//        // the call to sl_NetAppSet corrupts the input string URN=args[1], so we copy into a local buffer
//        if (len > MAX_DEVICE_URN_LEN) {
//            mp_raise_ValueError(mpexception_value_invalid_arguments);
//        }
//        strcpy(urn, p);
//
//        if (sl_NetAppSet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, len, (unsigned char *)urn) < 0) {
//            mp_raise_OSError(MP_EIO);
//        }
//    }
//    else {
//        // get the URN
//        if (sl_NetAppGet(SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, &len, (uint8_t *)urn) < 0) {
//            mp_raise_OSError(MP_EIO);
//        }
//        return mp_obj_new_str(urn, (len - 1), false);
//    }
//
//    return mp_const_none;
//}
//STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_urn_obj, 1, 2, wlan_urn);

STATIC mp_obj_t wlan_print_ver(void) {
    SlVersionFull ver;
    byte config_opt = SL_DEVICE_GENERAL_VERSION;
    byte config_len = sizeof(ver);
    sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &config_opt, &config_len, (byte*)&ver);
    printf("NWP: %d.%d.%d.%d\n", (int)ver.NwpVersion[0], (int)ver.NwpVersion[1], (int)ver.NwpVersion[2], (int)ver.NwpVersion[3]);
    printf("MAC: %d.%d.%d.%d\n", (int)ver.ChipFwAndPhyVersion.FwVersion[0], (int)ver.ChipFwAndPhyVersion.FwVersion[1],
                                 (int)ver.ChipFwAndPhyVersion.FwVersion[2], (int)ver.ChipFwAndPhyVersion.FwVersion[3]);
    printf("PHY: %d.%d.%d.%d\n", ver.ChipFwAndPhyVersion.PhyVersion[0], ver.ChipFwAndPhyVersion.PhyVersion[1],
                                 ver.ChipFwAndPhyVersion.PhyVersion[2], ver.ChipFwAndPhyVersion.PhyVersion[3]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(wlan_print_ver_fun_obj, wlan_print_ver);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(wlan_print_ver_obj, MP_ROM_PTR(&wlan_print_ver_fun_obj));

STATIC const mp_rom_map_elem_t wlan_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),                MP_ROM_PTR(&wlan_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan),                MP_ROM_PTR(&wlan_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect),             MP_ROM_PTR(&wlan_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),          MP_ROM_PTR(&wlan_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected),         MP_ROM_PTR(&wlan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig),            MP_ROM_PTR(&wlan_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode),                MP_ROM_PTR(&wlan_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_ssid),                MP_ROM_PTR(&wlan_ssid_obj) },
    { MP_ROM_QSTR(MP_QSTR_auth),                MP_ROM_PTR(&wlan_auth_obj) },
    { MP_ROM_QSTR(MP_QSTR_channel),             MP_ROM_PTR(&wlan_channel_obj) },
    { MP_ROM_QSTR(MP_QSTR_antenna),             MP_ROM_PTR(&wlan_antenna_obj) },
    { MP_ROM_QSTR(MP_QSTR_mac),                 MP_ROM_PTR(&wlan_mac_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),                 MP_ROM_PTR(&wlan_irq_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_connections),         MP_ROM_PTR(&wlan_connections_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_urn),                 MP_ROM_PTR(&wlan_urn_obj) },
    { MP_ROM_QSTR(MP_QSTR_print_ver),           MP_ROM_PTR(&wlan_print_ver_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_STA),                 MP_ROM_INT(ROLE_STA) },
    { MP_ROM_QSTR(MP_QSTR_AP),                  MP_ROM_INT(ROLE_AP) },
    { MP_ROM_QSTR(MP_QSTR_WEP),                 MP_ROM_INT(SL_SEC_TYPE_WEP) },
    { MP_ROM_QSTR(MP_QSTR_WPA),                 MP_ROM_INT(SL_SEC_TYPE_WPA_WPA2) },
    { MP_ROM_QSTR(MP_QSTR_WPA2),                MP_ROM_INT(SL_SEC_TYPE_WPA_WPA2) },
    #if MICROPY_HW_ANTENNA_DIVERSITY
    { MP_ROM_QSTR(MP_QSTR_INT_ANT),             MP_ROM_INT(ANTENNA_TYPE_INTERNAL) },
    { MP_ROM_QSTR(MP_QSTR_EXT_ANT),             MP_ROM_INT(ANTENNA_TYPE_EXTERNAL) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_ANY_EVENT),           MP_ROM_INT(MODWLAN_WIFI_EVENT_ANY) },
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

STATIC const mp_irq_methods_t wlan_irq_methods = {
    .init = wlan_irq,
    .enable = wlan_lpds_irq_enable,
    .disable = wlan_lpds_irq_disable,
    .flags = wlan_irq_flags,
};
