/**
 * @file module_sync/class_event.h
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

#ifndef __MODULE_SYNC_CLASS_EVENT_H__
#define __MODULE_SYNC_CLASS_EVENT_H__

#include "pumbaa.h"

struct class_event_t {
    mp_obj_base_t base;
    struct event_t event;
};

extern const mp_obj_type_t module_sync_class_event;

#endif
