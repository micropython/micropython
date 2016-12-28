/**
 ******************************************************************************
 * @file    spark_wiring.h
 * @author  Satish Nair, Zachary Crockett, Zach Supalla and Mohit Bhoite
 * @version V1.0.0
 * @date    13-March-2013
 * @brief   Header for spark_wiring.c module
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

#ifndef SPARK_WIRING_H
#define SPARK_WIRING_H
#include "pinmap_hal.h"
#include "gpio_hal.h"
#include "adc_hal.h"
#include "dac_hal.h"
#include "pwm_hal.h"
#include "rng_hal.h"
#if PLATFORM_ID == 88
#include "device_name.h"
#endif
#include "config.h"
#include "spark_macros.h"
#include "debug.h"
#include "spark_wiring_arduino.h"
#include "spark_wiring_constants.h"
#include "spark_wiring_stream.h"
#include "spark_wiring_printable.h"
#include "spark_wiring_ipaddress.h"
#include "spark_wiring_cellularsignal.h"
#include "spark_wiring_wifi.h"
#include "spark_wiring_cellular.h"
#include "spark_wiring_character.h"
#include "spark_wiring_random.h"
#include "spark_wiring_system.h"
#include "spark_wiring_cloud.h"
#include "spark_wiring_rgb.h"
#include "spark_wiring_ticks.h"

/* To prevent build error, we are undefining and redefining DAC here */
#undef DAC
#define DAC DAC1

#ifdef __cplusplus
extern "C" {
#endif

/*
* ADC
*/
void setADCSampleTime(uint8_t ADC_SampleTime);
int32_t analogRead(uint16_t pin);

/*
* GPIO
*/
void pinMode(uint16_t pin, PinMode mode);
PinMode getPinMode(uint16_t pin);
bool pinAvailable(uint16_t pin);
void digitalWrite(uint16_t pin, uint8_t value);
int32_t digitalRead(uint16_t pin);


long map(long value, long fromStart, long fromEnd, long toStart, long toEnd);

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void serialReadLine(Stream *serialObj, char *dst, int max_len, system_tick_t timeout);

uint32_t pulseIn(pin_t pin, uint16_t value);

#ifdef __cplusplus
}
#endif

void analogWrite(uint16_t pin, uint16_t value);
void analogWrite(uint16_t pin, uint16_t value, uint16_t pwm_frequency);

#endif /* SPARK_WIRING_H_ */
