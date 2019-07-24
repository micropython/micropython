/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcSnapshotRecorder.c
 *
 * The generic core of the trace recorder's snapshot mode.
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
 * Tabs are used for indent in this file (1 tab = 4 spaces)
 *
 * Copyright Percepio AB, 2017.
 * www.percepio.com
 ******************************************************************************/

#include "trcRecorder.h"

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#if ((TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_INCR) || (TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_DECR))
	#error "CUSTOM timestamping mode is not (yet) supported in snapshot mode!"
#endif

/* DO NOT CHANGE */
#define TRACE_MINOR_VERSION 5
#if (TRC_CFG_INCLUDE_ISR_TRACING == 1)
static traceHandle isrstack[TRC_CFG_MAX_ISR_NESTING];
int32_t isPendingContextSwitch = 0;
#endif /* (TRC_CFG_INCLUDE_ISR_TRACING == 1) */

#if !defined TRC_CFG_INCLUDE_READY_EVENTS || TRC_CFG_INCLUDE_READY_EVENTS == 1
static int readyEventsEnabled = 1;
#endif /*!defined TRC_CFG_INCLUDE_READY_EVENTS || TRC_CFG_INCLUDE_READY_EVENTS == 1*/
/*******************************************************************************
 * uiTraceTickCount
 *
 * This variable is should be updated by the Kernel tick interrupt. This does
 * not need to be modified when developing a new timer port. It is preferred to
 * keep any timer port changes in the HWTC macro definitions, which typically
 * give sufficient flexibility.
 ******************************************************************************/
uint32_t uiTraceTickCount = 0;

uint32_t trace_disable_timestamp = 0;
static uint32_t last_timestamp = 0;
/* Flag that shows if inside a critical section of the recorder */
volatile int recorder_busy = 0;

/* Holds the value set by vTraceSetFrequency */
uint32_t timestampFrequency = 0;

/* The last error message of the recorder. NULL if no error message. */
const char* traceErrorMessage = NULL;


int8_t nISRactive = 0;
traceHandle handle_of_last_logged_task = 0;
uint8_t inExcludedTask = 0;

extern uint8_t inExcludedTask;
extern int8_t nISRactive;
extern traceHandle handle_of_last_logged_task;

/* Called when the recorder is stopped, set by vTraceSetStopHook. */
TRACE_STOP_HOOK vTraceStopHookPtr = (TRACE_STOP_HOOK)0;

/*************** Private Functions *******************************************/
static void prvStrncpy(char* dst, const char* src, uint32_t maxLength);
static uint8_t prvTraceGetObjectState(uint8_t objectclass, traceHandle id); 
static void prvTraceGetChecksum(const char *pname, uint8_t* pcrc, uint8_t* plength); 
static void* prvTraceNextFreeEventBufferSlot(void); 
static uint16_t prvTraceGetDTS(uint16_t param_maxDTS);
static traceString prvTraceOpenSymbol(const char* name, traceString userEventChannel);
static void prvTraceUpdateCounters(void);

#if (TRC_CFG_SNAPSHOT_MODE == TRC_SNAPSHOT_MODE_RING_BUFFER)
static void prvCheckDataToBeOverwrittenForMultiEntryEvents(uint8_t nEntries);
#endif

static traceString prvTraceCreateSymbolTableEntry(const char* name,
										 uint8_t crc6,
										 uint8_t len,
										 traceString channel);

static traceString prvTraceLookupSymbolTableEntry(const char* name,
										 uint8_t crc6,
										 uint8_t len,
										 traceString channel);


#if (TRC_CFG_INCLUDE_ISR_TRACING == 0)
/* ISR tracing is turned off */
void prvTraceIncreaseISRActive(void);
void prvTraceDecreaseISRActive(void);
#endif /*(TRC_CFG_INCLUDE_ISR_TRACING == 0)*/
										 
#if (TRC_CFG_USE_16BIT_OBJECT_HANDLES == 1)
static uint8_t prvTraceGet8BitHandle(traceHandle handle);
#else
#define prvTraceGet8BitHandle(x) ((uint8_t)x)
#endif


#if (TRC_CFG_INCLUDE_MEMMANG_EVENTS == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
 /* Current heap usage. Always updated. */
 static uint32_t heapMemUsage = 0;
#endif

#if (TRC_CFG_SCHEDULING_ONLY == 0)
static uint32_t prvTraceGetParam(uint32_t, uint32_t);
#endif

/*******************************************************************************
 * prvTraceInitTraceData
 *
 * Allocates and initializes the recorder data structure, based on the constants
 * in trcConfig.h. This allows for allocating the data on the heap, instead of
 * using a static declaration.
 ******************************************************************************/
static void prvTraceInitTraceData(void);

/*******************************************************************************
 * prvTracePortGetTimeStamp
 *
 * Returns the current time based on the HWTC macros which provide a hardware
 * isolation layer towards the hardware timer/counter.
 *
 * The HWTC macros and prvTracePortGetTimeStamp is the main porting issue
 * or the trace recorder library. Typically you should not need to change
 * the code of prvTracePortGetTimeStamp if using the HWTC macros.
 *
 ******************************************************************************/
void prvTracePortGetTimeStamp(uint32_t *puiTimestamp);

static void prvTraceTaskInstanceFinish(int8_t direct);

#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1))
static void vTracePrintF_Helper(traceString eventLabel, const char* formatStr, va_list vl);

#if (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)
static void vTraceUBData_Helper(traceUBChannel channelPair, va_list vl);
static void prvTraceUBHelper1(traceUBChannel channel, traceString eventLabel, traceString formatLabel, va_list vl);
static void prvTraceUBHelper2(traceUBChannel channel, uint32_t* data, uint32_t noOfSlots);
#endif /*(TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)*/
#endif /* ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1)) */

/********* Public Functions **************************************************/

uint16_t uiIndexOfObject(traceHandle objecthandle, uint8_t objectclass);

/*******************************************************************************
 * prvTraceError
 *
 * Called by various parts in the recorder. Stops the recorder and stores a
 * pointer to an error message, which is printed by the monitor task.
 ******************************************************************************/
void prvTraceError(const char* msg);

/******************************************************************************
* vTraceEnable(int startOption) - snapshot mode
*
* Initializes and optionally starts the trace, depending on the start option.
* To use the trace recorder, the startup must call vTraceEnable before any RTOS
* calls are made (including "create" calls). Three start options are provided:
* 
* TRC_START: Starts the tracing directly. In snapshot mode this allows for 
* starting the trace at any point in your code, assuming vTraceEnable(TRC_INIT)
* has been called in the startup.
* Can also be used for streaming without Tracealyzer control, e.g. to a local
* flash file system (assuming such a "stream port", see trcStreamingPort.h).
* 
* TRC_INIT: Initializes the trace recorder, but does not start the tracing.
* In snapshot mode, this must be followed by a vTraceEnable(TRC_START) sometime
* later.
*
* Usage examples, in snapshot mode:
* 
* Snapshot trace, from startup:
* 	<board init>
* 	vTraceEnable(TRC_START);
* 	<RTOS init>
*
* Snapshot trace, from a later point:
* 	<board init>
* 	vTraceEnable(TRC_INIT);
* 	<RTOS init>
* 	...
* 	vTraceEnable(TRC_START); // e.g., in task context, at some relevant event
* 
*
* Note: See other implementation of vTraceEnable in trcStreamingRecorder.c
******************************************************************************/
void vTraceEnable(int startOption)
{
	prvTraceInitTraceData();
	
	if (startOption == TRC_START)
	{
		vTraceStart();
	}
	else if (startOption == TRC_START_AWAIT_HOST)
	{
		prvTraceError("vTraceEnable(TRC_START_AWAIT_HOST) not allowed in Snapshot mode");
	}
	else if (startOption != TRC_INIT)
	{
		prvTraceError("Unexpected argument to vTraceEnable (snapshot mode)");
	}	
}

/*******************************************************************************
 * vTraceSetRecorderDataBuffer
 *
 * If custom allocation is used, this function must be called so the recorder
 * library knows where to save the trace data.
 ******************************************************************************/
#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM)
void vTraceSetRecorderDataBuffer(void* pRecorderData)
{
	TRACE_ASSERT(pRecorderData != NULL, "vTraceSetRecorderDataBuffer, pRecorderData == NULL", TRC_UNUSED);
	RecorderDataPtr = pRecorderData;
}
#endif

/*******************************************************************************
 * vTraceSetStopHook
 *
 * Sets a function to be called when the recorder is stopped. This can be used
 * to save the trace to a file system, if available. 
 ******************************************************************************/
void vTraceSetStopHook(TRACE_STOP_HOOK stopHookFunction)
{
	vTraceStopHookPtr = stopHookFunction;
}

/*******************************************************************************
 * vTraceClear
 *
 * Resets the recorder. Only necessary if a restart is desired - this is not
 * needed in the startup initialization.
 ******************************************************************************/
void vTraceClear(void)
{
	TRACE_ALLOC_CRITICAL_SECTION();
	trcCRITICAL_SECTION_BEGIN();
	RecorderDataPtr->absTimeLastEventSecond = 0;
	RecorderDataPtr->absTimeLastEvent = 0;
	RecorderDataPtr->nextFreeIndex = 0;
	RecorderDataPtr->numEvents = 0;
	RecorderDataPtr->bufferIsFull = 0;
	traceErrorMessage = NULL;
	RecorderDataPtr->internalErrorOccured = 0;
	(void)memset(RecorderDataPtr->eventData, 0, RecorderDataPtr->maxEvents * 4);
	handle_of_last_logged_task = 0;
	trcCRITICAL_SECTION_END();
}

/*******************************************************************************
 * uiTraceStart
 *
 * Starts the recorder. The recorder will not be started if an error has been
 * indicated using prvTraceError, e.g. if any of the Nx constants in trcConfig.h
 * has a too small value (TRC_CFG_NTASK, TRC_CFG_NQUEUE, etc).
 *
 * Returns 1 if the recorder was started successfully.
 * Returns 0 if the recorder start was prevented due to a previous internal
 * error. In that case, check xTraceGetLastError to get the error message.
 * Any error message is also presented when opening a trace file.
 *
 * This function is obsolete, but has been saved for backwards compatibility.
 * We recommend using vTraceEnable instead.
 ******************************************************************************/
uint32_t uiTraceStart(void)
{
	traceHandle handle;
	TRACE_ALLOC_CRITICAL_SECTION();

	handle = 0;

	if (RecorderDataPtr == NULL)
	{
		prvTraceError("RecorderDataPtr is NULL. Call vTraceInitTraceData() before starting trace.");
		return 0;
	}
	
	if (RecorderDataPtr->recorderActive == 1)
		return 1; /* Already running */

	if (traceErrorMessage == NULL)
	{
		trcCRITICAL_SECTION_BEGIN();
		RecorderDataPtr->recorderActive = 1;

		handle = TRACE_GET_TASK_NUMBER(TRACE_GET_CURRENT_TASK());
		if (handle == 0)
		{
			/* This occurs if the scheduler is not yet started.
			This creates a dummy "(startup)" task entry internally in the
			recorder */
			handle = prvTraceGetObjectHandle(TRACE_CLASS_TASK);
			prvTraceSetObjectName(TRACE_CLASS_TASK, handle, "(startup)");

			prvTraceSetPriorityProperty(TRACE_CLASS_TASK, handle, 0);
		}

		prvTraceStoreTaskswitch(handle); /* Register the currently running task */
		trcCRITICAL_SECTION_END();
	}

	return RecorderDataPtr->recorderActive;
}

/*******************************************************************************
 * vTraceStart
 *
 * Starts the recorder. The recorder will not be started if an error has been
 * indicated using prvTraceError, e.g. if any of the Nx constants in trcConfig.h
 * has a too small value (TRC_CFG_NTASK, TRC_CFG_NQUEUE, etc).
 *
 * This function is obsolete, but has been saved for backwards compatibility.
 * We recommend using vTraceEnable instead.
 ******************************************************************************/
void vTraceStart(void)
{
	(void)uiTraceStart();
}

/*******************************************************************************
 * vTraceStop
 *
 * Stops the recorder. The recording can be resumed by calling vTraceStart.
 * This does not reset the recorder. Use vTraceClear if that is desired.
 ******************************************************************************/
void vTraceStop(void)
{
	RecorderDataPtr->recorderActive = 0;

	if (vTraceStopHookPtr != (TRACE_STOP_HOOK)0)
	{
		(*vTraceStopHookPtr)();			/* An application call-back function. */
	}
}

/*******************************************************************************
 * xTraceGetLastError
 *
 * Gives the last error message, if any. NULL if no error message is stored.
 * Any error message is also presented when opening a trace file.
 ******************************************************************************/
const char* xTraceGetLastError(void)
{
	return traceErrorMessage;
}

/*******************************************************************************
* vTraceClearError
*
* Removes any previous error message generated by recorder calling prvTraceError.
* By calling this function, it may be possible to start/restart the trace
* despite errors in the recorder, but there is no guarantee that the trace
* recorder will work correctly in that case, depending on the type of error.
******************************************************************************/
void vTraceClearError(void)
{
	traceErrorMessage = NULL;
	if (RecorderDataPtr != NULL)
	{
		RecorderDataPtr->internalErrorOccured = 0;
	}
}

/*******************************************************************************
 * xTraceGetTraceBuffer
 *
 * Returns a pointer to the recorder data structure. Use this together with
 * uiTraceGetTraceBufferSize if you wish to implement an own store/upload
 * solution, e.g., in case a debugger connection is not available for uploading
 * the data.
 ******************************************************************************/
void* xTraceGetTraceBuffer(void)
{
	return RecorderDataPtr;
}

/*******************************************************************************
 * uiTraceGetTraceBufferSize
 *
 * Gets the size of the recorder data structure. For use together with
 * vTraceGetTraceBuffer if you wish to implement an own store/upload solution,
 * e.g., in case a debugger connection is not available for uploading the data.
 ******************************************************************************/
uint32_t uiTraceGetTraceBufferSize(void)
{
	return sizeof(RecorderDataType);
}

/******************************************************************************
 * prvTraceTaskInstanceFinish
 *
 * Private common function for the vTraceTaskInstanceFinishXXX functions.
 * 
 *****************************************************************************/
