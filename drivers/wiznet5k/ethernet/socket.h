//*****************************************************************************
//
//! \file socket.h
//! \brief SOCKET APIs Header file.
//! \details SOCKET APIs like as berkeley socket api. 
//! \version 1.0.2
//! \date 2013/10/21
//! \par  Revision history
//!       <2014/05/01> V1.0.2. Refer to M20140501
//!         1. Modify the comment : SO_REMAINED -> PACK_REMAINED
//!         2. Add the comment as zero byte udp data reception in getsockopt(). 
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
/**
 * @defgroup WIZnet_socket_APIs 1. WIZnet socket APIs
 * @brief WIZnet socket APIs are based on Berkeley socket APIs,  thus it has much similar name and interface.
 *        But there is a little bit of difference.
 * @details
 * <b> Comparison between WIZnet and Berkeley SOCKET APIs </b>
 * <table>
 *    <tr>   <td><b>API</b></td> <td><b>WIZnet</b></td> <td><b>Berkeley</b></td>   </tr>
 *    <tr>   <td>socket()</td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>bind()</b></td> <td>X</td> <td>O</td>   </tr>
 *    <tr>   <td><b>listen()</b></td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>connect()</b></td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>accept()</b></td> <td>X</td> <td>O</td>   </tr>
 *    <tr>   <td><b>recv()</b></td> <td>O</td> <td>O</td>    </tr>
 *    <tr>   <td><b>send()</b></td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>recvfrom()</b></td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>sendto()</b></td> <td>O</td> <td>O</td>    </tr>
 *    <tr>   <td><b>closesocket()</b></td> <td>O<br>close() & disconnect()</td> <td>O</td>   </tr>
 * </table>
 * There are @b bind() and @b accept() functions in @b Berkeley SOCKET API but,
 * not in @b WIZnet SOCKET API. Because socket() of WIZnet is not only creating a SOCKET but also binding a local port number,
 * and listen() of WIZnet is not only listening to connection request from client but also accepting the connection request. \n
 * When you program "TCP SERVER" with Berkeley SOCKET API, you can use only one listen port.
 * When the listen SOCKET accepts a connection request from a client, it keeps listening.
 * After accepting the connection request, a new SOCKET is created and the new SOCKET is used in communication with the client. \n
 * Following figure shows network flow diagram by Berkeley SOCKET API.
 * @image html Berkeley_SOCKET.jpg "<Berkeley SOCKET API>"
 * But, When you program "TCP SERVER" with WIZnet SOCKET API, you can use as many as 8 listen SOCKET with same port number. \n
 * Because there's no accept() in WIZnet SOCKET APIs, when the listen SOCKET accepts a connection request from a client,
 * it is changed in order to communicate with the client.
 * And the changed SOCKET is not listening any more and is dedicated for communicating with the client. \n
 * If there're many listen SOCKET with same listen port number and a client requests a connection,
 * the SOCKET which has the smallest SOCKET number accepts the request and is changed as communication SOCKET. \n
 * Following figure shows network flow diagram by WIZnet SOCKET API.
 * @image html WIZnet_SOCKET.jpg "<WIZnet SOCKET API>"
 */
#ifndef _WIZCHIP_SOCKET_H_
#define _WIZCHIP_SOCKET_H_

// use this macro for exported names to avoid name clashes
#define WIZCHIP_EXPORT(name) wizchip_ ## name

#include "wizchip_conf.h"

#define SOCKET                uint8_t  ///< SOCKET type define for legacy driver

#define SOCK_OK               1        ///< Result is OK about socket process.
#define SOCK_BUSY             0        ///< Socket is busy on processing the operation. Valid only Non-block IO Mode.
#define SOCK_FATAL            -1000    ///< Result is fatal error about socket process.

