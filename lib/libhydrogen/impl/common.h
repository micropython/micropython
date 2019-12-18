#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__unix__) && (defined(__APPLE__) || defined(__linux__))
#define __unix__ 1
#endif
#ifndef __GNUC__
#define __restrict__
#endif

#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && \
    __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define NATIVE_BIG_ENDIAN
#endif
#ifndef NATIVE_BIG_ENDIAN
#ifndef NATIVE_LITTLE_ENDIAN
#define NATIVE_LITTLE_ENDIAN
#endif
#endif

#ifndef TLS
#if defined(_WIN32) && !defined(__GNUC__)
#define TLS __declspec(thread)
#elif (defined(__clang__) || defined(__GNUC__)) && defined(__unix__)
#define TLS __thread
#else
#define TLS
#endif
#endif

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t) -1)
#endif

#ifdef __OpenBSD__
#define HAVE_EXPLICIT_BZERO 1
#elif defined(__GLIBC__) && defined(__GLIBC_PREREQ) && defined(_GNU_SOURCE)
#if __GLIBC_PREREQ(2, 25)
#define HAVE_EXPLICIT_BZERO 1
#endif
#endif

#define COMPILER_ASSERT(X) (void) sizeof(char[(X) ? 1 : -1])

#define ROTL32(x, b) (uint32_t)(((x) << (b)) | ((x) >> (32 - (b))))
#define ROTL64(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define ROTR32(x, b) (uint32_t)(((x) >> (b)) | ((x) << (32 - (b))))
#define ROTR64(x, b) (uint64_t)(((x) >> (b)) | ((x) << (64 - (b))))

#define LOAD64_LE(SRC) load64_le(SRC)
static inline uint64_t
load64_le(const uint8_t src[8])
{
#ifdef NATIVE_LITTLE_ENDIAN
    uint64_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint64_t w = (uint64_t) src[0];
    w |= (uint64_t) src[1] << 8;
    w |= (uint64_t) src[2] << 16;
    w |= (uint64_t) src[3] << 24;
    w |= (uint64_t) src[4] << 32;
    w |= (uint64_t) src[5] << 40;
    w |= (uint64_t) src[6] << 48;
    w |= (uint64_t) src[7] << 56;
    return w;
#endif
}

#define STORE64_LE(DST, W) store64_le((DST), (W))
static inline void
store64_le(uint8_t dst[8], uint64_t w)
{
#ifdef NATIVE_LITTLE_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[0] = (uint8_t) w;
    w >>= 8;
    dst[1] = (uint8_t) w;
    w >>= 8;
    dst[2] = (uint8_t) w;
    w >>= 8;
    dst[3] = (uint8_t) w;
    w >>= 8;
    dst[4] = (uint8_t) w;
    w >>= 8;
    dst[5] = (uint8_t) w;
    w >>= 8;
    dst[6] = (uint8_t) w;
    w >>= 8;
    dst[7]     = (uint8_t) w;
#endif
}

#define LOAD32_LE(SRC) load32_le(SRC)
static inline uint32_t
load32_le(const uint8_t src[4])
{
#ifdef NATIVE_LITTLE_ENDIAN
    uint32_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint32_t w = (uint32_t) src[0];
    w |= (uint32_t) src[1] << 8;
    w |= (uint32_t) src[2] << 16;
    w |= (uint32_t) src[3] << 24;
    return w;
#endif
}

#define STORE32_LE(DST, W) store32_le((DST), (W))
static inline void
store32_le(uint8_t dst[4], uint32_t w)
{
#ifdef NATIVE_LITTLE_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[0] = (uint8_t) w;
    w >>= 8;
    dst[1] = (uint8_t) w;
    w >>= 8;
    dst[2] = (uint8_t) w;
    w >>= 8;
    dst[3]     = (uint8_t) w;
#endif
}

#define LOAD16_LE(SRC) load16_le(SRC)
static inline uint16_t
load16_le(const uint8_t src[2])
{
#ifdef NATIVE_LITTLE_ENDIAN
    uint16_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint16_t w = (uint16_t) src[0];
    w |= (uint16_t) src[1] << 8;
    return w;
#endif
}

#define STORE16_LE(DST, W) store16_le((DST), (W))
static inline void
store16_le(uint8_t dst[2], uint16_t w)
{
#ifdef NATIVE_LITTLE_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[0] = (uint8_t) w;
    w >>= 8;
    dst[1]     = (uint8_t) w;
#endif
}

