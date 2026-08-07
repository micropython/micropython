// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// Empty winsock2.h shim for the freestanding UEFI port. mbedtls x509_crt.c includes
// it on the _WIN32/_MSC_VER path for the platform inet_pton(); leaving AF_INET6
// undefined makes mbedtls compile and use its own software inet_pton instead.
#ifndef _UEFI_LIBC_WINSOCK2_H
#define _UEFI_LIBC_WINSOCK2_H
#endif
