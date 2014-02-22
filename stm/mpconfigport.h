#include <stdint.h>

// options to control how Micro Python is built

#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_REPL_HELPERS (1)
#define MICROPY_ENABLE_FLOAT        (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_PATH_MAX            (128)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

#define UINT_FMT "%lu"
#define INT_FMT "%ld"

typedef int32_t machine_int_t; // must be pointer size
typedef uint32_t machine_uint_t; // must be pointer size
typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size
typedef float machine_float_t;

machine_float_t machine_sqrt(machine_float_t x);

// There is no classical C heap in bare-metal ports, only Python
// garbage-collected heap. For completeness, emulate C heap via
// GC heap. Note that MicroPython core never uses malloc() and friends,
// so these defines are mostly to help extension module writers.
#define malloc gc_alloc
#define free gc_free
#define realloc gc_realloc

// board specific definitions

#include "mpconfigboard.h"

#define STM32F40_41xxx
#define USE_STDPERIPH_DRIVER
#if !defined(HSE_VALUE)
#define HSE_VALUE (8000000)
#endif
#define USE_DEVICE_MODE
//#define USE_HOST_MODE
