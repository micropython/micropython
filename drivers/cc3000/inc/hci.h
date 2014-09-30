/*****************************************************************************
*
*  hci.h  - CC3000 Host Driver Implementation.
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
#ifndef __CC3000_HCI_H__
#define __CC3000_HCI_H__

#include "cc3000_common.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef  __cplusplus
extern "C" {
#endif


#define SPI_HEADER_SIZE 				   			(5)
#define SIMPLE_LINK_HCI_CMND_HEADER_SIZE 			(4)
#define HEADERS_SIZE_CMD        					(SPI_HEADER_SIZE + SIMPLE_LINK_HCI_CMND_HEADER_SIZE)
#define SIMPLE_LINK_HCI_DATA_CMND_HEADER_SIZE 		(5)
#define SIMPLE_LINK_HCI_DATA_HEADER_SIZE 			(5)
#define SIMPLE_LINK_HCI_PATCH_HEADER_SIZE 			(2)


//*****************************************************************************
//
// Values that can be used as HCI Commands and HCI Packet header defines
//
//*****************************************************************************
#define  HCI_TYPE_CMND          0x1
#define  HCI_TYPE_DATA          0x2
#define  HCI_TYPE_PATCH         0x3
#define  HCI_TYPE_EVNT          0x4


#define HCI_EVENT_PATCHES_DRV_REQ			(1)
#define HCI_EVENT_PATCHES_FW_REQ			(2)
#define HCI_EVENT_PATCHES_BOOTLOAD_REQ		(3)


#define  HCI_CMND_WLAN_BASE  (0x0000)
#define  HCI_CMND_WLAN_CONNECT  0x0001
#define  HCI_CMND_WLAN_DISCONNECT   0x0002
#define  HCI_CMND_WLAN_IOCTL_SET_SCANPARAM    0x0003
#define  HCI_CMND_WLAN_IOCTL_SET_CONNECTION_POLICY  0x0004
#define  HCI_CMND_WLAN_IOCTL_ADD_PROFILE  0x0005
#define  HCI_CMND_WLAN_IOCTL_DEL_PROFILE  0x0006
#define  HCI_CMND_WLAN_IOCTL_GET_SCAN_RESULTS  0x0007
#define  HCI_CMND_EVENT_MASK    0x0008
#define  HCI_CMND_WLAN_IOCTL_STATUSGET 0x0009
#define  HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_START        0x000A
#define  HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_STOP         0x000B
#define  HCI_CMND_WLAN_IOCTL_SIMPLE_CONFIG_SET_PREFIX   0x000C
#define  HCI_CMND_WLAN_CONFIGURE_PATCH					0x000D


#define  HCI_CMND_SOCKET_BASE   0x1000
#define  HCI_CMND_SOCKET        0x1001
#define  HCI_CMND_BIND          0x1002
#define  HCI_CMND_RECV          0x1004
#define  HCI_CMND_ACCEPT        0x1005
#define  HCI_CMND_LISTEN        0x1006
#define  HCI_CMND_CONNECT       0x1007
#define  HCI_CMND_BSD_SELECT 	0x1008
#define  HCI_CMND_SETSOCKOPT    0x1009
#define  HCI_CMND_GETSOCKOPT    0x100A
#define  HCI_CMND_CLOSE_SOCKET  0x100B
#define  HCI_CMND_RECVFROM      0x100D
#define  HCI_CMND_GETHOSTNAME   0x1010
#define  HCI_CMND_MDNS_ADVERTISE	   0x1011
#define  HCI_CMND_GETMSSVALUE		0x1012


#define HCI_DATA_BASE								0x80

#define HCI_CMND_SEND           					(0x01 + HCI_DATA_BASE)
#define HCI_CMND_SENDTO        						(0x03 + HCI_DATA_BASE)
#define HCI_DATA_BSD_RECVFROM						(0x04 + HCI_DATA_BASE)
#define HCI_DATA_BSD_RECV							(0x05 + HCI_DATA_BASE)


#define HCI_CMND_NVMEM_CBASE		(0x0200)


#define HCI_CMND_NVMEM_CREATE_ENTRY (0x0203)
#define HCI_CMND_NVMEM_SWAP_ENTRY  	(0x0205)
#define HCI_CMND_NVMEM_READ    		(0x0201)
#define HCI_CMND_NVMEM_WRITE   		(0x0090)
#define HCI_CMND_NVMEM_WRITE_PATCH	(0x0204)
#define HCI_CMND_READ_SP_VERSION  	(0x0207)

#define  HCI_CMND_READ_BUFFER_SIZE  0x400B
#define  HCI_CMND_SIMPLE_LINK_START 0x4000

#define HCI_CMND_NETAPP_BASE		0x2000

#define HCI_NETAPP_DHCP				    (0x0001 + HCI_CMND_NETAPP_BASE)
#define HCI_NETAPP_PING_SEND                        (0x0002 + HCI_CMND_NETAPP_BASE)
#define HCI_NETAPP_PING_REPORT                      (0x0003 + HCI_CMND_NETAPP_BASE)
#define HCI_NETAPP_PING_STOP                        (0x0004 + HCI_CMND_NETAPP_BASE)
#define HCI_NETAPP_IPCONFIG                         (0x0005 + HCI_CMND_NETAPP_BASE)
#define HCI_NETAPP_ARP_FLUSH			    (0x0006 + HCI_CMND_NETAPP_BASE)
#define HCI_NETAPP_SET_DEBUG_LEVEL					(0x0008 + HCI_CMND_NETAPP_BASE)
#define HCI_NETAPP_SET_TIMERS						(0x0009 + HCI_CMND_NETAPP_BASE)






//*****************************************************************************
//
// Values that can be used as HCI Events defines
//
//*****************************************************************************
#define  HCI_EVNT_WLAN_BASE     0x0000
#define  HCI_EVNT_WLAN_CONNECT  0x0001
#define  HCI_EVNT_WLAN_DISCONNECT \
                                0x0002
#define  HCI_EVNT_WLAN_IOCTL_ADD_PROFILE  \
                                0x0005


#define  HCI_EVNT_SOCKET              HCI_CMND_SOCKET
#define  HCI_EVNT_BIND                HCI_CMND_BIND
#define  HCI_EVNT_RECV                HCI_CMND_RECV
#define  HCI_EVNT_ACCEPT              HCI_CMND_ACCEPT
#define  HCI_EVNT_LISTEN              HCI_CMND_LISTEN
#define  HCI_EVNT_CONNECT             HCI_CMND_CONNECT
#define  HCI_EVNT_SELECT              HCI_CMND_BSD_SELECT
#define  HCI_EVNT_CLOSE_SOCKET        HCI_CMND_CLOSE_SOCKET
#define  HCI_EVNT_RECVFROM            HCI_CMND_RECVFROM
#define  HCI_EVNT_SETSOCKOPT          HCI_CMND_SETSOCKOPT
#define  HCI_EVNT_GETSOCKOPT          HCI_CMND_GETSOCKOPT
#define  HCI_EVNT_BSD_GETHOSTBYNAME   HCI_CMND_GETHOSTNAME
#define  HCI_EVNT_MDNS_ADVERTISE   HCI_CMND_MDNS_ADVERTISE
#define  HCI_EVNT_GETMSSVALUE		HCI_CMND_GETMSSVALUE	

#define  HCI_EVNT_SEND          0x1003
#define  HCI_EVNT_WRITE         0x100E
#define  HCI_EVNT_SENDTO        0x100F

#define HCI_EVNT_PATCHES_REQ    0x1000

#define HCI_EVNT_UNSOL_BASE    0x4000

#define HCI_EVNT_WLAN_UNSOL_BASE     (0x8000)

#define HCI_EVNT_WLAN_UNSOL_CONNECT  	 (0x0001 + HCI_EVNT_WLAN_UNSOL_BASE)
#define HCI_EVNT_WLAN_UNSOL_DISCONNECT   (0x0002 + HCI_EVNT_WLAN_UNSOL_BASE)
#define HCI_EVNT_WLAN_UNSOL_INIT         (0x0004 + HCI_EVNT_WLAN_UNSOL_BASE)
#define HCI_EVNT_WLAN_TX_COMPLETE         (0x0008 + HCI_EVNT_WLAN_UNSOL_BASE)
#define HCI_EVNT_WLAN_UNSOL_DHCP         (0x0010 + HCI_EVNT_WLAN_UNSOL_BASE)
#define HCI_EVNT_WLAN_ASYNC_PING_REPORT  (0x0040 + HCI_EVNT_WLAN_UNSOL_BASE)
#define HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE  (0x0080 + HCI_EVNT_WLAN_UNSOL_BASE)
#define HCI_EVNT_WLAN_KEEPALIVE			 (0x0200  + HCI_EVNT_WLAN_UNSOL_BASE)
#define	HCI_EVNT_BSD_TCP_CLOSE_WAIT      (0x0800 + HCI_EVNT_WLAN_UNSOL_BASE)

#define HCI_EVNT_DATA_UNSOL_FREE_BUFF \
                                0x4100

#define HCI_EVNT_NVMEM_CREATE_ENTRY \
                                HCI_CMND_NVMEM_CREATE_ENTRY
#define HCI_EVNT_NVMEM_SWAP_ENTRY HCI_CMND_NVMEM_SWAP_ENTRY

#define HCI_EVNT_NVMEM_READ     HCI_CMND_NVMEM_READ
#define HCI_EVNT_NVMEM_WRITE    (0x0202)

#define HCI_EVNT_READ_SP_VERSION  	\
				HCI_CMND_READ_SP_VERSION

#define  HCI_EVNT_INPROGRESS    0xFFFF


#define HCI_DATA_RECVFROM       0x84
#define HCI_DATA_RECV           0x85
#define HCI_DATA_NVMEM          0x91

#define HCI_EVENT_CC3000_CAN_SHUT_DOWN 0x99

//*****************************************************************************
//
// Prototypes for the structures for APIs.
//
//*****************************************************************************

#define HCI_DATA_HEADER_SIZE		(5)
#define HCI_EVENT_HEADER_SIZE		(5)
#define HCI_DATA_CMD_HEADER_SIZE	(5)
#define HCI_PATCH_HEADER_SIZE		(6)

#define HCI_PACKET_TYPE_OFFSET		(0)
#define HCI_PACKET_ARGSIZE_OFFSET	(2)
#define HCI_PACKET_LENGTH_OFFSET	(3)


#define HCI_EVENT_OPCODE_OFFSET (1)
#define HCI_EVENT_LENGTH_OFFSET	(3)
#define HCI_EVENT_STATUS_OFFSET	(4)
#define HCI_DATA_LENGTH_OFFSET	(3)
  
  


//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//!  hci_command_send
//!
//!  @param  usOpcode     command operation code
//!  @param  pucBuff      pointer to the command's arguments buffer
//!  @param  ucArgsLength length of the arguments
//!
//!  @return              none
//!
//!  @brief               Initiate an HCI command.
//
//*****************************************************************************
extern UINT16 hci_command_send(UINT16 usOpcode, 
                                   UINT8 *ucArgs,
                                   UINT8 ucArgsLength);
 

//*****************************************************************************
//
//!  hci_data_send
//!
//!  @param  usOpcode        command operation code
//!	 @param  ucArgs					 pointer to the command's arguments buffer
//!  @param  usArgsLength    length of the arguments
//!  @param  ucTail          pointer to the data buffer
//!  @param  usTailLength    buffer length
//!
//!  @return none
//!
//!  @brief              Initiate an HCI data write operation
//
//*****************************************************************************
extern INT32 hci_data_send(UINT8 ucOpcode,
                                      UINT8 *ucArgs,
                                      UINT16 usArgsLength,
                                      UINT16 usDataLength,
                                      const UINT8 *ucTail,
                                      UINT16 usTailLength);


//*****************************************************************************
//
//!  hci_data_command_send
//!
//!  @param  usOpcode      command operation code
//!  @param  pucBuff       pointer to the data buffer
//!  @param  ucArgsLength  arguments length
//!  @param  ucDataLength  data length
//!
//!  @return none
//!
//!  @brief              Prepare HCI header and initiate an HCI data write operation
//
//*****************************************************************************
extern void hci_data_command_send(UINT16 usOpcode, UINT8 *pucBuff,
                     UINT8 ucArgsLength, UINT16 ucDataLength);

//*****************************************************************************
//
//!  hci_patch_send
//!
//!  @param  usOpcode      command operation code
//!  @param  pucBuff       pointer to the command's arguments buffer
//!  @param  patch         pointer to patch content buffer 
//!  @param  usDataLength  data length
//!
//!  @return              none
//!
//!  @brief               Prepare HCI header and initiate an HCI patch write operation
//
//*****************************************************************************
extern void hci_patch_send(UINT8 ucOpcode, UINT8 *pucBuff, CHAR *patch, UINT16 usDataLength);



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __CC3000_HCI_H__
