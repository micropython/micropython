# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#              http://ugfx.org/license.html

GFXSRC +=   $(GFXLIB)/src/gdisp/gdisp.c \
			$(GFXLIB)/src/gdisp/gdisp_fonts.c \
			$(GFXLIB)/src/gdisp/gdisp_pixmap.c \
			$(GFXLIB)/src/gdisp/gdisp_image.c \
			$(GFXLIB)/src/gdisp/gdisp_image_native.c \
			$(GFXLIB)/src/gdisp/gdisp_image_gif.c \
			$(GFXLIB)/src/gdisp/gdisp_image_bmp.c \
			$(GFXLIB)/src/gdisp/gdisp_image_jpg.c \
			$(GFXLIB)/src/gdisp/gdisp_image_png.c
			
MFDIR = $(GFXLIB)/src/gdisp/mcufont
include $(GFXLIB)/src/gdisp/mcufont/mcufont.mk
#GFXINC += $(MFDIR)
GFXSRC += $(MFSRC)
