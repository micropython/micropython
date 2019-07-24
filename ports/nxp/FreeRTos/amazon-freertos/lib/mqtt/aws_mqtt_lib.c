/*
 * Amazon FreeRTOS MQTT Library V1.1.3
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
 * @file aws_mqtt_lib.c
 * @brief MQTT Core Library implementation.
 */

/* Interface includes. */
#include "aws_mqtt_lib.h"

/* Standard includes. */
#include <string.h>

/**
 * @defgroup ControlTypes MQTT Control Packet Type.
 *
 * Values that appear in the first nibble (hence the shift by 4)
 * of the byte 1 of the MQTT fixed header.
 */
/** @{ */
#define mqttCONTROL_CONNECT        ( ( uint8_t ) 1 << ( uint8_t ) 4 )
#define mqttCONTROL_CONNACK        ( ( uint8_t ) 2 << ( uint8_t ) 4 )
#define mqttCONTROL_PUBLISH        ( ( uint8_t ) 3 << ( uint8_t ) 4 )
#define mqttCONTROL_PUBACK         ( ( uint8_t ) 4 << ( uint8_t ) 4 )
#define mqttCONTROL_PUBREC         ( ( uint8_t ) 5 << ( uint8_t ) 4 )
#define mqttCONTROL_PUBREL         ( ( uint8_t ) 6 << ( uint8_t ) 4 )
#define mqttCONTROL_PUBCOMP        ( ( uint8_t ) 7 << ( uint8_t ) 4 )
#define mqttCONTROL_SUBSCRIBE      ( ( uint8_t ) 8 << ( uint8_t ) 4 )
#define mqttCONTROL_SUBACK         ( ( uint8_t ) 9 << ( uint8_t ) 4 )
#define mqttCONTROL_UNSUBSCRIBE    ( ( uint8_t ) 10 << ( uint8_t ) 4 )
#define mqttCONTROL_UNSUBACK       ( ( uint8_t ) 11 << ( uint8_t ) 4 )
#define mqttCONTROL_PINGREQ        ( ( uint8_t ) 12 << ( uint8_t ) 4 )
#define mqttCONTROL_PINGRESP       ( ( uint8_t ) 13 << ( uint8_t ) 4 )
#define mqttCONTROL_DISCONNECT     ( ( uint8_t ) 14 << ( uint8_t ) 4 )
/** @} */

/**
 * @defgroup Flags MQTT Control Packet Flags.
 *
 * Lower nibble in the byte 1 of the fixed header contain flags specific
 * to each MQTT Control Packet. Flags marked reserved must match the
 * value in the packet.
 */
/** @{ */
#define mqttFLAGS_CONNECT           ( ( uint8_t ) 0 )   /**< Reserved. */
#define mqttFLAGS_CONNACK           ( ( uint8_t ) 0 )   /**< Reserved. */
#define mqttFLAGS_PUBLISH_DUP       ( ( uint8_t ) 8 )
#define mqttFLAGS_PUBLISH_QOS       ( ( uint8_t ) 6 )
#define mqttFLAGS_PUBLISH_RETAIN    ( ( uint8_t ) 1 )
#define mqttFLAGS_PUBACK            ( ( uint8_t ) 0 )   /**< Reserved. */
#define mqttFLAGS_PUBREC            ( ( uint8_t ) 0 )   /**< Reserved. */
#define mqttFLAGS_PUBREL            ( ( uint8_t ) 2 )   /**< Reserved. */
#define mqttFLAGS_PUBCOMP           ( ( uint8_t ) 0 )   /**< Reserved. */
#define mqttFLAGS_SUBSCRIBE         ( ( uint8_t ) 2 )   /**< Reserved. */
#define mqttFLAGS_SUBACK            ( ( uint8_t ) 0 )   /**< Reserved. */
#define mqttFLAGS_UNSUBSCRIBE       ( ( uint8_t ) 2 )   /**< Reserved. */
#define mqttFLAGS_UNSUBACK          ( ( uint8_t ) 0 )   /**< Reserved. */
#define mqttFLAGS_PINGREQ           ( ( uint8_t ) 0 )   /**< Reserved. */
#define mqttFLAGS_PINGRESP          ( ( uint8_t ) 0 )   /**< Reserved. */
#define mqttFLAGS_DISCONNECT        ( ( uint8_t ) 0 )   /**< Reserved. */
/** @} */

/**
 * @defgroup ConnectFlags Connect flags present in the variable header of MQTT
 * connect message.
 *
 * These flags specify the behavior of the MQTT connection. They also indicate
 * the presence or absence of certain fields in the payload.
 */
/** @{ */
#define mqttCONNECT_CLEAN_SESSION_FLAG    ( ( uint8_t ) ( ( uint8_t ) 1 << ( uint8_t ) 1 ) )
#define mqttCONNECT_WILL_FLAG             ( ( uint8_t ) ( ( uint8_t ) 1 << ( uint8_t ) 2 ) )
#define mqttCONNECT_WILL_RETAIN_FLAG      ( ( uint8_t ) ( ( uint8_t ) 1 << ( uint8_t ) 5 ) )
#define mqttCONNECT_PASSWORD_FLAG         ( ( uint8_t ) ( ( uint8_t ) 1 << ( uint8_t ) 6 ) )
#define mqttCONNECT_USER_NAME_FLAG        ( ( uint8_t ) ( ( uint8_t ) 1 << ( uint8_t ) 7 ) )
/** @} */

/**
 * @brief Protocol level value is 4 for version 3.1.1.
 */
#define mqttPROTOCOL_LEVEL    ( ( uint8_t ) 0x04 )

/**
 * @brief Macro to calculate total message length.
 *
 * @param[in] ucRemainingLengthFieldBytes Number of bytes the "Remaining Length"
 * field spans.
 * @param[in] ulRemainingLength "Remaining Length" i.e. length of the MQTT
 * packet excluding the length of fixed header.
 */
#define mqttTOTAL_MESSAGE_LENGTH( ucRemainingLengthFieldBytes, ulRemainingLength ) \
    ( ( uint32_t ) 1 /* Control Packet Type + Flags. */ +                          \
      ( uint32_t ) ucRemainingLengthFieldBytes +                                   \
      ulRemainingLength )

/**
 * @defgroup FieldLength Lengths (in bytes) of various packet specific fields.
 *
 * Helpful in calculating total packet length.
 */
/** @{ */
#define mqttSUBSCRIBE_PACKET_IDENTIFER_LENGTH       2
#define mqttSUBSCRIBE_REQUESTED_QOS_LENGTH          1
#define mqttUNSUBSCRIBE_PACKET_IDENTIFER_LENGTH     2
#define mqttUNSUBACK_PACKET_IDENTIFER_LENGTH        2
#define mqttPUBLISH_QOS0_PACKET_IDENTIFER_LENGTH    0 /**< QoS0 Publish packet does not contain a packet identifier. */
#define mqttPUBLISH_QOS1_PACKET_IDENTIFER_LENGTH    2
#define mqttPUBLISH_QOS2_PACKET_IDENTIFER_LENGTH    2
#define mqttPUBACK_PACKET_IDENTIFER_LENGTH          2
/** @} */

/**
 * @defgroup RemainingLength Values to help encode and decode the "Remaining Length"
 * field in the MQTT fixed header.
 *
 * "Remaining Length" field starts at second byte and can comprise of 1 to 4 bytes.
 */
/** @{ */
#define mqttREMAINING_LENGTH_CONTINUATION_BITMASK    ( ( uint8_t ) ( ( uint8_t ) 1 << ( uint8_t ) 7 ) )   /**< Bit 7 in an encoded byte represent whether or not it is the last byte containing length. */
#define mqttLENGTH_BITMASK_REMAINING_LENGTH          ( ~mqttREMAINING_LENGTH_CONTINUATION_BITMASK )       /**< Lower 7 bits in an encoded byte represent length. */
#define mqttLENGTH_BITS_REMAINING_LENGTH             7                                                    /**< Lower 7 bits in an encoded byte represent length. */
#define mqttONE_BYTE_MAX_REMAINING_LENGTH            127                                                  /**< Maximum value that can be encoded in one byte. */
#define mqttTWO_BYTES_MAX_REMAINING_LENGTH           16383                                                /**< Maximum value that can be encoded in two bytes. */
#define mqttTHREE_BYTES_MAX_REMAINING_LENGTH         2097151                                              /**< Maximum value that can be encoded in three bytes. */
#define mqttMAX_REMAINING_LENGTH_LENGTH              268435455                                            /**< Maximum value that can be encoded. */
/** @} */

/**
 * @brief Helper macro to convert the given string length to the length occupied by it
 * in the MQTT packet.
 *
 * Strings in MQTT protocol are prefixed with a two byte length field which gives the
 * number of bytes in the string. This macro converts the given string length to the length
 * as occupied by it in the actual MQTT packet.
 *
 * @param[in] usStringLength The given string length.
 */
#define mqttSTRLEN( usStringLength )                                ( ( usStringLength ) + ( uint16_t ) 2 )

/**
 * @brief Helper macro to adjust various offset according to the number of bytes the
 * "Remaining Length" in the fixed header spans.
 *
 * Since "Remaining Length" field can comprise of variable number of bytes ( 1 to 4 ),
 * all the offsets after the "Remaining Length" will vary depending on the number of
 * bytes taken by this field. Hence all the offsets after "Remaining Length" should
 * use the mqttADJUST_OFFSET macro like:
 * @code
 * mqttADJUST_OFFSET( mqttVARIABLE_LENGTH_HEADER_START_OFFSET, ucRemainingLengthFieldBytes )
 * mqttADJUST_OFFSET( mqttCONNECT_CLIENT_ID_OFFSET, ucRemainingLengthFieldBytes )
 * @endcode
 * where ucRemainingLengthFieldBytes is the number of bytes, the encoded "Remaining Length"
 * field is comprised of.
 *
 * @param[in] offset The offset to adjust.
 * @param[in] ucRemainingLengthFieldBytes Number of bytes the "Remaining Length" field spans.
 */
#define mqttADJUST_OFFSET( offset, ucRemainingLengthFieldBytes )    ( ( uint32_t ) offset + ( uint32_t ) ucRemainingLengthFieldBytes - ( uint32_t ) 1 )

/**
 * @defgroup FixhedHeaderOffsets Offsets to data within the fixed header.
 */
/** @{ */
#define mqttFIXED_HEADER_CONTROL_BYTE_OFFSET        0   /* Always fixed - must not use mqttADJUST_OFFSET. */
#define mqttFIXED_HEADER_REMAINING_LENGTH_OFFSET    1   /* Always fixed - must not use mqttADJUST_OFFSET. */
#define mqttVARIABLE_LENGTH_HEADER_START_OFFSET     2
/** @} */

/**
 * @defgroup ConnectOffsets Offsets to data within the CONNECT packet
 * variable header.
 */
/** @{ */
#define mqttCONNECT_FLAGS_OFFSET            9
#define mqttCONNECT_KEEPALIVE_MSB_OFFSET    10
#define mqttCONNECT_KEEPALIVE_LSB_OFFSET    11
#define mqttCONNECT_CLIENT_ID_OFFSET        12
/** @} */

/**
 * @defgroup PublishOffsets Offsets to data within the PUBLISH packet
 * variable header.
 */
/** @{ */
#define mqttPUBLISH_TOPIC_OFFSET           2
#define mqttPUBLISH_TOPIC_LENGTH_MSB       2
#define mqttPUBLISH_TOPIC_LENGTH_LSB       3
#define mqttPUBLISH_TOPIC_STRING_OFFSET    4
/** @} */

/**
 * @defgroup SubscribeOffsets Offsets to data within the SUBSCRIBE packet
 * variable header.
 */
/** @{ */
#define mqttSUBSCRIBE_PACKET_ID_MSB_OFFSET    2
#define mqttSUBSCRIBE_PACKET_ID_LSB_OFFSET    3
#define mqttSUBSCRIBE_TOPIC_OFFSET            4
/** @} */

/**
 * @defgroup UnsubscribeOffsets Offsets to data within the UNSUBSCRIBE packet
 * variable header.
 */
/** @{ */
#define mqttUNSUBSCRIBE_PACKET_ID_MSB_OFFSET    2
#define mqttUNSUBSCRIBE_PACKET_ID_LSB_OFFSET    3
#define mqttUNSUBSCRIBE_TOPIC_OFFSET            4
/** @} */

/**
 * @defgroup SubAckOffsets Offsets to data within the SUBACK packet.
 */
/** @{ */
#define mqttSUBACK_PACKET_ID_MSB_OFFSET    2
#define mqttSUBACK_PACKET_ID_LSB_OFFSET    3
#define mqttSUBACK_RETURN_CODE_OFFSET      4
/** @} */

/**
 * @defgroup UnSubAckOffsets Offsets to data within the UNSUBACK packet.
 */
/** @{ */
#define mqttUNSUBACK_PACKET_ID_MSB_OFFSET    2
#define mqttUNSUBACK_PACKET_ID_LSB_OFFSET    3
/** @} */

/**
 * @defgroup ConnAckOffsets Offsets to data within the CONNACK packet.
 */
/** @{ */
#define mqttCONNACK_SESSION_PRESENT_OFFSET    2
#define mqttCONNACK_RETURN_CODE_OFFSET        3
/** @} */

/**
 * @defgroup PubAckOffsets Offsets to data within the PUBACK packet.
 */
/** @{ */
#define mqttPUBACK_PACKET_ID_MSB_OFFSET    2
#define mqttPUBACK_PACKET_ID_LSB_OFFSET    3
/** @} */

/**
 * @brief Extracts retain bit from the control byte of the PUBLISH message
 * received from the broker.
 *
 * @param[in] x The control byte of the received publish message.
 */
#define mqttPUBLISH_RETAIN_BIT( x )    ( x & ( uint8_t ) 0x01 )

/**
 * @brief Extracts QoS value from the control byte of the PUBLISH message
 * received from the broker.
 *
 * @param[in] x The control byte of the received publish message.
 */
#define mqttPUBLISH_QoS_BITS( x )      ( ( x >> ( uint8_t ) 1 ) & ( uint8_t ) 0x03 )

/**
 * @brief Extracts dup bit from the control byte of the PUBLISH message
 * received from the broker.
 *
 * @param[in] x The control byte of the received publish message.
 */
#define mqttPUBLISH_DUP_BIT( x )       ( ( x >> ( uint8_t ) 3 ) & ( uint8_t ) 0x01 )

/**
 * @defgroup Misc. Helper macros.
 */
/** @{ */
#define mqttBITS_PER_BYTE        8
#define mqttTOP_NIBBLE_MASK      ( ( uint8_t ) 0xF0 )
#define mqttLOWER_NIBBLE_MASK    ( ( uint8_t ) 0x0F )
/** @} */

/**
 * @brief Returns minimum of the two given values.
 *
 * @param[in] A The first value to compare.
 * @param[in] B The second value to compare.
 */
#define mqttMIN( A, B )    ( ( A ) < ( B ) ? ( A ) : ( B ) )

/**
 * @brief Copies the given number of bytes from the source buffer to the
 * destination buffer.
 *
 * Also, increments the srcIndex and dstIndex by the number of bytes copied.
 *
 * @param[in] srcBuffer The source buffer to copy from.
 * @param[in,out] srcIndex The index in the source buffer from where to start
 * copying from. It is incremented by the number of bytes copied.
 * @param[out] dstBuffer The destination buffer to copy to.
 * @param[in,out] dstIndex The index in the destination buffer where to start
 * copying to. It is incremented by the number of bytes copied.
 * @param[in] byteCount The number of bytes to copy.
 */
#define mqttCOPY_BYTES( srcBuffer, srcIndex, dstBuffer, dstIndex, byteCount )                            \
    {                                                                                                    \
        memcpy( &( ( dstBuffer )[ ( dstIndex ) ] ), &( ( srcBuffer )[ ( srcIndex ) ] ), ( byteCount ) ); \
        ( srcIndex ) = ( uint32_t ) ( srcIndex ) + ( uint32_t ) ( byteCount );                           \
        ( dstIndex ) = ( uint32_t ) ( dstIndex ) + ( uint32_t ) ( byteCount );                           \
    }
/*-----------------------------------------------------------*/

/**
 * @brief Takes a buffer of the desired length from the free buffer
 * pool using the user supplied buffer pool interface and returns it.
 *
 * The actual requested buffer length from the buffer pool is
 * ulBufferLength + sizeof( MQTTBufferMetadata_t ) to ensure
 * the space for storing the metadata. It also initializes the
 * metadata portion of the buffer.
 *
 * @param[in] pxMQTTContext The MQTT context for which to get the buffer.
 * @param[in] ulBufferLength The desired length of the buffer.
 *
 * @return Handle to a free buffer if one is available, NULL otherwise.
 */
static MQTTBufferHandle_t prvGetFreeBuffer( MQTTContext_t * pxMQTTContext,
                                            uint32_t ulBufferLength );

/**
 * @brief Returns the given buffer back to the free buffer pool.
 *
 * Removes the buffer from the Tx buffer list if it is part of that and returns
 * it back to the free buffer pool using the user supplied buffer pool interface.
 *
 * @param[in] pxMQTTContext The MQTT context to which to return the buffer.
 * @param[in] xBuffer The buffer to return.
 */
static void prvReturnBuffer( MQTTContext_t * pxMQTTContext,
                             MQTTBufferHandle_t xBuffer );

/**
 * @brief Finds a Tx buffer containing the MQTT message matching the given packet
 * type and flags.
 *
 * Iterates over all the buffers in the Tx buffer list to see if any buffer
 * contains a message in which:
 * 1. Top nibble of first byte matches the given packet type.
 * 2. Lower nibble of the first byte matches the given flags.
 *
 * @param[in] pxMQTTContext The MQTT context for which to find the buffer.
 * @param[in] ucPacketType The packet type to match.
 * @param[in] ucFlags The flags to match.
 *
 * @return The handle to the buffer if it finds one, NULL otherwise.
 */
static MQTTBufferHandle_t prvPacketTypeFlagsGetTxBuffer( MQTTContext_t * pxMQTTContext,
                                                         uint8_t ucPacketType,
                                                         uint8_t ucFlags );

/**
 * @brief Finds a Tx buffer containing the MQTT message matching the given packet
 * type, flags and identifier.
 *
 * Iterates over all the buffers in the Tx buffer list to see if any buffer contains
 * a message in which:
 * 1. Top nibble of first byte matches the given packet type.
 * 2. Lower nibble of the first byte matches the given flags.
 * 3. The packet identifier in the buffer matches the given one.
 *
 * @param[in] pxMQTTContext The MQTT context for which to find the buffer.
 * @param[in] ucPacketType The packet type to match.
 * @param[in] ucFlags The flags to match.
 * @param[in] usPacketIdentifier The packet identifier to match.
 *
 * @return The handle to the buffer if it finds one, NULL otherwise.
 */
