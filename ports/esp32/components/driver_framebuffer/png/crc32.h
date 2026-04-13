#ifndef LIB_CRC32_H
#define LIB_CRC32_H

#include <stdint.h>
#include <stddef.h>

#define LIB_CRC32_INIT 0

extern uint32_t lib_crc32(const uint8_t *buf, size_t buf_len, uint32_t crc);

#endif // LIB_CRC32_H
