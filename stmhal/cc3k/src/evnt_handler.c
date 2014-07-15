/*****************************************************************************
*
*  evnt_handler.c  - CC3000 Host Driver Implementation.
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
//! \addtogroup evnt_handler_api
//! @{
//
//******************************************************************************

//******************************************************************************
//                  INCLUDE FILES
//******************************************************************************

#include "cc3000_common.h"
#include "string.h"
#include "hci.h"
#include "evnt_handler.h"
#include "wlan.h"
#include "socket.h"
#include "netapp.h"
#include "ccspi.h"



//*****************************************************************************
//                  COMMON DEFINES
//*****************************************************************************

#define FLOW_CONTROL_EVENT_HANDLE_OFFSET		(0)
#define FLOW_CONTROL_EVENT_BLOCK_MODE_OFFSET	(1)
#define FLOW_CONTROL_EVENT_FREE_BUFFS_OFFSET	(2)
#define FLOW_CONTROL_EVENT_SIZE					(4)

#define BSD_RSP_PARAMS_SOCKET_OFFSET		(0)
#define BSD_RSP_PARAMS_STATUS_OFFSET		(4)

#define GET_HOST_BY_NAME_RETVAL_OFFSET	(0)
#define GET_HOST_BY_NAME_ADDR_OFFSET	(4)

#define ACCEPT_SD_OFFSET			(0)
#define ACCEPT_RETURN_STATUS_OFFSET	(4)
#define ACCEPT_ADDRESS__OFFSET		(8)

#define SL_RECEIVE_SD_OFFSET			(0)
#define SL_RECEIVE_NUM_BYTES_OFFSET		(4)
#define SL_RECEIVE__FLAGS__OFFSET		(8)


#define SELECT_STATUS_OFFSET			(0)
#define SELECT_READFD_OFFSET			(4)
#define SELECT_WRITEFD_OFFSET			(8)
#define SELECT_EXFD_OFFSET				(12)


#define NETAPP_IPCONFIG_IP_OFFSET				(0)
#define NETAPP_IPCONFIG_SUBNET_OFFSET			(4)
#define NETAPP_IPCONFIG_GW_OFFSET				(8)
#define NETAPP_IPCONFIG_DHCP_OFFSET				(12)
#define NETAPP_IPCONFIG_DNS_OFFSET				(16)
#define NETAPP_IPCONFIG_MAC_OFFSET				(20)
#define NETAPP_IPCONFIG_SSID_OFFSET				(26)

#define NETAPP_IPCONFIG_IP_LENGTH				(4)
#define NETAPP_IPCONFIG_MAC_LENGTH				(6)
#define NETAPP_IPCONFIG_SSID_LENGTH				(32)


#define NETAPP_PING_PACKETS_SENT_OFFSET			(0)
#define NETAPP_PING_PACKETS_RCVD_OFFSET			(4)
#define NETAPP_PING_MIN_RTT_OFFSET				(8)
#define NETAPP_PING_MAX_RTT_OFFSET				(12)
#define NETAPP_PING_AVG_RTT_OFFSET				(16)

#define GET_SCAN_RESULTS_TABlE_COUNT_OFFSET				(0)
#define GET_SCAN_RESULTS_SCANRESULT_STATUS_OFFSET		(4)
#define GET_SCAN_RESULTS_ISVALID_TO_SSIDLEN_OFFSET		(8)
#define GET_SCAN_RESULTS_FRAME_TIME_OFFSET				(10)
#define GET_SCAN_RESULTS_SSID_MAC_LENGTH				(38)

#define GET_MSS_VAL_RETVAL_OFFSET	(0)

//*****************************************************************************
//                  GLOBAL VARAIABLES
//*****************************************************************************

UINT32 socket_active_status = SOCKET_STATUS_INIT_VAL; 


//*****************************************************************************
//            Prototypes for the static functions
//*****************************************************************************

static INT32 hci_event_unsol_flowcontrol_handler(CHAR *pEvent);

static void update_socket_active_status(CHAR *resp_params);


//*****************************************************************************
//
//!  hci_unsol_handle_patch_request
//!
//!  @param  event_hdr  event header
//!
//!  @return none
//!
//!  @brief   Handle unsolicited event from type patch request
//
//*****************************************************************************
void hci_unsol_handle_patch_request(CHAR *event_hdr)
{
	CHAR *params = (CHAR *)(event_hdr) + HCI_EVENT_HEADER_SIZE;
	UINT32 ucLength = 0;
	CHAR *patch;

	switch (*params)
	{
	case HCI_EVENT_PATCHES_DRV_REQ:

		if (tSLInformation.sDriverPatches)
		{
			patch = tSLInformation.sDriverPatches(&ucLength);

			if (patch)
			{
				hci_patch_send(HCI_EVENT_PATCHES_DRV_REQ, 
					tSLInformation.pucTxCommandBuffer, patch, ucLength);
				return;
			}
		}

		// Send 0 length Patches response event
		hci_patch_send(HCI_EVENT_PATCHES_DRV_REQ, 
			tSLInformation.pucTxCommandBuffer, 0, 0);
		break;

	case HCI_EVENT_PATCHES_FW_REQ:

		if (tSLInformation.sFWPatches)
		{
			patch = tSLInformation.sFWPatches(&ucLength);

			// Build and send a patch
			if (patch)
			{
				hci_patch_send(HCI_EVENT_PATCHES_FW_REQ, 
					tSLInformation.pucTxCommandBuffer, patch, ucLength);
				return;
			}
		}

		// Send 0 length Patches response event
		hci_patch_send(HCI_EVENT_PATCHES_FW_REQ, 
			tSLInformation.pucTxCommandBuffer, 0, 0);
		break;

	case HCI_EVENT_PATCHES_BOOTLOAD_REQ:

		if (tSLInformation.sBootLoaderPatches)
		{
			patch = tSLInformation.sBootLoaderPatches(&ucLength);

			if (patch)
			{
				hci_patch_send(HCI_EVENT_PATCHES_BOOTLOAD_REQ,  
					tSLInformation.pucTxCommandBuffer, patch, ucLength);
				return;
			}
		}

		// Send 0 length Patches response event
		hci_patch_send(HCI_EVENT_PATCHES_BOOTLOAD_REQ, 
			tSLInformation.pucTxCommandBuffer, 0, 0);
		break;
	}
}



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


UINT8 * hci_event_handler(void *pRetParams, UINT8 *from, UINT8 *fromlen)
{
	UINT8 *pucReceivedData, ucArgsize;
	UINT16 usLength;
	UINT8 *pucReceivedParams;
	UINT16 usReceivedEventOpcode = 0;
	UINT32 retValue32;
	UINT8 * RecvParams;
	UINT8 *RetParams;


	while (1)
	{
		if (tSLInformation.usEventOrDataReceived != 0)
		{				
			pucReceivedData = (tSLInformation.pucReceivedData);

			if (*pucReceivedData == HCI_TYPE_EVNT)
			{
				// Event Received
				STREAM_TO_UINT16((CHAR *)pucReceivedData, HCI_EVENT_OPCODE_OFFSET,
					usReceivedEventOpcode);
				pucReceivedParams = pucReceivedData + HCI_EVENT_HEADER_SIZE;		
				RecvParams = pucReceivedParams;
				RetParams = pRetParams;

				// In case unsolicited event received - here the handling finished
				if (hci_unsol_event_handler((CHAR *)pucReceivedData) == 0)
				{
					STREAM_TO_UINT8(pucReceivedData, HCI_DATA_LENGTH_OFFSET, usLength);

					switch(usReceivedEventOpcode)
					{		
					case HCI_CMND_READ_BUFFER_SIZE:
						{
							STREAM_TO_UINT8((CHAR *)pucReceivedParams, 0, 
								tSLInformation.usNumberOfFreeBuffers);
							STREAM_TO_UINT16((CHAR *)pucReceivedParams, 1, 
								tSLInformation.usSlBufferLength);
						}
						break;

					case HCI_CMND_WLAN_CONFIGURE_PATCH:
					case HCI_NETAPP_DHCP:
					case HCI_NETAPP_PING_SEND:
					case HCI_NETAPP_PING_STOP:
					case HCI_NETAPP_ARP_FLUSH:
					case HCI_NETAPP_SET_DEBUG_LEVEL:
					case HCI_NETAPP_SET_TIMERS:
					case HCI_EVNT_NVMEM_READ:
					case HCI_EVNT_NVMEM_CREATE_ENTRY:
					case HCI_CMND_NVMEM_WRITE_PATCH:
					case HCI_NETAPP_PING_REPORT:
					case HCI_EVNT_MDNS_ADVERTISE:

						STREAM_TO_UINT8(pucReceivedData, HCI_EVENT_STATUS_OFFSET
							,*(UINT8 *)pRetParams);
						break;

					case HCI_CMND_SETSOCKOPT:
					case HCI_CMND_WLAN_CONNECT:
					case HCI_CMND_WLAN_IOCTL_STATUSGET:
					case HCI_EVNT_WLAN_IOCTL_ADD_PROFILE:
					case HCI_CMND_WLAN_IOCTL_DEL_PROFILE:
					case HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY:
					case HCI_CMND_WLAN_IOCTL_SET_SCANPARAM:
					case HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_START:
					case HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_STOP:
					case HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_SET_PREFIX:
					case HCI_CMND_EVENT_MASK:
					case HCI_EVNT_WLAN_DISCONNECT:
					case HCI_EVNT_SOCKET:
					case HCI_EVNT_BIND:
					case HCI_CMND_LISTEN:
					case HCI_EVNT_CLOSE_SOCKET:
					case HCI_EVNT_CONNECT:
					case HCI_EVNT_NVMEM_WRITE:

						STREAM_TO_UINT32((CHAR *)pucReceivedParams,0
							,*(UINT32 *)pRetParams);
						break;

					case HCI_EVNT_READ_SP_VERSION:

						STREAM_TO_UINT8(pucReceivedData, HCI_EVENT_STATUS_OFFSET
							,*(UINT8 *)pRetParams);
						pRetParams = ((CHAR *)pRetParams) + 1;
						STREAM_TO_UINT32((CHAR *)pucReceivedParams, 0, retValue32);
						UINT32_TO_STREAM((UINT8 *)pRetParams, retValue32);				
						break;

					case HCI_EVNT_BSD_GETHOSTBYNAME:

						STREAM_TO_UINT32((CHAR *)pucReceivedParams
							,GET_HOST_BY_NAME_RETVAL_OFFSET,*(UINT32 *)pRetParams);
						pRetParams = ((CHAR *)pRetParams) + 4;
						STREAM_TO_UINT32((CHAR *)pucReceivedParams
							,GET_HOST_BY_NAME_ADDR_OFFSET,*(UINT32 *)pRetParams);					
						break;

					case HCI_EVNT_GETMSSVALUE:

						STREAM_TO_UINT16((CHAR *)pucReceivedParams
							,GET_MSS_VAL_RETVAL_OFFSET,*(UINT16 *)pRetParams);					

						break;

					case HCI_EVNT_ACCEPT:
						{
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,ACCEPT_SD_OFFSET
								,*(UINT32 *)pRetParams);
							pRetParams = ((CHAR *)pRetParams) + 4;
							STREAM_TO_UINT32((CHAR *)pucReceivedParams
								,ACCEPT_RETURN_STATUS_OFFSET,*(UINT32 *)pRetParams);
							pRetParams = ((CHAR *)pRetParams) + 4; 

							//This argument returns in network order
							memcpy((UINT8 *)pRetParams, 
								pucReceivedParams + ACCEPT_ADDRESS__OFFSET, sizeof(sockaddr));	
							break;
						}

					case HCI_EVNT_RECV:
					case HCI_EVNT_RECVFROM:
						{
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,SL_RECEIVE_SD_OFFSET ,*(UINT32 *)pRetParams);
							pRetParams = ((CHAR *)pRetParams) + 4;
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,SL_RECEIVE_NUM_BYTES_OFFSET,*(UINT32 *)pRetParams);
							pRetParams = ((CHAR *)pRetParams) + 4;
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,SL_RECEIVE__FLAGS__OFFSET,*(UINT32 *)pRetParams);

							if(((tBsdReadReturnParams *)pRetParams)->iNumberOfBytes == ERROR_SOCKET_INACTIVE)
							{
								set_socket_active_status(((tBsdReadReturnParams *)pRetParams)->iSocketDescriptor,SOCKET_STATUS_INACTIVE);
							}
							break;
						}

					case HCI_EVNT_SEND:
					case HCI_EVNT_SENDTO:
						{
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,SL_RECEIVE_SD_OFFSET ,*(UINT32 *)pRetParams);
							pRetParams = ((CHAR *)pRetParams) + 4;
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,SL_RECEIVE_NUM_BYTES_OFFSET,*(UINT32 *)pRetParams);
							pRetParams = ((CHAR *)pRetParams) + 4;

							break;
						}

					case HCI_EVNT_SELECT:
						{ 
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,SELECT_STATUS_OFFSET,*(UINT32 *)pRetParams);
							pRetParams = ((CHAR *)pRetParams) + 4;
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,SELECT_READFD_OFFSET,*(UINT32 *)pRetParams);
							pRetParams = ((CHAR *)pRetParams) + 4;
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,SELECT_WRITEFD_OFFSET,*(UINT32 *)pRetParams);
							pRetParams = ((CHAR *)pRetParams) + 4;
							STREAM_TO_UINT32((CHAR *)pucReceivedParams,SELECT_EXFD_OFFSET,*(UINT32 *)pRetParams);			
							break;
						}

					case HCI_CMND_GETSOCKOPT:

						STREAM_TO_UINT8(pucReceivedData, HCI_EVENT_STATUS_OFFSET,((tBsdGetSockOptReturnParams *)pRetParams)->iStatus);
						//This argument returns in network order
						memcpy((UINT8 *)pRetParams, pucReceivedParams, 4);
						break;

					case HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS:

						STREAM_TO_UINT32((CHAR *)pucReceivedParams,GET_SCAN_RESULTS_TABlE_COUNT_OFFSET,*(UINT32 *)pRetParams);
						pRetParams = ((CHAR *)pRetParams) + 4;   					
						STREAM_TO_UINT32((CHAR *)pucReceivedParams,GET_SCAN_RESULTS_SCANRESULT_STATUS_OFFSET,*(UINT32 *)pRetParams);
						pRetParams = ((CHAR *)pRetParams) + 4;                                                        					
						STREAM_TO_UINT16((CHAR *)pucReceivedParams,GET_SCAN_RESULTS_ISVALID_TO_SSIDLEN_OFFSET,*(UINT32 *)pRetParams);
						pRetParams = ((CHAR *)pRetParams) + 2;   					
						STREAM_TO_UINT16((CHAR *)pucReceivedParams,GET_SCAN_RESULTS_FRAME_TIME_OFFSET,*(UINT32 *)pRetParams);
						pRetParams = ((CHAR *)pRetParams) + 2;  
						memcpy((UINT8 *)pRetParams, (CHAR *)(pucReceivedParams + GET_SCAN_RESULTS_FRAME_TIME_OFFSET + 2), GET_SCAN_RESULTS_SSID_MAC_LENGTH);	
						break;

					case HCI_CMND_SIMPLE_LINK_START:
						break;

					case HCI_NETAPP_IPCONFIG:

						//Read IP address
						STREAM_TO_STREAM(RecvParams,RetParams,NETAPP_IPCONFIG_IP_LENGTH);
						RecvParams += 4;

						//Read subnet
						STREAM_TO_STREAM(RecvParams,RetParams,NETAPP_IPCONFIG_IP_LENGTH);
						RecvParams += 4;

						//Read default GW
						STREAM_TO_STREAM(RecvParams,RetParams,NETAPP_IPCONFIG_IP_LENGTH);
						RecvParams += 4;

						//Read DHCP server                                          	
						STREAM_TO_STREAM(RecvParams,RetParams,NETAPP_IPCONFIG_IP_LENGTH);
						RecvParams += 4;

						//Read DNS server                                           
						STREAM_TO_STREAM(RecvParams,RetParams,NETAPP_IPCONFIG_IP_LENGTH);
						RecvParams += 4;

						//Read Mac address                            	
						STREAM_TO_STREAM(RecvParams,RetParams,NETAPP_IPCONFIG_MAC_LENGTH);
						RecvParams += 6;

						//Read SSID
						STREAM_TO_STREAM(RecvParams,RetParams,NETAPP_IPCONFIG_SSID_LENGTH);

					}
				}

				if (usReceivedEventOpcode == tSLInformation.usRxEventOpcode)
				{
					tSLInformation.usRxEventOpcode = 0;
				}
			}
			else
			{				
				pucReceivedParams = pucReceivedData;
				STREAM_TO_UINT8((CHAR *)pucReceivedData, HCI_PACKET_ARGSIZE_OFFSET, ucArgsize);

				STREAM_TO_UINT16((CHAR *)pucReceivedData, HCI_PACKET_LENGTH_OFFSET, usLength);

				// Data received: note that the only case where from and from length 
				// are not null is in recv from, so fill the args accordingly
				if (from)
				{
					STREAM_TO_UINT32((CHAR *)(pucReceivedData + HCI_DATA_HEADER_SIZE), BSD_RECV_FROM_FROMLEN_OFFSET, *(UINT32 *)fromlen);
					memcpy(from, (pucReceivedData + HCI_DATA_HEADER_SIZE + BSD_RECV_FROM_FROM_OFFSET) ,*fromlen);
				}

				memcpy(pRetParams, pucReceivedParams + HCI_DATA_HEADER_SIZE + ucArgsize,
					usLength - ucArgsize);

				tSLInformation.usRxDataPending = 0;
			}

			tSLInformation.usEventOrDataReceived = 0;

			SpiResumeSpi();

			// Since we are going to TX - we need to handle this event after the 
			// ResumeSPi since we need interrupts
			if ((*pucReceivedData == HCI_TYPE_EVNT) &&
				(usReceivedEventOpcode == HCI_EVNT_PATCHES_REQ))
			{
				hci_unsol_handle_patch_request((CHAR *)pucReceivedData);
			}

			if ((tSLInformation.usRxEventOpcode == 0) && (tSLInformation.usRxDataPending == 0))
			{
				return NULL;
			}	
		}
	}

}

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
INT32 hci_unsol_event_handler(CHAR *event_hdr)
{
	CHAR * data = NULL;
	INT32 event_type;
	UINT32 NumberOfReleasedPackets;
	UINT32 NumberOfSentPackets;

	STREAM_TO_UINT16(event_hdr, HCI_EVENT_OPCODE_OFFSET,event_type);

	if (event_type & HCI_EVNT_UNSOL_BASE)
	{
		switch(event_type)
		{

		case HCI_EVNT_DATA_UNSOL_FREE_BUFF:
			{
				hci_event_unsol_flowcontrol_handler(event_hdr);

				NumberOfReleasedPackets = tSLInformation.NumberOfReleasedPackets;
				NumberOfSentPackets = tSLInformation.NumberOfSentPackets;

				if (NumberOfReleasedPackets == NumberOfSentPackets)
				{
					if (tSLInformation.InformHostOnTxComplete)
					{
						tSLInformation.sWlanCB(HCI_EVENT_CC3000_CAN_SHUT_DOWN, NULL, 0);
					}
				}				
				return 1;

			}
		}
	}

	if(event_type & HCI_EVNT_WLAN_UNSOL_BASE)
	{           
		switch(event_type)
		{
		case HCI_EVNT_WLAN_KEEPALIVE:
		case HCI_EVNT_WLAN_UNSOL_CONNECT:
		case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
		case HCI_EVNT_WLAN_UNSOL_INIT:
		case HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE:

			if( tSLInformation.sWlanCB )
			{
				tSLInformation.sWlanCB(event_type, 0, 0);
			}
			break;

		case HCI_EVNT_WLAN_UNSOL_DHCP:
			{
				UINT8	params[NETAPP_IPCONFIG_MAC_OFFSET + 1];	// extra byte is for the status
				UINT8 *recParams = params;

				data = (CHAR*)(event_hdr) + HCI_EVENT_HEADER_SIZE;

				//Read IP address
				STREAM_TO_STREAM(data,recParams,NETAPP_IPCONFIG_IP_LENGTH);
				data += 4;
				//Read subnet
				STREAM_TO_STREAM(data,recParams,NETAPP_IPCONFIG_IP_LENGTH);
				data += 4;
				//Read default GW
				STREAM_TO_STREAM(data,recParams,NETAPP_IPCONFIG_IP_LENGTH); 
				data += 4;
				//Read DHCP server  
				STREAM_TO_STREAM(data,recParams,NETAPP_IPCONFIG_IP_LENGTH);     
				data += 4;
				//Read DNS server  
				STREAM_TO_STREAM(data,recParams,NETAPP_IPCONFIG_IP_LENGTH); 
				// read the status
				STREAM_TO_UINT8(event_hdr, HCI_EVENT_STATUS_OFFSET, *recParams);


				if( tSLInformation.sWlanCB )
				{
					tSLInformation.sWlanCB(event_type, (CHAR *)params, sizeof(params));
				}
			}
			break;

		case HCI_EVNT_WLAN_ASYNC_PING_REPORT:
			{
				netapp_pingreport_args_t params;			
				data = (CHAR*)(event_hdr) + HCI_EVENT_HEADER_SIZE;			
				STREAM_TO_UINT32(data, NETAPP_PING_PACKETS_SENT_OFFSET, params.packets_sent);			
				STREAM_TO_UINT32(data, NETAPP_PING_PACKETS_RCVD_OFFSET, params.packets_received);			
				STREAM_TO_UINT32(data, NETAPP_PING_MIN_RTT_OFFSET, params.min_round_time);		
				STREAM_TO_UINT32(data, NETAPP_PING_MAX_RTT_OFFSET, params.max_round_time);	
				STREAM_TO_UINT32(data, NETAPP_PING_AVG_RTT_OFFSET, params.avg_round_time);

				if( tSLInformation.sWlanCB )
				{
					tSLInformation.sWlanCB(event_type, (CHAR *)&params, sizeof(params));
				}
			}
			break;
		case HCI_EVNT_BSD_TCP_CLOSE_WAIT:
			{
				data = (CHAR *)(event_hdr) + HCI_EVENT_HEADER_SIZE;
				if( tSLInformation.sWlanCB )
				{
					//data[0] represents the socket id, for which FIN was received by remote.
					//Upon receiving this event, the user can close the socket, or else the 
					//socket will be closded after inacvitity timeout (by default 60 seconds)
					tSLInformation.sWlanCB(event_type, data, 1);
				}
			}
			break;

			//'default' case which means "event not supported" 	
		default: 
			return (0);
		}
		return(1);
	}

	if ((event_type == HCI_EVNT_SEND) || (event_type == HCI_EVNT_SENDTO)
		|| (event_type == HCI_EVNT_WRITE))
	{
		CHAR *pArg;
		INT32 status;

		pArg = M_BSD_RESP_PARAMS_OFFSET(event_hdr);
		STREAM_TO_UINT32(pArg, BSD_RSP_PARAMS_STATUS_OFFSET,status);

		if (ERROR_SOCKET_INACTIVE == status)
		{
			// The only synchronous event that can come from SL device in form of 
			// command complete is "Command Complete" on data sent, in case SL device 
			// was unable to transmit
			STREAM_TO_UINT8(event_hdr, HCI_EVENT_STATUS_OFFSET, tSLInformation.slTransmitDataError);
			update_socket_active_status(M_BSD_RESP_PARAMS_OFFSET(event_hdr));

			return (1);
		}
		else
			return (0);
	}

	//handle a case where unsolicited event arrived, but was not handled by any of the cases above
	if ((event_type != tSLInformation.usRxEventOpcode) && (event_type != HCI_EVNT_PATCHES_REQ))
	{
		return(1);
	}

	return(0);
}

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
INT32 hci_unsolicited_event_handler(void)
{
	UINT32   res = 0;
	UINT8 *pucReceivedData;

	if (tSLInformation.usEventOrDataReceived != 0)
	{
		pucReceivedData = (tSLInformation.pucReceivedData);

		if (*pucReceivedData == HCI_TYPE_EVNT)
		{			

			// In case unsolicited event received - here the handling finished
			if (hci_unsol_event_handler((CHAR *)pucReceivedData) == 1)
			{

				// There was an unsolicited event received - we can release the buffer
				// and clean the event received 
				tSLInformation.usEventOrDataReceived = 0;

				res = 1;
				SpiResumeSpi();
			}
		}
	}

	return res;
}

//*****************************************************************************
//
//!  set_socket_active_status
//!
//!  @param Sd
//!	 @param Status
//!  @return         none
//!
//!  @brief          Check if the socket ID and status are valid and set 
//!                  accordingly  the global socket status
//
//*****************************************************************************
void set_socket_active_status(INT32 Sd, INT32 Status)
{
	if(M_IS_VALID_SD(Sd) && M_IS_VALID_STATUS(Status))
	{
		socket_active_status &= ~(1 << Sd);      /* clean socket's mask */
		socket_active_status |= (Status << Sd); /* set new socket's mask */
	}
}


