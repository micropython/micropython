/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcRecorder.h
 *
 * The public API of the trace recorder.
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

#ifndef TRC_RECORDER_H
#define TRC_RECORDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdint.h>
#include "trcConfig.h"
#include "trcPortDefines.h"



#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
typedef uint16_t traceString;
typedef uint8_t traceUBChannel;
typedef uint8_t traceObjectClass;

#if (TRC_CFG_USE_16BIT_OBJECT_HANDLES == 1)
typedef uint16_t traceHandle;
#else
typedef uint8_t traceHandle;
#endif

#include "trcHardwarePort.h"
#include "trcKernelPort.h"

#endif

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

typedef const char* traceString;
typedef const void* traceHandle;

#include "trcHardwarePort.h"
#include "trcStreamingPort.h"
#include "trcKernelPort.h"

#endif

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

/******************************************************************************/
/*** Common API - both Snapshot and Streaming mode ****************************/
/******************************************************************************/

/******************************************************************************
* vTraceEnable(int startOption);
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
* TRC_START_AWAIT_HOST: For streaming mode only. Initializes the trace recorder
* if necessary and waits for a Start command from Tracealyzer ("Start Recording"
* button). This call is intentionally blocking! By calling vTraceEnable with
* this option from the startup code, you start tracing at this point and capture
* the early events.
*
* TRC_INIT: Initializes the trace recorder, but does not start the tracing.
* In snapshot mode, this must be followed by a vTraceEnable(TRC_START) sometime
* later.
*
* Usage examples:
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
* Streaming trace, from startup:
*	<board init>
*	vTraceEnable(TRC_START_AWAIT_HOST); // Blocks!
*	<RTOS init>
*
* Streaming trace, from a later point:
*	<board startup>
*	vTraceEnable(TRC_INIT);
*	<RTOS startup>
*
******************************************************************************/
void vTraceEnable(int startOption);

/******************************************************************************
 * vTracePrintF
 *
 * Generates "User Events", with formatted text and data, similar to a "printf".
 * User Events can be used for very efficient logging from your application code.
 * It is very fast since the actual string formatting is done on the host side,
 * when the trace is displayed. The execution time is just some microseconds on
 * a 32-bit MCU.
 *
 * User Events are shown as yellow labels in the main trace view of $PNAME.
 *
 * An advantage of User Events is that data can be plotted in the "User Event
 * Signal Plot" view, visualizing any data you log as User Events, discrete
 * states or control system signals (e.g. system inputs or outputs).
 *
 * You may group User Events into User Event Channels. The yellow User Event
 * labels show the logged string, preceded by the channel name within brackets.
 *
 * Example:
 *
 *  "[MyChannel] Hello World!"
 *
 * The User Event Channels are shown in the View Filter, which makes it easy to
 * select what User Events you wish to display. User Event Channels are created
 * using xTraceRegisterString().
 *
 * Example:
 *
 *	 traceString adc_uechannel = xTraceRegisterString("ADC User Events");
 *	 ...
 *	 vTracePrintF(adc_uechannel,
 *				 "ADC channel %d: %d volts",
 *				 ch, adc_reading);
 *
 * The following format specifiers are supported in both modes:
 * %d - signed integer.
 * %u - unsigned integer.
 * %X - hexadecimal, uppercase.
 * %x - hexadecimal, lowercase.
 * %s - string (see comment below)
 *
 * For integer formats (%d, %u, %x, %X) you may also use width and padding.
 * If using -42 as data argument, two examples are:
 *    "%05d" -> "-0042"
 *     "%5d" -> "  -42".
 *
 * String arguments are supported in both snapshot and streaming, but in streaming
 * mode you need to use xTraceRegisterString and use the returned traceString as
 * the argument. In snapshot you simply provide a char* as argument.
 *
 * Snapshot: vTracePrintF(myChn, "my string: %s", str);
 * Streaming: vTracePrintF(myChn, "my string: %s", xTraceRegisterString(str));
 *
 * In snapshot mode you can specify 8-bit or 16-bit arguments to reduce RAM usage:
 *     %hd -> 16 bit (h) signed integer (d).
 *     %bu -> 8 bit (b) unsigned integer (u).
 *
 * However, in streaming mode all data arguments are assumed to be 32 bit wide.
 * Width specifiers (e.g. %hd) are accepted but ignored (%hd treated like %d).
 *
 * The maximum event size also differs between the modes. In streaming this is
 * limited by a maximum payload size of 52 bytes, including format string and
 * data arguments. So if using one data argument, the format string is limited
 * to 48 byte, etc. If this is exceeded,  the format string is truncated and you
 * get a warning in Tracealyzer.
 *
 * In snapshot mode you are limited to maximum 15 arguments, that must not exceed
 * 32 bytes in total (not counting the format string). If exceeded, the recorder
 * logs an internal error (displayed when opening the trace) and stops recording.
 *
 ******************************************************************************/
void vTracePrintF(traceString chn, const char* fmt, ...);

/******************************************************************************
* vTracePrint
*
* A faster version of vTracePrintF, that only allows for logging a string.
*
* Example:
*
*	 traceString chn = xTraceRegisterString("MyChannel");
*	 ...
*	 vTracePrint(chn, "Hello World!");
*
******************************************************************************/
void vTracePrint(traceString chn, const char* str);

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
traceString xTraceRegisterString(const char* name);

/*******************************************************************************
 * vTraceSet...Name(void* object, const char* name)
 *
 * Parameter object: pointer to the kernel object that shall be named
 * Parameter name: the name to set
 *
 * Kernel-specific functions for setting names of kernel objects, for display in
 * Tracealyzer.
 *
 * See trcKernelPort.h for details (since kernel-specific)
 ******************************************************************************/

/*******************************************************************************
 * vTraceExclude...
 *
 * Kernel-specific macros for excluding specified events from the trace. Allows
 * for capturing longer traces in snapshot mode by selective tracing.
 *
 * See trcKernelPort.h for details (kernel-specific)
 ******************************************************************************/

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
traceHandle xTraceSetISRProperties(const char* name, uint8_t priority);

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
void vTraceStoreISRBegin(traceHandle handle);

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
void vTraceStoreISREnd(int isTaskSwitchRequired);

