/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// This file contains default configuration settings for MicroPython.
// You can override any of these options using mpconfigport.h file located
// in a directory of your port.

#include <mpconfigport.h>

// Any options not explicitly set in mpconfigport.h will get default
// values below.

/*****************************************************************************/
/* Memory allocation policy                                                  */

// Initial amount for lexer indentation level
#ifndef MICROPY_ALLOC_LEXER_INDENT_INIT
#define MICROPY_ALLOC_LEXER_INDENT_INIT (10)
#endif

// Increment for lexer indentation level
#ifndef MICROPY_ALLOC_LEXEL_INDENT_INC
#define MICROPY_ALLOC_LEXEL_INDENT_INC (8)
#endif

// Initial amount for parse rule stack
#ifndef MICROPY_ALLOC_PARSE_RULE_INIT
#define MICROPY_ALLOC_PARSE_RULE_INIT (64)
#endif

// Increment for parse rule stack
#ifndef MICROPY_ALLOC_PARSE_RULE_INC
#define MICROPY_ALLOC_PARSE_RULE_INC (16)
#endif

// Initial amount for parse result stack
#ifndef MICROPY_ALLOC_PARSE_RESULT_INIT
#define MICROPY_ALLOC_PARSE_RESULT_INIT (32)
#endif

// Increment for parse result stack
#ifndef MICROPY_ALLOC_PARSE_RESULT_INC
#define MICROPY_ALLOC_PARSE_RESULT_INC (16)
#endif

// Strings this length or less will be interned by the parser
#ifndef MICROPY_ALLOC_PARSE_INTERN_STRING_LEN
#define MICROPY_ALLOC_PARSE_INTERN_STRING_LEN (10)
#endif

// Initial amount for ids in a scope
#ifndef MICROPY_ALLOC_SCOPE_ID_INIT
#define MICROPY_ALLOC_SCOPE_ID_INIT (4)
#endif

// Increment for ids in a scope
#ifndef MICROPY_ALLOC_SCOPE_ID_INC
#define MICROPY_ALLOC_SCOPE_ID_INC (6)
#endif

// Maximum length of a path in the filesystem
// So we can allocate a buffer on the stack for path manipulation in import
#ifndef MICROPY_ALLOC_PATH_MAX
#define MICROPY_ALLOC_PATH_MAX (512)
#endif

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

// Whether to emit x86 native code
#ifndef MICROPY_EMIT_X86
#define MICROPY_EMIT_X86 (0)
#endif

// Whether to emit thumb native code
#ifndef MICROPY_EMIT_THUMB
#define MICROPY_EMIT_THUMB (0)
#endif

// Whether to enable the thumb inline assembler
#ifndef MICROPY_EMIT_INLINE_THUMB
#define MICROPY_EMIT_INLINE_THUMB (0)
#endif

// Whether to emit ARM native code
#ifndef MICROPY_EMIT_ARM
#define MICROPY_EMIT_ARM (0)
#endif

// Convenience definition for whether any native emitter is enabled
#define MICROPY_EMIT_NATIVE (MICROPY_EMIT_X64 || MICROPY_EMIT_X86 || MICROPY_EMIT_THUMB || MICROPY_EMIT_ARM)

/*****************************************************************************/
/* Compiler configuration                                                    */

// Whether to enable constant optimisation; id = const(value)
#ifndef MICROPY_COMP_CONST
#define MICROPY_COMP_CONST (1)
#endif

/*****************************************************************************/
/* Internal debugging stuff                                                  */

// Whether to collect memory allocation stats
#ifndef MICROPY_MEM_STATS
#define MICROPY_MEM_STATS (0)
#endif

// Whether to build functions that print debugging info:
//   mp_token_show
//   mp_bytecode_print
//   mp_parse_node_print
#ifndef MICROPY_DEBUG_PRINTERS
#define MICROPY_DEBUG_PRINTERS (0)
#endif

/*****************************************************************************/
/* Optimisations                                                             */

// Whether to use computed gotos in the VM, or a switch
// Computed gotos are roughly 10% faster, and increase VM code size by a little
#ifndef MICROPY_OPT_COMPUTED_GOTO
#define MICROPY_OPT_COMPUTED_GOTO (0)
#endif

/*****************************************************************************/
/* Python internal features                                                  */

// Whether to include the garbage collector
#ifndef MICROPY_ENABLE_GC
#define MICROPY_ENABLE_GC (0)
#endif

// Whether to enable finalisers in the garbage collector (ie call __del__)
#ifndef MICROPY_ENABLE_GC_FINALISER
#define MICROPY_ENABLE_GC_FINALISER (0)
#endif

// Whether to check C stack usage. C stack used for calling Python functions,
// etc. Not checking means segfault on overflow.
#ifndef MICROPY_STACK_CHECK
#define MICROPY_STACK_CHECK (1)
#endif

