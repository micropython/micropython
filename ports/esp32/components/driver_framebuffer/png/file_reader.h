#ifndef LIB_FILE_READER_H
#define LIB_FILE_READER_H

#include <stdint.h>
#include <stddef.h>

struct lib_file_reader {
	int fd;
	int bufsize;
	int buflen;
	int bufpos;
	uint8_t buf[0];
};

extern struct lib_file_reader * lib_file_new(const char *filename, int bufsize);
extern ssize_t lib_file_read(struct lib_file_reader *fr, uint8_t *buf, size_t buf_len);
extern void lib_file_destroy(struct lib_file_reader *fr);

#endif // LIB_FILE_READER_H
