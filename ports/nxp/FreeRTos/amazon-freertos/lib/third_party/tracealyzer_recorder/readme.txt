-------------------------------------------------------------------------------
          Tracealyzer for FreeRTOS - Trace Recorder Library v3.1.1                      
-------------------------------------------------------------------------------

Tracealyzer for FreeRTOS is a sophisticated tool for tracing and visualization
of FreeRTOS-based systems. Tracealyzer gives an unprecedented insight into the
runtime behavior, which speeds up debugging, validation and optimization. 

To learn more about this, see

 - Getting Started (videos etc): http://percepio.com/gettingstarted/tz-freertos/

 - User Manual (incl. Recorder API): http://percepio.com/docs/FreeRTOS/manual

 - FAQ: http://percepio.com/category/faq/

In case you have any questions, don't hesitate to contact support@percepio.com

-------------------------------------------------------------------------------

Changes, v3.1.1 -> v3.1.2

- Fixed two bugs related to User Events, one in vTracePrintF and other in vTracePrint.

- Fixed a build problem related to a single reference of the old FreeRTOS type "xTaskHandle", in trcKernelPort.c.
  Changed to "TaskHandle_t", unless if using an older FreeRTOS kernel or the "compatibility mode".

- Removed traceCREATE_MUTEX hook for FreeRTOS v9 or later (no longer required)

- Updated the User Manual regarding snapshot trace via IAR Embedded Workbench.

- Renamed vTraceGetTraceBuffer to xTraceGetTraceBuffer, since returning a pointer.

-------------------------------------------------------------------------------

Changes, v3.1.0 -> v3.1.1

After the major changes in the v3.1.0 trace recorder library, this update 
corrects a number of minor issues. Only minor functional improvements.

- You can now use TRC_ALLOC_CUSTOM_BUFFER to declare a trace buffer on a custom
  location (using linker directives). 
  The related function vTraceSetRecorderDataBuffer has been promoted to the
  Common API (previously only supported in snapshot mode, but custom allocation
  is now generally supported also in streaming mode).
  
- Removed TRC_CFG_USE_LINKER_PRAGMA. No longer necessary thanks to the custom
  allocation mode.
  
- Added support for timestamping from custom periodic timers, required for
  accurate timestamping on Cortex-M0/M0+ devices when using tickless idle.
  Only for streaming mode so far. See TRC_CUSTOM_TIMER_INCR / DECR.

- ARM Cortex-M port: Made sure the DWT unit is initialized properly, in case
  the debugger doesn't handle this.

- ARM Cortex-M port: Added possibility to use Systick timestamping also on
  Cortex-M3/M4/M7 devices (that otherwise use DWT timestamping by default).
  To use this option, define the macro TRC_CFG_ARM_CM_USE_SYSTICK.

- J-Link streaming: The default RTT buffer has been changed from 0 to 1.

- J-Link streaming: The RTT buffer settings for buffer 1 and higher, are now
  found in trcStreamingPort.h. Note: These settings don't apply to buffer 0.

- vTracePrint has been optimized for better performance in string logging.

- Minor performance improvement related to symbol table transfer in streaming mode.

- Timer names now registered also in streaming mode.

- Timer start and stop event are now traced.

- Implemented support for queue registry (traceQUEUE_REGISTRY_ADD) also for streaming.

- Fixed a bug related to repeated calls of vTraceEnable.

- Fixed a bug where task-switches seemed to occur even though the scheduler was disabled.

- Renamed HARDWARE_PORT_TEXAS_INSTRUMENTS_TMS570_RM48, added prefix TRC.

- Fixed several language issues in the comments and documentation.

- Fixed several minor issues and warnings from different compilers
 (including PowerPC/gcc) and configurations.

-------------------------------------------------------------------------------
 
Changes, v3.0.9 -> v3.1.0

- Merge of previously separated snapshot and streaming recorders into a single
  recorder supporting both streaming and snapshot as different modes.
  
- New common API for supporting both streaming and snapshot modes.
  
- New integration guide, see the User Manual.

- Major improvement of API documentation in source files and User Manual.
  
- New concept of "stream ports", giving a better structure defining streaming
  interfaces, and restructured the J-Link and TCP/IP streaming as stream ports.
  
- Added a stream port for USB CDC connections, with STM32 as example.
  Since Tracealyzer now can receive serial data on Windows COM ports, this is
  really easy to use.

- Added a warning (#error) for cases where FreeRTOS tickless idle mode is used
  together with timestamping using SysTick or other periodic interrupt timers,
  Tracing with tickless idle requires an independent time source to correctly
  capture the length of the idle periods.
 
- Major changes in the recorder API. Important examples are:

  * Some configuration macros have changed names, e.g. for "hardware port".
    Make sure to remove any old "trcConfig.h" files if upgrading from an
	earlier version!

  * Recorder configuration in trcConfig.h has been minimized and now only 
    includes the important settings that are independent of recorder mode.
	Advanced settings for each mode are found in trcSnapshotConfig.h and
	trcStreamingConfig.h.
		
  * vTraceEnable replaces Trace_Init and vTraceInitTraceData, as well as
    vTraceStart and uiTraceStart.
  
  * vTraceStop now part of the common API and thereby available also in
    streaming. And since vTraceEnable can start the streaming directly
	you have the option control the tracing from target, e.g., for
	streaming to a device file system.
  
  * vTraceStoreKernelObjectName from old streaming recorder has been replaced
    by vTraceSetQueueName, vTraceSetSemaphoreName, etc.
	 
  * vTraceSetISRProperties now returns a "traceHandle" that should be passed as
    parameter to vTraceStoreISRBegin and vTraceStoreISREnd.
	
  * xTraceRegisterString has replaced the old functions xTraceOpenLabel and 
    vTraceStoreUserEventChannelName. This now returns a "traceString" for use
	as "channel" parameter in vTracePrintF, and in other places where strings
	are stored.
	
  * Removed vTraceStoreISREndManual and vTraceStoreISREndAuto, use
    vTraceStoreISREnd instead.
  
  * Renamed the functions for saving User Events in a separate buffer:
     - xTraceRegisterChannelFormat	->	xTraceRegisterUBChannel
     - vTraceChannelPrintF 		->	vTraceUBData
     - vTraceChannelUserEvent 		->	vTraceUBEvent
  
 
-------------------------------------------------------------------------------
Copyright Percepio AB, 2017. 