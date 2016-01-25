/*
 * @brief  LPC18xx/43xx A/D conversion driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
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
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __ADC_18XX_43XX_H_
#define __ADC_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup ADC_18XX_43XX CHIP:  LPC18xx/43xx A/D conversion driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

#define ADC_ACC_10BITS

#define ADC_MAX_SAMPLE_RATE 400000

/**
 * @brief 10 or 12-bit ADC register block structure
 */
typedef struct {					/*!< ADCn Structure */
	__IO uint32_t CR;				/*!< A/D Control Register. The AD0CR register must be written to select the operating mode before A/D conversion can occur. */
	__I  uint32_t GDR;				/*!< A/D Global Data Register. Contains the result of the most recent A/D conversion. */
	__I  uint32_t RESERVED0;
	__IO uint32_t INTEN;			/*!< A/D Interrupt Enable Register. This register contains enable bits that allow the DONE flag of each A/D channel to be included or excluded from contributing to the generation of an A/D interrupt. */
	__I  uint32_t DR[8];			/*!< A/D Channel Data Register. This register contains the result of the most recent conversion completed on channel n. */
	__I  uint32_t STAT;				/*!< A/D Status Register. This register contains DONE and OVERRUN flags for all of the A/D channels, as well as the A/D interrupt flag. */
} LPC_ADC_T;

/**
 * @brief ADC register support bitfields and mask
 */

#define ADC_DR_RESULT(n)        ((((n) >> 6) & 0x3FF))	/*!< Mask for getting the 10 bits ADC data read value */
#define ADC_CR_BITACC(n)        ((((n) & 0x7) << 17))	/*!< Number of ADC accuracy bits */
#define ADC_DR_DONE(n)          (((n) >> 31))			/*!< Mask for reading the ADC done status */
#define ADC_DR_OVERRUN(n)       ((((n) >> 30) & (1UL)))	/*!< Mask for reading the ADC overrun status */
#define ADC_CR_CH_SEL(n)        ((1UL << (n)))			/*!< Selects which of the AD0.0:7 pins is (are) to be sampled and converted */
#define ADC_CR_CLKDIV(n)        ((((n) & 0xFF) << 8))	/*!< The APB clock (PCLK) is divided by (this value plus one) to produce the clock for the A/D */
#define ADC_CR_BURST            ((1UL << 16))			/*!< Repeated conversions A/D enable bit */
#define ADC_CR_PDN              ((1UL << 21))			/*!< ADC convert is operational */
#define ADC_CR_START_MASK       ((7UL << 24))			/*!< ADC start mask bits */
#define ADC_CR_START_MODE_SEL(SEL)  ((SEL << 24))		/*!< Select Start Mode */
#define ADC_CR_START_NOW        ((1UL << 24))			/*!< Start conversion now */
#define ADC_CR_START_CTOUT15    ((2UL << 24))			/*!< Start conversion when the edge selected by bit 27 occurs on CTOUT_15 */
#define ADC_CR_START_CTOUT8     ((3UL << 24))			/*!< Start conversion when the edge selected by bit 27 occurs on CTOUT_8 */
#define ADC_CR_START_ADCTRIG0   ((4UL << 24))			/*!< Start conversion when the edge selected by bit 27 occurs on ADCTRIG0 */
#define ADC_CR_START_ADCTRIG1   ((5UL << 24))			/*!< Start conversion when the edge selected by bit 27 occurs on ADCTRIG1 */
#define ADC_CR_START_MCOA2      ((6UL << 24))			/*!< Start conversion when the edge selected by bit 27 occurs on Motocon PWM output MCOA2 */
#define ADC_CR_EDGE             ((1UL << 27))			/*!< Start conversion on a falling edge on the selected CAP/MAT signal */
#define ADC_SAMPLE_RATE_CONFIG_MASK         (ADC_CR_CLKDIV(0xFF) | ADC_CR_BITACC(0x07))

/**
 * @brief	ADC status register used for IP drivers
 */
