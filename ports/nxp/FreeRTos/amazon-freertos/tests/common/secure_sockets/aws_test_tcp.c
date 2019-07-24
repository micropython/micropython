/*
 * Amazon FreeRTOS Secure Sockets AFQP V1.1.4
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

/* Secure Sockets includes. */
#include "aws_secure_sockets.h"

/* Logging includes. */
#include "aws_logging_task.h"

/* Test framework includes. */
#include "unity_fixture.h"
#include "aws_test_runner.h"
#include "aws_test_utils.h"
#include "aws_test_tcp.h"

/* Update this file with port-specific values. */
#include "aws_test_tcp_config.h"

/* Update this file with AWS Credentials. */
#include "aws_clientcredential.h"

/* Verbose printing. */
#define tcptestPRINTF( x )
/* In case of test failures, FAILUREPRINTF may provide more detailed information. */
#define tcptestFAILUREPRINTF( x )    vLoggingPrintf x
/* Fail the test on tcptestASSERT. */
#define tcptestASSERT( x )           configASSERT( x )
/* Take measurements of the FreeRTOS heap before and after every test. */
#define tcptestMEMORYWATCH    0

/* Connection type. Add additional test servers here. */
typedef enum
{
    eSecure,    /* Secure test server.  Uses tcptestECHO_SERVER_TLS. */
    eNonsecure, /* Non secure test server.  Uses tcptestECHO_SERVER. */
    eAwsBroker  /* AWS IoT Broker.  Uses clientcredential. */
} Server_t;

/* The echo tasks create a socket, send out a number of echo requests, listen
 * for the echo reply, then close the socket again before starting over.  This
 * delay is used between each iteration to ensure the network does not get too
 * congested. */
#define tcptestLOOP_DELAY_MS    ( 150 )
#define tcptestLOOP_DELAY       ( ( TickType_t ) tcptestLOOP_DELAY_MS / portTICK_PERIOD_MS )

/* The number of times SOCKETS_Connect_TCP opens and closes a socket. */
/* Need to test at least 20 times. So bugs were not discovered with only 10 loops. */
#define tcptestCONNECT_AND_CLOSE_LOOP    ( 20 )

/* Filler values in the RX and TX buffers used to check for undesirable
 * buffer modification. */
#define tcptestRX_BUFFER_FILLER          0xFF
#define tcptestTX_BUFFER_FILLER          0xAA

/* Size of pcTxBuffer and pcRxBuffer. */
#define tcptestBUFFER_SIZE               ( testrunnerBUFFER_SIZE / 2 )

/* Global buffers are shared between tests to reduce RAM usage. */
char * pcTxBuffer = &cBuffer[ 0 ];
char * pcRxBuffer = &cBuffer[ tcptestBUFFER_SIZE ];

/* Default Rx and Tx time outs are used to ensure the sockets do not
 * wait too long for missing data. */
static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS( integrationtestportableRECEIVE_TIMEOUT );
static const TickType_t xSendTimeOut = pdMS_TO_TICKS( integrationtestportableSEND_TIMEOUT );

/* Primary socket, declared globally so that it can be closed in test tear down
 * in the event that a test exits/fails before socket is closed. */
volatile Socket_t xSocket = SOCKETS_INVALID_SOCKET;
volatile BaseType_t xSocketOpen = pdFALSE;

/*
 * Uses a socket to send more than MSS bytes in one go to the standard echo
 * port number 7.  The echoed data is received on the same socket but in a
 * different task (see prvEchoClientRxTask() below).
 */
static void prvEchoClientTxTask( void * pvParameters );

/* TCP Echo Client tasks multi-task test parameters. These can be configured in aws_test_tcp_config.h. */
#ifndef tcptestTCP_ECHO_TASKS_STACK_SIZE
    #define tcptestTCP_ECHO_TASKS_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
#endif
#ifndef tcptestTCP_ECHO_TASKS_PRIORITY
    #define tcptestTCP_ECHO_TASKS_PRIORITY      ( tskIDLE_PRIORITY )
#endif

/* The queue used by prvEchoClientTxTask() to send the next socket to use to
 * prvEchoClientRxTask(). */
static volatile QueueHandle_t xSocketPassingQueue = NULL;

/* The event group used by the prvEchoClientTxTask() and prvEchoClientRxTask()
 * to synchronize prior to commencing a cycle using a new socket. */
static volatile EventGroupHandle_t xSyncEventGroup = NULL;


/* Bit definitions used with the xSyncEventGroup event group to allow the
 * prvEchoClientTxTask() and prvEchoClientRxTask() tasks to synchronize before
 * commencing a new cycle with a different socket. */
#define tcptestTX_TASK_BIT    ( 0x01 << 1 )
#define tcptestRX_TASK_BIT    ( 0x01 << 2 )


/* Array setup for 2 times the biggest possible frame (eth0 and WIFI). */
/* WIFI is 2304 and eth0 is 1522. */
#define tcptestTWICE_MAX_FRAME_SIZE    ( 2 * 2304 )
/* Array is defined as const so it can be located in flash. */
static const char cTransmittedString[ tcptestTWICE_MAX_FRAME_SIZE ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 1 };

#ifndef tcptestECHO_TEST_SYNC_TIMEOUT
    #define tcptestECHO_TEST_SYNC_TIMEOUT    80000
#endif

#ifndef tcptestECHO_TEST_RXTX_TIMEOUT
    #define tcptestECHO_TEST_RXTX_TIMEOUT       5000
#endif
#define tcptestECHO_CLIENT_EVENT_MASK           ( ( 1 << tcptestNUM_ECHO_CLIENTS ) - 1 )

#define  tcptestECHO_TEST_SYNC_TIMEOUT_TICKS    ( pdMS_TO_TICKS( tcptestECHO_TEST_SYNC_TIMEOUT ) )
static const TickType_t xEchoTestRxTxTimeOut = pdMS_TO_TICKS( tcptestECHO_TEST_RXTX_TIMEOUT );

/* All the modes for the test  SOCKETS_Echo_Client_Separate_Tasks */
typedef enum
{
    LARGE_BUFFER_HIGH_PRIORITY,
    SMALL_BUFFER_HIGH_PRIORITY,
    LARGE_BUFFER_LOW_PRIORITY,
    SMALL_BUFFER_LOW_PRIORITY,
    tcptestMAX_ECHO_TEST_MODES
} tcptestEchoTestModes_t;

typedef struct
{
    Server_t xConn;
    uint16_t usTaskTag;
    BaseType_t xResult;
    TaskHandle_t xTaskHandle;
} tcptestEchoClientsTaskParams_t;

/* Number of time the test goes through all the modes. */
#define tcptestMAX_LOOPS_ECHO_TEST            ( 4 * tcptestMAX_ECHO_TEST_MODES )

#define tcptestECHO_TEST_LOW_PRIORITY         tskIDLE_PRIORITY
#define tcptestECHO_TEST_HIGH_PRIORITY        ( configMAX_PRIORITIES - 1 )

#ifndef ipconfigTCP_MSS
    #define ipconfigTCP_MSS                   ( 256 )
#endif
#define tcptestNUM_ECHO_CLIENTS               ( 2 )
#define tcptestMAX_LOOPS_ECHO_CLIENTS_LOOP    ( 10 )

static void prvThreadSafeDifferentSocketsDifferentTasks( void * pvParameters );
/****************** Unity Test Code *********************************/
size_t xHeapB;
size_t xHeapA;
/* Group declaration required by Unity framework. */
TEST_GROUP( Full_TCP );

/* Setup required by Unity framework. */
TEST_SETUP( Full_TCP )
{
    #if ( tcptestMEMORYWATCH == 1 )
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapB = xPortGetFreeHeapSize();
    #endif
}

/* Tear down required by Unity framework.
 * Closes the global socket if it was left open by the test. */
TEST_TEAR_DOWN( Full_TCP )
{
    int32_t lReturn;

    if( TEST_PROTECT() )
    {
        if( xSocketOpen == pdTRUE )
        {
            lReturn = SOCKETS_Close( xSocket );
            xSocketOpen = pdFALSE;
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, lReturn, "SOCKETS_Close failed in tear down. \r\n" );
        }
    }

    #if ( tcptestMEMORYWATCH == 1 )
        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapA = xPortGetFreeHeapSize();

        configPRINTF( ( "Heap before: %d, Heap After: %d \r\n", xHeapB, xHeapA ) );
    #endif
}


/*-----------------------------------------------------------*/
static void prvCreateTxData( char * cBuffer,
                             size_t xMessageLength,
                             uint32_t ulTxCount );
static BaseType_t prvCheckRxTxBuffers( uint8_t * pucTxBuffer,
                                       uint8_t * pucRxBuffer,
                                       size_t xMessageLength );

/*-----------------------------------------------------------*/

/* Creates a TCP Socket.
 * Causes a test failure if socket creation fails. */
static Socket_t prvTcpSocketHelper( volatile BaseType_t * pxSocketOpen )
{
    Socket_t xSocket;

    /* Make socket. */
    xSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                              SOCKETS_SOCK_STREAM,
                              SOCKETS_IPPROTO_TCP );

    if( xSocket != SOCKETS_INVALID_SOCKET )
    {
        /* Test variable for closing exits/fails before closing. */
        *pxSocketOpen = pdTRUE;
    }

    return xSocket;
}


/*-----------------------------------------------------------*/

