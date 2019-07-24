/*
 * Amazon FreeRTOS Demo Bootloader V1.4.6
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */



/**
 * @file aws_boot_log.h
 * @brief Boot logging utilities header.
 */

#ifndef _AWS_BOOT_LOG_H_
#define _AWS_BOOT_LOG_H_

/* Standard includes.*/
#include "stddef.h"

/*
 * Max characters per line.
 */
#define BOOT_LOG_PRINT_BUFFER_SIZE    ( 120U )

/*
 * Debug log level setting.
 */
#define BOOT_LOG_DEBUG_LEVEL          ( 2U )

/*
 * Debug print function.
 */
#define BOOT_LOG_CONSOLE              BOOT_LOG_Printf

/*
 * Boot debug print definitions.
 */
#if BOOT_LOG_DEBUG_LEVEL >= 1
    #define DEFINE_BOOT_METHOD_NAME( name )    static const char BOOT_METHOD_NAME[] = name;
    #define BOOT_LOG_L1    BOOT_LOG_CONSOLE
#else
    #define BOOT_LOG_L1( ... )
#endif

#if BOOT_LOG_DEBUG_LEVEL >= 2
    #define BOOT_LOG_L2    BOOT_LOG_CONSOLE
#else
    #define BOOT_LOG_L2( ... )
#endif

#if BOOT_LOG_DEBUG_LEVEL >= 3
    #define BOOT_LOG_L3    BOOT_LOG_CONSOLE
#else
    #define BOOT_LOG_L3( ... )
#endif

/**
 * @brief BOOT_LOG_UART_Write
 * This is a platform specific uart write function.
 */
size_t BOOT_LOG_UART_Write( const char * pcBuf,
                            size_t xNumBytes );

#endif /*_AWS_BOOT_LOG_H_*/
