/*
 * netapp.c - CC31xx/CC32xx Host Driver Implementation
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
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
*/
    


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "simplelink.h"
#include "protocol.h"
#include "driver.h"

/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/
#define NETAPP_MDNS_OPTIONS_ADD_SERVICE_BIT					 ((_u32)0x1 << 31)

#ifdef SL_TINY
#define NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH         63
#else
#define NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH         255
#endif


/*****************************************************************************/
/* Functions prototypes                                                      */
/*****************************************************************************/
void _sl_HandleAsync_DnsGetHostByName(void *pVoidBuf);

#ifndef SL_TINY_EXT
void _sl_HandleAsync_DnsGetHostByService(void *pVoidBuf);
void _sl_HandleAsync_PingResponse(void *pVoidBuf);
#endif

void CopyPingResultsToReport(_PingReportResponse_t *pResults,SlPingReport_t *pReport);
_i16 sl_NetAppMDNSRegisterUnregisterService(const _i8* 		pServiceName, 
											const _u8   ServiceNameLen,
											const _i8* 		pText,
											const _u8   TextLen,
											const _u16  Port,
											const _u32    TTL,
											const _u32    Options);

#if defined(sl_HttpServerCallback) || defined(EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS)
_u16 _sl_NetAppSendTokenValue(slHttpServerData_t * Token);
#endif
typedef union
{
	_NetAppStartStopCommand_t       Cmd;
	_NetAppStartStopResponse_t   Rsp;
}_SlNetAppStartStopMsg_u;


#if _SL_INCLUDE_FUNC(sl_NetAppStart)

const _SlCmdCtrl_t _SlNetAppStartCtrl =
{
    SL_OPCODE_NETAPP_START_COMMAND,
    sizeof(_NetAppStartStopCommand_t),
    sizeof(_NetAppStartStopResponse_t)
};

_i16 sl_NetAppStart(const _u32 AppBitMap)
{
    _SlNetAppStartStopMsg_u Msg;
    Msg.Cmd.appId = AppBitMap;
    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlNetAppStartCtrl, &Msg, NULL));

    return Msg.Rsp.status;
}
#endif

/*****************************************************************************
 sl_NetAppStop
*****************************************************************************/
#if _SL_INCLUDE_FUNC(sl_NetAppStop)


const _SlCmdCtrl_t _SlNetAppStopCtrl =
{
    SL_OPCODE_NETAPP_STOP_COMMAND,
    sizeof(_NetAppStartStopCommand_t),
    sizeof(_NetAppStartStopResponse_t)
};



_i16 sl_NetAppStop(const _u32 AppBitMap)
{
    _SlNetAppStartStopMsg_u Msg;
    Msg.Cmd.appId = AppBitMap;
    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlNetAppStopCtrl, &Msg, NULL));

    return Msg.Rsp.status;
}
#endif


/******************************************************************************/
/* sl_NetAppGetServiceList */
/******************************************************************************/
typedef struct
{
    _u8  IndexOffest;
    _u8  MaxServiceCount;
    _u8  Flags;
    _i8  Padding;
}NetappGetServiceListCMD_t;

typedef union
{
	 NetappGetServiceListCMD_t      Cmd;
	_BasicResponse_t                Rsp;
}_SlNetappGetServiceListMsg_u;


#if _SL_INCLUDE_FUNC(sl_NetAppGetServiceList)

const _SlCmdCtrl_t _SlGetServiceListeCtrl =
{
    SL_OPCODE_NETAPP_NETAPP_MDNS_LOOKUP_SERVICE,
    sizeof(NetappGetServiceListCMD_t),
    sizeof(_BasicResponse_t)
};

_i16 sl_NetAppGetServiceList(const _u8  IndexOffest,
						     const _u8  MaxServiceCount,
							 const _u8  Flags,
						           _i8  *pBuffer,
							 const _u32  RxBufferLength
							)
{

    _i32 					 retVal= 0;
    _SlNetappGetServiceListMsg_u Msg;
    _SlCmdExt_t                  CmdExt;
	_u16               ServiceSize = 0;
	_u16               BufferSize = 0;

	/*
	Calculate RX pBuffer size
    WARNING:
    if this size is BufferSize than 1480 error should be returned because there
    is no place in the RX packet.
    */
    switch(Flags)
    {
        case SL_NET_APP_FULL_SERVICE_WITH_TEXT_IPV4_TYPE:
            ServiceSize =  sizeof(SlNetAppGetFullServiceWithTextIpv4List_t);
            break;

        case SL_NET_APP_FULL_SERVICE_IPV4_TYPE:
            ServiceSize =  sizeof(SlNetAppGetFullServiceIpv4List_t);
            break;

        case SL_NET_APP_SHORT_SERVICE_IPV4_TYPE:
            ServiceSize =  sizeof(SlNetAppGetShortServiceIpv4List_t);
            break;

        default:
			ServiceSize =  sizeof(_BasicResponse_t);
			break;
    }



	BufferSize =  MaxServiceCount * ServiceSize;

	/*Check the size of the requested services is smaller than size of the user buffer.
	  If not an error is returned in order to avoid overwriting memory. */
	if(RxBufferLength <= BufferSize)
	{
		return SL_ERROR_NETAPP_RX_BUFFER_LENGTH_ERROR;
	}

    _SlDrvResetCmdExt(&CmdExt);
    CmdExt.RxPayloadLen = BufferSize;
    CmdExt.pRxPayload = (_u8 *)pBuffer; 

    Msg.Cmd.IndexOffest		= IndexOffest;
    Msg.Cmd.MaxServiceCount = MaxServiceCount;
    Msg.Cmd.Flags			= Flags;
    Msg.Cmd.Padding			= 0;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlGetServiceListeCtrl, &Msg, &CmdExt));
    retVal = Msg.Rsp.status;

    return (_i16)retVal;
}

