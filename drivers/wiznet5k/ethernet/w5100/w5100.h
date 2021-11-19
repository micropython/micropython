//* ****************************************************************************
//! \file w5100.h
//! \brief W5100 HAL Header File.
//! \version 1.0.0
//! \date 2013/10/21
//! \par  Revision history
//!       <2013/10/21> 1st Release
//! \author MidnightCow
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

#ifndef	_W5100_H_
#define	_W5100_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wizchip_conf.h"

/// \cond DOXY_APPLY_CODE
#if   (_WIZCHIP_ == 5100)
/// \endcond

#define _WIZCHIP_SN_BASE_  (0x0400)
#define _WIZCHIP_SN_SIZE_  (0x0100)
#define _WIZCHIP_IO_TXBUF_ (0x4000) /* Internal Tx buffer address of the iinchip */
#define _WIZCHIP_IO_RXBUF_ (0x6000) /* Internal Rx buffer address of the iinchip */


#define WIZCHIP_CREG_BLOCK      	      0x00   ///< Common register block
#define WIZCHIP_SREG_BLOCK(N)    	   (_WIZCHIP_SN_BASE_+ _WIZCHIP_SN_SIZE_*N) ///< Socket N register block

#define WIZCHIP_OFFSET_INC(ADDR, N)    (ADDR + N) ///< Increase offset address

#if (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_DIR_)
   #define _W5100_IO_BASE_     _WIZCHIP_IO_BASE_
#elif (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_)
	#define IDM_OR             ((_WIZCHIP_IO_BASE  + 0x0000))
	#define IDM_AR0            ((_WIZCHIP_IO_BASE_ + 0x0001))
	#define IDM_AR1            ((_WIZCHIP_IO_BASE_ + 0x0002))
	#define IDM_DR             ((_WIZCHIP_IO_BASE_ + 0x0003))
	#define _W5100_IO_BASE_    0x0000
#elif (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_)
   #define _W5100_IO_BASE_    0x0000
#endif

///////////////////////////////////////
// Definition For Legacy Chip Driver //
///////////////////////////////////////
#define IINCHIP_READ(ADDR)                WIZCHIP_READ(ADDR)               ///< The defined for legacy chip driver
#define IINCHIP_WRITE(ADDR,VAL)           WIZCHIP_WRITE(ADDR,VAL)          ///< The defined for legacy chip driver
#define IINCHIP_READ_BUF(ADDR,BUF,LEN)    WIZCHIP_READ_BUF(ADDR,BUF,LEN)   ///< The defined for legacy chip driver
#define IINCHIP_WRITE_BUF(ADDR,BUF,LEN)   WIZCHIP_WRITE(ADDR,BUF,LEN)      ///< The defined for legacy chip driver


//-----------    defgroup --------------------------------

/**
 * @defgroup W5100 W5100
 * @brief WHIZCHIP register defines and I/O functions of @b W5100.
 *
 * - @ref WIZCHIP_register_W5100 : @ref Common_register_group_W5100 and @ref Socket_register_group_W5100
 * - @ref WIZCHIP_IO_Functions_W5100 : @ref Basic_IO_function_W5100, @ref Common_register_access_function_W5100 and @ref Socket_register_group_W5100
 */
 
 /**
 * @defgroup WIZCHIP_register_W5100 WIZCHIP register
 * @ingroup W5100
 * @brief WIZCHIP register defines register group of <b> W5100 </b>.
 *
 * - \ref Common_register_group_W5100 : Common register group W5100
 * - \ref Socket_register_group_W5100 : \c SOCKET n register group W5100
 */
 

/**
 * @defgroup WIZCHIP_IO_Functions_W5100 WIZCHIP I/O functions
 * @ingroup W5100 
 * @brief This supports the basic I/O functions for \ref WIZCHIP_register_W5100.
 *
 * - <b> Basic I/O function </b> \n
 *   WIZCHIP_READ(), WIZCHIP_WRITE(), WIZCHIP_READ_BUF(), WIZCHIP_WRITE_BUF() \n\n
 *
 * - \ref Common_register_group_W5100 <b>access functions</b> \n
 * 	-# @b Mode \n
 *    getMR(), setMR()
 * 	-# @b Interrupt \n
 *    getIR(), setIR(), getIMR(), setIMR(), 
 * 	-# <b> Network Information </b> \n
 *    getSHAR(), setSHAR(), getGAR(), setGAR(), getSUBR(), setSUBR(), getSIPR(), setSIPR()
 * 	-# @b Retransmission \n
 *    getRCR(), setRCR(), getRTR(), setRTR()
 * 	-# @b PPPoE \n
 *    getPTIMER(), setPTIMER(), getPMAGIC(), getPMAGIC()
 *
 * - \ref Socket_register_group_W5100 <b>access functions</b> \n
 *   -# <b> SOCKET control</b> \n
 *      getSn_MR(), setSn_MR(), getSn_CR(), setSn_CR(), getSn_IR(), setSn_IR()
 *   -# <b> SOCKET information</b> \n
 *      getSn_SR(), getSn_DHAR(), setSn_DHAR(), getSn_PORT(), setSn_PORT(), getSn_DIPR(), setSn_DIPR(), getSn_DPORT(), setSn_DPORT()
 *      getSn_MSSR(), setSn_MSSR()
 *   -# <b> SOCKET communication </b> \n
 *      getSn_RXMEM_SIZE(), setSn_RXMEM_SIZE(), getSn_TXMEM_SIZE(), setSn_TXMEM_SIZE() \n
 *      getSn_TX_RD(), getSn_TX_WR(), setSn_TX_WR() \n
 *      getSn_RX_RD(), setSn_RX_RD(), getSn_RX_WR() \n
 *      getSn_TX_FSR(), getSn_RX_RSR()
 *   -# <b> IP header field </b> \n
 *      getSn_FRAG(), setSn_FRAG(),  getSn_TOS(), setSn_TOS() \n
 *      getSn_TTL(), setSn_TTL()
 */
 
/**
 * @defgroup Common_register_group_W5100 Common register
 * @ingroup WIZCHIP_register_W5100
 * @brief Common register group\n
 * It set the basic for the networking\n
 * It set the configuration such as interrupt, network information, ICMP, etc.
 * @details
 * @sa MR : Mode register.
 * @sa GAR, SUBR, SHAR, SIPR
 * @sa IR, Sn_IR, _IMR_  : Interrupt.
 * @sa _RTR_, _RCR_ : Data retransmission.
 * @sa PTIMER, PMAGIC : PPPoE.
 */


 /**
 * @defgroup Socket_register_group_W5100 Socket register
 * @ingroup WIZCHIP_register_W5100
 * @brief Socket register group\n
 * Socket register configures and control SOCKETn which is necessary to data communication.
 * @details
 * @sa Sn_MR, Sn_CR, Sn_IR : SOCKETn Control
 * @sa Sn_SR, Sn_PORT, Sn_DHAR, Sn_DIPR, Sn_DPORT : SOCKETn Information
 * @sa Sn_MSSR, Sn_TOS, Sn_TTL, Sn_FRAG : Internet protocol.
 * @sa Sn_RXMEM_SIZE, Sn_TXMEM_SIZE, Sn_TX_FSR, Sn_TX_RD, Sn_TX_WR, Sn_RX_RSR, Sn_RX_RD, Sn_RX_WR : Data communication
 */
 
 /**
 * @defgroup Basic_IO_function_W5100 Basic I/O function
 * @ingroup WIZCHIP_IO_Functions_W5100
 * @brief These are basic input/output functions to read values from register or write values to register.
 */

/**
 * @defgroup Common_register_access_function_W5100 Common register access functions
 * @ingroup WIZCHIP_IO_Functions_W5100
 * @brief These are functions to access <b>common registers</b>.
 */
 
/**
 * @defgroup Socket_register_access_function_W5100 Socket register access functions
 * @ingroup WIZCHIP_IO_Functions_W5100
 * @brief These are functions to access <b>socket registers</b>.
 */
 
 //-----------------------------------------------------------------------------------

//----------------------------- W5100 Common Registers IOMAP -----------------------------
/**
 * @ingroup Common_register_group_W5100
 * @brief Mode Register address(R/W)\n
 * \ref MR is used for S/W reset, ping block mode, PPPoE mode and etc.
 * @details Each bit of \ref MR defined as follows.
 * <table>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>RST</td> <td>Reserved</td> <td>WOL</td> <td>PB</td> <td>PPPoE</td> <td>Reserved</td> <td>AI</td> <td>IND</td> </tr>
 * </table>
 * - \ref MR_RST		 	   : Reset
 * - \ref MR_PB         	: Ping block
 * - \ref MR_PPPOE      	: PPPoE mode
 * - \ref MR_AI         	: Address Auto-Increment in Indirect Bus Interface
 * - \ref MR_IND         	: Indirect Bus Interface mode
 */
#if _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_
   #define MR					(_WIZCHIP_IO_BASE_ + (0x0000))  // Mode
#else
   #define MR					(_W5100_IO_BASE_ + (0x0000))  // Mode
