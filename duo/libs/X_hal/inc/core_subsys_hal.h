/**
 ******************************************************************************
 * @file    core_subsys_hal.h
 * @author  Matthew McGowan
 * @version V1.0.0
 * @date    12-Sept-2014
 * @brief   Defines the subsystem name and version.
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

#ifndef CORE_SUBSYS_H
#define	CORE_SUBSYS_H

#ifdef	__cplusplus
extern "C" {
#endif


/**
 * Reads the subsystem version as a string into a given buffer.
 * @return 0 on success.
 */
int HAL_core_subsystem_version(char* buf, int bufLen);

#if PLATFORM_ID < 3
    /**
     * The event name to publish for this subsystem type.
     */
    #define SPARK_SUBSYSTEM_EVENT_NAME "cc3000-patch-version"

#endif

#ifdef	__cplusplus
}
#endif

#endif	/* CORE_SUBSYS_H */

