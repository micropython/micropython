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
 * @file aws_ota_agent.h
 * @brief OTA Agent Interface
 */

#ifndef _AWS_OTA_AGENT_H_
#define _AWS_OTA_AGENT_H_

/* Type definitions for OTA Agent */
#include "aws_ota_types.h"

/* Includes required by the FreeRTOS timers structure. */
#include "FreeRTOS.h"
#include "timers.h"

/* Include for console serial output. */
#include "aws_logging_task.h"

/* Evaluates to the length of a constant string defined like 'static const char str[]= "xyz"; */
#define CONST_STRLEN( s )    ( ( ( uint32_t ) sizeof( s ) ) - 1UL )

/* The OTA signature algorithm string is specified by the PAL. */
#define OTA_FILE_SIG_KEY_STR_MAX_LENGTH    32
extern const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ];

/**
 * @brief Special OTA Agent printing definition.
 */
#define OTA_DEBUG_LOG_LEVEL    1
#if OTA_DEBUG_LOG_LEVEL >= 1
    #define DEFINE_OTA_METHOD_NAME( name )    static const char OTA_METHOD_NAME[] = name;
    #define OTA_LOG_L1         vLoggingPrintf
#else
    #define DEFINE_OTA_METHOD_NAME( name )
    #define OTA_LOG_L1( ... )
#endif
#if OTA_DEBUG_LOG_LEVEL >= 2
    #define DEFINE_OTA_METHOD_NAME_L2( name )    static const char OTA_METHOD_NAME[] = name;
    #define OTA_LOG_L2    vLoggingPrintf
#else
    #define DEFINE_OTA_METHOD_NAME_L2( name )
    #define OTA_LOG_L2( ... )
#endif
#if OTA_DEBUG_LOG_LEVEL >= 3
    #define DEFINE_OTA_METHOD_NAME_L3( name )    static const char OTA_METHOD_NAME[] = name;
    #define OTA_LOG_L3    vLoggingPrintf
#else
    #define DEFINE_OTA_METHOD_NAME_L3( name )
    #define OTA_LOG_L3( ... )
#endif

/**
 * @brief OTA Agent states.
 *
 * The current state of the OTA Task (OTA Agent).
 *
 * @note There is currently support only for a single OTA context.
 */
typedef enum
{
    eOTA_AgentState_Unknown = -1,     /*!< The OTA agent state is not yet known. */
    eOTA_AgentState_NotReady = 0,     /*!< The OTA agent task is not running. */
    eOTA_AgentState_Ready = 1,        /*!< The OTA agent task is running and ready to transfer. */
    eOTA_AgentState_Active = 2,       /*!< The OTA agent is actively receiving an update. */
    eOTA_AgentState_ShuttingDown = 3, /*!< The OTA agent task is performing shut down activities. */
    eOTA_NumAgentStates = 4
} OTA_State_t;

/* A composite cryptographic signature structure able to hold our largest supported signature. */

#define kOTA_MaxSignatureSize    256            /* Max bytes supported for a file signature (2048 bit RSA is 256 bytes). */

typedef struct
{
    uint16_t usSize;                         /* Size, in bytes, of the signature. */
    uint8_t ucData[ kOTA_MaxSignatureSize ]; /* The binary signature data. */
} Sig256_t;

/**
 * @brief OTA Error type.
 */
typedef uint32_t OTA_Err_t;

/**
 * @defgroup OTA Error code operation helpers.
 * @brief Helper constants for extracting the error code from the OTA error returned.
 *
 * The OTA error codes consist of an agent code in the upper 8 bits of a 32 bit word and sometimes
 * merged with a platform specific code in the lower 24 bits. You must refer to the platform PAL
 * layer in use to determine the meaning of the lower 24 bits.
 */
