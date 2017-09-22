/*
 * @brief LPC18xx/43xx RTC driver
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

#ifndef __RTC_18XX_43XX_H_
#define __RTC_18XX_43XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup RTC_18XX_43XX CHIP: LPC18xx/43xx Real Time Clock driver
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

#define RTC_EV_SUPPORT      1				/* Event Monitor/Recorder support */

/**
 * @brief RTC time type option
 */
typedef enum {
	RTC_TIMETYPE_SECOND,		/*!< Second */
	RTC_TIMETYPE_MINUTE,		/*!< Month */
	RTC_TIMETYPE_HOUR,			/*!< Hour */
	RTC_TIMETYPE_DAYOFMONTH,	/*!< Day of month */
	RTC_TIMETYPE_DAYOFWEEK,		/*!< Day of week */
	RTC_TIMETYPE_DAYOFYEAR,		/*!< Day of year */
	RTC_TIMETYPE_MONTH,			/*!< Month */
	RTC_TIMETYPE_YEAR,			/*!< Year */
	RTC_TIMETYPE_LAST
} RTC_TIMEINDEX_T;

#if RTC_EV_SUPPORT
/**
 * @brief Event Channel Identifier definitions
 */
typedef enum {
	RTC_EV_CHANNEL_1 = 0,
	RTC_EV_CHANNEL_2,
	RTC_EV_CHANNEL_3,
	RTC_EV_CHANNEL_NUM,
} RTC_EV_CHANNEL_T;
#endif /*RTC_EV_SUPPORT*/

/**
 * @brief Real Time Clock register block structure
 */
typedef struct {							/*!< RTC Structure          */
	__IO uint32_t  ILR;						/*!< Interrupt Location Register */
	__I  uint32_t  RESERVED0;
	__IO uint32_t  CCR;						/*!< Clock Control Register */
	__IO uint32_t  CIIR;					/*!< Counter Increment Interrupt Register */
	__IO uint32_t  AMR;						/*!< Alarm Mask Register    */
	__I  uint32_t  CTIME[3];				/*!< Consolidated Time Register 0,1,2 */
	__IO uint32_t  TIME[RTC_TIMETYPE_LAST];	/*!< Timer field registers */
	__IO uint32_t  CALIBRATION;				/*!< Calibration Value Register */
	__I  uint32_t  RESERVED1[7];
	__IO uint32_t  ALRM[RTC_TIMETYPE_LAST];	/*!< Alarm field registers */
#if RTC_EV_SUPPORT
	__IO uint32_t ERSTATUS;					/*!< Event Monitor/Recorder Status register*/
	__IO uint32_t ERCONTROL;				/*!< Event Monitor/Recorder Control register*/
	__I  uint32_t ERCOUNTERS;				/*!< Event Monitor/Recorder Counters register*/
	__I  uint32_t RESERVED2;
	__I  uint32_t ERFIRSTSTAMP[RTC_EV_CHANNEL_NUM];			/*!<Event Monitor/Recorder First Stamp registers*/
	__I  uint32_t RESERVED3;
	__I  uint32_t ERLASTSTAMP[RTC_EV_CHANNEL_NUM];			/*!<Event Monitor/Recorder Last Stamp registers*/
#endif /*RTC_EV_SUPPORT*/
} LPC_RTC_T;

/**
 * @brief Register File register block structure
 */
typedef struct {
	__IO uint32_t REGFILE[64];	/*!< General purpose storage register */
} LPC_REGFILE_T;

/*
 * @brief ILR register definitions
 */
/** ILR register mask */
#define RTC_ILR_BITMASK         ((0x00000003))
/** Bit inform the source interrupt is counter increment*/
#define RTC_IRL_RTCCIF          ((1 << 0))
/** Bit inform the source interrupt is alarm match*/
#define RTC_IRL_RTCALF          ((1 << 1))

/*
 * @brief CCR register definitions
 */
