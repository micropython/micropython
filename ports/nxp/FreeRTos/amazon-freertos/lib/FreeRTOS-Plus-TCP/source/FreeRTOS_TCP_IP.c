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

/*
 * FreeRTOS_TCP_IP.c
 * Module which handles the TCP connections for FreeRTOS+TCP.
 * It depends on  FreeRTOS_TCP_WIN.c, which handles the TCP windowing
 * schemes.
 *
 * Endianness: in this module all ports and IP addresses are stored in
 * host byte-order, except fields in the IP-packets
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_DHCP.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"
#include "FreeRTOS_ARP.h"
#include "FreeRTOS_TCP_WIN.h"


/* Just make sure the contents doesn't get compiled if TCP is not enabled. */
#if ipconfigUSE_TCP == 1

/* This compile-time test was moved to here because some macro's
were unknown within 'FreeRTOSIPConfigDefaults.h'.  It tests whether
the defined MTU size can contain at least a complete TCP packet. */

#if ( ( ipconfigTCP_MSS + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER ) > ipconfigNETWORK_MTU )
	#error The ipconfigTCP_MSS setting in FreeRTOSIPConfig.h is too large.
#endif

/*
 * The meaning of the TCP flags:
 */
#define ipTCP_FLAG_FIN			0x0001u /* No more data from sender */
#define ipTCP_FLAG_SYN			0x0002u /* Synchronize sequence numbers */
#define ipTCP_FLAG_RST			0x0004u /* Reset the connection */
#define ipTCP_FLAG_PSH			0x0008u /* Push function: please push buffered data to the recv application */
#define ipTCP_FLAG_ACK			0x0010u /* Acknowledgment field is significant */
#define ipTCP_FLAG_URG			0x0020u /* Urgent pointer field is significant */
#define ipTCP_FLAG_ECN			0x0040u /* ECN-Echo */
#define ipTCP_FLAG_CWR			0x0080u /* Congestion Window Reduced */
#define ipTCP_FLAG_NS			0x0100u /* ECN-nonce concealment protection */
#define ipTCP_FLAG_RSV			0x0E00u /* Reserved, keep 0 */

/* A mask to filter all protocol flags. */
#define ipTCP_FLAG_CTRL			0x001Fu

/*
 * A few values of the TCP options:
 */
#define TCP_OPT_END				0u   /* End of TCP options list */
#define TCP_OPT_NOOP			1u   /* "No-operation" TCP option */
#define TCP_OPT_MSS				2u   /* Maximum segment size TCP option */
#define TCP_OPT_WSOPT			3u   /* TCP Window Scale Option (3-byte long) */
#define TCP_OPT_SACK_P			4u   /* Advertize that SACK is permitted */
#define TCP_OPT_SACK_A			5u   /* SACK option with first/last */
#define TCP_OPT_TIMESTAMP		8u   /* Time-stamp option */

#define TCP_OPT_MSS_LEN			4u   /* Length of TCP MSS option. */
#define TCP_OPT_WSOPT_LEN		3u   /* Length of TCP WSOPT option. */

#define TCP_OPT_TIMESTAMP_LEN	10	/* fixed length of the time-stamp option */

#ifndef ipconfigTCP_ACK_EARLIER_PACKET
	#define ipconfigTCP_ACK_EARLIER_PACKET		1
#endif

/*
 * The macro NOW_CONNECTED() is use to determine if the connection makes a
 * transition from connected to non-connected and vice versa.
 * NOW_CONNECTED() returns true when the status has one of these values:
 * eESTABLISHED, eFIN_WAIT_1, eFIN_WAIT_2, eCLOSING, eLAST_ACK, eTIME_WAIT
 * Technically the connection status is closed earlier, but the library wants
 * to prevent that the socket will be deleted before the last ACK has been
 * and thus causing a 'RST' packet on either side.
 */
#define NOW_CONNECTED( status )\
	( ( status >= eESTABLISHED ) && ( status != eCLOSE_WAIT ) )

/*
 * The highest 4 bits in the TCP offset byte indicate the total length of the
 * TCP header, divided by 4.
 */
#define VALID_BITS_IN_TCP_OFFSET_BYTE		( 0xF0u )

/*
 * Acknowledgements to TCP data packets may be delayed as long as more is being expected.
 * A normal delay would be 200ms.  Here a much shorter delay of 20 ms is being used to
 * gain performance.
 */
#define DELAYED_ACK_SHORT_DELAY_MS			( 2 )
#define DELAYED_ACK_LONGER_DELAY_MS			( 20 )

/*
 * The MSS (Maximum Segment Size) will be taken as large as possible. However, packets with
 * an MSS of 1460 bytes won't be transported through the internet.  The MSS will be reduced
 * to 1400 bytes.
 */
#define REDUCED_MSS_THROUGH_INTERNET		( 1400 )

/*
 * When there are no TCP options, the TCP offset equals 20 bytes, which is stored as
 * the number 5 (words) in the higher niblle of the TCP-offset byte.
 */
#define TCP_OFFSET_LENGTH_BITS			( 0xf0u )
#define TCP_OFFSET_STANDARD_LENGTH		( 0x50u )

/*
 * Each TCP socket is checked regularly to see if it can send data packets.
 * By default, the maximum number of packets sent during one check is limited to 8.
 * This amount may be further limited by setting the socket's TX window size.
 */
#if( !defined( SEND_REPEATED_COUNT ) )
	#define SEND_REPEATED_COUNT		( 8 )
#endif /* !defined( SEND_REPEATED_COUNT ) */

/*
 * Define a maximum perdiod of time (ms) to leave a TCP-socket unattended.
 * When a TCP timer expires, retries and keep-alive messages will be checked.
 */
#ifndef	tcpMAXIMUM_TCP_WAKEUP_TIME_MS
	#define	tcpMAXIMUM_TCP_WAKEUP_TIME_MS		20000u
#endif

/*
 * The names of the different TCP states may be useful in logging.
 */
#if( ( ipconfigHAS_DEBUG_PRINTF != 0 ) || ( ipconfigHAS_PRINTF != 0 ) )
	static const char *pcStateNames[] = {
		"eCLOSED",
		"eTCP_LISTEN",
		"eCONNECT_SYN",
		"eSYN_FIRST",
		"eSYN_RECEIVED",
		"eESTABLISHED",
		"eFIN_WAIT_1",
		"eFIN_WAIT_2",
		"eCLOSE_WAIT",
		"eCLOSING",
		"eLAST_ACK",
		"eTIME_WAIT",
		"eUNKNOWN",
};
#endif /* ( ipconfigHAS_DEBUG_PRINTF != 0 ) || ( ipconfigHAS_PRINTF != 0 ) */

/*
 * Returns true if the socket must be checked.  Non-active sockets are waiting
 * for user action, either connect() or close().
 */
static BaseType_t prvTCPSocketIsActive( UBaseType_t uxStatus );

/*
 * Either sends a SYN or calls prvTCPSendRepeated (for regular messages).
 */
static int32_t prvTCPSendPacket( FreeRTOS_Socket_t *pxSocket );

/*
 * Try to send a series of messages.
 */
static int32_t prvTCPSendRepeated( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer );

/*
 * Return or send a packet to the other party.
 */
static void prvTCPReturnPacket( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer,
	uint32_t ulLen, BaseType_t xReleaseAfterSend );

/*
 * Initialise the data structures which keep track of the TCP windowing system.
 */
static void prvTCPCreateWindow( FreeRTOS_Socket_t *pxSocket );

/*
 * Let ARP look-up the MAC-address of the peer and initialise the first SYN
 * packet.
 */
static BaseType_t prvTCPPrepareConnect( FreeRTOS_Socket_t *pxSocket );

#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	/*
	 * For logging and debugging: make a string showing the TCP flags.
	 */
	static const char *prvTCPFlagMeaning( UBaseType_t xFlags);
#endif /* ipconfigHAS_DEBUG_PRINTF != 0 */

/*
 * Parse the TCP option(s) received, if present.
 */
static void prvCheckOptions( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer );

/*
 * Set the initial properties in the options fields, like the preferred
 * value of MSS and whether SACK allowed.  Will be transmitted in the state
 * 'eCONNECT_SYN'.
 */
static UBaseType_t prvSetSynAckOptions( FreeRTOS_Socket_t *pxSocket, TCPPacket_t * pxTCPPacket );

/*
 * For anti-hang protection and TCP keep-alive messages.  Called in two places:
 * after receiving a packet and after a state change.  The socket's alive timer
 * may be reset.
 */
static void prvTCPTouchSocket( FreeRTOS_Socket_t *pxSocket );

/*
 * Prepare an outgoing message, if anything has to be sent.
 */
static int32_t prvTCPPrepareSend( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer, UBaseType_t uxOptionsLength );

/*
 * Calculate when this socket needs to be checked to do (re-)transmissions.
 */
static TickType_t prvTCPNextTimeout( FreeRTOS_Socket_t *pxSocket );

/*
 * The API FreeRTOS_send() adds data to the TX stream.  Add
 * this data to the windowing system to it can be transmitted.
 */
static void prvTCPAddTxData( FreeRTOS_Socket_t *pxSocket );

/*
 *  Called to handle the closure of a TCP connection.
 */
static BaseType_t prvTCPHandleFin( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer );

/*
 * Called from prvTCPHandleState().  Find the TCP payload data and check and
 * return its length.
 */
static BaseType_t prvCheckRxData( NetworkBufferDescriptor_t *pxNetworkBuffer, uint8_t **ppucRecvData );

/*
 * Called from prvTCPHandleState().  Check if the payload data may be accepted.
 * If so, it will be added to the socket's reception queue.
 */
static BaseType_t prvStoreRxData( FreeRTOS_Socket_t *pxSocket, uint8_t *pucRecvData,
	NetworkBufferDescriptor_t *pxNetworkBuffer, uint32_t ulReceiveLength );

/*
 * Set the TCP options (if any) for the outgoing packet.
 */
static UBaseType_t prvSetOptions( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer );

/*
 * Called from prvTCPHandleState() as long as the TCP status is eSYN_RECEIVED to
 * eCONNECT_SYN.
 */
static BaseType_t prvHandleSynReceived( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer,
	uint32_t ulReceiveLength, UBaseType_t uxOptionsLength );

/*
 * Called from prvTCPHandleState() as long as the TCP status is eESTABLISHED.
 */
static BaseType_t prvHandleEstablished( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer,
	uint32_t ulReceiveLength, UBaseType_t uxOptionsLength );

/*
 * Called from prvTCPHandleState().  There is data to be sent.
 * If ipconfigUSE_TCP_WIN is defined, and if only an ACK must be sent, it will
 * be checked if it would better be postponed for efficiency.
 */
static BaseType_t prvSendData( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer,
	uint32_t ulReceiveLength, BaseType_t xSendLength );

/*
 * The heart of all: check incoming packet for valid data and acks and do what
 * is necessary in each state.
 */
static BaseType_t prvTCPHandleState( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer );

/*
 * Reply to a peer with the RST flag on, in case a packet can not be handled.
 */
static BaseType_t prvTCPSendReset( NetworkBufferDescriptor_t *pxNetworkBuffer );

/*
 * Set the initial value for MSS (Maximum Segment Size) to be used.
 */
static void prvSocketSetMSS( FreeRTOS_Socket_t *pxSocket );

/*
 * Return either a newly created socket, or the current socket in a connected
 * state (depends on the 'bReuseSocket' flag).
 */
static FreeRTOS_Socket_t *prvHandleListen( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer );

/*
 * After a listening socket receives a new connection, it may duplicate itself.
 * The copying takes place in prvTCPSocketCopy.
 */
static BaseType_t prvTCPSocketCopy( FreeRTOS_Socket_t *pxNewSocket, FreeRTOS_Socket_t *pxSocket );

/*
 * prvTCPStatusAgeCheck() will see if the socket has been in a non-connected
 * state for too long.  If so, the socket will be closed, and -1 will be
 * returned.
 */
#if( ipconfigTCP_HANG_PROTECTION == 1 )
	static BaseType_t prvTCPStatusAgeCheck( FreeRTOS_Socket_t *pxSocket );
#endif

static NetworkBufferDescriptor_t *prvTCPBufferResize( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer,
	int32_t lDataLen, UBaseType_t uxOptionsLength );

#if( ( ipconfigHAS_DEBUG_PRINTF != 0 ) || ( ipconfigHAS_PRINTF != 0 ) )
	const char *FreeRTOS_GetTCPStateName( UBaseType_t ulState );
#endif

#if( ipconfigUSE_TCP_WIN != 0 )
	static uint8_t prvWinScaleFactor( FreeRTOS_Socket_t *pxSocket );
#endif

/*
 * Generate a randomized TCP Initial Sequence Number per RFC.
 */
extern uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
													uint16_t usSourcePort,
													uint32_t ulDestinationAddress,
													uint16_t usDestinationPort );

/*-----------------------------------------------------------*/

/* prvTCPSocketIsActive() returns true if the socket must be checked.
 * Non-active sockets are waiting for user action, either connect()
 * or close(). */
static BaseType_t prvTCPSocketIsActive( UBaseType_t uxStatus )
{
	switch( uxStatus )
	{
	case eCLOSED:
	case eCLOSE_WAIT:
	case eFIN_WAIT_2:
	case eCLOSING:
	case eTIME_WAIT:
		return pdFALSE;
	default:
		return pdTRUE;
	}
}
/*-----------------------------------------------------------*/

#if( ipconfigTCP_HANG_PROTECTION == 1 )

	static BaseType_t prvTCPStatusAgeCheck( FreeRTOS_Socket_t *pxSocket )
	{
	BaseType_t xResult;
		switch( pxSocket->u.xTCP.ucTCPState )
		{
		case eESTABLISHED:
			/* If the 'ipconfigTCP_KEEP_ALIVE' option is enabled, sockets in
			state ESTABLISHED can be protected using keep-alive messages. */
			xResult = pdFALSE;
			break;
		case eCLOSED:
		case eTCP_LISTEN:
		case eCLOSE_WAIT:
			/* These 3 states may last for ever, up to the owner. */
			xResult = pdFALSE;
			break;
		default:
			/* All other (non-connected) states will get anti-hanging
			protection. */
			xResult = pdTRUE;
			break;
		}
		if( xResult != pdFALSE )
		{
			/* How much time has past since the last active moment which is
			defined as A) a state change or B) a packet has arrived. */
			TickType_t xAge = xTaskGetTickCount( ) - pxSocket->u.xTCP.xLastActTime;

			/* ipconfigTCP_HANG_PROTECTION_TIME is in units of seconds. */
			if( xAge > ( ipconfigTCP_HANG_PROTECTION_TIME * configTICK_RATE_HZ ) )
			{
				#if( ipconfigHAS_DEBUG_PRINTF == 1 )
				{
					FreeRTOS_debug_printf( ( "Inactive socket closed: port %u rem %lxip:%u status %s\n",
						pxSocket->usLocalPort,
						pxSocket->u.xTCP.ulRemoteIP,
						pxSocket->u.xTCP.usRemotePort,
						FreeRTOS_GetTCPStateName( ( UBaseType_t ) pxSocket->u.xTCP.ucTCPState ) ) );
				}
				#endif /* ipconfigHAS_DEBUG_PRINTF */

				/* Move to eCLOSE_WAIT, user may close the socket. */
				vTCPStateChange( pxSocket, eCLOSE_WAIT );

				/* When 'bPassQueued' true, this socket is an orphan until it
				gets connected. */
				if( pxSocket->u.xTCP.bits.bPassQueued != pdFALSE_UNSIGNED )
				{
					if( pxSocket->u.xTCP.bits.bReuseSocket == pdFALSE_UNSIGNED )
					{
						/* As it did not get connected, and the user can never
						accept() it anymore, it will be deleted now.  Called from
						the IP-task, so it's safe to call the internal Close
						function: vSocketClose(). */
						vSocketClose( pxSocket );
					}
					/* Return a negative value to tell to inform the caller
					xTCPTimerCheck()
					that the socket got closed and may not be accessed anymore. */
					xResult = -1;
				}
			}
		}
		return xResult;
	}
	/*-----------------------------------------------------------*/

#endif

/*
 * As soon as a TCP socket timer expires, this function xTCPSocketCheck
 * will be called (from xTCPTimerCheck)
 * It can send a delayed ACK or new data
 * Sequence of calling (normally) :
 * IP-Task:
 *		xTCPTimerCheck()				// Check all sockets ( declared in FreeRTOS_Sockets.c )
 *		xTCPSocketCheck()				// Either send a delayed ACK or call prvTCPSendPacket()
 *		prvTCPSendPacket()				// Either send a SYN or call prvTCPSendRepeated ( regular messages )
 *		prvTCPSendRepeated()			// Send at most 8 messages on a row
 *			prvTCPReturnPacket()		// Prepare for returning
 *			xNetworkInterfaceOutput()	// Sends data to the NIC ( declared in portable/NetworkInterface/xxx )
 */
