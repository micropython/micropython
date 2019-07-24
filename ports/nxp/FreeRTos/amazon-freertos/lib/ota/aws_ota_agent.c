/*
 * Amazon FreeRTOS OTA Agent V1.0.0
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


/* Standard library includes. */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* OTA agent includes. */
#include "aws_ota_pal.h"
#include "aws_ota_agent.h" /*lint !e537 intentional include of all interfaces used by this file. */
#include "event_groups.h"
#include "aws_clientcredential.h"
#include "aws_ota_cbor.h"
#include "aws_application_version.h"
#include "aws_ota_agent_config.h"

/* Internal header file for shared definitions. */
#include "aws_ota_agent_internal.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"     /*lint !e537 intentional include of all interfaces used by this file. */
#include "timers.h"       /*lint !e537 intentional include of all interfaces used by this file. */
#include "task.h"         /*lint !e537 intentional include of all interfaces used by this file. */
#include "event_groups.h" /*lint !e537 intentional include of all interfaces used by this file. */
#include "queue.h"

/* MQTT includes. */
#include "aws_mqtt_agent.h"

/* JSON job document parser includes. */
#include "jsmn.h" /*lint !e537 All headers have multiple inclusion prevention. */
#include "mbedtls/base64.h"

/* Returns the byte offset of the element 'e' in the typedef structure 't'.
 * Setting an arbitrarily large base of 0x10000 and masking off that base allows
 * us to do the same thing as a zero offset without the lint warnings of using a
 * null pointer. No structure is anywhere near 64K in size.
 * */
/*lint -emacro((923,9078),OFFSET_OF) Intentionally cast pointer to uint32_t because we are using it as an offset. */
#define OFFSET_OF( t, e )    ( ( uint32_t ) ( &( ( t * ) 0x10000UL )->e ) & 0xffffUL )

/* General constants. */
#define OTA_MAX_JSON_STR_LEN               256U             /* Limit our JSON string compares to something small to avoid going into the weeds. */
#define OTA_ERASED_BLOCKS_VAL              0xffU            /* The starting state of a group of erased blocks in the Rx block bitmap. */
#define OTA_MAX_FILES                      1U               /* [MUST REMAIN 1! Future support.] Maximum number of concurrent OTA files. */
#define OTA_NUM_MSG_Q_ENTRIES              6U               /* Maximum number of entries in the OTA message queue. */
#define OTA_SUBSCRIBE_WAIT_TICKS           pdMS_TO_TICKS( 30000UL )
#define OTA_UNSUBSCRIBE_WAIT_TICKS         pdMS_TO_TICKS( 1000UL )
#define OTA_PUBLISH_WAIT_TICKS             pdMS_TO_TICKS( 10000UL )
#define OTA_MAX_STREAM_REQUEST_MOMENTUM    32U              /* Max number of stream requests allowed without a response before we abort. */
#define U32_MAX_PLACES                     10U              /* Maximum number of output digits of an unsigned long value. */

/* OTA Agent task event flags. */
#define OTA_EVT_MASK_MSG_READY             0x00000001UL /* OTA MQTT message ready event flag. */
#define OTA_EVT_MASK_SHUTDOWN              0x00000002UL /* Event flag to request OTA shutdown. */
#define OTA_EVT_MASK_REQ_TIMEOUT           0x00000004UL /* Event flag indicating the request timer has timed out. */
#define OTA_EVT_MASK_USER_ABORT            0x00000008UL /* Event flag to indicate user initiated OTA abort. */
#define OTA_EVT_MASK_ALL_EVENTS            ( OTA_EVT_MASK_MSG_READY | OTA_EVT_MASK_SHUTDOWN | OTA_EVT_MASK_REQ_TIMEOUT | OTA_EVT_MASK_USER_ABORT )

/* Stream GET message constants. */

#define OTA_CLIENT_TOKEN             "rdy"              /* Arbitrary client token sent in the stream "GET" message. */
#define OTA_MAX_BLOCK_BITMAP_SIZE    128U               /* Max allowed number of bytes to track all blocks of an OTA file. Adjust block size if more range is needed. */
#define OTA_REQUEST_MSG_MAX_SIZE     ( 3U * OTA_MAX_BLOCK_BITMAP_SIZE )

/* Agent to Job Service status message constants. */

#define OTA_STATUS_MSG_MAX_SIZE        128U             /* Max length of a job status message to the service. */
#define OTA_UPDATE_STATUS_FREQUENCY    64U              /* Update the job status every 64 unique blocks received. */

/* Job document parser constants. */

#define OTA_MAX_JSON_TOKENS    64U                      /* Number of JSON tokens supported in a single parser call. */
#define OTA_MAX_TOPIC_LEN      256U                     /* Max length of a dynamically generated topic string (usually on the stack). */

/* When subscribing to MQTT topics with a callback handler, we use the callback
 * context variable as a subscription type to expedite dispatch of the published
 * messages instead of comparing against the topic string.
 */
typedef enum
{
    eOTA_PubMsgType_Job = 0, /* Messages on the topic are job messages. */
    eOTA_PubMsgType_Stream   /* Messages on the topic are stream messages. */
} OTA_PubMsgType_t;


typedef struct
{
    MQTTPublishData_t xPubData; /* The MQTT publish data. */
    union
    {
        OTA_PubMsgType_t eMsgType; /* The OTA published message type (job or stream). */
        int32_t lMsgType;          /* The OTA published message type as a signed 32 bit value. */
    };                             /*lint !e658 Anonymous union is intentional as we use prefixes and don't need a union name. */
} OTA_PubMsg_t;


/* The OTA job document contains parameters that are required for us to build the
 * stream request message and manage the OTA process. Including info like file name,
 * size, attributes, etc. The following value specifies the number of parameters
 * that are included in the job document model although some may be optional. */

#define OTA_NUM_JOB_PARAMS         ( 16 ) /* Number of parameters in the job document. */
/* We need the following string to match in a couple places in the code so use a #define. */
#define OTA_JSON_UPDATED_BY_KEY    "updatedBy"

static const char cOTA_JSON_ClientTokenKey[] = "clientToken";
static const char cOTA_JSON_ExecutionKey[] = "execution";
static const char cOTA_JSON_JobIDKey[] = "jobId";
static const char cOTA_JSON_StatusDetailsKey[] = "statusDetails";
static const char cOTA_JSON_SelfTestKey[] = "self_test";
static const char cOTA_JSON_UpdatedByKey[] = OTA_JSON_UPDATED_BY_KEY;
static const char cOTA_JSON_JobDocKey[] = "jobDocument";
static const char cOTA_JSON_OTAUnitKey[] = "afr_ota";
static const char cOTA_JSON_FileGroupKey[] = "files";
static const char cOTA_JSON_StreamNameKey[] = "streamname";
static const char cOTA_JSON_FilePathKey[] = "filepath";
static const char cOTA_JSON_FileSizeKey[] = "filesize";
static const char cOTA_JSON_FileIDKey[] = "fileid";
static const char cOTA_JSON_FileAttributeKey[] = "attr";
static const char cOTA_JSON_FileCertNameKey[] = "certfile";

enum
{
    eJobReason_Receiving = 0,  /* Update progress status. */
    eJobReason_SigCheckPassed, /* Set status details to Self Test Ready. */
    eJobReason_SelfTestActive, /* Set status details to Self Test Active. */
    eJobReason_Accepted,       /* Set job state to Succeeded. */
    eJobReason_Rejected,       /* Set job state to Failed. */
    eJobReason_Aborted,        /* Set job state to Failed. */
    eNumJobReasons
};

/* These are the associated statusDetails 'reason' codes that go along with
 * the above enums during the OTA update process. The 'Receiving' state is
 * updated with transfer progress as #blocks received of #total.
 */
static const char * pcOTA_JobReason_Strings[ eNumJobReasons ] = { "", "ready", "active", "accepted", "rejected", "aborted" };

/*lint -esym(749,OTA_JobStatus_t::eJobStatus_Rejected) Until the Job Service supports it, this is unused. */
typedef enum
{
    eJobStatus_InProgress = 0,
    eJobStatus_Failed,
    eJobStatus_Succeeded,
    eJobStatus_Rejected,      /* Not possible today using the "get next job" feature. FUTURE! */
    eJobStatus_FailedWithVal, /* This shows 2 numeric reason codes. */
    eNumJobStatusMappings
} OTA_JobStatus_t;

/* We map all of the above status cases to one of these 4 status strings.
 * These are the only strings that are supported by the Job Service. You
 * shall not change them to arbitrary strings or the job will not change
 * states.
 * */
static const char cOTA_String_InProgress[] = "IN_PROGRESS";
static const char cOTA_String_Failed[] = "FAILED";
static const char cOTA_String_Succeeded[] = "SUCCEEDED";
static const char cOTA_String_Rejected[] = "REJECTED";

static const char * pcOTA_JobStatus_Strings[ eNumJobStatusMappings ] =
{
    cOTA_String_InProgress,
    cOTA_String_Failed,
    cOTA_String_Succeeded,
    cOTA_String_Rejected,
    cOTA_String_Failed, /* eJobStatus_FailedWithVal */
};


#define OTA_DOC_MODEL_MAX_PARAMS    32U                    /* The parameter list is backed by a 32 bit longword bitmap by design. */
#define OTA_JOB_PARAM_REQUIRED      ( ( bool_t ) pdTRUE )  /* Used to denote a required document model parameter. */
#define OTA_JOB_PARAM_OPTIONAL      ( ( bool_t ) pdFALSE ) /* Used to denote an optional document model parameter. */
#define OTA_DONT_STORE_PARAM        0xffffffffUL           /* If ulDestOffset in the model is 0xffffffff, do not store the value. */

/* This union allows us to access document model parameter addresses as their
 * actual type without casting every time we access a parameter. */

typedef union MultiParmPtr
{
    char ** ppcPtr;
    const char ** ppcConstPtr;
    uint32_t * pulPtr;
    uint32_t ulVal;
    bool_t * pxBoolPtr;
    Sig256_t ** ppxSig256Ptr;
    void ** ppvPtr;
} MultiParmPtr_t;


/* OTA job document parser error codes. */

typedef enum
{
    eOTA_JobParseErr_Unknown = -1,        /* The error code has not yet been set by a logic path. */
    eOTA_JobParseErr_None = 0,            /* Signifies no error has occurred. */
    eOTA_JobParseErr_BusyWithExistingJob, /* We're busy with a job but received a new job document. */
    eOTA_JobParseErr_NullJob,             /* A null job was reported (no job ID). */
    eOTA_JobParseErr_BusyWithSameJob,     /* We're already busy with the reported job ID. */
    eOTA_JobParseErr_ZeroFileSize,        /* Job document specified a zero sized file. This is not allowed. */
    eOTA_JobParseErr_NonConformingJobDoc, /* The job document failed to fulfill the model requirements. */
    eOTA_JobParseErr_BadModelInitParams,  /* There was an invalid initialization parameter used in the document model. */
    eOTA_JobParseErr_NoContextAvailable   /* There wasn't an OTA context available. */
} OTA_JobParseErr_t;


/*lint -e830 -e9003 Keep these in one location for easy discovery should they change in the future. */
/* Topic strings used by the OTA process. */
/* These first few are topic extensions to the dynamic base topic that includes the Thing name. */
static const char cOTA_JobsGetNextAccepted_TopicTemplate[] = "$aws/things/%s/jobs/$next/get/accepted";
static const char cOTA_JobsNotifyNext_TopicTemplate[] = "$aws/things/%s/jobs/notify-next";
static const char cOTA_JobsGetNext_TopicTemplate[] = "$aws/things/%s/jobs/$next/get";
static const char cOTA_JobStatus_TopicTemplate[] = "$aws/things/%s/jobs/%s/update";
static const char cOTA_StreamData_TopicTemplate[] = "$aws/things/%s/streams/%s/data/cbor";
static const char cOTA_GetStream_TopicTemplate[] = "$aws/things/%s/streams/%s/get/cbor";
static const char cOTA_GetNextJob_MsgTemplate[] = "{\"clientToken\":\"%u:%s\"}";
static const char cOTA_JobStatus_StatusTemplate[] = "{\"status\":\"%s\",\"statusDetails\":{";
static const char cOTA_JobStatus_ReceiveDetailsTemplate[] = "\"%s\":\"%u/%u\"}}";
static const char cOTA_JobStatus_SelfTestDetailsTemplate[] = "\"%s\":\"%s\",\"" OTA_JSON_UPDATED_BY_KEY "\":\"0x%x\"}}";
static const char cOTA_JobStatus_ReasonStrTemplate[] = "\"reason\":\"%s: 0x%08x\"}}";
static const char cOTA_JobStatus_SucceededStrTemplate[] = "\"reason\":\"%s v%u.%u.%u\"}}";
static const char cOTA_JobStatus_ReasonValTemplate[] = "\"reason\":\"0x%08x: 0x%08x\"}}";
static const char cOTA_String_Receive[] = "receive";

/* Flag for self-test mode. */
static bool_t xInSelfTest = false;

/* Test a null terminated string against a JSON string of known length and return whether
 * it is the same or not. */

bool_t JSON_IsCStringEqual( const char * pcJSONString,
                            uint32_t ulLen,
                            const char * pcCString );

/* OTA agent private function prototypes. */

static void prvOTAUpdateTask( void * pvUnused );

/* Start a timer to kick-off the OTA update request. Pass it the OTA file context. */

static void prvStartRequestTimer( OTA_FileContext_t * C );

/* Stop the OTA update request timer. */

static void prvStopRequestTimer( OTA_FileContext_t * C );

/* This is the OTA update request timer callback action (upon timeout). */

static void prvRequestTimer_Callback( TimerHandle_t T );

/* Stop the OTA self test timer if it is running. */

static void prvStopSelfTestTimer( void );

/* Start the self test timer if we're in self test mode. */

static BaseType_t OTA_CheckForSelfTest( void );

/* When the self test response timer expires, reset the device since we're likely broken. */

static void prvSelfTestTimer_Callback( TimerHandle_t T );

/* Subscribe to the jobs notification topic (i.e. New file version available). */

static bool_t prvSubscribeToJobNotificationTopics( void );

/* UnSubscribe from the jobs notification topic. */

static void prvUnSubscribeFromJobNotificationTopic( void );

/* Subscribe to the firmware update receive topic. */

static bool_t prvSubscribeToDataStream( OTA_FileContext_t * C );

/* UnSubscribe from the firmware update receive topic. */

static bool_t prvUnSubscribeFromDataStream( OTA_FileContext_t * C );

/* Publish a message using the platforms PubSub mechanism. */

static MQTTAgentReturnCode_t prvPublishMessage( void * pvClient,
                                                const char * const pcTopic,
                                                uint16_t usTopicLen,
                                                char * pcMsg,
                                                uint32_t ulMsgSize,
                                                MQTTQoS_t eQOS );

/* Called when the OTA agent receives a file data block message. */

static IngestResult_t prvIngestDataBlock( OTA_FileContext_t * C,
                                          const char * pcRawMsg,
                                          uint32_t ulMsgSize,
                                          OTA_Err_t * pxCloseResult );

/* Called when the OTA agent receives an OTA version message. */

static OTA_FileContext_t * prvProcessOTAJobMsg( const char * pcRawMsg,
                                                uint32_t ulMsgLen );

/* Get an available OTA file context structure or NULL if none available. */

static OTA_FileContext_t * prvGetFreeContext( void );

/* Parse a JSON document using the specified document model. */

static DocParseErr_t prvParseJSONbyModel( const char * pcJSON,
                                          uint32_t ulMsgLen,
                                          JSON_DocModel_t * pxDocModel );

/* Parse the OTA job document, validate and return the populated OTA context if valid. */

static OTA_FileContext_t * prvParseJobDoc( const char * pcJSON,
                                           uint32_t ulMsgLen );

/* Close an open OTA file context and free it. */

static bool_t prvOTA_Close( OTA_FileContext_t * const C );

/* Called when a MQTT message is received on an OTA agent topic of interest. */

static MQTTBool_t prvOTAPublishCallback( void * pvCallbackContext,
                                         const MQTTPublishData_t * const pxPublishData );

/* Update the job status topic with our progress of the OTA transfer. */

static void prvUpdateJobStatus( OTA_FileContext_t * C,
                                OTA_JobStatus_t eStatus,
                                int32_t lReason,
                                int32_t lSubReason );

