/*
 * Amazon FreeRTOS
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
 * @file aws_wifi.h
 * @brief Wi-Fi Interface.
 */

#ifndef _AWS_WIFI_H_
#define _AWS_WIFI_H_

#include <stdint.h>

/* FreeRTOS include for BaseType_t. */
#include "portmacro.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

/**
 * @brief Return code denoting API status.
 *
 * @note Codes other than eWiFiSuccess are failure codes.
 */
typedef enum
{
    eWiFiSuccess = 0,      /**< Success. */
    eWiFiFailure = 1,      /**< Failure. */
    eWiFiTimeout = 2,      /**< Timeout. */
    eWiFiNotSupported = 3, /**< Not supported. */
} WIFIReturnCode_t;

/**
 * @brief Wi-Fi Security types.
 */
typedef enum
{
    eWiFiSecurityOpen = 0,    /**< Open - No Security. */
    eWiFiSecurityWEP,         /**< WEP Security. */
    eWiFiSecurityWPA,         /**< WPA Security. */
    eWiFiSecurityWPA2,        /**< WPA2 Security. */
    eWiFiSecurityNotSupported /**< Unknown Security. */
} WIFISecurity_t;

/**
 * @brief Wi-Fi device modes.
 *
 * Device roles/modes supported.
 */
typedef enum
{
    eWiFiModeStation = 0, /**< Station mode. */
    eWiFiModeAP,          /**< Access point mode. */
    eWiFiModeP2P,         /**< P2P mode. */
    eWiFiModeNotSupported /**< Unsupported mode. */
} WIFIDeviceMode_t;

/**
 * @brief Wi-Fi device power management modes.
 *
 * Device power management modes supported.
 */
typedef enum
{
    eWiFiPMNormal = 0,  /**< Normal mode. */
    eWiFiPMLowPower,    /**< Low Power mode. */
    eWiFiPMAlwaysOn,    /**< Always On mode. */
    eWiFiPMNotSupported /**< Unsupported PM mode. */
} WIFIPMMode_t;

/**
 * @brief Parameters passed to the WIFI_ConnectAP API for connection.
 */
typedef struct
{
    const char * pcSSID;      /**< SSID of the Wi-Fi network to join. */
    uint8_t ucSSIDLength;     /**< SSID length not including NULL termination. */
    const char * pcPassword;  /**< Password needed to join the AP. */
    uint8_t ucPasswordLength; /**< Password length not including NULL termination. */
    WIFISecurity_t xSecurity; /**< Wi-Fi Security. @see WIFISecurity_t. */
    int8_t cChannel;          /**< Channel number. */
} WIFINetworkParams_t;

/**
 * @brief Wi-Fi scan results.
 *
 * Structure to store the Wi-Fi scan results.
 *
 * @note The size of char arrays are the MAX lengths + 1 to
 * account for possible null terminating at the end of the
 * strings.
 *
 * @see WIFI_Scan
 */
typedef struct
{
    char cSSID[ wificonfigMAX_SSID_LEN + 1 ];   /**< SSID of the Wi-Fi network with a NULL termination. */
    uint8_t ucBSSID[ wificonfigMAX_BSSID_LEN ]; /**< BSSID of the Wi-Fi network. */
    WIFISecurity_t xSecurity;                   /**< Wi-Fi Security. @see WIFISecurity_t. */
    int8_t cRSSI;                               /**< Signal Strength. */
    int8_t cChannel;                            /**< Channel number. */
    uint8_t ucHidden;                           /**< Hidden channel. */
} WIFIScanResult_t;

/**
 * @brief Wi-Fi network parameters passed to the WIFI_NetworkAdd API.
 *
 * @note The size of char arrays are the MAX lengths + 1 to
 * account for possible null terminating at the end of the
 * strings.
 */
typedef struct
{
    char cSSID[ wificonfigMAX_SSID_LEN + 1 ];           /**< SSID of the Wi-Fi network to join with a NULL termination. */
    uint8_t ucSSIDLength;                               /**< SSID length not including NULL termination. */
    uint8_t ucBSSID[ wificonfigMAX_BSSID_LEN ];         /**< BSSID of the Wi-Fi network. */
    char cPassword[ wificonfigMAX_PASSPHRASE_LEN + 1 ]; /**< Password needed to join the AP with a NULL termination. */
    uint8_t ucPasswordLength;                           /**< Password length not including null termination. */
    WIFISecurity_t xSecurity;                           /**< Wi-Fi Security. @see WIFISecurity_t. */
} WIFINetworkProfile_t;

/**
 * @brief Turns on Wi-Fi.
 *
 * This function turns on Wi-Fi module,initializes the drivers and must be called
 * before calling any other Wi-Fi API
 *
 * @return eWiFiSuccess if Wi-Fi module was successfully turned on, failure code otherwise.
 */
WIFIReturnCode_t WIFI_On( void );

