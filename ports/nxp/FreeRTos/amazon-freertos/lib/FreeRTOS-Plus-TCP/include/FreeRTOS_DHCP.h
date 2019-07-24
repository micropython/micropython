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

#ifndef FREERTOS_DHCP_H
#define FREERTOS_DHCP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Application level configuration options. */
#include "FreeRTOSIPConfig.h"
#include "IPTraceMacroDefaults.h"

/* Used in the DHCP callback if ipconfigUSE_DHCP_HOOK is set to 1. */
typedef enum eDHCP_PHASE
{
	eDHCPPhasePreDiscover,	/* Driver is about to send a DHCP discovery. */
	eDHCPPhasePreRequest,	/* Driver is about to request DHCP an IP address. */
#if( ipconfigDHCP_SEND_DISCOVER_AFTER_AUTO_IP != 0 )
	eDHCPPhasePreLLA,		/* Driver is about to try get an LLA address */
#endif /* ipconfigDHCP_SEND_DISCOVER_AFTER_AUTO_IP */
} eDHCPCallbackPhase_t;

/* Used in the DHCP callback if ipconfigUSE_DHCP_HOOK is set to 1. */
typedef enum eDHCP_ANSWERS
{
	eDHCPContinue,			/* Continue the DHCP process */
	eDHCPUseDefaults,		/* Stop DHCP and use the static defaults. */
	eDHCPStopNoChanges,		/* Stop DHCP and continue with current settings. */
} eDHCPCallbackAnswer_t;

/*
 * NOT A PUBLIC API FUNCTION.
 */
void vDHCPProcess( BaseType_t xReset );

/* Internal call: returns true if socket is the current DHCP socket */
BaseType_t xIsDHCPSocket( Socket_t xSocket );

/* Prototype of the hook (or callback) function that must be provided by the
application if ipconfigUSE_DHCP_HOOK is set to 1.  See the following URL for
usage information:
http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_IP_Configuration.html#ipconfigUSE_DHCP_HOOK
*/
eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress );

#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif /* FREERTOS_DHCP_H */













