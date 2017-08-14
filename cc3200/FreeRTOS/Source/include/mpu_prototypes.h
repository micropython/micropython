/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * When the MPU is used the standard (non MPU) API functions are mapped to
 * equivalents that start "MPU_", the prototypes for which are defined in this
 * header files.  This will cause the application code to call the MPU_ version
 * which wraps the non-MPU version with privilege promoting then demoting code,
 * so the kernel code always runs will full privileges.
 */


#ifndef MPU_PROTOTYPES_H
#define MPU_PROTOTYPES_H

/* MPU versions of tasks.h API function. */
BaseType_t MPU_xTaskCreate( TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask );
TaskHandle_t MPU_xTaskCreateStatic( TaskFunction_t pxTaskCode, const char * const pcName, const uint32_t ulStackDepth, void * const pvParameters, UBaseType_t uxPriority, StackType_t * const puxStackBuffer, StaticTask_t * const pxTaskBuffer );
BaseType_t MPU_xTaskCreateRestricted( const TaskParameters_t * const pxTaskDefinition, TaskHandle_t *pxCreatedTask );
void MPU_vTaskAllocateMPURegions( TaskHandle_t xTask, const MemoryRegion_t * const pxRegions );
void MPU_vTaskDelete( TaskHandle_t xTaskToDelete );
void MPU_vTaskDelay( const TickType_t xTicksToDelay );
void MPU_vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement );
BaseType_t MPU_xTaskAbortDelay( TaskHandle_t xTask );
UBaseType_t MPU_uxTaskPriorityGet( TaskHandle_t xTask );
eTaskState MPU_eTaskGetState( TaskHandle_t xTask );
void MPU_vTaskGetInfo( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState );
void MPU_vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority );
void MPU_vTaskSuspend( TaskHandle_t xTaskToSuspend );
void MPU_vTaskResume( TaskHandle_t xTaskToResume );
void MPU_vTaskStartScheduler( void );
void MPU_vTaskSuspendAll( void );
BaseType_t MPU_xTaskResumeAll( void );
TickType_t MPU_xTaskGetTickCount( void );
UBaseType_t MPU_uxTaskGetNumberOfTasks( void );
char * MPU_pcTaskGetName( TaskHandle_t xTaskToQuery );
TaskHandle_t MPU_xTaskGetHandle( const char *pcNameToQuery );
UBaseType_t MPU_uxTaskGetStackHighWaterMark( TaskHandle_t xTask );
void MPU_vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxHookFunction );
TaskHookFunction_t MPU_xTaskGetApplicationTaskTag( TaskHandle_t xTask );
void MPU_vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue );
void * MPU_pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex );
BaseType_t MPU_xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter );
TaskHandle_t MPU_xTaskGetIdleTaskHandle( void );
UBaseType_t MPU_uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray, const UBaseType_t uxArraySize, uint32_t * const pulTotalRunTime );
void MPU_vTaskList( char * pcWriteBuffer );
void MPU_vTaskGetRunTimeStats( char *pcWriteBuffer );
BaseType_t MPU_xTaskGenericNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue );
BaseType_t MPU_xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait );
uint32_t MPU_ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait );
BaseType_t MPU_xTaskNotifyStateClear( TaskHandle_t xTask );
BaseType_t MPU_xTaskIncrementTick( void );
TaskHandle_t MPU_xTaskGetCurrentTaskHandle( void );
void MPU_vTaskSetTimeOutState( TimeOut_t * const pxTimeOut );
BaseType_t MPU_xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait );
void MPU_vTaskMissedYield( void );
BaseType_t MPU_xTaskGetSchedulerState( void );

