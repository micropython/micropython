/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GDISP && GDISP_NEED_TEXT

#include "mcufont/mcufont.h"

/* Custom flag to indicate dynamically allocated font */
#define FONT_FLAG_DYNAMIC 0x80

/**
 * Match a pattern against the font name.
 */
static bool_t matchfont(const char *pattern, const char *name) {
	while(1) {
		switch (pattern[0]) {
		case '*':
			if (name[0] == 0)
				return pattern[1] == 0;
			if (pattern[1] == name[0])
				pattern++;
			else
				name++;
			break;
		case 0:
			return name[0] == 0;
		default:
			if (name[0] != pattern[0])
				return FALSE;
			pattern++;
			name++;
			break;
		}
	}
}

font_t gdispOpenFont(const char *name) {
	const struct mf_font_list_s *fp;
	
	
	// Try the long names first
	for(fp = mf_get_font_list(); fp; fp = fp->next) {
		if (matchfont(name, fp->font->full_name))
			return fp->font;
	}

	// Try the short names if no long names match
	for(fp = mf_get_font_list(); fp; fp = fp->next) {
		if (matchfont(name, fp->font->short_name))
			return fp->font;
	}
	
	/* Return default font.. better than nothing. */
	return mf_get_font_list()->font;
}

void gdispCloseFont(font_t font) {
	if (font->flags & FONT_FLAG_DYNAMIC)
	{
		struct mf_font_s *dfont = (struct mf_font_s *)font;
		
		/* Make sure that no-one can successfully use font after closing */
		dfont->render_character = 0;
		
		/* Release the allocated memory */
		gfxFree(dfont);
	}
}

font_t gdispScaleFont(font_t font, uint8_t scale_x, uint8_t scale_y)
{
	struct mf_scaledfont_s *newfont = gfxAlloc(sizeof(struct mf_scaledfont_s));
	mf_scale_font(newfont, font, scale_x, scale_y);
	return (font_t)newfont;
}

const char *gdispGetFontName(font_t font) {
	return font->short_name;
}

#endif /* GFX_USE_GDISP && GDISP_NEED_TEXT */
