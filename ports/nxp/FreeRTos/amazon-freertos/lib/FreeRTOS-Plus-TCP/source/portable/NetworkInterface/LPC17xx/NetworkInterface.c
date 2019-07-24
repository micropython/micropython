/*
FreeRTOS+TCP V2.0.10
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Hardware abstraction. */
#include "FreeRTOS_IO.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "NetworkBufferManagement.h"

/* Driver includes. */
#include "lpc17xx_emac.h"
#include "lpc17xx_pinsel.h"

/* Demo includes. */
#include "NetworkInterface.h"

#if ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES != 1
	#define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eProcessBuffer
#else
	#define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eConsiderFrameForProcessing( ( pucEthernetBuffer ) )
#endif

/* When a packet is ready to be sent, if it cannot be sent immediately then the
task performing the transmit will block for niTX_BUFFER_FREE_WAIT
milliseconds.  It will do this a maximum of niMAX_TX_ATTEMPTS before giving
up. */
#define niTX_BUFFER_FREE_WAIT	( pdMS_TO_TICKS( 2UL ) )
#define niMAX_TX_ATTEMPTS		( 5 )

/* The length of the queue used to send interrupt status words from the
interrupt handler to the deferred handler task. */
#define niINTERRUPT_QUEUE_LENGTH	( 10 )

/*-----------------------------------------------------------*/

/*
 * A deferred interrupt handler task that processes
 */
static void prvEMACHandlerTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* The queue used to communicate Ethernet events with the IP task. */
extern QueueHandle_t xNetworkEventQueue;

/* The semaphore used to wake the deferred interrupt handler task when an Rx
interrupt is received. */
static SemaphoreHandle_t xEMACRxEventSemaphore = NULL;
/*-----------------------------------------------------------*/

BaseType_t xNetworkInterfaceInitialise( void )
{
EMAC_CFG_Type Emac_Config;
PINSEL_CFG_Type xPinConfig;
BaseType_t xStatus, xReturn;
extern uint8_t ucMACAddress[ 6 ];

	/* Enable Ethernet Pins */
	boardCONFIGURE_ENET_PINS( xPinConfig );

	Emac_Config.Mode = EMAC_MODE_AUTO;
	Emac_Config.pbEMAC_Addr = ucMACAddress;
	xStatus = EMAC_Init( &Emac_Config );

	LPC_EMAC->IntEnable &= ~( EMAC_INT_TX_DONE );

	if( xStatus != ERROR )
	{
		vSemaphoreCreateBinary( xEMACRxEventSemaphore );
		configASSERT( xEMACRxEventSemaphore );

		/* The handler task is created at the highest possible priority to
		ensure the interrupt handler can return directly to it. */
		xTaskCreate( prvEMACHandlerTask, "EMAC", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL );

		/* Enable the interrupt and set its priority to the minimum
		interrupt priority.  */
		NVIC_SetPriority( ENET_IRQn, configMAC_INTERRUPT_PRIORITY );
		NVIC_EnableIRQ( ENET_IRQn );

		xReturn = pdPASS;
	}
	else
	{
		xReturn = pdFAIL;
	}

	configASSERT( xStatus != ERROR );

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxNetworkBuffer )
{
BaseType_t xReturn = pdFAIL;
int32_t x;
extern void EMAC_StartTransmitNextBuffer( uint32_t ulLength );
extern void EMAC_SetNextPacketToSend( uint8_t * pucBuffer );


	/* Attempt to obtain access to a Tx buffer. */
	for( x = 0; x < niMAX_TX_ATTEMPTS; x++ )
	{
		if( EMAC_CheckTransmitIndex() == TRUE )
		{
			/* Will the data fit in the Tx buffer? */
			if( pxNetworkBuffer->xDataLength < EMAC_ETH_MAX_FLEN ) /*_RB_ The size needs to come from FreeRTOSIPConfig.h. */
			{
				/* Assign the buffer to the Tx descriptor that is now known to
				be free. */
				EMAC_SetNextPacketToSend( pxNetworkBuffer->pucBuffer );

				/* The EMAC now owns the buffer. */
				pxNetworkBuffer->pucBuffer = NULL;

				/* Initiate the Tx. */
				EMAC_StartTransmitNextBuffer( pxNetworkBuffer->xDataLength );
				iptraceNETWORK_INTERFACE_TRANSMIT();

				/* The Tx has been initiated. */
				xReturn = pdPASS;
			}
			break;
		}
		else
		{
			vTaskDelay( niTX_BUFFER_FREE_WAIT );
		}
	}

	/* Finished with the network buffer. */
	vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );

	return xReturn;
}
/*-----------------------------------------------------------*/

