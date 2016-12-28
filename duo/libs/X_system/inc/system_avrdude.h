/**
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

#ifndef SYSTEM_AVRDUDE_H_
#define SYSTEM_AVRDUDE_H_

#include "file_transfer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Stream Stream;
#include <stdint.h>

#if (PLATFORM_ID==88)
bool Avrdude_Serial_Flash_Update(Stream *serialObj, FileTransfer::Descriptor& desc, void*);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_AVRDUDE_H */