#endif   

/**
 * @ingroup Common_register_group_W5100
 * @brief Gateway IP Register address(R/W)
 * @details \ref GAR configures the default gateway address.
 */
#define GAR     			(_W5100_IO_BASE_ + (0x0001))  // GW Address

/**
 * @ingroup Common_register_group_W5100
 * @brief Subnet mask Register address(R/W)
 * @details \ref SUBR configures the subnet mask address.
 */
#define SUBR    			(_W5100_IO_BASE_ + (0x0005)) // SN Mask Address

/**
 * @ingroup Common_register_group_W5100
 * @brief Source MAC Register address(R/W)
 * @details \ref SHAR configures the source hardware address.
 */
#define SHAR    			(_W5100_IO_BASE_ + (0x0009)) // Source Hardware Address

/**
 * @ingroup Common_register_group_W5100
 * @brief Source IP Register address(R/W)
 * @details \ref SIPR configures the source IP address.
 */
#define SIPR    			(_W5100_IO_BASE_ + (0x000F)) // Source IP Address

// Reserved					(_W5100_IO_BASE_ + (0x0013))
// Reserved					(_W5100_IO_BASE_ + (0x0014))

/**
 * @ingroup Common_register_group_W5100
 * @brief Interrupt Register(R/W)
 * @details \ref IR indicates the interrupt status. Each bit of \ref IR will be still until the bit will be written to by the host.
 * If \ref IR is not equal to x00 INTn PIN is asserted to low until it is x00\n\n
 * Each bit of \ref IR defined as follows.
 * <table>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>CONFLICT</td> <td>UNREACH</td> <td>PPPoE</td> <td>Reserved</td> <td>S3_INT</td> <td>S2_INT</td> <td>S1_INT</td> <td>S0_INT</td> </tr>
 * </table>
 * - \ref IR_CONFLICT : IP conflict
 * - \ref IR_UNREACH  : Destination unreachable
 * - \ref IR_PPPoE	 : PPPoE connection close
 * - \ref IR_SOCK(3)  : SOCKET 3 Interrupt
 * - \ref IR_SOCK(2)  : SOCKET 2 Interrupt
 * - \ref IR_SOCK(1)  : SOCKET 1 Interrupt
 * - \ref IR_SOCK(0)  : SOCKET 0 Interrupt 
 */
#define IR					(_W5100_IO_BASE_ + (0x0015)) // Interrupt

/**
 * @ingroup Common_register_group_W5100
 * @brief Socket Interrupt Mask Register(R/W)
 * @details Each bit of \ref _IMR_ corresponds to each bit of \ref IR.
 * When a bit of \ref _IMR_ is and the corresponding bit of \ref IR is set, Interrupt will be issued.
 */
#define _IMR_    			(_W5100_IO_BASE_ + (0x0016)) // Socket Interrupt Mask

/**
 * @ingroup Common_register_group_W5100
 * @brief Timeout register address( 1 is 100us )(R/W)
 * @details \ref _RTR_ configures the retransmission timeout period. The unit of timeout period is 100us and the default of \ref _RTR_ is x07D0or 000
 * And so the default timeout period is 200ms(100us X 2000). During the time configured by \ref _RTR_, W5100 waits for the peer response
 * to the packet that is transmitted by \ref Sn_CR (CONNECT, DISCON, CLOSE, SEND, SEND_MAC, SEND_KEEP command).
 * If the peer does not respond within the \ref _RTR_ time, W5100 retransmits the packet or issues timeout.
 */
#define _RTR_     			(_W5100_IO_BASE_ + (0x0017)) // Retry Time

/**
 * @ingroup Common_register_group_W5100
 * @brief Retry count register(R/W)
 * @details \ref _RCR_ configures the number of time of retransmission.
 * When retransmission occurs as many as ref _RCR_+1 Timeout interrupt is issued (\ref Sn_IR_TIMEOUT = '1').
 */
#define _RCR_      		(_W5100_IO_BASE_ + (0x0019)) // Retry Count
#define RMSR				(_W5100_IO_BASE_ + (0x001A)) // Receicve Memory Size
#define TMSR				(_W5100_IO_BASE_ + (0x001B)) // Trnasmit Memory Size


/**
 * @ingroup Common_register_group_W5100
 * @brief PPP LCP Request Timer register  in PPPoE mode(R)
 * @details \ref PATR notifies authentication method that has been agreed at the connection with
 * PPPoE Server. W5100 supports two types of Authentication method - PAP and CHAP.
 */
#define PATR            (_W5100_IO_BASE_ + (0x001C))


/**
 * @ingroup Common_register_group_W5100
 * @brief PPP LCP Request Timer register  in PPPoE mode(R)
 * @details \ref PTIMER configures the time for sending LCP echo request. The unit of time is 25ms.
 */
#define PTIMER  			(_W5100_IO_BASE_ + (0x0028)) // PPP LCP RequestTimer

/**
 * @ingroup Common_register_group_W5100
 * @brief PPP LCP Magic number register  in PPPoE mode(R)
 * @details \ref PMAGIC configures the 4bytes magic number to be used in LCP negotiation.
 */
#define PMAGIC   			(_W5100_IO_BASE_ + (0x0029)) // PPP LCP Magic number

#define UIPR0				(_W5100_IO_BASE_ + (0x002A))
#define UPORT0          (_W5100_IO_BASE  + (0x002E))



//----------------------------- W5100 Socket Registers -----------------------------

//--------------------------- For Backward Compatibility ---------------------------

/**
 * @ingroup Socket_register_group_W5100
 * @brief socket Mode register(R/W)
 * @details \ref Sn_MR configures the option or protocol type of Socket n.\n\n
 * Each bit of \ref Sn_MR defined as the following.
 * <table>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>MULTI</td> <td>MF</td> <td>ND/MC</td> <td>Reserved</td> <td>Protocol[3]</td> <td>Protocol[2]</td> <td>Protocol[1]</td> <td>Protocol[0]</td> </tr>
 * </table>
 * - \ref Sn_MR_MULTI	: Support UDP Multicasting
 * - \ref Sn_MR_MF	   : Support MACRAW
 * - \ref Sn_MR_ND		: No Delayed Ack(TCP) flag
 * - \ref Sn_MR_MC   	: IGMP version used <b>in UDP mulitcasting</b>
 * - <b>Protocol</b>
 * <table>
 * 		<tr>   <td><b>Protocol[3]</b></td> <td><b>Protocol[2]</b></td> <td><b>Protocol[1]</b></td> <td><b>Protocol[0]</b></td> <td>@b Meaning</td>   </tr>
 * 		<tr>   <td>0</td> <td>0</td> <td>0</td> <td>0</td> <td>Closed</td>   </tr>
 * 		<tr>   <td>0</td> <td>0</td> <td>0</td> <td>1</td> <td>TCP</td>   </tr>
 * 		<tr>   <td>0</td> <td>0</td> <td>1</td> <td>0</td> <td>UDP</td>   </tr>
 * 		<tr>   <td>0</td> <td>1</td> <td>0</td> <td>0</td> <td>MACRAW</td>   </tr>
 * </table>
 * - <b>In case of Socket 0</b>
 *  <table>
 * 		<tr>   <td><b>Protocol[3]</b></td> <td><b>Protocol[2]</b></td> <td><b>Protocol[1]</b></td> <td><b>Protocol[0]</b></td> <td>@b Meaning</td>   </tr>
 * 		<tr>   <td>0</td> <td>1</td> <td>0</td> <td>0</td> <td>MACRAW</td>   </tr>
 * 		<tr>   <td>0</td> <td>1</td> <td>0</td> <td>1</td> <td>PPPoE</td>   </tr>
 * </table> 
 *	 - \ref Sn_MR_MACRAW	: MAC LAYER RAW SOCK \n
 *  - \ref Sn_MR_UDP		: UDP
 *  - \ref Sn_MR_TCP		: TCP
 *  - \ref Sn_MR_CLOSE	: Unused socket
 *  @note MACRAW mode should be only used in Socket 0.
 */
#define Sn_MR(sn)			(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0000)) // socket Mode register