/*******************************************************************************
 * vTraceInstanceFinishNow
 *
 * Creates an event that ends the current task instance at this very instant.
 * This makes the viewer to splits the current fragment at this point and begin
 * a new actor instance, even if no task-switch has occurred.
 *****************************************************************************/
void vTraceInstanceFinishedNow(void);

/*******************************************************************************
 * vTraceInstanceFinishedNext
 *
 * Marks the current "task instance" as finished on the next kernel call.
 *
 * If that kernel call is blocking, the instance ends after the blocking event
 * and the corresponding return event is then the start of the next instance.
 * If the kernel call is not blocking, the viewer instead splits the current
 * fragment right before the kernel call, which makes this call the first event
 * of the next instance.
 *****************************************************************************/
void vTraceInstanceFinishedNext(void);

/*******************************************************************************
 * xTraceGetLastError
 *
 * Returns the last error, if any.
 *****************************************************************************/
const char* xTraceGetLastError(void);

/*******************************************************************************
 * vTraceClearError
 *
 * Clears any errors.
 *****************************************************************************/
void vTraceClearError(void);

/*******************************************************************************
* vTraceStop
*
* Stops the recording. Intended for snapshot mode or if streaming without
* Tracealyzer control (e.g., to a device file system).
******************************************************************************/
void vTraceStop(void);

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
void vTraceSetFrequency(uint32_t frequency);

/*******************************************************************************
* vTraceSetRecorderDataBuffer
*
* The trcConfig.h setting TRC_CFG_RECORDER_BUFFER_ALLOCATION allows for selecting
* custom allocation (TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM), which allows you to
* control where the recorder trace buffer is allocated.
*
* When custom allocation is selected, use TRC_ALLOC_CUSTOM_BUFFER to make the
* allocation (in global context) and then call vTraceSetRecorderDataBuffer to
* register the allocated buffer. This supports both snapshot and streaming,
* and has no effect if using other allocation modes than CUSTOM.
*
* NOTE: vTraceSetRecorderDataBuffer must be called before vTraceEnable.
******************************************************************************/
#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM)
void vTraceSetRecorderDataBuffer(void* pRecorderData);
#else
#define vTraceSetRecorderDataBuffer(pRecorderData)
#endif


/*******************************************************************************
* TRC_ALLOC_CUSTOM_BUFFER
*
* If using custom allocation of the trace buffer (i.e., your trcConfig.h has the
* setting TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM), this macro allows you to declare
* the trace buffer in a portable way that works both in snapshot and streaming.
*
* This macro has no effect if using another allocation mode, so you can easily
* switch between different recording modes and configurations, using the same
* initialization code.
*
* This translates to a single static allocation, on which you can apply linker
* directives to place it in a particular memory region.
* - Snapshot mode: "RecorderDataType <name>"
* - Streaming mode: "char <name> [<size>]", with <size> from trcStreamingPort.h.
*
* Example:
*
*   // GCC example: place myTraceBuffer in section .tz, defined in the .ld file.
*   TRC_ALLOC_CUSTOM_BUFFER(myTraceBuffer) __attribute__((section(".tz")));
*
*   int main(void)
*   {
*      ...
*      vTraceSetRecorderDataBuffer(&myTraceBuffer); // Note the "&"
*      ...
*      vTraceEnable(TRC_INIT); // Initialize the data structure
*
******************************************************************************/
#ifndef TRC_ALLOC_CUSTOM_BUFFER
/* Definition for snapshot mode only. Also defined in trcStreamingPort.h */
#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM)
#define TRC_ALLOC_CUSTOM_BUFFER(bufname) RecorderDataType bufname;
#else
#define TRC_ALLOC_CUSTOM_BUFFER(bufname)
#endif
#endif


#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

/******************************************************************************/
/*** Extended API for Snapshot mode *******************************************/
/******************************************************************************/

/******************************************************************************
* TRACE_STOP_HOOK - Hook Pointer Data Type
*
* Declares a data type for a call back function that will be invoked whenever
* the recorder is stopped.
*
* Snapshot mode only!
******************************************************************************/
typedef void(*TRACE_STOP_HOOK)(void);

/*******************************************************************************
* vTraceStopHookPtr
*
* Points to a call back function that is called from vTraceStop().
*
* Snapshot mode only!
******************************************************************************/
extern TRACE_STOP_HOOK vTraceStopHookPtr;

/*******************************************************************************
* vTraceSetStopHook
*
* Sets a function to be called when the recorder is stopped.
*
* Snapshot mode only!
******************************************************************************/
void vTraceSetStopHook(TRACE_STOP_HOOK stopHookFunction);

/*******************************************************************************
* uiTraceStart
*
* [DEPRECATED] Use vTraceEnable instead.
*
* Starts the recorder. The recorder will not be started if an error has been
* indicated using prvTraceError, e.g. if any of the Nx constants in
* trcSnapshotConfig.h has a too small value (TRC_CFG_NTASK, TRC_CFG_NQUEUE, etc).
*
* Returns 1 if the recorder was started successfully.
* Returns 0 if the recorder start was prevented due to a previous internal
* error. In that case, check xTraceGetLastError to get the error message.
* Any error message is also presented when opening a trace file.
*
*
* Snapshot mode only!
******************************************************************************/
uint32_t uiTraceStart(void);

/*******************************************************************************
* vTraceStart
*
* [DEPRECATED] Use vTraceEnable instead.
*
* Starts the recorder. The recorder will not be started if an error has been
* indicated using prvTraceError, e.g. if any of the Nx constants in
* trcSnapshotConfig.h has a too small value (TRC_CFG_NTASK, TRC_CFG_NQUEUE, etc).
*
* Snapshot mode only!
******************************************************************************/
void vTraceStart(void);

/*******************************************************************************
* vTraceClear
*
* Resets the recorder. Only necessary if a restart is desired - this is not
* needed in the startup initialization.
*
* Snapshot mode only!
******************************************************************************/
void vTraceClear(void);