static MQTTBufferHandle_t prvPacketTypeFlagsIdentifierGetTxBuffer( MQTTContext_t * pxMQTTContext,
                                                                   uint8_t ucPacketType,
                                                                   uint8_t ucFlags,
                                                                   uint16_t usPacketIdentifier );

/**
 * @brief Finds a Tx buffer containing the MQTT message matching the given packet
 * type and identifier.
 *
 * Iterates over all the buffers in the Tx buffer list to see if any buffer contains
 * a message in which:
 * 1. Top nibble of first byte matches the given packet type.
 * 2. The packet identifier in the buffer matches the given one.
 *
 * @param[in] pxMQTTContext The MQTT context for which to find the buffer.
 * @param[in] ucPacketType The packet type to match.
 * @param[in] usPacketIdentifier The packet identifier to match.
 *
 * @return The handle to the buffer if it finds one, NULL otherwise.
 */
static MQTTBufferHandle_t prvPacketTypeIdentifierGetTxBuffer( MQTTContext_t * pxMQTTContext,
                                                              uint8_t ucPacketType,
                                                              uint16_t usPacketIdentifier );

/**
 * @brief Resets the Rx message state of the context to prepare it for
 * receiving the next message.
 *
 * @param[in] pxMQTTContext The MQTT context to reset the Rx message state.
 */
static void prvResetRxMessageState( MQTTContext_t * pxMQTTContext );

/**
 * @brief Resets the MQTT contexts and puts in "not connected" state.
 *
 * All the buffers on the Tx buffer list and the Rx buffer are returned
 * to the free buffer pool. Rx message state is reset and connection state
 * is marked as "not connected". All the subscription entires in the
 * subscription manager are marked as free.
 *
 * @param[in] pxMQTTContext The MQTT context to reset.
 */
static void prvResetMQTTContext( MQTTContext_t * pxMQTTContext );

/**
 * @brief Gets the current tick count.
 *
 * If the user has supplied GetTicks function, it invokes it to get the
 * current tick count. If not, it returns zero to indicate that the tick
 * count should be stored on the next invocation of MQTT_Periodic.
 *
 * @param[in] pxMQTTContext The MQTT context.
 *
 * @return The current tick count if the user has supplied GetTicks function,
 * zero otherwise.
 */
static uint64_t prvGetCurrentTickCount( MQTTContext_t * pxMQTTContext );

/**
 * @brief Compares the recorded tick count with the current tick count to see
 * if the remaining ticks have elapsed.
 *
 * If the recorded tick count is zero, it indicates that the user has not supplied
 * the get ticks function and we should update the recorded the tick count to the
 * current tick count. This way the timeout processing starts from this invocation
 * of MQTT_Periodic. If the recorded tick count is not zero, it compares the current
 * tick count with the recorded tick count to see if the remaining ticks have elapsed.
 * If remaining ticks have elapsed, it returns eMQTTTrue to indicate timeout otherwise
 * it returns eMQTTFalse and updates the remaining ticks & recorded tick count.
 *
 * @param[in,out] pxRecordedTickCount The recorded tick count.
 * @param[in] xCurrentTickCount The current tick count.
 * @param[in,out] pulRemainingTicks Remaining ticks.
 *
 * @return eMQTTTrue if the time has elapsed, eMQTTFalse otherwise.
 */
static MQTTBool_t prvIsTimeElapsed( uint64_t * pxRecordedTickCount,
                                    uint64_t xCurrentTickCount,
                                    uint32_t * pulRemainingTicks );

/**
 * @brief Transmits the data using the user supplied callback.
 *
 * It also updates the xLastSentMessageTimestamp and ulNextPeriodicInvokeTicks
 * in the MQTT context in case of a successful transmit to ensure that
 * keep alive messages are sent when needed.
 *
 * @param[in] pxMQTTContext The MQTT context.
 * @param[in] pucData The data to transmit.
 * @param[in] ulDataLength Length of the data.
 *
 * @return eMQTTSuccess if send is successful, eMQTTSendFailed otherwise.
 */
static MQTTReturnCode_t prvSendData( MQTTContext_t * pxMQTTContext,
                                     const uint8_t * const pucData,
                                     uint32_t ulDataLength );

/**
 * @brief Decodes and processes the received MQTT message containing only fixed header.
 *
 * We always write the fixed header of the received message in a separate
 * fixed header buffer. Then we decode the remaining length and try to get
 * a free buffer. If a free buffer is not available or the buffer is not large
 * enough to hold the complete message, we drop the packet (we can do so because
 * we already know the packet length and therefore we know how many next bytes to
 * drop). If we get a large enough free buffer, we copy the fixed header in that
 * buffer and the rest of the packet is also stored in that buffer.
 * Certain messages (like PINGRESP) only contain fixed header (i.e. remaining length
 * is zero) and can be parsed as soon as the fixed header is received. This function
 * is invoked whenever a message containing only fixed header is received (for other
 * messages prvProcessReceivedMQTTPacket is invoked). The message is contained in
 * pxMQTTContext->ucRxFixedHeaderBuffer.
 *
 * @param[in] pxMQTTContext The MQTT context for which the message was received.
 */
static void prvProcessReceivedFixedHeaderOnlyMQTTPacket( MQTTContext_t * pxMQTTContext );

/**
 * @brief Decodes and processes the received MQTT message.
 *
 * This is invoked whenever a complete MQTT message is received. Depending on
 * the received message it may invoke the user supplied callback to inform
 * about the received message. The message is contained in pxMQTTContext->xRxBuffer.
 *
 * @param[in] pxMQTTContext The MQTT context for which the message was received.
 */
static void prvProcessReceivedMQTTPacket( MQTTContext_t * pxMQTTContext );

/**
 * @brief Decodes and processes the received CONNACK message.
 *
 * It tries to find out if this is a valid and expected CONNACK i.e. a Connect
 * message was sent before and has not timed out yet. It invokes the user supplied
 * callback to inform about the received message.
 *
 * @param[in] pxMQTTContext The MQTT context for which the message was received.
 */
static void prvProcessReceivedCONNACK( MQTTContext_t * pxMQTTContext );

/**
 * @brief Decodes and processes the received SUBACK message.
 *
 * It tries to find out if this is a valid and expected SUBACK i.e. a Subscribe
 * message was sent before and has not timed out yet. It invokes the user supplied
 * callback to inform about the received message.
 *
 * @param[in] pxMQTTContext The MQTT context for which the message was received.
 */
static void prvProcessReceivedSUBACK( MQTTContext_t * pxMQTTContext );

/**
 * @brief Decodes and processes the received UNSUBACK message.
 *
 * It tries to find out if this is a valid and expected UNSUBACK i.e. an Unsubscribe
 * message was sent before and has not timed out yet. It invokes the user supplied
 * callback to inform about the received message.
 *
 * @param[in] pxMQTTContext The MQTT context for which the message was received.
 */
static void prvProcessReceivedUNSUBACK( MQTTContext_t * pxMQTTContext );

/**
 * @brief Decodes and processes the received PUBACK message.
 *
 * It tries to find out if this is a valid and expected PUBACK i.e. a Publish
 * message was sent before and has not timed out yet. It invokes the user supplied
 * callback to inform about the received message.
 *
 * @param[in] pxMQTTContext The MQTT context for which the message was received.
 */
static void prvProcessReceivedPUBACK( MQTTContext_t * pxMQTTContext );

/**
 * @brief Decodes and processes the received PINGRESP message.
 *
 * It tries to find out if this is a valid PINGRESP message.
 *
 * @param[in] pxMQTTContext The MQTT context for which the message was received.
 */
static void prvProcessReceivedPINGRESP( MQTTContext_t * pxMQTTContext );

/**
 * @brief Decodes and processes the received Publish message.
 *
 * Parses the message and invokes the user supplied callback to inform about
 * the publish message received from the broker. The buffer containing the
 * whole MQTT message is supplied in the callback (xBuffer member of the
 * MQTTPublishData_t). The user can choose to own the buffer afterwards by
 * returning eMQTTTrue from the callback in which case the user should
 * free the buffer whenever done or supply it back for re-use by calling
 * MQTT_GiveBuffer.
 *
 * @param[in] pxMQTTContext The MQTT context for which the message was received.
 */
static void prvProcessReceivedPublish( MQTTContext_t * pxMQTTContext );

/**
 * @brief Invokes the user supplied callback.
 *
 * If the event is eMQTTPublish, it tries to invoke the subscription callback.
 * If the user has not registered any specific callback for this topic or
 * subscription management is not enabled, it invokes the user supplied generic
 * callback. For all events other than eMQTTPublish, it just invokes the user
 * supplied generic callback. If user has not supplied the generic callback,
 * it does nothing.
 *
 * @param[in] pxMQTTContext The MQTT context to invoke the callback for.
 * @param[in] pxEventCallbackParams Parameters to pass in the callback.
 *
 * @return returned value from the callback, if one was invoked, eMQTTFalse
 * otherwise.
 */
static MQTTBool_t prvInvokeCallback( MQTTContext_t * pxMQTTContext,
                                     MQTTEventCallbackParams_t * pxEventCallbackParams );

/**
 * @brief Writes a string in the expected MQTT format which means prefixed with
 * the string's length.
 *
 * First writes the string length and then copies the supplied string into the
 * destination buffer. It uses the pointer to the last byte in buffer to avoid
 * overflow. If it determines that the destination buffer does not have enough
 * space, it writes nothing in it and returns the pointer to the starting of
 * destination buffer to indicate that nothing has been written to the buffer.
 * In case of a successful write, it returns the address of the byte following
 * the string just copied to the buffer.
 *
 * @param[out] pucDestination The destination buffer.
 * @param[in] pucLastByteInBuffer Pointer to the last byte in the destination buffer.
 * @param[in] pucString The string to copy.
 * @param[in] usStringLength The length of the string.
 *
 * @return The address of the byte following the string just copied to the buffer
 * if write is successful, pucDestination otherwise.
 */
static uint8_t * prvWriteString( uint8_t * pucDestination,
                                 const uint8_t * const pucLastByteInBuffer,
                                 const uint8_t * const pucString,
                                 uint16_t usStringLength );

/**
 * @brief Returns the number of bytes required to encode the given "Remaining Length"
 * using the variable length encoding scheme documented by the MQTT protocol spec.
 *
 * @param[in] ulRemainingLength The given remaining length.
 *
 * @return The number of bytes required to encode the given remaining length. If the
 * given remaining length is not within the permissible limits as documented by the
 * MQTT protocol spec, it returns 0 to indicate failure.
 */
static uint8_t prvSizeOfRemainingLength( uint32_t ulRemainingLength );

/**
 * @brief Encodes the given remaining length using the variable length encoding
 * scheme documented by the MQTT protocol spec.
 *
 * Encoded length is returned in the provided buffer which must be large enough
 * to hold the result. If it fails to encode the remaining length (which can happen
 * if the length is more than the maximum that can be encoded in four bytes or if
 * the provided buffer does not have enough space), it returns zero to indicate that
 * nothing was written to the buffer otherwise it returns the number of bytes
 * written to the buffer. The pointer to the last byte in buffer is used to check
 * for overflow.
 *
 * @param[in] ulRemainingLength The remaining length to encode.
 * @param[out] pucEncodedRemainingLength The buffer to write the encoded length in.
 * @param[in] pucLastByteInBuffer Pointer to the last byte in the provided buffer.
 *
 * @return The number of bytes written to the buffer.
 */
static uint8_t prvEncodeRemainingLength( uint32_t ulRemainingLength,
                                         uint8_t * const pucEncodedRemainingLength,
                                         const uint8_t * const pucLastByteInBuffer );

/**
 * @brief Decodes the given remaining length using the variable length encoding
 * scheme documented by the MQTT protocol spec.
 *
 * If the user has provided an output buffer, decoded length is returned in it. The
 * function returns the number of bytes read from the provided buffer. If the buffer
 * does not contain a valid encoded length, it returns zero to indicate a decoding
 * failure.
 *
 * @param[in] pucEncodedRemainingLength The buffer containing the encoded length.
 * @param[out] pulRemainingLength Used to return the decoded length. If the user
 * is not interested in the decoded length, this should be passed as NULL.
 *
 * @return Number of bytes read from the buffer.
 */
static uint8_t prvDecodeRemainingLength( const uint8_t * const pucEncodedRemainingLength,
                                         uint32_t * const pulRemainingLength );

/**
 * @brief Store the subscription in the subscription manager.
 *
 * This function can fail to store the subscription if all the entries in the
 * subscription manager are in use or the topic name is longer than the maximum
 * length as specified by the mqttconfigSUBSCRIPTION_MANAGER_MAX_TOPIC_LENGTH
 * macro or if the topic represents an invalid topic filter. eMQTTFalse is returned
 * to indicate the failure.
 *
 * @param[in] pxMQTTContext The MQTT context for which to store the subscription.
 * @param[in] pucTopic The topic this subscription entry is for.
 * @param[in] usTopicLength The length of the topic.
 * @param[in] pvPublishCallbackContext The user supplied callback context.
 * @param[in] pxPublishCallback The callback to invoke whenever a publish message
 * is received on this topic.
 *
 * @return eMQTTTrue if subscription is stored successfully, eMQTTFalse otherwise.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static MQTTBool_t prvStoreSubscription( MQTTContext_t * pxMQTTContext,
                                            const uint8_t * const pucTopic,
                                            uint16_t usTopicLength,
                                            void * pvPublishCallbackContext,
                                            MQTTPublishCallback_t pxPublishCallback );

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

/**
 * @brief Removes the subscription entry from the subscription manager corresponding
 * to the provided topic.
 *
 * Iterates over all the entries in the subscription manager and tries to find
 * the one with the matching topic. If it finds one, removes it by marking it
 * free.
 *
 * @param[in] pxMQTTContext The MQTT context for which to remove the subscription.
 * @param[in] pucTopic The topic for which the subscription entry is to be removed.
 * @param[in] usTopicLength The length of the topic.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static void prvRemoveSubscription( MQTTContext_t * pxMQTTContext,
                                       const uint8_t * const pucTopic,
                                       uint16_t usTopicLength );

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

/**
 * @brief Removes the subscription entry from the subscription manager corresponding
 * to the topic contained in the provided MQTT subscribe or unsubscribe message.
 *
 * The provided buffer must contain a valid MQTT subscribe or unsubscribe message.
 * It extracts the topic from the MQTT message and removes the corresponding entry
 * in the subscription manager by calling prvRemoveSubscription.
 *
 * @param[in] pxMQTTContext The MQTT context for which to remove the subscription.
 * @param[in] xBuffer The provided buffer containing a valid MQTT subscribe or
 * unsubscribe message.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static void prvRemoveSubscriptionForSubscribeOrUnsubscribeBuffer( MQTTContext_t * pxMQTTContext,
                                                                      MQTTBufferHandle_t xBuffer );

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

/**
 * @brief Invokes the subscription callbacks for the topic on which the provided
 * publish message is received.
 *
 * It stops as soon as the user takes the ownership of the MQTT buffer by
 * returning eMQTTTrue from the callback. It follows the following sequence
 * for invoking callbacks:
 * - First it tries to find an exact match with the entries containing topic
 *   filters without wild-cards.
 * - Then it tries to find entries containing topic filters with wild-cards
 *   which match the topic on which the publish message is received.
 *
 * @param[in] pxMQTTContext The MQTT context for which to invoke the subscription callbacks.
 * @param[in] pxPublishData The publish data containing the topic and the received message.
 * @param[out] pxSubscriptionCallbackInvoked Set to eMQTTTrue if any callback was invoked,
 * otherwise set to eMQTTFalse.
 *
 * @return eMQTTTrue if the user took the ownership of the MQTT buffer, eMQTTFalse otherwise.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static MQTTBool_t prvInvokeSubscriptionCallbacks( MQTTContext_t * pxMQTTContext,
                                                      const MQTTPublishData_t * pxPublishData,
                                                      MQTTBool_t * pxSubscriptionCallbackInvoked );

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

/**
 * @brief Infers the type of the given topic filter.
 *
 * It ensure that if the given topic filter contains wild-cards, then those
 * wild-cards are valid as per the rules documented by the MQTT protocol
 * spec. The rules it checks are:
 * - '+' must either be the first character or the preceding character
 *   must be '/'.
 * - '+' must either be the last character or the succeeding character
 *   must be '/'.
 * - '#' must either be the first character or the preceding character
 *   must be '/'.
 * - '#' must be the last character.
 *
 * @param[in] pucTopicFilter The given topic filter.
 * @param[in] usTopicFilterLength The length of the given topic filter.
 *
 * @return eMQTTTopicFilterTypeSimple if the given topic filter is valid and
 * does not contain any wild-card, eMQTTTopicFilterTypeWildCard if the given
 * topic filter is valid and contains wild-cards, eMQTTTopicFilterTypeInvalid
 * otherwise.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static MQTTTopicFilterType_t prvGetTopicFilterType( const uint8_t * const pucTopicFilter,
                                                        uint16_t usTopicFilterLength );

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

/**
 * @brief Checks whether the given topic matches the given topic
 * filter.
 *
 * @warning It assumes that the given topic filter is valid i.e.
 * calling prvIsValidTopicFilter with the given topic will not
 * return eMQTTTopicFilterTypeInvalid.
 *
 * Some corner cases as documented by the MQTT protocol spec are:
 * - Filter of type "sport/#" also matches the singular "sport"
 *   since # includes the parent level.
 * - Filter of type "sport/+" also matches the "sport/" but not
 *   "sport".
 *
 * @param[in] pucTopic The given topic to match.
 * @param[in] usTopicLength The length of the given topic.
 * @param[in] pucTopicFilter The given topic filter with which to match
 * the given topic.
 * @param[in] usTopicFilterLength The length of the given topic filter.
 *
 * @return eMQTTTrue if the topic matches the filter, eMQTTFalse otherwise.
 */
#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static MQTTBool_t prvDoesTopicMatchTopicFilter( const uint8_t * const pucTopic,
                                                    uint16_t usTopicLength,
                                                    const uint8_t * const pucTopicFilter,
                                                    uint16_t usTopicFilterLength );

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
/*-----------------------------------------------------------*/

static MQTTBufferHandle_t prvGetFreeBuffer( MQTTContext_t * pxMQTTContext,
                                            uint32_t ulBufferLength )
{
    MQTTBufferHandle_t xFreeBufferHandle = NULL;
    uint32_t ulRequestedBufferLength = 0;
    uint8_t * pucFreeBuffer;

    /* Request a buffer enough to hold the metadata along
     * with the MQTT message. */
    ulRequestedBufferLength = ulBufferLength + ( uint32_t ) sizeof( MQTTBufferMetadata_t );

    /* Try to get a buffer from the free buffer pool. */
    pucFreeBuffer = pxMQTTContext->xBufferPoolInterface.pxGetBufferFxn( &ulRequestedBufferLength );

    /* Initialize the metadata, if we got a buffer. */
    if( pucFreeBuffer != NULL )
    {
        mqttbufferINIT_BUFFER( pucFreeBuffer, ulRequestedBufferLength );

        /* Get the handle to return. */
        xFreeBufferHandle = mqttbufferGET_HANDLE_FROM_RAW_BUFFER( pucFreeBuffer ); /*lint !e9087 Opaque pointer. */

        /* Ensure that the actual space in the buffer to store
         * data is at least what the user requested. */
        mqttconfigASSERT( mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xFreeBufferHandle ) >= ulBufferLength );
    }

    return xFreeBufferHandle;
}
/*-----------------------------------------------------------*/