BaseType_t xTCPSocketCheck( FreeRTOS_Socket_t *pxSocket )
{
BaseType_t xResult = 0;
BaseType_t xReady = pdFALSE;

	if( ( pxSocket->u.xTCP.ucTCPState >= eESTABLISHED ) && ( pxSocket->u.xTCP.txStream != NULL ) )
	{
		/* The API FreeRTOS_send() might have added data to the TX stream.  Add
		this data to the windowing system to it can be transmitted. */
		prvTCPAddTxData( pxSocket );
	}

	#if ipconfigUSE_TCP_WIN == 1
	{
		if( pxSocket->u.xTCP.pxAckMessage != NULL )
		{
			/* The first task of this regular socket check is to send-out delayed
			ACK's. */
			if( pxSocket->u.xTCP.bits.bUserShutdown == pdFALSE_UNSIGNED )
			{
				/* Earlier data was received but not yet acknowledged.  This
				function is called when the TCP timer for the socket expires, the
				ACK may be sent now. */
				if( pxSocket->u.xTCP.ucTCPState != eCLOSED )
				{
					if( xTCPWindowLoggingLevel > 1 && ipconfigTCP_MAY_LOG_PORT( pxSocket->usLocalPort ) )
					{
						FreeRTOS_debug_printf( ( "Send[%u->%u] del ACK %lu SEQ %lu (len %u)\n",
							pxSocket->usLocalPort,
							pxSocket->u.xTCP.usRemotePort,
							pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber - pxSocket->u.xTCP.xTCPWindow.rx.ulFirstSequenceNumber,
							pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber   - pxSocket->u.xTCP.xTCPWindow.tx.ulFirstSequenceNumber,
							ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER ) );
					}

					prvTCPReturnPacket( pxSocket, pxSocket->u.xTCP.pxAckMessage, ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER, ipconfigZERO_COPY_TX_DRIVER );

					#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
					{
						/* The ownership has been passed to the SEND routine,
						clear the pointer to it. */
						pxSocket->u.xTCP.pxAckMessage = NULL;
					}
					#endif /* ipconfigZERO_COPY_TX_DRIVER */
				}
				if( prvTCPNextTimeout( pxSocket ) > 1 )
				{
					/* Tell the code below that this function is ready. */
					xReady = pdTRUE;
				}
			}
			else
			{
				/* The user wants to perform an active shutdown(), skip sending
				the	delayed	ACK.  The function prvTCPSendPacket() will send the
				FIN	along with the ACK's. */
			}

			if( pxSocket->u.xTCP.pxAckMessage != NULL )
			{
				vReleaseNetworkBufferAndDescriptor( pxSocket->u.xTCP.pxAckMessage );
				pxSocket->u.xTCP.pxAckMessage = NULL;
			}
		}
	}
	#endif /* ipconfigUSE_TCP_WIN */

	if( xReady == pdFALSE )
	{
		/* The second task of this regular socket check is sending out data. */
		if( ( pxSocket->u.xTCP.ucTCPState >= eESTABLISHED ) ||
			( pxSocket->u.xTCP.ucTCPState == eCONNECT_SYN ) )
		{
			prvTCPSendPacket( pxSocket );
		}

		/* Set the time-out for the next wakeup for this socket. */
		prvTCPNextTimeout( pxSocket );

		#if( ipconfigTCP_HANG_PROTECTION == 1 )
		{
			/* In all (non-connected) states in which keep-alive messages can not be sent
			the anti-hang protocol will close sockets that are 'hanging'. */
			xResult = prvTCPStatusAgeCheck( pxSocket );
		}
		#endif
	}

	return xResult;
}
/*-----------------------------------------------------------*/

/*
 * prvTCPSendPacket() will be called when the socket time-out has been reached.
 * It is only called by xTCPSocketCheck().
 */
static int32_t prvTCPSendPacket( FreeRTOS_Socket_t *pxSocket )
{
int32_t lResult = 0;
UBaseType_t uxOptionsLength;
TCPPacket_t *pxTCPPacket;
NetworkBufferDescriptor_t *pxNetworkBuffer;

	if( pxSocket->u.xTCP.ucTCPState != eCONNECT_SYN )
	{
		/* The connection is in s state other than SYN. */
		pxNetworkBuffer = NULL;

		/* prvTCPSendRepeated() will only create a network buffer if necessary,
		i.e. when data must be sent to the peer. */
		lResult = prvTCPSendRepeated( pxSocket, &pxNetworkBuffer );

		if( pxNetworkBuffer != NULL )
		{
			vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
		}
	}
	else
	{
		if( pxSocket->u.xTCP.ucRepCount >= 3u )
		{
			/* The connection is in the SYN status. The packet will be repeated
			to most 3 times.  When there is no response, the socket get the
			status 'eCLOSE_WAIT'. */
			FreeRTOS_debug_printf( ( "Connect: giving up %lxip:%u\n",
				pxSocket->u.xTCP.ulRemoteIP,		/* IP address of remote machine. */
				pxSocket->u.xTCP.usRemotePort ) );	/* Port on remote machine. */
			vTCPStateChange( pxSocket, eCLOSE_WAIT );
		}
		else if( ( pxSocket->u.xTCP.bits.bConnPrepared != pdFALSE_UNSIGNED ) || ( prvTCPPrepareConnect( pxSocket ) == pdTRUE ) )
		{
			/* Or else, if the connection has been prepared, or can be prepared
			now, proceed to send the packet with the SYN flag.
			prvTCPPrepareConnect() prepares 'xPacket' and returns pdTRUE if
			the Ethernet address of the peer or the gateway is found. */
			pxTCPPacket = ( TCPPacket_t * )pxSocket->u.xTCP.xPacket.u.ucLastPacket;

			/* About to send a SYN packet.  Call prvSetSynAckOptions() to set
			the proper options: The size of MSS and whether SACK's are
			allowed. */
			uxOptionsLength = prvSetSynAckOptions( pxSocket, pxTCPPacket );

			/* Return the number of bytes to be sent. */
			lResult = ( BaseType_t ) ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + uxOptionsLength );

			/* Set the TCP offset field:  ipSIZE_OF_TCP_HEADER equals 20 and
			uxOptionsLength is always a multiple of 4.  The complete expression
			would be:
			ucTCPOffset = ( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) / 4 ) << 4 */
			pxTCPPacket->xTCPHeader.ucTCPOffset = ( uint8_t )( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) << 2 );

			/* Repeat Count is used for a connecting socket, to limit the number
			of tries. */
			pxSocket->u.xTCP.ucRepCount++;

			/* Send the SYN message to make a connection.  The messages is
			stored in the socket field 'xPacket'.  It will be wrapped in a
			pseudo network buffer descriptor before it will be sent. */
			prvTCPReturnPacket( pxSocket, NULL, ( uint32_t ) lResult, pdFALSE );
		}
	}

	/* Return the total number of bytes sent. */
	return lResult;
}
/*-----------------------------------------------------------*/

/*
 * prvTCPSendRepeated will try to send a series of messages, as long as there is
 * data to be sent and as long as the transmit window isn't full.
 */
static int32_t prvTCPSendRepeated( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer )
{
UBaseType_t uxIndex;
int32_t lResult = 0;
UBaseType_t uxOptionsLength = 0u;
int32_t xSendLength;

	for( uxIndex = 0u; uxIndex < ( UBaseType_t ) SEND_REPEATED_COUNT; uxIndex++ )
	{
		/* prvTCPPrepareSend() might allocate a network buffer if there is data
		to be sent. */
		xSendLength = prvTCPPrepareSend( pxSocket, ppxNetworkBuffer, uxOptionsLength );
		if( xSendLength <= 0 )
		{
			break;
		}

		/* And return the packet to the peer. */
		prvTCPReturnPacket( pxSocket, *ppxNetworkBuffer, ( uint32_t ) xSendLength, ipconfigZERO_COPY_TX_DRIVER );

		#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
		{
			*ppxNetworkBuffer = NULL;
		}
		#endif /* ipconfigZERO_COPY_TX_DRIVER */

		lResult += xSendLength;
	}

	/* Return the total number of bytes sent. */
	return lResult;
}
/*-----------------------------------------------------------*/

/*
 * Return (or send) a packet the the peer.  The data is stored in pxBuffer,
 * which may either point to a real network buffer or to a TCP socket field
 * called 'xTCP.xPacket'.   A temporary xNetworkBuffer will be used to pass
 * the data to the NIC.
 */