/*****************************************************************************/
/*** INTERNAL SNAPSHOT FUNCTIONS *********************************************/
/*****************************************************************************/

#undef INCLUDE_xTaskGetSchedulerState
#define INCLUDE_xTaskGetSchedulerState 1

#undef INCLUDE_xTaskGetCurrentTaskHandle
#define INCLUDE_xTaskGetCurrentTaskHandle 1

#ifndef TRC_CFG_INCLUDE_OBJECT_DELETE
#define TRC_CFG_INCLUDE_OBJECT_DELETE 0
#endif

#ifndef TRC_CFG_INCLUDE_READY_EVENTS
#define TRC_CFG_INCLUDE_READY_EVENTS 1
#endif

#ifndef TRC_CFG_INCLUDE_OSTICK_EVENTS
#define TRC_CFG_INCLUDE_OSTICK_EVENTS 0
#endif

#define TRC_UNUSED

#if (TRC_CFG_INCLUDE_OBJECT_DELETE == 1)
/* This macro will remove the task and store it in the event buffer */
#undef trcKERNEL_HOOKS_TASK_DELETE
#define trcKERNEL_HOOKS_TASK_DELETE(SERVICE, pxTCB) \
	prvTraceStoreKernelCall(TRACE_GET_TASK_EVENT_CODE(SERVICE, SUCCESS, CLASS, pxTCB), TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB)); \
	prvTraceStoreObjectNameOnCloseEvent(TRACE_GET_TASK_NUMBER(pxTCB), TRACE_CLASS_TASK); \
	prvTraceStoreObjectPropertiesOnCloseEvent(TRACE_GET_TASK_NUMBER(pxTCB), TRACE_CLASS_TASK); \
	prvTraceSetPriorityProperty(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), TRACE_GET_TASK_PRIORITY(pxTCB)); \
	prvTraceSetObjectState(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), TASK_STATE_INSTANCE_NOT_ACTIVE); \
	prvTraceFreeObjectHandle(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB));
#else /*(TRC_CFG_INCLUDE_OBJECT_DELETE == 1)*/
#undef trcKERNEL_HOOKS_TASK_DELETE
#define trcKERNEL_HOOKS_TASK_DELETE(SERVICE, pxTCB)
#endif /*(TRC_CFG_INCLUDE_OBJECT_DELETE == 1)*/

#if (TRC_CFG_INCLUDE_OBJECT_DELETE == 1)
/* This macro will remove the object and store it in the event buffer */
#undef trcKERNEL_HOOKS_OBJECT_DELETE
#define trcKERNEL_HOOKS_OBJECT_DELETE(SERVICE, CLASS, pxObject) \
	prvTraceStoreKernelCall(TRACE_GET_OBJECT_EVENT_CODE(SERVICE, SUCCESS, CLASS, pxObject), TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject)); \
	prvTraceStoreObjectNameOnCloseEvent(TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject)); \
	prvTraceStoreObjectPropertiesOnCloseEvent(TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject)); \
	prvTraceFreeObjectHandle(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject));
#else /*TRC_CFG_INCLUDE_OBJECT_DELETE*/
#undef trcKERNEL_HOOKS_OBJECT_DELETE
#define trcKERNEL_HOOKS_OBJECT_DELETE(SERVICE, CLASS, pxObject)
#endif /*TRC_CFG_INCLUDE_OBJECT_DELETE*/

/* This macro will create a task in the object table */
#undef trcKERNEL_HOOKS_TASK_CREATE
#define trcKERNEL_HOOKS_TASK_CREATE(SERVICE, CLASS, pxTCB) \
	TRACE_SET_TASK_NUMBER(pxTCB) \
	prvTraceSetObjectName(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), TRACE_GET_TASK_NAME(pxTCB)); \
	prvTraceSetPriorityProperty(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), TRACE_GET_TASK_PRIORITY(pxTCB)); \
	prvTraceStoreKernelCall(TRACE_GET_TASK_EVENT_CODE(SERVICE, SUCCESS, CLASS, pxTCB), TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB));

/* This macro will create a failed create call to create a task */
#undef trcKERNEL_HOOKS_TASK_CREATE_FAILED
#define trcKERNEL_HOOKS_TASK_CREATE_FAILED(SERVICE, CLASS) \
	prvTraceStoreKernelCall(TRACE_GET_TASK_EVENT_CODE(SERVICE, FAILED, CLASS, 0), TRACE_CLASS_TASK, 0);

/* This macro will setup a task in the object table */
#undef trcKERNEL_HOOKS_OBJECT_CREATE
#define trcKERNEL_HOOKS_OBJECT_CREATE(SERVICE, CLASS, pxObject)\
	TRACE_SET_OBJECT_NUMBER(CLASS, pxObject);\
	prvMarkObjectAsUsed(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject),  TRACE_GET_OBJECT_NUMBER(CLASS, pxObject));\
	prvTraceStoreKernelCall(TRACE_GET_OBJECT_EVENT_CODE(SERVICE, SUCCESS, CLASS, pxObject), TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject)); \
	prvTraceSetObjectState(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), 0);

/* This macro will create a failed create call to create an object */
#undef trcKERNEL_HOOKS_OBJECT_CREATE_FAILED
#define trcKERNEL_HOOKS_OBJECT_CREATE_FAILED(SERVICE, CLASS, kernelClass) \
	prvTraceStoreKernelCall(TRACE_GET_CLASS_EVENT_CODE(SERVICE, FAILED, CLASS, kernelClass), TRACE_GET_CLASS_TRACE_CLASS(CLASS, kernelClass), 0);

/* This macro will create a call to a kernel service with a certain result, with an object as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE
#define trcKERNEL_HOOKS_KERNEL_SERVICE(SERVICE, RESULT, CLASS, pxObject) \
	prvTraceStoreKernelCall(TRACE_GET_OBJECT_EVENT_CODE(SERVICE, RESULT, CLASS, pxObject), TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject));

/* This macro will set the state for an object */
#undef trcKERNEL_HOOKS_SET_OBJECT_STATE
#define trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, pxObject, STATE) \
	prvTraceSetObjectState(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), STATE);

