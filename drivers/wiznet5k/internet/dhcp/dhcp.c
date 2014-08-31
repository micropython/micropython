//*****************************************************************************
//
//! \file dhcp.c
//! \brief DHCP APIs implement file.
//! \details Processig DHCP protocol as DISCOVER, OFFER, REQUEST, ACK, NACK and DECLINE.
//! \version 1.1.0
//! \date 2013/11/18
//! \par  Revision history
//!       <2013/11/18> 1st Release
//!       <2012/12/20> V1.1.0
//!         1. Optimize code
//!         2. Add reg_dhcp_cbfunc()
//!         3. Add DHCP_stop() 
//!         4. Integrate check_DHCP_state() & DHCP_run() to DHCP_run()
//!         5. Don't care system endian
//!         6. Add comments
//!       <2012/12/26> V1.1.1
//!         1. Modify variable declaration: dhcp_tick_1s is declared volatile for code optimization
//! \author Eric Jung & MidnightCow
//! \copyright
//!
//! Copyright (c)  2013, WIZnet Co., LTD.
//! All rights reserved.
//! 
//! Redistribution and use in source and binary forms, with or without 
//! modification, are permitted provided that the following conditions 
//! are met: 
//! 
//!     * Redistributions of source code must retain the above copyright 
//! notice, this list of conditions and the following disclaimer. 
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution. 
//!     * Neither the name of the <ORGANIZATION> nor the names of its 
//! contributors may be used to endorse or promote products derived 
//! from this software without specific prior written permission. 
//! 
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//#include "Ethernet/socket.h"
//#include "Internet/DHCP/dhcp.h"
#include "../../Ethernet/socket.h"
#include "dhcp.h"

/* If you want to display debug & processing message, Define _DHCP_DEBUG_ in dhcp.h */

#ifdef _DHCP_DEBUG_
   #include <stdio.h>
#endif   

/* DHCP state machine. */
#define STATE_DHCP_INIT          0        ///< Initialize
#define STATE_DHCP_DISCOVER      1        ///< send DISCOVER and wait OFFER
#define STATE_DHCP_REQUEST       2        ///< send REQEUST and wait ACK or NACK
#define STATE_DHCP_LEASED        3        ///< ReceiveD ACK and IP leased
#define STATE_DHCP_REREQUEST     4        ///< send REQUEST for maintaining leased IP
#define STATE_DHCP_RELEASE       5        ///< No use
#define STATE_DHCP_STOP          6        ///< Stop processing DHCP

#define DHCP_FLAGSBROADCAST      0x8000   ///< The broadcast value of flags in @ref RIP_MSG 
#define DHCP_FLAGSUNICAST        0x0000   ///< The unicast   value of flags in @ref RIP_MSG

/* DHCP message OP code */
#define DHCP_BOOTREQUEST         1        ///< Request Message used in op of @ref RIP_MSG
#define DHCP_BOOTREPLY           2        ///< Reply Message used i op of @ref RIP_MSG

/* DHCP message type */
#define DHCP_DISCOVER            1        ///< DISCOVER message in OPT of @ref RIP_MSG
#define DHCP_OFFER               2        ///< OFFER message in OPT of @ref RIP_MSG
#define DHCP_REQUEST             3        ///< REQUEST message in OPT of @ref RIP_MSG
#define DHCP_DECLINE             4        ///< DECLINE message in OPT of @ref RIP_MSG
#define DHCP_ACK                 5        ///< ACK message in OPT of @ref RIP_MSG
#define DHCP_NAK                 6        ///< NACK message in OPT of @ref RIP_MSG
#define DHCP_RELEASE             7        ///< RELEASE message in OPT of @ref RIP_MSG. No use
#define DHCP_INFORM              8        ///< INFORM message in OPT of @ref RIP_MSG. No use

#define DHCP_HTYPE10MB           1        ///< Used in type of @ref RIP_MSG
#define DHCP_HTYPE100MB          2        ///< Used in type of @ref RIP_MSG

#define DHCP_HLENETHERNET        6        ///< Used in hlen of @ref RIP_MSG
#define DHCP_HOPS                0        ///< Used in hops of @ref RIP_MSG
#define DHCP_SECS                0        ///< Used in secs of @ref RIP_MSG

#define INFINITE_LEASETIME       0xffffffff	///< Infinite lease time

#define OPT_SIZE                 312               /// Max OPT size of @ref RIP_MSG
#define RIP_MSG_SIZE             (236+OPT_SIZE)    /// Max size of @ref RIP_MSG

