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

#ifndef SPARK_WIRING_ARDUINO_H
#define	SPARK_WIRING_ARDUINO_H

#define strcpy_P strcpy
#define strlcpy_P strncpy
#define sprintf_P sprintf
#define strlen_P strlen
#define strcmp_P strcmp
#define memcpy_P memcpy
#define vsnprintf_P vsnprintf
#define PROGMEM
#define PSTR(x) (x)
#define pgm_read_byte(x)      (*(x))
#define pgm_read_word(x)      ((uint16_t)(*(x)))
#define pgm_read_byte_near(x) (*(x))
#define pgm_read_word_near(x) ((uint16_t)(*(x)))



#endif	/* SPARK_WIRING_ARDUINO_H */

