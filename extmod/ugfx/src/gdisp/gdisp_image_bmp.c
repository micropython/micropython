/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GDISP && GDISP_NEED_IMAGE && GDISP_NEED_IMAGE_BMP

#include "gdisp_image_support.h"

/**
 * How big a pixel array to allocate for blitting (in pixels)
 * Bigger is faster but uses more RAM.
 * This must be greater than 40 bytes and 32 pixels as we read our headers into this space as well
 */
#define BLIT_BUFFER_SIZE_BMP	32

typedef struct gdispImagePrivate_BMP {
	uint8_t		bmpflags;
		#define BMP_V2				0x01		// Version 2 (old) header format
		#define BMP_V4				0x02		// Version 4 (alpha support) header format
		#define BMP_PALETTE			0x04		// Uses a palette
		#define	BMP_COMP_RLE		0x08		// Uses RLE compression
		#define	BMP_COMP_MASK		0x10		// Uses mask & shift decoding
		#define BMP_RLE_ENC			0x20		// Currently in RLE encoded run
		#define BMP_RLE_ABS			0x40		// Currently in RLE absolute run
		#define BMP_TOP_TO_BOTTOM	0x80		// Decodes bottom to top line
	uint8_t		bitsperpixel;
#if GDISP_NEED_IMAGE_BMP_1 || GDISP_NEED_IMAGE_BMP_4 || GDISP_NEED_IMAGE_BMP_4_RLE || GDISP_NEED_IMAGE_BMP_8 || GDISP_NEED_IMAGE_BMP_8_RLE
	uint16_t	palsize;
	pixel_t		*palette;
#endif
#if GDISP_NEED_IMAGE_BMP_4_RLE || GDISP_NEED_IMAGE_BMP_8_RLE
	uint16_t	rlerun;
	uint8_t		rlecode;
#endif
#if GDISP_NEED_IMAGE_BMP_16 || GDISP_NEED_IMAGE_BMP_32
	int8_t		shiftred;
	int8_t		shiftgreen;
	int8_t		shiftblue;
	int8_t		shiftalpha;
	uint32_t	maskred;
	uint32_t	maskgreen;
	uint32_t	maskblue;
	uint32_t	maskalpha;
#endif
	size_t		frame0pos;
	pixel_t		*frame0cache;
	pixel_t		buf[BLIT_BUFFER_SIZE_BMP];
	} gdispImagePrivate_BMP;

void gdispImageClose_BMP(gdispImage *img) {
	gdispImagePrivate_BMP *priv;

	priv = (gdispImagePrivate_BMP *)img->priv;
	if (priv) {
#if GDISP_NEED_IMAGE_BMP_1 || GDISP_NEED_IMAGE_BMP_4 || GDISP_NEED_IMAGE_BMP_4_RLE || GDISP_NEED_IMAGE_BMP_8 || GDISP_NEED_IMAGE_BMP_8_RLE
		if (priv->palette)
			gdispImageFree(img, (void *)priv->palette, priv->palsize*sizeof(color_t));
#endif
		if (priv->frame0cache)
			gdispImageFree(img, (void *)priv->frame0cache, img->width*img->height*sizeof(pixel_t));
		gdispImageFree(img, (void *)priv, sizeof(gdispImagePrivate_BMP));
		img->priv = 0;
	}
}

