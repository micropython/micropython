#include <stdint.h>

#include "uart.h"

// options to control how Micro Python is built
#define MICROPY_ENABLE_COMPILER (1)

#define MICROPY_QSTR_BYTES_IN_HASH  (1)
#define MICROPY_ALLOC_PATH_MAX      (512)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_EMIT_INLINE_THUMB_ARMV7M (0)
#define MICROPY_EMIT_INLINE_THUMB_FLOAT (0)
#define MICROPY_EMIT_ARM            (1)

//#define MICROPY_ENABLE_GC (1)
//#define MICROPY_ENABLE_FINALISER (0)
//#define MICROPY_STACK_CHECK (0)

#ifdef MICROPY_ERROR_REPORTING
#undef MICROPY_ERROR_REPORTING
#define MICROPY_ERROR_REPORTING (MICROPY_ERROR_REPORTING_DETAILED)
#endif
#define MICROPY_WARNINGS (1)

#define MICROPY_COMP_MODULE_CONST   (0)
#define MICROPY_COMP_CONST          (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_MEM_STATS           (0)
#define MICROPY_DEBUG_PRINTERS      (1)
#define MICROPY_HELPER_REPL         (0)
#define MICROPY_HELPER_LEXER_UNIX   (0)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_ENABLE_DOC_STRING   (1)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_PY_BUILTINS_COMPILE (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (0)
#define MICROPY_PY_BUILTINS_ENUMERATE (1)
#define MICROPY_PY_BUILTINS_FROZENSET (0)
#define MICROPY_PY_BUILTINS_REVERSED (0)
#define MICROPY_PY_BUILTINS_SET     (0)
#define MICROPY_PY_BUILTINS_SLICE   (0)
#define MICROPY_PY_BUILTINS_PROPERTY (0)
#define MICROPY_PY___FILE__         (0)
#define MICROPY_PY_GC               (0)
#define MICROPY_PY_ARRAY            (1)
#define MICROPY_PY_ATTRTUPLE        (0)
#define MICROPY_PY_COLLECTIONS      (0)
#define MICROPY_PY_MATH             (0)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (0)
#define MICROPY_PY_STRUCT           (0)
#define MICROPY_PY_SYS              (1)
#define MICROPY_CPYTHON_COMPAT      (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_LONGLONG)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_NONE)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

#define MICROPY_MAKE_POINTER_CALLABLE(p) BOOTUP(p)

#define UINT_FMT "%lu"
#define INT_FMT "%ld"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size
typedef long mp_off_t;

// dummy print
#define MP_PLAT_PRINT_STRN(str, len) uart_puts(str, len)

extern const struct _mp_obj_module_t cmodule;
extern const struct _mp_obj_module_t mp_module_cdebug, mp_module_cio;

#define MICROPY_PORT_BUILTIN_MODULES \
        { MP_OBJ_NEW_QSTR(MP_QSTR_C), (mp_obj_t)&cmodule }, \
        { MP_OBJ_NEW_QSTR(MP_QSTR_Cdebug), (mp_obj_t)&mp_module_cdebug }, \
        { MP_OBJ_NEW_QSTR(MP_QSTR_cio), (mp_obj_t)&mp_module_cio }, \
        

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

// We need to provide a declaration/definition of alloca()
#include <alloca.h>
