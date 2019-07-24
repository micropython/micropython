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
 * @file aws_shadow.h
 * @brief Shadow API configuration, functions, and parameter structs.
 */

#ifndef _AWS_SHADOW_H_
#define _AWS_SHADOW_H_

#include "FreeRTOS.h"

#include "aws_mqtt_agent.h"


/**
 * @brief The handle of a Shadow Client.
 */
typedef void * ShadowClientHandle_t;

/**
 * @brief Return values of Shadow API functions.
 *
 * Negative values indicate jsmn (JSON parser) errors, and @c 0 indicates
 * success. Positive values indicate other errors. Values in the range of @c 400
 * to @c 500 correspond to Shadow service rejection reasons.
 *
 * Refer to jsmn.h for jsmn errors.
 *
 * Refer to
 * http://docs.aws.amazon.com/iot/latest/developerguide/thing-shadow-error-messages.html
 * for Shadow service rejection reasons.
 */
#define eShadowJSMNPart     ( -3 )
#define eShadowJSMNInval    ( -2 )
#define eShadowJSMNNoMem    ( -1 )

/** An operation was accepted by the Shadow service, and all subroutines
 * completed successfully. */
#define eShadowSuccess      ( 0 )

/** Timeout signifies that at least one of the API call's subroutines failed to
 * complete within the given time. This error can usually be fixed by increasing
 * the timeout parameter.
 * @note
 * - Timeout doesn't always imply rejection. An operation may still be accepted
 * after a timeout. However, the Shadow Client will not notify of acceptance or
 * rejection after a timeout.
 * - If an update fails because of incorrect JSON syntax in the update document,
 * the update API call will return this code instead of @c eShadowRejectedBadRequest
 * and print an error message if #shadowENABLE_DEBUG_LOGS is @c 1. */
#define eShadowTimeout      ( 1 )

/** Failure code; signifies that at least one subroutine failed to complete
 * and did not time out. */
#define eShadowFailure      ( 2 )

/** Indicates neither failure nor success. This value should never be returned
 * by a Shadow API function, but is used internally. */
#define eShadowUnknown      ( 3 )

/**
 * @brief Shadow service rejection reasons. */
/** @{ */
#define eShadowRejectedBadRequest              ( 400 )
#define eShadowRejectedUnauthorized            ( 401 )
#define eShadowRejectedForbidden               ( 403 )
#define eShadowRejectedNotFound                ( 404 )
#define eShadowRejectedConflict                ( 409 )
#define eShadowRejectedPayloadTooLarge         ( 413 )
#define eShadowRejectedUnsupportedMediaType    ( 415 )
#define eShadowRejectedTooManyRequests         ( 429 )
#define eShadowRejectedInternalServerError     ( 500 )
typedef int16_t ShadowReturnCode_t;
/** @} */

/**
 * @brief Type of MQTT client in a new Shadow Client.
 *
 * The possible values of #ShadowCreateParams_t.xMQTTClientType; determines
 * whether to initialize a shared or dedicated MQTT client.
 * @note Only #eDedicatedMQTTClient is supported at this time.
 */
typedef enum ShadowMQTTClientType
{
    /** A shared MQTT client that may be used for purposes other than Shadow
     * Client. Currently unsupported. */
    eSharedMQTTClient,
    /** A dedicated MQTT client to be used only by Shadow Client. */
    eDedicatedMQTTClient
} ShadowMQTTClientType_t;

/**
 * @brief Parameters to pass into #SHADOW_ClientCreate.
 */
typedef struct ShadowCreateParams
{
    /**
     * @brief MQTT Client type.
     *
     * Whether the new Shadow Client will use a shared or dedicated MQTT Client.
     * Currently, only #eDedicatedMQTTClient is supported.
     * @see ShadowMQTTClientType. */
    ShadowMQTTClientType_t xMQTTClientType;

    /**
     * @brief Handle of a shared MQTT client.
     *
     * If #ShadowCreateParams_t.xMQTTClientType is #eSharedMQTTClient, set this to
     * the handle of the shared client. This member is ignored if
     #ShadowCreateParams_t.xMQTTClientType is #eDedicatedMQTTClient. */
    MQTTAgentHandle_t xMQTTClientHandle;
} ShadowCreateParams_t;

