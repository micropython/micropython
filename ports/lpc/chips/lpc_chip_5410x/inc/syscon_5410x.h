/*
 * @brief LPC5410X System & Control driver inclusion file
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

#ifndef __SYSCON_5410X_H_
#define __SYSCON_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SYSCON_5410X CHIP: LPC5410X System and Control Driver
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */

/**
 * @brief LPC5410X Main system configuration register block structure
 */
typedef struct {
	__IO uint32_t SYSMEMREMAP;			/*!< System Remap register */
	__I  uint32_t RESERVED0[4];
	__IO uint32_t SYSTCKCAL;			/*!< System Tick Calibration register */
	__I  uint32_t RESERVED1[1];
	__IO uint32_t NMISRC;				/*!< NMI Source select register */
	__IO uint32_t ASYNCAPBCTRL;			/*!< Asynch APB chiplet control register */
	__I  uint32_t RESERVED2[7];
	__IO uint32_t SYSRSTSTAT;			/*!< System Reset Stat register */
	__IO uint32_t PRESETCTRL[2];		/*!< Peripheral Reset Ctrl register */
	__IO uint32_t PRESETCTRLSET[2];		/*!< Peripheral Reset Ctrl Set register */
	__IO uint32_t PRESETCTRLCLR[2];		/*!< Peripheral Reset Ctrl Clr register */
	__IO uint32_t PIOPORCAP[2];			/*!< PIO Power-On Reset Capture register */
	__I  uint32_t RESERVED3[1];
	__IO uint32_t PIORESCAP[2];			/*!< PIO Pad Reset Capture register */
	__I  uint32_t RESERVED4[4];
	__IO uint32_t MAINCLKSELA;			/*!< Main Clk sel Source Sel A register */
	__IO uint32_t MAINCLKSELB;			/*!< Main Clk sel Source Sel B register */
	__I  uint32_t RESERVED5;
	__IO uint32_t ADCCLKSEL;			/*!< ADC Async Clk Sel register */
	__I  uint32_t RESERVED6;
	__IO uint32_t CLKOUTSELA;			/*!< Clk Out Sel Source A register */
	__IO uint32_t CLKOUTSELB;			/*!< Clk Out Sel Source B register */
	__I  uint32_t RESERVED7;
	__IO uint32_t SYSPLLCLKSEL;			/*!< System PLL Clk Selregister */
	__I  uint32_t RESERVED8[7];
	__IO uint32_t AHBCLKCTRL[2];		/*!< AHB Peripheral Clk Enable register */
	__IO uint32_t AHBCLKCTRLSET[2];		/*!< AHB Peripheral Clk Enable Set register */
	__IO uint32_t AHBCLKCTRLCLR[2];		/*!< AHB Peripheral Clk Enable Clr register */
	__I  uint32_t RESERVED9[2];
	__IO uint32_t SYSTICKCLKDIV;		/*!< Systick Clock divider register */
	__I  uint32_t RESERVED10[7];
	__IO uint32_t AHBCLKDIV;			/*!< Main Clk Divider register */
	__IO uint32_t RESERVED11;
	__IO uint32_t ADCCLKDIV;			/*!< ADC Async Clk Divider register */
	__IO uint32_t CLKOUTDIV;			/*!< Clk Out Divider register */
	__I  uint32_t RESERVED12[4];
	__IO uint32_t FREQMECTRL;			/*!< Frequency Measure Control register */
	__IO uint32_t FLASHCFG;				/*!< Flash Config register */
	__I  uint32_t RESERVED13[8];
	__IO uint32_t FIFOCTRL;				/*!< VFIFO control register */
	__I  uint32_t RESERVED14[14];
	__I  uint32_t RESERVED15[1];
	__I  uint32_t RESERVED16[2];
	__IO uint32_t RTCOSCCTRL;			/*!< RTC Oscillator Control register */
	__I  uint32_t RESERVED17[7];
	__IO uint32_t SYSPLLCTRL;			/*!< System PLL control register */
	__IO uint32_t SYSPLLSTAT;			/*!< PLL status register */
	__IO uint32_t SYSPLLNDEC;			/*!< PLL N decoder register */
	__IO uint32_t SYSPLLPDEC;			/*!< PLL P decoder register */
	__IO uint32_t SYSPLLSSCTRL[2];	/*!< Spread Spectrum control registers */
	__I  uint32_t RESERVED18[18];
	__IO uint32_t PDRUNCFG;				/*!< Power Down Run Config register */
	__IO uint32_t PDRUNCFGSET;			/*!< Power Down Run Config Set register */
	__IO uint32_t PDRUNCFGCLR;			/*!< Power Down Run Config Clr register */
	__I  uint32_t RESERVED19[9];
	__IO uint32_t STARTERP[2];			/*!< Start Signal Enable Register */
	__IO uint32_t STARTERSET[2];		/*!< Start Signal Enable Set Register */
	__IO uint32_t STARTERCLR[2];		/*!< Start Signal Enable Clr Register */
	__I  uint32_t RESERVED20[42];
	__I  uint32_t RESERVED20A[4];
	__I  uint32_t RESERVED21[57];
	__IO uint32_t JTAG_IDCODE;
	__IO uint32_t DEVICE_ID0;			/*!< Boot ROM and die revision register */
	__IO uint32_t DEVICE_ID1;			/*!< Boot ROM and die revision register */
} LPC_SYSCON_T;