// Whether to have an emergency exception buffer
#ifndef MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (0)
#endif
#if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF
#   ifndef MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE
#   define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE (0)   // 0 - implies dynamic allocation
#   endif
#endif

// Whether to include REPL helper function
#ifndef MICROPY_HELPER_REPL
#define MICROPY_HELPER_REPL (0)
#endif

// Whether to include lexer helper function for unix
#ifndef MICROPY_HELPER_LEXER_UNIX
#define MICROPY_HELPER_LEXER_UNIX (0)
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

// Exception messages are short static strings (TODO)
#define MICROPY_ERROR_REPORTING_TERSE    (1)
// Exception messages provide basic error details
#define MICROPY_ERROR_REPORTING_NORMAL   (2)
// Exception messages provide full info, e.g. object names
#define MICROPY_ERROR_REPORTING_DETAILED (3)

#ifndef MICROPY_ERROR_REPORTING
#define MICROPY_ERROR_REPORTING (MICROPY_ERROR_REPORTING_NORMAL)
#endif

// Float and complex implementation
#define MICROPY_FLOAT_IMPL_NONE (0)
#define MICROPY_FLOAT_IMPL_FLOAT (1)
#define MICROPY_FLOAT_IMPL_DOUBLE (2)

#ifndef MICROPY_FLOAT_IMPL
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_NONE)
#endif

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define MICROPY_PY_BUILTINS_FLOAT (1)
#define MICROPY_FLOAT_C_FUN(fun) fun##f
typedef float mp_float_t;
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define MICROPY_PY_BUILTINS_FLOAT (1)
#define MICROPY_FLOAT_C_FUN(fun) fun
typedef double mp_float_t;
#else
#define MICROPY_PY_BUILTINS_FLOAT (0)
#endif

#ifndef MICROPY_PY_BUILTINS_COMPLEX
#define MICROPY_PY_BUILTINS_COMPLEX (MICROPY_PY_BUILTINS_FLOAT)
#endif

// Enable features which improve CPython compatibility
// but may lead to more code size/memory usage.
// TODO: Originally intended as generic category to not
// add bunch of once-off options. May need refactoring later
#ifndef MICROPY_CPYTHON_COMPAT
#define MICROPY_CPYTHON_COMPAT (1)
#endif

// Whether POSIX-semantics non-blocking streams are supported
#ifndef MICROPY_STREAMS_NON_BLOCK
#define MICROPY_STREAMS_NON_BLOCK (0)
#endif

/*****************************************************************************/
/* Fine control over Python builtins, classes, modules, etc                  */

// Whether str object is proper unicode
#ifndef MICROPY_PY_BUILTINS_STR_UNICODE
#define MICROPY_PY_BUILTINS_STR_UNICODE (0)
#endif

// Whether to support bytearray object
#ifndef MICROPY_PY_BUILTINS_BYTEARRAY
#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#endif

// Whether to support set object
#ifndef MICROPY_PY_BUILTINS_SET
#define MICROPY_PY_BUILTINS_SET (1)
#endif

// Whether to support slice subscript operators and slice object
#ifndef MICROPY_PY_BUILTINS_SLICE
#define MICROPY_PY_BUILTINS_SLICE (1)
#endif

// Whether to support frozenset object
#ifndef MICROPY_PY_BUILTINS_FROZENSET
#define MICROPY_PY_BUILTINS_FROZENSET (0)
#endif

// Whether to support property object
#ifndef MICROPY_PY_BUILTINS_PROPERTY
#define MICROPY_PY_BUILTINS_PROPERTY (1)
#endif

// Whether to set __file__ for imported modules
#ifndef MICROPY_PY___FILE__
#define MICROPY_PY___FILE__ (1)
#endif

// Whether to provide "array" module. Note that large chunk of the
// underlying code is shared with "bytearray" builtin type, so to
// get real savings, it should be disabled too.
#ifndef MICROPY_PY_ARRAY
#define MICROPY_PY_ARRAY (1)
#endif

// Whether to provide "collections" module
#ifndef MICROPY_PY_COLLECTIONS
#define MICROPY_PY_COLLECTIONS (1)
#endif

// Whether to provide "math" module
#ifndef MICROPY_PY_MATH
#define MICROPY_PY_MATH (1)
#endif

// Whether to provide "cmath" module
#ifndef MICROPY_PY_CMATH
#define MICROPY_PY_CMATH (0)
#endif

// Whether to provide "gc" module
#ifndef MICROPY_PY_GC
#define MICROPY_PY_GC (1)
#endif

// Whether to return number of collected objects from gc.collect()
#ifndef MICROPY_PY_GC_COLLECT_RETVAL
#define MICROPY_PY_GC_COLLECT_RETVAL (0)
#endif

// Whether to provide "io" module
#ifndef MICROPY_PY_IO
#define MICROPY_PY_IO (1)
#endif

