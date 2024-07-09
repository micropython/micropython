// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/ssl/__init__.h"
#include "shared-bindings/ssl/__init__.h"
#include "shared-bindings/ssl/SSLContext.h"
#include "lib/mbedtls_config/crt_bundle.h"

void common_hal_ssl_create_default_context(ssl_sslcontext_obj_t *self) {
    common_hal_ssl_sslcontext_construct(self);
}

void ssl_reset(void) {
    crt_bundle_detach(NULL);
}
