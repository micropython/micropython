/**
 ******************************************************************************
 * @file    spark_wiring_tcpserver.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    13-March-2013
 * @brief   Header for spark_wiring_tcpserver.cpp module
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

#ifndef __SPARK_WIRING_TCPSERVER_H
#define __SPARK_WIRING_TCPSERVER_H

#include "spark_wiring.h"
#include "system_network.h"

class TCPClient;

class TCPServer : public Print {
private:
	uint16_t _port;
        network_interface_t _nif;
	sock_handle_t _sock;
	TCPClient _client;

public:
	TCPServer(uint16_t, network_interface_t nif=0);
        ~TCPServer() { stop(); }

	TCPClient available();
	virtual bool begin();
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *buf, size_t size);
        void stop();
	using Print::write;
};

#endif