/**
 * @ingroup Socket_register_group_W5100
 * @brief Socket command register(R/W)
 * @details This is used to set the command for Socket n such as OPEN, CLOSE, CONNECT, LISTEN, SEND, and RECEIVE.\n
 * After W5100 accepts the command, the \ref Sn_CR register is automatically cleared to 0x00.
 * Even though \ref Sn_CR is cleared to 0x00, the command is still being processed.\n
 * To check whether the command is completed or not, please check the \ref Sn_IR or \ref Sn_SR.
 * - \ref Sn_CR_OPEN 		: Initialize or open socket.
 * - \ref Sn_CR_LISTEN 		: Wait connection request in TCP mode(<b>Server mode</b>)
 * - \ref Sn_CR_CONNECT 	: Send connection request in TCP mode(<b>Client mode</b>)
 * - \ref Sn_CR_DISCON 		: Send closing request in TCP mode.
 * - \ref Sn_CR_CLOSE   	: Close socket.
 * - \ref Sn_CR_SEND    	: Update TX buffer pointer and send data.
 * - \ref Sn_CR_SEND_MAC	: Send data with MAC address, so without ARP process.
 * - \ref Sn_CR_SEND_KEEP 	: Send keep alive message.
 * - \ref Sn_CR_RECV		: Update RX buffer pointer and receive data.
 * - <b>In case of S0_MR(P3:P0) = S0_MR_PPPoE</b>
 *  <table>
 * 		<tr>   <td><b>Value</b></td> <td><b>Symbol</b></td> <td><b>Description</b></td></tr>
 * 		<tr>   <td>0x23</td> <td>PCON</td> <td>PPPoE connection begins by transmitting PPPoE discovery packet</td>  </tr>
 * 		<tr>   <td>0x24</td> <td>PDISCON</td> <td>Closes PPPoE connection</td>  </tr>
 * 		<tr>   <td>0x25</td> <td>PCR</td> <td>In each phase, it transmits REQ message.</td> </tr>
 * 		<tr>   <td>0x26</td> <td>PCN</td> <td>In each phase, it transmits NAK message.</td> </tr>
 * 		<tr>   <td>0x27</td> <td>PCJ</td> <td>In each phase, it transmits REJECT message.</td> </tr>
 * </table> 
 */
#define Sn_CR(sn)			(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0001)) // channel Sn_CR register

/**
 * @ingroup Socket_register_group_W5100
 * @brief Socket interrupt register(R)
 * @details \ref Sn_IR indicates the status of Socket Interrupt such as establishment, termination, receiving data, timeout).\n
 * When an interrupt occurs and the corresponding bit \ref IR_SOCK(N) in \ref _IMR_ are set, \ref IR_SOCK(N) in \ref IR becomes '1'.\n
 * In order to clear the \ref Sn_IR bit, the host should write the bit to \n
 * <table>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>PRECV</td> <td>PFAIL</td> <td>PNEXT</td> <td>SEND_OK</td> <td>TIMEOUT</td> <td>RECV</td> <td>DISCON</td> <td>CON</td> </tr>
 * </table>
 * - \ref Sn_IR_PRECV : <b>PPP Receive Interrupt</b>
 * - \ref Sn_IR_PFAIL : <b>PPP Fail Interrupt</b>
 * - \ref Sn_IR_PNEXT : <b>PPP Next Phase Interrupt</b>
 * - \ref Sn_IR_SENDOK : <b>SEND_OK Interrupt</b>
 * - \ref Sn_IR_TIMEOUT : <b>TIMEOUT Interrupt</b>
 * - \ref Sn_IR_RECV : <b>RECV Interrupt</b>
 * - \ref Sn_IR_DISCON : <b>DISCON Interrupt</b>
 * - \ref Sn_IR_CON : <b>CON Interrupt</b>
 */
#define Sn_IR(sn)			(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0002)) // channel interrupt register

/**
 * @ingroup Socket_register_group_W5100
 * @brief Socket status register(R)
 * @details \ref Sn_SR indicates the status of Socket n.\n
 * The status of Socket n is changed by \ref Sn_CR or some special control packet as SYN, FIN packet in TCP.
 * @par Normal status
 * - \ref SOCK_CLOSED 		: Closed
 * - \ref SOCK_INIT   		: Initiate state
 * - \ref SOCK_LISTEN    	: Listen state
 * - \ref SOCK_ESTABLISHED 	: Success to connect
 * - \ref SOCK_CLOSE_WAIT   : Closing state
 * - \ref SOCK_UDP   		: UDP socket
 * - \ref SOCK_MACRAW  		: MAC raw mode socket
 *@par Temporary status during changing the status of Socket n.
 * - \ref SOCK_SYNSENT   	: This indicates Socket n sent the connect-request packet (SYN packet) to a peer.
 * - \ref SOCK_SYNRECV    	: It indicates Socket n successfully received the connect-request packet (SYN packet) from a peer.
 * - \ref SOCK_FIN_WAIT		: Connection state
 * - \ref SOCK_CLOSING		: Closing state
 * - \ref SOCK_TIME_WAIT	: Closing state
 * - \ref SOCK_LAST_ACK 	: Closing state
 */
#define Sn_SR(sn)			(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0003)) // channel status register

/**
 * @ingroup Socket_register_group_W5100
 * @brief source port register(R/W)
 * @details \ref Sn_PORT configures the source port number of Socket n.
 * It is valid when Socket n is used in TCP/UDP mode. It should be set before OPEN command is ordered.
*/
#define Sn_PORT(sn)		(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0004)) // source port register

/**
 * @ingroup Socket_register_group_W5100
 * @brief Peer MAC register address(R/W)
 * @details \ref Sn_DHAR configures the destination hardware address of Socket n when using SEND_MAC command in UDP mode or
 * it indicates that it is acquired in ARP-process by CONNECT/SEND command.
 */
#define Sn_DHAR(sn)     (_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0006)) // Peer MAC register address

/**
 * @ingroup Socket_register_group_W5100
 * @brief Peer IP register address(R/W)
 * @details \ref Sn_DIPR configures or indicates the destination IP address of Socket n. It is valid when Socket n is used in TCP/UDP mode.
 * In TCP client mode, it configures an IP address of TCP server before CONNECT command.
 * In TCP server mode, it indicates an IP address of TCP client after successfully establishing connection.
 * In UDP mode, it configures an IP address of peer to be received the UDP packet by SEND or SEND_MAC command.
 */
#define Sn_DIPR(sn)		(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x000C)) // Peer IP register address

/**
 * @ingroup Socket_register_group_W5100
 * @brief Peer port register address(R/W)
 * @details \ref Sn_DPORT configures or indicates the destination port number of Socket n. It is valid when Socket n is used in TCP/UDP mode.
 * In TCP clientmode, it configures the listen port number of TCP server before CONNECT command.
 * In TCP Servermode, it indicates the port number of TCP client after successfully establishing connection.
 * In UDP mode, it configures the port number of peer to be transmitted the UDP packet by SEND/SEND_MAC command.
 */
#define Sn_DPORT(sn)    (_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0010)) // Peer port register address

/**
 * @ingroup Socket_register_group_W5100
 * @brief Maximum Segment Size(Sn_MSSR0) register address(R/W)
 * @details \ref Sn_MSSR configures or indicates the MTU(Maximum Transfer Unit) of Socket n.
 */
#define Sn_MSSR(sn)	   (_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0012)) // Maximum Segment Size(Sn_MSSR0) register address

/**
 * @ingroup Socket_register_group_W5100
 * @brief IP Protocol(PROTO) Register(R/W)
 * @details \ref Sn_PROTO that sets the protocol number field of the IP header at the IP layer. It is
 * valid only in IPRAW mode, and ignored in other modes.
 */
#define Sn_PROTO(sn)	   (_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0014)) // Protocol of IP Header field register in IP raw mode

/**
 * @ingroup Socket_register_group_W5100
 * @brief IP Type of Service(TOS) Register(R/W)
 * @details \ref Sn_TOS configures the TOS(Type Of Service field in IP Header) of Socket n.
 * It is set before OPEN command.
 */
#define Sn_TOS(sn)			(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + 0x0015) // IP Type of Service(TOS) Register 

/**
 * @ingroup Socket_register_group_W5100
 * @brief IP Time to live(TTL) Register(R/W)
 * @details \ref Sn_TTL configures the TTL(Time To Live field in IP header) of Socket n.
 * It is set before OPEN command.
 */
#define Sn_TTL(sn)		(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0016)) // IP Time to live(TTL) Register 

// Reserved					(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0017))
// Reserved					(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0018))
// Reserved					(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0019))
// Reserved					(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001A))
// Reserved					(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001B))
// Reserved					(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001C))
// Reserved					(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001D))

/**
 * @ingroup Socket_register_group_W5100
 * @brief Transmit free memory size register(R)
 * @details \ref Sn_TX_FSR indicates the free size of Socket n TX Buffer Block. It is initialized to the configured size by \ref Sn_TXMEM_SIZE.
 * Data bigger than \ref Sn_TX_FSR should not be saved in the Socket n TX Buffer because the bigger data overwrites the previous saved data not yet sent.
 * Therefore, check before saving the data to the Socket n TX Buffer, and if data is equal or smaller than its checked size,
 * transmit the data with SEND/SEND_MAC command after saving the data in Socket n TX buffer. But, if data is bigger than its checked size,
 * transmit the data after dividing into the checked size and saving in the Socket n TX buffer.
 */
#define Sn_TX_FSR(sn)	(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0020)) // Transmit free memory size register

/**
 * @ingroup Socket_register_group_W5100
 * @brief Transmit memory read pointer register address(R)
 * @details \ref Sn_TX_RD is initialized by OPEN command. However, if Sn_MR(P[3:0]) is TCP mode(001), it is re-initialized while connecting with TCP.
 * After its initialization, it is auto-increased by SEND command.
 * SEND command transmits the saved data from the current \ref Sn_TX_RD to the \ref Sn_TX_WR in the Socket n TX Buffer.
 * After transmitting the saved data, the SEND command increases the \ref Sn_TX_RD as same as the \ref Sn_TX_WR.
 * If its increment value exceeds the maximum value 0xFFFF, (greater than 0x10000 and the carry bit occurs),
 * then the carry bit is ignored and will automatically update with the lower 16bits value.
 */