gdispImageError gdispImageOpen_BMP(gdispImage *img) {
	gdispImagePrivate_BMP *priv;
	uint8_t		hdr[2];
	uint16_t	aword;
	uint32_t	adword;
	uint32_t	offsetColorTable;

	/* Read the file identifier */
	if (gfileRead(img->f, hdr, 2) != 2)
		return GDISP_IMAGE_ERR_BADFORMAT;		// It can't be us

	/* Process the BITMAPFILEHEADER structure */

	/**
	 * We only accept Windows V2+ bitmaps.
	 *  - we don't support OS/2 bitmaps, icons, pointers, or Windows V1 bitmaps.
	 */
	if (hdr[0] != 'B' || hdr[1] != 'M')
		return GDISP_IMAGE_ERR_BADFORMAT;		// It can't be us

	/* We know we are a BMP format image */
	img->flags = 0;

	/* Allocate our private area */
	if (!(img->priv = gdispImageAlloc(img, sizeof(gdispImagePrivate_BMP))))
		return GDISP_IMAGE_ERR_NOMEMORY;

	/* Initialise the essential bits in the private area */
	priv = (gdispImagePrivate_BMP *)img->priv;
	priv->frame0cache = 0;
	priv->bmpflags = 0;
#if GDISP_NEED_IMAGE_BMP_1 || GDISP_NEED_IMAGE_BMP_4 || GDISP_NEED_IMAGE_BMP_4_RLE || GDISP_NEED_IMAGE_BMP_8 || GDISP_NEED_IMAGE_BMP_8_RLE
	priv->palette = 0;
#endif

	/* Skip the size field and the 2 reserved fields */
	if (gfileRead(img->f, priv->buf, 8) != 8)
		goto baddatacleanup;

	/* Get the offset to the bitmap data */
	if (gfileRead(img->f, &priv->frame0pos, 4) != 4)
		goto baddatacleanup;
	gdispImageMakeLE32(priv->frame0pos);

	/* Process the BITMAPCOREHEADER structure */

	/* Get the offset to the colour data */
	if (gfileRead(img->f, &offsetColorTable, 4) != 4)
		goto baddatacleanup;
	gdispImageMakeLE32(offsetColorTable);
	offsetColorTable += 14;						// Add the size of the BITMAPFILEHEADER

	// Detect our bitmap version
	if (offsetColorTable == 12+14) {
		priv->bmpflags |= BMP_V2;

		// Read the header
		if (gfileRead(img->f, priv->buf, 12-4) != 12-4)
			goto baddatacleanup;
		// Get the width
		img->width = gdispImageGetAlignedLE16(priv->buf, 0);
		// Get the height
		img->height = gdispImageGetAlignedLE16(priv->buf, 2);
		if (img->height < 0) {
			priv->bmpflags |= BMP_TOP_TO_BOTTOM;
			img->height = -img->height;
		}
		// Get the planes
		aword = gdispImageGetAlignedLE16(priv->buf, 4);
		if (aword != 1)
			goto unsupportedcleanup;
		// Get the bits per pixel
		aword = gdispImageGetAlignedLE16(priv->buf, 6);
		switch(aword) {
#if GDISP_NEED_IMAGE_BMP_1
		case 1:
#endif
#if GDISP_NEED_IMAGE_BMP_4
		case 4:
#endif
#if GDISP_NEED_IMAGE_BMP_8
		case 8:
#endif
#if GDISP_NEED_IMAGE_BMP_1 || GDISP_NEED_IMAGE_BMP_4 || GDISP_NEED_IMAGE_BMP_8
			priv->bmpflags |= BMP_PALETTE;
			priv->palsize = 1<<aword;
			break;
#endif
#if GDISP_NEED_IMAGE_BMP_24
		case 24:
			break;
#endif
		default:
			goto unsupportedcleanup;
		}
		priv->bitsperpixel = aword;

	} else if (offsetColorTable >= 40+14) {
		if (offsetColorTable > 40+14)
			priv->bmpflags |= BMP_V4;

		// Read the header
		if (gfileRead(img->f, priv->buf, 40-4) != 40-4)
			goto baddatacleanup;
		// Get the width
		adword = gdispImageGetAlignedLE32(priv->buf, 0);
		if (adword > 32768)				// This also picks up negative values
			goto unsupportedcleanup;
		img->width = adword;
		// Get the height
		adword = gdispImageGetAlignedLE32(priv->buf, 4);
		if ((int32_t)adword < 0) {		// Negative test
			priv->bmpflags |= BMP_TOP_TO_BOTTOM;
			adword = -adword;
		}
		if (adword > 32768)
			goto unsupportedcleanup;
		img->height = adword;
		// Get the planes
		aword = gdispImageGetAlignedLE16(priv->buf, 8);
		if (aword != 1)
			goto unsupportedcleanup;
		// Get the bits per pixel
		aword = gdispImageGetAlignedLE16(priv->buf, 10);
		switch(aword) {
#if GDISP_NEED_IMAGE_BMP_1
		case 1:
#endif
#if GDISP_NEED_IMAGE_BMP_4 || GDISP_NEED_IMAGE_BMP_4_RLE
		case 4:
#endif
#if GDISP_NEED_IMAGE_BMP_8 || GDISP_NEED_IMAGE_BMP_8_RLE
		case 8:
#endif
#if GDISP_NEED_IMAGE_BMP_1 || GDISP_NEED_IMAGE_BMP_4 || GDISP_NEED_IMAGE_BMP_4_RLE || GDISP_NEED_IMAGE_BMP_8 || GDISP_NEED_IMAGE_BMP_8_RLE
			priv->bmpflags |= BMP_PALETTE;
			priv->palsize = 1<<aword;
			break;
#endif
#if GDISP_NEED_IMAGE_BMP_16
		case 16:
#endif
#if GDISP_NEED_IMAGE_BMP_24
		case 24:
#endif
#if GDISP_NEED_IMAGE_BMP_32
		case 32:
#endif
#if GDISP_NEED_IMAGE_BMP_16 || GDISP_NEED_IMAGE_BMP_24 || GDISP_NEED_IMAGE_BMP_32
			break;
#endif
		default:
			goto unsupportedcleanup;
		}
		priv->bitsperpixel = aword;
		// Get the compression
		adword = gdispImageGetAlignedLE32(priv->buf, 12);
		switch(adword) {
		case 0:					// BI_RGB - uncompressed
			break;
#if GDISP_NEED_IMAGE_BMP_8_RLE
		case 1:					// BI_RLE8 compression
			if (priv->bitsperpixel != 8)
				goto unsupportedcleanup;
			priv->bmpflags |= BMP_COMP_RLE;
			break;
#endif
#if GDISP_NEED_IMAGE_BMP_4_RLE
		case 2:					// BI_RLE4 compression
			if (priv->bitsperpixel != 4)
				goto unsupportedcleanup;
			priv->bmpflags |= BMP_COMP_RLE;
			break;
#endif
#if GDISP_NEED_IMAGE_BMP_16 || GDISP_NEED_IMAGE_BMP_32
		case 3:					// BI_BITFIELDS decoding
			if (priv->bitsperpixel < 16 || priv->bitsperpixel == 24)
				goto unsupportedcleanup;
			priv->bmpflags |= BMP_COMP_MASK;
			if (priv->bmpflags & BMP_V4)		// V4 stored the masks in the header
				offsetColorTable = 40+14;
			break;
#endif
		default:
			goto unsupportedcleanup;
		}
		priv->bitsperpixel = aword;
#if GDISP_NEED_IMAGE_BMP_1 || GDISP_NEED_IMAGE_BMP_4 || GDISP_NEED_IMAGE_BMP_4_RLE || GDISP_NEED_IMAGE_BMP_8 || GDISP_NEED_IMAGE_BMP_8_RLE
		// Get the actual colors used
		adword = gdispImageGetAlignedLE32(priv->buf, 28);
		if (adword && adword < priv->palsize)
			priv->palsize = adword;
#endif
	} else
		goto baddatacleanup;

#if GDISP_NEED_IMAGE_BMP_1 || GDISP_NEED_IMAGE_BMP_4 || GDISP_NEED_IMAGE_BMP_4_RLE || GDISP_NEED_IMAGE_BMP_8 || GDISP_NEED_IMAGE_BMP_8_RLE
	/* Load the palette tables */
	if (priv->bmpflags & BMP_PALETTE) {
		gfileSetPos(img->f, offsetColorTable);

		if (!(priv->palette = (color_t *)gdispImageAlloc(img, priv->palsize*sizeof(color_t))))
			return GDISP_IMAGE_ERR_NOMEMORY;
		if (priv->bmpflags & BMP_V2) {
			for(aword = 0; aword < priv->palsize; aword++) {
				if (gfileRead(img->f, &priv->buf, 3) != 3) goto baddatacleanup;
				priv->palette[aword] = RGB2COLOR(((uint8_t *)priv->buf)[2], ((uint8_t *)priv->buf)[1], ((uint8_t *)priv->buf)[0]);
			}
		} else {
			for(aword = 0; aword < priv->palsize; aword++) {
				if (gfileRead(img->f, &priv->buf, 4) != 4) goto baddatacleanup;
				priv->palette[aword] = RGB2COLOR(((uint8_t *)priv->buf)[2], ((uint8_t *)priv->buf)[1], ((uint8_t *)priv->buf)[0]);
			}
		}

	}
#endif

#if GDISP_NEED_IMAGE_BMP_16 || GDISP_NEED_IMAGE_BMP_32
	/* Load the bit masks */
	if (priv->bmpflags & BMP_COMP_MASK) {
		gfileSetPos(img->f, offsetColorTable);
		if (gfileRead(img->f, &priv->maskred, 4) != 4) goto baddatacleanup;
		gdispImageMakeLE32(priv->maskred);
		if (gfileRead(img->f, &priv->maskgreen, 4) != 4) goto baddatacleanup;
		gdispImageMakeLE32(priv->maskgreen);
		if (gfileRead(img->f, &priv->maskblue, 4) != 4) goto baddatacleanup;
		gdispImageMakeLE32(priv->maskblue);
		if (priv->bmpflags & BMP_V4) {
			if (gfileRead(img->f, &priv->maskalpha, 4) != 4) goto baddatacleanup;
			gdispImageMakeLE32(priv->maskalpha);
		} else
			priv->maskalpha = 0;
	} else if (priv->bitsperpixel == 16) {
		priv->bmpflags |= BMP_COMP_MASK;
		priv->maskred = 0x7C00;
		priv->maskgreen = 0x03E0;
		priv->maskblue = 0x001F;
		priv->maskalpha = 0;
	} else if (priv->bitsperpixel == 32) {
		priv->bmpflags |= BMP_COMP_MASK;
		priv->maskred = 0x00FF0000;
		priv->maskgreen = 0x0000FF00;
		priv->maskblue = 0x000000FF;
		priv->maskalpha = 0;
	}

	/* We need to adjust the masks and calculate the shift values so the result scales 0 -> 255 */
	if (priv->bmpflags & BMP_COMP_MASK) {
		priv->shiftred = 0;
		priv->shiftgreen = 0;
		priv->shiftblue = 0;
		if (priv->maskred) {
			if (priv->maskred < 256)
				for(adword = priv->maskred;  adword < 128; priv->shiftred--, adword <<= 1);
			else
				for(adword = priv->maskred;  adword > 255; priv->shiftred++, adword >>= 1);
		}
		if (priv->maskgreen) {
			if (priv->maskgreen < 256)
				for(adword = priv->maskgreen;  adword < 128; priv->shiftgreen--, adword <<= 1);
			else
				for(adword = priv->maskgreen;  adword > 255; priv->shiftgreen++, adword >>= 1);
		}
		if (priv->maskblue) {
			if (priv->maskblue < 256)
				for(adword = priv->maskblue;  adword < 128; priv->shiftblue--, adword <<= 1);
			else
				for(adword = priv->maskblue;  adword > 255; priv->shiftblue++, adword >>= 1);
		}
		if (priv->maskalpha) {
			if (priv->maskalpha < 256)
				for(adword = priv->maskalpha;  adword < 128; priv->shiftalpha--, adword <<= 1);
			else
				for(adword = priv->maskalpha;  adword > 255; priv->shiftalpha++, adword >>= 1);
		}
	}
#endif

	img->type = GDISP_IMAGE_TYPE_BMP;
	return GDISP_IMAGE_ERR_OK;

baddatacleanup:
	gdispImageClose_BMP(img);				// Clean up the private data area
	return GDISP_IMAGE_ERR_BADDATA;			// Oops - something wrong

unsupportedcleanup:
	gdispImageClose_BMP(img);				// Clean up the private data area
	return GDISP_IMAGE_ERR_UNSUPPORTED;		// Not supported
}

