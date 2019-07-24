/*
FreeRTOS Kernel V10.0.1
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

#ifndef NET_IF_H
#define NET_IF_H

/*
 * Send uip_len bytes from uip_buf to the network interface selected by the 
 * configNETWORK_INTERFACE_TO_USE constant (defined in FreeRTOSConfig.h). 
 */
void vNetifTx( void );

/*
 * Receive bytes from the network interface selected by the 
 * configNETWORK_INTERFACE_TO_USE constant (defined in FreeRTOSConfig.h).  The
 * bytes are placed in uip_buf.  The number of bytes copied into uip_buf is
 * returned.
 */
UBaseType_t uxNetifRx( void );

/*
 * Prepare a packet capture session.  This will print out all the network 
 * interfaces available, and the one actually used is set by the 
 * configNETWORK_INTERFACE_TO_USE constant that is defined in 
 * FreeRTOSConfig.h. */
BaseType_t xNetifInit( void );

#endif /* NET_IF_H */
