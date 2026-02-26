/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include <stdint.h>

#include "mpconfigboard.h"

/****************************** user define ************************************/
// python grammar
#define MICROPY_PY_ATTRTUPLE                (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY 		(1)
#define MICROPY_CPYTHON_COMPAT      		(1)

// builtin op
#define MICROPY_PY_BUILTINS_STR_OP_MODULO   (1)
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       quecpython_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES    (1)

// modules
#define MICROPY_PY_IO                       (1)
#define MICROPY_PY_JSON            		    (1)
#define MICROPY_VFS                         (1)
#define MICROPY_PY_VFS                      (0)
#define MICROPY_PY_TIME                     (0)
#define MICROPY_PY_HEAPQ                    (0)
#define MICROPY_PY_OS                       (0)
#define MICROPY_PY_PLATFORM                 (0)
#define MICROPY_PY_SELECT                   (0)
#define MICROPY_PY_FRAMEBUF                 (0)
#define MICROPY_PY_ASYNCIO                  (0)
#define MICROPY_PY_BINASCII_CRC32           (0)

#define MICROPY_PY_UCTYPES          		(MICROPY_QPY_PY_UCTYPES)
#define MICROPY_PY_DEFLATE            		(0)

#define MICROPY_OPT_COMPUTED_GOTO           (1)

/******************************** base define **********************************/
// options to control how MicroPython is built

// Use the minimal starting configuration (disables all optional features).
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER     	  (1)

#define MICROPY_ENABLE_GC                 (1)
#define MICROPY_PY_GC               	  (1)
#define MICROPY_HELPER_REPL               (1)
#define MICROPY_ENABLE_EXTERNAL_IMPORT    (1)
#define MICROPY_LONGINT_IMPL              (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          	  (MICROPY_FLOAT_IMPL_DOUBLE) //decimal numbers support
#define MICROPY_USE_INTERNAL_PRINTF 	  (0)
#define MPZ_DIG_SIZE                      (16)
#define MICROPY_PY_SYS                    (1)

#define MICROPY_ALLOC_PATH_MAX            (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT    (16)

#define MICROPY_ERROR_REPORTING           MICROPY_ERROR_REPORTING_NORMAL
#define MICROPY_USE_INTERNAL_ERRNO        (1)
#define MICROPY_ENABLE_SCHEDULER    	  (1)
#define MICROPY_SCHEDULER_STATIC_NODES	  (0)
#define MICROPY_SCHEDULER_DEPTH     	  (64)//8
#define MICROPY_REPL_AUTO_INDENT    	  (1)
#define MICROPY_PY_UTIME_MP_HAL    	  	  (1)


#define MICROPY_PY_THREAD           	  (1)
#define MICROPY_PY_THREAD_GIL       	  (1)
#define MICROPY_ENABLE_CALLBACK_DEAL      (1)
#define MICROPY_READER_VFS                (1)

#define MICROPY_KBD_EXCEPTION             (1)
#define MICROPY_PY_SOFT_RESET             (0)
#define MICROPY_PY_IO_FILEIO			  (1)
#define MICROPY_PY_BUILTINS_SLICE   	  (1)
#define MICROPY_PY_BUILTINS_PROPERTY 	  (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY 	  (1)
#define MICROPY_PY_BUILTINS_SET      	  (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW	  (1)
#define MICROPY_PY_SYS_STDFILES			  (1)
#define MICROPY_PY_SYS_STDIO_BUFFER		  (1)
#define MICROPY_PY_STRUCT				  (1)
#define MICROPY_ENABLE_FINALISER		  (1)
#define MICROPY_PY_BUILTINS_ENUMERATE     (1)
#define MICROPY_PY_BUILTINS_MIN_MAX       (1)
#if defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)
#define MICROPY_PY_URANDOM				  (1)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS 	  (1)
#endif

#define MICROPY_PERSISTENT_CODE_LOAD      (1)
#define MICROPY_PY_MICROPYTHON            (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO   (1)

#ifndef SSIZE_MAX
#define SSIZE_MAX  0xFFFFFFFF
#endif

#if defined(PLAT_Unisoc) \
 || defined(PLAT_Unisoc_8850) \
 || defined(PLAT_ASR_1803s) \
 || defined(PLAT_RDA) \
 || defined(PLAT_Qualcomm) \
 || defined(PLAT_ASR)
#define MICROPY_PY_REPL_PASSWORD_PROTECT  (1)
#endif

