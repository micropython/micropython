/*
 * FreeRTOS Kernel V10.0.1
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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
* Application specific definitions.
*
* These definitions should be adjusted for your particular hardware and
* application requirements.
*
* THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
* FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
* http://www.freertos.org/a00110.html
*
* The bottom of this file contains some constants specific to running the UDP
* stack in this demo.  Constants specific to FreeRTOS+TCP itself (rather than
* the demo) are contained in FreeRTOSIPConfig.h.
*----------------------------------------------------------*/

#define configMAX_API_CALL_INTERRUPT_PRIORITY	18

#define configUSE_DAEMON_TASK_STARTUP_HOOK         1
#define configENABLE_BACKWARD_COMPATIBILITY        0
#define configUSE_PREEMPTION                       1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
#define configUSE_TICKLESS_IDLE					0
#define configMAX_PRIORITIES                       ( 7 )
#define configTICK_RATE_HZ                         ( 1000 )
#define configMINIMAL_STACK_SIZE                   ( ( unsigned short ) 200 )
#define configTOTAL_HEAP_SIZE                      ( ( size_t ) ( 4 * 1024 * 1024 ) )
#define configMAX_TASK_NAME_LEN                    ( 15 )
#define configUSE_TRACE_FACILITY                   1
#define configUSE_16_BIT_TICKS                     0
#define configIDLE_SHOULD_YIELD                    1
#define configUSE_CO_ROUTINES                      0
#define configUSE_MUTEXES                          1
#define configUSE_RECURSIVE_MUTEXES                1
#define configQUEUE_REGISTRY_SIZE                  8
#define configUSE_APPLICATION_TASK_TAG             0
#define configUSE_COUNTING_SEMAPHORES              1
#define configUSE_QUEUE_SETS					1
#define configUSE_ALTERNATIVE_API                  0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS    3      /* FreeRTOS+FAT requires 2 pointers if a CWD is supported. */
#define configRECORD_STACK_HIGH_ADDRESS            1

/* Hook function related definitions. */
#define configUSE_TICK_HOOK                        0
#define configUSE_IDLE_HOOK                        0
#define configUSE_MALLOC_FAILED_HOOK               1
#define configCHECK_FOR_STACK_OVERFLOW             2

/* Software timer related definitions. */
#define configUSE_TIMERS                           1
#define configTIMER_TASK_PRIORITY                  ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                   5
#define configTIMER_TASK_STACK_DEPTH               ( configMINIMAL_STACK_SIZE * 2 )

/* Event group related definitions. */
#define configUSE_EVENT_GROUPS                     1

/* Run time stats gathering definitions. */
/* FIX ME: Uncomment if you plan to use Tracealyzer.
*/
#define configUSE_STATS_FORMATTING_FUNCTIONS	1
/*
#define configGENERATE_RUN_TIME_STATS 0
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()
*/

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )

/* Currently the TCP/IP stack is using dynamic allocation, and the MQTT task is
 * using static allocation. */
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         1

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
    /* __NVIC_PRIO_BITS will be specified when CMSIS is being used. */
    #define configPRIO_BITS    __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS    3                                 /* 8 priority levels. */
#endif

/* This demo makes use of one or more example stats formatting functions.  These
 * format the raw data provided by the uxTaskGetSystemState() function in to human
 * readable ASCII form.  See the notes in the implementation of vTaskList() within
 * FreeRTOS/Source/tasks.c for limitations.  configUSE_STATS_FORMATTING_FUNCTIONS
 * is set to 2 so the formatting functions are included without the stdio.h being
 * included in tasks.c.  That is because this project defines its own sprintf()
 * functions. */
#define configUSE_STATS_FORMATTING_FUNCTIONS    1

/* Assert call defined for debug builds. */
void vAssertCalled( const char * pcFile,
                    uint32_t ulLine );

#define configASSERT( x )    if( ( x ) == 0 )  vAssertCalled(__FILE__, __LINE__)

/* Sets the length of the buffers into which logging messages are written - so
 * also defines the maximum length of each log message. */ 
#define configLOGGING_MAX_MESSAGE_LENGTH            100

/* Set to 1 to prepend each log message with a message number, the task name,
 * and a time stamp. */
#define configLOGGING_INCLUDE_TIME_AND_TASK_NAME    1

/* The priority at which the tick interrupt runs.  This should probably be kept at 1. */
#define configKERNEL_INTERRUPT_PRIORITY             1

/****** Hardware specific settings. *******************************************/

/*
 * The application must provide a function that configures a peripheral to
 * create the FreeRTOS tick interrupt, then define configSETUP_TICK_INTERRUPT()
 * in FreeRTOSConfig.h to call the function.  This file contains a function
 * that is suitable for use on the Zynq MPU.  FreeRTOS_Tick_Handler() must
 * be installed as the peripheral's interrupt handler.
 */
void vConfigureTickInterrupt( void );
#define configSETUP_TICK_INTERRUPT() vConfigureTickInterrupt()

void vClearTickInterrupt( void );
#define configCLEAR_TICK_INTERRUPT() vClearTickInterrupt()

/* The following constant describe the hardware, and are correct for the
Zynq MPU. */
#define configINTERRUPT_CONTROLLER_BASE_ADDRESS 		( XPAR_PS7_SCUGIC_0_DIST_BASEADDR )
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET ( -0xf00 )
#define configUNIQUE_INTERRUPT_PRIORITIES				32

/* Application specific definitions follow. **********************************/

/* If configINCLUDE_DEMO_DEBUG_STATS is set to one, then a few basic IP trace
 * macros are defined to gather some UDP stack statistics that can then be viewed
 * through the CLI interface. */
#define configINCLUDE_DEMO_DEBUG_STATS       1

/* The size of the global output buffer that is available for use when there
 * are multiple command interpreters running at once (for example, one on a UART
 * and one on TCP/IP).  This is done to prevent an output buffer being defined by
 * each implementation - which would waste RAM.  In this case, there is only one
 * command interpreter running, and it has its own local output buffer, so the
 * global buffer is just set to be one byte long as it is not used and should not
 * take up unnecessary RAM. */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE    1

/* The UDP port to which print messages are sent. */
#define configPRINT_PORT                     ( 15000 )

#define configPROFILING                      ( 0 )

/* Pseudo random number generater used by some demo tasks. */
extern uint32_t uxRand();
#define configRAND32()    uxRand()

#endif /* FREERTOS_CONFIG_H */
