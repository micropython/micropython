/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/ginput/ginput_rules.h
 * @brief   GINPUT safety rules header file.
 *
 * @addtogroup GINPUT
 * @{
 */

#ifndef _GINPUT_RULES_H
#define _GINPUT_RULES_H

#if GFX_USE_GINPUT
	#if !GFX_USE_GEVENT
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GINPUT: GFX_USE_GEVENT is required if GFX_USE_GINPUT is TRUE. It has been turned on for you."
		#endif
		#undef GFX_USE_GEVENT
		#define	GFX_USE_GEVENT		TRUE
	#endif
	#if !GFX_USE_GTIMER
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GINPUT: GFX_USE_GTIMER is required if GFX_USE_GINPUT is TRUE. It has been turned on for you."
		#endif
		#undef GFX_USE_GTIMER
		#define	GFX_USE_GTIMER		TRUE
	#endif
	#if GINPUT_NEED_MOUSE
		#if GINPUT_TOUCH_NOTOUCH
			// No warning needed for this
			#undef GINPUT_TOUCH_NOCALIBRATE
			#define GINPUT_TOUCH_NOCALIBRATE	TRUE
		#endif
		#if GINPUT_TOUCH_NOCALIBRATE
			// No warning needed for this
			#undef GINPUT_TOUCH_NOCALIBRATE_GUI
			#define GINPUT_TOUCH_NOCALIBRATE_GUI	TRUE
		#endif
		#if !GINPUT_TOUCH_NOTOUCH && GINPUT_MOUSE_CLICK_TIME > GINPUT_TOUCH_CXTCLICK_TIME
			#error "GINPUT MOUSE: The GINPUT_MOUSE_CLICK_TIME must be <= GINPUT_TOUCH_CXTCLICK_TIME"
		#endif
	#endif
#endif

#endif /* _GINPUT_RULES_H */
/** @} */
