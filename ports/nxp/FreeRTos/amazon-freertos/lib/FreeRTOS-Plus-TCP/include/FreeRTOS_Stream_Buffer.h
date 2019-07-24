/*
 * FreeRTOS+TCP V2.0.10
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/*
 *	FreeRTOS_Stream_Buffer.h
 *
 *	A cicular character buffer
 *	An implementation of a circular buffer without a length field
 *	If LENGTH defines the size of the buffer, a maximum of (LENGT-1) bytes can be stored
 *	In order to add or read data from the buffer, memcpy() will be called at most 2 times
 */

#ifndef FREERTOS_STREAM_BUFFER_H
#define	FREERTOS_STREAM_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xSTREAM_BUFFER {
	volatile size_t uxTail;		/* next item to read */
	volatile size_t uxMid;		/* iterator within the valid items */
	volatile size_t uxHead;		/* next position store a new item */
	volatile size_t uxFront;	/* iterator within the free space */
	size_t LENGTH;				/* const value: number of reserved elements */
	uint8_t ucArray[ sizeof( size_t ) ];
} StreamBuffer_t;

static portINLINE void vStreamBufferClear( StreamBuffer_t *pxBuffer );
static portINLINE void vStreamBufferClear( StreamBuffer_t *pxBuffer )
{
	/* Make the circular buffer empty */
	pxBuffer->uxHead = 0u;
	pxBuffer->uxTail = 0u;
	pxBuffer->uxFront = 0u;
	pxBuffer->uxMid = 0u;
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxStreamBufferSpace( const StreamBuffer_t *pxBuffer, const size_t uxLower, const size_t uxUpper );
static portINLINE size_t uxStreamBufferSpace( const StreamBuffer_t *pxBuffer, const size_t uxLower, const size_t uxUpper )
{
/* Returns the space between uxLower and uxUpper, which equals to the distance minus 1 */
size_t uxCount;

	uxCount = pxBuffer->LENGTH + uxUpper - uxLower - 1u;
	if( uxCount >= pxBuffer->LENGTH )
	{
		uxCount -= pxBuffer->LENGTH;
	}

	return uxCount;
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxStreamBufferDistance( const StreamBuffer_t *pxBuffer, const size_t uxLower, const size_t uxUpper );
static portINLINE size_t uxStreamBufferDistance( const StreamBuffer_t *pxBuffer, const size_t uxLower, const size_t uxUpper )
{
/* Returns the distance between uxLower and uxUpper */
size_t uxCount;

	uxCount = pxBuffer->LENGTH + uxUpper - uxLower;
	if ( uxCount >= pxBuffer->LENGTH )
	{
		uxCount -= pxBuffer->LENGTH;
	}

	return uxCount;
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxStreamBufferGetSpace( const StreamBuffer_t *pxBuffer );
static portINLINE size_t uxStreamBufferGetSpace( const StreamBuffer_t *pxBuffer )
{
/* Returns the number of items which can still be added to uxHead
before hitting on uxTail */
size_t uxHead = pxBuffer->uxHead;
size_t uxTail = pxBuffer->uxTail;

	return uxStreamBufferSpace( pxBuffer, uxHead, uxTail );
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxStreamBufferFrontSpace( const StreamBuffer_t *pxBuffer );
static portINLINE size_t uxStreamBufferFrontSpace( const StreamBuffer_t *pxBuffer )
{
/* Distance between uxFront and uxTail
or the number of items which can still be added to uxFront,
before hitting on uxTail */

size_t uxFront = pxBuffer->uxFront;
size_t uxTail = pxBuffer->uxTail;

	return uxStreamBufferSpace( pxBuffer, uxFront, uxTail );
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxStreamBufferGetSize( const StreamBuffer_t *pxBuffer );
static portINLINE size_t uxStreamBufferGetSize( const StreamBuffer_t *pxBuffer )
{
/* Returns the number of items which can be read from uxTail
before reaching uxHead */
size_t uxHead = pxBuffer->uxHead;
size_t uxTail = pxBuffer->uxTail;

	return uxStreamBufferDistance( pxBuffer, uxTail, uxHead );
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxStreamBufferMidSpace( const StreamBuffer_t *pxBuffer );
static portINLINE size_t uxStreamBufferMidSpace( const StreamBuffer_t *pxBuffer )
{
/* Returns the distance between uxHead and uxMid */
size_t uxHead = pxBuffer->uxHead;
size_t uxMid = pxBuffer->uxMid;

	return uxStreamBufferDistance( pxBuffer, uxMid, uxHead );
}
/*-----------------------------------------------------------*/

static portINLINE void vStreamBufferMoveMid( StreamBuffer_t *pxBuffer, size_t uxCount );
static portINLINE void vStreamBufferMoveMid( StreamBuffer_t *pxBuffer, size_t uxCount )
{
/* Increment uxMid, but no further than uxHead */
size_t uxSize = uxStreamBufferMidSpace( pxBuffer );

	if( uxCount > uxSize )
	{
		uxCount = uxSize;
	}
	pxBuffer->uxMid += uxCount;
	if( pxBuffer->uxMid >= pxBuffer->LENGTH )
	{
		pxBuffer->uxMid -= pxBuffer->LENGTH;
	}
}
/*-----------------------------------------------------------*/
static portINLINE BaseType_t xStreamBufferIsEmpty( const StreamBuffer_t *pxBuffer );
static portINLINE BaseType_t xStreamBufferIsEmpty( const StreamBuffer_t *pxBuffer )
{
BaseType_t xReturn;

	/* True if no item is available */
	if( pxBuffer->uxHead == pxBuffer->uxTail )
	{
		xReturn = pdTRUE;
	}
	else
	{
		xReturn = pdFALSE;
	}
	return xReturn;
}
/*-----------------------------------------------------------*/

static portINLINE BaseType_t xStreamBufferIsFull( const StreamBuffer_t *pxBuffer );
static portINLINE BaseType_t xStreamBufferIsFull( const StreamBuffer_t *pxBuffer )
{
	/* True if the available space equals zero. */
	return ( BaseType_t ) ( uxStreamBufferGetSpace( pxBuffer ) == 0u );
}
/*-----------------------------------------------------------*/

static portINLINE BaseType_t xStreamBufferLessThenEqual( const StreamBuffer_t *pxBuffer, const size_t uxLeft, const size_t uxRight );
static portINLINE BaseType_t xStreamBufferLessThenEqual( const StreamBuffer_t *pxBuffer, const size_t uxLeft, const size_t uxRight )
{
BaseType_t xReturn;
size_t uxTail = pxBuffer->uxTail;

	/* Returns true if ( uxLeft < uxRight ) */
	if( ( uxLeft < uxTail ) ^ ( uxRight < uxTail ) )
	{
		if( uxRight < uxTail )
		{
			xReturn = pdTRUE;
		}
		else
		{
			xReturn = pdFALSE;
		}
	}
	else
	{
		if( uxLeft <= uxRight )
		{
			xReturn = pdTRUE;
		}
		else
		{
			xReturn = pdFALSE;
		}
	}
	return xReturn;
}
/*-----------------------------------------------------------*/

static portINLINE size_t uxStreamBufferGetPtr( StreamBuffer_t *pxBuffer, uint8_t **ppucData );
static portINLINE size_t uxStreamBufferGetPtr( StreamBuffer_t *pxBuffer, uint8_t **ppucData )
{
size_t uxNextTail = pxBuffer->uxTail;
size_t uxSize = uxStreamBufferGetSize( pxBuffer );

	*ppucData = pxBuffer->ucArray + uxNextTail;

	return FreeRTOS_min_uint32( uxSize, pxBuffer->LENGTH - uxNextTail );
}

/*
 * Add bytes to a stream buffer.
 *
 * pxBuffer -	The buffer to which the bytes will be added.
 * uxOffset -	If uxOffset > 0, data will be written at an offset from uxHead
 *				while uxHead will not be moved yet.
 * pucData -	A pointer to the data to be added.
 * uxCount -	The number of bytes to add.
 */
size_t uxStreamBufferAdd( StreamBuffer_t *pxBuffer, size_t uxOffset, const uint8_t *pucData, size_t uxCount );

/*
 * Read bytes from a stream buffer.
 *
 * pxBuffer -	The buffer from which the bytes will be read.
 * uxOffset -	Can be used to read data located at a certain offset from 'uxTail'.
 * pucData -	A pointer to the buffer into which data will be read.
 * uxMaxCount -	The number of bytes to read.
 * xPeek -		If set to pdTRUE the data will remain in the buffer.
 */
size_t uxStreamBufferGet( StreamBuffer_t *pxBuffer, size_t uxOffset, uint8_t *pucData, size_t uxMaxCount, BaseType_t xPeek );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif	/* !defined( FREERTOS_STREAM_BUFFER_H ) */
