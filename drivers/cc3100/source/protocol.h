/*
 * protocol.h - CC31xx/CC32xx Host Driver Implementation
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/ 
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

/*******************************************************************************\
*
*   FILE NAME:      protocol.h
*
*   DESCRIPTION:    Constant and data structure definitions and function
*                   prototypes for the SL protocol module, which implements
*                   processing of SimpleLink Commands.
*
*   AUTHOR:
*
\*******************************************************************************/

#ifndef _SL_PROTOCOL_TYPES_H_
#define _SL_PROTOCOL_TYPES_H_

/****************************************************************************
**
**  User I/F pools definitions
**
****************************************************************************/

/****************************************************************************
**
**  Definitions for SimpleLink Commands
**
****************************************************************************/


/* pattern for LE 8/16/32 or BE*/
#define H2N_SYNC_PATTERN     {0xBBDDEEFF,0x4321,0x34,0x12}
#define H2N_CNYS_PATTERN     {0xBBDDEEFF,0x8765,0x78,0x56}

#define H2N_DUMMY_PATTERN    (_u32)0xFFFFFFFF
#define N2H_SYNC_PATTERN     (_u32)0xABCDDCBA

#define SYNC_PATTERN_LEN     (_u32)sizeof(_u32)
#define UART_SET_MODE_MAGIC_CODE    (_u32)0xAA55AA55
#define SPI_16BITS_BUG(pattern)     (_u32)((_u32)pattern & (_u32)0xFFFF7FFF)
#define SPI_8BITS_BUG(pattern)      (_u32)((_u32)pattern & (_u32)0xFFFFFF7F)



typedef struct
{
	_u16 Opcode;
	_u16 Len;
}_SlGenericHeader_t;


typedef struct
{
    _u32  Long;
    _u16  Short;
    _u8  Byte1;
    _u8  Byte2;
}_SlSyncPattern_t;

typedef _SlGenericHeader_t _SlCommandHeader_t;

typedef struct
{
    _SlGenericHeader_t  GenHeader;
    _u8               TxPoolCnt;
    _u8               DevStatus;
    _u8               SocketTXFailure;
    _u8               SocketNonBlocking;
}_SlResponseHeader_t;

#define _SL_RESP_SPEC_HDR_SIZE (sizeof(_SlResponseHeader_t) - sizeof(_SlGenericHeader_t))
#define _SL_RESP_HDR_SIZE       sizeof(_SlResponseHeader_t)
#define _SL_CMD_HDR_SIZE        sizeof(_SlCommandHeader_t)

#define _SL_RESP_ARGS_START(_pMsg) (((_SlResponseHeader_t *)(_pMsg)) + 1)

/* Used only in NWP! */
typedef struct
{
    _SlCommandHeader_t  sl_hdr;
    _u8   func_args_start;
} T_SCMD;


#define WLAN_CONN_STATUS_BIT 0x01
#define EVENTS_Q_STATUS_BIT  0x02
#define PENDING_RCV_CMD_BIT  0x04
#define FW_BUSY_PACKETS_BIT  0x08

#define INIT_STA_OK   0x11111111
#define INIT_STA_ERR  0x22222222
#define INIT_AP_OK    0x33333333
#define INIT_AP_ERR   0x44444444
#define INIT_P2P_OK   0x55555555
#define INIT_P2P_ERR  0x66666666

/****************************************************************************
**  OPCODES
****************************************************************************/
#define SL_IPV4_IPV6_OFFSET                            ( 9 )
#define SL_OPCODE_IPV4							       ( 0x0 << SL_IPV4_IPV6_OFFSET )
#define SL_OPCODE_IPV6							       ( 0x1 << SL_IPV4_IPV6_OFFSET )

#define SL_SYNC_ASYNC_OFFSET                           ( 10 )
#define SL_OPCODE_SYNC							       (0x1 << SL_SYNC_ASYNC_OFFSET )
#define SL_OPCODE_SILO_OFFSET                           ( 11 )
#define SL_OPCODE_SILO_MASK                             ( 0xF << SL_OPCODE_SILO_OFFSET )
#define SL_OPCODE_SILO_DEVICE                           ( 0x0 << SL_OPCODE_SILO_OFFSET )
#define SL_OPCODE_SILO_WLAN                             ( 0x1 << SL_OPCODE_SILO_OFFSET )
#define SL_OPCODE_SILO_SOCKET                           ( 0x2 << SL_OPCODE_SILO_OFFSET )
#define SL_OPCODE_SILO_NETAPP                           ( 0x3 << SL_OPCODE_SILO_OFFSET )
#define SL_OPCODE_SILO_NVMEM                            ( 0x4 << SL_OPCODE_SILO_OFFSET )
#define SL_OPCODE_SILO_NETCFG                           ( 0x5 << SL_OPCODE_SILO_OFFSET )

#define SL_FAMILY_SHIFT                            (0x4)
#define SL_FLAGS_MASK                              (0xF)

#define SL_OPCODE_DEVICE_INITCOMPLETE                               	0x0008
#define SL_OPCODE_DEVICE_ABORT			                               	0x000C
#define SL_OPCODE_DEVICE_STOP_COMMAND                               	0x8473
#define SL_OPCODE_DEVICE_STOP_RESPONSE                              	0x0473
#define SL_OPCODE_DEVICE_STOP_ASYNC_RESPONSE                        	0x0073
#define SL_OPCODE_DEVICE_DEVICEASYNCDUMMY                           	0x0063

