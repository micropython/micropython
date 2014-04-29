// options to control how Micro Python is built

#define MICROPY_USE_READLINE        (0)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_MEM_STATS           (0)
#define MICROPY_PATH_MAX            (512)
#define MICROPY_DEBUG_PRINTERS      (1)
#define MICROPY_ENABLE_REPL_HELPERS (1)
#define MICROPY_ENABLE_LEXER_UNIX   (1)
#define MICROPY_ENABLE_MOD_CMATH    (1)
#define MICROPY_MOD_SYS_STDFILES    (1)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_DOUBLE)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)

#define MICROPY_EXTRA_CONSTANTS { "dummy", 0 } //can't have 0 sized array


// type definitions for the specific machine

typedef int machine_int_t; // must be pointer size
typedef unsigned int machine_uint_t; // must be pointer size

#define BYTES_PER_WORD sizeof(machine_int_t)

typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size

extern const struct _mp_obj_fun_native_t mp_builtin_open_obj;
#define MICROPY_EXTRA_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },


// sanity checks

#if !defined( _WIN32 ) || !defined( _MSC_VER ) || ( _MSC_VER < 1800 )
    #error Can only build for 32bit Windows with Visual Studio 2013 toolset
#endif 


//  CL specific overrides from mpconfig

#define NORETURN
#define DYNAMIC_ARRAY_DEFAULT_SIZE (1) //can't have zero-sized arrays in a struct


// CL specific definitions

#define inline __inline
#define restrict
#define snprintf _snprintf
#define alloca _alloca
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define PATH_MAX 260 //from minwindef.h
#define S_ISREG(m) (((m)& S_IFMT)==S_IFREG)
#define S_ISDIR(m) (((m)& S_IFMT)==S_IFDIR)

#include <stddef.h> //for NULL
#include <assert.h> //for assert

#pragma warning ( disable : 4715 ) //not all control paths return a value, when using nlr_raise
#pragma warning ( disable : 4716 ) //function must return a value, when using nlr_raise