#if defined(PLAT_SONY_ALT1350)
#define MICROPY_PY_USOCKET_EVENTS  (1)
#endif

#if defined(PLAT_SONY_ALT1350)
#define MICROPY_PY_USSL               (1)
#define MICROPY_SSL_MBEDTLS           (1)
#endif

#if defined(PLAT_Qualcomm)
#define mp_type_fileio                      mp_type_vfs_efs_fileio
#define mp_type_textio                      mp_type_vfs_efs_textio
#elif defined(PLAT_EIGEN)
#define mp_type_fileio                      mp_type_vfs_lfs2_fileio
#define mp_type_textio                      mp_type_vfs_lfs2_textio
#elif defined(PLAT_EIGEN_718)
#define mp_type_fileio                      mp_type_vfs_lfs2_fileio
#define mp_type_textio                      mp_type_vfs_lfs2_textio
#else
#define mp_type_fileio                      mp_type_vfs_lfs1_fileio
#define mp_type_textio                      mp_type_vfs_lfs1_textio
#endif


// type definitions for the specific machine

typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef long mp_off_t;
#define UINT_FMT "%lu"
#define INT_FMT "%ld"


#if MICROPY_ENABLE_GC
#ifndef MICROPY_QPY_GC_HEAP_SIZE
#define MICROPY_QPY_GC_HEAP_SIZE (512 * 1024)
#endif
#define MICROPY_GC_HEAP_SIZE    (MICROPY_QPY_GC_HEAP_SIZE)
#endif

#ifdef MICROPY_QPY_MAIN_TASK_STACK_SIZE
#define MP_QPY_TASK_STACK_SIZE      (MICROPY_QPY_MAIN_TASK_STACK_SIZE)
#else
#if defined(PLAT_RDA)
#define MP_QPY_TASK_STACK_SIZE      (32 * 1024)
#elif defined(PLAT_EIGEN)
#define MP_QPY_TASK_STACK_SIZE      (32 * 1024)
#elif defined(PLAT_EIGEN_718)
#define MP_QPY_TASK_STACK_SIZE      (8 * 1024)
#elif defined(PLAT_ECR6600)
#define MP_QPY_TASK_STACK_SIZE      (8 * 1024)
#elif defined(PLAT_aic8800m40)
#define MP_QPY_TASK_STACK_SIZE      (10 * 1024)
#elif defined(PLAT_Unisoc_8850) || defined(PLAT_Unisoc_8850_R02)
	#if defined(BOARD_EC600GCN_LD) || defined(BOARD_EC800GCN_LD) || defined(BOARD_EC800GCN_LD_XBND) \
	|| defined(BOARD_EC800GCN_LD_HRXM) || defined(BOARD_EC600GCN_LD_YM) || defined(BOARD_EC800GCN_TT) \
	|| defined(BOARD_EG800GLA_LD)
		#define MP_QPY_TASK_STACK_SIZE      (16 * 1024)
	#else
		#define MP_QPY_TASK_STACK_SIZE      (64 * 1024)
	#endif
#else
#define MP_QPY_TASK_STACK_SIZE      (64 * 1024)
#endif
#endif


// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define STRINGIFY_VALUE(s)  STRINGIFY(s)
#define STRINGIFY(s) #s
#define MICROPY_HW_BOARD_NAME STRINGIFY_VALUE(BOARD)
#define MICROPY_HW_MCU_NAME	"QUECTEL"
// board specifics
#define MICROPY_PY_SYS_PLATFORM STRINGIFY_VALUE(BOARD)
#define QUECPYTHON_VERSION_STRING STRINGIFY_VALUE(QPYVER)

#ifdef __linux__
#define MICROPY_MIN_USE_STDOUT (1)
#endif

#ifdef __thumb__
#define MICROPY_MIN_USE_CORTEX_CPU (0)
#define MICROPY_MIN_USE_STM32_MCU (0)
#endif

#define MP_STATE_PORT MP_STATE_VM


#if MICROPY_PY_THREAD
#define MICROPY_EVENT_POLL_HOOK \
		do { \
			extern void mp_handle_pending(bool); \
			mp_handle_pending(true); \
			MP_THREAD_GIL_EXIT(); \
			MP_THREAD_GIL_ENTER(); \
		} while (0);
#else
#define MICROPY_EVENT_POLL_HOOK \
		do { \
			extern void mp_handle_pending(bool); \
			mp_handle_pending(true); \
			asm ("waiti 0"); \
		} while (0);
#endif
