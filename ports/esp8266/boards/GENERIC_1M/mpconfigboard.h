#define MICROPY_HW_BOARD_NAME "ESP module (1M)"
#define MICROPY_HW_MCU_NAME "ESP8266"

#define MICROPY_PERSISTENT_CODE_LOAD    (1)
#define MICROPY_EMIT_XTENSA             (1)
#define MICROPY_EMIT_INLINE_XTENSA      (1)

#define MICROPY_DEBUG_PRINTERS          (1)
#define MICROPY_ERROR_REPORTING         (MICROPY_ERROR_REPORTING_NORMAL)

#define MICROPY_READER_VFS              (MICROPY_VFS)
#define MICROPY_VFS                     (1)

#define MICROPY_PY_BUILTINS_SLICE_ATTRS (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS  (1)
#define MICROPY_PY_IO_FILEIO            (1)
#define MICROPY_PY_SYS_STDIO_BUFFER     (1)
#define MICROPY_PY_URE_SUB              (1)
#define MICROPY_PY_UCRYPTOLIB           (1)
#define MICROPY_PY_FRAMEBUF             (1)
