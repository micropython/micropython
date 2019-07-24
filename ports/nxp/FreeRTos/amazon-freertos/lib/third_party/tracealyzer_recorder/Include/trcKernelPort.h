/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * Terms of Use
 * This file is part of the trace recorder library (RECORDER), which is the 
 * intellectual property of Percepio AB (PERCEPIO) and provided under a
 * license as follows.
 * The RECORDER may be used free of charge for the purpose of recording data
 * intended for analysis in PERCEPIO products. It may not be used or modified
 * for other purposes without explicit permission from PERCEPIO.
 * You may distribute the RECORDER in its original source code form, assuming
 * this text (terms of use, disclaimer, copyright notice) is unchanged. You are
 * allowed to distribute the RECORDER with minor modifications intended for
 * configuration or porting of the RECORDER, e.g., to allow using it on a 
 * specific processor, processor family or with a specific communication
 * interface. Any such modifications should be documented directly below
 * this comment block.  
 *
 * Disclaimer
 * The RECORDER is being delivered to you AS IS and PERCEPIO makes no warranty
 * as to its use or performance. PERCEPIO does not and cannot warrant the 
 * performance or results you may obtain by using the RECORDER or documentation.
 * PERCEPIO make no warranties, express or implied, as to noninfringement of
 * third party rights, merchantability, or fitness for any particular purpose.
 * In no event will PERCEPIO, its technology partners, or distributors be liable
 * to you for any consequential, incidental or special damages, including any
 * lost profits or lost savings, even if a representative of PERCEPIO has been
 * advised of the possibility of such damages, or for any claim by any third
 * party. Some jurisdictions do not allow the exclusion or limitation of
 * incidental, consequential or special damages, or the exclusion of implied
 * warranties or limitations on how long an implied warranty may last, so the
 * above limitations may not apply to you.
 *
 * FreeRTOS-specific definitions needed by the trace recorder
 *
 * <LICENSE INFO>
 *
 * Tabs are used for indent in this file (1 tab = 4 spaces)
 *
 * Copyright Percepio AB, 2017.
 * www.percepio.com
 ******************************************************************************/

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include "FreeRTOS.h"	/* Defines configUSE_TRACE_FACILITY */
#include "trcPortDefines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_TRACEALYZER_RECORDER configUSE_TRACE_FACILITY

/*** FreeRTOS version codes **************************************************/
#define FREERTOS_VERSION_NOT_SET				0
#define TRC_FREERTOS_VERSION_7_3_OR_7_4			1
#define TRC_FREERTOS_VERSION_7_5_OR_7_6			2
#define TRC_FREERTOS_VERSION_8_X				3
#define TRC_FREERTOS_VERSION_9_X				4

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

/*******************************************************************************
 * INCLUDE_xTaskGetCurrentTaskHandle must be set to 1 for tracing to work properly
 ******************************************************************************/
#undef INCLUDE_xTaskGetCurrentTaskHandle
#define INCLUDE_xTaskGetCurrentTaskHandle 1

/*******************************************************************************
 * vTraceSetQueueName(void* object, const char* name)
 *
 * Parameter object: pointer to the Queue that shall be named
 * Parameter name: the name to set (const string literal)
 *
 * Sets a name for Queue objects for display in Tracealyzer.
 ******************************************************************************/
void vTraceSetQueueName(void* object, const char* name);

/*******************************************************************************
 * vTraceSetSemaphoreName(void* object, const char* name)
 *
 * Parameter object: pointer to the Semaphore that shall be named
 * Parameter name: the name to set (const string literal)
 *
 * Sets a name for Semaphore objects for display in Tracealyzer.
 ******************************************************************************/
void vTraceSetSemaphoreName(void* object, const char* name);

/*******************************************************************************
 * vTraceSetMutexName(void* object, const char* name)
 *
 * Parameter object: pointer to the Mutex that shall be named
 * Parameter name: the name to set (const string literal)
 *
 * Sets a name for Semaphore objects for display in Tracealyzer.
 ******************************************************************************/
void vTraceSetMutexName(void* object, const char* name);

/*******************************************************************************
 * Note: Setting names for event groups is difficult to support, this has been 
 * excluded intentionally. This since we don't know if event_groups.c is 
 * included in the build, so referencing it from the recorder may cause errors.
 ******************************************************************************/

/* Gives the currently executing task (wrapper for RTOS-specific function) */
void* prvTraceGetCurrentTaskHandle(void);

#if (((TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT) && (TRC_CFG_INCLUDE_ISR_TRACING == 1)) || (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING))
/* Tells if the scheduler currently is suspended (task-switches can't occur) */
unsigned char prvTraceIsSchedulerSuspended(void);

/*******************************************************************************
 * INCLUDE_xTaskGetSchedulerState must be set to 1 for tracing to work properly
 ******************************************************************************/
#undef INCLUDE_xTaskGetSchedulerState
#define INCLUDE_xTaskGetSchedulerState 1

#endif /* (((TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT) && (TRC_CFG_INCLUDE_ISR_TRACING == 1)) || (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)) */

#define TRACE_KERNEL_VERSION 0x1AA1
#define TRACE_TICK_RATE_HZ configTICK_RATE_HZ /* Defined in "FreeRTOS.h" */
#define TRACE_CPU_CLOCK_HZ configCPU_CLOCK_HZ /* Defined in "FreeRTOSConfig.h" */
#define TRACE_GET_CURRENT_TASK() prvTraceGetCurrentTaskHandle()

#define TRACE_GET_OS_TICKS() (uiTraceTickCount) /* Streaming only */

/* If using dynamic allocation of snapshot trace buffer... */
#define TRACE_MALLOC(size) pvPortMalloc(size) 	

#ifdef configUSE_TIMERS
#if (configUSE_TIMERS == 1)
#undef INCLUDE_xTimerGetTimerDaemonTaskHandle
#define INCLUDE_xTimerGetTimerDaemonTaskHandle 1
#endif /* configUSE_TIMERS == 1*/
#endif /* configUSE_TIMERS */

/* For ARM Cortex-M devices - assumes the ARM CMSIS API is available */
#if (defined (__CORTEX_M))	
	#define TRACE_ALLOC_CRITICAL_SECTION() uint32_t __irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() {__irq_status = __get_PRIMASK(); __set_PRIMASK(1);} /* PRIMASK disables ALL interrupts - allows for tracing in any ISR */
	#define TRACE_EXIT_CRITICAL_SECTION() {__set_PRIMASK(__irq_status);}
#endif

#if ((TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_CORTEX_A9) || (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Renesas_RX600) || (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_MICROCHIP_PIC24_PIC32))
	#define TRACE_ALLOC_CRITICAL_SECTION() int __irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() {__irq_status = portSET_INTERRUPT_MASK_FROM_ISR();}
	#define TRACE_EXIT_CRITICAL_SECTION() {portCLEAR_INTERRUPT_MASK_FROM_ISR(__irq_status);}
#endif

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Win32)
    /* In the Win32 port, there are no real interrupts, so we can use the normal critical sections */
	#define TRACE_ALLOC_CRITICAL_SECTION()
	#define TRACE_ENTER_CRITICAL_SECTION() portENTER_CRITICAL()
	#define TRACE_EXIT_CRITICAL_SECTION() portEXIT_CRITICAL()
#endif

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_POWERPC_Z4)
#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X)
    /* FreeRTOS v8.0 or later */    
	#define TRACE_ALLOC_CRITICAL_SECTION() UBaseType_t __irq_status;
    #define TRACE_ENTER_CRITICAL_SECTION() {__irq_status = portSET_INTERRUPT_MASK_FROM_ISR();}
    #define TRACE_EXIT_CRITICAL_SECTION() {portCLEAR_INTERRUPT_MASK_FROM_ISR(__irq_status);}
#else 
	/* FreeRTOS v7.x */
    #define TRACE_ALLOC_CRITICAL_SECTION() unsigned portBASE_TYPE __irq_status;
    #define TRACE_ENTER_CRITICAL_SECTION() {__irq_status = portSET_INTERRUPT_MASK_FROM_ISR();}
    #define TRACE_EXIT_CRITICAL_SECTION() {portCLEAR_INTERRUPT_MASK_FROM_ISR(__irq_status);}
#endif
#endif

#ifndef TRACE_ENTER_CRITICAL_SECTION
	#error "This hardware port has no definition for critical sections! See http://percepio.com/2014/10/27/how-to-define-critical-sections-for-the-recorder/"
#endif

/******************************************************************************/
/*** Definitions for Snapshot mode ********************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

/*** The object classes *******************************************************/

#define TRACE_NCLASSES 7
#define TRACE_CLASS_QUEUE ((traceObjectClass)0)
#define TRACE_CLASS_SEMAPHORE ((traceObjectClass)1)
#define TRACE_CLASS_MUTEX ((traceObjectClass)2)
#define TRACE_CLASS_TASK ((traceObjectClass)3)
#define TRACE_CLASS_ISR ((traceObjectClass)4)
#define TRACE_CLASS_TIMER ((traceObjectClass)5)
#define TRACE_CLASS_EVENTGROUP ((traceObjectClass)6)

/*** Definitions for Object Table ********************************************/
#define TRACE_KERNEL_OBJECT_COUNT (TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER + TRC_CFG_NEVENTGROUP)

/* Queue properties (except name):	current number of message in queue */
#define PropertyTableSizeQueue		(TRC_CFG_NAME_LEN_QUEUE + 1)

/* Semaphore properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeSemaphore	(TRC_CFG_NAME_LEN_SEMAPHORE + 1)

/* Mutex properties (except name):	owner (task handle, 0 = free) */
#define PropertyTableSizeMutex		(TRC_CFG_NAME_LEN_MUTEX + 1)

/* Task properties (except name):	Byte 0: Current priority
									Byte 1: state (if already active)
									Byte 2: legacy, not used
									Byte 3: legacy, not used */
#define PropertyTableSizeTask		(TRC_CFG_NAME_LEN_TASK + 4)

/* ISR properties:					Byte 0: priority
									Byte 1: state (if already active) */
#define PropertyTableSizeISR		(TRC_CFG_NAME_LEN_ISR + 2)

/* TRC_CFG_NTIMER properties:				Byte 0: state (unused for now) */
#define PropertyTableSizeTimer		(TRC_CFG_NAME_LEN_TIMER + 1)

/* TRC_CFG_NEVENTGROUP properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeEventGroup	(TRC_CFG_NAME_LEN_EVENTGROUP + 4)


/* The layout of the byte array representing the Object Property Table */
#define StartIndexQueue			0
#define StartIndexSemaphore		StartIndexQueue		+ TRC_CFG_NQUEUE		* PropertyTableSizeQueue
#define StartIndexMutex			StartIndexSemaphore + TRC_CFG_NSEMAPHORE	* PropertyTableSizeSemaphore
#define StartIndexTask			StartIndexMutex		+ TRC_CFG_NMUTEX		* PropertyTableSizeMutex
#define StartIndexISR			StartIndexTask		+ TRC_CFG_NTASK			* PropertyTableSizeTask
#define StartIndexTimer			StartIndexISR		+ TRC_CFG_NISR			* PropertyTableSizeISR
#define StartIndexEventGroup	StartIndexTimer		+ TRC_CFG_NTIMER		* PropertyTableSizeTimer

/* Number of bytes used by the object table */
#define TRACE_OBJECT_TABLE_SIZE	StartIndexEventGroup + TRC_CFG_NEVENTGROUP * PropertyTableSizeEventGroup

///*** FreeRTOS version codes **************************************************/
//#define FREERTOS_VERSION_NOT_SET				0
//#define TRC_FREERTOS_VERSION_7_3_OR_7_4			1
//#define TRC_FREERTOS_VERSION_7_5_OR_7_6			2
//#define TRC_FREERTOS_VERSION_8_X				3
//#define TRC_FREERTOS_VERSION_9_X				4


/* Flag to tell the context of tracePEND_FUNC_CALL_FROM_ISR */
extern int uiInEventGroupSetBitsFromISR;

/* Initialization of the object property table */
void vTraceInitObjectPropertyTable(void);

/* Initialization of the handle mechanism, see e.g, prvTraceGetObjectHandle */
void vTraceInitObjectHandleStack(void);

/* Returns the "Not enough handles" error message for the specified object class */
const char* pszTraceGetErrorNotEnoughHandles(traceObjectClass objectclass);

traceHandle prvTraceGetObjectNumber(void* handle);

uint8_t prvTraceGetObjectType(void* handle);

traceHandle prvTraceGetTaskNumber(void* handle);

