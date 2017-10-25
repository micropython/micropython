/*
    FreeRTOS V8.1.2 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
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
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			        1
#define configUSE_IDLE_HOOK                     1
#define configUSE_TICK_HOOK				        1
#define configCPU_CLOCK_HZ				        ( ( unsigned long ) 80000000 )
#define configTICK_RATE_HZ				        ( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE		        ( ( unsigned short ) 72 )
#define configTOTAL_HEAP_SIZE			        ( ( size_t ) ( \
    16384 /* 16kbytes for FreeRTOS data structures and heap */ \
    - sizeof(StaticTask_t) - configMINIMAL_STACK_SIZE * sizeof(StackType_t) /* TCB+stack for idle task */ \
    - sizeof(StaticTask_t) - 1024 /* TCB+stack for servers task */ \
    - sizeof(StaticTask_t) - 6656 /* TCB+stack for main MicroPython task */ \
    - sizeof(StaticTask_t) - 896 /* TCB+stack for simplelink spawn task */ \
    ) )
#define configMAX_TASK_NAME_LEN			        ( 8 )
#define configUSE_TRACE_FACILITY		        0
#define configUSE_16_BIT_TICKS			        0
#define configIDLE_SHOULD_YIELD			        1
#define configUSE_CO_ROUTINES 			        0
#define configUSE_MUTEXES				        0
#define configUSE_RECURSIVE_MUTEXES		        0
#ifdef DEBUG
#define configCHECK_FOR_STACK_OVERFLOW	        1
#else
#define configCHECK_FOR_STACK_OVERFLOW          0
#endif
#define configUSE_QUEUE_SETS			        0
#define configUSE_COUNTING_SEMAPHORES	        0
#define configUSE_ALTERNATIVE_API		        0

#define configMAX_PRIORITIES			        ( 4UL )
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )
#define configQUEUE_REGISTRY_SIZE		        0

/* Timer related defines. */
#define configUSE_TIMERS				        0
#define configTIMER_TASK_PRIORITY		        2
#define configTIMER_QUEUE_LENGTH		        20
#define configTIMER_TASK_STACK_DEPTH	        ( configMINIMAL_STACK_SIZE * 2 )
#ifdef DEBUG
#define configUSE_MALLOC_FAILED_HOOK            1
#else
#define configUSE_MALLOC_FAILED_HOOK            0
#endif
#define configENABLE_BACKWARD_COMPATIBILITY     0
/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet				0
#define INCLUDE_uxTaskPriorityGet				0
#define INCLUDE_vTaskDelete						0
#define INCLUDE_vTaskCleanUpResources			0
#define INCLUDE_vTaskSuspend					0
#define INCLUDE_vTaskDelayUntil					0
#define INCLUDE_vTaskDelay						1
#ifdef DEBUG
#define INCLUDE_uxTaskGetStackHighWaterMark		1
#else
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#endif
#define INCLUDE_xTaskGetSchedulerState			0
#define INCLUDE_xTimerGetTimerDaemonTaskHandle	0
#ifdef DEBUG
#define INCLUDE_xTaskGetIdleTaskHandle			1
#else
#define INCLUDE_xTaskGetIdleTaskHandle          0
#endif
#define INCLUDE_pcTaskGetTaskName				0
#define INCLUDE_eTaskGetState					0
#define INCLUDE_xSemaphoreGetMutexHolder		0

#define configKERNEL_INTERRUPT_PRIORITY 		( 7 << 5 )	/* Priority 7, or 255 as only the top three bits are implemented.  This is the lowest priority. */
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( 1 << 5 )  /* Priority 5, or 160 as only the top three bits are implemented. */

/* Use the Cortex-M3 optimised task selection rather than the generic C code
version. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1

/* We provide a definition of ucHeap so it can go in a special segment. */
#define configAPPLICATION_ALLOCATED_HEAP 1

/* We use static versions of functions (like xTaskCreateStatic) */
#define configSUPPORT_STATIC_ALLOCATION 1

/* For threading */
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 1
#undef configUSE_MUTEXES
#define configUSE_MUTEXES 1
#undef INCLUDE_vTaskDelete
#define INCLUDE_vTaskDelete 1

#endif /* FREERTOS_CONFIG_H */