typedef enum IP_ADC_STATUS {
	ADC_DR_DONE_STAT,	/*!< ADC data register staus */
	ADC_DR_OVERRUN_STAT,/*!< ADC data overrun staus */
	ADC_DR_ADINT_STAT	/*!< ADC interrupt status */
} ADC_STATUS_T;

/** The channels on one ADC peripheral*/
typedef enum CHIP_ADC_CHANNEL {
	ADC_CH0 = 0,	/**< ADC channel 0 */
	ADC_CH1,		/**< ADC channel 1 */
	ADC_CH2,		/**< ADC channel 2 */
	ADC_CH3,		/**< ADC channel 3 */
	ADC_CH4,		/**< ADC channel 4 */
	ADC_CH5,		/**< ADC channel 5 */
	ADC_CH6,		/**< ADC channel 6 */
	ADC_CH7,		/**< ADC channel 7 */
} ADC_CHANNEL_T;

/** The number of bits of accuracy of the result in the LS bits of ADDR*/
typedef enum CHIP_ADC_RESOLUTION {
	ADC_10BITS = 0,		/**< ADC 10 bits */
	ADC_9BITS,			/**< ADC 9 bits  */
	ADC_8BITS,			/**< ADC 8 bits  */
	ADC_7BITS,			/**< ADC 7 bits  */
	ADC_6BITS,			/**< ADC 6 bits  */
	ADC_5BITS,			/**< ADC 5 bits  */
	ADC_4BITS,			/**< ADC 4 bits  */
	ADC_3BITS,			/**< ADC 3 bits  */
} ADC_RESOLUTION_T;

/** Edge configuration, which controls rising or falling edge on the selected signal for the start of a conversion */
typedef enum CHIP_ADC_EDGE_CFG {
	ADC_TRIGGERMODE_RISING = 0,		/**< Trigger event: rising edge */
	ADC_TRIGGERMODE_FALLING,		/**< Trigger event: falling edge */
} ADC_EDGE_CFG_T;

/** Start mode, which controls the start of an A/D conversion when the BURST bit is 0. */
typedef enum CHIP_ADC_START_MODE {
	ADC_NO_START = 0,
	ADC_START_NOW,			/*!< Start conversion now */
	ADC_START_ON_CTOUT15,	/*!< Start conversion when the edge selected by bit 27 occurs on CTOUT_15 */
	ADC_START_ON_CTOUT8,	/*!< Start conversion when the edge selected by bit 27 occurs on CTOUT_8 */
	ADC_START_ON_ADCTRIG0,	/*!< Start conversion when the edge selected by bit 27 occurs on ADCTRIG0 */
	ADC_START_ON_ADCTRIG1,	/*!< Start conversion when the edge selected by bit 27 occurs on ADCTRIG1 */
	ADC_START_ON_MCOA2		/*!< Start conversion when the edge selected by bit 27 occurs on Motocon PWM output MCOA2 */
} ADC_START_MODE_T;

/** Clock setup structure for ADC controller passed to the initialize function */
typedef struct {
	uint32_t adcRate;		/*!< ADC rate */
	uint8_t  bitsAccuracy;	/*!< ADC bit accuracy */
	bool     burstMode;		/*!< ADC Burt Mode */
} ADC_CLOCK_SETUP_T;

/**
 * @brief	Initialize the ADC peripheral and the ADC setup structure to default value
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	ADCSetup	: ADC setup structure to be set
 * @return	Nothing
 * @note	Default setting for ADC is 400kHz - 10bits
 */
void Chip_ADC_Init(LPC_ADC_T *pADC, ADC_CLOCK_SETUP_T *ADCSetup);

/**
 * @brief	Shutdown ADC
 * @param	pADC	: The base of ADC peripheral on the chip
 * @return	Nothing
 */
void Chip_ADC_DeInit(LPC_ADC_T *pADC);

/**
 * @brief	Read the ADC value from a channel
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	channel		: ADC channel to read
 * @param	data		: Pointer to where to put data
 * @return	SUCCESS or ERROR if no conversion is ready
 */
Status Chip_ADC_ReadValue(LPC_ADC_T *pADC, uint8_t channel, uint16_t *data);

