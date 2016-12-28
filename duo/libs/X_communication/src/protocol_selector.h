/**
 ******************************************************************************
 Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

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

#include "hal_platform.h"

#if HAL_PLATFORM_CLOUD_UDP
#define PARTICLE_PROTOCOL
#endif

#ifdef PARTICLE_PROTOCOL
#ifdef __cplusplus
namespace particle { namespace protocol { class Protocol; }}
typedef particle::protocol::Protocol ProtocolFacade;
#else
typedef void* ProtocolFacade;
#endif
#else
#ifdef __cplusplus
class SparkProtocol;
typedef SparkProtocol ProtocolFacade;
#else
struct SparkProtocol;
typedef struct SparkProtocol ProtocolFacade;

#endif
#endif
