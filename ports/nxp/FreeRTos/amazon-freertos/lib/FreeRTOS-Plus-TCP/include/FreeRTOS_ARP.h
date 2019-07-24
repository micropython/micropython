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

#ifndef FREERTOS_ARP_H
#define FREERTOS_ARP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Application level configuration options. */
#include "FreeRTOSIPConfig.h"
#include "FreeRTOSIPConfigDefaults.h"
#include "IPTraceMacroDefaults.h"

/*-----------------------------------------------------------*/
/* Miscellaneous structure and definitions. */
/*-----------------------------------------------------------*/

typedef struct xARP_CACHE_TABLE_ROW
{
	uint32_t ulIPAddress;		/* The IP address of an ARP cache entry. */
	MACAddress_t xMACAddress;  /* The MAC address of an ARP cache entry. */
	uint8_t ucAge;				/* A value that is periodically decremented but can also be refreshed by active communication.  The ARP cache entry is removed if the value reaches zero. */
    uint8_t ucValid;			/* pdTRUE: xMACAddress is valid, pdFALSE: waiting for ARP reply */
} ARPCacheRow_t;

typedef enum
{
	eARPCacheMiss = 0,			/* 0 An ARP table lookup did not find a valid entry. */
	eARPCacheHit,				/* 1 An ARP table lookup found a valid entry. */
	eCantSendPacket				/* 2 There is no IP address, or an ARP is still in progress, so the packet cannot be sent. */
} eARPLookupResult_t;

typedef enum
{
	eNotFragment = 0,			/* The IP packet being sent is not part of a fragment. */
	eFirstFragment,				/* The IP packet being sent is the first in a set of fragmented packets. */
	eFollowingFragment			/* The IP packet being sent is part of a set of fragmented packets. */
} eIPFragmentStatus_t;

/*
 * If ulIPAddress is already in the ARP cache table then reset the age of the
 * entry back to its maximum value.  If ulIPAddress is not already in the ARP
 * cache table then add it - replacing the oldest current entry if there is not
 * a free space available.
 */
void vARPRefreshCacheEntry( const MACAddress_t * pxMACAddress, const uint32_t ulIPAddress );

#if( ipconfigARP_USE_CLASH_DETECTION != 0 )
	/* Becomes non-zero if another device responded to a gratuitos ARP message. */
	extern BaseType_t xARPHadIPClash;
	/* MAC-address of the other device containing the same IP-address. */
	extern MACAddress_t xARPClashMacAddress;
#endif /* ipconfigARP_USE_CLASH_DETECTION */

#if( ipconfigUSE_ARP_REMOVE_ENTRY != 0 )

	/*
	 * In some rare cases, it might be useful to remove a ARP cache entry of a
	 * known MAC address to make sure it gets refreshed.
	 */
	uint32_t ulARPRemoveCacheEntryByMac( const MACAddress_t * pxMACAddress );

#endif /* ipconfigUSE_ARP_REMOVE_ENTRY != 0 */

/*
 * Look for ulIPAddress in the ARP cache.  If the IP address exists, copy the
 * associated MAC address into pxMACAddress, refresh the ARP cache entry's
 * age, and return eARPCacheHit.  If the IP address does not exist in the ARP
 * cache return eARPCacheMiss.  If the packet cannot be sent for any reason
 * (maybe DHCP is still in process, or the addressing needs a gateway but there
 * isn't a gateway defined) then return eCantSendPacket.
 */
eARPLookupResult_t eARPGetCacheEntry( uint32_t *pulIPAddress, MACAddress_t * const pxMACAddress );

#if( ipconfigUSE_ARP_REVERSED_LOOKUP != 0 )

	/* Lookup an IP-address if only the MAC-address is known */
	eARPLookupResult_t eARPGetCacheEntryByMac( MACAddress_t * const pxMACAddress, uint32_t *pulIPAddress );

#endif
/*
 * Reduce the age count in each entry within the ARP cache.  An entry is no
 * longer considered valid and is deleted if its age reaches zero.
 */
void vARPAgeCache( void );

/*
 * Send out an ARP request for the IP address contained in pxNetworkBuffer, and
 * add an entry into the ARP table that indicates that an ARP reply is
 * outstanding so re-transmissions can be generated.
 */
void vARPGenerateRequestPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer );

/*
 * After DHCP is ready and when changing IP address, force a quick send of our new IP
 * address
 */
void vARPSendGratuitous( void );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* FREERTOS_ARP_H */