static void prvTCPReturnPacket( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer, uint32_t ulLen, BaseType_t xReleaseAfterSend )
{
TCPPacket_t * pxTCPPacket;
IPHeader_t *pxIPHeader;
EthernetHeader_t *pxEthernetHeader;
uint32_t ulFrontSpace, ulSpace, ulSourceAddress, ulWinSize;
TCPWindow_t *pxTCPWindow;
NetworkBufferDescriptor_t xTempBuffer;
/* For sending, a pseudo network buffer will be used, as explained above. */

	if( pxNetworkBuffer == NULL )
	{
		pxNetworkBuffer = &xTempBuffer;

		#if( ipconfigUSE_LINKED_RX_MESSAGES != 0 )
		{
			xTempBuffer.pxNextBuffer = NULL;
		}
		#endif
		xTempBuffer.pucEthernetBuffer = pxSocket->u.xTCP.xPacket.u.ucLastPacket;
		xTempBuffer.xDataLength = sizeof( pxSocket->u.xTCP.xPacket.u.ucLastPacket );
		xReleaseAfterSend = pdFALSE;
	}

	#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
	{
		if( xReleaseAfterSend == pdFALSE )
		{
			pxNetworkBuffer = pxDuplicateNetworkBufferWithDescriptor( pxNetworkBuffer, ( BaseType_t ) pxNetworkBuffer->xDataLength );
			if( pxNetworkBuffer == NULL )
			{
				FreeRTOS_debug_printf( ( "prvTCPReturnPacket: duplicate failed\n" ) );
			}
			xReleaseAfterSend = pdTRUE;
		}
	}
	#endif /* ipconfigZERO_COPY_TX_DRIVER */

	if( pxNetworkBuffer != NULL )
	{
		pxTCPPacket = ( TCPPacket_t * ) ( pxNetworkBuffer->pucEthernetBuffer );
		pxIPHeader = &pxTCPPacket->xIPHeader;
		pxEthernetHeader = &pxTCPPacket->xEthernetHeader;

		/* Fill the packet, using hton translations. */
		if( pxSocket != NULL )
		{
			/* Calculate the space in the RX buffer in order to advertise the
			size of this socket's reception window. */
			pxTCPWindow = &( pxSocket->u.xTCP.xTCPWindow );

			if( pxSocket->u.xTCP.rxStream != NULL )
			{
				/* An RX stream was created already, see how much space is
				available. */
				ulFrontSpace = ( uint32_t ) uxStreamBufferFrontSpace( pxSocket->u.xTCP.rxStream );
			}
			else
			{
				/* No RX stream has been created, the full stream size is
				available. */
				ulFrontSpace = ( uint32_t ) pxSocket->u.xTCP.uxRxStreamSize;
			}

			/* Take the minimum of the RX buffer space and the RX window size. */
			ulSpace = FreeRTOS_min_uint32( pxSocket->u.xTCP.ulRxCurWinSize, pxTCPWindow->xSize.ulRxWindowLength );

			if( ( pxSocket->u.xTCP.bits.bLowWater != pdFALSE_UNSIGNED ) || ( pxSocket->u.xTCP.bits.bRxStopped != pdFALSE_UNSIGNED ) )
			{
				/* The low-water mark was reached, meaning there was little
				space left.  The socket will wait until the application has read
				or flushed the incoming data, and 'zero-window' will be
				advertised. */
				ulSpace = 0u;
			}

			/* If possible, advertise an RX window size of at least 1 MSS, otherwise
			the peer might start 'zero window probing', i.e. sending small packets
			(1, 2, 4, 8... bytes). */
			if( ( ulSpace < pxSocket->u.xTCP.usCurMSS ) && ( ulFrontSpace >= pxSocket->u.xTCP.usCurMSS ) )
			{
				ulSpace = pxSocket->u.xTCP.usCurMSS;
			}

			/* Avoid overflow of the 16-bit win field. */
			#if( ipconfigUSE_TCP_WIN != 0 )
			{
				ulWinSize = ( ulSpace >> pxSocket->u.xTCP.ucMyWinScaleFactor );
			}
			#else
			{
				ulWinSize = ulSpace;
			}
			#endif
			if( ulWinSize > 0xfffcUL )
			{
				ulWinSize = 0xfffcUL;
			}

			pxTCPPacket->xTCPHeader.usWindow = FreeRTOS_htons( ( uint16_t ) ulWinSize );

			#if( ipconfigHAS_DEBUG_PRINTF != 0 )
			{
				if( ipconfigTCP_MAY_LOG_PORT( pxSocket->usLocalPort ) != pdFALSE )
				{
					if( ( xTCPWindowLoggingLevel != 0 ) && ( pxSocket->u.xTCP.bits.bWinChange != pdFALSE_UNSIGNED ) )
					{
					size_t uxFrontSpace;

						if(pxSocket->u.xTCP.rxStream != NULL)
						{
							uxFrontSpace =  uxStreamBufferFrontSpace( pxSocket->u.xTCP.rxStream ) ;
						}
						else
						{
							uxFrontSpace = 0u;
						}

						FreeRTOS_debug_printf( ( "%s: %lxip:%u: [%lu < %lu] winSize %ld\n",
						pxSocket->u.xTCP.bits.bLowWater ? "STOP" : "GO ",
							pxSocket->u.xTCP.ulRemoteIP,
							pxSocket->u.xTCP.usRemotePort,
							pxSocket->u.xTCP.bits.bLowWater ? pxSocket->u.xTCP.uxLittleSpace : uxFrontSpace, pxSocket->u.xTCP.uxEnoughSpace,
							(int32_t) ( pxTCPWindow->rx.ulHighestSequenceNumber - pxTCPWindow->rx.ulCurrentSequenceNumber ) ) );
					}
				}
			}
			#endif /* ipconfigHAS_DEBUG_PRINTF != 0 */

			/* The new window size has been advertised, switch off the flag. */
			pxSocket->u.xTCP.bits.bWinChange = pdFALSE_UNSIGNED;

			/* Later on, when deciding to delay an ACK, a precise estimate is needed
			of the free RX space.  At this moment, 'ulHighestRxAllowed' would be the
			highest sequence number minus 1 that the socket will accept. */
			pxSocket->u.xTCP.ulHighestRxAllowed = pxTCPWindow->rx.ulCurrentSequenceNumber + ulSpace;

			#if( ipconfigTCP_KEEP_ALIVE == 1 )
				if( pxSocket->u.xTCP.bits.bSendKeepAlive != pdFALSE_UNSIGNED )
				{
					/* Sending a keep-alive packet, send the current sequence number
					minus 1, which will	be recognised as a keep-alive packet an
					responded to by acknowledging the last byte. */
					pxSocket->u.xTCP.bits.bSendKeepAlive = pdFALSE_UNSIGNED;
					pxSocket->u.xTCP.bits.bWaitKeepAlive = pdTRUE_UNSIGNED;

					pxTCPPacket->xTCPHeader.ulSequenceNumber = pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber - 1UL;
					pxTCPPacket->xTCPHeader.ulSequenceNumber = FreeRTOS_htonl( pxTCPPacket->xTCPHeader.ulSequenceNumber );
				}
				else
			#endif
			{
				pxTCPPacket->xTCPHeader.ulSequenceNumber = FreeRTOS_htonl( pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber );

				if( ( pxTCPPacket->xTCPHeader.ucTCPFlags & ( uint8_t ) ipTCP_FLAG_FIN ) != 0u )
				{
					/* Suppress FIN in case this packet carries earlier data to be
					retransmitted. */
					uint32_t ulDataLen = ( uint32_t ) ( ulLen - ( ipSIZE_OF_TCP_HEADER + ipSIZE_OF_IPv4_HEADER ) );
					if( ( pxTCPWindow->ulOurSequenceNumber + ulDataLen ) != pxTCPWindow->tx.ulFINSequenceNumber )
					{
						pxTCPPacket->xTCPHeader.ucTCPFlags &= ( ( uint8_t ) ~ipTCP_FLAG_FIN );
						FreeRTOS_debug_printf( ( "Suppress FIN for %lu + %lu < %lu\n",
							pxTCPWindow->ulOurSequenceNumber - pxTCPWindow->tx.ulFirstSequenceNumber,
							ulDataLen,
							pxTCPWindow->tx.ulFINSequenceNumber - pxTCPWindow->tx.ulFirstSequenceNumber ) );
					}
				}
			}

			/* Tell which sequence number is expected next time */
			pxTCPPacket->xTCPHeader.ulAckNr = FreeRTOS_htonl( pxTCPWindow->rx.ulCurrentSequenceNumber );
		}
		else
		{
			/* Sending data without a socket, probably replying with a RST flag
			Just swap the two sequence numbers. */
			vFlip_32( pxTCPPacket->xTCPHeader.ulSequenceNumber, pxTCPPacket->xTCPHeader.ulAckNr );
		}

		pxIPHeader->ucTimeToLive		   = ( uint8_t ) ipconfigTCP_TIME_TO_LIVE;
		pxIPHeader->usLength			   = FreeRTOS_htons( ulLen );
		if( ( pxSocket == NULL ) || ( *ipLOCAL_IP_ADDRESS_POINTER == 0ul ) )
		{
			/* When pxSocket is NULL, this function is called by prvTCPSendReset()
			and the IP-addresses must be swapped.
			Also swap the IP-addresses in case the IP-tack doesn't have an
			IP-address yet, i.e. when ( *ipLOCAL_IP_ADDRESS_POINTER == 0ul ). */
			ulSourceAddress = pxIPHeader->ulDestinationIPAddress;
		}
		else
		{
			ulSourceAddress = *ipLOCAL_IP_ADDRESS_POINTER;
		}
		pxIPHeader->ulDestinationIPAddress = pxIPHeader->ulSourceIPAddress;
		pxIPHeader->ulSourceIPAddress = ulSourceAddress;
		vFlip_16( pxTCPPacket->xTCPHeader.usSourcePort, pxTCPPacket->xTCPHeader.usDestinationPort );

		/* Just an increasing number. */
		pxIPHeader->usIdentification = FreeRTOS_htons( usPacketIdentifier );
		usPacketIdentifier++;
		pxIPHeader->usFragmentOffset = 0u;

		#if( ipconfigDRIVER_INCLUDED_TX_IP_CHECKSUM == 0 )
		{
			/* calculate the IP header checksum, in case the driver won't do that. */
			pxIPHeader->usHeaderChecksum = 0x00u;
			pxIPHeader->usHeaderChecksum = usGenerateChecksum( 0UL, ( uint8_t * ) &( pxIPHeader->ucVersionHeaderLength ), ipSIZE_OF_IPv4_HEADER );
			pxIPHeader->usHeaderChecksum = ~FreeRTOS_htons( pxIPHeader->usHeaderChecksum );

			/* calculate the TCP checksum for an outgoing packet. */
			usGenerateProtocolChecksum( (uint8_t*)pxTCPPacket, pxNetworkBuffer->xDataLength, pdTRUE );

			/* A calculated checksum of 0 must be inverted as 0 means the checksum
			is disabled. */
			if( pxTCPPacket->xTCPHeader.usChecksum == 0x00u )
			{
				pxTCPPacket->xTCPHeader.usChecksum = 0xffffU;
			}
		}
		#endif

	#if( ipconfigUSE_LINKED_RX_MESSAGES != 0 )
		pxNetworkBuffer->pxNextBuffer = NULL;
	#endif

		/* Important: tell NIC driver how many bytes must be sent. */
		pxNetworkBuffer->xDataLength = ulLen + ipSIZE_OF_ETH_HEADER;

		/* Fill in the destination MAC addresses. */
		memcpy( ( void * ) &( pxEthernetHeader->xDestinationAddress ), ( void * ) &( pxEthernetHeader->xSourceAddress ),
			sizeof( pxEthernetHeader->xDestinationAddress ) );

		/* The source MAC addresses is fixed to 'ipLOCAL_MAC_ADDRESS'. */
		memcpy( ( void * ) &( pxEthernetHeader->xSourceAddress) , ( void * ) ipLOCAL_MAC_ADDRESS, ( size_t ) ipMAC_ADDRESS_LENGTH_BYTES );

		#if defined( ipconfigETHERNET_MINIMUM_PACKET_BYTES )
		{
			if( pxNetworkBuffer->xDataLength < ( size_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES )
			{
			BaseType_t xIndex;

				for( xIndex = ( BaseType_t ) pxNetworkBuffer->xDataLength; xIndex < ( BaseType_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES; xIndex++ )
				{
					pxNetworkBuffer->pucEthernetBuffer[ xIndex ] = 0u;
				}
				pxNetworkBuffer->xDataLength = ( size_t ) ipconfigETHERNET_MINIMUM_PACKET_BYTES;
			}
		}
		#endif

		/* Send! */
		xNetworkInterfaceOutput( pxNetworkBuffer, xReleaseAfterSend );

		if( xReleaseAfterSend == pdFALSE )
		{
			/* Swap-back some fields, as pxBuffer probably points to a socket field
			containing the packet header. */
			vFlip_16( pxTCPPacket->xTCPHeader.usSourcePort, pxTCPPacket->xTCPHeader.usDestinationPort);
			pxTCPPacket->xIPHeader.ulSourceIPAddress = pxTCPPacket->xIPHeader.ulDestinationIPAddress;
			memcpy( pxEthernetHeader->xSourceAddress.ucBytes, pxEthernetHeader->xDestinationAddress.ucBytes, ( size_t ) ipMAC_ADDRESS_LENGTH_BYTES );
		}
		else
		{
			/* Nothing to do: the buffer has been passed to DMA and will be released after use */
		}
	} /* if( pxNetworkBuffer != NULL ) */
}
/*-----------------------------------------------------------*/

/*
 * The SYN event is very important: the sequence numbers, which have a kind of
 * random starting value, are being synchronised.  The sliding window manager
 * (in FreeRTOS_TCP_WIN.c) needs to know them, along with the Maximum Segment
 * Size (MSS) in use.
 */
static void prvTCPCreateWindow( FreeRTOS_Socket_t *pxSocket )
{
	if( xTCPWindowLoggingLevel )
		FreeRTOS_debug_printf( ( "Limits (using): TCP Win size %lu Water %lu <= %lu <= %lu\n",
			pxSocket->u.xTCP.uxRxWinSize * ipconfigTCP_MSS,
			pxSocket->u.xTCP.uxLittleSpace ,
			pxSocket->u.xTCP.uxEnoughSpace,
			pxSocket->u.xTCP.uxRxStreamSize ) );
	vTCPWindowCreate(
		&pxSocket->u.xTCP.xTCPWindow,
		ipconfigTCP_MSS * pxSocket->u.xTCP.uxRxWinSize,
		ipconfigTCP_MSS * pxSocket->u.xTCP.uxTxWinSize,
		pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber,
		pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber,
		( uint32_t ) pxSocket->u.xTCP.usInitMSS );
}
/*-----------------------------------------------------------*/

/*
 * Connecting sockets have a special state: eCONNECT_SYN.  In this phase,
 * the Ethernet address of the target will be found using ARP.  In case the
 * target IP address is not within the netmask, the hardware address of the
 * gateway will be used.
 */
static BaseType_t prvTCPPrepareConnect( FreeRTOS_Socket_t *pxSocket )
{
TCPPacket_t *pxTCPPacket;
IPHeader_t *pxIPHeader;
eARPLookupResult_t eReturned;
uint32_t ulRemoteIP;
MACAddress_t xEthAddress;
BaseType_t xReturn = pdTRUE;
uint32_t ulInitialSequenceNumber = 0;

	#if( ipconfigHAS_PRINTF != 0 )
	{
		/* Only necessary for nicer logging. */
		memset( xEthAddress.ucBytes, '\0', sizeof( xEthAddress.ucBytes ) );
	}
	#endif /* ipconfigHAS_PRINTF != 0 */

	ulRemoteIP = FreeRTOS_htonl( pxSocket->u.xTCP.ulRemoteIP );

	/* Determine the ARP cache status for the requested IP address. */
	eReturned = eARPGetCacheEntry( &( ulRemoteIP ), &( xEthAddress ) );

	switch( eReturned )
	{
	case eARPCacheHit:		/* An ARP table lookup found a valid entry. */
		break;				/* We can now prepare the SYN packet. */
	case eARPCacheMiss:		/* An ARP table lookup did not find a valid entry. */
	case eCantSendPacket:	/* There is no IP address, or an ARP is still in progress. */
	default:
		/* Count the number of times it couldn't find the ARP address. */
		pxSocket->u.xTCP.ucRepCount++;

		FreeRTOS_debug_printf( ( "ARP for %lxip (using %lxip): rc=%d %02X:%02X:%02X %02X:%02X:%02X\n",
			pxSocket->u.xTCP.ulRemoteIP,
			FreeRTOS_htonl( ulRemoteIP ),
			eReturned,
			xEthAddress.ucBytes[ 0 ],
			xEthAddress.ucBytes[ 1 ],
			xEthAddress.ucBytes[ 2 ],
			xEthAddress.ucBytes[ 3 ],
			xEthAddress.ucBytes[ 4 ],
			xEthAddress.ucBytes[ 5 ] ) );

		/* And issue a (new) ARP request */
		FreeRTOS_OutputARPRequest( ulRemoteIP );

		xReturn = pdFALSE;
	}

	if( xReturn != pdFALSE )
	{
		/* Get a difficult-to-predict initial sequence number for this 4-tuple. */
		ulInitialSequenceNumber = ulApplicationGetNextSequenceNumber( *ipLOCAL_IP_ADDRESS_POINTER,
																	  pxSocket->usLocalPort,
																	  pxSocket->u.xTCP.ulRemoteIP,
																	  pxSocket->u.xTCP.usRemotePort );

		/* Check for a random number generation error. */
		if( 0 == ulInitialSequenceNumber )
		{
			xReturn = pdFALSE;
		}
	}

	if( xReturn != pdFALSE )
	{
		/* The MAC-address of the peer (or gateway) has been found,
		now prepare the initial TCP packet and some fields in the socket. */
		pxTCPPacket = ( TCPPacket_t * )pxSocket->u.xTCP.xPacket.u.ucLastPacket;
		pxIPHeader = &pxTCPPacket->xIPHeader;

		/* reset the retry counter to zero. */
		pxSocket->u.xTCP.ucRepCount = 0u;

		/* And remember that the connect/SYN data are prepared. */
		pxSocket->u.xTCP.bits.bConnPrepared = pdTRUE_UNSIGNED;

		/* Now that the Ethernet address is known, the initial packet can be
		prepared. */
		memset( pxSocket->u.xTCP.xPacket.u.ucLastPacket, '\0', sizeof( pxSocket->u.xTCP.xPacket.u.ucLastPacket ) );

		/* Write the Ethernet address in Source, because it will be swapped by
		prvTCPReturnPacket(). */
		memcpy( &pxTCPPacket->xEthernetHeader.xSourceAddress, &xEthAddress, sizeof( xEthAddress ) );

		/* 'ipIPv4_FRAME_TYPE' is already in network-byte-order. */
		pxTCPPacket->xEthernetHeader.usFrameType = ipIPv4_FRAME_TYPE;

		pxIPHeader->ucVersionHeaderLength = 0x45u;
		pxIPHeader->usLength = FreeRTOS_htons( sizeof( TCPPacket_t ) - sizeof( pxTCPPacket->xEthernetHeader ) );
		pxIPHeader->ucTimeToLive = ( uint8_t ) ipconfigTCP_TIME_TO_LIVE;

		pxIPHeader->ucProtocol = ( uint8_t ) ipPROTOCOL_TCP;

		/* Addresses and ports will be stored swapped because prvTCPReturnPacket
		will swap them back while replying. */
		pxIPHeader->ulDestinationIPAddress = *ipLOCAL_IP_ADDRESS_POINTER;
		pxIPHeader->ulSourceIPAddress = FreeRTOS_htonl( pxSocket->u.xTCP.ulRemoteIP );

		pxTCPPacket->xTCPHeader.usSourcePort = FreeRTOS_htons( pxSocket->u.xTCP.usRemotePort );
		pxTCPPacket->xTCPHeader.usDestinationPort = FreeRTOS_htons( pxSocket->usLocalPort );

		/* We are actively connecting, so the peer's Initial Sequence Number (ISN)
		isn't known yet. */
		pxSocket->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber = 0ul;

		/* Start with ISN (Initial Sequence Number). */
		pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber = ulInitialSequenceNumber;

		/* The TCP header size is 20 bytes, divided by 4 equals 5, which is put in
		the high nibble of the TCP offset field. */
		pxTCPPacket->xTCPHeader.ucTCPOffset = 0x50u;

		/* Only set the SYN flag. */
		pxTCPPacket->xTCPHeader.ucTCPFlags = ipTCP_FLAG_SYN;

		/* Set the values of usInitMSS / usCurMSS for this socket. */
		prvSocketSetMSS( pxSocket );

		/* For now this is also the advertised window size. */
		pxSocket->u.xTCP.ulRxCurWinSize = pxSocket->u.xTCP.usInitMSS;

		/* The initial sequence numbers at our side are known.  Later
		vTCPWindowInit() will be called to fill in the peer's sequence numbers, but
		first wait for a SYN+ACK reply. */
		prvTCPCreateWindow( pxSocket );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

/* For logging and debugging: make a string showing the TCP flags
*/
#if( ipconfigHAS_DEBUG_PRINTF != 0 )

	static const char *prvTCPFlagMeaning( UBaseType_t xFlags)
	{
		static char retString[10];
		snprintf(retString, sizeof( retString ), "%c%c%c%c%c%c%c%c%c",
			( xFlags & ipTCP_FLAG_FIN )  ? 'F' : '.',	/* 0x0001: No more data from sender */
			( xFlags & ipTCP_FLAG_SYN )  ? 'S' : '.',	/* 0x0002: Synchronize sequence numbers */
			( xFlags & ipTCP_FLAG_RST )  ? 'R' : '.',	/* 0x0004: Reset the connection */
			( xFlags & ipTCP_FLAG_PSH )  ? 'P' : '.',	/* 0x0008: Push function: please push buffered data to the recv application */
			( xFlags & ipTCP_FLAG_ACK )  ? 'A' : '.',	/* 0x0010: Acknowledgment field is significant */
			( xFlags & ipTCP_FLAG_URG )  ? 'U' : '.',	/* 0x0020: Urgent pointer field is significant */
			( xFlags & ipTCP_FLAG_ECN )  ? 'E' : '.',	/* 0x0040: ECN-Echo */
			( xFlags & ipTCP_FLAG_CWR )  ? 'C' : '.',	/* 0x0080: Congestion Window Reduced */
			( xFlags & ipTCP_FLAG_NS )   ? 'N' : '.');	/* 0x0100: ECN-nonce concealment protection */
		return retString;
	}
	/*-----------------------------------------------------------*/

#endif /* ipconfigHAS_DEBUG_PRINTF */

/*
 * Parse the TCP option(s) received, if present.  It has already been verified
 * that: ((pxTCPHeader->ucTCPOffset & 0xf0) > 0x50), meaning that the TP header
 * is longer than the usual 20 (5 x 4) bytes.
 */
static void prvCheckOptions( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer )
{
TCPPacket_t * pxTCPPacket;
TCPHeader_t * pxTCPHeader;
const unsigned char *pucPtr;
const unsigned char *pucLast;
TCPWindow_t *pxTCPWindow;
UBaseType_t uxNewMSS;

	pxTCPPacket = ( TCPPacket_t * ) ( pxNetworkBuffer->pucEthernetBuffer );
	pxTCPHeader = &pxTCPPacket->xTCPHeader;

	/* A character pointer to iterate through the option data */
	pucPtr = pxTCPHeader->ucOptdata;
	pucLast = pucPtr + (((pxTCPHeader->ucTCPOffset >> 4) - 5) << 2);
	pxTCPWindow = &pxSocket->u.xTCP.xTCPWindow;

	/* Validate options size calculation. */
	if( pucLast > ( pxNetworkBuffer->pucEthernetBuffer + pxNetworkBuffer->xDataLength ) )
	{
		return;
	}

	/* The comparison with pucLast is only necessary in case the option data are
	corrupted, we don't like to run into invalid memory and crash. */
	while( pucPtr < pucLast )
	{
		UBaseType_t xRemainingOptionsBytes = pucLast - pucPtr;

		if( pucPtr[ 0 ] == TCP_OPT_END )
		{
			/* End of options. */
			break;
		}
		if( pucPtr[ 0 ] == TCP_OPT_NOOP)
		{
			/* NOP option, inserted to make the length a multiple of 4. */
			pucPtr++;
			continue;
		}

		/* Any other well-formed option must be at least two bytes: the option
		type byte followed by a length byte. */
		if( xRemainingOptionsBytes < 2 )
		{
			break;
		}
#if( ipconfigUSE_TCP_WIN != 0 )
		else if( pucPtr[ 0 ] == TCP_OPT_WSOPT )
		{
			/* Confirm that the option fits in the remaining buffer space. */
			if( ( xRemainingOptionsBytes < TCP_OPT_WSOPT_LEN ) || ( pucPtr[ 1 ] != TCP_OPT_WSOPT_LEN ) )
			{
				break;
			}

			pxSocket->u.xTCP.ucPeerWinScaleFactor = pucPtr[ 2 ];
			pxSocket->u.xTCP.bits.bWinScaling = pdTRUE_UNSIGNED;
			pucPtr += TCP_OPT_WSOPT_LEN;
		}
#endif	/* ipconfigUSE_TCP_WIN */
		else if( pucPtr[ 0 ] == TCP_OPT_MSS )
		{
			/* Confirm that the option fits in the remaining buffer space. */
			if( ( xRemainingOptionsBytes < TCP_OPT_MSS_LEN )|| ( pucPtr[ 1 ] != TCP_OPT_MSS_LEN ) )
			{
				break;
			}

			/* An MSS option with the correct option length.  FreeRTOS_htons()
			is not needed here because usChar2u16() already returns a host
			endian number. */
			uxNewMSS = usChar2u16( pucPtr + 2 );

			if( pxSocket->u.xTCP.usInitMSS != uxNewMSS )
			{
				/* Perform a basic check on the the new MSS. */
				if( uxNewMSS == 0 )
				{
					break;
				}

				FreeRTOS_debug_printf( ( "MSS change %u -> %lu\n", pxSocket->u.xTCP.usInitMSS, uxNewMSS ) );
			}

			if( pxSocket->u.xTCP.usInitMSS > uxNewMSS )
			{
				/* our MSS was bigger than the MSS of the other party: adapt it. */
				pxSocket->u.xTCP.bits.bMssChange = pdTRUE_UNSIGNED;
				if( ( pxTCPWindow != NULL ) && ( pxSocket->u.xTCP.usCurMSS > uxNewMSS ) )
				{
					/* The peer advertises a smaller MSS than this socket was
					using.  Use that as well. */
					FreeRTOS_debug_printf( ( "Change mss %d => %lu\n", pxSocket->u.xTCP.usCurMSS, uxNewMSS ) );
					pxSocket->u.xTCP.usCurMSS = ( uint16_t ) uxNewMSS;
				}
				pxTCPWindow->xSize.ulRxWindowLength = ( ( uint32_t ) uxNewMSS ) * ( pxTCPWindow->xSize.ulRxWindowLength / ( ( uint32_t ) uxNewMSS ) );
				pxTCPWindow->usMSSInit = ( uint16_t ) uxNewMSS;
				pxTCPWindow->usMSS = ( uint16_t ) uxNewMSS;
				pxSocket->u.xTCP.usInitMSS = ( uint16_t ) uxNewMSS;
				pxSocket->u.xTCP.usCurMSS = ( uint16_t ) uxNewMSS;
			}

			#if( ipconfigUSE_TCP_WIN != 1 )
				/* Without scaled windows, MSS is the only interesting option. */
				break;
			#else
				/* Or else we continue to check another option: selective ACK. */
				pucPtr += TCP_OPT_MSS_LEN;
			#endif	/* ipconfigUSE_TCP_WIN != 1 */
		}
		else
		{
			/* All other options have a length field, so that we easily
			can skip past them. */
			unsigned char len = pucPtr[ 1 ];
			if( ( len < 2 ) || ( len > xRemainingOptionsBytes ) )
			{
				/* If the length field is too small or too big, the options are malformed.
				Don't process them further. */
				break;
			}

			#if( ipconfigUSE_TCP_WIN == 1 )
			{
				/* Selective ACK: the peer has received a packet but it is missing earlier
				packets.  At least this packet does not need retransmission anymore
				ulTCPWindowTxSack( ) takes care of this administration. */
				if( pucPtr[0] == TCP_OPT_SACK_A )
				{
					len -= 2;
					pucPtr += 2;

					while( len >= 8 )
					{
					uint32_t ulFirst = ulChar2u32( pucPtr );
					uint32_t ulLast  = ulChar2u32( pucPtr + 4 );
					uint32_t ulCount = ulTCPWindowTxSack( &pxSocket->u.xTCP.xTCPWindow, ulFirst, ulLast );
						/* ulTCPWindowTxSack( ) returns the number of bytes which have been acked
						starting from the head position.
						Advance the tail pointer in txStream. */
						if( ( pxSocket->u.xTCP.txStream  != NULL ) && ( ulCount > 0 ) )
						{
							/* Just advancing the tail index, 'ulCount' bytes have been confirmed. */
							uxStreamBufferGet( pxSocket->u.xTCP.txStream, 0, NULL, ( size_t ) ulCount, pdFALSE );
							pxSocket->xEventBits |= eSOCKET_SEND;

							#if ipconfigSUPPORT_SELECT_FUNCTION == 1
							{
								if( pxSocket->xSelectBits & eSELECT_WRITE )
								{
									/* The field 'xEventBits' is used to store regular socket events (at most 8),
									as well as 'select events', which will be left-shifted */
									pxSocket->xEventBits |= ( eSELECT_WRITE << SOCKET_EVENT_BIT_COUNT );
								}
							}
							#endif

							/* In case the socket owner has installed an OnSent handler,
							call it now. */
							#if( ipconfigUSE_CALLBACKS == 1 )
							{
								if( ipconfigIS_VALID_PROG_ADDRESS( pxSocket->u.xTCP.pxHandleSent ) )
								{
									pxSocket->u.xTCP.pxHandleSent( (Socket_t *)pxSocket, ulCount );
								}
							}
							#endif /* ipconfigUSE_CALLBACKS == 1  */
						}
						pucPtr += 8;
						len -= 8;
					}
					/* len should be 0 by now. */
				}
			}
			#endif	/* ipconfigUSE_TCP_WIN == 1 */

			pucPtr += len;
		}
	}
}
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP_WIN != 0 )

	static uint8_t prvWinScaleFactor( FreeRTOS_Socket_t *pxSocket )
	{
	size_t uxWinSize;
	uint8_t ucFactor;

		/* 'xTCP.uxRxWinSize' is the size of the reception window in units of MSS. */
		uxWinSize = pxSocket->u.xTCP.uxRxWinSize * ( size_t ) pxSocket->u.xTCP.usInitMSS;
		ucFactor = 0u;
		while( uxWinSize > 0xfffful )
		{
			/* Divide by two and increase the binary factor by 1. */
			uxWinSize >>= 1;
			ucFactor++;
		}

		FreeRTOS_debug_printf( ( "prvWinScaleFactor: uxRxWinSize %lu MSS %lu Factor %u\n",
			pxSocket->u.xTCP.uxRxWinSize,
			pxSocket->u.xTCP.usInitMSS,
			ucFactor ) );

		return ucFactor;
	}

#endif
/*-----------------------------------------------------------*/

/*
 * When opening a TCP connection, while SYN's are being sent, the  parties may
 * communicate what MSS (Maximum Segment Size) they intend to use.   MSS is the
 * nett size of the payload, always smaller than MTU.
*/
static UBaseType_t prvSetSynAckOptions( FreeRTOS_Socket_t *pxSocket, TCPPacket_t * pxTCPPacket )
{
TCPHeader_t *pxTCPHeader = &pxTCPPacket->xTCPHeader;
uint16_t usMSS = pxSocket->u.xTCP.usInitMSS;
UBaseType_t uxOptionsLength;

	/* We send out the TCP Maximum Segment Size option with our SYN[+ACK]. */

	pxTCPHeader->ucOptdata[ 0 ] = ( uint8_t ) TCP_OPT_MSS;
	pxTCPHeader->ucOptdata[ 1 ] = ( uint8_t ) TCP_OPT_MSS_LEN;
	pxTCPHeader->ucOptdata[ 2 ] = ( uint8_t ) ( usMSS >> 8 );
	pxTCPHeader->ucOptdata[ 3 ] = ( uint8_t ) ( usMSS & 0xffu );

	#if( ipconfigUSE_TCP_WIN != 0 )
	{
		pxSocket->u.xTCP.ucMyWinScaleFactor = prvWinScaleFactor( pxSocket );

		pxTCPHeader->ucOptdata[ 4 ] = TCP_OPT_NOOP;
		pxTCPHeader->ucOptdata[ 5 ] = ( uint8_t ) ( TCP_OPT_WSOPT );
		pxTCPHeader->ucOptdata[ 6 ] = ( uint8_t ) ( TCP_OPT_WSOPT_LEN );
		pxTCPHeader->ucOptdata[ 7 ] = ( uint8_t ) pxSocket->u.xTCP.ucMyWinScaleFactor;
		uxOptionsLength = 8u;
	}
	#else
	{
		uxOptionsLength = 4u;
	}
	#endif

	#if( ipconfigUSE_TCP_WIN == 0 )
	{
		return uxOptionsLength;
	}
	#else
	{
		pxTCPHeader->ucOptdata[ uxOptionsLength + 0 ] = TCP_OPT_NOOP;
		pxTCPHeader->ucOptdata[ uxOptionsLength + 1 ] = TCP_OPT_NOOP;
		pxTCPHeader->ucOptdata[ uxOptionsLength + 2 ] = TCP_OPT_SACK_P;	/* 4: Sack-Permitted Option. */
		pxTCPHeader->ucOptdata[ uxOptionsLength + 3 ] = 2;	/* 2: length of this option. */
		uxOptionsLength += 4u;

		return uxOptionsLength; /* bytes, not words. */
	}
	#endif	/* ipconfigUSE_TCP_WIN == 0 */
}

/*
 * For anti-hanging protection and TCP keep-alive messages.  Called in two
 * places: after receiving a packet and after a state change.  The socket's
 * alive timer may be reset.
 */
static void prvTCPTouchSocket( FreeRTOS_Socket_t *pxSocket )
{
	#if( ipconfigTCP_HANG_PROTECTION == 1 )
	{
		pxSocket->u.xTCP.xLastActTime = xTaskGetTickCount( );
	}
	#endif

	#if( ipconfigTCP_KEEP_ALIVE == 1 )
	{
		pxSocket->u.xTCP.bits.bWaitKeepAlive = pdFALSE_UNSIGNED;
		pxSocket->u.xTCP.bits.bSendKeepAlive = pdFALSE_UNSIGNED;
		pxSocket->u.xTCP.ucKeepRepCount = 0u;
		pxSocket->u.xTCP.xLastAliveTime = xTaskGetTickCount();
	}
	#endif

	( void ) pxSocket;
}
/*-----------------------------------------------------------*/

/*
 * Changing to a new state. Centralised here to do specific actions such as
 * resetting the alive timer, calling the user's OnConnect handler to notify
 * that a socket has got (dis)connected, and setting bit to unblock a call to
 * FreeRTOS_select()
 */
void vTCPStateChange( FreeRTOS_Socket_t *pxSocket, enum eTCP_STATE eTCPState )
{
FreeRTOS_Socket_t *xParent = NULL;
BaseType_t bBefore = ( BaseType_t ) NOW_CONNECTED( pxSocket->u.xTCP.ucTCPState );	/* Was it connected ? */
BaseType_t bAfter  = ( BaseType_t ) NOW_CONNECTED( eTCPState );						/* Is it connected now ? */
#if( ipconfigHAS_DEBUG_PRINTF != 0 )
	BaseType_t xPreviousState = ( BaseType_t ) pxSocket->u.xTCP.ucTCPState;
#endif
#if( ipconfigUSE_CALLBACKS == 1 )
	FreeRTOS_Socket_t *xConnected = NULL;
#endif

	/* Has the connected status changed? */
	if( bBefore != bAfter )
	{
		/* Is the socket connected now ? */
		if( bAfter != pdFALSE )
		{
			/* if bPassQueued is true, this socket is an orphan until it gets connected. */
			if( pxSocket->u.xTCP.bits.bPassQueued != pdFALSE_UNSIGNED )
			{
				/* Now that it is connected, find it's parent. */
				if( pxSocket->u.xTCP.bits.bReuseSocket != pdFALSE_UNSIGNED )
				{
					xParent = pxSocket;
				}
				else
				{
					xParent = pxSocket->u.xTCP.pxPeerSocket;
					configASSERT( xParent != NULL );
				}
				if( xParent != NULL )
				{
					if( xParent->u.xTCP.pxPeerSocket == NULL )
					{
						xParent->u.xTCP.pxPeerSocket = pxSocket;
					}

					xParent->xEventBits |= eSOCKET_ACCEPT;

					#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
					{
						/* Library support FreeRTOS_select().  Receiving a new
						connection is being translated as a READ event. */
						if( ( xParent->xSelectBits & eSELECT_READ ) != 0 )
						{
							xParent->xEventBits |= ( eSELECT_READ << SOCKET_EVENT_BIT_COUNT );
						}
					}
					#endif

					#if( ipconfigUSE_CALLBACKS == 1 )
					{
						if( ( ipconfigIS_VALID_PROG_ADDRESS( xParent->u.xTCP.pxHandleConnected ) != pdFALSE ) &&
							( xParent->u.xTCP.bits.bReuseSocket == pdFALSE_UNSIGNED ) )
						{
							/* The listening socket does not become connected itself, in stead
							a child socket is created.
							Postpone a call the OnConnect event until the end of this function. */
							xConnected = xParent;
						}
					}
					#endif
				}

				/* Don't need to access the parent socket anymore, so the
				reference 'pxPeerSocket' may be cleared. */
				pxSocket->u.xTCP.pxPeerSocket = NULL;
				pxSocket->u.xTCP.bits.bPassQueued = pdFALSE_UNSIGNED;

				/* When true, this socket may be returned in a call to accept(). */
				pxSocket->u.xTCP.bits.bPassAccept = pdTRUE_UNSIGNED;
			}
			else
			{
				pxSocket->xEventBits |= eSOCKET_CONNECT;

				#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
				{
					if( pxSocket->xSelectBits & eSELECT_WRITE )
					{
						pxSocket->xEventBits |= ( eSELECT_WRITE << SOCKET_EVENT_BIT_COUNT );
					}
				}
				#endif
			}
		}
		else  /* bAfter == pdFALSE, connection is closed. */
		{
			/* Notify/wake-up the socket-owner by setting a semaphore. */
			pxSocket->xEventBits |= eSOCKET_CLOSED;

			#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
			{
				if( ( pxSocket->xSelectBits & eSELECT_EXCEPT ) != 0 )
				{
					pxSocket->xEventBits |= ( eSELECT_EXCEPT << SOCKET_EVENT_BIT_COUNT );
				}
			}
			#endif
		}
		#if( ipconfigUSE_CALLBACKS == 1 )
		{
			if( ( ipconfigIS_VALID_PROG_ADDRESS( pxSocket->u.xTCP.pxHandleConnected ) != pdFALSE ) && ( xConnected == NULL ) )
			{
				/* The 'connected' state has changed, call the user handler. */
				xConnected = pxSocket;
			}
		}
		#endif /* ipconfigUSE_CALLBACKS */

		if( prvTCPSocketIsActive( ( UBaseType_t ) pxSocket->u.xTCP.ucTCPState ) == pdFALSE )
		{
			/* Now the socket isn't in an active state anymore so it
			won't need further attention of the IP-task.
			Setting time-out to zero means that the socket won't get checked during
			timer events. */
			pxSocket->u.xTCP.usTimeout = 0u;
		}
	}
	else
	{
		if( eTCPState == eCLOSED )
		{
			/* Socket goes to status eCLOSED because of a RST.
			When nobody owns the socket yet, delete it. */
			if( ( pxSocket->u.xTCP.bits.bPassQueued != pdFALSE_UNSIGNED ) ||
				( pxSocket->u.xTCP.bits.bPassAccept != pdFALSE_UNSIGNED ) )
			{
				FreeRTOS_debug_printf( ( "vTCPStateChange: Closing socket\n" ) );
				if( pxSocket->u.xTCP.bits.bReuseSocket == pdFALSE_UNSIGNED )
				{
					FreeRTOS_closesocket( pxSocket );
				}
			}
		}
	}

	/* Fill in the new state. */
	pxSocket->u.xTCP.ucTCPState = ( uint8_t ) eTCPState;

	/* touch the alive timers because moving to another state. */
	prvTCPTouchSocket( pxSocket );

	#if( ipconfigHAS_DEBUG_PRINTF == 1 )
	{
	if( ( xTCPWindowLoggingLevel >= 0 ) && ( ipconfigTCP_MAY_LOG_PORT( pxSocket->usLocalPort ) != pdFALSE ) )
		FreeRTOS_debug_printf( ( "Socket %d -> %lxip:%u State %s->%s\n",
			pxSocket->usLocalPort,
			pxSocket->u.xTCP.ulRemoteIP,
			pxSocket->u.xTCP.usRemotePort,
			FreeRTOS_GetTCPStateName( ( UBaseType_t ) xPreviousState ),
			FreeRTOS_GetTCPStateName( ( UBaseType_t ) eTCPState ) ) );
	}
	#endif /* ipconfigHAS_DEBUG_PRINTF */

	#if( ipconfigUSE_CALLBACKS == 1 )
	{
		if( xConnected != NULL )
		{
			/* The 'connected' state has changed, call the OnConnect handler of the parent. */
			xConnected->u.xTCP.pxHandleConnected( ( Socket_t * ) xConnected, bAfter );
		}
	}
	#endif
	if( xParent != NULL )
	{
		vSocketWakeUpUser( xParent );
	}
}
/*-----------------------------------------------------------*/

static NetworkBufferDescriptor_t *prvTCPBufferResize( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer,
	int32_t lDataLen, UBaseType_t uxOptionsLength )
{
NetworkBufferDescriptor_t *pxReturn;
int32_t lNeeded;
BaseType_t xResize;

	if( xBufferAllocFixedSize != pdFALSE )
	{
		/* Network buffers are created with a fixed size and can hold the largest
		MTU. */
		lNeeded = ( int32_t ) ipTOTAL_ETHERNET_FRAME_SIZE;
		/* and therefore, the buffer won't be too small.
		Only ask for a new network buffer in case none was supplied. */
		xResize = ( pxNetworkBuffer == NULL );
	}
	else
	{
		/* Network buffers are created with a variable size. See if it must
		grow. */
		lNeeded = FreeRTOS_max_int32( ( int32_t ) sizeof( pxSocket->u.xTCP.xPacket.u.ucLastPacket ),
			( int32_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + uxOptionsLength ) + lDataLen );
		/* In case we were called from a TCP timer event, a buffer must be
		created.  Otherwise, test 'xDataLength' of the provided buffer. */
		xResize = ( pxNetworkBuffer == NULL ) || ( pxNetworkBuffer->xDataLength < (size_t)lNeeded );
	}

	if( xResize != pdFALSE )
	{
		/* The caller didn't provide a network buffer or the provided buffer is
		too small.  As we must send-out a data packet, a buffer will be created
		here. */
		pxReturn = pxGetNetworkBufferWithDescriptor( ( uint32_t ) lNeeded, 0u );

		if( pxReturn != NULL )
		{
			/* Set the actual packet size, in case the returned buffer is larger. */
			pxReturn->xDataLength = lNeeded;

			/* Copy the existing data to the new created buffer. */
			if( pxNetworkBuffer )
			{
				/* Either from the previous buffer... */
				memcpy( pxReturn->pucEthernetBuffer, pxNetworkBuffer->pucEthernetBuffer, pxNetworkBuffer->xDataLength );

				/* ...and release it. */
				vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
			}
			else
			{
				/* Or from the socket field 'xTCP.xPacket'. */
				memcpy( pxReturn->pucEthernetBuffer, pxSocket->u.xTCP.xPacket.u.ucLastPacket, sizeof( pxSocket->u.xTCP.xPacket.u.ucLastPacket ) );
			}
		}
	}
	else
	{
		/* xResize is false, the network buffer provided was big enough. */
		pxReturn = pxNetworkBuffer;

		/* Thanks to Andrey Ivanov from swissEmbedded for reporting that the
		xDataLength member must get the correct length too! */
		pxNetworkBuffer->xDataLength = ( size_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + uxOptionsLength ) + ( size_t ) lDataLen;
	}

	return pxReturn;
}
/*-----------------------------------------------------------*/

/*
 * Prepare an outgoing message, in case anything has to be sent.
 */
static int32_t prvTCPPrepareSend( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer, UBaseType_t uxOptionsLength )
{
int32_t lDataLen;
uint8_t *pucEthernetBuffer, *pucSendData;
TCPPacket_t *pxTCPPacket;
size_t uxOffset;
uint32_t ulDataGot, ulDistance;
TCPWindow_t *pxTCPWindow;
NetworkBufferDescriptor_t *pxNewBuffer;
int32_t lStreamPos;

	if( ( *ppxNetworkBuffer ) != NULL )
	{
		/* A network buffer descriptor was already supplied */
		pucEthernetBuffer = ( *ppxNetworkBuffer )->pucEthernetBuffer;
	}
	else
	{
		/* For now let it point to the last packet header */
		pucEthernetBuffer = pxSocket->u.xTCP.xPacket.u.ucLastPacket;
	}

	pxTCPPacket = ( TCPPacket_t * ) ( pucEthernetBuffer );
	pxTCPWindow = &pxSocket->u.xTCP.xTCPWindow;
	lDataLen = 0;
	lStreamPos = 0;
	pxTCPPacket->xTCPHeader.ucTCPFlags |= ipTCP_FLAG_ACK;

	if( pxSocket->u.xTCP.txStream != NULL )
	{
		/* ulTCPWindowTxGet will return the amount of data which may be sent
		along with the position in the txStream.
		Why check for MSS > 1 ?
		Because some TCP-stacks (like uIP) use it for flow-control. */
		if( pxSocket->u.xTCP.usCurMSS > 1u )
		{
			lDataLen = ( int32_t ) ulTCPWindowTxGet( pxTCPWindow, pxSocket->u.xTCP.ulWindowSize, &lStreamPos );
		}

		if( lDataLen > 0 )
		{
			/* Check if the current network buffer is big enough, if not,
			resize it. */
			pxNewBuffer = prvTCPBufferResize( pxSocket, *ppxNetworkBuffer, lDataLen, uxOptionsLength );

			if( pxNewBuffer != NULL )
			{
				*ppxNetworkBuffer = pxNewBuffer;
				pucEthernetBuffer = pxNewBuffer->pucEthernetBuffer;
				pxTCPPacket = ( TCPPacket_t * ) ( pucEthernetBuffer );

				pucSendData = pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + uxOptionsLength;

				/* Translate the position in txStream to an offset from the tail
				marker. */
				uxOffset = uxStreamBufferDistance( pxSocket->u.xTCP.txStream, pxSocket->u.xTCP.txStream->uxTail, ( size_t ) lStreamPos );

				/* Here data is copied from the txStream in 'peek' mode.  Only
				when the packets are acked, the tail marker will be updated. */
				ulDataGot = ( uint32_t ) uxStreamBufferGet( pxSocket->u.xTCP.txStream, uxOffset, pucSendData, ( size_t ) lDataLen, pdTRUE );

				#if( ipconfigHAS_DEBUG_PRINTF != 0 )
				{
					if( ulDataGot != ( uint32_t ) lDataLen )
					{
						FreeRTOS_debug_printf( ( "uxStreamBufferGet: pos %lu offs %lu only %lu != %lu\n",
							lStreamPos, uxOffset, ulDataGot, lDataLen ) );
					}
				}
				#endif

				/* If the owner of the socket requests a closure, add the FIN
				flag to the last packet. */
				if( ( pxSocket->u.xTCP.bits.bCloseRequested != pdFALSE_UNSIGNED ) && ( pxSocket->u.xTCP.bits.bFinSent == pdFALSE_UNSIGNED ) )
				{
					ulDistance = ( uint32_t ) uxStreamBufferDistance( pxSocket->u.xTCP.txStream, ( size_t ) lStreamPos, pxSocket->u.xTCP.txStream->uxHead );

					if( ulDistance == ulDataGot )
					{
						#if (ipconfigHAS_DEBUG_PRINTF == 1)
						{
						/* the order of volatile accesses is undefined
							so such workaround */
							size_t uxHead = pxSocket->u.xTCP.txStream->uxHead;
							size_t uxMid = pxSocket->u.xTCP.txStream->uxMid;
							size_t uxTail = pxSocket->u.xTCP.txStream->uxTail;

							FreeRTOS_debug_printf( ( "CheckClose %lu <= %lu (%lu <= %lu <= %lu)\n", ulDataGot, ulDistance,
								uxTail, uxMid, uxHead ) );
						}
						#endif
						/* Although the socket sends a FIN, it will stay in
						ESTABLISHED until all current data has been received or
						delivered. */
						pxTCPPacket->xTCPHeader.ucTCPFlags |= ipTCP_FLAG_FIN;
						pxTCPWindow->tx.ulFINSequenceNumber = pxTCPWindow->ulOurSequenceNumber + ( uint32_t ) lDataLen;
						pxSocket->u.xTCP.bits.bFinSent = pdTRUE_UNSIGNED;
					}
				}
			}
			else
			{
				lDataLen = -1;
			}
		}
	}

	if( ( lDataLen >= 0 ) && ( pxSocket->u.xTCP.ucTCPState == eESTABLISHED ) )
	{
		/* See if the socket owner wants to shutdown this connection. */
		if( ( pxSocket->u.xTCP.bits.bUserShutdown != pdFALSE_UNSIGNED ) &&
			( xTCPWindowTxDone( pxTCPWindow ) != pdFALSE ) )
		{
			pxSocket->u.xTCP.bits.bUserShutdown = pdFALSE_UNSIGNED;
			pxTCPPacket->xTCPHeader.ucTCPFlags |= ipTCP_FLAG_FIN;
			pxSocket->u.xTCP.bits.bFinSent = pdTRUE_UNSIGNED;
			pxSocket->u.xTCP.bits.bWinChange = pdTRUE_UNSIGNED;
			pxTCPWindow->tx.ulFINSequenceNumber = pxTCPWindow->tx.ulCurrentSequenceNumber;
			vTCPStateChange( pxSocket, eFIN_WAIT_1 );
		}

		#if( ipconfigTCP_KEEP_ALIVE != 0 )
		{
			if( pxSocket->u.xTCP.ucKeepRepCount > 3u )
			{
				FreeRTOS_debug_printf( ( "keep-alive: giving up %lxip:%u\n",
					pxSocket->u.xTCP.ulRemoteIP,			/* IP address of remote machine. */
					pxSocket->u.xTCP.usRemotePort ) );	/* Port on remote machine. */
				vTCPStateChange( pxSocket, eCLOSE_WAIT );
				lDataLen = -1;
			}
			if( ( lDataLen == 0 ) && ( pxSocket->u.xTCP.bits.bWinChange == pdFALSE_UNSIGNED ) )
			{
				/* If there is no data to be sent, and no window-update message,
				we might want to send a keep-alive message. */
				TickType_t xAge = xTaskGetTickCount( ) - pxSocket->u.xTCP.xLastAliveTime;
				TickType_t xMax;
				xMax = ( ( TickType_t ) ipconfigTCP_KEEP_ALIVE_INTERVAL * configTICK_RATE_HZ );
				if( pxSocket->u.xTCP.ucKeepRepCount )
				{
					xMax = ( 3u * configTICK_RATE_HZ );
				}
				if( xAge > xMax )
				{
					pxSocket->u.xTCP.xLastAliveTime = xTaskGetTickCount( );
					if( xTCPWindowLoggingLevel )
						FreeRTOS_debug_printf( ( "keep-alive: %lxip:%u count %u\n",
							pxSocket->u.xTCP.ulRemoteIP,
							pxSocket->u.xTCP.usRemotePort,
							pxSocket->u.xTCP.ucKeepRepCount ) );
					pxSocket->u.xTCP.bits.bSendKeepAlive = pdTRUE_UNSIGNED;
					pxSocket->u.xTCP.usTimeout = ( ( uint16_t ) pdMS_TO_TICKS( 2500 ) );
					pxSocket->u.xTCP.ucKeepRepCount++;
				}
			}
		}
		#endif /* ipconfigTCP_KEEP_ALIVE */
	}

	/* Anything to send, a change of the advertised window size, or maybe send a
	keep-alive message? */
	if( ( lDataLen > 0 ) ||
		( pxSocket->u.xTCP.bits.bWinChange != pdFALSE_UNSIGNED ) ||
		( pxSocket->u.xTCP.bits.bSendKeepAlive != pdFALSE_UNSIGNED ) )
	{
		pxTCPPacket->xTCPHeader.ucTCPFlags &= ( ( uint8_t ) ~ipTCP_FLAG_PSH );
		pxTCPPacket->xTCPHeader.ucTCPOffset = ( uint8_t )( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) << 2 );

		pxTCPPacket->xTCPHeader.ucTCPFlags |= ( uint8_t ) ipTCP_FLAG_ACK;

		if( lDataLen != 0l )
		{
			pxTCPPacket->xTCPHeader.ucTCPFlags |= ( uint8_t ) ipTCP_FLAG_PSH;
		}

		lDataLen += ( int32_t ) ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + uxOptionsLength );
	}

	return lDataLen;
}
/*-----------------------------------------------------------*/

/*
 * Calculate after how much time this socket needs to be checked again.
 */
static TickType_t prvTCPNextTimeout ( FreeRTOS_Socket_t *pxSocket )
{
TickType_t ulDelayMs = ( TickType_t ) tcpMAXIMUM_TCP_WAKEUP_TIME_MS;

	if( pxSocket->u.xTCP.ucTCPState == eCONNECT_SYN )
	{
		/* The socket is actively connecting to a peer. */
		if( pxSocket->u.xTCP.bits.bConnPrepared )
		{
			/* Ethernet address has been found, use progressive timeout for
			active connect(). */
			if( pxSocket->u.xTCP.ucRepCount < 3u )
			{
				ulDelayMs = ( 3000UL << ( pxSocket->u.xTCP.ucRepCount - 1u ) );
			}
			else
			{
				ulDelayMs = 11000UL;
			}
		}
		else
		{
			/* Still in the ARP phase: check every half second. */
			ulDelayMs = 500UL;
		}

		FreeRTOS_debug_printf( ( "Connect[%lxip:%u]: next timeout %u: %lu ms\n",
			pxSocket->u.xTCP.ulRemoteIP, pxSocket->u.xTCP.usRemotePort,
			pxSocket->u.xTCP.ucRepCount, ulDelayMs ) );
		pxSocket->u.xTCP.usTimeout = ( uint16_t )pdMS_TO_MIN_TICKS( ulDelayMs );
	}
	else if( pxSocket->u.xTCP.usTimeout == 0u )
	{
		/* Let the sliding window mechanism decide what time-out is appropriate. */
		BaseType_t xResult = xTCPWindowTxHasData( &pxSocket->u.xTCP.xTCPWindow, pxSocket->u.xTCP.ulWindowSize, &ulDelayMs );
		if( ulDelayMs == 0u )
		{
			if( xResult != ( BaseType_t )0 )
			{
				ulDelayMs = 1UL;
			}
			else
			{
				ulDelayMs = tcpMAXIMUM_TCP_WAKEUP_TIME_MS;
			}
		}
		else
		{
			/* ulDelayMs contains the time to wait before a re-transmission. */
		}
		pxSocket->u.xTCP.usTimeout = ( uint16_t )pdMS_TO_MIN_TICKS( ulDelayMs );
	}
	else
	{
		/* field '.usTimeout' has already been set (by the
		keep-alive/delayed-ACK mechanism). */
	}

	/* Return the number of clock ticks before the timer expires. */
	return ( TickType_t ) pxSocket->u.xTCP.usTimeout;
}
/*-----------------------------------------------------------*/

static void prvTCPAddTxData( FreeRTOS_Socket_t *pxSocket )
{
int32_t lCount, lLength;

	/* A txStream has been created already, see if the socket has new data for
	the sliding window.

	uxStreamBufferMidSpace() returns the distance between rxHead and rxMid.  It contains new
	Tx data which has not been passed to the sliding window yet.  The oldest
	data not-yet-confirmed can be found at rxTail. */
	lLength = ( int32_t ) uxStreamBufferMidSpace( pxSocket->u.xTCP.txStream );

	if( lLength > 0 )
	{
		/* All data between txMid and rxHead will now be passed to the sliding
		window manager, so it can start transmitting them.

		Hand over the new data to the sliding window handler.  It will be
		split-up in chunks of 1460 bytes each (or less, depending on
		ipconfigTCP_MSS). */
		lCount = lTCPWindowTxAdd(	&pxSocket->u.xTCP.xTCPWindow,
								( uint32_t ) lLength,
								( int32_t ) pxSocket->u.xTCP.txStream->uxMid,
								( int32_t ) pxSocket->u.xTCP.txStream->LENGTH );

		/* Move the rxMid pointer forward up to rxHead. */
		if( lCount > 0 )
		{
			vStreamBufferMoveMid( pxSocket->u.xTCP.txStream, ( size_t ) lCount );
		}
	}
}
/*-----------------------------------------------------------*/

/*
 * prvTCPHandleFin() will be called to handle socket closure
 * The Closure starts when either a FIN has been received and accepted,
 * Or when the socket has sent a FIN flag to the peer
 * Before being called, it has been checked that both reception and transmission
 * are complete.
 */
static BaseType_t prvTCPHandleFin( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer )
{
TCPPacket_t *pxTCPPacket = ( TCPPacket_t * ) ( pxNetworkBuffer->pucEthernetBuffer );
TCPHeader_t *pxTCPHeader = &pxTCPPacket->xTCPHeader;
uint8_t ucTCPFlags = pxTCPHeader->ucTCPFlags;
TCPWindow_t *pxTCPWindow = &pxSocket->u.xTCP.xTCPWindow;
BaseType_t xSendLength = 0;
uint32_t ulAckNr = FreeRTOS_ntohl( pxTCPHeader->ulAckNr );

	if( ( ucTCPFlags & ipTCP_FLAG_FIN ) != 0u )
	{
		pxTCPWindow->rx.ulCurrentSequenceNumber = pxTCPWindow->rx.ulFINSequenceNumber + 1u;
	}
	if( pxSocket->u.xTCP.bits.bFinSent == pdFALSE_UNSIGNED )
	{
		/* We haven't yet replied with a FIN, do so now. */
		pxTCPWindow->tx.ulFINSequenceNumber = pxTCPWindow->tx.ulCurrentSequenceNumber;
		pxSocket->u.xTCP.bits.bFinSent = pdTRUE_UNSIGNED;
	}
	else
	{
		/* We did send a FIN already, see if it's ACK'd. */
		if( ulAckNr == pxTCPWindow->tx.ulFINSequenceNumber + 1u )
		{
			pxSocket->u.xTCP.bits.bFinAcked = pdTRUE_UNSIGNED;
		}
	}

	if( pxSocket->u.xTCP.bits.bFinAcked == pdFALSE_UNSIGNED )
	{
		pxTCPWindow->tx.ulCurrentSequenceNumber = pxTCPWindow->tx.ulFINSequenceNumber;
		pxTCPHeader->ucTCPFlags = ipTCP_FLAG_ACK | ipTCP_FLAG_FIN;

		/* And wait for the final ACK. */
		vTCPStateChange( pxSocket, eLAST_ACK );
	}
	else
	{
		/* Our FIN has been ACK'd, the outgoing sequence number is now fixed. */
		pxTCPWindow->tx.ulCurrentSequenceNumber = pxTCPWindow->tx.ulFINSequenceNumber + 1u;
		if( pxSocket->u.xTCP.bits.bFinRecv == pdFALSE_UNSIGNED )
		{
			/* We have sent out a FIN but the peer hasn't replied with a FIN
			yet. Do nothing for the moment. */
			pxTCPHeader->ucTCPFlags = 0u;
		}
		else
		{
			if( pxSocket->u.xTCP.bits.bFinLast == pdFALSE_UNSIGNED )
			{
				/* This is the third of the three-way hand shake: the last
				ACK. */
				pxTCPHeader->ucTCPFlags = ipTCP_FLAG_ACK;
			}
			else
			{
				/* The other party started the closure, so we just wait for the
				last ACK. */
				pxTCPHeader->ucTCPFlags = 0u;
			}

			/* And wait for the user to close this socket. */
			vTCPStateChange( pxSocket, eCLOSE_WAIT );
		}
	}

	pxTCPWindow->ulOurSequenceNumber = pxTCPWindow->tx.ulCurrentSequenceNumber;

	if( pxTCPHeader->ucTCPFlags != 0u )
	{
		xSendLength = ( BaseType_t ) ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + pxTCPWindow->ucOptionLength );
	}

	pxTCPHeader->ucTCPOffset = ( uint8_t ) ( ( ipSIZE_OF_TCP_HEADER + pxTCPWindow->ucOptionLength ) << 2 );

	if( xTCPWindowLoggingLevel != 0 )
	{
		FreeRTOS_debug_printf( ( "TCP: send FIN+ACK (ack %lu, cur/nxt %lu/%lu) ourSeqNr %lu | Rx %lu\n",
			ulAckNr - pxTCPWindow->tx.ulFirstSequenceNumber,
			pxTCPWindow->tx.ulCurrentSequenceNumber - pxTCPWindow->tx.ulFirstSequenceNumber,
			pxTCPWindow->ulNextTxSequenceNumber - pxTCPWindow->tx.ulFirstSequenceNumber,
			pxTCPWindow->ulOurSequenceNumber - pxTCPWindow->tx.ulFirstSequenceNumber,
			pxTCPWindow->rx.ulCurrentSequenceNumber - pxTCPWindow->rx.ulFirstSequenceNumber ) );
	}

	return xSendLength;
}
/*-----------------------------------------------------------*/

