/*
 * @brief  LPC18xx/43xx High speed ADC driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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

#ifndef __HSADC_18XX_43XX_H_
#define __HSADC_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup HSADC_18XX_43XX CHIP:  LPC18xx/43xx High speed ADC driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

/**
 * @brief High speed ADC interrupt control structure
 */
typedef struct {
	__O  uint32_t CLR_EN;			/*!< Interrupt clear mask */
	__O  uint32_t SET_EN;			/*!< Interrupt set mask */
	__I  uint32_t MASK;				/*!< Interrupt mask */
	__I  uint32_t STATUS;			/*!< Interrupt status */
	__O  uint32_t CLR_STAT;			/*!< Interrupt clear status */
	__O  uint32_t SET_STAT;			/*!< Interrupt set status */
	uint32_t RESERVED[2];
} HSADCINTCTRL_T;

/**
 * @brief HSADC register block structure
 */
typedef struct {					/*!< HSADC Structure */
	__O  uint32_t FLUSH;			/*!< Flushes FIFO */
	__IO uint32_t DMA_REQ;			/*!< Set or clear DMA write request */
	__I  uint32_t FIFO_STS;			/*!< Indicates FIFO fill level status */
	__IO uint32_t FIFO_CFG;			/*!< Configures FIFO fill level */
	__O  uint32_t TRIGGER;			/*!< Enable software trigger to start descriptor processing */
	__IO uint32_t DSCR_STS;			/*!< Indicates active descriptor table and descriptor entry */
	__IO uint32_t POWER_DOWN;		/*!< Set or clear power down mode */
	__IO uint32_t CONFIG;			/*!< Configures external trigger mode, store channel ID in FIFO and walk-up recovery time from power down */
	__IO uint32_t THR[2];			/*!< Configures window comparator A or B levels */
	__I  uint32_t LAST_SAMPLE[6];	/*!< Contains last converted sample of input M [M=0..5) and result of window comparator */
	uint32_t RESERVED0[49];
	__IO uint32_t ADC_SPEED;		/*!< ADC speed control */
	__IO uint32_t POWER_CONTROL;	/*!< Configures ADC power vs. speed, DC-in biasing, output format and power gating */
	uint32_t RESERVED1[61];
	__I  uint32_t FIFO_OUTPUT[16];	/*!< FIFO output mapped to 16 consecutive address locations */
	uint32_t RESERVED2[48];
	__IO uint32_t DESCRIPTOR[2][8];	/*!< Table 0 and 1 descriptors */
	uint32_t RESERVED3[752];
	HSADCINTCTRL_T INTS[2];			/*!< Interrupt 0 and 1 control and status registers */
} LPC_HSADC_T;

#define HSADC_MAX_SAMPLEVAL 0xFFF

/**
 * @brief	Initialize the High speed ADC
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	Nothing
 */
void Chip_HSADC_Init(LPC_HSADC_T *pHSADC);

/**
 * @brief	Shutdown HSADC
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	Nothing
 */
void Chip_HSADC_DeInit(LPC_HSADC_T *pHSADC);

/**
 * @brief	Flush High speed ADC FIFO
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_HSADC_FlushFIFO(LPC_HSADC_T *pHSADC)
{
	pHSADC->FLUSH = 1;
}

/**
 * @brief	Load a descriptor table from memory by requesting a DMA write
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	Nothing
 * @note	WHat is this used for?
 */
STATIC INLINE void Chip_HSADC_LoadDMADesc(LPC_HSADC_T *pHSADC)
{
	pHSADC->DMA_REQ = 1;
}

/**
 * @brief	Returns current HSADC FIFO fill level
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	FIFO level, 0 for empty, 1 to 15, or 16 for full
 * @note	WHat is this used for?
 */
STATIC INLINE uint32_t Chip_HSADC_GetFIFOLevel(LPC_HSADC_T *pHSADC)
{
	return pHSADC->FIFO_STS;
}

/**
 * @brief	Sets up HSADC FIFO trip level and packing
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @param	trip	: HSADC FIFO trip point (1 to 15 samples)
 * @param	packed	: true to pack samples, false for no packing
 * @return	Nothing
 * @note	The FIFO trip point is used for the DMA or interrupt level.
 *			Sample packging allows packing 2 samples into a single 32-bit
 *			word.
 */
void Chip_HSADC_SetupFIFO(LPC_HSADC_T *pHSADC, uint8_t trip, bool packed);

