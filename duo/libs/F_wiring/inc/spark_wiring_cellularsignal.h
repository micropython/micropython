/**
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

#ifndef __SPARK_WIRING_CELLULARSIGNAL_H
#define __SPARK_WIRING_CELLULARSIGNAL_H

#include <string.h>
#include "spark_wiring_printable.h"
#include "spark_wiring_string.h"
#include "cellular_hal.h"

class CellularSignal : public Printable {

public:
    int rssi = 0;
    int qual = 0;

    CellularSignal() { /* n/a */ }

    virtual size_t printTo(Print& p) const;
};

#endif
