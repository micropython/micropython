//*****************************************************************************
//
//! \file dhcp.h
//! \brief DHCP APIs Header file.
//! \details Processig DHCP protocol as DISCOVER, OFFER, REQUEST, ACK, NACK and DECLINE.
//! \version 1.1.0
//! \date 2013/11/18
//! \par  Revision history
//!       <2013/11/18> 1st Release
//!       <2012/12/20> V1.1.0
//!         1. Move unreferenced DEFINE to dhcp.c
//!       <2012/12/26> V1.1.1
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
#ifndef _DHCP_H_
#define _DHCP_H_

/*
 * @brief 
 * @details If you want to display debug & processing message, Define _DHCP_DEBUG_
 * @note    If defined, it depends on <stdio.h>
 */

//#define _DHCP_DEBUG_

/* Retry to processing DHCP */
#define	MAX_DHCP_RETRY          2        ///< Maximum retry count
#define	DHCP_WAIT_TIME          10       ///< Wait Time 10s

/* UDP port numbers for DHCP */
#define DHCP_SERVER_PORT      		67	      ///< DHCP server port number
#define DHCP_CLIENT_PORT         	68	      ///< DHCP client port number

#define MAGIC_COOKIE             0x63825363  ///< Any number. You can be modified it any number

#define DCHP_HOST_NAME           "WIZnet\0"

/* 
 * @brief return value of @ref DHCP_run()
 */
enum
{
   DHCP_FAILED = 0,  ///< Processing Fail
   DHCP_RUNNING,     ///< Processing DHCP protocol
   DHCP_IP_ASSIGN,   ///< First Occupy IP from DHPC server      (if cbfunc == null, act as default default_ip_assign)
   DHCP_IP_CHANGED,  ///< Change IP address by new IP address from DHCP (if cbfunc == null, act as default default_ip_update)
   DHCP_IP_LEASED,   ///< Stand by 
   DHCP_STOPPED      ///< Stop processing DHCP protocol
};

/*
 * @brief DHCP client initialization (outside of the main loop)
 * @param s   - socket number
 * @param buf - buffer for processing DHCP message
 */
void DHCP_init(uint8_t s, uint8_t * buf);

/*
 * @brief DHCP 1s Tick Timer handler
 * @note SHOULD BE register to your system 1s Tick timer handler 
 */
void DHCP_time_handler(void);

/* 
 * @brief Register call back function 
 * @param ip_assign   - callback func when IP is assigned from DHCP server first
 * @param ip_update   - callback func when IP is changed
 * @prarm ip_conflict - callback func when the assigned IP is conflict with others.
 */
void reg_dhcp_cbfunc(void(*ip_assign)(void), void(*ip_update)(void), void(*ip_conflict)(void));

/*
 * @brief DHCP client in the main loop
 * @return    The value is as the follow \n
 *            @ref DHCP_FAILED     \n
 *            @ref DHCP_RUNNING    \n
 *            @ref DHCP_IP_ASSIGN  \n
 *            @ref DHCP_IP_CHANGED \n
 * 			  @ref DHCP_IP_LEASED  \n
 *            @ref DHCP_STOPPED    \n
 *
 * @note This function is always called by you main task.
 */ 
uint8_t DHCP_run(void);

/*
 * @brief Stop DHCP processing
 * @note If you want to restart. call DHCP_init() and DHCP_run()
 */ 
void    DHCP_stop(void);

/* Get Network information assigned from DHCP server */
/*
 * @brief Get IP address
 * @param ip  - IP address to be returned
 */
void getIPfromDHCP(uint8_t* ip);
/*
 * @brief Get Gateway address
 * @param ip  - Gateway address to be returned
 */
void getGWfromDHCP(uint8_t* ip);
/*
 * @brief Get Subnet mask value
 * @param ip  - Subnet mask to be returned
 */
void getSNfromDHCP(uint8_t* ip);
/*
 * @brief Get DNS address
 * @param ip  - DNS address to be returned
 */
void getDNSfromDHCP(uint8_t* ip);

/*
 * @brief Get the leased time by DHCP sever
 * @return unit 1s
 */
uint32_t getDHCPLeasetime(void);

#endif	/* _DHCP_H_ */
