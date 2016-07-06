/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GDISP && GDISP_NEED_IMAGE && GDISP_NEED_IMAGE_PNG

#include "gdisp_image_support.h"

/**
 * How big a pixel array to allocate for blitting the image to the display (in pixels)
 * Bigger is faster but uses more RAM.
 */
#define PNG_BLIT_BUFFER_SIZE	32
/**
 * How big a byte array to use for input file buffer
 * Bigger is faster but uses more RAM.
 * Must be more than 8 bytes
 */
#define PNG_FILE_BUFFER_SIZE	8
/**
 * How big a byte array to use for inflate decompression
 * Bigger is faster but uses more RAM.
 * Must be >= 32768 due to the PNG 32K sliding window
 * More efficient code is generated if it is a power of 2
 */
#define PNG_Z_BUFFER_SIZE		32768

/*-----------------------------------------------------------------
 * Structure definitions
 *---------------------------------------------------------------*/

struct PNG_decode;

// PNG info (comes from the PNG header)
typedef struct PNG_info {
	uint8_t		flags;								// Flags (global)
		#define PNG_FLG_HEADERDONE			0x01		// The header has been processed
		#define PNG_FLG_TRANSPARENT			0x02		// Has transparency
		#define PNG_FLG_INTERLACE			0x04		// Is Interlaced
		#define PNG_FLG_BACKGROUND			0x08		// Has a specified background color
	uint8_t		bitdepth;							// 1, 2, 4, 8, 16
	uint8_t		mode;								// The PNG color-mode
		#define PNG_COLORMODE_GRAY			0x00		// Grayscale
		#define PNG_COLORMODE_RGB			0x02		// RGB
		#define PNG_COLORMODE_PALETTE		0x03		// Pallete
		#define PNG_COLORMODE_GRAYALPHA		0x04		// Grayscale with Alpha
		#define PNG_COLORMODE_RGBA			0x06		// RGBA
	uint8_t		bpp;								// Bits per pixel

	uint8_t		*cache;								// The image cache
	unsigned	cachesz;							// The image cache size

	void 		(*out)(struct PNG_decode *);		// The scan line output function

	#if GDISP_NEED_IMAGE_PNG_BACKGROUND
		color_t		bg;								// The background color
	#endif
	#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
		uint16_t	trans_r;						// Red/grayscale component of the transparent color (PNG_COLORMODE_GRAY and PNG_COLORMODE_RGB only)
		uint16_t	trans_g;						// Green component of the transparent color (PNG_COLORMODE_RGB only)
		uint16_t	trans_b;						// Blue component of the transparent color (PNG_COLORMODE_RGB only)
	#endif
	#if GDISP_NEED_IMAGE_PNG_PALETTE_124 || GDISP_NEED_IMAGE_PNG_PALETTE_8
		uint16_t	palsize;						// palette size in number of colors
		uint8_t 	*palette;						// palette in RGBA RGBA... order (4 bytes per entry - PNG_COLORMODE_PALETTE only)
	#endif
	} PNG_info;

// Handle the PNG file stream
typedef struct PNG_input {
	GFILE *		f;								// The gfile to retrieve data from
	unsigned	buflen;							// The number of bytes left in the buffer
	uint8_t		*pbuf;							// The pointer to the next byte
	uint32_t	chunklen;						// The number of bytes left in the current PNG chunk
	uint32_t	chunknext;						// The file position of the next PNG chunk
	uint8_t		buf[PNG_FILE_BUFFER_SIZE];		// Must be a minimum of 8 bytes to hold a chunk header
	} PNG_input;

// Handle the display output and windowing
typedef struct PNG_output {
	GDisplay	*g;
	coord_t		x, y;
	coord_t		cx, cy;
	coord_t		sx, sy;
	coord_t		ix, iy;
	unsigned	cnt;
	pixel_t		buf[PNG_BLIT_BUFFER_SIZE];
	} PNG_output;

// Handle the PNG scan line filter
typedef struct PNG_filter {
	unsigned	scanbytes;
	unsigned	bytewidth;
	uint8_t		*line;
	uint8_t		*prev;
	} PNG_filter;

// Handle the PNG inflate decompression
typedef struct PNG_zTree {
	uint16_t table[16];			// Table of code length counts
	uint16_t trans[288];		// Code to symbol translation table
	} PNG_zTree;

typedef struct PNG_zinflate {
	uint8_t		data;					// The current input stream data byte
	uint8_t		bits;					// The number of bits left in the data byte
	uint8_t		flags;					// Decompression flags
	#define PNG_ZFLG_EOF			0x01	// No more input data
	#define PNG_ZFLG_FINAL			0x02	// This is the final block
	#define PNG_ZFLG_RESUME_MASK	0x0C	// The mask of bits for the resume state
	#define PNG_ZFLG_RESUME_NEW		0x00	// Process a new block
	#define PNG_ZFLG_RESUME_COPY	0x04	// Resume a byte copy from the input stream (length in tmp)
	#define PNG_ZFLG_RESUME_INFLATE	0x08	// Resume using the specified symbol (symbol in tmp[0])
	#define PNG_ZFLG_RESUME_OFFSET	0x0C	// Resume a byte offset copy from the buffer (length and offset in tmp)

	unsigned		bufpos;				// The current buffer output position
	unsigned		bufend;				// The current buffer end position (wraps)

	PNG_zTree	ltree;					// The dynamic length tree
	PNG_zTree	dtree;					// The dynamic distance tree
	uint8_t		tmp[288+32];			// Temporary space for decoding dynamic trees and other temporary uses
	uint8_t		buf[PNG_Z_BUFFER_SIZE];	// The decoding buffer and sliding window
	} PNG_zinflate;

// Put all the decoding structures together.
// Note this is immediately followed by 2 scan lines of uncompressed image data for filtering (dynamic size).
typedef struct PNG_decode {
	gdispImage		*img;
	PNG_info		*pinfo;
	PNG_input		i;
	PNG_output		o;
	PNG_filter		f;
	PNG_zinflate	z;
	} PNG_decode;

/*-----------------------------------------------------------------
 * PNG input data stream functions
 *---------------------------------------------------------------*/

// Input initialization
static void PNG_iInit(PNG_decode *d) {
	if (d->pinfo->cache) {
		d->i.pbuf = d->pinfo->cache;
		d->i.buflen = d->pinfo->cachesz;
		d->i.f = 0;
	} else {
		d->i.buflen = 0;
		d->i.chunklen = 0;
		d->i.chunknext = 8;
		d->i.f = d->img->f;
	}
}

// Load the next byte of image data from the PNG file
static bool_t PNG_iLoadData(PNG_decode *d) {
	uint32_t	sz;

	// Is there data still left in the buffer?
	if (d->i.buflen)
		return TRUE;

	// If we are cached then we have no more data
	if (!d->i.f)
		return FALSE;

	// Have we finished the current chunk?
	if (!d->i.chunklen) {
		while(1) {
			// Find a new chunk
			gfileSetPos(d->i.f, d->i.chunknext);
			if (gfileRead(d->i.f, d->i.buf, 8) != 8)
				return FALSE;

			// Calculate the chunk length and next chunk
			d->i.chunklen = gdispImageGetAlignedBE32(d->i.buf, 0);
			d->i.chunknext += d->i.chunklen + 12;

			// Process only image data chunks
			switch (gdispImageGetAlignedBE32(d->i.buf, 4)) {
			case 0x49444154:		// "IDAT" - Image Data
				if (!d->i.chunklen)
					break;
				goto gotchunk;
			case 0x49454E44:		// "IEND"	- All done
				return FALSE;
			}
		}
	}

gotchunk:

	// Try to read data some from the chunk
	sz = d->i.chunklen;
	if (sz > PNG_FILE_BUFFER_SIZE)
		sz = PNG_FILE_BUFFER_SIZE;
	if (gfileRead(d->i.f, d->i.buf, sz) != sz)
		return FALSE;
	d->i.chunklen -= sz;
	d->i.buflen = sz;
	d->i.pbuf = d->i.buf;
	return TRUE;
}

