// Options to control how MicroPython is built for this port,
// overriding defaults in py/mpconfig.h.

#include <stdint.h>
#include <alloca.h>
#include "rom/ets_sys.h"

// object representation and NLR handling
#define MICROPY_OBJ_REPR                    (MICROPY_OBJ_REPR_A)
#define MICROPY_NLR_SETJMP                  (1)

// memory allocation policies
#define MICROPY_ALLOC_PATH_MAX              (128)

// emitters
#define MICROPY_PERSISTENT_CODE_LOAD        (1)

// compiler configuration
#define MICROPY_COMP_MODULE_CONST           (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN    (1)

// optimisations
#define MICROPY_OPT_COMPUTED_GOTO           (1)
#define MICROPY_OPT_MPZ_BITWISE             (1)

// Python internal features
#define MICROPY_READER_VFS                  (1)
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_ENABLE_FINALISER            (1)
#define MICROPY_STACK_CHECK                 (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_KBD_EXCEPTION               (1)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_REPL_EMACS_KEYS             (1)
#define MICROPY_REPL_AUTO_INDENT            (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ENABLE_SOURCE_LINE          (1)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_NORMAL)
#define MICROPY_WARNINGS                    (1)
#define MICROPY_FLOAT_IMPL                  (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_CPYTHON_COMPAT              (1)
#define MICROPY_STREAMS_NON_BLOCK           (1)
#define MICROPY_STREAMS_POSIX_API           (1)
#define MICROPY_MODULE_BUILTIN_INIT         (1)
#define MICROPY_MODULE_WEAK_LINKS           (1)
#define MICROPY_MODULE_FROZEN_STR           (0)
#define MICROPY_MODULE_FROZEN_MPY           (1)
#define MICROPY_QSTR_EXTRA_POOL             mp_qstr_frozen_const_pool
#define MICROPY_CAN_OVERRIDE_BUILTINS       (1)
#define MICROPY_USE_INTERNAL_ERRNO          (1)
#define MICROPY_USE_INTERNAL_PRINTF         (0) // ESP32 SDK requires its own printf
#define MICROPY_ENABLE_SCHEDULER            (1)
#define MICROPY_SCHEDULER_DEPTH             (8)
#define MICROPY_VFS                         (1)
#define MICROPY_VFS_FAT                     (1)

// control over Python builtins
#define MICROPY_PY_FUNCTION_ATTRS           (1)
#define MICROPY_PY_DESCRIPTORS              (1)
#define MICROPY_PY_STR_BYTES_CMP_WARN       (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE     (1)
#define MICROPY_PY_BUILTINS_STR_CENTER      (1)
#define MICROPY_PY_BUILTINS_STR_PARTITION   (1)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES  (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY       (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW      (1)
#define MICROPY_PY_BUILTINS_SET             (1)
#define MICROPY_PY_BUILTINS_SLICE           (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS     (1)
#define MICROPY_PY_BUILTINS_FROZENSET       (1)
#define MICROPY_PY_BUILTINS_PROPERTY        (1)
#define MICROPY_PY_BUILTINS_RANGE_ATTRS     (1)
#define MICROPY_PY_BUILTINS_ROUND_INT       (1)
#define MICROPY_PY_BUILTINS_TIMEOUTERROR    (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS      (1)
#define MICROPY_PY_BUILTINS_COMPILE         (1)
#define MICROPY_PY_BUILTINS_ENUMERATE       (1)
#define MICROPY_PY_BUILTINS_EXECFILE        (1)
#define MICROPY_PY_BUILTINS_FILTER          (1)
#define MICROPY_PY_BUILTINS_REVERSED        (1)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED  (1)
#define MICROPY_PY_BUILTINS_INPUT           (1)
#define MICROPY_PY_BUILTINS_MIN_MAX         (1)
#define MICROPY_PY_BUILTINS_POW3            (1)
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       esp32_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES    (1)
#define MICROPY_PY___FILE__                 (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO     (1)
#define MICROPY_PY_ARRAY                    (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN       (1)
#define MICROPY_PY_ATTRTUPLE                (1)
#define MICROPY_PY_COLLECTIONS              (1)
#define MICROPY_PY_COLLECTIONS_DEQUE        (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT  (1)
#define MICROPY_PY_MATH                     (1)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS   (1)
#define MICROPY_PY_CMATH                    (1)
#define MICROPY_PY_GC                       (1)
#define MICROPY_PY_IO                       (1)
#define MICROPY_PY_IO_IOBASE                (1)
#define MICROPY_PY_IO_FILEIO                (1)
#define MICROPY_PY_IO_BYTESIO               (1)
#define MICROPY_PY_IO_BUFFEREDWRITER        (1)
#define MICROPY_PY_STRUCT                   (1)
#define MICROPY_PY_SYS                      (1)
#define MICROPY_PY_SYS_MAXSIZE              (1)
#define MICROPY_PY_SYS_MODULES              (1)
#define MICROPY_PY_SYS_EXIT                 (1)
#define MICROPY_PY_SYS_STDFILES             (1)
#define MICROPY_PY_SYS_STDIO_BUFFER         (1)
#define MICROPY_PY_UERRNO                   (1)
#define MICROPY_PY_USELECT                  (1)
#define MICROPY_PY_UTIME_MP_HAL             (1)
#define MICROPY_PY_THREAD                   (1)
#define MICROPY_PY_THREAD_GIL               (1)
#define MICROPY_PY_THREAD_GIL_VM_DIVISOR    (32)