#define SOCK_ERROR            0        
#define SOCKERR_SOCKNUM       (SOCK_ERROR - 1)     ///< Invalid socket number
#define SOCKERR_SOCKOPT       (SOCK_ERROR - 2)     ///< Invalid socket option
#define SOCKERR_SOCKINIT      (SOCK_ERROR - 3)     ///< Socket is not initialized
#define SOCKERR_SOCKCLOSED    (SOCK_ERROR - 4)     ///< Socket unexpectedly closed.
#define SOCKERR_SOCKMODE      (SOCK_ERROR - 5)     ///< Invalid socket mode for socket operation.
#define SOCKERR_SOCKFLAG      (SOCK_ERROR - 6)     ///< Invalid socket flag
#define SOCKERR_SOCKSTATUS    (SOCK_ERROR - 7)     ///< Invalid socket status for socket operation.
#define SOCKERR_ARG           (SOCK_ERROR - 10)    ///< Invalid argument.
#define SOCKERR_PORTZERO      (SOCK_ERROR - 11)    ///< Port number is zero
#define SOCKERR_IPINVALID     (SOCK_ERROR - 12)    ///< Invalid IP address
#define SOCKERR_TIMEOUT       (SOCK_ERROR - 13)    ///< Timeout occurred
#define SOCKERR_DATALEN       (SOCK_ERROR - 14)    ///< Data length is zero or greater than buffer max size.
#define SOCKERR_BUFFER        (SOCK_ERROR - 15)    ///< Socket buffer is not enough for data communication.

#define SOCKFATAL_PACKLEN     (SOCK_FATAL - 1)     ///< Invalid packet length. Fatal Error.

/*
 * SOCKET FLAG
 */
#define SF_ETHER_OWN           (Sn_MR_MFEN)        ///< In \ref Sn_MR_MACRAW, Receive only the packet as broadcast, multicast and own packet
#define SF_IGMP_VER2           (Sn_MR_MC)          ///< In \ref Sn_MR_UDP with \ref SF_MULTI_ENABLE, Select IGMP version 2.   
#define SF_TCP_NODELAY         (Sn_MR_ND)          ///< In \ref Sn_MR_TCP, Use to nodelayed ack.
#define SF_MULTI_ENABLE        (Sn_MR_MULTI)       ///< In \ref Sn_MR_UDP, Enable multicast mode.

#if _WIZCHIP_ == 5500
   #define SF_BROAD_BLOCK         (Sn_MR_BCASTB)   ///< In \ref Sn_MR_UDP or \ref Sn_MR_MACRAW, Block broadcast packet. Valid only in W5500
   #define SF_MULTI_BLOCK         (Sn_MR_MMB)      ///< In \ref Sn_MR_MACRAW, Block multicast packet. Valid only in W5500
   #define SF_IPv6_BLOCK          (Sn_MR_MIP6B)    ///< In \ref Sn_MR_MACRAW, Block IPv6 packet. Valid only in W5500
   #define SF_UNI_BLOCK           (Sn_MR_UCASTB)   ///< In \ref Sn_MR_UDP with \ref SF_MULTI_ENABLE. Valid only in W5500
#endif

#define SF_IO_NONBLOCK           0x01              ///< Socket nonblock io mode. It used parameter in \ref socket().

/*
 * UDP & MACRAW Packet Infomation
 */
#define PACK_FIRST               0x80              ///< In Non-TCP packet, It indicates to start receiving a packet.
#define PACK_REMAINED            0x01              ///< In Non-TCP packet, It indicates to remain a packet to be received.
#define PACK_COMPLETED           0x00              ///< In Non-TCP packet, It indicates to complete to receive a packet.

// resets all global state associated with the socket interface
void WIZCHIP_EXPORT(socket_reset)(void);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Open a socket.
 * @details Initializes the socket with 'sn' passed as parameter and open.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param protocol Protocol type to operate such as TCP, UDP and MACRAW.
 * @param port Port number to be bined.
 * @param flag Socket flags as \ref SF_ETHER_OWN, \ref SF_IGMP_VER2, \ref SF_TCP_NODELAY, \ref SF_MULTI_ENABLE, \ref SF_IO_NONBLOCK and so on.\n
 *             Valid flags only in W5500 : @ref SF_BROAD_BLOCK, @ref SF_MULTI_BLOCK, @ref SF_IPv6_BLOCK, and @ref SF_UNI_BLOCK.
 * @sa Sn_MR
 *
 * @return @b Success : The socket number @b 'sn' passed as parameter\n
 *         @b Fail    :\n @ref SOCKERR_SOCKNUM     - Invalid socket number\n
 *                        @ref SOCKERR_SOCKMODE    - Not support socket mode as TCP, UDP, and so on. \n
 *                        @ref SOCKERR_SOCKFLAG    - Invaild socket flag.
 */
