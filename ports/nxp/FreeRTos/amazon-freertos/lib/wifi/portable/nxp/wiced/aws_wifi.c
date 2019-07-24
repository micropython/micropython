/*
 * Amazon FreeRTOS Wi-Fi for LPC54018 IoT Module V1.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_WIFI.c
 * @brief Wi-Fi Interface.
 */

#include "lwip/ip_addr.h"


#include <stdio.h>
#include "FreeRTOS.h"

#include "aws_wifi.h"
#include "wwd.h"
#include "wwd_wiced.h"
#include "wwd_network.h"
#include "wwd_constants.h"
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/prot/dhcp.h"


#ifndef WIFI_CONNECT_TIMEOUT_MS
#define WIFI_CONNECT_TIMEOUT_MS         (10000)
#endif

#ifndef WIFI_DHCP_DELAY_MS
#define WIFI_DHCP_DELAY_MS              (500)
#endif

#ifndef WIFI_DNS_TIMEOUT_MS
#define WIFI_DNS_TIMEOUT_MS             (10000)
#endif

#ifndef WIFI_SCAN_TIMEOUT_MS
#define WIFI_SCAN_TIMEOUT_MS            (10000)
#endif

typedef struct _wiced_scan_buf
{
    WIFIScanResult_t    *pxBuffer;
    uint8_t             uxNumNetworks;
    uint16_t            result_buff_num;
} wiced_scan_buf;

extern wiced_result_t wiced_wlan_connectivity_init( void );
extern void etharp_cleanup_netif(struct netif *netif);
extern err_t dns_gethostbyname(const char *hostname, ip_addr_t *addr, dns_found_callback found, void *callback_arg);

/* Protect API */
static SemaphoreHandle_t g_api_mutex = NULL;
static SemaphoreHandle_t g_api_sema = NULL;

/* DNS result IP */
static ip_addr_t g_host_ip = {0};

/* WiFi interface is on */
static volatile uint8_t g_wifi_on = 0;

/* Initialize lwIP only once */
static volatile uint8_t g_lwip_is_ready = 0;

/* WiFi is connected to AP, mutex protected write */
static volatile uint8_t g_ap_connected = 0;

/* IP addresses */
static ip4_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;

/* Network fface */
static struct netif fsl_netif0;

/* Temporary SSID */
static wiced_ssid_t ssid = {0};


static int wifi_is_enabled(void)
{
    return g_wifi_on;
}

static void wifi_set_enabled(int value)
{
    g_wifi_on = !!value;
}

static int wifi_is_connected(void)
{
    return g_wifi_on && g_ap_connected;
}

static void wifi_set_connected(int value)
{
    g_ap_connected = !!value;
}

static wiced_security_t conv_security_to_wiced(WIFISecurity_t api_sec)
{
    switch (api_sec)
    {
        case eWiFiSecurityOpen:
            return WICED_SECURITY_OPEN;
        case eWiFiSecurityWEP:
            return WICED_SECURITY_WEP_PSK;
        case eWiFiSecurityWPA:
            return WICED_SECURITY_WPA_MIXED_PSK;
        case eWiFiSecurityWPA2:
            return WICED_SECURITY_WPA2_MIXED_PSK;
        default:
            return WICED_SECURITY_UNKNOWN;
    }
}


static WIFISecurity_t conv_security_from_wiced(wiced_security_t api_sec)
{
    switch (api_sec)
    {
        case WICED_SECURITY_OPEN:
            return eWiFiSecurityOpen;
        case WICED_SECURITY_WEP_PSK:
            return eWiFiSecurityWEP;
        case WICED_SECURITY_WPA_MIXED_PSK:
            return eWiFiSecurityWPA;
        case WICED_SECURITY_WPA2_MIXED_PSK:
            return eWiFiSecurityWPA2;
        default:
            return eWiFiSecurityNotSupported;
    }
}


