//* ****************************************************************************
//! \file w5100S.h
//! \brief W5100S HAL Header File.
//! \version 1.0.0
//! \date 2018/03/29
//! \par  Revision history
//!       <2018/03/29> 1st Release
//! \author Peter
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


#ifndef	_w5105_H_
#define	_w5105_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../wizchip_conf.h"

/// \cond DOXY_APPLY_CODE
#if   (_WIZCHIP_ == 5105)
/// \endcond

#define _WIZCHIP_SN_BASE_  (0x0400)
#define _WIZCHIP_SN_SIZE_  (0x0100)
#define _WIZCHIP_IO_TXBUF_ (0x4000) /* Internal Tx buffer address of the iinchip */
#define _WIZCHIP_IO_RXBUF_ (0x6000) /* Internal Rx buffer address of the iinchip */


#define WIZCHIP_CREG_BLOCK      	      0x00   ///< Common register block
#define WIZCHIP_SREG_BLOCK(N)    	   (_WIZCHIP_SN_BASE_+ _WIZCHIP_SN_SIZE_*N) ///< Socket N register block

#define WIZCHIP_OFFSET_INC(ADDR, N)    (ADDR + N) ///< Increase offset address

#if (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_DIR_)
   #define _W5100S_IO_BASE_     _WIZCHIP_IO_BASE_
#elif (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_)
	#define IDM_OR             ((_WIZCHIP_IO_BASE  + 0x0000))
	#define IDM_AR0            ((_WIZCHIP_IO_BASE_ + 0x0001))
	#define IDM_AR1            ((_WIZCHIP_IO_BASE_ + 0x0002))
	#define IDM_DR             ((_WIZCHIP_IO_BASE_ + 0x0003))
	#define _W5100S_IO_BASE_    0x0000
#elif (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_)
   #define _W5100S_IO_BASE_    0x0000
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
 * @defgroup W5100S W5100S
 * @brief WHIZCHIP register defines and I/O functions of @b W5100S.
 *
 * - @ref WIZCHIP_register_W5100S: @ref Common_register_group_W5100S and @ref Socket_register_group_W5100S
 * - @ref WIZCHIP_IO_Functions_W5100S: @ref Basic_IO_function_W5100S, @ref Common_register_access_function_W5100S and @ref Special_function_W5100S
 */

 /**
 * @defgroup WIZCHIP_register_W5100S WIZCHIP register
 * @ingroup W5100S
 * @brief WIZCHIP register defines register group of <b> W5100S </b>.
 *
 * - \ref Common_register_group_W5100S : Common register group W5100S
 * - \ref Socket_register_group_W5100S : \c SOCKET n register group W5100S
 */


/**
 * @defgroup WIZCHIP_IO_Functions_W5100S WIZCHIP I/O functions
 * @ingroup W5100S
 * @brief This supports the basic I/O functions for \ref WIZCHIP_register_W5100S.
 *
 * - <b> Basic I/O function </b> \n
 *   WIZCHIP_READ(), WIZCHIP_WRITE(), WIZCHIP_READ_BUF(), WIZCHIP_WRITE_BUF(), wiz_recv_data(), wiz_recv_ignore(), wiz_send_data() \n\n
 *
 * - \ref Common_register_group_W5100S <b>access functions</b> \n
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
 * - \ref Socket_register_group_W5100S <b>access functions</b> \n
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
 * @defgroup Common_register_group_W5100S Common register
 * @ingroup WIZCHIP_register_W5100S
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
 * @defgroup Socket_register_group_W5100S Socket register
 * @ingroup WIZCHIP_register_W5100S
 * @brief Socket register group\n
 * Socket register configures and control SOCKETn which is necessary to data communication.
 * @details
 * @sa Sn_MR, Sn_CR, Sn_IR : SOCKETn Control
 * @sa Sn_SR, Sn_PORT, Sn_DHAR, Sn_DIPR, Sn_DPORT : SOCKETn Information
 * @sa Sn_MSSR, Sn_TOS, Sn_TTL, Sn_FRAGR : Internet protocol.
 * @sa Sn_RXMEM_SIZE, Sn_TXMEM_SIZE, Sn_TX_FSR, Sn_TX_RD, Sn_TX_WR, Sn_RX_RSR, Sn_RX_RD, Sn_RX_WR : Data communication
 */

 /**
 * @defgroup Basic_IO_function_W5100S Basic I/O function
 * @ingroup WIZCHIP_IO_Functions_W5100S
 * @brief These are basic input/output functions to read values from register or write values to register.
 */

/**
 * @defgroup Common_register_access_function_W5100S Common register access functions
 * @ingroup WIZCHIP_IO_Functions_W5100S
 * @brief These are functions to access <b>common registers</b>.
 */

/**
 * @defgroup Socket_register_access_function_W5100S Socket register access functions
 * @ingroup WIZCHIP_IO_Functions_W5100S
 * @brief These are functions to access <b>socket registers</b>.
 */

 /**
 * @defgroup Special_function_W5100S Special functions
 * @ingroup WIZCHIP_IO_Functions_W5100S
 * @brief These are special functions to access to the PHY
 */

 //-----------------------------------------------------------------------------------

//----------------------------- W5100S Common Registers IOMAP -----------------------------
/**
 * @ingroup Common_register_group_W5100S
 * @brief Mode Register address(R/W)\n
 * \ref MR is used for S/W reset, ping block mode, PPPoE mode and etc.
 * @details Each bit of \ref MR defined as follows.
 * <table>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>RST</td> <td>Reserved</td> <td>WOL</td> <td>PB</td> <td>PPPoE</td> <td>Reserved</td> <td>AI</td> <td>IND</td> </tr>
 * </table>
 * - \ref MR_RST		 	: Reset
 * - \ref MR_PB         	: Ping block
 * - \ref MR_PPPOE      	: PPPoE mode
 * - \ref MR_AI         	: Address Auto-Increment in Indirect Bus Interface
 * - \ref MR_IND         	: Indirect Bus Interface mode
 */
#if _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_
   #define MR					(_WIZCHIP_IO_BASE_ + (0x0000))  // Mode
#else
   #define MR					(_W5100S_IO_BASE_ + (0x0000))  // Mode
#endif

/**
 * @ingroup Common_register_group_W5100S
 * @brief Gateway IP Register address(R/W)
 * @details \ref GAR configures the default gateway address.
 */
#define GAR     			(_W5100S_IO_BASE_ + (0x0001))  // GW Address

/**
 * @ingroup Common_register_group_W5100S
 * @brief Subnet mask Register address(R/W)
 * @details \ref SUBR configures the subnet mask address.
 */
#define SUBR    			(_W5100S_IO_BASE_ + (0x0005)) // SN Mask Address

/**
 * @ingroup Common_register_group_W5100S
 * @brief Source MAC Register address(R/W)
 * @details \ref SHAR configures the source hardware address.
 */
#define SHAR    			(_W5100S_IO_BASE_ + (0x0009)) // Source Hardware Address

/**
 * @ingroup Common_register_group_W5100S
 * @brief Source IP Register address(R/W)
 * @details \ref SIPR configures the source IP address.
 */
#define SIPR    			(_W5100S_IO_BASE_ + (0x000F)) // Source IP Address

// Reserved					(_W5100S_IO_BASE_ + (0x0013))
// Reserved					(_W5100S_IO_BASE_ + (0x0014))

/**
 * @ingroup Common_register_group_W5100S
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
#define IR					(_W5100S_IO_BASE_ + (0x0015)) // Interrupt

/**
 * @ingroup Common_register_group_W5100S
 * @brief Socket Interrupt Mask Register(R/W)
 * @details Each bit of \ref _IMR_ corresponds to each bit of \ref IR.
 * When a bit of \ref _IMR_ is and the corresponding bit of \ref IR is set, Interrupt will be issued.
 */
#define _IMR_    			(_W5100S_IO_BASE_ + (0x0016)) // Socket Interrupt Mask

/**
 * @ingroup Common_register_group_W5100S
 * @brief Timeout register address( 1 is 100us )(R/W)
 * @details \ref _RTR_ configures the retransmission timeout period. The unit of timeout period is 100us and the default of \ref _RTR_ is x07D0or 000
 * And so the default timeout period is 200ms(100us X 2000). During the time configured by \ref _RTR_, W5100S waits for the peer response
 * to the packet that is transmitted by \ref Sn_CR (CONNECT, DISCON, CLOSE, SEND, SEND_MAC, SEND_KEEP command).
 * If the peer does not respond within the \ref _RTR_ time, W5100S retransmits the packet or issues timeout.
 */
#define _RTR_     			(_W5100S_IO_BASE_ + (0x0017)) // Retry Time

/**
 * @ingroup Common_register_group_W5100S
 * @brief Retry count register(R/W)
 * @details \ref _RCR_ configures the number of time of retransmission.
 * When retransmission occurs as many as ref _RCR_+1 Timeout interrupt is issued (\ref Sn_IR_TIMEOUT = '1').
 */
#define _RCR_				(_W5100S_IO_BASE_ + (0x0019)) // Retry Count

/**
 * @ingroup Common_register_group_W5100S
 * @brief Receive Memory Size Register
 * @details \ref RMSR register configures RX bufffer Size of the SOCKET
 * The sum of the RX buffers can not exceed 8kB.
 * <table>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>S3-1</td> <td>S3-0</td> <td>S2-1</td> <td>S2-0</td> <td>S1-1</td> <td>S1-0</td> <td>S0-1</td> <td>S0-0</td> </tr>
 * </table>
 * <table>
 * 		<tr> <td>Memory Size</td><td>Sn-1</td><td>Sn-0</td> </tr>
 * 		<tr> <td>1KB</td><td>0</td><td>0</td> </tr>
 * 		<tr> <td>2KB</td><td>0</td><td>1</td> </tr>
 * 		<tr> <td>4KB</td><td>1</td><td>0</td> </tr>
 * 		<tr> <td>8KB</td><td>1</td><td>1</td> </tr>
 * </table>
 */
