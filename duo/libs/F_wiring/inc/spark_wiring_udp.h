/**
 ******************************************************************************
 * @file    spark_wiring_udp.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    13-March-2013
 * @brief   Header for spark_wiring_udp.cpp module
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.
  Copyright (c) 2008 Bjoern Hartmann

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

#ifndef __SPARK_WIRING_UDP_H
#define __SPARK_WIRING_UDP_H

#include "spark_wiring.h"
#include "spark_wiring_printable.h"
#include "spark_wiring_stream.h"
#include "socket_hal.h"


class UDP : public Stream, public Printable {
private:
        /**
         * The underlying socket handle from the HAL.
         */
	sock_handle_t _sock;

        /**
         * The local port this UDP socket is bound to.
         */
	uint16_t _port;

        /**
         * The IP address of the peer that sent the received packet.
         * Available after parsePacket().
         */
	IPAddress _remoteIP;

        /**
         * The port of the peer that sent the received packet.
         * Available after parsePacket().
         */
	uint16_t _remotePort;

        /**
         * The current read/write offset in the buffer. Set to 0 after
         * parsePacket(), incremented during write()
         */
	uint16_t _offset;

        /**
         * The number of bytes in the buffer. Available after parsePacket()
         */
        uint16_t _total;

        /**
         * The dynamically allocated buffer to store the packet that has been read or
         * the packet that is being written.
         */
        uint8_t* _buffer;

        /**
         * The size of the buffer.
         */
        size_t _buffer_size;

        /**
         * The network interface this UDP socket should bind to.
         */
        network_interface_t _nif;

        /**
         * Set to non-zero if the buffer was dynamically allocated by this class.
         */
        uint8_t _buffer_allocated;



public:
	UDP();
        virtual ~UDP() { stop(); releaseBuffer(); }
        /**
         * @param buffer_size The size of the read/write buffer. Can be 0 if
         * only `readPacket()` and `sendPacket()` are used, as these methods
         * use client-provided buffers.
         *
         * @param buffer    A pre-allocated buffer. This is optional, and if not specified
         *  the UDP class will allocate the buffer dynamically.
         * @param port
         * @param nif
         * @return
         */
        bool setBuffer(size_t buffer_size, uint8_t* buffer=NULL);

        /**
         * Releases the current buffer, discarding any previously allocated memory.
         * After this call only {@link #sendPacket} and {@link #receivePacket} may be used,
         * until a new buffer is set via {@link @setBuffer}.
         */
        void releaseBuffer();

        /**
         * @param port  The local port to connect to.
         * @param nif   The network interface to connect to
         * @return non-zero on success
         */
	virtual uint8_t begin(uint16_t port, network_interface_t nif=0);

        /**
         * Disconnects this UDP socket.
         */
	virtual void stop();

        /**
         * Sends an packet directly. This does not require the UDP instance to have an allocated buffer.
         *
         * @param buffer
         * @param buffer_size
         * @param ip
         * @param port
         * @return
         */
        virtual int sendPacket(const uint8_t* buffer, size_t buffer_size, IPAddress destination, uint16_t port);
        virtual int sendPacket(const char* buffer, size_t buffer_size, IPAddress destination, uint16_t port) {
            return sendPacket((uint8_t*)buffer, buffer_size, destination, port);
        }

        /**
         * Retrieves a packet directly. This does not require the UDP instance to have an allocated buffer.
         * If the buffer is not large enough
         * for the packet, the remainder that doesn't fit is discarded.
         *
         * @param buffer        The buffer to read data to
         * @param buf_size      The buffer size
         * @return The number of bytes written to the buffer, or a negative value on error.
         */
        virtual int receivePacket(uint8_t* buffer, size_t buf_size);
        virtual int receivePacket(char* buffer, size_t buf_size) { return receivePacket((uint8_t*)buffer, buf_size); }

        /**
         * Begin writing a packet to the given destination.
         * @param ip        The IP address of the destination peer.
         * @param port      The destination port of the peer
         * @return non-zero on success.
         */
	virtual int beginPacket(IPAddress ip, uint16_t port);
	virtual int beginPacket(const char *host, uint16_t port);

        /**
         * Writes to the currently open packet after a call to {@link #beginPacket}.
         * @return 1 if the data was written, 0 otherwise.
         * Note that the data is buffered and not sent over the network.
         */
	virtual size_t write(uint8_t);

        /**
         * Writes to the currently open packet after a call to {@link #beginPacket}.
         * @return a positive number if the data was written, 0 otherwise.
         * Note that the data is buffered and not sent over the network.
         */
	virtual size_t write(const uint8_t *buffer, size_t size);

        /**
         * Sends the current buffered packet over the network and clears the buffer.
         * @return
         */
	virtual int endPacket();


        virtual int parsePacket();
        /**
         * Retrieves the size of the unread data following a call to
         * {@ #parsePacket}.
         * @return
         */
	virtual int available();

        /**
         * Read a single byte from the read buffer. Available after parsePacket().
         * @return
         */
	virtual int read();
	virtual int read(unsigned char* buffer, size_t len);

	virtual int read(char* buffer, size_t len) { return read((unsigned char*)buffer, len); };
	virtual int peek();

        /**
         * Discards the currently read packet.
         */
	virtual void flush();


	virtual IPAddress remoteIP() { return _remoteIP; };
	virtual uint16_t remotePort() { return _remotePort; };

        /**
         * Prints the current read parsed packet to the given output.
         * @param p
         * @return
         */
        virtual size_t printTo(Print& p) const;

	/*
	 * Join a multicast address for all UDP sockets which are on the same interface as this one.
	 * This will allow reception of multicast packets sent to the given address for UDP sockets
	 * which have bound the port to which the multicast packet was sent.
	 * NOTE: Can be called only after begin() was called.
	 * @param addr IP multicast address to join
	 * @return Return the result of the join operation
	 */
	int joinMulticast(const IPAddress& ip);

	/*
	 * Leave a multicast address previously joined with socket_join_multicast.
	 * @param addr IP multicast address to leave
	 * @return Return the result of the leave operation
	 */
	int leaveMulticast(const IPAddress& ip);

	using Print::write;
};

#endif
