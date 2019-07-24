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

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"

/* The ItemValue of the sockets xBoundSocketListItem member holds the socket's
port number. */
#define socketSET_SOCKET_PORT( pxSocket, usPort ) listSET_LIST_ITEM_VALUE( ( &( ( pxSocket )->xBoundSocketListItem ) ), ( usPort ) )
#define socketGET_SOCKET_PORT( pxSocket ) listGET_LIST_ITEM_VALUE( ( &( ( pxSocket )->xBoundSocketListItem ) ) )

/* Test if a socket it bound which means it is either included in
xBoundUDPSocketsList or xBoundTCPSocketsList */
#define socketSOCKET_IS_BOUND( pxSocket )	  ( listLIST_ITEM_CONTAINER( & ( pxSocket )->xBoundSocketListItem ) != NULL )

/* If FreeRTOS_sendto() is called on a socket that is not bound to a port
number then, depending on the FreeRTOSIPConfig.h settings, it might be that a
port number is automatically generated for the socket.  Automatically generated
port numbers will be between socketAUTO_PORT_ALLOCATION_START_NUMBER and
0xffff.

Per https://tools.ietf.org/html/rfc6056, "the dynamic ports consist of the range
49152-65535. However, ephemeral port selection algorithms should use the whole
range 1024-65535" excluding those already in use (inbound or outbound). */
#if !defined( socketAUTO_PORT_ALLOCATION_START_NUMBER )
	#define socketAUTO_PORT_ALLOCATION_START_NUMBER ( ( uint16_t ) 0x0400 )
#endif

#define socketAUTO_PORT_ALLOCATION_MAX_NUMBER   ( ( uint16_t ) 0xffff )

/* The number of octets that make up an IP address. */
#define socketMAX_IP_ADDRESS_OCTETS		4u

/* A block time of 0 simply means "don't block". */
#define socketDONT_BLOCK				( ( TickType_t ) 0 )

#if( ( ipconfigUSE_TCP == 1 ) && !defined( ipTCP_TIMER_PERIOD_MS ) )
	#define ipTCP_TIMER_PERIOD_MS	( 1000 )
#endif

/* The next private port number to use when binding a client socket is stored in
the usNextPortToUse[] array - which has either 1 or two indexes depending on
whether TCP is being supported. */
#if( ipconfigUSE_TCP == 1 )
	#define socketPROTOCOL_COUNT		2
#else
	#define socketPROTOCOL_COUNT		1
#endif

/* Indexes into the usNextPortToUse[] array for UDP and TCP sockets
respectively. */
#define socketNEXT_UDP_PORT_NUMBER_INDEX	0
#define socketNEXT_TCP_PORT_NUMBER_INDEX	1


/*-----------------------------------------------------------*/

/*
 * Allocate the next port number from the private allocation range.
 * TCP and UDP each have their own series of port numbers
 * ulProtocol is either ipPROTOCOL_UDP or ipPROTOCOL_TCP
 */
static uint16_t prvGetPrivatePortNumber( BaseType_t xProtocol );

/*
 * Return the list item from within pxList that has an item value of
 * xWantedItemValue.  If there is no such list item return NULL.
 */
static const ListItem_t * pxListFindListItemWithValue( const List_t *pxList, TickType_t xWantedItemValue );

/*
 * Return pdTRUE only if pxSocket is valid and bound, as far as can be
 * determined.
 */
static BaseType_t prvValidSocket( FreeRTOS_Socket_t *pxSocket, BaseType_t xProtocol, BaseType_t xIsBound );

/*
 * Before creating a socket, check the validity of the parameters used
 * and find the size of the socket space, which is different for UDP and TCP
 */
static BaseType_t prvDetermineSocketSize( BaseType_t xDomain, BaseType_t xType, BaseType_t xProtocol, size_t *pxSocketSize );

#if( ipconfigUSE_TCP == 1 )
	/*
	 * Create a txStream or a rxStream, depending on the parameter 'xIsInputStream'
	 */
	static StreamBuffer_t *prvTCPCreateStream (FreeRTOS_Socket_t *pxSocket, BaseType_t xIsInputStream );
#endif /* ipconfigUSE_TCP == 1 */

#if( ipconfigUSE_TCP == 1 )
	/*
	 * Called from FreeRTOS_send(): some checks which will be done before
	 * sending a TCP packed.
	 */
	static int32_t prvTCPSendCheck( FreeRTOS_Socket_t *pxSocket, size_t xDataLength );
#endif /* ipconfigUSE_TCP */

#if( ipconfigUSE_TCP == 1 )
	/*
	 * When a child socket gets closed, make sure to update the child-count of the parent
	 */
	static void prvTCPSetSocketCount( FreeRTOS_Socket_t *pxSocketToDelete );
#endif  /* ipconfigUSE_TCP == 1 */

#if( ipconfigUSE_TCP == 1 )
	/*
	 * Called from FreeRTOS_connect(): make some checks and if allowed, send a
	 * message to the IP-task to start connecting to a remote socket
	 */
	static BaseType_t prvTCPConnectStart( FreeRTOS_Socket_t *pxSocket, struct freertos_sockaddr *pxAddress );
#endif /* ipconfigUSE_TCP */

#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )

	/* Executed by the IP-task, it will check all sockets belonging to a set */
	static FreeRTOS_Socket_t *prvFindSelectedSocket( SocketSelect_t *pxSocketSet );

#endif /* ipconfigSUPPORT_SELECT_FUNCTION == 1 */
/*-----------------------------------------------------------*/

/* The list that contains mappings between sockets and port numbers.  Accesses
to this list must be protected by critical sections of one kind or another. */
List_t xBoundUDPSocketsList;

#if ipconfigUSE_TCP == 1
	List_t xBoundTCPSocketsList;
#endif /* ipconfigUSE_TCP == 1 */

/*-----------------------------------------------------------*/

