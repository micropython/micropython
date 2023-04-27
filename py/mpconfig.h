/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_MPCONFIG_H
#define MICROPY_INCLUDED_PY_MPCONFIG_H

// Current version of MicroPython
#define MICROPY_VERSION_MAJOR 1
#define MICROPY_VERSION_MINOR 20
#define MICROPY_VERSION_MICRO 0

// Combined version as a 32-bit number for convenience
#define MICROPY_VERSION ( \
    MICROPY_VERSION_MAJOR << 16 \
        | MICROPY_VERSION_MINOR << 8 \
        | MICROPY_VERSION_MICRO)

// String version
#define MICROPY_VERSION_STRING \
    MP_STRINGIFY(MICROPY_VERSION_MAJOR) "." \
    MP_STRINGIFY(MICROPY_VERSION_MINOR) "." \
    MP_STRINGIFY(MICROPY_VERSION_MICRO)

// This file contains default configuration settings for MicroPython.
// You can override any of the options below using mpconfigport.h file
// located in a directory of your port.

// mpconfigport.h is a file containing configuration settings for a
// particular port. mpconfigport.h is actually a default name for
// such config, and it can be overridden using MP_CONFIGFILE preprocessor
// define (you can do that by passing CFLAGS_EXTRA='-DMP_CONFIGFILE="<file.h>"'
// argument to make when using standard MicroPython makefiles).
// This is useful to have more than one config per port, for example,
// release vs debug configs, etc. Note that if you switch from one config
// to another, you must rebuild from scratch using "-B" switch to make.

// Disable all optional features (i.e. minimal port).
#define MICROPY_CONFIG_ROM_LEVEL_MINIMUM (0)
// Only enable core features (constrained flash, e.g. STM32L072)
#define MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES (10)
// Enable most common features (small on-device flash, e.g. STM32F411)
#define MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES (20)
// Enable convenience features (medium on-device flash, e.g. STM32F405)
#define MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES (30)
// Enable all common features (large/external flash, rp2, unix)
#define MICROPY_CONFIG_ROM_LEVEL_FULL_FEATURES (40)
// Enable everything (e.g. coverage)
#define MICROPY_CONFIG_ROM_LEVEL_EVERYTHING (50)

#ifdef MP_CONFIGFILE
#include MP_CONFIGFILE
#else
#include <mpconfigport.h>
#endif

// Ports/boards should set this, but default to level=core.
#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES)
#endif

// Helper macros for "have at least this level".
#define MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES (MICROPY_CONFIG_ROM_LEVEL >= MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES)
#define MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_BASIC_FEATURES (MICROPY_CONFIG_ROM_LEVEL >= MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES)
#define MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES (MICROPY_CONFIG_ROM_LEVEL >= MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
#define MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_FULL_FEATURES (MICROPY_CONFIG_ROM_LEVEL >= MICROPY_CONFIG_ROM_LEVEL_FULL_FEATURES)
#define MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING (MICROPY_CONFIG_ROM_LEVEL >= MICROPY_CONFIG_ROM_LEVEL_EVERYTHING)

// Any options not explicitly set in mpconfigport.h will get default
// values below.

/*****************************************************************************/
/* Object representation                                                     */

// A MicroPython object is a machine word having the following form:
//  - xxxx...xxx1 : a small int, bits 1 and above are the value
//  - xxxx...x010 : a qstr, bits 3 and above are the value
//  - xxxx...x110 : an immediate object, bits 3 and above are the value
//  - xxxx...xx00 : a pointer to an mp_obj_base_t (unless a fake object)
#define MICROPY_OBJ_REPR_A (0)

// A MicroPython object is a machine word having the following form:
//  - xxxx...xx01 : a small int, bits 2 and above are the value
//  - xxxx...x011 : a qstr, bits 3 and above are the value
//  - xxxx...x111 : an immediate object, bits 3 and above are the value
//  - xxxx...xxx0 : a pointer to an mp_obj_base_t (unless a fake object)
#define MICROPY_OBJ_REPR_B (1)

// A MicroPython object is a machine word having the following form (called R):
//  - iiiiiiii iiiiiiii iiiiiiii iiiiiii1 small int with 31-bit signed value
//  - 01111111 1qqqqqqq qqqqqqqq qqqq0110 str with 19-bit qstr value
//  - 01111111 10000000 00000000 ssss1110 immediate object with 4-bit value
//  - s1111111 10000000 00000000 00000010 +/- inf
//  - s1111111 1xxxxxxx xxxxxxxx xxxxx010 nan, x != 0
//  - seeeeeee efffffff ffffffff ffffff10 30-bit fp, e != 0xff
//  - pppppppp pppppppp pppppppp pppppp00 ptr (4 byte alignment)
// Str, immediate and float stored as O = R + 0x80800000, retrieved as R = O - 0x80800000.
// This makes strs/immediates easier to encode/decode as they have zeros in the top 9 bits.
// This scheme only works with 32-bit word size and float enabled.
#define MICROPY_OBJ_REPR_C (2)

// A MicroPython object is a 64-bit word having the following form (called R):
//  - seeeeeee eeeeffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff 64-bit fp, e != 0x7ff
//  - s1111111 11110000 00000000 00000000 00000000 00000000 00000000 00000000 +/- inf
//  - 01111111 11111000 00000000 00000000 00000000 00000000 00000000 00000000 normalised nan
//  - 01111111 11111101 iiiiiiii iiiiiiii iiiiiiii iiiiiiii iiiiiiii iiiiiii1 small int
//  - 01111111 11111110 00000000 00000000 qqqqqqqq qqqqqqqq qqqqqqqq qqqqqqq1 str
//  - 01111111 11111111 ss000000 00000000 00000000 00000000 00000000 00000000 immediate object
//  - 01111111 11111100 00000000 00000000 pppppppp pppppppp pppppppp pppppp00 ptr (4 byte alignment)
// Stored as O = R + 0x8004000000000000, retrieved as R = O - 0x8004000000000000.
// This makes pointers have all zeros in the top 32 bits.
// Small-ints and strs have 1 as LSB to make sure they don't look like pointers
// to the garbage collector.
#define MICROPY_OBJ_REPR_D (3)

#ifndef MICROPY_OBJ_REPR
#define MICROPY_OBJ_REPR (MICROPY_OBJ_REPR_A)
#endif

// Whether to encode None/False/True as immediate objects instead of pointers to
// real objects.  Reduces code size by a decent amount without hurting
// performance, for all representations except D on some architectures.
#ifndef MICROPY_OBJ_IMMEDIATE_OBJS
#define MICROPY_OBJ_IMMEDIATE_OBJS (MICROPY_OBJ_REPR != MICROPY_OBJ_REPR_D)
#endif

/*****************************************************************************/
/* Memory allocation policy                                                  */

// Number of bytes in memory allocation/GC block. Any size allocated will be
// rounded up to be multiples of this.
#ifndef MICROPY_BYTES_PER_GC_BLOCK
#define MICROPY_BYTES_PER_GC_BLOCK (4 * MP_BYTES_PER_OBJ_WORD)
#endif

// Number of words allocated (in BSS) to the GC stack (minimum is 1)
#ifndef MICROPY_ALLOC_GC_STACK_SIZE
#define MICROPY_ALLOC_GC_STACK_SIZE (64)
#endif

// The C-type to use for entries in the GC stack.  By default it allows the
// heap to be as large as the address space, but the bit-width of this type can
// be reduced to save memory when the heap is small enough.  The type must be
// big enough to index all blocks in the heap, which is set by
// heap-size-in-bytes / MICROPY_BYTES_PER_GC_BLOCK.
#ifndef MICROPY_GC_STACK_ENTRY_TYPE
#define MICROPY_GC_STACK_ENTRY_TYPE size_t
#endif

// Be conservative and always clear to zero newly (re)allocated memory in the GC.
// This helps eliminate stray pointers that hold on to memory that's no longer
// used.  It decreases performance due to unnecessary memory clearing.
// A memory manager which always clears memory can set this to 0.
// TODO Do analysis to understand why some memory is not properly cleared and
// find a more efficient way to clear it.
#ifndef MICROPY_GC_CONSERVATIVE_CLEAR
#define MICROPY_GC_CONSERVATIVE_CLEAR (MICROPY_ENABLE_GC)
#endif

// Support automatic GC when reaching allocation threshold,
// configurable by gc.threshold().
#ifndef MICROPY_GC_ALLOC_THRESHOLD
#define MICROPY_GC_ALLOC_THRESHOLD (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Number of bytes to allocate initially when creating new chunks to store
// interned string data.  Smaller numbers lead to more chunks being needed
// and more wastage at the end of the chunk.  Larger numbers lead to wasted
// space at the end when no more strings need interning.
#ifndef MICROPY_ALLOC_QSTR_CHUNK_INIT
#define MICROPY_ALLOC_QSTR_CHUNK_INIT (128)
#endif

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

// Number of bytes to allocate initially when creating new chunks to store
// parse nodes.  Small leads to fragmentation, large leads to excess use.
#ifndef MICROPY_ALLOC_PARSE_CHUNK_INIT
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (128)
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