// extended modules
#define MICROPY_PY_UCTYPES                  (1)
#define MICROPY_PY_UZLIB                    (1)
#define MICROPY_PY_UJSON                    (1)
#define MICROPY_PY_URE                      (1)
#define MICROPY_PY_URE_SUB                  (1)
#define MICROPY_PY_UHEAPQ                   (1)
#define MICROPY_PY_UTIMEQ                   (1)
#define MICROPY_PY_UHASHLIB                 (1)
#define MICROPY_PY_UHASHLIB_SHA1            (1)
#define MICROPY_PY_UHASHLIB_SHA256          (1)
#define MICROPY_PY_UCRYPTOLIB               (1)
#define MICROPY_PY_UBINASCII                (1)
#define MICROPY_PY_UBINASCII_CRC32          (1)
#define MICROPY_PY_URANDOM                  (1)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS      (1)
#define MICROPY_PY_OS_DUPTERM               (1)
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new
#define MICROPY_PY_MACHINE_PULSE            (1)
#define MICROPY_PY_MACHINE_I2C              (1)
#define MICROPY_PY_MACHINE_SPI              (1)
#define MICROPY_PY_MACHINE_SPI_MSB          (0)
#define MICROPY_PY_MACHINE_SPI_LSB          (1)
#define MICROPY_PY_MACHINE_SPI_MAKE_NEW     machine_hw_spi_make_new
#define MICROPY_HW_SOFTSPI_MIN_DELAY        (0)
#define MICROPY_HW_SOFTSPI_MAX_BAUDRATE     (ets_get_cpu_frequency() * 1000000 / 200) // roughly
#define MICROPY_PY_USSL                     (1)
#define MICROPY_SSL_MBEDTLS                 (1)
#define MICROPY_PY_USSL_FINALISER           (1)
#define MICROPY_PY_UWEBSOCKET               (1)
#define MICROPY_PY_WEBREPL                  (1)
#define MICROPY_PY_FRAMEBUF                 (1)
#define MICROPY_PY_USOCKET_EVENTS           (MICROPY_PY_WEBREPL)

// fatfs configuration
#define MICROPY_FATFS_ENABLE_LFN            (1)
#define MICROPY_FATFS_RPATH                 (2)
#define MICROPY_FATFS_MAX_SS                (4096)
#define MICROPY_FATFS_LFN_CODE_PAGE         (437) /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define mp_type_fileio                      mp_type_vfs_fat_fileio
#define mp_type_textio                      mp_type_vfs_fat_textio