#define Sn_TX_RD(sn)		(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0022)) // Transmit memory read pointer register address

/**
 * @ingroup Socket_register_group_W5100
 * @brief Transmit memory write pointer register address(R/W)
 * @details \ref Sn_TX_WR is initialized by OPEN command. However, if Sn_MR(P[3:0]) is TCP mode(001), it is re-initialized while connecting with TCP.\n
 * It should be read or be updated like as follows.\n
 * 1. Read the starting address for saving the transmitting data.\n
 * 2. Save the transmitting data from the starting address of Socket n TX buffer.\n
 * 3. After saving the transmitting data, update \ref Sn_TX_WR to the increased value as many as transmitting data size.
 * If the increment value exceeds the maximum value 0xFFFF(greater than 0x10000 and the carry bit occurs),
 * then the carry bit is ignored and will automatically update with the lower 16bits value.\n
 * 4. Transmit the saved data in Socket n TX Buffer by using SEND/SEND command
 */
#define Sn_TX_WR(sn)		(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0024)) // Transmit memory write pointer register address

/**
 * @ingroup Socket_register_group_W5100
 * @brief Received data size register(R)
 * @details \ref Sn_RX_RSR indicates the data size received and saved in Socket n RX Buffer.
 * \ref Sn_RX_RSR does not exceed the \ref Sn_RXMEM_SIZE and is calculated as the difference between
 * Socket n RX Write Pointer (\ref Sn_RX_WR)and Socket n RX Read Pointer (\ref Sn_RX_RD)
 */
#define Sn_RX_RSR(sn)	(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0026)) // Received data size register

/**
 * @ingroup Socket_register_group_W5100
 * @brief Read point of Receive memory(R/W)
 * @details \ref Sn_RX_RD is initialized by OPEN command. Make sure to be read or updated as follows.\n
 * 1. Read the starting save address of the received data.\n
 * 2. Read data from the starting address of Socket n RX Buffer.\n
 * 3. After reading the received data, Update \ref Sn_RX_RD to the increased value as many as the reading size.
 * If the increment value exceeds the maximum value 0xFFFF, that is, is greater than 0x10000 and the carry bit occurs,
 * update with the lower 16bits value ignored the carry bit.\n
 * 4. Order RECV command is for notifying the updated \ref Sn_RX_RD to W5100.
 */
#define Sn_RX_RD(sn)		(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0028)) // Read point of Receive memory

/**
 * @ingroup Socket_register_group_W5100
 * @brief Write point of Receive memory(R)
 * @details \ref Sn_RX_WR is initialized by OPEN command and it is auto-increased by the data reception.
 * If the increased value exceeds the maximum value 0xFFFF, (greater than 0x10000 and the carry bit occurs),
 * then the carry bit is ignored and will automatically update with the lower 16bits value.
 */
#define Sn_RX_WR(sn)		(_W5100_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x002A)) // Write point of Receive memory


//----------------------------- W5100 Register values  -----------------------------

/* MODE register values */
/**
 * @brief Reset
 * @details If this bit is  All internal registers will be initialized. It will be automatically cleared as after S/W reset.
 */
#define MR_RST				0x80 ///< reset 


/**
 * @brief Ping block
 * @details 0 : Disable Ping block\n
 * 1 : Enable Ping block\n
 * If the bit is  it blocks the response to a ping request.
 */
#define MR_PB				0x10 ///< ping block 

/**
 * @brief Enable PPPoE
 * @details 0 : DisablePPPoE mode\n
 * 1 : EnablePPPoE mode\n
 * If you use ADSL, this bit should be '1'.
 */
#define MR_PPPOE			0x08 ///< enable pppoe 

/**
 * @brief Address Auto-Increment in Indirect Bus Interface
 * @details 0 : Disable auto-increment \n
 * 1 : Enable auto-incremente \n
 * At the Indirect Bus Interface mode, if this bit is set as ��1��, the address will
 * be automatically increased by 1 whenever read and write are performed.
 */
#define MR_AI				0x02 ///< auto-increment in indirect mode 

/**
 * @brief Indirect Bus Interface mode
 * @details 0 : Disable Indirect bus Interface mode \n
 * 1 : Enable Indirect bus Interface mode \n
 * If this bit is set as ��1��, Indirect Bus Interface mode is set.
 */
#define MR_IND				0x01 ///< enable indirect mode 

/* IR register values */
/**
 * @brief Check IP conflict.
 * @details Bit is set as when own source IP address is same with the sender IP address in the received ARP request.
 */
#define IR_CONFLICT			0x80 ///< check ip confict 

/**
 * @brief Get the destination unreachable message in UDP sending.
 * @details When receiving the ICMP (Destination port unreachable) packet, this bit is set as 
 * When this bit is  Destination Information such as IP address and Port number may be checked with the corresponding @ref UIPR & @ref UPORTR.
 */
#define IR_UNREACH			0x40 ///< check destination unreachable

/**
 * @brief Get the PPPoE close message.
 * @details When PPPoE is disconnected during PPPoE mode, this bit is set.
 */
#define IR_PPPoE			0x20 ///< get the PPPoE close message 

#define IR_SOCK(sn)		(0x01 << sn)	///< check socket interrupt


// Sn_MR values 
/* Sn_MR Default values */
/**
 * @brief Unused socket
 * @details This configures the protocol mode of Socket n.
 */
#define Sn_MR_CLOSE			0x00 ///< unused socket 

/**
 * @brief TCP
 * @details This configures the protocol mode of Socket n.
 */
#define Sn_MR_TCP			0x01 ///< TCP 

/**
 * @brief UDP
 * @details This configures the protocol mode of Socket n.
 */
#define Sn_MR_UDP       0x02 ///< UDP 
#define Sn_MR_IPRAW     0x03 ///< IP LAYER RAW SOCK 

/**
 * @brief MAC LAYER RAW SOCK
 * @details This configures the protocol mode of Socket n.
 * @note MACRAW mode should be only used in Socket 0.
 */
#define Sn_MR_MACRAW		0x04 ///< MAC LAYER RAW SOCK 

/**
 * @brief PPPoE
 * @details This configures the protocol mode of Socket n.
 * @note PPPoE mode should be only used in Socket 0.
 */
#define Sn_MR_PPPoE			0x05 ///< PPPoE

/**
 * @brief No Delayed Ack(TCP), Multicast flag
 * @details 0 : Disable No Delayed ACK option\n
 * 1 : Enable No Delayed ACK option\n
 * This bit is applied only during TCP mode (P[3:0] = 001).\n
 * When this bit is  It sends the ACK packet without delay as soon as a Data packet is received from a peer.\n
 * When this bit is  It sends the ACK packet after waiting for the timeout time configured by \ref _RTR_.
 */
#define Sn_MR_ND			0x20 ///< No Delayed Ack(TCP) flag 

/**
 * @brief Support UDP Multicasting
 * @details 0 : using IGMP version 2\n
 * 1 : using IGMP version 1\n
 * This bit is applied only during UDP mode(P[3:0] = 010 and MULTI = '1')
 * It configures the version for IGMP messages (Join/Leave/Report).
 */
#define Sn_MR_MC			Sn_MR_ND ///< Select IGMP version 1(0) or 2(1) 

/**
 * @brief MAC filter enable in @ref Sn_MR_MACRAW mode
 * @details 0 : disable MAC Filtering\n
 * 1 : enable MAC Filtering\n
 * This bit is applied only during MACRAW mode(P[3:0] = 100.\n
 * When set as  W5100 can only receive broadcasting packet or packet sent to itself.
 * When this bit is  W5100 can receive all packets on Ethernet.
 * If user wants to implement Hybrid TCP/IP stack,
 * it is recommended that this bit is set as for reducing host overhead to process the all received packets.
 */
#define Sn_MR_MF			0x40 ///< Use MAC filter
#define Sn_MR_MFEN      Sn_MR_MF


/* Sn_MR Default values */
/**
 * @brief Support UDP Multicasting
 * @details 0 : disable Multicasting\n
 * 1 : enable Multicasting\n
 * This bit is applied only during UDP mode(P[3:0] = 010).\n
 * To use multicasting, \ref Sn_DIPR & \ref Sn_DPORT should be respectively configured with the multicast group IP address & port number
 * before Socket n is opened by OPEN command of \ref Sn_CR.
 */
#define Sn_MR_MULTI			0x80 ///< support multicating 