#define RMSR				(_W5100S_IO_BASE_ + (0x001A)) // Receive Memory Size

/**
 * @ingroup Common_register_group_W5100S
 * @brief Transmit Memory Size Register
 * @details \ref TMSR register configures TX bufffer Size of the SOCKET
 * The sum of the TX buffers can not exceed 8kB.
 * <table>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>S3-1</td> <td>S3-0</td> <td>S2-1</td> <td>S2-0</td> <td>S1-1</td> <td>S1-0</td> <td>S0-1</td> <td>S0-0</td> </tr>
 * </table>
 * <table>
 * 		<tr> <td>Memory Size</td><td>Sn-1</td><td>Sn-0</td> </tr>
 * 		<tr> <td>1KB</td><td>0</td><td>0</td> </tr>
 * 		<tr> <td>2KB</td><td>0</td><td>1</td> </tr>
 * 		<tr> <td>4KB</td><td>1</td><td>0</td> </tr>
 * 		<tr> <td>8KB</td><td>1</td><td>1</td> </tr>
 * </table>
 */
#define TMSR				(_W5100S_IO_BASE_ + (0x001B)) // Transmit Memory Size

/**
 * @ingroup Common_register_group_W5100S
 * @brief Interrupt register 2
 * @details \ref IR2 indicates the interrupt status.
 * Each bit of IR2 will be still until the bit will be written to by the host.
 * <table>
 * 		<tr>  <td>7:1</td> <td>0</td>   </tr>
 * 		<tr>  <td>Reserved</td> <td>WOL</td> </tr>
 * </table>
 * - \ref IR2_WOL : WOL MAGIC PACKET Interrupt Mask
 */
#define IR2					(_W5100S_IO_BASE_ + (0x0020))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Interrupt mask register 2
 * @details \ref IMR2 Each bit of IMR2 corresponds to each bit of IR2.
 * When a bit of IMR2 is and the corresponding bit of IR2 is set, Interrupt will be issued.
 */
#define IMR2				(_W5100S_IO_BASE_ + (0x0021))


/**
 * @ingroup Common_register_group_W5100S
 * @brief PPP LCP Request Timer register  in PPPoE mode(R)
 * @details \ref PTIMER configures the time for sending LCP echo request. The unit of time is 25ms.
 */
#define PTIMER				(_W5100S_IO_BASE_ + (0x0028)) // PPP LCP RequestTimer

/**
 * @ingroup Common_register_group_W5100S
 * @brief PPP LCP Magic number register  in PPPoE mode(R)
 * @details \ref PMAGIC configures the 4bytes magic number to be used in LCP negotiation.
 */
#define PMAGIC				(_W5100S_IO_BASE_ + (0x0029)) // PPP LCP Magic number

/**
 * @ingroup Common_register_group_W5100S
 * @brief Unreachable IP address register
 * @details \ref
 */
#define UIPR				(_W5100S_IO_BASE_ + (0x002A))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Unreachable Port register
 * @details \ref
 */
#define UPORTR				(_W5100S_IO_BASE_  + (0x002E))

/* register for W5100S only */

/*------------------------------------------ Common registers ------------------------------------------*/

/**
 * @ingroup Common_register_group_W5100S
 * @brief MR2 Mode register 2
 * @details \reg
 */
#define MR2					(_W5100S_IO_BASE_ + (0x0030))


/**
 * @ingroup Common_register_group_W5100S
 * @brief Destination Hardware address in PPPoE
 * @details \reg
 */
#define PHAR				(_W5100S_IO_BASE_ + (0x0032))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Session ID in PPPoE
 * @details \reg
 */
#define PSIDR				(_W5100S_IO_BASE_ + (0x0038))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Maximum receive Unit in PPPoE
 * @details \reg
 */
#define PMRUR				(_W5100S_IO_BASE_ + (0x003A))


/*------------------------------------------ PHY registers ------------------------------------------*/

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY status register
 * @details \reg
 */
#define PHYSR				(_W5100S_IO_BASE_ + (0x003C))

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY status register(hidden)
 * @details \reg
 */
#define PHYSR1				(_W5100S_IO_BASE_ + (0x003D))

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY Address value
 * @details \reg
 */
#define PHYAR				(_W5100S_IO_BASE_ + (0x003E))

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY Register address
 * @details \reg
 */
#define PHYRAR				(_W5100S_IO_BASE_ + (0x003F))

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY Data input register
 * @details \reg
 */
#define PHYDIR				(_W5100S_IO_BASE_ + (0x0040))

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY data output register
 * @details \reg
 */
#define PHYDOR				(_W5100S_IO_BASE_ + (0x0042))

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY Action register
 * @details \reg
 */
#define PHYACR				(_W5100S_IO_BASE_ + (0x0044))

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY Division register
 * @details \reg
 */
#define PHYDIVR				(_W5100S_IO_BASE_ + (0x0045))

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY Control register 0
 * @details \reg
 */
#define PHYCR0					(_W5100S_IO_BASE_ + (0x0046))
/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY Control register 1
 * @details \reg
 */
#define PHYCR1					(_W5100S_IO_BASE_ + (0x0047))

/*------------------------------------------ Socket Less registers ------------------------------------------*/

/**
 * @ingroup Common_register_group_W5100S
 * @brief Socket-less control register
 * @details \reg
 */
#define SLCR				(_W5100S_IO_BASE_ + (0x004C))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Socket-less retry time register
 * @details \reg
 */
#define SLRTR				(_W5100S_IO_BASE_ + (0x004D))


/**
 * @ingroup Common_register_group_W5100S
 * @brief Socket-less retry count register
 * @details \reg
 */
#define SLRCR				(_W5100S_IO_BASE_ + (0x004F))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Socket-less peer IP address register
 * @details \reg
 */
#define SLPIPR				(_W5100S_IO_BASE_ + (0x0050))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Socket-less peer hardware address register
 * @details \reg
 */
#define SLPHAR				(_W5100S_IO_BASE_ + (0x0054))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Ping sequence number register
 * @details \reg
 */
#define PINGSEQR				(_W5100S_IO_BASE_ + (0x005A))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Ping ID register
 * @details \reg
 */
#define PINGIDR				(_W5100S_IO_BASE_ + (0x005C))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Socket-less interrupt mask register
 * @details \reg
 */
#define SLIMR				(_W5100S_IO_BASE_ + (0x005E))

/**
 * @ingroup Common_register_group_W5100S
 * @brief  Socket-less interrupt register
 * @details \reg
 */
#define SLIR				(_W5100S_IO_BASE_ + (0x005F))

/**
 * @ingroup Common_register_group_W5100S
 * @brief DBGOUT(hidden)
 * @details \reg
 */
#define DBGOUT				(_W5100S_IO_BASE_ + (0x0060))

/**
 * @ingroup Common_register_group_W5100S
 * @brief NICMAXCOLR(hidden)
 * @details \reg
 */
#define NICMAXCOLR			(_W5100S_IO_BASE_ + (0x0063))
/*------------------------------------------ CFG registers ------------------------------------------*/

/**
 * @ingroup Common_register_group_W5100S
 * @brief Chip Configuration locking register
 * @details \reg
 */
#define CHIPLCKR				(_W5100S_IO_BASE_ + (0x0070))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Network Configuration locking register
 * @details \reg
 */
#define NETLCKR				(_W5100S_IO_BASE_ + (0x0071))

/**
 * @ingroup Common_register_group_W5100S
 * @brief PHY Configuration locking register
 * @details \reg
 */
#define PHYLCKR				(_W5100S_IO_BASE_ + (0x0072))

/**
 * @ingroup Common_register_group_W5100S
 * @brief version register
 * @details \reg
 */
#define VERR				(_W5100S_IO_BASE_ + (0x0080))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Core 100us Counter register
 * @details \reg
 */
#define TCNTR				(_W5100S_IO_BASE_ + (0x0082))

/**
 * @ingroup Common_register_group_W5100S
 * @brief Core 100us Counter clear register
 * @details \reg
 */
#define TCNTCLKR			(_W5100S_IO_BASE_ + (0x0088))

//----------------------------- W5100S Socket Registers -----------------------------
#define WIZCHIP_SREG_ADDR(sn, addr) (WIZCHIP_SREG_BLOCK(sn) + (addr))
//--------------------------- For Backward Compatibility ---------------------------

/**
 * @ingroup Socket_register_group_W5100S
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
#define Sn_MR(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0000)) // socket Mode register

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Socket command register(R/W)
 * @details This is used to set the command for Socket n such as OPEN, CLOSE, CONNECT, LISTEN, SEND, and RECEIVE.\n
 * After W5100S accepts the command, the \ref Sn_CR register is automatically cleared to 0x00.
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
#define Sn_CR(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0001)) // channel Sn_CR register

/**
 * @ingroup Socket_register_group_W5100S
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
#define Sn_IR(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0002)) // channel interrupt register

/**
 * @ingroup Socket_register_group_W5100S
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
#define Sn_SR(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0003)) // channel status register

/**
 * @ingroup Socket_register_group_W5100S
 * @brief source port register(R/W)
 * @details \ref Sn_PORT configures the source port number of Socket n.
 * It is valid when Socket n is used in TCP/UDP mode. It should be set before OPEN command is ordered.
*/
#define Sn_PORT(sn)		(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0004)) // source port register

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Peer MAC register address(R/W)
 * @details \ref Sn_DHAR configures the destination hardware address of Socket n when using SEND_MAC command in UDP mode or
 * it indicates that it is acquired in ARP-process by CONNECT/SEND command.
 */