int8_t  WIZCHIP_EXPORT(socket)(uint8_t sn, uint8_t protocol, uint16_t port, uint8_t flag);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Close a socket.
 * @details It closes the socket  with @b'sn' passed as parameter.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 *
 * @return @b Success : @ref SOCK_OK \n
 *         @b Fail    : @ref SOCKERR_SOCKNUM - Invalid socket number
 */
int8_t  WIZCHIP_EXPORT(close)(uint8_t sn);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Listen to a connection request from a client.
 * @details It is listening to a connection request from a client.
 * If connection request is accepted successfully, the connection is established. Socket sn is used in passive(server) mode.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return @b Success : @ref SOCK_OK \n
 *         @b Fail    :\n @ref SOCKERR_SOCKINIT   - Socket is not initialized \n
 *                        @ref SOCKERR_SOCKCLOSED - Socket closed unexpectedly.
 */
int8_t  WIZCHIP_EXPORT(listen)(uint8_t sn);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Try to connect a server.
 * @details It requests connection to the server with destination IP address and port number passed as parameter.\n
 * @note It is valid only in TCP client mode. 
 *       In block io mode, it does not return until connection is completed.
 *       In Non-block io mode, it return @ref SOCK_BUSY immediately.
 *
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param addr Pointer variable of destination IP address. It should be allocated 4 bytes.
 * @param port Destination port number.
 *
 * @return @b Success : @ref SOCK_OK \n
 * @b Fail    :\n @ref SOCKERR_SOCKNUM   - Invalid socket number\n
 *                @ref SOCKERR_SOCKMODE  - Invalid socket mode\n
 *                @ref SOCKERR_SOCKINIT  - Socket is not initialized\n
 *                @ref SOCKERR_IPINVALID - Wrong server IP address\n
 *                @ref SOCKERR_PORTZERO  - Server port zero\n
 *                @ref SOCKERR_TIMEOUT   - Timeout occurred during request connection\n
 *                @ref SOCK_BUSY         - In non-block io mode, it returned immediately\n
 */
int8_t  WIZCHIP_EXPORT(connect)(uint8_t sn, uint8_t * addr, uint16_t port);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Try to disconnect a connection socket.
 * @details It sends request message to disconnect the TCP socket 'sn' passed as parameter to the server or client.
 * @note It is valid only in TCP server or client mode. \n
 *       In block io mode, it does not return until disconnection is completed. \n
 *       In Non-block io mode, it return @ref SOCK_BUSY immediately. \n

 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return @b Success :   @ref SOCK_OK \n
 *         @b Fail    :\n @ref SOCKERR_SOCKNUM  - Invalid socket number \n
 *                        @ref SOCKERR_SOCKMODE - Invalid operation in the socket \n
 *                        @ref SOCKERR_TIMEOUT  - Timeout occurred \n
 *                        @ref SOCK_BUSY        - Socket is busy.
 */
int8_t  WIZCHIP_EXPORT(disconnect)(uint8_t sn);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief	Send data to the connected peer in TCP socket.
 * @details It is used to send outgoing data to the connected socket.
 * @note    It is valid only in TCP server or client mode. It can't send data greater than socket buffer size. \n
 *          In block io mode, It doesn't return until data send is completed - socket buffer size is greater than data. \n
 *          In non-block io mode, It return @ref SOCK_BUSY immediately when socket buffer is not enough. \n
 * @param sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buf Pointer buffer containing data to be sent.
 * @param len The byte length of data in buf.
 * @return	@b Success : The sent data size \n
 *          @b Fail    : \n @ref SOCKERR_SOCKSTATUS - Invalid socket status for socket operation \n
 *                          @ref SOCKERR_TIMEOUT    - Timeout occurred \n
 *                          @ref SOCKERR_SOCKMODE 	- Invalid operation in the socket \n
 *                          @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                          @ref SOCKERR_DATALEN    - zero data length \n
 *                          @ref SOCK_BUSY          - Socket is busy.
 */
