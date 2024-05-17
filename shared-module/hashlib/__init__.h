// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2014 Paul Sokolovsky
//
// SPDX-License-Identifier: MIT

#pragma once

#include "mbedtls/version.h"

#if MBEDTLS_VERSION_NUMBER < 0x02070000 || MBEDTLS_VERSION_NUMBER >= 0x03000000
#define mbedtls_sha1_starts_ret mbedtls_sha1_starts
#define mbedtls_sha1_update_ret mbedtls_sha1_update
#define mbedtls_sha1_finish_ret mbedtls_sha1_finish
#endif
