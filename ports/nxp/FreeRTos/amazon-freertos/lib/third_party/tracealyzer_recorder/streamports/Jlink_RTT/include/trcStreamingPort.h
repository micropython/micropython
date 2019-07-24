/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcStreamingPort.h
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use SEGGER RTT as streaming channel.
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

#ifndef TRC_STREAMING_PORT_H
#define TRC_STREAMING_PORT_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Configuration Macro: TRC_CFG_RTT_BUFFER_SIZE_UP
 *
 * Defines the size of the "up" RTT buffer (target -> host) to use for writing
 * the trace data, for RTT buffer 1 or higher.
 *
 * This setting is ignored for RTT buffer 0, which can't be reconfigured
 * in runtime and therefore hard-coded to use the defines in SEGGER_RTT_Conf.h.
 *
 * Default buffer size for Tracealyzer is 5000 bytes. 
 *
 * If you have a stand-alone J-Link probe, the can be decreased to around 1 KB.
 * But integrated J-Link OB interfaces are slower and needs about 5-10 KB, 
 * depending on the amount of data produced.
 ******************************************************************************/
#define TRC_CFG_RTT_BUFFER_SIZE_UP 5000

/*******************************************************************************
 * Configuration Macro: TRC_CFG_RTT_BUFFER_SIZE_DOWN
 *
 * Defines the size of the "down" RTT buffer (host -> target) to use for reading
 * commands from Tracealyzer, for RTT buffer 1 or higher.
 *
 * Default buffer size for Tracealyzer is 32 bytes.
 *
 * This setting is ignored for RTT buffer 0, which can't be reconfigured
 * in runtime and therefore hard-coded to use the defines in SEGGER_RTT_Conf.h.
 ******************************************************************************/
#define TRC_CFG_RTT_BUFFER_SIZE_DOWN 32

/*******************************************************************************
 * Configuration Macro: TRC_CFG_RTT_UP_BUFFER_INDEX
 *
 * Defines the RTT buffer to use for writing the trace data. Make sure that
 * the PC application has the same setting (File->Settings).
 *
 * Default: 1
 *
 * We don't recommend using RTT buffer 0, since mainly intended for terminals.
 * If you prefer to use buffer 0, it must be configured in SEGGER_RTT_Conf.h. 
 ******************************************************************************/
#define TRC_CFG_RTT_UP_BUFFER_INDEX 1

/*******************************************************************************
 * Configuration Macro: TRC_CFG_RTT_DOWN_BUFFER_INDEX
 *
 * Defines the RTT buffer to use for reading the trace data. Make sure that
 * the PC application has the same setting (File->Settings).
 *
 * Default: 1
 *
 * We don't recommend using RTT buffer 0, since mainly intended for terminals.
 * If you prefer to use buffer 0, it must be configured in SEGGER_RTT_Conf.h. 
 ******************************************************************************/
#define TRC_CFG_RTT_DOWN_BUFFER_INDEX 1

/*******************************************************************************
 * TRC_CFG_RTT_MODE
 * This stream port for J-Link streaming relies on SEGGER RTT, that contains an
 * internal RAM buffer read by the J-Link probes during execution.
 *
 * Possible values:
 * - SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL (default)
 * - SEGGER_RTT_MODE_NO_BLOCK_SKIP
 *
 * We recommend using SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL, to ensure you get a
 * complete and valid trace. This may however cause blocking if your streaming
 * interface isn't fast enough, which may disturb the real-time behavior.
 * We therefore recommend to try SEGGER_RTT_MODE_NO_BLOCK_SKIP as well. 
 * In this mode, Tracealyzer will report lost events if the transfer is not
 * fast enough. In that case, try increasing the size of the "up buffer".
 ******************************************************************************/
#define TRC_CFG_RTT_MODE SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL

#include "SEGGER_RTT_Conf.h"
#include "SEGGER_RTT.h"

#if (TRC_CFG_RTT_UP_BUFFER_INDEX >= SEGGER_RTT_MAX_NUM_UP_BUFFERS)
#error "TRC_CFG_RTT_UP_BUFFER_INDEX must be smaller than SEGGER_RTT_MAX_NUM_UP_BUFFERS"
#endif

#if (TRC_CFG_RTT_DOWN_BUFFER_INDEX >= SEGGER_RTT_MAX_NUM_DOWN_BUFFERS)
#error "TRC_CFG_RTT_DOWN_BUFFER_INDEX must be smaller than SEGGER_RTT_MAX_NUM_DOWN_BUFFERS"
#endif

