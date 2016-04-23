/**
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

#ifndef SYSTEM_USER_H
#define	SYSTEM_USER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void setup();
void loop();

void _post_loop();
void serialEventRun();

/**
 * Code that is initialized before any user constructors are called.
 */
void module_user_init_hook(void);

/**
 * Determines if the backup ram was valid, or if it was initialized.
 * @return true if the backup ram was valid and didn't need initializing. False if the ram was invalid and was initialized.
 */
bool __backup_ram_was_valid();

void random_seed_from_cloud(unsigned int value) __attribute__((weak));


#ifdef __cplusplus
}
#endif

#endif	/* SYSTEM_USER_H */

