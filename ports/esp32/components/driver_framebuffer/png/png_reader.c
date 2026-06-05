#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "crc32.h"
#include "adler32.h"
#include "deflate_reader.h"
#include "png_reader.h"

#include <stdio.h>

/* http://www.libpng.org/pub/png/spec/1.2/PNG-Contents.html */

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
 #define TYPE_ANCILLARY 0x00000020
 #define TYPE_PRIVATE   0x00002000
 #define TYPE_SAFE2COPY 0x20000000

 #define TYPE_IHDR      0x52444849
 #define TYPE_PLTE      0x45544C50
 #define TYPE_TRNS      0x534E5274
 #define TYPE_IDAT      0x54414449
 #define TYPE_IEND      0x444E4549

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
 #define TYPE_ANCILLARY 0x20000000
 #define TYPE_PRIVATE   0x00200000
 #define TYPE_SAFE2COPY 0x00000020

 #define TYPE_IHDR      0x49484452
 #define TYPE_PLTE      0x504C5445
 #define TYPE_TRNS      0x74524E53
 #define TYPE_IDAT      0x49444154
 #define TYPE_IEND      0x49454E44
#endif

struct lib_png_reader *
lib_png_new(lib_reader_read_t read, void *read_p)
{
	struct lib_png_reader *pr = (struct lib_png_reader *) malloc(sizeof(struct lib_png_reader));
	if (pr == NULL)
		return NULL;

	memset(pr, 0, sizeof(struct lib_png_reader));
	pr->read = read;
	pr->read_p = read_p;

	return pr;
}

ssize_t
lib_png_chunk_read(struct lib_png_reader *pr)
{
	struct lib_png_chunk *c = &pr->chunk;

	if (c->in_chunk)
		return -LIB_PNG_ERROR_UNEXPECTED_END_OF_FILE;

	ssize_t res = pr->read(pr->read_p, (uint8_t *) &c->len, 4);
	if (res < 0)
		return res;
	if (res < 4)
		return -LIB_PNG_ERROR_UNEXPECTED_END_OF_FILE;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	c->len = __builtin_bswap32(c->len);
#endif

	res = pr->read(pr->read_p, (uint8_t *) &c->type, 4);
	if (res < 0)
		return res;
	if (res < 4)
		return -LIB_PNG_ERROR_UNEXPECTED_END_OF_FILE;

	c->crc = lib_crc32((uint8_t *) &c->type, 4, LIB_CRC32_INIT);
	c->in_chunk = true;

	return c->len;
}

ssize_t
lib_png_chunk_read_data(struct lib_png_reader *pr, uint8_t *buf, size_t buf_len)
{
	struct lib_png_chunk *c = &pr->chunk;

	if (buf_len > c->len)
		buf_len = c->len;

	ssize_t res = pr->read(pr->read_p, buf, buf_len);
	if (res < 0)
		return res;
	if (res < buf_len)
		return -LIB_PNG_ERROR_UNEXPECTED_END_OF_FILE;

	c->len -= buf_len;
	c->crc = lib_crc32(buf, buf_len, c->crc);

	if (c->len == 0)
	{
		uint32_t crc_check;
		ssize_t res = pr->read(pr->read_p, (uint8_t *) &crc_check, 4);
		if (res < 0)
			return res;
		if (res < 4)
			return -LIB_PNG_ERROR_UNEXPECTED_END_OF_FILE;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		crc_check = __builtin_bswap32(crc_check);
#endif
		if (c->crc != crc_check)
			return -LIB_PNG_ERROR_CRC32_CHECKSUM_MISMATCH;

		c->in_chunk = false;
	}

	return buf_len;
}