/* Construct the "Get Stream" message and publish it to the stream service request topic. */

static OTA_Err_t prvPublishGetStreamMessage( OTA_FileContext_t * C );

/* Internal function to set the image state including an optional reason code. */

static OTA_Err_t prvSetImageStateWithReason( OTA_ImageState_t eState,
                                             uint32_t ulReason );

/* The default OTA callback handler if not provided to OTA_AgentInit(). */

static void prvDefaultOTACompleteCallback( OTA_JobEvent_t eEvent );

/* A helper function to cleanup resources during OTA agent shutdown. */

static void prvAgentShutdownCleanup( OTA_PubMsg_t * pxMsgMetaData );

/* Search the document model for a key that matches the specified JSON key. */

static DocParseErr_t prvSearchModelForTokenKey( JSON_DocModel_t * pxDocModel,
                                                const char * pcJSONString,
                                                uint32_t ulStrLen,
                                                uint16_t * pusMatchingIndexResult );

/* Prepare the document model for use by sanity checking the initialization parameters
 * and detecting all required parameters. */

static DocParseErr_t prvInitDocModel( JSON_DocModel_t * pxDocModel,
                                      const JSON_DocParam_t * pxBodyDef,
                                      uint32_t ulContextBaseAddr,
                                      uint32_t ulContextSize,
                                      uint16_t usNumJobParams );

/* Attempt to force reset the device. Normally called by the agent when a self test rejects the update. */

static OTA_Err_t prvResetDevice( void );

/* Check if the platform is in self-test. */

static bool_t prvInSelftest( void );

/* This is the OTA statistics structure to hold useful info. */

typedef struct ota_agent_statistics
{
    uint32_t ulOTA_PacketsReceived;  /* Number of OTA packets received by the MQTT callback. */
    uint32_t ulOTA_PacketsQueued;    /* Number of OTA packets queued by the MQTT callback. */
    uint32_t ulOTA_PacketsProcessed; /* Number of OTA packets processed by the OTA task. */
    uint32_t ulOTA_PacketsDropped;   /* Number of OTA packets dropped due to congestion. */
    uint32_t ulOTA_PublishFailures;  /* Number of MQTT publish failures. */
} OTA_AgentStatistics_t;

/* The OTA agent is a singleton today. The structure keeps it nice and organized. */

typedef struct ota_agent_context
{
    OTA_State_t eState;                                     /* State of the OTA agent. */
    uint8_t ucThingName[ otaconfigMAX_THINGNAME_LEN + 1U ]; /* Thing name + zero terminator. */
    void * pvPubSubClient;                                  /* The current publish/subscribe client context (use is determined by the client). */
    OTA_FileContext_t xOTA_Files[ OTA_MAX_FILES ];          /* Static array of OTA file structures. */
    EventGroupHandle_t xOTA_EventFlags;                     /* Event group for communicating with the OTA task. */
    pxOTACompleteCallback_t xOTAJobCompleteCallback;        /* The user level function to call after the OTA job is complete. */
    uint8_t * pucOTA_Singleton_ActiveJobName;               /* The currently active job name. We only allow one at a time. */
    uint8_t * pucClientTokenFromJob;                        /* The clientToken field from the latest update job. */
    TimerHandle_t xSelfTestTimer;                           /* The self test response expected timer. */
    OTA_ImageState_t eImageState;                           /* The current OTA image state as set by the OTA agent. */
    QueueHandle_t xOTA_MsgQ;                                /* Used to pass MQTT messages to the OTA agent. */
    OTA_AgentStatistics_t xStatistics;                      /* The OTA agent statistics block. */
} OTA_AgentContext_t;


/* This is THE OTA agent context and initialization state. */

static OTA_AgentContext_t xOTA_Agent =
{
    .eState                         = eOTA_AgentState_NotReady,
    .ucThingName                    = { 0 },
    .pvPubSubClient                 = NULL,
    .xOTA_Files                     = { { 0 } }, /*lint !e910 !e9080 Zero initialization of all members of the single file context structure.*/
    .xOTA_EventFlags                = NULL,
    .xOTAJobCompleteCallback        = NULL,
    .pucOTA_Singleton_ActiveJobName = NULL,
    .pucClientTokenFromJob          = NULL,
    .xSelfTestTimer                 = NULL,
    .eImageState                    = eOTA_ImageState_Unknown,
    .xOTA_MsgQ                      = NULL,
    .xStatistics                    = { 0 },
};


/* This is the default OTA callback handler if the user does not provide
 * one. It will do the basic activation and commit of accepted images.
 *
 * The OTA agent has completed the update job or determined we're in self
 * test mode. If the update was accepted, we want to activate the new image
 * by resetting the device to boot the new firmware.  If now is not a good
 * time to reset the device, the user should have registered their own
 * callback function instead of this default callback to allow user level
 * self tests and a user scheduled reset.
 * If the update was rejected, just return without doing anything and we'll
 * wait for another job. If it reported that we're in self test mode, we've
 * already successfully connected to the AWS IoT broker and received the
 * latest job so go ahead and set the image as accepted since there is no
 * additional user level tests to run.
 */
static void prvDefaultOTACompleteCallback( OTA_JobEvent_t eEvent )
{
    DEFINE_OTA_METHOD_NAME( "prvDefaultOTACompleteCallback" );

    OTA_Err_t xErr = kOTA_Err_Uninitialized;

    if( eEvent == eOTA_JobEvent_Activate )
    {
        OTA_LOG_L1( "[%s] Received eOTA_JobEvent_Activate callback from OTA Agent.\r\n", OTA_METHOD_NAME );
        ( void ) OTA_ActivateNewImage();
    }
    else if( eEvent == eOTA_JobEvent_Fail )
    {
        OTA_LOG_L1( "[%s] Received eOTA_JobEvent_Fail callback from OTA Agent.\r\n", OTA_METHOD_NAME );
        /* Nothing special to do. The OTA agent handles it. */
    }
    else if( eEvent == eOTA_JobEvent_StartTest )
    {
        /* Accept the image since it was a good transfer
         * and networking and services are all working.
         */
        OTA_LOG_L1( "[%s] Received eOTA_JobEvent_StartTest callback from OTA Agent.\r\n", OTA_METHOD_NAME );
        xErr = OTA_SetImageState( eOTA_ImageState_Accepted );

        if( xErr != kOTA_Err_None )
        {
            OTA_LOG_L1( "[%s] Error! Failed to set image state.\r\n", OTA_METHOD_NAME );
        }
    }
    else
    {
        OTA_LOG_L1( "[%s] Received invalid job event %d from OTA Agent.\r\n", OTA_METHOD_NAME, eEvent );
    }
}


/* Public API to initialize the OTA Agent.
 *
 * If the Application calls OTA_AgentInit() after it is already initialized, we will
 * only reset the statistics counters and set the job complete callback but will not
 * modify the existing OTA agent context. You must first call OTA_AgentShutdown()
 * successfully.
 */

OTA_State_t OTA_AgentInit( void * pvClient,
                           const uint8_t * pucThingName,
                           pxOTACompleteCallback_t xFunc,
                           TickType_t xTicksToWait )
{
    DEFINE_OTA_METHOD_NAME( "OTA_AgentInit" );

    static TaskHandle_t xOTA_TaskHandle;

    uint32_t ulIndex;
    BaseType_t xReturn = 0;

    /* The actual OTA queue control structure. Only created once. */
    static StaticQueue_t xStaticQueue;

    /* The array to use as the queue's data area. This is an array of
     * OTA MQTT message data structures (OTA_PubMsg_t metadata).
     * Also only created once. */
    static OTA_PubMsg_t xQueueData[ OTA_NUM_MSG_Q_ENTRIES ];

    /* Set the function to be called after an OTA job is complete or starting test mode. */
    if( xFunc == NULL )
    {
        /* If the caller didn't supply a callback function, provide a default handler. */
        xOTA_Agent.xOTAJobCompleteCallback = prvDefaultOTACompleteCallback;
    }
    else
    {
        xOTA_Agent.xOTAJobCompleteCallback = xFunc;
    }

    /* Reset our statistics counters. */
    xOTA_Agent.xStatistics.ulOTA_PacketsReceived = 0;
    xOTA_Agent.xStatistics.ulOTA_PacketsDropped = 0;
    xOTA_Agent.xStatistics.ulOTA_PacketsQueued = 0;
    xOTA_Agent.xStatistics.ulOTA_PacketsProcessed = 0;
    xOTA_Agent.xStatistics.ulOTA_PublishFailures = 0;

    if( pucThingName != NULL )
    {
        uint32_t ulStrLen = strlen( ( const char * ) pucThingName );

        if( ulStrLen <= otaconfigMAX_THINGNAME_LEN )
        {
            /* Store the Thing name to be used for topics later. */
            memcpy( xOTA_Agent.ucThingName, pucThingName, ulStrLen + 1UL ); /* Include zero terminator when saving the Thing name. */
        }

        portENTER_CRITICAL();

        if( xOTA_Agent.eState == eOTA_AgentState_NotReady )
        {
            xOTA_Agent.eImageState = eOTA_ImageState_Unknown; /* The current OTA image state as set by the OTA agent. */
            xOTA_Agent.pvPubSubClient = pvClient;             /* Save the current pub/sub client as specified by the user. */

            /* Create the queue used to pass MQTT publish messages to the OTA task. */
            xOTA_Agent.xOTA_MsgQ = xQueueCreateStatic( ( UBaseType_t ) OTA_NUM_MSG_Q_ENTRIES, ( UBaseType_t ) sizeof( OTA_PubMsg_t ), ( uint8_t * ) xQueueData, &xStaticQueue );
            configASSERT( xOTA_Agent.xOTA_MsgQ );

            for( ulIndex = 0; ulIndex < OTA_MAX_FILES; ulIndex++ )
            {
                xOTA_Agent.xOTA_Files[ ulIndex ].pucFilePath = NULL;
            }

            xReturn = xTaskCreate( prvOTAUpdateTask, "OTA Task", otaconfigSTACK_SIZE, NULL, otaconfigAGENT_PRIORITY, &xOTA_TaskHandle );
            portEXIT_CRITICAL(); /* Protected elements are initialized. It's now safe to context switch. */

            if( xReturn == pdPASS )
            {
                /* Wait for the OTA agent to be ready if requested. */
                while( ( xTicksToWait-- > 0U ) && ( xOTA_Agent.eState != eOTA_AgentState_Ready ) )
                {
                    vTaskDelay( 1 );
                }
            }
            else
            {
                /* Task creation failed so fall through to exit. */
            }
        }
        else
        {
            portEXIT_CRITICAL();
        }
    }

    if( xOTA_Agent.eState == eOTA_AgentState_Ready )
    {
        OTA_LOG_L1( "[%s] Ready.\r\n", OTA_METHOD_NAME );
    }
    else
    {
        OTA_LOG_L1( "[%s] Failed to start. OTA Task:%08x  Queue:%08x\r\n", OTA_METHOD_NAME, xReturn, xOTA_Agent.xOTA_MsgQ );
    }

    return xOTA_Agent.eState; /* Return status of agent. */
}



/* Public API to shutdown the OTA process. */

OTA_State_t OTA_AgentShutdown( TickType_t xTicksToWait )
{
    DEFINE_OTA_METHOD_NAME( "OTA_AgentShutdown" );

    OTA_LOG_L2( "[%s] Start: %u ticks\r\n", OTA_METHOD_NAME, xTicksToWait );

    if( ( xOTA_Agent.eState != eOTA_AgentState_NotReady ) && ( xOTA_Agent.eState != eOTA_AgentState_ShuttingDown ) )
    {
        ( void ) xEventGroupSetBits( xOTA_Agent.xOTA_EventFlags, OTA_EVT_MASK_SHUTDOWN );

        /* Wait for the OTA agent to complete shutdown, if requested. */
        while( ( xTicksToWait > 0U ) && ( xOTA_Agent.eState != eOTA_AgentState_NotReady ) )
        {
            vTaskDelay( 1 );
            xTicksToWait--;
        }
    }
    else
    {
        OTA_LOG_L1( "[%s] Nothing to do: Already in state %u\r\n", OTA_METHOD_NAME, xOTA_Agent.eState );
    }

    OTA_LOG_L2( "[%s] End: %u ticks\r\n", OTA_METHOD_NAME, xTicksToWait );
    return xOTA_Agent.eState;
}


/* Return the current state of the OTA agent. */

OTA_State_t OTA_GetAgentState( void )
{
    return xOTA_Agent.eState;
}

uint32_t OTA_GetPacketsDropped( void )
{
    return xOTA_Agent.xStatistics.ulOTA_PacketsDropped;
}

uint32_t OTA_GetPacketsQueued( void )
{
    return xOTA_Agent.xStatistics.ulOTA_PacketsQueued;
}

uint32_t OTA_GetPacketsProcessed( void )
{
    return xOTA_Agent.xStatistics.ulOTA_PacketsProcessed;
}

uint32_t OTA_GetPacketsReceived( void )
{
    return xOTA_Agent.xStatistics.ulOTA_PacketsReceived;
}

/* Request for the next available OTA job from the job service by publishing
 * a "get next job" message to the job service. */

OTA_Err_t OTA_CheckForUpdate( void )
{
    DEFINE_OTA_METHOD_NAME( "OTA_CheckForUpdate" );

    char cJobTopic[ OTA_MAX_TOPIC_LEN ];
    static uint32_t ulReqCounter = 0;
    MQTTAgentReturnCode_t eResult;
    uint32_t ulMsgLen;
    uint16_t usTopicLen;
    OTA_Err_t xError = kOTA_Err_Uninitialized;

    /* The following buffer is big enough to hold a dynamically constructed $next/get job message.
     * It contains a client token that is used to track how many requests have been made. */
    char cMsg[ CONST_STRLEN( cOTA_GetNextJob_MsgTemplate ) + U32_MAX_PLACES + otaconfigMAX_THINGNAME_LEN ];

    OTA_LOG_L1( "[%s] Request #%u\r\n", OTA_METHOD_NAME, ulReqCounter );
    /*lint -e586 Intentionally using snprintf. */
    ulMsgLen = ( uint32_t ) snprintf( cMsg,
                                      sizeof( cMsg ),
                                      cOTA_GetNextJob_MsgTemplate,
                                      ulReqCounter,
                                      xOTA_Agent.ucThingName );
    ulReqCounter++;
    usTopicLen = ( uint16_t ) snprintf( cJobTopic,
                                        sizeof( cJobTopic ),
                                        cOTA_JobsGetNext_TopicTemplate,
                                        xOTA_Agent.ucThingName );

    if( ( usTopicLen > 0U ) && ( usTopicLen < sizeof( cJobTopic ) ) )
    {
        eResult = prvPublishMessage(
            xOTA_Agent.pvPubSubClient,
            cJobTopic,
            usTopicLen,
            cMsg,
            ulMsgLen,
            eMQTTQoS1 );

        if( eResult != eMQTTAgentSuccess )
        {
            OTA_LOG_L1( "[%s] Failed to publish MQTT message.\r\n", OTA_METHOD_NAME );
            xError = kOTA_Err_PublishFailed;
        }
        else
        {
            /* Nothing special to do. We succeeded. */
            xError = kOTA_Err_None;
        }
    }
    else
    {
        OTA_LOG_L1( "[%s] Topic too large for supplied buffer.\r\n", OTA_METHOD_NAME );
        xError = kOTA_Err_TopicTooLarge;
    }

    return xError;
}


/* Clean up after the OTA process is done. Possibly free memory for re-use. */