static BaseType_t prvValidSocket( FreeRTOS_Socket_t *pxSocket, BaseType_t xProtocol, BaseType_t xIsBound )
{
BaseType_t xReturn = pdTRUE;

	if( ( pxSocket == NULL ) || ( pxSocket == FREERTOS_INVALID_SOCKET ) )
	{
		xReturn = pdFALSE;
	}
	else if( ( xIsBound != pdFALSE ) && ( socketSOCKET_IS_BOUND( pxSocket ) == pdFALSE ) )
	{
		/* The caller expects the socket to be bound, but it isn't. */
		xReturn = pdFALSE;
	}
	else if( pxSocket->ucProtocol != ( uint8_t ) xProtocol )
	{
		/* Socket has a wrong type (UDP != TCP). */
		xReturn = pdFALSE;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t vNetworkSocketsInit( void )
{
	vListInitialise( &xBoundUDPSocketsList );

	#if( ipconfigUSE_TCP == 1 )
	{
		vListInitialise( &xBoundTCPSocketsList );
	}
	#endif  /* ipconfigUSE_TCP == 1 */

	return pdTRUE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvDetermineSocketSize( BaseType_t xDomain, BaseType_t xType, BaseType_t xProtocol, size_t *pxSocketSize )
{
BaseType_t xReturn = pdPASS;
FreeRTOS_Socket_t *pxSocket;

	/* Asserts must not appear before it has been determined that the network
	task is ready - otherwise the asserts will fail. */
	if( xIPIsNetworkTaskReady() == pdFALSE )
	{
		xReturn = pdFAIL;
	}
	else
	{
		/* Only Ethernet is currently supported. */
		configASSERT( xDomain == FREERTOS_AF_INET );

		/* Check if the UDP socket-list has been initialised. */
		configASSERT( listLIST_IS_INITIALISED( &xBoundUDPSocketsList ) );
		#if( ipconfigUSE_TCP == 1 )
		{
			/* Check if the TCP socket-list has been initialised. */
			configASSERT( listLIST_IS_INITIALISED( &xBoundTCPSocketsList ) );
		}
		#endif  /* ipconfigUSE_TCP == 1 */

		if( xProtocol == FREERTOS_IPPROTO_UDP )
		{
			if( xType != FREERTOS_SOCK_DGRAM )
			{
				xReturn = pdFAIL;
				configASSERT( xReturn );
			}
			/* In case a UDP socket is created, do not allocate space for TCP data. */
			*pxSocketSize = ( sizeof( *pxSocket ) - sizeof( pxSocket->u ) ) + sizeof( pxSocket->u.xUDP );
		}
#if( ipconfigUSE_TCP == 1 )
		else if( xProtocol == FREERTOS_IPPROTO_TCP )
		{
			if( xType != FREERTOS_SOCK_STREAM )
			{
				xReturn = pdFAIL;
				configASSERT( xReturn );
			}

			*pxSocketSize = ( sizeof( *pxSocket ) - sizeof( pxSocket->u ) ) + sizeof( pxSocket->u.xTCP );
		}
#endif  /* ipconfigUSE_TCP == 1 */
		else
		{
			xReturn = pdFAIL;
			configASSERT( xReturn );
		}
	}
	/* In case configASSERT() is not used */
	( void )xDomain;
	return xReturn;
}
/*-----------------------------------------------------------*/

/* FreeRTOS_socket() allocates and initiates a socket */
Socket_t FreeRTOS_socket( BaseType_t xDomain, BaseType_t xType, BaseType_t xProtocol )
{
FreeRTOS_Socket_t *pxSocket;
size_t uxSocketSize;
EventGroupHandle_t xEventGroup;
Socket_t xReturn;

	if( prvDetermineSocketSize( xDomain, xType, xProtocol, &uxSocketSize ) == pdFAIL )
	{
		xReturn = FREERTOS_INVALID_SOCKET;
	}
	else
	{
		/* Allocate the structure that will hold the socket information.  The
		size depends on the type of socket: UDP sockets need less space.  A
		define 'pvPortMallocSocket' will used to allocate the necessary space.
		By default it points to the FreeRTOS function 'pvPortMalloc()'. */
		pxSocket = ( FreeRTOS_Socket_t * ) pvPortMallocSocket( uxSocketSize );

		if( pxSocket == NULL )
		{
			pxSocket = ( FreeRTOS_Socket_t * ) FREERTOS_INVALID_SOCKET;
			iptraceFAILED_TO_CREATE_SOCKET();
		}
		else if( ( xEventGroup = xEventGroupCreate() ) == NULL )
		{
			vPortFreeSocket( pxSocket );
			pxSocket = ( FreeRTOS_Socket_t * ) FREERTOS_INVALID_SOCKET;
			iptraceFAILED_TO_CREATE_EVENT_GROUP();
		}
		else
		{
			/* Clear the entire space to avoid nulling individual entries */
			memset( pxSocket, '\0', uxSocketSize );

			pxSocket->xEventGroup = xEventGroup;

			/* Initialise the socket's members.  The semaphore will be created
			if the socket is bound to an address, for now the pointer to the
			semaphore is just set to NULL to show it has not been created. */
			if( xProtocol == FREERTOS_IPPROTO_UDP )
			{
				vListInitialise( &( pxSocket->u.xUDP.xWaitingPacketsList ) );

				#if( ipconfigUDP_MAX_RX_PACKETS > 0 )
				{
					pxSocket->u.xUDP.uxMaxPackets = ( UBaseType_t ) ipconfigUDP_MAX_RX_PACKETS;
				}
				#endif /* ipconfigUDP_MAX_RX_PACKETS > 0 */
			}

			vListInitialiseItem( &( pxSocket->xBoundSocketListItem ) );
			listSET_LIST_ITEM_OWNER( &( pxSocket->xBoundSocketListItem ), ( void * ) pxSocket );

			pxSocket->xReceiveBlockTime = ipconfigSOCK_DEFAULT_RECEIVE_BLOCK_TIME;
			pxSocket->xSendBlockTime	= ipconfigSOCK_DEFAULT_SEND_BLOCK_TIME;
			pxSocket->ucSocketOptions   = ( uint8_t ) FREERTOS_SO_UDPCKSUM_OUT;
			pxSocket->ucProtocol		= ( uint8_t ) xProtocol; /* protocol: UDP or TCP */

			#if( ipconfigUSE_TCP == 1 )
			{
				if( xProtocol == FREERTOS_IPPROTO_TCP )
				{
					/* StreamSize is expressed in number of bytes */
					/* Round up buffer sizes to nearest multiple of MSS */
					pxSocket->u.xTCP.usInitMSS	= pxSocket->u.xTCP.usCurMSS = ipconfigTCP_MSS;
					pxSocket->u.xTCP.uxRxStreamSize = ( size_t ) ipconfigTCP_RX_BUFFER_LENGTH;
					pxSocket->u.xTCP.uxTxStreamSize = ( size_t ) FreeRTOS_round_up( ipconfigTCP_TX_BUFFER_LENGTH, ipconfigTCP_MSS );
					/* Use half of the buffer size of the TCP windows */
					#if ( ipconfigUSE_TCP_WIN == 1 )
					{
						pxSocket->u.xTCP.uxRxWinSize  = FreeRTOS_max_uint32( 1UL, ( uint32_t ) ( pxSocket->u.xTCP.uxRxStreamSize / 2 ) / ipconfigTCP_MSS );
						pxSocket->u.xTCP.uxTxWinSize  = FreeRTOS_max_uint32( 1UL, ( uint32_t ) ( pxSocket->u.xTCP.uxTxStreamSize / 2 ) / ipconfigTCP_MSS );
					}
					#else
					{
						pxSocket->u.xTCP.uxRxWinSize  = 1u;
						pxSocket->u.xTCP.uxTxWinSize  = 1u;
					}
					#endif
					/* The above values are just defaults, and can be overridden by
					calling FreeRTOS_setsockopt().  No buffers will be allocated until a
					socket is connected and data is exchanged. */
				}
			}
			#endif  /* ipconfigUSE_TCP == 1 */
		}

		xReturn = ( Socket_t ) pxSocket;
	}

	/* Remove compiler warnings in the case the configASSERT() is not defined. */
	( void ) xDomain;

	return xReturn;
}
/*-----------------------------------------------------------*/

#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )

	SocketSet_t FreeRTOS_CreateSocketSet( void )
	{
	SocketSelect_t *pxSocketSet;

		pxSocketSet = ( SocketSelect_t * ) pvPortMalloc( sizeof( *pxSocketSet ) );

		if( pxSocketSet != NULL )
		{
			memset( pxSocketSet, '\0', sizeof( *pxSocketSet ) );
			pxSocketSet->xSelectGroup = xEventGroupCreate();

			if( pxSocketSet->xSelectGroup == NULL )
			{
				vPortFree( ( void* ) pxSocketSet );
				pxSocketSet = NULL;
			}
		}

		return ( SocketSet_t * ) pxSocketSet;
	}

#endif /* ipconfigSUPPORT_SELECT_FUNCTION == 1 */
/*-----------------------------------------------------------*/

#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )

	void FreeRTOS_DeleteSocketSet( SocketSet_t xSocketSet )
	{
		SocketSelect_t *pxSocketSet = ( SocketSelect_t*) xSocketSet;

		vEventGroupDelete( pxSocketSet->xSelectGroup );
		vPortFree( ( void* ) pxSocketSet );
	}

#endif /* ipconfigSUPPORT_SELECT_FUNCTION == 1 */
/*-----------------------------------------------------------*/

#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )

	/* Add a socket to a set */
	void FreeRTOS_FD_SET( Socket_t xSocket, SocketSet_t xSocketSet, EventBits_t xSelectBits )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	SocketSelect_t *pxSocketSet = ( SocketSelect_t * ) xSocketSet;

		configASSERT( pxSocket != NULL );
		configASSERT( xSocketSet != NULL );

		/* Make sure we're not adding bits which are reserved for internal use,
		such as eSELECT_CALL_IP */
		pxSocket->xSelectBits |= ( xSelectBits & eSELECT_ALL );

		if( ( pxSocket->xSelectBits & eSELECT_ALL ) != 0 )
		{
			/* Adding a socket to a socket set. */
			pxSocket->pxSocketSet = ( SocketSelect_t * ) xSocketSet;

			/* Now have the IP-task call vSocketSelect() to see if the set contains
			any sockets which are 'ready' and set the proper bits.
			By setting 'bApiCalled = false', vSocketSelect() knows that it was
			not called from a user API */
			pxSocketSet->bApiCalled = pdFALSE;
			prvFindSelectedSocket( pxSocketSet );
		}
	}

#endif /* ipconfigSUPPORT_SELECT_FUNCTION == 1 */
/*-----------------------------------------------------------*/

#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
	/* Clear select bits for a socket
	If the mask becomes 0, remove the socket from the set */
	void FreeRTOS_FD_CLR( Socket_t xSocket, SocketSet_t xSocketSet, EventBits_t xSelectBits )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;

		configASSERT( pxSocket != NULL );
		configASSERT( xSocketSet != NULL );

		pxSocket->xSelectBits &= ~( xSelectBits & eSELECT_ALL );
		if( ( pxSocket->xSelectBits & eSELECT_ALL ) != 0 )
		{
			pxSocket->pxSocketSet = ( SocketSelect_t *)xSocketSet;
		}
		else
		{
			/* disconnect it from the socket set */
			pxSocket->pxSocketSet = ( SocketSelect_t *)NULL;
		}
	}

#endif /* ipconfigSUPPORT_SELECT_FUNCTION == 1 */
/*-----------------------------------------------------------*/


#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )

	/* Test if a socket belongs to a socket-set */
	EventBits_t FreeRTOS_FD_ISSET( Socket_t xSocket, SocketSet_t xSocketSet )
	{
	EventBits_t xReturn;
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;

		configASSERT( pxSocket != NULL );
		configASSERT( xSocketSet != NULL );

		if( xSocketSet == ( SocketSet_t ) pxSocket->pxSocketSet )
		{
			/* Make sure we're not adding bits which are reserved for internal
			use. */
			xReturn = pxSocket->xSocketBits & eSELECT_ALL;
		}
		else
		{
			xReturn = 0;
		}

		return xReturn;
	}

#endif /* ipconfigSUPPORT_SELECT_FUNCTION == 1 */
/*-----------------------------------------------------------*/

#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )

	/* The select() statement: wait for an event to occur on any of the sockets
	included in a socket set */
	BaseType_t FreeRTOS_select( SocketSet_t xSocketSet, TickType_t xBlockTimeTicks )
	{
	TimeOut_t xTimeOut;
	TickType_t xRemainingTime;
	SocketSelect_t *pxSocketSet = ( SocketSelect_t*) xSocketSet;
	BaseType_t xResult;

		configASSERT( xSocketSet != NULL );

		/* Only in the first round, check for non-blocking */
		xRemainingTime = xBlockTimeTicks;

		/* Fetch the current time */
		vTaskSetTimeOutState( &xTimeOut );

		for( ;; )
		{
			/* Find a socket which might have triggered the bit
			This function might return immediately or block for a limited time */
			xResult = ( BaseType_t ) xEventGroupWaitBits( pxSocketSet->xSelectGroup, eSELECT_ALL, pdFALSE, pdFALSE, xRemainingTime );

			#if( ipconfigSUPPORT_SIGNALS != 0 )
			{
				if( ( xResult & eSELECT_INTR ) != 0u )
				{
					xEventGroupClearBits( pxSocketSet->xSelectGroup, eSELECT_INTR );
					FreeRTOS_debug_printf( ( "FreeRTOS_select: interrupted\n" ) );
					break;
				}
			}
			#endif /* ipconfigSUPPORT_SIGNALS */

			/* Have the IP-task find the socket which had an event */
			pxSocketSet->bApiCalled = pdTRUE;
			prvFindSelectedSocket( pxSocketSet );

			xResult = ( BaseType_t ) xEventGroupGetBits( pxSocketSet->xSelectGroup );

			if( xResult != 0 )
			{
				break;
			}

			/* Has the timeout been reached? */
			if( xTaskCheckForTimeOut( &xTimeOut, &xRemainingTime ) != pdFALSE )
			{
				break;
			}
		}

		return xResult;
	}

#endif /* ipconfigSUPPORT_SELECT_FUNCTION */
/*-----------------------------------------------------------*/

#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )

	/* Send a message to the IP-task to have it check all sockets belonging to
	'pxSocketSet' */
	static FreeRTOS_Socket_t *prvFindSelectedSocket( SocketSelect_t *pxSocketSet )
	{
	IPStackEvent_t xSelectEvent;
	FreeRTOS_Socket_t *xReturn;

		xSelectEvent.eEventType = eSocketSelectEvent;
		xSelectEvent.pvData = ( void * ) pxSocketSet;

		/* while the IP-task works on the request, the API will block on
		'eSELECT_CALL_IP'.  So clear it first. */
		xEventGroupClearBits( pxSocketSet->xSelectGroup, eSELECT_CALL_IP );

		/* Now send the socket select event */
		if( xSendEventStructToIPTask( &xSelectEvent, ( TickType_t ) portMAX_DELAY ) == pdFAIL )
		{
			/* Oops, we failed to wake-up the IP task. No use to wait for it. */
			FreeRTOS_debug_printf( ( "prvFindSelectedSocket: failed\n" ) );
			xReturn = NULL;
		}
		else
		{
			/* As soon as the IP-task is ready, it will set 'eSELECT_CALL_IP' to
			wakeup the calling API */
			xEventGroupWaitBits( pxSocketSet->xSelectGroup, eSELECT_CALL_IP, pdTRUE, pdFALSE, portMAX_DELAY );

			/* Return 'pxSocket' which is set by the IP-task */
			xReturn = pxSocketSet->pxSocket;
		}

		return xReturn;
	}

#endif /* ipconfigSUPPORT_SELECT_FUNCTION == 1 */
/*-----------------------------------------------------------*/

/*
 * FreeRTOS_recvfrom: receive data from a bound socket
 * In this library, the function can only be used with connectionsless sockets
 * (UDP)
 */
int32_t FreeRTOS_recvfrom( Socket_t xSocket, void *pvBuffer, size_t xBufferLength, BaseType_t xFlags, struct freertos_sockaddr *pxSourceAddress, socklen_t *pxSourceAddressLength )
{
BaseType_t lPacketCount = 0;
NetworkBufferDescriptor_t *pxNetworkBuffer;
FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
TickType_t xRemainingTime = ( TickType_t ) 0; /* Obsolete assignment, but some compilers output a warning if its not done. */
BaseType_t xTimed = pdFALSE;
TimeOut_t xTimeOut;
int32_t lReturn;
EventBits_t xEventBits = ( EventBits_t ) 0;

	if( prvValidSocket( pxSocket, FREERTOS_IPPROTO_UDP, pdTRUE ) == pdFALSE )
	{
		return -pdFREERTOS_ERRNO_EINVAL;
	}

	lPacketCount = ( BaseType_t ) listCURRENT_LIST_LENGTH( &( pxSocket->u.xUDP.xWaitingPacketsList ) );

	/* The function prototype is designed to maintain the expected Berkeley
	sockets standard, but this implementation does not use all the parameters. */
	( void ) pxSourceAddressLength;

	while( lPacketCount == 0 )
	{
		if( xTimed == pdFALSE )
		{
			/* Check to see if the socket is non blocking on the first
			iteration.  */
			xRemainingTime = pxSocket->xReceiveBlockTime;

			if( xRemainingTime == ( TickType_t ) 0 )
			{
				#if( ipconfigSUPPORT_SIGNALS != 0 )
				{
					/* Just check for the interrupt flag. */
					xEventBits = xEventGroupWaitBits( pxSocket->xEventGroup, eSOCKET_INTR,
						pdTRUE /*xClearOnExit*/, pdFALSE /*xWaitAllBits*/, socketDONT_BLOCK );
				}
				#endif /* ipconfigSUPPORT_SIGNALS */
				break;
			}

			if( ( xFlags & FREERTOS_MSG_DONTWAIT ) != 0 )
			{
				break;
			}

			/* To ensure this part only executes once. */
			xTimed = pdTRUE;

			/* Fetch the current time. */
			vTaskSetTimeOutState( &xTimeOut );
		}

		/* Wait for arrival of data.  While waiting, the IP-task may set the
		'eSOCKET_RECEIVE' bit in 'xEventGroup', if it receives data for this
		socket, thus unblocking this API call. */
		xEventBits = xEventGroupWaitBits( pxSocket->xEventGroup, eSOCKET_RECEIVE | eSOCKET_INTR,
			pdTRUE /*xClearOnExit*/, pdFALSE /*xWaitAllBits*/, xRemainingTime );

		#if( ipconfigSUPPORT_SIGNALS != 0 )
		{
			if( ( xEventBits & eSOCKET_INTR ) != 0 )
			{
				if( ( xEventBits & eSOCKET_RECEIVE ) != 0 )
				{
					/* Shouldn't have cleared the eSOCKET_RECEIVE flag. */
					xEventGroupSetBits( pxSocket->xEventGroup, eSOCKET_RECEIVE );
				}
				break;
			}
		}
		#else
		{
			( void ) xEventBits;
		}
		#endif /* ipconfigSUPPORT_SIGNALS */

		lPacketCount = ( BaseType_t ) listCURRENT_LIST_LENGTH( &( pxSocket->u.xUDP.xWaitingPacketsList ) );

		if( lPacketCount != 0 )
		{
			break;
		}

		/* Has the timeout been reached ? */
		if( xTaskCheckForTimeOut( &xTimeOut, &xRemainingTime ) )
		{
			break;
		}
	} /* while( lPacketCount == 0 ) */

	if( lPacketCount != 0 )
	{
		taskENTER_CRITICAL();
		{
			/* The owner of the list item is the network buffer. */
			pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) listGET_OWNER_OF_HEAD_ENTRY( &( pxSocket->u.xUDP.xWaitingPacketsList ) );

			if( ( xFlags & FREERTOS_MSG_PEEK ) == 0 )
			{
				/* Remove the network buffer from the list of buffers waiting to
				be processed by the socket. */
				uxListRemove( &( pxNetworkBuffer->xBufferListItem ) );
			}
		}
		taskEXIT_CRITICAL();

		/* The returned value is the data length, which may have been capped to
		the receive buffer size. */
		lReturn = ( int32_t ) pxNetworkBuffer->xDataLength;

		if( pxSourceAddress != NULL )
		{
			pxSourceAddress->sin_port = pxNetworkBuffer->usPort;
			pxSourceAddress->sin_addr = pxNetworkBuffer->ulIPAddress;
		}

		if( ( xFlags & FREERTOS_ZERO_COPY ) == 0 )
		{
			/* The zero copy flag is not set.  Truncate the length if it won't
			fit in the provided buffer. */
			if( lReturn > ( int32_t ) xBufferLength )
			{
				iptraceRECVFROM_DISCARDING_BYTES( ( xBufferLength - lReturn ) );
				lReturn = ( int32_t )xBufferLength;
			}

			/* Copy the received data into the provided buffer, then release the
			network buffer. */
			memcpy( pvBuffer, ( void * ) &( pxNetworkBuffer->pucEthernetBuffer[ ipUDP_PAYLOAD_OFFSET_IPv4 ] ), ( size_t )lReturn );

			if( ( xFlags & FREERTOS_MSG_PEEK ) == 0 )
			{
				vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
			}
		}
		else
		{
			/* The zero copy flag was set.  pvBuffer is not a buffer into which
			the received data can be copied, but a pointer that must be set to
			point to the buffer in which the received data has already been
			placed. */
			*( ( void** ) pvBuffer ) = ( void * ) ( &( pxNetworkBuffer->pucEthernetBuffer[ ipUDP_PAYLOAD_OFFSET_IPv4 ] ) );
		}

	}
#if( ipconfigSUPPORT_SIGNALS != 0 )
	else if( ( xEventBits & eSOCKET_INTR ) != 0 )
	{
		lReturn = -pdFREERTOS_ERRNO_EINTR;
		iptraceRECVFROM_INTERRUPTED();
	}
#endif /* ipconfigSUPPORT_SIGNALS */
	else
	{
		lReturn = -pdFREERTOS_ERRNO_EWOULDBLOCK;
		iptraceRECVFROM_TIMEOUT();
	}

	return lReturn;
}
/*-----------------------------------------------------------*/

int32_t FreeRTOS_sendto( Socket_t xSocket, const void *pvBuffer, size_t xTotalDataLength, BaseType_t xFlags, const struct freertos_sockaddr *pxDestinationAddress, socklen_t xDestinationAddressLength )
{
NetworkBufferDescriptor_t *pxNetworkBuffer;
IPStackEvent_t xStackTxEvent = { eStackTxEvent, NULL };
TimeOut_t xTimeOut;
TickType_t xTicksToWait;
int32_t lReturn = 0;
FreeRTOS_Socket_t *pxSocket;

	pxSocket = ( FreeRTOS_Socket_t * ) xSocket;

	/* The function prototype is designed to maintain the expected Berkeley
	sockets standard, but this implementation does not use all the
	parameters. */
	( void ) xDestinationAddressLength;
	configASSERT( pvBuffer );

	if( xTotalDataLength <= ( size_t ) ipMAX_UDP_PAYLOAD_LENGTH )
	{
		/* If the socket is not already bound to an address, bind it now.
		Passing NULL as the address parameter tells FreeRTOS_bind() to select
		the address to bind to. */
		if( ( socketSOCKET_IS_BOUND( pxSocket ) != pdFALSE ) ||
			( FreeRTOS_bind( xSocket, NULL, 0u ) == 0 ) )
		{
			xTicksToWait = pxSocket->xSendBlockTime;

			#if( ipconfigUSE_CALLBACKS != 0 )
			{
				if( xIsCallingFromIPTask() != pdFALSE )
				{
					/* If this send function is called from within a call-back
					handler it may not block, otherwise chances would be big to
					get a deadlock: the IP-task waiting for itself. */
					xTicksToWait = ( TickType_t )0;
				}
			}
			#endif /* ipconfigUSE_CALLBACKS */

			if( ( xFlags & FREERTOS_MSG_DONTWAIT ) != 0 )
			{
				xTicksToWait = ( TickType_t ) 0;
			}

			if( ( xFlags & FREERTOS_ZERO_COPY ) == 0 )
			{
				/* Zero copy is not set, so obtain a network buffer into
				which the payload will be copied. */
				vTaskSetTimeOutState( &xTimeOut );

				/* Block until a buffer becomes available, or until a
				timeout has been reached */
				pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( xTotalDataLength + sizeof( UDPPacket_t ), xTicksToWait );

				if( pxNetworkBuffer != NULL )
				{
					memcpy( ( void * ) &( pxNetworkBuffer->pucEthernetBuffer[ ipUDP_PAYLOAD_OFFSET_IPv4 ] ), ( void * ) pvBuffer, xTotalDataLength );

					if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdTRUE )
					{
						/* The entire block time has been used up. */
						xTicksToWait = ( TickType_t ) 0;
					}
				}
			}
			else
			{
				/* When zero copy is used, pvBuffer is a pointer to the
				payload of a buffer that has already been obtained from the
				stack.  Obtain the network buffer pointer from the buffer. */
				pxNetworkBuffer = pxUDPPayloadBuffer_to_NetworkBuffer( (void*)pvBuffer );
			}

			if( pxNetworkBuffer != NULL )
			{
				pxNetworkBuffer->xDataLength = xTotalDataLength;
				pxNetworkBuffer->usPort = pxDestinationAddress->sin_port;
				pxNetworkBuffer->usBoundPort = ( uint16_t ) socketGET_SOCKET_PORT( pxSocket );
				pxNetworkBuffer->ulIPAddress = pxDestinationAddress->sin_addr;

				/* The socket options are passed to the IP layer in the
				space that will eventually get used by the Ethernet header. */
				pxNetworkBuffer->pucEthernetBuffer[ ipSOCKET_OPTIONS_OFFSET ] = pxSocket->ucSocketOptions;

				/* Tell the networking task that the packet needs sending. */
				xStackTxEvent.pvData = pxNetworkBuffer;

				/* Ask the IP-task to send this packet */
				if( xSendEventStructToIPTask( &xStackTxEvent, xTicksToWait ) == pdPASS )
				{
					/* The packet was successfully sent to the IP task. */
					lReturn = ( int32_t ) xTotalDataLength;
					#if( ipconfigUSE_CALLBACKS == 1 )
					{
						if( ipconfigIS_VALID_PROG_ADDRESS( pxSocket->u.xUDP.pxHandleSent ) )
						{
							pxSocket->u.xUDP.pxHandleSent( (Socket_t *)pxSocket, xTotalDataLength );
						}
					}
					#endif /* ipconfigUSE_CALLBACKS */
				}
				else
				{
					/* If the buffer was allocated in this function, release
					it. */
					if( ( xFlags & FREERTOS_ZERO_COPY ) == 0 )
					{
						vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
					}
					iptraceSTACK_TX_EVENT_LOST( ipSTACK_TX_EVENT );
				}
			}
			else
			{
				/* If errno was available, errno would be set to
				FREERTOS_ENOPKTS.  As it is, the function must return the
				number of transmitted bytes, so the calling function knows
				how	much data was actually sent. */
				iptraceNO_BUFFER_FOR_SENDTO();
			}
		}
		else
		{
			iptraceSENDTO_SOCKET_NOT_BOUND();
		}
	}
	else
	{
		/* The data is longer than the available buffer space. */
		iptraceSENDTO_DATA_TOO_LONG();
	}

	return lReturn;
} /* Tested */
/*-----------------------------------------------------------*/