// Initial size of module dict
#ifndef MICROPY_MODULE_DICT_SIZE
#define MICROPY_MODULE_DICT_SIZE (1)
#endif

// Initial size of sys.modules dict
#ifndef MICROPY_LOADED_MODULES_DICT_SIZE
#define MICROPY_LOADED_MODULES_DICT_SIZE (3)
#endif

// Whether realloc/free should be passed allocated memory region size
// You must enable this if MICROPY_MEM_STATS is enabled
#ifndef MICROPY_MALLOC_USES_ALLOCATED_SIZE
#define MICROPY_MALLOC_USES_ALLOCATED_SIZE (0)
#endif

// Number of bytes used to store qstr length
// Dictates hard limit on maximum Python identifier length, but 1 byte
// (limit of 255 bytes in an identifier) should be enough for everyone
#ifndef MICROPY_QSTR_BYTES_IN_LEN
#define MICROPY_QSTR_BYTES_IN_LEN (1)
#endif

// Number of bytes used to store qstr hash
#ifndef MICROPY_QSTR_BYTES_IN_HASH
#if MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES
#define MICROPY_QSTR_BYTES_IN_HASH (2)
#else
#define MICROPY_QSTR_BYTES_IN_HASH (1)
#endif
#endif

// Avoid using C stack when making Python function calls. C stack still
// may be used if there's no free heap.
#ifndef MICROPY_STACKLESS
#define MICROPY_STACKLESS (0)
#endif

// Never use C stack when making Python function calls. This may break
// testsuite as will subtly change which exception is thrown in case
// of too deep recursion and other similar cases.
#ifndef MICROPY_STACKLESS_STRICT
#define MICROPY_STACKLESS_STRICT (0)
#endif

// Don't use alloca calls. As alloca() is not part of ANSI C, this
// workaround option is provided for compilers lacking this de-facto
// standard function. The way it works is allocating from heap, and
// relying on garbage collection to free it eventually. This is of
// course much less optimal than real alloca().
#if defined(MICROPY_NO_ALLOCA) && MICROPY_NO_ALLOCA
#undef alloca
#define alloca(x) m_malloc(x)
#endif

/*****************************************************************************/
/* MicroPython emitters                                                     */

// Whether to support loading of persistent code
#ifndef MICROPY_PERSISTENT_CODE_LOAD
#define MICROPY_PERSISTENT_CODE_LOAD (0)
#endif

// Whether to support saving of persistent code
#ifndef MICROPY_PERSISTENT_CODE_SAVE
#define MICROPY_PERSISTENT_CODE_SAVE (0)
#endif

// Whether to support saving persistent code to a file via mp_raw_code_save_file
#ifndef MICROPY_PERSISTENT_CODE_SAVE_FILE
#define MICROPY_PERSISTENT_CODE_SAVE_FILE (0)
#endif

// Whether generated code can persist independently of the VM/runtime instance
// This is enabled automatically when needed by other features
#ifndef MICROPY_PERSISTENT_CODE
#define MICROPY_PERSISTENT_CODE (MICROPY_PERSISTENT_CODE_LOAD || MICROPY_PERSISTENT_CODE_SAVE || MICROPY_MODULE_FROZEN_MPY)
#endif

// Whether bytecode uses a qstr_table to map internal qstr indices in the bytecode
// to global qstr values in the runtime (behaviour when feature is enabled), or
// just stores global qstr values directly in the bytecode.  This must be enabled
// if MICROPY_PERSISTENT_CODE is enabled.
#ifndef MICROPY_EMIT_BYTECODE_USES_QSTR_TABLE
#define MICROPY_EMIT_BYTECODE_USES_QSTR_TABLE (MICROPY_PERSISTENT_CODE)
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

// Whether to emit ARMv7-M instruction support in thumb native code
#ifndef MICROPY_EMIT_THUMB_ARMV7M
#define MICROPY_EMIT_THUMB_ARMV7M (1)
#endif

// Whether to enable the thumb inline assembler
#ifndef MICROPY_EMIT_INLINE_THUMB
#define MICROPY_EMIT_INLINE_THUMB (0)
#endif

// Whether to enable float support in the Thumb2 inline assembler
#ifndef MICROPY_EMIT_INLINE_THUMB_FLOAT
#define MICROPY_EMIT_INLINE_THUMB_FLOAT (1)
#endif

// Whether to emit ARM native code
#ifndef MICROPY_EMIT_ARM
#define MICROPY_EMIT_ARM (0)
#endif

// Whether to emit Xtensa native code
#ifndef MICROPY_EMIT_XTENSA
#define MICROPY_EMIT_XTENSA (0)
#endif

// Whether to enable the Xtensa inline assembler
#ifndef MICROPY_EMIT_INLINE_XTENSA
#define MICROPY_EMIT_INLINE_XTENSA (0)
#endif

// Whether to emit Xtensa-Windowed native code
#ifndef MICROPY_EMIT_XTENSAWIN
#define MICROPY_EMIT_XTENSAWIN (0)
#endif

// Convenience definition for whether any native emitter is enabled
#define MICROPY_EMIT_NATIVE (MICROPY_EMIT_X64 || MICROPY_EMIT_X86 || MICROPY_EMIT_THUMB || MICROPY_EMIT_ARM || MICROPY_EMIT_XTENSA || MICROPY_EMIT_XTENSAWIN)

// Some architectures cannot read byte-wise from executable memory.  In this case
// the prelude for a native function (which usually sits after the machine code)
// must be separated and placed somewhere where it can be read byte-wise.
#define MICROPY_EMIT_NATIVE_PRELUDE_SEPARATE_FROM_MACHINE_CODE (MICROPY_EMIT_XTENSAWIN)

// Convenience definition for whether any inline assembler emitter is enabled
#define MICROPY_EMIT_INLINE_ASM (MICROPY_EMIT_INLINE_THUMB || MICROPY_EMIT_INLINE_XTENSA)

// Convenience definition for whether any native or inline assembler emitter is enabled
#define MICROPY_EMIT_MACHINE_CODE (MICROPY_EMIT_NATIVE || MICROPY_EMIT_INLINE_ASM)

// Whether native relocatable code loaded from .mpy files is explicitly tracked
// so that the GC cannot reclaim it.  Needed on architectures that allocate
// executable memory on the MicroPython heap and don't explicitly track this
// data some other way.
#ifndef MICROPY_PERSISTENT_CODE_TRACK_RELOC_CODE
#if !MICROPY_EMIT_MACHINE_CODE || defined(MP_PLAT_ALLOC_EXEC) || defined(MP_PLAT_COMMIT_EXEC)
#define MICROPY_PERSISTENT_CODE_TRACK_RELOC_CODE (0)
#else
#define MICROPY_PERSISTENT_CODE_TRACK_RELOC_CODE (1)
#endif
#endif

/*****************************************************************************/
/* Compiler configuration                                                    */

// Whether to include the compiler
#ifndef MICROPY_ENABLE_COMPILER
#define MICROPY_ENABLE_COMPILER (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether the compiler is dynamically configurable (ie at runtime)
// This will disable the ability to execute native/viper code
#ifndef MICROPY_DYNAMIC_COMPILER
#define MICROPY_DYNAMIC_COMPILER (0)
#endif

// Whether to enable constant folding; eg 1+2 rewritten as 3
#ifndef MICROPY_COMP_CONST_FOLDING
#define MICROPY_COMP_CONST_FOLDING (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to compile constant tuples immediately to their respective objects; eg (1, True)
// Otherwise the tuple will be built at runtime
#ifndef MICROPY_COMP_CONST_TUPLE
#define MICROPY_COMP_CONST_TUPLE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to enable optimisations for constant literals, eg OrderedDict
#ifndef MICROPY_COMP_CONST_LITERAL
#define MICROPY_COMP_CONST_LITERAL (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to enable lookup of constants in modules; eg module.CONST
#ifndef MICROPY_COMP_MODULE_CONST
#define MICROPY_COMP_MODULE_CONST (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to enable constant optimisation; id = const(value)
#ifndef MICROPY_COMP_CONST
#define MICROPY_COMP_CONST (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to enable optimisation of: a, b = c, d
// Costs 124 bytes (Thumb2)
#ifndef MICROPY_COMP_DOUBLE_TUPLE_ASSIGN
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to enable optimisation of: a, b, c = d, e, f
// Requires MICROPY_COMP_DOUBLE_TUPLE_ASSIGN and costs 68 bytes (Thumb2)
#ifndef MICROPY_COMP_TRIPLE_TUPLE_ASSIGN
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to enable optimisation of: return a if b else c
// Costs about 80 bytes (Thumb2) and saves 2 bytes of bytecode for each use
#ifndef MICROPY_COMP_RETURN_IF_EXPR
#define MICROPY_COMP_RETURN_IF_EXPR (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

/*****************************************************************************/
/* Internal debugging stuff                                                  */

