/**
 ******************************************************************************
 * @file    spark_wiring_tcpclient.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    13-March-2013
 * @brief   Header for spark_wiring_tcpclient.cpp module
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

#ifndef __SPARK_WIRING_TCPCLIENT_H
#define __SPARK_WIRING_TCPCLIENT_H

#include "system_network.h"
#include "spark_wiring_client.h"
#include "spark_wiring_ipaddress.h"
#include "spark_wiring_print.h"
#include "socket_hal.h"

#define TCPCLIENT_BUF_MAX_SIZE	128

class TCPClient : public Client {

public:
	TCPClient();
	TCPClient(sock_handle_t sock);
        virtual ~TCPClient() {};

        uint8_t status();
	virtual int connect(IPAddress ip, uint16_t port, network_interface_t=0);
	virtual int connect(const char *host, uint16_t port, network_interface_t=0);
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *buffer, size_t size);
	virtual int available();
	virtual int read();
	virtual int read(uint8_t *buffer, size_t size);
	virtual int peek();
	virtual void flush();
        void flush_buffer();
	virtual void stop();
	virtual uint8_t connected();
	virtual operator bool();

        virtual IPAddress remoteIP();

	friend class TCPServer;

	using Print::write;

protected:
        inline sock_handle_t sock_handle() { return _sock; }

private:
	static uint16_t _srcport;
	sock_handle_t _sock;
	uint8_t _buffer[TCPCLIENT_BUF_MAX_SIZE];
	uint16_t _offset;
	uint16_t _total;
        IPAddress _remoteIP;
	inline int bufferCount();

};

#endif