/*
 * prvCheckRxData(): called from prvTCPHandleState()
 *
 * The first thing that will be done is find the TCP payload data
 * and check the length of this data.
 */
static BaseType_t prvCheckRxData( NetworkBufferDescriptor_t *pxNetworkBuffer, uint8_t **ppucRecvData )
{
TCPPacket_t *pxTCPPacket = ( TCPPacket_t * ) ( pxNetworkBuffer->pucEthernetBuffer );
TCPHeader_t *pxTCPHeader = &( pxTCPPacket->xTCPHeader );
int32_t lLength, lTCPHeaderLength, lReceiveLength, lUrgentLength;

	/* Determine the length and the offset of the user-data sent to this
	node.

	The size of the TCP header is given in a multiple of 4-byte words (single
	byte, needs no ntoh() translation).  A shift-right 2: is the same as
	(offset >> 4) * 4. */
	lTCPHeaderLength = ( BaseType_t ) ( ( pxTCPHeader->ucTCPOffset & VALID_BITS_IN_TCP_OFFSET_BYTE ) >> 2 );

	/* Let pucRecvData point to the first byte received. */
	*ppucRecvData = pxNetworkBuffer->pucEthernetBuffer + ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv4_HEADER + lTCPHeaderLength;

	/* Calculate lReceiveLength - the length of the TCP data received.  This is
	equal to the total packet length minus:
	( LinkLayer length (14) + IP header length (20) + size of TCP header(20 +) ).*/
	lReceiveLength = ( ( int32_t ) pxNetworkBuffer->xDataLength ) - ( int32_t ) ipSIZE_OF_ETH_HEADER;
	lLength =  ( int32_t )FreeRTOS_htons( pxTCPPacket->xIPHeader.usLength );

	if( lReceiveLength > lLength )
	{
		/* More bytes were received than the reported length, often because of
		padding bytes at the end. */
		lReceiveLength = lLength;
	}

	/* Subtract the size of the TCP and IP headers and the actual data size is
	known. */
	if( lReceiveLength > ( lTCPHeaderLength + ( int32_t ) ipSIZE_OF_IPv4_HEADER ) )
	{
		lReceiveLength -= ( lTCPHeaderLength + ( int32_t ) ipSIZE_OF_IPv4_HEADER );
	}
	else
	{
		lReceiveLength = 0;
	}

	/* Urgent Pointer:
	This field communicates the current value of the urgent pointer as a
	positive offset from the sequence number in this segment.  The urgent
	pointer points to the sequence number of the octet following the urgent
	data.  This field is only be interpreted in segments with the URG control
	bit set. */
	if( ( pxTCPHeader->ucTCPFlags & ipTCP_FLAG_URG ) != 0u )
	{
		/* Although we ignore the urgent data, we have to skip it. */
		lUrgentLength = ( int32_t ) FreeRTOS_htons( pxTCPHeader->usUrgent );
		*ppucRecvData += lUrgentLength;
		lReceiveLength -= FreeRTOS_min_int32( lReceiveLength, lUrgentLength );
	}

	return ( BaseType_t ) lReceiveLength;
}
/*-----------------------------------------------------------*/

