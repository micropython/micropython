/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2013 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _core_pins_h_
#define _core_pins_h_

#include "mk20dx128.h"
#include "pins_arduino.h"


#define HIGH		1
#define LOW		0
#define INPUT		0
#define OUTPUT		1
#define INPUT_PULLUP	2
#define LSBFIRST	0
#define MSBFIRST	1
#define _BV(n)		(1<<(n))
#define CHANGE		4
#define FALLING		2
#define RISING		3

// Pin				Arduino
//  0	B16			RXD
//  1	B17			TXD
//  2	D0
//  3	A12	FTM1_CH0
//  4	A13	FTM1_CH1
//  5	D7	FTM0_CH7	OC0B/T1
//  6	D4	FTM0_CH4	OC0A
//  7	D2
//  8	D3			ICP1
//  9	C3	FTM0_CH2	OC1A
// 10	C4	FTM0_CH3	SS/OC1B
// 11	C6			MOSI/OC2A
// 12	C7			MISO
// 13	C5			SCK
// 14	D1
// 15	C0
// 16	B0	(FTM1_CH0)
// 17	B1	(FTM1_CH1)
// 18	B3			SDA
// 19	B2			SCL
// 20	D5	FTM0_CH5
// 21	D6	FTM0_CH6
// 22	C1	FTM0_CH0
// 23	C2	FTM0_CH1
// 24	A5	(FTM0_CH2)
// 25	B19
// 26	E1
// 27	C9
// 28	C8
// 29	C10
// 30	C11
// 31	E0
// 32	B18
// 33	A4	(FTM0_CH1)
// (34)	analog only
// (35)	analog only
// (36)	analog only
// (37)	analog only

// not available to user:
//	A0	FTM0_CH5	SWD Clock
//	A1	FTM0_CH6	USB ID
//	A2	FTM0_CH7	SWD Trace
//	A3	FTM0_CH0	SWD Data

#define CORE_NUM_TOTAL_PINS     34
#define CORE_NUM_DIGITAL        34
#define CORE_NUM_INTERRUPT      34
#if defined(__MK20DX128__)
#define CORE_NUM_ANALOG         14
#define CORE_NUM_PWM            10
#elif defined(__MK20DX256__)
#define CORE_NUM_ANALOG         21
#define CORE_NUM_PWM            12
#endif

#define CORE_PIN0_BIT		16
#define CORE_PIN1_BIT		17
#define CORE_PIN2_BIT		0
#define CORE_PIN3_BIT		12
#define CORE_PIN4_BIT		13
#define CORE_PIN5_BIT		7
#define CORE_PIN6_BIT		4
#define CORE_PIN7_BIT		2
#define CORE_PIN8_BIT		3
#define CORE_PIN9_BIT		3
#define CORE_PIN10_BIT		4
#define CORE_PIN11_BIT		6
#define CORE_PIN12_BIT		7
#define CORE_PIN13_BIT		5
#define CORE_PIN14_BIT		1
#define CORE_PIN15_BIT		0
#define CORE_PIN16_BIT		0
#define CORE_PIN17_BIT		1
#define CORE_PIN18_BIT		3
#define CORE_PIN19_BIT		2
#define CORE_PIN20_BIT		5
#define CORE_PIN21_BIT		6
#define CORE_PIN22_BIT		1
#define CORE_PIN23_BIT		2
#define CORE_PIN24_BIT		5
#define CORE_PIN25_BIT		19
#define CORE_PIN26_BIT		1
#define CORE_PIN27_BIT		9
#define CORE_PIN28_BIT		8
#define CORE_PIN29_BIT		10
#define CORE_PIN30_BIT		11
#define CORE_PIN31_BIT		0
#define CORE_PIN32_BIT		18
#define CORE_PIN33_BIT		4