#define SL_OPCODE_DEVICE_VERSIONREADCOMMAND	                            0x8470
#define SL_OPCODE_DEVICE_VERSIONREADRESPONSE	                        0x0470
#define SL_OPCODE_DEVICE_ASYNC_GENERAL_ERROR                        	0x0078
#define SL_OPCODE_WLAN_WLANCONNECTCOMMAND                           	0x8C80
#define SL_OPCODE_WLAN_WLANCONNECTRESPONSE                          	0x0C80
#define SL_OPCODE_WLAN_WLANASYNCCONNECTEDRESPONSE                   	0x0880
#define SL_OPCODE_WLAN_P2P_DEV_FOUND                                    0x0830
#define SL_OPCODE_WLAN_CONNECTION_FAILED                                0x0831
#define SL_OPCODE_WLAN_P2P_NEG_REQ_RECEIVED                             0x0832

#define SL_OPCODE_WLAN_WLANDISCONNECTCOMMAND                        	0x8C81
#define SL_OPCODE_WLAN_WLANDISCONNECTRESPONSE                       	0x0C81
#define SL_OPCODE_WLAN_WLANASYNCDISCONNECTEDRESPONSE                	0x0881
#define SL_OPCODE_WLAN_WLANCONNECTEAPCOMMAND                        	0x8C82
#define SL_OPCODE_WLAN_WLANCONNECTEAPCRESPONSE                      	0x0C82
#define SL_OPCODE_WLAN_PROFILEADDCOMMAND                            	0x8C83
#define SL_OPCODE_WLAN_PROFILEADDRESPONSE                           	0x0C83
#define SL_OPCODE_WLAN_PROFILEGETCOMMAND                            	0x8C84
#define SL_OPCODE_WLAN_PROFILEGETRESPONSE                           	0x0C84
#define SL_OPCODE_WLAN_PROFILEDELCOMMAND                            	0x8C85
#define SL_OPCODE_WLAN_PROFILEDELRESPONSE                           	0x0C85
#define SL_OPCODE_WLAN_POLICYSETCOMMAND                             	0x8C86
#define SL_OPCODE_WLAN_POLICYSETRESPONSE                            	0x0C86
#define SL_OPCODE_WLAN_POLICYGETCOMMAND                             	0x8C87
#define SL_OPCODE_WLAN_POLICYGETRESPONSE                            	0x0C87
#define SL_OPCODE_WLAN_FILTERADD                                    	0x8C88
#define SL_OPCODE_WLAN_FILTERADDRESPONSE                            	0x0C88
#define SL_OPCODE_WLAN_FILTERGET                                    	0x8C89
#define SL_OPCODE_WLAN_FILTERGETRESPONSE                            	0x0C89
#define SL_OPCODE_WLAN_FILTERDELETE                                 	0x8C8A
#define SL_OPCODE_WLAN_FILTERDELETERESPOSNE                         	0x0C8A
#define SL_OPCODE_WLAN_WLANGETSTATUSCOMMAND                         	0x8C8F
#define SL_OPCODE_WLAN_WLANGETSTATUSRESPONSE                        	0x0C8F
#define SL_OPCODE_WLAN_STARTTXCONTINUESCOMMAND                      	0x8CAA
#define SL_OPCODE_WLAN_STARTTXCONTINUESRESPONSE                     	0x0CAA
#define SL_OPCODE_WLAN_STOPTXCONTINUESCOMMAND                       	0x8CAB
#define SL_OPCODE_WLAN_STOPTXCONTINUESRESPONSE                      	0x0CAB
#define SL_OPCODE_WLAN_STARTRXSTATCOMMAND                           	0x8CAC
#define SL_OPCODE_WLAN_STARTRXSTATRESPONSE                          	0x0CAC
#define SL_OPCODE_WLAN_STOPRXSTATCOMMAND                            	0x8CAD
#define SL_OPCODE_WLAN_STOPRXSTATRESPONSE                           	0x0CAD
#define SL_OPCODE_WLAN_GETRXSTATCOMMAND                             	0x8CAF
#define SL_OPCODE_WLAN_GETRXSTATRESPONSE                            	0x0CAF
#define SL_OPCODE_WLAN_POLICYSETCOMMANDNEW                          	0x8CB0
#define SL_OPCODE_WLAN_POLICYSETRESPONSENEW                         	0x0CB0
#define SL_OPCODE_WLAN_POLICYGETCOMMANDNEW                          	0x8CB1
#define SL_OPCODE_WLAN_POLICYGETRESPONSENEW                         	0x0CB1

#define SL_OPCODE_WLAN_SMART_CONFIG_START_COMMAND                   	0x8CB2
#define SL_OPCODE_WLAN_SMART_CONFIG_START_RESPONSE                  	0x0CB2
#define SL_OPCODE_WLAN_SMART_CONFIG_START_ASYNC_RESPONSE            	0x08B2
#define SL_OPCODE_WLAN_SMART_CONFIG_STOP_COMMAND                    	0x8CB3
#define SL_OPCODE_WLAN_SMART_CONFIG_STOP_RESPONSE                   	0x0CB3
#define SL_OPCODE_WLAN_SMART_CONFIG_STOP_ASYNC_RESPONSE             	0x08B3
#define SL_OPCODE_WLAN_SET_MODE                                         0x8CB4
#define SL_OPCODE_WLAN_SET_MODE_RESPONSE                                0x0CB4
#define SL_OPCODE_WLAN_CFG_SET                                          0x8CB5
#define SL_OPCODE_WLAN_CFG_SET_RESPONSE                                 0x0CB5
#define SL_OPCODE_WLAN_CFG_GET                                          0x8CB6
#define SL_OPCODE_WLAN_CFG_GET_RESPONSE                                 0x0CB6
#define SL_OPCODE_WLAN_STA_CONNECTED                                	0x082E
#define SL_OPCODE_WLAN_STA_DISCONNECTED                             	0x082F
#define SL_OPCODE_WLAN_EAP_PROFILEADDCOMMAND                            0x8C67
#define SL_OPCODE_WLAN_EAP_PROFILEADDCOMMAND_RESPONSE                   0x0C67 

