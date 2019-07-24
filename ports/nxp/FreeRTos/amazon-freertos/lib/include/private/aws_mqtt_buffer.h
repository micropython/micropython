/*
 * Amazon FreeRTOS
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
 * @file aws_mqtt_buffer.h
 * @brief MQTT Buffer management.
 *
 * Each MQTT buffer is represented as MQTTBufferHandle_t which is opaque
 * to the user. Buffers are internally organized as circular doubly linked
 * list and the metadata needed for that is stored in the beginning of the
 * buffer along with the other metadata. This reduces the actual buffer
 * space available to store the data. More specifically, the first
 * sizeof( MQTTBufferMetadata_t ) bytes of each buffer are used to store
 * the metadata. Macros provided in this file should be used to convert
 * between raw buffer and MQTTBufferHandle_t and to access various fields.
 */

#ifndef _AWS_MQTT_BUFFER_H_
#define _AWS_MQTT_BUFFER_H_

/* Utils includes. */
#include "aws_doubly_linked_list.h"

/**
 * @brief Opaque handle to represent an MQTT buffer.
 */
typedef void * MQTTBufferHandle_t;

/**
 * @brief Structure containing the state of a transmitted buffer.
 *
 * Each buffer containing a transmitted MQTT message is put on a
 * waiting ACK list and is removed when it is acknowledged by the
 * broker or the operation times out. The packet identifier helps
 * in matching the received ACK with the transmitted message while
 * the recorded tick count and timeout ticks help in tracking timeout.
 */
typedef struct MQTTBufferState
{
    uint64_t xRecordedTickCount; /**< The time-stamp when this packet was sent. */
    uint32_t ulTimeoutTicks;     /**< The time interval after which this packet should timeout i.e. stop waiting for ACK. */
    uint16_t usPacketIdentifier; /**< Packet identifier sent with this packet. */
} MQTTBufferState_t;

/**
 * @brief Metadata stored in each MQTT buffer.
 *
 * The first sizeof( MQTTBufferMetadata_t ) bytes of each buffer are
 * used to store the metadata. This reduces the space available to store
 * the actual data in the buffer. The metadata among other things include
 * a link structure (of type Link_t) which helps in organizing the buffers
 * as a doubly linked list, length of the buffer and the length of the
 * data in the buffer.
 */
typedef struct MQTTBufferMetadata
{
    MQTTBufferState_t xBufferState; /**< State of the buffer. @see MQTTBufferState_t. */
    Link_t xLink;                   /**< Contains links to previous and next buffers in the list. */
    uint32_t ulBufferLength;        /**< The length of the buffer. */
    uint32_t ulDataLength;          /**< The length of the data in the buffer. */
} MQTTBufferMetadata_t;

/**
 * @brief Given the buffer handle, extracts the link structure from the
 * metadata portion of the buffer.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferGET_LINK( xBufferHandle )                          ( ( ( MQTTBufferMetadata_t * ) ( xBufferHandle ) )->xLink )

/**
 * @brief Given the pointer to a link structure, finds the buffer handle
 * containing the link.
 *
 * @param[in] pxLink The pointer to the link struct.
 */
#define mqttbufferGET_BUFFER_HANDLE_FROM_LINK( pxLink )              ( ( MQTTBufferHandle_t ) listCONTAINER( pxLink, MQTTBufferMetadata_t, xLink ) )

/**
 * @brief Given the buffer handle, finds the pointer to the starting of
 * the buffer.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferGET_RAW_BUFFER_FROM_HANDLE( xBufferHandle )        ( ( uint8_t * ) ( xBufferHandle ) )

/**
 * @brief Gets the buffer handle from the given uint8_t buffer.
 *
 * @param[in] pucBuffer The given uint8_t buffer.
 */
#define mqttbufferGET_HANDLE_FROM_RAW_BUFFER( pucBuffer )            ( ( MQTTBufferHandle_t ) ( pucBuffer ) )