#define CORE_PIN0_BITMASK	(1<<(CORE_PIN0_BIT))
#define CORE_PIN1_BITMASK	(1<<(CORE_PIN1_BIT))
#define CORE_PIN2_BITMASK	(1<<(CORE_PIN2_BIT))
#define CORE_PIN3_BITMASK	(1<<(CORE_PIN3_BIT))
#define CORE_PIN4_BITMASK	(1<<(CORE_PIN4_BIT))
#define CORE_PIN5_BITMASK	(1<<(CORE_PIN5_BIT))
#define CORE_PIN6_BITMASK	(1<<(CORE_PIN6_BIT))
#define CORE_PIN7_BITMASK	(1<<(CORE_PIN7_BIT))
#define CORE_PIN8_BITMASK	(1<<(CORE_PIN8_BIT))
#define CORE_PIN9_BITMASK	(1<<(CORE_PIN9_BIT))
#define CORE_PIN10_BITMASK	(1<<(CORE_PIN10_BIT))
#define CORE_PIN11_BITMASK	(1<<(CORE_PIN11_BIT))
#define CORE_PIN12_BITMASK	(1<<(CORE_PIN12_BIT))
#define CORE_PIN13_BITMASK	(1<<(CORE_PIN13_BIT))
#define CORE_PIN14_BITMASK	(1<<(CORE_PIN14_BIT))
#define CORE_PIN15_BITMASK	(1<<(CORE_PIN15_BIT))
#define CORE_PIN16_BITMASK	(1<<(CORE_PIN16_BIT))
#define CORE_PIN17_BITMASK	(1<<(CORE_PIN17_BIT))
#define CORE_PIN18_BITMASK	(1<<(CORE_PIN18_BIT))
#define CORE_PIN19_BITMASK	(1<<(CORE_PIN19_BIT))
#define CORE_PIN20_BITMASK	(1<<(CORE_PIN20_BIT))
#define CORE_PIN21_BITMASK	(1<<(CORE_PIN21_BIT))
#define CORE_PIN22_BITMASK	(1<<(CORE_PIN22_BIT))
#define CORE_PIN23_BITMASK	(1<<(CORE_PIN23_BIT))
#define CORE_PIN24_BITMASK	(1<<(CORE_PIN24_BIT))
#define CORE_PIN25_BITMASK	(1<<(CORE_PIN25_BIT))
#define CORE_PIN26_BITMASK	(1<<(CORE_PIN26_BIT))
#define CORE_PIN27_BITMASK	(1<<(CORE_PIN27_BIT))
#define CORE_PIN28_BITMASK	(1<<(CORE_PIN28_BIT))
#define CORE_PIN29_BITMASK	(1<<(CORE_PIN29_BIT))
#define CORE_PIN30_BITMASK	(1<<(CORE_PIN30_BIT))
#define CORE_PIN31_BITMASK	(1<<(CORE_PIN31_BIT))
#define CORE_PIN32_BITMASK	(1<<(CORE_PIN32_BIT))
#define CORE_PIN33_BITMASK	(1<<(CORE_PIN33_BIT))

#define CORE_PIN0_PORTREG	GPIOB_PDOR
#define CORE_PIN1_PORTREG	GPIOB_PDOR
#define CORE_PIN2_PORTREG	GPIOD_PDOR
#define CORE_PIN3_PORTREG	GPIOA_PDOR
#define CORE_PIN4_PORTREG	GPIOA_PDOR
#define CORE_PIN5_PORTREG	GPIOD_PDOR
#define CORE_PIN6_PORTREG	GPIOD_PDOR
#define CORE_PIN7_PORTREG	GPIOD_PDOR
#define CORE_PIN8_PORTREG	GPIOD_PDOR
#define CORE_PIN9_PORTREG	GPIOC_PDOR
#define CORE_PIN10_PORTREG	GPIOC_PDOR
#define CORE_PIN11_PORTREG	GPIOC_PDOR
#define CORE_PIN12_PORTREG	GPIOC_PDOR
#define CORE_PIN13_PORTREG	GPIOC_PDOR
#define CORE_PIN14_PORTREG	GPIOD_PDOR
#define CORE_PIN15_PORTREG	GPIOC_PDOR
#define CORE_PIN16_PORTREG	GPIOB_PDOR
#define CORE_PIN17_PORTREG	GPIOB_PDOR
#define CORE_PIN18_PORTREG	GPIOB_PDOR
#define CORE_PIN19_PORTREG	GPIOB_PDOR
#define CORE_PIN20_PORTREG	GPIOD_PDOR
#define CORE_PIN21_PORTREG	GPIOD_PDOR
#define CORE_PIN22_PORTREG	GPIOC_PDOR
#define CORE_PIN23_PORTREG	GPIOC_PDOR
#define CORE_PIN24_PORTREG	GPIOA_PDOR
#define CORE_PIN25_PORTREG	GPIOB_PDOR
#define CORE_PIN26_PORTREG	GPIOE_PDOR
#define CORE_PIN27_PORTREG	GPIOC_PDOR
#define CORE_PIN28_PORTREG	GPIOC_PDOR
#define CORE_PIN29_PORTREG	GPIOC_PDOR
#define CORE_PIN30_PORTREG	GPIOC_PDOR
#define CORE_PIN31_PORTREG	GPIOE_PDOR
#define CORE_PIN32_PORTREG	GPIOB_PDOR
#define CORE_PIN33_PORTREG	GPIOA_PDOR