/* Sn_CR values */
/**
 * @brief Initialize or open socket
 * @details Socket n is initialized and opened according to the protocol selected in Sn_MR(P3:P0).
 * The table below shows the value of \ref Sn_SR corresponding to \ref Sn_MR.\n
 * <table>
 *   <tr>  <td>\b Sn_MR (P[3:0])</td> <td>\b Sn_SR</td>            		 </tr>
 *   <tr>  <td>Sn_MR_CLOSE  (000)</td> <td>--</td>         	   		 </tr>
 *   <tr>  <td>Sn_MR_TCP  (001)</td> <td>SOCK_INIT (0x13)</td>  		 </tr>
 *   <tr>  <td>Sn_MR_UDP  (010)</td>  <td>SOCK_UDP (0x22)</td>  		 </tr>
 *   <tr>  <td>S0_MR_IPRAW  (011)</td>  <td>SOCK_IPRAW (0x32)</td>  </tr>
 *   <tr>  <td>S0_MR_MACRAW  (100)</td>  <td>SOCK_MACRAW (0x42)</td>  </tr>
 *   <tr>  <td>S0_MR_PPPoE  (101)</td>  <td>SOCK_PPPoE (0x5F)</td>  </tr>
 * </table>
 */
#define Sn_CR_OPEN			0x01 ///< initialize or open socket 

/**
 * @brief Wait connection request in TCP mode(Server mode)
 * @details This is valid only in TCP mode (Sn_MR(P3:P0) = \ref Sn_MR_TCP).//
 * In this mode, Socket n operates as a 'TCP server' and waits for connection-request (SYN packet) from any 'TCP client'.//
 * The \ref Sn_SR changes the state from SOCK_INIT to SOCKET_LISTEN.//
 * When a 'TCP client' connection request is successfully established,
 * the \ref Sn_SR changes from SOCK_LISTEN to SOCK_ESTABLISHED and the Sn_IR(0) becomes 
 * But when a 'TCP client' connection request is failed, Sn_IR(3) becomes and the status of \ref Sn_SR changes to SOCK_CLOSED.
 */
#define Sn_CR_LISTEN		0x02 ///< wait connection request in tcp mode(Server mode) 

/**
 * @brief Send connection request in TCP mode(Client mode)
 * @details  To connect, a connect-request (SYN packet) is sent to <b>TCP server</b>configured by \ref Sn_DIPR & Sn_DPORT(destination address & port).
 * If the connect-request is successful, the \ref Sn_SR is changed to \ref SOCK_ESTABLISHED and the Sn_IR(0) becomes \n\n
 * The connect-request fails in the following three cases.\n
 * 1. When a @b ARPTO occurs (\ref Sn_IR[3] = '1') because destination hardware address is not acquired through the ARP-process.\n
 * 2. When a @b SYN/ACK packet is not received and @b TCPTO (Sn_IR(3) ='1')\n
 * 3. When a @b RST packet is received instead of a @b SYN/ACK packet. In these cases, \ref Sn_SR is changed to \ref SOCK_CLOSED.
 * @note This is valid only in TCP mode and operates when Socket n acts as <b>TCP client</b>
 */
#define Sn_CR_CONNECT		0x04 ///< send connection request in tcp mode(Client mode) 

/**
 * @brief Send closing request in TCP mode
 * @details Regardless of <b>TCP server</b>or <b>TCP client</b> the DISCON command processes the disconnect-process (<b>Active close</b>or <b>Passive close</b>.\n
 * @par Active close
 * it transmits disconnect-request(FIN packet) to the connected peer\n
 * @par Passive close
 * When FIN packet is received from peer, a FIN packet is replied back to the peer.\n
 * @details When the disconnect-process is successful (that is, FIN/ACK packet is received successfully), \ref Sn_SR is changed to \ref SOCK_CLOSED.\n
 * Otherwise, TCPTO occurs (Sn_IR(3)='1') and then \ref Sn_SR is changed to \ref SOCK_CLOSED.
 * @note Valid only in TCP mode.
 */
#define Sn_CR_DISCON		0x08 ///< send closing reqeuset in tcp mode 

/**
 * @brief Close socket
 * @details Sn_SR is changed to \ref SOCK_CLOSED.
 */
#define Sn_CR_CLOSE         0x10

/**
 * @brief Update TX buffer pointer and send data
 * @details SEND transmits all the data in the Socket n TX buffer.\n
 * For more details, please refer to Socket n TX Free Size Register (\ref Sn_TX_FSR), Socket n,
 * TX Write Pointer Register(\ref Sn_TX_WR), and Socket n TX Read Pointer Register(\ref Sn_TX_RD).
 */
#define Sn_CR_SEND          0x20

/**
 * @brief Send data with MAC address, so without ARP process
 * @details The basic operation is same as SEND.\n
 * Normally SEND transmits data after destination hardware address is acquired by the automatic ARP-process(Address Resolution Protocol).\n
 * But SEND_MAC transmits data without the automatic ARP-process.\n
 * In this case, the destination hardware address is acquired from \ref Sn_DHAR configured by host, instead of APR-process.
 * @note Valid only in UDP mode.
 */
#define Sn_CR_SEND_MAC      0x21

/**
 * @brief Send keep alive message
 * @details It checks the connection status by sending 1byte keep-alive packet.\n
 * If the peer can not respond to the keep-alive packet during timeout time, the connection is terminated and the timeout interrupt will occur.
 * @note Valid only in TCP mode.
 */
#define Sn_CR_SEND_KEEP     0x22

/**
 * @brief Update RX buffer pointer and receive data
 * @details RECV completes the processing of the received data in Socket n RX Buffer by using a RX read pointer register (\ref Sn_RX_RD).\n
 * For more details, refer to Socket n RX Received Size Register (\ref Sn_RX_RSR), Socket n RX Write Pointer Register (\ref Sn_RX_WR),
 * and Socket n RX Read Pointer Register (\ref Sn_RX_RD).
 */
#define Sn_CR_RECV          0x40

/**
 * @brief PPPoE connection
 * @details PPPoE connection begins by transmitting PPPoE discovery packet
 */
#define Sn_CR_PCON			0x23

/**
 * @brief Closes PPPoE connection
 * @details Closes PPPoE connection
 */		 
#define Sn_CR_PDISCON		0x24		 

/**
 * @brief REQ message transmission
 * @details In each phase, it transmits REQ message.
 */
#define Sn_CR_PCR			0x25		 

/**
 * @brief NAK massage transmission
 * @details In each phase, it transmits NAK message.
 */
#define Sn_CR_PCN			0x26		

/**
 * @brief REJECT message transmission
 * @details In each phase, it transmits REJECT message.
 */
#define Sn_CR_PCJ			0x27		

/* Sn_IR values */
/**
 * @brief PPP Receive Interrupt
 * @details PPP Receive Interrupts when the option which is not supported is received.
 */
#define Sn_IR_PRECV			0x80	

/**
 * @brief PPP Fail Interrupt
 * @details PPP Fail Interrupts when PAP Authentication is failed.
 */	
#define Sn_IR_PFAIL			0x40		

/**
 * @brief PPP Next Phase Interrupt
 * @details PPP Next Phase Interrupts when the phase is changed during ADSL connection process.
 */
#define Sn_IR_PNEXT			0x20		

/**
 * @brief SEND_OK Interrupt
 * @details This is issued when SEND command is completed.
 */
#define Sn_IR_SENDOK		0x10 ///< complete sending 

/**
 * @brief TIMEOUT Interrupt
 * @details This is issued when ARPTO or TCPTO occurs.
 */
#define Sn_IR_TIMEOUT		0x08 ///< assert timeout 

/**
 * @brief RECV Interrupt
 * @details This is issued whenever data is received from a peer.
 */
#define Sn_IR_RECV          0x04

/**
 * @brief DISCON Interrupt
 * @details This is issued when FIN or FIN/ACK packet is received from a peer.
 */
#define Sn_IR_DISCON        0x02

/**
 * @brief CON Interrupt
 * @details This is issued one time when the connection with peer is successful and then \ref Sn_SR is changed to \ref SOCK_ESTABLISHED.
 */
#define Sn_IR_CON           0x01

/* Sn_SR values */
/**
 * @brief Closed
 * @details This indicates that Socket n is released.\n
 * When DICON, CLOSE command is ordered, or when a timeout occurs, it is changed to \ref SOCK_CLOSED regardless of previous status.
 */
#define SOCK_CLOSED			0x00 ///< closed 

/**
 * @brief Initiate state
 * @details This indicates Socket n is opened with TCP mode.\n
 * It is changed to \ref SOCK_INIT when Sn_MR(P[3:0]) = 001)and OPEN command is ordered.\n
 * After \ref SOCK_INIT, user can use LISTEN /CONNECT command.
 */
#define SOCK_INIT 			0x13 ///< init state 

/**
 * @brief Listen state
 * @details This indicates Socket n is operating as <b>TCP server</b>mode and waiting for connection-request (SYN packet) from a peer (<b>TCP client</b>).\n
 * It will change to \ref SOCK_ESTABLISHED when the connection-request is successfully accepted.\n
 * Otherwise it will change to \ref SOCK_CLOSED after TCPTO occurred (Sn_IR(TIMEOUT) = '1').
 */
#define SOCK_LISTEN         0x14