/* 
 * @brief DHCP option and value (cf. RFC1533)
 */
enum
{
   padOption               = 0,
   subnetMask              = 1,
   timerOffset             = 2,
   routersOnSubnet         = 3,
   timeServer              = 4,
   nameServer              = 5,
   dns                     = 6,
   logServer               = 7,
   cookieServer            = 8,
   lprServer               = 9,
   impressServer           = 10,
   resourceLocationServer	= 11,
   hostName                = 12,
   bootFileSize            = 13,
   meritDumpFile           = 14,
   domainName              = 15,
   swapServer              = 16,
   rootPath                = 17,
   extentionsPath          = 18,
   IPforwarding            = 19,
   nonLocalSourceRouting   = 20,
   policyFilter            = 21,
   maxDgramReasmSize       = 22,
   defaultIPTTL            = 23,
   pathMTUagingTimeout     = 24,
   pathMTUplateauTable     = 25,
   ifMTU                   = 26,
   allSubnetsLocal         = 27,
   broadcastAddr           = 28,
   performMaskDiscovery    = 29,
   maskSupplier            = 30,
   performRouterDiscovery  = 31,
   routerSolicitationAddr  = 32,
   staticRoute             = 33,
   trailerEncapsulation    = 34,
   arpCacheTimeout         = 35,
   ethernetEncapsulation   = 36,
   tcpDefaultTTL           = 37,
   tcpKeepaliveInterval    = 38,
   tcpKeepaliveGarbage     = 39,
   nisDomainName           = 40,
   nisServers              = 41,
   ntpServers              = 42,
   vendorSpecificInfo      = 43,
   netBIOSnameServer       = 44,
   netBIOSdgramDistServer	= 45,
   netBIOSnodeType         = 46,
   netBIOSscope            = 47,
   xFontServer             = 48,
   xDisplayManager         = 49,
   dhcpRequestedIPaddr     = 50,
   dhcpIPaddrLeaseTime     = 51,
   dhcpOptionOverload      = 52,
   dhcpMessageType         = 53,
   dhcpServerIdentifier    = 54,
   dhcpParamRequest        = 55,
   dhcpMsg                 = 56,
   dhcpMaxMsgSize          = 57,
   dhcpT1value             = 58,
   dhcpT2value             = 59,
   dhcpClassIdentifier     = 60,
   dhcpClientIdentifier    = 61,
   endOption               = 255
};

/*
 * @brief DHCP message format
 */ 
typedef struct {
	uint8_t  op;            ///< @ref DHCP_BOOTREQUEST or @ref DHCP_BOOTREPLY
	uint8_t  htype;         ///< @ref DHCP_HTYPE10MB or @ref DHCP_HTYPE100MB
	uint8_t  hlen;          ///< @ref DHCP_HLENETHERNET
	uint8_t  hops;          ///< @ref DHCP_HOPS
	uint32_t xid;           ///< @ref DHCP_XID  This increase one every DHCP transaction.
	uint16_t secs;          ///< @ref DHCP_SECS
	uint16_t flags;         ///< @ref DHCP_FLAGSBROADCAST or @ref DHCP_FLAGSUNICAST
	uint8_t  ciaddr[4];     ///< @ref Request IP to DHCP sever
	uint8_t  yiaddr[4];     ///< @ref Offered IP from DHCP server
	uint8_t  siaddr[4];     ///< No use 
	uint8_t  giaddr[4];     ///< No use
	uint8_t  chaddr[16];    ///< DHCP client 6bytes MAC address. Others is filled to zero
	uint8_t  sname[64];     ///< No use
	uint8_t  file[128];     ///< No use
	uint8_t  OPT[OPT_SIZE]; ///< Option
} RIP_MSG;



uint8_t DHCP_SOCKET;                      // Socket number for DHCP

uint8_t DHCP_SIP[4];                      // DHCP Server IP address

// Network information from DHCP Server
uint8_t OLD_allocated_ip[4]   = {0, };    // Previous IP address
uint8_t DHCP_allocated_ip[4]  = {0, };    // IP address from DHCP
uint8_t DHCP_allocated_gw[4]  = {0, };    // Gateway address from DHCP
uint8_t DHCP_allocated_sn[4]  = {0, };    // Subnet mask from DHCP
uint8_t DHCP_allocated_dns[4] = {0, };    // DNS address from DHCP


int8_t   dhcp_state        = STATE_DHCP_INIT;   // DHCP state
int8_t   dhcp_retry_count  = 0;                 

