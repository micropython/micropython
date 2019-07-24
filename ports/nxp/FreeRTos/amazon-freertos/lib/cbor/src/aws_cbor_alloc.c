/*
 * Amazon FreeRTOS CBOR Library V1.0.1
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
#include "aws_cbor_internals.h"
#include <stdlib.h>
#include <string.h>

#ifdef __free_rtos__
    #include "FreeRTOS.h"
    void *( *pxCBOR_malloc )( size_t ) = pvPortMalloc;
    void (* pxCBOR_free)( void * ) = vPortFree;
    void *(* pxCBOR_realloc)( void *,
                              size_t ) = CBOR_ReallocImpl;
#else
    void *(* pxCBOR_malloc)( size_t ) = malloc;
    void (* pxCBOR_free)( void * ) = free;
    void *(* pxCBOR_realloc)( void *,
                              size_t ) = realloc;
#endif

void * CBOR_ReallocImpl( void * pxOld_ptr,
                         size_t xNew_size )
{
    void * pxNew_ptr = pxCBOR_malloc( xNew_size );

    if( NULL == pxNew_ptr )
    {
        return NULL;
    }

    /* Ref: aws_cbor_mem.c:64 */
    /* Realloc is called with a 1.5 multiplier */

    /* This calculates the old size to ensure the correct amount of memory is
     * copied */
    size_t xOld_size = xNew_size * 2 + 1;
    xOld_size /= 3;
    memcpy( pxNew_ptr, pxOld_ptr, xOld_size );
    pxCBOR_free( pxOld_ptr );

    return pxNew_ptr;
}
