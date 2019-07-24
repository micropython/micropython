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

#include <stdio.h>
#include "FreeRTOS.h"
#include "aws_wifi.h"
#include "wifi_common.h"
#include "qcom_api.h"
#include "atheros_wifi.h"

/* This is here because the maximum DNS name length is defined in aws_secure_sockets.h.
 * Wi-Fi must not have a dependency on aws_secure_sockets.h
 */
#define wifiMAX_DNS_NAME_LENGTH 253

/* Only 1 Wi-Fi module is present at the time */
static uint8_t g_devid = 0;

/* NOTE: Could be located on stack */
static QCOM_SSID g_ssid = {0};

/* NOTE: Could be located on stack */
static QCOM_PASSPHRASE g_passphr = {0};

/* Semaphore for connection */
static SemaphoreHandle_t g_connect_semaph;

/* Semaphore for dhcp response */
static SemaphoreHandle_t g_dhcp_semaph;

/* Protect API */
static SemaphoreHandle_t g_wifi_semaph;

/* Wi-Fi/HW hardware configuration */
QCA_CONTEXT_STRUCT g_wifi_ctx = {0};

/* Wi-Fi status - turned on */
static uint8_t g_wifi_is_on = 0;

/* Wi-Fi status - turned on */
static uint8_t g_connected = 0;

/* What event is expected */
enum expected_event {
    expected_event_default,
    expected_event_connect,
    expected_event_disconnect,
};

enum expected_event g_expected_event = expected_event_default;

/* Wi-Fi security */
static WIFISecurity_t g_security = eWiFiSecurityNotSupported;

extern const QCA_MAC_IF_STRUCT ATHEROS_WIFI_IF;

/* Wi-Fi interface object */
const QCA_IF_STRUCT g_wifi_if = {
    .MAC_IF         = &ATHEROS_WIFI_IF,
    .MAC_NUMBER     = 0,
    .PHY_NUMBER     = 0,
    .PHY_ADDRESS    = 0,
};

/* Wi-Fi params */
const QCA_PARAM_STRUCT g_wifi_params = {
    .QCA_IF         = &g_wifi_if,
    .MODE           = Auto_Negotiate,
    .OPTIONS        = 0,
    .NUM_RX_PCBS    = WLAN_CONFIG_NUM_PRE_ALLOC_RX_BUFFERS,
};

/* Singleton, provides Wi-Fi context structure */
QCA_CONTEXT_STRUCT *wlan_get_context(void)
{
    return &g_wifi_ctx;
}

static void ip_to_pxIPAddr(uint32_t ip, uint8_t *pxIPAddr)
{
    pxIPAddr[0] = (uint8_t)(ip >> 24);
    pxIPAddr[1] = (uint8_t)(ip >> 16);
    pxIPAddr[2] = (uint8_t)(ip >> 8);
    pxIPAddr[3] = (uint8_t)(ip);
}

static void pxIPAddr_to_ip(uint8_t *pxIPAddr, uint32_t *ip32)
{
    *ip32 = (((pxIPAddr[0]) & 0xFF) << 24) | \
            (((pxIPAddr[1]) & 0xFF) << 16) | \
            (((pxIPAddr[2]) & 0xFF) << 8) | \
            (((pxIPAddr[3]) & 0xFF));
}

static BaseType_t post_semaph(SemaphoreHandle_t semaph)
{
    BaseType_t result = pdFALSE;
    result = xSemaphoreGive(semaph);
//    assert(pdPASS == result);
    return result;
}