#define SL_OPCODE_SOCKET_SOCKET                                     	0x9401
#define SL_OPCODE_SOCKET_SOCKETRESPONSE                             	0x1401
#define SL_OPCODE_SOCKET_CLOSE                                      	0x9402
#define SL_OPCODE_SOCKET_CLOSERESPONSE                              	0x1402
#define SL_OPCODE_SOCKET_ACCEPT                                     	0x9403
#define SL_OPCODE_SOCKET_ACCEPTRESPONSE                             	0x1403
#define SL_OPCODE_SOCKET_ACCEPTASYNCRESPONSE                        	0x1003
#define SL_OPCODE_SOCKET_ACCEPTASYNCRESPONSE_V6                     	0x1203
#define SL_OPCODE_SOCKET_BIND                                       	0x9404
#define SL_OPCODE_SOCKET_BIND_V6                                    	0x9604
#define SL_OPCODE_SOCKET_BINDRESPONSE                               	0x1404
#define SL_OPCODE_SOCKET_LISTEN                                     	0x9405
#define SL_OPCODE_SOCKET_LISTENRESPONSE                             	0x1405
#define SL_OPCODE_SOCKET_CONNECT                                    	0x9406
#define SL_OPCODE_SOCKET_CONNECT_V6                                 	0x9606
#define SL_OPCODE_SOCKET_CONNECTRESPONSE                            	0x1406
#define SL_OPCODE_SOCKET_CONNECTASYNCRESPONSE                       	0x1006
#define SL_OPCODE_SOCKET_SELECT                                     	0x9407
#define SL_OPCODE_SOCKET_SELECTRESPONSE                             	0x1407
#define SL_OPCODE_SOCKET_SELECTASYNCRESPONSE                        	0x1007
#define SL_OPCODE_SOCKET_SETSOCKOPT                                 	0x9408
#define SL_OPCODE_SOCKET_SETSOCKOPTRESPONSE                         	0x1408
#define SL_OPCODE_SOCKET_GETSOCKOPT                                 	0x9409
#define SL_OPCODE_SOCKET_GETSOCKOPTRESPONSE                         	0x1409
#define SL_OPCODE_SOCKET_RECV                                       	0x940A
#define SL_OPCODE_SOCKET_RECVASYNCRESPONSE                          	0x100A
#define SL_OPCODE_SOCKET_RECVFROM                                   	0x940B
#define SL_OPCODE_SOCKET_RECVFROMASYNCRESPONSE                      	0x100B
#define SL_OPCODE_SOCKET_RECVFROMASYNCRESPONSE_V6                   	0x120B
#define SL_OPCODE_SOCKET_SEND                                       	0x940C
#define SL_OPCODE_SOCKET_SENDTO                                     	0x940D
#define SL_OPCODE_SOCKET_SENDTO_V6                                  	0x960D
#define SL_OPCODE_SOCKET_TXFAILEDASYNCRESPONSE                      	0x100E
#define SL_OPCODE_SOCKET_SOCKETASYNCEVENT                               0x100F
#define SL_OPCODE_NETAPP_START_COMMAND                                  0x9C0A
#define SL_OPCODE_NETAPP_START_RESPONSE                                	0x1C0A
#define SL_OPCODE_NETAPP_NETAPPSTARTRESPONSE                        	0x1C0A
#define SL_OPCODE_NETAPP_STOP_COMMAND                               	0x9C61
#define SL_OPCODE_NETAPP_STOP_RESPONSE                              	0x1C61
#define SL_OPCODE_NETAPP_NETAPPSET                            	        0x9C0B
#define SL_OPCODE_NETAPP_NETAPPSETRESPONSE                    	        0x1C0B
#define SL_OPCODE_NETAPP_NETAPPGET                            	        0x9C27
#define SL_OPCODE_NETAPP_NETAPPGETRESPONSE                    	        0x1C27
#define SL_OPCODE_NETAPP_DNSGETHOSTBYNAME                           	0x9C20
#define SL_OPCODE_NETAPP_DNSGETHOSTBYNAMERESPONSE                   	0x1C20
#define SL_OPCODE_NETAPP_DNSGETHOSTBYNAMEASYNCRESPONSE              	0x1820
#define SL_OPCODE_NETAPP_DNSGETHOSTBYNAMEASYNCRESPONSE_V6           	0x1A20
#define SL_OPCODE_NETAPP_NETAPP_MDNS_LOOKUP_SERVICE                     0x9C71
#define SL_OPCODE_NETAPP_NETAPP_MDNS_LOOKUP_SERVICE_RESPONSE            0x1C72
#define SL_OPCODE_NETAPP_MDNSREGISTERSERVICE                            0x9C34
#define SL_OPCODE_NETAPP_MDNSREGISTERSERVICERESPONSE                    0x1C34
#define SL_OPCODE_NETAPP_MDNSGETHOSTBYSERVICE                           0x9C35
#define SL_OPCODE_NETAPP_MDNSGETHOSTBYSERVICERESPONSE                   0x1C35
#define SL_OPCODE_NETAPP_MDNSGETHOSTBYSERVICEASYNCRESPONSE              0x1835
#define SL_OPCODE_NETAPP_MDNSGETHOSTBYSERVICEASYNCRESPONSE_V6           0x1A35
#define SL_OPCODE_NETAPP_DNSGETHOSTBYADDR                           	0x9C26
#define SL_OPCODE_NETAPP_DNSGETHOSTBYADDR_V6                        	0x9E26
#define SL_OPCODE_NETAPP_DNSGETHOSTBYADDRRESPONSE                   	0x1C26
#define SL_OPCODE_NETAPP_DNSGETHOSTBYADDRASYNCRESPONSE              	0x1826
#define SL_OPCODE_NETAPP_PINGSTART                                  	0x9C21
#define SL_OPCODE_NETAPP_PINGSTART_V6                               	0x9E21
#define SL_OPCODE_NETAPP_PINGSTARTRESPONSE                          	0x1C21
#define SL_OPCODE_NETAPP_PINGREPORTREQUEST                          	0x9C22
#define SL_OPCODE_NETAPP_PINGREPORTREQUESTRESPONSE                  	0x1822
#define SL_OPCODE_NETAPP_PINGSTOP                                   	0x9C23
#define SL_OPCODE_NETAPP_PINGSTOPRESPONSE                           	0x1C23
#define SL_OPCODE_NETAPP_ARPFLUSH                                   	0x9C24
#define SL_OPCODE_NETAPP_ARPFLUSHRESPONSE                           	0x1C24
#define SL_OPCODE_NETAPP_IPACQUIRED                                 	0x1825
#define SL_OPCODE_NETAPP_IPV4_LOST	                                 	0x1832
#define SL_OPCODE_NETAPP_DHCP_IPV4_ACQUIRE_TIMEOUT                  	0x1833
#define SL_OPCODE_NETAPP_IPACQUIRED_V6                              	0x1A25
#define SL_OPCODE_NETAPP_IPERFSTARTCOMMAND                          	0x9C28
#define SL_OPCODE_NETAPP_IPERFSTARTRESPONSE                         	0x1C28
#define SL_OPCODE_NETAPP_IPERFSTOPCOMMAND                           	0x9C29
#define SL_OPCODE_NETAPP_IPERFSTOPRESPONSE                          	0x1C29
#define SL_OPCODE_NETAPP_CTESTSTARTCOMMAND                          	0x9C2A
#define SL_OPCODE_NETAPP_CTESTSTARTRESPONSE                         	0x1C2A
#define SL_OPCODE_NETAPP_CTESTASYNCRESPONSE                         	0x182A
#define SL_OPCODE_NETAPP_CTESTSTOPCOMMAND                           	0x9C2B
#define SL_OPCODE_NETAPP_CTESTSTOPRESPONSE                          	0x1C2B
#define SL_OPCODE_NETAPP_IP_LEASED                                  	0x182C
#define SL_OPCODE_NETAPP_IP_RELEASED                                	0x182D
#define SL_OPCODE_NETAPP_HTTPGETTOKENVALUE                          	0x182E
#define SL_OPCODE_NETAPP_HTTPSENDTOKENVALUE                         	0x9C2F
#define SL_OPCODE_NETAPP_HTTPPOSTTOKENVALUE                         	0x1830
#define SL_OPCODE_NVMEM_FILEOPEN                                    	0xA43C
#define SL_OPCODE_NVMEM_FILEOPENRESPONSE                             	0x243C
#define SL_OPCODE_NVMEM_FILECLOSE                                    	0xA43D
#define SL_OPCODE_NVMEM_FILECLOSERESPONSE                           	0x243D
#define SL_OPCODE_NVMEM_FILEREADCOMMAND                              	0xA440
#define SL_OPCODE_NVMEM_FILEREADRESPONSE                            	0x2440
#define SL_OPCODE_NVMEM_FILEWRITECOMMAND                            	0xA441
#define SL_OPCODE_NVMEM_FILEWRITERESPONSE                           	0x2441
#define SL_OPCODE_NVMEM_FILEGETINFOCOMMAND                          	0xA442
#define SL_OPCODE_NVMEM_FILEGETINFORESPONSE                         	0x2442
#define SL_OPCODE_NVMEM_FILEDELCOMMAND                              	0xA443
#define SL_OPCODE_NVMEM_FILEDELRESPONSE                             	0x2443
#define SL_OPCODE_NVMEM_NVMEMFORMATCOMMAND                          	0xA444
#define SL_OPCODE_NVMEM_NVMEMFORMATRESPONSE                         	0x2444

