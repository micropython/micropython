/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* Text alignment and justification algorithm. Supports left, right, center
 * alignment and justify. Supports tab stops and kerning.
 */

#ifndef _MF_JUSTIFY_H_
#define _MF_JUSTIFY_H_

#include "mf_rlefont.h"
#include <stdbool.h>

enum mf_align_t
{
    MF_ALIGN_LEFT = 0,
    MF_ALIGN_CENTER,
    MF_ALIGN_RIGHT
};

/* Callback for rendering a single character.
 * x0:          Left edge of the target position of character.
 * y0:          Upper edge of the target position of character.
 * character:   Character to render.
 * state:       Free state variable for use by the callback.
 * Returns the width of the character.
 */
typedef uint8_t (*mf_character_callback_t) (int16_t x0, int16_t y0,
                                            mf_char character, void *state);

/* Get width of a string in pixels.
 *
 * font:   Pointer to the font definition.
 * text:   Pointer to start of the text to measure.
 * count:  Number of characters on the line or 0 to read until end of string.
 * kern:   True to consider kerning (slower).
 */
MF_EXTERN int16_t mf_get_string_width(const struct mf_font_s *font,
                                      mf_str text, uint16_t count, bool kern);

/* Render a single line of aligned text.
 *
 * font:     Pointer to the font definition.
 * x0:       Depending on aligned, either left, center or right edge of target.
 * y0:       Upper edge of the target area.
 * align:    Type of alignment.
 * text:     Pointer to start of the text to render.
 * count:    Number of characters on the line or 0 to read until end of string.
 * callback: Callback to call for each character.
 * state:    Free variable for use in the callback.
 */
MF_EXTERN void mf_render_aligned(const struct mf_font_s *font,
                                 int16_t x0, int16_t y0,
                                 enum mf_align_t align,
                                 mf_str text, uint16_t count,
                                 mf_character_callback_t callback,
                                 void *state);

/* Render a single line of justified text.
 *
 * font:     Pointer to the font definition.
 * x0:       Left edge of the target area.
 * y0:       Upper edge of the target area.
 * width:    Width of the target area.
 * text:     Pointer to start of the text to render.
 * count:    Number of characters on the line or 0 to read until end of string.
 * callback: Callback to call for each character.
 * state:    Free variable for use in the callback.
 */
MF_EXTERN void mf_render_justified(const struct mf_font_s *font,
                                   int16_t x0, int16_t y0, int16_t width,
                                   mf_str text, uint16_t count,
                                   mf_character_callback_t callback,
                                   void *state);


#endif
