/**
 * @file config.h
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifndef CONFIG_ASSERT
#    define CONFIG_ASSERT                                   0
#endif

#ifndef CONFIG_START_NETWORK
#    define CONFIG_START_NETWORK                            1
#endif

#ifndef CONFIG_THRD_IDLE_STACK_SIZE
#    if defined(ARCH_ARM)
#        define CONFIG_THRD_IDLE_STACK_SIZE              1024
#    endif
#endif

#endif
