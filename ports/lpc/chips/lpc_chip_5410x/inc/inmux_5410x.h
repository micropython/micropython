/*
 * @brief LPC5410X Input Mux Registers and Driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
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

#ifndef __INMUX_5410X_H_
#define __INMUX_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup INMUX_5410X CHIP: LPC5410X Input Mux Registers and Driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/**
 * @brief LPC5410X Input Mux Register Block Structure
 */
typedef struct {						/*!< INMUX Structure */
	__IO uint32_t RESERVED0[6];
	__I  uint32_t RESERVED1[42];
	__IO uint32_t PINTSEL[8];			/*!< Pin interrupt select registers */
	__IO uint32_t DMA_ITRIG_INMUX[22];	/*!< Input mux register for DMA trigger inputs */
	__I  uint32_t RESERVED2[2];
	__IO uint32_t DMA_OTRIG_INMUX[4];	/*!< Input mux register for DMA trigger inputs */
	__I  uint32_t RESERVED3[4];
	__IO uint32_t FREQMEAS_REF;			/*!< Clock selection for frequency measurement ref clock */
	__IO uint32_t FREQMEAS_TARGET;		/*!< Clock selection for frequency measurement target clock */
} LPC_INMUX_T;

/**
 * @brief	GPIO Pin Interrupt Pin Select (sets PINTSEL register)
 * @param	pintSel	: GPIO PINTSEL interrupt, should be: 0 to 7
 * @param	portNum	: GPIO port number interrupt, should be: 0 to 1
 * @param	pinNum	: GPIO pin number Interrupt, should be: 0 to 31
 * @return	Nothing
 */
__STATIC_INLINE void Chip_INMUX_PinIntSel(uint8_t pintSel, uint8_t portNum, uint8_t pinNum)
{
	LPC_INMUX->PINTSEL[pintSel] = (portNum * 32) + pinNum;
}

/* DMA triggers that can mapped to DMA channels */
typedef enum {
	DMATRIG_ADC0_SEQA_IRQ = 0,			/*!< ADC0 sequencer A interrupt as trigger */
	DMATRIG_ADC0_SEQB_IRQ,				/*!< ADC0 sequencer B interrupt as trigger */
	DMATRIG_SCT0_DMA0,					/*!< SCT 0, DMA 0 as trigger */
	DMATRIG_SCT0_DMA1,					/*!< SCT 1, DMA 1 as trigger */
	DMATRIG_TIMER0_MATCH0,				/*!< Timer 0, match 0 trigger */
	DMATRIG_TIMER0_MATCH1,				/*!< Timer 0, match 1 trigger */
	DMATRIG_TIMER1_MATCH0,				/*!< Timer 1, match 0 trigger */
	DMATRIG_TIMER1_MATCH1,				/*!< Timer 1, match 1 trigger */
	DMATRIG_TIMER2_MATCH0,				/*!< Timer 2, match 0 trigger */
	DMATRIG_TIMER2_MATCH1,				/*!< Timer 2, match 1 trigger */
	DMATRIG_TIMER3_MATCH0,				/*!< Timer 3, match 0 trigger */
	DMATRIG_TIMER3_MATCH1,				/*!< Timer 3, match 1 trigger */
	DMATRIG_TIMER4_MATCH0,				/*!< Timer 4, match 0 trigger */
	DMATRIG_TIMER4_MATCH1,				/*!< Timer 4, match 1 trigger */
	DMATRIG_PININT0,					/*!< Pin interrupt 0 trigger */
	DMATRIG_PININT1,					/*!< Pin interrupt 1 trigger */
	DMATRIG_PININT2,					/*!< Pin interrupt 2 trigger */
	DMATRIG_PININT3,					/*!< Pin interrupt 3 trigger */
	DMATRIG_OUTMUX0,					/*!< DMA trigger tied to this source, Select with Chip_INMUX_SetDMAOutMux */
	DMATRIG_OUTMUX1,					/*!< DMA trigger tied to this source, Select with Chip_INMUX_SetDMAOutMux */
	DMATRIG_OUTMUX2,					/*!< DMA trigger tied to this source, Select with Chip_INMUX_SetDMAOutMux */
	DMATRIG_OUTMUX3						/*!< DMA trigger tied to this source, Select with Chip_INMUX_SetDMAOutMux */
} DMA_TRIGSRC_T;

/**
 * @brief	Select a trigger source for a DMA channel
 * @param	ch		: DMA channel number
 * @param	trig	: Trigger source for the DMA channel
 * @return	Nothing
 */
__STATIC_INLINE void Chip_INMUX_SetDMATrigger(uint8_t ch, DMA_TRIGSRC_T trig)
{
	LPC_INMUX->DMA_ITRIG_INMUX[ch] = (uint32_t) trig;
}

/**
 * @brief	Selects a DMA trigger source for the DMATRIG_OUTMUXn IDs
 * @param	index	: Select 0 to 3 to sets the source for DMATRIG_OUTMUX0 to DMATRIG_OUTMUX3
 * @param	dmaCh	: DMA channel to select for DMATRIG_OUTMUXn source
 * @return	Nothing
 * @note	This function sets the DMA trigger (out) source used with the DMATRIG_OUTMUXn
 *			trigger source.
 */
__STATIC_INLINE void Chip_INMUX_SetDMAOutMux(uint8_t index, uint8_t dmaCh)
{
	LPC_INMUX->DMA_OTRIG_INMUX[index] = (uint32_t) dmaCh;
}

/* Freqeuency measure reference and target clock sources */
typedef enum {
	FREQMSR_CLKIN = 0,				/*!< CLKIN pin */
	FREQMSR_IRC,					/*!< Internal RC (IRC) oscillator */
	FREQMSR_WDOSC,					/*!< Watchdog oscillator */
	FREQMSR_32KHZOSC,				/*!< 32KHz (RTC) oscillator rate */
	FREQ_MEAS_MAIN_CLK,				/*!< main system clock */
	FREQMSR_PIO0_4,					/*!< External pin PIO0_4 as input rate */
	FREQMSR_PIO0_20,				/*!< External pin PIO0_20 as input rate */
	FREQMSR_PIO0_24,				/*!< External pin PIO0_24 as input rate */
	FREQMSR_PIO1_4					/*!< External pin PIO1_4 as input rate */
} FREQMSR_SRC_T;

/**
 * @brief	Selects a reference clock used with the frequency measure function
 * @param	ref	: Frequency measure function reference clock
 * @return	Nothing
 */
__STATIC_INLINE void Chip_INMUX_SetFreqMeasRefClock(FREQMSR_SRC_T ref)
{
	LPC_INMUX->FREQMEAS_REF = (uint32_t) ref;
}

/**
 * @brief	Selects a target clock used with the frequency measure function
 * @param	targ	: Frequency measure function reference clock
 * @return	Nothing
 */
__STATIC_INLINE void Chip_INMUX_SetFreqMeasTargClock(FREQMSR_SRC_T targ)
{
	LPC_INMUX->FREQMEAS_TARGET = (uint32_t) targ;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __INMUX_5410X_H_ */
