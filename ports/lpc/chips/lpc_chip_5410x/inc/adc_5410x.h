/*
 * @brief LPC5410X ADC driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2015
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

#ifndef __ADC_5410X_H_
#define __ADC_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup ADC_5410X CHIP:  LPC5410X A/D conversion driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/** Sequence index enumerations, used in various parts of the code for
    register indexing and sequencer selection */
typedef enum {
	ADC_SEQA_IDX = 0,
	ADC_SEQB_IDX
} ADC_SEQ_IDX_T;

/**
 * @brief ADC register block structure
 */
typedef struct {	/*!< ADCn Structure */
	__IO uint32_t CTRL;
	__O uint32_t RESERVED0;
	__IO uint32_t SEQ_CTRL[2];
	__IO uint32_t SEQ_GDAT[2];
	__IO uint32_t RESERVED1[2];
	__IO uint32_t DAT[12];
	__IO uint32_t THR_LOW[2];
	__IO uint32_t THR_HIGH[2];
	__IO uint32_t CHAN_THRSEL;
	__IO uint32_t INTEN;
	__IO uint32_t FLAGS;
	__IO uint32_t STARTUP;
	__IO uint32_t CALIBR;
} LPC_ADC_T;

/** Maximum sample rate in Hz (12-bit conversions) */
#define ADC_MAX_SAMPLE_RATE 48000000
#define ADC_MAX_CHANNEL_NUM 12

/**
 * @brief ADC register support bitfields and mask
 */
/** ADC Control register bit fields */
#define ADC_CR_CLKDIV_MASK      (0xFF << 0)				/*!< Mask for Clock divider value */
#define ADC_CR_CLKDIV_BITPOS    (0)						/*!< Bit position for Clock divider value */
#define ADC_CR_ASYNC_MODE       (1 << 8)				/*!< Asynchronous mode enable bit */
#define ADC_CR_RESOL(n)         ((n) << 9)				/*!< 2-bits, 6(0x0),8(0x1),10(0x2),12(0x3)-bit mode enable bit */
#define ADC_CR_LPWRMODEBIT      (1 << 10)				/*!< Low power mode enable bit */
#define ADC_CR_BYPASS           (1 << 11)				/*!< Bypass mode */
#define ADC_CR_TSAMP(n)         ((n) << 12)				/*!< 3-bits, 2.5(0x0),3.5(0x1),4.5(0x2),5.5(0x3),6.5(0x4),7.5(0x5),8.5(0x6),9.5(0x7) ADC clocks sampling time */
#define ADC_CR_CALMODEBIT       (1 << 30)				/*!< Self calibration cycle enable bit */
#define ADC_CR_BITACC(n)        ((((n) & 0x1) << 9))	/*!< 12-bit or 10-bit ADC accuracy */
#define ADC_CR_CLKDIV(n)        ((((n) & 0xFF) << 0))	/*!< The APB clock (PCLK) is divided by (this value plus one) to produce the clock for the A/D */
#define ADC_SAMPLE_RATE_CONFIG_MASK (ADC_CR_CLKDIV(0xFF) | ADC_CR_BITACC(0x01))

/** ADC Sequence Control register bit fields */
#define ADC_SEQ_CTRL_CHANSEL(n)             (1 << (n))		/*!< Channel select macro */
#define ADC_SEQ_CTRL_CHANSEL_BITPOS(n)      ((n) << 0)				/*!< Channel select macro */
#define ADC_SEQ_CTRL_CHANSEL_MASK           (0xFFF)				/*!< Channel select mask */

/**
 * @brief ADC sampling time bits 12, 13 and 14
 */
typedef enum _ADC_TSAMP_T {
	ADC_TSAMP_2CLK5 = 0,
	ADC_TSAMP_3CLK5,
	ADC_TSAMP_4CLK5,
	ADC_TSAMP_5CLK5,
	ADC_TSAMP_6CLK5,
	ADC_TSAMP_7CLK5,
	ADC_TSAMP_8CLK5,
	ADC_TSAMP_9CLK5,
} ADC_TSAMP_T;

