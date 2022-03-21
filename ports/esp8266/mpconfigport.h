// Options to control how MicroPython is built for this port,
// overriding defaults in py/mpconfig.h.

// Board-specific definitions
#include "mpconfigboard.h"

#include <stdint.h>

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
#define MICROPY_GC_STACK_ENTRY_TYPE uint16_t
#define MICROPY_ALLOC_PATH_MAX      (128)
#define MICROPY_ALLOC_LEXER_INDENT_INIT (8)
#define MICROPY_ALLOC_PARSE_RULE_INIT   (48)
#define MICROPY_ALLOC_PARSE_RULE_INC    (8)
#define MICROPY_ALLOC_PARSE_RESULT_INC  (8)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT  (64)
#define MICROPY_DEBUG_PRINTER       (&mp_debug_print)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_REPL_EVENT_DRIVEN   (0)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_MODULE_BUILTIN_INIT (1)
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#define MICROPY_USE_INTERNAL_ERRNO  (1)
#define MICROPY_ENABLE_SCHEDULER    (1)
#define MICROPY_PY_DESCRIPTORS      (1)
#define MICROPY_PY_BUILTINS_COMPLEX (0)
#define MICROPY_PY_BUILTINS_STR_UNICODE (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_BUILTINS_ROUND_INT (1)
#define MICROPY_PY_BUILTINS_INPUT   (1)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT esp_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_PY___FILE__         (0)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (1)
#define MICROPY_PY_COLLECTIONS_DEQUE (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT (1)
#define MICROPY_PY_IO_IOBASE        (1)
#define MICROPY_PY_SYS_MAXSIZE      (1)
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_UERRNO           (1)
#define MICROPY_PY_UBINASCII        (1)
#define MICROPY_PY_UCTYPES          (1)
#define MICROPY_PY_UHASHLIB         (1)
#define MICROPY_PY_UHASHLIB_SHA1    (MICROPY_PY_USSL && MICROPY_SSL_AXTLS)
#define MICROPY_PY_UHEAPQ           (1)
#define MICROPY_PY_UTIMEQ           (1)
#define MICROPY_PY_UJSON            (1)
#define MICROPY_PY_URANDOM          (1)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC (*WDEV_HWRNG)
#define MICROPY_PY_URE              (1)
#define MICROPY_PY_USELECT          (1)
#define MICROPY_PY_UTIME_MP_HAL     (1)
#define MICROPY_PY_UZLIB            (1)
#define MICROPY_PY_LWIP             (1)
#define MICROPY_PY_LWIP_SOCK_RAW    (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW mp_pin_make_new
#define MICROPY_PY_MACHINE_BITSTREAM (1)
#define MICROPY_PY_MACHINE_PULSE    (1)
#define MICROPY_PY_MACHINE_PWM      (1)
#define MICROPY_PY_MACHINE_PWM_INIT (1)
#define MICROPY_PY_MACHINE_PWM_DUTY (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE "ports/esp8266/machine_pwm.c"
#define MICROPY_PY_MACHINE_I2C      (1)
#define MICROPY_PY_MACHINE_SOFTI2C  (1)
#define MICROPY_PY_MACHINE_SPI      (1)
#define MICROPY_PY_MACHINE_SOFTSPI  (1)
#define MICROPY_PY_UWEBSOCKET       (1)
#define MICROPY_PY_ONEWIRE          (1)
#define MICROPY_PY_WEBREPL          (1)
#define MICROPY_PY_WEBREPL_DELAY    (20)
#define MICROPY_PY_WEBREPL_STATIC_FILEBUF (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_UOS              (1)
#define MICROPY_PY_UOS_INCLUDEFILE  "ports/esp8266/moduos.c"
#define MICROPY_PY_OS_DUPTERM       (2)
#define MICROPY_PY_UOS_DUPTERM_NOTIFY (1)
#define MICROPY_PY_UOS_DUPTERM_STREAM_DETACHED_ATTACHED (1)
#define MICROPY_PY_UOS_UNAME        (1)
#define MICROPY_PY_UOS_UNAME_RELEASE_DYNAMIC (1)
#define MICROPY_PY_UOS_URANDOM      (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_WARNINGS            (1)
#define MICROPY_PY_STR_BYTES_CMP_WARN (1)
#define MICROPY_STREAMS_NON_BLOCK   (1)
#define MICROPY_STREAMS_POSIX_API   (1)
#define MICROPY_MODULE_FROZEN_LEXER mp_lexer_new_from_str32