#define kOTA_PAL_ErrMask                 0xffffffUL   /*!< The PAL layer uses the signed low 24 bits of the OTA error code. */
#define kOTA_Main_ErrMask                0xff000000UL /*!< Mask out all but the OTA Agent error code (high 8 bits). */
#define kOTA_MainErrShiftDownBits        24U          /*!< The OTA Agent error code is the highest 8 bits of the word. */

/**
 * @defgroup OTA Agent error codes.
 * @brief Error codes returned by OTA agent API.
 *
 * @note OTA agent error codes are in the upper 8 bits of the 32 bit OTA error word, OTA_Err_t.
 */
#define kOTA_Err_Panic                   0xfe000000UL     /*!< Unrecoverable FW error. Probably should log error and reboot. */
#define kOTA_Err_Uninitialized           0xff000000UL     /*!< The error code has not yet been set by a logic path. */
#define kOTA_Err_None                    0x00000000UL
#define kOTA_Err_SignatureCheckFailed    0x01000000UL     /*!< The signature check failed for the specified file. */
#define kOTA_Err_BadSignerCert           0x02000000UL     /*!< The signer certificate was not readable or zero length. */
#define kOTA_Err_OutOfMemory             0x03000000UL     /*!< General out of memory error. */
#define kOTA_Err_ActivateFailed          0x04000000UL     /*!< The activation of the new OTA image failed. */
#define kOTA_Err_CommitFailed            0x05000000UL     /*!< The acceptance commit of the new OTA image failed. */
#define kOTA_Err_RejectFailed            0x06000000UL     /*!< Error trying to reject the OTA image. */
#define kOTA_Err_AbortFailed             0x07000000UL     /*!< Error trying to abort the OTA. */
#define kOTA_Err_PublishFailed           0x08000000UL     /*!< Attempt to publish a MQTT message failed. */
#define kOTA_Err_BadImageState           0x09000000UL     /*!< The specified OTA image state was out of range. */
#define kOTA_Err_NoActiveJob             0x0a000000UL     /*!< Attempt to set final image state without an active job. */
#define kOTA_Err_NoFreeContext           0x0b000000UL     /*!< There wasn't an OTA file context available for processing. */
#define kOTA_Err_FileAbort               0x10000000UL     /*!< Error in low level file abort. */
#define kOTA_Err_FileClose               0x11000000UL     /*!< Error in low level file close. */
#define kOTA_Err_RxFileCreateFailed      0x12000000UL     /*!< The PAL failed to create the OTA receive file. */
#define kOTA_Err_BootInfoCreateFailed    0x13000000UL     /*!< The PAL failed to create the OTA boot info file. */
#define kOTA_Err_RxFileTooLarge          0x14000000UL     /*!< The OTA receive file is too big for the platform to support. */
#define kOTA_Err_NullFilePtr             0x20000000UL     /*!< Attempt to use a null file pointer. */
#define kOTA_Err_MomentumAbort           0x21000000UL     /*!< Too many OTA stream requests without any response. */
#define kOTA_Err_DowngradeNotAllowed     0x22000000UL     /*!< Firmware version is older than the previous version. */
#define kOTA_Err_SameFirmwareVersion     0x23000000UL     /*!< Firmware version is the same as previous. New firmware could have failed to commit. */
#define kOTA_Err_JobParserError          0x24000000UL     /*!< An error occurred during job document parsing. See reason sub-code. */
#define kOTA_Err_FailedToEncodeCBOR      0x25000000UL     /*!< Failed to encode CBOR object. */
#define kOTA_Err_ImageStateMismatch      0x26000000UL     /*!< The OTA job was in Self Test but the platform image state was not. Possible tampering. */
#define kOTA_Err_GenericIngestError      0x27000000UL     /*!< A failure in block ingestion not caused by the PAL. See the error sub code. */
#define kOTA_Err_UserAbort               0x28000000UL     /*!< User aborted the active OTA. */
#define kOTA_Err_ResetNotSupported       0x29000000UL     /*!< We tried to reset the device but the device doesn't support it. */
#define kOTA_Err_TopicTooLarge           0x2a000000UL     /*!< Attempt to build a topic string larger than the supplied buffer. */