/** SEQ_CTRL register bit fields */
// #define ADC_SEQ_CTRL_TRIGGER(n)          ((n)<<12)
#define ADC_SEQ_CTRL_HWTRIG_POLPOS       (1 << 18)		/*!< HW trigger polarity - positive edge */
#define ADC_SEQ_CTRL_HWTRIG_SYNCBYPASS   (1 << 19)		/*!< HW trigger bypass synchronisation */
#define ADC_SEQ_CTRL_START               (1 << 26)		/*!< Start conversion enable bit */
#define ADC_SEQ_CTRL_BURST               (1 << 27)		/*!< Repeated conversion enable bit */
#define ADC_SEQ_CTRL_SINGLESTEP          (1 << 28)		/*!< Single step enable bit */
#define ADC_SEQ_CTRL_LOWPRIO             (1 << 29)		/*!< High priority enable bit (regardless of name) */
#define ADC_SEQ_CTRL_MODE_EOS            (1 << 30)		/*!< Mode End of sequence enable bit */
#define ADC_SEQ_CTRL_SEQ_ENA             (1UL << 31)	/*!< Sequence enable bit */

/** ADC global data register bit fields */
#define ADC_SEQ_GDAT_RESULT_MASK         (0xFFF << 4)	/*!< Result value mask */
#define ADC_SEQ_GDAT_RESULT_BITPOS       (4)			/*!< Result start bit position */
#define ADC_SEQ_GDAT_THCMPRANGE_MASK     (0x3 << 16)	/*!< Comparion range mask */
#define ADC_SEQ_GDAT_THCMPRANGE_BITPOS   (16)			/*!< Comparison range bit position */
#define ADC_SEQ_GDAT_THCMPCROSS_MASK     (0x3 << 18)	/*!< Comparion cross mask */
#define ADC_SEQ_GDAT_THCMPCROSS_BITPOS   (18)			/*!< Comparison cross bit position */
#define ADC_SEQ_GDAT_CHAN_MASK           (0xF << 26)	/*!< Channel number mask */
#define ADC_SEQ_GDAT_CHAN_BITPOS         (26)			/*!< Channel number bit position */
#define ADC_SEQ_GDAT_OVERRUN             (1 << 30)		/*!< Overrun bit */
#define ADC_SEQ_GDAT_DATAVALID           (1UL << 31)	/*!< Data valid bit */

/** ADC Data register bit fields */
#define ADC_DR_RESULT_BITPOS       (4)			/*!< Result start bit position */
#define ADC_DR_RESULT(n)           ((((n) >> 4) & 0xFFF))	/*!< Macro for getting the ADC data value */
#define ADC_DR_THCMPRANGE_MASK     (0x3 << 16)			/*!< Comparion range mask */
#define ADC_DR_THCMPRANGE_BITPOS   (16)					/*!< Comparison range bit position */
#define ADC_DR_THCMPRANGE(n)       (((n) >> ADC_DR_THCMPRANGE_BITPOS) & 0x3)
#define ADC_DR_THCMPCROSS_MASK     (0x3 << 18)			/*!< Comparion cross mask */
#define ADC_DR_THCMPCROSS_BITPOS   (18)					/*!< Comparison cross bit position */
#define ADC_DR_THCMPCROSS(n)       (((n) >> ADC_DR_THCMPCROSS_BITPOS) & 0x3)
#define ADC_DR_CHAN_MASK           (0xF << 26)			/*!< Channel number mask */
#define ADC_DR_CHAN_BITPOS         (26)					/*!< Channel number bit position */
#define ADC_DR_CHANNEL(n)          (((n) >> ADC_DR_CHAN_BITPOS) & 0xF)	/*!< Channel number bit position */
#define ADC_DR_OVERRUN             (1 << 30)			/*!< Overrun bit */
#define ADC_DR_DATAVALID           (1UL << 31)			/*!< Data valid bit */
#define ADC_DR_DONE(n)             (((n) >> 31))

/** ADC low/high Threshold register bit fields */
#define ADC_THR_VAL_MASK            (0xFFF << 4)		/*!< Threshold value bit mask */
#define ADC_THR_VAL_POS             (4)					/*!< Threshold value bit position */

/** ADC Threshold select register bit fields */
#define ADC_THRSEL_CHAN_SEL_THR1(n) (1 << (n))			/*!< Select THR1 register for channel n */

