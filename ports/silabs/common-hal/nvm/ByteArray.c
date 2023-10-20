/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "common-hal/nvm/ByteArray.h"
#include "supervisor/shared/stack.h"

#include "py/runtime.h"

#include "nvm3_default.h"
#include "nvm3_default_config.h"

uint8_t nvm_array[NVM_BYTEARRAY_BUFFER_SIZE];
STATIC bool isInitialized = false;
#define NVM_KEY 98

// Get length of nvm bytearray
uint32_t common_hal_nvm_bytearray_get_length(nvm_bytearray_obj_t *self) {
    return self->len;
}

uint32_t nvm3_read(nvm_bytearray_obj_t *self) {
    uint32_t type;
    Ecode_t err;
    size_t len;

    if (isInitialized == false) {
        nvm3_initDefault();
        isInitialized = true;
    }
    err = nvm3_getObjectInfo(nvm3_defaultHandle, NVM_KEY, &type, &len);
    if (err != ECODE_NVM3_OK || type != NVM3_OBJECTTYPE_DATA
        || len != NVM_BYTEARRAY_BUFFER_SIZE) {

        nvm3_deleteObject(nvm3_defaultHandle, NVM_KEY);
        nvm3_writeData(nvm3_defaultHandle, NVM_KEY, nvm_array,
            NVM_BYTEARRAY_BUFFER_SIZE);
    }

    err = nvm3_readData(nvm3_defaultHandle, NVM_KEY, nvm_array,
        NVM_BYTEARRAY_BUFFER_SIZE);

    return err;
}

// Write n bytes to nvm bytearray
bool common_hal_nvm_bytearray_set_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t *values,
    uint32_t len) {
    Ecode_t err;
    err = nvm3_read(self);

    if (err != ECODE_NVM3_OK) {
        mp_raise_RuntimeError(translate("NVM3 read false"));
    }
    // Set bytes in buffer
    memmove(nvm_array + start_index, values, len);

    err = nvm3_writeData(nvm3_defaultHandle, NVM_KEY, nvm_array,
        NVM_BYTEARRAY_BUFFER_SIZE);

    if (err != ECODE_NVM3_OK) {
        mp_raise_RuntimeError(translate("NVM3 write false"));
    }

    return true;
}

// Read n bytes from nvm bytearray
void common_hal_nvm_bytearray_get_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index,
    uint32_t len, uint8_t *values) {
    nvm3_read(self);
    memcpy(values, nvm_array + start_index, len);
}
