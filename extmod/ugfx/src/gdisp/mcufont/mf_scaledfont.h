/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* Generate scaled (nearest-neighbor) fonts. This can be used for displaying
 * larger text without spending the memory required for including larger fonts.
 */

#ifndef _MF_SCALEDFONT_H_
#define _MF_SCALEDFONT_H_

#include "mf_font.h"

struct mf_scaledfont_s
{
    struct mf_font_s font;
    
    const struct mf_font_s *basefont;
    uint8_t x_scale;
    uint8_t y_scale;
};

MF_EXTERN void mf_scale_font(struct mf_scaledfont_s *newfont,
                             const struct mf_font_s *basefont,
                             uint8_t x_scale, uint8_t y_scale);

#ifdef MF_SCALEDFONT_INTERNALS
/* Internal functions, don't use these directly. */
MF_EXTERN uint8_t mf_scaled_render_character(const struct mf_font_s *font,
                                              int16_t x0, int16_t y0,
                                              uint16_t character,
                                              mf_pixel_callback_t callback,
                                              void *state);

MF_EXTERN uint8_t mf_scaled_character_width(const struct mf_font_s *font,
                                             uint16_t character);
#endif

#endif
