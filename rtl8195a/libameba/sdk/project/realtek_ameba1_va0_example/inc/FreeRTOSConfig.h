/*
    FreeRTOS V7.3.0 - Copyright (C) 2012 Real Time Engineers Ltd.

    FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT 
    http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest versions, license 
    and contact details.  
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__) 
#include <stdint.h> 
extern uint32_t SystemCoreClock; 
#endif 

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

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				1
#define configUSE_TICK_HOOK				0
#define configCPU_CLOCK_HZ				( SystemCoreClock )
#define configTICK_RATE_HZ				( ( uint32_t ) 1000 )
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 70 )
#ifdef CONFIG_UVC
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 110 * 1024 ) )	// use HEAP5
#else
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 60 * 1024 ) )	// use HEAP5
#endif

#ifdef ARDUINO_SDK
#undef  configTOTAL_HEAP_SIZE
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 40 * 1024 ) )	// use HEAP5
#define configTOTAL_SDRHEAP_SIZE		( ( size_t ) ( 60 * 1024 ) )	// use HEAP5
#endif

#define configMAX_TASK_NAME_LEN			( 10 )
#define configUSE_TRACE_FACILITY		0
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			0
#define configUSE_CO_ROUTINES 			1
#define configUSE_MUTEXES				1
#define configUSE_TIMERS                1

#define configMAX_PRIORITIES			( 11 )
#define PRIORITIE_OFFSET				( 4 )

#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

#define configUSE_COUNTING_SEMAPHORES 	1
#define configUSE_ALTERNATIVE_API 		0
#define configCHECK_FOR_STACK_OVERFLOW	2
#define configUSE_RECURSIVE_MUTEXES		1
#define configQUEUE_REGISTRY_SIZE		0
#define configGENERATE_RUN_TIME_STATS	0
#if configGENERATE_RUN_TIME_STATS
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() //( ulHighFrequencyTimerTicks = 0UL )
#define portGET_RUN_TIME_COUNTER_VALUE() xTickCount //ulHighFrequencyTimerTicks
#undef	configUSE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY			1
#define portCONFIGURE_STATS_PEROID_VALUE	1000 //unit Ticks
#endif

#define configTIMER_TASK_PRIORITY       ( 1 )
#define configTIMER_QUEUE_LENGTH        ( 10 )
#define configTIMER_TASK_STACK_DEPTH    ( 512 )     // 512*4=2K

#if (__IASMARM__ != 1)

extern void freertos_pre_sleep_processing(unsigned int *expected_idle_time);
extern int  freertos_ready_to_sleep();

/* Enable tickless power saving. */
#define configUSE_TICKLESS_IDLE                 1

/* In wlan usage, this value is suggested to use value less than 80 milliseconds */
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   2

/* It's magic trick that let us can use our own sleep function */
#define configPRE_SLEEP_PROCESSING( x )         ( freertos_pre_sleep_processing(&x) )

/* It's magic trick that let us can enable/disable tickless dynamically */
#define traceLOW_POWER_IDLE_BEGIN();            do { \
                                                        if (!freertos_ready_to_sleep()) { \
                                                            mtCOVERAGE_TEST_MARKER(); \
                                                            break; \
                                                        }

                                                        // portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime );

#define traceLOW_POWER_IDLE_END();              } while (0);

/* It's FreeRTOS related feature but it's not included in FreeRTOS design. */
#define configUSE_WAKELOCK_PMU                  1

#endif // #if (__IASMARM__ != 1)

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1
#define INCLUDE_pcTaskGetTaskName       1
#define INCLUDE_xTimerPendFunctionCall 	1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif


/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0x0f

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5


/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

//#define RTK_MODE_TIMER


#endif /* FREERTOS_CONFIG_H */