#endif

/*****************************************************************************/
/* sl_mDNSRegisterService */
/*****************************************************************************/
/*
 * The below struct depicts the constant parameters of the command/API RegisterService.
 *
   1. ServiceLen                      - The length of the service should be smaller than NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH.
   2. TextLen                         - The length of the text should be smaller than NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH.
   3. port                            - The port on this target host.
   4. TTL                             - The TTL of the service
   5. Options                         - bitwise parameters:
                                        bit 0  - is unique (means if the service needs to be unique)
										bit 31  - for internal use if the service should be added or deleted (set means ADD).
                                        bit 1-30 for future.

   NOTE:

   1. There are another variable parameter is this API which is the service name and the text.
   2. According to now there is no warning and Async event to user on if the service is a unique.
*
 */


typedef struct
{
    _u8   ServiceNameLen;
    _u8   TextLen;
    _u16  Port;
    _u32   TTL;
    _u32   Options;
}NetappMdnsSetService_t;

typedef union
{
	 NetappMdnsSetService_t         Cmd;
	_BasicResponse_t                Rsp;
}_SlNetappMdnsRegisterServiceMsg_u;


#if _SL_INCLUDE_FUNC(sl_NetAppMDNSRegisterUnregisterService)

const _SlCmdCtrl_t _SlRegisterServiceCtrl =
{
    SL_OPCODE_NETAPP_MDNSREGISTERSERVICE,
    sizeof(NetappMdnsSetService_t),
    sizeof(_BasicResponse_t)
};

/******************************************************************************

    sl_NetAppMDNSRegisterService

    CALLER          user from its host


    DESCRIPTION:
                    Add/delete  service
					The function manipulates the command that register the service and call
					to the NWP in order to add/delete the service to/from the mDNS package and to/from the DB.
                    
					This register service is a service offered by the application.
					This unregister service is a service offered by the application before.
                     
					The service name should be full service name according to RFC
                    of the DNS-SD - means the value in name field in SRV answer.
                    
					Example for service name:
                    1. PC1._ipp._tcp.local
                    2. PC2_server._ftp._tcp.local

                    If the option is_unique is set, mDNS probes the service name to make sure
                    it is unique before starting to announce the service on the network.
                    Instance is the instance portion of the service name.

 


    PARAMETERS:

                    The command is from constant parameters and variables parameters.

					Constant parameters are:

                    ServiceLen                          - The length of the service.
                    TextLen                             - The length of the service should be smaller than 64.
                    port                                - The port on this target host.
                    TTL                                 - The TTL of the service
                    Options                             - bitwise parameters:
                                                            bit 0  - is unique (means if the service needs to be unique)
                                                            bit 31  - for internal use if the service should be added or deleted (set means ADD).
                                                            bit 1-30 for future.

                   The variables parameters are:

                    Service name(full service name)     - The service name.
                                                          Example for service name:
                                                          1. PC1._ipp._tcp.local
                                                          2. PC2_server._ftp._tcp.local

                    Text                                - The description of the service.
                                                          should be as mentioned in the RFC
                                                          (according to type of the service IPP,FTP...)

					NOTE - pay attention

						1. Temporary -  there is an allocation on stack of internal buffer.
						Its size is NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH.
						It means that the sum of the text length and service name length cannot be bigger than
						NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH.
						If it is - An error is returned.

					    2. According to now from certain constraints the variables parameters are set in the
					    attribute part (contain constant parameters)



	  RETURNS:        Status - the immediate response of the command status.
							   0 means success. 



******************************************************************************/
_i16 sl_NetAppMDNSRegisterUnregisterService(	const _i8* 		pServiceName, 
											const _u8   ServiceNameLen,
											const _i8* 		pText,
											const _u8   TextLen,
											const _u16  Port,
											const _u32   TTL,
											const _u32   Options)

{
    _SlNetappMdnsRegisterServiceMsg_u			Msg;
    _SlCmdExt_t									CmdExt ;
 _i8 									ServiceNameAndTextBuffer[NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH];
 _i8 									*TextPtr;

	/*

	NOTE - pay attention

		1. Temporary -  there is an allocation on stack of internal buffer.
		Its size is NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH.
		It means that the sum of the text length and service name length cannot be bigger than
		NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH.
		If it is - An error is returned.

		2. According to now from certain constraints the variables parameters are set in the
		attribute part (contain constant parameters)


	*/

	/*build the attribute part of the command.
	  It contains the constant parameters of the command*/

	Msg.Cmd.ServiceNameLen	= ServiceNameLen;
	Msg.Cmd.Options			= Options;
	Msg.Cmd.Port			= Port;
	Msg.Cmd.TextLen			= TextLen;
	Msg.Cmd.TTL				= TTL;

	/*Build the payload part of the command
	 Copy the service name and text to one buffer.
	 NOTE - pay attention
	 			The size of the service length + the text length should be smaller than 255,
	 			Until the simplelink drive supports to variable length through SPI command. */
	if(TextLen + ServiceNameLen > (NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH - 1 )) /*-1 is for giving a place to set null termination at the end of the text*/
	{
		return -1;
	}

    _SlDrvMemZero(ServiceNameAndTextBuffer, NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH);

	
	/*Copy the service name*/
	sl_Memcpy(ServiceNameAndTextBuffer,
		      pServiceName,   
			  ServiceNameLen);

	if(TextLen > 0 )
	{
		
		TextPtr = &ServiceNameAndTextBuffer[ServiceNameLen];
		/*Copy the text just after the service name*/
		sl_Memcpy(TextPtr,
				  pText,   
				  TextLen);

  
	}

    _SlDrvResetCmdExt(&CmdExt);
    CmdExt.TxPayloadLen = (TextLen + ServiceNameLen);
    CmdExt.pTxPayload   = (_u8 *)ServiceNameAndTextBuffer;

	
	VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlRegisterServiceCtrl, &Msg, &CmdExt));

	return (_i16)Msg.Rsp.status;

	
}
#endif

