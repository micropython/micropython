/*
 * FreeRTOS+TCP V2.0.10
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef FREERTOS_IP_PRIVATE_H
#define FREERTOS_IP_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Application level configuration options. */
#include "FreeRTOSIPConfig.h"
#include "FreeRTOSIPConfigDefaults.h"
#include "FreeRTOS_Sockets.h"
#include "IPTraceMacroDefaults.h"
#include "FreeRTOS_Stream_Buffer.h"
#if( ipconfigUSE_TCP == 1 )
	#include "FreeRTOS_TCP_WIN.h"
	#include "FreeRTOS_TCP_IP.h"
#endif

#include "event_groups.h"

typedef struct xNetworkAddressingParameters
{
	uint32_t ulDefaultIPAddress;
	uint32_t ulNetMask;
	uint32_t ulGatewayAddress;
	uint32_t ulDNSServerAddress;
	uint32_t ulBroadcastAddress;
} NetworkAddressingParameters_t;

extern BaseType_t xTCPWindowLoggingLevel;

/*-----------------------------------------------------------*/
/* Protocol headers.                                         */
/*-----------------------------------------------------------*/

#include "pack_struct_start.h"
struct xETH_HEADER
{
	MACAddress_t xDestinationAddress; /*  0 + 6 = 6  */
	MACAddress_t xSourceAddress;      /*  6 + 6 = 12 */
	uint16_t usFrameType;              /* 12 + 2 = 14 */
}
#include "pack_struct_end.h"
typedef struct xETH_HEADER EthernetHeader_t;

#include "pack_struct_start.h"
struct xARP_HEADER
{
	uint16_t usHardwareType;				/*  0 +  2 =  2 */
	uint16_t usProtocolType;				/*  2 +  2 =  4 */
	uint8_t ucHardwareAddressLength;		/*  4 +  1 =  5 */
	uint8_t ucProtocolAddressLength;		/*  5 +  1 =  6 */
	uint16_t usOperation;					/*  6 +  2 =  8 */
	MACAddress_t xSenderHardwareAddress;	/*  8 +  6 = 14 */
	uint8_t ucSenderProtocolAddress[ 4 ];	/* 14 +  4 = 18  */
	MACAddress_t xTargetHardwareAddress;	/* 18 +  6 = 24  */
	uint32_t ulTargetProtocolAddress;		/* 24 +  4 = 28  */
}
#include "pack_struct_end.h"
typedef struct xARP_HEADER ARPHeader_t;

#include "pack_struct_start.h"
struct xIP_HEADER
{
	uint8_t ucVersionHeaderLength;        /*  0 + 1 =  1 */
	uint8_t ucDifferentiatedServicesCode; /*  1 + 1 =  2 */
	uint16_t usLength;                    /*  2 + 2 =  4 */
	uint16_t usIdentification;            /*  4 + 2 =  6 */
	uint16_t usFragmentOffset;            /*  6 + 2 =  8 */
	uint8_t ucTimeToLive;                 /*  8 + 1 =  9 */
	uint8_t ucProtocol;                   /*  9 + 1 = 10 */
	uint16_t usHeaderChecksum;            /* 10 + 2 = 12 */
	uint32_t ulSourceIPAddress;           /* 12 + 4 = 16 */
	uint32_t ulDestinationIPAddress;      /* 16 + 4 = 20 */
}
#include "pack_struct_end.h"
typedef struct xIP_HEADER IPHeader_t;

#include "pack_struct_start.h"
struct xIGMP_HEADER
{
	uint8_t ucVersionType;     /* 0 + 1 = 1 */
	uint8_t ucMaxResponseTime; /* 1 + 1 = 2 */
	uint16_t usChecksum;       /* 2 + 2 = 4 */
	uint32_t usGroupAddress;   /* 4 + 4 = 8 */
}
#include "pack_struct_end.h"
typedef struct xIGMP_HEADER IGMPHeader_t;

#include "pack_struct_start.h"
struct xICMP_HEADER
{
	uint8_t ucTypeOfMessage;   /* 0 + 1 = 1 */
	uint8_t ucTypeOfService;   /* 1 + 1 = 2 */
	uint16_t usChecksum;       /* 2 + 2 = 4 */
	uint16_t usIdentifier;     /* 4 + 2 = 6 */
	uint16_t usSequenceNumber; /* 6 + 2 = 8 */
}
#include "pack_struct_end.h"
typedef struct xICMP_HEADER ICMPHeader_t;

