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

#ifndef HAL_DYNALIB_WLAN_H
#define	HAL_DYNALIB_WLAN_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "wlan_hal.h"
#include "inet_hal.h"
#endif

// WARNING
// The order of functions must not be changed or older applications will break
// when used with newer system firmware.
// Function signatures shouldn't be changed other than changing pointer types.
// New HAL functions must be added to the end of this list.
// GNINRAW

DYNALIB_BEGIN(hal_wlan)
DYNALIB_FN(hal_wlan,wlan_connect_init)
DYNALIB_FN(hal_wlan,wlan_connect_finalize)
DYNALIB_FN(hal_wlan,wlan_reset_credentials_store_required)
DYNALIB_FN(hal_wlan,wlan_reset_credentials_store)
DYNALIB_FN(hal_wlan,wlan_disconnect_now)
DYNALIB_FN(hal_wlan,wlan_activate)
DYNALIB_FN(hal_wlan,wlan_deactivate)
DYNALIB_FN(hal_wlan,wlan_connected_rssi)

DYNALIB_FN(hal_wlan,wlan_clear_credentials)
DYNALIB_FN(hal_wlan,wlan_has_credentials)
DYNALIB_FN(hal_wlan,wlan_set_credentials)

DYNALIB_FN(hal_wlan,wlan_smart_config_init)
DYNALIB_FN(hal_wlan,wlan_smart_config_cleanup)
DYNALIB_FN(hal_wlan,wlan_smart_config_finalize)

DYNALIB_FN(hal_wlan,wlan_set_error_count)
DYNALIB_FN(hal_wlan,wlan_fetch_ipconfig)
DYNALIB_FN(hal_wlan,wlan_setup)

DYNALIB_FN(hal_wlan,HAL_NET_SetNetWatchDog)
DYNALIB_FN(hal_wlan,inet_gethostbyname)
DYNALIB_FN(hal_wlan,inet_ping)
DYNALIB_FN(hal_wlan,wlan_select_antenna)
DYNALIB_FN(hal_wlan,wlan_set_ipaddress)
DYNALIB_FN(hal_wlan,wlan_set_ipaddress_source)
DYNALIB_FN(hal_wlan,wlan_scan)
DYNALIB_FN(hal_wlan,wlan_get_credentials)
DYNALIB_END(hal_wlan)

#endif	/* HAL_DYNALIB_WLAN_H */