static void prvReturnBuffer( MQTTContext_t * pxMQTTContext,
                             MQTTBufferHandle_t xBuffer )
{
    if( xBuffer != NULL )
    {
        /* Clear the payload memory. */
        memset( mqttbufferGET_DATA( xBuffer ), 0x00, mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBuffer ) );

        /* If the buffer is part of Tx list, remove it. */
        mqttbufferLIST_REMOVE( xBuffer );

        /* Return the buffer to the free buffer pool. */
        pxMQTTContext->xBufferPoolInterface.pxReturnBufferFxn( mqttbufferGET_RAW_BUFFER_FROM_HANDLE( xBuffer ) );
    }
}
/*-----------------------------------------------------------*/

static MQTTBufferHandle_t prvPacketTypeFlagsGetTxBuffer( MQTTContext_t * pxMQTTContext,
                                                         uint8_t ucPacketType,
                                                         uint8_t ucFlags )
{
    Link_t * pxLink;
    MQTTBufferHandle_t xBuffer = NULL;
    MQTTBool_t xFound = eMQTTFalse;

    /* Iterate over all buffers in the Tx buffer list. */
    listFOR_EACH( pxLink, &( pxMQTTContext->xTxBufferListHead ) )
    {
        xBuffer = mqttbufferGET_BUFFER_HANDLE_FROM_LINK( pxLink );

        /* Check if the first byte contains the given packet type
         * and flags. */
        if( ( ( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] & mqttTOP_NIBBLE_MASK ) == ucPacketType ) &&
            ( ( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] & mqttLOWER_NIBBLE_MASK ) == ucFlags ) )
        {
            xFound = eMQTTTrue;
            break;
        }
    }

    /* Did we find a buffer? */
    if( xFound == eMQTTFalse )
    {
        xBuffer = NULL;
    }

    return xBuffer;
}
/*-----------------------------------------------------------*/

static MQTTBufferHandle_t prvPacketTypeFlagsIdentifierGetTxBuffer( MQTTContext_t * pxMQTTContext,
                                                                   uint8_t ucPacketType,
                                                                   uint8_t ucFlags,
                                                                   uint16_t usPacketIdentifier )
{
    Link_t * pxLink;
    MQTTBufferHandle_t xBuffer = NULL;
    MQTTBool_t xFound = eMQTTFalse;

    /* Iterate over all buffers in the Tx buffer list. */
    listFOR_EACH( pxLink, &( pxMQTTContext->xTxBufferListHead ) )
    {
        xBuffer = mqttbufferGET_BUFFER_HANDLE_FROM_LINK( pxLink );

        /* Check that the first byte contains the given packet type
         * and flags and the packet identifier matches the given one. */
        if( ( ( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] & mqttTOP_NIBBLE_MASK ) == ucPacketType ) &&
            ( ( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] & mqttLOWER_NIBBLE_MASK ) == ucFlags ) &&
            ( mqttbufferGET_PACKET_IDENTIFIER( xBuffer ) == usPacketIdentifier ) )
        {
            xFound = eMQTTTrue;
            break;
        }
    }

    /* Did we find a buffer? */
    if( xFound == eMQTTFalse )
    {
        xBuffer = NULL;
    }

    return xBuffer;
}
/*-----------------------------------------------------------*/

static MQTTBufferHandle_t prvPacketTypeIdentifierGetTxBuffer( MQTTContext_t * pxMQTTContext,
                                                              uint8_t ucPacketType,
                                                              uint16_t usPacketIdentifier )
{
    Link_t * pxLink;
    MQTTBufferHandle_t xBuffer = NULL;
    MQTTBool_t xFound = eMQTTFalse;

    /* Iterate over all buffers in the Tx buffer list. */
    listFOR_EACH( pxLink, &( pxMQTTContext->xTxBufferListHead ) )
    {
        xBuffer = mqttbufferGET_BUFFER_HANDLE_FROM_LINK( pxLink );

        /* Check that the first byte contains the given packet type
         * and flags and the packet identifier matches the given one. */
        if( ( ( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] & mqttTOP_NIBBLE_MASK ) == ucPacketType ) &&
            ( mqttbufferGET_PACKET_IDENTIFIER( xBuffer ) == usPacketIdentifier ) )
        {
            xFound = eMQTTTrue;
            break;
        }
    }

    /* Did we find a buffer? */
    if( xFound == eMQTTFalse )
    {
        xBuffer = NULL;
    }

    return xBuffer;
}
/*-----------------------------------------------------------*/

static void prvResetRxMessageState( MQTTContext_t * pxMQTTContext )
{
    /* Prepares the context to receive the next message. */
    pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes = 0;
    pxMQTTContext->xRxMessageState.ulTotalMessageLength = 0;
    pxMQTTContext->xRxMessageState.xRxMessageAction = eMQTTRxMessageStore;
    pxMQTTContext->xRxMessageState.xRxNextByte = eMQTTRxNextBytePacketType;
    pxMQTTContext->ulRxMessageReceivedLength = 0;
    pxMQTTContext->xRxBuffer = NULL;
}
/*-----------------------------------------------------------*/

static void prvResetMQTTContext( MQTTContext_t * pxMQTTContext )
{
    Link_t * pxLink, * pxTempLink;
    MQTTBufferHandle_t xBufferHandle;

    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        uint32_t x;
    #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

    /* Set connection state to not connected. */
    pxMQTTContext->xConnectionState = eMQTTNotConnected;

    /* Return all Tx buffers to the free buffer pool. */
    listFOR_EACH_SAFE( pxLink, pxTempLink, &( pxMQTTContext->xTxBufferListHead ) )
    {
        xBufferHandle = mqttbufferGET_BUFFER_HANDLE_FROM_LINK( pxLink );
        prvReturnBuffer( pxMQTTContext, xBufferHandle );
    }

    /* Return Rx buffer to the free buffer pool. */
    prvReturnBuffer( pxMQTTContext, pxMQTTContext->xRxBuffer );

    /* Reset Rx message state. */
    prvResetRxMessageState( pxMQTTContext );

    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

        /* Mark all the subscription entires in the subscription
         * manager as free. */
        for( x = 0; x < ( uint32_t ) mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS; x++ )
        {
            pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xInUse = eMQTTFalse;
        }

        /* Set the number of in-use subscription entries to zero. */
        pxMQTTContext->xSubscriptionManager.ulInUseSubscriptions = 0;
    #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
}
/*-----------------------------------------------------------*/

static uint64_t prvGetCurrentTickCount( MQTTContext_t * pxMQTTContext )
{
    uint64_t uxCurrentTickCount = 0;

    /* If user has supplied get ticks function, use it to get the
     * current tick count. */
    if( pxMQTTContext->pxGetTicksFxn != NULL )
    {
        pxMQTTContext->pxGetTicksFxn( &( uxCurrentTickCount ) );
    }

    /* If user has not supplied get ticks function, we return zero
     * and time-stamp is stored on the next invocation of MQTT_Periodic.
     * Note that in this case the time measurement will start from the
     * next invocation of MQTT_Periodic( as opposed to this moment ). */
    return uxCurrentTickCount;
}
/*-----------------------------------------------------------*/

static MQTTBool_t prvIsTimeElapsed( uint64_t * pxRecordedTickCount,
                                    uint64_t xCurrentTickCount,
                                    uint32_t * pulRemainingTicks )
{
    uint32_t ulElapsedTicks;
    MQTTBool_t xTimeElasped = eMQTTFalse;

    if( ( *pxRecordedTickCount ) == ( uint64_t ) 0 )
    {
        /* If recorded tick count is zero, this means that user has not
         * supplied get ticks function and we should store tick count. The
         * timeout will be measured from this moment onwards. */
        *pxRecordedTickCount = xCurrentTickCount;
    }
    else
    {
        /* Check how much time has elapsed. */
        ulElapsedTicks = ( uint32_t ) ( xCurrentTickCount - *pxRecordedTickCount );

        /* Update the recorded tick count to the current tick count. */
        *pxRecordedTickCount = xCurrentTickCount;

        /* If elapsed ticks are more than remaining, return eMQTTTrue. */
        if( ulElapsedTicks >= *pulRemainingTicks )
        {
            xTimeElasped = eMQTTTrue;
            *pulRemainingTicks = 0;
        }
        else
        {
            /* Otherwise update the remaining Ticks. */
            *pulRemainingTicks -= ulElapsedTicks;
        }
    }

    return xTimeElasped;
}
/*-----------------------------------------------------------*/

