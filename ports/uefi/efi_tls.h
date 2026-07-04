/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* Typed EFI_TLS_PROTOCOL definitions (TLS=efi backend), same style as efi.h /
 * efi_net.h. Transcribed from EDK2 MdePkg Protocol/Tls.h, Protocol/TlsConfig.h and
 * IndustryStandard/Tls1.h.
 */
#ifndef MPY_UEFI_EFI_TLS_H
#define MPY_UEFI_EFI_TLS_H

#include "efi.h"
#include "efi_net.h"   // EFI_SERVICE_BINDING_PROTOCOL, EFI_IPv4_ADDRESS, net_sb_* helpers

typedef char CHAR8;

// The TLS service binding is a standard EFI_SERVICE_BINDING_PROTOCOL instance.
typedef EFI_SERVICE_BINDING_PROTOCOL EFI_TLS_SERVICE_BINDING_PROTOCOL;

/* ---- GUIDs ---- */
#define EFI_TLS_SERVICE_BINDING_PROTOCOL_GUID \
    { 0x952cb795, 0xff36, 0x48cf, { 0xa2, 0x49, 0x4d, 0xf4, 0x86, 0xd6, 0xab, 0x8d } }
#define EFI_TLS_PROTOCOL_GUID \
    { 0x00ca959f, 0x6cfa, 0x4db1, { 0x95, 0xbc, 0xe4, 0x6c, 0x47, 0x51, 0x43, 0x90 } }
#define EFI_TLS_CONFIGURATION_PROTOCOL_GUID \
    { 0x1682fe44, 0xbd7a, 0x4407, { 0xb7, 0xc7, 0xdc, 0xa3, 0x7c, 0xa3, 0x92, 0x2d } }

/* ---- EFI_TLS_SESSION_DATA_TYPE (order matters — VerifyMethod is index 5) ---- */
typedef enum {
    EfiTlsVersion,             // EFI_TLS_VERSION
    EfiTlsConnectionEnd,       // EFI_TLS_CONNECTION_END
    EfiTlsCipherList,          // list of EFI_TLS_CIPHER
    EfiTlsCompressionMethod,   // EFI_TLS_COMPRESSION
    EfiTlsExtensionData,       // list of EFI_TLS_EXTENSION
    EfiTlsVerifyMethod,        // EFI_TLS_VERIFY
    EfiTlsSessionID,           // EFI_TLS_SESSION_ID
    EfiTlsSessionState,        // EFI_TLS_SESSION_STATE
    EfiTlsClientRandom,        // EFI_TLS_RANDOM
    EfiTlsServerRandom,        // EFI_TLS_RANDOM
    EfiTlsKeyMaterial,         // EFI_TLS_MASTER_SECRET
    EfiTlsVerifyHost,          // EFI_TLS_VERIFY_HOST
    EfiTlsSessionDataTypeMaximum
} EFI_TLS_SESSION_DATA_TYPE;

typedef struct { UINT8 Major;
                 UINT8 Minor;
} EFI_TLS_VERSION;

typedef enum { EfiTlsClient, EfiTlsServer } EFI_TLS_CONNECTION_END;

typedef UINT32 EFI_TLS_VERIFY;
#define EFI_TLS_VERIFY_NONE                 0x0
#define EFI_TLS_VERIFY_PEER                 0x1
#define EFI_TLS_VERIFY_FAIL_IF_NO_PEER_CERT 0x2
#define EFI_TLS_VERIFY_CLIENT_ONCE          0x4

typedef UINT32 EFI_TLS_VERIFY_HOST_FLAG;
#define EFI_TLS_VERIFY_FLAG_NONE 0x00

/* EFI_TLS_VERIFY_HOST: {UINT32 Flags; CHAR8 *HostName;} packed. */
#pragma pack(1)
typedef struct {
    EFI_TLS_VERIFY_HOST_FLAG Flags;
    CHAR8 *HostName;
} EFI_TLS_VERIFY_HOST;
#pragma pack()

