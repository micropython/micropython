#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "../include/driver_framebuffer_compositor.h"
#include "../include/driver_framebuffer.h"

#include "reader.h"

#ifdef __cplusplus
extern "C" {
#endif

enum lib_png_error_t {
	LIB_PNG_ERROR_BASE = 0x1100,
	LIB_PNG_ERROR_OUT_OF_MEMORY,
	LIB_PNG_ERROR_MISSING_SIGNATURE,
	LIB_PNG_ERROR_UNEXPECTED_END_OF_FILE,
	LIB_PNG_ERROR_UNEXPECTED_END_OF_CHUNK,
	LIB_PNG_ERROR_CHUNK_TOO_LARGE,
	LIB_PNG_ERROR_INVALID_FIRST_CHUNK,
	LIB_PNG_ERROR_UNKNOWN_CRITICAL_CHUNK,
	LIB_PNG_ERROR_ADLER32_CHECKSUM_MISMATCH,
	LIB_PNG_ERROR_CRC32_CHECKSUM_MISMATCH,
	LIB_PNG_ERROR_INVALID_PNG_TYPE,
	LIB_PNG_ERROR_NO_PALETTE_FOUND,
	LIB_PNG_ERROR_MULTIPLE_PALETTES_FOUND,
	LIB_PNG_ERROR_PALETTE_NOT_ALLOWED_FOR_PNG_TYPE,
	LIB_PNG_ERROR_INVALID_PALETTE_SIZE,
	LIB_PNG_ERROR_INVALID_DEFLATE_HEADER,
	LIB_PNG_ERROR_INVALID_PNG_SCANLINE_TYPE,
	LIB_PNG_ERROR_TOP,
	LIB_PNG_ERROR_MULTIPLE_TRNS_FOUND,
	LIB_PNG_ERROR_INVALID_TRNS_SIZE,
};

struct lib_png_chunk {
	bool in_chunk;
	uint32_t type;
	uint32_t len;
	uint32_t crc;
};

struct lib_png_ihdr {
	uint32_t width;
	uint32_t height;
	uint8_t bit_depth;
	uint8_t color_type;
	uint8_t compression_method;
	uint8_t filter_method;
	uint8_t interlace_method;
} __attribute__((packed));

struct lib_png_reader {
	lib_reader_read_t read;
	void *read_p;

	struct lib_png_chunk chunk;
	struct lib_png_ihdr ihdr;

	uint8_t *palette;
	int palette_len;
	uint8_t *trns;
	int trns_len;

	uint8_t scanline_bpp; // 1, 2, 3, 4, 6 or 8
	uint32_t scanline_width;
	uint8_t *scanline; // large enough for scanline + temp secondary scanline

	struct lib_deflate_reader *dr;
	uint32_t adler;
};

extern struct lib_png_reader * lib_png_new(lib_reader_read_t read, void *read_p);
extern int lib_png_read_header(struct lib_png_reader *pr);
extern int lib_png_load_image(Window *window, struct lib_png_reader *pr, int16_t offset_x, int16_t offset_y, uint32_t dst_min_x, uint32_t dst_min_y, uint32_t dst_width, uint32_t dst_height, uint32_t dst_linelen);
extern void lib_png_destroy(struct lib_png_reader *pr);

#ifdef __cplusplus
}
#endif
