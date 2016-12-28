#pragma once

/**
  ******************************************************************************
  * @file    dynalib.h
  * @authors Matthew McGowan
  * @brief   Import and Export of dynamically linked functions
  ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
  */

//
// The dynalib macros produce either a a dynamic lib export table (jump table)
// or a set of import stubs.
// DYNALIB_EXPORT is defined to produce a jump table.
// DYNALIB_IMPORT is defined to produce a set of function stubs
//

// DYNALIB_EXETRN_C to mark symbols with C linkage
#ifdef __cplusplus
#define DYNALIB_EXTERN_C extern "C"
#define EXTERN_C DYNALIB_EXTERN_C
#else
#define DYNALIB_EXTERN_C
#define EXTERN_C extern
#endif

#define DYNALIB_TABLE_EXTERN(tablename) \
    EXTERN_C const void* const dynalib_##tablename[];

#define DYNALIB_TABLE_NAME(tablename) \
    dynalib_##tablename


#ifdef DYNALIB_EXPORT

    /**
     * Begin the jump table definition
     */
    #define DYNALIB_BEGIN(tablename) \
        DYNALIB_EXTERN_C const void* const dynalib_##tablename[] = {

    #define DYNALIB_FN(tablename,name) \
        (const void*)&name,

    #define DYNALIB_FN_PLACEHOLDER(tablename) \
        0,

    #define DYNALIB_END(name)   \
        0 };


#elif defined(DYNALIB_IMPORT)

    #ifdef __arm__

        #define DYNALIB_BEGIN(tablename)    \
            EXTERN_C const void* dynalib_location_##tablename;

        #define __S(x) #x
        #define __SX(x) __S(x)

        #define DYNALIB_FN(tablename, name) \
            const char check_name_##tablename_##name[0]={}; /* this will fail if the name is already defined */ \
            void name() __attribute__((naked)); \
            void name() { \
                asm volatile ( \
                    ".equ offset, ( " __SX(__COUNTER__) " * 4)\n" \
                    ".extern dynalib_location_" #tablename "\n" \
                    "push {r3, lr}\n"           /* save register we will change plus storage for sp value */ \
                                                /* pushes highest register first, so lowest register is at lowest memory address */ \
                                                /* SP points to the last pushed item, which is r3. sp+4 is then the pushed lr value */ \
                    "ldr r3, =dynalib_location_" #tablename "\n" \
                    "ldr r3, [r3]\n"                    /* the address of the jump table */ \
                    "ldr r3, [r3, #offset]\n"    /* the address at index __COUNTER__ */ \
                    "str r3, [sp, #4]\n"                /* patch the link address on the stack */ \
                    "pop {r3, pc}\n"                    /* restore register and jump to function */ \
                ); \
            };

        #define DYNALIB_FN_PLACEHOLDER(tablename)

        #define DYNALIB_END(name)
    #else
        #error Unknown architecture
    #endif // __arm__
#endif


