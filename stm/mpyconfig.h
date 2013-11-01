// options to control how Micro Python is built

#define MICROPY_ENABLE_FLOAT        (0)
#define MICROPY_EMIT_CPYTHON        (0)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

typedef int32_t machine_int_t; // must be pointer size
typedef uint32_t machine_uint_t; // must be pointer size
typedef void *machine_ptr_t; // must be of pointer size
typedef float machine_float_t;