/** ADC Interrupt Enable register bit fields */
#define ADC_INTEN_SEQA_ENABLE       (1 << 0)			/*!< Sequence A Interrupt enable bit */
#define ADC_INTEN_SEQB_ENABLE       (1 << 1)			/*!< Sequence B Interrupt enable bit */
#define ADC_INTEN_SEQN_ENABLE(seq)  (1 << (seq))		/*!< Sequence A/B Interrupt enable bit */
#define ADC_INTEN_OVRRUN_ENABLE     (1 << 2)			/*!< Overrun Interrupt enable bit */
#define ADC_INTEN_CMP_DISBALE       (0)					/*!< Disable comparison interrupt value */
#define ADC_INTEN_CMP_OUTSIDETH     (1)					/*!< Outside threshold interrupt value */
#define ADC_INTEN_CMP_CROSSTH       (2)					/*!< Crossing threshold interrupt value */
#define ADC_INTEN_CMP_MASK          (3)					/*!< Comparison interrupt value mask */
#define ADC_INTEN_CMP_ENABLE(isel, ch) (((isel) & ADC_INTEN_CMP_MASK) << ((2 * (ch)) + 3))	/*!< Interrupt selection for channel */

/** ADC Flags register bit fields */
#define ADC_FLAGS_THCMP_MASK(ch)    (1 << (ch))		/*!< Threshold comparison status for channel */
#define ADC_FLAGS_OVRRUN_MASK(ch)   (1 << (12 + (ch)))	/*!< Overrun status for channel */
#define ADC_FLAGS_SEQA_OVRRUN_MASK  (1 << 24)			/*!< Seq A Overrun status */
#define ADC_FLAGS_SEQB_OVRRUN_MASK  (1 << 25)			/*!< Seq B Overrun status */
#define ADC_FLAGS_SEQN_OVRRUN_MASK(seq) (1 << (24 + (seq)))	/*!< Seq A/B Overrun status */
#define ADC_FLAGS_SEQA_INT_MASK     (1 << 28)			/*!< Seq A Interrupt status */
#define ADC_FLAGS_SEQB_INT_MASK     (1 << 29)			/*!< Seq B Interrupt status */
#define ADC_FLAGS_SEQN_INT_MASK(seq) (1 << (28 + (seq)))/*!< Seq A/B Interrupt status */
#define ADC_FLAGS_THCMP_INT_MASK    (1 << 30)			/*!< Threshold comparison Interrupt status */
#define ADC_FLAGS_OVRRUN_INT_MASK   (1UL << 31)			/*!< Overrun Interrupt status */

/** ADC Startup register bit fields */
#define ADC_STARTUP_ENABLE       (0x1 << 0)
#define ADC_STARTUP_INIT         (0x1 << 1)

/* ADC Calibration register definition */
#define ADC_CALIB                       (0x1 << 0)
#define ADC_CALREQD                     (0x1 << 1)

/**
 * @brief	Set Interrupt bits (safe)
 * @param	pADC	: Pointer to ADC peripheral
 * @param	intMask	: Mask of bits to be set
 * @return	Nothing
 */
__STATIC_INLINE void Chip_ADC_SetIntBits(LPC_ADC_T *pADC, uint32_t intMask)
{
	/* Read and write values may not be the same, write 0 to undefined bits */
	pADC->INTEN = (pADC->INTEN | intMask) & 0x07FFFFFF;
}

/**
 * @brief	Clear Interrupt bits (safe)
 * @param	pADC	: Pointer to ADC peripheral
 * @param	intMask	: Mask of bits to be cleared
 * @return	Nothing
 */
__STATIC_INLINE void Chip_ADC_ClearIntBits(LPC_ADC_T *pADC, uint32_t intMask)
{
	/* Read and write values may not be the same, write 0 to undefined bits */
	pADC->INTEN = pADC->INTEN & (0x07FFFFFF & ~intMask);
}

/**
 * @brief	Set Threshold select bits (safe)
 * @param	pADC	: Pointer to ADC peripheral
 * @param	mask	: Mask of bits to be set
 * @return	Nothing
 */