// Whether to collect memory allocation stats
#ifndef MICROPY_MEM_STATS
#define MICROPY_MEM_STATS (0)
#endif

// The mp_print_t printer used for debugging output
#ifndef MICROPY_DEBUG_PRINTER
#define MICROPY_DEBUG_PRINTER (&mp_plat_print)
#endif

// Whether to build functions that print debugging info:
//   mp_bytecode_print
//   mp_parse_node_print
#ifndef MICROPY_DEBUG_PRINTERS
#define MICROPY_DEBUG_PRINTERS (0)
#endif

// Whether to enable all debugging outputs (it will be extremely verbose)
#ifndef MICROPY_DEBUG_VERBOSE
#define MICROPY_DEBUG_VERBOSE (0)
#endif

// Whether to enable debugging versions of MP_OBJ_NULL/STOP_ITERATION/SENTINEL
#ifndef MICROPY_DEBUG_MP_OBJ_SENTINELS
#define MICROPY_DEBUG_MP_OBJ_SENTINELS (0)
#endif

// Whether to print parse rule names (rather than integers) in mp_parse_node_print
#ifndef MICROPY_DEBUG_PARSE_RULE_NAME
#define MICROPY_DEBUG_PARSE_RULE_NAME (0)
#endif

// Whether to enable a simple VM stack overflow check
#ifndef MICROPY_DEBUG_VM_STACK_OVERFLOW
#define MICROPY_DEBUG_VM_STACK_OVERFLOW (0)
#endif

// Whether to enable extra instrumentation for valgrind
#ifndef MICROPY_DEBUG_VALGRIND
#define MICROPY_DEBUG_VALGRIND (0)
#endif

/*****************************************************************************/
/* Optimisations                                                             */

// Whether to use computed gotos in the VM, or a switch
// Computed gotos are roughly 10% faster, and increase VM code size by a little,
// e.g. ~1kiB on Cortex M4.
// Note: enabling this will use the gcc-specific extensions of ranged designated
// initialisers and addresses of labels, which are not part of the C99 standard.
#ifndef MICROPY_OPT_COMPUTED_GOTO
#define MICROPY_OPT_COMPUTED_GOTO (0)
#endif

// Optimise the fast path for loading attributes from instance types. Increases
// Thumb2 code size by about 48 bytes.
#ifndef MICROPY_OPT_LOAD_ATTR_FAST_PATH
#define MICROPY_OPT_LOAD_ATTR_FAST_PATH (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Use extra RAM to cache map lookups by remembering the likely location of
// the index. Avoids the hash computation on unordered maps, and avoids the
// linear search on ordered (especially in-ROM) maps. Can provide a +10-15%
// performance improvement on benchmarks involving lots of attribute access
// or dictionary lookup.
#ifndef MICROPY_OPT_MAP_LOOKUP_CACHE
#define MICROPY_OPT_MAP_LOOKUP_CACHE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// How much RAM (in bytes) to use for the map lookup cache.
#ifndef MICROPY_OPT_MAP_LOOKUP_CACHE_SIZE
#define MICROPY_OPT_MAP_LOOKUP_CACHE_SIZE (128)
#endif

// Whether to use fast versions of bitwise operations (and, or, xor) when the
// arguments are both positive.  Increases Thumb2 code size by about 250 bytes.
#ifndef MICROPY_OPT_MPZ_BITWISE
#define MICROPY_OPT_MPZ_BITWISE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif


// Whether math.factorial is large, fast and recursive (1) or small and slow (0).
#ifndef MICROPY_OPT_MATH_FACTORIAL
#define MICROPY_OPT_MATH_FACTORIAL (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

/*****************************************************************************/
/* Python internal features                                                  */

// Whether to enable import of external modules
// When disabled, only importing of built-in modules is supported
// When enabled, a port must implement mp_import_stat (among other things)
#ifndef MICROPY_ENABLE_EXTERNAL_IMPORT
#define MICROPY_ENABLE_EXTERNAL_IMPORT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to use the POSIX reader for importing files
#ifndef MICROPY_READER_POSIX
#define MICROPY_READER_POSIX (0)
#endif

// Whether to use the VFS reader for importing files
#ifndef MICROPY_READER_VFS
#define MICROPY_READER_VFS (0)
#endif

// Whether any readers have been defined
#ifndef MICROPY_HAS_FILE_READER
#define MICROPY_HAS_FILE_READER (MICROPY_READER_POSIX || MICROPY_READER_VFS)
#endif

// Hook for the VM at the start of the opcode loop (can contain variable
// definitions usable by the other hook functions)
#ifndef MICROPY_VM_HOOK_INIT
#define MICROPY_VM_HOOK_INIT
#endif

// Hook for the VM during the opcode loop (but only after jump opcodes)
#ifndef MICROPY_VM_HOOK_LOOP
#define MICROPY_VM_HOOK_LOOP
#endif

// Hook for the VM just before return opcode is finished being interpreted
#ifndef MICROPY_VM_HOOK_RETURN
#define MICROPY_VM_HOOK_RETURN
#endif

// Hook for mp_sched_schedule when a function gets scheduled on sched_queue
// (this macro executes within an atomic section)
#ifndef MICROPY_SCHED_HOOK_SCHEDULED
#define MICROPY_SCHED_HOOK_SCHEDULED
#endif

// Whether to include the garbage collector
#ifndef MICROPY_ENABLE_GC
#define MICROPY_ENABLE_GC (0)
#endif

// Whether the garbage-collected heap can be split over multiple memory areas.
#ifndef MICROPY_GC_SPLIT_HEAP
#define MICROPY_GC_SPLIT_HEAP (0)
#endif

// Hook to run code during time consuming garbage collector operations
#ifndef MICROPY_GC_HOOK_LOOP
#define MICROPY_GC_HOOK_LOOP
#endif

// Whether to provide m_tracked_calloc, m_tracked_free functions
#ifndef MICROPY_TRACKED_ALLOC
#define MICROPY_TRACKED_ALLOC (0)
#endif

// Whether to enable finalisers in the garbage collector (ie call __del__)
#ifndef MICROPY_ENABLE_FINALISER
#define MICROPY_ENABLE_FINALISER (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to enable a separate allocator for the Python stack.
// If enabled then the code must call mp_pystack_init before mp_init.
#ifndef MICROPY_ENABLE_PYSTACK
#define MICROPY_ENABLE_PYSTACK (0)
#endif

// Number of bytes that memory returned by mp_pystack_alloc will be aligned by.
#ifndef MICROPY_PYSTACK_ALIGN
#define MICROPY_PYSTACK_ALIGN (8)
#endif

// Whether to check C stack usage. C stack used for calling Python functions,
// etc. Not checking means segfault on overflow.
#ifndef MICROPY_STACK_CHECK
#define MICROPY_STACK_CHECK (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to have an emergency exception buffer
#ifndef MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (0)
#endif
#if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF
#ifndef MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE (0)      // 0 - implies dynamic allocation
#endif
#endif

// Whether to provide the mp_kbd_exception object, and micropython.kbd_intr function
#ifndef MICROPY_KBD_EXCEPTION
#define MICROPY_KBD_EXCEPTION (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Prefer to raise KeyboardInterrupt asynchronously (from signal or interrupt
// handler) - if supported by a particular port.
#ifndef MICROPY_ASYNC_KBD_INTR
#define MICROPY_ASYNC_KBD_INTR (0)
#endif

// Whether to include REPL helper function
#ifndef MICROPY_HELPER_REPL
#define MICROPY_HELPER_REPL (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Allow enabling debug prints after each REPL line
#ifndef MICROPY_REPL_INFO
#define MICROPY_REPL_INFO (0)
#endif

// Whether to include emacs-style readline behavior in REPL
#ifndef MICROPY_REPL_EMACS_KEYS
#define MICROPY_REPL_EMACS_KEYS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to include emacs-style word movement/kill readline behavior in REPL.
// This adds Alt+F, Alt+B, Alt+D and Alt+Backspace for forward-word, backward-word, forward-kill-word
// and backward-kill-word, respectively.
#ifndef MICROPY_REPL_EMACS_WORDS_MOVE
#define MICROPY_REPL_EMACS_WORDS_MOVE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether to include extra convenience keys for word movement/kill in readline REPL.
// This adds Ctrl+Right, Ctrl+Left and Ctrl+W for forward-word, backward-word and backward-kill-word
// respectively. Ctrl+Delete is not implemented because it's a very different escape sequence.
// Depends on MICROPY_REPL_EMACS_WORDS_MOVE.
#ifndef MICROPY_REPL_EMACS_EXTRA_WORDS_MOVE
#define MICROPY_REPL_EMACS_EXTRA_WORDS_MOVE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether to implement auto-indent in REPL
#ifndef MICROPY_REPL_AUTO_INDENT
#define MICROPY_REPL_AUTO_INDENT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether port requires event-driven REPL functions
#ifndef MICROPY_REPL_EVENT_DRIVEN
#define MICROPY_REPL_EVENT_DRIVEN (0)
#endif

