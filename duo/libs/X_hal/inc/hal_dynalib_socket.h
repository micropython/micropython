/**
 ******************************************************************************
 * @file    hal_dynalib_socket.h
 * @authors mat
 * @date    04 March 2015
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

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

#ifndef HAL_DYNALIB_SOCKET_H
#define	HAL_DYNALIB_SOCKET_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "socket_hal.h"
#endif

// WARNING
// The order of functions must not be changed or older applications will break
// when used with newer system firmware.
// Function signatures shouldn't be changed other than changing pointer types.
// New HAL functions must be added to the end of this list.
// GNINRAW

DYNALIB_BEGIN(hal_socket)
DYNALIB_FN(hal_socket,socket_active_status)
DYNALIB_FN(hal_socket,socket_handle_valid)
DYNALIB_FN(hal_socket,socket_create)
DYNALIB_FN(hal_socket,socket_connect)
DYNALIB_FN(hal_socket,socket_receive)
DYNALIB_FN(hal_socket,socket_receivefrom)
DYNALIB_FN(hal_socket,socket_send)
DYNALIB_FN(hal_socket,socket_sendto)
DYNALIB_FN(hal_socket,socket_close)
DYNALIB_FN(hal_socket,socket_reset_blocking_call)
DYNALIB_FN(hal_socket,socket_create_tcp_server)
DYNALIB_FN(hal_socket,socket_accept)
DYNALIB_FN(hal_socket,socket_handle_invalid)
DYNALIB_FN(hal_socket,socket_join_multicast)
DYNALIB_FN(hal_socket,socket_leave_multicast)
DYNALIB_FN(hal_socket,socket_peer)
DYNALIB_END(hal_socket)

#endif	/* HAL_DYNALIB_SOCKET_H */