static void prvTraceTaskInstanceFinish(int8_t direct)
{
	TaskInstanceStatusEvent* tis;
	uint8_t dts45;

	TRACE_ALLOC_CRITICAL_SECTION();

	trcCRITICAL_SECTION_BEGIN();
	if (RecorderDataPtr->recorderActive && (! inExcludedTask || nISRactive) && handle_of_last_logged_task)
	{
		dts45 = (uint8_t)prvTraceGetDTS(0xFF);
		tis = (TaskInstanceStatusEvent*) prvTraceNextFreeEventBufferSlot();
		if (tis != NULL)
		{
			if (direct == 0)
				tis->type = TASK_INSTANCE_FINISHED_NEXT_KSE;
			else
				tis->type = TASK_INSTANCE_FINISHED_DIRECT;

			tis->dts = dts45;
			prvTraceUpdateCounters();
		}
	}
	trcCRITICAL_SECTION_END();
}

/******************************************************************************
 * vTraceInstanceFinishedNext(void)
 *
 * Marks the current task instance as finished on the next kernel call.
 *
 * If that kernel call is blocking, the instance ends after the blocking event
 * and the corresponding return event is then the start of the next instance.
 * If the kernel call is not blocking, the viewer instead splits the current
 * fragment right before the kernel call, which makes this call the first event
 * of the next instance.
 *
 * See also TRC_CFG_USE_IMPLICIT_IFE_RULES in trcConfig.h
 *
 * Example:
 *
 *		while(1)
 *		{
 *			xQueueReceive(CommandQueue, &command, timeoutDuration);
 *			processCommand(command);
 *          vTraceInstanceFinishedNext();
 *		}
 *
 * Note: This is only supported in Tracealyzer tools v2.7 or later
 *
 *****************************************************************************/
void vTraceInstanceFinishedNext(void)
{
    prvTraceTaskInstanceFinish(0);
}

/******************************************************************************
 * vTraceInstanceFinishedNow(void)
 *
 * Marks the current task instance as finished at this very instant.
 * This makes the viewer to splits the current fragment at this point and begin
 * a new actor instance.
 *
 * See also TRC_CFG_USE_IMPLICIT_IFE_RULES in trcConfig.h
 *
 * Example:
 *
 *		This example will generate two instances for each loop iteration.
 *		The first instance ends at vTraceInstanceFinishedNow(), while the second
 *      instance ends at the next xQueueReceive call.
 *
 *		while (1)
 *		{
 *          xQueueReceive(CommandQueue, &command, timeoutDuration);
 *			ProcessCommand(command);
 *			vTraceInstanceFinishedNow();
 *			DoSometingElse();
 *          vTraceInstanceFinishedNext();
 *      }
 *
 * Note: This is only supported in Tracealyzer tools v2.7 or later
 *
 *****************************************************************************/
void vTraceInstanceFinishedNow(void)
{
	prvTraceTaskInstanceFinish(1);
}

/*******************************************************************************
 * Interrupt recording functions
 ******************************************************************************/

#if (TRC_CFG_INCLUDE_ISR_TRACING == 1)

/*******************************************************************************
 * xTraceSetISRProperties
 *
 * Stores a name and priority level for an Interrupt Service Routine, to allow
 * for better visualization. Returns a traceHandle used by vTraceStoreISRBegin. 
 *
 * Example:
 *	 #define PRIO_ISR_TIMER1 3 // the hardware priority of the interrupt
 *	 ...
 *	 traceHandle Timer1Handle = xTraceSetISRProperties("ISRTimer1", PRIO_ISR_TIMER1);
 *	 ...
 *	 void ISR_handler()
 *	 {
 *		 vTraceStoreISRBegin(Timer1Handle);
 *		 ...
 *		 vTraceStoreISREnd(0);
 *	 }
 *
 ******************************************************************************/
 traceHandle xTraceSetISRProperties(const char* name, uint8_t priority)
{
	static traceHandle handle = 0;
	handle++;
	TRACE_ASSERT(handle <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[TRACE_CLASS_ISR], "xTraceSetISRProperties: Invalid value for handle", 0);
	TRACE_ASSERT(name != NULL, "xTraceSetISRProperties: name == NULL", 0);

	prvTraceSetObjectName(TRACE_CLASS_ISR, handle, name);
	prvTraceSetPriorityProperty(TRACE_CLASS_ISR, handle, priority);
	
	return handle;
}

/*******************************************************************************
 * vTraceStoreISRBegin
 *
 * Registers the beginning of an Interrupt Service Routine, using a traceHandle
 * provided by xTraceSetISRProperties.
 *
 * Example:
 *	 #define PRIO_ISR_TIMER1 3 // the hardware priority of the interrupt
 *	 ...
 *	 traceHandle Timer1Handle = xTraceSetISRProperties("ISRTimer1", PRIO_ISR_TIMER1);
 *	 ...
 *	 void ISR_handler()
 *	 {
 *		 vTraceStoreISRBegin(Timer1Handle);
 *		 ...
 *		 vTraceStoreISREnd(0);
 *	 }
 *
 ******************************************************************************/
void vTraceStoreISRBegin(traceHandle handle)
{
	TRACE_ALLOC_CRITICAL_SECTION();

	if (recorder_busy)
	{
		/*************************************************************************
		* This occurs if an ISR calls a trace function, preempting a previous
		* trace call that is being processed in a different ISR or task. 
		* If this occurs, there is probably a problem in the definition of the
		* recorder's internal critical sections (TRACE_ENTER_CRITICAL_SECTION and
		* TRACE_EXIT_CRITICAL_SECTION). They must disable the RTOS tick interrupt
		* and any other ISRs that calls the trace recorder directly or via
		* traced kernel functions. The ARM port disables all interrupts using the
		* PRIMASK register to avoid this issue.
		*************************************************************************/
		prvTraceError("vTraceStoreISRBegin - recorder busy! See code comment.");
		return;
	}
	trcCRITICAL_SECTION_BEGIN();
	
	if (RecorderDataPtr->recorderActive && handle_of_last_logged_task)
	{
		uint16_t dts4;
		
		TRACE_ASSERT(handle <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[TRACE_CLASS_ISR], "vTraceStoreISRBegin: Invalid value for handle", TRC_UNUSED);
		
		dts4 = (uint16_t)prvTraceGetDTS(0xFFFF);

		if (RecorderDataPtr->recorderActive) /* Need to repeat this check! */
		{
			if (nISRactive < TRC_CFG_MAX_ISR_NESTING)
			{
				TSEvent* ts;
				uint8_t hnd8 = prvTraceGet8BitHandle(handle);
				isrstack[nISRactive] = handle;
				nISRactive++;
				ts = (TSEvent*)prvTraceNextFreeEventBufferSlot();
				if (ts != NULL)
				{
					ts->type = TS_ISR_BEGIN;
					ts->dts = dts4;
					ts->objHandle = hnd8;
					prvTraceUpdateCounters();
				}
			}
			else
			{
				/* This should not occur unless something is very wrong */
				prvTraceError("Too many nested interrupts!");
			}
		}
	}
	trcCRITICAL_SECTION_END();
}

/*******************************************************************************
 * vTraceStoreISREnd
 *
 * Registers the end of an Interrupt Service Routine.
 *
 * The parameter pendingISR indicates if the interrupt has requested a
 * task-switch (= 1), e.g., by signaling a semaphore. Otherwise (= 0) the 
 * interrupt is assumed to return to the previous context.
 *
 * Example:
 *	 #define PRIO_OF_ISR_TIMER1 3 // the hardware priority of the interrupt
 *	 traceHandle traceHandleIsrTimer1 = 0; // The ID set by the recorder
 *	 ...
 *	 traceHandleIsrTimer1 = xTraceSetISRProperties("ISRTimer1", PRIO_OF_ISR_TIMER1);
 *	 ...
 *	 void ISR_handler()
 *	 {
 *		 vTraceStoreISRBegin(traceHandleIsrTimer1);
 *		 ...
 *		 vTraceStoreISREnd(0);
 *	 }
 *
 ******************************************************************************/
void vTraceStoreISREnd(int pendingISR)
{
	TSEvent* ts;
	uint16_t dts5;
	uint8_t hnd8 = 0, type = 0;
	
	TRACE_ALLOC_CRITICAL_SECTION();

	if (! RecorderDataPtr->recorderActive ||  ! handle_of_last_logged_task)
	{
		return;
	}

	if (recorder_busy)
	{
		/*************************************************************************
		* This occurs if an ISR calls a trace function, preempting a previous
		* trace call that is being processed in a different ISR or task. 
		* If this occurs, there is probably a problem in the definition of the
		* recorder's internal critical sections (TRACE_ENTER_CRITICAL_SECTION and
		* TRACE_EXIT_CRITICAL_SECTION). They must disable the RTOS tick interrupt
		* and any other ISRs that calls the trace recorder directly or via
		* traced kernel functions. The ARM port disables all interrupts using the
		* PRIMASK register to avoid this issue.
		*************************************************************************/
		prvTraceError("vTraceStoreISREnd - recorder busy! See code comment.");
		return;
	}
	
	if (nISRactive == 0)
	{
		prvTraceError("Unmatched call to vTraceStoreISREnd (nISRactive == 0, expected > 0)");
		return;
	}

	trcCRITICAL_SECTION_BEGIN();
	isPendingContextSwitch |= pendingISR;	/* Is there a pending context switch right now? */
	nISRactive--;
	if (nISRactive > 0)
	{
		/* Return to another ISR */
		type = TS_ISR_RESUME;
		hnd8 = prvTraceGet8BitHandle(isrstack[nISRactive - 1]); /* isrstack[nISRactive] is the handle of the ISR we're currently exiting. isrstack[nISRactive - 1] is the handle of the ISR that was executing previously. */
	}
	else if ((isPendingContextSwitch == 0) || (prvTraceIsSchedulerSuspended()))	
	{
		/* Return to interrupted task, if no context switch will occur in between. */
		type = TS_TASK_RESUME;
		hnd8 = prvTraceGet8BitHandle(handle_of_last_logged_task);
	}

	if (type != 0)
	{
		dts5 = (uint16_t)prvTraceGetDTS(0xFFFF);
		ts = (TSEvent*)prvTraceNextFreeEventBufferSlot();
		if (ts != NULL)
		{
			ts->type = type;
			ts->objHandle = hnd8;
			ts->dts = dts5;
			prvTraceUpdateCounters();
		}
	}

	trcCRITICAL_SECTION_END();
}

#else

/* ISR tracing is turned off */
void prvTraceIncreaseISRActive(void)
{
	if (RecorderDataPtr->recorderActive && handle_of_last_logged_task)
		nISRactive++;
}

void prvTraceDecreaseISRActive(void)
{
	if (RecorderDataPtr->recorderActive && handle_of_last_logged_task)
		nISRactive--;
}
#endif /* (TRC_CFG_INCLUDE_ISR_TRACING == 1)*/

/********************************************************************************/
/* User Event functions															*/
/********************************************************************************/

#define MAX_ARG_SIZE (4+32)

#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1))
static uint8_t writeInt8(void * buffer, uint8_t i, uint8_t value)
{
	TRACE_ASSERT(buffer != NULL, "writeInt8: buffer == NULL", 0);

	if (i >= MAX_ARG_SIZE)
	{
		return 255;
	}

	((uint8_t*)buffer)[i] = value;

	if (i + 1 > MAX_ARG_SIZE)
	{
		return 255;
	}

	return ((uint8_t) (i + 1));
}
#endif

#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1))
static uint8_t writeInt16(void * buffer, uint8_t i, uint16_t value)
{
	TRACE_ASSERT(buffer != NULL, "writeInt16: buffer == NULL", 0);

	/* Align to multiple of 2 */
	while ((i % 2) != 0)
	{
		if (i >= MAX_ARG_SIZE)
		{
			return 255;
		}

		((uint8_t*)buffer)[i] = 0;
		i++;
	}

	if (i + 2 > MAX_ARG_SIZE)
	{
		return 255;
	}

	((uint16_t*)buffer)[i/2] = value;

	return ((uint8_t) (i + 2));
}
#endif

#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1))
static uint8_t writeInt32(void * buffer, uint8_t i, uint32_t value)
{
	TRACE_ASSERT(buffer != NULL, "writeInt32: buffer == NULL", 0);

	/* A 32 bit value should begin at an even 4-byte address */
	while ((i % 4) != 0)
	{
		if (i >= MAX_ARG_SIZE)
		{
			return 255;
		}

		((uint8_t*)buffer)[i] = 0;
		i++;
	}

	if (i + 4 > MAX_ARG_SIZE)
	{
		return 255;
	}

	((uint32_t*)buffer)[i/4] = value;

	return ((uint8_t) (i + 4));
}
#endif

#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_INCLUDE_FLOAT_SUPPORT))
static uint8_t writeFloat(void * buffer, uint8_t i, float value)
{
	TRACE_ASSERT(buffer != NULL, "writeFloat: buffer == NULL", 0);

	/* A 32 bit value should begin at an even 4-byte address */
	while ((i % 4) != 0)
	{
		if (i >= MAX_ARG_SIZE)
		{
			return 255;
		}

		((uint8_t*)buffer)[i] = 0;
		i++;
	}

	if (i + 4 > MAX_ARG_SIZE)
	{
		return 255;
	}

	((float*)buffer)[i/4] = value;

	return i + 4;
}
#endif

#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_INCLUDE_FLOAT_SUPPORT))
static uint8_t writeDouble(void * buffer, uint8_t i, double value)
{
	uint32_t * dest;
	uint32_t * src = (uint32_t*)&value;

	TRACE_ASSERT(buffer != NULL, "writeDouble: buffer == NULL", 0);

	/* The double is written as two 32 bit values, and should begin at an even
	4-byte address (to avoid having to align with 8 byte) */
	while (i % 4 != 0)
	{
		if (i >= MAX_ARG_SIZE)
		{
			return 255;
		}

		((uint8_t*)buffer)[i] = 0;
		i++;
	}

	if (i + 8 > MAX_ARG_SIZE)
	{
		return 255;
	}

	dest = &(((uint32_t *)buffer)[i/4]);

	dest[0] = src[0];
	dest[1] = src[1];

	return i + 8;
}
#endif