/* Sets secure socket options and returns the address of the secure echo server. */
static BaseType_t prvSecureConnectHelper( Socket_t xSocket,
                                          SocketsSockaddr_t * pxHostAddress )
{
    BaseType_t xResult = pdFAIL;

    /* Echo requests are sent to the secure echo server and port. */
    pxHostAddress->ulAddress = SOCKETS_inet_addr_quick( tcptestECHO_SERVER_TLS_ADDR0,
                                                        tcptestECHO_SERVER_TLS_ADDR1,
                                                        tcptestECHO_SERVER_TLS_ADDR2,
                                                        tcptestECHO_SERVER_TLS_ADDR3 );
    pxHostAddress->usPort = SOCKETS_htons( tcptestECHO_PORT_TLS );
    pxHostAddress->ucLength = sizeof( SocketsSockaddr_t );
    pxHostAddress->ucSocketDomain = SOCKETS_AF_INET;

    /* Set the socket to use TLS. */
    xResult = SOCKETS_SetSockOpt( xSocket,
                                  0,
                                  SOCKETS_SO_REQUIRE_TLS,
                                  NULL,
                                  ( size_t ) 0 );

    if( xResult == SOCKETS_ERROR_NONE )
    {
        /* Set the socket to use the secure server's root CA cert. */
        xResult = SOCKETS_SetSockOpt( xSocket,
                                      0,
                                      SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                                      tcptestECHO_HOST_ROOT_CA,
                                      sizeof( tcptestECHO_HOST_ROOT_CA ) );

        if( xResult != SOCKETS_ERROR_NONE )
        {
            tcptestFAILUREPRINTF( ( "%s: Failed to setSockOpt SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE \r\n", __FUNCTION__ ) );
        }
    }
    else
    {
        tcptestFAILUREPRINTF( ( "%: Failed to setSockOpt SOCKETS_SO_REQUIRE_TLS \r\n", __FUNCTION__ ) );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static BaseType_t prvNonSecureConnectHelper( Socket_t xSocket,
                                             SocketsSockaddr_t * pxHostAddress )
{
    /* Echo requests are sent to the echo server.  The echo server is
    * listening to tcptestECHO_PORT on this computer's IP address. */
    pxHostAddress->ulAddress = SOCKETS_inet_addr_quick( tcptestECHO_SERVER_ADDR0,
                                                        tcptestECHO_SERVER_ADDR1,
                                                        tcptestECHO_SERVER_ADDR2,
                                                        tcptestECHO_SERVER_ADDR3 );
    pxHostAddress->usPort = SOCKETS_htons( tcptestECHO_PORT );
    pxHostAddress->ucLength = sizeof( SocketsSockaddr_t );
    pxHostAddress->ucSocketDomain = SOCKETS_AF_INET;

    return SOCKETS_ERROR_NONE;
}

/*-----------------------------------------------------------*/

static BaseType_t prvAwsIotBrokerConnectHelper( Socket_t xSocket,
                                                SocketsSockaddr_t * pxHostAddress )
{
    BaseType_t xResult = SOCKETS_SOCKET_ERROR;

    /* Resolve the broker endpoint to an IP address. */
    pxHostAddress->ulAddress = SOCKETS_GetHostByName( clientcredentialMQTT_BROKER_ENDPOINT );

    if( pxHostAddress->ulAddress != 0 )
    {
        pxHostAddress->usPort = SOCKETS_htons( clientcredentialMQTT_BROKER_PORT );
        pxHostAddress->ucLength = sizeof( SocketsSockaddr_t );
        pxHostAddress->ucSocketDomain = SOCKETS_AF_INET;

        /* Set the socket to use TLS. */
        xResult = SOCKETS_SetSockOpt( xSocket,
                                      0,
                                      SOCKETS_SO_REQUIRE_TLS,
                                      NULL,
                                      ( size_t ) 0 );

        if( xResult != SOCKETS_ERROR_NONE )
        {
            tcptestFAILUREPRINTF( ( "%s: Failed to setSockOpt SOCKETS_SO_REQUIRE_TLS \r\n", __FUNCTION__ ) );
        }
    }

    return xResult;
}
/*-----------------------------------------------------------*/

static BaseType_t prvConnectHelper( Socket_t xSocket,
                                    Server_t xConn )
{
    BaseType_t xResult = pdFAIL;
    SocketsSockaddr_t xEchoServerAddress;

    if( xConn == eSecure )
    {
        xResult = prvSecureConnectHelper( xSocket, &xEchoServerAddress );
    }
    else if( xConn == eNonsecure )
    {
        xResult = prvNonSecureConnectHelper( xSocket, &xEchoServerAddress );
    }
    else if( xConn == eAwsBroker )
    {
        xResult = prvAwsIotBrokerConnectHelper( xSocket, &xEchoServerAddress );
    }
    else
    {
        /* If a delay server is introduced, it will be added here. */
        tcptestFAILUREPRINTF( ( "%s: Failed to setSockOpt SOCKETS_SO_REQUIRE_TLS \r\n", __FUNCTION__ ) );
    }

    if( xResult == SOCKETS_ERROR_NONE )
    {
        uint32_t ulInitialRetryPeriodMs = tcptestLOOP_DELAY_MS;
        BaseType_t xMaxRetries = tcptestRETRY_CONNECTION_TIMES;
        RETRY_EXPONENTIAL( xResult = SOCKETS_Connect( xSocket,
                                                      &xEchoServerAddress,
                                                      sizeof( xEchoServerAddress ) ),
                           SOCKETS_ERROR_NONE,
                           ulInitialRetryPeriodMs,
                           xMaxRetries );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static BaseType_t prvSetSockOptHelper( Socket_t xSocket,
                                       TickType_t xRxTimeOut,
                                       TickType_t xTxTimeOut )
{
    BaseType_t xResult;

    /* Set a time out so a missing reply does not cause the task to block
     * indefinitely. */
    xResult = SOCKETS_SetSockOpt( xSocket,
                                  0,
                                  SOCKETS_SO_RCVTIMEO,
                                  &xRxTimeOut,
                                  sizeof( xRxTimeOut ) );

    if( xResult == SOCKETS_ERROR_NONE )
    {
        xResult = SOCKETS_SetSockOpt( xSocket,
                                      0,
                                      SOCKETS_SO_SNDTIMEO,
                                      &xTxTimeOut,
                                      sizeof( xTxTimeOut ) );

        if( xResult != SOCKETS_ERROR_NONE )
        {
            tcptestFAILUREPRINTF( ( "%s: Socket set sock opt send timeout failed \r\n", __FUNCTION__ ) );
        }
    }
    else
    {
        tcptestFAILUREPRINTF( ( "%s: Socket set sock opt receive timeout failed \r\n", __FUNCTION__ ) );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static BaseType_t prvConnectHelperWithRetry( volatile Socket_t * pxSocket,
                                             Server_t xConn,
                                             TickType_t xRxTimeOut,
                                             TickType_t xTxTimeOut,
                                             volatile BaseType_t * pxSocketIsAllocated )
{
    BaseType_t xIsConnected = pdFALSE;
    BaseType_t xResult = SOCKETS_ERROR_NONE;
    BaseType_t xRetry = 0;
    SocketsSockaddr_t xEchoServerAddress;
    TickType_t xRetryTimeoutTicks = tcptestLOOP_DELAY;

    do
    {
        /* Create the socket. */
        *pxSocket = prvTcpSocketHelper( pxSocketIsAllocated );

        if( SOCKETS_INVALID_SOCKET == *pxSocket )
        {
            xResult = SOCKETS_SOCKET_ERROR;
        }

        /* Set the appropriate socket options for the destination. */
        if( SOCKETS_ERROR_NONE == xResult )
        {
            if( xConn == eSecure )
            {
                xResult = prvSecureConnectHelper( *pxSocket, &xEchoServerAddress );
            }
            else if( xConn == eNonsecure )
            {
                xResult = prvNonSecureConnectHelper( *pxSocket, &xEchoServerAddress );
            }
            else if( xConn == eAwsBroker )
            {
                xResult = prvAwsIotBrokerConnectHelper( *pxSocket, &xEchoServerAddress );
            }
            else
            {
                /* If a delay server is introduced, it will be added here. */
                tcptestFAILUREPRINTF( ( "%s: Failed to setSockOpt SOCKETS_SO_REQUIRE_TLS \r\n", __FUNCTION__ ) );
            }
        }

        /* Set socket timeout options. */
        if( SOCKETS_ERROR_NONE == xResult )
        {
            xResult = prvSetSockOptHelper( *pxSocket, xRxTimeOut, xTxTimeOut );
        }

        /* Attempt, with possible retries, to connect to the destination. */
        if( SOCKETS_ERROR_NONE == xResult )
        {
            xResult = SOCKETS_Connect( *pxSocket,
                                       &xEchoServerAddress,
                                       sizeof( xEchoServerAddress ) );

            if( SOCKETS_ERROR_NONE == xResult )
            {
                xIsConnected = pdTRUE;
            }
            else
            {
                if( xRetry < tcptestRETRY_CONNECTION_TIMES )
                {
                    SOCKETS_Close( *pxSocket );
                    *pxSocket = SOCKETS_INVALID_SOCKET;
                    xResult = SOCKETS_ERROR_NONE;
                    xRetry++;
                    vTaskDelay( xRetryTimeoutTicks );
                    /* Exponetially backoff the retry times */
                    xRetryTimeoutTicks *= 2; /*Arbitrarily chose 2*/
                }
                else
                {
                    /* Too many failures. Give up. */
                    break;
                }
            }
        }
    }
    while( pdFALSE == xIsConnected );

    return xResult;
}

/*-----------------------------------------------------------*/

static BaseType_t prvSendHelper( Socket_t xSocket,
                                 uint8_t * pucTxBuffer,
                                 size_t xLength )
{
    BaseType_t xNumBytesSentTotal;
    BaseType_t xNumBytes;
    BaseType_t xResult;

    xResult = pdPASS;
    xNumBytesSentTotal = 0;

    while( ( size_t ) xNumBytesSentTotal < xLength )
    {
        xNumBytes = SOCKETS_Send( xSocket,                            /* The socket being sent to. */
                                  &pucTxBuffer[ xNumBytesSentTotal ], /* The data being sent. */
                                  xLength - xNumBytesSentTotal,       /* The length of the data being sent. */
                                  0 );                                /* No flags. */

        if( xNumBytes <= 0 )
        {
            xResult = pdFAIL;
            tcptestFAILUREPRINTF( ( "Error in SOCKETS_Send.  Return value: %d \r\n", xNumBytes ) );
            break;
        }

        xNumBytesSentTotal += xNumBytes;
    }

    if( ( size_t ) xNumBytesSentTotal != xLength )
    {
        xResult = pdFAIL;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static BaseType_t prvRecvHelper( Socket_t xSocket,
                                 uint8_t * pucRxBuffer,
                                 size_t xLength )
{
    BaseType_t xNumBytesRecvTotal;
    BaseType_t xNumBytes;
    BaseType_t xResult;

    xResult = pdPASS;
    xNumBytesRecvTotal = 0;

    while( ( size_t ) xNumBytesRecvTotal < xLength )
    {
        xNumBytes = SOCKETS_Recv( xSocket,
                                  &pucRxBuffer[ xNumBytesRecvTotal ],
                                  xLength - xNumBytesRecvTotal,
                                  0 );

        if( xNumBytes == 0 )
        {
            tcptestFAILUREPRINTF( ( "Timed out receiving from echo server \r\n" ) );
            xResult = pdFAIL;
            break;
        }
        else if( xNumBytes < 0 )
        {
            tcptestFAILUREPRINTF( ( "Error %d while receiving from echo server\r\n", xNumBytes ) );
            xResult = pdFAIL;
            break;
        }

        xNumBytesRecvTotal += xNumBytes;
    }

    if( ( size_t ) xNumBytesRecvTotal != xLength )
    {
        xResult = pdFAIL;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static BaseType_t prvShutdownHelper( Socket_t xSocket )
{
    BaseType_t xResult;

    /* The caller is done with the connected socket; initiate a graceful close:
     * send a FIN packet and wait for the server to stop sending. */
    xResult = SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RDWR );

    if( 0 == xResult )
    {
        /* Keep calling receive until an error code is returned. */
        do
        {
            xResult = SOCKETS_Recv( xSocket,            /* The socket being received from. */
                                    pcRxBuffer,         /* The buffer into which the received data will be written. */
                                    tcptestBUFFER_SIZE, /* The size of the buffer provided to receive the data. */
                                    0 );
        }
        while( xResult >= 0 );

        xResult = 0;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static BaseType_t prvCloseHelper( Socket_t xSocket,
                                  volatile BaseType_t * pxSocketOpen )
{
    BaseType_t xResult;

    xResult = SOCKETS_Close( xSocket );

    *pxSocketOpen = pdFALSE;

    return xResult;
}

/*-----------------------------------------------------------*/

static void prvCreateTxData( char * cBuffer,
                             size_t xMessageLength,
                             uint32_t ulTxCount )
{
    size_t xCharacter;
    char cChar = '0';

    /* Set the Tx buffer to a known filler value. */
    memset( cBuffer, tcptestTX_BUFFER_FILLER, tcptestBUFFER_SIZE );

    /* Fill the end of the buffer with ascending characters. */
    if( xMessageLength > 25 )
    {
        for( xCharacter = 0; xCharacter < xMessageLength; xCharacter++ )
        {
            cBuffer[ xCharacter ] = cChar;
            cChar++;

            if( cChar > '~' )
            {
                cChar = '0';
            }
        }
    }

    /* Write as much of the identifying string as possible to the buffer. */
    snprintf( cBuffer, xMessageLength, "%u TxRx with of length %u", ( unsigned ) ulTxCount, ( unsigned ) xMessageLength );
}

/*-----------------------------------------------------------*/

static BaseType_t prvCheckRxTxBuffers( uint8_t * pucTxBuffer,
                                       uint8_t * pucRxBuffer,
                                       size_t xMessageLength )
{
    BaseType_t xReturn;
    size_t xIndex;

    xReturn = pdPASS;

    /* Check that the message received is correct. */
    for( xIndex = 0; xIndex < xMessageLength; xIndex++ )
    {
        if( pucTxBuffer[ xIndex ] != pucRxBuffer[ xIndex ] )
        {
            xReturn = pdFAIL;
            tcptestFAILUREPRINTF( ( "Message bytes received different than those sent. Message Length %d, Byte Index %d \r\n", xMessageLength, xIndex ) );
            tcptestASSERT( xReturn == pdPASS );
        }
    }

    /* Check that neither the Rx nor Tx buffers were modified/overflowed. */
    for( xIndex = xMessageLength; xIndex < tcptestBUFFER_SIZE; xIndex++ )
    {
        if( pucRxBuffer[ xIndex ] != tcptestRX_BUFFER_FILLER )
        {
            xReturn = pdFAIL;
            tcptestFAILUREPRINTF( ( "Rx buffer padding modified byte. Message Length %d, Byte Index %d \r\n", xMessageLength, xIndex ) );
        }

        if( pucTxBuffer[ xIndex ] != tcptestTX_BUFFER_FILLER )
        {
            xReturn = pdFAIL;
            tcptestFAILUREPRINTF( ( "Tx buffer padding modified. Message Length %d, Byte Index %d \r\n", xMessageLength, xIndex ) );
        }
    }

    tcptestASSERT( xReturn == pdPASS );

    return xReturn;
}

/*-----------------------------------------------------------*/

static BaseType_t prvCheckBufferUnmodified( uint8_t * pucBuffer,
                                            uint8_t ucFiller,
                                            size_t xLength )
{
    BaseType_t xResult;
    size_t xIndex;

    xResult = pdPASS;

    for( xIndex = 0; xIndex < xLength; xIndex++ )
    {
        if( pucBuffer[ xIndex ] != ucFiller )
        {
            xResult = pdFAIL;
            tcptestFAILUREPRINTF( ( "Buffer modified at byte %d \r\n", xIndex ) );
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/

static BaseType_t prvCheckTimeout( TickType_t xStartTime,
                                   TickType_t xEndTime,
                                   TickType_t xTimeout )
{
    BaseType_t xResult = pdPASS;

    /* Did the function run at least as long as the timeout?
     * An "under tolerance" may be needed when the application timer
     * and the network timer are sourced from separate clocks. */
    if( ( int32_t ) ( xEndTime - xStartTime ) < ( int32_t ) ( xTimeout - integrationtestportableTIMEOUT_UNDER_TOLERANCE ) )
    {
        xResult = pdFAIL;
        tcptestFAILUREPRINTF( ( "Start Time: %d, End Time: %d, Timeout: %d \n", xStartTime, xEndTime, xTimeout ) );
    }

    /* Did the function exit after a reasonable amount of time?
     * An "over tolerance" is used to allow for overhead surrounding the timeout. */
    if( ( xEndTime - xStartTime ) > ( xTimeout + integrationtestportableTIMEOUT_OVER_TOLERANCE ) )
    {
        xResult = pdFAIL;
        tcptestFAILUREPRINTF( ( "Start Time: %d, End Time: %d, Timeout: %d \n", xStartTime, xEndTime, xTimeout ) );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

TEST_GROUP_RUNNER( Full_TCP )
{
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_CloseInvalidParams );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_CloseWithoutReceiving );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_ShutdownInvalidParams );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_ShutdownWithoutReceiving );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Recv_On_Unconnected_Socket );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Threadsafe_SameSocketDifferentTasks );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Connect_InvalidParams );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Connect_InvalidAddressLength );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Threadsafe_DifferentSocketsDifferentTasks );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Socket_TCP );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_SetSockOpt_RCVTIMEO );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_NonBlocking_Test );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_SetSockOpt_InvalidParams );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Shutdown );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Close );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Recv_ByteByByte );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_SendRecv_VaryLength );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Socket_InvalidTooManySockets );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Socket_InvalidInputParams );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Send_Invalid );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_Recv_Invalid );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_htons_HappyCase );
    RUN_TEST_CASE( Full_TCP, AFQP_SOCKETS_inet_addr_quick_HappyCase );

    #if ( tcptestSECURE_SERVER == 1 )
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_CloseInvalidParams );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_CloseWithoutReceiving );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_ShutdownInvalidParams );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_ShutdownWithoutReceiving );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Recv_On_Unconnected_Socket );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasks );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_SetSockOpt_TRUSTED_SERVER_CERTIFICATE );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_SetSockOpt_SERVER_NAME_INDICATION );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Connect_InvalidParams );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Connect_InvalidAddressLength );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Threadsafe_DifferentSocketsDifferentTasks );
        /* SECURE_SOCKETS_Socket_TCP DNE. */
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_SetSockOpt_RCVTIMEO );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_NonBlocking_Test );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_SetSockOpt_InvalidParams );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Shutdown );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Close );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Recv_ByteByByte );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_SendRecv_VaryLength );
        /* SECURE_SOCKETS_Socket_InvalidTooManySockets has not been implemented. */
        /*SECURE_SOCKETS_Socket_InvalidInputParams DNE.*/
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Send_Invalid );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_Recv_Invalid );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_SockEventHandler );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_NonBlockingConnect );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_TwoSecureConnections );
        RUN_TEST_CASE( Full_TCP, AFQP_SECURE_SOCKETS_SetSecureOptionsAfterConnect );
    #endif /* if ( tcptestSECURE_SERVER == 1 ) */
}

