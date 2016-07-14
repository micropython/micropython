/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GDISP && GDISP_NEED_PIXMAP

// We undef everything because the system may think we are in a single controller situation
//	but the pixmap supports adds another virtual display
#undef GDISP_HARDWARE_DEINIT
#undef GDISP_HARDWARE_FLUSH
#undef GDISP_HARDWARE_STREAM_WRITE
#undef GDISP_HARDWARE_STREAM_READ
#undef GDISP_HARDWARE_STREAM_POS
#undef GDISP_HARDWARE_DRAWPIXEL
#undef GDISP_HARDWARE_CLEARS
#undef GDISP_HARDWARE_FILLS
#undef GDISP_HARDWARE_BITFILLS
#undef GDISP_HARDWARE_SCROLL
#undef GDISP_HARDWARE_PIXELREAD
#undef GDISP_HARDWARE_CONTROL
#undef GDISP_HARDWARE_QUERY
#undef GDISP_HARDWARE_CLIP
#define GDISP_HARDWARE_DEINIT			TRUE
#define GDISP_HARDWARE_DRAWPIXEL		TRUE
#define GDISP_HARDWARE_PIXELREAD		TRUE
#define GDISP_HARDWARE_CONTROL			TRUE
#define IN_PIXMAP_DRIVER				TRUE
#define GDISP_DRIVER_VMT				GDISPVMT_pixmap
#define GDISP_DRIVER_VMT_FLAGS			(GDISP_VFLG_DYNAMICONLY|GDISP_VFLG_PIXMAP)

// This pseudo driver currently only supports unpacked formats with more than 8 bits per pixel
//	that is, we only support GRAY_SCALE and PALETTE with 8 bits per pixel or any unpacked TRUE_COLOR format.
#if (GDISP_LLD_PIXELFORMAT & GDISP_COLORSYSTEM_GRAYSCALE) && (GDISP_LLD_PIXELFORMAT & 0xFF) != 8
	#error "GDISP Pixmap: Pixmap's do not currently support the specified GDISP_LLD_PIXELFORMAT"
#endif

#include "gdisp_driver.h"
#include "../gdriver/gdriver.h"

typedef struct pixmap {
	#if GDISP_NEED_PIXMAP_IMAGE
		uint8_t		imghdr[8];			// This field must come just before the data member.
	#endif
	color_t			pixels[1];			// We really want pixels[0] but some compilers don't allow that even though it is C standard.
	} pixmap;

GDisplay *gdispPixmapCreate(coord_t width, coord_t height) {
	GDisplay	*g;
	pixmap		*p;
	unsigned	i;

	// Calculate the size of the display surface in bytes
	i = width*height*sizeof(color_t);
	if (i < 2*sizeof(coord_t))
		i = 2*sizeof(coord_t);

	// Allocate the pixmap
	if (!(p = gfxAlloc(i+sizeof(pixmap)-sizeof(p->pixels))))
		return 0;

	// Fill in the image header (if required)
	#if GDISP_NEED_PIXMAP_IMAGE
		p->imghdr[0] = 'N';
		p->imghdr[1] = 'I';
		p->imghdr[2] = (uint8_t)(width >> 8);
		p->imghdr[3] = (uint8_t)width;
		p->imghdr[4] = (uint8_t)(height >> 8);
		p->imghdr[5] = (uint8_t)height;
		p->imghdr[6] = (uint8_t)(GDISP_PIXELFORMAT >> 8);
		p->imghdr[7] = (uint8_t)(GDISP_PIXELFORMAT);
	#endif

	// Save the width and height so the driver can retrieve it.
	((coord_t *)p->pixels)[0] = width;
	((coord_t *)p->pixels)[1] = height;

	// Register the driver
	g = (GDisplay *)gdriverRegister(&GDISPVMT_pixmap->d, p);
	if (!g)
		gfxFree(p);
	return g;
}

void gdispPixmapDelete(GDisplay *g) {
	if (gvmt(g) != GDISPVMT_pixmap)
		return;
	gdriverUnRegister(&g->d);
}

