/*
 * @brief LPC5410X State Configurable Timer (SCT) Chip driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licenser disclaim any and
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

#ifndef __SCT_5410X_H_
#define __SCT_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SCT_5410X CHIP: LPC5410X State Configurable Timer driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/*                         match/cap registers,     events,           states,         inputs,         outputs
 *
 * @brief SCT Module configuration
 */
#define CONFIG_SCT_nEV   (13)			/*!< Number of events */
#define CONFIG_SCT_nRG   (13)			/*!< Number of match/compare registers */
#define CONFIG_SCT_nOU   (8)			/*!< Number of outputs */
#define CONFIG_SCT_nIN   (8)			/*!< Number of outputs */

/**
 * @brief State Configurable Timer register block structure
 */
typedef struct {
	__IO  uint32_t CONFIG;				/*!< configuration Register (offset (0x000) */
	union {
		__IO uint32_t CTRL_U;			/*!< control Register */
		struct {
			__IO uint16_t CTRL_L;		/*!< low control register */
			__IO uint16_t CTRL_H;		/*!< high control register */
		};

	};

	union {
		__IO uint32_t LIMIT_U;			/*!< limit Register */
		struct {
			__IO uint16_t LIMIT_L;		/*!< limit register for counter L */
			__IO uint16_t LIMIT_H;		/*!< limit register for counter H */
		};

	};

	union {
		__IO uint32_t HALT_U;			/*!< halt Register */
		struct {
			__IO uint16_t HALT_L;		/*!< halt register for counter L */
			__IO uint16_t HALT_H;		/*!< halt register for counter H */
		};

	};

	union {
		__IO uint32_t STOP_U;			/*!< stop Register */
		struct {
			__IO uint16_t STOP_L;		/*!< stop register for counter L */
			__IO uint16_t STOP_H;		/*!< stop register for counter H */
		};

	};

	union {
		__IO uint32_t START_U;			/*!< start Register */
		struct {
			__IO uint16_t START_L;		/*!< start register for counter L */
			__IO uint16_t START_H;		/*!< start register for counter H */
		};

	};

	uint32_t RESERVED1[10];				/*!< 0x018 - 0x03C reserved */

	union {
		__IO uint32_t COUNT_U;			/*!< counter register (offset 0x040)*/
		struct {
			__IO uint16_t COUNT_L;		/*!< counter register for counter L */
			__IO uint16_t COUNT_H;		/*!< counter register for counter H */
		};

	};

	union {
		__IO uint32_t STATE_U;			/*!< State register */
		struct {
			__IO uint16_t STATE_L;		/*!< state register for counter L */
			__IO uint16_t STATE_H;		/*!< state register for counter H */
		};

	};

	__I  uint32_t INPUT;				/*!< input register */
	union {
		__IO uint32_t REGMODE_U;		/*!< RegMode register */
		struct {
			__IO uint16_t REGMODE_L;	/*!< match - capture registers mode register L */
			__IO uint16_t REGMODE_H;	/*!< match - capture registers mode register H */
		};

	};

	__IO uint32_t OUTPUT;				/*!< output register */
	__IO uint32_t OUTPUTDIRCTRL;		/*!< output counter direction Control Register */
	__IO uint32_t RES;					/*!< conflict resolution register */
	__IO uint32_t DMAREQ0;				/*!< DMA0 Request Register */
	__IO uint32_t DMAREQ1;				/*!< DMA1 Request Register */
	uint32_t RESERVED2[35];
	__IO uint32_t EVEN;		/*!< event enable register */
	__IO uint32_t EVFLAG;		/*!< event flag register */
	__IO uint32_t CONEN;	/*!< conflict enable register */
	__IO uint32_t CONFLAG;		/*!< conflict flag register */

	union {

		__IO union {	/*!< ... Match / Capture value */
			uint32_t U;		/*!<       SCTMATCH[i].U  Unified 32-bit register */

			struct {
				uint16_t L;		/*!<       SCTMATCH[i].L  Access to L value */
				uint16_t H;		/*!<       SCTMATCH[i].H  Access to H value */
			};

		} MATCH[CONFIG_SCT_nRG];

		__I union {
			uint32_t U;		/*!<       SCTCAP[i].U  Unified 32-bit register */

			struct {
				uint16_t L;		/*!<       SCTCAP[i].L  Access to L value */
				uint16_t H;		/*!<       SCTCAP[i].H  Access to H value */
			};

		} CAP[CONFIG_SCT_nRG];

	};

	uint32_t RESERVED3[48 + (16 - CONFIG_SCT_nRG)];

	union {

		__IO union {	/* 0x200-... Match Reload / Capture Control value */
			uint32_t U;		/*       SCTMATCHREL[i].U  Unified 32-bit register */

			struct {
				uint16_t L;		/*       SCTMATCHREL[i].L  Access to L value */
				uint16_t H;		/*       SCTMATCHREL[i].H  Access to H value */
			};

		} MATCHREL[CONFIG_SCT_nRG];

		__IO union {
			uint32_t U;		/*       SCTCAPCTRL[i].U  Unified 32-bit register */

			struct {
				uint16_t L;		/*       SCTCAPCTRL[i].L  Access to H value */
				uint16_t H;		/*       SCTCAPCTRL[i].H  Access to H value */
			};

		} CAPCTRL[CONFIG_SCT_nRG];

	};

	uint32_t RESERVED6[48 + (16 - CONFIG_SCT_nRG)];

	__IO struct {		/* 0x300-0x3FC  SCTEVENT[i].STATE / SCTEVENT[i].CTRL*/
		uint32_t STATE;		/* Event State Register */
		uint32_t CTRL;		/* Event Control Register */
	} EVENT[CONFIG_SCT_nEV];

	uint32_t RESERVED9[128 - 2 * CONFIG_SCT_nEV];		/*!< ...-0x4FC reserved */

	__IO struct {		/*!< 0x500-0x57C  SCTOUT[i].SET / SCTOUT[i].CLR */
		uint32_t SET;		/*!< Output n Set Register */
		uint32_t CLR;		/*!< Output n Clear Register */
	} OUT[CONFIG_SCT_nOU];

	uint32_t RESERVED10[191 - 2 * CONFIG_SCT_nOU];		/*!< ...-0x7F8 reserved */
	__I uint32_t MODULECONTENT;		/*!< 0x7FC Module Content */
} LPC_SCT_T;