void* prvTraceGetCurrentTaskHandle(void);

uint8_t uiTraceIsObjectExcluded(traceObjectClass objectclass, traceHandle handle);

#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X)
traceHandle prvTraceGetEventGroupNumber(void* handle);
#endif

/******************************************************************************
 * TraceObjectClassTable
 * Translates a FreeRTOS QueueType into trace objects classes (TRACE_CLASS_).
 * Has one entry for each QueueType, gives TRACE_CLASS ID.
 ******************************************************************************/			
extern traceObjectClass TraceObjectClassTable[5];


/*** Event codes for snapshot mode - must match Tracealyzer config files ******/

#define NULL_EVENT					(0x00UL)

/*******************************************************************************
 * EVENTGROUP_DIV
 *
 * Miscellaneous events.
 ******************************************************************************/
#define EVENTGROUP_DIV				(NULL_EVENT + 1UL)					/*0x01*/
#define DIV_XPS						(EVENTGROUP_DIV + 0UL)				/*0x01*/
#define DIV_TASK_READY				(EVENTGROUP_DIV + 1UL)				/*0x02*/
#define DIV_NEW_TIME				(EVENTGROUP_DIV + 2UL)				/*0x03*/

/*******************************************************************************
 * EVENTGROUP_TS
 *
 * Events for storing task-switches and interrupts. The RESUME events are
 * generated if the task/interrupt is already marked active.
 ******************************************************************************/
#define EVENTGROUP_TS				(EVENTGROUP_DIV + 3UL)				/*0x04*/
#define TS_ISR_BEGIN				(EVENTGROUP_TS + 0UL)					/*0x04*/
#define TS_ISR_RESUME				(EVENTGROUP_TS + 1UL)					/*0x05*/
#define TS_TASK_BEGIN				(EVENTGROUP_TS + 2UL)					/*0x06*/
#define TS_TASK_RESUME				(EVENTGROUP_TS + 3UL)					/*0x07*/

/*******************************************************************************
 * EVENTGROUP_OBJCLOSE_NAME
 *
 * About Close Events
 * When an object is evicted from the object property table (object close), two
 * internal events are stored (EVENTGROUP_OBJCLOSE_NAME and
 * EVENTGROUP_OBJCLOSE_PROP), containing the handle-name mapping and object
 * properties valid up to this point.
 ******************************************************************************/
#define EVENTGROUP_OBJCLOSE_NAME	(EVENTGROUP_TS + 4UL)					/*0x08*/

/*******************************************************************************
 * EVENTGROUP_OBJCLOSE_PROP
 *
 * The internal event carrying properties of deleted objects
 * The handle and object class of the closed object is not stored in this event,
 * but is assumed to be the same as in the preceding CLOSE event. Thus, these
 * two events must be generated from within a critical section.
 * When queues are closed, arg1 is the "state" property (i.e., number of
 * buffered messages/signals).
 * When actors are closed, arg1 is priority, arg2 is handle of the "instance
 * finish" event, and arg3 is event code of the "instance finish" event.
 * In this case, the lower three bits is the object class of the instance finish
 * handle. The lower three bits are not used (always zero) when queues are
 * closed since the queue type is given in the previous OBJCLOSE_NAME event.
 ******************************************************************************/
#define EVENTGROUP_OBJCLOSE_PROP	(EVENTGROUP_OBJCLOSE_NAME + 8UL)		/*0x10*/

/*******************************************************************************
 * EVENTGROUP_CREATE
 *
 * The events in this group are used to log Kernel object creations.
 * The lower three bits in the event code gives the object class, i.e., type of
 * create operation (task, queue, semaphore, etc).
 ******************************************************************************/
#define EVENTGROUP_CREATE_OBJ_SUCCESS	(EVENTGROUP_OBJCLOSE_PROP + 8UL)	/*0x18*/

/*******************************************************************************
 * EVENTGROUP_SEND
 *
 * The events in this group are used to log Send/Give events on queues,
 * semaphores and mutexes The lower three bits in the event code gives the
 * object class, i.e., what type of object that is operated on (queue, semaphore
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_SEND_SUCCESS	(EVENTGROUP_CREATE_OBJ_SUCCESS + 8UL)		/*0x20*/

/*******************************************************************************
 * EVENTGROUP_RECEIVE
 *
 * The events in this group are used to log Receive/Take events on queues,
 * semaphores and mutexes. The lower three bits in the event code gives the
 * object class, i.e., what type of object that is operated on (queue, semaphore
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_RECEIVE_SUCCESS	(EVENTGROUP_SEND_SUCCESS + 8UL)		/*0x28*/

/* Send/Give operations, from ISR */
#define EVENTGROUP_SEND_FROM_ISR_SUCCESS \
									(EVENTGROUP_RECEIVE_SUCCESS + 8UL)	/*0x30*/

/* Receive/Take operations, from ISR */
#define EVENTGROUP_RECEIVE_FROM_ISR_SUCCESS \
							(EVENTGROUP_SEND_FROM_ISR_SUCCESS + 8UL)		/*0x38*/

/* "Failed" event type versions of above (timeout, failed allocation, etc) */
#define EVENTGROUP_KSE_FAILED \
							(EVENTGROUP_RECEIVE_FROM_ISR_SUCCESS + 8UL)	/*0x40*/

/* Failed create calls - memory allocation failed */
#define EVENTGROUP_CREATE_OBJ_FAILED	(EVENTGROUP_KSE_FAILED)			/*0x40*/

/* Failed send/give - timeout! */
#define EVENTGROUP_SEND_FAILED		(EVENTGROUP_CREATE_OBJ_FAILED + 8UL)	/*0x48*/

/* Failed receive/take - timeout! */
#define EVENTGROUP_RECEIVE_FAILED	 (EVENTGROUP_SEND_FAILED + 8UL)		/*0x50*/

/* Failed non-blocking send/give - queue full */
#define EVENTGROUP_SEND_FROM_ISR_FAILED (EVENTGROUP_RECEIVE_FAILED + 8UL) /*0x58*/

/* Failed non-blocking receive/take - queue empty */
#define EVENTGROUP_RECEIVE_FROM_ISR_FAILED \
								 (EVENTGROUP_SEND_FROM_ISR_FAILED + 8UL)	/*0x60*/

/* Events when blocking on receive/take */
#define EVENTGROUP_RECEIVE_BLOCK \
							(EVENTGROUP_RECEIVE_FROM_ISR_FAILED + 8UL)	/*0x68*/

/* Events when blocking on send/give */
#define EVENTGROUP_SEND_BLOCK	(EVENTGROUP_RECEIVE_BLOCK + 8UL)			/*0x70*/

/* Events on queue peek (receive) */
#define EVENTGROUP_PEEK_SUCCESS	(EVENTGROUP_SEND_BLOCK + 8UL)				/*0x78*/

/* Events on object delete (vTaskDelete or vQueueDelete) */
#define EVENTGROUP_DELETE_OBJ_SUCCESS	(EVENTGROUP_PEEK_SUCCESS + 8UL)	/*0x80*/

/* Other events - object class is implied: TASK */
#define EVENTGROUP_OTHERS	(EVENTGROUP_DELETE_OBJ_SUCCESS + 8UL)			/*0x88*/
#define TASK_DELAY_UNTIL	(EVENTGROUP_OTHERS + 0UL)						/*0x88*/
#define TASK_DELAY			(EVENTGROUP_OTHERS + 1UL)						/*0x89*/
#define TASK_SUSPEND		(EVENTGROUP_OTHERS + 2UL)						/*0x8A*/
#define TASK_RESUME			(EVENTGROUP_OTHERS + 3UL)						/*0x8B*/
#define TASK_RESUME_FROM_ISR	(EVENTGROUP_OTHERS + 4UL)					/*0x8C*/
#define TASK_PRIORITY_SET		(EVENTGROUP_OTHERS + 5UL)					/*0x8D*/
#define TASK_PRIORITY_INHERIT	(EVENTGROUP_OTHERS + 6UL)					/*0x8E*/
#define TASK_PRIORITY_DISINHERIT	(EVENTGROUP_OTHERS + 7UL)				/*0x8F*/

#define EVENTGROUP_MISC_PLACEHOLDER	(EVENTGROUP_OTHERS + 8UL)				/*0x90*/
#define PEND_FUNC_CALL		(EVENTGROUP_MISC_PLACEHOLDER+0UL)				/*0x90*/
#define PEND_FUNC_CALL_FROM_ISR (EVENTGROUP_MISC_PLACEHOLDER+1UL)			/*0x91*/
#define PEND_FUNC_CALL_FAILED (EVENTGROUP_MISC_PLACEHOLDER+2UL)			/*0x92*/
#define PEND_FUNC_CALL_FROM_ISR_FAILED (EVENTGROUP_MISC_PLACEHOLDER+3UL)	/*0x93*/
#define MEM_MALLOC_SIZE (EVENTGROUP_MISC_PLACEHOLDER+4UL)					/*0x94*/
#define MEM_MALLOC_ADDR (EVENTGROUP_MISC_PLACEHOLDER+5UL)					/*0x95*/
#define MEM_FREE_SIZE (EVENTGROUP_MISC_PLACEHOLDER+6UL)					/*0x96*/
#define MEM_FREE_ADDR (EVENTGROUP_MISC_PLACEHOLDER+7UL)					/*0x97*/

/* User events */
#define EVENTGROUP_USEREVENT (EVENTGROUP_MISC_PLACEHOLDER + 8UL)			/*0x98*/
#define USER_EVENT (EVENTGROUP_USEREVENT + 0UL)

/* Allow for 0-15 arguments (the number of args is added to event code) */
#define USER_EVENT_LAST (EVENTGROUP_USEREVENT + 15UL)						/*0xA7*/

/*******************************************************************************
 * XTS Event - eXtended TimeStamp events
 * The timestamps used in the recorder are "differential timestamps" (DTS), i.e.
 * the time since the last stored event. The DTS fields are either 1 or 2 bytes
 * in the other events, depending on the bytes available in the event struct.
 * If the time since the last event (the DTS) is larger than allowed for by
 * the DTS field of the current event, an XTS event is inserted immediately
 * before the original event. The XTS event contains up to 3 additional bytes
 * of the DTS value - the higher bytes of the true DTS value. The lower 1-2
 * bytes are stored in the normal DTS field.
 * There are two types of XTS events, XTS8 and XTS16. An XTS8 event is stored
 * when there is only room for 1 byte (8 bit) DTS data in the original event,
 * which means a limit of 0xFF (255UL). The XTS16 is used when the original event
 * has a 16 bit DTS field and thereby can handle values up to 0xFFFF (65535UL).
 *
 * Using a very high frequency time base can result in many XTS events.
 * Preferably, the time between two OS ticks should fit in 16 bits, i.e.,
 * at most 65535. If your time base has a higher frequency, you can define
 * the TRACE
 ******************************************************************************/

#define EVENTGROUP_SYS (EVENTGROUP_USEREVENT + 16UL)						/*0xA8*/
#define XTS8 (EVENTGROUP_SYS + 0UL)										/*0xA8*/
#define XTS16 (EVENTGROUP_SYS + 1UL)										/*0xA9*/
#define EVENT_BEING_WRITTEN (EVENTGROUP_SYS + 2UL)						/*0xAA*/
#define RESERVED_DUMMY_CODE (EVENTGROUP_SYS + 3UL)						/*0xAB*/
#define LOW_POWER_BEGIN (EVENTGROUP_SYS + 4UL)							/*0xAC*/
#define LOW_POWER_END (EVENTGROUP_SYS + 5UL)								/*0xAD*/
#define XID (EVENTGROUP_SYS + 6UL)										/*0xAE*/
#define XTS16L (EVENTGROUP_SYS + 7UL)										/*0xAF*/

#define EVENTGROUP_TIMER (EVENTGROUP_SYS + 8UL)							/*0xB0*/
#define TIMER_CREATE (EVENTGROUP_TIMER + 0UL)								/*0xB0*/
#define TIMER_START (EVENTGROUP_TIMER + 1UL)								/*0xB1*/
#define TIMER_RST (EVENTGROUP_TIMER + 2UL)								/*0xB2*/
#define TIMER_STOP (EVENTGROUP_TIMER + 3UL)								/*0xB3*/
#define TIMER_CHANGE_PERIOD (EVENTGROUP_TIMER + 4UL)						/*0xB4*/
#define TIMER_DELETE (EVENTGROUP_TIMER + 5UL)								/*0xB5*/
#define TIMER_START_FROM_ISR (EVENTGROUP_TIMER + 6UL)						/*0xB6*/
#define TIMER_RESET_FROM_ISR (EVENTGROUP_TIMER + 7UL)						/*0xB7*/
#define TIMER_STOP_FROM_ISR (EVENTGROUP_TIMER + 8UL)						/*0xB8*/

