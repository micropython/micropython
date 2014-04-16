#include <stdint.h>

// options to control how Micro Python is built

#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_MEM_STATS           (0)
#define MICROPY_DEBUG_PRINTERS      (0)
#define MICROPY_ENABLE_GC           (0)
#define MICROPY_ENABLE_FINALISER    (0)
#define MICROPY_ENABLE_REPL_HELPERS (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PATH_MAX            (128)
#define MICROPY_ENABLE_MOD_IO       (0)

// extra built in names to add to the global namespace
extern const struct _mp_obj_fun_native_t mp_builtin_help_obj;
#define MICROPY_EXTRA_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_help), (mp_obj_t)&mp_builtin_help_obj },

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

#define UINT_FMT "%lu"
#define INT_FMT "%ld"

typedef int32_t machine_int_t; // must be pointer size
typedef uint32_t machine_uint_t; // must be pointer size
typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size