#define Sn_DHAR(sn)     (_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0006)) // Peer MAC register address

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Peer IP register address(R/W)
 * @details \ref Sn_DIPR configures or indicates the destination IP address of Socket n. It is valid when Socket n is used in TCP/UDP mode.
 * In TCP client mode, it configures an IP address of TCP server before CONNECT command.
 * In TCP server mode, it indicates an IP address of TCP client after successfully establishing connection.
 * In UDP mode, it configures an IP address of peer to be received the UDP packet by SEND or SEND_MAC command.
 */
#define Sn_DIPR(sn)		(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x000C)) // Peer IP register address

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Peer port register address(R/W)
 * @details \ref Sn_DPORT configures or indicates the destination port number of Socket n. It is valid when Socket n is used in TCP/UDP mode.
 * In TCP clientmode, it configures the listen port number of TCP server before CONNECT command.
 * In TCP Servermode, it indicates the port number of TCP client after successfully establishing connection.
 * In UDP mode, it configures the port number of peer to be transmitted the UDP packet by SEND/SEND_MAC command.
 */
#define Sn_DPORT(sn)    (_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0010)) // Peer port register address

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Maximum Segment Size(Sn_MSSR0) register address(R/W)
 * @details \ref Sn_MSSR configures or indicates the MTU(Maximum Transfer Unit) of Socket n.
 */
#define Sn_MSSR(sn)	   (_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0012)) // Maximum Segment Size(Sn_MSSR0) register address

/**
 * @ingroup Socket_register_group_W5100S
 * @brief IP Protocol(PROTO) Register(R/W)
 * @details \ref Sn_PROTO that sets the protocol number field of the IP header at the IP layer. It is
 * valid only in IPRAW mode, and ignored in other modes.
 */
#define Sn_PROTO(sn)	   (_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0014)) // Protocol of IP Header field register in IP raw mode

/**
 * @ingroup Socket_register_group_W5100S
 * @brief IP Type of Service(TOS) Register(R/W)
 * @details \ref Sn_TOS configures the TOS(Type Of Service field in IP Header) of Socket n.
 * It is set before OPEN command.
 */
#define Sn_TOS(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + 0x0015) // IP Type of Service(TOS) Register

/**
 * @ingroup Socket_register_group_W5100S
 * @brief IP Time to live(TTL) Register(R/W)
 * @details \ref Sn_TTL configures the TTL(Time To Live field in IP header) of Socket n.
 * It is set before OPEN command.
 */
#define Sn_TTL(sn)		(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0016)) // IP Time to live(TTL) Register

// Reserved					(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0017))
// Reserved					(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0018))
// Reserved					(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0019))
// Reserved					(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001A))
// Reserved					(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001B))
// Reserved					(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001C))
// Reserved					(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001D))



/**
 * @ingroup Socket_register_group_W5100S
 * @brief Receive memory size register(R/W)
 * @details @ref Sn_RXBUF_SIZE configures the RX buffer block size of Socket n.
 * Socket n RX Buffer Block size can be configured with 1,2,4 and 8Kbytes.
 * If a different size is configured, the data cannot be normally received from a peer.
 * Although Socket n RX Buffer Block size is initially configured to 2Kbytes,
 * user can re-configure its size using @ref Sn_RXBUF_SIZE. The total sum of @ref Sn_RXBUF_SIZE can not be exceed 8Kbytes.
 * When exceeded, the data reception error is occurred.
 */
#define Sn_RXBUF_SIZE(sn)   (_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001E))

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Transmit memory size register(R/W)
 * @details @ref Sn_TXBUF_SIZE configures the TX buffer block size of Socket n. Socket n TX Buffer Block size can be configured with 1,2,4 and 8Kbytes.
 * If a different size is configured, the data cannot be normally transmitted to a peer.
 * Although Socket n TX Buffer Block size is initially configured to 2Kbytes,
 * user can be re-configure its size using @ref Sn_TXBUF_SIZE. The total sum of @ref Sn_TXBUF_SIZE can not be exceed 8Kbytes.
 * When exceeded, the data transmission error is occurred.
 */
#define Sn_TXBUF_SIZE(sn)   (_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x001F))


/**
 * @ingroup Socket_register_group_W5100S
 * @brief Transmit free memory size register(R)
 * @details \ref Sn_TX_FSR indicates the free size of Socket n TX Buffer Block. It is initialized to the configured size by \ref Sn_TXMEM_SIZE.
 * Data bigger than \ref Sn_TX_FSR should not be saved in the Socket n TX Buffer because the bigger data overwrites the previous saved data not yet sent.
 * Therefore, check before saving the data to the Socket n TX Buffer, and if data is equal or smaller than its checked size,
 * transmit the data with SEND/SEND_MAC command after saving the data in Socket n TX buffer. But, if data is bigger than its checked size,
 * transmit the data after dividing into the checked size and saving in the Socket n TX buffer.
 */
#define Sn_TX_FSR(sn)	(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0020)) // Transmit free memory size register

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Transmit memory read pointer register address(R)
 * @details \ref Sn_TX_RD is initialized by OPEN command. However, if Sn_MR(P[3:0]) is TCP mode(001), it is re-initialized while connecting with TCP.
 * After its initialization, it is auto-increased by SEND command.
 * SEND command transmits the saved data from the current \ref Sn_TX_RD to the \ref Sn_TX_WR in the Socket n TX Buffer.
 * After transmitting the saved data, the SEND command increases the \ref Sn_TX_RD as same as the \ref Sn_TX_WR.
 * If its increment value exceeds the maximum value 0xFFFF, (greater than 0x10000 and the carry bit occurs),
 * then the carry bit is ignored and will automatically update with the lower 16bits value.
 */
#define Sn_TX_RD(sn)		(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0022)) // Transmit memory read pointer register address

/**
 * @ingroup Socket_register_group_W5100S
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
#define Sn_TX_WR(sn)		(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0024)) // Transmit memory write pointer register address

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Received data size register(R)
 * @details \ref Sn_RX_RSR indicates the data size received and saved in Socket n RX Buffer.
 * \ref Sn_RX_RSR does not exceed the \ref Sn_RXMEM_SIZE and is calculated as the difference between
 * Socket n RX Write Pointer (\ref Sn_RX_WR)and Socket n RX Read Pointer (\ref Sn_RX_RD)
 */
#define Sn_RX_RSR(sn)	(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0026)) // Received data size register

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Read point of Receive memory(R/W)
 * @details \ref Sn_RX_RD is initialized by OPEN command. Make sure to be read or updated as follows.\n
 * 1. Read the starting save address of the received data.\n
 * 2. Read data from the starting address of Socket n RX Buffer.\n
 * 3. After reading the received data, Update \ref Sn_RX_RD to the increased value as many as the reading size.
 * If the increment value exceeds the maximum value 0xFFFF, that is, is greater than 0x10000 and the carry bit occurs,
 * update with the lower 16bits value ignored the carry bit.\n
 * 4. Order RECV command is for notifying the updated \ref Sn_RX_RD to W5100S.
 */
#define Sn_RX_RD(sn)		(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0028)) // Read point of Receive memory

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Write point of Receive memory(R)
 * @details \ref Sn_RX_WR is initialized by OPEN command and it is auto-increased by the data reception.
 * If the increased value exceeds the maximum value 0xFFFF, (greater than 0x10000 and the carry bit occurs),
 * then the carry bit is ignored and will automatically update with the lower 16bits value.
 */
#define Sn_RX_WR(sn)		(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x002A)) // Write point of Receive memory


//todo
/**
 * @ingroup Socket_register_group_W5100S
 * @brief Socket interrupt mask register
 * @details Register address to configure the interrupt mask of the socket
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 *
 */
#define Sn_IMR(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x002C))

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Socket fragment field register
 * @details Register to configure the Fragment field of IP Header
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 */
#define Sn_FRAGR(sn)		(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x002D)) // and +1

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Socket Mode register 2
 * @details Register to set mode 2
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 */
#define Sn_MR2(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x002F))

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Socket n Keep Alive Timer Register
 * @details Register to set the transmission period of keep alive packet.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 */
#define Sn_KPALVTR(sn)		(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0030))

/** todo delete
 * @ingroup Socket_register_group_W5100S
 * @brief Socket n Timer Status Register
 * @details
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 */
//#define Sn_TSR(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0031))

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Socket n Retry Time-value Register
 * @details Register to set the retry time value
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 */
#define Sn_RTR(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0032))

/**
 * @ingroup Socket_register_group_W5100S
 * @brief Socket n Retry Count-value Register
 * @details Register to set the retry count value
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 */
#define Sn_RCR(sn)			(_W5100S_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (0x0034))


/*----------------------------- W5100S Register values  -----------------------------*/

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
 * At the Indirect Bus Interface mode, if this bit is set as the address will
 * be automatically increased by 1 whenever read and write are performed.
 */
#define MR_AI				0x02 ///< auto-increment in indirect mode

/**
 * @brief Indirect Bus Interface mode
 * @details 0 : Disable Indirect bus Interface mode \n
 * 1 : Enable Indirect bus Interface mode \n
 * If this bit is set as Indirect Bus Interface mode is set.
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

/**
 * @brief Socket interrupt bit
 * @details Indicates whether each socket interrupt has occured.
 */