__STATIC_INLINE void Chip_ADC_SetTHRSELBits(LPC_ADC_T *pADC, uint32_t mask)
{
	/* Read and write values may not be the same, write 0 to undefined bits */
	pADC->CHAN_THRSEL = (pADC->CHAN_THRSEL | mask) & 0x00000FFF;
}

/**
 * @brief	Clear Threshold select bits (safe)
 * @param	pADC	: Pointer to ADC peripheral
 * @param	mask	: Mask of bits to be cleared
 * @return	Nothing
 */
__STATIC_INLINE void Chip_ADC_ClearTHRSELBits(LPC_ADC_T *pADC, uint32_t mask)
{
	/* Read and write values may not be the same, write 0 to undefined bits */
	pADC->CHAN_THRSEL = pADC->CHAN_THRSEL & (0x00000FFF & ~mask);
}

/**
 * @brief	Initialize the ADC peripheral
 * @param	pADC	: The base of ADC peripheral on the chip
 * @param	flags	: ADC flags for init (ADC_CR_MODE10BIT and/or ADC_CR_LPWRMODEBIT)
 * @return	Nothing
 * @note	To select low-power ADC mode, enable the ADC_CR_LPWRMODEBIT flag.
 * To select 10-bit conversion mode, enable the ADC_CR_MODE10BIT flag.<br>
 * Example: Chip_ADC_Init(LPC_ADC, (ADC_CR_MODE10BIT | ADC_CR_LPWRMODEBIT));
 */
void Chip_ADC_Init(LPC_ADC_T *pADC, uint32_t flags);

/**
 * @brief	Shutdown ADC
 * @param	pADC	: The base of ADC peripheral on the chip
 * @return	Nothing
 * @note	Disables the ADC clocks and ADC power
 */
void Chip_ADC_DeInit(LPC_ADC_T *pADC);

/**
 * @brief	Set ADC divider
 * @param	pADC	: The base of ADC peripheral on the chip
 * @param	div		: ADC divider value to set minus 1
 * @return	Nothing
 * @note	The value is used as a divider to generate the ADC
 * clock rate from the ADC input clock. The ADC input clock is based
 * on the system clock. Valid values for this function are from 0 to 255
 * with 0=divide by 1, 1=divide by 2, 2=divide by 3, etc.<br>
 * Do not decrement this value by 1.<br>
 * To set the ADC clock rate to 1MHz, use the following function:<br>
 * Chip_ADC_SetDivider(LPC_ADC, (Chip_Clock_GetSystemClockRate() / 1000000) - 1);
 */
__STATIC_INLINE void Chip_ADC_SetDivider(LPC_ADC_T *pADC, uint8_t div)
{
	uint32_t temp;

	temp = pADC->CTRL & ~(ADC_CR_CLKDIV_MASK);
	pADC->CTRL = temp | (uint32_t) div;
}

/**
 * @brief	Set ADC clock rate
 * @param	pADC	: The base of ADC peripheral on the chip
 * @param	rate	: rate in Hz to set ADC clock to (maximum ADC_MAX_SAMPLE_RATE)
 * @return	Nothing
 */
void Chip_ADC_SetClockRate(LPC_ADC_T *pADC, uint32_t rate);

/**
 * @brief	Get ADC divider
 * @param	pADC	: The base of ADC peripheral on the chip
 * @return	the current ADC divider
 * @note	This function returns the divider that is used to generate the
 * ADC frequency. The returned value must be incremented by 1. The
 * frequency can be determined with the following function:<br>
 * adc_freq = Chip_Clock_GetSystemClockRate() / (Chip_ADC_GetDivider(LPC_ADC) + 1);
 */
__STATIC_INLINE uint8_t Chip_ADC_GetDivider(LPC_ADC_T *pADC)
{
	return pADC->CTRL & ADC_CR_CLKDIV_MASK;
}

/**
 * @brief	Calibrate ADC upon startup or wakeup after powerdown
 * @pre		ADC must be Initialized and Configured
 * @param	pADC		: Pointer to ADC peripheral
 * @return	Result of calibrarion operation
 * @retval	LPC_OK				Calibration is successful
 * @retval	ERR_ADC_NO_POWER	Unable to powerup ADC
 * @retval	ERR_TIME_OUT		Calibration operation timed-out
 */
