/*
 * @brief LPC18xx/43xx 16/32-bit Timer/PWM driver
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

#ifndef __TIMER_18XX_43XX_H_
#define __TIMER_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup TIMER_18XX_43XX CHIP: LPC18xx/43xx 16/32-bit Timer driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief 32-bit Standard timer register block structure
 */
typedef struct {					/*!< TIMERn Structure       */
	__IO uint32_t IR;				/*!< Interrupt Register. The IR can be written to clear interrupts. The IR can be read to identify which of eight possible interrupt sources are pending. */
	__IO uint32_t TCR;				/*!< Timer Control Register. The TCR is used to control the Timer Counter functions. The Timer Counter can be disabled or reset through the TCR. */
	__IO uint32_t TC;				/*!< Timer Counter. The 32 bit TC is incremented every PR+1 cycles of PCLK. The TC is controlled through the TCR. */
	__IO uint32_t PR;				/*!< Prescale Register. The Prescale Counter (below) is equal to this value, the next clock increments the TC and clears the PC. */
	__IO uint32_t PC;				/*!< Prescale Counter. The 32 bit PC is a counter which is incremented to the value stored in PR. When the value in PR is reached, the TC is incremented and the PC is cleared. The PC is observable and controllable through the bus interface. */
	__IO uint32_t MCR;				/*!< Match Control Register. The MCR is used to control if an interrupt is generated and if the TC is reset when a Match occurs. */
	__IO uint32_t MR[4];			/*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	__IO uint32_t CCR;				/*!< Capture Control Register. The CCR controls which edges of the capture inputs are used to load the Capture Registers and whether or not an interrupt is generated when a capture takes place. */
	__IO uint32_t CR[4];			/*!< Capture Register. CR is loaded with the value of TC when there is an event on the CAPn.0 input. */
	__IO uint32_t EMR;				/*!< External Match Register. The EMR controls the external match pins MATn.0-3 (MAT0.0-3 and MAT1.0-3 respectively). */
	__I  uint32_t RESERVED0[12];
	__IO uint32_t CTCR;				/*!< Count Control Register. The CTCR selects between Timer and Counter mode, and in Counter mode selects the signal and edge(s) for counting. */
} LPC_TIMER_T;

/** Macro to clear interrupt pending */
#define TIMER_IR_CLR(n)         _BIT(n)

/** Macro for getting a timer match interrupt bit */
#define TIMER_MATCH_INT(n)      (_BIT((n) & 0x0F))
/** Macro for getting a capture event interrupt bit */
#define TIMER_CAP_INT(n)        (_BIT((((n) & 0x0F) + 4)))

/** Timer/counter enable bit */
#define TIMER_ENABLE            ((uint32_t) (1 << 0))
/** Timer/counter reset bit */
#define TIMER_RESET             ((uint32_t) (1 << 1))

/** Bit location for interrupt on MRx match, n = 0 to 3 */
#define TIMER_INT_ON_MATCH(n)   (_BIT(((n) * 3)))
/** Bit location for reset on MRx match, n = 0 to 3 */
#define TIMER_RESET_ON_MATCH(n) (_BIT((((n) * 3) + 1)))
/** Bit location for stop on MRx match, n = 0 to 3 */
#define TIMER_STOP_ON_MATCH(n)  (_BIT((((n) * 3) + 2)))

/** Bit location for CAP.n on CRx rising edge, n = 0 to 3 */
#define TIMER_CAP_RISING(n)     (_BIT(((n) * 3)))
/** Bit location for CAP.n on CRx falling edge, n = 0 to 3 */
#define TIMER_CAP_FALLING(n)    (_BIT((((n) * 3) + 1)))
/** Bit location for CAP.n on CRx interrupt enable, n = 0 to 3 */
#define TIMER_INT_ON_CAP(n)     (_BIT((((n) * 3) + 2)))

/**
 * @brief	Initialize a timer
 * @param	pTMR	: Pointer to timer IP register address
 * @return	Nothing
 */
void Chip_TIMER_Init(LPC_TIMER_T *pTMR);

/**
 * @brief	Shutdown a timer
 * @param	pTMR	: Pointer to timer IP register address
 * @return	Nothing
 */