#define TIMER_CREATE_FAILED (EVENTGROUP_TIMER + 9UL)						/*0xB9*/
#define TIMER_START_FAILED (EVENTGROUP_TIMER + 10UL)						/*0xBA*/
#define TIMER_RESET_FAILED (EVENTGROUP_TIMER + 11UL)						/*0xBB*/
#define TIMER_STOP_FAILED (EVENTGROUP_TIMER + 12UL)						/*0xBC*/
#define TIMER_CHANGE_PERIOD_FAILED (EVENTGROUP_TIMER + 13UL)				/*0xBD*/
#define TIMER_DELETE_FAILED (EVENTGROUP_TIMER + 14UL)						/*0xBE*/
#define TIMER_START_FROM_ISR_FAILED (EVENTGROUP_TIMER + 15UL)				/*0xBF*/
#define TIMER_RESET_FROM_ISR_FAILED (EVENTGROUP_TIMER + 16UL)				/*0xC0*/
#define TIMER_STOP_FROM_ISR_FAILED (EVENTGROUP_TIMER + 17UL)				/*0xC1*/

#define EVENTGROUP_EG (EVENTGROUP_TIMER + 18UL)							/*0xC2*/
#define EVENT_GROUP_CREATE (EVENTGROUP_EG + 0UL)							/*0xC2*/
#define EVENT_GROUP_CREATE_FAILED (EVENTGROUP_EG + 1UL)					/*0xC3*/
#define EVENT_GROUP_SYNC_BLOCK (EVENTGROUP_EG + 2UL)						/*0xC4*/
#define EVENT_GROUP_SYNC_END (EVENTGROUP_EG + 3UL)						/*0xC5*/
#define EVENT_GROUP_WAIT_BITS_BLOCK (EVENTGROUP_EG + 4UL)					/*0xC6*/
#define EVENT_GROUP_WAIT_BITS_END (EVENTGROUP_EG + 5UL)					/*0xC7*/
#define EVENT_GROUP_CLEAR_BITS (EVENTGROUP_EG + 6UL)						/*0xC8*/
#define EVENT_GROUP_CLEAR_BITS_FROM_ISR (EVENTGROUP_EG + 7UL)				/*0xC9*/
#define EVENT_GROUP_SET_BITS (EVENTGROUP_EG + 8UL)						/*0xCA*/
#define EVENT_GROUP_DELETE (EVENTGROUP_EG + 9UL)							/*0xCB*/
#define EVENT_GROUP_SYNC_END_FAILED (EVENTGROUP_EG + 10UL)				/*0xCC*/
#define EVENT_GROUP_WAIT_BITS_END_FAILED (EVENTGROUP_EG + 11UL)			/*0xCD*/
#define EVENT_GROUP_SET_BITS_FROM_ISR (EVENTGROUP_EG + 12UL)				/*0xCE*/
#define EVENT_GROUP_SET_BITS_FROM_ISR_FAILED (EVENTGROUP_EG + 13UL)		/*0xCF*/

#define TASK_INSTANCE_FINISHED_NEXT_KSE (EVENTGROUP_EG + 14UL)			/*0xD0*/
#define TASK_INSTANCE_FINISHED_DIRECT (EVENTGROUP_EG + 15UL)				/*0xD1*/

#define TRACE_TASK_NOTIFY_GROUP (EVENTGROUP_EG + 16UL)					/*0xD2*/
#define TRACE_TASK_NOTIFY (TRACE_TASK_NOTIFY_GROUP + 0UL)					/*0xD2*/
#define TRACE_TASK_NOTIFY_TAKE (TRACE_TASK_NOTIFY_GROUP + 1UL)			/*0xD3*/
#define TRACE_TASK_NOTIFY_TAKE_BLOCK (TRACE_TASK_NOTIFY_GROUP + 2UL)		/*0xD4*/
#define TRACE_TASK_NOTIFY_TAKE_FAILED (TRACE_TASK_NOTIFY_GROUP + 3UL)		/*0xD5*/
#define TRACE_TASK_NOTIFY_WAIT (TRACE_TASK_NOTIFY_GROUP + 4UL)			/*0xD6*/
#define TRACE_TASK_NOTIFY_WAIT_BLOCK (TRACE_TASK_NOTIFY_GROUP + 5UL)		/*0xD7*/
#define TRACE_TASK_NOTIFY_WAIT_FAILED (TRACE_TASK_NOTIFY_GROUP + 6UL)		/*0xD8*/
#define TRACE_TASK_NOTIFY_FROM_ISR (TRACE_TASK_NOTIFY_GROUP + 7UL)		/*0xD9*/
#define TRACE_TASK_NOTIFY_GIVE_FROM_ISR (TRACE_TASK_NOTIFY_GROUP + 8UL)	/*0xDA*/


#define TRACE_GET_TASK_PRIORITY(pxTCB) ((uint8_t)pxTCB->uxPriority)
#define TRACE_GET_TASK_NAME(pxTCB) ((char*)pxTCB->pcTaskName)
#define TRACE_GET_TASK_NUMBER(pxTCB) (prvTraceGetTaskNumber(pxTCB))
#define TRACE_SET_TASK_NUMBER(pxTCB) pxTCB->uxTaskNumber = prvTraceGetObjectHandle(TRACE_CLASS_TASK);

#define TRACE_GET_CLASS_TRACE_CLASS(CLASS, kernelClass) TraceObjectClassTable[kernelClass]
#define TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject) TRACE_GET_CLASS_TRACE_CLASS(CLASS, prvTraceGetObjectType(pxObject))

#define TRACE_GET_TIMER_NUMBER(tmr) (((uint32_t)tmr) != 0 ? ( ( traceHandle ) ((Timer_t*)tmr)->uxTimerNumber ) : 0)
#define TRACE_SET_TIMER_NUMBER(tmr) ((Timer_t*)tmr)->uxTimerNumber = prvTraceGetObjectHandle(TRACE_CLASS_TIMER);
#define TRACE_GET_TIMER_NAME(pxTimer) pxTimer->pcTimerName
#define TRACE_GET_TIMER_PERIOD(pxTimer) pxTimer->xTimerPeriodInTicks

#define TRACE_GET_EVENTGROUP_NUMBER(eg) ( ( traceHandle ) uxEventGroupGetNumber(eg) )

#define TRACE_SET_EVENTGROUP_NUMBER(eg) ((EventGroup_t*)eg)->uxEventGroupNumber = prvTraceGetObjectHandle(TRACE_CLASS_EVENTGROUP);

#define TRACE_GET_OBJECT_NUMBER(CLASS, pxObject) (prvTraceGetObjectNumber(pxObject))

#if (TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_8_X)
	#define TRACE_SET_OBJECT_NUMBER(CLASS, pxObject) pxObject->ucQueueNumber = prvTraceGetObjectHandle(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject));
#else
	#define TRACE_SET_OBJECT_NUMBER(CLASS, pxObject) pxObject->uxQueueNumber = prvTraceGetObjectHandle(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject));
#endif

#define TRACE_GET_CLASS_EVENT_CODE(SERVICE, RESULT, CLASS, kernelClass) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_CLASS_TRACE_CLASS(CLASS, kernelClass))
#define TRACE_GET_OBJECT_EVENT_CODE(SERVICE, RESULT, CLASS, pxObject) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject))
#define TRACE_GET_TASK_EVENT_CODE(SERVICE, RESULT, CLASS, pxTCB) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_CLASS_TASK)

/*** The trace macros for snapshot mode **************************************/

#ifdef configUSE_TICKLESS_IDLE
#if (configUSE_TICKLESS_IDLE != 0)

#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() \
	{ \
		extern uint32_t trace_disable_timestamp; \
		prvTraceStoreLowPower(0); \
		trace_disable_timestamp = 1; \
	}

#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() \
	{ \
		extern uint32_t trace_disable_timestamp; \
		trace_disable_timestamp = 0; \
		prvTraceStoreLowPower(1); \
	}

#endif /* configUSE_TICKLESS_IDLE != 0 */
#endif /* configUSE_TICKLESS_IDLE  */		

/* A macro that will update the tick count when returning from tickless idle */
#undef traceINCREASE_TICK_COUNT
#define traceINCREASE_TICK_COUNT( xCount ) 

/* Called for each task that becomes ready */
#undef traceMOVED_TASK_TO_READY_STATE
#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) \
	trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE(pxTCB);

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
#undef traceTASK_INCREMENT_TICK

#if (TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_7_3_OR_7_4)

#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( unsigned portBASE_TYPE ) pdTRUE || uxMissedTicks == 0) { trcKERNEL_HOOKS_INCREMENT_TICK(); } \
	if (uxSchedulerSuspended == ( unsigned portBASE_TYPE ) pdFALSE) { trcKERNEL_HOOKS_NEW_TIME(DIV_NEW_TIME, xTickCount + 1); }

#else

#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( unsigned portBASE_TYPE ) pdTRUE || uxPendedTicks == 0) { trcKERNEL_HOOKS_INCREMENT_TICK(); } \
	if (uxSchedulerSuspended == ( unsigned portBASE_TYPE ) pdFALSE) { trcKERNEL_HOOKS_NEW_TIME(DIV_NEW_TIME, xTickCount + 1); }

#endif

/* Called on each task-switch */
#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN() \
		trcKERNEL_HOOKS_TASK_SWITCH(TRACE_GET_CURRENT_TASK());

/* Called on vTaskSuspend */
#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND( pxTaskToSuspend ) \
	trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND, pxTaskToSuspend);

/* Called from special case with timer only */
#undef traceTASK_DELAY_SUSPEND
#define traceTASK_DELAY_SUSPEND( pxTaskToSuspend ) \
	trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND, pxTaskToSuspend); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

/* Called on vTaskDelay - note the use of FreeRTOS variable xTicksToDelay */
#undef traceTASK_DELAY
#define traceTASK_DELAY() \
	trcKERNEL_HOOKS_TASK_DELAY(TASK_DELAY, pxCurrentTCB, xTicksToDelay); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

/* Called on vTaskDelayUntil - note the use of FreeRTOS variable xTimeToWake */
#undef traceTASK_DELAY_UNTIL
#if TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X
#define traceTASK_DELAY_UNTIL(xTimeToWake) \
	trcKERNEL_HOOKS_TASK_DELAY(TASK_DELAY_UNTIL, pxCurrentTCB, xTimeToWake); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */
#define traceTASK_DELAY_UNTIL() \
	trcKERNEL_HOOKS_TASK_DELAY(TASK_DELAY_UNTIL, pxCurrentTCB, xTimeToWake); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */

#if (TRC_CFG_INCLUDE_OBJECT_DELETE == 1)
/* Called on vTaskDelete */
#undef traceTASK_DELETE
#define traceTASK_DELETE( pxTaskToDelete ) \
	{ TRACE_ALLOC_CRITICAL_SECTION(); \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_TASK_DELETE(DELETE_OBJ, pxTaskToDelete); \
	TRACE_EXIT_CRITICAL_SECTION(); }

/* Called on vQueueDelete */
#undef traceQUEUE_DELETE
#define traceQUEUE_DELETE( pxQueue ) \
	{ TRACE_ALLOC_CRITICAL_SECTION(); \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_OBJECT_DELETE(DELETE_OBJ, TRC_UNUSED, pxQueue); \
	TRACE_EXIT_CRITICAL_SECTION(); }
#endif

/* Called on vTaskCreate */
#undef traceTASK_CREATE
#define traceTASK_CREATE(pxNewTCB) \
	if (pxNewTCB != NULL) \
	{ \
		trcKERNEL_HOOKS_TASK_CREATE(CREATE_OBJ, TRC_UNUSED, pxNewTCB); \
	}

/* Called in vTaskCreate, if it fails (typically if the stack can not be allocated) */
#undef traceTASK_CREATE_FAILED
#define traceTASK_CREATE_FAILED() \
	trcKERNEL_HOOKS_TASK_CREATE_FAILED(CREATE_OBJ, TRC_UNUSED);

/* Called in xQueueCreate, and thereby for all other object based on queues, such as semaphores. */
#undef traceQUEUE_CREATE
#define traceQUEUE_CREATE( pxNewQueue )\
	trcKERNEL_HOOKS_OBJECT_CREATE(CREATE_OBJ, TRC_UNUSED, pxNewQueue);

