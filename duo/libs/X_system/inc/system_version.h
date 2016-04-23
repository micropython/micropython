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
#ifndef VERSION_H
#define	VERSION_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * This file is referenced from https://github.com/spark/firmware/wiki/Firmware-Release-Checklist
 */

#define SYSTEM_VERSION_v011  0x00010100
#define SYSTEM_VERSION_v020  0x00020000
#define SYSTEM_VERSION_v021  0x00020100
#define SYSTEM_VERSION_v022  0x00020200
#define SYSTEM_VERSION_v023RC2  0x00020302
#define SYSTEM_VERSION_v023  0x00020300

#define SYSTEM_VERSION  SYSTEM_VERSION_v023

#define SYSTEM_VERSION_011
#define SYSTEM_VERSION_020
#define SYSTEM_VERSION_021
#define SYSTEM_VERSION_022
#define SYSTEM_VERSION_023RC2
#define SYSTEM_VERSION_023

typedef struct __attribute__((packed)) SystemVersionInfo
{
    uint16_t size = sizeof(SystemVersionInfo);
    uint16_t reserved;      // use this if you need to.
    uint32_t versionNumber;
    char     versionString[20];



} SystemVersionInfo;

int system_version_info(SystemVersionInfo* target, void* reserved);

#ifdef	__cplusplus
}
#endif

#endif	/* VERSION_H */

