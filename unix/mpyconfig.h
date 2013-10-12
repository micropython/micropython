// options to control how Micro Python is built

//#define MICROPY_ENABLE_FLOAT
#define MICROPY_EMIT_ENABLE_CPYTHON
#define MICROPY_EMIT_ENABLE_X64
//#define MICROPY_EMIT_ENABLE_THUMB
#define MICROPY_EMIT_ENABLE_INLINE_THUMB

// type definitions for the specific machine

typedef int64_t machine_int_t; // must be pointer size
typedef uint64_t machine_uint_t; // must be pointer size
typedef void *machine_ptr_t; // must be of pointer size
typedef double machine_float_t;