/**
 * @brief OTA Job callback events.
 *
 * After an OTA update image is received and authenticated, the agent calls the user
 * callback (set with the OTA_AgentInit API) with the value eOTA_JobEvent_Activate to
 * signal that the device must be rebooted to activate the new image. When the device
 * boots, if the OTA job status is in self test mode, the agent calls the user callback
 * with the value eOTA_JobEvent_StartTest, signaling that any additional self tests
 * should be performed.
 *
 * If the OTA receive fails for any reason, the agent calls the user callback with
 * the value eOTA_JobEvent_Fail instead to allow the user to log the failure and take
 * any action deemed appropriate by the user code.
 *
 * See the OTA_ImageState_t type for more information.
 */
typedef enum
{
    eOTA_JobEvent_Activate = 0,  /*!< OTA receive is authenticated and ready to activate. */
    eOTA_JobEvent_Fail = 1,      /*!< OTA receive failed. Unable to use this update. */
    eOTA_JobEvent_StartTest = 2, /*!< OTA job is now in self test, perform user tests. */
    eOTA_LastJobEvent = eOTA_JobEvent_StartTest
} OTA_JobEvent_t;


/**
 * @brief OTA Image states.
 *
 * After an OTA update image is received and authenticated, it is logically moved to
 * the Self Test state by the OTA agent pending final acceptance. After the image is
 * activated and tested by your user code, you should put it into either the Accepted
 * or Rejected state by calling OTA_SetImageState( eOTA_ImageState_Accepted ) or
 * OTA_SetImageState( eOTA_ImageState_Rejected ). If the image is accepted, it becomes
 * the main firmware image to be booted from then on. If it is rejected, the image is
 * no longer valid and shall not be used, reverting to the last known good image.
 *
 * If you want to abort an active OTA transfer, you may do so by calling the API
 * OTA_SetImageState( eOTA_ImageState_Aborted ).
 */
typedef enum
{
    eOTA_ImageState_Unknown = 0,  /*!< The initial state of the OTA MCU Image. */
    eOTA_ImageState_Testing = 1,  /*!< The state of the OTA MCU Image post successful download and reboot. */
    eOTA_ImageState_Accepted = 2, /*!< The state of the OTA MCU Image post successful download and successful self_test. */
    eOTA_ImageState_Rejected = 3, /*!< The state of the OTA MCU Image when the job has been rejected. */
    eOTA_ImageState_Aborted = 4,  /*!< The state of the OTA MCU Image after a timeout publish to the stream request fails.
                                   *   Also if the OTA MCU image is aborted in the middle of a stream. */
    eOTA_LastImageState = eOTA_ImageState_Aborted
} OTA_ImageState_t;


/**
 * @brief OTA File Context Information.
 *
 * Information about an OTA Update file that is to be streamed. This structure is filled in from a
 * job notification MQTT message. Currently only one file context can be streamed at time.
 */
typedef struct
{
    uint8_t * pucFilePath; /*!< Local file pathname. */
    union
    {
        int32_t lFileHandle;      /*!< Device internal file pointer or handle.
                                   * File type is handle after file is open for write. */
        #if WIN32
            FILE * pxFile;       /*!< File type is stdio FILE structure after file is open for write. */
        #endif
        uint8_t * pucFile;        /*!< File type is RAM/Flash image pointer after file is open for write. */
    };
    TimerHandle_t xRequestTimer; /*!< The request timer associated with this OTA context. */
    uint32_t ulFileSize;          /*!< The size of the file in bytes. */
    uint32_t ulBlocksRemaining;   /*!< How many blocks remain to be received (a code optimization). */
    uint32_t ulFileAttributes;    /*!< Flags specific to the file being received (e.g. secure, bundle, archive). */
    uint32_t ulServerFileID;      /*!< The file is referenced by this numeric ID in the OTA job. */
    uint32_t ulRequestMomentum;   /*!< The number of stream requests published before a response was received. */
    uint8_t * pucJobName;         /*!< The job name associated with this file from the job service. */
    uint8_t * pucStreamName;      /*!< The stream associated with this file from the OTA service. */
    Sig256_t * pxSignature;       /*!< Pointer to the file's signature structure. */
    uint8_t * pucRxBlockBitmap;   /*!< Bitmap of blocks received (for de-duping and missing block request). */
    uint8_t * pucCertFilepath;    /*!< Pathname of the certificate file used to validate the receive file. */
    uint32_t ulUpdaterVersion;    /*!< Used by OTA self-test detection, the version of FW that did the update. */
    bool_t xIsInSelfTest;         /*!< True if the job is in self test mode. */
} OTA_FileContext_t;