/* If index is defined as 0, the internal RTT buffers will be used instead of this. */
#if TRC_CFG_RTT_UP_BUFFER_INDEX == 0
#define TRC_RTT_ALLOC_UP() static char* _TzTraceData = NULL;    /* Not actually used. Ignore allocation method. */
#define TRC_STREAM_PORT_MALLOC() /* Static allocation. Not used. */
#define TRC_ALLOC_CUSTOM_BUFFER(bufname) /* Only for custom allocation */
#else
#if TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_STATIC
#define TRC_RTT_ALLOC_UP() char _TzTraceData[TRC_CFG_RTT_BUFFER_SIZE_UP];    /* Static allocation */
#define TRC_STREAM_PORT_MALLOC() /* Static allocation. Not used. */
#define TRC_ALLOC_CUSTOM_BUFFER(bufname) /* Only for custom allocation */
#endif
#if TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC
#define TRC_RTT_ALLOC_UP() char* _TzTraceData = NULL;    /* Dynamic allocation */
#define TRC_STREAM_PORT_MALLOC() _TzTraceData = TRC_PORT_MALLOC(TRC_CFG_RTT_BUFFER_SIZE_UP);
#define TRC_ALLOC_CUSTOM_BUFFER(bufname) /* Only for custom allocation */
#endif
#if TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM
#define TRC_RTT_ALLOC_UP() char* _TzTraceData = NULL;					/* Custom allocation, user needs to call vTraceSetRecorderDataBuffer before vTraceEnable, to assign this */ 
#define TRC_STREAM_PORT_MALLOC()										/* Not used in custom mode */
#define TRC_ALLOC_CUSTOM_BUFFER(bufname) char bufname [TRC_CFG_RTT_BUFFER_SIZE_UP];  /* Not static in this case, since declared in user code */
#endif
#endif

/* Down-buffer. If index is defined as 0, the internal RTT buffers will be used instead of this. */ \
#if TRC_CFG_RTT_DOWN_BUFFER_INDEX == 0
#define TRC_RTT_ALLOC_DOWN() static char* _TzCtrlData = NULL;           /* Not actually used. Ignore allocation method. */
#else
#define TRC_RTT_ALLOC_DOWN() static char _TzCtrlData[TRC_CFG_RTT_BUFFER_SIZE_DOWN]; /* Always static allocation, since usually small. */
#endif
  
#define TRC_STREAM_PORT_ALLOCATE_FIELDS() \
	TRC_RTT_ALLOC_UP() /* Macro that will result in proper UP buffer allocation */ \
	TRC_RTT_ALLOC_DOWN() /* Macro that will result in proper DOWN buffer allocation */

#define TRC_STREAM_PORT_INIT() \
	TRC_STREAM_PORT_MALLOC(); /*Dynamic allocation or empty if static */ \
	SEGGER_RTT_ConfigUpBuffer(TRC_CFG_RTT_UP_BUFFER_INDEX, "TzData", _TzTraceData, TRC_CFG_RTT_BUFFER_SIZE_UP, TRC_CFG_RTT_MODE ); \
	SEGGER_RTT_ConfigDownBuffer(TRC_CFG_RTT_DOWN_BUFFER_INDEX, "TzCtrl", _TzCtrlData, TRC_CFG_RTT_BUFFER_SIZE_DOWN, TRC_CFG_RTT_MODE);

#define TRC_STREAM_PORT_ALLOCATE_EVENT(_type, _ptrData, _size) _type _tmpArray[_size / sizeof(_type)]; _type* _ptrData = _tmpArray;
#define TRC_STREAM_PORT_ALLOCATE_DYNAMIC_EVENT(_type, _ptrData, _size) _type _tmpArray[sizeof(largestEventType) / sizeof(_type)]; _type* _ptrData = _tmpArray;
#define TRC_STREAM_PORT_COMMIT_EVENT(_ptrData, _size) SEGGER_RTT_Write(TRC_CFG_RTT_UP_BUFFER_INDEX, (const char*)_ptrData, _size);
#define TRC_STREAM_PORT_READ_DATA(_ptrData, _size, _ptrBytesRead) if (SEGGER_RTT_HASDATA(TRC_CFG_RTT_DOWN_BUFFER_INDEX)) *_ptrBytesRead = (int)SEGGER_RTT_Read(TRC_CFG_RTT_DOWN_BUFFER_INDEX, (char*)_ptrData, _size);
#define TRC_STREAM_PORT_PERIODIC_SEND_DATA(_ptrBytesSent)

#define TRC_STREAM_PORT_ON_TRACE_BEGIN() /* Do nothing */
#define TRC_STREAM_PORT_ON_TRACE_END() /* Do nothing */
    

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