static void prvAgentShutdownCleanup( OTA_PubMsg_t * pxMsgMetaData )
{
    DEFINE_OTA_METHOD_NAME( "prvAgentShutdownCleanup" );
    uint32_t ulIndex;

    xOTA_Agent.eState = eOTA_AgentState_ShuttingDown;

    /* Stop and delete any existing self test timer. */
    if( xOTA_Agent.xSelfTestTimer != NULL )
    {
        ( void ) xTimerStop( xOTA_Agent.xSelfTestTimer, 0 );
        ( void ) xTimerDelete( xOTA_Agent.xSelfTestTimer, 0 );
        xOTA_Agent.xSelfTestTimer = NULL;
    }

    /* Don't remain subscribed to the OTA job notification topic since we're shutting down. */
    prvUnSubscribeFromJobNotificationTopic();

    /* Close any open OTA transfers. */
    for( ulIndex = 0; ulIndex < OTA_MAX_FILES; ulIndex++ )
    {
        if( prvOTA_Close( &xOTA_Agent.xOTA_Files[ ulIndex ] ) == ( bool_t ) pdFALSE )
        {
            OTA_LOG_L1( "[%s] Error! OTA_FileContext_t[%u] pointer is null.\r\n", OTA_METHOD_NAME, ulIndex );
        }
    }

    /* Free any remaining string memory holding the job name. */
    if( xOTA_Agent.pucOTA_Singleton_ActiveJobName != NULL )
    {
        vPortFree( xOTA_Agent.pucOTA_Singleton_ActiveJobName );
        xOTA_Agent.pucOTA_Singleton_ActiveJobName = NULL;
    }

    /* If there are any queued OTA messages from MQTT, give the buffers back to MQTT for re-use. */
    if( pxMsgMetaData != NULL )
    {
        pxMsgMetaData->xPubData.xBuffer = NULL; /* Reset the buffer pointer before we get a real one from the queue. */

        if( xOTA_Agent.xOTA_MsgQ != NULL )
        {
            while( xQueueReceive( xOTA_Agent.xOTA_MsgQ, pxMsgMetaData, 0 ) != pdFALSE )
            {
                xOTA_Agent.xStatistics.ulOTA_PacketsDropped++;

                /* Return the MQTT buffer to the agent. */
                if( ( MQTT_AGENT_ReturnBuffer( xOTA_Agent.pvPubSubClient, pxMsgMetaData->xPubData.xBuffer ) ) == eMQTTAgentSuccess )
                {
                    OTA_LOG_L2( "[%s] Returned buffer to MQTT Client.\r\n", OTA_METHOD_NAME );
                }
                else
                {
                    OTA_LOG_L1( "[%s] MQTT_AGENT_ReturnBuffer() failed.\r\n", OTA_METHOD_NAME );
                    break;
                }
            }
        }
        else
        {
            OTA_LOG_L1( "[%s] Attempt to flush MQTT messages from a null queue pointer.\r\n", OTA_METHOD_NAME );
        }
    }
    else
    {
        OTA_LOG_L1( "[%s] Attempt to flush MQTT messages without a storage buffer.\r\n", OTA_METHOD_NAME );
    }
}


/* This should be called by the user application or the default OTA callback handler
 * after an OTA update is considered accepted. It simply calls the platform specific
 * code required to activate the received OTA update (usually just a device reset).
 */
OTA_Err_t OTA_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME( "OTA_ActivateNewImage" );

    OTA_Err_t xErr;

    /* Call platform specific code to activate the image. This should reset the device
     * and not return unless there is a problem within the PAL layer. If it does return,
     * output an error message. The device may need to be reset manually. */
    xErr = prvPAL_ActivateNewImage();
    OTA_LOG_L1( "[%s] Failed to activate new image (0x%08x). Please reset manually.\r\n", OTA_METHOD_NAME, xErr );
    return xErr;
}


/* This is a private function only meant to be called by the OTA agent after the
 * currently running image that is in the self test phase rejects the update.
 * It simply calls the platform specific code required to reset the device.
 */
static OTA_Err_t prvResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME( "prvResetDevice" );

    OTA_Err_t xErr;

    /* Call platform specific code to reset the device. This should not return unless
     * there is a problem within the PAL layer. If it does return, output an error
     * message. The device may need to be reset manually. */
    OTA_LOG_L1( "[%s] Attempting forced reset of device...\r\n", OTA_METHOD_NAME );
    xErr = prvPAL_ResetDevice();
    OTA_LOG_L1( "[%s] Failed to reset the device (0x%08x). Please reset manually.\r\n", OTA_METHOD_NAME, xErr );
    return xErr;
}

/*
 * This is a private function which checks if the platform is in self-test.
 */
static bool_t prvInSelftest( void )
{
    bool_t xSelfTest = false;

    if( prvPAL_GetPlatformImageState() == eOTA_PAL_ImageState_PendingCommit )
    {
        xSelfTest = true;
    }

    return xSelfTest;
}


/* Accept, reject or abort the OTA image transfer.
 *
 * If accepting or rejecting an image transfer, this API
 * will set the OTA image state and update the job status
 * appropriately.
 * If aborting a transfer, it will trigger the OTA task to
 * abort via an RTOS event asynchronously and therefore do
 * not set the image state here.
 *
 * NOTE: This call may block due to the status update message.
 */

OTA_Err_t OTA_SetImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "OTA_SetImageState" );

    OTA_Err_t xErr = kOTA_Err_Uninitialized;

    switch( eState )
    {
        case eOTA_ImageState_Aborted:

            if( xOTA_Agent.xOTA_EventFlags != NULL )
            {
                ( void ) xEventGroupSetBits( xOTA_Agent.xOTA_EventFlags, OTA_EVT_MASK_USER_ABORT );
                xErr = kOTA_Err_None;
                /* xOTA_Agent.eImageState will be set later when the event is processed. */
            }
            else
            {
                OTA_LOG_L1( "[%s] FW Error! Missing event flag variable.\r\n", OTA_METHOD_NAME );
                xErr = kOTA_Err_Panic;
            }

            break;

        case eOTA_ImageState_Rejected:
            xErr = prvSetImageStateWithReason( eState, ( uint32_t ) NULL );
            break;

        case eOTA_ImageState_Accepted:
            xErr = prvSetImageStateWithReason( eState, ( uint32_t ) NULL );

            if( xErr == kOTA_Err_None )
            {
                /* Reset the self-test flag.*/
                xInSelfTest = false;
            }

            break;

        default: /*lint -e788 Keep lint quiet about the obvious unused states we're catching here. */
            xErr = kOTA_Err_BadImageState;
            break;
    }

    return xErr;
}


OTA_ImageState_t OTA_GetImageState( void )
{
    return xOTA_Agent.eImageState;
}


static OTA_Err_t prvSetImageStateWithReason( OTA_ImageState_t eState,
                                             uint32_t ulReason )
{
    OTA_Err_t xErr = kOTA_Err_Uninitialized;

    if( ( eState > eOTA_ImageState_Unknown ) && ( eState <= eOTA_LastImageState ) )
    {
        /* Call the platform specific code to set the image state. */
        xErr = prvPAL_SetPlatformImageState( eState );

        /* If the platform image state couldn't be set correctly, force fail the update. */
        if( xErr != kOTA_Err_None )
        {
            /* Maintain Aborted since it's also a failed OTA and we want the initial failure type. */
            if( eState != eOTA_ImageState_Aborted )
            {
                eState = eOTA_ImageState_Rejected; /*lint !e9044 intentionally override eState since we failed within this function. */

                if( ulReason == kOTA_Err_None )
                {
                    /* Capture the failure reason if not already set (and we're not already Aborted as checked above). */
                    ulReason = ( uint32_t ) xErr; /*lint !e9044 intentionally override lReason since we failed within this function. */
                }
                else
                {
                    /* Keep the original reject reason code since it is possible for the PAL
                     * to fail to update the image state in some cases (e.g. a reset already
                     * caused the bundle rollback and we failed to rollback again). */
                }
            }
            else
            {
                /* If it was aborted, keep the original abort reason code. That's more useful
                 * to the OTA operator. */
            }
        }

        xOTA_Agent.eImageState = eState;

        if( xOTA_Agent.pucOTA_Singleton_ActiveJobName != NULL )
        {
            if( eState == eOTA_ImageState_Testing )
            {   /* We discovered we're ready for test mode, put job status in self_test active. */
                prvUpdateJobStatus( NULL, eJobStatus_InProgress, ( int32_t ) eJobReason_SelfTestActive, ( int32_t ) NULL );
            }
            else
            {
                if( eState == eOTA_ImageState_Accepted )
                {   /* Now that we've accepted the firmware update, we can complete the job. */
                    prvStopSelfTestTimer();
                    prvUpdateJobStatus( NULL, eJobStatus_Succeeded, ( int32_t ) eJobReason_Accepted, xAppFirmwareVersion.u.lVersion32 );
                }
                else if( eState == eOTA_ImageState_Rejected )
                {
                    /* The firmware update was rejected, complete the job as FAILED (Job service
                     * doesn't allow us to set REJECTED after the job has been started already). */
                    prvUpdateJobStatus( NULL, eJobStatus_Failed, ( int32_t ) eJobReason_Rejected, ( int32_t ) ulReason );
                }
                else /* All other states have been checked so it must be ABORTED. */
                { /* Complete the job as FAILED. */
                    prvUpdateJobStatus( NULL, eJobStatus_Failed, ( int32_t ) eJobReason_Aborted, ( int32_t ) ulReason );
                }

                /* We don't need the job name memory anymore since we're done with this job. */
                vPortFree( xOTA_Agent.pucOTA_Singleton_ActiveJobName );
                xOTA_Agent.pucOTA_Singleton_ActiveJobName = NULL;
            }

            xErr = kOTA_Err_None;
        }
        else
        {
            xErr = kOTA_Err_NoActiveJob;
        }
    }
    else
    {
        xErr = kOTA_Err_BadImageState;
    }

    return xErr;
}



/* Update the job status on the service side with progress or completion info. */

static void prvUpdateJobStatus( OTA_FileContext_t * C,
                                OTA_JobStatus_t eStatus,
                                int32_t lReason,
                                int32_t lSubReason )
{
    DEFINE_OTA_METHOD_NAME( "prvUpdateJobStatus" );

    uint32_t ulTopicLen, ulNumBlocks, ulReceived, ulMsgSize;
    MQTTAgentReturnCode_t eResult;
    MQTTQoS_t eQOS;
    char cMsg[ OTA_STATUS_MSG_MAX_SIZE ];
    char cTopicBuffer[ OTA_MAX_TOPIC_LEN ];

    /* All job state transitions except streaming progress use QOS 1 since it is required to have status in the job document. */
    eQOS = eMQTTQoS1;

    /* A message size of zero means don't publish anything. */
    ulMsgSize = 0UL;

    if( eStatus == eJobStatus_InProgress )
    {
        if( lReason == ( int32_t ) eJobReason_Receiving )
        {
            if( C != NULL )
            {
                ulNumBlocks = ( C->ulFileSize + ( OTA_FILE_BLOCK_SIZE - 1U ) ) >> otaconfigLOG2_FILE_BLOCK_SIZE;
                ulReceived = ulNumBlocks - C->ulBlocksRemaining;

                if( ( ulReceived % OTA_UPDATE_STATUS_FREQUENCY ) == 0U ) /* Output a status update once in a while. */
                {
                    /* Downgrade Progress updates to QOS 0 to avoid overloading MQTT buffers during active streaming. */
                    eQOS = eMQTTQoS0;
                    ulMsgSize = ( uint32_t ) snprintf( cMsg, /*lint -e586 Intentionally using snprintf. */
                                                       sizeof( cMsg ),
                                                       cOTA_JobStatus_StatusTemplate,
                                                       pcOTA_JobStatus_Strings[ eStatus ] );
                    ulMsgSize += ( uint32_t ) snprintf( &cMsg[ ulMsgSize ], /*lint -e586 Intentionally using snprintf. */
                                                        sizeof( cMsg ) - ulMsgSize,
                                                        cOTA_JobStatus_ReceiveDetailsTemplate,
                                                        cOTA_String_Receive,
                                                        ulReceived,
                                                        ulNumBlocks );
                }
                else
                {   /* Don't send a status update yet. */
                    ulMsgSize = 0UL;
                }
            }
            else
            {
                /* Can't send a status update without data from the OTA context. Some calls intentionally
                 * don't use a context structure but never with this reason code so log this error. */
                OTA_LOG_L1( "[%s] Error: null context pointer!\r\n", OTA_METHOD_NAME );
                ulMsgSize = 0UL;
            }
        }
        else
        {
            /* We're no longer receiving but we're still In Progress so we are implicitly in the Self
             * Test phase. Prepare to update the job status with the self_test phase (ready or active). */
            ulMsgSize = ( uint32_t ) snprintf( cMsg, /*lint -e586 Intentionally using snprintf. */
                                               sizeof( cMsg ),
                                               cOTA_JobStatus_StatusTemplate,
                                               pcOTA_JobStatus_Strings[ eStatus ] );
            ulMsgSize += ( uint32_t ) snprintf( &cMsg[ ulMsgSize ], /*lint -e586 Intentionally using snprintf. */
                                                sizeof( cMsg ) - ulMsgSize,
                                                cOTA_JobStatus_SelfTestDetailsTemplate,
                                                cOTA_JSON_SelfTestKey,
                                                pcOTA_JobReason_Strings[ lReason ],
                                                xAppFirmwareVersion.u.ulVersion32 );
        }
    }
    else
    {
        if( eStatus < eNumJobStatusMappings )
        {
            /* Status updates that are NOT "IN PROGRESS" or "SUCCEEDED" map status and reason codes
             * to a string plus a sub-reason code except for FailedWithVal status. FailedWithVal uses
             * a numeric OTA error code and sub-reason code to cover the case where there may be too
             * many description strings to reasonably include in the code.
             */
            ulMsgSize = ( uint32_t ) snprintf( cMsg, /*lint -e586 Intentionally using snprintf. */
                                               sizeof( cMsg ),
                                               cOTA_JobStatus_StatusTemplate,
                                               pcOTA_JobStatus_Strings[ eStatus ] );

            if( eStatus == eJobStatus_FailedWithVal )
            {
                ulMsgSize += ( uint32_t ) snprintf( &cMsg[ ulMsgSize ], /*lint -e586 Intentionally using snprintf. */
                                                    sizeof( cMsg ) - ulMsgSize,
                                                    cOTA_JobStatus_ReasonValTemplate,
                                                    lReason,
                                                    lSubReason );
            }

            /* If the status update is for "SUCCEEDED," we are identifying the version of firmware
             * that has been accepted. This makes it easy to find the version associated with each
             * device (aka Thing) when examining the OTA jobs on the service side via the CLI or
             * possibly with some console tool.
             */
            else if( eStatus == eJobStatus_Succeeded )
            {
                AppVersion32_t xNewVersion;

                xNewVersion.u.lVersion32 = lSubReason;
                ulMsgSize += ( uint32_t ) snprintf( &cMsg[ ulMsgSize ], /*lint -e586 Intentionally using snprintf. */
                                                    sizeof( cMsg ) - ulMsgSize,
                                                    cOTA_JobStatus_SucceededStrTemplate,
                                                    pcOTA_JobReason_Strings[ lReason ],
                                                    xNewVersion.u.x.ucMajor,
                                                    xNewVersion.u.x.ucMinor,
                                                    xNewVersion.u.x.usBuild );
            }
            else
            {
                ulMsgSize += ( uint32_t ) snprintf( &cMsg[ ulMsgSize ], /*lint -e586 Intentionally using snprintf. */
                                                    sizeof( cMsg ) - ulMsgSize,
                                                    cOTA_JobStatus_ReasonStrTemplate,
                                                    pcOTA_JobReason_Strings[ lReason ],
                                                    lSubReason );
            }
        }
        else
        {   /* Unknown status code. Just ignore it. */
            ulMsgSize = 0UL;
        }
    }

    if( ulMsgSize > 0UL )
    {
        /* Try to build the dynamic job status topic . */
        ulTopicLen = ( uint32_t ) snprintf( cTopicBuffer, /*lint -e586 Intentionally using snprintf. */
                                            sizeof( cTopicBuffer ),
                                            cOTA_JobStatus_TopicTemplate,
                                            xOTA_Agent.ucThingName,
                                            xOTA_Agent.pucOTA_Singleton_ActiveJobName );

        /* If the topic name was built, try to publish the status message to it. */
        if( ( ulTopicLen > 0UL ) && ( ulTopicLen < sizeof( cTopicBuffer ) ) )
        {
            OTA_LOG_L1( "[%s] Msg: %s\r\n", OTA_METHOD_NAME, cMsg );
            eResult = prvPublishMessage(
                xOTA_Agent.pvPubSubClient,
                cTopicBuffer,
                ( uint16_t ) ulTopicLen,
                &cMsg[ 0 ],
                ulMsgSize,
                eQOS );

            if( eResult != eMQTTAgentSuccess )
            {
                OTA_LOG_L1( "[%s] Failed: %s\r\n", OTA_METHOD_NAME, cTopicBuffer );
            }
            else
            {
                OTA_LOG_L1( "[%s] '%s' to %s\r\n", OTA_METHOD_NAME, pcOTA_JobStatus_Strings[ eStatus ], cTopicBuffer );
            }
        }
        else
        {
            OTA_LOG_L1( "[%s] Failed to build job status topic!\r\n", OTA_METHOD_NAME );
        }
    }
    else
    {
        /* Just ignore the zero length message. */
    }
}