/*
 * prvStoreRxData(): called from prvTCPHandleState()
 *
 * The second thing is to do is check if the payload data may be accepted
 * If so, they will be added to the reception queue.
 */
static BaseType_t prvStoreRxData( FreeRTOS_Socket_t *pxSocket, uint8_t *pucRecvData,
	NetworkBufferDescriptor_t *pxNetworkBuffer, uint32_t ulReceiveLength )
{
TCPPacket_t *pxTCPPacket = ( TCPPacket_t * ) ( pxNetworkBuffer->pucEthernetBuffer );
TCPHeader_t *pxTCPHeader = &pxTCPPacket->xTCPHeader;
TCPWindow_t *pxTCPWindow = &pxSocket->u.xTCP.xTCPWindow;
uint32_t ulSequenceNumber, ulSpace;
int32_t lOffset, lStored;
BaseType_t xResult = 0;

	ulSequenceNumber = FreeRTOS_ntohl( pxTCPHeader->ulSequenceNumber );

	if( ( ulReceiveLength > 0u ) && ( pxSocket->u.xTCP.ucTCPState >= eSYN_RECEIVED ) )
	{
		/* See if way may accept the data contents and forward it to the socket
		owner.

		If it can't be "accept"ed it may have to be stored and send a selective
		ack (SACK) option to confirm it.  In that case, xTCPWindowRxStore() will be
		called later to store an out-of-order packet (in case lOffset is
		negative). */
		if ( pxSocket->u.xTCP.rxStream )
		{
			ulSpace = ( uint32_t )uxStreamBufferGetSpace ( pxSocket->u.xTCP.rxStream );
		}
		else
		{
			ulSpace = ( uint32_t )pxSocket->u.xTCP.uxRxStreamSize;
		}

		lOffset = lTCPWindowRxCheck( pxTCPWindow, ulSequenceNumber, ulReceiveLength, ulSpace );

		if( lOffset >= 0 )
		{
			/* New data has arrived and may be made available to the user.  See
			if the head marker in rxStream may be advanced,	only if lOffset == 0.
			In case the low-water mark is reached, bLowWater will be set
			"low-water" here stands for "little space". */
			lStored = lTCPAddRxdata( pxSocket, ( uint32_t ) lOffset, pucRecvData, ulReceiveLength );

			if( lStored != ( int32_t ) ulReceiveLength )
			{
				FreeRTOS_debug_printf( ( "lTCPAddRxdata: stored %ld / %lu bytes??\n", lStored, ulReceiveLength ) );

				/* Received data could not be stored.  The socket's flag
				bMallocError has been set.  The socket now has the status
				eCLOSE_WAIT and a RST packet will be sent back. */
				prvTCPSendReset( pxNetworkBuffer );
				xResult = -1;
			}
		}

		/* After a missing packet has come in, higher packets may be passed to
		the user. */
		#if( ipconfigUSE_TCP_WIN == 1 )
		{
			/* Now lTCPAddRxdata() will move the rxHead pointer forward
			so data becomes available to the user immediately
			In case the low-water mark is reached, bLowWater will be set. */
			if( ( xResult == 0 ) && ( pxTCPWindow->ulUserDataLength > 0 ) )
			{
				lTCPAddRxdata( pxSocket, 0ul, NULL, pxTCPWindow->ulUserDataLength );
				pxTCPWindow->ulUserDataLength = 0;
			}
		}
		#endif /* ipconfigUSE_TCP_WIN */
	}
	else
	{
		pxTCPWindow->ucOptionLength = 0u;
	}

	return xResult;
}
/*-----------------------------------------------------------*/

