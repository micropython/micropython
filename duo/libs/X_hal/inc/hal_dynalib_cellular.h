/**
 ******************************************************************************
 * @file    hal_dynalib_wlan.h
 * @authors Matthew McGowan
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

#ifndef HAL_DYNALIB_CELLULAR_H
#define	HAL_DYNALIB_CELLULAR_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "cellular_hal.h"
#include "inet_hal.h"
#endif

DYNALIB_BEGIN(hal_cellular)
DYNALIB_FN(hal_cellular, cellular_off)
DYNALIB_FN(hal_cellular, cellular_on)
DYNALIB_FN(hal_cellular, cellular_init)
DYNALIB_FN(hal_cellular, cellular_register)
DYNALIB_FN(hal_cellular, cellular_pdp_activate)
DYNALIB_FN(hal_cellular, cellular_pdp_deactivate)
DYNALIB_FN(hal_cellular, cellular_gprs_attach)
DYNALIB_FN(hal_cellular, cellular_gprs_detach)
DYNALIB_FN(hal_cellular, cellular_fetch_ipconfig)
DYNALIB_FN(hal_cellular, cellular_device_info)
DYNALIB_FN(hal_cellular, cellular_credentials_set)
DYNALIB_FN(hal_cellular, cellular_credentials_get)
DYNALIB_FN(hal_cellular, cellular_sim_ready)
DYNALIB_FN(hal_cellular, cellular_cancel)
DYNALIB_FN(hal_cellular, HAL_NET_SetNetWatchDog)
DYNALIB_FN(hal_cellular, inet_gethostbyname)
DYNALIB_FN(hal_cellular, inet_ping)
DYNALIB_FN(hal_cellular, cellular_signal)
DYNALIB_FN(hal_cellular, cellular_command)

DYNALIB_END(hal_cellular)


#endif