uint32_t Chip_ADC_Calibration(LPC_ADC_T *pADC);

/**
 * @brief	Helper function for safely setting ADC sequencer register bits
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to set bits for
 * @param	bits		: Or'ed bits of a sequencer register to set
 * @return	Nothing
 * @note	This function will safely set the ADC sequencer register bits
 * while maintaining bits 20..25 as 0, regardless of the read state of those bits.
 */
__STATIC_INLINE void Chip_ADC_SetSequencerBits(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex, uint32_t bits)
{
	/* OR in passed bits */
	pADC->SEQ_CTRL[seqIndex] = (pADC->SEQ_CTRL[seqIndex] & ~(0x3F << 20)) | bits;
}

/**
 * @brief	Helper function for safely clearing ADC sequencer register bits
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to clear bits for
 * @param	bits		: Or'ed bits of a sequencer register to clear
 * @return	Nothing
 * @note	This function will safely clear the ADC sequencer register bits
 * while maintaining bits 20..25 as 0, regardless of the read state of those bits.
 */
__STATIC_INLINE void Chip_ADC_ClearSequencerBits(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex, uint32_t bits)
{
	/* Read sequencer register and mask off bits 20..25 */
	pADC->SEQ_CTRL[seqIndex] = pADC->SEQ_CTRL[seqIndex] & ~((0x3F << 20) | bits);
}

/**
 * @brief	Sets up ADC conversion sequencer A or B
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to setup
 * @param	options		: OR'ed Sequencer options to setup (see notes)
 * @return	Nothing
 * @note	Sets up sequencer options for a conversion sequence. This function
 * should be used to setup the selected channels for the sequence, the sequencer
 * trigger, the trigger polarity, synchronization bypass, priority, and mode. All
 * options are passed to the functions as a OR'ed list of values. This function will
 * disable/clear the sequencer start/burst/single step/enable if they are enabled.<br>
 * Select the channels by OR'ing in one or more ADC_SEQ_CTRL_CHANSEL(ch) values.<br>
 * Select the hardware trigger by OR'ing in one ADC_SEQ_CTRL_HWTRIG_* value.<br>
 * Select a positive edge hardware trigger by OR'ing in ADC_SEQ_CTRL_HWTRIG_POLPOS.<br>
 * Select trigger bypass synchronisation by OR'ing in ADC_SEQ_CTRL_HWTRIG_SYNCBYPASS.<br>
 * Select ADC single step on trigger/start by OR'ing in ADC_SEQ_CTRL_SINGLESTEP.<br>
 * Select higher priority conversion on the other sequencer by OR'ing in ADC_SEQ_CTRL_LOWPRIO.<br>
 * Select end of seqeuence instead of end of conversion interrupt by OR'ing in ADC_SEQ_CTRL_MODE_EOS.<br>
 * Example for setting up sequencer A (channels 0-2, trigger on high edge of PIO0_2, interrupt on end of sequence):<br>
 * Chip_ADC_SetupSequencer(LPC_ADC, ADC_SEQA_IDX, (
 *     ADC_SEQ_CTRL_CHANSEL(0) | ADC_SEQ_CTRL_CHANSEL(1) | ADC_SEQ_CTRL_CHANSEL(2) |
 *     ADC_SEQ_CTRL_HWTRIG_PIO0_2 | ADC_SEQ_CTRL_HWTRIG_POLPOS | ADC_SEQ_CTRL_MODE_EOS));
 */
__STATIC_INLINE void Chip_ADC_SetupSequencer(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex, uint32_t options)
{
	pADC->SEQ_CTRL[seqIndex] = options;
}

/**
 * @brief	Get sequenceX control register value
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to setup
 * @return	Sequencer control register value
 */
__STATIC_INLINE uint32_t Chip_ADC_GetSequencerCtrl(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex)
{
	return pADC->SEQ_CTRL[seqIndex];
}

/**
 * @brief	Enables a sequencer
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to enable
 * @return	Nothing
 */