// The number of items to keep in the readline history.
#ifndef MICROPY_READLINE_HISTORY_SIZE
#define MICROPY_READLINE_HISTORY_SIZE (8)
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
#define MICROPY_ENABLE_SOURCE_LINE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to include doc strings (increases RAM usage)
#ifndef MICROPY_ENABLE_DOC_STRING
#define MICROPY_ENABLE_DOC_STRING (0)
#endif

// Exception messages are removed (requires disabling MICROPY_ROM_TEXT_COMPRESSION)
#define MICROPY_ERROR_REPORTING_NONE     (0)
// Exception messages are short static strings
#define MICROPY_ERROR_REPORTING_TERSE    (1)
// Exception messages provide basic error details
#define MICROPY_ERROR_REPORTING_NORMAL   (2)
// Exception messages provide full info, e.g. object names
#define MICROPY_ERROR_REPORTING_DETAILED (3)

#ifndef MICROPY_ERROR_REPORTING
#if MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_FULL_FEATURES
#define MICROPY_ERROR_REPORTING (MICROPY_ERROR_REPORTING_DETAILED)
#elif MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES
#define MICROPY_ERROR_REPORTING (MICROPY_ERROR_REPORTING_NORMAL)
#else
#define MICROPY_ERROR_REPORTING (MICROPY_ERROR_REPORTING_TERSE)
#endif
#endif

// Whether issue warnings during compiling/execution
#ifndef MICROPY_WARNINGS
#define MICROPY_WARNINGS (0)
#endif

// Whether to support warning categories
#ifndef MICROPY_WARNINGS_CATEGORY
#define MICROPY_WARNINGS_CATEGORY (0)
#endif

// This macro is used when printing runtime warnings and errors
#ifndef MICROPY_ERROR_PRINTER
#define MICROPY_ERROR_PRINTER (&mp_plat_print)
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
#define MICROPY_FLOAT_CONST(x) x##F
#define MICROPY_FLOAT_C_FUN(fun) fun##f
typedef float mp_float_t;
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define MICROPY_PY_BUILTINS_FLOAT (1)
#define MICROPY_FLOAT_CONST(x) x
#define MICROPY_FLOAT_C_FUN(fun) fun
typedef double mp_float_t;
#else
#define MICROPY_PY_BUILTINS_FLOAT (0)
#endif

#ifndef MICROPY_PY_BUILTINS_COMPLEX
#define MICROPY_PY_BUILTINS_COMPLEX (MICROPY_PY_BUILTINS_FLOAT)
#endif

// Whether to provide a high-quality hash for float and complex numbers.
// Otherwise the default is a very simple but correct hashing function.
#ifndef MICROPY_FLOAT_HIGH_QUALITY_HASH
#define MICROPY_FLOAT_HIGH_QUALITY_HASH (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Enable features which improve CPython compatibility
// but may lead to more code size/memory usage.
// TODO: Originally intended as generic category to not
// add bunch of once-off options. May need refactoring later
#ifndef MICROPY_CPYTHON_COMPAT
#define MICROPY_CPYTHON_COMPAT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Perform full checks as done by CPython. Disabling this
// may produce incorrect results, if incorrect data is fed,
// but should not lead to MicroPython crashes or similar
// grave issues (in other words, only user app should be,
// affected, not system).
#ifndef MICROPY_FULL_CHECKS
#define MICROPY_FULL_CHECKS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether POSIX-semantics non-blocking streams are supported
#ifndef MICROPY_STREAMS_NON_BLOCK
#define MICROPY_STREAMS_NON_BLOCK (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide stream functions with POSIX-like signatures
// (useful for porting existing libraries to MicroPython).
#ifndef MICROPY_STREAMS_POSIX_API
#define MICROPY_STREAMS_POSIX_API (0)
#endif

// Whether modules can use MP_MODULE_ATTR_DELEGATION_ENTRY() to delegate failed
// attribute lookups.
#ifndef MICROPY_MODULE_ATTR_DELEGATION
#define MICROPY_MODULE_ATTR_DELEGATION (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to call __init__ when importing builtin modules for the first time
#ifndef MICROPY_MODULE_BUILTIN_INIT
#define MICROPY_MODULE_BUILTIN_INIT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support module-level __getattr__ (see PEP 562)
#ifndef MICROPY_MODULE_GETATTR
#define MICROPY_MODULE_GETATTR (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether module weak links are supported
#ifndef MICROPY_MODULE_WEAK_LINKS
#define MICROPY_MODULE_WEAK_LINKS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to enable importing foo.py with __name__ set to '__main__'
// Used by the unix port for the -m flag.
#ifndef MICROPY_MODULE_OVERRIDE_MAIN_IMPORT
#define MICROPY_MODULE_OVERRIDE_MAIN_IMPORT (0)
#endif

// Whether frozen modules are supported in the form of strings
#ifndef MICROPY_MODULE_FROZEN_STR
#define MICROPY_MODULE_FROZEN_STR (0)
#endif

// Whether frozen modules are supported in the form of .mpy files
#ifndef MICROPY_MODULE_FROZEN_MPY
#define MICROPY_MODULE_FROZEN_MPY (0)
#endif

// Convenience macro for whether frozen modules are supported
#ifndef MICROPY_MODULE_FROZEN
#define MICROPY_MODULE_FROZEN (MICROPY_MODULE_FROZEN_STR || MICROPY_MODULE_FROZEN_MPY)
#endif

// Whether you can override builtins in the builtins module
#ifndef MICROPY_CAN_OVERRIDE_BUILTINS
#define MICROPY_CAN_OVERRIDE_BUILTINS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to check that the "self" argument of a builtin method has the
// correct type.  Such an explicit check is only needed if a builtin
// method escapes to Python land without a first argument, eg
// list.append([], 1).  Without this check such calls will have undefined
// behaviour (usually segfault) if the first argument is the wrong type.
#ifndef MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to use internally defined errno's (otherwise system provided ones)
#ifndef MICROPY_USE_INTERNAL_ERRNO
#define MICROPY_USE_INTERNAL_ERRNO (0)
#endif

// Whether to use internally defined *printf() functions (otherwise external ones)
#ifndef MICROPY_USE_INTERNAL_PRINTF
#define MICROPY_USE_INTERNAL_PRINTF (1)
#endif

// The mp_print_t printer used for printf output when MICROPY_USE_INTERNAL_PRINTF is enabled
#ifndef MICROPY_INTERNAL_PRINTF_PRINTER
#define MICROPY_INTERNAL_PRINTF_PRINTER (&mp_plat_print)
#endif

// Whether to support mp_sched_vm_abort to asynchronously abort to the top level.
#ifndef MICROPY_ENABLE_VM_ABORT
#define MICROPY_ENABLE_VM_ABORT (0)
#endif

// Support for internal scheduler
#ifndef MICROPY_ENABLE_SCHEDULER
#define MICROPY_ENABLE_SCHEDULER (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether the scheduler supports scheduling static nodes with C callbacks
#ifndef MICROPY_SCHEDULER_STATIC_NODES
#define MICROPY_SCHEDULER_STATIC_NODES (0)
#endif

// Maximum number of entries in the scheduler
#ifndef MICROPY_SCHEDULER_DEPTH
#define MICROPY_SCHEDULER_DEPTH (4)
#endif

// Support for generic VFS sub-system
#ifndef MICROPY_VFS
#define MICROPY_VFS (0)
#endif

// Support for VFS POSIX component, to mount a POSIX filesystem within VFS
#ifndef MICROPY_VFS_POSIX
#define MICROPY_VFS_POSIX (0)
#endif

// Support for VFS FAT component, to mount a FAT filesystem within VFS
#ifndef MICROPY_VFS_FAT
#define MICROPY_VFS_FAT (0)
#endif

// Support for VFS LittleFS v1 component, to mount a LFSv1 filesystem within VFS
#ifndef MICROPY_VFS_LFS1
#define MICROPY_VFS_LFS1 (0)
#endif

// Support for VFS LittleFS v2 component, to mount a LFSv2 filesystem within VFS
#ifndef MICROPY_VFS_LFS2
#define MICROPY_VFS_LFS2 (0)
#endif

/*****************************************************************************/
/* Fine control over Python builtins, classes, modules, etc                  */