#include "pack_struct_start.h"
struct xUDP_HEADER
{
	uint16_t usSourcePort;      /* 0 + 2 = 2 */
	uint16_t usDestinationPort; /* 2 + 2 = 4 */
	uint16_t usLength;          /* 4 + 2 = 6 */
	uint16_t usChecksum;        /* 6 + 2 = 8 */
}
#include "pack_struct_end.h"
typedef struct xUDP_HEADER UDPHeader_t;

#include "pack_struct_start.h"
struct xTCP_HEADER
{
	uint16_t usSourcePort;		/* +  2 =  2 */
	uint16_t usDestinationPort;	/* +  2 =  4 */
	uint32_t ulSequenceNumber;	/* +  4 =  8 */
	uint32_t ulAckNr;   	 	/* +  4 = 12 */
	uint8_t  ucTCPOffset;		/* +  1 = 13 */
	uint8_t  ucTCPFlags;		/* +  1 = 14 */
	uint16_t usWindow;			/* +  2 = 15 */
	uint16_t usChecksum;		/* +  2 = 18 */
	uint16_t usUrgent;			/* +  2 = 20 */
#if ipconfigUSE_TCP == 1
	/* the option data is not a part of the TCP header */
	uint8_t  ucOptdata[ipSIZE_TCP_OPTIONS];		/* + 12 = 32 */
#endif
}
#include "pack_struct_end.h"
typedef struct xTCP_HEADER TCPHeader_t;

#include "pack_struct_start.h"
struct xPSEUDO_HEADER
{
	uint32_t ulSourceAddress;
	uint32_t ulDestinationAddress;
	uint8_t ucZeros;
	uint8_t ucProtocol;
	uint16_t usUDPLength;
}
#include "pack_struct_end.h"
typedef struct xPSEUDO_HEADER PseudoHeader_t;

/*-----------------------------------------------------------*/
/* Nested protocol packets.                                  */
/*-----------------------------------------------------------*/

#include "pack_struct_start.h"
struct xARP_PACKET
{
	EthernetHeader_t xEthernetHeader;	/*  0 + 14 = 14 */
	ARPHeader_t xARPHeader;			/* 14 + 28 = 42 */
}
#include "pack_struct_end.h"
typedef struct xARP_PACKET ARPPacket_t;

#include "pack_struct_start.h"
struct xIP_PACKET
{
	EthernetHeader_t xEthernetHeader;
	IPHeader_t xIPHeader;
}
#include "pack_struct_end.h"
typedef struct xIP_PACKET IPPacket_t;

#include "pack_struct_start.h"
struct xICMP_PACKET
{
	EthernetHeader_t xEthernetHeader;
	IPHeader_t xIPHeader;
	ICMPHeader_t xICMPHeader;
}
#include "pack_struct_end.h"
typedef struct xICMP_PACKET ICMPPacket_t;

#include "pack_struct_start.h"
struct xUDP_PACKET
{
	EthernetHeader_t xEthernetHeader; /*  0 + 14 = 14 */
	IPHeader_t xIPHeader;             /* 14 + 20 = 34 */
	UDPHeader_t xUDPHeader;           /* 34 +  8 = 42 */
}
#include "pack_struct_end.h"
typedef struct xUDP_PACKET UDPPacket_t;

#include "pack_struct_start.h"
struct xTCP_PACKET
{
	EthernetHeader_t xEthernetHeader; /*  0 + 14 = 14 */
	IPHeader_t xIPHeader;             /* 14 + 20 = 34 */
	TCPHeader_t xTCPHeader;           /* 34 + 32 = 66 */
}
#include "pack_struct_end.h"
typedef struct xTCP_PACKET TCPPacket_t;

typedef union XPROT_PACKET
{
	ARPPacket_t xARPPacket;
	TCPPacket_t xTCPPacket;
	UDPPacket_t xUDPPacket;
	ICMPPacket_t xICMPPacket;
} ProtocolPacket_t;


/* The maximum UDP payload length. */
#define ipMAX_UDP_PAYLOAD_LENGTH ( ( ipconfigNETWORK_MTU - ipSIZE_OF_IPv4_HEADER ) - ipSIZE_OF_UDP_HEADER )

typedef enum
{
	eReleaseBuffer = 0,		/* Processing the frame did not find anything to do - just release the buffer. */
	eProcessBuffer,			/* An Ethernet frame has a valid address - continue process its contents. */
	eReturnEthernetFrame,	/* The Ethernet frame contains an ARP or ICMP packet that can be returned to its source. */
	eFrameConsumed			/* Processing the Ethernet packet contents resulted in the payload being sent to the stack. */
} eFrameProcessingResult_t;

