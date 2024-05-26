// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#if CIRCUITPY_SSL_MBEDTLS
#include "shared-module/ssl/SSLContext.h"
#else
#include "common-hal/ssl/SSLContext.h"
#endif

void common_hal_ssl_create_default_context(ssl_sslcontext_obj_t *self);
