/**
 * \file ssl.h
 *
 * \brief SSL/TLS functions.
 *
 *  Copyright (C) 2006-2014, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef POLARSSL_SSL_H
#define POLARSSL_SSL_H

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif
#include "net.h"
#include "bignum.h"
#include "ecp.h"

#include "ssl_ciphersuites.h"

#if defined(POLARSSL_MD5_C)
#include "md5.h"
#endif

#if defined(POLARSSL_SHA1_C)
#include "sha1.h"
#endif

#if defined(POLARSSL_SHA256_C)
#include "sha256.h"
#endif

#if defined(POLARSSL_SHA512_C)
#include "sha512.h"
#endif

// for session tickets
#if defined(POLARSSL_AES_C)
#include "aes.h"
#endif

#if defined(POLARSSL_X509_CRT_PARSE_C)
#include "x509_crt.h"
#include "x509_crl.h"
#endif

#if defined(POLARSSL_DHM_C)
#include "dhm.h"
#endif

#if defined(POLARSSL_ECDH_C)
#include "ecdh.h"
#endif

#if defined(POLARSSL_ZLIB_SUPPORT)
#include "zlib.h"
#endif

#if defined(POLARSSL_HAVE_TIME)
#include <time.h>
#endif

/* For convenience below and in programs */
#if defined(POLARSSL_KEY_EXCHANGE_PSK_ENABLED) ||                           \
    defined(POLARSSL_KEY_EXCHANGE_RSA_PSK_ENABLED) ||                       \
    defined(POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED) ||                       \
    defined(POLARSSL_KEY_EXCHANGE_ECDHE_PSK_ENABLED)
#define POLARSSL_KEY_EXCHANGE__SOME__PSK_ENABLED
#endif

#if defined(POLARSSL_KEY_EXCHANGE_ECDHE_RSA_ENABLED) ||                     \
    defined(POLARSSL_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) ||                   \
    defined(POLARSSL_KEY_EXCHANGE_ECDHE_PSK_ENABLED)
#define POLARSSL_KEY_EXCHANGE__SOME__ECDHE_ENABLED
#endif

#if defined(_MSC_VER) && !defined(inline)
#define inline _inline
#else
#if defined(__ARMCC_VERSION) && !defined(inline)
#define inline __inline
#endif /* __ARMCC_VERSION */
#endif /*_MSC_VER */

/*
 * SSL Error codes
 */
#define POLARSSL_ERR_SSL_FEATURE_UNAVAILABLE               -0x7080  /**< The requested feature is not available. */
#define POLARSSL_ERR_SSL_BAD_INPUT_DATA                    -0x7100  /**< Bad input parameters to function. */
#define POLARSSL_ERR_SSL_INVALID_MAC                       -0x7180  /**< Verification of the message MAC failed. */
#define POLARSSL_ERR_SSL_INVALID_RECORD                    -0x7200  /**< An invalid SSL record was received. */
#define POLARSSL_ERR_SSL_CONN_EOF                          -0x7280  /**< The connection indicated an EOF. */
#define POLARSSL_ERR_SSL_UNKNOWN_CIPHER                    -0x7300  /**< An unknown cipher was received. */
#define POLARSSL_ERR_SSL_NO_CIPHER_CHOSEN                  -0x7380  /**< The server has no ciphersuites in common with the client. */
#define POLARSSL_ERR_SSL_NO_RNG                            -0x7400  /**< No RNG was provided to the SSL module. */
#define POLARSSL_ERR_SSL_NO_CLIENT_CERTIFICATE             -0x7480  /**< No client certification received from the client, but required by the authentication mode. */
#define POLARSSL_ERR_SSL_CERTIFICATE_TOO_LARGE             -0x7500  /**< Our own certificate(s) is/are too large to send in an SSL message.*/
#define POLARSSL_ERR_SSL_CERTIFICATE_REQUIRED              -0x7580  /**< The own certificate is not set, but needed by the server. */
#define POLARSSL_ERR_SSL_PRIVATE_KEY_REQUIRED              -0x7600  /**< The own private key or pre-shared key is not set, but needed. */
#define POLARSSL_ERR_SSL_CA_CHAIN_REQUIRED                 -0x7680  /**< No CA Chain is set, but required to operate. */
#define POLARSSL_ERR_SSL_UNEXPECTED_MESSAGE                -0x7700  /**< An unexpected message was received from our peer. */
#define POLARSSL_ERR_SSL_FATAL_ALERT_MESSAGE               -0x7780  /**< A fatal alert message was received from our peer. */
#define POLARSSL_ERR_SSL_PEER_VERIFY_FAILED                -0x7800  /**< Verification of our peer failed. */
#define POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY                 -0x7880  /**< The peer notified us that the connection is going to be closed. */
#define POLARSSL_ERR_SSL_BAD_HS_CLIENT_HELLO               -0x7900  /**< Processing of the ClientHello handshake message failed. */
#define POLARSSL_ERR_SSL_BAD_HS_SERVER_HELLO               -0x7980  /**< Processing of the ServerHello handshake message failed. */
#define POLARSSL_ERR_SSL_BAD_HS_CERTIFICATE                -0x7A00  /**< Processing of the Certificate handshake message failed. */
#define POLARSSL_ERR_SSL_BAD_HS_CERTIFICATE_REQUEST        -0x7A80  /**< Processing of the CertificateRequest handshake message failed. */
#define POLARSSL_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE        -0x7B00  /**< Processing of the ServerKeyExchange handshake message failed. */
#define POLARSSL_ERR_SSL_BAD_HS_SERVER_HELLO_DONE          -0x7B80  /**< Processing of the ServerHelloDone handshake message failed. */
#define POLARSSL_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE        -0x7C00  /**< Processing of the ClientKeyExchange handshake message failed. */
#define POLARSSL_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE_RP     -0x7C80  /**< Processing of the ClientKeyExchange handshake message failed in DHM / ECDH Read Public. */
#define POLARSSL_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE_CS     -0x7D00  /**< Processing of the ClientKeyExchange handshake message failed in DHM / ECDH Calculate Secret. */
#define POLARSSL_ERR_SSL_BAD_HS_CERTIFICATE_VERIFY         -0x7D80  /**< Processing of the CertificateVerify handshake message failed. */
#define POLARSSL_ERR_SSL_BAD_HS_CHANGE_CIPHER_SPEC         -0x7E00  /**< Processing of the ChangeCipherSpec handshake message failed. */
#define POLARSSL_ERR_SSL_BAD_HS_FINISHED                   -0x7E80  /**< Processing of the Finished handshake message failed. */
#define POLARSSL_ERR_SSL_MALLOC_FAILED                     -0x7F00  /**< Memory allocation failed */
#define POLARSSL_ERR_SSL_HW_ACCEL_FAILED                   -0x7F80  /**< Hardware acceleration function returned with error */
#define POLARSSL_ERR_SSL_HW_ACCEL_FALLTHROUGH              -0x6F80  /**< Hardware acceleration function skipped / left alone data */
#define POLARSSL_ERR_SSL_COMPRESSION_FAILED                -0x6F00  /**< Processing of the compression / decompression failed */
#define POLARSSL_ERR_SSL_BAD_HS_PROTOCOL_VERSION           -0x6E80  /**< Handshake protocol not within min/max boundaries */
#define POLARSSL_ERR_SSL_BAD_HS_NEW_SESSION_TICKET         -0x6E00  /**< Processing of the NewSessionTicket handshake message failed. */
#define POLARSSL_ERR_SSL_SESSION_TICKET_EXPIRED            -0x6D80  /**< Session ticket has expired. */
#define POLARSSL_ERR_SSL_PK_TYPE_MISMATCH                  -0x6D00  /**< Public key type mismatch (eg, asked for RSA key exchange and presented EC key) */
#define POLARSSL_ERR_SSL_UNKNOWN_IDENTITY                  -0x6C80  /**< Unknown identity received (eg, PSK identity) */
#define POLARSSL_ERR_SSL_INTERNAL_ERROR                    -0x6C00  /**< Internal error (eg, unexpected failure in lower-level module) */
#define POLARSSL_ERR_SSL_COUNTER_WRAPPING                  -0x6B80  /**< A counter would wrap (eg, too many messages exchanged). */

/*
 * Various constants
 */
#define SSL_MAJOR_VERSION_3             3
#define SSL_MINOR_VERSION_0             0   /*!< SSL v3.0 */
#define SSL_MINOR_VERSION_1             1   /*!< TLS v1.0 */
#define SSL_MINOR_VERSION_2             2   /*!< TLS v1.1 */
#define SSL_MINOR_VERSION_3             3   /*!< TLS v1.2 */

/* Determine minimum supported version */
#define SSL_MIN_MAJOR_VERSION           SSL_MAJOR_VERSION_3

#if defined(POLARSSL_SSL_PROTO_SSL3)
#define SSL_MIN_MINOR_VERSION           SSL_MINOR_VERSION_0
#else
#if defined(POLARSSL_SSL_PROTO_TLS1)
#define SSL_MIN_MINOR_VERSION           SSL_MINOR_VERSION_1
#else
#if defined(POLARSSL_SSL_PROTO_TLS1_1)
#define SSL_MIN_MINOR_VERSION           SSL_MINOR_VERSION_2
#else
#if defined(POLARSSL_SSL_PROTO_TLS1_2)
#define SSL_MIN_MINOR_VERSION           SSL_MINOR_VERSION_3
#endif /* POLARSSL_SSL_PROTO_TLS1_2 */
#endif /* POLARSSL_SSL_PROTO_TLS1_1 */
#endif /* POLARSSL_SSL_PROTO_TLS1   */
#endif /* POLARSSL_SSL_PROTO_SSL3   */