#define CORE_PIN0_PORTSET	GPIOB_PSOR
#define CORE_PIN1_PORTSET	GPIOB_PSOR
#define CORE_PIN2_PORTSET	GPIOD_PSOR
#define CORE_PIN3_PORTSET	GPIOA_PSOR
#define CORE_PIN4_PORTSET	GPIOA_PSOR
#define CORE_PIN5_PORTSET	GPIOD_PSOR
#define CORE_PIN6_PORTSET	GPIOD_PSOR
#define CORE_PIN7_PORTSET	GPIOD_PSOR
#define CORE_PIN8_PORTSET	GPIOD_PSOR
#define CORE_PIN9_PORTSET	GPIOC_PSOR
#define CORE_PIN10_PORTSET	GPIOC_PSOR
#define CORE_PIN11_PORTSET	GPIOC_PSOR
#define CORE_PIN12_PORTSET	GPIOC_PSOR
#define CORE_PIN13_PORTSET	GPIOC_PSOR
#define CORE_PIN14_PORTSET	GPIOD_PSOR
#define CORE_PIN15_PORTSET	GPIOC_PSOR
#define CORE_PIN16_PORTSET	GPIOB_PSOR
#define CORE_PIN17_PORTSET	GPIOB_PSOR
#define CORE_PIN18_PORTSET	GPIOB_PSOR
#define CORE_PIN19_PORTSET	GPIOB_PSOR
#define CORE_PIN20_PORTSET	GPIOD_PSOR
#define CORE_PIN21_PORTSET	GPIOD_PSOR
#define CORE_PIN22_PORTSET	GPIOC_PSOR
#define CORE_PIN23_PORTSET	GPIOC_PSOR
#define CORE_PIN24_PORTSET	GPIOA_PSOR
#define CORE_PIN25_PORTSET	GPIOB_PSOR
#define CORE_PIN26_PORTSET	GPIOE_PSOR
#define CORE_PIN27_PORTSET	GPIOC_PSOR
#define CORE_PIN28_PORTSET	GPIOC_PSOR
#define CORE_PIN29_PORTSET	GPIOC_PSOR
#define CORE_PIN30_PORTSET	GPIOC_PSOR
#define CORE_PIN31_PORTSET	GPIOE_PSOR
#define CORE_PIN32_PORTSET	GPIOB_PSOR
#define CORE_PIN33_PORTSET	GPIOA_PSOR

#define CORE_PIN0_PORTCLEAR	GPIOB_PCOR
#define CORE_PIN1_PORTCLEAR	GPIOB_PCOR
#define CORE_PIN2_PORTCLEAR	GPIOD_PCOR
#define CORE_PIN3_PORTCLEAR	GPIOA_PCOR
#define CORE_PIN4_PORTCLEAR	GPIOA_PCOR
#define CORE_PIN5_PORTCLEAR	GPIOD_PCOR
#define CORE_PIN6_PORTCLEAR	GPIOD_PCOR
#define CORE_PIN7_PORTCLEAR	GPIOD_PCOR
#define CORE_PIN8_PORTCLEAR	GPIOD_PCOR
#define CORE_PIN9_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN10_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN11_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN12_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN13_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN14_PORTCLEAR	GPIOD_PCOR
#define CORE_PIN15_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN16_PORTCLEAR	GPIOB_PCOR
#define CORE_PIN17_PORTCLEAR	GPIOB_PCOR
#define CORE_PIN18_PORTCLEAR	GPIOB_PCOR
#define CORE_PIN19_PORTCLEAR	GPIOB_PCOR
#define CORE_PIN20_PORTCLEAR	GPIOD_PCOR
#define CORE_PIN21_PORTCLEAR	GPIOD_PCOR
#define CORE_PIN22_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN23_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN24_PORTCLEAR	GPIOA_PCOR
#define CORE_PIN25_PORTCLEAR	GPIOB_PCOR
#define CORE_PIN26_PORTCLEAR	GPIOE_PCOR
#define CORE_PIN27_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN28_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN29_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN30_PORTCLEAR	GPIOC_PCOR
#define CORE_PIN31_PORTCLEAR	GPIOE_PCOR
#define CORE_PIN32_PORTCLEAR	GPIOB_PCOR
#define CORE_PIN33_PORTCLEAR	GPIOA_PCOR

#define CORE_PIN0_DDRREG	GPIOB_PDDR
#define CORE_PIN1_DDRREG	GPIOB_PDDR
#define CORE_PIN2_DDRREG	GPIOD_PDDR
#define CORE_PIN3_DDRREG	GPIOA_PDDR
#define CORE_PIN4_DDRREG	GPIOA_PDDR
#define CORE_PIN5_DDRREG	GPIOD_PDDR
#define CORE_PIN6_DDRREG	GPIOD_PDDR
#define CORE_PIN7_DDRREG	GPIOD_PDDR
#define CORE_PIN8_DDRREG	GPIOD_PDDR
#define CORE_PIN9_DDRREG	GPIOC_PDDR
#define CORE_PIN10_DDRREG	GPIOC_PDDR
#define CORE_PIN11_DDRREG	GPIOC_PDDR
#define CORE_PIN12_DDRREG	GPIOC_PDDR
#define CORE_PIN13_DDRREG	GPIOC_PDDR
#define CORE_PIN14_DDRREG	GPIOD_PDDR
#define CORE_PIN15_DDRREG	GPIOC_PDDR
#define CORE_PIN16_DDRREG	GPIOB_PDDR
#define CORE_PIN17_DDRREG	GPIOB_PDDR
#define CORE_PIN18_DDRREG	GPIOB_PDDR
#define CORE_PIN19_DDRREG	GPIOB_PDDR
#define CORE_PIN20_DDRREG	GPIOD_PDDR
#define CORE_PIN21_DDRREG	GPIOD_PDDR
#define CORE_PIN22_DDRREG	GPIOC_PDDR
#define CORE_PIN23_DDRREG	GPIOC_PDDR
#define CORE_PIN24_DDRREG	GPIOA_PDDR
#define CORE_PIN25_DDRREG	GPIOB_PDDR
#define CORE_PIN26_DDRREG	GPIOE_PDDR
#define CORE_PIN27_DDRREG	GPIOC_PDDR
#define CORE_PIN28_DDRREG	GPIOC_PDDR
#define CORE_PIN29_DDRREG	GPIOC_PDDR
#define CORE_PIN30_DDRREG	GPIOC_PDDR
#define CORE_PIN31_DDRREG	GPIOE_PDDR
#define CORE_PIN32_DDRREG	GPIOB_PDDR
#define CORE_PIN33_DDRREG	GPIOA_PDDR

