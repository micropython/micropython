/*
 * @brief LPC5410X PLL driver
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

#ifndef __PLL_5410X_H_
#define __PLL_5410X_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup PLL_5410X CHIP: LPC5410X PLL Driver
 * @ingroup CHIP_5410X_DRIVERS
 * The PLL in the LPC5410x is flexible, but can be complex to use. This driver
 * provides functions to help setup and use the PLL in it's various supported
 * modes.<br>
 *
 * This driver does not alter PLL clock source or system clocks outside the
 * PLL (like the main clock source) that may be referenced from the PLL. It
 * may optionally setup system voltages, wait for PLL lock, and power cycle
 * the PLL during setup based on setup flags.
 *
 * The driver works by first generating a PLL setup structure from a desired
 * PLL configuration structure. The PLL setup structure is then passed to the
 * PLL setup function to setup the PLL. In a user spplication, the PLL setup
 * structure can be pre-populated with PLL setup data to avoid using the PLL
 * configuration structure (or multiple PLL setup structures can be used to
 * more dynamically control PLL output rate).
 *
 * <b>How to use this driver</b><br>
   @verbatim
   // Setup PLL configuration
   PLL_CONFIG_T pllConfig = {
    75000000,		// desiredRate = 75MHz
    0,				// InputRate = 0Hz (not used)
    0				// No flags, function will determine best setup to get closest rate
   };

   // Get closest PLL setup to get the desired configuration
   PLL_SETUP_T pllSetup;
   uint32_t actualPllRate;
   PLL_ERROR_T pllError;
   pllError = Chip_Clock_SetupPLLData(&pllConfig, &pllSetup, &actualPllRate);
   if (pllError != PLL_ERROR_SUCCESS) {
    printf("PLL setup error #%x\r\n", (uint32_t) pllError);
    while (1);
   }
   else {
    printf("PLL config successful, actual config rate = %uHz\r\n", actualPllRate);
   }

   // Make sure main system clock is not using PLL, as the PLL setup
   // function will power off and optionally power on the PLL
   Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_IRC);

   // Setup PLL source
   Chip_Clock_SetSystemPLLSource(SYSCON_PLLCLKSRC_IRC);

   // Now to apply the configuration to the PLL
   pllSetup.flags = PLL_SETUPFLAG_WAITLOCK;
   Chip_Clock_SetupSystemPLLPrec(&pllSetup);

   // Switch main system clock to PLL
   Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_PLLOUT);
   @endverbatim
 *
 * @{
 */

/**
 * Clock sources for system PLLs
 */
typedef enum CHIP_SYSCON_PLLCLKSRC {
	SYSCON_PLLCLKSRC_IRC = 0,		/*!< Internal oscillator */
	SYSCON_PLLCLKSRC_CLKIN,			/*!< External clock input pin */
	SYSCON_PLLCLKSRC_WDTOSC,		/*!< WDT oscillator */
	SYSCON_PLLCLKSRC_RTC,			/*!< RTC 32KHz oscillator */
} CHIP_SYSCON_PLLCLKSRC_T;

/**
 * @brief	Set System PLL clock source
 * @param	src	: Clock source for system PLL
 * @return	Nothing
 * @note	The PLL should be pwoered down prior to changing the source.
 */
__STATIC_INLINE void Chip_Clock_SetSystemPLLSource(CHIP_SYSCON_PLLCLKSRC_T src)
{
	LPC_SYSCON->SYSPLLCLKSEL = (uint32_t) src;
}

/**
 * @brief	Return System PLL input clock rate
 * @return	System PLL input clock rate
 */
uint32_t Chip_Clock_GetSystemPLLInClockRate(void);

/**
 * @brief	Return System PLL output clock rate
 * @param	recompute	: Forces a PLL rate recomputation if true
 * @return	System PLL output clock rate
 * @note	The PLL rate is cached in the driver in a variable as
 * the rate computation function can take some time to perform. It
 * is recommended to use 'false' with the 'recompute' parameter.
 */
uint32_t Chip_Clock_GetSystemPLLOutClockRate(bool recompute);

/**
 * @brief	Enables and disables PLL bypass mode
 * @brief	bypass	: true to bypass PLL (PLL output = PLL input, false to disable bypass
 * @return	System PLL output clock rate
 */
void Chip_Clock_SetBypassPLL(bool bypass);

/**
 * @brief	Check if PLL is locked or not
 * @return	true if the PLL is locked, false if not locked
 */
__STATIC_INLINE bool Chip_Clock_IsSystemPLLLocked(void)
{
	return (bool) ((LPC_SYSCON->SYSPLLSTAT & 1) != 0);
}

