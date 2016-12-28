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


#ifndef DEBUG_OUTPUT_HANDLER_H
#define	DEBUG_OUTPUT_HANDLER_H

#include "spark_wiring_usartserial.h"
#include "spark_wiring_usbserial.h"
#include "service_debug.h"
#include "delay_hal.h"

struct SerialDebugOutput
{
    SerialDebugOutput(int baud=9600, LoggerOutputLevel level=ALL_LEVEL)
    {
        Serial.begin(baud);
        set_logger_output(log_output, level);
    }

    static void log_output(const char* msg)
    {
        Serial.print(msg);
    //    Serial.flush();
     //  HAL_Delay_Milliseconds(10);
    }

};

struct Serial1DebugOutput
{
    Serial1DebugOutput(int baud=9600, LoggerOutputLevel level=ALL_LEVEL)
    {
        Serial1.begin(baud);
        set_logger_output(log_output, level);
    }

    static void log_output(const char* msg)
    {
        Serial1.print(msg);
    }

};


#endif	/* DEBUG_OUTPUT_HANDLER_H */

