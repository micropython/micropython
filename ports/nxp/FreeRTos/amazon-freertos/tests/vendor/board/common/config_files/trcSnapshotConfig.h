/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcSnapshotConfig.h
 *
 * Configuration parameters for trace recorder library in snapshot mode.
 * Read more at http://percepio.com/2016/10/05/rtos-tracing/
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

#ifndef TRC_SNAPSHOT_CONFIG_H
#define TRC_SNAPSHOT_CONFIG_H

#define TRC_SNAPSHOT_MODE_RING_BUFFER       ( 0x01 )
#define TRC_SNAPSHOT_MODE_STOP_WHEN_FULL    ( 0x02 )

/******************************************************************************
 * TRC_CFG_SNAPSHOT_MODE
 *
 * Macro which should be defined as one of:
 * - TRC_SNAPSHOT_MODE_RING_BUFFER
 * - TRC_SNAPSHOT_MODE_STOP_WHEN_FULL
 * Default is TRC_SNAPSHOT_MODE_RING_BUFFER.
 *
 * With TRC_CFG_SNAPSHOT_MODE set to TRC_SNAPSHOT_MODE_RING_BUFFER, the
 * events are stored in a ring buffer, i.e., where the oldest events are
 * overwritten when the buffer becomes full. This allows you to get the last
 * events leading up to an interesting state, e.g., an error, without having
 * to store the whole run since startup.
 *
 * When TRC_CFG_SNAPSHOT_MODE is TRC_SNAPSHOT_MODE_STOP_WHEN_FULL, the
 * recording is stopped when the buffer becomes full. This is useful for
 * recording events following a specific state, e.g., the startup sequence.
 *****************************************************************************/
#define TRC_CFG_SNAPSHOT_MODE               TRC_SNAPSHOT_MODE_RING_BUFFER

/*******************************************************************************
 * TRC_CFG_SCHEDULING_ONLY
 *
 * Macro which should be defined as an integer value.
 *
 * If this setting is enabled (= 1), only scheduling events are recorded.
 * If disabled (= 0), all events are recorded.
 *
 * For users of Tracealyzer Free Edition, that only displays scheduling events, this
 * option can be used to avoid storing other events.
 *
 * Default value is 0 (store all enabled events).
 *
 ******************************************************************************/
#define TRC_CFG_SCHEDULING_ONLY             0

/*******************************************************************************
 * TRC_CFG_EVENT_BUFFER_SIZE
 *
 * Macro which should be defined as an integer value.
 *
 * This defines the capacity of the event buffer, i.e., the number of records
 * it may store. Most events use one record (4 byte), although some events
 * require multiple 4-byte records. You should adjust this to the amount of RAM
 * available in the target system.
 *
 * Default value is 1000, which means that 4000 bytes is allocated for the
 * event buffer.
 ******************************************************************************/
#define TRC_CFG_EVENT_BUFFER_SIZE           15000

/*******************************************************************************
 * TRC_CFG_NTASK, TRC_CFG_NISR, TRC_CFG_NQUEUE, TRC_CFG_NSEMAPHORE...
 *
 * A group of macros which should be defined as integer values, zero or larger.
 *
 * These define the capacity of the Object Property Table, i.e., the maximum
 * number of objects active at any given point, within each object class (e.g.,
 * task, queue, semaphore, ...).
 *
 * If tasks or other objects are deleted in your system, this
 * setting does not limit the total amount of objects created, only the number
 * of objects that have been successfully created but not yet deleted.
 *
 * Using too small values will cause vTraceError to be called, which stores an
 * error message in the trace that is shown when opening the trace file. The
 * error message can also be retrieved using xTraceGetLastError.
 *
 * It can be wise to start with large values for these constants,
 * unless you are very confident on these numbers. Then do a recording and
 * check the actual usage by selecting View menu -> Trace Details ->
 * Resource Usage -> Object Table.
 ******************************************************************************/
#define TRC_CFG_NTASK                       150
#define TRC_CFG_NISR                        90
#define TRC_CFG_NQUEUE                      90
#define TRC_CFG_NSEMAPHORE                  90
#define TRC_CFG_NMUTEX                      90
#define TRC_CFG_NTIMER                      250
#define TRC_CFG_NEVENTGROUP                 90

/******************************************************************************
 * TRC_CFG_INCLUDE_MEMMANG_EVENTS
 *
 * Macro which should be defined as either zero (0) or one (1).
 *
 * This controls if malloc and free calls should be traced. Set this to zero (0)
 * to exclude malloc/free calls, or one (1) to include such events in the trace.
 *
 * Default value is 1.
 *****************************************************************************/