uint32_t dhcp_lease_time   			= INFINITE_LEASETIME;
volatile uint32_t dhcp_tick_1s      = 0;                 // unit 1 second
uint32_t dhcp_tick_next    			= DHCP_WAIT_TIME ;

uint32_t DHCP_XID;      // Any number

RIP_MSG* pDHCPMSG;      // Buffer pointer for DHCP processing

uint8_t HOST_NAME[] = DCHP_HOST_NAME;  

uint8_t DHCP_CHADDR[6]; // DHCP Client MAC address.

/* The default callback function */
void default_ip_assign(void);
void default_ip_update(void);
void default_ip_conflict(void);

/* Callback handler */
void (*dhcp_ip_assign)(void)   = default_ip_assign;     /* handler to be called when the IP address from DHCP server is first assigned */
void (*dhcp_ip_update)(void)   = default_ip_update;     /* handler to be called when the IP address from DHCP server is updated */
void (*dhcp_ip_conflict)(void) = default_ip_conflict;   /* handler to be called when the IP address from DHCP server is conflict */

void reg_dhcp_cbfunc(void(*ip_assign)(void), void(*ip_update)(void), void(*ip_conflict)(void));


/* send DISCOVER message to DHCP server */
void     send_DHCP_DISCOVER(void);

/* send REQEUST message to DHCP server */
void     send_DHCP_REQUEST(void);

/* send DECLINE message to DHCP server */
void     send_DHCP_DECLINE(void);

/* IP conflict check by sending ARP-request to leased IP and wait ARP-response. */
int8_t   check_DHCP_leasedIP(void);

/* check the timeout in DHCP process */
uint8_t  check_DHCP_timeout(void);

/* Intialize to timeout process.  */
void     reset_DHCP_timeout(void);

/* Parse message as OFFER and ACK and NACK from DHCP server.*/
int8_t   parseDHCPCMSG(void);

/* The default handler of ip assign first */
void default_ip_assign(void)
{
   setSIPR(DHCP_allocated_ip);
   setSUBR(DHCP_allocated_sn);
   setGAR (DHCP_allocated_gw);
}

/* The default handler of ip changed */
void default_ip_update(void)
{
	/* WIZchip Software Reset */
   setMR(MR_RST);
   getMR(); // for delay
   default_ip_assign();
   setSHAR(DHCP_CHADDR);
}

/* The default handler of ip changed */
void default_ip_conflict(void)
{
	// WIZchip Software Reset
	setMR(MR_RST);
	getMR(); // for delay
	setSHAR(DHCP_CHADDR);
}

/* register the call back func. */
void reg_dhcp_cbfunc(void(*ip_assign)(void), void(*ip_update)(void), void(*ip_conflict)(void))
{
   dhcp_ip_assign   = default_ip_assign;
   dhcp_ip_update   = default_ip_update;
   dhcp_ip_conflict = default_ip_conflict;
   if(ip_assign)   dhcp_ip_assign = ip_assign;
   if(ip_update)   dhcp_ip_update = ip_update;
   if(ip_conflict) dhcp_ip_conflict = ip_conflict;
}