/**
 * @brief	Starts a manual (software) trigger of HSADC descriptors
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_HSADC_SWTrigger(LPC_HSADC_T *pHSADC)
{
	pHSADC->TRIGGER = 1;
}

/**
 * @brief	Set active table descriptor index and number
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @param	table	: Table index, 0 or 1
 * @param	desc	: Descriptor index, 0 to 7
 * @return	Nothing
 * @note	This function can be used to set active descriptor table and
 *			active descriptor entry values. The new values will be updated
 *			immediately. This should only be updated when descriptors are
 *			not running (halted).
 */
STATIC INLINE void Chip_HSADC_SetActiveDescriptor(LPC_HSADC_T *pHSADC, uint8_t table, uint8_t desc)
{
	pHSADC->DSCR_STS = (uint32_t) ((desc << 1) | table);
}

/**
 * @brief	Returns currently active descriptor index being processed
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	the current active descriptor index, 0 to 7
 */
STATIC INLINE uint8_t Chip_HSADC_GetActiveDescriptorIndex(LPC_HSADC_T *pHSADC)
{
	return (uint8_t) ((pHSADC->DSCR_STS >> 1) & 0x7);
}

/**
 * @brief	Returns currently active descriptor table being processed
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	the current active descriptor table, 0 or 1
 */
STATIC INLINE uint8_t Chip_HSADC_GetActiveDescriptorTable(LPC_HSADC_T *pHSADC)
{
	return (uint8_t) (pHSADC->DSCR_STS & 1);
}

/**
 * @brief	Enables ADC power down mode
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	Nothing
 * @note	In most cases, this function doesn't need to be used as
 * the descriptors control power as needed.
 */
STATIC INLINE void Chip_HSADC_EnablePowerDownMode(LPC_HSADC_T *pHSADC)
{
	pHSADC->POWER_DOWN = 1;
}

/**
 * @brief	Disables ADC power down mode
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	Nothing
 * @note	In most cases, this function doesn't need to be used as
 * the descriptors control power as needed.
 */
STATIC INLINE void Chip_HSADC_DisablePowerDownMode(LPC_HSADC_T *pHSADC)
{
	pHSADC->POWER_DOWN = 0;
}

/* HSADC trigger configuration mask types */
typedef enum {
	HSADC_CONFIG_TRIGGER_OFF = 0,				/*!< ADCHS triggers off */
	HSADC_CONFIG_TRIGGER_SW = 1,				/*!< ADCHS software trigger only */
	HSADC_CONFIG_TRIGGER_EXT = 2,				/*!< ADCHS external trigger only */
	HSADC_CONFIG_TRIGGER_BOTH = 3				/*!< ADCHS both software and external triggers allowed */
} HSADC_TRIGGER_MASK_T;

/* HSADC trigger configuration mode types */
typedef enum {
	HSADC_CONFIG_TRIGGER_RISEEXT = (0 << 2),	/*!< ADCHS rising external trigger */
	HSADC_CONFIG_TRIGGER_FALLEXT = (1 << 2),	/*!< ADCHS falling external trigger */
	HSADC_CONFIG_TRIGGER_LOWEXT = (2 << 2),		/*!< ADCHS low external trigger */
	HSADC_CONFIG_TRIGGER_HIGHEXT = (3 << 2)		/*!< ADCHS high external trigger */
} HSADC_TRIGGER_MODE_T;

/* HSADC trigger configuration sync types */
typedef enum {
	HSADC_CONFIG_TRIGGER_NOEXTSYNC = (0 << 4),	/*!< do not synchronize external trigger input */
	HSADC_CONFIG_TRIGGER_EXTSYNC = (1 << 4),	/*!< synchronize external trigger input */
} HSADC_TRIGGER_SYNC_T;

/* HSADC trigger configuration channel ID */
typedef enum {
	HSADC_CHANNEL_ID_EN_NONE = (0 << 5),	/*!< do not add channel ID to FIFO output data */
	HSADC_CHANNEL_ID_EN_ADD = (1 << 5),		/*!< add channel ID to FIFO output data */
} HSADC_CHANNEL_ID_EN_T;

/**
 * @brief	Configure HSADC trigger source and recovery time
 * @param	pHSADC			: The base of HSADC peripheral on the chip
 * @param	mask			: HSADC trigger configuration mask type
 * @param	mode			: HSADC trigger configuration mode type
 * @param	sync			: HSADC trigger configuration sync type
 * @param	chID			: HSADC trigger configuration channel ID enable
 * @param	recoveryTime	: ADC recovery time (in HSADC clocks) from powerdown (255 max)
 * @return	Nothing
 */
