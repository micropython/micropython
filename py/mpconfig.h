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

// Whether to support float and complex types
#ifndef MICROPY_ENABLE_FLOAT
#define MICROPY_ENABLE_FLOAT (0)
#endif

// Whether to support slice object and correspondingly
// slice subscript operators
#ifndef MICROPY_ENABLE_SLICE
#define MICROPY_ENABLE_SLICE (1)
#endif

// Enable features which improve CPython compatibility
// but may lead to more code size/memory usage.
// TODO: Originally intended as generic category to not
// add bunch of once-off options. May need refactoring later
#ifndef MICROPY_CPYTHON_COMPAT
#define MICROPY_CPYTHON_COMPAT (1)
#endif

/*****************************************************************************/
/* Miscellaneous settings                                                    */

#define BITS_PER_BYTE (8)
#define BITS_PER_WORD (BITS_PER_BYTE * BYTES_PER_WORD)
// machine_int_t value with most significant bit set
#define WORD_MSBIT_HIGH (((machine_uint_t)1) << (BYTES_PER_WORD * 8 - 1))

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