ssize_t
lib_png_chunk_read_idat(struct lib_png_reader *pr, uint8_t *buf, size_t buf_len)
{
	ssize_t res_len = 0;
	while (buf_len > 0)
	{
		while (!pr->chunk.in_chunk)
		{
			ssize_t res = lib_png_chunk_read(pr);
			if (res < 0)
				return res;
			if (pr->chunk.type == TYPE_IDAT)
				break;

			if ((pr->chunk.type & TYPE_ANCILLARY) == 0)
				return -LIB_PNG_ERROR_UNKNOWN_CRITICAL_CHUNK;

			// skip chunk
			while (pr->chunk.in_chunk)
			{
				uint8_t buf[256];
				res = lib_png_chunk_read_data(pr, buf, sizeof(buf));
				if (res < 0)
					return res;
			}
		}

		ssize_t res = lib_png_chunk_read_data(pr, buf, buf_len);
		if (res < 0)
			return res;

		buf_len -= res;
		buf = &buf[res];
		res_len += res;
	}

	return res_len;
}

static inline ssize_t
lib_png_deflate_read(struct lib_png_reader *pr, uint8_t *buf, size_t buf_len)
{
	ssize_t res = lib_deflate_read(pr->dr, buf, buf_len);
	if (res < 0)
		return res;
	if (res < buf_len)
		return -LIB_PNG_ERROR_UNEXPECTED_END_OF_FILE;
	pr->adler = lib_adler32(buf, buf_len, pr->adler);

	return res;
}

static inline uint8_t
lib_png_paeth(uint8_t _a, uint8_t _b, uint8_t _c)
{
	int32_t a = _a; // left
	int32_t b = _b; // up
	int32_t c = _c; // up-left

	int32_t p = a + b - c;
	int32_t pa = p < a ? a - p : p - a;
	int32_t pb = p < b ? b - p : p - b;
	int32_t pc = p < c ? c - p : p - c;

	if (pa <= pb && pa <= pc)
		return _a;

	if (pb <= pc)
		return _b;

	return _c;
}

