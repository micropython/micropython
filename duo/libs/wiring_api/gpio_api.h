#ifndef _GPIO_API_H
#define _GPIO_API_H

#include "pinmap_hal.h"

void     pinMode(uint16_t pin, PinMode mode);
PinMode  getPinMode(uint16_t pin);
bool     pinAvailable(uint16_t pin);

void     setADCSampleTime(uint8_t ADC_SampleTime);
int32_t  analogRead(uint16_t pin);
void     wiring_analogWrite(uint16_t pin, uint16_t value);
void     wiring_analogWriteWithFreq(uint16_t pin, uint16_t value, uint16_t pwm_frequency);

void     digitalWrite(uint16_t pin, uint8_t value);
int32_t  digitalRead(uint16_t pin);

long     map(long value, long fromStart, long fromEnd, long toStart, long toEnd);

void     shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t  shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

uint32_t pulseIn(pin_t pin, uint16_t value);


#endif
