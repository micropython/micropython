/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gaudio/gaudio_rules.h
 * @brief   GAUDIO safety rules header file.
 *
 * @addtogroup GAUDIO
 * @{
 */

#ifndef _GAUDIO_RULES_H
#define _GAUDIO_RULES_H

#if GFX_USE_GAUDIO
	#if !GAUDIO_NEED_PLAY && !GAUDIO_NEED_RECORD
		#error "GAUDIO: GAUDIO_NEED_PLAY and/or GAUDIO_NEED_RECORD is required if GFX_USE_GAUDIO is TRUE"
	#endif
	#if !GFX_USE_GQUEUE
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GAUDIO: GFX_USE_GQUEUE is required if GFX_USE_GAUDIO is TRUE. It has been turned on for you."
		#endif
		#undef GFX_USE_GQUEUE
		#define	GFX_USE_GQUEUE		TRUE
	#endif
	#if GAUDIO_NEED_PLAY && !GQUEUE_NEED_ASYNC
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GAUDIO: GQUEUE_NEED_ASYNC is required if GAUDIO_NEED_PLAY is TRUE. It has been turned on for you."
		#endif
		#undef GQUEUE_NEED_ASYNC
		#define	GQUEUE_NEED_ASYNC		TRUE
	#endif
	#if !GQUEUE_NEED_GSYNC || !GQUEUE_NEED_BUFFERS
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GAUDIO: GQUEUE_NEED_BUFFERS and GQUEUE_NEED_GSYNC are required if GFX_USE_GAUDIO is TRUE. They have been turned on for you."
		#endif
		#undef GQUEUE_NEED_BUFFERS
		#define	GQUEUE_NEED_BUFFERS		TRUE
		#undef GQUEUE_NEED_GSYNC
		#define	GQUEUE_NEED_GSYNC		TRUE
	#endif
	#if GFX_USE_GEVENT && !GFX_USE_GTIMER
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GAUDIO: GFX_USE_GTIMER is required if GFX_USE_GAUDIO and GFX_USE_GEVENT are TRUE. It has been turned on for you."
		#endif
		#undef GFX_USE_GTIMER
		#define	GFX_USE_GTIMER		TRUE
	#endif
#endif

#endif /* _GAUDIO_RULES_H */
/** @} */