void wiced_scan_results_handler( wiced_scan_result_t** result_ptr, void* user_data, wiced_scan_status_t status )
{
    wiced_scan_buf *scan_buf = (wiced_scan_buf *) user_data;

    if ( result_ptr == NULL )
    {
        /* finished */
        scan_buf->pxBuffer[scan_buf->result_buff_num].cChannel = 0xff;// TODO: check
        xSemaphoreGive(g_api_sema);
        WPRINT_WWD_INFO(("scan completed\r\n"));
        return;
    }

    if ( scan_buf->result_buff_num >= scan_buf->uxNumNetworks )
    {
        WPRINT_WWD_INFO(("scan result overflow %d vs %d \r\n", scan_buf->result_buff_num, scan_buf->uxNumNetworks));
        return;
    }

    strncpy((char*)scan_buf->pxBuffer[scan_buf->result_buff_num].cSSID, (char const*)result_ptr[0]->SSID.value, wificonfigMAX_SSID_LEN);
    memcpy((char*)scan_buf->pxBuffer[scan_buf->result_buff_num].ucBSSID, (char const*)result_ptr[0]->BSSID.octet, wificonfigMAX_BSSID_LEN);

    scan_buf->pxBuffer[scan_buf->result_buff_num].cRSSI = result_ptr[0]->signal_strength;
    scan_buf->pxBuffer[scan_buf->result_buff_num].cChannel = result_ptr[0]->channel;
    scan_buf->pxBuffer[scan_buf->result_buff_num].ucHidden = 0;
    scan_buf->pxBuffer[scan_buf->result_buff_num].xSecurity = conv_security_from_wiced(result_ptr[0]->security);

    scan_buf->result_buff_num++;
}


static void iface_up(void)
{
    IP4_ADDR(&fsl_netif0_ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&fsl_netif0_netmask, 0, 0, 0, 0);
    IP4_ADDR(&fsl_netif0_gw, 0, 0, 0, 0);

    netifapi_netif_add(&fsl_netif0, &fsl_netif0_ipaddr, &fsl_netif0_netmask, &fsl_netif0_gw, (void *)WWD_STA_INTERFACE,
              wlanif_init, tcpip_input);

    netifapi_netif_set_default(&fsl_netif0);
    netifapi_netif_set_up(&fsl_netif0);
}


static void iface_down(void)
{
    netifapi_netif_set_down(&fsl_netif0);
    netifapi_netif_remove(&fsl_netif0);
}


/**
 * @brief Initializes the Wi-Fi module.
 *
 * This function must be called exactly once before any other
 * Wi-Fi functions (including socket functions) can be used.
 *
 * @return eWiFiSuccess if everything succeeds, eWiFiFailure otherwise.
 */
WIFIReturnCode_t WIFI_On( void )
{
    /* NOTE: this function is thread unsave,
     * caller is repsonsible for proper protection */
    wwd_result_t result;

    /* Expects WiFi disabled */
    if (wifi_is_enabled())
        return eWiFiFailure;

    /* API mutex must not exist */
    if (NULL != g_api_mutex)
        return eWiFiFailure;

    /* API sema must not exist */
    assert(!(NULL != g_api_sema));
    if (NULL != g_api_sema)
        return eWiFiFailure;

    do {
        /* Create API mutex */
        g_api_mutex = xSemaphoreCreateBinary();
        assert(!(NULL == g_api_mutex));
        if (NULL == g_api_mutex)
            break;
        xSemaphoreGive(g_api_mutex);

        /* Create API sema */
        g_api_sema = xSemaphoreCreateCounting((unsigned portBASE_TYPE)0x7fffffff, (unsigned portBASE_TYPE)0);
        assert(!(NULL == g_api_sema));
        if (NULL == g_api_sema)
            break;

        /* Initialize TCP stack only once */
        if (0 == g_lwip_is_ready)
        {
            /* Initialize TCP/IP */
            tcpip_init( NULL, NULL );

            /* Initialize WICED */
            result = (wwd_result_t) wiced_wlan_connectivity_init();
            assert(!(result != WWD_SUCCESS));
            if (result != WWD_SUCCESS)
                break;

            g_lwip_is_ready = 1;
        }

        /* Enable default iface */
        iface_up();

        wifi_set_enabled(1);
        return eWiFiSuccess;
    } while (0);

    /* Cleanup API mutex */
    if (NULL != g_api_mutex)
    {
        vSemaphoreDelete(g_api_mutex);
        g_api_mutex = NULL;
    }

    /* Cleanup API sema */
    if (NULL != g_api_sema)
    {
        vSemaphoreDelete(g_api_sema);
        g_api_sema = NULL;
    }
    if (wifi_is_enabled())
    {
        iface_down();
        wifi_set_enabled(0);
    }

    return eWiFiFailure;
}


WIFIReturnCode_t WIFI_Off( void )
{
    /* NOTE: this function is thread unsave,
     * caller is repsonsible for proper protection */

    /* Expects WiFi enabled */
    if (!wifi_is_enabled())
        return eWiFiFailure;

    /* Disconnect from AP */
    WIFI_Disconnect();

    iface_down();
    wifi_set_enabled(0);

    /* Cleanup API mutex, expects mutex initialized */
    assert(NULL != g_api_mutex);
    if (NULL != g_api_mutex)
    {
        vSemaphoreDelete(g_api_mutex);
        g_api_mutex = NULL;
    }

    /* Cleanup API sema, expects sema initialized */
    assert(NULL != g_api_sema);
    if (NULL != g_api_sema)
    {
        vSemaphoreDelete(g_api_sema);
        g_api_sema = NULL;
    }

    return eWiFiSuccess;
}


