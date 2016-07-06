/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gos/gos_rules.h
 * @brief   GOS safety rules header file.
 *
 * @addtogroup GOS
 * @{
 */

#ifndef _GOS_RULES_H
#define _GOS_RULES_H

#if !GFX_USE_OS_CHIBIOS && !GFX_USE_OS_WIN32 && !GFX_USE_OS_LINUX && !GFX_USE_OS_OSX && !GFX_USE_OS_RAW32 && !GFX_USE_OS_FREERTOS && !GFX_USE_OS_ECOS && !GFX_USE_OS_RAWRTOS && !GFX_USE_OS_ARDUINO && !GFX_USE_OS_CMSIS && !GFX_USE_OS_KEIL && !GFX_USE_OS_NIOS
	#error "GOS: No operating system has been defined."
#endif

#if GFX_USE_OS_CHIBIOS + GFX_USE_OS_WIN32 + GFX_USE_OS_LINUX + GFX_USE_OS_OSX + GFX_USE_OS_RAW32 + GFX_USE_OS_FREERTOS + GFX_USE_OS_ECOS + GFX_USE_OS_RAWRTOS + GFX_USE_OS_ARDUINO + GFX_USE_OS_CMSIS + GFX_USE_OS_KEIL + GFX_USE_OS_NIOS != 1 * TRUE
	#error "GOS: More than one operation system has been defined as TRUE."
#endif

#if GFX_FREERTOS_USE_TRACE && !GFX_USE_OS_FREERTOS
 	#error "GOS: GFX_FREERTOS_USE_TRACE is only available for the FreeRTOS port."
#endif

#if GFX_EMULATE_MALLOC
	#if GFX_USE_OS_WIN32 || GFX_USE_OS_LINUX || GFX_USE_OS_OSX || GFX_USE_OS_ECOS || \
			(GFX_OS_HEAP_SIZE == 0 && (GFX_USE_OS_RAW32 || GFX_USE_OS_ARDUINO || GFX_USE_OS_CMSIS || GFX_USE_OS_KEIL))
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GOS: Cannot emulate malloc as gfxAlloc() internally uses malloc on this platform"
		#endif
		#undef GFX_EMULATE_MALLOC
		#define GFX_EMULATE_MALLOC	FALSE
	#endif
#endif

#endif /* _GOS_RULES_H */
/** @} */