/* Set the TCP options (if any) for the outgoing packet. */
static UBaseType_t prvSetOptions( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer )
{
TCPPacket_t *pxTCPPacket = ( TCPPacket_t * ) ( pxNetworkBuffer->pucEthernetBuffer );
TCPHeader_t *pxTCPHeader = &pxTCPPacket->xTCPHeader;
TCPWindow_t *pxTCPWindow = &pxSocket->u.xTCP.xTCPWindow;
UBaseType_t uxOptionsLength = pxTCPWindow->ucOptionLength;

	#if(	ipconfigUSE_TCP_WIN == 1 )
		if( uxOptionsLength != 0u )
		{
			/* TCP options must be sent because a packet which is out-of-order
			was received. */
			if( xTCPWindowLoggingLevel >= 0 )
				FreeRTOS_debug_printf( ( "SACK[%d,%d]: optlen %lu sending %lu - %lu\n",
					pxSocket->usLocalPort,
					pxSocket->u.xTCP.usRemotePort,
					uxOptionsLength,
					FreeRTOS_ntohl( pxTCPWindow->ulOptionsData[ 1 ] ) - pxSocket->u.xTCP.xTCPWindow.rx.ulFirstSequenceNumber,
					FreeRTOS_ntohl( pxTCPWindow->ulOptionsData[ 2 ] ) - pxSocket->u.xTCP.xTCPWindow.rx.ulFirstSequenceNumber ) );
			memcpy( pxTCPHeader->ucOptdata, pxTCPWindow->ulOptionsData, ( size_t ) uxOptionsLength );

			/* The header length divided by 4, goes into the higher nibble,
			effectively a shift-left 2. */
			pxTCPHeader->ucTCPOffset = ( uint8_t )( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) << 2 );
		}
		else
	#endif	/* ipconfigUSE_TCP_WIN */
	if( ( pxSocket->u.xTCP.ucTCPState >= eESTABLISHED ) && ( pxSocket->u.xTCP.bits.bMssChange != pdFALSE_UNSIGNED ) )
	{
		/* TCP options must be sent because the MSS has changed. */
		pxSocket->u.xTCP.bits.bMssChange = pdFALSE_UNSIGNED;
		if( xTCPWindowLoggingLevel >= 0 )
		{
			FreeRTOS_debug_printf( ( "MSS: sending %d\n", pxSocket->u.xTCP.usCurMSS ) );
		}

		pxTCPHeader->ucOptdata[ 0 ] = TCP_OPT_MSS;
		pxTCPHeader->ucOptdata[ 1 ] = TCP_OPT_MSS_LEN;
		pxTCPHeader->ucOptdata[ 2 ] = ( uint8_t ) ( ( pxSocket->u.xTCP.usCurMSS ) >> 8 );
		pxTCPHeader->ucOptdata[ 3 ] = ( uint8_t ) ( ( pxSocket->u.xTCP.usCurMSS ) & 0xffu );
		uxOptionsLength = 4u;
		pxTCPHeader->ucTCPOffset = ( uint8_t )( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) << 2 );
	}

	return uxOptionsLength;
}
/*-----------------------------------------------------------*/

/*
 * prvHandleSynReceived(): called from prvTCPHandleState()
 *
 * Called from the states: eSYN_RECEIVED and eCONNECT_SYN
 * If the flags received are correct, the socket will move to eESTABLISHED.
 */
static BaseType_t prvHandleSynReceived( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer,
	uint32_t ulReceiveLength, UBaseType_t uxOptionsLength )
{
TCPPacket_t *pxTCPPacket = ( TCPPacket_t * ) ( (*ppxNetworkBuffer)->pucEthernetBuffer );
TCPHeader_t *pxTCPHeader = &pxTCPPacket->xTCPHeader;
TCPWindow_t *pxTCPWindow = &pxSocket->u.xTCP.xTCPWindow;
uint8_t ucTCPFlags = pxTCPHeader->ucTCPFlags;
uint32_t ulSequenceNumber = FreeRTOS_ntohl( pxTCPHeader->ulSequenceNumber );
BaseType_t xSendLength = 0;

	/* Either expect a ACK or a SYN+ACK. */
	uint16_t usExpect = ( uint16_t ) ipTCP_FLAG_ACK;
	if( pxSocket->u.xTCP.ucTCPState == eCONNECT_SYN )
	{
		usExpect |= ( uint16_t ) ipTCP_FLAG_SYN;
	}

	if( ( ucTCPFlags & 0x17u ) != usExpect )
	{
		/* eSYN_RECEIVED: flags 0010 expected, not 0002. */
		/* eSYN_RECEIVED: flags ACK  expected, not SYN. */
		FreeRTOS_debug_printf( ( "%s: flags %04X expected, not %04X\n",
			pxSocket->u.xTCP.ucTCPState == eSYN_RECEIVED ? "eSYN_RECEIVED" : "eCONNECT_SYN",
			usExpect, ucTCPFlags ) );
		vTCPStateChange( pxSocket, eCLOSE_WAIT );
		pxTCPHeader->ucTCPFlags |= ipTCP_FLAG_RST;
		xSendLength = ( BaseType_t ) ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + uxOptionsLength );
		pxTCPHeader->ucTCPOffset = ( uint8_t )( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) << 2 );
	}
	else
	{
		pxTCPWindow->usPeerPortNumber = pxSocket->u.xTCP.usRemotePort;
		pxTCPWindow->usOurPortNumber = pxSocket->usLocalPort;

		if( pxSocket->u.xTCP.ucTCPState == eCONNECT_SYN )
		{
			TCPPacket_t *pxLastTCPPacket = ( TCPPacket_t * ) ( pxSocket->u.xTCP.xPacket.u.ucLastPacket );

			/* Clear the SYN flag in lastPacket. */
			pxLastTCPPacket->xTCPHeader.ucTCPFlags = ipTCP_FLAG_ACK;

			/* This socket was the one connecting actively so now perofmr the
			synchronisation. */
			vTCPWindowInit( &pxSocket->u.xTCP.xTCPWindow,
				ulSequenceNumber, pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber, ( uint32_t ) pxSocket->u.xTCP.usCurMSS );
			pxTCPWindow->rx.ulCurrentSequenceNumber = pxTCPWindow->rx.ulHighestSequenceNumber = ulSequenceNumber + 1u;
			pxTCPWindow->tx.ulCurrentSequenceNumber++; /* because we send a TCP_SYN [ | TCP_ACK ]; */
			pxTCPWindow->ulNextTxSequenceNumber++;
		}
		else if( ulReceiveLength == 0u )
		{
			pxTCPWindow->rx.ulCurrentSequenceNumber = ulSequenceNumber;
		}

		/* The SYN+ACK has been confirmed, increase the next sequence number by
		1. */
		pxTCPWindow->ulOurSequenceNumber = pxTCPWindow->tx.ulFirstSequenceNumber + 1u;

		#if( ipconfigUSE_TCP_WIN == 1 )
		{
			FreeRTOS_debug_printf( ( "TCP: %s %d => %lxip:%d set ESTAB (scaling %u)\n",
				pxSocket->u.xTCP.ucTCPState == eCONNECT_SYN ? "active" : "passive",
				pxSocket->usLocalPort,
				pxSocket->u.xTCP.ulRemoteIP,
				pxSocket->u.xTCP.usRemotePort,
				( unsigned ) pxSocket->u.xTCP.bits.bWinScaling ) );
		}
		#endif /* ipconfigUSE_TCP_WIN */

		if( ( pxSocket->u.xTCP.ucTCPState == eCONNECT_SYN ) || ( ulReceiveLength != 0u ) )
		{
			pxTCPHeader->ucTCPFlags = ipTCP_FLAG_ACK;
			xSendLength = ( BaseType_t ) ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + uxOptionsLength );
			pxTCPHeader->ucTCPOffset = ( uint8_t ) ( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) << 2 );
		}
		#if( ipconfigUSE_TCP_WIN != 0 )
		{
			if( pxSocket->u.xTCP.bits.bWinScaling == pdFALSE_UNSIGNED )
			{
				/* The other party did not send a scaling factor.
				A shifting factor in this side must be canceled. */
				pxSocket->u.xTCP.ucMyWinScaleFactor = 0;
				pxSocket->u.xTCP.ucPeerWinScaleFactor = 0;
			}
		}
		#endif /* ipconfigUSE_TCP_WIN */
		/* This was the third step of connecting: SYN, SYN+ACK, ACK	so now the
		connection is established. */
		vTCPStateChange( pxSocket, eESTABLISHED );
	}

	return xSendLength;
}
/*-----------------------------------------------------------*/

/*
 * prvHandleEstablished(): called from prvTCPHandleState()
 *
 * Called if the status is eESTABLISHED.  Data reception has been handled
 * earlier.  Here the ACK's from peer will be checked, and if a FIN is received,
 * the code will check if it may be accepted, i.e. if all expected data has been
 * completely received.
 */
static BaseType_t prvHandleEstablished( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer,
	uint32_t ulReceiveLength, UBaseType_t uxOptionsLength )
{
TCPPacket_t *pxTCPPacket = ( TCPPacket_t * ) ( (*ppxNetworkBuffer)->pucEthernetBuffer );
TCPHeader_t *pxTCPHeader = &pxTCPPacket->xTCPHeader;
TCPWindow_t *pxTCPWindow = &pxSocket->u.xTCP.xTCPWindow;
uint8_t ucTCPFlags = pxTCPHeader->ucTCPFlags;
uint32_t ulSequenceNumber = FreeRTOS_ntohl( pxTCPHeader->ulSequenceNumber ), ulCount;
BaseType_t xSendLength = 0, xMayClose = pdFALSE, bRxComplete, bTxDone;
int32_t lDistance, lSendResult;

	/* Remember the window size the peer is advertising. */
	pxSocket->u.xTCP.ulWindowSize = FreeRTOS_ntohs( pxTCPHeader->usWindow );
	#if( ipconfigUSE_TCP_WIN != 0 )
	{
		pxSocket->u.xTCP.ulWindowSize =
			( pxSocket->u.xTCP.ulWindowSize << pxSocket->u.xTCP.ucPeerWinScaleFactor );
	}
	#endif

	if( ( ucTCPFlags & ( uint8_t ) ipTCP_FLAG_ACK ) != 0u )
	{
		ulCount = ulTCPWindowTxAck( pxTCPWindow, FreeRTOS_ntohl( pxTCPPacket->xTCPHeader.ulAckNr ) );

		/* ulTCPWindowTxAck() returns the number of bytes which have been acked,
		starting at 'tx.ulCurrentSequenceNumber'.  Advance the tail pointer in
		txStream. */
		if( ( pxSocket->u.xTCP.txStream != NULL ) && ( ulCount > 0u ) )
		{
			/* Just advancing the tail index, 'ulCount' bytes have been
			confirmed, and because there is new space in the txStream, the
			user/owner should be woken up. */
			/* _HT_ : only in case the socket's waiting? */
			if( uxStreamBufferGet( pxSocket->u.xTCP.txStream, 0u, NULL, ( size_t ) ulCount, pdFALSE ) != 0u )
			{
				pxSocket->xEventBits |= eSOCKET_SEND;

				#if ipconfigSUPPORT_SELECT_FUNCTION == 1
				{
					if( ( pxSocket->xSelectBits & eSELECT_WRITE ) != 0 )
					{
						pxSocket->xEventBits |= ( eSELECT_WRITE << SOCKET_EVENT_BIT_COUNT );
					}
				}
				#endif
				/* In case the socket owner has installed an OnSent handler,
				call it now. */
				#if( ipconfigUSE_CALLBACKS == 1 )
				{
					if( ipconfigIS_VALID_PROG_ADDRESS( pxSocket->u.xTCP.pxHandleSent ) )
					{
						pxSocket->u.xTCP.pxHandleSent( (Socket_t *)pxSocket, ulCount );
					}
				}
				#endif /* ipconfigUSE_CALLBACKS == 1  */
			}
		}
	}

	/* If this socket has a stream for transmission, add the data to the
	outgoing segment(s). */
	if( pxSocket->u.xTCP.txStream != NULL )
	{
		prvTCPAddTxData( pxSocket );
	}

	pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber = pxTCPWindow->tx.ulCurrentSequenceNumber;

	if( ( pxSocket->u.xTCP.bits.bFinAccepted != pdFALSE_UNSIGNED ) || ( ( ucTCPFlags & ( uint8_t ) ipTCP_FLAG_FIN ) != 0u ) )
	{
		/* Peer is requesting to stop, see if we're really finished. */
		xMayClose = pdTRUE;

		/* Checks are only necessary if we haven't sent a FIN yet. */
		if( pxSocket->u.xTCP.bits.bFinSent == pdFALSE_UNSIGNED )
		{
			/* xTCPWindowTxDone returns true when all Tx queues are empty. */
			bRxComplete = xTCPWindowRxEmpty( pxTCPWindow );
			bTxDone	 = xTCPWindowTxDone( pxTCPWindow );

			if( ( bRxComplete == 0 ) || ( bTxDone == 0 ) )
			{
				/* Refusing FIN: Rx incomp 1 optlen 4 tx done 1. */
				FreeRTOS_debug_printf( ( "Refusing FIN[%u,%u]: RxCompl %lu tx done %ld\n",
					pxSocket->usLocalPort,
					pxSocket->u.xTCP.usRemotePort,
					bRxComplete, bTxDone ) );
				xMayClose = pdFALSE;
			}
			else
			{
				lDistance = ( int32_t ) ( ulSequenceNumber + ulReceiveLength - pxTCPWindow->rx.ulCurrentSequenceNumber );

				if( lDistance > 1 )
				{
					FreeRTOS_debug_printf( ( "Refusing FIN: Rx not complete %ld (cur %lu high %lu)\n",
						lDistance, pxTCPWindow->rx.ulCurrentSequenceNumber - pxTCPWindow->rx.ulFirstSequenceNumber,
						pxTCPWindow->rx.ulHighestSequenceNumber - pxTCPWindow->rx.ulFirstSequenceNumber ) );

					xMayClose = pdFALSE;
				}
			}
		}

		if( xTCPWindowLoggingLevel > 0 )
		{
			FreeRTOS_debug_printf( ( "TCP: FIN received, mayClose = %ld (Rx %lu Len %ld, Tx %lu)\n",
				xMayClose, ulSequenceNumber - pxSocket->u.xTCP.xTCPWindow.rx.ulFirstSequenceNumber, ulReceiveLength,
				pxTCPWindow->tx.ulCurrentSequenceNumber - pxSocket->u.xTCP.xTCPWindow.tx.ulFirstSequenceNumber ) );
		}

		if( xMayClose != pdFALSE )
		{
			pxSocket->u.xTCP.bits.bFinAccepted = pdTRUE_UNSIGNED;
			xSendLength = prvTCPHandleFin( pxSocket, *ppxNetworkBuffer );
		}
	}

	if( xMayClose == pdFALSE )
	{
		pxTCPHeader->ucTCPFlags = ipTCP_FLAG_ACK;

		if( ulReceiveLength != 0u )
		{
			xSendLength = ( BaseType_t ) ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + uxOptionsLength );
			/* TCP-offsett equals '( ( length / 4 ) << 4 )', resulting in a shift-left 2 */
			pxTCPHeader->ucTCPOffset = ( uint8_t )( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) << 2 );

			if( pxSocket->u.xTCP.bits.bFinSent != pdFALSE_UNSIGNED )
			{
				pxTCPWindow->tx.ulCurrentSequenceNumber = pxTCPWindow->tx.ulFINSequenceNumber;
			}
		}

		/* Now get data to be transmitted. */
		/* _HT_ patch: since the MTU has be fixed at 1500 in stead of 1526, TCP
		can not	send-out both TCP options and also a full packet. Sending
		options (SACK) is always more urgent than sending data, which can be
		sent later. */
		if( uxOptionsLength == 0u )
		{
			/* prvTCPPrepareSend might allocate a bigger network buffer, if
			necessary. */
			lSendResult = prvTCPPrepareSend( pxSocket, ppxNetworkBuffer, uxOptionsLength );
			if( lSendResult > 0 )
			{
				xSendLength = ( BaseType_t ) lSendResult;
			}
		}
	}

	return xSendLength;
}
/*-----------------------------------------------------------*/

/*
 * Called from prvTCPHandleState().  There is data to be sent.  If
 * ipconfigUSE_TCP_WIN is defined, and if only an ACK must be sent, it will be
 * checked if it would better be postponed for efficiency.
 */