/**
 * @brief Macro defines for SCT configuration register
 */
#define SCT_CONFIG_16BIT_COUNTER        0x00000000	/*!< Operate as 2 16-bit counters */
#define SCT_CONFIG_32BIT_COUNTER        0x00000001	/*!< Operate as 1 32-bit counter */

#define SCT_CONFIG_CLKMODE_BUSCLK       (0x0 << 1)	/*!< Bus clock */
#define SCT_CONFIG_CLKMODE_SCTCLK       (0x1 << 1)	/*!< SCT clock */
#define SCT_CONFIG_CLKMODE_INCLK        (0x2 << 1)	/*!< Input clock selected in CLKSEL field */
#define SCT_CONFIG_CLKMODE_INEDGECLK    (0x3 << 1)	/*!< Input clock edge selected in CLKSEL field */

#define SCT_CONFIG_CLKMODE_SYSCLK               (0x0 << 1)	/*!< System clock */
#define SCT_CONFIG_CLKMODE_PRESCALED_SYSCLK     (0x1 << 1)	/*!< Prescaled system clock */
#define SCT_CONFIG_CLKMODE_SCT_INPUT            (0x2 << 1)	/*!< Input clock/edge selected in CKSEL field */
#define SCT_CONFIG_CLKMODE_PRESCALED_SCT_INPUT  (0x3 << 1)	/*!< Prescaled input clock/edge selected in CKSEL field */

