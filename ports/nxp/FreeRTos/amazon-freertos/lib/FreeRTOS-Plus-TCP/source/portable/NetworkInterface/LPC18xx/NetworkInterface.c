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
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

/* LPCOpen includes. */
#include "chip.h"
#include "lpc_phy.h"

/* The size of the stack allocated to the task that handles Rx packets. */
#define nwRX_TASK_STACK_SIZE	140

#ifndef	PHY_LS_HIGH_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still high after 15 seconds of not
	receiving packets. */
	#define PHY_LS_HIGH_CHECK_TIME_MS	15000
#endif

#ifndef	PHY_LS_LOW_CHECK_TIME_MS
	/* Check if the LinkSStatus in the PHY is still low every second. */
	#define PHY_LS_LOW_CHECK_TIME_MS	1000
#endif

#ifndef configUSE_RMII
	#define configUSE_RMII 1
#endif

#ifndef configNUM_RX_DESCRIPTORS
	#error please define configNUM_RX_DESCRIPTORS in your FreeRTOSIPConfig.h
#endif

#ifndef configNUM_TX_DESCRIPTORS
	#error please define configNUM_TX_DESCRIPTORS in your FreeRTOSIPConfig.h
#endif

#ifndef NETWORK_IRQHandler
	#error NETWORK_IRQHandler must be defined to the name of the function that is installed in the interrupt vector table to handle Ethernet interrupts.
#endif

#if !defined( MAC_FF_HMC )
	/* Hash for multicast. */
	#define MAC_FF_HMC     ( 1UL << 2UL )
#endif

#ifndef iptraceEMAC_TASK_STARTING
	#define iptraceEMAC_TASK_STARTING()	do { } while( 0 )
#endif

/* Define the bits of .STATUS that indicate a reception error. */
#define nwRX_STATUS_ERROR_BITS \
	( RDES_CE  /* CRC Error */                        | \
	  RDES_RE  /* Receive Error */                    | \
	  RDES_DE  /* Descriptor Error */                 | \
	  RDES_RWT /* Receive Watchdog Timeout */         | \
	  RDES_LC  /* Late Collision */                   | \
	  RDES_OE  /* Overflow Error */                   | \
	  RDES_SAF /* Source Address Filter Fail */       | \
	  RDES_AFM /* Destination Address Filter Fail */  | \
	  RDES_LE  /* Length Error */                     )

/* Define the EMAC status bits that should trigger an interrupt. */
#define nwDMA_INTERRUPT_MASK \
	( DMA_IE_TIE  /* Transmit interrupt enable */         | \
	  DMA_IE_TSE  /* Transmit stopped enable */           | \
	  DMA_IE_OVE  /* Overflow interrupt enable */         | \
	  DMA_IE_RIE  /* Receive interrupt enable */          | \
	  DMA_IE_NIE  /* Normal interrupt summary enable */   | \
	  DMA_IE_AIE  /* Abnormal interrupt summary enable */ | \
	  DMA_IE_RUE  /* Receive buffer unavailable enable */ | \
	  DMA_IE_UNE  /* Underflow interrupt enable. */       | \
	  DMA_IE_TJE  /* Transmit jabber timeout enable */    | \
	  DMA_IE_RSE  /* Received stopped enable */           | \
	  DMA_IE_RWE  /* Receive watchdog timeout enable */   | \
	  DMA_IE_FBE )/* Fatal bus error enable */

/* Interrupt events to process.  Currently only the RX/TX events are processed
although code for other events is included to allow for possible future
expansion. */
#define EMAC_IF_RX_EVENT        1UL
#define EMAC_IF_TX_EVENT        2UL
#define EMAC_IF_ERR_EVENT       4UL
#define EMAC_IF_ALL_EVENT       ( EMAC_IF_RX_EVENT | EMAC_IF_TX_EVENT | EMAC_IF_ERR_EVENT )

 /* If ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES is set to 1, then the Ethernet
 driver will filter incoming packets and only pass the stack those packets it
 considers need processing. */
 #if( ipconfigETHERNET_DRIVER_FILTERS_FRAME_TYPES == 0 )
 	#define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eProcessBuffer
 #else
 	#define ipCONSIDER_FRAME_FOR_PROCESSING( pucEthernetBuffer ) eConsiderFrameForProcessing( ( pucEthernetBuffer ) )
 #endif

#if( ipconfigZERO_COPY_RX_DRIVER == 0 ) || ( ipconfigZERO_COPY_TX_DRIVER == 0 )
	#warning It is adviced to enable both macros
#endif

#ifndef configPLACE_IN_SECTION_RAM
	#define configPLACE_IN_SECTION_RAM
/*
	#define configPLACE_IN_SECTION_RAM	__attribute__ ((section(".ramfunc")))
*/
#endif

/*-----------------------------------------------------------*/

/*
 * Delay function passed into the library.  The implementation uses FreeRTOS
 * calls so the scheduler must be started before the driver can be used.
 */
static void prvDelay( uint32_t ulMilliSeconds );

/*
 * Initialises the Tx and Rx descriptors respectively.
 */