/*******************************************************************************
 * prvTraceUserEventFormat
 *
 * Parses the format string and stores the arguments in the buffer.
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1))
static uint8_t prvTraceUserEventFormat(const char* formatStr, va_list vl, uint8_t* buffer, uint8_t byteOffset)
{
	uint16_t formatStrIndex = 0;
	uint8_t argCounter = 0;
	uint8_t i = byteOffset;

	while (formatStr[formatStrIndex] != '\0')
	{
		if (formatStr[formatStrIndex] == '%')
		{
			argCounter++;

			if (argCounter > 15)
			{
				prvTraceError("vTracePrintF - Too many arguments, max 15 allowed!");
				return 0;
			}

			formatStrIndex++;

			while ((formatStr[formatStrIndex] >= '0' && formatStr[formatStrIndex] <= '9') || formatStr[formatStrIndex] == '#' || formatStr[formatStrIndex] == '.')
				formatStrIndex++;

			if (formatStr[formatStrIndex] != '\0')
			{
				switch (formatStr[formatStrIndex])
				{
					case 'd':	i = writeInt32(	buffer,
												i,
												(uint32_t)va_arg(vl, uint32_t));
								break;
					case 'x':
					case 'X':
					case 'u':	i = writeInt32(	buffer,
												i,
												(uint32_t)va_arg(vl, uint32_t));
								break;
					case 's':	i = writeInt16(	buffer,
												i,
												xTraceRegisterString((char*)va_arg(vl, char*)));
								break;

#if (TRC_CFG_INCLUDE_FLOAT_SUPPORT)
					/* Yes, "double" as type also in the float
					case. This since "float" is promoted into "double"
					by the va_arg stuff. */
					case 'f':	i = writeFloat(	buffer,
												i,
												(float)va_arg(vl, double));
								break;
#else
					/* No support for floats, but attempt to store a float user event
					avoid a possible crash due to float reference. Instead store the
					data on uint_32 format (will not be displayed anyway). This is just
					to keep va_arg and i consistent. */

					case 'f':	i = writeInt32(	buffer,
												i,
												(uint32_t)va_arg(vl, double));
								break;
#endif
					case 'l':
								formatStrIndex++;
								switch (formatStr[formatStrIndex])
								{
#if (TRC_CFG_INCLUDE_FLOAT_SUPPORT)
									case 'f':	i = writeDouble(buffer,
																i,
																(double)va_arg(vl, double));
												break;
#else
									/* No support for floats, but attempt to store a float user event
									avoid a possible crash due to float reference. Instead store the
									data on uint_32 format (will not be displayed anyway). This is just
									to keep va_arg and i consistent. */
									case 'f':	i = writeInt32(	buffer, /* In this case, the value will not be shown anyway */
																i,
																(uint32_t)va_arg(vl, double));

												i = writeInt32(	buffer, /* Do it twice, to write in total 8 bytes */
																i,
																(uint32_t)va_arg(vl, double));
										break;
#endif

								}
								break;
					case 'h':
								formatStrIndex++;
								switch (formatStr[formatStrIndex])
								{
									case 'd':	i = writeInt16(	buffer,
																i,
																(uint16_t)va_arg(vl, uint32_t));
												break;
									case 'u':	i = writeInt16(	buffer,
																i,
																(uint16_t)va_arg(vl, uint32_t));
												break;
								}
								break;
					case 'b':
								formatStrIndex++;
								switch (formatStr[formatStrIndex])
								{
									case 'd':	i = writeInt8(	buffer,
																i,
																(uint8_t)va_arg(vl, uint32_t));
												break;

									case 'u':	i = writeInt8(	buffer,
																i,
																(uint8_t)va_arg(vl, uint32_t));
												break;
								}
								break;
				}
			}
			else
				break;
		}
		formatStrIndex++;
		if (i == 255)
		{
			prvTraceError("vTracePrintF - Too large arguments, max 32 byte allowed!");
			return 0;
		}
	}
	return (uint8_t)(i+3)/4;
}
#endif

/*******************************************************************************
 * prvTraceClearChannelBuffer
 *
 * Clears a number of items in the channel buffer, starting from nextSlotToWrite.
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1))
static void prvTraceClearChannelBuffer(uint32_t count)
{
	uint32_t slots;

	TRACE_ASSERT(TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE >= count,
		"prvTraceClearChannelBuffer: TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE is too small to handle this event.", TRC_UNUSED);

	/* Check if we're close to the end of the buffer */
	if (RecorderDataPtr->userEventBuffer.nextSlotToWrite + count > TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE)
	{
		slots = TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE - RecorderDataPtr->userEventBuffer.nextSlotToWrite; /* Number of slots before end of buffer */
		(void)memset(&RecorderDataPtr->userEventBuffer.channelBuffer[RecorderDataPtr->userEventBuffer.nextSlotToWrite], 0, slots);
		(void)memset(&RecorderDataPtr->userEventBuffer.channelBuffer[0], 0, (count - slots));
	}
	else
		(void)memset(&RecorderDataPtr->userEventBuffer.channelBuffer[RecorderDataPtr->userEventBuffer.nextSlotToWrite], 0, count);
}
#endif

/*******************************************************************************
 * prvTraceCopyToDataBuffer
 *
 * Copies a number of items to the data buffer, starting from nextSlotToWrite.
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1))
static void prvTraceCopyToDataBuffer(uint32_t* data, uint32_t count)
{
	uint32_t slots;
	
	TRACE_ASSERT(data != NULL,
		"prvTraceCopyToDataBuffer: data == NULL.", TRC_UNUSED);
	TRACE_ASSERT(count <= TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE,
		"prvTraceCopyToDataBuffer: TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE is too small to handle this event.", TRC_UNUSED);
	/* Check if we're close to the end of the buffer */
	if (RecorderDataPtr->userEventBuffer.nextSlotToWrite + count > TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE)
	{
		slots = TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE - RecorderDataPtr->userEventBuffer.nextSlotToWrite; /* Number of slots before end of buffer */
		(void)memcpy(&RecorderDataPtr->userEventBuffer.dataBuffer[RecorderDataPtr->userEventBuffer.nextSlotToWrite * 4], data, slots * 4);
		(void)memcpy(&RecorderDataPtr->userEventBuffer.dataBuffer[0], data + slots, (count - slots) * 4);
	}
	else
	{
		(void)memcpy(&RecorderDataPtr->userEventBuffer.dataBuffer[RecorderDataPtr->userEventBuffer.nextSlotToWrite * 4], data, count * 4);
	}
}
#endif

/*******************************************************************************
 * prvTraceUBHelper1
 *
 * Calls on prvTraceUserEventFormat() to do the actual formatting, then goes on
 * to the next helper function.
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1))
static void prvTraceUBHelper1(traceUBChannel channel, traceString eventLabel, traceString formatLabel, va_list vl)
{
	uint32_t data[(3 + MAX_ARG_SIZE) / 4];
	uint8_t byteOffset = 4; /* Need room for timestamp */
	uint8_t noOfSlots;

	if (channel == 0)
	{
		/* We are dealing with an unknown channel format pair */
		byteOffset = (uint8_t)(byteOffset + 4); /* Also need room for channel and format */
		((uint16_t*)data)[2] = eventLabel;
		((uint16_t*)data)[3] = formatLabel;
	}

	noOfSlots = prvTraceUserEventFormat((char*)&(RecorderDataPtr->SymbolTable.symbytes[formatLabel+4]), vl, (uint8_t*)data, byteOffset);

	prvTraceUBHelper2(channel, data, noOfSlots);
}
#endif

/*******************************************************************************
 * prvTraceUBHelper2
 *
 * This function simply copies the data buffer to the actual user event buffer.
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1))
static void prvTraceUBHelper2(traceUBChannel channel, uint32_t* data, uint32_t noOfSlots)
{
	static uint32_t old_timestamp = 0;
	uint32_t old_nextSlotToWrite = 0;
	
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ASSERT(TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE >= noOfSlots, "prvTraceUBHelper2: TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE is too small to handle this event.", TRC_UNUSED);

	trcCRITICAL_SECTION_BEGIN();
	/* Store the timestamp */
	prvTracePortGetTimeStamp(data);

	if (*data < old_timestamp)
	{
		RecorderDataPtr->userEventBuffer.wraparoundCounter++;
	}

	old_timestamp = *data;

	/* Start by erasing any information in the channel buffer */
	prvTraceClearChannelBuffer(noOfSlots);

	prvTraceCopyToDataBuffer(data, noOfSlots); /* Will wrap around the data if necessary */

	old_nextSlotToWrite = RecorderDataPtr->userEventBuffer.nextSlotToWrite; /* Save the index that we want to write the channel data at when we're done */
	RecorderDataPtr->userEventBuffer.nextSlotToWrite = (RecorderDataPtr->userEventBuffer.nextSlotToWrite + noOfSlots) % TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE; /* Make sure we never end up outside the buffer */

	/* Write to the channel buffer to indicate that this user event is ready to be used */
	if (channel != 0)
	{
		RecorderDataPtr->userEventBuffer.channelBuffer[old_nextSlotToWrite] = channel;
	}
	else
	{
		/* 0xFF indicates that this is not a normal channel id */
		RecorderDataPtr->userEventBuffer.channelBuffer[old_nextSlotToWrite] = (traceUBChannel)0xFF;
	}
	trcCRITICAL_SECTION_END();
}
#endif

/*******************************************************************************
 * xTraceRegisterUBChannel
 *
 * Registers a channel for Separated User Events, i.e., those stored in the
 * separate user event buffer.
 *
 * Note: Only available if TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER is enabled in
 * trcSnapshotConfig.h
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1))
traceUBChannel xTraceRegisterUBChannel(traceString channel, traceString formatStr)
{
	uint8_t i;
	traceUBChannel retVal = 0;
	
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ASSERT(formatStr != 0, "xTraceRegisterChannelFormat: formatStr == 0", (traceUBChannel)0);

	trcCRITICAL_SECTION_BEGIN();
	for (i = 1; i <= TRC_CFG_UB_CHANNELS; i++) /* Size of the channels buffer is TRC_CFG_UB_CHANNELS + 1. Index 0 is unused. */
	{
		if(RecorderDataPtr->userEventBuffer.channels[i].name == 0 && RecorderDataPtr->userEventBuffer.channels[i].defaultFormat == 0)
		{
			/* Found empty slot */
			RecorderDataPtr->userEventBuffer.channels[i].name = channel;
			RecorderDataPtr->userEventBuffer.channels[i].defaultFormat = formatStr;
			retVal = (traceUBChannel)i;
			break;
		}

		if (RecorderDataPtr->userEventBuffer.channels[i].name == channel && RecorderDataPtr->userEventBuffer.channels[i].defaultFormat == formatStr)
		{
			/* Found a match */
			retVal = (traceUBChannel)i;
			break;
		}
	}
	trcCRITICAL_SECTION_END();

	return retVal;
}
#endif

/******************************************************************************
 * vTraceUBData
 *
 * Slightly faster version of vTracePrintF() due to no lookups.
 *
 * Note: This is only available if TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER is enabled in
 * trcConfig.h
 *
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1))
void vTraceUBData(traceUBChannel channelPair, ...)
{
	va_list vl;
	
	TRACE_ASSERT(channelPair != 0, "vTraceUBData: Not a valid traceUBChannel!", TRC_UNUSED);

	va_start(vl, channelPair);
	vTraceUBData_Helper(channelPair, vl);
	va_end(vl);
}
#endif

/* Extracts the channel name and format string from the traceUBChannel, then calls prvTraceUBHelper1. */
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1))
void vTraceUBData_Helper(traceUBChannel channelPair, va_list vl)
{
	traceString channel;
	traceString formatStr;

	TRACE_ASSERT(channelPair != 0, "vTraceUBData_Helper: channelPair == 0", TRC_UNUSED);
	TRACE_ASSERT(channelPair <= TRC_CFG_UB_CHANNELS, "vTraceUBData_Helper: ", TRC_UNUSED);

	channel = RecorderDataPtr->userEventBuffer.channels[channelPair].name;
	formatStr = RecorderDataPtr->userEventBuffer.channels[channelPair].defaultFormat;

	prvTraceUBHelper1(channelPair, channel, formatStr, vl);
}
#endif

/******************************************************************************
 * vTraceUBEvent
 *
 * Slightly faster version of ... due to no lookups.
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1))
void vTraceUBEvent(traceUBChannel channelPair)
{
	uint32_t data[(3 + MAX_ARG_SIZE) / 4];

	TRACE_ASSERT(channelPair != 0, "vTraceUBEvent: channelPair == 0", TRC_UNUSED);
	TRACE_ASSERT(channelPair <= TRC_CFG_UB_CHANNELS, "vTraceUBEvent: ", TRC_UNUSED);

	prvTraceUBHelper2(channelPair, data, 1); /* Only need one slot for timestamp */
}
#endif