/**
 * @brief LPC5410X Asynchronous system configuration register block structure
 */
typedef struct {
	__IO uint32_t AYSNCPRESETCTRL;		/*!< peripheral reset register */
	__IO uint32_t ASYNCPRESETCTRLSET;	/*!< peripheral reset Set register */
	__IO uint32_t ASYNCPRESETCTRLCLR;	/*!< peripheral reset Clr register */
	__I  uint32_t RESERVED0;
	__IO uint32_t ASYNCAPBCLKCTRL;		/*!< clk enable register */
	__IO uint32_t ASYNCAPBCLKCTRLSET;	/*!< clk enable Set register */
	__IO uint32_t ASYNCAPBCLKCTRLCLR;	/*!< clk enable Clr register */
	__I  uint32_t RESERVED1;
	__IO uint32_t ASYNCAPBCLKSELA;		/*!< clk source mux A register */
	__IO uint32_t ASYNCAPBCLKSELB;		/*!< clk source mux B register */
	__IO uint32_t ASYNCCLKDIV;			/*!< clk div register */
	__I  uint32_t RESERVED2;
	__IO uint32_t FRGCTRL;				/*!< Fraction Rate Generator Ctrl register */
} LPC_ASYNC_SYSCON_T;

/**
 * System memory remap modes used to remap interrupt vectors
 */
typedef enum CHIP_SYSCON_BOOT_MODE_REMAP {
	REMAP_BOOT_LOADER_MODE,	/*!< Interrupt vectors are re-mapped to Boot ROM */
	REMAP_USER_RAM_MODE,	/*!< Interrupt vectors are re-mapped to user Static RAM */
	REMAP_USER_FLASH_MODE	/*!< Interrupt vectors are not re-mapped and reside in Flash */
} CHIP_SYSCON_BOOT_MODE_REMAP_T;

/** @brief V4 CHIP PART ID */
#define V4_UID              (0x08C1FECE)

/**
 * @brief	Re-map interrupt vectors
 * @param	remap	: system memory map value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SYSCON_Map(CHIP_SYSCON_BOOT_MODE_REMAP_T remap)
{
	LPC_SYSCON->SYSMEMREMAP = (uint32_t) remap;
}

/**
 * @brief	Get system remap setting
 * @return	System remap setting
 */
__STATIC_INLINE CHIP_SYSCON_BOOT_MODE_REMAP_T Chip_SYSCON_GetMemoryMap(void)
{
	return (CHIP_SYSCON_BOOT_MODE_REMAP_T) LPC_SYSCON->SYSMEMREMAP;
}

/**
 * @brief	Set System tick timer calibration value
 * @param	sysCalVal	: System tick timer calibration value
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SYSCON_SetSYSTCKCAL(uint32_t sysCalVal)
{
	LPC_SYSCON->SYSTCKCAL = sysCalVal;
}

/**
 * Non-Maskable Interrupt Enable/Disable value
 */
#define SYSCON_NMISRC_M0_ENABLE   ((uint32_t) 1 << 30)	/*!< Enable the Non-Maskable Interrupt M0 (NMI) source */
#define SYSCON_NMISRC_M4_ENABLE   ((uint32_t) 1 << 31)	/*!< Enable the Non-Maskable Interrupt M4 (NMI) source */

/**
 * @brief	Set source for non-maskable interrupt (NMI)
 * @param	intsrc	: IRQ number to assign to the NMI
 * @return	Nothing
 * @note	The NMI source will be disabled upon exiting this function. Use the
 * Chip_SYSCON_EnableNMISource() function to enable the NMI source.
 */