#define TRC_CFG_INCLUDE_MEMMANG_EVENTS      1

/******************************************************************************
 * TRC_CFG_INCLUDE_USER_EVENTS
 *
 * Macro which should be defined as either zero (0) or one (1).
 *
 * If this is zero (0) the code for creating User Events is excluded to
 * reduce code size. User Events are application-generated events, like
 * "printf" but for the trace log and the formatting is done offline, by the
 * Tracealyzer visualization tool. User Events are much faster than a printf
 * and can therefore be used in timing critical code.
 *
 * Default value is 1.
 *****************************************************************************/
#define TRC_CFG_INCLUDE_USER_EVENTS         1

/*****************************************************************************
* TRC_CFG_INCLUDE_ISR_TRACING
*
* Macro which should be defined as either zero (0) or one (1).
*
* If this is zero (0), the code for recording Interrupt Service Routines is
* excluded, in order to reduce code size.
*
* Default value is 1.
*
* Note: tracing ISRs requires that you insert calls to vTraceStoreISRBegin
* and vTraceStoreISREnd in your interrupt handlers.
*****************************************************************************/
#define TRC_CFG_INCLUDE_ISR_TRACING         1

/*****************************************************************************
* TRC_CFG_INCLUDE_READY_EVENTS
*
* Macro which should be defined as either zero (0) or one (1).
*
* If one (1), events are recorded when tasks enter scheduling state "ready".
* This allows Tracealyzer to show the initial pending time before tasks enter
* the execution state, and present accurate response times.
* If zero (0), "ready events" are not created, which allows for recording
* longer traces in the same amount of RAM.
*
* Default value is 1.
*****************************************************************************/
#define TRC_CFG_INCLUDE_READY_EVENTS        1

/*****************************************************************************
* TRC_CFG_INCLUDE_OSTICK_EVENTS
*
* Macro which should be defined as either zero (0) or one (1).
*
* If this is one (1), events will be generated whenever the OS clock is
* increased. If zero (0), OS tick events are not generated, which allows for
* recording longer traces in the same amount of RAM.
*
* Default value is 0.
*****************************************************************************/
#define TRC_CFG_INCLUDE_OSTICK_EVENTS       1

/******************************************************************************
 * TRC_CFG_INCLUDE_FLOAT_SUPPORT
 *
 * Macro which should be defined as either zero (0) or one (1).
 *
 * If this is zero (0), the support for logging floating point values in
 * vTracePrintF is stripped out, in case floating point values are not used or
 * supported by the platform used.
 *
 * Floating point values are only used in vTracePrintF and its subroutines, to
 * allow for storing float (%f) or double (%lf) arguments.
 *
 * vTracePrintF can be used with integer and string arguments in either case.
 *
 * Default value is 0.
 *****************************************************************************/
#define TRC_CFG_INCLUDE_FLOAT_SUPPORT       0

/******************************************************************************
 * TRC_CFG_INCLUDE_OBJECT_DELETE
 *
 * Macro which should be defined as either zero (0) or one (1).
 *
 * This must be enabled (1) if tasks, queues or other
 * traced kernel objects are deleted at runtime. If no deletes are made, this
 * can be set to 0 in order to exclude the delete-handling code.
 *
 * Default value is 1.
 *****************************************************************************/
#define TRC_CFG_INCLUDE_OBJECT_DELETE       1

/*******************************************************************************
 * TRC_CFG_SYMBOL_TABLE_SIZE
 *
 * Macro which should be defined as an integer value.
 *
 * This defines the capacity of the symbol table, in bytes. This symbol table
 * stores User Events labels and names of deleted tasks, queues, or other kernel
 * objects. If you don't use User Events or delete any kernel
 * objects you set this to a very low value. The minimum recommended value is 4.
 * A size of zero (0) is not allowed since a zero-sized array may result in a
 * 32-bit pointer, i.e., using 4 bytes rather than 0.
 *
 * Default value is 800.
 ******************************************************************************/
#define TRC_CFG_SYMBOL_TABLE_SIZE           5000

#if ( TRC_CFG_SYMBOL_TABLE_SIZE == 0 )
    #error "TRC_CFG_SYMBOL_TABLE_SIZE may not be zero!"
#endif

/******************************************************************************
 * TRC_CFG_NAME_LEN_TASK, TRC_CFG_NAME_LEN_QUEUE, ...
 *
 * Macros that specify the maximum lengths (number of characters) for names of
 * kernel objects, such as tasks and queues. If longer names are used, they will
 * be truncated when stored in the recorder.
 *****************************************************************************/