/**
 * @brief Parameters for a Shadow API operation.
 *
 * Pass this struct as one of the arguments for #SHADOW_Update, #SHADOW_Get, or
 * #SHADOW_Delete.
 */
typedef struct ShadowOperationParams
{
    /** @brief Thing name of Shadow. */
    const char * pcThingName;

    /**
     * @brief Input parameter for #SHADOW_Update; output parameter of #SHADOW_Get.
     * - For #SHADOW_Update, set this to point to the Shadow update document.
     * - For #SHADOW_Get, this is the output parameter pointing to a buffer which
     * holds the Shadow document. If #SHADOW_Get fails, this will be set to NULL.
     * - This member is ignored by #SHADOW_Delete. */
    const char * pcData;

    /**
     * @brief Input parameter for #SHADOW_Update; output parameter of #SHADOW_Get.
     * - For #SHADOW_Update, set this to length of the Shadow update document.
     * - For #SHADOW_Get, this is the output parameter specifying the length of
     #ShadowOperationParams_t.pcData. If #SHADOW_Get fails, this will be set
     * to @c 0.
     * @note If the received Shadow document is longer than this value, the
     * document will be truncated. A warning message will be printed if
     #shadowENABLE_DEBUG_LOGS is @c 1.
     * - This member is ignored by #SHADOW_Delete. */
    uint32_t ulDataLength;

    /**
     * @brief Output parameter of #SHADOW_Get.
     * - This member is ignored by #SHADOW_Update.
     * - For #SHADOW_Get, this is the output parameter for the MQTT Buffer Handle.
     * This buffer must be returned to the MQTT Client after the user is done
     * with the received document by calling #SHADOW_ReturnMQTTBuffer.
     * - This member is ignored by #SHADOW_Delete. */
    MQTTBufferHandle_t xBuffer;

    /**
     * @brief Controls whether topic subscriptions remain active after a Shadow
     * operation completes.
     *
     * Allows the MQTT subscriptions of the operation to remain active if set
     * to @c 1, saving time if the same operation is performed again. Set this
     * value to @c 0 to deactivate the operation's MQTT subscriptions after the
     * operation completes.
     * @warning Users may be billed for extraneous messages received on an
     * operation's MQTT topics. If other clients are publishing to the same topics,
     * it is best to deactivate the subscriptions. */
    uint8_t ucKeepSubscriptions;

    /**
     * @brief MQTT QoS when publishing from the Shadow Client @b to the Shadow Service.
     * @note The Shadow services always publishes to the Shadow Client with QoS 1. */
    MQTTQoS_t xQoS;
} ShadowOperationParams_t;

/**
 * @brief Function signature of a callback function registered for /update/documents.
 *
 * @param in: Custom user data.
 * @param pcThingName The Thing Name of the updated Shadow.
 * @param pcUpdateDocument The update document, a JSON string.
 * @param ulDocumentLength The length of the update document.
 * @param xBuffer The MQTT buffer containing @p pcUpdateDocument. This buffer
 * must eventually be returned by calling #SHADOW_ReturnMQTTBuffer if taken.
 *
 * @return
 * - pdTRUE to take ownership of the MQTT buffer containing pcUpdateDocument
 * - pdFALSE to leave pcUpdateDocument with the MQTT client. The MQTT client
 *   may overwrite pcUpdateDocument.
 *
 * @note Callback functions are called from the MQTT task.
 * @warning
 * - <b> Do not make any blocking calls (including #SHADOW_Update, #SHADOW_Get, or
 * #SHADOW_Delete) in a callback function! </b>
 * - The MQTT client does not guarantee that messages on /update/documents
 * will be received in the order they were published. If updates are being
 * performed frequently, it is possible to receive newer update documents
 * before older ones.
 */
typedef BaseType_t ( * ShadowUpdatedCallback_t )( void * pvUserData,
                                                  const char * const pcThingName,
                                                  const char * const pcUpdateDocument,
                                                  uint32_t ulDocumentLength,
                                                  MQTTBufferHandle_t xBuffer );

