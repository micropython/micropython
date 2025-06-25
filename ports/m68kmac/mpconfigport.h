#include <stdint.h>

// options to control how MicroPython is built

#define MICROPY_OBJ_REPR (MICROPY_OBJ_REPR_B)

// Use the minimal starting configuration (disables all optional features).
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_FULL_FEATURES)

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER     (1)

#define MICROPY_QSTR_EXTRA_POOL           mp_qstr_frozen_const_pool
#define MICROPY_ENABLE_GC                 (1)
#define MICROPY_HELPER_REPL               (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT    (1)
#define MICROPY_PY_UCTYPES                (1)
#define MICROPY_PY_SYS_STDFILES           (1)
#define MICROPY_STACK_CHECK               (0)
#define MICROPY_LONGINT_IMPL              (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL                (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_READER_POSIX              (0)
#define MICROPY_READER_VFS                (1)
#define MICROPY_VFS                       (1)
#define MICROPY_PY_OS_STATVFS             (0)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS      (1)
#define MICROPY_EPOCH_IS_1970             (0)
#define MICROPY_PY_TIME_INCLUDEFILE       "ports/m68kmac/modtime.c"

#define MP_SSIZE_MAX LONG_MAX

#define MICROPY_ALLOC_PATH_MAX            (256)

// Use the minimum headroom in the chunk allocator for parse nodes.
#define MICROPY_ALLOC_PARSE_CHUNK_INIT    (16)

// type definitions for the specific machine

typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_HW_BOARD_NAME "macplus"
#define MICROPY_HW_MCU_NAME "m68000"

#define MICROPY_MIN_USE_STDOUT (0)
#define MICROPY_HEAP_SIZE      (100 * 1024)

#define MP_STATE_PORT MP_STATE_VM

typedef struct _mp_obj_type_t mp_obj_type_t;
extern const mp_obj_type_t mp_type_vfs_mac;
#define MICROPY_VFS_PORT { MP_ROM_QSTR(MP_QSTR_VfsMac), MP_ROM_PTR(&mp_type_vfs_mac) }
