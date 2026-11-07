// arch/cc.h - lwIP compiler/architecture port header for GD32F4xx.
#ifndef MICROPY_INCLUDED_GD32_LWIP_ARCH_CC_H
#define MICROPY_INCLUDED_GD32_LWIP_ARCH_CC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// lwIP is compiled as third-party code and does not see MicroPython's headers
// everywhere, so keep the diagnostics backend free of py/ dependencies and
// route messages to stdout instead.
#include "py/mpconfig.h"

// Byte order
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

typedef uint8_t   u8_t;
typedef int8_t    s8_t;
typedef uint16_t  u16_t;
typedef int16_t   s16_t;
typedef uint32_t  u32_t;
typedef int32_t   s32_t;
typedef uintptr_t mem_ptr_t;

// Diagnostics.  Using plain stdio keeps this header independent of py/obj.h.
#define LWIP_PLATFORM_DIAG(x)   do { printf x; } while (0)
#define LWIP_PLATFORM_ASSERT(x) do { \
    printf("lwIP assert: %s\n", x); \
    for (;;) { } \
} while (0)

// Random seed from the port's hardware RNG.
uint32_t mp_hal_get_hw_random_u32(void);
#define LWIP_RAND() mp_hal_get_hw_random_u32()

#endif // MICROPY_INCLUDED_GD32_LWIP_ARCH_CC_H