/**
 * @brief Turns off Wi-Fi.
 *
 * This function turns off the Wi-Fi module. The Wi-Fi peripheral should be put in a
 * low power or off state in this routine.
 *
 * @return eWiFiSuccess if Wi-Fi module was successfully turned off, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Off( void );

/**
 * @brief Connects to the Wi-Fi Access Point (AP) specified in the input.
 *
 * The Wi-Fi should stay connected when the same Access Point it is currently connected to
 * is specified. Otherwise, the Wi-Fi should disconnect and connect to the new Access Point
 * specified. If the new Access Point specifed has invalid parameters, then the Wi-Fi should be
 * disconnected.
 *
 * @param[in] pxNetworkParams Configuration to join AP.
 *
 * @return eWiFiSuccess if connection is successful, failure code otherwise.
 *
 * @code
 * WIFINetworkParams_t xNetworkParams;
 * WIFIReturnCode_t xWifiStatus;
 * xNetworkParams.pcSSID = "SSID String";
 * xNetworkParams.ucSSIDLength = SSIDLen;
 * xNetworkParams.pcPassword = "Password String";
 * xNetworkParams.ucPasswordLength = PassLength;
 * xNetworkParams.xSecurity = eWiFiSecurityWPA2;
 * xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );
 * if(xWifiStatus == eWiFiSuccess)
 * {
 *     //Connected to AP.
 * }
 * @endcode
 *
 * @see WIFINetworkParams_t
 */
WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams );

/**
 * @brief Disconnects from the currently connected Access Point.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if disconnection was successful or if the device is already
 * disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Disconnect( void );

/**
 * @brief Resets the Wi-Fi Module.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if Wi-Fi module was successfully reset, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Reset( void );

/**
 * @brief Sets the Wi-Fi mode.
 *
 * @param[in] xDeviceMode - Mode of the device Station / Access Point /P2P.
 *
 * @code
 * WIFIReturnCode_t xWifiStatus;
 * xWifiStatus = WIFI_SetMode(eWiFiModeStation);
 * if(xWifiStatus == eWiFiSuccess)
 * {
 *     //device Set to station mode
 * }
 * @endcode
 *
 * @return eWiFiSuccess if Wi-Fi mode was set successfully, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode );

/**
 * @brief Gets the Wi-Fi mode.
 *
 * @param[out] pxDeviceMode - return mode Station / Access Point /P2P
 *
 * @code
 * WIFIReturnCode_t xWifiStatus;
 * WIFIDeviceMode_t xDeviceMode;
 * xWifiStatus = WIFI_GetMode(&xDeviceMode);
 * if(xWifiStatus == eWiFiSuccess)
 * {
 *    //device mode is xDeviceMode
 * }
 * @endcode
 *
 * @return eWiFiSuccess if Wi-Fi mode was successfully retrieved, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode );

/**
 * @brief Add a Wi-Fi Network profile.
 *
 * Adds a Wi-fi network to the network list in Non Volatile memory.
 *
 * @param[in] pxNetworkProfile - Network profile parameters
 * @param[out] pusIndex - Network profile index in storage
 *
 * @return Index of the profile storage on success, or failure return code on failure.
 *
 * @code
 * WIFINetworkProfile_t xNetworkProfile = {0};
 * WIFIReturnCode_t xWiFiStatus;
 * uint16_t usIndex;
 * strncpy( xNetworkProfile.cSSID, "SSID_Name", SSIDLen));
 * xNetworkProfile.ucSSIDLength = SSIDLen;
 * strncpy( xNetworkProfile.cPassword, "PASSWORD",PASSLen );
 * xNetworkProfile.ucPasswordLength = PASSLen;
 * xNetworkProfile.xSecurity = eWiFiSecurityWPA2;
 * WIFI_NetworkAdd( &xNetworkProfile, &usIndex );
 * @endcode
 */
WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex );


/**
 * @brief Get a Wi-Fi network profile.
 *
 * Gets the Wi-Fi network parameters at the given index from network list in non-volatile
 * memory.
 *
 * @note The WIFINetworkProfile_t data returned must have the the SSID and Password lengths
 * specified as the length without a null terminator.
 *
 * @param[out] pxNetworkProfile - pointer to return network profile parameters
 * @param[in] usIndex - Index of the network profile,
 *                      must be between 0 to wificonfigMAX_NETWORK_PROFILES
 *
 * @return eWiFiSuccess if the network profile was successfully retrieved, failure code
 * otherwise.
 *
 * @see WIFINetworkProfile_t
 *
 * @code
 * WIFINetworkProfile_t xNetworkProfile = {0};
 * uint16_t usIndex = 3;  //Get profile stored at index 3.
 * WIFI_NetworkGet( &xNetworkProfile, usIndex );
 * @endcode
 */
WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex );

/**
 * @brief Delete a Wi-Fi Network profile.
 *
 * Deletes the Wi-Fi network profile from the network profile list at given index in
 * non-volatile memory
 *
 * @param[in] usIndex - Index of the network profile, must be between 0 to
 *                      wificonfigMAX_NETWORK_PROFILES.
 *
 *                      If wificonfigMAX_NETWORK_PROFILES is the index, then all
 *                      network profiles will be deleted.
 *
 * @return eWiFiSuccess if successful, failure code otherwise. If successful, the
 * interface IP address is copied into the IP address buffer.
 *
 * @code
 * uint16_t usIndex = 2; //Delete profile at index 2
 * WIFI_NetworkDelete( usIndex );
 * @endcode
 *
 */
WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex );

/**
 * @brief Ping an IP address in the network.
 *
 * @param[in] IP Address array to ping.
 * @param[in] Number of times to ping
 * @param[in] Interval in mili-seconds for ping operation
 *
 * @return eWiFiSuccess if ping was successful, other failure code otherwise.
 */
WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS );

/**
 * @brief Retrieves the Wi-Fi interface's IP address.
 *
 * @param[out] IP Address buffer.
 *
 * @return eWiFiSuccess if successful and IP Address buffer has the interface's IP address,
 * failure code otherwise.
 *
 * @code
 * uint8_t ucIPAddr[ 4 ];
 * WIFI_GetIP( &ucIPAddr[0] );
 * @endcode
 */
WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr );

/**
 * @brief Retrieves the Wi-Fi interface's MAC address.
 *
 * @param[out] MAC Address buffer sized 6 bytes.
 *
 * @code
 * uint8_t ucMacAddressVal[ wificonfigMAX_BSSID_LEN ];
 * WIFI_GetMAC( &ucMacAddressVal[0] );
 * @endcode
 *
 * @return eWiFiSuccess if the MAC address was successfully retrieved, failure code
 * otherwise. The returned MAC address must be 6 consecutive bytes with no delimitters.
 */
WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac );

/**
 * @brief Retrieves the host IP address from a host name using DNS.
 *
 * @param[in] pxHost - Host (node) name.
 * @param[in] pxIPAddr - IP Address buffer.
 *
 * @return eWiFiSuccess if the host IP address was successfully retrieved, failure code
 * otherwise.
 *
 * @code
 * uint8_t ucIPAddr[ 4 ];
 * WIFI_GetHostIP( "amazon.com", &ucIPAddr[0] );
 * @endcode
 */
WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr );

/**
 * @brief Perform a Wi-Fi network Scan.
 *
 * @param[in] pxBuffer - Buffer for scan results.
 * @param[in] uxNumNetworks - Number of networks to retrieve in scan result.
 *
 * @return eWiFiSuccess if the Wi-Fi network scan was successful, failure code otherwise.
 *
 * @note The input buffer will have the results of the scan.
 *
 * @code
 * const uint8_t ucNumNetworks = 10; //Get 10 scan results
 * WIFIScanResult_t xScanResults[ ucNumNetworks ];
 * WIFI_Scan( xScanResults, ucNumNetworks );
 * @endcode
 */
WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks );

/**
 * @brief Start SoftAP mode.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if SoftAP was successfully started, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartAP( void );

/**
 * @brief Stop SoftAP mode.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if the SoftAP was successfully stopped, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StopAP( void );

/**
 * @brief Configure SoftAP.
 *
 * @param[in] pxNetworkParams - Network parameters to configure AP.
 *
 * @return eWiFiSuccess if SoftAP was successfully configured, failure code otherwise.
 *
 * @code
 * WIFINetworkParams_t xNetworkParams;
 * xNetworkParams.pcSSID = "SSID_Name";
 * xNetworkParams.pcPassword = "PASSWORD";
 * xNetworkParams.xSecurity = eWiFiSecurityWPA2;
 * xNetworkParams.cChannel = ChannelNum;
 * WIFI_ConfigureAP( &xNetworkParams );
 * @endcode
 */
WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams );

/**
 * @brief Set the Wi-Fi power management mode.
 *
 * @param[in] xPMModeType - Power mode type.
 *
 * @param[in] pvOptionValue - A buffer containing the value of the option to set
 *                            depends on the mode type
 *                            example - beacon interval in sec
 *
 * @return eWiFiSuccess if the power mode was successfully configured, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue );

/**
 * @brief Get the Wi-Fi power management mode
 *
 * @param[out] xPMModeType - pointer to get current power mode set.
 *
 * @param[out] pvOptionValue - optional value
 *
 * @return eWiFiSuccess if the power mode was successfully retrieved, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue );


/**
 * @brief Check if the Wi-Fi is connected.
 *
 * @return pdTRUE if the link is up, pdFalse otherwise.
 */
BaseType_t WIFI_IsConnected( void );

#endif /* _AWS_WIFI_H_ */
