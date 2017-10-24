/*
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __SCU_18XX_43XX_H_
#define __SCU_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SCU_18XX_43XX CHIP: LPC18xx/43xx SCU Driver (configures pin functions)
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */


/**
 * @brief Array of pin definitions passed to Chip_SCU_SetPinMuxing() must be in this format
 */
typedef struct {
	uint8_t pingrp;		/* Pin group */
	uint8_t pinnum;		/* Pin number */
	uint16_t modefunc;	/* Pin mode and function for SCU */
} PINMUX_GRP_T;

/**
 * @brief System Control Unit register block
 */
typedef struct {
	__IO uint32_t  SFSP[16][32];
	__I  uint32_t  RESERVED0[256];
	__IO uint32_t  SFSCLK[4];			/*!< Pin configuration register for pins CLK0-3 */
	__I  uint32_t  RESERVED16[28];
	__IO uint32_t  SFSUSB;				/*!< Pin configuration register for USB */
	__IO uint32_t  SFSI2C0;				/*!< Pin configuration register for I2C0-bus pins */
	__IO uint32_t  ENAIO[3];			/*!< Analog function select registerS */
	__I  uint32_t  RESERVED17[27];
	__IO uint32_t  EMCDELAYCLK;			/*!< EMC clock delay register */
	__I  uint32_t  RESERVED18[63];
	__IO uint32_t  PINTSEL[2];			/*!< Pin interrupt select register for pin int 0 to 3 index 0, 4 to 7 index 1. */
} LPC_SCU_T;

/**
 * SCU function and mode selection definitions
 * See the User Manual for specific modes and functions supoprted by the
 * various LPC18xx/43xx devices. Functionality can vary per device.
 */
#define SCU_MODE_PULLUP            (0x0 << 3)		/*!< Enable pull-up resistor at pad */
#define SCU_MODE_REPEATER          (0x1 << 3)		/*!< Enable pull-down and pull-up resistor at resistor at pad (repeater mode) */
#define SCU_MODE_INACT             (0x2 << 3)		/*!< Disable pull-down and pull-up resistor at resistor at pad */
#define SCU_MODE_PULLDOWN          (0x3 << 3)		/*!< Enable pull-down resistor at pad */
#define SCU_MODE_HIGHSPEEDSLEW_EN  (0x1 << 5)		/*!< Enable high-speed slew */
#define SCU_MODE_INBUFF_EN         (0x1 << 6)		/*!< Enable Input buffer */
#define SCU_MODE_ZIF_DIS           (0x1 << 7)		/*!< Disable input glitch filter */
#define SCU_MODE_4MA_DRIVESTR      (0x0 << 8)		/*!< Normal drive: 4mA drive strength */
#define SCU_MODE_8MA_DRIVESTR      (0x1 << 8)		/*!< Medium drive: 8mA drive strength */
#define SCU_MODE_14MA_DRIVESTR     (0x2 << 8)		/*!< High drive: 14mA drive strength */
#define SCU_MODE_20MA_DRIVESTR     (0x3 << 8)		/*!< Ultra high- drive: 20mA drive strength */
#define SCU_MODE_FUNC0             0x0				/*!< Selects pin function 0 */
#define SCU_MODE_FUNC1             0x1				/*!< Selects pin function 1 */
#define SCU_MODE_FUNC2             0x2				/*!< Selects pin function 2 */
#define SCU_MODE_FUNC3             0x3				/*!< Selects pin function 3 */
#define SCU_MODE_FUNC4             0x4				/*!< Selects pin function 4 */
#define SCU_MODE_FUNC5             0x5				/*!< Selects pin function 5 */
#define SCU_MODE_FUNC6             0x6				/*!< Selects pin function 6 */
#define SCU_MODE_FUNC7             0x7				/*!< Selects pin function 7 */
#define SCU_PINIO_FAST             (SCU_MODE_INACT | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)