typedef enum
{
	eNoEvent = -1,
	eNetworkDownEvent,		/* 0: The network interface has been lost and/or needs [re]connecting. */
	eNetworkRxEvent,		/* 1: The network interface has queued a received Ethernet frame. */
	eARPTimerEvent,			/* 2: The ARP timer expired. */
	eStackTxEvent,			/* 3: The software stack has queued a packet to transmit. */
	eDHCPEvent,				/* 4: Process the DHCP state machine. */
	eTCPTimerEvent,			/* 5: See if any TCP socket needs attention. */
	eTCPAcceptEvent,		/* 6: Client API FreeRTOS_accept() waiting for client connections. */
	eTCPNetStat,			/* 7: IP-task is asked to produce a netstat listing. */
	eSocketBindEvent,		/* 8: Send a message to the IP-task to bind a socket to a port. */
	eSocketCloseEvent,		/* 9: Send a message to the IP-task to close a socket. */
	eSocketSelectEvent,		/*10: Send a message to the IP-task for select(). */
	eSocketSignalEvent,		/*11: A socket must be signalled. */
} eIPEvent_t;

typedef struct IP_TASK_COMMANDS
{
	eIPEvent_t eEventType;
	void *pvData;
} IPStackEvent_t;

#define ipBROADCAST_IP_ADDRESS 0xffffffffUL

/* Offset into the Ethernet frame that is used to temporarily store information
on the fragmentation status of the packet being sent.  The value is important,
as it is past the location into which the destination address will get placed. */
#define ipFRAGMENTATION_PARAMETERS_OFFSET		( 6 )
#define ipSOCKET_OPTIONS_OFFSET					( 6 )

/* Only used when outgoing fragmentation is being used (FreeRTOSIPConfig.h
setting. */
#define ipGET_UDP_PAYLOAD_OFFSET_FOR_FRAGMENT( usFragmentOffset ) ( ( ( usFragmentOffset ) == 0 ) ? ipUDP_PAYLOAD_OFFSET_IPv4 : ipIP_PAYLOAD_OFFSET )

/* The offset into a UDP packet at which the UDP data (payload) starts. */
#define ipUDP_PAYLOAD_OFFSET_IPv4	( sizeof( UDPPacket_t ) )

/* The offset into an IP packet into which the IP data (payload) starts. */
#define ipIP_PAYLOAD_OFFSET		( sizeof( IPPacket_t ) )

#include "pack_struct_start.h"
struct xUDP_IP_FRAGMENT_PARAMETERS
{
	uint8_t ucSocketOptions;
	uint8_t ucPadFor16BitAlignment;
	uint16_t usFragmentedPacketOffset;
	uint16_t usFragmentLength;
	uint16_t usPayloadChecksum;
}
#include "pack_struct_end.h"
typedef struct xUDP_IP_FRAGMENT_PARAMETERS IPFragmentParameters_t;

#if( ipconfigBYTE_ORDER == pdFREERTOS_LITTLE_ENDIAN )

	/* Ethernet frame types. */
	#define ipARP_FRAME_TYPE	( 0x0608U )
	#define ipIPv4_FRAME_TYPE	( 0x0008U )

	/* ARP related definitions. */
	#define ipARP_PROTOCOL_TYPE				( 0x0008U )
	#define ipARP_HARDWARE_TYPE_ETHERNET	( 0x0100U )
	#define ipARP_REQUEST					( 0x0100U )
	#define ipARP_REPLY						( 0x0200U )

#else

	/* Ethernet frame types. */
	#define ipARP_FRAME_TYPE	( 0x0806U )
	#define ipIPv4_FRAME_TYPE	( 0x0800U )

	/* ARP related definitions. */
	#define ipARP_PROTOCOL_TYPE ( 0x0800U )
	#define ipARP_HARDWARE_TYPE_ETHERNET ( 0x0001U )
	#define ipARP_REQUEST ( 0x0001 )
	#define ipARP_REPLY ( 0x0002 )

#endif /* ipconfigBYTE_ORDER == pdFREERTOS_LITTLE_ENDIAN */


/* For convenience, a MAC address of all zeros and another of all 0xffs are
defined const for quick reference. */
extern const MACAddress_t xBroadcastMACAddress; /* all 0xff's */
extern uint16_t usPacketIdentifier;

