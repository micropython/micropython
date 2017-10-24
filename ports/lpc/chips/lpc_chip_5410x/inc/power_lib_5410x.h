/*
 * @brief LPC5410x Power library functions
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

#ifndef __POWER_LIB_5410X_H_
#define __POWER_LIB_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup POWER_LIBRARY_5410X CHIP: LPC5410X Power LIBRARY functions
 * The power library provides functions to control system power usage and
 * place the device into low power modes.<br>
 *
 * <b>Clock shutdown in sleep and power down modes</b><br>
 * When using the Chip_POWER_EnterPowerMode() function, system clocks are
 * shutdown based on the selected sleep or power down mode and the device
 * version being used. The following list details which clocks are shut down
 * in which modes for which device versions. You can keep a clock enabled
 * for a sleep or power down mode by enabling it in the 'peripheral_ctrl'
 * field in the Chip_POWER_EnterPowerMode() function.<br>
 *
 * Mode: Sleep<br>
 * No clocks are disabled for any chip version.<br>
 *
 * Mode: Deep sleep<br>
 * SYSCON_PDRUNCFG_PD_IRC_OSC<br>
 * SYSCON_PDRUNCFG_PD_IRC<br>
 * SYSCON_PDRUNCFG_PD_FLASH (v17.1 and later only)<br>
 * SYSCON_PDRUNCFG_PD_BOD_INTR<br>
 * SYSCON_PDRUNCFG_PD_ADC0<br>
 * SYSCON_PDRUNCFG_PD_ROM<br>
 * SYSCON_PDRUNCFG_PD_VDDA_ENA<br>
 * SYSCON_PDRUNCFG_PD_SYS_PLL<br>
 * SYSCON_PDRUNCFG_PD_VREFP<br>
 *
 * Mode: Power down<br>
 * SYSCON_PDRUNCFG_PD_IRC_OSC<br>
 * SYSCON_PDRUNCFG_PD_IRC<br>
 * SYSCON_PDRUNCFG_PD_FLASH (v17.1 and later only)<br>
 * SYSCON_PDRUNCFG_PD_BOD_RST<br>
 * SYSCON_PDRUNCFG_PD_BOD_INTR<br>
 * SYSCON_PDRUNCFG_PD_ADC0<br>
 * SYSCON_PDRUNCFG_PD_SRAM0B<br>
 * SYSCON_PDRUNCFG_PD_SRAM1<br>
 * SYSCON_PDRUNCFG_PD_SRAM2<br>
 * SYSCON_PDRUNCFG_PD_ROM<br>
 * SYSCON_PDRUNCFG_PD_VDDA_ENA<br>
 * SYSCON_PDRUNCFG_PD_WDT_OSC<br>
 * SYSCON_PDRUNCFG_PD_SYS_PLL<br>
 * SYSCON_PDRUNCFG_PD_VREFP<br>
 * SYSCON_PDRUNCFG_PD_32K_OSC<br>
 *
 * Mode: Deep power down<br>
 * All clocks are disabled for all chip versions.<br>
 *
 * If you are using a peripheral was a wakeup source for a power down mode,
 * it needs to be kept active with the call to Chip_POWER_EnterPowerMode(). For
 * example, if you are using the RTC to wake the system up from power down mode,
 * the 32KHz RTC oscillator needs to remain active, so the power down call would
 * look like this:<br>
 * Chip_POWER_EnterPowerMode(POWER_POWER_DOWN, SYSCON_PDRUNCFG_PD_32K_OSC);<br>
 * If your application uses internal RAM beyond the first 8K, you will also need
 * to prevent power down of the IRAM like this:<br>
 * Chip_POWER_EnterPowerMode(POWER_POWER_DOWN, (SYSCON_PDRUNCFG_PD_32K_OSC | SYSCON_PDRUNCFG_PD_SRAM0A));<br>
 * @ingroup CHIP_5410X_DRIVERS
 * @{
 */
