/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* The very first thing that has to be compiled here is mf_font.c so that
 * inclusion of the font header files does not stop the inclusion of the
 * implementation specific parts of the font files.
 */
#include "mcufont/mf_font.c"
#include "mcufont/mf_rlefont.c"
#include "mcufont/mf_bwfont.c"
#include "mcufont/mf_scaledfont.c"
#include "mcufont/mf_encoding.c"
#include "mcufont/mf_justify.c"
#include "mcufont/mf_kerning.c"
#include "mcufont/mf_wordwrap.c"

#include "gdisp.c"
#include "gdisp_fonts.c"
#include "gdisp_pixmap.c"
#include "gdisp_image.c"
#include "gdisp_image_native.c"
#include "gdisp_image_gif.c"
#include "gdisp_image_bmp.c"
#include "gdisp_image_jpg.c"
#include "gdisp_image_png.c"