__STATIC_INLINE void Chip_ADC_EnableSequencer(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex)
{
	Chip_ADC_SetSequencerBits(pADC, seqIndex, ADC_SEQ_CTRL_SEQ_ENA);
}

/**
 * @brief	Disables a sequencer
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to disable
 * @return	Nothing
 */
__STATIC_INLINE void Chip_ADC_DisableSequencer(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex)
{
	Chip_ADC_ClearSequencerBits(pADC, seqIndex, ADC_SEQ_CTRL_SEQ_ENA);
}

/**
 * @brief	Forces a sequencer trigger event (software trigger of ADC)
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to start
 * @return	Nothing
 * @note	This function sets the START bit for the sequencer to force a
 * single conversion sequence or a single step conversion. START and BURST bits can not
 * be set at the same time, thus, BURST bit will be cleared.
 */
__STATIC_INLINE void Chip_ADC_StartSequencer(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex)
{
	Chip_ADC_ClearSequencerBits(pADC, seqIndex, ADC_SEQ_CTRL_BURST);
	Chip_ADC_SetSequencerBits(pADC, seqIndex, ADC_SEQ_CTRL_START);
}

/**
 * @brief	Starts sequencer burst mode
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to start burst on
 * @return	Nothing
 * @note	This function sets the BURST bit for the sequencer to force
 * continuous conversion. Use Chip_ADC_StopBurstSequencer() to stop the
 * ADC burst sequence. START and BURST bits can not be set at the same time, thus,
 * START bit will be cleared.
 */
__STATIC_INLINE void Chip_ADC_StartBurstSequencer(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex)
{
	/* START and BURST bits can not be set at the same time. */
	Chip_ADC_ClearSequencerBits(pADC, seqIndex, ADC_SEQ_CTRL_START);
	Chip_ADC_SetSequencerBits(pADC, seqIndex, ADC_SEQ_CTRL_BURST);
}

/**
 * @brief	Stops sequencer burst mode
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to stop burst on
 * @return	Nothing
 */
__STATIC_INLINE void Chip_ADC_StopBurstSequencer(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex)
{
	Chip_ADC_ClearSequencerBits(pADC, seqIndex, ADC_SEQ_CTRL_BURST);
}

/** ADC sequence global data register threshold comparison range enumerations */
typedef enum {
	ADC_DR_THCMPRANGE_INRANGE,
	ADC_DR_THCMPRANGE_RESERVED,
	ADC_DR_THCMPRANGE_BELOW,
	ADC_DR_THCMPRANGE_ABOVE
} ADC_DR_THCMPRANGE_T;

/** ADC sequence global data register threshold comparison cross enumerations */
typedef enum {
	ADC_DR_THCMPCROSS_NOCROSS,
	ADC_DR_THCMPCROSS_RESERVED,
	ADC_DR_THCMPCROSS_DOWNWARD,
	ADC_DR_THCMPCROSS_UPWARD
} ADC_DR_THCMPCROSS_T;

/**
 * @brief	Read a ADC sequence global data register
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param	seqIndex	: Sequencer to read
 * @return	Current raw value of the ADC sequence A or B global data register
 * @note	This function returns the raw value of the data register and clears
 * the overrun and datavalid status for the register. Once this register is read,
 * the following functions can be used to parse the raw value:<br>
 * uint32_t adcDataRawValue = Chip_ADC_GetGlobalDataReg(LPC_ADC, ADC_SEQA_IDX); // Get raw value
 * uint32_t adcDataValue = ADC_DR_RESULT(adcDataRawValue); // Aligned and masked ADC data value
 * ADC_DR_THCMPRANGE_T adcRange = (ADC_DR_THCMPRANGE_T) ADC_DR_THCMPRANGE(adcDataRawValue); // Sample range compared to threshold low/high
 * ADC_DR_THCMPCROSS_T adcRange = (ADC_DR_THCMPCROSS_T) ADC_DR_THCMPCROSS(adcDataRawValue); // Sample cross compared to threshold low
 * uint32_t channel = ADC_DR_CHANNEL(adcDataRawValue); // ADC channel for this sample/data
 * bool adcDataOverrun = (bool) ((adcDataRawValue & ADC_DR_OVERRUN) != 0); // Data overrun flag
 * bool adcDataValid = (bool) ((adcDataRawValue & ADC_SEQ_GDAT_DATAVALID) != 0); // Data valid flag
 */