#define CORE_PIN0_PINREG	GPIOB_PDIR
#define CORE_PIN1_PINREG	GPIOB_PDIR
#define CORE_PIN2_PINREG	GPIOD_PDIR
#define CORE_PIN3_PINREG	GPIOA_PDIR
#define CORE_PIN4_PINREG	GPIOA_PDIR
#define CORE_PIN5_PINREG	GPIOD_PDIR
#define CORE_PIN6_PINREG	GPIOD_PDIR
#define CORE_PIN7_PINREG	GPIOD_PDIR
#define CORE_PIN8_PINREG	GPIOD_PDIR
#define CORE_PIN9_PINREG	GPIOC_PDIR
#define CORE_PIN10_PINREG	GPIOC_PDIR
#define CORE_PIN11_PINREG	GPIOC_PDIR
#define CORE_PIN12_PINREG	GPIOC_PDIR
#define CORE_PIN13_PINREG	GPIOC_PDIR
#define CORE_PIN14_PINREG	GPIOD_PDIR
#define CORE_PIN15_PINREG	GPIOC_PDIR
#define CORE_PIN16_PINREG	GPIOB_PDIR
#define CORE_PIN17_PINREG	GPIOB_PDIR
#define CORE_PIN18_PINREG	GPIOB_PDIR
#define CORE_PIN19_PINREG	GPIOB_PDIR
#define CORE_PIN20_PINREG	GPIOD_PDIR
#define CORE_PIN21_PINREG	GPIOD_PDIR
#define CORE_PIN22_PINREG	GPIOC_PDIR
#define CORE_PIN23_PINREG	GPIOC_PDIR
#define CORE_PIN24_PINREG	GPIOA_PDIR
#define CORE_PIN25_PINREG	GPIOB_PDIR
#define CORE_PIN26_PINREG	GPIOE_PDIR
#define CORE_PIN27_PINREG	GPIOC_PDIR
#define CORE_PIN28_PINREG	GPIOC_PDIR
#define CORE_PIN29_PINREG	GPIOC_PDIR
#define CORE_PIN30_PINREG	GPIOC_PDIR
#define CORE_PIN31_PINREG	GPIOE_PDIR
#define CORE_PIN32_PINREG	GPIOB_PDIR
#define CORE_PIN33_PINREG	GPIOA_PDIR

#define CORE_PIN0_CONFIG	PORTB_PCR16
#define CORE_PIN1_CONFIG	PORTB_PCR17
#define CORE_PIN2_CONFIG	PORTD_PCR0
#define CORE_PIN3_CONFIG	PORTA_PCR12
#define CORE_PIN4_CONFIG	PORTA_PCR13
#define CORE_PIN5_CONFIG	PORTD_PCR7
#define CORE_PIN6_CONFIG	PORTD_PCR4
#define CORE_PIN7_CONFIG	PORTD_PCR2
#define CORE_PIN8_CONFIG	PORTD_PCR3
#define CORE_PIN9_CONFIG	PORTC_PCR3
#define CORE_PIN10_CONFIG	PORTC_PCR4
#define CORE_PIN11_CONFIG	PORTC_PCR6
#define CORE_PIN12_CONFIG	PORTC_PCR7
#define CORE_PIN13_CONFIG	PORTC_PCR5
#define CORE_PIN14_CONFIG	PORTD_PCR1
#define CORE_PIN15_CONFIG	PORTC_PCR0
#define CORE_PIN16_CONFIG	PORTB_PCR0
#define CORE_PIN17_CONFIG	PORTB_PCR1
#define CORE_PIN18_CONFIG	PORTB_PCR3
#define CORE_PIN19_CONFIG	PORTB_PCR2
#define CORE_PIN20_CONFIG	PORTD_PCR5
#define CORE_PIN21_CONFIG	PORTD_PCR6
#define CORE_PIN22_CONFIG	PORTC_PCR1
#define CORE_PIN23_CONFIG	PORTC_PCR2
#define CORE_PIN24_CONFIG	PORTA_PCR5
#define CORE_PIN25_CONFIG	PORTB_PCR19
#define CORE_PIN26_CONFIG	PORTE_PCR1
#define CORE_PIN27_CONFIG	PORTC_PCR9
#define CORE_PIN28_CONFIG	PORTC_PCR8
#define CORE_PIN29_CONFIG	PORTC_PCR10
#define CORE_PIN30_CONFIG	PORTC_PCR11
#define CORE_PIN31_CONFIG	PORTE_PCR0
#define CORE_PIN32_CONFIG	PORTB_PCR18
#define CORE_PIN33_CONFIG	PORTA_PCR4

