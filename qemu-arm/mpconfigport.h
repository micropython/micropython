#include <stdint.h>

// options to control how Micro Python is built

#define MICROPY_ALLOC_PATH_MAX      (512)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_MALLOC_USES_ALLOCATED_SIZE (1)
#define MICROPY_MEM_STATS           (1)
#define MICROPY_DEBUG_PRINTERS      (0)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_HELPER_REPL         (0)
#define MICROPY_HELPER_LEXER_UNIX   (0)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_POW3    (1)
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_SYS_EXIT         (1)
#define MICROPY_PY_SYS_MAXSIZE      (1)
#define MICROPY_PY_UERRNO           (1)
#define MICROPY_PY_UBINASCII        (1)
#define MICROPY_PY_URANDOM          (1)
#define MICROPY_PY_UCTYPES          (1)
#define MICROPY_PY_UZLIB            (1)
#define MICROPY_PY_UJSON            (1)
#define MICROPY_PY_URE              (1)
#define MICROPY_PY_UHEAPQ           (1)
#define MICROPY_PY_UHASHLIB         (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_USE_INTERNAL_PRINTF (0)
#define MICROPY_VFS                 (1)

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)

#define UINT_FMT "%lu"
#define INT_FMT "%ld"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

#include <unistd.h>
#define MP_PLAT_PRINT_STRN(str, len) write(1, str, len)

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

// extra built-in modules to add to the list of known ones
extern const struct _mp_obj_module_t mp_module_uos;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) }, \
    { MP_ROM_QSTR(MP_QSTR_umachine), MP_ROM_PTR(&mp_module_machine) }, \

// We need to provide a declaration/definition of alloca()
#include <alloca.h>