static MQTTReturnCode_t prvSendData( MQTTContext_t * pxMQTTContext,
                                     const uint8_t * const pucData,
                                     uint32_t ulDataLength )
{
    MQTTReturnCode_t xReturnCode = eMQTTSendFailed;

    if( pxMQTTContext->pxMQTTSendFxn( pxMQTTContext->pvSendContext, pucData, ulDataLength ) == ulDataLength )
    {
        xReturnCode = eMQTTSuccess;

        /* Reset the last sent message timestamp. Keep alive messages
         * need to be sent only if we are not sending any message. So
         * sending any message essentially delays when the next keep
         * alive should be sent. */
        pxMQTTContext->xLastSentMessageTimestamp = prvGetCurrentTickCount( pxMQTTContext );
        pxMQTTContext->ulNextPeriodicInvokeTicks = pxMQTTContext->ulKeepAliveActualIntervalTicks;
    }

    return xReturnCode;
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedFixedHeaderOnlyMQTTPacket( MQTTContext_t * pxMQTTContext )
{
    MQTTEventCallbackParams_t xEventCallbackParams;

    /* Is this a PINGRESP? */
    if( pxMQTTContext->ucRxFixedHeaderBuffer[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] == ( uint8_t ) ( mqttCONTROL_PINGRESP | mqttFLAGS_PINGRESP ) )
    {
        prvProcessReceivedPINGRESP( pxMQTTContext );
    }
    /* Any other fixed header only packet is considered a malformed packet. */
    else
    {
        /* A malformed packet has been received - disconnect. */
        prvResetMQTTContext( pxMQTTContext );

        /* Inform the user about the malformed packet. */
        xEventCallbackParams.xEventType = eMQTTClientDisconnected;
        xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonMalformedPacket;
        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedMQTTPacket( MQTTContext_t * pxMQTTContext )
{
    MQTTEventCallbackParams_t xEventCallbackParams;

    /* Is this a publish message from broker? */
    if( ( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] & mqttTOP_NIBBLE_MASK ) == mqttCONTROL_PUBLISH )
    {
        prvProcessReceivedPublish( pxMQTTContext );
    }
    /* Is this a CONNACK? */
    else if( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] == ( uint8_t ) ( mqttCONTROL_CONNACK | mqttFLAGS_CONNACK ) )
    {
        prvProcessReceivedCONNACK( pxMQTTContext );
    }
    /* Is this a PUBACK? */
    else if( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] == ( uint8_t ) ( mqttCONTROL_PUBACK | mqttFLAGS_PUBACK ) )
    {
        prvProcessReceivedPUBACK( pxMQTTContext );
    }
    /* Is this a SUBACK? */
    else if( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] == ( uint8_t ) ( mqttCONTROL_SUBACK | mqttFLAGS_SUBACK ) )
    {
        prvProcessReceivedSUBACK( pxMQTTContext );
    }
    /* Is this an UNSUBACK? */
    else if( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] == ( uint8_t ) ( mqttCONTROL_UNSUBACK | mqttFLAGS_UNSUBACK ) )
    {
        prvProcessReceivedUNSUBACK( pxMQTTContext );
    }
    /* Any other packet is considered malformed. */
    else
    {
        /* A malformed packet has been received - disconnect. */
        prvResetMQTTContext( pxMQTTContext );

        /* Inform the user about the malformed packet. */
        xEventCallbackParams.xEventType = eMQTTClientDisconnected;
        xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonMalformedPacket;
        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedCONNACK( MQTTContext_t * pxMQTTContext )
{
    MQTTBufferHandle_t xConnectTxBuffer;
    MQTTEventCallbackParams_t xEventCallbackParams;
    MQTTBool_t xConnectionEstablished = eMQTTFalse, xConnectionRefused = eMQTTFalse, xMalformedPacket = eMQTTFalse;
    uint8_t ucReturnCode;
    static const uint8_t ucDefaultCONNACKParameters[] =
    {
        mqttCONTROL_CONNACK | mqttFLAGS_CONNACK, /* Fixed header control packet type. */
        2,                                       /* Fixed header remaining length - always 2 for CONNACK. */
        0,                                       /* Bit 0 is SP - Session Present. */
        0,                                       /* Return code. */
    };

    /* Is there a connect message waiting for CONNACK? */
    xConnectTxBuffer = prvPacketTypeFlagsGetTxBuffer( pxMQTTContext, mqttCONTROL_CONNECT, mqttFLAGS_CONNECT );

    if( xConnectTxBuffer == NULL )
    {
        /* Either the connect was never sent or the sender has
         * timed out i.e. not waiting for CONNACK. Either case,
         * it is an unexpected CONANCK. Just inform the user that
         * an unexpected CONNACK (which might be a delayed one from
         * user's perspective) was received. */
        xEventCallbackParams.xEventType = eMQTTUnexpectedConnACK;
        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }
    else
    {
        if( mqttbufferGET_DATA_LENGTH( pxMQTTContext->xRxBuffer ) >= sizeof( ucDefaultCONNACKParameters ) )
        {
            /* Received enough data for a CONNACK - does the received fixed header match
             * the expected one for the CONNACK message (Fixed header is of 2 bytes for CONNACK
             * message because Remaining Length is 2 which takes only one byte)? */
            if( memcmp( ucDefaultCONNACKParameters, mqttbufferGET_DATA( pxMQTTContext->xRxBuffer ), mqttFIXED_HEADER_MIN_SIZE ) == 0 )
            {
                mqttconfigDEBUG_LOG( ( "CONNACK received.\r\n" ) );

                xEventCallbackParams.xEventType = eMQTTConnACK;

                /* Since AWS IoT only supports CleanSession 1, SP bit will
                 * always be zero. */
                ucReturnCode = mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttCONNACK_RETURN_CODE_OFFSET ];

                if( ucReturnCode == ( uint8_t ) 0 ) /* Connection Accepted. */
                {
                    /* Server has accepted the connection and we are now in
                     * connected state. */
                    xEventCallbackParams.xEventType = eMQTTConnACK;
                    xEventCallbackParams.u.xMQTTConnACKData.xConnACKReturnCode = eMQTTConnACKConnectionAccepted;
                    xEventCallbackParams.u.xMQTTConnACKData.usPacketIdentifier = mqttbufferGET_PACKET_IDENTIFIER( xConnectTxBuffer );
                    ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                    /* Connection is established. */
                    xConnectionEstablished = eMQTTTrue;
                }
                else if( ( ucReturnCode >= ( uint8_t ) 1 ) && ( ucReturnCode <= ( uint8_t ) 5 ) )
                {
                    /* Server refused to accept the connection. */
                    xEventCallbackParams.xEventType = eMQTTConnACK;

                    /* Convert the return code to a user friendly enum value. */
                    if( ucReturnCode == ( uint8_t ) 1 )
                    {
                        xEventCallbackParams.u.xMQTTConnACKData.xConnACKReturnCode = eMQTTConnACKUnacceptableProtocolVersion;
                    }
                    else if( ucReturnCode == ( uint8_t ) 2 )
                    {
                        xEventCallbackParams.u.xMQTTConnACKData.xConnACKReturnCode = eMQTTConnACKIdentifierRejected;
                    }
                    else if( ucReturnCode == ( uint8_t ) 3 )
                    {
                        xEventCallbackParams.u.xMQTTConnACKData.xConnACKReturnCode = eMQTTConnACKServerUnavailable;
                    }
                    else if( ucReturnCode == ( uint8_t ) 4 )
                    {
                        xEventCallbackParams.u.xMQTTConnACKData.xConnACKReturnCode = eMQTTConnACKBadUsernameOrPassword;
                    }
                    else
                    {
                        xEventCallbackParams.u.xMQTTConnACKData.xConnACKReturnCode = eMQTTConnACKUnauthorized;
                    }

                    xEventCallbackParams.u.xMQTTConnACKData.usPacketIdentifier = mqttbufferGET_PACKET_IDENTIFIER( xConnectTxBuffer );
                    ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                    xConnectionRefused = eMQTTTrue;
                }
                else
                {
                    /* Malformed packet - Reserved return code. */
                    xMalformedPacket = eMQTTTrue;
                }
            }
            else
            {
                mqttconfigDEBUG_LOG( ( "Unknown messages %x %x %x %x, expected CONNACK, disconnecting socket.\r\n",
                                       mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ 0 ],
                                       mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ 1 ],
                                       mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ 2 ],
                                       mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ 3 ] ) );

                /* Malformed packet - Fixed header does not match. */
                xMalformedPacket = eMQTTTrue;
            }
        }
        else
        {
            /* Malformed packet - Not enough bytes. */
            xMalformedPacket = eMQTTTrue;
        }

        /* Return the Tx buffer to the free buffer pool. */
        prvReturnBuffer( pxMQTTContext, xConnectTxBuffer );
    }

    /* If connection was refused by the server or a malformed packet
     * was received, we need to disconnect and inform the user. */
    if( ( xConnectionRefused == eMQTTTrue ) || ( xMalformedPacket == eMQTTTrue ) )
    {
        prvResetMQTTContext( pxMQTTContext );
        xEventCallbackParams.xEventType = eMQTTClientDisconnected;

        /* Set the appropriate disconnect reason. */
        if( xMalformedPacket == eMQTTTrue )
        {
            xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonMalformedPacket;
        }
        else
        {
            xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonBrokerRefusedConnection;
        }

        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }

    /* Did we get an expected CONNACK from the server. */
    if( xConnectionEstablished == eMQTTTrue )
    {
        /* Set the connection state as connected. */
        pxMQTTContext->xConnectionState = eMQTTConnected;

        /* Now onwards, keep alive messages should be sent regularly in
         * order to ensure a long lived connection. */
        pxMQTTContext->xLastSentMessageTimestamp = prvGetCurrentTickCount( pxMQTTContext );
        pxMQTTContext->ulNextPeriodicInvokeTicks = pxMQTTContext->ulKeepAliveActualIntervalTicks;

        /* No ping has been sent yet. */
        pxMQTTContext->xWaitingForPingResp = eMQTTFalse;
    }

    /* Return the RxBuffer to the free buffer pool. */
    prvReturnBuffer( pxMQTTContext, pxMQTTContext->xRxBuffer );
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedSUBACK( MQTTContext_t * pxMQTTContext )
{
    MQTTBufferHandle_t xSubscribeTxBuffer;
    MQTTEventCallbackParams_t xEventCallbackParams;
    MQTTBool_t xMalformedPacket = eMQTTFalse;
    uint8_t ucReturnCode;
    uint16_t usPacketIdentifier;

    /* Must have enough bytes to at least read out one return code. */
    if( mqttbufferGET_DATA_LENGTH( pxMQTTContext->xRxBuffer ) > ( uint32_t ) mqttADJUST_OFFSET( mqttSUBACK_RETURN_CODE_OFFSET,
                                                                                                pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) )
    {
        /* Extract the packet identifier and see if there is a subscribe
         * packet waiting for ACK. */
        usPacketIdentifier = ( uint16_t ) ( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttSUBACK_PACKET_ID_MSB_OFFSET,
                                                                                                               pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ] );
        usPacketIdentifier <<= mqttBITS_PER_BYTE;
        usPacketIdentifier |= ( uint8_t ) ( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttSUBACK_PACKET_ID_LSB_OFFSET,
                                                                                                               pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ] );

        xSubscribeTxBuffer = prvPacketTypeFlagsIdentifierGetTxBuffer( pxMQTTContext, mqttCONTROL_SUBSCRIBE, mqttFLAGS_SUBSCRIBE, usPacketIdentifier );

        if( xSubscribeTxBuffer == NULL )
        {
            /* Either a subscribe was never sent or the sender
             * timed out. Either case, this is an unexpected
             * SUBACK. */
            xEventCallbackParams.xEventType = eMQTTUnexpectedSubACK;
            ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
        }
        else
        {
            /* Extract the return code from the packet. */
            ucReturnCode = mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttSUBACK_RETURN_CODE_OFFSET,
                                                                                              pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ];

            /* Return code must be valid. Note that QoS2 is not supported. */
            if( ( ucReturnCode <= ( uint8_t ) 1 ) || ( ucReturnCode == ( uint8_t ) 128 ) )
            {
                /* Inform the user about the received SUBACK. */
                xEventCallbackParams.xEventType = eMQTTSubACK;

                /* Convert the return code to a user friendly enum value. */
                if( ucReturnCode == ( uint8_t ) 0 )
                {
                    xEventCallbackParams.u.xMQTTSubACKData.xSubACKReturnCode = eMQTTSubACKSuccessQos0;
                }
                else if( ucReturnCode == ( uint8_t ) 1 )
                {
                    xEventCallbackParams.u.xMQTTSubACKData.xSubACKReturnCode = eMQTTSubACKSuccessQos1;
                }
                else
                {
                    xEventCallbackParams.u.xMQTTSubACKData.xSubACKReturnCode = eMQTTSubACKFailure;
                }

                xEventCallbackParams.u.xMQTTSubACKData.usPacketIdentifier = usPacketIdentifier;
                ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

                    /* If the broker rejected the subscription request,
                     * remove the subscription entry from the subscription
                     * manager. */
                    if( ucReturnCode == ( uint8_t ) 128 ) /* Subscription failure. */
                    {
                        prvRemoveSubscriptionForSubscribeOrUnsubscribeBuffer( pxMQTTContext, xSubscribeTxBuffer );
                    }
                #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

                /* Return the Tx Buffer to the pool. */
                prvReturnBuffer( pxMQTTContext, xSubscribeTxBuffer );
            }
            else
            {
                /* Malformed packet - reserved return code. */
                xMalformedPacket = eMQTTTrue;
            }
        }
    }
    else
    {
        /* Malformed packet - not enough bytes. */
        xMalformedPacket = eMQTTTrue;
    }

    /* A malformed packet should result in disconnect. Note
     * that there is no need to disconnect in the case of
     * unexpected SUBACK. */
    if( xMalformedPacket == eMQTTTrue )
    {
        prvResetMQTTContext( pxMQTTContext );

        /* Inform user about the malformed packet received. */
        xEventCallbackParams.xEventType = eMQTTClientDisconnected;
        xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonMalformedPacket;
        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }

    /* Return the RxBuffer to the free buffer pool. */
    prvReturnBuffer( pxMQTTContext, pxMQTTContext->xRxBuffer );
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedUNSUBACK( MQTTContext_t * pxMQTTContext )
{
    MQTTBufferHandle_t xUnsubscribeTxBuffer;
    MQTTEventCallbackParams_t xEventCallbackParams;
    MQTTBool_t xMalformedPacket = eMQTTFalse;
    uint16_t usPacketIdentifier;
    static const uint8_t ucUNSUBACKFixedHeader[] =
    {
        mqttCONTROL_UNSUBACK | mqttFLAGS_UNSUBACK, /* Fixed header control packet type. */
        2,                                         /* Fixed header remaining length - always 2 for UNSUBACK. */
    };

    /* Must have enough bytes to form a complete UNSUBACK packet
     * which contains 2 byte packet identifier other than the fixed
     * header. */
    if( mqttbufferGET_DATA_LENGTH( pxMQTTContext->xRxBuffer ) >= ( sizeof( ucUNSUBACKFixedHeader ) + ( uint32_t ) mqttUNSUBACK_PACKET_IDENTIFER_LENGTH ) )
    {
        /* Received enough data for an UNSUBACK - does the received fixed header match
         * the expected one for the UNSUBACK message (Fixed header is of 2 bytes for UNSUBACK
         * message because Remaining Length is 2 which takes only one byte)? */
        if( memcmp( ucUNSUBACKFixedHeader, mqttbufferGET_DATA( pxMQTTContext->xRxBuffer ), sizeof( ucUNSUBACKFixedHeader ) ) == 0 )
        {
            /* Extract the packet identifier and see if there is an unsubscribe
             * packet waiting for ACK. */
            usPacketIdentifier = ( uint8_t ) ( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttUNSUBACK_PACKET_ID_MSB_OFFSET,
                                                                                                                  pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ] );
            usPacketIdentifier <<= mqttBITS_PER_BYTE;
            usPacketIdentifier |= ( uint8_t ) ( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttUNSUBACK_PACKET_ID_LSB_OFFSET,
                                                                                                                   pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ] );

            xUnsubscribeTxBuffer = prvPacketTypeFlagsIdentifierGetTxBuffer( pxMQTTContext, mqttCONTROL_UNSUBSCRIBE, mqttFLAGS_UNSUBSCRIBE, usPacketIdentifier );

            if( xUnsubscribeTxBuffer == NULL )
            {
                /* Either an unsubscribe was never sent or the sender
                 * timed out. Either case, this is an unexpected UNSUBACK. */
                xEventCallbackParams.xEventType = eMQTTUnexpectedUnSubACK;
                ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
            }
            else
            {
                /* Inform the user about the received UNSUBACK. */
                xEventCallbackParams.xEventType = eMQTTUnSubACK;
                xEventCallbackParams.u.xMQTTUnSubACKData.usPacketIdentifier = usPacketIdentifier;
                ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

                    /* If we successfully unsubscribed, remove the
                     * corresponding subscription entry from the subscription
                     * manager. */
                    prvRemoveSubscriptionForSubscribeOrUnsubscribeBuffer( pxMQTTContext, xUnsubscribeTxBuffer );
                #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

                /* Return the Tx Buffer to the pool. */
                prvReturnBuffer( pxMQTTContext, xUnsubscribeTxBuffer );
            }
        }
        else
        {
            /* Malformed packet - fixed header does not match the expected one. */
            xMalformedPacket = eMQTTTrue;
        }
    }
    else
    {
        /* Malformed packet - not enough bytes. */
        xMalformedPacket = eMQTTTrue;
    }

    /* A malformed packet should result in disconnect. Note
     * that there is no need to disconnect in the case of
     * unexpected UNSUBACK. */
    if( xMalformedPacket == eMQTTTrue )
    {
        prvResetMQTTContext( pxMQTTContext );

        /* Inform user about the malformed packet received. */
        xEventCallbackParams.xEventType = eMQTTClientDisconnected;
        xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonMalformedPacket;
        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }

    /* Return the RxBuffer to the free buffer pool. */
    prvReturnBuffer( pxMQTTContext, pxMQTTContext->xRxBuffer );
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedPUBACK( MQTTContext_t * pxMQTTContext )
{
    MQTTBufferHandle_t xPublishTxBuffer;
    MQTTEventCallbackParams_t xEventCallbackParams;
    MQTTBool_t xMalformedPacket = eMQTTFalse;
    uint16_t usPacketIdentifier;
    static const uint8_t ucPUBACKFixedHeader[] =
    {
        mqttCONTROL_PUBACK | mqttFLAGS_PUBACK, /* Fixed header control packet type. */
        2,                                     /* Fixed header remaining length - always 2 for PUBACK. */
    };

    /* Must have enough bytes to form a complete PUBACK packet
     * which contains 2 byte packet identifier other than the fixed
     * header. */
    if( mqttbufferGET_DATA_LENGTH( pxMQTTContext->xRxBuffer ) >= ( sizeof( ucPUBACKFixedHeader ) + ( uint32_t ) mqttPUBACK_PACKET_IDENTIFER_LENGTH ) )
    {
        /* Received enough data for a PUBACK - does the received fixed header match
         * the expected one for the PUBACK message (Fixed header is of 2 bytes for PUBACK
         * message because Remaining Length is 2 which takes only one byte)? */
        if( memcmp( ucPUBACKFixedHeader, mqttbufferGET_DATA( pxMQTTContext->xRxBuffer ), sizeof( ucPUBACKFixedHeader ) ) == 0 )
        {
            /* Extract the packet identifier and see if there is a publish
             * packet waiting for ACK. */
            usPacketIdentifier = ( uint8_t ) ( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttPUBACK_PACKET_ID_MSB_OFFSET,
                                                                                                                  pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ] );
            usPacketIdentifier <<= mqttBITS_PER_BYTE;
            usPacketIdentifier |= ( uint8_t ) ( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttPUBACK_PACKET_ID_LSB_OFFSET,
                                                                                                                   pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ] );

            xPublishTxBuffer = prvPacketTypeIdentifierGetTxBuffer( pxMQTTContext, mqttCONTROL_PUBLISH, usPacketIdentifier );

            if( xPublishTxBuffer == NULL )
            {
                /* Either a publish was never sent or the sender
                 * timed out. Either case, this is an unexpected PUBACK. */
                xEventCallbackParams.xEventType = eMQTTUnexpectedPubACK;
                ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
            }
            else
            {
                /* Inform the user about the received UNSUBACK. */
                xEventCallbackParams.xEventType = eMQTTPubACK;
                xEventCallbackParams.u.xMQTTPubACKData.usPacketIdentifier = usPacketIdentifier;
                ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                /* Return the Tx Buffer to the pool. */
                prvReturnBuffer( pxMQTTContext, xPublishTxBuffer );
            }
        }
        else
        {
            /* Malformed packet - fixed header does not match the expected one. */
            xMalformedPacket = eMQTTTrue;
        }
    }
    else
    {
        /* Malformed packet - not enough bytes. */
        xMalformedPacket = eMQTTTrue;
    }

    /* A malformed packet should result in disconnect. Note
     * that there is no need to disconnect in the case of
     * unexpected PUBACK. */
    if( xMalformedPacket == eMQTTTrue )
    {
        prvResetMQTTContext( pxMQTTContext );

        /* Inform user about the malformed packet received. */
        xEventCallbackParams.xEventType = eMQTTClientDisconnected;
        xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonMalformedPacket;
        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }

    /* Return the RxBuffer to the free buffer pool. */
    prvReturnBuffer( pxMQTTContext, pxMQTTContext->xRxBuffer );
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedPINGRESP( MQTTContext_t * pxMQTTContext )
{
    MQTTEventCallbackParams_t xEventCallbackParams;
    MQTTBool_t xValidPingResponse = eMQTTFalse;
    static const uint8_t ucPingRespPacket[] =
    {
        mqttCONTROL_PINGRESP | mqttFLAGS_PINGRESP,
        0 /* PINGRESP packet does not have any variable header or payload. */
    };

    /* Did we receive a valid PINGRESP? */
    if( pxMQTTContext->ulRxMessageReceivedLength >= sizeof( ucPingRespPacket ) )
    {
        if( memcmp( pxMQTTContext->ucRxFixedHeaderBuffer, ucPingRespPacket, sizeof( ucPingRespPacket ) ) == 0 )
        {
            /* PINGRESP packet is valid. */
            xValidPingResponse = eMQTTTrue;
        }
    }

    if( xValidPingResponse == eMQTTTrue )
    {
        /* If we were expecting a PINGESP, update the time-stamp
         * when the next PINGREQ should be sent. Otherwise ignore
         * this unexpected PINRESP. */
        if( pxMQTTContext->xWaitingForPingResp == eMQTTTrue )
        {
            /* Mark that we received the expected PINGRESP. */
            pxMQTTContext->xWaitingForPingResp = eMQTTFalse;

            /* Reset the last sent message timestamp so that the
             * next PINGREQ can be sent at appropriate time. */
            pxMQTTContext->xLastSentMessageTimestamp = prvGetCurrentTickCount( pxMQTTContext );
            pxMQTTContext->ulNextPeriodicInvokeTicks = pxMQTTContext->ulKeepAliveActualIntervalTicks;
        }
        else
        {
            /* Unexpected PINGRESP. */
            mqttconfigDEBUG_LOG( ( "Unexpected PINGRESP received.\r\n" ) );
        }
    }
    else
    {
        /* A malformed packet should result in disconnect. */
        prvResetMQTTContext( pxMQTTContext );

        /* Inform user about the malformed packet received. */
        xEventCallbackParams.xEventType = eMQTTClientDisconnected;
        xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonMalformedPacket;
        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }
}
/*-----------------------------------------------------------*/

static void prvProcessReceivedPublish( MQTTContext_t * pxMQTTContext )
{
    MQTTEventCallbackParams_t xEventCallbackParams;
    uint8_t ucPacketIdentiferLength; /* Length in bytes taken by the packet identifier field in the received publish packet. */
    uint8_t ucQos;
    static uint8_t ucPUBACKPacket[] =
    {
        mqttCONTROL_PUBACK | mqttFLAGS_PUBACK, /* Fixed header control packet type. */
        2,                                     /* Fixed header remaining length - always 2 for PUBACK. */
        0,                                     /* Packet identifier MSB. */
        0                                      /* Packet identifier LSB. */
    };

    /* A broker has sent a message to this client.  Decode it, then pass the
     * decoded message into an application defined callback. */
    xEventCallbackParams.xEventType = eMQTTPublish;

    /*_TODO_ Do we want to expose DUP and RETAIN? */
    ucQos = mqttPUBLISH_QoS_BITS( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] );

    /* QoS2 is not supported. */
    if( ( ucQos == ( uint8_t ) 0 /* QoS0. */ ) || ( ucQos == ( uint8_t ) 1 /* QoS1. */ ) )
    {
        xEventCallbackParams.u.xPublishData.xQos = ( ucQos == ( uint8_t ) 0 ) ? eMQTTQoS0 : eMQTTQoS1;

        if( xEventCallbackParams.u.xPublishData.xQos == eMQTTQoS0 )
        {
            ucPacketIdentiferLength = mqttPUBLISH_QOS0_PACKET_IDENTIFER_LENGTH;
        }
        else
        {
            ucPacketIdentiferLength = mqttPUBLISH_QOS1_PACKET_IDENTIFER_LENGTH;
        }

        /* Extract Topic Length. */
        xEventCallbackParams.u.xPublishData.usTopicLength = ( uint16_t ) mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttPUBLISH_TOPIC_LENGTH_MSB,
                                                                                                                                            pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ];
        xEventCallbackParams.u.xPublishData.usTopicLength <<= mqttBITS_PER_BYTE;
        xEventCallbackParams.u.xPublishData.usTopicLength |= ( uint16_t ) mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttPUBLISH_TOPIC_LENGTH_LSB,
                                                                                                                                             pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ];

        /* Extract Topic. */
        xEventCallbackParams.u.xPublishData.pucTopic = &( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttPUBLISH_TOPIC_STRING_OFFSET,
                                                                                                                             pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) ] );

        /* Extract Published Data. */
        xEventCallbackParams.u.xPublishData.pvData = ( void * ) &( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttPUBLISH_TOPIC_STRING_OFFSET,
                                                                                                                                      pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) +
                                                                                                                   xEventCallbackParams.u.xPublishData.usTopicLength +
                                                                                                                   ucPacketIdentiferLength ] ); /*lint !e9087 Publish data is provided as void* to the user. */

        /* Topic string is followed by packet identifier which is
         * followed by actual data. NOte that QoS0 publishes do not
         * have packet identifier. */
        xEventCallbackParams.u.xPublishData.ulDataLength = pxMQTTContext->xRxMessageState.ulTotalMessageLength - ( mqttADJUST_OFFSET( mqttPUBLISH_TOPIC_STRING_OFFSET,
                                                                                                                                      pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) +
                                                                                                                   xEventCallbackParams.u.xPublishData.usTopicLength +
                                                                                                                   ucPacketIdentiferLength );

        /* Pass the handle of the buffer containing the whole MQTT message. */
        xEventCallbackParams.u.xPublishData.xBuffer = pxMQTTContext->xRxBuffer;

        /* If this is a QoS1 publish, send the PUBACK before invoking the
         * callback. */
        if( xEventCallbackParams.u.xPublishData.xQos == eMQTTQoS1 )
        {
            /* Extract the packet identifier from the publish message
             * to set the same in PUBACK message. */
            ucPUBACKPacket[ mqttPUBACK_PACKET_ID_MSB_OFFSET ] = mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttPUBLISH_TOPIC_STRING_OFFSET,
                                                                                                                                   pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) +
                                                                                                                xEventCallbackParams.u.xPublishData.usTopicLength ];
            ucPUBACKPacket[ mqttPUBACK_PACKET_ID_LSB_OFFSET ] = mqttbufferGET_DATA( pxMQTTContext->xRxBuffer )[ mqttADJUST_OFFSET( mqttPUBLISH_TOPIC_STRING_OFFSET,
                                                                                                                                   pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes ) +
                                                                                                                xEventCallbackParams.u.xPublishData.usTopicLength +
                                                                                                                ( uint16_t ) 1 /* Packet ID LSB follows MSB. */ ];

            /* Send a PUBACK to the broker confirming the receipt
             * of the publish message. If we fail to send the PUBACK,
             * we will receive the same publish message again. */
            ( void ) prvSendData( pxMQTTContext, ucPUBACKPacket, ( uint32_t ) sizeof( ucPUBACKPacket ) );
        }

        /* If the user chooses not to take the ownership of the buffer,
         * return it back to the free buffer pool. */
        if( prvInvokeCallback( pxMQTTContext, &xEventCallbackParams ) == eMQTTFalse )
        {
            prvReturnBuffer( pxMQTTContext, pxMQTTContext->xRxBuffer );
        }
    }
    else
    {
        /* A publish packet with QoS2 is considered malformed and
         * we disconnect. */
        prvResetMQTTContext( pxMQTTContext );

        /* Inform user about the malformed packet received. */
        xEventCallbackParams.xEventType = eMQTTClientDisconnected;
        xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonMalformedPacket;
        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }
}
/*-----------------------------------------------------------*/

