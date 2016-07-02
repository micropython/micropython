/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gtimer/gtimer_rules.h
 * @brief   GTIMER safety rules header file.
 *
 * @addtogroup GTIMER
 * @{
 */

#ifndef _GTIMER_RULES_H
#define _GTIMER_RULES_H

#if GFX_USE_GTIMER
	#if GFX_USE_GDISP && !GDISP_NEED_MULTITHREAD
		#if GFX_DISPLAY_RULE_WARNINGS
			#warning "GTIMER: GDISP_NEED_MULTITHREAD has not been specified."
			#warning "GTIMER: Make sure you are not performing any GDISP/GWIN drawing operations in the timer callback!"
		#endif
	#endif
#endif

#endif /* _GTIMER_RULES_H */
/** @} */