/* Invoked from 'driver_task', on SUCCESS post semaphore */
static void aws_connect_cb(QCOM_ONCONNECT_EVENT event, uint8_t devid, QCOM_BSSID bssid, boolean bss_conn)
{
    if ((g_security == eWiFiSecurityWPA) || (g_security == eWiFiSecurityWPA2))
    {
        /* 4 -way handshake completed */
        if (0x10 == (int)event)
        {
            g_connected = 1;
            post_semaph(g_connect_semaph);
        }
        /* invalid profile, auth failed */
        else if (0xa == (int)event)
        {
            g_connected = 0;
            post_semaph(g_connect_semaph);
        }
        else if (QCOM_ONCONNECT_EVENT_SUCCESS == event)
        {
            /* Do nothing, wait for 4-way handshake */
        }
        else if (QCOM_ONCONNECT_EVENT_DISCONNECT == event)
        {
            g_connected = 0;
            
            /* Avoid situation when receive disconnect followed by connect */
            if (expected_event_disconnect == g_expected_event)
            {
                post_semaph(g_connect_semaph);
            }
        }
        else {
            assert(0);
        }
    }
    else
    {
        if (QCOM_ONCONNECT_EVENT_SUCCESS == event)
        {
            g_connected = 1;
            post_semaph(g_connect_semaph);
        }
        else if (QCOM_ONCONNECT_EVENT_DISCONNECT == event)
        {
            g_connected = 0;
            /* Avoid situation when receive disconnect followed by connect */
            if (expected_event_disconnect == g_expected_event)
            {
                post_semaph(g_connect_semaph);
            }
        }
        else
        {
            assert(0);
        }
    }
}

/* Current IP settings */
static uint32_t g_ip4_addr = 0;
static uint32_t g_ip4_mask = 0;
static uint32_t g_ip4_gw = 0;

static inline uint32_t IPs_are_valid(uint32_t ip4_addr, uint32_t ip4_mask, uint32_t ip4_gw)
{
    uint32_t bitval = 0, expected_bitval = 1;

    /* not valid */
    if (0 == ip4_mask)
        return 0;

    /* Check continuous 1 and 0 in BE format */
    for (int8_t i = 31; i >= 0; i--)
    {
        bitval = (ip4_mask >> i) & 1;
        if (0 == expected_bitval && 1 == bitval)
        {
            /* not valid */
            return 0;
        }
        expected_bitval = bitval;
    }

    /* Not valid ip against gw */
    return (ip4_addr != 0 && ((ip4_addr & ip4_mask) == (ip4_gw & ip4_mask)));
}

/* Receive DHCP response */
static uint32_t aws_dhcpc_callback(uint32_t ip, uint32_t mask, uint32_t gw)
{
    BaseType_t result = pdFALSE;
    (void)result;
    /* Unify to BE format */
    g_ip4_addr = A_CPU2BE32(ip);
    g_ip4_mask = A_CPU2BE32(mask);
    g_ip4_gw = A_CPU2BE32(gw);
    result = xSemaphoreGive(g_dhcp_semaph);
//    assert(pdPASS == result);
    return 0;
}

static WIFIReturnCode_t conv_security_to_qcom(WIFISecurity_t api_sec, WLAN_AUTH_MODE *qcom_auth, WLAN_CRYPT_TYPE *qcom_crypt)
{
    switch (api_sec)
    {
        case eWiFiSecurityOpen:
            *qcom_crypt = WLAN_CRYPT_NONE;
            *qcom_auth = WLAN_AUTH_NONE;
            break;
        case eWiFiSecurityWEP:
            *qcom_crypt = WLAN_CRYPT_WEP_CRYPT;
            *qcom_auth = WLAN_AUTH_WEP;
            break;
        case eWiFiSecurityWPA:
            // *qcom_crypt = WLAN_CRYPT_TKIP_CRYPT;
            *qcom_crypt = WLAN_CRYPT_AES_CRYPT;
            *qcom_auth = WLAN_AUTH_WPA_PSK;
            break;
        case eWiFiSecurityWPA2:
            // *qcom_crypt = WLAN_CRYPT_TKIP_CRYPT;
            *qcom_crypt = WLAN_CRYPT_AES_CRYPT;
            *qcom_auth = WLAN_AUTH_WPA2_PSK;
            break;
        default:
            return eWiFiFailure;
    }
    return eWiFiSuccess;
}

static WIFIReturnCode_t conv_qcom_to_mode(QCOM_WLAN_DEV_MODE dev_mode, WIFIDeviceMode_t *pxDeviceMode)
{
    switch (dev_mode)
    {
        case QCOM_WLAN_DEV_MODE_STATION:
            *pxDeviceMode = eWiFiModeStation;
            break;
        case QCOM_WLAN_DEV_MODE_AP:
            *pxDeviceMode = eWiFiModeAP;
            break;
        case QCOM_WLAN_DEV_MODE_ADHOC:
            *pxDeviceMode = eWiFiModeP2P;
            break;
        default:
            return eWiFiFailure;
    }
    return eWiFiSuccess;
}