/*
 * FreeRTOS_bind() : binds a sockt to a local port number.  If port 0 is
 * provided, a system provided port number will be assigned.  This function can
 * be used for both UDP and TCP sockets.  The actual binding will be performed
 * by the IP-task to avoid mutual access to the bound-socket-lists
 * (xBoundUDPSocketsList or xBoundTCPSocketsList).
 */
BaseType_t FreeRTOS_bind( Socket_t xSocket, struct freertos_sockaddr * pxAddress, socklen_t xAddressLength )
{
IPStackEvent_t xBindEvent;
FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
BaseType_t xReturn = 0;

	( void ) xAddressLength;

	if( ( pxSocket == NULL ) || ( pxSocket == FREERTOS_INVALID_SOCKET ) )
	{
		xReturn = -pdFREERTOS_ERRNO_EINVAL;
	}
	/* Once a socket is bound to a port, it can not be bound to a different
	port number */
	else if( socketSOCKET_IS_BOUND( pxSocket) != pdFALSE )
	{
		/* The socket is already bound. */
		FreeRTOS_debug_printf( ( "vSocketBind: Socket already bound to %d\n", pxSocket->usLocalPort ) );
		xReturn = -pdFREERTOS_ERRNO_EINVAL;
	}
	else
	{
		/* Prepare a messages to the IP-task in order to perform the binding.
		The desired port number will be passed in usLocalPort. */
		xBindEvent.eEventType = eSocketBindEvent;
		xBindEvent.pvData = ( void * ) xSocket;
		if( pxAddress != NULL )
		{
			pxSocket->usLocalPort = FreeRTOS_ntohs( pxAddress->sin_port );
		}
		else
		{
			/* Caller wants to bind to a random port number. */
			pxSocket->usLocalPort = 0u;
		}

		/* portMAX_DELAY is used as a the time-out parameter, as binding *must*
		succeed before the socket can be used.  _RB_ The use of an infinite
		block time needs be changed as it could result in the task hanging. */
		if( xSendEventStructToIPTask( &xBindEvent, ( TickType_t ) portMAX_DELAY ) == pdFAIL )
		{
			/* Failed to wake-up the IP-task, no use to wait for it */
			FreeRTOS_debug_printf( ( "FreeRTOS_bind: send event failed\n" ) );
			xReturn = -pdFREERTOS_ERRNO_ECANCELED;
		}
		else
		{
			/* The IP-task will set the 'eSOCKET_BOUND' bit when it has done its
			job. */
			xEventGroupWaitBits( pxSocket->xEventGroup, eSOCKET_BOUND, pdTRUE /*xClearOnExit*/, pdFALSE /*xWaitAllBits*/, portMAX_DELAY );
			if( socketSOCKET_IS_BOUND( pxSocket ) == pdFALSE )
			{
				xReturn = -pdFREERTOS_ERRNO_EINVAL;
			}
		}
	}

	return xReturn;
}

/*
 * vSocketBind(): internal version of bind() that should not be called directly.
 * 'xInternal' is used for TCP sockets only: it allows to have several
 * (connected) child sockets bound to the same server port.
 */
BaseType_t vSocketBind( FreeRTOS_Socket_t *pxSocket, struct freertos_sockaddr * pxAddress, size_t uxAddressLength, BaseType_t xInternal )
{
BaseType_t xReturn = 0; /* In Berkeley sockets, 0 means pass for bind(). */
List_t *pxSocketList;
#if( ipconfigALLOW_SOCKET_SEND_WITHOUT_BIND == 1 )
	struct freertos_sockaddr xAddress;
#endif /* ipconfigALLOW_SOCKET_SEND_WITHOUT_BIND */

#if( ipconfigUSE_TCP == 1 )
	if( pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_TCP )
	{
		pxSocketList = &xBoundTCPSocketsList;
	}
	else
#endif  /* ipconfigUSE_TCP == 1 */
	{
		pxSocketList = &xBoundUDPSocketsList;
	}

	/* The function prototype is designed to maintain the expected Berkeley
	sockets standard, but this implementation does not use all the parameters. */
	( void ) uxAddressLength;

	configASSERT( pxSocket );
	configASSERT( pxSocket != FREERTOS_INVALID_SOCKET );

	#if( ipconfigALLOW_SOCKET_SEND_WITHOUT_BIND == 1 )
	{
		/* pxAddress will be NULL if sendto() was called on a socket without the
		socket being bound to an address. In this case, automatically allocate
		an address and port to the socket. */
		if( pxAddress == NULL )
		{
			pxAddress = &xAddress;
			/* Put the port to zero to be assigned later. */
			pxAddress->sin_port = 0u;
		}
	}
	#endif /* ipconfigALLOW_SOCKET_SEND_WITHOUT_BIND == 1 */

	/* Sockets must be bound before calling FreeRTOS_sendto() if
	ipconfigALLOW_SOCKET_SEND_WITHOUT_BIND is not set to 1. */
	configASSERT( pxAddress );

	if( pxAddress != NULL )
	{
		if( pxAddress->sin_port == 0u )
		{
			pxAddress->sin_port = prvGetPrivatePortNumber( ( BaseType_t )pxSocket->ucProtocol );
			if( 0 == pxAddress->sin_port )
			{
				return -pdFREERTOS_ERRNO_EADDRNOTAVAIL;
			}
		}

		/* If vSocketBind() is called from the API FreeRTOS_bind() it has been
		confirmed that the socket was not yet bound to a port.  If it is called
		from the IP-task, no such check is necessary. */

		/* Check to ensure the port is not already in use.  If the bind is
		called internally, a port MAY be used by more than one socket. */
		if( ( ( xInternal == pdFALSE ) || ( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP ) ) &&
			( pxListFindListItemWithValue( pxSocketList, ( TickType_t ) pxAddress->sin_port ) != NULL ) )
		{
			FreeRTOS_debug_printf( ( "vSocketBind: %sP port %d in use\n",
				pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_TCP ? "TC" : "UD",
				FreeRTOS_ntohs( pxAddress->sin_port ) ) );
			xReturn = -pdFREERTOS_ERRNO_EADDRINUSE;
		}
		else
		{
			/* Allocate the port number to the socket.
			This macro will set 'xBoundSocketListItem->xItemValue' */
			socketSET_SOCKET_PORT( pxSocket, pxAddress->sin_port );

			/* And also store it in a socket field 'usLocalPort' in host-byte-order,
			mostly used for logging and debugging purposes */
			pxSocket->usLocalPort = FreeRTOS_ntohs( pxAddress->sin_port );

			/* Add the socket to the list of bound ports. */
			{
				/* If the network driver can iterate through 'xBoundUDPSocketsList',
				by calling xPortHasUDPSocket() then the IP-task must temporarily
				suspend the scheduler to keep the list in a consistent state. */
				#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 1 )
				{
					vTaskSuspendAll();
				}
				#endif /* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */

				/* Add the socket to 'xBoundUDPSocketsList' or 'xBoundTCPSocketsList' */
				vListInsertEnd( pxSocketList, &( pxSocket->xBoundSocketListItem ) );

				#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 1 )
				{
					xTaskResumeAll();
				}
				#endif /* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */
			}
		}
	}
	else
	{
		xReturn = -pdFREERTOS_ERRNO_EADDRNOTAVAIL;
		FreeRTOS_debug_printf( ( "vSocketBind: Socket no addr\n" ) );
	}

	if( xReturn != 0 )
	{
		iptraceBIND_FAILED( xSocket, ( FreeRTOS_ntohs( pxAddress->sin_port ) ) );
	}

	return xReturn;
} /* Tested */
/*-----------------------------------------------------------*/

/*
 * Close a socket and free the allocated space
 * In case of a TCP socket: the connection will not be closed automatically
 * Subsequent messages for the closed socket will be responded to with a RST
 * The IP-task will actually close the socket, after receiving a 'eSocketCloseEvent' message
 */
BaseType_t FreeRTOS_closesocket( Socket_t xSocket )
{
BaseType_t xResult;
#if( ipconfigUSE_TCP == 1 ) && ( ipconfigUSE_CALLBACKS == 1 )
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * )xSocket;
#endif
IPStackEvent_t xCloseEvent;
xCloseEvent.eEventType = eSocketCloseEvent;
xCloseEvent.pvData = ( void * ) xSocket;

	if( ( xSocket == NULL ) || ( xSocket == FREERTOS_INVALID_SOCKET ) )
	{
		xResult = 0;
	}
	else
	{
		#if( ( ipconfigUSE_TCP == 1 ) && ( ipconfigUSE_CALLBACKS == 1 ) )
		{
			if( pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_TCP )
			{
				/* Make sure that IP-task won't call the user callback's anymore */
				pxSocket->u.xTCP.pxHandleConnected = NULL;
				pxSocket->u.xTCP.pxHandleReceive = NULL;
				pxSocket->u.xTCP.pxHandleSent = NULL;
			}
		}
		#endif  /* ( ( ipconfigUSE_TCP == 1 ) && ( ipconfigUSE_CALLBACKS == 1 ) ) */

		/* Let the IP task close the socket to keep it synchronised	with the
		packet handling. */

		/* Note when changing the time-out value below, it must be checked who is calling
		this function. If it is called by the IP-task, a deadlock could occur.
		The IP-task would only call it in case of a user call-back */
		if( xSendEventStructToIPTask( &xCloseEvent, ( TickType_t ) 0 ) == pdFAIL )
		{
			FreeRTOS_debug_printf( ( "FreeRTOS_closesocket: failed\n" ) );
			xResult = -1;
		}
		else
		{
			xResult = 1;
		}
	}

	return xResult;
}

/* This is the internal version of FreeRTOS_closesocket()
 * It will be called by the IPtask only to avoid problems with synchronicity
 */
void *vSocketClose( FreeRTOS_Socket_t *pxSocket )
{
NetworkBufferDescriptor_t *pxNetworkBuffer;

	#if( ipconfigUSE_TCP == 1 )
	{
		/* For TCP: clean up a little more. */
		if( pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			#if( ipconfigUSE_TCP_WIN == 1 )
			{
				if( pxSocket->u.xTCP.pxAckMessage != NULL )
				{
					vReleaseNetworkBufferAndDescriptor( pxSocket->u.xTCP.pxAckMessage );
				}
				/* Free the resources which were claimed by the tcpWin member */
				vTCPWindowDestroy( &pxSocket->u.xTCP.xTCPWindow );
			}
			#endif /* ipconfigUSE_TCP_WIN */

			/* Free the input and output streams */
			if( pxSocket->u.xTCP.rxStream != NULL )
			{
				vPortFreeLarge( pxSocket->u.xTCP.rxStream );
			}

			if( pxSocket->u.xTCP.txStream != NULL )
			{
				vPortFreeLarge( pxSocket->u.xTCP.txStream );
			}

			/* In case this is a child socket, make sure the child-count of the
			parent socket is decreased. */
			prvTCPSetSocketCount( pxSocket );
		}
	}
	#endif  /* ipconfigUSE_TCP == 1 */

	/* Socket must be unbound first, to ensure no more packets are queued on
	it. */
	if( socketSOCKET_IS_BOUND( pxSocket ) != pdFALSE )
	{
		/* If the network driver can iterate through 'xBoundUDPSocketsList',
		by calling xPortHasUDPSocket(), then the IP-task must temporarily
		suspend the scheduler to keep the list in a consistent state. */
		#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 1 )
		{
			vTaskSuspendAll();
		}
		#endif /* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */

		uxListRemove( &( pxSocket->xBoundSocketListItem ) );

		#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 1 )
		{
			xTaskResumeAll();
		}
		#endif /* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */
	}

	/* Now the socket is not bound the list of waiting packets can be
	drained. */
	if( pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_UDP )
	{
		while( listCURRENT_LIST_LENGTH( &( pxSocket->u.xUDP.xWaitingPacketsList ) ) > 0U )
		{
			pxNetworkBuffer = ( NetworkBufferDescriptor_t * ) listGET_OWNER_OF_HEAD_ENTRY( &( pxSocket->u.xUDP.xWaitingPacketsList ) );
			uxListRemove( &( pxNetworkBuffer->xBufferListItem ) );
			vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
		}
	}

	if( pxSocket->xEventGroup )
	{
		vEventGroupDelete( pxSocket->xEventGroup );
	}

	#if( ipconfigUSE_TCP == 1 ) && ( ipconfigHAS_DEBUG_PRINTF != 0 )
	{
		if( pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			FreeRTOS_debug_printf( ( "FreeRTOS_closesocket[%u to %lxip:%u]: buffers %lu socks %lu\n",
				pxSocket->usLocalPort,
				pxSocket->u.xTCP.ulRemoteIP,
				pxSocket->u.xTCP.usRemotePort,
				uxGetNumberOfFreeNetworkBuffers(),
				listCURRENT_LIST_LENGTH( &xBoundTCPSocketsList ) ) );
		}
	}
	#endif /* ( ipconfigUSE_TCP == 1 ) && ( ipconfigHAS_DEBUG_PRINTF != 0 ) */

	/* Anf finally, after all resources have been freed, free the socket space */
	vPortFreeSocket( pxSocket );

	return 0;
} /* Tested */

/*-----------------------------------------------------------*/

#if ipconfigUSE_TCP == 1

	/*
	 * When a child socket gets closed, make sure to update the child-count of the
	 * parent.  When a listening parent socket is closed, make sure no child-sockets
	 * keep a pointer to it.
	 */
	static void prvTCPSetSocketCount( FreeRTOS_Socket_t *pxSocketToDelete )
	{
	const ListItem_t *pxIterator;
	const MiniListItem_t *pxEnd = ( const MiniListItem_t* )listGET_END_MARKER( &xBoundTCPSocketsList );
	FreeRTOS_Socket_t *pxOtherSocket;
	uint16_t usLocalPort = pxSocketToDelete->usLocalPort;

		for( pxIterator  = ( const ListItem_t * ) listGET_NEXT( pxEnd );
			 pxIterator != ( const ListItem_t * ) pxEnd;
			 pxIterator  = ( const ListItem_t * ) listGET_NEXT( pxIterator ) )
		{
			pxOtherSocket = ( FreeRTOS_Socket_t * ) listGET_LIST_ITEM_OWNER( pxIterator );
			if( ( pxOtherSocket->u.xTCP.ucTCPState == eTCP_LISTEN ) &&
				( pxOtherSocket->usLocalPort == usLocalPort ) &&
				( pxOtherSocket->u.xTCP.usChildCount ) )
			{
				pxOtherSocket->u.xTCP.usChildCount--;
				FreeRTOS_debug_printf( ( "Lost: Socket %u now has %u / %u child%s\n",
					pxOtherSocket->usLocalPort,
					pxOtherSocket->u.xTCP.usChildCount,
					pxOtherSocket->u.xTCP.usBacklog,
					pxOtherSocket->u.xTCP.usChildCount == 1u ? "" : "ren" ) );
				break;
			}
		}
	}

#endif /* ipconfigUSE_TCP == 1 */