/* Determine maximum supported version */
#define SSL_MAX_MAJOR_VERSION           SSL_MAJOR_VERSION_3

#if defined(POLARSSL_SSL_PROTO_TLS1_2)
#define SSL_MAX_MINOR_VERSION           SSL_MINOR_VERSION_3
#else
#if defined(POLARSSL_SSL_PROTO_TLS1_1)
#define SSL_MAX_MINOR_VERSION           SSL_MINOR_VERSION_2
#else
#if defined(POLARSSL_SSL_PROTO_TLS1)
#define SSL_MAX_MINOR_VERSION           SSL_MINOR_VERSION_1
#else
#if defined(POLARSSL_SSL_PROTO_SSL3)
#define SSL_MAX_MINOR_VERSION           SSL_MINOR_VERSION_0
#endif /* POLARSSL_SSL_PROTO_SSL3   */
#endif /* POLARSSL_SSL_PROTO_TLS1   */
#endif /* POLARSSL_SSL_PROTO_TLS1_1 */
#endif /* POLARSSL_SSL_PROTO_TLS1_2 */

/* RFC 6066 section 4, see also mfl_code_to_length in ssl_tls.c
 * NONE must be zero so that memset()ing structure to zero works */
#define SSL_MAX_FRAG_LEN_NONE           0   /*!< don't use this extension   */
#define SSL_MAX_FRAG_LEN_512            1   /*!< MaxFragmentLength 2^9      */
#define SSL_MAX_FRAG_LEN_1024           2   /*!< MaxFragmentLength 2^10     */
#define SSL_MAX_FRAG_LEN_2048           3   /*!< MaxFragmentLength 2^11     */
#define SSL_MAX_FRAG_LEN_4096           4   /*!< MaxFragmentLength 2^12     */
#define SSL_MAX_FRAG_LEN_INVALID        5   /*!< first invalid value        */

#define SSL_IS_CLIENT                   0
#define SSL_IS_SERVER                   1
#define SSL_COMPRESS_NULL               0
#define SSL_COMPRESS_DEFLATE            1

#define SSL_VERIFY_NONE                 0
#define SSL_VERIFY_OPTIONAL             1
#define SSL_VERIFY_REQUIRED             2

#define SSL_INITIAL_HANDSHAKE           0
#define SSL_RENEGOTIATION               1   /* In progress */
#define SSL_RENEGOTIATION_DONE          2   /* Done */
#define SSL_RENEGOTIATION_PENDING       3   /* Requested (server only) */

#define SSL_LEGACY_RENEGOTIATION        0
#define SSL_SECURE_RENEGOTIATION        1

#define SSL_RENEGOTIATION_DISABLED      0
#define SSL_RENEGOTIATION_ENABLED       1

#define SSL_RENEGOTIATION_NOT_ENFORCED  -1
#define SSL_RENEGO_MAX_RECORDS_DEFAULT  16

#define SSL_LEGACY_NO_RENEGOTIATION     0
#define SSL_LEGACY_ALLOW_RENEGOTIATION  1
#define SSL_LEGACY_BREAK_HANDSHAKE      2

#define SSL_TRUNC_HMAC_DISABLED         0
#define SSL_TRUNC_HMAC_ENABLED          1
#define SSL_TRUNCATED_HMAC_LEN          10  /* 80 bits, rfc 6066 section 7 */

#define SSL_SESSION_TICKETS_DISABLED     0
#define SSL_SESSION_TICKETS_ENABLED      1

/**
 * \name SECTION: Module settings
 *
 * The configuration options you can set for this module are in this section.
 * Either change them in config.h or define them on the compiler command line.
 * \{
 */

#if !defined(SSL_DEFAULT_TICKET_LIFETIME)
#define SSL_DEFAULT_TICKET_LIFETIME     86400 /**< Lifetime of session tickets (if enabled) */
#endif

/*
 * Size of the input / output buffer.
 * Note: the RFC defines the default size of SSL / TLS messages. If you
 * change the value here, other clients / servers may not be able to
 * communicate with you anymore. Only change this value if you control
 * both sides of the connection and have it reduced at both sides, or
 * if you're using the Max Fragment Length extension and you know all your
 * peers are using it too!
 */
#if !defined(SSL_MAX_CONTENT_LEN)
#define SSL_MAX_CONTENT_LEN         16384   /**< Size of the input / output buffer */
#endif

/* \} name SECTION: Module settings */

/*
 * Allow extra bytes for record, authentication and encryption overhead:
 * counter (8) + header (5) + IV(16) + MAC (16-48) + padding (0-256)
 * and allow for a maximum of 1024 of compression expansion if
 * enabled.
 */
#if defined(POLARSSL_ZLIB_SUPPORT)
#define SSL_COMPRESSION_ADD          1024
#else
#define SSL_COMPRESSION_ADD             0
#endif

#if defined(POLARSSL_RC4_C) || defined(POLARSSL_CIPHER_MODE_CBC)
/* Ciphersuites using HMAC */
#if defined(POLARSSL_SHA512_C)
#define SSL_MAC_ADD                 48  /* SHA-384 used for HMAC */
#elif defined(POLARSSL_SHA256_C)
#define SSL_MAC_ADD                 32  /* SHA-256 used for HMAC */
#else
#define SSL_MAC_ADD                 20  /* SHA-1   used for HMAC */
#endif
#else
/* AEAD ciphersuites: GCM and CCM use a 128 bits tag */
#define SSL_MAC_ADD                 16
#endif

#if defined(POLARSSL_CIPHER_MODE_CBC)
#define SSL_PADDING_ADD            256
#else
#define SSL_PADDING_ADD              0
#endif

#define SSL_BUFFER_LEN  ( SSL_MAX_CONTENT_LEN               \
                        + SSL_COMPRESSION_ADD               \
                        + 29 /* counter + header + IV */    \
                        + SSL_MAC_ADD                       \
                        + SSL_PADDING_ADD                   \
                        )

/*
 * Signaling ciphersuite values (SCSV)
 */
#define SSL_EMPTY_RENEGOTIATION_INFO    0xFF   /**< renegotiation info ext */

/*
 * Supported Signature and Hash algorithms (For TLS 1.2)
 * RFC 5246 section 7.4.1.4.1
 */
#define SSL_HASH_NONE                0
#define SSL_HASH_MD5                 1
#define SSL_HASH_SHA1                2
#define SSL_HASH_SHA224              3
#define SSL_HASH_SHA256              4
#define SSL_HASH_SHA384              5
#define SSL_HASH_SHA512              6

#define SSL_SIG_ANON                 0
#define SSL_SIG_RSA                  1
#define SSL_SIG_ECDSA                3

/*
 * Client Certificate Types
 * RFC 5246 section 7.4.4 plus RFC 4492 section 5.5
 */
#define SSL_CERT_TYPE_RSA_SIGN       1
#define SSL_CERT_TYPE_ECDSA_SIGN    64

/*
 * Message, alert and handshake types
 */
#define SSL_MSG_CHANGE_CIPHER_SPEC     20
#define SSL_MSG_ALERT                  21
#define SSL_MSG_HANDSHAKE              22
#define SSL_MSG_APPLICATION_DATA       23

#define SSL_ALERT_LEVEL_WARNING         1
#define SSL_ALERT_LEVEL_FATAL           2

#define SSL_ALERT_MSG_CLOSE_NOTIFY           0  /* 0x00 */
#define SSL_ALERT_MSG_UNEXPECTED_MESSAGE    10  /* 0x0A */
#define SSL_ALERT_MSG_BAD_RECORD_MAC        20  /* 0x14 */
#define SSL_ALERT_MSG_DECRYPTION_FAILED     21  /* 0x15 */
#define SSL_ALERT_MSG_RECORD_OVERFLOW       22  /* 0x16 */
#define SSL_ALERT_MSG_DECOMPRESSION_FAILURE 30  /* 0x1E */
#define SSL_ALERT_MSG_HANDSHAKE_FAILURE     40  /* 0x28 */
#define SSL_ALERT_MSG_NO_CERT               41  /* 0x29 */
#define SSL_ALERT_MSG_BAD_CERT              42  /* 0x2A */
#define SSL_ALERT_MSG_UNSUPPORTED_CERT      43  /* 0x2B */
#define SSL_ALERT_MSG_CERT_REVOKED          44  /* 0x2C */
#define SSL_ALERT_MSG_CERT_EXPIRED          45  /* 0x2D */
#define SSL_ALERT_MSG_CERT_UNKNOWN          46  /* 0x2E */
#define SSL_ALERT_MSG_ILLEGAL_PARAMETER     47  /* 0x2F */
#define SSL_ALERT_MSG_UNKNOWN_CA            48  /* 0x30 */
#define SSL_ALERT_MSG_ACCESS_DENIED         49  /* 0x31 */
#define SSL_ALERT_MSG_DECODE_ERROR          50  /* 0x32 */
#define SSL_ALERT_MSG_DECRYPT_ERROR         51  /* 0x33 */
#define SSL_ALERT_MSG_EXPORT_RESTRICTION    60  /* 0x3C */
#define SSL_ALERT_MSG_PROTOCOL_VERSION      70  /* 0x46 */
#define SSL_ALERT_MSG_INSUFFICIENT_SECURITY 71  /* 0x47 */
#define SSL_ALERT_MSG_INTERNAL_ERROR        80  /* 0x50 */
#define SSL_ALERT_MSG_USER_CANCELED         90  /* 0x5A */
#define SSL_ALERT_MSG_NO_RENEGOTIATION     100  /* 0x64 */
#define SSL_ALERT_MSG_UNSUPPORTED_EXT      110  /* 0x6E */
#define SSL_ALERT_MSG_UNRECOGNIZED_NAME    112  /* 0x70 */
#define SSL_ALERT_MSG_UNKNOWN_PSK_IDENTITY 115  /* 0x73 */
#define SSL_ALERT_MSG_NO_APPLICATION_PROTOCOL 120 /* 0x78 */