/**********************************************************************************************/
#if _SL_INCLUDE_FUNC(sl_NetAppMDNSRegisterService)

_i16 sl_NetAppMDNSRegisterService(	const _i8* 		pServiceName, 
									const _u8   ServiceNameLen,
									const _i8* 		pText,
									const _u8   TextLen,
									const _u16  Port,
									const _u32    TTL,
									     _u32    Options)

{

	/*

	NOTE - pay attention

	1. Temporary -  there is an allocation on stack of internal buffer.
	Its size is NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH.
	It means that the sum of the text length and service name length cannot be bigger than
	NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH.
	If it is - An error is returned.

	2. According to now from certain constraints the variables parameters are set in the
	attribute part (contain constant parameters)

	*/

	/*Set the add service bit in the options parameter.
	  In order not use different opcodes for the register service and unregister service
	  bit 31 in option is taken for this purpose. if it is set it means in NWP that the service should be added
	  if it is cleared it means that the service should be deleted and there is only meaning to pServiceName
	  and ServiceNameLen values. */
	Options |=  NETAPP_MDNS_OPTIONS_ADD_SERVICE_BIT;

    return  sl_NetAppMDNSRegisterUnregisterService(	pServiceName, 
											        ServiceNameLen,
													pText,
													TextLen,
													Port,
													TTL,
													Options);

	
}
#endif
/**********************************************************************************************/



/**********************************************************************************************/
#if _SL_INCLUDE_FUNC(sl_NetAppMDNSUnRegisterService)

_i16 sl_NetAppMDNSUnRegisterService(	const _i8* 		pServiceName, 
									const _u8   ServiceNameLen)


{
    _u32    Options = 0;

	/*
	
	NOTE - pay attention

			The size of the service length  should be smaller than 255,
			Until the simplelink drive supports to variable length through SPI command.


	*/

	/*Clear the add service bit in the options parameter.
	  In order not use different opcodes for the register service and unregister service
	  bit 31 in option is taken for this purpose. if it is set it means in NWP that the service should be added
	  if it is cleared it means that the service should be deleted and there is only meaning to pServiceName
	  and ServiceNameLen values.*/
	
	Options &=  (~NETAPP_MDNS_OPTIONS_ADD_SERVICE_BIT);

    return  sl_NetAppMDNSRegisterUnregisterService(	pServiceName, 
											        ServiceNameLen,
													NULL,
													0,
													0,
													0,
													Options);

	
}
#endif
/**********************************************************************************************/



/*****************************************************************************/
/* sl_DnsGetHostByService */
/*****************************************************************************/
/*
 * The below struct depicts the constant parameters of the command/API sl_DnsGetHostByService.
 *
   1. ServiceLen                      - The length of the service should be smaller than 255.
   2. AddrLen                         - TIPv4 or IPv6 (SL_AF_INET , SL_AF_INET6).
*
 */

typedef struct 
{
	 _u8   ServiceLen;
	 _u8   AddrLen;
	 _u16  Padding;
}_GetHostByServiceCommand_t;



/*
 * The below structure depict the constant parameters that are returned in the Async event answer
 * according to command/API sl_DnsGetHostByService for IPv4 and IPv6.
 *
	1Status						- The status of the response.
	2.Address						- Contains the IP address of the service.
	3.Port							- Contains the port of the service.
	4.TextLen						- Contains the max length of the text that the user wants to get.
												it means that if the test of service is bigger that its value than
												the text is cut to inout_TextLen value.
										Output: Contain the length of the text that is returned. Can be full text or part
												of the text (see above).
															   
*
 */
typedef struct 
{
	_u16   Status;
	_u16   TextLen;
	_u32    Port;
	_u32    Address;
}_GetHostByServiceIPv4AsyncResponse_t;


typedef struct 
{
	_u16   Status;
	_u16   TextLen;
	_u32    Port;
	_u32    Address[4];
}_GetHostByServiceIPv6AsyncResponse_t;


typedef union
{
    _GetHostByServiceIPv4AsyncResponse_t IpV4;
    _GetHostByServiceIPv6AsyncResponse_t IpV6;
}_GetHostByServiceAsyncResponseAttribute_u;

/*
 * The below struct contains pointers to the output parameters that the user gives 
 *
 */
typedef struct
{
    _i16           Status;
	_u32   *out_pAddr;
	_u32   *out_pPort;
	_u16   *inout_TextLen; /* in: max len , out: actual len */
 _i8            *out_pText;
}_GetHostByServiceAsyncResponse_t;