/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_setsockopt( Socket_t xSocket, int32_t lLevel, int32_t lOptionName, const void *pvOptionValue, size_t xOptionLength )
{
/* The standard Berkeley function returns 0 for success. */
BaseType_t xReturn = -pdFREERTOS_ERRNO_EINVAL;
BaseType_t lOptionValue;
FreeRTOS_Socket_t *pxSocket;

	pxSocket = ( FreeRTOS_Socket_t * ) xSocket;

	/* The function prototype is designed to maintain the expected Berkeley
	sockets standard, but this implementation does not use all the parameters. */
	( void ) lLevel;
	( void ) xOptionLength;

	configASSERT( xSocket );

	switch( lOptionName )
	{
		case FREERTOS_SO_RCVTIMEO	:
			/* Receive time out. */
			pxSocket->xReceiveBlockTime = *( ( TickType_t * ) pvOptionValue );
			xReturn = 0;
			break;

		case FREERTOS_SO_SNDTIMEO	:
			pxSocket->xSendBlockTime = *( ( TickType_t * ) pvOptionValue );
			if( pxSocket->ucProtocol == ( uint8_t ) FREERTOS_IPPROTO_UDP )
			{
				/* The send time out is capped for the reason stated in the
				comments where ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS is defined
				in FreeRTOSIPConfig.h (assuming an official configuration file
				is being used. */
				if( pxSocket->xSendBlockTime > ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS )
				{
					pxSocket->xSendBlockTime = ipconfigUDP_MAX_SEND_BLOCK_TIME_TICKS;
				}
			}
			else
			{
				/* For TCP socket, it isn't necessary to limit the blocking time
				because	the FreeRTOS_send() function does not wait for a network
				buffer to become available. */
			}
			xReturn = 0;
			break;
		#if( ipconfigUDP_MAX_RX_PACKETS > 0 )
			case FREERTOS_SO_UDP_MAX_RX_PACKETS:
				if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_UDP )
				{
					break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
				}
				pxSocket->u.xUDP.uxMaxPackets = *( ( UBaseType_t * ) pvOptionValue );
				xReturn = 0;
				break;
		#endif /* ipconfigUDP_MAX_RX_PACKETS */

		case FREERTOS_SO_UDPCKSUM_OUT :
			/* Turn calculating of the UDP checksum on/off for this socket. */
			lOptionValue = ( BaseType_t ) pvOptionValue;

			if( lOptionValue == 0 )
			{
				pxSocket->ucSocketOptions &= ( uint8_t ) ~FREERTOS_SO_UDPCKSUM_OUT;
			}
			else
			{
				pxSocket->ucSocketOptions |= ( uint8_t ) FREERTOS_SO_UDPCKSUM_OUT;
			}
			xReturn = 0;
			break;

		#if( ipconfigUSE_CALLBACKS == 1 )
			#if( ipconfigUSE_TCP == 1 )
				case FREERTOS_SO_TCP_CONN_HANDLER:	/* Set a callback for (dis)connection events */
				case FREERTOS_SO_TCP_RECV_HANDLER:	/* Install a callback for receiving TCP data. Supply pointer to 'F_TCP_UDP_Handler_t' (see below) */
				case FREERTOS_SO_TCP_SENT_HANDLER:	/* Install a callback for sending TCP data. Supply pointer to 'F_TCP_UDP_Handler_t' (see below) */
			#endif /* ipconfigUSE_TCP */
				case FREERTOS_SO_UDP_RECV_HANDLER:	/* Install a callback for receiving UDP data. Supply pointer to 'F_TCP_UDP_Handler_t' (see below) */
				case FREERTOS_SO_UDP_SENT_HANDLER:	/* Install a callback for sending UDP data. Supply pointer to 'F_TCP_UDP_Handler_t' (see below) */
					{
						#if( ipconfigUSE_TCP == 1 )
						{
							UBaseType_t uxProtocol;
							if( ( lOptionName == FREERTOS_SO_UDP_RECV_HANDLER ) ||
								( lOptionName == FREERTOS_SO_UDP_SENT_HANDLER ) )
							{
								uxProtocol = ( UBaseType_t ) FREERTOS_IPPROTO_UDP;
							}
							else
							{
								uxProtocol = ( UBaseType_t ) FREERTOS_IPPROTO_TCP;
							}

							if( pxSocket->ucProtocol != ( uint8_t ) uxProtocol )
							{
								break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
							}
						}
						#else
						{
							/* No need to check if the socket has the right
							protocol, because only UDP socket can be created. */
						}
						#endif /* ipconfigUSE_TCP */

						switch( lOptionName )
						{
						#if ipconfigUSE_TCP == 1
							case FREERTOS_SO_TCP_CONN_HANDLER:
								pxSocket->u.xTCP.pxHandleConnected = ((F_TCP_UDP_Handler_t *)pvOptionValue)->pxOnTCPConnected;
								break;
							case FREERTOS_SO_TCP_RECV_HANDLER:
								pxSocket->u.xTCP.pxHandleReceive = ((F_TCP_UDP_Handler_t *)pvOptionValue)->pxOnTCPReceive;
								break;
							case FREERTOS_SO_TCP_SENT_HANDLER:
								pxSocket->u.xTCP.pxHandleSent = ((F_TCP_UDP_Handler_t *)pvOptionValue)->pxOnTCPSent;
								break;
						#endif /* ipconfigUSE_TCP */
						case FREERTOS_SO_UDP_RECV_HANDLER:
							pxSocket->u.xUDP.pxHandleReceive = ((F_TCP_UDP_Handler_t *)pvOptionValue)->pxOnUDPReceive;
							break;
						case FREERTOS_SO_UDP_SENT_HANDLER:
							pxSocket->u.xUDP.pxHandleSent = ((F_TCP_UDP_Handler_t *)pvOptionValue)->pxOnUDPSent;
							break;
						default:
							break;
						}
					}

					xReturn = 0;
					break;
		#endif /* ipconfigUSE_CALLBACKS */

		#if( ipconfigUSE_TCP != 0 )
			#if( ipconfigSOCKET_HAS_USER_SEMAPHORE != 0 )
				/* Each socket has a semaphore on which the using task normally
				sleeps. */
				case FREERTOS_SO_SET_SEMAPHORE:
					{
						pxSocket->pxUserSemaphore = *( ( SemaphoreHandle_t * ) pvOptionValue );
						xReturn = 0;
					}
					break;
			#endif /* ipconfigSOCKET_HAS_USER_SEMAPHORE */

			#if( ipconfigSOCKET_HAS_USER_WAKE_CALLBACK != 0 )
				case FREERTOS_SO_WAKEUP_CALLBACK:
				{
					/* Each socket can have a callback function that is executed
					when there is an event the socket's owner might want to
					process. */
					pxSocket->pxUserWakeCallback = ( SocketWakeupCallback_t ) pvOptionValue;
					xReturn = 0;
				}
				break;
			#endif /* ipconfigSOCKET_HAS_USER_WAKE_CALLBACK */

			case FREERTOS_SO_SNDBUF:	/* Set the size of the send buffer, in units of MSS (TCP only) */
			case FREERTOS_SO_RCVBUF:	/* Set the size of the receive buffer, in units of MSS (TCP only) */
				{
					uint32_t ulNewValue;

					if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
					{
						FreeRTOS_debug_printf( ( "Set SO_%sBUF: wrong socket type\n",
							( lOptionName == FREERTOS_SO_SNDBUF ) ? "SND" : "RCV" ) );
						break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
					}

					if( ( ( lOptionName == FREERTOS_SO_SNDBUF ) && ( pxSocket->u.xTCP.txStream != NULL ) ) ||
						( ( lOptionName == FREERTOS_SO_RCVBUF ) && ( pxSocket->u.xTCP.rxStream != NULL ) ) )
					{
						FreeRTOS_debug_printf( ( "Set SO_%sBUF: buffer already created\n",
							( lOptionName == FREERTOS_SO_SNDBUF ) ? "SND" : "RCV" ) );
						break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
					}

					ulNewValue = *( ( uint32_t * ) pvOptionValue );

					if( lOptionName == FREERTOS_SO_SNDBUF )
					{
						/* Round up to nearest MSS size */
						ulNewValue = FreeRTOS_round_up( ulNewValue, ( uint32_t ) pxSocket->u.xTCP.usInitMSS );
						pxSocket->u.xTCP.uxTxStreamSize = ulNewValue;
					}
					else
					{
						pxSocket->u.xTCP.uxRxStreamSize = ulNewValue;
					}
				}
				xReturn = 0;
				break;

			case FREERTOS_SO_WIN_PROPERTIES:	/* Set all buffer and window properties in one call, parameter is pointer to WinProperties_t */
				{
					WinProperties_t* pxProps;

					if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
					{
						FreeRTOS_debug_printf( ( "Set SO_WIN_PROP: wrong socket type\n" ) );
						break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
					}

					if( ( pxSocket->u.xTCP.txStream != NULL ) || ( pxSocket->u.xTCP.rxStream != NULL ) )
					{
						FreeRTOS_debug_printf( ( "Set SO_WIN_PROP: buffer already created\n" ) );
						break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
					}

					pxProps = ( ( WinProperties_t * ) pvOptionValue );
					FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_SNDBUF, &( pxProps->lTxBufSize ), sizeof( pxProps->lTxBufSize ) );
					FreeRTOS_setsockopt( xSocket, 0, FREERTOS_SO_RCVBUF, &( pxProps->lRxBufSize ), sizeof( pxProps->lRxBufSize ) );
					#if( ipconfigUSE_TCP_WIN == 1 )
					{
						pxSocket->u.xTCP.uxRxWinSize = ( uint32_t )pxProps->lRxWinSize;	/* Fixed value: size of the TCP reception window */
						pxSocket->u.xTCP.uxTxWinSize = ( uint32_t )pxProps->lTxWinSize;	/* Fixed value: size of the TCP transmit window */
					}
					#else
					{
						pxSocket->u.xTCP.uxRxWinSize = 1u;
						pxSocket->u.xTCP.uxTxWinSize = 1u;
					}
					#endif

					/* In case the socket has already initialised its tcpWin,
					adapt the window size parameters */
					if( pxSocket->u.xTCP.xTCPWindow.u.bits.bHasInit != pdFALSE_UNSIGNED )
					{
						pxSocket->u.xTCP.xTCPWindow.xSize.ulRxWindowLength = pxSocket->u.xTCP.uxRxWinSize * pxSocket->u.xTCP.usInitMSS;
						pxSocket->u.xTCP.xTCPWindow.xSize.ulTxWindowLength = pxSocket->u.xTCP.uxTxWinSize * pxSocket->u.xTCP.usInitMSS;
					}
				}

				xReturn = 0;
				break;

			case FREERTOS_SO_REUSE_LISTEN_SOCKET:	/* If true, the server-socket will turn into a connected socket */
				{
					if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
					{
						break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
					}
					if( *( ( BaseType_t * ) pvOptionValue ) != 0 )
					{
						pxSocket->u.xTCP.bits.bReuseSocket = pdTRUE_UNSIGNED;
					}
					else
					{
						pxSocket->u.xTCP.bits.bReuseSocket = pdFALSE_UNSIGNED;
					}
				}
				xReturn = 0;
				break;

			case FREERTOS_SO_CLOSE_AFTER_SEND:		/* As soon as the last byte has been transmitted, finalise the connection */
				{
					if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
					{
						break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
					}

					if( *( ( BaseType_t * ) pvOptionValue ) != 0 )
					{
						pxSocket->u.xTCP.bits.bCloseAfterSend = pdTRUE_UNSIGNED;
					}
					else
					{
						pxSocket->u.xTCP.bits.bCloseAfterSend = pdFALSE_UNSIGNED;
					}
				}
				xReturn = 0;
				break;

			case FREERTOS_SO_SET_FULL_SIZE:		/* Refuse to send packets smaller than MSS  */
				{
					if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
					{
						break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
					}

					if( *( ( BaseType_t * ) pvOptionValue ) != 0 )
					{
						pxSocket->u.xTCP.xTCPWindow.u.bits.bSendFullSize = pdTRUE_UNSIGNED;
					}
					else
					{
						pxSocket->u.xTCP.xTCPWindow.u.bits.bSendFullSize = pdFALSE_UNSIGNED;
					}

					if( ( pxSocket->u.xTCP.xTCPWindow.u.bits.bSendFullSize == pdFALSE_UNSIGNED ) &&
						( pxSocket->u.xTCP.ucTCPState >= eESTABLISHED ) &&
						( FreeRTOS_outstanding( pxSocket ) != 0 ) )
					{
						pxSocket->u.xTCP.usTimeout = 1u; /* to set/clear bSendFullSize */
						xSendEventToIPTask( eTCPTimerEvent );
					}
				}
				xReturn = 0;
				break;

			case FREERTOS_SO_STOP_RX:		/* Refuse to receive more packts */
				{
					if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
					{
						break;	/* will return -pdFREERTOS_ERRNO_EINVAL */
					}

					if( *( ( BaseType_t * ) pvOptionValue ) != 0 )
					{
						pxSocket->u.xTCP.bits.bRxStopped = pdTRUE_UNSIGNED;
					}
					else
					{
						pxSocket->u.xTCP.bits.bRxStopped = pdFALSE_UNSIGNED;
					}

					pxSocket->u.xTCP.bits.bWinChange = pdTRUE_UNSIGNED;
					pxSocket->u.xTCP.usTimeout = 1u; /* to set/clear bRxStopped */
					xSendEventToIPTask( eTCPTimerEvent );
				}
				xReturn = 0;
				break;

		#endif  /* ipconfigUSE_TCP == 1 */

		default :
			/* No other options are handled. */
			xReturn = -pdFREERTOS_ERRNO_ENOPROTOOPT;
			break;
	}

	return xReturn;
} /* Tested */

/*-----------------------------------------------------------*/

/* Find an available port number per https://tools.ietf.org/html/rfc6056. */
static uint16_t prvGetPrivatePortNumber( BaseType_t xProtocol )
{
const uint16_t usEphemeralPortCount =
	socketAUTO_PORT_ALLOCATION_MAX_NUMBER - socketAUTO_PORT_ALLOCATION_START_NUMBER + 1;
uint16_t usIterations = usEphemeralPortCount;
uint32_t ulRandomSeed = 0;
uint16_t usResult = 0;
BaseType_t xGotZeroOnce = pdFALSE;
const List_t *pxList;

#if ipconfigUSE_TCP == 1
	if( xProtocol == ( BaseType_t ) FREERTOS_IPPROTO_TCP )
	{
		pxList = &xBoundTCPSocketsList;
	}
	else
#endif
	{
		pxList = &xBoundUDPSocketsList;
	}

	/* Avoid compiler warnings if ipconfigUSE_TCP is not defined. */
	( void ) xProtocol;

	/* Find the next available port using the random seed as a starting
	point. */
	do
	{
		/* Generate a random seed. */
		ulRandomSeed = ipconfigRAND32( );

		/* Only proceed if the random number generator succeeded. */
		if( 0 == ulRandomSeed )
		{
			if( pdFALSE == xGotZeroOnce )
			{
				xGotZeroOnce = pdTRUE;
				continue;
			}
			else
			{
				break;
			}
		}

		/* Map the random to a candidate port. */
		usResult =
			socketAUTO_PORT_ALLOCATION_START_NUMBER +
			( ( ( uint16_t )ulRandomSeed ) % usEphemeralPortCount );

		/* Check if there's already an open socket with the same protocol
		and port. */
		if( NULL == pxListFindListItemWithValue(
			pxList,
			( TickType_t )FreeRTOS_htons( usResult ) ) )
		{
			usResult = FreeRTOS_htons( usResult );
			break;
		}
		else
		{
			usResult = 0;
		}

		usIterations--;
	}
	while( usIterations > 0 );

	return usResult;
}
/*-----------------------------------------------------------*/

/* pxListFindListItemWithValue: find a list item in a bound socket list
'xWantedItemValue' refers to a port number */
static const ListItem_t * pxListFindListItemWithValue( const List_t *pxList, TickType_t xWantedItemValue )
{
const ListItem_t * pxResult = NULL;

	if( ( xIPIsNetworkTaskReady() != pdFALSE ) && ( pxList != NULL ) )
	{
		const ListItem_t *pxIterator;
		const MiniListItem_t *pxEnd = ( const MiniListItem_t* )listGET_END_MARKER( pxList );
		for( pxIterator  = ( const ListItem_t * ) listGET_NEXT( pxEnd );
			 pxIterator != ( const ListItem_t * ) pxEnd;
			 pxIterator  = ( const ListItem_t * ) listGET_NEXT( pxIterator ) )
		{
			if( listGET_LIST_ITEM_VALUE( pxIterator ) == xWantedItemValue )
			{
				pxResult = pxIterator;
				break;
			}
		}
	}

	return pxResult;
} /* Tested */

