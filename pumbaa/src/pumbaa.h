/**
 * @file pumbaa.h
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

#ifndef __PUMBAA_H__
#define __PUMBAA_H__

#include <pumbaa_config.h>

#include "simba.h"

#include "pumbaa_config_default.h"

#include "genhdr/mpversion.h"

#include "py/runtime.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/smallint.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "py/frozenmod.h"
#include "py/stream.h"
#include "lib/utils/pyexec.h"
#include "lib/mp-readline/readline.h"

#include "module_kernel/class_timer.h"
#include "module_sync/class_event.h"
#include "module_sync/class_queue.h"
#include "module_drivers/class_pin.h"

#if defined(FAMILY_SAM)
#    include "module_drivers/class_dac.h"
#    include "module_drivers/class_exti.h"
#    include "module_drivers/class_spi.h"
#    include "module_drivers/class_sd.h"
#endif

#if defined(FAMILY_LINUX)
#    include "module_drivers/class_dac.h"
#    include "module_drivers/class_exti.h"
#    include "module_drivers/class_spi.h"
#    include "module_drivers/class_sd.h"
#    include "module_drivers/class_i2c_soft.h"
#endif

#if defined(ARCH_ESP)
#    include "module_drivers/class_exti.h"
#endif

#endif