/* Construct the "Get Stream" message and publish it to the stream service request topic. */

static OTA_Err_t prvPublishGetStreamMessage( OTA_FileContext_t * C )
{
    DEFINE_OTA_METHOD_NAME( "prvPublishGetStreamMessage" );

    uint32_t ulMsgSizeToPublish;
    size_t xMsgSizeFromStream;
    uint32_t ulNumBlocks, ulBitmapLen, ulTopicLen;
    MQTTAgentReturnCode_t eResult;
    OTA_Err_t xErr = kOTA_Err_None;
    char cMsg[ OTA_REQUEST_MSG_MAX_SIZE ];
    char cTopicBuffer[ OTA_MAX_TOPIC_LEN ];

    if( C != NULL )
    {
        if( C->ulRequestMomentum < OTA_MAX_STREAM_REQUEST_MOMENTUM )
        {
            ulNumBlocks = ( C->ulFileSize + ( OTA_FILE_BLOCK_SIZE - 1U ) ) >> otaconfigLOG2_FILE_BLOCK_SIZE;
            ulBitmapLen = ( ulNumBlocks + ( BITS_PER_BYTE - 1U ) ) >> LOG2_BITS_PER_BYTE;

            if( pdTRUE == OTA_CBOR_Encode_GetStreamRequestMessage(
                    ( uint8_t * ) cMsg,
                    sizeof( cMsg ),
                    &xMsgSizeFromStream,
                    OTA_CLIENT_TOKEN,
                    ( int32_t ) C->ulServerFileID,
                    ( int32_t ) ( OTA_FILE_BLOCK_SIZE & 0x7fffffffUL ), /* Mask to keep lint happy. It's still a constant. */
                    0,
                    C->pucRxBlockBitmap,
                    ulBitmapLen ) )
            {
                ulMsgSizeToPublish = ( uint32_t ) xMsgSizeFromStream;

                /* Each Get Stream Request increases the momentum until a response
                 * is received to ANY request. Too much momentum is interpreted as
                 * a failure to communicate and will cause us to abort the OTA. */
                C->ulRequestMomentum++;

                /* Try to build the dynamic data REQUEST topic and subscribe to it. */
                ulTopicLen = ( uint32_t ) snprintf( cTopicBuffer, /*lint -e586 Intentionally using snprintf. */
                                                    sizeof( cTopicBuffer ),
                                                    cOTA_GetStream_TopicTemplate,
                                                    xOTA_Agent.ucThingName,
                                                    ( const char * ) C->pucStreamName );

                if( ( ulTopicLen > 0U ) && ( ulTopicLen < sizeof( cTopicBuffer ) ) )
                {
                    eResult = prvPublishMessage(
                        xOTA_Agent.pvPubSubClient,
                        cTopicBuffer,
                        ( uint16_t ) ulTopicLen,
                        &cMsg[ 0 ],
                        ulMsgSizeToPublish,
                        eMQTTQoS0 );

                    if( eResult != eMQTTAgentSuccess )
                    {
                        OTA_LOG_L1( "[%s] Failed: %s\r\n", OTA_METHOD_NAME, cTopicBuffer );
                        /* Don't return an error. Let max momentum catch it since this may be intermittent. */
                    }
                    else
                    {
                        OTA_LOG_L1( "[%s] OK: %s\r\n", OTA_METHOD_NAME, cTopicBuffer );
                        /* Restart the request timer to retry if we don't complete the update. */
                        prvStartRequestTimer( C );
                    }
                }
                else
                {
                    /* 0 should never happen since we supply the format strings. It must be overflow. */
                    OTA_LOG_L1( "[%s] Failed to build stream topic!\r\n", OTA_METHOD_NAME );
                    xErr = kOTA_Err_TopicTooLarge;
                }
            }
            else
            {
                OTA_LOG_L1( "[%s] CBOR encode failed.\r\n", OTA_METHOD_NAME );
                xErr = kOTA_Err_FailedToEncodeCBOR;
            }
        }
        else
        {
            /* Too many requests have been sent without a response or too many failures
             * when trying to publish the request message. Abort. Store attempt count in low bits. */
            xErr = ( uint32_t ) kOTA_Err_MomentumAbort | ( OTA_MAX_STREAM_REQUEST_MOMENTUM & ( uint32_t ) kOTA_PAL_ErrMask );
        }
    }
    else
    {
        /* Defensive programming. */
    }

    return xErr;
}


/* This function is called whenever we receive a MQTT publish message on one of our OTA topics. */

static MQTTBool_t prvOTAPublishCallback( void * pvCallbackContext,
                                         const MQTTPublishData_t * const pxPublishData )
{
    DEFINE_OTA_METHOD_NAME_L2( "prvOTAPublishCallback" );

    MQTTBool_t xTakeOwnership = eMQTTFalse;
    BaseType_t xReturn;
    OTA_PubMsg_t xMsg;

    /* If we're running the OTA task, send publish messages to it for processing. */
    if( xOTA_Agent.xOTA_EventFlags != NULL )
    {
        xOTA_Agent.xStatistics.ulOTA_PacketsReceived++;
        xMsg.lMsgType = ( int32_t ) pvCallbackContext; /*lint !e923 The context variable is actually the message type. */
        xMsg.xPubData = *pxPublishData;
        xReturn = xQueueSendToBack( xOTA_Agent.xOTA_MsgQ, &xMsg, ( TickType_t ) 0 );

        if( xReturn == pdPASS )
        {
            xOTA_Agent.xStatistics.ulOTA_PacketsQueued++;
            ( void ) xEventGroupSetBits( xOTA_Agent.xOTA_EventFlags, OTA_EVT_MASK_MSG_READY );
            /* Take ownership of the MQTT buffer. */
            xTakeOwnership = eMQTTTrue;
        }
        else
        {
            xOTA_Agent.xStatistics.ulOTA_PacketsDropped++;
        }
    }
    else
    {
        /* This doesn't normally occur unless we're subscribed to an OTA topic when
         * the OTA agent is not initialized. Just drop the message by not taking
         * ownership since we don't know if we'll ever be able to process it. */
        OTA_LOG_L2( "[%s] Warning: Received MQTT message but agent isn't ready.\r\n", OTA_METHOD_NAME );
        xOTA_Agent.xStatistics.ulOTA_PacketsDropped++;
    }

    return xTakeOwnership;
}


/* NOTE: This implementation only supports 1 OTA context. Concurrent OTA is not supported. */

static void prvOTAUpdateTask( void * pvUnused )
{
    DEFINE_OTA_METHOD_NAME( "prvOTAUpdateTask" );

    EventBits_t xBits;
    OTA_FileContext_t * pxC = NULL;
    OTA_Err_t xErr;

    ( void ) pvUnused;
    OTA_PubMsg_t xMsgMetaData;

    /* Subscribe to the OTA job notification topic. */
    if( prvSubscribeToJobNotificationTopics() == ( bool_t ) pdTRUE )
    {
        xOTA_Agent.xOTA_EventFlags = xEventGroupCreate();

        if( xOTA_Agent.xOTA_EventFlags != NULL )
        {
            /* Check if the firmware image is in self test mode. If so, enable the self test timer. */
            ( void ) OTA_CheckForSelfTest();

            /*Save the self-test mode in a flag. This flag is cleared when self-test is successful.*/
            xInSelfTest = prvInSelftest();

            /* Send a request to the job service for the latest available job. */
            ( void ) OTA_CheckForUpdate();

            /* Put the OTA agent in the ready state. */
            xOTA_Agent.eState = eOTA_AgentState_Ready;

            for( ; ; )
            {
                xBits = xEventGroupWaitBits(
                    xOTA_Agent.xOTA_EventFlags, /* The event group being tested. */
                    OTA_EVT_MASK_ALL_EVENTS,    /* The bits within the event group to wait for. */
                    pdTRUE,                     /* Bits should be cleared before returning. */
                    pdFALSE,                    /* Any bit set will do. */
                    ( TickType_t ) ~( 0U ) );   /* Wait forever. */

                /* Check for the shutdown event. */
                if( ( ( uint32_t ) xBits & OTA_EVT_MASK_SHUTDOWN ) != 0U )
                {
                    OTA_LOG_L1( "[%s] Received shutdown event.\r\n", OTA_METHOD_NAME );
                    break; /* Break, so we stop all OTA processing. */
                }

                /* Check for a user initiated abort. */
                if( ( ( ( uint32_t ) xBits & OTA_EVT_MASK_USER_ABORT ) != 0U ) && ( pxC != NULL ) )
                {
                    OTA_LOG_L1( "[%s] Received user abort event.\r\n", OTA_METHOD_NAME );
                    ( void ) prvSetImageStateWithReason( eOTA_ImageState_Aborted, kOTA_Err_UserAbort );
                    ( void ) prvOTA_Close( pxC ); /* Ignore false result since we're setting the pointer to null on the next line. */
                    pxC = NULL;
                }

                /* On OTA request timer timeout, publish the stream request if we have context. */
                if( ( ( xBits & OTA_EVT_MASK_REQ_TIMEOUT ) != 0U ) && ( pxC != NULL ) )
                {
                    if( pxC->ulBlocksRemaining > 0U )
                    {
                        xErr = prvPublishGetStreamMessage( pxC );

                        if( xErr != kOTA_Err_None )
                        {                                 /* Abort the current OTA. */
                            ( void ) prvSetImageStateWithReason( eOTA_ImageState_Aborted, xErr );
                            ( void ) prvOTA_Close( pxC ); /* Ignore false result since we're setting the pointer to null on the next line. */
                            pxC = NULL;
                        }
                    }
                    else
                    {
                        /* If there are no blocks remaining, this may have been in flight when the
                         * last block was received. Just ignore it since it will be cleaned up by
                         * the agent as required. */
                    }
                }

                /* Check if a MQTT message is ready for us to process. */
                if( ( xBits & OTA_EVT_MASK_MSG_READY ) != 0U )
                {
                    xMsgMetaData.xPubData.xBuffer = NULL;

                    if( ( xOTA_Agent.eState == eOTA_AgentState_Ready ) || ( xOTA_Agent.eState == eOTA_AgentState_Active ) )
                    {
                        while( xQueueReceive( xOTA_Agent.xOTA_MsgQ, &xMsgMetaData, 0 ) != pdFALSE )
                        {
                            /* Check for OTA update job messages. */
                            if( xMsgMetaData.eMsgType == eOTA_PubMsgType_Job )
                            {
                                if( pxC != NULL )
                                {
                                    ( void ) prvSetImageStateWithReason( eOTA_ImageState_Aborted, kOTA_Err_UserAbort );
                                    ( void ) prvOTA_Close( pxC ); /* Abort the existing OTA and ignore impossible false result by design. */
                                }

                                pxC = prvProcessOTAJobMsg( ( const char * ) xMsgMetaData.xPubData.pvData, /*lint !e9079 pointer to void is OK to cast to the real type. */
                                                           xMsgMetaData.xPubData.ulDataLength );

                                /* A null context here could either mean we didn't receive a valid job or it could
                                 * signify that we're in the self test phase (where the OTA file transfer is already
                                 * completed and we've reset the device and are now running the new firmware). We
                                 * will check the state to determine which case we're in. */
                                if( pxC == NULL )
                                {
                                    /* If the OTA job is in the self_test state, alert the application layer. */
                                    if( OTA_GetImageState() == eOTA_ImageState_Testing )
                                    {
                                        /* Check the platform's OTA update image state. It should also be in self test. */
                                        if( OTA_CheckForSelfTest() == pdTRUE )
                                        {
                                            xOTA_Agent.xOTAJobCompleteCallback( eOTA_JobEvent_StartTest );
                                        }
                                        else
                                        {
                                            /* The job is in self test but the platform image state is not so it could be
                                             * an attack on the platform image state. Reject the update (this should also
                                             * cause the image to be erased), aborting the job and reset the device. */
                                            OTA_LOG_L1( "[%s] Job in self test but platform state is not!\r\n", OTA_METHOD_NAME );
                                            ( void ) prvSetImageStateWithReason( eOTA_ImageState_Rejected, kOTA_Err_ImageStateMismatch );
                                            ( void ) prvResetDevice(); /* Ignore return code since there's nothing we can do if we can't force reset. */
                                        }
                                    }
                                }
                                else
                                {
                                    xOTA_Agent.eState = eOTA_AgentState_Active;
                                }
                            }
                            /* It's not a job message, maybe it's a data stream message... */
                            else if( xMsgMetaData.eMsgType == eOTA_PubMsgType_Stream )
                            {
                                /* Ingest data blocks if the platform is not in self-test. */
                                if( ( pxC != NULL ) && ( xInSelfTest == false ) )
                                {
                                    OTA_Err_t xCloseResult;
                                    IngestResult_t xResult = prvIngestDataBlock( pxC,
                                                                                 ( const char * ) xMsgMetaData.xPubData.pvData, /*lint !e9079 pointer to void is OK to cast to the real type. */
                                                                                 xMsgMetaData.xPubData.ulDataLength,
                                                                                 &xCloseResult );

                                    if( xResult < eIngest_Result_Accepted_Continue )
                                    {
                                        /* Negative result codes mean we should stop the OTA process
                                         * because we are either done or in an unrecoverable error state.
                                         * We don't want to hang on to the resources. */

                                        if( xResult == eIngest_Result_FileComplete )
                                        {
                                            /* File receive is complete and authenticated. Update the job status with the self_test ready identifier. */
                                            prvUpdateJobStatus( pxC, eJobStatus_InProgress, ( int32_t ) eJobReason_SigCheckPassed, ( int32_t ) NULL );
                                        }
                                        else
                                        {
                                            OTA_LOG_L1( "[%s] Aborting due to IngestResult_t error %d\r\n", OTA_METHOD_NAME, ( int32_t ) xResult );
                                            /* Call the platform specific code to reject the image. */
                                            xErr = prvPAL_SetPlatformImageState( eOTA_ImageState_Rejected );

                                            if( xErr != kOTA_Err_None )
                                            {
                                                OTA_LOG_L2( "[%s] Error trying to set platform image state (0x%08x)\r\n", OTA_METHOD_NAME, ( int32_t ) xErr );
                                            }
                                            else
                                            {
                                                /* Nothing special to do on success. */
                                            }

                                            prvUpdateJobStatus( pxC, eJobStatus_FailedWithVal, ( int32_t ) xCloseResult, ( int32_t ) xResult );
                                        }

                                        /* Release all remaining resources of the OTA file. */
                                        ( void ) prvOTA_Close( pxC ); /* Ignore false result since we're setting the pointer to null on the next line. */
                                        pxC = NULL;

                                        /* Let main application know of our result. */
                                        xOTA_Agent.xOTAJobCompleteCallback( ( xResult == eIngest_Result_FileComplete ) ? eOTA_JobEvent_Activate : eOTA_JobEvent_Fail );

                                        /* Free any remaining string memory holding the job name since this job is done. */
                                        if( xOTA_Agent.pucOTA_Singleton_ActiveJobName != NULL )
                                        {
                                            vPortFree( xOTA_Agent.pucOTA_Singleton_ActiveJobName );
                                            xOTA_Agent.pucOTA_Singleton_ActiveJobName = NULL;
                                        }
                                    }
                                    else
                                    {   /* We're actively receiving a file so update the job status as needed. */
                                      /* First reset the momentum counter since we received a good block. */
                                        pxC->ulRequestMomentum = 0;
                                        prvUpdateJobStatus( pxC, eJobStatus_InProgress, ( int32_t ) eJobReason_Receiving, ( int32_t ) NULL );
                                    }
                                }
                            }
                            else
                            {
                                /* Ignore unknown message types. */
                                OTA_LOG_L2( "[%s] Ignoring unknown message type %d.\r\n", OTA_METHOD_NAME, xMsgMetaData.lMsgType );
                            }

                            if( xMsgMetaData.xPubData.xBuffer != NULL )
                            {
                                xOTA_Agent.xStatistics.ulOTA_PacketsProcessed++;

                                /* Return the MQTT buffer since we're done with it (even if we ignored the message). */
                                if( ( MQTT_AGENT_ReturnBuffer( xOTA_Agent.pvPubSubClient, xMsgMetaData.xPubData.xBuffer ) ) == eMQTTAgentSuccess )
                                {
                                    OTA_LOG_L2( "[%s] Returned buffer to MQTT client.\r\n", OTA_METHOD_NAME );
                                }
                                else
                                {
                                    OTA_LOG_L1( "[%s] MQTT_AGENT_ReturnBuffer() failed.\r\n", OTA_METHOD_NAME );
                                    break;
                                }
                            }
                            else
                            {
                                /* Safety ignore messages not associated with an MQTT buffer. */
                            }
                        }
                    }
                }

                if( pxC == NULL )
                {   /* Any event that releases the context structure tells us we're not active anymore. */
                    xOTA_Agent.eState = eOTA_AgentState_Ready;
                }
            }

            /* If we're here, we're shutting down the OTA agent. Free up all resources and quit. */
            prvAgentShutdownCleanup( &xMsgMetaData );
            vEventGroupDelete( xOTA_Agent.xOTA_EventFlags );
        }
    }

    /* Clear the entire agent context. This includes the OTA agent state. */
    memset( &xOTA_Agent, 0, sizeof( xOTA_Agent ) );

    /* Reset the image and agent states. Possibly redundant but safer.
     * Finally, self destruct. */
    xOTA_Agent.eImageState = eOTA_ImageState_Unknown;
    xOTA_Agent.eState = eOTA_AgentState_NotReady;
    vTaskDelete( NULL );
}