/*-------------------------------------------------------------------*/
/*-----------------------Begin Tests---------------------------------*/
/*-------------------------------------------------------------------*/

static void prvSOCKETS_CloseInvalidParams( Server_t xConn )
{
    BaseType_t xResult;

    /* Try to close with an invalid socket. */
    xResult = SOCKETS_Close( SOCKETS_INVALID_SOCKET );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_EINVAL, xResult, "Socket failed to close" );
}

TEST( Full_TCP, AFQP_SOCKETS_CloseInvalidParams )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_CloseInvalidParams( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_CloseInvalidParams )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_CloseInvalidParams( eSecure );
}

/*-----------------------------------------------------------*/

static void prvSOCKETS_CloseWithoutReceiving( Server_t xConn )
{
    BaseType_t xResult;

    /* Attempt to establish the requested connection. */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    /* Send a lot of data to the echo server but never use the recv. */
    xResult = SOCKETS_Send( xSocket, &cTransmittedString, tcptestTWICE_MAX_FRAME_SIZE, 0 );
    TEST_ASSERT_GREATER_THAN_MESSAGE( 0, xResult, "Socket was not able to send" );

    /* Try to close. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}


TEST( Full_TCP, AFQP_SOCKETS_CloseWithoutReceiving )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_CloseWithoutReceiving( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_CloseWithoutReceiving )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_CloseWithoutReceiving( eSecure );
}

/*-----------------------------------------------------------*/

static void prvSOCKETS_ShutdownInvalidParams( Server_t xConn )
{
    BaseType_t xResult;

    xSocket = prvTcpSocketHelper( &xSocketOpen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

    xResult = prvSetSockOptHelper( xSocket, xReceiveTimeOut, xSendTimeOut );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Set sockopt Failed" );

    xResult = SOCKETS_Shutdown( xSocket, ~0 );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket could shutdown with an invalid param" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    xSocket = prvTcpSocketHelper( &xSocketOpen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

    /* Call shutdown with invalid parameters. */
    xResult = SOCKETS_Shutdown( SOCKETS_INVALID_SOCKET, SOCKETS_SHUT_RDWR );
    TEST_ASSERT_EQUAL_MESSAGE( SOCKETS_EINVAL, xResult, "Socket could shutdown with an invalid param" );
}

TEST( Full_TCP, AFQP_SOCKETS_ShutdownInvalidParams )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_ShutdownInvalidParams( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_ShutdownInvalidParams )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_ShutdownInvalidParams( eSecure );
}

/*-----------------------------------------------------------*/
static void prvSOCKETS_ShutdownWithoutReceiving( Server_t xConn )
{
    BaseType_t xResult;

    /* Attempt to establish the requested connection. */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    /* Send a lot of data to the echo server but never use the recv. */
    xResult = SOCKETS_Send( xSocket, &cTransmittedString, tcptestTWICE_MAX_FRAME_SIZE, 0 );
    TEST_ASSERT_GREATER_THAN_MESSAGE( 0, xResult, "Socket was not able to send" );

    xResult = SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RDWR );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to shutdown" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}


TEST( Full_TCP, AFQP_SOCKETS_ShutdownWithoutReceiving )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_ShutdownWithoutReceiving( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_ShutdownWithoutReceiving )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_ShutdownWithoutReceiving( eSecure );
}

/*-----------------------------------------------------------*/

static void prvSOCKETS_Recv_On_Unconnected_Socket( Server_t xConn )
{
    BaseType_t xResult;
    uint8_t ucBuf;
    volatile BaseType_t xConnectedSocketOpen = pdFALSE;
    volatile Socket_t xConnectedSocket;

    if( TEST_PROTECT() )
    {
        xSocket = prvTcpSocketHelper( &xSocketOpen );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

        xResult = prvSetSockOptHelper( xSocket, xReceiveTimeOut, xSendTimeOut );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "SetSockOpt Failed" );


        /* We connect another socket. The rational for this is a bug that was experienced in the past
         * where the data would be received  by from another socket. */
        /* Attempt to establish the requested connection. */
        xResult = prvConnectHelperWithRetry( &xConnectedSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xConnectedSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

        /* Send data from the connected socket. */
        xResult = SOCKETS_Send( xConnectedSocket, &ucBuf, 1, 0 );
        TEST_ASSERT_GREATER_THAN_MESSAGE( 0, xResult, "Socket was not able to send" );

        /* Try to receive on unconnected socket. */
        xResult = SOCKETS_Recv( xSocket, &ucBuf, 1, 0 );
        TEST_ASSERT_LESS_THAN_MESSAGE( 0, xResult, "Socket was able to receive from unconnected socket" );

        xResult = prvCloseHelper( xSocket, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

        /* Close the other socket. */
        xResult = prvShutdownHelper( xConnectedSocket );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to shutdown" );

        xResult = prvCloseHelper( xConnectedSocket, &xConnectedSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
        xConnectedSocketOpen = pdFALSE;
    }

    if( xConnectedSocketOpen == pdTRUE )
    {
        ( void ) prvShutdownHelper( xConnectedSocket );

        xResult = prvCloseHelper( xConnectedSocket, &xConnectedSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close." );
    }
}

TEST( Full_TCP, AFQP_SOCKETS_Recv_On_Unconnected_Socket )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Recv_On_Unconnected_Socket( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Recv_On_Unconnected_Socket )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Recv_On_Unconnected_Socket( eSecure );
}


/*-----------------------------------------------------------*/

static void prvSOCKETS_Socket_TCP( Server_t xConn )
{
    BaseType_t xResult;

    /* Make TCP socket. */
    xSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                              SOCKETS_SOCK_STREAM,
                              SOCKETS_IPPROTO_TCP );

    TEST_ASSERT_NOT_EQUAL( xSocket, SOCKETS_INVALID_SOCKET );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

TEST( Full_TCP, AFQP_SOCKETS_Socket_TCP )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Socket_TCP( eNonsecure );
}

/* Since no connection is made, a secure version of this test is not needed. */

/*-----------------------------------------------------------*/

static void prvSOCKETS_SetSockOpt_RCVTIMEO( Server_t xConn )
{
    BaseType_t xResult;
    UBaseType_t uxOldPriority;
    TickType_t xStartTime;
    TickType_t xEndTime;
    TickType_t xTimeout;
    TickType_t xTimeouts[] = { 30, 100, 10000 }; /* TODO: Add 0, nonblocking tests */
    uint8_t ucBuffer;
    BaseType_t xIndex;


    xResult = pdPASS;
    tcptestPRINTF( ( "Starting %s \r\n", __FUNCTION__ ) );
    tcptestPRINTF( ( "This tests timeouts, so it takes a while! \r\n" ) );

    /* Attempt to establish the requested connection. */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    /* Record the priority of this task. */
    uxOldPriority = uxTaskPriorityGet( NULL );

    /*
     * Set this task to be the highest possible priority.
     * Since we are testing timeouts, we don't want other tasks
     * running in the middle of it.
     */
    vTaskPrioritySet( NULL, configMAX_PRIORITIES - 1 );

    /*
     * Set the receive timeout, check the tick count,
     * call receive (with no data sent), check the tick
     * count again, make sure time elapsed is within
     * time expected.
     */
    for( xIndex = 0; xIndex < sizeof( xTimeouts ) / sizeof( TickType_t ); xIndex++ )
    {
        xTimeout = pdMS_TO_TICKS( xTimeouts[ xIndex ] );
        xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_RCVTIMEO, &xTimeout, sizeof( TickType_t ) );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to set receive timeout" );

        xStartTime = xTaskGetTickCount();
        xResult = SOCKETS_Recv( xSocket, &ucBuffer, 1, 0 );
        xEndTime = xTaskGetTickCount();
        TEST_ASSERT_LESS_THAN_MESSAGE( 1, xResult, "Receive call failed in receive timeout test" );
        xResult = prvCheckTimeout( xStartTime, xEndTime, xTimeout );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Receive timeout was outside of acceptable range" );
    }

    /* Restore the old priority of this task. */
    vTaskPrioritySet( NULL, uxOldPriority );
    xResult = prvShutdownHelper( xSocket );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to shutdown" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    tcptestPRINTF( ( "%s passed\r\n", __FUNCTION__ ) );
}


TEST( Full_TCP, AFQP_SOCKETS_SetSockOpt_RCVTIMEO )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_SetSockOpt_RCVTIMEO( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_SetSockOpt_RCVTIMEO )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_SetSockOpt_RCVTIMEO( eSecure );
}
/*-----------------------------------------------------------*/

static void prvSOCKETS_NonBlocking_Test( Server_t xConn )
{
    BaseType_t xResult;
    int32_t lNumBytes;
    TickType_t xStartTime;
    TickType_t xEndTime;
    TickType_t xTimeout = 0;
    TickType_t xWaitTime = 1000;
    uint8_t * pucTxBuffer = ( uint8_t * ) pcTxBuffer;
    uint8_t * pucRxBuffer = ( uint8_t * ) pcRxBuffer;
    size_t xMessageLength = 1200;
    size_t xNumBytesReceived = 0;


    xResult = pdPASS;
    tcptestPRINTF( ( "Starting %s \r\n", __FUNCTION__ ) );
    tcptestPRINTF( ( "This tests timeouts, so it takes a while! \r\n" ) );

    if( TEST_PROTECT() )
    {
        /* Attempt to establish the requested connection. */
        xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

        /*
         * Set the receive timeout, check the tick count,
         * call receive (with no data sent), check the tick
         * count again, make sure time elapsed is within
         * time expected.
         */
        xResult = SOCKETS_SetSockOpt( xSocket, 0, SOCKETS_SO_NONBLOCK, &xTimeout, sizeof( TickType_t ) );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to set receive timeout" );

        xStartTime = xTaskGetTickCount();
        xResult = SOCKETS_Recv( xSocket, &pucRxBuffer, 1, 0 );
        xEndTime = xTaskGetTickCount();
        TEST_ASSERT_LESS_THAN_MESSAGE( 1, xResult, "Receive call failed in receive timeout test" );

        xResult = prvCheckTimeout( xStartTime, xEndTime, xTimeout );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Receive timeout was outside of acceptable range" );

        /*
         * Send data and receive data in nonblocking mode
         */
        memset( pucTxBuffer, tcptestTX_BUFFER_FILLER, tcptestBUFFER_SIZE );
        prvCreateTxData( ( char * ) pucTxBuffer, xMessageLength, 0 );

        xResult = prvSendHelper( xSocket, pucTxBuffer, xMessageLength );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Data failed to send\r\n" );

        memset( pucRxBuffer, tcptestRX_BUFFER_FILLER, tcptestBUFFER_SIZE );
        xStartTime = xTaskGetTickCount();
        xEndTime = xTaskGetTickCount();

        do
        {
            lNumBytes = SOCKETS_Recv( xSocket, &pucRxBuffer[ xNumBytesReceived ], xMessageLength - xNumBytesReceived, 0 );

            if( lNumBytes > 0 )
            {
                /*
                 * If xResult is negative, it's an error code. Because xNumBytesReceived is an
                 * unsigned int, it will become a large postive number, so the while loop
                 * would end prematurely. This if statement prevents that.
                 */
                xNumBytesReceived += lNumBytes;
            }

            xEndTime = xTaskGetTickCount();
        } while( ( ( xEndTime - xStartTime ) < xWaitTime ) && ( xMessageLength > xNumBytesReceived ) );

        TEST_ASSERT_EQUAL_INT32_MESSAGE( xMessageLength, xNumBytesReceived, "Data was not received \r\n" );

        xResult = prvCheckRxTxBuffers( pucTxBuffer, pucRxBuffer, xMessageLength );

        xResult = prvShutdownHelper( xSocket );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to shutdown" );
    }

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    tcptestPRINTF( ( "%s passed\r\n", __FUNCTION__ ) );
}