/**
 * @brief Connection state
 * @details This indicates Socket n sent the connect-request packet (SYN packet) to a peer.\n
 * It is temporarily shown when \ref Sn_SR is changed from \ref SOCK_INIT to \ref SOCK_ESTABLISHED by CONNECT command.\n
 * If connect-accept(SYN/ACK packet) is received from the peer at SOCK_SYNSENT, it changes to \ref SOCK_ESTABLISHED.\n
 * Otherwise, it changes to \ref SOCK_CLOSED after TCPTO (\ref Sn_IR[TIMEOUT] = '1') is occurred.
 */
#define SOCK_SYNSENT        0x15

/**
 * @brief Connection state
 * @details It indicates Socket n successfully received the connect-request packet (SYN packet) from a peer.\n
 * If socket n sends the response (SYN/ACK  packet) to the peer successfully,  it changes to \ref SOCK_ESTABLISHED. \n
 * If not, it changes to \ref SOCK_CLOSED after timeout occurs (\ref Sn_IR[TIMEOUT] = '1').
 */
#define SOCK_SYNRECV        0x16

/**
 * @brief Success to connect
 * @details This indicates the status of the connection of Socket n.\n
 * It changes to \ref SOCK_ESTABLISHED when the <b>TCP SERVER</b>processed the SYN packet from the <b>TCP CLIENT</b>during \ref SOCK_LISTEN, or
 * when the CONNECT command is successful.\n
 * During \ref SOCK_ESTABLISHED, DATA packet can be transferred using SEND or RECV command.
 */
#define SOCK_ESTABLISHED    0x17

/**
 * @brief Closing state
 * @details These indicate Socket n is closing.\n
 * These are shown in disconnect-process such as active-close and passive-close.\n
 * When Disconnect-process is successfully completed, or when timeout occurs, these change to \ref SOCK_CLOSED.
 */
#define SOCK_FIN_WAIT       0x18

/**
 * @brief Closing state
 * @details These indicate Socket n is closing.\n
 * These are shown in disconnect-process such as active-close and passive-close.\n
 * When Disconnect-process is successfully completed, or when timeout occurs, these change to \ref SOCK_CLOSED.
 */
#define SOCK_CLOSING        0x1A

/**
 * @brief Closing state
 * @details These indicate Socket n is closing.\n
 * These are shown in disconnect-process such as active-close and passive-close.\n
 * When Disconnect-process is successfully completed, or when timeout occurs, these change to \ref SOCK_CLOSED.
 */
#define SOCK_TIME_WAIT      0x1B

/**
 * @brief Closing state
 * @details This indicates Socket n received the disconnect-request (FIN packet) from the connected peer.\n
 * This is half-closing status, and data can be transferred.\n
 * For full-closing, DISCON command is used. But For just-closing, @ref Sn_CR_CLOSE command is used.
 */
#define SOCK_CLOSE_WAIT     0x1C

/**
 * @brief Closing state
 * @details This indicates Socket n is waiting for the response (FIN/ACK packet) to the disconnect-request (FIN packet) by passive-close.\n
 * It changes to \ref SOCK_CLOSED when Socket n received the response successfully, or when timeout occurs  (\ref Sn_IR[TIMEOUT] = '1').
 */
#define SOCK_LAST_ACK       0x1D

/**
 * @brief UDP socket
 * @details This indicates Socket n is opened in UDP mode(Sn_MR(P[3:0]) = 010).\n
 * It changes to SOCK_UDP when Sn_MR(P[3:0]) = 010 and @ref Sn_CR_OPEN command is ordered.\n
 * Unlike TCP mode, data can be transfered without the connection-process.
 */
#define SOCK_UDP			0x22 ///< udp socket 

/**
 * @brief IP raw mode socket
 * @details TThe socket is opened in IPRAW mode. The SOCKET status is change to SOCK_IPRAW when @ref Sn_MR (P3:P0) is
 * Sn_MR_IPRAW and @ref Sn_CR_OPEN command is used.\n
 * IP Packet can be transferred without a connection similar to the UDP mode.
*/
#define SOCK_IPRAW			0x32 ///< ip raw mode socket 

/**
 * @brief MAC raw mode socket
 * @details This indicates Socket 0 is opened in MACRAW mode (@ref Sn_MR(P[3:0]) = '100' and n=0) and is valid only in Socket 0.\n
 * It changes to SOCK_MACRAW when @ref Sn_MR(P[3:0]) = '100' and @ref Sn_CR_OPEN command is ordered.\n
 * Like UDP mode socket, MACRAW mode Socket 0 can transfer a MAC packet (Ethernet frame) without the connection-process.
 */
#define SOCK_MACRAW			0x42 ///< mac raw mode socket 

/**
 * @brief PPPoE mode socket
 * @details It is the status that SOCKET0 is open as PPPoE mode. It is changed to SOCK_PPPoE in case of S0_CR=OPEN and S0_MR
 * (P3:P0)=S0_MR_PPPoE.\n
 * It is temporarily used at the PPPoE
connection.
 */
#define SOCK_PPPOE			0x5F ///< pppoe socket 

// IP PROTOCOL 
#define IPPROTO_IP			0 ///< Dummy for IP 
#define IPPROTO_ICMP		   1 ///< Control message protocol 
#define IPPROTO_IGMP		   2 ///< Internet group management protocol 
#define IPPROTO_GGP			3 ///< GW^2 (deprecated) 
#define IPPROTO_TCP			6 ///< TCP 
#define IPPROTO_PUP			12 ///< PUP 
#define IPPROTO_UDP			17 ///< UDP 
#define IPPROTO_IDP			22 ///< XNS idp 
#define IPPROTO_ND			77 ///< UNOFFICIAL net disk protocol 
#define IPPROTO_RAW			255 ///< Raw IP packet 

/**
 * @brief Enter a critical section
 *
 * @details It is provided to protect your shared code which are executed without distribution. \n \n
 *
 * In non-OS environment, It can be just implemented by disabling whole interrupt.\n
 * In OS environment, You can replace it to critical section api supported by OS.
 *
 * \sa WIZCHIP_READ(), WIZCHIP_WRITE(), WIZCHIP_READ_BUF(), WIZCHIP_WRITE_BUF()
 * \sa WIZCHIP_CRITICAL_EXIT()
 */
#define WIZCHIP_CRITICAL_ENTER()    WIZCHIP.CRIS._enter()

#ifdef _exit
#undef _exit
#endif

/**
 * @brief Exit a critical section
 *
 * @details It is provided to protect your shared code which are executed without distribution. \n\n
 *
 * In non-OS environment, It can be just implemented by disabling whole interrupt. \n
 * In OS environment, You can replace it to critical section api supported by OS.
 *
 * @sa WIZCHIP_READ(), WIZCHIP_WRITE(), WIZCHIP_READ_BUF(), WIZCHIP_WRITE_BUF()
 * @sa WIZCHIP_CRITICAL_ENTER()
 */
#define WIZCHIP_CRITICAL_EXIT()     WIZCHIP.CRIS._exit()



////////////////////////
// Basic I/O Function //
////////////////////////
//
//M20150601 :  uint16_t AddrSel --> uint32_t AddrSel
//
/**
 * @ingroup Basic_IO_function_W5100
 * @brief It reads 1 byte value from a register.
 * @param AddrSel Register address
 * @return The value of register
 */
uint8_t  WIZCHIP_READ (uint32_t AddrSel);

/**
 * @ingroup Basic_IO_function_W5100
 * @brief It writes 1 byte value to a register.
 * @param AddrSel Register address
 * @param wb Write data
 * @return void
 */
void     WIZCHIP_WRITE(uint32_t AddrSel, uint8_t wb );

/**
 * @ingroup Basic_IO_function_W5100
 * @brief It reads sequence data from registers.
 * @param AddrSel Register address
 * @param pBuf Pointer buffer to read data
 * @param len Data length
 */
void     WIZCHIP_READ_BUF (uint32_t AddrSel, uint8_t* pBuf, uint16_t len);

/**
 * @ingroup Basic_IO_function_W5100
 * @brief It writes sequence data to registers.
 * @param AddrSel Register address
 * @param pBuf Pointer buffer to write data
 * @param len Data length
 */
void     WIZCHIP_WRITE_BUF(uint32_t AddrSel, uint8_t* pBuf, uint16_t len);


/////////////////////////////////
// Common Register IO function //
/////////////////////////////////

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set Mode Register
 * @param (uint8_t)mr The value to be set.
 * @sa getMR()
 */
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_)
   #define setMR(mr) 	WIZCHIP_WRITE(MR,mr)
#else
   #define setMR(mr)    (*((uint8_t*)MR) = mr)
#endif

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get @ref MR.
 * @return uint8_t. The value of Mode register.
 * @sa setMR()
 */
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_)
   #define getMR() 		WIZCHIP_READ(MR)
#else
   #define getMR()      (*(uint8_t*)MR)
#endif
   
/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set @ref GAR.
 * @param (uint8_t*)gar Pointer variable to set gateway IP address. It should be allocated 4 bytes.
 * @sa getGAR()
 */
