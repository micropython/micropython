/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gfx.c
 * @brief   GFX common routines.
 */

/* Display various warnings from gfx_rules.h */
#define GFX_DISPLAY_RULE_WARNINGS	TRUE

#include "../gfx.h"

static bool_t gfxInitDone = FALSE;

/* These init functions are defined by each module but not published */
extern void _gosInit(void);
extern void _gosDeinit(void);
#ifdef GFX_OS_PRE_INIT_FUNCTION
		extern void GFX_OS_PRE_INIT_FUNCTION(void);
#endif
#ifdef GFX_OS_EXTRA_INIT_FUNCTION
		extern void GFX_OS_EXTRA_INIT_FUNCTION(void);
#endif
#ifdef GFX_OS_EXTRA_DEINIT_FUNCTION
		extern void GFX_OS_EXTRA_DEINIT_FUNCTION(void);
#endif
#if GFX_USE_GDRIVER
	extern void _gdriverInit(void);
	extern void _gdriverDeinit(void);
#endif
#if GFX_USE_GFILE
	extern void _gfileInit(void);
	extern void _gfileDeinit(void);
#endif
#if GFX_USE_GDISP
	extern void _gdispInit(void);
	extern void _gdispDeinit(void);
#endif
#if GFX_USE_GWIN
	extern void _gwinInit(void);
	extern void _gwinDeinit(void);
#endif
#if GFX_USE_GEVENT
	extern void _geventInit(void);
	extern void _geventDeinit(void);
#endif
#if GFX_USE_GTIMER
	extern void _gtimerInit(void);
	extern void _gtimerDeinit(void);
#endif
#if GFX_USE_GINPUT
	extern void _ginputInit(void);
	extern void _ginputDeinit(void);
#endif
#if GFX_USE_GADC
	extern void _gadcInit(void);
	extern void _gadcDeinit(void);
#endif
#if GFX_USE_GAUDIO
	extern void _gaudioInit(void);
	extern void _gaudioDeinit(void);
#endif
#if GFX_USE_GQUEUE
	extern void _gqueueInit(void);
	extern void _gqueueDeinit(void);
#endif
#if GFX_USE_GMISC
	extern void _gmiscInit(void);
	extern void _gmiscDeinit(void);
#endif
#if GFX_USE_GTRANS
	extern void _gtransInit(void);
	extern void _gtransDeinit(void);
#endif

void gfxInit(void)
{
	/* Ensure we only initialise once */
	if (gfxInitDone)
		return;
	gfxInitDone = TRUE;

	// These must be initialised in the order of their dependancies

	#ifdef GFX_OS_PRE_INIT_FUNCTION
		GFX_OS_PRE_INIT_FUNCTION();
	#endif
	_gosInit();
	#ifdef GFX_OS_EXTRA_INIT_FUNCTION
		GFX_OS_EXTRA_INIT_FUNCTION();
	#endif
	#if GFX_USE_GQUEUE
		_gqueueInit();
	#endif
	#if GFX_USE_GMISC
		_gmiscInit();
	#endif
	#if GFX_USE_GTRANS
		_gtransInit();
	#endif
	#if GFX_USE_GEVENT
		_geventInit();
	#endif
	#if GFX_USE_GTIMER
		_gtimerInit();
	#endif
	#if GFX_USE_GDRIVER
		_gdriverInit();
	#endif
	#if GFX_USE_GFILE
		_gfileInit();
	#endif
	#if GFX_USE_GDISP
		_gdispInit();
	#endif
	#if GFX_USE_GINPUT
		_ginputInit();
	#endif
	#if GFX_USE_GADC
		_gadcInit();
	#endif
	#if GFX_USE_GAUDIO
		_gaudioInit();
	#endif
	#if GFX_USE_GWIN
		_gwinInit();
	#endif
}

void gfxDeinit(void)
{
	if (!gfxInitDone)
		return;
	gfxInitDone = FALSE;

	// We deinitialise the opposite way as we initialised
	#if GFX_USE_GWIN
		_gwinDeinit();
	#endif
	#if GFX_USE_GAUDIO
		_gaudioDeinit();
	#endif
	#if GFX_USE_GADC
		_gadcDeinit();
	#endif
	#if GFX_USE_GINPUT
		_ginputDeinit();
	#endif
	#if GFX_USE_GDISP
		_gdispDeinit();
	#endif
	#if GFX_USE_GFILE
		_gfileDeinit();
	#endif
	#if GFX_USE_GDRIVER
		_gdriverDeinit();
	#endif
	#if GFX_USE_GTIMER
		_gtimerDeinit();
	#endif
	#if GFX_USE_GEVENT
		_geventDeinit();
	#endif
	#if GFX_USE_GTRANS
		_gtransDeinit();
	#endif
	#if GFX_USE_GMISC
		_gmiscDeinit();
	#endif
	#if GFX_USE_GQUEUE
		_gqueueDeinit();
	#endif
	#ifdef GFX_OS_EXTRA_DEINIT_FUNCTION
		GFX_OS_EXTRA_DEINIT_FUNCTION();
	#endif
	_gosDeinit();
}
