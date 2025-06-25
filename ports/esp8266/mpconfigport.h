// Options to control how MicroPython is built for this port,
// overriding defaults in py/mpconfig.h.

// This needs to be defined before any ESP SDK headers are included.
#define USE_US_TIMER 1

// Board-specific definitions
#include "mpconfigboard.h"

#include <stdbool.h>
#include <stdint.h>

// Set the rom feature level.
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

// Disable items from the extra level that are not wanted.
#define MICROPY_COMP_MODULE_CONST               (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN        (0)
#define MICROPY_COMP_RETURN_IF_EXPR             (0)
#define MICROPY_OPT_LOAD_ATTR_FAST_PATH         (0)
#define MICROPY_OPT_MAP_LOOKUP_CACHE            (0)
#define MICROPY_OPT_MPZ_BITWISE                 (0)
#define MICROPY_OPT_MATH_FACTORIAL              (0)
#define MICROPY_REPL_EMACS_KEYS                 (0)
#define MICROPY_PY_BUILTINS_COMPLEX             (0)
#define MICROPY_PY_DELATTR_SETATTR              (0)
#define MICROPY_PY_BUILTINS_STR_CENTER          (0)
#define MICROPY_PY_BUILTINS_STR_PARTITION       (0)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES      (0)
#define MICROPY_PY_BUILTINS_SLICE_INDICES       (0)
#define MICROPY_PY_BUILTINS_COMPILE             (0)
#define MICROPY_PY_BUILTINS_EXECFILE            (0)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED      (0)
#define MICROPY_PY_BUILTINS_POW3                (0)
#define MICROPY_PY___FILE__                     (0)
#define MICROPY_PY_MATH_CONSTANTS               (0)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS       (0)
#define MICROPY_PY_MATH_FACTORIAL               (0)
#define MICROPY_PY_MATH_ISCLOSE                 (0)
#define MICROPY_PY_SYS_PS1_PS2                  (0)
#define MICROPY_PY_BINASCII_CRC32               (0)
#define MICROPY_PY_RANDOM_EXTRA_FUNCS           (0)

// Configure other options.
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
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_REPL_EVENT_DRIVEN   (0)
#define MICROPY_USE_INTERNAL_ERRNO  (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT esp_help_text
#define MICROPY_PY_HASHLIB_SHA1     (MICROPY_PY_SSL && MICROPY_SSL_AXTLS)
#define MICROPY_PY_RANDOM_SEED_INIT_FUNC (*WDEV_HWRNG)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS (1)
#define MICROPY_PY_TIME_INCLUDEFILE "ports/esp8266/modtime.c"
#define MICROPY_PY_LWIP             (1)
#define MICROPY_PY_LWIP_SOCK_RAW    (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE "ports/esp8266/modmachine.c"
#define MICROPY_PY_MACHINE_RESET    (1)
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS (1)
#define MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ (1)
#define MICROPY_PY_MACHINE_ADC      (1)
#define MICROPY_PY_MACHINE_ADC_INCLUDEFILE "ports/esp8266/machine_adc.c"
#define MICROPY_PY_MACHINE_ADC_READ (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW mp_pin_make_new
#define MICROPY_PY_MACHINE_BITSTREAM (1)
#define MICROPY_PY_MACHINE_DHT_READINTO (1)
#define MICROPY_PY_MACHINE_PULSE    (1)
#define MICROPY_PY_MACHINE_PWM      (1)
#define MICROPY_PY_MACHINE_PWM_DUTY (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE "ports/esp8266/machine_pwm.c"
#define MICROPY_PY_MACHINE_SOFTI2C  (1)
#define MICROPY_PY_MACHINE_SPI      (1)
#define MICROPY_PY_MACHINE_SOFTSPI  (1)
#define MICROPY_PY_MACHINE_UART     (1)
#define MICROPY_PY_MACHINE_UART_INCLUDEFILE "ports/esp8266/machine_uart.c"
#define MICROPY_PY_MACHINE_WDT      (1)
#define MICROPY_PY_MACHINE_WDT_INCLUDEFILE "ports/esp8266/machine_wdt.c"
#define MICROPY_PY_NETWORK (1)
#ifndef MICROPY_PY_NETWORK_HOSTNAME_DEFAULT
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp8266"
#endif
#define MICROPY_PY_NETWORK_INCLUDEFILE "ports/esp8266/modnetwork.h"
#define MICROPY_PY_NETWORK_MODULE_GLOBALS_INCLUDEFILE "ports/esp8266/modnetwork_globals.h"
#define MICROPY_PY_WEBSOCKET        (1)
#define MICROPY_PY_ONEWIRE          (1)
#define MICROPY_PY_WEBREPL          (1)
#define MICROPY_PY_WEBREPL_DELAY    (20)
#define MICROPY_PY_WEBREPL_STATIC_FILEBUF (1)
#define MICROPY_PY_OS_INCLUDEFILE   "ports/esp8266/modos.c"
#define MICROPY_PY_OS_DUPTERM       (2)
#define MICROPY_PY_OS_DUPTERM_NOTIFY (1)
#define MICROPY_PY_OS_DUPTERM_STREAM_DETACHED_ATTACHED (1)
#define MICROPY_PY_OS_SYNC          (1)
#define MICROPY_PY_OS_UNAME         (1)
#define MICROPY_PY_OS_UNAME_RELEASE_DYNAMIC (1)
#define MICROPY_PY_OS_URANDOM       (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_WARNINGS            (1)
#define MICROPY_PY_STR_BYTES_CMP_WARN (1)
#define MICROPY_STREAMS_POSIX_API   (1)
#define MICROPY_MODULE_FROZEN_LEXER mp_lexer_new_from_str32

#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_MAX_SS           (4096)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_ESP8266_APA102         (1)

// Print error information at reboot time if the board crashed.
#define MICROPY_HW_HARD_FAULT_DEBUG    (0)

// No blocking wait-for-event on ESP8266, only non-blocking pump of the "OS" event
// loop
//
// TODO: When TIMEOUT_MS==-1, it may be possible to have MICROPY_INTERNAL_WFE() call the "waiti" instruction.
// See mp_machine_idle() and mp_machine_lightsleep() in esp8266/modmachine.c
//
// Note: We have to scope the declaration of ets_loop_iter() here as there are multiple incompatible
// definitions at compile time between the SDK and axTLS!
#define MICROPY_INTERNAL_WFE(TIMEOUT_MS)
#define MICROPY_INTERNAL_EVENT_HOOK \
    do { \
        extern bool ets_loop_iter(void); \
        ets_loop_iter(); \
    } while (0)

#define MICROPY_VM_HOOK_COUNT (10)
#define MICROPY_VM_HOOK_INIT static uint vm_hook_divisor = MICROPY_VM_HOOK_COUNT;
#define MICROPY_VM_HOOK_POLL if (--vm_hook_divisor == 0) { \
        vm_hook_divisor = MICROPY_VM_HOOK_COUNT; \
        MICROPY_INTERNAL_EVENT_HOOK; \
}
#define MICROPY_VM_HOOK_LOOP MICROPY_VM_HOOK_POLL
#define MICROPY_VM_HOOK_RETURN MICROPY_VM_HOOK_POLL

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

#define MP_STATE_PORT MP_STATE_VM

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
