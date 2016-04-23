/**
 ******************************************************************************
 * @file    inet_hal.h
 * @author  Matthew McGowan
 * @version V1.0.0
 * @date    25-Sept-2014
 * @brief   Internet APIs
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#ifndef DNS_HAL_H
#define	DNS_HAL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "static_assert.h"

#if (PLATFORM_ID>=4 && PLATFORM_ID<=8) || (PLATFORM_ID==88)
#define HAL_IPv6 1
#else
#define HAL_IPv6 0
#endif

#if HAL_IPv6
typedef struct __attribute__((__packed__)) _HAL_IPAddress_t  {
    union {
        uint32_t ipv4;
        uint32_t ipv6[4];
    };
    uint8_t v;              // 4 for Ipv4, 6 for Ipv6
} HAL_IPAddress;

#define HAL_IPV4_SET(paddress, value)  ((paddress)->v = 4); ((paddress)->ipv4 = value)

STATIC_ASSERT(HAL_IPAddress_size, sizeof(HAL_IPAddress)==17);
#else
typedef struct __attribute__((__packed__)) _HAL_IPAddress_t {
    union {
        uint32_t ipv4;
    };
} HAL_IPAddress;
STATIC_ASSERT(HAL_IPAddress_size, sizeof(HAL_IPAddress)==4);
#define HAL_IPV4_SET(paddress, value)  (paddress->ipv4 = value)

#endif

inline bool is_ipv4(const HAL_IPAddress* address)
{
#if HAL_IPv6
	return address->v==4;
#else
	return true;
#endif
}

typedef struct __attribute__((__packed__)) _NetworkConfig_t {
    HAL_IPAddress aucIP;             // byte 0 is MSB, byte 3 is LSB
    HAL_IPAddress aucSubnetMask;     // byte 0 is MSB, byte 3 is LSB
    HAL_IPAddress aucDefaultGateway; // byte 0 is MSB, byte 3 is LSB
    HAL_IPAddress aucDHCPServer;     // byte 0 is MSB, byte 3 is LSB
    HAL_IPAddress aucDNSServer;      // byte 0 is MSB, byte 3 is LSB
    uint8_t uaMacAddr[6];
} NetworkConfig;
STATIC_ASSERT(NetworkConfig_size, sizeof(HAL_IPAddress)*5+6);


typedef struct __attribute__((__packed__))  _WLanConfig_t {
    uint16_t size;
    NetworkConfig nw;
    uint8_t uaSSID[33];
    uint8_t BSSID[6];			// since V2
} WLanConfig;

#define WLanConfig_Size_V1   (sizeof(NetworkConfig)+2+33)
#define WLanConfig_Size_V2   (WLanConfig_Size_V1+6)

STATIC_ASSERT(WLanConfigSize, sizeof(WLanConfig)==WLanConfig_Size_V2);

typedef uint32_t network_interface_t;

/**
 *
 * @param hostname      buffer to receive the hostname
 * @param hostnameLen   length of the hostname buffer
 * @param out_ip_addr   The ip address in network byte order.
 * @return
 */
int inet_gethostbyname(const char* hostname, uint16_t hostnameLen, HAL_IPAddress* out_ip_addr,
        network_interface_t nif, void* reserved);


/**
 *
 * @param remoteIP  The IP address. MSB..LSB [0..3]
 * @param nTries
 * @return >0 on success. 0 on timeout? <0 on error.
 */
int inet_ping(const HAL_IPAddress* address, network_interface_t nif, uint8_t nTries,
        void* reserved);


#ifdef	__cplusplus
}
#endif

#endif	/* DNS_HAL_H */