#define CORE_ADC0_PIN		14
#define CORE_ADC1_PIN		15
#define CORE_ADC2_PIN		16
#define CORE_ADC3_PIN		17
#define CORE_ADC4_PIN		18
#define CORE_ADC5_PIN		19
#define CORE_ADC6_PIN		20
#define CORE_ADC7_PIN		21
#define CORE_ADC8_PIN		22
#define CORE_ADC9_PIN		23
#define CORE_ADC10_PIN		34
#define CORE_ADC11_PIN		35
#define CORE_ADC12_PIN		36
#define CORE_ADC13_PIN		37

#define CORE_RXD0_PIN		0
#define CORE_TXD0_PIN		1
#define CORE_RXD1_PIN		9
#define CORE_TXD1_PIN		10
#define CORE_RXD2_PIN		7
#define CORE_TXD2_PIN		8

#define CORE_INT0_PIN		0
#define CORE_INT1_PIN		1
#define CORE_INT2_PIN		2
#define CORE_INT3_PIN		3
#define CORE_INT4_PIN		4
#define CORE_INT5_PIN		5
#define CORE_INT6_PIN		6
#define CORE_INT7_PIN		7
#define CORE_INT8_PIN		8
#define CORE_INT9_PIN		9
#define CORE_INT10_PIN		10
#define CORE_INT11_PIN		11
#define CORE_INT12_PIN		12
#define CORE_INT13_PIN		13
#define CORE_INT14_PIN		14
#define CORE_INT15_PIN		15
#define CORE_INT16_PIN		16
#define CORE_INT17_PIN		17
#define CORE_INT18_PIN		18
#define CORE_INT19_PIN		19
#define CORE_INT20_PIN		20
#define CORE_INT21_PIN		21
#define CORE_INT22_PIN		22
#define CORE_INT23_PIN		23
#define CORE_INT24_PIN		24
#define CORE_INT25_PIN		25
#define CORE_INT26_PIN		26
#define CORE_INT27_PIN		27
#define CORE_INT28_PIN		28
#define CORE_INT29_PIN		29
#define CORE_INT30_PIN		30
#define CORE_INT31_PIN		31
#define CORE_INT32_PIN		32
#define CORE_INT33_PIN		33
#define CORE_INT_EVERY_PIN	1