WIFIReturnCode_t WIFI_DisconnectInternal(void)
{
      /* NOTE: this function is thread unsave,
       * caller is repsonsible for proper protection */
      err_t lwip_result = 0;
      WIFIReturnCode_t result = eWiFiSuccess;
      wwd_result_t wwd_status;

      /* Release IP and stop DHCP client */
      lwip_result = netifapi_dhcp_release(&fsl_netif0);
      assert(!(ERR_OK != lwip_result));
      if (ERR_OK != lwip_result)
          result = eWiFiFailure;
      netifapi_dhcp_stop(&fsl_netif0);

      /* Cleanup ARP cache */
      LOCK_TCPIP_CORE();
      etharp_cleanup_netif(&fsl_netif0);
      UNLOCK_TCPIP_CORE();

      /* Disconnect from AP */
      wwd_status = wwd_wifi_leave(WWD_STA_INTERFACE);
      assert(!(WWD_SUCCESS != wwd_status));
      if (WWD_SUCCESS != wwd_status)
          result = eWiFiFailure;

      wifi_set_connected(0);
      return result;
}


WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    wwd_result_t wwd_result = WWD_SUCCESS;
    WIFIReturnCode_t result = eWiFiFailure;
    wiced_security_t auth_mode = WICED_SECURITY_UNKNOWN;
    struct dhcp *pdhcp = NULL;

    /* Default values, used for WICED_SECURITY_OPEN */
    char const *t_pass = NULL;
    uint8_t t_pass_len = 0;

    /* Check params */
    if (NULL == pxNetworkParams || NULL == pxNetworkParams->pcSSID)
        return eWiFiFailure;

    /* Check supported auth */
    auth_mode = conv_security_to_wiced(pxNetworkParams->xSecurity);
    if (WICED_SECURITY_UNKNOWN == auth_mode)
        return eWiFiFailure;

    /* Check password ptr for auth != open */
    if (WICED_SECURITY_OPEN != auth_mode && NULL == pxNetworkParams->pcPassword)
        return eWiFiFailure;

    /* WiFi is not turned ON ! */
    if (!wifi_is_enabled())
        return eWiFiFailure;

    if (xSemaphoreTake(g_api_mutex, pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS)) == pdTRUE)
    {
        do {
            /* Disconnect if connected */
            if (wifi_is_connected())
            {
                WIFI_DisconnectInternal(); //TODO: check
            }

            /* Use pass, pass_length from args */
            if (WICED_SECURITY_OPEN != auth_mode)
            {
                t_pass = pxNetworkParams->pcPassword;
                t_pass_len = pxNetworkParams->ucPasswordLength;
            }

            /* Set SSID */
            //ssid.length = pxNetworkParams->ucSSIDLength;
            ssid.length = strlen(pxNetworkParams->pcSSID);
            if (ssid.length > SSID_NAME_SIZE)
            {
                break;
            }
            memcpy((char*)ssid.value, pxNetworkParams->pcSSID, ssid.length);

            /* Try to join network */
            wwd_result = wwd_wifi_join( &ssid, auth_mode, (uint8_t const *)t_pass, t_pass_len, NULL, WWD_STA_INTERFACE);
            if ( WWD_SUCCESS != wwd_result )
                break;

            /* Launch DHCP client and wait some time to get result */
            dhcp_start(&fsl_netif0);
            pdhcp = (struct dhcp *)netif_get_client_data(&fsl_netif0, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
            assert( NULL != pdhcp);
            for (int i = 10; i > 0 && pdhcp->state != DHCP_STATE_BOUND; i--)
            {
                vTaskDelay(pdMS_TO_TICKS(WIFI_DHCP_DELAY_MS));
            }

            /* Still no response from DHCP server, report an error */
            if (pdhcp->state != DHCP_STATE_BOUND)
            {
                break;
            }

            /* Consider connected */
            wifi_set_connected(1);
            result = eWiFiSuccess;
        } while(0);
        /* Release semaphore */
        xSemaphoreGive(g_api_mutex);
    }

    return result;
}