int32_t WIZCHIP_EXPORT(send)(uint8_t sn, uint8_t * buf, uint16_t len);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief	Receive data from the connected peer.
 * @details It is used to read incoming data from the connected socket.\n
 *          It waits for data as much as the application wants to receive.
 * @note    It is valid only in TCP server or client mode. It can't receive data greater than socket buffer size. \n
 *          In block io mode, it doesn't return until data reception is completed - data is filled as <I>len</I> in socket buffer. \n
 *          In non-block io mode, it return @ref SOCK_BUSY immediately when <I>len</I> is greater than data size in socket buffer. \n
 *
 * @param sn  Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buf Pointer buffer to read incoming data.
 * @param len The max data length of data in buf.
 * @return	@b Success : The real received data size \n
 *          @b Fail    :\n
 *                     @ref SOCKERR_SOCKSTATUS - Invalid socket status for socket operation \n
 *                     @ref SOCKERR_SOCKMODE   - Invalid operation in the socket \n
 *                     @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                     @ref SOCKERR_DATALEN    - zero data length \n
 *                     @ref SOCK_BUSY          - Socket is busy.
 */
int32_t WIZCHIP_EXPORT(recv)(uint8_t sn, uint8_t * buf, uint16_t len);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief	Sends datagram to the peer with destination IP address and port number passed as parameter.
 * @details It sends datagram of UDP or MACRAW to the peer with destination IP address and port number passed as parameter.\n
 *          Even if the connectionless socket has been previously connected to a specific address,
 *          the address and port number parameters override the destination address for that particular datagram only.
 * @note    In block io mode, It doesn't return until data send is completed - socket buffer size is greater than <I>len</I>.
 *          In non-block io mode, It return @ref SOCK_BUSY immediately when socket buffer is not enough.
 *
 * @param sn    Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buf   Pointer buffer to send outgoing data.
 * @param len   The byte length of data in buf.
 * @param addr  Pointer variable of destination IP address. It should be allocated 4 bytes.
 * @param port  Destination port number.
 *
 * @return @b Success : The sent data size \n
 *         @b Fail    :\n @ref SOCKERR_SOCKNUM     - Invalid socket number \n
 *                        @ref SOCKERR_SOCKMODE    - Invalid operation in the socket \n
 *                        @ref SOCKERR_SOCKSTATUS  - Invalid socket status for socket operation \n
 *                        @ref SOCKERR_DATALEN     - zero data length \n
 *                        @ref SOCKERR_IPINVALID   - Wrong server IP address\n
 *                        @ref SOCKERR_PORTZERO    - Server port zero\n
 *                        @ref SOCKERR_SOCKCLOSED  - Socket unexpectedly closed \n
 *                        @ref SOCKERR_TIMEOUT     - Timeout occurred \n
 *                        @ref SOCK_BUSY           - Socket is busy. 
 */
int32_t WIZCHIP_EXPORT(sendto)(uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port);