typedef union
{
	_GetHostByServiceCommand_t      Cmd;
	_BasicResponse_t                Rsp;
}_SlGetHostByServiceMsg_u;


#if _SL_INCLUDE_FUNC(sl_NetAppDnsGetHostByService)

const _SlCmdCtrl_t _SlGetHostByServiceCtrl =
{
    SL_OPCODE_NETAPP_MDNSGETHOSTBYSERVICE,
    sizeof(_GetHostByServiceCommand_t),
    sizeof(_BasicResponse_t)
};

/******************************************************************************/

_i32 sl_NetAppDnsGetHostByService(_i8 		*pServiceName,	/* string containing all (or only part): name + subtype + service */
								  const _u8  ServiceLen,
								  const _u8  Family,			/* 4-IPv4 , 16-IPv6 */
								  _u32  pAddr[], 
								  _u32  *pPort,
								  _u16 *pTextLen, /* in: max len , out: actual len */
								  _i8          *pText
						         )
{
    _SlGetHostByServiceMsg_u         Msg;
    _SlCmdExt_t                      CmdExt ;
    _GetHostByServiceAsyncResponse_t AsyncRsp;
	_u8 ObjIdx = MAX_CONCURRENT_ACTIONS;

/*
	Note:
	1. The return's attributes are belonged to first service that is found.
	It can be other services with the same service name will response to
	the query. The results of these responses are saved in the peer cache of the NWP, and
	should be read by another API.

	2. Text length can be 120 bytes only - not more
	It is because of constraints in the NWP on the buffer that is allocated for the Async event.

	3.The API waits to Async event by blocking. It means that the API is finished only after an Async event
	is sent by the NWP.

	4.No rolling option!!! - only PTR type is sent.

 
*/
	/*build the attribute part of the command.
	  It contains the constant parameters of the command */

	Msg.Cmd.ServiceLen = ServiceLen;
	Msg.Cmd.AddrLen    = Family;

	/*Build the payload part of the command
	  Copy the service name and text to one buffer.*/

    _SlDrvResetCmdExt(&CmdExt);
	CmdExt.TxPayloadLen = ServiceLen;
    CmdExt.pTxPayload   = (_u8 *)pServiceName;

	/*set pointers to the output parameters (the returned parameters).
	  This pointers are belonged to local struct that is set to global Async response parameter.
	  It is done in order not to run more than one sl_DnsGetHostByService at the same time.
	  The API should be run only if global parameter is pointed to NULL. */
	AsyncRsp.out_pText     = pText;
	AsyncRsp.inout_TextLen = (_u16* )pTextLen;
	AsyncRsp.out_pPort     = pPort;
	AsyncRsp.out_pAddr     = (_u32 *)pAddr;


    ObjIdx = _SlDrvProtectAsyncRespSetting((_u8*)&AsyncRsp, GETHOSYBYSERVICE_ID, SL_MAX_SOCKETS);

    if (MAX_CONCURRENT_ACTIONS == ObjIdx)
    {
        return SL_POOL_IS_EMPTY;
    }

    
	if (SL_AF_INET6 == Family)  
	{
		g_pCB->ObjPool[ObjIdx].AdditionalData |= SL_NETAPP_FAMILY_MASK;
	}
    /* Send the command */
	VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlGetHostByServiceCtrl, &Msg, &CmdExt));

 
	 
    /* If the immediate reponse is O.K. than  wait for aSYNC event response. */
	if(SL_RET_CODE_OK == Msg.Rsp.status)
    {        
        _SlDrvSyncObjWaitForever(&g_pCB->ObjPool[ObjIdx].SyncObj);
        
		/* If we are - it means that Async event was sent.
		   The results are copied in the Async handle return functions */
		
		Msg.Rsp.status = AsyncRsp.Status;
    }

    _SlDrvReleasePoolObj(ObjIdx);
    return Msg.Rsp.status;
}
#endif

/******************************************************************************/