static MQTTBool_t prvInvokeCallback( MQTTContext_t * pxMQTTContext,
                                     MQTTEventCallbackParams_t * pxEventCallbackParams )
{
    MQTTBool_t xBufferOwnershipTaken = eMQTTFalse;

    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        MQTTBool_t xSubscriptionCallbackInvoked = eMQTTFalse;

        /* In case of a publish message, try to invoke
         * subscription specific callback first. If the
         * user has not registered any specific callback
         * for this topic, invoke the generic one. */
        if( pxEventCallbackParams->xEventType == eMQTTPublish )
        {
            xBufferOwnershipTaken = prvInvokeSubscriptionCallbacks( pxMQTTContext,
                                                                    &( pxEventCallbackParams->u.xPublishData ),
                                                                    &( xSubscriptionCallbackInvoked ) );
        }

        /* Invoke generic callback, if any subscription callback was
         * not invoked and the user has registered a generic one. Note
         * that since no callback was invoked, we are sure that the user
         * has not got any chance to take the buffer ownership. That's
         * why no check for xBufferOwnershipTaken. */
        if( ( xSubscriptionCallbackInvoked == eMQTTFalse ) && ( pxMQTTContext->pxCallback != NULL ) )
        {
            xBufferOwnershipTaken = pxMQTTContext->pxCallback( pxMQTTContext->pvCallbackContext, pxEventCallbackParams );
        }
    #else /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
        /* Invoke the generic callback, if the user has registered one. */
        if( pxMQTTContext->pxCallback != NULL )
        {
            xBufferOwnershipTaken = pxMQTTContext->pxCallback( pxMQTTContext->pvCallbackContext, pxEventCallbackParams );
        }
    #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

    return xBufferOwnershipTaken;
}
/*-----------------------------------------------------------*/

static uint8_t * prvWriteString( uint8_t * pucDestination,
                                 const uint8_t * const pucLastByteInBuffer,
                                 const uint8_t * const pucString,
                                 uint16_t usStringLength )
{
    uint8_t * pucEndAddress;

    /* Check that writing the string and the two byte length into pucDestination
     * won't overflow the buffer. */
    pucEndAddress = &( pucDestination[ mqttSTRLEN( usStringLength ) - ( uint16_t ) 1 ] );

    /* Check the calculated end address against the last address of the Tx
     * buffer. */
    if( pucEndAddress <= pucLastByteInBuffer ) /*lint !e946 Pointer comparison is ok as we are checking for memory overflow. */
    {
        *pucDestination = ( uint8_t ) ( usStringLength >> mqttBITS_PER_BYTE );
        pucDestination++;
        *pucDestination = ( uint8_t ) usStringLength;
        pucDestination++;
        memcpy( pucDestination, pucString, usStringLength );
    }
    else
    {
        /* Writing the string would have overflowed the Tx buffer. */
        usStringLength = ( uint16_t ) 0;
    }

    /* Return the address of the byte following the string just copied to the
     * buffer. */
    return &( pucDestination[ usStringLength ] );
}
/*-----------------------------------------------------------*/

static uint8_t prvSizeOfRemainingLength( uint32_t ulRemainingLength )
{
    uint8_t ucRemainingLengthFieldBytes = 0;

    /* Ensure that given length is within the limits of what can be
     * encoded in 4 bytes. */
    mqttconfigASSERT( ulRemainingLength <= mqttMAX_REMAINING_LENGTH_LENGTH );

    /* The numbers below are taken from the MQTT protocol spec. */
    if( ulRemainingLength <= ( uint32_t ) mqttONE_BYTE_MAX_REMAINING_LENGTH )
    {
        ucRemainingLengthFieldBytes = 1;
    }
    else if( ( ulRemainingLength > ( uint32_t ) mqttONE_BYTE_MAX_REMAINING_LENGTH ) && ( ulRemainingLength <= ( uint32_t ) mqttTWO_BYTES_MAX_REMAINING_LENGTH ) )
    {
        ucRemainingLengthFieldBytes = 2;
    }
    else if( ( ulRemainingLength > ( uint32_t ) mqttTWO_BYTES_MAX_REMAINING_LENGTH ) && ( ulRemainingLength <= ( uint32_t ) mqttTHREE_BYTES_MAX_REMAINING_LENGTH ) )
    {
        ucRemainingLengthFieldBytes = 3;
    }
    else if( ( ulRemainingLength > ( uint32_t ) mqttTHREE_BYTES_MAX_REMAINING_LENGTH ) && ( ulRemainingLength <= ( uint32_t ) mqttMAX_REMAINING_LENGTH_LENGTH ) )
    {
        ucRemainingLengthFieldBytes = 4;
    }
    else
    {
        /* Incorrect remaining length. */
    }

    return ucRemainingLengthFieldBytes;
}
/*-----------------------------------------------------------*/

static uint8_t prvEncodeRemainingLength( uint32_t ulRemainingLength,
                                         uint8_t * const pucEncodedRemainingLength,
                                         const uint8_t * const pucLastByteInBuffer )
{
    uint8_t ucBytesWritten = 0, ucEncodedByte;

    /* Ensure that given length is within the limits of what can be
     * encoded in 4 bytes. */
    mqttconfigASSERT( ulRemainingLength <= mqttMAX_REMAINING_LENGTH_LENGTH );

    do
    {
        /* Extract 7 bits from the length. */
        ucEncodedByte = ( ( uint8_t ) ( ulRemainingLength ) & mqttLENGTH_BITMASK_REMAINING_LENGTH );
        ulRemainingLength >>= mqttLENGTH_BITS_REMAINING_LENGTH;

        /* If there is more data to encode, set the continuation
         * bit of this byte. */
        if( ulRemainingLength > ( uint32_t ) 0 )
        {
            ucEncodedByte |= mqttREMAINING_LENGTH_CONTINUATION_BITMASK;
        }

        /* Ensure that writing this byte will not result in buffer overflow. */
        mqttconfigASSERT( &( pucEncodedRemainingLength[ ucBytesWritten ] ) <= pucLastByteInBuffer );

        if( &( pucEncodedRemainingLength[ ucBytesWritten ] ) <= pucLastByteInBuffer ) /*lint !e946 Pointer comparison is ok as we are checking for memory overflow. */
        {
            /* Write the encoded byte to the provided buffer. */
            pucEncodedRemainingLength[ ucBytesWritten ] = ucEncodedByte;
            ucBytesWritten++;
        }
        else
        {
            /* There is not enough space in the provided buffer. Inform the caller
             * by returning zero. */
            ucBytesWritten = 0;
            break;
        }
    } while( ulRemainingLength > ( uint32_t ) 0 );

    return ucBytesWritten;
}
/*-----------------------------------------------------------*/

static uint8_t prvDecodeRemainingLength( const uint8_t * const pucEncodedRemainingLength,
                                         uint32_t * const pulRemainingLength )
{
    uint8_t ucBytesRead, ucDecodedByte;
    uint32_t x, ulDecodedRemainingLength = 0, ulDecodedByte;

    for( x = 0; x < ( uint32_t ) mqttREMAINING_LENGTH_MAX_BYTES; x++ )
    {
        /* Extract 7 lower bits from the current byte - These bits contain the
         * actual length information. */
        ucDecodedByte = ( pucEncodedRemainingLength[ x ] & mqttLENGTH_BITMASK_REMAINING_LENGTH );

        /* Move the extracted bits to their correct location. */
        ulDecodedByte = ( uint32_t ) ucDecodedByte;
        ulDecodedByte <<= ( x * ( uint32_t ) mqttLENGTH_BITS_REMAINING_LENGTH );

        /* Place the extracted bits in the decoded length. */
        ulDecodedRemainingLength |= ulDecodedByte;

        /* If the continuation bit is not set in the current byte, decoding is
         * complete. */
        if( ( pucEncodedRemainingLength[ x ] & mqttREMAINING_LENGTH_CONTINUATION_BITMASK ) == ( uint8_t ) 0 )
        {
            break;
        }
    }

    if( x >= ( uint32_t ) mqttREMAINING_LENGTH_MAX_BYTES )
    {
        /* This would happen in case 4th byte also has continuation bit set.
         * This indicates a malformed encoded length and we inform the caller by
         * returning 0. Note that the provided output buffer pulRemainingLength
         * is not modified in this case. */
        ucBytesRead = 0;
    }
    else
    {
        /* The decoding is successful. Inform the user by returning the number
         * of bytes used in decoding from given encoded length. */
        ucBytesRead = ( uint8_t ) x + ( uint8_t ) 1;

        /* If the user has provided output buffer, return the decoded length
         * in it. */
        if( pulRemainingLength != NULL )
        {
            *pulRemainingLength = ulDecodedRemainingLength;
        }
    }

    return ucBytesRead;
}
/*-----------------------------------------------------------*/

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static MQTTBool_t prvStoreSubscription( MQTTContext_t * pxMQTTContext,
                                            const uint8_t * const pucTopic,
                                            uint16_t usTopicLength,
                                            void * pvPublishCallbackContext,
                                            MQTTPublishCallback_t pxPublishCallback )
    {
        uint32_t x;
        MQTTBool_t xSubscriptionStored = eMQTTFalse;
        MQTTTopicFilterType_t xTopicFilterType;

        /* Is there a free entry in the subscription manager? */
        if( pxMQTTContext->xSubscriptionManager.ulInUseSubscriptions < ( uint32_t ) mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS )
        {
            /* Check that the topic name is not too long. */
            if( usTopicLength <= ( uint16_t ) mqttconfigSUBSCRIPTION_MANAGER_MAX_TOPIC_LENGTH )
            {
                /* Ensure that the topic is not invalid. */
                xTopicFilterType = prvGetTopicFilterType( pucTopic, usTopicLength );

                if( xTopicFilterType != eMQTTTopicFilterTypeInvalid )
                {
                    /* Ensure that subscription manager does not contain
                     * an entry for the topic filter already. */
                    prvRemoveSubscription( pxMQTTContext, pucTopic, usTopicLength );

                    /* Find a free entry in the subscription manager. */
                    for( x = 0; x < ( uint32_t ) mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS; x++ )
                    {
                        if( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xInUse == eMQTTFalse )
                        {
                            /* Found a free entry. Mark it as used and
                             * store the subscription. */
                            pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xInUse = eMQTTTrue;

                            /* Store the subscription. */
                            memcpy( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].ucTopicFilter,
                                    pucTopic,
                                    usTopicLength );
                            pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].usTopicFilterLength = usTopicLength;
                            pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].pvPublishCallbackContext = pvPublishCallbackContext;
                            pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].pxPublishCallback = pxPublishCallback;
                            pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xTopicFilterType = xTopicFilterType;

                            /* Increase the in-use subscription entries count. */
                            pxMQTTContext->xSubscriptionManager.ulInUseSubscriptions += ( uint32_t ) 1;

                            /* Inform the user that the subscription was stored
                             * successfully. */
                            xSubscriptionStored = eMQTTTrue;

                            /* Done. */
                            break;
                        }
                    }
                }
                else
                {
                    /* The provided topic filter is invalid. */
                    mqttconfigDEBUG_LOG( ( "WARN: The topic filter is invalid.\r\n" ) );
                }
            }
            else
            {
                /* Topic too long. */
                mqttconfigDEBUG_LOG( ( "WARN: Topic is too long and cannot be stored in the subscription manager. Consider increasing mqttconfigSUBSCRIPTION_MANAGER_MAX_TOPIC_LENGTH.\r\n" ) );
            }
        }
        else
        {
            /* Subscription Manager full. */
            mqttconfigDEBUG_LOG( ( "WARN: Subscription Manager full! No space left to store new subscriptions.\r\n" ) );
        }

        return xSubscriptionStored;
    }

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
/*-----------------------------------------------------------*/

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static void prvRemoveSubscription( MQTTContext_t * pxMQTTContext,
                                       const uint8_t * const pucTopic,
                                       uint16_t usTopicLength )
    {
        uint32_t x;

        /* Iterate over all the subscription entries in
         * the subscription manager and try to find the
         * matching one. */
        for( x = 0; x < ( uint32_t ) mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS; x++ )
        {
            if( ( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xInUse == eMQTTTrue ) &&
                ( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].usTopicFilterLength == usTopicLength ) )
            {
                if( memcmp( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].ucTopicFilter, pucTopic, usTopicLength ) == 0 )
                {
                    /* Found a matching subscription, mark it as free. */
                    pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xInUse = eMQTTFalse;

                    /* Reduce the count of in-use subscription entries
                     * in the subscription manager. */
                    pxMQTTContext->xSubscriptionManager.ulInUseSubscriptions -= ( uint32_t ) 1;

                    /* Done. */
                    break;
                }
            }
        }
    }

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
/*-----------------------------------------------------------*/

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static void prvRemoveSubscriptionForSubscribeOrUnsubscribeBuffer( MQTTContext_t * pxMQTTContext,
                                                                      MQTTBufferHandle_t xBuffer )
    {
        uint8_t ucRemaingingLengthFieldBytes;
        uint16_t usTopicLength;

        /* Get the number of bytes "Remaining Length" field spans
         * from the subscribe or unsubscribe Tx buffer. */
        ucRemaingingLengthFieldBytes = prvDecodeRemainingLength( &( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_REMAINING_LENGTH_OFFSET ] ), NULL );

        /* We must be able to successfully decode the remaining length
         * as this MQTT packet was constructed by us. */
        mqttconfigASSERT( ucRemaingingLengthFieldBytes > 0 );

        /* Read the topic length which precedes the topic name
         * in the MQTT subscribe packet. Note that the topic
         * happens to be at the same offset in both subscribe
         * and unsubscribe message and therefore there is no need
         * to repeat the same code with different #defines. */
        usTopicLength = ( uint8_t ) ( mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttSUBSCRIBE_TOPIC_OFFSET,
                                                                                        ucRemaingingLengthFieldBytes ) ] );
        usTopicLength <<= mqttBITS_PER_BYTE;
        usTopicLength |= ( uint8_t ) ( mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( ( mqttSUBSCRIBE_TOPIC_OFFSET + 1 ),
                                                                                         ucRemaingingLengthFieldBytes ) ] );

        /* Remove the subscription entry from the subscription manager. */
        prvRemoveSubscription( pxMQTTContext,
                               &( mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( ( mqttSUBSCRIBE_TOPIC_OFFSET + 2 ), ucRemaingingLengthFieldBytes ) ] ),
                               usTopicLength );
    }
#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
/*-----------------------------------------------------------*/

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static MQTTBool_t prvInvokeSubscriptionCallbacks( MQTTContext_t * pxMQTTContext,
                                                      const MQTTPublishData_t * pxPublishData,
                                                      MQTTBool_t * pxSubscriptionCallbackInvoked )
    {
        MQTTBool_t xBufferOwnershipTaken = eMQTTFalse;
        MQTTSubscription_t * pxSubscription;
        uint32_t x;

        /* Set the output parameter to eMQTTFalse. It will
         * be set to eMQTTTrue if any callback is invoked. */
        *pxSubscriptionCallbackInvoked = eMQTTFalse;

        /* Iterate over the subscription entries containing topic filters
         * without any wild-cards and invoke the registered callbacks. */
        for( x = 0; x < ( uint32_t ) mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS; x++ )
        {
            if( ( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xInUse == eMQTTTrue ) &&
                ( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xTopicFilterType == eMQTTTopicFilterTypeSimple ) &&
                ( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].usTopicFilterLength == pxPublishData->usTopicLength ) )
            {
                if( memcmp( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].ucTopicFilter, pxPublishData->pucTopic, pxPublishData->usTopicLength ) == 0 )
                {
                    /* Found a matching subscription. */
                    pxSubscription = &( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ] );

                    /* If a callback is registered with the subscription,
                     * invoke it. */
                    if( pxSubscription->pxPublishCallback != NULL )
                    {
                        /* Note that a callback was invoked. */
                        *pxSubscriptionCallbackInvoked = eMQTTTrue;

                        /* Invoke callback. */
                        xBufferOwnershipTaken = pxSubscription->pxPublishCallback( pxSubscription->pvPublishCallbackContext, pxPublishData );

                        /* If the user takes the buffer ownership, do
                         * not invoke any other callbacks. */
                        if( xBufferOwnershipTaken == eMQTTTrue )
                        {
                            break;
                        }
                    }
                }
            }
        }

        /* If the user has not taken the buffer ownership yet (which can
         * happen if there is no exact matching entry in the subscription
         * manager or the user does not take the ownership in the callback),
         * iterate over the subscription entries containing topic filters
         * with wild-cards and invoke the registered callbacks for the ones
         * which match the topic. */
        if( xBufferOwnershipTaken == eMQTTFalse )
        {
            for( x = 0; x < ( uint32_t ) mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS; x++ )
            {
                if( ( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xInUse == eMQTTTrue ) &&
                    ( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xTopicFilterType == eMQTTTopicFilterTypeWildCard ) )
                {
                    if( prvDoesTopicMatchTopicFilter( pxPublishData->pucTopic,
                                                      pxPublishData->usTopicLength,
                                                      pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].ucTopicFilter,
                                                      pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].usTopicFilterLength ) == eMQTTTrue )
                    {
                        /* Found a matching subscription. */
                        pxSubscription = &( pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ] );

                        /* If a callback is registered with the subscription,
                         * invoke it. */
                        if( pxSubscription->pxPublishCallback != NULL )
                        {
                            /* Note that a callback was invoked. */
                            *pxSubscriptionCallbackInvoked = eMQTTTrue;

                            /* Invoke callback. */
                            xBufferOwnershipTaken = pxSubscription->pxPublishCallback( pxSubscription->pvPublishCallbackContext, pxPublishData );

                            /* If the user takes the buffer ownership, do
                             * not invoke any other callbacks. */
                            if( xBufferOwnershipTaken == eMQTTTrue )
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }

        /* Return whether or not the user has taken the
         * ownership of the MQTT buffer. */
        return xBufferOwnershipTaken;
    }

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
/*-----------------------------------------------------------*/

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static MQTTTopicFilterType_t prvGetTopicFilterType( const uint8_t * const pucTopicFilter,
                                                        uint16_t usTopicFilterLength )
    {
        MQTTTopicFilterType_t xTopicFilterType = eMQTTTopicFilterTypeSimple;
        uint16_t x;

        /* Iterate over the topic filter. */
        for( x = 0; x < usTopicFilterLength; x++ )
        {
            if( pucTopicFilter[ x ] == ( uint8_t ) '+' )
            {
                /* The topic filter contains wild-card. */
                xTopicFilterType = eMQTTTopicFilterTypeWildCard;

                /* Either '+' should be the first character or the
                 * preceding character should be '/'. */
                if( ( x == ( uint16_t ) 0 ) || ( pucTopicFilter[ x - ( uint16_t ) 1 ] == ( uint8_t ) '/' ) )
                {
                    /* Either '+' should be the last character or the
                     * succeeding character should be '/'. */
                    if( ( x == usTopicFilterLength - ( uint16_t ) 1 ) || ( pucTopicFilter[ x + ( uint16_t ) 1 ] == ( uint8_t ) '/' ) )
                    {
                        /* This '+' character in the given topic
                         * filter is valid. */
                        continue;
                    }
                    else
                    {
                        /* If '+' is not the last character and the succeeding
                         * character is not '/', then it is not a valid topic
                         * filter. */
                        xTopicFilterType = eMQTTTopicFilterTypeInvalid;
                        break;
                    }
                }
                else
                {
                    /* If '+' is not the first character and the preceding
                     * character is not '/', then it is not a valid topic
                     * filter. */
                    xTopicFilterType = eMQTTTopicFilterTypeInvalid;
                    break;
                }
            }
            else if( pucTopicFilter[ x ] == ( uint8_t ) '#' )
            {
                /* The topic filter contains wild-card. */
                xTopicFilterType = eMQTTTopicFilterTypeWildCard;

                /* Either '#' should be the first character or the
                 * preceding character must be '/'. */
                if( ( x == ( uint16_t ) 0 ) || ( pucTopicFilter[ x - ( uint16_t ) 1 ] == ( uint8_t ) '/' ) )
                {
                    /* '#' must be the last character in the topic
                     * string. */
                    if( x == usTopicFilterLength - ( uint16_t ) 1 )
                    {
                        /* This '#' character in the given topic filter
                         * is valid. */
                        break;
                    }
                    else
                    {
                        /* If '#' is not the last character, then it is
                         * not a valid topic filter. */
                        xTopicFilterType = eMQTTTopicFilterTypeInvalid;
                        break;
                    }
                }
                else
                {
                    /* If '#' is not the first character and the preceding
                     * character is not '/', then it is not a valid topic
                     * filter. */
                    xTopicFilterType = eMQTTTopicFilterTypeInvalid;
                    break;
                }
            }
            else
            {
                /* Any character other than '+' and '#' is okay. */
            }
        }

        return xTopicFilterType;
    }

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
/*-----------------------------------------------------------*/

