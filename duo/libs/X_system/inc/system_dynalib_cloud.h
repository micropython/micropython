/**
 ******************************************************************************
 * @file    system_dynalib_cloud.h
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

#ifndef SYSTEM_DYNALIB_CLOUD_H
#define	SYSTEM_DYNALIB_CLOUD_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "system_cloud.h"
#endif


DYNALIB_BEGIN(system_cloud)
DYNALIB_FN(system_cloud, spark_variable)
DYNALIB_FN(system_cloud, spark_function)
DYNALIB_FN(system_cloud, spark_process)
DYNALIB_FN(system_cloud, spark_connect)
DYNALIB_FN(system_cloud, spark_disconnect)
DYNALIB_FN(system_cloud, spark_connected)
DYNALIB_FN(system_cloud, system_cloud_protocol_instance)
DYNALIB_FN(system_cloud, spark_deviceID)
DYNALIB_FN(system_cloud, spark_send_event)
DYNALIB_FN(system_cloud, spark_subscribe)
DYNALIB_END(system_cloud)

#endif	/* SYSTEM_DYNALIB_CLOUD_H */

