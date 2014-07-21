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
//#include "HardwareSerial.h"

static uint8_t calibrating;
static uint8_t analog_right_shift = 0;
static uint8_t analog_config_bits = 10;
static uint8_t analog_num_average = 4;
static uint8_t analog_reference_internal = 0;

// the alternate clock is connected to OSCERCLK (16 MHz).
// datasheet says ADC clock should be 2 to 12 MHz for 16 bit mode
// datasheet says ADC clock should be 1 to 18 MHz for 8-12 bit mode

#if F_BUS == 60000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1) // 7.5 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 15 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 15 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 15 MHz
#elif F_BUS == 56000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1) // 7 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 14 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 14 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 14 MHz
#elif F_BUS == 48000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 12 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 12 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 12 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1) // 24 MHz
#elif F_BUS == 40000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 10 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 10 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 10 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1) // 20 MHz
#elif F_BUS == 36000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // 9 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1) // 18 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1) // 18 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1) // 18 MHz
#elif F_BUS == 24000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0) // 12 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0) // 12 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0) // 12 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 24 MHz
#elif F_BUS == 16000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 8 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 8 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 8 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 16 MHz
#elif F_BUS == 8000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 8 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 8 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 8 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 8 MHz
#elif F_BUS == 4000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 4 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 4 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 4 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 4 MHz
#elif F_BUS == 2000000
  #define ADC_CFG1_16BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 2 MHz
  #define ADC_CFG1_12BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 2 MHz
  #define ADC_CFG1_10BIT  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 2 MHz
  #define ADC_CFG1_8BIT   ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // 2 MHz
#else
#error "F_BUS must be 60, 56, 48, 40, 36, 24, 4 or 2 MHz"
#endif

void analog_init(void)
{
	uint32_t num;

	VREF_TRM = 0x60;
	VREF_SC = 0xE1;		// enable 1.2 volt ref

	if (analog_config_bits == 8) {
		ADC0_CFG1 = ADC_CFG1_8BIT + ADC_CFG1_MODE(0);
		ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3);
		#if defined(__MK20DX256__)
		ADC1_CFG1 = ADC_CFG1_8BIT + ADC_CFG1_MODE(0);
		ADC1_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3);
		#endif
	} else if (analog_config_bits == 10) {
		ADC0_CFG1 = ADC_CFG1_10BIT + ADC_CFG1_MODE(2) + ADC_CFG1_ADLSMP;
		ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3);
		#if defined(__MK20DX256__)
		ADC1_CFG1 = ADC_CFG1_10BIT + ADC_CFG1_MODE(2) + ADC_CFG1_ADLSMP;
		ADC1_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3);
		#endif
	} else if (analog_config_bits == 12) {
		ADC0_CFG1 = ADC_CFG1_12BIT + ADC_CFG1_MODE(1) + ADC_CFG1_ADLSMP;
		ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2);
		#if defined(__MK20DX256__)
		ADC1_CFG1 = ADC_CFG1_12BIT + ADC_CFG1_MODE(1) + ADC_CFG1_ADLSMP;
		ADC1_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2);
		#endif
	} else {
		ADC0_CFG1 = ADC_CFG1_16BIT + ADC_CFG1_MODE(3) + ADC_CFG1_ADLSMP;
		ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2);
		#if defined(__MK20DX256__)
		ADC1_CFG1 = ADC_CFG1_16BIT + ADC_CFG1_MODE(3) + ADC_CFG1_ADLSMP;
		ADC1_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2);
		#endif
	}

	if (analog_reference_internal) {
		ADC0_SC2 = ADC_SC2_REFSEL(1); // 1.2V ref
		#if defined(__MK20DX256__)
		ADC1_SC2 = ADC_SC2_REFSEL(1); // 1.2V ref
		#endif
	} else {
		ADC0_SC2 = ADC_SC2_REFSEL(0); // vcc/ext ref
		#if defined(__MK20DX256__)
		ADC1_SC2 = ADC_SC2_REFSEL(0); // vcc/ext ref
		#endif
	}

	num = analog_num_average;
	if (num <= 1) {
		ADC0_SC3 = ADC_SC3_CAL;  // begin cal
		#if defined(__MK20DX256__)
		ADC1_SC3 = ADC_SC3_CAL;  // begin cal
		#endif
	} else if (num <= 4) {
		ADC0_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(0);
		#if defined(__MK20DX256__)
		ADC1_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(0);
		#endif
	} else if (num <= 8) {
		ADC0_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(1);
		#if defined(__MK20DX256__)
		ADC1_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(1);
		#endif
	} else if (num <= 16) {
		ADC0_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(2);
		#if defined(__MK20DX256__)
		ADC1_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(2);
		#endif
	} else {
		ADC0_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(3);
		#if defined(__MK20DX256__)
		ADC1_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(3);
		#endif
	}
	calibrating = 1;
}

