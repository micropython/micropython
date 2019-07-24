/*
 * Amazon FreeRTOS
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
#ifndef AWS_CBOR_ALLOC_H
#define AWS_CBOR_ALLOC_H

#include "aws_cbor.h"

/**
 * @file
 *
 * Function pointers to allocators for compatability with some embedded
 * platforms and RTOSes (e.g. FreeRTOS).
 *
 * @code
 * pxCBOR_malloc  = pvPortMalloc
 * pxCBOR_free    = vPortFree
 * pxCBOR_realloc = pvPortRealloc
 * @endcode
 */

/** Defaults to \c malloc from \b stdlib.h */
extern void *( *pxCBOR_malloc )( size_t );
/** Defaults to \c free from \b stdlib.h */
extern void (* pxCBOR_free)( void * );
/** Defaults to \c realloc from \b stdlib.h */
extern void *(* pxCBOR_realloc)( void *,
                                 size_t );

/**
 * @brief Implementation of realloc for platforms that lack realloc (e.g.
 *     FreeRTOS)
 * @warning Do not use if another realloc function is available!
 */
void * CBOR_ReallocImpl( void * /*old_ptr*/, size_t /*new_size*/ );

#endif /* end of include guard: AWS_CBOR_ALLOC_H */