/******************************************************************************
 * vTracePrintF
 *
 * Generates User Event with formatted text and data, similar to a "printf".
 * It is very fast compared to a normal "printf" since this function only
 * stores the arguments. The actual formatting is done
 * on the host PC when the trace is displayed in the viewer tool.
 *
 * User Event labels are created using xTraceRegisterString.
 * Example:
 *
 *	 traceString adc_uechannel = xTraceRegisterString("ADC User Events");
 *	 ...
 *	 vTracePrintF(adc_uechannel,
 *				 "ADC channel %d: %lf volts",
 *				 ch, (double)adc_reading/(double)scale);
 *
 * This can be combined into one line, if desired, but this is slower:
 *
 *	 vTracePrintF(xTraceRegisterString("ADC User Events"),
 *				 "ADC channel %d: %lf volts",
 *				 ch, (double)adc_reading/(double)scale);
 *
 * Calling xTraceRegisterString multiple times will not create duplicate entries, but
 * it is of course faster to just do it once, and then keep the handle for later
 * use. If you don't have any data arguments, only a text label/string, it is
 * better to use vTracePrint - it is faster.
 *
 * Format specifiers supported:
 * %d - 32 bit signed integer
 * %u - 32 bit unsigned integer
 * %f - 32 bit float
 * %s - string (is copied to the recorder symbol table)
 * %hd - 16 bit signed integer
 * %hu - 16 bit unsigned integer
 * %bd - 8 bit signed integer
 * %bu - 8 bit unsigned integer
 * %lf - double-precision float (Note! See below...)
 *
 * Up to 15 data arguments are allowed, with a total size of maximum 32 byte.
 * In case this is exceeded, the user event is changed into an error message.
 *
 * The data is stored in trace buffer, and is packed to allow storing multiple
 * smaller data entries in the same 4-byte record, e.g., four 8-bit values.
 * A string requires two bytes, as the symbol table is limited to 64K. Storing
 * a double (%lf) uses two records, so this is quite costly. Use float (%f)
 * unless the higher precision is really necessary.
 *
 * Note that the double-precision float (%lf) assumes a 64 bit double
 * representation. This does not seem to be the case on e.g. PIC24 and PIC32.
 * Before using a %lf argument on a 16-bit MCU, please verify that
 * "sizeof(double)" actually gives 8 as expected. If not, use %f instead.
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1))
void vTracePrintF(traceString eventLabel, const char* formatStr, ...)
{
	va_list vl;

	va_start(vl, formatStr);
	vTracePrintF_Helper(eventLabel, formatStr, vl);
	va_end(vl);
}
#endif

#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1))
void vTracePrintF_Helper(traceString eventLabel, const char* formatStr, va_list vl)
{
#if (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 0)
	uint32_t noOfSlots;
	UserEvent* ue1;
	uint32_t tempDataBuffer[(3 + MAX_ARG_SIZE) / 4];
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ASSERT(formatStr != NULL, "vTracePrintF_Helper: formatStr == NULL", TRC_UNUSED);

	trcCRITICAL_SECTION_BEGIN();

	if (RecorderDataPtr->recorderActive && (! inExcludedTask || nISRactive) && handle_of_last_logged_task)
	{
		/* First, write the "primary" user event entry in the local buffer, but
		let the event type be "EVENT_BEING_WRITTEN" for now...*/

		ue1 = (UserEvent*)(&tempDataBuffer[0]);

		ue1->type = EVENT_BEING_WRITTEN;	 /* Update this as the last step */

		noOfSlots = prvTraceUserEventFormat(formatStr, vl, (uint8_t*)tempDataBuffer, 4);

		/* Store the format string, with a reference to the channel symbol */
		ue1->payload = prvTraceOpenSymbol(formatStr, eventLabel);

		ue1->dts = (uint8_t)prvTraceGetDTS(0xFF);

		 /* prvTraceGetDTS might stop the recorder in some cases... */
		if (RecorderDataPtr->recorderActive)
		{

			/* If the data does not fit in the remaining main buffer, wrap around to
			0 if allowed, otherwise stop the recorder and quit). */
			if (RecorderDataPtr->nextFreeIndex + noOfSlots > RecorderDataPtr->maxEvents)
			{
				#if (TRC_CFG_SNAPSHOT_MODE == TRC_SNAPSHOT_MODE_RING_BUFFER)
				(void)memset(& RecorderDataPtr->eventData[RecorderDataPtr->nextFreeIndex * 4],
						0,
						(RecorderDataPtr->maxEvents - RecorderDataPtr->nextFreeIndex)*4);
				RecorderDataPtr->nextFreeIndex = 0;
				RecorderDataPtr->bufferIsFull = 1;
				#else

				/* Stop recorder, since the event data will not fit in the
				buffer and not circular buffer in this case... */
				vTraceStop();
				#endif
			}

			/* Check if recorder has been stopped (i.e., vTraceStop above) */
			if (RecorderDataPtr->recorderActive)
			{
				/* Check that the buffer to be overwritten does not contain any user
				events that would be partially overwritten. If so, they must be "killed"
				by replacing the user event and following data with NULL events (i.e.,
				using a memset to zero).*/
				#if (TRC_CFG_SNAPSHOT_MODE == TRC_SNAPSHOT_MODE_RING_BUFFER)
				prvCheckDataToBeOverwrittenForMultiEntryEvents((uint8_t)noOfSlots);
				#endif
				/* Copy the local buffer to the main buffer */
				(void)memcpy(& RecorderDataPtr->eventData[RecorderDataPtr->nextFreeIndex * 4],
						tempDataBuffer,
						noOfSlots * 4);

				/* Update the event type, i.e., number of data entries following the
				main USER_EVENT entry (Note: important that this is after the memcpy,
				but within the critical section!)*/
				RecorderDataPtr->eventData[RecorderDataPtr->nextFreeIndex * 4] =
				 (uint8_t) ( USER_EVENT + noOfSlots - 1 );

				/* Update the main buffer event index (already checked that it fits in
				the buffer, so no need to check for wrapping)*/

				RecorderDataPtr->nextFreeIndex += noOfSlots;
				RecorderDataPtr->numEvents += noOfSlots;

				if (RecorderDataPtr->nextFreeIndex >= TRC_CFG_EVENT_BUFFER_SIZE)
				{
					#if (TRC_CFG_SNAPSHOT_MODE == TRC_SNAPSHOT_MODE_RING_BUFFER)
					/* We have reached the end, but this is a ring buffer. Start from the beginning again. */
					RecorderDataPtr->bufferIsFull = 1;
					RecorderDataPtr->nextFreeIndex = 0;
					#else
					/* We have reached the end so we stop. */
					vTraceStop();
					#endif
				}
			}

			#if (TRC_CFG_SNAPSHOT_MODE == TRC_SNAPSHOT_MODE_RING_BUFFER)
			/* Make sure the next entry is cleared correctly */
			prvCheckDataToBeOverwrittenForMultiEntryEvents(1);
			#endif

		}
	}
	trcCRITICAL_SECTION_END();

#elif (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)
	/* Use the separate user event buffer */
	traceString formatLabel;
	traceUBChannel channel;

	if (RecorderDataPtr->recorderActive && (! inExcludedTask || nISRactive) && handle_of_last_logged_task)
	{
		formatLabel = xTraceRegisterString(formatStr);

		channel = xTraceRegisterUBChannel(eventLabel, formatLabel);

		prvTraceUBHelper1(channel, eventLabel, formatLabel, vl);
	}
#endif
}
#endif

/******************************************************************************
 * vTracePrint
 *
 * Basic user event
 *
 * Generates a User Event with a text label. The label is created/looked up
 * in the symbol table using xTraceRegisterString.
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1))
void vTracePrint(traceString chn, const char* str)
{
#if (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 0)
	UserEvent* ue;
	uint8_t dts1;
	TRACE_ALLOC_CRITICAL_SECTION();

	trcCRITICAL_SECTION_BEGIN();
	if (RecorderDataPtr->recorderActive && (! inExcludedTask || nISRactive) && handle_of_last_logged_task)
	{
		dts1 = (uint8_t)prvTraceGetDTS(0xFF);
		ue = (UserEvent*) prvTraceNextFreeEventBufferSlot();
		if (ue != NULL)
		{
			ue->dts = dts1;
			ue->type = USER_EVENT;
			ue->payload = prvTraceOpenSymbol(str, chn);
			prvTraceUpdateCounters();
		}
	}
	trcCRITICAL_SECTION_END();

#elif (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)
	traceUBChannel channel;
	uint32_t noOfSlots = 1;
	uint32_t tempDataBuffer[(3 + MAX_ARG_SIZE) / 4];
	if (RecorderDataPtr->recorderActive && (! inExcludedTask || nISRactive) && handle_of_last_logged_task)
	{
		traceString trcStr = prvTraceOpenSymbol(str, chn);
		channel = xTraceRegisterUBChannel(chn, trcStr);

		if (channel == 0)
		{
			/* We are dealing with an unknown channel format pair */
			noOfSlots++; /* Also need room for channel and format */
			((uint16_t*)tempDataBuffer)[2] = chn;
			((uint16_t*)tempDataBuffer)[3] = trcStr;
		}

		prvTraceUBHelper2(channel, tempDataBuffer, noOfSlots);
	}
#endif
}
#endif

/*******************************************************************************
 * xTraceRegisterString
 *
 * Register strings in the recorder, e.g. for names of user event channels.
 *
 * Example:
 *	 myEventHandle = xTraceRegisterString("MyUserEvent");
 *	 ...
 *	 vTracePrintF(myEventHandle, "My value is: %d", myValue);
 *
 ******************************************************************************/
#if ((TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1))
traceString xTraceRegisterString(const char* label)
{
	TRACE_ASSERT(label != NULL, "xTraceRegisterString: label == NULL", (traceString)0);

	return prvTraceOpenSymbol(label, 0);
}
#endif


#if ((!defined TRC_CFG_INCLUDE_READY_EVENTS) || (TRC_CFG_INCLUDE_READY_EVENTS == 1))

void prvTraceSetReadyEventsEnabled(int status) 
{
	readyEventsEnabled = status;
}

/*******************************************************************************
 * prvTraceStoreTaskReady
 *
 * This function stores a ready state for the task handle sent in as parameter.
 ******************************************************************************/
void prvTraceStoreTaskReady(traceHandle handle) 
{
	uint16_t dts3;
	TREvent* tr;
	uint8_t hnd8;

	TRACE_ALLOC_CRITICAL_SECTION();

	if (handle == 0)
	{
		/*  On FreeRTOS v7.3.0, this occurs when creating tasks due to a bad
		placement of the trace macro. In that case, the events are ignored. */
		return;
	}
	
	if (! readyEventsEnabled)
	{
		/* When creating tasks, ready events are also created. If creating 
		a "hidden" (not traced) task, we must therefore disable recording 
		of ready events to avoid an undesired ready event... */
		return;
	}

	TRACE_ASSERT(handle <= TRC_CFG_NTASK, "prvTraceStoreTaskReady: Invalid value for handle", TRC_UNUSED);

	if (recorder_busy)
	{
		/*************************************************************************
		* This occurs if an ISR calls a trace function, preempting a previous
		* trace call that is being processed in a different ISR or task. 
		* If this occurs, there is probably a problem in the definition of the
		* recorder's internal critical sections (TRACE_ENTER_CRITICAL_SECTION and
		* TRACE_EXIT_CRITICAL_SECTION). They must disable the RTOS tick interrupt
		* and any other ISRs that calls the trace recorder directly or via
		* traced kernel functions. The ARM port disables all interrupts using the
		* PRIMASK register to avoid this issue.
		*************************************************************************/
		prvTraceError("Recorder busy - high priority ISR using syscall? (1)");
		return;
	}

	trcCRITICAL_SECTION_BEGIN();
	if (RecorderDataPtr->recorderActive) /* Need to repeat this check! */
	{
		if (!TRACE_GET_TASK_FLAG_ISEXCLUDED(handle))
		{
			dts3 = (uint16_t)prvTraceGetDTS(0xFFFF);
			hnd8 = prvTraceGet8BitHandle(handle);
			tr = (TREvent*)prvTraceNextFreeEventBufferSlot();
			if (tr != NULL)
			{
				tr->type = DIV_TASK_READY;
				tr->dts = dts3;
				tr->objHandle = hnd8;
				prvTraceUpdateCounters();
			}
		}
	}
	trcCRITICAL_SECTION_END();
}
#endif

/*******************************************************************************
 * prvTraceStoreLowPower
 *
 * This function stores a low power state.
 ******************************************************************************/
void prvTraceStoreLowPower(uint32_t flag) 
{
	uint16_t dts;
	LPEvent* lp;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ASSERT(flag <= 1, "prvTraceStoreLowPower: Invalid flag value", TRC_UNUSED);

	if (recorder_busy)
	{
		/*************************************************************************
		* This occurs if an ISR calls a trace function, preempting a previous
		* trace call that is being processed in a different ISR or task. 
		* If this occurs, there is probably a problem in the definition of the
		* recorder's internal critical sections (TRACE_ENTER_CRITICAL_SECTION and
		* TRACE_EXIT_CRITICAL_SECTION). They must disable the RTOS tick interrupt
		* and any other ISRs that calls the trace recorder directly or via
		* traced kernel functions. The ARM port disables all interrupts using the
		* PRIMASK register to avoid this issue.
		*************************************************************************/
		prvTraceError("Recorder busy - high priority ISR using syscall? (1)");
		return;
	}

	trcCRITICAL_SECTION_BEGIN();
	if (RecorderDataPtr->recorderActive)
	{
		dts = (uint16_t)prvTraceGetDTS(0xFFFF);
		lp = (LPEvent*)prvTraceNextFreeEventBufferSlot();
		if (lp != NULL)
		{
			lp->type = (uint8_t) (LOW_POWER_BEGIN + ( uint8_t ) flag); /* BEGIN or END depending on flag */
			lp->dts = dts;
			prvTraceUpdateCounters();
		}
	}
	trcCRITICAL_SECTION_END();
}

/*******************************************************************************
 * vTraceStoreMemMangEvent
 *
 * This function stores malloc and free events. Each call requires two records,
 * for size and address respectively. The event code parameter (ecode) is applied
 * to the first record (size) and the following address record gets event
 * code "ecode + 1", so make sure this is respected in the event code table.
 * Note: On "free" calls, the signed_size parameter should be negative.
 ******************************************************************************/
