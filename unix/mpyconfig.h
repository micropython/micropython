// options to control how Micro Python is built

#define MICROPY_ENABLE_FLOAT        (1)
#define MICROPY_EMIT_CPYTHON        (0)
#define MICROPY_EMIT_X64            (1)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)

// type definitions for the specific machine

#define BYTES_PER_WORD (8)

typedef int64_t machine_int_t; // must be pointer size
typedef uint64_t machine_uint_t; // must be pointer size
typedef void *machine_ptr_t; // must be of pointer size
typedef double machine_float_t;

machine_float_t machine_sqrt(machine_float_t x);