#define SSL_HS_HELLO_REQUEST            0
#define SSL_HS_CLIENT_HELLO             1
#define SSL_HS_SERVER_HELLO             2
#define SSL_HS_NEW_SESSION_TICKET       4
#define SSL_HS_CERTIFICATE             11
#define SSL_HS_SERVER_KEY_EXCHANGE     12
#define SSL_HS_CERTIFICATE_REQUEST     13
#define SSL_HS_SERVER_HELLO_DONE       14
#define SSL_HS_CERTIFICATE_VERIFY      15
#define SSL_HS_CLIENT_KEY_EXCHANGE     16
#define SSL_HS_FINISHED                20

/*
 * TLS extensions
 */
#define TLS_EXT_SERVERNAME                   0
#define TLS_EXT_SERVERNAME_HOSTNAME          0

#define TLS_EXT_MAX_FRAGMENT_LENGTH          1

#define TLS_EXT_TRUNCATED_HMAC               4

#define TLS_EXT_SUPPORTED_ELLIPTIC_CURVES   10
#define TLS_EXT_SUPPORTED_POINT_FORMATS     11

#define TLS_EXT_SIG_ALG                     13

#define TLS_EXT_ALPN                        16

#define TLS_EXT_SESSION_TICKET              35

#define TLS_EXT_RENEGOTIATION_INFO      0xFF01

/*
 * TLS extension flags (for extensions with outgoing ServerHello content
 * that need it (e.g. for RENEGOTIATION_INFO the server already knows because
 * of state of the renegotiation flag, so no indicator is required)
 */
#define TLS_EXT_SUPPORTED_POINT_FORMATS_PRESENT (1 << 0)

/*
 * Size defines
 */
#if !defined(POLARSSL_PSK_MAX_LEN)
#define POLARSSL_PSK_MAX_LEN            32 /* 256 bits */
#endif

/* Dummy type used only for its size */
union _ssl_premaster_secret
{
#if defined(POLARSSL_KEY_EXCHANGE_RSA_ENABLED)
    unsigned char _pms_rsa[48];                         /* RFC 5246 8.1.1 */
#endif
#if defined(POLARSSL_KEY_EXCHANGE_DHE_RSA_ENABLED)
    unsigned char _pms_dhm[POLARSSL_MPI_MAX_SIZE];      /* RFC 5246 8.1.2 */
#endif
#if defined(POLARSSL_KEY_EXCHANGE_ECDHE_RSA_ENABLED)    || \
    defined(POLARSSL_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED)  || \
    defined(POLARSSL_KEY_EXCHANGE_ECDH_RSA_ENABLED)     || \
    defined(POLARSSL_KEY_EXCHANGE_ECDH_ECDSA_ENABLED)
    unsigned char _pms_ecdh[POLARSSL_ECP_MAX_BYTES];    /* RFC 4492 5.10 */
#endif
#if defined(POLARSSL_KEY_EXCHANGE_PSK_ENABLED)
    unsigned char _pms_psk[4 + 2 * POLARSSL_PSK_MAX_LEN];       /* RFC 4279 2 */
#endif
#if defined(POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED)
    unsigned char _pms_dhe_psk[4 + POLARSSL_MPI_MAX_SIZE
                                 + POLARSSL_PSK_MAX_LEN];       /* RFC 4279 3 */
#endif
#if defined(POLARSSL_KEY_EXCHANGE_RSA_PSK_ENABLED)
    unsigned char _pms_rsa_psk[52 + POLARSSL_PSK_MAX_LEN];      /* RFC 4279 4 */
#endif
#if defined(POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED)
    unsigned char _pms_ecdhe_psk[4 + POLARSSL_ECP_MAX_BYTES
                                   + POLARSSL_PSK_MAX_LEN];     /* RFC 5489 2 */
#endif
};

#define POLARSSL_PREMASTER_SIZE     sizeof( union _ssl_premaster_secret )

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Generic function pointers for allowing external RSA private key
 * implementations.
 */
typedef int (*rsa_decrypt_func)( void *ctx, int mode, size_t *olen,
                        const unsigned char *input, unsigned char *output,
                        size_t output_max_len );
typedef int (*rsa_sign_func)( void *ctx,
                     int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                     int mode, md_type_t md_alg, unsigned int hashlen,
                     const unsigned char *hash, unsigned char *sig );
typedef size_t (*rsa_key_len_func)( void *ctx );

/*
 * SSL state machine
 */
typedef enum
{
    SSL_HELLO_REQUEST,
    SSL_CLIENT_HELLO,
    SSL_SERVER_HELLO,
    SSL_SERVER_CERTIFICATE,
    SSL_SERVER_KEY_EXCHANGE,
    SSL_CERTIFICATE_REQUEST,
    SSL_SERVER_HELLO_DONE,
    SSL_CLIENT_CERTIFICATE,
    SSL_CLIENT_KEY_EXCHANGE,
    SSL_CERTIFICATE_VERIFY,
    SSL_CLIENT_CHANGE_CIPHER_SPEC,
    SSL_CLIENT_FINISHED,
    SSL_SERVER_CHANGE_CIPHER_SPEC,
    SSL_SERVER_FINISHED,
    SSL_FLUSH_BUFFERS,
    SSL_HANDSHAKE_WRAPUP,
    SSL_HANDSHAKE_OVER,
    SSL_SERVER_NEW_SESSION_TICKET,
}
ssl_states;

typedef struct _ssl_session ssl_session;
typedef struct _ssl_context ssl_context;
typedef struct _ssl_transform ssl_transform;
typedef struct _ssl_handshake_params ssl_handshake_params;
#if defined(POLARSSL_SSL_SESSION_TICKETS)
typedef struct _ssl_ticket_keys ssl_ticket_keys;
#endif
#if defined(POLARSSL_X509_CRT_PARSE_C)
typedef struct _ssl_key_cert ssl_key_cert;
#endif

/*
 * This structure is used for storing current session data.
 */
struct _ssl_session
{
#if defined(POLARSSL_HAVE_TIME)
    time_t start;               /*!< starting time      */
#endif
    int ciphersuite;            /*!< chosen ciphersuite */
    int compression;            /*!< chosen compression */
    size_t length;              /*!< session id length  */
    unsigned char id[32];       /*!< session identifier */
    unsigned char master[48];   /*!< the master secret  */

#if defined(POLARSSL_X509_CRT_PARSE_C)
    x509_crt *peer_cert;        /*!< peer X.509 cert chain */
#endif /* POLARSSL_X509_CRT_PARSE_C */
    int verify_result;          /*!<  verification result     */

#if defined(POLARSSL_SSL_SESSION_TICKETS)
    unsigned char *ticket;      /*!< RFC 5077 session ticket */
    size_t ticket_len;          /*!< session ticket length   */
    uint32_t ticket_lifetime;   /*!< ticket lifetime hint    */
#endif /* POLARSSL_SSL_SESSION_TICKETS */

#if defined(POLARSSL_SSL_MAX_FRAGMENT_LENGTH)
    unsigned char mfl_code;     /*!< MaxFragmentLength negotiated by peer */
#endif /* POLARSSL_SSL_MAX_FRAGMENT_LENGTH */

#if defined(POLARSSL_SSL_TRUNCATED_HMAC)
    int trunc_hmac;             /*!< flag for truncated hmac activation   */
#endif /* POLARSSL_SSL_TRUNCATED_HMAC */
};

/*
 * This structure contains a full set of runtime transform parameters
 * either in negotiation or active.
 */
struct _ssl_transform
{
    /*
     * Session specific crypto layer
     */
    const ssl_ciphersuite_t *ciphersuite_info;
                                        /*!<  Chosen cipersuite_info  */
    unsigned int keylen;                /*!<  symmetric key length    */
    size_t minlen;                      /*!<  min. ciphertext length  */
    size_t ivlen;                       /*!<  IV length               */
    size_t fixed_ivlen;                 /*!<  Fixed part of IV (AEAD) */
    size_t maclen;                      /*!<  MAC length              */

    unsigned char iv_enc[16];           /*!<  IV (encryption)         */
    unsigned char iv_dec[16];           /*!<  IV (decryption)         */

#if defined(POLARSSL_SSL_PROTO_SSL3)
    /* Needed only for SSL v3.0 secret */
    unsigned char mac_enc[48];          /*!<  SSL v3.0 secret (enc)   */
    unsigned char mac_dec[48];          /*!<  SSL v3.0 secret (dec)   */
#endif /* POLARSSL_SSL_PROTO_SSL3 */

    md_context_t md_ctx_enc;            /*!<  MAC (encryption)        */
    md_context_t md_ctx_dec;            /*!<  MAC (decryption)        */

    cipher_context_t cipher_ctx_enc;    /*!<  encryption context      */
    cipher_context_t cipher_ctx_dec;    /*!<  decryption context      */