void Chip_TIMER_DeInit(LPC_TIMER_T *pTMR);

/**
 * @brief	Determine if a match interrupt is pending
 * @param	pTMR		: Pointer to timer IP register address
 * @param	matchnum	: Match interrupt number to check
 * @return	false if the interrupt is not pending, otherwise true
 * @note	Determine if the match interrupt for the passed timer and match
 * counter is pending.
 */
STATIC INLINE bool Chip_TIMER_MatchPending(LPC_TIMER_T *pTMR, int8_t matchnum)
{
	return (bool) ((pTMR->IR & TIMER_MATCH_INT(matchnum)) != 0);
}

/**
 * @brief	Determine if a capture interrupt is pending
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capnum	: Capture interrupt number to check
 * @return	false if the interrupt is not pending, otherwise true
 * @note	Determine if the capture interrupt for the passed capture pin is
 * pending.
 */
STATIC INLINE bool Chip_TIMER_CapturePending(LPC_TIMER_T *pTMR, int8_t capnum)
{
	return (bool) ((pTMR->IR & TIMER_CAP_INT(capnum)) != 0);
}

/**
 * @brief	Clears a (pending) match interrupt
 * @param	pTMR		: Pointer to timer IP register address
 * @param	matchnum	: Match interrupt number to clear
 * @return	Nothing
 * @note	Clears a pending timer match interrupt.
 */
STATIC INLINE void Chip_TIMER_ClearMatch(LPC_TIMER_T *pTMR, int8_t matchnum)
{
	pTMR->IR = TIMER_IR_CLR(matchnum);
}

/**
 * @brief	Clears a (pending) capture interrupt
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capnum	: Capture interrupt number to clear
 * @return	Nothing
 * @note	Clears a pending timer capture interrupt.
 */
STATIC INLINE void Chip_TIMER_ClearCapture(LPC_TIMER_T *pTMR, int8_t capnum)
{
	pTMR->IR = (0x10 << capnum);
}

/**
 * @brief	Enables the timer (starts count)
 * @param	pTMR	: Pointer to timer IP register address
 * @return	Nothing
 * @note	Enables the timer to start counting.
 */
STATIC INLINE void Chip_TIMER_Enable(LPC_TIMER_T *pTMR)
{
	pTMR->TCR |= TIMER_ENABLE;
}

/**
 * @brief	Disables the timer (stops count)
 * @param	pTMR	: Pointer to timer IP register address
 * @return	Nothing
 * @note	Disables the timer to stop counting.
 */
STATIC INLINE void Chip_TIMER_Disable(LPC_TIMER_T *pTMR)
{
	pTMR->TCR &= ~TIMER_ENABLE;
}

/**
 * @brief	Returns the current timer count
 * @param	pTMR	: Pointer to timer IP register address
 * @return	Current timer terminal count value
 * @note	Returns the current timer terminal count.
 */
STATIC INLINE uint32_t Chip_TIMER_ReadCount(LPC_TIMER_T *pTMR)
{
	return pTMR->TC;
}

/**
 * @brief	Returns the current prescale count
 * @param	pTMR	: Pointer to timer IP register address
 * @return	Current timer prescale count value
 * @note	Returns the current prescale count.
 */
STATIC INLINE uint32_t Chip_TIMER_ReadPrescale(LPC_TIMER_T *pTMR)
{
	return pTMR->PC;
}

/**
 * @brief	Sets the prescaler value
 * @param	pTMR		: Pointer to timer IP register address
 * @param	prescale	: Prescale value to set the prescale register to
 * @return	Nothing
 * @note	Sets the prescale count value.
 */
STATIC INLINE void Chip_TIMER_PrescaleSet(LPC_TIMER_T *pTMR, uint32_t prescale)
{
	pTMR->PR = prescale;
}

/**
 * @brief	Sets a timer match value
 * @param	pTMR		: Pointer to timer IP register address
 * @param	matchnum	: Match timer to set match count for
 * @param	matchval	: Match value for the selected match count
 * @return	Nothing
 * @note	Sets one of the timer match values.
 */