static BaseType_t prvSendData( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer,
	uint32_t ulReceiveLength, BaseType_t xSendLength )
{
TCPPacket_t *pxTCPPacket = ( TCPPacket_t * ) ( (*ppxNetworkBuffer)->pucEthernetBuffer );
TCPHeader_t *pxTCPHeader = &pxTCPPacket->xTCPHeader;
TCPWindow_t *pxTCPWindow = &pxSocket->u.xTCP.xTCPWindow;
/* Find out what window size we may advertised. */
uint32_t ulFrontSpace;
int32_t lRxSpace;
#if( ipconfigUSE_TCP_WIN == 1 )
	#if( ipconfigTCP_ACK_EARLIER_PACKET == 0 )
		const int32_t lMinLength = 0;
	#else
		int32_t lMinLength;
	#endif
#endif
	pxSocket->u.xTCP.ulRxCurWinSize = pxTCPWindow->xSize.ulRxWindowLength -
									 ( pxTCPWindow->rx.ulHighestSequenceNumber - pxTCPWindow->rx.ulCurrentSequenceNumber );

	/* Free space in rxStream. */
	if( pxSocket->u.xTCP.rxStream != NULL )
	{
		ulFrontSpace = ( uint32_t ) uxStreamBufferFrontSpace( pxSocket->u.xTCP.rxStream );
	}
	else
	{
		ulFrontSpace = ( uint32_t ) pxSocket->u.xTCP.uxRxStreamSize;
	}

	pxSocket->u.xTCP.ulRxCurWinSize = FreeRTOS_min_uint32( ulFrontSpace, pxSocket->u.xTCP.ulRxCurWinSize );

	/* Set the time-out field, so that we'll be called by the IP-task in case no
	next message will be received. */
	lRxSpace = (int32_t)( pxSocket->u.xTCP.ulHighestRxAllowed - pxTCPWindow->rx.ulCurrentSequenceNumber );
	#if ipconfigUSE_TCP_WIN == 1
	{

		#if( ipconfigTCP_ACK_EARLIER_PACKET != 0 )
		{
			lMinLength = ( ( int32_t ) 2 ) * ( ( int32_t ) pxSocket->u.xTCP.usCurMSS );
		}
		#endif /* ipconfigTCP_ACK_EARLIER_PACKET */

		/* In case we're receiving data continuously, we might postpone sending
		an ACK to gain performance. */
		if( ( ulReceiveLength > 0 ) &&							/* Data was sent to this socket. */
			( lRxSpace >= lMinLength ) &&						/* There is Rx space for more data. */
			( pxSocket->u.xTCP.bits.bFinSent == pdFALSE_UNSIGNED ) &&	/* Not in a closure phase. */
			( xSendLength == ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER ) ) && /* No Tx data or options to be sent. */
			( pxSocket->u.xTCP.ucTCPState == eESTABLISHED ) &&	/* Connection established. */
			( pxTCPHeader->ucTCPFlags == ipTCP_FLAG_ACK ) )		/* There are no other flags than an ACK. */
		{
			if( pxSocket->u.xTCP.pxAckMessage != *ppxNetworkBuffer )
			{
				/* There was still a delayed in queue, delete it. */
				if( pxSocket->u.xTCP.pxAckMessage != 0 )
				{
					vReleaseNetworkBufferAndDescriptor( pxSocket->u.xTCP.pxAckMessage );
				}

				pxSocket->u.xTCP.pxAckMessage = *ppxNetworkBuffer;
			}
			if( ( ulReceiveLength < ( uint32_t ) pxSocket->u.xTCP.usCurMSS ) ||	/* Received a small message. */
				( lRxSpace < ( int32_t ) ( 2U * pxSocket->u.xTCP.usCurMSS ) ) )	/* There are less than 2 x MSS space in the Rx buffer. */
			{
				pxSocket->u.xTCP.usTimeout = ( uint16_t ) pdMS_TO_MIN_TICKS( DELAYED_ACK_SHORT_DELAY_MS );
			}
			else
			{
				/* Normally a delayed ACK should wait 200 ms for a next incoming
				packet.  Only wait 20 ms here to gain performance.  A slow ACK
				for full-size message. */
				pxSocket->u.xTCP.usTimeout = ( uint16_t ) pdMS_TO_MIN_TICKS( DELAYED_ACK_LONGER_DELAY_MS );
			}

			if( ( xTCPWindowLoggingLevel > 1 ) && ( ipconfigTCP_MAY_LOG_PORT( pxSocket->usLocalPort ) != pdFALSE ) )
			{
				FreeRTOS_debug_printf( ( "Send[%u->%u] del ACK %lu SEQ %lu (len %lu) tmout %u d %lu\n",
					pxSocket->usLocalPort,
					pxSocket->u.xTCP.usRemotePort,
					pxTCPWindow->rx.ulCurrentSequenceNumber - pxTCPWindow->rx.ulFirstSequenceNumber,
					pxSocket->u.xTCP.xTCPWindow.ulOurSequenceNumber - pxTCPWindow->tx.ulFirstSequenceNumber,
					xSendLength,
					pxSocket->u.xTCP.usTimeout, lRxSpace ) );
			}

			*ppxNetworkBuffer = NULL;
			xSendLength = 0;
		}
		else if( pxSocket->u.xTCP.pxAckMessage != NULL )
		{
			/* As an ACK is not being delayed, remove any earlier delayed ACK
			message. */
			if( pxSocket->u.xTCP.pxAckMessage != *ppxNetworkBuffer )
			{
				vReleaseNetworkBufferAndDescriptor( pxSocket->u.xTCP.pxAckMessage );
			}

			pxSocket->u.xTCP.pxAckMessage = NULL;
		}
	}
	#else
	{
		/* Remove compiler warnings. */
		( void ) ulReceiveLength;
		( void ) pxTCPHeader;
		( void ) lRxSpace;
	}
	#endif /* ipconfigUSE_TCP_WIN */

	if( xSendLength != 0 )
	{
		if( ( xTCPWindowLoggingLevel > 1 ) && ( ipconfigTCP_MAY_LOG_PORT( pxSocket->usLocalPort ) != pdFALSE ) )
		{
			FreeRTOS_debug_printf( ( "Send[%u->%u] imm ACK %lu SEQ %lu (len %lu)\n",
				pxSocket->usLocalPort,
				pxSocket->u.xTCP.usRemotePort,
				pxTCPWindow->rx.ulCurrentSequenceNumber - pxTCPWindow->rx.ulFirstSequenceNumber,
				pxTCPWindow->ulOurSequenceNumber - pxTCPWindow->tx.ulFirstSequenceNumber,
				xSendLength ) );
		}

		/* Set the parameter 'xReleaseAfterSend' to the value of
		ipconfigZERO_COPY_TX_DRIVER. */
		prvTCPReturnPacket( pxSocket, *ppxNetworkBuffer, ( uint32_t ) xSendLength, ipconfigZERO_COPY_TX_DRIVER );
		#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
		{
			/* The driver has taken ownership of the Network Buffer. */
			*ppxNetworkBuffer = NULL;
		}
		#endif
	}

	return xSendLength;
}
/*-----------------------------------------------------------*/

/*
 * prvTCPHandleState()
 * is the most important function of this TCP stack
 * We've tried to keep it (relatively short) by putting a lot of code in
 * the static functions above:
 *
 *		prvCheckRxData()
 *		prvStoreRxData()
 *		prvSetOptions()
 *		prvHandleSynReceived()
 *		prvHandleEstablished()
 *		prvSendData()
 *
 * As these functions are declared static, and they're called from one location
 * only, most compilers will inline them, thus avoiding a call and return.
 */
static BaseType_t prvTCPHandleState( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer )
{
TCPPacket_t *pxTCPPacket = ( TCPPacket_t * ) ( (*ppxNetworkBuffer)->pucEthernetBuffer );
TCPHeader_t *pxTCPHeader = &( pxTCPPacket->xTCPHeader );
BaseType_t xSendLength = 0;
uint32_t ulReceiveLength;	/* Number of bytes contained in the TCP message. */
uint8_t *pucRecvData;
uint32_t ulSequenceNumber = FreeRTOS_ntohl (pxTCPHeader->ulSequenceNumber);

	/* uxOptionsLength: the size of the options to be sent (always a multiple of
	4 bytes)
	1. in the SYN phase, we shall communicate the MSS
	2. in case of a SACK, Selective ACK, ack a segment which comes in
	out-of-order. */
UBaseType_t uxOptionsLength = 0u;
uint8_t ucTCPFlags = pxTCPHeader->ucTCPFlags;
TCPWindow_t *pxTCPWindow = &( pxSocket->u.xTCP.xTCPWindow );

	/* First get the length and the position of the received data, if any.
	pucRecvData will point to the first byte of the TCP payload. */
	ulReceiveLength = ( uint32_t ) prvCheckRxData( *ppxNetworkBuffer, &pucRecvData );

	if( pxSocket->u.xTCP.ucTCPState >= eESTABLISHED )
	{
		if ( pxTCPWindow->rx.ulCurrentSequenceNumber == ulSequenceNumber + 1u )
		{
			/* This is most probably a keep-alive message from peer.  Setting
			'bWinChange' doesn't cause a window-size-change, the flag is used
			here to force sending an immediate ACK. */
			pxSocket->u.xTCP.bits.bWinChange = pdTRUE_UNSIGNED;
		}
	}

	/* Keep track of the highest sequence number that might be expected within
	this connection. */
	if( ( ( int32_t ) ( ulSequenceNumber + ulReceiveLength - pxTCPWindow->rx.ulHighestSequenceNumber ) ) > 0 )
	{
		pxTCPWindow->rx.ulHighestSequenceNumber = ulSequenceNumber + ulReceiveLength;
	}

	/* Storing data may result in a fatal error if malloc() fails. */
	if( prvStoreRxData( pxSocket, pucRecvData, *ppxNetworkBuffer, ulReceiveLength ) < 0 )
	{
		xSendLength = -1;
	}
	else
	{
		uxOptionsLength = prvSetOptions( pxSocket, *ppxNetworkBuffer );

		if( ( pxSocket->u.xTCP.ucTCPState == eSYN_RECEIVED ) && ( ( ucTCPFlags & ipTCP_FLAG_CTRL ) == ipTCP_FLAG_SYN ) )
		{
			FreeRTOS_debug_printf( ( "eSYN_RECEIVED: ACK expected, not SYN: peer missed our SYN+ACK\n" ) );

			/* In eSYN_RECEIVED a simple ACK is expected, but apparently the
			'SYN+ACK' didn't arrive.  Step back to the previous state in which
			a first incoming SYN is handled.  The SYN was counted already so
			decrease it first. */
			vTCPStateChange( pxSocket, eSYN_FIRST );
		}

		if( ( ( ucTCPFlags & ipTCP_FLAG_FIN ) != 0u ) && ( pxSocket->u.xTCP.bits.bFinRecv == pdFALSE_UNSIGNED ) )
		{
			/* It's the first time a FIN has been received, remember its
			sequence number. */
			pxTCPWindow->rx.ulFINSequenceNumber = ulSequenceNumber + ulReceiveLength;
			pxSocket->u.xTCP.bits.bFinRecv = pdTRUE_UNSIGNED;

			/* Was peer the first one to send a FIN? */
			if( pxSocket->u.xTCP.bits.bFinSent == pdFALSE_UNSIGNED )
			{
				/* If so, don't send the-last-ACK. */
				pxSocket->u.xTCP.bits.bFinLast = pdTRUE_UNSIGNED;
			}
		}

		switch (pxSocket->u.xTCP.ucTCPState)
		{
		case eCLOSED:		/* (server + client) no connection state at all. */
			/* Nothing to do for a closed socket, except waiting for the
			owner. */
			break;

		case eTCP_LISTEN:	/* (server) waiting for a connection request from
							any remote TCP and port. */
			/* The listen state was handled in xProcessReceivedTCPPacket().
			Should not come here. */
			break;

		case eSYN_FIRST:	/* (server) Just received a SYN request for a server
							socket. */
			{
				/* A new socket has been created, reply with a SYN+ACK.
				Acknowledge with seq+1 because the SYN is seen as pseudo data
				with len = 1. */
				uxOptionsLength = prvSetSynAckOptions( pxSocket, pxTCPPacket );
				pxTCPHeader->ucTCPFlags = ipTCP_FLAG_SYN | ipTCP_FLAG_ACK;

				xSendLength = ( BaseType_t ) ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + uxOptionsLength );

				/* Set the TCP offset field:  ipSIZE_OF_TCP_HEADER equals 20 and
				uxOptionsLength is a multiple of 4.  The complete expression is:
				ucTCPOffset = ( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) / 4 ) << 4 */
				pxTCPHeader->ucTCPOffset = ( uint8_t )( ( ipSIZE_OF_TCP_HEADER + uxOptionsLength ) << 2 );
				vTCPStateChange( pxSocket, eSYN_RECEIVED );

				pxTCPWindow->rx.ulCurrentSequenceNumber = pxTCPWindow->rx.ulHighestSequenceNumber = ulSequenceNumber + 1u;
				pxTCPWindow->tx.ulCurrentSequenceNumber = pxTCPWindow->ulNextTxSequenceNumber = pxTCPWindow->tx.ulFirstSequenceNumber + 1u; /* because we send a TCP_SYN. */
			}
			break;

		case eCONNECT_SYN:	/* (client) also called SYN_SENT: we've just send a
							SYN, expect	a SYN+ACK and send a ACK now. */
			/* Fall through */
		case eSYN_RECEIVED:	/* (server) we've had a SYN, replied with SYN+SCK
							expect a ACK and do nothing. */
			xSendLength = prvHandleSynReceived( pxSocket, ppxNetworkBuffer, ulReceiveLength, uxOptionsLength );
			break;

		case eESTABLISHED:	/* (server + client) an open connection, data
							received can be	delivered to the user. The normal
							state for the data transfer phase of the connection
							The closing states are also handled here with the
							use of some flags. */
			xSendLength = prvHandleEstablished( pxSocket, ppxNetworkBuffer, ulReceiveLength, uxOptionsLength );
			break;

		case eLAST_ACK:		/* (server + client) waiting for an acknowledgement
							of the connection termination request previously
							sent to the remote TCP (which includes an
							acknowledgement of its connection termination
							request). */
			/* Fall through */
		case eFIN_WAIT_1:	/* (server + client) waiting for a connection termination request from the remote TCP,
							 * or an acknowledgement of the connection termination request previously sent. */
			/* Fall through */
		case eFIN_WAIT_2:	/* (server + client) waiting for a connection termination request from the remote TCP. */
			xSendLength = prvTCPHandleFin( pxSocket, *ppxNetworkBuffer );
			break;

		case eCLOSE_WAIT:	/* (server + client) waiting for a connection
							termination request from the local user.  Nothing to
							do, connection is closed, wait for owner to close
							this socket. */
			break;

		case eCLOSING:		/* (server + client) waiting for a connection
							termination request acknowledgement from the remote
							TCP. */
			break;

		case eTIME_WAIT:	/* (either server or client) waiting for enough time
							to pass to be sure the remote TCP received the
							acknowledgement of its connection termination
							request. [According to RFC 793 a connection can stay
							in TIME-WAIT for a maximum of four minutes known as
							a MSL (maximum segment lifetime).]  These states are
							implemented implicitly by settings flags like
							'bFinSent', 'bFinRecv', and 'bFinAcked'. */
			break;
		default:
			break;
		}
	}

	if( xSendLength > 0 )
	{
		xSendLength = prvSendData( pxSocket, ppxNetworkBuffer, ulReceiveLength, xSendLength );
	}

	return xSendLength;
}
/*-----------------------------------------------------------*/

static BaseType_t prvTCPSendReset( NetworkBufferDescriptor_t *pxNetworkBuffer )
{
	#if( ipconfigIGNORE_UNKNOWN_PACKETS == 0 )
	{
	TCPPacket_t *pxTCPPacket = ( TCPPacket_t * ) ( pxNetworkBuffer->pucEthernetBuffer );
	const BaseType_t xSendLength = ( BaseType_t ) ( ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER + 0u );	/* Plus 0 options. */

		pxTCPPacket->xTCPHeader.ucTCPFlags = ipTCP_FLAG_ACK | ipTCP_FLAG_RST;
		pxTCPPacket->xTCPHeader.ucTCPOffset = ( ipSIZE_OF_TCP_HEADER + 0u ) << 2;

		prvTCPReturnPacket( NULL, pxNetworkBuffer, ( uint32_t ) xSendLength, pdFALSE );
	}
	#endif /* !ipconfigIGNORE_UNKNOWN_PACKETS */

	/* Remove compiler warnings if ipconfigIGNORE_UNKNOWN_PACKETS == 1. */
	( void ) pxNetworkBuffer;

	/* The packet was not consumed. */
	return pdFAIL;
}
/*-----------------------------------------------------------*/

static void prvSocketSetMSS( FreeRTOS_Socket_t *pxSocket )
{
uint32_t ulMSS = ipconfigTCP_MSS;

	if( ( ( FreeRTOS_ntohl( pxSocket->u.xTCP.ulRemoteIP ) ^ *ipLOCAL_IP_ADDRESS_POINTER ) & xNetworkAddressing.ulNetMask ) != 0ul )
	{
		/* Data for this peer will pass through a router, and maybe through
		the internet.  Limit the MSS to 1400 bytes or less. */
		ulMSS = FreeRTOS_min_uint32( ( uint32_t ) REDUCED_MSS_THROUGH_INTERNET, ulMSS );
	}

	FreeRTOS_debug_printf( ( "prvSocketSetMSS: %lu bytes for %lxip:%u\n", ulMSS, pxSocket->u.xTCP.ulRemoteIP, pxSocket->u.xTCP.usRemotePort ) );

	pxSocket->u.xTCP.usInitMSS = pxSocket->u.xTCP.usCurMSS = ( uint16_t ) ulMSS;
}
/*-----------------------------------------------------------*/