    /*
     * Session specific compression layer
     */
#if defined(POLARSSL_ZLIB_SUPPORT)
    z_stream ctx_deflate;               /*!<  compression context     */
    z_stream ctx_inflate;               /*!<  decompression context   */
#endif
};

/*
 * This structure contains the parameters only needed during handshake.
 */
struct _ssl_handshake_params
{
    /*
     * Handshake specific crypto variables
     */
    int sig_alg;                        /*!<  Hash algorithm for signature   */
    int cert_type;                      /*!<  Requested cert type            */
    int verify_sig_alg;                 /*!<  Signature algorithm for verify */
#if defined(POLARSSL_DHM_C)
    dhm_context dhm_ctx;                /*!<  DHM key exchange        */
#endif
#if defined(POLARSSL_ECDH_C)
    ecdh_context ecdh_ctx;              /*!<  ECDH key exchange       */
#endif
#if defined(POLARSSL_ECDH_C) || defined(POLARSSL_ECDSA_C)
    const ecp_curve_info **curves;      /*!<  Supported elliptic curves */
#endif
#if defined(POLARSSL_X509_CRT_PARSE_C)
    /**
     * Current key/cert or key/cert list.
     * On client: pointer to ssl->key_cert, only the first entry used.
     * On server: starts as a pointer to ssl->key_cert, then becomes
     * a pointer to the chosen key from this list or the SNI list.
     */
    ssl_key_cert *key_cert;
#if defined(POLARSSL_SSL_SERVER_NAME_INDICATION)
    ssl_key_cert *sni_key_cert;         /*!<  key/cert list from SNI  */
#endif
#endif /* POLARSSL_X509_CRT_PARSE_C */

    /*
     * Checksum contexts
     */
#if defined(POLARSSL_SSL_PROTO_SSL3) || defined(POLARSSL_SSL_PROTO_TLS1) || \
    defined(POLARSSL_SSL_PROTO_TLS1_1)
       md5_context fin_md5;
      sha1_context fin_sha1;
#endif
#if defined(POLARSSL_SSL_PROTO_TLS1_2)
#if defined(POLARSSL_SHA256_C)
    sha256_context fin_sha256;
#endif
#if defined(POLARSSL_SHA512_C)
    sha512_context fin_sha512;
#endif
#endif /* POLARSSL_SSL_PROTO_TLS1_2 */

    void (*update_checksum)(ssl_context *, const unsigned char *, size_t);
    void (*calc_verify)(ssl_context *, unsigned char *);
    void (*calc_finished)(ssl_context *, unsigned char *, int);
    int  (*tls_prf)(const unsigned char *, size_t, const char *,
                    const unsigned char *, size_t,
                    unsigned char *, size_t);

    size_t pmslen;                      /*!<  premaster length        */

    unsigned char randbytes[64];        /*!<  random bytes            */
    unsigned char premaster[POLARSSL_PREMASTER_SIZE];
                                        /*!<  premaster secret        */

    int resume;                         /*!<  session resume indicator*/
    int max_major_ver;                  /*!< max. major version client*/
    int max_minor_ver;                  /*!< max. minor version client*/
    int cli_exts;                       /*!< client extension presence*/

#if defined(POLARSSL_SSL_SESSION_TICKETS)
    int new_session_ticket;             /*!< use NewSessionTicket?    */
#endif /* POLARSSL_SSL_SESSION_TICKETS */
};

#if defined(POLARSSL_SSL_SESSION_TICKETS)
/*
 * Parameters needed to secure session tickets
 */
struct _ssl_ticket_keys
{
    unsigned char key_name[16];     /*!< name to quickly discard bad tickets */
    aes_context enc;                /*!< encryption context                  */
    aes_context dec;                /*!< decryption context                  */
    unsigned char mac_key[16];      /*!< authentication key                  */
};
#endif /* POLARSSL_SSL_SESSION_TICKETS */

#if defined(POLARSSL_X509_CRT_PARSE_C)
/*
 * List of certificate + private key pairs
 */
struct _ssl_key_cert
{
    x509_crt *cert;                 /*!< cert                       */
    pk_context *key;                /*!< private key                */
    int key_own_alloc;              /*!< did we allocate key?       */
    ssl_key_cert *next;             /*!< next key/cert pair         */
};
#endif /* POLARSSL_X509_CRT_PARSE_C */

struct _ssl_context
{
    /*
     * Miscellaneous
     */
    int state;                  /*!< SSL handshake: current state     */
    int renegotiation;          /*!< Initial or renegotiation         */
    int renego_records_seen;    /*!< Records since renego request     */

    int major_ver;              /*!< equal to  SSL_MAJOR_VERSION_3    */
    int minor_ver;              /*!< either 0 (SSL3) or 1 (TLS1.0)    */

    int max_major_ver;          /*!< max. major version used          */
    int max_minor_ver;          /*!< max. minor version used          */
    int min_major_ver;          /*!< min. major version used          */
    int min_minor_ver;          /*!< min. minor version used          */

    /*
     * Callbacks (RNG, debug, I/O, verification)
     */
    int  (*f_rng)(void *, unsigned char *, size_t);
    void (*f_dbg)(void *, int, const char *);
    int (*f_recv)(void *, unsigned char *, size_t);
    int (*f_send)(void *, const unsigned char *, size_t);
    int (*f_get_cache)(void *, ssl_session *);
    int (*f_set_cache)(void *, const ssl_session *);

    void *p_rng;                /*!< context for the RNG function     */
    void *p_dbg;                /*!< context for the debug function   */
    void *p_recv;               /*!< context for reading operations   */
    void *p_send;               /*!< context for writing operations   */
    void *p_get_cache;          /*!< context for cache retrieval      */
    void *p_set_cache;          /*!< context for cache store          */
    void *p_hw_data;            /*!< context for HW acceleration      */

#if defined(POLARSSL_SSL_SERVER_NAME_INDICATION)
    int (*f_sni)(void *, ssl_context *, const unsigned char *, size_t);
    void *p_sni;                /*!< context for SNI extension        */
#endif

#if defined(POLARSSL_X509_CRT_PARSE_C)
    int (*f_vrfy)(void *, x509_crt *, int, int *);
    void *p_vrfy;               /*!< context for verification         */
#endif

#if defined(POLARSSL_KEY_EXCHANGE__SOME__PSK_ENABLED)
    int (*f_psk)(void *, ssl_context *, const unsigned char *, size_t);
    void *p_psk;               /*!< context for PSK retrieval         */
#endif

    /*
     * Session layer
     */
    ssl_session *session_in;            /*!<  current session data (in)   */
    ssl_session *session_out;           /*!<  current session data (out)  */
    ssl_session *session;               /*!<  negotiated session data     */
    ssl_session *session_negotiate;     /*!<  session data in negotiation */

    ssl_handshake_params *handshake;    /*!<  params required only during
                                              the handshake process        */

    /*
     * Record layer transformations
     */
    ssl_transform *transform_in;        /*!<  current transform params (in)   */
    ssl_transform *transform_out;       /*!<  current transform params (in)   */
    ssl_transform *transform;           /*!<  negotiated transform params     */
    ssl_transform *transform_negotiate; /*!<  transform params in negotiation */

    /*
     * Record layer (incoming data)
     */
    unsigned char *in_ctr;      /*!< 64-bit incoming message counter  */
    unsigned char *in_hdr;      /*!< 5-byte record header (in_ctr+8)  */
    unsigned char *in_iv;       /*!< ivlen-byte IV (in_hdr+5)         */
    unsigned char *in_msg;      /*!< message contents (in_iv+ivlen)   */
    unsigned char *in_offt;     /*!< read offset in application data  */

    int in_msgtype;             /*!< record header: message type      */
    size_t in_msglen;           /*!< record header: message length    */
    size_t in_left;             /*!< amount of data read so far       */

    size_t in_hslen;            /*!< current handshake message length */
    int nb_zero;                /*!< # of 0-length encrypted messages */
    int record_read;            /*!< record is already present        */

    /*
     * Record layer (outgoing data)
     */
    unsigned char *out_ctr;     /*!< 64-bit outgoing message counter  */
    unsigned char *out_hdr;     /*!< 5-byte record header (out_ctr+8) */
    unsigned char *out_iv;      /*!< ivlen-byte IV (out_hdr+5)        */
    unsigned char *out_msg;     /*!< message contents (out_iv+ivlen)  */

    int out_msgtype;            /*!< record header: message type      */
    size_t out_msglen;          /*!< record header: message length    */
    size_t out_left;            /*!< amount of data not yet written   */

#if defined(POLARSSL_ZLIB_SUPPORT)
    unsigned char *compress_buf;        /*!<  zlib data buffer        */
#endif
#if defined(POLARSSL_SSL_MAX_FRAGMENT_LENGTH)
    unsigned char mfl_code;     /*!< MaxFragmentLength chosen by us   */
#endif /* POLARSSL_SSL_MAX_FRAGMENT_LENGTH */

    /*
     * PKI layer
     */
#if defined(POLARSSL_X509_CRT_PARSE_C)
    ssl_key_cert *key_cert;             /*!<  own certificate(s)/key(s) */

    x509_crt *ca_chain;                 /*!<  own trusted CA chain      */
    x509_crl *ca_crl;                   /*!<  trusted CA CRLs           */
    const char *peer_cn;                /*!<  expected peer CN          */
#endif /* POLARSSL_X509_CRT_PARSE_C */

