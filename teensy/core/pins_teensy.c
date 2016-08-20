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

#include "core_pins.h"
#include "pins_arduino.h"
#include "HardwareSerial.h"


#if defined(KINETISK)
#define GPIO_BITBAND_ADDR(reg, bit) (((uint32_t)&(reg) - 0x40000000) * 32 + (bit) * 4 + 0x42000000)
#define GPIO_BITBAND_PTR(reg, bit) ((uint32_t *)GPIO_BITBAND_ADDR((reg), (bit)))
//#define GPIO_SET_BIT(reg, bit) (*GPIO_BITBAND_PTR((reg), (bit)) = 1)
//#define GPIO_CLR_BIT(reg, bit) (*GPIO_BITBAND_PTR((reg), (bit)) = 0)
const struct digital_pin_bitband_and_config_table_struct digital_pin_to_info_PGM[] = {
	{GPIO_BITBAND_PTR(CORE_PIN0_PORTREG, CORE_PIN0_BIT), &CORE_PIN0_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN1_PORTREG, CORE_PIN1_BIT), &CORE_PIN1_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN2_PORTREG, CORE_PIN2_BIT), &CORE_PIN2_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN3_PORTREG, CORE_PIN3_BIT), &CORE_PIN3_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN4_PORTREG, CORE_PIN4_BIT), &CORE_PIN4_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN5_PORTREG, CORE_PIN5_BIT), &CORE_PIN5_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN6_PORTREG, CORE_PIN6_BIT), &CORE_PIN6_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN7_PORTREG, CORE_PIN7_BIT), &CORE_PIN7_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN8_PORTREG, CORE_PIN8_BIT), &CORE_PIN8_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN9_PORTREG, CORE_PIN9_BIT), &CORE_PIN9_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN10_PORTREG, CORE_PIN10_BIT), &CORE_PIN10_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN11_PORTREG, CORE_PIN11_BIT), &CORE_PIN11_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN12_PORTREG, CORE_PIN12_BIT), &CORE_PIN12_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN13_PORTREG, CORE_PIN13_BIT), &CORE_PIN13_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN14_PORTREG, CORE_PIN14_BIT), &CORE_PIN14_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN15_PORTREG, CORE_PIN15_BIT), &CORE_PIN15_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN16_PORTREG, CORE_PIN16_BIT), &CORE_PIN16_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN17_PORTREG, CORE_PIN17_BIT), &CORE_PIN17_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN18_PORTREG, CORE_PIN18_BIT), &CORE_PIN18_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN19_PORTREG, CORE_PIN19_BIT), &CORE_PIN19_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN20_PORTREG, CORE_PIN20_BIT), &CORE_PIN20_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN21_PORTREG, CORE_PIN21_BIT), &CORE_PIN21_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN22_PORTREG, CORE_PIN22_BIT), &CORE_PIN22_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN23_PORTREG, CORE_PIN23_BIT), &CORE_PIN23_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN24_PORTREG, CORE_PIN24_BIT), &CORE_PIN24_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN25_PORTREG, CORE_PIN25_BIT), &CORE_PIN25_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN26_PORTREG, CORE_PIN26_BIT), &CORE_PIN26_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN27_PORTREG, CORE_PIN27_BIT), &CORE_PIN27_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN28_PORTREG, CORE_PIN28_BIT), &CORE_PIN28_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN29_PORTREG, CORE_PIN29_BIT), &CORE_PIN29_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN30_PORTREG, CORE_PIN30_BIT), &CORE_PIN30_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN31_PORTREG, CORE_PIN31_BIT), &CORE_PIN31_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN32_PORTREG, CORE_PIN32_BIT), &CORE_PIN32_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN33_PORTREG, CORE_PIN33_BIT), &CORE_PIN33_CONFIG},
#ifdef CORE_PIN34_PORTREG
	{GPIO_BITBAND_PTR(CORE_PIN34_PORTREG, CORE_PIN34_BIT), &CORE_PIN34_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN35_PORTREG, CORE_PIN35_BIT), &CORE_PIN35_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN36_PORTREG, CORE_PIN36_BIT), &CORE_PIN36_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN37_PORTREG, CORE_PIN37_BIT), &CORE_PIN37_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN38_PORTREG, CORE_PIN38_BIT), &CORE_PIN38_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN39_PORTREG, CORE_PIN39_BIT), &CORE_PIN39_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN40_PORTREG, CORE_PIN40_BIT), &CORE_PIN40_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN41_PORTREG, CORE_PIN41_BIT), &CORE_PIN41_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN42_PORTREG, CORE_PIN42_BIT), &CORE_PIN42_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN43_PORTREG, CORE_PIN43_BIT), &CORE_PIN43_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN44_PORTREG, CORE_PIN44_BIT), &CORE_PIN44_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN45_PORTREG, CORE_PIN45_BIT), &CORE_PIN45_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN46_PORTREG, CORE_PIN46_BIT), &CORE_PIN46_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN47_PORTREG, CORE_PIN47_BIT), &CORE_PIN47_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN48_PORTREG, CORE_PIN48_BIT), &CORE_PIN48_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN49_PORTREG, CORE_PIN49_BIT), &CORE_PIN49_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN50_PORTREG, CORE_PIN50_BIT), &CORE_PIN50_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN51_PORTREG, CORE_PIN51_BIT), &CORE_PIN51_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN52_PORTREG, CORE_PIN52_BIT), &CORE_PIN52_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN53_PORTREG, CORE_PIN53_BIT), &CORE_PIN53_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN54_PORTREG, CORE_PIN54_BIT), &CORE_PIN54_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN55_PORTREG, CORE_PIN55_BIT), &CORE_PIN55_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN56_PORTREG, CORE_PIN56_BIT), &CORE_PIN56_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN57_PORTREG, CORE_PIN57_BIT), &CORE_PIN57_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN58_PORTREG, CORE_PIN58_BIT), &CORE_PIN58_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN59_PORTREG, CORE_PIN59_BIT), &CORE_PIN59_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN60_PORTREG, CORE_PIN60_BIT), &CORE_PIN60_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN61_PORTREG, CORE_PIN61_BIT), &CORE_PIN61_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN62_PORTREG, CORE_PIN62_BIT), &CORE_PIN62_CONFIG},
	{GPIO_BITBAND_PTR(CORE_PIN63_PORTREG, CORE_PIN63_BIT), &CORE_PIN63_CONFIG},
#endif
};

