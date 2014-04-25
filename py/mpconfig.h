// This file contains default configuration settings for MicroPython.
// You can override any of these options using mpconfigport.h file located
// in a directory of your port.

#include <mpconfigport.h>

// Any options not explicitly set in mpconfigport.h will get default
// values below.

/*****************************************************************************/
/* Micro Python emitters                                                     */

// Whether to emit CPython byte codes (for debugging/testing)
// Enabling this overrides all other emitters
#ifndef MICROPY_EMIT_CPYTHON
#define MICROPY_EMIT_CPYTHON (0)
#endif

// Whether to emit x64 native code
#ifndef MICROPY_EMIT_X64
#define MICROPY_EMIT_X64 (0)
#endif

// Whether to emit thumb native code
#ifndef MICROPY_EMIT_THUMB
#define MICROPY_EMIT_THUMB (0)
#endif

// Whether to enable the thumb inline assembler
#ifndef MICROPY_EMIT_INLINE_THUMB
#define MICROPY_EMIT_INLINE_THUMB (0)
#endif

/*****************************************************************************/
/* Internal debugging stuff                                                  */

// Whether to collect memory allocation stats
#ifndef MICROPY_MEM_STATS
#define MICROPY_MEM_STATS (0)
#endif

// Whether to build functions that print debugging info:
//   mp_token_show
//   mp_byte_code_print
//   mp_parse_node_print
#ifndef MICROPY_DEBUG_PRINTERS
#define MICROPY_DEBUG_PRINTERS (0)
#endif

/*****************************************************************************/
/* Fine control over Python features                                         */

// Whether to include the garbage collector
#ifndef MICROPY_ENABLE_GC
#define MICROPY_ENABLE_GC (0)
#endif

// Whether to enable finalisers in the garbage collector (ie call __del__)
#ifndef MICROPY_ENABLE_GC_FINALISER
#define MICROPY_ENABLE_GC_FINALISER (0)
#endif

// Whether to include REPL helper function
#ifndef MICROPY_ENABLE_REPL_HELPERS
#define MICROPY_ENABLE_REPL_HELPERS (0)
#endif

// Whether to include lexer helper function for unix
#ifndef MICROPY_ENABLE_LEXER_UNIX
#define MICROPY_ENABLE_LEXER_UNIX (0)
#endif

// Long int implementation
#define MICROPY_LONGINT_IMPL_NONE (0)
#define MICROPY_LONGINT_IMPL_LONGLONG (1)
#define MICROPY_LONGINT_IMPL_MPZ (2)

#ifndef MICROPY_LONGINT_IMPL
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_NONE)
#endif

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_LONGLONG
typedef long long mp_longint_impl_t;
#endif

// Whether to include information in the byte code to determine source
// line number (increases RAM usage, but doesn't slow byte code execution)
#ifndef MICROPY_ENABLE_SOURCE_LINE
#define MICROPY_ENABLE_SOURCE_LINE (0)
#endif

// Whether to include doc strings (increases RAM usage)
#ifndef MICROPY_ENABLE_DOC_STRING
#define MICROPY_ENABLE_DOC_STRING (0)
#endif

// Float and complex implementation
#define MICROPY_FLOAT_IMPL_NONE (0)
#define MICROPY_FLOAT_IMPL_FLOAT (1)
#define MICROPY_FLOAT_IMPL_DOUBLE (2)

#ifndef MICROPY_FLOAT_IMPL
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_NONE)
#endif

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define MICROPY_ENABLE_FLOAT (1)
#define MICROPY_FLOAT_C_FUN(fun) fun##f
typedef float mp_float_t;
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define MICROPY_ENABLE_FLOAT (1)
#define MICROPY_FLOAT_C_FUN(fun) fun
typedef double mp_float_t;
#else
#define MICROPY_ENABLE_FLOAT (0)
#endif

// Whether to provide "math" module
#ifndef MICROPY_ENABLE_MOD_MATH
#define MICROPY_ENABLE_MOD_MATH (1)
#endif