/* This macro will flag a certain task as a finished instance */
#undef trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED
#define trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED() \
	prvTraceSetTaskInstanceFinished(TRACE_GET_TASK_NUMBER(TRACE_GET_CURRENT_TASK()));

#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
/* This macro will create an event to indicate that a task became Ready */
#undef trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE
#define trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE(pxTCB) \
	prvTraceStoreTaskReady(TRACE_GET_TASK_NUMBER(pxTCB));
#else /*(TRC_CFG_INCLUDE_READY_EVENTS == 1)*/
#undef trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE
#define trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE(pxTCB)
#endif /*(TRC_CFG_INCLUDE_READY_EVENTS == 1)*/

/* This macro will update the internal tick counter and call prvTracePortGetTimeStamp(0) to update the internal counters */
#undef trcKERNEL_HOOKS_INCREMENT_TICK
#define trcKERNEL_HOOKS_INCREMENT_TICK() \
	{ extern uint32_t uiTraceTickCount; uiTraceTickCount++; prvTracePortGetTimeStamp(0); }

#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)
/* This macro will create an event indicating that the OS tick count has increased */
#undef trcKERNEL_HOOKS_NEW_TIME
#define trcKERNEL_HOOKS_NEW_TIME(SERVICE, xValue) \
	prvTraceStoreKernelCallWithNumericParamOnly(SERVICE, xValue);
#else /*(TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)*/
#undef trcKERNEL_HOOKS_NEW_TIME
#define trcKERNEL_HOOKS_NEW_TIME(SERVICE, xValue)
#endif /*(TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)*/

/* This macro will create a task switch event to the currently executing task */
#undef trcKERNEL_HOOKS_TASK_SWITCH
#define trcKERNEL_HOOKS_TASK_SWITCH( pxTCB ) \
	prvTraceStoreTaskswitch(TRACE_GET_TASK_NUMBER(pxTCB));

/* This macro will create an event to indicate that the task has been suspended */
#undef trcKERNEL_HOOKS_TASK_SUSPEND
#define trcKERNEL_HOOKS_TASK_SUSPEND(SERVICE, pxTCB) \
	prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB)); \
	prvTraceSetTaskInstanceFinished((uint8_t)TRACE_GET_TASK_NUMBER(pxTCB));

/* This macro will create an event to indicate that a task has called a wait/delay function */
#undef trcKERNEL_HOOKS_TASK_DELAY
#define trcKERNEL_HOOKS_TASK_DELAY(SERVICE, pxTCB, xValue) \
	prvTraceStoreKernelCallWithNumericParamOnly(SERVICE, xValue); \
	prvTraceSetTaskInstanceFinished((uint8_t)TRACE_GET_TASK_NUMBER(pxTCB));

/* This macro will create an event to indicate that a task has gotten its priority changed */
#undef trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE
#define trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(SERVICE, pxTCB, uxNewPriority) \
	prvTraceStoreKernelCallWithParam(SERVICE, TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), prvTraceGetPriorityProperty(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB)));\
	prvTraceSetPriorityProperty(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), (uint8_t)uxNewPriority);

/* This macro will create an event to indicate that the task has been resumed */
#undef trcKERNEL_HOOKS_TASK_RESUME
#define trcKERNEL_HOOKS_TASK_RESUME(SERVICE, pxTCB) \
	prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB));

#undef trcKERNEL_HOOKS_TIMER_EVENT
#define trcKERNEL_HOOKS_TIMER_EVENT(SERVICE, pxTimer) \
	prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TIMER, TRACE_GET_TIMER_NUMBER(pxTimer));

/* This macro will create a timer in the object table and assign the timer a trace handle (timer number).*/
#undef trcKERNEL_HOOKS_TIMER_CREATE
#define trcKERNEL_HOOKS_TIMER_CREATE(SERVICE, pxTimer) \
TRACE_SET_TIMER_NUMBER(pxTimer); \
prvTraceSetObjectName(TRACE_CLASS_TIMER, TRACE_GET_TIMER_NUMBER(pxTimer), TRACE_GET_TIMER_NAME(pxTimer)); \
prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TIMER, TRACE_GET_TIMER_NUMBER(pxTimer));

#undef trcKERNEL_HOOKS_TIMER_DELETE
#define trcKERNEL_HOOKS_TIMER_DELETE(SERVICE, pxTimer) \
prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TIMER, TRACE_GET_TIMER_NUMBER(pxTimer)); \
prvTraceStoreObjectNameOnCloseEvent(TRACE_GET_TIMER_NUMBER(pxTimer), TRACE_CLASS_TIMER); \
prvTraceStoreObjectPropertiesOnCloseEvent(TRACE_GET_TIMER_NUMBER(pxTimer), TRACE_CLASS_TIMER); \
prvTraceFreeObjectHandle(TRACE_CLASS_TIMER, TRACE_GET_TIMER_NUMBER(pxTimer));

#if !defined TRC_CFG_INCLUDE_READY_EVENTS || TRC_CFG_INCLUDE_READY_EVENTS == 1
	void prvTraceSetReadyEventsEnabled(int status);
	void prvTraceStoreTaskReady(traceHandle handle);
#else
	#define prvTraceSetReadyEventsEnabled(status)
#endif

void prvTraceStoreLowPower(uint32_t flag);

void prvTraceStoreTaskswitch(traceHandle task_handle);


#if (TRC_CFG_SCHEDULING_ONLY == 0)

void prvTraceStoreKernelCall(uint32_t eventcode, traceObjectClass objectClass, uint32_t byteParam);

void prvTraceStoreKernelCallWithNumericParamOnly(uint32_t evtcode, uint32_t param);

void prvTraceStoreKernelCallWithParam(uint32_t evtcode, traceObjectClass objectClass,
									uint32_t objectNumber, uint32_t param);
#else

#define prvTraceStoreKernelCall(eventcode, objectClass, byteParam) {}
#define prvTraceStoreKernelCallWithNumericParamOnly(evtcode, param) {}
#define prvTraceStoreKernelCallWithParam(evtcode, objectClass, objectNumber, param) {}