__STATIC_INLINE uint32_t Chip_ADC_GetGlobalDataReg(LPC_ADC_T *pADC, ADC_SEQ_IDX_T seqIndex)
{
	return pADC->SEQ_GDAT[seqIndex];
}

/**
 * @brief	Read a ADC data register
 * @param	pADC	: The base of ADC peripheral on the chip
 * @param	index	: Data register to read, 0-11
 * @return	Current raw value of the ADC data register
 * @note	This function returns the raw value of the data register and clears
 * the overrun and datavalid status for the register. Once this register is read,
 * the following functions can be used to parse the raw value:<br>
 * uint32_t adcDataRawValue = Chip_ADC_GetDataReg(LPC_ADC, ADC_MAX_CHANNEL_NUM); // Get raw value
 * uint32_t adcDataValue = ADC_DR_RESULT(adcDataRawValue); // Aligned and masked ADC data value
 * ADC_DR_THCMPRANGE_T adcRange = (ADC_DR_THCMPRANGE_T) ADC_DR_THCMPRANGE(adcDataRawValue); // Sample range compared to threshold low/high
 * ADC_DR_THCMPCROSS_T adcRange = (ADC_DR_THCMPCROSS_T) ADC_DR_THCMPCROSS(adcDataRawValue); // Sample cross compared to threshold low
 * uint32_t channel = ADC_DR_CHANNEL(adcDataRawValue); // ADC channel for this sample/data
 * bool adcDataOverrun = (bool) ((adcDataRawValue & ADC_DR_OVERRUN) != 0); // Data overrun flag
 * bool adcDataValid = (bool) ((adcDataRawValue & ADC_SEQ_GDAT_DATAVALID) != 0); // Data valid flag
 */
__STATIC_INLINE uint32_t Chip_ADC_GetDataReg(LPC_ADC_T *pADC, uint8_t index)
{
	return pADC->DAT[index];
}

/**
 * @brief	Set Threshold low value in ADC
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param   thrnum      : Threshold register value (1 for threshold register 1, 0 for threshold register 0)
 * @param   value       : Threshold low data value (should be 12-bit value)
 * @return	None
 */
__STATIC_INLINE void Chip_ADC_SetThrLowValue(LPC_ADC_T *pADC, uint8_t thrnum, uint16_t value)
{
	pADC->THR_LOW[thrnum] = (((uint32_t) value) << ADC_THR_VAL_POS);
}

/**
 * @brief	Set Threshold high value in ADC
 * @param	pADC	: The base of ADC peripheral on the chip
 * @param   thrnum	: Threshold register value (1 for threshold register 1, 0 for threshold register 0)
 * @param   value	: Threshold high data value (should be 12-bit value)
 * @return	None
 */
__STATIC_INLINE void Chip_ADC_SetThrHighValue(LPC_ADC_T *pADC, uint8_t thrnum, uint16_t value)
{
	pADC->THR_HIGH[thrnum] = (((uint32_t) value) << ADC_THR_VAL_POS);
}

/**
 * @brief	Select threshold 0 values for comparison for selected channels
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param   channels	: An OR'ed value of one or more ADC_THRSEL_CHAN_SEL_THR1(ch) values
 * @return	None
 * @note	Select multiple channels to use the threshold 0 comparison.<br>
 * Example:<br>
 * Chip_ADC_SelectTH0Channels(LPC_ADC, (ADC_THRSEL_CHAN_SEL_THR1(1) | ADC_THRSEL_CHAN_SEL_THR1(2))); // Selects channels 1 and 2 for threshold 0
 */
__STATIC_INLINE void Chip_ADC_SelectTH0Channels(LPC_ADC_T *pADC, uint32_t channels)
{
	Chip_ADC_ClearTHRSELBits(pADC, channels);
}