/* If we're in self test mode, start the self test timer. The latest job should
 * also be in the self test state. We must complete the self test and either
 * accept or reject the image before the timer expires or we shall reset the
 * device to initiate a roll back of the MCU firmware bundle. This will catch
 * issues like improper credentials or a device that is connected to a non-
 * production stage of the job service resulting in a different job queue being
 * used.
 */
static BaseType_t OTA_CheckForSelfTest( void )
{
    DEFINE_OTA_METHOD_NAME( "OTA_CheckForSelfTest" );
    static const char cTimerName[] = "OTA_SelfTest";
    BaseType_t xTimerStarted = pdFALSE;
    static StaticTimer_t xTimerBuffer;

    if( prvPAL_GetPlatformImageState() == eOTA_PAL_ImageState_PendingCommit )
    {
        if( xOTA_Agent.xSelfTestTimer == NULL )
        {
            xOTA_Agent.xSelfTestTimer = xTimerCreateStatic( cTimerName,
                                                            pdMS_TO_TICKS( otaconfigSELF_TEST_RESPONSE_WAIT_MS ),
                                                            pdFALSE, NULL, prvSelfTestTimer_Callback,
                                                            &xTimerBuffer );

            if( xOTA_Agent.xSelfTestTimer != NULL )
            {
                xTimerStarted = xTimerStart( xOTA_Agent.xSelfTestTimer, portMAX_DELAY );
            }
            else
            {
                /* Static timers are guaranteed to be created unless we pass in a NULL buffer. */
            }
        }
        else
        {
            xTimerStarted = xTimerReset( xOTA_Agent.xSelfTestTimer, portMAX_DELAY );
        }

        /* Common check for whether the timer was started or not. It should be impossible to not start. */
        if( xTimerStarted == pdTRUE )
        {
            OTA_LOG_L1( "[%s] Starting %s timer.\r\n", OTA_METHOD_NAME, cTimerName );
        }
        else
        {
            OTA_LOG_L1( "[%s] ERROR: failed to reset/start %s timer.\r\n", OTA_METHOD_NAME, cTimerName );
        }
    }

    return xTimerStarted;
}


/* When the self test response timer expires, reset the device since we're likely broken. */

static void prvSelfTestTimer_Callback( TimerHandle_t T )
{
    DEFINE_OTA_METHOD_NAME( "prvSelfTestTimer_Callback" );
    ( void ) T;

    OTA_LOG_L1( "[%s] Self test failed to complete within %ums\r\n", OTA_METHOD_NAME, otaconfigSELF_TEST_RESPONSE_WAIT_MS );
    ( void ) prvPAL_ResetDevice();
}


/* Stop the OTA self test timer if it is running. */

static void prvStopSelfTestTimer( void )
{
    DEFINE_OTA_METHOD_NAME( "prvStopSelfTestTimer" );

    if( xOTA_Agent.xSelfTestTimer != NULL )
    {
        ( void ) xTimerStop( xOTA_Agent.xSelfTestTimer, portMAX_DELAY );
        OTA_LOG_L1( "[%s] Stopping the self test timer.\r\n", OTA_METHOD_NAME );
    }
}


/* When the OTA request timer expires, signal the OTA task to request the file. */

static void prvRequestTimer_Callback( TimerHandle_t T )
{
    ( void ) T;

    if( xOTA_Agent.xOTA_EventFlags != NULL )
    {
        ( void ) xEventGroupSetBits( xOTA_Agent.xOTA_EventFlags, OTA_EVT_MASK_REQ_TIMEOUT );
    }
}


/* Create and start or reset the OTA request timer to kick off the process if needed.
 * Do not output an important log message on reset since this gets called every time a file
 * block is received. Use log level 2 at most.
 */
static void prvStartRequestTimer( OTA_FileContext_t * C )
{
    DEFINE_OTA_METHOD_NAME( "prvStartRequestTimer" );
    static const char cTimerName[] = "OTA_FileRequest";

    BaseType_t xTimerStarted = pdFALSE;

    if( C->xRequestTimer == NULL )
    {
        C->xRequestTimer = xTimerCreate( cTimerName,
                                         pdMS_TO_TICKS( otaconfigFILE_REQUEST_WAIT_MS ),
                                         pdFALSE,
                                         ( void * ) C, /*lint !e9087 Using the file context as the timer ID does not cause undefined behavior. */
                                         prvRequestTimer_Callback );

        if( C->xRequestTimer != NULL )
        {
            xTimerStarted = xTimerStart( C->xRequestTimer, 0 );
        }
    }
    else
    {
        xTimerStarted = xTimerReset( C->xRequestTimer, portMAX_DELAY );
    }

    if( xTimerStarted == pdTRUE )
    {
        OTA_LOG_L2( "[%s] Starting %s timer.\r\n", OTA_METHOD_NAME, cTimerName );
    }
    else
    {
        OTA_LOG_L1( "[%s] ERROR: failed to reset/start %s timer.\r\n", OTA_METHOD_NAME, cTimerName );
    }
}


/* Stop the firmware request timer if it is running. */

static void prvStopRequestTimer( OTA_FileContext_t * C )
{
    DEFINE_OTA_METHOD_NAME( "prvStopRequestTimer" );

    if( C->xRequestTimer != NULL )
    {
        ( void ) xTimerStop( C->xRequestTimer, 0 );
        OTA_LOG_L1( "[%s] Stopping file request timer.\r\n", OTA_METHOD_NAME );
    }
}


/* Close an existing OTA context and free its resources. */

static bool_t prvOTA_Close( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvOTA_Close" );

    bool_t xResult = pdFALSE;

    OTA_LOG_L1( "[%s] Context->0x%08x\r\n", OTA_METHOD_NAME, C );

    if( C != NULL )
    {
        /* Stop and delete any existing transfer request timer. */
        if( C->xRequestTimer != NULL )
        {
            ( void ) xTimerStop( C->xRequestTimer, 0 );
            ( void ) xTimerDelete( C->xRequestTimer, 0 );
            C->xRequestTimer = NULL;
        }

        if( C->pucStreamName != NULL )
        {
            ( void ) prvUnSubscribeFromDataStream( C ); /* Unsubscribe from the data stream if needed. */
            vPortFree( C->pucStreamName );              /* Free any previously allocated stream name memory. */
            C->pucStreamName = NULL;
        }

        if( C->pucJobName != NULL )
        {
            vPortFree( C->pucJobName ); /* Free the job name memory. */
            C->pucJobName = NULL;
        }

        if( C->pucRxBlockBitmap != NULL )
        {
            vPortFree( C->pucRxBlockBitmap ); /* Free the previously allocated block bitmap. */
            C->pucRxBlockBitmap = NULL;
        }

        if( C->pxSignature != NULL )
        {
            vPortFree( C->pxSignature ); /* Free the image signature memory. */
            C->pxSignature = NULL;
        }

        if( C->pucFilePath != NULL )
        {
            vPortFree( C->pucFilePath ); /* Free the file path name string memory. */
            C->pucFilePath = NULL;
        }

        if( C->pucCertFilepath != NULL )
        {
            vPortFree( C->pucCertFilepath ); /* Free the certificate path name string memory. */
            C->pucCertFilepath = NULL;
        }

        /* Abort any active file access and release the file resource, if needed. */
        ( void ) prvPAL_Abort( C );
        memset( C, 0, sizeof( OTA_FileContext_t ) ); /* Clear the entire structure now that it is free. */
        xResult = pdTRUE;
    }

    return xResult;
}


/* Find an available OTA transfer context structure. */

static OTA_FileContext_t * prvGetFreeContext( void )
{
    uint32_t ulIndex = 0U;
    OTA_FileContext_t * pxC = NULL;

    while( ( ulIndex < OTA_MAX_FILES ) && ( xOTA_Agent.xOTA_Files[ ulIndex ].pucFilePath != NULL ) )
    {
        ulIndex++;
    }

    if( ulIndex != OTA_MAX_FILES )
    {
        memset( &xOTA_Agent.xOTA_Files[ ulIndex ], 0, sizeof( OTA_FileContext_t ) );
        pxC = &xOTA_Agent.xOTA_Files[ ulIndex ];
    }
    else
    {
        /* Not able to support this request so we'll return NULL. */
    }

    return pxC;
}


bool_t JSON_IsCStringEqual( const char * pcJSONString,
                            uint32_t ulLen,
                            const char * pcCString )
{
    bool_t xResult;

    if( ( ulLen <= OTA_MAX_JSON_STR_LEN ) &&
        ( strncmp( pcJSONString, pcCString, ulLen ) == 0 ) && /*lint !e9007 I see no side effect. Possibly a lint bug. */
        ( pcCString[ ulLen ] == '\0' ) )
    {
        xResult = pdTRUE;
    }
    else
    {
        xResult = pdFALSE;
    }

    return xResult;
}


/* Search our document model for a key match with the given token. */

static DocParseErr_t prvSearchModelForTokenKey( JSON_DocModel_t * pxDocModel,
                                                const char * pcJSONString,
                                                uint32_t ulStrLen,
                                                uint16_t * pusMatchingIndexResult )
{
    DocParseErr_t eErr = eDocParseErr_ParamKeyNotInModel;
    uint16_t usParamIndex;

    for( usParamIndex = 0; usParamIndex < pxDocModel->usNumModelParams; usParamIndex++ )
    {
        if( JSON_IsCStringEqual( pcJSONString, ulStrLen,
                                 pxDocModel->pxBodyDef[ usParamIndex ].pcSrcKey ) == ( bool_t ) pdTRUE )
        {
            /* Per Security, don't allow multiple entries of the same parameter. */
            if( ( pxDocModel->ulParamsReceivedBitmap & ( 1U << usParamIndex ) ) != 0U ) /*lint !e9032 usParamIndex will never be greater than kDocModel_MaxParams, which is the the size of the bitmap. */
            {
                eErr = eDocParseErr_DuplicatesNotAllowed;
            }
            else
            {
                /* Mark parameter as received in the bitmap. */
                pxDocModel->ulParamsReceivedBitmap |= ( 1U << usParamIndex ); /*lint !e9032 usParamIndex will never be greater than kDocModel_MaxParams, which is the the size of the bitmap. */
                *pusMatchingIndexResult = usParamIndex;                       /* Save result index for caller. */
                eErr = eDocParseErr_None;                                     /* We found a matching key in the document model. */
            }

            break; /* We found a key match so stop searching. */
        }
    }

    return eErr;
}



/* Extract the desired fields from the JSON document based on the specified document model. */