#define TRC_CFG_NAME_LEN_TASK          15
#define TRC_CFG_NAME_LEN_ISR           15
#define TRC_CFG_NAME_LEN_QUEUE         15
#define TRC_CFG_NAME_LEN_SEMAPHORE     15
#define TRC_CFG_NAME_LEN_MUTEX         15
#define TRC_CFG_NAME_LEN_TIMER         15
#define TRC_CFG_NAME_LEN_EVENTGROUP    15

/******************************************************************************
 *** ADVANCED SETTINGS ********************************************************
 ******************************************************************************
 * The remaining settings are not necessary to modify but allows for optimizing
 * the recorder setup for your specific needs, e.g., to exclude events that you
 * are not interested in, in order to get longer traces.
 *****************************************************************************/

/******************************************************************************
* TRC_CFG_HEAP_SIZE_BELOW_16M
*
* An integer constant that can be used to reduce the buffer usage of memory
* allocation events (malloc/free). This value should be 1 if the heap size is
* below 16 MB (2^24 byte), and you can live with reported addresses showing the
* lower 24 bits only. If 0, you get the full 32-bit addresses.
*
* Default value is 0.
******************************************************************************/
#define TRC_CFG_HEAP_SIZE_BELOW_16M                0

/******************************************************************************
 * TRC_CFG_USE_IMPLICIT_IFE_RULES
 *
 * Macro which should be defined as either zero (0) or one (1).
 * Default is 1.
 *
 * Tracealyzer groups the events into "instances" based on Instance Finish
 * Events (IFEs), produced either by default rules or calls to the recorder
 * functions vTraceInstanceFinishedNow and vTraceInstanceFinishedNext.
 *
 * If TRC_CFG_USE_IMPLICIT_IFE_RULES is one (1), the default IFE rules is
 * used, resulting in a "typical" grouping of events into instances.
 * If these rules don't give appropriate instances in your case, you can
 * override the default rules using vTraceInstanceFinishedNow/Next for one
 * or several tasks. The default IFE rules are then disabled for those tasks.
 *
 * If TRC_CFG_USE_IMPLICIT_IFE_RULES is zero (0), the implicit IFE rules are
 * disabled globally. You must then call vTraceInstanceFinishedNow or
 * vTraceInstanceFinishedNext to manually group the events into instances,
 * otherwise the tasks will appear a single long instance.
 *
 * The default IFE rules count the following events as "instance finished":
 * - Task delay, delay until
 * - Task suspend
 * - Blocking on "input" operations, i.e., when the task is waiting for the
 *   next a message/signal/event. But only if this event is blocking.
 *
 * For details, see trcSnapshotKernelPort.h and look for references to the
 * macro trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED.
 *****************************************************************************/
#define TRC_CFG_USE_IMPLICIT_IFE_RULES             1

/******************************************************************************
 * TRC_CFG_USE_16BIT_OBJECT_HANDLES
 *
 * Macro which should be defined as either zero (0) or one (1).
 *
 * If set to 0 (zero), the recorder uses 8-bit handles to identify kernel
 * objects such as tasks and queues. This limits the supported number of
 * concurrently active objects to 255 of each type (tasks, queues, mutexes,
 * etc.) Note: 255, not 256, since handle 0 is reserved.
 *
 * If set to 1 (one), the recorder uses 16-bit handles to identify kernel
 * objects such as tasks and queues. This limits the supported number of
 * concurrent objects to 65535 of each type (object class). However, since the
 * object property table is limited to 64 KB, the practical limit is about
 * 3000 objects in total.
 *
 * Default is 0 (8-bit handles)
 *
 * NOTE: An object with handle above 255 will use an extra 4-byte record in
 * the event buffer whenever the object is referenced. Moreover, some internal
 * tables in the recorder gets slightly larger when using 16-bit handles.
 *****************************************************************************/
#define TRC_CFG_USE_16BIT_OBJECT_HANDLES           0

/******************************************************************************
 * TRC_CFG_USE_TRACE_ASSERT
 *
 * Macro which should be defined as either zero (0) or one (1).
 * Default is 1.
 *
 * If this is one (1), the TRACE_ASSERT macro (used at various locations in the
 * trace recorder) will verify that a relevant condition is true.
 * If the condition is false, prvTraceError() will be called, which stops the
 * recording and stores an error message that is displayed when opening the
 * trace in Tracealyzer.
 *
 * This is used on several places in the recorder code for sanity checks on
 * parameters. Can be switched off to reduce the footprint of the tracing, but
 * we recommend to have it enabled initially.
 *****************************************************************************/