/*-----------------------------------------------------------*/

FreeRTOS_Socket_t *pxUDPSocketLookup( UBaseType_t uxLocalPort )
{
const ListItem_t *pxListItem;
FreeRTOS_Socket_t *pxSocket = NULL;

	/* Looking up a socket is quite simple, find a match with the local port.

	See if there is a list item associated with the port number on the
	list of bound sockets. */
	pxListItem = pxListFindListItemWithValue( &xBoundUDPSocketsList, ( TickType_t ) uxLocalPort );

	if( pxListItem != NULL )
	{
		/* The owner of the list item is the socket itself. */
		pxSocket = ( FreeRTOS_Socket_t * ) listGET_LIST_ITEM_OWNER( pxListItem );
		configASSERT( pxSocket != NULL );
	}
	return pxSocket;
}

/*-----------------------------------------------------------*/

#if ipconfigINCLUDE_FULL_INET_ADDR == 1

	uint32_t FreeRTOS_inet_addr( const char * pcIPAddress )
	{
	const uint32_t ulDecimalBase = 10u;
	uint8_t ucOctet[ socketMAX_IP_ADDRESS_OCTETS ];
	const char *pcPointerOnEntering;
	uint32_t ulReturn = 0UL, ulValue;
	UBaseType_t uxOctetNumber;
	BaseType_t xResult = pdPASS;

		for( uxOctetNumber = 0u; uxOctetNumber < socketMAX_IP_ADDRESS_OCTETS; uxOctetNumber++ )
		{
			ulValue = 0ul;
			pcPointerOnEntering = pcIPAddress;

			while( ( *pcIPAddress >= '0' ) && ( *pcIPAddress <= '9' ) )
			{
				/* Move previous read characters into the next decimal
				position. */
				ulValue *= ulDecimalBase;

				/* Add the binary value of the ascii character. */
				ulValue += ( ( uint32_t ) ( *pcIPAddress ) - ( uint32_t ) '0' );

				/* Move to next character in the string. */
				pcIPAddress++;
			}

			/* Check characters were read. */
			if( pcIPAddress == pcPointerOnEntering )
			{
				xResult = pdFAIL;
			}

			/* Check the value fits in an 8-bit number. */
			if( ulValue > 0xffUL )
			{
				xResult = pdFAIL;
			}
			else
			{
				ucOctet[ uxOctetNumber ] = ( uint8_t ) ulValue;

				/* Check the next character is as expected. */
				if( uxOctetNumber < ( socketMAX_IP_ADDRESS_OCTETS - 1u ) )
				{
					if( *pcIPAddress != '.' )
					{
						xResult = pdFAIL;
					}
					else
					{
						/* Move past the dot. */
						pcIPAddress++;
					}
				}
			}

			if( xResult == pdFAIL )
			{
				/* No point going on. */
				break;
			}
		}

		if( *pcIPAddress != ( char ) 0 )
		{
			/* Expected the end of the string. */
			xResult = pdFAIL;
		}

		if( uxOctetNumber != socketMAX_IP_ADDRESS_OCTETS )
		{
			/* Didn't read enough octets. */
			xResult = pdFAIL;
		}

		if( xResult == pdPASS )
		{
			ulReturn = FreeRTOS_inet_addr_quick( ucOctet[ 0 ], ucOctet[ 1 ], ucOctet[ 2 ], ucOctet[ 3 ] );
		}

		return ulReturn;
	}

#endif /* ipconfigINCLUDE_FULL_INET_ADDR */

/*-----------------------------------------------------------*/

/* Function to get the local address and IP port */
size_t FreeRTOS_GetLocalAddress( Socket_t xSocket, struct freertos_sockaddr *pxAddress )
{
FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;

	/* IP address of local machine. */
	pxAddress->sin_addr = *ipLOCAL_IP_ADDRESS_POINTER;

	/* Local port on this machine. */
	pxAddress->sin_port = FreeRTOS_htons( pxSocket->usLocalPort );

	return sizeof( *pxAddress );
}

/*-----------------------------------------------------------*/

void vSocketWakeUpUser( FreeRTOS_Socket_t *pxSocket )
{
/* _HT_ must work this out, now vSocketWakeUpUser will be called for any important
 * event or transition */
	#if( ipconfigSOCKET_HAS_USER_SEMAPHORE == 1 )
	{
		if( pxSocket->pxUserSemaphore != NULL )
		{
			xSemaphoreGive( pxSocket->pxUserSemaphore );
		}
	}
	#endif /* ipconfigSOCKET_HAS_USER_SEMAPHORE */

	#if( ipconfigSOCKET_HAS_USER_WAKE_CALLBACK == 1 )
	{
		if( pxSocket->pxUserWakeCallback != NULL )
		{
			pxSocket->pxUserWakeCallback( pxSocket );
		}
	}
	#endif /* ipconfigSOCKET_HAS_USER_SEMAPHORE */

	#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
	{
		if( pxSocket->pxSocketSet != NULL )
		{
			EventBits_t xSelectBits = ( pxSocket->xEventBits >> SOCKET_EVENT_BIT_COUNT ) & eSELECT_ALL;
			if( xSelectBits != 0ul )
			{
				pxSocket->xSocketBits |= xSelectBits;
				xEventGroupSetBits( pxSocket->pxSocketSet->xSelectGroup, xSelectBits );
			}
		}

		pxSocket->xEventBits &= eSOCKET_ALL;
	}
	#endif /* ipconfigSUPPORT_SELECT_FUNCTION */

	if( ( pxSocket->xEventGroup != NULL ) && ( pxSocket->xEventBits != 0u ) )
	{
		xEventGroupSetBits( pxSocket->xEventGroup, pxSocket->xEventBits );
	}

	pxSocket->xEventBits = 0ul;
}

/*-----------------------------------------------------------*/

#if( ipconfigETHERNET_DRIVER_FILTERS_PACKETS == 1 )

	/* This define makes it possible for network-card drivers to inspect
	 * UDP message and see if there is any UDP socket bound to a given port
	 * number.
	 * This is probably only usefull in systems with a minimum of RAM and
	 * when lots of anonymous broadcast messages come in
	 */
	BaseType_t xPortHasUDPSocket( uint16_t usPortNr )
	{
	BaseType_t xFound = pdFALSE;

		vTaskSuspendAll();
		{
			if( ( pxListFindListItemWithValue( &xBoundUDPSocketsList, ( TickType_t ) usPortNr ) != NULL ) )
			{
				xFound = pdTRUE;
			}
		}
		xTaskResumeAll();

		return xFound;
	}

#endif /* ipconfigETHERNET_DRIVER_FILTERS_PACKETS */

/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	static BaseType_t bMayConnect( FreeRTOS_Socket_t *pxSocket );
	static BaseType_t bMayConnect( FreeRTOS_Socket_t *pxSocket )
	{
		switch( pxSocket->u.xTCP.ucTCPState )
		{
			case eCLOSED:
			case eCLOSE_WAIT:	return 0;
			case eCONNECT_SYN:	return -pdFREERTOS_ERRNO_EINPROGRESS;
			default:			return -pdFREERTOS_ERRNO_EAGAIN;
		}
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	static BaseType_t prvTCPConnectStart( FreeRTOS_Socket_t *pxSocket, struct freertos_sockaddr *pxAddress )
	{
	BaseType_t xResult = 0;

		if( prvValidSocket( pxSocket, FREERTOS_IPPROTO_TCP, pdFALSE ) == pdFALSE )
		{
			/* Not a valid socket or wrong type */
			xResult = -pdFREERTOS_ERRNO_EBADF;
		}
		else if( FreeRTOS_issocketconnected( pxSocket ) > 0 )
		{
			/* The socket is already connected. */
			xResult = -pdFREERTOS_ERRNO_EISCONN;
		}
		else if( socketSOCKET_IS_BOUND( pxSocket ) == pdFALSE )
		{
			/* Bind the socket to the port that the client task will send from.
			Non-standard, so the error returned is that returned by bind(). */
			xResult = FreeRTOS_bind( ( Socket_t ) pxSocket, NULL, 0u );
		}

		if( xResult == 0 )
		{
			/* Check if it makes any sense to wait for a connect event, this condition
			might change while sleeping, so it must be checked within each loop */
			xResult = bMayConnect( pxSocket ); /* -EINPROGRESS, -EAGAIN, or 0 for OK */

			/* Start the connect procedure, kernel will start working on it */
			if( xResult == 0 )
			{
				pxSocket->u.xTCP.bits.bConnPrepared = pdFALSE_UNSIGNED;
				pxSocket->u.xTCP.ucRepCount = 0u;

				FreeRTOS_debug_printf( ( "FreeRTOS_connect: %u to %lxip:%u\n",
					pxSocket->usLocalPort, FreeRTOS_ntohl( pxAddress->sin_addr ), FreeRTOS_ntohs( pxAddress->sin_port ) ) );

				/* Port on remote machine. */
				pxSocket->u.xTCP.usRemotePort = FreeRTOS_ntohs( pxAddress->sin_port );

				/* IP address of remote machine. */
				pxSocket->u.xTCP.ulRemoteIP = FreeRTOS_ntohl( pxAddress->sin_addr );

				/* (client) internal state: socket wants to send a connect. */
				vTCPStateChange( pxSocket, eCONNECT_SYN );

				/* To start an active connect. */
				pxSocket->u.xTCP.usTimeout = 1u;

				if( xSendEventToIPTask( eTCPTimerEvent ) != pdPASS )
				{
					xResult = -pdFREERTOS_ERRNO_ECANCELED;
				}
			}
		}

		return xResult;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/*
	 * FreeRTOS_connect: socket wants to connect to a remote port
	 */
	BaseType_t FreeRTOS_connect( Socket_t xClientSocket, struct freertos_sockaddr *pxAddress, socklen_t xAddressLength )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t* ) xClientSocket;
	TickType_t xRemainingTime;
	BaseType_t xTimed = pdFALSE;
	BaseType_t xResult;
	TimeOut_t xTimeOut;

		( void ) xAddressLength;

		xResult = prvTCPConnectStart( pxSocket, pxAddress );

		if( xResult == 0 )
		{
			/* And wait for the result */
			for( ;; )
			{
				if( xTimed == pdFALSE )
				{
					/* Only in the first round, check for non-blocking */
					xRemainingTime = pxSocket->xReceiveBlockTime;
					if( xRemainingTime == ( TickType_t )0 )
					{
						/* Not yet connected, correct state, non-blocking. */
						xResult = -pdFREERTOS_ERRNO_EWOULDBLOCK;
						break;
					}

					/* Don't get here a second time. */
					xTimed = pdTRUE;

					/* Fetch the current time */
					vTaskSetTimeOutState( &xTimeOut );
				}

				/* Did it get connected while sleeping ? */
				xResult = FreeRTOS_issocketconnected( pxSocket );

				/* Returns positive when connected, negative means an error */
				if( xResult < 0 )
				{
					/* Return the error */
					break;
				}

				if( xResult > 0 )
				{
					/* Socket now connected, return a zero */
					xResult = 0;
					break;
				}

				/* Is it allowed to sleep more? */
				if( xTaskCheckForTimeOut( &xTimeOut, &xRemainingTime ) )
				{
					xResult = -pdFREERTOS_ERRNO_ETIMEDOUT;
					break;
				}

				/* Go sleeping until we get any down-stream event */
				xEventGroupWaitBits( pxSocket->xEventGroup, eSOCKET_CONNECT, pdTRUE /*xClearOnExit*/, pdFALSE /*xWaitAllBits*/, xRemainingTime );
			}
		}

		return xResult;
	}
#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/*
	 * FreeRTOS_accept: can return a new connected socket
	 * if the server socket is in listen mode and receives a connection request
	 * The new socket will be bound already to the same port number as the listing
	 * socket.
	 */
	Socket_t FreeRTOS_accept( Socket_t xServerSocket, struct freertos_sockaddr *pxAddress, socklen_t *pxAddressLength )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xServerSocket;
	FreeRTOS_Socket_t *pxClientSocket = NULL;
	TickType_t xRemainingTime;
	BaseType_t xTimed = pdFALSE, xAsk = pdFALSE;
	TimeOut_t xTimeOut;
	IPStackEvent_t xAskEvent;

		if( prvValidSocket( pxSocket, FREERTOS_IPPROTO_TCP, pdTRUE ) == pdFALSE )
		{
			/* Not a valid socket or wrong type */
			pxClientSocket = ( FreeRTOS_Socket_t * ) FREERTOS_INVALID_SOCKET;
		}
		else if( ( pxSocket->u.xTCP.bits.bReuseSocket == pdFALSE_UNSIGNED ) &&
				 ( pxSocket->u.xTCP.ucTCPState != eTCP_LISTEN ) )
		{
			/* Parent socket is not in listening mode */
			pxClientSocket = ( FreeRTOS_Socket_t * ) FREERTOS_INVALID_SOCKET;
		}
		else
		{
			/* Loop will stop with breaks. */
			for( ; ; )
			{
				/* Is there a new client? */
				vTaskSuspendAll();
				{
					if( pxSocket->u.xTCP.bits.bReuseSocket == pdFALSE_UNSIGNED )
					{
						pxClientSocket = pxSocket->u.xTCP.pxPeerSocket;
					}
					else
					{
						pxClientSocket = pxSocket;
					}
					if( pxClientSocket != NULL )
					{
						pxSocket->u.xTCP.pxPeerSocket = NULL;

						/* Is it still not taken ? */
						if( pxClientSocket->u.xTCP.bits.bPassAccept != pdFALSE_UNSIGNED )
						{
							pxClientSocket->u.xTCP.bits.bPassAccept = pdFALSE_UNSIGNED;
						}
						else
						{
							pxClientSocket = NULL;
						}
					}
				}
				xTaskResumeAll();

				if( pxClientSocket != NULL )
				{
					if( pxAddress != NULL )
					{
						/* IP address of remote machine. */
						pxAddress->sin_addr = FreeRTOS_ntohl( pxClientSocket->u.xTCP.ulRemoteIP );

						/* Port on remote machine. */
						pxAddress->sin_port = FreeRTOS_ntohs( pxClientSocket->u.xTCP.usRemotePort );
					}
					if( pxAddressLength != NULL )
					{
						*pxAddressLength = sizeof( *pxAddress );
					}

					if( pxSocket->u.xTCP.bits.bReuseSocket == pdFALSE_UNSIGNED )
					{
						xAsk = pdTRUE;
					}
				}

				if( xAsk != pdFALSE )
				{
					/* Ask to set an event in 'xEventGroup' as soon as a new
					client gets connected for this listening socket. */
					xAskEvent.eEventType = eTCPAcceptEvent;
					xAskEvent.pvData = ( void * ) pxSocket;
					xSendEventStructToIPTask( &xAskEvent, portMAX_DELAY );
				}

				if( pxClientSocket != NULL )
				{
					break;
				}

				if( xTimed == pdFALSE )
				{
					/* Only in the first round, check for non-blocking */
					xRemainingTime = pxSocket->xReceiveBlockTime;
					if( xRemainingTime == ( TickType_t ) 0 )
					{
						break;
					}

					/* Don't get here a second time */
					xTimed = pdTRUE;

					/* Fetch the current time */
					vTaskSetTimeOutState( &xTimeOut );
				}

				/* Has the timeout been reached? */
				if( xTaskCheckForTimeOut( &xTimeOut, &xRemainingTime ) != pdFALSE )
				{
					break;
				}

				/* Go sleeping until we get any down-stream event */
				xEventGroupWaitBits( pxSocket->xEventGroup, eSOCKET_ACCEPT, pdTRUE /*xClearOnExit*/, pdFALSE /*xWaitAllBits*/, xRemainingTime );
			}
		}

		return ( Socket_t ) pxClientSocket;
	}
