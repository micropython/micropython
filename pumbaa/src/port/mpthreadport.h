/**
 * @file port/mphalport.h
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

#ifndef __MPTHREADPORT_H__
#define __MPTHREADPORT_H__

#include "simba.h"

typedef struct _mp_thread_mutex_t {
    struct sem_t sem;
} mp_thread_mutex_t;

/**
 * Initialize the thread module.
 */
void module_thread_init(void);

void mp_thread_gc_others(void);

#endif