/** CCR register mask */
#define RTC_CCR_BITMASK         ((0x00000013))
/** Clock enable */
#define RTC_CCR_CLKEN           ((1 << 0))
/** Clock reset */
#define RTC_CCR_CTCRST          ((1 << 1))
/** Calibration counter enable */
#define RTC_CCR_CCALEN          ((1 << 4))

/*
 * @brief CIIR and AMR register definitions
 */
/** Counter Increment Interrupt bit for second */
#define RTC_AMR_CIIR_IMSEC          ((1 << 0))
/** Counter Increment Interrupt bit for minute */
#define RTC_AMR_CIIR_IMMIN          ((1 << 1))
/** Counter Increment Interrupt bit for hour */
#define RTC_AMR_CIIR_IMHOUR         ((1 << 2))
/** Counter Increment Interrupt bit for day of month */
#define RTC_AMR_CIIR_IMDOM          ((1 << 3))
/** Counter Increment Interrupt bit for day of week */
#define RTC_AMR_CIIR_IMDOW          ((1 << 4))
/** Counter Increment Interrupt bit for day of year */
#define RTC_AMR_CIIR_IMDOY          ((1 << 5))
/** Counter Increment Interrupt bit for month */
#define RTC_AMR_CIIR_IMMON          ((1 << 6))
/** Counter Increment Interrupt bit for year */
#define RTC_AMR_CIIR_IMYEAR         ((1 << 7))
/** CIIR bit mask */
#define RTC_AMR_CIIR_BITMASK        ((0xFF))

/*
 * @brief RTC_AUX register definitions
 */
/** RTC Oscillator Fail detect flag */
#define RTC_AUX_RTC_OSCF        ((1 << 4))

/*
 * @brief RTC_AUXEN register definitions
 */
/** Oscillator Fail Detect interrupt enable*/
#define RTC_AUXEN_RTC_OSCFEN    ((1 << 4))

/*
 * @brief Consolidated Time Register 0 definitions
 */
#define RTC_CTIME0_SECONDS_MASK     ((0x3F))
#define RTC_CTIME0_MINUTES_MASK     ((0x3F00))
#define RTC_CTIME0_HOURS_MASK       ((0x1F0000))
#define RTC_CTIME0_DOW_MASK         ((0x7000000))

/*
 * @brief Consolidated Time Register 1 definitions
 */
#define RTC_CTIME1_DOM_MASK         ((0x1F))
#define RTC_CTIME1_MONTH_MASK       ((0xF00))
#define RTC_CTIME1_YEAR_MASK        ((0xFFF0000))

/*
 * @brief Consolidated Time Register 2 definitions
 */
#define RTC_CTIME2_DOY_MASK         ((0xFFF))

/*
 * @brief Time Counter Group and Alarm register group
 */
/** SEC register mask */
#define RTC_SEC_MASK            (0x0000003F)
/** MIN register mask */
#define RTC_MIN_MASK            (0x0000003F)
/** HOUR register mask */
#define RTC_HOUR_MASK           (0x0000001F)
/** DOM register mask */
#define RTC_DOM_MASK            (0x0000001F)
/** DOW register mask */
#define RTC_DOW_MASK            (0x00000007)
/** DOY register mask */
#define RTC_DOY_MASK            (0x000001FF)
/** MONTH register mask */
#define RTC_MONTH_MASK          (0x0000000F)
/** YEAR register mask */
#define RTC_YEAR_MASK           (0x00000FFF)

