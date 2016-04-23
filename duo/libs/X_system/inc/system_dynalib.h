/**
 ******************************************************************************
 * @file    system_dynalib.h
 * @authors Matthew McGowan
 * @date    12 February 2015
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

#ifndef SYSTEM_DYNALIB_H
#define	SYSTEM_DYNALIB_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "system_mode.h"
#include "system_sleep.h"
#include "system_task.h"
#include "system_update.h"
#include "system_event.h"
#include "system_version.h"
#endif

DYNALIB_BEGIN(system)
DYNALIB_FN(system, system_mode)
DYNALIB_FN(system, set_system_mode)

DYNALIB_FN(system, set_ymodem_serial_flash_update_handler)
DYNALIB_FN(system, system_firmwareUpdate)
DYNALIB_FN(system, system_fileTransfer)

DYNALIB_FN(system, system_delay_ms)
DYNALIB_FN(system, system_sleep)
DYNALIB_FN(system, system_sleep_pin)
DYNALIB_FN(system, system_subscribe_event)
DYNALIB_FN(system, system_unsubscribe_event)
DYNALIB_FN(system, system_button_pushed_duration)
DYNALIB_FN(system, system_thread_set_state)
DYNALIB_FN(system, system_version_info)
DYNALIB_FN(system, system_internal)
DYNALIB_FN(system, system_set_flag)
DYNALIB_FN(system, system_get_flag)
DYNALIB_FN(system, Spark_Prepare_For_Firmware_Update)
DYNALIB_FN(system, Spark_Save_Firmware_Chunk)
DYNALIB_FN(system, Spark_Finish_Firmware_Update)

DYNALIB_END(system)


#endif	/* SYSTEM_DYNALIB_H */

