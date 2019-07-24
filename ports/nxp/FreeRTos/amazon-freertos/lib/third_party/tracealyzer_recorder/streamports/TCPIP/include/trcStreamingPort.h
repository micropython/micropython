/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcStreamingPort.h
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use TCP/IP as streaming channel.
 * The example is for lwIP.
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
 * TRC_RECORDER_TRANSFER_METHOD_TCPIP
 * 
 * This stream port for TCP/IP uses a temporary buffer consisting of multiple 
 * pages, that are transmitted periodically by the TzCtrl task. You can modify 
 * the supporting functions to match your system. See trcStreamingPort.c
 ******************************************************************************/

int32_t trcTcpWrite(void* data, uint32_t size, int32_t *ptrBytesWritten);
int32_t trcTcpRead(void* data, uint32_t size, int32_t *ptrBytesRead);

#if TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_STATIC
#define TRC_STREAM_PORT_ALLOCATE_FIELDS() static char _TzTraceData[TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT * TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE];       /* Static allocation. */
#define TRC_STREAM_PORT_MALLOC() /* Static allocation. Not used. */
#else
#define TRC_STREAM_PORT_ALLOCATE_FIELDS() static char* _TzTraceData = NULL;     /* Dynamic allocation. */
#define TRC_STREAM_PORT_MALLOC() _TzTraceData = TRC_PORT_MALLOC(TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT * TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE);
#endif

#define TRC_STREAM_PORT_INIT() \
        TRC_STREAM_PORT_MALLOC(); /*Dynamic allocation or empty if static */ \
        prvPagedEventBufferInit(_TzTraceData);

#define TRC_STREAM_PORT_ALLOCATE_EVENT(_type, _ptrData, _size) _type* _ptrData; _ptrData = (_type*)prvPagedEventBufferGetWritePointer(_size);
#define TRC_STREAM_PORT_ALLOCATE_DYNAMIC_EVENT(_type, _ptrData, _size) TRC_STREAM_PORT_ALLOCATE_EVENT(_type, _ptrData, _size) /* We do the same thing as for non-dynamic event sizes */
#define TRC_STREAM_PORT_COMMIT_EVENT(_ptrData, _size) /* Not needed since we write immediately into the buffer received above by TRC_STREAM_PORT_ALLOCATE_EVENT, and the TRC_STREAM_PORT_PERIODIC_SEND_DATA defined below will take care of the actual trace transfer. */
#define TRC_STREAM_PORT_READ_DATA(_ptrData, _size, _ptrBytesRead) trcTcpRead(_ptrData, _size, _ptrBytesRead);
#define TRC_STREAM_PORT_PERIODIC_SEND_DATA(_ptrBytesSent) prvPagedEventBufferTransfer(trcTcpWrite, _ptrBytesSent);

#define TRC_STREAM_PORT_ON_TRACE_BEGIN() prvPagedEventBufferInit(_TzTraceData);
#define TRC_STREAM_PORT_ON_TRACE_END() /* Do nothing */

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
