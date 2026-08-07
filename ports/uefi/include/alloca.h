// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

/* <alloca.h> shim — alloca is the compiler builtin (also #defined in mpconfigport.h). */
#ifndef MPY_UEFI_ALLOCA_H
#define MPY_UEFI_ALLOCA_H
#ifndef alloca
#define alloca(n) __builtin_alloca(n)
#endif
#endif