#define SCT_CONFIG_CKSEL_RISING_IN_0    (0x0UL << 3)
#define SCT_CONFIG_CKSEL_FALLING_IN_0   (0x1UL << 3)
#define SCT_CONFIG_CKSEL_RISING_IN_1    (0x2UL << 3)
#define SCT_CONFIG_CKSEL_FALLING_IN_1   (0x3UL << 3)
#define SCT_CONFIG_CKSEL_RISING_IN_2    (0x4UL << 3)
#define SCT_CONFIG_CKSEL_FALLING_IN_2   (0x5UL << 3)
#define SCT_CONFIG_CKSEL_RISING_IN_3    (0x6UL << 3)
#define SCT_CONFIG_CKSEL_FALLING_IN_3   (0x7UL << 3)
#define SCT_CONFIG_CKSEL_RISING_IN_4    (0x8UL << 3)
#define SCT_CONFIG_CKSEL_FALLING_IN_4   (0x9UL << 3)
#define SCT_CONFIG_CKSEL_RISING_IN_5    (0xAUL << 3)
#define SCT_CONFIG_CKSEL_FALLING_IN_5   (0xBUL << 3)
#define SCT_CONFIG_CKSEL_RISING_IN_6    (0xCUL << 3)
#define SCT_CONFIG_CKSEL_FALLING_IN_6   (0xDUL << 3)
#define SCT_CONFIG_CKSEL_RISING_IN_7    (0xEUL << 3)
#define SCT_CONFIG_CKSEL_FALLING_IN_7   (0xFUL << 3)
#define SCT_CONFIG_NORELOADL_U          (0x1 << 7)	/*!< Operate as 1 32-bit counter */
#define SCT_CONFIG_NORELOADH            (0x1 << 8)	/*!< Operate as 1 32-bit counter */
#define SCT_CONFIG_AUTOLIMIT_U          (0x1UL << 17)
#define SCT_CONFIG_AUTOLIMIT_L          (0x1UL << 17)
#define SCT_CONFIG_AUTOLIMIT_H          (0x1UL << 18)

/**
 * @brief Macro defines for SCT control register
 */
#define COUNTUP_TO_LIMIT_THEN_CLEAR_TO_ZERO     0			/*!< Direction for low or unified counter */
#define COUNTUP_TO LIMIT_THEN_COUNTDOWN_TO_ZERO 1

#define SCT_CTRL_STOP_L                 (1 << 1)				/*!< Stop low counter */
#define SCT_CTRL_HALT_L                 (1 << 2)				/*!< Halt low counter */
#define SCT_CTRL_CLRCTR_L               (1 << 3)				/*!< Clear low or unified counter */
#define SCT_CTRL_BIDIR_L(x)             (((x) & 0x01) << 4)		/*!< Bidirectional bit */
#define SCT_CTRL_PRE_L(x)               (((x) & 0xFF) << 5)		/*!< Prescale clock for low or unified counter */

#define COUNTUP_TO_LIMIT_THEN_CLEAR_TO_ZERO     0			/*!< Direction for high counter */
#define COUNTUP_TO LIMIT_THEN_COUNTDOWN_TO_ZERO 1
#define SCT_CTRL_STOP_H                 (1 << 17)				/*!< Stop high counter */
#define SCT_CTRL_HALT_H                 (1 << 18)				/*!< Halt high counter */
#define SCT_CTRL_CLRCTR_H               (1 << 19)				/*!< Clear high counter */
#define SCT_CTRL_BIDIR_H(x)             (((x) & 0x01) << 20)
#define SCT_CTRL_PRE_H(x)               (((x) & 0xFF) << 21)	/*!< Prescale clock for high counter */

#define SCT_EV_CTRL_MATCHSEL(reg)               (reg << 0)
#define SCT_EV_CTRL_HEVENT_L                    (0UL << 4)
#define SCT_EV_CTRL_HEVENT_H                    (1UL << 4)
#define SCT_EV_CTRL_OUTSEL_INPUT                (0UL << 5)
#define SCT_EV_CTRL_OUTSEL_OUTPUT               (0UL << 5)
#define SCT_EV_CTRL_IOSEL(signal)               (signal << 6)

