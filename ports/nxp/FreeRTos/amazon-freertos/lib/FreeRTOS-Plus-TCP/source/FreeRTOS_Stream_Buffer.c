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

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"

/*
 * uxStreamBufferAdd( )
 * Adds data to a stream buffer.  If uxOffset > 0, data will be written at
 * an offset from uxHead while uxHead will not be moved yet.  This possibility
 * will be used when TCP data is received while earlier data is still missing.
 * If 'pucData' equals NULL, the function is called to advance 'uxHead' only.
 */
size_t uxStreamBufferAdd( StreamBuffer_t *pxBuffer, size_t uxOffset, const uint8_t *pucData, size_t uxCount )
{
size_t uxSpace, uxNextHead, uxFirst;

	uxSpace = uxStreamBufferGetSpace( pxBuffer );

	/* If uxOffset > 0, items can be placed in front of uxHead */
	if( uxSpace > uxOffset )
	{
		uxSpace -= uxOffset;
	}
	else
	{
		uxSpace = 0u;
	}

	/* The number of bytes that can be written is the minimum of the number of
	bytes requested and the number available. */
	uxCount = FreeRTOS_min_uint32( uxSpace, uxCount );

	if( uxCount != 0u )
	{
		uxNextHead = pxBuffer->uxHead;

		if( uxOffset != 0u )
		{
			/* ( uxOffset > 0 ) means: write in front if the uxHead marker */
			uxNextHead += uxOffset;
			if( uxNextHead >= pxBuffer->LENGTH )
			{
				uxNextHead -= pxBuffer->LENGTH;
			}
		}

		if( pucData != NULL )
		{
			/* Calculate the number of bytes that can be added in the first
			write - which may be less than the total number of bytes that need
			to be added if the buffer will wrap back to the beginning. */
			uxFirst = FreeRTOS_min_uint32( pxBuffer->LENGTH - uxNextHead, uxCount );

			/* Write as many bytes as can be written in the first write. */
			memcpy( ( void* ) ( pxBuffer->ucArray + uxNextHead ), pucData, uxFirst );

			/* If the number of bytes written was less than the number that
			could be written in the first write... */
			if( uxCount > uxFirst )
			{
				/* ...then write the remaining bytes to the start of the
				buffer. */
				memcpy( ( void * )pxBuffer->ucArray, pucData + uxFirst, uxCount - uxFirst );
			}
		}

		if( uxOffset == 0u )
		{
			/* ( uxOffset == 0 ) means: write at uxHead position */
			uxNextHead += uxCount;
			if( uxNextHead >= pxBuffer->LENGTH )
			{
				uxNextHead -= pxBuffer->LENGTH;
			}
			pxBuffer->uxHead = uxNextHead;
		}

		if( xStreamBufferLessThenEqual( pxBuffer, pxBuffer->uxFront, uxNextHead ) != pdFALSE )
		{
			/* Advance the front pointer */
			pxBuffer->uxFront = uxNextHead;
		}
	}

	return uxCount;
}
/*-----------------------------------------------------------*/

/*
 * uxStreamBufferGet( )
 * 'uxOffset' can be used to read data located at a certain offset from 'lTail'.
 * If 'pucData' equals NULL, the function is called to advance 'lTail' only.
 * if 'xPeek' is pdTRUE, or if 'uxOffset' is non-zero, the 'lTail' pointer will
 * not be advanced.
 */
size_t uxStreamBufferGet( StreamBuffer_t *pxBuffer, size_t uxOffset, uint8_t *pucData, size_t uxMaxCount, BaseType_t xPeek )
{
size_t uxSize, uxCount, uxFirst, uxNextTail;

	/* How much data is available? */
	uxSize = uxStreamBufferGetSize( pxBuffer );

	if( uxSize > uxOffset )
	{
		uxSize -= uxOffset;
	}
	else
	{
		uxSize = 0u;
	}

	/* Use the minimum of the wanted bytes and the available bytes. */
	uxCount = FreeRTOS_min_uint32( uxSize, uxMaxCount );

	if( uxCount > 0u )
	{
		uxNextTail = pxBuffer->uxTail;

		if( uxOffset != 0u )
		{
			uxNextTail += uxOffset;
			if( uxNextTail >= pxBuffer->LENGTH )
			{
				uxNextTail -= pxBuffer->LENGTH;
			}
		}

		if( pucData != NULL )
		{
			/* Calculate the number of bytes that can be read - which may be
			less than the number wanted if the data wraps around to the start of
			the buffer. */
			uxFirst = FreeRTOS_min_uint32( pxBuffer->LENGTH - uxNextTail, uxCount );

			/* Obtain the number of bytes it is possible to obtain in the first
			read. */
			memcpy( pucData, pxBuffer->ucArray + uxNextTail, uxFirst );

			/* If the total number of wanted bytes is greater than the number
			that could be read in the first read... */
			if( uxCount > uxFirst )
			{
				/*...then read the remaining bytes from the start of the buffer. */
				memcpy( pucData + uxFirst, pxBuffer->ucArray, uxCount - uxFirst );
			}
		}

		if( ( xPeek == pdFALSE ) && ( uxOffset == 0UL ) )
		{
			/* Move the tail pointer to effecively remove the data read from
			the buffer. */
			uxNextTail += uxCount;

			if( uxNextTail >= pxBuffer->LENGTH )
			{
				uxNextTail -= pxBuffer->LENGTH;
			}

			pxBuffer->uxTail = uxNextTail;
		}
	}

	return uxCount;
}