#if (TRC_CFG_INCLUDE_MEMMANG_EVENTS == 1)
#if (TRC_CFG_SCHEDULING_ONLY == 0)
void vTraceStoreMemMangEvent(uint32_t ecode, uint32_t address, int32_t signed_size)
{
	uint8_t dts1;
	MemEventSize * ms;
	MemEventAddr * ma;
	uint16_t size_low;
	uint16_t addr_low;
	uint8_t addr_high;
	uint32_t size;
	TRACE_ALLOC_CRITICAL_SECTION();

	if (RecorderDataPtr == NULL)
	{
		/* Occurs in vTraceInitTraceData, if using dynamic allocation. */
		return;
	}
	
	if (signed_size < 0)
		size = (uint32_t)(- signed_size);
	else
		size = (uint32_t)(signed_size);

	trcCRITICAL_SECTION_BEGIN();
	
	heapMemUsage = heapMemUsage + (uint32_t)signed_size;
	
	if (RecorderDataPtr->recorderActive)
	{
		/* If it is an ISR or NOT an excluded task, this kernel call will be stored in the trace */
		if (nISRactive || !inExcludedTask)
		{
			dts1 = (uint8_t)prvTraceGetDTS(0xFF);
			size_low = (uint16_t)prvTraceGetParam(0xFFFF, size);
			ms = (MemEventSize *)prvTraceNextFreeEventBufferSlot();

			if (ms != NULL)
			{
				ms->dts = dts1;
				ms->type = NULL_EVENT; /* Updated when all events are written */
				ms->size = size_low;
				prvTraceUpdateCounters();

				/* Storing a second record with address (signals "failed" if null) */
				#if (TRC_CFG_HEAP_SIZE_BELOW_16M)
				    /* If the heap address range is within 16 MB, i.e., the upper 8 bits
					of addresses are constant, this optimization avoids storing an extra
					event record by ignoring the upper 8 bit of the address */
					addr_low = address & 0xFFFF;          
					addr_high = (address >> 16) & 0xFF;
				#else
				    /* The whole 32 bit address is stored using a second event record
					for the upper 16 bit */
					addr_low = (uint16_t)prvTraceGetParam(0xFFFF, address);
					addr_high = 0;
				#endif

				ma = (MemEventAddr *) prvTraceNextFreeEventBufferSlot();
				if (ma != NULL)
				{
					ma->addr_low = addr_low;
					ma->addr_high = addr_high;
					ma->type = (uint8_t) (ecode  + 1); /* Note this! */
					ms->type = (uint8_t) ecode;
					prvTraceUpdateCounters();					
					RecorderDataPtr->heapMemUsage = heapMemUsage;
				}
			}
		}
	}
	trcCRITICAL_SECTION_END();
}
#endif /* TRC_CFG_SCHEDULING_ONLY */
#endif

/*******************************************************************************
 * prvTraceStoreKernelCall
 *
 * This is the main integration point for storing kernel calls, and
 * is called by the hooks in trcKernelHooks.h (see trcKernelPort.h for event codes).
 ******************************************************************************/
#if (TRC_CFG_SCHEDULING_ONLY == 0)
void prvTraceStoreKernelCall(uint32_t ecode, traceObjectClass objectClass, uint32_t objectNumber)
{
	KernelCall * kse;
	uint16_t dts1;
	uint8_t hnd8;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ASSERT(ecode < 0xFF, "prvTraceStoreKernelCall: ecode >= 0xFF", TRC_UNUSED);
	TRACE_ASSERT(objectClass < TRACE_NCLASSES, "prvTraceStoreKernelCall: objectClass >= TRACE_NCLASSES", TRC_UNUSED);
	TRACE_ASSERT(objectNumber <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectClass], "prvTraceStoreKernelCall: Invalid value for objectNumber", TRC_UNUSED);

	if (recorder_busy)
	{
		/*************************************************************************
		* This occurs if an ISR calls a trace function, preempting a previous
		* trace call that is being processed in a different ISR or task. 
		* If this occurs, there is probably a problem in the definition of the
		* recorder's internal critical sections (TRACE_ENTER_CRITICAL_SECTION and
		* TRACE_EXIT_CRITICAL_SECTION). They must disable the RTOS tick interrupt
		* and any other ISRs that calls the trace recorder directly or via
		* traced kernel functions. The ARM port disables all interrupts using the
		* PRIMASK register to avoid this issue.
		*************************************************************************/
		prvTraceError("Recorder busy - high priority ISR using syscall? (2)");
		return;
	}

	if (handle_of_last_logged_task == 0)
	{
		return;
	}

	trcCRITICAL_SECTION_BEGIN();
	if (RecorderDataPtr->recorderActive)
	{
		/* If it is an ISR or NOT an excluded task, this kernel call will be stored in the trace */
		if (nISRactive || !inExcludedTask)
		{
			/* Check if the referenced object or the event code is excluded */
			if (!uiTraceIsObjectExcluded(objectClass, (traceHandle)objectNumber) && !TRACE_GET_EVENT_CODE_FLAG_ISEXCLUDED(ecode))
			{
				dts1 = (uint16_t)prvTraceGetDTS(0xFFFF);
				hnd8 = prvTraceGet8BitHandle((traceHandle)objectNumber);
				kse = (KernelCall*) prvTraceNextFreeEventBufferSlot();
				if (kse != NULL)
				{
					kse->dts = dts1;
					kse->type = (uint8_t)ecode;
					kse->objHandle = hnd8;
					prvTraceUpdateCounters();
				}
			}
		}
	}
	trcCRITICAL_SECTION_END();
}
#endif /* TRC_CFG_SCHEDULING_ONLY */

/*******************************************************************************
 * prvTraceStoreKernelCallWithParam
 *
 * Used for storing kernel calls with a handle and a numeric parameter. If the
 * numeric parameter does not fit in one byte, and extra XPS event is inserted
 * before the kernel call event containing the three upper bytes.
 ******************************************************************************/
#if (TRC_CFG_SCHEDULING_ONLY == 0)
void prvTraceStoreKernelCallWithParam(uint32_t evtcode,
									traceObjectClass objectClass,
									uint32_t objectNumber,
									uint32_t param)
{
	KernelCallWithParamAndHandle * kse;
	uint8_t dts2;
	uint8_t hnd8;
	uint8_t p8;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ASSERT(evtcode < 0xFF, "prvTraceStoreKernelCallWithParam: evtcode >= 0xFF", TRC_UNUSED);
	TRACE_ASSERT(objectClass < TRACE_NCLASSES, "prvTraceStoreKernelCallWithParam: objectClass >= TRACE_NCLASSES", TRC_UNUSED);
	TRACE_ASSERT(objectNumber <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectClass], "prvTraceStoreKernelCallWithParam: Invalid value for objectNumber", TRC_UNUSED);

	if (recorder_busy)
	{
		/*************************************************************************
		* This occurs if an ISR calls a trace function, preempting a previous
		* trace call that is being processed in a different ISR or task. 
		* If this occurs, there is probably a problem in the definition of the
		* recorder's internal critical sections (TRACE_ENTER_CRITICAL_SECTION and
		* TRACE_EXIT_CRITICAL_SECTION). They must disable the RTOS tick interrupt
		* and any other ISRs that calls the trace recorder directly or via
		* traced kernel functions. The ARM port disables all interrupts using the
		* PRIMASK register to avoid this issue.
		*************************************************************************/
		prvTraceError("Recorder busy - high priority ISR using syscall? (3)");
		return;
	}

	trcCRITICAL_SECTION_BEGIN();
	if (RecorderDataPtr->recorderActive && handle_of_last_logged_task && (! inExcludedTask || nISRactive))
	{
		/* Check if the referenced object or the event code is excluded */
		if (!uiTraceIsObjectExcluded(objectClass, (traceHandle)objectNumber) &&
			!TRACE_GET_EVENT_CODE_FLAG_ISEXCLUDED(evtcode))
		{
			dts2 = (uint8_t)prvTraceGetDTS(0xFF);
			p8 = (uint8_t) prvTraceGetParam(0xFF, param);
			hnd8 = prvTraceGet8BitHandle((traceHandle)objectNumber);
			kse = (KernelCallWithParamAndHandle*) prvTraceNextFreeEventBufferSlot();
			if (kse != NULL)
			{
				kse->dts = dts2;
				kse->type = (uint8_t)evtcode;
				kse->objHandle = hnd8;
				kse->param = p8;
				prvTraceUpdateCounters();
			}
		}
	}
	trcCRITICAL_SECTION_END();
}
#endif /* TRC_CFG_SCHEDULING_ONLY */


/*******************************************************************************
 * prvTraceGetParam
 *
 * Used for storing extra bytes for kernel calls with numeric parameters.
 *
 * May only be called within a critical section!
 ******************************************************************************/
#if (TRC_CFG_SCHEDULING_ONLY == 0)
static uint32_t prvTraceGetParam(uint32_t param_max, uint32_t param)
{
	XPSEvent* xps;

	TRACE_ASSERT(param_max == 0xFF || param_max == 0xFFFF,
		"prvTraceGetParam: Invalid value for param_max", param);

	if (param <= param_max)
	{
		return param;
	}
	else
	{
		xps = (XPSEvent*) prvTraceNextFreeEventBufferSlot();
		if (xps != NULL)
		{
			xps->type = DIV_XPS;
			xps->xps_8 = (uint8_t)((param & (0xFF00 & ~param_max)) >> 8);
			xps->xps_16 = (uint16_t)((param & (0xFFFF0000 & ~param_max)) >> 16);
			prvTraceUpdateCounters();
		}

		return param & param_max;
	}
}
#endif

/*******************************************************************************
 * prvTraceStoreKernelCallWithNumericParamOnly
 *
 * Used for storing kernel calls with numeric parameters only. This is
 * only used for traceTASK_DELAY and traceDELAY_UNTIL at the moment.
 ******************************************************************************/
#if (TRC_CFG_SCHEDULING_ONLY == 0)
void prvTraceStoreKernelCallWithNumericParamOnly(uint32_t evtcode, uint32_t param)
{
	KernelCallWithParam16 * kse;
	uint8_t dts6;
	uint16_t restParam;
	TRACE_ALLOC_CRITICAL_SECTION();

	restParam = 0;

	TRACE_ASSERT(evtcode < 0xFF, "prvTraceStoreKernelCallWithNumericParamOnly: Invalid value for evtcode", TRC_UNUSED);

	if (recorder_busy)
	{
		/*************************************************************************
		* This occurs if an ISR calls a trace function, preempting a previous
		* trace call that is being processed in a different ISR or task. 
		* If this occurs, there is probably a problem in the definition of the
		* recorder's internal critical sections (TRACE_ENTER_CRITICAL_SECTION and
		* TRACE_EXIT_CRITICAL_SECTION). They must disable the RTOS tick interrupt
		* and any other ISRs that calls the trace recorder directly or via
		* traced kernel functions. The ARM port disables all interrupts using the
		* PRIMASK register to avoid this issue.
		*************************************************************************/
		prvTraceError("Recorder busy - high priority ISR using syscall? (4)");
		return;
	}

	trcCRITICAL_SECTION_BEGIN();
	if (RecorderDataPtr->recorderActive && handle_of_last_logged_task
		&& (! inExcludedTask || nISRactive))
	{
		/* Check if the event code is excluded */
		if (!TRACE_GET_EVENT_CODE_FLAG_ISEXCLUDED(evtcode))
		{
			dts6 = (uint8_t)prvTraceGetDTS(0xFF);
			restParam = (uint16_t)prvTraceGetParam(0xFFFF, param);
			kse = (KernelCallWithParam16*) prvTraceNextFreeEventBufferSlot();
			if (kse != NULL)
			{
				kse->dts = dts6;
				kse->type = (uint8_t)evtcode;
				kse->param = restParam;
				prvTraceUpdateCounters();
			}
		}
	}
	trcCRITICAL_SECTION_END();
}
#endif /* TRC_CFG_SCHEDULING_ONLY */

/*******************************************************************************
 * prvTraceStoreTaskswitch
 * Called by the scheduler from the SWITCHED_OUT hook, and by uiTraceStart.
 * At this point interrupts are assumed to be disabled!
 ******************************************************************************/
void prvTraceStoreTaskswitch(traceHandle task_handle)
{
	uint16_t dts3;
	TSEvent* ts;
	int8_t skipEvent;
	uint8_t hnd8;
#if (TRC_CFG_INCLUDE_ISR_TRACING == 1)
	extern int32_t isPendingContextSwitch;
#endif
	trcSR_ALLOC_CRITICAL_SECTION_ON_CORTEX_M_ONLY();

	skipEvent = 0;

	TRACE_ASSERT(task_handle <= TRC_CFG_NTASK,
		"prvTraceStoreTaskswitch: Invalid value for task_handle", TRC_UNUSED);

	/* Skip the event if the task has been excluded, using vTraceExcludeTask */
	if (TRACE_GET_TASK_FLAG_ISEXCLUDED(task_handle))
	{
		skipEvent = 1;
		inExcludedTask = 1;
	}
	else
	{
		inExcludedTask = 0;
	}

	trcCRITICAL_SECTION_BEGIN_ON_CORTEX_M_ONLY();

	/* Skip the event if the same task is scheduled */
	if (task_handle == handle_of_last_logged_task)
	{
		skipEvent = 1;
	}

	if (!RecorderDataPtr->recorderActive)
	{
		skipEvent = 1;
	}

	/* If this event should be logged, log it! */
	if (skipEvent == 0)
	{
#if (TRC_CFG_INCLUDE_ISR_TRACING == 1)
		isPendingContextSwitch = 0;
#endif

		dts3 = (uint16_t)prvTraceGetDTS(0xFFFF);
		handle_of_last_logged_task = task_handle;
		hnd8 = prvTraceGet8BitHandle(handle_of_last_logged_task);
		ts = (TSEvent*)prvTraceNextFreeEventBufferSlot();

		if (ts != NULL)
		{
			if (prvTraceGetObjectState(TRACE_CLASS_TASK,
				handle_of_last_logged_task) == TASK_STATE_INSTANCE_ACTIVE)
			{
				ts->type = TS_TASK_RESUME;
			}
			else
			{
				ts->type = TS_TASK_BEGIN;
			}

			ts->dts = dts3;
			ts->objHandle = hnd8;

			prvTraceSetObjectState(TRACE_CLASS_TASK,
									handle_of_last_logged_task,
									TASK_STATE_INSTANCE_ACTIVE);

			prvTraceUpdateCounters();
		}
	}

	trcCRITICAL_SECTION_END_ON_CORTEX_M_ONLY();
}

/*******************************************************************************
 * prvTraceStoreObjectNameOnCloseEvent
 *
 * Updates the symbol table with the name of this object from the dynamic
 * objects table and stores a "close" event, holding the mapping between handle
 * and name (a symbol table handle). The stored name-handle mapping is thus the
 * "old" one, valid up until this point.
 ******************************************************************************/