static coord_t getPixels(gdispImage *img, coord_t x) {
	gdispImagePrivate_BMP *	priv;
	color_t *			pc;
	coord_t				len;

	priv = (gdispImagePrivate_BMP *)img->priv;
	pc = priv->buf;
	len = 0;

	switch(priv->bitsperpixel) {
#if GDISP_NEED_IMAGE_BMP_1
	case 1:
		{
		uint8_t		b[4];
		uint8_t		m;

			priv = (gdispImagePrivate_BMP *)img->priv;
			pc = priv->buf;
			len = 0;

			while(x < img->width && len <= BLIT_BUFFER_SIZE_BMP-32) {
				if (gfileRead(img->f, &b, 4) != 4)
					return 0;

				for(m=0x80; m; m >>= 1, pc++)
					pc[0] = priv->palette[(m&b[0]) ? 1 : 0];
				for(m=0x80; m; m >>= 1, pc++)
					pc[0] = priv->palette[(m&b[1]) ? 1 : 0];
				for(m=0x80; m; m >>= 1, pc++)
					pc[0] = priv->palette[(m&b[2]) ? 1 : 0];
				for(m=0x80; m; m >>= 1, pc++)
					pc[0] = priv->palette[(m&b[3]) ? 1 : 0];
				len += 32;
				x += 32;
			}
		}
		return len;
#endif

#if GDISP_NEED_IMAGE_BMP_4 || GDISP_NEED_IMAGE_BMP_4_RLE
	case 4:
	#if GDISP_NEED_IMAGE_BMP_4_RLE
		#if GDISP_NEED_IMAGE_BMP_4
		if (priv->bmpflags & BMP_COMP_RLE)
		#endif
		{
			uint8_t		b[4];

			while(x < img->width) {
				if (priv->bmpflags & BMP_RLE_ENC) {
					while (priv->rlerun && len <= BLIT_BUFFER_SIZE_BMP-2 && x < img->width) {
						*pc++ = priv->palette[priv->rlecode >> 4];
						priv->rlerun--;
						len++;
						x++;
						if (priv->rlerun) {
							*pc++ = priv->palette[priv->rlecode & 0x0F];
							priv->rlerun--;
							len++;
							x++;
						}
					}
					if (priv->rlerun)			// Return if we have more run to do
						return len;
				} else if (priv->bmpflags & BMP_RLE_ABS) {
					while (priv->rlerun && len <= BLIT_BUFFER_SIZE_BMP-2 && x < img->width) {
						if (gfileRead(img->f, &b, 1) != 1)
							return 0;
						*pc++ = priv->palette[b[0] >> 4];
						priv->rlerun--;
						len++;
						x++;
						if (priv->rlerun) {
							*pc++ = priv->palette[b[0] & 0x0F];
							priv->rlerun--;
							len++;
							x++;
						}
					}
					if (priv->rlerun)			// Return if we have more run to do
						return len;
					if ((gfileGetPos(img->f) - priv->frame0pos)&1) {	// Make sure we are on a word boundary
						if (gfileRead(img->f, &b, 1) != 1)
							return 0;
					}
				}

				// We have finished the current run - read a new run
				priv->bmpflags &= ~(BMP_RLE_ENC|BMP_RLE_ABS);

				// There are always at least 2 bytes in an RLE code
				if (gfileRead(img->f, &b, 2) != 2)
					return 0;

				if (b[0]) {								// Encoded mode
					priv->rlerun = b[0];
					priv->rlecode = b[1];
					priv->bmpflags |= BMP_RLE_ENC;
				} else if (b[1] == 0) {					// End of line
					if (x < img->width) {
						priv->rlerun = img->width - x;
						priv->rlecode = 0;					// Who knows what color this should really be
						priv->bmpflags |= BMP_RLE_ENC;
					}
				} else if (b[1] == 1) {					// End of file
					return len;
				} else if (b[1] == 2) {					// Delta x, y
					// There are always at least 2 bytes in an RLE code
					if (gfileRead(img->f, &b, 2) != 2)
						return 0;
					priv->rlerun = b[0] + (uint16_t)b[1] * img->width;
					priv->rlecode = 0;					// Who knows what color this should really be
					priv->bmpflags |= BMP_RLE_ENC;
				} else {								// Absolute mode
					priv->rlerun = b[1];
					priv->bmpflags |= BMP_RLE_ABS;
				}
			}
			return len;
		}
	#endif
	#if GDISP_NEED_IMAGE_BMP_4
		{
			uint8_t		b[4];

			while(x < img->width && len <= BLIT_BUFFER_SIZE_BMP-8) {
				if (gfileRead(img->f, &b, 4) != 4)
					return 0;

				*pc++ = priv->palette[b[0] >> 4];
				*pc++ = priv->palette[b[0] & 0x0F];
				*pc++ = priv->palette[b[1] >> 4];
				*pc++ = priv->palette[b[1] & 0x0F];
				*pc++ = priv->palette[b[2] >> 4];
				*pc++ = priv->palette[b[2] & 0x0F];
				*pc++ = priv->palette[b[3] >> 4];
				*pc++ = priv->palette[b[3] & 0x0F];
				len += 8;
				x += 8;
			}
			return len;
		}
	#endif
#endif

#if GDISP_NEED_IMAGE_BMP_8 || GDISP_NEED_IMAGE_BMP_8_RLE
	case 8:
	#if GDISP_NEED_IMAGE_BMP_8_RLE
		#if GDISP_NEED_IMAGE_BMP_8
		if (priv->bmpflags & BMP_COMP_RLE)
		#endif
		{
			uint8_t		b[4];

			while(x < img->width) {
				if (priv->bmpflags & BMP_RLE_ENC) {
					while (priv->rlerun && len < BLIT_BUFFER_SIZE_BMP && x < img->width) {
						*pc++ = priv->palette[priv->rlecode];
						priv->rlerun--;
						len++;
						x++;
					}
					if (priv->rlerun)			// Return if we have more run to do
						return len;
				} else if (priv->bmpflags & BMP_RLE_ABS) {
					while (priv->rlerun && len < BLIT_BUFFER_SIZE_BMP && x < img->width) {
						if (gfileRead(img->f, &b, 1) != 1)
							return 0;
						*pc++ = priv->palette[b[0]];
						priv->rlerun--;
						len++;
						x++;
					}
					if (priv->rlerun)			// Return if we have more run to do
						return len;
					if ((gfileGetPos(img->f) - priv->frame0pos)&1) {	// Make sure we are on a word boundary
						if (gfileRead(img->f, &b, 1) != 1)
							return 0;
					}
				}

				// We have finished the current run - read a new run
				priv->bmpflags &= ~(BMP_RLE_ENC|BMP_RLE_ABS);

				// There are always at least 2 bytes in an RLE code
				if (gfileRead(img->f, &b, 2) != 2)
					return 0;

				if (b[0]) {								// Encoded mode
					priv->rlerun = b[0];
					priv->rlecode = b[1];
					priv->bmpflags |= BMP_RLE_ENC;
				} else if (b[1] == 0) {					// End of line
					if (x < img->width) {
						priv->rlerun = img->width - x;
						priv->rlecode = 0;					// Who knows what color this should really be
						priv->bmpflags |= BMP_RLE_ENC;
					}
				} else if (b[1] == 1) {					// End of file
					return len;
				} else if (b[1] == 2) {					// Delta x, y
					// There are always at least 2 bytes in an RLE code
					if (gfileRead(img->f, &b, 2) != 2)
						return GDISP_IMAGE_ERR_BADDATA;
					priv->rlerun = b[0] + (uint16_t)b[1] * img->width;
					priv->rlecode = 0;					// Who knows what color this should really be
					priv->bmpflags |= BMP_RLE_ENC;
				} else {								// Absolute mode
					priv->rlerun = b[1];
					priv->bmpflags |= BMP_RLE_ABS;
				}
			}
			return len;
		}
	#endif
	#if GDISP_NEED_IMAGE_BMP_8
		{
			uint8_t		b[4];

			while(x < img->width && len <= BLIT_BUFFER_SIZE_BMP-4) {
				if (gfileRead(img->f, &b, 4) != 4)
					return 0;

				*pc++ = priv->palette[b[0]];
				*pc++ = priv->palette[b[1]];
				*pc++ = priv->palette[b[2]];
				*pc++ = priv->palette[b[3]];
				len += 4;
				x += 4;
			}
			return len;
		}
	#endif
#endif

#if GDISP_NEED_IMAGE_BMP_16
	case 16:
		{
		uint16_t	w[2];
		color_t		r, g, b;

			while(x < img->width && len <= BLIT_BUFFER_SIZE_BMP-2) {
				if (gfileRead(img->f, &w, 4) != 4)
					return 0;
				gdispImageMakeLE16(w[0]);
				gdispImageMakeLE16(w[1]);
				if (priv->shiftred < 0)
					r = (color_t)((w[0] & priv->maskred) << -priv->shiftred);
				else
					r = (color_t)((w[0] & priv->maskred) >> priv->shiftred);
				if (priv->shiftgreen < 0)
					g = (color_t)((w[0] & priv->maskgreen) << -priv->shiftgreen);
				else
					g = (color_t)((w[0] & priv->maskgreen) >> priv->shiftgreen);
				if (priv->shiftblue < 0)
					b = (color_t)((w[0] & priv->maskblue) << -priv->shiftblue);
				else
					b = (color_t)((w[0] & priv->maskblue) >> priv->shiftblue);
				/* We don't support alpha yet */
				*pc++ = RGB2COLOR(r, g, b);
				if (priv->shiftred < 0)
					r = (color_t)((w[1] & priv->maskred) << -priv->shiftred);
				else
					r = (color_t)((w[1] & priv->maskred) >> priv->shiftred);
				if (priv->shiftgreen < 0)
					g = (color_t)((w[1] & priv->maskgreen) << -priv->shiftgreen);
				else
					g = (color_t)((w[1] & priv->maskgreen) >> priv->shiftgreen);
				if (priv->shiftblue < 0)
					b = (color_t)((w[1] & priv->maskblue) << -priv->shiftblue);
				else
					b = (uint8_t)((w[1] & priv->maskblue) >> priv->shiftblue);
				/* We don't support alpha yet */
				*pc++ = RGB2COLOR(r, g, b);
				x += 2;
				len += 2;
			}
		}
		return len;
#endif

#if GDISP_NEED_IMAGE_BMP_24
	case 24:
		{
		uint8_t		b[3];

			while(x < img->width && len < BLIT_BUFFER_SIZE_BMP) {
				if (gfileRead(img->f, &b, 3) != 3)
					return 0;
				*pc++ = RGB2COLOR(b[2], b[1], b[0]);
				x++;
				len++;
			}

			if (x >= img->width) {
				// Make sure we have read a multiple of 4 bytes for the line
				if ((x & 3) && gfileRead(img->f, &b, x & 3) != (x & 3))
					return 0;
			}
		}
		return len;
#endif

#if GDISP_NEED_IMAGE_BMP_32
	case 32:
		{
		uint32_t	dw;
		color_t		r, g, b;

			while(x < img->width && len < BLIT_BUFFER_SIZE_BMP) {
				if (gfileRead(img->f, &dw, 4) != 4)
					return 0;
				gdispImageMakeLE32(dw);
				if (priv->shiftred < 0)
					r = (color_t)((dw & priv->maskred) << -priv->shiftred);
				else
					r = (color_t)((dw & priv->maskred) >> priv->shiftred);
				if (priv->shiftgreen < 0)
					g = (color_t)((dw & priv->maskgreen) << -priv->shiftgreen);
				else
					g = (color_t)((dw & priv->maskgreen) >> priv->shiftgreen);
				if (priv->shiftblue < 0)
					b = (color_t)((dw & priv->maskblue) << -priv->shiftblue);
				else
					b = (color_t)((dw & priv->maskblue) >> priv->shiftblue);
				/* We don't support alpha yet */
				*pc++ = RGB2COLOR(r, g, b);
				x++;
				len++;
			}
		}
		return len;
#endif

	default:
		return len;
	}
}

