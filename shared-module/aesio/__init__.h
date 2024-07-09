// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"

#include "shared-module/aesio/aes.h"

// These values were chosen to correspond with the values
// present in pycrypto.
enum AES_MODE {
    AES_MODE_ECB = 1,
    AES_MODE_CBC = 2,
    AES_MODE_CTR = 6,
};

typedef struct {
    mp_obj_base_t base;

    // The tinyaes context
    struct AES_ctx ctx;

    // Which AES mode this instance of the object is configured to use
    enum AES_MODE mode;

    // Counter for running in CTR mode
    uint32_t counter;
} aesio_aes_obj_t;