STATIC INLINE void Chip_HSADC_ConfigureTrigger(LPC_HSADC_T *pHSADC,
											   HSADC_TRIGGER_MASK_T mask,
											   HSADC_TRIGGER_MODE_T mode,
											   HSADC_TRIGGER_SYNC_T sync,
											   HSADC_CHANNEL_ID_EN_T chID, uint16_t recoveryTime)
{
	pHSADC->CONFIG = (uint32_t) mask | (uint32_t) mode | (uint32_t) sync |
					 (uint32_t) chID | (uint32_t) (recoveryTime << 6);
}

/**
 * @brief	Set HSADC Threshold low value
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @param   thrnum	: Threshold register value (0 for threshold register A, 1 for threshold register B)
 * @param   value	: Threshold low data value (should be 12-bit value)
 * @return	None
 */
void Chip_HSADC_SetThrLowValue(LPC_HSADC_T *pHSADC, uint8_t thrnum, uint16_t value);

/**
 * @brief	Set HSADC Threshold high value
 * @param	pHSADC		: The base of HSADC peripheral on the chip
 * @param   thrnum      : Threshold register value (0 for threshold register A, 1 for threshold register B)
 * @param   value       : Threshold high data value (should be 12-bit value)
 * @return	None
 */
void Chip_HSADC_SetThrHighValue(LPC_HSADC_T *pHSADC, uint8_t thrnum, uint16_t value);

/** HSADC last sample registers bit fields */
#define HSADC_LS_DONE                    (1 << 0)		/*!< Sample conversion complete bit */
#define HSADC_LS_OVERRUN                 (1 << 1)		/*!< Sample overrun bit */
#define HSADC_LS_RANGE_IN                (0 << 2)		/*!< Threshold range comparison is in range */
#define HSADC_LS_RANGE_BELOW             (1 << 2)		/*!< Threshold range comparison is below range */
#define HSADC_LS_RANGE_ABOVE             (2 << 2)		/*!< Threshold range comparison is above range */
#define HSADC_LS_RANGE(val)              ((val) & 0xC)	/*!< Mask for threshold crossing comparison result */
#define HSADC_LS_CROSSING_NONE           (0 << 4)		/*!< No threshold crossing detected */
#define HSADC_LS_CROSSING_DOWN           (1 << 4)		/*!< Downward threshold crossing detected */
#define HSADC_LS_CROSSING_UP             (2 << 4)		/*!< Upward threshold crossing detected */
#define HSADC_LS_CROSSING(val)           ((val) & 0x30)	/*!< Mask for threshold crossing comparison result */
#define HSADC_LS_DATA(val)               ((val) >> 6)	/*!< Mask data value out of sample */

/**
 * @brief	Read a ADC last sample register
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	channel	: Last sample register to read, 0-5
 * @return	Current raw value of the indexed last sample register
 * @note	This function returns the raw value of the indexed last sample register
 * and clears the sample's DONE and OVERRUN statuses if set. You can determine
 * the overrun and datavalid status for the sample by masking the return value
 * with HSADC_LS_DONE or HSADC_LS_OVERRUN. To get the data value for the sample,
 * use the HSADC_LS_DATA(sample) macro. The threshold range and crossing results
 * can be determined by using the HSADC_LS_RANGE(sample) and
 * HSADC_LS_CROSSING(sample) macros and comparing the result against the
 * HSADC_LS_RANGE_* or HSADC_LS_CROSSING_* definitions.<br>
 */
STATIC INLINE uint32_t Chip_HSADC_GetLastSample(LPC_HSADC_T *pHSADC, uint8_t channel)
{
	return pHSADC->LAST_SAMPLE[channel];
}

/**
 * @brief	Setup speed for a input channel
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	channel	: Input to set, 0-5
 * @param	speed	: Speed value to set (0xF, 0xE, or 0x0), see user manual
 * @return	Nothing
 * @note	It is recommended not to use this function, as the values needed
 * for this register will be setup with the Chip_HSADC_SetPowerSpeed() function.
 */
void Chip_HSADC_SetSpeed(LPC_HSADC_T *pHSADC, uint8_t channel, uint8_t speed);