#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/*
	 * Read incoming data from a TCP socket
	 * Only after the last byte has been read, a close error might be returned
	 */
	BaseType_t FreeRTOS_recv( Socket_t xSocket, void *pvBuffer, size_t xBufferLength, BaseType_t xFlags )
	{
	BaseType_t xByteCount;
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	TickType_t xRemainingTime;
	BaseType_t xTimed = pdFALSE;
	TimeOut_t xTimeOut;
	EventBits_t xEventBits = ( EventBits_t ) 0;

		/* Check if the socket is valid, has type TCP and if it is bound to a
		port. */
		if( prvValidSocket( pxSocket, FREERTOS_IPPROTO_TCP, pdTRUE ) == pdFALSE )
		{
			xByteCount = -pdFREERTOS_ERRNO_EINVAL;
		}
		else
		{
			if( pxSocket->u.xTCP.rxStream != NULL )
			{
				xByteCount = ( BaseType_t )uxStreamBufferGetSize ( pxSocket->u.xTCP.rxStream );
			}
			else
			{
				xByteCount = 0;
			}

			while( xByteCount == 0 )
			{
				switch( pxSocket->u.xTCP.ucTCPState )
				{
				case eCLOSED:
				case eCLOSE_WAIT:	/* (server + client) waiting for a connection termination request from the local user. */
				case eCLOSING:		/* (server + client) waiting for a connection termination request acknowledgement from the remote TCP. */
					if( pxSocket->u.xTCP.bits.bMallocError != pdFALSE_UNSIGNED )
					{
						/* The no-memory error has priority above the non-connected error.
						Both are fatal and will elad to closing the socket. */
						xByteCount = -pdFREERTOS_ERRNO_ENOMEM;
					}
					else
					{
						xByteCount = -pdFREERTOS_ERRNO_ENOTCONN;
					}
					/* Call continue to break out of the switch and also the while
					loop. */
					continue;
				default:
					break;
				}

				if( xTimed == pdFALSE )
				{
					/* Only in the first round, check for non-blocking. */
					xRemainingTime = pxSocket->xReceiveBlockTime;

					if( xRemainingTime == ( TickType_t ) 0 )
					{
						#if( ipconfigSUPPORT_SIGNALS != 0 )
						{
							/* Just check for the interrupt flag. */
							xEventBits = xEventGroupWaitBits( pxSocket->xEventGroup, eSOCKET_INTR,
								pdTRUE /*xClearOnExit*/, pdFALSE /*xWaitAllBits*/, socketDONT_BLOCK );
						}
						#endif /* ipconfigSUPPORT_SIGNALS */
						break;
					}

					if( ( xFlags & FREERTOS_MSG_DONTWAIT ) != 0 )
					{
						break;
					}

					/* Don't get here a second time. */
					xTimed = pdTRUE;

					/* Fetch the current time. */
					vTaskSetTimeOutState( &xTimeOut );
				}

				/* Has the timeout been reached? */
				if( xTaskCheckForTimeOut( &xTimeOut, &xRemainingTime ) != pdFALSE )
				{
					break;
				}

				/* Block until there is a down-stream event. */
				xEventBits = xEventGroupWaitBits( pxSocket->xEventGroup,
					eSOCKET_RECEIVE | eSOCKET_CLOSED | eSOCKET_INTR,
					pdTRUE /*xClearOnExit*/, pdFALSE /*xWaitAllBits*/, xRemainingTime );
				#if( ipconfigSUPPORT_SIGNALS != 0 )
				{
					if( ( xEventBits & eSOCKET_INTR ) != 0u )
					{
						break;
					}
				}
				#else
				{
					( void ) xEventBits;
				}
				#endif /* ipconfigSUPPORT_SIGNALS */

				if( pxSocket->u.xTCP.rxStream != NULL )
				{
					xByteCount = ( BaseType_t ) uxStreamBufferGetSize ( pxSocket->u.xTCP.rxStream );
				}
				else
				{
					xByteCount = 0;
				}
			}

		#if( ipconfigSUPPORT_SIGNALS != 0 )
			if( ( xEventBits & eSOCKET_INTR ) != 0 )
			{
				if( ( xEventBits & ( eSOCKET_RECEIVE | eSOCKET_CLOSED ) ) != 0 )
				{
					/* Shouldn't have cleared other flags. */
					xEventBits &= ~eSOCKET_INTR;
					xEventGroupSetBits( pxSocket->xEventGroup, xEventBits );
				}
				xByteCount = -pdFREERTOS_ERRNO_EINTR;
			}
			else
		#endif /* ipconfigSUPPORT_SIGNALS */
			if( xByteCount > 0 )
			{
				if( ( xFlags & FREERTOS_ZERO_COPY ) == 0 )
				{
					xByteCount = ( BaseType_t ) uxStreamBufferGet( pxSocket->u.xTCP.rxStream, 0ul, ( uint8_t * ) pvBuffer, ( size_t ) xBufferLength, ( xFlags & FREERTOS_MSG_PEEK ) != 0 );
					if( pxSocket->u.xTCP.bits.bLowWater != pdFALSE_UNSIGNED )
					{
						/* We had reached the low-water mark, now see if the flag
						can be cleared */
						size_t uxFrontSpace = uxStreamBufferFrontSpace( pxSocket->u.xTCP.rxStream );

						if( uxFrontSpace >= pxSocket->u.xTCP.uxEnoughSpace )
						{
							pxSocket->u.xTCP.bits.bLowWater = pdFALSE_UNSIGNED;
							pxSocket->u.xTCP.bits.bWinChange = pdTRUE_UNSIGNED;
							pxSocket->u.xTCP.usTimeout = 1u; /* because bLowWater is cleared. */
							xSendEventToIPTask( eTCPTimerEvent );
						}
					}
				}
				else
				{
					/* Zero-copy reception of data: pvBuffer is a pointer to a pointer. */
					xByteCount = ( BaseType_t ) uxStreamBufferGetPtr( pxSocket->u.xTCP.rxStream, (uint8_t **)pvBuffer );
				}
			}
		} /* prvValidSocket() */

		return xByteCount;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	static int32_t prvTCPSendCheck( FreeRTOS_Socket_t *pxSocket, size_t xDataLength )
	{
	int32_t xResult = 1;

		/* Is this a socket of type TCP and is it already bound to a port number ? */
		if( prvValidSocket( pxSocket, FREERTOS_IPPROTO_TCP, pdTRUE ) == pdFALSE )
		{
			xResult = -pdFREERTOS_ERRNO_EINVAL;
		}
		else if( pxSocket->u.xTCP.bits.bMallocError != pdFALSE_UNSIGNED )
		{
			xResult = -pdFREERTOS_ERRNO_ENOMEM;
		}
		else if( pxSocket->u.xTCP.ucTCPState == eCLOSED )
		{
			xResult = -pdFREERTOS_ERRNO_ENOTCONN;
		}
		else if( pxSocket->u.xTCP.bits.bFinSent != pdFALSE_UNSIGNED )
		{
			/* This TCP connection is closing already, the FIN flag has been sent.
			Maybe it is still delivering or receiving data.
			Return OK in order not to get closed/deleted too quickly */
			xResult = 0;
		}
		else if( xDataLength == 0ul )
		{
			/* send() is being called to send zero bytes */
			xResult = 0;
		}
		else if( pxSocket->u.xTCP.txStream == NULL )
		{
			/* Create the outgoing stream only when it is needed */
			prvTCPCreateStream( pxSocket, pdFALSE );

			if( pxSocket->u.xTCP.txStream == NULL )
			{
				xResult = -pdFREERTOS_ERRNO_ENOMEM;
			}
		}

		return xResult;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/* Get a direct pointer to the circular transmit buffer.
	'*pxLength' will contain the number of bytes that may be written. */
	uint8_t *FreeRTOS_get_tx_head( Socket_t xSocket, BaseType_t *pxLength )
	{
	uint8_t *pucReturn;
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	StreamBuffer_t *pxBuffer = pxSocket->u.xTCP.txStream;

		if( pxBuffer != NULL )
		{
		BaseType_t xSpace = ( BaseType_t ) uxStreamBufferGetSpace( pxBuffer );
		BaseType_t xRemain = ( BaseType_t ) ( pxBuffer->LENGTH - pxBuffer->uxHead );

			*pxLength = FreeRTOS_min_BaseType( xSpace, xRemain );
			pucReturn = pxBuffer->ucArray + pxBuffer->uxHead;
		}
		else
		{
			*pxLength = 0;
			pucReturn = NULL;
		}

		return pucReturn;
	}
#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )
	/*
	 * Send data using a TCP socket.  It is not necessary to have the socket
	 * connected already.  Outgoing data will be stored and delivered as soon as
	 * the socket gets connected.
	 */
	BaseType_t FreeRTOS_send( Socket_t xSocket, const void *pvBuffer, size_t uxDataLength, BaseType_t xFlags )
	{
	BaseType_t xByteCount;
	BaseType_t xBytesLeft;
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	TickType_t xRemainingTime;
	BaseType_t xTimed = pdFALSE;
	TimeOut_t xTimeOut;
	BaseType_t xCloseAfterSend;

		/* Prevent compiler warnings about unused parameters.  The parameter
		may be used in future versions. */
		( void ) xFlags;

		xByteCount = ( BaseType_t ) prvTCPSendCheck( pxSocket, uxDataLength );

		if( xByteCount > 0 )
		{
			/* xBytesLeft is number of bytes to send, will count to zero. */
			xBytesLeft = ( BaseType_t ) uxDataLength;

			/* xByteCount is number of bytes that can be sent now. */
			xByteCount = ( BaseType_t ) uxStreamBufferGetSpace( pxSocket->u.xTCP.txStream );

			/* While there are still bytes to be sent. */
			while( xBytesLeft > 0 )
			{
				/* If txStream has space. */
				if( xByteCount > 0 )
				{
					/* Don't send more than necessary. */
					if( xByteCount > xBytesLeft )
					{
						xByteCount = xBytesLeft;
					}

					/* Is the close-after-send flag set and is this really the
					last transmission? */
					if( ( pxSocket->u.xTCP.bits.bCloseAfterSend != pdFALSE_UNSIGNED ) && ( xByteCount == xBytesLeft ) )
					{
						xCloseAfterSend = pdTRUE;
					}
					else
					{
						xCloseAfterSend = pdFALSE;
					}

					/* The flag 'bCloseAfterSend' can be set before sending data
					using setsockopt()

					When the last data packet is being sent out, a FIN flag will
					be included to let the peer know that no more data is to be
					expected.  The use of 'bCloseAfterSend' is not mandatory, it
					is just a faster way of transferring files (e.g. when using
					FTP). */
					if( xCloseAfterSend != pdFALSE )
					{
						/* Now suspend the scheduler: sending the last data	and
						setting bCloseRequested must be done together */
						vTaskSuspendAll();
						pxSocket->u.xTCP.bits.bCloseRequested = pdTRUE_UNSIGNED;
					}

					xByteCount = ( BaseType_t ) uxStreamBufferAdd( pxSocket->u.xTCP.txStream, 0ul, ( const uint8_t * ) pvBuffer, ( size_t ) xByteCount );

					if( xCloseAfterSend != pdFALSE )
					{
						/* Now when the IP-task transmits the data, it will also
						see	that bCloseRequested is true and include the FIN
						flag to start closure of the connection. */
						xTaskResumeAll();
					}

					/* Send a message to the IP-task so it can work on this
					socket.  Data is sent, let the IP-task work on it. */
					pxSocket->u.xTCP.usTimeout = 1u;

					if( xIsCallingFromIPTask() == pdFALSE )
					{
						/* Only send a TCP timer event when not called from the
						IP-task. */
						xSendEventToIPTask( eTCPTimerEvent );
					}

					xBytesLeft -= xByteCount;

					if( xBytesLeft == 0 )
					{
						break;
					}

					/* As there are still bytes left to be sent, increase the
					data pointer. */
					pvBuffer = ( void * ) ( ( ( const uint8_t * ) pvBuffer) + xByteCount );
				}

				/* Not all bytes have been sent. In case the socket is marked as
				blocking sleep for a while. */
				if( xTimed == pdFALSE )
				{
					/* Only in the first round, check for non-blocking. */
					xRemainingTime = pxSocket->xSendBlockTime;

					#if( ipconfigUSE_CALLBACKS != 0 )
					{
						if( xIsCallingFromIPTask() != pdFALSE )
						{
							/* If this send function is called from within a
							call-back handler it may not block, otherwise
							chances would be big to get a deadlock: the IP-task
							waiting for	itself. */
							xRemainingTime = ( TickType_t ) 0;
						}
					}
					#endif /* ipconfigUSE_CALLBACKS */

					if( xRemainingTime == ( TickType_t ) 0 )
					{
						break;
					}

					if( ( xFlags & FREERTOS_MSG_DONTWAIT ) != 0 )
					{
						break;
					}

					/* Don't get here a second time. */
					xTimed = pdTRUE;

					/* Fetch the current time. */
					vTaskSetTimeOutState( &xTimeOut );
				}
				else
				{
					/* Has the timeout been reached? */
					if( xTaskCheckForTimeOut( &xTimeOut, &xRemainingTime ) != pdFALSE )
					{
						break;
					}
				}

				/* Go sleeping until down-stream events are received. */
				xEventGroupWaitBits( pxSocket->xEventGroup, eSOCKET_SEND | eSOCKET_CLOSED,
					pdTRUE /*xClearOnExit*/, pdFALSE /*xWaitAllBits*/, xRemainingTime );

				xByteCount = ( BaseType_t ) uxStreamBufferGetSpace( pxSocket->u.xTCP.txStream );
			}

			/* How much was actually sent? */
			xByteCount = ( ( BaseType_t ) uxDataLength ) - xBytesLeft;

			if( xByteCount == 0 )
			{
				if( pxSocket->u.xTCP.ucTCPState > eESTABLISHED )
				{
					xByteCount = ( BaseType_t ) -pdFREERTOS_ERRNO_ENOTCONN;
				}
				else
				{
					if( ipconfigTCP_MAY_LOG_PORT( pxSocket->usLocalPort ) != pdFALSE )
					{
						FreeRTOS_debug_printf( ( "FreeRTOS_send: %u -> %lxip:%d: no space\n",
							pxSocket->usLocalPort,
							pxSocket->u.xTCP.ulRemoteIP,
							pxSocket->u.xTCP.usRemotePort ) );
					}

					xByteCount = ( BaseType_t ) -pdFREERTOS_ERRNO_ENOSPC;
				}
			}
		}

		return xByteCount;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/*
	 * Request to put a socket in listen mode
	 */
	BaseType_t FreeRTOS_listen( Socket_t xSocket, BaseType_t xBacklog )
	{
	FreeRTOS_Socket_t *pxSocket;
	BaseType_t xResult = 0;

		pxSocket = ( FreeRTOS_Socket_t * ) xSocket;

		/* listen() is allowed for a valid TCP socket in Closed state and already
		bound. */
		if( prvValidSocket( pxSocket, FREERTOS_IPPROTO_TCP, pdTRUE ) == pdFALSE )
		{
			xResult = -pdFREERTOS_ERRNO_EOPNOTSUPP;
		}
		else if( ( pxSocket->u.xTCP.ucTCPState != eCLOSED ) && ( pxSocket->u.xTCP.ucTCPState != eCLOSE_WAIT ) )
		{
			/* Socket is in a wrong state. */
			xResult = -pdFREERTOS_ERRNO_EOPNOTSUPP;
		}
		else
		{
			/* Backlog is interpreted here as "the maximum number of child
			sockets. */
			pxSocket->u.xTCP.usBacklog = ( uint16_t )FreeRTOS_min_int32( ( int32_t ) 0xffff, ( int32_t ) xBacklog );

			/* This cleaning is necessary only if a listening socket is being
			reused as it might have had a previous connection. */
			if( pxSocket->u.xTCP.bits.bReuseSocket )
			{
				if( pxSocket->u.xTCP.rxStream != NULL )
				{
					vStreamBufferClear( pxSocket->u.xTCP.rxStream );
				}

				if( pxSocket->u.xTCP.txStream != NULL )
				{
					vStreamBufferClear( pxSocket->u.xTCP.txStream );
				}

				memset( pxSocket->u.xTCP.xPacket.u.ucLastPacket, '\0', sizeof( pxSocket->u.xTCP.xPacket.u.ucLastPacket ) );
				memset( &pxSocket->u.xTCP.xTCPWindow, '\0', sizeof( pxSocket->u.xTCP.xTCPWindow ) );
				memset( &pxSocket->u.xTCP.bits, '\0', sizeof( pxSocket->u.xTCP.bits ) );

				/* Now set the bReuseSocket flag again, because the bits have
				just been cleared. */
				pxSocket->u.xTCP.bits.bReuseSocket = pdTRUE_UNSIGNED;
			}

			vTCPStateChange( pxSocket, eTCP_LISTEN );
		}

		return xResult;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/* shutdown - shut down part of a full-duplex connection */
	BaseType_t FreeRTOS_shutdown( Socket_t xSocket, BaseType_t xHow )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xResult;

		if( prvValidSocket( pxSocket, FREERTOS_IPPROTO_TCP, pdTRUE ) == pdFALSE )
		{
			/*_RB_ Is this comment correct?  The socket is not of a type that
			supports the listen() operation. */
			xResult = -pdFREERTOS_ERRNO_EOPNOTSUPP;
		}
		else if ( pxSocket->u.xTCP.ucTCPState != eESTABLISHED )
		{
			/*_RB_ Is this comment correct?  The socket is not of a type that
			supports the listen() operation. */
			xResult = -pdFREERTOS_ERRNO_EOPNOTSUPP;
		}
		else
		{
			pxSocket->u.xTCP.bits.bUserShutdown = pdTRUE_UNSIGNED;

			/* Let the IP-task perform the shutdown of the connection. */
			pxSocket->u.xTCP.usTimeout = 1u;
			xSendEventToIPTask( eTCPTimerEvent );
			xResult = 0;
		}
		(void) xHow;

		return xResult;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/*
	 * A TCP timer has expired, now check all TCP sockets for:
	 * - Active connect
	 * - Send a delayed ACK
	 * - Send new data
	 * - Send a keep-alive packet
	 * - Check for timeout (in non-connected states only)
	 */
	TickType_t xTCPTimerCheck( BaseType_t xWillSleep )
	{
	FreeRTOS_Socket_t *pxSocket;
	TickType_t xShortest = pdMS_TO_TICKS( ( TickType_t ) ipTCP_TIMER_PERIOD_MS );
	TickType_t xNow = xTaskGetTickCount();
	static TickType_t xLastTime = 0u;
	TickType_t xDelta = xNow - xLastTime;
	ListItem_t* pxEnd = ( ListItem_t * ) listGET_END_MARKER( &xBoundTCPSocketsList );
	ListItem_t *pxIterator = ( ListItem_t * ) listGET_HEAD_ENTRY( &xBoundTCPSocketsList );

		xLastTime = xNow;

		if( xDelta == 0u )
		{
			xDelta = 1u;
		}

		while( pxIterator != pxEnd )
		{
			pxSocket = ( FreeRTOS_Socket_t * )listGET_LIST_ITEM_OWNER( pxIterator );
			pxIterator = ( ListItem_t * ) listGET_NEXT( pxIterator );

			/* Sockets with 'tmout == 0' do not need any regular attention. */
			if( pxSocket->u.xTCP.usTimeout == 0u )
			{
				continue;
			}

			if( xDelta < ( TickType_t ) pxSocket->u.xTCP.usTimeout )
			{
				pxSocket->u.xTCP.usTimeout = ( uint16_t ) ( ( ( TickType_t ) pxSocket->u.xTCP.usTimeout ) - xDelta );
			}
			else
			{
			int rc ;
				pxSocket->u.xTCP.usTimeout = 0u;
				rc = xTCPSocketCheck( pxSocket );

				/* Within this function, the socket might want to send a delayed
				ack or send out data or whatever it needs to do. */
				if( rc < 0 )
				{
					/* Continue because the socket was deleted. */
					continue;
				}
			}

			/* In xEventBits the driver may indicate that the socket has
			important events for the user.  These are only done just before the
			IP-task goes to sleep. */
			if( pxSocket->xEventBits != 0u )
			{
				if( xWillSleep != pdFALSE )
				{
					/* The IP-task is about to go to sleep, so messages can be
					sent to the socket owners. */
					vSocketWakeUpUser( pxSocket );
				}
				else
				{
					/* Or else make sure this will be called again to wake-up
					the sockets' owner. */
					xShortest = ( TickType_t ) 0;
				}
			}

			if( ( pxSocket->u.xTCP.usTimeout != 0u ) && ( xShortest > ( TickType_t ) pxSocket->u.xTCP.usTimeout ) )
			{
				xShortest = ( TickType_t ) pxSocket->u.xTCP.usTimeout;
			}
		}

		return xShortest;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/*
	 * TCP: as multiple sockets may be bound to the same local port number
	 * looking up a socket is a little more complex:
	 * Both a local port, and a remote port and IP address are being used
	 * For a socket in listening mode, the remote port and IP address are both 0
	 */
	FreeRTOS_Socket_t *pxTCPSocketLookup( uint32_t ulLocalIP, UBaseType_t uxLocalPort, uint32_t ulRemoteIP, UBaseType_t uxRemotePort )
	{
	ListItem_t *pxIterator;
	FreeRTOS_Socket_t *pxResult = NULL, *pxListenSocket = NULL;
	MiniListItem_t *pxEnd = ( MiniListItem_t* )listGET_END_MARKER( &xBoundTCPSocketsList );

		/* Parameter not yet supported. */
		( void ) ulLocalIP;

		for( pxIterator  = ( ListItem_t * ) listGET_NEXT( pxEnd );
			 pxIterator != ( ListItem_t * ) pxEnd;
			 pxIterator  = ( ListItem_t * ) listGET_NEXT( pxIterator ) )
		{
			FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) listGET_LIST_ITEM_OWNER( pxIterator );

			if( pxSocket->usLocalPort == ( uint16_t ) uxLocalPort )
			{
				if( pxSocket->u.xTCP.ucTCPState == eTCP_LISTEN )
				{
					/* If this is a socket listening to uxLocalPort, remember it
					in case there is no perfect match. */
					pxListenSocket = pxSocket;
				}
				else if( ( pxSocket->u.xTCP.usRemotePort == ( uint16_t ) uxRemotePort ) && ( pxSocket->u.xTCP.ulRemoteIP == ulRemoteIP ) )
				{
					/* For sockets not in listening mode, find a match with
					xLocalPort, ulRemoteIP AND xRemotePort. */
					pxResult = pxSocket;
					break;
				}
			}
		}
		if( pxResult == NULL )
		{
			/* An exact match was not found, maybe a listening socket was
			found. */
			pxResult = pxListenSocket;
		}

		return pxResult;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	const struct xSTREAM_BUFFER *FreeRTOS_get_rx_buf( Socket_t xSocket )
	{
	FreeRTOS_Socket_t *pxSocket = (FreeRTOS_Socket_t *)xSocket;

		return pxSocket->u.xTCP.rxStream;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	static StreamBuffer_t *prvTCPCreateStream ( FreeRTOS_Socket_t *pxSocket, BaseType_t xIsInputStream )
	{
	StreamBuffer_t *pxBuffer;
	size_t uxLength;
	size_t uxSize;

		/* Now that a stream is created, the maximum size is fixed before
		creation, it could still be changed with setsockopt(). */
		if( xIsInputStream != pdFALSE )
		{
			uxLength = pxSocket->u.xTCP.uxRxStreamSize;

			if( pxSocket->u.xTCP.uxLittleSpace == 0ul )
			{
				pxSocket->u.xTCP.uxLittleSpace  = ( 1ul * pxSocket->u.xTCP.uxRxStreamSize ) / 5u; /*_RB_ Why divide by 5?  Can this be changed to a #define? */
			}

			if( pxSocket->u.xTCP.uxEnoughSpace == 0ul )
			{
				pxSocket->u.xTCP.uxEnoughSpace = ( 4ul * pxSocket->u.xTCP.uxRxStreamSize ) / 5u; /*_RB_ Why multiply by 4?  Maybe sock80_PERCENT?*/
			}
		}
		else
		{
			uxLength = pxSocket->u.xTCP.uxTxStreamSize;
		}

		/* Add an extra 4 (or 8) bytes. */
		uxLength += sizeof( size_t );

		/* And make the length a multiple of sizeof( size_t ). */
		uxLength &= ~( sizeof( size_t ) - 1u );

		uxSize = sizeof( *pxBuffer ) - sizeof( pxBuffer->ucArray ) + uxLength;

		pxBuffer = ( StreamBuffer_t * )pvPortMallocLarge( uxSize );

		if( pxBuffer == NULL )
		{
			FreeRTOS_debug_printf( ( "prvTCPCreateStream: malloc failed\n" ) );
			pxSocket->u.xTCP.bits.bMallocError = pdTRUE_UNSIGNED;
			vTCPStateChange( pxSocket, eCLOSE_WAIT );
		}
		else
		{
			/* Clear the markers of the stream */
			memset( pxBuffer, '\0', sizeof( *pxBuffer ) - sizeof( pxBuffer->ucArray ) );
			pxBuffer->LENGTH = ( size_t ) uxLength ;

			if( xTCPWindowLoggingLevel != 0 )
			{
				FreeRTOS_debug_printf( ( "prvTCPCreateStream: %cxStream created %lu bytes (total %lu)\n", xIsInputStream ? 'R' : 'T', uxLength, uxSize ) );
			}

			if( xIsInputStream != 0 )
			{
				pxSocket->u.xTCP.rxStream = pxBuffer;
			}
			else
			{
				pxSocket->u.xTCP.txStream = pxBuffer;
			}
		}

		return pxBuffer;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/*
	 * Add data to the RxStream.  When uxOffset > 0, data has come in out-of-order
	 * and will be put in front of the head so it can not be popped by the user.
	 */
	int32_t lTCPAddRxdata( FreeRTOS_Socket_t *pxSocket, size_t uxOffset, const uint8_t *pcData, uint32_t ulByteCount )
	{
	StreamBuffer_t *pxStream = pxSocket->u.xTCP.rxStream;
	int32_t xResult;
	#if( ipconfigUSE_CALLBACKS == 1 )
		BaseType_t bHasHandler = ipconfigIS_VALID_PROG_ADDRESS( pxSocket->u.xTCP.pxHandleReceive );
		const uint8_t *pucBuffer = NULL;
	#endif /* ipconfigUSE_CALLBACKS */

		/* int32_t uxStreamBufferAdd( pxBuffer, uxOffset, pucData, aCount )
		if( pucData != NULL ) copy data the the buffer
		if( pucData == NULL ) no copying, just advance rxHead
		if( uxOffset != 0 ) Just store data which has come out-of-order
		if( uxOffset == 0 ) Also advance rxHead */
		if( pxStream == NULL )
		{
			pxStream = prvTCPCreateStream( pxSocket, pdTRUE );
			if( pxStream == NULL )
			{
				return -1;
			}
		}

		#if( ipconfigUSE_CALLBACKS == 1 )
		{
			if( ( bHasHandler != pdFALSE ) && ( uxStreamBufferGetSize( pxStream ) == 0u ) && ( uxOffset == 0ul ) && ( pcData != NULL ) )
			{
				/* Data can be passed directly to the user */
				pucBuffer = pcData;

				/* Zero-copy for call-back: no need to add the bytes to the
				stream, only the pointer will be advanced by uxStreamBufferAdd(). */
				pcData = NULL;
			}
		}
		#endif /* ipconfigUSE_CALLBACKS */

		xResult = ( int32_t ) uxStreamBufferAdd( pxStream, uxOffset, pcData, ( size_t ) ulByteCount );

		#if( ipconfigHAS_DEBUG_PRINTF != 0 )
		{
			if( xResult != ( int32_t ) ulByteCount )
			{
				FreeRTOS_debug_printf( ( "lTCPAddRxdata: at %ld: %ld/%lu bytes (tail %lu head %lu space %lu front %lu)\n",
					uxOffset, xResult, ulByteCount,
					pxStream->uxTail,
					pxStream->uxHead,
					uxStreamBufferFrontSpace( pxStream ),
					pxStream->uxFront ) );
			}
		}
		#endif /* ipconfigHAS_DEBUG_PRINTF */

		if( uxOffset == 0u )
		{
			/* Data is being added to rxStream at the head (offs = 0) */
			#if( ipconfigUSE_CALLBACKS == 1 )
				if( bHasHandler != pdFALSE )
				{
					/* The socket owner has installed an OnReceive handler. Pass the
					Rx data, without copying from the rxStream, to the user. */
					for (;;)
					{
						uint8_t *ucReadPtr = NULL;
						uint32_t ulCount;
						if( pucBuffer != NULL )
						{
							ucReadPtr = ( uint8_t * )pucBuffer;
							ulCount = ulByteCount;
							pucBuffer = NULL;
						}
						else
						{
							ulCount = ( uint32_t ) uxStreamBufferGetPtr( pxStream, &( ucReadPtr ) );
						}

						if( ulCount == 0ul )
						{
							break;
						}

						pxSocket->u.xTCP.pxHandleReceive( (Socket_t *)pxSocket, ( void* )ucReadPtr, ( size_t ) ulCount );
						uxStreamBufferGet( pxStream, 0ul, NULL, ( size_t ) ulCount, pdFALSE );
					}
				} else
			#endif /* ipconfigUSE_CALLBACKS */
			{
				/* See if running out of space. */
				if( pxSocket->u.xTCP.bits.bLowWater == pdFALSE_UNSIGNED )
				{
					size_t uxFrontSpace = uxStreamBufferFrontSpace( pxSocket->u.xTCP.rxStream );
					if( uxFrontSpace <= pxSocket->u.xTCP.uxLittleSpace  )
					{
						pxSocket->u.xTCP.bits.bLowWater = pdTRUE_UNSIGNED;
						pxSocket->u.xTCP.bits.bWinChange = pdTRUE_UNSIGNED;

						/* bLowWater was reached, send the changed window size. */
						pxSocket->u.xTCP.usTimeout = 1u;
						xSendEventToIPTask( eTCPTimerEvent );
					}
				}

				/* New incoming data is available, wake up the user.   User's
				semaphores will be set just before the IP-task goes asleep. */
				pxSocket->xEventBits |= eSOCKET_RECEIVE;

				#if ipconfigSUPPORT_SELECT_FUNCTION == 1
				{
					if( ( pxSocket->xSelectBits & eSELECT_READ ) != 0 )
					{
						pxSocket->xEventBits |= ( eSELECT_READ << SOCKET_EVENT_BIT_COUNT );
					}
				}
				#endif
			}
		}

		return xResult;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/* Function to get the remote address and IP port */
	BaseType_t FreeRTOS_GetRemoteAddress( Socket_t xSocket, struct freertos_sockaddr *pxAddress )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xResult;

		if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			xResult = -pdFREERTOS_ERRNO_EINVAL;
		}
		else
		{
			/* BSD style sockets communicate IP and port addresses in network
			byte order.

			IP address of remote machine. */
			pxAddress->sin_addr = FreeRTOS_htonl ( pxSocket->u.xTCP.ulRemoteIP );

			/* Port on remote machine. */
			pxAddress->sin_port = FreeRTOS_htons ( pxSocket->u.xTCP.usRemotePort );

			xResult = ( BaseType_t ) sizeof( ( *pxAddress ) );
		}

		return xResult;
	}

#endif /* ipconfigUSE_TCP */

/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/* Returns the number of bytes that may be added to txStream */
	BaseType_t FreeRTOS_maywrite( Socket_t xSocket )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xResult;

		if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			xResult = -pdFREERTOS_ERRNO_EINVAL;
		}
		else if( pxSocket->u.xTCP.ucTCPState != eESTABLISHED )
		{
			if( ( pxSocket->u.xTCP.ucTCPState < eCONNECT_SYN ) || ( pxSocket->u.xTCP.ucTCPState > eESTABLISHED ) )
			{
				xResult = -1;
			}
			else
			{
				xResult = 0;
			}
		}
		else if( pxSocket->u.xTCP.txStream == NULL )
		{
			xResult = ( BaseType_t ) pxSocket->u.xTCP.uxTxStreamSize;
		}
		else
		{
			xResult = ( BaseType_t ) uxStreamBufferGetSpace( pxSocket->u.xTCP.txStream );
		}

		return xResult;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP ==1 )

	BaseType_t FreeRTOS_tx_space( Socket_t xSocket )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xReturn;

		if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			xReturn = -pdFREERTOS_ERRNO_EINVAL;
		}
		else
		{
			if( pxSocket->u.xTCP.txStream != NULL )
			{
				xReturn = ( BaseType_t ) uxStreamBufferGetSpace ( pxSocket->u.xTCP.txStream );
			}
			else
			{
				xReturn = ( BaseType_t ) pxSocket->u.xTCP.uxTxStreamSize;
			}
		}

		return xReturn;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	BaseType_t FreeRTOS_tx_size( Socket_t xSocket )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xReturn;

		if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			xReturn = -pdFREERTOS_ERRNO_EINVAL;
		}
		else
		{
			if( pxSocket->u.xTCP.txStream != NULL )
			{
				xReturn = ( BaseType_t ) uxStreamBufferGetSize ( pxSocket->u.xTCP.txStream );
			}
			else
			{
				xReturn = 0;
			}
		}

		return xReturn;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/* Returns pdTRUE if TCP socket is connected. */
	BaseType_t FreeRTOS_issocketconnected( Socket_t xSocket )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xReturn = pdFALSE;

		if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			xReturn = -pdFREERTOS_ERRNO_EINVAL;
		}
		else
		{
			if( pxSocket->u.xTCP.ucTCPState >= eESTABLISHED )
			{
				if( pxSocket->u.xTCP.ucTCPState < eCLOSE_WAIT )
				{
					xReturn = pdTRUE;
				}
			}
		}

		return xReturn;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/* returns the actual size of MSS being used */
	BaseType_t FreeRTOS_mss( Socket_t xSocket )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xReturn;

		if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			xReturn = -pdFREERTOS_ERRNO_EINVAL;
		}
		else
		{
			/* usCurMSS is declared as uint16_t to save space.  FreeRTOS_mss()
			will often be used in signed native-size expressions cast it to
			BaseType_t. */
			xReturn = ( BaseType_t ) ( pxSocket->u.xTCP.usCurMSS );
		}

		return xReturn;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/* HT: for internal use only: return the connection status */
	BaseType_t FreeRTOS_connstatus( Socket_t xSocket )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xReturn;

		if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			xReturn = -pdFREERTOS_ERRNO_EINVAL;
		}
		else
		{
			/* Cast it to BaseType_t */
			xReturn = ( BaseType_t ) ( pxSocket->u.xTCP.ucTCPState );
		}

		return xReturn;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	/*
	 * Returns the number of bytes which can be read.
	 */
	BaseType_t FreeRTOS_rx_size( Socket_t xSocket )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xReturn;

		if( pxSocket->ucProtocol != ( uint8_t ) FREERTOS_IPPROTO_TCP )
		{
			xReturn = -pdFREERTOS_ERRNO_EINVAL;
		}
		else if( pxSocket->u.xTCP.rxStream != NULL )
		{
			xReturn = ( BaseType_t ) uxStreamBufferGetSize( pxSocket->u.xTCP.rxStream );
		}
		else
		{
			xReturn = 0;
		}

		return xReturn;
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ipconfigUSE_TCP == 1 )

	void FreeRTOS_netstat( void )
	{
	IPStackEvent_t xAskEvent;

		/* Ask the IP-task to call vTCPNetStat()
		 * to avoid accessing xBoundTCPSocketsList
		 */
		xAskEvent.eEventType = eTCPNetStat;
		xAskEvent.pvData = ( void * ) NULL;
		xSendEventStructToIPTask( &xAskEvent, 1000u );
	}

