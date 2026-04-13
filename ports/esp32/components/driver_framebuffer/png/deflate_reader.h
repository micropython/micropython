#ifndef LIB_DEFLATE_READER_H
#define LIB_DEFLATE_READER_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "reader.h"

enum lib_deflate_state_t {
	LIB_DEFLATE_STATE_NEW_BLOCK = 0,
	LIB_DEFLATE_STATE_STORED_BLOCK,
	LIB_DEFLATE_STATE_HUFFMAN,
	LIB_DEFLATE_STATE_HUFFMAN_REPEAT,
};

enum lib_deflate_error_t {
	LIB_DEFLATE_ERROR_BASE = 0x1000,
	LIB_DEFLATE_ERROR_UNEXPECTED_END_OF_FILE,
	LIB_DEFLATE_ERROR_UNBALANCED_HUFFMAN_TREE,
	LIB_DEFLATE_ERROR_INVALID_COPY_LENGTH,
	LIB_DEFLATE_ERROR_RESERVED_BLOCK_TYPE,
	LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR,
	LIB_DEFLATE_ERROR_HUFFMAN_RESERVED_LENGTH,
	LIB_DEFLATE_ERROR_HUFFMAN_INVALID_DISTANCE,
	LIB_DEFLATE_ERROR_TOP,
};

struct lib_deflate_reader {
	lib_reader_read_t read;
	void *read_p;

	uint8_t bitbuf[2];
	uint8_t bitlen;

	bool is_last_block;
	enum lib_deflate_state_t state;
	int lb_size;
	int lb_capacity;
	int lb_pos;

	int copy_dist;
	int copy_len;

	uint16_t huffman_lc_tree[288+15];
	uint16_t huffman_dc_tree[32+15];

	uint8_t look_behind[];
};

extern struct lib_deflate_reader * lib_deflate_new(lib_reader_read_t read, void *read_p, int lb_capacity);
extern void lib_deflate_init(struct lib_deflate_reader *dr, lib_reader_read_t read, void *read_p);
extern ssize_t lib_deflate_read(struct lib_deflate_reader *dr, uint8_t *buf, size_t buf_len);
extern void lib_deflate_destroy(struct lib_deflate_reader *dr);

#endif // LIB_DEFLATE_READER_H