#define SL_OPCODE_DEVICE_SETDEBUGLEVELCOMMAND                       	0x846A
#define SL_OPCODE_DEVICE_SETDEBUGLEVELRESPONSE                      	0x046A

#define SL_OPCODE_DEVICE_NETCFG_SET_COMMAND                 	        0x8432
#define SL_OPCODE_DEVICE_NETCFG_SET_RESPONSE                	        0x0432
#define SL_OPCODE_DEVICE_NETCFG_GET_COMMAND                 	        0x8433
#define SL_OPCODE_DEVICE_NETCFG_GET_RESPONSE                	        0x0433
/*  */
#define SL_OPCODE_DEVICE_SETUARTMODECOMMAND                         	0x846B
#define SL_OPCODE_DEVICE_SETUARTMODERESPONSE                        	0x046B
#define SL_OPCODE_DEVICE_SSISIZESETCOMMAND	                            0x846B
#define SL_OPCODE_DEVICE_SSISIZESETRESPONSE	                            0x046B

/*  */
#define SL_OPCODE_DEVICE_EVENTMASKSET                               	0x8464
#define SL_OPCODE_DEVICE_EVENTMASKSETRESPONSE                       	0x0464
#define SL_OPCODE_DEVICE_EVENTMASKGET                               	0x8465
#define SL_OPCODE_DEVICE_EVENTMASKGETRESPONSE                       	0x0465

#define SL_OPCODE_DEVICE_DEVICEGET                                  	0x8466
#define SL_OPCODE_DEVICE_DEVICEGETRESPONSE                              0x0466
#define SL_OPCODE_DEVICE_DEVICESET										0x84B7
#define SL_OPCODE_DEVICE_DEVICESETRESPONSE								0x04B7

#define SL_OPCODE_WLAN_SCANRESULTSGETCOMMAND                        	0x8C8C
#define SL_OPCODE_WLAN_SCANRESULTSGETRESPONSE                       	0x0C8C
#define SL_OPCODE_WLAN_SMARTCONFIGOPTSET                            	0x8C8D
#define SL_OPCODE_WLAN_SMARTCONFIGOPTSETRESPONSE                    	0x0C8D
#define SL_OPCODE_WLAN_SMARTCONFIGOPTGET                            	0x8C8E
#define SL_OPCODE_WLAN_SMARTCONFIGOPTGETRESPONSE                    	0x0C8E