/**
 * @brief Disconnects from Access Point.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Disconnect( void )
{
    WIFIReturnCode_t result = eWiFiFailure;

    /* WiFi not turned ON */
    if (!wifi_is_enabled())
        return eWiFiFailure;

    if (xSemaphoreTake(g_api_mutex, portMAX_DELAY) == pdTRUE)
    {
        /* If connected - disconnect */
        if (wifi_is_connected())
        {
            result = WIFI_DisconnectInternal();
        }
        else
        {
            result = eWiFiSuccess;
        }

        /* Release semaphore */
        xSemaphoreGive(g_api_mutex);
    }

    return result;
}

/**
 * @brief Resets the Wi-Fi Module.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Reset( void )
{
    return eWiFiNotSupported;
}

/**
 * @brief Sets Wi-Fi mode.
 *
 * @param[in] xDeviceMode - Mode of the device Station / Access Point /P2P.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    return eWiFiNotSupported;
}

/**
 * @brief Gets Wi-Fi mode.
 *
 * @param[in] pxDeviceMode - return mode Station / Access Point /P2P
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    return eWiFiNotSupported;
}

/**
 * @brief Wi-Fi Add Network profile.
 *
 * Adds Wi-Fi network to network list in non-volatile memory
 *
 * @param[in] pxNetworkProfile - network profile parameters
 * @param[out] pusIndex - network profile index
 *
 * @return Profile stored index on success, or negative error code on failure..
 *
 * @see WIFINetworkParams_t
 */
WIFIReturnCode_t WIFI_NetworkAdd(const WIFINetworkProfile_t * const pxNetworkProfile, uint16_t * pusIndex )
{
    return eWiFiNotSupported;
}


/**
 * @brief Wi-Fi Get Network profile .
 *
 * Gets Wi-Fi network params at given index from network list in non-volatile memory
 *
 * @param[out] pxNetworkProfile - pointer to return network profile parameters
 * @param[in] usIndex - Index of the network profile, must be between 0 to wificonfigMAX_NETWORK_PROFILES
 * @return eWiFiSuccess if success, eWiFiFailure otherwise.
 *
 * @see WIFINetworkParams_t
 */
WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                            uint16_t uxIndex )
{
    return eWiFiNotSupported;
}

/**
 * @brief Wi-Fi Delete Network profile .
 *
 * Deletes Wi-Fi network from network list at given index in non-volatile memory
 *
 * @param[in] usIndex - Index of the network profile, must be between 0 to wificonfigMAX_NETWORK_PROFILES
 *                      wificonfigMAX_NETWORK_PROFILES as index will delete all network profiles
 *
 * @return eWiFiSuccess if deleted, eWiFiFailure otherwise.
 *
 */
WIFIReturnCode_t WIFI_NetworkDelete( uint16_t uxIndex )
{
    return eWiFiNotSupported;
}

/**
 * @brief Ping an IP address in the network.
 *
 * @param[in] IP Address to ping.
 * @param[in] Number of times to ping
 * @param[in] Interval in mili seconds for ping operation
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Ping( uint8_t * pxIPAddr,
                                       uint16_t xCount,
                                       uint32_t xIntervalMS )
{
    /* Check connection */
    if (!wifi_is_connected())
        return eWiFiFailure;

    return eWiFiNotSupported;
}

/**
 * @brief Retrieves the Wi-Fi interface's IP address.
 *
 * @param[in] IP Address buffer.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetIP( uint8_t * pxIPAddr )
{
    if (NULL == pxIPAddr)
        return eWiFiFailure;

    /* Check connection */
    if (!wifi_is_connected())
        return eWiFiFailure;

    //TODO: semaphore ???
    memcpy((char*)pxIPAddr, (const char *)&fsl_netif0.ip_addr.addr, sizeof(fsl_netif0.ip_addr.addr));

    return eWiFiSuccess;
}

/**
 * @brief Retrieves the Wi-Fi interface's MAC address.
 *
 * @param[in] MAC Address buffer.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetMAC( uint8_t * pxMac )
{
    wiced_mac_t mac = {0};

    /* Check params */
    if (NULL == pxMac)
        return eWiFiFailure;

    /*TODO: ?? */
    if (!wifi_is_enabled())
        return eWiFiFailure;

    if (WWD_SUCCESS != wwd_wifi_get_mac_address(&mac, WWD_STA_INTERFACE))
        return eWiFiFailure;

    strncpy((char*)pxMac, (const char*)mac.octet, sizeof(mac.octet));
    return eWiFiSuccess;
}


void smtp_serverFound(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    assert(NULL != g_api_sema);
    if (NULL != ipaddr && 0 != ipaddr->addr)
    {
        ip4_addr_copy(g_host_ip, *ipaddr);
        xSemaphoreGive(g_api_sema);
    }
}