/**
 * @brief OTA update complete callback function typedef.
 *
 * The user may register a callback function when initializing the OTA Agent. This
 * callback is used to notify the main application when the OTA update job is complete.
 * Typically, it is used to reset the device after a successful update by calling
 * OTA_ActivateNewImage() and may also be used to kick off user specified self tests
 * during the Self Test phase. If the user does not supply a custom callback function,
 * a default callback handler is used that automatically calls OTA_ActivateNewImage()
 * after a successful update.
 *
 * @note:
 *
 * The callback function is called with one of the following arguments:
 *
 *      eOTA_JobEvent_Activate      OTA update is authenticated and ready to activate.
 *      eOTA_JobEvent_Fail          OTA update failed. Unable to use this update.
 *      eOTA_JobEvent_StartTest     OTA job is now ready for optional user self tests.
 *
 * When eOTA_JobEvent_Activate is received, the job status details have been updated with
 * the state as ready for Self Test. After reboot, the new firmware will (normally) be
 * notified that it is in the Self Test phase via the callback and the application may
 * then optionally run its own tests before committing the new image.
 *
 * If the callback function is called with a result of eOTA_JobEvent_Fail, the OTA update
 * job has failed in some way and should be rejected.
 *
 * @param[in] eEvent An OTA update event from the OTA_JobEvent_t enum.
 */
typedef void (* pxOTACompleteCallback_t)( OTA_JobEvent_t eEvent );

/*---------------------------------------------------------------------------*/
/*								Public API									 */
/*---------------------------------------------------------------------------*/

/**
 * @brief OTA Agent initialization function.
 *
 * Initialize the OTA engine by starting the OTA Agent ("OTA Task") in the system. This function must
 * be called with the MQTT messaging client context before calling OTA_CheckForUpdate(). Only one
 * OTA Agent may exist.
 *
 * @param[in] pvClient The messaging protocol client context (e.g. an MQTT context).
 * @param[in] pucThingName A pointer to a C string holding the Thing name.
 * @param[in] xFunc Static callback function for when an OTA job is complete. This function will have
 * input of the state of the OTA image after download and during self-test.
 * @param[in] xTicksToWait The number of ticks to wait until the OTA Task signals that it is ready.
 * If this is set to zero, then the function will return immediately after creating the OTA task but
 * the OTA task may not be ready to operate yet. The state may be queried with OTA_GetAgentState().
 *
 * @return The state of the OTA Agent upon return from the OTA_State_t enum.
 * If the agent was successfully initialized and ready to operate, the state will be
 * eOTA_AgentState_Ready. Otherwise, it will be one of the other OTA_State_t enum values.
 */
OTA_State_t OTA_AgentInit( void * pvClient,
                           const uint8_t * pucThingName,
                           pxOTACompleteCallback_t xFunc,
                           TickType_t xTicksToWait );