#define RTC_SECOND_MAX      59	/*!< Maximum value of second */
#define RTC_MINUTE_MAX      59	/*!< Maximum value of minute*/
#define RTC_HOUR_MAX        23	/*!< Maximum value of hour*/
#define RTC_MONTH_MIN       1	/*!< Minimum value of month*/
#define RTC_MONTH_MAX       12	/*!< Maximum value of month*/
#define RTC_DAYOFMONTH_MIN  1	/*!< Minimum value of day of month*/
#define RTC_DAYOFMONTH_MAX  31	/*!< Maximum value of day of month*/
#define RTC_DAYOFWEEK_MAX   6	/*!< Maximum value of day of week*/
#define RTC_DAYOFYEAR_MIN   1	/*!< Minimum value of day of year*/
#define RTC_DAYOFYEAR_MAX   366	/*!< Maximum value of day of year*/
#define RTC_YEAR_MAX        4095/*!< Maximum value of year*/

/*
 * @brief Calibration register
 */
/** Calibration value */
#define RTC_CALIBRATION_CALVAL_MASK     ((0x1FFFF))
/** Calibration direction */
#define RTC_CALIBRATION_LIBDIR          ((1 << 17))
/** Calibration max value */
#define RTC_CALIBRATION_MAX             ((0x20000))
/** Calibration definitions */
#define RTC_CALIB_DIR_FORWARD           ((uint8_t) (0))
#define RTC_CALIB_DIR_BACKWARD          ((uint8_t) (1))

#if RTC_EV_SUPPORT
/*
 * @brief Event Monitor/Recorder Control register
 */
/**  Event Monitor/Recorder Control register mask */
#define RTC_ERCTRL_BITMASK          ((uint32_t) 0xC0F03C0F)
/** Enable event interrupt and wakeup */
#define RTC_ERCTRL_INTWAKE_EN       ((uint32_t) (1 << 0))
/** Enables automatically clearing the RTC general purpose registers when an event occurs*/
#define RTC_ERCTRL_GPCLEAR_EN       ((uint32_t) (1 << 1))
/** Select polarity for a channel event on the input pin.*/
#define RTC_ERCTRL_POL_NEGATIVE     (0)		/* Event as positive edge */
#define RTC_ERCTRL_POL_POSITIVE     ((uint32_t) (1 << 2))	/* Event as negative edge */
/** Enable event input.*/
#define RTC_ERCTRL_INPUT_EN         ((uint32_t) (1 << 3))
/** Configure a specific channel */
#define RTC_ERCTRL_CHANNEL_CONFIG_BITMASK(ch)   ((uint32_t) (0x0F << (10 * ch)))
#define RTC_ERCTRL_CHANNEL_CONFIG(ch, flag) ((uint32_t) (flag << (10 * ch)))

/** Enable Event Monitor/Recorder and select its operating frequency.*/
#define RTC_ERCTRL_MODE_MASK                (((uint32_t) 3) << 30)
#define RTC_ERCTRL_MODE_CLK_DISABLE         (((uint32_t) 0) << 30)
#define RTC_ERCTRL_MODE_16HZ                (((uint32_t) 1) << 30)
#define RTC_ERCTRL_MODE_64HZ                (((uint32_t) 2) << 30)
#define RTC_ERCTRL_MODE_1KHZ                (((uint32_t) 3) << 30)
#define RTC_ERCTRL_MODE(n)                  (((uint32_t) n) << 30)

/*
 * @brief Event Monitor/Recorder Status register
 */
/** Event Flag for a specific channel */
#define RTC_ERSTATUS_CHANNEL_EV(ch)               ((uint32_t) (1 << ch))		/* At least 1 event has occurred on a specific channel */
/** General purpose registers have been asynchronous cleared. */
#define RTC_ERSTATUS_GPCLEARED            ((uint32_t) (1 << 3))
/** An interrupt/wakeup request is pending.*/
#define RTC_ERSTATUS_WAKEUP            ((uint32_t) (((uint32_t) 1) << 31))

/*
 * @brief Event Monitor/Recorder Counter register
 */
/** Value of the counter for Events occurred on a specific channel */
#define RTC_ER_COUNTER(ch, n)            ((uint32_t) ((n >> (8 * ch)) & 0x07))