pixel_t	*gdispPixmapGetBits(GDisplay *g) {
	if (gvmt(g) != GDISPVMT_pixmap)
		return 0;
	return ((pixmap *)g->priv)->pixels;
}

#if GDISP_NEED_PIXMAP_IMAGE
	void *gdispPixmapGetMemoryImage(GDisplay *g) {
		if (gvmt(g) != GDISPVMT_pixmap)
			return 0;
		return ((pixmap *)g->priv)->imghdr;
	}
#endif

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	// The user api function should have already allocated and initialised the pixmap
	//	structure and put it into the priv member during driver initialisation.
	if (!g->priv)
		return FALSE;

	// Initialize the GDISP structure
	//	Width and height were saved into the start of the framebuffer.
	g->g.Width = ((coord_t *)((pixmap *)g->priv)->pixels)[0];
	g->g.Height = ((coord_t *)((pixmap *)g->priv)->pixels)[1];
	g->g.Backlight = 100;
	g->g.Contrast = 50;
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->board = 0;

	return TRUE;
}

LLDSPEC	void gdisp_lld_deinit(GDisplay *g) {
	gfxFree(g->priv);
}

LLDSPEC void gdisp_lld_draw_pixel(GDisplay *g) {
	unsigned	pos;

	#if GDISP_NEED_CONTROL
		switch(g->g.Orientation) {
		case GDISP_ROTATE_0:
		default:
			pos = g->p.y * g->g.Width + g->p.x;
			break;
		case GDISP_ROTATE_90:
			pos = (g->g.Width-g->p.x-1) * g->g.Height + g->p.y;
			break;
		case GDISP_ROTATE_180:
			pos = (g->g.Height-g->p.y-1) * g->g.Width + g->g.Width-g->p.x-1;
			break;
		case GDISP_ROTATE_270:
			pos = g->p.x * g->g.Height + g->g.Height-g->p.y-1;
			break;
		}
	#else
		pos = g->p.y * g->g.Width + g->p.x;
	#endif

	((pixmap *)(g)->priv)->pixels[pos] = g->p.color;
}

LLDSPEC	color_t gdisp_lld_get_pixel_color(GDisplay *g) {
	unsigned		pos;

	#if GDISP_NEED_CONTROL
		switch(g->g.Orientation) {
		case GDISP_ROTATE_0:
		default:
			pos = g->p.y * g->g.Width + g->p.x;
			break;
		case GDISP_ROTATE_90:
			pos = (g->g.Width-g->p.x-1) * g->g.Height + g->p.y;
			break;
		case GDISP_ROTATE_180:
			pos = (g->g.Height-g->p.y-1) * g->g.Width + g->g.Width-g->p.x-1;
			break;
		case GDISP_ROTATE_270:
			pos = g->p.x * g->g.Height + g->g.Height-g->p.y-1;
			break;
		}
	#else
		pos = g->p.y * g->g.Width + g->p.x;
	#endif

	return ((pixmap *)(g)->priv)->pixels[pos];
}

#if GDISP_NEED_CONTROL
	LLDSPEC void gdisp_lld_control(GDisplay *g) {
		switch(g->p.x) {
		case GDISP_CONTROL_ORIENTATION:
			if (g->g.Orientation == (orientation_t)g->p.ptr)
				return;
			switch((orientation_t)g->p.ptr) {
				case GDISP_ROTATE_0:
				case GDISP_ROTATE_180:
					if (g->g.Orientation == GDISP_ROTATE_90 || g->g.Orientation == GDISP_ROTATE_270) {
						coord_t		tmp;

						tmp = g->g.Width;
						g->g.Width = g->g.Height;
						g->g.Height = tmp;
					}
					break;
				case GDISP_ROTATE_90:
				case GDISP_ROTATE_270:
					if (g->g.Orientation == GDISP_ROTATE_0 || g->g.Orientation == GDISP_ROTATE_180) {
						coord_t		tmp;

						tmp = g->g.Width;
						g->g.Width = g->g.Height;
						g->g.Height = tmp;
					}
					break;
				default:
					return;
			}
			g->g.Orientation = (orientation_t)g->p.ptr;
			return;
		}
	}
#endif

#endif /* GFX_USE_GDISP */