#ifdef __cplusplus
extern "C" {
#endif

void digitalWrite(uint8_t pin, uint8_t val);
static inline void digitalWriteFast(uint8_t pin, uint8_t val) __attribute__((always_inline, unused));
static inline void digitalWriteFast(uint8_t pin, uint8_t val)
{
	if (__builtin_constant_p(pin)) {
		if (val) {
			if (pin == 0) {
				CORE_PIN0_PORTSET = CORE_PIN0_BITMASK;
			} else if (pin == 1) {
				CORE_PIN1_PORTSET = CORE_PIN1_BITMASK;
			} else if (pin == 2) {
				CORE_PIN2_PORTSET = CORE_PIN2_BITMASK;
			} else if (pin == 3) {
				CORE_PIN3_PORTSET = CORE_PIN3_BITMASK;
			} else if (pin == 4) {
				CORE_PIN4_PORTSET = CORE_PIN4_BITMASK;
			} else if (pin == 5) {
				CORE_PIN5_PORTSET = CORE_PIN5_BITMASK;
			} else if (pin == 6) {
				CORE_PIN6_PORTSET = CORE_PIN6_BITMASK;
			} else if (pin == 7) {
				CORE_PIN7_PORTSET = CORE_PIN7_BITMASK;
			} else if (pin == 8) {
				CORE_PIN8_PORTSET = CORE_PIN8_BITMASK;
			} else if (pin == 9) {
				CORE_PIN9_PORTSET = CORE_PIN9_BITMASK;
			} else if (pin == 10) {
				CORE_PIN10_PORTSET = CORE_PIN10_BITMASK;
			} else if (pin == 11) {
				CORE_PIN11_PORTSET = CORE_PIN11_BITMASK;
			} else if (pin == 12) {
				CORE_PIN12_PORTSET = CORE_PIN12_BITMASK;
			} else if (pin == 13) {
				CORE_PIN13_PORTSET = CORE_PIN13_BITMASK;
			} else if (pin == 14) {
				CORE_PIN14_PORTSET = CORE_PIN14_BITMASK;
			} else if (pin == 15) {
				CORE_PIN15_PORTSET = CORE_PIN15_BITMASK;
			} else if (pin == 16) {
				CORE_PIN16_PORTSET = CORE_PIN16_BITMASK;
			} else if (pin == 17) {
				CORE_PIN17_PORTSET = CORE_PIN17_BITMASK;
			} else if (pin == 18) {
				CORE_PIN18_PORTSET = CORE_PIN18_BITMASK;
			} else if (pin == 19) {
				CORE_PIN19_PORTSET = CORE_PIN19_BITMASK;
			} else if (pin == 20) {
				CORE_PIN20_PORTSET = CORE_PIN20_BITMASK;
			} else if (pin == 21) {
				CORE_PIN21_PORTSET = CORE_PIN21_BITMASK;
			} else if (pin == 22) {
				CORE_PIN22_PORTSET = CORE_PIN22_BITMASK;
			} else if (pin == 23) {
				CORE_PIN23_PORTSET = CORE_PIN23_BITMASK;
			} else if (pin == 24) {
				CORE_PIN24_PORTSET = CORE_PIN24_BITMASK;
			} else if (pin == 25) {
				CORE_PIN25_PORTSET = CORE_PIN25_BITMASK;
			} else if (pin == 26) {
				CORE_PIN26_PORTSET = CORE_PIN26_BITMASK;
			} else if (pin == 27) {
				CORE_PIN27_PORTSET = CORE_PIN27_BITMASK;
			} else if (pin == 28) {
				CORE_PIN28_PORTSET = CORE_PIN28_BITMASK;
			} else if (pin == 29) {
				CORE_PIN29_PORTSET = CORE_PIN29_BITMASK;
			} else if (pin == 30) {
				CORE_PIN30_PORTSET = CORE_PIN30_BITMASK;
			} else if (pin == 31) {
				CORE_PIN31_PORTSET = CORE_PIN31_BITMASK;
			} else if (pin == 32) {
				CORE_PIN32_PORTSET = CORE_PIN32_BITMASK;
			} else if (pin == 33) {
				CORE_PIN33_PORTSET = CORE_PIN33_BITMASK;
			}
		} else {
			if (pin == 0) {
				CORE_PIN0_PORTCLEAR = CORE_PIN0_BITMASK;
			} else if (pin == 1) {
				CORE_PIN1_PORTCLEAR = CORE_PIN1_BITMASK;
			} else if (pin == 2) {
				CORE_PIN2_PORTCLEAR = CORE_PIN2_BITMASK;
			} else if (pin == 3) {
				CORE_PIN3_PORTCLEAR = CORE_PIN3_BITMASK;
			} else if (pin == 4) {
				CORE_PIN4_PORTCLEAR = CORE_PIN4_BITMASK;
			} else if (pin == 5) {
				CORE_PIN5_PORTCLEAR = CORE_PIN5_BITMASK;
			} else if (pin == 6) {
				CORE_PIN6_PORTCLEAR = CORE_PIN6_BITMASK;
			} else if (pin == 7) {
				CORE_PIN7_PORTCLEAR = CORE_PIN7_BITMASK;
			} else if (pin == 8) {
				CORE_PIN8_PORTCLEAR = CORE_PIN8_BITMASK;
			} else if (pin == 9) {
				CORE_PIN9_PORTCLEAR = CORE_PIN9_BITMASK;
			} else if (pin == 10) {
				CORE_PIN10_PORTCLEAR = CORE_PIN10_BITMASK;
			} else if (pin == 11) {
				CORE_PIN11_PORTCLEAR = CORE_PIN11_BITMASK;
			} else if (pin == 12) {
				CORE_PIN12_PORTCLEAR = CORE_PIN12_BITMASK;
			} else if (pin == 13) {
				CORE_PIN13_PORTCLEAR = CORE_PIN13_BITMASK;
			} else if (pin == 14) {
				CORE_PIN14_PORTCLEAR = CORE_PIN14_BITMASK;
			} else if (pin == 15) {
				CORE_PIN15_PORTCLEAR = CORE_PIN15_BITMASK;
			} else if (pin == 16) {
				CORE_PIN16_PORTCLEAR = CORE_PIN16_BITMASK;
			} else if (pin == 17) {
				CORE_PIN17_PORTCLEAR = CORE_PIN17_BITMASK;
			} else if (pin == 18) {
				CORE_PIN18_PORTCLEAR = CORE_PIN18_BITMASK;
			} else if (pin == 19) {
				CORE_PIN19_PORTCLEAR = CORE_PIN19_BITMASK;
			} else if (pin == 20) {
				CORE_PIN20_PORTCLEAR = CORE_PIN20_BITMASK;
			} else if (pin == 21) {
				CORE_PIN21_PORTCLEAR = CORE_PIN21_BITMASK;
			} else if (pin == 22) {
				CORE_PIN22_PORTCLEAR = CORE_PIN22_BITMASK;
			} else if (pin == 23) {
				CORE_PIN23_PORTCLEAR = CORE_PIN23_BITMASK;
			} else if (pin == 24) {
				CORE_PIN24_PORTCLEAR = CORE_PIN24_BITMASK;
			} else if (pin == 25) {
				CORE_PIN25_PORTCLEAR = CORE_PIN25_BITMASK;
			} else if (pin == 26) {
				CORE_PIN26_PORTCLEAR = CORE_PIN26_BITMASK;
			} else if (pin == 27) {
				CORE_PIN27_PORTCLEAR = CORE_PIN27_BITMASK;
			} else if (pin == 28) {
				CORE_PIN28_PORTCLEAR = CORE_PIN28_BITMASK;
			} else if (pin == 29) {
				CORE_PIN29_PORTCLEAR = CORE_PIN29_BITMASK;
			} else if (pin == 30) {
				CORE_PIN30_PORTCLEAR = CORE_PIN30_BITMASK;
			} else if (pin == 31) {
				CORE_PIN31_PORTCLEAR = CORE_PIN31_BITMASK;
			} else if (pin == 32) {
				CORE_PIN32_PORTCLEAR = CORE_PIN32_BITMASK;
			} else if (pin == 33) {
				CORE_PIN33_PORTCLEAR = CORE_PIN33_BITMASK;
			}
		}
	} else {
		if (val) {
			*portSetRegister(pin) = 1;
		} else {
			*portClearRegister(pin) = 1;
		}
	}
}

uint8_t digitalRead(uint8_t pin);
static inline uint8_t digitalReadFast(uint8_t pin) __attribute__((always_inline, unused));
static inline uint8_t digitalReadFast(uint8_t pin)
{
	if (__builtin_constant_p(pin)) {
		if (pin == 0) {
			return (CORE_PIN0_PINREG & CORE_PIN0_BITMASK) ? 1 : 0;
		} else if (pin == 1) {
			return (CORE_PIN1_PINREG & CORE_PIN1_BITMASK) ? 1 : 0;
		} else if (pin == 2) {
			return (CORE_PIN2_PINREG & CORE_PIN2_BITMASK) ? 1 : 0;
		} else if (pin == 3) {
			return (CORE_PIN3_PINREG & CORE_PIN3_BITMASK) ? 1 : 0;
		} else if (pin == 4) {
			return (CORE_PIN4_PINREG & CORE_PIN4_BITMASK) ? 1 : 0;
		} else if (pin == 5) {
			return (CORE_PIN5_PINREG & CORE_PIN5_BITMASK) ? 1 : 0;
		} else if (pin == 6) {
			return (CORE_PIN6_PINREG & CORE_PIN6_BITMASK) ? 1 : 0;
		} else if (pin == 7) {
			return (CORE_PIN7_PINREG & CORE_PIN7_BITMASK) ? 1 : 0;
		} else if (pin == 8) {
			return (CORE_PIN8_PINREG & CORE_PIN8_BITMASK) ? 1 : 0;
		} else if (pin == 9) {
			return (CORE_PIN9_PINREG & CORE_PIN9_BITMASK) ? 1 : 0;
		} else if (pin == 10) {
			return (CORE_PIN10_PINREG & CORE_PIN10_BITMASK) ? 1 : 0;
		} else if (pin == 11) {
			return (CORE_PIN11_PINREG & CORE_PIN11_BITMASK) ? 1 : 0;
		} else if (pin == 12) {
			return (CORE_PIN12_PINREG & CORE_PIN12_BITMASK) ? 1 : 0;
		} else if (pin == 13) {
			return (CORE_PIN13_PINREG & CORE_PIN13_BITMASK) ? 1 : 0;
		} else if (pin == 14) {
			return (CORE_PIN14_PINREG & CORE_PIN14_BITMASK) ? 1 : 0;
		} else if (pin == 15) {
			return (CORE_PIN15_PINREG & CORE_PIN15_BITMASK) ? 1 : 0;
		} else if (pin == 16) {
			return (CORE_PIN16_PINREG & CORE_PIN16_BITMASK) ? 1 : 0;
		} else if (pin == 17) {
			return (CORE_PIN17_PINREG & CORE_PIN17_BITMASK) ? 1 : 0;
		} else if (pin == 18) {
			return (CORE_PIN18_PINREG & CORE_PIN18_BITMASK) ? 1 : 0;
		} else if (pin == 19) {
			return (CORE_PIN19_PINREG & CORE_PIN19_BITMASK) ? 1 : 0;
		} else if (pin == 20) {
			return (CORE_PIN20_PINREG & CORE_PIN20_BITMASK) ? 1 : 0;
		} else if (pin == 21) {
			return (CORE_PIN21_PINREG & CORE_PIN21_BITMASK) ? 1 : 0;
		} else if (pin == 22) {
			return (CORE_PIN22_PINREG & CORE_PIN22_BITMASK) ? 1 : 0;
		} else if (pin == 23) {
			return (CORE_PIN23_PINREG & CORE_PIN23_BITMASK) ? 1 : 0;
		} else if (pin == 24) {
			return (CORE_PIN24_PINREG & CORE_PIN24_BITMASK) ? 1 : 0;
		} else if (pin == 25) {
			return (CORE_PIN25_PINREG & CORE_PIN25_BITMASK) ? 1 : 0;
		} else if (pin == 26) {
			return (CORE_PIN26_PINREG & CORE_PIN26_BITMASK) ? 1 : 0;
		} else if (pin == 27) {
			return (CORE_PIN27_PINREG & CORE_PIN27_BITMASK) ? 1 : 0;
		} else if (pin == 28) {
			return (CORE_PIN28_PINREG & CORE_PIN28_BITMASK) ? 1 : 0;
		} else if (pin == 29) {
			return (CORE_PIN29_PINREG & CORE_PIN29_BITMASK) ? 1 : 0;
		} else if (pin == 30) {
			return (CORE_PIN30_PINREG & CORE_PIN30_BITMASK) ? 1 : 0;
		} else if (pin == 31) {
			return (CORE_PIN31_PINREG & CORE_PIN31_BITMASK) ? 1 : 0;
		} else if (pin == 32) {
			return (CORE_PIN32_PINREG & CORE_PIN32_BITMASK) ? 1 : 0;
		} else if (pin == 33) {
			return (CORE_PIN33_PINREG & CORE_PIN33_BITMASK) ? 1 : 0;
		} else {
			return 0;
		}
	} else {
		return *portInputRegister(pin);
	}
}


void pinMode(uint8_t pin, uint8_t mode);
void init_pins(void);
void analogWrite(uint8_t pin, int val);
void analogWriteRes(uint32_t bits);
static inline void analogWriteResolution(uint32_t bits) { analogWriteRes(bits); }
void analogWriteFrequency(uint8_t pin, uint32_t frequency);
void analogWriteDAC0(int val);
void attachInterrupt(uint8_t pin, void (*function)(void), int mode);
void detachInterrupt(uint8_t pin);
void _init_Teensyduino_internal_(void);

int analogRead(uint8_t pin);
void analogReference(uint8_t type);
void analogReadRes(unsigned int bits);
static inline void analogReadResolution(unsigned int bits) { analogReadRes(bits); }
void analogReadAveraging(unsigned int num);
void analog_init(void);

#define DEFAULT         0
#define INTERNAL        2
#define INTERNAL1V2     2
#define INTERNAL1V1     2
#define EXTERNAL        0

int touchRead(uint8_t pin);


static inline void shiftOut(uint8_t, uint8_t, uint8_t, uint8_t) __attribute__((always_inline, unused));
extern void _shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value) __attribute__((noinline));
extern void shiftOut_lsbFirst(uint8_t dataPin, uint8_t clockPin, uint8_t value) __attribute__((noinline));
extern void shiftOut_msbFirst(uint8_t dataPin, uint8_t clockPin, uint8_t value) __attribute__((noinline));

