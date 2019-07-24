/*
 * Amazon FreeRTOS Wi-Fi V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_wifi.c
 * @brief Wi-Fi Interface.
 */

/* Socket and Wi-Fi interface includes. */
#include "FreeRTOS.h"
#include "aws_wifi.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
    /* FIX ME. */
    return eWiFiFailure;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    /* FIX ME. */
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected(void)
{
	/* FIX ME. */
	return pdFALSE;
}
