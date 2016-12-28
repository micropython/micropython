/*
  Client.h - Base class that provides Client
  Copyright (c) 2011 Adrian McEwen.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef client_h
#define client_h

#include "system_network.h"
#include "spark_wiring_print.h"
#include "spark_wiring_stream.h"
#include "spark_wiring_ipaddress.h"

class Client : public Stream {
protected:
    network_interface_t nif;

public:
  virtual int connect(IPAddress ip, uint16_t port, network_interface_t=0) =0;
  virtual int connect(const char *host, uint16_t port, network_interface_t=0) =0;
  virtual size_t write(uint8_t) =0;
  virtual size_t write(const uint8_t *buf, size_t size) =0;
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int read(uint8_t *buf, size_t size) = 0;
  virtual int peek() = 0;
  virtual void flush() = 0;
  virtual void stop() = 0;
  virtual uint8_t connected() = 0;
  virtual operator bool() = 0;
};

#endif