#if (TRC_CFG_INCLUDE_OBJECT_DELETE == 1)
void prvTraceStoreObjectNameOnCloseEvent(traceHandle handle,
										traceObjectClass objectclass)
{
	ObjCloseNameEvent * ce;
	const char * name;
	traceString idx;

	TRACE_ASSERT(objectclass < TRACE_NCLASSES,
		"prvTraceStoreObjectNameOnCloseEvent: objectclass >= TRACE_NCLASSES", TRC_UNUSED);
	TRACE_ASSERT(handle <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass],
		"prvTraceStoreObjectNameOnCloseEvent: Invalid value for handle", TRC_UNUSED);

	if (RecorderDataPtr->recorderActive)
	{
		uint8_t hnd8 = prvTraceGet8BitHandle(handle);
		name = TRACE_PROPERTY_NAME_GET(objectclass, handle);
		idx = prvTraceOpenSymbol(name, 0);

		// Interrupt disable not necessary, already done in trcHooks.h macro
		ce = (ObjCloseNameEvent*) prvTraceNextFreeEventBufferSlot();
		if (ce != NULL)
		{
			ce->type = (uint8_t) (EVENTGROUP_OBJCLOSE_NAME + objectclass);
			ce->objHandle = hnd8;
			ce->symbolIndex = idx;
			prvTraceUpdateCounters();
		}
	}
}

void prvTraceStoreObjectPropertiesOnCloseEvent(traceHandle handle,
											 traceObjectClass objectclass)
{
	ObjClosePropEvent * pe;

	TRACE_ASSERT(objectclass < TRACE_NCLASSES,
		"prvTraceStoreObjectPropertiesOnCloseEvent: objectclass >= TRACE_NCLASSES", TRC_UNUSED);
	TRACE_ASSERT(handle <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass],
		"prvTraceStoreObjectPropertiesOnCloseEvent: Invalid value for handle", TRC_UNUSED);

	if (RecorderDataPtr->recorderActive)
	{
		// Interrupt disable not necessary, already done in trcHooks.h macro
		pe = (ObjClosePropEvent*) prvTraceNextFreeEventBufferSlot();
		if (pe != NULL)
		{
			if (objectclass == TRACE_CLASS_TASK)
			{
				pe->arg1 = TRACE_PROPERTY_ACTOR_PRIORITY(objectclass, handle);
			}
			else
			{
				pe->arg1 = TRACE_PROPERTY_OBJECT_STATE(objectclass, handle);
			}
			pe->type = (uint8_t) (EVENTGROUP_OBJCLOSE_PROP + objectclass);
			prvTraceUpdateCounters();
		}
	}
}
#endif

void prvTraceSetPriorityProperty(uint8_t objectclass, traceHandle id, uint8_t value)
{
	TRACE_ASSERT(objectclass < TRACE_NCLASSES,
		"prvTraceSetPriorityProperty: objectclass >= TRACE_NCLASSES", TRC_UNUSED);
	TRACE_ASSERT(id <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass],
		"prvTraceSetPriorityProperty: Invalid value for id", TRC_UNUSED);

	TRACE_PROPERTY_ACTOR_PRIORITY(objectclass, id) = value;
}

uint8_t prvTraceGetPriorityProperty(uint8_t objectclass, traceHandle id)
{
	TRACE_ASSERT(objectclass < TRACE_NCLASSES,
		"prvTraceGetPriorityProperty: objectclass >= TRACE_NCLASSES", 0);
	TRACE_ASSERT(id <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass],
		"prvTraceGetPriorityProperty: Invalid value for id", 0);

	return TRACE_PROPERTY_ACTOR_PRIORITY(objectclass, id);
}

void prvTraceSetObjectState(uint8_t objectclass, traceHandle id, uint8_t value)
{
	TRACE_ASSERT(objectclass < TRACE_NCLASSES,
		"prvTraceSetObjectState: objectclass >= TRACE_NCLASSES", TRC_UNUSED);
	TRACE_ASSERT(id <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass],
		"prvTraceSetObjectState: Invalid value for id", TRC_UNUSED);

	TRACE_PROPERTY_OBJECT_STATE(objectclass, id) = value;
}

uint8_t prvTraceGetObjectState(uint8_t objectclass, traceHandle id)
{
	TRACE_ASSERT(objectclass < TRACE_NCLASSES,
		"prvTraceGetObjectState: objectclass >= TRACE_NCLASSES", 0);
	TRACE_ASSERT(id <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass],
		"prvTraceGetObjectState: Invalid value for id", 0);

	return TRACE_PROPERTY_OBJECT_STATE(objectclass, id);
}

void prvTraceSetTaskInstanceFinished(traceHandle handle)
{
	TRACE_ASSERT(handle <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[TRACE_CLASS_TASK],
		"prvTraceSetTaskInstanceFinished: Invalid value for handle", TRC_UNUSED);

#if (TRC_CFG_USE_IMPLICIT_IFE_RULES == 1)
	TRACE_PROPERTY_OBJECT_STATE(TRACE_CLASS_TASK, handle) = 0;
#endif
}

/*******************************************************************************
 * Static data initializations
 ******************************************************************************/

/* Tasks and kernel objects can be explicitly excluded from the trace to reduce
buffer usage. This structure handles the exclude flags for all objects and tasks. 
Note that slot 0 is not used, since not a valid handle. */
uint8_t trcExcludedObjects[(TRACE_KERNEL_OBJECT_COUNT + TRACE_NCLASSES) / 8 + 1] = { 0 };

/* Specific events can also be excluded, i.e., by the event code. This can be
used to exclude kernel calls that don't refer to a kernel object, like a delay.
This structure handle the exclude flags for all event codes */
uint8_t trcExcludedEventCodes[NEventCodes / 8 + 1] = { 0 };

/* A set of stacks that keeps track of available object handles for each class.
The stacks are empty initially, meaning that allocation of new handles will be 
based on a counter (for each object class). Any delete operation will
return the handle to the corresponding stack, for reuse on the next allocate.*/
objectHandleStackType objectHandleStacks = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };

/* Initial TRC_HWTC_COUNT value, for detecting if the time-stamping source is 
enabled. If using the OS periodic timer for time-stamping, this might not 
have been configured on the earliest events during the startup. */
uint32_t init_hwtc_count;

/*******************************************************************************
 * RecorderData
 *
 * The main data structure in snapshot mode, when using the default static memory
 * allocation (TRC_RECORDER_BUFFER_ALLOCATION_STATIC). The recorder uses a pointer
 * RecorderDataPtr to access the data, to also allow for dynamic or custom data
 * allocation (see TRC_CFG_RECORDER_BUFFER_ALLOCATION).
 ******************************************************************************/
#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_STATIC)
RecorderDataType RecorderData;
#endif

/*******************************************************************************
 * RecorderDataPtr
 *
 * Pointer to the main data structure, when in snapshot mode. 
 ******************************************************************************/
RecorderDataType* RecorderDataPtr = NULL;

/* This version of the function dynamically allocates the trace data */
void prvTraceInitTraceData()
{	
	
	if (RecorderDataPtr == NULL)
	{
#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_STATIC)
	RecorderDataPtr = &RecorderData;
#elif (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC)
	RecorderDataPtr = (RecorderDataType*)TRACE_MALLOC(sizeof(RecorderDataType));
	if (! RecorderDataPtr)
	{
		prvTraceError("Failed allocating recorder buffer!");
		return;
	}
#elif (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM)
		if (! RecorderDataPtr)
		{
			prvTraceError("Recorder data pointer not set! Use vTraceSetRecorderDataBuffer().");
			return;
		}
#endif
	}
	else
	{
		if (RecorderDataPtr->startmarker0 == 1)
		{
			/* Already initialized */
			return;
		}
	}
	
	init_hwtc_count = TRC_HWTC_COUNT;
		
	(void)memset(RecorderDataPtr, 0, sizeof(RecorderDataType));
	
	RecorderDataPtr->version = TRACE_KERNEL_VERSION;
	RecorderDataPtr->minor_version = TRACE_MINOR_VERSION;
	RecorderDataPtr->irq_priority_order = TRC_IRQ_PRIORITY_ORDER;
	RecorderDataPtr->filesize = sizeof(RecorderDataType);
	RecorderDataPtr->maxEvents = TRC_CFG_EVENT_BUFFER_SIZE;
	RecorderDataPtr->debugMarker0 = (int32_t) 0xF0F0F0F0;
	RecorderDataPtr->isUsing16bitHandles = TRC_CFG_USE_16BIT_OBJECT_HANDLES;
	RecorderDataPtr->isrTailchainingThreshold = TRC_CFG_ISR_TAILCHAINING_THRESHOLD;

	/* This function is kernel specific */
	vTraceInitObjectPropertyTable();

	RecorderDataPtr->debugMarker1 = (int32_t)0xF1F1F1F1;
	RecorderDataPtr->SymbolTable.symTableSize = TRC_CFG_SYMBOL_TABLE_SIZE;
	RecorderDataPtr->SymbolTable.nextFreeSymbolIndex = 1;
#if (TRC_CFG_INCLUDE_FLOAT_SUPPORT == 1)
	RecorderDataPtr->exampleFloatEncoding = 1.0f; /* otherwise already zero */
#endif
	RecorderDataPtr->debugMarker2 = (int32_t)0xF2F2F2F2;
	prvStrncpy(RecorderDataPtr->systemInfo, "Trace Recorder Demo", 80);
	RecorderDataPtr->debugMarker3 = (int32_t)0xF3F3F3F3;
	RecorderDataPtr->endmarker0 = 0x0A;
	RecorderDataPtr->endmarker1 = 0x0B;
	RecorderDataPtr->endmarker2 = 0x0C;
	RecorderDataPtr->endmarker3 = 0x0D;
	RecorderDataPtr->endmarker4 = 0x71;
	RecorderDataPtr->endmarker5 = 0x72;
	RecorderDataPtr->endmarker6 = 0x73;
	RecorderDataPtr->endmarker7 = 0x74;
	RecorderDataPtr->endmarker8 = 0xF1;
	RecorderDataPtr->endmarker9 = 0xF2;
	RecorderDataPtr->endmarker10 = 0xF3;
	RecorderDataPtr->endmarker11 = 0xF4;

#if TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER
	RecorderDataPtr->userEventBuffer.bufferID = 1;
	RecorderDataPtr->userEventBuffer.version = 0;
	RecorderDataPtr->userEventBuffer.numberOfSlots = TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE;
	RecorderDataPtr->userEventBuffer.numberOfChannels = TRC_CFG_UB_CHANNELS + 1;
#endif

	/* Kernel specific initialization of the objectHandleStacks variable */
	vTraceInitObjectHandleStack();

	
	/* Finally, the 12-byte "start markers" are initialized, allowing for 
	Tracealyzer to find the trace data in a larger RAM dump. 
	
	The start and end markers must be unique, but without proper precautions there
	might be a risk of accidental duplicates of the start/end markers, e.g., due to
	compiler optimizations.
	
	The below initialization of the start marker is therefore made in reverse order 
	and the fields are volatile to ensure this assignment order. This to avoid any 
	chance of accidental duplicates of this elsewhere in memory.
	
	Moreover, the fields are set byte-by-byte to avoid endian issues.*/
	
	RecorderDataPtr->startmarker11 = 0xF4;
	RecorderDataPtr->startmarker10 = 0xF3;
	RecorderDataPtr->startmarker9 = 0xF2;
	RecorderDataPtr->startmarker8 = 0xF1;
	RecorderDataPtr->startmarker7 = 0x74;
	RecorderDataPtr->startmarker6 = 0x73;
	RecorderDataPtr->startmarker5 = 0x72;
	RecorderDataPtr->startmarker4 = 0x71;
	RecorderDataPtr->startmarker3 = 0x04;
	RecorderDataPtr->startmarker2 = 0x03;
	RecorderDataPtr->startmarker1 = 0x02;	
	RecorderDataPtr->startmarker0 = 0x01; 

	
#ifdef TRC_PORT_SPECIFIC_INIT
	TRC_PORT_SPECIFIC_INIT();
#endif
}


void* prvTraceNextFreeEventBufferSlot(void)
{
	if (! RecorderDataPtr->recorderActive)
	{
		/* If an XTS or XPS event prior to the main event has filled the buffer
		before saving the main event, and store mode is "stop when full". */
		return NULL;
	}

	if (RecorderDataPtr->nextFreeIndex >= TRC_CFG_EVENT_BUFFER_SIZE)
	{
		prvTraceError("Attempt to index outside event buffer!");
		return NULL;
	}
	return (void*)(&RecorderDataPtr->eventData[RecorderDataPtr->nextFreeIndex*4]);
}

uint16_t uiIndexOfObject(traceHandle objecthandle, uint8_t objectclass)
{
	TRACE_ASSERT(objectclass < TRACE_NCLASSES, 
		"uiIndexOfObject: Invalid value for objectclass", 0);
	TRACE_ASSERT(objecthandle > 0 && objecthandle <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass], 
		"uiIndexOfObject: Invalid value for objecthandle", 0);

	if ((objectclass < TRACE_NCLASSES) && (objecthandle > 0) && 
		(objecthandle <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass]))
	{
		return (uint16_t)(RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[objectclass] + 
			(RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[objectclass] * (objecthandle-1)));
	}

	prvTraceError("Object table lookup with invalid object handle or object class!");
	return 0;
}

traceHandle prvTraceGetObjectHandle(traceObjectClass objectclass)
{
	traceHandle handle;
	static int indexOfHandle;

	TRACE_ASSERT(objectclass < TRACE_NCLASSES, 
		"prvTraceGetObjectHandle: Invalid value for objectclass", (traceHandle)0);

	indexOfHandle = objectHandleStacks.indexOfNextAvailableHandle[objectclass];
	if (objectHandleStacks.objectHandles[indexOfHandle] == 0)
	{
		/* Zero is used to indicate a never before used handle, i.e.,
			new slots in the handle stack. The handle slot needs to
			be initialized here (starts at 1). */
		objectHandleStacks.objectHandles[indexOfHandle] =
			(traceHandle)(1 + indexOfHandle -
			objectHandleStacks.lowestIndexOfClass[objectclass]);
	}

	handle = objectHandleStacks.objectHandles[indexOfHandle];

	if (objectHandleStacks.indexOfNextAvailableHandle[objectclass]
		> objectHandleStacks.highestIndexOfClass[objectclass])
	{
		prvTraceError(pszTraceGetErrorNotEnoughHandles(objectclass));
		handle = 0;
	}
	else
	{
		int hndCount;
		objectHandleStacks.indexOfNextAvailableHandle[objectclass]++;

		hndCount = objectHandleStacks.indexOfNextAvailableHandle[objectclass] -
			objectHandleStacks.lowestIndexOfClass[objectclass];

		if (hndCount >
			objectHandleStacks.handleCountWaterMarksOfClass[objectclass])
		{
			objectHandleStacks.handleCountWaterMarksOfClass[objectclass] =
				(traceHandle)hndCount;
		}
		TRACE_CLEAR_OBJECT_FLAG_ISEXCLUDED(objectclass, handle);
	}
	return handle;
}