/* make the common DHCP message */
void makeDHCPMSG(void)
{
   uint8_t  bk_mac[6];
   uint8_t* ptmp;
   uint8_t  i;
   getSHAR(bk_mac);
	pDHCPMSG->op      = DHCP_BOOTREQUEST;
	pDHCPMSG->htype   = DHCP_HTYPE10MB;
	pDHCPMSG->hlen    = DHCP_HLENETHERNET;
	pDHCPMSG->hops    = DHCP_HOPS;
	ptmp              = (uint8_t*)(&pDHCPMSG->xid);
	*(ptmp+0)         = (uint8_t)((DHCP_XID & 0xFF000000) >> 24);
	*(ptmp+1)         = (uint8_t)((DHCP_XID & 0x00FF0000) >> 16);
   *(ptmp+2)         = (uint8_t)((DHCP_XID & 0x0000FF00) >>  8);
	*(ptmp+3)         = (uint8_t)((DHCP_XID & 0x000000FF) >>  0);   
	pDHCPMSG->secs    = DHCP_SECS;
	ptmp              = (uint8_t*)(&pDHCPMSG->flags);	
	*(ptmp+0)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0xFF00) >> 8);
	*(ptmp+1)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0x00FF) >> 0);

	pDHCPMSG->ciaddr[0] = 0;
	pDHCPMSG->ciaddr[1] = 0;
	pDHCPMSG->ciaddr[2] = 0;
	pDHCPMSG->ciaddr[3] = 0;

	pDHCPMSG->yiaddr[0] = 0;
	pDHCPMSG->yiaddr[1] = 0;
	pDHCPMSG->yiaddr[2] = 0;
	pDHCPMSG->yiaddr[3] = 0;

	pDHCPMSG->siaddr[0] = 0;
	pDHCPMSG->siaddr[1] = 0;
	pDHCPMSG->siaddr[2] = 0;
	pDHCPMSG->siaddr[3] = 0;

	pDHCPMSG->giaddr[0] = 0;
	pDHCPMSG->giaddr[1] = 0;
	pDHCPMSG->giaddr[2] = 0;
	pDHCPMSG->giaddr[3] = 0;

	pDHCPMSG->chaddr[0] = DHCP_CHADDR[0];
	pDHCPMSG->chaddr[1] = DHCP_CHADDR[1];
	pDHCPMSG->chaddr[2] = DHCP_CHADDR[2];
	pDHCPMSG->chaddr[3] = DHCP_CHADDR[3];
	pDHCPMSG->chaddr[4] = DHCP_CHADDR[4];
	pDHCPMSG->chaddr[5] = DHCP_CHADDR[5];

	for (i = 6; i < 16; i++)  pDHCPMSG->chaddr[i] = 0;
	for (i = 0; i < 64; i++)  pDHCPMSG->sname[i]  = 0;
	for (i = 0; i < 128; i++) pDHCPMSG->file[i]   = 0;

	// MAGIC_COOKIE
	pDHCPMSG->OPT[0] = (uint8_t)((MAGIC_COOKIE & 0xFF000000) >> 24);
	pDHCPMSG->OPT[1] = (uint8_t)((MAGIC_COOKIE & 0x00FF0000) >> 16);
	pDHCPMSG->OPT[2] = (uint8_t)((MAGIC_COOKIE & 0x0000FF00) >>  8);
	pDHCPMSG->OPT[3] = (uint8_t) (MAGIC_COOKIE & 0x000000FF) >>  0;
}

/* SEND DHCP DISCOVER */
void send_DHCP_DISCOVER(void)
{
	uint16_t i;
	uint8_t ip[4];
	uint16_t k = 0;
   
   makeDHCPMSG();

   k = 4;     // because MAGIC_COOKIE already made by makeDHCPMSG()
   
	// Option Request Param
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_DISCOVER;
	
	// Client identifier
	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[0];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[1];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[2];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];
	
	// host name
	pDHCPMSG->OPT[k++] = hostName;
	pDHCPMSG->OPT[k++] = 0;          // fill zero length of hostname 
	for(i = 0 ; HOST_NAME[i] != 0; i++)
   	pDHCPMSG->OPT[k++] = HOST_NAME[i];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];
	pDHCPMSG->OPT[k - (i+3+1)] = i+3; // length of hostname

	pDHCPMSG->OPT[k++] = dhcpParamRequest;
	pDHCPMSG->OPT[k++] = 0x06;	// length of request
	pDHCPMSG->OPT[k++] = subnetMask;
	pDHCPMSG->OPT[k++] = routersOnSubnet;
	pDHCPMSG->OPT[k++] = dns;
	pDHCPMSG->OPT[k++] = domainName;
	pDHCPMSG->OPT[k++] = dhcpT1value;
	pDHCPMSG->OPT[k++] = dhcpT2value;
	pDHCPMSG->OPT[k++] = endOption;

	for (i = k; i < OPT_SIZE; i++) pDHCPMSG->OPT[i] = 0;

	// send broadcasting packet
	ip[0] = 255;
	ip[1] = 255;
	ip[2] = 255;
	ip[3] = 255;

#ifdef _DHCP_DEBUG_
	printf("> Send DHCP_DISCOVER\r\n");
#endif

	sendto(DHCP_SOCKET, (uint8_t *)pDHCPMSG, RIP_MSG_SIZE, ip, DHCP_SERVER_PORT);
}

