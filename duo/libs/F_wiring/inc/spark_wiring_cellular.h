/*
 ******************************************************************************
  Copyright (c) 2014-2015 Particle Industries, Inc.  All rights reserved.

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

#ifndef SPARK_WIRING_CELLULAR_H
#define	SPARK_WIRING_CELLULAR_H

#include "spark_wiring_platform.h"
#include "spark_wiring_network.h"
#include "system_network.h"
#include "cellular_hal.h"
#include "spark_wiring_cellularsignal.h"

#if Wiring_Cellular

namespace spark {

class CellularClass : public NetworkClass
{
    CellularDevice device;

public:

    void on() {
        network_on(*this, 0, 0, NULL);
    }
    void off() {
        network_off(*this, 0, 0, NULL);
    }
    void connect(unsigned flags=0) {
        network_connect(*this, flags, 0, NULL);
    }
    void disconnect() {
        network_disconnect(*this, 0, NULL);
    }

    void setCredentials(const char* apn) {
        setCredentials(apn, "", "");
    }
    void setCredentials(const char* username, const char* password) {
        setCredentials("", username, password);
    }
    void setCredentials(const char* apn, const char* username, const char* password) {
        // todo
    }

    void listen(bool begin=true) {
        network_listen(*this, begin ? 0 : 1, NULL);
    }

    bool listening(void) {
        return network_listening(*this, 0, NULL);
    }

    bool ready()
    {
        return network_ready(*this, 0,  NULL);
    }

    CellularSignal RSSI();

    template<typename... Targs>
    inline int command(const char* format, Targs... Fargs)
    {
        return cellular_command(NULL, NULL, 10000, format, Fargs...);
    }

    template<typename... Targs>
    inline int command(system_tick_t timeout_ms, const char* format, Targs... Fargs)
    {
        return cellular_command(NULL, NULL, timeout_ms, format, Fargs...);
    }

    template<typename T, typename... Targs>
    inline int command(int (*cb)(int type, const char* buf, int len, T* param),
            T* param, const char* format, Targs... Fargs)
    {
        return cellular_command((_CALLBACKPTR_MDM)cb, (void*)param, 10000, format, Fargs...);
    }

    template<typename T, typename... Targs>
    inline int command(int (*cb)(int type, const char* buf, int len, T* param),
            T* param, system_tick_t timeout_ms, const char* format, Targs... Fargs)
    {
        return cellular_command((_CALLBACKPTR_MDM)cb, (void*)param, timeout_ms, format, Fargs...);
    }
};


extern CellularClass Cellular;

}   // namespace Spark

#endif  // Wiring_Cellular
#endif	/* SPARK_WIRING_CELLULAR_H */

