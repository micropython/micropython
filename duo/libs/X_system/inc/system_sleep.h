/**
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

#ifndef SYSTEM_SLEEP_H
#define	SYSTEM_SLEEP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SLEEP_MODE_WLAN = 0, SLEEP_MODE_DEEP = 1, SLEEP_MODE_SOFTPOWEROFF = 2
} Spark_Sleep_TypeDef;

void system_sleep(Spark_Sleep_TypeDef mode, long seconds, uint32_t param, void* reserved);
void system_sleep_pin(uint16_t pin, uint16_t mode, long seconds, uint32_t param, void* reserved);

#ifdef __cplusplus
}
#endif


#endif	/* SYSTEM_SLEEP_H */