static DocParseErr_t prvParseJSONbyModel( const char * pcJSON,
                                          uint32_t ulMsgLen,
                                          JSON_DocModel_t * pxDocModel )
{
    DEFINE_OTA_METHOD_NAME( "prvParseJSONbyModel" );

    const JSON_DocParam_t * pxModelParam;
    jsmn_parser xParser;
    jsmntok_t * pxTokens;
    jsmntok_t * pxValTok;
    uint32_t ulNumTokens, ulTokenLen;
    MultiParmPtr_t xParamAddr; /*lint !e9018 We intentionally use this union to cast the parameter address to the proper type. */
    uint32_t ulIndex;
    uint16_t usModelParamIndex;
    uint32_t ulScanIndex;
    DocParseErr_t eErr = eDocParseErr_Unknown;


    /* Reset the Jasmine tokenizer. */
    jsmn_init( &xParser );

    /* Validate some initial parameters. */
    if( pxDocModel == NULL )
    {
        OTA_LOG_L1( "[%s] The pointer to the document model is NULL.\r\n", OTA_METHOD_NAME );
        eErr = eDocParseErr_NullModelPointer;
    }
    else if( pxDocModel->pxBodyDef == NULL )
    {
        OTA_LOG_L1( "[%s] Document model 0x%08x body pointer is NULL.\r\n", OTA_METHOD_NAME, pxDocModel );
        eErr = eDocParseErr_NullBodyPointer;
    }
    else if( pxDocModel->usNumModelParams > OTA_DOC_MODEL_MAX_PARAMS )
    {
        OTA_LOG_L1( "[%s] Model has too many parameters (%u).\r\n", OTA_METHOD_NAME, pxDocModel->usNumModelParams );
        eErr = eDocParseErr_TooManyParams;
    }
    else if( pcJSON == NULL )
    {
        OTA_LOG_L1( "[%s] JSON document pointer is NULL!\r\n", OTA_METHOD_NAME );
        eErr = eDocParseErr_NullDocPointer;
    }
    else
    {
        pxModelParam = pxDocModel->pxBodyDef;

        /* Count the total number of tokens in our JSON document. */
        ulNumTokens = ( uint32_t ) jsmn_parse( &xParser, pcJSON, ( size_t ) ulMsgLen, NULL, 1UL );

        if( ulNumTokens > 0U )
        {
            /* If the JSON document isn't too big for our token array... */
            if( ulNumTokens <= OTA_MAX_JSON_TOKENS )
            {
                /* Allocate space for the document JSON tokens. */
                void * pvTokenArray = pvPortMalloc( ulNumTokens * sizeof( jsmntok_t ) ); /* Allocate space on heap for temporary token array. */
                pxTokens = ( jsmntok_t * ) pvTokenArray;                                 /*lint !e9079 !e9087 heap allocations return void* so we allow casting to a pointer to the actual type. */

                if( pxTokens != NULL )
                {
                    /* Reset Jasmine again and tokenize the document for real. */
                    jsmn_init( &xParser );
                    ulIndex = ( uint32_t ) jsmn_parse( &xParser, pcJSON, ulMsgLen, pxTokens, ulNumTokens );

                    if( ulIndex == ulNumTokens )
                    {
                        /* Start the parser in an error free state. */
                        eErr = eDocParseErr_None;

                        /* Examine each JSON token, searching for job parameters based on our document model. */
                        for( ulIndex = 0U; ( eErr == eDocParseErr_None ) && ( ulIndex < ulNumTokens ); ulIndex++ )
                        {
                            /* All parameter keys are JSON strings. */
                            if( pxTokens[ ulIndex ].type == JSMN_STRING )
                            {
                                /* Search the document model to see if it matches the current key. */
                                ulTokenLen = ( uint32_t ) pxTokens[ ulIndex ].end - ( uint32_t ) pxTokens[ ulIndex ].start;
                                eErr = prvSearchModelForTokenKey( pxDocModel, &pcJSON[ pxTokens[ ulIndex ].start ], ulTokenLen, &usModelParamIndex );

                                /* If we didn't find a match in the model, skip over it and its descendants. */
                                if( eErr == eDocParseErr_ParamKeyNotInModel )
                                {
                                    int32_t lRoot = ( int32_t ) ulIndex; /* Create temp root from the unrecognized tokens index. Use signed int since the parent index is signed. */
                                    ulIndex++;                           /* Skip the active key since it's the one we don't recognize. */

                                    /* Skip tokens whose parents are equal to or deeper than the unrecognized temporary root token level. */
                                    while( ( ulIndex < ulNumTokens ) && ( pxTokens[ ulIndex ].parent >= lRoot ) )
                                    {
                                        ulIndex++; /* Skip over all descendants of the unknown parent. */
                                    }

                                    --ulIndex;                /* Adjust for outer for-loop increment. */
                                    eErr = eDocParseErr_None; /* Unknown key structures are simply skipped so clear the error state to continue. */
                                }
                                else if( eErr == eDocParseErr_None )
                                {
                                    /* We found the parameter key in the document model. */

                                    /* Get the value field (i.e. the following token) for the parameter. */
                                    pxValTok = &pxTokens[ ulIndex + 1UL ];

                                    /* Verify the field type is what we expect for this parameter. */
                                    if( pxValTok->type != pxModelParam[ usModelParamIndex ].eJasmineType )
                                    {
                                        ulTokenLen = ( uint32_t ) ( pxValTok->end ) - ( uint32_t ) ( pxValTok->start );
                                        OTA_LOG_L1( "[%s] parameter type mismatch [ %s : %.*s ] type %u, expected %u\r\n",
                                                    OTA_METHOD_NAME, pxModelParam[ usModelParamIndex ].pcSrcKey, ulTokenLen,
                                                    &pcJSON[ pxValTok->start ],
                                                    pxValTok->type, pxModelParam[ usModelParamIndex ].eJasmineType );
                                        eErr = eDocParseErr_FieldTypeMismatch;
                                        /* break; */
                                    }
                                    else if( OTA_DONT_STORE_PARAM == pxModelParam[ usModelParamIndex ].ulDestOffset )
                                    {
                                        /* Nothing to do with this parameter since we're not storing it. */
                                        continue;
                                    }
                                    else
                                    {
                                        /* Get destination offset to parameter storage location. */

                                        /* If it's within the models context structure, add in the context instance base address. */
                                        if( pxModelParam[ usModelParamIndex ].ulDestOffset < pxDocModel->ulContextSize )
                                        {
                                            xParamAddr.ulVal = pxDocModel->ulContextBase + pxModelParam[ usModelParamIndex ].ulDestOffset;
                                        }
                                        else
                                        {
                                            /* It's a raw pointer so keep it as is. */
                                            xParamAddr.ulVal = pxModelParam[ usModelParamIndex ].ulDestOffset;
                                        }

                                        if( eModelParamType_StringCopy == pxModelParam[ usModelParamIndex ].xModelParamType )
                                        {
                                            /* Malloc memory for a copy of the value string plus a zero terminator. */
                                            ulTokenLen = ( uint32_t ) ( pxValTok->end ) - ( uint32_t ) ( pxValTok->start );
                                            void * pvStringCopy = pvPortMalloc( ulTokenLen + 1U );

                                            if( pvStringCopy != NULL )
                                            {
                                                *xParamAddr.ppvPtr = pvStringCopy;
                                                char * pcStringCopy = *xParamAddr.ppcPtr;
                                                /* Copy parameter string into newly allocated memory. */
                                                memcpy( pcStringCopy, &pcJSON[ pxValTok->start ], ulTokenLen );
                                                /* Zero terminate the new string. */
                                                pcStringCopy[ ulTokenLen ] = '\0';
                                                OTA_LOG_L1( "[%s] Extracted parameter [ %s: %s ]\r\n",
                                                            OTA_METHOD_NAME,
                                                            pxModelParam[ usModelParamIndex ].pcSrcKey,
                                                            pcStringCopy );
                                            }
                                            else
                                            {   /* Stop processing on error. */
                                                eErr = eDocParseErr_OutOfMemory;
                                                /* break; */
                                            }
                                        }
                                        else if( eModelParamType_StringInDoc == pxModelParam[ usModelParamIndex ].xModelParamType )
                                        {
                                            /* Copy pointer to source string instead of duplicating the string. */
                                            const char * pcStringInDoc = &pcJSON[ pxValTok->start ];

                                            if( pcStringInDoc != NULL ) /*lint !e774 This can result in NULL if offset rolls the address around. */
                                            {
                                                *xParamAddr.ppcConstPtr = pcStringInDoc;
                                                ulTokenLen = ( uint32_t ) ( pxValTok->end ) - ( uint32_t ) ( pxValTok->start );
                                                OTA_LOG_L1( "[%s] Extracted parameter [ %s: %.*s ]\r\n",
                                                            OTA_METHOD_NAME,
                                                            pxModelParam[ usModelParamIndex ].pcSrcKey,
                                                            ulTokenLen, pcStringInDoc );
                                            }
                                            else
                                            {
                                                /* This should never happen unless there's a bug or memory is corrupted. */
                                                OTA_LOG_L1( "[%s] Error! JSON token produced a null pointer for parameter [ %s ]\r\n",
                                                            OTA_METHOD_NAME,
                                                            pxModelParam[ usModelParamIndex ].pcSrcKey );
                                                eErr = eDocParseErr_InvalidToken;
                                            }
                                        }
                                        else if( eModelParamType_UInt32 == pxModelParam[ usModelParamIndex ].xModelParamType )
                                        {
                                            char * pcEnd;
                                            const char * pcStart = &pcJSON[ pxValTok->start ];
                                            *xParamAddr.pulPtr = strtoul( pcStart, &pcEnd, 0 );

                                            if( pcEnd == &pcJSON[ pxValTok->end ] )
                                            {
                                                OTA_LOG_L1( "[%s] Extracted parameter [ %s: %u ]\r\n",
                                                            OTA_METHOD_NAME,
                                                            pxModelParam[ usModelParamIndex ].pcSrcKey,
                                                            *xParamAddr.pulPtr );
                                            }
                                            else
                                            {
                                                eErr = eDocParseErr_InvalidNumChar;
                                            }
                                        }
                                        else if( eModelParamType_SigBase64 == pxModelParam[ usModelParamIndex ].xModelParamType )
                                        {
                                            /* Allocate space for and decode the base64 signature. */
                                            void * pvSignature = pvPortMalloc( sizeof( Sig256_t ) );

                                            if( pvSignature != NULL )
                                            {
                                                size_t xActualLen;
                                                *xParamAddr.ppvPtr = pvSignature;
                                                Sig256_t * pxSig256 = *xParamAddr.ppxSig256Ptr;
                                                ulTokenLen = ( uint32_t ) ( pxValTok->end ) - ( uint32_t ) ( pxValTok->start );

                                                if( mbedtls_base64_decode( pxSig256->ucData, sizeof( pxSig256->ucData ), &xActualLen,
                                                                           ( const uint8_t * ) &pcJSON[ pxValTok->start ], ulTokenLen ) != 0 )
                                                {   /* Stop processing on error. */
                                                    OTA_LOG_L1( "[%s] mbedtls_base64_decode failed.\r\n", OTA_METHOD_NAME );
                                                    eErr = eDocParseErr_Base64Decode;
                                                    /* break; */
                                                }
                                                else
                                                {
                                                    pxSig256->usSize = ( uint16_t ) xActualLen;
                                                    OTA_LOG_L1( "[%s] Extracted parameter [ %s: %.32s... ]\r\n",
                                                                OTA_METHOD_NAME,
                                                                pxModelParam[ usModelParamIndex ].pcSrcKey,
                                                                &pcJSON[ pxValTok->start ] );
                                                }
                                            }
                                            else
                                            {
                                                /* We failed to allocate needed memory. Everything will be freed below upon failure. */
                                                eErr = eDocParseErr_OutOfMemory;
                                            }
                                        }
                                        else if( eModelParamType_Ident == pxModelParam[ usModelParamIndex ].xModelParamType )
                                        {
                                            OTA_LOG_L1( "[%s] Identified parameter [ %s ]\r\n",
                                                        OTA_METHOD_NAME,
                                                        pxModelParam[ usModelParamIndex ].pcSrcKey );
                                            *xParamAddr.pxBoolPtr = pdTRUE;
                                        }
                                        else
                                        {
                                            /* Ignore invalid document model type. */
                                        }
                                    }
                                }
                                else
                                {
                                    /* Nothing special to do. The error will break us out of the loop. */
                                }
                            }
                            else
                            {
                                /* Ignore tokens that are not strings and move on to the next. */
                            }
                        }

                        /* Free the token memory. */
                        vPortFree( pxTokens ); /*lint !e850 ulIndex is intentionally modified within the loop to skip over unknown tags. */

                        if( eErr == eDocParseErr_None )
                        {
                            uint32_t ulMissingParams = ( pxDocModel->ulParamsReceivedBitmap & pxDocModel->ulParamsRequiredBitmap )
                                                       ^ pxDocModel->ulParamsRequiredBitmap;

                            if( ulMissingParams != 0U )
                            {
                                /* The job document did not have all required document model parameters. */
                                for( ulScanIndex = 0UL; ulScanIndex < pxDocModel->usNumModelParams; ulScanIndex++ )
                                {
                                    if( ( ulMissingParams & ( 1UL << ulScanIndex ) ) != 0UL )
                                    {
                                        OTA_LOG_L1( "[%s] parameter not present: %s\r\n",
                                                    OTA_METHOD_NAME,
                                                    pxModelParam[ ulScanIndex ].pcSrcKey );
                                    }
                                }

                                eErr = eDocParseErr_MalformedDoc;
                            }
                        }
                        else
                        {
                            OTA_LOG_L1( "[%s] Error (%d) parsing JSON document.\r\n", OTA_METHOD_NAME, ( int32_t ) eErr );
                        }
                    }
                    else
                    {
                        OTA_LOG_L1( "[%s] jsmn_parse didn't match token count when parsing.\r\n", OTA_METHOD_NAME );
                        eErr = eDocParseErr_JasmineCountMismatch;
                    }
                }
                else
                {
                    OTA_LOG_L1( "[%s] No memory for JSON tokens.\r\n", OTA_METHOD_NAME );
                    eErr = eDocParseErr_OutOfMemory;
                }
            }
            else
            {
                OTA_LOG_L1( "[%s] Document has too many keys.\r\n", OTA_METHOD_NAME );
                eErr = eDocParseErr_TooManyTokens;
            }
        }
        else
        {
            OTA_LOG_L1( "[%s] Invalid JSON document. No tokens parsed. \r\n", OTA_METHOD_NAME );
            eErr = eDocParseErr_NoTokens;
        }
    }

    configASSERT( eErr != eDocParseErr_Unknown );
    return eErr;
}


/* Prepare the document model for use by sanity checking the initialization parameters
 * and detecting all required parameters. */

static DocParseErr_t prvInitDocModel( JSON_DocModel_t * pxDocModel,
                                      const JSON_DocParam_t * pxBodyDef,
                                      uint32_t ulContextBaseAddr,
                                      uint32_t ulContextSize,
                                      uint16_t usNumJobParams )
{
    DEFINE_OTA_METHOD_NAME( "prvInitDocModel" );

    DocParseErr_t eErr = eDocParseErr_Unknown;
    uint32_t ulScanIndex;

    /* Sanity check the model pointers and parameter count. Exclude the context base address and size since
     * it is technically possible to create a model that writes entirely into absolute memory locations.
     */
    if( pxDocModel == NULL )
    {
        OTA_LOG_L1( "[%s] The pointer to the document model is NULL.\r\n", OTA_METHOD_NAME );
        eErr = eDocParseErr_NullModelPointer;
    }
    else if( pxBodyDef == NULL )
    {
        OTA_LOG_L1( "[%s] Document model 0x%08x body pointer is NULL.\r\n", OTA_METHOD_NAME, pxDocModel );
        eErr = eDocParseErr_NullBodyPointer;
    }
    else if( usNumJobParams > OTA_DOC_MODEL_MAX_PARAMS )
    {
        OTA_LOG_L1( "[%s] Model has too many parameters (%u).\r\n", OTA_METHOD_NAME, pxDocModel->usNumModelParams );
        eErr = eDocParseErr_TooManyParams;
    }
    else
    {
        pxDocModel->ulContextBase = ulContextBaseAddr;
        pxDocModel->ulContextSize = ulContextSize;
        pxDocModel->pxBodyDef = pxBodyDef;
        pxDocModel->usNumModelParams = usNumJobParams;
        pxDocModel->ulParamsReceivedBitmap = 0;
        pxDocModel->ulParamsRequiredBitmap = 0;

        /* Scan the model and detect all required parameters (i.e. not optional). */
        for( ulScanIndex = 0; ulScanIndex < pxDocModel->usNumModelParams; ulScanIndex++ )
        {
            if( pxDocModel->pxBodyDef[ ulScanIndex ].bRequired == ( bool_t ) pdTRUE )
            {
                /* Add parameter to the required bitmap. */
                pxDocModel->ulParamsRequiredBitmap |= ( 1UL << ulScanIndex );
            }
        }

        eErr = eDocParseErr_None;
    }

    return eErr;
}


/* Parse the OTA job document and validate. Return the populated
 * OTA context if valid otherwise return NULL.
 */