#if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

    static MQTTBool_t prvDoesTopicMatchTopicFilter( const uint8_t * const pucTopic,
                                                    uint16_t usTopicLength,
                                                    const uint8_t * const pucTopicFilter,
                                                    uint16_t usTopicFilterLength )
    {
        MQTTBool_t xTopicMatchesTopicFilter = eMQTTFalse;
        uint16_t usTopicIndex = 0, usTopicFilterIndex = 0;

        while( ( usTopicIndex < usTopicLength ) && ( usTopicFilterIndex < usTopicFilterLength ) )
        {
            /* Does the character in the topic string matches the
             * corresponding one in the topic filter string? */
            if( pucTopic[ usTopicIndex ] == pucTopicFilter[ usTopicFilterIndex ] )
            {
                /* Handle special corner cases as documented by
                 * the MQTT protocol spec. */

                /* Filter of type "sport/#" also matches the singular
                 * "sport" since # includes the parent level. */
                if( ( usTopicIndex == usTopicLength - ( uint16_t ) 1 ) &&
                    ( usTopicFilterIndex == usTopicFilterLength - ( uint16_t ) 3 ) &&
                    ( pucTopicFilter[ usTopicFilterIndex + ( uint16_t ) 1 ] == ( uint8_t ) '/' ) &&
                    ( pucTopicFilter[ usTopicFilterIndex + ( uint16_t ) 2 ] == ( uint8_t ) '#' ) )
                {
                    xTopicMatchesTopicFilter = eMQTTTrue;
                    break;
                }

                /* Filter of type "sport/+" also matches the "sport/"
                 * but not "sport". */
                if( ( usTopicIndex == usTopicLength - ( uint16_t ) 1 ) &&
                    ( usTopicFilterIndex == usTopicFilterLength - ( uint16_t ) 2 ) &&
                    ( pucTopicFilter[ usTopicFilterIndex + ( uint16_t ) 1 ] == ( uint8_t ) '+' ) )
                {
                    xTopicMatchesTopicFilter = eMQTTTrue;
                    break;
                }
            }
            else
            {
                if( pucTopicFilter[ usTopicFilterIndex ] == ( uint8_t ) '+' )
                {
                    /* Consume the current topic level which is until
                     * we encounter '/' in the topic string or the end
                     * of the topic string. */
                    while( usTopicIndex < usTopicLength && pucTopic[ usTopicIndex ] != ( uint8_t ) '/' )
                    {
                        usTopicIndex++;
                    }

                    /* Since we are already at the '/' character or at
                     * the end in the topic string, increment the topic
                     * filter index here and skip the index increments
                     * at the end of the loop. */
                    usTopicFilterIndex++;
                    continue;
                }
                else if( pucTopicFilter[ usTopicFilterIndex ] == ( uint8_t ) '#' )
                {
                    /* Since it is assumed that the given topic filter
                     * is a valid topic filter, this must be the last
                     * character in it. The rest of the topic string
                     * matches this multi-level wild-card and hence this
                     * is a match. */
                    xTopicMatchesTopicFilter = eMQTTTrue;
                    break;
                }
                else
                {
                    /* Any character mismatch other than '+' or
                     * '#' means the topic does not match the
                     * given filter. */
                    break;
                }
            }

            /* Increment indexes. */
            usTopicIndex++;
            usTopicFilterIndex++;
        }

        /* If the topic is not matched yet and both topic
         * and topic filter strings are consumed, it's a match
         * else we would have broken the loop in between.*/
        if( ( xTopicMatchesTopicFilter == eMQTTFalse ) &&
            ( ( usTopicIndex > ( uint16_t ) 0 ) && ( usTopicIndex >= usTopicLength ) ) &&
            ( ( usTopicFilterIndex > ( uint16_t ) 0 ) && ( usTopicFilterIndex >= usTopicFilterLength ) ) )
        {
            xTopicMatchesTopicFilter = eMQTTTrue;
        }

        /* Return the result. */
        return xTopicMatchesTopicFilter;
    }

#endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
/*-----------------------------------------------------------*/

MQTTReturnCode_t MQTT_Init( MQTTContext_t * pxMQTTContext,
                            const MQTTInitParams_t * const pxInitParams )
{
    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )
        uint32_t x;
    #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

    /* These are checked here once and are later used without
     * NULL checks. */
    mqttconfigASSERT( pxMQTTContext != NULL );
    mqttconfigASSERT( pxInitParams->pxMQTTSendFxn != NULL );
    mqttconfigASSERT( pxInitParams->xBufferPoolInterface.pxGetBufferFxn != NULL );
    mqttconfigASSERT( pxInitParams->xBufferPoolInterface.pxReturnBufferFxn != NULL );
    mqttconfigASSERT( pxInitParams != NULL );

    /* Initialize Tx Buffer List. */
    listINIT_HEAD( &( pxMQTTContext->xTxBufferListHead ) );

    /* Initialize Rx State. */
    prvResetRxMessageState( pxMQTTContext );

    /* Set connection state to not connected. */
    pxMQTTContext->xConnectionState = eMQTTNotConnected;

    /* Store callback context and function. */
    pxMQTTContext->pvCallbackContext = pxInitParams->pvCallbackContext;
    pxMQTTContext->pxCallback = pxInitParams->pxCallback;

    /* Store send context and function. */
    pxMQTTContext->pvSendContext = pxInitParams->pvSendContext;
    pxMQTTContext->pxMQTTSendFxn = pxInitParams->pxMQTTSendFxn;

    /* Store get ticks function. */
    pxMQTTContext->pxGetTicksFxn = pxInitParams->pxGetTicksFxn;

    /* Store buffer pool interface. */
    pxMQTTContext->xBufferPoolInterface = pxInitParams->xBufferPoolInterface;

    #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

        /* Mark all the subscription entires in the subscription
         * manager as free. */
        for( x = 0; x < ( uint32_t ) mqttconfigSUBSCRIPTION_MANAGER_MAX_SUBSCRIPTIONS; x++ )
        {
            pxMQTTContext->xSubscriptionManager.xSubscriptions[ x ].xInUse = eMQTTFalse;
        }

        /* Set the number of in-use subscription entries to zero. */
        pxMQTTContext->xSubscriptionManager.ulInUseSubscriptions = 0;
    #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

    return eMQTTSuccess;
}
/*-----------------------------------------------------------*/

