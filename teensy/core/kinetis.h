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

#ifndef _kinetis_h_
#define _kinetis_h_

#include <stdint.h>

// Teensy 3.0
#if defined(__MK20DX128__)
enum IRQ_NUMBER_t {
	IRQ_DMA_CH0 =		0,
	IRQ_DMA_CH1 =		1,
	IRQ_DMA_CH2 =		2,
	IRQ_DMA_CH3 =		3,
	IRQ_DMA_ERROR =		4,
	IRQ_FTFL_COMPLETE =	6,
	IRQ_FTFL_COLLISION =	7,
	IRQ_LOW_VOLTAGE =	8,
	IRQ_LLWU =		9,
	IRQ_WDOG =		10,
	IRQ_I2C0 =		11,
	IRQ_SPI0 =		12,
	IRQ_I2S0_TX =		13,
	IRQ_I2S0_RX =		14,
	IRQ_UART0_LON =		15,
	IRQ_UART0_STATUS =	16,
	IRQ_UART0_ERROR =	17,
	IRQ_UART1_STATUS =	18,
	IRQ_UART1_ERROR =	19,
	IRQ_UART2_STATUS =	20,
	IRQ_UART2_ERROR =	21,
	IRQ_ADC0 =		22,
	IRQ_CMP0 =		23,
	IRQ_CMP1 =		24,
	IRQ_FTM0 =		25,
	IRQ_FTM1 =		26,
	IRQ_CMT =		27,
	IRQ_RTC_ALARM =		28,
	IRQ_RTC_SECOND =	29,
	IRQ_PIT_CH0 =		30,
	IRQ_PIT_CH1 =		31,
	IRQ_PIT_CH2 =		32,
	IRQ_PIT_CH3 =		33,
	IRQ_PDB =		34,
	IRQ_USBOTG =		35,
	IRQ_USBDCD =		36,
	IRQ_TSI =		37,
	IRQ_MCG =		38,
	IRQ_LPTMR =		39,
	IRQ_PORTA =		40,
	IRQ_PORTB =		41,
	IRQ_PORTC =		42,
	IRQ_PORTD =		43,
	IRQ_PORTE =		44,
	IRQ_SOFTWARE =		45
};
#define NVIC_NUM_INTERRUPTS	46
#define DMA_NUM_CHANNELS	4

#define DMAMUX_SOURCE_UART0_RX		2
#define DMAMUX_SOURCE_UART0_TX		3
#define DMAMUX_SOURCE_UART1_RX		4
#define DMAMUX_SOURCE_UART1_TX		5
#define DMAMUX_SOURCE_UART2_RX		6
#define DMAMUX_SOURCE_UART2_TX		7
#define DMAMUX_SOURCE_I2S0_RX		14
#define DMAMUX_SOURCE_I2S0_TX		15
#define DMAMUX_SOURCE_SPI0_RX		16
#define DMAMUX_SOURCE_SPI0_TX		17
#define DMAMUX_SOURCE_I2C0		22
#define DMAMUX_SOURCE_FTM0_CH0		24
#define DMAMUX_SOURCE_FTM0_CH1		25
#define DMAMUX_SOURCE_FTM0_CH2		26
#define DMAMUX_SOURCE_FTM0_CH3		27
#define DMAMUX_SOURCE_FTM0_CH4		28
#define DMAMUX_SOURCE_FTM0_CH5		29
#define DMAMUX_SOURCE_FTM0_CH6		30
#define DMAMUX_SOURCE_FTM0_CH7		31
#define DMAMUX_SOURCE_FTM1_CH0		32
#define DMAMUX_SOURCE_FTM1_CH1		33
#define DMAMUX_SOURCE_ADC0		40
#define DMAMUX_SOURCE_CMP0		42
#define DMAMUX_SOURCE_CMP1		43
#define DMAMUX_SOURCE_DAC0		45
#define DMAMUX_SOURCE_CMT		47
#define DMAMUX_SOURCE_PDB		48
#define DMAMUX_SOURCE_PORTA		49
#define DMAMUX_SOURCE_PORTB		50
#define DMAMUX_SOURCE_PORTC		51
#define DMAMUX_SOURCE_PORTD		52
#define DMAMUX_SOURCE_PORTE		53
#define DMAMUX_SOURCE_ALWAYS0		54
#define DMAMUX_SOURCE_ALWAYS1		55
#define DMAMUX_SOURCE_ALWAYS2		56
#define DMAMUX_SOURCE_ALWAYS3		57
#define DMAMUX_SOURCE_ALWAYS4		58
#define DMAMUX_SOURCE_ALWAYS5		59
#define DMAMUX_SOURCE_ALWAYS6		60
#define DMAMUX_SOURCE_ALWAYS7		61
#define DMAMUX_SOURCE_ALWAYS8		62
#define DMAMUX_SOURCE_ALWAYS9		63
#define DMAMUX_NUM_SOURCE_ALWAYS	10

#define KINETISK
#define HAS_KINETISK_UART0
#define HAS_KINETISK_UART0_FIFO
#define HAS_KINETISK_UART1
#define HAS_KINETISK_UART2
#define HAS_KINETIS_I2C0
#define HAS_KINETIS_LLWU_16CH
#define HAS_KINETIS_ADC0
#define HAS_KINETIS_TSI
#define HAS_KINETIS_FLASH_FTFL


// Teensy 3.1 & 3.2
#elif defined(__MK20DX256__)
enum IRQ_NUMBER_t {
	IRQ_DMA_CH0 =		0,
	IRQ_DMA_CH1 =		1,
	IRQ_DMA_CH2 =		2,
	IRQ_DMA_CH3 =		3,
	IRQ_DMA_CH4 =		4,
	IRQ_DMA_CH5 =		5,
	IRQ_DMA_CH6 =		6,
	IRQ_DMA_CH7 =		7,
	IRQ_DMA_CH8 =		8,
	IRQ_DMA_CH9 =		9,
	IRQ_DMA_CH10 =		10,
	IRQ_DMA_CH11 =		11,
	IRQ_DMA_CH12 =		12,
	IRQ_DMA_CH13 =		13,
	IRQ_DMA_CH14 =		14,
	IRQ_DMA_CH15 =		15,
	IRQ_DMA_ERROR =		16,
	IRQ_FTFL_COMPLETE =	18,
	IRQ_FTFL_COLLISION =	19,
	IRQ_LOW_VOLTAGE =	20,
	IRQ_LLWU =		21,
	IRQ_WDOG =		22,
	IRQ_I2C0 =		24,
	IRQ_I2C1 =		25,
	IRQ_SPI0 =		26,
	IRQ_SPI1 =		27,
	IRQ_CAN_MESSAGE =	29,
	IRQ_CAN_BUS_OFF =	30,
	IRQ_CAN_ERROR =		31,
	IRQ_CAN_TX_WARN =	32,
	IRQ_CAN_RX_WARN =	33,
	IRQ_CAN_WAKEUP =	34,
	IRQ_I2S0_TX =		35,
	IRQ_I2S0_RX =		36,
	IRQ_UART0_LON =		44,
	IRQ_UART0_STATUS =	45,
	IRQ_UART0_ERROR =	46,
	IRQ_UART1_STATUS =	47,
	IRQ_UART1_ERROR =	48,
	IRQ_UART2_STATUS =	49,
	IRQ_UART2_ERROR =	50,
	IRQ_ADC0 =		57,
	IRQ_ADC1 =		58,
	IRQ_CMP0 =		59,
	IRQ_CMP1 =		60,
	IRQ_CMP2 =		61,
	IRQ_FTM0 =		62,
	IRQ_FTM1 =		63,
	IRQ_FTM2 =		64,
	IRQ_CMT =		65,
	IRQ_RTC_ALARM =		66,
	IRQ_RTC_SECOND =	67,
	IRQ_PIT_CH0 =		68,
	IRQ_PIT_CH1 =		69,
	IRQ_PIT_CH2 =		70,
	IRQ_PIT_CH3 =		71,
	IRQ_PDB =		72,
	IRQ_USBOTG =		73,
	IRQ_USBDCD =		74,
	IRQ_DAC0 =		81,
	IRQ_TSI =		83,
	IRQ_MCG =		84,
	IRQ_LPTMR =		85,
	IRQ_PORTA =		87,
	IRQ_PORTB =		88,
	IRQ_PORTC =		89,
	IRQ_PORTD =		90,
	IRQ_PORTE =		91,
	IRQ_SOFTWARE =		94
};
#define NVIC_NUM_INTERRUPTS	95
#define DMA_NUM_CHANNELS	16

#define DMAMUX_SOURCE_UART0_RX		2
#define DMAMUX_SOURCE_UART0_TX		3
#define DMAMUX_SOURCE_UART1_RX		4
#define DMAMUX_SOURCE_UART1_TX		5
#define DMAMUX_SOURCE_UART2_RX		6
#define DMAMUX_SOURCE_UART2_TX		7
#define DMAMUX_SOURCE_I2S0_RX		14
#define DMAMUX_SOURCE_I2S0_TX		15
#define DMAMUX_SOURCE_SPI0_RX		16
#define DMAMUX_SOURCE_SPI0_TX		17
#define DMAMUX_SOURCE_SPI1_RX		18
#define DMAMUX_SOURCE_SPI1_TX		19
#define DMAMUX_SOURCE_I2C0		22
#define DMAMUX_SOURCE_I2C1		23
#define DMAMUX_SOURCE_FTM0_CH0		24
#define DMAMUX_SOURCE_FTM0_CH1		25
#define DMAMUX_SOURCE_FTM0_CH2		26
#define DMAMUX_SOURCE_FTM0_CH3		27
#define DMAMUX_SOURCE_FTM0_CH4		28
#define DMAMUX_SOURCE_FTM0_CH5		29
#define DMAMUX_SOURCE_FTM0_CH6		30
#define DMAMUX_SOURCE_FTM0_CH7		31
#define DMAMUX_SOURCE_FTM1_CH0		32
#define DMAMUX_SOURCE_FTM1_CH1		33
#define DMAMUX_SOURCE_FTM2_CH0		34
#define DMAMUX_SOURCE_FTM2_CH1		35
#define DMAMUX_SOURCE_ADC0		40
#define DMAMUX_SOURCE_ADC1		41
#define DMAMUX_SOURCE_CMP0		42
#define DMAMUX_SOURCE_CMP1		43
#define DMAMUX_SOURCE_CMP2		44
#define DMAMUX_SOURCE_DAC0		45
#define DMAMUX_SOURCE_CMT		47
#define DMAMUX_SOURCE_PDB		48
#define DMAMUX_SOURCE_PORTA		49
#define DMAMUX_SOURCE_PORTB		50
#define DMAMUX_SOURCE_PORTC		51
#define DMAMUX_SOURCE_PORTD		52
#define DMAMUX_SOURCE_PORTE		53
#define DMAMUX_SOURCE_ALWAYS0		54
#define DMAMUX_SOURCE_ALWAYS1		55
#define DMAMUX_SOURCE_ALWAYS2		56
#define DMAMUX_SOURCE_ALWAYS3		57
#define DMAMUX_SOURCE_ALWAYS4		58
#define DMAMUX_SOURCE_ALWAYS5		59
#define DMAMUX_SOURCE_ALWAYS6		60
#define DMAMUX_SOURCE_ALWAYS7		61
#define DMAMUX_SOURCE_ALWAYS8		62
#define DMAMUX_SOURCE_ALWAYS9		63
#define DMAMUX_NUM_SOURCE_ALWAYS	10

#define KINETISK
#define HAS_KINETISK_UART0
#define HAS_KINETISK_UART0_FIFO
#define HAS_KINETISK_UART1
#define HAS_KINETISK_UART1_FIFO
#define HAS_KINETISK_UART2
#define HAS_KINETIS_I2C0
#define HAS_KINETIS_I2C1
#define HAS_KINETIS_LLWU_16CH
#define HAS_KINETIS_ADC0
#define HAS_KINETIS_ADC1
#define HAS_KINETIS_TSI
#define HAS_KINETIS_FLASH_FTFL

// Teensy-LC
#elif defined(__MKL26Z64__)
enum IRQ_NUMBER_t {
	IRQ_DMA_CH0 =		0,
	IRQ_DMA_CH1 =		1,
	IRQ_DMA_CH2 =		2,
	IRQ_DMA_CH3 =		3,
        IRQ_FTFA =              5,
        IRQ_LOW_VOLTAGE =       6,
        IRQ_LLWU =              7,
        IRQ_I2C0 =              8,
        IRQ_I2C1 =              9,
        IRQ_SPI0 =              10,
        IRQ_SPI1 =              11,
        IRQ_UART0_STATUS =      12,
        IRQ_UART1_STATUS =      13,
        IRQ_UART2_STATUS =      14,
        IRQ_ADC0 =              15,
        IRQ_CMP0 =              16,
        IRQ_FTM0 =              17,
        IRQ_FTM1 =              18,
        IRQ_FTM2 =              19,
        IRQ_RTC_ALARM =         20,
        IRQ_RTC_SECOND =        21,
        IRQ_PIT =               22,
        IRQ_I2S0 =              23,
        IRQ_USBOTG =            24,
        IRQ_DAC0 =              25,
        IRQ_TSI =               26,
        IRQ_MCG =               27,
        IRQ_LPTMR =             28,
	IRQ_SOFTWARE =		29,  // TODO: verify this works
        IRQ_PORTA =             30,
        IRQ_PORTCD =            31
};
#define NVIC_NUM_INTERRUPTS     32
#define DMA_NUM_CHANNELS        4

#define DMAMUX_SOURCE_UART0_RX		2
#define DMAMUX_SOURCE_UART0_TX		3
#define DMAMUX_SOURCE_UART1_RX		4
#define DMAMUX_SOURCE_UART1_TX		5
#define DMAMUX_SOURCE_UART2_RX		6
#define DMAMUX_SOURCE_UART2_TX		7
#define DMAMUX_SOURCE_I2S0_RX		14
#define DMAMUX_SOURCE_I2S0_TX		15
#define DMAMUX_SOURCE_SPI0_RX		16
#define DMAMUX_SOURCE_SPI0_TX		17
#define DMAMUX_SOURCE_SPI1_RX		18
#define DMAMUX_SOURCE_SPI1_TX		19
#define DMAMUX_SOURCE_I2C0		22
#define DMAMUX_SOURCE_I2C1		23
#define DMAMUX_SOURCE_TPM0_CH0		24
#define DMAMUX_SOURCE_TPM0_CH1		25
#define DMAMUX_SOURCE_TPM0_CH2		26
#define DMAMUX_SOURCE_TPM0_CH3		27
#define DMAMUX_SOURCE_TPM0_CH4		28
#define DMAMUX_SOURCE_TPM0_CH5		29
#define DMAMUX_SOURCE_TPM1_CH0		32
#define DMAMUX_SOURCE_TPM1_CH1		33
#define DMAMUX_SOURCE_TPM2_CH0		34
#define DMAMUX_SOURCE_TPM2_CH1		35
#define DMAMUX_SOURCE_ADC0		40
#define DMAMUX_SOURCE_CMP0		42
#define DMAMUX_SOURCE_DAC0		45
#define DMAMUX_SOURCE_PORTA		49
#define DMAMUX_SOURCE_PORTC		51
#define DMAMUX_SOURCE_PORTD		52
#define DMAMUX_SOURCE_FTM0_OV		54
#define DMAMUX_SOURCE_FTM1_OV		55
#define DMAMUX_SOURCE_FTM2_OV		56
#define DMAMUX_SOURCE_TSI		57
#define DMAMUX_SOURCE_ALWAYS0		60
#define DMAMUX_SOURCE_ALWAYS1		61
#define DMAMUX_SOURCE_ALWAYS2		62
#define DMAMUX_SOURCE_ALWAYS3		63
#define DMAMUX_NUM_SOURCE_ALWAYS	4

#define KINETISL
#define HAS_KINETISL_UART0
#define HAS_KINETISL_UART1
#define HAS_KINETISL_UART2
#define HAS_KINETIS_I2C0
#define HAS_KINETIS_I2C0_STOPF
#define HAS_KINETIS_I2C1
#define HAS_KINETIS_I2C1_STOPF
#define HAS_KINETIS_LLWU_16CH
#define HAS_KINETIS_ADC0
#define HAS_KINETIS_TSI_LITE
#define HAS_KINETIS_FLASH_FTFA


#elif defined(__MK64FX512__)
enum IRQ_NUMBER_t {
	IRQ_DMA_CH0 =		0,
	IRQ_DMA_CH1 =		1,
	IRQ_DMA_CH2 =		2,
	IRQ_DMA_CH3 =		3,
	IRQ_DMA_CH4 =		4,
	IRQ_DMA_CH5 =		5,
	IRQ_DMA_CH6 =		6,
	IRQ_DMA_CH7 =		7,
	IRQ_DMA_CH8 =		8,
	IRQ_DMA_CH9 =		9,
	IRQ_DMA_CH10 =		10,
	IRQ_DMA_CH11 =		11,
	IRQ_DMA_CH12 =		12,
	IRQ_DMA_CH13 =		13,
	IRQ_DMA_CH14 =		14,
	IRQ_DMA_CH15 =		15,
	IRQ_DMA_ERROR =		16,
	IRQ_MCM =		17,
	IRQ_FTFL_COMPLETE =	18,
	IRQ_FTFL_COLLISION =	19,
	IRQ_LOW_VOLTAGE =	20,
	IRQ_LLWU =		21,
	IRQ_WDOG =		22,
	IRQ_RNG =		23,
	IRQ_I2C0 =		24,
	IRQ_I2C1 =		25,
	IRQ_SPI0 =		26,
	IRQ_SPI1 =		27,
	IRQ_I2S0_TX =		28,
	IRQ_I2S0_RX =		29,
	IRQ_UART0_STATUS =	31,
	IRQ_UART0_ERROR =	32,
	IRQ_UART1_STATUS =	33,
	IRQ_UART1_ERROR =	34,
	IRQ_UART2_STATUS =	35,
	IRQ_UART2_ERROR =	36,
	IRQ_UART3_STATUS =	37,
	IRQ_UART3_ERROR =	38,
	IRQ_ADC0 =		39,
	IRQ_CMP0 =		40,
	IRQ_CMP1 =		41,
	IRQ_FTM0 =		42,
	IRQ_FTM1 =		43,
	IRQ_FTM2 =		44,
	IRQ_CMT =		45,
	IRQ_RTC_ALARM =		46,
	IRQ_RTC_SECOND =	47,
	IRQ_PIT_CH0 =		48,
	IRQ_PIT_CH1 =		49,
	IRQ_PIT_CH2 =		50,
	IRQ_PIT_CH3 =		51,
	IRQ_PDB =		52,
	IRQ_USBOTG =		53,
	IRQ_USBDCD =		54,
	IRQ_DAC0 =		56,
	IRQ_MCG =		57,
	IRQ_LPTMR =		58,
	IRQ_PORTA =		59,
	IRQ_PORTB =		60,
	IRQ_PORTC =		61,
	IRQ_PORTD =		62,
	IRQ_PORTE =		63,
	IRQ_SOFTWARE =		64,
	IRQ_SPI2 =		65,
	IRQ_UART4_STATUS =	66,
	IRQ_UART4_ERROR =	67,
	IRQ_UART5_STATUS =	68,
	IRQ_UART5_ERROR =	69,
	IRQ_CMP2 =		70,
	IRQ_FTM3 =		71,
	IRQ_DAC1 =		72,
	IRQ_ADC1 =		73,
	IRQ_I2C2 =		74,
	IRQ_CAN0_MESSAGE =	75,
	IRQ_CAN0_BUS_OFF =	76,
	IRQ_CAN0_ERROR =	77,
	IRQ_CAN0_TX_WARN =	78,
	IRQ_CAN0_RX_WARN =	79,
	IRQ_CAN0_WAKEUP =	80,
	IRQ_SDHC =		81,
	IRQ_ENET_TIMER =	82,
	IRQ_ENET_TX =		83,
	IRQ_ENET_RX =		84,
	IRQ_ENET_ERROR =	85
};
#define NVIC_NUM_INTERRUPTS	86
#define DMA_NUM_CHANNELS	16

#define DMAMUX_SOURCE_TSI		1
#define DMAMUX_SOURCE_UART0_RX		2
#define DMAMUX_SOURCE_UART0_TX		3
#define DMAMUX_SOURCE_UART1_RX		4
#define DMAMUX_SOURCE_UART1_TX		5
#define DMAMUX_SOURCE_UART2_RX		6
#define DMAMUX_SOURCE_UART2_TX		7
#define DMAMUX_SOURCE_UART3_RX		8
#define DMAMUX_SOURCE_UART3_TX		9
#define DMAMUX_SOURCE_UART4_RXTX	10
#define DMAMUX_SOURCE_UART5_RXTX	11
#define DMAMUX_SOURCE_I2S0_RX		12
#define DMAMUX_SOURCE_I2S0_TX		13
#define DMAMUX_SOURCE_SPI0_RX		14
#define DMAMUX_SOURCE_SPI0_TX		15
#define DMAMUX_SOURCE_SPI1_RX		16
#define DMAMUX_SOURCE_SPI1_TX		17
#define DMAMUX_SOURCE_I2C0		18
#define DMAMUX_SOURCE_I2C1		19
#define DMAMUX_SOURCE_I2C2		19
#define DMAMUX_SOURCE_FTM0_CH0		20
#define DMAMUX_SOURCE_FTM0_CH1		21
#define DMAMUX_SOURCE_FTM0_CH2		22
#define DMAMUX_SOURCE_FTM0_CH3		23
#define DMAMUX_SOURCE_FTM0_CH4		24
#define DMAMUX_SOURCE_FTM0_CH5		25
#define DMAMUX_SOURCE_FTM0_CH6		26
#define DMAMUX_SOURCE_FTM0_CH7		27
#define DMAMUX_SOURCE_FTM1_CH0		28
#define DMAMUX_SOURCE_FTM1_CH1		29
#define DMAMUX_SOURCE_FTM2_CH0		30
#define DMAMUX_SOURCE_FTM2_CH1		31
#define DMAMUX_SOURCE_FTM3_CH0		32
#define DMAMUX_SOURCE_FTM3_CH1		33
#define DMAMUX_SOURCE_FTM3_CH2		34
#define DMAMUX_SOURCE_FTM3_CH3		35
#define DMAMUX_SOURCE_FTM3_CH4		36
#define DMAMUX_SOURCE_FTM3_CH5		37
#define DMAMUX_SOURCE_FTM3_CH6		38
#define DMAMUX_SOURCE_FTM3_CH7		39
#define DMAMUX_SOURCE_ADC0		40
#define DMAMUX_SOURCE_ADC1		41
#define DMAMUX_SOURCE_CMP0		42
#define DMAMUX_SOURCE_CMP1		43
#define DMAMUX_SOURCE_CMP2		44
#define DMAMUX_SOURCE_DAC0		45
#define DMAMUX_SOURCE_DAC1		46
#define DMAMUX_SOURCE_CMT		47
#define DMAMUX_SOURCE_PDB		48
#define DMAMUX_SOURCE_PORTA		49
#define DMAMUX_SOURCE_PORTB		50
#define DMAMUX_SOURCE_PORTC		51
#define DMAMUX_SOURCE_PORTD		52
#define DMAMUX_SOURCE_PORTE		53
#define DMAMUX_SOURCE_IEEE1588_T0	54
#define DMAMUX_SOURCE_IEEE1588_T1	55
#define DMAMUX_SOURCE_IEEE1588_T2	56
#define DMAMUX_SOURCE_IEEE1588_T3	57
#define DMAMUX_SOURCE_ALWAYS0		58
#define DMAMUX_SOURCE_ALWAYS1		59
#define DMAMUX_SOURCE_ALWAYS2		60
#define DMAMUX_SOURCE_ALWAYS3		61
#define DMAMUX_SOURCE_ALWAYS4		62
#define DMAMUX_SOURCE_ALWAYS5		63
#define DMAMUX_NUM_SOURCE_ALWAYS	6

#define KINETISK
#define HAS_KINETISK_UART0
#define HAS_KINETISK_UART0_FIFO
#define HAS_KINETISK_UART1
#define HAS_KINETISK_UART1_FIFO
#define HAS_KINETISK_UART2
#define HAS_KINETISK_UART3
#define HAS_KINETISK_UART4
#define HAS_KINETISK_UART5
#define HAS_KINETIS_I2C0
#define HAS_KINETIS_I2C0_STOPF
#define HAS_KINETIS_I2C1
#define HAS_KINETIS_I2C1_STOPF
#define HAS_KINETIS_I2C2
#define HAS_KINETIS_I2C2_STOPF
#define HAS_KINETIS_LLWU_32CH
#define HAS_KINETIS_MPU
#define HAS_KINETIS_ADC0
#define HAS_KINETIS_ADC1
#define HAS_KINETIS_FLASH_FTFE


#elif defined(__MK66FX1M0__)
// https://forum.pjrc.com/threads/24633-Any-Chance-of-a-Teensy-3-1?p=78655&viewfull=1#post78655
enum IRQ_NUMBER_t {
	IRQ_DMA_CH0 =		0,
	IRQ_DMA_CH1 =		1,
	IRQ_DMA_CH2 =		2,
	IRQ_DMA_CH3 =		3,
	IRQ_DMA_CH4 =		4,
	IRQ_DMA_CH5 =		5,
	IRQ_DMA_CH6 =		6,
	IRQ_DMA_CH7 =		7,
	IRQ_DMA_CH8 =		8,
	IRQ_DMA_CH9 =		9,
	IRQ_DMA_CH10 =		10,
	IRQ_DMA_CH11 =		11,
	IRQ_DMA_CH12 =		12,
	IRQ_DMA_CH13 =		13,
	IRQ_DMA_CH14 =		14,
	IRQ_DMA_CH15 =		15,
	IRQ_DMA_ERROR =		16,
	IRQ_MCM =		17,
	IRQ_FTFL_COMPLETE =	18,
	IRQ_FTFL_COLLISION =	19,
	IRQ_LOW_VOLTAGE =	20,
	IRQ_LLWU =		21,
	IRQ_WDOG =		22,
	IRQ_RNG =		23,
	IRQ_I2C0 =		24,
	IRQ_I2C1 =		25,
	IRQ_SPI0 =		26,
	IRQ_SPI1 =		27,
	IRQ_I2S0_TX =		28,
	IRQ_I2S0_RX =		29,
	IRQ_UART0_STATUS =	31,
	IRQ_UART0_ERROR =	32,
	IRQ_UART1_STATUS =	33,
	IRQ_UART1_ERROR =	34,
	IRQ_UART2_STATUS =	35,
	IRQ_UART2_ERROR =	36,
	IRQ_UART3_STATUS =	37,
	IRQ_UART3_ERROR =	38,
	IRQ_ADC0 =		39,
	IRQ_CMP0 =		40,
	IRQ_CMP1 =		41,
	IRQ_FTM0 =		42,
	IRQ_FTM1 =		43,
	IRQ_FTM2 =		44,
	IRQ_CMT =		45,
	IRQ_RTC_ALARM =		46,
	IRQ_RTC_SECOND =	47,
	IRQ_PIT_CH0 =		48,
	IRQ_PIT_CH1 =		49,
	IRQ_PIT_CH2 =		50,
	IRQ_PIT_CH3 =		51,
	IRQ_PDB =		52,
	IRQ_USBOTG =		53,
	IRQ_USBDCD =		54,
	IRQ_DAC0 =		56,
	IRQ_MCG =		57,
	IRQ_LPTMR =		58,
	IRQ_PORTA =		59,
	IRQ_PORTB =		60,
	IRQ_PORTC =		61,
	IRQ_PORTD =		62,
	IRQ_PORTE =		63,
	IRQ_SOFTWARE =		64,
	IRQ_SPI2 =		65,
	IRQ_UART4_STATUS =	66,
	IRQ_UART4_ERROR =	67,
	IRQ_CMP2 =		70,
	IRQ_FTM3 =		71,
	IRQ_DAC1 =		72,
	IRQ_ADC1 =		73,
	IRQ_I2C2 =		74,
	IRQ_CAN0_MESSAGE =	75,
	IRQ_CAN0_BUS_OFF =	76,
	IRQ_CAN0_ERROR =	77,
	IRQ_CAN0_TX_WARN =	78,
	IRQ_CAN0_RX_WARN =	79,
	IRQ_CAN0_WAKEUP =	80,
	IRQ_SDHC =		81,
	IRQ_ENET_TIMER =	82,
	IRQ_ENET_TX =		83,
	IRQ_ENET_RX =		84,
	IRQ_ENET_ERROR =	85,
	IRQ_LPUART0 =		86,
	IRQ_TSI =		87,
	IRQ_TPM1 =		88,
	IRQ_TPM2 =		89,
	IRQ_USBHS_PHY =		90,
	IRQ_I2C3 =		91,
	IRQ_CMP3 =		92,
	IRQ_USBHS =		93,
	IRQ_CAN1_MESSAGE =	94,
	IRQ_CAN1_BUS_OFF =	95,
	IRQ_CAN1_ERROR =	96,
	IRQ_CAN1_TX_WARN =	97,
	IRQ_CAN1_RX_WARN =	98,
	IRQ_CAN1_WAKEUP =	99
};
#define NVIC_NUM_INTERRUPTS	100
#define DMA_NUM_CHANNELS	32

#define DMAMUX_SOURCE_TSI		1
#define DMAMUX_SOURCE_UART0_RX		2
#define DMAMUX_SOURCE_UART0_TX		3
#define DMAMUX_SOURCE_UART1_RX		4
#define DMAMUX_SOURCE_UART1_TX		5
#define DMAMUX_SOURCE_UART2_RX		6
#define DMAMUX_SOURCE_UART2_TX		7
#define DMAMUX_SOURCE_UART3_RX		8
#define DMAMUX_SOURCE_UART3_TX		9
#define DMAMUX_SOURCE_UART4_RXTX	10
#define DMAMUX_SOURCE_I2S0_RX		12
#define DMAMUX_SOURCE_I2S0_TX		13
#define DMAMUX_SOURCE_SPI0_RX		14
#define DMAMUX_SOURCE_SPI0_TX		15
#define DMAMUX_SOURCE_SPI1_RX		16
#define DMAMUX_SOURCE_SPI1_TX		17
#define DMAMUX_SOURCE_I2C0		18
#define DMAMUX_SOURCE_I2C3		18
#define DMAMUX_SOURCE_I2C1		19
#define DMAMUX_SOURCE_I2C2		19
#define DMAMUX_SOURCE_FTM0_CH0		20
#define DMAMUX_SOURCE_FTM0_CH1		21
#define DMAMUX_SOURCE_FTM0_CH2		22
#define DMAMUX_SOURCE_FTM0_CH3		23
#define DMAMUX_SOURCE_FTM0_CH4		24
#define DMAMUX_SOURCE_FTM0_CH5		25
#define DMAMUX_SOURCE_FTM0_CH6		26
#define DMAMUX_SOURCE_FTM0_CH7		27
#define DMAMUX_SOURCE_FTM1_CH0		28
#define DMAMUX_SOURCE_TPM1_CH0		28
#define DMAMUX_SOURCE_FTM1_CH1		29
#define DMAMUX_SOURCE_TPM1_CH1		29
#define DMAMUX_SOURCE_FTM2_CH0		30
#define DMAMUX_SOURCE_TPM2_CH0		30
#define DMAMUX_SOURCE_FTM2_CH1		31
#define DMAMUX_SOURCE_TPM2_CH1		31
#define DMAMUX_SOURCE_FTM3_CH0		32
#define DMAMUX_SOURCE_FTM3_CH1		33
#define DMAMUX_SOURCE_FTM3_CH2		34
#define DMAMUX_SOURCE_FTM3_CH3		35
#define DMAMUX_SOURCE_FTM3_CH4		36
#define DMAMUX_SOURCE_FTM3_CH5		37
#define DMAMUX_SOURCE_FTM3_CH6		38
#define DMAMUX_SOURCE_SPI2_RX		38
#define DMAMUX_SOURCE_FTM3_CH7		39
#define DMAMUX_SOURCE_SPI2_TX		39
#define DMAMUX_SOURCE_ADC0		40
#define DMAMUX_SOURCE_ADC1		41
#define DMAMUX_SOURCE_CMP0		42
#define DMAMUX_SOURCE_CMP1		43
#define DMAMUX_SOURCE_CMP2		44
#define DMAMUX_SOURCE_CMP3		44
#define DMAMUX_SOURCE_DAC0		45
#define DMAMUX_SOURCE_DAC1		46
#define DMAMUX_SOURCE_CMT		47
#define DMAMUX_SOURCE_PDB		48
#define DMAMUX_SOURCE_PORTA		49
#define DMAMUX_SOURCE_PORTB		50
#define DMAMUX_SOURCE_PORTC		51
#define DMAMUX_SOURCE_PORTD		52
#define DMAMUX_SOURCE_PORTE		53
#define DMAMUX_SOURCE_IEEE1588_T0	54
#define DMAMUX_SOURCE_IEEE1588_T1	55
#define DMAMUX_SOURCE_FTM1_OV		55
#define DMAMUX_SOURCE_IEEE1588_T2	56
#define DMAMUX_SOURCE_FTM2_OV		56
#define DMAMUX_SOURCE_IEEE1588_T3	57
#define DMAMUX_SOURCE_LPUART0_RX	58
#define DMAMUX_SOURCE_LPUART0_TX	59
#define DMAMUX_SOURCE_ALWAYS0		60
#define DMAMUX_SOURCE_ALWAYS1		61
#define DMAMUX_SOURCE_ALWAYS2		62
#define DMAMUX_SOURCE_ALWAYS3		63
#define DMAMUX_NUM_SOURCE_ALWAYS	4

#define KINETISK
#define HAS_KINETISK_UART0
#define HAS_KINETISK_UART0_FIFO
#define HAS_KINETISK_UART1
#define HAS_KINETISK_UART1_FIFO
#define HAS_KINETISK_UART2
#define HAS_KINETISK_UART3
#define HAS_KINETISK_UART4
#define HAS_KINETISK_LPUART0
#define HAS_KINETIS_I2C0
#define HAS_KINETIS_I2C0_STOPF
#define HAS_KINETIS_I2C1
#define HAS_KINETIS_I2C1_STOPF
#define HAS_KINETIS_I2C2
#define HAS_KINETIS_I2C2_STOPF
#define HAS_KINETIS_I2C3
#define HAS_KINETIS_I2C3_STOPF
#define HAS_KINETIS_LLWU_32CH
#define HAS_KINETIS_MPU
#define HAS_KINETIS_ADC0
#define HAS_KINETIS_ADC1
#define HAS_KINETIS_TSI_LITE
#define HAS_KINETIS_FLASH_FTFE



#endif // end of board-specific definitions


#if (F_CPU == 240000000)
 #define F_PLL 240000000
 #ifndef F_BUS
 #define F_BUS 60000000
 //#define F_BUS 80000000   // uncomment these to try peripheral overclocking
 //#define F_BUS 120000000  // all the usual overclocking caveats apply...
 #endif
 #define F_MEM 30000000
#elif (F_CPU == 216000000)
 #define F_PLL 216000000
 #ifndef F_BUS
 #define F_BUS 54000000
 //#define F_BUS 72000000
 //#define F_BUS 108000000
 #endif
 #define F_MEM 27000000
#elif (F_CPU == 192000000)
 #define F_PLL 192000000
 #ifndef F_BUS
 #define F_BUS 48000000
 //#define F_BUS 64000000
 //#define F_BUS 96000000
 #endif
 #define F_MEM 27428571
#elif (F_CPU == 180000000)
 #define F_PLL 180000000
 #ifndef F_BUS
 #define F_BUS 60000000
 //#define F_BUS 90000000
 #endif
 #define F_MEM 25714286
#elif (F_CPU == 168000000)
 #define F_PLL 168000000
 #define F_BUS 56000000
 #define F_MEM 28000000
#elif (F_CPU == 144000000)
 #define F_PLL 144000000
 #ifndef F_BUS
 #define F_BUS 48000000
 //#define F_BUS 72000000
 #endif
 #define F_MEM 28800000
#elif (F_CPU == 120000000)
 #define F_PLL 120000000
 #ifndef F_BUS
 #define F_BUS 60000000
 //#define F_BUS 120000000
 #endif
 #define F_MEM 24000000
#elif (F_CPU == 96000000)
 #define F_PLL 96000000
 #ifndef F_BUS
 #define F_BUS 48000000
 //#define F_BUS 96000000
 #endif
 #define F_MEM 24000000
#elif (F_CPU == 72000000)
 #define F_PLL 72000000
 #ifndef F_BUS
 #define F_BUS 36000000
 //#define F_BUS 72000000
 #endif
 #define F_MEM 24000000
#elif (F_CPU == 48000000)
 #define F_PLL 96000000
 #if defined(KINETISK)
 #define F_BUS 48000000
 #elif defined(KINETISL)
 #define F_BUS 24000000
 #endif
 #define F_MEM 24000000
#elif (F_CPU == 24000000)
 #define F_PLL 96000000
 #define F_BUS 24000000
 #define F_MEM 24000000
#elif (F_CPU == 16000000)
 #define F_PLL 16000000
 #define F_BUS 16000000
 #define F_MEM 16000000
#elif (F_CPU == 8000000)
 #define F_PLL 8000000
 #define F_BUS 8000000
 #define F_MEM 8000000
#elif (F_CPU == 4000000)
 #define F_PLL 4000000
 #define F_BUS 4000000
 #define F_MEM 4000000
#elif (F_CPU == 2000000)
 #define F_PLL 2000000
 #define F_BUS 2000000
 #define F_MEM 1000000
#endif


#ifndef NULL
#define NULL (0)
#endif

// Port control and interrupts (PORT)

#define PORTA_PCR0		(*(volatile uint32_t *)0x40049000) // Pin Control Register n
#define PORT_PCR_ISF			((uint32_t)0x01000000)		// Interrupt Status Flag
#define PORT_PCR_IRQC(n)		((uint32_t)(((n) & 15) << 16))	// Interrupt Configuration
#define PORT_PCR_IRQC_MASK		((uint32_t)0x000F0000)
#define PORT_PCR_LK			((uint32_t)0x00008000)		// Lock Register
#define PORT_PCR_MUX(n)			((uint32_t)(((n) & 7) << 8))	// Pin Mux Control
#define PORT_PCR_MUX_MASK		((uint32_t)0x00000700)
#define PORT_PCR_DSE			((uint32_t)0x00000040)		// Drive Strength Enable
#define PORT_PCR_ODE			((uint32_t)0x00000020)		// Open Drain Enable
#define PORT_PCR_PFE			((uint32_t)0x00000010)		// Passive Filter Enable
#define PORT_PCR_SRE			((uint32_t)0x00000004)		// Slew Rate Enable
#define PORT_PCR_PE			((uint32_t)0x00000002)		// Pull Enable
#define PORT_PCR_PS			((uint32_t)0x00000001)		// Pull Select
#define PORTA_PCR1		(*(volatile uint32_t *)0x40049004) // Pin Control Register n
#define PORTA_PCR2		(*(volatile uint32_t *)0x40049008) // Pin Control Register n
#define PORTA_PCR3		(*(volatile uint32_t *)0x4004900C) // Pin Control Register n
#define PORTA_PCR4		(*(volatile uint32_t *)0x40049010) // Pin Control Register n
#define PORTA_PCR5		(*(volatile uint32_t *)0x40049014) // Pin Control Register n
#define PORTA_PCR6		(*(volatile uint32_t *)0x40049018) // Pin Control Register n
#define PORTA_PCR7		(*(volatile uint32_t *)0x4004901C) // Pin Control Register n
#define PORTA_PCR8		(*(volatile uint32_t *)0x40049020) // Pin Control Register n
#define PORTA_PCR9		(*(volatile uint32_t *)0x40049024) // Pin Control Register n
#define PORTA_PCR10		(*(volatile uint32_t *)0x40049028) // Pin Control Register n
#define PORTA_PCR11		(*(volatile uint32_t *)0x4004902C) // Pin Control Register n
#define PORTA_PCR12		(*(volatile uint32_t *)0x40049030) // Pin Control Register n
#define PORTA_PCR13		(*(volatile uint32_t *)0x40049034) // Pin Control Register n
#define PORTA_PCR14		(*(volatile uint32_t *)0x40049038) // Pin Control Register n
#define PORTA_PCR15		(*(volatile uint32_t *)0x4004903C) // Pin Control Register n
#define PORTA_PCR16		(*(volatile uint32_t *)0x40049040) // Pin Control Register n
#define PORTA_PCR17		(*(volatile uint32_t *)0x40049044) // Pin Control Register n
#define PORTA_PCR18		(*(volatile uint32_t *)0x40049048) // Pin Control Register n
#define PORTA_PCR19		(*(volatile uint32_t *)0x4004904C) // Pin Control Register n
#define PORTA_PCR20		(*(volatile uint32_t *)0x40049050) // Pin Control Register n
#define PORTA_PCR21		(*(volatile uint32_t *)0x40049054) // Pin Control Register n
#define PORTA_PCR22		(*(volatile uint32_t *)0x40049058) // Pin Control Register n
#define PORTA_PCR23		(*(volatile uint32_t *)0x4004905C) // Pin Control Register n
#define PORTA_PCR24		(*(volatile uint32_t *)0x40049060) // Pin Control Register n
#define PORTA_PCR25		(*(volatile uint32_t *)0x40049064) // Pin Control Register n
#define PORTA_PCR26		(*(volatile uint32_t *)0x40049068) // Pin Control Register n
#define PORTA_PCR27		(*(volatile uint32_t *)0x4004906C) // Pin Control Register n
#define PORTA_PCR28		(*(volatile uint32_t *)0x40049070) // Pin Control Register n
#define PORTA_PCR29		(*(volatile uint32_t *)0x40049074) // Pin Control Register n
#define PORTA_PCR30		(*(volatile uint32_t *)0x40049078) // Pin Control Register n
#define PORTA_PCR31		(*(volatile uint32_t *)0x4004907C) // Pin Control Register n
#define PORTA_GPCLR		(*(volatile uint32_t *)0x40049080) // Global Pin Control Low Register
#define PORTA_GPCHR		(*(volatile uint32_t *)0x40049084) // Global Pin Control High Register
#define PORTA_ISFR		(*(volatile uint32_t *)0x400490A0) // Interrupt Status Flag Register
#define PORTB_PCR0		(*(volatile uint32_t *)0x4004A000) // Pin Control Register n
#define PORTB_PCR1		(*(volatile uint32_t *)0x4004A004) // Pin Control Register n
#define PORTB_PCR2		(*(volatile uint32_t *)0x4004A008) // Pin Control Register n
#define PORTB_PCR3		(*(volatile uint32_t *)0x4004A00C) // Pin Control Register n
#define PORTB_PCR4		(*(volatile uint32_t *)0x4004A010) // Pin Control Register n
#define PORTB_PCR5		(*(volatile uint32_t *)0x4004A014) // Pin Control Register n
#define PORTB_PCR6		(*(volatile uint32_t *)0x4004A018) // Pin Control Register n
#define PORTB_PCR7		(*(volatile uint32_t *)0x4004A01C) // Pin Control Register n
#define PORTB_PCR8		(*(volatile uint32_t *)0x4004A020) // Pin Control Register n
#define PORTB_PCR9		(*(volatile uint32_t *)0x4004A024) // Pin Control Register n
#define PORTB_PCR10		(*(volatile uint32_t *)0x4004A028) // Pin Control Register n
#define PORTB_PCR11		(*(volatile uint32_t *)0x4004A02C) // Pin Control Register n
#define PORTB_PCR12		(*(volatile uint32_t *)0x4004A030) // Pin Control Register n
#define PORTB_PCR13		(*(volatile uint32_t *)0x4004A034) // Pin Control Register n
#define PORTB_PCR14		(*(volatile uint32_t *)0x4004A038) // Pin Control Register n
#define PORTB_PCR15		(*(volatile uint32_t *)0x4004A03C) // Pin Control Register n
#define PORTB_PCR16		(*(volatile uint32_t *)0x4004A040) // Pin Control Register n
#define PORTB_PCR17		(*(volatile uint32_t *)0x4004A044) // Pin Control Register n
#define PORTB_PCR18		(*(volatile uint32_t *)0x4004A048) // Pin Control Register n
#define PORTB_PCR19		(*(volatile uint32_t *)0x4004A04C) // Pin Control Register n
#define PORTB_PCR20		(*(volatile uint32_t *)0x4004A050) // Pin Control Register n
#define PORTB_PCR21		(*(volatile uint32_t *)0x4004A054) // Pin Control Register n
#define PORTB_PCR22		(*(volatile uint32_t *)0x4004A058) // Pin Control Register n
#define PORTB_PCR23		(*(volatile uint32_t *)0x4004A05C) // Pin Control Register n
#define PORTB_PCR24		(*(volatile uint32_t *)0x4004A060) // Pin Control Register n
#define PORTB_PCR25		(*(volatile uint32_t *)0x4004A064) // Pin Control Register n
#define PORTB_PCR26		(*(volatile uint32_t *)0x4004A068) // Pin Control Register n
#define PORTB_PCR27		(*(volatile uint32_t *)0x4004A06C) // Pin Control Register n
#define PORTB_PCR28		(*(volatile uint32_t *)0x4004A070) // Pin Control Register n
#define PORTB_PCR29		(*(volatile uint32_t *)0x4004A074) // Pin Control Register n
#define PORTB_PCR30		(*(volatile uint32_t *)0x4004A078) // Pin Control Register n
#define PORTB_PCR31		(*(volatile uint32_t *)0x4004A07C) // Pin Control Register n
#define PORTB_GPCLR		(*(volatile uint32_t *)0x4004A080) // Global Pin Control Low Register
#define PORTB_GPCHR		(*(volatile uint32_t *)0x4004A084) // Global Pin Control High Register
#define PORTB_ISFR		(*(volatile uint32_t *)0x4004A0A0) // Interrupt Status Flag Register
#define PORTC_PCR0		(*(volatile uint32_t *)0x4004B000) // Pin Control Register n
#define PORTC_PCR1		(*(volatile uint32_t *)0x4004B004) // Pin Control Register n
#define PORTC_PCR2		(*(volatile uint32_t *)0x4004B008) // Pin Control Register n
#define PORTC_PCR3		(*(volatile uint32_t *)0x4004B00C) // Pin Control Register n
#define PORTC_PCR4		(*(volatile uint32_t *)0x4004B010) // Pin Control Register n
#define PORTC_PCR5		(*(volatile uint32_t *)0x4004B014) // Pin Control Register n
#define PORTC_PCR6		(*(volatile uint32_t *)0x4004B018) // Pin Control Register n
#define PORTC_PCR7		(*(volatile uint32_t *)0x4004B01C) // Pin Control Register n
#define PORTC_PCR8		(*(volatile uint32_t *)0x4004B020) // Pin Control Register n
#define PORTC_PCR9		(*(volatile uint32_t *)0x4004B024) // Pin Control Register n
#define PORTC_PCR10		(*(volatile uint32_t *)0x4004B028) // Pin Control Register n
#define PORTC_PCR11		(*(volatile uint32_t *)0x4004B02C) // Pin Control Register n
#define PORTC_PCR12		(*(volatile uint32_t *)0x4004B030) // Pin Control Register n
#define PORTC_PCR13		(*(volatile uint32_t *)0x4004B034) // Pin Control Register n
#define PORTC_PCR14		(*(volatile uint32_t *)0x4004B038) // Pin Control Register n
#define PORTC_PCR15		(*(volatile uint32_t *)0x4004B03C) // Pin Control Register n
#define PORTC_PCR16		(*(volatile uint32_t *)0x4004B040) // Pin Control Register n
#define PORTC_PCR17		(*(volatile uint32_t *)0x4004B044) // Pin Control Register n
#define PORTC_PCR18		(*(volatile uint32_t *)0x4004B048) // Pin Control Register n
#define PORTC_PCR19		(*(volatile uint32_t *)0x4004B04C) // Pin Control Register n
#define PORTC_PCR20		(*(volatile uint32_t *)0x4004B050) // Pin Control Register n
#define PORTC_PCR21		(*(volatile uint32_t *)0x4004B054) // Pin Control Register n
#define PORTC_PCR22		(*(volatile uint32_t *)0x4004B058) // Pin Control Register n
#define PORTC_PCR23		(*(volatile uint32_t *)0x4004B05C) // Pin Control Register n
#define PORTC_PCR24		(*(volatile uint32_t *)0x4004B060) // Pin Control Register n
#define PORTC_PCR25		(*(volatile uint32_t *)0x4004B064) // Pin Control Register n
#define PORTC_PCR26		(*(volatile uint32_t *)0x4004B068) // Pin Control Register n
#define PORTC_PCR27		(*(volatile uint32_t *)0x4004B06C) // Pin Control Register n
#define PORTC_PCR28		(*(volatile uint32_t *)0x4004B070) // Pin Control Register n
#define PORTC_PCR29		(*(volatile uint32_t *)0x4004B074) // Pin Control Register n
#define PORTC_PCR30		(*(volatile uint32_t *)0x4004B078) // Pin Control Register n
#define PORTC_PCR31		(*(volatile uint32_t *)0x4004B07C) // Pin Control Register n
#define PORTC_GPCLR		(*(volatile uint32_t *)0x4004B080) // Global Pin Control Low Register
#define PORTC_GPCHR		(*(volatile uint32_t *)0x4004B084) // Global Pin Control High Register
#define PORTC_ISFR		(*(volatile uint32_t *)0x4004B0A0) // Interrupt Status Flag Register
#define PORTD_PCR0		(*(volatile uint32_t *)0x4004C000) // Pin Control Register n
#define PORTD_PCR1		(*(volatile uint32_t *)0x4004C004) // Pin Control Register n
#define PORTD_PCR2		(*(volatile uint32_t *)0x4004C008) // Pin Control Register n
#define PORTD_PCR3		(*(volatile uint32_t *)0x4004C00C) // Pin Control Register n
#define PORTD_PCR4		(*(volatile uint32_t *)0x4004C010) // Pin Control Register n
#define PORTD_PCR5		(*(volatile uint32_t *)0x4004C014) // Pin Control Register n
#define PORTD_PCR6		(*(volatile uint32_t *)0x4004C018) // Pin Control Register n
#define PORTD_PCR7		(*(volatile uint32_t *)0x4004C01C) // Pin Control Register n
#define PORTD_PCR8		(*(volatile uint32_t *)0x4004C020) // Pin Control Register n
#define PORTD_PCR9		(*(volatile uint32_t *)0x4004C024) // Pin Control Register n
#define PORTD_PCR10		(*(volatile uint32_t *)0x4004C028) // Pin Control Register n
#define PORTD_PCR11		(*(volatile uint32_t *)0x4004C02C) // Pin Control Register n
#define PORTD_PCR12		(*(volatile uint32_t *)0x4004C030) // Pin Control Register n
#define PORTD_PCR13		(*(volatile uint32_t *)0x4004C034) // Pin Control Register n
#define PORTD_PCR14		(*(volatile uint32_t *)0x4004C038) // Pin Control Register n
#define PORTD_PCR15		(*(volatile uint32_t *)0x4004C03C) // Pin Control Register n
#define PORTD_PCR16		(*(volatile uint32_t *)0x4004C040) // Pin Control Register n
#define PORTD_PCR17		(*(volatile uint32_t *)0x4004C044) // Pin Control Register n
#define PORTD_PCR18		(*(volatile uint32_t *)0x4004C048) // Pin Control Register n
#define PORTD_PCR19		(*(volatile uint32_t *)0x4004C04C) // Pin Control Register n
#define PORTD_PCR20		(*(volatile uint32_t *)0x4004C050) // Pin Control Register n
#define PORTD_PCR21		(*(volatile uint32_t *)0x4004C054) // Pin Control Register n
#define PORTD_PCR22		(*(volatile uint32_t *)0x4004C058) // Pin Control Register n
#define PORTD_PCR23		(*(volatile uint32_t *)0x4004C05C) // Pin Control Register n
#define PORTD_PCR24		(*(volatile uint32_t *)0x4004C060) // Pin Control Register n
#define PORTD_PCR25		(*(volatile uint32_t *)0x4004C064) // Pin Control Register n
#define PORTD_PCR26		(*(volatile uint32_t *)0x4004C068) // Pin Control Register n
#define PORTD_PCR27		(*(volatile uint32_t *)0x4004C06C) // Pin Control Register n
#define PORTD_PCR28		(*(volatile uint32_t *)0x4004C070) // Pin Control Register n
#define PORTD_PCR29		(*(volatile uint32_t *)0x4004C074) // Pin Control Register n
#define PORTD_PCR30		(*(volatile uint32_t *)0x4004C078) // Pin Control Register n
#define PORTD_PCR31		(*(volatile uint32_t *)0x4004C07C) // Pin Control Register n
#define PORTD_GPCLR		(*(volatile uint32_t *)0x4004C080) // Global Pin Control Low Register
#define PORTD_GPCHR		(*(volatile uint32_t *)0x4004C084) // Global Pin Control High Register
#define PORTD_ISFR		(*(volatile uint32_t *)0x4004C0A0) // Interrupt Status Flag Register
#define PORTE_PCR0		(*(volatile uint32_t *)0x4004D000) // Pin Control Register n
#define PORTE_PCR1		(*(volatile uint32_t *)0x4004D004) // Pin Control Register n
#define PORTE_PCR2		(*(volatile uint32_t *)0x4004D008) // Pin Control Register n
#define PORTE_PCR3		(*(volatile uint32_t *)0x4004D00C) // Pin Control Register n
#define PORTE_PCR4		(*(volatile uint32_t *)0x4004D010) // Pin Control Register n
#define PORTE_PCR5		(*(volatile uint32_t *)0x4004D014) // Pin Control Register n
#define PORTE_PCR6		(*(volatile uint32_t *)0x4004D018) // Pin Control Register n
#define PORTE_PCR7		(*(volatile uint32_t *)0x4004D01C) // Pin Control Register n
#define PORTE_PCR8		(*(volatile uint32_t *)0x4004D020) // Pin Control Register n
#define PORTE_PCR9		(*(volatile uint32_t *)0x4004D024) // Pin Control Register n
#define PORTE_PCR10		(*(volatile uint32_t *)0x4004D028) // Pin Control Register n
#define PORTE_PCR11		(*(volatile uint32_t *)0x4004D02C) // Pin Control Register n
#define PORTE_PCR12		(*(volatile uint32_t *)0x4004D030) // Pin Control Register n
#define PORTE_PCR13		(*(volatile uint32_t *)0x4004D034) // Pin Control Register n
#define PORTE_PCR14		(*(volatile uint32_t *)0x4004D038) // Pin Control Register n
#define PORTE_PCR15		(*(volatile uint32_t *)0x4004D03C) // Pin Control Register n
#define PORTE_PCR16		(*(volatile uint32_t *)0x4004D040) // Pin Control Register n
#define PORTE_PCR17		(*(volatile uint32_t *)0x4004D044) // Pin Control Register n
#define PORTE_PCR18		(*(volatile uint32_t *)0x4004D048) // Pin Control Register n
#define PORTE_PCR19		(*(volatile uint32_t *)0x4004D04C) // Pin Control Register n
#define PORTE_PCR20		(*(volatile uint32_t *)0x4004D050) // Pin Control Register n
#define PORTE_PCR21		(*(volatile uint32_t *)0x4004D054) // Pin Control Register n
#define PORTE_PCR22		(*(volatile uint32_t *)0x4004D058) // Pin Control Register n
#define PORTE_PCR23		(*(volatile uint32_t *)0x4004D05C) // Pin Control Register n
#define PORTE_PCR24		(*(volatile uint32_t *)0x4004D060) // Pin Control Register n
#define PORTE_PCR25		(*(volatile uint32_t *)0x4004D064) // Pin Control Register n
#define PORTE_PCR26		(*(volatile uint32_t *)0x4004D068) // Pin Control Register n
#define PORTE_PCR27		(*(volatile uint32_t *)0x4004D06C) // Pin Control Register n
#define PORTE_PCR28		(*(volatile uint32_t *)0x4004D070) // Pin Control Register n
#define PORTE_PCR29		(*(volatile uint32_t *)0x4004D074) // Pin Control Register n
#define PORTE_PCR30		(*(volatile uint32_t *)0x4004D078) // Pin Control Register n
#define PORTE_PCR31		(*(volatile uint32_t *)0x4004D07C) // Pin Control Register n
#define PORTE_GPCLR		(*(volatile uint32_t *)0x4004D080) // Global Pin Control Low Register
#define PORTE_GPCHR		(*(volatile uint32_t *)0x4004D084) // Global Pin Control High Register
#define PORTE_ISFR		(*(volatile uint32_t *)0x4004D0A0) // Interrupt Status Flag Register

// System Integration Module (SIM)

#define SIM_SOPT1		(*(volatile uint32_t *)0x40047000) // System Options Register 1
#define SIM_SOPT1_USBREGEN		((uint32_t)0x80000000)		// USB regulator enable
#define SIM_SOPT1_USBSSTBY		((uint32_t)0x40000000)		// USB regulator standby in Stop, VLPS, LLS and VLLS
#define SIM_SOPT1_USBVSTBY		((uint32_t)0x20000000)		// USB regulator standby in  VLPR and VLPW
#define SIM_SOPT1_OSC32KSEL(n)		((uint32_t)(((n) & 3) << 18))	// 32K oscillator clock, 0=system osc, 2=rtc osc, 3=lpo
#define SIM_SOPT1CFG		(*(volatile uint32_t *)0x40047004) // SOPT1 Configuration Register
#define SIM_SOPT1CFG_USSWE		((uint32_t)0x04000000)		// USB voltage regulator stop standby write enable
#define SIM_SOPT1CFG_UVSWE		((uint32_t)0x02000000)		// USB voltage regulator VLP standby write enable
#define SIM_SOPT1CFG_URWE		((uint32_t)0x01000000)		// USB voltage regulator enable write enable
#define SIM_USBPHYCTL		(*(volatile uint32_t *)0x40047008) // USB PHY Control Register
#define SIM_USBPHYCTL_USBDISILIM	((uint32_t)0x00800000)		// USB Disable Inrush Current Limit
#define SIM_USBPHYCTL_USB3VOUTTRG(n)	((uint32_t)(((n) & 7) << 20))	// USB 3.3V Output Target
#define SIM_USBPHYCTL_USBVREGPD		((uint32_t)0x00020000)		// Enables the pulldown on the output of the USB Regulator.
#define SIM_USBPHYCTL_USBVREGSEL	((uint32_t)0x00010000)		// Selects the default input voltage source
#define SIM_SOPT2		(*(volatile uint32_t *)0x40048004) // System Options Register 2
#define SIM_SOPT2_SDHCSRC(n)            (uint32_t)(((n) & 3) << 28)     // SDHC Clock, 0=system, 1=FLL/PLL, 2=OSCERCLK, 3=external
#define SIM_SOPT2_LPUARTSRC(n)          (uint32_t)(((n) & 3) << 26)     // LPUART Clock, 0=off, 1=FLL/PLL, 2=OSCERCLK, 3=MCGIRCLK
#define SIM_SOPT2_UART0SRC(n)           (uint32_t)(((n) & 3) << 26)     // UART0 Clock, 0=off, 1=FLL/PLL, 2=OSCERCLK, 3=MCGIRCLK
#define SIM_SOPT2_TPMSRC(n)             (uint32_t)(((n) & 3) << 24)     // TPM Clock, 0=off, 1=FLL/PLL, 2=OSCERCLK, 3=MCGIRCLK
#define SIM_SOPT2_TIMESRC(n)		(uint32_t)(((n) & 3) << 20)	// IEEE 1588 clock, 0=system, 1=FLL/PLL, 2=OSCERCLK, 3=external
#define SIM_SOPT2_RMIISRC		((uint32_t)0x00080000)		// 0=external, 1=external 1588
#define SIM_SOPT2_USBSRC		((uint32_t)0x00040000)		// 0=USB_CLKIN, 1=FFL/PLL
#define SIM_SOPT2_PLLFLLSEL		((uint32_t)0x00010000)		// 0=FLL, 1=PLL
#define SIM_SOPT2_IRC48SEL		((uint32_t)0x00030000)		// 0=FLL, 1=PLL
#define SIM_SOPT2_TRACECLKSEL		((uint32_t)0x00001000)		// 0=MCGOUTCLK, 1=CPU
#define SIM_SOPT2_PTD7PAD		((uint32_t)0x00000800)		// 0=normal, 1=double drive PTD7
#define SIM_SOPT2_FBSL(n)		((uint32_t)(((n) & 3) << 8))	// FlexBus security level
#define SIM_SOPT2_CLKOUTSEL(n)		((uint32_t)(((n) & 7) << 5))	// Selects the clock to output on the CLKOUT pin.
#define SIM_SOPT2_RTCCLKOUTSEL		((uint32_t)0x00000010)		// RTC clock out select
#define SIM_SOPT2_USBREGEN		((uint32_t)0x00000002)		// USB PHY PLL Regulator Enable
#define SIM_SOPT2_USBSLSRC		((uint32_t)0x00000001)		// USB Slow Clock Source
#define SIM_SOPT4		(*(volatile uint32_t *)0x4004800C) // System Options Register 4
#define SIM_SOPT4_FTM3TRG1SRC		((uint32_t)0x80000000)		// FlexTimer 3 Hardware Trigger 1 Source Select
#define SIM_SOPT4_FTM3TRG0SRC		((uint32_t)0x40000000)		// FlexTimer 3 Hardware Trigger 0 Source Select
#define SIM_SOPT4_FTM0TRG1SRC		((uint32_t)0x20000000)		// FlexTimer 0 Hardware Trigger 1 Source Select
#define SIM_SOPT4_FTM0TRG0SRC		((uint32_t)0x10000000)		// FlexTimer 0 Hardware Trigger 0 Source Select
#define SIM_SOPT4_FTM3CLKSEL		((uint32_t)0x08000000)		// FlexTimer 3 External Clock Pin Select
#define SIM_SOPT4_FTM2CLKSEL		((uint32_t)0x04000000)		// FlexTimer 2 External Clock Pin Select
#define SIM_SOPT4_FTM1CLKSEL		((uint32_t)0x02000000)		// FTM1 External Clock Pin Select
#define SIM_SOPT4_FTM0CLKSEL		((uint32_t)0x01000000)		// FlexTimer 0 External Clock Pin Select
#define SIM_SOPT4_FTM2CH1SRC		((uint32_t)0x00400000)		// FTM2 channel 1 input capture source select
#define SIM_SOPT4_FTM2CH0SRC(n)		((uint32_t)(((n) & 3) << 20))	// FTM2 channel 0 input capture source select
#define SIM_SOPT4_FTM1CH0SRC(n)		((uint32_t)(((n) & 3) << 18))	// FTM1 channel 0 input capture source select
#define SIM_SOPT4_FTM3FLT0		((uint32_t)0x00001000)		// FTM3 Fault 0 Select
#define SIM_SOPT4_FTM2FLT0		((uint32_t)0x00000100)		// FTM2 Fault 0 Select
#define SIM_SOPT4_FTM1FLT0		((uint32_t)0x00000010)		// FTM1 Fault 0 Select
#define SIM_SOPT4_FTM0FLT3		((uint32_t)0x00000008)		// FTM0 Fault 3 Select
#define SIM_SOPT4_FTM0FLT2		((uint32_t)0x00000004)		// FTM0 Fault 2 Select
#define SIM_SOPT4_FTM0FLT1		((uint32_t)0x00000002)		// FTM0 Fault 1 Select
#define SIM_SOPT4_FTM0FLT0		((uint32_t)0x00000001)		// FTM0 Fault 0 Select
#define SIM_SOPT5		(*(volatile uint32_t *)0x40048010) // System Options Register 5
#define SIM_SOPT5_LPUART0RXSRC(n)	(uint32_t)(((n) & 3) << 18)	// LPUART0 receive data source select
#define SIM_SOPT5_LPUART0TXSRC(n)	(uint32_t)(((n) & 3) << 16)	// LPUART0 transmit data source select
#define SIM_SOPT5_UART1RXSRC(n)		(uint32_t)(((n) & 3) << 6)	// UART 1 receive data source select
#define SIM_SOPT5_UART1TXSRC(n)		(uint32_t)(((n) & 3) << 4)	// UART 1 transmit data source select
#define SIM_SOPT5_UART0RXSRC(n)		(uint32_t)(((n) & 3) << 2)	// UART 0 receive data source select
#define SIM_SOPT5_UART0TXSRC(n)		(uint32_t)(((n) & 3) << 0)	// UART 0 transmit data source select
#define SIM_SOPT7		(*(volatile uint32_t *)0x40048018) // System Options Register 7
#define SIM_SOPT7_ADC1ALTTRGEN		((uint32_t)0x00008000)		// ADC1 alternate trigger enable
#define SIM_SOPT7_ADC1PRETRGSEL		((uint32_t)0x00001000)		// ADC1 pre-trigger select
#define SIM_SOPT7_ADC1TRGSEL(n)		(uint32_t)(((n) & 15) << 8)	// ADC1 trigger select
#define SIM_SOPT7_ADC0ALTTRGEN		((uint32_t)0x00000080)		// ADC0 alternate trigger enable
#define SIM_SOPT7_ADC0PRETRGSEL		((uint32_t)0x00000010)		// ADC0 pretrigger select
#define SIM_SOPT7_ADC0TRGSEL(n)		(uint32_t)(((n) & 15) << 0)	// ADC0 trigger select
#define SIM_SOPT8		(*(volatile uint32_t *)0x4004801C) // System Options Register 8
#define SIM_SOPT8_FTM3OCH7SRC		((uint32_t)0x80000000)		// FTM3 channel 7 output source
#define SIM_SOPT8_FTM3OCH6SRC		((uint32_t)0x40000000)		// FTM3 channel 6 output source
#define SIM_SOPT8_FTM3OCH5SRC		((uint32_t)0x20000000)		// FTM3 channel 5 output source
#define SIM_SOPT8_FTM3OCH4SRC		((uint32_t)0x10000000)		// FTM3 channel 4 output source
#define SIM_SOPT8_FTM3OCH3SRC		((uint32_t)0x08000000)		// FTM3 channel 3 output source
#define SIM_SOPT8_FTM3OCH2SRC		((uint32_t)0x04000000)		// FTM3 channel 2 output source
#define SIM_SOPT8_FTM3OCH1SRC		((uint32_t)0x02000000)		// FTM3 channel 1 output source
#define SIM_SOPT8_FTM3OCH0SRC		((uint32_t)0x01000000)		// FTM3 channel 0 output source
#define SIM_SOPT8_FTM0OCH7SRC		((uint32_t)0x00800000)		// FTM0 channel 7 output source
#define SIM_SOPT8_FTM0OCH6SRC		((uint32_t)0x00400000)		// FTM0 channel 6 output source
#define SIM_SOPT8_FTM0OCH5SRC		((uint32_t)0x00200000)		// FTM0 channel 5 output source
#define SIM_SOPT8_FTM0OCH4SRC		((uint32_t)0x00100000)		// FTM0 channel 4 output source
#define SIM_SOPT8_FTM0OCH3SRC		((uint32_t)0x00080000)		// FTM0 channel 3 output source
#define SIM_SOPT8_FTM0OCH2SRC		((uint32_t)0x00040000)		// FTM0 channel 2 output source
#define SIM_SOPT8_FTM0OCH1SRC		((uint32_t)0x00020000)		// FTM0 channel 1 output source
#define SIM_SOPT8_FTM0OCH0SRC		((uint32_t)0x00010000)		// FTM0 channel 0 output source
#define SIM_SOPT8_FTM3SYNCBIT		((uint32_t)0x00000008)		// FTM3 Hardware Trigger 0 Software Synchronization
#define SIM_SOPT8_FTM2SYNCBIT		((uint32_t)0x00000004)		// FTM2 Hardware Trigger 0 Software Synchronization
#define SIM_SOPT8_FTM1SYNCBIT		((uint32_t)0x00000002)		// FTM1 Hardware Trigger 0 Software Synchronization
#define SIM_SOPT8_FTM0SYNCBIT		((uint32_t)0x00000001)		// FTM0 Hardware Trigger 0 Software Synchronization
#define SIM_SOPT9		(*(volatile uint32_t *)0x40048020) // System Options Register 9
#define SIM_SOPT9_TPM2CLKSEL		((uint32_t)0x02000000)		// TPM2 External Clock Pin Select
#define SIM_SOPT9_TPM1CLKSEL		((uint32_t)0x01000000)		// TPM1 External Clock Pin Select
#define SIM_SOPT9_TPM2CH0SRC(n)		(uint32_t)(((n) & 3) << 20)	// TPM2 channel 0 input capture source select
#define SIM_SOPT9_TPM1CH0SRC(n)		(uint32_t)(((n) & 3) << 18)	// TPM1 channel 0 input capture source select
#define SIM_SDID		(*(const uint32_t *)0x40048024)    // System Device Identification Register
#define SIM_SCGC1		(*(volatile uint32_t *)0x40048028) // System Clock Gating Control Register 1
#define SIM_SCGC1_UART5			((uint32_t)0x00000800)		// UART5 Clock Gate Control
#define SIM_SCGC1_UART4			((uint32_t)0x00000400)		// UART4 Clock Gate Control
#define SIM_SCGC1_I2C3			((uint32_t)0x00000080)		// I2C3 Clock Gate Control
#define SIM_SCGC1_I2C2			((uint32_t)0x00000040)		// I2C2 Clock Gate Control
#define SIM_SCGC2		(*(volatile uint32_t *)0x4004802C) // System Clock Gating Control Register 2
#if defined(KINETISK)
#define SIM_SCGC2_DAC1			((uint32_t)0x00002000)		// DAC1 Clock Gate Control
#define SIM_SCGC2_DAC0			((uint32_t)0x00001000)		// DAC0 Clock on APIS1 (base addr 400CC000)
#define SIM_SCGC2_TPM2			((uint32_t)0x00000400)		// TPM2 Clock Gate Control
#define SIM_SCGC2_TPM1			((uint32_t)0x00000200)		// TPM1 Clock Gate Control
#define SIM_SCGC2_LPUART0		((uint32_t)0x00000010)		// LPUART0 Clock Gate Control
#define SIM_SCGC2_ENET			((uint32_t)0x00000001)		// Ethernet Clock Gate Control
#endif
#define SIM_SCGC3		(*(volatile uint32_t *)0x40048030) // System Clock Gating Control Register 3
#define SIM_SCGC3_ADC1			((uint32_t)0x08000000)		// ADC1 Clock Gate Control
#define SIM_SCGC3_FTM3			((uint32_t)0x02000000)		// FTM3 Clock Gate Control
#define SIM_SCGC3_FTM2			((uint32_t)0x01000000)		// FTM2 Clock on APIS1 (base addr 400B8000)
#define SIM_SCGC3_SDHC			((uint32_t)0x00020000)		// SDHC Clock Gate Control
#define SIM_SCGC3_SPI2			((uint32_t)0x00001000)		// SPI2 Clock Gate Control
#define SIM_SCGC3_FLEXCAN1		((uint32_t)0x00000010)		// FLEXCAN1 Clock Gate Control
#define SIM_SCGC3_USBHSDCD		((uint32_t)0x00000008)		// USBHSDCD Clock Gate Control
#define SIM_SCGC3_USBHSPHY		((uint32_t)0x00000004)		// USBHSPHY Clock Gate Control
#define SIM_SCGC3_USBHS			((uint32_t)0x00000002)		// USBHS Clock Gate Control
//#define SIM_SCGC3_RNGA		((uint32_t)0x00000001)		// RNGA Clock on APIS1 (base addr 400A0000)
#define SIM_SCGC4		(*(volatile uint32_t *)0x40048034) // System Clock Gating Control Register 4
#define SIM_SCGC4_VREF			((uint32_t)0x00100000)		// VREF Clock Gate Control
#define SIM_SCGC4_CMP			((uint32_t)0x00080000)		// Comparator Clock Gate Control
#define SIM_SCGC4_USBOTG		((uint32_t)0x00040000)		// USB Clock Gate Control
#define SIM_SCGC4_UART3			((uint32_t)0x00002000)		// UART3 Clock Gate Control
#define SIM_SCGC4_UART2			((uint32_t)0x00001000)		// UART2 Clock Gate Control
#define SIM_SCGC4_UART1			((uint32_t)0x00000800)		// UART1 Clock Gate Control
#define SIM_SCGC4_UART0			((uint32_t)0x00000400)		// UART0 Clock Gate Control
#define SIM_SCGC4_I2C1			((uint32_t)0x00000080)		// I2C1 Clock Gate Control
#define SIM_SCGC4_I2C0			((uint32_t)0x00000040)		// I2C0 Clock Gate Control
#define SIM_SCGC4_CMT			((uint32_t)0x00000004)		// CMT Clock Gate Control
#define SIM_SCGC4_EWM			((uint32_t)0x00000002)		// EWM Clock Gate Control
#ifdef KINETISL
#define SIM_SCGC4_SPI1			((uint32_t)0x00800000)		//
#define SIM_SCGC4_SPI0			((uint32_t)0x00400000)		//
#endif
#define SIM_SCGC5		(*(volatile uint32_t *)0x40048038) // System Clock Gating Control Register 5
#define SIM_SCGC5_PORTE			((uint32_t)0x00002000)		// Port E Clock Gate Control
#define SIM_SCGC5_PORTD			((uint32_t)0x00001000)		// Port D Clock Gate Control
#define SIM_SCGC5_PORTC			((uint32_t)0x00000800)		// Port C Clock Gate Control
#define SIM_SCGC5_PORTB			((uint32_t)0x00000400)		// Port B Clock Gate Control
#define SIM_SCGC5_PORTA			((uint32_t)0x00000200)		// Port A Clock Gate Control
#define SIM_SCGC5_TSI			((uint32_t)0x00000020)		// Touch Sense Input TSI Clock Gate Control
#define SIM_SCGC5_LPTIMER		((uint32_t)0x00000001)		// Low Power Timer Access Control
#define SIM_SCGC6		(*(volatile uint32_t *)0x4004803C) // System Clock Gating Control Register 6
#if defined(KINETISL)
#define SIM_SCGC6_DAC0			((uint32_t)0x80000000)		// DAC on Kinetis-L
#define SIM_SCGC6_TPM2			((uint32_t)0x04000000)		// FTM2 Clock Gate Control
#define SIM_SCGC6_TPM1			((uint32_t)0x02000000)		// FTM1 Clock Gate Control
#define SIM_SCGC6_TPM0			((uint32_t)0x01000000)		// FTM0 Clock Gate Control
#elif defined(KINETISK)
//#define SIM_SCGC6_DAC0		((uint32_t)0x80000000)		// DAC0 Clock on APIS0 (base addr 4003F000)
//#define SIM_SCGC6_FTM2		((uint32_t)0x04000000)		// FTM2 Clock on APIS0 (base addr 4003A000)
#define SIM_SCGC6_PDB			((uint32_t)0x00400000)		// PDB Clock Gate Control
#define SIM_SCGC6_USBDCD		((uint32_t)0x00200000)		// USB DCD Clock Gate Control
#define SIM_SCGC6_SPI1			((uint32_t)0x00002000)		// SPI1 Clock Gate Control
#define SIM_SCGC6_SPI0			((uint32_t)0x00001000)		// SPI0 Clock Gate Control
#define SIM_SCGC6_RNGA			((uint32_t)0x00000200)		// RNGA Clock on APIS0 (base addr 40029000)
#define SIM_SCGC6_FLEXCAN0		((uint32_t)0x00000010)		// FlexCAN0 Clock Gate Control
#define SIM_SCGC6_CRC			((uint32_t)0x00040000)		// CRC Clock Gate Control
#endif
#define SIM_SCGC6_RTC			((uint32_t)0x20000000)		// RTC Access
#define SIM_SCGC6_ADC0			((uint32_t)0x08000000)		// ADC0 Clock Gate Control
#define SIM_SCGC6_FTM1			((uint32_t)0x02000000)		// FTM1 Clock Gate Control
#define SIM_SCGC6_FTM0			((uint32_t)0x01000000)		// FTM0 Clock Gate Control
#define SIM_SCGC6_PIT			((uint32_t)0x00800000)		// PIT Clock Gate Control
#define SIM_SCGC6_I2S			((uint32_t)0x00008000)		// I2S Clock Gate Control
#define SIM_SCGC6_DMAMUX		((uint32_t)0x00000002)		// DMA Mux Clock Gate Control
#define SIM_SCGC6_FTFL			((uint32_t)0x00000001)		// Flash Memory Clock Gate Control
#define SIM_SCGC7		(*(volatile uint32_t *)0x40048040) // System Clock Gating Control Register 7
#if defined(KINETISK)
#define SIM_SCGC7_SDRAMC		((uint32_t)0x00000008)		// SDRAM Clock Gate Control
#define SIM_SCGC7_MPU			((uint32_t)0x00000004)		// MPU Clock Gate Control
#define SIM_SCGC7_DMA			((uint32_t)0x00000002)		// DMA Clock Gate Control
#define SIM_SCGC7_FLEXBUS		((uint32_t)0x00000001)		// FLEXBUS Clock Gate Control
#elif defined(KINETISL)
#define SIM_SCGC7_DMA			((uint32_t)0x00000100)		// DMA Clock Gate Control
#endif
#define SIM_CLKDIV1		(*(volatile uint32_t *)0x40048044) // System Clock Divider Register 1
#define SIM_CLKDIV1_OUTDIV1(n)		((uint32_t)(((n) & 0x0F) << 28)) // divide value for the core/system clock
#define SIM_CLKDIV1_OUTDIV2(n)		((uint32_t)(((n) & 0x0F) << 24)) // divide value for the peripheral clock
#define SIM_CLKDIV1_OUTDIV3(n)		((uint32_t)(((n) & 0x0F) << 20)) // divide value for the flexbus clock
#define SIM_CLKDIV1_OUTDIV4(n)		((uint32_t)(((n) & 0x0F) << 16)) // divide value for the flash clock
#define SIM_CLKDIV2		(*(volatile uint32_t *)0x40048048) // System Clock Divider Register 2
#define SIM_CLKDIV2_USBDIV(n)		((uint32_t)(((n) & 0x07) << 1))
#define SIM_CLKDIV2_USBFRAC		((uint32_t)0x01)
#define SIM_FCFG1		(*(const uint32_t *)0x4004804C)    // Flash Configuration Register 1
#define SIM_FCFG1_FLASHDOZE		((uint32_t)0x00000002)		// Flash Doze (disabled during wait)
#define SIM_FCFG1_FLASHDIS		((uint32_t)0x00000001)		// Flash Disable
#define SIM_FCFG2		(*(const uint32_t *)0x40048050)    // Flash Configuration Register 2
#define SIM_UIDH		(*(const uint32_t *)0x40048054)    // Unique Identification Register High
#define SIM_UIDMH		(*(const uint32_t *)0x40048058)    // Unique Identification Register Mid-High
#define SIM_UIDML		(*(const uint32_t *)0x4004805C)    // Unique Identification Register Mid Low
#define SIM_UIDL		(*(const uint32_t *)0x40048060)    // Unique Identification Register Low
#define SIM_CLKDIV3		(*(volatile uint32_t *)0x40048064) // System Clock Divider Register 3 (LPUART & TPM)
#define SIM_CLKDIV3_PLLFLLDIV(n)		((uint32_t)(((n) & 0x07) << 1))
#define SIM_CLKDIV3_PLLFLLFRAC		((uint32_t)0x01)
#define SIM_CLKDIV4		(*(volatile uint32_t *)0x40048068) // System Clock Divider Register 4 (Trace)
#define SIM_CLKDIV4_TRACEDIV(n)		((uint32_t)(((n) & 0x07) << 1))
#define SIM_CLKDIV4_TRACEFRAC		((uint32_t)0x01)
#if defined(KINETISL)
#define SIM_COPC                (*(volatile uint32_t *)0x40048100) // COP Control Register (SIM_COPC)
#define SIM_SRVCOP              (*(volatile uint32_t *)0x40048104) // Service COP Register (SIM_SRVCOP)
#endif

// Reset Control Module (RCM)

#define RCM_SRS0		(*(volatile uint8_t  *)0x4007F000) // System Reset Status Register 0
#define RCM_SRS0_POR			((uint8_t)0x80)
#define RCM_SRS0_PIN			((uint8_t)0x40)
#define RCM_SRS0_WDOG			((uint8_t)0x20)
#define RCM_SRS0_LOL			((uint8_t)0x08)
#define RCM_SRS0_LOC			((uint8_t)0x04)
#define RCM_SRS0_LVD			((uint8_t)0x02)
#define RCM_SRS0_WAKEUP			((uint8_t)0x01)
#define RCM_SRS1		(*(volatile uint8_t  *)0x4007F001) // System Reset Status Register 1
#define RCM_SRS1_SACKERR		((uint8_t)0x20)
#define RCM_SRS1_EZPT			((uint8_t)0x10)
#define RCM_SRS1_MDM_AP			((uint8_t)0x08)
#define RCM_SRS1_SW			((uint8_t)0x04)
#define RCM_SRS1_LOCKUP			((uint8_t)0x02)
#define RCM_SRS1_JTAG			((uint8_t)0x01)
#define RCM_RPFC		(*(volatile uint8_t  *)0x4007F004) // Reset Pin Filter Control Register
#define RCM_RPFW		(*(volatile uint8_t  *)0x4007F005) // Reset Pin Filter Width Register
#define RCM_MR			(*(volatile uint8_t  *)0x4007F007) // Mode Register
#define RCM_SSRS0		(*(volatile uint8_t  *)0x4007F008) // Sticky System Reset Status Register 0
#define RCM_SSRS1		(*(volatile uint8_t  *)0x4007F009) // Sticky System Reset Status Register 0

// System Mode Controller

#define SMC_PMPROT		(*(volatile uint8_t  *)0x4007E000) // Power Mode Protection Register
#define SMC_PMPROT_AHSRUN		((uint8_t)0x80)			// Allow high speed run mode
#define SMC_PMPROT_AVLP			((uint8_t)0x20)			// Allow very low power modes
#define SMC_PMPROT_ALLS			((uint8_t)0x08)			// Allow low leakage stop mode
#define SMC_PMPROT_AVLLS		((uint8_t)0x02)			// Allow very low leakage stop mode
#define SMC_PMCTRL		(*(volatile uint8_t  *)0x4007E001) // Power Mode Control Register
#define SMC_PMCTRL_LPWUI		((uint8_t)0x80)			// Low Power Wake Up on Interrupt
#define SMC_PMCTRL_RUNM(n)		((uint8_t)(((n) & 0x03) << 5))	// Run Mode Control
#define SMC_PMCTRL_STOPA		((uint8_t)0x08)			// Stop Aborted
#define SMC_PMCTRL_STOPM(n)		((uint8_t)((n) & 0x07))		// Stop Mode Control
#define SMC_VLLSCTRL		(*(volatile uint8_t  *)0x4007E002) // VLLS Control Register
#define SMC_VLLSCTRL_PORPO		((uint8_t)0x20)			// POR Power Option
#define SMC_VLLSCTRL_VLLSM(n)		((uint8_t)((n) & 0x07))		// VLLS Mode Control
#define SMC_PMSTAT		(*(volatile uint8_t  *)0x4007E003) // Power Mode Status Register
#define SMC_PMSTAT_RUN			((uint8_t)0x01)			// Current power mode is RUN
#define SMC_PMSTAT_STOP			((uint8_t)0x02)			// Current power mode is STOP
#define SMC_PMSTAT_VLPR			((uint8_t)0x04)			// Current power mode is VLPR
#define SMC_PMSTAT_VLPW			((uint8_t)0x08)			// Current power mode is VLPW
#define SMC_PMSTAT_VLPS			((uint8_t)0x10)			// Current power mode is VLPS
#define SMC_PMSTAT_LLS			((uint8_t)0x20)			// Current power mode is LLS
#define SMC_PMSTAT_VLLS			((uint8_t)0x40)			// Current power mode is VLLS
#define SMC_PMSTAT_HSRUN		((uint8_t)0x80)			// Current power mode is HSRUN

// Power Management Controller

#define PMC_LVDSC1		(*(volatile uint8_t  *)0x4007D000) // Low Voltage Detect Status And Control 1 register
#define PMC_LVDSC1_LVDF			((uint8_t)0x80)			// Low-Voltage Detect Flag
#define PMC_LVDSC1_LVDACK		((uint8_t)0x40)			// Low-Voltage Detect Acknowledge
#define PMC_LVDSC1_LVDIE		((uint8_t)0x20)			// Low-Voltage Detect Interrupt Enable
#define PMC_LVDSC1_LVDRE		((uint8_t)0x10)			// Low-Voltage Detect Reset Enable
#define PMC_LVDSC1_LVDV(n)		((uint8_t)((n) & 0x03))		// Low-Voltage Detect Voltage Select
#define PMC_LVDSC2		(*(volatile uint8_t  *)0x4007D001) // Low Voltage Detect Status And Control 2 register
#define PMC_LVDSC2_LVWF			((uint8_t)0x80)			// Low-Voltage Warning Flag
#define PMC_LVDSC2_LVWACK		((uint8_t)0x40)			// Low-Voltage Warning Acknowledge
#define PMC_LVDSC2_LVWIE		((uint8_t)0x20)			// Low-Voltage Warning Interrupt Enable
#define PMC_LVDSC2_LVWV(n)		((uint8_t)((n) & 0x03))		// Low-Voltage Warning Voltage Select
#define PMC_REGSC		(*(volatile uint8_t  *)0x4007D002) // Regulator Status And Control register
#define PMC_REGSC_BGEN			((uint8_t)0x10)			// Bandgap Enable In VLPx Operation
#define PMC_REGSC_ACKISO		((uint8_t)0x08)			// Acknowledge Isolation
#define PMC_REGSC_REGONS		((uint8_t)0x04)			// Regulator In Run Regulation Status
#define PMC_REGSC_BGBE			((uint8_t)0x01)			// Bandgap Buffer Enable

// Low-Leakage Wakeup Unit (LLWU)

#if defined(HAS_KINETIS_LLWU_32CH)
#define LLWU_PE1		(*(volatile uint8_t  *)0x4007C000) // LLWU Pin Enable 1 register
#define LLWU_PE1_WUPE0(n)       ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P0
#define LLWU_PE1_WUPE1(n)       ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P1
#define LLWU_PE1_WUPE2(n)       ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P2
#define LLWU_PE1_WUPE3(n)       ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P3
#define LLWU_PE2		(*(volatile uint8_t  *)0x4007C001) // LLWU Pin Enable 2 register
#define LLWU_PE2_WUPE4(n)       ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P4
#define LLWU_PE2_WUPE5(n)       ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P5
#define LLWU_PE2_WUPE6(n)       ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P6
#define LLWU_PE2_WUPE7(n)       ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P7
#define LLWU_PE3		(*(volatile uint8_t  *)0x4007C002) // LLWU Pin Enable 3 register
#define LLWU_PE3_WUPE8(n)       ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P8
#define LLWU_PE3_WUPE9(n)       ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P9
#define LLWU_PE3_WUPE10(n)      ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P10
#define LLWU_PE3_WUPE11(n)      ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P11
#define LLWU_PE4		(*(volatile uint8_t  *)0x4007C003) // LLWU Pin Enable 4 register
#define LLWU_PE4_WUPE12(n)      ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P12
#define LLWU_PE4_WUPE13(n)      ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P13
#define LLWU_PE4_WUPE14(n)      ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P14
#define LLWU_PE4_WUPE15(n)      ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P15
#define LLWU_PE5		(*(volatile uint8_t  *)0x4007C004) // LLWU Pin Enable 5 register
#define LLWU_PE5_WUPE19(n)      ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P19
#define LLWU_PE5_WUPE18(n)      ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P18
#define LLWU_PE5_WUPE17(n)      ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P17
#define LLWU_PE5_WUPE16(n)      ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P16
#define LLWU_PE6		(*(volatile uint8_t  *)0x4007C005) // LLWU Pin Enable 6 register
#define LLWU_PE6_WUPE23(n)      ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P23
#define LLWU_PE6_WUPE22(n)      ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P22
#define LLWU_PE6_WUPE21(n)      ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P21
#define LLWU_PE6_WUPE20(n)      ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P20
#define LLWU_PE7		(*(volatile uint8_t  *)0x4007C006) // LLWU Pin Enable 7 register
#define LLWU_PE7_WUPE27(n)      ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P27
#define LLWU_PE7_WUPE26(n)      ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P26
#define LLWU_PE7_WUPE25(n)      ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P25
#define LLWU_PE7_WUPE24(n)      ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P24
#define LLWU_PE8		(*(volatile uint8_t  *)0x4007C007) // LLWU Pin Enable 8 register
#define LLWU_PE8_WUPE31(n)      ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P31
#define LLWU_PE8_WUPE30(n)      ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P30
#define LLWU_PE8_WUPE29(n)      ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P29
#define LLWU_PE8_WUPE28(n)      ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P28
#define LLWU_ME			(*(volatile uint8_t  *)0x4007C008) // LLWU Module Enable register
#define LLWU_ME_WUME0           ((uint8_t)0x01) // Wakeup Module Enable For Module 0
#define LLWU_ME_WUME1           ((uint8_t)0x02) // Wakeup Module Enable For Module 1
#define LLWU_ME_WUME2           ((uint8_t)0x04) // Wakeup Module Enable For Module 2
#define LLWU_ME_WUME3           ((uint8_t)0x08) // Wakeup Module Enable For Module 3
#define LLWU_ME_WUME4           ((uint8_t)0x10) // Wakeup Module Enable For Module 4
#define LLWU_ME_WUME5           ((uint8_t)0x20) // Wakeup Module Enable For Module 5
#define LLWU_ME_WUME6           ((uint8_t)0x40) // Wakeup Module Enable For Module 6
#define LLWU_ME_WUME7           ((uint8_t)0x80) // Wakeup Module Enable For Module 7
#define LLWU_PF1		(*(volatile uint8_t  *)0x4007C009) // LLWU Pin Flag 1 register
#define LLWU_PF1_WUF0            ((uint8_t)0x01) // Wakeup Flag For LLWU_P0
#define LLWU_PF1_WUF1            ((uint8_t)0x02) // Wakeup Flag For LLWU_P1
#define LLWU_PF1_WUF2            ((uint8_t)0x04) // Wakeup Flag For LLWU_P2
#define LLWU_PF1_WUF3            ((uint8_t)0x08) // Wakeup Flag For LLWU_P3
#define LLWU_PF1_WUF4            ((uint8_t)0x10) // Wakeup Flag For LLWU_P4
#define LLWU_PF1_WUF5            ((uint8_t)0x20) // Wakeup Flag For LLWU_P5
#define LLWU_PF1_WUF6            ((uint8_t)0x40) // Wakeup Flag For LLWU_P6
#define LLWU_PF1_WUF7            ((uint8_t)0x80) // Wakeup Flag For LLWU_P7
#define LLWU_PF2		(*(volatile uint8_t  *)0x4007C00A) // LLWU Pin Flag 2 register
#define LLWU_PF2_WUF8            ((uint8_t)0x01) // Wakeup Flag For LLWU_P8
#define LLWU_PF2_WUF9            ((uint8_t)0x02) // Wakeup Flag For LLWU_P9
#define LLWU_PF2_WUF10           ((uint8_t)0x04) // Wakeup Flag For LLWU_P10
#define LLWU_PF2_WUF11           ((uint8_t)0x08) // Wakeup Flag For LLWU_P11
#define LLWU_PF2_WUF12           ((uint8_t)0x10) // Wakeup Flag For LLWU_P12
#define LLWU_PF2_WUF13           ((uint8_t)0x20) // Wakeup Flag For LLWU_P13
#define LLWU_PF2_WUF14           ((uint8_t)0x40) // Wakeup Flag For LLWU_P14
#define LLWU_PF2_WUF15           ((uint8_t)0x80) // Wakeup Flag For LLWU_P15
#define LLWU_PF3		(*(volatile uint8_t  *)0x4007C00B) // LLWU Pin Flag 3 register
#define LLWU_PF3_WUF16           ((uint8_t)0x01) // Wakeup Flag For LLWU_P16
#define LLWU_PF3_WUF17           ((uint8_t)0x02) // Wakeup Flag For LLWU_P17
#define LLWU_PF3_WUF18           ((uint8_t)0x04) // Wakeup Flag For LLWU_P18
#define LLWU_PF3_WUF19           ((uint8_t)0x08) // Wakeup Flag For LLWU_P19
#define LLWU_PF3_WUF20           ((uint8_t)0x10) // Wakeup Flag For LLWU_P20
#define LLWU_PF3_WUF21           ((uint8_t)0x20) // Wakeup Flag For LLWU_P21
#define LLWU_PF3_WUF22           ((uint8_t)0x40) // Wakeup Flag For LLWU_P22
#define LLWU_PF3_WUF23           ((uint8_t)0x80) // Wakeup Flag For LLWU_P23
#define LLWU_PF4		(*(volatile uint8_t  *)0x4007C00C) // LLWU Pin Flag 4 register
#define LLWU_PF4_WUF31           ((uint8_t)0x01) // Wakeup Flag For LLWU_P31
#define LLWU_PF4_WUF30           ((uint8_t)0x02) // Wakeup Flag For LLWU_P30
#define LLWU_PF4_WUF29           ((uint8_t)0x04) // Wakeup Flag For LLWU_P29
#define LLWU_PF4_WUF28           ((uint8_t)0x08) // Wakeup Flag For LLWU_P28
#define LLWU_PF4_WUF27           ((uint8_t)0x10) // Wakeup Flag For LLWU_P27
#define LLWU_PF4_WUF26           ((uint8_t)0x20) // Wakeup Flag For LLWU_P26
#define LLWU_PF4_WUF25           ((uint8_t)0x40) // Wakeup Flag For LLWU_P25
#define LLWU_PF4_WUF24           ((uint8_t)0x80) // Wakeup Flag For LLWU_P24
#define LLWU_MF5		(*(volatile uint8_t  *)0x4007C00D) // LLWU Module Flag 5 register
#define LLWU_MF5_MWUF0           ((uint8_t)0x01) // Wakeup flag For module 0
#define LLWU_MF5_MWUF1           ((uint8_t)0x02) // Wakeup flag For module 1
#define LLWU_MF5_MWUF2           ((uint8_t)0x04) // Wakeup flag For module 2
#define LLWU_MF5_MWUF3           ((uint8_t)0x08) // Wakeup flag For module 3
#define LLWU_MF5_MWUF4           ((uint8_t)0x10) // Wakeup flag For module 4
#define LLWU_MF5_MWUF5           ((uint8_t)0x20) // Wakeup flag For module 5
#define LLWU_MF5_MWUF6           ((uint8_t)0x40) // Wakeup flag For module 6
#define LLWU_MF5_MWUF7           ((uint8_t)0x80) // Wakeup flag For module 7
#define LLWU_FILT1		(*(volatile uint8_t  *)0x4007C00E) // LLWU Pin Filter 1 register
#define LLWU_FILT2		(*(volatile uint8_t  *)0x4007C00F) // LLWU Pin Filter 2 register
#define LLWU_FILT3		(*(volatile uint8_t  *)0x4007C010) // LLWU Pin Filter 3 register
#define LLWU_FILT4		(*(volatile uint8_t  *)0x4007C011) // LLWU Pin Filter 4 register
#elif defined(HAS_KINETIS_LLWU_16CH)
#define LLWU_PE1		(*(volatile uint8_t  *)0x4007C000) // LLWU Pin Enable 1 register
#define LLWU_PE1_WUPE0(n)       ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P0
#define LLWU_PE1_WUPE1(n)       ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P1
#define LLWU_PE1_WUPE2(n)       ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P2
#define LLWU_PE1_WUPE3(n)       ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P3
#define LLWU_PE2		(*(volatile uint8_t  *)0x4007C001) // LLWU Pin Enable 2 register
#define LLWU_PE2_WUPE4(n)       ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P4
#define LLWU_PE2_WUPE5(n)       ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P5
#define LLWU_PE2_WUPE6(n)       ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P6
#define LLWU_PE2_WUPE7(n)       ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P7
#define LLWU_PE3		(*(volatile uint8_t  *)0x4007C002) // LLWU Pin Enable 3 register
#define LLWU_PE3_WUPE8(n)       ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P8
#define LLWU_PE3_WUPE9(n)       ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P9
#define LLWU_PE3_WUPE10(n)      ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P10
#define LLWU_PE3_WUPE11(n)      ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P11
#define LLWU_PE4		(*(volatile uint8_t  *)0x4007C003) // LLWU Pin Enable 4 register
#define LLWU_PE4_WUPE12(n)      ((uint8_t)((n) & 0x03)) // Wakeup Pin Enable For LLWU_P12
#define LLWU_PE4_WUPE13(n)      ((uint8_t)(((n) & 0x03) << 2)) // Wakeup Pin Enable For LLWU_P13
#define LLWU_PE4_WUPE14(n)      ((uint8_t)(((n) & 0x03) << 4)) // Wakeup Pin Enable For LLWU_P14
#define LLWU_PE4_WUPE15(n)      ((uint8_t)(((n) & 0x03) << 6)) // Wakeup Pin Enable For LLWU_P15
#define LLWU_ME			(*(volatile uint8_t  *)0x4007C004) // LLWU Module Enable register
#define LLWU_ME_WUME0           ((uint8_t)0x01) // Wakeup Module Enable For Module 0
#define LLWU_ME_WUME1           ((uint8_t)0x02) // Wakeup Module Enable For Module 1
#define LLWU_ME_WUME2           ((uint8_t)0x04) // Wakeup Module Enable For Module 2
#define LLWU_ME_WUME3           ((uint8_t)0x08) // Wakeup Module Enable For Module 3
#define LLWU_ME_WUME4           ((uint8_t)0x10) // Wakeup Module Enable For Module 4
#define LLWU_ME_WUME5           ((uint8_t)0x20) // Wakeup Module Enable For Module 5
#define LLWU_ME_WUME6           ((uint8_t)0x40) // Wakeup Module Enable For Module 6
#define LLWU_ME_WUME7           ((uint8_t)0x80) // Wakeup Module Enable For Module 7
#define LLWU_F1			(*(volatile uint8_t  *)0x4007C005) // LLWU Flag 1 register
#define LLWU_F1_WUF0            ((uint8_t)0x01) // Wakeup Flag For LLWU_P0
#define LLWU_F1_WUF1            ((uint8_t)0x02) // Wakeup Flag For LLWU_P1
#define LLWU_F1_WUF2            ((uint8_t)0x04) // Wakeup Flag For LLWU_P2
#define LLWU_F1_WUF3            ((uint8_t)0x08) // Wakeup Flag For LLWU_P3
#define LLWU_F1_WUF4            ((uint8_t)0x10) // Wakeup Flag For LLWU_P4
#define LLWU_F1_WUF5            ((uint8_t)0x20) // Wakeup Flag For LLWU_P5
#define LLWU_F1_WUF6            ((uint8_t)0x40) // Wakeup Flag For LLWU_P6
#define LLWU_F1_WUF7            ((uint8_t)0x80) // Wakeup Flag For LLWU_P7
#define LLWU_F2			(*(volatile uint8_t  *)0x4007C006) // LLWU Flag 2 register
#define LLWU_F2_WUF8            ((uint8_t)0x01) // Wakeup Flag For LLWU_P8
#define LLWU_F2_WUF9            ((uint8_t)0x02) // Wakeup Flag For LLWU_P9
#define LLWU_F2_WUF10           ((uint8_t)0x04) // Wakeup Flag For LLWU_P10
#define LLWU_F2_WUF11           ((uint8_t)0x08) // Wakeup Flag For LLWU_P11
#define LLWU_F2_WUF12           ((uint8_t)0x10) // Wakeup Flag For LLWU_P12
#define LLWU_F2_WUF13           ((uint8_t)0x20) // Wakeup Flag For LLWU_P13
#define LLWU_F2_WUF14           ((uint8_t)0x40) // Wakeup Flag For LLWU_P14
#define LLWU_F2_WUF15           ((uint8_t)0x80) // Wakeup Flag For LLWU_P15
#define LLWU_F3			(*(volatile uint8_t  *)0x4007C007) // LLWU Flag 3 register
#define LLWU_F3_MWUF0           ((uint8_t)0x01) // Wakeup flag For module 0
#define LLWU_F3_MWUF1           ((uint8_t)0x02) // Wakeup flag For module 1
#define LLWU_F3_MWUF2           ((uint8_t)0x04) // Wakeup flag For module 2
#define LLWU_F3_MWUF3           ((uint8_t)0x08) // Wakeup flag For module 3
#define LLWU_F3_MWUF4           ((uint8_t)0x10) // Wakeup flag For module 4
#define LLWU_F3_MWUF5           ((uint8_t)0x20) // Wakeup flag For module 5
#define LLWU_F3_MWUF6           ((uint8_t)0x40) // Wakeup flag For module 6
#define LLWU_F3_MWUF7           ((uint8_t)0x80) // Wakeup flag For module 7
#define LLWU_FILT1		(*(volatile uint8_t  *)0x4007C008) // LLWU Pin Filter 1 register
#define LLWU_FILT2		(*(volatile uint8_t  *)0x4007C009) // LLWU Pin Filter 2 register
#define LLWU_RST		(*(volatile uint8_t  *)0x4007C00A) // LLWU Reset Enable register
#endif

// Miscellaneous Control Module (MCM)

#if defined(KINETISK)
#define MCM_PLASC		(*(volatile uint16_t *)0xE0080008) // Crossbar Switch (AXBS) Slave Configuration
#define MCM_PLAMC		(*(volatile uint16_t *)0xE008000A) // Crossbar Switch (AXBS) Master Configuration
#define MCM_PLACR		(*(volatile uint32_t *)0xE008000C) // Crossbar Switch (AXBS) Control Register (MK20DX128)
#define MCM_PLACR_ARG			((uint32_t)0x00000200)		// Arbitration select, 0=fixed, 1=round-robin
#define MCM_CR			(*(volatile uint32_t *)0xE008000C) // RAM arbitration control register (MK20DX256)
#define MCM_CR_SRAMLWP			((uint32_t)0x40000000)		// SRAM_L write protect
#define MCM_CR_SRAMLAP(n)		((uint32_t)(((n) & 0x03) << 28)) // SRAM_L priority, 0=RR, 1=favor DMA, 2=CPU, 3=DMA
#define MCM_CR_SRAMUWP			((uint32_t)0x04000000)		// SRAM_U write protect
#define MCM_CR_SRAMUAP(n)		((uint32_t)(((n) & 0x03) << 24)) // SRAM_U priority, 0=RR, 1=favor DMA, 2=CPU, 3=DMA
#define MCM_ISCR		(*(volatile uint32_t *)0xE0080010) // Interrupt Status Register
#define MCM_ETBCC		(*(volatile uint32_t *)0xE0080014) // ETB Counter Control register
#define MCM_ETBRL		(*(volatile uint32_t *)0xE0080018) // ETB Reload register
#define MCM_ETBCNT		(*(volatile uint32_t *)0xE008001C) // ETB Counter Value register
#define MCM_FADR		(*(volatile uint32_t *)0xE0080020) // Fault address register
#define MCM_FATR		(*(volatile uint32_t *)0xE0080024) // Fault attributes register
#define MCM_FDR			(*(volatile uint32_t *)0xE0080028) // Fault data register
#define MCM_PID			(*(volatile uint32_t *)0xE0080030) // Process ID register
#define MCM_CPO			(*(volatile uint32_t *)0xE0080040) // Compute Operation Control Register
#elif defined(KINETISL)
#define MCM_PLASC               (*(volatile uint16_t *)0xF0003008) // Crossbar Switch (AXBS) Slave Configuration
#define MCM_PLAMC               (*(volatile uint16_t *)0xF000300A) // Crossbar Switch (AXBS) Master Configuration
#define MCM_PLACR               (*(volatile uint32_t *)0xF000300C) // Platform Control Register
#define MCM_PLACR_ESFC			((uint32_t)0x00010000)		// Enable Stalling Flash Controller
#define MCM_PLACR_DFCS			((uint32_t)0x00008000)		// Disable Flash Controller Speculation
#define MCM_PLACR_EFDS			((uint32_t)0x00004000)		// Enable Flash Data Speculation
#define MCM_PLACR_DFCC			((uint32_t)0x00002000)		// Disable Flash Controller Cache
#define MCM_PLACR_DFCIC			((uint32_t)0x00001000)		// Disable Flash Controller Instruction Caching
#define MCM_PLACR_DFCDA			((uint32_t)0x00000800)		// Disable Flash Controller Data Caching
#define MCM_PLACR_CFCC			((uint32_t)0x00000400)		// Clear Flash Controller Cache
#define MCM_PLACR_ARB			((uint32_t)0x00000200)		// Arbitration select
#define MCM_CPO                 (*(volatile uint32_t *)0xF0003040) // Compute Operation Control Register
#endif

// Crossbar Switch (AXBS) - not programmable on MK20DX128 & Kinetis-L

#define AXBS_PRS0		(*(volatile uint32_t *)0x40004000) // Priority Registers Slave 0
#define AXBS_CRS0		(*(volatile uint32_t *)0x40004010) // Control Register 0
#define AXBS_PRS1		(*(volatile uint32_t *)0x40004100) // Priority Registers Slave 1
#define AXBS_CRS1		(*(volatile uint32_t *)0x40004110) // Control Register 1
#define AXBS_PRS2		(*(volatile uint32_t *)0x40004200) // Priority Registers Slave 2
#define AXBS_CRS2		(*(volatile uint32_t *)0x40004210) // Control Register 2
#define AXBS_PRS3		(*(volatile uint32_t *)0x40004300) // Priority Registers Slave 3
#define AXBS_CRS3		(*(volatile uint32_t *)0x40004310) // Control Register 3
#define AXBS_PRS4		(*(volatile uint32_t *)0x40004400) // Priority Registers Slave 4
#define AXBS_CRS4		(*(volatile uint32_t *)0x40004410) // Control Register 4
#define AXBS_PRS5		(*(volatile uint32_t *)0x40004500) // Priority Registers Slave 5
#define AXBS_CRS5		(*(volatile uint32_t *)0x40004510) // Control Register 5
#define AXBS_PRS6		(*(volatile uint32_t *)0x40004600) // Priority Registers Slave 6
#define AXBS_CRS6		(*(volatile uint32_t *)0x40004610) // Control Register 6
#define AXBS_PRS7		(*(volatile uint32_t *)0x40004700) // Priority Registers Slave 7
#define AXBS_CRS7		(*(volatile uint32_t *)0x40004710) // Control Register 7
#define AXBS_MGPCR0		(*(volatile uint32_t *)0x40004800) // Master 0 General Purpose Control Register
#define AXBS_MGPCR1		(*(volatile uint32_t *)0x40004900) // Master 1 General Purpose Control Register
#define AXBS_MGPCR2		(*(volatile uint32_t *)0x40004A00) // Master 2 General Purpose Control Register
#define AXBS_MGPCR3		(*(volatile uint32_t *)0x40004B00) // Master 3 General Purpose Control Register
#define AXBS_MGPCR4		(*(volatile uint32_t *)0x40004C00) // Master 4 General Purpose Control Register
#define AXBS_MGPCR5		(*(volatile uint32_t *)0x40004D00) // Master 5 General Purpose Control Register
#define AXBS_MGPCR6		(*(volatile uint32_t *)0x40004E00) // Master 6 General Purpose Control Register
#define AXBS_MGPCR7		(*(volatile uint32_t *)0x40004F00) // Master 7 General Purpose Control Register
#define AXBS_CRS_READONLY		((uint32_t)0x80000000)
#define AXBS_CRS_HALTLOWPRIORITY	((uint32_t)0x40000000)
#define AXBS_CRS_ARB_FIXED		((uint32_t)0x00000000)
#define AXBS_CRS_ARB_ROUNDROBIN		((uint32_t)0x00010000)
#define AXBS_CRS_PARK_FIXED		((uint32_t)0x00000000)
#define AXBS_CRS_PARK_PREVIOUS		((uint32_t)0x00000010)
#define AXBS_CRS_PARK_NONE		((uint32_t)0x00000020)
#define AXBS_CRS_PARK(n)		((uint32_t)(((n) & 7) << 0))

// Peripheral Bridge (AIPS-Lite)

#define AIPS0_MPRA		(*(volatile uint32_t *)0x40000000) // Master Privilege Register A
#define AIPS0_PACRA		(*(volatile uint32_t *)0x40000020) // Peripheral Access Control Register
#define AIPS0_PACRB		(*(volatile uint32_t *)0x40000024) // Peripheral Access Control Register
#define AIPS0_PACRC		(*(volatile uint32_t *)0x40000028) // Peripheral Access Control Register
#define AIPS0_PACRD		(*(volatile uint32_t *)0x4000002C) // Peripheral Access Control Register
#define AIPS0_PACRE		(*(volatile uint32_t *)0x40000040) // Peripheral Access Control Register
#define AIPS0_PACRF		(*(volatile uint32_t *)0x40000044) // Peripheral Access Control Register
#define AIPS0_PACRG		(*(volatile uint32_t *)0x40000048) // Peripheral Access Control Register
#define AIPS0_PACRH		(*(volatile uint32_t *)0x4000004C) // Peripheral Access Control Register
#define AIPS0_PACRI		(*(volatile uint32_t *)0x40000050) // Peripheral Access Control Register
#define AIPS0_PACRJ		(*(volatile uint32_t *)0x40000054) // Peripheral Access Control Register
#define AIPS0_PACRK		(*(volatile uint32_t *)0x40000058) // Peripheral Access Control Register
#define AIPS0_PACRL		(*(volatile uint32_t *)0x4000005C) // Peripheral Access Control Register
#define AIPS0_PACRM		(*(volatile uint32_t *)0x40000060) // Peripheral Access Control Register
#define AIPS0_PACRN		(*(volatile uint32_t *)0x40000064) // Peripheral Access Control Register
#define AIPS0_PACRO		(*(volatile uint32_t *)0x40000068) // Peripheral Access Control Register
#define AIPS0_PACRP		(*(volatile uint32_t *)0x4000006C) // Peripheral Access Control Register
#define AIPS1_MPRA		(*(volatile uint32_t *)0x40080000) // Master Privilege Register A
#define AIPS1_PACRA		(*(volatile uint32_t *)0x40080020) // Peripheral Access Control Register
#define AIPS1_PACRB		(*(volatile uint32_t *)0x40080024) // Peripheral Access Control Register
#define AIPS1_PACRC		(*(volatile uint32_t *)0x40080028) // Peripheral Access Control Register
#define AIPS1_PACRD		(*(volatile uint32_t *)0x4008002C) // Peripheral Access Control Register
#define AIPS1_PACRE		(*(volatile uint32_t *)0x40080040) // Peripheral Access Control Register
#define AIPS1_PACRF		(*(volatile uint32_t *)0x40080044) // Peripheral Access Control Register
#define AIPS1_PACRG		(*(volatile uint32_t *)0x40080048) // Peripheral Access Control Register
#define AIPS1_PACRH		(*(volatile uint32_t *)0x4008004C) // Peripheral Access Control Register
#define AIPS1_PACRI		(*(volatile uint32_t *)0x40080050) // Peripheral Access Control Register
#define AIPS1_PACRJ		(*(volatile uint32_t *)0x40080054) // Peripheral Access Control Register
#define AIPS1_PACRK		(*(volatile uint32_t *)0x40080058) // Peripheral Access Control Register
#define AIPS1_PACRL		(*(volatile uint32_t *)0x4008005C) // Peripheral Access Control Register
#define AIPS1_PACRM		(*(volatile uint32_t *)0x40080060) // Peripheral Access Control Register
#define AIPS1_PACRN		(*(volatile uint32_t *)0x40080064) // Peripheral Access Control Register
#define AIPS1_PACRO		(*(volatile uint32_t *)0x40080068) // Peripheral Access Control Register
#define AIPS1_PACRP		(*(volatile uint32_t *)0x4008006C) // Peripheral Access Control Register

// Memory Protection Unit (MPU)

#if defined(HAS_KINETIS_MPU)
#define MPU_CESR		(*(volatile uint32_t *)0x4000D000) // Control/Error Status Register
#define MPU_EAR0		(*(volatile uint32_t *)0x4000D010) // Error Address Register, slave port 0
#define MPU_EDR0		(*(volatile uint32_t *)0x4000D014) // Error Detail Register, slave port 0
#define MPU_EAR1		(*(volatile uint32_t *)0x4000D018) // Error Address Register, slave port 1
#define MPU_EDR1		(*(volatile uint32_t *)0x4000D01C) // Error Detail Register, slave port 1
#define MPU_EAR2		(*(volatile uint32_t *)0x4000D020) // Error Address Register, slave port 2
#define MPU_EDR2		(*(volatile uint32_t *)0x4000D024) // Error Detail Register, slave port 2
#define MPU_EAR3		(*(volatile uint32_t *)0x4000D028) // Error Address Register, slave port 3
#define MPU_EDR3		(*(volatile uint32_t *)0x4000D02C) // Error Detail Register, slave port 3
#define MPU_EAR4		(*(volatile uint32_t *)0x4000D030) // Error Address Register, slave port 4
#define MPU_EDR4		(*(volatile uint32_t *)0x4000D034) // Error Detail Register, slave port 4
#define MPU_RGD0_WORD0		(*(volatile uint32_t *)0x4000D400) // Region Descriptor 0, Word 0
#define MPU_RGD0_WORD1		(*(volatile uint32_t *)0x4000D404) // Region Descriptor 0, Word 1
#define MPU_RGD0_WORD2		(*(volatile uint32_t *)0x4000D408) // Region Descriptor 0, Word 2
#define MPU_RGD0_WORD3		(*(volatile uint32_t *)0x4000D40C) // Region Descriptor 0, Word 3
#define MPU_RGD1_WORD0		(*(volatile uint32_t *)0x4000D410) // Region Descriptor 1, Word 0
#define MPU_RGD1_WORD1		(*(volatile uint32_t *)0x4000D414) // Region Descriptor 1, Word 1
#define MPU_RGD1_WORD2		(*(volatile uint32_t *)0x4000D418) // Region Descriptor 1, Word 2
#define MPU_RGD1_WORD3		(*(volatile uint32_t *)0x4000D41C) // Region Descriptor 1, Word 3
#define MPU_RGD2_WORD0		(*(volatile uint32_t *)0x4000D420) // Region Descriptor 2, Word 0
#define MPU_RGD2_WORD1		(*(volatile uint32_t *)0x4000D424) // Region Descriptor 2, Word 1
#define MPU_RGD2_WORD2		(*(volatile uint32_t *)0x4000D428) // Region Descriptor 2, Word 2
#define MPU_RGD2_WORD3		(*(volatile uint32_t *)0x4000D42C) // Region Descriptor 2, Word 3
#define MPU_RGD3_WORD0		(*(volatile uint32_t *)0x4000D430) // Region Descriptor 3, Word 0
#define MPU_RGD3_WORD1		(*(volatile uint32_t *)0x4000D434) // Region Descriptor 3, Word 1
#define MPU_RGD3_WORD2		(*(volatile uint32_t *)0x4000D438) // Region Descriptor 3, Word 2
#define MPU_RGD3_WORD3		(*(volatile uint32_t *)0x4000D43C) // Region Descriptor 3, Word 3
#define MPU_RGD4_WORD0		(*(volatile uint32_t *)0x4000D440) // Region Descriptor 4, Word 0
#define MPU_RGD4_WORD1		(*(volatile uint32_t *)0x4000D444) // Region Descriptor 4, Word 1
#define MPU_RGD4_WORD2		(*(volatile uint32_t *)0x4000D448) // Region Descriptor 4, Word 2
#define MPU_RGD4_WORD3		(*(volatile uint32_t *)0x4000D44C) // Region Descriptor 4, Word 3
#define MPU_RGD5_WORD0		(*(volatile uint32_t *)0x4000D450) // Region Descriptor 5, Word 0
#define MPU_RGD5_WORD1		(*(volatile uint32_t *)0x4000D454) // Region Descriptor 5, Word 1
#define MPU_RGD5_WORD2		(*(volatile uint32_t *)0x4000D458) // Region Descriptor 5, Word 2
#define MPU_RGD5_WORD3		(*(volatile uint32_t *)0x4000D45C) // Region Descriptor 5, Word 3
#define MPU_RGD6_WORD0		(*(volatile uint32_t *)0x4000D460) // Region Descriptor 6, Word 0
#define MPU_RGD6_WORD1		(*(volatile uint32_t *)0x4000D464) // Region Descriptor 6, Word 1
#define MPU_RGD6_WORD2		(*(volatile uint32_t *)0x4000D468) // Region Descriptor 6, Word 2
#define MPU_RGD6_WORD3		(*(volatile uint32_t *)0x4000D46C) // Region Descriptor 6, Word 3
#define MPU_RGD7_WORD0		(*(volatile uint32_t *)0x4000D470) // Region Descriptor 7, Word 0
#define MPU_RGD7_WORD1		(*(volatile uint32_t *)0x4000D474) // Region Descriptor 7, Word 1
#define MPU_RGD7_WORD2		(*(volatile uint32_t *)0x4000D478) // Region Descriptor 7, Word 2
#define MPU_RGD7_WORD3		(*(volatile uint32_t *)0x4000D47C) // Region Descriptor 7, Word 3
#define MPU_RGD8_WORD0		(*(volatile uint32_t *)0x4000D480) // Region Descriptor 8, Word 0
#define MPU_RGD8_WORD1		(*(volatile uint32_t *)0x4000D484) // Region Descriptor 8, Word 1
#define MPU_RGD8_WORD2		(*(volatile uint32_t *)0x4000D488) // Region Descriptor 8, Word 2
#define MPU_RGD8_WORD3		(*(volatile uint32_t *)0x4000D48C) // Region Descriptor 8, Word 3
#define MPU_RGD9_WORD0		(*(volatile uint32_t *)0x4000D490) // Region Descriptor 9, Word 0
#define MPU_RGD9_WORD1		(*(volatile uint32_t *)0x4000D494) // Region Descriptor 9, Word 1
#define MPU_RGD9_WORD2		(*(volatile uint32_t *)0x4000D498) // Region Descriptor 9, Word 2
#define MPU_RGD9_WORD3		(*(volatile uint32_t *)0x4000D49C) // Region Descriptor 9, Word 3
#define MPU_RGD10_WORD0		(*(volatile uint32_t *)0x4000D4A0) // Region Descriptor 10, Word 0
#define MPU_RGD10_WORD1		(*(volatile uint32_t *)0x4000D4A4) // Region Descriptor 10, Word 1
#define MPU_RGD10_WORD2		(*(volatile uint32_t *)0x4000D4A8) // Region Descriptor 10, Word 2
#define MPU_RGD10_WORD3		(*(volatile uint32_t *)0x4000D4AC) // Region Descriptor 10, Word 3
#define MPU_RGD11_WORD0		(*(volatile uint32_t *)0x4000D4B0) // Region Descriptor 11, Word 0
#define MPU_RGD11_WORD1		(*(volatile uint32_t *)0x4000D4B4) // Region Descriptor 11, Word 1
#define MPU_RGD11_WORD2		(*(volatile uint32_t *)0x4000D4B8) // Region Descriptor 11, Word 2
#define MPU_RGD11_WORD3		(*(volatile uint32_t *)0x4000D4BC) // Region Descriptor 11, Word 3
#define MPU_RGDAAC0		(*(volatile uint32_t *)0x4000D800) // Region Descriptor Alternate Access Control 0
#define MPU_RGDAAC1		(*(volatile uint32_t *)0x4000D804) // Region Descriptor Alternate Access Control 1
#define MPU_RGDAAC2		(*(volatile uint32_t *)0x4000D808) // Region Descriptor Alternate Access Control 2
#define MPU_RGDAAC3		(*(volatile uint32_t *)0x4000D80C) // Region Descriptor Alternate Access Control 3
#define MPU_RGDAAC4		(*(volatile uint32_t *)0x4000D810) // Region Descriptor Alternate Access Control 4
#define MPU_RGDAAC5		(*(volatile uint32_t *)0x4000D814) // Region Descriptor Alternate Access Control 5
#define MPU_RGDAAC6		(*(volatile uint32_t *)0x4000D818) // Region Descriptor Alternate Access Control 6
#define MPU_RGDAAC7		(*(volatile uint32_t *)0x4000D81C) // Region Descriptor Alternate Access Control 7
#define MPU_RGDAAC8		(*(volatile uint32_t *)0x4000D820) // Region Descriptor Alternate Access Control 8
#define MPU_RGDAAC9		(*(volatile uint32_t *)0x4000D824) // Region Descriptor Alternate Access Control 9
#define MPU_RGDAAC10		(*(volatile uint32_t *)0x4000D828) // Region Descriptor Alternate Access Control 10
#define MPU_RGDAAC11		(*(volatile uint32_t *)0x4000D82C) // Region Descriptor Alternate Access Control 11
#endif

// Direct Memory Access Multiplexer (DMAMUX)

#if DMA_NUM_CHANNELS >= 4
#define DMAMUX0_CHCFG0		(*(volatile uint8_t  *)0x40021000) // Channel Configuration register
#define DMAMUX0_CHCFG1		(*(volatile uint8_t  *)0x40021001) // Channel Configuration register
#define DMAMUX0_CHCFG2		(*(volatile uint8_t  *)0x40021002) // Channel Configuration register
#define DMAMUX0_CHCFG3		(*(volatile uint8_t  *)0x40021003) // Channel Configuration register
#endif
#if DMA_NUM_CHANNELS >= 16
#define DMAMUX0_CHCFG4		(*(volatile uint8_t  *)0x40021004) // Channel Configuration register
#define DMAMUX0_CHCFG5		(*(volatile uint8_t  *)0x40021005) // Channel Configuration register
#define DMAMUX0_CHCFG6		(*(volatile uint8_t  *)0x40021006) // Channel Configuration register
#define DMAMUX0_CHCFG7		(*(volatile uint8_t  *)0x40021007) // Channel Configuration register
#define DMAMUX0_CHCFG8		(*(volatile uint8_t  *)0x40021008) // Channel Configuration register
#define DMAMUX0_CHCFG9		(*(volatile uint8_t  *)0x40021009) // Channel Configuration register
#define DMAMUX0_CHCFG10		(*(volatile uint8_t  *)0x4002100A) // Channel Configuration register
#define DMAMUX0_CHCFG11		(*(volatile uint8_t  *)0x4002100B) // Channel Configuration register
#define DMAMUX0_CHCFG12		(*(volatile uint8_t  *)0x4002100C) // Channel Configuration register
#define DMAMUX0_CHCFG13		(*(volatile uint8_t  *)0x4002100D) // Channel Configuration register
#define DMAMUX0_CHCFG14		(*(volatile uint8_t  *)0x4002100E) // Channel Configuration register
#define DMAMUX0_CHCFG15		(*(volatile uint8_t  *)0x4002100F) // Channel Configuration register
#endif
#if DMA_NUM_CHANNELS >= 32
#define DMAMUX0_CHCFG16		(*(volatile uint8_t  *)0x40021010) // Channel Configuration register
#define DMAMUX0_CHCFG17		(*(volatile uint8_t  *)0x40021011) // Channel Configuration register
#define DMAMUX0_CHCFG18		(*(volatile uint8_t  *)0x40021012) // Channel Configuration register
#define DMAMUX0_CHCFG19		(*(volatile uint8_t  *)0x40021013) // Channel Configuration register
#define DMAMUX0_CHCFG20		(*(volatile uint8_t  *)0x40021014) // Channel Configuration register
#define DMAMUX0_CHCFG21		(*(volatile uint8_t  *)0x40021015) // Channel Configuration register
#define DMAMUX0_CHCFG22		(*(volatile uint8_t  *)0x40021016) // Channel Configuration register
#define DMAMUX0_CHCFG23		(*(volatile uint8_t  *)0x40021017) // Channel Configuration register
#define DMAMUX0_CHCFG24		(*(volatile uint8_t  *)0x40021018) // Channel Configuration register
#define DMAMUX0_CHCFG25		(*(volatile uint8_t  *)0x40021019) // Channel Configuration register
#define DMAMUX0_CHCFG26		(*(volatile uint8_t  *)0x4002101A) // Channel Configuration register
#define DMAMUX0_CHCFG27		(*(volatile uint8_t  *)0x4002101B) // Channel Configuration register
#define DMAMUX0_CHCFG28		(*(volatile uint8_t  *)0x4002101C) // Channel Configuration register
#define DMAMUX0_CHCFG29		(*(volatile uint8_t  *)0x4002101D) // Channel Configuration register
#define DMAMUX0_CHCFG30		(*(volatile uint8_t  *)0x4002101E) // Channel Configuration register
#define DMAMUX0_CHCFG31		(*(volatile uint8_t  *)0x4002101F) // Channel Configuration register
#endif
#define DMAMUX_DISABLE			0
#define DMAMUX_TRIG			64
#define DMAMUX_ENABLE			128

// Direct Memory Access Controller (eDMA)

#if defined(KINETISK)
#define DMA_CR			(*(volatile uint32_t *)0x40008000) // Control Register
#define DMA_CR_CX			((uint32_t)(1<<17))	// Cancel Transfer
#define DMA_CR_ECX			((uint32_t)(1<<16))	// Error Cancel Transfer
#define DMA_CR_EMLM			((uint32_t)0x80)	// Enable Minor Loop Mapping
#define DMA_CR_CLM			((uint32_t)0x40)	// Continuous Link Mode
#define DMA_CR_HALT			((uint32_t)0x20)	// Halt DMA Operations
#define DMA_CR_HOE			((uint32_t)0x10)	// Halt On Error
#define DMA_CR_ERCA			((uint32_t)0x04)	// Enable Round Robin Channel Arbitration
#define DMA_CR_EDBG			((uint32_t)0x02)	// Enable Debug
#define DMA_ES			(*(volatile uint32_t *)0x40008004) // Error Status Register
#define DMA_ERQ			(*(volatile uint32_t *)0x4000800C) // Enable Request Register
#define DMA_EEI			(*(volatile uint32_t *)0x40008014) // Enable Error Interrupt Register
#define DMA_CEEI		(*(volatile uint8_t  *)0x40008018) // Clear Enable Error Interrupt Register
#define DMA_CEEI_CEEI(n)		((uint8_t)(n & 15)<<0)	// Clear Enable Error Interrupt
#define DMA_CEEI_CAEE			((uint8_t)1<<6)		// Clear All Enable Error Interrupts
#define DMA_CEEI_NOP			((uint8_t)1<<7)		// NOP
#define DMA_SEEI		(*(volatile uint8_t  *)0x40008019) // Set Enable Error Interrupt Register
#define DMA_SEEI_SEEI(n)		((uint8_t)(n & 15)<<0)	// Set Enable Error Interrupt
#define DMA_SEEI_SAEE			((uint8_t)1<<6)		// Set All Enable Error Interrupts
#define DMA_SEEI_NOP			((uint8_t)1<<7)		// NOP
#define DMA_CERQ		(*(volatile uint8_t  *)0x4000801A) // Clear Enable Request Register
#define DMA_CERQ_CERQ(n)		((uint8_t)(n & 15)<<0)	// Clear Enable Request
#define DMA_CERQ_CAER			((uint8_t)1<<6)		// Clear All Enable Requests
#define DMA_CERQ_NOP			((uint8_t)1<<7)		// NOP
#define DMA_SERQ		(*(volatile uint8_t  *)0x4000801B) // Set Enable Request Register
#define DMA_SERQ_SERQ(n)		((uint8_t)(n & 15)<<0)	// Set Enable Request
#define DMA_SERQ_SAER			((uint8_t)1<<6)		// Set All Enable Requests
#define DMA_SERQ_NOP			((uint8_t)1<<7)		// NOP
#define DMA_CDNE		(*(volatile uint8_t  *)0x4000801C) // Clear DONE Status Bit Register
#define DMA_CDNE_CDNE(n)		((uint8_t)(n & 15)<<0)	// Clear Done Bit
#define DMA_CDNE_CADN			((uint8_t)1<<6)		// Clear All Done Bits
#define DMA_CDNE_NOP			((uint8_t)1<<7)		// NOP
#define DMA_SSRT		(*(volatile uint8_t  *)0x4000801D) // Set START Bit Register
#define DMA_SSRT_SSRT(n)		((uint8_t)(n & 15)<<0)	// Set Start Bit
#define DMA_SSRT_SAST			((uint8_t)1<<6)		// Set All Start Bits
#define DMA_SSRT_NOP			((uint8_t)1<<7)		// NOP
#define DMA_CERR		(*(volatile uint8_t  *)0x4000801E) // Clear Error Register
#define DMA_CERR_CERR(n)		((uint8_t)(n & 15)<<0)	// Clear Error Indicator
#define DMA_CERR_CAEI			((uint8_t)1<<6)		// Clear All Error Indicators
#define DMA_CERR_NOP			((uint8_t)1<<7)		// NOP
#define DMA_CINT		(*(volatile uint8_t  *)0x4000801F) // Clear Interrupt Request Register
#define DMA_CINT_CINT(n)		((uint8_t)(n & 15)<<0)	// Clear Interrupt Request
#define DMA_CINT_CAIR			((uint8_t)1<<6)		// Clear All Interrupt Requests
#define DMA_CINT_NOP			((uint8_t)1<<7)		// NOP
#define DMA_INT			(*(volatile uint32_t *)0x40008024) // Interrupt Request Register
#define DMA_ERR			(*(volatile uint32_t *)0x4000802C) // Error Register
#define DMA_HRS			(*(volatile uint32_t *)0x40008034) // Hardware Request Status Register

#if DMA_NUM_CHANNELS >= 4
#define DMA_ERQ_ERQ0			((uint32_t)1<<0)	// Enable DMA Request 0
#define DMA_ERQ_ERQ1			((uint32_t)1<<1)	// Enable DMA Request 1
#define DMA_ERQ_ERQ2			((uint32_t)1<<2)	// Enable DMA Request 2
#define DMA_ERQ_ERQ3			((uint32_t)1<<3)	// Enable DMA Request 3
#define DMA_INT_INT0			((uint32_t)1<<0)	// Interrupt Request 0
#define DMA_INT_INT1			((uint32_t)1<<1)	// Interrupt Request 1
#define DMA_INT_INT2			((uint32_t)1<<2)	// Interrupt Request 2
#define DMA_INT_INT3			((uint32_t)1<<3)	// Interrupt Request 3
#define DMA_ERR_ERR0			((uint32_t)1<<0)	// Error in Channel 0
#define DMA_ERR_ERR1			((uint32_t)1<<1)	// Error in Channel 1
#define DMA_ERR_ERR2			((uint32_t)1<<2)	// Error in Channel 2
#define DMA_ERR_ERR3			((uint32_t)1<<3)	// Error in Channel 3
#define DMA_HRS_HRS0			((uint32_t)1<<0)	// Hardware Request Status Channel 0
#define DMA_HRS_HRS1			((uint32_t)1<<1)	// Hardware Request Status Channel 1
#define DMA_HRS_HRS2			((uint32_t)1<<2)	// Hardware Request Status Channel 2
#define DMA_HRS_HRS3			((uint32_t)1<<3)	// Hardware Request Status Channel 3
#endif
#if DMA_NUM_CHANNELS >= 16
#define DMA_ERQ_ERQ4			((uint32_t)1<<4)	// Enable DMA Request 4
#define DMA_ERQ_ERQ5			((uint32_t)1<<5)	// Enable DMA Request 5
#define DMA_ERQ_ERQ6			((uint32_t)1<<6)	// Enable DMA Request 6
#define DMA_ERQ_ERQ7			((uint32_t)1<<7)	// Enable DMA Request 7
#define DMA_ERQ_ERQ8			((uint32_t)1<<8)	// Enable DMA Request 8
#define DMA_ERQ_ERQ9			((uint32_t)1<<9)	// Enable DMA Request 9
#define DMA_ERQ_ERQ10			((uint32_t)1<<10)	// Enable DMA Request 10
#define DMA_ERQ_ERQ11			((uint32_t)1<<11)	// Enable DMA Request 11
#define DMA_ERQ_ERQ12			((uint32_t)1<<12)	// Enable DMA Request 12
#define DMA_ERQ_ERQ13			((uint32_t)1<<13)	// Enable DMA Request 13
#define DMA_ERQ_ERQ14			((uint32_t)1<<14)	// Enable DMA Request 14
#define DMA_ERQ_ERQ15			((uint32_t)1<<15)	// Enable DMA Request 15
#define DMA_INT_INT4			((uint32_t)1<<4)	// Interrupt Request 4
#define DMA_INT_INT5			((uint32_t)1<<5)	// Interrupt Request 5
#define DMA_INT_INT6			((uint32_t)1<<6)	// Interrupt Request 6
#define DMA_INT_INT7			((uint32_t)1<<7)	// Interrupt Request 7
#define DMA_INT_INT8			((uint32_t)1<<8)	// Interrupt Request 8
#define DMA_INT_INT9			((uint32_t)1<<9)	// Interrupt Request 9
#define DMA_INT_INT10			((uint32_t)1<<10)	// Interrupt Request 10
#define DMA_INT_INT11			((uint32_t)1<<11)	// Interrupt Request 11
#define DMA_INT_INT12			((uint32_t)1<<12)	// Interrupt Request 12
#define DMA_INT_INT13			((uint32_t)1<<13)	// Interrupt Request 13
#define DMA_INT_INT14			((uint32_t)1<<14)	// Interrupt Request 14
#define DMA_INT_INT15			((uint32_t)1<<15)	// Interrupt Request 15
#define DMA_ERR_ERR4			((uint32_t)1<<4)	// Error in Channel 4
#define DMA_ERR_ERR5			((uint32_t)1<<5)	// Error in Channel 5
#define DMA_ERR_ERR6			((uint32_t)1<<6)	// Error in Channel 6
#define DMA_ERR_ERR7			((uint32_t)1<<7)	// Error in Channel 7
#define DMA_ERR_ERR8			((uint32_t)1<<8)	// Error in Channel 8
#define DMA_ERR_ERR9			((uint32_t)1<<9)	// Error in Channel 9
#define DMA_ERR_ERR10			((uint32_t)1<<10)	// Error in Channel 10
#define DMA_ERR_ERR11			((uint32_t)1<<11)	// Error in Channel 11
#define DMA_ERR_ERR12			((uint32_t)1<<12)	// Error in Channel 12
#define DMA_ERR_ERR13			((uint32_t)1<<13)	// Error in Channel 13
#define DMA_ERR_ERR14			((uint32_t)1<<14)	// Error in Channel 14
#define DMA_ERR_ERR15			((uint32_t)1<<15)	// Error in Channel 15
#define DMA_HRS_HRS4			((uint32_t)1<<4)	// Hardware Request Status Channel 4
#define DMA_HRS_HRS5			((uint32_t)1<<5)	// Hardware Request Status Channel 5
#define DMA_HRS_HRS6			((uint32_t)1<<6)	// Hardware Request Status Channel 6
#define DMA_HRS_HRS7			((uint32_t)1<<7)	// Hardware Request Status Channel 7
#define DMA_HRS_HRS8			((uint32_t)1<<8)	// Hardware Request Status Channel 8
#define DMA_HRS_HRS9			((uint32_t)1<<9)	// Hardware Request Status Channel 9
#define DMA_HRS_HRS10			((uint32_t)1<<10)	// Hardware Request Status Channel 10
#define DMA_HRS_HRS11			((uint32_t)1<<11)	// Hardware Request Status Channel 11
#define DMA_HRS_HRS12			((uint32_t)1<<12)	// Hardware Request Status Channel 12
#define DMA_HRS_HRS13			((uint32_t)1<<13)	// Hardware Request Status Channel 13
#define DMA_HRS_HRS14			((uint32_t)1<<14)	// Hardware Request Status Channel 14
#define DMA_HRS_HRS15			((uint32_t)1<<15)	// Hardware Request Status Channel 15
#endif
#if DMA_NUM_CHANNELS >= 32
#define DMA_ERQ_ERQ16			((uint32_t)1<<16)	// Enable DMA Request 16
#define DMA_ERQ_ERQ17			((uint32_t)1<<17)	// Enable DMA Request 17
#define DMA_ERQ_ERQ18			((uint32_t)1<<18)	// Enable DMA Request 18
#define DMA_ERQ_ERQ19			((uint32_t)1<<19)	// Enable DMA Request 19
#define DMA_ERQ_ERQ20			((uint32_t)1<<20)	// Enable DMA Request 20
#define DMA_ERQ_ERQ21			((uint32_t)1<<21)	// Enable DMA Request 21
#define DMA_ERQ_ERQ22			((uint32_t)1<<22)	// Enable DMA Request 22
#define DMA_ERQ_ERQ23			((uint32_t)1<<23)	// Enable DMA Request 23
#define DMA_ERQ_ERQ24			((uint32_t)1<<24)	// Enable DMA Request 24
#define DMA_ERQ_ERQ25			((uint32_t)1<<25)	// Enable DMA Request 25
#define DMA_ERQ_ERQ26			((uint32_t)1<<26)	// Enable DMA Request 26
#define DMA_ERQ_ERQ27			((uint32_t)1<<27)	// Enable DMA Request 27
#define DMA_ERQ_ERQ28			((uint32_t)1<<28)	// Enable DMA Request 28
#define DMA_ERQ_ERQ29			((uint32_t)1<<29)	// Enable DMA Request 29
#define DMA_ERQ_ERQ30			((uint32_t)1<<30)	// Enable DMA Request 30
#define DMA_ERQ_ERQ31			((uint32_t)1<<31)	// Enable DMA Request 31
#define DMA_INT_INT16			((uint32_t)1<<16)	// Interrupt Request 16
#define DMA_INT_INT17			((uint32_t)1<<17)	// Interrupt Request 17
#define DMA_INT_INT18			((uint32_t)1<<18)	// Interrupt Request 18
#define DMA_INT_INT19			((uint32_t)1<<19)	// Interrupt Request 19
#define DMA_INT_INT20			((uint32_t)1<<20)	// Interrupt Request 20
#define DMA_INT_INT21			((uint32_t)1<<21)	// Interrupt Request 21
#define DMA_INT_INT22			((uint32_t)1<<22)	// Interrupt Request 22
#define DMA_INT_INT23			((uint32_t)1<<23)	// Interrupt Request 23
#define DMA_INT_INT24			((uint32_t)1<<24)	// Interrupt Request 24
#define DMA_INT_INT25			((uint32_t)1<<25)	// Interrupt Request 25
#define DMA_INT_INT26			((uint32_t)1<<26)	// Interrupt Request 26
#define DMA_INT_INT27			((uint32_t)1<<27)	// Interrupt Request 27
#define DMA_INT_INT28			((uint32_t)1<<28)	// Interrupt Request 28
#define DMA_INT_INT29			((uint32_t)1<<29)	// Interrupt Request 29
#define DMA_INT_INT30			((uint32_t)1<<30)	// Interrupt Request 30
#define DMA_INT_INT31			((uint32_t)1<<31)	// Interrupt Request 31
#define DMA_ERR_ERR16			((uint32_t)1<<16)	// Error in Channel 16
#define DMA_ERR_ERR17			((uint32_t)1<<17)	// Error in Channel 17
#define DMA_ERR_ERR18			((uint32_t)1<<18)	// Error in Channel 18
#define DMA_ERR_ERR19			((uint32_t)1<<19)	// Error in Channel 19
#define DMA_ERR_ERR20			((uint32_t)1<<20)	// Error in Channel 20
#define DMA_ERR_ERR21			((uint32_t)1<<21)	// Error in Channel 21
#define DMA_ERR_ERR22			((uint32_t)1<<22)	// Error in Channel 22
#define DMA_ERR_ERR23			((uint32_t)1<<23)	// Error in Channel 23
#define DMA_ERR_ERR24			((uint32_t)1<<24)	// Error in Channel 24
#define DMA_ERR_ERR25			((uint32_t)1<<25)	// Error in Channel 25
#define DMA_ERR_ERR26			((uint32_t)1<<26)	// Error in Channel 26
#define DMA_ERR_ERR27			((uint32_t)1<<27)	// Error in Channel 27
#define DMA_ERR_ERR28			((uint32_t)1<<28)	// Error in Channel 28
#define DMA_ERR_ERR29			((uint32_t)1<<29)	// Error in Channel 29
#define DMA_ERR_ERR30			((uint32_t)1<<30)	// Error in Channel 30
#define DMA_ERR_ERR31			((uint32_t)1<<31)	// Error in Channel 31
#define DMA_HRS_HRS16			((uint32_t)1<<16)	// Hardware Request Status Channel 16
#define DMA_HRS_HRS17			((uint32_t)1<<17)	// Hardware Request Status Channel 17
#define DMA_HRS_HRS18			((uint32_t)1<<18)	// Hardware Request Status Channel 18
#define DMA_HRS_HRS19			((uint32_t)1<<19)	// Hardware Request Status Channel 19
#define DMA_HRS_HRS20			((uint32_t)1<<20)	// Hardware Request Status Channel 20
#define DMA_HRS_HRS21			((uint32_t)1<<21)	// Hardware Request Status Channel 21
#define DMA_HRS_HRS22			((uint32_t)1<<22)	// Hardware Request Status Channel 22
#define DMA_HRS_HRS23			((uint32_t)1<<23)	// Hardware Request Status Channel 23
#define DMA_HRS_HRS24			((uint32_t)1<<24)	// Hardware Request Status Channel 24
#define DMA_HRS_HRS25			((uint32_t)1<<25)	// Hardware Request Status Channel 25
#define DMA_HRS_HRS26			((uint32_t)1<<26)	// Hardware Request Status Channel 26
#define DMA_HRS_HRS27			((uint32_t)1<<27)	// Hardware Request Status Channel 27
#define DMA_HRS_HRS28			((uint32_t)1<<28)	// Hardware Request Status Channel 28
#define DMA_HRS_HRS29			((uint32_t)1<<29)	// Hardware Request Status Channel 29
#define DMA_HRS_HRS30			((uint32_t)1<<30)	// Hardware Request Status Channel 30
#define DMA_HRS_HRS31			((uint32_t)1<<31)	// Hardware Request Status Channel 31
#endif

#if DMA_NUM_CHANNELS >= 4
#define DMA_DCHPRI3		(*(volatile uint8_t  *)0x40008100) // Channel n Priority Register
#define DMA_DCHPRI2		(*(volatile uint8_t  *)0x40008101) // Channel n Priority Register
#define DMA_DCHPRI1		(*(volatile uint8_t  *)0x40008102) // Channel n Priority Register
#define DMA_DCHPRI0		(*(volatile uint8_t  *)0x40008103) // Channel n Priority Register
#endif
#define DMA_DCHPRI_CHPRI(n)		((uint8_t)(n & 15)<<0)	// Channel Arbitration Priority
#define DMA_DCHPRI_DPA			((uint8_t)1<<6)		// Disable PreEmpt Ability
#define DMA_DCHPRI_ECP			((uint8_t)1<<7)		// Enable PreEmption
#if DMA_NUM_CHANNELS >= 16
#define DMA_DCHPRI7		(*(volatile uint8_t  *)0x40008104) // Channel n Priority Register
#define DMA_DCHPRI6		(*(volatile uint8_t  *)0x40008105) // Channel n Priority Register
#define DMA_DCHPRI5		(*(volatile uint8_t  *)0x40008106) // Channel n Priority Register
#define DMA_DCHPRI4		(*(volatile uint8_t  *)0x40008107) // Channel n Priority Register
#define DMA_DCHPRI11		(*(volatile uint8_t  *)0x40008108) // Channel n Priority Register
#define DMA_DCHPRI10		(*(volatile uint8_t  *)0x40008109) // Channel n Priority Register
#define DMA_DCHPRI9		(*(volatile uint8_t  *)0x4000810A) // Channel n Priority Register
#define DMA_DCHPRI8		(*(volatile uint8_t  *)0x4000810B) // Channel n Priority Register
#define DMA_DCHPRI15		(*(volatile uint8_t  *)0x4000810C) // Channel n Priority Register
#define DMA_DCHPRI14		(*(volatile uint8_t  *)0x4000810D) // Channel n Priority Register
#define DMA_DCHPRI13		(*(volatile uint8_t  *)0x4000810E) // Channel n Priority Register
#define DMA_DCHPRI12		(*(volatile uint8_t  *)0x4000810F) // Channel n Priority Register
#endif
#if DMA_NUM_CHANNELS >= 32
#define DMA_DCHPRI19		(*(volatile uint8_t  *)0x40008110) // Channel n Priority Register
#define DMA_DCHPRI18		(*(volatile uint8_t  *)0x40008111) // Channel n Priority Register
#define DMA_DCHPRI17		(*(volatile uint8_t  *)0x40008112) // Channel n Priority Register
#define DMA_DCHPRI16		(*(volatile uint8_t  *)0x40008113) // Channel n Priority Register
#define DMA_DCHPRI23		(*(volatile uint8_t  *)0x40008114) // Channel n Priority Register
#define DMA_DCHPRI22		(*(volatile uint8_t  *)0x40008115) // Channel n Priority Register
#define DMA_DCHPRI21		(*(volatile uint8_t  *)0x40008116) // Channel n Priority Register
#define DMA_DCHPRI20		(*(volatile uint8_t  *)0x40008117) // Channel n Priority Register
#define DMA_DCHPRI27		(*(volatile uint8_t  *)0x40008118) // Channel n Priority Register
#define DMA_DCHPRI26		(*(volatile uint8_t  *)0x40008119) // Channel n Priority Register
#define DMA_DCHPRI25		(*(volatile uint8_t  *)0x4000811A) // Channel n Priority Register
#define DMA_DCHPRI24		(*(volatile uint8_t  *)0x4000811B) // Channel n Priority Register
#define DMA_DCHPRI31		(*(volatile uint8_t  *)0x4000811C) // Channel n Priority Register
#define DMA_DCHPRI30		(*(volatile uint8_t  *)0x4000811D) // Channel n Priority Register
#define DMA_DCHPRI29		(*(volatile uint8_t  *)0x4000811E) // Channel n Priority Register
#define DMA_DCHPRI28		(*(volatile uint8_t  *)0x4000811F) // Channel n Priority Register
#define DMA_CR_GRP0PRI			((uint32_t)0x100)
#define DMA_CR_GRP1PRI			((uint32_t)0x400)
#endif

#define DMA_TCD_ATTR_SMOD(n)		(((n) & 0x1F) << 11)
#define DMA_TCD_ATTR_SSIZE(n)		(((n) & 0x7) << 8)
#define DMA_TCD_ATTR_DMOD(n)		(((n) & 0x1F) << 3)
#define DMA_TCD_ATTR_DSIZE(n)		(((n) & 0x7) << 0)
#define DMA_TCD_ATTR_SIZE_8BIT		0
#define DMA_TCD_ATTR_SIZE_16BIT		1
#define DMA_TCD_ATTR_SIZE_32BIT		2
#define DMA_TCD_ATTR_SIZE_16BYTE	4
#define DMA_TCD_ATTR_SIZE_32BYTE	5 // caution: this might not be supported in newer chips?
#define DMA_TCD_CSR_BWC(n)		(((n) & 0x3) << 14)
#define DMA_TCD_CSR_BWC_MASK		0xC000
#define DMA_TCD_CSR_MAJORLINKCH(n)	(((n) & 0xF) << 8)
#define DMA_TCD_CSR_MAJORLINKCH_MASK	0x0F00
#define DMA_TCD_CSR_DONE		0x0080
#define DMA_TCD_CSR_ACTIVE		0x0040
#define DMA_TCD_CSR_MAJORELINK		0x0020
#define DMA_TCD_CSR_ESG			0x0010
#define DMA_TCD_CSR_DREQ		0x0008
#define DMA_TCD_CSR_INTHALF		0x0004
#define DMA_TCD_CSR_INTMAJOR		0x0002
#define DMA_TCD_CSR_START		0x0001
#define DMA_TCD_CITER_MASK		((uint16_t)0x7FFF)	   // Loop count mask
#define DMA_TCD_CITER_ELINK		((uint16_t)1<<15)	   // Enable channel linking on minor-loop complete
#define DMA_TCD_BITER_MASK		((uint16_t)0x7FFF)	   // Loop count mask
#define DMA_TCD_BITER_ELINK		((uint16_t)1<<15)	   // Enable channel linking on minor-loop complete
#define DMA_TCD_BITER_ELINKYES_ELINK		0x8000
#define DMA_TCD_BITER_ELINKYES_LINKCH(n)	(((n) & 0xF) << 9)
#define DMA_TCD_BITER_ELINKYES_LINKCH_MASK	0x1E00
#define DMA_TCD_BITER_ELINKYES_BITER(n)		(((n) & 0x1FF) << 0)
#define DMA_TCD_BITER_ELINKYES_BITER_MASK	0x01FF
#define DMA_TCD_CITER_ELINKYES_ELINK		0x8000
#define DMA_TCD_CITER_ELINKYES_LINKCH(n)	(((n) & 0xF) << 9)
#define DMA_TCD_CITER_ELINKYES_LINKCH_MASK	0x1E00
#define DMA_TCD_CITER_ELINKYES_CITER(n)		(((n) & 0x1FF) << 0)
#define DMA_TCD_CITER_ELINKYES_CITER_MASK	0x01FF
#define DMA_TCD_NBYTES_SMLOE		    ((uint32_t)1<<31)		    // Source Minor Loop Offset Enable
#define DMA_TCD_NBYTES_DMLOE		    ((uint32_t)1<<30)		    // Destination Minor Loop Offset Enable
#define DMA_TCD_NBYTES_MLOFFNO_NBYTES(n)    ((uint32_t)((n) & 0x3FFFFFFF))  // NBytes transfer count when minor loop disabled
#define DMA_TCD_NBYTES_MLOFFYES_NBYTES(n)   ((uint32_t)((n) & 0x1F))	    // NBytes transfer count when minor loop enabled
#define DMA_TCD_NBYTES_MLOFFYES_MLOFF(n)    ((uint32_t)((n) & 0xFFFFF)<<10) // Minor loop offset

#if DMA_NUM_CHANNELS >= 4
#define DMA_TCD0_SADDR		(*(volatile const void * volatile *)0x40009000) // TCD Source Address
#define DMA_TCD0_SOFF		(*(volatile int16_t *)0x40009004)  // TCD Signed Source Address Offset
#define DMA_TCD0_ATTR		(*(volatile uint16_t *)0x40009006) // TCD Transfer Attributes
#define DMA_TCD0_NBYTES_MLNO	(*(volatile uint32_t *)0x40009008) // TCD Minor Byte Count (Minor Loop Disabled)
#define DMA_TCD0_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009008) // TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled)
#define DMA_TCD0_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009008) // TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled)
#define DMA_TCD0_SLAST		(*(volatile int32_t *)0x4000900C)  // TCD Last Source Address Adjustment
#define DMA_TCD0_DADDR		(*(volatile void * volatile *)0x40009010) // TCD Destination Address
#define DMA_TCD0_DOFF		(*(volatile int16_t *)0x40009014)  // TCD Signed Destination Address Offset
#define DMA_TCD0_CITER_ELINKYES (*(volatile uint16_t *)0x40009016) // TCD Current Minor Loop Link, Major Loop Count, Channel Linking Enabled
#define DMA_TCD0_CITER_ELINKNO	(*(volatile uint16_t *)0x40009016) // ??
#define DMA_TCD0_DLASTSGA	(*(volatile int32_t *)0x40009018)  // TCD Last Destination Address Adjustment/Scatter Gather Address
#define DMA_TCD0_CSR		(*(volatile uint16_t *)0x4000901C) // TCD Control and Status
#define DMA_TCD0_BITER_ELINKYES (*(volatile uint16_t *)0x4000901E) // TCD Beginning Minor Loop Link, Major Loop Count, Channel Linking Enabled
#define DMA_TCD0_BITER_ELINKNO	(*(volatile uint16_t *)0x4000901E) // TCD Beginning Minor Loop Link, Major Loop Count, Channel Linking Disabled

#define DMA_TCD1_SADDR		(*(volatile const void * volatile *)0x40009020) // TCD Source Address
#define DMA_TCD1_SOFF		(*(volatile int16_t *)0x40009024)  // TCD Signed Source Address Offset
#define DMA_TCD1_ATTR		(*(volatile uint16_t *)0x40009026) // TCD Transfer Attributes
#define DMA_TCD1_NBYTES_MLNO	(*(volatile uint32_t *)0x40009028) // TCD Minor Byte Count, Minor Loop Disabled
#define DMA_TCD1_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009028) // TCD Signed Minor Loop Offset, Minor Loop Enabled and Offset Disabled
#define DMA_TCD1_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009028) // TCD Signed Minor Loop Offset, Minor Loop and Offset Enabled
#define DMA_TCD1_SLAST		(*(volatile int32_t *)0x4000902C)  // TCD Last Source Address Adjustment
#define DMA_TCD1_DADDR		(*(volatile void * volatile *)0x40009030) // TCD Destination Address
#define DMA_TCD1_DOFF		(*(volatile int16_t *)0x40009034)  // TCD Signed Destination Address Offset
#define DMA_TCD1_CITER_ELINKYES (*(volatile uint16_t *)0x40009036) // TCD Current Minor Loop Link, Major Loop Count, Channel Linking Enabled
#define DMA_TCD1_CITER_ELINKNO	(*(volatile uint16_t *)0x40009036) // ??
#define DMA_TCD1_DLASTSGA	(*(volatile int32_t *)0x40009038)  // TCD Last Destination Address Adjustment/Scatter Gather Address
#define DMA_TCD1_CSR		(*(volatile uint16_t *)0x4000903C) // TCD Control and Status
#define DMA_TCD1_BITER_ELINKYES (*(volatile uint16_t *)0x4000903E) // TCD Beginning Minor Loop Link, Major Loop Count Channel Linking Enabled
#define DMA_TCD1_BITER_ELINKNO	(*(volatile uint16_t *)0x4000903E) // TCD Beginning Minor Loop Link, Major Loop Count, Channel Linking Disabled

#define DMA_TCD2_SADDR		(*(volatile const void * volatile *)0x40009040) // TCD Source Address
#define DMA_TCD2_SOFF		(*(volatile int16_t *)0x40009044)  // TCD Signed Source Address Offset
#define DMA_TCD2_ATTR		(*(volatile uint16_t *)0x40009046) // TCD Transfer Attributes
#define DMA_TCD2_NBYTES_MLNO	(*(volatile uint32_t *)0x40009048) // TCD Minor Byte Count, Minor Loop Disabled
#define DMA_TCD2_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009048) // TCD Signed Minor Loop Offset, Minor Loop Enabled and Offset Disabled
#define DMA_TCD2_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009048) // TCD Signed Minor Loop Offset, Minor Loop and Offset Enabled
#define DMA_TCD2_SLAST		(*(volatile int32_t *)0x4000904C)  // TCD Last Source Address Adjustment
#define DMA_TCD2_DADDR		(*(volatile void * volatile *)0x40009050) // TCD Destination Address
#define DMA_TCD2_DOFF		(*(volatile int16_t *)0x40009054)  // TCD Signed Destination Address Offset
#define DMA_TCD2_CITER_ELINKYES (*(volatile uint16_t *)0x40009056) // TCD Current Minor Loop Link, Major Loop Count, Channel Linking Enabled
#define DMA_TCD2_CITER_ELINKNO	(*(volatile uint16_t *)0x40009056) // ??
#define DMA_TCD2_DLASTSGA	(*(volatile int32_t *)0x40009058)  // TCD Last Destination Address Adjustment/Scatter Gather Address
#define DMA_TCD2_CSR		(*(volatile uint16_t *)0x4000905C) // TCD Control and Status
#define DMA_TCD2_BITER_ELINKYES (*(volatile uint16_t *)0x4000905E) // TCD Beginning Minor Loop Link, Major Loop Count, Channel Linking Enabled
#define DMA_TCD2_BITER_ELINKNO	(*(volatile uint16_t *)0x4000905E) // TCD Beginning Minor Loop Link, Major Loop Count, Channel Linking Disabled

#define DMA_TCD3_SADDR		(*(volatile const void * volatile *)0x40009060) // TCD Source Address
#define DMA_TCD3_SOFF		(*(volatile int16_t *)0x40009064)  // TCD Signed Source Address Offset
#define DMA_TCD3_ATTR		(*(volatile uint16_t *)0x40009066) // TCD Transfer Attributes
#define DMA_TCD3_NBYTES_MLNO	(*(volatile uint32_t *)0x40009068) // TCD Minor Byte Count, Minor Loop Disabled
#define DMA_TCD3_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009068) // TCD Signed Minor Loop Offset, Minor Loop Enabled and Offset Disabled
#define DMA_TCD3_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009068) // TCD Signed Minor Loop Offset, Minor Loop and Offset Enabled
#define DMA_TCD3_SLAST		(*(volatile int32_t *)0x4000906C)  // TCD Last Source Address Adjustment
#define DMA_TCD3_DADDR		(*(volatile void * volatile *)0x40009070) // TCD Destination Address
#define DMA_TCD3_DOFF		(*(volatile int16_t *)0x40009074)  // TCD Signed Destination Address Offset
#define DMA_TCD3_CITER_ELINKYES (*(volatile uint16_t *)0x40009076) // TCD Current Minor Loop Link, Major Loop Count, Channel Linking Enabled
#define DMA_TCD3_CITER_ELINKNO	(*(volatile uint16_t *)0x40009076) // ??
#define DMA_TCD3_DLASTSGA	(*(volatile int32_t *)0x40009078)  // TCD Last Destination Address Adjustment/Scatter Gather Address
#define DMA_TCD3_CSR		(*(volatile uint16_t *)0x4000907C) // TCD Control and Status
#define DMA_TCD3_BITER_ELINKYES (*(volatile uint16_t *)0x4000907E) // TCD Beginning Minor Loop Link, Major Loop Count ,Channel Linking Enabled
#define DMA_TCD3_BITER_ELINKNO	(*(volatile uint16_t *)0x4000907E) // TCD Beginning Minor Loop Link, Major Loop Count ,Channel Linking Disabled

#define DMA_TCD4_SADDR		(*(volatile const void * volatile *)0x40009080) // TCD Source Addr
#define DMA_TCD4_SOFF		(*(volatile int16_t *)0x40009084)  // TCD Signed Source Address Offset
#define DMA_TCD4_ATTR		(*(volatile uint16_t *)0x40009086) // TCD Transfer Attributes
#define DMA_TCD4_NBYTES_MLNO	(*(volatile uint32_t *)0x40009088) // TCD Minor Byte Count
#define DMA_TCD4_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009088) // TCD Signed Minor Loop Offset
#define DMA_TCD4_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009088) // TCD Signed Minor Loop Offset
#define DMA_TCD4_SLAST		(*(volatile int32_t *)0x4000908C)  // TCD Last Source Addr Adj.
#define DMA_TCD4_DADDR		(*(volatile void * volatile *)0x40009090) // TCD Destination Address
#define DMA_TCD4_DOFF		(*(volatile int16_t *)0x40009094)  // TCD Signed Dest Address Offset
#define DMA_TCD4_CITER_ELINKYES (*(volatile uint16_t *)0x40009096) // TCD Current Minor Loop Link
#define DMA_TCD4_CITER_ELINKNO	(*(volatile uint16_t *)0x40009096) // ??
#define DMA_TCD4_DLASTSGA	(*(volatile int32_t *)0x40009098)  // TCD Last Destination Addr Adj
#define DMA_TCD4_CSR		(*(volatile uint16_t *)0x4000909C) // TCD Control and Status
#define DMA_TCD4_BITER_ELINKYES (*(volatile uint16_t *)0x4000909E) // TCD Beginning Minor Loop Link
#define DMA_TCD4_BITER_ELINKNO	(*(volatile uint16_t *)0x4000909E) // TCD Beginning Minor Loop Link
#endif

#if DMA_NUM_CHANNELS >= 16
#define DMA_TCD5_SADDR		(*(volatile const void * volatile *)0x400090A0) // TCD Source Addr
#define DMA_TCD5_SOFF		(*(volatile int16_t *)0x400090A4)  // TCD Signed Source Address Offset
#define DMA_TCD5_ATTR		(*(volatile uint16_t *)0x400090A6) // TCD Transfer Attributes
#define DMA_TCD5_NBYTES_MLNO	(*(volatile uint32_t *)0x400090A8) // TCD Minor Byte Count
#define DMA_TCD5_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400090A8) // TCD Signed Minor Loop Offset
#define DMA_TCD5_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400090A8) // TCD Signed Minor Loop Offset
#define DMA_TCD5_SLAST		(*(volatile int32_t *)0x400090AC)  // TCD Last Source Addr Adj.
#define DMA_TCD5_DADDR		(*(volatile void * volatile *)0x400090B0) // TCD Destination Address
#define DMA_TCD5_DOFF		(*(volatile int16_t *)0x400090B4)  // TCD Signed Dest Address Offset
#define DMA_TCD5_CITER_ELINKYES (*(volatile uint16_t *)0x400090B6) // TCD Current Minor Loop Link
#define DMA_TCD5_CITER_ELINKNO	(*(volatile uint16_t *)0x400090B6) // ??
#define DMA_TCD5_DLASTSGA	(*(volatile int32_t *)0x400090B8)  // TCD Last Destination Addr Adj
#define DMA_TCD5_CSR		(*(volatile uint16_t *)0x400090BC) // TCD Control and Status
#define DMA_TCD5_BITER_ELINKYES (*(volatile uint16_t *)0x400090BE) // TCD Beginning Minor Loop Link
#define DMA_TCD5_BITER_ELINKNO	(*(volatile uint16_t *)0x400090BE) // TCD Beginning Minor Loop Link

#define DMA_TCD6_SADDR		(*(volatile const void * volatile *)0x400090C0) // TCD Source Addr
#define DMA_TCD6_SOFF		(*(volatile int16_t *)0x400090C4)  // TCD Signed Source Address Offset
#define DMA_TCD6_ATTR		(*(volatile uint16_t *)0x400090C6) // TCD Transfer Attributes
#define DMA_TCD6_NBYTES_MLNO	(*(volatile uint32_t *)0x400090C8) // TCD Minor Byte Count
#define DMA_TCD6_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400090C8) // TCD Signed Minor Loop Offset
#define DMA_TCD6_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400090C8) // TCD Signed Minor Loop Offset
#define DMA_TCD6_SLAST		(*(volatile int32_t *)0x400090CC)  // TCD Last Source Addr Adj.
#define DMA_TCD6_DADDR		(*(volatile void * volatile *)0x400090D0) // TCD Destination Address
#define DMA_TCD6_DOFF		(*(volatile int16_t *)0x400090D4)  // TCD Signed Dest Address Offset
#define DMA_TCD6_CITER_ELINKYES (*(volatile uint16_t *)0x400090D6) // TCD Current Minor Loop Link
#define DMA_TCD6_CITER_ELINKNO	(*(volatile uint16_t *)0x400090D6) // ??
#define DMA_TCD6_DLASTSGA	(*(volatile int32_t *)0x400090D8)  // TCD Last Destination Addr Adj
#define DMA_TCD6_CSR		(*(volatile uint16_t *)0x400090DC) // TCD Control and Status
#define DMA_TCD6_BITER_ELINKYES (*(volatile uint16_t *)0x400090DE) // TCD Beginning Minor Loop Link
#define DMA_TCD6_BITER_ELINKNO	(*(volatile uint16_t *)0x400090DE) // TCD Beginning Minor Loop Link

#define DMA_TCD7_SADDR		(*(volatile const void * volatile *)0x400090E0) // TCD Source Addr
#define DMA_TCD7_SOFF		(*(volatile int16_t *)0x400090E4)  // TCD Signed Source Address Offset
#define DMA_TCD7_ATTR		(*(volatile uint16_t *)0x400090E6) // TCD Transfer Attributes
#define DMA_TCD7_NBYTES_MLNO	(*(volatile uint32_t *)0x400090E8) // TCD Minor Byte Count
#define DMA_TCD7_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400090E8) // TCD Signed Minor Loop Offset
#define DMA_TCD7_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400090E8) // TCD Signed Minor Loop Offset
#define DMA_TCD7_SLAST		(*(volatile int32_t *)0x400090EC)  // TCD Last Source Addr Adj.
#define DMA_TCD7_DADDR		(*(volatile void * volatile *)0x400090F0) // TCD Destination Address
#define DMA_TCD7_DOFF		(*(volatile int16_t *)0x400090F4)  // TCD Signed Dest Address Offset
#define DMA_TCD7_CITER_ELINKYES (*(volatile uint16_t *)0x400090F6) // TCD Current Minor Loop Link
#define DMA_TCD7_CITER_ELINKNO	(*(volatile uint16_t *)0x400090F6) // ??
#define DMA_TCD7_DLASTSGA	(*(volatile int32_t *)0x400090F8)  // TCD Last Destination Addr Adj
#define DMA_TCD7_CSR		(*(volatile uint16_t *)0x400090FC) // TCD Control and Status
#define DMA_TCD7_BITER_ELINKYES (*(volatile uint16_t *)0x400090FE) // TCD Beginning Minor Loop Link
#define DMA_TCD7_BITER_ELINKNO	(*(volatile uint16_t *)0x400090FE) // TCD Beginning Minor Loop Link

#define DMA_TCD8_SADDR		(*(volatile const void * volatile *)0x40009100) // TCD Source Addr
#define DMA_TCD8_SOFF		(*(volatile int16_t *)0x40009104)  // TCD Signed Source Address Offset
#define DMA_TCD8_ATTR		(*(volatile uint16_t *)0x40009106) // TCD Transfer Attributes
#define DMA_TCD8_NBYTES_MLNO	(*(volatile uint32_t *)0x40009108) // TCD Minor Byte Count
#define DMA_TCD8_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009108) // TCD Signed Minor Loop Offset
#define DMA_TCD8_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009108) // TCD Signed Minor Loop Offset
#define DMA_TCD8_SLAST		(*(volatile int32_t *)0x4000910C)  // TCD Last Source Addr Adj.
#define DMA_TCD8_DADDR		(*(volatile void * volatile *)0x40009110) // TCD Destination Address
#define DMA_TCD8_DOFF		(*(volatile int16_t *)0x40009114)  // TCD Signed Dest Address Offset
#define DMA_TCD8_CITER_ELINKYES (*(volatile uint16_t *)0x40009116) // TCD Current Minor Loop Link
#define DMA_TCD8_CITER_ELINKNO	(*(volatile uint16_t *)0x40009116) // ??
#define DMA_TCD8_DLASTSGA	(*(volatile int32_t *)0x40009118)  // TCD Last Destination Addr Adj
#define DMA_TCD8_CSR		(*(volatile uint16_t *)0x4000911C) // TCD Control and Status
#define DMA_TCD8_BITER_ELINKYES (*(volatile uint16_t *)0x4000911E) // TCD Beginning Minor Loop Link
#define DMA_TCD8_BITER_ELINKNO	(*(volatile uint16_t *)0x4000911E) // TCD Beginning Minor Loop Link

#define DMA_TCD9_SADDR		(*(volatile const void * volatile *)0x40009120) // TCD Source Addr
#define DMA_TCD9_SOFF		(*(volatile int16_t *)0x40009124)  // TCD Signed Source Address Offset
#define DMA_TCD9_ATTR		(*(volatile uint16_t *)0x40009126) // TCD Transfer Attributes
#define DMA_TCD9_NBYTES_MLNO	(*(volatile uint32_t *)0x40009128) // TCD Minor Byte Count
#define DMA_TCD9_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009128) // TCD Signed Minor Loop Offset
#define DMA_TCD9_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009128) // TCD Signed Minor Loop Offset
#define DMA_TCD9_SLAST		(*(volatile int32_t *)0x4000912C)  // TCD Last Source Addr Adj.
#define DMA_TCD9_DADDR		(*(volatile void * volatile *)0x40009130) // TCD Destination Address
#define DMA_TCD9_DOFF		(*(volatile int16_t *)0x40009134)  // TCD Signed Dest Address Offset
#define DMA_TCD9_CITER_ELINKYES (*(volatile uint16_t *)0x40009136) // TCD Current Minor Loop Link
#define DMA_TCD9_CITER_ELINKNO	(*(volatile uint16_t *)0x40009136) // ??
#define DMA_TCD9_DLASTSGA	(*(volatile int32_t *)0x40009138)  // TCD Last Destination Addr Adj
#define DMA_TCD9_CSR		(*(volatile uint16_t *)0x4000913C) // TCD Control and Status
#define DMA_TCD9_BITER_ELINKYES (*(volatile uint16_t *)0x4000913E) // TCD Beginning Minor Loop Link
#define DMA_TCD9_BITER_ELINKNO	(*(volatile uint16_t *)0x4000913E) // TCD Beginning Minor Loop Link

#define DMA_TCD10_SADDR		(*(volatile const void * volatile *)0x40009140) // TCD Source Addr
#define DMA_TCD10_SOFF		(*(volatile int16_t *)0x40009144)	 // TCD Signed Source Address Offset
#define DMA_TCD10_ATTR		(*(volatile uint16_t *)0x40009146) // TCD Transfer Attributes
#define DMA_TCD10_NBYTES_MLNO	(*(volatile uint32_t *)0x40009148) // TCD Minor Byte Count
#define DMA_TCD10_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009148) // TCD Signed Minor Loop Offset
#define DMA_TCD10_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009148) // TCD Signed Minor Loop Offset
#define DMA_TCD10_SLAST		(*(volatile int32_t *)0x4000914C)	 // TCD Last Source Addr Adj.
#define DMA_TCD10_DADDR		(*(volatile void * volatile *)0x40009150) // TCD Destination Address
#define DMA_TCD10_DOFF		(*(volatile int16_t *)0x40009154)	 // TCD Signed Dest Address Offset
#define DMA_TCD10_CITER_ELINKYES (*(volatile uint16_t *)0x40009156) // TCD Current Minor Loop Link
#define DMA_TCD10_CITER_ELINKNO	(*(volatile uint16_t *)0x40009156) // ??
#define DMA_TCD10_DLASTSGA	(*(volatile int32_t *)0x40009158)	 // TCD Last Destination Addr Adj
#define DMA_TCD10_CSR		(*(volatile uint16_t *)0x4000915C) // TCD Control and Status
#define DMA_TCD10_BITER_ELINKYES (*(volatile uint16_t *)0x4000915E) // TCD Beginning Minor Loop Link
#define DMA_TCD10_BITER_ELINKNO	(*(volatile uint16_t *)0x4000915E) // TCD Beginning Minor Loop Link

#define DMA_TCD11_SADDR		(*(volatile const void * volatile *)0x40009160) // TCD Source Addr
#define DMA_TCD11_SOFF		(*(volatile int16_t *)0x40009164)	 // TCD Signed Source Address Offset
#define DMA_TCD11_ATTR		(*(volatile uint16_t *)0x40009166) // TCD Transfer Attributes
#define DMA_TCD11_NBYTES_MLNO	(*(volatile uint32_t *)0x40009168) // TCD Minor Byte Count
#define DMA_TCD11_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009168) // TCD Signed Minor Loop Offset
#define DMA_TCD11_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009168) // TCD Signed Minor Loop Offset
#define DMA_TCD11_SLAST		(*(volatile int32_t *)0x4000916C)	 // TCD Last Source Addr Adj.
#define DMA_TCD11_DADDR		(*(volatile void * volatile *)0x40009170) // TCD Destination Address
#define DMA_TCD11_DOFF		(*(volatile int16_t *)0x40009174)	 // TCD Signed Dest Address Offset
#define DMA_TCD11_CITER_ELINKYES (*(volatile uint16_t *)0x40009176) // TCD Current Minor Loop Link
#define DMA_TCD11_CITER_ELINKNO	(*(volatile uint16_t *)0x40009176) // ??
#define DMA_TCD11_DLASTSGA	(*(volatile int32_t *)0x40009178)	 // TCD Last Destination Addr Adj
#define DMA_TCD11_CSR		(*(volatile uint16_t *)0x4000917C) // TCD Control and Status
#define DMA_TCD11_BITER_ELINKYES (*(volatile uint16_t *)0x4000917E) // TCD Beginning Minor Loop Link
#define DMA_TCD11_BITER_ELINKNO	(*(volatile uint16_t *)0x4000917E) // TCD Beginning Minor Loop Link

#define DMA_TCD12_SADDR		(*(volatile const void * volatile *)0x40009180) // TCD Source Addr
#define DMA_TCD12_SOFF		(*(volatile int16_t *)0x40009184)	 // TCD Signed Source Address Offset
#define DMA_TCD12_ATTR		(*(volatile uint16_t *)0x40009186) // TCD Transfer Attributes
#define DMA_TCD12_NBYTES_MLNO	(*(volatile uint32_t *)0x40009188) // TCD Minor Byte Count
#define DMA_TCD12_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009188) // TCD Signed Minor Loop Offset
#define DMA_TCD12_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009188) // TCD Signed Minor Loop Offset
#define DMA_TCD12_SLAST		(*(volatile int32_t *)0x4000918C)	 // TCD Last Source Addr Adj.
#define DMA_TCD12_DADDR		(*(volatile void * volatile *)0x40009190) // TCD Destination Address
#define DMA_TCD12_DOFF		(*(volatile int16_t *)0x40009194)	 // TCD Signed Dest Address Offset
#define DMA_TCD12_CITER_ELINKYES (*(volatile uint16_t *)0x40009196) // TCD Current Minor Loop Link
#define DMA_TCD12_CITER_ELINKNO	(*(volatile uint16_t *)0x40009196) // ??
#define DMA_TCD12_DLASTSGA	(*(volatile int32_t *)0x40009198)	 // TCD Last Destination Addr Adj
#define DMA_TCD12_CSR		(*(volatile uint16_t *)0x4000919C) // TCD Control and Status
#define DMA_TCD12_BITER_ELINKYES (*(volatile uint16_t *)0x4000919E) // TCD Beginning Minor Loop Link
#define DMA_TCD12_BITER_ELINKNO	(*(volatile uint16_t *)0x4000919E) // TCD Beginning Minor Loop Link

#define DMA_TCD13_SADDR		(*(volatile const void * volatile *)0x400091A0) // TCD Source Addr
#define DMA_TCD13_SOFF		(*(volatile int16_t *)0x400091A4)	 // TCD Signed Source Address Offset
#define DMA_TCD13_ATTR		(*(volatile uint16_t *)0x400091A6) // TCD Transfer Attributes
#define DMA_TCD13_NBYTES_MLNO	(*(volatile uint32_t *)0x400091A8) // TCD Minor Byte Count
#define DMA_TCD13_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400091A8) // TCD Signed Minor Loop Offset
#define DMA_TCD13_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400091A8) // TCD Signed Minor Loop Offset
#define DMA_TCD13_SLAST		(*(volatile int32_t *)0x400091AC)	 // TCD Last Source Addr Adj.
#define DMA_TCD13_DADDR		(*(volatile void * volatile *)0x400091B0) // TCD Destination Address
#define DMA_TCD13_DOFF		(*(volatile int16_t *)0x400091B4)	 // TCD Signed Dest Address Offset
#define DMA_TCD13_CITER_ELINKYES (*(volatile uint16_t *)0x400091B6) // TCD Current Minor Loop Link
#define DMA_TCD13_CITER_ELINKNO	(*(volatile uint16_t *)0x400091B6) // ??
#define DMA_TCD13_DLASTSGA	(*(volatile int32_t *)0x400091B8)	 // TCD Last Destination Addr Adj
#define DMA_TCD13_CSR		(*(volatile uint16_t *)0x400091BC) // TCD Control and Status
#define DMA_TCD13_BITER_ELINKYES (*(volatile uint16_t *)0x400091BE) // TCD Beginning Minor Loop Link
#define DMA_TCD13_BITER_ELINKNO	(*(volatile uint16_t *)0x400091BE) // TCD Beginning Minor Loop Link

#define DMA_TCD14_SADDR		(*(volatile const void * volatile *)0x400091C0) // TCD Source Addr
#define DMA_TCD14_SOFF		(*(volatile int16_t *)0x400091C4)	 // TCD Signed Source Address Offset
#define DMA_TCD14_ATTR		(*(volatile uint16_t *)0x400091C6) // TCD Transfer Attributes
#define DMA_TCD14_NBYTES_MLNO	(*(volatile uint32_t *)0x400091C8) // TCD Minor Byte Count
#define DMA_TCD14_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400091C8) // TCD Signed Minor Loop Offset
#define DMA_TCD14_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400091C8) // TCD Signed Minor Loop Offset
#define DMA_TCD14_SLAST		(*(volatile int32_t *)0x400091CC)	 // TCD Last Source Addr Adj.
#define DMA_TCD14_DADDR		(*(volatile void * volatile *)0x400091D0) // TCD Destination Address
#define DMA_TCD14_DOFF		(*(volatile int16_t *)0x400091D4)	 // TCD Signed Dest Address Offset
#define DMA_TCD14_CITER_ELINKYES (*(volatile uint16_t *)0x400091D6) // TCD Current Minor Loop Link
#define DMA_TCD14_CITER_ELINKNO	(*(volatile uint16_t *)0x400091D6) // ??
#define DMA_TCD14_DLASTSGA	(*(volatile int32_t *)0x400091D8)	 // TCD Last Destination Addr Adj
#define DMA_TCD14_CSR		(*(volatile uint16_t *)0x400091DC) // TCD Control and Status
#define DMA_TCD14_BITER_ELINKYES (*(volatile uint16_t *)0x400091DE) // TCD Beginning Minor Loop Link
#define DMA_TCD14_BITER_ELINKNO	(*(volatile uint16_t *)0x400091DE) // TCD Beginning Minor Loop Link

#define DMA_TCD15_SADDR		(*(volatile const void * volatile *)0x400091E0) // TCD Source Addr
#define DMA_TCD15_SOFF		(*(volatile int16_t *)0x400091E4)	 // TCD Signed Source Address Offset
#define DMA_TCD15_ATTR		(*(volatile uint16_t *)0x400091E6) // TCD Transfer Attributes
#define DMA_TCD15_NBYTES_MLNO	(*(volatile uint32_t *)0x400091E8) // TCD Minor Byte Count
#define DMA_TCD15_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400091E8) // TCD Signed Minor Loop Offset
#define DMA_TCD15_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400091E8) // TCD Signed Minor Loop Offset
#define DMA_TCD15_SLAST		(*(volatile int32_t *)0x400091EC)	 // TCD Last Source Addr Adj.
#define DMA_TCD15_DADDR		(*(volatile void * volatile *)0x400091F0) // TCD Destination Address
#define DMA_TCD15_DOFF		(*(volatile int16_t *)0x400091F4)	 // TCD Signed Dest Address Offset
#define DMA_TCD15_CITER_ELINKYES (*(volatile uint16_t *)0x400091F6) // TCD Current Minor Loop Link
#define DMA_TCD15_CITER_ELINKNO	(*(volatile uint16_t *)0x400091F6) // ??
#define DMA_TCD15_DLASTSGA	(*(volatile int32_t *)0x400091F8)	 // TCD Last Destination Addr Adj
#define DMA_TCD15_CSR		(*(volatile uint16_t *)0x400091FC) // TCD Control and Status
#define DMA_TCD15_BITER_ELINKYES (*(volatile uint16_t *)0x400091FE) // TCD Beginning Minor Loop Link
#define DMA_TCD15_BITER_ELINKNO	(*(volatile uint16_t *)0x400091FE) // TCD Beginning Minor Loop Link
#endif

#if DMA_NUM_CHANNELS >= 32
#define DMA_TCD16_SADDR		(*(volatile const void * volatile *)0x40009200) // TCD Source Addr
#define DMA_TCD16_SOFF		(*(volatile int16_t *)0x40009204)	 // TCD Signed Source Address Offset
#define DMA_TCD16_ATTR		(*(volatile uint16_t *)0x40009206) // TCD Transfer Attributes
#define DMA_TCD16_NBYTES_MLNO	(*(volatile uint32_t *)0x40009208) // TCD Minor Byte Count
#define DMA_TCD16_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009208) // TCD Signed Minor Loop Offset
#define DMA_TCD16_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009208) // TCD Signed Minor Loop Offset
#define DMA_TCD16_SLAST		(*(volatile int32_t *)0x4000920C)	 // TCD Last Source Addr Adj.
#define DMA_TCD16_DADDR		(*(volatile void * volatile *)0x40009210) // TCD Destination Address
#define DMA_TCD16_DOFF		(*(volatile int16_t *)0x40009214)	 // TCD Signed Dest Address Offset
#define DMA_TCD16_CITER_ELINKYES (*(volatile uint16_t *)0x40009216) // TCD Current Minor Loop Link
#define DMA_TCD16_CITER_ELINKNO	(*(volatile uint16_t *)0x40009216) // ??
#define DMA_TCD16_DLASTSGA	(*(volatile int32_t *)0x40009218)	 // TCD Last Destination Addr Adj
#define DMA_TCD16_CSR		(*(volatile uint16_t *)0x4000921C) // TCD Control and Status
#define DMA_TCD16_BITER_ELINKYES (*(volatile uint16_t *)0x4000921E) // TCD Beginning Minor Loop Link
#define DMA_TCD16_BITER_ELINKNO	(*(volatile uint16_t *)0x4000921E) // TCD Beginning Minor Loop Link

#define DMA_TCD17_SADDR		(*(volatile const void * volatile *)0x40009220) // TCD Source Addr
#define DMA_TCD17_SOFF		(*(volatile int16_t *)0x40009224)	 // TCD Signed Source Address Offset
#define DMA_TCD17_ATTR		(*(volatile uint16_t *)0x40009226) // TCD Transfer Attributes
#define DMA_TCD17_NBYTES_MLNO	(*(volatile uint32_t *)0x40009228) // TCD Minor Byte Count
#define DMA_TCD17_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009228) // TCD Signed Minor Loop Offset
#define DMA_TCD17_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009228) // TCD Signed Minor Loop Offset
#define DMA_TCD17_SLAST		(*(volatile int32_t *)0x4000922C)	 // TCD Last Source Addr Adj.
#define DMA_TCD17_DADDR		(*(volatile void * volatile *)0x40009230) // TCD Destination Address
#define DMA_TCD17_DOFF		(*(volatile int16_t *)0x40009234)	 // TCD Signed Dest Address Offset
#define DMA_TCD17_CITER_ELINKYES (*(volatile uint16_t *)0x40009236) // TCD Current Minor Loop Link
#define DMA_TCD17_CITER_ELINKNO	(*(volatile uint16_t *)0x40009236) // ??
#define DMA_TCD17_DLASTSGA	(*(volatile int32_t *)0x40009238)	 // TCD Last Destination Addr Adj
#define DMA_TCD17_CSR		(*(volatile uint16_t *)0x4000923C) // TCD Control and Status
#define DMA_TCD17_BITER_ELINKYES (*(volatile uint16_t *)0x4000923E) // TCD Beginning Minor Loop Link
#define DMA_TCD17_BITER_ELINKNO	(*(volatile uint16_t *)0x4000923E) // TCD Beginning Minor Loop Link

#define DMA_TCD18_SADDR		(*(volatile const void * volatile *)0x40009240) // TCD Source Addr
#define DMA_TCD18_SOFF		(*(volatile int16_t *)0x40009244)	 // TCD Signed Source Address Offset
#define DMA_TCD18_ATTR		(*(volatile uint16_t *)0x40009246) // TCD Transfer Attributes
#define DMA_TCD18_NBYTES_MLNO	(*(volatile uint32_t *)0x40009248) // TCD Minor Byte Count
#define DMA_TCD18_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009248) // TCD Signed Minor Loop Offset
#define DMA_TCD18_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009248) // TCD Signed Minor Loop Offset
#define DMA_TCD18_SLAST		(*(volatile int32_t *)0x4000924C)	 // TCD Last Source Addr Adj.
#define DMA_TCD18_DADDR		(*(volatile void * volatile *)0x40009250) // TCD Destination Address
#define DMA_TCD18_DOFF		(*(volatile int16_t *)0x40009254)	 // TCD Signed Dest Address Offset
#define DMA_TCD18_CITER_ELINKYES (*(volatile uint16_t *)0x40009256) // TCD Current Minor Loop Link
#define DMA_TCD18_CITER_ELINKNO	(*(volatile uint16_t *)0x40009256) // ??
#define DMA_TCD18_DLASTSGA	(*(volatile int32_t *)0x40009258)	 // TCD Last Destination Addr Adj
#define DMA_TCD18_CSR		(*(volatile uint16_t *)0x4000925C) // TCD Control and Status
#define DMA_TCD18_BITER_ELINKYES (*(volatile uint16_t *)0x4000925E) // TCD Beginning Minor Loop Link
#define DMA_TCD18_BITER_ELINKNO	(*(volatile uint16_t *)0x4000925E) // TCD Beginning Minor Loop Link

#define DMA_TCD19_SADDR		(*(volatile const void * volatile *)0x40009260) // TCD Source Addr
#define DMA_TCD19_SOFF		(*(volatile int16_t *)0x40009264)	 // TCD Signed Source Address Offset
#define DMA_TCD19_ATTR		(*(volatile uint16_t *)0x40009266) // TCD Transfer Attributes
#define DMA_TCD19_NBYTES_MLNO	(*(volatile uint32_t *)0x40009268) // TCD Minor Byte Count
#define DMA_TCD19_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009268) // TCD Signed Minor Loop Offset
#define DMA_TCD19_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009268) // TCD Signed Minor Loop Offset
#define DMA_TCD19_SLAST		(*(volatile int32_t *)0x4000926C)	 // TCD Last Source Addr Adj.
#define DMA_TCD19_DADDR		(*(volatile void * volatile *)0x40009270) // TCD Destination Address
#define DMA_TCD19_DOFF		(*(volatile int16_t *)0x40009274)	 // TCD Signed Dest Address Offset
#define DMA_TCD19_CITER_ELINKYES (*(volatile uint16_t *)0x40009276) // TCD Current Minor Loop Link
#define DMA_TCD19_CITER_ELINKNO	(*(volatile uint16_t *)0x40009276) // ??
#define DMA_TCD19_DLASTSGA	(*(volatile int32_t *)0x40009278)	 // TCD Last Destination Addr Adj
#define DMA_TCD19_CSR		(*(volatile uint16_t *)0x4000927C) // TCD Control and Status
#define DMA_TCD19_BITER_ELINKYES (*(volatile uint16_t *)0x4000927E) // TCD Beginning Minor Loop Link
#define DMA_TCD19_BITER_ELINKNO	(*(volatile uint16_t *)0x4000927E) // TCD Beginning Minor Loop Link

#define DMA_TCD20_SADDR		(*(volatile const void * volatile *)0x40009280) // TCD Source Addr
#define DMA_TCD20_SOFF		(*(volatile int16_t *)0x40009284)	 // TCD Signed Source Address Offset
#define DMA_TCD20_ATTR		(*(volatile uint16_t *)0x40009286) // TCD Transfer Attributes
#define DMA_TCD20_NBYTES_MLNO	(*(volatile uint32_t *)0x40009288) // TCD Minor Byte Count
#define DMA_TCD20_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009288) // TCD Signed Minor Loop Offset
#define DMA_TCD20_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009288) // TCD Signed Minor Loop Offset
#define DMA_TCD20_SLAST		(*(volatile int32_t *)0x4000928C)	 // TCD Last Source Addr Adj.
#define DMA_TCD20_DADDR		(*(volatile void * volatile *)0x40009290) // TCD Destination Address
#define DMA_TCD20_DOFF		(*(volatile int16_t *)0x40009294)	 // TCD Signed Dest Address Offset
#define DMA_TCD20_CITER_ELINKYES (*(volatile uint16_t *)0x40009296) // TCD Current Minor Loop Link
#define DMA_TCD20_CITER_ELINKNO	(*(volatile uint16_t *)0x40009296) // ??
#define DMA_TCD20_DLASTSGA	(*(volatile int32_t *)0x40009298)	 // TCD Last Destination Addr Adj
#define DMA_TCD20_CSR		(*(volatile uint16_t *)0x4000929C) // TCD Control and Status
#define DMA_TCD20_BITER_ELINKYES (*(volatile uint16_t *)0x4000929E) // TCD Beginning Minor Loop Link
#define DMA_TCD20_BITER_ELINKNO	(*(volatile uint16_t *)0x4000929E) // TCD Beginning Minor Loop Link

#define DMA_TCD21_SADDR		(*(volatile const void * volatile *)0x400092A0) // TCD Source Addr
#define DMA_TCD21_SOFF		(*(volatile int16_t *)0x400092A4)	 // TCD Signed Source Address Offset
#define DMA_TCD21_ATTR		(*(volatile uint16_t *)0x400092A6) // TCD Transfer Attributes
#define DMA_TCD21_NBYTES_MLNO	(*(volatile uint32_t *)0x400092A8) // TCD Minor Byte Count
#define DMA_TCD21_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400092A8) // TCD Signed Minor Loop Offset
#define DMA_TCD21_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400092A8) // TCD Signed Minor Loop Offset
#define DMA_TCD21_SLAST		(*(volatile int32_t *)0x400092AC)	 // TCD Last Source Addr Adj.
#define DMA_TCD21_DADDR		(*(volatile void * volatile *)0x400092B0) // TCD Destination Address
#define DMA_TCD21_DOFF		(*(volatile int16_t *)0x400092B4)	 // TCD Signed Dest Address Offset
#define DMA_TCD21_CITER_ELINKYES (*(volatile uint16_t *)0x400092B6) // TCD Current Minor Loop Link
#define DMA_TCD21_CITER_ELINKNO	(*(volatile uint16_t *)0x400092B6) // ??
#define DMA_TCD21_DLASTSGA	(*(volatile int32_t *)0x400092B8)	 // TCD Last Destination Addr Adj
#define DMA_TCD21_CSR		(*(volatile uint16_t *)0x400092BC) // TCD Control and Status
#define DMA_TCD21_BITER_ELINKYES (*(volatile uint16_t *)0x400092BE) // TCD Beginning Minor Loop Link
#define DMA_TCD21_BITER_ELINKNO	(*(volatile uint16_t *)0x400092BE) // TCD Beginning Minor Loop Link

#define DMA_TCD22_SADDR		(*(volatile const void * volatile *)0x400092C0) // TCD Source Addr
#define DMA_TCD22_SOFF		(*(volatile int16_t *)0x400092C4)	 // TCD Signed Source Address Offset
#define DMA_TCD22_ATTR		(*(volatile uint16_t *)0x400092C6) // TCD Transfer Attributes
#define DMA_TCD22_NBYTES_MLNO	(*(volatile uint32_t *)0x400092C8) // TCD Minor Byte Count
#define DMA_TCD22_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400092C8) // TCD Signed Minor Loop Offset
#define DMA_TCD22_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400092C8) // TCD Signed Minor Loop Offset
#define DMA_TCD22_SLAST		(*(volatile int32_t *)0x400092CC)	 // TCD Last Source Addr Adj.
#define DMA_TCD22_DADDR		(*(volatile void * volatile *)0x400092D0) // TCD Destination Address
#define DMA_TCD22_DOFF		(*(volatile int16_t *)0x400092D4)	 // TCD Signed Dest Address Offset
#define DMA_TCD22_CITER_ELINKYES (*(volatile uint16_t *)0x400092D6) // TCD Current Minor Loop Link
#define DMA_TCD22_CITER_ELINKNO	(*(volatile uint16_t *)0x400092D6) // ??
#define DMA_TCD22_DLASTSGA	(*(volatile int32_t *)0x400092D8)	 // TCD Last Destination Addr Adj
#define DMA_TCD22_CSR		(*(volatile uint16_t *)0x400092DC) // TCD Control and Status
#define DMA_TCD22_BITER_ELINKYES (*(volatile uint16_t *)0x400092DE) // TCD Beginning Minor Loop Link
#define DMA_TCD22_BITER_ELINKNO	(*(volatile uint16_t *)0x400092DE) // TCD Beginning Minor Loop Link

#define DMA_TCD23_SADDR		(*(volatile const void * volatile *)0x400092E0) // TCD Source Addr
#define DMA_TCD23_SOFF		(*(volatile int16_t *)0x400092E4)	 // TCD Signed Source Address Offset
#define DMA_TCD23_ATTR		(*(volatile uint16_t *)0x400092E6) // TCD Transfer Attributes
#define DMA_TCD23_NBYTES_MLNO	(*(volatile uint32_t *)0x400092E8) // TCD Minor Byte Count
#define DMA_TCD23_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400092E8) // TCD Signed Minor Loop Offset
#define DMA_TCD23_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400092E8) // TCD Signed Minor Loop Offset
#define DMA_TCD23_SLAST		(*(volatile int32_t *)0x400092EC)	 // TCD Last Source Addr Adj.
#define DMA_TCD23_DADDR		(*(volatile void * volatile *)0x400092F0) // TCD Destination Address
#define DMA_TCD23_DOFF		(*(volatile int16_t *)0x400092F4)	 // TCD Signed Dest Address Offset
#define DMA_TCD23_CITER_ELINKYES (*(volatile uint16_t *)0x400092F6) // TCD Current Minor Loop Link
#define DMA_TCD23_CITER_ELINKNO	(*(volatile uint16_t *)0x400092F6) // ??
#define DMA_TCD23_DLASTSGA	(*(volatile int32_t *)0x400092F8)	 // TCD Last Destination Addr Adj
#define DMA_TCD23_CSR		(*(volatile uint16_t *)0x400092FC) // TCD Control and Status
#define DMA_TCD23_BITER_ELINKYES (*(volatile uint16_t *)0x400092FE) // TCD Beginning Minor Loop Link
#define DMA_TCD23_BITER_ELINKNO	(*(volatile uint16_t *)0x400092FE) // TCD Beginning Minor Loop Link

#define DMA_TCD24_SADDR		(*(volatile const void * volatile *)0x40009300) // TCD Source Addr
#define DMA_TCD24_SOFF		(*(volatile int16_t *)0x40009304)	 // TCD Signed Source Address Offset
#define DMA_TCD24_ATTR		(*(volatile uint16_t *)0x40009306) // TCD Transfer Attributes
#define DMA_TCD24_NBYTES_MLNO	(*(volatile uint32_t *)0x40009308) // TCD Minor Byte Count
#define DMA_TCD24_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009308) // TCD Signed Minor Loop Offset
#define DMA_TCD24_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009308) // TCD Signed Minor Loop Offset
#define DMA_TCD24_SLAST		(*(volatile int32_t *)0x4000930C)	 // TCD Last Source Addr Adj.
#define DMA_TCD24_DADDR		(*(volatile void * volatile *)0x40009310) // TCD Destination Address
#define DMA_TCD24_DOFF		(*(volatile int16_t *)0x40009314)	 // TCD Signed Dest Address Offset
#define DMA_TCD24_CITER_ELINKYES (*(volatile uint16_t *)0x40009316) // TCD Current Minor Loop Link
#define DMA_TCD24_CITER_ELINKNO	(*(volatile uint16_t *)0x40009316) // ??
#define DMA_TCD24_DLASTSGA	(*(volatile int32_t *)0x40009318)	 // TCD Last Destination Addr Adj
#define DMA_TCD24_CSR		(*(volatile uint16_t *)0x4000931C) // TCD Control and Status
#define DMA_TCD24_BITER_ELINKYES (*(volatile uint16_t *)0x4000931E) // TCD Beginning Minor Loop Link
#define DMA_TCD24_BITER_ELINKNO	(*(volatile uint16_t *)0x4000931E) // TCD Beginning Minor Loop Link

#define DMA_TCD25_SADDR		(*(volatile const void * volatile *)0x40009320) // TCD Source Addr
#define DMA_TCD25_SOFF		(*(volatile int16_t *)0x40009324)	 // TCD Signed Source Address Offset
#define DMA_TCD25_ATTR		(*(volatile uint16_t *)0x40009326) // TCD Transfer Attributes
#define DMA_TCD25_NBYTES_MLNO	(*(volatile uint32_t *)0x40009328) // TCD Minor Byte Count
#define DMA_TCD25_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009328) // TCD Signed Minor Loop Offset
#define DMA_TCD25_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009328) // TCD Signed Minor Loop Offset
#define DMA_TCD25_SLAST		(*(volatile int32_t *)0x4000932C)	 // TCD Last Source Addr Adj.
#define DMA_TCD25_DADDR		(*(volatile void * volatile *)0x40009330) // TCD Destination Address
#define DMA_TCD25_DOFF		(*(volatile int16_t *)0x40009334)	 // TCD Signed Dest Address Offset
#define DMA_TCD25_CITER_ELINKYES (*(volatile uint16_t *)0x40009336) // TCD Current Minor Loop Link
#define DMA_TCD25_CITER_ELINKNO	(*(volatile uint16_t *)0x40009336) // ??
#define DMA_TCD25_DLASTSGA	(*(volatile int32_t *)0x40009338)	 // TCD Last Destination Addr Adj
#define DMA_TCD25_CSR		(*(volatile uint16_t *)0x4000933C) // TCD Control and Status
#define DMA_TCD25_BITER_ELINKYES (*(volatile uint16_t *)0x4000933E) // TCD Beginning Minor Loop Link
#define DMA_TCD25_BITER_ELINKNO	(*(volatile uint16_t *)0x4000933E) // TCD Beginning Minor Loop Link

#define DMA_TCD26_SADDR		(*(volatile const void * volatile *)0x40009340) // TCD Source Addr
#define DMA_TCD26_SOFF		(*(volatile int16_t *)0x40009344)	 // TCD Signed Source Address Offset
#define DMA_TCD26_ATTR		(*(volatile uint16_t *)0x40009346) // TCD Transfer Attributes
#define DMA_TCD26_NBYTES_MLNO	(*(volatile uint32_t *)0x40009348) // TCD Minor Byte Count
#define DMA_TCD26_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009348) // TCD Signed Minor Loop Offset
#define DMA_TCD26_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009348) // TCD Signed Minor Loop Offset
#define DMA_TCD26_SLAST		(*(volatile int32_t *)0x4000934C)	 // TCD Last Source Addr Adj.
#define DMA_TCD26_DADDR		(*(volatile void * volatile *)0x40009350) // TCD Destination Address
#define DMA_TCD26_DOFF		(*(volatile int16_t *)0x40009354)	 // TCD Signed Dest Address Offset
#define DMA_TCD26_CITER_ELINKYES (*(volatile uint16_t *)0x40009356) // TCD Current Minor Loop Link
#define DMA_TCD26_CITER_ELINKNO	(*(volatile uint16_t *)0x40009356) // ??
#define DMA_TCD26_DLASTSGA	(*(volatile int32_t *)0x40009358)	 // TCD Last Destination Addr Adj
#define DMA_TCD26_CSR		(*(volatile uint16_t *)0x4000935C) // TCD Control and Status
#define DMA_TCD26_BITER_ELINKYES (*(volatile uint16_t *)0x4000935E) // TCD Beginning Minor Loop Link
#define DMA_TCD26_BITER_ELINKNO	(*(volatile uint16_t *)0x4000935E) // TCD Beginning Minor Loop Link

#define DMA_TCD27_SADDR		(*(volatile const void * volatile *)0x40009360) // TCD Source Addr
#define DMA_TCD27_SOFF		(*(volatile int16_t *)0x40009364)	 // TCD Signed Source Address Offset
#define DMA_TCD27_ATTR		(*(volatile uint16_t *)0x40009366) // TCD Transfer Attributes
#define DMA_TCD27_NBYTES_MLNO	(*(volatile uint32_t *)0x40009368) // TCD Minor Byte Count
#define DMA_TCD27_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009368) // TCD Signed Minor Loop Offset
#define DMA_TCD27_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009368) // TCD Signed Minor Loop Offset
#define DMA_TCD27_SLAST		(*(volatile int32_t *)0x4000936C)	 // TCD Last Source Addr Adj.
#define DMA_TCD27_DADDR		(*(volatile void * volatile *)0x40009370) // TCD Destination Address
#define DMA_TCD27_DOFF		(*(volatile int16_t *)0x40009374)	 // TCD Signed Dest Address Offset
#define DMA_TCD27_CITER_ELINKYES (*(volatile uint16_t *)0x40009376) // TCD Current Minor Loop Link
#define DMA_TCD27_CITER_ELINKNO	(*(volatile uint16_t *)0x40009376) // ??
#define DMA_TCD27_DLASTSGA	(*(volatile int32_t *)0x40009378)	 // TCD Last Destination Addr Adj
#define DMA_TCD27_CSR		(*(volatile uint16_t *)0x4000937C) // TCD Control and Status
#define DMA_TCD27_BITER_ELINKYES (*(volatile uint16_t *)0x4000937E) // TCD Beginning Minor Loop Link
#define DMA_TCD27_BITER_ELINKNO	(*(volatile uint16_t *)0x4000937E) // TCD Beginning Minor Loop Link

#define DMA_TCD28_SADDR		(*(volatile const void * volatile *)0x40009380) // TCD Source Addr
#define DMA_TCD28_SOFF		(*(volatile int16_t *)0x40009384)	 // TCD Signed Source Address Offset
#define DMA_TCD28_ATTR		(*(volatile uint16_t *)0x40009386) // TCD Transfer Attributes
#define DMA_TCD28_NBYTES_MLNO	(*(volatile uint32_t *)0x40009388) // TCD Minor Byte Count
#define DMA_TCD28_NBYTES_MLOFFNO (*(volatile uint32_t *)0x40009388) // TCD Signed Minor Loop Offset
#define DMA_TCD28_NBYTES_MLOFFYES (*(volatile uint32_t *)0x40009388) // TCD Signed Minor Loop Offset
#define DMA_TCD28_SLAST		(*(volatile int32_t *)0x4000938C)	 // TCD Last Source Addr Adj.
#define DMA_TCD28_DADDR		(*(volatile void * volatile *)0x40009390) // TCD Destination Address
#define DMA_TCD28_DOFF		(*(volatile int16_t *)0x40009394)	 // TCD Signed Dest Address Offset
#define DMA_TCD28_CITER_ELINKYES (*(volatile uint16_t *)0x40009396) // TCD Current Minor Loop Link
#define DMA_TCD28_CITER_ELINKNO	(*(volatile uint16_t *)0x40009396) // ??
#define DMA_TCD28_DLASTSGA	(*(volatile int32_t *)0x40009398)	 // TCD Last Destination Addr Adj
#define DMA_TCD28_CSR		(*(volatile uint16_t *)0x4000939C) // TCD Control and Status
#define DMA_TCD28_BITER_ELINKYES (*(volatile uint16_t *)0x4000939E) // TCD Beginning Minor Loop Link
#define DMA_TCD28_BITER_ELINKNO	(*(volatile uint16_t *)0x4000939E) // TCD Beginning Minor Loop Link

#define DMA_TCD29_SADDR		(*(volatile const void * volatile *)0x400093A0) // TCD Source Addr
#define DMA_TCD29_SOFF		(*(volatile int16_t *)0x400093A4)	 // TCD Signed Source Address Offset
#define DMA_TCD29_ATTR		(*(volatile uint16_t *)0x400093A6) // TCD Transfer Attributes
#define DMA_TCD29_NBYTES_MLNO	(*(volatile uint32_t *)0x400093A8) // TCD Minor Byte Count
#define DMA_TCD29_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400093A8) // TCD Signed Minor Loop Offset
#define DMA_TCD29_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400093A8) // TCD Signed Minor Loop Offset
#define DMA_TCD29_SLAST		(*(volatile int32_t *)0x400093AC)	 // TCD Last Source Addr Adj.
#define DMA_TCD29_DADDR		(*(volatile void * volatile *)0x400093B0) // TCD Destination Address
#define DMA_TCD29_DOFF		(*(volatile int16_t *)0x400093B4)	 // TCD Signed Dest Address Offset
#define DMA_TCD29_CITER_ELINKYES (*(volatile uint16_t *)0x400093B6) // TCD Current Minor Loop Link
#define DMA_TCD29_CITER_ELINKNO	(*(volatile uint16_t *)0x400093B6) // ??
#define DMA_TCD29_DLASTSGA	(*(volatile int32_t *)0x400093B8)	 // TCD Last Destination Addr Adj
#define DMA_TCD29_CSR		(*(volatile uint16_t *)0x400093BC) // TCD Control and Status
#define DMA_TCD29_BITER_ELINKYES (*(volatile uint16_t *)0x400093BE) // TCD Beginning Minor Loop Link
#define DMA_TCD29_BITER_ELINKNO	(*(volatile uint16_t *)0x400093BE) // TCD Beginning Minor Loop Link

#define DMA_TCD30_SADDR		(*(volatile const void * volatile *)0x400093C0) // TCD Source Addr
#define DMA_TCD30_SOFF		(*(volatile int16_t *)0x400093C4)	 // TCD Signed Source Address Offset
#define DMA_TCD30_ATTR		(*(volatile uint16_t *)0x400093C6) // TCD Transfer Attributes
#define DMA_TCD30_NBYTES_MLNO	(*(volatile uint32_t *)0x400093C8) // TCD Minor Byte Count
#define DMA_TCD30_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400093C8) // TCD Signed Minor Loop Offset
#define DMA_TCD30_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400093C8) // TCD Signed Minor Loop Offset
#define DMA_TCD30_SLAST		(*(volatile int32_t *)0x400093CC)	 // TCD Last Source Addr Adj.
#define DMA_TCD30_DADDR		(*(volatile void * volatile *)0x400093D0) // TCD Destination Address
#define DMA_TCD30_DOFF		(*(volatile int16_t *)0x400093D4)	 // TCD Signed Dest Address Offset
#define DMA_TCD30_CITER_ELINKYES (*(volatile uint16_t *)0x400093D6) // TCD Current Minor Loop Link
#define DMA_TCD30_CITER_ELINKNO	(*(volatile uint16_t *)0x400093D6) // ??
#define DMA_TCD30_DLASTSGA	(*(volatile int32_t *)0x400093D8)	 // TCD Last Destination Addr Adj
#define DMA_TCD30_CSR		(*(volatile uint16_t *)0x400093DC) // TCD Control and Status
#define DMA_TCD30_BITER_ELINKYES (*(volatile uint16_t *)0x400093DE) // TCD Beginning Minor Loop Link
#define DMA_TCD30_BITER_ELINKNO	(*(volatile uint16_t *)0x400093DE) // TCD Beginning Minor Loop Link

#define DMA_TCD31_SADDR		(*(volatile const void * volatile *)0x400093E0) // TCD Source Addr
#define DMA_TCD31_SOFF		(*(volatile int16_t *)0x400093E4)	 // TCD Signed Source Address Offset
#define DMA_TCD31_ATTR		(*(volatile uint16_t *)0x400093E6) // TCD Transfer Attributes
#define DMA_TCD31_NBYTES_MLNO	(*(volatile uint32_t *)0x400093E8) // TCD Minor Byte Count
#define DMA_TCD31_NBYTES_MLOFFNO (*(volatile uint32_t *)0x400093E8) // TCD Signed Minor Loop Offset
#define DMA_TCD31_NBYTES_MLOFFYES (*(volatile uint32_t *)0x400093E8) // TCD Signed Minor Loop Offset
#define DMA_TCD31_SLAST		(*(volatile int32_t *)0x400093EC)	 // TCD Last Source Addr Adj.
#define DMA_TCD31_DADDR		(*(volatile void * volatile *)0x400093F0) // TCD Destination Address
#define DMA_TCD31_DOFF		(*(volatile int16_t *)0x400093F4)	 // TCD Signed Dest Address Offset
#define DMA_TCD31_CITER_ELINKYES (*(volatile uint16_t *)0x400093F6) // TCD Current Minor Loop Link
#define DMA_TCD31_CITER_ELINKNO	(*(volatile uint16_t *)0x400093F6) // ??
#define DMA_TCD31_DLASTSGA	(*(volatile int32_t *)0x400093F8)	 // TCD Last Destination Addr Adj
#define DMA_TCD31_CSR		(*(volatile uint16_t *)0x400093FC) // TCD Control and Status
#define DMA_TCD31_BITER_ELINKYES (*(volatile uint16_t *)0x400093FE) // TCD Beginning Minor Loop Link
#define DMA_TCD31_BITER_ELINKNO	(*(volatile uint16_t *)0x400093FE) // TCD Beginning Minor Loop Link
#endif

#elif defined(KINETISL)
#define DMA_SAR0		(*(volatile const void * volatile *)0x40008100)  // Source Address
#define DMA_DAR0		(*(volatile void * volatile *)0x40008104)  // Destination Address
#define DMA_DSR_BCR0		(*(volatile uint32_t *)0x40008108)  // Status / Byte Count
#define DMA_DCR0		(*(volatile uint32_t *)0x4000810C)  // Control
#define DMA_SAR1		(*(volatile const void * volatile *)0x40008110)  // Source Address
#define DMA_DAR1		(*(volatile void * volatile *)0x40008114)  // Destination Address
#define DMA_DSR_BCR1		(*(volatile uint32_t *)0x40008118)  // Status / Byte Count
#define DMA_DCR1		(*(volatile uint32_t *)0x4000811C)  // Control
#define DMA_SAR2		(*(volatile const void * volatile *)0x40008120)  // Source Address
#define DMA_DAR2		(*(volatile void * volatile *)0x40008124)  // Destination Address
#define DMA_DSR_BCR2		(*(volatile uint32_t *)0x40008128)  // Status / Byte Count
#define DMA_DCR2		(*(volatile uint32_t *)0x4000812C)  // Control
#define DMA_SAR3		(*(volatile const void * volatile *)0x40008130)  // Source Address
#define DMA_DAR3		(*(volatile void * volatile *)0x40008134)  // Destination Address
#define DMA_DSR_BCR3		(*(volatile uint32_t *)0x40008138)  // Status / Byte Count
#define DMA_DCR3		(*(volatile uint32_t *)0x4000813C)  // Control
#define DMA_DSR_BCR_CE			((uint32_t)0x40000000)	// Configuration Error
#define DMA_DSR_BCR_BES			((uint32_t)0x20000000)	// Bus Error on Source
#define DMA_DSR_BCR_BED			((uint32_t)0x10000000)	// Bus Error on Destination
#define DMA_DSR_BCR_REQ			((uint32_t)0x04000000)	// Request
#define DMA_DSR_BCR_BSY			((uint32_t)0x02000000)	// Busy
#define DMA_DSR_BCR_DONE		((uint32_t)0x01000000)	// Transactions Done
#define DMA_DSR_BCR_BCR(n)		((n) & 0x00FFFFFF)	// Byte Count Remaining
#define DMA_DCR_EINT			((uint32_t)0x80000000)	// Enable Interrupt on Completion
#define DMA_DCR_ERQ			((uint32_t)0x40000000)	// Enable Peripheral Request
#define DMA_DCR_CS			((uint32_t)0x20000000)	// Cycle Steal
#define DMA_DCR_AA			((uint32_t)0x10000000)	// Auto-align
#define DMA_DCR_EADREQ			((uint32_t)0x00800000)	// Enable asynchronous DMA requests
#define DMA_DCR_SINC			((uint32_t)0x00400000)	// Source Increment
#define DMA_DCR_SSIZE(n)		(((n) & 3) << 20)	// Source Size, 0=32, 1=8, 2=16
#define DMA_DCR_DINC			((uint32_t)0x00080000)	// Destination Increment
#define DMA_DCR_DSIZE(n)		(((n) & 3) << 17)	// Dest Size, 0=32, 1=8, 2=16
#define DMA_DCR_START			((uint32_t)0x00010000)	// Start Transfer
#define DMA_DCR_SMOD(n)			(((n) & 15) << 12)	// Source Address Modulo
#define DMA_DCR_DMOD(n)			(((n) & 15) << 8)	// Destination Address Modulo
#define DMA_DCR_D_REQ			((uint32_t)0x00000080)	// Disable Request
#define DMA_DCR_LINKCC(n)		(((n) & 3) << 4)	// Link Channel Control
#define DMA_DCR_LCH1(n)			(((n) & 3) << 2)	// Link Channel 1
#define DMA_DCR_LCH2(n)			(((n) & 3) << 0)	// Link Channel 2
#endif


// External Watchdog Monitor (EWM)

#define EWM_CTRL		(*(volatile uint8_t  *)0x40061000) // Control Register
#define EWM_SERV		(*(volatile uint8_t  *)0x40061001) // Service Register
#define EWM_CMPL		(*(volatile uint8_t  *)0x40061002) // Compare Low Register
#define EWM_CMPH		(*(volatile uint8_t  *)0x40061003) // Compare High Register

// Watchdog Timer (WDOG)

#define WDOG_STCTRLH		(*(volatile uint16_t *)0x40052000) // Watchdog Status and Control Register High
#define WDOG_STCTRLH_DISTESTWDOG	((uint16_t)0x4000)		// Allows the WDOG's functional test mode to be disabled permanently.
#define WDOG_STCTRLH_BYTESEL(n)		((uint16_t)(((n) & 3) << 12))	// selects the byte to be tested when the watchdog is in the byte test mode.
#define WDOG_STCTRLH_TESTSEL		((uint16_t)0x0800)
#define WDOG_STCTRLH_TESTWDOG		((uint16_t)0x0400)
#define WDOG_STCTRLH_WAITEN		((uint16_t)0x0080)
#define WDOG_STCTRLH_STOPEN		((uint16_t)0x0040)
#define WDOG_STCTRLH_DBGEN		((uint16_t)0x0020)
#define WDOG_STCTRLH_ALLOWUPDATE	((uint16_t)0x0010)
#define WDOG_STCTRLH_WINEN		((uint16_t)0x0008)
#define WDOG_STCTRLH_IRQRSTEN		((uint16_t)0x0004)
#define WDOG_STCTRLH_CLKSRC		((uint16_t)0x0002)
#define WDOG_STCTRLH_WDOGEN		((uint16_t)0x0001)
#define WDOG_STCTRLL		(*(volatile uint16_t *)0x40052002) // Watchdog Status and Control Register Low
#define WDOG_TOVALH		(*(volatile uint16_t *)0x40052004) // Watchdog Time-out Value Register High
#define WDOG_TOVALL		(*(volatile uint16_t *)0x40052006) // Watchdog Time-out Value Register Low
#define WDOG_WINH		(*(volatile uint16_t *)0x40052008) // Watchdog Window Register High
#define WDOG_WINL		(*(volatile uint16_t *)0x4005200A) // Watchdog Window Register Low
#define WDOG_REFRESH		(*(volatile uint16_t *)0x4005200C) // Watchdog Refresh register
#define WDOG_UNLOCK		(*(volatile uint16_t *)0x4005200E) // Watchdog Unlock register
#define WDOG_UNLOCK_SEQ1		((uint16_t)0xC520)
#define WDOG_UNLOCK_SEQ2		((uint16_t)0xD928)
#define WDOG_TMROUTH		(*(volatile uint16_t *)0x40052010) // Watchdog Timer Output Register High
#define WDOG_TMROUTL		(*(volatile uint16_t *)0x40052012) // Watchdog Timer Output Register Low
#define WDOG_RSTCNT		(*(volatile uint16_t *)0x40052014) // Watchdog Reset Count register
#define WDOG_PRESC		(*(volatile uint16_t *)0x40052016) // Watchdog Prescaler register

// Multipurpose Clock Generator (MCG)

typedef struct {
	volatile uint8_t C1;
	volatile uint8_t C2;
	volatile uint8_t C3;
	volatile uint8_t C4;
	volatile uint8_t C5;
	volatile uint8_t C6;
	volatile uint8_t S;
	volatile uint8_t unused1;
	volatile uint8_t SC;
	volatile uint8_t unused2;
	volatile uint8_t ATCVH;
	volatile uint8_t ATCVL;
	volatile uint8_t C7;
	volatile uint8_t C8;
	volatile uint8_t C9;
	volatile uint8_t unused3;
	volatile uint8_t C11;
	volatile uint8_t C12;
	volatile uint8_t S2;
	volatile uint8_t T3;
} KINETIS_MCG_t;
#define KINETIS_MCG		(*(KINETIS_MCG_t *)0x40064000)
#define MCG_C1                  (KINETIS_MCG.C1)		// 40064000  MCG Control 1 Register
#define MCG_C1_IREFSTEN			(uint8_t)0x01			// Internal Reference Stop Enable, Controls whether or not the internal reference clock remains enabled when the MCG enters Stop mode.
#define MCG_C1_IRCLKEN			(uint8_t)0x02			// Internal Reference Clock Enable, Enables the internal reference clock for use as MCGIRCLK.
#define MCG_C1_IREFS			(uint8_t)0x04			// Internal Reference Select, Selects the reference clock source for the FLL.
#define MCG_C1_FRDIV(n)			(uint8_t)(((n) & 0x07) << 3)	// FLL External Reference Divider, Selects the amount to divide down the external reference clock for the FLL
#define MCG_C1_CLKS(n)			(uint8_t)(((n) & 0x03) << 6)	// Clock Source Select, Selects the clock source for MCGOUTCLK
#define MCG_C2                  (KINETIS_MCG.C2)		// 40064001  MCG Control 2 Register
#define MCG_C2_IRCS			(uint8_t)0x01			// Internal Reference Clock Select, Selects between the fast or slow internal reference clock source.
#define MCG_C2_LP			(uint8_t)0x02			// Low Power Select, Controls whether the FLL or PLL is disabled in BLPI and BLPE modes.
#define MCG_C2_EREFS			(uint8_t)0x04			// External Reference Select, Selects the source for the external reference clock.
#define MCG_C2_HGO0			(uint8_t)0x08			// High Gain Oscillator Select, Controls the crystal oscillator mode of operation
#define MCG_C2_RANGE0(n)		(uint8_t)(((n) & 0x03) << 4)	// Frequency Range Select, Selects the frequency range for the crystal oscillator
#define MCG_C2_LOCRE0			(uint8_t)0x80			// Loss of Clock Reset Enable, Determines whether an interrupt or a reset request is made following a loss of OSC0
#define MCG_C3                  (KINETIS_MCG.C3)		// 40064002  MCG Control 3 Register
#define MCG_C3_SCTRIM(n)		(uint8_t)(n)			// Slow Internal Reference Clock Trim Setting
#define MCG_C4                  (KINETIS_MCG.C4)		// 40064003  MCG Control 4 Register
#define MCG_C4_SCFTRIM			(uint8_t)0x01			// Slow Internal Reference Clock Fine Trim
#define MCG_C4_FCTRIM(n)		(uint8_t)(((n) & 0x0F) << 1)	// Fast Internal Reference Clock Trim Setting
#define MCG_C4_DRST_DRS(n)		(uint8_t)(((n) & 0x03) << 5)	// DCO Range Select
#define MCG_C4_DMX32			(uint8_t)0x80			// DCO Maximum Frequency with 32.768 kHz Reference, controls whether the DCO frequency range is narrowed
#define MCG_C5                  (KINETIS_MCG.C5)		// 40064004  MCG Control 5 Register
#define MCG_C5_PRDIV0(n)		(uint8_t)((n) & 0x1F)		// PLL External Reference Divider
#define MCG_C5_PLLSTEN0			(uint8_t)0x20			// PLL Stop Enable
#define MCG_C5_PLLCLKEN0		(uint8_t)0x40			// PLL Clock Enable
#define MCG_C6                  (KINETIS_MCG.C6)		// 40064005  MCG Control 6 Register
#define MCG_C6_VDIV0(n)			(uint8_t)((n) & 0x1F)		// VCO 0 Divider
#define MCG_C6_CME0			(uint8_t)0x20			// Clock Monitor Enable
#define MCG_C6_PLLS			(uint8_t)0x40			// PLL Select, Controls whether the PLL or FLL output is selected as the MCG source when CLKS[1:0]=00.
#define MCG_C6_LOLIE0			(uint8_t)0x80			// Loss of Lock Interrrupt Enable
#define MCG_S                   (KINETIS_MCG.S)			// 40064006  MCG Status Register
#define MCG_S_IRCST			(uint8_t)0x01			// Internal Reference Clock Status
#define MCG_S_OSCINIT0			(uint8_t)0x02			// OSC Initialization,  resets to 0, is set to 1 after the initialization cycles of the crystal oscillator
#define MCG_S_CLKST(n)			(uint8_t)(((n) & 0x03) << 2)	// Clock Mode Status, 0=FLL is selected, 1= Internal ref, 2=External ref, 3=PLL
#define MCG_S_CLKST_MASK		(uint8_t)0x0C
#define MCG_S_IREFST			(uint8_t)0x10			// Internal Reference Status
#define MCG_S_PLLST			(uint8_t)0x20			// PLL Select Status
#define MCG_S_LOCK0			(uint8_t)0x40			// Lock Status, 0=PLL Unlocked, 1=PLL Locked
#define MCG_S_LOLS0			(uint8_t)0x80			// Loss of Lock Status
#define MCG_SC                  (KINETIS_MCG.SC)		// 40064008  MCG Status and Control Register
#define MCG_SC_LOCS0			(uint8_t)0x01			// OSC0 Loss of Clock Status
#define MCG_SC_FCRDIV(n)		(uint8_t)(((n) & 0x07) << 1)	// Fast Clock Internal Reference Divider
#define MCG_SC_FLTPRSRV			(uint8_t)0x10			// FLL Filter Preserve Enable
#define MCG_SC_ATMF			(uint8_t)0x20			// Automatic Trim Machine Fail Flag
#define MCG_SC_ATMS			(uint8_t)0x40			// Automatic Trim Machine Select
#define MCG_SC_ATME			(uint8_t)0x80			// Automatic Trim Machine Enable
#define MCG_ATCVH               (KINETIS_MCG.ATCVH)		// 4006400A  MCG Auto Trim Compare Value High Register
#define MCG_ATCVL               (KINETIS_MCG.ATCVL)		// 4006400B  MCG Auto Trim Compare Value Low Register
#define MCG_C7                  (KINETIS_MCG.C7)		// 4006400C  MCG Control 7 Register
#define MCG_C8                  (KINETIS_MCG.C8)		// 4006400D  MCG Control 8 Register
#define MCG_C9			(KINETIS_MCG.C9)		// 4006400E  MCG Control 9 Register
#define MCG_C11			(KINETIS_MCG.C11)		// 40064010  MCG Control 11 Register
#define MCG_C12			(KINETIS_MCG.C12)		// 40064011  MCG Control 12 Register
#define MCG_S2			(KINETIS_MCG.S2)		// 40064012  MCG Status 2 Register
#define MCG_T3			(KINETIS_MCG.T3)		// 40064013  MCG Test 3 Register


// Oscillator (OSC)
#define OSC0_CR			(*(volatile uint8_t  *)0x40065000) // OSC Control Register
#define OSC_SC16P			((uint8_t)0x01)			// Oscillator 16 pF Capacitor Load Configure
#define OSC_SC8P			((uint8_t)0x02)			// Oscillator 8 pF Capacitor Load Configure
#define OSC_SC4P			((uint8_t)0x04)			// Oscillator 4 pF Capacitor Load Configure
#define OSC_SC2P			((uint8_t)0x08)			// Oscillator 2 pF Capacitor Load Configure
#define OSC_EREFSTEN			((uint8_t)0x20)			// External Reference Stop Enable, Controls whether or not the external reference clock (OSCERCLK) remains enabled when MCU enters Stop mode.
#define OSC_ERCLKEN			((uint8_t)0x80)			// External Reference Enable, Enables external reference clock (OSCERCLK).
#define OSC0_OSC_DIV		(*(volatile uint8_t  *)0x40065002) // Clock divider register


// Local Memory Controller

#define LMEM_PCCCR		(*(volatile uint32_t *)0xE0082000) // Cache control register
#define LMEM_PCCLCR		(*(volatile uint32_t *)0xE0082004) // Cache line control register
#define LMEM_PCCSAR		(*(volatile uint32_t *)0xE0082008) // Cache search address register
#define LMEM_PCCCVR		(*(volatile uint32_t *)0xE008200C) // Cache read/write value register
#define LMEM_PCCRMR		(*(volatile uint32_t *)0xE0082020) // Cache regions mode register


// Flash Memory Controller (FMC)

#define FMC_PFAPR		(*(volatile uint32_t *)0x4001F000) // Flash Access Protection
#define FMC_PFB0CR		(*(volatile uint32_t *)0x4001F004) // Flash Control
#define FMC_TAGVDW0S0		(*(volatile uint32_t *)0x4001F100) // Cache Tag Storage
#define FMC_TAGVDW0S1		(*(volatile uint32_t *)0x4001F104) // Cache Tag Storage
#define FMC_TAGVDW1S0		(*(volatile uint32_t *)0x4001F108) // Cache Tag Storage
#define FMC_TAGVDW1S1		(*(volatile uint32_t *)0x4001F10C) // Cache Tag Storage
#define FMC_TAGVDW2S0		(*(volatile uint32_t *)0x4001F110) // Cache Tag Storage
#define FMC_TAGVDW2S1		(*(volatile uint32_t *)0x4001F114) // Cache Tag Storage
#define FMC_TAGVDW3S0		(*(volatile uint32_t *)0x4001F118) // Cache Tag Storage
#define FMC_TAGVDW3S1		(*(volatile uint32_t *)0x4001F11C) // Cache Tag Storage
#define FMC_DATAW0S0		(*(volatile uint32_t *)0x4001F200) // Cache Data Storage
#define FMC_DATAW0S1		(*(volatile uint32_t *)0x4001F204) // Cache Data Storage
#define FMC_DATAW1S0		(*(volatile uint32_t *)0x4001F208) // Cache Data Storage
#define FMC_DATAW1S1		(*(volatile uint32_t *)0x4001F20C) // Cache Data Storage
#define FMC_DATAW2S0		(*(volatile uint32_t *)0x4001F210) // Cache Data Storage
#define FMC_DATAW2S1		(*(volatile uint32_t *)0x4001F214) // Cache Data Storage
#define FMC_DATAW3S0		(*(volatile uint32_t *)0x4001F218) // Cache Data Storage
#define FMC_DATAW3S1		(*(volatile uint32_t *)0x4001F21C) // Cache Data Storage

// Flash Memory Module (FTFL)

#define FTFL_FSTAT		(*(volatile uint8_t  *)0x40020000) // Flash Status Register
#define FTFL_FSTAT_CCIF			((uint8_t)0x80)			// Command Complete Interrupt Flag
#define FTFL_FSTAT_RDCOLERR		((uint8_t)0x40)			// Flash Read Collision Error Flag
#define FTFL_FSTAT_ACCERR		((uint8_t)0x20)			// Flash Access Error Flag
#define FTFL_FSTAT_FPVIOL		((uint8_t)0x10)			// Flash Protection Violation Flag
#define FTFL_FSTAT_MGSTAT0		((uint8_t)0x01)			// Memory Controller Command Completion Status Flag
#define FTFL_FCNFG		(*(volatile uint8_t  *)0x40020001) // Flash Configuration Register
#define FTFL_FCNFG_CCIE			((uint8_t)0x80)			// Command Complete Interrupt Enable
#define FTFL_FCNFG_RDCOLLIE		((uint8_t)0x40)			// Read Collision Error Interrupt Enable
#define FTFL_FCNFG_ERSAREQ		((uint8_t)0x20)			// Erase All Request
#define FTFL_FCNFG_ERSSUSP		((uint8_t)0x10)			// Erase Suspend
#define FTFL_FCNFG_PFLSH		((uint8_t)0x04)			// Flash memory configuration
#define FTFL_FCNFG_RAMRDY		((uint8_t)0x02)			// RAM Ready
#define FTFL_FCNFG_EEERDY		((uint8_t)0x01)			// EEPROM Ready
#define FTFL_FSEC		(*(const    uint8_t  *)0x40020002) // Flash Security Register
#define FTFL_FOPT		(*(const    uint8_t  *)0x40020003) // Flash Option Register
#define FTFL_FCCOB3		(*(volatile uint8_t  *)0x40020004) // Flash Common Command Object Registers
#define FTFL_FCCOB2		(*(volatile uint8_t  *)0x40020005)
#define FTFL_FCCOB1		(*(volatile uint8_t  *)0x40020006)
#define FTFL_FCCOB0		(*(volatile uint8_t  *)0x40020007)
#define FTFL_FCCOB7		(*(volatile uint8_t  *)0x40020008)
#define FTFL_FCCOB6		(*(volatile uint8_t  *)0x40020009)
#define FTFL_FCCOB5		(*(volatile uint8_t  *)0x4002000A)
#define FTFL_FCCOB4		(*(volatile uint8_t  *)0x4002000B)
#define FTFL_FCCOBB		(*(volatile uint8_t  *)0x4002000C)
#define FTFL_FCCOBA		(*(volatile uint8_t  *)0x4002000D)
#define FTFL_FCCOB9		(*(volatile uint8_t  *)0x4002000E)
#define FTFL_FCCOB8		(*(volatile uint8_t  *)0x4002000F)
#define FTFL_FPROT3		(*(volatile uint8_t  *)0x40020010) // Program Flash Protection Registers
#define FTFL_FPROT2		(*(volatile uint8_t  *)0x40020011) // Program Flash Protection Registers
#define FTFL_FPROT1		(*(volatile uint8_t  *)0x40020012) // Program Flash Protection Registers
#define FTFL_FPROT0		(*(volatile uint8_t  *)0x40020013) // Program Flash Protection Registers
#define FTFL_FEPROT		(*(volatile uint8_t  *)0x40020016) // EEPROM Protection Register
#define FTFL_FDPROT		(*(volatile uint8_t  *)0x40020017) // Data Flash Protection Register

// Cyclic Redundancy Check (CRC)

#define CRC_CRC			(*(volatile uint32_t *)0x40032000) // CRC Data register
#define CRC_GPOLY		(*(volatile uint32_t *)0x40032004) // CRC Polynomial register
#define CRC_CTRL		(*(volatile uint32_t *)0x40032008) // CRC Control register

// Cryptographic Acceleration Unit (CAU)

#define CAU_CASR		(*(volatile uint32_t *)0xE0081000) // Status Register
#define CAU_CAA			(*(volatile uint32_t *)0xE0081001) // Accumulator
#define CAU_CA0			(*(volatile uint32_t *)0xE0081002) // General Purpose Register
#define CAU_CA1			(*(volatile uint32_t *)0xE0081003) // General Purpose Register
#define CAU_CA2			(*(volatile uint32_t *)0xE0081004) // General Purpose Register
#define CAU_CA3			(*(volatile uint32_t *)0xE0081005) // General Purpose Register
#define CAU_CA4			(*(volatile uint32_t *)0xE0081006) // General Purpose Register
#define CAU_CA5			(*(volatile uint32_t *)0xE0081007) // General Purpose Register
#define CAU_CA6			(*(volatile uint32_t *)0xE0081008) // General Purpose Register
#define CAU_CA7			(*(volatile uint32_t *)0xE0081009) // General Purpose Register
#define CAU_CA8			(*(volatile uint32_t *)0xE008100A) // General Purpose Register

// Random Number Generator Accelerator (RNGA)

#define RNG_CR			(*(volatile uint32_t *)0x40029000) // RNGA Control Register
#define RNG_SR			(*(volatile uint32_t *)0x40029004) // RNGA Status Register
#define RNG_ER			(*(volatile uint32_t *)0x40029008) // RNGA Entropy Register
#define RNG_OR			(*(volatile uint32_t *)0x4002900C) // RNGA Output Register

// Analog-to-Digital Converter (ADC)

#define ADC0_SC1A		(*(volatile uint32_t *)0x4003B000) // ADC status and control registers 1
#define ADC0_SC1B		(*(volatile uint32_t *)0x4003B004) // ADC status and control registers 1
#define ADC_SC1_COCO			((uint32_t)0x80)		// Conversion complete flag
#define ADC_SC1_AIEN			((uint32_t)0x40)		// Interrupt enable
#define ADC_SC1_DIFF			((uint32_t)0x20)		// Differential mode enable
#define ADC_SC1_ADCH(n)			((uint32_t)((n) & 0x1F))		// Input channel select
#define ADC0_CFG1		(*(volatile uint32_t *)0x4003B008) // ADC configuration register 1
#define ADC_CFG1_ADLPC			((uint32_t)0x80)		// Low-power configuration
#define ADC_CFG1_ADIV(n)		((uint32_t)(((n) & 3) << 5))	// Clock divide select, 0=direct, 1=div2, 2=div4, 3=div8
#define ADC_CFG1_ADLSMP			((uint32_t)0x10)		// Sample time configuration, 0=Short, 1=Long
#define ADC_CFG1_MODE(n)		((uint32_t)(((n) & 3) << 2))	// Conversion mode, 0=8 bit, 1=12 bit, 2=10 bit, 3=16 bit
#define ADC_CFG1_ADICLK(n)		((uint32_t)(((n) & 3) << 0))	// Input clock, 0=bus, 1=bus/2, 2=OSCERCLK, 3=async
#define ADC0_CFG2		(*(volatile uint32_t *)0x4003B00C) // Configuration register 2
#define ADC_CFG2_MUXSEL			((uint32_t)0x10)		// 0=a channels, 1=b channels
#define ADC_CFG2_ADACKEN		((uint32_t)0x08)		// async clock enable
#define ADC_CFG2_ADHSC			((uint32_t)0x04)		// High speed configuration
#define ADC_CFG2_ADLSTS(n)		((uint32_t)(((n) & 3) << 0))	// Sample time, 0=24 cycles, 1=12 cycles, 2=6 cycles, 3=2 cycles
#define ADC0_RA			(*(volatile uint32_t *)0x4003B010) // ADC data result register
#define ADC0_RB			(*(volatile uint32_t *)0x4003B014) // ADC data result register
#define ADC0_CV1		(*(volatile uint32_t *)0x4003B018) // Compare value registers
#define ADC0_CV2		(*(volatile uint32_t *)0x4003B01C) // Compare value registers
#define ADC0_SC2		(*(volatile uint32_t *)0x4003B020) // Status and control register 2
#define ADC_SC2_ADACT			((uint32_t)0x80)		// Conversion active
#define ADC_SC2_ADTRG			((uint32_t)0x40)		// Conversion trigger select, 0=software, 1=hardware
#define ADC_SC2_ACFE			((uint32_t)0x20)		// Compare function enable
#define ADC_SC2_ACFGT			((uint32_t)0x10)		// Compare function greater than enable
#define ADC_SC2_ACREN			((uint32_t)0x08)		// Compare function range enable
#define ADC_SC2_DMAEN			((uint32_t)0x04)		// DMA enable
#define ADC_SC2_REFSEL(n)		((uint32_t)(((n) & 3) << 0))	// Voltage reference, 0=vcc/external, 1=1.2 volts
#define ADC0_SC3		(*(volatile uint32_t *)0x4003B024) // Status and control register 3
#define ADC_SC3_CAL			((uint32_t)0x80)		// Calibration, 1=begin, stays set while cal in progress
#define ADC_SC3_CALF			((uint32_t)0x40)		// Calibration failed flag
#define ADC_SC3_ADCO			((uint32_t)0x08)		// Continuous conversion enable
#define ADC_SC3_AVGE			((uint32_t)0x04)		// Hardware average enable
#define ADC_SC3_AVGS(n)			((uint32_t)(((n) & 3) << 0))	// avg select, 0=4 samples, 1=8 samples, 2=16 samples, 3=32 samples
#define ADC0_OFS		(*(volatile uint32_t *)0x4003B028) // ADC offset correction register
#define ADC0_PG			(*(volatile uint32_t *)0x4003B02C) // ADC plus-side gain register
#define ADC0_MG			(*(volatile uint32_t *)0x4003B030) // ADC minus-side gain register
#define ADC0_CLPD		(*(volatile uint32_t *)0x4003B034) // ADC plus-side general calibration value register
#define ADC0_CLPS		(*(volatile uint32_t *)0x4003B038) // ADC plus-side general calibration value register
#define ADC0_CLP4		(*(volatile uint32_t *)0x4003B03C) // ADC plus-side general calibration value register
#define ADC0_CLP3		(*(volatile uint32_t *)0x4003B040) // ADC plus-side general calibration value register
#define ADC0_CLP2		(*(volatile uint32_t *)0x4003B044) // ADC plus-side general calibration value register
#define ADC0_CLP1		(*(volatile uint32_t *)0x4003B048) // ADC plus-side general calibration value register
#define ADC0_CLP0		(*(volatile uint32_t *)0x4003B04C) // ADC plus-side general calibration value register
#define ADC0_PGA		(*(volatile uint32_t *)0x4003B050) // ADC Programmable Gain Amplifier
#define ADC_PGA_PGAEN			((uint32_t)0x00800000)		// Enable
#define ADC_PGA_PGALPB			((uint32_t)0x00100000)		// Low-Power Mode Control, 0=low power, 1=normal
#define ADC_PGA_PGAG(n)			((uint32_t)(((n) & 15) << 16))	// Gain, 0=1X, 1=2X, 2=4X, 3=8X, 4=16X, 5=32X, 6=64X
#define ADC0_CLMD		(*(volatile uint32_t *)0x4003B054) // ADC minus-side general calibration value register
#define ADC0_CLMS		(*(volatile uint32_t *)0x4003B058) // ADC minus-side general calibration value register
#define ADC0_CLM4		(*(volatile uint32_t *)0x4003B05C) // ADC minus-side general calibration value register
#define ADC0_CLM3		(*(volatile uint32_t *)0x4003B060) // ADC minus-side general calibration value register
#define ADC0_CLM2		(*(volatile uint32_t *)0x4003B064) // ADC minus-side general calibration value register
#define ADC0_CLM1		(*(volatile uint32_t *)0x4003B068) // ADC minus-side general calibration value register
#define ADC0_CLM0		(*(volatile uint32_t *)0x4003B06C) // ADC minus-side general calibration value register

#define ADC1_SC1A		(*(volatile uint32_t *)0x400BB000) // ADC status and control registers 1
#define ADC1_SC1B		(*(volatile uint32_t *)0x400BB004) // ADC status and control registers 1
#define ADC1_CFG1		(*(volatile uint32_t *)0x400BB008) // ADC configuration register 1
#define ADC1_CFG2		(*(volatile uint32_t *)0x400BB00C) // Configuration register 2
#define ADC1_RA			(*(volatile uint32_t *)0x400BB010) // ADC data result register
#define ADC1_RB			(*(volatile uint32_t *)0x400BB014) // ADC data result register
#define ADC1_CV1		(*(volatile uint32_t *)0x400BB018) // Compare value registers
#define ADC1_CV2		(*(volatile uint32_t *)0x400BB01C) // Compare value registers
#define ADC1_SC2		(*(volatile uint32_t *)0x400BB020) // Status and control register 2
#define ADC1_SC3		(*(volatile uint32_t *)0x400BB024) // Status and control register 3
#define ADC1_OFS		(*(volatile uint32_t *)0x400BB028) // ADC offset correction register
#define ADC1_PG			(*(volatile uint32_t *)0x400BB02C) // ADC plus-side gain register
#define ADC1_MG			(*(volatile uint32_t *)0x400BB030) // ADC minus-side gain register
#define ADC1_CLPD		(*(volatile uint32_t *)0x400BB034) // ADC plus-side general calibration value register
#define ADC1_CLPS		(*(volatile uint32_t *)0x400BB038) // ADC plus-side general calibration value register
#define ADC1_CLP4		(*(volatile uint32_t *)0x400BB03C) // ADC plus-side general calibration value register
#define ADC1_CLP3		(*(volatile uint32_t *)0x400BB040) // ADC plus-side general calibration value register
#define ADC1_CLP2		(*(volatile uint32_t *)0x400BB044) // ADC plus-side general calibration value register
#define ADC1_CLP1		(*(volatile uint32_t *)0x400BB048) // ADC plus-side general calibration value register
#define ADC1_CLP0		(*(volatile uint32_t *)0x400BB04C) // ADC plus-side general calibration value register
#define ADC1_PGA		(*(volatile uint32_t *)0x400BB050) // ADC Programmable Gain Amplifier
#define ADC1_CLMD		(*(volatile uint32_t *)0x400BB054) // ADC minus-side general calibration value register
#define ADC1_CLMS		(*(volatile uint32_t *)0x400BB058) // ADC minus-side general calibration value register
#define ADC1_CLM4		(*(volatile uint32_t *)0x400BB05C) // ADC minus-side general calibration value register
#define ADC1_CLM3		(*(volatile uint32_t *)0x400BB060) // ADC minus-side general calibration value register
#define ADC1_CLM2		(*(volatile uint32_t *)0x400BB064) // ADC minus-side general calibration value register
#define ADC1_CLM1		(*(volatile uint32_t *)0x400BB068) // ADC minus-side general calibration value register
#define ADC1_CLM0		(*(volatile uint32_t *)0x400BB06C) // ADC minus-side general calibration value register


// 12-bit Digital-to-Analog Converter (DAC)

#if defined(KINETISK)
#define DAC0_DAT0L		(*(volatile uint8_t  *)0x400CC000) // DAC Data Low Register
#define DAC0_DATH		(*(volatile uint8_t  *)0x400CC001) // DAC Data High Register
#define DAC0_DAT1L		(*(volatile uint8_t  *)0x400CC002) // DAC Data Low Register
#define DAC0_DAT2L		(*(volatile uint8_t  *)0x400CC004) // DAC Data Low Register
#define DAC0_DAT3L		(*(volatile uint8_t  *)0x400CC006) // DAC Data Low Register
#define DAC0_DAT4L		(*(volatile uint8_t  *)0x400CC008) // DAC Data Low Register
#define DAC0_DAT5L		(*(volatile uint8_t  *)0x400CC00A) // DAC Data Low Register
#define DAC0_DAT6L		(*(volatile uint8_t  *)0x400CC00C) // DAC Data Low Register
#define DAC0_DAT7L		(*(volatile uint8_t  *)0x400CC00E) // DAC Data Low Register
#define DAC0_DAT8L		(*(volatile uint8_t  *)0x400CC010) // DAC Data Low Register
#define DAC0_DAT9L		(*(volatile uint8_t  *)0x400CC012) // DAC Data Low Register
#define DAC0_DAT10L		(*(volatile uint8_t  *)0x400CC014) // DAC Data Low Register
#define DAC0_DAT11L		(*(volatile uint8_t  *)0x400CC016) // DAC Data Low Register
#define DAC0_DAT12L		(*(volatile uint8_t  *)0x400CC018) // DAC Data Low Register
#define DAC0_DAT13L		(*(volatile uint8_t  *)0x400CC01A) // DAC Data Low Register
#define DAC0_DAT14L		(*(volatile uint8_t  *)0x400CC01C) // DAC Data Low Register
#define DAC0_DAT15L		(*(volatile uint8_t  *)0x400CC01E) // DAC Data Low Register
#define DAC0_SR			(*(volatile uint8_t  *)0x400CC020) // DAC Status Register
#define DAC0_C0			(*(volatile uint8_t  *)0x400CC021) // DAC Control Register
#define DAC_C0_DACEN			0x80				// DAC Enable
#define DAC_C0_DACRFS			0x40				// DAC Reference Select
#define DAC_C0_DACTRGSEL		0x20				// DAC Trigger Select
#define DAC_C0_DACSWTRG			0x10				// DAC Software Trigger
#define DAC_C0_LPEN			0x08				// DAC Low Power Control
#define DAC_C0_DACBWIEN			0x04				// DAC Buffer Watermark Interrupt Enable
#define DAC_C0_DACBTIEN			0x02				// DAC Buffer Read Pointer Top Flag Interrupt Enable
#define DAC_C0_DACBBIEN			0x01				// DAC Buffer Read Pointer Bottom Flag Interrupt Enable
#define DAC0_C1			(*(volatile uint8_t  *)0x400CC022) // DAC Control Register 1
#define DAC_C1_DMAEN			0x80				// DMA Enable Select
#define DAC_C1_DACBFWM(n)		((((n) & 3) << 3))		// DAC Buffer Watermark Select
#define DAC_C1_DACBFMD(n)		((((n) & 3) << 1))		// DAC Buffer Work Mode Select
#define DAC_C1_DACBFEN			0x01				// DAC Buffer Enable
#define DAC0_C2			(*(volatile uint8_t  *)0x400CC023) // DAC Control Register 2
#define DAC_C2_DACBFRP(n)		((((n) & 15) << 4))		// DAC Buffer Read Pointer
#define DAC_C2_DACBFUP(n)		((((n) & 15) << 0))		// DAC Buffer Upper Limit
#define DAC1_DAT0L		(*(volatile uint8_t  *)0x400CD000) // DAC Data Low Register
#define DAC1_DATH		(*(volatile uint8_t  *)0x400CD001) // DAC Data High Register
#define DAC1_DAT1L		(*(volatile uint8_t  *)0x400CD002) // DAC Data Low Register
#define DAC1_DAT2L		(*(volatile uint8_t  *)0x400CD004) // DAC Data Low Register
#define DAC1_DAT3L		(*(volatile uint8_t  *)0x400CD006) // DAC Data Low Register
#define DAC1_DAT4L		(*(volatile uint8_t  *)0x400CD008) // DAC Data Low Register
#define DAC1_DAT5L		(*(volatile uint8_t  *)0x400CD00A) // DAC Data Low Register
#define DAC1_DAT6L		(*(volatile uint8_t  *)0x400CD00C) // DAC Data Low Register
#define DAC1_DAT7L		(*(volatile uint8_t  *)0x400CD00E) // DAC Data Low Register
#define DAC1_DAT8L		(*(volatile uint8_t  *)0x400CD010) // DAC Data Low Register
#define DAC1_DAT9L		(*(volatile uint8_t  *)0x400CD012) // DAC Data Low Register
#define DAC1_DAT10L		(*(volatile uint8_t  *)0x400CD014) // DAC Data Low Register
#define DAC1_DAT11L		(*(volatile uint8_t  *)0x400CD016) // DAC Data Low Register
#define DAC1_DAT12L		(*(volatile uint8_t  *)0x400CD018) // DAC Data Low Register
#define DAC1_DAT13L		(*(volatile uint8_t  *)0x400CD01A) // DAC Data Low Register
#define DAC1_DAT14L		(*(volatile uint8_t  *)0x400CD01C) // DAC Data Low Register
#define DAC1_DAT15L		(*(volatile uint8_t  *)0x400CD01E) // DAC Data Low Register
#define DAC1_SR			(*(volatile uint8_t  *)0x400CD020) // DAC Status Register
#define DAC1_C0			(*(volatile uint8_t  *)0x400CD021) // DAC Control Register
#define DAC1_C1			(*(volatile uint8_t  *)0x400CD022) // DAC Control Register 1
#define DAC1_C2			(*(volatile uint8_t  *)0x400CD023) // DAC Control Register 2

#elif defined(KINETISL)
#define DAC0_DAT0L		(*(volatile uint8_t  *)0x4003F000) // Data Low
#define DAC0_DAT0H		(*(volatile uint8_t  *)0x4003F001) // Data High
#define DAC0_DAT1L		(*(volatile uint8_t  *)0x4003F002) // Data Low
#define DAC0_DAT1H		(*(volatile uint8_t  *)0x4003F003) // Data High
#define DAC0_SR			(*(volatile uint8_t  *)0x4003F020) // Status
#define DAC0_C0			(*(volatile uint8_t  *)0x4003F021) // Control Register
#define DAC0_C1			(*(volatile uint8_t  *)0x4003F022) // Control Register 1
#define DAC0_C2			(*(volatile uint8_t  *)0x4003F023) // Control Register 2
#define DAC_SR_DACBFRPTF		((uint8_t)0x02)		// Read Pointer Top Position Flag
#define DAC_SR_DACBFRPBF		((uint8_t)0x01)		// Read Pointer Bottom Position Flag
#define DAC_C0_DACEN			((uint8_t)0x80)		// Enable
#define DAC_C0_DACRFS			((uint8_t)0x40)		// Reference, 0=AREF pin, 1=VCC
#define DAC_C0_DACTRGSEL		((uint8_t)0x20)		// Trigger Select
#define DAC_C0_DACSWTRG			((uint8_t)0x10)		// Software Trigger
#define DAC_C0_LPEN			((uint8_t)0x08)		// Low Power Control
#define DAC_C0_DACBTIEN			((uint8_t)0x02)		// Top Flag Interrupt Enable
#define DAC_C0_DACBBIEN			((uint8_t)0x01)		// Bottom Flag Interrupt Enable
#define DAC_C1_DMAEN			((uint8_t)0x80)		// DMA Enable
#define DAC_C1_DACBFMD			((uint8_t)0x04)		// Work Mode Select
#define DAC_C1_DACBFEN			((uint8_t)0x01)		// Buffer Enable
#define DAC_C2_DACBFRP			((uint8_t)0x10)		// Buffer Read Pointer
#define DAC_C2_DACBFUP			((uint8_t)0x01)		// Buffer Upper Limit
#endif

// Analog Comparator (CMP)

#define CMP0_CR0		(*(volatile uint8_t  *)0x40073000) // CMP Control Register 0
#define CMP_CR0_FILTER_CNT(n)   (uint8_t)(((n) & 0x07) << 4)
#define CMP_CR0_HYSTCTR(n)      (uint8_t)(((n) & 0x03) << 0)
#define CMP0_CR1		(*(volatile uint8_t  *)0x40073001) // CMP Control Register
#define CMP_CR1_SE              (uint8_t)0x80 // Sample Enable
#define CMP_CR1_WE              (uint8_t)0x40 // Windowing Enable
#define CMP_CR1_TRIGM           (uint8_t)0x20 // Trigger Mode Enable
#define CMP_CR1_PMODE           (uint8_t)0x10 // Power Mode Select
#define CMP_CR1_INV             (uint8_t)0x08 // Comparator INVERT
#define CMP_CR1_COS             (uint8_t)0x04 // Comparator Output Select
#define CMP_CR1_OPE             (uint8_t)0x02 // Comparator Output Pin Enable
#define CMP_CR1_EN              (uint8_t)0x01 // Comparator Module Enable
#define CMP0_FPR		(*(volatile uint8_t  *)0x40073002) // CMP Filter Period Register
#define CMP0_SCR		(*(volatile uint8_t  *)0x40073003) // CMP Status and Control Register
#define CMP_SCR_DMAEN           (uint8_t)0x40 // DMA Enable Control
#define CMP_SCR_IER             (uint8_t)0x10 // Comparator Interrupt Enable Rising
#define CMP_SCR_IEF             (uint8_t)0x08 // Comparator Interrupt Enable Falling
#define CMP_SCR_CFR             (uint8_t)0x04 // Analog Comparator Flag Rising
#define CMP_SCR_CFF             (uint8_t)0x02 // Analog Comparator Flag Falling
#define CMP_SCR_COUT            (uint8_t)0x01 // Analog Comparator Output
#define CMP0_DACCR		(*(volatile uint8_t  *)0x40073004) // DAC Control Register
#define CMP_DACCR_DACEN         (uint8_t)0x80 // DAC Enable
#define CMP_DACCR_VRSEL         (uint8_t)0x40 // Supply Voltage Reference Source Select
#define CMP_DACCR_VOSEL(n)      (uint8_t)(((n) & 0x3F) << 0) // DAC Output Voltage Select
#define CMP0_MUXCR		(*(volatile uint8_t  *)0x40073005) // MUX Control Register
#define CMP_MUXCR_PSTM          (uint8_t)0x40 // Pass Through Mode Enable
#define CMP_MUXCR_PSEL(n)       (uint8_t)(((n) & 0x07) << 3) // Plus Input Mux Control
#define CMP_MUXCR_MSEL(n)       (uint8_t)(((n) & 0x07) << 0) // Minus Input Mux Control
#define CMP1_CR0		(*(volatile uint8_t  *)0x40073008) // CMP Control Register 0
#define CMP1_CR1		(*(volatile uint8_t  *)0x40073009) // CMP Control Register 1
#define CMP1_FPR		(*(volatile uint8_t  *)0x4007300A) // CMP Filter Period Register
#define CMP1_SCR		(*(volatile uint8_t  *)0x4007300B) // CMP Status and Control Register
#define CMP1_DACCR		(*(volatile uint8_t  *)0x4007300C) // DAC Control Register
#define CMP1_MUXCR		(*(volatile uint8_t  *)0x4007300D) // MUX Control Register
#define CMP2_CR0		(*(volatile uint8_t  *)0x40073010) // CMP Control Register 0
#define CMP2_CR1		(*(volatile uint8_t  *)0x40073011) // CMP Control Register 1
#define CMP2_FPR		(*(volatile uint8_t  *)0x40073012) // CMP Filter Period Register
#define CMP2_SCR		(*(volatile uint8_t  *)0x40073013) // CMP Status and Control Register
#define CMP2_DACCR		(*(volatile uint8_t  *)0x40073014) // DAC Control Register
#define CMP2_MUXCR		(*(volatile uint8_t  *)0x40073015) // MUX Control Register
#define CMP3_CR0		(*(volatile uint8_t  *)0x40073018) // CMP Control Register 0
#define CMP3_CR1		(*(volatile uint8_t  *)0x40073019) // CMP Control Register 1
#define CMP3_FPR		(*(volatile uint8_t  *)0x4007301A) // CMP Filter Period Register
#define CMP3_SCR		(*(volatile uint8_t  *)0x4007301B) // CMP Status and Control Register
#define CMP3_DACCR		(*(volatile uint8_t  *)0x4007301C) // DAC Control Register
#define CMP3_MUXCR		(*(volatile uint8_t  *)0x4007301D) // MUX Control Register

// Analog Voltage Reference (VREFV1)

#define VREF_TRM		(*(volatile uint8_t  *)0x40074000) // VREF Trim Register
#define VREF_TRM_CHOPEN			((uint8_t)0x40)			// Chop oscillator enable
#define VREF_TRM_TRIM(n)		((n) & 0x3F)			// Trim bits
#define VREF_SC			(*(volatile uint8_t  *)0x40074001) // VREF Status and Control Register
#define VREF_SC_VREFEN			((uint8_t)0x80)			// Internal Voltage Reference enable
#define VREF_SC_REGEN			((uint8_t)0x40)			// Regulator enable
#define VREF_SC_ICOMPEN			((uint8_t)0x20)			// Second order curvature compensation enable
#define VREF_SC_VREFST			((uint8_t)0x04)			// Internal Voltage Reference stable flag
#define VREF_SC_MODE_LV(n)		(uint8_t)(((n) & 3) << 0)	// Buffer Mode selection: 0=Bandgap on only
									//  2=Low-power buffer mode

// Programmable Delay Block (PDB)

#define PDB0_SC			(*(volatile uint32_t *)0x40036000) // Status and Control Register
#define PDB_SC_LDMOD(n)			(((n) & 3) << 18)	// Load Mode Select
#define PDB_SC_PDBEIE			0x00020000		// Sequence Error Interrupt Enable
#define PDB_SC_SWTRIG			0x00010000		// Software Trigger
#define PDB_SC_DMAEN			0x00008000		// DMA Enable
#define PDB_SC_PRESCALER(n)		(((n) & 7) << 12)	// Prescaler Divider Select
#define PDB_SC_TRGSEL(n)		(((n) & 15) << 8)	// Trigger Input Source Select
#define PDB_SC_PDBEN			0x00000080		// PDB Enable
#define PDB_SC_PDBIF			0x00000040		// PDB Interrupt Flag
#define PDB_SC_PDBIE			0x00000020		// PDB Interrupt Enable.
#define PDB_SC_MULT(n)			(((n) & 3) << 2)	// Multiplication Factor
#define PDB_SC_CONT			0x00000002		// Continuous Mode Enable
#define PDB_SC_LDOK			0x00000001		// Load OK
#define PDB0_MOD		(*(volatile uint32_t *)0x40036004) // Modulus Register
#define PDB0_CNT		(*(volatile uint32_t *)0x40036008) // Counter Register
#define PDB0_IDLY		(*(volatile uint32_t *)0x4003600C) // Interrupt Delay Register
#define PDB0_CH0C1		(*(volatile uint32_t *)0x40036010) // Channel 0 Control Register 1
#define PDB0_CH0S		(*(volatile uint32_t *)0x40036014) // Channel 0 Status Register
#define PDB0_CH0DLY0		(*(volatile uint32_t *)0x40036018) // Channel 0 Delay 0 Register
#define PDB0_CH0DLY1		(*(volatile uint32_t *)0x4003601C) // Channel 0 Delay 1 Register
#define PDB0_CH1C1		(*(volatile uint32_t *)0x40036038) // Channel 1 Control Register 1
#define PDB0_CH1S		(*(volatile uint32_t *)0x4003603C) // Channel 1 Status Register
#define PDB0_CH1DLY0		(*(volatile uint32_t *)0x40036040) // Channel 1 Delay 0 Register
#define PDB0_CH1DLY1		(*(volatile uint32_t *)0x40036044) // Channel 1 Delay 1 Register
#define PDB0_DACINTC0		(*(volatile uint32_t *)0x40036150) // DAC Interval Trigger n Control Register
#define PDB0_DACINT0		(*(volatile uint32_t *)0x40036154) // DAC Interval n Register
#define PDB0_DACINTC1		(*(volatile uint32_t *)0x40036158) // DAC Interval Trigger n Control register
#define PDB0_DACINT1		(*(volatile uint32_t *)0x4003615C) // DAC Interval n register
#define PDB0_POEN		(*(volatile uint32_t *)0x40036190) // Pulse-Out n Enable Register
#define PDB0_PO0DLY		(*(volatile uint32_t *)0x40036194) // Pulse-Out n Delay Register
#define PDB0_PO1DLY		(*(volatile uint32_t *)0x40036198) // Pulse-Out n Delay Register
#define PDB0_PO2DLY		(*(volatile uint32_t *)0x4003619C) // Pulse-Out n Delay Register
#define PDB0_PO3DLY		(*(volatile uint32_t *)0x400361A0) // Pulse-Out n Delay Register


// Timer/PWM Module (TPM)

#if defined(KINETISL)
#define TPM0_SC			(*(volatile uint32_t *)0x40038000) // Status And Control
#define TPM0_CNT		(*(volatile uint32_t *)0x40038004) // Counter
#define TPM0_MOD		(*(volatile uint32_t *)0x40038008) // Modulo
#define TPM0_C0SC		(*(volatile uint32_t *)0x4003800C) // Channel 0 Status And Control
#define TPM0_C0V		(*(volatile uint32_t *)0x40038010) // Channel 0 Value
#define TPM0_C1SC		(*(volatile uint32_t *)0x40038014) // Channel 1 Status And Control
#define TPM0_C1V		(*(volatile uint32_t *)0x40038018) // Channel 1 Value
#define TPM0_C2SC		(*(volatile uint32_t *)0x4003801C) // Channel 2 Status And Control
#define TPM0_C2V		(*(volatile uint32_t *)0x40038020) // Channel 2 Value
#define TPM0_C3SC		(*(volatile uint32_t *)0x40038024) // Channel 3 Status And Control
#define TPM0_C3V		(*(volatile uint32_t *)0x40038028) // Channel 3 Value
#define TPM0_C4SC		(*(volatile uint32_t *)0x4003802C) // Channel 4 Status And Control
#define TPM0_C4V		(*(volatile uint32_t *)0x40038030) // Channel 4 Value
#define TPM0_C5SC		(*(volatile uint32_t *)0x40038034) // Channel 5 Status And Control
#define TPM0_C5V		(*(volatile uint32_t *)0x40038038) // Channel 5 Value
#define TPM0_STATUS		(*(volatile uint32_t *)0x40038050) // Capture And Compare Status
#define TPM0_CONF		(*(volatile uint32_t *)0x40038084) // Configuration
#define TPM1_SC			(*(volatile uint32_t *)0x40039000) // Status And Control
#define TPM1_CNT		(*(volatile uint32_t *)0x40039004) // Counter
#define TPM1_MOD		(*(volatile uint32_t *)0x40039008) // Modulo
#define TPM1_C0SC		(*(volatile uint32_t *)0x4003900C) // Channel 0 Status And Control
#define TPM1_C0V		(*(volatile uint32_t *)0x40039010) // Channel 0 Value
#define TPM1_C1SC		(*(volatile uint32_t *)0x40039014) // Channel 1 Status And Control
#define TPM1_C1V		(*(volatile uint32_t *)0x40039018) // Channel 1 Value
#define TPM1_STATUS		(*(volatile uint32_t *)0x40039050) // Capture And Compare Status
#define TPM1_CONF		(*(volatile uint32_t *)0x40039084) // Configuration
#define TPM2_SC			(*(volatile uint32_t *)0x4003A000) // Status And Control
#define TPM2_CNT		(*(volatile uint32_t *)0x4003A004) // Counter
#define TPM2_MOD		(*(volatile uint32_t *)0x4003A008) // Modulo
#define TPM2_C0SC		(*(volatile uint32_t *)0x4003A00C) // Channel 0 Status And Control
#define TPM2_C0V		(*(volatile uint32_t *)0x4003A010) // Channel 0 Value
#define TPM2_C1SC		(*(volatile uint32_t *)0x4003A014) // Channel 1 Status And Control
#define TPM2_C1V		(*(volatile uint32_t *)0x4003A018) // Channel 1 Value
#define TPM2_STATUS		(*(volatile uint32_t *)0x4003A050) // Capture And Compare Status
#define TPM2_CONF		(*(volatile uint32_t *)0x4003A084) // Configuration
#elif defined(KINETISK)
#define TPM1_SC			(*(volatile uint32_t *)0x400C9000) // Status And Control
#define TPM1_CNT		(*(volatile uint32_t *)0x400C9004) // Counter
#define TPM1_MOD		(*(volatile uint32_t *)0x400C9008) // Modulo
#define TPM1_C0SC		(*(volatile uint32_t *)0x400C900C) // Channel 0 Status And Control
#define TPM1_C0V		(*(volatile uint32_t *)0x400C9010) // Channel 0 Value
#define TPM1_C1SC		(*(volatile uint32_t *)0x400C9014) // Channel 1 Status And Control
#define TPM1_C1V		(*(volatile uint32_t *)0x400C9018) // Channel 1 Value
#define TPM1_STATUS		(*(volatile uint32_t *)0x400C9050) // Capture And Compare Status
#define TPM1_COMBINE		(*(volatile uint32_t *)0x400C9064) // Function For Linked Channels
#define TPM1_POL		(*(volatile uint32_t *)0x400C9070) // Channels Polarity
#define TPM1_FILTER		(*(volatile uint32_t *)0x400C9078) // Input Capture Filter Control
#define TPM1_QDCTRL		(*(volatile uint32_t *)0x400C9080) // Quadrature Decoder Control And Status
#define TPM1_CONF		(*(volatile uint32_t *)0x400C9084) // Configuration
#define TPM2_SC			(*(volatile uint32_t *)0x400CA000) // Status And Control
#define TPM2_CNT		(*(volatile uint32_t *)0x400CA004) // Counter
#define TPM2_MOD		(*(volatile uint32_t *)0x400CA008) // Modulo
#define TPM2_C0SC		(*(volatile uint32_t *)0x400CA00C) // Channel 0 Status And Control
#define TPM2_C0V		(*(volatile uint32_t *)0x400CA010) // Channel 0 Value
#define TPM2_C1SC		(*(volatile uint32_t *)0x400CA014) // Channel 1 Status And Control
#define TPM2_C1V		(*(volatile uint32_t *)0x400CA018) // Channel 1 Value
#define TPM2_STATUS		(*(volatile uint32_t *)0x400CA050) // Capture And Compare Status
#define TPM2_COMBINE		(*(volatile uint32_t *)0x400CA064) // Function For Linked Channels
#define TPM2_POL		(*(volatile uint32_t *)0x400CA070) // Channels Polarity
#define TPM2_FILTER		(*(volatile uint32_t *)0x400CA078) // Input Capture Filter Control
#define TPM2_QDCTRL		(*(volatile uint32_t *)0x400CA080) // Quadrature Decoder Control And Status
#define TPM2_CONF		(*(volatile uint32_t *)0x400CA084) // Configuration
#endif


// FlexTimer Module (FTM)

#define FTM0_SC			(*(volatile uint32_t *)0x40038000) // Status And Control
#ifdef KINETISL
#define FTM_SC_DMA			0x100				// DMA Enable
#endif
#define FTM_SC_TOF			0x80				// Timer Overflow Flag
#define FTM_SC_TOIE			0x40				// Timer Overflow Interrupt Enable
#define FTM_SC_CPWMS			0x20				// Center-Aligned PWM Select
#define FTM_SC_CLKS(n)			(((n) & 3) << 3)		// Clock Source Selection
#define FTM_SC_CLKS_MASK		0x18
#define FTM_SC_PS(n)			(((n) & 7) << 0)		// Prescale Factor Selection
#define FTM_SC_PS_MASK			0x07
#define FTM0_CNT		(*(volatile uint32_t *)0x40038004) // Counter
#define FTM0_MOD		(*(volatile uint32_t *)0x40038008) // Modulo
#define FTM0_C0SC		(*(volatile uint32_t *)0x4003800C) // Channel 0 Status And Control
#define FTM_CSC_CHF			0x80				// Channel Flag
#define FTM_CSC_CHIE			0x40				// Channel Interrupt Enable
#define FTM_CSC_MSB			0x20				// Channel Mode Select
#define FTM_CSC_MSA			0x10				// Channel Mode Select
#define FTM_CSC_ELSB			0x08				// Edge or Level Select
#define FTM_CSC_ELSA			0x04				// Edge or Level Select
#define FTM_CSC_DMA			0x01				// DMA Enable
#define FTM0_C0V		(*(volatile uint32_t *)0x40038010) // Channel 0 Value
#define FTM0_C1SC		(*(volatile uint32_t *)0x40038014) // Channel 1 Status And Control
#define FTM0_C1V		(*(volatile uint32_t *)0x40038018) // Channel 1 Value
#define FTM0_C2SC		(*(volatile uint32_t *)0x4003801C) // Channel 2 Status And Control
#define FTM0_C2V		(*(volatile uint32_t *)0x40038020) // Channel 2 Value
#define FTM0_C3SC		(*(volatile uint32_t *)0x40038024) // Channel 3 Status And Control
#define FTM0_C3V		(*(volatile uint32_t *)0x40038028) // Channel 3 Value
#define FTM0_C4SC		(*(volatile uint32_t *)0x4003802C) // Channel 4 Status And Control
#define FTM0_C4V		(*(volatile uint32_t *)0x40038030) // Channel 4 Value
#define FTM0_C5SC		(*(volatile uint32_t *)0x40038034) // Channel 5 Status And Control
#define FTM0_C5V		(*(volatile uint32_t *)0x40038038) // Channel 5 Value
#define FTM0_C6SC		(*(volatile uint32_t *)0x4003803C) // Channel 6 Status And Control
#define FTM0_C6V		(*(volatile uint32_t *)0x40038040) // Channel 6 Value
#define FTM0_C7SC		(*(volatile uint32_t *)0x40038044) // Channel 7 Status And Control
#define FTM0_C7V		(*(volatile uint32_t *)0x40038048) // Channel 7 Value
#define FTM0_CNTIN		(*(volatile uint32_t *)0x4003804C) // Counter Initial Value
#define FTM0_STATUS		(*(volatile uint32_t *)0x40038050) // Capture And Compare Status
#define FTM_STATUS_CH7F			0x80				//
#define FTM_STATUS_CH6F			0x40				//
#define FTM_STATUS_CH5F			0x20				//
#define FTM_STATUS_CH4F			0x10				//
#define FTM_STATUS_CH3F			0x08				//
#define FTM_STATUS_CH2F			0x04				//
#define FTM_STATUS_CH1F			0x02				//
#define FTM_STATUS_CH0F			0x01				//
#define FTM0_MODE		(*(volatile uint32_t *)0x40038054) // Features Mode Selection
#define FTM_MODE_FAULTIE		0x80				// Fault Interrupt Enable
#define FTM_MODE_FAULTM(n)		(((n) & 3) << 5)		// Fault Control Mode
#define FTM_MODE_FAULTM_MASK		0x60
#define FTM_MODE_CAPTEST		0x10				// Capture Test Mode Enable
#define FTM_MODE_PWMSYNC		0x08				// PWM Synchronization Mode
#define FTM_MODE_WPDIS			0x04				// Write Protection Disable
#define FTM_MODE_INIT			0x02				// Initialize The Channels Output
#define FTM_MODE_FTMEN			0x01				// FTM Enable
#define FTM0_SYNC		(*(volatile uint32_t *)0x40038058) // Synchronization
#define FTM_SYNC_SWSYNC			0x80				//
#define FTM_SYNC_TRIG2			0x40				//
#define FTM_SYNC_TRIG1			0x20				//
#define FTM_SYNC_TRIG0			0x10				//
#define FTM_SYNC_SYNCHOM		0x08				//
#define FTM_SYNC_REINIT			0x04				//
#define FTM_SYNC_CNTMAX			0x02				//
#define FTM_SYNC_CNTMIN			0x01				//
#define FTM0_OUTINIT		(*(volatile uint32_t *)0x4003805C) // Initial State For Channels Output
#define FTM_OUTINIT_CH7OI		0x80				//
#define FTM_OUTINIT_CH6OI		0x40				//
#define FTM_OUTINIT_CH5OI		0x20				//
#define FTM_OUTINIT_CH4OI		0x10				//
#define FTM_OUTINIT_CH3OI		0x08				//
#define FTM_OUTINIT_CH2OI		0x04				//
#define FTM_OUTINIT_CH1OI		0x02				//
#define FTM_OUTINIT_CH0OI		0x01				//
#define FTM0_OUTMASK		(*(volatile uint32_t *)0x40038060) // Output Mask
#define FTM_OUTMASK_CH7OM		0x80				//
#define FTM_OUTMASK_CH6OM		0x40				//
#define FTM_OUTMASK_CH5OM		0x20				//
#define FTM_OUTMASK_CH4OM		0x10				//
#define FTM_OUTMASK_CH3OM		0x08				//
#define FTM_OUTMASK_CH2OM		0x04				//
#define FTM_OUTMASK_CH1OM		0x02				//
#define FTM_OUTMASK_CH0OM		0x01				//
#define FTM0_COMBINE		(*(volatile uint32_t *)0x40038064) // Function For Linked Channels
#define FTM_COMBINE_FAULTEN3		0x40000000			// Enable the fault control, ch #6 & #7
#define FTM_COMBINE_SYNCEN3		0x20000000			// Enable PWM sync of C6V & C7V
#define FTM_COMBINE_DTEN3		0x10000000			// Enable deadtime insertion, ch #6 & #7
#define FTM_COMBINE_DECAP3		0x08000000			// Dual Edge Capture Mode
#define FTM_COMBINE_DECAPEN3		0x04000000			// Dual Edge Capture Mode Enable
#define FTM_COMBINE_COMP3		0x02000000			// Complement Of Channel #6 & #7
#define FTM_COMBINE_COMBINE3		0x01000000			// Combine Channels #6 & #7
#define FTM_COMBINE_FAULTEN2		0x00400000			// Enable the fault control, ch #4 & #5
#define FTM_COMBINE_SYNCEN2		0x00200000			// Enable PWM sync of C4V & C5V
#define FTM_COMBINE_DTEN2		0x00100000			// Enable deadtime insertion, ch #4 & #5
#define FTM_COMBINE_DECAP2		0x00080000			// Dual Edge Capture Mode
#define FTM_COMBINE_DECAPEN2		0x00040000			// Dual Edge Capture Mode Enable
#define FTM_COMBINE_COMP2		0x00020000			// Complement Of Channel #4 & #5
#define FTM_COMBINE_COMBINE2		0x00010000			// Combine Channels #4 & #5
#define FTM_COMBINE_FAULTEN1		0x00004000			// Enable the fault control, ch #2 & #3
#define FTM_COMBINE_SYNCEN1		0x00002000			// Enable PWM sync of C2V & C3V
#define FTM_COMBINE_DTEN1		0x00001000			// Enable deadtime insertion, ch #2 & #3
#define FTM_COMBINE_DECAP1		0x00000800			// Dual Edge Capture Mode
#define FTM_COMBINE_DECAPEN1		0x00000400			// Dual Edge Capture Mode Enable
#define FTM_COMBINE_COMP1		0x00000200			// Complement Of Channel #2 & #3
#define FTM_COMBINE_COMBINE1		0x00000100			// Combine Channels #2 & #3
#define FTM_COMBINE_FAULTEN0		0x00000040			// Enable the fault control, ch #0 & #1
#define FTM_COMBINE_SYNCEN0		0x00000020			// Enable PWM sync of C0V & C1V
#define FTM_COMBINE_DTEN0		0x00000010			// Enable deadtime insertion, ch #0 & #1
#define FTM_COMBINE_DECAP0		0x00000008			// Dual Edge Capture Mode
#define FTM_COMBINE_DECAPEN0		0x00000004			// Dual Edge Capture Mode Enable
#define FTM_COMBINE_COMP0		0x00000002			// Complement Of Channel #0 & #1
#define FTM_COMBINE_COMBINE0		0x00000001			// Combine Channels #0 & #1
#define FTM0_DEADTIME		(*(volatile uint32_t *)0x40038068) // Deadtime Insertion Control
#define FTM_DEADTIME_DTPS(n)		(((n) & 3) << 6)		// Prescaler Value, 0=1x, 2=4x, 3=16x
#define FTM_DEADTIME_DTPS_MASK		0xC0
#define FTM_DEADTIME_DTVAL(n)		(((n) & 63) << 0)		// Deadtime Value
#define FTM_DEADTIME_DTVAL_MASK		0x3F
#define FTM0_EXTTRIG		(*(volatile uint32_t *)0x4003806C) // FTM External Trigger
#define FTM_EXTTRIG_TRIGF		0x80				// Channel Trigger Flag
#define FTM_EXTTRIG_INITTRIGEN		0x40				// Initialization Trigger Enable
#define FTM_EXTTRIG_CH1TRIG		0x20				// Channel 1 Trigger Enable
#define FTM_EXTTRIG_CH0TRIG		0x10				// Channel 0 Trigger Enable
#define FTM_EXTTRIG_CH5TRIG		0x08				// Channel 5 Trigger Enable
#define FTM_EXTTRIG_CH4TRIG		0x04				// Channel 4 Trigger Enable
#define FTM_EXTTRIG_CH3TRIG		0x02				// Channel 3 Trigger Enable
#define FTM_EXTTRIG_CH2TRIG		0x01				// Channel 2 Trigger Enable
#define FTM0_POL		(*(volatile uint32_t *)0x40038070) // Channels Polarity
#define FTM_POL_POL7			0x80				// Channel 7 Polarity, 0=active high, 1=active low
#define FTM_POL_POL6			0x40				// Channel 6 Polarity, 0=active high, 1=active low
#define FTM_POL_POL5			0x20				// Channel 5 Polarity, 0=active high, 1=active low
#define FTM_POL_POL4			0x10				// Channel 4 Polarity, 0=active high, 1=active low
#define FTM_POL_POL3			0x08				// Channel 3 Polarity, 0=active high, 1=active low
#define FTM_POL_POL2			0x04				// Channel 2 Polarity, 0=active high, 1=active low
#define FTM_POL_POL1			0x02				// Channel 1 Polarity, 0=active high, 1=active low
#define FTM_POL_POL0			0x01				// Channel 0 Polarity, 0=active high, 1=active low
#define FTM0_FMS		(*(volatile uint32_t *)0x40038074) // Fault Mode Status
#define FTM_FMS_FAULTF			0x80				// Fault Detection Flag
#define FTM_FMS_WPEN			0x40				// Write Protection Enable
#define FTM_FMS_FAULTIN			0x20				// Fault Inputs
#define FTM_FMS_FAULTF3			0x08				// Fault Detection Flag 3
#define FTM_FMS_FAULTF2			0x04				// Fault Detection Flag 2
#define FTM_FMS_FAULTF1			0x02				// Fault Detection Flag 1
#define FTM_FMS_FAULTF0			0x01				// Fault Detection Flag 0
#define FTM0_FILTER		(*(volatile uint32_t *)0x40038078) // Input Capture Filter Control
#define FTM_FILTER_CH3FVAL(n)		(((n) & 15) << 12)		// Channel 3 Input Filter
#define FTM_FILTER_CH2FVAL(n)		(((n) & 15) << 8)		// Channel 2 Input Filter
#define FTM_FILTER_CH1FVAL(n)		(((n) & 15) << 4)		// Channel 1 Input Filter
#define FTM_FILTER_CH0FVAL(n)		(((n) & 15) << 0)		// Channel 0 Input Filter
#define FTM_FILTER_CH3FVAL_MASK		0xF000
#define FTM_FILTER_CH2FVAL_MASK		0x0F00
#define FTM_FILTER_CH1FVAL_MASK		0x00F0
#define FTM_FILTER_CH0FVAL_MASK		0x000F
#define FTM0_FLTCTRL		(*(volatile uint32_t *)0x4003807C) // Fault Control
#define FTM_FLTCTRL_FFVAL(n)		(((n) & 15) << 8)		// Fault Input Filter Value, 0=disable
#define FTM_FLTCTRL_FFVAL_MASK		0xF00
#define FTM_FLTCTRL_FFLTR3EN		0x80				// Fault Input 3 Filter Enable
#define FTM_FLTCTRL_FFLTR2EN		0x40				// Fault Input 2 Filter Enable
#define FTM_FLTCTRL_FFLTR1EN		0x20				// Fault Input 1 Filter Enable
#define FTM_FLTCTRL_FFLTR0EN		0x10				// Fault Input 0 Filter Enable
#define FTM_FLTCTRL_FAULT3EN		0x08				// Fault Input 3 Enable
#define FTM_FLTCTRL_FAULT2EN		0x04				// Fault Input 2 Enable
#define FTM_FLTCTRL_FAULT1EN		0x02				// Fault Input 1 Enable
#define FTM_FLTCTRL_FAULT0EN		0x01				// Fault Input 0 Enable
#define FTM0_QDCTRL		(*(volatile uint32_t *)0x40038080) // Quadrature Decoder Control And Status
#define FTM_QDCTRL_PHAFLTREN		0x80				// Phase A Input Filter Enable
#define FTM_QDCTRL_PHBFLTREN		0x40				// Phase B Input Filter Enable
#define FTM_QDCTRL_PHAPOL		0x20				// Phase A Input Polarity
#define FTM_QDCTRL_PHBPOL		0x10				// Phase B Input Polarity
#define FTM_QDCTRL_QUADMODE		0x08				// Quadrature Decoder Mode
#define FTM_QDCTRL_QUADIR		0x04				// FTM Counter Direction In Quadrature Decoder Mode
#define FTM_QDCTRL_TOFDIR		0x02				// Timer Overflow Direction In Quadrature Decoder Mode
#define FTM_QDCTRL_QUADEN		0x01				// Quadrature Decoder Mode Enable
#define FTM0_CONF		(*(volatile uint32_t *)0x40038084) // Configuration
#define FTM_CONF_GTBEOUT		0x400				// Global Time Base Output
#define FTM_CONF_GTBEEN			0x200				// Global Time Base Enable
#define FTM_CONF_BDMMODE		(((n) & 3) << 6)		// Behavior when in debug mode
#define FTM_CONF_NUMTOF			(((n) & 31) << 0)		// ratio of counter overflows to TOF bit set
#define FTM0_FLTPOL		(*(volatile uint32_t *)0x40038088) // FTM Fault Input Polarity
#define FTM_FLTPOL_FLT3POL		0x08				// Fault Input 3 Polarity
#define FTM_FLTPOL_FLT2POL		0x04				// Fault Input 2 Polarity
#define FTM_FLTPOL_FLT1POL		0x02				// Fault Input 1 Polarity
#define FTM_FLTPOL_FLT0POL		0x01				// Fault Input 0 Polarity
#define FTM0_SYNCONF		(*(volatile uint32_t *)0x4003808C) // Synchronization Configuration
#define FTM_SYNCONF_HWSOC		0x100000			// Software output control synchronization is activated by a hardware trigger.
#define FTM_SYNCONF_HWINVC		0x080000			// Inverting control synchronization is activated by a hardware trigger.
#define FTM_SYNCONF_HWOM		0x040000			// Output mask synchronization is activated by a hardware trigger.
#define FTM_SYNCONF_HWWRBUF		0x020000			// MOD, CNTIN, and CV registers synchronization is activated by a hardware trigger.
#define FTM_SYNCONF_HWRSTCNT		0x010000			// FTM counter synchronization is activated by a hardware trigger.
#define FTM_SYNCONF_SWSOC		0x001000			// Software output control synchronization is activated by the software trigger.
#define FTM_SYNCONF_SWINVC		0x000800			// Inverting control synchronization is activated by the software trigger.
#define FTM_SYNCONF_SWOM		0x000400			// Output mask synchronization is activated by the software trigger.
#define FTM_SYNCONF_SWWRBUF		0x000200			// MOD, CNTIN, and CV registers synchronization is activated by the software trigger.
#define FTM_SYNCONF_SWRSTCNT		0x000100			// FTM counter synchronization is activated by the software trigger.
#define FTM_SYNCONF_SYNCMODE		0x000080			// Synchronization Mode, 0=Legacy, 1=Enhanced PWM
#define FTM_SYNCONF_SWOC		0x000020			// SWOCTRL Register Synchronization
#define FTM_SYNCONF_INVC		0x000010			// INVCTRL Register Synchronization
#define FTM_SYNCONF_CNTINC		0x000004			// CNTIN Register Synchronization
#define FTM_SYNCONF_HWTRIGMODE		0x000001			// Hardware Trigger Mode
#define FTM0_INVCTRL		(*(volatile uint32_t *)0x40038090) // FTM Inverting Control
#define FTM_INVCTRL_INV3EN		0x08				// Pair Channels 3 Inverting Enable
#define FTM_INVCTRL_INV2EN		0x04				// Pair Channels 2 Inverting Enable
#define FTM_INVCTRL_INV1EN		0x02				// Pair Channels 1 Inverting Enable
#define FTM_INVCTRL_INV0EN		0x01				// Pair Channels 0 Inverting Enable
#define FTM0_SWOCTRL		(*(volatile uint32_t *)0x40038094) // FTM Software Output Control
#define FTM_SWOCTRL_CH7OCV		0x8000				// Channel 7 Software Output Control Value
#define FTM_SWOCTRL_CH6OCV		0x4000				// Channel 6 Software Output Control Value
#define FTM_SWOCTRL_CH5OCV		0x2000				// Channel 5 Software Output Control Value
#define FTM_SWOCTRL_CH4OCV		0x1000				// Channel 4 Software Output Control Value
#define FTM_SWOCTRL_CH3OCV		0x0800				// Channel 3 Software Output Control Value
#define FTM_SWOCTRL_CH2OCV		0x0400				// Channel 2 Software Output Control Value
#define FTM_SWOCTRL_CH1OCV		0x0200				// Channel 1 Software Output Control Value
#define FTM_SWOCTRL_CH0OCV		0x0100				// Channel 0 Software Output Control Value
#define FTM_SWOCTRL_CH7OC		0x0080				// Channel 7 Software Output Control Enable
#define FTM_SWOCTRL_CH6OC		0x0040				// Channel 6 Software Output Control Enable
#define FTM_SWOCTRL_CH5OC		0x0020				// Channel 5 Software Output Control Enable
#define FTM_SWOCTRL_CH4OC		0x0010				// Channel 4 Software Output Control Enable
#define FTM_SWOCTRL_CH3OC		0x0008				// Channel 3 Software Output Control Enable
#define FTM_SWOCTRL_CH2OC		0x0004				// Channel 2 Software Output Control Enable
#define FTM_SWOCTRL_CH1OC		0x0002				// Channel 1 Software Output Control Enable
#define FTM_SWOCTRL_CH0OC		0x0001				// Channel 0 Software Output Control Enable
#define FTM0_PWMLOAD		(*(volatile uint32_t *)0x40038098) // FTM PWM Load
#define FTM_PWMLOAD_LDOK		0x200				// Enables the loading of the MOD, CNTIN, and CV registers with the values of their write buffers
#define FTM_PWMLOAD_CH7SEL		0x80				// Channel 7 Select
#define FTM_PWMLOAD_CH6SEL		0x40				// Channel 6 Select
#define FTM_PWMLOAD_CH5SEL		0x20				// Channel 5 Select
#define FTM_PWMLOAD_CH4SEL		0x10				// Channel 4 Select
#define FTM_PWMLOAD_CH3SEL		0x08				// Channel 4 Select
#define FTM_PWMLOAD_CH2SEL		0x04				// Channel 3 Select
#define FTM_PWMLOAD_CH1SEL		0x02				// Channel 2 Select
#define FTM_PWMLOAD_CH0SEL		0x01				// Channel 1 Select
#define FTM1_SC			(*(volatile uint32_t *)0x40039000) // Status And Control
#define FTM1_CNT		(*(volatile uint32_t *)0x40039004) // Counter
#define FTM1_MOD		(*(volatile uint32_t *)0x40039008) // Modulo
#define FTM1_C0SC		(*(volatile uint32_t *)0x4003900C) // Channel 0 Status And Control
#define FTM1_C0V		(*(volatile uint32_t *)0x40039010) // Channel 0 Value
#define FTM1_C1SC		(*(volatile uint32_t *)0x40039014) // Channel 1 Status And Control
#define FTM1_C1V		(*(volatile uint32_t *)0x40039018) // Channel 1 Value
#define FTM1_CNTIN		(*(volatile uint32_t *)0x4003904C) // Counter Initial Value
#define FTM1_STATUS		(*(volatile uint32_t *)0x40039050) // Capture And Compare Status
#define FTM1_MODE		(*(volatile uint32_t *)0x40039054) // Features Mode Selection
#define FTM1_SYNC		(*(volatile uint32_t *)0x40039058) // Synchronization
#define FTM1_OUTINIT		(*(volatile uint32_t *)0x4003905C) // Initial State For Channels Output
#define FTM1_OUTMASK		(*(volatile uint32_t *)0x40039060) // Output Mask
#define FTM1_COMBINE		(*(volatile uint32_t *)0x40039064) // Function For Linked Channels
#define FTM1_DEADTIME		(*(volatile uint32_t *)0x40039068) // Deadtime Insertion Control
#define FTM1_EXTTRIG		(*(volatile uint32_t *)0x4003906C) // FTM External Trigger
#define FTM1_POL		(*(volatile uint32_t *)0x40039070) // Channels Polarity
#define FTM1_FMS		(*(volatile uint32_t *)0x40039074) // Fault Mode Status
#define FTM1_FILTER		(*(volatile uint32_t *)0x40039078) // Input Capture Filter Control
#define FTM1_FLTCTRL		(*(volatile uint32_t *)0x4003907C) // Fault Control
#define FTM1_QDCTRL		(*(volatile uint32_t *)0x40039080) // Quadrature Decoder Control And Status
#define FTM1_CONF		(*(volatile uint32_t *)0x40039084) // Configuration
#define FTM1_FLTPOL		(*(volatile uint32_t *)0x40039088) // FTM Fault Input Polarity
#define FTM1_SYNCONF		(*(volatile uint32_t *)0x4003908C) // Synchronization Configuration
#define FTM1_INVCTRL		(*(volatile uint32_t *)0x40039090) // FTM Inverting Control
#define FTM1_SWOCTRL		(*(volatile uint32_t *)0x40039094) // FTM Software Output Control
#define FTM1_PWMLOAD		(*(volatile uint32_t *)0x40039098) // FTM PWM Load

#if defined(KINETISK)
#define FTM2_SC			(*(volatile uint32_t *)0x400B8000) // Status And Control
#define FTM2_CNT		(*(volatile uint32_t *)0x400B8004) // Counter
#define FTM2_MOD		(*(volatile uint32_t *)0x400B8008) // Modulo
#define FTM2_C0SC		(*(volatile uint32_t *)0x400B800C) // Channel 0 Status And Control
#define FTM2_C0V		(*(volatile uint32_t *)0x400B8010) // Channel 0 Value
#define FTM2_C1SC		(*(volatile uint32_t *)0x400B8014) // Channel 1 Status And Control
#define FTM2_C1V		(*(volatile uint32_t *)0x400B8018) // Channel 1 Value
#define FTM2_CNTIN		(*(volatile uint32_t *)0x400B804C) // Counter Initial Value
#define FTM2_STATUS		(*(volatile uint32_t *)0x400B8050) // Capture And Compare Status
#define FTM2_MODE		(*(volatile uint32_t *)0x400B8054) // Features Mode Selection
#define FTM2_SYNC		(*(volatile uint32_t *)0x400B8058) // Synchronization
#define FTM2_OUTINIT		(*(volatile uint32_t *)0x400B805C) // Initial State For Channels Output
#define FTM2_OUTMASK		(*(volatile uint32_t *)0x400B8060) // Output Mask
#define FTM2_COMBINE		(*(volatile uint32_t *)0x400B8064) // Function For Linked Channels
#define FTM2_DEADTIME		(*(volatile uint32_t *)0x400B8068) // Deadtime Insertion Control
#define FTM2_EXTTRIG		(*(volatile uint32_t *)0x400B806C) // FTM External Trigger
#define FTM2_POL		(*(volatile uint32_t *)0x400B8070) // Channels Polarity
#define FTM2_FMS		(*(volatile uint32_t *)0x400B8074) // Fault Mode Status
#define FTM2_FILTER		(*(volatile uint32_t *)0x400B8078) // Input Capture Filter Control
#define FTM2_FLTCTRL		(*(volatile uint32_t *)0x400B807C) // Fault Control
#define FTM2_QDCTRL		(*(volatile uint32_t *)0x400B8080) // Quadrature Decoder Control And Status
#define FTM2_CONF		(*(volatile uint32_t *)0x400B8084) // Configuration
#define FTM2_FLTPOL		(*(volatile uint32_t *)0x400B8088) // FTM Fault Input Polarity
#define FTM2_SYNCONF		(*(volatile uint32_t *)0x400B808C) // Synchronization Configuration
#define FTM2_INVCTRL		(*(volatile uint32_t *)0x400B8090) // FTM Inverting Control
#define FTM2_SWOCTRL		(*(volatile uint32_t *)0x400B8094) // FTM Software Output Control
#define FTM2_PWMLOAD		(*(volatile uint32_t *)0x400B8098) // FTM PWM Load
#define FTM3_SC			(*(volatile uint32_t *)0x400B9000) // Status And Control
#define FTM3_CNT		(*(volatile uint32_t *)0x400B9004) // Counter
#define FTM3_MOD		(*(volatile uint32_t *)0x400B9008) // Modulo
#define FTM3_C0SC		(*(volatile uint32_t *)0x400B900C) // Channel 0 Status And Control
#define FTM3_C0V		(*(volatile uint32_t *)0x400B9010) // Channel 0 Value
#define FTM3_C1SC		(*(volatile uint32_t *)0x400B9014) // Channel 1 Status And Control
#define FTM3_C1V		(*(volatile uint32_t *)0x400B9018) // Channel 1 Value
#define FTM3_C2SC		(*(volatile uint32_t *)0x400B901C) // Channel 1 Status And Control
#define FTM3_C2V		(*(volatile uint32_t *)0x400B9020) // Channel 1 Value
#define FTM3_C3SC		(*(volatile uint32_t *)0x400B9024) // Channel 1 Status And Control
#define FTM3_C3V		(*(volatile uint32_t *)0x400B9028) // Channel 1 Value
#define FTM3_C4SC		(*(volatile uint32_t *)0x400B902C) // Channel 1 Status And Control
#define FTM3_C4V		(*(volatile uint32_t *)0x400B9030) // Channel 1 Value
#define FTM3_C5SC		(*(volatile uint32_t *)0x400B9034) // Channel 1 Status And Control
#define FTM3_C5V		(*(volatile uint32_t *)0x400B9038) // Channel 1 Value
#define FTM3_C6SC		(*(volatile uint32_t *)0x400B903C) // Channel 1 Status And Control
#define FTM3_C6V		(*(volatile uint32_t *)0x400B9040) // Channel 1 Value
#define FTM3_C7SC		(*(volatile uint32_t *)0x400B9044) // Channel 1 Status And Control
#define FTM3_C7V		(*(volatile uint32_t *)0x400B9048) // Channel 1 Value
#define FTM3_CNTIN		(*(volatile uint32_t *)0x400B904C) // Counter Initial Value
#define FTM3_STATUS		(*(volatile uint32_t *)0x400B9050) // Capture And Compare Status
#define FTM3_MODE		(*(volatile uint32_t *)0x400B9054) // Features Mode Selection
#define FTM3_SYNC		(*(volatile uint32_t *)0x400B9058) // Synchronization
#define FTM3_OUTINIT		(*(volatile uint32_t *)0x400B905C) // Initial State For Channels Output
#define FTM3_OUTMASK		(*(volatile uint32_t *)0x400B9060) // Output Mask
#define FTM3_COMBINE		(*(volatile uint32_t *)0x400B9064) // Function For Linked Channels
#define FTM3_DEADTIME		(*(volatile uint32_t *)0x400B9068) // Deadtime Insertion Control
#define FTM3_EXTTRIG		(*(volatile uint32_t *)0x400B906C) // FTM External Trigger
#define FTM3_POL		(*(volatile uint32_t *)0x400B9070) // Channels Polarity
#define FTM3_FMS		(*(volatile uint32_t *)0x400B9074) // Fault Mode Status
#define FTM3_FILTER		(*(volatile uint32_t *)0x400B9078) // Input Capture Filter Control
#define FTM3_FLTCTRL		(*(volatile uint32_t *)0x400B907C) // Fault Control
#define FTM3_QDCTRL		(*(volatile uint32_t *)0x400B9080) // Quadrature Decoder Control And Status
#define FTM3_CONF		(*(volatile uint32_t *)0x400B9084) // Configuration
#define FTM3_FLTPOL		(*(volatile uint32_t *)0x400B9088) // FTM Fault Input Polarity
#define FTM3_SYNCONF		(*(volatile uint32_t *)0x400B908C) // Synchronization Configuration
#define FTM3_INVCTRL		(*(volatile uint32_t *)0x400B9090) // FTM Inverting Control
#define FTM3_SWOCTRL		(*(volatile uint32_t *)0x400B9094) // FTM Software Output Control
#define FTM3_PWMLOAD		(*(volatile uint32_t *)0x400B9098) // FTM PWM Load
#elif defined(KINETISL)
#define FTM2_SC			(*(volatile uint32_t *)0x4003A000) // Status And Control
#define FTM2_CNT		(*(volatile uint32_t *)0x4003A004) // Counter
#define FTM2_MOD		(*(volatile uint32_t *)0x4003A008) // Modulo
#define FTM2_C0SC		(*(volatile uint32_t *)0x4003A00C) // Channel 0 Status And Control
#define FTM2_C0V		(*(volatile uint32_t *)0x4003A010) // Channel 0 Value
#define FTM2_C1SC		(*(volatile uint32_t *)0x4003A014) // Channel 1 Status And Control
#define FTM2_C1V		(*(volatile uint32_t *)0x4003A018) // Channel 1 Value
#define FTM2_STATUS		(*(volatile uint32_t *)0x4003A050) // Capture And Compare Status
#define FTM2_CONF		(*(volatile uint32_t *)0x4003A084) // Configuration
#endif


// Periodic Interrupt Timer (PIT)

#define PIT_MCR			(*(volatile uint32_t *)0x40037000) // PIT Module Control Register
#define PIT_MCR_MDIS            (1<<1)                               // Module disable
#define PIT_MCR_FRZ             (1<<0)                               // Freeze
#if defined(KINETISL)
#define PIT_LTMR64H             (*(volatile uint32_t *)0x400370E0) // PIT Upper Lifetime Timer Register
#define PIT_LTMR64L             (*(volatile uint32_t *)0x400370E4) // PIT Lower Lifetime Timer Register
#endif // defined(KINETISL)
#define PIT_LDVAL0		(*(volatile uint32_t *)0x40037100) // Timer Load Value Register
#define PIT_CVAL0		(*(volatile uint32_t *)0x40037104) // Current Timer Value Register
#define PIT_TCTRL0		(*(volatile uint32_t *)0x40037108) // Timer Control Register
#define PIT_TCTRL_CHN           (1<<2)                               // Chain Mode
#define PIT_TCTRL_TIE           (1<<1)                               // Timer Interrupt Enable
#define PIT_TCTRL_TEN           (1<<0)                               // Timer Enable
#define PIT_TFLG0		(*(volatile uint32_t *)0x4003710C) // Timer Flag Register
#define PIT_TFLG_TIF            (1<<0)                               // Timer Interrupt Flag (write 1 to clear)
#define PIT_LDVAL1		(*(volatile uint32_t *)0x40037110) // Timer Load Value Register
#define PIT_CVAL1		(*(volatile uint32_t *)0x40037114) // Current Timer Value Register
#define PIT_TCTRL1		(*(volatile uint32_t *)0x40037118) // Timer Control Register
#define PIT_TFLG1		(*(volatile uint32_t *)0x4003711C) // Timer Flag Register
#if defined(KINETISK) // the 3.1 has 4 PITs, LC has only 2
#define PIT_LDVAL2		(*(volatile uint32_t *)0x40037120) // Timer Load Value Register
#define PIT_CVAL2		(*(volatile uint32_t *)0x40037124) // Current Timer Value Register
#define PIT_TCTRL2		(*(volatile uint32_t *)0x40037128) // Timer Control Register
#define PIT_TFLG2		(*(volatile uint32_t *)0x4003712C) // Timer Flag Register
#define PIT_LDVAL3		(*(volatile uint32_t *)0x40037130) // Timer Load Value Register
#define PIT_CVAL3		(*(volatile uint32_t *)0x40037134) // Current Timer Value Register
#define PIT_TCTRL3		(*(volatile uint32_t *)0x40037138) // Timer Control Register
#define PIT_TFLG3		(*(volatile uint32_t *)0x4003713C) // Timer Flag Register
#endif // defined(KINETISK)

// Low-Power Timer (LPTMR)

#define LPTMR0_CSR		(*(volatile uint32_t *)0x40040000) // Low Power Timer Control Status Register
#define LPTMR_CSR_TCF			0x80				// Compare Flag
#define LPTMR_CSR_TIE			0x40				// Interrupt Enable
#define LPTMR_CSR_TPS(n)		(((n) & 3) << 4)		// Pin: 0=CMP0, 1=xtal, 2=pin13
#define LPTMR_CSR_TPP			0x08				// Pin Polarity
#define LPTMR_CSR_TFC			0x04				// Free-Running Counter
#define LPTMR_CSR_TMS			0x02				// Mode Select, 0=timer, 1=counter
#define LPTMR_CSR_TEN			0x01				// Enable
#define LPTMR0_PSR		(*(volatile uint32_t *)0x40040004) // Low Power Timer Prescale Register
#define LPTMR_PSR_PRESCALE(n)		(((n) & 15) << 3)		// Prescaler value
#define LPTMR_PSR_PBYP			0x04				// Prescaler bypass
#define LPTMR_PSR_PCS(n)		(((n) & 3) << 0)		// Clock: 0=MCGIRCLK, 1=LPO(1kHz), 2=ERCLK32K, 3=OSCERCLK
#define LPTMR0_CMR		(*(volatile uint32_t *)0x40040008) // Low Power Timer Compare Register
#define LPTMR0_CNR		(*(volatile uint32_t *)0x4004000C) // Low Power Timer Counter Register

// Carrier Modulator Transmitter (CMT)

#define CMT_CGH1		(*(volatile uint8_t  *)0x40062000) // CMT Carrier Generator High Data Register 1
#define CMT_CGL1		(*(volatile uint8_t  *)0x40062001) // CMT Carrier Generator Low Data Register 1
#define CMT_CGH2		(*(volatile uint8_t  *)0x40062002) // CMT Carrier Generator High Data Register 2
#define CMT_CGL2		(*(volatile uint8_t  *)0x40062003) // CMT Carrier Generator Low Data Register 2
#define CMT_OC			(*(volatile uint8_t  *)0x40062004) // CMT Output Control Register
#define CMT_MSC			(*(volatile uint8_t  *)0x40062005) // CMT Modulator Status and Control Register
#define CMT_CMD1		(*(volatile uint8_t  *)0x40062006) // CMT Modulator Data Register Mark High
#define CMT_CMD2		(*(volatile uint8_t  *)0x40062007) // CMT Modulator Data Register Mark Low
#define CMT_CMD3		(*(volatile uint8_t  *)0x40062008) // CMT Modulator Data Register Space High
#define CMT_CMD4		(*(volatile uint8_t  *)0x40062009) // CMT Modulator Data Register Space Low
#define CMT_PPS			(*(volatile uint8_t  *)0x4006200A) // CMT Primary Prescaler Register
#define CMT_DMA			(*(volatile uint8_t  *)0x4006200B) // CMT Direct Memory Access Register

// Real Time Clock (RTC)

#define RTC_TSR			(*(volatile uint32_t *)0x4003D000) // RTC Time Seconds Register
#define RTC_TPR			(*(volatile uint32_t *)0x4003D004) // RTC Time Prescaler Register
#define RTC_TAR			(*(volatile uint32_t *)0x4003D008) // RTC Time Alarm Register
#define RTC_TCR			(*(volatile uint32_t *)0x4003D00C) // RTC Time Compensation Register
#define RTC_TCR_CIC(n)			(((n) & 255) << 24)		// Compensation Interval Counter
#define RTC_TCR_TCV(n)			(((n) & 255) << 16)		// Time Compensation Value
#define RTC_TCR_CIR(n)			(((n) & 255) << 8)		// Compensation Interval Register
#define RTC_TCR_TCR(n)			(((n) & 255) << 0)		// Time Compensation Register
#define RTC_CR			(*(volatile uint32_t *)0x4003D010) // RTC Control Register
#define RTC_CR_SC2P			((uint32_t)0x00002000)		//
#define RTC_CR_SC4P			((uint32_t)0x00001000)		//
#define RTC_CR_SC8P			((uint32_t)0x00000800)		//
#define RTC_CR_SC16P			((uint32_t)0x00000400)		//
#define RTC_CR_CLKO			((uint32_t)0x00000200)		//
#define RTC_CR_OSCE			((uint32_t)0x00000100)		//
#define RTC_CR_UM			((uint32_t)0x00000008)		//
#define RTC_CR_SUP			((uint32_t)0x00000004)		//
#define RTC_CR_WPE			((uint32_t)0x00000002)		//
#define RTC_CR_SWR			((uint32_t)0x00000001)		//
#define RTC_SR			(*(volatile uint32_t *)0x4003D014) // RTC Status Register
#define RTC_SR_TCE			((uint32_t)0x00000010)		//
#define RTC_SR_TAF			((uint32_t)0x00000004)		//
#define RTC_SR_TOF			((uint32_t)0x00000002)		//
#define RTC_SR_TIF			((uint32_t)0x00000001)		//
#define RTC_LR			(*(volatile uint32_t *)0x4003D018) // RTC Lock Register
#define RTC_IER			(*(volatile uint32_t *)0x4003D01C) // RTC Interrupt Enable Register
#define RTC_WAR			(*(volatile uint32_t *)0x4003D800) // RTC Write Access Register
#define RTC_RAR			(*(volatile uint32_t *)0x4003D804) // RTC Read Access Register


// 10/100-Mbps Ethernet MAC (ENET)

#define ENET_EIR		(*(volatile uint32_t *)0x400C0004) // Interrupt Event Register
#define ENET_EIR_BABR			((uint32_t)0x40000000)		// Babbling Receive Error
#define ENET_EIR_BABT			((uint32_t)0x20000000)		// Babbling Transmit Error
#define ENET_EIR_GRA			((uint32_t)0x10000000)		// Graceful Stop Complete
#define ENET_EIR_TXF			((uint32_t)0x08000000)		// Transmit Frame Interrupt
#define ENET_EIR_TXB			((uint32_t)0x04000000)		// Transmit Buffer Interrupt
#define ENET_EIR_RXF			((uint32_t)0x02000000)		// Receive Frame Interrupt
#define ENET_EIR_RXB			((uint32_t)0x01000000)		// Receive Buffer Interrupt
#define ENET_EIR_MII			((uint32_t)0x00800000)		// MII Interrupt
#define ENET_EIR_EBERR			((uint32_t)0x00400000)		// Ethernet Bus Error
#define ENET_EIR_LC			((uint32_t)0x00200000)		// Late Collision
#define ENET_EIR_RL			((uint32_t)0x00100000)		// Collision Retry Limit
#define ENET_EIR_UN			((uint32_t)0x00080000)		// Transmit FIFO Underrun
#define ENET_EIR_PLR			((uint32_t)0x00040000)		// Payload Receive Error
#define ENET_EIR_WAKEUP			((uint32_t)0x00020000)		// Node Wakeup Request Indication
#define ENET_EIR_TS_AVAIL		((uint32_t)0x00010000)		// Transmit Timestamp Available
#define ENET_EIR_TS_TIMER		((uint32_t)0x00008000)		// Timestamp Timer
#define ENET_EIMR		(*(volatile uint32_t *)0x400C0008) // Interrupt Mask Register
#define ENET_EIRM_BABR			((uint32_t)0x40000000)		// Babbling Receive Error Mask
#define ENET_EIRM_BABT			((uint32_t)0x20000000)		// Babbling Transmit Error Mask
#define ENET_EIRM_GRA			((uint32_t)0x10000000)		// Graceful Stop Complete Mask
#define ENET_EIRM_TXF			((uint32_t)0x08000000)		// Transmit Frame Interrupt Mask
#define ENET_EIRM_TXB			((uint32_t)0x04000000)		// Transmit Buffer Interrupt Mask
#define ENET_EIRM_RXF			((uint32_t)0x02000000)		// Receive Frame Interrupt Mask
#define ENET_EIRM_RXB			((uint32_t)0x01000000)		// Receive Buffer Interrupt Mask
#define ENET_EIRM_MII			((uint32_t)0x00800000)		// MII Interrupt Mask
#define ENET_EIRM_EBERR			((uint32_t)0x00400000)		// Ethernet Bus Error Mask
#define ENET_EIRM_LC			((uint32_t)0x00200000)		// Late Collision Mask
#define ENET_EIRM_RL			((uint32_t)0x00100000)		// Collision Retry Limit Mask
#define ENET_EIRM_UN			((uint32_t)0x00080000)		// Transmit FIFO Underrun Mask
#define ENET_EIRM_PLR			((uint32_t)0x00040000)		// Payload Receive Error Mask
#define ENET_EIRM_WAKEUP		((uint32_t)0x00020000)		// Node Wakeup Request Indication Mask
#define ENET_EIRM_TS_AVAIL		((uint32_t)0x00010000)		// Transmit Timestamp Available Mask
#define ENET_EIRM_TS_TIMER		((uint32_t)0x00008000)		// Timestamp Timer Mask
#define ENET_RDAR		(*(volatile uint32_t *)0x400C0010) // Receive Descriptor Active Register
#define ENET_RDAR_RDAR			((uint32_t)0x01000000)
#define ENET_TDAR		(*(volatile uint32_t *)0x400C0014) // Transmit Descriptor Active Register
#define ENET_TDAR_TDAR			((uint32_t)0x01000000)
#define ENET_ECR		(*(volatile uint32_t *)0x400C0024) // Ethernet Control Register
#define ENET_ECR_DBSWP			((uint32_t)0x00000100)		// Descriptor Byte Swapping Enable
#define ENET_ECR_STOPEN			((uint32_t)0x00000080)		// STOPEN Signal Control
#define ENET_ECR_DBGEN			((uint32_t)0x00000040)		// Debug Enable
#define ENET_ECR_EN1588			((uint32_t)0x00000010)		// EN1588 Enable
#define ENET_ECR_SLEEP			((uint32_t)0x00000008)		// Sleep Mode Enable
#define ENET_ECR_MAGICEN		((uint32_t)0x00000004)		// Magic Packet Detection Enable
#define ENET_ECR_ETHEREN		((uint32_t)0x00000002)		// Ethernet Enable
#define ENET_ECR_RESET			((uint32_t)0x00000001)		// Ethernet MAC Reset
#define ENET_MMFR		(*(volatile uint32_t *)0x400C0040) // MII Management Frame Register
#define ENET_MMFR_ST(n)			(uint32_t)(((n) & 0x3) << 30)
#define ENET_MMFR_OP(n)			(uint32_t)(((n) & 0x3) << 28)
#define ENET_MMFR_PA(n)			(uint32_t)(((n) & 0x1F) << 23)
#define ENET_MMFR_RA(n)			(uint32_t)(((n) & 0x1F) << 18)
#define ENET_MMFR_TA(n)			(uint32_t)(((n) & 0x3) << 16)
#define ENET_MMFR_DATA(n)		(uint32_t)(((n) & 0xFFFF) << 0)
#define ENET_MMFR_DATA_MASK		((uint32_t)0x0000FFFF)
#define ENET_MSCR		(*(volatile uint32_t *)0x400C0044) // MII Speed Control Register
#define ENET_MSCR_HOLDTIME(n)		(uint32_t)(((n) & 0x7) << 8)
#define ENET_MSCR_DIS_PRE		((uint32_t)0x00000080)
#define ENET_MSCR_MII_SPEED(n)		(uint32_t)(((n) & 0x3F) << 1)
#define ENET_MIBC		(*(volatile uint32_t *)0x400C0064) // MIB Control Register
#define ENET_MIBC_MIB_DIS		((uint32_t)0x80000000)		// Disable MIB Logic
#define ENET_MIBC_MIB_IDLE		((uint32_t)0x40000000)		// MIB Idle
#define ENET_MIBC_MIB_CLEAR		((uint32_t)0x20000000)		// MIB Clear
#define ENET_RCR		(*(volatile uint32_t *)0x400C0084) // Receive Control Register
#define ENET_RCR_GRS			((uint32_t)0x80000000)		// Graceful Receive Stopped
#define ENET_RCR_NLC			((uint32_t)0x40000000)		// Payload Length Check Disable
#define ENET_RCR_MAX_FL(n)		(uint32_t)(((n) & 0x3FFF)<<16)	// Maximum Frame Length
#define ENET_RCR_CFEN			((uint32_t)0x00008000)		// MAC Control Frame Enable
#define ENET_RCR_CRCFWD			((uint32_t)0x00004000)		// Terminate/Forward Received CRC
#define ENET_RCR_PAUFWD			((uint32_t)0x00002000)		// Terminate/Forward Pause Frames
#define ENET_RCR_PADEN			((uint32_t)0x00001000)		// Enable Frame Padding Remove On Receive
#define ENET_RCR_RMII_10T		((uint32_t)0x00000200)		// Enables 10-Mbps mode of the RMII
#define ENET_RCR_RMII_MODE		((uint32_t)0x00000100)		// RMII Mode Enable
#define ENET_RCR_FCE			((uint32_t)0x00000020)		// Flow Control Enable
#define ENET_RCR_BC_REJ			((uint32_t)0x00000010)		// Broadcast Frame Reject
#define ENET_RCR_PROM			((uint32_t)0x00000008)		// Promiscuous Mode
#define ENET_RCR_MII_MODE		((uint32_t)0x00000004)		// Media Independent Interface Mode
#define ENET_RCR_DRT			((uint32_t)0x00000002)		// Disable Receive On Transmit
#define ENET_RCR_LOOP			((uint32_t)0x00000001)		// Internal Loopback
#define ENET_TCR		(*(volatile uint32_t *)0x400C00C4) // Transmit Control Register
#define ENET_TCR_CRCFWD			((uint32_t)0x00000200)		// Forward Frame From Application With CRC
#define ENET_TCR_ADDINS			((uint32_t)0x00000100)		// Set MAC Address On Transmit
#define ENET_TCR_ADDSEL(n)		(uint32_t)(((n) & 0x7)<<5)	// Source MAC Address Select On Transmit
#define ENET_TCR_RFC_PAUSE		((uint32_t)0x00000010)		// Receive Frame Control Pause
#define ENET_TCR_TFC_PAUSE		((uint32_t)0x00000008)		// Transmit Frame Control Pause
#define ENET_TCR_FDEN			((uint32_t)0x00000004)		// Full-Duplex Enable
#define ENET_TCR_GTS			((uint32_t)0x00000001)		// Graceful Transmit Stop
#define ENET_PALR		(*(volatile uint32_t *)0x400C00E4) // Physical Address Lower Register
#define ENET_PAUR		(*(volatile uint32_t *)0x400C00E8) // Physical Address Upper Register
#define ENET_OPD		(*(volatile uint32_t *)0x400C00EC) // Opcode/Pause Duration Register
#define ENET_IAUR		(*(volatile uint32_t *)0x400C0118) // Descriptor Individual Upper Address Register
#define ENET_IALR		(*(volatile uint32_t *)0x400C011C) // Descriptor Individual Lower Address Register
#define ENET_GAUR		(*(volatile uint32_t *)0x400C0120) // Descriptor Group Upper Address Register
#define ENET_GALR		(*(volatile uint32_t *)0x400C0124) // Descriptor Group Lower Address Register
#define ENET_TFWR		(*(volatile uint32_t *)0x400C0144) // Transmit FIFO Watermark Register
#define ENET_TFWR_STRFWD		((uint32_t)0x00000100)		// Store And Forward Enable
#define ENET_TFWR_TFWR(n)		(uint32_t)(((n) & 0x3F)<<0)	// Transmit FIFO Write (X64 bytes)
#define ENET_RDSR		(*(volatile uint32_t *)0x400C0180) // Receive Descriptor Ring Start Register
#define ENET_TDSR		(*(volatile uint32_t *)0x400C0184) // Transmit Buffer Descriptor Ring Start Register
#define ENET_MRBR		(*(volatile uint32_t *)0x400C0188) // Maximum Receive Buffer Size Register
#define ENET_RSFL		(*(volatile uint32_t *)0x400C0190) // Receive FIFO Section Full Threshold
#define ENET_RSEM		(*(volatile uint32_t *)0x400C0194) // Receive FIFO Section Empty Threshold
#define ENET_RSEM_STAT_SECTION_EMPTY(n)	(uint32_t)(((n) & 0x7)<<5)	// RX Status FIFO Section Empty Threshold
#define ENET_RSEM_RX_SECTION_EMPTY(n)	(uint32_t)(((n) & 0x7)<<5)	// Value Of The Receive FIFO Section Empty Threshold
#define ENET_RAEM		(*(volatile uint32_t *)0x400C0198) // Receive FIFO Almost Empty Threshold
#define ENET_RAFL		(*(volatile uint32_t *)0x400C019C) // Receive FIFO Almost Full Threshold
#define ENET_TSEM		(*(volatile uint32_t *)0x400C01A0) // Transmit FIFO Section Empty Threshold
#define ENET_TAEM		(*(volatile uint32_t *)0x400C01A4) // Transmit FIFO Almost Empty Threshold
#define ENET_TAFL		(*(volatile uint32_t *)0x400C01A8) // Transmit FIFO Almost Full Threshold
#define ENET_TIPG		(*(volatile uint32_t *)0x400C01AC) // Transmit Inter-Packet Gap
#define ENET_FTRL		(*(volatile uint32_t *)0x400C01B0) // Frame Truncation Length
#define ENET_TACC		(*(volatile uint32_t *)0x400C01C0) // Transmit Accelerator Function Configuration
#define ENET_TACC_PROCHK		((uint32_t)0x00000010)		// Enables insertion of protocol checksum
#define ENET_TACC_IPCHK			((uint32_t)0x00000008)		// Enables insertion of IP header checksum
#define ENET_TACC_SHIFT16		((uint32_t)0x00000001)		// TX FIFO Shift-16 (align data to 32 bits)
#define ENET_RACC		(*(volatile uint32_t *)0x400C01C4) // Receive Accelerator Function Configuration
#define ENET_RACC_SHIFT16		((uint32_t)0x00000080)		// RX FIFO Shift-16 (align data to 32 bits)
#define ENET_RACC_LINEDIS		((uint32_t)0x00000040)		// Enable Discard Of Frames With MAC Layer Errors
#define ENET_RACC_PRODIS		((uint32_t)0x00000004)		// Enable Discard Of Frames With Wrong Protocol Checksum
#define ENET_RACC_IPDIS			((uint32_t)0x00000002)		// Enable Discard Of Frames With Wrong IPv4 Header Checksum
#define ENET_RACC_PADREM		((uint32_t)0x00000001)		// Enable Padding Removal For Short IP Frames
#define ENET_RMON_T_DROP	(*(volatile uint32_t *)0x400C0200) // Reserved Statistic Register
#define ENET_RMON_T_PACKETS	(*(volatile uint32_t *)0x400C0204) // Tx Packet Count Statistic Register
#define ENET_RMON_T_BC_PKT	(*(volatile uint32_t *)0x400C0208) // Tx Broadcast Packets Statistic Register
#define ENET_RMON_T_MC_PKT	(*(volatile uint32_t *)0x400C020C) // Tx Multicast Packets Statistic Register
#define ENET_RMON_T_CRC_ALIGN	(*(volatile uint32_t *)0x400C0210) // Tx Packets with CRC/Align Error Statistic Register
#define ENET_RMON_T_UNDERSIZE	(*(volatile uint32_t *)0x400C0214) // Tx Packets Less Than Bytes and Good CRC Statistic Register
#define ENET_RMON_T_OVERSIZE	(*(volatile uint32_t *)0x400C0218) // Tx Packets GT MAX_FL bytes and Good CRC Statistic Register
#define ENET_RMON_T_FRAG	(*(volatile uint32_t *)0x400C021C) // Tx Packets Less Than 64 Bytes and Bad CRC Statistic Register
#define ENET_RMON_T_JAB		(*(volatile uint32_t *)0x400C0220) // Tx Packets Greater Than MAX_FL bytes and Bad CRC Statistic Register
#define ENET_RMON_T_COL		(*(volatile uint32_t *)0x400C0224) // Tx Collision Count Statistic Register
#define ENET_RMON_T_P64		(*(volatile uint32_t *)0x400C0228) // Tx 64-Byte Packets Statistic Register
#define ENET_RMON_T_P65TO127	(*(volatile uint32_t *)0x400C022C) // Tx 65- to 127-byte Packets Statistic Register
#define ENET_RMON_T_P128TO255	(*(volatile uint32_t *)0x400C0230) // Tx 128- to 255-byte Packets Statistic Register
#define ENET_RMON_T_P256TO511	(*(volatile uint32_t *)0x400C0234) // Tx 256- to 511-byte Packets Statistic Register
#define ENET_RMON_T_P512TO1023	(*(volatile uint32_t *)0x400C0238) // Tx 512- to 1023-byte Packets Statistic Register
#define ENET_RMON_T_P1024TO2047	(*(volatile uint32_t *)0x400C023C) // Tx 1024- to 2047-byte Packets Statistic Register
#define ENET_RMON_T_P_GTE2048	(*(volatile uint32_t *)0x400C0240) // Tx Packets Greater Than 2048 Bytes Statistic Register
#define ENET_RMON_T_OCTETS	(*(volatile uint32_t *)0x400C0244) // Tx Octets Statistic Register
#define ENET_IEEE_T_DROP	(*(volatile uint32_t *)0x400C0248) // IEEE_T_DROP Reserved Statistic Register
#define ENET_IEEE_T_FRAME_OK	(*(volatile uint32_t *)0x400C024C) // Frames Transmitted OK Statistic Register
#define ENET_IEEE_T_1COL	(*(volatile uint32_t *)0x400C0250) // Frames Transmitted with Single Collision Statistic Register
#define ENET_IEEE_T_MCOL	(*(volatile uint32_t *)0x400C0254) // Frames Transmitted with Multiple Collisions Statistic Register
#define ENET_IEEE_T_DEF		(*(volatile uint32_t *)0x400C0258) // Frames Transmitted after Deferral Delay Statistic Register
#define ENET_IEEE_T_LCOL	(*(volatile uint32_t *)0x400C025C) // Frames Transmitted with Late Collision Statistic Register
#define ENET_IEEE_T_EXCOL	(*(volatile uint32_t *)0x400C0260) // Frames Transmitted with Excessive Collisions Statistic Register
#define ENET_IEEE_T_MACERR	(*(volatile uint32_t *)0x400C0264) // Frames Transmitted with Tx FIFO Underrun Statistic Register
#define ENET_IEEE_T_CSERR	(*(volatile uint32_t *)0x400C0268) // Frames Transmitted with Carrier Sense Error Statistic Register
#define ENET_IEEE_T_SQE		(*(volatile uint32_t *)0x400C026C) // ??
#define ENET_IEEE_T_FDXFC	(*(volatile uint32_t *)0x400C0270) // Flow Control Pause Frames Transmitted Statistic Register
#define ENET_IEEE_T_OCTETS_OK	(*(volatile uint32_t *)0x400C0274) // Octet Count for Frames Transmitted w/o Error Statistic Register
#define ENET_RMON_R_PACKETS	(*(volatile uint32_t *)0x400C0284) // Rx Packet Count Statistic Register
#define ENET_RMON_R_BC_PKT	(*(volatile uint32_t *)0x400C0288) // Rx Broadcast Packets Statistic Register
#define ENET_RMON_R_MC_PKT	(*(volatile uint32_t *)0x400C028C) // Rx Multicast Packets Statistic Register
#define ENET_RMON_R_CRC_ALIGN	(*(volatile uint32_t *)0x400C0290) // Rx Packets with CRC/Align Error Statistic Register
#define ENET_RMON_R_UNDERSIZE	(*(volatile uint32_t *)0x400C0294) // Rx Packets with Less Than 64 Bytes and Good CRC Statistic Register
#define ENET_RMON_R_OVERSIZE	(*(volatile uint32_t *)0x400C0298) // Rx Packets Greater Than MAX_FL and Good CRC Statistic Register
#define ENET_RMON_R_FRAG	(*(volatile uint32_t *)0x400C029C) // Rx Packets Less Than 64 Bytes and Bad CRC Statistic Register
#define ENET_RMON_R_JAB		(*(volatile uint32_t *)0x400C02A0) // Rx Packets Greater Than MAX_FL Bytes and Bad CRC Statistic Register
#define ENET_RMON_R_RESVD_0	(*(volatile uint32_t *)0x400C02A4) // Reserved Statistic Register
#define ENET_RMON_R_P64		(*(volatile uint32_t *)0x400C02A8) // Rx 64-Byte Packets Statistic Register
#define ENET_RMON_R_P65TO127	(*(volatile uint32_t *)0x400C02AC) // Rx 65- to 127-Byte Packets Statistic Register
#define ENET_RMON_R_P128TO255	(*(volatile uint32_t *)0x400C02B0) // Rx 128- to 255-Byte Packets Statistic Register
#define ENET_RMON_R_P256TO511	(*(volatile uint32_t *)0x400C02B4) // Rx 256- to 511-Byte Packets Statistic Register
#define ENET_RMON_R_P512TO1023	(*(volatile uint32_t *)0x400C02B8) // Rx 512- to 1023-Byte Packets Statistic Register
#define ENET_RMON_R_P1024TO2047	(*(volatile uint32_t *)0x400C02BC) // Rx 1024- to 2047-Byte Packets Statistic Register
#define ENET_RMON_R_P_GTE2048	(*(volatile uint32_t *)0x400C02C0) // Rx Packets Greater than 2048 Bytes Statistic Register
#define ENET_RMON_R_OCTETS	(*(volatile uint32_t *)0x400C02C4) // Rx Octets Statistic Register
#define ENET_IEEE_R_DROP	(*(volatile uint32_t *)0x400C02C8) // Frames not Counted Correctly Statistic Register
#define ENET_IEEE_R_FRAME_OK	(*(volatile uint32_t *)0x400C02CC) // Frames Received OK Statistic Register
#define ENET_IEEE_R_CRC		(*(volatile uint32_t *)0x400C02D0) // Frames Received with CRC Error Statistic Register
#define ENET_IEEE_R_ALIGN	(*(volatile uint32_t *)0x400C02D4) // Frames Received with Alignment Error Statistic Register
#define ENET_IEEE_R_MACERR	(*(volatile uint32_t *)0x400C02D8) // Receive FIFO Overflow Count Statistic Register
#define ENET_IEEE_R_FDXFC	(*(volatile uint32_t *)0x400C02DC) // Flow Control Pause Frames Received Statistic Register
#define ENET_IEEE_R_OCTETS_OK	(*(volatile uint32_t *)0x400C02E0) // Octet Count for Frames Received without Error Statistic Register
#define ENET_ATCR		(*(volatile uint32_t *)0x400C0400) // Adjustable Timer Control Register
#define ENET_ATCR_SLAVE			((uint32_t)0x00002000)		// Enable Timer Slave Mode
#define ENET_ATCR_CAPTURE		((uint32_t)0x00000800)		// Capture Timer Value
#define ENET_ATCR_RESTART		((uint32_t)0x00000400)		// Reset Timer
#define ENET_ATCR_PINPER		((uint32_t)0x00000080)		// Enables event signal output assertion on period event
#define ENET_ATCR_PEREN			((uint32_t)0x00000010)		// Enable Periodical Event
#define ENET_ATCR_OFFRST		((uint32_t)0x00000008)		// Reset Timer On Offset Event
#define ENET_ATCR_OFFEN			((uint32_t)0x00000004)		// Enable One-Shot Offset Event
#define ENET_ATCR_EN			((uint32_t)0x00000001)		// Enable Timer
#define ENET_ATVR		(*(volatile uint32_t *)0x400C0404) // Timer Value Register
#define ENET_ATOFF		(*(volatile uint32_t *)0x400C0408) // Timer Offset Register
#define ENET_ATPER		(*(volatile uint32_t *)0x400C040C) // Timer Period Register
#define ENET_ATCOR		(*(volatile uint32_t *)0x400C0410) // Timer Correction Register
#define ENET_ATINC		(*(volatile uint32_t *)0x400C0414) // Time-Stamping Clock Period Register
#define ENET_ATINC_INC_CORR(n)		(uint32_t)(((n) & 0x7F)<<8)	// Correction Increment Value
#define ENET_ATINC_INC(n)		(uint32_t)(((n) & 0x7F)<<0)	// Clock Period Of The Timestamping Clock (ts_clk) In Nanoseconds
#define ENET_ATSTMP		(*(volatile uint32_t *)0x400C0418) // Timestamp of Last Transmitted Frame
#define ENET_TGSR		(*(volatile uint32_t *)0x400C0604) // Timer Global Status Register
#define ENET_TGSR_TF3			((uint32_t)0x00000008)		// Timer Flag For Channel 3
#define ENET_TGSR_TF2			((uint32_t)0x00000004)		// Timer Flag For Channel 2
#define ENET_TGSR_TF1			((uint32_t)0x00000002)		// Timer Flag For Channel 1
#define ENET_TGSR_TF0			((uint32_t)0x00000001)		// Timer Flag For Channel 0
#define ENET_TCSR0		(*(volatile uint32_t *)0x400C0608) // Timer Control Status Register
#define ENET_TCSR_TF			((uint32_t)0x00000080)		// Timer Flag
#define ENET_TCSR_TIR			((uint32_t)0x00000040)		// Timer Interrupt Enable
#define ENET_TCSR_TMODE(n)		(uint32_t)(((n) & 0xF)<<2)	// Timer Mode
#define ENET_TCSR_TDRE			((uint32_t)0x00000001)		// Timer DMA Request Enable
#define ENET_TCCR0		(*(volatile uint32_t *)0x400C060C) // Timer Compare Capture Register
#define ENET_TCSR1		(*(volatile uint32_t *)0x400C0610) // Timer Control Status Register
#define ENET_TCCR1		(*(volatile uint32_t *)0x400C0614) // Timer Compare Capture Register
#define ENET_TCSR2		(*(volatile uint32_t *)0x400C0618) // Timer Control Status Register
#define ENET_TCCR2		(*(volatile uint32_t *)0x400C061C) // Timer Compare Capture Register
#define ENET_TCSR3		(*(volatile uint32_t *)0x400C0620) // Timer Control Status Register
#define ENET_TCCR3		(*(volatile uint32_t *)0x400C0624) // Timer Compare Capture Register


// Universal Serial Bus OTG Controller (USBOTG)

#define USB0_PERID		(*(const    uint8_t  *)0x40072000) // Peripheral ID register
#define USB0_IDCOMP		(*(const    uint8_t  *)0x40072004) // Peripheral ID Complement register
#define USB0_REV		(*(const    uint8_t  *)0x40072008) // Peripheral Revision register
#define USB0_ADDINFO		(*(volatile uint8_t  *)0x4007200C) // Peripheral Additional Info register
#define USB0_OTGISTAT		(*(volatile uint8_t  *)0x40072010) // OTG Interrupt Status register
#define USB_OTGISTAT_IDCHG		((uint8_t)0x80)			//
#define USB_OTGISTAT_ONEMSEC		((uint8_t)0x40)			//
#define USB_OTGISTAT_LINE_STATE_CHG	((uint8_t)0x20)			//
#define USB_OTGISTAT_SESSVLDCHG		((uint8_t)0x08)			//
#define USB_OTGISTAT_B_SESS_CHG		((uint8_t)0x04)			//
#define USB_OTGISTAT_AVBUSCHG		((uint8_t)0x01)			//
#define USB0_OTGICR		(*(volatile uint8_t  *)0x40072014) // OTG Interrupt Control Register
#define USB_OTGICR_IDEN			((uint8_t)0x80)			//
#define USB_OTGICR_ONEMSECEN		((uint8_t)0x40)			//
#define USB_OTGICR_LINESTATEEN		((uint8_t)0x20)			//
#define USB_OTGICR_SESSVLDEN		((uint8_t)0x08)			//
#define USB_OTGICR_BSESSEN		((uint8_t)0x04)			//
#define USB_OTGICR_AVBUSEN		((uint8_t)0x01)			//
#define USB0_OTGSTAT		(*(volatile uint8_t  *)0x40072018) // OTG Status register
#define USB_OTGSTAT_ID			((uint8_t)0x80)			//
#define USB_OTGSTAT_ONEMSECEN		((uint8_t)0x40)			//
#define USB_OTGSTAT_LINESTATESTABLE	((uint8_t)0x20)			//
#define USB_OTGSTAT_SESS_VLD		((uint8_t)0x08)			//
#define USB_OTGSTAT_BSESSEND		((uint8_t)0x04)			//
#define USB_OTGSTAT_AVBUSVLD		((uint8_t)0x01)			//
#define USB0_OTGCTL		(*(volatile uint8_t  *)0x4007201C) // OTG Control Register
#define USB_OTGCTL_DPHIGH		((uint8_t)0x80)			//
#define USB_OTGCTL_DPLOW		((uint8_t)0x20)			//
#define USB_OTGCTL_DMLOW		((uint8_t)0x10)			//
#define USB_OTGCTL_OTGEN		((uint8_t)0x04)			//
#define USB0_ISTAT		(*(volatile uint8_t  *)0x40072080) // Interrupt Status Register
#define USB_ISTAT_STALL			((uint8_t)0x80)			//
#define USB_ISTAT_ATTACH		((uint8_t)0x40)			//
#define USB_ISTAT_RESUME		((uint8_t)0x20)			//
#define USB_ISTAT_SLEEP			((uint8_t)0x10)			//
#define USB_ISTAT_TOKDNE		((uint8_t)0x08)			//
#define USB_ISTAT_SOFTOK		((uint8_t)0x04)			//
#define USB_ISTAT_ERROR			((uint8_t)0x02)			//
#define USB_ISTAT_USBRST		((uint8_t)0x01)			//
#define USB0_INTEN		(*(volatile uint8_t  *)0x40072084) // Interrupt Enable Register
#define USB_INTEN_STALLEN		((uint8_t)0x80)			//
#define USB_INTEN_ATTACHEN		((uint8_t)0x40)			//
#define USB_INTEN_RESUMEEN		((uint8_t)0x20)			//
#define USB_INTEN_SLEEPEN		((uint8_t)0x10)			//
#define USB_INTEN_TOKDNEEN		((uint8_t)0x08)			//
#define USB_INTEN_SOFTOKEN		((uint8_t)0x04)			//
#define USB_INTEN_ERROREN		((uint8_t)0x02)			//
#define USB_INTEN_USBRSTEN		((uint8_t)0x01)			//
#define USB0_ERRSTAT		(*(volatile uint8_t  *)0x40072088) // Error Interrupt Status Register
#define USB_ERRSTAT_BTSERR		((uint8_t)0x80)			//
#define USB_ERRSTAT_DMAERR		((uint8_t)0x20)			//
#define USB_ERRSTAT_BTOERR		((uint8_t)0x10)			//
#define USB_ERRSTAT_DFN8		((uint8_t)0x08)			//
#define USB_ERRSTAT_CRC16		((uint8_t)0x04)			//
#define USB_ERRSTAT_CRC5EOF		((uint8_t)0x02)			//
#define USB_ERRSTAT_PIDERR		((uint8_t)0x01)			//
#define USB0_ERREN		(*(volatile uint8_t  *)0x4007208C) // Error Interrupt Enable Register
#define USB_ERREN_BTSERREN		((uint8_t)0x80)			//
#define USB_ERREN_DMAERREN		((uint8_t)0x20)			//
#define USB_ERREN_BTOERREN		((uint8_t)0x10)			//
#define USB_ERREN_DFN8EN		((uint8_t)0x08)			//
#define USB_ERREN_CRC16EN		((uint8_t)0x04)			//
#define USB_ERREN_CRC5EOFEN		((uint8_t)0x02)			//
#define USB_ERREN_PIDERREN		((uint8_t)0x01)			//
#define USB0_STAT		(*(volatile uint8_t  *)0x40072090) // Status Register
#define USB_STAT_TX			((uint8_t)0x08)			//
#define USB_STAT_ODD			((uint8_t)0x04)			//
#define USB_STAT_ENDP(n)		((uint8_t)((n) >> 4))		//
#define USB0_CTL		(*(volatile uint8_t  *)0x40072094) // Control Register
#define USB_CTL_JSTATE			((uint8_t)0x80)			//
#define USB_CTL_SE0			((uint8_t)0x40)			//
#define USB_CTL_TXSUSPENDTOKENBUSY	((uint8_t)0x20)			//
#define USB_CTL_RESET			((uint8_t)0x10)			//
#define USB_CTL_HOSTMODEEN		((uint8_t)0x08)			//
#define USB_CTL_RESUME			((uint8_t)0x04)			//
#define USB_CTL_ODDRST			((uint8_t)0x02)			//
#define USB_CTL_USBENSOFEN		((uint8_t)0x01)			//
#define USB0_ADDR		(*(volatile uint8_t  *)0x40072098) // Address Register
#define USB0_BDTPAGE1		(*(volatile uint8_t  *)0x4007209C) // BDT Page Register 1
#define USB0_FRMNUML		(*(volatile uint8_t  *)0x400720A0) // Frame Number Register Low
#define USB0_FRMNUMH		(*(volatile uint8_t  *)0x400720A4) // Frame Number Register High
#define USB0_TOKEN		(*(volatile uint8_t  *)0x400720A8) // Token Register
#define USB0_SOFTHLD		(*(volatile uint8_t  *)0x400720AC) // SOF Threshold Register
#define USB0_BDTPAGE2		(*(volatile uint8_t  *)0x400720B0) // BDT Page Register 2
#define USB0_BDTPAGE3		(*(volatile uint8_t  *)0x400720B4) // BDT Page Register 3
#define USB0_ENDPT0		(*(volatile uint8_t  *)0x400720C0) // Endpoint Control Register
#define USB_ENDPT_HOSTWOHUB		((uint8_t)0x80)			// host only, enable low speed
#define USB_ENDPT_RETRYDIS		((uint8_t)0x40)			// host only, set to disable NAK retry
#define USB_ENDPT_EPCTLDIS		((uint8_t)0x10)			// 0=control, 1=bulk, interrupt, isync
#define USB_ENDPT_EPRXEN		((uint8_t)0x08)			// enables the endpoint for RX transfers.
#define USB_ENDPT_EPTXEN		((uint8_t)0x04)			// enables the endpoint for TX transfers.
#define USB_ENDPT_EPSTALL		((uint8_t)0x02)			// set to stall endpoint
#define USB_ENDPT_EPHSHK		((uint8_t)0x01)			// enable handshaking during a transaction, generally set unless Isochronous
#define USB0_ENDPT1		(*(volatile uint8_t  *)0x400720C4) // Endpoint Control Register
#define USB0_ENDPT2		(*(volatile uint8_t  *)0x400720C8) // Endpoint Control Register
#define USB0_ENDPT3		(*(volatile uint8_t  *)0x400720CC) // Endpoint Control Register
#define USB0_ENDPT4		(*(volatile uint8_t  *)0x400720D0) // Endpoint Control Register
#define USB0_ENDPT5		(*(volatile uint8_t  *)0x400720D4) // Endpoint Control Register
#define USB0_ENDPT6		(*(volatile uint8_t  *)0x400720D8) // Endpoint Control Register
#define USB0_ENDPT7		(*(volatile uint8_t  *)0x400720DC) // Endpoint Control Register
#define USB0_ENDPT8		(*(volatile uint8_t  *)0x400720E0) // Endpoint Control Register
#define USB0_ENDPT9		(*(volatile uint8_t  *)0x400720E4) // Endpoint Control Register
#define USB0_ENDPT10		(*(volatile uint8_t  *)0x400720E8) // Endpoint Control Register
#define USB0_ENDPT11		(*(volatile uint8_t  *)0x400720EC) // Endpoint Control Register
#define USB0_ENDPT12		(*(volatile uint8_t  *)0x400720F0) // Endpoint Control Register
#define USB0_ENDPT13		(*(volatile uint8_t  *)0x400720F4) // Endpoint Control Register
#define USB0_ENDPT14		(*(volatile uint8_t  *)0x400720F8) // Endpoint Control Register
#define USB0_ENDPT15		(*(volatile uint8_t  *)0x400720FC) // Endpoint Control Register
#define USB0_USBCTRL		(*(volatile uint8_t  *)0x40072100) // USB Control Register
#define USB_USBCTRL_SUSP		((uint8_t)0x80)			// Places the USB transceiver into the suspend state.
#define USB_USBCTRL_PDE			((uint8_t)0x40)			// Enables the weak pulldowns on the USB transceiver.
#define USB0_OBSERVE		(*(volatile uint8_t  *)0x40072104) // USB OTG Observe Register
#define USB_OBSERVE_DPPU		((uint8_t)0x80)			//
#define USB_OBSERVE_DPPD		((uint8_t)0x40)			//
#define USB_OBSERVE_DMPD		((uint8_t)0x10)			//
#define USB0_CONTROL		(*(volatile uint8_t  *)0x40072108) // USB OTG Control Register
#define USB_CONTROL_DPPULLUPNONOTG	((uint8_t)0x10)			//  Provides control of the DP PULLUP in the USB OTG module, if USB is configured in non-OTG device mode.
#define USB0_USBTRC0		(*(volatile uint8_t  *)0x4007210C) // USB Transceiver Control Register 0
#define USB_USBTRC_USBRESET		((uint8_t)0x80)			//
#define USB_USBTRC_USBRESMEN		((uint8_t)0x20)			//
#define USB_USBTRC_SYNC_DET		((uint8_t)0x02)			//
#define USB_USBTRC_USB_RESUME_INT	((uint8_t)0x01)			//
#define USB0_USBFRMADJUST	(*(volatile uint8_t  *)0x40072114) // Frame Adjust Register


// USB Device Charger Detection Module (USBDCD)

#define USBDCD_CONTROL		(*(volatile uint32_t *)0x40035000) // Control register
#define USBDCD_CONTROL_SR		((uint32_t)0x02000000)
#define USBDCD_CONTROL_START		((uint32_t)0x01000000)
#define USBDCD_CONTROL_BC12		((uint32_t)0x00020000)
#define USBDCD_CONTROL_IE		((uint32_t)0x00010000)
#define USBDCD_CONTROL_IF		((uint32_t)0x00000100)
#define USBDCD_CONTROL_IACK		((uint32_t)0x00000001)
#define USBDCD_CLOCK		(*(volatile uint32_t *)0x40035004) // Clock register
#define USBDCD_CLOCK_CLOCK_SPEED(n)	(uint32_t)(((n) & 0x3FF) << 2)
#define USBDCD_CLOCK_CLOCK_UNIT		((uint32_t)0x00000001)
#define USBDCD_STATUS		(*(volatile uint32_t *)0x40035008) // Status register
#define USBDCD_STATUS_ACTIVE		((uint32_t)0x00400000)
#define USBDCD_STATUS_ID		((uint32_t)0x00200000)
#define USBDCD_STATUS_ERR		((uint32_t)0x00100000)
#define USBDCD_STATUS_SEQ_STAT(n)	(uint32_t)(((n) & 0x3) << 18)
#define USBDCD_STATUS_SEQ_STAT_MASK	((uint32_t)0x000C0000)
#define USBDCD_STATUS_SEQ_RES(n)	(uint32_t)(((n) & 0x3) << 16)
#define USBDCD_STATUS_SEQ_RES_MASK	((uint32_t)0x00030000)
#define USBDCD_TIMER0		(*(volatile uint32_t *)0x40035010) // TIMER0 register
#define USBDCD_TIMER1		(*(volatile uint32_t *)0x40035014) // TIMER1 register
#define USBDCD_TIMER2		(*(volatile uint32_t *)0x40035018) // TIMER2 register
#define USBHSDCD_CONTROL	(*(volatile uint32_t *)0x400A3000) // Control register
#define USBHSDCD_CLOCK		(*(volatile uint32_t *)0x400A3004) // Clock register
#define USBHSDCD_STATUS		(*(volatile uint32_t *)0x400A3008) // Status register
#define USBHSDCD_TIMER0		(*(volatile uint32_t *)0x400A3010) // TIMER0 register
#define USBHSDCD_TIMER1		(*(volatile uint32_t *)0x400A3014) // TIMER1 register
#define USBHSDCD_TIMER2		(*(volatile uint32_t *)0x400A3018) // TIMER2 register


// USB High Speed OTG Controller (USBHS)

#define USBHS_ID		(*(volatile uint32_t *)0x400A1000) // Identification Register
#define USBHS_HWGENERAL		(*(volatile uint32_t *)0x400A1004) // General Hardware Parameters Register
#define USBHS_HWHOST		(*(volatile uint32_t *)0x400A1008) // Host Hardware Parameters Register
#define USBHS_HWDEVICE		(*(volatile uint32_t *)0x400A100C) // Device Hardware Parameters Register
#define USBHS_HWTXBUF		(*(volatile uint32_t *)0x400A1010) // Transmit Buffer Hardware Parameters Register
#define USBHS_HWRXBUF		(*(volatile uint32_t *)0x400A1014) // Receive Buffer Hardware Parameters Register
#define USBHS_GPTIMER0LD	(*(volatile uint32_t *)0x400A1080) // General Purpose Timer n Load Register
#define USBHS_GPTIMER0CTL	(*(volatile uint32_t *)0x400A1084) // General Purpose Timer n Control Register
#define USBHS_GPTIMERCTL_RUN		((uint32_t)0x80000000)
#define USBHS_GPTIMERCTL_RST		((uint32_t)0x40000000)
#define USBHS_GPTIMERCTL_MODE		((uint32_t)0x01000000)
#define USBHS_GPTIMERCTL_GPTCNT(n)	(uint32_t)(((n) & 0xFFFFFF) << 0)
#define USBHS_GPTIMER1LD	(*(volatile uint32_t *)0x400A1088) // General Purpose Timer n Load Register
#define USBHS_GPTIMER1CTL	(*(volatile uint32_t *)0x400A108C) // General Purpose Timer n Control Register
#define USBHS_USB_SBUSCFG	(*(volatile uint32_t *)0x400A1090) // System Bus Interface Configuration Register
#define USBHS_HCIVERSION	(*(volatile uint32_t *)0x400A1100) // Host Controller Interface Version and Capability Registers Length Register
#define USBHS_HCSPARAMS		(*(volatile uint32_t *)0x400A1104) // Host Controller Structural Parameters Register
#define USBHS_HCCPARAMS		(*(volatile uint32_t *)0x400A1108) // Host Controller Capability Parameters Register
#define USBHS_DCIVERSION	(*(volatile uint16_t *)0x400A1122) // Device Controller Interface Version
#define USBHS_DCCPARAMS		(*(volatile uint32_t *)0x400A1124) // Device Controller Capability Parameters
#define USBHS_USBCMD		(*(volatile uint32_t *)0x400A1140) // USB Command Register
#define USBHS_USBCMD_ITC(n)		(uint32_t)(((n) & 0xFF) << 16)
#define USBHS_USBCMD_FS2		((uint32_t)0x00008000)
#define USBHS_USBCMD_ATDTW		((uint32_t)0x00004000)
#define USBHS_USBCMD_SUTW		((uint32_t)0x00002000)
#define USBHS_USBCMD_ASPE		((uint32_t)0x00000800)
#define USBHS_USBCMD_ASP(n)		(uint32_t)(((n) & 0x3) << 8)
#define USBHS_USBCMD_IAA		((uint32_t)0x00000040)
#define USBHS_USBCMD_ASE		((uint32_t)0x00000020)
#define USBHS_USBCMD_PSE		((uint32_t)0x00000010)
#define USBHS_USBCMD_FS(n)		(uint32_t)(((n) & 0x3) << 2)
#define USBHS_USBCMD_RST		((uint32_t)0x00000002)
#define USBHS_USBCMD_RS			((uint32_t)0x00000001)
#define USBHS_USBSTS		(*(volatile uint32_t *)0x400A1144) // USB Status Register
#define USBHS_USBSTS_TI1		((uint32_t)0x02000000)
#define USBHS_USBSTS_TI0		((uint32_t)0x01000000)
#define USBHS_USBSTS_UPI		((uint32_t)0x00080000)
#define USBHS_USBSTS_UAI		((uint32_t)0x00040000)
#define USBHS_USBSTS_NAKI		((uint32_t)0x00010000)
#define USBHS_USBSTS_AS			((uint32_t)0x00008000)
#define USBHS_USBSTS_PS			((uint32_t)0x00004000)
#define USBHS_USBSTS_RCL		((uint32_t)0x00002000)
#define USBHS_USBSTS_HCH		((uint32_t)0x00001000)
#define USBHS_USBSTS_SLI		((uint32_t)0x00000100)
#define USBHS_USBSTS_SRI		((uint32_t)0x00000080)
#define USBHS_USBSTS_URI		((uint32_t)0x00000040)
#define USBHS_USBSTS_AAI		((uint32_t)0x00000020)
#define USBHS_USBSTS_SEI		((uint32_t)0x00000010)
#define USBHS_USBSTS_FRI		((uint32_t)0x00000008)
#define USBHS_USBSTS_PCI		((uint32_t)0x00000004)
#define USBHS_USBSTS_UEI		((uint32_t)0x00000002)
#define USBHS_USBSTS_UI			((uint32_t)0x00000001)
#define USBHS_USBINTR		(*(volatile uint32_t *)0x400A1148) // USB Interrupt Enable Register
#define USBHS_USBINTR_TIE1		((uint32_t)0x02000000)
#define USBHS_USBINTR_TIE0		((uint32_t)0x01000000)
#define USBHS_USBINTR_UPIE		((uint32_t)0x00080000)
#define USBHS_USBINTR_UAIE		((uint32_t)0x00040000)
#define USBHS_USBINTR_NAKE		((uint32_t)0x00010000)
#define USBHS_USBINTR_SLE		((uint32_t)0x00000100)
#define USBHS_USBINTR_SRE		((uint32_t)0x00000080)
#define USBHS_USBINTR_URE		((uint32_t)0x00000040)
#define USBHS_USBINTR_AAE		((uint32_t)0x00000020)
#define USBHS_USBINTR_SEE		((uint32_t)0x00000010)
#define USBHS_USBINTR_FRE		((uint32_t)0x00000008)
#define USBHS_USBINTR_PCE		((uint32_t)0x00000004)
#define USBHS_USBINTR_UEE		((uint32_t)0x00000002)
#define USBHS_USBINTR_UE		((uint32_t)0x00000001)
#define USBHS_FRINDEX		(*(volatile uint32_t *)0x400A114C) // Frame Index Register
#define USBHS_PERIODICLISTBASE	(*(volatile uint32_t *)0x400A1154) // Periodic Frame List Base Address Register
#define USBHS_DEVICEADDR	(*(volatile uint32_t *)0x400A1154) // Device Address Register
#define USBHS_DEVICEADDR_USBADR(n)	(uint32_t)(((n) & 0x7F) << 25)
#define USBHS_DEVICEADDR_USBADRA	((uint32_t)0x01000000)
#define USBHS_ASYNCLISTADDR	(*(volatile uint32_t *)0x400A1158) // Current Asynchronous List Address Register
#define USBHS_EPLISTADDR	(*(volatile uint32_t *)0x400A1158) // Endpoint List Address Register
#define USBHS_TTCTRL		(*(volatile uint32_t *)0x400A115C) // Host TT Asynchronous Buffer Control
#define USBHS_TTCTRL_TTHA(n)		(uint32_t)(((n) & 0x7F) << 24)
#define USBHS_BURSTSIZE		(*(volatile uint32_t *)0x400A1160) // Master Interface Data Burst Size Register
#define USBHS_BURSTSIZE_TXPBURST(n)	(uint32_t)(((n) & 0xFF) << 8)
#define USBHS_BURSTSIZE_RXPBURST(n)	(uint32_t)(((n) & 0xFF) << 0)
#define USBHS_TXFILLTUNING	(*(volatile uint32_t *)0x400A1164) // Transmit FIFO Tuning Control Register
#define USBHS_TXFILLTUNING_TXFIFOTHRES(n) (uint32_t)(((n) & 0x3F) << 16)
#define USBHS_TXFILLTUNING_TXSCHHEALTH(n) (uint32_t)(((n) & 0x1F) << 8)
#define USBHS_TXFILLTUNING_TXSCHOH(n)	(uint32_t)(((n) & 0x7F) << 0)
#define USBHS_ENDPTNAK		(*(volatile uint32_t *)0x400A1178) // Endpoint NAK Register
#define USBHS_ENDPTNAKEN	(*(volatile uint32_t *)0x400A117C) // Endpoint NAK Enable Register
#define USBHS_CONFIGFLAG	(*(volatile uint32_t *)0x400A1180) // Configure Flag Register
#define USBHS_PORTSC1		(*(volatile uint32_t *)0x400A1184) // Port Status and Control Registers
#define USBHS_PORTSC_PTS(n)		(uint32_t)(((n) & 0x3) << 30)
#define USBHS_PORTSC_PSPD(n)		(uint32_t)(((n) & 0x3) << 26)
#define USBHS_PORTSC_PTS2		((uint32_t)0x02000000)
#define USBHS_PORTSC_PFSC		((uint32_t)0x01000000)
#define USBHS_PORTSC_PHCD		((uint32_t)0x00800000)
#define USBHS_PORTSC_WKOC		((uint32_t)0x00400000)
#define USBHS_PORTSC_WKDS		((uint32_t)0x00200000)
#define USBHS_PORTSC_WKCN		((uint32_t)0x00100000)
#define USBHS_PORTSC_PTC(n)		(uint32_t)(((n) & 0xF) << 16)
#define USBHS_PORTSC_PIC(n)		(uint32_t)(((n) & 0x3) << 14)
#define USBHS_PORTSC_PO			((uint32_t)0x00002000)
#define USBHS_PORTSC_PP			((uint32_t)0x00001000)
#define USBHS_PORTSC_LS(n)		(uint32_t)(((n) & 0x3) << 10)
#define USBHS_PORTSC_HSP		((uint32_t)0x00000200)
#define USBHS_PORTSC_PR			((uint32_t)0x00000100)
#define USBHS_PORTSC_SUSP		((uint32_t)0x00000080)
#define USBHS_PORTSC_FPR		((uint32_t)0x00000040)
#define USBHS_PORTSC_OCC		((uint32_t)0x00000020)
#define USBHS_PORTSC_OCA		((uint32_t)0x00000010)
#define USBHS_PORTSC_PEC		((uint32_t)0x00000008)
#define USBHS_PORTSC_PE			((uint32_t)0x00000004)
#define USBHS_PORTSC_CSC		((uint32_t)0x00000002)
#define USBHS_PORTSC_CCS		((uint32_t)0x00000001)
#define USBHS_OTGSC		(*(volatile uint32_t *)0x400A11A4) // On-the-Go Status and Control Register
#define USBHS_OTGSC_DPIE		((uint32_t)0x40000000)
#define USBHS_OTGSC_MSE			((uint32_t)0x20000000)
#define USBHS_OTGSC_BSEIE		((uint32_t)0x10000000)
#define USBHS_OTGSC_BSVIE		((uint32_t)0x08000000)
#define USBHS_OTGSC_ASVIE		((uint32_t)0x04000000)
#define USBHS_OTGSC_AVVIE		((uint32_t)0x02000000)
#define USBHS_OTGSC_IDIE		((uint32_t)0x01000000)
#define USBHS_OTGSC_DPIS		((uint32_t)0x00400000)
#define USBHS_OTGSC_MSS			((uint32_t)0x00200000)
#define USBHS_OTGSC_BSEIS		((uint32_t)0x00100000)
#define USBHS_OTGSC_BSVIS		((uint32_t)0x00080000)
#define USBHS_OTGSC_ASVIS		((uint32_t)0x00040000)
#define USBHS_OTGSC_AVVIS		((uint32_t)0x00020000)
#define USBHS_OTGSC_IDIS		((uint32_t)0x00010000)
#define USBHS_OTGSC_DPS			((uint32_t)0x00004000)
#define USBHS_OTGSC_MST			((uint32_t)0x00002000)
#define USBHS_OTGSC_BSE			((uint32_t)0x00001000)
#define USBHS_OTGSC_BSV			((uint32_t)0x00000800)
#define USBHS_OTGSC_ASV			((uint32_t)0x00000400)
#define USBHS_OTGSC_AVV			((uint32_t)0x00000200)
#define USBHS_OTGSC_ID			((uint32_t)0x00000100)
#define USBHS_OTGSC_HABA		((uint32_t)0x00000080)
#define USBHS_OTGSC_IDPU		((uint32_t)0x00000020)
#define USBHS_OTGSC_DP			((uint32_t)0x00000010)
#define USBHS_OTGSC_OT			((uint32_t)0x00000008)
#define USBHS_OTGSC_HAAR		((uint32_t)0x00000004)
#define USBHS_OTGSC_VC			((uint32_t)0x00000002)
#define USBHS_OTGSC_VD			((uint32_t)0x00000001)
#define USBHS_USBMODE		(*(volatile uint32_t *)0x400A11A8) // USB Mode Register
#define USBHS_USBMODE_TXHSD(n)		(uint32_t)(((n) & 0x7) << 12)
#define USBHS_USBMODE_SDIS		((uint32_t)0x00000010)
#define USBHS_USBMODE_SLOM		((uint32_t)0x00000008)
#define USBHS_USBMODE_ES		((uint32_t)0x00000004)
#define USBHS_USBMODE_CM(n)		(uint32_t)(((n) & 0x3) << 0)
#define USBHS_EPSETUPSR		(*(volatile uint32_t *)0x400A11AC) // Endpoint Setup Status Register
#define USBHS_EPPRIME		(*(volatile uint32_t *)0x400A11B0) // Endpoint Initialization Register
#define USBHS_EPFLUSH		(*(volatile uint32_t *)0x400A11B4) // Endpoint Flush Register
#define USBHS_EPSR		(*(volatile uint32_t *)0x400A11B8) // Endpoint Status Register
#define USBHS_EPCOMPLETE	(*(volatile uint32_t *)0x400A11BC) // Endpoint Complete Register
#define USBHS_EPCR0		(*(volatile uint32_t *)0x400A11C0) // Endpoint Control Register 0
#define USBHS_EPCR_TXE			((uint32_t)0x00800000)
#define USBHS_EPCR_TXR			((uint32_t)0x00400000)
#define USBHS_EPCR_TXI			((uint32_t)0x00200000)
#define USBHS_EPCR_TXT(n)		(uint32_t)(((n) & 0x3) << 18)
#define USBHS_EPCR_TXD			((uint32_t)0x00020000)
#define USBHS_EPCR_TXS			((uint32_t)0x00010000)
#define USBHS_EPCR_RXE			((uint32_t)0x00000080)
#define USBHS_EPCR_RXR			((uint32_t)0x00000040)
#define USBHS_EPCR_RXI			((uint32_t)0x00000020)
#define USBHS_EPCR_RXT(n)		(uint32_t)(((n) & 0x3) << 2)
#define USBHS_EPCR_RXD			((uint32_t)0x00000002)
#define USBHS_EPCR_RXS			((uint32_t)0x00000001)
#define USBHS_EPCR1		(*(volatile uint32_t *)0x400A11C4) // Endpoint Control Register 1
#define USBHS_EPCR2		(*(volatile uint32_t *)0x400A11C8) // Endpoint Control Register 2
#define USBHS_EPCR3		(*(volatile uint32_t *)0x400A11CC) // Endpoint Control Register 3
#define USBHS_EPCR4		(*(volatile uint32_t *)0x400A11D0) // Endpoint Control Register 4
#define USBHS_EPCR5		(*(volatile uint32_t *)0x400A11D4) // Endpoint Control Register 5
#define USBHS_EPCR6		(*(volatile uint32_t *)0x400A11D8) // Endpoint Control Register 6
#define USBHS_EPCR7		(*(volatile uint32_t *)0x400A11DC) // Endpoint Control Register 7
#define USBHS_USBGENCTRL	(*(volatile uint32_t *)0x400A1200) // USB General Control Register
#define USBHS_USBGENCTRL_WU_INT_CLR	((uint32_t)0x00000020)
#define USBHS_USBGENCTRL_WU_IE		((uint32_t)0x00000001)


// Universal Serial Bus 2.0 Integrated PHY (USB-PHY)

#define USBPHY_PWD		(*(volatile uint32_t *)0x400A2000) // USB PHY Power-Down Register
#define USBPHY_PWD_RXPWDRX			((uint32_t)0x00100000)
#define USBPHY_PWD_RXPWDDIFF			((uint32_t)0x00080000)
#define USBPHY_PWD_RXPWD1PT1			((uint32_t)0x00040000)
#define USBPHY_PWD_RXPWDENV			((uint32_t)0x00020000)
#define USBPHY_PWD_TXPWDV2I			((uint32_t)0x00001000)
#define USBPHY_PWD_TXPWDIBIAS			((uint32_t)0x00000800)
#define USBPHY_PWD_TXPWDFS			((uint32_t)0x00000400)
#define USBPHY_PWD_SET		(*(volatile uint32_t *)0x400A2004) // USB PHY Power-Down Register
#define USBPHY_PWD_CLR		(*(volatile uint32_t *)0x400A2008) // USB PHY Power-Down Register
#define USBPHY_PWD_TOG		(*(volatile uint32_t *)0x400A200C) // USB PHY Power-Down Register
#define USBPHY_TX		(*(volatile uint32_t *)0x400A2010) // USB PHY Transmitter Control Register
#define USBPHY_TX_SET		(*(volatile uint32_t *)0x400A2014) // USB PHY Transmitter Control Register
#define USBPHY_TX_CLR		(*(volatile uint32_t *)0x400A2018) // USB PHY Transmitter Control Register
#define USBPHY_TX_TOG		(*(volatile uint32_t *)0x400A201C) // USB PHY Transmitter Control Register
#define USBPHY_RX		(*(volatile uint32_t *)0x400A2020) // USB PHY Receiver Control Register
#define USBPHY_RX_SET		(*(volatile uint32_t *)0x400A2024) // USB PHY Receiver Control Register
#define USBPHY_RX_CLR		(*(volatile uint32_t *)0x400A2028) // USB PHY Receiver Control Register
#define USBPHY_RX_TOG		(*(volatile uint32_t *)0x400A202C) // USB PHY Receiver Control Register
#define USBPHY_CTRL		(*(volatile uint32_t *)0x400A2030) // USB PHY General Control Register
#define USBPHY_CTRL_SFTRST			((uint32_t)0x80000000)
#define USBPHY_CTRL_CLKGATE			((uint32_t)0x40000000)
#define USBPHY_CTRL_UTMI_SUSPENDM		((uint32_t)0x20000000)
#define USBPHY_CTRL_HOST_FORCE_LS_SE0		((uint32_t)0x10000000)
#define USBPHY_CTRL_OTG_ID_VALUE		((uint32_t)0x08000000)
#define USBPHY_CTRL_FSDLL_RST_EN		((uint32_t)0x01000000)
#define USBPHY_CTRL_ENAUTOCLR_PHY_PWD		((uint32_t)0x00100000)
#define USBPHY_CTRL_ENAUTOCLR_CLKGATE		((uint32_t)0x00080000)
#define USBPHY_CTRL_AUTORESUME_EN		((uint32_t)0x00040000)
#define USBPHY_CTRL_ENUTMILEVEL3		((uint32_t)0x00008000)
#define USBPHY_CTRL_ENUTMILEVEL2		((uint32_t)0x00004000)
#define USBPHY_CTRL_DEVPLUGIN_IRQ		((uint32_t)0x00001000)
#define USBPHY_CTRL_ENDEVPLUGINDET		((uint32_t)0x00000010)
#define USBPHY_CTRL_HOSTDISCONDETECT_IRQ	((uint32_t)0x00000008)
#define USBPHY_CTRL_ENHOSTDISCONDETECT		((uint32_t)0x00000002)
#define USBPHY_CTRL_SET		(*(volatile uint32_t *)0x400A2034) // USB PHY General Control Register
#define USBPHY_CTRL_CLR		(*(volatile uint32_t *)0x400A2038) // USB PHY General Control Register
#define USBPHY_CTRL_TOG		(*(volatile uint32_t *)0x400A203C) // USB PHY General Control Register
#define USBPHY_STATUS		(*(volatile uint32_t *)0x400A2040) // USB PHY Status Register
#define USBPHY_STATUS_RESUME_STATUS		((uint32_t)0x00000400)
#define USBPHY_STATUS_OTGID_STATUS		((uint32_t)0x00000100)
#define USBPHY_STATUS_DEVPLUGIN_STATUS		((uint32_t)0x00000040)
#define USBPHY_STATUS_HOSTDISCONDETECT_STATUS	((uint32_t)0x00000008)
#define USBPHY_DEBUG		(*(volatile uint32_t *)0x400A2050) // USB PHY Debug Register
#define USBPHY_DEBUG_SET	(*(volatile uint32_t *)0x400A2054) // USB PHY Debug Register
#define USBPHY_DEBUG_CLR	(*(volatile uint32_t *)0x400A2058) // USB PHY Debug Register
#define USBPHY_DEBUG_TOG	(*(volatile uint32_t *)0x400A205C) // USB PHY Debug Register
#define USBPHY_DEBUG0_STATUS	(*(volatile uint32_t *)0x400A2060) // UTMI Debug Status Register 0
#define USBPHY_DEBUG1		(*(volatile uint32_t *)0x400A2070) // UTMI Debug Status Register 1
#define USBPHY_DEBUG1_SET	(*(volatile uint32_t *)0x400A2074) // UTMI Debug Status Register 1
#define USBPHY_DEBUG1_CLR	(*(volatile uint32_t *)0x400A2078) // UTMI Debug Status Register 1
#define USBPHY_DEBUG1_TOG	(*(volatile uint32_t *)0x400A207C) // UTMI Debug Status Register 1
#define USBPHY_VERSION		(*(volatile uint32_t *)0x400A2080) // UTMI RTL Version
#define USBPHY_PLL_SIC		(*(volatile uint32_t *)0x400A20A0) // USB PHY PLL Control/Status Register
#define USBPHY_PLL_SIC_PLL_LOCK			((uint32_t)0x80000000)
#define USBPHY_PLL_SIC_PLL_BYPASS		((uint32_t)0x00010000)
#define USBPHY_PLL_SIC_PLL_ENABLE		((uint32_t)0x00002000)
#define USBPHY_PLL_SIC_PLL_POWER		((uint32_t)0x00001000)
#define USBPHY_PLL_SIC_PLL_HOLD_RING_OFF	((uint32_t)0x00000800)
#define USBPHY_PLL_SIC_PLL_EN_USB_CLKS		((uint32_t)0x00000040)
#define USBPHY_PLL_SIC_PLL_DIV_SEL(n)		(uint32_t)((n) & 3)
#define USBPHY_PLL_SIC_SET	(*(volatile uint32_t *)0x400A20A4) // USB PHY PLL Control/Status Register
#define USBPHY_PLL_SIC_CLR	(*(volatile uint32_t *)0x400A20A8) // USB PHY PLL Control/Status Register
#define USBPHY_PLL_SIC_TOG	(*(volatile uint32_t *)0x400A20AC) // USB PHY PLL Control/Status Register
#define USBPHY_USB1_VBUS_DETECT	(*(volatile uint32_t *)0x400A20C0) // USB PHY VBUS Detect Control Register
#define USBPHY_USB1_VBUS_DETECT_EN_CHARGER_RESISTOR	((uint32_t)0x80000000)
#define USBPHY_USB1_VBUS_DETECT_DISCHARGE_VBUS		((uint32_t)0x04000000)
#define USBPHY_USB1_VBUS_DETECT_PWRUP_CMPS		((uint32_t)0x00100000)
#define USBPHY_USB1_VBUS_DETECT_VBUSVALID_TO_SESSVALID	((uint32_t)0x00040000)
#define USBPHY_USB1_VBUS_DETECT_VBUS_SOURCE_SEL(n)	(uint32_t)(((n) & 3) << 9)
#define USBPHY_USB1_VBUS_DETECT_VBUSVALID_SEL		((uint32_t)0x00000100)
#define USBPHY_USB1_VBUS_DETECT_VBUSVALID_OVERRIDE	((uint32_t)0x00000080)
#define USBPHY_USB1_VBUS_DETECT_AVALID_OVERRIDE		((uint32_t)0x00000040)
#define USBPHY_USB1_VBUS_DETECT_BVALID_OVERRIDE		((uint32_t)0x00000020)
#define USBPHY_USB1_VBUS_DETECT_SESSEND_OVERRIDE	((uint32_t)0x00000010)
#define USBPHY_USB1_VBUS_DETECT_VBUS_OVERRIDE_EN	((uint32_t)0x00000008)
#define USBPHY_USB1_VBUS_DETECT_VBUSVALID_THRESH(n)	((uint32_t)((n) & 7)
#define USBPHY_USB1_VBUS_DETECT_SET	(*(volatile uint32_t *)0x400A20C4) // USB PHY VBUS Detect Control Register
#define USBPHY_USB1_VBUS_DETECT_CLR	(*(volatile uint32_t *)0x400A20C8) // USB PHY VBUS Detect Control Register
#define USBPHY_USB1_VBUS_DETECT_TOG	(*(volatile uint32_t *)0x400A20CC) // USB PHY VBUS Detect Control Register
#define USBPHY_USB1_VBUS_DET_STAT	(*(volatile uint32_t *)0x400A20D0) // USB PHY VBUS Detector Status Register
#define USBPHY_USB1_VBUS_DET_STAT_VBUS_VALID_3V		((uint32_t)0x00000010)
#define USBPHY_USB1_VBUS_DET_STAT_VBUS_VALID		((uint32_t)0x00000008)
#define USBPHY_USB1_VBUS_DET_STAT_AVALID		((uint32_t)0x00000004)
#define USBPHY_USB1_VBUS_DET_STAT_BVALID		((uint32_t)0x00000002)
#define USBPHY_USB1_VBUS_DET_STAT_SESSEND		((uint32_t)0x00000001)
#define USBPHY_USB1_CHRG_DET_STAT	(*(volatile uint32_t *)0x400A20F0) // USB PHY Charger Detect Status Register
#define USBPHY_USB1_CHRG_DET_STAT_SECDET_DCP		((uint32_t)0x00000010)
#define USBPHY_USB1_CHRG_DET_STAT_DP_STATE		((uint32_t)0x00000008)
#define USBPHY_USB1_CHRG_DET_STAT_DM_STATE		((uint32_t)0x00000004)
#define USBPHY_USB1_CHRG_DET_STAT_CHRG_DETECTED		((uint32_t)0x00000002)
#define USBPHY_USB1_CHRG_DET_STAT_PLUG_CONTACT		((uint32_t)0x00000001)
#define USBPHY_ANACTRL		(*(volatile uint32_t *)0x400A2100) // USB PHY Analog Control Register
#define USBPHY_ANACTRL_SET	(*(volatile uint32_t *)0x400A2104) // USB PHY Analog Control Register
#define USBPHY_ANACTRL_CLR	(*(volatile uint32_t *)0x400A2108) // USB PHY Analog Control Register
#define USBPHY_ANACTRL_TOG	(*(volatile uint32_t *)0x400A210C) // USB PHY Analog Control Register
#define USBPHY_USB1_LOOPBACK	(*(volatile uint32_t *)0x400A2110) // USB PHY Loopback Control/Status Register
#define USBPHY_USB1_LOOPBACK_SET	(*(volatile uint32_t *)0x400A2114) // USB PHY Loopback Control/Status Register
#define USBPHY_USB1_LOOPBACK_CLR	(*(volatile uint32_t *)0x400A2118) // USB PHY Loopback Control/Status Register
#define USBPHY_USB1_LOOPBACK_TOG	(*(volatile uint32_t *)0x400A211C) // USB PHY Loopback Control/Status Register
#define USBPHY_USB1_LOOPBACK_HSFSCNT	(*(volatile uint32_t *)0x400A2120) // USB PHY Loopback Packet Number Select Register
#define USBPHY_USB1_LOOPBACK_HSFSCNT_SET	(*(volatile uint32_t *)0x400A2124) // USB PHY Loopback Packet Number Select Register
#define USBPHY_USB1_LOOPBACK_HSFSCNT_CLR	(*(volatile uint32_t *)0x400A2128) // USB PHY Loopback Packet Number Select Register
#define USBPHY_USB1_LOOPBACK_HSFSCNT_TOG	(*(volatile uint32_t *)0x400A212C) // USB PHY Loopback Packet Number Select Register
#define USBPHY_TRIM_OVERRIDE_EN		(*(volatile uint32_t *)0x400A2130) // USB PHY Trim Override Enable Register
#define USBPHY_TRIM_OVERRIDE_EN_SET	(*(volatile uint32_t *)0x400A2134) // USB PHY Trim Override Enable Register
#define USBPHY_TRIM_OVERRIDE_EN_CLR	(*(volatile uint32_t *)0x400A2138) // USB PHY Trim Override Enable Register
#define USBPHY_TRIM_OVERRIDE_EN_TOG	(*(volatile uint32_t *)0x400A213C) // USB PHY Trim Override Enable Register


// CAN - Controller Area Network (FlexCAN)

#define CAN0_MCR		(*(volatile uint32_t *)0x40024000) // Module Configuration Register
#define CAN0_CTRL1		(*(volatile uint32_t *)0x40024004) // Control 1 register
#define CAN0_TIMER		(*(volatile uint32_t *)0x40024008) // Free Running Timer
#define CAN0_RXMGMASK		(*(volatile uint32_t *)0x40024010) // Rx Mailboxes Global Mask Register
#define CAN0_RX14MASK		(*(volatile uint32_t *)0x40024014) // Rx 14 Mask register
#define CAN0_RX15MASK		(*(volatile uint32_t *)0x40024018) // Rx 15 Mask register
#define CAN0_ECR		(*(volatile uint32_t *)0x4002401C) // Error Counter
#define CAN0_ESR1		(*(volatile uint32_t *)0x40024020) // Error and Status 1 register
#define CAN0_IMASK1		(*(volatile uint32_t *)0x40024028) // Interrupt Masks 1 register
#define CAN0_IFLAG1		(*(volatile uint32_t *)0x40024030) // Interrupt Flags 1 register
#define CAN0_CTRL2		(*(volatile uint32_t *)0x40024034) // Control 2 register
#define CAN0_ESR2		(*(volatile uint32_t *)0x40024038) // Error and Status 2 register
#define CAN0_CRCR		(*(volatile uint32_t *)0x40024044) // CRC Register
#define CAN0_RXFGMASK		(*(volatile uint32_t *)0x40024048) // Rx FIFO Global Mask register
#define CAN0_RXFIR		(*(volatile uint32_t *)0x4002404C) // Rx FIFO Information Register
#define CAN0_RXIMR0		(*(volatile uint32_t *)0x40024880) // Rx Individual Mask Registers
#define CAN0_RXIMR1		(*(volatile uint32_t *)0x40024884) // Rx Individual Mask Registers
#define CAN0_RXIMR2		(*(volatile uint32_t *)0x40024888) // Rx Individual Mask Registers
#define CAN0_RXIMR3		(*(volatile uint32_t *)0x4002488C) // Rx Individual Mask Registers
#define CAN0_RXIMR4		(*(volatile uint32_t *)0x40024890) // Rx Individual Mask Registers
#define CAN0_RXIMR5		(*(volatile uint32_t *)0x40024894) // Rx Individual Mask Registers
#define CAN0_RXIMR6		(*(volatile uint32_t *)0x40024898) // Rx Individual Mask Registers
#define CAN0_RXIMR7		(*(volatile uint32_t *)0x4002489C) // Rx Individual Mask Registers
#define CAN0_RXIMR8		(*(volatile uint32_t *)0x400248A0) // Rx Individual Mask Registers
#define CAN0_RXIMR9		(*(volatile uint32_t *)0x400248A4) // Rx Individual Mask Registers
#define CAN0_RXIMR10		(*(volatile uint32_t *)0x400248A8) // Rx Individual Mask Registers
#define CAN0_RXIMR11		(*(volatile uint32_t *)0x400248AC) // Rx Individual Mask Registers
#define CAN0_RXIMR12		(*(volatile uint32_t *)0x400248B0) // Rx Individual Mask Registers
#define CAN0_RXIMR13		(*(volatile uint32_t *)0x400248B4) // Rx Individual Mask Registers
#define CAN0_RXIMR14		(*(volatile uint32_t *)0x400248B8) // Rx Individual Mask Registers
#define CAN0_RXIMR15		(*(volatile uint32_t *)0x400248BC) // Rx Individual Mask Registers
#define CAN1_MCR		(*(volatile uint32_t *)0x400A4000) // Module Configuration Register
#define CAN1_CTRL1		(*(volatile uint32_t *)0x400A4004) // Control 1 register
#define CAN1_TIMER		(*(volatile uint32_t *)0x400A4008) // Free Running Timer
#define CAN1_RXMGMASK		(*(volatile uint32_t *)0x400A4010) // Rx Mailboxes Global Mask Register
#define CAN1_RX14MASK		(*(volatile uint32_t *)0x400A4014) // Rx 14 Mask register
#define CAN1_RX15MASK		(*(volatile uint32_t *)0x400A4018) // Rx 15 Mask register
#define CAN1_ECR		(*(volatile uint32_t *)0x400A401C) // Error Counter
#define CAN1_ESR1		(*(volatile uint32_t *)0x400A4020) // Error and Status 1 register
#define CAN1_IMASK1		(*(volatile uint32_t *)0x400A4028) // Interrupt Masks 1 register
#define CAN1_IFLAG1		(*(volatile uint32_t *)0x400A4030) // Interrupt Flags 1 register
#define CAN1_CTRL2		(*(volatile uint32_t *)0x400A4034) // Control 2 register
#define CAN1_ESR2		(*(volatile uint32_t *)0x400A4038) // Error and Status 2 register
#define CAN1_CRCR		(*(volatile uint32_t *)0x400A4044) // CRC Register
#define CAN1_RXFGMASK		(*(volatile uint32_t *)0x400A4048) // Rx FIFO Global Mask register
#define CAN1_RXFIR		(*(volatile uint32_t *)0x400A404C) // Rx FIFO Information Register
#define CAN1_RXIMR0		(*(volatile uint32_t *)0x400A4880) // Rx Individual Mask Registers
#define CAN1_RXIMR1		(*(volatile uint32_t *)0x400A4884) // Rx Individual Mask Registers
#define CAN1_RXIMR2		(*(volatile uint32_t *)0x400A4888) // Rx Individual Mask Registers
#define CAN1_RXIMR3		(*(volatile uint32_t *)0x400A488C) // Rx Individual Mask Registers
#define CAN1_RXIMR4		(*(volatile uint32_t *)0x400A4890) // Rx Individual Mask Registers
#define CAN1_RXIMR5		(*(volatile uint32_t *)0x400A4894) // Rx Individual Mask Registers
#define CAN1_RXIMR6		(*(volatile uint32_t *)0x400A4898) // Rx Individual Mask Registers
#define CAN1_RXIMR7		(*(volatile uint32_t *)0x400A489C) // Rx Individual Mask Registers
#define CAN1_RXIMR8		(*(volatile uint32_t *)0x400A48A0) // Rx Individual Mask Registers
#define CAN1_RXIMR9		(*(volatile uint32_t *)0x400A48A4) // Rx Individual Mask Registers
#define CAN1_RXIMR10		(*(volatile uint32_t *)0x400A48A8) // Rx Individual Mask Registers
#define CAN1_RXIMR11		(*(volatile uint32_t *)0x400A48AC) // Rx Individual Mask Registers
#define CAN1_RXIMR12		(*(volatile uint32_t *)0x400A48B0) // Rx Individual Mask Registers
#define CAN1_RXIMR13		(*(volatile uint32_t *)0x400A48B4) // Rx Individual Mask Registers
#define CAN1_RXIMR14		(*(volatile uint32_t *)0x400A48B8) // Rx Individual Mask Registers
#define CAN1_RXIMR15		(*(volatile uint32_t *)0x400A48BC) // Rx Individual Mask Registers


// SPI (DSPI)

#if defined(KINETISK)
typedef struct {
	volatile uint32_t	MCR;	// 0
	volatile uint32_t	unused1;// 4
	volatile uint32_t	TCR;	// 8
	volatile uint32_t	CTAR0;	// c
	volatile uint32_t	CTAR1;	// 10
	volatile uint32_t	CTAR2;	// 14
	volatile uint32_t	CTAR3;	// 18
	volatile uint32_t	CTAR4;	// 1c
	volatile uint32_t	CTAR5;	// 20
	volatile uint32_t	CTAR6;	// 24
	volatile uint32_t	CTAR7;	// 28
	volatile uint32_t	SR;	// 2c
	volatile uint32_t	RSER;	// 30
	volatile uint32_t	PUSHR;	// 34
	volatile uint32_t	POPR;	// 38
	volatile uint32_t	TXFR[16]; // 3c
	volatile uint32_t	RXFR[16]; // 7c
} KINETISK_SPI_t;
#define KINETISK_SPI0		(*(KINETISK_SPI_t *)0x4002C000)
#define SPI0_MCR		(KINETISK_SPI0.MCR)	// DSPI Module Configuration Register
#define SPI_MCR_MSTR			((uint32_t)0x80000000)		// Master/Slave Mode Select
#define SPI_MCR_CONT_SCKE		((uint32_t)0x40000000)		//
#define SPI_MCR_DCONF(n)		(((n) & 3) << 28)		//
#define SPI_MCR_FRZ			((uint32_t)0x08000000)		//
#define SPI_MCR_MTFE			((uint32_t)0x04000000)		//
#define SPI_MCR_ROOE			((uint32_t)0x01000000)		//
#define SPI_MCR_PCSIS(n)		(((n) & 0x1F) << 16)		//
#define SPI_MCR_DOZE			((uint32_t)0x00008000)		//
#define SPI_MCR_MDIS			((uint32_t)0x00004000)		//
#define SPI_MCR_DIS_TXF			((uint32_t)0x00002000)		//
#define SPI_MCR_DIS_RXF			((uint32_t)0x00001000)		//
#define SPI_MCR_CLR_TXF			((uint32_t)0x00000800)		//
#define SPI_MCR_CLR_RXF			((uint32_t)0x00000400)		//
#define SPI_MCR_SMPL_PT(n)		(((n) & 3) << 8)		//
#define SPI_MCR_HALT			((uint32_t)0x00000001)		//
#define SPI0_TCR		(KINETISK_SPI0.TCR)	// DSPI Transfer Count Register
#define SPI0_CTAR0		(KINETISK_SPI0.CTAR0)	// DSPI Clock and Transfer Attributes Register, In Master Mode
#define SPI_CTAR_DBR			((uint32_t)0x80000000)		// Double Baud Rate
#define SPI_CTAR_FMSZ(n)		(((n) & 15) << 27)		// Frame Size (+1)
#define SPI_CTAR_CPOL			((uint32_t)0x04000000)		// Clock Polarity
#define SPI_CTAR_CPHA			((uint32_t)0x02000000)		// Clock Phase
#define SPI_CTAR_LSBFE			((uint32_t)0x01000000)		// LSB First
#define SPI_CTAR_PCSSCK(n)		(((n) & 3) << 22)		// PCS to SCK Delay Prescaler
#define SPI_CTAR_PASC(n)		(((n) & 3) << 20)		// After SCK Delay Prescaler
#define SPI_CTAR_PDT(n)			(((n) & 3) << 18)		// Delay after Transfer Prescaler
#define SPI_CTAR_PBR(n)			(((n) & 3) << 16)		// Baud Rate Prescaler
#define SPI_CTAR_CSSCK(n)		(((n) & 15) << 12)		// PCS to SCK Delay Scaler
#define SPI_CTAR_ASC(n)			(((n) & 15) << 8)		// After SCK Delay Scaler
#define SPI_CTAR_DT(n)			(((n) & 15) << 4)		// Delay After Transfer Scaler
#define SPI_CTAR_BR(n)			(((n) & 15) << 0)		// Baud Rate Scaler
#define SPI0_CTAR0_SLAVE	(KINETISK_SPI0.CTAR0)	// DSPI Clock and Transfer Attributes Register, In Slave Mode
#define SPI0_CTAR1		(KINETISK_SPI0.CTAR1)	// DSPI Clock and Transfer Attributes Register, In Master Mode
#define SPI0_SR			(KINETISK_SPI0.SR)	// DSPI Status Register
#define SPI_SR_TCF			((uint32_t)0x80000000)		// Transfer Complete Flag
#define SPI_SR_TXRXS			((uint32_t)0x40000000)		// TX and RX Status
#define SPI_SR_EOQF			((uint32_t)0x10000000)		// End of Queue Flag
#define SPI_SR_TFUF			((uint32_t)0x08000000)		// Transmit FIFO Underflow Flag
#define SPI_SR_TFFF			((uint32_t)0x02000000)		// Transmit FIFO Fill Flag
#define SPI_SR_RFOF			((uint32_t)0x00080000)		// Receive FIFO Overflow Flag
#define SPI_SR_RFDF			((uint32_t)0x00020000)		// Receive FIFO Drain Flag
#define SPI0_RSER		(KINETISK_SPI0.RSER)	// DSPI DMA/Interrupt Request Select and Enable Register
#define SPI_RSER_TCF_RE			((uint32_t)0x80000000)		// Transmission Complete Request Enable
#define SPI_RSER_EOQF_RE		((uint32_t)0x10000000)		// DSPI Finished Request Request Enable
#define SPI_RSER_TFUF_RE		((uint32_t)0x08000000)		// Transmit FIFO Underflow Request Enable
#define SPI_RSER_TFFF_RE		((uint32_t)0x02000000)		// Transmit FIFO Fill Request Enable
#define SPI_RSER_TFFF_DIRS		((uint32_t)0x01000000)		// Transmit FIFO FIll Dma or Interrupt Request Select
#define SPI_RSER_RFOF_RE		((uint32_t)0x00080000)		// Receive FIFO Overflow Request Enable
#define SPI_RSER_RFDF_RE		((uint32_t)0x00020000)		// Receive FIFO Drain Request Enable
#define SPI_RSER_RFDF_DIRS		((uint32_t)0x00010000)		// Receive FIFO Drain DMA or Interrupt Request Select
#define SPI0_PUSHR		(KINETISK_SPI0.PUSHR)	// DSPI PUSH TX FIFO Register In Master Mode
#define SPI_PUSHR_CONT			((uint32_t)0x80000000)		//
#define SPI_PUSHR_CTAS(n)		(((n) & 7) << 28)		//
#define SPI_PUSHR_EOQ			((uint32_t)0x08000000)		//
#define SPI_PUSHR_CTCNT			((uint32_t)0x04000000)		//
#define SPI_PUSHR_PCS(n)		(((n) & 31) << 16)		//
#define SPI0_PUSHR_SLAVE	(KINETISK_SPI0.PUSHR)	// DSPI PUSH TX FIFO Register In Slave Mode
#define SPI0_POPR		(KINETISK_SPI0.POPR)	// DSPI POP RX FIFO Register
#define SPI0_TXFR0		(KINETISK_SPI0.TXFR[0])	// DSPI Transmit FIFO Registers
#define SPI0_TXFR1		(KINETISK_SPI0.TXFR[1])	// DSPI Transmit FIFO Registers
#define SPI0_TXFR2		(KINETISK_SPI0.TXFR[2])	// DSPI Transmit FIFO Registers
#define SPI0_TXFR3		(KINETISK_SPI0.TXFR[3])	// DSPI Transmit FIFO Registers
#define SPI0_RXFR0		(KINETISK_SPI0.RXFR[0])	// DSPI Receive FIFO Registers
#define SPI0_RXFR1		(KINETISK_SPI0.RXFR[1])	// DSPI Receive FIFO Registers
#define SPI0_RXFR2		(KINETISK_SPI0.RXFR[2])	// DSPI Receive FIFO Registers
#define SPI0_RXFR3		(KINETISK_SPI0.RXFR[3])	// DSPI Receive FIFO Registers

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define KINETISK_SPI1		(*(KINETISK_SPI_t *)0x4002D000)
#define SPI1_MCR		(KINETISK_SPI1.MCR)	// DSPI Module Configuration Register
#define SPI1_TCR		(KINETISK_SPI1.TCR)	// DSPI Transfer Count Register
#define SPI1_CTAR0		(KINETISK_SPI1.CTAR0)	// DSPI Clock and Transfer Attributes Register, In Master Mode
#define SPI1_CTAR0_SLAVE	(KINETISK_SPI1.CTAR0)	// DSPI Clock and Transfer Attributes Register, In Slave Mode
#define SPI1_CTAR1		(KINETISK_SPI1.CTAR1)	// DSPI Clock and Transfer Attributes Register, In Master Mode
#define SPI1_SR			(KINETISK_SPI1.SR)	// DSPI Status Register
#define SPI1_RSER		(KINETISK_SPI1.RSER)	// DSPI DMA/Interrupt Request Select and Enable Register
#define SPI1_PUSHR		(KINETISK_SPI1.PUSHR)	// DSPI PUSH TX FIFO Register In Master Mode
#define SPI1_PUSHR_SLAVE	(KINETISK_SPI1.PUSHR)	// DSPI PUSH TX FIFO Register In Slave Mode
#define SPI1_POPR		(KINETISK_SPI1.POPR)	// DSPI POP RX FIFO Register
#define SPI1_TXFR0		(KINETISK_SPI1.TXFR[0])	// DSPI Transmit FIFO Registers
#define SPI1_TXFR1		(KINETISK_SPI1.TXFR[1])	// DSPI Transmit FIFO Registers
#define SPI1_TXFR2		(KINETISK_SPI1.TXFR[2])	// DSPI Transmit FIFO Registers
#define SPI1_TXFR3		(KINETISK_SPI1.TXFR[3])	// DSPI Transmit FIFO Registers
#define SPI1_RXFR0		(KINETISK_SPI1.RXFR[0])	// DSPI Receive FIFO Registers
#define SPI1_RXFR1		(KINETISK_SPI1.RXFR[1])	// DSPI Receive FIFO Registers
#define SPI1_RXFR2		(KINETISK_SPI1.RXFR[2])	// DSPI Receive FIFO Registers
#define SPI1_RXFR3		(KINETISK_SPI1.RXFR[3])	// DSPI Receive FIFO Registers

#define KINETISK_SPI2		(*(KINETISK_SPI_t *)0x400AC000)
#define SPI2_MCR		(KINETISK_SPI2.MCR)	// DSPI Module Configuration Register
#define SPI2_TCR		(KINETISK_SPI2.TCR)	// DSPI Transfer Count Register
#define SPI2_CTAR0		(KINETISK_SPI2.CTAR0)	// DSPI Clock and Transfer Attributes Register, In Master Mode
#define SPI2_CTAR0_SLAVE	(KINETISK_SPI2.CTAR0)	// DSPI Clock and Transfer Attributes Register, In Slave Mode
#define SPI2_CTAR1		(KINETISK_SPI2.CTAR1)	// DSPI Clock and Transfer Attributes Register, In Master Mode
#define SPI2_SR			(KINETISK_SPI2.SR)	// DSPI Status Register
#define SPI2_RSER		(KINETISK_SPI2.RSER)	// DSPI DMA/Interrupt Request Select and Enable Register
#define SPI2_PUSHR		(KINETISK_SPI2.PUSHR)	// DSPI PUSH TX FIFO Register In Master Mode
#define SPI2_PUSHR_SLAVE	(KINETISK_SPI2.PUSHR)	// DSPI PUSH TX FIFO Register In Slave Mode
#define SPI2_POPR		(KINETISK_SPI2.POPR)	// DSPI POP RX FIFO Register
#define SPI2_TXFR0		(KINETISK_SPI2.TXFR[0])	// DSPI Transmit FIFO Registers
#define SPI2_TXFR1		(KINETISK_SPI2.TXFR[1])	// DSPI Transmit FIFO Registers
#define SPI2_TXFR2		(KINETISK_SPI2.TXFR[2])	// DSPI Transmit FIFO Registers
#define SPI2_TXFR3		(KINETISK_SPI2.TXFR[3])	// DSPI Transmit FIFO Registers
#define SPI2_RXFR0		(KINETISK_SPI2.RXFR[0])	// DSPI Receive FIFO Registers
#define SPI2_RXFR1		(KINETISK_SPI2.RXFR[1])	// DSPI Receive FIFO Registers
#define SPI2_RXFR2		(KINETISK_SPI2.RXFR[2])	// DSPI Receive FIFO Registers
#define SPI2_RXFR3		(KINETISK_SPI2.RXFR[3])	// DSPI Receive FIFO Registers

#endif
#elif defined(KINETISL)
typedef struct {
	volatile uint8_t	S;
	volatile uint8_t	BR;
	volatile uint8_t	C2;
	volatile uint8_t	C1;
	volatile uint8_t	ML;
	volatile uint8_t	MH;
	volatile uint8_t	DL;
	volatile uint8_t	DH;
	volatile uint8_t	unused1;
	volatile uint8_t	unused2;
	volatile uint8_t	CI;
	volatile uint8_t	C3;
} KINETISL_SPI_t;
#define KINETISL_SPI0		(*(KINETISL_SPI_t *)0x40076000)
#define KINETISL_SPI1		(*(KINETISL_SPI_t *)0x40077000)
#define SPI0_S			(KINETISL_SPI0.S)		// Status
#define SPI_S_SPRF			((uint8_t)0x80)			// Read Buffer Full Flag
#define SPI_S_SPMF			((uint8_t)0x40)			// Match Flag
#define SPI_S_SPTEF			((uint8_t)0x20)			// Transmit Buffer Empty Flag
#define SPI_S_MODF			((uint8_t)0x10)			// Fault Flag
#define SPI_S_RNFULLF			((uint8_t)0x08)			// Receive FIFO nearly full flag
#define SPI_S_TNEAREF			((uint8_t)0x04)			// Transmit FIFO nearly empty flag
#define SPI_S_TXFULLF			((uint8_t)0x02)			// Transmit FIFO full flag
#define SPI_S_RFIFOEF			((uint8_t)0x01)			// Read FIFO empty flag
#define SPI0_BR			(KINETISL_SPI0.BR)		// Baud Rate
#define SPI_BR_SPPR(n)			(((n) & 7) << 4)		// Prescale = N+1
#define SPI_BR_SPR(n)			(((n) & 15) << 0)		// Baud Rate Divisor = 2^(N+1) : 0-8 -> 2 to 512
#define SPI0_C2			(KINETISL_SPI0.C2)		// Control Register 2
#define SPI_C2_SPMIE			((uint8_t)0x80)			// Match Interrupt Enable
#define SPI_C2_SPIMODE			((uint8_t)0x40)			// 0 = 8 bit mode, 1 = 16 bit mode
#define SPI_C2_TXDMAE			((uint8_t)0x20)			// Transmit DMA enable
#define SPI_C2_MODFEN			((uint8_t)0x10)			// Master Mode-Fault Function Enable
#define SPI_C2_BIDIROE			((uint8_t)0x08)			// Bidirectional Mode Output Enable
#define SPI_C2_RXDMAE			((uint8_t)0x04)			// Receive DMA enable
#define SPI_C2_SPISWAI			((uint8_t)0x02)			// SPI Stop in Wait Mode
#define SPI_C2_SPC0			((uint8_t)0x01)			// SPI Pin Control, 0=normal, 1=single bidirectional
#define SPI0_C1			(KINETISL_SPI0.C1)		// Control Register 1
#define SPI_C1_SPIE			((uint8_t)0x80)			// Interrupt Enable
#define SPI_C1_SPE			((uint8_t)0x40)			// SPI System Enable
#define SPI_C1_SPTIE			((uint8_t)0x20)			// Transmit Interrupt Enable
#define SPI_C1_MSTR			((uint8_t)0x10)			// Master/Slave Mode: 0=slave, 1=master
#define SPI_C1_CPOL			((uint8_t)0x08)			// Clock Polarity
#define SPI_C1_CPHA			((uint8_t)0x04)			// Clock Phase
#define SPI_C1_SSOE			((uint8_t)0x02)			// Slave Select Output Enable
#define SPI_C1_LSBFE			((uint8_t)0x01)			// LSB First: 0=MSB First, 1=LSB First
#define SPI0_ML			(KINETISL_SPI0.ML)		// Match Low
#define SPI0_MH			(KINETISL_SPI0.MH)		// Match High
#define SPI0_DL			(KINETISL_SPI0.DL)		// Data Low
#define SPI0_DH			(KINETISL_SPI0.DH)		// Data High
#define SPI0_CI			(KINETISL_SPI0.CI)		// Clear Interrupt
#define SPI_CI_TXFERR			((uint8_t)0x80)			// Transmit FIFO error flag
#define SPI_CI_RXFERR			((uint8_t)0x40)			// Receive FIFO error flag
#define SPI_CI_TXFOF			((uint8_t)0x20)			// Transmit FIFO overflow flag
#define SPI_CI_RXFOF			((uint8_t)0x10)			// Receive FIFO overflow flag
#define SPI_CI_TNEAREFCI		((uint8_t)0x08)			// Transmit FIFO nearly empty flag clear interrupt
#define SPI_CI_RNFULLFCI		((uint8_t)0x04)			// Receive FIFO nearly full flag clear interrupt
#define SPI_CI_SPTEFCI			((uint8_t)0x02)			// Transmit FIFO empty flag clear interrupt
#define SPI_CI_SPRFCI			((uint8_t)0x01)			// Receive FIFO full flag clear interrupt
#define SPI0_C3			(KINETISL_SPI0.C3)		// Control Register 3
#define SPI_C3_TNEAREF_MARK		((uint8_t)0x20)			// Transmit FIFO nearly empty watermark
#define SPI_C3_RNFULLF_MARK		((uint8_t)0x10)			// Receive FIFO nearly full watermark
#define SPI_C3_INTCLR			((uint8_t)0x08)			// Interrupt clearing mechanism select
#define SPI_C3_TNEARIEN			((uint8_t)0x04)			// Transmit FIFO nearly empty interrupt enable
#define SPI_C3_RNFULLIEN		((uint8_t)0x02)			// Receive FIFO nearly full interrupt enable
#define SPI_C3_FIFOMODE			((uint8_t)0x01)			// FIFO mode enable
#define SPI1_S			(KINETISL_SPI1.S)		// Status
#define SPI1_BR			(KINETISL_SPI1.BR)		// Baud Rate
#define SPI1_C2			(KINETISL_SPI1.C2)		// Control Register 2
#define SPI1_C1			(KINETISL_SPI1.C1)		// Control Register 1
#define SPI1_ML			(KINETISL_SPI1.ML)		// Match Low
#define SPI1_MH			(KINETISL_SPI1.MH)		// Match High
#define SPI1_DL			(KINETISL_SPI1.DL)		// Data Low
#define SPI1_DH			(KINETISL_SPI1.DH)		// Data High
#define SPI1_CI			(KINETISL_SPI1.CI)		// Dlear Interrupt
#define SPI1_C3			(KINETISL_SPI1.C3)		// Control Register 3
#endif


// Inter-Integrated Circuit (I2C)

typedef struct {
	volatile uint8_t	A1;
	volatile uint8_t	F;
	volatile uint8_t	C1;
	volatile uint8_t	S;
	volatile uint8_t	D;
	volatile uint8_t	C2;
	volatile uint8_t	FLT;
	volatile uint8_t	RA;
	volatile uint8_t	SMB;
	volatile uint8_t	A2;
	volatile uint8_t	SLTH;
	volatile uint8_t	SLTL;
} KINETIS_I2C_t;
#define KINETIS_I2C0		(*(KINETIS_I2C_t *)0x40066000)
#define KINETIS_I2C1		(*(KINETIS_I2C_t *)0x40067000)
#define KINETIS_I2C2		(*(KINETIS_I2C_t *)0x400E6000)
#define KINETIS_I2C3		(*(KINETIS_I2C_t *)0x400E7000)
#define I2C0_A1			(KINETIS_I2C0.A1)		// I2C Address Register 1
#define I2C0_F			(KINETIS_I2C0.F)		// I2C Frequency Divider register
#define I2C_F_DIV20			((uint8_t)0x00)
#define I2C_F_DIV22			((uint8_t)0x01)
#define I2C_F_DIV24			((uint8_t)0x02)
#define I2C_F_DIV26			((uint8_t)0x03)
#define I2C_F_DIV28			((uint8_t)0x04)
#define I2C_F_DIV30			((uint8_t)0x05)
#define I2C_F_DIV32			((uint8_t)0x09)
#define I2C_F_DIV34			((uint8_t)0x06)
#define I2C_F_DIV36			((uint8_t)0x0A)
#define I2C_F_DIV40			((uint8_t)0x07)
#define I2C_F_DIV44			((uint8_t)0x0C)
#define I2C_F_DIV48			((uint8_t)0x0D)
#define I2C_F_DIV56			((uint8_t)0x0E)
#define I2C_F_DIV64			((uint8_t)0x12)
#define I2C_F_DIV68			((uint8_t)0x0F)
#define I2C_F_DIV72			((uint8_t)0x13)
#define I2C_F_DIV80			((uint8_t)0x14)
#define I2C_F_DIV88			((uint8_t)0x15)
#define I2C_F_DIV96			((uint8_t)0x19)
#define I2C_F_DIV104			((uint8_t)0x16)
#define I2C_F_DIV112			((uint8_t)0x1A)
#define I2C_F_DIV128			((uint8_t)0x17)
#define I2C_F_DIV144			((uint8_t)0x1C)
#define I2C_F_DIV160			((uint8_t)0x1D)
#define I2C_F_DIV192			((uint8_t)0x1E)
#define I2C_F_DIV224			((uint8_t)0x22)
#define I2C_F_DIV240			((uint8_t)0x1F)
#define I2C_F_DIV256			((uint8_t)0x23)
#define I2C_F_DIV288			((uint8_t)0x24)
#define I2C_F_DIV320			((uint8_t)0x25)
#define I2C_F_DIV384			((uint8_t)0x26)
#define I2C_F_DIV480			((uint8_t)0x27)
#define I2C_F_DIV448			((uint8_t)0x2A)
#define I2C_F_DIV512			((uint8_t)0x2B)
#define I2C_F_DIV576			((uint8_t)0x2C)
#define I2C_F_DIV640			((uint8_t)0x2D)
#define I2C_F_DIV768			((uint8_t)0x2E)
#define I2C_F_DIV896			((uint8_t)0x32)
#define I2C_F_DIV960			((uint8_t)0x2F)
#define I2C_F_DIV1024			((uint8_t)0x33)
#define I2C_F_DIV1152			((uint8_t)0x34)
#define I2C_F_DIV1280			((uint8_t)0x35)
#define I2C_F_DIV1536			((uint8_t)0x36)
#define I2C_F_DIV1920			((uint8_t)0x37)
#define I2C_F_DIV1792			((uint8_t)0x3A)
#define I2C_F_DIV2048			((uint8_t)0x3B)
#define I2C_F_DIV2304			((uint8_t)0x3C)
#define I2C_F_DIV2560			((uint8_t)0x3D)
#define I2C_F_DIV3072			((uint8_t)0x3E)
#define I2C_F_DIV3840			((uint8_t)0x3F)
//#define I2C_F_DIV28			((uint8_t)0x08)
//#define I2C_F_DIV40			((uint8_t)0x0B)
//#define I2C_F_DIV48			((uint8_t)0x10)
//#define I2C_F_DIV56			((uint8_t)0x11)
//#define I2C_F_DIV80			((uint8_t)0x18)
//#define I2C_F_DIV128			((uint8_t)0x1B)
//#define I2C_F_DIV160			((uint8_t)0x20)
//#define I2C_F_DIV192			((uint8_t)0x21)
//#define I2C_F_DIV320			((uint8_t)0x28)
//#define I2C_F_DIV384			((uint8_t)0x29)
//#define I2C_F_DIV640			((uint8_t)0x30)
//#define I2C_F_DIV768			((uint8_t)0x31)
//#define I2C_F_DIV1280			((uint8_t)0x38)
//#define I2C_F_DIV1536			((uint8_t)0x39)
#define I2C0_C1			(KINETIS_I2C0.C1)		// I2C Control Register 1
#define I2C_C1_IICEN			((uint8_t)0x80)			// I2C Enable
#define I2C_C1_IICIE			((uint8_t)0x40)			// I2C Interrupt Enable
#define I2C_C1_MST			((uint8_t)0x20)			// Master Mode Select
#define I2C_C1_TX			((uint8_t)0x10)			// Transmit Mode Select
#define I2C_C1_TXAK			((uint8_t)0x08)			// Transmit Acknowledge Enable
#define I2C_C1_RSTA			((uint8_t)0x04)			// Repeat START
#define I2C_C1_WUEN			((uint8_t)0x02)			// Wakeup Enable
#define I2C_C1_DMAEN			((uint8_t)0x01)			// DMA Enable
#define I2C0_S			(KINETIS_I2C0.S)		// I2C Status register
#define I2C_S_TCF			((uint8_t)0x80)			// Transfer Complete Flag
#define I2C_S_IAAS			((uint8_t)0x40)			// Addressed As A Slave
#define I2C_S_BUSY			((uint8_t)0x20)			// Bus Busy
#define I2C_S_ARBL			((uint8_t)0x10)			// Arbitration Lost
#define I2C_S_RAM			((uint8_t)0x08)			// Range Address Match
#define I2C_S_SRW			((uint8_t)0x04)			// Slave Read/Write
#define I2C_S_IICIF			((uint8_t)0x02)			// Interrupt Flag
#define I2C_S_RXAK			((uint8_t)0x01)			// Receive Acknowledge
#define I2C0_D			(KINETIS_I2C0.D)		// I2C Data I/O register
#define I2C0_C2			(KINETIS_I2C0.C2)		// I2C Control Register 2
#define I2C_C2_GCAEN			((uint8_t)0x80)			// General Call Address Enable
#define I2C_C2_ADEXT			((uint8_t)0x40)			// Address Extension
#define I2C_C2_HDRS			((uint8_t)0x20)			// High Drive Select
#define I2C_C2_SBRC			((uint8_t)0x10)			// Slave Baud Rate Control
#define I2C_C2_RMEN			((uint8_t)0x08)			// Range Address Matching Enable
#define I2C_C2_AD(n)			((n) & 7)			// Slave Address, upper 3 bits
#define I2C0_FLT		(KINETIS_I2C0.FLT)		// I2C Programmable Input Glitch Filter register
#define I2C_FLT_SHEN			((uint8_t)0x80)			// Stop Hold Enable
#define I2C_FLT_STOPF			((uint8_t)0x40)			// Stop Detect Flag
#define I2C_FLT_STOPIE			((uint8_t)0x20)			// Stop Interrupt Enable
#define I2C_FLT_FTL(n)			((n) & 0x1F)			// Programmable Filter Factor
#define I2C0_RA			(KINETIS_I2C0.RA)		// I2C Range Address register
#define I2C0_SMB		(KINETIS_I2C0.SMB)		// I2C SMBus Control and Status register
#define I2C0_A2			(KINETIS_I2C0.A2)		// I2C Address Register 2
#define I2C0_SLTH		(KINETIS_I2C0.SLTH)		// I2C SCL Low Timeout Register High
#define I2C0_SLTL		(KINETIS_I2C0.SLTL)		// I2C SCL Low Timeout Register Low

#define I2C1_A1			(KINETIS_I2C1.A1)		// I2C Address Register 1
#define I2C1_F			(KINETIS_I2C1.F)		// I2C Frequency Divider register
#define I2C1_C1			(KINETIS_I2C1.C1)		// I2C Control Register 1
#define I2C1_S			(KINETIS_I2C1.S)		// I2C Status register
#define I2C1_D			(KINETIS_I2C1.D)		// I2C Data I/O register
#define I2C1_C2			(KINETIS_I2C1.C2)		// I2C Control Register 2
#define I2C1_FLT		(KINETIS_I2C1.FLT)		// I2C Programmable Input Glitch Filter register
#define I2C1_RA			(KINETIS_I2C1.RA)		// I2C Range Address register
#define I2C1_SMB		(KINETIS_I2C1.SMB)		// I2C SMBus Control and Status register
#define I2C1_A2			(KINETIS_I2C1.A2)		// I2C Address Register 2
#define I2C1_SLTH		(KINETIS_I2C1.SLTH)		// I2C SCL Low Timeout Register High
#define I2C1_SLTL		(KINETIS_I2C1.SLTL)		// I2C SCL Low Timeout Register Low

#define I2C2_A1			(KINETIS_I2C2.A1)		// I2C Address Register 1
#define I2C2_F			(KINETIS_I2C2.F)		// I2C Frequency Divider register
#define I2C2_C1			(KINETIS_I2C2.C1)		// I2C Control Register 1
#define I2C2_S			(KINETIS_I2C2.S)		// I2C Status register
#define I2C2_D			(KINETIS_I2C2.D)		// I2C Data I/O register
#define I2C2_C2			(KINETIS_I2C2.C2)		// I2C Control Register 2
#define I2C2_FLT		(KINETIS_I2C2.FLT)		// I2C Programmable Input Glitch Filter register
#define I2C2_RA			(KINETIS_I2C2.RA)		// I2C Range Address register
#define I2C2_SMB		(KINETIS_I2C2.SMB)		// I2C SMBus Control and Status register
#define I2C2_A2			(KINETIS_I2C2.A2)		// I2C Address Register 2
#define I2C2_SLTH		(KINETIS_I2C2.SLTH)		// I2C SCL Low Timeout Register High
#define I2C2_SLTL		(KINETIS_I2C2.SLTL)		// I2C SCL Low Timeout Register Low

#define I2C3_A1			(KINETIS_I2C3.A1)		// I2C Address Register 1
#define I2C3_F			(KINETIS_I2C3.F)		// I2C Frequency Divider register
#define I2C3_C1			(KINETIS_I2C3.C1)		// I2C Control Register 1
#define I2C3_S			(KINETIS_I2C3.S)		// I2C Status register
#define I2C3_D			(KINETIS_I2C3.D)		// I2C Data I/O register
#define I2C3_C2			(KINETIS_I2C3.C2)		// I2C Control Register 2
#define I2C3_FLT		(KINETIS_I2C3.FLT)		// I2C Programmable Input Glitch Filter register
#define I2C3_RA			(KINETIS_I2C3.RA)		// I2C Range Address register
#define I2C3_SMB		(KINETIS_I2C3.SMB)		// I2C SMBus Control and Status register
#define I2C3_A2			(KINETIS_I2C3.A2)		// I2C Address Register 2
#define I2C3_SLTH		(KINETIS_I2C3.SLTH)		// I2C SCL Low Timeout Register High
#define I2C3_SLTL		(KINETIS_I2C3.SLTL)		// I2C SCL Low Timeout Register Low


// Universal Asynchronous Receiver/Transmitter (UART)

typedef struct __attribute__((packed)) {
	volatile uint8_t	BDH;
	volatile uint8_t	BDL;
	volatile uint8_t	C1;
	volatile uint8_t	C2;
	volatile uint8_t	S1;
	volatile uint8_t	S2;
	volatile uint8_t	C3;
	volatile uint8_t	D;
	volatile uint8_t	MA1;
	volatile uint8_t	MA2;
	volatile uint8_t	C4;
	volatile uint8_t	C5;
	volatile uint8_t	ED;
	volatile uint8_t	MODEM;
	volatile uint8_t	IR;
	volatile uint8_t	unused1;
	volatile uint8_t	PFIFO;
	volatile uint8_t	CFIFO;
	volatile uint8_t	SFIFO;
	volatile uint8_t	TWFIFO;
	volatile uint8_t	TCFIFO;
	volatile uint8_t	RWFIFO;
	volatile uint8_t	RCFIFO;
	volatile uint8_t	unused2;
	volatile uint8_t	C7816;
	volatile uint8_t	IE7816;
	volatile uint8_t	IS7816;
	union { volatile uint8_t WP7816T0; volatile uint8_t WP7816T1; };
	volatile uint8_t	WN7816;
	volatile uint8_t	WF7816;
	volatile uint8_t	ET7816;
	volatile uint8_t	TL7816;
	volatile uint8_t	unused3;
	volatile uint8_t	C6;
	volatile uint8_t	PCTH;
	volatile uint8_t	PCTL;
	volatile uint8_t	B1T;
	volatile uint8_t	SDTH;
	volatile uint8_t	SDTL;
	volatile uint8_t	PRE;
	volatile uint8_t	TPL;
	volatile uint8_t	IE;
	volatile uint8_t	WB;
	volatile uint8_t	S3;
	volatile uint8_t	S4;
	volatile uint8_t	RPL;
	volatile uint8_t	RPREL;
	volatile uint8_t	CPW;
	volatile uint8_t	RIDT;
	volatile uint8_t	TIDT;
} KINETISK_UART_t;
#define KINETISK_UART0		(*(KINETISK_UART_t *)0x4006A000)
#define UART0_BDH		(KINETISK_UART0.BDH)		// UART Baud Rate Registers: High
#define UART_BDH_SBNS		0x20			// UART Stop Bit Number Select (TLC T3.5 T3.6)
#define UART0_BDL		(KINETISK_UART0.BDL)		// UART Baud Rate Registers: Low
#define UART0_C1		(KINETISK_UART0.C1)		// UART Control Register 1
#define UART_C1_LOOPS		0x80			//  When LOOPS is set, the RxD pin is disconnected from the UART and the transmitter output is internally connected to the receiver input
#define UART_C1_UARTSWAI	0x40			//  UART Stops in Wait Mode
#define UART_C1_RSRC		0x20			//  When LOOPS is set, the RSRC field determines the source for the receiver shift register input
#define UART_C1_M			0x10			//  9-bit or 8-bit Mode Select
#define UART_C1_WAKE		0x08			//  Determines which condition wakes the UART
#define UART_C1_ILT		0x04			//  Idle Line Type Select
#define UART_C1_PE		0x02			//  Parity Enable
#define UART_C1_PT		0x01			//  Parity Type, 0=even, 1=odd
#define UART0_C2		(KINETISK_UART0.C2)		// UART Control Register 2
#define UART_C2_TIE		0x80			//  Transmitter Interrupt or DMA Transfer Enable.
#define UART_C2_TCIE		0x40			//  Transmission Complete Interrupt Enable
#define UART_C2_RIE		0x20			//  Receiver Full Interrupt or DMA Transfer Enable
#define UART_C2_ILIE		0x10			//  Idle Line Interrupt Enable
#define UART_C2_TE		0x08			//  Transmitter Enable
#define UART_C2_RE		0x04			//  Receiver Enable
#define UART_C2_RWU		0x02			//  Receiver Wakeup Control
#define UART_C2_SBK		0x01			//  Send Break
#define UART0_S1		(KINETISK_UART0.S1)		// UART Status Register 1
#define UART_S1_TDRE		0x80			//  Transmit Data Register Empty Flag
#define UART_S1_TC		0x40			//  Transmit Complete Flag
#define UART_S1_RDRF		0x20			//  Receive Data Register Full Flag
#define UART_S1_IDLE		0x10			//  Idle Line Flag
#define UART_S1_OR		0x08			//  Receiver Overrun Flag
#define UART_S1_NF		0x04			//  Noise Flag
#define UART_S1_FE		0x02			//  Framing Error Flag
#define UART_S1_PF		0x01			//  Parity Error Flag
#define UART0_S2		(KINETISK_UART0.S2)		// UART Status Register 2
#define UART_S2_LBKDIF		0x80			// LIN Break Detect Interrupt Flag
#define UART_S2_RXEDGIF		0x40			// RxD Pin Active Edge Interrupt Flag
#define UART_S2_MSBF		0x20			// Most Significant Bit First
#define UART_S2_RXINV		0x10			// Receive Data Inversion
#define UART_S2_RWUID		0x08			// Receive Wakeup Idle Detect
#define UART_S2_BRK13		0x04			// Break Transmit Character Length
#define UART_S2_LBKDE		0x02			// LIN Break Detection Enable
#define UART_S2_RAF		0x01			// Receiver Active Flag
#define UART0_C3		(KINETISK_UART0.C3)		// UART Control Register 3
#define UART_C3_R8		0x80			// Received Bit 8
#define UART_C3_T8		0x40			// Transmit Bit 8
#define UART_C3_TXDIR		0x20			// TX Pin Direction in Single-Wire mode
#define UART_C3_TXINV		0x10			// Transmit Data Inversion
#define UART_C3_ORIE		0x08			// Overrun Error Interrupt Enable
#define UART_C3_NEIE		0x04			// Noise Error Interrupt Enable
#define UART_C3_FEIE		0x02			// Framing Error Interrupt Enable
#define UART_C3_PEIE		0x01			// Parity Error Interrupt Enable
#define UART0_D			(KINETISK_UART0.D)		// UART Data Register
#define UART0_MA1		(KINETISK_UART0.MA1)		// UART Match Address Registers 1
#define UART0_MA2		(KINETISK_UART0.MA2)		// UART Match Address Registers 2
#define UART0_C4		(KINETISK_UART0.C4)		// UART Control Register 4
#define UART_C4_MAEN1		0x80			// Match Address Mode Enable 1
#define UART_C4_MAEN2		0x40			// Match Address Mode Enable 2
#define UART_C4_M10		0x20			// 10-bit Mode select
#define UART_C4_BRFA(n)		((n) & 31)		// Baud Rate Fine Adjust
#define UART0_C5		(KINETISK_UART0.C5)		// UART Control Register 5
#define UART_C5_TDMAS		0x80			// Transmitter DMA Select
#define UART_C5_RDMAS		0x20			// Receiver Full DMA Select
#define UART0_ED		(KINETISK_UART0.ED)		// UART Extended Data Register
#define UART_ED_NOISY		0x80			// data received with noise
#define UART_ED_PARITYE		0x40			// data received with a parity error
#define UART0_MODEM		(KINETISK_UART0.MODEM)		// UART Modem Register
#define UART_MODEM_RXRTSE	0x08			// Receiver request-to-send enable
#define UART_MODEM_TXRTSPOL	0x04			// Transmitter request-to-send polarity
#define UART_MODEM_TXRTSE	0x02			// Transmitter request-to-send enable
#define UART_MODEM_TXCTSE	0x01			// Transmitter clear-to-send enable
#define UART0_IR		(KINETISK_UART0.IR)		// UART Infrared Register
#define UART_IR_IREN		0x04			// Infrared enable
#define UART_IR_TNP(n)		((n) & 3)		// TX narrow pulse, 0=3/16, 1=1/16, 2=1/32, 3=1/4
#define UART0_PFIFO		(KINETISK_UART0.PFIFO)		// UART FIFO Parameters
#define UART_PFIFO_TXFE		0x80			//  Transmit FIFO Enable
#define UART_PFIFO_TXFIFOSIZE(n) (((n) & 7) << 4)	//  Transmit FIFO Size, 0=1, 1=4, 2=8, 3=16, 4=32, 5=64, 6=128
#define UART_PFIFO_RXFE		0x08			//  Receive FIFO Enable
#define UART_PFIFO_RXFIFOSIZE(n) (((n) & 7) << 0)	//  Transmit FIFO Size, 0=1, 1=4, 2=8, 3=16, 4=32, 5=64, 6=128
#define UART0_CFIFO		(KINETISK_UART0.CFIFO)		// UART FIFO Control Register
#define UART_CFIFO_TXFLUSH	0x80			//  Transmit FIFO/Buffer Flush
#define UART_CFIFO_RXFLUSH	0x40			//  Receive FIFO/Buffer Flush
#define UART_CFIFO_RXOFE	0x04			//  Receive FIFO Overflow Interrupt Enable
#define UART_CFIFO_TXOFE	0x02			//  Transmit FIFO Overflow Interrupt Enable
#define UART_CFIFO_RXUFE	0x01			//  Receive FIFO Underflow Interrupt Enable
#define UART0_SFIFO		(KINETISK_UART0.SFIFO)		// UART FIFO Status Register
#define UART_SFIFO_TXEMPT	0x80			//  Transmit Buffer/FIFO Empty
#define UART_SFIFO_RXEMPT	0x40			//  Receive Buffer/FIFO Empty
#define UART_SFIFO_RXOF		0x04			//  Receiver Buffer Overflow Flag
#define UART_SFIFO_TXOF		0x02			//  Transmitter Buffer Overflow Flag
#define UART_SFIFO_RXUF		0x01			//  Receiver Buffer Underflow Flag
#define UART0_TWFIFO		(KINETISK_UART0.TWFIFO)		// UART FIFO Transmit Watermark
#define UART0_TCFIFO		(KINETISK_UART0.TCFIFO)		// UART FIFO Transmit Count
#define UART0_RWFIFO		(KINETISK_UART0.RWFIFO)		// UART FIFO Receive Watermark
#define UART0_RCFIFO		(KINETISK_UART0.RCFIFO)		// UART FIFO Receive Count
#define UART0_C7816		(KINETISK_UART0.C7816)		// UART 7816 Control Register
#define UART_C7816_ONACK	0x10			//  Generate NACK on Overflow
#define UART_C7816_ANACK	0x08			//  Generate NACK on Error
#define UART_C7816_INIT		0x04			//  Detect Initial Character
#define UART_C7816_TTYPE	0x02			//  Transfer Type
#define UART_C7816_ISO_7816E	0x01			//  ISO-7816 Functionality Enabled
#define UART0_IE7816		(KINETISK_UART0.IE7816)		// UART 7816 Interrupt Enable Register
#define UART_IE7816_WTE		0x80			//  Wait Timer Interrupt Enable
#define UART_IE7816_CWTE	0x40			//  Character Wait Timer Interrupt Enable
#define UART_IE7816_BWTE	0x20			//  Block Wait Timer Interrupt Enable
#define UART_IE7816_INITDE	0x10			//  Initial Character Detected Interrupt Enable
#define UART_IE7816_GTVE	0x04			//  Guard Timer Violated Interrupt Enable
#define UART_IE7816_TXTE	0x02			//  Transmit Threshold Exceeded Interrupt Enable
#define UART_IE7816_RXTE	0x01			//  Receive Threshold Exceeded Interrupt Enable
#define UART0_IS7816		(KINETISK_UART0.IS7816)		// UART 7816 Interrupt Status Register
#define UART_IS7816_WT		0x80			//  Wait Timer Interrupt
#define UART_IS7816_CWT		0x40			//  Character Wait Timer Interrupt
#define UART_IS7816_BWT		0x20			//  Block Wait Timer Interrupt
#define UART_IS7816_INITD	0x10			//  Initial Character Detected Interrupt
#define UART_IS7816_GTV		0x04			//  Guard Timer Violated Interrupt
#define UART_IS7816_TXT		0x02			//  Transmit Threshold Exceeded Interrupt
#define UART_IS7816_RXT		0x01			//  Receive Threshold Exceeded Interrupt
#define UART0_WP7816T0		(KINETISK_UART0.WP7816T0)	// UART 7816 Wait Parameter Register
#define UART0_WP7816T1		(KINETISK_UART0.WP7816T1)	// UART 7816 Wait Parameter Register
#define UART_WP7816T1_CWI(n)	(((n) & 15) << 4)	//  Character Wait Time Integer (C7816[TTYPE] = 1)
#define UART_WP7816T1_BWI(n)	(((n) & 15) << 0)	//  Block Wait Time Integer(C7816[TTYPE] = 1)
#define UART0_WN7816		(KINETISK_UART0.WN7816)		// UART 7816 Wait N Register
#define UART0_WF7816		(KINETISK_UART0.WF7816)		// UART 7816 Wait FD Register
#define UART0_ET7816		(KINETISK_UART0.ET7816)		// UART 7816 Error Threshold Register
#define UART_ET7816_TXTHRESHOLD(n) (((n) & 15) << 4)	//  Transmit NACK Threshold
#define UART_ET7816_RXTHRESHOLD(n) (((n) & 15) << 0)	//  Receive NACK Threshold
#define UART0_TL7816		(KINETISK_UART0.TL7816)		// UART 7816 Transmit Length Register
#define UART0_C6		(KINETISK_UART0.C6)		// UART CEA709.1-B Control Register 6
#define UART_C6_EN709		0x80			//  Enables the CEA709.1-B feature.
#define UART_C6_TX709		0x40			//  Starts CEA709.1-B transmission.
#define UART_C6_CE		0x20			//  Collision Enable
#define UART_C6_CP		0x10			//  Collision Signal Polarity
#define UART0_PCTH		(KINETISK_UART0.PCTH)		// UART CEA709.1-B Packet Cycle Time Counter High
#define UART0_PCTL		(KINETISK_UART0.PCTL)		// UART CEA709.1-B Packet Cycle Time Counter Low
#define UART0_B1T		(KINETISK_UART0.B1T)		// UART CEA709.1-B Beta1 Timer
#define UART0_SDTH		(KINETISK_UART0.SDTH)		// UART CEA709.1-B Secondary Delay Timer High
#define UART0_SDTL		(KINETISK_UART0.SDTL)		// UART CEA709.1-B Secondary Delay Timer Low
#define UART0_PRE		(KINETISK_UART0.PRE)		// UART CEA709.1-B Preamble
#define UART0_TPL		(KINETISK_UART0.TPL)		// UART CEA709.1-B Transmit Packet Length
#define UART0_IE		(KINETISK_UART0.IE)		// UART CEA709.1-B Interrupt Enable Register
#define UART_IE_WBEIE		0x40			//  WBASE Expired Interrupt Enable
#define UART_IE_ISDIE		0x20			//  Initial Sync Detection Interrupt Enable
#define UART_IE_PRXIE		0x10			//  Packet Received Interrupt Enable
#define UART_IE_PTXIE		0x08			//  Packet Transmitted Interrupt Enable
#define UART_IE_PCTEIE		0x04			//  Packet Cycle Timer Interrupt Enable
#define UART_IE_PSIE		0x02			//  Preamble Start Interrupt Enable
#define UART_IE_TXFIE		0x01			//  Transmission Fail Interrupt Enable
#define UART0_WB		(KINETISK_UART0.WB)		// UART CEA709.1-B WBASE
#define UART0_S3		(KINETISK_UART0.S3)		// UART CEA709.1-B Status Register
#define UART_S3_PEF		0x80			//  Preamble Error Flag
#define UART_S3_WBEF		0x40			//  Wbase Expired Flag
#define UART_S3_ISD		0x20			//  Initial Sync Detect
#define UART_S3_PRXF		0x10			//  Packet Received Flag
#define UART_S3_PTXF		0x08			//  Packet Transmitted Flag
#define UART_S3_PCTEF		0x04			//  Packet Cycle Timer Expired Flag
#define UART_S3_PSF		0x02			//  Preamble Start Flag
#define UART_S3_TXFF		0x01			//  Transmission Fail Flag
#define UART0_S4		(KINETISK_UART0.S4)		// UART CEA709.1-B Status Register
#define UART_S4_INITF		0x10			//  Initial Synchronization Fail Flag
#define UART_S4_CDET(n)		(((n) & 3) << 2)	//  Indicates collision: 0=none, 1=preamble, 2=data, 3=line code violation
#define UART_S4_ILCV		0x02			//  Improper Line Code Violation
#define UART_S4_FE		0x01			//  Framing Error
#define UART0_RPL		(KINETISK_UART0.RPL)	// UART CEA709.1-B Received Packet Length
#define UART0_RPREL		(KINETISK_UART0.RPREL)	// UART CEA709.1-B Received Preamble Length
#define UART0_CPW		(KINETISK_UART0.CPW)	// UART CEA709.1-B Collision Pulse Width
#define UART0_RIDT		(KINETISK_UART0.RIDT)	// UART CEA709.1-B Receive Indeterminate Time
#define UART0_TIDT		(KINETISK_UART0.TIDT)	// UART CEA709.1-B Transmit Indeterminate Time
#define KINETISK_UART1		(*(KINETISK_UART_t *)0x4006B000)
#define UART1_BDH		(KINETISK_UART1.BDH)	// UART Baud Rate Registers: High
#define UART1_BDL		(KINETISK_UART1.BDL)	// UART Baud Rate Registers: Low
#define UART1_C1		(KINETISK_UART1.C1)	// UART Control Register 1
#define UART1_C2		(KINETISK_UART1.C2)	// UART Control Register 2
#define UART1_S1		(KINETISK_UART1.S1)	// UART Status Register 1
#define UART1_S2		(KINETISK_UART1.S2)	// UART Status Register 2
#define UART1_C3		(KINETISK_UART1.C3)	// UART Control Register 3
#define UART1_D			(KINETISK_UART1.D)	// UART Data Register
#define UART1_MA1		(KINETISK_UART1.MA1)	// UART Match Address Registers 1
#define UART1_MA2		(KINETISK_UART1.MA2)	// UART Match Address Registers 2
#define UART1_C4		(KINETISK_UART1.C4)	// UART Control Register 4
#define UART1_C5		(KINETISK_UART1.C5)	// UART Control Register 5
#define UART1_ED		(KINETISK_UART1.ED)	// UART Extended Data Register
#define UART1_MODEM		(KINETISK_UART1.MODEM)	// UART Modem Register
#define UART1_IR		(KINETISK_UART1.IR)	// UART Infrared Register
#define UART1_PFIFO		(KINETISK_UART1.PFIFO)	// UART FIFO Parameters
#define UART1_CFIFO		(KINETISK_UART1.CFIFO)	// UART FIFO Control Register
#define UART1_SFIFO		(KINETISK_UART1.SFIFO)	// UART FIFO Status Register
#define UART1_TWFIFO		(KINETISK_UART1.TWFIFO)	// UART FIFO Transmit Watermark
#define UART1_TCFIFO		(KINETISK_UART1.TCFIFO)	// UART FIFO Transmit Count
#define UART1_RWFIFO		(KINETISK_UART1.RWFIFO)	// UART FIFO Receive Watermark
#define UART1_RCFIFO		(KINETISK_UART1.RCFIFO)	// UART FIFO Receive Count
#define UART1_C7816		(KINETISK_UART1.C7816)	// UART 7816 Control Register
#define UART1_IE7816		(KINETISK_UART1.IE7816)	// UART 7816 Interrupt Enable Register
#define UART1_IS7816		(KINETISK_UART1.IS7816)	// UART 7816 Interrupt Status Register
#define UART1_WP7816T0		(KINETISK_UART1.WP7816T0)// UART 7816 Wait Parameter Register
#define UART1_WP7816T1		(KINETISK_UART1.WP7816T1)// UART 7816 Wait Parameter Register
#define UART1_WN7816		(KINETISK_UART1.WN7816)	// UART 7816 Wait N Register
#define UART1_WF7816		(KINETISK_UART1.WF7816)	// UART 7816 Wait FD Register
#define UART1_ET7816		(KINETISK_UART1.ET7816)	// UART 7816 Error Threshold Register
#define UART1_TL7816		(KINETISK_UART1.TL7816)	// UART 7816 Transmit Length Register
#define UART1_C6		(KINETISK_UART1.C6)	// UART CEA709.1-B Control Register 6
#define UART1_PCTH		(KINETISK_UART1.PCTH)	// UART CEA709.1-B Packet Cycle Time Counter High
#define UART1_PCTL		(KINETISK_UART1.PCTL)	// UART CEA709.1-B Packet Cycle Time Counter Low
#define UART1_B1T		(KINETISK_UART1.B1T)	// UART CEA709.1-B Beta1 Timer
#define UART1_SDTH		(KINETISK_UART1.SDTH)	// UART CEA709.1-B Secondary Delay Timer High
#define UART1_SDTL		(KINETISK_UART1.SDTL)	// UART CEA709.1-B Secondary Delay Timer Low
#define UART1_PRE		(KINETISK_UART1.PRE)	// UART CEA709.1-B Preamble
#define UART1_TPL		(KINETISK_UART1.TPL)	// UART CEA709.1-B Transmit Packet Length
#define UART1_IE		(KINETISK_UART1.IE)	// UART CEA709.1-B Interrupt Enable Register
#define UART1_WB		(KINETISK_UART1.WB)	// UART CEA709.1-B WBASE
#define UART1_S3		(KINETISK_UART1.S3)	// UART CEA709.1-B Status Register
#define UART1_S4		(KINETISK_UART1.S4)	// UART CEA709.1-B Status Register
#define UART1_RPL		(KINETISK_UART1.RPL)	// UART CEA709.1-B Received Packet Length
#define UART1_RPREL		(KINETISK_UART1.RPREL)	// UART CEA709.1-B Received Preamble Length
#define UART1_CPW		(KINETISK_UART1.CPW)	// UART CEA709.1-B Collision Pulse Width
#define UART1_RIDT		(KINETISK_UART1.RIDT)	// UART CEA709.1-B Receive Indeterminate Time
#define UART1_TIDT		(KINETISK_UART1.TIDT)	// UART CEA709.1-B Transmit Indeterminate Time
#define KINETISK_UART2		(*(KINETISK_UART_t *)0x4006C000)
#define UART2_BDH		(KINETISK_UART2.BDH)	// UART Baud Rate Registers: High
#define UART2_BDL		(KINETISK_UART2.BDL)	// UART Baud Rate Registers: Low
#define UART2_C1		(KINETISK_UART2.C1)	// UART Control Register 1
#define UART2_C2		(KINETISK_UART2.C2)	// UART Control Register 2
#define UART2_S1		(KINETISK_UART2.S1)	// UART Status Register 1
#define UART2_S2		(KINETISK_UART2.S2)	// UART Status Register 2
#define UART2_C3		(KINETISK_UART2.C3)	// UART Control Register 3
#define UART2_D			(KINETISK_UART2.D)	// UART Data Register
#define UART2_MA1		(KINETISK_UART2.MA1)	// UART Match Address Registers 1
#define UART2_MA2		(KINETISK_UART2.MA2)	// UART Match Address Registers 2
#define UART2_C4		(KINETISK_UART2.C4)	// UART Control Register 4
#define UART2_C5		(KINETISK_UART2.C5)	// UART Control Register 5
#define UART2_ED		(KINETISK_UART2.ED)	// UART Extended Data Register
#define UART2_MODEM		(KINETISK_UART2.MODEM)	// UART Modem Register
#define UART2_IR		(KINETISK_UART2.IR)	// UART Infrared Register
#define UART2_PFIFO		(KINETISK_UART2.PFIFO)	// UART FIFO Parameters
#define UART2_CFIFO		(KINETISK_UART2.CFIFO)	// UART FIFO Control Register
#define UART2_SFIFO		(KINETISK_UART2.SFIFO)	// UART FIFO Status Register
#define UART2_TWFIFO		(KINETISK_UART2.TWFIFO)	// UART FIFO Transmit Watermark
#define UART2_TCFIFO		(KINETISK_UART2.TCFIFO)	// UART FIFO Transmit Count
#define UART2_RWFIFO		(KINETISK_UART2.RWFIFO)	// UART FIFO Receive Watermark
#define UART2_RCFIFO		(KINETISK_UART2.RCFIFO)	// UART FIFO Receive Count
#define UART2_C7816		(KINETISK_UART2.C7816)	// UART 7816 Control Register
#define UART2_IE7816		(KINETISK_UART2.IE7816)	// UART 7816 Interrupt Enable Register
#define UART2_IS7816		(KINETISK_UART2.IS7816)	// UART 7816 Interrupt Status Register
#define UART2_WP7816T0		(KINETISK_UART2.WP7816T0)// UART 7816 Wait Parameter Register
#define UART2_WP7816T1		(KINETISK_UART2.WP7816T1)// UART 7816 Wait Parameter Register
#define UART2_WN7816		(KINETISK_UART2.WN7816)	// UART 7816 Wait N Register
#define UART2_WF7816		(KINETISK_UART2.WF7816)	// UART 7816 Wait FD Register
#define UART2_ET7816		(KINETISK_UART2.ET7816)	// UART 7816 Error Threshold Register
#define UART2_TL7816		(KINETISK_UART2.TL7816)	// UART 7816 Transmit Length Register
#define UART2_C6		(KINETISK_UART2.C6)	// UART CEA709.1-B Control Register 6
#define UART2_PCTH		(KINETISK_UART2.PCTH)	// UART CEA709.1-B Packet Cycle Time Counter High
#define UART2_PCTL		(KINETISK_UART2.PCTL)	// UART CEA709.1-B Packet Cycle Time Counter Low
#define UART2_B1T		(KINETISK_UART2.B1T)	// UART CEA709.1-B Beta1 Timer
#define UART2_SDTH		(KINETISK_UART2.SDTH)	// UART CEA709.1-B Secondary Delay Timer High
#define UART2_SDTL		(KINETISK_UART2.SDTL)	// UART CEA709.1-B Secondary Delay Timer Low
#define UART2_PRE		(KINETISK_UART2.PRE)	// UART CEA709.1-B Preamble
#define UART2_TPL		(KINETISK_UART2.TPL)	// UART CEA709.1-B Transmit Packet Length
#define UART2_IE		(KINETISK_UART2.IE)	// UART CEA709.1-B Interrupt Enable Register
#define UART2_WB		(KINETISK_UART2.WB)	// UART CEA709.1-B WBASE
#define UART2_S3		(KINETISK_UART2.S3)	// UART CEA709.1-B Status Register
#define UART2_S4		(KINETISK_UART2.S4)	// UART CEA709.1-B Status Register
#define UART2_RPL		(KINETISK_UART2.RPL)	// UART CEA709.1-B Received Packet Length
#define UART2_RPREL		(KINETISK_UART2.RPREL)	// UART CEA709.1-B Received Preamble Length
#define UART2_CPW		(KINETISK_UART2.CPW)	// UART CEA709.1-B Collision Pulse Width
#define UART2_RIDT		(KINETISK_UART2.RIDT)	// UART CEA709.1-B Receive Indeterminate Time
#define UART2_TIDT		(KINETISK_UART2.TIDT)	// UART CEA709.1-B Transmit Indeterminate Time

#define KINETISK_UART3		(*(KINETISK_UART_t *)0x4006D000)
#define UART3_BDH		(KINETISK_UART3.BDH)	// UART Baud Rate Registers: High
#define UART3_BDL		(KINETISK_UART3.BDL)	// UART Baud Rate Registers: Low
#define UART3_C1		(KINETISK_UART3.C1)	// UART Control Register 1
#define UART3_C2		(KINETISK_UART3.C2)	// UART Control Register 2
#define UART3_S1		(KINETISK_UART3.S1)	// UART Status Register 1
#define UART3_S2		(KINETISK_UART3.S2)	// UART Status Register 2
#define UART3_C3		(KINETISK_UART3.C3)	// UART Control Register 3
#define UART3_D			(KINETISK_UART3.D)	// UART Data Register
#define UART3_MA1		(KINETISK_UART3.MA1)	// UART Match Address Registers 1
#define UART3_MA2		(KINETISK_UART3.MA2)	// UART Match Address Registers 2
#define UART3_C4		(KINETISK_UART3.C4)	// UART Control Register 4
#define UART3_C5		(KINETISK_UART3.C5)	// UART Control Register 5
#define UART3_ED		(KINETISK_UART3.ED)	// UART Extended Data Register
#define UART3_MODEM		(KINETISK_UART3.MODEM)	// UART Modem Register
#define UART3_IR		(KINETISK_UART3.IR)	// UART Infrared Register
#define UART3_PFIFO		(KINETISK_UART3.PFIFO)	// UART FIFO Parameters
#define UART3_CFIFO		(KINETISK_UART3.CFIFO)	// UART FIFO Control Register
#define UART3_SFIFO		(KINETISK_UART3.SFIFO)	// UART FIFO Status Register
#define UART3_TWFIFO		(KINETISK_UART3.TWFIFO)	// UART FIFO Transmit Watermark
#define UART3_TCFIFO		(KINETISK_UART3.TCFIFO)	// UART FIFO Transmit Count
#define UART3_RWFIFO		(KINETISK_UART3.RWFIFO)	// UART FIFO Receive Watermark
#define UART3_RCFIFO		(KINETISK_UART3.RCFIFO)	// UART FIFO Receive Count
#define UART3_C7816		(KINETISK_UART3.C7816)	// UART 7816 Control Register
#define UART3_IE7816		(KINETISK_UART3.IE7816)	// UART 7816 Interrupt Enable Register
#define UART3_IS7816		(KINETISK_UART3.IS7816)	// UART 7816 Interrupt Status Register
#define UART3_WP7816T0		(KINETISK_UART3.WP7816T0)// UART 7816 Wait Parameter Register
#define UART3_WP7816T1		(KINETISK_UART3.WP7816T1)// UART 7816 Wait Parameter Register
#define UART3_WN7816		(KINETISK_UART3.WN7816)	// UART 7816 Wait N Register
#define UART3_WF7816		(KINETISK_UART3.WF7816)	// UART 7816 Wait FD Register
#define UART3_ET7816		(KINETISK_UART3.ET7816)	// UART 7816 Error Threshold Register
#define UART3_TL7816		(KINETISK_UART3.TL7816)	// UART 7816 Transmit Length Register

#define KINETISK_UART4		(*(KINETISK_UART_t *)0x400EA000)
#define UART4_BDH		(KINETISK_UART4.BDH)	// UART Baud Rate Registers: High
#define UART4_BDL		(KINETISK_UART4.BDL)	// UART Baud Rate Registers: Low
#define UART4_C1		(KINETISK_UART4.C1)	// UART Control Register 1
#define UART4_C2		(KINETISK_UART4.C2)	// UART Control Register 2
#define UART4_S1		(KINETISK_UART4.S1)	// UART Status Register 1
#define UART4_S2		(KINETISK_UART4.S2)	// UART Status Register 2
#define UART4_C3		(KINETISK_UART4.C3)	// UART Control Register 3
#define UART4_D			(KINETISK_UART4.D)	// UART Data Register
#define UART4_MA1		(KINETISK_UART4.MA1)	// UART Match Address Registers 1
#define UART4_MA2		(KINETISK_UART4.MA2)	// UART Match Address Registers 2
#define UART4_C4		(KINETISK_UART4.C4)	// UART Control Register 4
#define UART4_C5		(KINETISK_UART4.C5)	// UART Control Register 5
#define UART4_ED		(KINETISK_UART4.ED)	// UART Extended Data Register
#define UART4_MODEM		(KINETISK_UART4.MODEM)	// UART Modem Register
#define UART4_IR		(KINETISK_UART4.IR)	// UART Infrared Register
#define UART4_PFIFO		(KINETISK_UART4.PFIFO)	// UART FIFO Parameters
#define UART4_CFIFO		(KINETISK_UART4.CFIFO)	// UART FIFO Control Register
#define UART4_SFIFO		(KINETISK_UART4.SFIFO)	// UART FIFO Status Register
#define UART4_TWFIFO		(KINETISK_UART4.TWFIFO)	// UART FIFO Transmit Watermark
#define UART4_TCFIFO		(KINETISK_UART4.TCFIFO)	// UART FIFO Transmit Count
#define UART4_RWFIFO		(KINETISK_UART4.RWFIFO)	// UART FIFO Receive Watermark
#define UART4_RCFIFO		(KINETISK_UART4.RCFIFO)	// UART FIFO Receive Count
#define UART4_C7816		(KINETISK_UART4.C7816)	// UART 7816 Control Register
#define UART4_IE7816		(KINETISK_UART4.IE7816)	// UART 7816 Interrupt Enable Register
#define UART4_IS7816		(KINETISK_UART4.IS7816)	// UART 7816 Interrupt Status Register
#define UART4_WP7816T0		(KINETISK_UART4.WP7816T0)// UART 7816 Wait Parameter Register
#define UART4_WP7816T1		(KINETISK_UART4.WP7816T1)// UART 7816 Wait Parameter Register
#define UART4_WN7816		(KINETISK_UART4.WN7816)	// UART 7816 Wait N Register
#define UART4_WF7816		(KINETISK_UART4.WF7816)	// UART 7816 Wait FD Register
#define UART4_ET7816		(KINETISK_UART4.ET7816)	// UART 7816 Error Threshold Register
#define UART4_TL7816		(KINETISK_UART4.TL7816)	// UART 7816 Transmit Length Register

#define KINETISK_UART5		(*(KINETISK_UART_t *)0x400EB000)
#define UART5_BDH		(KINETISK_UART5.BDH)	// UART Baud Rate Registers: High
#define UART5_BDL		(KINETISK_UART5.BDL)	// UART Baud Rate Registers: Low
#define UART5_C1		(KINETISK_UART5.C1)	// UART Control Register 1
#define UART5_C2		(KINETISK_UART5.C2)	// UART Control Register 2
#define UART5_S1		(KINETISK_UART5.S1)	// UART Status Register 1
#define UART5_S2		(KINETISK_UART5.S2)	// UART Status Register 2
#define UART5_C3		(KINETISK_UART5.C3)	// UART Control Register 3
#define UART5_D			(KINETISK_UART5.D)	// UART Data Register
#define UART5_MA1		(KINETISK_UART5.MA1)	// UART Match Address Registers 1
#define UART5_MA2		(KINETISK_UART5.MA2)	// UART Match Address Registers 2
#define UART5_C4		(KINETISK_UART5.C4)	// UART Control Register 4
#define UART5_C5		(KINETISK_UART5.C5)	// UART Control Register 5
#define UART5_ED		(KINETISK_UART5.ED)	// UART Extended Data Register
#define UART5_MODEM		(KINETISK_UART5.MODEM)	// UART Modem Register
#define UART5_IR		(KINETISK_UART5.IR)	// UART Infrared Register
#define UART5_PFIFO		(KINETISK_UART5.PFIFO)	// UART FIFO Parameters
#define UART5_CFIFO		(KINETISK_UART5.CFIFO)	// UART FIFO Control Register
#define UART5_SFIFO		(KINETISK_UART5.SFIFO)	// UART FIFO Status Register
#define UART5_TWFIFO		(KINETISK_UART5.TWFIFO)	// UART FIFO Transmit Watermark
#define UART5_TCFIFO		(KINETISK_UART5.TCFIFO)	// UART FIFO Transmit Count
#define UART5_RWFIFO		(KINETISK_UART5.RWFIFO)	// UART FIFO Receive Watermark
#define UART5_RCFIFO		(KINETISK_UART5.RCFIFO)	// UART FIFO Receive Count
#define UART5_C7816		(KINETISK_UART5.C7816)	// UART 7816 Control Register
#define UART5_IE7816		(KINETISK_UART5.IE7816)	// UART 7816 Interrupt Enable Register
#define UART5_IS7816		(KINETISK_UART5.IS7816)	// UART 7816 Interrupt Status Register
#define UART5_WP7816T0		(KINETISK_UART5.WP7816T0)// UART 7816 Wait Parameter Register
#define UART5_WP7816T1		(KINETISK_UART5.WP7816T1)// UART 7816 Wait Parameter Register
#define UART5_WN7816		(KINETISK_UART5.WN7816)	// UART 7816 Wait N Register
#define UART5_WF7816		(KINETISK_UART5.WF7816)	// UART 7816 Wait FD Register
#define UART5_ET7816		(KINETISK_UART5.ET7816)	// UART 7816 Error Threshold Register
#define UART5_TL7816		(KINETISK_UART5.TL7816)	// UART 7816 Transmit Length Register


// Secured digital host controller (SDHC)

#define SDHC_DSADDR		(*(volatile uint32_t *)0x400B1000) // DMA System Address register
#define SDHC_BLKATTR		(*(volatile uint32_t *)0x400B1004) // Block Attributes register
#define SDHC_BLKATTR_BLKCNT(n)		(uint32_t)(((n) & 0xFFFF)<<16)	// Blocks Count For Current Transfer
#define SDHC_BLKATTR_BLKCNT_MASK	((uint32_t)0xFFFF0000)
#define SDHC_BLKATTR_BLKSIZE(n)		(uint32_t)(((n) & 0x1FFF)<<0)	// Transfer Block Size
#define SDHC_CMDARG		(*(volatile uint32_t *)0x400B1008) // Command Argument register
#define SDHC_XFERTYP		(*(volatile uint32_t *)0x400B100C) // Transfer Type register
#define SDHC_XFERTYP_CMDINX(n)		(uint32_t)(((n) & 0x3F)<<24)	// Command Index
#define SDHC_XFERTYP_CMDTYP(n)		(uint32_t)(((n) & 0x3)<<22)	// Command Type
#define SDHC_XFERTYP_DPSEL		((uint32_t)0x00200000)		// Data Present Select
#define SDHC_XFERTYP_CICEN		((uint32_t)0x00100000)		// Command Index Check Enable
#define SDHC_XFERTYP_CCCEN		((uint32_t)0x00080000)		// Command CRC Check Enable
#define SDHC_XFERTYP_RSPTYP(n)		(uint32_t)(((n) & 0x3)<<16)	// Response Type Select
#define SDHC_XFERTYP_MSBSEL		((uint32_t)0x00000020)		// Multi/Single Block Select
#define SDHC_XFERTYP_DTDSEL		((uint32_t)0x00000010)		// Data Transfer Direction Select
#define SDHC_XFERTYP_AC12EN		((uint32_t)0x00000004)		// Auto CMD12 Enable
#define SDHC_XFERTYP_BCEN		((uint32_t)0x00000002)		// Block Count Enable
#define SDHC_XFERTYP_DMAEN		((uint32_t)0x00000001)		// DMA Enable
#define SDHC_CMDRSP0		(*(volatile uint32_t *)0x400B1010) // Command Response 0
#define SDHC_CMDRSP1		(*(volatile uint32_t *)0x400B1014) // Command Response 1
#define SDHC_CMDRSP2		(*(volatile uint32_t *)0x400B1018) // Command Response 2
#define SDHC_CMDRSP3		(*(volatile uint32_t *)0x400B101C) // Command Response 3
#define SDHC_DATPORT		(*(volatile uint32_t *)0x400B1020) // Buffer Data Port register
#define SDHC_PRSSTAT		(*(volatile uint32_t *)0x400B1024) // Present State register
#define SDHC_PRSSTAT_DLSL_MASK		((uint32_t)0xFF000000)		// DAT Line Signal Level
#define SDHC_PRSSTAT_CLSL		((uint32_t)0x00800000)		// CMD Line Signal Level
#define SDHC_PRSSTAT_CINS		((uint32_t)0x00010000)		// Card Inserted
#define SDHC_PRSSTAT_BREN		((uint32_t)0x00000800)		// Buffer Read Enable
#define SDHC_PRSSTAT_BWEN		((uint32_t)0x00000400)		// Buffer Write Enable
#define SDHC_PRSSTAT_RTA		((uint32_t)0x00000200)		// Read Transfer Active
#define SDHC_PRSSTAT_WTA		((uint32_t)0x00000100)		// Write Transfer Active
#define SDHC_PRSSTAT_SDOFF		((uint32_t)0x00000080)		// SD Clock Gated Off Internally
#define SDHC_PRSSTAT_PEROFF		((uint32_t)0x00000040)		// SDHC clock Gated Off Internally
#define SDHC_PRSSTAT_HCKOFF		((uint32_t)0x00000020)		// System Clock Gated Off Internally
#define SDHC_PRSSTAT_IPGOFF		((uint32_t)0x00000010)		// Bus Clock Gated Off Internally
#define SDHC_PRSSTAT_SDSTB		((uint32_t)0x00000008)		// SD Clock Stable
#define SDHC_PRSSTAT_DLA		((uint32_t)0x00000004)		// Data Line Active
#define SDHC_PRSSTAT_CDIHB		((uint32_t)0x00000002)		// Command Inhibit (DAT)
#define SDHC_PRSSTAT_CIHB		((uint32_t)0x00000001)		// Command Inhibit (CMD)
#define SDHC_PROCTL		(*(volatile uint32_t *)0x400B1028) // Protocol Control register
#define SDHC_PROCTL_WECRM		((uint32_t)0x04000000)		// Wakeup Event Enable On SD Card Removal
#define SDHC_PROCTL_WECINS		((uint32_t)0x02000000)		// Wakeup Event Enable On SD Card Insertion
#define SDHC_PROCTL_WECINT		((uint32_t)0x01000000)		// Wakeup Event Enable On Card Interrupt
#define SDHC_PROCTL_IABG		((uint32_t)0x00080000)		// Interrupt At Block Gap
#define SDHC_PROCTL_RWCTL		((uint32_t)0x00040000)		// Read Wait Control
#define SDHC_PROCTL_CREQ		((uint32_t)0x00020000)		// Continue Request
#define SDHC_PROCTL_SABGREQ		((uint32_t)0x00010000)		// Stop At Block Gap Request
#define SDHC_PROCTL_DMAS(n)		(uint32_t)(((n) & 0x3)<<8)	// DMA Select
#define SDHC_PROCTL_CDSS		((uint32_t)0x00000080)		// Card Detect Signal Selection
#define SDHC_PROCTL_CDTL		((uint32_t)0x00000040)		// Card Detect Test Level
#define SDHC_PROCTL_EMODE(n)		(uint32_t)(((n) & 0x3)<<4)	// Endian Mode
#define SDHC_PROCTL_D3CD		((uint32_t)0x00000008)		// DAT3 As Card Detection Pin
#define SDHC_PROCTL_DTW(n)		(uint32_t)(((n) & 0x3)<<1)	// Data Transfer Width, 0=1bit, 1=4bit, 2=8bit
#define SDHC_PROCTL_DTW_MASK		((uint32_t)0x00000006)
#define SDHC_PROCTL_LCTL		((uint32_t)0x00000001)		// LED Control
#define SDHC_SYSCTL		(*(volatile uint32_t *)0x400B102C) // System Control register
#define SDHC_SYSCTL_INITA		((uint32_t)0x08000000)		// Initialization Active
#define SDHC_SYSCTL_RSTD		((uint32_t)0x04000000)		// Software Reset For DAT Line
#define SDHC_SYSCTL_RSTC		((uint32_t)0x02000000)		// Software Reset For CMD Line
#define SDHC_SYSCTL_RSTA		((uint32_t)0x01000000)		// Software Reset For ALL
#define SDHC_SYSCTL_DTOCV(n)		(uint32_t)(((n) & 0xF)<<16)	// Data Timeout Counter Value
#define SDHC_SYSCTL_DTOCV_MASK		((uint32_t)0x000F0000)
#define SDHC_SYSCTL_SDCLKFS(n)		(uint32_t)(((n) & 0xFF)<<8)	// SDCLK Frequency Select
#define SDHC_SYSCTL_SDCLKFS_MASK	((uint32_t)0x0000FF00)
#define SDHC_SYSCTL_DVS(n)		(uint32_t)(((n) & 0xF)<<4)	// Divisor
#define SDHC_SYSCTL_DVS_MASK		((uint32_t)0x000000F0)
#define SDHC_SYSCTL_SDCLKEN		((uint32_t)0x00000008)		// SD Clock Enable
#define SDHC_SYSCTL_PEREN		((uint32_t)0x00000004)		// Peripheral Clock Enable
#define SDHC_SYSCTL_HCKEN		((uint32_t)0x00000002)		// System Clock Enable
#define SDHC_SYSCTL_IPGEN		((uint32_t)0x00000001)		// IPG Clock Enable
#define SDHC_IRQSTAT		(*(volatile uint32_t *)0x400B1030) // Interrupt Status register
#define SDHC_IRQSTAT_DMAE		((uint32_t)0x10000000)		// DMA Error
#define SDHC_IRQSTAT_AC12E		((uint32_t)0x01000000)		// Auto CMD12 Error
#define SDHC_IRQSTAT_DEBE		((uint32_t)0x00400000)		// Data End Bit Error
#define SDHC_IRQSTAT_DCE		((uint32_t)0x00200000)		// Data CRC Error
#define SDHC_IRQSTAT_DTOE		((uint32_t)0x00100000)		// Data Timeout Error
#define SDHC_IRQSTAT_CIE		((uint32_t)0x00080000)		// Command Index Error
#define SDHC_IRQSTAT_CEBE		((uint32_t)0x00040000)		// Command End Bit Error
#define SDHC_IRQSTAT_CCE		((uint32_t)0x00020000)		// Command CRC Error
#define SDHC_IRQSTAT_CTOE		((uint32_t)0x00010000)		// Command Timeout Error
#define SDHC_IRQSTAT_CINT		((uint32_t)0x00000100)		// Card Interrupt
#define SDHC_IRQSTAT_CRM		((uint32_t)0x00000080)		// Card Removal
#define SDHC_IRQSTAT_CINS		((uint32_t)0x00000040)		// Card Insertion
#define SDHC_IRQSTAT_BRR		((uint32_t)0x00000020)		// Buffer Read Ready
#define SDHC_IRQSTAT_BWR		((uint32_t)0x00000010)		// Buffer Write Ready
#define SDHC_IRQSTAT_DINT		((uint32_t)0x00000008)		// DMA Interrupt
#define SDHC_IRQSTAT_BGE		((uint32_t)0x00000004)		// Block Gap Event
#define SDHC_IRQSTAT_TC			((uint32_t)0x00000002)		// Transfer Complete
#define SDHC_IRQSTAT_CC			((uint32_t)0x00000001)		// Command Complete
#define SDHC_IRQSTATEN		(*(volatile uint32_t *)0x400B1034) // Interrupt Status Enable register
#define SDHC_IRQSTATEN_DMAESEN		((uint32_t)0x10000000)		// DMA Error Status Enable
#define SDHC_IRQSTATEN_AC12ESEN		((uint32_t)0x01000000)		// Auto CMD12 Error Status Enable
#define SDHC_IRQSTATEN_DEBESEN		((uint32_t)0x00400000)		// Data End Bit Error Status Enable
#define SDHC_IRQSTATEN_DCESEN		((uint32_t)0x00200000)		// Data CRC Error Status Enable
#define SDHC_IRQSTATEN_DTOESEN		((uint32_t)0x00100000)		// Data Timeout Error Status Enable
#define SDHC_IRQSTATEN_CIESEN		((uint32_t)0x00080000)		// Command Index Error Status Enable
#define SDHC_IRQSTATEN_CEBESEN		((uint32_t)0x00040000)		// Command End Bit Error Status Enable
#define SDHC_IRQSTATEN_CCESEN		((uint32_t)0x00020000)		// Command CRC Error Status Enable
#define SDHC_IRQSTATEN_CTOESEN		((uint32_t)0x00010000)		// Command Timeout Error Status Enable
#define SDHC_IRQSTATEN_CINTSEN		((uint32_t)0x00000100)		// Card Interrupt Status Enable
#define SDHC_IRQSTATEN_CRMSEN		((uint32_t)0x00000080)		// Card Removal Status Enable
#define SDHC_IRQSTATEN_CINSEN		((uint32_t)0x00000040)		// Card Insertion Status Enable
#define SDHC_IRQSTATEN_BRRSEN		((uint32_t)0x00000020)		// Buffer Read Ready Status Enable
#define SDHC_IRQSTATEN_BWRSEN		((uint32_t)0x00000010)		// Buffer Write Ready Status Enable
#define SDHC_IRQSTATEN_DINTSEN		((uint32_t)0x00000008)		// DMA Interrupt Status Enable
#define SDHC_IRQSTATEN_BGESEN		((uint32_t)0x00000004)		// Block Gap Event Status Enable
#define SDHC_IRQSTATEN_TCSEN		((uint32_t)0x00000002)		// Transfer Complete Status Enable
#define SDHC_IRQSTATEN_CCSEN		((uint32_t)0x00000001)		// Command Complete Status Enable
#define SDHC_IRQSIGEN		(*(volatile uint32_t *)0x400B1038) // Interrupt Signal Enable register
#define SDHC_IRQSIGEN_DMAEIEN		((uint32_t)0x10000000)		// DMA Error Interrupt Enable
#define SDHC_IRQSIGEN_AC12EIEN		((uint32_t)0x01000000)		// Auto CMD12 Error Interrupt Enable
#define SDHC_IRQSIGEN_DEBEIEN		((uint32_t)0x00400000)		// Data End Bit Error Interrupt Enable
#define SDHC_IRQSIGEN_DCEIEN		((uint32_t)0x00200000)		// Data CRC Error Interrupt Enable
#define SDHC_IRQSIGEN_DTOEIEN		((uint32_t)0x00100000)		// Data Timeout Error Interrupt Enable
#define SDHC_IRQSIGEN_CIEIEN		((uint32_t)0x00080000)		// Command Index Error Interrupt Enable
#define SDHC_IRQSIGEN_CEBEIEN		((uint32_t)0x00040000)		// Command End Bit Error Interrupt Enable
#define SDHC_IRQSIGEN_CCEIEN		((uint32_t)0x00020000)		// Command CRC Error Interrupt Enable
#define SDHC_IRQSIGEN_CTOEIEN		((uint32_t)0x00010000)		// Command Timeout Error Interrupt Enable
#define SDHC_IRQSIGEN_CINTIEN		((uint32_t)0x00000100)		// Card Interrupt Interrupt Enable
#define SDHC_IRQSIGEN_CRMIEN		((uint32_t)0x00000080)		// Card Removal Interrupt Enable
#define SDHC_IRQSIGEN_CINSIEN		((uint32_t)0x00000040)		// Card Insertion Interrupt Enable
#define SDHC_IRQSIGEN_BRRIEN		((uint32_t)0x00000020)		// Buffer Read Ready Interrupt Enable
#define SDHC_IRQSIGEN_BWRIEN		((uint32_t)0x00000010)		// Buffer Write Ready Interrupt Enable
#define SDHC_IRQSIGEN_DINTIEN		((uint32_t)0x00000008)		// DMA Interrupt Interrupt Enable
#define SDHC_IRQSIGEN_BGEIEN		((uint32_t)0x00000004)		// Block Gap Event Interrupt Enable
#define SDHC_IRQSIGEN_TCIEN		((uint32_t)0x00000002)		// Transfer Complete Interrupt Enable
#define SDHC_IRQSIGEN_CCIEN		((uint32_t)0x00000001)		// Command Complete Interrupt Enable
#define SDHC_AC12ERR		(*(volatile uint32_t *)0x400B103C) // Auto CMD12 Error Status Register
#define SDHC_AC12ERR_CNIBAC12E		((uint32_t)0x00000080)		// Command Not Issued By Auto CMD12 Error
#define SDHC_AC12ERR_AC12IE		((uint32_t)0x00000010)		// Auto CMD12 Index Error
#define SDHC_AC12ERR_AC12CE		((uint32_t)0x00000008)		// Auto CMD12 CRC Error
#define SDHC_AC12ERR_AC12EBE		((uint32_t)0x00000004)		// Auto CMD12 End Bit Error
#define SDHC_AC12ERR_AC12TOE		((uint32_t)0x00000002)		// Auto CMD12 Timeout Error
#define SDHC_AC12ERR_AC12NE		((uint32_t)0x00000001)		// Auto CMD12 Not Executed
#define SDHC_HTCAPBLT		(*(volatile uint32_t *)0x400B1040) // Host Controller Capabilities
#define SDHC_WML		(*(volatile uint32_t *)0x400B1044) // Watermark Level Register
#define SDHC_WML_WRWML(n)		(uint32_t)(((n) & 0x7F)<<16)	// Write Watermark Level
#define SDHC_WML_RDWML(n)		(uint32_t)(((n) & 0x7F)<<0)	// Read Watermark Level
#define SDHC_FEVT		(*(volatile uint32_t *)0x400B1050) // Force Event register
#define SDHC_FEVT_CINT			((uint32_t)0x80000000)		// Force Event Card Interrupt
#define SDHC_FEVT_DMAE			((uint32_t)0x10000000)		// Force Event DMA Error
#define SDHC_FEVT_AC12E			((uint32_t)0x01000000)		// Force Event Auto CMD12 Error
#define SDHC_FEVT_DEBE			((uint32_t)0x00400000)		// Force Event Data End Bit Error
#define SDHC_FEVT_DCE			((uint32_t)0x00200000)		// Force Event Data CRC Error
#define SDHC_FEVT_DTOE			((uint32_t)0x00100000)		// Force Event Data Timeout Error
#define SDHC_FEVT_CIE			((uint32_t)0x00080000)		// Force Event Command Index Error
#define SDHC_FEVT_CEBE			((uint32_t)0x00040000)		// Force Event Command End Bit Error
#define SDHC_FEVT_CCE			((uint32_t)0x00020000)		// Force Event Command CRC Error
#define SDHC_FEVT_CTOE			((uint32_t)0x00010000)		// Force Event Command Timeout Error
#define SDHC_FEVT_CNIBAC12E		((uint32_t)0x00000080)		// Force Event Command Not Executed By Auto Command 12 Error
#define SDHC_FEVT_AC12IE		((uint32_t)0x00000010)		// Force Event Auto Command 12 Index Error
#define SDHC_FEVT_AC12EBE		((uint32_t)0x00000008)		// Force Event Auto Command 12 End Bit Error
#define SDHC_FEVT_AC12CE		((uint32_t)0x00000004)		// Force Event Auto Command 12 CRC Error
#define SDHC_FEVT_AC12TOE		((uint32_t)0x00000002)		// Force Event Auto Command 12 Time Out Error
#define SDHC_FEVT_AC12NE		((uint32_t)0x00000001)		// Force Event Auto Command 12 Not Executed
#define SDHC_ADMAES		(*(volatile uint32_t *)0x400B1054) // ADMA Error Status register
#define SDHC_ADMAES_ADMADCE		((uint32_t)0x00000008)
#define SDHC_ADMAES_ADMALME		((uint32_t)0x00000004)
#define SDHC_ADMAES_ADMAES_MASK		((uint32_t)0x00000003)
#define SDHC_ADSADDR		(*(volatile uint32_t *)0x400B1058) // ADMA System Addressregister
#define SDHC_VENDOR		(*(volatile uint32_t *)0x400B10C0) // Vendor Specific register
#define SDHC_VENDOR_INTSTVAL_MASK	((uint32_t)0x00FF0000)
#define SDHC_VENDOR_EXBLKNU		((uint32_t)0x00000002)
#define SDHC_MMCBOOT		(*(volatile uint32_t *)0x400B10C4) // MMC Boot register
#define SDHC_MMCBOOT_BOOTBLKCNT(n)	(uint32_t)(((n) & 0xFFF)<<16)	// stop at block gap value of automatic mode
#define SDHC_MMCBOOT_AUTOSABGEN 	((uint32_t)0x00000080)		// enable auto stop at block gap function
#define SDHC_MMCBOOT_BOOTEN		((uint32_t)0x00000040)		// Boot Mode Enable
#define SDHC_MMCBOOT_BOOTMODE		((uint32_t)0x00000020)		// Boot Mode Select
#define SDHC_MMCBOOT_BOOTACK		((uint32_t)0x00000010)		// Boot Ack Mode Select
#define SDHC_MMCBOOT_DTOCVACK(n)	(uint32_t)(((n) & 0xF)<<0)	// Boot ACK Time Out Counter Value
#define SDHC_HOSTVER		(*(volatile uint32_t *)0x400B10FC) // Host Controller Version

///////////////////////////////////
// Low Power Asynchronous Receiver/Transmitter (LPUART)

typedef struct __attribute__((packed)) {
	volatile uint32_t	BAUD;
	volatile uint32_t	STAT;
	volatile uint32_t	CTRL;
	volatile uint32_t	DATA;
	volatile uint32_t	MATCH;
	volatile uint32_t	MODIR;
} KINETISK_LPUART_t;
#define KINETISK_LPUART0	(*(KINETISK_LPUART_t *)0x400C4000)
#define LPUART0_BAUD		(KINETISK_LPUART0.BAUD)		// LPUART Baud Register
#define LPUART_BAUD_MAEN1		((uint32_t)0x80000000)		// Enable automatic address or data maching
#define LPUART_BAUD_MAEN2		((uint32_t)0x40000000)		// Enable automatic address or data maching
#define LPUART_BAUD_M10			((uint32_t)0x20000000)		// 10-bit Mode select
#define LPUART_BAUD_OSR(n)		((uint32_t)((n) & 0x1f) << 24) // Over sampling ratio
#define LPUART_BAUD_TDMAE		((uint32_t)0x00800000)		// Transmitter Dma Enable
#define LPUART_BAUD_RDMAE		((uint32_t)0x00400000)		// Receiver Dma Enable
#define LPUART_BAUD_BOTHEDGE	((uint32_t)0x00020000)		// Both edge sampling needed OSR 4-7
#define LPUART_BAUD_SBNS		((uint32_t)0x00002000)		// UART Stop Bit Number Select 
#define LPUART_BAUD_SBR(n)		((uint32_t)((n) & 0x1fff) << 0) // set baud rate divisor

#define LPUART0_STAT		(KINETISK_LPUART0.STAT)		// LPUART Status register
#define LPUART_STAT_LBKDIF		((uint32_t)0x80000000)		// LIN Break Detect Interrupt Flag
#define LPUART_STAT_RXEDGIF		((uint32_t)0x40000000)		// RxD Pin Active Edge Interrupt Flag
#define LPUART_STAT_MSBF		((uint32_t)0x20000000)		// Most Significant Bit First
#define LPUART_STAT_RXINV		((uint32_t)0x10000000)		// Receive Data Inversion
#define LPUART_STAT_RWUID		((uint32_t)0x08000000)		// Receive Wakeup Idle Detect
#define LPUART_STAT_BRK13		((uint32_t)0x04000000)		// Break Transmit Character Length
#define LPUART_STAT_LBKDE		((uint32_t)0x02000000)		// LIN Break Detection Enable
#define LPUART_STAT_RAF			((uint32_t)0x01000000)		// Receiver Active Flag
#define LPUART_STAT_TDRE		((uint32_t)0x00800000)		//  Transmit Data Register Empty Flag
#define LPUART_STAT_TC			((uint32_t)0x00400000)		//  Transmit Complete Flag
#define LPUART_STAT_RDRF		((uint32_t)0x00200000)		//  Receive Data Register Full Flag
#define LPUART_STAT_IDLE		((uint32_t)0x00100000)		//  Idle Line Flag
#define LPUART_STAT_OR			((uint32_t)0x00080000)		//  Receiver Overrun Flag
#define LPUART_STAT_NF			((uint32_t)0x00040000)		//  Noise Flag
#define LPUART_STAT_FE			((uint32_t)0x00020000)		//  Framing Error Flag
#define LPUART_STAT_PF			((uint32_t)0x00010000)		//  Parity Error Flag
#define LPUART_STAT_MA1F		((uint32_t)0x00008000)		//  Match 1 Flag
#define LPUART_STAT_MA2F		((uint32_t)0x00004000)		//  Match 2 Flag
#define LPUART0_CTRL		(KINETISK_LPUART0.CTRL)		// LPUART Control register
#define LPUART_CTRL_R8			((uint32_t)0x80000000)		// Received Bit 8
#define LPUART_CTRL_T8			((uint32_t)0x40000000)		// Transmit Bit 8
#define LPUART_CTRL_TXDIR		((uint32_t)0x20000000)		// TX Pin Direction in Single-Wire mode
#define LPUART_CTRL_TXINV		((uint32_t)0x10000000)		// Transmit Data Inversion
#define LPUART_CTRL_ORIE		((uint32_t)0x08000000)		// Overrun Error Interrupt Enable
#define LPUART_CTRL_NEIE		((uint32_t)0x04000000)		// Noise Error Interrupt Enable
#define LPUART_CTRL_FEIE		((uint32_t)0x02000000)		// Framing Error Interrupt Enable
#define LPUART_CTRL_PEIE		((uint32_t)0x01000000)		// Parity Error Interrupt Enable
#define LPUART_CTRL_TIE			((uint32_t)0x00800000)		//  Transmitter Interrupt or DMA Transfer Enable.
#define LPUART_CTRL_TCIE		((uint32_t)0x00400000)		//  Transmission Complete Interrupt Enable
#define LPUART_CTRL_RIE			((uint32_t)0x00200000)		//  Receiver Full Interrupt or DMA Transfer Enable
#define LPUART_CTRL_ILIE		((uint32_t)0x00100000)		//  Idle Line Interrupt Enable
#define LPUART_CTRL_TE			((uint32_t)0x00080000)		//  Transmitter Enable
#define LPUART_CTRL_RE			((uint32_t)0x00040000)		//  Receiver Enable
#define LPUART_CTRL_RWU			((uint32_t)0x00020000)		//  Receiver Wakeup Control
#define LPUART_CTRL_SBK			((uint32_t)0x00010000)		//  Send Break
#define LPUART_CTRL_MAEN1		((uint32_t)0x00008000)		// Match Address Mode Enable 1
#define LPUART_CTRL_MAEN2		((uint32_t)0x00004000)		// Match Address Mode Enable 2
#define LPUART_CTRL_LOOPS		((uint32_t)0x00000080)		//  When LOOPS is set, the RxD pin is disconnected from the UART and the transmitter output is internally connected to the receiver input
#define LPUART_CTRL_UARTSWAI	((uint32_t)0x00000040)		//  UART Stops in Wait Mode
#define LPUART_CTRL_RSRC		((uint32_t)0x00000020)		//  When LOOPS is set, the RSRC field determines the source for the receiver shift register input
#define LPUART_CTRL_M			((uint32_t)0x00000010)		//  9-bit or 8-bit Mode Select
#define LPUART_CTRL_WAKE		((uint32_t)0x00000008)		//  Determines which condition wakes the UART
#define LPUART_CTRL_ILT			((uint32_t)0x00000004)		//  Idle Line Type Select
#define LPUART_CTRL_PE			((uint32_t)0x00000002)		//  Parity Enable
#define LPUART_CTRL_PT			((uint32_t)0x00000001)		//  Parity Type, 0=even, 1=odd

#define LPUART0_DATA		(KINETISK_LPUART0.DATA)		// LPUART Data register
#define LPUART_DATA_NOISY		((uint32_t)0x00080000)		//  Data received with noise
#define LPUART_DATA_PARITY		((uint32_t)0x00040000)		//  Data received with Parity error
#define LPUART_DATA_FRETSC		((uint32_t)0x00020000)		//  Frame error/Transmit Special char
#define LPUART_DATA_RXEMPT		((uint32_t)0x00010000)		//  receive buffer empty
#define LPUART_DATA_IDLINE		((uint32_t)0x00008000)		// Match Address Mode Enable 1
#define LPUART0_MATCH		(KINETISK_LPUART0.MATCH)	// LPUART Match register
#define LPUART0_MODIR		(KINETISK_LPUART0.MODIR)	// LPUART Modem IrDA Register


// Synchronous Audio Interface (SAI)

#define I2S0_TCSR		(*(volatile uint32_t *)0x4002F000) // SAI Transmit Control Register
#define I2S_TCSR_TE			((uint32_t)0x80000000)		// Transmitter Enable
#define I2S_TCSR_STOPE			((uint32_t)0x40000000)		// Transmitter Enable in Stop mode
#define I2S_TCSR_DBGE			((uint32_t)0x20000000)		// Transmitter Enable in Debug mode
#define I2S_TCSR_BCE			((uint32_t)0x10000000)		// Bit Clock Enable
#define I2S_TCSR_FR			((uint32_t)0x02000000)		// FIFO Reset
#define I2S_TCSR_SR			((uint32_t)0x01000000)		// Software Reset
#define I2S_TCSR_WSF			((uint32_t)0x00100000)		// Word Start Flag
#define I2S_TCSR_SEF			((uint32_t)0x00080000)		// Sync Error Flag
#define I2S_TCSR_FEF			((uint32_t)0x00040000)		// FIFO Error Flag (underrun)
#define I2S_TCSR_FWF			((uint32_t)0x00020000)		// FIFO Warning Flag (empty)
#define I2S_TCSR_FRF			((uint32_t)0x00010000)		// FIFO Request Flag (Data Ready)
#define I2S_TCSR_WSIE			((uint32_t)0x00001000)		// Word Start Interrupt Enable
#define I2S_TCSR_SEIE			((uint32_t)0x00000800)		// Sync Error Interrupt Enable
#define I2S_TCSR_FEIE			((uint32_t)0x00000400)		// FIFO Error Interrupt Enable
#define I2S_TCSR_FWIE			((uint32_t)0x00000200)		// FIFO Warning Interrupt Enable
#define I2S_TCSR_FRIE			((uint32_t)0x00000100)		// FIFO Request Interrupt Enable
#define I2S_TCSR_FWDE			((uint32_t)0x00000002)		// FIFO Warning DMA Enable
#define I2S_TCSR_FRDE			((uint32_t)0x00000001)		// FIFO Request DMA Enable
#define I2S0_TCR1		(*(volatile uint32_t *)0x4002F004) // SAI Transmit Configuration 1 Register
#define I2S_TCR1_TFW(n)			((uint32_t)n & 0x03)		// Transmit FIFO watermark
#define I2S0_TCR2		(*(volatile uint32_t *)0x4002F008) // SAI Transmit Configuration 2 Register
#define I2S_TCR2_DIV(n)			((uint32_t)n & 0xff)		// Bit clock divide by (DIV+1)*2
#define I2S_TCR2_BCD			((uint32_t)1<<24)		// Bit clock direction
#define I2S_TCR2_BCP			((uint32_t)1<<25)		// Bit clock polarity
#define I2S_TCR2_MSEL(n)		((uint32_t)(n & 3)<<26)		// MCLK select, 0=bus clock, 1=I2S0_MCLK
#define I2S_TCR2_BCI			((uint32_t)1<<28)		// Bit clock input
#define I2S_TCR2_BCS			((uint32_t)1<<29)		// Bit clock swap
#define I2S_TCR2_SYNC(n)		((uint32_t)(n & 3)<<30)		// 0=async 1=sync with receiver
#define I2S0_TCR3		(*(volatile uint32_t *)0x4002F00C) // SAI Transmit Configuration 3 Register
#define I2S_TCR3_WDFL(n)		((uint32_t)n & 0x0f)		// word flag configuration
#define I2S_TCR3_TCE			((uint32_t)0x10000)		// transmit channel enable
#define I2S_TCR3_TCE_2CH		((uint32_t)0x30000)		// transmit 2 channel enable
#define I2S0_TCR4		(*(volatile uint32_t *)0x4002F010) // SAI Transmit Configuration 4 Register
#define I2S_TCR4_FSD			((uint32_t)1)			// Frame Sync Direction
#define I2S_TCR4_FSP			((uint32_t)2)			// Frame Sync Polarity
#define I2S_TCR4_FSE			((uint32_t)8)			// Frame Sync Early
#define I2S_TCR4_MF			((uint32_t)0x10)		// MSB First
#define I2S_TCR4_SYWD(n)		((uint32_t)(n & 0x1f)<<8)	// Sync Width
#define I2S_TCR4_FRSZ(n)		((uint32_t)(n & 0x0f)<<16)	// Frame Size
#define I2S0_TCR5		(*(volatile uint32_t *)0x4002F014) // SAI Transmit Configuration 5 Register
#define I2S_TCR5_FBT(n)			((uint32_t)(n & 0x1f)<<8)	// First Bit Shifted
#define I2S_TCR5_W0W(n)			((uint32_t)(n & 0x1f)<<16)	// Word 0 Width
#define I2S_TCR5_WNW(n)			((uint32_t)(n & 0x1f)<<24)	// Word N Width
#define I2S0_TDR0		(*(volatile uint32_t *)0x4002F020) // SAI Transmit Data Register
#define I2S0_TDR1		(*(volatile uint32_t *)0x4002F024) // SAI Transmit Data Register
#define I2S0_TFR0		(*(volatile uint32_t *)0x4002F040) // SAI Transmit FIFO Register
#define I2S0_TFR1		(*(volatile uint32_t *)0x4002F044) // SAI Transmit FIFO Register
#define I2S_TFR_RFP(n)			((uint32_t)n & 7)		// read FIFO pointer
#define I2S_TFR_WFP(n)			((uint32_t)(n & 7)<<16)		// write FIFO pointer
#define I2S0_TMR		(*(volatile uint32_t *)0x4002F060) // SAI Transmit Mask Register
#define I2S_TMR_TWM(n)			((uint32_t)n & 0xFFFFFFFF)	//
#define I2S0_RCSR		(*(volatile uint32_t *)0x4002F080) // SAI Receive Control Register
#define I2S_RCSR_RE			((uint32_t)0x80000000)		// Receiver Enable
#define I2S_RCSR_STOPE			((uint32_t)0x40000000)		// Receiver Enable in Stop mode
#define I2S_RCSR_DBGE			((uint32_t)0x20000000)		// Receiver Enable in Debug mode
#define I2S_RCSR_BCE			((uint32_t)0x10000000)		// Bit Clock Enable
#define I2S_RCSR_FR			((uint32_t)0x02000000)		// FIFO Reset
#define I2S_RCSR_SR			((uint32_t)0x01000000)		// Software Reset
#define I2S_RCSR_WSF			((uint32_t)0x00100000)		// Word Start Flag
#define I2S_RCSR_SEF			((uint32_t)0x00080000)		// Sync Error Flag
#define I2S_RCSR_FEF			((uint32_t)0x00040000)		// FIFO Error Flag (underrun)
#define I2S_RCSR_FWF			((uint32_t)0x00020000)		// FIFO Warning Flag (empty)
#define I2S_RCSR_FRF			((uint32_t)0x00010000)		// FIFO Request Flag (Data Ready)
#define I2S_RCSR_WSIE			((uint32_t)0x00001000)		// Word Start Interrupt Enable
#define I2S_RCSR_SEIE			((uint32_t)0x00000800)		// Sync Error Interrupt Enable
#define I2S_RCSR_FEIE			((uint32_t)0x00000400)		// FIFO Error Interrupt Enable
#define I2S_RCSR_FWIE			((uint32_t)0x00000200)		// FIFO Warning Interrupt Enable
#define I2S_RCSR_FRIE			((uint32_t)0x00000100)		// FIFO Request Interrupt Enable
#define I2S_RCSR_FWDE			((uint32_t)0x00000002)		// FIFO Warning DMA Enable
#define I2S_RCSR_FRDE			((uint32_t)0x00000001)		// FIFO Request DMA Enable
#define I2S0_RCR1		(*(volatile uint32_t *)0x4002F084) // SAI Receive Configuration 1 Register
#define I2S_RCR1_RFW(n)			((uint32_t)n & 0x03)		// Receive FIFO watermark
#define I2S0_RCR2		(*(volatile uint32_t *)0x4002F088) // SAI Receive Configuration 2 Register
#define I2S_RCR2_DIV(n)			((uint32_t)n & 0xff)		// Bit clock divide by (DIV+1)*2
#define I2S_RCR2_BCD			((uint32_t)1<<24)		// Bit clock direction
#define I2S_RCR2_BCP			((uint32_t)1<<25)		// Bit clock polarity
#define I2S_RCR2_MSEL(n)		((uint32_t)(n & 3)<<26)		// MCLK select, 0=bus clock, 1=I2S0_MCLK
#define I2S_RCR2_BCI			((uint32_t)1<<28)		// Bit clock input
#define I2S_RCR2_BCS			((uint32_t)1<<29)		// Bit clock swap
#define I2S_RCR2_SYNC(n)		((uint32_t)(n & 3)<<30)		// 0=async 1=sync with receiver
#define I2S0_RCR3		(*(volatile uint32_t *)0x4002F08C) // SAI Receive Configuration 3 Register
#define I2S_RCR3_WDFL(n)		((uint32_t)n & 0x0f)		// word flag configuration
#define I2S_RCR3_RCE			((uint32_t)0x10000)		// receive channel enable
#define I2S_RCR3_RCE_2CH		((uint32_t)0x30000)		// receive 2 channel enable
#define I2S0_RCR4		(*(volatile uint32_t *)0x4002F090) // SAI Receive Configuration 4 Register
#define I2S_RCR4_FSD			((uint32_t)1)			// Frame Sync Direction
#define I2S_RCR4_FSP			((uint32_t)2)			// Frame Sync Polarity
#define I2S_RCR4_FSE			((uint32_t)8)			// Frame Sync Early
#define I2S_RCR4_MF			((uint32_t)0x10)		// MSB First
#define I2S_RCR4_SYWD(n)		((uint32_t)(n & 0x1f)<<8)	// Sync Width
#define I2S_RCR4_FRSZ(n)		((uint32_t)(n & 0x0f)<<16)	// Frame Size
#define I2S0_RCR5		(*(volatile uint32_t *)0x4002F094) // SAI Receive Configuration 5 Register
#define I2S_RCR5_FBT(n)			((uint32_t)(n & 0x1f)<<8)	// First Bit Shifted
#define I2S_RCR5_W0W(n)			((uint32_t)(n & 0x1f)<<16)	// Word 0 Width
#define I2S_RCR5_WNW(n)			((uint32_t)(n & 0x1f)<<24)	// Word N Width
#define I2S0_RDR0		(*(volatile uint32_t *)0x4002F0A0) // SAI Receive Data Register
#define I2S0_RDR1		(*(volatile uint32_t *)0x4002F0A4) // SAI Receive Data Register
#define I2S0_RFR0		(*(volatile uint32_t *)0x4002F0C0) // SAI Receive FIFO Register
#define I2S0_RFR1		(*(volatile uint32_t *)0x4002F0C4) // SAI Receive FIFO Register
#define I2S_RFR_RFP(n)			((uint32_t)n & 7)		// read FIFO pointer
#define I2S_RFR_WFP(n)			((uint32_t)(n & 7)<<16)		// write FIFO pointer
#define I2S0_RMR		(*(volatile uint32_t *)0x4002F0E0) // SAI Receive Mask Register
#define I2S_RMR_RWM(n)			((uint32_t)n & 0xFFFFFFFF)	//
#define I2S0_MCR		(*(volatile uint32_t *)0x4002F100) // SAI MCLK Control Register
#define I2S_MCR_DUF			((uint32_t)1<<31)		// Divider Update Flag
#define I2S_MCR_MOE			((uint32_t)1<<30)		// MCLK Output Enable
#define I2S_MCR_MICS(n)			((uint32_t)(n & 3)<<24)		// MCLK Input Clock Select
#define I2S0_MDR		(*(volatile uint32_t *)0x4002F104) // SAI MCLK Divide Register
#define I2S_MDR_FRACT(n)		((uint32_t)(n & 0xff)<<12)	// MCLK Fraction
#define I2S_MDR_DIVIDE(n)		((uint32_t)(n & 0xfff))		// MCLK Divide

// General-Purpose Input/Output (GPIO)

#define GPIOA_PDOR		(*(volatile uint32_t *)0x400FF000) // Port Data Output Register
#define GPIOA_PSOR		(*(volatile uint32_t *)0x400FF004) // Port Set Output Register
#define GPIOA_PCOR		(*(volatile uint32_t *)0x400FF008) // Port Clear Output Register
#define GPIOA_PTOR		(*(volatile uint32_t *)0x400FF00C) // Port Toggle Output Register
#define GPIOA_PDIR		(*(volatile uint32_t *)0x400FF010) // Port Data Input Register
#define GPIOA_PDDR		(*(volatile uint32_t *)0x400FF014) // Port Data Direction Register
#define GPIOB_PDOR		(*(volatile uint32_t *)0x400FF040) // Port Data Output Register
#define GPIOB_PSOR		(*(volatile uint32_t *)0x400FF044) // Port Set Output Register
#define GPIOB_PCOR		(*(volatile uint32_t *)0x400FF048) // Port Clear Output Register
#define GPIOB_PTOR		(*(volatile uint32_t *)0x400FF04C) // Port Toggle Output Register
#define GPIOB_PDIR		(*(volatile uint32_t *)0x400FF050) // Port Data Input Register
#define GPIOB_PDDR		(*(volatile uint32_t *)0x400FF054) // Port Data Direction Register
#define GPIOC_PDOR		(*(volatile uint32_t *)0x400FF080) // Port Data Output Register
#define GPIOC_PSOR		(*(volatile uint32_t *)0x400FF084) // Port Set Output Register
#define GPIOC_PCOR		(*(volatile uint32_t *)0x400FF088) // Port Clear Output Register
#define GPIOC_PTOR		(*(volatile uint32_t *)0x400FF08C) // Port Toggle Output Register
#define GPIOC_PDIR		(*(volatile uint32_t *)0x400FF090) // Port Data Input Register
#define GPIOC_PDDR		(*(volatile uint32_t *)0x400FF094) // Port Data Direction Register
#define GPIOD_PDOR		(*(volatile uint32_t *)0x400FF0C0) // Port Data Output Register
#define GPIOD_PSOR		(*(volatile uint32_t *)0x400FF0C4) // Port Set Output Register
#define GPIOD_PCOR		(*(volatile uint32_t *)0x400FF0C8) // Port Clear Output Register
#define GPIOD_PTOR		(*(volatile uint32_t *)0x400FF0CC) // Port Toggle Output Register
#define GPIOD_PDIR		(*(volatile uint32_t *)0x400FF0D0) // Port Data Input Register
#define GPIOD_PDDR		(*(volatile uint32_t *)0x400FF0D4) // Port Data Direction Register
#define GPIOE_PDOR		(*(volatile uint32_t *)0x400FF100) // Port Data Output Register
#define GPIOE_PSOR		(*(volatile uint32_t *)0x400FF104) // Port Set Output Register
#define GPIOE_PCOR		(*(volatile uint32_t *)0x400FF108) // Port Clear Output Register
#define GPIOE_PTOR		(*(volatile uint32_t *)0x400FF10C) // Port Toggle Output Register
#define GPIOE_PDIR		(*(volatile uint32_t *)0x400FF110) // Port Data Input Register
#define GPIOE_PDDR		(*(volatile uint32_t *)0x400FF114) // Port Data Direction Register

#if defined(KINETISL)
#define FGPIOA_PDOR		(*(volatile uint32_t *)0xF8000000) // Port Data Output Register
#define FGPIOA_PSOR		(*(volatile uint32_t *)0xF8000004) // Port Set Output Register
#define FGPIOA_PCOR		(*(volatile uint32_t *)0xF8000008) // Port Clear Output Register
#define FGPIOA_PTOR		(*(volatile uint32_t *)0xF800000C) // Port Toggle Output Register
#define FGPIOA_PDIR		(*(volatile uint32_t *)0xF8000010) // Port Data Input Register
#define FGPIOA_PDDR		(*(volatile uint32_t *)0xF8000014) // Port Data Direction Register
#define FGPIOB_PDOR		(*(volatile uint32_t *)0xF8000040) // Port Data Output Register
#define FGPIOB_PSOR		(*(volatile uint32_t *)0xF8000044) // Port Set Output Register
#define FGPIOB_PCOR		(*(volatile uint32_t *)0xF8000048) // Port Clear Output Register
#define FGPIOB_PTOR		(*(volatile uint32_t *)0xF800004C) // Port Toggle Output Register
#define FGPIOB_PDIR		(*(volatile uint32_t *)0xF8000050) // Port Data Input Register
#define FGPIOB_PDDR		(*(volatile uint32_t *)0xF8000054) // Port Data Direction Register
#define FGPIOC_PDOR		(*(volatile uint32_t *)0xF8000080) // Port Data Output Register
#define FGPIOC_PSOR		(*(volatile uint32_t *)0xF8000084) // Port Set Output Register
#define FGPIOC_PCOR		(*(volatile uint32_t *)0xF8000088) // Port Clear Output Register
#define FGPIOC_PTOR		(*(volatile uint32_t *)0xF800008C) // Port Toggle Output Register
#define FGPIOC_PDIR		(*(volatile uint32_t *)0xF8000090) // Port Data Input Register
#define FGPIOC_PDDR		(*(volatile uint32_t *)0xF8000094) // Port Data Direction Register
#define FGPIOD_PDOR		(*(volatile uint32_t *)0xF80000C0) // Port Data Output Register
#define FGPIOD_PSOR		(*(volatile uint32_t *)0xF80000C4) // Port Set Output Register
#define FGPIOD_PCOR		(*(volatile uint32_t *)0xF80000C8) // Port Clear Output Register
#define FGPIOD_PTOR		(*(volatile uint32_t *)0xF80000CC) // Port Toggle Output Register
#define FGPIOD_PDIR		(*(volatile uint32_t *)0xF80000D0) // Port Data Input Register
#define FGPIOD_PDDR		(*(volatile uint32_t *)0xF80000D4) // Port Data Direction Register
#define FGPIOE_PDOR		(*(volatile uint32_t *)0xF8000100) // Port Data Output Register
#define FGPIOE_PSOR		(*(volatile uint32_t *)0xF8000104) // Port Set Output Register
#define FGPIOE_PCOR		(*(volatile uint32_t *)0xF8000108) // Port Clear Output Register
#define FGPIOE_PTOR		(*(volatile uint32_t *)0xF800010C) // Port Toggle Output Register
#define FGPIOE_PDIR		(*(volatile uint32_t *)0xF8000110) // Port Data Input Register
#define FGPIOE_PDDR		(*(volatile uint32_t *)0xF8000114) // Port Data Direction Register
#endif

// Touch sense input (TSI)

#if defined(HAS_KINETIS_TSI)
#define TSI0_GENCS		(*(volatile uint32_t *)0x40045000) // General Control and Status Register
#define TSI_GENCS_LPCLKS		((uint32_t)0x10000000)		//
#define TSI_GENCS_LPSCNITV(n)		(((n) & 15) << 24)		//
#define TSI_GENCS_NSCN(n)		(((n) & 31) << 19)		//
#define TSI_GENCS_PS(n)			(((n) & 7) << 16)		//
#define TSI_GENCS_EOSF			((uint32_t)0x00008000)		//
#define TSI_GENCS_OUTRGF		((uint32_t)0x00004000)		//
#define TSI_GENCS_EXTERF		((uint32_t)0x00002000)		//
#define TSI_GENCS_OVRF			((uint32_t)0x00001000)		//
#define TSI_GENCS_SCNIP			((uint32_t)0x00000200)		//
#define TSI_GENCS_SWTS			((uint32_t)0x00000100)		//
#define TSI_GENCS_TSIEN			((uint32_t)0x00000080)		//
#define TSI_GENCS_TSIIE			((uint32_t)0x00000040)		//
#define TSI_GENCS_ERIE			((uint32_t)0x00000020)		//
#define TSI_GENCS_ESOR			((uint32_t)0x00000010)		//
#define TSI_GENCS_STM			((uint32_t)0x00000002)		//
#define TSI_GENCS_STPE			((uint32_t)0x00000001)		//
#define TSI0_SCANC		(*(volatile uint32_t *)0x40045004) // SCAN Control Register
#define TSI_SCANC_REFCHRG(n)		(((n) & 15) << 24)		//
#define TSI_SCANC_EXTCHRG(n)		(((n) & 15) << 16)		//
#define TSI_SCANC_SMOD(n)		(((n) & 255) << 8)		//
#define TSI_SCANC_AMCLKS(n)		(((n) & 3) << 3)		//
#define TSI_SCANC_AMPSC(n)		(((n) & 7) << 0)		//
#define TSI0_PEN		(*(volatile uint32_t *)0x40045008) // Pin Enable Register
#define TSI0_WUCNTR		(*(volatile uint32_t *)0x4004500C) // Wake-Up Channel Counter Register
#define TSI0_CNTR1		(*(volatile uint32_t *)0x40045100) // Counter Register
#define TSI0_CNTR3		(*(volatile uint32_t *)0x40045104) // Counter Register
#define TSI0_CNTR5		(*(volatile uint32_t *)0x40045108) // Counter Register
#define TSI0_CNTR7		(*(volatile uint32_t *)0x4004510C) // Counter Register
#define TSI0_CNTR9		(*(volatile uint32_t *)0x40045110) // Counter Register
#define TSI0_CNTR11		(*(volatile uint32_t *)0x40045114) // Counter Register
#define TSI0_CNTR13		(*(volatile uint32_t *)0x40045118) // Counter Register
#define TSI0_CNTR15		(*(volatile uint32_t *)0x4004511C) // Counter Register
#define TSI0_THRESHOLD		(*(volatile uint32_t *)0x40045120) // Low Power Channel Threshold Register
#elif defined(HAS_KINETIS_TSI_LITE)
#define TSI0_GENCS		(*(volatile uint32_t *)0x40045000) // General Control and Status
#define TSI_GENCS_OUTRGF		((uint32_t)0x80000000)		// Out of Range Flag
#define TSI_GENCS_ESOR			((uint32_t)0x10000000)		// End-of-scan or Out-of-Range Interrupt Selection
#define TSI_GENCS_MODE(n)		(((n) & 15) << 24)		// analog modes & status
#define TSI_GENCS_REFCHRG(n)		(((n) & 7) << 21)		// reference  charge and discharge current
#define TSI_GENCS_DVOLT(n)		(((n) & 3) << 19)		// voltage rails
#define TSI_GENCS_EXTCHRG(n)		(((n) & 7) << 16)		// electrode charge and discharge current
#define TSI_GENCS_PS(n)			(((n) & 7) << 13)		// prescaler
#define TSI_GENCS_NSCN(n)		(((n) & 31) << 8)		// scan number
#define TSI_GENCS_TSIEN			((uint32_t)0x00000080)		// Enable
#define TSI_GENCS_TSIIEN		((uint32_t)0x00000040)		// Interrupt Enable
#define TSI_GENCS_STPE			((uint32_t)0x00000020)		// STOP Enable
#define TSI_GENCS_STM			((uint32_t)0x00000010)		// Trigger Mode
#define TSI_GENCS_SCNIP			((uint32_t)0x00000008)		// Scan In Progress Status
#define TSI_GENCS_EOSF			((uint32_t)0x00000004)		// End of Scan Flag
#define TSI_GENCS_CURSW			((uint32_t)0x00000002)		// current sources swapped
#define TSI0_DATA		(*(volatile uint32_t *)0x40045004) // Data
#define TSI_DATA_TSICH(n)		(((n) & 15) << 28)		// channel
#define TSI_DATA_DMAEN			((uint32_t)0x00800000)		// DMA Transfer Enabled
#define TSI_DATA_SWTS			((uint32_t)0x00400000)		// Software Trigger Start
#define TSI_DATA_TSICNT(n)		(((n) & 65535) << 0)		//  Conversion Counter Value
#define TSI0_TSHD		(*(volatile uint32_t *)0x40045008) // Threshold
#define TSI_TSHD_THRESH(n)		(((n) & 65535) << 16)		// High wakeup threshold
#define TSI_TSHD_THRESL(n)		(((n) & 65535) << 0)		// Low wakeup threshold
#endif

// Nested Vectored Interrupt Controller, Table 3-4 & ARMv7 ref, appendix B3.4 (page 750)
#define NVIC_STIR			(*(volatile uint32_t *)0xE000EF00)
#define NVIC_ENABLE_IRQ(n)	(*((volatile uint32_t *)0xE000E100 + ((n) >> 5)) = (1 << ((n) & 31)))
#define NVIC_DISABLE_IRQ(n)	(*((volatile uint32_t *)0xE000E180 + ((n) >> 5)) = (1 << ((n) & 31)))
#define NVIC_SET_PENDING(n)	(*((volatile uint32_t *)0xE000E200 + ((n) >> 5)) = (1 << ((n) & 31)))
#define NVIC_CLEAR_PENDING(n)	(*((volatile uint32_t *)0xE000E280 + ((n) >> 5)) = (1 << ((n) & 31)))
#define NVIC_IS_ENABLED(n)	(*((volatile uint32_t *)0xE000E100 + ((n) >> 5)) & (1 << ((n) & 31)))
#define NVIC_IS_PENDING(n)	(*((volatile uint32_t *)0xE000E200 + ((n) >> 5)) & (1 << ((n) & 31)))
#define NVIC_IS_ACTIVE(n)	(*((volatile uint32_t *)0xE000E300 + ((n) >> 5)) & (1 << ((n) & 31)))
#ifdef KINETISK
#define NVIC_TRIGGER_IRQ(n)	NVIC_STIR=(n)
#else
#define NVIC_TRIGGER_IRQ(n)	NVIC_SET_PENDING(n)
#endif

#define NVIC_ISER0		(*(volatile uint32_t *)0xE000E100)
#define NVIC_ISER1		(*(volatile uint32_t *)0xE000E104)
#define NVIC_ISER2		(*(volatile uint32_t *)0xE000E108)
#define NVIC_ISER3		(*(volatile uint32_t *)0xE000E10C)
#define NVIC_ICER0		(*(volatile uint32_t *)0xE000E180)
#define NVIC_ICER1		(*(volatile uint32_t *)0xE000E184)
#define NVIC_ICER2		(*(volatile uint32_t *)0xE000E188)
#define NVIC_ICER3		(*(volatile uint32_t *)0xE000E18C)

// 0 = highest priority
// Cortex-M4: 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240
// Cortex-M0: 0,64,128,192
#ifdef KINETISK
#define NVIC_SET_PRIORITY(irqnum, priority)  (*((volatile uint8_t *)0xE000E400 + (irqnum)) = (uint8_t)(priority))
#define NVIC_GET_PRIORITY(irqnum) (*((uint8_t *)0xE000E400 + (irqnum)))
#else
#define NVIC_SET_PRIORITY(irqnum, priority) (*((uint32_t *)0xE000E400 + ((irqnum) >> 2)) = (*((uint32_t *)0xE000E400 + ((irqnum) >> 2)) & (~(0xFF << (8 * ((irqnum) & 3))))) | (((priority) & 0xFF) << (8 * ((irqnum) & 3))))
#define NVIC_GET_PRIORITY(irqnum) (*((uint32_t *)0xE000E400 + ((irqnum) >> 2)) >> (8 * ((irqnum) & 3)) & 255)
#endif



#define __disable_irq() __asm__ volatile("CPSID i":::"memory");
#define __enable_irq()	__asm__ volatile("CPSIE i":::"memory");

// System Control Space (SCS), ARMv7 ref manual, B3.2, page 708
#define SCB_CPUID		(*(const    uint32_t *)0xE000ED00) // CPUID Base Register
#define SCB_ICSR		(*(volatile uint32_t *)0xE000ED04) // Interrupt Control and State
#define SCB_ICSR_PENDSTSET		((uint32_t)0x04000000)
#define SCB_VTOR		(*(volatile uint32_t *)0xE000ED08) // Vector Table Offset
#define SCB_AIRCR		(*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control
#define SCB_SCR			(*(volatile uint32_t *)0xE000ED10) // System Control Register
#define SCB_SCR_SEVONPEND   ((uint8_t)0x10)	   // Send Event on Pending bit
#define SCB_SCR_SLEEPDEEP   ((uint8_t)0x04)	   // Sleep or Deep Sleep
#define SCB_SCR_SLEEPONEXIT ((uint8_t)0x02)	   // Sleep-on-exit
#define SCB_CCR			(*(volatile uint32_t *)0xE000ED14) // Configuration and Control
#define SCB_SHPR1		(*(volatile uint32_t *)0xE000ED18) // System Handler Priority Register 1
#define SCB_SHPR2		(*(volatile uint32_t *)0xE000ED1C) // System Handler Priority Register 2
#define SCB_SHPR3		(*(volatile uint32_t *)0xE000ED20) // System Handler Priority Register 3
#define SCB_SHCSR		(*(volatile uint32_t *)0xE000ED24) // System Handler Control and State
#define SCB_CFSR		(*(volatile uint32_t *)0xE000ED28) // Configurable Fault Status Register
#define SCB_HFSR		(*(volatile uint32_t *)0xE000ED2C) // HardFault Status
#define SCB_DFSR		(*(volatile uint32_t *)0xE000ED30) // Debug Fault Status
#define SCB_MMFAR		(*(volatile uint32_t *)0xE000ED34) // MemManage Fault Address
#define SCB_BFAR		(*(volatile uint32_t *)0xE000ED38) // Bus Fault Address
#define SCB_AFAR		(*(volatile uint32_t *)0xE000ED3C) // Aux Fault Address
#define SCB_CPACR		(*(volatile uint32_t *)0xE000ED88) // Coprocessor Access Control
#define SCB_FPCCR		(*(volatile uint32_t *)0xE000EF34) // FP Context Control
#define SCB_FPCAR		(*(volatile uint32_t *)0xE000EF38) // FP Context Address
#define SCB_FPDSCR		(*(volatile uint32_t *)0xE000EF3C) // FP Default Status Control
#define SCB_MVFR0		(*(volatile uint32_t *)0xE000EF40) // Media & FP Feature 0
#define SCB_MVFR1		(*(volatile uint32_t *)0xE000EF44) // Media & FP Feature 1
#define SCB_MVFR2		(*(volatile uint32_t *)0xE000EF48) // Media & FP Feature 2

#define SYST_CSR		(*(volatile uint32_t *)0xE000E010) // SysTick Control and Status
#define SYST_CSR_COUNTFLAG		((uint32_t)0x00010000)
#define SYST_CSR_CLKSOURCE		((uint32_t)0x00000004)
#define SYST_CSR_TICKINT		((uint32_t)0x00000002)
#define SYST_CSR_ENABLE			((uint32_t)0x00000001)
#define SYST_RVR		(*(volatile uint32_t *)0xE000E014) // SysTick Reload Value Register
#define SYST_CVR		(*(volatile uint32_t *)0xE000E018) // SysTick Current Value Register
#define SYST_CALIB		(*(const    uint32_t *)0xE000E01C) // SysTick Calibration Value


#define ARM_DEMCR		(*(volatile uint32_t *)0xE000EDFC) // Debug Exception and Monitor Control
#define ARM_DEMCR_TRCENA		(1 << 24)	 // Enable debugging & monitoring blocks
#define ARM_DWT_CTRL		(*(volatile uint32_t *)0xE0001000) // DWT control register
#define ARM_DWT_CTRL_CYCCNTENA		(1 << 0)		// Enable cycle count
#define ARM_DWT_CYCCNT		(*(volatile uint32_t *)0xE0001004) // Cycle count register



#ifdef __cplusplus
extern "C" {
#endif
extern int nvic_execution_priority(void);

extern void nmi_isr(void);
extern void hard_fault_isr(void);
extern void memmanage_fault_isr(void);
extern void bus_fault_isr(void);
extern void usage_fault_isr(void);
extern void svcall_isr(void);
extern void debugmonitor_isr(void);
extern void pendablesrvreq_isr(void);
extern void systick_isr(void);
extern void dma_ch0_isr(void);
extern void dma_ch1_isr(void);
extern void dma_ch2_isr(void);
extern void dma_ch3_isr(void);
extern void dma_ch4_isr(void);
extern void dma_ch5_isr(void);
extern void dma_ch6_isr(void);
extern void dma_ch7_isr(void);
extern void dma_ch8_isr(void);
extern void dma_ch9_isr(void);
extern void dma_ch10_isr(void);
extern void dma_ch11_isr(void);
extern void dma_ch12_isr(void);
extern void dma_ch13_isr(void);
extern void dma_ch14_isr(void);
extern void dma_ch15_isr(void);
extern void dma_error_isr(void);
extern void mcm_isr(void);
extern void randnum_isr(void);
extern void flash_cmd_isr(void);
extern void flash_error_isr(void);
extern void low_voltage_isr(void);
extern void wakeup_isr(void);
extern void watchdog_isr(void);
extern void i2c0_isr(void);
extern void i2c1_isr(void);
extern void i2c2_isr(void);
extern void i2c3_isr(void);
extern void spi0_isr(void);
extern void spi1_isr(void);
extern void spi2_isr(void);
extern void sdhc_isr(void);
extern void enet_timer_isr(void);
extern void enet_tx_isr(void);
extern void enet_rx_isr(void);
extern void enet_error_isr(void);
extern void can0_message_isr(void);
extern void can0_bus_off_isr(void);
extern void can0_error_isr(void);
extern void can0_tx_warn_isr(void);
extern void can0_rx_warn_isr(void);
extern void can0_wakeup_isr(void);
extern void can1_message_isr(void);
extern void can1_bus_off_isr(void);
extern void can1_error_isr(void);
extern void can1_tx_warn_isr(void);
extern void can1_rx_warn_isr(void);
extern void can1_wakeup_isr(void);
extern void i2s0_tx_isr(void);
extern void i2s0_rx_isr(void);
extern void i2s0_isr(void);
extern void uart0_lon_isr(void);
extern void uart0_status_isr(void);
extern void uart0_error_isr(void);
extern void uart1_status_isr(void);
extern void uart1_error_isr(void);
extern void uart2_status_isr(void);
extern void uart2_error_isr(void);
extern void uart3_status_isr(void);
extern void uart3_error_isr(void);
extern void uart4_status_isr(void);
extern void uart4_error_isr(void);
extern void uart5_status_isr(void);
extern void uart5_error_isr(void);
extern void lpuart0_status_isr(void);
extern void adc0_isr(void);
extern void adc1_isr(void);
extern void cmp0_isr(void);
extern void cmp1_isr(void);
extern void cmp2_isr(void);
extern void cmp3_isr(void);
extern void ftm0_isr(void);
extern void ftm1_isr(void);
extern void ftm2_isr(void);
extern void ftm3_isr(void);
extern void tpm0_isr(void);
extern void tpm1_isr(void);
extern void tpm2_isr(void);
extern void cmt_isr(void);
extern void rtc_alarm_isr(void);
extern void rtc_seconds_isr(void);
extern void pit0_isr(void);
extern void pit1_isr(void);
extern void pit2_isr(void);
extern void pit3_isr(void);
extern void pit_isr(void);
extern void pdb_isr(void);
extern void usb_isr(void);
extern void usb_charge_isr(void);
extern void usbhs_isr(void);
extern void usbhs_phy_isr(void);
extern void dac0_isr(void);
extern void dac1_isr(void);
extern void tsi0_isr(void);
extern void mcg_isr(void);
extern void lptmr_isr(void);
extern void porta_isr(void);
extern void portb_isr(void);
extern void portc_isr(void);
extern void portd_isr(void);
extern void porte_isr(void);
extern void portcd_isr(void);
extern void software_isr(void);

extern void (* _VectorsRam[NVIC_NUM_INTERRUPTS+16])(void);
extern void (* const _VectorsFlash[NVIC_NUM_INTERRUPTS+16])(void);

#ifdef __cplusplus
}
#endif

#undef BEGIN_ENUM
#undef END_ENUM
#endif