/**
 * @brief Function header of a callback function registered for /delete/accepted.
 *
 * @param in: Custom user data.
 * @param pcThingName The Name of the Thing whose Shadow was deleted.
 *
 * @note Callback functions are called from the MQTT task.
 * @warning <b> Do not make any blocking calls (including #SHADOW_Update, #SHADOW_Get, or
 * #SHADOW_Delete) in a callback function! </b>
 */
typedef void ( * ShadowDeletedCallback_t )( void * pvUserData,
                                            const char * const pcThingName );

/**
 * @brief Function header of a callback function registered for /update/delta.
 *
 * @param in: Custom user data.
 * @param pcThingName The Thing Name of the delta document.
 * @param pcDeltaDocument The delta document, a JSON string.
 * @param ulDocumentLength The length of the delta document.
 * @param xBuffer The MQTT buffer containing @p pcDeltaDocument. This buffer
 * must eventually be returned by calling #SHADOW_ReturnMQTTBuffer if taken.
 *
 * @return
 * - pdTRUE to take ownership of the MQTT buffer containing pcDeltaDocument
 * - pdFALSE to leave pcDeltaDocument with the MQTT client. The MQTT client
 *   may overwrite pcDeltaDocument.
 *
 * @note Callback functions are called from the MQTT task.
 * @warning
 * - <b> Do not make any blocking calls (including #SHADOW_Update, #SHADOW_Get, or
 * #SHADOW_Delete) in a callback function! </b>
 * - The MQTT client does not guarantee that messages on /update/delta
 * will be received in the order they were published. If updates are being
 * performed frequently, it is possible to receive newer delta documents
 * before older ones.
 */
typedef BaseType_t ( * ShadowDeltaCallback_t )( void * pvUserData,
                                                const char * const pcThingName,
                                                const char * const pcDeltaDocument,
                                                uint32_t ulDocumentLength,
                                                MQTTBufferHandle_t xBuffer );

/**
 * @brief Parameters to #SHADOW_RegisterCallbacks.
 */
typedef struct ShadowCallbackParams
{
    /** @brief Thing Name for which callbacks are registered. */
    const char * pcThingName;

    /**
     * @brief Called to notify users that a Thing Shadow has been updated.
     *
     * Callback for a message on /update/documents. Set to NULL for no callback.
     * @note This function @b will be called if the same Shadow Client that registered
     * the callback function updated the Thing Shadow. */
    ShadowUpdatedCallback_t xShadowUpdatedCallback;

    /**
     * @brief Called to notify users that @b another client has deleted the Thing Shadow.
     *
     * Callback for a message on /delete/accepted. Set to NULL for no callback.
     * @note This function <b>will not</b> be called if the same Shadow Client that
     * registered the callback function deleted the Thing Shadow. */
    ShadowDeletedCallback_t xShadowDeletedCallback;

    /**
     * @brief Called to notify users that a delta document has been generated.
     *
     * Callback for a message on /delete/accepted. Set to NULL for no callback.
     * @note This function @b will be called whenever a delta document is generated,
     * regardless of which client performed an update. */
    ShadowDeltaCallback_t xShadowDeltaCallback;
} ShadowCallbackParams_t;

/**
 * @brief Create a new Shadow Client.
 *
 * @param[out] pxShadowClientHandle Output parameter for the handle of the
 * newly-created Shadow Client. Only valid if the return code is #eShadowSuccess.
 * @param[in] pxShadowCreateParams Pointer to a #ShadowCreateParams struct.
 * @param[in] xTimeoutTicks Number of ticks this function may block before
 * timeout.
 *
 * @return #ShadowReturnCode. The value of output parameter pxShadowClientHandle
 * is valid only if the return code is #eShadowSuccess.
 *
 * @note
 * - If xTimeoutTicks is insufficient time for initialization, this function
 * will timeout and stop Shadow Client initialization. However, this function
 * may block for up to (xTimeoutTicks + #shadowCLEANUP_TIME_MS) time as it
 * cleans up a partially-initialized Shadow Client.
 */
ShadowReturnCode_t SHADOW_ClientCreate( ShadowClientHandle_t * pxShadowClientHandle,
                                        const ShadowCreateParams_t * const pxShadowCreateParams );