void prvTraceFreeObjectHandle(traceObjectClass objectclass, traceHandle handle)
{
	int indexOfHandle;

	TRACE_ASSERT(objectclass < TRACE_NCLASSES, 
		"prvTraceFreeObjectHandle: Invalid value for objectclass", TRC_UNUSED);
	TRACE_ASSERT(handle > 0 && handle <= RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass], 
		"prvTraceFreeObjectHandle: Invalid value for handle", TRC_UNUSED);

	/* Check that there is room to push the handle on the stack */
	if ((objectHandleStacks.indexOfNextAvailableHandle[objectclass] - 1) <
		objectHandleStacks.lowestIndexOfClass[objectclass])
	{
		/* Error */
		prvTraceError("Attempt to free more handles than allocated!");
	}
	else
	{
		objectHandleStacks.indexOfNextAvailableHandle[objectclass]--;
		indexOfHandle = objectHandleStacks.indexOfNextAvailableHandle[objectclass];
		objectHandleStacks.objectHandles[indexOfHandle] = handle;
	}
}

/*******************************************************************************
 * prvMarkObjectAsUsed
 *
 * Sets an "is used flag" on object creation, using the first byte of the name
 * field. This allows for counting the number of used Object Table slots, even
 * if no names have been set.
 ******************************************************************************/
void prvMarkObjectAsUsed(traceObjectClass objectclass, traceHandle handle)
{
	uint16_t idx = uiIndexOfObject(handle, objectclass);
	RecorderDataPtr->ObjectPropertyTable.objbytes[idx] = 1;
}

/*******************************************************************************
 * prvStrncpy
 *
 * Private string copy function, to improve portability between compilers.
 ******************************************************************************/
static void prvStrncpy(char* dst, const char* src, uint32_t maxLength)
{
	uint32_t i;
	for (i = 0; i < maxLength; i++)
	{
		dst[i] = src[i];
		if (src[i] == 0)
			break;
	}
}

/*******************************************************************************
 * prvTraceSetObjectName
 *
 * Registers the names of queues, semaphores and other kernel objects in the
 * recorder's Object Property Table, at the given handle and object class.
 ******************************************************************************/
void prvTraceSetObjectName(traceObjectClass objectclass,
						 traceHandle handle,
						 const char* name)
{
	static uint16_t idx;

	TRACE_ASSERT(name != NULL, "prvTraceSetObjectName: name == NULL", TRC_UNUSED);

	if (objectclass >= TRACE_NCLASSES)
	{
		prvTraceError("Illegal object class in prvTraceSetObjectName");
		return;
	}

	if (handle == 0)
	{
		prvTraceError("Illegal handle (0) in prvTraceSetObjectName.");
		return;
	}

	if (handle > RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[objectclass])
	{
		/* ERROR */
		prvTraceError(pszTraceGetErrorNotEnoughHandles(objectclass));
	}
	else
	{
		idx = uiIndexOfObject(handle, objectclass);

		if (traceErrorMessage == NULL)
		{
			prvStrncpy((char*)&(RecorderDataPtr->ObjectPropertyTable.objbytes[idx]),
				name,
				RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[ objectclass ]);
		}
	}
}

traceString prvTraceOpenSymbol(const char* name, traceString userEventChannel)
{
	uint16_t result;
	uint8_t len;
	uint8_t crc;
	TRACE_ALLOC_CRITICAL_SECTION();
	
	len = 0;
	crc = 0;
	
	TRACE_ASSERT(name != NULL, "prvTraceOpenSymbol: name == NULL", (traceString)0);

	prvTraceGetChecksum(name, &crc, &len);

	trcCRITICAL_SECTION_BEGIN();
	result = prvTraceLookupSymbolTableEntry(name, crc, len, userEventChannel);
	if (!result)
	{
		result = prvTraceCreateSymbolTableEntry(name, crc, len, userEventChannel);
	}
	trcCRITICAL_SECTION_END();

	return result;
}


/******************************************************************************
* vTraceSetFrequency
*
* Registers the clock rate of the time source for the event timestamping.
* This is normally not required, but if the default value (TRC_HWTC_FREQ_HZ)
* should be incorrect for your setup, you can override it using this function.
*
* Must be called prior to vTraceEnable, and the time source is assumed to
* have a fixed clock frequency after the startup.
*
* Note that, in snapshot mode, the value is divided by the TRC_HWTC_DIVISOR.
* This is a software "prescaler" that is also applied on the timestamps.
*****************************************************************************/
void vTraceSetFrequency(uint32_t frequency)
{
	timestampFrequency = frequency;
}

/*******************************************************************************
 * Supporting functions
 ******************************************************************************/

/*******************************************************************************
 * prvTraceError
 *
 * Called by various parts in the recorder. Stops the recorder and stores a
 * pointer to an error message, which is printed by the monitor task.
 * If you are not using the monitor task, you may use xTraceGetLastError()
 * from your application to check if the recorder is OK.
 *
 * Note: If a recorder error is registered before vTraceStart is called, the
 * trace start will be aborted. This can occur if any of the Nxxxx constants
 * (e.g., TRC_CFG_NTASK) in trcConfig.h is too small.
 ******************************************************************************/
void prvTraceError(const char* msg)
{
	/* Stop the recorder */
	vTraceStop();
	
	/* If first error only... */
	if (traceErrorMessage == NULL)
	{
		traceErrorMessage = (char*)(intptr_t) msg;
		if (RecorderDataPtr != NULL)
		{
			prvStrncpy(RecorderDataPtr->systemInfo, traceErrorMessage, 80);
			RecorderDataPtr->internalErrorOccured = 1;
		}
	}
}

/******************************************************************************
 * prvCheckDataToBeOverwrittenForMultiEntryEvents
 *
 * This checks if the next event to be overwritten is a multi-entry user event,
 * i.e., a USER_EVENT followed by data entries.
 * Such data entries do not have an event code at byte 0, as other events.
 * All 4 bytes are user data, so the first byte of such data events must
 * not be interpreted as type field. The number of data entries following
 * a USER_EVENT is given in the event code of the USER_EVENT.
 * Therefore, when overwriting a USER_EVENT (when using in ring-buffer mode)
 * any data entries following must be replaced with NULL events (code 0).
 *
 * This is assumed to execute within a critical section...
 *****************************************************************************/

#if (TRC_CFG_SNAPSHOT_MODE == TRC_SNAPSHOT_MODE_RING_BUFFER)
void prvCheckDataToBeOverwrittenForMultiEntryEvents(uint8_t nofEntriesToCheck)
{
	/* Generic "int" type is desired - should be 16 bit variable on 16 bit HW */
	unsigned int i = 0;
	unsigned int e = 0;

	TRACE_ASSERT(nofEntriesToCheck != 0, 
		"prvCheckDataToBeOverwrittenForMultiEntryEvents: nofEntriesToCheck == 0", TRC_UNUSED);

	while (i < nofEntriesToCheck)
	{
		e = RecorderDataPtr->nextFreeIndex + i;
		if ((RecorderDataPtr->eventData[e*4] > USER_EVENT) &&
			(RecorderDataPtr->eventData[e*4] < USER_EVENT + 16))
		{
			uint8_t nDataEvents = (uint8_t)(RecorderDataPtr->eventData[e*4] - USER_EVENT);
			if ((e + nDataEvents) < RecorderDataPtr->maxEvents)
			{
				(void)memset(& RecorderDataPtr->eventData[e*4], 0, (size_t) (4 + 4 * nDataEvents));
			}
		}
		else if (RecorderDataPtr->eventData[e*4] == DIV_XPS)
		{
			if ((e + 1) < RecorderDataPtr->maxEvents)
			{
				/* Clear 8 bytes */
				(void)memset(& RecorderDataPtr->eventData[e*4], 0, 4 + 4);
			}
			else
			{
				/* Clear 8 bytes, 4 first and 4 last */
				(void)memset(& RecorderDataPtr->eventData[0], 0, 4);
				(void)memset(& RecorderDataPtr->eventData[e*4], 0, 4);
			}
		}
		i++;
	}
}
#endif

/*******************************************************************************
 * prvTraceUpdateCounters
 *
 * Updates the index of the event buffer.
 ******************************************************************************/
void prvTraceUpdateCounters(void)
{	
	if (RecorderDataPtr->recorderActive == 0)
	{
		return;
	}
	
	RecorderDataPtr->numEvents++;

	RecorderDataPtr->nextFreeIndex++;

	if (RecorderDataPtr->nextFreeIndex >= TRC_CFG_EVENT_BUFFER_SIZE)
	{
#if (TRC_CFG_SNAPSHOT_MODE == TRC_SNAPSHOT_MODE_RING_BUFFER)
		RecorderDataPtr->bufferIsFull = 1;
		RecorderDataPtr->nextFreeIndex = 0;
#else
		vTraceStop();
#endif
	}

#if (TRC_CFG_SNAPSHOT_MODE == TRC_SNAPSHOT_MODE_RING_BUFFER)
	prvCheckDataToBeOverwrittenForMultiEntryEvents(1);
#endif
}

/******************************************************************************
 * prvTraceGetDTS
 *
 * Returns a differential timestamp (DTS), i.e., the time since
 * last event, and creates an XTS event if the DTS does not fit in the
 * number of bits given. The XTS event holds the MSB bytes of the DTS.
 *
 * The parameter param_maxDTS should be 0xFF for 8-bit dts or 0xFFFF for
 * events with 16-bit dts fields.
 *****************************************************************************/
uint16_t prvTraceGetDTS(uint16_t param_maxDTS)
{
	static uint32_t old_timestamp = 0;
	XTSEvent* xts = 0;
	uint32_t dts = 0;
	uint32_t timestamp = 0;

	TRACE_ASSERT(param_maxDTS == 0xFF || param_maxDTS == 0xFFFF, "prvTraceGetDTS: Invalid value for param_maxDTS", 0);

	
	if (RecorderDataPtr->frequency == 0)
	{	
		if (timestampFrequency != 0)
		{
			/* If to override default TRC_HWTC_FREQ_HZ value with value set by vTraceSetFrequency */
			RecorderDataPtr->frequency = timestampFrequency / TRC_HWTC_DIVISOR;
		} 
		else if (init_hwtc_count != TRC_HWTC_COUNT)
		{
			/* If using default value and timer has been started. 
			Note: If the default frequency value set here would be incorrect, e.g.,
			if the timer has actually not been configured yet, override this 
			with vTraceSetFrequency.
			*/
			RecorderDataPtr->frequency = TRC_HWTC_FREQ_HZ / TRC_HWTC_DIVISOR;		
		}
		/* If no override (vTraceSetFrequency) and timer inactive -> no action */
	}
	
	/**************************************************************************
	* The below statements read the timestamp from the timer port module.
	* If necessary, whole seconds are extracted using division while the rest
	* comes from the modulo operation.
	**************************************************************************/
	
	prvTracePortGetTimeStamp(&timestamp);	
	
	/***************************************************************************
	* Since dts is unsigned the result will be correct even if timestamp has
	* wrapped around.
	***************************************************************************/
	dts = timestamp - old_timestamp;
	old_timestamp = timestamp;

	if (RecorderDataPtr->frequency > 0)
	{
		/* Check if dts > 1 second */
		if (dts > RecorderDataPtr->frequency)
		{
			/* More than 1 second has passed */
			RecorderDataPtr->absTimeLastEventSecond += dts / RecorderDataPtr->frequency;
			/* The part that is not an entire second is added to absTimeLastEvent */
			RecorderDataPtr->absTimeLastEvent += dts % RecorderDataPtr->frequency;
		}
		else
		{
			RecorderDataPtr->absTimeLastEvent += dts;
		}

		/* Check if absTimeLastEvent >= 1 second */
		if (RecorderDataPtr->absTimeLastEvent >= RecorderDataPtr->frequency)
		{
			/* RecorderDataPtr->absTimeLastEvent is more than or equal to 1 second, but always less than 2 seconds */
			RecorderDataPtr->absTimeLastEventSecond++;
			RecorderDataPtr->absTimeLastEvent -= RecorderDataPtr->frequency;
			/* RecorderDataPtr->absTimeLastEvent is now less than 1 second */
		}
	}
	else
	{
		/* Special case if the recorder has not yet started (frequency may be uninitialized, i.e., zero) */
		RecorderDataPtr->absTimeLastEvent = timestamp;
	}

	/* If the dts (time since last event) does not fit in event->dts (only 8 or 16 bits) */
	if (dts > param_maxDTS)
	{
		/* Create an XTS event (eXtended TimeStamp) containing the higher dts bits*/
		xts = (XTSEvent*) prvTraceNextFreeEventBufferSlot();

		if (xts != NULL)
		{
			if (param_maxDTS == 0xFFFF)
			{
				xts->type = XTS16;
				xts->xts_16 = (uint16_t)((dts / 0x10000) & 0xFFFF);
				xts->xts_8 = 0;
			}
			else if (param_maxDTS == 0xFF)
			{
				xts->type = XTS8;
				xts->xts_16 = (uint16_t)((dts / 0x100) & 0xFFFF);
				xts->xts_8 = (uint8_t)((dts / 0x1000000) & 0xFF);
			}
			else
			{
				prvTraceError("Bad param_maxDTS in prvTraceGetDTS");
			}
			prvTraceUpdateCounters();
		}
	}

	return (uint16_t)dts & param_maxDTS;
}

/*******************************************************************************
 * prvTraceLookupSymbolTableEntry
 *
 * Find an entry in the symbol table, return 0 if not present.
 *
 * The strings are stored in a byte pool, with four bytes of "meta-data" for
 * every string.
 * byte 0-1: index of next entry with same checksum (for fast lookup).
 * byte 2-3: reference to a symbol table entry, a label for vTracePrintF
 * format strings only (the handle of the destination channel).
 * byte 4..(4 + length): the string (object name or user event label), with
 * zero-termination
 ******************************************************************************/