/** @brief PLL configuration structure flags for 'flags' field
 * These flags control how the PLL configuration function sets up the PLL setup structure.<br>
 *
 * When the PLL_CONFIGFLAG_USEINRATE flag is selected, the 'InputRate' field in the
 * configuration structure must be assigned with the expected PLL frequency. If the
 * PLL_CONFIGFLAG_USEINRATE is not used, 'InputRate' is ignored in the configuration
 * function and the driver will determine the PLL rate from the currently selected
 * PLL source. This flag might be used to configure the PLL input clock more accurately
 * when using the WDT oscillator or a more dyanmic CLKIN source.<br>
 *
 * When the PLL_CONFIGFLAG_FORCENOFRACT flag is selected, the PLL hardware for the
 * automatic bandwidth selection, Spread Spectrum (SS) support, and fractional M-divider
 * are not used.<br>
 */
#define PLL_CONFIGFLAG_USEINRATE    (1 << 0)	/*!< Flag to use InputRate in PLL configuration structure for setup */
#define PLL_CONFIGFLAG_FORCENOFRACT (1 << 2)	/*!< Force non-fractional output mode, PLL output will not use the fractional, automatic bandwidth, or SS hardware */

/** @brief PLL Spread Spectrum (SS) Programmable modulation frequency
 * See (MF) field in the SYSPLLSSCTRL1 register in the UM.
 */
typedef enum {
	SS_MF_512 = (0 << 20),		/*!< Nss = 512 (fm ˜ 3.9 - 7.8 kHz) */
	SS_MF_384 = (1 << 20),		/*!< Nss ˜= 384 (fm ˜ 5.2 - 10.4 kHz) */
	SS_MF_256 = (2 << 20),		/*!< Nss = 256 (fm ˜ 7.8 - 15.6 kHz) */
	SS_MF_128 = (3 << 20),		/*!< Nss = 128 (fm ˜ 15.6 - 31.3 kHz) */
	SS_MF_64  = (4 << 20),		/*!< Nss = 64 (fm ˜ 32.3 - 64.5 kHz) */
	SS_MF_32  = (5 << 20),		/*!< Nss = 32 (fm ˜ 62.5- 125 kHz) */
	SS_MF_24  = (6 << 20),		/*!< Nss ˜= 24 (fm ˜ 83.3- 166.6 kHz) */
	SS_MF_16  = (7 << 20)		/*!< Nss = 16 (fm ˜ 125- 250 kHz) */
} SS_PROGMODFM_T;

/** @brief PLL Spread Spectrum (SS) Programmable frequency modulation depth
 * See (MR) field in the SYSPLLSSCTRL1 register in the UM.
 */
typedef enum {
	SS_MR_K0   = (0 << 23),		/*!< k = 0 (no spread spectrum) */
	SS_MR_K1   = (1 << 23),		/*!< k = 1 */
	SS_MR_K1_5 = (2 << 23),		/*!< k = 1.5 */
	SS_MR_K2   = (3 << 23),		/*!< k = 2 */
	SS_MR_K3   = (4 << 23),		/*!< k = 3 */
	SS_MR_K4   = (5 << 23),		/*!< k = 4 */
	SS_MR_K6   = (6 << 23),		/*!< k = 6 */
	SS_MR_K8   = (7 << 23)		/*!< k = 8 */
} SS_PROGMODDP_T;

/** @brief PLL Spread Spectrum (SS) Modulation waveform control
 * See (MC) field in the SYSPLLSSCTRL1 register in the UM.<br>
 * Compensation for low pass filtering of the PLL to get a triangular
 * modulation at the output of the PLL, giving a flat frequency spectrum.
 */
typedef enum {
	SS_MC_NOC  = (0 << 26),		/*!< no compensation */
	SS_MC_RECC = (2 << 26),		/*!< recommended setting */
	SS_MC_MAXC = (3 << 26),		/*!< max. compensation */
} SS_MODWVCTRL_T;

/** @brief PLL configuration structure
 * This structure can be used to configure the settings for a PLL
 * setup structure. Fill in the desired configuration for the PLL
 * and call the PLL setup function to fill in a PLL setup structure.
 */
typedef struct {
	uint32_t        desiredRate;	/*!< Desired PLL rate in Hz */
	uint32_t        InputRate;		/*!< PLL input clock in Hz, only used if PLL_CONFIGFLAG_USEINRATE flag is set */
	uint32_t        flags;			/*!< PLL configuration flags, Or'ed value of PLL_CONFIGFLAG_* definitions */
	SS_PROGMODFM_T  ss_mf;			/*!< SS Programmable modulation frequency, only applicable when not using PLL_CONFIGFLAG_FORCENOFRACT flag */
	SS_PROGMODDP_T  ss_mr;			/*!< SS Programmable frequency modulation depth, only applicable when not using PLL_CONFIGFLAG_FORCENOFRACT flag */
	SS_MODWVCTRL_T  ss_mc;			/*!< SS Modulation waveform control, only applicable when not using PLL_CONFIGFLAG_FORCENOFRACT flag */
	bool            mfDither;		/*!< false for fixed modulation frequency or true for dithering, only applicable when not using PLL_CONFIGFLAG_FORCENOFRACT flag */
} PLL_CONFIG_T;

