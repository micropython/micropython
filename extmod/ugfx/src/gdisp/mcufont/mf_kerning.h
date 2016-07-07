/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* Automatic kerning for font rendering. This solves the issue where some
 * fonts (especially serif fonts) have too much space between specific
 * character pairs, like WA or L'.
 */

#ifndef _MF_KERNING_H_
#define _MF_KERNING_H_

#include "mf_config.h"
#include "mf_rlefont.h"

/* Compute the kerning adjustment when c1 is followed by c2.
 * 
 * font: Pointer to the font definition.
 * c1: The previous character.
 * c2: The next character to render.
 * 
 * Returns the offset to add to the x position for c2.
 */
#if MF_USE_KERNING
MF_EXTERN int8_t mf_compute_kerning(const struct mf_font_s *font,
                                    mf_char c1, mf_char c2);
#else
#define mf_compute_kerning(font, c1, c2)		0
#endif

#endif