/*
 * @brief Event Monitor/Recorder TimeStamp register
 */
#define RTC_ER_TIMESTAMP_SEC(n)             ((uint32_t) (n & 0x3F))
#define RTC_ER_TIMESTAMP_MIN(n)             ((uint32_t) ((n >> 6) & 0x3F))
#define RTC_ER_TIMESTAMP_HOUR(n)            ((uint32_t) ((n >> 12) & 0x1F))
#define RTC_ER_TIMESTAMP_DOY(n)             ((uint32_t) ((n >> 17) & 0x1FF))

/**
 * @brief Event Monitor/Recorder Mode definition
 */
typedef enum IP_RTC_EV_MODE {
	RTC_EV_MODE_DISABLE = 0,		/*!< Event Monitor/Recoder is disabled */
	RTC_EV_MODE_ENABLE_16HZ =  1,	/*!< Event Monitor/Recoder is enabled and use 16Hz sample clock for event input */
	RTC_EV_MODE_ENABLE_64HZ = 2,	/*!< Event Monitor/Recoder is enabled and use 64Hz sample clock for event input */
	RTC_EV_MODE_ENABLE_1KHZ = 3,	/*!< Event Monitor/Recoder is enabled and use 1kHz sample clock for event input */
	RTC_EV_MODE_LAST,
} RTC_EV_MODE_T;

/**
 * @brief Event Monitor/Recorder Timestamp structure
 */
typedef struct {
	uint8_t     sec;		/*!<   Second */
	uint8_t     min;		/*!<   Minute */
	uint8_t     hour;		/*!<   Hour */
	uint16_t    dayofyear;	/*!<   Day of year */
} RTC_EV_TIMESTAMP_T;

#endif /*RTC_EV_SUPPORT*/

/**
 * @brief RTC enumeration
 */

/** @brief RTC interrupt source */
typedef enum {
	RTC_INT_COUNTER_INCREASE = RTC_IRL_RTCCIF,	/*!<  Counter Increment Interrupt */
	RTC_INT_ALARM = RTC_IRL_RTCALF				/*!< The alarm interrupt */
} RTC_INT_OPT_T;

typedef struct {
	uint32_t time[RTC_TIMETYPE_LAST];
} RTC_TIME_T;

/**
 * @brief	Reset clock tick counter in the RTC peripheral
 * @param	pRTC	: RTC peripheral selected
 * @return	None
 */
void Chip_RTC_ResetClockTickCounter(LPC_RTC_T *pRTC);

/**
 * @brief	Start/Stop RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	NewState	: New State of this function, should be:
 *							- ENABLE	:The time counters are enabled
 *							- DISABLE	:The time counters are disabled
 * @return	None
 */
void Chip_RTC_Enable(LPC_RTC_T *pRTC, FunctionalState NewState);

/**
 * @brief	Enable/Disable Counter increment interrupt for a time type in the RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	cntrMask	: Or'ed bit values for time types (RTC_AMR_CIIR_IM*)
 * @param	NewState	: ENABLE or DISABLE
 * @return	None
 */
void Chip_RTC_CntIncrIntConfig(LPC_RTC_T *pRTC, uint32_t cntrMask, FunctionalState NewState);

/**
 * @brief	Enable/Disable Alarm interrupt for a time type in the RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	alarmMask	: Or'ed bit values for ALARM types (RTC_AMR_CIIR_IM*)
 * @param	NewState	: ENABLE or DISABLE
 * @return	None
 */
void Chip_RTC_AlarmIntConfig(LPC_RTC_T *pRTC, uint32_t alarmMask, FunctionalState NewState);

/**
 * @brief	Set current time value for a time type in the RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	Timetype	: time field index type to set
 * @param	TimeValue	: Value to palce in time field
 * @return	None
 */
STATIC INLINE void Chip_RTC_SetTime(LPC_RTC_T *pRTC, RTC_TIMEINDEX_T Timetype, uint32_t TimeValue)
{
	pRTC->TIME[Timetype] = TimeValue;
}