#define IR_SOCK(sn)		(0x01 << sn)	///< check socket interrupt

/**
 * @brief IP conflict interrupt mask bit
 * @details If this bit is set, IP conflict interrupt is enabled.
 */
#define IMR_CONFLICT		0x80

/**
 * @brief Destination port unreachable interrupt mask bit
 * @details If this bit is set, destination port unreachable interrupt is enabled.
 */
#define IMR_UNREACH			0x40

/**
 * @brief PADT/LCPT interrupt mask bit(PPPoE)
 * @details If this bit is set, PADT/LCPT interrupt is enabled.
 */
#define IMR_PPPoE			0x20

/**
 * @brief Socket interrupt mask bit
 * @details If this bit is set, each socket interrupt is enabled.
 */
#define IMR_SOCK(sn)		(0x01 << sn)

/**
 * @brief Socket-less command register bit
 * @details ARP command
 */
#define SLCMD_ARP		(1<<1)

/**
 * @brief Socket-less command register bit
 * @details ARP command
 */
#define SLCMD_PING		(1<<0)

/**
 * @brief Socket-less command interrupt and interrupt mask register bit
 * @details Request command time out interrupt and interrupt mask
 */
#define SLIR_TIMEOUT		(1<<2)

/**
* @brief Socket less command interrupt and interrupt mask register bit
* @details Socket less command ARP interrupt and interrupt mask
*/
#define SLIR_ARP (1<<1)

/**
* @brief Socket less command interrupt and interrupt mask register bit
* @details Socket less command PING interrupt and interruptmask
*/
#define SLIR_PING (1<<0)



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
 * When set as  W5100S can only receive broadcasting packet or packet sent to itself.
 * When this bit is  W5100S can receive all packets on Ethernet.
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
 * @brief
 * @details
 */
#define Sn_CR_IGMP_JOIN			0x23

/**
 * @brief
 * @details
 */
#define Sn_CR_IGMP_LEAVE		0x24


/* Sn_IR values */

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



/*----------------------------- W5100S !!Only!! Register values  -----------------------------*/

//todo
/* MODE2 register values */

/**
 * @brief	Clock select bit
 * @details	With this bit, system clock can be selected to be 25Mhz or 100Mhz
 * 1: 25Mhz
 * 0: 100Mhz (default)
 */
#define MR2_CLKSEL				(1<<7)

/**
 * @brief	Interrupt pin enable bit
 * @details	This bit enables interrupt.
 * 1: Enable interrupt
 * 0: Disable interrupt
 */
#define MR2_G_IEN				(1<<6)


/**
 * @brief	No TCP Reset Packet send
 * @details This bit prevents sending reset packet.
 * 1: Block TCP reset packet send
 * 0: TCP Reset packet send
 */
#define MR2_NOTCPRST			(1<<5)

/**
 * @brief	Unreachable Packet Send Block bit
 * @details	This bit prevents sending Destination Port Unreachable Packet.
 * 1: Block Destination Port Unreachable Packet Send
 * 0: Destination Port Unreachable Packet Send
 */
#define MR2_UDPURB				(1<<4)

/**
 * @brief Wake On LAN
 * @details This bit enables WOL packet to be received.
 * 1: WOL packet can be received.
 * 0: WOL packet cannot be received.
 */
#define MR2_WOL					(1<<3)

/**todo
 * @brief MACRAW No Size Check
 * @details
 */
#define MR2_MNOSCHK				(1<<2)

/**
 * @brief	UDP force ARP
 * @details This bit can enables to force ARP for each send command.
 * 1: UDP Force ARP Enable
 * 0: UDP Force ARP Disable.
 *
 */
#define MR2_UDPFARP				(1<<1)

/**todo
 * @brief	Skip SRC Hardware Address
 * @details	This bit can enables to receive without checking the hardware address of the peer.
 * 1:
 */
#define MR2_SSRCHA				(1<<0)



/* Common interrupt register 2 values */

/**todo
 * @brief magic packet
 * @details
 */
#define IR2_MGC				(1<<1)

/**todo
 * @brief Magic packet interrupt mask bit
 * @details If this bit is set, each socket interrupt is enabled.
 */
#define IMR2_MGC			(1<<1)

/**todo
 * @brief
 * @details
 */
//#define IR2_MGD				(1<<1) /* Reserved */


/* PHY status register 0 values */

/**todo
 * @brief Ethernet CABLE OFF Signal
 * @details
 */
#define PHYSR_CABOFF				(1<<7)

/**todo
 * @brief
 * @details
 */
#define PHYSR_MD2				(1<<5)

/**todo
 * @brief
 * @details
 */
#define PHYSR_MD1				(1<<4)

/**todo
 * @brief
 * @details
 */
#define PHYSR_MD0				(1<<3)

/**todo
 * @brief
 * @details
 */
#define PHYSR_DUP				(1<<2)

/**todo
 * @brief
 * @details
 */
#define PHYSR_SPD				(1<<1)

/**todo
 * @brief LINKDONE register
 * @details If 1 Linked successfully, if 0 no link
 */
#define PHYSR_LNK				(1<<0)


/* PHY status register 10 values */

/**
 * @brieftodo
 * @details
 */
#define PHYSR1_RXPG				(1<<2)

/**
 * @brieftodo
 * @details
 */
#define PHYSR1_LPI				(1<<1)

/**
 * @brieftodo
 * @details
 */
#define PHYSR1_CLDN				(1<<0)

#define PHYCR_AUTONEGO_ENABLE	(0<<2)
#define PHYCR_AUTONEGO_DISABLE	(1<<2)

#define PHYCR_SPD_10			(1<<1)
#define PHYCR_SPD_100			(0<<1)

#define PHYCR_HALF_DUP			(1<<0)
#define PHYCR_FULL_DUP			(0<<0)

#define PHYCR1_RST				(0<<0)

#define	PHYCR1_PWDN_ENABLE		(1<<5)
#define	PHYCR1_PWDN_DISABLE		(0<<5)


/* Socket n MODE register 2 values */

/**
 * @brief Broadcast Blocking bit in MACRAW mode
 * @details In MACRAW mode, this bit is set to ????to block the broadcast packet.
 */
#define Sn_MR2_MBBLK  			(1<<6)

/**
 * @brief Multicast Blocking bit in MACRAW mode
 * @details In MACRAW mode, this bit is set to ????to block the multicast packet.
 */
#define Sn_MR2_MMBLK  			(1<<5)

/**
 * @brief IPv6 packet Blocking bit in MACRAW mode
 * @details In MACRAW mode, this bit is set to ????to block the IPv6 packet.
 */
#define Sn_MR2_IPV6BLK  		(1<<4)


/**
 * @brief Broadcast Blocking bit in UDP mode
 * @details In UDP mode, this bit is set to ????to block the broadcast packet.
 */
#define Sn_MR2_UBBLK			(1<<1)


/**
 * @brief TCP Force PSH bit
 * @details When the SOCKET transmits data in TCP mode, PSH Flag is set to all packets.
 */
#define Sn_MR2_FPSH			Sn_MR2_UBBLK

/**
 * @brief Unicast Blocking bit in UDP mode
 * @details In UDP mode, this bit is set to ????to block the Unicast packet.
 */
#define Sn_MR2_UUBLK			(1<<0)

/*----------------------------For PHY Control-------------------------------*/

/********************/
/* Register Address */
/********************/

//Basic mode control register, basic register
#define PHYMDIO_BMCR				0x00

//Basic mode status register, basic register
#define PHYMDIO_BMSR				0x01

//--------------------------------------Not used-------------------------------------------//
////PHY identifier register 1, extended register
//#define PHY_IDR1				0x02	//not used
//
////PHY identifier register 2, extended register
//#define PHY_IDR2				0x03	//not used
//
////Auto-negotiation advertisement register, extended register
//#define PHY_ANAR				0x04	//not used
//
////Auto-negotiation link partner ability register, extended register
//#define PHY_ANLPAR				0x05	//not used
//
////Auto-negotiation expansion register, extended register
//#define PHY_ANER				0x06	//not used
//
////Auto-negotiation next page transmit
//#define PHY_ANNP				0x07	//not used
//
////Auto-negotiation link partner of the next page receive
//#define PHY_ANLPNP				0x08	//not used
//
////MMD access control register
//#define PHY_REGCR				0x09	//not used
//
////MMD access address data register
//#define PHY_ADDAR				0x0e	//not used
//--------------------------------------Not used-------------------------------------------//

/********************/
/* Bit definitions  */
/********************/

//For BMCR register
#define BMCR_RESET				(1<<15)
#define BMCR_MLOOPBACK			(1<<14)
#define BMCR_SPEED				(1<<13)
#define BMCR_AUTONEGO			(1<<12)
#define BMCR_PWDN				(1<<11)
#define BMCR_ISOLATE			(1<<10)
#define BMCR_RSTNEGO			(1<<9)
#define BMCR_DUP				(1<<8)
#define BMCR_COLTEST			(1<<7)

//For BMSR register
#define BMSR_AUTONEGO_COMPL		(1<<5)
#define BMSR_REMOTE_FAULT		(1<<4)
#define BMSR_LINK_STATUS		(1<<2)
#define BMSR_JAB_DETECT			(1<<1)
#define EXTENDED_CAPA			(1<<0)

