#ifndef LIB_MEM_READER_H
#define LIB_MEM_READER_H

#include <stdint.h>
#include <stddef.h>

struct lib_mem_reader {
	const uint8_t *data;
	size_t len;
};

extern struct lib_mem_reader * lib_mem_new(const uint8_t *data, size_t len);
extern ssize_t lib_mem_read(struct lib_mem_reader *mr, uint8_t *buf, size_t buf_len);
extern void lib_mem_destroy(struct lib_mem_reader *mr);

#endif // LIB_MEM_READER_H
