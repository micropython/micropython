/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* Word wrapping algorithm with UTF-8 support. More than just a basic greedy
 * word-wrapper: it attempts to balance consecutive lines as pairs.
 */

#ifndef _MF_WORDWRAP_H_
#define _MF_WORDWRAP_H_

#include "mf_rlefont.h"
#include <stdbool.h>

/* Callback function for handling each line.
 *
 * line:   Pointer to the beginning of the string for this line.
 * count:  Number of characters on the line.
 * state:  Free variable that was passed to wordwrap().
 * 
 * Returns: true to continue, false to stop after this line.
 */
typedef bool (*mf_line_callback_t) (mf_str line, uint16_t count,
                                    void *state);

/* Word wrap a piece of text. Calls the callback function for each line.
 * 
 * font:  Font to use for metrics.
 * width: Maximum line width in pixels.
 * text:  Pointer to the start of the text to process.
 * state: Free variable for caller to use (can be NULL).
 */
MF_EXTERN void mf_wordwrap(const struct mf_font_s *font, int16_t width,
                           mf_str text, mf_line_callback_t callback, void *state);
              
#endif