TEST( Full_TCP, AFQP_SOCKETS_NonBlocking_Test )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_NonBlocking_Test( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_NonBlocking_Test )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_NonBlocking_Test( eSecure );
}
/*-----------------------------------------------------------*/

static void prvSetSockOpt_InvalidParams( Server_t xConn )
{
    BaseType_t xResult;

    if( TEST_PROTECT() )
    {
        /* Attempt to establish the requested connection. */
        xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

        /* Try to set the invalid option. */
        xResult = SOCKETS_SetSockOpt( xSocket,
                                      0,    /* lLevel - Not used. */
                                      -1,   /* Invalid option name. */
                                      NULL, /* pvOptionValue - This is insignificant as the option name is invalid. */
                                      0     /* xOptionLength - zero for NULL value. */
                                      );

        /* Since the option name supplied in the previous
         * call was invalid, we expect the call to the API
         * SOCKETS_SetSockOpt to fail. */
        TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "SOCKETS_SetSockOpt with invalid option succeeded." );

        /* Try to set a valid option on an invalid socket. */
        xResult = SOCKETS_SetSockOpt( SOCKETS_INVALID_SOCKET, /* Invalid socket. */
                                      0,                      /* lLevel - Not used. */
                                      SOCKETS_SO_RCVTIMEO,    /* Receive timeout. */
                                      &( xReceiveTimeOut ),
                                      sizeof( TickType_t ) );

        /* Since the socket passed in the previous call was
         * invalid, we expect the call to the API
         * SOCKETS_SetSockOpt to fail. */
        TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "SOCKETS_SetSockOpt with invalid socket succeeded." );
    }

    /* If the code reaches here, it means that the socket
     * was created successfully. Make sure to free it even
     * if any of the above assertion fails. This is needed
     * to avoid leaks. */
    xResult = prvCloseHelper( xSocket, &( xSocketOpen ) );

    /* If this fails there is nothing much we can do
     * to avoid leaking. */
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "SOCKETS_Close failed. \r\n" );
}

TEST( Full_TCP, AFQP_SOCKETS_SetSockOpt_InvalidParams )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSetSockOpt_InvalidParams( eNonsecure );

    tcptestPRINTF( ( "%s complete.\r\n", __FUNCTION__ ) );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_SetSockOpt_InvalidParams )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSetSockOpt_InvalidParams( eSecure );

    tcptestPRINTF( ( "%s complete.\r\n", __FUNCTION__ ) );
}
/*-----------------------------------------------------------*/

static void prvSOCKETS_SetSockOpt_SNDTIMEO( Server_t xConn )
{
    /* TODO: This is a stub function. */
    TEST_FAIL_MESSAGE( "This test is not implemented." );
}

TEST( Full_TCP, AFQP_SOCKETS_SetSockOpt_SNDTIMEO )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_SetSockOpt_SNDTIMEO( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_SetSockOpt_SNDTIMEO )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_SetSockOpt_SNDTIMEO( eSecure );
}

/*-----------------------------------------------------------*/

static void prvSOCKETS_Shutdown( Server_t xConn )
{
    BaseType_t xResult;
    uint8_t ucBuf;

    xResult = pdPASS;

    /* Shutdown: Read. */

    /* Attempt to establish the requested connection. */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    xResult = SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RD );
    TEST_ASSERT_EQUAL_INT32( SOCKETS_ERROR_NONE, xResult );

    do
    {
        xResult = SOCKETS_Recv( xSocket, &ucBuf, 1, 0 );
    }
    while( xResult >= 0 );

    TEST_ASSERT_LESS_THAN_UINT32( 0, xResult );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* Shutdown: Write. */

    /* Attempt to establish the requested connection. */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    /* AN: Should this test also make a send before closing the socket,
     * and make sure that the data is received "gracefully"? */

    xResult = SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_WR );
    TEST_ASSERT_EQUAL_INT32( SOCKETS_ERROR_NONE, xResult );

    /* Expected behavior of send after Shutdown/WRITE is ambiguous.
     * do
     * {
     *  xResult = SOCKETS_Send( xSocket, &ucBuf, 1, 0 );
     * }
     * while( xResult >= 0 );
     * TEST_ASSERT_LESS_THAN_UINT32( 0, xResult );
     */

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* Shutdown: Read/Write. */

    /* Attempt to establish the requested connection. */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    xResult = SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RDWR );
    TEST_ASSERT_EQUAL_INT32( SOCKETS_ERROR_NONE, xResult );

    do
    {
        xResult = SOCKETS_Recv( xSocket, &ucBuf, 1, 0 );
    }
    while( xResult >= 0 );

    TEST_ASSERT_LESS_THAN_UINT32( 0, xResult );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

TEST( Full_TCP, AFQP_SOCKETS_Shutdown )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Shutdown( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Shutdown )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Shutdown( eSecure );
}

/*-----------------------------------------------------------*/

static void prvTestSOCKETS_Close( Server_t xConn )
{
    BaseType_t xResult;
    uint8_t ucBuf;

    xResult = pdPASS;

    /* Close an unconnected socket */
    xSocket = prvTcpSocketHelper( &xSocketOpen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* The secure sockets API abstraction requires that Socket_t be treated as
     * opaque. It could be a pointer, a handle, an index, or whatever. In addition,
     * SOCKETS_Close treats the socket only as input. Therefore, it is not a valid
     * test to pass a socket that has been freed, via explicit close, to other
     * downstream socket APIs. */
    xSocket = SOCKETS_INVALID_SOCKET;

    /* Closed socket should not connect, send or receive */
    xResult = SOCKETS_Send( xSocket, &ucBuf, 1, 0 );
    TEST_ASSERT_LESS_THAN_UINT32( 0, xResult );
    xResult = SOCKETS_Recv( xSocket, &ucBuf, 1, 0 );
    TEST_ASSERT_LESS_THAN_UINT32( 0, xResult );

    /* Close a connected socket */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* See comment above. */
    xSocket = SOCKETS_INVALID_SOCKET;

    /* Closed socket should not connect, send or receive */
    xResult = SOCKETS_Send( xSocket, &ucBuf, 1, 0 );
    TEST_ASSERT_LESS_THAN_UINT32( 0, xResult );
    xResult = SOCKETS_Recv( xSocket, &ucBuf, 1, 0 );
    TEST_ASSERT_LESS_THAN_UINT32( 0, xResult );

    /* Close a shutdown socket */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    xResult = prvShutdownHelper( xSocket );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to shutdown" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* See comment above. */
    xSocket = SOCKETS_INVALID_SOCKET;

    /* Closed socket should not connect, send or receive */
    xResult = SOCKETS_Send( xSocket, &ucBuf, 1, 0 );
    TEST_ASSERT_LESS_THAN_UINT32( 0, xResult );
    xResult = SOCKETS_Recv( xSocket, &ucBuf, 1, 0 );
    TEST_ASSERT_LESS_THAN_UINT32( 0, xResult );

    tcptestPRINTF( ( "%s passed\r\n", __FUNCTION__ ) );
}

TEST( Full_TCP, AFQP_SOCKETS_Close )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvTestSOCKETS_Close( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Close )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvTestSOCKETS_Close( eSecure );
}


/*-----------------------------------------------------------*/

static void prvTestSOCKETS_Recv_ByteByByte( Server_t xConn )
{
    BaseType_t xResult = pdFAIL;
    uint32_t ulTxCount;
    size_t xBytesReceived;
    uint8_t * pucTxBuffer = ( uint8_t * ) pcTxBuffer;
    uint8_t * pucRxBuffer = ( uint8_t * ) pcRxBuffer;
    size_t xMessageLengths[] = { 2, 7, 8, 9, 20 };

    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    /* Attempt to establish the requested connection. */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    for( ulTxCount = 0; ulTxCount < sizeof( xMessageLengths ) / sizeof( xMessageLengths[ 0 ] ); ulTxCount++ )
    {
        xBytesReceived = 0;

        prvCreateTxData( ( char * ) pucTxBuffer, xMessageLengths[ ulTxCount ], ulTxCount );
        xResult = prvSendHelper( xSocket, pucTxBuffer, xMessageLengths[ ulTxCount ] );

        while( ( xResult == pdPASS ) && ( xBytesReceived < xMessageLengths[ ulTxCount ] ) )
        {
            memset( pucRxBuffer, tcptestRX_BUFFER_FILLER, tcptestBUFFER_SIZE );
            xResult = prvRecvHelper( xSocket, pucRxBuffer, 1 );

            if( xResult == pdPASS )
            {
                if( pucRxBuffer[ 0 ] == pucTxBuffer[ xBytesReceived ] )
                {
                    /* Check that all but the first byte of the RxBuffer is unmodified. */
                    xResult = prvCheckBufferUnmodified( pucRxBuffer + 1,
                                                        tcptestRX_BUFFER_FILLER,
                                                        tcptestBUFFER_SIZE - 1 );
                }
                else
                {
                    xResult = pdFAIL;
                    tcptestPRINTF( ( "Byte %d was incorrectly received\r\n", ( xBytesReceived + 1 ) ) );
                }
            }

            xBytesReceived++;
        }
    }

    /* Close this socket before looping back to create another. */
    TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Failed received\r\n" );
    xResult = prvShutdownHelper( xSocket );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to shutdown" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

TEST( Full_TCP, AFQP_SOCKETS_Recv_ByteByByte )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvTestSOCKETS_Recv_ByteByByte( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Recv_ByteByByte )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvTestSOCKETS_Recv_ByteByByte( eSecure );
}

/*-----------------------------------------------------------*/

static void prvSOCKETS_SendRecv_VaryLength( Server_t xConn )
{
    BaseType_t xResult;
    uint32_t ulIndex;
    uint32_t ulTxCount;
    const uint32_t ulMaxLoopCount = 10;
    uint32_t ulI;
    uint8_t * pucTxBuffer = ( uint8_t * ) pcTxBuffer;
    uint8_t * pucRxBuffer = ( uint8_t * ) pcRxBuffer;
    size_t xMessageLengths[] = { 1, 2, 7, 8, 9, 1199, 1200, 1201 }; /* TODO: Add 0, send sizes larger than MTU. */

    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    xResult = pdPASS;
    ulTxCount = 0;

    for( ulIndex = 0; ulIndex < sizeof( xMessageLengths ) / sizeof( size_t ); ulIndex++ )
    {
        /* Attempt to establish the requested connection. */
        xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

        /* Send each message length ulMaxLoopCount times. */
        for( ulI = 0; ulI < ulMaxLoopCount; ulI++ )
        {
            memset( pucTxBuffer, tcptestTX_BUFFER_FILLER, tcptestBUFFER_SIZE );

            prvCreateTxData( ( char * ) pucTxBuffer,
                             xMessageLengths[ ulIndex ],
                             ulTxCount );
            ulTxCount++;

            xResult = prvSendHelper( xSocket,
                                     pucTxBuffer,
                                     xMessageLengths[ ulIndex ] );

            TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Data failed to send\r\n" );
            memset( pucRxBuffer, tcptestRX_BUFFER_FILLER, tcptestBUFFER_SIZE );
            xResult = prvRecvHelper( xSocket,
                                     pucRxBuffer,
                                     xMessageLengths[ ulIndex ] );

            TEST_ASSERT_EQUAL_INT32_MESSAGE( pdPASS, xResult, "Data was not received \r\n" );
            xResult = prvCheckRxTxBuffers( pucTxBuffer,
                                           pucRxBuffer,
                                           xMessageLengths[ ulIndex ] );
        }

        xResult = prvShutdownHelper( xSocket );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to shutdown" );

        xResult = prvCloseHelper( xSocket, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

        /* Print feedback since this test takes a while! */
        tcptestPRINTF( ( " Sending messages with length %d complete\r\n", xMessageLengths[ ulIndex ] ) );

        /* Pause for a short while to ensure the network is not too
         * congested. */
        vTaskDelay( tcptestLOOP_DELAY );
    }

    /* Report Test Results. */
    tcptestPRINTF( ( "%s passed\r\n", __FUNCTION__ ) );
}

TEST( Full_TCP, AFQP_SOCKETS_SendRecv_VaryLength )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_SendRecv_VaryLength( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_SendRecv_VaryLength )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_SendRecv_VaryLength( eSecure );
}