/* SEND DHCP REQUEST */
void send_DHCP_REQUEST(void)
{
	int i;
	uint8_t ip[4];
	uint16_t k = 0;

   makeDHCPMSG();

   if(dhcp_state == STATE_DHCP_LEASED || dhcp_state == STATE_DHCP_REREQUEST)
   {
   	*((uint8_t*)(&pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
   	*((uint8_t*)(&pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);
   	pDHCPMSG->ciaddr[0] = DHCP_allocated_ip[0];
   	pDHCPMSG->ciaddr[1] = DHCP_allocated_ip[1];
   	pDHCPMSG->ciaddr[2] = DHCP_allocated_ip[2];
   	pDHCPMSG->ciaddr[3] = DHCP_allocated_ip[3];
   	ip[0] = DHCP_SIP[0];
   	ip[1] = DHCP_SIP[1];
   	ip[2] = DHCP_SIP[2];
   	ip[3] = DHCP_SIP[3];   	   	   	
   }
   else
   {
   	ip[0] = 255;
   	ip[1] = 255;
   	ip[2] = 255;
   	ip[3] = 255;   	   	   	
   }
   
   k = 4;      // because MAGIC_COOKIE already made by makeDHCPMSG()
	
	// Option Request Param.
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_REQUEST;

	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[0];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[1];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[2];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];

   if(ip[3] == 255)  // if(dchp_state == STATE_DHCP_LEASED || dchp_state == DHCP_REREQUEST_STATE)
   {
		pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
		pDHCPMSG->OPT[k++] = 0x04;
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[0];
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[1];
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[2];
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[3];
	
		pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
		pDHCPMSG->OPT[k++] = 0x04;
		pDHCPMSG->OPT[k++] = DHCP_SIP[0];
		pDHCPMSG->OPT[k++] = DHCP_SIP[1];
		pDHCPMSG->OPT[k++] = DHCP_SIP[2];
		pDHCPMSG->OPT[k++] = DHCP_SIP[3];
	}

	// host name
	pDHCPMSG->OPT[k++] = hostName;
	pDHCPMSG->OPT[k++] = 0; // length of hostname
	for(i = 0 ; HOST_NAME[i] != 0; i++)
   	pDHCPMSG->OPT[k++] = HOST_NAME[i];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];
	pDHCPMSG->OPT[k - (i+3+1)] = i+3; // length of hostname
	
	pDHCPMSG->OPT[k++] = dhcpParamRequest;
	pDHCPMSG->OPT[k++] = 0x08;
	pDHCPMSG->OPT[k++] = subnetMask;
	pDHCPMSG->OPT[k++] = routersOnSubnet;
	pDHCPMSG->OPT[k++] = dns;
	pDHCPMSG->OPT[k++] = domainName;
	pDHCPMSG->OPT[k++] = dhcpT1value;
	pDHCPMSG->OPT[k++] = dhcpT2value;
	pDHCPMSG->OPT[k++] = performRouterDiscovery;
	pDHCPMSG->OPT[k++] = staticRoute;
	pDHCPMSG->OPT[k++] = endOption;

	for (i = k; i < OPT_SIZE; i++) pDHCPMSG->OPT[i] = 0;

#ifdef _DHCP_DEBUG_
	printf("> Send DHCP_REQUEST\r\n");
#endif
	
	sendto(DHCP_SOCKET, (uint8_t *)pDHCPMSG, RIP_MSG_SIZE, ip, DHCP_SERVER_PORT);

}

/* SEND DHCP DHCPDECLINE */
void send_DHCP_DECLINE(void)
{
	int i;
	uint8_t ip[4];
	uint16_t k = 0;
	
	makeDHCPMSG();

   k = 4;      // because MAGIC_COOKIE already made by makeDHCPMSG()
   
	*((uint8_t*)(&pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
	*((uint8_t*)(&pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);

	// Option Request Param.
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_DECLINE;

	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[0];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[1];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[2];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];

	pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
	pDHCPMSG->OPT[k++] = 0x04;
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[0];
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[1];
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[2];
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[3];

	pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
	pDHCPMSG->OPT[k++] = 0x04;
	pDHCPMSG->OPT[k++] = DHCP_SIP[0];
	pDHCPMSG->OPT[k++] = DHCP_SIP[1];
	pDHCPMSG->OPT[k++] = DHCP_SIP[2];
	pDHCPMSG->OPT[k++] = DHCP_SIP[3];

	pDHCPMSG->OPT[k++] = endOption;

	for (i = k; i < OPT_SIZE; i++) pDHCPMSG->OPT[i] = 0;

	//send broadcasting packet
	ip[0] = 0xFF;
	ip[1] = 0xFF;
	ip[2] = 0xFF;
	ip[3] = 0xFF;

#ifdef _DHCP_DEBUG_
	printf("\r\n> Send DHCP_DECLINE\r\n");
#endif

	sendto(DHCP_SOCKET, (uint8_t *)pDHCPMSG, RIP_MSG_SIZE, ip, DHCP_SERVER_PORT);
}

/* PARSE REPLY pDHCPMSG */
int8_t parseDHCPMSG(void)
{
	uint8_t svr_addr[6];
	uint16_t  svr_port;
	uint16_t len;

	uint8_t * p;
	uint8_t * e;
	uint8_t type;
	uint8_t opt_len;
   
   if((len = getSn_RX_RSR(DHCP_SOCKET)) > 0)
   {
   	len = recvfrom(DHCP_SOCKET, (uint8_t *)pDHCPMSG, len, svr_addr, &svr_port);
   #ifdef _DHCP_DEBUG_   
      printf("DHCP message : %d.%d.%d.%d(%d) %d received. \r\n",svr_addr[0],svr_addr[1],svr_addr[2], svr_addr[3],svr_port, len);
   #endif   
   }
   else return 0;
	if (svr_port == DHCP_SERVER_PORT) {
      // compare mac address
		if ( (pDHCPMSG->chaddr[0] != DHCP_CHADDR[0]) || (pDHCPMSG->chaddr[1] != DHCP_CHADDR[1]) ||
		     (pDHCPMSG->chaddr[2] != DHCP_CHADDR[2]) || (pDHCPMSG->chaddr[3] != DHCP_CHADDR[3]) ||
		     (pDHCPMSG->chaddr[4] != DHCP_CHADDR[4]) || (pDHCPMSG->chaddr[5] != DHCP_CHADDR[5])   )
         return 0;
      type = 0;
		p = (uint8_t *)(&pDHCPMSG->op);
		p = p + 240;      // 240 = sizeof(RIP_MSG) + MAGIC_COOKIE size in RIP_MSG.opt - sizeof(RIP_MSG.opt)
		e = p + (len - 240);

		while ( p < e ) {

			switch ( *p ) {

   			case endOption :
   			   p = e;   // for break while(p < e)
   				break;
            case padOption :
   				p++;
   				break;
   			case dhcpMessageType :
   				p++;
   				p++;
   				type = *p++;
   				break;
   			case subnetMask :
   				p++;
   				p++;
   				DHCP_allocated_sn[0] = *p++;
   				DHCP_allocated_sn[1] = *p++;
   				DHCP_allocated_sn[2] = *p++;
   				DHCP_allocated_sn[3] = *p++;
   				break;
   			case routersOnSubnet :
   				p++;
   				opt_len = *p++;       
   				DHCP_allocated_gw[0] = *p++;
   				DHCP_allocated_gw[1] = *p++;
   				DHCP_allocated_gw[2] = *p++;
   				DHCP_allocated_gw[3] = *p++;
   				p = p + (opt_len - 4);
   				break;
   			case dns :
   				p++;                  
   				opt_len = *p++;       
   				DHCP_allocated_dns[0] = *p++;
   				DHCP_allocated_dns[1] = *p++;
   				DHCP_allocated_dns[2] = *p++;
   				DHCP_allocated_dns[3] = *p++;
   				p = p + (opt_len - 4);
   				break;
   			case dhcpIPaddrLeaseTime :
   				p++;
   				opt_len = *p++;
   				dhcp_lease_time  = *p++;
   				dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
   				dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
   				dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
            #ifdef _DHCP_DEBUG_  
                dhcp_lease_time = 10;
 			#endif
   				break;
   			case dhcpServerIdentifier :
   				p++;
   				opt_len = *p++;
   				DHCP_SIP[0] = *p++;
   				DHCP_SIP[1] = *p++;
   				DHCP_SIP[2] = *p++;
   				DHCP_SIP[3] = *p++;
   				break;
   			default :
   				p++;
   				opt_len = *p++;
   				p += opt_len;
   				break;
			} // switch
		} // while
	} // if
	return	type;
}

uint8_t DHCP_run(void)
{
	uint8_t  type;
	uint8_t  ret;

	if(dhcp_state == STATE_DHCP_STOP) return DHCP_STOPPED;

	if(getSn_SR(DHCP_SOCKET) != SOCK_UDP)
	   socket(DHCP_SOCKET, Sn_MR_UDP, DHCP_CLIENT_PORT, 0x00);

	ret = DHCP_RUNNING;
	type = parseDHCPMSG();

	switch ( dhcp_state ) {
	   case STATE_DHCP_INIT     :
         DHCP_allocated_ip[0] = 0;
         DHCP_allocated_ip[1] = 0;
         DHCP_allocated_ip[2] = 0;
         DHCP_allocated_ip[3] = 0;
   		send_DHCP_DISCOVER();
   		dhcp_state = STATE_DHCP_DISCOVER;
   		break;
		case STATE_DHCP_DISCOVER :
			if (type == DHCP_OFFER){
#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_OFFER\r\n");
#endif
            DHCP_allocated_ip[0] = pDHCPMSG->yiaddr[0];
            DHCP_allocated_ip[1] = pDHCPMSG->yiaddr[1];
            DHCP_allocated_ip[2] = pDHCPMSG->yiaddr[2];
            DHCP_allocated_ip[3] = pDHCPMSG->yiaddr[3];

				send_DHCP_REQUEST();
				dhcp_state = STATE_DHCP_REQUEST;
			} else ret = check_DHCP_timeout();
         break;

		case STATE_DHCP_REQUEST :
			if (type == DHCP_ACK) {

#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_ACK\r\n");
#endif
				if (check_DHCP_leasedIP()) {
					// Network info assignment from DHCP
					dhcp_ip_assign();
					reset_DHCP_timeout();

					dhcp_state = STATE_DHCP_LEASED;
				} else {
					// IP address conflict occurred
					reset_DHCP_timeout();
					dhcp_ip_conflict();
				    dhcp_state = STATE_DHCP_INIT;
				}
			} else if (type == DHCP_NAK) {

#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_NACK\r\n");
#endif

				reset_DHCP_timeout();

				dhcp_state = STATE_DHCP_DISCOVER;
			} else ret = check_DHCP_timeout();
		break;

		case STATE_DHCP_LEASED :
		   ret = DHCP_IP_LEASED;
			if ((dhcp_lease_time != INFINITE_LEASETIME) && ((dhcp_lease_time/2) < dhcp_tick_1s)) {
				
#ifdef _DHCP_DEBUG_
 				printf("> Maintains the IP address \r\n");
#endif

				type = 0;
				OLD_allocated_ip[0] = DHCP_allocated_ip[0];
				OLD_allocated_ip[1] = DHCP_allocated_ip[1];
				OLD_allocated_ip[2] = DHCP_allocated_ip[2];
				OLD_allocated_ip[3] = DHCP_allocated_ip[3];
				
				DHCP_XID++;

				send_DHCP_REQUEST();

				reset_DHCP_timeout();

				dhcp_state = STATE_DHCP_REREQUEST;
			}
		break;

		case STATE_DHCP_REREQUEST :
		   ret = DHCP_IP_LEASED;
			if (type == DHCP_ACK) {
				dhcp_retry_count = 0;
				if (OLD_allocated_ip[0] != DHCP_allocated_ip[0] || 
				    OLD_allocated_ip[1] != DHCP_allocated_ip[1] ||
				    OLD_allocated_ip[2] != DHCP_allocated_ip[2] ||
				    OLD_allocated_ip[3] != DHCP_allocated_ip[3]) 
				{
					ret = DHCP_IP_CHANGED;
					dhcp_ip_update();
               #ifdef _DHCP_DEBUG_
                  printf(">IP changed.\r\n");
               #endif
					
				}
         #ifdef _DHCP_DEBUG_
            else printf(">IP is continued.\r\n");
         #endif            				
				reset_DHCP_timeout();
				dhcp_state = STATE_DHCP_LEASED;
			} else if (type == DHCP_NAK) {

#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_NACK, Failed to maintain ip\r\n");
#endif

				reset_DHCP_timeout();

				dhcp_state = STATE_DHCP_DISCOVER;
			} else ret = check_DHCP_timeout();
	   	break;
		default :
   		break;
	}

	return ret;
}

void    DHCP_stop(void)
{
   close(DHCP_SOCKET);
   dhcp_state = STATE_DHCP_STOP;
}

uint8_t check_DHCP_timeout(void)
{
	uint8_t ret = DHCP_RUNNING;
	
	if (dhcp_retry_count < MAX_DHCP_RETRY) {
		if (dhcp_tick_next < dhcp_tick_1s) {

			switch ( dhcp_state ) {
				case STATE_DHCP_DISCOVER :
//					printf("<<timeout>> state : STATE_DHCP_DISCOVER\r\n");
					send_DHCP_DISCOVER();
				break;
		
				case STATE_DHCP_REQUEST :
//					printf("<<timeout>> state : STATE_DHCP_REQUEST\r\n");

					send_DHCP_REQUEST();
				break;

				case STATE_DHCP_REREQUEST :
//					printf("<<timeout>> state : STATE_DHCP_REREQUEST\r\n");
					
					send_DHCP_REQUEST();
				break;
		
				default :
				break;
			}

			dhcp_tick_1s = 0;
			dhcp_tick_next = dhcp_tick_1s + DHCP_WAIT_TIME;
			dhcp_retry_count++;
		}
	} else { // timeout occurred

		switch(dhcp_state) {
			case STATE_DHCP_DISCOVER:
				dhcp_state = STATE_DHCP_INIT;
				ret = DHCP_FAILED;
				break;
			case STATE_DHCP_REQUEST:
			case STATE_DHCP_REREQUEST:
				send_DHCP_DISCOVER();
				dhcp_state = STATE_DHCP_DISCOVER;
				break;
			default :
				break;
		}
		reset_DHCP_timeout();
	}
	return ret;
}

int8_t check_DHCP_leasedIP(void)
{
	uint8_t tmp;
	int32_t ret;

	//WIZchip RCR value changed for ARP Timeout count control
	tmp = getRCR();
	setRCR(0x03);

	// IP conflict detection : ARP request - ARP reply
	// Broadcasting ARP Request for check the IP conflict using UDP sendto() function
	ret = sendto(DHCP_SOCKET, (uint8_t *)"CHECK_IP_CONFLICT", 17, DHCP_allocated_ip, 5000);

	// RCR value restore
	setRCR(tmp);

	if(ret == SOCKERR_TIMEOUT) {
		// UDP send Timeout occurred : allocated IP address is unique, DHCP Success

#ifdef _DHCP_DEBUG_
		printf("\r\n> Check leased IP - OK\r\n");
#endif

		return 1;
	} else {
		// Received ARP reply or etc : IP address conflict occur, DHCP Failed
		send_DHCP_DECLINE();

		ret = dhcp_tick_1s;
		while((dhcp_tick_1s - ret) < 2) ;   // wait for 1s over; wait to complete to send DECLINE message;

		return 0;
	}
}	

void DHCP_init(uint8_t s, uint8_t * buf)
{
   uint8_t zeroip[4] = {0,0,0,0};
   getSHAR(DHCP_CHADDR);
   if((DHCP_CHADDR[0] | DHCP_CHADDR[1]  | DHCP_CHADDR[2] | DHCP_CHADDR[3] | DHCP_CHADDR[4] | DHCP_CHADDR[5]) == 0x00)
   {
      // assign temporary mac address, you should be set SHAR before call this function.
      DHCP_CHADDR[0] = 0x00;
      DHCP_CHADDR[1] = 0x08;
      DHCP_CHADDR[2] = 0xdc;      
      DHCP_CHADDR[3] = 0x00;
      DHCP_CHADDR[4] = 0x00;
      DHCP_CHADDR[5] = 0x00; 
      setSHAR(DHCP_CHADDR);     
   }

	DHCP_SOCKET = s; // SOCK_DHCP
	pDHCPMSG = (RIP_MSG*)buf;
	DHCP_XID = 0x12345678;

	// WIZchip Netinfo Clear
	setSIPR(zeroip);
	setSIPR(zeroip);
	setGAR(zeroip);

	reset_DHCP_timeout();
	dhcp_state = STATE_DHCP_INIT;
}


/* Rset the DHCP timeout count and retry count. */
void reset_DHCP_timeout(void)
{
	dhcp_tick_1s = 0;
	dhcp_tick_next = DHCP_WAIT_TIME;
	dhcp_retry_count = 0;
}

void DHCP_time_handler(void)
{
	dhcp_tick_1s++;
}

void getIPfromDHCP(uint8_t* ip)
{
	ip[0] = DHCP_allocated_ip[0];
	ip[1] = DHCP_allocated_ip[1];
	ip[2] = DHCP_allocated_ip[2];	
	ip[3] = DHCP_allocated_ip[3];
}

void getGWfromDHCP(uint8_t* ip)
{
	ip[0] =DHCP_allocated_gw[0];
	ip[1] =DHCP_allocated_gw[1];
	ip[2] =DHCP_allocated_gw[2];
	ip[3] =DHCP_allocated_gw[3];			
}

void getSNfromDHCP(uint8_t* ip)
{
   ip[0] = DHCP_allocated_sn[0];
   ip[1] = DHCP_allocated_sn[1];
   ip[2] = DHCP_allocated_sn[2];
   ip[3] = DHCP_allocated_sn[3];         
}

void getDNSfromDHCP(uint8_t* ip)
{
   ip[0] = DHCP_allocated_dns[0];
   ip[1] = DHCP_allocated_dns[1];
   ip[2] = DHCP_allocated_dns[2];
   ip[3] = DHCP_allocated_dns[3];         
}

uint32_t getDHCPLeasetime(void)
{
	return dhcp_lease_time;
}