#define TRC_CFG_USE_TRACE_ASSERT                   1

/*******************************************************************************
 * TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER
 *
 * Macro which should be defined as an integer value.
 *
 * Set TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER to 1 to enable the
 * separate user event buffer (UB).
 * In this mode, user events are stored separately from other events,
 * e.g., RTOS events. Thereby you can get a much longer history of
 * user events as they don't need to share the buffer space with more
 * frequent events.
 *
 * The UB is typically used with the snapshot ring-buffer mode, so the
 * recording can continue when the main buffer gets full. And since the
 * main buffer then overwrites the earliest events, Tracealyzer displays
 * "Unknown Actor" instead of task scheduling for periods with UB data only.
 *
 * In UB mode, user events are structured as UB channels, which contains
 * a channel name and a default format string. Register a UB channel using
 * xTraceRegisterUBChannel.
 *
 * Events and data arguments are written using vTraceUBEvent and
 * vTraceUBData. They are designed to provide efficient logging of
 * repeating events, using the same format string within each channel.
 *
 * Examples:
 *
 *  traceString chn1 = xTraceRegisterString("Channel 1");
 *  traceString fmt1 = xTraceRegisterString("Event!");
 *  traceUBChannel UBCh1 = xTraceRegisterUBChannel(chn1, fmt1);
 *
 *  traceString chn2 = xTraceRegisterString("Channel 2");
 *  traceString fmt2 = xTraceRegisterString("X: %d, Y: %d");
 *	traceUBChannel UBCh2 = xTraceRegisterUBChannel(chn2, fmt2);
 *
 *  // Result in "[Channel 1] Event!"
 *	vTraceUBEvent(UBCh1);
 *
 *  // Result in "[Channel 2] X: 23, Y: 19"
 *	vTraceUBData(UBCh2, 23, 19);
 *
 * You can also use the other user event functions, like vTracePrintF.
 * as they are then rerouted to the UB instead of the main event buffer.
 * vTracePrintF then looks up the correct UB channel based on the
 * provided channel name and format string, or creates a new UB channel
 * if no match is found. The format string should therefore not contain
 * "random" messages but mainly format specifiers. Random strings should
 * be stored using %s and with the string as an argument.
 *
 *  // Creates a new UB channel ("Channel 2", "%Z: %d")
 *  vTracePrintF(chn2, "%Z: %d", value1);
 *
 *  // Finds the existing UB channel
 *  vTracePrintF(chn2, "%Z: %d", value2);
 *
 ******************************************************************************/
#define TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER     0

/*******************************************************************************
 * TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE
 *
 * Macro which should be defined as an integer value.
 *
 * This defines the capacity of the user event buffer (UB), in number of slots.
 * A single user event can use multiple slots, depending on the arguments.
 *
 * Only applicable if TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER is 1.
 ******************************************************************************/
#define TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE    200

/*******************************************************************************
 * TRC_CFG_UB_CHANNELS
 *
 * Macro which should be defined as an integer value.
 *
 * This defines the number of User Event Buffer Channels (UB channels).
 * These are used to structure the events when using the separate user
 * event buffer, and contains both a User Event Channel (the name) and
 * a default format string for the channel.
 *
 * Only applicable if TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER is 1.
 ******************************************************************************/
#define TRC_CFG_UB_CHANNELS                        32

/*******************************************************************************
 * TRC_CFG_ISR_TAILCHAINING_THRESHOLD
 *
 * Macro which should be defined as an integer value.
 *
 * If tracing multiple ISRs, this setting allows for accurate display of the
 * context-switching also in cases when the ISRs execute in direct sequence.
 *
 * vTraceStoreISREnd normally assumes that the ISR returns to the previous
 * context, i.e., a task or a preempted ISR. But if another traced ISR
 * executes in direct sequence, Tracealyzer may incorrectly display a minimal
 * fragment of the previous context in between the ISRs.
 *
 * By using TRC_CFG_ISR_TAILCHAINING_THRESHOLD you can avoid this. This is
 * however a threshold value that must be measured for your specific setup.
 * See http://percepio.com/2014/03/21/isr_tailchaining_threshold/
 *
 * The default setting is 0, meaning "disabled" and that you may get an
 * extra fragments of the previous context in between tail-chained ISRs.
 *
 * Note: This setting has separate definitions in trcSnapshotConfig.h and
 * trcStreamingConfig.h, since it is affected by the recorder mode.
 ******************************************************************************/
#define TRC_CFG_ISR_TAILCHAINING_THRESHOLD         0

#endif /*TRC_SNAPSHOT_CONFIG_H*/