void ENET_IRQHandler( void )
{
uint32_t ulInterruptCause;

	while( ( ulInterruptCause = LPC_EMAC->IntStatus ) != 0 )
	{
		/* Clear the interrupt. */
		LPC_EMAC->IntClear = ulInterruptCause;

		/* Clear fatal error conditions.  NOTE:  The driver does not clear all
		errors, only those actually experienced.  For future reference, range
		errors are not actually errors so can be ignored. */
		if( ( ulInterruptCause & EMAC_INT_TX_UNDERRUN ) != 0U )
		{
			LPC_EMAC->Command |= EMAC_CR_TX_RES;
		}

		/* Unblock the deferred interrupt handler task if the event was an
		Rx. */
		if( ( ulInterruptCause & EMAC_INT_RX_DONE ) != 0UL )
		{
			xSemaphoreGiveFromISR( xEMACRxEventSemaphore, NULL );
		}
	}

	/* ulInterruptCause is used for convenience here.  A context switch is
	wanted, but coding portEND_SWITCHING_ISR( 1 ) would likely result in a
	compiler warning. */
	portEND_SWITCHING_ISR( ulInterruptCause );
}
/*-----------------------------------------------------------*/

static void prvEMACHandlerTask( void *pvParameters )
{
size_t xDataLength;
const uint16_t usCRCLength = 4;
NetworkBufferDescriptor_t *pxNetworkBuffer;
IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };

/* This is not included in the header file for some reason. */
extern uint8_t *EMAC_NextPacketToRead( void );

	( void ) pvParameters;
	configASSERT( xEMACRxEventSemaphore );

	for( ;; )
	{
		/* Wait for the EMAC interrupt to indicate that another packet has been
		received.  The while() loop is only needed if INCLUDE_vTaskSuspend is
		set to 0 in FreeRTOSConfig.h. */
		while( xSemaphoreTake( xEMACRxEventSemaphore, portMAX_DELAY ) == pdFALSE );

		/* At least one packet has been received. */
		while( EMAC_CheckReceiveIndex() != FALSE )
		{
			/* Obtain the length, minus the CRC.  The CRC is four bytes
			but the length is already minus 1. */
			xDataLength = ( size_t ) EMAC_GetReceiveDataSize() - ( usCRCLength - 1U );

			if( xDataLength > 0U )
			{
				/* Obtain a network buffer to pass this data into the
				stack.  No storage is required as the network buffer
				will point directly to the buffer that already holds
				the	received data. */
				pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( 0, ( TickType_t ) 0 );

				if( pxNetworkBuffer != NULL )
				{
					pxNetworkBuffer->pucBuffer = EMAC_NextPacketToRead();
					pxNetworkBuffer->xDataLength = xDataLength;
					xRxEvent.pvData = ( void * ) pxNetworkBuffer;

					/* Data was received and stored.  Send a message to the IP
					task to let it know. */
					if( xSendEventStructToIPTask( &xRxEvent, ( TickType_t ) 0 ) == pdFAIL )
					{
						vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
						iptraceETHERNET_RX_EVENT_LOST();
					}
				}
				else
				{
					iptraceETHERNET_RX_EVENT_LOST();
				}

				iptraceNETWORK_INTERFACE_RECEIVE();
			}

			/* Release the frame. */
			EMAC_UpdateRxConsumeIndex();
		}
	}
}
/*-----------------------------------------------------------*/