/* Define a default UDP packet header (declared in FreeRTOS_UDP_IP.c) */
typedef union xUDPPacketHeader
{
	uint8_t ucBytes[24];
	uint32_t ulWords[6];
} UDPPacketHeader_t;
extern UDPPacketHeader_t xDefaultPartUDPPacketHeader;

/* Structure that stores the netmask, gateway address and DNS server addresses. */
extern NetworkAddressingParameters_t xNetworkAddressing;

/* Structure that stores the defaults for netmask, gateway address and DNS.
These values will be copied to 'xNetworkAddressing' in case DHCP is not used,
and also in case DHCP does not lead to a confirmed request. */
extern NetworkAddressingParameters_t xDefaultAddressing;

/* True when BufferAllocation_1.c was included, false for BufferAllocation_2.c */
extern const BaseType_t xBufferAllocFixedSize;

/* Defined in FreeRTOS_Sockets.c */
#if ( ipconfigUSE_TCP == 1 )
	extern List_t xBoundTCPSocketsList;
#endif

/* The local IP address is accessed from within xDefaultPartUDPPacketHeader,
rather than duplicated in its own variable. */
#define ipLOCAL_IP_ADDRESS_POINTER ( ( uint32_t * ) &( xDefaultPartUDPPacketHeader.ulWords[ 20u / sizeof(uint32_t) ] ) )

/* The local MAC address is accessed from within xDefaultPartUDPPacketHeader,
rather than duplicated in its own variable. */
#define ipLOCAL_MAC_ADDRESS ( &xDefaultPartUDPPacketHeader.ucBytes[0] )

/* ICMP packets are sent using the same function as UDP packets.  The port
number is used to distinguish between the two, as 0 is an invalid UDP port. */
#define ipPACKET_CONTAINS_ICMP_DATA					( 0 )

/* For now, the lower 8 bits in 'xEventBits' will be reserved for the above
socket events. */
#define SOCKET_EVENT_BIT_COUNT   8

#define vSetField16( pxBase, xType, xField, usValue ) \
{ \
	( ( uint8_t* )( pxBase ) ) [ offsetof( xType, xField ) + 0 ] = ( uint8_t ) ( ( usValue ) >> 8 ); \
	( ( uint8_t* )( pxBase ) ) [ offsetof( xType, xField ) + 1 ] = ( uint8_t ) ( ( usValue ) & 0xff ); \
}

#define vSetField32( pxBase, xType, xField, ulValue ) \
{ \
	( (uint8_t*)( pxBase ) ) [ offsetof( xType, xField ) + 0 ] = ( uint8_t )   ( ( ulValue ) >> 24 ); \
	( (uint8_t*)( pxBase ) ) [ offsetof( xType, xField ) + 1 ] = ( uint8_t ) ( ( ( ulValue ) >> 16 ) & 0xff ); \
	( (uint8_t*)( pxBase ) ) [ offsetof( xType, xField ) + 2 ] = ( uint8_t ) ( ( ( ulValue ) >> 8 ) & 0xff ); \
	( (uint8_t*)( pxBase ) ) [ offsetof( xType, xField ) + 3 ] = ( uint8_t )   ( ( ulValue ) & 0xff ); \
}

#define vFlip_16( left, right ) \
	do { \
		uint16_t tmp = (left); \
		(left) = (right); \
		(right) = tmp; \
	} while (0)

#define vFlip_32( left, right ) \
	do { \
		uint32_t tmp = (left); \
		(left) = (right); \
		(right) = tmp; \
	} while (0)

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(x)	(BaseType_t)(sizeof(x)/sizeof(x)[0])
#endif

/*
 * A version of FreeRTOS_GetReleaseNetworkBuffer() that can be called from an
 * interrupt.  If a non zero value is returned, then the calling ISR should
 * perform a context switch before exiting the ISR.
 */
BaseType_t FreeRTOS_ReleaseFreeNetworkBufferFromISR( void );

/*
 * Create a message that contains a command to initialise the network interface.
 * This is used during initialisation, and at any time the network interface
 * goes down thereafter.  The network interface hardware driver is responsible
 * for sending the message that contains the network interface down command/
 * event.
 *
 * Only use the FreeRTOS_NetworkDownFromISR() version if the function is to be
 * called from an interrupt service routine.  If FreeRTOS_NetworkDownFromISR()
 * returns a non-zero value then a context switch should be performed ebfore
 * the interrupt is exited.
 */
void FreeRTOS_NetworkDown( void );
BaseType_t FreeRTOS_NetworkDownFromISR( void );

/*
 * Processes incoming ARP packets.
 */
