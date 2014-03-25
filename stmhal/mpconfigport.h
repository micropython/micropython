#include <stdint.h>

// options to control how Micro Python is built

#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_REPL_HELPERS (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PATH_MAX            (128)
/* Enable FatFS LFNs
    0: Disable LFN feature.
    1: Enable LFN with static working buffer on the BSS. Always NOT reentrant.
    2: Enable LFN with dynamic working buffer on the STACK.
    3: Enable LFN with dynamic working buffer on the HEAP.
*/
#define MICROPY_ENABLE_LFN          (1)
#define MICROPY_LFN_CODE_PAGE       (437) /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */

// extra built in names to add to the global namespace
extern const struct _mp_obj_fun_native_t mp_builtin_help_obj;
extern const struct _mp_obj_fun_native_t mp_builtin_input_obj;
extern const struct _mp_obj_fun_native_t mp_builtin_open_obj;
#define MICROPY_EXTRA_BUILTINS \
    { MP_QSTR_help, (mp_obj_t)&mp_builtin_help_obj }, \
    { MP_QSTR_input, (mp_obj_t)&mp_builtin_input_obj }, \
    { MP_QSTR_open, (mp_obj_t)&mp_builtin_open_obj },

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t os_module;
extern const struct _mp_obj_module_t pyb_module;
extern const struct _mp_obj_module_t time_module;
#define MICROPY_EXTRA_BUILTIN_MODULES \
    { MP_QSTR_os, (mp_obj_t)&os_module }, \
    { MP_QSTR_pyb, (mp_obj_t)&pyb_module }, \
    { MP_QSTR_time, (mp_obj_t)&time_module }, \

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

#define UINT_FMT "%lu"
#define INT_FMT "%ld"

typedef int32_t machine_int_t; // must be pointer size
typedef uint32_t machine_uint_t; // must be pointer size
typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size

// There is no classical C heap in bare-metal ports, only Python
// garbage-collected heap. For completeness, emulate C heap via
// GC heap. Note that MicroPython core never uses malloc() and friends,
// so these defines are mostly to help extension module writers.
#define malloc gc_alloc
#define free gc_free
#define realloc gc_realloc

#define USE_DEVICE_MODE
//#define USE_HOST_MODE

// board specific definitions
#include "mpconfigboard.h"