static WIFIReturnCode_t conv_mode_to_qcom(WIFIDeviceMode_t xDeviceMode, QCOM_WLAN_DEV_MODE *dev_mode)
{
    switch (xDeviceMode)
    {
        case eWiFiModeStation:
            *dev_mode = QCOM_WLAN_DEV_MODE_STATION;
            break;
        case eWiFiModeAP:
            *dev_mode = QCOM_WLAN_DEV_MODE_AP;
            break;
        case eWiFiModeP2P:
            *dev_mode = QCOM_WLAN_DEV_MODE_ADHOC;
            break;
        default:
            return eWiFiFailure;
    }
    return eWiFiSuccess;
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
    A_STATUS result;

    /* Prevent re-initialization. WiFi is aleady on this is successful. */
    if (g_wifi_is_on)
        return eWiFiSuccess;

    /* Initialize Wi-Fi shield */
    result = (A_STATUS)WIFISHIELD_Init();
    if (A_OK != result)
        return eWiFiFailure;

    /* Power off the WLAN and wait 30ms */
    CUSTOM_HW_POWER_UP_DOWN(NULL, false);
    vTaskDelay(MSEC_TO_TICK(30));

    g_wifi_ctx.PARAM_PTR = &g_wifi_params;
    if (A_OK != ATHEROS_WIFI_IF.INIT(&g_wifi_ctx))
        return eWiFiFailure;

    /* Disable low power mode to avoid SPI bus flood */
    qcom_power_set_mode(0, MAX_PERF_POWER, USER);

    /* Create a on_connect semaphore, */
    g_wifi_semaph = xSemaphoreCreateBinary();
    if (NULL == g_wifi_semaph)
        return eWiFiFailure;
    xSemaphoreGive(g_wifi_semaph);

    /* Create a on_connect semaphore, */
    g_connect_semaph = xSemaphoreCreateBinary();
    if (NULL == g_connect_semaph)
        return eWiFiFailure;

    /* Create a dhcp semaphore, */
    g_dhcp_semaph = xSemaphoreCreateBinary();
    if (NULL == g_dhcp_semaph)
        return eWiFiFailure;

    /* Wait for Wi-Fi */
    vTaskDelay(MSEC_TO_TICK(100));
    g_wifi_is_on = 1;

    return eWiFiSuccess;
}

WIFIReturnCode_t WIFI_Off( void )
{
    return eWiFiNotSupported;
}

/**
 * @brief Connects to Access Point.
 *
 * @param[in] pxNetworkParams Configuration to join AP.
 *
 * @return eWiFiSuccess if connection is successful, eWiFiFailure otherwise.
 */
WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    WLAN_AUTH_MODE auth_mode;
    WLAN_CRYPT_TYPE crypt_type;
    WIFIReturnCode_t status = eWiFiFailure;
    const TickType_t connect_timeout = pdMS_TO_TICKS( 30000UL );
    const TickType_t dhcp_timeout = pdMS_TO_TICKS( 20000UL );
    uint32_t tmp_ip4_addr = 0, tmp_ip4_mask = 0, tmp_ip4_gw = 0;
    uint8_t ucDhcpSuccessful = 0;

    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Check params */
    if (NULL == pxNetworkParams || NULL == pxNetworkParams->pcSSID || NULL == pxNetworkParams->pcPassword)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Reset "connect" and "dhcp" semaphores */
            g_expected_event = expected_event_default;
            xQueueReset((void*)g_connect_semaph);
            xQueueReset((void*)g_dhcp_semaph);
            /* Disconnect Wi-Fi */
            if (g_connected)
            {
                g_expected_event = expected_event_disconnect;
                if (A_OK == qcom_disconnect(g_devid))
                {
                    /* Consider disconnected */
                    g_connected = 0;
                    /* Wait for callback, that is invoked from 'driver_task' context */
                    if (pdTRUE != xSemaphoreTake(g_connect_semaph, connect_timeout))
                    {
                        break;
                    }
                    /* Workaround for ARP cache */
                    if (0 != g_ip4_gw)
                    {
                        if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_STATIC, &g_ip4_gw, &g_ip4_mask, &g_ip4_gw))
                        {
                            break;
                        }
                        g_ip4_addr = g_ip4_mask = g_ip4_gw = 0;
                    }
                }
                else
                {
                    break;
                }
            }
            /* Set Wi-Fi to device mode */
            if (A_OK != (A_STATUS)qcom_op_set_mode(g_devid, QCOM_WLAN_DEV_MODE_STATION))
            {
                break;
            }

            /* Set SSID, must be done before auth, cipher and passphrase */
            strncpy(g_ssid.ssid, pxNetworkParams->pcSSID, sizeof(g_ssid.ssid));
            if (A_OK != (A_STATUS)qcom_set_ssid(g_devid, &g_ssid))
            {
                break;
            }

            g_security = pxNetworkParams->xSecurity;
            /* Convert 'WIFISecurity_t' to 'WLAN_AUTH_MODE', 'WLAN_CRYPT_TYPE' */
            if (eWiFiSuccess != conv_security_to_qcom(pxNetworkParams->xSecurity, &auth_mode, &crypt_type))
            {
                break;
            }

            /* Set encyption mode */
            if (A_OK != (A_STATUS)qcom_sec_set_encrypt_mode(g_devid, crypt_type))
            {
                break;
            }

            /* Set auth mode */
            if (A_OK != qcom_sec_set_auth_mode(g_devid, auth_mode))
            {
                break;
            }

            /* Set passphrase */
            strncpy(g_passphr.passphrase, pxNetworkParams->pcPassword, sizeof(g_passphr.passphrase) - 1);
            if (A_OK != qcom_sec_set_passphrase(g_devid, &g_passphr))
            {
                break;
            }

            /* Set channel */
            if (0 != pxNetworkParams->cChannel)
            {
                if (A_OK != qcom_set_channel(g_devid, pxNetworkParams->cChannel))
                {
                    break;
                }
            }

            /* Set connect_callback */
            if (A_OK != qcom_set_connect_callback(g_devid, (void *)aws_connect_cb))
            {
                break;
            }

            g_expected_event = expected_event_connect;
            /* Commit settings to Wi-Fi module. Calling this function starts the connection process. */
            if (A_OK != qcom_commit(g_devid))
            {
                break;
            }

            /* Wait for callback, that is invoked from 'driver_task' context. This callback sets g_connected to connected (1) or disconnected (0). */
            if (pdTRUE != xSemaphoreTake(g_connect_semaph, connect_timeout))
            {
                break;
            }

            /* Register DHCP callback */
            if (A_OK != qcom_dhcpc_register_cb(0, (void*)aws_dhcpc_callback))
            {
                break;
            }

            /* Try several attempts in worst case */
            for (int i = 0; i < 10 && 0 == g_ip4_addr; i++)
            {
                /* Perform DHCP request */
                if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_DHCP, &tmp_ip4_addr, &tmp_ip4_mask, &tmp_ip4_gw))
                {
                    break;
                }

                /* If DHCP response is not available immediately, wait for DHCP callback */
                if (!IPs_are_valid(tmp_ip4_addr, tmp_ip4_mask, tmp_ip4_gw))
                {
                    /* Wait for DHCP response */
                    if (pdTRUE != xSemaphoreTake(g_dhcp_semaph, dhcp_timeout))
                    {
                        break;
                    }
                }
                else
                {
                    /* Valid IP of DHCP response */
                    g_ip4_addr = tmp_ip4_addr;
                    g_ip4_mask = tmp_ip4_mask;
                    g_ip4_gw = tmp_ip4_gw;
                }
            }

            /* Still not a valid IP, report error */
            if (!IPs_are_valid(g_ip4_addr, g_ip4_mask, g_ip4_gw))
            {
                break;
            }
            /* Otherwise after all is said and done the DHCP request is successful. */
            else
            {
                ucDhcpSuccessful = 1;
            }

            /* Everything is OK. We connected to the AP and got an IP address with DHCP. */
            status = ( g_connected && ucDhcpSuccessful ) ? eWiFiSuccess : eWiFiFailure;
        } while (0);

        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
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
    const TickType_t connect_timeout = pdMS_TO_TICKS( 20000UL );
    WIFIReturnCode_t status = eWiFiFailure;

    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Reset connect semaphore */
            g_expected_event = expected_event_default;
            xQueueReset((void*)g_connect_semaph);

            /* Connected to AP */
            if (g_connected)
            {
                /* Register connect cb */
                if (A_OK != qcom_set_connect_callback(g_devid, (void *)aws_connect_cb))
                {
                    break;
                }

                g_expected_event = expected_event_disconnect;
                /* Make disconnect request */
                if (A_OK != qcom_disconnect(g_devid))
                {
                    break;
                }

//                /* Consider disconnected, even if it's not yet confirmed by callback */
//                g_connected = 0;

                /* Wait for disconnect response */
                if (pdTRUE != xSemaphoreTake(g_connect_semaph, connect_timeout))
                {
                    break;
                }

                /* Workaround for ARP cache */
                if (0 != g_ip4_gw)
                {
                    if (A_OK != qcom_ipconfig(g_devid, QCOM_IPCONFIG_STATIC, &g_ip4_gw, &g_ip4_mask, &g_ip4_gw))
                        break;
                    g_ip4_addr = g_ip4_mask = g_ip4_gw = 0;
                }

                /* Consider OK */
                status = eWiFiSuccess;
            }
            /* Disconnected from AP */
            else
            {
                status = eWiFiSuccess;
            }
        } while(0);

        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
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
    QCOM_WLAN_DEV_MODE dev_mode = QCOM_WLAN_DEV_MODE_INVALID;
    WIFIReturnCode_t status = eWiFiFailure;

    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            if (eWiFiSuccess != conv_mode_to_qcom(xDeviceMode, &dev_mode))
                break;
            if (A_OK != (A_STATUS)qcom_op_set_mode(g_devid, dev_mode))
                break;
            status = eWiFiSuccess;
        } while (0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;;
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
    QCOM_WLAN_DEV_MODE dev_mode = QCOM_WLAN_DEV_MODE_INVALID;
    WIFIReturnCode_t status = eWiFiFailure;

    /* Check params */
    if (NULL == pxDeviceMode)
        return eWiFiFailure;

    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            if (A_OK != (A_STATUS)qcom_op_get_mode(g_devid, &dev_mode))
                break;
            if (eWiFiSuccess != conv_qcom_to_mode(dev_mode, pxDeviceMode))
                break;
            status = eWiFiSuccess;
        } while(0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
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
    uint32_t failed_cnt = 0;
    uint32_t ip4_addr = 0;

    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Check params */
    if ((NULL == pxIPAddr) || (0 == xCount))
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        pxIPAddr_to_ip(pxIPAddr, &ip4_addr);
        for (uint16_t i = 0; i < xCount; i++)
        {
            if (xIntervalMS == 0)
                xIntervalMS = 20000; // max 20 seconds
            if (A_OK != qcom_ping_ms(ip4_addr, 32, xIntervalMS))
            {
                failed_cnt++;
            }
        }
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    /* Report failure if all attempts failed */
    return failed_cnt == xCount ? eWiFiFailure : eWiFiSuccess;
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
    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Check params */
    if (NULL == pxIPAddr)
        return eWiFiFailure;

    ip_to_pxIPAddr(g_ip4_addr, pxIPAddr);

    return g_ip4_addr == 0 ? eWiFiFailure : eWiFiSuccess;
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
    WIFIReturnCode_t status = eWiFiFailure;
    uint8_t mac_addr[ATH_MAC_LEN] = {0};

    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Check params */
    if (NULL == pxMac)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            if (A_OK != qcom_get_bssid(g_devid, mac_addr))
                break;
            memcpy(pxMac, mac_addr, ATH_MAC_LEN);
            status = eWiFiSuccess;
        } while (0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
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
    WIFIReturnCode_t status = eWiFiFailure;
    uint32_t result = A_OK;
    IP_ADDR_T tmp_ip = {0};
    IP_ADDR_T dns_ip = {0};
    uint32_t dns_servers[MAX_DNSADDRS] = {0};
    uint32_t dns_servers_num = 0;

    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Check params */
    if ((NULL == pxIPAddr) || (NULL == pxHost))
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* 'pxHost' is an IP address */
            int tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0;
            if (4 == sscanf(pxHost, "%u.%u.%u.%u", &tmp1, &tmp2, &tmp3, &tmp4))
            {
                pxIPAddr[0] = tmp4;
                pxIPAddr[1] = tmp3;
                pxIPAddr[2] = tmp2;
                pxIPAddr[3] = tmp1;
                status = eWiFiSuccess;
                break;
            }
            /* Obtain DNS IP from DHCP result */
            result = qcom_dns_server_address_get(dns_servers, &dns_servers_num);
            if ((A_OK != result) || (dns_servers_num == 0))
                break;

            /* Perform DNS/UDP request, loop over DNS servers until first success */
            for (uint32_t dns_idx = 0; dns_idx < dns_servers_num; dns_idx++)
            {
                dns_ip.s_addr = dns_servers[dns_idx];
                if (A_OK != qcom_dns_resolver(dns_ip, pxHost, &tmp_ip, wificonfigDNS_QUERY_TIMEOUT))
                    continue;

                /* Copy to output format and terminate loop*/
                pxIPAddr[0] = (uint8_t)(tmp_ip.s_addr >> 24);
                pxIPAddr[1] = (uint8_t)(tmp_ip.s_addr >> 16);
                pxIPAddr[2] = (uint8_t)(tmp_ip.s_addr >> 8);
                pxIPAddr[3] = (uint8_t)(tmp_ip.s_addr);
                status = eWiFiSuccess;
                break;
            }
        } while(0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }

    return status;
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
    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Check params */
    if (NULL == pxIPAddr)
        return eWiFiFailure;

    ip_to_pxIPAddr(g_ip4_addr, pxIPAddr);

    return g_ip4_addr == 0 ? eWiFiFailure : eWiFiSuccess;
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
    WIFIReturnCode_t status = eWiFiFailure;
    QCOM_BSS_SCAN_INFO *scan_result = NULL;
    int16_t scan_result_num = 0;

    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Check params */
    if ((NULL == pxBuffer) || (0 == uxNumNetworks))
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Note: won't work if SSID is set already */
            if (A_OK != qcom_set_scan(g_devid, NULL))
                break;
            if (qcom_get_scan(g_devid, &scan_result, &scan_result_num))
                break;
            /* Get MIN(scan_result_num, uxNumNetworks) */
            if (scan_result_num > (int16_t)uxNumNetworks)
                scan_result_num = (int16_t)uxNumNetworks;
            for (int16_t i  = 0 ; i < scan_result_num; i++)
            {
                strncpy((char*)pxBuffer->cSSID, (char const*)scan_result[i].ssid, wificonfigMAX_SSID_LEN);
                strncpy((char*)pxBuffer->ucBSSID, (char const*)scan_result[i].bssid, wificonfigMAX_BSSID_LEN);
                pxBuffer->cRSSI = scan_result[i].rssi;
                pxBuffer->cChannel = scan_result[i].channel;
                pxBuffer->ucHidden = 0;
                if (!scan_result[i].security_enabled)
                {
                    pxBuffer->xSecurity = eWiFiSecurityOpen;
                }
                else if (
                    (0 == scan_result[i].rsn_cipher) ||
                    (ATH_CIPHER_TYPE_WEP & scan_result[i].rsn_cipher)
                )
                {
                    pxBuffer->xSecurity = eWiFiSecurityWEP;
                }
                else if (ATH_CIPHER_TYPE_CCMP & scan_result[i].rsn_cipher)
                {
                    pxBuffer->xSecurity = eWiFiSecurityWPA2;
                }
                else
                {
                    /* TODO: Expect WPA */
                    pxBuffer->xSecurity = eWiFiSecurityWPA;
                }
#if 0
                PRINTF("-----------------------\r\n");
                PRINTF("channel: %d \r\n", scan_result[i].channel);
                PRINTF("ssid_len: %d \r\n", scan_result[i].ssid_len);
                PRINTF("rssi: %d \r\n", scan_result[i].rssi);
                PRINTF("security_enabled: %d \r\n", scan_result[i].security_enabled);
                PRINTF("beacon_period: %d \r\n", scan_result[i].beacon_period);
                PRINTF("preamble: %d \r\n", scan_result[i].preamble);
                PRINTF("bss_type: %d \r\n", scan_result[i].bss_type);
                PRINTF("bssid: %x%x%x%x%x%x \r\n",
                    scan_result[i].bssid[0],
                    scan_result[i].bssid[1],
                    scan_result[i].bssid[2],
                    scan_result[i].bssid[3],
                    scan_result[i].bssid[4],
                    scan_result[i].bssid[5]
                );
                PRINTF("ssid: %s \r\n", scan_result[i].ssid);
                PRINTF("rsn_cipher: %d \r\n", scan_result[i].rsn_cipher);
                PRINTF("rsn_auth: %d \r\n", scan_result[i].rsn_auth);
                PRINTF("wpa_cipher: %d \r\n", scan_result[i].wpa_cipher);
                PRINTF("wpa_auth: %d \r\n", scan_result[i].wpa_auth);
#endif

                pxBuffer += 1;
            }
            status = eWiFiSuccess;
        } while (0);
        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }
    return status;
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
    WIFIReturnCode_t status = eWiFiFailure;
    WLAN_AUTH_MODE auth_mode = WLAN_AUTH_INVALID;
    WLAN_CRYPT_TYPE crypt_type = WLAN_CRYPT_INVALID;

    /* Check initialization */
    if (!g_wifi_is_on)
        return eWiFiFailure;

    /* Check params */
    if (NULL == pxNetworkParams || NULL == pxNetworkParams->pcSSID || NULL == pxNetworkParams->pcPassword)
        return eWiFiFailure;

    /* Acquire semaphore */
    if (xSemaphoreTake(g_wifi_semaph, portMAX_DELAY) == pdTRUE)
    {
        do {
            /* Set Wi-Fi to device mode */
            if (A_OK != (A_STATUS)qcom_op_set_mode(g_devid, QCOM_WLAN_DEV_MODE_AP))
                break;

            /* Set SSID, must be done before auth, cipher and passphrase */
            strncpy(g_ssid.ssid, pxNetworkParams->pcSSID, sizeof(g_ssid.ssid));
            if (A_OK != (A_STATUS)qcom_set_ssid(g_devid, &g_ssid))
                break;

            /* Convert 'WIFISecurity_t' to 'WLAN_AUTH_MODE', 'WLAN_CRYPT_TYPE' */
            if (eWiFiSuccess != conv_security_to_qcom(pxNetworkParams->xSecurity, &auth_mode, &crypt_type))
                break;

            /* Set encyption mode */
            if (A_OK != (A_STATUS)qcom_sec_set_encrypt_mode(g_devid, crypt_type))
                break;

            /* Set auth mode */
            if (A_OK != qcom_sec_set_auth_mode(g_devid, auth_mode))
                break;

            /* Set passphrase */
            strncpy(g_passphr.passphrase, pxNetworkParams->pcPassword, sizeof(g_passphr.passphrase) - 1);
            if (A_OK != qcom_sec_set_passphrase(g_devid, &g_passphr))
                break;

            /* Commit settings to WiFi module */
            if (A_OK != qcom_commit(g_devid))
                break;

            status = eWiFiSuccess;
        } while (0);

        /* Release semaphore */
        xSemaphoreGive(g_wifi_semaph);
    }

    return status;
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
//    uint32_t ip4_addr = 0, ip4_mask = 0, ip4_gw = 0;
    BaseType_t xIsConnected = pdFALSE;

//    if (A_OK == qcom_ipconfig( g_devid, QCOM_IPCONFIG_DHCP, &ip4_addr, &ip4_mask, &ip4_gw ) )
    if (1 == g_connected)
    {
        xIsConnected = pdTRUE;
    }

    return xIsConnected;
}