eFrameProcessingResult_t eARPProcessPacket( ARPPacket_t * const pxARPFrame );

/*
 * Inspect an Ethernet frame to see if it contains data that the stack needs to
 * process.  eProcessBuffer is returned if the frame should be processed by the
 * stack.  eReleaseBuffer is returned if the frame should be discarded.
 */
eFrameProcessingResult_t eConsiderFrameForProcessing( const uint8_t * const pucEthernetBuffer );

/*
 * Return the checksum generated over xDataLengthBytes from pucNextData.
 */
uint16_t usGenerateChecksum( uint32_t ulSum, const uint8_t * pucNextData, size_t uxDataLengthBytes );

/* Socket related private functions. */

/* 
 * The caller must ensure that pxNetworkBuffer->xDataLength is the UDP packet 
 * payload size (excluding packet headers) and that the packet in pucEthernetBuffer 
 * is at least the size of UDPPacket_t. 
 */
BaseType_t xProcessReceivedUDPPacket( NetworkBufferDescriptor_t *pxNetworkBuffer, uint16_t usPort );

/*
 * Initialize the socket list data structures for TCP and UDP. 
 */
BaseType_t vNetworkSocketsInit( void );

/*
 * Returns pdTRUE if the IP task has been created and is initialised.  Otherwise
 * returns pdFALSE.
 */
BaseType_t xIPIsNetworkTaskReady( void );

#if( ipconfigSOCKET_HAS_USER_WAKE_CALLBACK == 1 )
	struct XSOCKET;
	typedef void (*SocketWakeupCallback_t)( struct XSOCKET * pxSocket );
#endif