/** @brief PLL setup structure flags for 'flags' field
 * These flags control how the PLL setup function sets up the PLL
 */
#define PLL_SETUPFLAG_POWERUP       (1 << 0)	/*!< Setup will power on the PLL after setup */
#define PLL_SETUPFLAG_WAITLOCK      (1 << 1)	/*!< Setup will wait for PLL lock, implies the PLL will be pwoered on */
#define PLL_SETUPFLAG_ADGVOLT       (1 << 2)	/*!< Optimize system voltage for the new PLL rate */

/** @brief PLL setup structure
 * This structure can be used to pre-build a PLL setup configuration
 * at run-time and quickly set the PLL to the configuration. It can be
 * populated with the PLL setup function. If powering up or waiting
 * for PLL lock, the PLL input clock source should be configured prior
 * to PLL setup.
 */
typedef struct {
	uint32_t    flags;				/*!< PLL setup flags, Or'ed value of PLL_SETUPFLAG_* definitions */
	uint32_t    SYSPLLCTRL;			/*!< PLL control register */
	uint32_t    SYSPLLNDEC;			/*!< PLL NDEC register */
	uint32_t    SYSPLLPDEC;			/*!< PLL PDEC register */
	uint32_t    SYSPLLSSCTRL[2];	/*!< PLL SSCTL registers */
} PLL_SETUP_T;

/** @brief PLL status definitions
 */
typedef enum {
	PLL_ERROR_SUCCESS = 0,			/*!< PLL operation was successful */
	PLL_ERROR_OUTPUT_TOO_LOW,		/*!< PLL output rate request was too low */
	PLL_ERROR_OUTPUT_TOO_HIGH,		/*!< PLL output rate request was too high */
	PLL_ERROR_INPUT_TOO_LOW,		/*!< PLL input rate is too low */
	PLL_ERROR_INPUT_TOO_HIGH,		/*!< PLL input rate is too high */
	PLL_ERROR_OUTSIDE_INTLIMIT		/*!< Requested output rate isn't possible */
} PLL_ERROR_T;

/**
 * @brief	Return System PLL output clock rate from setup structure
 * @param	pSetup	: Pointer to a PLL setup structure
 * @return	System PLL output clock rate the setup structure will generate
 */
uint32_t Chip_Clock_GetSystemPLLOutFromSetup(PLL_SETUP_T *pSetup);

/**
 * @brief	Set PLL output based on the passed PLL setup data
 * @param	pControl	: Pointer to populated PLL control structure to generate setup with
 * @param	pSetup		: Pointer to PLL setup structure to be filled
 * @return	PLL_ERROR_SUCCESS on success, or PLL setup error code
 * @note	Actual frequency for setup may vary from the desired frequency based on the
 * accuracy of input clocks, rounding, non-fractional PLL mode, etc.
 */
PLL_ERROR_T Chip_Clock_SetupPLLData(PLL_CONFIG_T *pControl, PLL_SETUP_T *pSetup);

/**
 * @brief	Set PLL output from PLL setup structure (precise frequency)
 * @param	pSetup	: Pointer to populated PLL setup structure
 * @return	PLL_ERROR_SUCCESS on success, or PLL setup error code
 * @note	This function will power off the PLL, setup the PLL with the
 * new setup data, and then optionally powerup the PLL, wait for PLL lock,
 * and adjust system voltages to the new PLL rate. The function will not
 * alter any source clocks (ie, main systen clock) that may use the PLL,
 * so these should be setup prior to and after exiting the function.
 */
PLL_ERROR_T Chip_Clock_SetupSystemPLLPrec(PLL_SETUP_T *pSetup);

/**
 * @brief	Set PLL output based on the multiplier and input frequency
 * @param	multiply_by	: multiplier
 * @param	input_freq	: Clock input frequency of the PLL
 * @return	Nothing
 * @note	Unlike the Chip_Clock_SetupSystemPLLPrec() function, this
 * function does not disable or enable PLL power, wait for PLL lock,
 * or adjust system voltages. These must be done in the application.
 * The function will not alter any source clocks (ie, main systen clock)
 * that may use the PLL, so these should be setup prior to and after
 * exiting the function.
 */
void Chip_Clock_SetupSystemPLL(uint32_t multiply_by, uint32_t input_freq);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __PLL_5410X_H_ */