// Whether to support multiple inheritance of Python classes.  Multiple
// inheritance makes some C functions inherently recursive, and adds a bit of
// code overhead.
#ifndef MICROPY_MULTIPLE_INHERITANCE
#define MICROPY_MULTIPLE_INHERITANCE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to implement attributes on functions
#ifndef MICROPY_PY_FUNCTION_ATTRS
#define MICROPY_PY_FUNCTION_ATTRS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support the descriptors __get__, __set__, __delete__
// This costs some code size and makes load/store/delete of instance
// attributes slower for the classes that use this feature
#ifndef MICROPY_PY_DESCRIPTORS
#define MICROPY_PY_DESCRIPTORS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support class __delattr__ and __setattr__ methods
// This costs some code size and makes store/delete of instance
// attributes slower for the classes that use this feature
#ifndef MICROPY_PY_DELATTR_SETATTR
#define MICROPY_PY_DELATTR_SETATTR (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Support for async/await/async for/async with
#ifndef MICROPY_PY_ASYNC_AWAIT
#define MICROPY_PY_ASYNC_AWAIT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Support for literal string interpolation, f-strings (see PEP 498, Python 3.6+)
#ifndef MICROPY_PY_FSTRINGS
#define MICROPY_PY_FSTRINGS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Support for assignment expressions with := (see PEP 572, Python 3.8+)
#ifndef MICROPY_PY_ASSIGN_EXPR
#define MICROPY_PY_ASSIGN_EXPR (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Non-standard .pend_throw() method for generators, allowing for
// Future-like behavior with respect to exception handling: an
// exception set with .pend_throw() will activate on the next call
// to generator's .send() or .__next__(). (This is useful to implement
// async schedulers.)
#ifndef MICROPY_PY_GENERATOR_PEND_THROW
#define MICROPY_PY_GENERATOR_PEND_THROW (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Issue a warning when comparing str and bytes objects
#ifndef MICROPY_PY_STR_BYTES_CMP_WARN
#define MICROPY_PY_STR_BYTES_CMP_WARN (0)
#endif

// Add bytes.hex and bytes.fromhex
#ifndef MICROPY_PY_BUILTINS_BYTES_HEX
#define MICROPY_PY_BUILTINS_BYTES_HEX (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether str object is proper unicode
#ifndef MICROPY_PY_BUILTINS_STR_UNICODE
#define MICROPY_PY_BUILTINS_STR_UNICODE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to check for valid UTF-8 when converting bytes to str
#ifndef MICROPY_PY_BUILTINS_STR_UNICODE_CHECK
#define MICROPY_PY_BUILTINS_STR_UNICODE_CHECK (MICROPY_PY_BUILTINS_STR_UNICODE)
#endif

// Whether str.center() method provided
#ifndef MICROPY_PY_BUILTINS_STR_CENTER
#define MICROPY_PY_BUILTINS_STR_CENTER (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether str.count() method provided
#ifndef MICROPY_PY_BUILTINS_STR_COUNT
#define MICROPY_PY_BUILTINS_STR_COUNT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether str % (...) formatting operator provided
#ifndef MICROPY_PY_BUILTINS_STR_OP_MODULO
#define MICROPY_PY_BUILTINS_STR_OP_MODULO (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether str.partition()/str.rpartition() method provided
#ifndef MICROPY_PY_BUILTINS_STR_PARTITION
#define MICROPY_PY_BUILTINS_STR_PARTITION (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether str.splitlines() method provided
#ifndef MICROPY_PY_BUILTINS_STR_SPLITLINES
#define MICROPY_PY_BUILTINS_STR_SPLITLINES (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support bytearray object
#ifndef MICROPY_PY_BUILTINS_BYTEARRAY
#define MICROPY_PY_BUILTINS_BYTEARRAY (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support dict.fromkeys() class method
#ifndef MICROPY_PY_BUILTINS_DICT_FROMKEYS
#define MICROPY_PY_BUILTINS_DICT_FROMKEYS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support memoryview object
#ifndef MICROPY_PY_BUILTINS_MEMORYVIEW
#define MICROPY_PY_BUILTINS_MEMORYVIEW (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support memoryview.itemsize attribute
#ifndef MICROPY_PY_BUILTINS_MEMORYVIEW_ITEMSIZE
#define MICROPY_PY_BUILTINS_MEMORYVIEW_ITEMSIZE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether to support set object
#ifndef MICROPY_PY_BUILTINS_SET
#define MICROPY_PY_BUILTINS_SET (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support slice subscript operators and slice object
#ifndef MICROPY_PY_BUILTINS_SLICE
#define MICROPY_PY_BUILTINS_SLICE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support slice attribute read access,
// i.e. slice.start, slice.stop, slice.step
#ifndef MICROPY_PY_BUILTINS_SLICE_ATTRS
#define MICROPY_PY_BUILTINS_SLICE_ATTRS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support the .indices(len) method on slice objects
#ifndef MICROPY_PY_BUILTINS_SLICE_INDICES
#define MICROPY_PY_BUILTINS_SLICE_INDICES (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support frozenset object
#ifndef MICROPY_PY_BUILTINS_FROZENSET
#define MICROPY_PY_BUILTINS_FROZENSET (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support property object
#ifndef MICROPY_PY_BUILTINS_PROPERTY
#define MICROPY_PY_BUILTINS_PROPERTY (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to implement the start/stop/step attributes (readback) on
// the "range" builtin type. Rarely used, and costs ~60 bytes (x86).
#ifndef MICROPY_PY_BUILTINS_RANGE_ATTRS
#define MICROPY_PY_BUILTINS_RANGE_ATTRS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support binary ops [only (in)equality is defined] between range
// objects.  With this option disabled all range objects that are not exactly
// the same object will compare as not-equal.  With it enabled the semantics
// match CPython and ranges are equal if they yield the same sequence of items.
#ifndef MICROPY_PY_BUILTINS_RANGE_BINOP
#define MICROPY_PY_BUILTINS_RANGE_BINOP (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Support for callling next() with second argument
#ifndef MICROPY_PY_BUILTINS_NEXT2
#define MICROPY_PY_BUILTINS_NEXT2 (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether to support rounding of integers (incl bignum); eg round(123,-1)=120
#ifndef MICROPY_PY_BUILTINS_ROUND_INT
#define MICROPY_PY_BUILTINS_ROUND_INT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support complete set of special methods for user
// classes, or only the most used ones. "Inplace" methods are
// controlled by MICROPY_PY_ALL_INPLACE_SPECIAL_METHODS below.
// "Reverse" methods are controlled by
// MICROPY_PY_REVERSE_SPECIAL_METHODS below.
#ifndef MICROPY_PY_ALL_SPECIAL_METHODS
#define MICROPY_PY_ALL_SPECIAL_METHODS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support all inplace arithmetic operarion methods
// (__imul__, etc.)
#ifndef MICROPY_PY_ALL_INPLACE_SPECIAL_METHODS
#define MICROPY_PY_ALL_INPLACE_SPECIAL_METHODS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether to support reverse arithmetic operarion methods
// (__radd__, etc.). Additionally gated by
// MICROPY_PY_ALL_SPECIAL_METHODS.
#ifndef MICROPY_PY_REVERSE_SPECIAL_METHODS
#define MICROPY_PY_REVERSE_SPECIAL_METHODS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support compile function
#ifndef MICROPY_PY_BUILTINS_COMPILE
#define MICROPY_PY_BUILTINS_COMPILE (MICROPY_ENABLE_COMPILER && MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support enumerate function(type)
#ifndef MICROPY_PY_BUILTINS_ENUMERATE
#define MICROPY_PY_BUILTINS_ENUMERATE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support eval and exec functions
// By default they are supported if the compiler is enabled
#ifndef MICROPY_PY_BUILTINS_EVAL_EXEC
#define MICROPY_PY_BUILTINS_EVAL_EXEC (MICROPY_ENABLE_COMPILER)
#endif

// Whether to support the Python 2 execfile function
#ifndef MICROPY_PY_BUILTINS_EXECFILE
#define MICROPY_PY_BUILTINS_EXECFILE (MICROPY_ENABLE_COMPILER && MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support filter function(type)
#ifndef MICROPY_PY_BUILTINS_FILTER
#define MICROPY_PY_BUILTINS_FILTER (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support reversed function(type)
#ifndef MICROPY_PY_BUILTINS_REVERSED
#define MICROPY_PY_BUILTINS_REVERSED (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to define "NotImplemented" special constant
#ifndef MICROPY_PY_BUILTINS_NOTIMPLEMENTED
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide the built-in input() function. The implementation of this
// uses shared/readline, so can only be enabled if the port uses this readline.
#ifndef MICROPY_PY_BUILTINS_INPUT
#define MICROPY_PY_BUILTINS_INPUT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support min/max functions
#ifndef MICROPY_PY_BUILTINS_MIN_MAX
#define MICROPY_PY_BUILTINS_MIN_MAX (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Support for calls to pow() with 3 integer arguments
#ifndef MICROPY_PY_BUILTINS_POW3
#define MICROPY_PY_BUILTINS_POW3 (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide the help function
#ifndef MICROPY_PY_BUILTINS_HELP
#define MICROPY_PY_BUILTINS_HELP (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Use this to configure the help text shown for help().  It should be a
// variable with the type "const char*".  A sensible default is provided.
#ifndef MICROPY_PY_BUILTINS_HELP_TEXT
#define MICROPY_PY_BUILTINS_HELP_TEXT mp_help_default_text
#endif