#if( ipconfigUSE_TCP == 1 )

	/*
	 * Actually a user thing, but because xBoundTCPSocketsList, let it do by the
	 * IP-task
	 */
	void vTCPNetStat( void );

	/*
	 * At least one socket needs to check for timeouts
	 */
	TickType_t xTCPTimerCheck( BaseType_t xWillSleep );

	/* Every TCP socket has a buffer space just big enough to store
	the last TCP header received.
	As a reference of this field may be passed to DMA, force the
	alignment to 8 bytes. */
	typedef union
	{
		struct
		{
			/* Increase the alignment of this union by adding a 64-bit variable. */
			uint64_t ullAlignmentWord;
		} a;
		struct
		{
			/* The next field only serves to give 'ucLastPacket' a correct
			alignment of 8 + 2.  See comments in FreeRTOS_IP.h */
			uint8_t ucFillPacket[ ipconfigPACKET_FILLER_SIZE ];
			uint8_t ucLastPacket[ sizeof( TCPPacket_t ) ];
		} u;
	} LastTCPPacket_t;

	/*
	 * Note that the values of all short and long integers in these structs
	 * are being stored in the native-endian way
	 * Translation should take place when accessing any structure which defines
	 * network packets, such as IPHeader_t and TCPHeader_t
	 */
	typedef struct TCPSOCKET
	{
		uint32_t ulRemoteIP;		/* IP address of remote machine */
		uint16_t usRemotePort;		/* Port on remote machine */
		struct {
			/* Most compilers do like bit-flags */
			uint32_t
				bMssChange : 1,		/* This socket has seen a change in MSS */
				bPassAccept : 1,	/* when true, this socket may be returned in a call to accept() */
				bPassQueued : 1,	/* when true, this socket is an orphan until it gets connected
									 * Why an orphan? Because it may not be returned in a accept() call until it
									 * gets the state eESTABLISHED */
				bReuseSocket : 1,	/* When a listening socket gets a connection, do not create a new instance but keep on using it */
				bCloseAfterSend : 1,/* As soon as the last byte has been transmitted, finalise the connection
									 * Useful in e.g. FTP connections, where the last data bytes are sent along with the FIN flag */
				bUserShutdown : 1,	/* User requesting a graceful shutdown */
				bCloseRequested : 1,/* Request to finalise the connection */
				bLowWater : 1,		/* high-water level has been reached. Cleared as soon as 'rx-count < lo-water' */
				bWinChange : 1,		/* The value of bLowWater has changed, must send a window update */
				bSendKeepAlive : 1,	/* When this flag is true, a TCP keep-alive message must be send */
				bWaitKeepAlive : 1,	/* When this flag is true, a TCP keep-alive reply is expected */
				bConnPrepared : 1,	/* Connecting socket: Message has been prepared */
				#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
					bConnPassed : 1,	/* Connecting socket: Socket has been passed in a successful select()  */
				#endif /* ipconfigSUPPORT_SELECT_FUNCTION */
				bFinAccepted : 1,	/* This socket has received (or sent) a FIN and accepted it */
				bFinSent : 1,		/* We've sent out a FIN */
				bFinRecv : 1,		/* We've received a FIN from our peer */
				bFinAcked : 1,		/* Our FIN packet has been acked */
				bFinLast : 1,		/* The last ACK (after FIN and FIN+ACK) has been sent or will be sent by the peer */
				bRxStopped : 1,		/* Application asked to temporarily stop reception */
				bMallocError : 1,	/* There was an error allocating a stream */
				bWinScaling : 1;	/* A TCP-Window Scaling option was offered and accepted in the SYN phase. */
		} bits;
		uint32_t ulHighestRxAllowed;
								/* The highest sequence number that we can receive at any moment */
		uint16_t usTimeout;		/* Time (in ticks) after which this socket needs attention */
		uint16_t usCurMSS;		/* Current Maximum Segment Size */
		uint16_t usInitMSS;		/* Initial maximum segment Size */
		uint16_t usChildCount;	/* In case of a listening socket: number of connections on this port number */
		uint16_t usBacklog;		/* In case of a listening socket: maximum number of concurrent connections on this port number */
		uint8_t ucRepCount;		/* Send repeat count, for retransmissions
								 * This counter is separate from the xmitCount in the
								 * TCP win segments */
		uint8_t ucTCPState;		/* TCP state: see eTCP_STATE */
		struct XSOCKET *pxPeerSocket;	/* for server socket: child, for child socket: parent */
		#if( ipconfigTCP_KEEP_ALIVE == 1 )
			uint8_t ucKeepRepCount;
			TickType_t xLastAliveTime;
		#endif /* ipconfigTCP_KEEP_ALIVE */
		#if( ipconfigTCP_HANG_PROTECTION == 1 )
			TickType_t xLastActTime;
		#endif /* ipconfigTCP_HANG_PROTECTION */
		size_t uxLittleSpace;
		size_t uxEnoughSpace;
		size_t uxRxStreamSize;
		size_t uxTxStreamSize;
		StreamBuffer_t *rxStream;
		StreamBuffer_t *txStream;
		#if( ipconfigUSE_TCP_WIN == 1 )
			NetworkBufferDescriptor_t *pxAckMessage;
		#endif /* ipconfigUSE_TCP_WIN */
		/* Buffer space to store the last TCP header received. */
		LastTCPPacket_t xPacket;
		uint8_t tcpflags;		/* TCP flags */
		#if( ipconfigUSE_TCP_WIN != 0 )
			uint8_t ucMyWinScaleFactor;
			uint8_t ucPeerWinScaleFactor;
		#endif
		#if( ipconfigUSE_CALLBACKS == 1 )
			FOnTCPReceive_t pxHandleReceive;	/*
										 		 * In case of a TCP socket:
										 		 * typedef void (* FOnTCPReceive_t) (Socket_t xSocket, void *pData, size_t xLength );
										 		 */
			FOnTCPSent_t pxHandleSent;
			FOnConnected_t pxHandleConnected;	/* Actually type: typedef void (* FOnConnected_t) (Socket_t xSocket, BaseType_t ulConnected ); */
		#endif /* ipconfigUSE_CALLBACKS */
		uint32_t ulWindowSize;		/* Current Window size advertised by peer */
		uint32_t ulRxCurWinSize;	/* Constantly changing: this is the current size available for data reception */
		size_t uxRxWinSize;	/* Fixed value: size of the TCP reception window */
		size_t uxTxWinSize;	/* Fixed value: size of the TCP transmit window */

		TCPWindow_t xTCPWindow;
	} IPTCPSocket_t;

#endif /* ipconfigUSE_TCP */

typedef struct UDPSOCKET
{
	List_t xWaitingPacketsList;	/* Incoming packets */
	#if( ipconfigUDP_MAX_RX_PACKETS > 0 )
		UBaseType_t uxMaxPackets; /* Protection: limits the number of packets buffered per socket */
	#endif /* ipconfigUDP_MAX_RX_PACKETS */
	#if( ipconfigUSE_CALLBACKS == 1 )
		FOnUDPReceive_t pxHandleReceive;	/*
											 * In case of a UDP socket:
											 * typedef void (* FOnUDPReceive_t) (Socket_t xSocket, void *pData, size_t xLength, struct freertos_sockaddr *pxAddr );
											 */
		FOnUDPSent_t pxHandleSent;
	#endif /* ipconfigUSE_CALLBACKS */
} IPUDPSocket_t;