//--------------------------------------Not used-------------------------------------------//
////For ANAR register
//#define ANAR_NP					(1<<15)
//#define ANAR_ACK				(1<<14)
//#define ANAR_RF					(1<<13)
//#define ANAR_ASM				(3<<10)
//#define ANAR_T4					(1<<9)
//#define ANAR_TX_FD				(1<<8)
//#define ANAR_TX_HD				(1<<7)
//#define ANAR_10_FD				(1<<6)
//#define ANAR_10_HD				(1<<5)
//#define ANAR_SELECTOR			(0x1F<<0)
//
////For ANAR register
//#define ANLPAR_NP				(1<<15)
//#define ANLPAR_ACK				(1<<14)
//#define ANLPAR_RF				(1<<13)
//#define ANLPAR_LP_DIR			(1<<11)
//#define ANLPAR_PAUSE			(1<<10)
//#define ANLPAR_T4				(1<<9)
//#define ANLPAR_TX_FD			(1<<8)
//#define ANLPAR_TX_HD			(1<<7)
//#define ANLPAR_10_FD			(1<<6)
//#define ANLPAR_10_HD			(1<<5)
//#define ANLPAR_SELECTOR			(0x1F<<0)

/**/
/* MDIO register*/
//PCS_CTL_1			|		PCS control 1 register
//PCS_STS_1			|		PCS status 1 register
//EEE_ABILITY  		|		EEE capability register
//WAKE_ER_CNTR		|		EEE wake error counter
//EEE_ADVR 			|		EEE Advertisement register
//EEE_LPAR 			|		EEE link partner ability register

//--------------------------------------Not used-------------------------------------------//

/********************/
/*Functions for PHY */
/********************/
//todo move this definition to bit area
#define PHYACR_READ			0x02
#define PHYACR_WRITE		0x01




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
 * @ingroup Basic_IO_function_W5100S 
 * @brief It reads 1 byte value from a register.
 * @param AddrSel Register address
 * @return The value of register
 */
uint8_t  WIZCHIP_READ (uint32_t AddrSel);

/**
 * @ingroup Basic_IO_function_W5100S
 * @brief It writes 1 byte value to a register.
 * @param AddrSel Register address
 * @param wb Write data
 * @return void
 */
void     WIZCHIP_WRITE(uint32_t AddrSel, uint8_t wb );

/**
 * @ingroup Basic_IO_function_W5100S
 * @brief It reads sequence data from registers.
 * @param AddrSel Register address
 * @param pBuf Pointer buffer to read data
 * @param len Data length
 */
void     WIZCHIP_READ_BUF (uint32_t AddrSel, uint8_t* pBuf, uint16_t len);

/**
 * @ingroup Basic_IO_function_W5100S
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
 * @ingroup Common_register_access_function_W5100S
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
 * @ingroup Common_register_access_function_W5100S
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
 * @ingroup Common_register_access_function_W5100S
 * @brief Set @ref GAR.
 * @param (uint8_t*)gar Pointer variable to set gateway IP address. It should be allocated 4 bytes.
 * @sa getGAR()
 */
#define setGAR(gar) \
		WIZCHIP_WRITE_BUF(GAR,gar,4)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get @ref GAR.
 * @param (uint8_t*)gar Pointer variable to get gateway IP address. It should be allocated 4 bytes.
 * @sa setGAR()
 */
#define getGAR(gar) \
		WIZCHIP_READ_BUF(GAR,gar,4)

/**
 * @ingroup Common_register_access_function_W5100S
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
 * @ingroup Common_register_access_function_W5100S
 * @brief Get @ref SUBR.
 * @param (uint8_t*)subr Pointer variable to get subnet mask address. It should be allocated 4 bytes.
 * @sa setSUBR()
 */
#define getSUBR(subr) \
		WIZCHIP_READ_BUF(SUBR, subr, 4)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set @ref SHAR.
 * @param (uint8_t*)shar Pointer variable to set local MAC address. It should be allocated 6 bytes.
 * @sa getSHAR()
 */
#define setSHAR(shar) \
		WIZCHIP_WRITE_BUF(SHAR, shar, 6)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get @ref SHAR.
 * @param (uint8_t*)shar Pointer variable to get local MAC address. It should be allocated 6 bytes.
 * @sa setSHAR()
 */
#define getSHAR(shar) \
		WIZCHIP_READ_BUF(SHAR, shar, 6)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set @ref SIPR.
 * @param (uint8_t*)sipr Pointer variable to set local IP address. It should be allocated 4 bytes.
 * @sa getSIPR()
*/
#define setSIPR(sipr) \
		WIZCHIP_WRITE_BUF(SIPR, sipr, 4)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get @ref SIPR.
 * @param (uint8_t*)sipr Pointer variable to get local IP address. It should be allocated 4 bytes.
 * @sa setSIPR()
 */
#define getSIPR(sipr) \
		WIZCHIP_READ_BUF(SIPR, sipr, 4)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref IR register
 * @param (uint8_t)ir Value to set \ref IR register.
 * @sa getIR()
 */
#define setIR(ir) \
		WIZCHIP_WRITE(IR, (ir & 0xE0)) //peter 2016.11.07 unreachable interrupt bit added
		//WIZCHIP_WRITE(IR, (ir & 0xA0))
/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref IR register
 * @return uint8_t. Value of \ref IR register.
 * @sa setIR()
 */
#define getIR() \
		(WIZCHIP_READ(IR) & 0xE0) //peter 2016.11.07 unreachable interrupt bit added
		//(WIZCHIP_READ(IR) & 0xA0)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref _IMR_ register
 * @param (uint8_t)imr Value to set @ref _IMR_ register.
 * @sa getIMR()
 */
#define setIMR(imr) \
		WIZCHIP_WRITE(_IMR_, imr)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref _IMR_ register
 * @return uint8_t. Value of @ref _IMR_ register.
 * @sa setIMR()
 */
#define getIMR() \
		WIZCHIP_READ(_IMR_)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref _RTR_ register
 * @param (uint16_t)rtr Value to set @ref _RTR_ register.
 * @sa getRTR()
 */
#define setRTR(rtr)   {\
		WIZCHIP_WRITE(_RTR_,   (uint8_t)(rtr >> 8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(_RTR_,1), (uint8_t) rtr); \
	}

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref _RTR_ register
 * @return uint16_t. Value of @ref _RTR_ register.
 * @sa setRTR()
 */
#define getRTR() \
		(((uint16_t)WIZCHIP_READ(_RTR_) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(_RTR_,1)))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref _RCR_ register
 * @param (uint8_t)rcr Value to set @ref _RCR_ register.
 * @sa getRCR()
 */
#define setRCR(rcr) \
		WIZCHIP_WRITE(_RCR_, rcr)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref _RCR_ register
 * @return uint8_t. Value of @ref _RCR_ register.
 * @sa setRCR()
 */
#define getRCR() \
		WIZCHIP_READ(_RCR_)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref RMSR register
 * @sa getRMSR()
 */
#define setRMSR(rmsr)   \
      WIZCHIP_WRITE(RMSR,rmsr) // Receicve Memory Size

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref RMSR register
 * @return uint8_t. Value of @ref RMSR register.
 * @sa setRMSR()
 */
 #define getRMSR()   \
      WIZCHIP_READ(RMSR) // Receicve Memory Size

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref TMSR register
 * @sa getTMSR()
 */
#define setTMSR(tmsr)   \
      WIZCHIP_WRITE(TMSR,tmsr) // Receicve Memory Size

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref TMSR register
 * @return uint8_t. Value of @ref TMSR register.
 * @sa setTMSR()
 */
#define getTMSR()	\
	WIZCHIP_READ(TMSR)


/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PATR register
 * @return uint16_t. Value to set \ref PATR register
 */
#define getPATR() \
		(((uint16_t)WIZCHIP_READ(PATR) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(PATR,1)))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PPPALGO register
 * @return uint8_t. Value to set \ref PPPALGO register
 */
#define getPPPALGO() \
		WIZCHIP_READ(PPPALGO)


/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PTIMER register
 * @param (uint8_t)ptimer Value to set \ref PTIMER register.
 * @sa getPTIMER()
 */
#define setPTIMER(ptimer) \
		WIZCHIP_WRITE(PTIMER, ptimer)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PTIMER register
 * @return uint8_t. Value of @ref PTIMER register.
 * @sa setPTIMER()
 */
#define getPTIMER() \
		WIZCHIP_READ(PTIMER)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PMAGIC register
 * @param (uint8_t)pmagic Value to set @ref PMAGIC register.
 * @sa getPMAGIC()
 */
#define setPMAGIC(pmagic) \
		WIZCHIP_WRITE(PMAGIC, pmagic)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PMAGIC register
 * @return uint8_t. Value of @ref PMAGIC register.
 * @sa setPMAGIC()
 */
#define getPMAGIC() \
		WIZCHIP_READ(PMAGIC)


//todo Functions for W5100S

/*----------------------------------------------------------------------*/
/*								W5100S only								*/
/*----------------------------------------------------------------------*/

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref IR2 register
 * @param (uint8_t)ir2 Value to set @ref IR2 register.
 * @sa getIR2()
 */
#define setIR2(ir2) \
		WIZCHIP_WRITE(IR2, ir2)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref IR2 register
 * @return uint8_t. Value of @ref IR2 register.
 * @sa setIR2()
 */
#define getIR2() \
		WIZCHIP_READ(IR2)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref IMR2 register
 * @param (uint8_t)imr2 Value to set @ref IMR2 register.
 * @sa setIMR2()
 */
#define setIMR2(imr2) \
		WIZCHIP_WRITE(IMR2,imr2)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref IMR2 register
 * @return uint8_t. Value of @ref IMR2 register.
 * @sa getIMR2()
 */