/******************************************************************************
    _sl_HandleAsync_DnsGetHostByService

    CALLER          NWP - Async event on sl_DnsGetHostByService with IPv4 Family


    DESCRIPTION: 
					
					Async event on sl_DnsGetHostByService command with IPv4 Family.
					Return service attributes like IP address, port and text according to service name.
					The user sets a service name Full/Part (see example below), and should get the:
					1. IP of the service
					2. The port of service.
					3. The text of service.

					Hence it can make a connection to the specific service and use it.
					It is similar to get host by name method.

					It is done by a single shot query with PTR type on the service name.



					Note:
					1. The return's attributes are belonged to first service that is found.
					It can be other services with the same service name will response to
					the query. The results of these responses are saved in the peer cache of the NWP, and
					should be read by another API.

	
	    PARAMETERS:

                  pVoidBuf - is point to opcode of the event.
				  it contains the outputs that are given to the user

				  outputs description:

				   1.out_pAddr[]					- output: Contain the IP address of the service.
				   2.out_pPort						- output: Contain the port of the service.
				   3.inout_TextLen					- Input:  Contain the max length of the text that the user wants to get.
															  it means that if the test of service is bigger that its value than
															  the text is cut to inout_TextLen value.
													  Output: Contain the length of the text that is returned. Can be full text or part
														      of the text (see above).
															   
				   4.out_pText						- Contain the text of the service (full or part see above- inout_TextLen description).

  *


    RETURNS:        success or fail.





******************************************************************************/
#ifndef SL_TINY_EXT
void _sl_HandleAsync_DnsGetHostByService(void *pVoidBuf)
{

	_GetHostByServiceAsyncResponse_t* Res;
	_u16 				  TextLen;
	_u16 				  UserTextLen;


	/*pVoidBuf - is point to opcode of the event.*/
    
	/*set pMsgArgs to point to the attribute of the event.*/
	_GetHostByServiceIPv4AsyncResponse_t   *pMsgArgs   = (_GetHostByServiceIPv4AsyncResponse_t *)_SL_RESP_ARGS_START(pVoidBuf);

    VERIFY_SOCKET_CB(NULL != g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs);

	/*IPv6*/
	if(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].AdditionalData & SL_NETAPP_FAMILY_MASK)
	{
		return;
	}
	/*IPv4*/
	else
	{
    /*************************************************************************************************
	
	1. Copy the attribute part of the evnt to the attribute part of the response
	sl_Memcpy(g_pCB->GetHostByServiceCB.pAsyncRsp, pMsgArgs, sizeof(_GetHostByServiceIPv4AsyncResponse_t));

    set to TextLen the text length of the service.*/
	TextLen = pMsgArgs->TextLen;
	
	/*Res pointed to mDNS global object struct */
		Res = (_GetHostByServiceAsyncResponse_t*)g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs;



	/*It is 4 bytes so we avoid from memcpy*/
	Res->out_pAddr[0]	= pMsgArgs->Address;
	Res->out_pPort[0]	= pMsgArgs->Port;
	Res->Status			= pMsgArgs->Status;
	
	/*set to TextLen the text length of the user (input fromthe user).*/
	UserTextLen			= Res->inout_TextLen[0];
    
	/*Cut the service text if the user requested for smaller text.*/
	UserTextLen = (TextLen <= UserTextLen) ? TextLen : UserTextLen;
	Res->inout_TextLen[0] = UserTextLen ;

    /**************************************************************************************************

	2. Copy the payload part of the evnt (the text) to the payload part of the response
	the lenght of the copy is according to the text length in the attribute part. */
	

	sl_Memcpy(Res->out_pText          ,
		     (_i8 *)(& pMsgArgs[1])  ,   /* & pMsgArgs[1] -> 1st byte after the fixed header = 1st byte of variable text.*/
			 UserTextLen              );


    /**************************************************************************************************/

		_SlDrvSyncObjSignal(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj);
		return;
	}
}

/*****************************************************************************/
/*  _sl_HandleAsync_DnsGetHostByAddr */
/*****************************************************************************/
void _sl_HandleAsync_DnsGetHostByAddr(void *pVoidBuf)
{
    SL_TRACE0(DBG_MSG, MSG_303, "STUB: _sl_HandleAsync_DnsGetHostByAddr not implemented yet!");
    return;
}
#endif

/*****************************************************************************/
/* sl_DnsGetHostByName */
/*****************************************************************************/
typedef union
{
    _GetHostByNameIPv4AsyncResponse_t IpV4;
    _GetHostByNameIPv6AsyncResponse_t IpV6;
}_GetHostByNameAsyncResponse_u;

typedef union
{
	_GetHostByNameCommand_t         Cmd;
	_BasicResponse_t                Rsp;
}_SlGetHostByNameMsg_u;


#if _SL_INCLUDE_FUNC(sl_NetAppDnsGetHostByName)
const _SlCmdCtrl_t _SlGetHostByNameCtrl =
{
    SL_OPCODE_NETAPP_DNSGETHOSTBYNAME,
    sizeof(_GetHostByNameCommand_t),
    sizeof(_BasicResponse_t)
};

_i16 sl_NetAppDnsGetHostByName(_i8 * hostname,const  _u16 usNameLen, _u32*  out_ip_addr,const _u8 family)
{
    _SlGetHostByNameMsg_u           Msg;
    _SlCmdExt_t                     ExtCtrl;
    _GetHostByNameAsyncResponse_u   AsyncRsp;
	_u8 ObjIdx = MAX_CONCURRENT_ACTIONS;


    _SlDrvResetCmdExt(&ExtCtrl);
    ExtCtrl.TxPayloadLen = usNameLen;
    ExtCtrl.pTxPayload = (_u8 *)hostname;

    Msg.Cmd.Len = usNameLen;
    Msg.Cmd.family = family;

	/*Use Obj to issue the command, if not available try later */
	ObjIdx = (_u8)_SlDrvWaitForPoolObj(GETHOSYBYNAME_ID,SL_MAX_SOCKETS);
	if (MAX_CONCURRENT_ACTIONS == ObjIdx)
	{
		return SL_POOL_IS_EMPTY;
	}

    _SlDrvProtectionObjLockWaitForever();

	g_pCB->ObjPool[ObjIdx].pRespArgs =  (_u8 *)&AsyncRsp;
	/*set bit to indicate IPv6 address is expected */
	if (SL_AF_INET6 == family)
	{
		g_pCB->ObjPool[ObjIdx].AdditionalData |= SL_NETAPP_FAMILY_MASK;
	}
	
    _SlDrvProtectionObjUnLock();

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlGetHostByNameCtrl, &Msg, &ExtCtrl));

    if(SL_RET_CODE_OK == Msg.Rsp.status)
    {
        _SlDrvSyncObjWaitForever(&g_pCB->ObjPool[ObjIdx].SyncObj);
        
        Msg.Rsp.status = AsyncRsp.IpV4.status;

        if(SL_OS_RET_CODE_OK == (_i16)Msg.Rsp.status)
        {
            sl_Memcpy((_i8 *)out_ip_addr,
                      (_i8 *)&AsyncRsp.IpV4.ip0, 
                      (SL_AF_INET == family) ? SL_IPV4_ADDRESS_SIZE : SL_IPV6_ADDRESS_SIZE);
        }
    }
    _SlDrvReleasePoolObj(ObjIdx);
    return Msg.Rsp.status;
}
#endif


