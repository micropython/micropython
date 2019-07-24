/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef __SECURE_INIT_H__
#define __SECURE_INIT_H__

/**
 * @brief De-prioritizes the non-secure exceptions.
 *
 * This is needed to ensure that the non-secure PendSV runs at the lowest
 * priority. Context switch is done in the non-secure PendSV handler.
 *
 * @note This function must be called in the handler mode. It is no-op if called
 * in the thread mode.
 */
void SecureInit_DePrioritizeNSExceptions( void );

/**
 * @brief Sets up the Floating Point Unit (FPU) for Non-Secure access.
 *
 * Also sets FPCCR.TS=1 to ensure that the content of the Floating Point
 * Registers are not leaked to the non-secure side.
 *
 * @note This function must be called in the handler mode. It is no-op if called
 * in the thread mode.
 */
void SecureInit_EnableNSFPUAccess( void );

#endif /* __SECURE_INIT_H__ */
