#include <stdint.h>

// options to control how MicroPython is built

#define MICROPY_CONFIG_ROM_LEVEL    (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

#if defined(__ARM_ARCH_ISA_ARM)
#define MICROPY_EMIT_ARM            (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#elif defined(__ARM_ARCH_ISA_THUMB)
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))
#endif

#define MICROPY_MALLOC_USES_ALLOCATED_SIZE (1)
#define MICROPY_MEM_STATS           (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_KBD_EXCEPTION       (0)
#define MICROPY_HELPER_REPL         (0)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_WARNINGS            (1)
#define MICROPY_PY_BUILTINS_INPUT   (0)
#define MICROPY_PY_BUILTINS_HELP    (0)
#define MICROPY_PY_IO_IOBASE        (0)
#define MICROPY_PY_SYS_PLATFORM     "qemu-arm"
#define MICROPY_PY_SYS_STDFILES     (0)
#define MICROPY_PY_SYS_STDIO_BUFFER (0)
#define MICROPY_PY_SELECT           (0)
#define MICROPY_PY_TIME             (0)
#define MICROPY_PY_ASYNCIO          (0)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE "ports/qemu-arm/modmachine.c"
#define MICROPY_PY_MACHINE_PIN_BASE (1)
#define MICROPY_VFS                 (1)

// type definitions for the specific machine

#define MP_SSIZE_MAX (0x7fffffff)

#define UINT_FMT "%lu"
#define INT_FMT "%ld"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

// We need an implementation of the log2 function which is not a macro.
#define MP_NEED_LOG2 (1)

#ifdef TEST
#include "shared/upytesthelper/upytesthelper.h"
#undef MP_PLAT_PRINT_STRN
#define MP_PLAT_PRINT_STRN(str, len) upytest_output(str, len)
#endif