/******************************************************************************/
/*  _sl_HandleAsync_DnsGetHostByName */
/******************************************************************************/
void _sl_HandleAsync_DnsGetHostByName(void *pVoidBuf)
{
    _GetHostByNameIPv4AsyncResponse_t     *pMsgArgs   = (_GetHostByNameIPv4AsyncResponse_t *)_SL_RESP_ARGS_START(pVoidBuf);

   _SlDrvProtectionObjLockWaitForever();

    VERIFY_SOCKET_CB(NULL != g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs);

	/*IPv6 */
	if(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].AdditionalData & SL_NETAPP_FAMILY_MASK)
	{
		sl_Memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(_GetHostByNameIPv6AsyncResponse_t));
	}
	/*IPv4 */
	else
	{
		sl_Memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(_GetHostByNameIPv4AsyncResponse_t));
	}
	_SlDrvSyncObjSignal(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj);
    _SlDrvProtectionObjUnLock();
    return;
}


#ifndef SL_TINY_EXT
void CopyPingResultsToReport(_PingReportResponse_t *pResults,SlPingReport_t *pReport)
{
    pReport->PacketsSent     = pResults->numSendsPings;
    pReport->PacketsReceived = pResults->numSuccsessPings;
    pReport->MinRoundTime    = pResults->rttMin;
    pReport->MaxRoundTime    = pResults->rttMax;
    pReport->AvgRoundTime    = pResults->rttAvg;
    pReport->TestTime        = pResults->testTime;
}

/*****************************************************************************/
/*  _sl_HandleAsync_PingResponse */
/*****************************************************************************/
void _sl_HandleAsync_PingResponse(void *pVoidBuf)
{
    _PingReportResponse_t     *pMsgArgs   = (_PingReportResponse_t *)_SL_RESP_ARGS_START(pVoidBuf);
    SlPingReport_t            pingReport;
    
    if(pPingCallBackFunc)
    {
        CopyPingResultsToReport(pMsgArgs,&pingReport);
        pPingCallBackFunc(&pingReport);
    }
    else
    {
       
        _SlDrvProtectionObjLockWaitForever();
        
        VERIFY_SOCKET_CB(NULL != g_pCB->PingCB.PingAsync.pAsyncRsp);

		if (NULL != g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs)
		{
		   sl_Memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(_PingReportResponse_t));
		   _SlDrvSyncObjSignal(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj);
		}
       _SlDrvProtectionObjUnLock();
    }
    return;
}
#endif

/*****************************************************************************/
/* sl_PingStart */
/*****************************************************************************/
typedef union
{
	_PingStartCommand_t   Cmd;
	_PingReportResponse_t  Rsp;
}_SlPingStartMsg_u;


typedef enum
{
  CMD_PING_TEST_RUNNING = 0,
  CMD_PING_TEST_STOPPED
}_SlPingStatus_e;


#if _SL_INCLUDE_FUNC(sl_NetAppPingStart)
_i16 sl_NetAppPingStart(const SlPingStartCommand_t* pPingParams,const _u8 family,SlPingReport_t *pReport,const P_SL_DEV_PING_CALLBACK pPingCallback)
{
    _SlCmdCtrl_t                CmdCtrl = {0, sizeof(_PingStartCommand_t), sizeof(_BasicResponse_t)};
    _SlPingStartMsg_u           Msg;
    _PingReportResponse_t       PingRsp;
    _u8 ObjIdx = MAX_CONCURRENT_ACTIONS;

    if( 0 == pPingParams->Ip ) 
    {/* stop any ongoing ping */
       return _SlDrvBasicCmd(SL_OPCODE_NETAPP_PINGSTOP); 
    }

    if(SL_AF_INET == family)
    {
        CmdCtrl.Opcode = SL_OPCODE_NETAPP_PINGSTART;
        sl_Memcpy(&Msg.Cmd.ip0, &pPingParams->Ip, SL_IPV4_ADDRESS_SIZE);
    }
    else
    {
        CmdCtrl.Opcode = SL_OPCODE_NETAPP_PINGSTART_V6;
        sl_Memcpy(&Msg.Cmd.ip0, &pPingParams->Ip, SL_IPV6_ADDRESS_SIZE);
    }

    Msg.Cmd.pingIntervalTime        = pPingParams->PingIntervalTime;
    Msg.Cmd.PingSize                = pPingParams->PingSize;
    Msg.Cmd.pingRequestTimeout      = pPingParams->PingRequestTimeout;
    Msg.Cmd.totalNumberOfAttempts   = pPingParams->TotalNumberOfAttempts;
    Msg.Cmd.flags                   = pPingParams->Flags;

    
    if( pPingCallback )
    {	
       pPingCallBackFunc = pPingCallback;
    }
    else
    {
       /*Use Obj to issue the command, if not available try later */
	   ObjIdx = (_u8)_SlDrvWaitForPoolObj(PING_ID,SL_MAX_SOCKETS);
	   if (MAX_CONCURRENT_ACTIONS == ObjIdx)
	   {
		  return SL_POOL_IS_EMPTY;
	   }
       OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));
        /* async response handler for non callback mode */
       g_pCB->ObjPool[ObjIdx].pRespArgs = (_u8 *)&PingRsp;
       pPingCallBackFunc = NULL;
       OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
    }

    
    VERIFY_RET_OK(_SlDrvCmdOp(&CmdCtrl, &Msg, NULL));
	/*send the command*/
    if(CMD_PING_TEST_RUNNING == (_i16)Msg.Rsp.status || CMD_PING_TEST_STOPPED == (_i16)Msg.Rsp.status )
    {
        /* block waiting for results if no callback function is used */
        if( NULL == pPingCallback )
        {
            _SlDrvSyncObjWaitForever(&g_pCB->ObjPool[ObjIdx].SyncObj);

            if( SL_OS_RET_CODE_OK == (_i16)PingRsp.status )
            {
                CopyPingResultsToReport(&PingRsp,pReport);
            }
            _SlDrvReleasePoolObj(ObjIdx);
        }
    }
    else
    {   /* ping failure, no async response */
        if( NULL == pPingCallback ) 
        {	
            _SlDrvReleasePoolObj(ObjIdx);
        }
    }

    return Msg.Rsp.status;
}
#endif