#endif

void prvTraceSetTaskInstanceFinished(traceHandle handle);

void prvTraceSetPriorityProperty(uint8_t objectclass, traceHandle id, uint8_t value);

uint8_t prvTraceGetPriorityProperty(uint8_t objectclass, traceHandle id);

void prvTraceSetObjectState(uint8_t objectclass, traceHandle id, uint8_t value);

void prvMarkObjectAsUsed(traceObjectClass objectclass, traceHandle handle);


#if (TRC_CFG_INCLUDE_OBJECT_DELETE == 1)

void prvTraceStoreObjectNameOnCloseEvent(traceHandle handle,
										traceObjectClass objectclass);

void prvTraceStoreObjectPropertiesOnCloseEvent(traceHandle handle,
											 traceObjectClass objectclass);
#endif

/* Internal constants for task state */
#define TASK_STATE_INSTANCE_NOT_ACTIVE 0
#define TASK_STATE_INSTANCE_ACTIVE 1


#if (TRC_CFG_INCLUDE_ISR_TRACING == 0)

//void prvTraceIncreaseISRActive(void);

//void prvTraceDecreaseISRActive(void);
#undef vTraceSetISRProperties
#define vTraceSetISRProperties(handle, name, priority)

#undef vTraceStoreISRBegin
#define vTraceStoreISRBegin(x) (void)x

#undef vTraceStoreISREnd
#define vTraceStoreISREnd(x) (void)x

#undef xTraceSetISRProperties
#define xTraceSetISRProperties(name, priority) 0

#endif /*(TRC_CFG_INCLUDE_ISR_TRACING == 0)*/

/*******************************************************************************
 * xTraceGetTraceBuffer
 *
 * Returns a pointer to the recorder data structure. Use this together with
 * uiTraceGetTraceBufferSize if you wish to implement an own store/upload
 * solution, e.g., in case a debugger connection is not available for uploading
 * the data.
 ******************************************************************************/
void* xTraceGetTraceBuffer(void);

/*******************************************************************************
 * uiTraceGetTraceBufferSize
 *
 * Gets the size of the recorder data structure. For use together with
 * vTraceGetTraceBuffer if you wish to implement an own store/upload solution,
 * e.g., in case a debugger connection is not available for uploading the data.
 ******************************************************************************/
uint32_t uiTraceGetTraceBufferSize(void);

#if (TRC_CFG_SCHEDULING_ONLY == 1)
#undef TRC_CFG_INCLUDE_USER_EVENTS
#define TRC_CFG_INCLUDE_USER_EVENTS 0
#endif /*(TRC_CFG_SCHEDULING_ONLY == 1)*/

#if ((TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_SCHEDULING_ONLY == 0))

#if (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)
traceUBChannel xTraceRegisterUBChannel(traceString channel, traceString formatStr);
void vTraceUBData(traceUBChannel channel, ...);
void vTraceUBEvent(traceUBChannel channel);
#endif /*(TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)*/

#else /*((TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_SCHEDULING_ONLY == 0))*/

#undef vTracePrint
#define vTracePrint(chn, ...) (void)chn
#undef vTracePrintF
#define vTracePrintF(chn, ...) (void)chn
#undef xTraceRegisterString
#define xTraceRegisterString(x) 0; (void)x;
#undef xTraceRegisterChannelFormat
#define xTraceRegisterChannelFormat(eventLabel, formatStr) 0
#undef vTraceUBData
#define vTraceUBData(label, ...) {}
#undef vTraceChannelPrint
#define vTraceChannelPrint(label) {}

#endif /*(TRC_CFG_INCLUDE_USER_EVENTS == 1)*/

#define NEventCodes 0x100

/* Our local critical sections for the recorder */
#define trcCRITICAL_SECTION_BEGIN() {TRACE_ENTER_CRITICAL_SECTION(); recorder_busy++;}
#define trcCRITICAL_SECTION_END() {recorder_busy--; TRACE_EXIT_CRITICAL_SECTION();}

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_Cortex_M)
	#define trcSR_ALLOC_CRITICAL_SECTION_ON_CORTEX_M_ONLY TRACE_ALLOC_CRITICAL_SECTION
	#define trcCRITICAL_SECTION_BEGIN_ON_CORTEX_M_ONLY trcCRITICAL_SECTION_BEGIN
	#define trcCRITICAL_SECTION_END_ON_CORTEX_M_ONLY trcCRITICAL_SECTION_END
#else
	#define trcSR_ALLOC_CRITICAL_SECTION_ON_CORTEX_M_ONLY() {}
	#define trcCRITICAL_SECTION_BEGIN_ON_CORTEX_M_ONLY() recorder_busy++;
	#define trcCRITICAL_SECTION_END_ON_CORTEX_M_ONLY() recorder_busy--;
#endif

/* Structure to handle the exclude flags for all objects and tasks. We add some extra objects since index 0 is not used for each object class. */
extern uint8_t trcExcludedObjects[(TRACE_KERNEL_OBJECT_COUNT + TRACE_NCLASSES) / 8 + 1];

/* Structure to handle the exclude flags for all event codes */
extern uint8_t trcExcludedEventCodes[NEventCodes / 8 + 1];

/******************************************************************************
 * ObjectHandleStack
 * This data-structure is used to provide a mechanism for 1-byte trace object
 * handles. This way, only 1 byte is necessary instead of 4 bytes (a pointer)
 * when storing a reference to an object. This allows for up to 255 objects of
 * each object class active at any given moment. There can be more "historic"
 * objects, that have been deleted - that number is only limited by the size of
 * the symbol table.
 * Note that handle zero (0) is not used, it is a code for an invalid handle.
 *
 * This data structure keeps track of the FREE handles, not the handles in use.
 * This data structure contains one stack per object class. When a handle is
 * allocated to an object, the next free handle is popped from the stack. When
 * a handle is released (on object delete), it is pushed back on the stack.
 * Note that there is no initialization code that pushed the free handles
 * initially, that is not necessary due to the following optimization:
 *
 * The stack of handles (objectHandles) is initially all zeros. Since zero
 * is not a valid handle, that is a signal of additional handles needed.
 * If a zero is received when popping a new handle, it is replaced by the
 * index of the popped handle instead.
 *
 *****************************************************************************/