// Add the ability to list the available modules when executing help('modules')
#ifndef MICROPY_PY_BUILTINS_HELP_MODULES
#define MICROPY_PY_BUILTINS_HELP_MODULES (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to set __file__ for imported modules
#ifndef MICROPY_PY___FILE__
#define MICROPY_PY___FILE__ (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to provide mem-info related functions in micropython module
#ifndef MICROPY_PY_MICROPYTHON_MEM_INFO
#define MICROPY_PY_MICROPYTHON_MEM_INFO (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide "micropython.stack_use" function
#ifndef MICROPY_PY_MICROPYTHON_STACK_USE
#define MICROPY_PY_MICROPYTHON_STACK_USE (MICROPY_PY_MICROPYTHON_MEM_INFO)
#endif

// Whether to provide the "micropython.heap_locked" function
#ifndef MICROPY_PY_MICROPYTHON_HEAP_LOCKED
#define MICROPY_PY_MICROPYTHON_HEAP_LOCKED (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether to provide "array" module. Note that large chunk of the
// underlying code is shared with "bytearray" builtin type, so to
// get real savings, it should be disabled too.
#ifndef MICROPY_PY_ARRAY
#define MICROPY_PY_ARRAY (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to support slice assignments for array (and bytearray).
// This is rarely used, but adds ~0.5K of code.
#ifndef MICROPY_PY_ARRAY_SLICE_ASSIGN
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support attrtuple type (MicroPython extension)
// It provides space-efficient tuples with attribute access
#ifndef MICROPY_PY_ATTRTUPLE
#define MICROPY_PY_ATTRTUPLE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to provide "collections" module
#ifndef MICROPY_PY_COLLECTIONS
#define MICROPY_PY_COLLECTIONS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to provide "ucollections.deque" type
#ifndef MICROPY_PY_COLLECTIONS_DEQUE
#define MICROPY_PY_COLLECTIONS_DEQUE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide "collections.OrderedDict" type
#ifndef MICROPY_PY_COLLECTIONS_ORDEREDDICT
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide the _asdict function for namedtuple
#ifndef MICROPY_PY_COLLECTIONS_NAMEDTUPLE__ASDICT
#define MICROPY_PY_COLLECTIONS_NAMEDTUPLE__ASDICT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether to provide "math" module
#ifndef MICROPY_PY_MATH
#define MICROPY_PY_MATH (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to provide all math module constants (Python 3.5+), or just pi and e.
#ifndef MICROPY_PY_MATH_CONSTANTS
#define MICROPY_PY_MATH_CONSTANTS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide special math functions: math.{erf,erfc,gamma,lgamma}
#ifndef MICROPY_PY_MATH_SPECIAL_FUNCTIONS
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide math.factorial function
#ifndef MICROPY_PY_MATH_FACTORIAL
#define MICROPY_PY_MATH_FACTORIAL (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide math.isclose function
#ifndef MICROPY_PY_MATH_ISCLOSE
#define MICROPY_PY_MATH_ISCLOSE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide fix for atan2 Inf handling.
#ifndef MICROPY_PY_MATH_ATAN2_FIX_INFNAN
#define MICROPY_PY_MATH_ATAN2_FIX_INFNAN (0)
#endif

// Whether to provide fix for fmod Inf handling.
#ifndef MICROPY_PY_MATH_FMOD_FIX_INFNAN
#define MICROPY_PY_MATH_FMOD_FIX_INFNAN (0)
#endif

// Whether to provide fix for modf negative zero handling.
#ifndef MICROPY_PY_MATH_MODF_FIX_NEGZERO
#define MICROPY_PY_MATH_MODF_FIX_NEGZERO (0)
#endif

// Whether to provide fix for pow(1, NaN) and pow(NaN, 0), which both should be 1 not NaN.
#ifndef MICROPY_PY_MATH_POW_FIX_NAN
#define MICROPY_PY_MATH_POW_FIX_NAN (0)
#endif

// Whether to provide "cmath" module
#ifndef MICROPY_PY_CMATH
#define MICROPY_PY_CMATH (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide "micropython" module
#ifndef MICROPY_PY_MICROPYTHON
#define MICROPY_PY_MICROPYTHON (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to provide "gc" module
#ifndef MICROPY_PY_GC
#define MICROPY_PY_GC (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to return number of collected objects from gc.collect()
#ifndef MICROPY_PY_GC_COLLECT_RETVAL
#define MICROPY_PY_GC_COLLECT_RETVAL (0)
#endif

// Whether to provide "io" module
#ifndef MICROPY_PY_IO
#define MICROPY_PY_IO (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to provide "io.IOBase" class to support user streams
#ifndef MICROPY_PY_IO_IOBASE
#define MICROPY_PY_IO_IOBASE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide "io.BytesIO" class
#ifndef MICROPY_PY_IO_BYTESIO
#define MICROPY_PY_IO_BYTESIO (1)
#endif

// Whether to provide "io.BufferedWriter" class
#ifndef MICROPY_PY_IO_BUFFEREDWRITER
#define MICROPY_PY_IO_BUFFEREDWRITER (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether to provide "struct" module
#ifndef MICROPY_PY_STRUCT
#define MICROPY_PY_STRUCT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to provide "sys" module
#ifndef MICROPY_PY_SYS
#define MICROPY_PY_SYS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_CORE_FEATURES)
#endif

// Whether to initialise "sys.path" and "sys.argv" to their defaults in mp_init()
#ifndef MICROPY_PY_SYS_PATH_ARGV_DEFAULTS
#define MICROPY_PY_SYS_PATH_ARGV_DEFAULTS (MICROPY_PY_SYS)
#endif

// Whether to provide "sys.maxsize" constant
#ifndef MICROPY_PY_SYS_MAXSIZE
#define MICROPY_PY_SYS_MAXSIZE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide "sys.modules" dictionary
#ifndef MICROPY_PY_SYS_MODULES
#define MICROPY_PY_SYS_MODULES (1)
#endif

// Whether to provide "sys.exc_info" function
// Avoid enabling this, this function is Python2 heritage
#ifndef MICROPY_PY_SYS_EXC_INFO
#define MICROPY_PY_SYS_EXC_INFO (0)
#endif

// Whether to provide "sys.executable", which is the absolute path to the
// micropython binary
// Intended for use on the "OS" ports (e.g. Unix)
#ifndef MICROPY_PY_SYS_EXECUTABLE
#define MICROPY_PY_SYS_EXECUTABLE (0)
#endif

// Whether to provide "sys.exit" function
#ifndef MICROPY_PY_SYS_EXIT
#define MICROPY_PY_SYS_EXIT (1)
#endif

// Whether to provide "sys.atexit" function (MicroPython extension)
#ifndef MICROPY_PY_SYS_ATEXIT
#define MICROPY_PY_SYS_ATEXIT (0)
#endif

// Whether to provide sys.{ps1,ps2} mutable attributes, to control REPL prompts
#ifndef MICROPY_PY_SYS_PS1_PS2
#define MICROPY_PY_SYS_PS1_PS2 (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide "sys.settrace" function
#ifndef MICROPY_PY_SYS_SETTRACE
#define MICROPY_PY_SYS_SETTRACE (0)
#endif

// Whether to provide "sys.getsizeof" function
#ifndef MICROPY_PY_SYS_GETSIZEOF
#define MICROPY_PY_SYS_GETSIZEOF (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether to provide sys.{stdin,stdout,stderr} objects
#ifndef MICROPY_PY_SYS_STDFILES
#define MICROPY_PY_SYS_STDFILES (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide sys.{stdin,stdout,stderr}.buffer object
// This is implemented per-port
#ifndef MICROPY_PY_SYS_STDIO_BUFFER
#define MICROPY_PY_SYS_STDIO_BUFFER (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide sys.tracebacklimit mutable attribute
#ifndef MICROPY_PY_SYS_TRACEBACKLIMIT
#define MICROPY_PY_SYS_TRACEBACKLIMIT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

// Whether the sys module supports attribute delegation
// This is enabled automatically when needed by other features
#ifndef MICROPY_PY_SYS_ATTR_DELEGATION
#define MICROPY_PY_SYS_ATTR_DELEGATION (MICROPY_PY_SYS_PS1_PS2 || MICROPY_PY_SYS_TRACEBACKLIMIT)
#endif

// Whether to provide "uerrno" module
#ifndef MICROPY_PY_UERRNO
#define MICROPY_PY_UERRNO (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide the uerrno.errorcode dict
#ifndef MICROPY_PY_UERRNO_ERRORCODE
#define MICROPY_PY_UERRNO_ERRORCODE (1)
#endif

// Whether to provide "uselect" module (baremetal implementation)
#ifndef MICROPY_PY_USELECT
#define MICROPY_PY_USELECT (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to enable the select() function in the "uselect" module (baremetal
// implementation). This is present for compatibility but can be disabled to
// save space.
#ifndef MICROPY_PY_USELECT_SELECT
#define MICROPY_PY_USELECT_SELECT (1)
#endif

// Whether to provide the "utime" module
#ifndef MICROPY_PY_UTIME
#define MICROPY_PY_UTIME (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_BASIC_FEATURES)
#endif

// Whether to provide utime.gmtime/localtime/mktime functions
#ifndef MICROPY_PY_UTIME_GMTIME_LOCALTIME_MKTIME
#define MICROPY_PY_UTIME_GMTIME_LOCALTIME_MKTIME (0)
#endif

