/*****************************************************************************
*
*  cc3000_common.h  - CC3000 Host Driver Implementation.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
#ifndef __CC3000_COMMON_H__
#define __CC3000_COMMON_H__

#include "data_types.h"

//******************************************************************************
// Include files
//******************************************************************************
#include <stdlib.h>
#include <stdint.h>

//*****************************************************************************
// Prefix exported names to avoid name clash
//*****************************************************************************
#define CC3000_EXPORT(name) cc3000_ ## name

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef  __cplusplus
extern "C" {
#endif

extern int CC3000_EXPORT(errno);

//*****************************************************************************
//                  ERROR CODES
//*****************************************************************************
#define ESUCCESS        0
#define EFAIL          -1
#define EERROR          EFAIL

//*****************************************************************************
//                  COMMON DEFINES
//*****************************************************************************
#define ERROR_SOCKET_INACTIVE   -57 

#define WLAN_ENABLE      (1)   
#define WLAN_DISABLE     (0)

#define	MAC_ADDR_LEN	(6)

#define	SP_PORTION_SIZE	(32)
  
/*Defines for minimal and maximal RX buffer size. This size includes the spi 
  header and hci header.
  The maximal buffer size derives from:
    MTU + HCI header + SPI header + sendto() agrs size
  The minimum buffer size derives from:
    HCI header + SPI header + max args size

  This buffer is used for receiving events and data.
  The packet can not be longer than MTU size and CC3000 does not support 
  fragmentation. Note that the same buffer is used for reception of the data 
  and events from CC3000. That is why the minimum is defined. 
  The calculation for the actual size of buffer for reception is:
  Given the maximal data size MAX_DATA that is expected to be received by
  application, the required buffer is:
  Using recv() or recvfrom():
 
    max(CC3000_MINIMAL_RX_SIZE, MAX_DATA + HEADERS_SIZE_DATA + fromlen
    + ucArgsize + 1)
 
  Using gethostbyname() with minimal buffer size will limit the host name
  returned to 99 bytes only.
  The 1 is used for the overrun detection 

  Buffer size increased to 130 following the add_profile() with WEP security
  which requires TX buffer size of 130 bytes: 
  HEADERS_SIZE_EVNT + WLAN_ADD_PROFILE_WEP_PARAM_LEN + MAX SSID LEN + 4 * MAX KEY LEN = 130
  MAX SSID LEN = 32 
  MAX SSID LEN = 13 (with add_profile only ascii key setting is supported, 
  therfore maximum key size is 13)
*/

#define CC3000_MINIMAL_RX_SIZE      (130 + 1)
#define CC3000_MAXIMAL_RX_SIZE      (1519 + 1)

/*Defines for minimal and maximal TX buffer size.
  This buffer is used for sending events and data.
  The packet can not be longer than MTU size and CC3000 does not support 
  fragmentation. Note that the same buffer is used for transmission of the data
  and commands. That is why the minimum is defined.
  The calculation for the actual size of buffer for transmission is:
  Given the maximal data size MAX_DATA, the required buffer is:
  Using Sendto():
 
   max(CC3000_MINIMAL_TX_SIZE, MAX_DATA + SPI_HEADER_SIZE
   + SOCKET_SENDTO_PARAMS_LEN + SIMPLE_LINK_HCI_DATA_HEADER_SIZE + 1)
 
  Using Send():
 
   max(CC3000_MINIMAL_TX_SIZE, MAX_DATA + SPI_HEADER_SIZE
   + HCI_CMND_SEND_ARG_LENGTH + SIMPLE_LINK_HCI_DATA_HEADER_SIZE + 1)
 
  The 1 is used for the overrun detection */ 

#define	CC3000_MINIMAL_TX_SIZE      (130 + 1)  
#define	CC3000_MAXIMAL_TX_SIZE      (1519 + 1)

