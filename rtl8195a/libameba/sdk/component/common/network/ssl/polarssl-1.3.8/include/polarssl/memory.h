/**
 * \file memory.h
 *
 * \brief Memory allocation layer (Deprecated to platform layer)
 *
 *  Copyright (C) 2006-2014, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef POLARSSL_MEMORY_H
#define POLARSSL_MEMORY_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#include <stdlib.h>

#if defined(POLARSSL_MEMORY_C) && !defined(POLARSSL_PLATFORM_MEMORY)
#define POLARSSL_PLATFORM_MEMORY
#endif

#include "platform.h"
#include "memory_buffer_alloc.h"

static int memory_set_own( void * (*malloc_func)( size_t ),
                    void (*free_func)( void * ) )
{
    return platform_set_malloc_free( malloc_func, free_func );
}


#endif /* memory.h */