void Chip_SYSCON_SetNMISource(uint32_t intsrc);

/**
 * @brief	Enable interrupt used for NMI source
 * @return	Nothing
 */
void Chip_SYSCON_EnableNMISource(void);

/**
 * @brief	Disable interrupt used for NMI source
 * @return	Nothing
 */
void Chip_SYSCON_DisableNMISource(void);

/**
 * @brief	Enable or disable asynchronous APB bridge and subsystem
 * @param	enable	: true to enable, false to disable
 * @return	Nothing
 * @note	This bridge must be enabled to access peripherals on the
 * associated bridge.
 */
void Chip_SYSCON_Enable_ASYNC_Syscon(bool enable);

/**
 * @brief	Set UART Fractional divider value
 * @param	fmul	: Fractional multiplier value
 * @param	fdiv	: Fractional divider value (Must always be 0xFF)
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SYSCON_SetUSARTFRGCtrl(uint8_t fmul, uint8_t fdiv)
{
	LPC_ASYNC_SYSCON->FRGCTRL = ((uint32_t) fmul << 8) | fdiv;
}

/**
 * System reset status values
 */
#define SYSCON_RST_POR    (1 << 0)	/*!< POR reset status */
#define SYSCON_RST_EXTRST (1 << 1)	/*!< External reset status */
#define SYSCON_RST_WDT    (1 << 2)	/*!< Watchdog reset status */
#define SYSCON_RST_BOD    (1 << 3)	/*!< Brown-out detect reset status */
#define SYSCON_RST_SYSRST (1 << 4)	/*!< software system reset status */

/**
 * @brief	Get system reset status
 * @return	An Or'ed value of SYSCON_RST_*
 * @note	This function returns the detected reset source(s).
 */
__STATIC_INLINE uint32_t Chip_SYSCON_GetSystemRSTStatus(void)
{
	return LPC_SYSCON->SYSRSTSTAT;
}

/**
 * @brief	Clear system reset status
 * @param	reset	: An Or'ed value of SYSCON_RST_* status to clear
 * @return	Nothing
 * @note	This function clears the specified reset source(s).
 */
__STATIC_INLINE void Chip_SYSCON_ClearSystemRSTStatus(uint32_t reset)
{
	LPC_SYSCON->SYSRSTSTAT = reset;
}

/**
 * Peripheral reset identifiers
 */
typedef enum {
	/* Peripheral reset enables for PRESETCTRL0 */
	RESET_FLASH = 7,				/*!< Flash controller */
	RESET_FMC,						/*!< Flash accelerator */
	RESET_INMUX = 11,				/*!< Input mux */
	RESET_IOCON = 13,				/*!< IOCON */
	RESET_GPIO0,					/*!< GPIO Port 0 */
	RESET_GPIO1,					/*!< GPIO Port 1 */
	RESET_PINT = 18,				/*!< Pin interrupt */
	RESET_GINT,						/*!< Grouped interrupt (GINT) */
	RESET_DMA,						/*!< DMA */
	RESET_CRC,						/*!< CRC */
	RESET_WWDT,						/*!< Watchdog timer */
	RESET_RTC,						/*!< RTC */
	RESET_MAILBOX = 26,				/*!< Mailbox */
	RESET_ADC0,						/*!< ADC0 */

	/* Peripheral reset enables for PRESETCTRL1 */
	RESET_MRT = 32 + 0,				/*!< multi-rate timer */
	RESET_RIT,						/*!< Repetitive interrupt timer */
	RESET_SCT0,						/*!< SCT0 */
	RESET_FIFO = 32 + 9,			/*!< System FIFO */
	RESET_UTICK,					/*!< Micro-tick Timer */
	RESET_TIMER2 = 32 + 22,			/*!< TIMER2 */
	RESET_TIMER3 = 32 + 26,			/*!< TIMER3 */
	RESET_TIMER4,					/*!< TIMER4 */

	/* Async peripheral reset enables for ASYNCPRESETCTRL */
	RESET_USART0 = 128 + 1,			/*!< UART0 */
	RESET_USART1,					/*!< UART1 */
	RESET_USART2,					/*!< UART2 */
	RESET_USART3,					/*!< UART3 */
	RESET_I2C0,						/*!< I2C0 */
	RESET_I2C1,						/*!< I2C1 */
	RESET_I2C2,						/*!< I2C2 */
	RESET_SPI0 = 128 + 9,			/*!< SPI0 */
	RESET_SPI1,						/*!< SPI1 */
	RESET_TIMER0 = 128 + 13,		/*!< TIMER0 */
	RESET_TIMER1,					/*!< TIMER1 */
	RESET_FRG0						/*!< FRG */
} CHIP_SYSCON_PERIPH_RESET_T;