/**
 * @brief	Get current time value for a type time type
 * @param	pRTC		: RTC peripheral selected
 * @param	Timetype	: Time field index type to get
 * @return	Value of time field according to specified time type
 */
STATIC INLINE uint32_t Chip_RTC_GetTime(LPC_RTC_T *pRTC, RTC_TIMEINDEX_T Timetype)
{
	return pRTC->TIME[Timetype];
}

/**
 * @brief	Set full time in the RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	pFullTime	: Pointer to full time data
 * @return	None
 */
void Chip_RTC_SetFullTime(LPC_RTC_T *pRTC, RTC_TIME_T *pFullTime);

/**
 * @brief	Get full time from the RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	pFullTime	: Pointer to full time record to fill
 * @return	None
 */
void Chip_RTC_GetFullTime(LPC_RTC_T *pRTC, RTC_TIME_T *pFullTime);

/**
 * @brief	Set alarm time value for a time type
 * @param	pRTC		: RTC peripheral selected
 * @param	Timetype	: Time index field to set
 * @param	ALValue		: Alarm time value to set
 * @return	None
 */
STATIC INLINE void Chip_RTC_SetAlarmTime(LPC_RTC_T *pRTC, RTC_TIMEINDEX_T Timetype, uint32_t ALValue)
{
	pRTC->ALRM[Timetype] = ALValue;
}

/**
 * @brief	Get alarm time value for a time type
 * @param	pRTC		: RTC peripheral selected
 * @param	Timetype	: Time index field to get
 * @return	Value of Alarm time according to specified time type
 */
STATIC INLINE uint32_t Chip_RTC_GetAlarmTime(LPC_RTC_T *pRTC, RTC_TIMEINDEX_T Timetype)
{
	return pRTC->ALRM[Timetype];
}

/**
 * @brief	Set full alarm time in the RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	pFullTime	: Pointer to full time record to set alarm
 * @return	None
 */
void Chip_RTC_SetFullAlarmTime(LPC_RTC_T *pRTC, RTC_TIME_T *pFullTime);

/**
 * @brief	Get full alarm time in the RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	pFullTime	: Pointer to full time record to fill
 * @return	None
 */
void Chip_RTC_GetFullAlarmTime(LPC_RTC_T *pRTC, RTC_TIME_T *pFullTime);

/**
 * @brief	Write value to General purpose registers
 * @param	pRegFile	: RegFile peripheral selected
 * @param	index		: General purpose register index
 * @param	value		: Value to write
 * @return	None
 * @note	These General purpose registers can be used to store important
 * information when the main power supply is off. The value in these
 * registers is not affected by chip reset. These registers are
 * powered in the RTC power domain.
 */
STATIC INLINE void Chip_REGFILE_Write(LPC_REGFILE_T *pRegFile, uint8_t index, uint32_t value)
{
	pRegFile->REGFILE[index] = value;
}

/**
 * @brief	Read value from General purpose registers
 * @param	pRegFile	: RegFile peripheral selected
 * @param	index		: General purpose register index
 * @return	Read Value
 * @note	These General purpose registers can be used to store important
 * information when the main power supply is off. The value in these
 * registers is not affected by chip reset. These registers are
 * powered in the RTC power domain.
 */
STATIC INLINE uint32_t Chip_REGFILE_Read(LPC_REGFILE_T *pRegFile, uint8_t index)
{
	return pRegFile->REGFILE[index];
}

/**
 * @brief	Enable/Disable calibration counter in the RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	NewState	: New State of this function, should be:
 *							- ENABLE	:The calibration counter is enabled and counting
 *							- DISABLE	:The calibration counter is disabled and reset to zero
 * @return	None
 */
void Chip_RTC_CalibCounterCmd(LPC_RTC_T *pRTC, FunctionalState NewState);