    /*
     * Support for generating and checking session tickets
     */
#if defined(POLARSSL_SSL_SESSION_TICKETS)
    ssl_ticket_keys *ticket_keys;       /*!<  keys for ticket encryption */
#endif /* POLARSSL_SSL_SESSION_TICKETS */

    /*
     * User settings
     */
    int endpoint;                       /*!<  0: client, 1: server    */
    int authmode;                       /*!<  verification mode       */
    int client_auth;                    /*!<  flag for client auth.   */
    int verify_result;                  /*!<  verification result     */
    int disable_renegotiation;          /*!<  enable/disable renegotiation   */
    int allow_legacy_renegotiation;     /*!<  allow legacy renegotiation     */
    int renego_max_records;             /*!<  grace period for renegotiation */
    const int *ciphersuite_list[4];     /*!<  allowed ciphersuites / version */
#if defined(POLARSSL_SSL_SET_CURVES)
    const ecp_group_id *curve_list;     /*!<  allowed curves                 */
#endif
#if defined(POLARSSL_SSL_TRUNCATED_HMAC)
    int trunc_hmac;                     /*!<  negotiate truncated hmac?      */
#endif
#if defined(POLARSSL_SSL_SESSION_TICKETS)
    int session_tickets;                /*!<  use session tickets?    */
    int ticket_lifetime;                /*!<  session ticket lifetime */
#endif

#if defined(POLARSSL_DHM_C)
    mpi dhm_P;                          /*!<  prime modulus for DHM   */
    mpi dhm_G;                          /*!<  generator for DHM       */
#endif

#if defined(POLARSSL_KEY_EXCHANGE__SOME__PSK_ENABLED)
    /*
     * PSK values
     */
    unsigned char *psk;
    size_t         psk_len;
    unsigned char *psk_identity;
    size_t         psk_identity_len;
#endif

#if defined(POLARSSL_SSL_SERVER_NAME_INDICATION)
    /*
     * SNI extension
     */
    unsigned char *hostname;
    size_t         hostname_len;
#endif

#if defined(POLARSSL_SSL_ALPN)
    /*
     * ALPN extension
     */
    const char **alpn_list;     /*!<  ordered list of supported protocols   */
    const char *alpn_chosen;    /*!<  negotiated protocol                   */
#endif

    /*
     * Secure renegotiation
     */
    int secure_renegotiation;           /*!<  does peer support legacy or
                                              secure renegotiation           */
    size_t verify_data_len;             /*!<  length of verify data stored   */
    char own_verify_data[36];           /*!<  previous handshake verify data */
    char peer_verify_data[36];          /*!<  previous handshake verify data */
};

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)

#define SSL_CHANNEL_OUTBOUND    0
#define SSL_CHANNEL_INBOUND     1

extern int (*ssl_hw_record_init)(ssl_context *ssl,
                const unsigned char *key_enc, const unsigned char *key_dec,
                size_t keylen,
                const unsigned char *iv_enc,  const unsigned char *iv_dec,
                size_t ivlen,
                const unsigned char *mac_enc, const unsigned char *mac_dec,
                size_t maclen);
extern int (*ssl_hw_record_activate)(ssl_context *ssl, int direction);
extern int (*ssl_hw_record_reset)(ssl_context *ssl);
extern int (*ssl_hw_record_write)(ssl_context *ssl);
extern int (*ssl_hw_record_read)(ssl_context *ssl);
extern int (*ssl_hw_record_finish)(ssl_context *ssl);
#endif /* POLARSSL_SSL_HW_RECORD_ACCEL */

/**
 * \brief Returns the list of ciphersuites supported by the SSL/TLS module.
 *
 * \return              a statically allocated array of ciphersuites, the last
 *                      entry is 0.
 */
const int *ssl_list_ciphersuites( void );

/**
 * \brief               Return the name of the ciphersuite associated with the
 *                      given ID
 *
 * \param ciphersuite_id SSL ciphersuite ID
 *
 * \return              a string containing the ciphersuite name
 */
const char *ssl_get_ciphersuite_name( const int ciphersuite_id );

/**
 * \brief               Return the ID of the ciphersuite associated with the
 *                      given name
 *
 * \param ciphersuite_name SSL ciphersuite name
 *
 * \return              the ID with the ciphersuite or 0 if not found
 */
int ssl_get_ciphersuite_id( const char *ciphersuite_name );

/**
 * \brief          Initialize an SSL context
 *                 (An individual SSL context is not thread-safe)
 *
 * \param ssl      SSL context
 *
 * \return         0 if successful, or POLARSSL_ERR_SSL_MALLOC_FAILED if
 *                 memory allocation failed
 */
int ssl_init( ssl_context *ssl );

/**
 * \brief          Reset an already initialized SSL context for re-use
 *                 while retaining application-set variables, function
 *                 pointers and data.
 *
 * \param ssl      SSL context
 * \return         0 if successful, or POLASSL_ERR_SSL_MALLOC_FAILED,
                   POLARSSL_ERR_SSL_HW_ACCEL_FAILED or
 *                 POLARSSL_ERR_SSL_COMPRESSION_FAILED
 */
int ssl_session_reset( ssl_context *ssl );

/**
 * \brief          Set the current endpoint type
 *
 * \param ssl      SSL context
 * \param endpoint must be SSL_IS_CLIENT or SSL_IS_SERVER
 *
 * \note           This function should be called right after ssl_init() since
 *                 some other ssl_set_foo() functions depend on it.
 */
void ssl_set_endpoint( ssl_context *ssl, int endpoint );

/**
 * \brief          Set the certificate verification mode
 *
 * \param ssl      SSL context
 * \param authmode can be:
 *
 *  SSL_VERIFY_NONE:      peer certificate is not checked (default),
 *                        this is insecure and SHOULD be avoided.
 *
 *  SSL_VERIFY_OPTIONAL:  peer certificate is checked, however the
 *                        handshake continues even if verification failed;
 *                        ssl_get_verify_result() can be called after the
 *                        handshake is complete.
 *
 *  SSL_VERIFY_REQUIRED:  peer *must* present a valid certificate,
 *                        handshake is aborted if verification failed.
 *
 * \note On client, SSL_VERIFY_REQUIRED is the recommended mode.
 * With SSL_VERIFY_OPTIONAL, the user needs to call ssl_get_verify_result() at
 * the right time(s), which may not be obvious, while REQUIRED always perform
 * the verification as soon as possible. For example, REQUIRED was protecting
 * against the "triple handshake" attack even before it was found.
 */
void ssl_set_authmode( ssl_context *ssl, int authmode );

#if defined(POLARSSL_X509_CRT_PARSE_C)
/**
 * \brief          Set the verification callback (Optional).
 *
 *                 If set, the verify callback is called for each
 *                 certificate in the chain. For implementation
 *                 information, please see \c x509parse_verify()
 *
 * \param ssl      SSL context
 * \param f_vrfy   verification function
 * \param p_vrfy   verification parameter
 */
void ssl_set_verify( ssl_context *ssl,
                     int (*f_vrfy)(void *, x509_crt *, int, int *),
                     void *p_vrfy );
#endif /* POLARSSL_X509_CRT_PARSE_C */

/**
 * \brief          Set the random number generator callback
 *
 * \param ssl      SSL context
 * \param f_rng    RNG function
 * \param p_rng    RNG parameter
 */
void ssl_set_rng( ssl_context *ssl,
                  int (*f_rng)(void *, unsigned char *, size_t),
                  void *p_rng );

/**
 * \brief          Set the debug callback
 *
 * \param ssl      SSL context
 * \param f_dbg    debug function
 * \param p_dbg    debug parameter
 */
void ssl_set_dbg( ssl_context *ssl,
                  void (*f_dbg)(void *, int, const char *),
                  void  *p_dbg );

/**
 * \brief          Set the underlying BIO read and write callbacks
 *
 * \param ssl      SSL context
 * \param f_recv   read callback
 * \param p_recv   read parameter
 * \param f_send   write callback
 * \param p_send   write parameter
 */
void ssl_set_bio( ssl_context *ssl,
        int (*f_recv)(void *, unsigned char *, size_t), void *p_recv,
        int (*f_send)(void *, const unsigned char *, size_t), void *p_send );

/**
 * \brief          Set the session cache callbacks (server-side only)
 *                 If not set, no session resuming is done.
 *
 *                 The session cache has the responsibility to check for stale
 *                 entries based on timeout. See RFC 5246 for recommendations.
 *
 *                 Warning: session.peer_cert is cleared by the SSL/TLS layer on
 *                 connection shutdown, so do not cache the pointer! Either set
 *                 it to NULL or make a full copy of the certificate.
 *
 *                 The get callback is called once during the initial handshake
 *                 to enable session resuming. The get function has the
 *                 following parameters: (void *parameter, ssl_session *session)
 *                 If a valid entry is found, it should fill the master of
 *                 the session object with the cached values and return 0,
 *                 return 1 otherwise. Optionally peer_cert can be set as well
 *                 if it is properly present in cache entry.
 *
 *                 The set callback is called once during the initial handshake
 *                 to enable session resuming after the entire handshake has
 *                 been finished. The set function has the following parameters:
 *                 (void *parameter, const ssl_session *session). The function
 *                 should create a cache entry for future retrieval based on
 *                 the data in the session structure and should keep in mind
 *                 that the ssl_session object presented (and all its referenced
 *                 data) is cleared by the SSL/TLS layer when the connection is
 *                 terminated. It is recommended to add metadata to determine if
 *                 an entry is still valid in the future. Return 0 if
 *                 successfully cached, return 1 otherwise.
 *
 * \param ssl            SSL context
 * \param f_get_cache    session get callback
 * \param p_get_cache    session get parameter
 * \param f_set_cache    session set callback
 * \param p_set_cache    session set parameter
 */
