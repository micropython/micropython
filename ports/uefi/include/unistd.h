// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

/* Minimal <unistd.h> for the UEFI freestanding build (a few core files include
 * it only for ssize_t). */
#ifndef MPY_UEFI_UNISTD_H
#define MPY_UEFI_UNISTD_H
#include <stdint.h>
typedef intptr_t ssize_t;
#endif
