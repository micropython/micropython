/**
 ******************************************************************************
 * @file    services-dynalib.h
 * @author  Matthew McGowan
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

#ifndef SERVICES_DYNALIB_H
#define	SERVICES_DYNALIB_H

#include "dynalib.h"

DYNALIB_BEGIN(services)
DYNALIB_FN(services,LED_SetRGBColor)
DYNALIB_FN(services,LED_SetSignalingColor)
DYNALIB_FN(services,LED_Signaling_Start)
DYNALIB_FN(services,LED_Signaling_Stop)
DYNALIB_FN(services,LED_SetBrightness)
DYNALIB_FN(services,LED_RGB_Get)
DYNALIB_FN(services,LED_RGB_IsOverRidden)
DYNALIB_FN(services,LED_On)
DYNALIB_FN(services,LED_Off)
DYNALIB_FN(services,LED_Toggle)
DYNALIB_FN(services,LED_Fade)
DYNALIB_FN(services,Get_LED_Brightness)

DYNALIB_FN(services,set_logger_output)
DYNALIB_FN(services,panic_)

DYNALIB_FN(services, jsmn_init)
DYNALIB_FN(services, jsmn_parse)
DYNALIB_FN(services, log_print_)
DYNALIB_FN(services,LED_RGB_SetChangeHandler)
DYNALIB_FN(services, log_print_direct_)
DYNALIB_FN(services, LED_GetColor)
//DYNALIB_FN(services, log_level_active)
DYNALIB_END(services)



#endif	/* SERVICES_DYNALIB_H */

