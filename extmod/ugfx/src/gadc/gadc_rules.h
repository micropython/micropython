/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gadc/gadc_rules.h
 * @brief   GADC safety rules header file.
 *
 * @addtogroup GADC
 * @{
 */

#ifndef _GADC_RULES_H
#define _GADC_RULES_H

#if GFX_USE_GADC
	#if !GFX_USE_GTIMER
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GADC: GFX_USE_GTIMER is required if GFX_USE_GADC is TRUE. It has been turned on for you."
		#endif
		#undef GFX_USE_GTIMER
		#define	GFX_USE_GTIMER		TRUE
	#endif
	#if !GFX_USE_GQUEUE || !GQUEUE_NEED_GSYNC || !GQUEUE_NEED_BUFFERS
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GADC: GFX_USE_GQUEUE, GQUEUE_NEED_BUFFERS and GQUEUE_NEED_GSYNC are required if GFX_USE_GADC is TRUE. They have been turned on for you."
		#endif
		#undef GFX_USE_GQUEUE
		#define	GFX_USE_GQUEUE		TRUE
		#undef GQUEUE_NEED_BUFFERS
		#define	GQUEUE_NEED_BUFFERS		TRUE
		#undef GQUEUE_NEED_GSYNC
		#define	GQUEUE_NEED_GSYNC		TRUE
	#endif
#endif

#endif /* _GADC_RULES_H */
/** @} */