/*/ *-----------------------------------------------------------* / */

static void prvSOCKETS_Socket_InvalidInputParams( Server_t xConn )
{
    BaseType_t xResult;

    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    /* Providing invalid domain. */
    if( TEST_PROTECT() )
    {
        xSocket = SOCKETS_Socket( ( SOCKETS_AF_INET + 1 ),
                                  SOCKETS_SOCK_STREAM,
                                  SOCKETS_IPPROTO_TCP );

        /* If the test code reaches here, it failed. */
        TEST_FAIL_MESSAGE( "Invalid socket domain accepted" );
        xResult = prvCloseHelper( xSocket, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
    }

    /* Providing invalid type. */
    if( TEST_PROTECT() )
    {
        xSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                  ( SOCKETS_SOCK_STREAM | SOCKETS_SOCK_DGRAM ),
                                  SOCKETS_IPPROTO_TCP );

        TEST_FAIL_MESSAGE( "Invalid socket type accepted" );
        xResult = prvCloseHelper( xSocket, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
    }

    /* Providing invalid protocol. */
    if( TEST_PROTECT() )
    {
        xSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                  SOCKETS_SOCK_STREAM,
                                  ( SOCKETS_IPPROTO_TCP | SOCKETS_IPPROTO_UDP ) );

        TEST_FAIL_MESSAGE( "Invalid socket protocol accepted" );
        xResult = prvCloseHelper( xSocket, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
    }

    /* Creating a UPD socket. UDP is unsupported. */
    if( TEST_PROTECT() )
    {
        /* Mixing DGRAM type with TCP protocol (instead of UDP). */
        xSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                  SOCKETS_SOCK_DGRAM,
                                  SOCKETS_IPPROTO_UDP );

        TEST_FAIL_MESSAGE( "Invalid socket created - UDP is not supported. " );
        prvCloseHelper( xSocket, &xSocketOpen );
    }

    if( TEST_PROTECT() )
    {
        /* Mixing DGRAM type with TCP protocol (instead of UDP). */
        xSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                  SOCKETS_SOCK_DGRAM,
                                  SOCKETS_IPPROTO_TCP );

        TEST_FAIL_MESSAGE( "Invalid socket created - mixed TCP with DGRAM " );
        xResult = prvCloseHelper( xSocket, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
    }

    /* Mixing STREAM type with UDP protocol (instead of TCP). */
    if( TEST_PROTECT() )
    {
        xSocket = SOCKETS_Socket( SOCKETS_AF_INET,
                                  SOCKETS_SOCK_STREAM,
                                  SOCKETS_IPPROTO_UDP );

        TEST_FAIL_MESSAGE( "Invalid socket created - mixed UDP with STREAM" );
        xResult = prvCloseHelper( xSocket, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
    }

    /* Report Test Results. */
    tcptestPRINTF( ( "%s passed\r\n", __FUNCTION__ ) );
}

TEST( Full_TCP, AFQP_SOCKETS_Socket_InvalidInputParams )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Socket_InvalidInputParams( eNonsecure );
}

/* No secure stub needed here since there are no secure input params. */

/*-----------------------------------------------------------*/

static void prvSOCKETS_Socket_InvalidTooManySockets( Server_t xConn )
{
    #if !defined( WIN32 ) && !defined( PIC32MZ ) && !defined( ESP32 ) && !defined( ZYNQ7000 ) && !defined( __RX ) /* Socket can be created as much as there is memory */
        BaseType_t xResult;
        Socket_t xCreatedSockets[ integrationtestportableMAX_NUM_UNSECURE_SOCKETS ];
        BaseType_t xSocketsCreated;
        Socket_t xSocket;

        tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

        xResult = pdPASS;

        for( xSocketsCreated = 0; xSocketsCreated < integrationtestportableMAX_NUM_UNSECURE_SOCKETS; xSocketsCreated++ )
        {
            xSocket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );

            if( xSocket == SOCKETS_INVALID_SOCKET )
            {
                xResult = pdFAIL;
                tcptestPRINTF( ( "%s failed creating a socket number %d \r\n", __FUNCTION__, xSocketsCreated ) );
                break;
            }
            else
            {
                xCreatedSockets[ xSocketsCreated ] = xSocket;
            }
        }

        if( xResult == pdPASS )
        {
            xSocket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );

            if( xSocket != SOCKETS_INVALID_SOCKET )
            {
                xResult = pdFAIL;
                SOCKETS_Close( xSocket );
            }
        }

        /* Cleanup. */
        while( xSocketsCreated > 0 )
        {
            --xSocketsCreated;
            SOCKETS_Close( xCreatedSockets[ xSocketsCreated ] );
        }

        TEST_ASSERT_EQUAL_UINT32_MESSAGE( pdPASS, xResult, "Max num sockets test failed" );

        /* Report Test Results. */
        tcptestPRINTF( ( "%s passed\r\n", __FUNCTION__ ) );
    #endif /*ifndef WIN32 */
}

TEST( Full_TCP, AFQP_SOCKETS_Socket_InvalidTooManySockets )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Socket_InvalidTooManySockets( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Socket_InvalidTooManySockets )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );
    /* TODO: Modify so that the number of secure sockets can be counted. */
    /*prvSOCKETS_Socket_InvalidTooManySockets( secure ); */
}



/*-----------------------------------------------------------*/

static void prvSOCKETS_Connect_InvalidParams( Server_t xConn )
{
    BaseType_t xResult;
    SocketsSockaddr_t xEchoServerAddress;

    uint32_t ulEchoServerIP;

    ulEchoServerIP = SOCKETS_inet_addr_quick( tcptestECHO_SERVER_ADDR0,
                                              tcptestECHO_SERVER_ADDR1,
                                              tcptestECHO_SERVER_ADDR2,
                                              tcptestECHO_SERVER_ADDR3 );


    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    xSocket = prvTcpSocketHelper( &xSocketOpen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

    /* Echo requests are sent to the echo server.  The echo server is
    * listening to tcptestECHO_PORT on this computer's IP address. */
    xEchoServerAddress.usPort = SOCKETS_htons( tcptestECHO_PORT );
    xEchoServerAddress.ulAddress = ulEchoServerIP;

    /* Invalid socket. */
    xResult = SOCKETS_Connect( SOCKETS_INVALID_SOCKET,
                               &xEchoServerAddress,
                               sizeof( xEchoServerAddress ) );

    TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Connect on an invalid socket failed" );

    /* Invalid IP address (0.0.0.0).  TODO: Investigate reserved IP addresses */
    xEchoServerAddress.ulAddress = SOCKETS_inet_addr_quick( 0, 0, 0, 0 );
    xResult = SOCKETS_Connect( xSocket,
                               &xEchoServerAddress,
                               sizeof( xEchoServerAddress ) );

    TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Connect to IP Address 0.0.0.0 worked" );

    /* Invalid port (0). */
    xEchoServerAddress.usPort = SOCKETS_htons( 0 );
    xEchoServerAddress.ulAddress = ulEchoServerIP;
    xResult = SOCKETS_Connect( xSocket,
                               &xEchoServerAddress,
                               sizeof( xEchoServerAddress ) );

    TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Connect to Port 0 worked" );

    /* NULL Address. */
    xResult = SOCKETS_Connect( xSocket, NULL, 0 );

    /* Ensure that the attempt to connect to NULL address fails. */
    TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Connect to NULL Address worked." );

    /* TODO: Does port 0 mean connect to any available port? */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

TEST( Full_TCP, AFQP_SOCKETS_Connect_InvalidParams )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Connect_InvalidParams( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Connect_InvalidParams )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Connect_InvalidParams( eSecure );
}

/*-----------------------------------------------------------*/

static void prvConnect_InvalidAddressLength( Server_t xConn,
                                             uint32_t ulAddressLength )
{
    BaseType_t xResult;
    SocketsSockaddr_t xEchoServerAddress;

    if( TEST_PROTECT() )
    {
        if( xConn == eNonsecure )
        {
            /* Populate the non-secure echo server address. */
            xEchoServerAddress.ulAddress = SOCKETS_inet_addr_quick( tcptestECHO_SERVER_ADDR0,
                                                                    tcptestECHO_SERVER_ADDR1,
                                                                    tcptestECHO_SERVER_ADDR2,
                                                                    tcptestECHO_SERVER_ADDR3 );
            xEchoServerAddress.usPort = SOCKETS_htons( tcptestECHO_PORT );
        }
        else if( xConn == eSecure )
        {
            /* Populate the secure echo server address. */
            xEchoServerAddress.ulAddress = SOCKETS_inet_addr_quick( tcptestECHO_SERVER_TLS_ADDR0,
                                                                    tcptestECHO_SERVER_TLS_ADDR1,
                                                                    tcptestECHO_SERVER_TLS_ADDR2,
                                                                    tcptestECHO_SERVER_TLS_ADDR3 );
            xEchoServerAddress.usPort = SOCKETS_htons( tcptestECHO_PORT_TLS );
        }
        else
        {
            /* This test is not designed for AWS IoT Broker. */
            TEST_ABORT();
        }

        /* Create a socket. */
        xSocket = prvTcpSocketHelper( &( xSocketOpen ) );

        /* Ensure that the socket was created successfully. */
        TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed." );
        TEST_ASSERT_EQUAL_MESSAGE( pdTRUE, xSocketOpen, "Socket creation failed." );

        /* Connect to the echo server but instead of calculating
         * the length of xEchoServerAddress, specify the length
         * supplied as parameter to this function for the xAddressLength
         * field. Since xAddressLength field of SOCKETS_Connect is
         * currently ignored, we expect the connect to succeed.
         */
        xResult = SOCKETS_Connect( xSocket, &( xEchoServerAddress ), ulAddressLength );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket connect failed." );
    }

    /* If a socket was created, make sure to free it even
     * if any of the above assertion fails. This is needed
     * to avoid leaks. */
    if( xSocketOpen == pdTRUE )
    {
        xResult = prvCloseHelper( xSocket, &( xSocketOpen ) );

        /* If this fails there is nothing much we can do
         * to avoid leaking. */
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "SOCKETS_Close failed. \r\n" );
    }
}
/*-----------------------------------------------------------*/

TEST( Full_TCP, AFQP_SOCKETS_Connect_InvalidAddressLength )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    /* AddressLength 0. */
    prvConnect_InvalidAddressLength( eNonsecure, 0 );

    /* AddressLength 1000. */
    prvConnect_InvalidAddressLength( eNonsecure, 1000 );

    tcptestPRINTF( ( "%s complete.\r\n", __FUNCTION__ ) );
}
/*-----------------------------------------------------------*/

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Connect_InvalidAddressLength )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    /* AddressLength 0. */
    prvConnect_InvalidAddressLength( eSecure, 0 );

    /* AddressLength 1000. */
    prvConnect_InvalidAddressLength( eSecure, 1000 );

    tcptestPRINTF( ( "%s complete.\r\n", __FUNCTION__ ) );
}
/*-----------------------------------------------------------*/

static void prvSend_Invalid( Server_t xConn )
{
    BaseType_t xResult;
    uint8_t * pucTxBuffer = ( uint8_t * ) pcTxBuffer;

    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    /* Unconnected socket. */
    if( TEST_PROTECT() )
    {
        /* Attempt to establish the requested connection. */
        xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

        /* Invalid socket. */
        xResult = SOCKETS_Send( SOCKETS_INVALID_SOCKET, pucTxBuffer, 300, 0 );
        TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Socket send fails with invalid socket" );

        /* NULL Tx Buffer. */

        xResult = SOCKETS_Send( xSocket, NULL, 300, 0 );
        TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Socket send should return error for NULL input buffer" );
    }
    else
    {
        TEST_FAIL_MESSAGE( "Sending on an TCP socket with NULL buffer triggered an assert " );
    }

    /* Cleanup. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* Unconnected socket. */
    if( TEST_PROTECT() )
    {
        xSocket = prvTcpSocketHelper( &xSocketOpen );
        TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

        xResult = prvSetSockOptHelper( xSocket, xReceiveTimeOut, xSendTimeOut );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Set sockopt failed" );
        xResult = SOCKETS_Send( xSocket, pucTxBuffer, 300, 0 );
        TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Socket send worked with an unconnected socket" );

        xResult = prvCloseHelper( xSocket, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket close failed" );
    }
    else
    {
        TEST_FAIL_MESSAGE( "Sending on an unconnected TCP socket triggered an assert " );
    }

    /* Report Test Results. */
    tcptestPRINTF( ( "%s passed\r\n", __FUNCTION__ ) );
}

TEST( Full_TCP, AFQP_SOCKETS_Send_Invalid )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSend_Invalid( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Send_Invalid )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSend_Invalid( eSecure );
}