/* Called in xQueueCreate, if the queue creation fails */
#undef traceQUEUE_CREATE_FAILED
#define traceQUEUE_CREATE_FAILED( queueType ) \
	trcKERNEL_HOOKS_OBJECT_CREATE_FAILED(CREATE_OBJ, TRC_UNUSED, queueType);

/* This macro is not necessary as of FreeRTOS v9.0.0 */
#if (TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_9_X)
/* Called in xQueueCreateMutex, and thereby also from xSemaphoreCreateMutex and xSemaphoreCreateRecursiveMutex */
#undef traceCREATE_MUTEX
#define traceCREATE_MUTEX( pxNewQueue ) \
	trcKERNEL_HOOKS_OBJECT_CREATE(CREATE_OBJ, TRC_UNUSED, pxNewQueue);
	
/* Called in xQueueCreateMutex when the operation fails (when memory allocation fails) */
#undef traceCREATE_MUTEX_FAILED
#define traceCREATE_MUTEX_FAILED() \
	trcKERNEL_HOOKS_OBJECT_CREATE_FAILED(CREATE_OBJ, TRC_UNUSED, queueQUEUE_TYPE_MUTEX);
#endif /* (TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X) */

/* Called when the Mutex can not be given, since not holder */
#undef traceGIVE_MUTEX_RECURSIVE_FAILED
#define traceGIVE_MUTEX_RECURSIVE_FAILED( pxMutex ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, FAILED, TRC_UNUSED, pxMutex);

/* Called when a message is sent to a queue */	/* CS IS NEW ! */
#undef traceQUEUE_SEND
#define traceQUEUE_SEND( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, SUCCESS, TRC_UNUSED, pxQueue); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(TRC_UNUSED, pxQueue, TRACE_GET_OBJECT_TRACE_CLASS(TRC_UNUSED, pxQueue) == TRACE_CLASS_MUTEX ? (uint8_t)0 : (uint8_t)(pxQueue->uxMessagesWaiting + 1));

/* Called when a message failed to be sent to a queue (timeout) */
#undef traceQUEUE_SEND_FAILED
#define traceQUEUE_SEND_FAILED( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, FAILED, TRC_UNUSED, pxQueue);

/* Called when the task is blocked due to a send operation on a full queue */
#undef traceBLOCKING_ON_QUEUE_SEND
#define traceBLOCKING_ON_QUEUE_SEND( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND, BLOCK, TRC_UNUSED, pxQueue);

/* Called when a message is received from a queue */
#undef traceQUEUE_RECEIVE
#define traceQUEUE_RECEIVE( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, SUCCESS, TRC_UNUSED, pxQueue); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(TRC_UNUSED, pxQueue, TRACE_GET_OBJECT_TRACE_CLASS(TRC_UNUSED, pxQueue) == TRACE_CLASS_MUTEX ? (uint8_t)TRACE_GET_TASK_NUMBER(TRACE_GET_CURRENT_TASK()) : (uint8_t)(pxQueue->uxMessagesWaiting - 1));

/* Called when a receive operation on a queue fails (timeout) */
#undef traceQUEUE_RECEIVE_FAILED
#define traceQUEUE_RECEIVE_FAILED( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, FAILED, TRC_UNUSED, pxQueue);

/* Called when the task is blocked due to a receive operation on an empty queue */
#undef traceBLOCKING_ON_QUEUE_RECEIVE
#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE, BLOCK, TRC_UNUSED, pxQueue); \
	if (TRACE_GET_OBJECT_TRACE_CLASS(TRC_UNUSED, pxQueue) != TRACE_CLASS_MUTEX) \
	{trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();}

/* Called on xQueuePeek */
#undef traceQUEUE_PEEK
#define traceQUEUE_PEEK( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(PEEK, SUCCESS, TRC_UNUSED, pxQueue);

/* Called when a message is sent from interrupt context, e.g., using xQueueSendFromISR */
#undef traceQUEUE_SEND_FROM_ISR
#define traceQUEUE_SEND_FROM_ISR( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND_FROM_ISR, SUCCESS, TRC_UNUSED, pxQueue); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(TRC_UNUSED, pxQueue, (uint8_t)(pxQueue->uxMessagesWaiting + 1));

/* Called when a message send from interrupt context fails (since the queue was full) */
#undef traceQUEUE_SEND_FROM_ISR_FAILED
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(SEND_FROM_ISR, FAILED, TRC_UNUSED, pxQueue);

/* Called when a message is received in interrupt context, e.g., using xQueueReceiveFromISR */
#undef traceQUEUE_RECEIVE_FROM_ISR
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE_FROM_ISR, SUCCESS, TRC_UNUSED, pxQueue); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(TRC_UNUSED, pxQueue, (uint8_t)(pxQueue->uxMessagesWaiting - 1));

/* Called when a message receive from interrupt context fails (since the queue was empty) */
#undef traceQUEUE_RECEIVE_FROM_ISR_FAILED
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(RECEIVE_FROM_ISR, FAILED, TRC_UNUSED, pxQueue);

/* Called in vTaskPrioritySet */
#undef traceTASK_PRIORITY_SET
#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority ) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_SET, pxTask, uxNewPriority);

/* Called in vTaskPriorityInherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_INHERIT
#define traceTASK_PRIORITY_INHERIT( pxTask, uxNewPriority ) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_INHERIT, pxTask, uxNewPriority);

/* Called in vTaskPriorityDisinherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_DISINHERIT
#define traceTASK_PRIORITY_DISINHERIT( pxTask, uxNewPriority ) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_DISINHERIT, pxTask, uxNewPriority);

/* Called in vTaskResume */
#undef traceTASK_RESUME
#define traceTASK_RESUME( pxTaskToResume ) \
	trcKERNEL_HOOKS_TASK_RESUME(TASK_RESUME, pxTaskToResume);

/* Called in vTaskResumeFromISR */
#undef traceTASK_RESUME_FROM_ISR
#define traceTASK_RESUME_FROM_ISR( pxTaskToResume ) \
	trcKERNEL_HOOKS_TASK_RESUME(TASK_RESUME_FROM_ISR, pxTaskToResume);


#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X)

#if (TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_MEMMANG_EVENTS == 1)

extern void vTraceStoreMemMangEvent(uint32_t ecode, uint32_t address, int32_t size);

#undef traceMALLOC
#define traceMALLOC( pvAddress, uiSize ) {if (pvAddress != 0) vTraceStoreMemMangEvent(MEM_MALLOC_SIZE, ( uint32_t ) pvAddress, (int32_t)uiSize); }

#undef traceFREE
#define traceFREE( pvAddress, uiSize ) {vTraceStoreMemMangEvent(MEM_FREE_SIZE, ( uint32_t ) pvAddress, -((int32_t)uiSize)); }

#endif /* (TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_MEMMANG_EVENTS == 1) */

/* Called in timer.c - xTimerCreate */
#undef traceTIMER_CREATE
#define traceTIMER_CREATE(tmr) \
	trcKERNEL_HOOKS_TIMER_CREATE(TIMER_CREATE, tmr);

#undef traceTIMER_CREATE_FAILED
#define traceTIMER_CREATE_FAILED() \
	trcKERNEL_HOOKS_TIMER_EVENT(TIMER_CREATE_FAILED, 0);

/* Note that xCommandID can never be tmrCOMMAND_EXECUTE_CALLBACK (-1) since the trace macro is not called in that case */
#undef traceTIMER_COMMAND_SEND
#define traceTIMER_COMMAND_SEND(tmr, xCommandID, xOptionalValue, xReturn) \
if (xCommandID > tmrCOMMAND_START_DONT_TRACE){\
		if (xCommandID == tmrCOMMAND_CHANGE_PERIOD){ prvTraceStoreKernelCallWithParam((xReturn == pdPASS) ? TIMER_CHANGE_PERIOD : TIMER_CHANGE_PERIOD_FAILED, TRACE_CLASS_TIMER, TRACE_GET_TIMER_NUMBER(tmr), xOptionalValue);}\
		else if ((xCommandID == tmrCOMMAND_DELETE) && (xReturn == pdPASS)){ trcKERNEL_HOOKS_TIMER_DELETE(TIMER_DELETE, tmr); } \
		else {trcKERNEL_HOOKS_TIMER_EVENT(EVENTGROUP_TIMER + (uint32_t)xCommandID + ((xReturn == pdPASS)?0:(TIMER_CREATE_FAILED - TIMER_CREATE)), tmr); }\
}

#undef tracePEND_FUNC_CALL
#define tracePEND_FUNC_CALL(func, arg1, arg2, ret) \
if (ret == pdPASS){ \
	prvTraceStoreKernelCall(PEND_FUNC_CALL, TRACE_CLASS_TASK, uxTaskGetTaskNumber(xTimerGetTimerDaemonTaskHandle()) ); \
}else{ \
	prvTraceStoreKernelCall(PEND_FUNC_CALL_FAILED, TRACE_CLASS_TASK, uxTaskGetTaskNumber(xTimerGetTimerDaemonTaskHandle()) );}

#undef tracePEND_FUNC_CALL_FROM_ISR
#define tracePEND_FUNC_CALL_FROM_ISR(func, arg1, arg2, ret) \
	if (! uiInEventGroupSetBitsFromISR){ prvTraceStoreKernelCall(PEND_FUNC_CALL_FROM_ISR, TRACE_CLASS_TASK, uxTaskGetTaskNumber(xTimerGetTimerDaemonTaskHandle()) ); } \
	uiInEventGroupSetBitsFromISR = 0;

#endif /* (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X) */

#undef traceEVENT_GROUP_CREATE
#define traceEVENT_GROUP_CREATE(eg) \
	TRACE_SET_EVENTGROUP_NUMBER(eg); \
	prvTraceStoreKernelCall(EVENT_GROUP_CREATE, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg));

#undef traceEVENT_GROUP_DELETE
#define traceEVENT_GROUP_DELETE(eg) \
	prvTraceStoreKernelCall(EVENT_GROUP_DELETE, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg)); \
	prvTraceStoreObjectNameOnCloseEvent(TRACE_GET_EVENTGROUP_NUMBER(eg), TRACE_CLASS_EVENTGROUP); \
	prvTraceStoreObjectPropertiesOnCloseEvent(TRACE_GET_EVENTGROUP_NUMBER(eg), TRACE_CLASS_EVENTGROUP); \
	prvTraceFreeObjectHandle(TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg));

#undef traceEVENT_GROUP_CREATE_FAILED
#define traceEVENT_GROUP_CREATE_FAILED() \
	prvTraceStoreKernelCall(EVENT_GROUP_CREATE_FAILED, TRACE_CLASS_EVENTGROUP, 0);

#undef traceEVENT_GROUP_SYNC_BLOCK
#define traceEVENT_GROUP_SYNC_BLOCK(eg, bitsToSet, bitsToWaitFor) \
	prvTraceStoreKernelCallWithParam(EVENT_GROUP_SYNC_BLOCK, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToWaitFor);

#undef traceEVENT_GROUP_SYNC_END
#define traceEVENT_GROUP_SYNC_END(eg, bitsToSet, bitsToWaitFor, wasTimeout) \
	if (wasTimeout){ prvTraceStoreKernelCallWithParam(EVENT_GROUP_SYNC_END_FAILED, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToWaitFor);} \
	else{ prvTraceStoreKernelCallWithParam(EVENT_GROUP_SYNC_END, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToWaitFor); }

#undef traceEVENT_GROUP_WAIT_BITS_BLOCK
#define traceEVENT_GROUP_WAIT_BITS_BLOCK(eg, bitsToWaitFor) \
	prvTraceStoreKernelCallWithParam(EVENT_GROUP_WAIT_BITS_BLOCK, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToWaitFor); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

#undef traceEVENT_GROUP_WAIT_BITS_END
#define traceEVENT_GROUP_WAIT_BITS_END(eg, bitsToWaitFor, wasTimeout) \
	if (wasTimeout){ prvTraceStoreKernelCallWithParam(EVENT_GROUP_WAIT_BITS_END_FAILED, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToWaitFor); } \
	else{ prvTraceStoreKernelCallWithParam(EVENT_GROUP_WAIT_BITS_END, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToWaitFor); }

#undef traceEVENT_GROUP_CLEAR_BITS
#define traceEVENT_GROUP_CLEAR_BITS(eg, bitsToClear) \
	if (bitsToClear) prvTraceStoreKernelCallWithParam(EVENT_GROUP_CLEAR_BITS, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToClear);

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(eg, bitsToClear) \
	if (bitsToClear) prvTraceStoreKernelCallWithParam(EVENT_GROUP_CLEAR_BITS_FROM_ISR, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToClear);