/*
 *	FreeRTOS_TCP_IP has only 2 public functions, this is the second one:
 *	xProcessReceivedTCPPacket()
 *		prvTCPHandleState()
 *			prvTCPPrepareSend()
 *				prvTCPReturnPacket()
 *				xNetworkInterfaceOutput()	// Sends data to the NIC
 *		prvTCPSendRepeated()
 *			prvTCPReturnPacket()		// Prepare for returning
 *			xNetworkInterfaceOutput()	// Sends data to the NIC
*/
BaseType_t xProcessReceivedTCPPacket( NetworkBufferDescriptor_t *pxNetworkBuffer )
{
FreeRTOS_Socket_t *pxSocket;
TCPPacket_t * pxTCPPacket = ( TCPPacket_t * ) ( pxNetworkBuffer->pucEthernetBuffer );
uint16_t ucTCPFlags;
uint32_t ulLocalIP;
uint16_t xLocalPort;
uint32_t ulRemoteIP;
uint16_t xRemotePort;
BaseType_t xResult = pdPASS;

	/* Check for a minimum packet size. */
	if( pxNetworkBuffer->xDataLength >= ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv4_HEADER + ipSIZE_OF_TCP_HEADER ) )
	{
		ucTCPFlags = pxTCPPacket->xTCPHeader.ucTCPFlags;
		ulLocalIP = FreeRTOS_htonl( pxTCPPacket->xIPHeader.ulDestinationIPAddress );
		xLocalPort = FreeRTOS_htons( pxTCPPacket->xTCPHeader.usDestinationPort );
		ulRemoteIP = FreeRTOS_htonl( pxTCPPacket->xIPHeader.ulSourceIPAddress );
		xRemotePort = FreeRTOS_htons( pxTCPPacket->xTCPHeader.usSourcePort );

		/* Find the destination socket, and if not found: return a socket listing to
		the destination PORT. */
		pxSocket = ( FreeRTOS_Socket_t * )pxTCPSocketLookup( ulLocalIP, xLocalPort, ulRemoteIP, xRemotePort );
	}
	else
	{
		return pdFAIL;
	}

	if( ( pxSocket == NULL ) || ( prvTCPSocketIsActive( ( UBaseType_t ) pxSocket->u.xTCP.ucTCPState ) == pdFALSE ) )
	{
		/* A TCP messages is received but either there is no socket with the
		given port number or the there is a socket, but it is in one of these
		non-active states:  eCLOSED, eCLOSE_WAIT, eFIN_WAIT_2, eCLOSING, or
		eTIME_WAIT. */

		FreeRTOS_debug_printf( ( "TCP: No active socket on port %d (%lxip:%d)\n", xLocalPort, ulRemoteIP, xRemotePort ) );

		/* Send a RST to all packets that can not be handled.  As a result
		the other party will get a ECONN error.  There are two exceptions:
		1) A packet that already has the RST flag set.
		2) A packet that only has the ACK flag set.
		A packet with only the ACK flag set might be the last ACK in
	 	a three-way hand-shake that closes a connection. */
		if( ( ( ucTCPFlags & ipTCP_FLAG_CTRL ) != ipTCP_FLAG_ACK ) &&
			( ( ucTCPFlags & ipTCP_FLAG_RST ) == 0u ) )
		{
			prvTCPSendReset( pxNetworkBuffer );
		}

		/* The packet can't be handled. */
		xResult = pdFAIL;
	}
	else
	{
		pxSocket->u.xTCP.ucRepCount = 0u;

		if( pxSocket->u.xTCP.ucTCPState == eTCP_LISTEN )
		{
			/* The matching socket is in a listening state.  Test if the peer
			has set the SYN flag. */
			if( ( ucTCPFlags & ipTCP_FLAG_CTRL ) != ipTCP_FLAG_SYN )
			{
				/* What happens: maybe after a reboot, a client doesn't know the
				connection had gone.  Send a RST in order to get a new connect
				request. */
				#if( ipconfigHAS_DEBUG_PRINTF == 1 )
				{
				FreeRTOS_debug_printf( ( "TCP: Server can't handle flags: %s from %lxip:%u to port %u\n",
					prvTCPFlagMeaning( ( UBaseType_t ) ucTCPFlags ), ulRemoteIP, xRemotePort, xLocalPort ) );
				}
				#endif /* ipconfigHAS_DEBUG_PRINTF */

				if( ( ucTCPFlags & ipTCP_FLAG_RST ) == 0u )
				{
					prvTCPSendReset( pxNetworkBuffer );
				}
				xResult = pdFAIL;
			}
			else
			{
				/* prvHandleListen() will either return a newly created socket
				(if bReuseSocket is false), otherwise it returns the current
				socket which will later get connected. */
				pxSocket = prvHandleListen( pxSocket, pxNetworkBuffer );

				if( pxSocket == NULL )
				{
					xResult = pdFAIL;
				}
			}
		}	/* if( pxSocket->u.xTCP.ucTCPState == eTCP_LISTEN ). */
		else
		{
			/* This is not a socket in listening mode. Check for the RST
			flag. */
			if( ( ucTCPFlags & ipTCP_FLAG_RST ) != 0u )
			{
				/* The target socket is not in a listening state, any RST packet
				will cause the socket to be closed. */
				FreeRTOS_debug_printf( ( "TCP: RST received from %lxip:%u for %u\n", ulRemoteIP, xRemotePort, xLocalPort ) );
				/* _HT_: should indicate that 'ECONNRESET' must be returned to the used during next API. */
				vTCPStateChange( pxSocket, eCLOSED );

				/* The packet cannot be handled. */
				xResult = pdFAIL;
			}
			else if( ( ( ucTCPFlags & ipTCP_FLAG_CTRL ) == ipTCP_FLAG_SYN ) && ( pxSocket->u.xTCP.ucTCPState >= eESTABLISHED ) )
			{
				/* SYN flag while this socket is already connected. */
				FreeRTOS_debug_printf( ( "TCP: SYN unexpected from %lxip:%u\n", ulRemoteIP, xRemotePort ) );

				/* The packet cannot be handled. */
				xResult = pdFAIL;
			}
			else
			{
				/* Update the copy of the TCP header only (skipping eth and IP
				headers).  It might be used later on, whenever data must be sent
				to the peer. */
				const BaseType_t lOffset = ( BaseType_t ) ( ipSIZE_OF_ETH_HEADER + ipSIZE_OF_IPv4_HEADER );
				memcpy( pxSocket->u.xTCP.xPacket.u.ucLastPacket + lOffset, pxNetworkBuffer->pucEthernetBuffer + lOffset, ipSIZE_OF_TCP_HEADER );
			}
		}
	}

	if( xResult != pdFAIL )
	{
		/* Touch the alive timers because we received a message	for this
		socket. */
		prvTCPTouchSocket( pxSocket );

		/* Parse the TCP option(s), if present. */
		/* _HT_ : if we're in the SYN phase, and peer does not send a MSS option,
		then we MUST assume an MSS size of 536 bytes for backward compatibility. */

		/* When there are no TCP options, the TCP offset equals 20 bytes, which is stored as
		the number 5 (words) in the higher niblle of the TCP-offset byte. */
		if( ( pxTCPPacket->xTCPHeader.ucTCPOffset & TCP_OFFSET_LENGTH_BITS ) > TCP_OFFSET_STANDARD_LENGTH )
		{
			prvCheckOptions( pxSocket, pxNetworkBuffer );
		}


		#if( ipconfigUSE_TCP_WIN == 1 )
		{
			pxSocket->u.xTCP.ulWindowSize = FreeRTOS_ntohs( pxTCPPacket->xTCPHeader.usWindow );
			pxSocket->u.xTCP.ulWindowSize =
				( pxSocket->u.xTCP.ulWindowSize << pxSocket->u.xTCP.ucPeerWinScaleFactor );
		}
		#endif

		/* In prvTCPHandleState() the incoming messages will be handled
		depending on the current state of the connection. */
		if( prvTCPHandleState( pxSocket, &pxNetworkBuffer ) > 0 )
		{
			/* prvTCPHandleState() has sent a message, see if there are more to
			be transmitted. */
			#if( ipconfigUSE_TCP_WIN == 1 )
			{
				prvTCPSendRepeated( pxSocket, &pxNetworkBuffer );
			}
			#endif /* ipconfigUSE_TCP_WIN */
		}

		if( pxNetworkBuffer != NULL )
		{
			/* We must check if the buffer is unequal to NULL, because the
			socket might keep a reference to it in case a delayed ACK must be
			sent. */
			vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
			pxNetworkBuffer = NULL;
		}

		/* And finally, calculate when this socket wants to be woken up. */
		prvTCPNextTimeout ( pxSocket );
		/* Return pdPASS to tell that the network buffer is 'consumed'. */
		xResult = pdPASS;
	}

	/* pdPASS being returned means the buffer has been consumed. */
	return xResult;
}
/*-----------------------------------------------------------*/

static FreeRTOS_Socket_t *prvHandleListen( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer )
{
TCPPacket_t * pxTCPPacket = ( TCPPacket_t * ) ( pxNetworkBuffer->pucEthernetBuffer );
FreeRTOS_Socket_t *pxReturn = NULL;
uint32_t ulInitialSequenceNumber;

	/* Assume that a new Initial Sequence Number will be required. Request
	it now in order to fail out if necessary. */
	ulInitialSequenceNumber = ulApplicationGetNextSequenceNumber( *ipLOCAL_IP_ADDRESS_POINTER,
																  pxSocket->usLocalPort,
																  pxTCPPacket->xIPHeader.ulSourceIPAddress,
																  pxTCPPacket->xTCPHeader.usSourcePort );

	/* A pure SYN (without ACK) has come in, create a new socket to answer
	it. */
	if( 0 != ulInitialSequenceNumber )
	{
		if( pxSocket->u.xTCP.bits.bReuseSocket != pdFALSE_UNSIGNED )
		{
			/* The flag bReuseSocket indicates that the same instance of the
			listening socket should be used for the connection. */
			pxReturn = pxSocket;
			pxSocket->u.xTCP.bits.bPassQueued = pdTRUE_UNSIGNED;
			pxSocket->u.xTCP.pxPeerSocket = pxSocket;
		}
		else
		{
			/* The socket does not have the bReuseSocket flag set meaning create a
			new socket when a connection comes in. */
			pxReturn = NULL;

			if( pxSocket->u.xTCP.usChildCount >= pxSocket->u.xTCP.usBacklog )
			{
				FreeRTOS_printf( ( "Check: Socket %u already has %u / %u child%s\n",
					pxSocket->usLocalPort,
					pxSocket->u.xTCP.usChildCount,
					pxSocket->u.xTCP.usBacklog,
					pxSocket->u.xTCP.usChildCount == 1 ? "" : "ren" ) );
				prvTCPSendReset( pxNetworkBuffer );
			}
			else
			{
				FreeRTOS_Socket_t *pxNewSocket = ( FreeRTOS_Socket_t * )
					FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );

				if( ( pxNewSocket == NULL ) || ( pxNewSocket == FREERTOS_INVALID_SOCKET ) )
				{
					FreeRTOS_debug_printf( ( "TCP: Listen: new socket failed\n" ) );
					prvTCPSendReset( pxNetworkBuffer );
				}
				else if( prvTCPSocketCopy( pxNewSocket, pxSocket ) != pdFALSE )
				{
					/* The socket will be connected immediately, no time for the
					owner to setsockopt's, therefore copy properties of the server
					socket to the new socket.  Only the binding might fail (due to
					lack of resources). */
					pxReturn = pxNewSocket;
				}
			}
		}
	}

	if( ( 0 != ulInitialSequenceNumber ) && ( pxReturn != NULL ) )
	{
		pxReturn->u.xTCP.usRemotePort = FreeRTOS_htons( pxTCPPacket->xTCPHeader.usSourcePort );
		pxReturn->u.xTCP.ulRemoteIP = FreeRTOS_htonl( pxTCPPacket->xIPHeader.ulSourceIPAddress );
		pxReturn->u.xTCP.xTCPWindow.ulOurSequenceNumber = ulInitialSequenceNumber;

		/* Here is the SYN action. */
		pxReturn->u.xTCP.xTCPWindow.rx.ulCurrentSequenceNumber = FreeRTOS_ntohl( pxTCPPacket->xTCPHeader.ulSequenceNumber );
		prvSocketSetMSS( pxReturn );

		prvTCPCreateWindow( pxReturn );

		vTCPStateChange( pxReturn, eSYN_FIRST );

		/* Make a copy of the header up to the TCP header.  It is needed later
		on, whenever data must be sent to the peer. */
		memcpy( pxReturn->u.xTCP.xPacket.u.ucLastPacket, pxNetworkBuffer->pucEthernetBuffer, sizeof( pxReturn->u.xTCP.xPacket.u.ucLastPacket ) );
	}
	return pxReturn;
}
/*-----------------------------------------------------------*/

/*
 * Duplicates a socket after a listening socket receives a connection.
 */
static BaseType_t prvTCPSocketCopy( FreeRTOS_Socket_t *pxNewSocket, FreeRTOS_Socket_t *pxSocket )
{
struct freertos_sockaddr xAddress;

	pxNewSocket->xReceiveBlockTime = pxSocket->xReceiveBlockTime;
	pxNewSocket->xSendBlockTime = pxSocket->xSendBlockTime;
	pxNewSocket->ucSocketOptions = pxSocket->ucSocketOptions;
	pxNewSocket->u.xTCP.uxRxStreamSize = pxSocket->u.xTCP.uxRxStreamSize;
	pxNewSocket->u.xTCP.uxTxStreamSize = pxSocket->u.xTCP.uxTxStreamSize;
	pxNewSocket->u.xTCP.uxLittleSpace = pxSocket->u.xTCP.uxLittleSpace;
	pxNewSocket->u.xTCP.uxEnoughSpace = pxSocket->u.xTCP.uxEnoughSpace;
	pxNewSocket->u.xTCP.uxRxWinSize  = pxSocket->u.xTCP.uxRxWinSize;
	pxNewSocket->u.xTCP.uxTxWinSize  = pxSocket->u.xTCP.uxTxWinSize;

	#if( ipconfigSOCKET_HAS_USER_SEMAPHORE == 1 )
	{
		pxNewSocket->pxUserSemaphore = pxSocket->pxUserSemaphore;
	}
	#endif /* ipconfigSOCKET_HAS_USER_SEMAPHORE */

	#if( ipconfigUSE_CALLBACKS == 1 )
	{
		/* In case call-backs are used, copy them from parent to child. */
		pxNewSocket->u.xTCP.pxHandleConnected = pxSocket->u.xTCP.pxHandleConnected;
		pxNewSocket->u.xTCP.pxHandleReceive = pxSocket->u.xTCP.pxHandleReceive;
		pxNewSocket->u.xTCP.pxHandleSent = pxSocket->u.xTCP.pxHandleSent;
	}
	#endif /* ipconfigUSE_CALLBACKS */

	#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
	{
		/* Child socket of listening sockets will inherit the Socket Set
		Otherwise the owner has no chance of including it into the set. */
		if( pxSocket->pxSocketSet )
		{
			pxNewSocket->pxSocketSet = pxSocket->pxSocketSet;
			pxNewSocket->xSelectBits = pxSocket->xSelectBits | eSELECT_READ | eSELECT_EXCEPT;
		}
	}
	#endif /* ipconfigSUPPORT_SELECT_FUNCTION */

	/* And bind it to the same local port as its parent. */
	xAddress.sin_addr = *ipLOCAL_IP_ADDRESS_POINTER;
	xAddress.sin_port = FreeRTOS_htons( pxSocket->usLocalPort );

	#if( ipconfigTCP_HANG_PROTECTION == 1 )
	{
		/* Only when there is anti-hanging protection, a socket may become an
		orphan temporarily.  Once this socket is really connected, the owner of
		the server socket will be notified. */

		/* When bPassQueued is true, the socket is an orphan until it gets
		connected. */
		pxNewSocket->u.xTCP.bits.bPassQueued = pdTRUE_UNSIGNED;
		pxNewSocket->u.xTCP.pxPeerSocket = pxSocket;
	}
	#else
	{
		/* A reference to the new socket may be stored and the socket is marked
		as 'passable'. */

		/* When bPassAccept is pdTRUE_UNSIGNED this socket may be returned in a call to
		accept(). */
		pxNewSocket->u.xTCP.bits.bPassAccept = pdTRUE_UNSIGNED;
		if(pxSocket->u.xTCP.pxPeerSocket == NULL )
		{
			pxSocket->u.xTCP.pxPeerSocket = pxNewSocket;
		}
	}
	#endif

	pxSocket->u.xTCP.usChildCount++;

	FreeRTOS_debug_printf( ( "Gain: Socket %u now has %u / %u child%s\n",
		pxSocket->usLocalPort,
		pxSocket->u.xTCP.usChildCount,
		pxSocket->u.xTCP.usBacklog,
		pxSocket->u.xTCP.usChildCount == 1u ? "" : "ren" ) );

	/* Now bind the child socket to the same port as the listening socket. */
	if( vSocketBind ( pxNewSocket, &xAddress, sizeof( xAddress ), pdTRUE ) != 0 )
	{
		FreeRTOS_debug_printf( ( "TCP: Listen: new socket bind error\n" ) );
		vSocketClose( pxNewSocket );
		return pdFALSE;
	}

	return pdTRUE;
}
/*-----------------------------------------------------------*/

#if( ( ipconfigHAS_DEBUG_PRINTF != 0 ) || ( ipconfigHAS_PRINTF != 0 ) )

	const char *FreeRTOS_GetTCPStateName( UBaseType_t ulState )
	{
		if( ulState >= ( UBaseType_t ) ARRAY_SIZE( pcStateNames ) )
		{
			ulState = ( UBaseType_t ) ARRAY_SIZE( pcStateNames ) - 1u;
		}
		return pcStateNames[ ulState ];
	}

#endif /* ( ( ipconfigHAS_DEBUG_PRINTF != 0 ) || ( ipconfigHAS_PRINTF != 0 ) ) */
/*-----------------------------------------------------------*/

/*
 * In the API accept(), the user asks is there is a new client?  As API's can
 * not walk through the xBoundTCPSocketsList the IP-task will do this.
 */
BaseType_t xTCPCheckNewClient( FreeRTOS_Socket_t *pxSocket )
{
TickType_t xLocalPort = FreeRTOS_htons( pxSocket->usLocalPort );
ListItem_t *pxIterator;
FreeRTOS_Socket_t *pxFound;
BaseType_t xResult = pdFALSE;

	/* Here xBoundTCPSocketsList can be accessed safely IP-task is the only one
	who has access. */
	for( pxIterator = ( ListItem_t * ) listGET_HEAD_ENTRY( &xBoundTCPSocketsList );
		pxIterator != ( ListItem_t * ) listGET_END_MARKER( &xBoundTCPSocketsList );
		pxIterator = ( ListItem_t * ) listGET_NEXT( pxIterator ) )
	{
		if( listGET_LIST_ITEM_VALUE( pxIterator ) == xLocalPort )
		{
			pxFound = ( FreeRTOS_Socket_t * ) listGET_LIST_ITEM_OWNER( pxIterator );
			if( ( pxFound->ucProtocol == FREERTOS_IPPROTO_TCP ) && ( pxFound->u.xTCP.bits.bPassAccept != pdFALSE_UNSIGNED ) )
			{
				pxSocket->u.xTCP.pxPeerSocket = pxFound;
				FreeRTOS_debug_printf( ( "xTCPCheckNewClient[0]: client on port %u\n", pxSocket->usLocalPort ) );
				xResult = pdTRUE;
				break;
			}
		}
	}
	return xResult;
}
/*-----------------------------------------------------------*/

#endif /* ipconfigUSE_TCP == 1 */

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
	#include "aws_freertos_tcp_test_access_tcp_define.h"
#endif

