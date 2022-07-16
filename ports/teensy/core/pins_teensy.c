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

#if 0
// moved to pins_arduino.h
struct digital_pin_bitband_and_config_table_struct {
	volatile uint32_t *reg;
	volatile uint32_t *config;
};
const struct digital_pin_bitband_and_config_table_struct digital_pin_to_info_PGM[];

// compatibility macros
#define digitalPinToPort(pin) (pin)
#define digitalPinToBitMask(pin) (1)
#define portOutputRegister(pin) ((volatile uint8_t *)(digital_pin_to_info_PGM[(pin)].reg + 0))
#define portSetRegister(pin)    ((volatile uint8_t *)(digital_pin_to_info_PGM[(pin)].reg + 32))
#define portClearRegister(pin)  ((volatile uint8_t *)(digital_pin_to_info_PGM[(pin)].reg + 64))
#define portToggleRegister(pin) ((volatile uint8_t *)(digital_pin_to_info_PGM[(pin)].reg + 96))
#define portInputRegister(pin)  ((volatile uint8_t *)(digital_pin_to_info_PGM[(pin)].reg + 128))
#define portModeRegister(pin)   ((volatile uint8_t *)(digital_pin_to_info_PGM[(pin)].reg + 160))
#define portConfigRegister(pin) ((volatile uint32_t *)(digital_pin_to_info_PGM[(pin)].config))
#endif

//#define digitalPinToTimer(P) ( pgm_read_byte( digital_pin_to_timer_PGM + (P) ) )
//#define analogInPinToBit(P) (P)

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
	{GPIO_BITBAND_PTR(CORE_PIN33_PORTREG, CORE_PIN33_BIT), &CORE_PIN33_CONFIG}
};




typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr intFunc[CORE_NUM_DIGITAL];