STATIC INLINE void Chip_TIMER_SetMatch(LPC_TIMER_T *pTMR, int8_t matchnum, uint32_t matchval)
{
	pTMR->MR[matchnum] = matchval;
}

/**
 * @brief	Reads a capture register
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capnum	: Capture register to read
 * @return	The selected capture register value
 * @note	Returns the selected capture register value.
 */
STATIC INLINE uint32_t Chip_TIMER_ReadCapture(LPC_TIMER_T *pTMR, int8_t capnum)
{
	return pTMR->CR[capnum];
}

/**
 * @brief	Resets the timer terminal and prescale counts to 0
 * @param	pTMR	: Pointer to timer IP register address
 * @return	Nothing
 */
void Chip_TIMER_Reset(LPC_TIMER_T *pTMR);

/**
 * @brief	Enables a match interrupt that fires when the terminal count
 *			matches the match counter value.
 * @param	pTMR		: Pointer to timer IP register address
 * @param	matchnum	: Match timer, 0 to 3
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_MatchEnableInt(LPC_TIMER_T *pTMR, int8_t matchnum)
{
	pTMR->MCR |= TIMER_INT_ON_MATCH(matchnum);
}

/**
 * @brief	Disables a match interrupt for a match counter.
 * @param	pTMR		: Pointer to timer IP register address
 * @param	matchnum	: Match timer, 0 to 3
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_MatchDisableInt(LPC_TIMER_T *pTMR, int8_t matchnum)
{
	pTMR->MCR &= ~TIMER_INT_ON_MATCH(matchnum);
}

/**
 * @brief	For the specific match counter, enables reset of the terminal count register when a match occurs
 * @param	pTMR		: Pointer to timer IP register address
 * @param	matchnum	: Match timer, 0 to 3
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_ResetOnMatchEnable(LPC_TIMER_T *pTMR, int8_t matchnum)
{
	pTMR->MCR |= TIMER_RESET_ON_MATCH(matchnum);
}

/**
 * @brief	For the specific match counter, disables reset of the terminal count register when a match occurs
 * @param	pTMR		: Pointer to timer IP register address
 * @param	matchnum	: Match timer, 0 to 3
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_ResetOnMatchDisable(LPC_TIMER_T *pTMR, int8_t matchnum)
{
	pTMR->MCR &= ~TIMER_RESET_ON_MATCH(matchnum);
}

/**
 * @brief	Enable a match timer to stop the terminal count when a
 *			match count equals the terminal count.
 * @param	pTMR		: Pointer to timer IP register address
 * @param	matchnum	: Match timer, 0 to 3
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_StopOnMatchEnable(LPC_TIMER_T *pTMR, int8_t matchnum)
{
	pTMR->MCR |= TIMER_STOP_ON_MATCH(matchnum);
}

/**
 * @brief	Disable stop on match for a match timer. Disables a match timer
 *			to stop the terminal count when a match count equals the terminal count.
 * @param	pTMR		: Pointer to timer IP register address
 * @param	matchnum	: Match timer, 0 to 3
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_StopOnMatchDisable(LPC_TIMER_T *pTMR, int8_t matchnum)
{
	pTMR->MCR &= ~TIMER_STOP_ON_MATCH(matchnum);
}

/**
 * @brief	Enables capture on on rising edge of selected CAP signal for the
 *			selected capture register, enables the selected CAPn.capnum signal to load
 *			the capture register with the terminal coount on a rising edge.
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_CaptureRisingEdgeEnable(LPC_TIMER_T *pTMR, int8_t capnum)
{
	pTMR->CCR |= TIMER_CAP_RISING(capnum);
}

/**
 * @brief	Disables capture on on rising edge of selected CAP signal. For the
 *			selected capture register, disables the selected CAPn.capnum signal to load
 *			the capture register with the terminal coount on a rising edge.
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_CaptureRisingEdgeDisable(LPC_TIMER_T *pTMR, int8_t capnum)
{
	pTMR->CCR &= ~TIMER_CAP_RISING(capnum);
}

/**
 * @brief	Enables capture on on falling edge of selected CAP signal. For the
 *			selected capture register, enables the selected CAPn.capnum signal to load
 *			the capture register with the terminal coount on a falling edge.
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER_T *pTMR, int8_t capnum)
{
	pTMR->CCR |= TIMER_CAP_FALLING(capnum);
}

/**
 * @brief	Disables capture on on falling edge of selected CAP signal. For the
 *			selected capture register, disables the selected CAPn.capnum signal to load
 *			the capture register with the terminal coount on a falling edge.
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_CaptureFallingEdgeDisable(LPC_TIMER_T *pTMR, int8_t capnum)
{
	pTMR->CCR &= ~TIMER_CAP_FALLING(capnum);
}

/**
 * @brief	Enables interrupt on capture of selected CAP signal. For the
 *			selected capture register, an interrupt will be generated when the enabled
 *			rising or falling edge on CAPn.capnum is detected.
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_CaptureEnableInt(LPC_TIMER_T *pTMR, int8_t capnum)
{
	pTMR->CCR |= TIMER_INT_ON_CAP(capnum);
}

/**
 * @brief	Disables interrupt on capture of selected CAP signal
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capnum	: Capture signal/register to use
 * @return	Nothing
 */
