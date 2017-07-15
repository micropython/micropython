#include <stdint.h>

#ifndef __INCLUDED_MPCONFIGPORT_H
#define __INCLUDED_MPCONFIGPORT_H

#define MICROPY_PY_SYS_PLATFORM                     "Atmel SAMD21"

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)

// options to control how Micro Python is built

#define MICROPY_QSTR_BYTES_IN_HASH  (1)
#define MICROPY_ALLOC_PATH_MAX      (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (16)
// TODO(tannewt): Reduce this number if we want the REPL to function under 512
// free bytes.
// #define MICROPY_ALLOC_PARSE_RULE_INIT   (64)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_COMP_MODULE_CONST   (1)
#define MICROPY_COMP_CONST          (1)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (1)
#define MICROPY_MEM_STATS           (0)
#define MICROPY_DEBUG_PRINTERS      (0)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_GC_ALLOC_THRESHOLD  (0)
#define MICROPY_REPL_EVENT_DRIVEN   (0)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_HELPER_LEXER_UNIX   (0)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_ENABLE_DOC_STRING   (0)
//#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_NORMAL)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_PY_ASYNC_AWAIT      (0)
#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_ENUMERATE (1)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT circuitpython_help_text
#define MICROPY_PY_BUILTINS_INPUT   (1)
#define MICROPY_PY_BUILTINS_FILTER  (1)
#define MICROPY_PY_BUILTINS_SET     (1)
#define MICROPY_PY_BUILTINS_SLICE   (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS (1)
#define MICROPY_PY_BUILTINS_PROPERTY (1)
#define MICROPY_PY_BUILTINS_MIN_MAX (1)
#define MICROPY_PY___FILE__         (1)
#define MICROPY_PY_GC               (1)
#define MICROPY_PY_ARRAY            (1)
#define MICROPY_PY_ATTRTUPLE        (1)
#define MICROPY_PY_COLLECTIONS      (1)
#define MICROPY_PY_DESCRIPTORS      (1)
#define MICROPY_PY_MATH             (1)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (0)
#define MICROPY_PY_URANDOM          (0)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS (0)
#define MICROPY_PY_STRUCT           (1)
#define MICROPY_PY_SYS              (1)
#define MICROPY_CPYTHON_COMPAT      (0)
// If you change MICROPY_LONGINT_IMPL, also change MPY_TOOL_LONGINT_IMPL in mpconfigport.mk.
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_NONE)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_STREAMS_NON_BLOCK   (1)

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    (437) /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_VOLUMES          (4)
#define MICROPY_FATFS_MULTI_PARTITION  (1)
#define MICROPY_FATFS_NUM_PERSISTENT   (1)
// Only enable this if you really need it. It allocates a byte cache of this
// size.
// #define MICROPY_FATFS_MAX_SS           (4096)

#define FILESYSTEM_BLOCK_SIZE       (512)

#define MICROPY_VFS                 (1)
#define MICROPY_VFS_FAT             (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_MODULE_WEAK_LINKS   (0)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_USE_INTERNAL_PRINTF (1)
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_IO_FILEIO        (1)
#define MICROPY_READER_VFS        (1)
#define MICROPY_PERSISTENT_CODE_LOAD (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE (1)

#define MICROPY_KBD_EXCEPTION       (1)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

#define MICROPY_STACK_CHECK         (1)

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open_obj mp_vfs_open_obj

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_help), (mp_obj_t)&mp_builtin_help_obj }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&mp_builtin_input_obj }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

