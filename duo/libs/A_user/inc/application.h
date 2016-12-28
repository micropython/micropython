/**
  ******************************************************************************
  * @file    application.h
  * @authors  Satish Nair, Zachary Crockett, Zach Supalla and Mohit Bhoite
  * @version V1.0.0
  * @date    30-April-2013
  * @brief   User Application File Header
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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "system_version.h"

#ifdef SPARK_PLATFORM
#include "platform_headers.h"
#endif


#include "spark_wiring.h"
#include "spark_wiring_cloud.h"
#include "spark_wiring_interrupts.h"
#include "spark_wiring_string.h"
#include "spark_wiring_power.h"
#include "spark_wiring_fuel.h"  
#include "spark_wiring_print.h"
#include "spark_wiring_usartserial.h"
#include "spark_wiring_can.h"
#include "spark_wiring_usbserial.h"
#include "spark_wiring_usbmouse.h"
#include "spark_wiring_usbkeyboard.h"
#include "spark_wiring_i2c.h"
#include "spark_wiring_servo.h"
#include "spark_wiring_wifi.h"
#include "spark_wiring_network.h"
#include "spark_wiring_client.h"
#include "spark_wiring_startup.h"
#include "spark_wiring_timer.h"
#include "spark_wiring_tcpclient.h"
#include "spark_wiring_tcpserver.h"
#include "spark_wiring_udp.h"
#include "spark_wiring_time.h"
#include "spark_wiring_tone.h"
#include "spark_wiring_eeprom.h"
#include "spark_wiring_version.h"
#include "spark_wiring_watchdog.h"
#include "spark_wiring_thread.h"
#include "fast_pin.h"
#include "string_convert.h"
#include "debug_output_handler.h"

#if PLATFORM_ID==88
#include "spark_wiring_hciserial.h"
#include "spark_wiring_btstack.h"
#endif
#include "spark_wiring_spi.h"

// this was being implicitly pulled in by some of the other headers
// adding here for backwards compatibility.
#include "system_task.h"
#include "system_user.h"

#include "stdio.h"

using namespace spark;

#endif /* APPLICATION_H_ */
