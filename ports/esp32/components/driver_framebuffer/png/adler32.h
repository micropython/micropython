#ifndef LIB_ADLER32_H
#define LIB_ADLER32_H

#include <stdint.h>
#include <stddef.h>

#define LIB_ADLER32_INIT 1

extern uint32_t lib_adler32(const uint8_t *buf, size_t buf_len, uint32_t adler);

#endif // LIB_ADLER32_H
