#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "deflate_reader.h"

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

static inline int
lib_deflate_get_bits_long(struct lib_deflate_reader *dr, int num)
{
	int value = dr->bitbuf[1];
	if (num > dr->bitlen + 8)
	{
		ssize_t res = dr->read(dr->read_p, dr->bitbuf, 2);
		if (unlikely(res < 2))
		{
			if (res < 0)
				return res;
			return -LIB_DEFLATE_ERROR_UNEXPECTED_END_OF_FILE;
		}
		uint16_t *bitbuf = (uint16_t *) dr->bitbuf;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		value |= *bitbuf << 8;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		value |= __builtin_bswap16(*bitbuf) << 8;
#endif
	}
	else if (num > dr->bitlen)
	{
		ssize_t res = dr->read(dr->read_p, &(dr->bitbuf[1]), 1);
		if (unlikely(res < 1))
		{
			if (res < 0)
				return res;
			return -LIB_DEFLATE_ERROR_UNEXPECTED_END_OF_FILE;
		}
		value |= dr->bitbuf[1] << 8;
	}

	value >>= 8 - dr->bitlen;
	value &= (1 << num) - 1;
	dr->bitlen -= num;
	dr->bitlen &= 7;

	return value;
}

static inline int
lib_deflate_get_bits(struct lib_deflate_reader *dr, int num)
{
	int value = dr->bitbuf[1];
	if (num > dr->bitlen)
	{
		ssize_t res = dr->read(dr->read_p, &(dr->bitbuf[1]), 1);
		if (unlikely(res < 1))
		{
			if (res < 0)
				return res;
			return -LIB_DEFLATE_ERROR_UNEXPECTED_END_OF_FILE;
		}
		value |= dr->bitbuf[1] << 8;
	}

	value >>= 8 - dr->bitlen;
	value &= (1 << num) - 1;
	dr->bitlen -= num;
	dr->bitlen &= 7;

	return value;
}

static inline int
lib_deflate_build_huffman(const uint8_t *tbl, int tbl_len, uint16_t *tree)
{
	int bits;
	int bits_prev = 0;
	int total = 0;
	for (bits=1; bits<16; bits++)
	{
		int i;
		for (i=0; i<tbl_len; i++)
		{
			if (unlikely(tbl[i] == bits))
			{
				if (unlikely(bits - bits_prev > 8))
				{
					*tree++ = 8;
					bits_prev += 8;
				}
				uint16_t *base = tree++;
				*tree++ = i;
				int count = 1;
				i++;
				for (; i<tbl_len; i++)
				{
					if (unlikely(tbl[i] == bits))
					{
						*tree++ = i;
						count++;
					}
				}
				*base = (count << 4) | (bits - bits_prev);
				bits_prev = bits;
				total += (0x10000 * count) >> bits;
				break;
			}
		}
	}

	if (unlikely(total != 0x10000))
	{
		// table not well-balanced. something is wrong.
		return -LIB_DEFLATE_ERROR_UNBALANCED_HUFFMAN_TREE;
	}

	return 0;
}

static inline int
lib_deflate_get_huffman(struct lib_deflate_reader *dr, const uint16_t *tbl)
{
	int value = 0;
	while (1)
	{
		uint16_t bits = *tbl++;
		uint16_t entries = bits >> 4;

		bits &= 15;
		int res = lib_deflate_get_bits(dr, bits);
		if (unlikely(res < 0))
			return res;

		// shift res into value, in reverse order.
		switch (bits)
		{
			case 8: value <<= 1; value += res & 1; res >>= 1;
			case 7: value <<= 1; value += res & 1; res >>= 1;
			case 6: value <<= 1; value += res & 1; res >>= 1;
			case 5: value <<= 1; value += res & 1; res >>= 1;
			case 4: value <<= 1; value += res & 1; res >>= 1;
			case 3: value <<= 1; value += res & 1; res >>= 1;
			case 2: value <<= 1; value += res & 1; res >>= 1;
			case 1: value <<= 1; value += res;
		}

		if (value < entries)
			return tbl[value];

		value -= entries;
		tbl = &tbl[entries];
	}

	return value;
}