#endif /* ipconfigUSE_TCP */
/*-----------------------------------------------------------*/

#if( ( ipconfigHAS_PRINTF != 0 ) && ( ipconfigUSE_TCP == 1 ) )

	void vTCPNetStat( void )
	{
	/* Show a simple listing of all created sockets and their connections */
	ListItem_t *pxIterator;
	BaseType_t count = 0;

		if( listLIST_IS_INITIALISED( &xBoundTCPSocketsList ) == pdFALSE )
		{
			FreeRTOS_printf( ( "PLUS-TCP not initialized\n" ) );
		}
		else
		{
			FreeRTOS_printf( ( "Prot Port IP-Remote	   : Port  R/T Status	   Alive  tmout Child\n" ) );
			for( pxIterator  = ( ListItem_t * ) listGET_HEAD_ENTRY( &xBoundTCPSocketsList );
				 pxIterator != ( ListItem_t * ) listGET_END_MARKER( &xBoundTCPSocketsList );
				 pxIterator  = ( ListItem_t * ) listGET_NEXT( pxIterator ) )
			{
				FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) listGET_LIST_ITEM_OWNER( pxIterator );
				#if( ipconfigTCP_KEEP_ALIVE == 1 )
					TickType_t age = xTaskGetTickCount() - pxSocket->u.xTCP.xLastAliveTime;
				#else
					TickType_t age = 0u;
				#endif
				#if( ipconfigUSE_CALLBACKS == 1 )
					void *pxHandleReceive = (void*)pxSocket->u.xTCP.pxHandleReceive;
				#else
					void *pxHandleReceive = (void*)NULL;
				#endif
				char ucChildText[16] = "";
				if (pxSocket->u.xTCP.ucTCPState == eTCP_LISTEN)
				{
					const int32_t copied_len = snprintf( ucChildText, sizeof( ucChildText ), " %d/%d",
						( int ) pxSocket->u.xTCP.usChildCount,
						( int ) pxSocket->u.xTCP.usBacklog);
					/* These should never evaluate to false since the buffers are both shorter than 5-6 characters (<=65535) */
					configASSERT( copied_len >= 0 );
					configASSERT( copied_len < sizeof( ucChildText ) );
				}
				FreeRTOS_printf( ( "TCP %5d %-16lxip:%5d %d/%d %-13.13s %6lu %6u%s\n",
					pxSocket->usLocalPort,		/* Local port on this machine */
					pxSocket->u.xTCP.ulRemoteIP,	/* IP address of remote machine */
					pxSocket->u.xTCP.usRemotePort,	/* Port on remote machine */
					pxSocket->u.xTCP.rxStream != NULL,
					pxSocket->u.xTCP.txStream != NULL,
					FreeRTOS_GetTCPStateName( pxSocket->u.xTCP.ucTCPState ),
					(age > 999999 ? 999999 : age), /* Format 'age' for printing */
					pxSocket->u.xTCP.usTimeout,
					ucChildText ) );
					/* Remove compiler warnings if FreeRTOS_debug_printf() is not defined. */
					( void ) pxHandleReceive;
				count++;
			}

			for( pxIterator  = ( ListItem_t * ) listGET_HEAD_ENTRY( &xBoundUDPSocketsList );
				 pxIterator != ( ListItem_t * ) listGET_END_MARKER( &xBoundUDPSocketsList );
				 pxIterator  = ( ListItem_t * ) listGET_NEXT( pxIterator ) )
			{
				/* Local port on this machine */
				FreeRTOS_printf( ( "UDP Port %5u\n",
					FreeRTOS_ntohs( listGET_LIST_ITEM_VALUE( pxIterator ) ) ) );
				count++;
			}

			FreeRTOS_printf( ( "FreeRTOS_netstat: %lu sockets %lu < %lu < %d buffers free\n",
				count,
				uxGetMinimumFreeNetworkBuffers( ),
				uxGetNumberOfFreeNetworkBuffers( ),
				ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ) );
		}
	}