/**
 * @brief	Setup (common) HSADC power and speed settings
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	comp2	: True sets up for 2's complement, false sets up for offset binary data format
 * @return	Nothing
 * @note	This function sets up the HSADC current/power/speed settings that
 * apply to all HSADC channels (inputs). Based on the HSADC clock rate, it will
 * automatically setup the best current setting (CRS) and speed settings (DGEC)
 * for all channels. (See user manual).<br>
 * This function is also used to set the data format of the sampled data. It is
 * recommended to call this function if the HSADC sample rate changes.
 */
void Chip_HSADC_SetPowerSpeed(LPC_HSADC_T *pHSADC, bool comp2);

/* AC-DC coupling selection for vin_neg and vin_pos sides */
typedef enum {
	HSADC_CHANNEL_NODCBIAS = 0,		/*!< No DC bias */
	HSADC_CHANNEL_DCBIAS = 1,		/*!< DC bias on vin_neg side */
} HSADC_DCBIAS_T;

/**
 * @brief	Setup AC-DC coupling selection for a channel
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	channel	: Input to set, 0-5
 * @param	dcInNeg	: AC-DC coupling selection on vin_neg side
 * @param	dcInPos	: AC-DC coupling selection on vin_pos side
 * @return	Nothing
 * @note	This function sets up the HSADC current/power/speed settings that
 * apply to all HSADC channels (inputs). Based on the HSADC clock rate, it will
 * automatically setup the best current setting (CRS) and speed settings (DGEC)
 * for all channels. (See user manual).<br>
 * This function is also used to set the data format of the sampled data. It is
 * recommended to call this function if the HSADC sample rate changes.
 */
void Chip_HSADC_SetACDCBias(LPC_HSADC_T *pHSADC, uint8_t channel,
							HSADC_DCBIAS_T dcInNeg, HSADC_DCBIAS_T dcInPos);

/**
 * @brief	Enable HSADC power control and band gap reference
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @return	Nothing
 * @note	This function enables both the HSADC power and band gap
 * reference.
 */
STATIC INLINE void Chip_HSADC_EnablePower(LPC_HSADC_T *pHSADC)
{
	pHSADC->POWER_CONTROL |= (1 << 17) | (1 << 18);
}

/**
 * @brief	Disable HSADC power control and band gap reference
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @return	Nothing
 * @note	This function disables both the HSADC power and band gap
 * reference.
 */
STATIC INLINE void Chip_HSADC_DisablePower(LPC_HSADC_T *pHSADC)
{
	pHSADC->POWER_CONTROL &= ~((1 << 17) | (1 << 18));
}

/** HSADC FIFO registers bit fields for unpacked sample in lower 16 bits */
#define HSADC_FIFO_SAMPLE_MASK      (0xFFF)					/*!< 12-bit sample mask (unpacked) */
#define HSADC_FIFO_SAMPLE(val)      ((val) & 0xFFF)			/*!< Macro for stripping out unpacked sample data */
#define HSADC_FIFO_CHAN_ID_MASK     (0x7000)				/*!< Channel ID mask */
#define HSADC_FIFO_CHAN_ID(val)     (((val) >> 12) & 0x7)	/*!< Macro for stripping out sample data */
#define HSADC_FIFO_EMPTY            (0x1 << 15)				/*!< FIFO empty (invalid sample) */
#define HSADC_FIFO_SHIFTPACKED(val) ((val) >> 16)			/*!< Shifts the packed FIFO sample into the lower 16-bits of a word */
#define HSADC_FIFO_PACKEDMASK       (1UL << 31)				/*!< Packed sample check mask */

/**
 * @brief	Reads the HSADC FIFO
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @return	HSADC FIFO value
 * @note	This function reads and pops the HSADC FIFO. The FIFO
 * contents can be determined by using the HSADC_FIFO_* macros. If
 * FIFO packing is enabled, this may contain 2 samples. Use the
 * HSADC_FIFO_SHIFTPACKED macro to shift packed sample data into a
 * variable that can be used with the HSADC_FIFO_* macros. Note that
 * even if packing is enabled, the packed sample may not be valid.
 */
STATIC INLINE uint32_t Chip_HSADC_GetFIFO(LPC_HSADC_T *pHSADC)
{
	return pHSADC->FIFO_OUTPUT[0];
}