#define getIMR2() \
		WIZCHIP_READ(IMR2)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref UIPR(Unreachable IP Address Register) registers
 * @param (uint8_t*)uipr Value to set @ref UIPR registers.
 * @sa setUIPR()
 */
#define setUIPR(uipr) \
		WIZCHIP_WRITE_BUF(UIPR,uipr,4)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref UIPR(Unreachable IP Address Register) registers
 * @param (uint8_t*)uipr Value to get @ref UIPR registers
 * @sa setUIPR()
 */
#define getUIPR(uipr) \
		WIZCHIP_READ_BUF(UIPR,uipr,4)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref UPORTR(Unreachable Port Address Register) register
 * @param (uint16_t)uportr Value to set @ref UPORTR register.
 * @sa getUPORTR()
 */
#define	setUPORTR(uportr) {\
		WIZCHIP_WRITE(UPORTR, (uint8_t)(uportr >> 8)); \
		WIZCHIP_WRITE(UPORTR+1, (uint8_t) uportr); \
	}

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref UPORTR(Unreachable Port Address Register) register
 * @return uint16_t. Value of @ref UPORTR register.
 * @sa setUPORTR()
 */
#define getUPORTR() \
		(((uint16_t)WIZCHIP_READ(UPORTR) << 8) + WIZCHIP_READ(UPORTR+1))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref MR2 register
 * @param (uint8_t)mr2 Value to set @ref MR2 registers.
 * @sa getMR2()
 */
#define setMR2(mr2) \
		WIZCHIP_WRITE(MR2,mr2)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref MR2 register
 * @return uint8_t. Value of @ref MR2 register.
 * @sa setMR2()
 */
#define getMR2() \
		WIZCHIP_READ(MR2)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PHAR registers
 * @param (uint8_t*)phar Value to set @ref PHAR registers.
 * @sa getPHAR()
 */
#define setPHAR(phar) \
		WIZCHIP_WRITE_BUF(PHAR,phar,6)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHAR registers
 * @param (uint8_t*)phar Pointer variable to get @ref PHAR registers.
 * @sa setPHAR()
 */
#define getPHAR(phar) \
		WIZCHIP_READ_BUF(PHAR,phar,6)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PSIDR register
 * @param (uint16_t)psidr Value to set @ref PSIDR register.
 * @sa getPSIDR()
 */
#define setPSIDR(psidr) {\
		WIZCHIP_WRITE(PSIDR, (uint8_t)(psidr >> 8)); \
		WIZCHIP_WRITE(PSIDR+1, (uint8_t) psidr); \
	}

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PSIDR register
 * @return uint16_t. Value of @ref PSIDR register.
 * @sa setPSIDR()
 */
#define getPSIDR() \
		(((uint16_t)WIZCHIP_READ(PSIDR) << 8) + WIZCHIP_READ(PSIDR+1))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PMRUR register
 * @param (uint16_t)pmrur Value to set @ref PMRUR register.
 * @sa getPMRUR()
 */
#define setPMRUR(pmrur) {\
		WIZCHIP_WRITE(PMRUR, (uint8_t)(pmrur >> 8)); \
		WIZCHIP_WRITE(PMRUR+1, (uint8_t) pmrur); \
	}

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PMRUR register
 * @return uint16_t. Value of @ref PMRUR register.
 * @sa setPMRUR()
 */
#define getPMRUR() \
		(((uint16_t)WIZCHIP_READ(PMRUR) << 8) + WIZCHIP_READ(PMRUR+1))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHYSR register
 * @return uint8_t. Value of @ref PHYSR register.
 * @sa setPHYSR()
 */
#define getPHYSR()	\
		WIZCHIP_READ(PHYSR)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHYSR1 register
 * @return uint8_t. Value of @ref PHYSR1 register.
 * @sa setPHYSR1()
 */
#define getPHYSR1()	\
		WIZCHIP_READ(PHYSR1)

/**
 * For internal uses
 * The address of the PHY is fixed as "0x0A".
 */
#define getPHYAR() \
		WIZCHIP_READ(PHYAR)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHYRAR register
 * @return uint8_t. Value of @ref PHYRAR register.
 * @sa setPHYRAR()
 */
#define getPHYRAR() \
		WIZCHIP_READ(PHYRAR)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PHYRR register
 * @param (uint8_t)phyrar Value to set @ref PHYRR register.
 * @sa getPHYRR()
 */
#define setPHYRR(phyrar) \
		WIZCHIP_WRITE(PHYRAR, phyrar)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHYDIR register
 * @return uint16_t. Value of @ref PHYDIR register.
 * @sa setPHYRAR()
 */
//read the value of the phy data input register
#define getPHYDIR() \
		(((uint16_t)WIZCHIP_READ(PHYDIR+1) << 8) + WIZCHIP_READ(PHYDIR))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PHYDIR register
 * @param (uint16_t)phydir Value to set @ref PHYDIR register.
 * @sa getPHYDIR()
 */
//write the value of the phy data input register
#define setPHYDIR(phydir) {\
		WIZCHIP_WRITE(PHYDIR+1, (uint8_t)(phydir >> 8)); \
		WIZCHIP_WRITE(PHYDIR, (uint8_t) phydir); \
	}

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHYDOR register
 * @return uint16_t. Value of @ref PHYDOR register.
 * @sa setPHYDOR()
 */
//read the value of the phy data output register
#define getPHYDOR()	\
		(((uint16_t)WIZCHIP_READ(PHYDOR+1) << 8) + WIZCHIP_READ(PHYDOR))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PHYDOR register
 * @param (uint16_t)phydor Value to set @ref PHYDOR register.
 * @sa getPHYDOR()
 */
//write the value of the phy data output register
#define setPHYDOR(phydor) {\
		WIZCHIP_WRITE(PHYDOR, (uint8_t)(phydor >> 8)); \
		WIZCHIP_WRITE(PHYDOR+1, (uint8_t) phydor); \
	}

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHYACR register
 * @return uint8_t. Value of @ref PHYACR register.
 * @sa setPHYACR()
 */
//read the value of the phy action register ***This register will be cleared automatically***
#define getPHYACR() \
		WIZCHIP_READ(PHYACR)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PHYACR register
 * @param (uint8_t)phyacr Value to set @ref PHYACR register.
 * @sa getPHYACR()
 */
//write the value of the phy action register
#define setPHYACR(phyacr)	\
		WIZCHIP_WRITE(PHYACR,phyacr)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PHYDIVR register
 * @param (uint8_t)phydivr Value to set @ref PHYDIVR register.
 * @sa getPHYDIVR()
 */
#define setPHYDIVR(phydivr)	\
		WIZCHIP_WRITE(PHYDIVR, phydivr)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHYDIVR register
 * @return uint8_t. Value of @ref PHYDIVR register.
 * @sa setPHYDIVR()
 */
#define getPHYDIVR()	\
		WIZCHIP_READ(PHYDIVR)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PHYCR0 register
 * @param (uint8_t)phych0 Value to set @ref PHYCR0 register.
 * @sa getPHYCR0()
 */
#define setPHYCR0(phych0)		\
		WIZCHIP_WRITE(PHYCR0,phych0)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHYCR0 register
 * @return uint8_t. Value of @ref PHYCR0 register.
 * @sa setPHYCR0()
 */
#define getPHYCR0()	\
		WIZCHIP_READ(PHYCR0)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PHYCR1 register
 * @param (uint8_t)phycr1 Value to set @ref PHYCR1 register.
 * @sa getPHYCR1()
 */
#define setPHYCR1(phycr1)	\
		WIZCHIP_WRITE(PHYCR1,phycr1)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PHYCR1 register
 * @return uint8_t. Value of @ref PHYCR1 register.
 * @sa setPHYCR1()
 */
#define getPHYCR1() \
		WIZCHIP_READ(PHYCR1)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref SLCR register
 * @param (uint8_t)rqcr Value to set @ref SLCR register.
 * @sa getSLCR()
 */
#define setSLCR(rqcr)	\
		WIZCHIP_WRITE(SLCR, rqcr)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref RQCR register
 * @return uint8_t. Value of @ref RQCR register.
 * @sa setRQCR()
 */
#define getSLCR()	\
		WIZCHIP_READ(RQCR)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref SLRTR register
 * @param (uint16_t)slrtr Value to set @ref SLRTR register.
 * @sa getSLRTR()
 */
#define setSLRTR(slrtr)	\
		WIZCHIP_WRITE(SLRTR, (uint8_t)(slrtr >> 8)); \
		WIZCHIP_WRITE(SLRTR+1, (uint8_t) slrtr); \

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref SLRTR register
 * @return uint16_t. Value of @ref SLRTR register.
 * @sa setSLRTR()
 */
#define getSLRTR()	\
		(((uint16_t)WIZCHIP_READ(SLRTR) << 8) + WIZCHIP_READ(SLRTR+1))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref SLRCR register
 * @param (uint8_t)slrcr Value to set @ref SLRCR register.
 * @sa getSLRCR()
 */
#define setSLRCR(slrcr)	\
		WIZCHIP_WRITE(SLRCR,slrcr)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref SLRCR register
 * @return uint8_t. Value of @ref SLRCR register.
 * @sa setSLRCR()
 */
#define getSLRCR()	\
		WIZCHIP_READ(SLRCR)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref SLPIPR registers
 * @param (uint8_t*)slpipr Values to set @ref SLPIPR registers.
 * @sa getSLPIPR()
 */
#define setSLPIPR(slpipr)	\
		WIZCHIP_WRITE_BUF(SLPIPR,slpipr,4)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref SLPIPR registers
 * @param (uint8_t*)slpipr Values to get @ref SLPIPR registers.
 * @sa getSLPIPR()
 */