#endif /* ( ( ipconfigHAS_PRINTF != 0 ) && ( ipconfigUSE_TCP == 1 ) ) */
/*-----------------------------------------------------------*/

#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )

	void vSocketSelect( SocketSelect_t *pxSocketSet )
	{
	BaseType_t xRound;
	EventBits_t xSocketBits, xBitsToClear;
	#if ipconfigUSE_TCP == 1
		BaseType_t xLastRound = 1;
	#else
		BaseType_t xLastRound = 0;
	#endif

		/* These flags will be switched on after checking the socket status. */
		EventBits_t xGroupBits = 0;
		pxSocketSet->pxSocket = NULL;

		for( xRound = 0; xRound <= xLastRound; xRound++ )
		{
			const ListItem_t *pxIterator;
			const MiniListItem_t *pxEnd;
			if( xRound == 0 )
			{
				pxEnd = ( const MiniListItem_t* )listGET_END_MARKER( &xBoundUDPSocketsList );
			}
		#if ipconfigUSE_TCP == 1
			else
			{
				pxEnd = ( const MiniListItem_t* )listGET_END_MARKER( &xBoundTCPSocketsList );
			}
		#endif /* ipconfigUSE_TCP == 1 */
			for( pxIterator = ( const ListItem_t * ) ( listGET_NEXT( pxEnd ) );
				 pxIterator != ( const ListItem_t * ) pxEnd;
				 pxIterator = ( const ListItem_t * ) listGET_NEXT( pxIterator ) )
			{
				FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) listGET_LIST_ITEM_OWNER( pxIterator );
				if( pxSocket->pxSocketSet != pxSocketSet )
				{
					/* Socket does not belong to this select group. */
					continue;
				}
				xSocketBits = 0;

				#if( ipconfigUSE_TCP == 1 )
					if( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP )
					{
						/* Check if the socket has already been accepted by the
						owner.  If not, it is useless to return it from a
						select(). */
						BaseType_t bAccepted = pdFALSE;

						if( pxSocket->u.xTCP.bits.bPassQueued == pdFALSE_UNSIGNED )
						{
							if( pxSocket->u.xTCP.bits.bPassAccept == pdFALSE_UNSIGNED )
							{
								bAccepted = pdTRUE;
							}
						}

						/* Is the set owner interested in READ events? */
						if( ( pxSocket->xSelectBits & eSELECT_READ ) != 0 )
						{
							if( pxSocket->u.xTCP.ucTCPState == eTCP_LISTEN )
							{
								if( ( pxSocket->u.xTCP.pxPeerSocket != NULL ) && ( pxSocket->u.xTCP.pxPeerSocket->u.xTCP.bits.bPassAccept != 0 ) )
								{
									xSocketBits |= eSELECT_READ;
								}
							}
							else if( ( pxSocket->u.xTCP.bits.bReuseSocket != pdFALSE_UNSIGNED ) && ( pxSocket->u.xTCP.bits.bPassAccept != pdFALSE_UNSIGNED ) )
							{
								/* This socket has the re-use flag. After connecting it turns into
								aconnected socket. Set the READ event, so that accept() will be called. */
								xSocketBits |= eSELECT_READ;
							}
							else if( ( bAccepted != 0 ) && ( FreeRTOS_recvcount( pxSocket ) > 0 ) )
							{
								xSocketBits |= eSELECT_READ;
							}
						}
						/* Is the set owner interested in EXCEPTION events? */
						if( ( pxSocket->xSelectBits & eSELECT_EXCEPT ) != 0 )
						{
							if( ( pxSocket->u.xTCP.ucTCPState == eCLOSE_WAIT ) || ( pxSocket->u.xTCP.ucTCPState == eCLOSED ) )
							{
								xSocketBits |= eSELECT_EXCEPT;
							}
						}

						/* Is the set owner interested in WRITE events? */
						if( ( pxSocket->xSelectBits & eSELECT_WRITE ) != 0 )
						{
							BaseType_t bMatch = pdFALSE;

							if( bAccepted != 0 )
							{
								if( FreeRTOS_tx_space( pxSocket ) > 0 )
								{
									bMatch = pdTRUE;
								}
							}

							if( bMatch == pdFALSE )
							{
								if( ( pxSocket->u.xTCP.bits.bConnPrepared != pdFALSE_UNSIGNED ) &&
									( pxSocket->u.xTCP.ucTCPState >= eESTABLISHED ) &&
									( pxSocket->u.xTCP.bits.bConnPassed == pdFALSE_UNSIGNED ) )
								{
									pxSocket->u.xTCP.bits.bConnPassed = pdTRUE_UNSIGNED;
									bMatch = pdTRUE;
								}
							}

							if( bMatch != pdFALSE )
							{
								xSocketBits |= eSELECT_WRITE;
							}
						}
					}
					else
				#endif /* ipconfigUSE_TCP == 1 */
				{
					/* Select events for UDP are simpler. */
					if( ( ( pxSocket->xSelectBits & eSELECT_READ ) != 0 ) &&
						( listCURRENT_LIST_LENGTH( &( pxSocket->u.xUDP.xWaitingPacketsList ) ) > 0U ) )
					{
						xSocketBits |= eSELECT_READ;
					}
					/* The WRITE and EXCEPT bits are not used for UDP */
				}	/* if( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP ) */

				/* Each socket keeps its own event flags, which are looked-up
				by FreeRTOS_FD_ISSSET() */
				pxSocket->xSocketBits = xSocketBits;

				/* The ORed value will be used to set the bits in the event
				group. */
				xGroupBits |= xSocketBits;

			}	/* for( pxIterator ... ) */
		}	/* for( xRound = 0; xRound <= xLastRound; xRound++ ) */

		xBitsToClear = xEventGroupGetBits( pxSocketSet->xSelectGroup );

		/* Now set the necessary bits. */
		xBitsToClear = ( xBitsToClear & ~xGroupBits ) & eSELECT_ALL;

		#if( ipconfigSUPPORT_SIGNALS != 0 )
		{
			/* Maybe the socketset was signalled, but don't
			clear the 'eSELECT_INTR' bit here, as it will be used
			and cleared in FreeRTOS_select(). */
			xBitsToClear &= ( EventBits_t ) ~eSELECT_INTR;
		}
		#endif /* ipconfigSUPPORT_SIGNALS */

		if( xBitsToClear != 0 )
		{
			xEventGroupClearBits( pxSocketSet->xSelectGroup, xBitsToClear );
		}

		/* Now include eSELECT_CALL_IP to wakeup the caller. */
		xEventGroupSetBits( pxSocketSet->xSelectGroup, xGroupBits | eSELECT_CALL_IP );
	}

#endif /* ipconfigSUPPORT_SELECT_FUNCTION == 1 */
/*-----------------------------------------------------------*/

#if( ipconfigSUPPORT_SIGNALS != 0 )

	/* Send a signal to the task which reads from this socket. */
	BaseType_t FreeRTOS_SignalSocket( Socket_t xSocket )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xReturn;

		if( pxSocket == NULL )
		{
			xReturn = -pdFREERTOS_ERRNO_EINVAL;
		}
		else
	#if( ipconfigSUPPORT_SELECT_FUNCTION == 1 )
		if( ( pxSocket->pxSocketSet != NULL ) && ( pxSocket->pxSocketSet->xSelectGroup != NULL ) )
		{
			xEventGroupSetBits( pxSocket->pxSocketSet->xSelectGroup, eSELECT_INTR );
			xReturn = 0;
		}
		else
	#endif /* ipconfigSUPPORT_SELECT_FUNCTION */
		if( pxSocket->xEventGroup != NULL )
		{
			xEventGroupSetBits( pxSocket->xEventGroup, eSOCKET_INTR );
			xReturn = 0;
		}
		else
		{
			xReturn = -pdFREERTOS_ERRNO_EINVAL;
		}

		return xReturn;
	}

#endif /* ipconfigSUPPORT_SIGNALS */
/*-----------------------------------------------------------*/

#if( ipconfigSUPPORT_SIGNALS != 0 )

	/* Send a signal to the task which reads from this socket (FromISR version). */
	BaseType_t FreeRTOS_SignalSocketFromISR( Socket_t xSocket, BaseType_t *pxHigherPriorityTaskWoken )
	{
	FreeRTOS_Socket_t *pxSocket = ( FreeRTOS_Socket_t * ) xSocket;
	BaseType_t xReturn;
	IPStackEvent_t xEvent;
	extern QueueHandle_t xNetworkEventQueue;

		configASSERT( pxSocket != NULL );
		configASSERT( pxSocket->ucProtocol == FREERTOS_IPPROTO_TCP );
		configASSERT( pxSocket->xEventGroup );

		xEvent.eEventType = eSocketSignalEvent;
		xEvent.pvData = ( void * )pxSocket;

		/* The IP-task will call FreeRTOS_SignalSocket for this socket. */
		xReturn = xQueueSendToBackFromISR( xNetworkEventQueue, &xEvent, pxHigherPriorityTaskWoken );

		return xReturn;
	}

#endif /* ipconfigSUPPORT_SIGNALS */
/*-----------------------------------------------------------*/
