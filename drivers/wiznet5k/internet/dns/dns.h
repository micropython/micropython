//*****************************************************************************
//
//! \file dns.h
//! \brief DNS APIs Header file.
//! \details Send DNS query & Receive DNS reponse. 
//! \version 1.1.0
//! \date 2013/11/18
//! \par  Revision history
//!       <2013/10/21> 1st Release
//!       <2013/12/20> V1.1.0
//!         1. Remove secondary DNS server in DNS_run
//!            If 1st DNS_run failed, call DNS_run with 2nd DNS again
//!         2. DNS_timerHandler -> DNS_time_handler
//!         3. Move the no reference define to dns.c
//!         4. Integrated dns.h dns.c & dns_parse.h dns_parse.c into dns.h & dns.c
//!       <2013/12/20> V1.1.0
//!
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

#ifndef	_DNS_H_
#define	_DNS_H_

#include <stdint.h>
/*
 * @brief Define it for Debug & Monitor DNS processing.
 * @note If defined, it depends on <stdio.h>
 */

//#define _DNS_DEBUG_

#define	MAX_DNS_BUF_SIZE	256		///< maximum size of DNS buffer. */
/*
 * @brief Maximum length of your queried Domain name
 * @todo SHOULD BE defined it equal as or greater than your Domain name length + null character(1)
 * @note SHOULD BE careful to stack overflow because it is allocated 1.5 times as MAX_DOMAIN_NAME in stack.
 */
#define  MAX_DOMAIN_NAME   32       // for example "www.google.com"

#define	MAX_DNS_RETRY     2        ///< Requery Count
#define	DNS_WAIT_TIME     4        ///< Wait response time. unit 1s.

#define	IPPORT_DOMAIN     53       ///< DNS server port number

#define DNS_MSG_ID         0x1122   ///< ID for DNS message. You can be modified it any number
/*
 * @brief DNS process initialize
 * @param s   : Socket number for DNS
 * @param buf : Buffer for DNS message
 */
void DNS_init(uint8_t s, uint8_t * buf);

/*
 * @brief DNS process
 * @details Send DNS query and receive DNS response
 * @param dns_ip        : DNS server ip address
 * @param name          : Domain name to be queried
 * @param ip_from_dns   : IP address from DNS server
 * @return  -1 : failed. @ref MAX_DOMIN_NAME is too small \n
 *           0 : failed  (Timeout or Parse error)\n
 *           1 : success
 * @note This function blocks until success or fail. max time = @ref MAX_DNS_RETRY * @ref DNS_WAIT_TIME
 */
int8_t DNS_run(uint8_t * dns_ip, uint8_t * name, uint8_t * ip_from_dns);

#endif	/* _DNS_H_ */