// use vfs's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open mp_vfs_open
#define mp_builtin_open_obj mp_vfs_open_obj

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&mp_builtin_input_obj }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t esp_module;
extern const struct _mp_obj_module_t esp32_module;
extern const struct _mp_obj_module_t utime_module;
extern const struct _mp_obj_module_t uos_module;
extern const struct _mp_obj_module_t mp_module_usocket;
extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t mp_module_network;
extern const struct _mp_obj_module_t mp_module_onewire;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_esp), (mp_obj_t)&esp_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_esp32), (mp_obj_t)&esp32_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_utime), (mp_obj_t)&utime_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_uos), (mp_obj_t)&uos_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_usocket), (mp_obj_t)&mp_module_usocket }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_machine), (mp_obj_t)&mp_module_machine }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_network), (mp_obj_t)&mp_module_network }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR__onewire), (mp_obj_t)&mp_module_onewire }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_uhashlib), (mp_obj_t)&mp_module_uhashlib }, \

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_binascii), (mp_obj_t)&mp_module_ubinascii }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_collections), (mp_obj_t)&mp_module_collections }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_errno), (mp_obj_t)&mp_module_uerrno }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_hashlib), (mp_obj_t)&mp_module_uhashlib }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_heapq), (mp_obj_t)&mp_module_uheapq }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_io), (mp_obj_t)&mp_module_io }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_json), (mp_obj_t)&mp_module_ujson }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_os), (mp_obj_t)&uos_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_random), (mp_obj_t)&mp_module_urandom }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_re), (mp_obj_t)&mp_module_ure }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_select), (mp_obj_t)&mp_module_uselect }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&mp_module_usocket }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_ssl), (mp_obj_t)&mp_module_ussl }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_struct), (mp_obj_t)&mp_module_ustruct }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t)&utime_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_zlib), (mp_obj_t)&mp_module_uzlib }, \

#define MP_STATE_PORT MP_STATE_VM

struct _machine_timer_obj_t;

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    mp_obj_t machine_pin_irq_handler[40]; \
    struct _machine_timer_obj_t *machine_timer_obj_head; \

// type definitions for the specific machine

#define BYTES_PER_WORD (4)
#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p)))
#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)
#define MP_SSIZE_MAX (0x7fffffff)

// Note: these "critical nested" macros do not ensure cross-CPU exclusion,
// the only disable interrupts on the current CPU.  To full manage exclusion
// one should use portENTER_CRITICAL/portEXIT_CRITICAL instead.
#include "freertos/FreeRTOS.h"
#define MICROPY_BEGIN_ATOMIC_SECTION() portENTER_CRITICAL_NESTED()
#define MICROPY_END_ATOMIC_SECTION(state) portEXIT_CRITICAL_NESTED(state)

#if MICROPY_PY_USOCKET_EVENTS
#define MICROPY_PY_USOCKET_EVENTS_HANDLER extern void usocket_events_handler(void); usocket_events_handler();
#else
#define MICROPY_PY_USOCKET_EVENTS_HANDLER
#endif

#if MICROPY_PY_THREAD
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(void); \
        mp_handle_pending(); \
        MICROPY_PY_USOCKET_EVENTS_HANDLER \
        MP_THREAD_GIL_EXIT(); \
        MP_THREAD_GIL_ENTER(); \
    } while (0);
#else
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(void); \
        mp_handle_pending(); \
        MICROPY_PY_USOCKET_EVENTS_HANDLER \
        asm("waiti 0"); \
    } while (0);
#endif

#define UINT_FMT "%u"
#define INT_FMT "%d"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;
// ssize_t, off_t as required by POSIX-signatured functions in stream.h
#include <sys/types.h>

// board specifics

#define MICROPY_HW_BOARD_NAME "ESP32 module"
#define MICROPY_HW_MCU_NAME "ESP32"
#define MICROPY_PY_SYS_PLATFORM "esp32"