typedef struct
{
	/* For each object class, the index of the next handle to allocate */
	uint16_t indexOfNextAvailableHandle[ TRACE_NCLASSES ];

	/* The lowest index of this class (constant) */
	uint16_t lowestIndexOfClass[ TRACE_NCLASSES ];

	/* The highest index of this class (constant) */
	uint16_t highestIndexOfClass[ TRACE_NCLASSES ];

	/* The highest use count for this class (for statistics) */
	uint16_t handleCountWaterMarksOfClass[ TRACE_NCLASSES ];

	/* The free object handles - a set of stacks within this array */
	traceHandle objectHandles[ TRACE_KERNEL_OBJECT_COUNT ];

} objectHandleStackType;

extern objectHandleStackType objectHandleStacks;

/******************************************************************************
 * Object Property Table
 * The Object Table contains name and other properties of the objects (tasks,
 * queues, mutexes, etc). The below data structures defines the properties of
 * each object class and are used to cast the byte buffer into a cleaner format.
 *
 * The values in the object table are continuously overwritten and always
 * represent the current state. If a property is changed during runtime, the OLD
 * value should be stored in the trace buffer, not the new value (since the new
 * value is found in the Object Property Table).
 * For close events this mechanism is the old names are stored in the symbol
 * table), for "priority set" (the old priority is stored in the event data)
 * and for "isActive", where the value decides if the task switch event type
 * should be "new" or "resume".
 ******************************************************************************/

typedef struct
{
	/* = NCLASSES */
	uint32_t NumberOfObjectClasses;

	uint32_t ObjectPropertyTableSizeInBytes;

	/* This is used to calculate the index in the dynamic object table
	(handle - 1 - nofStaticObjects = index)*/
#if (TRC_CFG_USE_16BIT_OBJECT_HANDLES == 1)
	traceHandle NumberOfObjectsPerClass[2*((TRACE_NCLASSES+1)/2)];
#else
	traceHandle NumberOfObjectsPerClass[4*((TRACE_NCLASSES+3)/4)];
#endif

	/* Allocation size rounded up to the closest multiple of 4 */
	uint8_t NameLengthPerClass[ 4*((TRACE_NCLASSES+3)/4) ];

	uint8_t TotalPropertyBytesPerClass[ 4*((TRACE_NCLASSES+3)/4) ];

	/* Allocation size rounded up to the closest multiple of 2 */
	uint16_t StartIndexOfClass[ 2*((TRACE_NCLASSES+1)/2) ];

	/* The actual handles issued, should be Initiated to all zeros */
	uint8_t objbytes[ 4*((TRACE_OBJECT_TABLE_SIZE+3)/4) ];
} ObjectPropertyTableType;

/* Symbol table data structure */
typedef struct
{
	/* = SYMBOL_HISTORY_TABLE_SIZE_IN_BYTES */
	uint32_t symTableSize;

	/* Entry 0 is reserved. Any reference to entry 0 implies NULL*/
	uint32_t nextFreeSymbolIndex;

	/* Size rounded up to closest multiple of 4, to avoid alignment issues*/
	uint8_t symbytes[4*((TRC_CFG_SYMBOL_TABLE_SIZE+3)/4)];

	/* Used for lookups - Up to 64 linked lists within the symbol table
	connecting all entries with the same 6 bit checksum.
	This field holds the current list heads. Should be initiated to zeros */
	uint16_t latestEntryOfChecksum[64];
} symbolTableType;


/*******************************************************************************
 * The data structures of the different events, all 4 bytes long
 ******************************************************************************/

typedef struct
{
	uint8_t type;
	uint8_t objHandle;
	uint16_t dts;	/* differential timestamp - time since last event */
} TSEvent, TREvent;

typedef struct
{
	uint8_t type;
	uint8_t dummy;
	uint16_t dts;	/* differential timestamp - time since last event */
} LPEvent;

typedef struct
{
	uint8_t type;
	uint8_t objHandle;
	uint16_t dts;	/* differential timestamp - time since last event */
} KernelCall;

typedef struct
{
	uint8_t type;
	uint8_t objHandle;
	uint8_t param;
	uint8_t dts;	/* differential timestamp - time since last event */
} KernelCallWithParamAndHandle;

typedef struct
{
	uint8_t type;
	uint8_t dts;	/* differential timestamp - time since last event */
	uint16_t param;
} KernelCallWithParam16;

typedef struct
{
	uint8_t type;
	uint8_t objHandle;	/* the handle of the closed object */
	uint16_t symbolIndex;		 /* the name of the closed object */
} ObjCloseNameEvent;

typedef struct
{
	uint8_t type;
	uint8_t arg1;
	uint8_t arg2;
	uint8_t arg3;
} ObjClosePropEvent;

typedef struct
{
	uint8_t type;
	uint8_t unused1;
	uint8_t unused2;
	uint8_t dts;
} TaskInstanceStatusEvent;

typedef struct
{
	uint8_t type;
	uint8_t dts;
	uint16_t payload;		 /* the name of the user event */
} UserEvent;

typedef struct
{
	uint8_t type;

	/* 8 bits extra for storing DTS, if it does not fit in ordinary event
	(this one is always MSB if used) */
	uint8_t xts_8;

	/* 16 bits extra for storing DTS, if it does not fit in ordinary event. */
	uint16_t xts_16;
} XTSEvent;

typedef struct
{
	uint8_t type;

	uint8_t xps_8;
	uint16_t xps_16;
} XPSEvent;

typedef struct{
	uint8_t type;
	uint8_t dts;
	uint16_t size;
} MemEventSize;

typedef struct{
	uint8_t type;
	uint8_t addr_high;
	uint16_t addr_low;
} MemEventAddr;

/*******************************************************************************
 * The separate user event buffer structure. Can be enabled in trcConfig.h.
 ******************************************************************************/

