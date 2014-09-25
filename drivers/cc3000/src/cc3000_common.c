/*****************************************************************************
*
*  cc3000_common.c.c  - CC3000 Host Driver Implementation.
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
//*****************************************************************************
//
//! \addtogroup common_api
//! @{
//
//*****************************************************************************
/******************************************************************************
*
* Include files
*
*****************************************************************************/

#include "cc3000_common.h"
#include "socket.h"
#include "wlan.h"
#include "evnt_handler.h"

//*****************************************************************************
//
//!  __error__
//!
//!  @param  pcFilename - file name, where error occurred
//!  @param  ulLine     - line number, where error occurred
//!
//!  @return none
//!
//!  @brief stub function for ASSERT macro
//
//*****************************************************************************
void __error__(CHAR *pcFilename, UINT32 ulLine)
{
	//TODO full up function
}



//*****************************************************************************
//
//!  UINT32_TO_STREAM_f
//!
//!  @param  p       pointer to the new stream
//!  @param  u32     pointer to the 32 bit
//!
//!  @return               pointer to the new stream
//!
//!  @brief                This function is used for copying 32 bit to stream
//!						   while converting to little endian format.
//
//*****************************************************************************

UINT8* UINT32_TO_STREAM_f (UINT8 *p, UINT32 u32)
{
	*(p)++ = (UINT8)(u32);
	*(p)++ = (UINT8)((u32) >> 8);
	*(p)++ = (UINT8)((u32) >> 16);
	*(p)++ = (UINT8)((u32) >> 24);
	return p;
}

//*****************************************************************************
//
//!  UINT16_TO_STREAM_f
//!
//!  @param  p       pointer to the new stream
//!  @param  u32     pointer to the 16 bit
//!
//!  @return               pointer to the new stream
//!
//!  @brief               This function is used for copying 16 bit to stream
//!                       while converting to little endian format.
//
//*****************************************************************************

UINT8* UINT16_TO_STREAM_f (UINT8 *p, UINT16 u16)
{
	*(p)++ = (UINT8)(u16);
	*(p)++ = (UINT8)((u16) >> 8);
	return p;
}

//*****************************************************************************
//
//!  STREAM_TO_UINT16_f
//!
//!  @param  p          pointer to the stream
//!  @param  offset     offset in the stream
//!
//!  @return               pointer to the new 16 bit
//!
//!  @brief               This function is used for copying received stream to
//!                       16 bit in little endian format.
//
//*****************************************************************************

UINT16 STREAM_TO_UINT16_f(CHAR* p, UINT16 offset)
{
	return (UINT16)((UINT16)((UINT16)
		(*(p + offset + 1)) << 8) + (UINT16)(*(p + offset)));
}

//*****************************************************************************
//
//!  STREAM_TO_UINT32_f
//!
//!  @param  p          pointer to the stream
//!  @param  offset     offset in the stream
//!
//!  @return               pointer to the new 32 bit
//!
//!  @brief               This function is used for copying received stream to
//!                       32 bit in little endian format.
//
//*****************************************************************************

UINT32 STREAM_TO_UINT32_f(CHAR* p, UINT16 offset)
{
	return (UINT32)((UINT32)((UINT32)
		(*(p + offset + 3)) << 24) + (UINT32)((UINT32)
		(*(p + offset + 2)) << 16) + (UINT32)((UINT32)
		(*(p + offset + 1)) << 8) + (UINT32)(*(p + offset)));
}



//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