MQTTReturnCode_t MQTT_Connect( MQTTContext_t * pxMQTTContext,
                               const MQTTConnectParams_t * const pxConnectParams )
{
    uint8_t * pucNextByte, * pucLastByteInBuffer, ucRemainingLengthFieldBytes;
    uint32_t ulRemainingLength, ulTotalMessageLength = 0;
    uint16_t usClientIdLength, usUserNameLength, usPasswordLength;
    MQTTBufferHandle_t xBuffer = NULL;
    MQTTReturnCode_t xReturnCode = eMQTTFailure;
    static const uint8_t ucDefaultConnectVariableHeader[] =
    {
        0,                              /* Protocol name length MSB. */
        4,                              /* Protocol name length LSB. */
        ( uint8_t ) 'M',                /* Protocol name byte 0. */
        ( uint8_t ) 'Q',                /* Protocol name byte 1. */
        ( uint8_t ) 'T',                /* Protocol name byte 2. */
        ( uint8_t ) 'T',                /* Protocol name byte 3. */
        mqttPROTOCOL_LEVEL,             /* Protocol level. */
        (mqttCONNECT_CLEAN_SESSION_FLAG | mqttCONNECT_USER_NAME_FLAG), /* AWS IoT Broker does not support persistent sessions i.e. Clean Session flag must be set. */
        ( uint8_t ) 0,                  /* Keep-alive time in seconds MSB. */
        ( uint8_t ) 0,                  /* Keep-alive time in seconds LSB. */
    };

    /* These are checked here once and are later used without
     * NULL checks. */
    mqttconfigASSERT( pxMQTTContext != NULL );
    mqttconfigASSERT( pxMQTTContext->pxMQTTSendFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxGetBufferFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxReturnBufferFxn != NULL );
    mqttconfigASSERT( pxConnectParams != NULL );

    /* Connect should only be sent in the "not connected" state. */
    if( pxMQTTContext->xConnectionState != eMQTTNotConnected )
    {
        /* Fail the connect operation immediately, if the
         * MQTT client is already connected or connection is in
         * progress. */
        xReturnCode = ( pxMQTTContext->xConnectionState == eMQTTConnected ) ? eMQTTClientAlreadyConnected : eMQTTClientConnectionInProgress;
    }
    else
    {
        /* Store keep alive actual interval and timeout. */
        pxMQTTContext->ulKeepAliveActualIntervalTicks = pxConnectParams->ulKeepAliveActualIntervalTicks;
        pxMQTTContext->ulPingRequestTimeoutTicks = pxConnectParams->ulPingRequestTimeoutTicks;

        /* Client ID and username length. */
        usClientIdLength = mqttSTRLEN( pxConnectParams->usClientIdLength );
        usUserNameLength = pxConnectParams->usUserNameLength > ( uint16_t ) 0 ? mqttSTRLEN( pxConnectParams->usUserNameLength ) : ( uint16_t ) 0;
        usPasswordLength = pxConnectParams->usPasswordLength > ( uint16_t ) 0 ? mqttSTRLEN( pxConnectParams->usPasswordLength ) : ( uint16_t ) 0;

        /* Calculate "Remaining Length" i.e. length of the packet excluding Fixed Header. */
        ulRemainingLength = ( uint32_t ) sizeof( ucDefaultConnectVariableHeader ) +
                            ( uint32_t ) usClientIdLength +
                            ( uint32_t ) usUserNameLength +
                            ( uint32_t ) usPasswordLength;

        /* Calculate the number of bytes occupied by the "Remaining Length" field. */
        ucRemainingLengthFieldBytes = prvSizeOfRemainingLength( ulRemainingLength );

        /* Make sure that "Remaining Length" is within the permissible limits. */
        if( ucRemainingLengthFieldBytes > ( uint8_t ) 0 )
        {
            /* Calculate total MQTT message length. */
            ulTotalMessageLength = mqttTOTAL_MESSAGE_LENGTH( ucRemainingLengthFieldBytes, ulRemainingLength );

            /* Try to get a buffer from the free buffer pool. */
            xBuffer = prvGetFreeBuffer( pxMQTTContext, ulTotalMessageLength );

            if( xBuffer == NULL )
            {
                /* Fail the connect operation immediately, if no free
                 * buffer is available. */
                mqttconfigDEBUG_LOG( ( "No free buffer is available to carry out the operation. \r\n" ) );
                xReturnCode = eMQTTNoFreeBuffer;
            }
            else
            {
                /* Add the buffer to the Tx buffer list. */
                mqttbufferLIST_ADD( &( pxMQTTContext->xTxBufferListHead ), xBuffer );

                /* To help debugging only. */
                memset( mqttbufferGET_DATA( xBuffer ), 0x00, mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBuffer ) );

                /* Record time-stamp and store timeout. */
                mqttbufferGET_PACKET_RECORDED_TICK_COUNT( xBuffer ) = prvGetCurrentTickCount( pxMQTTContext );
                mqttbufferGET_PACKET_TIMEOUT_TICKS( xBuffer ) = pxConnectParams->ulTimeoutTicks;

                /* Write Fixed Header control packet type. */
                mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] = mqttCONTROL_CONNECT | mqttFLAGS_CONNECT;

                /* Write encoded "Remaining Length" in the fixed header. */
                pucNextByte = &( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_REMAINING_LENGTH_OFFSET ] );
                pucLastByteInBuffer = &( mqttbufferGET_DATA( xBuffer )[ mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBuffer ) - ( uint32_t ) 1 ] );
                ucRemainingLengthFieldBytes = prvEncodeRemainingLength( ulRemainingLength, pucNextByte, pucLastByteInBuffer );

                /* We should have successfully encoded the remaining length field
                 * as we already have a large enough buffer. */
                mqttconfigASSERT( ucRemainingLengthFieldBytes == prvSizeOfRemainingLength( ulRemainingLength ) );

                /* Write default variable header. */
                pucNextByte = &( mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttVARIABLE_LENGTH_HEADER_START_OFFSET, ucRemainingLengthFieldBytes ) ] );
                memcpy( pucNextByte, ucDefaultConnectVariableHeader, sizeof( ucDefaultConnectVariableHeader ) );

                if(pxConnectParams->usPasswordLength > ( uint16_t ) 0)
                {
                    pucNextByte[7] |= mqttCONNECT_PASSWORD_FLAG;
                }


                /* Update the user name flag. */
                if( pxConnectParams->usUserNameLength > ( uint16_t ) 0 )
                {
                    mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttCONNECT_FLAGS_OFFSET, ucRemainingLengthFieldBytes ) ] |= mqttCONNECT_USER_NAME_FLAG;
                }

                /* Update the password flag. */
                if( pxConnectParams->usPasswordLength > ( uint16_t ) 0 )
                {
                    mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttCONNECT_FLAGS_OFFSET, ucRemainingLengthFieldBytes ) ] |= mqttCONNECT_PASSWORD_FLAG;
                }

                /* Update keep alive timeout. */
                mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttCONNECT_KEEPALIVE_MSB_OFFSET,
                                                                  ucRemainingLengthFieldBytes ) ] = ( uint8_t ) ( pxConnectParams->usKeepAliveIntervalSeconds >> mqttBITS_PER_BYTE );
                mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttCONNECT_KEEPALIVE_LSB_OFFSET,
                                                                  ucRemainingLengthFieldBytes ) ] = ( uint8_t ) ( pxConnectParams->usKeepAliveIntervalSeconds );

                /* Write the client ID into the payload. */
                pucNextByte = &( mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttCONNECT_CLIENT_ID_OFFSET, ucRemainingLengthFieldBytes ) ] );
                pucNextByte = prvWriteString( pucNextByte, pucLastByteInBuffer, pxConnectParams->pucClientId, pxConnectParams->usClientIdLength );

                /* Write the user name into the payload. */
                if( pxConnectParams->usUserNameLength > ( uint16_t ) 0 )
                {
                    pucNextByte = prvWriteString( pucNextByte, pucLastByteInBuffer, pxConnectParams->pucUserName, pxConnectParams->usUserNameLength );
                }

                /* Write the password into the payload. */
                if( pxConnectParams->usPasswordLength > ( uint16_t ) 0 )
                {
                    pucNextByte = prvWriteString( pucNextByte, pucLastByteInBuffer, (const uint8_t*)pxConnectParams->pucPassword, pxConnectParams->usPasswordLength );
                }

                /* MISRA compliance. */
                ( void ) pucNextByte;

                /* Store the packet identifier in TxBuffer also for matching
                 * ACK later. */
                mqttbufferGET_PACKET_IDENTIFIER( xBuffer ) = pxConnectParams->usPacketIdentifier;

                /* Update the number of bytes written to the buffer. */
                mqttbufferGET_DATA_LENGTH( xBuffer ) = ulTotalMessageLength;

                /* MQTT packet created. */
                xReturnCode = eMQTTSuccess;
            }
        }
    }

    /* If the packet was successfully constructed, transmit it. */
    if( xReturnCode == eMQTTSuccess )
    {
        xReturnCode = prvSendData( pxMQTTContext, mqttbufferGET_DATA( xBuffer ), mqttbufferGET_DATA_LENGTH( xBuffer ) );
    }

    /* If some error occurred, return the TxBuffer, otherwise it
     * will be returned upon receiving ACK or timeout. */
    if( xReturnCode != eMQTTSuccess )
    {
        /* Return the buffer to the free buffer pool. */
        prvReturnBuffer( pxMQTTContext, xBuffer );
    }
    else
    {
        /* Connect has been sent and we are waiting for CONNACK. */
        pxMQTTContext->xConnectionState = eMQTTConnectionInProgress;
    }

    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTReturnCode_t MQTT_Disconnect( MQTTContext_t * pxMQTTContext )
{
    MQTTReturnCode_t xReturnCode = eMQTTSuccess;
    MQTTEventCallbackParams_t xEventCallbackParams;
    static const uint8_t ucDisconnectPacket[] =
    {
        mqttCONTROL_DISCONNECT | mqttFLAGS_DISCONNECT,
        0 /* DISCONNECT packet does not have any variable header or payload. */
    };

    /* These are checked here once and are later used without
     * NULL checks. */
    mqttconfigASSERT( pxMQTTContext != NULL );
    mqttconfigASSERT( pxMQTTContext->pxMQTTSendFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxGetBufferFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxReturnBufferFxn != NULL );

    /* Send the MQTT Disconnect message only if the MQTT client
     * is connected or a connection attempt is in progress (i.e.
     * we have sent connect message and are waiting for a CONNACK).
     * Do not send the disconnect message if someone is trying to
     * disconnect an already disconnected client. */
    if( ( pxMQTTContext->xConnectionState == eMQTTConnected ) || ( pxMQTTContext->xConnectionState == eMQTTConnectionInProgress ) )
    {
        if( prvSendData( pxMQTTContext, ucDisconnectPacket, sizeof( ucDisconnectPacket ) ) != eMQTTSuccess )
        {
            xReturnCode = eMQTTSendFailed;
        }

        /* Inform the user about disconnect. */
        xEventCallbackParams.xEventType = eMQTTClientDisconnected;
        xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonUserRequest;
        ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );
    }

    /* Reset the internal state even if we fail to send the MQTT
     * message. */
    prvResetMQTTContext( pxMQTTContext );

    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTReturnCode_t MQTT_Subscribe( MQTTContext_t * pxMQTTContext,
                                 const MQTTSubscribeParams_t * const pxSubscribeParams )
{
    uint8_t * pucNextByte, * pucLastByteInBuffer, ucRemainingLengthFieldBytes;
    uint32_t ulRemainingLength, ulTotalMessageLength;
    uint16_t usTopicLength;
    MQTTBufferHandle_t xBuffer = NULL;
    MQTTReturnCode_t xReturnCode = eMQTTFailure;

    /* These are checked here once and are later used without
     * NULL checks. */
    mqttconfigASSERT( pxMQTTContext != NULL );
    mqttconfigASSERT( pxMQTTContext->pxMQTTSendFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxGetBufferFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxReturnBufferFxn != NULL );
    mqttconfigASSERT( pxSubscribeParams != NULL );
    mqttconfigASSERT( pxSubscribeParams->pucTopic != NULL );
    mqttconfigASSERT( pxSubscribeParams->xQos != eMQTTQoS2 ); /* QoS2 is not supported. */

    mqttconfigDEBUG_LOG( ( "Initiating MQTT subscribe.\r\n" ) );

    if( pxMQTTContext->xConnectionState != eMQTTConnected )
    {
        /* Fail the subscribe operation immediately, if
         * MQTT client is not connected. */
        xReturnCode = eMQTTClientNotConnected;
    }
    else
    {
        #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

            /* Try to store the subscription in the subscription
             * manager. */
            if( prvStoreSubscription( pxMQTTContext,
                                      pxSubscribeParams->pucTopic,
                                      pxSubscribeParams->usTopicLength,
                                      pxSubscribeParams->pvPublishCallbackContext,
                                      pxSubscribeParams->pxPublishCallback ) == eMQTTFalse )
            {
                /* Fail the subscribe operation immediately, if we
                 * fail to store the subscription in the subscription
                 * manager. */
                xReturnCode = eMQTTSubscriptionManagerFull;
            }
            else
        #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
        {
            /* Length of the topic in the actual MQTT message. */
            usTopicLength = mqttSTRLEN( pxSubscribeParams->usTopicLength );

            /* Calculate the "Remaining Length" i.e. length of the packet
             * excluding fixed header. */
            ulRemainingLength = ( uint32_t ) mqttSUBSCRIBE_PACKET_IDENTIFER_LENGTH +
                                ( uint32_t ) usTopicLength +
                                ( uint32_t ) mqttSUBSCRIBE_REQUESTED_QOS_LENGTH;

            /* Calculate the number of bytes occupied by the "Remaining Length" field. */
            ucRemainingLengthFieldBytes = prvSizeOfRemainingLength( ulRemainingLength );

            /* Make sure that "Remaining Length" is within the permissible limits. */
            if( ucRemainingLengthFieldBytes > ( uint8_t ) 0 )
            {
                /* Calculate total MQTT message length. */
                ulTotalMessageLength = mqttTOTAL_MESSAGE_LENGTH( ucRemainingLengthFieldBytes, ulRemainingLength );

                /* Try to get a buffer from the free buffer pool. */
                xBuffer = prvGetFreeBuffer( pxMQTTContext, ulTotalMessageLength );

                if( xBuffer == NULL )
                {
                    /* Fail the subscribe operation immediately, if
                     * no free buffer is available. */
                    mqttconfigDEBUG_LOG( ( "No free buffer is available to carry out the operation. \r\n" ) );
                    xReturnCode = eMQTTNoFreeBuffer;
                }
                else
                {
                    /* Add the buffer to the Tx buffer list. */
                    mqttbufferLIST_ADD( &( pxMQTTContext->xTxBufferListHead ), xBuffer );

                    /* To help debugging only. */
                    memset( mqttbufferGET_DATA( xBuffer ), 0x00, mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBuffer ) );

                    /* Record time-stamp and store timeout. */
                    mqttbufferGET_PACKET_RECORDED_TICK_COUNT( xBuffer ) = prvGetCurrentTickCount( pxMQTTContext );
                    mqttbufferGET_PACKET_TIMEOUT_TICKS( xBuffer ) = pxSubscribeParams->ulTimeoutTicks;

                    /* Write the Fixed Header control packet type and flags. */
                    mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] = mqttCONTROL_SUBSCRIBE | mqttFLAGS_SUBSCRIBE;

                    /* Write the encoded remaining length in the fixed header. */
                    pucNextByte = &( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_REMAINING_LENGTH_OFFSET ] );
                    pucLastByteInBuffer = &( mqttbufferGET_DATA( xBuffer )[ mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBuffer ) - ( uint32_t ) 1 ] );
                    ucRemainingLengthFieldBytes = prvEncodeRemainingLength( ulRemainingLength, pucNextByte, pucLastByteInBuffer );

                    /* We should have successfully encoded the remaining length field
                     * as we already have a large enough buffer. */
                    mqttconfigASSERT( ucRemainingLengthFieldBytes == prvSizeOfRemainingLength( ulRemainingLength ) );

                    /* Write variable header which happens to contain only
                     * packet identifier in this case. */
                    mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttSUBSCRIBE_PACKET_ID_MSB_OFFSET,
                                                                      ucRemainingLengthFieldBytes ) ] = ( uint8_t ) ( pxSubscribeParams->usPacketIdentifier >> mqttBITS_PER_BYTE );
                    mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttSUBSCRIBE_PACKET_ID_LSB_OFFSET,
                                                                      ucRemainingLengthFieldBytes ) ] = ( uint8_t ) ( pxSubscribeParams->usPacketIdentifier );

                    /* Write the topic into the message. */
                    pucNextByte = &( mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttSUBSCRIBE_TOPIC_OFFSET, ucRemainingLengthFieldBytes ) ] );
                    pucNextByte = prvWriteString( pucNextByte, pucLastByteInBuffer, pxSubscribeParams->pucTopic, pxSubscribeParams->usTopicLength );

                    /* Write the Requested QoS into the message. */
                    *pucNextByte = ( uint8_t ) pxSubscribeParams->xQos;

                    /* Store the packet identifier in TxBuffer also for matching with
                     * the one received in ACK later. */
                    mqttbufferGET_PACKET_IDENTIFIER( xBuffer ) = pxSubscribeParams->usPacketIdentifier;

                    /* Update the number of bytes written to the buffer. */
                    mqttbufferGET_DATA_LENGTH( xBuffer ) = ulTotalMessageLength;

                    /* MQTT packet created. */
                    xReturnCode = eMQTTSuccess;
                }
            }
        }
    }

    /* If the packet was successfully constructed, transmit it. */
    if( xReturnCode == eMQTTSuccess )
    {
        xReturnCode = prvSendData( pxMQTTContext, mqttbufferGET_DATA( xBuffer ), mqttbufferGET_DATA_LENGTH( xBuffer ) );
    }

    /* If some error occurred, return the buffer, otherwise it
     * will be returned upon receiving ACK or timeout. Also,
     * remove the subscription entry from the subscription
     * manager. */
    if( xReturnCode != eMQTTSuccess )
    {
        /* Return the buffer to the free buffer pool. */
        prvReturnBuffer( pxMQTTContext, xBuffer );

        #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

            /* Remove the subscription entry from the subscription manager.
             * This if check is an optimization - If the subscribe failed because
             * we could not store the subscription, there is not need to remove
             * it as it was never stored. */
            if( xReturnCode != eMQTTSubscriptionManagerFull )
            {
                prvRemoveSubscription( pxMQTTContext, pxSubscribeParams->pucTopic, pxSubscribeParams->usTopicLength );
            }
        #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */
    }

    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTReturnCode_t MQTT_Unsubscribe( MQTTContext_t * pxMQTTContext,
                                   const MQTTUnsubscribeParams_t * const pxUnsubscribeParams )
{
    uint8_t * pucNextByte, * pucLastByteInBuffer, ucRemainingLengthFieldBytes;
    uint32_t ulRemainingLength, ulTotalMessageLength;
    uint16_t usTopicLength;
    MQTTBufferHandle_t xBuffer = NULL;
    MQTTReturnCode_t xReturnCode = eMQTTFailure;

    /* These are checked here once and are later used without
     * NULL checks. */
    mqttconfigASSERT( pxMQTTContext != NULL );
    mqttconfigASSERT( pxMQTTContext->pxMQTTSendFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxGetBufferFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxReturnBufferFxn != NULL );
    mqttconfigASSERT( pxUnsubscribeParams != NULL );

    mqttconfigDEBUG_LOG( ( "Initiating MQTT unsubscribe.\r\n" ) );

    if( pxMQTTContext->xConnectionState != eMQTTConnected )
    {
        /* Fail the unsubscribe operation immediately, if
         * MQTT client is not connected. */
        xReturnCode = eMQTTClientNotConnected;
    }
    else
    {
        /* Length of the topic in the actual MQTT message. */
        usTopicLength = mqttSTRLEN( pxUnsubscribeParams->usTopicLength );

        /* Calculate the "Remaining Length" i.e. length of the packet
         * excluding fixed header. */
        ulRemainingLength = ( uint32_t ) mqttUNSUBSCRIBE_PACKET_IDENTIFER_LENGTH + ( uint32_t ) usTopicLength;

        /* Calculate the number of bytes occupied by the "Remaining Length" field. */
        ucRemainingLengthFieldBytes = prvSizeOfRemainingLength( ulRemainingLength );

        /* Make sure that "Remaining Length" is within the permissible limits. */
        if( ucRemainingLengthFieldBytes > ( uint8_t ) 0 )
        {
            /* Calculate total MQTT message length. */
            ulTotalMessageLength = mqttTOTAL_MESSAGE_LENGTH( ucRemainingLengthFieldBytes, ulRemainingLength );

            /* Try to get a buffer from the free buffer pool. */
            xBuffer = prvGetFreeBuffer( pxMQTTContext, ulTotalMessageLength );

            if( xBuffer == NULL )
            {
                /* Fail the unsubscribe operation immediately, if
                 * no free buffer is available. */
                mqttconfigDEBUG_LOG( ( "No free buffer is available to carry out the operation. \r\n" ) );
                xReturnCode = eMQTTNoFreeBuffer;
            }
            else
            {
                /* Add the buffer to the Tx buffer list. */
                mqttbufferLIST_ADD( &( pxMQTTContext->xTxBufferListHead ), xBuffer );

                /* To help debugging only. */
                memset( mqttbufferGET_DATA( xBuffer ), 0x00, mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBuffer ) );

                /* Record time-stamp and store timeout. */
                mqttbufferGET_PACKET_RECORDED_TICK_COUNT( xBuffer ) = prvGetCurrentTickCount( pxMQTTContext );
                mqttbufferGET_PACKET_TIMEOUT_TICKS( xBuffer ) = pxUnsubscribeParams->ulTimeoutTicks;

                /* Write the Fixed Header control packet type and flags. */
                mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] = mqttCONTROL_UNSUBSCRIBE | mqttFLAGS_UNSUBSCRIBE;

                /* Write the encoded remaining length in the fixed header. */
                pucNextByte = &( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_REMAINING_LENGTH_OFFSET ] );
                pucLastByteInBuffer = &( mqttbufferGET_DATA( xBuffer )[ mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBuffer ) - ( uint32_t ) 1 ] );
                ucRemainingLengthFieldBytes = prvEncodeRemainingLength( ulRemainingLength, pucNextByte, pucLastByteInBuffer );

                /* We should have successfully encoded the remaining length field
                 * as we already have a large enough buffer. */
                mqttconfigASSERT( ucRemainingLengthFieldBytes == prvSizeOfRemainingLength( ulRemainingLength ) );

                /* Write variable header which happens to contain only
                 * packet identifier in this case. */
                mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttUNSUBSCRIBE_PACKET_ID_MSB_OFFSET,
                                                                  ucRemainingLengthFieldBytes ) ] = ( uint8_t ) ( pxUnsubscribeParams->usPacketIdentifier >> mqttBITS_PER_BYTE );
                mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttUNSUBSCRIBE_PACKET_ID_LSB_OFFSET,
                                                                  ucRemainingLengthFieldBytes ) ] = ( uint8_t ) ( pxUnsubscribeParams->usPacketIdentifier );

                /* Write the topic into the message. */
                pucNextByte = &( mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttUNSUBSCRIBE_TOPIC_OFFSET, ucRemainingLengthFieldBytes ) ] );
                pucNextByte = prvWriteString( pucNextByte, pucLastByteInBuffer, pxUnsubscribeParams->pucTopic, pxUnsubscribeParams->usTopicLength );

                /* MISRA compliance. */
                ( void ) pucNextByte;

                /* Store the packet identifier in TxBuffer also for matching with
                 * the one received in ACK later. */
                mqttbufferGET_PACKET_IDENTIFIER( xBuffer ) = pxUnsubscribeParams->usPacketIdentifier;

                /* Update the number of bytes written to the buffer. */
                mqttbufferGET_DATA_LENGTH( xBuffer ) = ulTotalMessageLength;

                /* MQTT packet created. */
                xReturnCode = eMQTTSuccess;
            }
        }
    }

    /* If the packet was successfully constructed, transmit it. */
    if( xReturnCode == eMQTTSuccess )
    {
        xReturnCode = prvSendData( pxMQTTContext, mqttbufferGET_DATA( xBuffer ), mqttbufferGET_DATA_LENGTH( xBuffer ) );
    }

    /* If some error occurred, return the buffer, otherwise it
     * will be returned upon receiving ACK or timeout. */
    if( xReturnCode != eMQTTSuccess )
    {
        /* Return the buffer to the free buffer pool. */
        prvReturnBuffer( pxMQTTContext, xBuffer );
    }

    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTReturnCode_t MQTT_Publish( MQTTContext_t * pxMQTTContext,
                               const MQTTPublishParams_t * const pxPublishParams )
{
    uint8_t * pucNextByte, * pucLastByteInBuffer, ucRemainingLengthFieldBytes;
    uint32_t ulRemainingLength, ulTotalMessageLength;
    uint16_t usTopicLength;
    MQTTBufferHandle_t xBuffer = NULL;
    MQTTReturnCode_t xReturnCode = eMQTTFailure;

    /* These are checked here once and are later used without
     * NULL checks. */
    mqttconfigASSERT( pxMQTTContext != NULL );
    mqttconfigASSERT( pxMQTTContext->pxMQTTSendFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxGetBufferFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxReturnBufferFxn != NULL );
    mqttconfigASSERT( pxPublishParams != NULL );

    mqttconfigDEBUG_LOG( ( "Initiating MQTT publish.\r\n" ) );

    if( pxMQTTContext->xConnectionState != eMQTTConnected )
    {
        /* Fail the publish operation immediately, if
         * MQTT client is not connected. */
        xReturnCode = eMQTTClientNotConnected;
    }
    else
    {
        /* Length of the topic in the actual MQTT message. */
        usTopicLength = mqttSTRLEN( pxPublishParams->usTopicLength );

        /* Calculate the "Remaining Length" i.e. length of the packet excluding Fixed Header. */
        ulRemainingLength = ( uint32_t ) usTopicLength +
                            ( pxPublishParams->xQos == eMQTTQoS0 ? ( uint32_t ) mqttPUBLISH_QOS0_PACKET_IDENTIFER_LENGTH : ( uint32_t ) mqttPUBLISH_QOS1_PACKET_IDENTIFER_LENGTH ) +
                            pxPublishParams->ulDataLength;

        /* Calculate the number of bytes occupied by the "Remaining Length" field. */
        ucRemainingLengthFieldBytes = prvSizeOfRemainingLength( ulRemainingLength );

        /* Make sure that "Remaining Length" is within the permissible limits. */
        if( ucRemainingLengthFieldBytes > ( uint8_t ) 0 )
        {
            /* Calculate total MQTT message length. */
            ulTotalMessageLength = mqttTOTAL_MESSAGE_LENGTH( ucRemainingLengthFieldBytes, ulRemainingLength );

            /* Try to get a buffer from the free buffer pool. */
            xBuffer = prvGetFreeBuffer( pxMQTTContext, ulTotalMessageLength );

            if( xBuffer == NULL )
            {
                /* Fail the publish operation immediately, if
                 * no free buffer is available. */
                mqttconfigDEBUG_LOG( ( "No free buffer is available to carry out the operation. \r\n" ) );
                xReturnCode = eMQTTNoFreeBuffer;
            }
            else
            {
                /* Add the buffer to the Tx buffer list. */
                mqttbufferLIST_ADD( &( pxMQTTContext->xTxBufferListHead ), xBuffer );

                /* To help debugging only. */
                memset( mqttbufferGET_DATA( xBuffer ), 0x00, mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBuffer ) );

                /* Record time-stamp and store timeout. */
                mqttbufferGET_PACKET_RECORDED_TICK_COUNT( xBuffer ) = prvGetCurrentTickCount( pxMQTTContext );
                mqttbufferGET_PACKET_TIMEOUT_TICKS( xBuffer ) = pxPublishParams->ulTimeoutTicks;

                /* Write Control Packet Type. */
                /*_TODO_ Note!  DUP and RETAIN are all currently all set to 0. */
                mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] = mqttCONTROL_PUBLISH;

                /* Set QoS. QoS2 is not supported.*/
                mqttconfigASSERT( pxPublishParams->xQos == eMQTTQoS0 || pxPublishParams->xQos == eMQTTQoS1 );
                mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] |= ( ( ( uint8_t ) ( pxPublishParams->xQos ) ) << 1 );

                /* Write encoded "Remaining Length" in the fixed header. */
                pucNextByte = &( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_REMAINING_LENGTH_OFFSET ] );
                pucLastByteInBuffer = &( mqttbufferGET_DATA( xBuffer )[ mqttbufferGET_EFFECTIVE_BUFFER_LENGTH( xBuffer ) - ( uint32_t ) 1 ] );
                ucRemainingLengthFieldBytes = prvEncodeRemainingLength( ulRemainingLength, pucNextByte, pucLastByteInBuffer );

                /* We should have successfully encoded the remaining length field
                 * as we already have a large enough buffer. */
                mqttconfigASSERT( ucRemainingLengthFieldBytes == prvSizeOfRemainingLength( ulRemainingLength ) );

                /* Write the topic into the message (part of variable header). */
                pucNextByte = &( mqttbufferGET_DATA( xBuffer )[ mqttADJUST_OFFSET( mqttPUBLISH_TOPIC_OFFSET, ucRemainingLengthFieldBytes ) ] );
                pucNextByte = prvWriteString( pucNextByte, pucLastByteInBuffer, pxPublishParams->pucTopic, pxPublishParams->usTopicLength );

                /* Write packet identifier into the message, if it is not QoS0. */
                if( pxPublishParams->xQos != eMQTTQoS0 )
                {
                    /* Write MSB. */
                    *pucNextByte = ( uint8_t ) ( ( pxPublishParams->usPacketIdentifier ) >> mqttBITS_PER_BYTE );
                    pucNextByte++;

                    /* Write LSB. */
                    *pucNextByte = ( uint8_t ) ( pxPublishParams->usPacketIdentifier );
                    pucNextByte++;
                }

                /* Write the payload into the message. */
                memcpy( pucNextByte, pxPublishParams->pvData, ( size_t ) pxPublishParams->ulDataLength );

                /* Store the packet identifier in TxBuffer also for matching
                 * ACK later. */
                mqttbufferGET_PACKET_IDENTIFIER( xBuffer ) = pxPublishParams->usPacketIdentifier;

                /* Update the number of bytes written to the buffer. */
                mqttbufferGET_DATA_LENGTH( xBuffer ) = ulTotalMessageLength;

                /* MQTT packet created. */
                xReturnCode = eMQTTSuccess;
            }
        }
    }

    /* If the packet was successfully constructed, transmit it. */
    if( xReturnCode == eMQTTSuccess )
    {
        xReturnCode = prvSendData( pxMQTTContext, mqttbufferGET_DATA( xBuffer ), mqttbufferGET_DATA_LENGTH( xBuffer ) );
    }

    /* If some error occurred or QOS0 (No ACK is expected in case of QOS0),
     * return the buffer, otherwise it will be returned upon receiving ACK
     * or timeout. */
    if( ( xReturnCode != eMQTTSuccess ) || ( pxPublishParams->xQos == eMQTTQoS0 ) )
    {
        /* Return the buffer to the free buffer pool. */
        prvReturnBuffer( pxMQTTContext, xBuffer );
    }

    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTReturnCode_t MQTT_ParseReceivedData( MQTTContext_t * pxMQTTContext,
                                         const uint8_t * pucReceivedData,
                                         size_t xReceivedDataLength )
{
    MQTTReturnCode_t xReturnCode = eMQTTSuccess;
    MQTTEventCallbackParams_t xEventCallbackParams;
    size_t xProcessedBytes = 0, xExpectedBytes, xUnprocessedBytes;

    /* These are checked here once and are later used without
     * NULL checks. */
    mqttconfigASSERT( pxMQTTContext != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxGetBufferFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxReturnBufferFxn != NULL );
    mqttconfigASSERT( pucReceivedData != NULL );

    /* Keep processing until all the supplied bytes are over. */
    while( xProcessedBytes < xReceivedDataLength )
    {
        /* Do not process any data received on the client
         * which is not connected. This check needs to be in
         * the loop because one iteration of the loop can result
         * in client getting disconnected because of a malformed
         * packet. */
        if( pxMQTTContext->xConnectionState == eMQTTNotConnected )
        {
            xReturnCode = eMQTTClientNotConnected;

            /* Stop. */
            break;
        }

        if( pxMQTTContext->xRxMessageState.xRxNextByte == eMQTTRxNextBytePacketType )
        {
            /* Looking for the start of a new MQTT message, which always begins with
             * one byte containing packet type and related flags. */
            mqttconfigASSERT( pxMQTTContext->ulRxMessageReceivedLength == 0 );
            mqttconfigASSERT( pxMQTTContext->xRxBuffer == NULL );

            /* We always write the packet type and "Remaining Length" in the fixed
             * header buffer so that we can decode the packet length even if no user
             * supplied buffer is available. This enables us to drop a packet if no
             * free buffer is available because we know how many bytes to drop before
             * the next packet starts. */

            /* Copy one byte containing packet type and flags. */
            mqttCOPY_BYTES( pucReceivedData, xProcessedBytes, pxMQTTContext->ucRxFixedHeaderBuffer, pxMQTTContext->ulRxMessageReceivedLength, 1 );

            /* Next bytes will contain "Remaining Length" field. */
            pxMQTTContext->xRxMessageState.xRxNextByte = eMQTTRxNextBytePacketLength;
        }
        else if( pxMQTTContext->xRxMessageState.xRxNextByte == eMQTTRxNextBytePacketLength )
        {
            /* Receiving "Remaining Length" field which is part of fixed header. */
            mqttconfigASSERT( pxMQTTContext->ulRxMessageReceivedLength < mqttFIXED_HEADER_MAX_SIZE );

            mqttCOPY_BYTES( pucReceivedData, xProcessedBytes, pxMQTTContext->ucRxFixedHeaderBuffer, pxMQTTContext->ulRxMessageReceivedLength, 1 );

            /* Are there more bytes containing packet length i.e. is the "continuation bit"
             * set in the received length byte? */
            if( ( ( pxMQTTContext->ucRxFixedHeaderBuffer[ pxMQTTContext->ulRxMessageReceivedLength - ( uint32_t ) 1 ] ) & mqttREMAINING_LENGTH_CONTINUATION_BITMASK ) != ( uint8_t ) 0 )
            {
                /* "Remaining Length" field cannot span more than 4 bytes. */
                if( pxMQTTContext->ulRxMessageReceivedLength >= ( uint32_t ) mqttFIXED_HEADER_MAX_SIZE )
                {
                    mqttconfigDEBUG_LOG( ( "Packet with malformed \"Remaining Length\" received.\r\n" ) );

                    /* A malformed packet has been received - disconnect. */
                    prvResetMQTTContext( pxMQTTContext );

                    /* Inform user about the malformed packet received. */
                    xEventCallbackParams.xEventType = eMQTTClientDisconnected;
                    xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonMalformedPacket;
                    ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                    xReturnCode = eMQTTMalformedPacketReceived;
                    break;
                }
            }
            else
            {
                /* Packet length has been received, decode it. */
                pxMQTTContext->xRxMessageState.ucRemaingingLengthFieldBytes = prvDecodeRemainingLength( &( pxMQTTContext->ucRxFixedHeaderBuffer[ mqttFIXED_HEADER_REMAINING_LENGTH_OFFSET ] ),
                                                                                                        &( pxMQTTContext->xRxMessageState.ulTotalMessageLength ) );

                mqttconfigDEBUG_LOG( ( "Received fixed header, %d bytes to receive.\r\n", pxMQTTContext->xRxMessageState.ulTotalMessageLength ) );

                /* "Remaining Length" does not include the length of fixed
                 * header, so this needs to be added. */
                pxMQTTContext->xRxMessageState.ulTotalMessageLength += pxMQTTContext->ulRxMessageReceivedLength;

                /* Does the message contain anything after fixed header? */
                if( pxMQTTContext->xRxMessageState.ulTotalMessageLength == pxMQTTContext->ulRxMessageReceivedLength )
                {
                    /*..if not, process it. */
                    prvProcessReceivedFixedHeaderOnlyMQTTPacket( pxMQTTContext );

                    /* Complete message received, start looking for the start of
                     * the next. */
                    prvResetRxMessageState( pxMQTTContext );
                }
                else
                {
                    /* Get a buffer to store the received message. */
                    pxMQTTContext->xRxBuffer = prvGetFreeBuffer( pxMQTTContext, pxMQTTContext->xRxMessageState.ulTotalMessageLength );

                    /* If we got a large enough free buffer, store the rest of the message. */
                    if( pxMQTTContext->xRxBuffer != NULL )
                    {
                        /* Copy the fixed header in the Rx buffer. */
                        memcpy( mqttbufferGET_DATA( pxMQTTContext->xRxBuffer ), pxMQTTContext->ucRxFixedHeaderBuffer, pxMQTTContext->ulRxMessageReceivedLength );
                        mqttbufferGET_DATA_LENGTH( pxMQTTContext->xRxBuffer ) = pxMQTTContext->ulRxMessageReceivedLength;

                        pxMQTTContext->xRxMessageState.xRxNextByte = eMQTTRxNextByteMessage;
                        pxMQTTContext->xRxMessageState.xRxMessageAction = eMQTTRxMessageStore; /*_TODO_ This needs a timeout in case the rest of the message never comes. */
                    }
                    else
                    {
                        /* Otherwise drop the message. */
                        pxMQTTContext->xRxMessageState.xRxNextByte = eMQTTRxNextByteMessage;
                        pxMQTTContext->xRxMessageState.xRxMessageAction = eMQTTRxMessageDrop;

                        /* If a buffer was received and we are dropping because it is not
                         * large enough, return the buffer to the free buffer pool so that it
                         * can be used for other operations. */
                        prvReturnBuffer( pxMQTTContext, pxMQTTContext->xRxBuffer );
                        pxMQTTContext->xRxBuffer = NULL;
                    }
                }
            }
        }
        else if( ( pxMQTTContext->xRxMessageState.xRxNextByte == eMQTTRxNextByteMessage ) && ( pxMQTTContext->xRxMessageState.xRxMessageAction == eMQTTRxMessageStore ) )
        {
            xExpectedBytes = pxMQTTContext->xRxMessageState.ulTotalMessageLength - mqttbufferGET_DATA_LENGTH( pxMQTTContext->xRxBuffer ); /* These many bytes are still needed to constitute a packet. */
            xUnprocessedBytes = xReceivedDataLength - xProcessedBytes;                                                                    /* These many bytes are still there to be processed. */

            /* If complete message has not been received, just copy the bytes
             * and keep waiting for the remaining ones. */
            if( xUnprocessedBytes < xExpectedBytes )
            {
                mqttCOPY_BYTES( pucReceivedData, xProcessedBytes, mqttbufferGET_DATA( pxMQTTContext->xRxBuffer ), mqttbufferGET_DATA_LENGTH( pxMQTTContext->xRxBuffer ), xUnprocessedBytes );
            }
            else
            {
                /* Sufficient bytes to form a complete packet have been received. */
                mqttCOPY_BYTES( pucReceivedData, xProcessedBytes, mqttbufferGET_DATA( pxMQTTContext->xRxBuffer ), mqttbufferGET_DATA_LENGTH( pxMQTTContext->xRxBuffer ), xExpectedBytes );

                /* Process the received packet. */
                prvProcessReceivedMQTTPacket( pxMQTTContext );

                /* Reset Rx state to receive next packet. */
                prvResetRxMessageState( pxMQTTContext );
            }
        }
        else if( ( pxMQTTContext->xRxMessageState.xRxNextByte == eMQTTRxNextByteMessage ) && ( pxMQTTContext->xRxMessageState.xRxMessageAction == eMQTTRxMessageDrop ) )
        {
            xExpectedBytes = pxMQTTContext->xRxMessageState.ulTotalMessageLength - pxMQTTContext->ulRxMessageReceivedLength; /* These many bytes are still needed to constitute a packet. */
            xUnprocessedBytes = xReceivedDataLength - xProcessedBytes;                                                       /* These many bytes are still there to be processed. */

            /* If complete message has not been received, just drop the bytes
             * and keep waiting for the remaining ones. */
            if( xUnprocessedBytes < xExpectedBytes )
            {
                xProcessedBytes += xUnprocessedBytes;
                pxMQTTContext->ulRxMessageReceivedLength += ( uint32_t ) xUnprocessedBytes;
            }
            else
            {
                /* Sufficient bytes to form a complete packet have been received. */
                xProcessedBytes += xExpectedBytes;
                pxMQTTContext->ulRxMessageReceivedLength += ( uint32_t ) xExpectedBytes;

                /* Complete packet dropped. */
                xEventCallbackParams.xEventType = eMQTTPacketDropped;
                ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                /* Reset Rx state to receive next packet. */
                prvResetRxMessageState( pxMQTTContext );
            }
        }
        else
        {
            /* Should not reach here. */
        }
    }

    return xReturnCode;
}
/*-----------------------------------------------------------*/