/**
 * @ingroup WIZnet_socket_APIs
 * @brief Receive datagram of UDP or MACRAW
 * @details This function is an application I/F function which is used to receive the data in other then TCP mode. \n
 *          This function is used to receive UDP and MAC_RAW mode, and handle the header as well. 
 *          This function can divide to received the packet data.
 *          On the MACRAW SOCKET, the addr and port parameters are ignored.
 * @note    In block io mode, it doesn't return until data reception is completed - data is filled as <I>len</I> in socket buffer
 *          In non-block io mode, it return @ref SOCK_BUSY immediately when <I>len</I> is greater than data size in socket buffer.
 *
 * @param sn   Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buf  Pointer buffer to read incoming data.
 * @param len  The max data length of data in buf. 
 *             When the received packet size <= len, receives data as packet sized.
 *             When others, receives data as len.
 * @param addr Pointer variable of destination IP address. It should be allocated 4 bytes.
 *             It is valid only when the first call recvfrom for receiving the packet.
 *             When it is valid, @ref  packinfo[7] should be set as '1' after call @ref getsockopt(sn, SO_PACKINFO, &packinfo).
 * @param port Pointer variable of destination port number.
 *             It is valid only when the first call recvform for receiving the packet.
*             When it is valid, @ref  packinfo[7] should be set as '1' after call @ref getsockopt(sn, SO_PACKINFO, &packinfo).
 *
 * @return	@b Success : This function return real received data size for success.\n
 *          @b Fail    : @ref SOCKERR_DATALEN    - zero data length \n
 *                       @ref SOCKERR_SOCKMODE   - Invalid operation in the socket \n
 *                       @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                       @ref SOCKBUSY           - Socket is busy.
 */
int32_t WIZCHIP_EXPORT(recvfrom)(uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port);


/////////////////////////////
// SOCKET CONTROL & OPTION //
/////////////////////////////
#define SOCK_IO_BLOCK         0  ///< Socket Block IO Mode in @ref setsockopt().
#define SOCK_IO_NONBLOCK      1  ///< Socket Non-block IO Mode in @ref setsockopt().

/**
 * @defgroup DATA_TYPE DATA TYPE
 */

/**
 * @ingroup DATA_TYPE
 * @brief The kind of Socket Interrupt.
 * @sa Sn_IR, Sn_IMR, setSn_IR(), getSn_IR(), setSn_IMR(), getSn_IMR()
 */
typedef enum
{
   SIK_CONNECTED     = (1 << 0),    ///< connected
   SIK_DISCONNECTED  = (1 << 1),    ///< disconnected
   SIK_RECEIVED      = (1 << 2),    ///< data received
   SIK_TIMEOUT       = (1 << 3),    ///< timeout occurred
   SIK_SENT          = (1 << 4),    ///< send ok
   SIK_ALL           = 0x1F,        ///< all interrupt
}sockint_kind;

/**
 * @ingroup DATA_TYPE
 * @brief The type of @ref ctlsocket().
 */
typedef enum
{
   CS_SET_IOMODE,          ///< set socket IO mode with @ref SOCK_IO_BLOCK or @ref SOCK_IO_NONBLOCK
   CS_GET_IOMODE,          ///< get socket IO mode
   CS_GET_MAXTXBUF,        ///< get the size of socket buffer allocated in TX memory
   CS_GET_MAXRXBUF,        ///< get the size of socket buffer allocated in RX memory
   CS_CLR_INTERRUPT,       ///< clear the interrupt of socket with @ref sockint_kind
   CS_GET_INTERRUPT,       ///< get the socket interrupt. refer to @ref sockint_kind
   CS_SET_INTMASK,         ///< set the interrupt mask of socket with @ref sockint_kind
   CS_GET_INTMASK          ///< get the masked interrupt of socket. refer to @ref sockint_kind
}ctlsock_type;


/**
 * @ingroup DATA_TYPE
 * @brief The type of socket option in @ref setsockopt() or @ref getsockopt()
 */ 
