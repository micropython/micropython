/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gfile/gfile_rules.h
 * @brief   GFILE safety rules header file.
 *
 * @addtogroup GFILE
 * @{
 */

#ifndef _GFILE_RULES_H
#define _GFILE_RULES_H

#if GFX_USE_GFILE
	#if GFILE_NEED_PETITFS && GFILE_NEED_FATFS
		#error "GFILE: Both GFILE_NEED_PETITFS and GFILE_NEED_FATFS cannot both be turned on at the same time."
	#endif
#endif

#endif /* _GFILE_RULES_H */
/** @} */