static void wait_for_cal(void)
{
	uint16_t sum;

	//serial_print("wait_for_cal\n");
#if defined(__MK20DX128__)
	while (ADC0_SC3 & ADC_SC3_CAL) {
		// wait
	}
#elif defined(__MK20DX256__)
	while ((ADC0_SC3 & ADC_SC3_CAL) || (ADC1_SC3 & ADC_SC3_CAL)) {
		// wait
	}
#endif
	__disable_irq();
	if (calibrating) {
		//serial_print("\n");
		sum = ADC0_CLPS + ADC0_CLP4 + ADC0_CLP3 + ADC0_CLP2 + ADC0_CLP1 + ADC0_CLP0;
		sum = (sum / 2) | 0x8000;
		ADC0_PG = sum;
		//serial_print("ADC0_PG = ");
		//serial_phex16(sum);
		//serial_print("\n");
		sum = ADC0_CLMS + ADC0_CLM4 + ADC0_CLM3 + ADC0_CLM2 + ADC0_CLM1 + ADC0_CLM0;
		sum = (sum / 2) | 0x8000;
		ADC0_MG = sum;
		//serial_print("ADC0_MG = ");
		//serial_phex16(sum);
		//serial_print("\n");
#if defined(__MK20DX256__)
		sum = ADC1_CLPS + ADC1_CLP4 + ADC1_CLP3 + ADC1_CLP2 + ADC1_CLP1 + ADC1_CLP0;
		sum = (sum / 2) | 0x8000;
		ADC1_PG = sum;
		sum = ADC1_CLMS + ADC1_CLM4 + ADC1_CLM3 + ADC1_CLM2 + ADC1_CLM1 + ADC1_CLM0;
		sum = (sum / 2) | 0x8000;
		ADC1_MG = sum;
#endif
		calibrating = 0;
	}
	__enable_irq();
}

// ADCx_SC2[REFSEL] bit selects the voltage reference sources for ADC.
//   VREFH/VREFL - connected as the primary reference option
//   1.2 V VREF_OUT - connected as the VALT reference option


#define DEFAULT         0
#define INTERNAL        2
#define INTERNAL1V2     2
#define INTERNAL1V1     2
#define EXTERNAL        0

void analogReference(uint8_t type)
{
	if (type) {
		// internal reference requested
		if (!analog_reference_internal) {
			analog_reference_internal = 1;
			if (calibrating) {
				ADC0_SC3 = 0; // cancel cal
#if defined(__MK20DX256__)
				ADC1_SC3 = 0; // cancel cal
#endif
			}
			analog_init();
		}
	} else {
		// vcc or external reference requested
		if (analog_reference_internal) {
			analog_reference_internal = 0;
			if (calibrating) {
				ADC0_SC3 = 0; // cancel cal
#if defined(__MK20DX256__)
				ADC1_SC3 = 0; // cancel cal
#endif
			}
			analog_init();
		}
	}
}


void analogReadRes(unsigned int bits)
{
	unsigned int config;

	if (bits >= 13) {
		if (bits > 16) bits = 16;
		config = 16;
	} else if (bits >= 11) {
		config = 12;
	} else if (bits >= 9) {
		config = 10;
	} else {
		config = 8;
	}
	analog_right_shift = config - bits;
	if (config != analog_config_bits) {
		analog_config_bits = config;
		if (calibrating) ADC0_SC3 = 0; // cancel cal
		analog_init();
	}
}

void analogReadAveraging(unsigned int num)
{

	if (calibrating) wait_for_cal();
	if (num <= 1) {
		num = 0;
		ADC0_SC3 = 0;
	} else if (num <= 4) {
		num = 4;
		ADC0_SC3 = ADC_SC3_AVGE + ADC_SC3_AVGS(0);
	} else if (num <= 8) {
		num = 8;
		ADC0_SC3 = ADC_SC3_AVGE + ADC_SC3_AVGS(1);
	} else if (num <= 16) {
		num = 16;
		ADC0_SC3 = ADC_SC3_AVGE + ADC_SC3_AVGS(2);
	} else {
		num = 32;
		ADC0_SC3 = ADC_SC3_AVGE + ADC_SC3_AVGS(3);
	}
	analog_num_average = num;
}

// The SC1A register is used for both software and hardware trigger modes of operation.

