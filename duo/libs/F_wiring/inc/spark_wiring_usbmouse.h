/**
 ******************************************************************************
 * @file    spark_wiring_usbmouse.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    31-March-2014
 * @brief   Header for spark_wiring_usbmouse.c module
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

#ifndef __SPARK_WIRING_USBMOUSE_H
#define __SPARK_WIRING_USBMOUSE_H

#ifdef SPARK_USB_MOUSE
#include "spark_wiring.h"

#define MOUSE_LEFT		0x01
#define MOUSE_RIGHT		0x02
#define MOUSE_MIDDLE	0x04
#define MOUSE_ALL		(MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

typedef struct
{
	uint8_t buttons;
	int8_t x;
	int8_t y;
	int8_t wheel;
} MouseReport;

class USBMouse
{
private:
	MouseReport mouseReport;
	void buttons(uint8_t button);

public:
	USBMouse(void);

	void begin(void);
	void end(void);
	void move(int8_t x, int8_t y, int8_t wheel);
	void click(uint8_t button = MOUSE_LEFT);
	void press(uint8_t button = MOUSE_LEFT);		// press LEFT by default
	void release(uint8_t button = MOUSE_LEFT);		// release LEFT by default
	bool isPressed(uint8_t button = MOUSE_LEFT);	// check LEFT by default
};

extern USBMouse Mouse;
#endif

#endif
