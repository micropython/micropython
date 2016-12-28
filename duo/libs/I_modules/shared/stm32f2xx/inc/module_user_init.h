/**
 ******************************************************************************
 * @file    user_module.h
 * @authors Matthew McGowan
 * @date    13 February 2015
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

#ifndef USER_MODULE_H
#define	USER_MODULE_H

#include "dynalib.h"
/**
 * Initializes the static memory for this module.
 * 
 * @return The end of static memory for this module.
 */
DYNALIB_EXTERN_C void* module_user_pre_init();

/**
 * Initializes the global object instances in this module. 
 */
DYNALIB_EXTERN_C void module_user_init();

DYNALIB_EXTERN_C void module_user_loop();

DYNALIB_EXTERN_C void module_user_setup();

#endif	/* USER_MODULE_H */

