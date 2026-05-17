/*
 * socket.h - CC31xx/CC32xx Host Driver Implementation
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
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
*/
 
/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "simplelink.h"

#ifndef __SL_SOCKET_H__
#define __SL_SOCKET_H__




#ifdef    __cplusplus
extern "C" {
#endif

/*!

    \addtogroup socket
    @{

*/

/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/

#define SL_FD_SETSIZE                         SL_MAX_SOCKETS         /* Number of sockets to select on - same is max sockets!               */
#define BSD_SOCKET_ID_MASK                     (0x0F)                 /* Index using the LBS 4 bits for socket id 0-7 */
/* Define some BSD protocol constants.  */
#define SL_SOCK_STREAM                         (1)                       /* TCP Socket                                                          */
#define SL_SOCK_DGRAM                          (2)                       /* UDP Socket                                                          */
#define SL_SOCK_RAW                            (3)                       /* Raw socket                                                          */
#define SL_IPPROTO_TCP                         (6)                       /* TCP Raw Socket                                                      */
#define SL_IPPROTO_UDP                         (17)                      /* UDP Raw Socket                                                      */
#define SL_IPPROTO_RAW                         (255)                     /* Raw Socket                                                          */
#define SL_SEC_SOCKET                          (100)                     /* Secured Socket Layer (SSL,TLS)                                      */

/* Address families.  */
#define     SL_AF_INET                         (2)                       /* IPv4 socket (UDP, TCP, etc)                                          */
#define     SL_AF_INET6                        (3)                       /* IPv6 socket (UDP, TCP, etc)                                          */
#define     SL_AF_INET6_EUI_48                 (9)
#define     SL_AF_RF                           (6)                       /* data include RF parameter, All layer by user (Wifi could be disconnected) */ 
#define     SL_AF_PACKET                       (17)
/* Protocol families, same as address families.  */
#define     SL_PF_INET                         AF_INET
#define     SL_PF_INET6                        AF_INET6
#define     SL_INADDR_ANY                      (0)                       /*  bind any address  */

/* error codes */
#define SL_SOC_ERROR                          (-1)  /* Failure.                                                             */
#define SL_SOC_OK                             ( 0)  /* Success.                                                             */
#define SL_INEXE                              (-8)   /* socket command in execution  */
#define SL_EBADF                              (-9)   /* Bad file number */
#define SL_ENSOCK                             (-10)  /* The system limit on the total number of open socket, has been reached */
#define SL_EAGAIN                             (-11)  /* Try again */
#define SL_EWOULDBLOCK                        SL_EAGAIN
#define SL_ENOMEM                             (-12)  /* Out of memory */
#define SL_EACCES                             (-13)  /* Permission denied */
#define SL_EFAULT                             (-14)  /* Bad address */
#define SL_ECLOSE                             (-15)  /* close socket operation failed to transmit all queued packets */
#define SL_EALREADY_ENABLED                   (-21)  /* Transceiver - Transceiver already ON. there could be only one */
#define SL_EINVAL                             (-22)  /* Invalid argument */
#define SL_EAUTO_CONNECT_OR_CONNECTING        (-69)  /* Transceiver - During connection, connected or auto mode started */
#define SL_CONNECTION_PENDING                  (-72)  /* Transceiver - Device is connected, disconnect first to open transceiver */
#define SL_EUNSUPPORTED_ROLE                  (-86)  /* Transceiver - Trying to start when WLAN role is AP or P2P GO */
#define SL_EDESTADDRREQ                       (-89)  /* Destination address required */
#define SL_EPROTOTYPE                         (-91)  /* Protocol wrong type for socket */
#define SL_ENOPROTOOPT                        (-92)  /* Protocol not available */
#define SL_EPROTONOSUPPORT                    (-93)  /* Protocol not supported */
#define SL_ESOCKTNOSUPPORT                    (-94)  /* Socket type not supported */
#define SL_EOPNOTSUPP                         (-95)  /* Operation not supported on transport endpoint */
#define SL_EAFNOSUPPORT                       (-97)  /* Address family not supported by protocol */
#define SL_EADDRINUSE                         (-98)  /* Address already in use */
#define SL_EADDRNOTAVAIL                      (-99)  /* Cannot assign requested address */
#define SL_ENETUNREACH                        (-101) /* Network is unreachable */
#define SL_ENOBUFS                            (-105) /* No buffer space available */
#define SL_EOBUFF                             SL_ENOBUFS 
#define SL_EISCONN                            (-106) /* Transport endpoint is already connected */
#define SL_ENOTCONN                           (-107) /* Transport endpoint is not connected */
#define SL_ETIMEDOUT                          (-110) /* Connection timed out */
#define SL_ECONNREFUSED                       (-111) /* Connection refused */
#define SL_EALREADY                           (-114) /* Non blocking connect in progress, try again */ 

#define SL_ESEC_RSA_WRONG_TYPE_E              (-130)  /* RSA wrong block type for RSA function */
#define SL_ESEC_RSA_BUFFER_E                  (-131)  /* RSA buffer error, output too small or */
#define SL_ESEC_BUFFER_E                      (-132)  /* output buffer too small or input too large */
#define SL_ESEC_ALGO_ID_E                     (-133)  /* setting algo id error */
#define SL_ESEC_PUBLIC_KEY_E                  (-134)  /* setting public key error */
#define SL_ESEC_DATE_E                        (-135)  /* setting date validity error */
#define SL_ESEC_SUBJECT_E                     (-136)  /* setting subject name error */
#define SL_ESEC_ISSUER_E                      (-137)  /* setting issuer  name error */
#define SL_ESEC_CA_TRUE_E                     (-138)  /* setting CA basic constraint true error */
#define SL_ESEC_EXTENSIONS_E                  (-139)  /* setting extensions error */
#define SL_ESEC_ASN_PARSE_E                   (-140)  /* ASN parsing error, invalid input */
#define SL_ESEC_ASN_VERSION_E                 (-141)  /* ASN version error, invalid number */
#define SL_ESEC_ASN_GETINT_E                  (-142)  /* ASN get big _i16 error, invalid data */
#define SL_ESEC_ASN_RSA_KEY_E                 (-143)  /* ASN key init error, invalid input */
#define SL_ESEC_ASN_OBJECT_ID_E               (-144)  /* ASN object id error, invalid id */
#define SL_ESEC_ASN_TAG_NULL_E                (-145)  /* ASN tag error, not null */
#define SL_ESEC_ASN_EXPECT_0_E                (-146)  /* ASN expect error, not zero */
#define SL_ESEC_ASN_BITSTR_E                  (-147)  /* ASN bit string error, wrong id */
#define SL_ESEC_ASN_UNKNOWN_OID_E             (-148)  /* ASN oid error, unknown sum id */
#define SL_ESEC_ASN_DATE_SZ_E                 (-149)  /* ASN date error, bad size */
#define SL_ESEC_ASN_BEFORE_DATE_E             (-150)  /* ASN date error, current date before */
#define SL_ESEC_ASN_AFTER_DATE_E              (-151)  /* ASN date error, current date after */
#define SL_ESEC_ASN_SIG_OID_E                 (-152)  /* ASN signature error, mismatched oid */
#define SL_ESEC_ASN_TIME_E                    (-153)  /* ASN time error, unknown time type */
#define SL_ESEC_ASN_INPUT_E                   (-154)  /* ASN input error, not enough data */
#define SL_ESEC_ASN_SIG_CONFIRM_E             (-155)  /* ASN sig error, confirm failure */
#define SL_ESEC_ASN_SIG_HASH_E                (-156)  /* ASN sig error, unsupported hash type */
#define SL_ESEC_ASN_SIG_KEY_E                 (-157)  /* ASN sig error, unsupported key type */
#define SL_ESEC_ASN_DH_KEY_E                  (-158)  /* ASN key init error, invalid input */
#define SL_ESEC_ASN_NTRU_KEY_E                (-159)  /* ASN ntru key decode error, invalid input */
#define SL_ESEC_ECC_BAD_ARG_E                 (-170)  /* ECC input argument of wrong type */
#define SL_ESEC_ASN_ECC_KEY_E                 (-171)  /* ASN ECC bad input */
#define SL_ESEC_ECC_CURVE_OID_E               (-172)  /* Unsupported ECC OID curve type */
#define SL_ESEC_BAD_FUNC_ARG                  (-173)  /* Bad function argument provided */
#define SL_ESEC_NOT_COMPILED_IN               (-174)  /* Feature not compiled in */
#define SL_ESEC_UNICODE_SIZE_E                (-175)  /* Unicode password too big */
#define SL_ESEC_NO_PASSWORD                   (-176)  /* no password provided by user */
#define SL_ESEC_ALT_NAME_E                    (-177)  /* alt name size problem, too big */
#define SL_ESEC_AES_GCM_AUTH_E                (-180)  /* AES-GCM Authentication check failure */
#define SL_ESEC_AES_CCM_AUTH_E                (-181)  /* AES-CCM Authentication check failure */
#define SL_SOCKET_ERROR_E                     (-208)  /* Error state on socket */

#define SL_ESEC_MEMORY_ERROR                  (-203)  /* out of memory            */
#define SL_ESEC_VERIFY_FINISHED_ERROR         (-204)  /* verify problem on finished */
#define SL_ESEC_VERIFY_MAC_ERROR              (-205)  /* verify mac problem       */
#define SL_ESEC_UNKNOWN_HANDSHAKE_TYPE        (-207)  /* weird handshake type     */
#define SL_ESEC_SOCKET_ERROR_E                (-208)  /* error state on socket    */
#define SL_ESEC_SOCKET_NODATA                 (-209)  /* expected data, not there */
#define SL_ESEC_INCOMPLETE_DATA               (-210)  /* don't have enough data to complete task            */
#define SL_ESEC_UNKNOWN_RECORD_TYPE           (-211)  /* unknown type in record hdr */
#define SL_ESEC_FATAL_ERROR                   (-213)  /* recvd alert fatal error  */
#define SL_ESEC_ENCRYPT_ERROR                 (-214)  /* error during encryption  */
#define SL_ESEC_NO_PEER_KEY                   (-216)  /* need peer's key          */
#define SL_ESEC_NO_PRIVATE_KEY                (-217)  /* need the private key     */
#define SL_ESEC_RSA_PRIVATE_ERROR             (-218)  /* error during rsa priv op */
#define SL_ESEC_NO_DH_PARAMS                  (-219)  /* server missing DH params */
#define SL_ESEC_BUILD_MSG_ERROR               (-220)  /* build message failure    */
#define SL_ESEC_BAD_HELLO                     (-221)  /* client hello malformed   */
#define SL_ESEC_DOMAIN_NAME_MISMATCH          (-222)  /* peer subject name mismatch */
#define SL_ESEC_WANT_READ                     (-223)  /* want read, call again    */
#define SL_ESEC_NOT_READY_ERROR               (-224)  /* handshake layer not ready */
#define SL_ESEC_PMS_VERSION_ERROR             (-225)  /* pre m secret version error */
#define SL_ESEC_VERSION_ERROR                 (-226)  /* record layer version error */
#define SL_ESEC_WANT_WRITE                    (-227)  /* want write, call again   */
#define SL_ESEC_BUFFER_ERROR                  (-228)  /* malformed buffer input   */
#define SL_ESEC_VERIFY_CERT_ERROR             (-229)  /* verify cert error        */
#define SL_ESEC_VERIFY_SIGN_ERROR             (-230)  /* verify sign error        */

#define SL_ESEC_LENGTH_ERROR                  (-241)  /* record layer length error */
#define SL_ESEC_PEER_KEY_ERROR                (-242)  /* can't decode peer key */
#define SL_ESEC_ZERO_RETURN                   (-243)  /* peer sent close notify */
#define SL_ESEC_SIDE_ERROR                    (-244)  /* wrong client/server type */
#define SL_ESEC_NO_PEER_CERT                  (-245)  /* peer didn't send key */
#define SL_ESEC_ECC_CURVETYPE_ERROR           (-250)  /* Bad ECC Curve Type */
#define SL_ESEC_ECC_CURVE_ERROR               (-251)  /* Bad ECC Curve */
#define SL_ESEC_ECC_PEERKEY_ERROR             (-252)  /* Bad Peer ECC Key */
#define SL_ESEC_ECC_MAKEKEY_ERROR             (-253)  /* Bad Make ECC Key */
#define SL_ESEC_ECC_EXPORT_ERROR              (-254)  /* Bad ECC Export Key */
#define SL_ESEC_ECC_SHARED_ERROR              (-255)  /* Bad ECC Shared Secret */
#define SL_ESEC_NOT_CA_ERROR                  (-257)  /* Not a CA cert error */
#define SL_ESEC_BAD_PATH_ERROR                (-258)  /* Bad path for opendir */
#define SL_ESEC_BAD_CERT_MANAGER_ERROR        (-259)  /* Bad Cert Manager */
#define SL_ESEC_MAX_CHAIN_ERROR               (-268)  /* max chain depth exceeded */
#define SL_ESEC_SUITES_ERROR                  (-271)  /* suites pointer error */
#define SL_ESEC_SSL_NO_PEM_HEADER             (-272)  /* no PEM header found */
#define SL_ESEC_OUT_OF_ORDER_E                (-273)  /* out of order message */
#define SL_ESEC_SANITY_CIPHER_E               (-275)  /* sanity check on cipher error */
#define SL_ESEC_GEN_COOKIE_E                  (-277)  /* Generate Cookie Error */
#define SL_ESEC_NO_PEER_VERIFY                (-278)  /* Need peer cert verify Error */
#define SL_ESEC_UNKNOWN_SNI_HOST_NAME_E       (-281)  /* Unrecognized host name Error */
/* begin negotiation parameter errors */
#define SL_ESEC_UNSUPPORTED_SUITE     (-290)            /* unsupported cipher suite */
#define SL_ESEC_MATCH_SUITE_ERROR      (-291 )            /* can't match cipher suite */

/* ssl tls security start with -300 offset */
#define SL_ESEC_CLOSE_NOTIFY                  (-300) /* ssl/tls alerts */   
#define SL_ESEC_UNEXPECTED_MESSAGE            (-310) /* ssl/tls alerts */   
#define SL_ESEC_BAD_RECORD_MAC                (-320) /* ssl/tls alerts */                 
#define SL_ESEC_DECRYPTION_FAILED             (-321) /* ssl/tls alerts */   
#define SL_ESEC_RECORD_OVERFLOW               (-322) /* ssl/tls alerts */    
#define SL_ESEC_DECOMPRESSION_FAILURE         (-330) /* ssl/tls alerts */                 
#define SL_ESEC_HANDSHAKE_FAILURE             (-340) /* ssl/tls alerts */    
#define SL_ESEC_NO_CERTIFICATE                (-341) /* ssl/tls alerts */    
#define SL_ESEC_BAD_CERTIFICATE               (-342) /* ssl/tls alerts */          
#define SL_ESEC_UNSUPPORTED_CERTIFICATE       (-343) /* ssl/tls alerts */     
#define SL_ESEC_CERTIFICATE_REVOKED           (-344) /* ssl/tls alerts */                 
#define SL_ESEC_CERTIFICATE_EXPIRED           (-345) /* ssl/tls alerts */                 
#define SL_ESEC_CERTIFICATE_UNKNOWN           (-346) /* ssl/tls alerts */                 
#define SL_ESEC_ILLEGAL_PARAMETER             (-347) /* ssl/tls alerts */                 
#define SL_ESEC_UNKNOWN_CA                    (-348) /* ssl/tls alerts */                
#define SL_ESEC_ACCESS_DENIED                 (-349) /* ssl/tls alerts */                
#define SL_ESEC_DECODE_ERROR                  (-350) /* ssl/tls alerts */   
#define SL_ESEC_DECRYPT_ERROR                 (-351) /* ssl/tls alerts */   
#define SL_ESEC_EXPORT_RESTRICTION            (-360) /* ssl/tls alerts */    
#define SL_ESEC_PROTOCOL_VERSION              (-370) /* ssl/tls alerts */    
#define SL_ESEC_INSUFFICIENT_SECURITY         (-371) /* ssl/tls alerts */   
#define SL_ESEC_INTERNAL_ERROR                (-380) /* ssl/tls alerts */   
#define SL_ESEC_USER_CANCELLED                (-390) /* ssl/tls alerts */   
#define SL_ESEC_NO_RENEGOTIATION              (-400) /* ssl/tls alerts */   
#define SL_ESEC_UNSUPPORTED_EXTENSION         (-410) /* ssl/tls alerts */   
#define SL_ESEC_CERTIFICATE_UNOBTAINABLE      (-411) /* ssl/tls alerts */         
#define SL_ESEC_UNRECOGNIZED_NAME             (-412) /* ssl/tls alerts */   
#define SL_ESEC_BAD_CERTIFICATE_STATUS_RESPONSE  (-413) /* ssl/tls alerts */   
#define SL_ESEC_BAD_CERTIFICATE_HASH_VALUE    (-414) /* ssl/tls alerts */   
/* propierty secure */
#define SL_ESECGENERAL                        (-450)  /* error secure level general error */
#define SL_ESECDECRYPT                        (-451)  /* error secure level, decrypt recv packet fail */
#define SL_ESECCLOSED                         (-452)  /* secure layrer is closed by other size , tcp is still connected  */
#define SL_ESECSNOVERIFY                      (-453)  /* Connected without server verification */
#define SL_ESECNOCAFILE                       (-454)  /* error secure level CA file not found*/
#define SL_ESECMEMORY                         (-455)  /* error secure level No memory  space available */
#define SL_ESECBADCAFILE                      (-456)  /* error secure level bad CA file */
#define SL_ESECBADCERTFILE                    (-457)  /* error secure level bad Certificate file */
#define SL_ESECBADPRIVATEFILE                 (-458)  /* error secure level bad private file */
#define SL_ESECBADDHFILE                      (-459)  /* error secure level bad DH file */
#define SL_ESECT00MANYSSLOPENED               (-460)  /* MAX SSL Sockets are opened */
#define SL_ESECDATEERROR                      (-461)  /* connected with certificate date verification error */
#define SL_ESECHANDSHAKETIMEDOUT              (-462)  /* connection timed out due to handshake time */

/* end error codes */

/* Max payload size by protocol */
#define SL_SOCKET_PAYLOAD_TYPE_MASK            (0xF0)  /*4 bits type, 4 bits sockets id */
#define SL_SOCKET_PAYLOAD_TYPE_UDP_IPV4        (0x00)  /* 1472 bytes */
#define SL_SOCKET_PAYLOAD_TYPE_TCP_IPV4        (0x10)  /* 1460 bytes */
#define SL_SOCKET_PAYLOAD_TYPE_UDP_IPV6        (0x20)  /* 1452 bytes */
#define SL_SOCKET_PAYLOAD_TYPE_TCP_IPV6        (0x30)  /* 1440 bytes */
#define SL_SOCKET_PAYLOAD_TYPE_UDP_IPV4_SECURE (0x40)  /*            */
#define SL_SOCKET_PAYLOAD_TYPE_TCP_IPV4_SECURE (0x50)  /*            */
#define SL_SOCKET_PAYLOAD_TYPE_UDP_IPV6_SECURE (0x60)  /*            */
#define SL_SOCKET_PAYLOAD_TYPE_TCP_IPV6_SECURE (0x70)  /*            */
#define SL_SOCKET_PAYLOAD_TYPE_RAW_TRANCEIVER  (0x80)  /* 1536 bytes */
#define SL_SOCKET_PAYLOAD_TYPE_RAW_PACKET      (0x90)  /* 1536 bytes */
#define SL_SOCKET_PAYLOAD_TYPE_RAW_IP4         (0xa0)  
#define SL_SOCKET_PAYLOAD_TYPE_RAW_IP6         (SL_SOCKET_PAYLOAD_TYPE_RAW_IP4 )  

  

#define SL_SOL_SOCKET          (1)   /* Define the socket option category. */
#define SL_IPPROTO_IP          (2)   /* Define the IP option category.     */
#define SL_SOL_PHY_OPT         (3)   /* Define the PHY option category.    */

#define SL_SO_RCVBUF           (8)   /* Setting TCP receive buffer size */
#define SL_SO_KEEPALIVE        (9)   /* Connections are kept alive with periodic messages */
#define SL_SO_RCVTIMEO         (20)  /* Enable receive timeout */
#define SL_SO_NONBLOCKING      (24)  /* Enable . disable nonblocking mode  */
#define SL_SO_SECMETHOD        (25)  /* security metohd */
#define SL_SO_SECURE_MASK      (26)  /* security mask */
#define SL_SO_SECURE_FILES     (27)  /* security files */
#define SL_SO_CHANGE_CHANNEL   (28)  /* This option is available only when transceiver started */
#define SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME (30) /* This option used to configue secure file */
#define SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME (31) /* This option used to configue secure file */
#define SL_SO_SECURE_FILES_CA_FILE_NAME          (32) /* This option used to configue secure file */
#define SL_SO_SECURE_FILES_DH_KEY_FILE_NAME      (33) /* This option used to configue secure file */

#define SL_IP_MULTICAST_IF     (60) /* Specify outgoing multicast interface */
#define SL_IP_MULTICAST_TTL    (61) /* Specify the TTL value to use for outgoing multicast packet. */
#define SL_IP_ADD_MEMBERSHIP   (65) /* Join IPv4 multicast membership */
#define SL_IP_DROP_MEMBERSHIP  (66) /* Leave IPv4 multicast membership */
#define SL_IP_HDRINCL          (67) /* Raw socket IPv4 header included. */
#define SL_IP_RAW_RX_NO_HEADER (68) /* Proprietary socket option that does not includeIPv4/IPv6 header (and extension headers) on received raw sockets*/
#define SL_IP_RAW_IPV6_HDRINCL (69) /* Transmitted buffer over IPv6 socket contains IPv6 header. */

#define SL_SO_PHY_RATE              (100)   /* WLAN Transmit rate */
#define SL_SO_PHY_TX_POWER          (101)   /* TX Power level */  
#define SL_SO_PHY_NUM_FRAMES_TO_TX  (102)   /* Number of frames to transmit */
#define SL_SO_PHY_PREAMBLE          (103)   /* Preamble for transmission */

#define SL_SO_SEC_METHOD_SSLV3                             (0)  /* security metohd SSL v3*/
#define SL_SO_SEC_METHOD_TLSV1                             (1)  /* security metohd TLS v1*/
#define SL_SO_SEC_METHOD_TLSV1_1                           (2)  /* security metohd TLS v1_1*/
#define SL_SO_SEC_METHOD_TLSV1_2                           (3)  /* security metohd TLS v1_2*/
#define SL_SO_SEC_METHOD_SSLv3_TLSV1_2                     (4)  /* use highest possible version from SSLv3 - TLS 1.2*/
#define SL_SO_SEC_METHOD_DLSV1                             (5)  /* security metohd DTL v1  */

#define SL_SEC_MASK_SSL_RSA_WITH_RC4_128_SHA               (1 << 0)
#define SL_SEC_MASK_SSL_RSA_WITH_RC4_128_MD5               (1 << 1)
#define SL_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA           (1 << 2)
#define SL_SEC_MASK_TLS_DHE_RSA_WITH_AES_256_CBC_SHA       (1 << 3)
#define SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA     (1 << 4)
#define SL_SEC_MASK_TLS_ECDHE_RSA_WITH_RC4_128_SHA         (1 << 5)
#define SL_SEC_MASK_TLS_RSA_WITH_AES_128_CBC_SHA256              (1 << 6)
#define SL_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA256              (1 << 7)
#define SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256        (1 << 8)
#define SL_SEC_MASK_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256      (1 << 9)


#define SL_SEC_MASK_SECURE_DEFAULT                         ((SL_SEC_MASK_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256  <<  1)  -  1)

#define SL_MSG_DONTWAIT                                   (0x00000008)  /* Nonblocking IO */

/* AP DHCP Server - IP Release reason code */
#define SL_IP_LEASE_PEER_RELEASE     (0)
#define SL_IP_LEASE_PEER_DECLINE     (1)
#define SL_IP_LEASE_EXPIRED          (2)

/* possible types when receiving SL_SOCKET_ASYNC_EVENT*/
#define SSL_ACCEPT                                (1) /* accept failed due to ssl issue ( tcp pass) */
#define RX_FRAGMENTATION_TOO_BIG                  (2) /* connection less mode, rx packet fragmentation > 16K, packet is being released */
#define OTHER_SIDE_CLOSE_SSL_DATA_NOT_ENCRYPTED   (3) /* remote side down from secure to unsecure */



#ifdef SL_INC_STD_BSD_API_NAMING

#define FD_SETSIZE                          SL_FD_SETSIZE        
                                                                       
#define SOCK_STREAM                         SL_SOCK_STREAM        
#define SOCK_DGRAM                          SL_SOCK_DGRAM         
#define SOCK_RAW                            SL_SOCK_RAW           
#define IPPROTO_TCP                         SL_IPPROTO_TCP        
#define IPPROTO_UDP                         SL_IPPROTO_UDP        
#define IPPROTO_RAW                         SL_IPPROTO_RAW        
                                                                       
#define AF_INET                             SL_AF_INET            
#define AF_INET6                            SL_AF_INET6           
#define AF_INET6_EUI_48                     SL_AF_INET6_EUI_48
#define AF_RF                               SL_AF_RF              
#define AF_PACKET                           SL_AF_PACKET              
                                                                       
#define PF_INET                             SL_PF_INET            
#define PF_INET6                            SL_PF_INET6           
                                                                       
#define INADDR_ANY                          SL_INADDR_ANY                                                   
#define ERROR                               SL_SOC_ERROR                                                                                                                
#define INEXE                               SL_INEXE                 
#define EBADF                               SL_EBADF                 
#define ENSOCK                              SL_ENSOCK                
#define EAGAIN                              SL_EAGAIN                
#define EWOULDBLOCK                         SL_EWOULDBLOCK           
#define ENOMEM                              SL_ENOMEM                
#define EACCES                              SL_EACCES                
#define EFAULT                              SL_EFAULT                
#define EINVAL                              SL_EINVAL                
#define EDESTADDRREQ                        SL_EDESTADDRREQ          
#define EPROTOTYPE                          SL_EPROTOTYPE            
#define ENOPROTOOPT                         SL_ENOPROTOOPT           
#define EPROTONOSUPPORT                     SL_EPROTONOSUPPORT       
#define ESOCKTNOSUPPORT                     SL_ESOCKTNOSUPPORT       
#define EOPNOTSUPP                          SL_EOPNOTSUPP            
#define EAFNOSUPPORT                        SL_EAFNOSUPPORT          
#define EADDRINUSE                          SL_EADDRINUSE            
#define EADDRNOTAVAIL                       SL_EADDRNOTAVAIL         
#define ENETUNREACH                         SL_ENETUNREACH           
#define ENOBUFS                             SL_ENOBUFS               
#define EOBUFF                              SL_EOBUFF                
#define EISCONN                             SL_EISCONN               
#define ENOTCONN                            SL_ENOTCONN              
#define ETIMEDOUT                           SL_ETIMEDOUT             
#define ECONNREFUSED                        SL_ECONNREFUSED          

#define SOL_SOCKET                          SL_SOL_SOCKET         
#define IPPROTO_IP                          SL_IPPROTO_IP                     
#define SO_KEEPALIVE                        SL_SO_KEEPALIVE            
                                                                       
#define SO_RCVTIMEO                         SL_SO_RCVTIMEO        
#define SO_NONBLOCKING                      SL_SO_NONBLOCKING     
                                                                       
#define IP_MULTICAST_IF                     SL_IP_MULTICAST_IF    
#define IP_MULTICAST_TTL                    SL_IP_MULTICAST_TTL   
#define IP_ADD_MEMBERSHIP                   SL_IP_ADD_MEMBERSHIP  
#define IP_DROP_MEMBERSHIP                  SL_IP_DROP_MEMBERSHIP 
                                                                       
#define socklen_t                           SlSocklen_t
#define timeval                             SlTimeval_t
#define sockaddr                            SlSockAddr_t
#define in6_addr                            SlIn6Addr_t
#define sockaddr_in6                        SlSockAddrIn6_t
#define in_addr                             SlInAddr_t
#define sockaddr_in                         SlSockAddrIn_t
                                                                       
#define MSG_DONTWAIT                        SL_MSG_DONTWAIT       
                                                                       
#define FD_SET                              SL_FD_SET  
#define FD_CLR                              SL_FD_CLR  
#define FD_ISSET                            SL_FD_ISSET
#define FD_ZERO                             SL_FD_ZERO 
#define fd_set                              SlFdSet_t    

#define socket                              sl_Socket
#define close                               sl_Close
#define accept                              sl_Accept
#define bind                                sl_Bind
#define listen                              sl_Listen
#define connect                             sl_Connect
#define select                              sl_Select
#define setsockopt                          sl_SetSockOpt
#define getsockopt                          sl_GetSockOpt
#define recv                                sl_Recv
#define recvfrom                            sl_RecvFrom
#define write                               sl_Write
#define send                                sl_Send
#define sendto                              sl_SendTo
#define gethostbyname                       sl_NetAppDnsGetHostByName
#define htonl                               sl_Htonl
#define ntohl                               sl_Ntohl
#define htons                               sl_Htons
#define ntohs                               sl_Ntohs
#endif

/*****************************************************************************/
/* Structure/Enum declarations                                               */
/*****************************************************************************/

/* Internet address   */
typedef struct SlInAddr_t
{
#ifndef s_addr 
    _u32           s_addr;             /* Internet address 32 bits */
#else
    union S_un {
       struct { _u8 s_b1,s_b2,s_b3,s_b4; } S_un_b;
       struct { _u8 s_w1,s_w2; } S_un_w;
        _u32 S_addr;
    } S_un;
#endif
}SlInAddr_t;


/* sockopt */
typedef struct 
{
    _u32 KeepaliveEnabled; /* 0 = disabled;1 = enabled; default = 1*/
}SlSockKeepalive_t;

typedef struct 
{
    _u32 ReuseaddrEnabled; /* 0 = disabled; 1 = enabled; default = 1*/
}SlSockReuseaddr_t;

typedef struct 
{
    _u32 Winsize;          /* receive window size for tcp sockets  */
}SlSockWinsize_t;

typedef struct 
{
    _u32 NonblockingEnabled;/* 0 = disabled;1 = enabled;default = 1*/
}SlSockNonblocking_t;


typedef struct
{
  _u8   sd;
  _u8   type;
  _i16  val;
  _u8*  pExtraInfo;
} SlSocketAsyncEvent_t;

typedef struct
{
  _i16       status;
  _u8        sd;
  _u8        padding;
} SlSockTxFailEventData_t;


typedef union
{
  SlSockTxFailEventData_t   SockTxFailData;
  SlSocketAsyncEvent_t      SockAsyncData;
} SlSockEventData_u;


typedef struct
{
   _u32                    Event;
   SlSockEventData_u       socketAsyncEvent;
} SlSockEvent_t;






typedef struct
{
    _u32    secureMask;
} SlSockSecureMask;

typedef struct
{
    _u8     secureMethod;
} SlSockSecureMethod;

typedef enum
{
  SL_BSD_SECURED_PRIVATE_KEY_IDX = 0,
  SL_BSD_SECURED_CERTIFICATE_IDX,
  SL_BSD_SECURED_CA_IDX,
  SL_BSD_SECURED_DH_IDX
}slBsd_secureSocketFilesIndex_e;

typedef struct 
{
    SlInAddr_t   imr_multiaddr;     /* The IPv4 multicast address to join */
    SlInAddr_t   imr_interface;     /* The interface to use for this group */
} SlSockIpMreq;


/* sockopt */
typedef _u32   SlTime_t;
typedef _u32   SlSuseconds_t;

typedef struct SlTimeval_t
{
    SlTime_t          tv_sec;             /* Seconds      */
    SlSuseconds_t     tv_usec;            /* Microseconds */
}SlTimeval_t;

typedef _u16 SlSocklen_t;

/* IpV4 socket address */
typedef struct SlSockAddr_t
{
    _u16          sa_family;     /* Address family (e.g. , AF_INET)     */
    _u8           sa_data[14];  /* Protocol- specific address information*/
}SlSockAddr_t;


/* IpV6 or Ipv6 EUI64 */
typedef struct SlIn6Addr_t
{
    union 
    {
        _u8   _S6_u8[16];
        _u32  _S6_u32[4];
    } _S6_un;
}SlIn6Addr_t;

typedef struct SlSockAddrIn6_t
{
    _u16           sin6_family;                 /* AF_INET6 || AF_INET6_EUI_48*/
    _u16           sin6_port;                   /* Transport layer port.  */
    _u32           sin6_flowinfo;               /* IPv6 flow information. */
    SlIn6Addr_t             sin6_addr;                   /* IPv6 address. */
    _u32           sin6_scope_id;               /* set of interfaces for a scope. */
}SlSockAddrIn6_t;

/* Socket address, Internet style. */

typedef struct SlSockAddrIn_t
{
    _u16              sin_family;         /* Internet Protocol (AF_INET).                    */
    _u16              sin_port;           /* Address port (16 bits).                         */
    SlInAddr_t                  sin_addr;           /* Internet address (32 bits).                     */
    _i8               sin_zero[8];        /* Not used.                                       */
}SlSockAddrIn_t;

typedef struct
{
    _u32 ip;
    _u32 gateway;
    _u32 dns;
}SlIpV4AcquiredAsync_t;

typedef struct  
{
    _u32 type;
    _u32 ip[4];
    _u32 gateway[4];
    _u32 dns[4];
}SlIpV6AcquiredAsync_t;

typedef struct
{
   _u32    ip_address;
   _u32    lease_time;
   _u8     mac[6];
   _u16    padding;
}SlIpLeasedAsync_t;

typedef struct
{
  _u32    ip_address;
  _u8     mac[6];
  _u16    reason;
}SlIpReleasedAsync_t;


typedef union
{
  SlIpV4AcquiredAsync_t    ipAcquiredV4; /*SL_NETAPP_IPV4_IPACQUIRED_EVENT*/
  SlIpV6AcquiredAsync_t    ipAcquiredV6; /*SL_NETAPP_IPV6_IPACQUIRED_EVENT*/
  _u32                      sd;           /*SL_SOCKET_TX_FAILED_EVENT*/ 
  SlIpLeasedAsync_t        ipLeased;     /* SL_NETAPP_IP_LEASED_EVENT   */
  SlIpReleasedAsync_t      ipReleased;   /* SL_NETAPP_IP_RELEASED_EVENT */
} SlNetAppEventData_u;

typedef struct
{
   _u32                     Event;
   SlNetAppEventData_u       EventData;
}SlNetAppEvent_t;


typedef struct sock_secureFiles
{
    _u8                     secureFiles[4];
}SlSockSecureFiles_t;


typedef struct SlFdSet_t                    /* The select socket array manager */
{ 
   _u32        fd_array[(SL_FD_SETSIZE + 31)/32]; /* Bit map of SOCKET Descriptors */
} SlFdSet_t;

typedef struct
{
    _u8   rate;               /* Recevied Rate  */
    _u8   channel;            /* The received channel*/
    _i8    rssi;               /* The computed RSSI value in db of current frame */
    _u8   padding;                                           /* pad to align to 32 bits */
    _u32  timestamp;          /* Timestamp in microseconds,     */
}SlTransceiverRxOverHead_t;



/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

/*!

    \brief create an endpoint for communication
 
    The socket function creates a new socket of a certain socket type, identified 
    by an integer number, and allocates system resources to it.
    This function is called by the application layer to obtain a socket handle.
 
    \param[in] domain           specifies the protocol family of the created socket.
                                For example:
                                   AF_INET for network protocol IPv4
                                   AF_RF for starting transceiver mode. Notes: 
                                   - sending and receiving any packet overriding 802.11 header
                                   - for optimized power consumption the socket will be started in TX 
                                     only mode until receive command is activated
                                   AF_INET6 for IPv6


    \param[in] type              specifies the communication semantic, one of:
                                   SOCK_STREAM (reliable stream-oriented service or Stream Sockets)
                                   SOCK_DGRAM (datagram service or Datagram Sockets)
                                   SOCK_RAW (raw protocols atop the network layer)
                                   when used with AF_RF:
                                                                     SOCK_DGRAM - L2 socket
                                                                     SOCK_RAW - L1 socket - bypass WLAN CCA (Clear Channel Assessment)

    \param[in] protocol         specifies a particular transport to be used with 
                                the socket.
                                The most common are IPPROTO_TCP, IPPROTO_SCTP, IPPROTO_UDP, 
                                IPPROTO_DCCP.
                                The value 0 may be used to select a default 
                                protocol from the selected domain and type

 
    \return                     On success, socket handle that is used for consequent socket operations. 
                                A successful return code should be a positive number (int16)
                                On error, a negative (int16) value will be returned specifying the error code.
                           SL_EAFNOSUPPORT  - illegal domain parameter
                           SL_EPROTOTYPE  - illegal type parameter
                           SL_EACCES   - permission denied
                           SL_ENSOCK  - exceeded maximal number of socket 
                           SL_ENOMEM  - memory allocation error
                           SL_EINVAL  - error in socket configuration
                           SL_EPROTONOSUPPORT  - illegal protocol parameter
                           SL_EOPNOTSUPP  - illegal combination of protocol and type parameters
 
 
    \sa                         sl_Close
    \note                       belongs to \ref basic_api
    \warning
*/
#if _SL_INCLUDE_FUNC(sl_Socket)
_i16 sl_Socket(_i16 Domain, _i16 Type, _i16 Protocol);
#endif

/*!
    \brief gracefully close socket

    This function causes the system to release resources allocated to a socket.  \n
    In case of TCP, the connection is terminated.

    \param[in] sd               socket handle (received in sl_Socket)

    \return                        On success, zero is returned. 
                                On error, a negative number is returned.                               

    \sa                         sl_Socket
    \note                       belongs to \ref ext_api
    \warning
*/
#if _SL_INCLUDE_FUNC(sl_Close)
_i16 sl_Close(_i16 sd);
#endif

/*!
    \brief Accept a connection on a socket
    
    This function is used with connection-based socket types (SOCK_STREAM).
    It extracts the first connection request on the queue of pending 
    connections, creates a new connected socket, and returns a new file 
    descriptor referring to that socket.
    The newly created socket is not in the listening state. The 
    original socket sd is unaffected by this call. 
    The argument sd is a socket that has been created with 
    sl_Socket(), bound to a local address with sl_Bind(), and is 
    listening for connections after a sl_Listen(). The argument \b 
    \e addr is a pointer to a sockaddr structure. This structure 
    is filled in with the address of the peer socket, as known to 
    the communications layer. The exact format of the address 
    returned addr is determined by the socket's address family. 
    The \b \e addrlen argument is a value-result argument: it 
    should initially contain the size of the structure pointed to 
    by addr, on return it will contain the actual length (in 
    bytes) of the address returned.
    
    \param[in] sd               socket descriptor (handle)
    \param[out] addr            the argument addr is a pointer 
                                to a sockaddr structure. This
                                structure is filled in with the
                                address of the peer socket, as
                                known to the communications
                                layer. The exact format of the
                                address returned addr is
                                determined by the socket's
                                address\n
                                sockaddr:\n - code for the
                                address format. On this version
                                only AF_INET is supported.\n -
                                socket address, the length
                                depends on the code format
    \param[out] addrlen         the addrlen argument is a value-result 
                                argument: it should initially contain the
                                size of the structure pointed to by addr
    
    \return                        On success, a socket handle.
                                On a non-blocking accept a possible negative value is SL_EAGAIN.
                                On failure, negative value.
                                SL_POOL_IS_EMPTY may be return in case there are no resources in the system
                                 In this case try again later or increase MAX_CONCURRENT_ACTIONS
    
    \sa                         sl_Socket  sl_Bind  sl_Listen
    \note                       belongs to \ref server_side
    \warning
*/
#if _SL_INCLUDE_FUNC(sl_Accept)
_i16 sl_Accept(_i16 sd, SlSockAddr_t *addr, SlSocklen_t *addrlen);
#endif

/*!
    \brief assign a name to a socket
 
    This function gives the socket the local address addr.
    addr is addrlen bytes long. Traditionally, this is called
    When a socket is created with socket, it exists in a name
    space (address family) but has no name assigned.
    It is necessary to assign a local address before a SOCK_STREAM
    socket may receive connections.
 
    \param[in] sd                socket descriptor (handle)
    \param[in] addr              specifies the destination 
                                addrs\n sockaddr:\n - code for
                                the address format. On this
                                version only AF_INET is
                                supported.\n - socket address,
                                the length depends on the code
                                format
    \param[in] addrlen          contains the size of the structure pointed to by addr
 
    \return                        On success, zero is returned. On error, a negative error code is returned.
 
    \sa                         sl_Socket  sl_Accept sl_Listen
    \note                       belongs to \ref basic_api
    \warning
*/
#if _SL_INCLUDE_FUNC(sl_Bind)
_i16 sl_Bind(_i16 sd, const SlSockAddr_t *addr, _i16 addrlen);
#endif

/*!
    \brief listen for connections on a socket
 
    The willingness to accept incoming connections and a queue
    limit for incoming connections are specified with listen(),
    and then the connections are accepted with accept.
    The listen() call applies only to sockets of type SOCK_STREAM
    The backlog parameter defines the maximum length the queue of
    pending connections may grow to. 
 
    \param[in] sd               socket descriptor (handle)
    \param[in] backlog          specifies the listen queue depth. 
                                
 
    \return                    On success, zero is returned. On error, a negative error code is returned.
 
    \sa                         sl_Socket  sl_Accept  sl_Bind
    \note                       belongs to \ref server_side
    \warning
*/
#if _SL_INCLUDE_FUNC(sl_Listen)
_i16 sl_Listen(_i16 sd, _i16 backlog);
#endif

/*!
    \brief Initiate a connection on a socket 
   
    Function connects the socket referred to by the socket 
    descriptor sd, to the address specified by addr. The addrlen 
    argument specifies the size of addr. The format of the 
    address in addr is determined by the address space of the 
    socket. If it is of type SOCK_DGRAM, this call specifies the 
    peer with which the socket is to be associated; this address 
    is that to which datagrams are to be sent, and the only 
    address from which datagrams are to be received.  If the 
    socket is of type SOCK_STREAM, this call attempts to make a 
    connection to another socket. The other socket is specified 
    by address, which is an address in the communications space 
    of the socket. 
   
   
    \param[in] sd               socket descriptor (handle)
    \param[in] addr             specifies the destination addr\n
                                sockaddr:\n - code for the
                                address format. On this version
                                only AF_INET is supported.\n -
                                socket address, the length
                                depends on the code format
   
    \param[in] addrlen          contains the size of the structure pointed 
                                to by addr
 
    \return                     On success, a socket handle.
                                On a non-blocking connect a possible negative value is SL_EALREADY.
                                On failure, negative value.
                                SL_POOL_IS_EMPTY may be return in case there are no resources in the system
                                  In this case try again later or increase MAX_CONCURRENT_ACTIONS
 
    \sa                         sl_Socket
    \note                       belongs to \ref client_side
    \warning
*/
#if _SL_INCLUDE_FUNC(sl_Connect)
_i16 sl_Connect(_i16 sd, const SlSockAddr_t *addr, _i16 addrlen);
#endif

/*!
    \brief Monitor socket activity
   
    Select allow a program to monitor multiple file descriptors,
    waiting until one or more of the file descriptors become 
    "ready" for some class of I/O operation 
   
   
    \param[in]  nfds        the highest-numbered file descriptor in any of the
                            three sets, plus 1.
    \param[out] readsds     socket descriptors list for read monitoring and accept monitoring
    \param[out] writesds    socket descriptors list for connect monitoring only, write monitoring is not supported, non blocking connect is supported
    \param[out] exceptsds   socket descriptors list for exception monitoring, not supported.
    \param[in]  timeout     is an upper bound on the amount of time elapsed
                            before select() returns. Null or above 0xffff seconds means 
                            infinity timeout. The minimum timeout is 10 milliseconds,
                            less than 10 milliseconds will be set automatically to 10 milliseconds. 
                            Max microseconds supported is 0xfffc00.
   
    \return                    On success, select()  returns the number of
                            file descriptors contained in the three returned
                            descriptor sets (that is, the total number of bits that
                            are set in readfds, writefds, exceptfds) which may be
                            zero if the timeout expires before anything interesting
                            happens. On error, a negative value is returned.
                            readsds - return the sockets on which Read request will
                            return without delay with valid data.
                            writesds - return the sockets on which Write request 
                            will return without delay.
                            exceptsds - return the sockets closed recently. 
                            SL_POOL_IS_EMPTY may be return in case there are no resources in the system
                               In this case try again later or increase MAX_CONCURRENT_ACTIONS
 
    \sa     sl_Socket
    \note   If the timeout value set to less than 5ms it will automatically set 
            to 5ms to prevent overload of the system
            belongs to \ref basic_api
            
            Only one sl_Select can be handled at a time.
            Calling this API while the same command is called from another thread, may result
                in one of the two scenarios:
            1. The command will wait (internal) until the previous command finish, and then be executed.
            2. There are not enough resources and SL_POOL_IS_EMPTY error will return. 
            In this case, MAX_CONCURRENT_ACTIONS can be increased (result in memory increase) or try
            again later to issue the command.
   
    \warning
*/
#if _SL_INCLUDE_FUNC(sl_Select)
_i16 sl_Select(_i16 nfds, SlFdSet_t *readsds, SlFdSet_t *writesds, SlFdSet_t *exceptsds, struct SlTimeval_t *timeout);


/*!
    \brief Select's SlFdSet_t SET function
   
    Sets current socket descriptor on SlFdSet_t container
*/
void SL_FD_SET(_i16 fd, SlFdSet_t *fdset);

/*!
    \brief Select's SlFdSet_t CLR function
   
    Clears current socket descriptor on SlFdSet_t container
*/
void SL_FD_CLR(_i16 fd, SlFdSet_t *fdset);


/*!
    \brief Select's SlFdSet_t ISSET function
   
    Checks if current socket descriptor is set (TRUE/FALSE)

    \return            Returns TRUE if set, FALSE if unset

*/
_i16  SL_FD_ISSET(_i16 fd, SlFdSet_t *fdset);

/*!
    \brief Select's SlFdSet_t ZERO function
   
    Clears all socket descriptors from SlFdSet_t
*/
void SL_FD_ZERO(SlFdSet_t *fdset);



#endif

/*!
    \brief set socket options
 
    This function manipulate the options associated with a socket.
    Options may exist at multiple protocol levels; they are always
    present at the uppermost socket level.
 
    When manipulating socket options the level at which the option resides
    and the name of the option must be specified.  To manipulate options at
    the socket level, level is specified as SOL_SOCKET.  To manipulate
    options at any other level the protocol number of the appropriate proto-
    col controlling the option is supplied.  For example, to indicate that an
    option is to be interpreted by the TCP protocol, level should be set to
    the protocol number of TCP; 
 
    The parameters optval and optlen are used to access optval - 
    ues for setsockopt().  For getsockopt() they identify a 
    buffer in which the value for the requested option(s) are to 
    be returned.  For getsockopt(), optlen is a value-result 
    parameter, initially containing the size of the buffer 
    pointed to by option_value, and modified on return to 
    indicate the actual size of the value returned.  If no option 
    value is to be supplied or returned, option_value may be 
    NULL.
   
    \param[in] sd               socket handle
    \param[in] level            defines the protocol level for this option
                                - <b>SL_SOL_SOCKET</b>   Socket level configurations (L4, transport layer)
                                - <b>SL_IPPROTO_IP</b>   IP level configurations (L3, network layer)
                                - <b>SL_SOL_PHY_OPT</b>  Link level configurations (L2, link layer)
    \param[in] optname          defines the option name to interrogate
                                - <b>SL_SOL_SOCKET</b>
                                  - <b>SL_SO_KEEPALIVE</b>  \n
                                                 Enable/Disable periodic keep alive.
                                                 Keeps TCP connections active by enabling the periodic transmission of messages \n
                                                 Timeout is 5 minutes.\n
                                                 Default: Enabled \n
                                                 This options takes SlSockKeepalive_t struct as parameter
                                  - <b>SL_SO_RCVTIMEO</b>  \n
                                                 Sets the timeout value that specifies the maximum amount of time an input function waits until it completes. \n
                                                 Default: No timeout \n
                                                 This options takes SlTimeval_t struct as parameter
                                  - <b>SL_SO_RCVBUF</b>  \n
                                                 Sets tcp max recv window size. \n
                                                 This options takes SlSockWinsize_t struct as parameter
                                  - <b>SL_SO_NONBLOCKING</b> \n
                                                 Sets socket to non-blocking operation Impacts: connect, accept, send, sendto, recv and recvfrom. \n
                                                 Default: Blocking.
                                                 This options takes SlSockNonblocking_t struct as parameter
                                  - <b>SL_SO_SECMETHOD</b> \n
                                                 Sets method to tcp secured socket (SL_SEC_SOCKET) \n
                                                 Default: SL_SO_SEC_METHOD_SSLv3_TLSV1_2 \n
                                                 This options takes SlSockSecureMethod struct as parameter
                                  - <b>SL_SO_SEC_MASK</b> \n
                                                 Sets specific cipher to tcp secured socket (SL_SEC_SOCKET) \n
                                                 Default: "Best" cipher suitable to method \n
                                                 This options takes SlSockSecureMask struct as parameter
                                  - <b>SL_SO_SECURE_FILES_CA_FILE_NAME</b> \n
                                                 Map secured socket to CA file by name \n
                                                 This options takes <b>_u8</b> buffer as parameter 
                                  - <b>SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME</b> \n
                                                 Map secured socket to private key by name \n
                                                 This options takes <b>_u8</b> buffer as parameter 
                                  - <b>SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME</b> \n
                                                 Map secured socket to certificate file by name \n
                                                 This options takes <b>_u8</b> buffer as parameter 
                                  - <b>SL_SO_SECURE_FILES_DH_KEY_FILE_NAME</b> \n
                                                 Map secured socket to Diffie Hellman file by name \n
                                                 This options takes <b>_u8</b> buffer as parameter 
                                  - <b>SL_SO_CHANGE_CHANNEL</b> \n
                                                 Sets channel in transceiver mode.
                                                 This options takes <b>_u32</b> as channel number parameter
                                - <b>SL_IPPROTO_IP</b> 
                                  - <b>SL_IP_MULTICAST_TTL</b> \n
                                                 Set the time-to-live value of outgoing multicast packets for this socket. \n
                                                 This options takes <b>_u8</b> as parameter 
                                  - <b>SL_IP_ADD_MEMBERSHIP</b> \n
                                                 UDP socket, Join a multicast group. \n
                                                 This options takes SlSockIpMreq struct as parameter
                                  - <b>SL_IP_DROP_MEMBERSHIP</b> \n
                                                 UDP socket, Leave a multicast group \n
                                                 This options takes SlSockIpMreq struct as parameter
                                  - <b>SL_IP_RAW_RX_NO_HEADER</b> \n                 
                                                 Raw socket remove IP header from received data. \n
                                                 Default: data includes ip header \n
                                                 This options takes <b>_u32</b> as parameter
                                  - <b>SL_IP_HDRINCL</b> \n
                                                 RAW socket only, the IPv4 layer generates an IP header when sending a packet unless \n
                                                 the IP_HDRINCL socket option is enabled on the socket.    \n
                                                 When it is enabled, the packet must contain an IP header. \n
                                                 Default: disabled, IPv4 header generated by Network Stack \n
                                                 This options takes <b>_u32</b> as parameter
                                  - <b>SL_IP_RAW_IPV6_HDRINCL</b> (inactive) \n
                                                 RAW socket only, the IPv6 layer generates an IP header when sending a packet unless \n
                                                 the IP_HDRINCL socket option is enabled on the socket. When it is enabled, the packet must contain an IP header \n
                                                 Default: disabled, IPv4 header generated by Network Stack \n
                                                 This options takes <b>_u32</b> as parameter
                                - <b>SL_SOL_PHY_OPT</b>
                                  - <b>SL_SO_PHY_RATE</b> \n
                                                 RAW socket, set WLAN PHY transmit rate \n
                                                 The values are based on RateIndex_e    \n
                                                 This options takes <b>_u32</b> as parameter
                                  - <b>SL_SO_PHY_TX_POWER</b> \n
                                                 RAW socket, set WLAN PHY TX power \n
                                                 Valid rage is 1-15 \n
                                                 This options takes <b>_u32</b> as parameter
                                  - <b>SL_SO_PHY_NUM_FRAMES_TO_TX</b> \n
                                                 RAW socket, set number of frames to transmit in transceiver mode.
                                                 Default: 1 packet
                                                 This options takes <b>_u32</b> as parameter
                                  - <b>SL_SO_PHY_PREAMBLE</b> \n
                                                 RAW socket, set WLAN PHY preamble for Long/Short\n
                                                 This options takes <b>_u32</b> as parameter
  
    \param[in] optval           specifies a value for the option
    \param[in] optlen           specifies the length of the 
        option value
 
    \return                     On success, zero is returned. 
                                On error, a negative value is returned. 
    \sa     sl_getsockopt
    \note   belongs to \ref basic_api  
    \warning
    \par   Examples:
    \par   
    <b> SL_SO_KEEPALIVE: </b>(disable Keepalive)
    \code		
           SlSockKeepalive_t enableOption;
           enableOption.KeepaliveEnabled = 0;
           sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_KEEPALIVE, (_u8 *)&enableOption,sizeof(enableOption));  
    \endcode
    \par   
    <b> SL_SO_RCVTIMEO: </b>
    \code
           struct SlTimeval_t timeVal;
           timeVal.tv_sec =  1;             // Seconds
           timeVal.tv_usec = 0;             // Microseconds. 10000 microseconds resolution
           sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_RCVTIMEO, (_u8 *)&timeVal, sizeof(timeVal));    // Enable receive timeout 
    \endcode
    \par   
    <b>    SL_SO_RCVBUF: </b>
    \code
           SlSockWinsize_t size;
           size.Winsize = 3000;  // bytes
           sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_RCVBUF, (_u8 *)&size, sizeof(size));
    \endcode
    \par   
    <b>    SL_SO_NONBLOCKING: </b>
    \code
           SlSockNonblocking_t enableOption;
           enableOption.NonblockingEnabled = 1;
           sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_NONBLOCKING, (_u8 *)&enableOption,sizeof(enableOption)); // Enable/disable nonblocking mode
    \endcode
    \par   
    <b>    SL_SO_SECMETHOD:</b>
    \code
           SlSockSecureMethod method;
           method.secureMethod = SL_SO_SEC_METHOD_SSLV3;                                 // security method we want to use
           SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, SL_SEC_SOCKET);
           sl_SetSockOpt(SockID, SL_SOL_SOCKET, SL_SO_SECMETHOD, (_u8 *)&method, sizeof(method));
    \endcode
    \par   
    <b>   SL_SO_SECURE_MASK:</b>
    \code 
           SlSockSecureMask cipher;
           cipher.secureMask = SL_SEC_MASK_SSL_RSA_WITH_RC4_128_SHA;                   // cipher type
           SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, SL_SEC_SOCKET);
           sl_SetSockOpt(SockID, SL_SOL_SOCKET, SL_SO_SEC_MASK,(_u8 *)&cipher, sizeof(cipher));
    \endcode
    \par   
    <b>   SL_SO_SECURE_FILES_CA_FILE_NAME:</b>
    \code           
           sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_SECURE_FILES_CA_FILE_NAME,"exuifaxCaCert.der",strlen("exuifaxCaCert.der"));
     \endcode

   \par   
    <b>   SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME:</b>
     \code           
           sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME,"myPrivateKey.der",strlen("myPrivateKey.der"));
     \endcode

   \par   
    <b>   SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME:</b>
     \code
           sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME,"myCertificate.der",strlen("myCertificate.der"));
     \endcode

   \par   
    <b>   SL_SO_SECURE_FILES_DH_KEY_FILE_NAME:</b>
     \code
           sl_SetSockOpt(SockID,SL_SOL_SOCKET,SL_SO_SECURE_FILES_DH_KEY_FILE_NAME,"myDHinServerMode.der",strlen("myDHinServerMode.der"));
     \endcode

   \par   
    <b>   SL_IP_MULTICAST_TTL:</b>
     \code
           _u8 ttl = 20;
           sl_SetSockOpt(SockID, SL_IPPROTO_IP, SL_IP_MULTICAST_TTL, &ttl, sizeof(ttl));
     \endcode

   \par   
    <b>   SL_IP_ADD_MEMBERSHIP:</b>
     \code
           SlSockIpMreq mreq;
           sl_SetSockOpt(SockID, SL_IPPROTO_IP, SL_IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    \endcode

   \par   
    <b>   SL_IP_DROP_MEMBERSHIP:</b>
    \code
           SlSockIpMreq mreq;
           sl_SetSockOpt(SockID, SL_IPPROTO_IP, SL_IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    \endcode

   \par   
    <b>   SL_SO_CHANGE_CHANNEL:</b>
    \code
          _u32 newChannel = 6; // range is 1-13
          sl_SetSockOpt(SockID, SL_SOL_SOCKET, SL_SO_CHANGE_CHANNEL, &newChannel, sizeof(newChannel));  
    \endcode

    \par   
    <b>   SL_IP_RAW_RX_NO_HEADER:</b>
    \code
          _u32 header = 1;  // remove ip header
          sl_SetSockOpt(SockID, SL_IPPROTO_IP, SL_IP_RAW_RX_NO_HEADER, &header, sizeof(header));
    \endcode

   \par   
    <b>   SL_IP_HDRINCL:</b>
    \code
         _u32 header = 1;
         sl_SetSockOpt(SockID, SL_IPPROTO_IP, SL_IP_HDRINCL, &header, sizeof(header));
    \endcode
   \par   
    <b>   SL_IP_RAW_IPV6_HDRINCL:</b>
    \code
          _u32 header = 1;
          sl_SetSockOpt(SockID, SL_IPPROTO_IP, SL_IP_RAW_IPV6_HDRINCL, &header, sizeof(header));
    \endcode

   \par   
    <b>   SL_SO_PHY_RATE:</b>
    \code
          _u32 rate = 6; // see wlan.h RateIndex_e for values
          sl_SetSockOpt(SockID, SL_SOL_PHY_OPT, SL_SO_PHY_RATE, &rate, sizeof(rate));  
    \endcode

   \par   
    <b>   SL_SO_PHY_TX_POWER:</b>
    \code
          _u32 txpower = 1; // valid range is 1-15
          sl_SetSockOpt(SockID, SL_SOL_PHY_OPT, SL_SO_PHY_TX_POWER, &txpower, sizeof(txpower));
    \endcode

   \par   
    <b>   SL_SO_PHY_NUM_FRAMES_TO_TX:</b>
    \code
     	  _u32 numframes = 1;
          sl_SetSockOpt(SockID, SL_SOL_PHY_OPT, SL_SO_PHY_NUM_FRAMES_TO_TX, &numframes, sizeof(numframes));
    \endcode

   \par   
    <b>   SL_SO_PHY_PREAMBLE:</b>
    \code
          _u32 preamble = 1;
          sl_SetSockOpt(SockID, SL_SOL_PHY_OPT, SL_SO_PHY_PREAMBLE, &preamble, sizeof(preamble));
    \endcode

*/
#if _SL_INCLUDE_FUNC(sl_SetSockOpt)
_i16 sl_SetSockOpt(_i16 sd, _i16 level, _i16 optname, const void *optval, SlSocklen_t optlen);
#endif

/*!
    \brief Get socket options
    
    This function manipulate the options associated with a socket.
    Options may exist at multiple protocol levels; they are always
    present at the uppermost socket level.
    
    When manipulating socket options the level at which the option resides
    and the name of the option must be specified.  To manipulate options at
    the socket level, level is specified as SOL_SOCKET.  To manipulate
    options at any other level the protocol number of the appropriate proto-
    col controlling the option is supplied.  For example, to indicate that an
    option is to be interpreted by the TCP protocol, level should be set to
    the protocol number of TCP; 
    
    The parameters optval and optlen are used to access optval - 
    ues for setsockopt().  For getsockopt() they identify a 
    buffer in which the value for the requested option(s) are to 
    be returned.  For getsockopt(), optlen is a value-result 
    parameter, initially containing the size of the buffer 
    pointed to by option_value, and modified on return to 
    indicate the actual size of the value returned.  If no option 
    value is to be supplied or returned, option_value may be 
    NULL. 
    
    
    \param[in]  sd              socket handle
    \param[in]  level           defines the protocol level for this option
    \param[in]  optname         defines the option name to interrogate
    \param[out] optval          specifies a value for the option
    \param[out] optlen          specifies the length of the 
                                option value
    
    \return                        On success, zero is returned. 
                                On error, a negative value is returned.
    \sa     sl_SetSockOpt
    \note   See sl_SetSockOpt
            belongs to \ref ext_api
    \warning
*/
#if _SL_INCLUDE_FUNC(sl_GetSockOpt)
_i16 sl_GetSockOpt(_i16 sd, _i16 level, _i16 optname, void *optval, SlSocklen_t *optlen);
#endif

/*!
    \brief read data from TCP socket
     
    function receives a message from a connection-mode socket
     
    \param[in]  sd              socket handle
    \param[out] buf             Points to the buffer where the 
                                message should be stored.
    \param[in]  Len             Specifies the length in bytes of 
                                the buffer pointed to by the buffer argument. 
                                Range: 1-16000 bytes
    \param[in]  flags           Specifies the type of message 
                                reception. On this version, this parameter is not
                                supported.
    
    \return                     return the number of bytes received, 
                                or a negative value if an error occurred.
                                using a non-blocking recv a possible negative value is SL_EAGAIN.
                                SL_POOL_IS_EMPTY may be return in case there are no resources in the system
                                 In this case try again later or increase MAX_CONCURRENT_ACTIONS
    
    \sa     sl_RecvFrom
    \note                       belongs to \ref recv_api
    \warning
    \par        Examples:
    \code       An example of receiving data using TCP socket:
    
                SlSockAddrIn_t  Addr;
                SlSockAddrIn_t  LocalAddr;
                _i16 AddrSize = sizeof(SlSockAddrIn_t);
                _i16 SockID, newSockID;
                _i16 Status;
                _i8 Buf[RECV_BUF_LEN];

                LocalAddr.sin_family = SL_AF_INET;
                LocalAddr.sin_port = sl_Htons(5001);
                LocalAddr.sin_addr.s_addr = 0;

                Addr.sin_family = SL_AF_INET;
                Addr.sin_port = sl_Htons(5001);
                Addr.sin_addr.s_addr = sl_Htonl(SL_IPV4_VAL(10,1,1,200));

                SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
                Status = sl_Bind(SockID, (SlSockAddr_t *)&LocalAddr, AddrSize);
                Status = sl_Listen(SockID, 0);
                newSockID = sl_Accept(SockID, (SlSockAddr_t*)&Addr, (SlSocklen_t*) &AddrSize);
                Status = sl_Recv(newSockID, Buf, 1460, 0);
    \endcode
    \code      Example code for Rx transceiver mode using a raw socket
               _i8 buffer[1536];
               _i16 sd;
               _u16 size;
               SlTransceiverRxOverHead_t *transHeader;
               sd = sl_Socket(SL_AF_RF,SL_SOCK_RAW,11); // channel 11
               while(1)
               {
                  size = sl_Recv(sd,buffer,1536,0);
                  transHeader = (SlTransceiverRxOverHead_t *)buffer;
                  printf("RSSI is %d frame type is 0x%x size %d\n",transHeader->rssi,buffer[sizeof(SlTransceiverRxOverHead_t)],size);
               }      
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_Recv)
_i16 sl_Recv(_i16 sd, void *buf, _i16 Len, _i16 flags);
#endif

/*!
    \brief read data from socket
    
    function receives a message from a connection-mode or
    connectionless-mode socket
    
    \param[in]  sd              socket handle 
    \param[out] buf             Points to the buffer where the message should be stored.
    \param[in]  Len             Specifies the length in bytes of the buffer pointed to by the buffer argument.
                                Range: 1-16000 bytes
    \param[in]  flags           Specifies the type of message
                                reception. On this version, this parameter is not
                                supported.
    \param[in]  from            pointer to an address structure 
                                indicating the source
                                address.\n sockaddr:\n - code
                                for the address format. On this
                                version only AF_INET is
                                supported.\n - socket address,
                                the length depends on the code
                                format
    \param[in]  fromlen         source address structure
                                size. This parameter MUST be set to the size of the structure pointed to by addr.
    
    
    \return                     return the number of bytes received, 
                                or a negative value if an error occurred.
                                using a non-blocking recv a possible negative value is SL_EAGAIN.
                                SL_RET_CODE_INVALID_INPUT (-2) will be returned if fromlen has incorrect length. 
                                SL_POOL_IS_EMPTY may be return in case there are no resources in the system
                                 In this case try again later or increase MAX_CONCURRENT_ACTIONS
    
    \sa     sl_Recv
    \note                       belongs to \ref recv_api
    \warning
    \par        Example:
    \code       An example of receiving data:
    
                SlSockAddrIn_t  Addr;
                SlSockAddrIn_t  LocalAddr;
                _i16 AddrSize = sizeof(SlSockAddrIn_t);
                _i16 SockID;
                _i16 Status;
                _i8 Buf[RECV_BUF_LEN];

                LocalAddr.sin_family = SL_AF_INET;
                LocalAddr.sin_port = sl_Htons(5001);
                LocalAddr.sin_addr.s_addr = 0;

                SockID = sl_Socket(SL_AF_INET,SL_SOCK_DGRAM, 0);
                Status = sl_Bind(SockID, (SlSockAddr_t *)&LocalAddr, AddrSize);
                Status = sl_RecvFrom(SockID, Buf, 1472, 0, (SlSockAddr_t *)&Addr, (SlSocklen_t*)&AddrSize);

    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_RecvFrom)
_i16 sl_RecvFrom(_i16 sd, void *buf, _i16 Len, _i16 flags, SlSockAddr_t *from, SlSocklen_t *fromlen);
#endif

/*!
    \brief write data to TCP socket
    
    This function is used to transmit a message to another socket.
    Returns immediately after sending data to device.
    In case of TCP failure an async event SL_SOCKET_TX_FAILED_EVENT is going to
    be received.
    In case of a RAW socket (transceiver mode), extra 4 bytes should be reserved at the end of the 
    frame data buffer for WLAN FCS 
     
    \param[in] sd               socket handle
    \param[in] buf              Points to a buffer containing 
                                the message to be sent
    \param[in] Len              message size in bytes. Range: 1-1460 bytes
    \param[in] flags            Specifies the type of message 
                                transmission. On this version, this parameter is not
                                supported for TCP.
                                For transceiver mode, the SL_RAW_RF_TX_PARAMS macro can be used to determine
                                transmission parameters (channel,rate,tx_power,preamble)
    
    
    \return                     Return the number of bytes transmitted, 
                                or -1 if an error occurred
    
    \sa     sl_SendTo 
    \note                       belongs to \ref send_api
    \warning   
    \par        Example:
    \code       An example of sending data:
    
                SlSockAddrIn_t  Addr;
                _i16 AddrSize = sizeof(SlSockAddrIn_t);
                _i16 SockID;
                _i16 Status;
                _i8 Buf[SEND_BUF_LEN];

                Addr.sin_family = SL_AF_INET;
                Addr.sin_port = sl_Htons(5001);
                Addr.sin_addr.s_addr = sl_Htonl(SL_IPV4_VAL(10,1,1,200));

                SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
                Status = sl_Connect(SockID, (SlSockAddr_t *)&Addr, AddrSize);
                Status = sl_Send(SockID, Buf, 1460, 0 );
 
    \endcode
 */ 
#if _SL_INCLUDE_FUNC(sl_Send )
_i16 sl_Send(_i16 sd, const void *buf, _i16 Len, _i16 flags);
#endif

/*!
    \brief write data to socket
    
    This function is used to transmit a message to another socket
    (connection less socket SOCK_DGRAM,  SOCK_RAW).
    Returns immediately after sending data to device.
    In case of transmission failure an async event SL_SOCKET_TX_FAILED_EVENT is going to
    be received.
    
    \param[in] sd               socket handle
    \param[in] buf              Points to a buffer containing 
                                the message to be sent
    \param[in] Len              message size in bytes. Range: 1-1460 bytes
    \param[in] flags            Specifies the type of message 
                                transmission. On this version, this parameter is not
                                supported 
    \param[in] to               pointer to an address structure 
                                indicating the destination
                                address.\n sockaddr:\n - code
                                for the address format. On this
                                version only AF_INET is
                                supported.\n - socket address,
                                the length depends on the code
                                format
    \param[in] tolen            destination address structure size 
    
    \return                     Return the number of transmitted bytes, 
                                or -1 if an error occurred
    
    \sa     sl_Send
    \note                       belongs to \ref send_api
    \warning
    \par        Example:
    \code       An example of sending data:
    
                SlSockAddrIn_t  Addr;
                _i16 AddrSize = sizeof(SlSockAddrIn_t);
                _i16 SockID;
                _i16 Status;
                _i8 Buf[SEND_BUF_LEN];

                Addr.sin_family = SL_AF_INET;
                Addr.sin_port = sl_Htons(5001);
                Addr.sin_addr.s_addr = sl_Htonl(SL_IPV4_VAL(10,1,1,200));

                SockID = sl_Socket(SL_AF_INET,SL_SOCK_DGRAM, 0);
                Status = sl_SendTo(SockID, Buf, 1472, 0, (SlSockAddr_t *)&Addr, AddrSize);

    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_SendTo)
_i16 sl_SendTo(_i16 sd, const void *buf, _i16 Len, _i16 flags, const SlSockAddr_t *to, SlSocklen_t tolen);
#endif

/*!
    \brief Reorder the bytes of a 32-bit unsigned value
    
    This function is used to Reorder the bytes of a 32-bit unsigned value from processor order to network order.
     
    \param[in] var              variable to reorder 
    
    \return                     Return the reorder variable, 
    
    \sa     sl_SendTo  sl_Bind  sl_Connect  sl_RecvFrom  sl_Accept
    \note                       belongs to \ref send_api
    \warning   
*/
#if _SL_INCLUDE_FUNC(sl_Htonl )
_u32 sl_Htonl( _u32 val );

#define sl_Ntohl sl_Htonl  /* Reorder the bytes of a 16-bit unsigned value from network order to processor orde. */
#endif

/*!
    \brief Reorder the bytes of a 16-bit unsigned value
    
    This function is used to Reorder the bytes of a 16-bit unsigned value from processor order to network order.
     
    \param[in] var              variable to reorder 
    
    \return                     Return the reorder variable, 
    
    \sa     sl_SendTo  sl_Bind    sl_Connect  sl_RecvFrom  sl_Accept
    \note                       belongs to \ref send_api
    \warning   
*/
#if _SL_INCLUDE_FUNC(sl_Htons )
_u16 sl_Htons( _u16 val );

#define sl_Ntohs sl_Htons   /* Reorder the bytes of a 16-bit unsigned value from network order to processor orde. */
#endif

/*!

 Close the Doxygen group.
 @}

 */


#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __SOCKET_H__ */


