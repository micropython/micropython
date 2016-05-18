/*****************************************************************************
*
*  socket.h  - CC3000 Host Driver Implementation.
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
#ifndef __CC3000_SOCKET_H__
#define __CC3000_SOCKET_H__

#include "cc3000_common.h"

//*****************************************************************************
//
//! \addtogroup socket_api
//! @{
//
//*****************************************************************************


//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef  __cplusplus
extern "C" {
#endif

#define HOSTNAME_MAX_LENGTH (230)  // 230 bytes + header shouldn't exceed 8 bit value

//--------- Address Families --------

#define  AF_INET                2
#define  AF_INET6               23

//------------ Socket Types ------------

#define  SOCK_STREAM            1
#define  SOCK_DGRAM             2
#define  SOCK_RAW               3           // Raw sockets allow new IPv4 protocols to be implemented in user space. A raw socket receives or sends the raw datagram not including link level headers
#define  SOCK_RDM               4
#define  SOCK_SEQPACKET         5

//----------- Socket Protocol ----------

#define IPPROTO_IP              0           // dummy for IP
#define IPPROTO_ICMP            1           // control message protocol
#define IPPROTO_IPV4            IPPROTO_IP  // IP inside IP
#define IPPROTO_TCP             6           // tcp
#define IPPROTO_UDP             17          // user datagram protocol
#define IPPROTO_IPV6            41          // IPv6 in IPv6
#define IPPROTO_NONE            59          // No next header
#define IPPROTO_RAW             255         // raw IP packet
#define IPPROTO_MAX             256

//----------- Socket retunr codes  -----------

#define SOC_ERROR				(-1)		// error 
#define SOC_IN_PROGRESS			(-2)		// socket in progress

//----------- Socket Options -----------
#define  SOL_SOCKET             0xffff		//  socket level
#define  SOCKOPT_RECV_NONBLOCK         	0	// recv non block mode, set SOCK_ON or SOCK_OFF (default block mode)
#define  SOCKOPT_RECV_TIMEOUT			1	// optname to configure recv and recvfromtimeout
#define  SOCKOPT_ACCEPT_NONBLOCK		2	// accept non block mode, set SOCK_ON or SOCK_OFF (default block mode)
#define  SOCK_ON                0			// socket non-blocking mode	is enabled		
#define  SOCK_OFF               1			// socket blocking mode is enabled

#define  MAX_PACKET_SIZE        1500
#define  MAX_LISTEN_QUEUE       4

#define  IOCTL_SOCKET_EVENTMASK

#define ENOBUFS                 55          // No buffer space available

#define __FD_SETSIZE            32

#define  ASIC_ADDR_LEN          8
	
#define NO_QUERY_RECIVED        -3
	
	
typedef struct _in_addr_t
{
    UINT32 s_addr;                   // load with inet_aton()
} in_addr;

typedef struct _sockaddr_t
{
    UINT16   sa_family;
    UINT8     sa_data[14];
} sockaddr;

typedef struct _sockaddr_in_t
{
    INT16            sin_family;            // e.g. AF_INET
    UINT16   sin_port;              // e.g. htons(3490)
    in_addr          sin_addr;              // see struct in_addr, below
    CHAR             sin_zero[8];           // zero this if you want to
} sockaddr_in;

typedef UINT32 socklen_t;

// The fd_set member is required to be an array of INT32s.
typedef INT32 __fd_mask;

// It's easier to assume 8-bit bytes than to get CHAR_BIT.
#define __NFDBITS               (8 * sizeof (__fd_mask))
#define __FDELT(d)              ((d) / __NFDBITS)
#define __FDMASK(d)             ((__fd_mask) 1 << ((d) % __NFDBITS))

// fd_set for select and pselect.
typedef struct
{
    __fd_mask fds_bits[__FD_SETSIZE / __NFDBITS];
#define __FDS_BITS(set)        ((set)->fds_bits)
} fd_set;

// We don't use `memset' because this would require a prototype and
//   the array isn't too big.
#define __FD_ZERO(set)                               \
  do {                                                \
    UINT16 __i;                                 \
    fd_set *__arr = (set);                            \
    for (__i = 0; __i < sizeof (fd_set) / sizeof (__fd_mask); ++__i) \
      __FDS_BITS (__arr)[__i] = 0;                    \
  } while (0)
#define __FD_SET(d, set)       (__FDS_BITS (set)[__FDELT (d)] |= __FDMASK (d))
#define __FD_CLR(d, set)       (__FDS_BITS (set)[__FDELT (d)] &= ~__FDMASK (d))
#define __FD_ISSET(d, set)     (__FDS_BITS (set)[__FDELT (d)] & __FDMASK (d))

// Access macros for 'fd_set'.
#define FD_SET(fd, fdsetp)      __FD_SET (fd, fdsetp)
#define FD_CLR(fd, fdsetp)      __FD_CLR (fd, fdsetp)
#define FD_ISSET(fd, fdsetp)    __FD_ISSET (fd, fdsetp)
#define FD_ZERO(fdsetp)         __FD_ZERO (fdsetp)

//Use in case of Big Endian only
  
#define htonl(A)    ((((UINT32)(A) & 0xff000000) >> 24) | \
                     (((UINT32)(A) & 0x00ff0000) >> 8) | \
                     (((UINT32)(A) & 0x0000ff00) << 8) | \
                     (((UINT32)(A) & 0x000000ff) << 24))

#define ntohl                   htonl

//Use in case of Big Endian only
#define htons(A)     ((((UINT32)(A) & 0xff00) >> 8) | \
                      (((UINT32)(A) & 0x00ff) << 8))


#define ntohs                   htons

// mDNS port - 5353    mDNS multicast address - 224.0.0.251 
#define SET_mDNS_ADD(sockaddr)     	   	sockaddr.sa_data[0] = 0x14; \
																								sockaddr.sa_data[1] = 0xe9; \
																								sockaddr.sa_data[2] = 0xe0; \
																								sockaddr.sa_data[3] = 0x0; \
																								sockaddr.sa_data[4] = 0x0; \
																								sockaddr.sa_data[5] = 0xfb; 


//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! socket
//!
//!  @param  domain    selects the protocol family which will be used for 
//!                    communication. On this version only AF_INET is supported
//!  @param  type      specifies the communication semantics. On this version 
//!                    only SOCK_STREAM, SOCK_DGRAM, SOCK_RAW are supported
//!  @param  protocol  specifies a particular protocol to be used with the 
//!                    socket IPPROTO_TCP, IPPROTO_UDP or IPPROTO_RAW are 
//!                    supported.
//!
//!  @return  On success, socket handle that is used for consequent socket 
//!           operations. On error, -1 is returned.
//!
//!  @brief  create an endpoint for communication
//!          The socket function creates a socket that is bound to a specific 
//!          transport service provider. This function is called by the 
//!          application layer to obtain a socket handle.
//
//*****************************************************************************
extern INT16 CC3000_EXPORT(socket)(INT32 domain, INT32 type, INT32 protocol);

//*****************************************************************************
//
//! closesocket
//!
//!  @param  sd    socket handle.
//!
//!  @return  On success, zero is returned. On error, -1 is returned.
//!
//!  @brief  The socket function closes a created socket.
//
//*****************************************************************************
extern INT32 CC3000_EXPORT(closesocket)(INT32 sd);

//*****************************************************************************
//
//! accept
//!
//!  @param[in]   sd      socket descriptor (handle)              
//!  @param[out]  addr    the argument addr is a pointer to a sockaddr structure
//!                       This structure is filled in with the address of the  
//!                       peer socket, as known to the communications layer.        
//!                       determined. The exact format of the address returned             
//!                       addr is by the socket's address sockaddr. 
//!                       On this version only AF_INET is supported.
//!                       This argument returns in network order.
//!  @param[out] addrlen  the addrlen argument is a value-result argument: 
//!                       it should initially contain the size of the structure
//!                       pointed to by addr.
//!
//!  @return  For socket in blocking mode:
//!				      On success, socket handle. on failure negative
//!			      For socket in non-blocking mode:
//!				     - On connection establishment, socket handle
//!				     - On connection pending, SOC_IN_PROGRESS (-2)
//!			       - On failure, SOC_ERROR	(-1)
//!
//!  @brief  accept a connection on a socket:
//!          This function is used with connection-based socket types 
//!          (SOCK_STREAM). It extracts the first connection request on the 
//!          queue of pending connections, creates a new connected socket, and
//!          returns a new file descriptor referring to that socket.
//!          The newly created socket is not in the listening state. 
//!          The original socket sd is unaffected by this call. 
//!          The argument sd is a socket that has been created with socket(),
//!          bound to a local address with bind(), and is  listening for 
//!          connections after a listen(). The argument addr is a pointer 
//!          to a sockaddr structure. This structure is filled in with the 
//!          address of the peer socket, as known to the communications layer.
//!          The exact format of the address returned addr is determined by the 
//!          socket's address family. The addrlen argument is a value-result
//!          argument: it should initially contain the size of the structure
//!          pointed to by addr, on return it will contain the actual 
//!          length (in bytes) of the address returned.
//!
//! @sa     socket ; bind ; listen
//
//*****************************************************************************
extern INT32 CC3000_EXPORT(accept)(INT32 sd, sockaddr *addr, socklen_t *addrlen);

//*****************************************************************************
//
//! bind
//!
//!  @param[in]   sd      socket descriptor (handle)              
//!  @param[out]  addr    specifies the destination address. On this version 
//!                       only AF_INET is supported.
//!  @param[out] addrlen  contains the size of the structure pointed to by addr.
//!
//!  @return  	On success, zero is returned. On error, -1 is returned.
//!
//!  @brief  assign a name to a socket
//!          This function gives the socket the local address addr.
//!          addr is addrlen bytes long. Traditionally, this is called when a 
//!          socket is created with socket, it exists in a name space (address 
//!          family) but has no name assigned.
//!          It is necessary to assign a local address before a SOCK_STREAM
//!          socket may receive connections.
//!
//! @sa     socket ; accept ; listen
//
//*****************************************************************************
extern INT32 CC3000_EXPORT(bind)(INT32 sd, const sockaddr *addr, INT32 addrlen);

//*****************************************************************************
//
//! listen
//!
//!  @param[in]   sd      socket descriptor (handle)              
//!  @param[in]  backlog  specifies the listen queue depth. On this version
//!                       backlog is not supported.
//!  @return  	On success, zero is returned. On error, -1 is returned.
//!
//!  @brief  listen for connections on a socket
//!          The willingness to accept incoming connections and a queue
//!          limit for incoming connections are specified with listen(),
//!          and then the connections are accepted with accept.
//!          The listen() call applies only to sockets of type SOCK_STREAM
//!          The backlog parameter defines the maximum length the queue of
//!          pending connections may grow to. 
//!
//! @sa     socket ; accept ; bind
//!
//! @note   On this version, backlog is not supported
//
//*****************************************************************************
extern INT32 CC3000_EXPORT(listen)(INT32 sd, INT32 backlog);

//*****************************************************************************
//
//! gethostbyname
//!
//!  @param[in]   hostname     host name              
//!  @param[in]   usNameLen    name length 
//!  @param[out]  out_ip_addr  This parameter is filled in with host IP address. 
//!                            In case that host name is not resolved, 
//!                            out_ip_addr is zero.                  
//!  @return  	On success, positive is returned. On error, negative is returned
//!
//!  @brief  Get host IP by name. Obtain the IP Address of machine on network, 
//!          by its name.
//!
//!  @note  On this version, only blocking mode is supported. Also note that
//!		     the function requires DNS server to be configured prior to its usage.
//
//*****************************************************************************
#ifndef CC3000_TINY_DRIVER 
extern INT16 CC3000_EXPORT(gethostbyname)(CHAR * hostname, UINT16 usNameLen, UINT32* out_ip_addr);
#endif


//*****************************************************************************
//
//! connect
//!
//!  @param[in]   sd       socket descriptor (handle)         
//!  @param[in]   addr     specifies the destination addr. On this version
//!                        only AF_INET is supported.
//!  @param[out]  addrlen  contains the size of the structure pointed to by addr    
//!  @return  	On success, zero is returned. On error, -1 is returned
//!
//!  @brief  initiate a connection on a socket 
//!          Function connects the socket referred to by the socket descriptor 
//!          sd, to the address specified by addr. The addrlen argument 
//!          specifies the size of addr. The format of the address in addr is 
//!          determined by the address space of the socket. If it is of type 
//!          SOCK_DGRAM, this call specifies the peer with which the socket is 
//!          to be associated; this address is that to which datagrams are to be
//!          sent, and the only address from which datagrams are to be received.  
//!          If the socket is of type SOCK_STREAM, this call attempts to make a 
//!          connection to another socket. The other socket is specified  by 
//!          address, which is an address in the communications space of the
//!          socket. Note that the function implements only blocking behavior 
//!          thus the caller will be waiting either for the connection 
//!          establishment or for the connection establishment failure.
//!
//!  @sa socket
//
//*****************************************************************************
extern INT32 CC3000_EXPORT(connect)(INT32 sd, const sockaddr *addr, INT32 addrlen);

//*****************************************************************************
//
//! select
//!
//!  @param[in]   nfds       the highest-numbered file descriptor in any of the
//!                           three sets, plus 1.     
//!  @param[out]   writesds   socket descriptors list for write monitoring
//!  @param[out]   readsds    socket descriptors list for read monitoring  
//!  @param[out]   exceptsds  socket descriptors list for exception monitoring
//!  @param[in]   timeout     is an upper bound on the amount of time elapsed
//!                           before select() returns. Null means infinity 
//!                           timeout. The minimum timeout is 5 milliseconds,
//!                          less than 5 milliseconds will be set
//!                           automatically to 5 milliseconds.
//!  @return  	On success, select() returns the number of file descriptors
//!             contained in the three returned descriptor sets (that is, the
//!             total number of bits that are set in readfds, writefds,
//!             exceptfds) which may be zero if the timeout expires before
//!             anything interesting  happens.
//!             On error, -1 is returned.
//!                   *readsds - return the sockets on which Read request will
//!                              return without delay with valid data.
//!                   *writesds - return the sockets on which Write request 
//!                                 will return without delay.
//!                   *exceptsds - return the sockets which closed recently.
//!
//!  @brief  Monitor socket activity  
//!          Select allow a program to monitor multiple file descriptors,
//!          waiting until one or more of the file descriptors become 
//!         "ready" for some class of I/O operation 
//!
//!  @Note   If the timeout value set to less than 5ms it will automatically set
//!          to 5ms to prevent overload of the system
//!
//!  @sa socket
//
//*****************************************************************************
extern INT16 CC3000_EXPORT(select)(INT32 nfds, fd_set *readsds, fd_set *writesds,
                  fd_set *exceptsds, struct cc3000_timeval *timeout);

//*****************************************************************************
//
//! setsockopt
//!
//!  @param[in]   sd          socket handle
//!  @param[in]   level       defines the protocol level for this option
//!  @param[in]   optname     defines the option name to Interrogate
//!  @param[in]   optval      specifies a value for the option
//!  @param[in]   optlen      specifies the length of the option value
//!  @return  	On success, zero is returned. On error, -1 is returned
//!
//!  @brief  set socket options
//!          This function manipulate the options associated with a socket.
//!          Options may exist at multiple protocol levels; they are always
//!          present at the uppermost socket level.
//!          When manipulating socket options the level at which the option 
//!          resides and the name of the option must be specified.  
//!          To manipulate options at the socket level, level is specified as 
//!          SOL_SOCKET. To manipulate options at any other level the protocol 
//!          number of the appropriate protocol controlling the option is 
//!          supplied. For example, to indicate that an option is to be 
//!          interpreted by the TCP protocol, level should be set to the 
//!          protocol number of TCP; 
//!          The parameters optval and optlen are used to access optval - 
//!          use for setsockopt(). For getsockopt() they identify a buffer
//!          in which the value for the requested option(s) are to 
//!          be returned. For getsockopt(), optlen is a value-result 
//!          parameter, initially containing the size of the buffer 
//!          pointed to by option_value, and modified on return to 
//!          indicate the actual size of the value returned. If no option 
//!          value is to be supplied or returned, option_value may be NULL.
//!
//!  @Note   On this version the following two socket options are enabled:
//!    			 The only protocol level supported in this version
//!          is SOL_SOCKET (level).
//!		       1. SOCKOPT_RECV_TIMEOUT (optname)
//!			      SOCKOPT_RECV_TIMEOUT configures recv and recvfrom timeout 
//!           in milliseconds.
//!		        In that case optval should be pointer to UINT32.
//!		       2. SOCKOPT_NONBLOCK (optname). sets the socket non-blocking mode on 
//!           or off.
//!		        In that case optval should be SOCK_ON or SOCK_OFF (optval).
//!
//!  @sa getsockopt
//
//*****************************************************************************
#ifndef CC3000_TINY_DRIVER 
extern INT16 CC3000_EXPORT(setsockopt)(INT32 sd, INT32 level, INT32 optname, const void *optval,
                      socklen_t optlen);
#endif
//*****************************************************************************
//
//! getsockopt
//!
//!  @param[in]   sd          socket handle
//!  @param[in]   level       defines the protocol level for this option
//!  @param[in]   optname     defines the option name to Interrogate
//!  @param[out]   optval      specifies a value for the option
//!  @param[out]   optlen      specifies the length of the option value
//!  @return  	On success, zero is returned. On error, -1 is returned
//!
//!  @brief  set socket options
//!          This function manipulate the options associated with a socket.
//!          Options may exist at multiple protocol levels; they are always
//!          present at the uppermost socket level.
//!          When manipulating socket options the level at which the option 
//!          resides and the name of the option must be specified.  
//!          To manipulate options at the socket level, level is specified as 
//!          SOL_SOCKET. To manipulate options at any other level the protocol 
//!          number of the appropriate protocol controlling the option is 
//!          supplied. For example, to indicate that an option is to be 
//!          interpreted by the TCP protocol, level should be set to the 
//!          protocol number of TCP; 
//!          The parameters optval and optlen are used to access optval - 
//!          use for setsockopt(). For getsockopt() they identify a buffer
//!          in which the value for the requested option(s) are to 
//!          be returned. For getsockopt(), optlen is a value-result 
//!          parameter, initially containing the size of the buffer 
//!          pointed to by option_value, and modified on return to 
//!          indicate the actual size of the value returned. If no option 
//!          value is to be supplied or returned, option_value may be NULL.
//!
//!  @Note   On this version the following two socket options are enabled:
//!    			 The only protocol level supported in this version
//!          is SOL_SOCKET (level).
//!		       1. SOCKOPT_RECV_TIMEOUT (optname)
//!			      SOCKOPT_RECV_TIMEOUT configures recv and recvfrom timeout 
//!           in milliseconds.
//!		        In that case optval should be pointer to UINT32.
//!		       2. SOCKOPT_NONBLOCK (optname). sets the socket non-blocking mode on 
//!           or off.
//!		        In that case optval should be SOCK_ON or SOCK_OFF (optval).
//!
//!  @sa setsockopt
//
//*****************************************************************************
extern INT16 CC3000_EXPORT(getsockopt)(INT32 sd, INT32 level, INT32 optname, void *optval,
                      socklen_t *optlen);

//*****************************************************************************
//
//!  recv
//!
//!  @param[in]  sd     socket handle
//!  @param[out] buf    Points to the buffer where the message should be stored
//!  @param[in]  len    Specifies the length in bytes of the buffer pointed to 
//!                     by the buffer argument.
//!  @param[in] flags   Specifies the type of message reception. 
//!                     On this version, this parameter is not supported.
//!
//!  @return         Return the number of bytes received, or -1 if an error
//!                  occurred
//!
//!  @brief          function receives a message from a connection-mode socket
//!
//!  @sa recvfrom
//!
//!  @Note On this version, only blocking mode is supported.
//
//*****************************************************************************
extern INT16 CC3000_EXPORT(recv)(INT32 sd, void *buf, INT32 len, INT32 flags);

//*****************************************************************************
//
//!  recvfrom
//!
//!  @param[in]  sd     socket handle
//!  @param[out] buf    Points to the buffer where the message should be stored
//!  @param[in]  len    Specifies the length in bytes of the buffer pointed to 
//!                     by the buffer argument.
//!  @param[in] flags   Specifies the type of message reception. 
//!                     On this version, this parameter is not supported.
//!  @param[in] from   pointer to an address structure indicating the source
//!                    address: sockaddr. On this version only AF_INET is
//!                    supported.
//!  @param[in] fromlen   source address structure size
//!
//!  @return         Return the number of bytes received, or -1 if an error
//!                  occurred
//!
//!  @brief         read data from socket
//!                 function receives a message from a connection-mode or
//!                 connectionless-mode socket. Note that raw sockets are not
//!                 supported.
//!
//!  @sa recv
//!
//!  @Note On this version, only blocking mode is supported.
//
//*****************************************************************************
extern INT16 CC3000_EXPORT(recvfrom)(INT32 sd, void *buf, INT32 len, INT32 flags, sockaddr *from, 
                    socklen_t *fromlen);

//*****************************************************************************
//
//!  send
//!
//!  @param sd       socket handle
//!  @param buf      Points to a buffer containing the message to be sent
//!  @param len      message size in bytes
//!  @param flags    On this version, this parameter is not supported
//!
//!  @return         Return the number of bytes transmitted, or -1 if an
//!                  error occurred
//!
//!  @brief          Write data to TCP socket
//!                  This function is used to transmit a message to another 
//!                  socket.
//!
//!  @Note           On this version, only blocking mode is supported.
//!
//!  @sa             sendto
//
//*****************************************************************************

extern INT16 CC3000_EXPORT(send)(INT32 sd, const void *buf, INT32 len, INT32 flags);

//*****************************************************************************
//
//!  sendto
//!
//!  @param sd       socket handle
//!  @param buf      Points to a buffer containing the message to be sent
//!  @param len      message size in bytes
//!  @param flags    On this version, this parameter is not supported
//!  @param to       pointer to an address structure indicating the destination
//!                  address: sockaddr. On this version only AF_INET is
//!                  supported.
//!  @param tolen    destination address structure size
//!
//!  @return         Return the number of bytes transmitted, or -1 if an
//!                  error occurred
//!
//!  @brief          Write data to TCP socket
//!                  This function is used to transmit a message to another 
//!                  socket.
//!
//!  @Note           On this version, only blocking mode is supported.
//!
//!  @sa             send
//
//*****************************************************************************

extern INT16 CC3000_EXPORT(sendto)(INT32 sd, const void *buf, INT32 len, INT32 flags, 
                  const sockaddr *to, socklen_t tolen);

//*****************************************************************************
//
//!  mdnsAdvertiser
//!
//!  @param[in] mdnsEnabled         flag to enable/disable the mDNS feature
//!  @param[in] deviceServiceName   Service name as part of the published
//!                                 canonical domain name
//!  @param[in] deviceServiceNameLength   Length of the service name - up to 32 chars
//!  
//!
//!  @return   On success, zero is returned, return SOC_ERROR if socket was not 
//!            opened successfully, or if an error occurred.
//!
//!  @brief    Set CC3000 in mDNS advertiser mode in order to advertise itself.
//
//*****************************************************************************
extern INT16 CC3000_EXPORT(mdnsAdvertiser)(UINT16 mdnsEnabled, CHAR * deviceServiceName, UINT16 deviceServiceNameLength);


//*****************************************************************************
//
//!  getmssvalue
//!
//!  @param[in] sd         socket descriptor
//!
//!  @return   On success, returns the MSS value of a TCP connection
//!
//!  @brief    Returns the MSS value of a TCP connection according to the socket descriptor
//
//*****************************************************************************
extern UINT16 CC3000_EXPORT(getmssvalue) (INT32 sd);

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
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __SOCKET_H__