typedef enum eSOCKET_EVENT {
	eSOCKET_RECEIVE = 0x0001,
	eSOCKET_SEND    = 0x0002,
	eSOCKET_ACCEPT  = 0x0004,
	eSOCKET_CONNECT = 0x0008,
	eSOCKET_BOUND   = 0x0010,
	eSOCKET_CLOSED	= 0x0020,
	eSOCKET_INTR	= 0x0040,
	eSOCKET_ALL		= 0x007F,
} eSocketEvent_t;

typedef struct XSOCKET
{
	EventBits_t xEventBits;
	EventGroupHandle_t xEventGroup;

	ListItem_t xBoundSocketListItem; /* Used to reference the socket from a bound sockets list. */
	TickType_t xReceiveBlockTime; /* if recv[to] is called while no data is available, wait this amount of time. Unit in clock-ticks */
	TickType_t xSendBlockTime; /* if send[to] is called while there is not enough space to send, wait this amount of time. Unit in clock-ticks */

	uint16_t usLocalPort;		/* Local port on this machine */
	uint8_t ucSocketOptions;
	uint8_t ucProtocol; /* choice of FREERTOS_IPPROTO_UDP/TCP */
	#if( ipconfigSOCKET_HAS_USER_SEMAPHORE == 1 )
		SemaphoreHandle_t pxUserSemaphore;
	#endif /* ipconfigSOCKET_HAS_USER_SEMAPHORE */
	#if( ipconfigSOCKET_HAS_USER_WAKE_CALLBACK == 1 )
		SocketWakeupCallback_t pxUserWakeCallback;
	#endif /* ipconfigSOCKET_HAS_USER_WAKE_CALLBACK */

	#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
		struct xSOCKET_SET *pxSocketSet;
		/* User may indicate which bits are interesting for this socket. */
		EventBits_t xSelectBits;
		/* These bits indicate the events which have actually occurred.
		They are maintained by the IP-task */
		EventBits_t xSocketBits;
	#endif /* ipconfigSUPPORT_SELECT_FUNCTION */
	/* TCP/UDP specific fields: */
	/* Before accessing any member of this structure, it should be confirmed */
	/* that the protocol corresponds with the type of structure */

	union
	{
		IPUDPSocket_t xUDP;
		#if( ipconfigUSE_TCP == 1 )
			IPTCPSocket_t xTCP;
			/* Make sure that xTCP is 8-bytes aligned by
			declaring a 64-bit variable in the same union */
			uint64_t ullTCPAlignment;
		#endif /* ipconfigUSE_TCP */
	} u;
} FreeRTOS_Socket_t;

#if( ipconfigUSE_TCP == 1 )
	/*
	 * Lookup a TCP socket, using a multiple matching: both port numbers and
	 * return IP address.
	 */
	FreeRTOS_Socket_t *pxTCPSocketLookup( uint32_t ulLocalIP, UBaseType_t uxLocalPort, uint32_t ulRemoteIP, UBaseType_t uxRemotePort );

#endif /* ipconfigUSE_TCP */

/*
 * Look up a local socket by finding a match with the local port.
 */
FreeRTOS_Socket_t *pxUDPSocketLookup( UBaseType_t uxLocalPort );

/*
 * Called when the application has generated a UDP packet to send.
 */
void vProcessGeneratedUDPPacket( NetworkBufferDescriptor_t * const pxNetworkBuffer );

/*
 * Calculate the upper-layer checksum
 * Works both for UDP, ICMP and TCP packages
 * bOut = true: checksum will be set in outgoing packets
 * bOut = false: checksum will be calculated for incoming packets
 *     returning 0xffff means: checksum was correct
 */
uint16_t usGenerateProtocolChecksum( const uint8_t * const pucEthernetBuffer, size_t uxBufferLength, BaseType_t xOutgoingPacket );

/*
 * An Ethernet frame has been updated (maybe it was an ARP request or a PING
 * request?) and is to be sent back to its source.
 */
void vReturnEthernetFrame( NetworkBufferDescriptor_t * pxNetworkBuffer, BaseType_t xReleaseAfterSend );

/*
 * The internal version of bind()
 * If 'ulInternal' is true, it is called by the driver
 * The TCP driver needs to bind a socket at the moment a listening socket
 * creates a new connected socket
 */
BaseType_t vSocketBind( FreeRTOS_Socket_t *pxSocket, struct freertos_sockaddr * pxAddress, size_t uxAddressLength, BaseType_t xInternal );