/*****************************************************************************/
/* sl_NetAppSet */
/*****************************************************************************/
typedef union
{
    _NetAppSetGet_t    Cmd;
    _BasicResponse_t   Rsp;
}_SlNetAppMsgSet_u;


#if _SL_INCLUDE_FUNC(sl_NetAppSet)

const _SlCmdCtrl_t _SlNetAppSetCmdCtrl =
{
    SL_OPCODE_NETAPP_NETAPPSET,
    sizeof(_NetAppSetGet_t),
    sizeof(_BasicResponse_t)
};

_i32 sl_NetAppSet(const _u8 AppId ,const _u8 Option,const _u8 OptionLen,const  _u8 *pOptionValue)
{
    _SlNetAppMsgSet_u         Msg;
    _SlCmdExt_t               CmdExt;


    _SlDrvResetCmdExt(&CmdExt);
	CmdExt.TxPayloadLen = (OptionLen+3) & (~3);
    CmdExt.pTxPayload = (_u8 *)pOptionValue;


    Msg.Cmd.AppId    = AppId;
    Msg.Cmd.ConfigLen   = OptionLen;
	Msg.Cmd.ConfigOpt   = Option;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlNetAppSetCmdCtrl, &Msg, &CmdExt));

    return (_i16)Msg.Rsp.status;
}
#endif

/*****************************************************************************/
/* sl_NetAppSendTokenValue */
/*****************************************************************************/
typedef union
{
    sl_NetAppHttpServerSendToken_t    Cmd;
    _BasicResponse_t   Rsp;
}_SlNetAppMsgSendTokenValue_u;



#if defined(sl_HttpServerCallback) || defined(EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS)
const _SlCmdCtrl_t _SlNetAppSendTokenValueCmdCtrl =
{
    SL_OPCODE_NETAPP_HTTPSENDTOKENVALUE,
    sizeof(sl_NetAppHttpServerSendToken_t),
    sizeof(_BasicResponse_t)
};

_u16 _sl_NetAppSendTokenValue(slHttpServerData_t * Token_value)
{
	_SlNetAppMsgSendTokenValue_u    Msg;
    _SlCmdExt_t						CmdExt;

	CmdExt.TxPayloadLen = (Token_value->value_len+3) & (~3);
    CmdExt.RxPayloadLen = 0;
	CmdExt.pTxPayload = (_u8 *) Token_value->token_value;
    CmdExt.pRxPayload = NULL;

	Msg.Cmd.token_value_len = Token_value->value_len;
	Msg.Cmd.token_name_len = Token_value->name_len;
	sl_Memcpy(&Msg.Cmd.token_name[0], Token_value->token_name, Token_value->name_len);
	

	VERIFY_RET_OK(_SlDrvCmdSend((_SlCmdCtrl_t *)&_SlNetAppSendTokenValueCmdCtrl, &Msg, &CmdExt));

	return Msg.Rsp.status;
}
#endif


/*****************************************************************************/
/* sl_NetAppGet */
/*****************************************************************************/
typedef union
{
	_NetAppSetGet_t	    Cmd;
	_NetAppSetGet_t	    Rsp;
}_SlNetAppMsgGet_u;


#if _SL_INCLUDE_FUNC(sl_NetAppGet)
const _SlCmdCtrl_t _SlNetAppGetCmdCtrl =
{
    SL_OPCODE_NETAPP_NETAPPGET,
    sizeof(_NetAppSetGet_t),
    sizeof(_NetAppSetGet_t)
};