#if defined(__MK20DX128__)
static const uint8_t channel2sc1a[] = {
	5, 14, 8, 9, 13, 12, 6, 7, 15, 4,
	0, 19, 3, 21, 26, 22, 23
};
#elif defined(__MK20DX256__)
static const uint8_t channel2sc1a[] = {
	5, 14, 8, 9, 13, 12, 6, 7, 15, 4,
	0, 19, 3, 19+128, 26, 18+128, 23,
	5+192, 5+128, 4+128, 6+128, 7+128, 4+192
// A15  26   E1   ADC1_SE5a  5+64
// A16  27   C9   ADC1_SE5b  5
// A17  28   C8   ADC1_SE4b  4
// A18  29   C10  ADC1_SE6b  6
// A19  30   C11  ADC1_SE7b  7
// A20  31   E0   ADC1_SE4a  4+64
};
#endif



// TODO: perhaps this should store the NVIC priority, so it works recursively?
static volatile uint8_t analogReadBusyADC0 = 0;
#if defined(__MK20DX256__)
static volatile uint8_t analogReadBusyADC1 = 0;
#endif

int analogRead(uint8_t pin)
{
	int result;
	uint8_t index, channel;

	//serial_phex(pin);
	//serial_print(" ");

	if (pin <= 13) {
		index = pin;      // 0-13 refer to A0-A13
	} else if (pin <= 23) {
		index = pin - 14; // 14-23 are A0-A9
#if defined(__MK20DX256__)
	} else if (pin >= 26 && pin <= 31) {
		index = pin - 9;  // 26-31 are A15-A20
#endif
	} else if (pin >= 34 && pin <= 40) {
		index = pin - 24;  // 34-37 are A10-A13, 38 is temp sensor,
			    // 39 is vref, 40 is unused (A14 on Teensy 3.1)
	} else {
		return 0;   // all others are invalid
	}

	//serial_phex(index);
	//serial_print(" ");

	channel = channel2sc1a[index];
	//serial_phex(channel);
	//serial_print(" ");

	//serial_print("analogRead");
	//return 0;
	if (calibrating) wait_for_cal();
	//pin = 5; // PTD1/SE5b, pin 14, analog 0

#if defined(__MK20DX256__)
	if (channel & 0x80) goto beginADC1;
#endif

	__disable_irq();
startADC0:
	//serial_print("startADC0\n");
	ADC0_SC1A = channel;
	analogReadBusyADC0 = 1;
	__enable_irq();
	while (1) {
		__disable_irq();
		if ((ADC0_SC1A & ADC_SC1_COCO)) {
			result = ADC0_RA;
			analogReadBusyADC0 = 0;
			__enable_irq();
			result >>= analog_right_shift;
			return result;
		}
		// detect if analogRead was used from an interrupt
		// if so, our analogRead got canceled, so it must
		// be restarted.
		if (!analogReadBusyADC0) goto startADC0;
		__enable_irq();
		yield();
	}

#if defined(__MK20DX256__)
beginADC1:
	__disable_irq();
startADC1:
	//serial_print("startADC0\n");
	// ADC1_CFG2[MUXSEL] bit selects between ADCx_SEn channels a and b.
	if (channel & 0x40) {
		ADC1_CFG2 &= ~ADC_CFG2_MUXSEL;
	} else {
		ADC1_CFG2 |= ADC_CFG2_MUXSEL;
	}
	ADC1_SC1A = channel & 0x3F;
	analogReadBusyADC1 = 1;
	__enable_irq();
	while (1) {
		__disable_irq();
		if ((ADC1_SC1A & ADC_SC1_COCO)) {
			result = ADC1_RA;
			analogReadBusyADC1 = 0;
			__enable_irq();
			result >>= analog_right_shift;
			return result;
		}
		// detect if analogRead was used from an interrupt
		// if so, our analogRead got canceled, so it must
		// be restarted.
		if (!analogReadBusyADC1) goto startADC1;
		__enable_irq();
		yield();
	}
#endif
}



void analogWriteDAC0(int val)
{
#if defined(__MK20DX256__)
	SIM_SCGC2 |= SIM_SCGC2_DAC0;
	if (analog_reference_internal) {
		DAC0_C0 = DAC_C0_DACEN;  // 1.2V ref is DACREF_1
	} else {
		DAC0_C0 = DAC_C0_DACEN | DAC_C0_DACRFS; // 3.3V VDDA is DACREF_2
	}
	if (val < 0) val = 0;  // TODO: saturate instruction?
	else if (val > 4095) val = 4095;
	*(int16_t *)&(DAC0_DAT0L) = val;
#endif
}



















