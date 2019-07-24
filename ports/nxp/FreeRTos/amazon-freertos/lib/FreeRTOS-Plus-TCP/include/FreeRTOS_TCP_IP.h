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

#ifndef FREERTOS_TCP_IP_H
#define FREERTOS_TCP_IP_H

#ifdef __cplusplus
extern "C" {
#endif

BaseType_t xProcessReceivedTCPPacket( NetworkBufferDescriptor_t *pxNetworkBuffer );

typedef enum eTCP_STATE {
	/* Comments about the TCP states are borrowed from the very useful
	 * Wiki page:
	 * http://en.wikipedia.org/wiki/Transmission_Control_Protocol */
	eCLOSED = 0u,	/* 0 (server + client) no connection state at all. */
	eTCP_LISTEN,	/* 1 (server) waiting for a connection request
						 from any remote TCP and port. */
	eCONNECT_SYN,	/* 2 (client) internal state: socket wants to send
						 a connect */
	eSYN_FIRST,		/* 3 (server) Just created, must ACK the SYN request. */
	eSYN_RECEIVED,	/* 4 (server) waiting for a confirming connection request
						 acknowledgement after having both received and sent a connection request. */
	eESTABLISHED,	/* 5 (server + client) an open connection, data received can be
						 delivered to the user. The normal state for the data transfer phase of the connection. */
	eFIN_WAIT_1,	/* 6 (server + client) waiting for a connection termination request from the remote TCP,
						 or an acknowledgement of the connection termination request previously sent. */
	eFIN_WAIT_2,	/* 7 (server + client) waiting for a connection termination request from the remote TCP. */
	eCLOSE_WAIT,	/* 8 (server + client) waiting for a connection termination request from the local user. */
	eCLOSING,		/*   (server + client) waiting for a connection termination request acknowledgement from the remote TCP. */
	eLAST_ACK,		/* 9 (server + client) waiting for an acknowledgement of the connection termination request
						 previously sent to the remote TCP
						 (which includes an acknowledgement of its connection termination request). */
	eTIME_WAIT,		/* 10 (either server or client) waiting for enough time to pass to be sure the remote TCP received the
						 acknowledgement of its connection termination request. [According to RFC 793 a connection can
						 stay in TIME-WAIT for a maximum of four minutes known as a MSL (maximum segment lifetime).] */
} eIPTCPState_t;


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* FREERTOS_TCP_IP_H */