// Saved in case they need to be re-installed publicly later
/* Power domains */
typedef enum  {
	POWER_VD1 = 0x0,
	POWER_VD2 = 0x1,
	POWER_VD3 = 0x2,
	POWER_VD8 = 0x3
} POWER_DOMAIN_T;

/* Power coarse voltage levels */
typedef enum {
	POWER_V0650 = 0x0,
	POWER_V0700 = 0x1,
	POWER_V0750 = 0x2,
	POWER_V0800 = 0x3,
	POWER_V0850 = 0x4,
	POWER_V0900 = 0x5,
	POWER_V0950 = 0x6,
	POWER_V1000 = 0x7,
	POWER_V1050 = 0x8,
	POWER_V1100 = 0x9,
	POWER_V1150 = 0xA,
	POWER_V1200 = 0xB,
	POWER_V1250 = 0xC,
	POWER_V1300 = 0xD,
	POWER_V1350 = 0xE,
	POWER_V1400 = 0xF
}  POWER_VOLTAGELEVEL_T;

/* Power fine voltage levels */
typedef enum {
	POWER_FINE_V_NONE = 0x0,
	POWER_FINE_V_M025 = 0x1,
	POWER_FINE_V_P025 = 0x3
} POWER_FINEVOLTAGELEVEL_T;

/* Low power voltage selection */
typedef enum {
	POWER_LP_V0700 = 0x0,
	POWER_LP_V1200 = 0x1
}  POWER_LPVOLTAGELEVEL_T;

/* Low power fine voltage selection */
typedef enum {
	POWER_FINE_LP_V_M050  = 0x0,
	POWER_FINE_LP_V_NONE  = 0x1,
	POWER_FINE_LP_V_P050  = 0x2,
	POWER_FINE_LP_V_P100  = 0x3
} POWER_LPFINEVOLTAGELEVEL_T;

/* 'mode' input values to set_voltage ROM function */
typedef enum {
	POWER_LOW_POWER_MODE = 0,
	POWER_BALANCED_MODE,
	POWER_HIGH_PERFORMANCE
} PERF_MODE_T;

/* 'mode' input values to power_mode_configure ROM function */
typedef enum {
	POWER_SLEEP = 0,
	POWER_DEEP_SLEEP,
	POWER_POWER_DOWN,
	POWER_DEEP_POWER_DOWN
} POWER_MODE_T;

/**
 * @brief	Sets up the System PLL given the PLL input frequency and feedback multiplier
 * @param	multiply_by	: PLL multiplier, minimum of 1, maximum of 16
 * @param	input_freq	: Input frequency into the PLL
 * @return	LPC_OK on success, or an error code (see error.h)
 */
uint32_t Chip_POWER_SetPLL(uint32_t multiply_by, uint32_t input_freq);

/**
 * @brief	Turn flash power on / off
 * @param	new_power_mode	: 0 = power off, != 0 = power on
 * @return	nothing
 */
void Chip_POWER_SetFLASHPower(uint32_t new_power_mode);

/**
 * @brief	Set optimal system voltage based on passed system frequency
 * @param	mode			: Power mode
 * @param	desired_freq	: System (CPU) frequency
 * @return	LPC_OK on success, or an error code (see error.h)
 * @note	This function will adjust the system voltages to the lowest
 * levels that will support the passed mode and CPU frequency.
 */
uint32_t Chip_POWER_SetVoltage(PERF_MODE_T mode, uint32_t desired_freq);

/**
 * @brief	Set voltage levels on individual voltage domains
 * @param	domain	: Power domain
 * @param	level	: Coarse voltage level
 * @param	flevel	: Fine voltage level
 * @return	Nothing
 */
uint32_t Chip_POWER_SetVDLevel(POWER_DOMAIN_T domain, POWER_VOLTAGELEVEL_T level, POWER_FINEVOLTAGELEVEL_T flevel);