/* Rx Filters opcodes */
#define SL_OPCODE_WLAN_WLANRXFILTERADDCOMMAND                           0x8C6C
#define SL_OPCODE_WLAN_WLANRXFILTERADDRESPONSE                          0x0C6C
#define SL_OPCODE_WLAN_WLANRXFILTERSETCOMMAND                           0x8C6D
#define SL_OPCODE_WLAN_WLANRXFILTERSETRESPONSE                          0x0C6D
#define SL_OPCODE_WLAN_WLANRXFILTERGETSTATISTICSINFOCOMMAND             0x8C6E
#define SL_OPCODE_WLAN_WLANRXFILTERGETSTATISTICSINFORESPONSE            0x0C6E
#define SL_OPCODE_WLAN_WLANRXFILTERGETCOMMAND                           0x8C6F
#define SL_OPCODE_WLAN_WLANRXFILTERGETRESPONSE                          0x0C6F
#define SL_OPCODE_WLAN_WLANRXFILTERGETINFO                              0x8C70
#define SL_OPCODE_WLAN_WLANRXFILTERGETINFORESPONSE                      0x0C70


/******************************************************************************************/
/*   Device structs  */
/******************************************************************************************/
typedef _u32 InitStatus_t;


typedef struct
{
    _i32 Status;
}InitComplete_t;

typedef struct
{
  _i16 status;
  _u16 padding;

}_BasicResponse_t;

typedef struct
{
  _u16 Timeout;
  _u16 padding;
}_DevStopCommand_t;

typedef struct
{
  _u32 group;
  _u32 mask;
}_DevMaskEventSetCommand_t;

typedef _BasicResponse_t _DevMaskEventSetResponse_t;


typedef struct
{
  _u32 group;
}_DevMaskEventGetCommand_t;


typedef struct
{
  _u32 group;
  _u32 mask;
}_DevMaskEventGetResponse_t;


typedef struct
{
  _u32 group;
}_DevStatusGetCommand_t;


typedef struct
{
  _u32 group;
  _u32 status;
}_DevStatusGetResponse_t;

typedef struct
{
    _u32  ChipId;
    _u32  FwVersion[4];
    _u8   PhyVersion[4];
}_Device_VersionReadResponsePart_t;

typedef struct
{
    _Device_VersionReadResponsePart_t part;
    _u32                            NwpVersion[4];
    _u16                            RomVersion;
    _u16                            Padding;
}_Device_VersionReadResponseFull_t;


typedef struct
{
	_u32 BaudRate;
	_u8  FlowControlEnable;
}_DevUartSetModeCommand_t;

typedef _BasicResponse_t _DevUartSetModeResponse_t;

/******************************************************/

typedef struct
{
    _u8 SsiSizeInBytes;
    _u8 Padding[3];
}_StellarisSsiSizeSet_t;

/*****************************************************************************************/
/*   WLAN structs */
/*****************************************************************************************/
#define MAXIMAL_PASSWORD_LENGTH					(64)

typedef struct{
	_u8	SecType;
	_u8	SsidLen;
    _u8	Bssid[6];
	_u8	PasswordLen;
}_WlanConnectCommon_t;

#define SSID_STRING(pCmd)       (_i8 *)((_WlanConnectCommon_t *)(pCmd) + 1)
#define PASSWORD_STRING(pCmd)   (SSID_STRING(pCmd) + ((_WlanConnectCommon_t *)(pCmd))->SsidLen)

typedef struct{
	_WlanConnectCommon_t            Common;
	_u8							UserLen;
	_u8							AnonUserLen;
    _u8   						CertIndex;
    _u32  						EapBitmask;
}_WlanConnectEapCommand_t;

#define EAP_SSID_STRING(pCmd)       (_i8 *)((_WlanConnectEapCommand_t *)(pCmd) + 1)
#define EAP_PASSWORD_STRING(pCmd)   (EAP_SSID_STRING(pCmd) + ((_WlanConnectEapCommand_t *)(pCmd))->Common.SsidLen)
#define EAP_USER_STRING(pCmd)       (EAP_PASSWORD_STRING(pCmd) + ((_WlanConnectEapCommand_t *)(pCmd))->Common.PasswordLen)
#define EAP_ANON_USER_STRING(pCmd)  (EAP_USER_STRING(pCmd) + ((_WlanConnectEapCommand_t *)(pCmd))->UserLen)


typedef struct
{
    _u8	PolicyType;
    _u8       Padding;
    _u8	PolicyOption;
    _u8	PolicyOptionLen;
}_WlanPoliciySetGet_t;


typedef struct{
	_u32  minDwellTime;
	_u32  maxDwellTime;
	_u32  numProbeResponse;
	_u32  G_Channels_mask;
	_i32   rssiThershold;
	_i32   snrThershold;
	_i32   defaultTXPower;
	_u16  intervalList[16];
}_WlanScanParamSetCommand_t;


typedef struct{
	_i8	SecType;
	_u8	SsidLen;
	_u8	Priority;
	_u8	Bssid[6];
    _u8   PasswordLen;
    _u8   WepKeyId;
}_WlanAddGetProfile_t;


typedef struct{
       _WlanAddGetProfile_t              Common;
       _u8                             UserLen;
       _u8                             AnonUserLen;
       _u8                             CertIndex;
       _u16                            padding;
       _u32                            EapBitmask;
}_WlanAddGetEapProfile_t;