void ssl_set_session_cache( ssl_context *ssl,
        int (*f_get_cache)(void *, ssl_session *), void *p_get_cache,
        int (*f_set_cache)(void *, const ssl_session *), void *p_set_cache );

/**
 * \brief          Request resumption of session (client-side only)
 *                 Session data is copied from presented session structure.
 *
 * \param ssl      SSL context
 * \param session  session context
 *
 * \return         0 if successful,
 *                 POLARSSL_ERR_SSL_MALLOC_FAILED if memory allocation failed,
 *                 POLARSSL_ERR_SSL_BAD_INPUT_DATA if used server-side or
 *                 arguments are otherwise invalid
 *
 * \sa             ssl_get_session()
 */
int ssl_set_session( ssl_context *ssl, const ssl_session *session );

/**
 * \brief               Set the list of allowed ciphersuites and the preference
 *                      order. First in the list has the highest preference.
 *                      (Overrides all version specific lists)
 *
 *                      Note: The PolarSSL SSL server uses its own preferences
 *                      over the preference of the connection SSL client unless
 *                      POLARSSL_SSL_SRV_RESPECT_CLIENT_PREFERENCE is defined!
 *
 * \param ssl           SSL context
 * \param ciphersuites  0-terminated list of allowed ciphersuites
 */
void ssl_set_ciphersuites( ssl_context *ssl, const int *ciphersuites );

/**
 * \brief               Set the list of allowed ciphersuites and the
 *                      preference order for a specific version of the protocol.
 *                      (Only useful on the server side)
 *
 * \param ssl           SSL context
 * \param ciphersuites  0-terminated list of allowed ciphersuites
 * \param major         Major version number (only SSL_MAJOR_VERSION_3
 *                      supported)
 * \param minor         Minor version number (SSL_MINOR_VERSION_0,
 *                      SSL_MINOR_VERSION_1 and SSL_MINOR_VERSION_2,
 *                      SSL_MINOR_VERSION_3 supported)
 */
void ssl_set_ciphersuites_for_version( ssl_context *ssl,
                                       const int *ciphersuites,
                                       int major, int minor );

#if defined(POLARSSL_X509_CRT_PARSE_C)
/**
 * \brief          Set the data required to verify peer certificate
 *
 * \param ssl      SSL context
 * \param ca_chain trusted CA chain (meaning all fully trusted top-level CAs)
 * \param ca_crl   trusted CA CRLs
 * \param peer_cn  expected peer CommonName (or NULL)
 */
void ssl_set_ca_chain( ssl_context *ssl, x509_crt *ca_chain,
                       x509_crl *ca_crl, const char *peer_cn );

/**
 * \brief          Set own certificate chain and private key
 *
 * \note           own_cert should contain in order from the bottom up your
 *                 certificate chain. The top certificate (self-signed)
 *                 can be omitted.
 *
 * \note           This function may be called more than once if you want to
 *                 support multiple certificates (eg, one using RSA and one
 *                 using ECDSA). However, on client, currently only the first
 *                 certificate is used (subsequent calls have no effect).
 *
 * \param ssl      SSL context
 * \param own_cert own public certificate chain
 * \param pk_key   own private key
 *
 * \return         0 on success or POLARSSL_ERR_SSL_MALLOC_FAILED
 */
int ssl_set_own_cert( ssl_context *ssl, x509_crt *own_cert,
                       pk_context *pk_key );

#if defined(POLARSSL_RSA_C)
/**
 * \brief          Set own certificate chain and private RSA key
 *
 *                 Note: own_cert should contain IN order from the bottom
 *                 up your certificate chain. The top certificate (self-signed)
 *                 can be omitted.
 *
 * \warning        This backwards-compatibility function is deprecated!
 *                 Please use \c ssl_set_own_cert() instead.
 *
 * \param ssl      SSL context
 * \param own_cert own public certificate chain
 * \param rsa_key  own private RSA key
 *
 * \return          0 on success, or a specific error code.
 */
int ssl_set_own_cert_rsa( ssl_context *ssl, x509_crt *own_cert,
                          rsa_context *rsa_key );
#endif /* POLARSSL_RSA_C */

/**
 * \brief          Set own certificate and alternate non-PolarSSL RSA private
 *                 key and handling callbacks, such as the PKCS#11 wrappers
 *                 or any other external private key handler.
 *                 (see the respective RSA functions in rsa.h for documentation
 *                 of the callback parameters, with the only change being
 *                 that the rsa_context * is a void * in the callbacks)
 *
 *                 Note: own_cert should contain IN order from the bottom
 *                 up your certificate chain. The top certificate (self-signed)
 *                 can be omitted.
 *
 * \warning        This backwards-compatibility function is deprecated!
 *                 Please use \c pk_init_ctx_rsa_alt()
 *                 and \c ssl_set_own_cert() instead.
 *
 * \param ssl      SSL context
 * \param own_cert own public certificate chain
 * \param rsa_key  alternate implementation private RSA key
 * \param rsa_decrypt  alternate implementation of \c rsa_pkcs1_decrypt()
 * \param rsa_sign     alternate implementation of \c rsa_pkcs1_sign()
 * \param rsa_key_len  function returning length of RSA key in bytes
 *
 * \return          0 on success, or a specific error code.
 */
int ssl_set_own_cert_alt( ssl_context *ssl, x509_crt *own_cert,
                          void *rsa_key,
                          rsa_decrypt_func rsa_decrypt,
                          rsa_sign_func rsa_sign,
                          rsa_key_len_func rsa_key_len );
#endif /* POLARSSL_X509_CRT_PARSE_C */

#if defined(POLARSSL_KEY_EXCHANGE__SOME__PSK_ENABLED)
/**
 * \brief          Set the Pre Shared Key (PSK) and the identity name connected
 *                 to it.
 *
 * \param ssl      SSL context
 * \param psk      pointer to the pre-shared key
 * \param psk_len  pre-shared key length
 * \param psk_identity      pointer to the pre-shared key identity
 * \param psk_identity_len  identity key length
 *
 * \return         0 if successful or POLARSSL_ERR_SSL_MALLOC_FAILED
 */
int ssl_set_psk( ssl_context *ssl, const unsigned char *psk, size_t psk_len,
                 const unsigned char *psk_identity, size_t psk_identity_len );

/**
 * \brief          Set the PSK callback (server-side only) (Optional).
 *
 *                 If set, the PSK callback is called for each
 *                 handshake where a PSK ciphersuite was negotiated.
 *                 The caller provides the identity received and wants to
 *                 receive the actual PSK data and length.
 *
 *                 The callback has the following parameters: (void *parameter,
 *                 ssl_context *ssl, const unsigned char *psk_identity,
 *                 size_t identity_len)
 *                 If a valid PSK identity is found, the callback should use
 *                 ssl_set_psk() on the ssl context to set the correct PSK and
 *                 identity and return 0.
 *                 Any other return value will result in a denied PSK identity.
 *
 * \param ssl      SSL context
 * \param f_psk    PSK identity function
 * \param p_psk    PSK identity parameter
 */
void ssl_set_psk_cb( ssl_context *ssl,
                     int (*f_psk)(void *, ssl_context *, const unsigned char *,
                                  size_t),
                     void *p_psk );
#endif /* POLARSSL_KEY_EXCHANGE__SOME__PSK_ENABLED */

#if defined(POLARSSL_DHM_C)
/**
 * \brief          Set the Diffie-Hellman public P and G values,
 *                 read as hexadecimal strings (server-side only)
 *                 (Default: POLARSSL_DHM_RFC5114_MODP_1024_[PG])
 *
 * \param ssl      SSL context
 * \param dhm_P    Diffie-Hellman-Merkle modulus
 * \param dhm_G    Diffie-Hellman-Merkle generator
 *
 * \return         0 if successful
 */
int ssl_set_dh_param( ssl_context *ssl, const char *dhm_P, const char *dhm_G );

/**
 * \brief          Set the Diffie-Hellman public P and G values,
 *                 read from existing context (server-side only)
 *
 * \param ssl      SSL context
 * \param dhm_ctx  Diffie-Hellman-Merkle context
 *
 * \return         0 if successful
 */
int ssl_set_dh_param_ctx( ssl_context *ssl, dhm_context *dhm_ctx );
#endif /* POLARSSL_DHM_C */

#if defined(POLARSSL_SSL_SET_CURVES)
/**
 * \brief          Set the allowed curves in order of preference.
 *                 (Default: all defined curves.)
 *
 *                 On server: this only affects selection of the ECDHE curve;
 *                 the curves used for ECDH and ECDSA are determined by the
 *                 list of available certificates instead.
 *
 *                 On client: this affects the list of curves offered for any
 *                 use. The server can override our preference order.
 *
 *                 Both sides: limits the set of curves used by peer to the
 *                 listed curves for any use (ECDH(E), certificates).
 *
 * \param ssl      SSL context
 * \param curves   Ordered list of allowed curves,
 *                 terminated by POLARSSL_ECP_DP_NONE.
 */
void ssl_set_curves( ssl_context *ssl, const ecp_group_id *curves );
#endif /* POLARSSL_SSL_SET_CURVES */

