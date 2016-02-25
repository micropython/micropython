/**
 * \file net.h
 *
 * \brief Network communication functions
 *
 *  Copyright (C) 2006-2011, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef POLARSSL_NET_H
#define POLARSSL_NET_H

#include <string.h>

#define POLARSSL_ERR_NET_UNKNOWN_HOST                      -0x0056  /**< Failed to get an IP address for the given hostname. */
#define POLARSSL_ERR_NET_SOCKET_FAILED                     -0x0042  /**< Failed to open a socket. */
#define POLARSSL_ERR_NET_CONNECT_FAILED                    -0x0044  /**< The connection to the given server / port failed. */
#define POLARSSL_ERR_NET_BIND_FAILED                       -0x0046  /**< Binding of the socket failed. */
#define POLARSSL_ERR_NET_LISTEN_FAILED                     -0x0048  /**< Could not listen on the socket. */
#define POLARSSL_ERR_NET_ACCEPT_FAILED                     -0x004A  /**< Could not accept the incoming connection. */
#define POLARSSL_ERR_NET_RECV_FAILED                       -0x004C  /**< Reading information from the socket failed. */
#define POLARSSL_ERR_NET_SEND_FAILED                       -0x004E  /**< Sending information through the socket failed. */
#define POLARSSL_ERR_NET_CONN_RESET                        -0x0050  /**< Connection was reset by peer. */
#define POLARSSL_ERR_NET_WANT_READ                         -0x0052  /**< Connection requires a read call. */
#define POLARSSL_ERR_NET_WANT_WRITE                        -0x0054  /**< Connection requires a write call. */

#define POLARSSL_NET_LISTEN_BACKLOG         10 /**< The backlog that listen() should use. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Initiate a TCP connection with host:port
 *
 * \param fd       Socket to use
 * \param host     Host to connect to
 * \param port     Port to connect to
 *
 * \return         0 if successful, or one of:
 *                      POLARSSL_ERR_NET_SOCKET_FAILED,
 *                      POLARSSL_ERR_NET_UNKNOWN_HOST,
 *                      POLARSSL_ERR_NET_CONNECT_FAILED
 */
int net_connect( int *fd, const char *host, int port );

/**
 * \brief          Create a listening socket on bind_ip:port.
 *                 If bind_ip == NULL, all interfaces are binded.
 *
 * \param fd       Socket to use
 * \param bind_ip  IP to bind to, can be NULL
 * \param port     Port number to use
 *
 * \return         0 if successful, or one of:
 *                      POLARSSL_ERR_NET_SOCKET_FAILED,
 *                      POLARSSL_ERR_NET_BIND_FAILED,
 *                      POLARSSL_ERR_NET_LISTEN_FAILED
 */
int net_bind( int *fd, const char *bind_ip, int port );

/**
 * \brief           Accept a connection from a remote client
 *
 * \param bind_fd   Relevant socket
 * \param client_fd Will contain the connected client socket
 * \param client_ip Will contain the client IP address
 *                  Must be at least 4 bytes, or 16 if IPv6 is supported
 *
 * \return          0 if successful, POLARSSL_ERR_NET_ACCEPT_FAILED, or
 *                  POLARSSL_ERR_NET_WANT_READ is bind_fd was set to
 *                  non-blocking and accept() is blocking.
 */
int net_accept( int bind_fd, int *client_fd, void *client_ip );

/**
 * \brief          Set the socket blocking
 *
 * \param fd       Socket to set
 *
 * \return         0 if successful, or a non-zero error code
 */
int net_set_block( int fd );

/**
 * \brief          Set the socket non-blocking
 *
 * \param fd       Socket to set
 *
 * \return         0 if successful, or a non-zero error code
 */
int net_set_nonblock( int fd );

/**
 * \brief          Portable usleep helper
 *
 * \param usec     Amount of microseconds to sleep
 *
 * \note           Real amount of time slept will not be less than
 *                 select()'s timeout granularity (typically, 10ms).
 */
void net_usleep( unsigned long usec );

/**
 * \brief          Read at most 'len' characters. If no error occurs,
 *                 the actual amount read is returned.
 *
 * \param ctx      Socket
 * \param buf      The buffer to write to
 * \param len      Maximum length of the buffer
 *
 * \return         This function returns the number of bytes received,
 *                 or a non-zero error code; POLARSSL_ERR_NET_WANT_READ
 *                 indicates read() is blocking.
 */
int net_recv( void *ctx, unsigned char *buf, size_t len );

/**
 * \brief          Write at most 'len' characters. If no error occurs,
 *                 the actual amount read is returned.
 *
 * \param ctx      Socket
 * \param buf      The buffer to read from
 * \param len      The length of the buffer
 *
 * \return         This function returns the number of bytes sent,
 *                 or a non-zero error code; POLARSSL_ERR_NET_WANT_WRITE
 *                 indicates write() is blocking.
 */
int net_send( void *ctx, const unsigned char *buf, size_t len );

/**
 * \brief          Gracefully shutdown the connection
 *
 * \param fd       The socket to close
 */
void net_close( int fd );

#ifdef __cplusplus
}
#endif

#endif /* net.h */