typedef enum
{
   SO_FLAG,           ///< Valid only in getsockopt(), For set flag of socket refer to <I>flag</I> in @ref socket().
   SO_TTL,              ///< Set/Get TTL. @ref Sn_TTL  ( @ref setSn_TTL(), @ref getSn_TTL() )
   SO_TOS,              ///< Set/Get TOS. @ref Sn_TOS  ( @ref setSn_TOS(), @ref getSn_TOS() )
   SO_MSS,              ///< Set/Get MSS. @ref Sn_MSSR ( @ref setSn_MSSR(), @ref getSn_MSSR() )
   SO_DESTIP,           ///< Set/Get the destination IP address. @ref Sn_DIPR ( @ref setSn_DIPR(), @ref getSn_DIPR() )
   SO_DESTPORT,         ///< Set/Get the destination Port number. @ref Sn_DPORT ( @ref setSn_DPORT(), @ref getSn_DPORT() )
#if _WIZCHIP_ != 5100   
   SO_KEEPALIVESEND,    ///< Valid only in setsockopt. Manually send keep-alive packet in TCP mode
   #if _WIZCHIP_ > 5200   
      SO_KEEPALIVEAUTO, ///< Set/Get keep-alive auto transmission timer in TCP mode
   #endif      
#endif
   SO_SENDBUF,          ///< Valid only in getsockopt. Get the free data size of Socekt TX buffer. @ref Sn_TX_FSR, @ref getSn_TX_FSR()
   SO_RECVBUF,          ///< Valid only in getsockopt. Get the received data size in socket RX buffer. @ref Sn_RX_RSR, @ref getSn_RX_RSR()
   SO_STATUS,           ///< Valid only in getsockopt. Get the socket status. @ref Sn_SR, @ref getSn_SR()
   SO_REMAINSIZE,       ///< Valid only in getsockopt. Get the remained packet size in other then TCP mode.
   SO_PACKINFO          ///< Valid only in getsockopt. Get the packet information as @ref PACK_FIRST, @ref PACK_REMAINED, and @ref PACK_COMPLETED in other then TCP mode.
}sockopt_type;

/**
 * @ingroup WIZnet_socket_APIs
 *  @brief Control socket.
 *  @details Control IO mode, Interrupt & Mask of socket and get the socket buffer information.
 *           Refer to @ref ctlsock_type.
 *  @param sn socket number
 *  @param cstype type of control socket. refer to @ref ctlsock_type.
 *  @param arg Data type and value is determined according to @ref ctlsock_type. \n
 *             <table>
 *                  <tr> <td> @b cstype </td> <td> @b data type</td><td>@b value</td></tr>
 *                  <tr> <td> @ref CS_SET_IOMODE \n @ref CS_GET_IOMODE </td> <td> uint8_t </td><td>@ref SOCK_IO_BLOCK @ref SOCK_IO_NONBLOCK</td></tr>
 *                  <tr> <td> @ref CS_GET_MAXTXBUF \n @ref CS_GET_MAXRXBUF </td> <td> uint16_t </td><td> 0 ~ 16K </td></tr>
 *                  <tr> <td> @ref CS_CLR_INTERRUPT \n @ref CS_GET_INTERRUPT \n @ref CS_SET_INTMASK \n @ref CS_GET_INTMASK </td> <td> @ref sockint_kind </td><td> @ref SIK_CONNECTED, etc.  </td></tr> 
 *             </table>
 *  @return @b Success @ref SOCK_OK \n
 *          @b fail    @ref SOCKERR_ARG         - Invalid argument\n
 */
int8_t  WIZCHIP_EXPORT(ctlsocket)(uint8_t sn, ctlsock_type cstype, void* arg);

/** 
 * @ingroup WIZnet_socket_APIs
 *  @brief set socket options
 *  @details Set socket option like as TTL, MSS, TOS, and so on. Refer to @ref sockopt_type.
 *               
 *  @param sn socket number
 *  @param sotype socket option type. refer to @ref sockopt_type
 *  @param arg Data type and value is determined according to <I>sotype</I>. \n
 *             <table>
 *                  <tr> <td> @b sotype </td> <td> @b data type</td><td>@b value</td></tr> 
 *                  <tr> <td> @ref SO_TTL </td> <td> uint8_t </td><td> 0 ~ 255 </td> </tr>
 *                  <tr> <td> @ref SO_TOS </td> <td> uint8_t </td><td> 0 ~ 255 </td> </tr>
 *                  <tr> <td> @ref SO_MSS </td> <td> uint16_t </td><td> 0 ~ 65535 </td> </tr>
 *                  <tr> <td> @ref SO_DESTIP </td> <td> uint8_t[4] </td><td>  </td></tr> 
 *                  <tr> <td> @ref SO_DESTPORT </td> <td> uint16_t </td><td> 0 ~ 65535 </td></tr> 
 *                  <tr> <td> @ref SO_KEEPALIVESEND </td> <td> null </td><td> null </td></tr> 
 *                  <tr> <td> @ref SO_KEEPALIVEAUTO </td> <td> uint8_t </td><td> 0 ~ 255 </td></tr> 
 *             </table>
 * @return 
 * - @b Success : @ref SOCK_OK \n
 * - @b Fail 
 *  - @ref SOCKERR_SOCKNUM     - Invalid Socket number \n
 *  - @ref SOCKERR_SOCKMODE    - Invalid socket mode \n
 *  - @ref SOCKERR_SOCKOPT     - Invalid socket option or its value \n
 *  - @ref SOCKERR_TIMEOUT     - Timeout occurred when sending keep-alive packet \n
 */