#if defined(POLARSSL_SSL_SERVER_NAME_INDICATION)
/**
 * \brief          Set hostname for ServerName TLS extension
 *                 (client-side only)
 *
 *
 * \param ssl      SSL context
 * \param hostname the server hostname
 *
 * \return         0 if successful or POLARSSL_ERR_SSL_MALLOC_FAILED
 */
int ssl_set_hostname( ssl_context *ssl, const char *hostname );

/**
 * \brief          Set server side ServerName TLS extension callback
 *                 (optional, server-side only).
 *
 *                 If set, the ServerName callback is called whenever the
 *                 server receives a ServerName TLS extension from the client
 *                 during a handshake. The ServerName callback has the
 *                 following parameters: (void *parameter, ssl_context *ssl,
 *                 const unsigned char *hostname, size_t len). If a suitable
 *                 certificate is found, the callback should set the
 *                 certificate and key to use with ssl_set_own_cert() (and
 *                 possibly adjust the CA chain as well) and return 0. The
 *                 callback should return -1 to abort the handshake at this
 *                 point.
 *
 * \param ssl      SSL context
 * \param f_sni    verification function
 * \param p_sni    verification parameter
 */
void ssl_set_sni( ssl_context *ssl,
                  int (*f_sni)(void *, ssl_context *, const unsigned char *,
                               size_t),
                  void *p_sni );
#endif /* POLARSSL_SSL_SERVER_NAME_INDICATION */

#if defined(POLARSSL_SSL_ALPN)
/**
 * \brief          Set the supported Application Layer Protocols.
 *
 * \param ssl      SSL context
 * \param protos   NULL-terminated list of supported protocols,
 *                 in decreasing preference order.
 *
 * \return         0 on success, or POLARSSL_ERR_SSL_BAD_INPUT_DATA.
 */
int ssl_set_alpn_protocols( ssl_context *ssl, const char **protos );

/**
 * \brief          Get the name of the negotiated Application Layer Protocol.
 *                 This function should be called after the handshake is
 *                 completed.
 *
 * \param ssl      SSL context
 *
 * \return         Protcol name, or NULL if no protocol was negotiated.
 */
const char *ssl_get_alpn_protocol( const ssl_context *ssl );
#endif /* POLARSSL_SSL_ALPN */

/**
 * \brief          Set the maximum supported version sent from the client side
 *                 and/or accepted at the server side
 *                 (Default: SSL_MAX_MAJOR_VERSION, SSL_MAX_MINOR_VERSION)
 *
 *                 Note: This ignores ciphersuites from 'higher' versions.
 *                 Note: Input outside of the SSL_MAX_XXXXX_VERSION and
 *                       SSL_MIN_XXXXX_VERSION range is ignored.
 *
 * \param ssl      SSL context
 * \param major    Major version number (only SSL_MAJOR_VERSION_3 supported)
 * \param minor    Minor version number (SSL_MINOR_VERSION_0,
 *                 SSL_MINOR_VERSION_1 and SSL_MINOR_VERSION_2,
 *                 SSL_MINOR_VERSION_3 supported)
 */
void ssl_set_max_version( ssl_context *ssl, int major, int minor );


/**
 * \brief          Set the minimum accepted SSL/TLS protocol version
 *                 (Default: SSL_MIN_MAJOR_VERSION, SSL_MIN_MINOR_VERSION)
 *
 *                 Note: Input outside of the SSL_MAX_XXXXX_VERSION and
 *                       SSL_MIN_XXXXX_VERSION range is ignored.
 *
 * \param ssl      SSL context
 * \param major    Major version number (only SSL_MAJOR_VERSION_3 supported)
 * \param minor    Minor version number (SSL_MINOR_VERSION_0,
 *                 SSL_MINOR_VERSION_1 and SSL_MINOR_VERSION_2,
 *                 SSL_MINOR_VERSION_3 supported)
 */
void ssl_set_min_version( ssl_context *ssl, int major, int minor );

#if defined(POLARSSL_SSL_MAX_FRAGMENT_LENGTH)
/**
 * \brief          Set the maximum fragment length to emit and/or negotiate
 *                 (Default: SSL_MAX_CONTENT_LEN, usually 2^14 bytes)
 *                 (Server: set maximum fragment length to emit,
 *                 usually negotiated by the client during handshake
 *                 (Client: set maximum fragment length to emit *and*
 *                 negotiate with the server during handshake)
 *
 * \param ssl      SSL context
 * \param mfl_code Code for maximum fragment length (allowed values:
 *                 SSL_MAX_FRAG_LEN_512,  SSL_MAX_FRAG_LEN_1024,
 *                 SSL_MAX_FRAG_LEN_2048, SSL_MAX_FRAG_LEN_4096)
 *
 * \return         O if successful or POLARSSL_ERR_SSL_BAD_INPUT_DATA
 */
int ssl_set_max_frag_len( ssl_context *ssl, unsigned char mfl_code );
#endif /* POLARSSL_SSL_MAX_FRAGMENT_LENGTH */

#if defined(POLARSSL_SSL_TRUNCATED_HMAC)
/**
 * \brief          Activate negotiation of truncated HMAC (Client only)
 *                 (Default: SSL_TRUNC_HMAC_ENABLED)
 *
 * \param ssl      SSL context
 * \param truncate Enable or disable (SSL_TRUNC_HMAC_ENABLED or
 *                                    SSL_TRUNC_HMAC_DISABLED)
 *
 * \return         O if successful,
 *                 POLARSSL_ERR_SSL_BAD_INPUT_DATA if used server-side
 */
int ssl_set_truncated_hmac( ssl_context *ssl, int truncate );
#endif /* POLARSSL_SSL_TRUNCATED_HMAC */

#if defined(POLARSSL_SSL_SESSION_TICKETS)
/**
 * \brief          Enable / Disable session tickets
 *                 (Default: SSL_SESSION_TICKETS_ENABLED on client,
 *                           SSL_SESSION_TICKETS_DISABLED on server)
 *
 * \note           On server, ssl_set_rng() must be called before this function
 *                 to allow generating the ticket encryption and
 *                 authentication keys.
 *
 * \param ssl      SSL context
 * \param use_tickets   Enable or disable (SSL_SESSION_TICKETS_ENABLED or
 *                                         SSL_SESSION_TICKETS_DISABLED)
 *
 * \return         O if successful,
 *                 or a specific error code (server only).
 */
int ssl_set_session_tickets( ssl_context *ssl, int use_tickets );

/**
 * \brief          Set session ticket lifetime (server only)
 *                 (Default: SSL_DEFAULT_TICKET_LIFETIME (86400 secs / 1 day))
 *
 * \param ssl      SSL context
 * \param lifetime session ticket lifetime
 */
void ssl_set_session_ticket_lifetime( ssl_context *ssl, int lifetime );
#endif /* POLARSSL_SSL_SESSION_TICKETS */

/**
 * \brief          Enable / Disable renegotiation support for connection when
 *                 initiated by peer
 *                 (Default: SSL_RENEGOTIATION_DISABLED)
 *
 *                 Note: A server with support enabled is more vulnerable for a
 *                 resource DoS by a malicious client. You should enable this on
 *                 a client to enable server-initiated renegotiation.
 *
 * \param ssl      SSL context
 * \param renegotiation     Enable or disable (SSL_RENEGOTIATION_ENABLED or
 *                                             SSL_RENEGOTIATION_DISABLED)
 */
void ssl_set_renegotiation( ssl_context *ssl, int renegotiation );

/**
 * \brief          Prevent or allow legacy renegotiation.
 *                 (Default: SSL_LEGACY_NO_RENEGOTIATION)
 *
 *                 SSL_LEGACY_NO_RENEGOTIATION allows connections to
 *                 be established even if the peer does not support
 *                 secure renegotiation, but does not allow renegotiation
 *                 to take place if not secure.
 *                 (Interoperable and secure option)
 *
 *                 SSL_LEGACY_ALLOW_RENEGOTIATION allows renegotiations
 *                 with non-upgraded peers. Allowing legacy renegotiation
 *                 makes the connection vulnerable to specific man in the
 *                 middle attacks. (See RFC 5746)
 *                 (Most interoperable and least secure option)
 *
 *                 SSL_LEGACY_BREAK_HANDSHAKE breaks off connections
 *                 if peer does not support secure renegotiation. Results
 *                 in interoperability issues with non-upgraded peers
 *                 that do not support renegotiation altogether.
 *                 (Most secure option, interoperability issues)
 *
 * \param ssl      SSL context
 * \param allow_legacy  Prevent or allow (SSL_NO_LEGACY_RENEGOTIATION,
 *                                        SSL_ALLOW_LEGACY_RENEGOTIATION or
 *                                        SSL_LEGACY_BREAK_HANDSHAKE)
 */
void ssl_legacy_renegotiation( ssl_context *ssl, int allow_legacy );

/**
 * \brief          Enforce server-requested renegotiation.
 *                 (Default: enforced, max_records = 16)
 *                 (No effect on client.)
 *
 *                 When a server requests a renegotiation, the client can
 *                 comply or ignore the request. This function allows the
 *                 server to decide if it should enforce its renegotiation
 *                 requests by closing the connection if the client doesn't
 *                 initiate a renegotiation.
 *
 *                 However, records could already be in transit from the
 *                 client to the server when the request is emitted. In order
 *                 to increase reliability, the server can accept a number of
 *                 records containing application data before the ClientHello
 *                 that was requested.
 *
 *                 The optimal value is highly dependent on the specific usage
 *                 scenario.
 *
 * \param ssl      SSL context
 * \param max_records Use SSL_RENEGOTIATION_NOT_ENFORCED if you don't want to
 *                 enforce renegotiation, or a non-negative value to enforce
 *                 it but allow for a grace period of max_records records.
 */