traceString prvTraceLookupSymbolTableEntry(const char* name,
										 uint8_t crc6,
										 uint8_t len,
										 traceString chn)
{
	uint16_t i = RecorderDataPtr->SymbolTable.latestEntryOfChecksum[ crc6 ];

	TRACE_ASSERT(name != NULL, "prvTraceLookupSymbolTableEntry: name == NULL", (traceString)0);
	TRACE_ASSERT(len != 0, "prvTraceLookupSymbolTableEntry: len == 0", (traceString)0);

	while (i != 0)
	{
		if (RecorderDataPtr->SymbolTable.symbytes[i + 2] == (chn & 0x00FF))
		{
			if (RecorderDataPtr->SymbolTable.symbytes[i + 3] == (chn / 0x100))
			{
				if (RecorderDataPtr->SymbolTable.symbytes[i + 4 + len] == '\0')
				{
					if (strncmp((char*)(& RecorderDataPtr->SymbolTable.symbytes[i + 4]), name, len) == 0)
					{
						break; /* found */
					}
				}
			}
		}
		i = (uint16_t)(RecorderDataPtr->SymbolTable.symbytes[i] + (RecorderDataPtr->SymbolTable.symbytes[i + 1] * 0x100));
	}
	return i;
}

/*******************************************************************************
 * prvTraceCreateSymbolTableEntry
 *
 * Creates an entry in the symbol table, independent if it exists already.
 *
 * The strings are stored in a byte pool, with four bytes of "meta-data" for
 * every string.
 * byte 0-1: index of next entry with same checksum (for fast lookup).
 * byte 2-3: reference to a symbol table entry, a label for vTracePrintF
 * format strings only (the handle of the destination channel).
 * byte 4..(4 + length): the string (object name or user event label), with
 * zero-termination
 ******************************************************************************/
uint16_t prvTraceCreateSymbolTableEntry(const char* name,
										uint8_t crc6,
										uint8_t len,
										traceString channel)
{
	uint16_t ret = 0;

	TRACE_ASSERT(name != NULL, "prvTraceCreateSymbolTableEntry: name == NULL", 0);
	TRACE_ASSERT(len != 0, "prvTraceCreateSymbolTableEntry: len == 0", 0);

	if (RecorderDataPtr->SymbolTable.nextFreeSymbolIndex + len + 4 >= TRC_CFG_SYMBOL_TABLE_SIZE)
	{
		prvTraceError("Symbol table full. Increase TRC_CFG_SYMBOL_TABLE_SIZE in trcConfig.h");
		ret = 0;
	}
	else
	{

		RecorderDataPtr->SymbolTable.symbytes
			[ RecorderDataPtr->SymbolTable.nextFreeSymbolIndex] =
			(uint8_t)(RecorderDataPtr->SymbolTable.latestEntryOfChecksum[ crc6 ] & 0x00FF);

		RecorderDataPtr->SymbolTable.symbytes
			[ RecorderDataPtr->SymbolTable.nextFreeSymbolIndex + 1] =
			(uint8_t)(RecorderDataPtr->SymbolTable.latestEntryOfChecksum[ crc6 ] / 0x100);

		RecorderDataPtr->SymbolTable.symbytes
			[ RecorderDataPtr->SymbolTable.nextFreeSymbolIndex + 2] =
			(uint8_t)(channel & 0x00FF);

		RecorderDataPtr->SymbolTable.symbytes
			[ RecorderDataPtr->SymbolTable.nextFreeSymbolIndex + 3] =
			(uint8_t)(channel / 0x100);

		/* set name (bytes 4...4+len-1) */
		prvStrncpy((char*)&(RecorderDataPtr->SymbolTable.symbytes
			[ RecorderDataPtr->SymbolTable.nextFreeSymbolIndex + 4]), name, len);

		/* Set zero termination (at offset 4+len) */
		RecorderDataPtr->SymbolTable.symbytes
			[RecorderDataPtr->SymbolTable.nextFreeSymbolIndex + 4 + len] = '\0';

		/* store index of entry (for return value, and as head of LL[crc6]) */
		RecorderDataPtr->SymbolTable.latestEntryOfChecksum
			[ crc6 ] = (uint16_t)RecorderDataPtr->SymbolTable.nextFreeSymbolIndex;

		RecorderDataPtr->SymbolTable.nextFreeSymbolIndex += (uint32_t) (len + 5);

		ret = (uint16_t)(RecorderDataPtr->SymbolTable.nextFreeSymbolIndex - (uint8_t)(len + 5));
	}

	return ret;
}


/*******************************************************************************
 * prvTraceGetChecksum
 *
 * Calculates a simple 6-bit checksum from a string, used to index the string
 * for fast symbol table lookup.
 ******************************************************************************/
void prvTraceGetChecksum(const char *pname, uint8_t* pcrc, uint8_t* plength)
{
	unsigned char c;
	int length = 1;		/* Should be 1 to account for '\0' */
	int crc = 0;

	TRACE_ASSERT(pname != NULL, "prvTraceGetChecksum: pname == NULL", TRC_UNUSED);
	TRACE_ASSERT(pcrc != NULL, "prvTraceGetChecksum: pcrc == NULL", TRC_UNUSED);
	TRACE_ASSERT(plength != NULL, "prvTraceGetChecksum: plength == NULL", TRC_UNUSED);

	if (pname != (const char *) 0)
	{
		for (; (c = (unsigned char) *pname++) != '\0';)
		{
			crc += c;
			length++;
		}
	}
	*pcrc = (uint8_t)(crc & 0x3F);
	*plength = (uint8_t)length;
}

#if (TRC_CFG_USE_16BIT_OBJECT_HANDLES == 1)

static void prvTraceStoreXID(traceHandle handle); 

/******************************************************************************
 * prvTraceStoreXID
 *
 * Stores an XID (eXtended IDentifier) event.
 * This is used if an object/task handle is larger than 255.
 * The parameter "handle" is the full (16 bit) handle, assumed to be 256 or 
 * larger. Handles below 256 should not use this function.
 *
 * NOTE: this function MUST be called from within a critical section.
 *****************************************************************************/
static void prvTraceStoreXID(traceHandle handle)
{
	XPSEvent* xid;

	TRACE_ASSERT(handle >= 256, "prvTraceStoreXID: Handle < 256", TRC_UNUSED);

	xid = (XPSEvent*)prvTraceNextFreeEventBufferSlot();

	if (xid != NULL)
	{
		xid->type = XID;

		/* This function is (only) used when traceHandle is 16 bit... */
		xid->xps_16 = handle; 

		prvTraceUpdateCounters();
	}
}

static uint8_t prvTraceGet8BitHandle(traceHandle handle)
{
	if (handle > 255)
	{		
		prvTraceStoreXID(handle);
		/* The full handle (16 bit) is stored in the XID event. 
		This code (255) is used instead of zero (which is an error code).*/
		return 255; 
	}
	return (uint8_t)(handle & 0xFF);
}
#endif /*(TRC_CFG_USE_16BIT_OBJECT_HANDLES == 1)*/


/* If using DWT timestamping (default on ARM Cortex-M3, M4 and M7), make sure the DWT unit is initialized. */
#ifndef TRC_CFG_ARM_CM_USE_SYSTICK
#if ((TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_Cortex_M) && (defined (__CORTEX_M) && (__CORTEX_M >= 0x03)))
void prvTraceInitCortexM()
{
	/* Ensure that the DWT registers are unlocked and can be modified. */
	TRC_REG_ITM_LOCKACCESS = TRC_ITM_LOCKACCESS_UNLOCK;

	/* Make sure DWT is enabled, if supported */
	TRC_REG_DEMCR |= TRC_DEMCR_TRCENA;

	do{
		/* Verify that DWT is supported */
		if (TRC_REG_DEMCR == 0)
		{
			/* This function is called on Cortex-M3, M4 and M7 devices to initialize
			the DWT unit, assumed present. The DWT cycle counter is used for timestamping. 
			
			If the below error is produced, the DWT unit does not seem to be available.
			
			In that case, define the macro TRC_CFG_ARM_CM_USE_SYSTICK in your build
			to use SysTick timestamping instead, or define your own timestamping by 
			setting TRC_CFG_HARDWARE_PORT to TRC_HARDWARE_PORT_APPLICATION_DEFINED
			and make the necessary definitions, as explained in trcHardwarePort.h.*/
			
			prvTraceError("DWT unit not available, see code comment.");
			break;
		}

		/* Verify that DWT_CYCCNT is supported */
		if (TRC_REG_DWT_CTRL & TRC_DWT_CTRL_NOCYCCNT)
		{
			/* This function is called on Cortex-M3, M4 and M7 devices to initialize
			the DWT unit, assumed present. The DWT cycle counter is used for timestamping. 
			
			If the below error is produced, the cycle counter does not seem to be available.
			
			In that case, define the macro TRC_CFG_ARM_CM_USE_SYSTICK in your build
			to use SysTick timestamping instead, or define your own timestamping by 
			setting TRC_CFG_HARDWARE_PORT to TRC_HARDWARE_PORT_APPLICATION_DEFINED
			and make the necessary definitions, as explained in trcHardwarePort.h.*/

			prvTraceError("DWT_CYCCNT not available, see code comment.");
			break;
		}

		/* Reset the cycle counter */
		TRC_REG_DWT_CYCCNT = 0;

		/* Enable the cycle counter */
		TRC_REG_DWT_CTRL |= TRC_DWT_CTRL_CYCCNTENA;

	}while(0);	/* breaks above jump here */
}
#endif
#endif

/******************************************************************************
 * prvTracePortGetTimeStamp
 *
 * Returns the current time based on the HWTC macros which provide a hardware
 * isolation layer towards the hardware timer/counter.
 *
 * The HWTC macros and prvTracePortGetTimeStamp is the main porting issue
 * or the trace recorder library. Typically you should not need to change
 * the code of prvTracePortGetTimeStamp if using the HWTC macros.
 *
 ******************************************************************************/
void prvTracePortGetTimeStamp(uint32_t *pTimestamp)
{
	static uint32_t last_hwtc_count = 0;
	uint32_t hwtc_count = 0;

#if TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR
	/* systick based timer */
	static uint32_t last_traceTickCount = 0;
	uint32_t traceTickCount = 0;
#else /*TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR*/
	/* Free running timer */
	static uint32_t last_hwtc_rest = 0;
	uint32_t diff = 0;
	uint32_t diff_scaled = 0;
#endif /*TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR*/

	if (trace_disable_timestamp == 1)
	{
		if (pTimestamp)
			*pTimestamp = last_timestamp;
		return;
	}

	/* Retrieve TRC_HWTC_COUNT only once since the same value should be used all throughout this function. */
#if (TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_INCR)
	/* Get the increasing tick count */
	hwtc_count = TRC_HWTC_COUNT;
#elif (TRC_HWTC_TYPE == TRC_OS_TIMER_DECR || TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_DECR)
	/* Convert decreasing tick count into increasing tick count */
	hwtc_count = TRC_HWTC_PERIOD - TRC_HWTC_COUNT;
#else
	#error "TRC_HWTC_TYPE has unexpected value"
#endif

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Win32)
	/* The Win32 port uses ulGetRunTimeCounterValue for timestamping, which in turn
	uses QueryPerformanceCounter. That function is not always reliable when used over
	multiple threads. We must therefore handle rare cases where the timestamp is less
	than the previous. In practice, this should "never" roll over since the
	performance counter is 64 bit wide. */

	if (last_hwtc_count > hwtc_count)
	{
		hwtc_count = last_hwtc_count;
	}
#endif

#if (TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR)
	/* Timestamping is based on a timer that wraps at TRC_HWTC_PERIOD */
	if (last_traceTickCount - uiTraceTickCount - 1 < 0x80000000)
	{
		/* This means last_traceTickCount is higher than uiTraceTickCount,
		so we have previously compensated for a missed tick.
		Therefore we use the last stored value because that is more accurate. */
		traceTickCount = last_traceTickCount;
	}
	else
	{
		/* Business as usual */
		traceTickCount = uiTraceTickCount;
	}

	/* Check for overflow. May occur if the update of uiTraceTickCount has been
	delayed due to disabled interrupts. */
	if (traceTickCount == last_traceTickCount && hwtc_count < last_hwtc_count)
	{
		/* A trace tick has occurred but not been executed by the kernel, so we compensate manually. */
		traceTickCount++;
	}

	/* Check if the return address is OK, then we perform the calculation. */
	if (pTimestamp)
	{
		/* Get timestamp from trace ticks. Scale down the period to avoid unwanted overflows. */
		last_timestamp = traceTickCount * (TRC_HWTC_PERIOD / TRC_HWTC_DIVISOR);
		/* Increase timestamp by (hwtc_count + "lost hardware ticks from scaling down period") / TRC_HWTC_DIVISOR. */
		last_timestamp += (hwtc_count + traceTickCount * (TRC_HWTC_PERIOD % TRC_HWTC_DIVISOR)) / TRC_HWTC_DIVISOR;
	}
	/* Store the previous value */
	last_traceTickCount = traceTickCount;
	
#else /*(TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR)*/
	
	/* Timestamping is based on a free running timer */
	/* This part handles free running clocks that can be scaled down to avoid too large DTS values.
	Without this, the scaled timestamp will incorrectly wrap at (2^32 / TRC_HWTC_DIVISOR) ticks.
	The scaled timestamp returned from this function is supposed to go from 0 -> 2^32, which in real time would represent (0 -> 2^32 * TRC_HWTC_DIVISOR) ticks. */
	
	/* First we see how long time has passed since the last timestamp call, and we also add the ticks that was lost when we scaled down the last time. */
	diff = (hwtc_count - last_hwtc_count) + last_hwtc_rest;
	
	/* Scale down the diff */
	diff_scaled = diff / TRC_HWTC_DIVISOR;
	
	/* Find out how many ticks were lost when scaling down, so we can add them the next time */
	last_hwtc_rest = diff % TRC_HWTC_DIVISOR;

	/* We increase the scaled timestamp by the scaled amount */
	last_timestamp += diff_scaled;
#endif /*(TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR)*/

	/* Is anyone interested in the results? */
	if (pTimestamp)
		*pTimestamp = last_timestamp;

	/* Store the previous value */
	last_hwtc_count = hwtc_count;
}

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)*/