int8_t  WIZCHIP_EXPORT(setsockopt)(uint8_t sn, sockopt_type sotype, void* arg);

/** 
 * @ingroup WIZnet_socket_APIs
 *  @brief get socket options
 *  @details Get socket option like as FLAG, TTL, MSS, and so on. Refer to @ref sockopt_type
 *  @param sn socket number
 *  @param sotype socket option type. refer to @ref sockopt_type
 *  @param arg Data type and value is determined according to <I>sotype</I>. \n
 *             <table>
 *                  <tr> <td> @b sotype </td> <td>@b data type</td><td>@b value</td></tr>
 *                  <tr> <td> @ref SO_FLAG </td> <td> uint8_t </td><td> @ref SF_ETHER_OWN, etc... </td> </tr>
 *                  <tr> <td> @ref SO_TOS </td> <td> uint8_t </td><td> 0 ~ 255 </td> </tr>
 *                  <tr> <td> @ref SO_MSS </td> <td> uint16_t </td><td> 0 ~ 65535 </td> </tr>
 *                  <tr> <td> @ref SO_DESTIP </td> <td> uint8_t[4] </td><td>  </td></tr> 
 *                  <tr> <td> @ref SO_DESTPORT </td> <td> uint16_t </td><td>  </td></tr> 
 *                  <tr> <td> @ref SO_KEEPALIVEAUTO </td> <td> uint8_t </td><td> 0 ~ 255 </td></tr> 
 *                  <tr> <td> @ref SO_SENDBUF </td> <td> uint16_t </td><td> 0 ~ 65535 </td></tr>  
 *                  <tr> <td> @ref SO_RECVBUF </td> <td> uint16_t </td><td> 0 ~ 65535 </td></tr>  
 *                  <tr> <td> @ref SO_STATUS </td> <td> uint8_t </td><td> @ref SOCK_ESTABLISHED, etc.. </td></tr>  
 *                  <tr> <td> @ref SO_REMAINSIZE </td> <td> uint16_t </td><td> 0~ 65535 </td></tr>
 *                  <tr> <td> @ref SO_PACKINFO </td> <td> uint8_t </td><td> @ref PACK_FIRST, etc... </td></tr>
 *             </table>
 * @return 
 * - @b Success : @ref SOCK_OK \n
 * - @b Fail 
 *  - @ref SOCKERR_SOCKNUM     - Invalid Socket number \n
 *  - @ref SOCKERR_SOCKOPT     - Invalid socket option or its value \n
 *  - @ref SOCKERR_SOCKMODE    - Invalid socket mode \n
 * @note
 *   The option as PACK_REMAINED and SO_PACKINFO is valid only in NON-TCP mode and after call @ref recvfrom(). \n
 *   When SO_PACKINFO value is PACK_FIRST and the return value of recvfrom() is zero, 
 *   This means the zero byte UDP data(UDP Header only) received.
  */
int8_t  WIZCHIP_EXPORT(getsockopt)(uint8_t sn, sockopt_type sotype, void* arg);

#endif   // _WIZCHIP_SOCKET_H_