MQTTReturnCode_t MQTT_ReturnBuffer( MQTTContext_t * pxMQTTContext,
                                    MQTTBufferHandle_t xBufferHandle )
{
    /* These are checked here once and are later used without
     * NULL checks. */
    mqttconfigASSERT( pxMQTTContext != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxGetBufferFxn != NULL );
    mqttconfigASSERT( pxMQTTContext->xBufferPoolInterface.pxReturnBufferFxn != NULL );
    mqttconfigASSERT( xBufferHandle != NULL );

    /* Return the buffer to the free buffer pool. */
    prvReturnBuffer( pxMQTTContext, xBufferHandle );

    /* Return success. */
    return eMQTTSuccess;
}
/*-----------------------------------------------------------*/

uint32_t MQTT_Periodic( MQTTContext_t * pxMQTTContext,
                        uint64_t xCurrentTickCount )
{
    Link_t * pxLink, * pxTempLink;
    MQTTBufferHandle_t xBuffer;
    MQTTEventCallbackParams_t xEventCallbackParams;
    uint32_t ulNextTimeoutTicks = UINT32_MAX;
    static const uint8_t ucPingReqPacket[] =
    {
        mqttCONTROL_PINGREQ | mqttFLAGS_PINGREQ,
        0 /* PINGREQ packet does not have any variable header or payload. */
    };

    /* These are checked here once and are later used without
     * NULL checks. */
    mqttconfigASSERT( pxMQTTContext != NULL );
    mqttconfigASSERT( pxMQTTContext->pxMQTTSendFxn != NULL );

    /* Check if any of the sent packet timed out while waiting for ACK. */
    listFOR_EACH_SAFE( pxLink, pxTempLink, &( pxMQTTContext->xTxBufferListHead ) )
    {
        /* Get the buffer from the link. */
        xBuffer = mqttbufferGET_BUFFER_HANDLE_FROM_LINK( pxLink );

        /* If the operation has timed out, inform the user and
         * return the buffer to the free buffer pool. */
        if( prvIsTimeElapsed( &( mqttbufferGET_PACKET_RECORDED_TICK_COUNT( xBuffer ) ), xCurrentTickCount, &( mqttbufferGET_PACKET_TIMEOUT_TICKS( xBuffer ) ) ) == eMQTTTrue )
        {
            /* If a connect timed out, disconnect the client and inform
             * the user about the same. */
            if( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] == ( uint8_t ) ( mqttCONTROL_CONNECT | mqttFLAGS_CONNECT ) )
            {
                /* Disconnect. */
                prvResetMQTTContext( pxMQTTContext );

                /* Inform the user about disconnect. */
                xEventCallbackParams.xEventType = eMQTTClientDisconnected;
                xEventCallbackParams.u.xDisconnectData.xDisconnectReason = eMQTTDisconnectReasonConnectTimeout;
                ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                /* No need to traverse the Tx list anymore as all
                 * the packets have already been returned to the
                 * buffer-pool in prvResetMQTTContext. */
                break;
            }
            else
            {
                #if ( mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT == 1 )

                    /* If a subscribe message timed out, remove the corresponding
                     * entry from the subscription manager. */
                    if( mqttbufferGET_DATA( xBuffer )[ mqttFIXED_HEADER_CONTROL_BYTE_OFFSET ] == ( uint8_t ) ( mqttCONTROL_SUBSCRIBE | mqttFLAGS_SUBSCRIBE ) )
                    {
                        prvRemoveSubscriptionForSubscribeOrUnsubscribeBuffer( pxMQTTContext, xBuffer );
                    }
                #endif /* mqttconfigENABLE_SUBSCRIPTION_MANAGEMENT */

                /* Inform the user about the timeout. */
                xEventCallbackParams.xEventType = eMQTTTimeout;
                xEventCallbackParams.u.xTimeoutData.usPacketIdentifier = mqttbufferGET_PACKET_IDENTIFIER( xBuffer );
                ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                /* Return the buffer back to the free buffer pool. */
                prvReturnBuffer( pxMQTTContext, xBuffer );
            }
        }
        else
        {
            /* If this Tx operation has not timed out yet, update when
             * the next earliest timeout will happen. */
            ulNextTimeoutTicks = mqttMIN( ulNextTimeoutTicks, mqttbufferGET_PACKET_TIMEOUT_TICKS( xBuffer ) );
        }
    }

    /* Check if the previously sent keep alive timed out,
     * or it is time to send a keep alive message. */
    if( pxMQTTContext->xConnectionState == eMQTTConnected )
    {
        if( prvIsTimeElapsed( &( pxMQTTContext->xLastSentMessageTimestamp ), xCurrentTickCount, &( pxMQTTContext->ulNextPeriodicInvokeTicks ) ) == eMQTTTrue )
        {
            /* If we were waiting for PINGRESP, it indicates that we failed to
             * receive PINGRESP in a reasonable time (mqttconfigKEEP_ALIVE_TIMEOUT_TICKS). */
            if( pxMQTTContext->xWaitingForPingResp == eMQTTTrue )
            {
                /* Inform the user about the ping timeout. */
                xEventCallbackParams.xEventType = eMQTTPingTimeout;
                ( void ) prvInvokeCallback( pxMQTTContext, &xEventCallbackParams );

                /* Mark that we are no longer waiting for PINGRESP. Note
                 * that keep alive time-stamp and timeout are not updated so
                 * that we try to send the next PINGREQ message as early as
                 * the next invocation of this periodic function. */
                pxMQTTContext->xWaitingForPingResp = eMQTTFalse;
            }
            else
            {
                /* If we were not waiting for PINGRESP, then it is time to send
                 * a PINGREQ. */

                /* If the following call fails to send the keep alive message,
                * the keep alive time-stamp and timeout are not updated - as
                * a result of which we will try to re-transmit the keep alive
                * message on the next invocation of this periodic function. */
                if( prvSendData( pxMQTTContext, ucPingReqPacket, sizeof( ucPingReqPacket ) ) == eMQTTSuccess )
                {
                    /* Update the last sent message timestamp. */
                    pxMQTTContext->xLastSentMessageTimestamp = prvGetCurrentTickCount( pxMQTTContext );

                    /* We must get PINGRESP within a reasonable time. */
                    pxMQTTContext->ulNextPeriodicInvokeTicks = pxMQTTContext->ulPingRequestTimeoutTicks;

                    /* Mark that we are waiting for PINGRESP. */
                    pxMQTTContext->xWaitingForPingResp = eMQTTTrue;
                }
            }
        }

        /* Update when the next earliest timeout will happen. */
        ulNextTimeoutTicks = mqttMIN( ulNextTimeoutTicks, pxMQTTContext->ulNextPeriodicInvokeTicks );
    }

    return ulNextTimeoutTicks;
}
/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_mqtt_lib_test_access_define.h"
#endif
/*-----------------------------------------------------------*/
