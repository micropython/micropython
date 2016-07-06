/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GOS_NIOS_H
#define _GOS_NIOS_H

#if GFX_USE_OS_NIOS

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/alt_alarm.h>

typedef alt_u32 systemticks_t;
typedef alt_u32 delaytime_t;
typedef unsigned char bool_t;

#ifdef __cplusplus
extern "C" {
#endif

void gfxHalt(const char* msg);
void gfxExit(void);
systemticks_t gfxSystemTicks(void);
systemticks_t gfxMillisecondsToTicks(delaytime_t ms);

#ifdef __cplusplus
}
#endif


// Use the generic thread handling and heap handling
#define GOS_NEED_X_THREADS	TRUE
#define GOS_NEED_X_HEAP		TRUE

#include "gos_x_threads.h"
#include "gos_x_heap.h"

#endif /* GFX_USE_OS_NIOS */
#endif /* _GOS_NIOS_H */