/**
 * @brief Signal to the OTA Agent to shut down.
 *
 * Signals the OTA agent task to shut down. The OTA agent will unsubscribe from all MQTT job
 * notification topics, stop in progress OTA jobs, if any, and clear all resources.
 *
 * @param[in] xTicksToWait The number of ticks to wait for the OTA Agent to complete the shutdown process.
 * If this is set to zero, the function will return immediately without waiting. The actual state is
 * returned to the caller.
 *
 * @return One of the OTA agent states from the OTA_State_t enum.
 * A normal shutdown will return eOTA_AgentState_NotReady. Otherwise, refer to the OTA_State_t enum for details.
 */
OTA_State_t OTA_AgentShutdown( TickType_t xTicksToWait );

/**
 * @brief Get the current state of the OTA agent.
 *
 * @return The current state of the OTA agent.
 */
OTA_State_t OTA_GetAgentState( void );

/**
 * @brief Activate the newest MCU image received via OTA.
 *
 * This function should reset the MCU and cause a reboot of the system to execute the newly updated
 * firmware. It should be called by the user code sometime after the eOTA_JobEvent_Activate event
 * is passed to the users application via the OTA Job Complete Callback mechanism. Refer to the
 * OTA_AgentInit() function for more information about configuring the callback.
 *
 * @return kOTA_Err_None if successful, otherwise an error code prefixed with 'kOTA_Err_' from the
 * list above.
 */
OTA_Err_t OTA_ActivateNewImage( void );

/**
 * @brief Set the state of the current MCU image.
 *
 * The states are eOTA_ImageState_Testing, eOTA_ImageState_Accepted, eOTA_ImageState_Aborted or
 * eOTA_ImageState_Rejected; see OTA_ImageState_t documentation. This will update the status of the
 * current image and publish to the active job status topic.
 *
 * @param[in] The state to set of the OTA image.
 *
 * @return kOTA_Err_None if successful, otherwise an error code prefixed with 'kOTA_Err_' from the
 * list above.
 */
OTA_Err_t OTA_SetImageState( OTA_ImageState_t eState );

/**
 * @brief Get the state of the currently running MCU image.
 *
 * The states are eOTA_ImageState_Testing, eOTA_ImageState_Accepted, eOTA_ImageState_Aborted or
 * eOTA_ImageState_Rejected; see OTA_ImageState_t documentation.
 *
 * @return The state of the current context's OTA image.
 */
OTA_ImageState_t OTA_GetImageState( void );

/* @brief Request for the next available OTA job from the job service via MQTT.
 *
 * @return kOTA_Err_None if successful, otherwise an error code prefixed with 'kOTA_Err_' from the
 * list above.
 */
OTA_Err_t OTA_CheckForUpdate( void );

/*---------------------------------------------------------------------------*/
/*							Statistics API									 */
/*---------------------------------------------------------------------------*/

/**
 * @brief Get the number of OTA message packets received by the OTA agent.
 *
 * @note Calling OTA_AgentInit() will reset this statistic.
 *
 * @return The number of OTA packets that have been received but not
 * necessarily queued for processing by the OTA agent.
 */
uint32_t OTA_GetPacketsReceived( void );

/**
 * @brief Get the number of OTA message packets queued by the OTA agent.
 *
 * @note Calling OTA_AgentInit() will reset this statistic.
 *
 * @return The number of OTA packets that have been queued for processing.
 * This implies there was a free message queue entry so it can be passed
 * to the agent for processing.
 */
uint32_t OTA_GetPacketsQueued( void );

/**
 * @brief Get the number of OTA message packets processed by the OTA agent.
 *
 * @note Calling OTA_AgentInit() will reset this statistic.
 *
 * @return the number of OTA packets that have actually been processed.
 *
 */
uint32_t OTA_GetPacketsProcessed( void );

/**
 * @brief Get the number of OTA message packets dropped by the OTA agent.
 *
 * @note Calling OTA_AgentInit() will reset this statistic.
 *
 * @return the number of OTA packets that have been dropped because
 * of either no queue or at shutdown cleanup.
 */
uint32_t OTA_GetPacketsDropped( void );

#endif /* ifndef _AWS_OTA_AGENT_H_ */