#define PROFILE_SSID_STRING(pCmd)       ((_i8 *)((_WlanAddGetProfile_t *)(pCmd) + 1))
#define PROFILE_PASSWORD_STRING(pCmd)   (PROFILE_SSID_STRING(pCmd) + ((_WlanAddGetProfile_t *)(pCmd))->SsidLen)

#define EAP_PROFILE_SSID_STRING(pCmd)       (_i8 *)((_WlanAddGetEapProfile_t *)(pCmd) + 1)
#define EAP_PROFILE_PASSWORD_STRING(pCmd)   (EAP_PROFILE_SSID_STRING(pCmd) + ((_WlanAddGetEapProfile_t *)(pCmd))->Common.SsidLen)
#define EAP_PROFILE_USER_STRING(pCmd)       (EAP_PROFILE_PASSWORD_STRING(pCmd) + ((_WlanAddGetEapProfile_t *)(pCmd))->Common.PasswordLen)
#define EAP_PROFILE_ANON_USER_STRING(pCmd)  (EAP_PROFILE_USER_STRING(pCmd) + ((_WlanAddGetEapProfile_t *)(pCmd))->UserLen)



typedef struct
{
	_u8	index;
	_u8	padding[3];
}_WlanProfileDelGetCommand_t;

typedef _BasicResponse_t _WlanGetNetworkListResponse_t;

typedef struct
{
	_u8 	index;
	_u8 	count;
 _i8 	padding[2];
}_WlanGetNetworkListCommand_t;




typedef struct
{
    _u32  						groupIdBitmask;
    _u8                           cipher;
    _u8                           publicKeyLen;
    _u8                           group1KeyLen;
    _u8                           group2KeyLen;
}_WlanSmartConfigStartCommand_t;

#define SMART_CONFIG_START_PUBLIC_KEY_STRING(pCmd)       ((_i8 *)((_WlanSmartConfigStartCommand_t *)(pCmd) + 1))
#define SMART_CONFIG_START_GROUP1_KEY_STRING(pCmd)       ((_i8 *) (SMART_CONFIG_START_PUBLIC_KEY_STRING(pCmd) + ((_WlanSmartConfigStartCommand_t *)(pCmd))->publicKeyLen))
#define SMART_CONFIG_START_GROUP2_KEY_STRING(pCmd)       ((_i8 *) (SMART_CONFIG_START_GROUP1_KEY_STRING(pCmd) + ((_WlanSmartConfigStartCommand_t *)(pCmd))->group1KeyLen))



typedef	struct
{
	_u8	mode;
    _u8   padding[3];
}_WlanSetMode_t;




typedef struct
{
    _u16  Status;
    _u16  ConfigId;
    _u16  ConfigOpt;
    _u16  ConfigLen;
}_WlanCfgSetGet_t;


/* ******************************************************************************/
/*     RX filters - Start  */
/* ******************************************************************************/
/*  -- 80 bytes */
typedef struct _WlanRxFilterAddCommand_t
{
	/*  -- 1 byte */
	SlrxFilterRuleType_t RuleType;
	/*  -- 1 byte */
	SlrxFilterFlags_t FilterFlags;
	/*  --  1 byte */
	SlrxFilterID_t FilterId;
	/*  --  1 byte */
	_u8 Padding;
	/*  -- 56 byte */
	SlrxFilterRule_t Rule;
	/*  --  12 byte ( 3 padding ) */
	SlrxFilterTrigger_t Trigger;
	/*  --  8 byte */
	SlrxFilterAction_t Action;
}_WlanRxFilterAddCommand_t;



/* -- 4 bytes */
typedef struct l_WlanRxFilterAddCommandReponse_t
{
	/*  -- 1 byte */
	SlrxFilterID_t FilterId;
	/* -- 1 Byte */
	_u8          Status;
	/*  -- 2 byte */
	_u8  Padding[2];

}_WlanRxFilterAddCommandReponse_t;



/*
 * \struct _WlanRxFilterSetCommand_t
 */
typedef struct _WlanRxFilterSetCommand_t
{
	_u16 InputBufferLength;
	/* 1 byte */
	SLrxFilterOperation_t RxFilterOperation;
	_u8 Padding[1];
}_WlanRxFilterSetCommand_t;

/**
 * \struct _WlanRxFilterSetCommandReponse_t
 */
typedef struct _WlanRxFilterSetCommandReponse_t
{
	/* 1 byte */
	_u8  Status;
	/* 3 bytes  */
	_u8 Padding[3];

}_WlanRxFilterSetCommandReponse_t;

/**
 * \struct _WlanRxFilterGetCommand_t
 */
typedef struct _WlanRxFilterGetCommand_t
{
	_u16 OutputBufferLength;
	/* 1 byte  */
	SLrxFilterOperation_t RxFilterOperation;
	_u8 Padding[1];
}_WlanRxFilterGetCommand_t;

/**
 * \struct _WlanRxFilterGetCommandReponse_t
 */
typedef struct _WlanRxFilterGetCommandReponse_t
{
	/* 1 byte  */
	_u8  Status;
	/* 1 bytes  */
	_u8 Padding;
	/* 2 byte  */
	_u16 OutputBufferLength;

}_WlanRxFilterGetCommandReponse_t;



/* ******************************************************************************/
/*     RX filters -- End  */
/* ******************************************************************************/

typedef struct
{
    _u16 status;
    _u8  WlanRole;     /* 0 = station, 2 = AP */
    _u8  Ipv6Enabled;
    _u8  Ipv6DhcpEnabled;

    _u32 ipV6Global[4];
    _u32 ipV6Local[4];
    _u32 ipV6DnsServer[4];
    _u8  Ipv6DhcpState;

}_NetappIpV6configRetArgs_t;


typedef struct
{
    _u8  ipV4[4];
    _u8  ipV4Mask[4];
    _u8  ipV4Gateway[4];
    _u8  ipV4DnsServer[4];
	_u8  ipV4Start[4];
	_u8  ipV4End[4];
}_NetCfgIpV4AP_Args_t;