/**
 * SCU function and mode selection definitions (old)
 * For backwards compatibility.
 */
#define MD_PUP						(0x0 << 3)		/** Enable pull-up resistor at pad */
#define MD_BUK						(0x1 << 3)		/** Enable pull-down and pull-up resistor at resistor at pad (repeater mode) */
#define MD_PLN						(0x2 << 3)		/** Disable pull-down and pull-up resistor at resistor at pad */
#define MD_PDN						(0x3 << 3)		/** Enable pull-down resistor at pad */
#define MD_EHS						(0x1 << 5)		/** Enable fast slew rate */
#define MD_EZI						(0x1 << 6)		/** Input buffer enable */
#define MD_ZI						(0x1 << 7)		/** Disable input glitch filter */
#define MD_EHD0						(0x1 << 8)		/** EHD driver strength low bit */
#define MD_EHD1						(0x1 << 8)		/** EHD driver strength high bit */
#define MD_PLN_FAST					(MD_PLN | MD_EZI | MD_ZI | MD_EHS)
#define I2C0_STANDARD_FAST_MODE		(1 << 3 | 1 << 11)	/** Pin configuration for STANDARD/FAST mode I2C */
#define I2C0_FAST_MODE_PLUS			(2 << 1 | 1 << 3 | 1 << 7 | 1 << 10 | 1 << 11)	/** Pin configuration for Fast-mode Plus I2C */
#define FUNC0						0x0				/** Pin function 0 */
#define FUNC1						0x1				/** Pin function 1 */
#define FUNC2						0x2				/** Pin function 2 */
#define FUNC3						0x3				/** Pin function 3 */
#define FUNC4						0x4				/** Pin function 4 */
#define FUNC5						0x5				/** Pin function 5 */
#define FUNC6						0x6				/** Pin function 6 */
#define FUNC7						0x7				/** Pin function 7 */

#define PORT_OFFSET					0x80			/** Port offset definition */
#define PIN_OFFSET					0x04			/** Pin offset definition */

/** Returns the SFSP register address in the SCU for a pin and port, recommend using (*(volatile int *) &LPC_SCU->SFSP[po][pi];) */
#define LPC_SCU_PIN(LPC_SCU_BASE, po, pi) (*(volatile int *) ((LPC_SCU_BASE) + ((po) * 0x80) + ((pi) * 0x4))

/** Returns the address in the SCU for a SFSCLK clock register, recommend using (*(volatile int *) &LPC_SCU->SFSCLK[c];) */
#define LPC_SCU_CLK(LPC_SCU_BASE, c) (*(volatile int *) ((LPC_SCU_BASE) +0xC00 + ((c) * 0x4)))

/**
 * @brief	Sets I/O Control pin mux
 * @param	port		: Port number, should be: 0..15
 * @param	pin			: Pin number, should be: 0..31
 * @param	modefunc	: OR'ed values or type SCU_MODE_*
 * @return	Nothing
 * @note	Do not use for clock pins (SFSCLK0 .. SFSCLK4). Use
 * Chip_SCU_ClockPinMux() function for SFSCLKx clock pins.
 */
STATIC INLINE void Chip_SCU_PinMuxSet(uint8_t port, uint8_t pin, uint16_t modefunc)
{
	LPC_SCU->SFSP[port][pin] = modefunc;
}

/**
 * @brief	Configure pin function
 * @param	port	: Port number, should be: 0..15
 * @param	pin		: Pin number, should be: 0..31
 * @param	mode	: OR'ed values or type SCU_MODE_*
 * @param	func	: Pin function, value of type SCU_MODE_FUNC0 to SCU_MODE_FUNC7
 * @return	Nothing
 * @note	Do not use for clock pins (SFSCLK0 .. SFSCLK4). Use
 * Chip_SCU_ClockPinMux() function for SFSCLKx clock pins.
 */