// board specific definitions
#include "mpconfigboard.h"

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t microcontroller_module;
extern const struct _mp_obj_module_t bitbangio_module;
extern const struct _mp_obj_module_t audioio_module;
extern const struct _mp_obj_module_t analogio_module;
extern const struct _mp_obj_module_t digitalio_module;
extern const struct _mp_obj_module_t pulseio_module;
extern const struct _mp_obj_module_t busio_module;
extern const struct _mp_obj_module_t board_module;
extern const struct _mp_obj_module_t os_module;
extern const struct _mp_obj_module_t random_module;
extern const struct _mp_obj_module_t storage_module;
extern const struct _mp_obj_module_t time_module;
extern const struct _mp_obj_module_t neopixel_write_module;
extern const struct _mp_obj_module_t uheap_module;
extern const struct _mp_obj_module_t samd_module;
extern const struct _mp_obj_module_t touchio_module;
extern const struct _mp_obj_module_t usb_hid_module;

// Internal flash size dependent settings.
#if BOARD_FLASH_SIZE > 192000
    #define MICROPY_PY_BUILTINS_STR_CENTER (1)
    #define MICROPY_PY_BUILTINS_STR_PARTITION (1)
    #define MICROPY_PY_BUILTINS_FROZENSET (1)
    #define MICROPY_PY_BUILTINS_STR_SPLITLINES (1)
    #define MICROPY_PY_BUILTINS_REVERSED (1)
    #define MICROPY_PY_URE (1)
    #define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
    #define MICROPY_PY_FRAMEBUF         (1)
    #define EXTRA_BUILTIN_MODULES \
        { MP_OBJ_NEW_QSTR(MP_QSTR_audioio), (mp_obj_t)&audioio_module }, \
        { MP_OBJ_NEW_QSTR(MP_QSTR_pulseio), (mp_obj_t)&pulseio_module }, \
        { MP_OBJ_NEW_QSTR(MP_QSTR_bitbangio), (mp_obj_t)&bitbangio_module }
    #define EXPRESS_BOARD
#else
    #define MICROPY_PY_BUILTINS_REVERSED (0)
    #define MICROPY_PY_MICROPYTHON_MEM_INFO (0)
    #define MICROPY_PY_FRAMEBUF         (0)
    #define EXTRA_BUILTIN_MODULES
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_touchio), (mp_obj_t)&touchio_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_microcontroller), (mp_obj_t)&microcontroller_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_analogio), (mp_obj_t)&analogio_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_digitalio), (mp_obj_t)&digitalio_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_busio), (mp_obj_t)&busio_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_board), (mp_obj_t)&board_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_os), (mp_obj_t)&os_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_random), (mp_obj_t)&random_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_storage), (mp_obj_t)&storage_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t)&time_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_neopixel_write),(mp_obj_t)&neopixel_write_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_samd),(mp_obj_t)&samd_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_usb_hid),(mp_obj_t)&usb_hid_module }, \
    EXTRA_BUILTIN_MODULES

#define MICROPY_PORT_BUILTIN_DEBUG_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_uheap),(mp_obj_t)&uheap_module }

#ifndef MICROPY_PIN_DEFS_PORT_H
#define MICROPY_PIN_DEFS_PORT_H "pins.h"
#endif

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#ifdef __linux__
#define MICROPY_MIN_USE_STDOUT (1)
#endif

#ifdef __thumb__
#define MICROPY_MIN_USE_CORTEX_CPU (1)
#define MICROPY_MIN_USE_SAMD21_MCU (1)
#endif

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    vstr_t *repl_line; \
    struct tc_module* audioout_sample_timer; \
    struct tc_module* audioout_block_counter; \
    struct dac_module* audioout_dac_instance; \
    struct events_resource* audioout_sample_event; \
    struct events_resource* audioout_block_event; \
    struct events_resource* audioout_dac_event; \
    struct tc_module* pulseout_tc_instance; \
    FLASH_ROOT_POINTERS \

void run_background_tasks(void);
#define MICROPY_VM_HOOK_LOOP run_background_tasks();
#define MICROPY_VM_HOOK_RETURN run_background_tasks();

#define CIRCUITPY_AUTORELOAD_DELAY_MS 500
#define CIRCUITPY_BOOT_OUTPUT_FILE "/boot_out.txt"

#endif  // __INCLUDED_MPCONFIGPORT_H
