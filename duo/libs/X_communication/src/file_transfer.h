/**
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#pragma once

#include <stdint.h>
#include "static_assert.h"

namespace FileTransfer {

    namespace Store {
        enum __attribute__ ((__packed__)) Enum {
            FIRMWARE,
            SYSTEM,          // storage provided by the platform, e.g. external flash
            APPLICATION=128, // storage provided by the application.
        };
    };

    struct __attribute__((packed)) Chunk
    {
        uint16_t size;
        uint16_t reserved2;         // padding - reuse if possible.

        /**
         * For memory devices, represents the offset in the memory where the file data should be stored.
         */
        uint32_t chunk_address;

        /**
         * The size of the chunks used to incrementally write the file.
         */
        uint16_t chunk_size;

        /**
         * The target device ID.
         * 0 means OTA storage.
         * 1 means system-provided storage
         * 2 means application-provided storage
         */
        Store::Enum store;
        uint8_t reserved;       // padding
    };

    STATIC_ASSERT(Chunk_size, sizeof(Chunk)==12);

    struct Descriptor : public Chunk
    {
        Descriptor() { size = sizeof(*this); }

        /**
         * The length of the file data.
         */
        uint32_t file_length;

        uint32_t file_address;

        unsigned chunk_count(unsigned chunk_size) {
            return chunk_size ? (file_length+chunk_size-1)/chunk_size : 0;
        }
    };

    STATIC_ASSERT(Descriptor_size, sizeof(Descriptor)==20);

};