/**
 * @brief Retrieves host IP address from URL using DNS
 *
 * @param[in] pxHost - Host URL.
 * @param[in] pxIPAddr - IP Address buffer.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetHostIP( char * pxHost,
                                            uint8_t * pxIPAddr )
{
    WIFIReturnCode_t result = eWiFiFailure;
    err_t status = ERR_OK;

    /* Check params */
    if (NULL == pxHost || NULL == pxIPAddr)
        return eWiFiFailure;

    /* Check connection */
    if (!wifi_is_connected())
        return eWiFiFailure;

    if (xSemaphoreTake(g_api_mutex, pdMS_TO_TICKS(WIFI_DNS_TIMEOUT_MS)) == pdTRUE)
    {
        /* Cleanup semaphore */
        xQueueReset((void*)g_api_sema);
        status = dns_gethostbyname(pxHost, &g_host_ip, smtp_serverFound, NULL);
        if (ERR_OK == status)
        {
            pxIPAddr[0] = ip4_addr1(&g_host_ip);
            pxIPAddr[1] = ip4_addr2(&g_host_ip);
            pxIPAddr[2] = ip4_addr3(&g_host_ip);
            pxIPAddr[3] = ip4_addr4(&g_host_ip);
            g_host_ip = (ip_addr_t){0};
            result = eWiFiSuccess;
        }
        else if (ERR_INPROGRESS  == status)
        {
            if (xSemaphoreTake(g_api_sema, pdMS_TO_TICKS(WIFI_DNS_TIMEOUT_MS)))
            {
                pxIPAddr[0] = ip4_addr1(&g_host_ip);
                pxIPAddr[1] = ip4_addr2(&g_host_ip);
                pxIPAddr[2] = ip4_addr3(&g_host_ip);
                pxIPAddr[3] = ip4_addr4(&g_host_ip);
                g_host_ip = (ip_addr_t){0};
                result = eWiFiSuccess;
            }
        }
        xSemaphoreGive(g_api_mutex);
    }

    return result;
}

/**
 * @brief Retrieves IP address in Access Point mode
 *
 * @param[in] pxIPAddr - IP Address buffer.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetAccessPointIP( uint8_t * pxIPAddr )
{
    return eWiFiNotSupported;
}

/**
 * @brief Perform WiF Scan
 *
 * @param[in] pxBuffer - Buffer for scan results.
 * @param[in] uxNumNetworks - Number of networks in scan result.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                                       uint8_t uxNumNetworks )
{
    WIFIReturnCode_t result = eWiFiFailure;
    wwd_result_t wwd_result;
    wiced_scan_buf scan_buf;
    static wiced_scan_result_t* result_ptr = NULL;

    /* Check param */
    if (NULL == pxBuffer)
        return eWiFiFailure;

    /* Check connection */
    if (!wifi_is_enabled())
        return eWiFiFailure;

    if (xSemaphoreTake(g_api_mutex, pdMS_TO_TICKS(WIFI_SCAN_TIMEOUT_MS)) == pdTRUE)
    {
        do {
            scan_buf.pxBuffer = pxBuffer;
            scan_buf.uxNumNetworks = uxNumNetworks;
            scan_buf.result_buff_num = 0;

            /* Cleanup semaphore */
            xQueueReset((void*)g_api_sema);

            /* Expects success, but it is not critical */
            wwd_result = wwd_wifi_set_scan_suppress(WICED_FALSE);
            assert(WWD_SUCCESS == wwd_result);

            wwd_result = wwd_wifi_scan( WICED_SCAN_TYPE_ACTIVE, WICED_BSS_TYPE_ANY,
                NULL, NULL, NULL, NULL, wiced_scan_results_handler,
                (wiced_scan_result_t **) &result_ptr, &scan_buf, WWD_STA_INTERFACE);
            if (WWD_SUCCESS != wwd_result)
                break;

            if (xSemaphoreTake(g_api_sema, pdMS_TO_TICKS(WIFI_SCAN_TIMEOUT_MS)))
            {
                result = eWiFiSuccess;
            }
        } while(0);
        xSemaphoreGive(g_api_mutex);
    }

    return result;
}

/**
 * @brief Configure SoftAP
 *
 * @param[in] pxNetworkParams - Network params to configure AP.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_ConfigureAP(const WIFINetworkParams_t * const pxNetworkParams )
{
    return eWiFiNotSupported;
}

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    return eWiFiNotSupported;
}

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    return eWiFiNotSupported;
}

BaseType_t WIFI_IsConnected( void )
{
    //TODO: assert
    return wwd_wifi_sta_is_only_connected() == WICED_TRUE;
}