/* ----- */

#define LOAD64_BE(SRC) load64_be(SRC)
static inline uint64_t
load64_be(const uint8_t src[8])
{
#ifdef NATIVE_BIG_ENDIAN
    uint64_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint64_t w = (uint64_t) src[7];
    w |= (uint64_t) src[6] << 8;
    w |= (uint64_t) src[5] << 16;
    w |= (uint64_t) src[4] << 24;
    w |= (uint64_t) src[3] << 32;
    w |= (uint64_t) src[2] << 40;
    w |= (uint64_t) src[1] << 48;
    w |= (uint64_t) src[0] << 56;
    return w;
#endif
}

#define STORE64_BE(DST, W) store64_be((DST), (W))
static inline void
store64_be(uint8_t dst[8], uint64_t w)
{
#ifdef NATIVE_BIG_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[7] = (uint8_t) w;
    w >>= 8;
    dst[6] = (uint8_t) w;
    w >>= 8;
    dst[5] = (uint8_t) w;
    w >>= 8;
    dst[4] = (uint8_t) w;
    w >>= 8;
    dst[3] = (uint8_t) w;
    w >>= 8;
    dst[2] = (uint8_t) w;
    w >>= 8;
    dst[1] = (uint8_t) w;
    w >>= 8;
    dst[0]     = (uint8_t) w;
#endif
}

#define LOAD32_BE(SRC) load32_be(SRC)
static inline uint32_t
load32_be(const uint8_t src[4])
{
#ifdef NATIVE_BIG_ENDIAN
    uint32_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint32_t w = (uint32_t) src[3];
    w |= (uint32_t) src[2] << 8;
    w |= (uint32_t) src[1] << 16;
    w |= (uint32_t) src[0] << 24;
    return w;
#endif
}

#define STORE32_BE(DST, W) store32_be((DST), (W))
static inline void
store32_be(uint8_t dst[4], uint32_t w)
{
#ifdef NATIVE_BIG_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[3] = (uint8_t) w;
    w >>= 8;
    dst[2] = (uint8_t) w;
    w >>= 8;
    dst[1] = (uint8_t) w;
    w >>= 8;
    dst[0]     = (uint8_t) w;
#endif
}

#define LOAD16_BE(SRC) load16_be(SRC)
static inline uint16_t
load16_be(const uint8_t src[2])
{
#ifdef NATIVE_BIG_ENDIAN
    uint16_t w;
    memcpy(&w, src, sizeof w);
    return w;
#else
    uint16_t w = (uint16_t) src[1];
    w |= (uint16_t) src[0] << 8;
    return w;
#endif
}

#define STORE16_BE(DST, W) store16_be((DST), (W))
static inline void
store16_be(uint8_t dst[2], uint16_t w)
{
#ifdef NATIVE_BIG_ENDIAN
    memcpy(dst, &w, sizeof w);
#else
    dst[1] = (uint8_t) w;
    w >>= 8;
    dst[0] = (uint8_t) w;
#endif
}

static inline void
mem_cpy(void *__restrict__ dst_, const void *__restrict__ src_, size_t n)
{
    unsigned char *      dst = (unsigned char *) dst_;
    const unsigned char *src = (const unsigned char *) src_;
    size_t               i;

    for (i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

static inline void
mem_zero(void *dst_, size_t n)
{
    unsigned char *dst = (unsigned char *) dst_;
    size_t         i;

    for (i = 0; i < n; i++) {
        dst[i] = 0;
    }
}

static inline void
mem_xor(void *__restrict__ dst_, const void *__restrict__ src_, size_t n)
{
    unsigned char *      dst = (unsigned char *) dst_;
    const unsigned char *src = (const unsigned char *) src_;
    size_t               i;

    for (i = 0; i < n; i++) {
        dst[i] ^= src[i];
    }
}

static inline void
mem_xor2(void *__restrict__ dst_, const void *__restrict__ src1_, const void *__restrict__ src2_,
         size_t n)
{
    unsigned char *      dst  = (unsigned char *) dst_;
    const unsigned char *src1 = (const unsigned char *) src1_;
    const unsigned char *src2 = (const unsigned char *) src2_;
    size_t               i;

    for (i = 0; i < n; i++) {
        dst[i] = src1[i] ^ src2[i];
    }
}

static const uint8_t zero[64] = { 0 };