/**
 * @brief Connect to the Shadow service.
 *
 * For a Shadow Client with a dedicated MQTT Client (#eDedicatedMQTTClient), calling
 * this function is the only way to connect to the Shadow service. For a Shadow
 * Client with a shared MQTT Client (#eSharedMQTTClient), calling this function is
 * equivalent to calling @c MQTT_AGENT_Connect with the corresponding shared
 * MQTT Client handle.
 *
 * @param[in] xShadowClientHandle Handle of Shadow Client to connect.
 * @param[in] pxConnectParams Pointer to a @c TradstoneMQTTConnectParams_t struct.
 * Note: ClientHandle should be at the top of user data passed in pxConnectParams
 * @param[in] xTimeoutTicks Number of ticks this function may block before timeout.
 *
 * @return #ShadowReturnCode.
 */
ShadowReturnCode_t SHADOW_ClientConnect( ShadowClientHandle_t xShadowClientHandle,
                                         MQTTAgentConnectParams_t * const pxConnectParams,
                                         TickType_t xTimeoutTicks );

/**
 * @brief Disconnect from the Shadow service.
 *
 * For a Shadow Client with a dedicated MQTT Client (#eDedicatedMQTTClient), calling
 * this function is the only way to disconnect from the Shadow service. For a Shadow
 * Client with a shared MQTT Client (#eSharedMQTTClient), calling this function is
 * equivalent to calling @c MQTT_AGENT_Disconnect with the corresponding shared
 * MQTT Client handle.
 *
 * @param[in] xShadowClientHandle Handle of Shadow Client to disconnect.
 *
 * @return Unless #shadowCLEANUP_TIME_MS is insufficient to disconnect a Shadow
 * Client, this function always returns #eShadowSuccess.
 */
ShadowReturnCode_t SHADOW_ClientDisconnect( ShadowClientHandle_t xShadowClientHandle );

/**
 * @brief Free resources used by a Shadow Client.
 *
 * @param[in] xShadowClientHandle Handle of Shadow Client to be deleted.
 *
 * @return #ShadowReturnCode.
 *
 * @warning Do not call while another operation is in progress on the Shadow Client.
 * A Shadow Client must be re-initialized after this function completes if it is
 * to be used again.
 */
ShadowReturnCode_t SHADOW_ClientDelete( ShadowClientHandle_t xShadowClientHandle );

/**
 * @brief Update a Thing Shadow.
 *
 * @param[in] xShadowClientHandle Handle of Shadow Client to use for update.
 * @param[in] pxUpdateParams A pointer to a #ShadowOperationParams struct.
 * @param[in] xTimeoutTicks Number of ticks this function may block before timeout.
 *
 * @return #ShadowReturnCode.
 *
 * @note
 * - If xTimeoutTicks is insufficient time for the update, this function will
 * timeout and stop the update. However, this function may block for up to
 * (xTimeoutTicks + #shadowCLEANUP_TIME_MS) time as it cleans up subscriptions.
 * - A timeout does not always mean that the update failed. The Shadow Service
 * may still accept an update after this function times out. However, the user
 * will not be notified if acceptance occurs after a timeout. The user may
 * intentionally set a short timeout if the result of the update isn't relevant,
 * but the timeout must still be long enough for the update to be published.
 */
ShadowReturnCode_t SHADOW_Update( ShadowClientHandle_t xShadowClientHandle,
                                  ShadowOperationParams_t * const pxUpdateParams,
                                  TickType_t xTimeoutTicks );

/**
 * @brief Get a Thing Shadow from the cloud.
 *
 * @param[in] xShadowClientHandle Handle of Shadow Client to use for get.
 * @param pxGetParams A pointer to a #ShadowOperationParams struct. This is used
 * as both an input and output parameter.
 * @param[in] xTimeoutTicks Number of ticks this function may block before timeout.
 *
 * @return #ShadowReturnCode.
 *
 * @note
 * - If xTimeoutTicks is insufficient time for the get, this function will
 * timeout and stop the get. However, this function may block for up to
 * (xTimeoutTicks + #shadowCLEANUP_TIME_MS) time as it cleans up subscriptions.
 * - Documents received from the cloud after a timeout are ignored.
 * - A call to #SHADOW_ReturnMQTTBuffer should follow a call to #SHADOW_Get to
 *   return the MQTT Buffer taken by #SHADOW_Get. #ShadowOperationParams_t.xBuffer
 *   should be passed as @p xBufferHandle.
 */