// Whether to provide "io.FileIO" class
#ifndef MICROPY_PY_IO_FILEIO
#define MICROPY_PY_IO_FILEIO (0)
#endif

// Whether to provide "io.BytesIO" class
#ifndef MICROPY_PY_IO_BYTESIO
#define MICROPY_PY_IO_BYTESIO (1)
#endif

// Whether to provide "struct" module
#ifndef MICROPY_PY_STRUCT
#define MICROPY_PY_STRUCT (1)
#endif

// Whether to provide "sys" module
#ifndef MICROPY_PY_SYS
#define MICROPY_PY_SYS (1)
#endif

// Whether to provide "sys.maxsize" constant
#ifndef MICROPY_PY_SYS_MAXSIZE
#define MICROPY_PY_SYS_MAXSIZE (0)
#endif

// Whether to provide "sys.exit" function
#ifndef MICROPY_PY_SYS_EXIT
#define MICROPY_PY_SYS_EXIT (0)
#endif

// Whether to provide sys.{stdin,stdout,stderr} objects
#ifndef MICROPY_PY_SYS_STDFILES
#define MICROPY_PY_SYS_STDFILES (0)
#endif


// Extended modules

#ifndef MICROPY_PY_UCTYPES
#define MICROPY_PY_UCTYPES (0)
#endif

#ifndef MICROPY_PY_ZLIBD
#define MICROPY_PY_ZLIBD (0)
#endif

/*****************************************************************************/
/* Hooks for a port to add builtins                                          */

// Additional builtin function definitions - see builtintables.c:builtin_object_table for format.
#ifndef MICROPY_PORT_BUILTINS
#define MICROPY_PORT_BUILTINS
#endif

// Additional builtin module definitions - see builtintables.c:builtin_module_table for format.
#ifndef MICROPY_PORT_BUILTIN_MODULES
#define MICROPY_PORT_BUILTIN_MODULES
#endif

// Additional constant definitions for the compiler - see compile.c:mp_constants_table.
#ifndef MICROPY_PORT_CONSTANTS
#define MICROPY_PORT_CONSTANTS
#endif

/*****************************************************************************/
/* Miscellaneous settings                                                    */

// On embedded platforms, these will typically enable/disable irqs.
#ifndef MICROPY_BEGIN_ATOMIC_SECTION
#define MICROPY_BEGIN_ATOMIC_SECTION()
#endif
#ifndef MICROPY_END_ATOMIC_SECTION
#define MICROPY_END_ATOMIC_SECTION()
#endif

// Allow to override static modifier for global objects, e.g. to use with
// object code analysis tools which don't support static symbols.
#ifndef STATIC
#define STATIC static
#endif

#define BITS_PER_BYTE (8)
#define BITS_PER_WORD (BITS_PER_BYTE * BYTES_PER_WORD)
// mp_int_t value with most significant bit set
#define WORD_MSBIT_HIGH (((mp_uint_t)1) << (BYTES_PER_WORD * 8 - 1))

#if !defined(MP_ENDIANNESS_LITTLE) && !defined(MP_ENDIANNESS_BIG)
// Just because most archs are such?
#define MP_ENDIANNESS_LITTLE (1)
#endif
// Ensure we don't accidentally set both endiannesses
#if MP_ENDIANNESS_BIG
#define MP_ENDIANNESS_LITTLE (0)
#endif

// Make a pointer to RAM callable (eg set lower bit for Thumb code)
// (This scheme won't work if we want to mix Thumb and normal ARM code.)
#ifndef MICROPY_MAKE_POINTER_CALLABLE
#define MICROPY_MAKE_POINTER_CALLABLE(p) (p)
#endif

#ifndef MP_PLAT_ALLOC_EXEC
#define MP_PLAT_ALLOC_EXEC(min_size, ptr, size) do { *ptr = m_new(byte, min_size); *size = min_size; } while(0)
#endif

#ifndef MP_PLAT_FREE_EXEC
#define MP_PLAT_FREE_EXEC(ptr, size) m_del(byte, ptr, size)
#endif

#ifndef MP_SSIZE_MAX
#define MP_SSIZE_MAX SSIZE_MAX
#endif

// printf format spec to use for mp_int_t and friends
#ifndef INT_FMT
#ifdef __LP64__
// Archs where mp_int_t == long, long != int
#define UINT_FMT "%lu"
#define INT_FMT "%ld"
#else
// Archs where mp_int_t == int
#define UINT_FMT "%u"
#define INT_FMT "%d"
#endif
#endif //INT_FMT

// Modifier for function which doesn't return
#ifndef NORETURN
#define NORETURN __attribute__((noreturn))
#endif

// Modifier for weak functions
#ifndef MP_WEAK
#define MP_WEAK __attribute__((weak))
#endif