#define getSLPIPR(slpipr)	\
		WIZCHIP_READ_BUF(SLPIPR,slpipr,4)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref SLPHAR registers
 * @param (uint8_t*)slphar Values to set @ref SLPHAR registers.
 * @sa getSLPHAR()
 */
#define setSLPHAR(slphar) \
		WIZCHIP_WRITE_BUF(SLPHAR,slphar,6)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref SLPHAR registers
 * @param (uint8_t*)slphar Values to get @ref SLPHAR registers.
 * @sa getSLPHAR()
 */
#define getSLPHAR(slphar) \
		WIZCHIP_READ_BUF(SLPHAR,slphar,6)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PINGSEQR register
 * @param (uint16_t)pingseqr Value to set @ref PINGSEQR register.
 * @sa getPINGSEQR()
 */
#define setPINGSEQR(pingseqr) {\
		WIZCHIP_WRITE(PINGSEQR, (uint8_t)(pingseqr >> 8)); \
		WIZCHIP_WRITE(PINGSEQR+1, (uint8_t) pingseqr); \
	}

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PINGSEQR register
 * @return uint16_t. Value of @ref PINGSEQR register.
 * @sa setPINGSEQR()
 */
#define getPINGSEQR() \
		(((uint16_t)WIZCHIP_READ(PINGSEQR) << 8) + WIZCHIP_READ(PINGSEQR+1))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref PINGIDR register
 * @param (uint16_t)pingidr Value to set @ref PINGIDR register.
 * @sa getPINGIDR()
 */
#define setPINGIDR(pingidr) {\
		WIZCHIP_WRITE(PINGIDR, (uint8_t)(pingidr >> 8)); \
		WIZCHIP_WRITE(PINGIDR+1, (uint8_t) pingidr); \
	}

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref PINGIDR register
 * @return uint16_t. Value of @ref PINGIDR register.
 * @sa setPINGIDR()
 */
#define getPINGIDR()	\
		(((uint16_t)WIZCHIP_READ(PINGIDR) << 8) + WIZCHIP_READ(PINGIDR+1))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref SLIMR register
 * @param (uint8_t)slimr Value to set @ref SLIMR register.
 * @sa getSLIMR()
 */
#define setSLIMR(slimr)	\
		WIZCHIP_WRITE(SLIMR, slimr)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref SLIMR register
 * @return uint8_t. Value of @ref SLIMR register.
 * @sa setSLIMR()
 */
#define getSLIMR()	\
		WIZCHIP_READ(SLIMR)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref SLIR register
 * @param (uint8_t)slir Value to set @ref SLIR register.
 * @sa getSLIMR()
 */
#define setSLIR(slir)	\
		WIZCHIP_WRITE(SLIR, slir)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref SLIMR register
 * @return uint8_t. Value of @ref SLIMR register.
 * @sa setSLIMR()
 */
#define getSLIR()	\
		WIZCHIP_READ(SLIR)

/*Hidden functions for W5100S*/
#define setDBGOUT(dbgout) 	{\
		WIZCHIP_WRITE(DBGOUT,(uint8_t)(dbgout >> 16));	\
		WIZCHIP_WRITE(DBGOUT,(uint8_t)(dbgout >> 8));	\
		WIZCHIP_WRITE(DBGOUT,(uint8_t)(dbgout));		\
	}

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref NICMAXCOLR register
 * @param (uint8_t)nicmaxcolr Value to set @ref NICMAXCOLR register.
 * @sa getNICMAXCOLR()
 */
#define setNICMAXCOLR(nicmaxcolr)	\
		WIZCHIP_WRITE(NICMAXCOLR,nicmaxcolr)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref NICMAXCOLR register
 * @return uint8_t. Value of @ref NICMAXCOLR register.
 * @sa setNICMAXCOLR()
 */
#define getNICMAXCOLR()	\
		WIZCHIP_READ(NICMAXCOLR)

/*Clock lock/unlock*/

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief LOCK Chip Information
 * @sa CHIPULLOCK()
 */
#define CHIPLOCK()	\
		WIZCHIP_WRITE(CHIPLCKR,0xff)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Unlock Chip Information
 * @sa CHIPLOCK()
 */
#define CHIPUNLOCK()	\
		WIZCHIP_WRITE(CHIPLCKR,0xCE)


/**
 * @ingroup Common_register_access_function_W5100S
 * @brief LOCK Chip Information
 * @sa CHIPULLOCK()
 */
/*Network information lock/unlock*/
#define NETLOCK()	\
		WIZCHIP_WRITE(NETLCKR,0x3A)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Unlock Chip Information
 * @sa CHIPLOCK()
 */
#define NETUNLOCK()	\
		WIZCHIP_WRITE(NETLCKR,0xC5)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Lock PHYCR0,CR1 Information
 * @sa CHIPULLOCK()
 */
/*PHY CR0,CR1 lock/unlock*/
#define PHYLOCK()	\
		WIZCHIP_WRITE(PHYLCKR,0xff)

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Lock PHYCR0,CR1 Information
 * @sa CHIPULLOCK()
 */
#define PHYUNLOCK()	\
		WIZCHIP_WRITE(PHYLCKR,0x53)

/**
 * @ingroup Version register_access_function_W5100SS
 * @brief Get version information.
 * @return uint16_t. It must be "0x51"
 */
#define getVER() \
		(WIZCHIP_READ(VERR))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Get \ref TCNTR register
 * @return uint16_t. Value of @ref TCNTR register.
 * @sa setNTCNTR()
 */
/*Get 100us internal counter*/
#define getTCNTR()	\
		(((uint16_t)WIZCHIP_READ(TCNTR) << 8) + WIZCHIP_READ(TCNTR+1))

/**
 * @ingroup Common_register_access_function_W5100S
 * @brief Set \ref TCNTR register
 * @param (uint8_t)
  Value to set @ref TCNTR register.
 * @sa getTCNTCLKR()
 */
/*Reset 100us internal counter(TCNTR)*/
#define setTCNTCLKR(var)	\
		WIZCHIP_WRITE(TCNTCLKR, var)

/*w5100s only end*/





///////////////////////////////////
// Socket N register I/O function //
///////////////////////////////////
/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_MR register
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 * @param mr Value to set @ref Sn_MR
 * @sa getSn_MR()
 */
#define setSn_MR(sn, mr) \
		WIZCHIP_WRITE(Sn_MR(sn),mr)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_MR register
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b> expect <b>bit 4</b>.
 * @return Value of @ref Sn_MR.
 * @sa setSn_MR()
 */