gdispImageError gdispImageCache_BMP(gdispImage *img) {
	gdispImagePrivate_BMP *	priv;
	color_t *			pcs;
	color_t *			pcd;
	coord_t				pos, x, y;
	size_t				len;

	/* If we are already cached - just return OK */
	priv = (gdispImagePrivate_BMP *)img->priv;
	if (priv->frame0cache)
		return GDISP_IMAGE_ERR_OK;

	/* We need to allocate the cache */
	len = img->width * img->height * sizeof(pixel_t);
	priv->frame0cache = (pixel_t *)gdispImageAlloc(img, len);
	if (!priv->frame0cache)
		return GDISP_IMAGE_ERR_NOMEMORY;

	/* Read the entire bitmap into cache */
	gfileSetPos(img->f, priv->frame0pos);
#if GDISP_NEED_IMAGE_BMP_4_RLE || GDISP_NEED_IMAGE_BMP_8_RLE
	priv->rlerun = 0;
	priv->rlecode = 0;
#endif

	pcs = priv->buf;				// This line is just to prevent a compiler warning.

	if (priv->bmpflags & BMP_TOP_TO_BOTTOM) {
		for(y = 0, pcd = priv->frame0cache; y < img->height; y++) {
			x = 0; pos = 0;
			while(x < img->width) {
				if (!pos) {
					if (!(pos = getPixels(img, x)))
						return GDISP_IMAGE_ERR_BADDATA;
					pcs = priv->buf;
				}
				*pcd++ = *pcs++;
				x++; pos--;
			}
		}
	} else {
		for(y = img->height-1, pcd = priv->frame0cache + img->width*(img->height-1); y >= 0; y--, pcd -= 2*img->width) {
			x = 0; pos = 0;
			while(x < img->width) {
				if (!pos) {
					if (!(pos = getPixels(img, x)))
						return GDISP_IMAGE_ERR_BADDATA;
					pcs = priv->buf;
				}
				*pcd++ = *pcs++;
				x++; pos--;
			}
		}
	}

	return GDISP_IMAGE_ERR_OK;
}

