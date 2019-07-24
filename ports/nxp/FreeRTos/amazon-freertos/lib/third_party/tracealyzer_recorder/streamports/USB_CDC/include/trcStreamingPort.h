/*******************************************************************************
 * Trace Recorder Library for Tracealyzer v3.1.2
 * Percepio AB, www.percepio.com
 *
 * trcStreamingPort.h
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use USB CDC as streaming channel.
 * The example is for STM32 using STM32Cube.
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
 * Implement the below macros to define your own stream port. If your transfer 
 * method uses RTOS functions, you should not send the data directly but use 
 * the recorder's internal buffer to store the trace data, for later transfer by
 * the TzCtrl task. Check the predefined stream ports for examples on how to use 
 * the internal buffer (e.g., TCP/IP, UART or USB CDC).
 *
 * Read more at http://percepio.com/2016/10/05/rtos-tracing/  
 ******************************************************************************/

/* Include files as needed, in this case it is files from STM32Cube FW_F7 V1.4.1 */
#include "usb_device.h"
#include "usbd_cdc.h"
#include "usbd_CDC_if.h"
#include "usb_device.h"

/* Tested on STM32 devices using Keil/CMSIS USB stack */

extern USBD_CDC_ItfTypeDef  USBD_Interface_fops_FS;
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

int32_t trcCDCReceive(void *data, uint32_t size, int32_t* NumBytes);
int32_t trcCDCTransmit(void* data, uint32_t size, int32_t * noOfBytesSent );

#if TRC_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_STATIC
#define TRC_STREAM_PORT_ALLOCATE_FIELDS() static char _TzTraceData[TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT * TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE];       /* Static allocation. */
#define TRC_STREAM_PORT_MALLOC() /* Static allocation. Not used. */
#else
#define TRC_STREAM_PORT_ALLOCATE_FIELDS() static char* _TzTraceData = NULL;     /* Dynamic allocation. */
#define TRC_STREAM_PORT_MALLOC() _TzTraceData = TRC_PORT_MALLOC(TRC_PAGED_EVENT_BUFFER_PAGE_COUNT * TRC_PAGED_EVENT_BUFFER_PAGE_SIZE);
#endif

#define TRC_STREAM_PORT_INIT() \
        MX_USB_DEVICE_Init(); \
        TRC_STREAM_PORT_MALLOC(); /*Dynamic allocation or empty if static */

#define TRC_STREAM_PORT_ALLOCATE_EVENT(_type, _ptrData, _size) _type* _ptrData; _ptrData = (_type*)prvPagedEventBufferGetWritePointer(_size);

#define TRC_STREAM_PORT_ALLOCATE_DYNAMIC_EVENT(_type, _ptrData, _size) TRC_STREAM_PORT_ALLOCATE_EVENT(_type, _ptrData, _size) /* We do the same thing as for non-dynamic event sizes */
#define TRC_STREAM_PORT_COMMIT_EVENT(_ptrData, _size) /* Not needed since we write immediately into the buffer received above by TRC_STREAM_PORT_ALLOCATE_EVENT, and the TRC_STREAM_PORT_PERIODIC_SEND_DATA defined below will take care of the actual trace transfer. */
#define TRC_STREAM_PORT_READ_DATA(_ptrData, _size, _ptrBytesRead) trcCDCReceive(_ptrData, _size, _ptrBytesRead);
#define TRC_STREAM_PORT_PERIODIC_SEND_DATA(_ptrBytesSent) prvPagedEventBufferTransfer(trcCDCTransmit, _ptrBytesSent);

#define TRC_STREAM_PORT_ON_TRACE_BEGIN() { prvPagedEventBufferInit(_TzTraceData); }
#define TRC_STREAM_PORT_ON_TRACE_END() /* Do nothing */

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
