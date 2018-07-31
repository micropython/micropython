#include <mpconfigport.h>

#undef MICROPY_EMIT_XTENSA
#define MICROPY_EMIT_XTENSA             (0)
#undef MICROPY_EMIT_INLINE_XTENSA
#define MICROPY_EMIT_INLINE_XTENSA      (0)

#undef MICROPY_DEBUG_PRINTERS
#define MICROPY_DEBUG_PRINTERS          (0)

#undef MICROPY_ERROR_REPORTING
#define MICROPY_ERROR_REPORTING         (MICROPY_ERROR_REPORTING_TERSE)

#undef MICROPY_VFS
#define MICROPY_VFS                     (0)
#undef MICROPY_VFS_FAT
#define MICROPY_VFS_FAT                 (0)

#undef MICROPY_PERSISTENT_CODE_LOAD
#define MICROPY_PERSISTENT_CODE_LOAD    (0)

#undef MICROPY_PY_IO_FILEIO
#define MICROPY_PY_IO_FILEIO            (0)

#undef MICROPY_PY_SYS_STDIO_BUFFER
#define MICROPY_PY_SYS_STDIO_BUFFER     (0)
#undef MICROPY_PY_BUILTINS_SLICE_ATTRS
#define MICROPY_PY_BUILTINS_SLICE_ATTRS (0)
#undef MICROPY_PY_ALL_SPECIAL_METHODS
#define MICROPY_PY_ALL_SPECIAL_METHODS  (0)

#undef MICROPY_PY_FRAMEBUF
#define MICROPY_PY_FRAMEBUF             (0)

#undef mp_import_stat
#undef mp_builtin_open
#undef mp_builtin_open_obj