/* MPU versions of queue.h API function. */
BaseType_t MPU_xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition );
BaseType_t MPU_xQueueGenericReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeek );
UBaseType_t MPU_uxQueueMessagesWaiting( const QueueHandle_t xQueue );
UBaseType_t MPU_uxQueueSpacesAvailable( const QueueHandle_t xQueue );
void MPU_vQueueDelete( QueueHandle_t xQueue );
QueueHandle_t MPU_xQueueCreateMutex( const uint8_t ucQueueType );
QueueHandle_t MPU_xQueueCreateMutexStatic( const uint8_t ucQueueType, StaticQueue_t *pxStaticQueue );
QueueHandle_t MPU_xQueueCreateCountingSemaphore( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount );
QueueHandle_t MPU_xQueueCreateCountingSemaphoreStatic( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount, StaticQueue_t *pxStaticQueue );
void* MPU_xQueueGetMutexHolder( QueueHandle_t xSemaphore );
BaseType_t MPU_xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xTicksToWait );
BaseType_t MPU_xQueueGiveMutexRecursive( QueueHandle_t pxMutex );
void MPU_vQueueAddToRegistry( QueueHandle_t xQueue, const char *pcName );
void MPU_vQueueUnregisterQueue( QueueHandle_t xQueue );
const char * MPU_pcQueueGetName( QueueHandle_t xQueue );
QueueHandle_t MPU_xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType );
QueueHandle_t MPU_xQueueGenericCreateStatic( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, uint8_t *pucQueueStorage, StaticQueue_t *pxStaticQueue, const uint8_t ucQueueType );
QueueSetHandle_t MPU_xQueueCreateSet( const UBaseType_t uxEventQueueLength );
BaseType_t MPU_xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet );
BaseType_t MPU_xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet );
QueueSetMemberHandle_t MPU_xQueueSelectFromSet( QueueSetHandle_t xQueueSet, const TickType_t xTicksToWait );
BaseType_t MPU_xQueueGenericReset( QueueHandle_t xQueue, BaseType_t xNewQueue );
void MPU_vQueueSetQueueNumber( QueueHandle_t xQueue, UBaseType_t uxQueueNumber );
UBaseType_t MPU_uxQueueGetQueueNumber( QueueHandle_t xQueue );
uint8_t MPU_ucQueueGetQueueType( QueueHandle_t xQueue );

/* MPU versions of timers.h API function. */
TimerHandle_t MPU_xTimerCreate(	const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction );
TimerHandle_t MPU_xTimerCreateStatic(	const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction, StaticTimer_t *pxTimerBuffer );
void * MPU_pvTimerGetTimerID( const TimerHandle_t xTimer );
void MPU_vTimerSetTimerID( TimerHandle_t xTimer, void *pvNewID );
BaseType_t MPU_xTimerIsTimerActive( TimerHandle_t xTimer );
TaskHandle_t MPU_xTimerGetTimerDaemonTaskHandle( void );
BaseType_t MPU_xTimerPendFunctionCall( PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, TickType_t xTicksToWait );
const char * MPU_pcTimerGetName( TimerHandle_t xTimer );
TickType_t MPU_xTimerGetPeriod( TimerHandle_t xTimer );
TickType_t MPU_xTimerGetExpiryTime( TimerHandle_t xTimer );
BaseType_t MPU_xTimerCreateTimerTask( void );
BaseType_t MPU_xTimerGenericCommand( TimerHandle_t xTimer, const BaseType_t xCommandID, const TickType_t xOptionalValue, BaseType_t * const pxHigherPriorityTaskWoken, const TickType_t xTicksToWait );

/* MPU versions of event_group.h API function. */
EventGroupHandle_t MPU_xEventGroupCreate( void );
EventGroupHandle_t MPU_xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer );
EventBits_t MPU_xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait );
EventBits_t MPU_xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear );
EventBits_t MPU_xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet );
EventBits_t MPU_xEventGroupSync( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait );
void MPU_vEventGroupDelete( EventGroupHandle_t xEventGroup );
UBaseType_t MPU_uxEventGroupGetNumber( void* xEventGroup );

#endif /* MPU_PROTOTYPES_H */