typedef enum {
    EfiTlsSessionNotStarted,
    EfiTlsSessionHandShaking,
    EfiTlsSessionDataTransferring,
    EfiTlsSessionClosing,
    EfiTlsSessionError,
    EfiTlsSessionStateMaximum
} EFI_TLS_SESSION_STATE;

typedef struct {
    UINT32 FragmentLength;
    void *FragmentBuffer;
} EFI_TLS_FRAGMENT_DATA;

typedef enum { EfiTlsEncrypt, EfiTlsDecrypt } EFI_TLS_CRYPT_MODE;

typedef struct _EFI_TLS_PROTOCOL EFI_TLS_PROTOCOL;
struct _EFI_TLS_PROTOCOL {
    EFI_STATUS(EFIAPI * SetSessionData)(EFI_TLS_PROTOCOL * This, EFI_TLS_SESSION_DATA_TYPE DataType,
        void *Data, UINTN DataSize);
    EFI_STATUS(EFIAPI * GetSessionData)(EFI_TLS_PROTOCOL * This, EFI_TLS_SESSION_DATA_TYPE DataType,
        void *Data OPTIONAL, UINTN *DataSize);
    EFI_STATUS(EFIAPI * BuildResponsePacket)(EFI_TLS_PROTOCOL * This, UINT8 *RequestBuffer OPTIONAL,
        UINTN RequestSize OPTIONAL, UINT8 *Buffer OPTIONAL, UINTN *BufferSize);
    EFI_STATUS(EFIAPI * ProcessPacket)(EFI_TLS_PROTOCOL * This, EFI_TLS_FRAGMENT_DATA **FragmentTable,
        UINT32 *FragmentCount, EFI_TLS_CRYPT_MODE CryptMode);
};

/* ---- EFI_TLS_CONFIGURATION_PROTOCOL ---- */
typedef enum {
    EfiTlsConfigDataTypeHostPublicCert,
    EfiTlsConfigDataTypeHostPrivateKey,
    EfiTlsConfigDataTypeCACertificate,
    EfiTlsConfigDataTypeCertRevocationList,
    EfiTlsConfigDataTypeMaximum
} EFI_TLS_CONFIG_DATA_TYPE;

typedef struct _EFI_TLS_CONFIGURATION_PROTOCOL EFI_TLS_CONFIGURATION_PROTOCOL;
struct _EFI_TLS_CONFIGURATION_PROTOCOL {
    EFI_STATUS(EFIAPI * SetData)(EFI_TLS_CONFIGURATION_PROTOCOL * This, EFI_TLS_CONFIG_DATA_TYPE DataType,
        void *Data, UINTN DataSize);
    EFI_STATUS(EFIAPI * GetData)(EFI_TLS_CONFIGURATION_PROTOCOL * This, EFI_TLS_CONFIG_DATA_TYPE DataType,
        void *Data OPTIONAL, UINTN *DataSize);
};

/* ---- TLS record framing (IndustryStandard/Tls1.h) ---- */
#define TLS_RECORD_HEADER_LENGTH 5
#define TLS_CONTENT_TYPE_CHANGE_CIPHER_SPEC 20
#define TLS_CONTENT_TYPE_ALERT 21
#define TLS_CONTENT_TYPE_HANDSHAKE 22
#define TLS_CONTENT_TYPE_APPLICATION_DATA 23
#define TLS12_VERSION_MAJOR 0x03
#define TLS12_VERSION_MINOR 0x03

/* Extra EFI status codes used by the TLS path (not already in efi.h). */
#ifndef EFI_INVALID_PARAMETER
#define EFI_INVALID_PARAMETER EFIERR(2)
#endif
#ifndef EFI_UNSUPPORTED
#define EFI_UNSUPPORTED EFIERR(3)
#endif
#ifndef EFI_ACCESS_DENIED
#define EFI_ACCESS_DENIED EFIERR(15)
#endif
#ifndef EFI_ABORTED
#define EFI_ABORTED EFIERR(21)
#endif

#endif // MPY_UEFI_EFI_TLS_H