// Get the last loaded byte of image data from the PNG file
static uint8_t PNG_iGetByte(PNG_decode *d) {
	d->i.buflen--;
	return *d->i.pbuf++;
}

/*-----------------------------------------------------------------
 * Display output and windowing functions
 *---------------------------------------------------------------*/

// Initialize the display output window
static void PNG_oInit(PNG_output *o, GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy) {
	o->g = g;
	o->x = x;
	o->y = y;
	o->cx = cx;
	o->cy = cy;
	o->sx = sx;
	o->sy = sy;
	o->ix = o->iy = 0;
	o->cnt = 0;
}

// Flush the output buffer to the display
static void PNG_oFlush(PNG_output *o) {
	switch(o->cnt) {
	case 0:		return;
	case 1:		gdispGDrawPixel(o->g, o->x+o->ix-o->sx, o->y+o->iy-o->sy, o->buf[0]); 						break;
	default:	gdispGBlitArea(o->g, o->x+o->ix-o->sx, o->y+o->iy-o->sy, o->cnt, 1, 0, 0, o->cnt, o->buf);	break;
	}
	o->ix += o->cnt;
	o->cnt = 0;
}

// Start a new image line
static bool_t PNG_oStartY(PNG_output *o, coord_t y) {
	if (y < o->sy || y >= o->sy+o->cy)
		return FALSE;
	o->ix = 0;
	o->iy = y;
	return TRUE;
}

// Feed a pixel color to the display buffer
static void PNG_oColor(PNG_output *o, color_t c) {
	// Is it in the window
	if (o->ix+(coord_t)o->cnt < o->sx || o->ix+(coord_t)o->cnt >= o->sx+o->cx) {
		// No - just skip the pixel
		PNG_oFlush(o);
		o->ix++;
		return;
	}

	// Is the buffer full
	if (o->cnt >= sizeof(o->buf)/sizeof(o->buf[0]))
		PNG_oFlush(o);

	// Save the pixel
	o->buf[o->cnt++] = c;
}

#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY || GDISP_NEED_IMAGE_PNG_ALPHACLIFF > 0
	// Feed a transparent pixel to the display buffer
	static void PNG_oTransparent(PNG_output *o) {
		// Flush any existing pixels
		PNG_oFlush(o);

		// Just skip the pixel
		o->ix++;
	}
#endif

/*-----------------------------------------------------------------
 * Inflate uncompress functions
 *---------------------------------------------------------------*/

// Wrap the zInflate buffer position (after increment)
#if (PNG_Z_BUFFER_SIZE & ~(PNG_Z_BUFFER_SIZE-1)) == PNG_Z_BUFFER_SIZE
	#define WRAP_ZBUF(x)	{ x &= PNG_Z_BUFFER_SIZE-1; }
#else
	#warning "PNG: PNG_Z_BUFFER_SIZE is more efficient as a power of 2"
	#define WRAP_ZBUF(x)	{ if (x >= PNG_Z_BUFFER_SIZE) x = 0; }
#endif

// Initialize the inflate decompressor
static void PNG_zInit(PNG_zinflate *z) {
	z->bits = 0;
	z->flags = 0;
	z->bufpos = z->bufend = 0;
}

// Get the inflate header (slightly customized for PNG validity testing)
static bool_t PNG_zGetHeader(PNG_decode *d) {
	if (!PNG_iLoadData(d))
		return FALSE;
	d->z.tmp[0] = PNG_iGetByte(d);
	if (!PNG_iLoadData(d))
		return FALSE;
	d->z.tmp[1] = PNG_iGetByte(d);
	if (gdispImageGetAlignedBE16(d->z.tmp, 0) % 31 != 0				// Must be modulo 31, the FCHECK value is made that way
			|| (d->z.tmp[0] & 0x0F) != 8 || (d->z.tmp[0] & 0x80)	// only method 8: inflate 32k sliding window
			|| (d->z.tmp[1] & 0x20))								// no preset dictionary
		return FALSE;
	return TRUE;
}

// Get a bit from the input (treated as a LSB first stream)
static unsigned PNG_zGetBit(PNG_decode *d) {
	unsigned	bit;

	// Check for EOF
	if ((d->z.flags & PNG_ZFLG_EOF))
		return 1;

	// Check if data is empty
	if (!d->z.bits) {
		if (!PNG_iLoadData(d)) {
			d->z.flags |= PNG_ZFLG_EOF;
			return 1;
		}
		d->z.data = PNG_iGetByte(d);
		d->z.bits = 8;
	}

	// Get the next bit
	d->z.bits--;
	bit = d->z.data & 0x01;
	d->z.data >>= 1;
	return bit;
}

// Get multiple bits from the input (treated as a LSB first stream with bit order retained)
static unsigned PNG_zGetBits(PNG_decode *d, unsigned num) {
	unsigned val;
	unsigned limit;
	unsigned mask;

	val = 0;
	limit = 1 << num;

	for (mask = 1; mask < limit; mask <<= 1)
		if (PNG_zGetBit(d))
			val += mask;
	return val;
}

// Build an inflate dynamic tree using a string of byte lengths
static void PNG_zBuildTree(PNG_zTree *t, const uint8_t *lengths, unsigned num) {
	unsigned		i, sum;
	uint16_t		offs[16];

	for (i = 0; i < 16; ++i)
		t->table[i] = 0;
	for (i = 0; i < num; ++i)
		t->table[lengths[i]]++;

	t->table[0] = 0;

	for (sum = 0, i = 0; i < 16; ++i) {
		offs[i] = sum;
		sum += t->table[i];
	}
	for (i = 0; i < num; ++i) {
		if (lengths[i])
			t->trans[offs[lengths[i]]++] = i;
	}
}

// Get an inflate decode symbol
static uint16_t PNG_zGetSymbol(PNG_decode *d, PNG_zTree *t) {
	int			sum, cur;
	unsigned	len;

	sum = cur = 0;
	len = 0;
	do {
		cur <<= 1;
		cur += PNG_zGetBit(d);
		if ((d->z.flags & PNG_ZFLG_EOF))
			return 0;
		len++;

		sum += t->table[len];
		cur -= t->table[len];
	} while (cur >= 0);

	return t->trans[sum + cur];
}

// Build inflate fixed length and distance trees
static void PNG_zBuildFixedTrees(PNG_decode *d) {
	unsigned	i;

	for (i = 0; i < 16; ++i)	d->z.ltree.table[i] = 0;
	d->z.ltree.table[7] = 24;
	d->z.ltree.table[8] = 152;
	d->z.ltree.table[9] = 112;
	for (i = 0; i < 24; ++i)	d->z.ltree.trans[i] = 256 + i;
	for (i = 0; i < 144; ++i)	d->z.ltree.trans[24 + i] = i;
	for (i = 0; i < 8; ++i)		d->z.ltree.trans[24 + 144 + i] = 280 + i;
	for (i = 0; i < 112; ++i)	d->z.ltree.trans[24 + 144 + 8 + i] = 144 + i;

	for (i = 0; i < 16; ++i)	d->z.dtree.table[i] = 0;
	d->z.dtree.table[5] = 32;
	for (i = 0; i < 32; ++i)	d->z.dtree.trans[i] = i;
	for ( ; i < 288; ++i)		d->z.dtree.trans[i] = 0;
}

