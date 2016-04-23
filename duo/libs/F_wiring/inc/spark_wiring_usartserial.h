/**
 ******************************************************************************
 * @file    spark_wiring_usartserial.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    13-March-2013
 * @brief   Header for spark_wiring_usartserial.c module
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

#ifndef __SPARK_WIRING_USARTSERIAL_H
#define __SPARK_WIRING_USARTSERIAL_H

#include "spark_wiring_stream.h"
#include "usart_hal.h"
#include "spark_wiring_platform.h"

class USARTSerial : public Stream
{
private:
  HAL_USART_Serial _serial;
  bool _blocking;
public:
  USARTSerial(HAL_USART_Serial serial, Ring_Buffer *rx_buffer, Ring_Buffer *tx_buffer);
  virtual ~USARTSerial() {};
  void begin(unsigned long);
  void begin(unsigned long, uint8_t);
  void halfduplex(bool);
  void end();

  virtual void blockOnOverrun(bool);

  virtual int availableForWrite(void);
  virtual int available(void);
  virtual int peek(void);
  virtual int read(void);
  virtual void flush(void);
  virtual size_t write(uint8_t);

  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }

  using Print::write; // pull in write(str) and write(buf, size) from Print

  operator bool();

  bool isEnabled(void);
};

#if Wiring_Serial2
void serialEventRun2(void) __attribute__((weak));
void serialEvent2(void) __attribute__((weak));
#endif

#if Wiring_Serial3
void serialEventRun3(void) __attribute__((weak));
void serialEvent3(void) __attribute__((weak));
#endif

#if Wiring_Serial4
void serialEventRun4(void) __attribute__((weak));
void serialEvent4(void) __attribute__((weak));
#endif

#if Wiring_Serial5
void serialEventRun5(void) __attribute__((weak));
void serialEvent5(void) __attribute__((weak));
#endif

inline void __handleSerialEvent(USARTSerial& serial, void (*handler)(void)) __attribute__((always_inline));

inline void __handleSerialEvent(USARTSerial& serial, void (*handler)(void))
{
    if (handler && serial.isEnabled() && serial.available()>0)
        handler();
}


#ifndef SPARK_WIRING_NO_USART_SERIAL
#define Serial1 __fetch_global_Serial1()
extern USARTSerial& __fetch_global_Serial1();

#if Wiring_Serial2
#define Serial2 __fetch_global_Serial2()
extern USARTSerial& __fetch_global_Serial2();
#endif

#if Wiring_Serial3
#define Serial3 __fetch_global_Serial3()
extern USARTSerial& __fetch_global_Serial3();
#endif

#if Wiring_Serial4
#define Serial4 __fetch_global_Serial4()
extern USARTSerial& __fetch_global_Serial4();
#endif

#if Wiring_Serial5
#define Serial5 __fetch_global_Serial5()
extern USARTSerial& __fetch_global_Serial5();
#endif

#endif

#endif