/*-------------------------------------------------------------------*/

static void prvRecv_Invalid( Server_t xConn )
{
    BaseType_t xResult;

    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    /* Attempt to establish the requested connection. */
    xResult = prvConnectHelperWithRetry( &xSocket, xConn, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    /* Receive with NULL buffer should fail. */
    xResult = SOCKETS_Recv( xSocket, NULL, 100, 0 );
    TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Socket receive with NULL receive buffer should have triggered error" );

    /* Receive on invalid socket should fail. */
    xResult = SOCKETS_Recv( SOCKETS_INVALID_SOCKET, pcRxBuffer, tcptestBUFFER_SIZE, 0 );
    TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Socket receive with invalid socket should have triggered error" );

    /* Cleanup. */
    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

TEST( Full_TCP, AFQP_SOCKETS_Recv_Invalid )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvRecv_Invalid( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Recv_Invalid )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvRecv_Invalid( eSecure );
}

/*-------------------------------------------------------------------*/

static void prvServerDomainName( void )
{
    BaseType_t xResult;
    SocketsSockaddr_t xAwsBrokerAddress;

    /* TODO: Generically change this. */
    char cRealAddress[ sizeof( clientcredentialMQTT_BROKER_ENDPOINT ) ];
    char cFakeAddress[ sizeof( clientcredentialMQTT_BROKER_ENDPOINT ) ];

    memcpy( cRealAddress, clientcredentialMQTT_BROKER_ENDPOINT, sizeof( clientcredentialMQTT_BROKER_ENDPOINT ) );
    memcpy( cFakeAddress, cRealAddress, sizeof( clientcredentialMQTT_BROKER_ENDPOINT ) );
    /* .com -> .cpm */
    cFakeAddress[ sizeof( clientcredentialMQTT_BROKER_ENDPOINT ) - 3 ]++;


    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    xSocket = prvTcpSocketHelper( &xSocketOpen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

    xResult = prvSetSockOptHelper( xSocket, xReceiveTimeOut, xSendTimeOut );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Set sockopt Failed" );

    /* Give the client a valid subject name to check against. */
    xResult = SOCKETS_SetSockOpt( xSocket,
                                  0,
                                  SOCKETS_SO_SERVER_NAME_INDICATION,
                                  cRealAddress,
                                  sizeof( cRealAddress ) );
    xResult = prvConnectHelper( xSocket, eAwsBroker );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    xSocket = prvTcpSocketHelper( &xSocketOpen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

    xResult = prvSetSockOptHelper( xSocket, xReceiveTimeOut, xSendTimeOut );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Set sockopt Failed" );

    /* Give the client an INVALID subject name to check against. */
    xResult = SOCKETS_SetSockOpt( xSocket,
                                  0,
                                  SOCKETS_SO_SERVER_NAME_INDICATION,
                                  cFakeAddress,
                                  sizeof( cFakeAddress ) );

    /* Get the address struct for AWS Broker and SetSockOpt REQUIRE_TLS. */
    xResult = prvAwsIotBrokerConnectHelper( xSocket, &xAwsBrokerAddress );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed setup AWS Broker address struct." );

    xResult = SOCKETS_Connect( xSocket, &xAwsBrokerAddress, sizeof( SocketsSockaddr_t ) );
    TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Connect worked when subject name check should have failed it." );

    /* Server unverified indicates a connection
     *  was established without verifying the server. */
    if( xResult != SOCKETS_TLS_HANDSHAKE_ERROR )
    {
        xResult = SOCKETS_Send( xSocket, pcTxBuffer, 100, 0 );
        TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Send allowed after subject name connection failure." );
    }

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_SetSockOpt_SERVER_NAME_INDICATION )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvServerDomainName();
}

/*-------------------------------------------------------------------*/

static void prvTrustedServerCertificate( void )
{
    BaseType_t xResult = pdFAIL;
    SocketsSockaddr_t xSecureEchoServerAddress;

    /* Attempt to establish the requested connection. */
    xResult = prvConnectHelperWithRetry( &xSocket, eSecure, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );

    /* Repeat the steps above, but without using SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE.
     * Make sure that an error is returned. */
    xSocket = prvTcpSocketHelper( &xSocketOpen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

    xResult = prvSetSockOptHelper( xSocket, xReceiveTimeOut, xSendTimeOut );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Set sockopt Failed" );

    /* Set the socket to use TLS. */
    xResult = SOCKETS_SetSockOpt( xSocket,
                                  0,
                                  SOCKETS_SO_REQUIRE_TLS,
                                  NULL,
                                  ( size_t ) 0 );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "SetSockOpt to require TLS failed" );

    /* Echo requests are sent to the secure echo server and port. */
    xSecureEchoServerAddress.ulAddress = SOCKETS_inet_addr_quick( tcptestECHO_SERVER_TLS_ADDR0,
                                                                  tcptestECHO_SERVER_TLS_ADDR1,
                                                                  tcptestECHO_SERVER_TLS_ADDR2,
                                                                  tcptestECHO_SERVER_TLS_ADDR3 );

    xSecureEchoServerAddress.usPort = SOCKETS_htons( tcptestECHO_PORT_TLS );

    xResult = SOCKETS_Connect( xSocket, &xSecureEchoServerAddress, sizeof( xSecureEchoServerAddress ) );
    TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Connection permitted with untrusted server CA cert" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_SetSockOpt_TRUSTED_SERVER_CERTIFICATE )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvTrustedServerCertificate();
}


/* This test is different than the previous test because it causes the
 * SL_SSL_NOTIFICATION_WRONG_ROOT_CA asynchronous event to be hit.
 */
static void prvTriggerWrongRootCA( void )
{
    BaseType_t xResult = pdFAIL;
    SocketsSockaddr_t xAwsIotBrokerAddress;

    /* Repeat the steps above, but without using SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE.
     * Make sure that an error is returned. */
    xSocket = prvTcpSocketHelper( &xSocketOpen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );
    xResult = prvSetSockOptHelper( xSocket, xReceiveTimeOut, xSendTimeOut );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Set sockopt Failed" );

    /* Set the socket to use TLS. */
    xResult = SOCKETS_SetSockOpt( xSocket,
                                  0,
                                  SOCKETS_SO_REQUIRE_TLS,
                                  NULL,
                                  ( size_t ) 0 );

    /* Set the socket to use the secure server's root CA cert. */
    xResult = SOCKETS_SetSockOpt( xSocket,
                                  0,
                                  SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                                  tcptestECHO_HOST_ROOT_CA,
                                  sizeof( tcptestECHO_HOST_ROOT_CA ) );

    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "SetSockOpt to require TLS failed" );

    /* Echo requests are sent to the secure echo server and port. */
    xAwsIotBrokerAddress.ulAddress = SOCKETS_GetHostByName( clientcredentialMQTT_BROKER_ENDPOINT );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xAwsIotBrokerAddress.ulAddress, "DNS look up failed." );
    xAwsIotBrokerAddress.usPort = SOCKETS_htons( clientcredentialMQTT_BROKER_PORT );

    xResult = SOCKETS_Connect( xSocket, &xAwsIotBrokerAddress, sizeof( xAwsIotBrokerAddress ) );
    TEST_ASSERT_LESS_THAN_INT32_MESSAGE( 0, xResult, "Connection permitted with untrusted server CA cert" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_SockEventHandler )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvTriggerWrongRootCA();
}

/*-----------------------------------------------------------*/

/** @brief This test will create a task the will send data to an echo server.
 * The data coming back will be received in that task.
 * The size of receiving buffer, the priority, the size of data send,
 * will keep changing to cover a maximum cases.
 */
static void prvSOCKETS_Threadsafe_SameSocketDifferentTasks( Server_t xConn )
{
    BaseType_t xTotalReceived, xReturned = 0;
    size_t xRecvLoop, xRecvLen;
    tcptestEchoTestModes_t xMode;
    BaseType_t xResult;
    volatile char * pcReceivedString;
    volatile BaseType_t xReceivedStringAllocated = pdFALSE;
    volatile BaseType_t xSocketPassingQueueAllocated = pdFALSE;
    volatile BaseType_t xSyncEventGroupAllocated = pdFALSE;
    volatile TaskHandle_t xCreatedTask;

    if( TEST_PROTECT() )
    {
        pcReceivedString = pvPortMalloc( ipconfigTCP_MSS * sizeof( char ) );
        configASSERT( pcReceivedString != NULL );
        xReceivedStringAllocated = pdTRUE;

        /* Create the queue used to pass the socket to use from the Tx task to the
         * Rx task. */
        xSocketPassingQueue = xQueueCreate( 1, sizeof( Socket_t ) );
        configASSERT( xSocketPassingQueue );
        xSocketPassingQueueAllocated = pdTRUE;

        /* Create the event group used by the Tx and Rx tasks to synchronize prior
         * to commencing a cycle using a new socket. */
        xSyncEventGroup = xEventGroupCreate();
        configASSERT( xSyncEventGroup );
        xSyncEventGroupAllocated = pdTRUE;

        /* Create the task that sends to an echo server, but lets a different task
         * receive the reply on the same socket. */
        xResult = xTaskCreate( prvEchoClientTxTask,              /* The function that implements the task. */
                               "EchoMultiTx",                    /* Just a text name for the task to aid debugging. */
                               tcptestTCP_ECHO_TASKS_STACK_SIZE, /* The stack size is defined in aws_demo_config.h. */
                               NULL,                             /* The task parameter, not used in this case. */
                               tcptestTCP_ECHO_TASKS_PRIORITY,   /* The priority assigned to the task is defined in aws_demo_config.h. */
                               ( TaskHandle_t * ) &xCreatedTask );
        TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Task creation failed" );

        for( xRecvLoop = 0; xRecvLoop < tcptestMAX_LOOPS_ECHO_TEST; xRecvLoop++ )
        {
            /* Attempt to establish the requested connection. */
            xResult = prvConnectHelperWithRetry( &xSocket, xConn, xEchoTestRxTxTimeOut, xEchoTestRxTxTimeOut, &xSocketOpen );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

            /* Wait to receive the socket that will be used from the Tx task. */
            if( xEventGroupSync( xSyncEventGroup,                             /* The event group used for the rendezvous. */
                                 tcptestRX_TASK_BIT,                          /* The bit representing the Tx task reaching the rendezvous. */
                                 ( tcptestTX_TASK_BIT | tcptestRX_TASK_BIT ), /* Also wait for the Rx task. */
                                 tcptestECHO_TEST_SYNC_TIMEOUT_TICKS ) != ( tcptestTX_TASK_BIT | tcptestRX_TASK_BIT ) )
            {
                TEST_FAIL();
            }

            /* Nothing received yet. */
            xTotalReceived = 0;

            xMode = ( tcptestEchoTestModes_t ) ( xRecvLoop % tcptestMAX_ECHO_TEST_MODES ); /* % should be optimized to simple masking since only 4 modes are present.*/
                                                                                           /* Using % to avoid bug in case a new state is unknowingly added. */

            xRecvLen = ipconfigTCP_MSS;
            vTaskPrioritySet( NULL, tcptestECHO_TEST_HIGH_PRIORITY );

            /* Set low priority if requested . */
            if( ( xMode == LARGE_BUFFER_LOW_PRIORITY ) || ( xMode == SMALL_BUFFER_LOW_PRIORITY ) )
            {
                vTaskPrioritySet( NULL, tcptestECHO_TEST_LOW_PRIORITY );
            }

            if( ( xMode == SMALL_BUFFER_HIGH_PRIORITY ) || ( xMode == SMALL_BUFFER_LOW_PRIORITY ) )
            {
                xRecvLen = 1;
            }

            while( xTotalReceived < tcptestTWICE_MAX_FRAME_SIZE )
            {
                xReturned = SOCKETS_Recv( ( Socket_t ) xSocket, ( char * ) pcReceivedString, xRecvLen, 0 );

                TEST_ASSERT_NOT_EQUAL_MESSAGE( 0, xReturned, "Timeout occurred" );
                TEST_ASSERT_GREATER_THAN_MESSAGE( 0, xReturned, "Error occured receiving large message" );

                /* Data was received. */
                TEST_ASSERT_EQUAL_MEMORY( &cTransmittedString[ xTotalReceived ], pcReceivedString, xReturned );

                xTotalReceived += xReturned;
            }

            /* Rendez-vous with the Tx task ready to start a new cycle with a
             * different socket. */
            if( xEventGroupSync( xSyncEventGroup,                             /* The event group used for the rendezvous. */
                                 tcptestRX_TASK_BIT,                          /* The bit representing the Rx task reaching the rendezvous. */
                                 ( tcptestTX_TASK_BIT | tcptestRX_TASK_BIT ), /* Also wait for the Tx task. */
                                 tcptestECHO_TEST_SYNC_TIMEOUT_TICKS ) != ( tcptestTX_TASK_BIT | tcptestRX_TASK_BIT ) )
            {
                TEST_FAIL();
            }

            xResult = prvShutdownHelper( xSocket );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to shutdown" );

            xResult = prvCloseHelper( xSocket, &xSocketOpen );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
        }
    }

    /* Free all dynamic memory. */
    if( xReceivedStringAllocated == pdTRUE )
    {
        vPortFree( ( char * ) pcReceivedString );
    }

    if( xSocketPassingQueueAllocated == pdTRUE )
    {
        vQueueDelete( xSocketPassingQueue );
    }

    if( xSyncEventGroupAllocated == pdTRUE )
    {
        vEventGroupDelete( xSyncEventGroup );
    }

    vTaskDelete( xCreatedTask );

    /* Set priority back. */
    vTaskPrioritySet( NULL, tskIDLE_PRIORITY );
}