// Build inflate dynamic length and distance trees
static bool_t PNG_zDecodeTrees(PNG_decode *d) {
	static const uint8_t IndexLookup[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
	unsigned	hlit, hdist, hclen;
	unsigned	i, num;
	uint16_t	symbol;
	uint8_t		val;

	hlit	= PNG_zGetBits(d, 5) + 257;		// 257 - 286
	hdist	= PNG_zGetBits(d, 5) + 1;		// 1 - 32
	hclen	= PNG_zGetBits(d, 4) + 4;		// 4 - 19

	if ((d->z.flags & PNG_ZFLG_EOF))
		return FALSE;

	for (i = 0; i < 19; ++i)
		d->z.tmp[i] = 0;

	// Get code lengths for the code length alphabet
	for (i = 0; i < hclen; ++i)
		d->z.tmp[IndexLookup[i]] = PNG_zGetBits(d, 3);

	if ((d->z.flags & PNG_ZFLG_EOF))
		return FALSE;

	// Build the code length tree
	PNG_zBuildTree(&d->z.ltree, d->z.tmp, 19);

	// Decode code lengths
	for (num = 0; num < hlit + hdist; ) {
		symbol = PNG_zGetSymbol(d, &d->z.ltree);
		if ((d->z.flags & PNG_ZFLG_EOF))
			return FALSE;

		switch(symbol) {
		case 16:		// Copy the previous code length 3-6 times
			val = d->z.tmp[num - 1];
			for (i = PNG_zGetBits(d, 2) + 3; i; i--)
				d->z.tmp[num++] = val;
			break;
		case 17:		// Repeat code length 0 for 3-10 times
			for (i = PNG_zGetBits(d, 3) + 3; i; i--)
				d->z.tmp[num++] = 0;
			break;
		case 18:		// Repeat code length 0 for 11-138 times
			for (i = PNG_zGetBits(d, 7) + 11; i; i--)
				d->z.tmp[num++] = 0;
			break;
		default:		// symbols 0-15 are the actual code lengths
			d->z.tmp[num++] = symbol;
			break;
		}
	}

	// Build the trees
	PNG_zBuildTree(&d->z.ltree, d->z.tmp, hlit);
	PNG_zBuildTree(&d->z.dtree, d->z.tmp + hlit, hdist);
	return TRUE;
}

// Copy bytes from the input stream. Completing the copy completes the block.
static bool_t PNG_zCopyInput(PNG_decode *d, unsigned length) {
	// Copy the block
	while(length--) {
		if (!PNG_iLoadData(d)) {				// EOF?
			d->z.flags |= PNG_ZFLG_EOF;
			return FALSE;
		}
		d->z.buf[d->z.bufend++] = PNG_iGetByte(d);
		WRAP_ZBUF(d->z.bufend);
		if (d->z.bufend == d->z.bufpos) {		// Buffer full?
			d->z.flags = (d->z.flags & ~PNG_ZFLG_RESUME_MASK) | PNG_ZFLG_RESUME_COPY;
			((unsigned *)d->z.tmp)[0] = length;
			return TRUE;
		}
	}

	// The block is done
	d->z.flags = (d->z.flags & ~PNG_ZFLG_RESUME_MASK) | PNG_ZFLG_RESUME_NEW;
	return TRUE;
}

// Copy an uncompressed inflate block into the output
static bool_t PNG_zUncompressedBlock(PNG_decode *d) {
	unsigned	length;

	// This block works on byte boundaries
	d->z.bits = 0;

	// Get 4 byte header
	for (length = 0; length < 4; length++) {
		if (!PNG_iLoadData(d)) {			// EOF?
			d->z.flags |= PNG_ZFLG_EOF;
			return FALSE;
		}
		d->z.tmp[length] = PNG_iGetByte(d);
	}

	// Get length
	length = gdispImageGetAlignedLE16(d->z.tmp, 0);

	// Check length
	if ((uint16_t)length != (uint16_t)~gdispImageGetAlignedLE16(d->z.tmp, 2)) {
		d->z.flags |= PNG_ZFLG_EOF;
		return FALSE;
	}

	// Copy the block
	return PNG_zCopyInput(d, length);
}

// Inflate a compressed inflate block into the output
static bool_t PNG_zInflateBlock(PNG_decode *d) {
	static const uint8_t	lbits[30]	= { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 6 };
	static const uint16_t	lbase[30]	= { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 323 };
	static const uint8_t	dbits[30]	= { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
	static const uint16_t	dbase[30]	= { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
	unsigned	length, dist, offset;
	uint16_t	symbol;

	while(1) {
		symbol = PNG_zGetSymbol(d, &d->z.ltree);							// EOF?
		if ((d->z.flags & PNG_ZFLG_EOF))
			goto iserror;

		// Is the block done?
		if (symbol == 256) {
			d->z.flags = (d->z.flags & ~PNG_ZFLG_RESUME_MASK) | PNG_ZFLG_RESUME_NEW;
			return TRUE;
		}

		if (symbol < 256) {
			// The symbol is the data
			d->z.buf[d->z.bufend++] = (uint8_t)symbol;
			WRAP_ZBUF(d->z.bufend);
			if (d->z.bufend == d->z.bufpos) {								// Buffer full?
				d->z.flags = (d->z.flags & ~PNG_ZFLG_RESUME_MASK) | PNG_ZFLG_RESUME_INFLATE;
				return TRUE;
			}
			continue;
		}

		// Shift the symbol down into an index
		symbol -= 257;

		if (symbol >= sizeof(lbits))										// Bad index?
			goto iserror;

		// Get more bits from length code
		length = PNG_zGetBits(d, lbits[symbol]) + lbase[symbol];
		if ((d->z.flags & PNG_ZFLG_EOF) || length >= PNG_Z_BUFFER_SIZE)		// Bad length?
			goto iserror;

		// Get the distance code
		dist = PNG_zGetSymbol(d, &d->z.dtree);								// Bad distance?
		if ((d->z.flags & PNG_ZFLG_EOF) || dist >= sizeof(dbits))
			goto iserror;

		// Get more bits from distance code
		offset = PNG_zGetBits(d, dbits[dist]) + dbase[dist];
		if ((d->z.flags & PNG_ZFLG_EOF) || offset >= PNG_Z_BUFFER_SIZE)		// Bad offset?
			goto iserror;

		// Work out the source buffer position allowing for wrapping
		if (offset > d->z.bufend)
			offset -= PNG_Z_BUFFER_SIZE;
		offset = d->z.bufend - offset;

		// Copy the matching string
		while (length--) {
			d->z.buf[d->z.bufend++] = d->z.buf[offset++];
			WRAP_ZBUF(d->z.bufend);
			WRAP_ZBUF(offset);
			if (d->z.bufend == d->z.bufpos) {								// Buffer full?
				d->z.flags = (d->z.flags & ~PNG_ZFLG_RESUME_MASK) | PNG_ZFLG_RESUME_OFFSET;
				((unsigned *)d->z.tmp)[0] = length;
				((unsigned *)d->z.tmp)[1] = offset;
				return TRUE;
			}
		}
	}

iserror:
	d->z.flags |= PNG_ZFLG_EOF;
	return FALSE;
}

// Start a new uncompressed/inflate block
static bool_t PNG_zStartBlock(PNG_decode *d) {
	// Check for previous error, EOF or no more blocks
	if ((d->z.flags & (PNG_ZFLG_EOF|PNG_ZFLG_FINAL)))
		return FALSE;

	// Is this the final inflate block?
	if (PNG_zGetBit(d))
		d->z.flags |= PNG_ZFLG_FINAL;

	// Get the block type
	switch (PNG_zGetBits(d, 2)) {

	case 0:			// Decompress uncompressed block
		if (!PNG_zUncompressedBlock(d))
			return FALSE;
		break;

	case 1:			// Decompress block with fixed huffman trees
		PNG_zBuildFixedTrees(d);
		if (!PNG_zInflateBlock(d))
			return FALSE;
		break;

	case 2:			// Decompress block with dynamic huffman trees
		if (!PNG_zDecodeTrees(d))
			return FALSE;
		if (!PNG_zInflateBlock(d))
			return FALSE;
		break;

	default:		// Bad block type
		// Mark it as an error
		d->z.flags |= PNG_ZFLG_EOF;
		return FALSE;
	}
	return TRUE;
}

// Resume an offset copy
static bool_t PNG_zResumeOffset(PNG_decode *d, unsigned length, unsigned offset) {
	// Copy the matching string
	while (length--) {
		d->z.buf[d->z.bufend++] = d->z.buf[offset++];
		WRAP_ZBUF(d->z.bufend);
		WRAP_ZBUF(offset);
		if (d->z.bufend == d->z.bufpos) {						// Buffer full?
			d->z.flags = (d->z.flags & ~PNG_ZFLG_RESUME_MASK) | PNG_ZFLG_RESUME_OFFSET;
			((unsigned *)d->z.tmp)[0] = length;
			((unsigned *)d->z.tmp)[1] = offset;
			return TRUE;
		}
	}
	return PNG_zInflateBlock(d);
}

// Get a fully decompressed byte from the inflate data stream
static uint8_t PNG_zGetByte(PNG_decode *d) {
	uint8_t		data;

	// Do we have any data in the buffers
	while (d->z.bufpos == d->z.bufend) {

		// No, get some more data

		switch((d->z.flags & PNG_ZFLG_RESUME_MASK)) {
		case PNG_ZFLG_RESUME_NEW:			// Start a new inflate block
			if (!PNG_zStartBlock(d))
				return 0xFF;
			break;
		case PNG_ZFLG_RESUME_COPY:			// Resume uncompressed block copy for length bytes
			if (!PNG_zCopyInput(d, ((unsigned *)d->z.tmp)[0]))
				return 0xFF;
			break;
		case PNG_ZFLG_RESUME_INFLATE:		// Resume compressed block
			if (!PNG_zInflateBlock(d))
				return 0xFF;
			break;
		case PNG_ZFLG_RESUME_OFFSET:		// Resume compressed block using offset copy for length bytes
			if (!PNG_zResumeOffset(d, ((unsigned *)d->z.tmp)[0], ((unsigned *)d->z.tmp)[1]))
				return 0xFF;
			break;
		}

		// Check for no data being provided
		// A resume code means the buffer is completely full so the test must be skipped
		if ((d->z.flags & PNG_ZFLG_RESUME_MASK) != PNG_ZFLG_RESUME_NEW)
			break;
	}

	// Get the next data byte
	data = d->z.buf[d->z.bufpos++];
	WRAP_ZBUF(d->z.bufpos);

	return data;
}

/*-----------------------------------------------------------------
 * Scan-line filter functions
 *---------------------------------------------------------------*/

// Initialise the scan-line engine
static void PNG_fInit(PNG_filter *f, uint8_t *buf, unsigned bytewidth, unsigned scanbytes) {
	f->scanbytes = scanbytes;
	f->bytewidth = bytewidth;
	f->line = buf;
	f->prev = 0;
}

// Get ready for the next scan-line
static void PNG_fNext(PNG_filter *f) {
	if (f->prev && f->line > f->prev) {
		f->line = f->prev;
		f->prev += f->scanbytes;
	} else {
		f->prev = f->line;
		f->line += f->scanbytes;
	}
}

// Predictor function for filter0 mode 4
static uint8_t PNG_fCalcPath(uint16_t a, uint16_t b, uint16_t c) {
	uint16_t pa = b > c ? (b - c) : (c - b);
	uint16_t pb = a > c ? (a - c) : (c - a);
	uint16_t pc = a + b > c + c ? (a + b - c - c) : (c + c - a - b);

	if (pc < pa && pc < pb)
		return (uint8_t)c;
	if (pb < pa)
		return (uint8_t)b;
	return (uint8_t)a;
}

// Scan-line filter type 0
static bool_t PNG_unfilter_type0(PNG_decode *d) {		// PNG filter method 0
	uint8_t		ft;
	unsigned	i;

	// Get the filter type and check for validity (eg not EOF)
	ft = PNG_zGetByte(d);
	if (ft > 0x04)
		return FALSE;

	// Uncompress the scan line
	for(i = 0; i < d->f.scanbytes; i++)
		d->f.line[i] = PNG_zGetByte(d);

	// Adjust the scan line based on the filter type
	// 0 = no adjustment
	switch(ft) {
	case 1:
		for(i = d->f.bytewidth; i < d->f.scanbytes; i++)
			d->f.line[i] += d->f.line[i - d->f.bytewidth];
		break;
	case 2:
		if (d->f.prev) {
			for(i = 0; i < d->f.scanbytes; i++)
				d->f.line[i] += d->f.prev[i];
		}
		break;
	case 3:
		if (d->f.prev) {
			for(i = 0; i < d->f.bytewidth; i++)
				d->f.line[i] += d->f.prev[i] / 2;
			for( ; i < d->f.scanbytes; i++)
				d->f.line[i] += (d->f.line[i - d->f.bytewidth] + d->f.prev[i]) / 2;
		} else {
			for(i = d->f.bytewidth; i < d->f.scanbytes; i++)
				d->f.line[i] += d->f.line[i - d->f.bytewidth] / 2;
		}
		break;
	case 4:
		if (d->f.prev) {
			for(i = 0; i < d->f.bytewidth; i++)
				d->f.line[i] += d->f.prev[i];					// PNG_fCalcPath(0, val, 0) is always val
			for( ; i < d->f.scanbytes; i++)
				d->f.line[i] += PNG_fCalcPath(d->f.line[i - d->f.bytewidth], d->f.prev[i], d->f.prev[i - d->f.bytewidth]);
		} else {
			for(i = d->f.bytewidth; i < d->f.scanbytes; i++)
				d->f.line[i] += d->f.line[i - d->f.bytewidth];	// PNG_fCalcPath(val, 0, 0) is always val
		}
		break;
	}

	return TRUE;
}

/*-----------------------------------------------------------------
 * Scan-line output and color conversion functions
 *---------------------------------------------------------------*/

#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_124
	static void PNG_OutGRAY124(PNG_decode *d) {
		unsigned	i;
		PNG_info 	*pinfo;
		uint8_t		px;
		uint8_t		bits;

		pinfo = d->pinfo;
		for(i = 0; i < d->f.scanbytes; i++) {
			for(bits = 8; bits; bits -= pinfo->bitdepth) {
				px = (d->f.line[i] >> (bits - pinfo->bitdepth)) & ((1U << pinfo->bitdepth)-1);
				#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
					if ((pinfo->flags & PNG_FLG_TRANSPARENT) && (uint16_t)px == pinfo->trans_r) {
						#if GDISP_NEED_IMAGE_PNG_BACKGROUND
							if ((pinfo->flags & PNG_FLG_BACKGROUND)) {
								PNG_oColor(&d->o, pinfo->bg);
								continue;
							}
						#endif
						PNG_oTransparent(&d->o);
						continue;
					}
				#endif
				px = px << (8-pinfo->bitdepth);
				if (px >= 0x80) px += ((1U << (8-pinfo->bitdepth))-1);
				PNG_oColor(&d->o, LUMA2COLOR(px));
			}
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_8
	static void PNG_OutGRAY8(PNG_decode *d) {
		unsigned		i;
		uint8_t			px;
		#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
			PNG_info 	*pinfo = d->pinfo;
		#endif

		for(i = 0; i < d->f.scanbytes; i++) {
			px = d->f.line[i];
			#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
				if ((pinfo->flags & PNG_FLG_TRANSPARENT) && (uint16_t)px == pinfo->trans_r) {
					#if GDISP_NEED_IMAGE_PNG_BACKGROUND
						if ((pinfo->flags & PNG_FLG_BACKGROUND)) {
							PNG_oColor(&d->o, pinfo->bg);
							continue;
						}
					#endif
					PNG_oTransparent(&d->o);
					continue;
				}
			#endif
			PNG_oColor(&d->o, LUMA2COLOR(px));
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_16
	static void PNG_OutGRAY16(PNG_decode *d) {
		unsigned		i;
		uint8_t			px;
		#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
			PNG_info 	*pinfo = d->pinfo;
		#endif

		for(i = 0; i < d->f.scanbytes; i+=2) {
			px = d->f.line[i];
			#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
				if ((pinfo->flags & PNG_FLG_TRANSPARENT) && gdispImageGetBE16(d->f.line, i) == pinfo->trans_r) {
					#if GDISP_NEED_IMAGE_PNG_BACKGROUND
						if ((pinfo->flags & PNG_FLG_BACKGROUND)) {
							PNG_oColor(&d->o, pinfo->bg);
							continue;
						}
					#endif
					PNG_oTransparent(&d->o);
					continue;
				}
			#endif
			PNG_oColor(&d->o, LUMA2COLOR(px));
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_RGB_8
	static void PNG_OutRGB8(PNG_decode *d) {
		unsigned		i;
		#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
			PNG_info 	*pinfo = d->pinfo;
		#endif

		for(i = 0; i < d->f.scanbytes; i+=3) {
			#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
				if ((pinfo->flags & PNG_FLG_TRANSPARENT)
							&& (uint16_t)d->f.line[i+0] == pinfo->trans_r
							&& (uint16_t)d->f.line[i+1] == pinfo->trans_g
							&& (uint16_t)d->f.line[i+2] == pinfo->trans_b) {
					#if GDISP_NEED_IMAGE_PNG_BACKGROUND
						if ((pinfo->flags & PNG_FLG_BACKGROUND)) {
							PNG_oColor(&d->o, pinfo->bg);
							continue;
						}
					#endif
					PNG_oTransparent(&d->o);
					continue;
				}
			#endif
			PNG_oColor(&d->o, RGB2COLOR(d->f.line[i+0], d->f.line[i+1], d->f.line[i+2]));
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_RGB_16
	static void PNG_OutRGB16(PNG_decode *d) {
		unsigned		i;
		#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
			PNG_info 	*pinfo = d->pinfo;
		#endif

		for(i = 0; i < d->f.scanbytes; i+=6) {
			#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
				if ((pinfo->flags & PNG_FLG_TRANSPARENT)
							&& gdispImageGetBE16(d->f.line, i+0) == pinfo->trans_r
							&& gdispImageGetBE16(d->f.line, i+2) == pinfo->trans_g
							&& gdispImageGetBE16(d->f.line, i+4) == pinfo->trans_b) {
					#if GDISP_NEED_IMAGE_PNG_BACKGROUND
						if ((pinfo->flags & PNG_FLG_BACKGROUND)) {
							PNG_oColor(&d->o, pinfo->bg);
							continue;
						}
					#endif
					PNG_oTransparent(&d->o);
					continue;
				}
			#endif
			PNG_oColor(&d->o, RGB2COLOR(d->f.line[i+0], d->f.line[i+2], d->f.line[i+4]));
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_PALETTE_124
	static void PNG_OutPAL124(PNG_decode *d) {
		unsigned	i;
		PNG_info 	*pinfo;
		unsigned	idx;
		uint8_t 	bits;

		pinfo = d->pinfo;
		for(i = 0; i < d->f.scanbytes; i++) {
			for(bits = 8; bits; bits -= pinfo->bitdepth) {
				idx = (d->f.line[i] >> (bits - pinfo->bitdepth)) & ((1U << pinfo->bitdepth)-1);

				if ((uint16_t)idx >= pinfo->palsize) {
					PNG_oColor(&d->o, RGB2COLOR(0, 0, 0));
					continue;
				}
				idx *= 4;

				#define pix_color	RGB2COLOR(pinfo->palette[idx], pinfo->palette[idx+1], pinfo->palette[idx+2])
				#define pix_alpha	pinfo->palette[idx+3]

				#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
					#if GDISP_NEED_IMAGE_PNG_BACKGROUND
						if (pix_alpha != 255 && (pinfo->flags & PNG_FLG_BACKGROUND)) {
							PNG_oColor(&d->o, gdispBlendColor(pix_color, pinfo->bg, pix_alpha));
							continue;
						}
					#endif
					#if GDISP_NEED_IMAGE_PNG_ALPHACLIFF > 0
						if (pix_alpha < GDISP_NEED_IMAGE_PNG_ALPHACLIFF) {
							PNG_oTransparent(&d->o);
							continue;
						}
					#endif
				#endif

				PNG_oColor(&d->o, pix_color);

				#undef pix_color
				#undef pix_alpha
			}
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_PALETTE_8
	static void PNG_OutPAL8(PNG_decode *d) {
		unsigned	i;
		PNG_info 	*pinfo;
		unsigned	idx;

		pinfo = d->pinfo;
		for(i = 0; i < d->f.scanbytes; i++) {
			idx = (unsigned)d->f.line[i];

			if ((uint16_t)idx >= pinfo->palsize) {
				PNG_oColor(&d->o, RGB2COLOR(0, 0, 0));
				continue;
			}
			idx *= 4;

			#define pix_color	RGB2COLOR(pinfo->palette[idx], pinfo->palette[idx+1], pinfo->palette[idx+2])
			#define pix_alpha	pinfo->palette[idx+3]

			#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
				#if GDISP_NEED_IMAGE_PNG_BACKGROUND
					if (pix_alpha != 255 && (pinfo->flags & PNG_FLG_BACKGROUND)) {
						PNG_oColor(&d->o, gdispBlendColor(pix_color, pinfo->bg, pix_alpha));
						continue;
					}
				#endif
				#if GDISP_NEED_IMAGE_PNG_ALPHACLIFF > 0
					if (pix_alpha < GDISP_NEED_IMAGE_PNG_ALPHACLIFF) {
						PNG_oTransparent(&d->o);
						continue;
					}
				#endif
			#endif

			PNG_oColor(&d->o, pix_color);

			#undef pix_color
			#undef pix_alpha
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_GRAYALPHA_8
	static void PNG_OutGRAYA8(PNG_decode *d) {
		unsigned		i;
		#if GDISP_NEED_IMAGE_PNG_BACKGROUND
			PNG_info 	*pinfo = d->pinfo;
		#endif

		for(i = 0; i < d->f.scanbytes; i+=2) {
			#define pix_color	LUMA2COLOR(d->f.line[i])
			#define pix_alpha	d->f.line[i+1]

			#if GDISP_NEED_IMAGE_PNG_BACKGROUND
				if (pix_alpha != 255 && (pinfo->flags & PNG_FLG_BACKGROUND)) {
					PNG_oColor(&d->o, gdispBlendColor(pix_color, pinfo->bg, pix_alpha));
					continue;
				}
			#endif
			#if GDISP_NEED_IMAGE_PNG_ALPHACLIFF > 0
				if (pix_alpha < GDISP_NEED_IMAGE_PNG_ALPHACLIFF) {
					PNG_oTransparent(&d->o);
					continue;
				}
			#endif

			PNG_oColor(&d->o, pix_color);

			#undef pix_color
			#undef pix_alpha
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_GRAYALPHA_16
	static void PNG_OutGRAYA16(PNG_decode *d) {
		unsigned		i;
		#if GDISP_NEED_IMAGE_PNG_BACKGROUND
			PNG_info 	*pinfo = d->pinfo;
		#endif

		for(i = 0; i < d->f.scanbytes; i+=4) {
			#define pix_color	LUMA2COLOR(d->f.line[i])
			#define pix_alpha	d->f.line[i+2]

			#if GDISP_NEED_IMAGE_PNG_BACKGROUND
				if (pix_alpha != 255 && (pinfo->flags & PNG_FLG_BACKGROUND)) {
					PNG_oColor(&d->o, gdispBlendColor(pix_color, pinfo->bg, pix_alpha));
					continue;
				}
			#endif
			#if GDISP_NEED_IMAGE_PNG_ALPHACLIFF > 0
				if (pix_alpha < GDISP_NEED_IMAGE_PNG_ALPHACLIFF) {
					PNG_oTransparent(&d->o);
					continue;
				}
			#endif

			PNG_oColor(&d->o, pix_color);

			#undef pix_color
			#undef pix_alpha
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_RGBALPHA_8
	static void PNG_OutRGBA8(PNG_decode *d) {
		unsigned		i;
		#if GDISP_NEED_IMAGE_PNG_BACKGROUND
			PNG_info 	*pinfo = d->pinfo;
		#endif

		for(i = 0; i < d->f.scanbytes; i+=4) {
			#define pix_color	RGB2COLOR(d->f.line[i+0], d->f.line[i+1], d->f.line[i+2])
			#define pix_alpha	d->f.line[i+3]

			#if GDISP_NEED_IMAGE_PNG_BACKGROUND
				if (pix_alpha != 255 && (pinfo->flags & PNG_FLG_BACKGROUND)) {
					PNG_oColor(&d->o, gdispBlendColor(pix_color, pinfo->bg, pix_alpha));
					continue;
				}
			#endif
			#if GDISP_NEED_IMAGE_PNG_ALPHACLIFF > 0
				if (pix_alpha < GDISP_NEED_IMAGE_PNG_ALPHACLIFF) {
					PNG_oTransparent(&d->o);
					continue;
				}
			#endif

			PNG_oColor(&d->o, pix_color);

			#undef pix_color
			#undef pix_alpha
		}
	}
#endif
#if GDISP_NEED_IMAGE_PNG_RGBALPHA_16
	static void PNG_OutRGBA16(PNG_decode *d) {
		unsigned		i;
		#if GDISP_NEED_IMAGE_PNG_BACKGROUND
			PNG_info 	*pinfo = d->pinfo;
		#endif

		for(i = 0; i < d->f.scanbytes; i+=8) {
			#define pix_color	RGB2COLOR(d->f.line[i+0], d->f.line[i+2], d->f.line[i+4])
			#define pix_alpha	d->f.line[i+6]

			#if GDISP_NEED_IMAGE_PNG_BACKGROUND
				if (pix_alpha != 255 && (pinfo->flags & PNG_FLG_BACKGROUND)) {
					PNG_oColor(&d->o, gdispBlendColor(pix_color, pinfo->bg, pix_alpha));
					continue;
				}
			#endif
			#if GDISP_NEED_IMAGE_PNG_ALPHACLIFF > 0
				if (pix_alpha < GDISP_NEED_IMAGE_PNG_ALPHACLIFF) {
					PNG_oTransparent(&d->o);
					continue;
				}
			#endif

			PNG_oColor(&d->o, pix_color);

			#undef pix_color
			#undef pix_alpha
		}
	}
#endif

/*-----------------------------------------------------------------
 * Public PNG functions
 *---------------------------------------------------------------*/

void gdispImageClose_PNG(gdispImage *img) {
	PNG_info *pinfo;

	pinfo = (PNG_info *)img->priv;
	if (pinfo) {
		if (pinfo->palette)
			gdispImageFree(img, (void *)pinfo->palette, pinfo->palsize*4);
		if (pinfo->cache)
			gdispImageFree(img, (void *)pinfo->cache, pinfo->cachesz);
		gdispImageFree(img, (void *)pinfo, sizeof(PNG_info));
		img->priv = 0;
	}
}

gdispImageError gdispImageOpen_PNG(gdispImage *img) {
	PNG_info	*pinfo;
	uint32_t	pos;
	uint32_t	len;
	uint8_t		buf[13];

	/* Read the file identifier */
	if (gfileRead(img->f, buf, 8) != 8)
		return GDISP_IMAGE_ERR_BADFORMAT;		// It can't be us

	// Check the PNG signature
	if(buf[0] != 137 || buf[1] != 80 || buf[2] != 78 || buf[3] != 71 || buf[4] != 13 || buf[5] != 10 || buf[6] != 26 || buf[7] != 10)
		return GDISP_IMAGE_ERR_BADFORMAT;		// It can't be us

	/* We know we are a PNG format image */
	img->flags = 0;
	img->priv = 0;
	img->type = GDISP_IMAGE_TYPE_PNG;

	/* Allocate our private area */
	if (!(img->priv = gdispImageAlloc(img, sizeof(PNG_info))))
		return GDISP_IMAGE_ERR_NOMEMORY;

	/* Initialise the essential bits in the private area */
	pinfo = (PNG_info *)img->priv;
	pinfo->flags = 0;
	pinfo->cache = 0;
	pinfo->trans_r = 0;
	pinfo->trans_g = 0;
	pinfo->trans_b = 0;
	#if GDISP_NEED_IMAGE_PNG_PALETTE_124 || GDISP_NEED_IMAGE_PNG_PALETTE_8
		pinfo->palsize = 0;
		pinfo->palette = 0;
	#endif

	// Cycle the chunks to get other information
	for(pos = 8; ; pos += len+12, gfileSetPos(img->f, pos)) {
		// Get a chunk header
		if (gfileRead(img->f, buf, 8) != 8)
			goto exit_baddata;

		// Calculate the chunk length
		len = gdispImageGetAlignedBE32(buf, 0);

		// Process the interesting information chunks
		switch (gdispImageGetAlignedBE32(buf, 4)) {
		case 0x49484452:		// "IHDR" - Header block

			// Check if the header is already done
			if ((pinfo->flags & PNG_FLG_HEADERDONE))
				goto exit_baddata;

			// Read the image parameters
			if (len < 13 || gfileRead(img->f, buf, 13) != 13)
				goto exit_baddata;

			img->width = gdispImageGetAlignedBE16(buf, 2);
			img->height = gdispImageGetAlignedBE16(buf, 6);
			pinfo->bitdepth = gdispImageGetVar(uint8_t, buf, 8);
			pinfo->mode = gdispImageGetVar(uint8_t, buf, 9);
			if (gdispImageGetVar(uint8_t, buf, 12)) {
				pinfo->flags |= PNG_FLG_INTERLACE;
				#if !GDISP_NEED_IMAGE_PNG_INTERLACED
					goto exit_unsupported;
				#endif
			}

			// Check width and height, filter, compression and interlacing
			if (gdispImageGetVar(uint16_t, buf, 0) != 0 || img->width <= 0			// width
					|| gdispImageGetVar(uint16_t, buf, 4) != 0  || img->height <= 0	// height
					|| gdispImageGetVar(uint8_t, buf, 10) != 0						// compression
					|| gdispImageGetVar(uint8_t, buf, 11) != 0						// filter
					|| gdispImageGetVar(uint8_t, buf, 12) > 1						// interlace
					)
				goto exit_unsupported;

			// Check mode, bitdepth and calculate bits per pixel
			switch(pinfo->mode) {
			#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_124 || GDISP_NEED_IMAGE_PNG_GRAYSCALE_8 || GDISP_NEED_IMAGE_PNG_GRAYSCALE_16
				case PNG_COLORMODE_GRAY:
					switch(pinfo->bitdepth) {
					#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_124
						case 1:
						case 2:
						case 4:		pinfo->out = PNG_OutGRAY124;	break;
					#endif
					#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_8
						case 8:		pinfo->out = PNG_OutGRAY8;	break;
					#endif
					#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_16
						case 16:	pinfo->out = PNG_OutGRAY16;	break;
					#endif
					default:	goto exit_unsupported;
					}
					pinfo->bpp = pinfo->bitdepth;
					break;
			#endif
			#if GDISP_NEED_IMAGE_PNG_RGB_8 || GDISP_NEED_IMAGE_PNG_RGB_16
				case PNG_COLORMODE_RGB:
					switch(pinfo->bitdepth) {
					#if GDISP_NEED_IMAGE_PNG_RGB_8
						case 8:		pinfo->out = PNG_OutRGB8;	break;
					#endif
					#if GDISP_NEED_IMAGE_PNG_RGB_16
						case 16:	pinfo->out = PNG_OutRGB16;	break;
					#endif
					default:	goto exit_unsupported;
					}
					pinfo->bpp = pinfo->bitdepth * 3;
					break;
			#endif
			#if GDISP_NEED_IMAGE_PNG_PALETTE_124 || GDISP_NEED_IMAGE_PNG_PALETTE_8
				case PNG_COLORMODE_PALETTE:
					switch(pinfo->bitdepth) {
					#if GDISP_NEED_IMAGE_PNG_PALETTE_124
						case 1:
						case 2:
						case 4:		pinfo->out = PNG_OutPAL124;	break;
					#endif
					#if GDISP_NEED_IMAGE_PNG_PALETTE_8
						case 8:		pinfo->out = PNG_OutPAL8;	break;
					#endif
					default:	goto exit_unsupported;
					}
					pinfo->bpp = pinfo->bitdepth;
					break;
			#endif
			#if GDISP_NEED_IMAGE_PNG_GRAYALPHA_8 || GDISP_NEED_IMAGE_PNG_GRAYALPHA_16
				case PNG_COLORMODE_GRAYALPHA:
					switch(pinfo->bitdepth) {
					#if GDISP_NEED_IMAGE_PNG_GRAYALPHA_8
						case 8:		pinfo->out = PNG_OutGRAYA8;	break;
					#endif
					#if GDISP_NEED_IMAGE_PNG_GRAYALPHA_16
						case 16:	pinfo->out = PNG_OutGRAYA16;	break;
					#endif
					default:	goto exit_unsupported;
					}
					pinfo->bpp = pinfo->bitdepth * 2;
					break;
			#endif
			#if GDISP_NEED_IMAGE_PNG_RGBALPHA_8 || GDISP_NEED_IMAGE_PNG_RGBALPHA_16
				case PNG_COLORMODE_RGBA:
					switch(pinfo->bitdepth) {
					#if GDISP_NEED_IMAGE_PNG_RGBALPHA_8
						case 8:		pinfo->out = PNG_OutRGBA8;	break;
					#endif
					#if GDISP_NEED_IMAGE_PNG_RGBALPHA_16
						case 16:	pinfo->out = PNG_OutRGBA16;	break;
					#endif
					default:	goto exit_unsupported;
					}
					pinfo->bpp = pinfo->bitdepth * 4;
					break;
			#endif
			default:
				goto exit_unsupported;
			}

			pinfo->flags |= PNG_FLG_HEADERDONE;
			break;

		case 0x49454E44:		// "IEND"	- All done
			goto exit_baddata;			// Oops we didn't get any data.

		case 0x49444154:		// "IDAT" - Image Data

			// Check if the header is already done
			if (!(pinfo->flags & PNG_FLG_HEADERDONE))
				goto exit_baddata;

			#if GDISP_NEED_IMAGE_PNG_PALETTE_124 || GDISP_NEED_IMAGE_PNG_PALETTE_8
				// Make sure a palette image actually has a palette
				if (pinfo->mode == PNG_COLORMODE_PALETTE && !pinfo->palette)
					goto exit_baddata;
			#endif

			// All good
			return GDISP_IMAGE_ERR_OK;

		#if GDISP_NEED_IMAGE_PNG_PALETTE_124 || GDISP_NEED_IMAGE_PNG_PALETTE_8
			case 0x504C5445:		// "PLTE"	- Palette

				// Check if the header is already done
				if (!(pinfo->flags & PNG_FLG_HEADERDONE))
					goto exit_baddata;

				// Skip a palette if we don't need it.
				if (pinfo->mode != PNG_COLORMODE_PALETTE)
					break;

				// Check the size and that we don't have one already
				if (len > 3 * 256 || pinfo->palette)
					goto exit_baddata;

				// Allocate the palette
				pinfo->palsize = len / 3;
				if (!(pinfo->palette = gfxAlloc(pinfo->palsize * 4)))
					goto exit_nonmem;

				// Read the palette
				{
					uint16_t	idx;
					uint8_t		*p;

					for(idx=pinfo->palsize, p=pinfo->palette; idx; p += 4, idx--) {
						if (gfileRead(img->f, p, 3) != 3)
							goto exit_baddata;
						p[3] = 255;
					}
				}

				break;
		#endif

		#if GDISP_NEED_IMAGE_PNG_TRANSPARENCY
			case 0x74524E53:		// "tRNS"	- Transparency

				// Check if the header is already done
				if (!(pinfo->flags & PNG_FLG_HEADERDONE))
					goto exit_baddata;

				// Transparency is handled differently depending on the mode
				switch(pinfo->mode) {

				#if GDISP_NEED_IMAGE_PNG_PALETTE_124 || GDISP_NEED_IMAGE_PNG_PALETTE_8
					case PNG_COLORMODE_PALETTE:
						if (len > pinfo->palsize)
							goto exit_baddata;

						// Adjust the palette
						{
							uint16_t	idx;
							uint8_t		*p;

							for(idx=len, p=pinfo->palette+3; idx; p += 4, idx--) {
								if (gfileRead(img->f, p, 1) != 1)
									goto exit_baddata;
							}
						}
						break;
				#endif

				#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_124 || GDISP_NEED_IMAGE_PNG_GRAYSCALE_8 || GDISP_NEED_IMAGE_PNG_GRAYSCALE_16
					case PNG_COLORMODE_GRAY:
						// Read the transparency color
						if (len != 2 || gfileRead(img->f, buf, 2) != 2)
							goto exit_baddata;

						pinfo->flags |= PNG_FLG_TRANSPARENT;
						pinfo->trans_r = gdispImageGetAlignedBE16(buf, 0);
						break;
				#endif
				#if GDISP_NEED_IMAGE_PNG_RGB_8 || GDISP_NEED_IMAGE_PNG_RGB_16
					case PNG_COLORMODE_RGB:
						// Read the transparency color
						if (len != 6 || gfileRead(img->f, buf, 6) != 6)
							goto exit_baddata;

						pinfo->flags |= PNG_FLG_TRANSPARENT;
						pinfo->trans_r = gdispImageGetAlignedBE16(buf, 0);
						pinfo->trans_g = gdispImageGetAlignedBE16(buf, 2);
						pinfo->trans_b = gdispImageGetAlignedBE16(buf, 4);
						break;
				#endif
				default:
					goto exit_unsupported;
				}

				break;
		#endif

		#if GDISP_NEED_IMAGE_PNG_BACKGROUND
			case 0x624B4744:		// "bKGD"	- Background

				// Check if the header is already done
				if (!(pinfo->flags & PNG_FLG_HEADERDONE))
					goto exit_baddata;

				pinfo->flags |= PNG_FLG_BACKGROUND;

				// Background data is handled differently depending on the mode
				switch(pinfo->mode) {

				#if GDISP_NEED_IMAGE_PNG_PALETTE_124 || GDISP_NEED_IMAGE_PNG_PALETTE_8
					case PNG_COLORMODE_PALETTE:
						if (!pinfo->palette || len < 1 || gfileRead(img->f, buf, 1) != 1 || (uint16_t)buf[0] >= pinfo->palsize)
							goto exit_baddata;
						pinfo->bg = RGB2COLOR(pinfo->palette[((unsigned)buf[0])*4+0],
												pinfo->palette[((unsigned)buf[0])*4+1],
												pinfo->palette[((unsigned)buf[0])*4+2]);
						break;
				#endif

				#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_124 || GDISP_NEED_IMAGE_PNG_GRAYSCALE_8 || GDISP_NEED_IMAGE_PNG_GRAYSCALE_16 || GDISP_NEED_IMAGE_PNG_GRAYALPHA_8 || GDISP_NEED_IMAGE_PNG_GRAYALPHA_16
					case PNG_COLORMODE_GRAY:
					case PNG_COLORMODE_GRAYALPHA:
						if (len < 2 || gfileRead(img->f, buf, 2) != 2)
							goto exit_baddata;
						switch(pinfo->bitdepth) {
						#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_124
							case 1:
							case 2:
							case 4:
								buf[1] <<= 8-pinfo->bitdepth;
								if (buf[1] >= 0x80)
									buf[1] += ((1U << (8-pinfo->bitdepth))-1);
								pinfo->bg = LUMA2COLOR(buf[1]);
								break;
						#endif
						#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_8 || GDISP_NEED_IMAGE_PNG_GRAYALPHA_8
							case 8:
								pinfo->bg = LUMA2COLOR(buf[1]);
								break;
						#endif
						#if GDISP_NEED_IMAGE_PNG_GRAYSCALE_16 || GDISP_NEED_IMAGE_PNG_GRAYALPHA_16
							case 16:
								pinfo->bg = LUMA2COLOR(buf[0]);
								break;
						#endif
						}
						break;
				#endif
				#if GDISP_NEED_IMAGE_PNG_RGB_8 || GDISP_NEED_IMAGE_PNG_RGB_16 || GDISP_NEED_IMAGE_PNG_RGBALPHA_8 || GDISP_NEED_IMAGE_PNG_RGBALPHA_16
					case PNG_COLORMODE_RGB:
					case PNG_COLORMODE_RGBA:
						if (len < 6 || gfileRead(img->f, buf, 6) != 6)
							goto exit_baddata;

						#if GDISP_NEED_IMAGE_PNG_RGB_16 || GDISP_NEED_IMAGE_PNG_RGBALPHA_16
							if (pinfo->bitdepth == 16) {
								pinfo->bg = RGB2COLOR(buf[0], buf[2], buf[4]);
							} else
						#endif
								pinfo->bg = RGB2COLOR(buf[1], buf[3], buf[5]);
						break;
				#endif
				default:
					goto exit_unsupported;
				}
				break;
		#endif

		}
	}
exit_baddata:
	gdispImageClose_PNG(img);
	return GDISP_IMAGE_ERR_BADDATA;
exit_unsupported:
	gdispImageClose_PNG(img);
	return GDISP_IMAGE_ERR_UNSUPPORTED;
exit_nonmem:
	gdispImageClose_PNG(img);
	return GDISP_IMAGE_ERR_NOMEMORY;
}

gdispImageError gdispGImageDraw_PNG(GDisplay *g, gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy) {
	PNG_info 	*pinfo;
	PNG_decode	*d;

	// Allocate the space to decode with including space for 2 full scan lines for filtering.
	pinfo = (PNG_info *)img->priv;
	if (!(d = gdispImageAlloc(img, sizeof(PNG_decode) + (img->width * pinfo->bpp + 7) / 4)))
		return GDISP_IMAGE_ERR_NOMEMORY;


	// Initialise the decoder
	d->img = img;
	d->pinfo = pinfo;
	PNG_iInit(d);
	PNG_oInit(&d->o, g, x, y, cx, cy, sx, sy);
	PNG_zInit(&d->z);

	// Process the zlib inflate header
	if (!PNG_zGetHeader(d))
		goto exit_baddata;

	#if GDISP_NEED_IMAGE_PNG_INTERLACED
		if ((pinfo->flags & PNG_FLG_INTERLACE)) {
			// Interlaced decoding
			#error "PNG Decoder: Interlaced PNG's are not supported yet!"
		} else
	#endif
	{
		// Non-interlaced decoding
		PNG_fInit(&d->f, (uint8_t *)(d+1), (pinfo->bpp + 7) / 8, (img->width * pinfo->bpp + 7) / 8);
		for(y = 0; y < sy+cy; PNG_fNext(&d->f), y++) {
			if (!PNG_unfilter_type0(d))
				goto exit_baddata;
			if (PNG_oStartY(&d->o, y)) {
				pinfo->out(d);
				PNG_oFlush(&d->o);
			}
		}
	}

	// Clean up
	gdispImageFree(img, d, sizeof(PNG_decode) + (img->width * pinfo->bpp + 7) / 4);
	return GDISP_IMAGE_ERR_OK;

exit_baddata:
	gdispImageFree(img, d, sizeof(PNG_decode) + (img->width * pinfo->bpp + 7) / 4);
	return GDISP_IMAGE_ERR_BADDATA;
}

gdispImageError gdispImageCache_PNG(gdispImage *img) {
	PNG_info 	*pinfo;
	unsigned	chunknext;
	unsigned	chunklen;
	uint8_t		*pcache;
	uint8_t		buf[8];

	// If we are already cached - just return OK
	pinfo = (PNG_info *)img->priv;
	if (pinfo->cache)
		return GDISP_IMAGE_ERR_OK;

	// Calculate the size of all the image data blocks in the image
	pinfo->cachesz = 0;
	chunknext = 8;
	while(1) {
		// Find a new chunk
		gfileSetPos(img->f, chunknext);
		if (gfileRead(img->f, buf, 8) != 8)
			return GDISP_IMAGE_ERR_BADDATA;

		// Calculate the chunk length and next chunk
		chunklen = gdispImageGetAlignedBE32(buf, 0);
		chunknext += chunklen + 12;

		// Process only image data chunks
		switch (gdispImageGetAlignedBE32(buf, 4)) {
		case 0x49444154:		// "IDAT" - Image Data
			pinfo->cachesz += chunklen;
			break;
		case 0x49454E44:		// "IEND"	- All done
			if (!pinfo->cachesz)
				return GDISP_IMAGE_ERR_BADDATA;
			goto gotsize;
		}
	}

gotsize:
	// Allocate the cache
	if (!(pcache = gdispImageAlloc(img, pinfo->cachesz)))
		return GDISP_IMAGE_ERR_NOMEMORY;

	pinfo->cache = pcache;

	// Read the image data into the cache
	chunknext = 8;
	while(1) {
		// Find a new chunk
		gfileSetPos(img->f, chunknext);
		if (gfileRead(img->f, buf, 8) != 8)
			goto baddata;

		// Calculate the chunk length and next chunk
		chunklen = gdispImageGetAlignedBE32(buf, 0);
		chunknext += chunklen + 12;

		// Process only image data chunks
		switch (gdispImageGetAlignedBE32(buf, 4)) {
		case 0x49444154:		// "IDAT" - Image Data
			if (gfileRead(img->f, pcache, chunklen) != chunklen)
				goto baddata;
			pcache += chunklen;
			break;
		case 0x49454E44:		// "IEND"	- All done
			return GDISP_IMAGE_ERR_OK;
		}
	}

baddata:
	// Oops - can't read the data. Throw away the cache.
	gdispImageFree(img, pinfo->cache, pinfo->cachesz);
	pinfo->cache = 0;
	return GDISP_IMAGE_ERR_BADDATA;
}

delaytime_t gdispImageNext_PNG(gdispImage *img) {
	(void) img;

	/* No more frames/pages */
	return TIME_INFINITE;
}

#endif /* GFX_USE_GDISP && GDISP_NEED_IMAGE && GDISP_NEED_IMAGE_PNG */
