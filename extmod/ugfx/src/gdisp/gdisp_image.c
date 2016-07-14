/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GDISP && GDISP_NEED_IMAGE

#include "gdisp_image_support.h"

#if GDISP_NEED_IMAGE_NATIVE
	extern gdispImageError gdispImageOpen_NATIVE(gdispImage *img);
	extern void gdispImageClose_NATIVE(gdispImage *img);
	extern gdispImageError gdispImageCache_NATIVE(gdispImage *img);
	extern gdispImageError gdispGImageDraw_NATIVE(GDisplay *g, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
	extern delaytime_t gdispImageNext_NATIVE(gdispImage *img);
#endif

#if GDISP_NEED_IMAGE_GIF
	extern gdispImageError gdispImageOpen_GIF(gdispImage *img);
	extern void gdispImageClose_GIF(gdispImage *img);
	extern gdispImageError gdispImageCache_GIF(gdispImage *img);
	extern gdispImageError gdispGImageDraw_GIF(GDisplay *g, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
	extern delaytime_t gdispImageNext_GIF(gdispImage *img);
#endif

#if GDISP_NEED_IMAGE_BMP
	extern gdispImageError gdispImageOpen_BMP(gdispImage *img);
	extern void gdispImageClose_BMP(gdispImage *img);
	extern gdispImageError gdispImageCache_BMP(gdispImage *img);
	extern gdispImageError gdispGImageDraw_BMP(GDisplay *g, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
	extern delaytime_t gdispImageNext_BMP(gdispImage *img);
#endif

#if GDISP_NEED_IMAGE_JPG
	extern gdispImageError gdispImageOpen_JPG(gdispImage *img);
	extern void gdispImageClose_JPG(gdispImage *img);
	extern gdispImageError gdispImageCache_JPG(gdispImage *img);
	extern gdispImageError gdispGImageDraw_JPG(GDisplay *g, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
	extern delaytime_t gdispImageNext_JPG(gdispImage *img);
#endif

#if GDISP_NEED_IMAGE_PNG
	extern gdispImageError gdispImageOpen_PNG(gdispImage *img);
	extern void gdispImageClose_PNG(gdispImage *img);
	extern gdispImageError gdispImageCache_PNG(gdispImage *img);
	extern gdispImageError gdispGImageDraw_PNG(GDisplay *g, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
	extern delaytime_t gdispImageNext_PNG(gdispImage *img);
#endif

/* The structure defining the routines for image drawing */
typedef struct gdispImageHandlers {
	gdispImageError	(*open)(gdispImage *img);			/* The open function */
	void			(*close)(gdispImage *img);			/* The close function */
	gdispImageError	(*cache)(gdispImage *img);			/* The cache function */
	gdispImageError	(*draw)(GDisplay *g,
							gdispImage *img,
							coord_t x, coord_t y,
							coord_t cx, coord_t cy,
							coord_t sx, coord_t sy);	/* The draw function */
	delaytime_t		(*next)(gdispImage *img);			/* The next frame function */
} gdispImageHandlers;

static gdispImageHandlers ImageHandlers[] = {
	#if GDISP_NEED_IMAGE_NATIVE
		{	gdispImageOpen_NATIVE,	gdispImageClose_NATIVE,
			gdispImageCache_NATIVE,	gdispGImageDraw_NATIVE,	gdispImageNext_NATIVE,
		},
	#endif
	#if GDISP_NEED_IMAGE_GIF
		{	gdispImageOpen_GIF,		gdispImageClose_GIF,
			gdispImageCache_GIF,	gdispGImageDraw_GIF,	gdispImageNext_GIF,
		},
	#endif
	#if GDISP_NEED_IMAGE_BMP
		{	gdispImageOpen_BMP,		gdispImageClose_BMP,
			gdispImageCache_BMP,	gdispGImageDraw_BMP,	gdispImageNext_BMP,
		},
	#endif
	#if GDISP_NEED_IMAGE_JPG
		{	gdispImageOpen_JPG,		gdispImageClose_JPG,
			gdispImageCache_JPG,	gdispGImageDraw_JPG,	gdispImageNext_JPG,
		},
	#endif
	#if GDISP_NEED_IMAGE_PNG
		{	gdispImageOpen_PNG,		gdispImageClose_PNG,
			gdispImageCache_PNG,	gdispGImageDraw_PNG,	gdispImageNext_PNG,
		},
	#endif
};

void gdispImageInit(gdispImage *img) {
	img->type = GDISP_IMAGE_TYPE_UNKNOWN;
}

gdispImageError gdispImageOpenGFile(gdispImage *img, GFILE *f) {
	gdispImageError err;

	if (!f)
		return GDISP_IMAGE_ERR_NOSUCHFILE;
	img->f = f;
	img->bgcolor = White;
	for(img->fns = ImageHandlers; img->fns < ImageHandlers+sizeof(ImageHandlers)/sizeof(ImageHandlers[0]); img->fns++) {
		err = img->fns->open(img);
		if (err != GDISP_IMAGE_ERR_BADFORMAT) {
			if ((err & GDISP_IMAGE_ERR_UNRECOVERABLE))
				goto unrecoverable;

			// Everything is possible
			return err;
		}

		// Try the next decoder
		gfileSetPos(img->f, 0);
	}

	err = GDISP_IMAGE_ERR_BADFORMAT;
	img->type = GDISP_IMAGE_TYPE_UNKNOWN;

unrecoverable:
	gfileClose(img->f);
	img->f = 0;
	img->flags = 0;
	img->fns = 0;
	img->priv = 0;
	return err;
}

void gdispImageClose(gdispImage *img) {
	if (img->fns)
		img->fns->close(img);
	gfileClose(img->f);
	img->type = GDISP_IMAGE_TYPE_UNKNOWN;
	img->flags = 0;
	img->fns = 0;
	img->priv = 0;
}

bool_t gdispImageIsOpen(gdispImage *img) {
	return img->type != GDISP_IMAGE_TYPE_UNKNOWN && img->fns != 0;
}

void gdispImageSetBgColor(gdispImage *img, color_t bgcolor) {
	img->bgcolor = bgcolor;
}

gdispImageError gdispImageCache(gdispImage *img) {
	if (!img->fns) return GDISP_IMAGE_ERR_BADFORMAT;
	return img->fns->cache(img);
}

gdispImageError gdispGImageDraw(GDisplay *g, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy) {
	if (!img->fns) return GDISP_IMAGE_ERR_BADFORMAT;

	// Check on window
	if (cx <= 0 || cy <= 0) return GDISP_IMAGE_ERR_OK;
	if (sx < 0) sx = 0;
	if (sy < 0) sy = 0;
	if (sx >= img->width || sy >= img->height) return GDISP_IMAGE_ERR_OK;
	if (sx + cx > img->width)  cx = img->width - sx;
	if (sy + cy > img->height) cy = img->height - sy;

	// Draw
	return img->fns->draw(g, img, x, y, cx, cy, sx, sy);
}

delaytime_t gdispImageNext(gdispImage *img) {
	if (!img->fns) return GDISP_IMAGE_ERR_BADFORMAT;
	return img->fns->next(img);
}

// Helper Routines
void *gdispImageAlloc(gdispImage *img, size_t sz) {
	#if GDISP_NEED_IMAGE_ACCOUNTING
		void *ptr;

		ptr = gfxAlloc(sz);
		if (ptr) {
			img->memused += sz;
			if (img->memused > img->maxmemused)
				img->maxmemused = img->memused;
		}
		return ptr;
	#else
		(void) img;
		return gfxAlloc(sz);
	#endif
}

void gdispImageFree(gdispImage *img, void *ptr, size_t sz) {
	#if GDISP_NEED_IMAGE_ACCOUNTING
		gfxFree(ptr);
		img->memused -= sz;
	#else
		(void) img;
		(void) sz;
		gfxFree(ptr);
	#endif
}

#if GFX_CPU_ENDIAN != GFX_CPU_ENDIAN_LITTLE && GFX_CPU_ENDIAN != GFX_CPU_ENDIAN_BIG \
		&& GFX_CPU_ENDIAN != GFX_CPU_ENDIAN_WBDWL && GFX_CPU_ENDIAN != GFX_CPU_ENDIAN_WLDWB

	union wbyteorder_u {
		uint8_t		b[2];
		uint32_t	w;
	};
	union dwbyteorder_u {
		uint8_t		b[4];
		uint32_t	l;
	};

	uint16_t gdispImageH16toLE16(uint16_t w) {
		union wbyteorder_u	we;

		we.w = w;
		return	 (((uint16_t)we.b[0]))|(((uint16_t)we.b[1]) << 8);
	}
	uint16_t gdispImageH16toBE16(uint16_t w) {
		union wbyteorder_u	we;

		we.w = w;
		return	 (((uint16_t)we.b[0]) << 8)|(((uint16_t)we.b[1]));
	}

	uint32_t gdispImageH32toLE32(uint32_t dw) {
		union dwbyteorder_u	we;

		we.l = dw;
		return	 (((uint32_t)we.b[0]))
				|(((uint32_t)we.b[1]) << 8)
				|(((uint32_t)we.b[2]) << 16)
				|(((uint32_t)we.b[3]) << 24);
	}
	uint32_t gdispImageH32toBE32(uint32_t dw) {
		union dwbyteorder_u	we;

		we.l = dw;
		return	 (((uint32_t)we.b[0]) << 24)
				|(((uint32_t)we.b[1]) << 16)
				|(((uint32_t)we.b[2]) << 8)
				|(((uint32_t)we.b[3]));
	}
#endif

#endif /* GFX_USE_GDISP && GDISP_NEED_IMAGE */