typedef struct
{
  _u16 status;
  _u8  MacAddr[6];
} _MAC_Address_SetGet_t;


typedef struct
{
    _u16  Status;
    _u16	ConfigId;
	_u16	ConfigOpt;
    _u16	ConfigLen;
}_NetCfgSetGet_t;

typedef struct
{
	_u16  Status;
	_u16  DeviceSetId;
	_u16  Option;
	_u16  ConfigLen;
}_DeviceSetGet_t;




/******************************************************************************************/
/*   Socket structs  */
/******************************************************************************************/

typedef struct
{
  _u8 Domain;
  _u8 Type;
  _u8 Protocol;
  _u8 Padding;
}_SocketCommand_t;


typedef struct
{
  _i16 statusOrLen;
  _u8  sd;
  _u8  padding;
}_SocketResponse_t;

typedef struct
{
  _u8 sd;
  _u8 family;
  _u8 padding1;
  _u8 padding2;
}_AcceptCommand_t;


typedef struct
{
  _i16 statusOrLen;
  _u8 sd;
  _u8 family;
  _u16 port;
  _u16 paddingOrAddr;
  _u32 address;
}_SocketAddrAsyncIPv4Response_t;

typedef struct
{
  _i16 statusOrLen;
  _u8 sd;
  _u8 family;
  _u16 port;
  _u8 address[6];
}_SocketAddrAsyncIPv6EUI48Response_t;
typedef struct
{
  _i16 statusOrLen;
  _u8 sd;
  _u8 family;
  _u16 port;
  _u16 paddingOrAddr;
  _u32 address[4];
}_SocketAddrAsyncIPv6Response_t;


typedef struct
{
  _i16 lenOrPadding;
  _u8 sd;
  _u8 FamilyAndFlags;
  _u16 port;
  _u16 paddingOrAddr;
  _u32 address;
}_SocketAddrIPv4Command_t;

typedef struct
{
  _i16 lenOrPadding;
  _u8 sd;
  _u8 FamilyAndFlags;
  _u16 port;
  _u8 address[6];
}_SocketAddrIPv6EUI48Command_t;
typedef struct
{
  _i16 lenOrPadding;
  _u8 sd;
  _u8 FamilyAndFlags;
  _u16 port;
  _u16 paddingOrAddr;
  _u32 address[4];
}_SocketAddrIPv6Command_t;

typedef union {
    _SocketAddrIPv4Command_t IpV4;
    _SocketAddrIPv6EUI48Command_t IpV6EUI48;
#ifdef SL_SUPPORT_IPV6
    _SocketAddrIPv6Command_t IpV6;
#endif
} _SocketAddrCommand_u;

typedef union {
    _SocketAddrAsyncIPv4Response_t IpV4;
    _SocketAddrAsyncIPv6EUI48Response_t IpV6EUI48;
#ifdef SL_SUPPORT_IPV6
    _SocketAddrAsyncIPv6Response_t IpV6;
#endif
} _SocketAddrResponse_u;

typedef struct
{
  _u8 sd;
  _u8 backlog;
  _u8 padding1;
  _u8 padding2;
}_ListenCommand_t;

typedef struct
{
  _u8 sd;
  _u8 padding0;
  _u8 padding1;
  _u8 padding2;
}_CloseCommand_t;


typedef struct
{
  _u8 nfds;
  _u8 readFdsCount;
  _u8 writeFdsCount;
  _u8 padding;
  _u16 readFds;
  _u16 writeFds;
  _u16 tv_usec;
  _u16 tv_sec;
}_SelectCommand_t;


typedef struct
{
  _u16 status;
  _u8 readFdsCount;
  _u8 writeFdsCount;
  _u16 readFds;
  _u16 writeFds;
}_SelectAsyncResponse_t;

typedef struct
{
  _u8 sd;
  _u8 level;
  _u8 optionName;
  _u8 optionLen;
}_setSockOptCommand_t;

typedef struct
{
  _u8 sd;
  _u8 level;
  _u8 optionName;
  _u8 optionLen;
}_getSockOptCommand_t;

typedef struct
{
  _i16 status;
  _u8 sd;
  _u8 optionLen;
}_getSockOptResponse_t;


typedef struct
{
  _u16 StatusOrLen;
  _u8  sd;
  _u8 FamilyAndFlags;
}_sendRecvCommand_t;

/*****************************************************************************************
*   NETAPP structs
******************************************************************************************/


typedef _BasicResponse_t _NetAppStartStopResponse_t;

typedef struct
{
    _u32  appId;
}_NetAppStartStopCommand_t;

typedef struct
{
    _u16  Status;
    _u16	AppId;
    _u16	ConfigOpt;
    _u16	ConfigLen;
}_NetAppSetGet_t;
typedef struct
{
    _u16  port_number;
} _NetAppHttpServerGetSet_port_num_t;

typedef struct
{
    _u8  auth_enable;
}_NetAppHttpServerGetSet_auth_enable_t;

typedef struct _sl_NetAppHttpServerGetToken_t
{
	_u8	token_name_len;
	_u8	padd1;
	_u16	padd2;
}sl_NetAppHttpServerGetToken_t;

typedef struct _sl_NetAppHttpServerSendToken_t
{
	_u8	token_value_len;
	_u8	token_name_len;
	_u8   token_name[MAX_TOKEN_NAME_LEN];
	_u16	padd;
}sl_NetAppHttpServerSendToken_t;

typedef struct _sl_NetAppHttpServerPostToken_t
{
	_u8 post_action_len;
	_u8 token_name_len;
	_u8 token_value_len;
	_u8 padding;
}sl_NetAppHttpServerPostToken_t;


typedef struct
{
  _u16 Len;
  _u8  family;
  _u8  padding;
}_GetHostByNameCommand_t;