#elif defined(KINETISL)
const struct digital_pin_bitband_and_config_table_struct digital_pin_to_info_PGM[] = {
	{((volatile uint8_t *)&CORE_PIN0_PORTREG + (CORE_PIN0_BIT >> 3)), &CORE_PIN0_CONFIG, (1<<(CORE_PIN0_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN1_PORTREG + (CORE_PIN1_BIT >> 3)), &CORE_PIN1_CONFIG, (1<<(CORE_PIN1_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN2_PORTREG + (CORE_PIN2_BIT >> 3)), &CORE_PIN2_CONFIG, (1<<(CORE_PIN2_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN3_PORTREG + (CORE_PIN3_BIT >> 3)), &CORE_PIN3_CONFIG, (1<<(CORE_PIN3_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN4_PORTREG + (CORE_PIN4_BIT >> 3)), &CORE_PIN4_CONFIG, (1<<(CORE_PIN4_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN5_PORTREG + (CORE_PIN5_BIT >> 3)), &CORE_PIN5_CONFIG, (1<<(CORE_PIN5_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN6_PORTREG + (CORE_PIN6_BIT >> 3)), &CORE_PIN6_CONFIG, (1<<(CORE_PIN6_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN7_PORTREG + (CORE_PIN7_BIT >> 3)), &CORE_PIN7_CONFIG, (1<<(CORE_PIN7_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN8_PORTREG + (CORE_PIN8_BIT >> 3)), &CORE_PIN8_CONFIG, (1<<(CORE_PIN8_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN9_PORTREG + (CORE_PIN9_BIT >> 3)), &CORE_PIN9_CONFIG, (1<<(CORE_PIN9_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN10_PORTREG + (CORE_PIN10_BIT >> 3)), &CORE_PIN10_CONFIG, (1<<(CORE_PIN10_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN11_PORTREG + (CORE_PIN11_BIT >> 3)), &CORE_PIN11_CONFIG, (1<<(CORE_PIN11_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN12_PORTREG + (CORE_PIN12_BIT >> 3)), &CORE_PIN12_CONFIG, (1<<(CORE_PIN12_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN13_PORTREG + (CORE_PIN13_BIT >> 3)), &CORE_PIN13_CONFIG, (1<<(CORE_PIN13_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN14_PORTREG + (CORE_PIN14_BIT >> 3)), &CORE_PIN14_CONFIG, (1<<(CORE_PIN14_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN15_PORTREG + (CORE_PIN15_BIT >> 3)), &CORE_PIN15_CONFIG, (1<<(CORE_PIN15_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN16_PORTREG + (CORE_PIN16_BIT >> 3)), &CORE_PIN16_CONFIG, (1<<(CORE_PIN16_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN17_PORTREG + (CORE_PIN17_BIT >> 3)), &CORE_PIN17_CONFIG, (1<<(CORE_PIN17_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN18_PORTREG + (CORE_PIN18_BIT >> 3)), &CORE_PIN18_CONFIG, (1<<(CORE_PIN18_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN19_PORTREG + (CORE_PIN19_BIT >> 3)), &CORE_PIN19_CONFIG, (1<<(CORE_PIN19_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN20_PORTREG + (CORE_PIN20_BIT >> 3)), &CORE_PIN20_CONFIG, (1<<(CORE_PIN20_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN21_PORTREG + (CORE_PIN21_BIT >> 3)), &CORE_PIN21_CONFIG, (1<<(CORE_PIN21_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN22_PORTREG + (CORE_PIN22_BIT >> 3)), &CORE_PIN22_CONFIG, (1<<(CORE_PIN22_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN23_PORTREG + (CORE_PIN23_BIT >> 3)), &CORE_PIN23_CONFIG, (1<<(CORE_PIN23_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN24_PORTREG + (CORE_PIN24_BIT >> 3)), &CORE_PIN24_CONFIG, (1<<(CORE_PIN24_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN25_PORTREG + (CORE_PIN25_BIT >> 3)), &CORE_PIN25_CONFIG, (1<<(CORE_PIN25_BIT & 7))},
	{((volatile uint8_t *)&CORE_PIN26_PORTREG + (CORE_PIN26_BIT >> 3)), &CORE_PIN26_CONFIG, (1<<(CORE_PIN26_BIT & 7))}
};

#endif


typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr intFunc[CORE_NUM_DIGITAL];
#if defined(KINETISK)
static void porta_interrupt(void);
static void portb_interrupt(void);
static void portc_interrupt(void);
static void portd_interrupt(void);
static void porte_interrupt(void);
#elif defined(KINETISL)
static void porta_interrupt(void);
static void portcd_interrupt(void);
#endif

void attachInterruptVector(enum IRQ_NUMBER_t irq, void (*function)(void))
{
	_VectorsRam[irq + 16] = function;
}

void attachInterrupt(uint8_t pin, void (*function)(void), int mode)
{
	volatile uint32_t *config;
	uint32_t cfg, mask;

	if (pin >= CORE_NUM_DIGITAL) return;
	switch (mode) {
	  case CHANGE:	mask = 0x0B; break;
	  case RISING:	mask = 0x09; break;
	  case FALLING:	mask = 0x0A; break;
	  case LOW:	mask = 0x08; break;
	  case HIGH:	mask = 0x0C; break;
	  default: return;
	}
	mask = (mask << 16) | 0x01000000;
	config = portConfigRegister(pin);

#if defined(KINETISK)
	attachInterruptVector(IRQ_PORTA, porta_interrupt);
	attachInterruptVector(IRQ_PORTB, portb_interrupt);
	attachInterruptVector(IRQ_PORTC, portc_interrupt);
	attachInterruptVector(IRQ_PORTD, portd_interrupt);
	attachInterruptVector(IRQ_PORTE, porte_interrupt);
#elif defined(KINETISL)
	attachInterruptVector(IRQ_PORTA, porta_interrupt);
	attachInterruptVector(IRQ_PORTCD, portcd_interrupt);
#endif
	__disable_irq();
	cfg = *config;
	cfg &= ~0x000F0000;		// disable any previous interrupt
	*config = cfg;
	intFunc[pin] = function;	// set the function pointer
	cfg |= mask;
	*config = cfg;			// enable the new interrupt
	__enable_irq();
}

void detachInterrupt(uint8_t pin)
{
	volatile uint32_t *config;

	config = portConfigRegister(pin);
	__disable_irq();
	*config = ((*config & ~0x000F0000) | 0x01000000);
	intFunc[pin] = NULL;
	__enable_irq();
}

#if defined(__MK20DX128__) || defined(__MK20DX256__)

static void porta_interrupt(void)
{
	uint32_t isfr = PORTA_ISFR;
	PORTA_ISFR = isfr;
	if ((isfr & CORE_PIN3_BITMASK) && intFunc[3]) intFunc[3]();
	if ((isfr & CORE_PIN4_BITMASK) && intFunc[4]) intFunc[4]();
	if ((isfr & CORE_PIN24_BITMASK) && intFunc[24]) intFunc[24]();
	if ((isfr & CORE_PIN33_BITMASK) && intFunc[33]) intFunc[33]();
}

static void portb_interrupt(void)
{
	uint32_t isfr = PORTB_ISFR;
	PORTB_ISFR = isfr;
	if ((isfr & CORE_PIN0_BITMASK) && intFunc[0]) intFunc[0]();
	if ((isfr & CORE_PIN1_BITMASK) && intFunc[1]) intFunc[1]();
	if ((isfr & CORE_PIN16_BITMASK) && intFunc[16]) intFunc[16]();
	if ((isfr & CORE_PIN17_BITMASK) && intFunc[17]) intFunc[17]();
	if ((isfr & CORE_PIN18_BITMASK) && intFunc[18]) intFunc[18]();
	if ((isfr & CORE_PIN19_BITMASK) && intFunc[19]) intFunc[19]();
	if ((isfr & CORE_PIN25_BITMASK) && intFunc[25]) intFunc[25]();
	if ((isfr & CORE_PIN32_BITMASK) && intFunc[32]) intFunc[32]();
}

static void portc_interrupt(void)
{
	// TODO: these are inefficent.  Use CLZ somehow....
	uint32_t isfr = PORTC_ISFR;
	PORTC_ISFR = isfr;
	if ((isfr & CORE_PIN9_BITMASK) && intFunc[9]) intFunc[9]();
	if ((isfr & CORE_PIN10_BITMASK) && intFunc[10]) intFunc[10]();
	if ((isfr & CORE_PIN11_BITMASK) && intFunc[11]) intFunc[11]();
	if ((isfr & CORE_PIN12_BITMASK) && intFunc[12]) intFunc[12]();
	if ((isfr & CORE_PIN13_BITMASK) && intFunc[13]) intFunc[13]();
	if ((isfr & CORE_PIN15_BITMASK) && intFunc[15]) intFunc[15]();
	if ((isfr & CORE_PIN22_BITMASK) && intFunc[22]) intFunc[22]();
	if ((isfr & CORE_PIN23_BITMASK) && intFunc[23]) intFunc[23]();
	if ((isfr & CORE_PIN27_BITMASK) && intFunc[27]) intFunc[27]();
	if ((isfr & CORE_PIN28_BITMASK) && intFunc[28]) intFunc[28]();
	if ((isfr & CORE_PIN29_BITMASK) && intFunc[29]) intFunc[29]();
	if ((isfr & CORE_PIN30_BITMASK) && intFunc[30]) intFunc[30]();
}

static void portd_interrupt(void)
{
	uint32_t isfr = PORTD_ISFR;
	PORTD_ISFR = isfr;
	if ((isfr & CORE_PIN2_BITMASK) && intFunc[2]) intFunc[2]();
	if ((isfr & CORE_PIN5_BITMASK) && intFunc[5]) intFunc[5]();
	if ((isfr & CORE_PIN6_BITMASK) && intFunc[6]) intFunc[6]();
	if ((isfr & CORE_PIN7_BITMASK) && intFunc[7]) intFunc[7]();
	if ((isfr & CORE_PIN8_BITMASK) && intFunc[8]) intFunc[8]();
	if ((isfr & CORE_PIN14_BITMASK) && intFunc[14]) intFunc[14]();
	if ((isfr & CORE_PIN20_BITMASK) && intFunc[20]) intFunc[20]();
	if ((isfr & CORE_PIN21_BITMASK) && intFunc[21]) intFunc[21]();
}

static void porte_interrupt(void)
{
	uint32_t isfr = PORTE_ISFR;
	PORTE_ISFR = isfr;
	if ((isfr & CORE_PIN26_BITMASK) && intFunc[26]) intFunc[26]();
	if ((isfr & CORE_PIN31_BITMASK) && intFunc[31]) intFunc[31]();
}

#elif defined(__MKL26Z64__)

static void porta_interrupt(void)
{
	uint32_t isfr = PORTA_ISFR;
	PORTA_ISFR = isfr;
	if ((isfr & CORE_PIN3_BITMASK) && intFunc[3]) intFunc[3]();
	if ((isfr & CORE_PIN4_BITMASK) && intFunc[4]) intFunc[4]();
}

static void portcd_interrupt(void)
{
	uint32_t isfr = PORTC_ISFR;
	PORTC_ISFR = isfr;
	if ((isfr & CORE_PIN9_BITMASK) && intFunc[9]) intFunc[9]();
	if ((isfr & CORE_PIN10_BITMASK) && intFunc[10]) intFunc[10]();
	if ((isfr & CORE_PIN11_BITMASK) && intFunc[11]) intFunc[11]();
	if ((isfr & CORE_PIN12_BITMASK) && intFunc[12]) intFunc[12]();
	if ((isfr & CORE_PIN13_BITMASK) && intFunc[13]) intFunc[13]();
	if ((isfr & CORE_PIN15_BITMASK) && intFunc[15]) intFunc[15]();
	if ((isfr & CORE_PIN22_BITMASK) && intFunc[22]) intFunc[22]();
	if ((isfr & CORE_PIN23_BITMASK) && intFunc[23]) intFunc[23]();
	isfr = PORTD_ISFR;
	PORTD_ISFR = isfr;
	if ((isfr & CORE_PIN2_BITMASK) && intFunc[2]) intFunc[2]();
	if ((isfr & CORE_PIN5_BITMASK) && intFunc[5]) intFunc[5]();
	if ((isfr & CORE_PIN6_BITMASK) && intFunc[6]) intFunc[6]();
	if ((isfr & CORE_PIN7_BITMASK) && intFunc[7]) intFunc[7]();
	if ((isfr & CORE_PIN8_BITMASK) && intFunc[8]) intFunc[8]();
	if ((isfr & CORE_PIN14_BITMASK) && intFunc[14]) intFunc[14]();
	if ((isfr & CORE_PIN20_BITMASK) && intFunc[20]) intFunc[20]();
	if ((isfr & CORE_PIN21_BITMASK) && intFunc[21]) intFunc[21]();
}

#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)

static void porta_interrupt(void)
{
	uint32_t isfr = PORTA_ISFR;
	PORTA_ISFR = isfr;
	if ((isfr & CORE_PIN3_BITMASK) && intFunc[3]) intFunc[3]();
	if ((isfr & CORE_PIN4_BITMASK) && intFunc[4]) intFunc[4]();
	if ((isfr & CORE_PIN25_BITMASK) && intFunc[25]) intFunc[25]();
	if ((isfr & CORE_PIN26_BITMASK) && intFunc[26]) intFunc[26]();
	if ((isfr & CORE_PIN27_BITMASK) && intFunc[27]) intFunc[27]();
	if ((isfr & CORE_PIN28_BITMASK) && intFunc[28]) intFunc[28]();
	if ((isfr & CORE_PIN39_BITMASK) && intFunc[39]) intFunc[39]();
	if ((isfr & CORE_PIN40_BITMASK) && intFunc[40]) intFunc[40]();
	if ((isfr & CORE_PIN41_BITMASK) && intFunc[41]) intFunc[41]();
	if ((isfr & CORE_PIN42_BITMASK) && intFunc[42]) intFunc[42]();
}

static void portb_interrupt(void)
{
	uint32_t isfr = PORTB_ISFR;
	PORTB_ISFR = isfr;
	if ((isfr & CORE_PIN0_BITMASK) && intFunc[0]) intFunc[0]();
	if ((isfr & CORE_PIN1_BITMASK) && intFunc[1]) intFunc[1]();
	if ((isfr & CORE_PIN16_BITMASK) && intFunc[16]) intFunc[16]();
	if ((isfr & CORE_PIN17_BITMASK) && intFunc[17]) intFunc[17]();
	if ((isfr & CORE_PIN18_BITMASK) && intFunc[18]) intFunc[18]();
	if ((isfr & CORE_PIN19_BITMASK) && intFunc[19]) intFunc[19]();
	if ((isfr & CORE_PIN29_BITMASK) && intFunc[29]) intFunc[29]();
	if ((isfr & CORE_PIN30_BITMASK) && intFunc[30]) intFunc[30]();
	if ((isfr & CORE_PIN31_BITMASK) && intFunc[31]) intFunc[31]();
	if ((isfr & CORE_PIN32_BITMASK) && intFunc[32]) intFunc[32]();
	if ((isfr & CORE_PIN43_BITMASK) && intFunc[43]) intFunc[43]();
	if ((isfr & CORE_PIN44_BITMASK) && intFunc[44]) intFunc[44]();
	if ((isfr & CORE_PIN45_BITMASK) && intFunc[45]) intFunc[45]();
	if ((isfr & CORE_PIN46_BITMASK) && intFunc[46]) intFunc[46]();
	if ((isfr & CORE_PIN49_BITMASK) && intFunc[49]) intFunc[49]();
	if ((isfr & CORE_PIN50_BITMASK) && intFunc[50]) intFunc[50]();
}

static void portc_interrupt(void)
{
	// TODO: these are inefficent.  Use CLZ somehow....
	uint32_t isfr = PORTC_ISFR;
	PORTC_ISFR = isfr;
	if ((isfr & CORE_PIN9_BITMASK) && intFunc[9]) intFunc[9]();
	if ((isfr & CORE_PIN10_BITMASK) && intFunc[10]) intFunc[10]();
	if ((isfr & CORE_PIN11_BITMASK) && intFunc[11]) intFunc[11]();
	if ((isfr & CORE_PIN12_BITMASK) && intFunc[12]) intFunc[12]();
	if ((isfr & CORE_PIN13_BITMASK) && intFunc[13]) intFunc[13]();
	if ((isfr & CORE_PIN15_BITMASK) && intFunc[15]) intFunc[15]();
	if ((isfr & CORE_PIN22_BITMASK) && intFunc[22]) intFunc[22]();
	if ((isfr & CORE_PIN23_BITMASK) && intFunc[23]) intFunc[23]();
	if ((isfr & CORE_PIN35_BITMASK) && intFunc[35]) intFunc[35]();
	if ((isfr & CORE_PIN36_BITMASK) && intFunc[36]) intFunc[36]();
	if ((isfr & CORE_PIN37_BITMASK) && intFunc[37]) intFunc[37]();
	if ((isfr & CORE_PIN38_BITMASK) && intFunc[38]) intFunc[38]();
}

static void portd_interrupt(void)
{
	uint32_t isfr = PORTD_ISFR;
	PORTD_ISFR = isfr;
	if ((isfr & CORE_PIN2_BITMASK) && intFunc[2]) intFunc[2]();
	if ((isfr & CORE_PIN5_BITMASK) && intFunc[5]) intFunc[5]();
	if ((isfr & CORE_PIN6_BITMASK) && intFunc[6]) intFunc[6]();
	if ((isfr & CORE_PIN7_BITMASK) && intFunc[7]) intFunc[7]();
	if ((isfr & CORE_PIN8_BITMASK) && intFunc[8]) intFunc[8]();
	if ((isfr & CORE_PIN14_BITMASK) && intFunc[14]) intFunc[14]();
	if ((isfr & CORE_PIN20_BITMASK) && intFunc[20]) intFunc[20]();
	if ((isfr & CORE_PIN21_BITMASK) && intFunc[21]) intFunc[21]();
	if ((isfr & CORE_PIN47_BITMASK) && intFunc[47]) intFunc[47]();
	if ((isfr & CORE_PIN48_BITMASK) && intFunc[48]) intFunc[48]();
	if ((isfr & CORE_PIN51_BITMASK) && intFunc[51]) intFunc[51]();
	if ((isfr & CORE_PIN52_BITMASK) && intFunc[52]) intFunc[52]();
	if ((isfr & CORE_PIN53_BITMASK) && intFunc[53]) intFunc[53]();
	if ((isfr & CORE_PIN54_BITMASK) && intFunc[54]) intFunc[54]();
	if ((isfr & CORE_PIN55_BITMASK) && intFunc[55]) intFunc[55]();
}

static void porte_interrupt(void)
{
	uint32_t isfr = PORTE_ISFR;
	PORTE_ISFR = isfr;
	if ((isfr & CORE_PIN24_BITMASK) && intFunc[24]) intFunc[24]();
	if ((isfr & CORE_PIN33_BITMASK) && intFunc[33]) intFunc[33]();
	if ((isfr & CORE_PIN34_BITMASK) && intFunc[34]) intFunc[34]();
	if ((isfr & CORE_PIN56_BITMASK) && intFunc[56]) intFunc[56]();
	if ((isfr & CORE_PIN57_BITMASK) && intFunc[57]) intFunc[57]();
	if ((isfr & CORE_PIN58_BITMASK) && intFunc[58]) intFunc[58]();
	if ((isfr & CORE_PIN59_BITMASK) && intFunc[59]) intFunc[59]();
	if ((isfr & CORE_PIN60_BITMASK) && intFunc[60]) intFunc[60]();
	if ((isfr & CORE_PIN61_BITMASK) && intFunc[61]) intFunc[61]();
	if ((isfr & CORE_PIN62_BITMASK) && intFunc[62]) intFunc[62]();
	if ((isfr & CORE_PIN63_BITMASK) && intFunc[63]) intFunc[63]();
}

#endif


#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)

unsigned long rtc_get(void)
{
	return RTC_TSR;
}

void rtc_set(unsigned long t)
{
	RTC_SR = 0;
	RTC_TPR = 0;
	RTC_TSR = t;
	RTC_SR = RTC_SR_TCE;
}

// adjust is the amount of crystal error to compensate, 1 = 0.1192 ppm
// For example, adjust = -100 is slows the clock by 11.92 ppm
//
void rtc_compensate(int adjust)
{
	uint32_t comp, interval, tcr;

	// This simple approach tries to maximize the interval.
	// Perhaps minimizing TCR would be better, so the
	// compensation is distributed more evenly across
	// many seconds, rather than saving it all up and then
	// altering one second up to +/- 0.38%
	if (adjust >= 0) {
		comp = adjust;
		interval = 256;
		while (1) {
			tcr = comp * interval;
			if (tcr < 128*256) break;
			if (--interval == 1) break;
		}
		tcr = tcr >> 8;
	} else {
		comp = -adjust;
		interval = 256;
		while (1) {
			tcr = comp * interval;
			if (tcr < 129*256) break;
			if (--interval == 1) break;
		}
		tcr = tcr >> 8;
		tcr = 256 - tcr;
	}
	RTC_TCR = ((interval - 1) << 8) | tcr;
}

#else

unsigned long rtc_get(void) { return 0; }
void rtc_set(unsigned long t) { }
void rtc_compensate(int adjust) { }

#endif



#if 0
// TODO: build system should define this
// so RTC is automatically initialized to approx correct time
// at least when the program begins running right after upload
#ifndef TIME_T
#define TIME_T 1350160272
#endif

void init_rtc(void)
{
	serial_print("init_rtc\n");
	//SIM_SCGC6 |= SIM_SCGC6_RTC;

	// enable the RTC crystal oscillator, for approx 12pf crystal
	if (!(RTC_CR & RTC_CR_OSCE)) {
		serial_print("start RTC oscillator\n");
		RTC_SR = 0;
		RTC_CR = RTC_CR_SC16P | RTC_CR_SC4P | RTC_CR_OSCE;
	}
	// should wait for crystal to stabilize.....

	serial_print("SR=");
	serial_phex32(RTC_SR);
	serial_print("\n");
	serial_print("CR=");
	serial_phex32(RTC_CR);
	serial_print("\n");
	serial_print("TSR=");
	serial_phex32(RTC_TSR);
	serial_print("\n");
	serial_print("TCR=");
	serial_phex32(RTC_TCR);
	serial_print("\n");

	if (RTC_SR & RTC_SR_TIF) {
		// enable the RTC
		RTC_SR = 0;
		RTC_TPR = 0;
		RTC_TSR = TIME_T;
		RTC_SR = RTC_SR_TCE;
	}
}
#endif

extern void usb_init(void);


// create a default PWM at the same 488.28 Hz as Arduino Uno

#if defined(KINETISK)
#define F_TIMER F_BUS
#elif defined(KINETISL)

#if F_CPU > 16000000
#define F_TIMER (F_PLL/2)
#else 
#define F_TIMER (F_PLL)
#endif//Low Power

#endif

#if F_TIMER == 120000000
#define DEFAULT_FTM_MOD (61440 - 1)
#define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 108000000
#define DEFAULT_FTM_MOD (55296 - 1)
#define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 96000000
#define DEFAULT_FTM_MOD (49152 - 1)
#define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 90000000
#define DEFAULT_FTM_MOD (46080 - 1)
#define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 80000000
#define DEFAULT_FTM_MOD (40960 - 1)
#define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 72000000
#define DEFAULT_FTM_MOD (36864 - 1)
#define DEFAULT_FTM_PRESCALE 2
#elif F_TIMER == 64000000
#define DEFAULT_FTM_MOD (65536 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 60000000
#define DEFAULT_FTM_MOD (61440 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 56000000
#define DEFAULT_FTM_MOD (57344 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 54000000
#define DEFAULT_FTM_MOD (55296 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 48000000
#define DEFAULT_FTM_MOD (49152 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 40000000
#define DEFAULT_FTM_MOD (40960 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 36000000
#define DEFAULT_FTM_MOD (36864 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_TIMER == 24000000
#define DEFAULT_FTM_MOD (49152 - 1)
#define DEFAULT_FTM_PRESCALE 0
#elif F_TIMER == 16000000
#define DEFAULT_FTM_MOD (32768 - 1)
#define DEFAULT_FTM_PRESCALE 0
#elif F_TIMER == 8000000
#define DEFAULT_FTM_MOD (16384 - 1)
#define DEFAULT_FTM_PRESCALE 0
#elif F_TIMER == 4000000
#define DEFAULT_FTM_MOD (8192 - 1)
#define DEFAULT_FTM_PRESCALE 0
#elif F_TIMER == 2000000
#define DEFAULT_FTM_MOD (4096 - 1)
#define DEFAULT_FTM_PRESCALE 0
#endif

//void init_pins(void)
void _init_Teensyduino_internal_(void)
{
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
	NVIC_ENABLE_IRQ(IRQ_PORTA);
	NVIC_ENABLE_IRQ(IRQ_PORTB);
	NVIC_ENABLE_IRQ(IRQ_PORTC);
	NVIC_ENABLE_IRQ(IRQ_PORTD);
	NVIC_ENABLE_IRQ(IRQ_PORTE);
#elif defined(__MKL26Z64__)
	NVIC_ENABLE_IRQ(IRQ_PORTA);
	NVIC_ENABLE_IRQ(IRQ_PORTCD);
#endif
	//SIM_SCGC6 |= SIM_SCGC6_FTM0;	// TODO: use bitband for atomic read-mod-write
	//SIM_SCGC6 |= SIM_SCGC6_FTM1;
	FTM0_CNT = 0;
	FTM0_MOD = DEFAULT_FTM_MOD;
	FTM0_C0SC = 0x28; // MSnB:MSnA = 10, ELSnB:ELSnA = 10
	FTM0_C1SC = 0x28;
	FTM0_C2SC = 0x28;
	FTM0_C3SC = 0x28;
	FTM0_C4SC = 0x28;
	FTM0_C5SC = 0x28;
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
	FTM0_C6SC = 0x28;
	FTM0_C7SC = 0x28;
#endif
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
	FTM3_C0SC = 0x28;
	FTM3_C1SC = 0x28;
	FTM3_C2SC = 0x28;
	FTM3_C3SC = 0x28;
	FTM3_C4SC = 0x28;
	FTM3_C5SC = 0x28;
	FTM3_C6SC = 0x28;
	FTM3_C7SC = 0x28;
#endif
	FTM0_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
	FTM1_CNT = 0;
	FTM1_MOD = DEFAULT_FTM_MOD;
	FTM1_C0SC = 0x28;
	FTM1_C1SC = 0x28;
	FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
#if defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(__MKL26Z64__)
	FTM2_CNT = 0;
	FTM2_MOD = DEFAULT_FTM_MOD;
	FTM2_C0SC = 0x28;
	FTM2_C1SC = 0x28;
	FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
#endif
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
	FTM3_CNT = 0;
	FTM3_MOD = DEFAULT_FTM_MOD;
	FTM3_C0SC = 0x28;
	FTM3_C1SC = 0x28;
	FTM3_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
#endif
#if defined(__MK66FX1M0__)
	SIM_SCGC2 |= SIM_SCGC2_TPM1;
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(2);
	TPM1_CNT = 0;
	TPM1_MOD = 32767;
	TPM1_C0SC = 0x28;
	TPM1_C1SC = 0x28;
	TPM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(0);
#endif
	analog_init();
	// for background about this startup delay, please see these conversations
	// https://forum.pjrc.com/threads/36606-startup-time-(400ms)?p=113980&viewfull=1#post113980
	// https://forum.pjrc.com/threads/31290-Teensey-3-2-Teensey-Loader-1-24-Issues?p=87273&viewfull=1#post87273
	delay(400);
	usb_init();
}


#if defined(__MK20DX128__)
#define FTM0_CH0_PIN 22
#define FTM0_CH1_PIN 23
#define FTM0_CH2_PIN  9
#define FTM0_CH3_PIN 10
#define FTM0_CH4_PIN  6
#define FTM0_CH5_PIN 20
#define FTM0_CH6_PIN 21
#define FTM0_CH7_PIN  5
#define FTM1_CH0_PIN  3
#define FTM1_CH1_PIN  4
#elif defined(__MK20DX256__)
#define FTM0_CH0_PIN 22
#define FTM0_CH1_PIN 23
#define FTM0_CH2_PIN  9
#define FTM0_CH3_PIN 10
#define FTM0_CH4_PIN  6
#define FTM0_CH5_PIN 20
#define FTM0_CH6_PIN 21
#define FTM0_CH7_PIN  5
#define FTM1_CH0_PIN  3
#define FTM1_CH1_PIN  4
#define FTM2_CH0_PIN 32
#define FTM2_CH1_PIN 25
#elif defined(__MKL26Z64__)
#define FTM0_CH0_PIN 22
#define FTM0_CH1_PIN 23
#define FTM0_CH2_PIN  9
#define FTM0_CH3_PIN 10
#define FTM0_CH4_PIN  6
#define FTM0_CH5_PIN 20
#define FTM1_CH0_PIN 16
#define FTM1_CH1_PIN 17
#define FTM2_CH0_PIN  3
#define FTM2_CH1_PIN  4
#elif defined(__MK64FX512__)
#define FTM0_CH0_PIN 22
#define FTM0_CH1_PIN 23
#define FTM0_CH2_PIN  9
#define FTM0_CH3_PIN 10
#define FTM0_CH4_PIN  6
#define FTM0_CH5_PIN 20
#define FTM0_CH6_PIN 21
#define FTM0_CH7_PIN  5
#define FTM1_CH0_PIN  3
#define FTM1_CH1_PIN  4
#define FTM2_CH0_PIN 29
#define FTM2_CH1_PIN 30
#define FTM3_CH0_PIN  2
#define FTM3_CH1_PIN 14
#define FTM3_CH2_PIN  7
#define FTM3_CH3_PIN  8
#define FTM3_CH4_PIN 35
#define FTM3_CH5_PIN 36
#define FTM3_CH6_PIN 37
#define FTM3_CH7_PIN 38
#elif defined(__MK66FX1M0__)
#define FTM0_CH0_PIN 22
#define FTM0_CH1_PIN 23
#define FTM0_CH2_PIN  9
#define FTM0_CH3_PIN 10
#define FTM0_CH4_PIN  6
#define FTM0_CH5_PIN 20
#define FTM0_CH6_PIN 21
#define FTM0_CH7_PIN  5
#define FTM1_CH0_PIN  3
#define FTM1_CH1_PIN  4
#define FTM2_CH0_PIN 29
#define FTM2_CH1_PIN 30
#define FTM3_CH0_PIN  2
#define FTM3_CH1_PIN 14
#define FTM3_CH2_PIN  7
#define FTM3_CH3_PIN  8
#define FTM3_CH4_PIN 35
#define FTM3_CH5_PIN 36
#define FTM3_CH6_PIN 37
#define FTM3_CH7_PIN 38
#define TPM1_CH0_PIN 16
#define TPM1_CH1_PIN 17
#endif
#define FTM_PINCFG(pin) FTM_PINCFG2(pin)
#define FTM_PINCFG2(pin) CORE_PIN ## pin ## _CONFIG

static uint8_t analog_write_res = 8;

// SOPT4 is SIM select clocks?
// FTM is clocked by the bus clock, either 24 or 48 MHz
// input capture can be FTM1_CH0, CMP0 or CMP1 or USB start of frame
// 24 MHz with reload 49152 to match Arduino's speed = 488.28125 Hz

void analogWrite(uint8_t pin, int val)
{
	uint32_t cval, max;

#if defined(__MK20DX256__)
	if (pin == A14) {
		uint8_t res = analog_write_res;
		if (res < 12) {
			val <<= 12 - res;
		} else if (res > 12) {
			val >>= res - 12;
		}
		analogWriteDAC0(val);
		return;
	}
#elif defined(__MKL26Z64__)
	if (pin == A12) {
		uint8_t res = analog_write_res;
		if (res < 12) {
			val <<= 12 - res;
		} else if (res > 12) {
			val >>= res - 12;
		}
		analogWriteDAC0(val);
		return;
	}
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
	if (pin == A21 || pin == A22) {
		uint8_t res = analog_write_res;
		if (res < 12) {
			val <<= 12 - res;
		} else if (res > 12) {
			val >>= res - 12;
		}
		if (pin == A21) analogWriteDAC0(val);
		else analogWriteDAC1(val);
		return;
	}
#endif

	max = 1 << analog_write_res;
	if (val <= 0) {
		digitalWrite(pin, LOW);
		pinMode(pin, OUTPUT);	// TODO: implement OUTPUT_LOW
		return;
	} else if (val >= max) {
		digitalWrite(pin, HIGH);
		pinMode(pin, OUTPUT);	// TODO: implement OUTPUT_HIGH
		return;
	}

	//serial_print("analogWrite\n");
	//serial_print("val = ");
	//serial_phex32(val);
	//serial_print("\n");
	//serial_print("analog_write_res = ");
	//serial_phex(analog_write_res);
	//serial_print("\n");
	if (pin == FTM1_CH0_PIN || pin == FTM1_CH1_PIN) {
		cval = ((uint32_t)val * (uint32_t)(FTM1_MOD + 1)) >> analog_write_res;
#if defined(FTM2_CH0_PIN)
	} else if (pin == FTM2_CH0_PIN || pin == FTM2_CH1_PIN) {
		cval = ((uint32_t)val * (uint32_t)(FTM2_MOD + 1)) >> analog_write_res;
#endif
#if defined(FTM3_CH0_PIN)
	} else if (pin == FTM3_CH0_PIN || pin == FTM3_CH1_PIN || pin == FTM3_CH2_PIN
	  || pin == FTM3_CH3_PIN || pin == FTM3_CH4_PIN || pin == FTM3_CH5_PIN
	  || pin == FTM3_CH6_PIN || pin == FTM3_CH7_PIN) {
		cval = ((uint32_t)val * (uint32_t)(FTM3_MOD + 1)) >> analog_write_res;
#endif
#if defined(TPM1_CH0_PIN)
	} else if (pin == TPM1_CH0_PIN || pin == TPM1_CH1_PIN) {
		cval = ((uint32_t)val * (uint32_t)(TPM1_MOD + 1)) >> analog_write_res;
#endif
	} else {
		cval = ((uint32_t)val * (uint32_t)(FTM0_MOD + 1)) >> analog_write_res;
	}
	//serial_print("cval = ");
	//serial_phex32(cval);
	//serial_print("\n");
	switch (pin) {
#ifdef FTM0_CH0_PIN
	  case FTM0_CH0_PIN: // PTC1, FTM0_CH0
		FTM0_C0V = cval;
		FTM_PINCFG(FTM0_CH0_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM0_CH1_PIN
	  case FTM0_CH1_PIN: // PTC2, FTM0_CH1
		FTM0_C1V = cval;
		FTM_PINCFG(FTM0_CH1_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM0_CH2_PIN
	  case FTM0_CH2_PIN: // PTC3, FTM0_CH2
		FTM0_C2V = cval;
		FTM_PINCFG(FTM0_CH2_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM0_CH3_PIN
	  case FTM0_CH3_PIN: // PTC4, FTM0_CH3
		FTM0_C3V = cval;
		FTM_PINCFG(FTM0_CH3_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM0_CH4_PIN
	  case FTM0_CH4_PIN: // PTD4, FTM0_CH4
		FTM0_C4V = cval;
		FTM_PINCFG(FTM0_CH4_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM0_CH5_PIN
	  case FTM0_CH5_PIN: // PTD5, FTM0_CH5
		FTM0_C5V = cval;
		FTM_PINCFG(FTM0_CH5_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM0_CH6_PIN
	  case FTM0_CH6_PIN: // PTD6, FTM0_CH6
		FTM0_C6V = cval;
		FTM_PINCFG(FTM0_CH6_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM0_CH7_PIN
	  case FTM0_CH7_PIN: // PTD7, FTM0_CH7
		FTM0_C7V = cval;
		FTM_PINCFG(FTM0_CH7_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM1_CH0_PIN
	  case FTM1_CH0_PIN: // PTA12, FTM1_CH0
		FTM1_C0V = cval;
		FTM_PINCFG(FTM1_CH0_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM1_CH1_PIN
	  case FTM1_CH1_PIN: // PTA13, FTM1_CH1
		FTM1_C1V = cval;
		FTM_PINCFG(FTM1_CH1_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM2_CH0_PIN
	  case FTM2_CH0_PIN: // PTB18, FTM2_CH0
		FTM2_C0V = cval;
		FTM_PINCFG(FTM2_CH0_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM2_CH1_PIN
	  case FTM2_CH1_PIN: // PTB19, FTM1_CH1
		FTM2_C1V = cval;
		FTM_PINCFG(FTM2_CH1_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM3_CH0_PIN
	  case FTM3_CH0_PIN:
		FTM3_C0V = cval;
		FTM_PINCFG(FTM3_CH0_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM3_CH1_PIN
	  case FTM3_CH1_PIN:
		FTM3_C1V = cval;
		FTM_PINCFG(FTM3_CH1_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM3_CH2_PIN
	  case FTM3_CH2_PIN:
		FTM3_C2V = cval;
		FTM_PINCFG(FTM3_CH2_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM3_CH3_PIN
	  case FTM3_CH3_PIN:
		FTM3_C3V = cval;
		FTM_PINCFG(FTM3_CH3_PIN) = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM3_CH4_PIN
	  case FTM3_CH4_PIN:
		FTM3_C4V = cval;
		FTM_PINCFG(FTM3_CH4_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM3_CH5_PIN
	  case FTM3_CH5_PIN:
		FTM3_C5V = cval;
		FTM_PINCFG(FTM3_CH5_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM3_CH6_PIN
	  case FTM3_CH6_PIN:
		FTM3_C6V = cval;
		FTM_PINCFG(FTM3_CH6_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef FTM3_CH7_PIN
	  case FTM3_CH7_PIN:
		FTM3_C7V = cval;
		FTM_PINCFG(FTM3_CH7_PIN) = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef TPM1_CH0_PIN
	  case TPM1_CH0_PIN:
		TPM1_C0V = cval;
		FTM_PINCFG(TPM1_CH0_PIN) = PORT_PCR_MUX(6) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
#ifdef TPM1_CH1_PIN
	  case TPM1_CH1_PIN:
		TPM1_C1V = cval;
		FTM_PINCFG(TPM1_CH1_PIN) = PORT_PCR_MUX(6) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#endif
	  default:
		digitalWrite(pin, (val > 127) ? HIGH : LOW);
		pinMode(pin, OUTPUT);
	}
}


void analogWriteRes(uint32_t bits)
{
	if (bits < 1) {
		bits = 1;
	} else if (bits > 16) {
		bits = 16;
	}
	analog_write_res = bits;
}


void analogWriteFrequency(uint8_t pin, float frequency)
{
	uint32_t prescale, mod, ftmClock, ftmClockSource;
	float minfreq;

	//serial_print("analogWriteFrequency: pin = ");
	//serial_phex(pin);
	//serial_print(", freq = ");
	//serial_phex32((uint32_t)frequency);
	//serial_print("\n");

#ifdef TPM1_CH0_PIN
	if (pin == TPM1_CH0_PIN || pin == TPM1_CH1_PIN) {
		ftmClockSource = 1;
		ftmClock = 16000000;
	} else
#endif
	if (frequency < (float)(F_TIMER >> 7) / 65536.0f) {
		// frequency is too low for working with F_TIMER:
		ftmClockSource = 2; 	// Use alternative 31250Hz clock source
		ftmClock = 31250;   	// Set variable for the actual timer clock frequency
	} else {
		ftmClockSource = 1; 	// Use default F_TIMER clock source
		ftmClock = F_TIMER;	// Set variable for the actual timer clock frequency
	}

	
	for (prescale = 0; prescale < 7; prescale++) {
		minfreq = (float)(ftmClock >> prescale) / 65536.0f;	//Use ftmClock instead of F_TIMER
		if (frequency >= minfreq) break;
	}
	//serial_print("F_TIMER/ftm_Clock = ");
	//serial_phex32(ftmClock >> prescale);
	//serial_print("\n");
	//serial_print("prescale = ");
	//serial_phex(prescale);
	//serial_print("\n");
	mod = (float)(ftmClock >> prescale) / frequency - 0.5f;	//Use ftmClock instead of F_TIMER
	if (mod > 65535) mod = 65535;
	//serial_print("mod = ");
	//serial_phex32(mod);
	//serial_print("\n");
	if (pin == FTM1_CH0_PIN || pin == FTM1_CH1_PIN) {
		FTM1_SC = 0;
		FTM1_CNT = 0;
		FTM1_MOD = mod;
		FTM1_SC = FTM_SC_CLKS(ftmClockSource) | FTM_SC_PS(prescale);	//Use ftmClockSource instead of 1
	} else if (pin == FTM0_CH0_PIN || pin == FTM0_CH1_PIN
	  || pin == FTM0_CH2_PIN || pin == FTM0_CH3_PIN
	  || pin == FTM0_CH4_PIN || pin == FTM0_CH5_PIN
#ifdef FTM0_CH6_PIN
	  || pin == FTM0_CH6_PIN || pin == FTM0_CH7_PIN
#endif
	  ) {
		FTM0_SC = 0;
		FTM0_CNT = 0;
		FTM0_MOD = mod;
		FTM0_SC = FTM_SC_CLKS(ftmClockSource) | FTM_SC_PS(prescale);	//Use ftmClockSource instead of 1
	}
#ifdef FTM2_CH0_PIN
	  else if (pin == FTM2_CH0_PIN || pin == FTM2_CH1_PIN) {
		FTM2_SC = 0;
		FTM2_CNT = 0;
		FTM2_MOD = mod;
		FTM2_SC = FTM_SC_CLKS(ftmClockSource) | FTM_SC_PS(prescale);	//Use ftmClockSource instead of 1
	}
#endif
#ifdef FTM3_CH0_PIN
	  else if (pin == FTM3_CH0_PIN || pin == FTM3_CH1_PIN
	  || pin == FTM3_CH2_PIN || pin == FTM3_CH3_PIN
	  || pin == FTM3_CH4_PIN || pin == FTM3_CH5_PIN
	  || pin == FTM3_CH6_PIN || pin == FTM3_CH7_PIN) {
		FTM3_SC = 0;
		FTM3_CNT = 0;
		FTM3_MOD = mod;
		FTM3_SC = FTM_SC_CLKS(ftmClockSource) | FTM_SC_PS(prescale);	//Use the new ftmClockSource instead of 1
	}
#endif
#ifdef TPM1_CH0_PIN
	  else if (pin == TPM1_CH0_PIN || pin == TPM1_CH1_PIN) {
		TPM1_SC = 0;
		TPM1_CNT = 0;
		TPM1_MOD = mod;
		TPM1_SC = FTM_SC_CLKS(ftmClockSource) | FTM_SC_PS(prescale);
	}
#endif
}



// TODO: startup code needs to initialize all pins to GPIO mode, input by default

void digitalWrite(uint8_t pin, uint8_t val)
{
	if (pin >= CORE_NUM_DIGITAL) return;
#ifdef KINETISK
	if (*portModeRegister(pin)) {
		if (val) {
			*portSetRegister(pin) = 1;
		} else {
			*portClearRegister(pin) = 1;
		}
#else
	if (*portModeRegister(pin) & digitalPinToBitMask(pin)) {
		if (val) {
			*portSetRegister(pin) = digitalPinToBitMask(pin);
		} else {
			*portClearRegister(pin) = digitalPinToBitMask(pin);
		}
#endif
	} else {
		volatile uint32_t *config = portConfigRegister(pin);
		if (val) {
			// TODO use bitband for atomic read-mod-write
			*config |= (PORT_PCR_PE | PORT_PCR_PS);
			//*config = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS;
		} else {
			// TODO use bitband for atomic read-mod-write
			*config &= ~(PORT_PCR_PE);
			//*config = PORT_PCR_MUX(1);
		}
	}

}

uint8_t digitalRead(uint8_t pin)
{
	if (pin >= CORE_NUM_DIGITAL) return 0;
#ifdef KINETISK
	return *portInputRegister(pin);
#else
	return (*portInputRegister(pin) & digitalPinToBitMask(pin)) ? 1 : 0;
#endif
}



void pinMode(uint8_t pin, uint8_t mode)
{
	volatile uint32_t *config;

	if (pin >= CORE_NUM_DIGITAL) return;
	config = portConfigRegister(pin);

	if (mode == OUTPUT || mode == OUTPUT_OPENDRAIN) {
#ifdef KINETISK
		*portModeRegister(pin) = 1;
#else
		*portModeRegister(pin) |= digitalPinToBitMask(pin); // TODO: atomic
#endif
		*config = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
		if (mode == OUTPUT_OPENDRAIN) {
		    *config |= PORT_PCR_ODE;
		} else {
		    *config &= ~PORT_PCR_ODE;
                }
	} else {
#ifdef KINETISK
		*portModeRegister(pin) = 0;
#else
		*portModeRegister(pin) &= ~digitalPinToBitMask(pin);
#endif
		if (mode == INPUT || mode == INPUT_PULLUP || mode == INPUT_PULLDOWN) {
			*config = PORT_PCR_MUX(1);
			if (mode == INPUT_PULLUP) {
		    	*config |= (PORT_PCR_PE | PORT_PCR_PS); // pullup
			} else if (mode == INPUT_PULLDOWN) {
			    *config |= (PORT_PCR_PE); // pulldown
			    *config &= ~(PORT_PCR_PS);
			}
		} else {
			*config = PORT_PCR_MUX(1) | PORT_PCR_PE | PORT_PCR_PS; // pullup
		}
	}

}


void _shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value)
{
        if (bitOrder == LSBFIRST) {
                shiftOut_lsbFirst(dataPin, clockPin, value);
        } else {
                shiftOut_msbFirst(dataPin, clockPin, value);
        }
}

void shiftOut_lsbFirst(uint8_t dataPin, uint8_t clockPin, uint8_t value)
{
        uint8_t mask;
        for (mask=0x01; mask; mask <<= 1) {
                digitalWrite(dataPin, value & mask);
                digitalWrite(clockPin, HIGH);
                digitalWrite(clockPin, LOW);
        }
}

void shiftOut_msbFirst(uint8_t dataPin, uint8_t clockPin, uint8_t value)
{
        uint8_t mask;
        for (mask=0x80; mask; mask >>= 1) {
                digitalWrite(dataPin, value & mask);
                digitalWrite(clockPin, HIGH);
                digitalWrite(clockPin, LOW);
        }
}

uint8_t _shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder)
{
        if (bitOrder == LSBFIRST) {
                return shiftIn_lsbFirst(dataPin, clockPin);
        } else {
                return shiftIn_msbFirst(dataPin, clockPin);
        }
}

uint8_t shiftIn_lsbFirst(uint8_t dataPin, uint8_t clockPin)
{
        uint8_t mask, value=0;
        for (mask=0x01; mask; mask <<= 1) {
                digitalWrite(clockPin, HIGH);
                if (digitalRead(dataPin)) value |= mask;
                digitalWrite(clockPin, LOW);
        }
        return value;
}

uint8_t shiftIn_msbFirst(uint8_t dataPin, uint8_t clockPin)
{
        uint8_t mask, value=0;
        for (mask=0x80; mask; mask >>= 1) {
                digitalWrite(clockPin, HIGH);
                if (digitalRead(dataPin)) value |= mask;
                digitalWrite(clockPin, LOW);
        }
        return value;
}



// the systick interrupt is supposed to increment this at 1 kHz rate
volatile uint32_t systick_millis_count = 0;

//uint32_t systick_current, systick_count, systick_istatus;  // testing only

uint32_t micros(void)
{
	uint32_t count, current, istatus;

	__disable_irq();
	current = SYST_CVR;
	count = systick_millis_count;
	istatus = SCB_ICSR;	// bit 26 indicates if systick exception pending
	__enable_irq();
	 //systick_current = current;
	 //systick_count = count;
	 //systick_istatus = istatus & SCB_ICSR_PENDSTSET ? 1 : 0;
	if ((istatus & SCB_ICSR_PENDSTSET) && current > 50) count++;
	current = ((F_CPU / 1000) - 1) - current;
#if defined(KINETISL) && F_CPU == 48000000
	return count * 1000 + ((current * (uint32_t)87381) >> 22);
#elif defined(KINETISL) && F_CPU == 24000000
	return count * 1000 + ((current * (uint32_t)174763) >> 22);
#endif
	return count * 1000 + current / (F_CPU / 1000000);
}

void delay(uint32_t ms)
{
	uint32_t start = micros();

	if (ms > 0) {
		while (1) {
			while ((micros() - start) >= 1000) {
				ms--;
				if (ms == 0) return;
				start += 1000;
			}
			yield();
		}
	}
}

// TODO: verify these result in correct timeouts...
#if F_CPU == 240000000
#define PULSEIN_LOOPS_PER_USEC 33
#elif F_CPU == 216000000
#define PULSEIN_LOOPS_PER_USEC 31
#elif F_CPU == 192000000
#define PULSEIN_LOOPS_PER_USEC 29
#elif F_CPU == 180000000
#define PULSEIN_LOOPS_PER_USEC 27
#elif F_CPU == 168000000
#define PULSEIN_LOOPS_PER_USEC 25
#elif F_CPU == 144000000
#define PULSEIN_LOOPS_PER_USEC 21
#elif F_CPU == 120000000
#define PULSEIN_LOOPS_PER_USEC 18
#elif F_CPU == 96000000
#define PULSEIN_LOOPS_PER_USEC 14
#elif F_CPU == 72000000
#define PULSEIN_LOOPS_PER_USEC 10
#elif F_CPU == 48000000
#define PULSEIN_LOOPS_PER_USEC 7
#elif F_CPU == 24000000
#define PULSEIN_LOOPS_PER_USEC 4
#elif F_CPU == 16000000
#define PULSEIN_LOOPS_PER_USEC 1
#elif F_CPU == 8000000
#define PULSEIN_LOOPS_PER_USEC 1
#elif F_CPU == 4000000
#define PULSEIN_LOOPS_PER_USEC 1
#elif F_CPU == 2000000
#define PULSEIN_LOOPS_PER_USEC 1
#endif

#if defined(KINETISK)
uint32_t pulseIn_high(volatile uint8_t *reg, uint32_t timeout)
{
	uint32_t timeout_count = timeout * PULSEIN_LOOPS_PER_USEC;
	uint32_t usec_start, usec_stop;

	// wait for any previous pulse to end
	while (*reg) {
		if (--timeout_count == 0) return 0;
	}
	// wait for the pulse to start
	while (!*reg) {
		if (--timeout_count == 0) return 0;
	}
	usec_start = micros();
	// wait for the pulse to stop
	while (*reg) {
		if (--timeout_count == 0) return 0;
	}
	usec_stop = micros();
	return usec_stop - usec_start;
}

uint32_t pulseIn_low(volatile uint8_t *reg, uint32_t timeout)
{
	uint32_t timeout_count = timeout * PULSEIN_LOOPS_PER_USEC;
	uint32_t usec_start, usec_stop;
	
	// wait for any previous pulse to end
	while (!*reg) {
		if (--timeout_count == 0) return 0;
	}
	// wait for the pulse to start
	while (*reg) {
		if (--timeout_count == 0) return 0;
	}
	usec_start = micros();
	// wait for the pulse to stop
	while (!*reg) {
		if (--timeout_count == 0) return 0;
	}
	usec_stop = micros();
	return usec_stop - usec_start;
}

// TODO: an inline version should handle the common case where state is const
uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout)
{
	if (pin >= CORE_NUM_DIGITAL) return 0;
	if (state) return pulseIn_high(portInputRegister(pin), timeout);
	return pulseIn_low(portInputRegister(pin), timeout);;
}

#elif defined(KINETISL)
// For TeencyLC need to use mask on the input register as the register is shared by several IO pins
uint32_t pulseIn_high(volatile uint8_t *reg, uint8_t mask, uint32_t timeout)
{
	uint32_t timeout_count = timeout * PULSEIN_LOOPS_PER_USEC;
	uint32_t usec_start, usec_stop;
	// wait for any previous pulse to end
	while (*reg & mask) {
		if (--timeout_count == 0) return -1;
	}
	// wait for the pulse to start
	while (!(*reg & mask)) {
		if (--timeout_count == 0) return 0;
	}
	usec_start = micros();
	// wait for the pulse to stop
	while (*reg & mask) {
		if (--timeout_count == 0) return 0;
	}
	usec_stop = micros();
	return usec_stop - usec_start;
}

uint32_t pulseIn_low(volatile uint8_t *reg, uint8_t mask, uint32_t timeout)
{
	uint32_t timeout_count = timeout * PULSEIN_LOOPS_PER_USEC;
	uint32_t usec_start, usec_stop;
	
	// wait for any previous pulse to end
	while (!(*reg & mask)) {
		if (--timeout_count == 0) return 0;
	}
	// wait for the pulse to start
	while (*reg & mask) {
		if (--timeout_count == 0) return 0;
	}
	usec_start = micros();
	// wait for the pulse to stop
	while (!(*reg & mask)) {
		if (--timeout_count == 0) return 0;
	}
	usec_stop = micros();
	return usec_stop - usec_start;
}

// TODO: an inline version should handle the common case where state is const
uint32_t pulseIn(uint8_t pin, uint8_t state, uint32_t timeout)
{
	if (pin >= CORE_NUM_DIGITAL) return 0;
	if (state) return pulseIn_high(portInputRegister(pin), digitalPinToBitMask(pin), timeout);
	return pulseIn_low(portInputRegister(pin), digitalPinToBitMask(pin), timeout);;
}
#endif
