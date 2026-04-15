#ifndef LIB_READER_H
#define LIB_READER_H

#include <stddef.h>
#include <unistd.h>

typedef ssize_t (*lib_reader_read_t)(void *, void *, size_t);

#endif // LIB_READER_H