/**
 * @brief	Set low-power voltage levels for LP mode
 * @return	Nothing
 */
void Chip_POWER_SetLPVDLevel(void);

/**
 * @brief	Enters the selected power state
 * @param	mode			: Power mode
 * @param	peripheral_ctrl	: Peripherals that will remain powered up in the power state
 * @return	Nothing
 * @note	The 'peripheral_ctrl' field is a bitmask of bits from the
 * PDRUNCFG register (SYSCON_PDRUNCFG_PD_*) that describe which
 * peripherals can wake up the chip from the power state. These
 * peripherals are not powered down during the power state.<br>
 */
void Chip_POWER_EnterPowerMode(POWER_MODE_T mode, uint32_t peripheral_ctrl);

/**
 * @brief	Enters the selected power state (Relocated)
 * @param	mode			: Power mode
 * @param	peripheral_ctrl	: Peripherals that will remain powered up in the power state
 * @param	relMem			: Address of the memory where relocated power function is [see Chip_POWER_RelocAPI()]
 * @return	Nothing
 * @note	This API is to be used when the powerdown mode [ @a mode is #POWER_POWER_DOWN ] is to be entered from flash.
 * Since powerdown mode turns off the flash memory the powerdown API, if executed from flash, can be relocated to RAM
 * using Chip_POWER_RelocAPI(), and powerdown mode can be entered using this API. @a relMem should be the pointer to the
 * memory used when calling Chip_POWER_RelocAPI(). For all power modes other than
 * #POWER_POWER_DOWN this API works exactly same as Chip_POWER_EnterPowerMode() API and @a relMem can be 0.<br>
 */
void Chip_POWER_EnterPowerModeReloc(POWER_MODE_T mode, uint32_t peripheral_ctrl, uint32_t relMem);

/**
 * @brief	Relocate the PowerDown code to given memory
 *
 * When executing from flash, entering PowerDown mode is not possible as the PowerDown mode turns
 * off the flash. This function will relocate the powerdown code to memory allocated in RAM and this
 * relocated function can be invoked using Chip_POWER_EnterPowerModeReloc() hence entering powerdown
 * mode from RAM and let the flash power be turned off.
 *
 * @param	relMem	: Memory where the function to be relocated [Needs to be word aligned]
 * @param	size	: Size of the memory in Bytes
 * @return	Nothing
 */
void Chip_POWER_RelocAPI(uint32_t *relMem, uint32_t size);

/* ROM versions */
#define LPC5410X_ROMVER_0   (0x1100)
#define LPC5410X_ROMVER_1   (0x1101)
#define LPC5410X_ROMVER_2   (0x1102)

/**
 * @brief	Fast powerdown for IRAM based applications
 * @param	peripheral_ctrl	: Peripherals that will remain powered up in the power down state
 * @return	Nothing
 * @note	The 'peripheral_ctrl' field is a bitmask of bits from the
 * PDRUNCFG register (SYSCON_PDRUNCFG_PD_*) that describe which
 * peripherals can wake up the chip from the power state. These
 * peripherals are not powered down during the power state.<br>
 * This function should only be used when not executing code in FLASH.
 * It will power down FLASH and leave it powered down on exit, so all
 * code should be placed in IRAM prior to calling. It provides a quicker
 * wakeup response than the default powerdown function
 * (Chip_POWER_EnterPowerMode(POWER_POWER_DOWN, ...)).
 */
void Chip_POWER_EnterPowerModeIramOnly(uint32_t peripheral_ctrl);

/**
 * @brief	Return ROM version
 * @return	ROM version
 * @note	Will return one of the following version numbers:<br>
 * (0x1100) for v17.0 ROMs.<br>
 * (0x1101) for v17.1 ROMs.<br>
 * (0x1102) for v17.2 ROMs.<br>
 */
uint32_t Chip_POWER_GetROMVersion(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __POWER_LIB_5410X_H_ */