/**
 * @brief	Select threshold 1 value for comparison for selected channels
 * @param	pADC		: The base of ADC peripheral on the chip
 * @param   channels	: An OR'ed value of one or more ADC_THRSEL_CHAN_SEL_THR1(ch) values
 * @return	None
 * @note	Select multiple channels to use the 1 threshold comparison.<br>
 * Example:<br>
 * Chip_ADC_SelectTH1Channels(LPC_ADC, (ADC_THRSEL_CHAN_SEL_THR1(4) | ADC_THRSEL_CHAN_SEL_THR1(5))); // Selects channels 4 and 5 for 1 threshold
 */
__STATIC_INLINE void Chip_ADC_SelectTH1Channels(LPC_ADC_T *pADC, uint32_t channels)
{
	Chip_ADC_SetTHRSELBits(pADC, channels);
}

/**
 * @brief	Enable interrupts in ADC (sequencers A/B and overrun)
 * @param	pADC	: The base of ADC peripheral on the chip
 * @param	intMask	: Interrupt values to be enabled (see notes)
 * @return	None
 * @note	Select one or more OR'ed values of ADC_INTEN_SEQA_ENABLE,
 * ADC_INTEN_SEQB_ENABLE, and ADC_INTEN_OVRRUN_ENABLE to enable the
 * specific ADC interrupts.
 */
__STATIC_INLINE void Chip_ADC_EnableInt(LPC_ADC_T *pADC, uint32_t intMask)
{
	Chip_ADC_SetIntBits(pADC, intMask);
}

/**
 * @brief	Disable interrupts in ADC (sequencers A/B and overrun)
 * @param	pADC	: The base of ADC peripheral on the chip
 * @param	intMask	: Interrupt values to be disabled (see notes)
 * @return	None
 * @note	Select one or more OR'ed values of ADC_INTEN_SEQA_ENABLE,
 * ADC_INTEN_SEQB_ENABLE, and ADC_INTEN_OVRRUN_ENABLE to disable the
 * specific ADC interrupts.
 */
__STATIC_INLINE void Chip_ADC_DisableInt(LPC_ADC_T *pADC, uint32_t intMask)
{
	Chip_ADC_ClearIntBits(pADC, intMask);
}

/** Threshold interrupt event options */
typedef enum {
	ADC_INTEN_THCMP_DISABLE,
	ADC_INTEN_THCMP_OUTSIDE,
	ADC_INTEN_THCMP_CROSSING,
} ADC_INTEN_THCMP_T;

/**
 * @brief	Enable a threshold event interrupt in ADC
 * @param	pADC	: The base of ADC peripheral on the chip
 * @param	ch		: ADC channel to set threshold inetrrupt for, 1-8
 * @param	thInt	: Selected threshold interrupt type
 * @return	None
 */
__STATIC_INLINE void Chip_ADC_SetThresholdInt(LPC_ADC_T *pADC, uint8_t ch, ADC_INTEN_THCMP_T thInt)
{
	int shiftIndex = 3 + (ch * 2);

	/* Clear current bits first */
	Chip_ADC_ClearIntBits(pADC, (ADC_INTEN_CMP_MASK << shiftIndex));

	/* Set new threshold interrupt type */
	Chip_ADC_SetIntBits(pADC, ((uint32_t) thInt << shiftIndex));
}

/**
 * @brief	Get flags register in ADC
 * @param	pADC	: The base of ADC peripheral on the chip
 * @return  Flags register value (ORed ADC_FLAG* values)
 * @note	Mask the return value of this function with the ADC_FLAGS_*
 * definitions to determine the overall ADC interrupt events.<br>
 * Example:<br>
 * if (Chip_ADC_GetFlags(LPC_ADC) & ADC_FLAGS_THCMP_MASK(3) // Check of threshold comp status for ADC channel 3
 */
__STATIC_INLINE uint32_t Chip_ADC_GetFlags(LPC_ADC_T *pADC)
{
	return pADC->FLAGS;
}

/**
 * @brief	Clear flags register in ADC
 * @param	pADC	: The base of ADC peripheral on the chip
 * @param	flags	: An Or'ed values of ADC_FLAGS_* values to clear
 * @return  Flags register value (ORed ADC_FLAG* values)
 */
__STATIC_INLINE void Chip_ADC_ClearFlags(LPC_ADC_T *pADC, uint32_t flags)
{
	pADC->FLAGS = flags;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_5410X_H_ */