/**
 * @brief	Configures Calibration in the RTC peripheral
 * @param	pRTC		: RTC peripheral selected
 * @param	CalibValue	: Calibration value, should be in range from 0 to 131,072
 * @param	CalibDir	: Calibration Direction, should be:
 *							- RTC_CALIB_DIR_FORWARD		:Forward calibration
 *							- RTC_CALIB_DIR_BACKWARD	:Backward calibration
 * @return	None
 */
STATIC INLINE void Chip_RTC_CalibConfig(LPC_RTC_T *pRTC, uint32_t CalibValue, uint8_t CalibDir)
{
	pRTC->CALIBRATION = ((CalibValue - 1) & RTC_CALIBRATION_CALVAL_MASK)
						| ((CalibDir == RTC_CALIB_DIR_BACKWARD) ? RTC_CALIBRATION_LIBDIR : 0);
}

/**
 * @brief	Clear specified Location interrupt pending in the RTC peripheral
 * @param	pRTC	: RTC peripheral selected
 * @param	IntType	: Interrupt location type, should be:
 *						- RTC_INT_COUNTER_INCREASE	:Clear Counter Increment Interrupt pending.
 *						- RTC_INT_ALARM				:Clear alarm interrupt pending
 * @return	None
 */
STATIC INLINE void Chip_RTC_ClearIntPending(LPC_RTC_T *pRTC, uint32_t IntType)
{
	pRTC->ILR = IntType;
}

/**
 * @brief	Check whether if specified location interrupt in the RTC peripheral is set or not
 * @param	pRTC	: RTC peripheral selected
 * @param	IntType	: Interrupt location type, should be:
 *						- RTC_INT_COUNTER_INCREASE: Counter Increment Interrupt block generated an interrupt.
 *						- RTC_INT_ALARM: Alarm generated an interrupt.
 * @return	New state of specified Location interrupt in RTC peripheral, SET OR RESET
 */
STATIC INLINE IntStatus Chip_RTC_GetIntPending(LPC_RTC_T *pRTC, uint32_t IntType)
{
	return (pRTC->ILR & IntType) ? SET : RESET;
}

#if RTC_EV_SUPPORT

/**
 * @brief	Configure a specific event channel
 * @param	pRTC	: RTC peripheral selected
 * @param	ch		: Channel number
 * @param	flag	: Configuration flag
 * @return	None
 * @note	flag is or-ed bit value of RTC_ERCTRL_INTWAKE_EN,RTC_ERCTRL_GPCLEAR_EN,
 *       RTC_ERCTRL_POL_POSITIVE and RTC_ERCTRL_INPUT_EN.
 */
STATIC INLINE void Chip_RTC_EV_Config(LPC_RTC_T *pRTC, RTC_EV_CHANNEL_T ch, uint32_t flag)
{
	uint32_t temp;

	temp = pRTC->ERCONTROL & (~(RTC_ERCTRL_CHANNEL_CONFIG_BITMASK(ch))) & RTC_ERCTRL_BITMASK;
	pRTC->ERCONTROL = temp | (RTC_ERCTRL_CHANNEL_CONFIG(ch, flag) & RTC_ERCTRL_BITMASK);
}

/**
 * @brief	Enable/Disable and select clock frequency for Event Monitor/Recorder
 * @param	pRTC	: RTC peripheral selected
 * @param	mode	: selected mode
 * @return	None
 */
STATIC INLINE void Chip_RTC_EV_SetMode(LPC_RTC_T *pRTC, RTC_EV_MODE_T mode)
{
	uint32_t temp;

	temp = pRTC->ERCONTROL & (~RTC_ERCTRL_MODE_MASK) & RTC_ERCTRL_BITMASK;
	pRTC->ERCONTROL = temp | RTC_ERCTRL_MODE(mode);
}