static void prvSetupTxDescriptors( void );
static void prvSetupRxDescriptors( void );

/*
 * A task that processes received frames.
 */
static void prvEMACHandlerTask( void *pvParameters );

/*
 * Sets up the MAC with the results of an auto-negotiation.
 */
static BaseType_t prvSetLinkSpeed( void );

/*
 * Generates a CRC for a MAC address that is then used to generate a hash index.
 */
static uint32_t prvGenerateCRC32( const uint8_t *ucAddress );

/*
 * Generates a hash index when setting a filter to permit a MAC address.
 */
static uint32_t prvGetHashIndex( const uint8_t *ucAddress );

/*
 * Update the hash table to allow a MAC address.
 */
static void prvAddMACAddress( const uint8_t* ucMacAddress );

/*
 * Sometimes the DMA will report received data as being longer than the actual
 * received from length.  This function checks the reported length and corrects
 * if if necessary.
 */
static void prvRemoveTrailingBytes( NetworkBufferDescriptor_t *pxDescriptor );

/*-----------------------------------------------------------*/

/* Bit map of outstanding ETH interrupt events for processing.  Currently only
the Rx and Tx interrupt is handled, although code is included for other events
to enable future expansion. */
static volatile uint32_t ulISREvents;

/* A copy of PHY register 1: 'PHY_REG_01_BMSR' */
static uint32_t ulPHYLinkStatus = 0;

/* Tx descriptors and index. */
static ENET_ENHTXDESC_T xDMATxDescriptors[ configNUM_TX_DESCRIPTORS ];

/* ulNextFreeTxDescriptor is declared volatile, because it is accessed from
to different tasks. */
static volatile uint32_t ulNextFreeTxDescriptor;
static uint32_t ulTxDescriptorToClear;

/* Rx descriptors and index. */
static ENET_ENHRXDESC_T xDMARxDescriptors[ configNUM_RX_DESCRIPTORS ];
static uint32_t ulNextRxDescriptorToProcess;

/* Must be defined externally - the demo applications define this in main.c. */
extern uint8_t ucMACAddress[ 6 ];

/* The handle of the task that processes Rx packets.  The handle is required so
the task can be notified when new packets arrive. */
static TaskHandle_t xRxHanderTask = NULL;

#if( ipconfigUSE_LLMNR == 1 )
	static const uint8_t xLLMNR_MACAddress[] = { '\x01', '\x00', '\x5E', '\x00', '\x00', '\xFC' };
#endif	/* ipconfigUSE_LLMNR == 1 */

/* xTXDescriptorSemaphore is a counting semaphore with
a maximum count of ETH_TXBUFNB, which is the number of
DMA TX descriptors. */
static SemaphoreHandle_t xTXDescriptorSemaphore = NULL;

/*-----------------------------------------------------------*/