static inline void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value)
{
        if (__builtin_constant_p(bitOrder)) {
                if (bitOrder == LSBFIRST) {
                        shiftOut_lsbFirst(dataPin, clockPin, value);
                } else {
                        shiftOut_msbFirst(dataPin, clockPin, value);
                }
        } else {
                _shiftOut(dataPin, clockPin, bitOrder, value);
        }
}

static inline uint8_t shiftIn(uint8_t, uint8_t, uint8_t) __attribute__((always_inline, unused));
extern uint8_t _shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) __attribute__((noinline));
extern uint8_t shiftIn_lsbFirst(uint8_t dataPin, uint8_t clockPin) __attribute__((noinline));
extern uint8_t shiftIn_msbFirst(uint8_t dataPin, uint8_t clockPin) __attribute__((noinline));

static inline uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder)
{
        if (__builtin_constant_p(bitOrder)) {
                if (bitOrder == LSBFIRST) {
                        return shiftIn_lsbFirst(dataPin, clockPin);
                } else {
                        return shiftIn_msbFirst(dataPin, clockPin);
                }
        } else {
                return _shiftIn(dataPin, clockPin, bitOrder);
        }
}

void _reboot_Teensyduino_(void) __attribute__((noreturn));
void _restart_Teensyduino_(void) __attribute__((noreturn));

