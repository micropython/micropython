#include <stdint.h>

// options to control how Micro Python is built

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
#define MICROPY_ALLOC_PATH_MAX      (128)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_MEM_STATS           (0)
#define MICROPY_DEBUG_PRINTERS      (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_STACK_CHECK         (0)
#define MICROPY_REPL_EVENT_DRIVEN   (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_HELPER_LEXER_UNIX   (0)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#define MICROPY_PY_BUILTINS_COMPLEX (0)
#define MICROPY_PY_BUILTINS_STR_UNICODE (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_BUILTINS_SET     (1)
#define MICROPY_PY_BUILTINS_SLICE   (1)
#define MICROPY_PY_BUILTINS_PROPERTY (1)
#define MICROPY_PY___FILE__         (0)
#define MICROPY_PY_GC               (1)
#define MICROPY_PY_ARRAY            (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (1)
#define MICROPY_PY_COLLECTIONS      (1)
#define MICROPY_PY_MATH             (1)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_STRUCT           (1)
#define MICROPY_PY_SYS              (1)
#define MICROPY_PY_SYS_MAXSIZE      (1)
#define MICROPY_PY_SYS_EXIT         (1)
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_UBINASCII        (1)
#define MICROPY_PY_UCTYPES          (1)
#define MICROPY_PY_UHASHLIB         (1)
#define MICROPY_PY_UHASHLIB_SHA1    (1)
#define MICROPY_PY_UJSON            (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_CPYTHON_COMPAT      (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_MODULE_FROZEN       (1)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p)))

#define MP_SSIZE_MAX (0x7fffffff)

#define UINT_FMT "%u"
#define INT_FMT "%d"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size
typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// extra built in names to add to the global namespace
extern const struct _mp_obj_fun_builtin_t mp_builtin_open_obj;
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t pyb_module;
extern const struct _mp_obj_module_t esp_module;
extern const struct _mp_obj_module_t network_module;
extern const struct _mp_obj_module_t utime_module;
extern const struct _mp_obj_module_t uos_module;
extern const struct _mp_obj_module_t mp_module_machine;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_pyb), (mp_obj_t)&pyb_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_esp), (mp_obj_t)&esp_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_network), (mp_obj_t)&network_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_utime), (mp_obj_t)&utime_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_uos), (mp_obj_t)&uos_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_machine), (mp_obj_t)&mp_module_machine }, \

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t)&utime_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_os), (mp_obj_t)&uos_module }, \

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    \
    /* Singleton instance of scan callback, meaning that there can
       be only one concurrent AP scan. */ \
    mp_obj_t scan_cb_obj; \

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

// board specifics

#define MICROPY_MPHALPORT_H "esp_mphal.h"
#define MICROPY_HW_BOARD_NAME "ESP module"
#define MICROPY_HW_MCU_NAME "ESP8266"
#define MICROPY_PY_SYS_PLATFORM "ESP8266"

#define _assert(expr) ((expr) ? (void)0 : __assert_func(__FILE__, __LINE__, __func__, #expr))