#define getSn_MR(sn) \
	WIZCHIP_READ(Sn_MR(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)cr Value to set @ref Sn_CR
 * @sa getSn_CR()
 */
#define setSn_CR(sn, cr) \
		WIZCHIP_WRITE(Sn_CR(sn), cr)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_CR.
 * @sa setSn_CR()
 */
#define getSn_CR(sn) \
		WIZCHIP_READ(Sn_CR(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_IR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)ir Value to set @ref Sn_IR
 * @sa getSn_IR()
 */
#define setSn_IR(sn, ir) \
		WIZCHIP_WRITE(Sn_IR(sn), ir)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_IR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_IR.
 * @sa setSn_IR()
 */
#define getSn_IR(sn) \
		WIZCHIP_READ(Sn_IR(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_SR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_SR.
 */
#define getSn_SR(sn) \
		WIZCHIP_READ(Sn_SR(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
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
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_PORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_PORT.
 * @sa setSn_PORT()
 */
#define getSn_PORT(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_PORT(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_PORT(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_DHAR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t*)dhar Pointer variable to set socket n destination hardware address. It should be allocated 6 bytes.
 * @sa getSn_DHAR()
 */
#define setSn_DHAR(sn, dhar) \
		WIZCHIP_WRITE_BUF(Sn_DHAR(sn), dhar, 6)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_DHAR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t*)dhar Pointer variable to get socket n destination hardware address. It should be allocated 6 bytes.
 * @sa setSn_DHAR()
 */
#define getSn_DHAR(sn, dhar) \
		WIZCHIP_READ_BUF(Sn_DHAR(sn), dhar, 6)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t*)dipr Pointer variable to set socket n destination IP address. It should be allocated 4 bytes.
 * @sa getSn_DIPR()
 */
#define setSn_DIPR(sn, dipr) \
		WIZCHIP_WRITE_BUF(Sn_DIPR(sn), dipr, 4)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t*)dipr Pointer variable to get socket n destination IP address. It should be allocated 4 bytes.
 * @sa SetSn_DIPR()
 */
#define getSn_DIPR(sn, dipr) \
		WIZCHIP_READ_BUF(Sn_DIPR(sn), dipr, 4)

/**
 * @ingroup Socket_register_access_function_W5100S
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
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_DPORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_DPORT.
 * @sa setSn_DPORT()
 */
#define getSn_DPORT(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_DPORT(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DPORT(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100S
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
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_MSSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_MSSR.
 * @sa setSn_MSSR()
 */
#define getSn_MSSR(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_MSSR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_MSSR(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_PROTO register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)proto Value to set \ref Sn_PROTO
 * @sa getSn_PROTO()
 */
#define setSn_PROTO(sn, proto) \
		WIZCHIP_WRITE(Sn_PROTO(sn), proto)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_PROTO register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_PROTO.
 * @sa setSn_PROTO()
 */
#define getSn_PROTO(sn) \
		WIZCHIP_READ(Sn_PROTO(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_TOS register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)tos Value to set @ref Sn_TOS
 * @sa getSn_TOS()
 */
#define setSn_TOS(sn, tos) \
		WIZCHIP_WRITE(Sn_TOS(sn), tos)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_TOS register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_ </b>.
 * @return uint8_t. Value of Sn_TOS.
 * @sa setSn_TOS()
 */
#define getSn_TOS(sn) \
		WIZCHIP_READ(Sn_TOS(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_TTL register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_ </b>.
 * @param (uint8_t)ttl Value to set @ref Sn_TTL
 * @sa getSn_TTL()
 */
#define setSn_TTL(sn, ttl) \
		WIZCHIP_WRITE(Sn_TTL(sn), ttl)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_TTL register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_ </b>.
 * @return uint8_t. Value of @ref Sn_TTL.
 * @sa setSn_TTL()
 */
#define getSn_TTL(sn) \
		WIZCHIP_READ(Sn_TTL(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_RXMEM_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_ </b>.
 * @param (uint8_t)rxmemsize Value to set \ref Sn_RXMEM_SIZE
 * @sa getSn_RXMEM_SIZE()
 */
#define  setSn_RXMEM_SIZE(sn, rxmemsize) \
      WIZCHIP_WRITE(RMSR, (WIZCHIP_READ(RMSR) & ~(0x03 << (2*sn))) | (rxmemsize << (2*sn)))
#define setSn_RXBUF_SIZE(sn,rxmemsize) setSn_RXMEM_SIZE(sn,rxmemsize)
/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_RXMEM_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_RXMEM.
 * @sa setSn_RXMEM_SIZE()
 */
#define  getSn_RXMEM_SIZE(sn) \
      ((WIZCHIP_READ(RMSR) & (0x03 << (2*sn))) >> (2*sn))
#define  getSn_RXBUF_SIZE(sn) getSn_RXMEM_SIZE(sn)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_TXMEM_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)txmemsize Value to set \ref Sn_TXMEM_SIZE
 * @sa getSn_TXMEM_SIZE()
 */
#define setSn_TXMEM_SIZE(sn, txmemsize) \
      WIZCHIP_WRITE(TMSR, (WIZCHIP_READ(TMSR) & ~(0x03 << (2*sn))) | (txmemsize << (2*sn)))
#define  setSn_TXBUF_SIZE(sn, txmemsize) setSn_TXMEM_SIZE(sn,txmemsize)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_TXMEM_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_TXMEM_SIZE.
 * @sa setSn_TXMEM_SIZE()
 */
#define  getSn_TXMEM_SIZE(sn) \
      ((WIZCHIP_READ(TMSR) & (0x03 << (2*sn))) >> (2*sn))
#define  getSn_TXBUF_SIZE(sn) getSn_TXMEM_SIZE(sn)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_TX_FSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_TX_FSR.
 */
uint16_t getSn_TX_FSR(uint8_t sn);

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_TX_RD register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_TX_RD.
 */
#define getSn_TX_RD(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_TX_RD(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_RD(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100S
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
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_TX_WR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_TX_WR.
 * @sa setSn_TX_WR()
 */
#define getSn_TX_WR(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_TX_WR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_WR(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_RX_RSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_RX_RSR.
 */
uint16_t getSn_RX_RSR(uint8_t sn);

/**
 * @ingroup Socket_register_access_function_W5100S
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
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_RX_RD register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @regurn uint16_t. Value of @ref Sn_RX_RD.
 * @sa setSn_RX_RD()
 */
#define getSn_RX_RD(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_RX_RD(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RD(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100S
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
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_RX_WR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_RX_WR.
 */
#define getSn_RX_WR(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_RX_WR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_WR(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set @ref Sn_FRAGR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)frag Value to set \ref Sn_FRAGR
 * @sa getSn_FRAG()
 */
#define setSn_FRAGR(sn, fragr) { \
		WIZCHIP_WRITE(Sn_FRAGR(sn),  (uint8_t)(fragr >>8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_FRAGR(sn),1), (uint8_t) fragr); \
	}

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get @ref Sn_FRAGR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of @ref Sn_FRAGR.
 * @sa setSn_FRAG()
 */
#define getSn_FRAGR(sn) \
		(((uint16_t)WIZCHIP_READ(Sn_FRAGR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_FRAGR(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the max RX buffer size of socket sn
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Max buffer size
 */
#define getSn_RxMAX(sn) \
		((uint16_t)(0x0001 << getSn_RXMEM_SIZE(sn)) << 10)


/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the max TX buffer size of socket sn
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Max buffer size
 */
#define getSn_TxMAX(sn) \
		((uint16_t)(0x0001 << getSn_TXMEM_SIZE(sn)) << 10)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the mask of socket sn RX buffer.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Mask value
 */
#define getSn_RxMASK(sn) \
		(getSn_RxMAX(sn) - 1)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the mask of socket sn TX buffer
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Mask value
 */
#define getSn_TxMASK(sn) \
		(getSn_TxMAX(sn) - 1)


/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the base address of socket sn RX buffer.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of Socket n RX buffer base address.
 */
uint32_t getSn_RxBASE(uint8_t sn);

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the base address of socket sn TX buffer.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of Socket n TX buffer base address.
 */
uint32_t getSn_TxBASE(uint8_t sn);


/*socket register W5100S only*/

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set the interrupt mask register of socket sn.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)imr Value to set \ref Sn_IMR
 * @sa getSn_IMR(sn)
 */
#define setSn_IMR(sn,imr)	\
		WIZCHIP_WRITE(Sn_IMR(sn),imr)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the interrupt mask register of socket sn.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of Socket n interrupt mask register.
 */
#define getSn_IMR(sn)	\
		WIZCHIP_READ(Sn_IMR(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set the Sn_MR2 value of socket sn.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param mr2 Value of Sn_MR2 register to set.
 */
#define setSn_MR2(sn,mr2)	\
		WIZCHIP_WRITE(Sn_MR2(sn), mr2)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the Sn_MR2 value of socket sn.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of Socket n Sn_MR2 register.
 */
#define getSn_MR2(sn)	\
		WIZCHIP_READ(Sn_MR2(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set the Sn_KPALVTR value of socket sn.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param kpalvtr Value of the Sn_KPALVTR register to set.
 */
#define setSn_KPALVTR(sn,kpalvtr)	\
		WIZCHIP_WRITE(Sn_KPALVTR(sn), kpalvtr)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the Sn_KPALVTR value of socket sn
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of the Sn_KPALVTR register.
 */
#define getSn_KPALVTR(sn)	\
		WIZCHIP_READ(Sn_KPALVTR(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the Sn_TSR register of socket sn.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of the Socket n Sn_TSR register.
 */
#define	getSn_TSR(sn)	\
		WIZCHIP_READ(Sn_TSR(sn))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set the Sn_RTR register of socket sn.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint16_t)rtr Value of the Socket n Sn_RTR register to set.
 */
#define	setSn_RTR(sn,rtr)	{ \
		WIZCHIP_WRITE(Sn_RTR(sn),   (uint8_t)(rtr >> 8)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_RTR(sn),1), (uint8_t) rtr); \
		}

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the Sn_RTR register of socket sn.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint16_t. Value of the Socket n Sn_RTR register.
 */
#define getSn_RTR(sn)	\
		(((uint16_t)WIZCHIP_READ(Sn_RTR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RTR(sn),1)))

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Set the Sn_RCR register of socket sn.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of the Socket n Sn_RCR register to set.
 */
#define setSn_RCR(sn,rcr)	\
		WIZCHIP_WRITE(Sn_RCR(sn),rcr)

/**
 * @ingroup Socket_register_access_function_W5100S
 * @brief Get the Sn_RCR of socket sn.
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of the Socket n Sn_RCR.
 */
#define getSn_RCR(sn)	\
		WIZCHIP_READ(Sn_RCR(sn))

/////////////////////////////////////
// Sn_TXBUF & Sn_RXBUF IO function //
/////////////////////////////////////
/**
 * @ingroup Basic_IO_function_W5100S
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
 * @ingroup Basic_IO_function_W5100S
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
 * @ingroup Basic_IO_function_W5100S
 * @brief It discard the received data in RX memory.
 * @details It discards the data of the length of <i>len(variable)</i> bytes in internal RX memory.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param len Data length
 */
void wiz_recv_ignore(uint8_t sn, uint16_t len);

/**
 * @ingroup Special_function_W5100S
 * @brief Write data to the PHY via MDC/MDIO interface.
 * @details Write command data to the PHY via MDC/MDIO interface.
 * @param (uint8_t)PHYMDIO_regadr Address of the PHY register. It should be PHYMDIO_BMCR or PHYMDIO_BMSR.
 * @param (uint16_t)var Data to write to the PHY register. Please refer to the bit definitions of the BMCR and BMSR register.
 */
void wiz_mdio_write(uint8_t PHYMDIO_regadr, uint16_t var);

/**
 * @ingroup Special_function_W5100S
 * @brief Read data from the PHY via MDC/MDIO interface.
 * @details Read command or status data from the PHY via MDC/MDIO interface.
 * @param (uint8_t)PHYMDIO_regadr Address of the PHY register. It should be PHYMDIO_BMCR or PHYMDIO_BMSR.
 * @return The value of the PHY register
 */
uint16_t wiz_mdio_read(uint8_t PHYMDIO_regadr);

/**
 * @ingroup Special_function_W5100S
 * @brief Delay function
 * @details Delay function using internal 100us timer of the W5100S
 * @param (uint32_t)ms Time to delay in milliseconds.
 */
void wiz_delay_ms(uint32_t ms);

/// @cond DOXY_APPLY_CODE
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif //_W5100S_H_