static OTA_FileContext_t * prvParseJobDoc( const char * pcJSON,
                                           uint32_t ulMsgLen )
{
    DEFINE_OTA_METHOD_NAME( "prvParseJobDoc" );

    /* This is the OTA job document model describing the parameters, their types, destination and how to extract. */
    /*lint -e{708} We intentionally do some things lint warns about but produce the proper model. */
    /* Namely union initialization and pointers converted to values. */
    static const JSON_DocParam_t xOTA_JobDocModelParamStructure[ OTA_NUM_JOB_PARAMS ] =
    {
        { cOTA_JSON_ClientTokenKey,   OTA_JOB_PARAM_OPTIONAL, { ( uint32_t ) &xOTA_Agent.pucClientTokenFromJob }, eModelParamType_StringInDoc, JSMN_STRING    }, /*lint !e9078 !e923 Get address of token as value. */
        { cOTA_JSON_ExecutionKey,     OTA_JOB_PARAM_REQUIRED, { OTA_DONT_STORE_PARAM                           }, eModelParamType_Object,      JSMN_OBJECT    },
        { cOTA_JSON_JobIDKey,         OTA_JOB_PARAM_REQUIRED, { OFFSET_OF( OTA_FileContext_t, pucJobName )     }, eModelParamType_StringCopy,  JSMN_STRING    },
        { cOTA_JSON_StatusDetailsKey, OTA_JOB_PARAM_OPTIONAL, { OTA_DONT_STORE_PARAM                           }, eModelParamType_Object,      JSMN_OBJECT    },
        { cOTA_JSON_SelfTestKey,      OTA_JOB_PARAM_OPTIONAL, { OFFSET_OF( OTA_FileContext_t, xIsInSelfTest )  }, eModelParamType_Ident,       JSMN_STRING    },
        { cOTA_JSON_UpdatedByKey,     OTA_JOB_PARAM_OPTIONAL, { OFFSET_OF( OTA_FileContext_t, ulUpdaterVersion )}, eModelParamType_UInt32,      JSMN_STRING    },
        { cOTA_JSON_JobDocKey,        OTA_JOB_PARAM_REQUIRED, { OTA_DONT_STORE_PARAM                           }, eModelParamType_Object,      JSMN_OBJECT    },
        { cOTA_JSON_OTAUnitKey,       OTA_JOB_PARAM_REQUIRED, { OTA_DONT_STORE_PARAM                           }, eModelParamType_Object,      JSMN_OBJECT    },
        { cOTA_JSON_StreamNameKey,    OTA_JOB_PARAM_REQUIRED, { OFFSET_OF( OTA_FileContext_t, pucStreamName )  }, eModelParamType_StringCopy,  JSMN_STRING    },
        { cOTA_JSON_FileGroupKey,     OTA_JOB_PARAM_REQUIRED, { OTA_DONT_STORE_PARAM                           }, eModelParamType_Array,       JSMN_ARRAY     },
        { cOTA_JSON_FilePathKey,      OTA_JOB_PARAM_REQUIRED, { OFFSET_OF( OTA_FileContext_t, pucFilePath )    }, eModelParamType_StringCopy,  JSMN_STRING    },
        { cOTA_JSON_FileSizeKey,      OTA_JOB_PARAM_REQUIRED, { OFFSET_OF( OTA_FileContext_t, ulFileSize )     }, eModelParamType_UInt32,      JSMN_PRIMITIVE },
        { cOTA_JSON_FileIDKey,        OTA_JOB_PARAM_REQUIRED, { OFFSET_OF( OTA_FileContext_t, ulServerFileID ) }, eModelParamType_UInt32,      JSMN_PRIMITIVE },
        { cOTA_JSON_FileCertNameKey,  OTA_JOB_PARAM_REQUIRED, { OFFSET_OF( OTA_FileContext_t, pucCertFilepath )}, eModelParamType_StringCopy,  JSMN_STRING    },
        { cOTA_JSON_FileSignatureKey, OTA_JOB_PARAM_REQUIRED, { OFFSET_OF( OTA_FileContext_t, pxSignature )    }, eModelParamType_SigBase64,   JSMN_STRING    },
        { cOTA_JSON_FileAttributeKey, OTA_JOB_PARAM_OPTIONAL, { OFFSET_OF( OTA_FileContext_t, ulFileAttributes )}, eModelParamType_UInt32,      JSMN_PRIMITIVE },
    };

    OTA_JobParseErr_t eErr = eOTA_JobParseErr_Unknown;
    OTA_FileContext_t * pxC;
    OTA_FileContext_t * pxFinalFile;

    pxFinalFile = NULL;
    pxC = prvGetFreeContext();

    if( pxC == NULL )
    {
        OTA_LOG_L1( "[%s] Error! No job context available.\r\n", OTA_METHOD_NAME );
        eErr = eOTA_JobParseErr_NoContextAvailable;
    }
    else
    {
        JSON_DocModel_t xOTA_JobDocModel;

        if( prvInitDocModel( &xOTA_JobDocModel,
                             xOTA_JobDocModelParamStructure,
                             ( uint32_t ) pxC, /*lint !e9078 !e923 Intentionally casting context pointer to a value for prvInitDocModel. */
                             sizeof( OTA_FileContext_t ),
                             OTA_NUM_JOB_PARAMS ) != eDocParseErr_None )
        {
            eErr = eOTA_JobParseErr_BadModelInitParams;
        }
        else if( prvParseJSONbyModel( pcJSON, ulMsgLen, &xOTA_JobDocModel ) == eDocParseErr_None )
        {   /* Validate the job document parameters. */
            eErr = eOTA_JobParseErr_None;

            if( pxC->ulFileSize == 0U )
            {
                OTA_LOG_L1( "[%s] Zero file size is not allowed!\r\n", OTA_METHOD_NAME );
                eErr = eOTA_JobParseErr_ZeroFileSize;
            }
            /* If there's an active job, verify that it's the same as what's being reported now. */
            /* We already checked for missing parameters so we SHOULD have a job name in the context. */
            else if( xOTA_Agent.pucOTA_Singleton_ActiveJobName != NULL )
            {
                if( pxC->pucJobName != NULL )
                {
                    /* C->pucJobName is guaranteed to be zero terminated. */
                    if( strcmp( ( char * ) xOTA_Agent.pucOTA_Singleton_ActiveJobName, ( char * ) pxC->pucJobName ) != 0 )
                    {
                        OTA_LOG_L1( "[%s] Busy with existing job. Ignoring.\r\n", OTA_METHOD_NAME );
                        eErr = eOTA_JobParseErr_BusyWithExistingJob;
                    }
                    else
                    {   /* The same job is being reported so free the duplicate job name from the context. */
                        OTA_LOG_L1( "[%s] Superfluous report of current job.\r\n", OTA_METHOD_NAME );
                        vPortFree( pxC->pucJobName );
                        pxC->pucJobName = NULL;
                        eErr = eOTA_JobParseErr_BusyWithSameJob;
                    }
                }
                else
                {
                    OTA_LOG_L1( "[%s] Null job reported while busy. Ignoring.\r\n", OTA_METHOD_NAME );
                    eErr = eOTA_JobParseErr_NullJob;
                }
            }
            else
            {   /* Assume control of the job name from the context. */
                xOTA_Agent.pucOTA_Singleton_ActiveJobName = pxC->pucJobName;
                pxC->pucJobName = NULL;
            }

            if( eErr == eOTA_JobParseErr_None )
            {
                /* If the job is in self test mode, don't start an
                 * OTA update but instead do the following:
                 *
                 * If the firmware that performed the update was older
                 * than the currently running firmware, set the image
                 * state to "Testing." This is the success path.
                 *
                 * If it's the same or newer, reject the job since
                 * either the firmware wasn't accepted during self
                 * test or an incorrect image was sent by the OTA
                 * operator.
                 */
                if( pxC->xIsInSelfTest == ( bool_t ) pdTRUE )
                {
                    OTA_LOG_L1( "[%s] In self test mode.\r\n", OTA_METHOD_NAME );

                    if( pxC->ulUpdaterVersion < xAppFirmwareVersion.u.ulVersion32 )
                    {
                        /* The running firmware version is newer than the firmware that performed
                         * the update so this means we're ready to start the self test phase.
                         *
                         * Set image state accordingly and update job status with self test identifier.
                         */
                        ( void ) prvSetImageStateWithReason( eOTA_ImageState_Testing, ( uint32_t ) NULL );
                    }
                    else
                    {
                        if( pxC->ulUpdaterVersion > xAppFirmwareVersion.u.ulVersion32 )
                        {
                            /* The running firmware is older than the firmware that performed the update so reject the job. */
                            OTA_LOG_L1( "[%s] Rejecting image because version is older than previous.\r\n", OTA_METHOD_NAME );
                            ( void ) prvSetImageStateWithReason( eOTA_ImageState_Rejected, kOTA_Err_DowngradeNotAllowed );
                        }
                        else /* Version reported is the same as the running version. */
                        {
                            if( ( xOTA_Agent.pucClientTokenFromJob == NULL ) ||
                                ( strtoul( ( const char * ) xOTA_Agent.pucClientTokenFromJob, NULL, 0 ) == 0U ) ) /*lint !e9007 We don't provide a modifiable variable to strtoul. */
                            {
                                /* The version is the same so either we're not actually the new firmware or
                                 * someone messed up and sent firmware with the same version. In either case,
                                 * this is a failure of the OTA update so reject the job. */
                                OTA_LOG_L1( "[%s] Failing job. We rebooted and the version is still the same.\r\n", OTA_METHOD_NAME );
                                ( void ) prvSetImageStateWithReason( eOTA_ImageState_Rejected, kOTA_Err_SameFirmwareVersion );
                            }
                            else
                            {
                                OTA_LOG_L1( "[%s] Ignoring job. Device must be rebooted first.\r\n", OTA_METHOD_NAME );
                            }
                        }

                        /* All reject cases must reset the device. */
                        ( void ) prvResetDevice(); /* Ignore return code since there's nothing we can do if we can't force reset. */
                    }
                }
                else
                {
                    /* Everything looks OK. Set final context structure to start OTA. */
                    OTA_LOG_L1( "[%s] Job was accepted. Attempting to start transfer.\r\n", OTA_METHOD_NAME );
                    pxFinalFile = pxC;
                }
            }
            else
            {
                OTA_LOG_L1( "[%s] Error %d parsing job document.\r\n", OTA_METHOD_NAME, eErr );
            }
        }
        else
        {   /* No need to print an error message. The JSON parser should have already. */
            eErr = eOTA_JobParseErr_NonConformingJobDoc;
        }
    }

    configASSERT( eErr != eOTA_JobParseErr_Unknown );

    if( eErr != eOTA_JobParseErr_None )
    {
        if( pxC == NULL )
        {
            OTA_LOG_L1( "[%s] Error! No job context available.\r\n", OTA_METHOD_NAME );
        }
        else
        {
            /* If job parsing failed AND there's a job ID, update the job state to FAILED with
             * a reason code.  Without a job ID, we can't update the status in the job service. */
            if( pxC->pucJobName != NULL )
            {
                OTA_LOG_L1( "[%s] Rejecting job due to OTA_JobParseErr_t %d\r\n", OTA_METHOD_NAME, eErr );
                /* Assume control of the job name from the context. */
                xOTA_Agent.pucOTA_Singleton_ActiveJobName = pxC->pucJobName;
                pxC->pucJobName = NULL;
                prvUpdateJobStatus( NULL, eJobStatus_FailedWithVal, ( int32_t ) kOTA_Err_JobParserError, ( int32_t ) eErr );
                /* We don't need the job name memory anymore since we're done with this job. */
                vPortFree( xOTA_Agent.pucOTA_Singleton_ActiveJobName );
                xOTA_Agent.pucOTA_Singleton_ActiveJobName = NULL;
            }
            else
            {
                OTA_LOG_L1( "[%s] Ignoring job without ID.\r\n", OTA_METHOD_NAME );
            }
        }
    }

    /* If we failed, free the reserved file context (C) to make it available again. */
    if( pxFinalFile == NULL )
    {
        ( void ) prvOTA_Close( pxC ); /* Ignore impossible false result by design */
    }

    /* Return pointer to populated file context or NULL if it failed. */
    return pxFinalFile;
}


/* prvProcessOTAJobMsg
 *
 * We received an OTA update job message from the job notification service.
 * Process the message and prepare for receiving the file as needed.
 */

static OTA_FileContext_t * prvProcessOTAJobMsg( const char * pcRawMsg,
                                                uint32_t ulMsgLen )
{
    uint32_t ulIndex;
    uint32_t ulNumBlocks;             /* How many data pages are in the expected update image. */
    uint32_t ulBitmapLen;             /* Length of the file block bitmap in bytes. */
    OTA_FileContext_t * pxUpdateFile; /* Pointer to an OTA update context. */
    OTA_Err_t xErr = kOTA_Err_Uninitialized;

    /* Populate an OTA update context from the OTA job document. */

    pxUpdateFile = prvParseJobDoc( pcRawMsg, ulMsgLen );

    if( ( pxUpdateFile != NULL ) && ( prvInSelftest() == false ) )
    {
        if( pxUpdateFile->pucRxBlockBitmap != NULL )
        {
            vPortFree( pxUpdateFile->pucRxBlockBitmap ); /* Free any previously allocated bitmap. */
            pxUpdateFile->pucRxBlockBitmap = NULL;
        }

        /* Calculate how many bytes we need in our bitmap for tracking received blocks.
         * The below calculation requires power of 2 page sizes. */

        ulNumBlocks = ( pxUpdateFile->ulFileSize + ( OTA_FILE_BLOCK_SIZE - 1U ) ) >> otaconfigLOG2_FILE_BLOCK_SIZE;
        ulBitmapLen = ( ulNumBlocks + ( BITS_PER_BYTE - 1U ) ) >> LOG2_BITS_PER_BYTE;
        pxUpdateFile->pucRxBlockBitmap = ( uint8_t * ) pvPortMalloc( ulBitmapLen ); /*lint !e9079 FreeRTOS malloc port returns void*. */

        if( pxUpdateFile->pucRxBlockBitmap != NULL )
        {
            if( ( BaseType_t ) ( prvSubscribeToDataStream( pxUpdateFile ) ) == pdTRUE )
            {
                /* Set all bits in the bitmap to the erased state (we use 1 for erased just like flash memory). */
                memset( pxUpdateFile->pucRxBlockBitmap, ( int ) OTA_ERASED_BLOCKS_VAL, ulBitmapLen );

                /* Mark as used any pages in the bitmap that are out of range, based on the file size.
                 * This keeps us from requesting those pages during retry processing or if using a windowed
                 * block request. It also avoids erroneously accepting an out of range data block should it
                 * get past any safety checks.
                 * Files aren't always a multiple of 8 pages (8 bits/pages per byte) so some bits of the
                 * last byte may be out of range and those are the bits we want to clear. */

                uint8_t ucBit = 1U << ( BITS_PER_BYTE - 1U );
                uint32_t ulNumOutOfRange = ( ulBitmapLen * BITS_PER_BYTE ) - ulNumBlocks;

                for( ulIndex = 0U; ulIndex < ulNumOutOfRange; ulIndex++ )
                {
                    pxUpdateFile->pucRxBlockBitmap[ ulBitmapLen - 1U ] &= ~ucBit;
                    ucBit >>= 1U;
                }

                pxUpdateFile->ulBlocksRemaining = ulNumBlocks; /* Initialize our blocks remaining counter. */
                prvStartRequestTimer( pxUpdateFile );

                /* Create/Open the OTA file on the file system. */
                xErr = prvPAL_CreateFileForRx( pxUpdateFile );

                if( xErr != kOTA_Err_None )
                {
                    ( void ) prvSetImageStateWithReason( eOTA_ImageState_Aborted, xErr );
                    ( void ) prvOTA_Close( pxUpdateFile ); /* Ignore false result since we're setting the pointer to null on the next line. */
                    pxUpdateFile = NULL;
                }
            }
            else
            {
                /* Can't receive the image without a subscription. */
                ( void ) prvOTA_Close( pxUpdateFile ); /* Ignore false result since we're setting the pointer to null on the next line. */
                pxUpdateFile = NULL;
            }
        }
        else
        {
            /* Can't receive the image without enough memory. */
            ( void ) prvOTA_Close( pxUpdateFile ); /* Ignore false result since we're setting the pointer to null on the next line. */
            pxUpdateFile = NULL;
        }
    }

    return pxUpdateFile; /* Return the OTA file context. */
}



/* prvIngestDataBlock
 *
 * A block of file data was received by the application via some configured communication protocol.
 * If it looks like it is in range, write it to persistent storage. If it's the last block we're
 * expecting, close the file and perform the final signature check on it. If the close and signature
 * check are OK, let the caller know so it can be used by the system. Firmware updates generally
 * reboot the system and perform a self test phase. If the close or signature check fails, abort
 * the file transfer and return the result and any available details to the caller.
 */