#undef traceEVENT_GROUP_SET_BITS
#define traceEVENT_GROUP_SET_BITS(eg, bitsToSet) \
	prvTraceStoreKernelCallWithParam(EVENT_GROUP_SET_BITS, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToSet);

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR
#define traceEVENT_GROUP_SET_BITS_FROM_ISR(eg, bitsToSet) \
	prvTraceStoreKernelCallWithParam(EVENT_GROUP_SET_BITS_FROM_ISR, TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(eg), bitsToSet); \
	uiInEventGroupSetBitsFromISR = 1;

#undef traceTASK_NOTIFY_TAKE
#if (TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_9_X)
#define traceTASK_NOTIFY_TAKE() \
	if (pxCurrentTCB->eNotifyState == eNotified){ \
		prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_TAKE, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait); \
	}else{ \
		prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_TAKE_FAILED, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait);}
#else /* TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_9_X */
#define traceTASK_NOTIFY_TAKE() \
	if (pxCurrentTCB->ucNotifyState == taskNOTIFICATION_RECEIVED){ \
		prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_TAKE, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait); \
	}else{ \
		prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_TAKE_FAILED, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait);}
#endif /* TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_9_X */

#undef traceTASK_NOTIFY_TAKE_BLOCK
#define traceTASK_NOTIFY_TAKE_BLOCK() \
	prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_TAKE_BLOCK, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

#undef traceTASK_NOTIFY_WAIT
#if (TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_9_X)
#define traceTASK_NOTIFY_WAIT() \
	if (pxCurrentTCB->eNotifyState == eNotified){ \
		prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_WAIT, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait); \
	}else{ \
		prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_WAIT_FAILED, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait);}
#else /* TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_9_X */
#define traceTASK_NOTIFY_WAIT() \
	if (pxCurrentTCB->ucNotifyState == taskNOTIFICATION_RECEIVED){ \
		prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_WAIT, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait); \
	}else{ \
		prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_WAIT_FAILED, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait); }
#endif /* TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_9_X */

#undef traceTASK_NOTIFY_WAIT_BLOCK
#define traceTASK_NOTIFY_WAIT_BLOCK() \
	prvTraceStoreKernelCallWithParam(TRACE_TASK_NOTIFY_WAIT_BLOCK, TRACE_CLASS_TASK, uxTaskGetTaskNumber(pxCurrentTCB), xTicksToWait); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

#undef traceTASK_NOTIFY
#define traceTASK_NOTIFY() \
	prvTraceStoreKernelCall(TRACE_TASK_NOTIFY, TRACE_CLASS_TASK, uxTaskGetTaskNumber(xTaskToNotify));

#undef traceTASK_NOTIFY_FROM_ISR
#define traceTASK_NOTIFY_FROM_ISR() \
	prvTraceStoreKernelCall(TRACE_TASK_NOTIFY_FROM_ISR, TRACE_CLASS_TASK, uxTaskGetTaskNumber(xTaskToNotify));
	
#undef traceTASK_NOTIFY_GIVE_FROM_ISR
#define traceTASK_NOTIFY_GIVE_FROM_ISR() \
	prvTraceStoreKernelCall(TRACE_TASK_NOTIFY_GIVE_FROM_ISR, TRACE_CLASS_TASK, uxTaskGetTaskNumber(xTaskToNotify));

#undef traceQUEUE_REGISTRY_ADD
#define traceQUEUE_REGISTRY_ADD(object, name) prvTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(TRC_UNUSED, object), TRACE_GET_OBJECT_NUMBER(TRC_UNUSED, object), name);

/*******************************************************************************
 * (macro) vTraceExcludeQueue(object)
 *
 * Parameter object: pointer to the Queue object that shall be excluded.
 *
 * Excludes all operations on this object from the trace. Allows for capturing 
 * longer traces the snapshot RAM buffer by filtering out irrelevant events.
 ******************************************************************************/
#define vTraceExcludeQueue(object) \
TRACE_SET_QUEUE_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(TRC_UNUSED, object));

/*******************************************************************************
 * (macro) vTraceExcludeSemaphore(object)
 *
 * Parameter object: pointer to the Semaphore object that shall be excluded.
 *
 * Excludes all operations on this object from the trace. Allows for capturing 
 * longer traces the snapshot RAM buffer by filtering out irrelevant events.
 * 
 * Note: Only for snapshot mode. 
 ******************************************************************************/
#define vTraceExcludeSemaphore(object) \
TRACE_SET_SEMAPHORE_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(TRC_UNUSED, object));

/*******************************************************************************
 * (macro) vTraceExcludeMutex(object)
 *
 * Parameter object: pointer to the Mutex object that shall be excluded.
 *
 * Excludes all operations on this object from the trace. Allows for capturing 
 * longer traces the snapshot RAM buffer by filtering out irrelevant events.
 * 
 * Note: Only for snapshot mode. 
 ******************************************************************************/
#define vTraceExcludeMutex(object) \
TRACE_SET_MUTEX_FLAG_ISEXCLUDED(TRACE_GET_OBJECT_NUMBER(TRC_UNUSED, object));

/*******************************************************************************
 * (macro) vTraceExcludeTimer(object)
 *
 * Parameter object: pointer to the Timer object that shall be excluded.
 *
 * Excludes all operations on this object from the trace. Allows for capturing 
 * longer traces the snapshot RAM buffer by filtering out irrelevant events.
 * 
 * Note: Only for snapshot mode. 
 ******************************************************************************/
#define vTraceExcludeTimer(object) \
TRACE_SET_TIMER_FLAG_ISEXCLUDED(TRACE_GET_TIMER_NUMBER(object));

/*******************************************************************************
 * (macro) vTraceExcludeEventGroup(object)
 *
 * Parameter object: pointer to the Event Group object that shall be excluded.
 *
 * Excludes all operations on this object from the trace. Allows for capturing 
 * longer traces the snapshot RAM buffer by filtering out irrelevant events.
 * 
 * Note: Only for snapshot mode. 
 ******************************************************************************/
#define vTraceExcludeEventGroup(object) \
TRACE_SET_EVENTGROUP_FLAG_ISEXCLUDED(TRACE_GET_EVENTGROUP_NUMBER(object));

/*******************************************************************************
 * (macro) vTraceExcludeTask(object)
 *
 * Parameter object: pointer to the Task object that shall be excluded.
 *
 * Excludes all events from the specified task. Allows for capturing 
 * longer traces the snapshot RAM buffer by filtering out irrelevant events.
 * 
 * Excluding tasks is problematic as  the previous task will appear to continue
 * executing while the excluded task is in fact executing. This therefore affects
 * the timing statistics in an unpredictable way. 
 * Moreover, any operations on queues, semaphores, etc. made by an excluded task
 * will also be excludes, so Tracealyzer will give an incorrect display regarding
 * the states of these objects (number of messages in a queue, etc.).
 *
 * This should only be used on short tasks that don't affect other kernel objects
 * of interest. 
 * 
 * Note: Only for snapshot mode. 
 ******************************************************************************/
#define vTraceExcludeTask(object) \
TRACE_SET_TASK_FLAG_ISEXCLUDED(TRACE_GET_TASK_NUMBER(object));

/******************************************************************************
 * (macro) vTraceExcludeDelays()
 *
 * Excludes all Delay operations from the trace. Allows for capturing 
 * longer traces the snapshot RAM buffer.
 * 
 * Note: Only for snapshot mode. 
 *****************************************************************************/
#define vTraceExcludeDelays() \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(TASK_DELAY); \
TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(TASK_DELAY_UNTIL);

/*** Private helper macros for exclude functionality ************************/

#define TRACE_SET_QUEUE_FLAG_ISEXCLUDED(queueIndex) TRACE_SET_FLAG_ISEXCLUDED(trcExcludedObjects, queueIndex)
#define TRACE_CLEAR_QUEUE_FLAG_ISEXCLUDED(queueIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(trcExcludedObjects, queueIndex)
#define TRACE_GET_QUEUE_FLAG_ISEXCLUDED(queueIndex) TRACE_GET_FLAG_ISEXCLUDED(trcExcludedObjects, queueIndex)

#define TRACE_SET_SEMAPHORE_FLAG_ISEXCLUDED(semaphoreIndex) TRACE_SET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+semaphoreIndex)
#define TRACE_CLEAR_SEMAPHORE_FLAG_ISEXCLUDED(semaphoreIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+semaphoreIndex)
#define TRACE_GET_SEMAPHORE_FLAG_ISEXCLUDED(semaphoreIndex) TRACE_GET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+semaphoreIndex)

#define TRACE_SET_MUTEX_FLAG_ISEXCLUDED(mutexIndex) TRACE_SET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+mutexIndex)
#define TRACE_CLEAR_MUTEX_FLAG_ISEXCLUDED(mutexIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+mutexIndex)
#define TRACE_GET_MUTEX_FLAG_ISEXCLUDED(mutexIndex) TRACE_GET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+mutexIndex)

#define TRACE_SET_TASK_FLAG_ISEXCLUDED(taskIndex) TRACE_SET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+TRC_CFG_NMUTEX+1+taskIndex)
#define TRACE_CLEAR_TASK_FLAG_ISEXCLUDED(taskIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+TRC_CFG_NMUTEX+1+taskIndex)
#define TRACE_GET_TASK_FLAG_ISEXCLUDED(taskIndex) TRACE_GET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+TRC_CFG_NMUTEX+1+taskIndex)

#define TRACE_SET_TIMER_FLAG_ISEXCLUDED(timerIndex) TRACE_SET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+TRC_CFG_NMUTEX+1+TRC_CFG_NTASK+1+timerIndex)
#define TRACE_CLEAR_TIMER_FLAG_ISEXCLUDED(timerIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+TRC_CFG_NMUTEX+1+TRC_CFG_NTASK+1+timerIndex)
#define TRACE_GET_TIMER_FLAG_ISEXCLUDED(timerIndex) TRACE_GET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+TRC_CFG_NMUTEX+1+TRC_CFG_NTASK+1+timerIndex)

#define TRACE_SET_EVENTGROUP_FLAG_ISEXCLUDED(egIndex) TRACE_SET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+TRC_CFG_NMUTEX+1+TRC_CFG_NTASK+1+TRC_CFG_NTIMER+1+egIndex)
#define TRACE_CLEAR_EVENTGROUP_FLAG_ISEXCLUDED(egIndex) TRACE_CLEAR_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+TRC_CFG_NMUTEX+1+TRC_CFG_NTASK+1+TRC_CFG_NTIMER+1+egIndex)
#define TRACE_GET_EVENTGROUP_FLAG_ISEXCLUDED(egIndex) TRACE_GET_FLAG_ISEXCLUDED(trcExcludedObjects, TRC_CFG_NQUEUE+1+TRC_CFG_NSEMAPHORE+1+TRC_CFG_NMUTEX+1+TRC_CFG_NTASK+1+TRC_CFG_NTIMER+1+egIndex)


#define TRACE_CLEAR_OBJECT_FLAG_ISEXCLUDED(objectclass, handle) \
switch (objectclass) \
{ \
case TRACE_CLASS_QUEUE: \
	TRACE_CLEAR_QUEUE_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_SEMAPHORE: \
	TRACE_CLEAR_SEMAPHORE_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_MUTEX: \
	TRACE_CLEAR_MUTEX_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TASK: \
	TRACE_CLEAR_TASK_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TIMER: \
	TRACE_CLEAR_TIMER_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_EVENTGROUP: \
	TRACE_CLEAR_EVENTGROUP_FLAG_ISEXCLUDED(handle); \
	break; \
}

#define TRACE_SET_OBJECT_FLAG_ISEXCLUDED(objectclass, handle) \
switch (objectclass) \
{ \
case TRACE_CLASS_QUEUE: \
	TRACE_SET_QUEUE_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_SEMAPHORE: \
	TRACE_SET_SEMAPHORE_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_MUTEX: \
	TRACE_SET_MUTEX_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TASK: \
	TRACE_SET_TASK_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_TIMER: \
	TRACE_SET_TIMER_FLAG_ISEXCLUDED(handle); \
	break; \
case TRACE_CLASS_EVENTGROUP: \
	TRACE_SET_EVENTGROUP_FLAG_ISEXCLUDED(handle); \
	break; \
}

#endif /*#if TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT */

/******************************************************************************/
/*** Definitions for Streaming mode *******************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/*******************************************************************************
* vTraceStoreKernelObjectName
*
* Set the name for a kernel object (defined by its address).
******************************************************************************/			
void vTraceStoreKernelObjectName(void* object, const char* name); 