#define SCT_EV_CTRL_IOCOND_LOW                  (0UL << 10)
#define SCT_EV_CTRL_IOCOND_RISE                 (0x1UL << 10)
#define SCT_EV_CTRL_IOCOND_FALL                 (0x2UL << 10)
#define SCT_EV_CTRL_IOCOND_HIGH                 (0x3UL << 10)
#define SCT_EV_CTRL_COMBMODE_OR                 (0x0UL << 12)
#define SCT_EV_CTRL_COMBMODE_MATCH              (0x1UL << 12)
#define SCT_EV_CTRL_COMBMODE_IO                 (0x2UL << 12)
#define SCT_EV_CTRL_COMBMODE_AND                (0x3UL << 12)
#define SCT_EV_CTRL_STATELD                     (0x1UL << 14)
#define SCT_EV_CTRL_STATEV(x)                   (x << 15)
#define SCT_EV_CTRL_MATCHMEM                    (0x1UL << 20)
#define SCT_EV_CTRL_DIRECTION_INDEPENDENT       (0x0UL << 21)
#define SCT_EV_CTRL_DIRECTION_UP                (0x1UL << 21)
#define SCT_EV_CTRL_DIRECTION_DOWN              (0x2UL << 21)

/**
 * @brief Macro defines for SCT Conflict resolution register
 */
#define SCT_RES_NOCHANGE                (0)
#define SCT_RES_SET_OUTPUT              (1)
#define SCT_RES_CLEAR_OUTPUT            (2)
#define SCT_RES_TOGGLE_OUTPUT           (3)

/**
 * SCT Match register values enum
 */
typedef enum CHIP_SCT_MATCH_REG {
	SCT_MATCH_0 = 0,	/*!< SCT Match register 0 */
	SCT_MATCH_1,
	SCT_MATCH_2,
	SCT_MATCH_3,
	SCT_MATCH_4,
	SCT_MATCH_5,
	SCT_MATCH_6,
	SCT_MATCH_7,
	SCT_MATCH_8,
	SCT_MATCH_9,
	SCT_MATCH_10,
	SCT_MATCH_11,
	SCT_MATCH_12,
	SCT_MATCH_13,
	SCT_MATCH_14,
	SCT_MATCH_15
} CHIP_SCT_MATCH_REG_T;

/**
 * SCT Event values enum
 */
typedef enum CHIP_SCT_EVENT {
	SCT_EVT_0 = (1 << 0),		/*!< Event 0 */
	SCT_EVT_1 = (1 << 1),		/*!< Event 1 */
	SCT_EVT_2 = (1 << 2),		/*!< Event 2 */
	SCT_EVT_3 = (1 << 3),		/*!< Event 3 */
	SCT_EVT_4 = (1 << 4),		/*!< Event 4 */
	SCT_EVT_5 = (1 << 5),		/*!< Event 5 */
	SCT_EVT_6 = (1 << 6),		/*!< Event 6 */
	SCT_EVT_7 = (1 << 7),		/*!< Event 7 */
	SCT_EVT_8 = (1 << 8),		/*!< Event 8 */
	SCT_EVT_9 = (1 << 9),		/*!< Event 9 */
	SCT_EVT_10 = (1 << 10),		/*!< Event 10 */
	SCT_EVT_11 = (1 << 11),		/*!< Event 11 */
	SCT_EVT_12 = (1 << 12),		/*!< Event 12 */
	SCT_EVT_13 = (1 << 13),		/*!< Event 13 */
	SCT_EVT_14 = (1 << 14),		/*!< Event 14 */
	SCT_EVT_15 = (1 << 15)		/*!< Event 15 */
} CHIP_SCT_EVENT_T;

/**
 * @brief	Set event control register
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	event_number
 * @param	value	: The 32-bit event control setting
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_EventControl(LPC_SCT_T *pSCT, uint32_t event_number,
										   uint32_t value) {
	pSCT->EVENT[event_number].CTRL = value;
}

/**
 * @brief	Set event state mask register
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	event_number
 * @param	event_state_mask      : The 32-bit event state mask setting
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_EventStateMask(LPC_SCT_T *pSCT, uint32_t event_number,
											 uint32_t event_state_mask) {
	pSCT->EVENT[event_number].STATE = event_state_mask;
}

/**
 * @brief	Set configuration register
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	cfg      : The 32-bit configuration setting
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_Config(LPC_SCT_T *pSCT, uint32_t cfg) {
	pSCT->CONFIG = cfg;
}

/**
 * @brief	Configures the Limit register
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	value	: The 32-bit Limit register value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_Limit(LPC_SCT_T *pSCT, uint32_t value) {
	pSCT->LIMIT_L = value;
}

/**
 * @brief	Set or Clear the Control register
 * @param	pSCT			: Pointer to SCT register block
 * @param	value			: SCT Control register value
 * @param	ena             : ENABLE - To set the fields specified by value
 *                          : DISABLE - To clear the field specified by value
 * @return	Nothing
 * Set or clear the control register bits as specified by the \a value
 * parameter. If \a ena is set to ENABLE, the mentioned register fields
 * will be set. If \a ena is set to DISABLE, the mentioned register
 * fields will be cleared
 */