//*****************************************************************************
//
//!  hci_event_unsol_flowcontrol_handler
//!
//!  @param  pEvent  pointer to the string contains parameters for IPERF
//!  @return         ESUCCESS if successful, EFAIL if an error occurred
//!
//!  @brief  Called in case unsolicited event from type
//!          HCI_EVNT_DATA_UNSOL_FREE_BUFF has received.
//!				   Keep track on the number of packets transmitted and update the
//!					 number of free buffer in the SL device.
//
//*****************************************************************************
INT32 hci_event_unsol_flowcontrol_handler(CHAR *pEvent)
{

	INT32 temp, value;
	UINT16 i;
	UINT16  pusNumberOfHandles=0;
	CHAR *pReadPayload;

	STREAM_TO_UINT16((CHAR *)pEvent,HCI_EVENT_HEADER_SIZE,pusNumberOfHandles);
	pReadPayload = ((CHAR *)pEvent +
		HCI_EVENT_HEADER_SIZE + sizeof(pusNumberOfHandles));	
	temp = 0;

	for(i = 0; i < pusNumberOfHandles ; i++)
	{
		STREAM_TO_UINT16(pReadPayload, FLOW_CONTROL_EVENT_FREE_BUFFS_OFFSET, value);
		temp += value;
		pReadPayload += FLOW_CONTROL_EVENT_SIZE;  
	}

	tSLInformation.usNumberOfFreeBuffers += temp;
	tSLInformation.NumberOfReleasedPackets += temp;

	return(ESUCCESS);
}