/**
 * @brief	Read the ADC value and convert it to 8bits value
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	channel:	selected channel
 * @param	data		: Storage for data
 * @return	Status	: ERROR or SUCCESS
 */
Status Chip_ADC_ReadByte(LPC_ADC_T *pADC, ADC_CHANNEL_T channel, uint8_t *data);

/**
 * @brief	Read the ADC channel status
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	channel		: ADC channel to read
 * @param	StatusType	: Status type of ADC_DR_*
 * @return	SET or RESET
 */
FlagStatus Chip_ADC_ReadStatus(LPC_ADC_T *pADC, uint8_t channel, uint32_t StatusType);

/**
 * @brief	Enable/Disable interrupt for ADC channel
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	channel		: ADC channel to read
 * @param	NewState	: New state, ENABLE or DISABLE
 * @return	SET or RESET
 */
void Chip_ADC_Int_SetChannelCmd(LPC_ADC_T *pADC, uint8_t channel, FunctionalState NewState);

/**
 * @brief	Enable/Disable global interrupt for ADC channel
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	NewState	: New state, ENABLE or DISABLE
 * @return	Nothing
 */
STATIC INLINE void Chip_ADC_Int_SetGlobalCmd(LPC_ADC_T *pADC, FunctionalState NewState)
{
	Chip_ADC_Int_SetChannelCmd(pADC, 8, NewState);
}

/**
 * @brief	Select the mode starting the AD conversion
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	mode		: Stating mode, should be :
 *							- ADC_NO_START				: Must be set for Burst mode
 *							- ADC_START_NOW				: Start conversion now
 *							- ADC_START_ON_CTOUT15		: Start conversion when the edge selected by bit 27 occurs on CTOUT_15
 *							- ADC_START_ON_CTOUT8		: Start conversion when the edge selected by bit 27 occurs on CTOUT_8
 *							- ADC_START_ON_ADCTRIG0		: Start conversion when the edge selected by bit 27 occurs on ADCTRIG0
 *							- ADC_START_ON_ADCTRIG1		: Start conversion when the edge selected by bit 27 occurs on ADCTRIG1
 *							- ADC_START_ON_MCOA2		: Start conversion when the edge selected by bit 27 occurs on Motocon PWM output MCOA2
 * @param	EdgeOption	: Stating Edge Condition, should be :
 *							- ADC_TRIGGERMODE_RISING	: Trigger event on rising edge
 *							- ADC_TRIGGERMODE_FALLING	: Trigger event on falling edge
 * @return	Nothing
 */
void Chip_ADC_SetStartMode(LPC_ADC_T *pADC, ADC_START_MODE_T mode, ADC_EDGE_CFG_T EdgeOption);

/**
 * @brief	Set the ADC Sample rate
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	ADCSetup	: ADC setup structure to be modified
 * @param	rate		: Sample rate, should be set so the clock for A/D converter is less than or equal to 4.5MHz.
 * @return	Nothing
 */
void Chip_ADC_SetSampleRate(LPC_ADC_T *pADC, ADC_CLOCK_SETUP_T *ADCSetup, uint32_t rate);

/**
 * @brief	Set the ADC accuracy bits
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	ADCSetup	: ADC setup structure to be modified
 * @param	resolution	: The resolution, should be ADC_10BITS -> ADC_3BITS
 * @return	Nothing
 */
void Chip_ADC_SetResolution(LPC_ADC_T *pADC, ADC_CLOCK_SETUP_T *ADCSetup, ADC_RESOLUTION_T resolution);

/**
 * @brief	Enable or disable the ADC channel on ADC peripheral
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	channel		: Channel to be enable or disable
 * @param	NewState	: New state, should be:
 *								- ENABLE
 *								- DISABLE
 * @return	Nothing
 */
void Chip_ADC_EnableChannel(LPC_ADC_T *pADC, ADC_CHANNEL_T channel, FunctionalState NewState);

/**
 * @brief	Enable burst mode
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	NewState	: New state, should be:
 *							- ENABLE
 *							- DISABLE
 * @return	Nothing
 */
void Chip_ADC_SetBurstCmd(LPC_ADC_T *pADC, FunctionalState NewState);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_18XX_43XX_H_ */
