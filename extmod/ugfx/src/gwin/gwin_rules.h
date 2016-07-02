/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_rules.h
 * @brief   GWIN safety rules header file.
 *
 * @addtogroup GWIN
 * @{
 */

#ifndef _GWIN_RULES_H
#define _GWIN_RULES_H

#if GFX_USE_GWIN
	// Sub-system rules
	#if !GFX_USE_GDISP
		#error "GWIN: GFX_USE_GDISP must be TRUE when using GWIN"
	#endif
	#if !GDISP_NEED_CLIP
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GWIN: Drawing can occur outside the defined windows as GDISP_NEED_CLIP is FALSE"
		#endif
	#endif

	// Objects require their super-class
	#if GWIN_NEED_TABSET || GWIN_NEED_FRAME || GWIN_NEED_CONTAINER
		#if !GWIN_NEED_CONTAINERS
			#if GFX_DISPLAY_RULE_WARNINGS
				#warning "GWIN: GWIN_NEED_CONTAINERS is required when a container is enabled. It has been turned on for you."
			#endif
			#undef GWIN_NEED_CONTAINERS
			#define GWIN_NEED_CONTAINERS	TRUE
		#endif
	#endif
	#if GWIN_NEED_BUTTON || GWIN_NEED_SLIDER || GWIN_NEED_CHECKBOX || GWIN_NEED_LABEL || GWIN_NEED_RADIO || GWIN_NEED_LIST || \
		GWIN_NEED_IMAGE || GWIN_NEED_CHECKBOX || GWIN_NEED_PROGRESSBAR || GWIN_NEED_KEYBOARD || GWIN_NEED_TEXTEDIT
		#if !GWIN_NEED_WIDGET
			#if GFX_DISPLAY_RULE_WARNINGS
				#warning "GWIN: GWIN_NEED_WIDGET is required when a widget is used. It has been turned on for you."
			#endif
			#undef GWIN_NEED_WIDGET
			#define GWIN_NEED_WIDGET	TRUE
		#endif
	#endif

	// Rules for the super-classes
	#if GWIN_NEED_CONTAINERS
		#if !GWIN_NEED_WIDGET
			#if GFX_DISPLAY_RULE_WARNINGS
				#warning "GWIN: GWIN_NEED_WIDGET is required when GWIN_NEED_CONTAINERS is enabled. It has been turned on for you."
			#endif
			#undef GWIN_NEED_WIDGET
			#define GWIN_NEED_WIDGET	TRUE
		#endif
	#endif
	#if GWIN_NEED_WIDGET
		#if !GDISP_NEED_TEXT
			#error "GWIN: GDISP_NEED_TEXT is required if GWIN_NEED_WIDGET is TRUE."
		#endif
		#if !GFX_USE_GINPUT
			// This test also ensures that GFX_USE_GEVENT is set
			#error "GWIN: GFX_USE_GINPUT (and one or more input sources) is required if GWIN_NEED_WIDGET is TRUE"
		#endif
		#if !GWIN_NEED_WINDOWMANAGER
			#if GFX_DISPLAY_RULE_WARNINGS
				#warning "GWIN: GWIN_NEED_WINDOWMANAGER is required if GWIN_NEED_WIDGET is TRUE. It has been turned on for you."
			#endif
			#undef GWIN_NEED_WINDOWMANAGER
			#define GWIN_NEED_WINDOWMANAGER	TRUE
		#endif
		#if !GDISP_NEED_MULTITHREAD
			#if GFX_DISPLAY_RULE_WARNINGS
				#warning "GWIN: GDISP_NEED_MULTITHREAD is required if GWIN_NEED_WIDGET is TRUE. It has been turned on for you"
			#endif
			#undef GDISP_NEED_MULTITHREAD
			#define GDISP_NEED_MULTITHREAD	TRUE
		#endif
	#endif
	#if GWIN_NEED_WINDOWMANAGER
		#if !GFX_USE_GQUEUE || !GQUEUE_NEED_ASYNC
			#if GFX_DISPLAY_RULE_WARNINGS
				#warning "GWIN: GFX_USE_GQUEUE and GQUEUE_NEED_ASYNC is required if GWIN_NEED_WINDOWMANAGER is TRUE. It has been turned on for you."
			#endif
			#undef GFX_USE_GQUEUE
			#undef GQUEUE_NEED_ASYNC
			#define GFX_USE_GQUEUE		TRUE
			#define GQUEUE_NEED_ASYNC	TRUE
		#endif
		#if !GFX_USE_GTIMER
			#if GFX_DISPLAY_RULE_WARNINGS
				#warning "GWIN: GFX_USE_GTIMER is required if GWIN_NEED_WINDOWMANAGER is TRUE. It has been turned on for you."
			#endif
			#undef GFX_USE_GTIMER
			#define GFX_USE_GTIMER		TRUE
		#endif
	#endif

	// Rules for individual objects
	#if GWIN_NEED_LIST
		#if !GDISP_NEED_TEXT
			#error "GWIN: GDISP_NEED_TEXT is required when GWIN_NEED_LIST is TRUE."
		#endif
	#endif
	#if GWIN_NEED_RADIO
		#if !GDISP_NEED_CIRCLE
			#if GFX_DISPLAY_RULE_WARNINGS
				#warning "GWIN: GDISP_NEED_CIRCLE should be set to TRUE for much nicer radio button widgets."
			#endif
		#endif
	#endif
	#if GWIN_NEED_IMAGE
		#if !GDISP_NEED_IMAGE
			#error "GWIN: GDISP_NEED_IMAGE is required when GWIN_NEED_IMAGE is TRUE."
		#endif
	#endif
	#if GWIN_NEED_CONSOLE
		#if !GDISP_NEED_TEXT
			#error "GWIN: GDISP_NEED_TEXT is required if GWIN_NEED_CONSOLE is TRUE."
		#endif
	#endif
	#if GWIN_NEED_TEXTEDIT
		#if !GDISP_NEED_TEXT
			#error "GWIN: GDISP_NEED_TEXT is required if GWIN_NEED_TEXTEDIT is TRUE."
		#endif
		#if !(GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD)
			#error "GWIN: GINPUT_NEED_KEYBOARD or GWIN_NEED_KEYBOARD is required if GWIN_NEED_TEXTEDIT is TRUE."
		#endif
	#endif
#endif

#endif /* _GWIN_RULES_H */
/** @} */
