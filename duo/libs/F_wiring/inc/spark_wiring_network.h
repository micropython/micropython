/**
 ******************************************************************************
 * @file    spark_wiring_network.h
 * @author  Satish Nair, Timothy Brown
 * @version V1.0.0
 * @date    18-Mar-2014
 * @brief   Header for spark_wiring_network.cpp module
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

#ifndef __SPARK_WIRING_NETWORK_H
#define __SPARK_WIRING_NETWORK_H

#include "spark_wiring_ipaddress.h"

namespace spark {

class NetworkClass;

// Defined as the primary network
extern NetworkClass& Network;


//Retained for compatibility and to flag compiler warnings as build errors
class NetworkClass
{
public:
    uint8_t* macAddress(uint8_t* mac) __attribute__((deprecated("Please use WiFi.macAddress() instead")));
    IPAddress localIP() __attribute__((deprecated("Please use WiFi.localIP() instead")));
    IPAddress subnetMask() __attribute__((deprecated("Please use WiFi.subnetMask() instead")));
    IPAddress gatewayIP() __attribute__((deprecated("Please use WiFi.gatewayIP() instead")));
    char* SSID() __attribute__((deprecated("Please use WiFi.SSID() instead")));
    int8_t RSSI() __attribute__((deprecated("Please use WiFi.RSSI() instead")));
    uint32_t ping(IPAddress remoteIP) __attribute__((deprecated("Please use WiFi.ping() instead")));
    uint32_t ping(IPAddress remoteIP, uint8_t nTries) __attribute__((deprecated("Please use WiFi.ping() instead")));

    static void connect(void) __attribute__((deprecated("Please use WiFi.connect() instead")));
    static void disconnect(void) __attribute__((deprecated("Please use WiFi.disconnect() instead")));
    static bool connecting(void) __attribute__((deprecated("Please use WiFi.connecting() instead")));
    virtual bool ready(void)=0;

    operator network_interface_t() {
        return 0;   // the default
    }


    static NetworkClass& from(network_interface_t nif) {
        // hard-code for now until multiple-networks are implemented.
        return Network;
    }
};


}

#endif