/**
 * @brief	Resets a peripheral
 * @param	periph	:	Peripheral to reset
 * @return	Nothing
 * Will assert and de-assert reset for a peripheral.
 */
void Chip_SYSCON_PeriphReset(CHIP_SYSCON_PERIPH_RESET_T periph);

/**
 * @brief	Read POR captured PIO status
 * @param	port	: 0 for port 0 pins, 1 for port 1 pins, 2 for port 2 pins, etc.
 * @return	captured Power-On-Reset (POR) PIO status
 */
__STATIC_INLINE uint32_t Chip_SYSCON_GetPORPIOStatus(uint8_t port)
{
	return LPC_SYSCON->PIOPORCAP[port];
}

/**
 * @brief	Read reset captured PIO status
 * @param	port	: 0 for port 0 pins, 1 for port 1 pins, 2 for port 2 pins, etc.
 * @return	captured reset PIO status
 * @note	Used when reset other than a Power-On-Reset (POR) occurs.
 */
__STATIC_INLINE uint32_t Chip_SYSCON_GetResetPIOStatus(uint8_t port)
{
	return LPC_SYSCON->PIORESCAP[port];
}

/**
 * @brief	Starts a frequency measurement cycle
 * @return	Nothing
 * @note	This function is meant to be used with the Chip_INMUX_SetFreqMeasRefClock()
 * and Chip_INMUX_SetFreqMeasTargClock() functions.
 */
__STATIC_INLINE void Chip_SYSCON_StartFreqMeas(void)
{
	LPC_SYSCON->FREQMECTRL = 0;
	LPC_SYSCON->FREQMECTRL = (1UL << 31);
}

/**
 * @brief	Indicates when a frequency measurement cycle is complete
 * @return	true if a measurement cycle is active, otherwise false
 */
__STATIC_INLINE bool Chip_SYSCON_IsFreqMeasComplete(void)
{
	return (bool) ((LPC_SYSCON->FREQMECTRL & (1UL << 31)) == 0);
}

/**
 * @brief	Returns the raw capture value for a frequency measurement cycle
 * @return	raw cpature value (this is not a frequency)
 */
__STATIC_INLINE uint32_t Chip_SYSCON_GetRawFreqMeasCapval(void)
{
	return LPC_SYSCON->FREQMECTRL & 0x3FFF;
}

/**
 * @brief	Returns the computed value for a frequency measurement cycle
 * @param	refClockRate	: Reference clock rate used during the frequency measurement cycle
 * @return	Computed cpature value
 */
uint32_t Chip_SYSCON_GetCompFreqMeas(uint32_t refClockRate);

/**
 * @brief FLASH Access time definitions
 */
typedef enum {
	SYSCON_FLASH_1CYCLE = 0,	/*!< Flash accesses use 1 CPU clock */
	FLASHTIM_20MHZ_CPU = SYSCON_FLASH_1CYCLE,
	SYSCON_FLASH_2CYCLE,		/*!< Flash accesses use 2 CPU clocks */
	SYSCON_FLASH_3CYCLE,		/*!< Flash accesses use 3 CPU clocks */
	SYSCON_FLASH_4CYCLE,		/*!< Flash accesses use 4 CPU clocks */
	SYSCON_FLASH_5CYCLE,		/*!< Flash accesses use 5 CPU clocks */
	SYSCON_FLASH_6CYCLE,		/*!< Flash accesses use 6 CPU clocks */
	SYSCON_FLASH_7CYCLE,		/*!< Flash accesses use 7 CPU clocks */
	SYSCON_FLASH_8CYCLE			/*!< Flash accesses use 8 CPU clocks */
} SYSCON_FLASHTIM_T;

/**
 * @brief	Set FLASH memory access time in clocks
 * @param	clks	: Clock cycles for FLASH access
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SYSCON_SetFLASHAccess(SYSCON_FLASHTIM_T clks)
{
	uint32_t tmp;

	tmp = LPC_SYSCON->FLASHCFG & ~(0xF << 12);

	/* Don't alter lower bits */
	LPC_SYSCON->FLASHCFG = tmp | ((uint32_t) clks << 12);
}

/**
 * @brief System FIFO bit definitions
 */