// Whether to provide utime.time/time_ns functions
#ifndef MICROPY_PY_UTIME_TIME_TIME_NS
#define MICROPY_PY_UTIME_TIME_TIME_NS (0)
#endif

// Period of values returned by utime.ticks_ms(), ticks_us(), ticks_cpu()
// functions. Should be power of two. All functions above use the same
// period, so if underlying hardware/API has different periods, the
// minimum of them should be used. The value below is the maximum value
// this parameter can take (corresponding to 30 bit tick values on 32-bit
// system).
#ifndef MICROPY_PY_UTIME_TICKS_PERIOD
#define MICROPY_PY_UTIME_TICKS_PERIOD (MP_SMALL_INT_POSITIVE_MASK + 1)
#endif

// Whether to provide "_thread" module
#ifndef MICROPY_PY_THREAD
#define MICROPY_PY_THREAD (0)
#endif

// Whether to make the VM/runtime thread-safe using a global lock
// If not enabled then thread safety must be provided at the Python level
#ifndef MICROPY_PY_THREAD_GIL
#define MICROPY_PY_THREAD_GIL (MICROPY_PY_THREAD)
#endif

// Number of VM jump-loops to do before releasing the GIL.
// Set this to 0 to disable the divisor.
#ifndef MICROPY_PY_THREAD_GIL_VM_DIVISOR
#define MICROPY_PY_THREAD_GIL_VM_DIVISOR (32)
#endif

// Extended modules

#ifndef MICROPY_PY_UASYNCIO
#define MICROPY_PY_UASYNCIO (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_UCTYPES
#define MICROPY_PY_UCTYPES (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to provide SHORT, INT, LONG, etc. types in addition to
// exact-bitness types like INT16, INT32, etc.
#ifndef MICROPY_PY_UCTYPES_NATIVE_C_TYPES
#define MICROPY_PY_UCTYPES_NATIVE_C_TYPES (1)
#endif

#ifndef MICROPY_PY_UZLIB
#define MICROPY_PY_UZLIB (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_UJSON
#define MICROPY_PY_UJSON (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to support the "separators" argument to dump, dumps
#ifndef MICROPY_PY_UJSON_SEPARATORS
#define MICROPY_PY_UJSON_SEPARATORS (1)
#endif

#ifndef MICROPY_PY_UOS
#define MICROPY_PY_UOS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_UOS_STATVFS
#define MICROPY_PY_UOS_STATVFS (MICROPY_PY_UOS)
#endif

#ifndef MICROPY_PY_URE
#define MICROPY_PY_URE (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_URE_DEBUG
#define MICROPY_PY_URE_DEBUG (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

#ifndef MICROPY_PY_URE_MATCH_GROUPS
#define MICROPY_PY_URE_MATCH_GROUPS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

#ifndef MICROPY_PY_URE_MATCH_SPAN_START_END
#define MICROPY_PY_URE_MATCH_SPAN_START_END (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EVERYTHING)
#endif

#ifndef MICROPY_PY_URE_SUB
#define MICROPY_PY_URE_SUB (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_UHEAPQ
#define MICROPY_PY_UHEAPQ (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Optimized heap queue for relative timestamps (only used by uasyncio v2)
#ifndef MICROPY_PY_UTIMEQ
#define MICROPY_PY_UTIMEQ (0)
#endif

#ifndef MICROPY_PY_UHASHLIB
#define MICROPY_PY_UHASHLIB (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_UHASHLIB_MD5
#define MICROPY_PY_UHASHLIB_MD5 (0)
#endif

#ifndef MICROPY_PY_UHASHLIB_SHA1
#define MICROPY_PY_UHASHLIB_SHA1  (0)
#endif

#ifndef MICROPY_PY_UHASHLIB_SHA256
#define MICROPY_PY_UHASHLIB_SHA256 (1)
#endif

#ifndef MICROPY_PY_UCRYPTOLIB
#define MICROPY_PY_UCRYPTOLIB (0)
#endif

// Depends on MICROPY_PY_UCRYPTOLIB
#ifndef MICROPY_PY_UCRYPTOLIB_CTR
#define MICROPY_PY_UCRYPTOLIB_CTR (0)
#endif

#ifndef MICROPY_PY_UCRYPTOLIB_CONSTS
#define MICROPY_PY_UCRYPTOLIB_CONSTS (0)
#endif

#ifndef MICROPY_PY_UBINASCII
#define MICROPY_PY_UBINASCII (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Depends on MICROPY_PY_UZLIB
#ifndef MICROPY_PY_UBINASCII_CRC32
#define MICROPY_PY_UBINASCII_CRC32 (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_URANDOM
#define MICROPY_PY_URANDOM (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

// Whether to include: randrange, randint, choice, random, uniform
#ifndef MICROPY_PY_URANDOM_EXTRA_FUNCS
#define MICROPY_PY_URANDOM_EXTRA_FUNCS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_MACHINE
#define MICROPY_PY_MACHINE (0)
#endif

// Whether to include: bitstream
#ifndef MICROPY_PY_MACHINE_BITSTREAM
#define MICROPY_PY_MACHINE_BITSTREAM (0)
#endif

// Whether to include: time_pulse_us
#ifndef MICROPY_PY_MACHINE_PULSE
#define MICROPY_PY_MACHINE_PULSE (0)
#endif

#ifndef MICROPY_PY_MACHINE_I2C
#define MICROPY_PY_MACHINE_I2C (0)
#endif

// Whether the low-level I2C transfer function supports a separate write as the first transfer
#ifndef MICROPY_PY_MACHINE_I2C_TRANSFER_WRITE1
#define MICROPY_PY_MACHINE_I2C_TRANSFER_WRITE1 (0)
#endif

// Whether to provide the "machine.SoftI2C" class
#ifndef MICROPY_PY_MACHINE_SOFTI2C
#define MICROPY_PY_MACHINE_SOFTI2C (0)
#endif

#ifndef MICROPY_PY_MACHINE_SPI
#define MICROPY_PY_MACHINE_SPI (0)
#endif

// Whether to provide the "machine.SoftSPI" class
#ifndef MICROPY_PY_MACHINE_SOFTSPI
#define MICROPY_PY_MACHINE_SOFTSPI (0)
#endif

// Whether to provide the "machine.Timer" class
#ifndef MICROPY_PY_MACHINE_TIMER
#define MICROPY_PY_MACHINE_TIMER (0)
#endif

// The default backlog value for socket.listen(backlog)
#ifndef MICROPY_PY_USOCKET_LISTEN_BACKLOG_DEFAULT
#define MICROPY_PY_USOCKET_LISTEN_BACKLOG_DEFAULT (2)
#endif

#ifndef MICROPY_PY_USSL
#define MICROPY_PY_USSL (0)
#endif

// Whether to add finaliser code to ussl objects
#ifndef MICROPY_PY_USSL_FINALISER
#define MICROPY_PY_USSL_FINALISER (0)
#endif

#ifndef MICROPY_PY_UWEBSOCKET
#define MICROPY_PY_UWEBSOCKET (0)
#endif

#ifndef MICROPY_PY_FRAMEBUF
#define MICROPY_PY_FRAMEBUF (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_BTREE
#define MICROPY_PY_BTREE (0)
#endif

// Whether to provide the low-level "_onewire" module
#ifndef MICROPY_PY_ONEWIRE
#define MICROPY_PY_ONEWIRE (0)
#endif

/*****************************************************************************/
/* Hooks for a port to add builtins                                          */

// Additional builtin function definitions - see modbuiltins.c:mp_module_builtins_globals_table for format.
#ifndef MICROPY_PORT_BUILTINS
#define MICROPY_PORT_BUILTINS
#endif

// Additional builtin function definitions for extension by command-line, boards or variants.
// See modbuiltins.c:mp_module_builtins_globals_table for format.
#ifndef MICROPY_PORT_EXTRA_BUILTINS
#define MICROPY_PORT_EXTRA_BUILTINS
#endif

// Additional constant definitions for the compiler - see compile.c:mp_constants_table.
#ifndef MICROPY_PORT_CONSTANTS
#define MICROPY_PORT_CONSTANTS
#endif

/*****************************************************************************/
/* Hooks for a port to wrap functions with attributes                        */

#ifndef MICROPY_WRAP_MP_BINARY_OP
#define MICROPY_WRAP_MP_BINARY_OP(f) f
#endif

#ifndef MICROPY_WRAP_MP_EXECUTE_BYTECODE
#define MICROPY_WRAP_MP_EXECUTE_BYTECODE(f) f
#endif

#ifndef MICROPY_WRAP_MP_LOAD_GLOBAL
#define MICROPY_WRAP_MP_LOAD_GLOBAL(f) f
#endif

#ifndef MICROPY_WRAP_MP_LOAD_NAME
#define MICROPY_WRAP_MP_LOAD_NAME(f) f
#endif

#ifndef MICROPY_WRAP_MP_MAP_LOOKUP
#define MICROPY_WRAP_MP_MAP_LOOKUP(f) f
#endif