typedef struct
{
  _u16 status;
  _u16 padding;
  _u32 ip0;
  _u32 ip1;
  _u32 ip2;
  _u32 ip3;
}_GetHostByNameIPv6AsyncResponse_t;

typedef struct
{
  _u16 status;
  _u8  padding1;
  _u8  padding2;
  _u32 ip0;
}_GetHostByNameIPv4AsyncResponse_t;




typedef enum
{
    CTST_BSD_UDP_TX,
    CTST_BSD_UDP_RX,
    CTST_BSD_TCP_TX,
    CTST_BSD_TCP_RX,
    CTST_BSD_TCP_SERVER_BI_DIR,
    CTST_BSD_TCP_CLIENT_BI_DIR,
    CTST_BSD_UDP_BI_DIR,
    CTST_BSD_RAW_TX,
    CTST_BSD_RAW_RX,
    CTST_BSD_RAW_BI_DIR,
    CTST_BSD_SECURED_TCP_TX,
    CTST_BSD_SECURED_TCP_RX,
    CTST_BSD_SECURED_TCP_SERVER_BI_DIR,
    CTST_BSD_SECURED_TCP_CLIENT_BI_DIR
 }CommTest_e;

typedef struct _sl_protocol_CtestStartCommand_t
{
    _u32 Test;
    _u16 DestPort;
    _u16 SrcPort;
    _u32 DestAddr[4];
    _u32 PayloadSize;
    _u32 timeout;
    _u32 csEnabled;
    _u32 secure;
    _u32 rawProtocol;
    _u8  reserved1[4];
}_CtestStartCommand_t;

typedef struct
{
  _u8  test;
  _u8  socket;
  _i16  status;
  _u32 startTime;
  _u32 endTime;
  _u16 txKbitsSec;
  _u16 rxKbitsSec;
  _u32 outOfOrderPackets;
  _u32 missedPackets;
  _i16 token;
}_CtestAsyncResponse_t;

typedef struct
{
    _u32 pingIntervalTime;
    _u16 PingSize;
    _u16 pingRequestTimeout;
    _u32 totalNumberOfAttempts;
    _u32 flags;
    _u32 ip0;
    _u32 ip1OrPaadding;
    _u32 ip2OrPaadding;
    _u32 ip3OrPaadding;
}_PingStartCommand_t;

typedef struct
{
    _u16 status;
    _u16 rttMin;
    _u16 rttMax;
    _u16 rttAvg;
    _u32 numSuccsessPings;
    _u32 numSendsPings;
    _u32 testTime;
}_PingReportResponse_t;


typedef struct
{
    _u32 ip;
    _u32 gateway;
    _u32 dns;
}_IpV4AcquiredAsync_t;


typedef enum
{
  ACQUIRED_IPV6_LOCAL = 1,
  ACQUIRED_IPV6_GLOBAL
}IpV6AcquiredType_e;


typedef struct
{
    _u32 type;
    _u32 ip[4];
    _u32 gateway[4];
    _u32 dns[4];
}_IpV6AcquiredAsync_t;


typedef union
{
    _SocketCommand_t     EventMask;
    _sendRecvCommand_t   DeviceInit;
}_device_commands_t;

/*****************************************************************************************
*   FS structs
******************************************************************************************/

typedef struct
{
    _u32 FileHandle;
    _u32 Offset;
    _u16 Len;  
    _u16 Padding;  
}_FsReadCommand_t;

typedef struct
{
  _u32 Mode;
  _u32 Token;
}_FsOpenCommand_t;

typedef struct 
{
  _u32 FileHandle;
  _u32 Token;
}_FsOpenResponse_t;


typedef struct
{
  _u32 FileHandle;
  _u32 CertificFileNameLength;
  _u32 SignatureLen;
}_FsCloseCommand_t;


typedef _BasicResponse_t _FsReadResponse_t;
typedef _BasicResponse_t _FsDeleteResponse_t;
typedef _BasicResponse_t _FsCloseResponse_t;

typedef struct
{
    _u16 Status;
    _u16 flags;
    _u32 FileLen;
    _u32 AllocatedLen;
    _u32 Token[4];
}_FsGetInfoResponse_t;

typedef struct
{
    _u8 DeviceID;
    _u8 Padding[3];
}_FsFormatCommand_t;

typedef _BasicResponse_t _FsFormatResponse_t;

typedef struct
{
    _u32 Token;
}_FsDeleteCommand_t;

typedef   _FsDeleteCommand_t  _FsGetInfoCommand_t;

typedef struct
{
    _u32 FileHandle;
    _u32 Offset;
    _u16 Len;
    _u16 Padding;
}_FsWriteCommand_t;

typedef _BasicResponse_t _FsWriteResponse_t;



/* TODO: Set MAx Async Payload length depending on flavor (Tiny, Small, etc.) */


#ifdef SL_TINY_EXT
#define SL_ASYNC_MAX_PAYLOAD_LEN        120  /* size must be aligned to 4 */
#else
#define SL_ASYNC_MAX_PAYLOAD_LEN        160 /* size must be aligned to 4 */
#endif

#define SL_ASYNC_MAX_MSG_LEN            (_SL_RESP_HDR_SIZE + SL_ASYNC_MAX_PAYLOAD_LEN)

#define RECV_ARGS_SIZE                  (sizeof(_SocketResponse_t))
#define RECVFROM_IPV4_ARGS_SIZE         (sizeof(_SocketAddrAsyncIPv4Response_t))
#define RECVFROM_IPV6_ARGS_SIZE         (sizeof(_SocketAddrAsyncIPv6Response_t))

#define SL_IPV4_ADDRESS_SIZE 			(sizeof(_u32))
#define SL_IPV6_ADDRESS_SIZE 			(4 * sizeof(_u32))

#endif /*  _SL_PROTOCOL_TYPES_H_  */
