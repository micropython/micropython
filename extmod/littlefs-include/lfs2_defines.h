#ifndef LFS2_DEFINES_H
#define LFS2_DEFINES_H

#include "py/mpconfig.h"

#if MICROPY_PY_DEFLATE
// We reuse the CRC32 implementation from uzlib to save a few bytes
#include "lib/uzlib/uzlib.h"
#define LFS2_CRC(crc, buffer, size) uzlib_crc32(buffer, size, crc)
#endif

#endif