#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_MAX_SS           (4096)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_ESP8266_APA102         (1)

#define MICROPY_EVENT_POLL_HOOK {ets_event_poll();}
#define MICROPY_VM_HOOK_COUNT (10)
#define MICROPY_VM_HOOK_INIT static uint vm_hook_divisor = MICROPY_VM_HOOK_COUNT;
#define MICROPY_VM_HOOK_POLL if (--vm_hook_divisor == 0) { \
        vm_hook_divisor = MICROPY_VM_HOOK_COUNT; \
        extern void ets_loop_iter(void); \
        ets_loop_iter(); \
}
#define MICROPY_VM_HOOK_LOOP MICROPY_VM_HOOK_POLL
#define MICROPY_VM_HOOK_RETURN MICROPY_VM_HOOK_POLL

#include "xtirq.h"
#define MICROPY_BEGIN_ATOMIC_SECTION() disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state) enable_irq(state)

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p)))

#define MP_SSIZE_MAX (0x7fffffff)

#define UINT_FMT "%u"
#define INT_FMT "%d"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;
typedef uint32_t sys_prot_t; // for modlwip
// ssize_t, off_t as required by POSIX-signatured functions in stream.h
#include <sys/types.h>

void *esp_native_code_commit(void *, size_t, void *);
#define MP_PLAT_COMMIT_EXEC(buf, len, reloc) esp_native_code_commit(buf, len, reloc)

// printer for debugging output, goes to UART only
extern const struct _mp_print_t mp_debug_print;

#if MICROPY_VFS_FAT
#define mp_type_fileio mp_type_vfs_fat_fileio
#define mp_type_textio mp_type_vfs_fat_textio
#elif MICROPY_VFS_LFS1
#define mp_type_fileio mp_type_vfs_lfs1_fileio
#define mp_type_textio mp_type_vfs_lfs1_textio
#elif MICROPY_VFS_LFS2
#define mp_type_fileio mp_type_vfs_lfs2_fileio
#define mp_type_textio mp_type_vfs_lfs2_textio
#endif

// use vfs's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open mp_vfs_open
#define mp_builtin_open_obj mp_vfs_open_obj

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t esp_module;
extern const struct _mp_obj_module_t network_module;
extern const struct _mp_obj_module_t utime_module;
extern const struct _mp_obj_module_t mp_module_lwip;
extern const struct _mp_obj_module_t mp_module_onewire;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_esp), MP_ROM_PTR(&esp_module) }, \
    { MP_ROM_QSTR(MP_QSTR_usocket), MP_ROM_PTR(&mp_module_lwip) }, \
    { MP_ROM_QSTR(MP_QSTR_network), MP_ROM_PTR(&network_module) }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&utime_module) }, \
    { MP_ROM_QSTR(MP_QSTR__onewire), MP_ROM_PTR(&mp_module_onewire) }, \

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    mp_obj_t pin_irq_handler[16]; \
    byte *uart0_rxbuf; \

// We need an implementation of the log2 function which is not a macro
#define MP_NEED_LOG2 (1)

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

// board specifics

#define MICROPY_MPHALPORT_H "esp_mphal.h"
#define MICROPY_PY_SYS_PLATFORM "esp8266"

#define MP_FASTCODE(n) __attribute__((section(".iram0.text." #n))) n
#define MICROPY_WRAP_MP_SCHED_EXCEPTION(f) MP_FASTCODE(f)
#define MICROPY_WRAP_MP_SCHED_KEYBOARD_INTERRUPT(f) MP_FASTCODE(f)
#define MICROPY_WRAP_MP_SCHED_SCHEDULE(f) MP_FASTCODE(f)

#define WDEV_HWRNG ((volatile uint32_t *)0x3ff20e44)

#define _assert(expr) ((expr) ? (void)0 : __assert_func(__FILE__, __LINE__, __func__, #expr))