#if (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)
typedef struct
{
	traceString name;
	traceString defaultFormat;
} ChannelFormatPair;

typedef struct
{
	uint16_t bufferID;
	uint16_t version;
	uint32_t wraparoundCounter;
	uint32_t numberOfSlots;
	uint32_t nextSlotToWrite;
	uint8_t numberOfChannels;
	uint8_t padding1;
	uint8_t padding2;
	uint8_t padding3;
	ChannelFormatPair channels[TRC_CFG_UB_CHANNELS+1];
	uint8_t channelBuffer[(TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE + 3) & 0xFFFFFFFC]; /* 1 byte per slot, with padding for 4 byte alignment */
	uint8_t dataBuffer[TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE * 4]; /* 4 bytes per slot */

} UserEventBuffer;
#endif

/*******************************************************************************
 * The main data structure, read by Tracealyzer from the RAM dump
 ******************************************************************************/

typedef struct
{
	volatile uint8_t startmarker0; /* Volatile is important, see init code. */
	volatile uint8_t startmarker1;
	volatile uint8_t startmarker2;
	volatile uint8_t startmarker3;
	volatile uint8_t startmarker4;
	volatile uint8_t startmarker5;
	volatile uint8_t startmarker6;
	volatile uint8_t startmarker7;
	volatile uint8_t startmarker8;
	volatile uint8_t startmarker9;
	volatile uint8_t startmarker10;
	volatile uint8_t startmarker11;

	/* Used to determine Kernel and Endianess */
	uint16_t version;

	/* Currently 5 */
	uint8_t minor_version;

	/* This should be 0 if lower IRQ priority values implies higher priority
	levels, such as on ARM Cortex M. If the opposite scheme is used, i.e.,
	if higher IRQ priority values means higher priority, this should be 1. */
	uint8_t irq_priority_order;

	/* sizeof(RecorderDataType) - just for control */
	uint32_t filesize;

	/* Current number of events recorded */
	uint32_t numEvents;

	/* The buffer size, in number of event records */
	uint32_t maxEvents;

	/* The event buffer index, where to write the next event */
	uint32_t nextFreeIndex;

	/* 1 if the buffer is full, 0 otherwise */
	uint32_t bufferIsFull;

	/* The frequency of the clock/timer/counter used as time base */
	uint32_t frequency;

	/* The absolute timestamp of the last stored event, in the native
	timebase, modulo frequency! */
	uint32_t absTimeLastEvent;

	/* The number of seconds in total - lasts for 136 years */
	uint32_t absTimeLastEventSecond;

	/* 1 if the recorder has been started, 0 if not yet started or stopped.
	This is a 32 bit variable due to alignment issues. */
	uint32_t recorderActive;

	/* If > 0, tells the maximum time between two traced ISRs that execute
	back-to-back. If the time between vTraceStoreISREnd and a directly
	following vTraceISRBegin is above isrTailchainingThreshold, we assume a
	return to the previous context in between the ISRs, otherwise we assume
	the have executed back-to-back and don't show any fragment of the previous
	context in between. */
	uint32_t isrTailchainingThreshold;

	/* Not used, remains for compatibility and future use */
	uint8_t notused[24];

	/* The amount of heap memory remaining at the last malloc or free event */
	uint32_t heapMemUsage;

	/* 0xF0F0F0F0 - for control only */
	int32_t debugMarker0;

	/* Set to value of TRC_CFG_USE_16BIT_OBJECT_HANDLES */
	uint32_t isUsing16bitHandles;

	/* The Object Property Table holds information about currently active
	tasks, queues, and other recorded objects. This is updated on each
	create call and includes object name and other properties. */
	ObjectPropertyTableType ObjectPropertyTable;

	/* 0xF1F1F1F1 - for control only */
	int32_t debugMarker1;

	/* The Symbol Table stores strings for User Events and is also used to
	store names of deleted objects, which still may be in the trace but no
	longer are available. */
	symbolTableType SymbolTable;

	/* For inclusion of float support, and for endian detection of floats.
	The value should be (float)1 or (uint32_t)0 */
#if (TRC_CFG_INCLUDE_FLOAT_SUPPORT == 1)
	float exampleFloatEncoding;
#else
	uint32_t exampleFloatEncoding;
#endif
	/* This is non-zero if an internal error occurred in the recorder, e.g., if
	one of the Nxxx constants was too small. The systemInfo string will then
	contain an error message that is displayed when attempting to view the
	trace file. */
	uint32_t internalErrorOccured;

	/* 0xF2F2F2F2 - for control only */
	int32_t debugMarker2;

	/* Error messages from the recorder. */
	char systemInfo[80];

	/* 0xF3F3F3F3 - for control only */
	int32_t debugMarker3;

	/* The event data, in 4-byte records */
	uint8_t eventData[ TRC_CFG_EVENT_BUFFER_SIZE * 4 ];

#if (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)
	UserEventBuffer userEventBuffer;
#endif

	/* This should always be 0 */
	uint32_t endOfSecondaryBlocks;

	uint8_t endmarker0;
	uint8_t endmarker1;
	uint8_t endmarker2;
	uint8_t endmarker3;
	uint8_t endmarker4;
	uint8_t endmarker5;
	uint8_t endmarker6;
	uint8_t endmarker7;
	uint8_t endmarker8;
	uint8_t endmarker9;
	uint8_t endmarker10;
	uint8_t endmarker11;
} RecorderDataType;

extern RecorderDataType* RecorderDataPtr;

/* Internal functions */

/* Signal an error. */
void prvTraceError(const char* msg);

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

traceHandle prvTraceGetObjectHandle(traceObjectClass objectclass);

void prvTraceFreeObjectHandle(traceObjectClass objectclass,
							traceHandle handle);

/* Private function. Use the public functions in trcKernelPort.h */
void prvTraceSetObjectName(traceObjectClass objectclass,
							traceHandle handle,
							const char* name);

/* Internal macros */

#define TRACE_PROPERTY_NAME_GET(objectclass, objecthandle) \
(const char*)(& RecorderDataPtr->ObjectPropertyTable.objbytes \
[uiIndexOfObject(objecthandle, objectclass)])