/*
 * Internal function to add streaming data to a TCP socket. If ulIn == true,
 * data will be added to the rxStream, otherwise to the tXStream.  Normally data
 * will be written with ulOffset == 0, meaning: at the end of the FIFO.  When
 * packet come in out-of-order, an offset will be used to put it in front and
 * the head will not change yet.
 */
int32_t lTCPAddRxdata(FreeRTOS_Socket_t *pxSocket, size_t uxOffset, const uint8_t *pcData, uint32_t ulByteCount);

/*
 * Currently called for any important event.
 */
void vSocketWakeUpUser( FreeRTOS_Socket_t *pxSocket );

/*
 * Some helping function, their meaning should be clear
 */
static portINLINE uint32_t ulChar2u32 (const uint8_t *apChr);
static portINLINE uint32_t ulChar2u32 (const uint8_t *apChr)
{
	return  ( ( ( uint32_t )apChr[0] ) << 24) |
			( ( ( uint32_t )apChr[1] ) << 16) |
			( ( ( uint32_t )apChr[2] ) << 8) |
			( ( ( uint32_t )apChr[3] ) );
}

static portINLINE uint16_t usChar2u16 (const uint8_t *apChr);
static portINLINE uint16_t usChar2u16 (const uint8_t *apChr)
{
	return ( uint16_t )
			( ( ( ( uint32_t )apChr[0] ) << 8) |
			  ( ( ( uint32_t )apChr[1] ) ) );
}

/* Check a single socket for retransmissions and timeouts */
BaseType_t xTCPSocketCheck( FreeRTOS_Socket_t *pxSocket );

BaseType_t xTCPCheckNewClient( FreeRTOS_Socket_t *pxSocket );

/* Defined in FreeRTOS_Sockets.c
 * Close a socket
 */
void *vSocketClose( FreeRTOS_Socket_t *pxSocket );

/*
 * Send the event eEvent to the IP task event queue, using a block time of
 * zero.  Return pdPASS if the message was sent successfully, otherwise return
 * pdFALSE.
*/
BaseType_t xSendEventToIPTask( eIPEvent_t eEvent );

/*
 * The same as above, but a struct as a parameter, containing:
 * 		eIPEvent_t eEventType;
 *		void *pvData;
 */
BaseType_t xSendEventStructToIPTask( const IPStackEvent_t *pxEvent, TickType_t xTimeout );

/*
 * Returns a pointer to the original NetworkBuffer from a pointer to a UDP
 * payload buffer.
 */
NetworkBufferDescriptor_t *pxUDPPayloadBuffer_to_NetworkBuffer( void *pvBuffer );

#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
	/*
	 * For the case where the network driver passes a buffer directly to a DMA
	 * descriptor, this function can be used to translate a 'network buffer' to
	 * a 'network buffer descriptor'.
	 */
	NetworkBufferDescriptor_t *pxPacketBuffer_to_NetworkBuffer( const void *pvBuffer );
#endif

/*
 * Internal: Sets a new state for a TCP socket and performs the necessary
 * actions like calling a OnConnected handler to notify the socket owner.
 */
#if( ipconfigUSE_TCP == 1 )
	void vTCPStateChange( FreeRTOS_Socket_t *pxSocket, enum eTCP_STATE eTCPState );
#endif /* ipconfigUSE_TCP */

/*_RB_ Should this be part of the public API? */
void FreeRTOS_netstat( void );

/* Returns pdTRUE is this function is called from the IP-task */
BaseType_t xIsCallingFromIPTask( void );

#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )

typedef struct xSOCKET_SET
{
	EventGroupHandle_t xSelectGroup;
	BaseType_t bApiCalled;	/* True if the API was calling  the private vSocketSelect */
	FreeRTOS_Socket_t *pxSocket;
} SocketSelect_t;

extern void vSocketSelect( SocketSelect_t *pxSocketSelect );

#endif /* ipconfigSUPPORT_SELECT_FUNCTION */

void vIPSetDHCPTimerEnableState( BaseType_t xEnableState );
void vIPReloadDHCPTimer( uint32_t ulLeaseTime );
#if( ipconfigDNS_USE_CALLBACKS != 0 )
	void vIPReloadDNSTimer( uint32_t ulCheckTime );
	void vIPSetDnsTimerEnableState( BaseType_t xEnableState );
#endif

/* Send the network-up event and start the ARP timer. */
void vIPNetworkUpCalls( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FREERTOS_IP_PRIVATE_H */