#define setGAR(gar) \
		WIZCHIP_WRITE_BUF(GAR,gar,4)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get @ref GAR.
 * @param (uint8_t*)gar Pointer variable to get gateway IP address. It should be allocated 4 bytes.
 * @sa setGAR()
 */
#define getGAR(gar) \
		WIZCHIP_READ_BUF(GAR,gar,4)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set @ref SUBR.
 * @param (uint8_t*)subr Pointer variable to set subnet mask address. It should be allocated 4 bytes.
 * @note If subr is null pointer, set the backup subnet to SUBR. \n
 *       If subr is 0.0.0.0, back up SUBR and clear it. \n
 *       Otherwize, set subr to SUBR
 * @sa getSUBR()
 */
#define setSUBR(subr) \
      WIZCHIP_WRITE_BUF(SUBR,subr,4)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get @ref SUBR.
 * @param (uint8_t*)subr Pointer variable to get subnet mask address. It should be allocated 4 bytes.
 * @sa setSUBR()
 */
#define getSUBR(subr) \
		WIZCHIP_READ_BUF(SUBR, subr, 4)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set @ref SHAR.
 * @param (uint8_t*)shar Pointer variable to set local MAC address. It should be allocated 6 bytes.
 * @sa getSHAR()
 */
#define setSHAR(shar) \
		WIZCHIP_WRITE_BUF(SHAR, shar, 6)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get @ref SHAR.
 * @param (uint8_t*)shar Pointer variable to get local MAC address. It should be allocated 6 bytes.
 * @sa setSHAR()
 */
#define getSHAR(shar) \
		WIZCHIP_READ_BUF(SHAR, shar, 6)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set @ref SIPR.
 * @param (uint8_t*)sipr Pointer variable to set local IP address. It should be allocated 4 bytes.
 * @sa getSIPR()
*/
#define setSIPR(sipr) \
		WIZCHIP_WRITE_BUF(SIPR, sipr, 4)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get @ref SIPR.
 * @param (uint8_t*)sipr Pointer variable to get local IP address. It should be allocated 4 bytes.
 * @sa setSIPR()
 */
#define getSIPR(sipr) \
		WIZCHIP_READ_BUF(SIPR, sipr, 4)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set \ref IR register
 * @param (uint8_t)ir Value to set \ref IR register.
 * @sa getIR()
 */
#define setIR(ir) \
		WIZCHIP_WRITE(IR, (ir & 0xA0))
/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref IR register
 * @return uint8_t. Value of \ref IR register.
 * @sa setIR()
 */
#define getIR() \
		(WIZCHIP_READ(IR) & 0xA0)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set \ref _IMR_ register
 * @param (uint8_t)imr Value to set @ref _IMR_ register.
 * @sa getIMR()
 */
#define setIMR(imr) \
		WIZCHIP_WRITE(_IMR_, imr)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref _IMR_ register
 * @return uint8_t. Value of @ref _IMR_ register.
 * @sa setIMR()
 */
#define getIMR() \
		WIZCHIP_READ(_IMR_)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set \ref _RTR_ register
 * @param (uint16_t)rtr Value to set @ref _RTR_ register.
 * @sa getRTR()
 */
#define setRTR(rtr)   {\
		WIZCHIP_WRITE(_RTR_,   (uint8_t)(rtr >> 8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(_RTR_,1), (uint8_t) rtr); \
	}

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref _RTR_ register
 * @return uint16_t. Value of @ref _RTR_ register.
 * @sa setRTR()
 */
#define getRTR() \
		(((uint16_t)WIZCHIP_READ(_RTR_) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(_RTR_,1)))

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set \ref _RCR_ register
 * @param (uint8_t)rcr Value to set @ref _RCR_ register.
 * @sa getRCR()
 */
#define setRCR(rcr) \
		WIZCHIP_WRITE(_RCR_, rcr)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref _RCR_ register
 * @return uint8_t. Value of @ref _RCR_ register.
 * @sa setRCR()
 */
#define getRCR() \
		WIZCHIP_READ(_RCR_)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref RMSR register
 * @sa getRMSR()
 */
#define setRMSR(rmsr)   \
      WIZCHIP_WRITE(RMSR) // Receicve Memory Size

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref RMSR register
 * @return uint8_t. Value of @ref RMSR register.
 * @sa setRMSR()
 */
 #define getRMSR()   \
      WIZCHIP_READ() // Receicve Memory Size

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref TMSR register
 * @sa getTMSR()
 */
#define setTMSR(rmsr)   \
      WIZCHIP_WRITE(TMSR) // Receicve Memory Size

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref TMSR register
 * @return uint8_t. Value of @ref TMSR register.
 * @sa setTMSR()
 */


/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref PATR register
 * @return uint16_t. Value to set \ref PATR register
 */
#define getPATR() \
		(((uint16_t)WIZCHIP_READ(PATR) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(PATR,1)))

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref PPPALGO register
 * @return uint8_t. Value to set \ref PPPALGO register
 */
#define getPPPALGO() \
		WIZCHIP_READ(PPPALGO)


/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set \ref PTIMER register
 * @param (uint8_t)ptimer Value to set \ref PTIMER register.
 * @sa getPTIMER()
 */
#define setPTIMER(ptimer) \
		WIZCHIP_WRITE(PTIMER, ptimer)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref PTIMER register
 * @return uint8_t. Value of @ref PTIMER register.
 * @sa setPTIMER()
 */
#define getPTIMER() \
		WIZCHIP_READ(PTIMER)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Set \ref PMAGIC register
 * @param (uint8_t)pmagic Value to set @ref PMAGIC register.
 * @sa getPMAGIC()
 */
#define setPMAGIC(pmagic) \
		WIZCHIP_WRITE(PMAGIC, pmagic)

/**
 * @ingroup Common_register_access_function_W5100
 * @brief Get \ref PMAGIC register
 * @return uint8_t. Value of @ref PMAGIC register.
 * @sa setPMAGIC()
 */
#define getPMAGIC() \
		WIZCHIP_READ(PMAGIC)

///////////////////////////////////
// Socket N register I/O function //
///////////////////////////////////
/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_MR register
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 * @param mr Value to set @ref Sn_MR
 * @sa getSn_MR()
 */
#define setSn_MR(sn, mr) \
		WIZCHIP_WRITE(Sn_MR(sn),mr)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_MR register
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 * @return Value of @ref Sn_MR.
 * @sa setSn_MR()
 */
#define getSn_MR(sn) \
	WIZCHIP_READ(Sn_MR(sn))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)cr Value to set @ref Sn_CR
 * @sa getSn_CR()
 */
#define setSn_CR(sn, cr) \
		WIZCHIP_WRITE(Sn_CR(sn), cr)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_CR.
 * @sa setSn_CR()
 */
#define getSn_CR(sn) \
		WIZCHIP_READ(Sn_CR(sn))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_IR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)ir Value to set @ref Sn_IR
 * @sa getSn_IR()
 */
#define setSn_IR(sn, ir) \
		WIZCHIP_WRITE(Sn_IR(sn), ir)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_IR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_IR.
 * @sa setSn_IR()
 */
#define getSn_IR(sn) \
		WIZCHIP_READ(Sn_IR(sn))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_SR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_SR.
 */
#define getSn_SR(sn) \
		WIZCHIP_READ(Sn_SR(sn))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_PORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)port Value to set @ref Sn_PORT.
 * @sa getSn_PORT()
 */
#define setSn_PORT(sn, port)  { \
		WIZCHIP_WRITE(Sn_PORT(sn),   (uint8_t)(port >> 8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_PORT(sn),1), (uint8_t) port); \
	}

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_PORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_PORT.
 * @sa setSn_PORT()
 */
#define getSn_PORT(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_PORT(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_PORT(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_DHAR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t*)dhar Pointer variable to set socket n destination hardware address. It should be allocated 6 bytes.
 * @sa getSn_DHAR()
 */
#define setSn_DHAR(sn, dhar) \
		WIZCHIP_WRITE_BUF(Sn_DHAR(sn), dhar, 6)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_DHAR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t*)dhar Pointer variable to get socket n destination hardware address. It should be allocated 6 bytes.
 * @sa setSn_DHAR()
 */
#define getSn_DHAR(sn, dhar) \
		WIZCHIP_READ_BUF(Sn_DHAR(sn), dhar, 6)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t*)dipr Pointer variable to set socket n destination IP address. It should be allocated 4 bytes.
 * @sa getSn_DIPR()
 */
#define setSn_DIPR(sn, dipr) \
		WIZCHIP_WRITE_BUF(Sn_DIPR(sn), dipr, 4)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t*)dipr Pointer variable to get socket n destination IP address. It should be allocated 4 bytes.
 * @sa SetSn_DIPR()
 */
#define getSn_DIPR(sn, dipr) \
		WIZCHIP_READ_BUF(Sn_DIPR(sn), dipr, 4)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_DPORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)dport Value to set @ref Sn_DPORT
 * @sa getSn_DPORT()
 */