#define TRACE_PROPERTY_OBJECT_STATE(objectclass, handle) \
RecorderDataPtr->ObjectPropertyTable.objbytes[uiIndexOfObject(handle, objectclass) \
+ RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[objectclass]]

#define TRACE_PROPERTY_ACTOR_PRIORITY(objectclass, handle) \
RecorderDataPtr->ObjectPropertyTable.objbytes[uiIndexOfObject(handle, objectclass) \
+ RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[objectclass] + 1]

#define TRACE_SET_FLAG_ISEXCLUDED(flags, bitIndex) flags[(bitIndex) >> 3] |= (1 << ((bitIndex) & 7))
#define TRACE_CLEAR_FLAG_ISEXCLUDED(flags, bitIndex) flags[(bitIndex) >> 3] &= (uint8_t)(~(1 << ((bitIndex) & 7)))
#define TRACE_GET_FLAG_ISEXCLUDED(flags, bitIndex) (flags[(bitIndex) >> 3] & (1 << ((bitIndex) & 7)))

#define TRACE_SET_EVENT_CODE_FLAG_ISEXCLUDED(eventCode) TRACE_SET_FLAG_ISEXCLUDED(trcExcludedEventCodes, eventCode)
#define TRACE_CLEAR_EVENT_CODE_FLAG_ISEXCLUDED(eventCode) TRACE_CLEAR_FLAG_ISEXCLUDED(trcExcludedEventCodes, eventCode)
#define TRACE_GET_EVENT_CODE_FLAG_ISEXCLUDED(eventCode) TRACE_GET_FLAG_ISEXCLUDED(trcExcludedEventCodes, eventCode)

/* DEBUG ASSERTS */
#if defined TRC_CFG_USE_TRACE_ASSERT && TRC_CFG_USE_TRACE_ASSERT != 0
#define TRACE_ASSERT(eval, msg, defRetVal) \
if (!(eval)) \
{ \
	prvTraceError("TRACE_ASSERT: " msg); \
	return defRetVal; \
}
#else
#define TRACE_ASSERT(eval, msg, defRetVal)
#endif

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)*/

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/******************************************************************************/
/*** INTERNAL STREAMING FUNCTIONS *********************************************/
/******************************************************************************/

/* Saves a symbol name (task name etc.) in symbol table */
void prvTraceSaveSymbol(const void *address, const char *name);

/* Deletes a symbol name (task name etc.) from symbol table */
void prvTraceDeleteSymbol(void *address);

/* Saves an object data entry (task base priority) in object data table */
void prvTraceSaveObjectData(const void *address, uint32_t data);

/* Removes an object data entry (task base priority) from object data table */
void prvTraceDeleteObjectData(void *address);

/* Store an event with zero parameters (event ID only) */
void prvTraceStoreEvent0(uint16_t eventID);

/* Store an event with one 32-bit parameter (pointer address or an int) */
void prvTraceStoreEvent1(uint16_t eventID,
	uint32_t param1);

/* Store an event with two 32-bit parameters */
void prvTraceStoreEvent2(uint16_t eventID,
	uint32_t param1,
	uint32_t param2);

/* Store an event with three 32-bit parameters */
void prvTraceStoreEvent3(uint16_t eventID,
	uint32_t param1,
	uint32_t param2,
	uint32_t param3);

/* Stores an event with <nParam> 32-bit integer parameters */
void prvTraceStoreEvent(int nParam, uint16_t EventID, ...);

/* Stories an event with a string and <nParam> 32-bit integer parameters */
void prvTraceStoreStringEvent(int nArgs, uint16_t eventID, const char* str, ...);

/* Initializes the paged event buffer used by certain stream ports */
void prvPagedEventBufferInit(char* buffer);

/* Retrieve a pointer to the paged event buffer */
void* prvPagedEventBufferGetWritePointer(int sizeOfEvent);

/* Transfer a full buffer page */
int32_t prvPagedEventBufferTransfer(int32_t(*writeFunc)(void* data, uint32_t size, int32_t* ptrBytesWritten), int32_t* nofBytes);

/* Resets the paged event buffer */
void prvPagedEventBufferReset(void);

/* The data structure for commands (a bit overkill) */
typedef struct
{
	unsigned char cmdCode;
	unsigned char param1;
	unsigned char param2;
	unsigned char param3;
	unsigned char param4;
	unsigned char param5;
	unsigned char checksumLSB;
	unsigned char checksumMSB;
} TracealyzerCommandType;

/* Checks if the provided command is a valid command */
int prvIsValidCommand(TracealyzerCommandType* cmd);

/* Executed the received command (Start or Stop) */
void prvProcessCommand(TracealyzerCommandType* cmd);


#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#else /* when TRC_USE_TRACEALYZER_RECORDER == 0 */

#define vTraceEnable(x)
#define xTraceRegisterString(x) 0; (void)x;
#define vTracePrint(chn, ...) (void)chn
#define vTracePrintF(chn, ...) (void)chn
#define vTraceInstanceFinishedNow()
#define vTraceInstanceFinishedNext()
#define vTraceStoreISRBegin(x) (void)x
#define vTraceStoreISREnd(x) (void)x
#define xTraceSetISRProperties(a, b) 0
#define vTraceStoreKernelObjectName(a, b)
#define xTraceRegisterChannelFormat(eventLabel, formatStr) 0
#define vTraceChannelPrint(label)
#define vTraceUBData(label, ...)

#define prvTraceSetReadyEventsEnabled(status)

#define vTraceExcludeTask(handle)

#define uiTraceStart() (1)
#define vTraceStart()
#define vTraceStop()

#ifndef vTraceSetRecorderDataBuffer
#define vTraceSetRecorderDataBuffer(pRecorderData)
#endif

#ifndef TRC_ALLOC_CUSTOM_BUFFER
#define TRC_ALLOC_CUSTOM_BUFFER(bufname)
#endif

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#ifdef __cplusplus
}
#endif

#endif /* TRC_RECORDER_H */
