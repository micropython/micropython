/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * Implementation of the wrapper functions used to raise the processor privilege
 * before calling a standard FreeRTOS API function.
 */

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"
#include "stream_buffer.h"
#include "mpu_prototypes.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/*
 * Calls the port specific code to raise the privilege.
 *
 * Returns pdFALSE if privilege was raised, pdTRUE otherwise.
 */
static BaseType_t xPortRaisePrivilege( void ) SYSTEM_CALL;

/*
 * If xRunningPrivileged is not pdTRUE, calls the port specific
 * code to reset the privilege.
 */
static void vPortResetPrivilege( BaseType_t xRunningPrivileged );
/*-----------------------------------------------------------*/

static BaseType_t xPortRaisePrivilege( void ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged;

	/* Check whether the processor is already privileged. */
	xRunningPrivileged = portIS_PRIVILEGED();

	/* If the processor is not already privileged, raise privilege. */
	if( xRunningPrivileged != pdTRUE )
	{
		portRAISE_PRIVILEGE();
	}

	return xRunningPrivileged;
}
/*-----------------------------------------------------------*/

static void vPortResetPrivilege( BaseType_t xRunningPrivileged )
{
	if( xRunningPrivileged != pdTRUE )
	{
		portRESET_PRIVILEGE();
	}
}
/*-----------------------------------------------------------*/

