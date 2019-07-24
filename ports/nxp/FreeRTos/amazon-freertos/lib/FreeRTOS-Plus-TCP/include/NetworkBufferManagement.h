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

#ifndef NETWORK_BUFFER_MANAGEMENT_H
#define NETWORK_BUFFER_MANAGEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE PUBLIC API FUNCTIONS. */
BaseType_t xNetworkBuffersInitialise( void );
NetworkBufferDescriptor_t *pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks );
NetworkBufferDescriptor_t *pxNetworkBufferGetFromISR( size_t xRequestedSizeBytes );
void vReleaseNetworkBufferAndDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer );
BaseType_t vNetworkBufferReleaseFromISR( NetworkBufferDescriptor_t * const pxNetworkBuffer );
uint8_t *pucGetNetworkBuffer( size_t *pxRequestedSizeBytes );
void vReleaseNetworkBuffer( uint8_t *pucEthernetBuffer );

/* Get the current number of free network buffers. */
UBaseType_t uxGetNumberOfFreeNetworkBuffers( void );

/* Get the lowest number of free network buffers. */
UBaseType_t uxGetMinimumFreeNetworkBuffers( void );

/* Copy a network buffer into a bigger buffer. */
NetworkBufferDescriptor_t *pxDuplicateNetworkBufferWithDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer,
	BaseType_t xNewLength);

/* Increase the size of a Network Buffer.
In case BufferAllocation_2.c is used, the new space must be allocated. */
NetworkBufferDescriptor_t *pxResizeNetworkBufferWithDescriptor( NetworkBufferDescriptor_t * pxNetworkBuffer,
	size_t xNewSizeBytes );

#if ipconfigTCP_IP_SANITY
	/*
	 * Check if an address is a valid pointer to a network descriptor
	 * by looking it up in the array of network descriptors
	 */
	UBaseType_t bIsValidNetworkDescriptor (const NetworkBufferDescriptor_t * pxDesc);
	BaseType_t prvIsFreeBuffer( const NetworkBufferDescriptor_t *pxDescr );
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* NETWORK_BUFFER_MANAGEMENT_H */