#define SYSCON_FIFO_U0TXFIFOEN      (1 << 0)	/*!< USART0 transmitter FIFO enable bit */
#define SYSCON_FIFO_U1TXFIFOEN      (1 << 1)	/*!< USART1 transmitter FIFO enable bit */
#define SYSCON_FIFO_U2TXFIFOEN      (1 << 2)	/*!< USART2 transmitter FIFO enable bit */
#define SYSCON_FIFO_U3TXFIFOEN      (1 << 3)	/*!< USART3 transmitter FIFO enable bit */
#define SYSCON_FIFO_SPI0TXFIFOEN    (1 << 4)	/*!< SPI0 transmitter FIFO enable bit */
#define SYSCON_FIFO_SPI1TXFIFOEN    (1 << 5)	/*!< SPI1 transmitter FIFO enable bit */
#define SYSCON_FIFO_U0RXFIFOEN      (1 << 8)	/*!< USART0 receiver FIFO enable bit */
#define SYSCON_FIFO_U1RXFIFOEN      (1 << 9)	/*!< USART1 receiver FIFO enable bit */
#define SYSCON_FIFO_U2RXFIFOEN      (1 << 10)	/*!< USART2 receiver FIFO enable bit */
#define SYSCON_FIFO_U3RXFIFOEN      (1 << 11)	/*!< USART3 receiver FIFO enable bit */
#define SYSCON_FIFO_SPI0RXFIFOEN    (1 << 12)	/*!< SPI0 receiver FIFO enable bit */
#define SYSCON_FIFO_SPI1RXFIFOEN    (1 << 13)	/*!< SPI1 receiver FIFO enable bit */

/**
 * @brief	Enable System FIFO(s) for a peripheral
 * @param	enMask	: Or'ed bits or type SYSCON_FIFO_* for enabling system FIFOs
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SYSCON_EnableSysFIFO(uint32_t enMask)
{
	LPC_SYSCON->FIFOCTRL |= enMask;
}

/**
 * @brief	Disable System FIFO(s) for a peripheral
 * @param	disMask	: Or'ed bits or type SYSCON_FIFO_* for disabling system FIFOs
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SYSCON_DisableSysFIFO(uint32_t disMask)
{
	LPC_SYSCON->FIFOCTRL &= ~disMask;
}

/**
 * Power control definition bits (0 = powered, 1 = powered down)
 */
#define SYSCON_PDRUNCFG_LP_VD1           (1 << 2)
#define SYSCON_PDRUNCFG_PD_IRC_OSC       (1 << 3)		/*!< IRC oscillator output */
#define SYSCON_PDRUNCFG_PD_IRC           (1 << 4)		/*!< IRC oscillator */
#define SYSCON_PDRUNCFG_PD_FLASH         (1 << 5)		/*!< Flash memory */
#define SYSCON_PDRUNCFG_PD_BOD_RST       (1 << 7)		/*!< Brown-out Detect reset */
#define SYSCON_PDRUNCFG_PD_BOD_INTR      (1 << 8)		/*!< Brown-out Detect interrupt */
#define SYSCON_PDRUNCFG_PD_VD2_ANA       (1 << 9)		/*!< Analog power */
#define SYSCON_PDRUNCFG_PD_ADC0          (1 << 10)		/*!< ADC0 */
#define SYSCON_PDRUNCFG_PD_SRAM0A        (1 << 13)		/*!< First 8 kB of SRAM0 */
#define SYSCON_PDRUNCFG_PD_SRAM0B        (1 << 14)		/*!< Remaining portion of SRAM0 */
#define SYSCON_PDRUNCFG_PD_SRAM1         (1 << 15)		/*!< SRAM1 */
#define SYSCON_PDRUNCFG_PD_SRAM2         (1 << 16)		/*!< SRAM2 */
#define SYSCON_PDRUNCFG_PD_ROM           (1 << 17)		/*!< ROM */
#define SYSCON_PDRUNCFG_PD_VDDA_ENA      (1 << 19)		/*!< Vdda to the ADC, must be enabled for the ADC to work */
#define SYSCON_PDRUNCFG_PD_WDT_OSC       (1 << 20)		/*!< Watchdog oscillator */
#define SYSCON_PDRUNCFG_PD_SYS_PLL       (1 << 22)		/*!< PLL0 */
#define SYSCON_PDRUNCFG_PD_VREFP         (1 << 23)		/*!< Vrefp to the ADC, must be enabled for the ADC to work */
#define SYSCON_PDRUNCFG_PD_32K_OSC       (1 << 24)		/*!< 32 kHz RTC oscillator */
#define SYSCON_PDRUNCFG_LP_VD2           (1 << 27)
#define SYSCON_PDRUNCFG_LP_VD3           (1 << 28)
#define SYSCON_PDRUNCFG_LP_VD8           (1UL << 29)