BaseType_t xNetworkInterfaceInitialise( void )
{
BaseType_t xReturn = pdPASS;

	/* The interrupt will be turned on when a link is established. */
	NVIC_DisableIRQ( ETHERNET_IRQn );

	/* Disable receive and transmit DMA processes. */
	LPC_ETHERNET->DMA_OP_MODE &= ~( DMA_OM_ST | DMA_OM_SR );

	/* Disable packet reception. */
	LPC_ETHERNET->MAC_CONFIG &= ~( MAC_CFG_RE | MAC_CFG_TE );

	/* Call the LPCOpen function to initialise the hardware. */
	Chip_ENET_Init( LPC_ETHERNET );

	/* Save MAC address. */
	Chip_ENET_SetADDR( LPC_ETHERNET, ucMACAddress );

	/* Clear all MAC address hash entries. */
	LPC_ETHERNET->MAC_HASHTABLE_HIGH = 0;
	LPC_ETHERNET->MAC_HASHTABLE_LOW = 0;

	#if( ipconfigUSE_LLMNR == 1 )
	{
		prvAddMACAddress( xLLMNR_MACAddress );
	}
	#endif /* ipconfigUSE_LLMNR == 1 */

	/* Promiscuous flag (PR) and Receive All flag (RA) set to zero.  The
	registers MAC_HASHTABLE_[LOW|HIGH] will be loaded to allow certain
	multi-cast addresses. */
	LPC_ETHERNET->MAC_FRAME_FILTER = MAC_FF_HMC;

	#if( configUSE_RMII == 1 )
	{
		if( lpc_phy_init( pdTRUE, prvDelay ) != SUCCESS )
		{
			xReturn = pdFAIL;
		}
	}
	#else
	{
		#warning This path has not been tested.
		if( lpc_phy_init( pdFALSE, prvDelay ) != SUCCESS )
		{
			xReturn = pdFAIL;
		}
	}
	#endif

	if( xReturn == pdPASS )
	{
		/* Guard against the task being created more than once and the
		descriptors being initialised more than once. */
		if( xRxHanderTask == NULL )
		{
			xReturn = xTaskCreate( prvEMACHandlerTask, "EMAC", nwRX_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &xRxHanderTask );
			configASSERT( xReturn );
		}

		if( xTXDescriptorSemaphore == NULL )
		{
			/* Create a counting semaphore, with a value of 'configNUM_TX_DESCRIPTORS'
			and a maximum of 'configNUM_TX_DESCRIPTORS'. */
			xTXDescriptorSemaphore = xSemaphoreCreateCounting( ( UBaseType_t ) configNUM_TX_DESCRIPTORS, ( UBaseType_t ) configNUM_TX_DESCRIPTORS );
			configASSERT( xTXDescriptorSemaphore );
		}

		/* Enable MAC interrupts. */
		LPC_ETHERNET->DMA_INT_EN = nwDMA_INTERRUPT_MASK;
	}

	if( xReturn != pdFAIL )
	{
		/* Auto-negotiate was already started.  Wait for it to complete. */
		xReturn = prvSetLinkSpeed();

		if( xReturn == pdPASS )
		{
       		/* Initialise the descriptors. */
			prvSetupTxDescriptors();
			prvSetupRxDescriptors();

			/* Clear all interrupts. */
			LPC_ETHERNET->DMA_STAT = DMA_ST_ALL;

			/* Enable receive and transmit DMA processes. */
			LPC_ETHERNET->DMA_OP_MODE |= DMA_OM_ST | DMA_OM_SR;

			/* Set Receiver / Transmitter Enable. */
			LPC_ETHERNET->MAC_CONFIG |= MAC_CFG_RE | MAC_CFG_TE;

			/* Start receive polling. */
			LPC_ETHERNET->DMA_REC_POLL_DEMAND = 1;

			/* Enable interrupts in the NVIC. */
			NVIC_SetPriority( ETHERNET_IRQn, configMAC_INTERRUPT_PRIORITY );
			NVIC_EnableIRQ( ETHERNET_IRQn );
		}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

#define niBUFFER_1_PACKET_SIZE		1536

static __attribute__ ((section("._ramAHB32"))) uint8_t ucNetworkPackets[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS * niBUFFER_1_PACKET_SIZE ] __attribute__ ( ( aligned( 32 ) ) );

void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{

uint8_t *ucRAMBuffer = ucNetworkPackets;
uint32_t ul;

	for( ul = 0; ul < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; ul++ )
	{
		pxNetworkBuffers[ ul ].pucEthernetBuffer = ucRAMBuffer + ipBUFFER_PADDING;
		*( ( unsigned * ) ucRAMBuffer ) = ( unsigned ) ( &( pxNetworkBuffers[ ul ] ) );
		ucRAMBuffer += niBUFFER_1_PACKET_SIZE;
	}
}
/*-----------------------------------------------------------*/

configPLACE_IN_SECTION_RAM
static void vClearTXBuffers()
{
uint32_t ulLastDescriptor = ulNextFreeTxDescriptor;
size_t uxCount = ( ( size_t ) configNUM_TX_DESCRIPTORS ) - uxSemaphoreGetCount( xTXDescriptorSemaphore );
#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
	NetworkBufferDescriptor_t *pxNetworkBuffer;
	uint8_t *ucPayLoad;
#endif

	/* This function is called after a TX-completion interrupt.
	It will release each Network Buffer used in xNetworkInterfaceOutput().
	'uxCount' represents the number of descriptors given to DMA for transmission.
	After sending a packet, the DMA will clear the 'TDES_OWN' bit. */
	while( ( uxCount > ( size_t ) 0u ) && ( ( xDMATxDescriptors[ ulTxDescriptorToClear ].CTRLSTAT & TDES_OWN ) == 0 ) )
	{
		if( ( ulTxDescriptorToClear == ulLastDescriptor ) && ( uxCount != ( size_t ) configNUM_TX_DESCRIPTORS ) )
		{
			break;
		}


		#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
		{
			ucPayLoad = ( uint8_t * )xDMATxDescriptors[ ulTxDescriptorToClear ].B1ADD;
			if( ucPayLoad != NULL )
			{
				/* B1ADD points to a pucEthernetBuffer of a Network Buffer descriptor. */
				pxNetworkBuffer = pxPacketBuffer_to_NetworkBuffer( ucPayLoad );

				configASSERT( pxNetworkBuffer != NULL );

				vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer ) ;
				xDMATxDescriptors[ ulTxDescriptorToClear ].B1ADD = ( uint32_t )0u;
			}
		}
		#endif /* ipconfigZERO_COPY_TX_DRIVER */

		/* Move onto the next descriptor, wrapping if necessary. */
		ulTxDescriptorToClear++;
		if( ulTxDescriptorToClear >= configNUM_TX_DESCRIPTORS )
		{
			ulTxDescriptorToClear = 0;
		}

		uxCount--;
		/* Tell the counting semaphore that one more TX descriptor is available. */
		xSemaphoreGive( xTXDescriptorSemaphore );
	}
}

/*-----------------------------------------------------------*/

configPLACE_IN_SECTION_RAM
BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxDescriptor, BaseType_t bReleaseAfterSend )
{
BaseType_t xReturn = pdFAIL;
const TickType_t xBlockTimeTicks = pdMS_TO_TICKS( 50 );

	/* Attempt to obtain access to a Tx descriptor. */
	do
	{
		if( xTXDescriptorSemaphore == NULL )
		{
			break;
		}
		if( xSemaphoreTake( xTXDescriptorSemaphore, xBlockTimeTicks ) != pdPASS )
		{
			/* Time-out waiting for a free TX descriptor. */
			break;
		}

		/* If the descriptor is still owned by the DMA it can't be used. */
		if( ( xDMATxDescriptors[ ulNextFreeTxDescriptor ].CTRLSTAT & TDES_OWN ) != 0 )
		{
			/* The semaphore was taken, the TX DMA-descriptor is still not available.
			Actually that should not occur, the 'TDES_OWN' was already confirmed low in vClearTXBuffers(). */
			xSemaphoreGive( xTXDescriptorSemaphore );
		}
		else
		{
			#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
			{
				/* bReleaseAfterSend should always be set when using the zero
				copy driver. */
				configASSERT( bReleaseAfterSend != pdFALSE );

				/* The DMA's descriptor to point directly to the data in the
				network buffer descriptor.  The data is not copied. */
				xDMATxDescriptors[ ulNextFreeTxDescriptor ].B1ADD = ( uint32_t ) pxDescriptor->pucEthernetBuffer;

				/* The DMA descriptor will 'own' this Network Buffer,
				until it has been sent.  So don't release it now. */
				bReleaseAfterSend = false;
			}
			#else
			{
				/* The data is copied from the network buffer descriptor into
				the DMA's descriptor. */
				memcpy( ( void * ) xDMATxDescriptors[ ulNextFreeTxDescriptor ].B1ADD, ( void * ) pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength );
			}
			#endif

			xDMATxDescriptors[ ulNextFreeTxDescriptor ].BSIZE = ( uint32_t ) TDES_ENH_BS1( pxDescriptor->xDataLength );

			/* This descriptor is given back to the DMA. */
			xDMATxDescriptors[ ulNextFreeTxDescriptor ].CTRLSTAT |= TDES_OWN;

			/* Ensure the DMA is polling Tx descriptors. */
			LPC_ETHERNET->DMA_TRANS_POLL_DEMAND = 1;

            iptraceNETWORK_INTERFACE_TRANSMIT();

			/* Move onto the next descriptor, wrapping if necessary. */
			ulNextFreeTxDescriptor++;
			if( ulNextFreeTxDescriptor >= configNUM_TX_DESCRIPTORS )
			{
				ulNextFreeTxDescriptor = 0;
			}

			/* The Tx has been initiated. */
			xReturn = pdPASS;
		}
	} while( 0 );

	/* The buffer has been sent so can be released. */
	if( bReleaseAfterSend != pdFALSE )
	{
		vReleaseNetworkBufferAndDescriptor( pxDescriptor );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static void prvDelay( uint32_t ulMilliSeconds )
{
	/* Ensure the scheduler was started before attempting to use the scheduler to
	create a delay. */
	configASSERT( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING );

	vTaskDelay( pdMS_TO_TICKS( ulMilliSeconds ) );
}
/*-----------------------------------------------------------*/

static void prvSetupTxDescriptors( void )
{
BaseType_t x;

	/* Start with Tx descriptors clear. */
	memset( ( void * ) xDMATxDescriptors, 0, sizeof( xDMATxDescriptors ) );

	/* Index to the next Tx descriptor to use. */
	ulNextFreeTxDescriptor = 0ul;

	/* Index to the next Tx descriptor to clear ( after transmission ). */
	ulTxDescriptorToClear = 0ul;

	for( x = 0; x < configNUM_TX_DESCRIPTORS; x++ )
	{
		#if( ipconfigZERO_COPY_TX_DRIVER != 0 )
		{
			/* Nothing to do, B1ADD will be set when data is ready to transmit.
			Currently the memset above will have set it to NULL. */
		}
		#else
		{
			/* Allocate a buffer to the Tx descriptor.  This is the most basic
			way of creating a driver as the data is then copied into the
			buffer. */
			xDMATxDescriptors[ x ].B1ADD = ( uint32_t ) pvPortMalloc( ipTOTAL_ETHERNET_FRAME_SIZE );

			/* Use an assert to check the allocation as +TCP applications will
			often not use a malloc() failed hook as the TCP stack will recover
			from allocation failures. */
			configASSERT( xDMATxDescriptors[ x ].B1ADD );
		}
		#endif

		/* Buffers hold an entire frame so all buffers are both the start and
		end of a frame. */
		/* TDES_ENH_TCH     Second Address Chained. */
		/* TDES_ENH_CIC(n)  Checksum Insertion Control, tried but it does not work for the LPC18xx... */
		/* TDES_ENH_FS      First Segment. */
		/* TDES_ENH_LS      Last Segment. */
		/* TDES_ENH_IC      Interrupt on Completion. */
		xDMATxDescriptors[ x ].CTRLSTAT = TDES_ENH_TCH | TDES_ENH_CIC( 3 ) | TDES_ENH_FS | TDES_ENH_LS | TDES_ENH_IC;
		xDMATxDescriptors[ x ].B2ADD = ( uint32_t ) &xDMATxDescriptors[ x + 1 ];
	}

	xDMATxDescriptors[ configNUM_TX_DESCRIPTORS - 1 ].CTRLSTAT |= TDES_ENH_TER;
	xDMATxDescriptors[ configNUM_TX_DESCRIPTORS - 1 ].B2ADD = ( uint32_t ) &xDMATxDescriptors[ 0 ];

	/* Point the DMA to the base of the descriptor list. */
	LPC_ETHERNET->DMA_TRANS_DES_ADDR = ( uint32_t ) xDMATxDescriptors;
}
/*-----------------------------------------------------------*/

static void prvSetupRxDescriptors( void )
{
BaseType_t x;
#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
	NetworkBufferDescriptor_t *pxNetworkBuffer;
#endif

	/* Index to the next Rx descriptor to use. */
	ulNextRxDescriptorToProcess = 0;

	/* Clear RX descriptor list. */
	memset( ( void * )  xDMARxDescriptors, 0, sizeof( xDMARxDescriptors ) );

	for( x = 0; x < configNUM_RX_DESCRIPTORS; x++ )
	{
		/* Allocate a buffer of the largest	possible frame size as it is not
		known what size received frames will be. */

		#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
		{
			pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( ipTOTAL_ETHERNET_FRAME_SIZE, 0 );

			/* During start-up there should be enough Network Buffers available,
			so it is safe to use configASSERT().
			In case this assert fails, please check: configNUM_RX_DESCRIPTORS,
			ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS, and in case BufferAllocation_2.c
			is included, check the amount of available heap. */
			configASSERT( pxNetworkBuffer != NULL );

			/* Pass the actual buffer to DMA. */
			xDMARxDescriptors[ x ].B1ADD = ( uint32_t ) pxNetworkBuffer->pucEthernetBuffer;
		}
		#else
		{
			/* All DMA descriptors are populated with permanent memory blocks.
			Their contents will be copy to Network Buffers. */
			xDMARxDescriptors[ x ].B1ADD = ( uint32_t ) pvPortMalloc( ipTOTAL_ETHERNET_FRAME_SIZE );
		}
		#endif /* ipconfigZERO_COPY_RX_DRIVER */

		/* Use an assert to check the allocation as +TCP applications will often
		not use a malloc failed hook as the TCP stack will recover from
		allocation failures. */
		configASSERT( xDMARxDescriptors[ x ].B1ADD );

		xDMARxDescriptors[ x ].B2ADD = ( uint32_t ) &( xDMARxDescriptors[ x + 1 ] );
		xDMARxDescriptors[ x ].CTRL = ( uint32_t ) RDES_ENH_BS1( ipTOTAL_ETHERNET_FRAME_SIZE ) | RDES_ENH_RCH;

		/* The descriptor is available for use by the DMA. */
		xDMARxDescriptors[ x ].STATUS = RDES_OWN;
	}

	/* RDES_ENH_RER  Receive End of Ring. */
	xDMARxDescriptors[ ( configNUM_RX_DESCRIPTORS - 1 ) ].CTRL |= RDES_ENH_RER;
	xDMARxDescriptors[ configNUM_RX_DESCRIPTORS - 1 ].B2ADD = ( uint32_t ) &( xDMARxDescriptors[ 0 ] );

	/* Point the DMA to the base of the descriptor list. */
	LPC_ETHERNET->DMA_REC_DES_ADDR = ( uint32_t ) xDMARxDescriptors;
}
/*-----------------------------------------------------------*/
configPLACE_IN_SECTION_RAM
static void prvRemoveTrailingBytes( NetworkBufferDescriptor_t *pxDescriptor )
{
size_t xExpectedLength;
IPPacket_t *pxIPPacket;

	pxIPPacket = ( IPPacket_t * ) pxDescriptor->pucEthernetBuffer;
	/* Look at the actual length of the packet, translate it to a host-endial notation. */
	xExpectedLength = sizeof( EthernetHeader_t ) + ( size_t ) FreeRTOS_htons( pxIPPacket->xIPHeader.usLength );

	if( xExpectedLength == ( pxDescriptor->xDataLength + 4 ) )
	{
		pxDescriptor->xDataLength -= 4;
	}
	else
	{
		if( pxDescriptor->xDataLength > xExpectedLength )
		{
			pxDescriptor->xDataLength = ( size_t ) xExpectedLength;
		}
	}
}
/*-----------------------------------------------------------*/
configPLACE_IN_SECTION_RAM
BaseType_t xGetPhyLinkStatus( void )
{
BaseType_t xReturn;

	if( ( ulPHYLinkStatus & PHY_LINK_CONNECTED ) == 0 )
	{
		xReturn = pdFALSE;
	}
	else
	{
		xReturn = pdTRUE;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

uint32_t ulDataAvailable;

configPLACE_IN_SECTION_RAM
static BaseType_t prvNetworkInterfaceInput()
{
BaseType_t xResult = pdFALSE;
uint32_t ulStatus;
eFrameProcessingResult_t eResult;
const TickType_t xDescriptorWaitTime = pdMS_TO_TICKS( 250 );
const UBaseType_t uxMinimumBuffersRemaining = 3UL;
uint16_t usLength;
NetworkBufferDescriptor_t *pxDescriptor;
#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
	NetworkBufferDescriptor_t *pxNewDescriptor;
#endif /* ipconfigZERO_COPY_RX_DRIVER */
#if( ipconfigUSE_LINKED_RX_MESSAGES == 0 )
	IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };
#endif

	/* Process each descriptor that is not still in use by the DMA. */
	ulStatus = xDMARxDescriptors[ ulNextRxDescriptorToProcess ].STATUS;
	if( ( ulStatus & RDES_OWN ) == 0 )
	{
		/* Check packet for errors */
		if( ( ulStatus & nwRX_STATUS_ERROR_BITS ) != 0 )
		{
			/* There is some reception error. */
			intCount[ 3 ]++;
			/* Clear error bits. */
			ulStatus &= ~( ( uint32_t )nwRX_STATUS_ERROR_BITS );
		}
		else
		{
			xResult++;

			eResult = ipCONSIDER_FRAME_FOR_PROCESSING( ( const uint8_t * const ) ( xDMARxDescriptors[ ulNextRxDescriptorToProcess ].B1ADD ) );
			if( eResult == eProcessBuffer )
			{
				if( ( ulPHYLinkStatus & PHY_LINK_CONNECTED ) == 0 )
				{
					ulPHYLinkStatus |= PHY_LINK_CONNECTED;
					FreeRTOS_printf( ( "prvEMACHandlerTask: PHY LS now %d (message received)\n", ( ulPHYLinkStatus & PHY_LINK_CONNECTED ) != 0 ) );
				}

			#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
				if( uxGetNumberOfFreeNetworkBuffers() > uxMinimumBuffersRemaining )
				{
					pxNewDescriptor = pxGetNetworkBufferWithDescriptor( ipTOTAL_ETHERNET_FRAME_SIZE, xDescriptorWaitTime );
				}
				else
				{
					/* Too risky to allocate a new Network Buffer. */
					pxNewDescriptor = NULL;
				}
				if( pxNewDescriptor != NULL )
			#else
				if( uxGetNumberOfFreeNetworkBuffers() > uxMinimumBuffersRemaining )
			#endif /* ipconfigZERO_COPY_RX_DRIVER */
				{
			#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
				const uint8_t *pucBuffer;
			#endif

					/* Get the actual length. */
					usLength = RDES_FLMSK( ulStatus );

					#if( ipconfigZERO_COPY_RX_DRIVER != 0 )
					{
						/* Replace the character buffer 'B1ADD'. */
						pucBuffer = ( const uint8_t * const ) ( xDMARxDescriptors[ ulNextRxDescriptorToProcess ].B1ADD );
						xDMARxDescriptors[ ulNextRxDescriptorToProcess ].B1ADD = ( uint32_t ) pxNewDescriptor->pucEthernetBuffer;

						/* 'B1ADD' contained the address of a 'pucEthernetBuffer' that
						belongs to a Network Buffer.  Find the original Network Buffer. */
						pxDescriptor = pxPacketBuffer_to_NetworkBuffer( pucBuffer );
						/* This zero-copy driver makes sure that every 'xDMARxDescriptors' contains
						a reference to a Network Buffer at any time.
						In case it runs out of Network Buffers, a DMA buffer won't be replaced,
						and the received messages is dropped. */
						configASSERT( pxDescriptor != NULL );
					}
					#else
					{
						/* Create a buffer of exactly the required length. */
						pxDescriptor = pxGetNetworkBufferWithDescriptor( usLength, xDescriptorWaitTime );
					}
					#endif /* ipconfigZERO_COPY_RX_DRIVER */

					if( pxDescriptor != NULL )
					{
						pxDescriptor->xDataLength = ( size_t ) usLength;
						#if( ipconfigZERO_COPY_RX_DRIVER == 0 )
						{
							/* Copy the data into the allocated buffer. */
							memcpy( ( void * ) pxDescriptor->pucEthernetBuffer, ( void * ) xDMARxDescriptors[ ulNextRxDescriptorToProcess ].B1ADD, usLength );
						}
						#endif /* ipconfigZERO_COPY_RX_DRIVER */
						/* It is possible that more data was copied than
						actually makes up the frame.  If this is the case
						adjust the length to remove any trailing bytes. */
						prvRemoveTrailingBytes( pxDescriptor );

						/* Pass the data to the TCP/IP task for processing. */
						xRxEvent.pvData = ( void * ) pxDescriptor;
						if( xSendEventStructToIPTask( &xRxEvent, xDescriptorWaitTime ) == pdFALSE )
						{
							/* Could not send the descriptor into the TCP/IP
							stack, it must be released. */
							vReleaseNetworkBufferAndDescriptor( pxDescriptor );
						}
						else
						{
							iptraceNETWORK_INTERFACE_RECEIVE();

							/* The data that was available at the top of this
							loop has been sent, so is no longer available. */
							ulDataAvailable = pdFALSE;
						}
					}
				}
			}
			else
			{
				/* The packet is discarded as uninteresting. */
				ulDataAvailable = pdFALSE;
			}
			/* Got here because received data was sent to the IP task or the
			data contained an error and was discarded.  Give the descriptor
			back to the DMA. */
			xDMARxDescriptors[ ulNextRxDescriptorToProcess ].STATUS = ulStatus | RDES_OWN;

			/* Move onto the next descriptor. */
			ulNextRxDescriptorToProcess++;
			if( ulNextRxDescriptorToProcess >= configNUM_RX_DESCRIPTORS )
			{
				ulNextRxDescriptorToProcess = 0;
			}

			ulStatus = xDMARxDescriptors[ ulNextRxDescriptorToProcess ].STATUS;
		} /* if( ( ulStatus & nwRX_STATUS_ERROR_BITS ) != 0 ) */
	} /* if( ( ulStatus & RDES_OWN ) == 0 ) */

	/* Restart receive polling. */
	LPC_ETHERNET->DMA_REC_POLL_DEMAND = 1;

	return xResult;
}
/*-----------------------------------------------------------*/

configPLACE_IN_SECTION_RAM
void NETWORK_IRQHandler( void )
{
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
uint32_t ulDMAStatus;
const uint32_t ulRxInterruptMask =
	DMA_ST_RI |		/* Receive interrupt */
	DMA_ST_RU;		/* Receive buffer unavailable */
const uint32_t ulTxInterruptMask =
	DMA_ST_TI |		/* Transmit interrupt */
	DMA_ST_TPS;		/* Transmit process stopped */

	configASSERT( xRxHanderTask );

	/* Get pending interrupts. */
	ulDMAStatus = LPC_ETHERNET->DMA_STAT;

	/* RX group interrupt(s). */
	if( ( ulDMAStatus & ulRxInterruptMask ) != 0x00 )
	{
		/* Remember that an RX event has happened. */
		ulISREvents |= EMAC_IF_RX_EVENT;
		vTaskNotifyGiveFromISR( xRxHanderTask, &xHigherPriorityTaskWoken );
		intCount[ 0 ]++;
	}

	/* TX group interrupt(s). */
	if( ( ulDMAStatus & ulTxInterruptMask ) != 0x00 )
	{
		/* Remember that a TX event has happened. */
		ulISREvents |= EMAC_IF_TX_EVENT;
		vTaskNotifyGiveFromISR( xRxHanderTask, &xHigherPriorityTaskWoken );
		intCount[ 1 ]++;
	}

	/* Test for 'Abnormal interrupt summary'. */
	if( ( ulDMAStatus & DMA_ST_AIE ) != 0x00 )
	{
		/* The trace macro must be written such that it can be called from
		an interrupt. */
		iptraceETHERNET_RX_EVENT_LOST();
	}

	/* Clear pending interrupts */
	LPC_ETHERNET->DMA_STAT = ulDMAStatus;

	/* Context switch needed? */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

static BaseType_t prvSetLinkSpeed( void )
{
BaseType_t xReturn = pdFAIL;
TickType_t xTimeOnEntering;
uint32_t ulPhyStatus;
const TickType_t xAutoNegotiateDelay = pdMS_TO_TICKS( 5000UL );

	/* Ensure polling does not starve lower priority tasks by temporarily
	setting the priority of this task to that of the idle task. */
	vTaskPrioritySet( NULL, tskIDLE_PRIORITY );

	xTimeOnEntering = xTaskGetTickCount();
	do
	{
		ulPhyStatus = lpcPHYStsPoll();
		if( ( ulPhyStatus & PHY_LINK_CONNECTED ) != 0x00 )
		{
			/* Set interface speed and duplex. */
			if( ( ulPhyStatus & PHY_LINK_SPEED100 ) != 0x00 )
			{
				Chip_ENET_SetSpeed( LPC_ETHERNET, 1 );
			}
			else
			{
				Chip_ENET_SetSpeed( LPC_ETHERNET, 0 );
			}

			if( ( ulPhyStatus & PHY_LINK_FULLDUPLX ) != 0x00 )
			{
				Chip_ENET_SetDuplex( LPC_ETHERNET, true );
			}
			else
			{
				Chip_ENET_SetDuplex( LPC_ETHERNET, false );
			}

			xReturn = pdPASS;
			break;
		}
	} while( ( xTaskGetTickCount() - xTimeOnEntering ) < xAutoNegotiateDelay );

	/* Reset the priority of this task back to its original value. */
	vTaskPrioritySet( NULL, ipconfigIP_TASK_PRIORITY );

	return xReturn;
}
/*-----------------------------------------------------------*/

static uint32_t prvGenerateCRC32( const uint8_t *ucAddress )
{
unsigned int j;
const uint32_t Polynomial = 0xEDB88320;
uint32_t crc = ~0ul;
const uint8_t *pucCurrent = ( const uint8_t * ) ucAddress;
const uint8_t *pucLast = pucCurrent + 6;

    /* Calculate  normal CRC32 */
    while( pucCurrent < pucLast )
    {
        crc ^= *( pucCurrent++ );
        for( j = 0; j < 8; j++ )
        {
            if( ( crc & 1 ) != 0 )
            {
                crc = (crc >> 1) ^ Polynomial;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return ~crc;
}
/*-----------------------------------------------------------*/

static uint32_t prvGetHashIndex( const uint8_t *ucAddress )
{
uint32_t ulCrc = prvGenerateCRC32( ucAddress );
uint32_t ulIndex = 0ul;
BaseType_t xCount = 6;

    /* Take the lowest 6 bits of the CRC32 and reverse them */
    while( xCount-- )
    {
        ulIndex <<= 1;
        ulIndex |= ( ulCrc & 1 );
        ulCrc >>= 1;
    }

    /* This is the has value of 'ucAddress' */
    return ulIndex;
}
/*-----------------------------------------------------------*/

static void prvAddMACAddress( const uint8_t* ucMacAddress )
{
BaseType_t xIndex;

    xIndex = prvGetHashIndex( ucMacAddress );
    if( xIndex >= 32 )
    {
        LPC_ETHERNET->MAC_HASHTABLE_HIGH |= ( 1u << ( xIndex - 32 ) );
    }
    else
    {
        LPC_ETHERNET->MAC_HASHTABLE_LOW |= ( 1u << xIndex );
    }
}
/*-----------------------------------------------------------*/

configPLACE_IN_SECTION_RAM
static void prvEMACHandlerTask( void *pvParameters )
{
TimeOut_t xPhyTime;
TickType_t xPhyRemTime;
UBaseType_t uxLastMinBufferCount = 0;
UBaseType_t uxCurrentCount;
BaseType_t xResult = 0;
uint32_t ulStatus;
const TickType_t xBlockTime = pdMS_TO_TICKS( 5000ul );

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	/* A possibility to set some additional task properties. */
	iptraceEMAC_TASK_STARTING();

	vTaskSetTimeOutState( &xPhyTime );
	xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );

	for( ;; )
	{
		uxCurrentCount = uxGetMinimumFreeNetworkBuffers();
		if( uxLastMinBufferCount != uxCurrentCount )
		{
			/* The logging produced below may be helpful
			while tuning +TCP: see how many buffers are in use. */
			uxLastMinBufferCount = uxCurrentCount;
			FreeRTOS_printf( ( "Network buffers: %lu lowest %lu\n",
				uxGetNumberOfFreeNetworkBuffers(), uxCurrentCount ) );
		}

		#if( ipconfigCHECK_IP_QUEUE_SPACE != 0 )
		{
		static UBaseType_t uxLastMinQueueSpace = 0;

			uxCurrentCount = uxGetMinimumIPQueueSpace();
			if( uxLastMinQueueSpace != uxCurrentCount )
			{
				/* The logging produced below may be helpful
				while tuning +TCP: see how many buffers are in use. */
				uxLastMinQueueSpace = uxCurrentCount;
				FreeRTOS_printf( ( "Queue space: lowest %lu\n", uxCurrentCount ) );
			}
		}
		#endif /* ipconfigCHECK_IP_QUEUE_SPACE */

		ulTaskNotifyTake( pdTRUE, xBlockTime );

		xResult = ( BaseType_t ) 0;

		if( ( ulISREvents & EMAC_IF_TX_EVENT ) != 0 )
		{
			/* Code to release TX buffers if zero-copy is used. */
			ulISREvents &= ~EMAC_IF_TX_EVENT;
			{
				/* Check if DMA packets have been delivered. */
				vClearTXBuffers();
			}
		}

		if( ( ulISREvents & EMAC_IF_RX_EVENT ) != 0 )
		{
			ulISREvents &= ~EMAC_IF_RX_EVENT;

			xResult = prvNetworkInterfaceInput();
			if( xResult > 0 )
			{
			  	while( prvNetworkInterfaceInput() > 0 )
				{
				}
			}
		}

		if( xResult > 0 )
		{
			/* A packet was received. No need to check for the PHY status now,
			but set a timer to check it later on. */
			vTaskSetTimeOutState( &xPhyTime );
			xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
			xResult = 0;
		}
		else if( xTaskCheckForTimeOut( &xPhyTime, &xPhyRemTime ) != pdFALSE )
		{
			ulStatus = lpcPHYStsPoll();

			if( ( ulPHYLinkStatus & PHY_LINK_CONNECTED ) != ( ulStatus & PHY_LINK_CONNECTED ) )
			{
				ulPHYLinkStatus = ulStatus;
				FreeRTOS_printf( ( "prvEMACHandlerTask: PHY LS now %d (polled PHY)\n", ( ulPHYLinkStatus & PHY_LINK_CONNECTED ) != 0 ) );
			}

			vTaskSetTimeOutState( &xPhyTime );
			if( ( ulPHYLinkStatus & PHY_LINK_CONNECTED ) != 0 )
			{
				xPhyRemTime = pdMS_TO_TICKS( PHY_LS_HIGH_CHECK_TIME_MS );
			}
			else
			{
				xPhyRemTime = pdMS_TO_TICKS( PHY_LS_LOW_CHECK_TIME_MS );
			}
		}
	}
}
/*-----------------------------------------------------------*/