ShadowReturnCode_t SHADOW_Get( ShadowClientHandle_t xShadowClientHandle,
                               ShadowOperationParams_t * const pxGetParams,
                               TickType_t xTimeoutTicks );

/**
 * @brief Delete a Thing Shadow in the cloud.
 *
 * @param[in] xShadowClientHandle Handle of Shadow Client to use for delete.
 * @param[in] pxDeleteParams A pointer to a #ShadowOperationParams struct.
 * @param[in] xTimeoutTicks Number of ticks this function may block before timeout.
 *
 * @return #ShadowReturnCode.
 *
 * @note
 * - If xTimeoutTicks is insufficient time for the delete, this function will
 * timeout and stop the delete. However, this function may block for up to
 * (xTimeoutTicks + #shadowCLEANUP_TIME_MS) time as it cleans up subscriptions.
 * - A timeout does not always mean that the delete failed. The Shadow Service
 * may still accept the delete after this function times out. However, the user
 * will not be notified if acceptance occurs after a timeout. The user may
 * intentionally set a short timeout if the result of the delete isn't relevant,
 * but the timeout must still be long enough for the delete to be published.
 */
ShadowReturnCode_t SHADOW_Delete( ShadowClientHandle_t xShadowClientHandle,
                                  ShadowOperationParams_t * const pxDeleteParams,
                                  TickType_t xTimeoutTicks );

/**
 * @brief Register callback functions.
 *
 * @param[in] xShadowClientHandle Handle of Shadow Client to use for registering callbacks.
 * @param[in] pxCallbackParams A pointer to a #ShadowCallbackParams struct.
 * @param[in] xTimeoutTicks Number of ticks this function may block before timeout.
 *
 * @return
 * - #eShadowSuccess if all callbacks registered successfully.
 * - #eShadowFailure if the Shadow Client failed to subscribe to at least one
 *   callback topic and did not time out.
 * - #eShadowTimeout if the Shadow Client timed out when subscribing to at least
 *   one callback topic.
 *
 * @note
 * - This function will register as many callbacks as it can before xTimeoutTicks
 * elapses.
 * - Callbacks are registered in the order Updated, Deleted, Delta.
 * - If a callback fails to register, this function will print a debug message if
 * #shadowENABLE_DEBUG_LOGS is @c 1 and attempt to register the next callback.
 */
ShadowReturnCode_t SHADOW_RegisterCallbacks( ShadowClientHandle_t xShadowClientHandle,
                                             ShadowCallbackParams_t * const pxCallbackParams,
                                             TickType_t xTimeoutTicks );

/**
 * @brief Return an MQTT Buffer to the MQTT client.
 *
 * For a Shadow Client with a dedicated MQTT Client (#eDedicatedMQTTClient), calling
 * this function is the only way to return an MQTT Buffer. For a Shadow Client with
 * a shared MQTT Client (#eSharedMQTTClient), calling this function is equivalent
 * to calling @c MQTT_AGENT_ReturnBuffer with the corresponding shared MQTT
 * Client handle.
 *
 * @param[in] xShadowClientHandle Handle of Shadow Client for MQTT Buffer return.
 * @param[in] xBufferHandle Handle of MQTT Buffer to return.
 *
 * @return #ShadowReturnCode.
 *
 * @note
 * - This function should be called after every #SHADOW_Get call to return the
 *   MQTT buffer taken by #SHADOW_Get. Otherwise, the MQTT client will run out
 *   of buffers after repeated calls to #SHADOW_Get.
 * - This function should also be called to return buffers taken in callback
 *   functions #ShadowUpdatedCallback_t and #ShadowDeltaCallback_t.
 */
ShadowReturnCode_t SHADOW_ReturnMQTTBuffer( ShadowClientHandle_t xShadowClientHandle,
                                            MQTTBufferHandle_t xBufferHandle );

#endif /* _AWS_SHADOW_H_ */
