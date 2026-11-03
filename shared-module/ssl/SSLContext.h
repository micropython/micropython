// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "mbedtls/ssl.h"
#include "psa/crypto.h"

typedef struct {
    mp_obj_base_t base;
    bool check_name, use_global_ca_store;
    const unsigned char *cacert_buf;
    size_t cacert_bytes;
    int (*crt_bundle_attach)(mbedtls_ssl_config *conf);
    mp_buffer_info_t cert_buf, key_buf;
    // When nonzero, the client-certificate private key is an opaque PSA key
    // (e.g. a hardwarekey.HardwareKey backed by the Digital Signature
    // peripheral) and key_buf is unused.
    psa_key_id_t hw_key_id;
} ssl_sslcontext_obj_t;
