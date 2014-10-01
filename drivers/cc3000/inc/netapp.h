/*****************************************************************************
*
*  netapp.h  - CC3000 Host Driver Implementation.
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
#ifndef __CC3000_NETAPP_H__
#define	__CC3000_NETAPP_H__

#include "data_types.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef	__cplusplus
extern "C" {
#endif

//*****************************************************************************
//
//! \addtogroup netapp_api
//! @{
//
//*****************************************************************************

typedef struct _netapp_dhcp_ret_args_t
{
    UINT8 aucIP[4];
	UINT8 aucSubnetMask[4];
	UINT8 aucDefaultGateway[4];
	UINT8 aucDHCPServer[4];
	UINT8 aucDNSServer[4];
}tNetappDhcpParams;

typedef struct _netapp_ipconfig_ret_args_t
{
    UINT8 aucIP[4];
	UINT8 aucSubnetMask[4];
	UINT8 aucDefaultGateway[4];
	UINT8 aucDHCPServer[4];
	UINT8 aucDNSServer[4];
	UINT8 uaMacAddr[6];
	UINT8 uaSSID[32];
}tNetappIpconfigRetArgs;


/*Ping send report parameters*/
typedef struct _netapp_pingreport_args
{
	UINT32 packets_sent;
	UINT32 packets_received;
	UINT32 min_round_time;
	UINT32 max_round_time;
	UINT32 avg_round_time;
} netapp_pingreport_args_t;


//*****************************************************************************
//
//!  netapp_config_mac_adrress
//!
//!  @param  mac   device mac address, 6 bytes. Saved: yes 
//!
//!  @return       return on success 0, otherwise error.
//!
//!  @brief        Configure device MAC address and store it in NVMEM. 
//!                The value of the MAC address configured through the API will
//!		             be stored in CC3000 non volatile memory, thus preserved 
//!                over resets.
//
//*****************************************************************************
extern INT32  netapp_config_mac_adrress( UINT8 *mac );

//*****************************************************************************
//
//!  netapp_dhcp
//!
//!  @param  aucIP               device mac address, 6 bytes. Saved: yes 
//!  @param  aucSubnetMask       device mac address, 6 bytes. Saved: yes 
//!  @param  aucDefaultGateway   device mac address, 6 bytes. Saved: yes 
//!  @param  aucDNSServer        device mac address, 6 bytes. Saved: yes 
//!
//!  @return       return on success 0, otherwise error.
//!
//!  @brief       netapp_dhcp is used to configure the network interface, 
//!               static or dynamic (DHCP).\n In order to activate DHCP mode, 
//!               aucIP, aucSubnetMask, aucDefaultGateway must be 0.
//!               The default mode of CC3000 is DHCP mode.
//!               Note that the configuration is saved in non volatile memory
//!               and thus preserved over resets.
//!	 
//! @note         If the mode is altered a reset of CC3000 device is required 
//!               in order to apply changes.\nAlso note that asynchronous event 
//!               of DHCP_EVENT, which is generated when an IP address is 
//!               allocated either by the DHCP server or due to static 
//!               allocation is generated only upon a connection to the 
//!               AP was established. 
//!
//*****************************************************************************
extern 	INT32 netapp_dhcp(UINT32 *aucIP, UINT32 *aucSubnetMask,UINT32 *aucDefaultGateway, UINT32 *aucDNSServer);



//*****************************************************************************
//
//!  netapp_timeout_values
//!
//!  @param  aucDHCP    DHCP lease time request, also impact 
//!                     the DHCP renew timeout. Range: [0-0xffffffff] seconds,
//!                     0 or 0xffffffff == infinity lease timeout.
//!                     Resolution:10 seconds. Influence: only after 
//!                     reconnecting to the AP. 
//!                     Minimal bound value: MIN_TIMER_VAL_SECONDS - 10 seconds.
//!                     The parameter is saved into the CC3000 NVMEM. 
//!                     The default value on CC3000 is 14400 seconds.
//!	 
//!  @param  aucARP     ARP refresh timeout, if ARP entry is not updated by
//!                     incoming packet, the ARP entry will be  deleted by
//!                     the end of the timeout. 
//!                     Range: [0-0xffffffff] seconds, 0 == infinity ARP timeout
//!                     Resolution: 10 seconds. Influence: on runtime.
//!                     Minimal bound value: MIN_TIMER_VAL_SECONDS - 10 seconds
//!                     The parameter is saved into the CC3000 NVMEM. 
//!	                    The default value on CC3000 is 3600 seconds.
//!
//!  @param  aucKeepalive   Keepalive event sent by the end of keepalive timeout
//!                         Range: [0-0xffffffff] seconds, 0 == infinity timeout
//!                         Resolution: 10 seconds.
//!                         Influence: on runtime.
//!                         Minimal bound value: MIN_TIMER_VAL_SECONDS - 10 sec
//!                         The parameter is saved into the CC3000 NVMEM. 
//!                         The default value on CC3000 is 10 seconds.
//!
//!  @param  aucInactivity   Socket inactivity timeout, socket timeout is
//!                          refreshed by incoming or outgoing packet, by the
//!                          end of the socket timeout the socket will be closed
//!                          Range: [0-0xffffffff] sec, 0 == infinity timeout.
//!                          Resolution: 10 seconds. Influence: on runtime.
//!                          Minimal bound value: MIN_TIMER_VAL_SECONDS - 10 sec
//!                          The parameter is saved into the CC3000 NVMEM. 
//!	                         The default value on CC3000 is 60 seconds.
//!
//!  @return       return on success 0, otherwise error.
//!
//!  @brief       Set new timeout values. Function set new timeout values for: 
//!               DHCP lease timeout, ARP  refresh timeout, keepalive event 
//!               timeout and socket inactivity timeout 
//!	 
//! @note         If a parameter set to non zero value which is less than 10s,
//!               it will be set automatically to 10s.
//!
//*****************************************************************************
#ifndef CC3000_TINY_DRIVER
extern INT32 netapp_timeout_values(UINT32 *aucDHCP, UINT32 *aucARP, UINT32 *aucKeepalive, UINT32 *aucInactivity);
#endif

