// options to control how Micro Python is built

#define MICROPY_EMIT_CPYTHON        (1)
#define MICROPY_ENABLE_LEXER_UNIX   (1)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_DOUBLE)
#define MICROPY_ENABLE_MOD_IO       (0)

// type definitions for the specific machine

#ifdef __LP64__
typedef long machine_int_t; // must be pointer size
typedef unsigned long machine_uint_t; // must be pointer size
#else
// These are definitions for machines where sizeof(int) == sizeof(void*),
// regardless for actual size.
typedef int machine_int_t; // must be pointer size
typedef unsigned int machine_uint_t; // must be pointer size
#endif

#define BYTES_PER_WORD sizeof(machine_int_t)

typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size
typedef double machine_float_t;

machine_float_t machine_sqrt(machine_float_t x);