#define setSn_DPORT(sn, dport) { \
		WIZCHIP_WRITE(Sn_DPORT(sn),   (uint8_t) (dport>>8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_DPORT(sn),1), (uint8_t)  dport); \
	}

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_DPORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_DPORT.
 * @sa setSn_DPORT()
 */
#define getSn_DPORT(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_DPORT(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DPORT(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_MSSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)mss Value to set @ref Sn_MSSR
 * @sa setSn_MSSR()
 */
#define setSn_MSSR(sn, mss) { \
		WIZCHIP_WRITE(Sn_MSSR(sn),   (uint8_t)(mss>>8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_MSSR(sn),1), (uint8_t) mss); \
	}

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_MSSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_MSSR.
 * @sa setSn_MSSR()
 */
#define getSn_MSSR(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_MSSR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_MSSR(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_PROTO register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)proto Value to set \ref Sn_PROTO
 * @sa getSn_PROTO()
 */
#define setSn_PROTO(sn, proto) \
		WIZCHIP_WRITE(Sn_TOS(sn), tos)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_PROTO register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_PROTO.
 * @sa setSn_PROTO()
 */
#define getSn_PROTO(sn) \
		WIZCHIP_READ(Sn_TOS(sn))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_TOS register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)tos Value to set @ref Sn_TOS
 * @sa getSn_TOS()
 */
#define setSn_TOS(sn, tos) \
		WIZCHIP_WRITE(Sn_TOS(sn), tos)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_TOS register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_ </b>.
 * @return uint8_t. Value of Sn_TOS.
 * @sa setSn_TOS()
 */
#define getSn_TOS(sn) \
		WIZCHIP_READ(Sn_TOS(sn))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_TTL register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_ </b>.
 * @param (uint8_t)ttl Value to set @ref Sn_TTL
 * @sa getSn_TTL()
 */
#define setSn_TTL(sn, ttl) \
		WIZCHIP_WRITE(Sn_TTL(sn), ttl)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_TTL register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_ </b>.
 * @return uint8_t. Value of @ref Sn_TTL.
 * @sa setSn_TTL()
 */
#define getSn_TTL(sn) \
		WIZCHIP_READ(Sn_TTL(sn))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_RXMEM_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_ </b>.
 * @param (uint8_t)rxmemsize Value to set \ref Sn_RXMEM_SIZE
 * @sa getSn_RXMEM_SIZE()
 */
#define  setSn_RXMEM_SIZE(sn, rxmemsize) \
      WIZCHIP_WRITE(RMSR, (WIZCHIP_READ(RMSR) & ~(0x03 << (2*sn))) | (rxmemsize << (2*sn)))
#define  setSn_RXBUF_SIZE(sn,rxmemsize) setSn_RXMEM_SIZE(sn,rxmemsize)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_RXMEM_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_RXMEM.
 * @sa setSn_RXMEM_SIZE()
 */
#define  getSn_RXMEM_SIZE(sn) \
      ((WIZCHIP_READ(RMSR) & (0x03 << (2*sn))) >> (2*sn))
#define  getSn_RXBUF_SIZE(sn) getSn_RXMEM_SIZE(sn)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_TXMEM_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)txmemsize Value to set \ref Sn_TXMEM_SIZE
 * @sa getSn_TXMEM_SIZE()
 */
#define setSn_TXMEM_SIZE(sn, txmemsize) \
      WIZCHIP_WRITE(TMSR, (WIZCHIP_READ(TMSR) & ~(0x03 << (2*sn))) | (txmemsize << (2*sn)))
#define  setSn_TXBUF_SIZE(sn, txmemsize) setSn_TXMEM_SIZE(sn,txmemsize)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_TXMEM_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_TXMEM_SIZE.
 * @sa setSn_TXMEM_SIZE()
 */
#define  getSn_TXMEM_SIZE(sn) \
      ((WIZCHIP_READ(TMSR) & (0x03 << (2*sn))) >> (2*sn))
#define  getSn_TXBUF_SIZE(sn) getSn_TXMEM_SIZE(sn)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_TX_FSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_TX_FSR.
 */
uint16_t getSn_TX_FSR(uint8_t sn);

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_TX_RD register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_TX_RD.
 */
#define getSn_TX_RD(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_TX_RD(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_RD(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_TX_WR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)txwr Value to set @ref Sn_TX_WR
 * @sa GetSn_TX_WR()
 */
#define setSn_TX_WR(sn, txwr) { \
		WIZCHIP_WRITE(Sn_TX_WR(sn),   (uint8_t)(txwr>>8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_TX_WR(sn),1), (uint8_t) txwr); \
		}

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_TX_WR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_TX_WR.
 * @sa setSn_TX_WR()
 */
#define getSn_TX_WR(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_TX_WR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_WR(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_RX_RSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_RX_RSR.
 */
uint16_t getSn_RX_RSR(uint8_t sn);

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_RX_RD register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)rxrd Value to set @ref Sn_RX_RD
 * @sa getSn_RX_RD()
 */
#define setSn_RX_RD(sn, rxrd) { \
		WIZCHIP_WRITE(Sn_RX_RD(sn),   (uint8_t)(rxrd>>8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_RX_RD(sn),1), (uint8_t) rxrd); \
	}

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_RX_RD register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @regurn uint16_t. Value of @ref Sn_RX_RD.
 * @sa setSn_RX_RD()
 */
#define getSn_RX_RD(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_RX_RD(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RD(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_RX_WR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)rxwr Value to set \ref Sn_RX_WR
 * @sa getSn_RX_WR()
 */
#define setSn_RX_WR(sn, rxwr) { \
		WIZCHIP_WRITE(Sn_RX_WR(sn),   (uint8_t)(rxwr>>8)); \
	   WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_RX_WR(sn),1), (uint8_t) rxwr); \
	}


/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_RX_WR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_RX_WR.
 */
#define getSn_RX_WR(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_RX_WR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_WR(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Set @ref Sn_FRAG register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)frag Value to set \ref Sn_FRAG
 * @sa getSn_FRAG()
 */
#define setSn_FRAG(sn, frag) { \
		WIZCHIP_WRITE(Sn_FRAG(sn),  (uint8_t)(frag >>8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_FRAG(sn),1), (uint8_t) frag); \
	}

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get @ref Sn_FRAG register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_FRAG.
 * @sa setSn_FRAG()
 */
#define getSn_FRAG(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_FRAG(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_FRAG(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get the max RX buffer size of socket sn
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Max buffer size
 */
#define getSn_RxMAX(sn) \
		((uint16_t)(1 << getSn_RXMEM_SIZE(sn)) << 10)


/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get the max TX buffer size of socket sn
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Max buffer size
 */
#define getSn_TxMAX(sn) \
		((uint16_t)(1 << getSn_TXMEM_SIZE(sn)) << 10)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get the mask of socket sn RX buffer.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Mask value
 */
#define getSn_RxMASK(sn) \
		(getSn_RxMAX(sn) - 1)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get the mask of socket sn TX buffer
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Mask value
 */
#define getSn_TxMASK(sn) \
		(getSn_TxMAX(sn) - 1)

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get the base address of socket sn RX buffer.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of Socket n RX buffer base address.
 */
uint32_t getSn_RxBASE(uint8_t sn);

/**
 * @ingroup Socket_register_access_function_W5100
 * @brief Get the base address of socket sn TX buffer.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of Socket n TX buffer base address.
 */
uint32_t getSn_TxBASE(uint8_t sn);

/////////////////////////////////////
// Sn_TXBUF & Sn_RXBUF IO function //
/////////////////////////////////////
/**
 * @ingroup Basic_IO_function_W5100
 * @brief It copies data to internal TX memory
 *
 * @details This function reads the Tx write pointer register and after that,
 * it copies the <i>wizdata(pointer buffer)</i> of the length of <i>len(variable)</i> bytes to internal TX memory
 * and updates the Tx write pointer register.
 * This function is being called by send() and sendto() function also.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param wizdata Pointer buffer to write data
 * @param len Data length
 * @sa wiz_recv_data()
 */
void wiz_send_data(uint8_t sn, uint8_t *wizdata, uint16_t len);

/**
 * @ingroup Basic_IO_function_W5100
 * @brief It copies data to your buffer from internal RX memory
 *
 * @details This function read the Rx read pointer register and after that,
 * it copies the received data from internal RX memory
 * to <i>wizdata(pointer variable)</i> of the length of <i>len(variable)</i> bytes.
 * This function is being called by recv() also.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param wizdata Pointer buffer to read data
 * @param len Data length
 * @sa wiz_send_data()
 */
void wiz_recv_data(uint8_t sn, uint8_t *wizdata, uint16_t len);

/**
 * @ingroup Basic_IO_function_W5100
 * @brief It discard the received data in RX memory.
 * @details It discards the data of the length of <i>len(variable)</i> bytes in internal RX memory.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param len Data length
 */
void wiz_recv_ignore(uint8_t sn, uint16_t len);

/// @cond DOXY_APPLY_CODE
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif //_W5100_H_