/*******************************************************************************
 * prvTraceOnBegin
 *
 * Called on trace begin.
 ******************************************************************************/
void prvTraceOnBegin(void);

/*******************************************************************************
 * prvTraceOnEnd
 *
 * Called on trace end.
 ******************************************************************************/
void prvTraceOnEnd(void);

/*******************************************************************************
* prvIsNewTCB
*
* Tells if this task is already executing, or if there has been a task-switch.
* Assumed to be called within a trace hook in kernel context.
*******************************************************************************/
uint32_t prvIsNewTCB(void* pNewTCB);

#define TRACE_GET_CURRENT_TASK() prvTraceGetCurrentTaskHandle()

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

/*** Event codes for streaming - should match the Tracealyzer config file *****/
#define PSF_EVENT_NULL_EVENT								0x00

#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03
#define PSF_EVENT_TASK_PRIORITY								0x04
#define PSF_EVENT_TASK_PRIO_INHERIT							0x05
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x06
#define PSF_EVENT_DEFINE_ISR								0x07

#define PSF_EVENT_TASK_CREATE								0x10
#define PSF_EVENT_QUEUE_CREATE								0x11
#define PSF_EVENT_SEMAPHORE_BINARY_CREATE					0x12
#define PSF_EVENT_MUTEX_CREATE								0x13
#define PSF_EVENT_TIMER_CREATE								0x14
#define PSF_EVENT_EVENTGROUP_CREATE							0x15
#define PSF_EVENT_SEMAPHORE_COUNTING_CREATE					0x16
#define PSF_EVENT_MUTEX_RECURSIVE_CREATE					0x17

#define PSF_EVENT_TASK_DELETE								0x20
#define PSF_EVENT_QUEUE_DELETE								0x21
#define PSF_EVENT_SEMAPHORE_DELETE							0x22
#define PSF_EVENT_MUTEX_DELETE								0x23
#define PSF_EVENT_TIMER_DELETE								0x24
#define PSF_EVENT_EVENTGROUP_DELETE							0x25

#define PSF_EVENT_TASK_READY								0x30
#define PSF_EVENT_NEW_TIME									0x31
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED				0x32
#define PSF_EVENT_ISR_BEGIN									0x33
#define PSF_EVENT_ISR_RESUME								0x34
#define PSF_EVENT_TS_BEGIN									0x35
#define PSF_EVENT_TS_RESUME									0x36
#define PSF_EVENT_TASK_ACTIVATE								0x37

#define PSF_EVENT_MALLOC									0x38
#define PSF_EVENT_FREE										0x39

#define PSF_EVENT_LOWPOWER_BEGIN							0x3A
#define PSF_EVENT_LOWPOWER_END								0x3B

#define PSF_EVENT_IFE_NEXT									0x3C
#define PSF_EVENT_IFE_DIRECT								0x3D

#define PSF_EVENT_TASK_CREATE_FAILED						0x40
#define PSF_EVENT_QUEUE_CREATE_FAILED						0x41
#define PSF_EVENT_SEMAPHORE_BINARY_CREATE_FAILED			0x42
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x43
#define PSF_EVENT_TIMER_CREATE_FAILED						0x44
#define PSF_EVENT_EVENTGROUP_CREATE_FAILED					0x45
#define PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED			0x46
#define PSF_EVENT_MUTEX_RECURSIVE_CREATE_FAILED				0x47

#define PSF_EVENT_TIMER_DELETE_FAILED						0x48

#define PSF_EVENT_QUEUE_SEND								0x50
#define PSF_EVENT_SEMAPHORE_GIVE							0x51
#define PSF_EVENT_MUTEX_GIVE								0x52

#define PSF_EVENT_QUEUE_SEND_FAILED							0x53
#define PSF_EVENT_SEMAPHORE_GIVE_FAILED						0x54
#define PSF_EVENT_MUTEX_GIVE_FAILED							0x55

#define PSF_EVENT_QUEUE_SEND_BLOCK							0x56
#define PSF_EVENT_SEMAPHORE_GIVE_BLOCK						0x57
#define PSF_EVENT_MUTEX_GIVE_BLOCK							0x58

#define PSF_EVENT_QUEUE_SEND_FROMISR						0x59
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR					0x5A

#define PSF_EVENT_QUEUE_SEND_FROMISR_FAILED					0x5C
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED				0x5D

#define PSF_EVENT_QUEUE_RECEIVE								0x60
#define PSF_EVENT_SEMAPHORE_TAKE							0x61
#define PSF_EVENT_MUTEX_TAKE								0x62

#define PSF_EVENT_QUEUE_RECEIVE_FAILED						0x63
#define PSF_EVENT_SEMAPHORE_TAKE_FAILED						0x64
#define PSF_EVENT_MUTEX_TAKE_FAILED							0x65

#define PSF_EVENT_QUEUE_RECEIVE_BLOCK						0x66
#define PSF_EVENT_SEMAPHORE_TAKE_BLOCK						0x67
#define PSF_EVENT_MUTEX_TAKE_BLOCK							0x68

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR						0x69
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR					0x6A

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED				0x6C
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED				0x6D

#define PSF_EVENT_QUEUE_PEEK								0x70
#define PSF_EVENT_SEMAPHORE_PEEK							0x71	/* Will never be used */
#define PSF_EVENT_MUTEX_PEEK								0x72	/* Will never be used */

#define PSF_EVENT_QUEUE_PEEK_FAILED							0x73
#define PSF_EVENT_SEMAPHORE_PEEK_FAILED						0x74	/* Will never be used */
#define PSF_EVENT_MUTEX_PEEK_FAILED							0x75	/* Will never be used */

#define PSF_EVENT_QUEUE_PEEK_BLOCK							0x76
#define PSF_EVENT_SEMAPHORE_PEEK_BLOCK						0x77	/* Will never be used */
#define PSF_EVENT_MUTEX_PEEK_BLOCK							0x78	/* Will never be used */

#define PSF_EVENT_TASK_DELAY_UNTIL							0x79
#define PSF_EVENT_TASK_DELAY								0x7A
#define PSF_EVENT_TASK_SUSPEND								0x7B
#define PSF_EVENT_TASK_RESUME								0x7C
#define PSF_EVENT_TASK_RESUME_FROMISR						0x7D

#define PSF_EVENT_TIMER_PENDFUNCCALL						0x80
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR				0x81
#define PSF_EVENT_TIMER_PENDFUNCCALL_FAILED					0x82
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR_FAILED			0x83

#define PSF_EVENT_USER_EVENT								0x90

#define PSF_EVENT_TIMER_START								0xA0
#define PSF_EVENT_TIMER_RESET								0xA1
#define PSF_EVENT_TIMER_STOP								0xA2
#define PSF_EVENT_TIMER_CHANGEPERIOD						0xA3
#define PSF_EVENT_TIMER_START_FROMISR						0xA4
#define PSF_EVENT_TIMER_RESET_FROMISR						0xA5
#define PSF_EVENT_TIMER_STOP_FROMISR						0xA6
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR				0xA7
#define PSF_EVENT_TIMER_START_FAILED						0xA8
#define PSF_EVENT_TIMER_RESET_FAILED						0xA9
#define PSF_EVENT_TIMER_STOP_FAILED							0xAA
#define PSF_EVENT_TIMER_CHANGEPERIOD_FAILED					0xAB
#define PSF_EVENT_TIMER_START_FROMISR_FAILED				0xAC
#define PSF_EVENT_TIMER_RESET_FROMISR_FAILED				0xAD
#define PSF_EVENT_TIMER_STOP_FROMISR_FAILED					0xAE
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED			0xAF

#define PSF_EVENT_EVENTGROUP_SYNC							0xB0
#define PSF_EVENT_EVENTGROUP_WAITBITS						0xB1
#define PSF_EVENT_EVENTGROUP_CLEARBITS						0xB2
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR				0xB3
#define PSF_EVENT_EVENTGROUP_SETBITS						0xB4
#define PSF_EVENT_EVENTGROUP_SETBITS_FROMISR				0xB5
#define PSF_EVENT_EVENTGROUP_SYNC_BLOCK						0xB6
#define PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK					0xB7
#define PSF_EVENT_EVENTGROUP_SYNC_FAILED					0xB8
#define PSF_EVENT_EVENTGROUP_WAITBITS_FAILED				0xB9

#define PSF_EVENT_QUEUE_SEND_FRONT							0xC0
#define PSF_EVENT_QUEUE_SEND_FRONT_FAILED					0xC1
#define PSF_EVENT_QUEUE_SEND_FRONT_BLOCK					0xC2
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR					0xC3
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR_FAILED			0xC4
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE						0xC5
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE_FAILED				0xC6
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE						0xC7
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE_FAILED				0xC8

#define PSF_EVENT_TASK_NOTIFY								0xC9
#define PSF_EVENT_TASK_NOTIFY_TAKE							0xCA
#define PSF_EVENT_TASK_NOTIFY_TAKE_BLOCK					0xCB
#define PSF_EVENT_TASK_NOTIFY_TAKE_FAILED					0xCC
#define PSF_EVENT_TASK_NOTIFY_WAIT							0xCD
#define PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK					0xCE
#define PSF_EVENT_TASK_NOTIFY_WAIT_FAILED					0xCF
#define PSF_EVENT_TASK_NOTIFY_FROM_ISR						0xD0
#define PSF_EVENT_TASK_NOTIFY_GIVE_FROM_ISR					0xD1

/*** The trace macros for streaming ******************************************/

#if (defined(configUSE_TICKLESS_IDLE) && configUSE_TICKLESS_IDLE != 0)

#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_LOWPOWER_BEGIN, xExpectedIdleTime); \
	}

#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() \
	{ \
		prvTraceStoreEvent0(PSF_EVENT_LOWPOWER_END); \
	}

#endif

/* A macro that will update the tick count when returning from tickless idle */
#undef traceINCREASE_TICK_COUNT
/* Note: This can handle time adjustments of max 2^32 ticks, i.e., 35 seconds at 120 MHz. Thus, tick-less idle periods longer than 2^32 ticks will appear "compressed" on the time line.*/
#define traceINCREASE_TICK_COUNT( xCount ) { extern uint32_t uiTraceTickCount; uiTraceTickCount += xCount; }

/* Called for each task that becomes ready */
#undef traceMOVED_TASK_TO_READY_STATE
#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) \
	prvTraceStoreEvent1(PSF_EVENT_TASK_READY, (uint32_t)pxTCB);

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
#undef traceTASK_INCREMENT_TICK
#if TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_7_3_OR_7_4
#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( unsigned portBASE_TYPE ) pdTRUE || uxMissedTicks == 0) { extern uint32_t uiTraceTickCount; uiTraceTickCount++; } \
	if (uxSchedulerSuspended == ( unsigned portBASE_TYPE ) pdFALSE) { prvTraceStoreEvent1(PSF_EVENT_NEW_TIME, (uint32_t)(xTickCount + 1)); }
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_7_3_OR_7_4 */
#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( unsigned portBASE_TYPE ) pdTRUE || uxPendedTicks == 0) { extern uint32_t uiTraceTickCount; uiTraceTickCount++; } \
	if (uxSchedulerSuspended == ( unsigned portBASE_TYPE ) pdFALSE) { prvTraceStoreEvent1(PSF_EVENT_NEW_TIME, (uint32_t)(xTickCount + 1)); }
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_7_3_OR_7_4 */

/* Called on each task-switch */
#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN() \
	if (prvIsNewTCB(pxCurrentTCB)) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_ACTIVATE, (uint32_t)pxCurrentTCB, pxCurrentTCB->uxPriority); \
	}

/* Called on vTaskSuspend */
#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND( pxTaskToSuspend ) \
	prvTraceStoreEvent1(PSF_EVENT_TASK_SUSPEND, (uint32_t)pxTaskToSuspend);

/* Called on vTaskDelay - note the use of FreeRTOS variable xTicksToDelay */
#undef traceTASK_DELAY
#define traceTASK_DELAY() \
	prvTraceStoreEvent1(PSF_EVENT_TASK_DELAY, xTicksToDelay);

/* Called on vTaskDelayUntil - note the use of FreeRTOS variable xTimeToWake */
#undef traceTASK_DELAY_UNTIL
#if TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X
#define traceTASK_DELAY_UNTIL(xTimeToWake) \
	prvTraceStoreEvent1(PSF_EVENT_TASK_DELAY_UNTIL, (uint32_t)xTimeToWake);
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */
#define traceTASK_DELAY_UNTIL() \
	prvTraceStoreEvent1(PSF_EVENT_TASK_DELAY_UNTIL, (uint32_t)xTimeToWake);
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */

/* Called on vTaskDelete */
#undef traceTASK_DELETE
#define traceTASK_DELETE( pxTaskToDelete ) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_DELETE, (uint32_t)pxTaskToDelete, (pxTaskToDelete != NULL) ? (pxTaskToDelete->uxPriority) : 0); \
	prvTraceDeleteSymbol(pxTaskToDelete); \
	prvTraceDeleteObjectData(pxTaskToDelete);

/* Called on vQueueDelete */
#undef traceQUEUE_DELETE
#define traceQUEUE_DELETE( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent2(PSF_EVENT_QUEUE_DELETE, (uint32_t)pxQueue, (pxQueue != NULL) ? (pxQueue->uxMessagesWaiting) : 0); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceStoreEvent2(PSF_EVENT_MUTEX_DELETE, (uint32_t)pxQueue, (pxQueue != NULL) ? (pxQueue->uxMessagesWaiting) : 0); \
			break; \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_DELETE, (uint32_t)pxQueue, (pxQueue != NULL) ? (pxQueue->uxMessagesWaiting) : 0); \
			break; \
	} \
	prvTraceDeleteSymbol(pxQueue);

/* Called on vTaskCreate */
#undef traceTASK_CREATE
#if TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X
#define traceTASK_CREATE(pxNewTCB) \
	if (pxNewTCB != NULL) \
	{ \
		prvTraceSaveSymbol(pxNewTCB, pxNewTCB->pcTaskName); \
		prvTraceSaveObjectData(pxNewTCB, pxNewTCB->uxPriority); \
		prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, pxNewTCB->pcTaskName, pxNewTCB); \
		prvTraceStoreEvent2(PSF_EVENT_TASK_CREATE, (uint32_t)pxNewTCB, pxNewTCB->uxPriority); \
	}
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */
#define traceTASK_CREATE(pxNewTCB) \
	if (pxNewTCB != NULL) \
	{ \
		prvTraceSaveSymbol(pxNewTCB, (const char*)pcName); \
		prvTraceSaveObjectData(pxNewTCB, uxPriority); \
		prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, pcName, pxNewTCB); \
		prvTraceStoreEvent2(PSF_EVENT_TASK_CREATE, (uint32_t)pxNewTCB, uxPriority); \
	}
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */

/* Called in vTaskCreate, if it fails (typically if the stack can not be allocated) */
#undef traceTASK_CREATE_FAILED
#define traceTASK_CREATE_FAILED() \
	prvTraceStoreEvent0(PSF_EVENT_TASK_CREATE_FAILED);

/* Called in xQueueCreate, and thereby for all other object based on queues, such as semaphores. */
#undef traceQUEUE_CREATE
#define traceQUEUE_CREATE( pxNewQueue )\
	switch (pxNewQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent2(PSF_EVENT_QUEUE_CREATE, (uint32_t)pxNewQueue, pxNewQueue->uxLength); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
			prvTraceStoreEvent1(PSF_EVENT_SEMAPHORE_BINARY_CREATE, (uint32_t)pxNewQueue); \
			break; \
	}

/* Called in xQueueCreate, if the queue creation fails */
#undef traceQUEUE_CREATE_FAILED
#define traceQUEUE_CREATE_FAILED( queueType ) \
	switch (queueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent0(PSF_EVENT_QUEUE_CREATE_FAILED); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
			prvTraceStoreEvent0(PSF_EVENT_SEMAPHORE_BINARY_CREATE_FAILED); \
			break; \
	}

/* Called in xQueueCreateCountingSemaphore, if the queue creation fails */
#undef traceCREATE_COUNTING_SEMAPHORE
#if TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X
#define traceCREATE_COUNTING_SEMAPHORE() \
	prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_COUNTING_CREATE, (uint32_t)xHandle, ((Queue_t *) xHandle)->uxMessagesWaiting);
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */
#define traceCREATE_COUNTING_SEMAPHORE() \
	prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_COUNTING_CREATE, (uint32_t)pxHandle, pxHandle->uxMessagesWaiting);
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */

#undef traceCREATE_COUNTING_SEMAPHORE_FAILED
#define traceCREATE_COUNTING_SEMAPHORE_FAILED() \
	prvTraceStoreEvent0(PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED);

/* Called in xQueueCreateMutex, and thereby also from xSemaphoreCreateMutex and xSemaphoreCreateRecursiveMutex */
#undef traceCREATE_MUTEX
#define traceCREATE_MUTEX( pxNewQueue ) \
	switch (pxNewQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_MUTEX: \
			prvTraceStoreEvent1(PSF_EVENT_MUTEX_CREATE, (uint32_t)pxNewQueue); \
			break; \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceStoreEvent1(PSF_EVENT_MUTEX_RECURSIVE_CREATE, (uint32_t)pxNewQueue); \
			break; \
	}

/* Called in xQueueCreateMutex when the operation fails (when memory allocation fails) */
#undef traceCREATE_MUTEX_FAILED
#define traceCREATE_MUTEX_FAILED() \
	prvTraceStoreEvent0(PSF_EVENT_MUTEX_CREATE_FAILED);

/* Called when a message is sent to a queue */	/* CS IS NEW ! */
#undef traceQUEUE_SEND
#define traceQUEUE_SEND( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND : PSF_EVENT_QUEUE_SEND_FRONT, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting + 1); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_GIVE, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting + 1); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceStoreEvent1(PSF_EVENT_MUTEX_GIVE, (uint32_t)pxQueue); \
			break; \
	}

/* Called when a message failed to be sent to a queue (timeout) */
#undef traceQUEUE_SEND_FAILED
#define traceQUEUE_SEND_FAILED( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FAILED : PSF_EVENT_QUEUE_SEND_FRONT_FAILED, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_GIVE_FAILED, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceStoreEvent1(PSF_EVENT_MUTEX_GIVE_FAILED, (uint32_t)pxQueue); \
			break; \
	}

/* Called when the task is blocked due to a send operation on a full queue */
#undef traceBLOCKING_ON_QUEUE_SEND
#define traceBLOCKING_ON_QUEUE_SEND( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_BLOCK : PSF_EVENT_QUEUE_SEND_FRONT_BLOCK, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_GIVE_BLOCK, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceStoreEvent1(PSF_EVENT_MUTEX_GIVE_BLOCK, (uint32_t)pxQueue); \
			break; \
	}

/* Called for Recursive Mutex */
#undef traceGIVE_MUTEX_RECURSIVE
#define traceGIVE_MUTEX_RECURSIVE( pxMutex ) \
	prvTraceStoreEvent1(PSF_EVENT_MUTEX_GIVE_RECURSIVE, (uint32_t)pxMutex);

/* Called for Recursive Mutex */
#undef traceGIVE_MUTEX_RECURSIVE_FAILED
#define traceGIVE_MUTEX_RECURSIVE_FAILED( pxMutex ) \
	prvTraceStoreEvent1(PSF_EVENT_MUTEX_GIVE_RECURSIVE_FAILED, (uint32_t)pxMutex);

/**************************************************************************/
/* Makes sure xQueueGiveFromISR also has a xCopyPosition parameter        */
/**************************************************************************/
/* Helpers needed to correctly expand names */
#define TZ__CAT2(a,b) a ## b
#define TZ__CAT(a,b) TZ__CAT2(a, b)

/* Expands name if this header is included... uxQueueType must be a macro that only exists in queue.c or whatever, and it must expand to nothing or to something that's valid in identifiers */
#define xQueueGiveFromISR(a,b) TZ__CAT(xQueueGiveFromISR__, uxQueueType) (a,b)

/* If in queue.c, the "uxQueueType" macro expands to "pcHead". queueSEND_TO_BACK is the value we need to send in */
#define xQueueGiveFromISR__pcHead(__a, __b) MyWrapper(__a, __b, const BaseType_t xCopyPosition); \
BaseType_t xQueueGiveFromISR(__a, __b) { return MyWrapper(xQueue, pxHigherPriorityTaskWoken, queueSEND_TO_BACK); } \
BaseType_t MyWrapper(__a, __b, const BaseType_t xCopyPosition)

/* If not in queue.c, "uxQueueType" isn't expanded */
#define xQueueGiveFromISR__uxQueueType(__a, __b) xQueueGiveFromISR(__a,__b)

/**************************************************************************/
/* End of xQueueGiveFromISR fix                                           */
/**************************************************************************/

/* Called when a message is sent from interrupt context, e.g., using xQueueSendFromISR */
#undef traceQUEUE_SEND_FROM_ISR
#define traceQUEUE_SEND_FROM_ISR( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR : PSF_EVENT_QUEUE_SEND_FRONT_FROMISR, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting + 1); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_GIVE_FROMISR, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting + 1); \
			break; \
	}

/* Called when a message send from interrupt context fails (since the queue was full) */
#undef traceQUEUE_SEND_FROM_ISR_FAILED
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR_FAILED : PSF_EVENT_QUEUE_SEND_FRONT_FROMISR_FAILED, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting); \
			break; \
	}

/* Called when a message is received from a queue */
#undef traceQUEUE_RECEIVE
#define traceQUEUE_RECEIVE( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent3(PSF_EVENT_QUEUE_RECEIVE, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxMessagesWaiting - 1); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent3(PSF_EVENT_SEMAPHORE_TAKE, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxMessagesWaiting - 1); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceStoreEvent2(PSF_EVENT_MUTEX_TAKE, (uint32_t)pxQueue, xTicksToWait); \
			break; \
	}

/* Called when a receive operation on a queue fails (timeout) */
#undef traceQUEUE_RECEIVE_FAILED
#define traceQUEUE_RECEIVE_FAILED( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent3(xJustPeeking == pdFALSE ? PSF_EVENT_QUEUE_RECEIVE_FAILED : PSF_EVENT_QUEUE_PEEK_FAILED, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent3(xJustPeeking == pdFALSE ? PSF_EVENT_SEMAPHORE_TAKE_FAILED : PSF_EVENT_SEMAPHORE_PEEK_FAILED, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceStoreEvent2(xJustPeeking == pdFALSE ? PSF_EVENT_MUTEX_TAKE_FAILED : PSF_EVENT_MUTEX_PEEK_FAILED, (uint32_t)pxQueue, xTicksToWait); \
			break; \
	}

/* Called when the task is blocked due to a receive operation on an empty queue */
#undef traceBLOCKING_ON_QUEUE_RECEIVE
#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent3(xJustPeeking == pdFALSE ? PSF_EVENT_QUEUE_RECEIVE_BLOCK : PSF_EVENT_QUEUE_PEEK_BLOCK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent3(xJustPeeking == pdFALSE ? PSF_EVENT_SEMAPHORE_TAKE_BLOCK : PSF_EVENT_SEMAPHORE_PEEK_BLOCK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceStoreEvent2(xJustPeeking == pdFALSE ? PSF_EVENT_MUTEX_TAKE_BLOCK : PSF_EVENT_MUTEX_PEEK_BLOCK, (uint32_t)pxQueue, xTicksToWait); \
			break; \
	}
		
#undef traceTAKE_MUTEX_RECURSIVE
#if TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X
#define traceTAKE_MUTEX_RECURSIVE( pxQueue ) \
	prvTraceStoreEvent2(PSF_EVENT_MUTEX_TAKE_RECURSIVE, (uint32_t)pxQueue, xTicksToWait);
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */
#define traceTAKE_MUTEX_RECURSIVE( pxQueue ) \
	prvTraceStoreEvent2(PSF_EVENT_MUTEX_TAKE_RECURSIVE, (uint32_t)pxQueue, xBlockTime);
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */

#undef traceTAKE_MUTEX_RECURSIVE_FAILED
#if TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X
#define traceTAKE_MUTEX_RECURSIVE_FAILED( pxQueue ) \
	prvTraceStoreEvent2(PSF_EVENT_MUTEX_TAKE_RECURSIVE_FAILED, (uint32_t)pxQueue, xTicksToWait);
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */
#define traceTAKE_MUTEX_RECURSIVE_FAILED( pxQueue ) \
	prvTraceStoreEvent2(PSF_EVENT_MUTEX_TAKE_RECURSIVE_FAILED, (uint32_t)pxQueue, xBlockTime);
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */

/* Called when a message is received in interrupt context, e.g., using xQueueReceiveFromISR */
#undef traceQUEUE_RECEIVE_FROM_ISR
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue ) \
switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent2(PSF_EVENT_QUEUE_RECEIVE_FROMISR, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting - 1); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_TAKE_FROMISR, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting - 1); \
			break; \
	}

