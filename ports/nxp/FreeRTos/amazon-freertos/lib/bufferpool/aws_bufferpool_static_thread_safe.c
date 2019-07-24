/*
 * Amazon FreeRTOS Buffer Pool V1.0.0
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


/**
 * @file aws_bufferpool_static_thread_safe.c
 * @brief A thread safe implementation of the BufferPool interface.
 *
 * A pool of statically allocated buffers is maintained. The number
 * of buffers in the pool and the size of each buffer is controlled
 * via macros bufferpoolconfigNUM_BUFFERS and bufferpoolconfigBUFFER_SIZE
 * which must be defined in BufferPoolConfig.h.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* BufferPool includes. */
#include "aws_bufferpool.h"
#include "aws_bufferpool_config.h"

/* Make sure that proper config options are defined. */
#ifndef bufferpoolconfigNUM_BUFFERS
    #error bufferpoolconfigNUM_BUFFERS must be defined in BufferPoolConfig.h
#endif

#ifndef bufferpoolconfigBUFFER_SIZE
    #error bufferpoolconfigBUFFER_SIZE must be defined in BufferPoolConfig.h
#endif

/**
 * @brief Moves the given pointer ahead by the number of bytes required to
 * properly align it as specified by portBYTE_ALIGNMENT.
 *
 * @param[in] pucPtr The given pointer to be aligned.
 */
#define bufferpoolstaticALIGN_POINTER( pucPtr )                                ( ( uint8_t * ) ( ( ( size_t ) ( pucPtr + ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( size_t ) portBYTE_ALIGNMENT_MASK ) ) )

/**
 * @brief Moves the given pointer ahead by the number of bytes required to
 * store the metadata.
 *
 * @param[in] pucPtr The given pointer to move.
 */
#define bufferpoolstaticRESERVE_METADATA_SPACE( pucPtr )                       ( ( uint8_t * ) ( pucPtr + sizeof( BufferMetadata_t ) ) )

/**
 * @brief Extracts the location of the user data in the given buffer. The data
 * location is ensured to be properly aligned as specified by portBYTE_ALIGNMENT.
 *
 * @param[in] pucBuffer The buffer for which to find the user data location.
 */
#define bufferpoolstaticDATA_LOCATION_IN_BUFFER( pucBuffer )                   ( ( uint8_t * ) ( bufferpoolstaticALIGN_POINTER( bufferpoolstaticRESERVE_METADATA_SPACE( pucBuffer ) ) ) )

/**
 * @brief Given the data location in a buffer, extracts the buffer "in-use"
 * information from the metadata portion of the buffer.
 *
 * @param[in] pucDataLocation The given data location in the buffer.
 */
#define bufferpoolstaticBUFFER_IN_USE_FROM_DATA_LOCATION( pucDataLocation )    ( ( ( BufferMetadata_t * ) ( ( pucDataLocation ) - sizeof( BufferMetadata_t ) ) )->ucBufferInUse )

/**
 * @brief Extracts the buffer "in-use" information from the metadata portion
 * of the given buffer.
 *
 * @param[in] pucBuffer The given buffer.
 */
#define bufferpoolstaticBUFFER_IN_USE( pucBuffer )                             bufferpoolstaticBUFFER_IN_USE_FROM_DATA_LOCATION( bufferpoolstaticDATA_LOCATION_IN_BUFFER( pucBuffer ) )
/*-----------------------------------------------------------*/

/**
 * @brief Metadata added in the beginning of each buffer.
 */
typedef struct BufferMetadata
{
    uint8_t ucBufferInUse; /**< Whether or not the buffer is in use. */
} BufferMetadata_t;
/*-----------------------------------------------------------*/

/**
 * @brief The pool of statically allocated buffers.
 *
 * The number of buffers in the pool and the size of each buffer is controlled
 * via macros bufferpoolconfigNUM_BUFFERS and bufferpoolconfigBUFFER_SIZE which
 * must be defined in BufferPoolConfig.h.
 *
 * @note Each buffer in the buffer pool allocates additional the space required
 * to store the metadata and to ensure alignment.
 */
static uint8_t ucBufferPool[ bufferpoolconfigNUM_BUFFERS ][ sizeof( BufferMetadata_t ) + bufferpoolconfigBUFFER_SIZE + ( portBYTE_ALIGNMENT - 1 ) ];
/*-----------------------------------------------------------*/

BaseType_t BUFFERPOOL_Init( void )
{
    BaseType_t x = 0;

    /* This function is supposed to be called exactly once
     * and hence no thread safety is ensured. */
    for( x = 0; x < bufferpoolconfigNUM_BUFFERS; x++ )
    {
        /* Mark all the buffers as free. */
        bufferpoolstaticBUFFER_IN_USE( ucBufferPool[ x ] ) = 0;
    }

    return pdPASS;
}
/*-----------------------------------------------------------*/

uint8_t * BUFFERPOOL_GetFreeBuffer( uint32_t * pulBufferLength )
{
    BaseType_t x = 0;
    uint8_t * pucFreeBuffer = NULL;

    /* All the buffers in the pool are of size bufferpoolconfigBUFFER_SIZE,
     * so we cannot provide any buffer larger than that. */
    if( *pulBufferLength <= bufferpoolconfigBUFFER_SIZE )
    {
        /* Iterate over all the buffers to find a free buffer. */
        for( x = 0; x < bufferpoolconfigNUM_BUFFERS; x++ )
        {
            /* Start critical section. */
            taskENTER_CRITICAL();

            /* Check if the buffer is free. */
            if( bufferpoolstaticBUFFER_IN_USE( ucBufferPool[ x ] ) == 0 )
            {
                /* Mark the buffer as "in-use". */
                bufferpoolstaticBUFFER_IN_USE( ucBufferPool[ x ] ) = 1;

                /* End critical section. The further operations in this
                 * if branch do not modify the buffer and hence the critical
                 * section is not needed hereafter. */
                taskEXIT_CRITICAL();

                /* Return the actual buffer size (as configured by the
                 * bufferpoolconfigBUFFER_SIZE macro) to the user. */
                *pulBufferLength = bufferpoolconfigBUFFER_SIZE;

                /* Return the data location to the user. */
                pucFreeBuffer = bufferpoolstaticDATA_LOCATION_IN_BUFFER( ucBufferPool[ x ] );

                /* Stop as we have found a buffer. */
                break;
            }
            else
            {
                /* End critical section. */
                taskEXIT_CRITICAL();
            }
        }
    }

    return pucFreeBuffer;
}
/*-----------------------------------------------------------*/

void BUFFERPOOL_ReturnBuffer( uint8_t * const pucBuffer )
{
    /* Start critical section. */
    taskENTER_CRITICAL();

    /* Mark the buffer as free. The returned buffer is the data
     * location in the actual buffer (because we gave the data location
     * to the user). */
    bufferpoolstaticBUFFER_IN_USE_FROM_DATA_LOCATION( pucBuffer ) = 0;

    /* End critical section. */
    taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/