/**
 * @brief	Get Event Monitor/Recorder Status
 * @param	pRTC	: RTC peripheral selected
 * @return	Or-ed bit value of RTC_ERSTATUS_GPCLEARED and RTC_ERSTATUS_WAKEUP
 */
STATIC INLINE uint8_t Chip_RTC_EV_GetStatus(LPC_RTC_T *pRTC)
{
	return pRTC->ERSTATUS & (RTC_ERSTATUS_GPCLEARED | RTC_ERSTATUS_WAKEUP);
}

/**
 * @brief	Clear Event Monitor/Recorder Status
 * @param	pRTC	: RTC peripheral selected
 * @param	flag	: Or-ed bit value of RTC_ERSTATUS_GPCLEARED and RTC_ERSTATUS_WAKEUP
 * @return	Nothing
 */
STATIC INLINE void Chip_RTC_EV_ClearStatus(LPC_RTC_T *pRTC, uint32_t flag)
{
	pRTC->ERSTATUS = flag & (RTC_ERSTATUS_GPCLEARED | RTC_ERSTATUS_WAKEUP);
}

/**
 * @brief	Get status of a specific event channel
 * @param	pRTC	: RTC peripheral selected
 * @param	ch		: Channel number
 * @return	SET (At least 1 event occurred on the channel), RESET: no event occured.
 */
STATIC INLINE FlagStatus Chip_RTC_EV_GetChannelStatus(LPC_RTC_T *pRTC, RTC_EV_CHANNEL_T ch)
{
	return (pRTC->ERSTATUS & RTC_ERSTATUS_CHANNEL_EV(ch)) ? SET : RESET;
}

/**
 * @brief	Clear status of a specific event channel
 * @param	pRTC	: RTC peripheral selected
 * @param	ch		: Channel number
 * @return	Nothing.
 */
STATIC INLINE void Chip_RTC_EV_ClearChannelStatus(LPC_RTC_T *pRTC, RTC_EV_CHANNEL_T ch)
{
	pRTC->ERSTATUS = RTC_ERSTATUS_CHANNEL_EV(ch);
}

/**
 * @brief	Get counter value of a specific event channel
 * @param	pRTC	: RTC peripheral selected
 * @param	ch		: Channel number
 * @return	counter value
 */
STATIC INLINE uint8_t Chip_RTC_EV_GetCounter(LPC_RTC_T *pRTC, RTC_EV_CHANNEL_T ch)
{
	return RTC_ER_COUNTER(ch, pRTC->ERCOUNTERS);
}

/**
 * @brief	Get first time stamp of a specific event channel
 * @param	pRTC		: RTC peripheral selected
 * @param	ch			: Channel number
 * @param	pTimeStamp	: pointer to Timestamp buffer
 * @return	Nothing.
 */
void Chip_RTC_EV_GetFirstTimeStamp(LPC_RTC_T *pRTC, RTC_EV_CHANNEL_T ch, RTC_EV_TIMESTAMP_T *pTimeStamp);

/**
 * @brief	Get last time stamp of a specific event channel
 * @param	pRTC		: RTC peripheral selected
 * @param	ch			: Channel number
 * @param	pTimeStamp	: pointer to Timestamp buffer
 * @return	Nothing.
 */
void Chip_RTC_EV_GetLastTimeStamp(LPC_RTC_T *pRTC, RTC_EV_CHANNEL_T ch, RTC_EV_TIMESTAMP_T *pTimeStamp);

#endif /*RTC_EV_SUPPORT*/

/**
 * @brief	Initialize the RTC peripheral
 * @param	pRTC	: RTC peripheral selected
 * @return	None
 */
void Chip_RTC_Init(LPC_RTC_T *pRTC);

/**
 * @brief	De-initialize the RTC peripheral
 * @param	pRTC	: RTC peripheral selected
 * @return	None
 */
void Chip_RTC_DeInit(LPC_RTC_T *pRTC);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_18XX_43XX_H_ */