_i32 sl_NetAppGet(const _u8 AppId,const  _u8 Option,_u8 *pOptionLen, _u8 *pOptionValue)
{
    _SlNetAppMsgGet_u         Msg;
    _SlCmdExt_t               CmdExt;

       if (*pOptionLen == 0)
       {
              return SL_EZEROLEN;
       }

    _SlDrvResetCmdExt(&CmdExt);
    CmdExt.RxPayloadLen = *pOptionLen;
    CmdExt.pRxPayload = (_u8 *)pOptionValue;

    Msg.Cmd.AppId    = AppId;
    Msg.Cmd.ConfigOpt   = Option;
    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlNetAppGetCmdCtrl, &Msg, &CmdExt));
    

       if (CmdExt.RxPayloadLen < CmdExt.ActualRxPayloadLen) 
       {
              *pOptionLen = (_u8)CmdExt.RxPayloadLen;
              return SL_ESMALLBUF;
       }
       else
       {
              *pOptionLen = (_u8)CmdExt.ActualRxPayloadLen;
       }
  
    return (_i16)Msg.Rsp.Status;
}
#endif


/*****************************************************************************/
/* _SlDrvNetAppEventHandler */
/*****************************************************************************/
void _SlDrvNetAppEventHandler(void* pArgs)
{
    _SlResponseHeader_t     *pHdr       = (_SlResponseHeader_t *)pArgs;
#if defined(sl_HttpServerCallback) || defined(EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS)
    SlHttpServerEvent_t		httpServerEvent;
    SlHttpServerResponse_t	httpServerResponse;
#endif
    
    switch(pHdr->GenHeader.Opcode)
    {
        case SL_OPCODE_NETAPP_DNSGETHOSTBYNAMEASYNCRESPONSE:
        case SL_OPCODE_NETAPP_DNSGETHOSTBYNAMEASYNCRESPONSE_V6:
            _sl_HandleAsync_DnsGetHostByName(pArgs);
            break;
#ifndef SL_TINY_EXT            
        case SL_OPCODE_NETAPP_MDNSGETHOSTBYSERVICEASYNCRESPONSE:
        case SL_OPCODE_NETAPP_MDNSGETHOSTBYSERVICEASYNCRESPONSE_V6:
            _sl_HandleAsync_DnsGetHostByService(pArgs);
            break;
        case SL_OPCODE_NETAPP_PINGREPORTREQUESTRESPONSE:
            _sl_HandleAsync_PingResponse(pArgs);
            break;
#endif

#if defined(sl_HttpServerCallback) || defined(EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS)
		case SL_OPCODE_NETAPP_HTTPGETTOKENVALUE:
		{              
			_u8 *pTokenName;
			slHttpServerData_t Token_value;
			sl_NetAppHttpServerGetToken_t *httpGetToken = (sl_NetAppHttpServerGetToken_t *)_SL_RESP_ARGS_START(pHdr);
                        pTokenName = (_u8 *)((sl_NetAppHttpServerGetToken_t *)httpGetToken + 1);

			httpServerResponse.Response = SL_NETAPP_HTTPSETTOKENVALUE;
			httpServerResponse.ResponseData.token_value.len = MAX_TOKEN_VALUE_LEN;

            /* Reuse the async buffer for getting the token value response from the user */
			httpServerResponse.ResponseData.token_value.data = (_u8 *)_SL_RESP_ARGS_START(pHdr) + MAX_TOKEN_NAME_LEN;

            httpServerEvent.Event = SL_NETAPP_HTTPGETTOKENVALUE_EVENT;
			httpServerEvent.EventData.httpTokenName.len = httpGetToken->token_name_len;
			httpServerEvent.EventData.httpTokenName.data = pTokenName;

			Token_value.token_name =  pTokenName;

            _SlDrvHandleHttpServerEvents (&httpServerEvent, &httpServerResponse);			

			Token_value.value_len = httpServerResponse.ResponseData.token_value.len;
			Token_value.name_len = httpServerEvent.EventData.httpTokenName.len;
			Token_value.token_value = httpServerResponse.ResponseData.token_value.data;
			    

			_sl_NetAppSendTokenValue(&Token_value);
		}
		break;

		case SL_OPCODE_NETAPP_HTTPPOSTTOKENVALUE:
		{
			_u8 *pPostParams;

			sl_NetAppHttpServerPostToken_t *httpPostTokenArgs = (sl_NetAppHttpServerPostToken_t *)_SL_RESP_ARGS_START(pHdr);
			pPostParams = (_u8 *)((sl_NetAppHttpServerPostToken_t *)httpPostTokenArgs + 1);

			httpServerEvent.Event = SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT;

			httpServerEvent.EventData.httpPostData.action.len = httpPostTokenArgs->post_action_len;
			httpServerEvent.EventData.httpPostData.action.data = pPostParams;
			pPostParams+=httpPostTokenArgs->post_action_len;

			httpServerEvent.EventData.httpPostData.token_name.len = httpPostTokenArgs->token_name_len;
			httpServerEvent.EventData.httpPostData.token_name.data = pPostParams;
			pPostParams+=httpPostTokenArgs->token_name_len;

			httpServerEvent.EventData.httpPostData.token_value.len = httpPostTokenArgs->token_value_len;
			httpServerEvent.EventData.httpPostData.token_value.data = pPostParams;

			httpServerResponse.Response = SL_NETAPP_RESPONSE_NONE;

            _SlDrvHandleHttpServerEvents (&httpServerEvent, &httpServerResponse);
			
		}
		break;
#endif

        
        default:
            SL_ERROR_TRACE2(MSG_305, "ASSERT: _SlDrvNetAppEventHandler : invalid opcode = 0x%x = %1", pHdr->GenHeader.Opcode, pHdr->GenHeader.Opcode);
            VERIFY_PROTOCOL(0);
    }
}