/** HSADC descriptor registers bit fields and support macros */
#define HSADC_DESC_CH(ch)           (ch)				/*!< Converter input channel */
#define HSADC_DESC_HALT             (1 << 3)			/*!< Descriptor halt after conversion bit */
#define HSADC_DESC_INT              (1 << 4)			/*!< Raise interrupt when ADC result is available bit */
#define HSADC_DESC_POWERDOWN        (1 << 5)			/*!< Power down after this conversion bit */
#define HSADC_DESC_BRANCH_NEXT      (0 << 6)			/*!< Continue with next descriptor */
#define HSADC_DESC_BRANCH_FIRST     (1 << 6)			/*!< Branch to the first descriptor */
#define HSADC_DESC_BRANCH_SWAP      (2 << 6)			/*!< Swap tables and branch to the first descriptor of the new table */
#define HSADC_DESC_MATCH(val)       ((val) << 8)		/*!< Match value used to trigger a descriptor */
#define HSADC_DESC_THRESH_NONE      (0 << 22)			/*!< No threshold detection performed */
#define HSADC_DESC_THRESH_A         (1 << 22)			/*!< Use A threshold detection */
#define HSADC_DESC_THRESH_B         (2 << 22)			/*!< Use B threshold detection */
#define HSADC_DESC_RESET_TIMER      (1 << 24)			/*!< Reset descriptor timer */
#define HSADC_DESC_UPDATE_TABLE     (1UL << 31)			/*!< Update table with all 8 descriptors of this table */

/**
 * @brief	Sets up a raw HSADC descriptor entry
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	table	: Descriptor table number, 0 or 1
 * @param	descNo	: Descriptor number to setup, 0 to 7
 * @param	desc	: Raw descriptor value (see notes)
 * @return	Nothing
 * @note	This function sets up a descriptor table entry. To setup
 * a descriptor entry, select a OR'ed combination of the HSADC_DESC_CH,
 * HSADC_DESC_HALT, HSADC_DESC_INT, HSADC_DESC_POWERDOWN, one of
 * HSADC_DESC_BRANCH_*, HSADC_DESC_MATCH, one of HSADC_DESC_THRESH_*, and
 * HSADC_DESC_RESET_TIMER definitions.<br>
 * Example for setting up a table 0, descriptor number 4 entry for input 0:<br>
 * Chip_HSADC_SetupDescEntry(LPC_HSADC, 0, 4, (HSADC_DESC_CH(0) | HSADC_DESC_HALT |
 *    HSADC_DESC_INT));
 */
STATIC INLINE void Chip_HSADC_SetupDescEntry(LPC_HSADC_T *pHSADC, uint8_t table,
											 uint8_t descNo, uint32_t desc)
{
	pHSADC->DESCRIPTOR[table][descNo] = desc;
}

/**
 * @brief	Update all descriptors of a table
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	table	: Descriptor table number, 0 or 1
 * @return	Nothing
 * @note	Updates descriptor table with all 8 descriptors. This
 * function should be used after all descriptors are setup with
 * the Chip_HSADC_SetupDescEntry() function.
 */
STATIC INLINE void Chip_HSADC_UpdateDescTable(LPC_HSADC_T *pHSADC, uint8_t table)
{
	pHSADC->DESCRIPTOR[table][0] |= HSADC_DESC_UPDATE_TABLE;
}

/* Interrupt selection for interrupt 0 set - these interrupts and statuses
   should only be used with the interrupt 0 register set */
#define HSADC_INT0_FIFO_FULL         (1 << 0)		/*!< number of samples in FIFO is more than FIFO_LEVEL */
#define HSADC_INT0_FIFO_EMPTY        (1 << 1)		/*!< FIFO is empty */
#define HSADC_INT0_FIFO_OVERFLOW     (1 << 2)		/*!< FIFO was full; conversion sample is not stored and lost */
#define HSADC_INT0_DSCR_DONE         (1 << 3)		/*!< The descriptor INTERRUPT field was enabled and its sample is converted */
#define HSADC_INT0_DSCR_ERROR        (1 << 4)		/*!< The ADC was not fully woken up when a sample was converted and the conversion results is unreliable */
#define HSADC_INT0_ADC_OVF           (1 << 5)		/*!< Converted sample value was over range of the 12 bit output code */
#define HSADC_INT0_ADC_UNF           (1 << 6)		/*!< Converted sample value was under range of the 12 bit output code */

/* Interrupt selection for interrupt 1 set - these interrupts and statuses
   should only be used with the interrupt 1 register set */
