/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    gfx.h
 * @brief   GFX system header file.
 *
 * @addtogroup GFX
 *
 * @brief	Main module to glue all the others together
 *
 * @{
 */

#ifndef _GFX_H
#define _GFX_H

/**
 * These two definitions below are required before anything else so that we can
 * turn module definitions off and on.
 */

/**
 * @brief   Generic 'false' boolean constant.
 */
#if !defined(FALSE) || defined(__DOXYGEN__)
	#define FALSE       0
#endif

/**
 * @brief   Generic 'true' boolean constant.
 */
#if !defined(TRUE) || defined(__DOXYGEN__)
	#define TRUE        -1
#endif

/* gfxconf.h is the user's project configuration for the GFX system. */
#include "gfxconf.h"

/* Include Compiler and CPU support */
#include "src/gfx_compilers.h"

/**
 * @name    GFX sub-systems that can be turned on
 * @{
 */
	/**
	 * @brief   GFX Driver API
	 * @details	Defaults to TRUE
	 * @note	Not much useful can be done without a driver
	 */
	#ifndef GFX_USE_GDRIVER
		#define GFX_USE_GDRIVER	TRUE
	#endif
	/**
	 * @brief   GFX Graphics Display Basic API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware driver to your makefile.
	 * 			Eg.  include $(GFXLIB)/drivers/gdisp/Nokia6610/driver.mk
	 */
	#ifndef GFX_USE_GDISP
		#define GFX_USE_GDISP	FALSE
	#endif
	/**
	 * @brief   GFX Graphics Windowing API
	 * @details	Defaults to FALSE
	 * @details	Extends the GDISP API to add the concept of graphic windows.
	 * @note	Also supports high-level "window" objects such as console windows,
	 * 			buttons, graphing etc
	 */
	#ifndef GFX_USE_GWIN
		#define GFX_USE_GWIN	FALSE
	#endif
	/**
	 * @brief   GFX Event API
	 * @details	Defaults to FALSE
	 * @details	Defines the concept of a "Source" that can send "Events" to "Listeners".
	 */
	#ifndef GFX_USE_GEVENT
		#define GFX_USE_GEVENT	FALSE
	#endif
	/**
	 * @brief   GFX Timer API
	 * @details	Defaults to FALSE
	 * @details	Provides thread context timers - both one-shot and periodic.
	 */
	#ifndef GFX_USE_GTIMER
		#define GFX_USE_GTIMER	FALSE
	#endif
	/**
	 * @brief   GFX Queue API
	 * @details	Defaults to FALSE
	 * @details	Provides queue management.
	 */
	#ifndef GFX_USE_GQUEUE
		#define GFX_USE_GQUEUE	FALSE
	#endif
	/**
	 * @brief   GFX Input Device API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware drivers to your makefile.
	 * 			Eg.
	 * 				include $(GFXLIB)/drivers/ginput/toggle/Pal/driver.mk
	 * 			and...
	 * 				include $(GFXLIB)/drivers/ginput/touch/MCU/driver.mk
	 */
	#ifndef GFX_USE_GINPUT
		#define GFX_USE_GINPUT	FALSE
	#endif
	/**
	 * @brief   GFX Generic Periodic ADC API
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_GADC
		#define GFX_USE_GADC	FALSE
	#endif
	/**
	 * @brief   GFX Audio API
	 * @details	Defaults to FALSE
	 * @note	Also add the specific hardware drivers to your makefile.
	 * 			Eg.
	 * 				include $(GFXLIB)/drivers/gaudio/GADC/driver.mk
	 */
	#ifndef GFX_USE_GAUDIO
		#define GFX_USE_GAUDIO	FALSE
	#endif
	/**
	 * @brief   GFX Miscellaneous Routines API
	 * @details	Defaults to FALSE
	 * @note	Turning this on without turning on any GMISC_NEED_xxx macros will result
	 * 			in no extra code being compiled in. GMISC is made up from the sum of its
	 * 			parts.
	 */
	#ifndef GFX_USE_GMISC
		#define GFX_USE_GMISC	FALSE
	#endif
	/**
	 * @brief   GFX File API
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_GFILE
		#define GFX_USE_GFILE	FALSE
	#endif
	/**
	 * @brief   GFX Translation Support API
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_GTRANS
		#define GFX_USE_GTRANS	FALSE
	#endif
/** @} */

/**
 * Get all the options for each sub-system.
 *
 */
#include "src/gos/gos_options.h"
#include "src/gdriver/gdriver_options.h"
#include "src/gfile/gfile_options.h"
#include "src/gmisc/gmisc_options.h"
#include "src/gtrans/gtrans_options.h"
#include "src/gqueue/gqueue_options.h"
#include "src/gevent/gevent_options.h"
#include "src/gtimer/gtimer_options.h"
#include "src/gdisp/gdisp_options.h"
#include "src/gwin/gwin_options.h"
#include "src/ginput/ginput_options.h"
#include "src/gadc/gadc_options.h"
#include "src/gaudio/gaudio_options.h"

/**
 * Interdependency safety checks on the sub-systems.
 * These must be in dependency order.
 *
 */
#ifndef GFX_DISPLAY_RULE_WARNINGS
	#define GFX_DISPLAY_RULE_WARNINGS	FALSE
#endif
#include "src/gwin/gwin_rules.h"
#include "src/ginput/ginput_rules.h"
#include "src/gdisp/gdisp_rules.h"
#include "src/gaudio/gaudio_rules.h"
#include "src/gadc/gadc_rules.h"
#include "src/gevent/gevent_rules.h"
#include "src/gtimer/gtimer_rules.h"
#include "src/gqueue/gqueue_rules.h"
#include "src/gmisc/gmisc_rules.h"
#include "src/gtrans/gtrans_rules.h"
#include "src/gfile/gfile_rules.h"
#include "src/gdriver/gdriver_rules.h"
#include "src/gos/gos_rules.h"

/**
 *  Include the sub-system header files
 */
#include "src/gos/gos.h"
//#include "src/gdriver/gdriver.h"			// This module is only included by source that needs it.
#include "src/gfile/gfile.h"
#include "src/gmisc/gmisc.h"
#include "src/gtrans/gtrans.h"
#include "src/gqueue/gqueue.h"
#include "src/gevent/gevent.h"
#include "src/gtimer/gtimer.h"
#include "src/gdisp/gdisp.h"
#include "src/gwin/gwin.h"
#include "src/ginput/ginput.h"
#include "src/gadc/gadc.h"
#include "src/gaudio/gaudio.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief	The one call to start it all
	 *
	 * @note	This will initialise each sub-system that has been turned on.
	 * 			For example, if GFX_USE_GDISP is defined then display will be initialised
	 * 			and cleared to black.
	 * @note	If you define GFX_OS_NO_INIT as TRUE in your gfxconf.h file then ugfx doesn't try to
	 * 			initialise the operating system for you when you call @p gfxInit().
	 * @note	If you define GFX_OS_EXTRA_INIT_FUNCTION in your gfxconf.h file the macro is the
	 * 			name of a void function with no parameters that is called immediately after
	 * 			operating system initialisation (whether or not GFX_OS_NO_INIT is set).
	 * @note	If you define GFX_OS_EXTRA_DEINIT_FUNCTION in your gfxconf.h file the macro is the
	 * 			name of a void function with no parameters that is called immediately before
	 * 			operating system de-initialisation (as ugfx is exiting).
	 *
	 * @api
	 */
	void gfxInit(void);

	/**
	 * @brief	The one call to end it all
	 *
	 * @note	This will de-initialise each sub-system that has been turned on.
	 *
	 * @api
	 */
	void gfxDeinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _GFX_H */
/** @} */