STATIC INLINE void Chip_SCU_PinMux(uint8_t port, uint8_t pin, uint16_t mode, uint8_t func)
{
	Chip_SCU_PinMuxSet(port, pin, (mode | (uint16_t) func));
}

/**
 * @brief	Configure clock pin function (pins SFSCLKx)
 * @param	clknum	: Clock pin number, should be: 0..3
 * @param	modefunc	: OR'ed values or type SCU_MODE_*
 * @return	Nothing
 */
STATIC INLINE void Chip_SCU_ClockPinMuxSet(uint8_t clknum, uint16_t modefunc)
{
	LPC_SCU->SFSCLK[clknum] = (uint32_t) modefunc;
}

/**
 * @brief	Configure clock pin function (pins SFSCLKx)
 * @param	clknum	: Clock pin number, should be: 0..3
 * @param	mode	: OR'ed values or type SCU_MODE_*
 * @param	func	: Pin function, value of type SCU_MODE_FUNC0 to SCU_MODE_FUNC7
 * @return	Nothing
 */
STATIC INLINE void Chip_SCU_ClockPinMux(uint8_t clknum, uint16_t mode, uint8_t func)
{
	LPC_SCU->SFSCLK[clknum] = ((uint32_t) mode | (uint32_t) func);
}

/**
 * @brief	GPIO Interrupt Pin Select
 * @param	PortSel	: GPIO PINTSEL interrupt, should be: 0 to 7
 * @param	PortNum	: GPIO port number interrupt, should be: 0 to 7
 * @param	PinNum	: GPIO pin number Interrupt , should be: 0 to 31
 * @return	Nothing
 */
STATIC INLINE void Chip_SCU_GPIOIntPinSel(uint8_t PortSel, uint8_t PortNum, uint8_t PinNum)
{
	int32_t of = (PortSel & 3) << 3;
	uint32_t val = (((PortNum & 0x7) << 5) | (PinNum & 0x1F)) << of;
	LPC_SCU->PINTSEL[PortSel >> 2] = (LPC_SCU->PINTSEL[PortSel >> 2] & ~(0xFF << of)) | val;
}

/**
 * @brief	I2C Pin Configuration
 * @param	I2C0Mode	: I2C0 mode, should be:
 *                  - I2C0_STANDARD_FAST_MODE: Standard/Fast mode transmit
 *                  - I2C0_FAST_MODE_PLUS: Fast-mode Plus transmit
 * @return	Nothing
 */
STATIC INLINE void Chip_SCU_I2C0PinConfig(uint32_t I2C0Mode)
{
	LPC_SCU->SFSI2C0 = I2C0Mode;
}

/**
 * @brief	ADC Pin Configuration
 * @param	ADC_ID	: ADC number
 * @param	channel	: ADC channel
 * @return	Nothing
 */
STATIC INLINE void Chip_SCU_ADC_Channel_Config(uint32_t ADC_ID, uint8_t channel)
{
	LPC_SCU->ENAIO[ADC_ID] |= 1UL << channel;
}

/**
 * @brief	DAC Pin Configuration
 * @return	Nothing
 */
STATIC INLINE void Chip_SCU_DAC_Analog_Config(void)
{
	/*Enable analog function DAC on pin P4_4*/
	LPC_SCU->ENAIO[2] |= 1;
}

/**
 * @brief	Set all I/O Control pin muxing
 * @param	pinArray    : Pointer to array of pin mux selections
 * @param	arrayLength : Number of entries in pinArray
 * @return	Nothing
 */
STATIC INLINE void Chip_SCU_SetPinMuxing(const PINMUX_GRP_T *pinArray, uint32_t arrayLength)
{
	uint32_t ix;
	for (ix = 0; ix < arrayLength; ix++ ) {
		Chip_SCU_PinMuxSet(pinArray[ix].pingrp, pinArray[ix].pinnum, pinArray[ix].modefunc);
	}
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SCU_18XX_43XX_H_ */






