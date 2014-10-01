/*****************************************************************************
*
*  evnt_handler.h  - CC3000 Host Driver Implementation.
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
#ifndef __CC3000_EVENT_HANDLER_H__
#define __CC3000_EVENT_HANDLER_H__
#include "hci.h"
#include "socket.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef  __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//!  hci_event_handler
//!
//!  @param  pRetParams     incoming data buffer
//!  @param  from           from information (in case of data received)
//!  @param  fromlen        from information length (in case of data received)
//!
//!  @return         none
//!
//!  @brief          Parse the incoming events packets and issues corresponding
//!                  event handler from global array of handlers pointers
//
//*****************************************************************************
extern UINT8 *hci_event_handler(void *pRetParams, UINT8 *from, UINT8 *fromlen);

//*****************************************************************************
//
//!  hci_unsol_event_handler
//!
//!  @param  event_hdr   event header
//!
//!  @return             1 if event supported and handled
//!                      0 if event is not supported
//!
//!  @brief              Handle unsolicited events
//
//*****************************************************************************
extern INT32 hci_unsol_event_handler(CHAR *event_hdr);

//*****************************************************************************
//
//!  hci_unsolicited_event_handler
//!
//!  @param None
//!
//!  @return         ESUCCESS if successful, EFAIL if an error occurred
//!
//!  @brief          Parse the incoming unsolicited event packets and issues 
//!                  corresponding event handler.
//
//*****************************************************************************
extern INT32 hci_unsolicited_event_handler(void);

#define M_BSD_RESP_PARAMS_OFFSET(hci_event_hdr)((CHAR *)(hci_event_hdr) + HCI_EVENT_HEADER_SIZE)

#define SOCKET_STATUS_ACTIVE       0
#define SOCKET_STATUS_INACTIVE     1
/* Init socket_active_status = 'all ones': init all sockets with SOCKET_STATUS_INACTIVE.
   Will be changed by 'set_socket_active_status' upon 'connect' and 'accept' calls */
#define SOCKET_STATUS_INIT_VAL  0xFFFF
#define M_IS_VALID_SD(sd) ((0 <= (sd)) && ((sd) <= 7))
#define M_IS_VALID_STATUS(status) (((status) == SOCKET_STATUS_ACTIVE)||((status) == SOCKET_STATUS_INACTIVE))

extern UINT32 socket_active_status;

extern void set_socket_active_status(INT32 Sd, INT32 Status);
extern INT32 get_socket_active_status(INT32 Sd);

typedef struct _bsd_accept_return_t
{
    INT32             iSocketDescriptor;
    INT32             iStatus;
    sockaddr   		tSocketAddress;
    
} tBsdReturnParams;


typedef struct _bsd_read_return_t
{
    INT32             iSocketDescriptor;
    INT32             iNumberOfBytes;
    UINT32	 uiFlags;
} tBsdReadReturnParams;

#define BSD_RECV_FROM_FROMLEN_OFFSET	(4)
#define BSD_RECV_FROM_FROM_OFFSET		(16)


typedef struct _bsd_select_return_t
{
    INT32					iStatus;
	UINT32 			uiRdfd;
	UINT32 			uiWrfd;
	UINT32 			uiExfd;
} tBsdSelectRecvParams;


typedef struct _bsd_getsockopt_return_t
{
	UINT8			ucOptValue[4];
	CHAR						iStatus;
} tBsdGetSockOptReturnParams;

typedef struct _bsd_gethostbyname_return_t
{
    INT32             retVal;
    INT32             outputAddress;
} tBsdGethostbynameParams;

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __CC3000_EVENT_HANDLER_H__