/**
 * @brief	Power up one or more blocks or peripherals
 * @return	OR'ed values of SYSCON_PDRUNCFG_* values
 * @note	A high state indicates the peripheral is powered down.
 */
__STATIC_INLINE uint32_t Chip_SYSCON_GetPowerStates(void)
{
	return LPC_SYSCON->PDRUNCFG;
}

/**
 * @brief	Power down one or more blocks or peripherals
 * @param	powerdownmask	: OR'ed values of SYSCON_PDRUNCFG_* values
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SYSCON_PowerDown(uint32_t powerdownmask)
{
	/* Disable peripheral states by setting high */
	LPC_SYSCON->PDRUNCFGSET = powerdownmask;
}

/**
 * @brief	Power up one or more blocks or peripherals
 * @param	powerupmask	: OR'ed values of SYSCON_PDRUNCFG_* values
 * @return	Nothing
 */
void Chip_SYSCON_PowerUp(uint32_t powerupmask);

/**
 * Start enable enumerations - for enabling and disabling peripheral wakeup
 */
typedef enum {
	SYSCON_STARTER_WWDT = 0,
	SYSCON_STARTER_BOD,
	SYSCON_STARTER_DMA = 3,
	SYSCON_STARTER_GINT0,
	SYSCON_STARTER_PINT0,
	SYSCON_STARTER_PINT1,
	SYSCON_STARTER_PINT2,
	SYSCON_STARTER_PINT3,
	SYSCON_STARTER_UTICK,
	SYSCON_STARTER_MRT,
	SYSCON_STARTER_TIMER0,
	SYSCON_STARTER_TIMER1,
	SYSCON_STARTER_TIMER2,
	SYSCON_STARTER_TIMER3,
	SYSCON_STARTER_TIMER4,
	SYSCON_STARTER_SCT0,
	SYSCON_STARTER_USART0,
	SYSCON_STARTER_USART1,
	SYSCON_STARTER_USART2,
	SYSCON_STARTER_USART3,
	SYSCON_STARTER_I2C0,
	SYSCON_STARTER_I2C1,
	SYSCON_STARTER_I2C2,
	SYSCON_STARTER_SPI0,
	SYSCON_STARTER_SPI1,
	SYSCON_STARTER_ADC0_SEQA,
	SYSCON_STARTER_ADC0_SEQB,
	SYSCON_STARTER_ADC0_THCMP,
	SYSCON_STARTER_RTC,
	SYSCON_STARTER_MAILBOX = 31,
	/* For M4 only */
	SYSCON_STARTER_GINT1 = 32 + 0,
	SYSCON_STARTER_PINT4,
	SYSCON_STARTER_PINT5,
	SYSCON_STARTER_PINT6,
	SYSCON_STARTER_PINT7,
	SYSCON_STARTER_RIT = 32 + 8,
} CHIP_SYSCON_WAKEUP_T;

/**
 * @brief	Enables a pin's (PINT) wakeup logic
 * @param	periphId	: Peripheral identifier
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SYSCON_EnableWakeup(CHIP_SYSCON_WAKEUP_T periphId)
{
	uint32_t pid = (uint32_t) periphId;

	if (pid < 32) {
		LPC_SYSCON->STARTERSET[0] = (1 << pid);
	}
	else {
		LPC_SYSCON->STARTERSET[1] = (1 << (pid - 32));
	}
}

/**
 * @brief	Disables peripheral's wakeup logic
 * @param	periphId	: Peripheral identifier
 * @return	Nothing
 */
__STATIC_INLINE void Chip_SYSCON_DisableWakeup(CHIP_SYSCON_WAKEUP_T periphId)
{
	uint32_t pid = (uint32_t) periphId;

	if (pid < 32) {
		LPC_SYSCON->STARTERCLR[0] = (1 << pid);
	}
	else {
		LPC_SYSCON->STARTERCLR[1] = (1 << (pid - 32));
	}
}

/**
 * @brief	Return the device ID
 * @return	Device ID
 */
__STATIC_INLINE uint32_t Chip_SYSCON_GetDeviceID(void)
{
	return LPC_SYSCON->DEVICE_ID0;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SYSCON_5410X_H_ */