//TX and RX buffer sizes, allow to receive and transmit maximum data at length 8.
#ifdef CC3000_TINY_DRIVER
#define TINY_CC3000_MAXIMAL_RX_SIZE 44
#define TINY_CC3000_MAXIMAL_TX_SIZE 59
#endif

/*In order to determine your preferred buffer size, 
  change CC3000_MAXIMAL_RX_SIZE and CC3000_MAXIMAL_TX_SIZE to a value between
  the minimal and maximal specified above. 
  Note that the buffers are allocated by SPI.
  In case you change the size of those buffers, you might need also to change
  the linker file, since for example on MSP430 FRAM devices the buffers are
  allocated in the FRAM section that is allocated manually and not by IDE.
*/
  
#ifndef CC3000_TINY_DRIVER
  
	#define CC3000_RX_BUFFER_SIZE   (CC3000_MAXIMAL_RX_SIZE)
	#define CC3000_TX_BUFFER_SIZE   (CC3000_MAXIMAL_TX_SIZE)
  
//if defined TINY DRIVER we use smaller RX and TX buffer in order to minimize RAM consumption
#else
	#define CC3000_RX_BUFFER_SIZE   (TINY_CC3000_MAXIMAL_RX_SIZE)
	#define CC3000_TX_BUFFER_SIZE   (TINY_CC3000_MAXIMAL_TX_SIZE)

#endif  

//*****************************************************************************
//                  Compound Types
//*****************************************************************************
typedef INT32 cc3000_time_t;
typedef UINT32 clock_t;
typedef INT32 suseconds_t;

typedef struct cc3000_timeval cc3000_timeval;

struct cc3000_timeval
{
    cc3000_time_t  tv_sec;                  /* seconds */
    suseconds_t    tv_usec;                 /* microseconds */
};

typedef CHAR *(*tFWPatches)(UINT32 *usLength);

typedef CHAR *(*tDriverPatches)(UINT32 *usLength);

typedef CHAR *(*tBootLoaderPatches)(UINT32 *usLength);

typedef void (*tWlanCB)(INT32 event_type, CHAR * data, UINT8 length );

typedef INT32 (*tWlanReadInteruptPin)(void);

typedef void (*tWlanInterruptEnable)(void);

typedef void (*tWlanInterruptDisable)(void);

typedef void (*tWriteWlanPin)(UINT8 val);

typedef struct
{
	UINT16	 usRxEventOpcode;
	UINT16	 usEventOrDataReceived;
	UINT8 	*pucReceivedData;
	UINT8 	*pucTxCommandBuffer;

	tFWPatches 			sFWPatches;
	tDriverPatches 		sDriverPatches;
	tBootLoaderPatches 	sBootLoaderPatches;
	tWlanCB	 			sWlanCB;
    tWlanReadInteruptPin  ReadWlanInterruptPin;
    tWlanInterruptEnable  WlanInterruptEnable;
    tWlanInterruptDisable WlanInterruptDisable;
    tWriteWlanPin         WriteWlanPin;

	INT32		 slTransmitDataError;
	UINT16	 usNumberOfFreeBuffers;
	UINT16	 usSlBufferLength;
	UINT16	 usBufferSize;
	UINT16	 usRxDataPending;

	UINT32    NumberOfSentPackets;
	UINT32    NumberOfReleasedPackets;

	UINT8	 InformHostOnTxComplete;
}sSimplLinkInformation;

extern volatile sSimplLinkInformation tSLInformation;


//*****************************************************************************
// Prototypes for the APIs.
//*****************************************************************************



//*****************************************************************************
//
//!  SimpleLinkWaitEvent
//!
//!  @param  usOpcode      command operation code
//!  @param  pRetParams    command return parameters
//!
//!  @return               none
//!
//!  @brief                Wait for event, pass it to the hci_event_handler and
//!                        update the event opcode in a global variable.
//
//*****************************************************************************

extern void SimpleLinkWaitEvent(UINT16 usOpcode, void *pRetParams);