void yield(void);

void delay(uint32_t msec);

extern volatile uint32_t systick_millis_count;

static inline uint32_t millis(void) __attribute__((always_inline, unused));
static inline uint32_t millis(void)
{
	volatile uint32_t ret = systick_millis_count; // single aligned 32 bit is atomic;
	return ret;
}

uint32_t micros(void);

static inline void delayMicroseconds(uint32_t) __attribute__((always_inline, unused));
static inline void delayMicroseconds(uint32_t usec)
{
#if F_CPU == 168000000
	uint32_t n = usec * 56;
#elif F_CPU == 144000000
	uint32_t n = usec * 48;
#elif F_CPU == 120000000
	uint32_t n = usec * 40;
#elif F_CPU == 96000000
	uint32_t n = usec << 5;
#elif F_CPU == 72000000
	uint32_t n = usec * 24;
#elif F_CPU == 48000000
	uint32_t n = usec << 4;
#elif F_CPU == 24000000
	uint32_t n = usec << 3;
#elif F_CPU == 16000000
	uint32_t n = usec << 2;
#elif F_CPU == 8000000
	uint32_t n = usec << 1;
#elif F_CPU == 4000000
	uint32_t n = usec;
#elif F_CPU == 2000000
	uint32_t n = usec >> 1;
#endif
    // changed because a delay of 1 micro Sec @ 2MHz will be 0
	if (n == 0) return;
	__asm__ volatile(
		"L_%=_delayMicroseconds:"		"\n\t"
#if F_CPU < 24000000
		"nop"					"\n\t"
#endif
		"subs   %0, #1"				"\n\t"
		"bne    L_%=_delayMicroseconds"		"\n"
		: "+r" (n) :
	);
}

#ifdef __cplusplus
}
#endif








#ifdef __cplusplus
extern "C" {
#endif
unsigned long rtc_get(void);
void rtc_set(unsigned long t);
void rtc_compensate(int adjust);
#ifdef __cplusplus
}
class teensy3_clock_class
{
public:
	static unsigned long get(void) __attribute__((always_inline)) { return rtc_get(); }
	static void set(unsigned long t) __attribute__((always_inline)) { rtc_set(t); }
	static void compensate(int adj) __attribute__((always_inline)) { rtc_compensate(adj); }
};
extern teensy3_clock_class Teensy3Clock;
#endif




#endif