static inline int
lib_png_decode(Window* window, struct lib_png_reader *pr, uint32_t width, uint32_t height, uint32_t scanline_width, int16_t offset_x, int16_t offset_y, uint32_t dst_min_x, uint32_t dst_min_y, uint32_t dst_width, uint32_t dst_height, uint32_t dst_pixlen, uint32_t dst_linelen)
{
	memset(pr->scanline, 0, scanline_width);

	uint32_t y;
	for (y=0; y<height; y++)
	{
		uint8_t filter_type;
		ssize_t res = lib_png_deflate_read(pr, &filter_type, 1);
		if (res < 0)
			return res;

		if (filter_type < 2)
		{
			ssize_t res = lib_png_deflate_read(pr, pr->scanline, scanline_width);
			if (res < 0)
				return res;
		}
		else
		{
			ssize_t res = lib_png_deflate_read(pr, &pr->scanline[scanline_width], scanline_width);
			if (res < 0)
				return res;
		}

		uint32_t x;
		uint8_t up_prev[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		uint8_t *scanline = pr->scanline;
		int bpp = pr->scanline_bpp;
		switch(filter_type)
		{
			case 0: // none
				break;

			case 1: // sub
				scanline = &scanline[bpp];
				for (x=bpp; x<scanline_width; x++)
				{
					*scanline += scanline[-bpp];
					scanline++;
				}
				break;

			case 2: // up
				for (x=0; x<scanline_width; x++)
				{
					*scanline += scanline[scanline_width];
					scanline++;
				}
				break;

			case 3: // average
				for (x=0; x<scanline_width && x<bpp; x++)
				{
					uint32_t up   = *scanline;
					*scanline = (up >> 1) + scanline[scanline_width];
					scanline++;
				}
				for (; x<scanline_width; x++)
				{
					uint32_t left = scanline[-bpp];
					uint32_t up   = *scanline;
					*scanline = ((left + up) >> 1) + scanline[scanline_width];
					scanline++;
				}
				break;

			case 4: // paeth
				for (x=0; x<scanline_width && x<bpp; x++)
				{
					uint8_t a = 0; // left
					uint8_t b = *scanline; // up
					uint8_t c = 0; // up-left
					up_prev[x] = b;

					*scanline = lib_png_paeth(a, b, c) + scanline[scanline_width];
					scanline++;
				}
				for (; x<scanline_width; x++)
				{
					uint8_t a = scanline[-bpp]; // left
					uint8_t b = *scanline; // up
					uint8_t c = up_prev[(x-bpp) & 7]; // up-left
					up_prev[x & 7] = b;

					*scanline = lib_png_paeth(a, b, c) + scanline[scanline_width];
					scanline++;
				}
				break;

			default:
				return -LIB_PNG_ERROR_INVALID_PNG_SCANLINE_TYPE;
		}

		for (x=0; x<width; x++)
		{
			uint16_t r=0, g=0, b=0, a=0;
			switch (pr->ihdr.color_type)
			{
				case 0:
					a = 0xffff;
					switch (pr->ihdr.bit_depth)
					{
						case 16:
							r = g = b = (pr->scanline[x*2 + 0] << 8) | pr->scanline[x*2 + 1];
							break;
						case 8:
							r = g = b = pr->scanline[x] * 0x0101;
							break;
						case 4:
							r = g = b = ((pr->scanline[x>>1] >> (((x^1)&1)*4)) & 15) * 0x1111;
							break;
						case 2:
							r = g = b = ((pr->scanline[x>>2] >> (((x^3)&3)*2)) & 3) * 0x5555;
							break;
						case 1:
							r = g = b = ((pr->scanline[x>>3] >> ((x^7)&7)) & 1) * 0xffff;
							break;
					}
					break;
				case 2:
					a = 0xffff;
					switch (pr->ihdr.bit_depth)
					{
						case 16:
							r = (pr->scanline[x*6 + 0] << 8) | pr->scanline[x*6 + 1];
							g = (pr->scanline[x*6 + 2] << 8) | pr->scanline[x*6 + 3];
							b = (pr->scanline[x*6 + 4] << 8) | pr->scanline[x*6 + 5];
							break;
						case 8:
							r = pr->scanline[x*3 + 0] * 0x0101;
							g = pr->scanline[x*3 + 1] * 0x0101;
							b = pr->scanline[x*3 + 2] * 0x0101;
							break;
					}
					break;
				case 3:
					// if trns does not exist or is shorter than the palette, the remaining alpha channels are the default 0xffff
					a = 0xffff;
					switch (pr->ihdr.bit_depth)
					{
						case 8:
							b = pr->scanline[x];
							break;
						case 4:
							b = (pr->scanline[x>>1] >> (((x^1)&1)*4)) & 15;
							break;
						case 2:
							b = (pr->scanline[x>>2] >> (((x^3)&3)*2)) & 3;
							break;
						case 1:
							b = (pr->scanline[x>>3] >> ((x^7)&7)) & 1;
							break;
					}
					if (pr->trns != NULL && b < pr->trns_len)
					{
						a = pr->trns[b] * 0x0101;
					}
					if (b < pr->palette_len)
					{
						r = pr->palette[b*3 + 0] * 0x0101;
						g = pr->palette[b*3 + 1] * 0x0101;
						b = pr->palette[b*3 + 2] * 0x0101;
					}
					else
					{
						r = g = b = 0; // or should this be an error?
					}
					break;
				case 4:
					switch (pr->ihdr.bit_depth)
					{
						case 16:
							r = g = b = (pr->scanline[x*4 + 0] << 8) | pr->scanline[x*4 + 1];
							a = (pr->scanline[x*4 + 2] << 8) | pr->scanline[x*4 + 3];
							break;
						case 8:
							r = g = b = pr->scanline[x*2 + 0] * 0x0101;
							a = pr->scanline[x*2 + 1] * 0x0101;
							break;
					}
					break;
				case 6:
					switch (pr->ihdr.bit_depth)
					{
						case 16:
							r = (pr->scanline[x*8 + 0] << 8) | pr->scanline[x*8 + 1];
							g = (pr->scanline[x*8 + 2] << 8) | pr->scanline[x*8 + 3];
							b = (pr->scanline[x*8 + 4] << 8) | pr->scanline[x*8 + 5];
							a = (pr->scanline[x*8 + 6] << 8) | pr->scanline[x*8 + 7];
							break;
						case 8:
							r = pr->scanline[x*4 + 0] * 0x0101;
							g = pr->scanline[x*4 + 1] * 0x0101;
							b = pr->scanline[x*4 + 2] * 0x0101;
							a = pr->scanline[x*4 + 3] * 0x0101;
							break;
					}
					break;
			}

			if (a != 0 && x >= dst_min_x && x < dst_width && y >= dst_min_y && y < dst_height)
			{
				#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE
				driver_framebuffer_setPixel(window, offset_x + x, offset_y + y, ((r>>8)<<16)+((g>>8)<<8)+(b>>8));
				#endif
			}
		}
	}

	return 0;
}

int
lib_png_read_header(struct lib_png_reader *pr)
{
	static const uint8_t png_sig[8] = {
		0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a,
	};

	// check if header is already read
	if (pr->ihdr.bit_depth != 0)
		return 0;

	uint8_t sig[8];
	ssize_t res = pr->read(pr->read_p, sig, 8);
	if (res < 0)
		return res;
	if (res < 8)
		return -LIB_PNG_ERROR_UNEXPECTED_END_OF_FILE;

	if (memcmp(sig, png_sig, 8) != 0)
		return -LIB_PNG_ERROR_MISSING_SIGNATURE;

	res = lib_png_chunk_read(pr);
	if (res < 0)
		return res;
	if (pr->chunk.type != TYPE_IHDR)
		return -LIB_PNG_ERROR_INVALID_FIRST_CHUNK;

	res = lib_png_chunk_read_data(pr, (uint8_t *) &pr->ihdr, sizeof(struct lib_png_ihdr));
	if (res < 0)
		return res;
	if (res < sizeof(struct lib_png_ihdr))
		return -LIB_PNG_ERROR_UNEXPECTED_END_OF_CHUNK; // chunk too small?

	if (pr->chunk.len != 0)
		return -LIB_PNG_ERROR_CHUNK_TOO_LARGE;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	pr->ihdr.width  = __builtin_bswap32(pr->ihdr.width);
	pr->ihdr.height = __builtin_bswap32(pr->ihdr.height);
#endif
	// verify png_ihdr
	if (pr->ihdr.width == 0 || pr->ihdr.width > 0x80000000)
		return -LIB_PNG_ERROR_INVALID_PNG_TYPE;
	if (pr->ihdr.height == 0 || pr->ihdr.height > 0x80000000)
		return -LIB_PNG_ERROR_INVALID_PNG_TYPE;
	switch (pr->ihdr.color_type)
	{
		case 0:
			if (pr->ihdr.bit_depth != 1
			 && pr->ihdr.bit_depth != 2
			 && pr->ihdr.bit_depth != 4
			 && pr->ihdr.bit_depth != 8
			 && pr->ihdr.bit_depth != 16)
				return -LIB_PNG_ERROR_INVALID_PNG_TYPE;
			break;

		case 2:
		case 4:
		case 6:
			if (pr->ihdr.bit_depth != 8
			 && pr->ihdr.bit_depth != 16)
				return -LIB_PNG_ERROR_INVALID_PNG_TYPE;
			break;

		case 3:
			if (pr->ihdr.bit_depth != 1
			 && pr->ihdr.bit_depth != 2
			 && pr->ihdr.bit_depth != 4
			 && pr->ihdr.bit_depth != 8)
				return -LIB_PNG_ERROR_INVALID_PNG_TYPE;
			break;

		default:
			return -LIB_PNG_ERROR_INVALID_PNG_TYPE;
	}
	if (pr->ihdr.compression_method != 0)
		return -LIB_PNG_ERROR_INVALID_PNG_TYPE;
	if (pr->ihdr.filter_method != 0)
		return -LIB_PNG_ERROR_INVALID_PNG_TYPE;
	if (pr->ihdr.interlace_method > 1)
		return -LIB_PNG_ERROR_INVALID_PNG_TYPE;

	// calculate scanline
	uint32_t bits_per_pixel = pr->ihdr.bit_depth;
	if (pr->ihdr.color_type == 2)
		bits_per_pixel *= 3; // r, g, b
	if (pr->ihdr.color_type == 4)
		bits_per_pixel *= 2; // grey, alpha
	if (pr->ihdr.color_type == 6)
		bits_per_pixel *= 4; // r, g, b, alpha

	uint32_t scanline_bits = pr->ihdr.width * bits_per_pixel;

	pr->scanline_bpp   = (bits_per_pixel + 7) >> 3;
	pr->scanline_width = (scanline_bits + 7) >> 3;

	return 0;
}

int lib_png_load_image(Window* window, struct lib_png_reader *pr, int16_t offset_x, int16_t offset_y, uint32_t dst_min_x, uint32_t dst_min_y, uint32_t dst_width, uint32_t dst_height, uint32_t dst_linelen)
{
	// read header
	int res = lib_png_read_header(pr);
	if (res < 0)
		return res;

	// read chunks until we locate the IDAT chunk
	while (1)
	{
		res = lib_png_chunk_read(pr);
		if (res < 0)
			return res;
		if (pr->chunk.type == TYPE_IDAT)
			break;

		if (pr->chunk.type == TYPE_PLTE)
		{
			if (pr->palette != NULL)
				return -LIB_PNG_ERROR_MULTIPLE_PALETTES_FOUND;
			if (pr->chunk.len > 768 || pr->chunk.len % 3 != 0)
				return -LIB_PNG_ERROR_INVALID_PALETTE_SIZE;

			pr->palette = (uint8_t *) malloc(pr->chunk.len);
			if (pr->palette == NULL)
				return -LIB_PNG_ERROR_OUT_OF_MEMORY;
			pr->palette_len = pr->chunk.len / 3;

			res = lib_png_chunk_read_data(pr, pr->palette, pr->chunk.len);
			if (res < 0)
				return res;
			continue;
		}

		if (pr->chunk.type == TYPE_TRNS)
		{
			if (pr->trns != NULL)
				return -LIB_PNG_ERROR_MULTIPLE_TRNS_FOUND;
			if (pr->chunk.len > 768 || (pr->palette != NULL && pr->palette_len < pr->chunk.len))
				return -LIB_PNG_ERROR_INVALID_TRNS_SIZE;

			pr->trns = (uint8_t *) malloc(pr->chunk.len);
			if (pr->trns == NULL)
				return -LIB_PNG_ERROR_OUT_OF_MEMORY;
			pr->trns_len = pr->chunk.len;

			res = lib_png_chunk_read_data(pr, pr->trns, pr->chunk.len);
			if (res < 0)
				return res;
			continue;
		}

		if ((pr->chunk.type & TYPE_ANCILLARY) == 0)
			return -LIB_PNG_ERROR_UNKNOWN_CRITICAL_CHUNK;

		// skip chunk
		while (pr->chunk.in_chunk)
		{
			uint8_t buf[256];
			res = lib_png_chunk_read_data(pr, buf, sizeof(buf));
			if (res < 0)
				return res;
		}
	}

	// do we have a palette?
	if (pr->ihdr.color_type == 3 && pr->palette == NULL)
		return -LIB_PNG_ERROR_NO_PALETTE_FOUND;
	if (pr->ihdr.color_type == 0 && pr->palette != NULL)
		return -LIB_PNG_ERROR_PALETTE_NOT_ALLOWED_FOR_PNG_TYPE;
	if (pr->ihdr.color_type == 4 && pr->palette != NULL)
		return -LIB_PNG_ERROR_PALETTE_NOT_ALLOWED_FOR_PNG_TYPE;

	// allocate scanline
	pr->scanline = (uint8_t *) malloc(pr->scanline_width * 2);
	if (pr->scanline == NULL)
		return -LIB_PNG_ERROR_OUT_OF_MEMORY;

	// start parsing IDAT
	uint8_t rfc1950_hdr[2];
	res = lib_png_chunk_read_idat(pr, rfc1950_hdr, 2);
	if (res < 0)
		return res;
	if (res < 2)
		return -LIB_PNG_ERROR_UNEXPECTED_END_OF_CHUNK;

	int windowsize = 1<<(((rfc1950_hdr[0] & 0xf0)>>4)+8);
	if ((rfc1950_hdr[0] & 0x0f) != 0x08) // should be deflate algorithm
		return -LIB_PNG_ERROR_INVALID_DEFLATE_HEADER;
	if (windowsize > 32768) // max window size is 32 KB
		return -LIB_PNG_ERROR_INVALID_DEFLATE_HEADER;
	if (rfc1950_hdr[1] & 0x20) // preset dictionary not allowed
		return -LIB_PNG_ERROR_INVALID_DEFLATE_HEADER;
	if (((rfc1950_hdr[0] << 8) + rfc1950_hdr[1]) % 31 != 0) // check checksum
		return -LIB_PNG_ERROR_INVALID_DEFLATE_HEADER;

	pr->dr = lib_deflate_new((lib_reader_read_t) &lib_png_chunk_read_idat, pr, windowsize);
    //printf("allocated deflate with header: %d\n", windowsize);
	if (pr->dr == NULL)
		return -LIB_PNG_ERROR_OUT_OF_MEMORY;

	pr->adler = LIB_ADLER32_INIT;

	res = lib_png_decode(window, pr, pr->ihdr.width, pr->ihdr.height, pr->scanline_width, offset_x, offset_y, dst_min_x, dst_min_y, dst_width, dst_height, 1, dst_linelen);
	if (res < 0) return res;

	// ensure we're at the end of the deflate stream
	{
		uint8_t test_eos;
		ssize_t res = lib_deflate_read(pr->dr, &test_eos, 1);
		if (res < 0)
			return res;
		if (res != 0)
			return -LIB_PNG_ERROR_CHUNK_TOO_LARGE;
	}

	// verify adler
	uint32_t adler_chk;
	res = lib_png_chunk_read_idat(pr, (uint8_t *) &adler_chk, 4);
	if (res < 0)
		return res;
	if (res < 4)
		return -LIB_PNG_ERROR_UNEXPECTED_END_OF_CHUNK;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	adler_chk = __builtin_bswap32(adler_chk);
#endif
	if (pr->adler != adler_chk)
		return -LIB_PNG_ERROR_ADLER32_CHECKSUM_MISMATCH;

	// verify IEND

	// read chunks until we locate the IEND chunk
	while (1)
	{
		res = lib_png_chunk_read(pr);
		if (res < 0)
			return res;
		if (pr->chunk.type == TYPE_IEND)
			break;

		if ((pr->chunk.type & TYPE_ANCILLARY) == 0)
			return -LIB_PNG_ERROR_UNKNOWN_CRITICAL_CHUNK;

		// skip chunk
		while (pr->chunk.in_chunk)
		{
			uint8_t buf[256];
			res = lib_png_chunk_read_data(pr, buf, sizeof(buf));
			if (res < 0)
				return res;
		}
	}

	if (pr->chunk.len != 0)
		return -LIB_PNG_ERROR_CHUNK_TOO_LARGE;

	res = lib_png_chunk_read_data(pr, NULL, 1);
	if (res < 0)
		return res;

	return 0;
}

void
lib_png_destroy(struct lib_png_reader *pr)
{
	if (pr->palette)
		free(pr->palette);
	pr->palette = NULL;

	if (pr->trns)
		free(pr->trns);
	pr->trns = NULL;

	if (pr->scanline)
		free(pr->scanline);
	pr->scanline = NULL;

	if (pr->dr)
		lib_deflate_destroy(pr->dr);
	pr->dr = NULL;

	free(pr);
}