void Chip_SCT_SetClrControl(LPC_SCT_T *pSCT, uint32_t value, FunctionalState ena);

/**
 * @brief	Set the conflict resolution
 * @param	pSCT			: Pointer to SCT register block
 * @param	outnum			: Output number
 * @param	value           : Output value
 *                          - SCT_RES_NOCHANGE		:No change
 *					        - SCT_RES_SET_OUTPUT	:Set output
 *					        - SCT_RES_CLEAR_OUTPUT	:Clear output
 *					        - SCT_RES_TOGGLE_OUTPUT :Toggle output
 *                          : SCT_RES_NOCHANGE
 *                          : DISABLE - To clear the field specified by value
 * @return	Nothing
 * Set conflict resolution for the output \a outnum
 */
void Chip_SCT_SetConflictResolution(LPC_SCT_T *pSCT, uint8_t outnum, uint8_t value);

/**
 * @brief	Set unified count value in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	count	: The 32-bit count value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_SetCount(LPC_SCT_T *pSCT, uint32_t count) {
	pSCT->COUNT_U = count;
}

/**
 * @brief	Set lower count value in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	count	: The 16-bit count value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_SetCountL(LPC_SCT_T *pSCT, uint16_t count) {
	pSCT->COUNT_L = count;
}

/**
 * @brief	Set higher count value in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	count	: The 16-bit count value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_SetCountH(LPC_SCT_T *pSCT, uint16_t count) {
	pSCT->COUNT_H = count;
}

/**
 * @brief	Set unified match count value in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	n		: Match register value
 * @param	value	: The 32-bit match count value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_SetMatchCount(LPC_SCT_T *pSCT, CHIP_SCT_MATCH_REG_T n, uint32_t value) {
	pSCT->MATCH[n].U = value;
}

/**
 * @brief	Set unified match reload count value in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	n		: Match register value
 * @param	value	: The 32-bit match count reload value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_SetMatchReload(LPC_SCT_T *pSCT, CHIP_SCT_MATCH_REG_T n, uint32_t value) {
	pSCT->MATCHREL[n].U = value;
}

/**
 * @brief	Enable the interrupt for the specified event in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	evt		: Event value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_EnableEventInt(LPC_SCT_T *pSCT, CHIP_SCT_EVENT_T evt) {
	pSCT->EVEN |= evt;
}

/**
 * @brief	Disable the interrupt for the specified event in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	evt		: Event value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_DisableEventInt(LPC_SCT_T *pSCT, CHIP_SCT_EVENT_T evt) {
	pSCT->EVEN &= ~(evt);
}

/**
 * @brief	Clear the specified event flag in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	evt		: Event value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_ClearEventFlag(LPC_SCT_T *pSCT, CHIP_SCT_EVENT_T evt) {
	pSCT->EVFLAG |= evt;
}

/**
 * @brief	Set control register in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	value	: Value (ORed value of SCT_CTRL_* bits)
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_SetControl(LPC_SCT_T *pSCT, uint32_t value) {
	pSCT->CTRL_U |= value;
}

/**
 * @brief	Clear control register in State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @param	value	: Value (ORed value of SCT_CTRL_* bits)
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SCT_ClearControl(LPC_SCT_T *pSCT, uint32_t value) {
	pSCT->CTRL_U &= ~(value);
}

/**
 * @brief	Initializes the State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @return	Nothing
 */
void Chip_SCT_Init(LPC_SCT_T *pSCT);

/**
 * @brief	Deinitializes the State Configurable Timer
 * @param	pSCT	: The base of SCT peripheral on the chip
 * @return	Nothing
 */
void Chip_SCT_DeInit(LPC_SCT_T *pSCT);

/**
 * @}
 */

#ifdef __cplusplus
}

#endif

#endif /* __SCT_5410X_H_ */
