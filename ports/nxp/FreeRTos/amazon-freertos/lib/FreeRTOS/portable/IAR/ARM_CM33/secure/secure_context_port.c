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

/* Secure context includes. */
#include "secure_context.h"

/* Secure port macros. */
#include "secure_port_macros.h"

/* Functions implemented in assembler file. */
extern void SecureContext_LoadContextAsm( SecureContextHandle_t xSecureContextHandle );
extern void SecureContext_SaveContextAsm( SecureContextHandle_t xSecureContextHandle );

secureportNON_SECURE_CALLABLE void SecureContext_LoadContext( SecureContextHandle_t xSecureContextHandle )
{
	SecureContext_LoadContextAsm( xSecureContextHandle );
}
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureContext_SaveContext( SecureContextHandle_t xSecureContextHandle )
{
	SecureContext_SaveContextAsm( xSecureContextHandle );
}
/*-----------------------------------------------------------*/
