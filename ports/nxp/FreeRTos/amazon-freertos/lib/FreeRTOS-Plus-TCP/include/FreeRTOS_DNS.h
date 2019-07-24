/*
 * FreeRTOS+TCP V2.0.10
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

#ifndef FREERTOS_DNS_H
#define FREERTOS_DNS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Application level configuration options. */
#include "FreeRTOSIPConfig.h"
#include "IPTraceMacroDefaults.h"


/* The Link-local Multicast Name Resolution (LLMNR)
 * is included.
 * Note that a special MAC address is required in addition to the NIC's actual
 * MAC address: 01:00:5E:00:00:FC
 *
 * The target IP address will be 224.0.0.252
 */
#if( ipconfigBYTE_ORDER == pdFREERTOS_BIG_ENDIAN )
	#define	ipLLMNR_IP_ADDR			0xE00000FC
#else
	#define	ipLLMNR_IP_ADDR			0xFC0000E0
#endif /* ipconfigBYTE_ORDER == pdFREERTOS_BIG_ENDIAN */

#define	ipLLMNR_PORT	5355 /* Standard LLMNR port. */
#define	ipDNS_PORT		53	/* Standard DNS port. */
#define	ipDHCP_CLIENT	67
#define	ipDHCP_SERVER	68
#define	ipNBNS_PORT		137	/* NetBIOS Name Service. */
#define	ipNBDGM_PORT	138 /* Datagram Service, not included. */

/*
 * The following function should be provided by the user and return true if it
 * matches the domain name.
 */
extern BaseType_t xApplicationDNSQueryHook( const char *pcName );

/*
 * LLMNR is very similar to DNS, so is handled by the DNS routines.
 */
uint32_t ulDNSHandlePacket( NetworkBufferDescriptor_t *pxNetworkBuffer );

#if( ipconfigUSE_LLMNR == 1 )
	extern const MACAddress_t xLLMNR_MacAdress;
#endif /* ipconfigUSE_LLMNR */

#if( ipconfigUSE_NBNS != 0 )

	/*
	 * Inspect a NetBIOS Names-Service message.  If the name matches with ours
	 * (xApplicationDNSQueryHook returns true) an answer will be sent back.
	 * Note that LLMNR is a better protocol for name services on a LAN as it is
	 * less polluted
	 */
	uint32_t ulNBNSHandlePacket (NetworkBufferDescriptor_t *pxNetworkBuffer );

#endif /* ipconfigUSE_NBNS */

#if( ipconfigUSE_DNS_CACHE != 0 )

	uint32_t FreeRTOS_dnslookup( const char *pcHostName );

#endif /* ipconfigUSE_DNS_CACHE != 0 */

#if( ipconfigDNS_USE_CALLBACKS != 0 )

	/*
	 * Users may define this type of function as a callback.
	 * It will be called when a DNS reply is received or when a timeout has been reached.
	 */
	typedef void (* FOnDNSEvent ) ( const char * /* pcName */, void * /* pvSearchID */, uint32_t /* ulIPAddress */ );

	/*
	 * Asynchronous version of gethostbyname()
	 * xTimeout is in units of ms.
	 */
	uint32_t FreeRTOS_gethostbyname_a( const char *pcHostName, FOnDNSEvent pCallback, void *pvSearchID, TickType_t xTimeout );
	void FreeRTOS_gethostbyname_cancel( void *pvSearchID );

#endif

/*
 * FULL, UP-TO-DATE AND MAINTAINED REFERENCE DOCUMENTATION FOR ALL THESE
 * FUNCTIONS IS AVAILABLE ON THE FOLLOWING URL:
 * _TBD_ Add URL
 */
uint32_t FreeRTOS_gethostbyname( const char *pcHostName );


#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif /* FREERTOS_DNS_H */