//*****************************************************************************
//
//!  get_socket_active_status
//!
//!  @param  Sd  Socket IS
//!  @return     Current status of the socket.   
//!
//!  @brief  Retrieve socket status
//
//*****************************************************************************

INT32 get_socket_active_status(INT32 Sd)
{
	if(M_IS_VALID_SD(Sd))
	{
		return (socket_active_status & (1 << Sd)) ? SOCKET_STATUS_INACTIVE : SOCKET_STATUS_ACTIVE;
	}
	return SOCKET_STATUS_INACTIVE;
}

//*****************************************************************************
//
//!  update_socket_active_status
//!
//!  @param  resp_params  Socket IS
//!  @return     Current status of the socket.   
//!
//!  @brief  Retrieve socket status
//
//*****************************************************************************
void update_socket_active_status(CHAR *resp_params)
{
	INT32 status, sd;

	STREAM_TO_UINT32(resp_params, BSD_RSP_PARAMS_SOCKET_OFFSET,sd);
	STREAM_TO_UINT32(resp_params, BSD_RSP_PARAMS_STATUS_OFFSET,status);

	if(ERROR_SOCKET_INACTIVE == status)
	{
		set_socket_active_status(sd, SOCKET_STATUS_INACTIVE);
	}
}


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

void SimpleLinkWaitEvent(UINT16 usOpcode, void *pRetParams)
{
	// In the blocking implementation the control to caller will be returned only 
	// after the end of current transaction
	tSLInformation.usRxEventOpcode = usOpcode;
	hci_event_handler(pRetParams, 0, 0);
}

//*****************************************************************************
//
//!  SimpleLinkWaitData
//!
//!  @param  pBuf       data buffer
//!  @param  from       from information
//!  @param  fromlen	from information length
//!
//!  @return               none
//!
//!  @brief                Wait for data, pass it to the hci_event_handler
//! 					   and update in a global variable that there is 
//!						   data to read.
//
//*****************************************************************************

void SimpleLinkWaitData(UINT8 *pBuf, UINT8 *from, UINT8 *fromlen)
{
	// In the blocking implementation the control to caller will be returned only 
	// after the end of current transaction, i.e. only after data will be received
	tSLInformation.usRxDataPending = 1;
	hci_event_handler(pBuf, from, fromlen);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
