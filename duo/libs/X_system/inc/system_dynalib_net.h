/**
 ******************************************************************************
 * @file    system_dynalib_network.h
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

#ifndef SYSTEM_DYNALIB_NETWORK_H
#define	SYSTEM_DYNALIB_NETWORK_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "system_network.h"
#endif

DYNALIB_BEGIN(system_net)
DYNALIB_FN(system_net, network_config)
DYNALIB_FN(system_net, network_connect)
DYNALIB_FN(system_net, network_connecting)
DYNALIB_FN(system_net, network_disconnect)
DYNALIB_FN(system_net, network_ready)
DYNALIB_FN(system_net, network_on)
DYNALIB_FN(system_net, network_off)
DYNALIB_FN(system_net, network_listen)
DYNALIB_FN(system_net, network_listening)
DYNALIB_FN(system_net, network_has_credentials)
DYNALIB_FN(system_net, network_set_credentials)
DYNALIB_FN(system_net, network_clear_credentials)
DYNALIB_END(system_net)


#endif	/* SYSTEM_DYNALIB_NETWORK_H */