void init_pin_interrupts(void)
{
	//SIM_SCGC5 = 0x00043F82;		// clocks active to all GPIO
	NVIC_ENABLE_IRQ(IRQ_PORTA);
	NVIC_ENABLE_IRQ(IRQ_PORTB);
	NVIC_ENABLE_IRQ(IRQ_PORTC);
	NVIC_ENABLE_IRQ(IRQ_PORTD);
	NVIC_ENABLE_IRQ(IRQ_PORTE);
	// TODO: maybe these should be set to a lower priority
	// so if the user puts lots of slow code on attachInterrupt
	// fast interrupts will still be serviced quickly?
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


void porta_isr(void)
{
	uint32_t isfr = PORTA_ISFR;
	PORTA_ISFR = isfr;
	if ((isfr & CORE_PIN3_BITMASK) && intFunc[3]) intFunc[3]();
	if ((isfr & CORE_PIN4_BITMASK) && intFunc[4]) intFunc[4]();
	if ((isfr & CORE_PIN24_BITMASK) && intFunc[24]) intFunc[24]();
	if ((isfr & CORE_PIN33_BITMASK) && intFunc[33]) intFunc[33]();
}

void portb_isr(void)
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

void portc_isr(void)
{
	// TODO: these are inefficient.  Use CLZ somehow....
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

void portd_isr(void)
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

void porte_isr(void)
{
	uint32_t isfr = PORTE_ISFR;
	PORTE_ISFR = isfr;
	if ((isfr & CORE_PIN26_BITMASK) && intFunc[26]) intFunc[26]();
	if ((isfr & CORE_PIN31_BITMASK) && intFunc[31]) intFunc[31]();
}




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

#if F_BUS == 60000000
#define DEFAULT_FTM_MOD (61440 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_BUS == 56000000
#define DEFAULT_FTM_MOD (57344 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_BUS == 48000000
#define DEFAULT_FTM_MOD (49152 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_BUS == 40000000
#define DEFAULT_FTM_MOD (40960 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_BUS == 36000000
#define DEFAULT_FTM_MOD (36864 - 1)
#define DEFAULT_FTM_PRESCALE 1
#elif F_BUS == 24000000
#define DEFAULT_FTM_MOD (49152 - 1)
#define DEFAULT_FTM_PRESCALE 0
#elif F_BUS == 16000000
#define DEFAULT_FTM_MOD (32768 - 1)
#define DEFAULT_FTM_PRESCALE 0
#elif F_BUS == 8000000
#define DEFAULT_FTM_MOD (16384 - 1)
#define DEFAULT_FTM_PRESCALE 0
#elif F_BUS == 4000000
#define DEFAULT_FTM_MOD (8192 - 1)
#define DEFAULT_FTM_PRESCALE 0
#elif F_BUS == 2000000
#define DEFAULT_FTM_MOD (4096 - 1)
#define DEFAULT_FTM_PRESCALE 0
#endif

//void init_pins(void)
void _init_Teensyduino_internal_(void)
{
	init_pin_interrupts();

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
	FTM0_C6SC = 0x28;
	FTM0_C7SC = 0x28;
	FTM0_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
	FTM1_CNT = 0;
	FTM1_MOD = DEFAULT_FTM_MOD;
	FTM1_C0SC = 0x28;
	FTM1_C1SC = 0x28;
	FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
#if defined(__MK20DX256__)
	FTM2_CNT = 0;
	FTM2_MOD = DEFAULT_FTM_MOD;
	FTM2_C0SC = 0x28;
	FTM2_C1SC = 0x28;
	FTM2_SC = FTM_SC_CLKS(1) | FTM_SC_PS(DEFAULT_FTM_PRESCALE);
#endif

	analog_init();
	//delay(100); // TODO: this is not necessary, right?
	delay(4);
	usb_init();
}



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
	if (pin == 3 || pin == 4) {
		cval = ((uint32_t)val * (uint32_t)(FTM1_MOD + 1)) >> analog_write_res;
#if defined(__MK20DX256__)
	} else if (pin == 25 || pin == 32) {
		cval = ((uint32_t)val * (uint32_t)(FTM2_MOD + 1)) >> analog_write_res;
#endif
	} else {
		cval = ((uint32_t)val * (uint32_t)(FTM0_MOD + 1)) >> analog_write_res;
	}
	//serial_print("cval = ");
	//serial_phex32(cval);
	//serial_print("\n");
	switch (pin) {
	  case 3: // PTA12, FTM1_CH0
		FTM1_C0V = cval;
		CORE_PIN3_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 4: // PTA13, FTM1_CH1
		FTM1_C1V = cval;
		CORE_PIN4_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 5: // PTD7, FTM0_CH7
		FTM0_C7V = cval;
		CORE_PIN5_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 6: // PTD4, FTM0_CH4
		FTM0_C4V = cval;
		CORE_PIN6_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 9: // PTC3, FTM0_CH2
		FTM0_C2V = cval;
		CORE_PIN9_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 10: // PTC4, FTM0_CH3
		FTM0_C3V = cval;
		CORE_PIN10_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 20: // PTD5, FTM0_CH5
		FTM0_C5V = cval;
		CORE_PIN20_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 21: // PTD6, FTM0_CH6
		FTM0_C6V = cval;
		CORE_PIN21_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 22: // PTC1, FTM0_CH0
		FTM0_C0V = cval;
		CORE_PIN22_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 23: // PTC2, FTM0_CH1
		FTM0_C1V = cval;
		CORE_PIN23_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
#if defined(__MK20DX256__)
	  case 32: // PTB18, FTM2_CH0
		FTM2_C0V = cval;
		CORE_PIN32_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
		break;
	  case 25: // PTB19, FTM1_CH1
		FTM2_C1V = cval;
		CORE_PIN25_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
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

void analogWriteFrequency(uint8_t pin, uint32_t frequency)
{
	uint32_t minfreq, prescale, mod;

	//serial_print("analogWriteFrequency: pin = ");
	//serial_phex(pin);
	//serial_print(", freq = ");
	//serial_phex32(frequency);
	//serial_print("\n");
	for (prescale = 0; prescale < 7; prescale++) {
		minfreq = (F_BUS >> 16) >> prescale;
		if (frequency > minfreq) break;
	}
	//serial_print("F_BUS = ");
	//serial_phex32(F_BUS >> prescale);
	//serial_print("\n");
	//serial_print("prescale = ");
	//serial_phex(prescale);
	//serial_print("\n");
	//mod = ((F_BUS >> prescale) / frequency) - 1;
	mod = (((F_BUS >> prescale) + (frequency >> 1)) / frequency) - 1;
	if (mod > 65535) mod = 65535;
	//serial_print("mod = ");
	//serial_phex32(mod);
	//serial_print("\n");
	if (pin == 3 || pin == 4) {
		FTM1_SC = 0;
		FTM1_CNT = 0;
		FTM1_MOD = mod;
		FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_PS(prescale);
	} else if (pin == 5 || pin == 6 || pin == 9 || pin == 10 ||
	  (pin >= 20 && pin <= 23)) {
		FTM0_SC = 0;
		FTM0_CNT = 0;
		FTM0_MOD = mod;
		FTM0_SC = FTM_SC_CLKS(1) | FTM_SC_PS(prescale);
	}
}




// TODO: startup code needs to initialize all pins to GPIO mode, input by default

void digitalWrite(uint8_t pin, uint8_t val)
{
	if (pin >= CORE_NUM_DIGITAL) return;
	if (*portModeRegister(pin)) {
		if (val) {
			*portSetRegister(pin) = 1;
		} else {
			*portClearRegister(pin) = 1;
		}
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
	return *portInputRegister(pin);
}



void pinMode(uint8_t pin, uint8_t mode)
{
	volatile uint32_t *config;

	if (pin >= CORE_NUM_DIGITAL) return;
	config = portConfigRegister(pin);

	if (mode == OUTPUT) {
		*portModeRegister(pin) = 1;
		*config = PORT_PCR_SRE | PORT_PCR_DSE | PORT_PCR_MUX(1);
	} else {
		*portModeRegister(pin) = 0;
		if (mode == INPUT) {
			*config = PORT_PCR_MUX(1);
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
	return count * 1000 + current / (F_CPU / 1000000);
}

void delay(uint32_t ms)
{
	uint32_t start = micros();

	if (ms > 0) {
		while (1) {
			if ((micros() - start) >= 1000) {
				ms--;
				if (ms == 0) return;
				start += 1000;
			}
			yield();
		}
	}
}

// TODO: verify these result in correct timeouts...
#if F_CPU == 168000000
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




