STATIC INLINE void Chip_TIMER_CaptureDisableInt(LPC_TIMER_T *pTMR, int8_t capnum)
{
	pTMR->CCR &= ~TIMER_INT_ON_CAP(capnum);
}

/**
 * @brief Standard timer initial match pin state and change state
 */
typedef enum IP_TIMER_PIN_MATCH_STATE {
	TIMER_EXTMATCH_DO_NOTHING = 0,	/*!< Timer match state does nothing on match pin */
	TIMER_EXTMATCH_CLEAR      = 1,	/*!< Timer match state sets match pin low */
	TIMER_EXTMATCH_SET        = 2,	/*!< Timer match state sets match pin high */
	TIMER_EXTMATCH_TOGGLE     = 3	/*!< Timer match state toggles match pin */
} TIMER_PIN_MATCH_STATE_T;

/**
 * @brief	Sets external match control (MATn.matchnum) pin control. For the pin
 *          selected with matchnum, sets the function of the pin that occurs on
 *          a terminal count match for the match count.
 * @param	pTMR			: Pointer to timer IP register address
 * @param	initial_state	: Initial state of the pin, high(1) or low(0)
 * @param	matchState		: Selects the match state for the pin
 * @param	matchnum		: MATn.matchnum signal to use
 * @return	Nothing
 * @note	For the pin selected with matchnum, sets the function of the pin that occurs on
 * a terminal count match for the match count.
 */
void Chip_TIMER_ExtMatchControlSet(LPC_TIMER_T *pTMR, int8_t initial_state,
												 TIMER_PIN_MATCH_STATE_T matchState, int8_t matchnum);

/**
 * @brief Standard timer clock and edge for count source
 */
typedef enum IP_TIMER_CAP_SRC_STATE {
	TIMER_CAPSRC_RISING_PCLK  = 0,	/*!< Timer ticks on PCLK rising edge */
	TIMER_CAPSRC_RISING_CAPN  = 1,	/*!< Timer ticks on CAPn.x rising edge */
	TIMER_CAPSRC_FALLING_CAPN = 2,	/*!< Timer ticks on CAPn.x falling edge */
	TIMER_CAPSRC_BOTH_CAPN    = 3	/*!< Timer ticks on CAPn.x both edges */
} TIMER_CAP_SRC_STATE_T;

/**
 * @brief	Sets timer count source and edge with the selected passed from CapSrc.
 *          If CapSrc selected a CAPn pin, select the specific CAPn pin with the capnum value.
 * @param	pTMR	: Pointer to timer IP register address
 * @param	capSrc	: timer clock source and edge
 * @param	capnum	: CAPn.capnum pin to use (if used)
 * @return	Nothing
 * @note	If CapSrc selected a CAPn pin, select the specific CAPn pin with the capnum value.
 */
STATIC INLINE void Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER_T *pTMR,
													 TIMER_CAP_SRC_STATE_T capSrc,
													 int8_t capnum)
{
	pTMR->CTCR = (uint32_t) capSrc | ((uint32_t) capnum) << 2;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_18XX_43XX_H_ */