#if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ))
	BaseType_t MPU_xTaskCreateRestricted( const TaskParameters_t * const pxTaskDefinition, TaskHandle_t *pxCreatedTask ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskCreateRestricted( pxTaskDefinition, pxCreatedTask );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif /* conifgSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	BaseType_t MPU_xTaskCreateRestrictedStatic( const TaskParameters_t * const pxTaskDefinition, TaskHandle_t *pxCreatedTask ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskCreateRestrictedStatic( pxTaskDefinition, pxCreatedTask );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif /* conifgSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	BaseType_t MPU_xTaskCreate( TaskFunction_t pvTaskCode, const char * const pcName, uint16_t usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	TaskHandle_t MPU_xTaskCreateStatic( TaskFunction_t pxTaskCode, const char * const pcName, const uint32_t ulStackDepth, void * const pvParameters, UBaseType_t uxPriority, StackType_t * const puxStackBuffer, StaticTask_t * const pxTaskBuffer ) /* SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskCreateStatic( pxTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif /* configSUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if ( portUSING_MPU_WRAPPERS == 1 )
void MPU_vTaskAllocateMPURegions( TaskHandle_t xTask, const MemoryRegion_t * const xRegions ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	vTaskAllocateMPURegions( xTask, xRegions );
	vPortResetPrivilege( xRunningPrivileged );
}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelete == 1 )
	void MPU_vTaskDelete( TaskHandle_t pxTaskToDelete ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskDelete( pxTaskToDelete );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelayUntil == 1 )
	void MPU_vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, TickType_t xTimeIncrement ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskAbortDelay == 1 )
	BaseType_t MPU_xTaskAbortDelay( TaskHandle_t xTask ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskAbortDelay( xTask );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelay == 1 )
	void MPU_vTaskDelay( TickType_t xTicksToDelay ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskDelay( xTicksToDelay );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskPriorityGet == 1 )
	UBaseType_t MPU_uxTaskPriorityGet( const TaskHandle_t pxTask ) /* SYSTEM_CALL */
	{
	UBaseType_t uxReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		uxReturn = uxTaskPriorityGet( pxTask );
		vPortResetPrivilege( xRunningPrivileged );
		return uxReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskPrioritySet == 1 )
	void MPU_vTaskPrioritySet( TaskHandle_t pxTask, UBaseType_t uxNewPriority ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskPrioritySet( pxTask, uxNewPriority );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_eTaskGetState == 1 )
	eTaskState MPU_eTaskGetState( TaskHandle_t pxTask ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();
	eTaskState eReturn;

		eReturn = eTaskGetState( pxTask );
		vPortResetPrivilege( xRunningPrivileged );
		return eReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TRACE_FACILITY == 1 )
	void MPU_vTaskGetInfo( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskGetInfo( xTask, pxTaskStatus, xGetFreeStackSpace, eState );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
	TaskHandle_t MPU_xTaskGetIdleTaskHandle( void ) /* SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskGetIdleTaskHandle();
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
	void MPU_vTaskSuspend( TaskHandle_t pxTaskToSuspend ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskSuspend( pxTaskToSuspend );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
	void MPU_vTaskResume( TaskHandle_t pxTaskToResume ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskResume( pxTaskToResume );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

void MPU_vTaskSuspendAll( void ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	vTaskSuspendAll();
	vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskResumeAll( void ) /* SYSTEM_CALL */
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xTaskResumeAll();
	vPortResetPrivilege( xRunningPrivileged );
	return xReturn;
}
/*-----------------------------------------------------------*/

TickType_t MPU_xTaskGetTickCount( void ) /* SYSTEM_CALL */
{
TickType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xTaskGetTickCount();
	vPortResetPrivilege( xRunningPrivileged );
	return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxTaskGetNumberOfTasks( void ) /* SYSTEM_CALL */
{
UBaseType_t uxReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	uxReturn = uxTaskGetNumberOfTasks();
	vPortResetPrivilege( xRunningPrivileged );
	return uxReturn;
}
/*-----------------------------------------------------------*/

char * MPU_pcTaskGetName( TaskHandle_t xTaskToQuery ) /* SYSTEM_CALL */
{
char *pcReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	pcReturn = pcTaskGetName( xTaskToQuery );
	vPortResetPrivilege( xRunningPrivileged );
	return pcReturn;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetHandle == 1 )
	TaskHandle_t MPU_xTaskGetHandle( const char *pcNameToQuery ) /* SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskGetHandle( pcNameToQuery );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	void MPU_vTaskList( char *pcWriteBuffer ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskList( pcWriteBuffer );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	void MPU_vTaskGetRunTimeStats( char *pcWriteBuffer ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskGetRunTimeStats( pcWriteBuffer );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
	void MPU_vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxTagValue ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskSetApplicationTaskTag( xTask, pxTagValue );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
	TaskHookFunction_t MPU_xTaskGetApplicationTaskTag( TaskHandle_t xTask ) /* SYSTEM_CALL */
	{
	TaskHookFunction_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskGetApplicationTaskTag( xTask );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
	void MPU_vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTaskSetThreadLocalStoragePointer( xTaskToSet, xIndex, pvValue );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
	void *MPU_pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex ) /* SYSTEM_CALL */
	{
	void *pvReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		pvReturn = pvTaskGetThreadLocalStoragePointer( xTaskToQuery, xIndex );
		vPortResetPrivilege( xRunningPrivileged );
		return pvReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
	BaseType_t MPU_xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskCallApplicationTaskHook( xTask, pvParameter );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )
	UBaseType_t MPU_uxTaskGetSystemState( TaskStatus_t *pxTaskStatusArray, UBaseType_t uxArraySize, uint32_t *pulTotalRunTime ) /* SYSTEM_CALL */
	{
	UBaseType_t uxReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		uxReturn = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, pulTotalRunTime );
		vPortResetPrivilege( xRunningPrivileged );
		return uxReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
	UBaseType_t MPU_uxTaskGetStackHighWaterMark( TaskHandle_t xTask ) /* SYSTEM_CALL */
	{
	UBaseType_t uxReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		uxReturn = uxTaskGetStackHighWaterMark( xTask );
		vPortResetPrivilege( xRunningPrivileged );
		return uxReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetCurrentTaskHandle == 1 )
	TaskHandle_t MPU_xTaskGetCurrentTaskHandle( void ) /* SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskGetCurrentTaskHandle();
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetSchedulerState == 1 )
	BaseType_t MPU_xTaskGetSchedulerState( void ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskGetSchedulerState();
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

void MPU_vTaskSetTimeOutState( TimeOut_t * const pxTimeOut ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	vTaskSetTimeOutState( pxTimeOut );
	vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait ) /* SYSTEM_CALL */
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xTaskCheckForTimeOut( pxTimeOut, pxTicksToWait );
	vPortResetPrivilege( xRunningPrivileged );
	return xReturn;
}
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
	BaseType_t MPU_xTaskGenericNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskGenericNotify( xTaskToNotify, ulValue, eAction, pulPreviousNotificationValue );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
	BaseType_t MPU_xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskNotifyWait( ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
	uint32_t MPU_ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait ) /* SYSTEM_CALL */
	{
	uint32_t ulReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		ulReturn = ulTaskNotifyTake( xClearCountOnExit, xTicksToWait );
		vPortResetPrivilege( xRunningPrivileged );
		return ulReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
	BaseType_t MPU_xTaskNotifyStateClear( TaskHandle_t xTask ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTaskNotifyStateClear( xTask );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	QueueHandle_t MPU_xQueueGenericCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize, uint8_t ucQueueType ) /* SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueGenericCreate( uxQueueLength, uxItemSize, ucQueueType );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	QueueHandle_t MPU_xQueueGenericCreateStatic( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, uint8_t *pucQueueStorage, StaticQueue_t *pxStaticQueue, const uint8_t ucQueueType ) /* SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueGenericCreateStatic( uxQueueLength, uxItemSize, pucQueueStorage, pxStaticQueue, ucQueueType );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericReset( QueueHandle_t pxQueue, BaseType_t xNewQueue ) /* SYSTEM_CALL */
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xQueueGenericReset( pxQueue, xNewQueue );
	vPortResetPrivilege( xRunningPrivileged );
	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, BaseType_t xCopyPosition ) /* SYSTEM_CALL */
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xQueueGenericSend( xQueue, pvItemToQueue, xTicksToWait, xCopyPosition );
	vPortResetPrivilege( xRunningPrivileged );
	return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueMessagesWaiting( const QueueHandle_t pxQueue ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
UBaseType_t uxReturn;

	uxReturn = uxQueueMessagesWaiting( pxQueue );
	vPortResetPrivilege( xRunningPrivileged );
	return uxReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueSpacesAvailable( const QueueHandle_t xQueue ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
UBaseType_t uxReturn;

	uxReturn = uxQueueSpacesAvailable( xQueue );
	vPortResetPrivilege( xRunningPrivileged );
	return uxReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueReceive( QueueHandle_t pxQueue, void * const pvBuffer, TickType_t xTicksToWait ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
BaseType_t xReturn;

	xReturn = xQueueReceive( pxQueue, pvBuffer, xTicksToWait );
	vPortResetPrivilege( xRunningPrivileged );
	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueuePeek( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
BaseType_t xReturn;

	xReturn = xQueuePeek( xQueue, pvBuffer, xTicksToWait );
	vPortResetPrivilege( xRunningPrivileged );
	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueSemaphoreTake( QueueHandle_t xQueue, TickType_t xTicksToWait ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
BaseType_t xReturn;

	xReturn = xQueueSemaphoreTake( xQueue, xTicksToWait );
	vPortResetPrivilege( xRunningPrivileged );
	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueuePeekFromISR( QueueHandle_t pxQueue, void * const pvBuffer ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();
BaseType_t xReturn;

	xReturn = xQueuePeekFromISR( pxQueue, pvBuffer );
	vPortResetPrivilege( xRunningPrivileged );
	return xReturn;
}
/*-----------------------------------------------------------*/

#if ( ( configUSE_MUTEXES == 1 ) && ( INCLUDE_xSemaphoreGetMutexHolder == 1 ) )
    void* MPU_xQueueGetMutexHolder( QueueHandle_t xSemaphore ) /* SYSTEM_CALL */
    {
    BaseType_t xRunningPrivileged = xPortRaisePrivilege();
    void * xReturn;

        xReturn = ( void * ) xQueueGetMutexHolder( xSemaphore );
        vPortResetPrivilege( xRunningPrivileged );
        return xReturn;
    }
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	QueueHandle_t MPU_xQueueCreateMutex( const uint8_t ucQueueType ) /* SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueCreateMutex( ucQueueType );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
	QueueHandle_t MPU_xQueueCreateMutexStatic( const uint8_t ucQueueType, StaticQueue_t *pxStaticQueue ) /* SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueCreateMutexStatic( ucQueueType, pxStaticQueue );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	QueueHandle_t MPU_xQueueCreateCountingSemaphore( UBaseType_t uxCountValue, UBaseType_t uxInitialCount ) /* SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueCreateCountingSemaphore( uxCountValue, uxInitialCount );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )

	QueueHandle_t MPU_xQueueCreateCountingSemaphoreStatic( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount, StaticQueue_t *pxStaticQueue ) /* SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueCreateCountingSemaphoreStatic( uxMaxCount, uxInitialCount, pxStaticQueue );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )
	BaseType_t MPU_xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xBlockTime ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueTakeMutexRecursive( xMutex, xBlockTime );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )
	BaseType_t MPU_xQueueGiveMutexRecursive( QueueHandle_t xMutex ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueGiveMutexRecursive( xMutex );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_QUEUE_SETS == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	QueueSetHandle_t MPU_xQueueCreateSet( UBaseType_t uxEventQueueLength ) /* SYSTEM_CALL */
	{
	QueueSetHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueCreateSet( uxEventQueueLength );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
	QueueSetMemberHandle_t MPU_xQueueSelectFromSet( QueueSetHandle_t xQueueSet, TickType_t xBlockTimeTicks ) /* SYSTEM_CALL */
	{
	QueueSetMemberHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueSelectFromSet( xQueueSet, xBlockTimeTicks );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
	BaseType_t MPU_xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueAddToSet( xQueueOrSemaphore, xQueueSet );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
	BaseType_t MPU_xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xQueueRemoveFromSet( xQueueOrSemaphore, xQueueSet );
		vPortResetPrivilege( xRunningPrivileged );
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
	void MPU_vQueueAddToRegistry( QueueHandle_t xQueue, const char *pcName ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vQueueAddToRegistry( xQueue, pcName );

		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
	void MPU_vQueueUnregisterQueue( QueueHandle_t xQueue ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vQueueUnregisterQueue( xQueue );

		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
	const char *MPU_pcQueueGetName( QueueHandle_t xQueue ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();
	const char *pcReturn;

		pcReturn = pcQueueGetName( xQueue );

		vPortResetPrivilege( xRunningPrivileged );
		return pcReturn;
	}
#endif
/*-----------------------------------------------------------*/

void MPU_vQueueDelete( QueueHandle_t xQueue ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	vQueueDelete( xQueue );

	vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	void *MPU_pvPortMalloc( size_t xSize ) /* SYSTEM_CALL */
	{
	void *pvReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		pvReturn = pvPortMalloc( xSize );

		vPortResetPrivilege( xRunningPrivileged );

		return pvReturn;
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	void MPU_vPortFree( void *pv ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vPortFree( pv );

		vPortResetPrivilege( xRunningPrivileged );
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	void MPU_vPortInitialiseBlocks( void ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vPortInitialiseBlocks();

		vPortResetPrivilege( xRunningPrivileged );
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	size_t MPU_xPortGetFreeHeapSize( void ) /* SYSTEM_CALL */
	{
	size_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xPortGetFreeHeapSize();

		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
	TimerHandle_t MPU_xTimerCreate( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction ) /* SYSTEM_CALL */
	{
	TimerHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
	TimerHandle_t MPU_xTimerCreateStatic( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction, StaticTimer_t *pxTimerBuffer ) /* SYSTEM_CALL */
	{
	TimerHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTimerCreateStatic( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction, pxTimerBuffer );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	void *MPU_pvTimerGetTimerID( const TimerHandle_t xTimer ) /* SYSTEM_CALL */
	{
	void * pvReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		pvReturn = pvTimerGetTimerID( xTimer );
		vPortResetPrivilege( xRunningPrivileged );

		return pvReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	void MPU_vTimerSetTimerID( TimerHandle_t xTimer, void *pvNewID ) /* SYSTEM_CALL */
	{
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		vTimerSetTimerID( xTimer, pvNewID );
		vPortResetPrivilege( xRunningPrivileged );
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	BaseType_t MPU_xTimerIsTimerActive( TimerHandle_t xTimer ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTimerIsTimerActive( xTimer );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	TaskHandle_t MPU_xTimerGetTimerDaemonTaskHandle( void ) /* SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTimerGetTimerDaemonTaskHandle();
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( INCLUDE_xTimerPendFunctionCall == 1 ) && ( configUSE_TIMERS == 1 ) )
	BaseType_t MPU_xTimerPendFunctionCall( PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, TickType_t xTicksToWait ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTimerPendFunctionCall( xFunctionToPend, pvParameter1, ulParameter2, xTicksToWait );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	const char * MPU_pcTimerGetName( TimerHandle_t xTimer ) /* SYSTEM_CALL */
	{
	const char * pcReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		pcReturn = pcTimerGetName( xTimer );
		vPortResetPrivilege( xRunningPrivileged );

		return pcReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	TickType_t MPU_xTimerGetPeriod( TimerHandle_t xTimer ) /* SYSTEM_CALL */
	{
	TickType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTimerGetPeriod( xTimer );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	TickType_t MPU_xTimerGetExpiryTime( TimerHandle_t xTimer ) /* SYSTEM_CALL */
	{
	TickType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTimerGetExpiryTime( xTimer );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	BaseType_t MPU_xTimerGenericCommand( TimerHandle_t xTimer, const BaseType_t xCommandID, const TickType_t xOptionalValue, BaseType_t * const pxHigherPriorityTaskWoken, const TickType_t xTicksToWait ) /* SYSTEM_CALL */
	{
	BaseType_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xTimerGenericCommand( xTimer, xCommandID, xOptionalValue, pxHigherPriorityTaskWoken, xTicksToWait );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	EventGroupHandle_t MPU_xEventGroupCreate( void ) /* SYSTEM_CALL */
	{
	EventGroupHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xEventGroupCreate();
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	EventGroupHandle_t MPU_xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer ) /* SYSTEM_CALL */
	{
	EventGroupHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xEventGroupCreateStatic( pxEventGroupBuffer );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait ) /* SYSTEM_CALL */
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xEventGroupWaitBits( xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear ) /* SYSTEM_CALL */
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xEventGroupClearBits( xEventGroup, uxBitsToClear );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet ) /* SYSTEM_CALL */
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xEventGroupSetBits( xEventGroup, uxBitsToSet );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSync( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait ) /* SYSTEM_CALL */
{
EventBits_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xEventGroupSync( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTicksToWait );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

void MPU_vEventGroupDelete( EventGroupHandle_t xEventGroup ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	vEventGroupDelete( xEventGroup );
	vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferSend( StreamBufferHandle_t xStreamBuffer, const void *pvTxData, size_t xDataLengthBytes, TickType_t xTicksToWait ) /* SYSTEM_CALL */
{
size_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferSend( xStreamBuffer, pvTxData, xDataLengthBytes, xTicksToWait );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferSendFromISR( StreamBufferHandle_t xStreamBuffer, const void *pvTxData, size_t xDataLengthBytes, BaseType_t * const pxHigherPriorityTaskWoken ) /* SYSTEM_CALL */
{
size_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferSendFromISR( xStreamBuffer, pvTxData, xDataLengthBytes, pxHigherPriorityTaskWoken );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferNextMessageLengthBytes( StreamBufferHandle_t xStreamBuffer )
{
size_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferNextMessageLengthBytes( xStreamBuffer );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferReceive( StreamBufferHandle_t xStreamBuffer, void *pvRxData, size_t xBufferLengthBytes, TickType_t xTicksToWait ) /* SYSTEM_CALL */
{
size_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferReceive( xStreamBuffer, pvRxData, xBufferLengthBytes, xTicksToWait );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferReceiveFromISR( StreamBufferHandle_t xStreamBuffer, void *pvRxData, size_t xBufferLengthBytes, BaseType_t * const pxHigherPriorityTaskWoken ) /* SYSTEM_CALL */
{
size_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferReceiveFromISR( xStreamBuffer, pvRxData, xBufferLengthBytes, pxHigherPriorityTaskWoken );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

void MPU_vStreamBufferDelete( StreamBufferHandle_t xStreamBuffer ) /* SYSTEM_CALL */
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	vStreamBufferDelete( xStreamBuffer );
	vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferIsFull( StreamBufferHandle_t xStreamBuffer ) /* SYSTEM_CALL */
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferIsFull( xStreamBuffer );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferIsEmpty( StreamBufferHandle_t xStreamBuffer ) /* SYSTEM_CALL */
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferIsEmpty( xStreamBuffer );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferReset( StreamBufferHandle_t xStreamBuffer ) /* SYSTEM_CALL */
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferReset( xStreamBuffer );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferSpacesAvailable( StreamBufferHandle_t xStreamBuffer ) /* SYSTEM_CALL */
{
size_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferSpacesAvailable( xStreamBuffer );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferBytesAvailable( StreamBufferHandle_t xStreamBuffer ) /* SYSTEM_CALL */
{
size_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferBytesAvailable( xStreamBuffer );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferSetTriggerLevel( StreamBufferHandle_t xStreamBuffer, size_t xTriggerLevel ) /* SYSTEM_CALL */
{
BaseType_t xReturn;
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	xReturn = xStreamBufferSetTriggerLevel( xStreamBuffer, xTriggerLevel );
	vPortResetPrivilege( xRunningPrivileged );

	return xReturn;
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	StreamBufferHandle_t MPU_xStreamBufferGenericCreate( size_t xBufferSizeBytes, size_t xTriggerLevelBytes, BaseType_t xIsMessageBuffer ) /* SYSTEM_CALL */
	{
	StreamBufferHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xStreamBufferGenericCreate( xBufferSizeBytes, xTriggerLevelBytes, xIsMessageBuffer );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	StreamBufferHandle_t MPU_xStreamBufferGenericCreateStatic( size_t xBufferSizeBytes, size_t xTriggerLevelBytes, BaseType_t xIsMessageBuffer, uint8_t * const pucStreamBufferStorageArea, StaticStreamBuffer_t * const pxStaticStreamBuffer ) /* SYSTEM_CALL */
	{
	StreamBufferHandle_t xReturn;
	BaseType_t xRunningPrivileged = xPortRaisePrivilege();

		xReturn = xStreamBufferGenericCreateStatic( xBufferSizeBytes, xTriggerLevelBytes, xIsMessageBuffer, pucStreamBufferStorageArea, pxStaticStreamBuffer );
		vPortResetPrivilege( xRunningPrivileged );

		return xReturn;
	}
#endif /* configSUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/


/* Functions that the application writer wants to execute in privileged mode
can be defined in application_defined_privileged_functions.h.  The functions
must take the same format as those above whereby the privilege state on exit
equals the privilege state on entry.  For example:

void MPU_FunctionName( [parameters ] )
{
BaseType_t xRunningPrivileged = xPortRaisePrivilege();

	FunctionName( [parameters ] );

	vPortResetPrivilege( xRunningPrivileged );
}
*/

#if configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS == 1
	#include "application_defined_privileged_functions.h"
#endif