//*****************************************************************************
//
//!  netapp_ping_send
//!
//!  @param  ip              destination IP address
//!  @param  pingAttempts    number of echo requests to send
//!  @param  pingSize        send buffer size which may be up to 1400 bytes 
//!  @param  pingTimeout     Time to wait for a response,in milliseconds.
//!
//!  @return       return on success 0, otherwise error.
//!
//!  @brief       send ICMP ECHO_REQUEST to network hosts 
//!	 
//! @note         If an operation finished successfully asynchronous ping report 
//!               event will be generated. The report structure is as defined
//!               by structure netapp_pingreport_args_t.
//!
//! @warning      Calling this function while a previous Ping Requests are in 
//!               progress will stop the previous ping request.
//*****************************************************************************

 #ifndef CC3000_TINY_DRIVER
extern INT32 netapp_ping_send(UINT32 *ip, UINT32 ulPingAttempts, UINT32 ulPingSize, UINT32 ulPingTimeout);
#endif

//*****************************************************************************
//
//!  netapp_ping_stop
//!
//!  @param  none
//!
//!  @return  On success, zero is returned. On error, -1 is returned.      
//!
//!  @brief   Stop any ping request.
//!	 
//!
//*****************************************************************************

#ifndef CC3000_TINY_DRIVER
extern INT32 netapp_ping_stop();
#endif
//*****************************************************************************
//
//!  netapp_ping_report
//!
//!  @param  none
//!
//!  @return  none
//!
//!  @brief   Request for ping status. This API triggers the CC3000 to send 
//!           asynchronous events: HCI_EVNT_WLAN_ASYNC_PING_REPORT.
//!           This event will carry  the report structure:
//!           netapp_pingreport_args_t. This structure is filled in with ping
//!           results up till point of triggering API.
//!           netapp_pingreport_args_t:\n packets_sent - echo sent,
//!           packets_received - echo reply, min_round_time - minimum
//!           round time, max_round_time - max round time,
//!           avg_round_time - average round time
//!	 
//! @note     When a ping operation is not active, the returned structure 
//!           fields are 0.
//!
//*****************************************************************************
#ifndef CC3000_TINY_DRIVER
extern void netapp_ping_report();
#endif


//*****************************************************************************
//
//!  netapp_ipconfig
//!
//!  @param[out]  ipconfig  This argument is a pointer to a 
//!                         tNetappIpconfigRetArgs structure. This structure is
//!                         filled in with the network interface configuration.
//!                         tNetappIpconfigRetArgs:\n aucIP - ip address,
//!                         aucSubnetMask - mask, aucDefaultGateway - default
//!                         gateway address, aucDHCPServer - dhcp server address
//!                         aucDNSServer - dns server address, uaMacAddr - mac
//!                         address, uaSSID - connected AP ssid
//!
//!  @return  none
//!
//!  @brief   Obtain the CC3000 Network interface information.
//!           Note that the information is available only after the WLAN
//!       	  connection was established. Calling this function before
//!           associated, will cause non-defined values to be returned.
//!	 
//! @note     The function is useful for figuring out the IP Configuration of
//!       		the device when DHCP is used and for figuring out the SSID of
//!       		the Wireless network the device is associated with.
//!
//*****************************************************************************

extern void netapp_ipconfig( tNetappIpconfigRetArgs * ipconfig );


//*****************************************************************************
//
//!  netapp_arp_flush
//!
//!  @param  none
//!
//!  @return  none
//!
//!  @brief  Flushes ARP table
//!
//*****************************************************************************

#ifndef CC3000_TINY_DRIVER
extern INT32 netapp_arp_flush();
#endif


//*****************************************************************************
//
//!  netapp_set_debug_level
//!
//!  @param[in]      level    debug level. Bitwise [0-8],
//!                         0(disable)or 1(enable).\n Bitwise map: 0 - Critical
//!                         message, 1 information message, 2 - core messages, 3 -
//!                         HCI messages, 4 - Network stack messages, 5 - wlan
//!                         messages, 6 - wlan driver messages, 7 - epprom messages,
//!                         8 - general messages. Default: 0x13f. Saved: no
//!
//!  @return  On success, zero is returned. On error, -1 is returned
//!
//!  @brief   Debug messages sent via the UART debug channel, this function
//!              enable/disable the debug level
//!
//*****************************************************************************


#ifndef CC3000_TINY_DRIVER
INT32 netapp_set_debug_level(UINT32 ulLevel);
#endif
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef	__cplusplus
}
#endif // __cplusplus

#endif	// __CC3000_NETAPP_H__