/* Called when a message receive from interrupt context fails (since the queue was empty) */
#undef traceQUEUE_RECEIVE_FROM_ISR_FAILED
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent2(PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED, (uint32_t)pxQueue, pxQueue->uxMessagesWaiting); \
			break; \
	}

/* Called on xQueuePeek */
#undef traceQUEUE_PEEK
#define traceQUEUE_PEEK( pxQueue ) \
	switch (pxQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
			prvTraceStoreEvent3(PSF_EVENT_QUEUE_PEEK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceStoreEvent3(PSF_EVENT_SEMAPHORE_PEEK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceStoreEvent1(PSF_EVENT_MUTEX_PEEK, (uint32_t)pxQueue); \
			break; \
	}

/* Called in vTaskPrioritySet */
#undef traceTASK_PRIORITY_SET
#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority ) \
	prvTraceSaveObjectData(pxTask, uxNewPriority); \
	prvTraceStoreEvent2(PSF_EVENT_TASK_PRIORITY, (uint32_t)pxTask, uxNewPriority);
	
/* Called in vTaskPriorityInherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_INHERIT
#define traceTASK_PRIORITY_INHERIT( pxTask, uxNewPriority ) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_PRIO_INHERIT, (uint32_t)pxTask, uxNewPriority);

/* Called in vTaskPriorityDisinherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_DISINHERIT
#define traceTASK_PRIORITY_DISINHERIT( pxTask, uxNewPriority ) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_PRIO_DISINHERIT, (uint32_t)pxTask, uxNewPriority);

/* Called in vTaskResume */
#undef traceTASK_RESUME
#define traceTASK_RESUME( pxTaskToResume ) \
	prvTraceStoreEvent1(PSF_EVENT_TASK_RESUME, (uint32_t)pxTaskToResume);

/* Called in vTaskResumeFromISR */
#undef traceTASK_RESUME_FROM_ISR
#define traceTASK_RESUME_FROM_ISR( pxTaskToResume ) \
	prvTraceStoreEvent1(PSF_EVENT_TASK_RESUME_FROMISR, (uint32_t)pxTaskToResume);

#undef traceMALLOC
#define traceMALLOC( pvAddress, uiSize ) \
	prvTraceStoreEvent2(PSF_EVENT_MALLOC, (uint32_t)pvAddress, uiSize);

#undef traceFREE
#define traceFREE( pvAddress, uiSize ) \
	prvTraceStoreEvent2(PSF_EVENT_FREE, (uint32_t)pvAddress, (uint32_t)(-uiSize));

/* Called in timer.c - xTimerCreate */
#undef traceTIMER_CREATE
#define traceTIMER_CREATE(tmr) \
	prvTraceSaveSymbol(tmr, tmr->pcTimerName); \
	prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, tmr->pcTimerName, tmr); \
	prvTraceStoreEvent2(PSF_EVENT_TIMER_CREATE, (uint32_t)tmr, tmr->xTimerPeriodInTicks);

#undef traceTIMER_CREATE_FAILED
#define traceTIMER_CREATE_FAILED() \
	prvTraceStoreEvent0(PSF_EVENT_TIMER_CREATE_FAILED);

#if (TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X)
#define traceTIMER_COMMAND_SEND_8_0_CASES(tmr) \
	case tmrCOMMAND_RESET: \
		prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_RESET : PSF_EVENT_TIMER_RESET_FAILED, (uint32_t)tmr, xOptionalValue); \
		break; \
	case tmrCOMMAND_START_FROM_ISR: \
		prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_START_FROMISR : PSF_EVENT_TIMER_START_FROMISR_FAILED, (uint32_t)tmr, xOptionalValue); \
		break; \
	case tmrCOMMAND_RESET_FROM_ISR: \
		prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_RESET_FROMISR : PSF_EVENT_TIMER_RESET_FROMISR_FAILED, (uint32_t)tmr, xOptionalValue); \
		break; \
	case tmrCOMMAND_STOP_FROM_ISR: \
		prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_STOP_FROMISR : PSF_EVENT_TIMER_STOP_FROMISR_FAILED, (uint32_t)tmr, xOptionalValue); \
		break; \
	case tmrCOMMAND_CHANGE_PERIOD_FROM_ISR: \
		prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR : PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED, (uint32_t)tmr, xOptionalValue); \
		break;
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */
#define traceTIMER_COMMAND_SEND_8_0_CASES(tmr) 
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_8_X || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */

/* Note that xCommandID can never be tmrCOMMAND_EXECUTE_CALLBACK (-1) since the trace macro is not called in that case */
#undef traceTIMER_COMMAND_SEND
#define traceTIMER_COMMAND_SEND(tmr, xCommandID, xOptionalValue, xReturn) \
	switch(xCommandID) \
	{ \
		case tmrCOMMAND_START: \
			prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_START : PSF_EVENT_TIMER_START_FAILED, (uint32_t)tmr); \
			break; \
		case tmrCOMMAND_STOP: \
			prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_STOP : PSF_EVENT_TIMER_STOP_FAILED, (uint32_t)tmr); \
			break; \
		case tmrCOMMAND_CHANGE_PERIOD: \
			prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD : PSF_EVENT_TIMER_CHANGEPERIOD_FAILED, (uint32_t)tmr, xOptionalValue); \
			break; \
		case tmrCOMMAND_DELETE: \
			prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_DELETE : PSF_EVENT_TIMER_DELETE_FAILED, (uint32_t)tmr); \
			break; \
		traceTIMER_COMMAND_SEND_8_0_CASES(tmr) \
	}

#undef tracePEND_FUNC_CALL
#define tracePEND_FUNC_CALL(func, arg1, arg2, ret) \
	prvTraceStoreEvent1((ret == pdPASS) ? PSF_EVENT_TIMER_PENDFUNCCALL : PSF_EVENT_TIMER_PENDFUNCCALL_FAILED, (uint32_t)func);

#undef tracePEND_FUNC_CALL_FROM_ISR
#define tracePEND_FUNC_CALL_FROM_ISR(func, arg1, arg2, ret) \
	prvTraceStoreEvent1((ret == pdPASS) ? PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR : PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR_FAILED, (uint32_t)func);

#undef traceEVENT_GROUP_CREATE
#define traceEVENT_GROUP_CREATE(eg) \
	prvTraceStoreEvent1(PSF_EVENT_EVENTGROUP_CREATE, (uint32_t)eg);

#undef traceEVENT_GROUP_DELETE
#define traceEVENT_GROUP_DELETE(eg) \
	prvTraceStoreEvent1(PSF_EVENT_EVENTGROUP_DELETE, (uint32_t)eg); \
	prvTraceDeleteSymbol(eg);

#undef traceEVENT_GROUP_CREATE_FAILED
#define traceEVENT_GROUP_CREATE_FAILED() \
	prvTraceStoreEvent0(PSF_EVENT_EVENTGROUP_CREATE_FAILED);

#undef traceEVENT_GROUP_SYNC_BLOCK
#define traceEVENT_GROUP_SYNC_BLOCK(eg, bitsToSet, bitsToWaitFor) \
	prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_SYNC_BLOCK, (uint32_t)eg, bitsToWaitFor);

#undef traceEVENT_GROUP_SYNC_END
#define traceEVENT_GROUP_SYNC_END(eg, bitsToSet, bitsToWaitFor, wasTimeout) \
	prvTraceStoreEvent2((wasTimeout != pdTRUE) ? PSF_EVENT_EVENTGROUP_SYNC : PSF_EVENT_EVENTGROUP_SYNC_FAILED, (uint32_t)eg, bitsToWaitFor);

#undef traceEVENT_GROUP_WAIT_BITS_BLOCK
#define traceEVENT_GROUP_WAIT_BITS_BLOCK(eg, bitsToWaitFor) \
	prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK, (uint32_t)eg, bitsToWaitFor);

#undef traceEVENT_GROUP_WAIT_BITS_END
#define traceEVENT_GROUP_WAIT_BITS_END(eg, bitsToWaitFor, wasTimeout) \
	prvTraceStoreEvent2((wasTimeout != pdTRUE) ? PSF_EVENT_EVENTGROUP_WAITBITS : PSF_EVENT_EVENTGROUP_WAITBITS_FAILED, (uint32_t)eg, bitsToWaitFor);

#undef traceEVENT_GROUP_CLEAR_BITS
#define traceEVENT_GROUP_CLEAR_BITS(eg, bitsToClear) \
	prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_CLEARBITS, (uint32_t)eg, bitsToClear);

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(eg, bitsToClear) \
	prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR, (uint32_t)eg, bitsToClear);

#undef traceEVENT_GROUP_SET_BITS
#define traceEVENT_GROUP_SET_BITS(eg, bitsToSet) \
	prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_SETBITS, (uint32_t)eg, bitsToSet);

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR
#define traceEVENT_GROUP_SET_BITS_FROM_ISR(eg, bitsToSet) \
	prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_SETBITS_FROMISR, (uint32_t)eg, bitsToSet);

#undef traceTASK_NOTIFY_TAKE
#if (TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X)
#define traceTASK_NOTIFY_TAKE() \
	if (pxCurrentTCB->ucNotifyState == taskNOTIFICATION_RECEIVED) \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_TAKE, (uint32_t)pxCurrentTCB, xTicksToWait); \
	else \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_TAKE_FAILED, (uint32_t)pxCurrentTCB, xTicksToWait);
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */
#define traceTASK_NOTIFY_TAKE() \
	if (pxCurrentTCB->eNotifyState == eNotified) \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_TAKE, (uint32_t)pxCurrentTCB, xTicksToWait); \
	else \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_TAKE_FAILED, (uint32_t)pxCurrentTCB, xTicksToWait);
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */

#undef traceTASK_NOTIFY_TAKE_BLOCK
#define traceTASK_NOTIFY_TAKE_BLOCK() \
	prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_TAKE_BLOCK, (uint32_t)pxCurrentTCB, xTicksToWait);

#undef traceTASK_NOTIFY_WAIT
#if (TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X)
#define traceTASK_NOTIFY_WAIT() \
	if (pxCurrentTCB->ucNotifyState == taskNOTIFICATION_RECEIVED) \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_WAIT, (uint32_t)pxCurrentTCB, xTicksToWait); \
	else \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_WAIT_FAILED, (uint32_t)pxCurrentTCB, xTicksToWait);
#else /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */
#define traceTASK_NOTIFY_WAIT() \
	if (pxCurrentTCB->eNotifyState == eNotified) \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_WAIT, (uint32_t)pxCurrentTCB, xTicksToWait); \
	else \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_WAIT_FAILED, (uint32_t)pxCurrentTCB, xTicksToWait);
#endif /* TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_X */

#undef traceTASK_NOTIFY_WAIT_BLOCK
#define traceTASK_NOTIFY_WAIT_BLOCK() \
	prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK, (uint32_t)pxCurrentTCB, xTicksToWait);

#undef traceTASK_NOTIFY
#define traceTASK_NOTIFY() \
	prvTraceStoreEvent1(PSF_EVENT_TASK_NOTIFY, (uint32_t)xTaskToNotify);

#undef traceTASK_NOTIFY_FROM_ISR
#define traceTASK_NOTIFY_FROM_ISR() \
	prvTraceStoreEvent1(PSF_EVENT_TASK_NOTIFY_FROM_ISR, (uint32_t)xTaskToNotify);
	
#undef traceTASK_NOTIFY_GIVE_FROM_ISR
#define traceTASK_NOTIFY_GIVE_FROM_ISR() \
	prvTraceStoreEvent1(PSF_EVENT_TASK_NOTIFY_GIVE_FROM_ISR, (uint32_t)xTaskToNotify);

#undef traceQUEUE_REGISTRY_ADD
#define traceQUEUE_REGISTRY_ADD(object, name) \
	prvTraceSaveSymbol(object, (const char*)name); \
	prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, name, object);

#endif /*#if TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING  */

#else /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
	
	/* when recorder disabled */
	#define vTraceSetQueueName(object, name)
	#define vTraceSetSemaphoreName(object, name)
	#define vTraceSetMutexName(object, name)

	#define vTraceExcludeQueue(handle)
	#define vTraceExcludeSemaphore(handle)
	#define vTraceExcludeMutex(handle)
	#define vTraceExcludeTimer(handle)
	#define vTraceExcludeEventGroup(handle)
	#define vTraceExcludeDelays()

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