void ssl_set_renegotiation_enforced( ssl_context *ssl, int max_records );

/**
 * \brief          Return the number of data bytes available to read
 *
 * \param ssl      SSL context
 *
 * \return         how many bytes are available in the read buffer
 */
size_t ssl_get_bytes_avail( const ssl_context *ssl );

/**
 * \brief          Return the result of the certificate verification
 *
 * \param ssl      SSL context
 *
 * \return         0 if successful, or a combination of:
 *                      BADCERT_EXPIRED
 *                      BADCERT_REVOKED
 *                      BADCERT_CN_MISMATCH
 *                      BADCERT_NOT_TRUSTED
 */
int ssl_get_verify_result( const ssl_context *ssl );

/**
 * \brief          Return the name of the current ciphersuite
 *
 * \param ssl      SSL context
 *
 * \return         a string containing the ciphersuite name
 */
const char *ssl_get_ciphersuite( const ssl_context *ssl );

/**
 * \brief          Return the current SSL version (SSLv3/TLSv1/etc)
 *
 * \param ssl      SSL context
 *
 * \return         a string containing the SSL version
 */
const char *ssl_get_version( const ssl_context *ssl );

#if defined(POLARSSL_X509_CRT_PARSE_C)
/**
 * \brief          Return the peer certificate from the current connection
 *
 *                 Note: Can be NULL in case no certificate was sent during
 *                 the handshake. Different calls for the same connection can
 *                 return the same or different pointers for the same
 *                 certificate and even a different certificate altogether.
 *                 The peer cert CAN change in a single connection if
 *                 renegotiation is performed.
 *
 * \param ssl      SSL context
 *
 * \return         the current peer certificate
 */
const x509_crt *ssl_get_peer_cert( const ssl_context *ssl );
#endif /* POLARSSL_X509_CRT_PARSE_C */

/**
 * \brief          Save session in order to resume it later (client-side only)
 *                 Session data is copied to presented session structure.
 *
 * \warning        Currently, peer certificate is lost in the operation.
 *
 * \param ssl      SSL context
 * \param session  session context
 *
 * \return         0 if successful,
 *                 POLARSSL_ERR_SSL_MALLOC_FAILED if memory allocation failed,
 *                 POLARSSL_ERR_SSL_BAD_INPUT_DATA if used server-side or
 *                 arguments are otherwise invalid
 *
 * \sa             ssl_set_session()
 */
int ssl_get_session( const ssl_context *ssl, ssl_session *session );

/**
 * \brief          Perform the SSL handshake
 *
 * \param ssl      SSL context
 *
 * \return         0 if successful, POLARSSL_ERR_NET_WANT_READ,
 *                 POLARSSL_ERR_NET_WANT_WRITE, or a specific SSL error code.
 */
int ssl_handshake( ssl_context *ssl );

/**
 * \brief          Perform a single step of the SSL handshake
 *
 *                 Note: the state of the context (ssl->state) will be at
 *                 the following state after execution of this function.
 *                 Do not call this function if state is SSL_HANDSHAKE_OVER.
 *
 * \param ssl      SSL context
 *
 * \return         0 if successful, POLARSSL_ERR_NET_WANT_READ,
 *                 POLARSSL_ERR_NET_WANT_WRITE, or a specific SSL error code.
 */
int ssl_handshake_step( ssl_context *ssl );

/**
 * \brief          Initiate an SSL renegotiation on the running connection.
 *                 Client: perform the renegotiation right now.
 *                 Server: request renegotiation, which will be performed
 *                 during the next call to ssl_read() if honored by client.
 *
 * \param ssl      SSL context
 *
 * \return         0 if successful, or any ssl_handshake() return value.
 */
int ssl_renegotiate( ssl_context *ssl );

/**
 * \brief          Read at most 'len' application data bytes
 *
 * \param ssl      SSL context
 * \param buf      buffer that will hold the data
 * \param len      how many bytes must be read
 *
 * \return         This function returns the number of bytes read, 0 for EOF,
 *                 or a negative error code.
 */
int ssl_read( ssl_context *ssl, unsigned char *buf, size_t len );

/**
 * \brief          Write exactly 'len' application data bytes
 *
 * \param ssl      SSL context
 * \param buf      buffer holding the data
 * \param len      how many bytes must be written
 *
 * \return         This function returns the number of bytes written,
 *                 or a negative error code.
 *
 * \note           When this function returns POLARSSL_ERR_NET_WANT_WRITE,
 *                 it must be called later with the *same* arguments,
 *                 until it returns a positive value.
 */
int ssl_write( ssl_context *ssl, const unsigned char *buf, size_t len );

/**
 * \brief           Send an alert message
 *
 * \param ssl       SSL context
 * \param level     The alert level of the message
 *                  (SSL_ALERT_LEVEL_WARNING or SSL_ALERT_LEVEL_FATAL)
 * \param message   The alert message (SSL_ALERT_MSG_*)
 *
 * \return          0 if successful, or a specific SSL error code.
 */
int ssl_send_alert_message( ssl_context *ssl,
                            unsigned char level,
                            unsigned char message );
/**
 * \brief          Notify the peer that the connection is being closed
 *
 * \param ssl      SSL context
 */
int ssl_close_notify( ssl_context *ssl );

/**
 * \brief          Free referenced items in an SSL context and clear memory
 *
 * \param ssl      SSL context
 */
void ssl_free( ssl_context *ssl );

/**
 * \brief          Initialize SSL session structure
 *
 * \param session  SSL session
 */
void ssl_session_init( ssl_session *session );

/**
 * \brief          Free referenced items in an SSL session including the
 *                 peer certificate and clear memory
 *
 * \param session  SSL session
 */
void ssl_session_free( ssl_session *session );

/**
 * \brief           Free referenced items in an SSL transform context and clear
 *                  memory
 *
 * \param transform SSL transform context
 */
void ssl_transform_free( ssl_transform *transform );

/**
 * \brief           Free referenced items in an SSL handshake context and clear
 *                  memory
 *
 * \param handshake SSL handshake context
 */
void ssl_handshake_free( ssl_handshake_params *handshake );

/*
 * Internal functions (do not call directly)
 */
int ssl_handshake_client_step( ssl_context *ssl );
int ssl_handshake_server_step( ssl_context *ssl );
void ssl_handshake_wrapup( ssl_context *ssl );

int ssl_send_fatal_handshake_failure( ssl_context *ssl );

int ssl_derive_keys( ssl_context *ssl );

int ssl_read_record( ssl_context *ssl );
/**
 * \return         0 if successful, POLARSSL_ERR_SSL_CONN_EOF on EOF or
 *                 another negative error code.
 */
int ssl_fetch_input( ssl_context *ssl, size_t nb_want );

int ssl_write_record( ssl_context *ssl );
int ssl_flush_output( ssl_context *ssl );

int ssl_parse_certificate( ssl_context *ssl );
int ssl_write_certificate( ssl_context *ssl );

int ssl_parse_change_cipher_spec( ssl_context *ssl );
int ssl_write_change_cipher_spec( ssl_context *ssl );

int ssl_parse_finished( ssl_context *ssl );
int ssl_write_finished( ssl_context *ssl );

void ssl_optimize_checksum( ssl_context *ssl,
                            const ssl_ciphersuite_t *ciphersuite_info );

#if defined(POLARSSL_KEY_EXCHANGE__SOME__PSK_ENABLED)
int ssl_psk_derive_premaster( ssl_context *ssl, key_exchange_type_t key_ex );
#endif

#if defined(POLARSSL_PK_C)
unsigned char ssl_sig_from_pk( pk_context *pk );
pk_type_t ssl_pk_alg_from_sig( unsigned char sig );
#endif

md_type_t ssl_md_alg_from_hash( unsigned char hash );

#if defined(POLARSSL_SSL_SET_CURVES)
int ssl_curve_is_acceptable( const ssl_context *ssl, ecp_group_id grp_id );
#endif

#if defined(POLARSSL_X509_CRT_PARSE_C)
static inline pk_context *ssl_own_key( ssl_context *ssl )
{
    return( ssl->handshake->key_cert == NULL ? NULL
            : ssl->handshake->key_cert->key );
}

static inline x509_crt *ssl_own_cert( ssl_context *ssl )
{
    return( ssl->handshake->key_cert == NULL ? NULL
            : ssl->handshake->key_cert->cert );
}

/*
 * Check usage of a certificate wrt extensions:
 * keyUsage, extendedKeyUsage (later), and nSCertType (later).
 *
 * Warning: cert_endpoint is the endpoint of the cert (ie, of our peer when we
 * check a cert we received from them)!
 *
 * Return 0 if everything is OK, -1 if not.
 */
int ssl_check_cert_usage( const x509_crt *cert,
                          const ssl_ciphersuite_t *ciphersuite,
                          int cert_endpoint );
#endif /* POLARSSL_X509_CRT_PARSE_C */

/* constant-time buffer comparison */
static inline int safer_memcmp( const void *a, const void *b, size_t n )
{
    size_t i;
    const unsigned char *A = (const unsigned char *) a;
    const unsigned char *B = (const unsigned char *) b;
    unsigned char diff = 0;

    for( i = 0; i < n; i++ )
        diff |= A[i] ^ B[i];

    return( diff );
}

#ifdef __cplusplus
}
#endif

#endif /* ssl.h */