gdispImageError gdispGImageDraw_BMP(GDisplay *g, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy) {
	gdispImagePrivate_BMP *	priv;
	coord_t				mx, my;
	coord_t				pos, len, st;

	priv = (gdispImagePrivate_BMP *)img->priv;

	/* Check some reasonableness */
	if (sx >= img->width || sy >= img->height) return GDISP_IMAGE_ERR_OK;
	if (sx + cx > img->width) cx = img->width - sx;
	if (sy + cy > img->height) cy = img->height - sy;

	/* Draw from the image cache - if it exists */
	if (priv->frame0cache) {
		gdispGBlitArea(g, x, y, cx, cy, sx, sy, img->width, priv->frame0cache);
		return GDISP_IMAGE_ERR_OK;
	}

	/* Start decoding from the beginning */
	gfileSetPos(img->f, priv->frame0pos);
#if GDISP_NEED_IMAGE_BMP_4_RLE || GDISP_NEED_IMAGE_BMP_8_RLE
	priv->rlerun = 0;
	priv->rlecode = 0;
#endif

	if (priv->bmpflags & BMP_TOP_TO_BOTTOM) {
		for(my = 0; my < img->height; my++) {
			mx = 0;
			while(mx < img->width) {
				if (!(pos = getPixels(img, mx)))
					return GDISP_IMAGE_ERR_BADDATA;
				if (my >= sy && my < sy+cy && mx < sx+cx && mx+pos >= sx) {
					st = mx < sx ? sx - mx : 0;
					len = pos-st;
					if (mx+st+len > sx+cx) len = sx+cx-mx-st;
					if (len == 1)
						gdispGDrawPixel(g, x+mx+st-sx, y+my-sy, priv->buf[st]);
					else
						gdispGBlitArea(g, x+mx+st-sx, y+my-sy, len, 1, st, 0, pos, priv->buf);
				}
				mx += pos;
			}
		}
	} else {
		for(my = img->height-1; my >= 0; my--) {
			mx = 0;
			while(mx < img->width) {
				if (!(pos = getPixels(img, mx)))
					return GDISP_IMAGE_ERR_BADDATA;
				if (my >= sy && my < sy+cy && mx < sx+cx && mx+pos >= sx) {
					st = mx < sx ? sx - mx : 0;
					len = pos-st;
					if (mx+st+len > sx+cx) len = sx+cx-mx-st;
					if (len == 1)
						gdispGDrawPixel(g, x+mx+st-sx, y+my-sy, priv->buf[st]);
					else
						gdispGBlitArea(g, x+mx+st-sx, y+my-sy, len, 1, st, 0, pos, priv->buf);
				}
				mx += pos;
			}
		}
	}

	return GDISP_IMAGE_ERR_OK;
}

delaytime_t gdispImageNext_BMP(gdispImage *img) {
	(void) img;

	/* No more frames/pages */
	return TIME_INFINITE;
}

#endif /* GFX_USE_GDISP && GDISP_NEED_IMAGE && GDISP_NEED_IMAGE_BMP */