// Whether to provide "cmath" module
#ifndef MICROPY_ENABLE_MOD_CMATH
#define MICROPY_ENABLE_MOD_CMATH (0)
#endif

// Whether to provide "io" module
#ifndef MICROPY_ENABLE_MOD_IO
#define MICROPY_ENABLE_MOD_IO (1)
#endif

// Whether to provide "struct" module
#ifndef MICROPY_ENABLE_MOD_STRUCT
#define MICROPY_ENABLE_MOD_STRUCT (1)
#endif

// Whether to provide "sys" module
#ifndef MICROPY_ENABLE_MOD_SYS
#define MICROPY_ENABLE_MOD_SYS (1)
#endif

#ifndef MICROPY_MOD_SYS_STDFILES
#define MICROPY_MOD_SYS_STDFILES (0)
#endif

// Whether to support slice object and correspondingly
// slice subscript operators
#ifndef MICROPY_ENABLE_SLICE
#define MICROPY_ENABLE_SLICE (1)
#endif

// Whether to support the property object
#ifndef MICROPY_ENABLE_PROPERTY
#define MICROPY_ENABLE_PROPERTY (1)
#endif

// Enable features which improve CPython compatibility
// but may lead to more code size/memory usage.
// TODO: Originally intended as generic category to not
// add bunch of once-off options. May need refactoring later
#ifndef MICROPY_CPYTHON_COMPAT
#define MICROPY_CPYTHON_COMPAT (1)
#endif

// Maximum length of a path in the filesystem
// So we can allocate a buffer on the stack for path manipulation in import
#ifndef MICROPY_PATH_MAX
#define MICROPY_PATH_MAX (512)
#endif

// Whether to use computed gotos in the VM, or a switch
// Computed gotos are roughly 10% faster, and increase VM code size by a little
#ifndef MICROPY_USE_COMPUTED_GOTO
#define MICROPY_USE_COMPUTED_GOTO (0)
#endif

// Additional builtin function definitions - see builtintables.c:builtin_object_table for format.
#ifndef MICROPY_EXTRA_BUILTINS
#define MICROPY_EXTRA_BUILTINS
#endif

// Additional builtin module definitions - see builtintables.c:builtin_module_table for format.
#ifndef MICROPY_EXTRA_BUILTIN_MODULES
#define MICROPY_EXTRA_BUILTIN_MODULES
#endif

// Additional constant definitions for the compiler - see compile.c:mp_constants_table.
#ifndef MICROPY_EXTRA_CONSTANTS
#define MICROPY_EXTRA_CONSTANTS
#endif

/*****************************************************************************/
/* Miscellaneous settings                                                    */

// Allow to override static modifier for global objects, e.g. to use with
// object code analysis tools which don't support static symbols.
#ifndef STATIC
#define STATIC static
#endif

#define BITS_PER_BYTE (8)
#define BITS_PER_WORD (BITS_PER_BYTE * BYTES_PER_WORD)
// machine_int_t value with most significant bit set
#define WORD_MSBIT_HIGH (((machine_uint_t)1) << (BYTES_PER_WORD * 8 - 1))

#if !defined(MP_ENDIANNESS_LITTLE) && !defined(MP_ENDIANNESS_BIG)
// Just because most archs are such?
#define MP_ENDIANNESS_LITTLE (1)
#endif
// Ensure we don't accidentally set both endiannesses
#if MP_ENDIANNESS_BIG
#define MP_ENDIANNESS_LITTLE (0)
#endif

// printf format spec to use for machine_int_t and friends
#ifndef INT_FMT
#ifdef __LP64__
// Archs where machine_int_t == long, long != int
#define UINT_FMT "%lu"
#define INT_FMT "%ld"
#else
// Archs where machine_int_t == int
#define UINT_FMT "%u"
#define INT_FMT "%d"
#endif
#endif //INT_FMT