#ifndef MICROPY_WRAP_MP_OBJ_GET_TYPE
#define MICROPY_WRAP_MP_OBJ_GET_TYPE(f) f
#endif

#ifndef MICROPY_WRAP_MP_SCHED_EXCEPTION
#define MICROPY_WRAP_MP_SCHED_EXCEPTION(f) f
#endif

#ifndef MICROPY_WRAP_MP_SCHED_KEYBOARD_INTERRUPT
#define MICROPY_WRAP_MP_SCHED_KEYBOARD_INTERRUPT(f) f
#endif

#ifndef MICROPY_WRAP_MP_SCHED_SCHEDULE
#define MICROPY_WRAP_MP_SCHED_SCHEDULE(f) f
#endif

#ifndef MICROPY_WRAP_MP_SCHED_VM_ABORT
#define MICROPY_WRAP_MP_SCHED_VM_ABORT(f) f
#endif

/*****************************************************************************/
/* Miscellaneous settings                                                    */

// All uPy objects in ROM must be aligned on at least a 4 byte boundary
// so that the small-int/qstr/pointer distinction can be made.  For machines
// that don't do this (eg 16-bit CPU), define the following macro to something
// like __attribute__((aligned(4))).
#ifndef MICROPY_OBJ_BASE_ALIGNMENT
#define MICROPY_OBJ_BASE_ALIGNMENT
#endif

// String used for the banner, and sys.version additional information
#ifndef MICROPY_BANNER_NAME_AND_VERSION
#define MICROPY_BANNER_NAME_AND_VERSION "MicroPython " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE
#endif

// String used for the second part of the banner, and sys.implementation._machine
#ifndef MICROPY_BANNER_MACHINE
#ifdef MICROPY_HW_BOARD_NAME
#define MICROPY_BANNER_MACHINE MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME
#else
#define MICROPY_BANNER_MACHINE MICROPY_PY_SYS_PLATFORM " [" MICROPY_PLATFORM_COMPILER "] version"
#endif
#endif

// On embedded platforms, these will typically enable/disable irqs.
#ifndef MICROPY_BEGIN_ATOMIC_SECTION
#define MICROPY_BEGIN_ATOMIC_SECTION() (0)
#endif
#ifndef MICROPY_END_ATOMIC_SECTION
#define MICROPY_END_ATOMIC_SECTION(state) (void)(state)
#endif

// Allow to override static modifier for global objects, e.g. to use with
// object code analysis tools which don't support static symbols.
#ifndef STATIC
#define STATIC static
#endif

// Number of bytes in an object word: mp_obj_t, mp_uint_t, mp_uint_t
#ifndef MP_BYTES_PER_OBJ_WORD
#define MP_BYTES_PER_OBJ_WORD (sizeof(mp_uint_t))
#endif

// Number of bits in a byte
#ifndef MP_BITS_PER_BYTE
#define MP_BITS_PER_BYTE (8)
#endif
// mp_int_t value with most significant bit set
#define MP_OBJ_WORD_MSBIT_HIGH (((mp_uint_t)1) << (MP_BYTES_PER_OBJ_WORD * MP_BITS_PER_BYTE - 1))

// Make sure both MP_ENDIANNESS_LITTLE and MP_ENDIANNESS_BIG are
// defined and that they are the opposite of each other.
#if defined(MP_ENDIANNESS_LITTLE)
#define MP_ENDIANNESS_BIG (!MP_ENDIANNESS_LITTLE)
#elif defined(MP_ENDIANNESS_BIG)
#define MP_ENDIANNESS_LITTLE (!MP_ENDIANNESS_BIG)
#else
// Endianness not defined by port so try to autodetect it.
  #if defined(__BYTE_ORDER__)
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      #define MP_ENDIANNESS_LITTLE (1)
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      #define MP_ENDIANNESS_LITTLE (0)
    #endif
  #else
    #include <endian.h>
      #if defined(__BYTE_ORDER)
        #if __BYTE_ORDER == __LITTLE_ENDIAN
          #define MP_ENDIANNESS_LITTLE (1)
        #elif __BYTE_ORDER == __BIG_ENDIAN
          #define MP_ENDIANNESS_LITTLE (0)
        #endif
      #endif
  #endif
  #ifndef MP_ENDIANNESS_LITTLE
    #error endianness not defined and cannot detect it
  #endif
  #define MP_ENDIANNESS_BIG (!MP_ENDIANNESS_LITTLE)
#endif

// Make a pointer to RAM callable (eg set lower bit for Thumb code)
// (This scheme won't work if we want to mix Thumb and normal ARM code.)
#ifndef MICROPY_MAKE_POINTER_CALLABLE
#define MICROPY_MAKE_POINTER_CALLABLE(p) (p)
#endif

// If these MP_PLAT_*_EXEC macros are overridden then the memory allocated by them
// must be somehow reachable for marking by the GC, since the native code
// generators store pointers to GC managed memory in the code.
#ifndef MP_PLAT_ALLOC_EXEC
#define MP_PLAT_ALLOC_EXEC(min_size, ptr, size) do { *ptr = m_new(byte, min_size); *size = min_size; } while (0)
#endif

#ifndef MP_PLAT_FREE_EXEC
#define MP_PLAT_FREE_EXEC(ptr, size) m_del(byte, ptr, size)
#endif

// This macro is used to do all output (except when MICROPY_PY_IO is defined)
#ifndef MP_PLAT_PRINT_STRN
#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)
#endif

#ifndef MP_SSIZE_MAX
#define MP_SSIZE_MAX SSIZE_MAX
#endif

// printf format spec to use for mp_int_t and friends
#ifndef INT_FMT
#if defined(__LP64__)
// Archs where mp_int_t == long, long != int
#define UINT_FMT "%lu"
#define INT_FMT "%ld"
#elif defined(_WIN64)
#define UINT_FMT "%llu"
#define INT_FMT "%lld"
#else
// Archs where mp_int_t == int
#define UINT_FMT "%u"
#define INT_FMT "%d"
#endif
#endif // INT_FMT

// Modifier for function which doesn't return
#ifndef NORETURN
#define NORETURN __attribute__((noreturn))
#endif

// Modifier for weak functions
#ifndef MP_WEAK
#define MP_WEAK __attribute__((weak))
#endif

// Modifier for functions which should be never inlined
#ifndef MP_NOINLINE
#define MP_NOINLINE __attribute__((noinline))
#endif

// Modifier for functions which should be always inlined
#ifndef MP_ALWAYSINLINE
#define MP_ALWAYSINLINE __attribute__((always_inline))
#endif

// Condition is likely to be true, to help branch prediction
#ifndef MP_LIKELY
#define MP_LIKELY(x) __builtin_expect((x), 1)
#endif

// Condition is likely to be false, to help branch prediction
#ifndef MP_UNLIKELY
#define MP_UNLIKELY(x) __builtin_expect((x), 0)
#endif

// To annotate that code is unreachable
#ifndef MP_UNREACHABLE
#if defined(__GNUC__)
#define MP_UNREACHABLE __builtin_unreachable();
#else
#define MP_UNREACHABLE for (;;);
#endif
#endif

// Explicitly annotate switch case fall throughs
#if defined(__GNUC__) && __GNUC__ >= 7
#define MP_FALLTHROUGH __attribute__((fallthrough));
#else
#define MP_FALLTHROUGH
#endif

#ifndef MP_HTOBE16
#if MP_ENDIANNESS_LITTLE
#define MP_HTOBE16(x) ((uint16_t)((((x) & 0xff) << 8) | (((x) >> 8) & 0xff)))
#define MP_BE16TOH(x) MP_HTOBE16(x)
#else
#define MP_HTOBE16(x) (x)
#define MP_BE16TOH(x) (x)
#endif
#endif

#ifndef MP_HTOBE32
#if MP_ENDIANNESS_LITTLE
#define MP_HTOBE32(x) ((uint32_t)((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) >> 8) & 0xff00) | (((x) >> 24) & 0xff)))
#define MP_BE32TOH(x) MP_HTOBE32(x)
#else
#define MP_HTOBE32(x) (x)
#define MP_BE32TOH(x) (x)
#endif
#endif

// Warning categories are by default implemented as strings, though
// hook is left for a port to define them as something else.
#if MICROPY_WARNINGS_CATEGORY
#ifndef MP_WARN_CAT
#define MP_WARN_CAT(x) #x
#endif
#else
#undef MP_WARN_CAT
#define MP_WARN_CAT(x) (NULL)
#endif

// Feature dependency check.
#if MICROPY_PY_SYS_SETTRACE
#if !MICROPY_PERSISTENT_CODE_SAVE
#error "MICROPY_PY_SYS_SETTRACE requires MICROPY_PERSISTENT_CODE_SAVE to be enabled"
#endif
#if MICROPY_COMP_CONST
#error "MICROPY_PY_SYS_SETTRACE requires MICROPY_COMP_CONST to be disabled"
#endif
#endif

#endif // MICROPY_INCLUDED_PY_MPCONFIG_H