void
lib_deflate_init(struct lib_deflate_reader *dr, lib_reader_read_t read, void *read_p)
{
	memset(dr, 0, sizeof(struct lib_deflate_reader));
	dr->read = read;
	dr->read_p = read_p;
}

struct lib_deflate_reader *
lib_deflate_new(lib_reader_read_t read, void *read_p, int lb_capacity)
{
	struct lib_deflate_reader *dr = (struct lib_deflate_reader *) malloc(sizeof(struct lib_deflate_reader)+lb_capacity);
	if (unlikely(dr == NULL))
		return NULL;

	lib_deflate_init(dr, read, read_p);
	dr->lb_capacity = lb_capacity;

	return dr;
}

ssize_t
lib_deflate_read(struct lib_deflate_reader *dr, uint8_t *buf, size_t buf_len)
{
	ssize_t buf_pos = 0;
	while (buf_pos < buf_len)
	{
		if (dr->state == LIB_DEFLATE_STATE_NEW_BLOCK)
		{ // initial state; have to read block header
			if (unlikely(dr->is_last_block))
				return buf_pos;

			int block_type = lib_deflate_get_bits(dr, 3);
			if (unlikely(block_type < 0))
				return block_type;

			dr->is_last_block = (block_type & 1) ? true : false;
			block_type >>= 1;

			if (block_type == 0)
			{ // stored block
				dr->bitlen = 0;

				uint16_t rd_buf[2];
				ssize_t res = dr->read(dr->read_p, (uint8_t *) rd_buf, 4);
				if (unlikely(res < 4))
				{
					if (res < 0)
						return res;
					return -LIB_DEFLATE_ERROR_UNEXPECTED_END_OF_FILE;
				}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				int len = rd_buf[0];
				int nlen = rd_buf[1];
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
				int len = __builtin_bswap16(rd_buf[0]);
				int nlen = __builtin_bswap16(rd_buf[1]);
#endif
				if (unlikely(len + nlen != 0xffff))
					return -LIB_DEFLATE_ERROR_INVALID_COPY_LENGTH;

				dr->copy_len = len;
				dr->state = LIB_DEFLATE_STATE_STORED_BLOCK;
			}
			else if (block_type == 1)
			{ // static huffman
				int i;
				uint16_t *tree = dr->huffman_lc_tree;
				*tree++ = ((280-256) << 4) | 7;
				for (i=256; i<280; i++)
					*tree++ = i;
				*tree++ = ((144-0+288-280) << 4) | 1;
				for (i=0; i<144; i++)
					*tree++ = i;
				for (i=280; i<288; i++)
					*tree++ = i;
				*tree++ = ((256-144) << 4) | 1;
				for (i=144; i<256; i++)
					*tree++ = i;

				tree = dr->huffman_dc_tree;
				*tree++ = ((32-0) << 4) | 5;
				for (i=0; i<32; i++)
					*tree++ = i;

				dr->state = LIB_DEFLATE_STATE_HUFFMAN;
			}
			else if (block_type == 2)
			{ // dynamic huffman
				int blc_dc_lc = lib_deflate_get_bits_long(dr, 14);
				if (unlikely(blc_dc_lc < 0))
					return blc_dc_lc;

				int lc_num  = 257 + ( blc_dc_lc        & 31);
				int dc_num  = 1   + ((blc_dc_lc >>  5) & 31);
				int blc_num = 4   +  (blc_dc_lc >> 10);

				// use the temp huffman array for all huffman table creates.
				uint8_t huffman[288];

				static const uint8_t blc_order[19] = {
					16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15, 
				};
				memset(huffman, 0, 19);
				int i;
				for (i=0; i<blc_num; i++)
				{
					int bits = lib_deflate_get_bits(dr, 3);
					if (unlikely(bits < 0))
						return bits;
					huffman[blc_order[i]] = bits;
				}

				uint16_t blc_tree[19 + 7];
				int res = lib_deflate_build_huffman(huffman, 19, blc_tree);
				if (unlikely(res < 0))
					return res; // invalid table

				int lc_i=0;
				while (lc_i < lc_num)
				{
					int len = lib_deflate_get_huffman(dr, blc_tree);
					if (unlikely(len < 0))
						return len;

					if (len < 16)
					{
						huffman[lc_i++] = len;
					}
					else if (len == 16)
					{
						if (unlikely(lc_i == 0))
							return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR;
						uint8_t prev_len = huffman[lc_i - 1];
						int repeat = lib_deflate_get_bits(dr, 2);
						if (unlikely(repeat < 0))
							return repeat;
						repeat += 3;
						if (unlikely(lc_i + repeat > lc_num))
							return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR; // overflow
						while (repeat--)
							huffman[lc_i++] = prev_len;
					}
					else if (len == 17)
					{
						int repeat = lib_deflate_get_bits(dr, 3);
						if (unlikely(repeat < 0))
							return repeat;
						repeat += 3;
						if (unlikely(lc_i + repeat > lc_num))
							return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR; // overflow
						while (repeat--)
							huffman[lc_i++] = 0;
					}
					else if (len == 18)
					{
						int repeat = lib_deflate_get_bits(dr, 7);
						if (unlikely(repeat < 0))
							return repeat;
						repeat += 11;
						if (unlikely(lc_i + repeat > lc_num))
							return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR; // overflow
						while (repeat--)
							huffman[lc_i++] = 0;
					}
					else return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR;
				}

				res = lib_deflate_build_huffman(huffman, lc_i, dr->huffman_lc_tree);
				if (unlikely(res < 0))
					return res; // invalid table

				int dc_i=0;
				while (dc_i < dc_num)
				{
					int len = lib_deflate_get_huffman(dr, blc_tree);
					if (unlikely(len < 0))
						return len;

					if (len < 16)
					{
						huffman[dc_i++] = len;
					}
					else if (len == 16)
					{
						if (unlikely(dc_i == 0))
							return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR;
						uint8_t prev_len = huffman[dc_i - 1];
						int repeat = lib_deflate_get_bits(dr, 2);
						if (unlikely(repeat < 0))
							return repeat;
						repeat += 3;
						if (unlikely(dc_i + repeat > dc_num))
							return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR; // overflow
						while (repeat--)
							huffman[dc_i++] = prev_len;
					}
					else if (len == 17)
					{
						int repeat = lib_deflate_get_bits(dr, 3);
						if (unlikely(repeat < 0))
							return repeat;
						repeat += 3;
						if (unlikely(dc_i + repeat > dc_num))
							return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR; // overflow
						while (repeat--)
							huffman[dc_i++] = 0;
					}
					else if (len == 18)
					{
						int repeat = lib_deflate_get_bits(dr, 7);
						if (unlikely(repeat < 0))
							return repeat;
						repeat += 11;
						if (unlikely(dc_i + repeat > dc_num))
							return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR; // overflow
						while (repeat--)
							huffman[dc_i++] = 0;
					}
					else return -LIB_DEFLATE_ERROR_DYNAMIC_HUFFMAN_SETUP_ERROR;
				}

				res = lib_deflate_build_huffman(huffman, dc_i, dr->huffman_dc_tree);
				if (unlikely(res < 0))
					return res; // invalid table

				dr->state = LIB_DEFLATE_STATE_HUFFMAN;
			}
			else
			{ // reserved block_type
				return -LIB_DEFLATE_ERROR_RESERVED_BLOCK_TYPE;
			}
		}

		if (dr->state == LIB_DEFLATE_STATE_STORED_BLOCK)
		{ // stored block
			while (dr->copy_len > 0)
			{
				if (buf_pos >= buf_len)
					return buf_pos;

				size_t copylen = dr->copy_len;
				if (copylen > buf_len - buf_pos)
					copylen = buf_len - buf_pos;
				if (copylen > dr->lb_capacity - dr->lb_pos)
					copylen = dr->lb_capacity - dr->lb_pos;

				uint8_t *rd_buf = &dr->look_behind[ dr->lb_pos ];
				ssize_t res = dr->read(dr->read_p, rd_buf, copylen);
				if (unlikely(res < copylen))
				{
					if (res < 0)
						return res;
					return -LIB_DEFLATE_ERROR_UNEXPECTED_END_OF_FILE;
				}

				memcpy(&buf[buf_pos], rd_buf, copylen);
				buf_pos += copylen;

				dr->copy_len -= copylen;

				dr->lb_pos += copylen;
				dr->lb_pos &= (dr->lb_capacity-1);
				if (dr->lb_size < dr->lb_capacity)
					dr->lb_size += copylen;
			}
			dr->state = LIB_DEFLATE_STATE_NEW_BLOCK;
		}

		if (dr->state == LIB_DEFLATE_STATE_HUFFMAN)
		{ // huffman encoded block.
			int token = lib_deflate_get_huffman(dr, dr->huffman_lc_tree);
			if (unlikely(token < 0))
				return token;
			if (token < 256)
			{ // store token and return
				buf[buf_pos++] = token;

				dr->look_behind[ dr->lb_pos ] = token;
				dr->lb_pos++;
				dr->lb_pos &= (dr->lb_capacity-1);
				if (dr->lb_size < dr->lb_capacity)
					dr->lb_size++;
			}
			else if (token == 256)
			{ // next block
				dr->state = LIB_DEFLATE_STATE_NEW_BLOCK;
			}
			else
			{ // copy data from look_behind
				// determine copy-length
				token -= 257;
				if (unlikely(token >= 29))
					return -LIB_DEFLATE_ERROR_HUFFMAN_RESERVED_LENGTH; // invalid index

				static const uint16_t clp[29] = {
					3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258
				};
				static const uint8_t clb[29] = {
					0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0
				};

				int copy_len = clb[ token ] ? lib_deflate_get_bits(dr, clb[ token ]) : 0;
				if (unlikely(copy_len < 0))
					return copy_len;
				copy_len += clp[ token ];

				// determine distance
				int token = lib_deflate_get_huffman(dr, dr->huffman_dc_tree);
				if (unlikely(token < 0))
					return token;
				if (unlikely(token >= 30))
					return -LIB_DEFLATE_ERROR_HUFFMAN_RESERVED_LENGTH; // invalid index

				static const uint16_t dcp[30] = {
					1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577
				};
				static const uint8_t dcb[30] = {
					0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13
				};

				int dist = dcb[ token ] ? lib_deflate_get_bits_long(dr, dcb[ token ]) : 0;
				if (unlikely(dist < 0))
					return dist;
				dist += dcp[ token ];

				if (unlikely(dist > dr->lb_size))
					return -LIB_DEFLATE_ERROR_HUFFMAN_INVALID_DISTANCE; // invalid distance

				dr->copy_dist = dist;
				dr->copy_len = copy_len;
				dr->state = LIB_DEFLATE_STATE_HUFFMAN_REPEAT;
			}
		}

		if (dr->state == LIB_DEFLATE_STATE_HUFFMAN_REPEAT)
		{ // repeating old data. will continue huffman decoding afterwards
			while (dr->copy_len > 0)
			{
				if (buf_pos >= buf_len)
					return buf_pos;

				// int copylen = dr->copy_len;
				// copylen = buf_len - buf_pos if copylen > buf_len - buf_pos
				// copylen = dr->copy_dist if copylen > dr->copy_dist
				// copylen = dr->lb_capacity - dr->lb_pos if copylen > dr->lb_capacity - dr->lb_pos
				// copylen = dr->lb_capacity - pos if copylen > dr->lb_capacity - pos
				dr->copy_len--;
				int pos = (dr->lb_pos - dr->copy_dist) & (dr->lb_capacity-1);
				int token = dr->look_behind[ pos ];

				buf[buf_pos++] = token;

				dr->look_behind[ dr->lb_pos ] = token;
				dr->lb_pos++;
				dr->lb_pos &= (dr->lb_capacity-1);
				if (dr->lb_size < dr->lb_capacity)
					dr->lb_size++;
			}
			dr->state = LIB_DEFLATE_STATE_HUFFMAN;
		}
	}

	return buf_pos;
}

void
lib_deflate_destroy(struct lib_deflate_reader *dr)
{
	free(dr);
}