TEST( Full_TCP, AFQP_SOCKETS_Threadsafe_SameSocketDifferentTasks )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Threadsafe_SameSocketDifferentTasks( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasks )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvSOCKETS_Threadsafe_SameSocketDifferentTasks( eSecure );
}

/*-----------------------------------------------------------*/

static void prvEchoClientTxTask( void * pvParameters )
{
    BaseType_t xReturned = 0;
    BaseType_t xTransmitted;
    BaseType_t xStatus;
    size_t xLenToSend, xSendLoop;
    size_t xMaxBufferSize;
    tcptestEchoTestModes_t xMode;


    /* Avoid warning about unused parameter. */
    ( void ) pvParameters;

    /* Offset by one so of sending and receiving task are not always the same. Sometimes both low/high or opposite. */

    /*  Recv task                   Sending task
     *  LARGE_BUFFER_HIGH_PRIORITY  SMALL_BUFFER_HIGH_PRIORITY
     *  SMALL_BUFFER_HIGH_PRIORITY  LARGE_BUFFER_LOW_PRIORITY
     *  LARGE_BUFFER_LOW_PRIORITY   SMALL_BUFFER_LOW_PRIORITY
     *  SMALL_BUFFER_LOW_PRIORITY   LARGE_BUFFER_HIGH_PRIORITY
     */
    for( xSendLoop = 1; xSendLoop < tcptestMAX_LOOPS_ECHO_TEST + 1; xSendLoop++ )
    {
        xMode = ( tcptestEchoTestModes_t ) ( xSendLoop % tcptestMAX_ECHO_TEST_MODES ); /* % should be optimized to simple masking since only 4 modes are present.*/
                                                                                       /* Using % to avoid bug in case a new state is unknowingly added. */

        vTaskPrioritySet( NULL, tcptestECHO_TEST_HIGH_PRIORITY );
        xMaxBufferSize = tcptestTWICE_MAX_FRAME_SIZE;

        /* Set low priority if requested . */
        if( ( xMode == LARGE_BUFFER_LOW_PRIORITY ) || ( xMode == SMALL_BUFFER_LOW_PRIORITY ) )
        {
            vTaskPrioritySet( NULL, tcptestECHO_TEST_LOW_PRIORITY );
        }

        /* Set buffer size to 1 if requested. */
        if( ( xMode == SMALL_BUFFER_HIGH_PRIORITY ) || ( xMode == SMALL_BUFFER_LOW_PRIORITY ) )
        {
            xMaxBufferSize = 1;
        }

        /* Wait for the Rx task to create and connect the socket. */
        if( xEventGroupSync( xSyncEventGroup,                             /* The event group used for the rendezvous. */
                             tcptestTX_TASK_BIT,                          /* The bit representing the Tx task reaching the rendezvous. */
                             ( tcptestTX_TASK_BIT | tcptestRX_TASK_BIT ), /* Also wait for the Rx task. */
                             tcptestECHO_TEST_SYNC_TIMEOUT_TICKS ) != ( tcptestTX_TASK_BIT | tcptestRX_TASK_BIT ) )
        {
            break;
        }

        xTransmitted = 0;
        xStatus = pdTRUE;

        /* Keep sending until the entire buffer has been sent. */
        while( xTransmitted < tcptestTWICE_MAX_FRAME_SIZE )
        {
            /* How many bytes are left to send?  Attempt to send them
             * all at once (so the length is potentially greater than the
             * MSS). */
            xLenToSend = tcptestTWICE_MAX_FRAME_SIZE - xTransmitted;

            /* Every loop switch the size of the packet from maximum to smallest. */
            if( xLenToSend > xMaxBufferSize )
            {
                xLenToSend = xMaxBufferSize;
            }

            xReturned = SOCKETS_Send( xSocket,                                            /* The socket being sent to. */
                                      ( void * ) &( cTransmittedString[ xTransmitted ] ), /* The data being sent. */
                                      xLenToSend,                                         /* The length of the data being sent. */
                                      0 );                                                /* ulFlags. */

            if( xReturned >= 0 )
            {
                /* Data was sent successfully. */
                xTransmitted += xReturned;
            }
            else
            {
                xStatus = pdFAIL;
                break;
            }
        }

        /* Wait for the Rx task to recognize the socket is closing and stop
         * using it. */

        if( xEventGroupSync( xSyncEventGroup,                             /* The event group used for the rendezvous. */
                             tcptestTX_TASK_BIT,                          /* The bit representing the Tx task reaching the rendezvous. */
                             ( tcptestTX_TASK_BIT | tcptestRX_TASK_BIT ), /* Also wait for the Rx task. */
                             tcptestECHO_TEST_SYNC_TIMEOUT_TICKS ) != ( tcptestTX_TASK_BIT | tcptestRX_TASK_BIT ) )
        {
            xStatus = pdFAIL;
        }

        if( xStatus == pdFAIL )
        {
            break;
        }
    }

    vTaskSuspend( NULL ); /* Delete this task. */
}

/*-----------------------------------------------------------*/


/** @brief This test will create x tasks that will send and receive to an
 * echo server on different socket at the same time.
 */
void prvStartTCPEchoClientTasks_DifferentSockets( Server_t xConn )
{
    uint16_t usIndex;
    tcptestEchoClientsTaskParams_t xTcptestEchoClientsTaskParams[ tcptestNUM_ECHO_CLIENTS ];
    uint32_t ulEventMask;
    volatile BaseType_t xSyncEventGroupAllocated = pdFALSE;
    BaseType_t xResult;

    if( TEST_PROTECT() )
    {
        /* Create the event group used by the Tx and Rx tasks to synchronize prior
         * to commencing a cycle using a new socket. */
        xSyncEventGroup = xEventGroupCreate();
        configASSERT( xSyncEventGroup );
        xSyncEventGroupAllocated = pdTRUE;

        /* Create the echo client tasks. */
        for( usIndex = 0; usIndex < tcptestNUM_ECHO_CLIENTS; usIndex++ )
        {
            xTcptestEchoClientsTaskParams[ usIndex ].usTaskTag = usIndex;
            xTcptestEchoClientsTaskParams[ usIndex ].xConn = xConn;
            xTcptestEchoClientsTaskParams[ usIndex ].xResult = SOCKETS_SOCKET_ERROR;

            xResult = xTaskCreate( prvThreadSafeDifferentSocketsDifferentTasks,                 /* The function that implements the task. */
                                   "ClientTask",                                                /* Just a text name for the task to aid debugging. */
                                   tcptestTCP_ECHO_TASKS_STACK_SIZE,                            /* The stack size is defined in FreeRTOSIPConfig.h. */
                                   &( xTcptestEchoClientsTaskParams[ usIndex ] ),               /* The task parameter, not used in this case. */
                                   tcptestTCP_ECHO_TASKS_PRIORITY,                              /* The priority assigned to the task is defined in FreeRTOSConfig.h. */
                                   &( xTcptestEchoClientsTaskParams[ usIndex ].xTaskHandle ) ); /* The task handle is not used. */
            TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Task creation failed" );
        }

        ulEventMask = xEventGroupSync( xSyncEventGroup, /* The event group used for the rendezvous. */
                                       0,
                                       tcptestECHO_CLIENT_EVENT_MASK,
                                       tcptestECHO_TEST_SYNC_TIMEOUT_TICKS );

        /* For each task not completed, delete the task.  */
        for( usIndex = 0; usIndex < tcptestNUM_ECHO_CLIENTS; usIndex++ )
        {
            if( ( ulEventMask & ( 1 << usIndex ) ) == 0 )
            {
                vTaskDelete( xTcptestEchoClientsTaskParams[ usIndex ].xTaskHandle );
            }
        }

        for( usIndex = 0; usIndex < tcptestNUM_ECHO_CLIENTS; usIndex++ )
        {
            TEST_ASSERT_EQUAL_MESSAGE( SOCKETS_ERROR_NONE,
                                       xTcptestEchoClientsTaskParams[ usIndex ].xResult,
                                       "Check aws_secure_sockets.h for error code" );
        }
    }

    if( xSyncEventGroupAllocated == pdTRUE )
    {
        vEventGroupDelete( xSyncEventGroup );
    }
}


TEST( Full_TCP, AFQP_SOCKETS_Threadsafe_DifferentSocketsDifferentTasks )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvStartTCPEchoClientTasks_DifferentSockets( eNonsecure );
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_Threadsafe_DifferentSocketsDifferentTasks )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvStartTCPEchoClientTasks_DifferentSockets( eSecure );
}
/*-----------------------------------------------------------*/

static void prvThreadSafeDifferentSocketsDifferentTasks( void * pvParameters )
{
    Socket_t xTaskSocket = SOCKETS_INVALID_SOCKET;
    int32_t lLoopCount = 0UL;
    BaseType_t xReceivedBytes, xReturned;
    BaseType_t xTransmitted;
    char * pcReceivedString;
    BaseType_t xResult = SOCKETS_SOCKET_ERROR;
    BaseType_t xReceivedStringAllocated = pdFALSE;
    BaseType_t xSocketOpen = pdFALSE;
    tcptestEchoClientsTaskParams_t * pxTcptestEchoClientsTaskParams;

    pxTcptestEchoClientsTaskParams = ( ( tcptestEchoClientsTaskParams_t * ) pvParameters );

    pcReceivedString = pvPortMalloc( ipconfigTCP_MSS * sizeof( char ) );

    if( pcReceivedString != NULL )
    {
        xReceivedStringAllocated = pdTRUE;
    }
    else
    {
        xResult = SOCKETS_SOCKET_ERROR;
        tcptestFAILUREPRINTF( ( "%s: Task %d failed to alloc memory \r\n",
                                __FUNCTION__,
                                ( int ) pxTcptestEchoClientsTaskParams->usTaskTag ) );
    }

    if( pcReceivedString != NULL )
    {
        /* Send a number of echo requests. */
        for( lLoopCount = 0; lLoopCount < tcptestMAX_LOOPS_ECHO_CLIENTS_LOOP; lLoopCount++ )
        {
            /* Attempt to establish the requested connection. */
            xResult = prvConnectHelperWithRetry( &xTaskSocket,
                                                 pxTcptestEchoClientsTaskParams->xConn,
                                                 xEchoTestRxTxTimeOut,
                                                 xEchoTestRxTxTimeOut,
                                                 &xSocketOpen );

            if( xResult != SOCKETS_ERROR_NONE )
            {
                tcptestFAILUREPRINTF( ( "%s: Task %d failed to connect with error code %d on loop %d \r\n",
                                        __FUNCTION__,
                                        xResult,
                                        lLoopCount,
                                        ( int ) pxTcptestEchoClientsTaskParams->usTaskTag ) );
                break;
            }

            /* Send the string to the socket. */
            xTransmitted = SOCKETS_Send( xTaskSocket,                   /* The socket being sent to. */
                                         ( void * ) cTransmittedString, /* The data being sent. */
                                         ipconfigTCP_MSS,               /* The length of the data being sent. */
                                         0 );                           /* No flags. */

            if( xTransmitted < ipconfigTCP_MSS )
            {
                tcptestFAILUREPRINTF( ( "%s: Task %d error  %ld while transmitting data\r\n",
                                        __FUNCTION__,
                                        ( int ) pxTcptestEchoClientsTaskParams->usTaskTag,
                                        xTransmitted ) );
                xResult = SOCKETS_SOCKET_ERROR;
                break;
            }

            /* Clear the buffer into which the echoed string will be
             * placed. */
            xReceivedBytes = 0;

            /* Receive data echoed back to the socket. */
            while( xReceivedBytes < xTransmitted )
            {
                xReturned = SOCKETS_Recv( xTaskSocket,                             /* The socket being received from. */
                                          &( pcReceivedString[ xReceivedBytes ] ), /* The buffer into which the received data will be written. */
                                          ipconfigTCP_MSS - xReceivedBytes,        /* The size of the buffer provided to receive the data. */
                                          0 );                                     /* No flags. */

                if( xReturned <= 0 )
                {
                    break;
                }

                xReceivedBytes += xReturned;
            }

            /* If an error occurred it will be latched in xReceivedBytes,
             * otherwise xReceived bytes will be just that - the number of
             * bytes received from the echo server. */
            if( xReceivedBytes == ipconfigTCP_MSS )
            {
                /* Compare the transmitted string to the received string. */
                if( strncmp( pcReceivedString, cTransmittedString, xTransmitted ) != 0 )
                {
                    tcptestFAILUREPRINTF( ( "%s: Task %d error while receiving data \r\n",
                                            __FUNCTION__,
                                            ( int ) pxTcptestEchoClientsTaskParams->usTaskTag ) );
                    xResult = SOCKETS_SOCKET_ERROR;
                    break;
                }
            }
            else
            {
                tcptestFAILUREPRINTF( ( "%s: Task %d error not enough bytes received \r\n",
                                        __FUNCTION__,
                                        ( int ) pxTcptestEchoClientsTaskParams->usTaskTag ) );
                xResult = SOCKETS_SOCKET_ERROR;
                break;
            }

            /* Close this socket before looping back to create another. */
            ( void ) prvShutdownHelper( xTaskSocket );
            ( void ) prvCloseHelper( xTaskSocket, &xSocketOpen );
        }
    }

    /* Free all dynamic memory. */
    if( xReceivedStringAllocated == pdTRUE )
    {
        vPortFree( pcReceivedString );
    }

    if( xSocketOpen == pdTRUE )
    {
        ( void ) prvCloseHelper( xTaskSocket, &xSocketOpen );
    }

    pxTcptestEchoClientsTaskParams->xResult = xResult;

    /* Don't wait, just flag it reached that point. */
    xEventGroupSync( xSyncEventGroup, /* The event group used for the rendezvous. */
                     ( 1 << pxTcptestEchoClientsTaskParams->usTaskTag ),
                     tcptestECHO_CLIENT_EVENT_MASK,
                     tcptestECHO_TEST_SYNC_TIMEOUT_TICKS );


    vTaskDelete( NULL ); /* Delete this task. */
}