/**
 * @brief Given the buffer handle, extracts the total length of the buffer
 * from the metadata portion of the buffer.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferGET_RAW_BUFFER_LENGTH( xBufferHandle )             ( ( ( MQTTBufferMetadata_t * ) ( xBufferHandle ) )->ulBufferLength )

/**
 * @brief Given the buffer handle, finds the length of the space available
 * to store the actual data.
 *
 * Effective Length = Total Buffer Length - Metadata Length.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBufferHandle )       ( ( ( ( MQTTBufferMetadata_t * ) ( xBufferHandle ) )->ulBufferLength ) - sizeof( MQTTBufferMetadata_t ) )

/**
 * @brief Given the buffer handle, finds the pointer to the location of the
 * actual data i.e. the location following the metadata.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferGET_DATA( xBufferHandle )                          ( ( uint8_t * ) ( ( ( uint8_t * ) ( xBufferHandle ) ) + sizeof( MQTTBufferMetadata_t ) ) )

/**
 * @brief Given the buffer handle, extracts the length of the data in the buffer
 * from the metadata portion of the buffer.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferGET_DATA_LENGTH( xBufferHandle )                   ( ( ( MQTTBufferMetadata_t * ) ( xBufferHandle ) )->ulDataLength )

/**
 * @brief Given the buffer handle, extracts the packet identifier from the metadata
 * portion of the buffer.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferGET_PACKET_IDENTIFIER( xBufferHandle )             ( ( ( MQTTBufferMetadata_t * ) ( xBufferHandle ) )->xBufferState.usPacketIdentifier )

/**
 * @brief Given the buffer handle, extracts the recorded tick count from the
 * metadata portion of the buffer.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferGET_PACKET_RECORDED_TICK_COUNT( xBufferHandle )    ( ( ( MQTTBufferMetadata_t * ) ( xBufferHandle ) )->xBufferState.xRecordedTickCount )

/**
 * @brief Given the buffer handle, extracts the timeout ticks from the metadata
 * portion of the buffer.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferGET_PACKET_TIMEOUT_TICKS( xBufferHandle )          ( ( ( MQTTBufferMetadata_t * ) ( xBufferHandle ) )->xBufferState.ulTimeoutTicks )

/**
 * @brief Given a list head and a buffer handle, adds the buffer to the given
 * list.
 *
 * @param[in] pxHead The head of the list to which the buffer is to be added.
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferLIST_ADD( pxHead, xBufferHandle )                  listADD( ( pxHead ), &( mqttbufferGET_LINK( xBufferHandle ) ) )

/**
 * @brief Given a buffer handle, removes it from the list it is part of. If it is
 * not a part of any list (i.e. next and previous nodes are NULL), nothing happens.
 *
 * @param[in] xBufferHandle The given buffer handle.
 */
#define mqttbufferLIST_REMOVE( xBufferHandle )                       listREMOVE( &( mqttbufferGET_LINK( xBufferHandle ) ) )

/**
 * @brief Given a pointer to a buffer and its length, initializes the metadata
 * portion of the buffer.
 *
 * @param[in] pucBuffer The given buffer.
 * @param[in] ulLength The length of the given buffer.
 */
#define mqttbufferINIT_BUFFER( pucBuffer, ulLength )                                 \
    {                                                                                \
        ( ( MQTTBufferMetadata_t * ) ( pucBuffer ) )->xLink.pxPrev = NULL;           \
        ( ( MQTTBufferMetadata_t * ) ( pucBuffer ) )->xLink.pxNext = NULL;           \
        ( ( MQTTBufferMetadata_t * ) ( pucBuffer ) )->ulBufferLength = ( ulLength ); \
        ( ( MQTTBufferMetadata_t * ) ( pucBuffer ) )->ulDataLength = 0;              \
    }

/**
 * @brief Removes the first node from the given list.
 *
 * It extracts the buffer handle corresponding to the removed node and assigns it to
 * xBufferHandle. If the list is empty, it assigns NULL to the xBufferHandle.
 *
 * @param[in] pxHead The head of the list from which to remove the node.
 * @param[out] xBufferHandle The output parameter to receive the buffer handle
 * corresponding to the removed node.
 */
#define mqttbufferLIST_POP( pxHead, xBufferHandle )                              \
    {                                                                            \
        Link_t * pxLink;                                                         \
        listPOP( ( pxHead ), pxLink );                                           \
        if( pxLink == NULL )                                                     \
        {                                                                        \
            ( xBufferHandle ) = NULL;                                            \
        }                                                                        \
        else                                                                     \
        {                                                                        \
            ( xBufferHandle ) = mqttbufferGET_BUFFER_HANDLE_FROM_LINK( pxLink ); \
        }                                                                        \
    }

#endif /* _AWS_MQTT_BUFFER_H_ */
