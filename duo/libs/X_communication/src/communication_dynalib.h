/**
  ******************************************************************************
  * @file    communication_dynalib.h
  * @authors  Matthew McGowan
  ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
  */

#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

#include "dynalib.h"
#include "protocol_selector.h"
#include "hal_platform.h"

DYNALIB_BEGIN(communication)
DYNALIB_FN(communication, spark_protocol_instance)
DYNALIB_FN(communication, spark_protocol_set_product_id)
DYNALIB_FN(communication, spark_protocol_set_product_firmware_version)
DYNALIB_FN(communication, spark_protocol_get_product_details)
DYNALIB_FN(communication, spark_protocol_communications_handlers)
DYNALIB_FN(communication, spark_protocol_init)
DYNALIB_FN(communication, spark_protocol_handshake)
DYNALIB_FN(communication, spark_protocol_event_loop)
DYNALIB_FN(communication, spark_protocol_is_initialized)
DYNALIB_FN(communication, spark_protocol_presence_announcement)
DYNALIB_FN(communication, spark_protocol_send_event)
DYNALIB_FN(communication, spark_protocol_send_subscription_device)
DYNALIB_FN(communication, spark_protocol_send_subscription_scope)
DYNALIB_FN(communication, spark_protocol_add_event_handler)
DYNALIB_FN(communication, spark_protocol_send_time_request)
DYNALIB_FN(communication, spark_protocol_send_subscriptions)
#if !defined(PARTICLE_PROTOCOL) || HAL_PLATFORM_CLOUD_TCP
DYNALIB_FN(communication, decrypt_rsa)
DYNALIB_FN(communication, gen_rsa_key)
DYNALIB_FN(communication, extract_public_rsa_key)
#endif
DYNALIB_FN(communication, spark_protocol_remove_event_handlers)
#if defined(PARTICLE_PROTOCOL) && HAL_PLATFORM_CLOUD_UDP
DYNALIB_FN(communication, gen_ec_key)
DYNALIB_FN(communication, extract_public_ec_key)
#endif
DYNALIB_END(communication)


#ifdef	__cplusplus
}
#endif