static IngestResult_t prvIngestDataBlock( OTA_FileContext_t * C,
                                          const char * pcRawMsg,
                                          uint32_t ulMsgSize,
                                          OTA_Err_t * pxCloseResult )
{
    DEFINE_OTA_METHOD_NAME( "prvIngestDataBlock" );

    IngestResult_t eIngestResult = eIngest_Result_Uninitialized;
    int32_t lFileId = 0;
    uint32_t ulBlockSize = 0;
    uint32_t ulBlockIndex = 0;
    uint8_t * pucPayload = NULL;
    size_t xPayloadSize = 0;

    if( C != NULL )
    {
        if( pxCloseResult != NULL )
        {
            *pxCloseResult = kOTA_Err_GenericIngestError; /* Default to a generic ingest function error until we prove success. */

            /* If we have a block bitmap available then process the message. */
            if( C->pucRxBlockBitmap && ( C->ulBlocksRemaining > 0U ) )
            {
                /* Reset or start the firmware request timer. */
                prvStartRequestTimer( C );

                /* Decode the CBOR content. */
                if( pdFALSE == OTA_CBOR_Decode_GetStreamResponseMessage(
                        ( const uint8_t * ) pcRawMsg,
                        ulMsgSize,
                        &lFileId,
                        ( int32_t * ) &ulBlockIndex, /*lint !e9087 CBOR requires pointer to int and our block index's never exceed 31 bits. */
                        ( int32_t * ) &ulBlockSize,  /*lint !e9087 CBOR requires pointer to int and our block sizes never exceed 31 bits. */
                        &pucPayload,                 /* This payload gets malloc'd by OTA_CBOR_Decode_GetStreamResponseMessage(). We must free it. */
                        ( size_t * ) &xPayloadSize ) )
                {
                    eIngestResult = eIngest_Result_BadData;
                }
                else
                {
                    /* Validate the block index and size. */
                    /* If it is NOT the last block, it MUST be equal to a full block size. */
                    /* If it IS the last block, it MUST be equal to the expected remainder. */
                    /* If the block ID is out of range, that's an error so abort. */
                    uint32_t ulLastBlock = ( ( C->ulFileSize + ( OTA_FILE_BLOCK_SIZE - 1U ) ) >> otaconfigLOG2_FILE_BLOCK_SIZE ) - 1U;

                    if( ( ( ( uint32_t ) ulBlockIndex < ulLastBlock ) && ( ulBlockSize == OTA_FILE_BLOCK_SIZE ) ) ||
                        ( ( ( uint32_t ) ulBlockIndex == ulLastBlock ) && ( ( uint32_t ) ulBlockSize == ( C->ulFileSize - ( ulLastBlock * OTA_FILE_BLOCK_SIZE ) ) ) ) )
                    {
                        OTA_LOG_L1( "[%s] Received file block %u, size %u\r\n", OTA_METHOD_NAME, ulBlockIndex, ulBlockSize );

                        /* Create bit mask for use in our bitmap. */
                        uint8_t ucBitMask = 1U << ( ulBlockIndex % BITS_PER_BYTE ); /*lint !e9031 The composite expression will never be greater than BITS_PER_BYTE(8). */
                        /* Calculate byte offset into bitmap. */
                        uint32_t ulByte = ulBlockIndex >> LOG2_BITS_PER_BYTE;

                        if( ( C->pucRxBlockBitmap[ ulByte ] & ucBitMask ) == 0U ) /* If we've already received this block... */
                        {
                            OTA_LOG_L1( "[%s] block %u is a DUPLICATE. %u blocks remaining.\r\n", OTA_METHOD_NAME,
                                        ulBlockIndex,
                                        C->ulBlocksRemaining );
                            eIngestResult = eIngest_Result_Duplicate_Continue;
                            *pxCloseResult = kOTA_Err_None; /* This is a success path. */
                        }
                        else /* Otherwise, process it normally... */
                        {
                            if( C->pucFile != NULL )
                            {
                                int32_t lBytesWritten = prvPAL_WriteBlock( C, ( ulBlockIndex * OTA_FILE_BLOCK_SIZE ), pucPayload, ( uint32_t ) ulBlockSize );

                                if( lBytesWritten < 0 )
                                {
                                    OTA_LOG_L1( "[%s] Error (%d) writing file block\r\n", OTA_METHOD_NAME, lBytesWritten );
                                    eIngestResult = eIngest_Result_WriteBlockFailed;
                                }
                                else
                                {
                                    C->pucRxBlockBitmap[ ulByte ] &= ~ucBitMask; /* Mark this block as received in our bitmap. */
                                    C->ulBlocksRemaining--;
                                    eIngestResult = eIngest_Result_Accepted_Continue;
                                    *pxCloseResult = kOTA_Err_None; /* This is a success path. */
                                }
                            }
                            else
                            {
                                OTA_LOG_L1( "[%s] Error: Unable to write block, file handle is NULL.\r\n", OTA_METHOD_NAME );
                                eIngestResult = eIngest_Result_BadFileHandle;
                            }

                            if( C->ulBlocksRemaining == 0U )
                            {
                                OTA_LOG_L1( "[%s] Received final expected block of file.\r\n", OTA_METHOD_NAME );
                                prvStopRequestTimer( C );         /* Don't request any more since we're done. */
                                vPortFree( C->pucRxBlockBitmap ); /* Free the bitmap now that we're done with the download. */
                                C->pucRxBlockBitmap = NULL;

                                if( C->pucFile != NULL )
                                {
                                    *pxCloseResult = prvPAL_CloseFile( C );

                                    if( *pxCloseResult == kOTA_Err_None )
                                    {
                                        OTA_LOG_L1( "[%s] File receive complete and signature is valid.\r\n", OTA_METHOD_NAME );
                                        eIngestResult = eIngest_Result_FileComplete;
                                    }
                                    else
                                    {
                                        uint32_t ulCloseResult = ( uint32_t ) *pxCloseResult;
                                        OTA_LOG_L1( "[%s] Error (%u:0x%06x) closing OTA file.\r\n",
                                                    OTA_METHOD_NAME,
                                                    ulCloseResult >> kOTA_MainErrShiftDownBits,
                                                    ulCloseResult & ( uint32_t ) kOTA_PAL_ErrMask );

                                        if( ( ulCloseResult & kOTA_Main_ErrMask ) == kOTA_Err_SignatureCheckFailed )
                                        {
                                            eIngestResult = eIngest_Result_SigCheckFail;
                                        }
                                        else
                                        {
                                            eIngestResult = eIngest_Result_FileCloseFail;
                                        }
                                    }

                                    C->pucFile = NULL; /* File is now closed so clear the file handle in the context. */
                                }
                                else
                                {
                                    OTA_LOG_L1( "[%s] Error: File handle is NULL after last block received.\r\n", OTA_METHOD_NAME );
                                    eIngestResult = eIngest_Result_BadFileHandle;
                                }
                            }
                            else
                            {
                                OTA_LOG_L1( "[%s] Remaining: %u\r\n", OTA_METHOD_NAME, C->ulBlocksRemaining );
                            }
                        }
                    }
                    else
                    {
                        OTA_LOG_L1( "[%s] Error! Block %u out of expected range! Size %u\r\n", OTA_METHOD_NAME, ulBlockIndex, ulBlockSize );
                        eIngestResult = eIngest_Result_BlockOutOfRange;
                    }
                }
            }
            else
            {
                eIngestResult = eIngest_Result_UnexpectedBlock;
            }
        }
        else
        {
            eIngestResult = eIngest_Result_NullResultPointer;
        }
    }
    else
    {
        eIngestResult = eIngest_Result_NullContext;
    }

    if( NULL != pucPayload )
    {
        vPortFree( pucPayload );
    }

    return eIngestResult;
}


/* Subscribe to the OTA job notification topics. */

static bool_t prvSubscribeToJobNotificationTopics( void )
{
    DEFINE_OTA_METHOD_NAME( "prvSubscribeToJobNotificationTopics" );

    bool_t xResult = pdFALSE;
    char cJobTopic[ OTA_MAX_TOPIC_LEN ];
    MQTTAgentSubscribeParams_t xJobsSubscription;

    /* Clear subscription struct and set common parameters for job topics used by OTA. */
    memset( &xJobsSubscription, 0, sizeof( xJobsSubscription ) );
    xJobsSubscription.xQoS = eMQTTQoS1;
    xJobsSubscription.pucTopic = ( const uint8_t * ) cJobTopic;                  /* Point to local string storage. Built below. */
    xJobsSubscription.pvPublishCallbackContext = ( void * ) eOTA_PubMsgType_Job; /*lint !e923 The publish callback context is implementing data hiding with a void* type.*/
    xJobsSubscription.pxPublishCallback = prvOTAPublishCallback;
    xJobsSubscription.usTopicLength = ( uint16_t ) snprintf( cJobTopic,          /*lint -e586 Intentionally using snprintf. */
                                                             sizeof( cJobTopic ),
                                                             cOTA_JobsGetNextAccepted_TopicTemplate,
                                                             xOTA_Agent.ucThingName );

    if( ( xJobsSubscription.usTopicLength > 0U ) && ( xJobsSubscription.usTopicLength < sizeof( cJobTopic ) ) )
    {
        /* Subscribe to the first of two jobs topics. */
        if( MQTT_AGENT_Subscribe( xOTA_Agent.pvPubSubClient,
                                  &xJobsSubscription,
                                  ( TickType_t ) OTA_SUBSCRIBE_WAIT_TICKS ) != eMQTTAgentSuccess )
        {
            OTA_LOG_L1( "[%s] Failed: %s\n\r", OTA_METHOD_NAME, xJobsSubscription.pucTopic );
        }
        else
        {
            OTA_LOG_L1( "[%s] OK: %s\n\r", OTA_METHOD_NAME, xJobsSubscription.pucTopic );
            xJobsSubscription.usTopicLength = ( uint16_t ) snprintf( cJobTopic, /*lint -e586 Intentionally using snprintf. */
                                                                     sizeof( cJobTopic ),
                                                                     cOTA_JobsNotifyNext_TopicTemplate,
                                                                     xOTA_Agent.ucThingName );

            if( ( xJobsSubscription.usTopicLength > 0U ) && ( xJobsSubscription.usTopicLength < sizeof( cJobTopic ) ) )
            {
                /* Subscribe to the second of two jobs topics. */
                if( MQTT_AGENT_Subscribe( xOTA_Agent.pvPubSubClient,
                                          &xJobsSubscription,
                                          ( TickType_t ) OTA_SUBSCRIBE_WAIT_TICKS ) != eMQTTAgentSuccess )
                {
                    OTA_LOG_L1( "[%s] Failed: %s\n\r", OTA_METHOD_NAME, xJobsSubscription.pucTopic );
                }
                else
                {
                    OTA_LOG_L1( "[%s] OK: %s\n\r", OTA_METHOD_NAME, xJobsSubscription.pucTopic );
                    xResult = pdTRUE;
                }
            }
        }
    }

    return xResult;
}


/* Subscribe to the OTA data stream topic. */

static bool_t prvSubscribeToDataStream( OTA_FileContext_t * C )
{
    DEFINE_OTA_METHOD_NAME( "prvSubscribeToDataStream" );

    bool_t xResult = pdFALSE;
    char cOTA_RxStreamTopic[ OTA_MAX_TOPIC_LEN ];
    MQTTAgentSubscribeParams_t xOTAUpdateDataSubscription;

    memset( &xOTAUpdateDataSubscription, 0, sizeof( xOTAUpdateDataSubscription ) );
    xOTAUpdateDataSubscription.xQoS = eMQTTQoS0;
    xOTAUpdateDataSubscription.pucTopic = ( const uint8_t * ) cOTA_RxStreamTopic;
    xOTAUpdateDataSubscription.pvPublishCallbackContext = ( void * ) eOTA_PubMsgType_Stream; /*lint !e923 The publish callback context is implementing data hiding with a void* type.*/
    xOTAUpdateDataSubscription.pxPublishCallback = prvOTAPublishCallback;
    xOTAUpdateDataSubscription.usTopicLength = ( uint16_t ) snprintf( cOTA_RxStreamTopic,    /*lint -e586 Intentionally using snprintf. */
                                                                      sizeof( cOTA_RxStreamTopic ),
                                                                      cOTA_StreamData_TopicTemplate,
                                                                      xOTA_Agent.ucThingName,
                                                                      ( const char * ) C->pucStreamName );

    if( ( xOTAUpdateDataSubscription.usTopicLength > 0U ) && ( xOTAUpdateDataSubscription.usTopicLength < sizeof( cOTA_RxStreamTopic ) ) )
    {
        if( MQTT_AGENT_Subscribe( xOTA_Agent.pvPubSubClient, &xOTAUpdateDataSubscription, ( TickType_t ) OTA_SUBSCRIBE_WAIT_TICKS ) != eMQTTAgentSuccess )
        {
            OTA_LOG_L1( "[%s] Failed: %s\n\r", OTA_METHOD_NAME, xOTAUpdateDataSubscription.pucTopic );
        }
        else
        {
            OTA_LOG_L1( "[%s] OK: %s\n\r", OTA_METHOD_NAME, xOTAUpdateDataSubscription.pucTopic );
            xResult = pdTRUE;
        }
    }
    else
    {
        OTA_LOG_L1( "[%s] Failed to build stream topic.\n\r", OTA_METHOD_NAME );
    }

    return xResult;
}


/* UnSubscribe from the OTA data stream topic. */

static bool_t prvUnSubscribeFromDataStream( OTA_FileContext_t * C )
{
    DEFINE_OTA_METHOD_NAME( "prvUnSubscribeFromDataStream" );

    MQTTAgentUnsubscribeParams_t xUnSub;
    bool_t xResult = pdFALSE;
    char cOTA_RxStreamTopic[ OTA_MAX_TOPIC_LEN ];

    if( C != NULL )
    {
        /* Try to build the dynamic data stream topic and un-subscribe from it. */

        xUnSub.usTopicLength = ( uint16_t ) snprintf( cOTA_RxStreamTopic, /*lint -e586 Intentionally using snprintf. */
                                                      sizeof( cOTA_RxStreamTopic ),
                                                      cOTA_StreamData_TopicTemplate,
                                                      xOTA_Agent.ucThingName,
                                                      ( const char * ) C->pucStreamName );

        if( ( xUnSub.usTopicLength > 0U ) && ( xUnSub.usTopicLength < sizeof( cOTA_RxStreamTopic ) ) )
        {
            xUnSub.pucTopic = ( const uint8_t * ) cOTA_RxStreamTopic;

            if( MQTT_AGENT_Unsubscribe( xOTA_Agent.pvPubSubClient, &xUnSub, OTA_UNSUBSCRIBE_WAIT_TICKS ) != eMQTTAgentSuccess )
            {
                OTA_LOG_L1( "[%s] Failed: %s\n\r", OTA_METHOD_NAME, cOTA_RxStreamTopic );
            }
            else
            {
                OTA_LOG_L1( "[%s] OK: %s\n\r", OTA_METHOD_NAME, cOTA_RxStreamTopic );
                xResult = pdTRUE;
            }
        }
        else
        {
            OTA_LOG_L1( "[%s] Failed to build stream topic.\n\r", OTA_METHOD_NAME );
        }
    }

    return xResult;
}


/* UnSubscribe from the OTA job notification topics. */
static void prvUnSubscribeFromJobNotificationTopic( void )
{
    DEFINE_OTA_METHOD_NAME( "prvUnSubscribeFromJobNotificationTopic" );

    MQTTAgentUnsubscribeParams_t xUnSub;
    char cJobTopic[ OTA_MAX_TOPIC_LEN ];

    /* Try to unsubscribe from the first of two job topics. */
    xUnSub.pucTopic = ( const uint8_t * ) cJobTopic;         /* Point to local string storage. Built below. */
    xUnSub.usTopicLength = ( uint16_t ) snprintf( cJobTopic, /*lint -e586 Intentionally using snprintf. */
                                                  sizeof( cJobTopic ),
                                                  cOTA_JobsNotifyNext_TopicTemplate,
                                                  xOTA_Agent.ucThingName );

    if( ( xUnSub.usTopicLength > 0U ) && ( xUnSub.usTopicLength < sizeof( cJobTopic ) ) )
    {
        if( MQTT_AGENT_Unsubscribe( xOTA_Agent.pvPubSubClient, &xUnSub, OTA_UNSUBSCRIBE_WAIT_TICKS ) != eMQTTAgentSuccess )
        {
            OTA_LOG_L1( "[%s] FAIL: %s\n\r", OTA_METHOD_NAME, xUnSub.pucTopic );
        }
        else
        {
            OTA_LOG_L1( "[%s] OK: %s\n\r", OTA_METHOD_NAME, xUnSub.pucTopic );
        }
    }

    /* Try to unsubscribe from the second of two job topics. */
    xUnSub.usTopicLength = ( uint16_t ) snprintf( cJobTopic, /*lint -e586 Intentionally using snprintf. */
                                                  sizeof( cJobTopic ),
                                                  cOTA_JobsGetNextAccepted_TopicTemplate,
                                                  xOTA_Agent.ucThingName );

    if( ( xUnSub.usTopicLength > 0U ) && ( xUnSub.usTopicLength < sizeof( cJobTopic ) ) )
    {
        if( MQTT_AGENT_Unsubscribe( xOTA_Agent.pvPubSubClient, &xUnSub, OTA_UNSUBSCRIBE_WAIT_TICKS ) != eMQTTAgentSuccess )
        {
            OTA_LOG_L1( "[%s] FAIL: %s\n\r", OTA_METHOD_NAME, xUnSub.pucTopic );
        }
        else
        {
            OTA_LOG_L1( "[%s] OK: %s\n\r", OTA_METHOD_NAME, xUnSub.pucTopic );
        }
    }
}


/* Publish a message to the specified client/topic at the given QOS. */

static MQTTAgentReturnCode_t prvPublishMessage( void * const pvClient,
                                                const char * const pcTopic,
                                                uint16_t usTopicLen,
                                                char * pcMsg,
                                                uint32_t ulMsgSize,
                                                MQTTQoS_t eQOS )
{
    MQTTAgentReturnCode_t eResult;
    MQTTAgentPublishParams_t xPublishParams;

    xPublishParams.pucTopic = ( const uint8_t * ) pcTopic;
    xPublishParams.usTopicLength = usTopicLen;
    xPublishParams.xQoS = eQOS;
    xPublishParams.pvData = pcMsg;
    xPublishParams.ulDataLength = ulMsgSize;
    eResult = MQTT_AGENT_Publish( pvClient, &xPublishParams, ( TickType_t ) OTA_PUBLISH_WAIT_TICKS );

    if( eResult != eMQTTAgentSuccess )
    {
        xOTA_Agent.xStatistics.ulOTA_PublishFailures++; /* Track how many publish failures we've had. */
    }
    else
    {
        /* Nothing special to do on success. */
    }

    return eResult;
}


/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_agent_test_access_define.h"
#endif