#define HSADC_INT1_THCMP_BRANGE(ch)  (1 << ((ch * 5) + 0))	/*!< Input channel result below range */
#define HSADC_INT1_THCMP_ARANGE(ch)  (1 << ((ch * 5) + 1))	/*!< Input channel result above range */
#define HSADC_INT1_THCMP_DCROSS(ch)  (1 << ((ch * 5) + 2))	/*!< Input channel result downward threshold crossing detected */
#define HSADC_INT1_THCMP_UCROSS(ch)  (1 << ((ch * 5) + 3))	/*!< Input channel result upward threshold crossing detected */
#define HSADC_INT1_OVERRUN(ch)       (1 << ((ch * 5) + 4))	/*!< New conversion on channel completed and has overwritten the previous contents of register LAST_SAMPLE [0] before it has been read */

/**
 * @brief	Enable an interrupt for HSADC interrupt group 0 or 1
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	intGrp	: Interrupt group 0 or 1
 * @param	intMask	: Interrupts to enable, use HSADC_INT0_* for group 0
 *                    and HSADC_INT1_* values for group 1
 * @return	Nothing
 */
STATIC INLINE void Chip_HSADC_EnableInts(LPC_HSADC_T *pHSADC, uint8_t intGrp, uint32_t intMask)
{
	pHSADC->INTS[intGrp].SET_EN = intMask;
}

/**
 * @brief	Disables an interrupt for HSADC interrupt group 0 or 1
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	intGrp	: Interrupt group 0 or 1
 * @param	intMask	: Interrupts to disable, use HSADC_INT0_* for group 0
 *                    and HSADC_INT1_* values for group 1
 * @return	Nothing
 */
STATIC INLINE void Chip_HSADC_DisableInts(LPC_HSADC_T *pHSADC, uint8_t intGrp, uint32_t intMask)
{
	pHSADC->INTS[intGrp].CLR_EN = intMask;
}

/**
 * @brief	Returns enabled interrupt for HSADC interrupt group 0 or 1
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	intGrp	: Interrupt group 0 or 1
 * @return	enabled interrupts for the selected group
 * @note	Mask the return value with a HSADC_INT0_* macro for group 0
 * or HSADC_INT1_* values for group 1 to determine which interrupts are enabled.
 */
STATIC INLINE uint32_t Chip_HSADC_GetEnabledInts(LPC_HSADC_T *pHSADC, uint8_t intGrp)
{
	return pHSADC->INTS[intGrp].MASK;
}

/**
 * @brief	Returns status for HSADC interrupt group 0 or 1
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	intGrp	: Interrupt group 0 or 1
 * @return	interrupt (pending) status for the selected group
 * @note	Mask the return value with a HSADC_INT0_* macro for group 0
 * or HSADC_INT1_* values for group 1 to determine which statuses are active.
 */
STATIC INLINE uint32_t Chip_HSADC_GetIntStatus(LPC_HSADC_T *pHSADC, uint8_t intGrp)
{
	return pHSADC->INTS[intGrp].STATUS;
}

/**
 * @brief	Clear a status for HSADC interrupt group 0 or 1
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	intGrp	: Interrupt group 0 or 1
 * @param	stsMask	: Statuses to clear, use HSADC_INT0_* for group 0
 *                    and HSADC_INT1_* values for group 1
 * @return	Nothing
 */
STATIC INLINE void Chip_HSADC_ClearIntStatus(LPC_HSADC_T *pHSADC, uint8_t intGrp, uint32_t stsMask)
{
	pHSADC->INTS[intGrp].CLR_STAT = stsMask;
}

/**
 * @brief	Sets a status for HSADC interrupt group 0 or 1
 * @param	pHSADC	: The base of ADC peripheral on the chip
 * @param	intGrp	: Interrupt group 0 or 1
 * @param	stsMask	: Statuses to set, use HSADC_INT0_* for group 0
 *                    and HSADC_INT1_* values for group 1
 * @return	Nothing
 */
STATIC INLINE void Chip_HSADC_SetIntStatus(LPC_HSADC_T *pHSADC, uint8_t intGrp, uint32_t stsMask)
{
	pHSADC->INTS[intGrp].SET_STAT = stsMask;
}

/**
 * @brief	Returns the clock rate in Hz for the HSADC
 * @param	pHSADC	: The base of HSADC peripheral on the chip
 * @return	clock rate in Hz for the HSADC
 */
STATIC INLINE uint32_t Chip_HSADC_GetBaseClockRate(LPC_HSADC_T *pHSADC)
{
	(void) pHSADC;

	/* Return computed sample rate for the high speed ADC peripheral */
	return Chip_Clock_GetRate(CLK_ADCHS);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __HSADC_18XX_43XX_H_ */






