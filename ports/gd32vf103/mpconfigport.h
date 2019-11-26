#include <stdint.h>

#define MICROPY_ENABLE_COMPILER     (1)

#define MICROPY_USE_INTERNAL_ERRNO  (1)
#define MICROPY_USE_INTERNAL_PRINTF (1)

#ifdef __GNUC__
#define MICROPY_OPT_COMPUTED_GOTO   (1)
#else
#define MICROPY_OPT_COMPUTED_GOTO   (0)
#endif
#define MICROPY_OPT_MPZ_BITWISE     (1)

#define MICROPY_ALLOC_PATH_MAX      (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (16)
#define MICRAPY_STACK_CHECK         (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)

#define MICROPY_COMP_MODULE_CONST   (1)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (1)

#define MICROPY_ENABLE_GC           (1)
#define MICROPY_GC_STACK_ENTRY_TYPE uint16_t
#define MICROPY_GC_ALLOC_THRESHOLD  (0)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)

#define MICROPY_PY_ASYNC_AWAIT      (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_COLLECTIONS      (0)
#define MICROPY_PY_MATH             (0)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (0)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_UTIME_MP_HAL     (1)
#define MICROPY_PY_UTIME_TICKS_PERIOD (1U << 30)

#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_NONE)

#if 0 // for tests
#include <limits.h>
#define MICROPY_CPYTHON_COMPAT (1)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (1)
#define MICROPY_PY_SYS_MAXSIZE (1)
#define MICROPY_ENABLE_SOURCE_LINE (1)
#define MP_SSIZE_MAX INT_MAX
#undef MICROPY_ERROR_REPORTING
#endif

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)(p))

#define INT_FMT "%d"
#define UINT_FMT "%u"
typedef int mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t mp_module_utime;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine) }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) },

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_HW_BOARD_NAME "LonganNano"
#define MICROPY_HW_MCU_NAME "gd32vf103"

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];