//*****************************************************************************
//
//!  SimpleLinkWaitData
//!
//!  @param  pBuf       data buffer
//!  @param  from       from information
//!  @param  fromlen	  from information length
//!
//!  @return               none
//!
//!  @brief                Wait for data, pass it to the hci_event_handler
//! 					   and update in a global variable that there is 
//!						   data to read.
//
//*****************************************************************************

extern void SimpleLinkWaitData(UINT8 *pBuf, UINT8 *from, UINT8 *fromlen);

//*****************************************************************************
//
//!  UINT32_TO_STREAM_f
//!
//!  \param  p       pointer to the new stream
//!  \param  u32     pointer to the 32 bit
//!
//!  \return               pointer to the new stream
//!
//!  \brief                This function is used for copying 32 bit to stream
//!						   while converting to little endian format.
//
//*****************************************************************************

extern UINT8* UINT32_TO_STREAM_f (UINT8 *p, UINT32 u32);

//*****************************************************************************
//
//!  UINT16_TO_STREAM_f
//!
//!  \param  p       pointer to the new stream
//!  \param  u32     pointer to the 16 bit
//!
//!  \return               pointer to the new stream
//!
//!  \brief               This function is used for copying 16 bit to stream 
//!                       while converting to little endian format.
//
//*****************************************************************************

extern UINT8* UINT16_TO_STREAM_f (UINT8 *p, UINT16 u16);

//*****************************************************************************
//
//!  STREAM_TO_UINT16_f
//!
//!  \param  p          pointer to the stream
//!  \param  offset     offset in the stream
//!
//!  \return               pointer to the new 16 bit
//!
//!  \brief               This function is used for copying received stream to 
//!                       16 bit in little endian format.
//
//*****************************************************************************

extern UINT16 STREAM_TO_UINT16_f(CHAR* p, UINT16 offset);

//*****************************************************************************
//
//!  STREAM_TO_UINT32_f
//!
//!  \param  p          pointer to the stream
//!  \param  offset     offset in the stream
//!
//!  \return               pointer to the new 32 bit
//!
//!  \brief               This function is used for copying received stream to
//!                       32 bit in little endian format.
//
//*****************************************************************************

extern UINT32 STREAM_TO_UINT32_f(CHAR* p, UINT16 offset);


//*****************************************************************************
//                    COMMON MACROs
//*****************************************************************************


//This macro is used for copying 8 bit to stream while converting to little endian format.
#define UINT8_TO_STREAM(_p, _val)	{*(_p)++ = (_val);}
//This macro is used for copying 16 bit to stream while converting to little endian format.
#define UINT16_TO_STREAM(_p, _u16)	(UINT16_TO_STREAM_f(_p, _u16))
//This macro is used for copying 32 bit to stream while converting to little endian format.
#define UINT32_TO_STREAM(_p, _u32)	(UINT32_TO_STREAM_f(_p, _u32))
//This macro is used for copying a specified value length bits (l) to stream while converting to little endian format.
#define ARRAY_TO_STREAM(p, a, l) 	{register INT16 _i; for (_i = 0; _i < l; _i++) *(p)++ = ((UINT8 *) a)[_i];}
//This macro is used for copying received stream to 8 bit in little endian format.
#define STREAM_TO_UINT8(_p, _offset, _u8)	{_u8 = (UINT8)(*(_p + _offset));}
//This macro is used for copying received stream to 16 bit in little endian format.
#define STREAM_TO_UINT16(_p, _offset, _u16)	{_u16 = STREAM_TO_UINT16_f(_p, _offset);}
//This macro is used for copying received stream to 32 bit in little endian format.
#define STREAM_TO_UINT32(_p, _offset, _u32)	{_u32 = STREAM_TO_UINT32_f(_p, _offset);}
#define STREAM_TO_STREAM(p, a, l) 	{register INT16 _i; for (_i = 0; _i < l; _i++) *(a)++= ((UINT8 *) p)[_i];}




//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __CC3000_COMMON_H__