/*-----------------------------------------------------------*/

static void prvNonBlockingConnect( void )
{
    BaseType_t xResult = pdFAIL;
    uint32_t ulDummy = 1;

    xSocket = prvTcpSocketHelper( &xSocketOpen );
    TEST_ASSERT_NOT_EQUAL_MESSAGE( SOCKETS_INVALID_SOCKET, xSocket, "Socket creation failed" );

    xResult = SOCKETS_SetSockOpt( xSocket,
                                  0,
                                  SOCKETS_SO_NONBLOCK,
                                  &ulDummy,
                                  sizeof( uint32_t ) );

    /* Nonblocking connect is not supported, setting the option before connecting should return an error. */
    TEST_ASSERT_LESS_THAN_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Port does not support nonblocking connect, but permitted sockopt to be set before connect. \r\n" );

    /* Test teardown will close the socket. */
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_NonBlockingConnect )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvNonBlockingConnect();
}

/*-----------------------------------------------------------*/

static void prvTwoSecureConnections( void )
{
    BaseType_t xResult = pdFAIL;
    uint8_t * pucRxBuffer = ( uint8_t * ) pcRxBuffer;
    uint32_t ulIndex;
    volatile Socket_t xSocketAWS;
    volatile BaseType_t xSocketOpenAWS = pdFALSE;
    volatile Socket_t xSocketSecServer;
    volatile BaseType_t xSocketOpenSecServer = pdFALSE;

    static const char cMessageAWS[] = "Message to AWS!  This message should not be sent to the secure server.";
    static const char cMessageSecServer[] = "Message to Secure Server!  This message should not be sent to AWS.";

    if( TEST_PROTECT() )
    {
        /* Create 2 sockets- one which will connect to AWS Broker, one which will connect to secure server. */

        xResult = prvConnectHelperWithRetry( &xSocketAWS, eAwsBroker, xReceiveTimeOut, xSendTimeOut, &xSocketOpenAWS );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect to AWS Broker" );

        xResult = prvConnectHelperWithRetry( &xSocketSecServer, eSecure, xReceiveTimeOut, xSendTimeOut, &xSocketOpenSecServer );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect to secure server" );

        /* Send message 1x to AWS Broker, 2x to Secure Echo Server, alternating. */
        xResult = prvSendHelper( xSocketSecServer, ( uint8_t * ) cMessageSecServer, sizeof( cMessageSecServer ) );
        TEST_ASSERT_EQUAL_UINT32_MESSAGE( pdPASS, xResult, "Send to secure server failed." );
        xResult = prvSendHelper( xSocketAWS, ( uint8_t * ) cMessageAWS, sizeof( cMessageAWS ) );
        TEST_ASSERT_EQUAL_UINT32_MESSAGE( pdPASS, xResult, "Send to AWS failed." );
        xResult = prvSendHelper( xSocketSecServer, ( uint8_t * ) cMessageSecServer, sizeof( cMessageSecServer ) );
        TEST_ASSERT_EQUAL_UINT32_MESSAGE( pdPASS, xResult, "Send to secure server failed." );

        /* Receive from secure echo server 1x. */
        xResult = prvRecvHelper( xSocketSecServer, pucRxBuffer, sizeof( cMessageSecServer ) );
        TEST_ASSERT_EQUAL_UINT32_MESSAGE( pdPASS, xResult, "Received incorrect number of bytes from sec server." );

        /* Verify the response is correct. */
        for( ulIndex = 0; ulIndex < sizeof( cMessageSecServer ); ulIndex++ )
        {
            if( pucRxBuffer[ ulIndex ] != cMessageSecServer[ ulIndex ] )
            {
                xResult = pdFAIL;
            }
        }

        TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Received incorrect message from secure server. " );

        /* There is no echo from the AWS broker, but let's make sure we don't see anything unexpected. */
        xResult = SOCKETS_Recv( xSocketAWS, pucRxBuffer, sizeof( cMessageAWS ), 0 /* flags. */ );

        if( ( xResult != 0 ) && ( xResult != SOCKETS_EWOULDBLOCK ) ) /* TS-2390 */
        {
            TEST_FAIL_MESSAGE( "Received bytes when there should be none to receive." );
        }

        /* Receive the second message from the secure echo server. */
        xResult = prvRecvHelper( xSocketSecServer, pucRxBuffer, sizeof( cMessageSecServer ) );
        TEST_ASSERT_EQUAL_UINT32_MESSAGE( pdPASS, xResult, "Received incorrect number of bytes from sec server." );

        for( ulIndex = 0; ulIndex < sizeof( cMessageSecServer ); ulIndex++ )
        {
            if( pucRxBuffer[ ulIndex ] != cMessageSecServer[ ulIndex ] )
            {
                xResult = pdFAIL;
            }
        }

        TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xResult, "Received incorrect message from secure server. " );

        /* Shutdown both sockets. */
        xResult = prvShutdownHelper( xSocketAWS );
        TEST_ASSERT_EQUAL_UINT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to shutdown socket." );
        xResult = prvShutdownHelper( xSocketSecServer );
        TEST_ASSERT_EQUAL_UINT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to shutdown socket." );
    }

    /* Close both sockets. */
    if( xSocketOpenAWS == pdTRUE )
    {
        if( TEST_PROTECT() )
        {
            xResult = prvCloseHelper( xSocketAWS, &xSocketOpenAWS );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
        }
    }

    if( xSocketOpenSecServer == pdTRUE )
    {
        if( TEST_PROTECT() )
        {
            xResult = prvCloseHelper( xSocketSecServer, &xSocketOpenSecServer );
            TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
        }
    }
}

TEST( Full_TCP, AFQP_SECURE_SOCKETS_TwoSecureConnections )
{
    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    prvTwoSecureConnections();
}
/*-----------------------------------------------------------*/

TEST( Full_TCP, AFQP_SOCKETS_htons_HappyCase )
{
    uint16_t usNetworkOrderValue;

    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    /* Convert the host order value to network order
     * value. */
    usNetworkOrderValue = SOCKETS_htons( 0x1234 );

    #if defined( socketsconfigBYTE_ORDER ) && ( socketsconfigBYTE_ORDER == pdLITTLE_ENDIAN )

        /* If the platform we are running on, is little
         * endian, bytes must have been swapped. */
        TEST_ASSERT_EQUAL_INT16_MESSAGE( 0x3412, usNetworkOrderValue, "SOCKETS_htons returned incorrect value." );
    #else

        /* If the platform we are running on, is big
         * endian, the output value must be same as
         * the input value. */
        TEST_ASSERT_EQUAL_INT16_MESSAGE( 0x1234, usNetworkOrderValue, "SOCKETS_htons returned incorrect value." );
    #endif

    tcptestPRINTF( ( "%s complete.\r\n", __FUNCTION__ ) );
}
/*-----------------------------------------------------------*/

TEST( Full_TCP, AFQP_SOCKETS_inet_addr_quick_HappyCase )
{
    uint32_t ulPackedIpAddress;

    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    ulPackedIpAddress = SOCKETS_inet_addr_quick( 192, 168, 2, 6 );

    #if defined( socketsconfigBYTE_ORDER ) && ( socketsconfigBYTE_ORDER == pdLITTLE_ENDIAN )

        /* The expected return value of SOCKETS_inet_addr_quick
         * on a little endian platform must be same as the correct
         * hand calculated value. */
        TEST_ASSERT_EQUAL_INT32_MESSAGE( 0x0602A8C0, ulPackedIpAddress, "SOCKETS_inet_addr_quick returned incorrect value." );
    #else

        /* The expected return value of SOCKETS_inet_addr_quick
         * on a big endian platform must be same as the correct
         * hand calculated value. */
        TEST_ASSERT_EQUAL_INT32_MESSAGE( 0xC0A80206, ulPackedIpAddress, "SOCKETS_inet_addr_quick returned incorrect value." );
    #endif

    tcptestPRINTF( ( "%s complete.\r\n", __FUNCTION__ ) );
}
/*-----------------------------------------------------------*/

TEST( Full_TCP, AFQP_SECURE_SOCKETS_SetSecureOptionsAfterConnect )
{
    BaseType_t xResult = pdFAIL;
    char * pcAlpns[] = { socketsAWS_IOT_ALPN_MQTT };

    tcptestPRINTF( ( "Starting %s.\r\n", __FUNCTION__ ) );

    if( TEST_PROTECT() )
    {
        xResult = prvConnectHelperWithRetry( &xSocket, eNonsecure, xReceiveTimeOut, xSendTimeOut, &xSocketOpen );
        TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Failed to connect" );

        /* Try to set the secure option AFTER the connect. */
        xResult = SOCKETS_SetSockOpt( xSocket,
                                      0,
                                      SOCKETS_SO_REQUIRE_TLS,
                                      NULL,
                                      ( size_t ) 0 );
        TEST_ASSERT_LESS_THAN_MESSAGE( SOCKETS_ERROR_NONE, xResult, "TLS setup after connect succeed when fail was expected." );

        /* Try to set ALPN option after connect. */
        xResult = SOCKETS_SetSockOpt( xSocket,
                                      0, /* Level - Unused. */
                                      SOCKETS_SO_ALPN_PROTOCOLS,
                                      pcAlpns,
                                      sizeof( pcAlpns ) / sizeof( pcAlpns[ 0 ] ) );
        TEST_ASSERT_LESS_THAN_MESSAGE( SOCKETS_ERROR_NONE, xResult, "ALPN setup after connect succeed when fail was expected." );


        /* Try to set SNI option after connect. */
        xResult = SOCKETS_SetSockOpt( xSocket,
                                      0, /* Level - Unused. */
                                      SOCKETS_SO_SERVER_NAME_INDICATION,
                                      clientcredentialMQTT_BROKER_ENDPOINT,
                                      sizeof( clientcredentialMQTT_BROKER_ENDPOINT ) );
        TEST_ASSERT_LESS_THAN_MESSAGE( SOCKETS_ERROR_NONE, xResult, "SNI setup after connect succeed when fail was expected." );

        /* Try to set Trusted certificate option after connect. */
        xResult = SOCKETS_SetSockOpt( xSocket,
                                      0,
                                      SOCKETS_SO_TRUSTED_SERVER_CERTIFICATE,
                                      tcptestECHO_HOST_ROOT_CA,
                                      sizeof( tcptestECHO_HOST_ROOT_CA ) );
        TEST_ASSERT_LESS_THAN_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Trusted server certificate setup after connect succeed when fail was expected." );
    }

    xResult = prvShutdownHelper( xSocket );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to shutdown" );

    xResult = prvCloseHelper( xSocket, &xSocketOpen );
    TEST_ASSERT_EQUAL_INT32_MESSAGE( SOCKETS_ERROR_NONE, xResult, "Socket failed to close" );
}

/* TODO: Investigate tests for loopback, other reserved IP addresses */
/* TODO: Implement tests with a bad TCP connection (dropped packets, repeated packets, connection refused etc */
/* TODO: Implement tests that have memory allocation errors (freertos heap is full) */
/*-------------------------------------------------------------------*/
/*-----------------------End Tests-----------------------------------*/
/*-------------------------------------------------------------------*/
