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

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define NVALMAX                     (0x100)
#define PVALMAX                     (0x20)
#define MVALMAX                     (0x8000)

/* SYS PLL related bit fields */
#define SYS_PLL_SELR(d)             (((d) & 0xf) << 0)		/*!< Bandwidth select R value */
#define SYS_PLL_SELI(d)             (((d) & 0x3f) << 4)		/*!< Bandwidth select I value */
#define SYS_PLL_SELP(d)             (((d) & 0x1f) << 10)	/*!< Bandwidth select P value */
#define SYS_PLL_BYPASS              (1 << 15)				/*!< Enable PLL bypass */
#define SYS_PLL_BYPASSCCODIV2       (1 << 16)				/*!< Enable bypass of extra divider by 2 */
#define SYS_PLL_UPLIMOFF            (1 << 17)				/*!< Enable spread spectrum/fractional mode */
#define SYS_PLL_BANDSEL             (1 << 18)				/*!< Enable MDEC control */
#define SYS_PLL_DIRECTI             (1 << 19)				/*!< PLL0 direct input enable */
#define SYS_PLL_DIRECTO             (1 << 20)				/*!< PLL0 direct output enable */

// #define FRAC_BITS_SELI			(8)		// For retaining fractions in divisions
#define PLL_SSCG0_MDEC_VAL_P    (0)			// MDEC is in bits  16 downto 0
#define PLL_SSCG0_MDEC_VAL_M    (0x1FFFFUL << PLL_SSCG0_MDEC_VAL_P)		// NDEC is in bits  9 downto 0
#define PLL_NDEC_VAL_P          (0)			// NDEC is in bits  9:0
#define PLL_NDEC_VAL_M          (0x3FFUL << PLL_NDEC_VAL_P)
#define PLL_PDEC_VAL_P          (0)			// PDEC is in bits 6:0
#define PLL_PDEC_VAL_M          (0x3FFUL << PLL_PDEC_VAL_P)

#define PLL_MIN_CCO_FREQ_MHZ    (75000000)
#define PLL_MAX_CCO_FREQ_MHZ    (150000000)
#define PLL_LOWER_IN_LIMIT      (4000)				/*!< Minimum PLL input rate */
#define PLL_MIN_IN_SSMODE       (2000000)
#define PLL_MAX_IN_SSMODE       (4000000)

// Middle of the range values for spread-spectrum
#define PLL_SSCG_MF_FREQ_VALUE                               4
#define PLL_SSCG_MC_COMP_VALUE                               2
#define PLL_SSCG_MR_DEPTH_VALUE                              4
#define PLL_SSCG_DITHER_VALUE                                0

// pll SYSPLLCTRL Bits
#define SYSCON_SYSPLLCTRL_SELR_P                                0
#define SYSCON_SYSPLLCTRL_SELR_M                                (0xFUL << SYSCON_SYSPLLCTRL_SELR_P)
#define SYSCON_SYSPLLCTRL_SELI_P                                4
#define SYSCON_SYSPLLCTRL_SELI_M                                (0x3FUL << SYSCON_SYSPLLCTRL_SELI_P)
#define SYSCON_SYSPLLCTRL_SELP_P                                10
#define SYSCON_SYSPLLCTRL_SELP_M                                (0x1FUL << SYSCON_SYSPLLCTRL_SELP_P)
#define SYSCON_SYSPLLCTRL_BYPASS_P                          15		// sys_pll150_ctrl
#define SYSCON_SYSPLLCTRL_BYPASS                                (1UL << SYSCON_SYSPLLCTRL_BYPASS_P)
#define SYSCON_SYSPLLCTRL_BYPASS_FBDIV2_P               16
#define SYSCON_SYSPLLCTRL_BYPASS_FBDIV2                 (1UL << SYSCON_SYSPLLCTRL_BYPASS_FBDIV2_P)
#define SYSCON_SYSPLLCTRL_UPLIMOFF_P                        17
#define SYSCON_SYSPLLCTRL_UPLIMOFF                          (1UL << SYSCON_SYSPLLCTRL_UPLIMOFF_P)
#define SYSCON_SYSPLLCTRL_BANDSEL_SSCGREG_N_P       18
#define SYSCON_SYSPLLCTRL_BANDSEL_SSCGREG_N         (1UL << SYSCON_SYSPLLCTRL_BANDSEL_SSCGREG_N_P)
#define SYSCON_SYSPLLCTRL_DIRECTI_P                         19
#define SYSCON_SYSPLLCTRL_DIRECTI                               (1UL << SYSCON_SYSPLLCTRL_DIRECTI_P)
#define SYSCON_SYSPLLCTRL_DIRECTO_P                         20
#define SYSCON_SYSPLLCTRL_DIRECTO                               (1UL << SYSCON_SYSPLLCTRL_DIRECTO_P)

#define SYSCON_SYSPLLSTAT_LOCK_P                                0
#define SYSCON_SYSPLLSTAT_LOCK                              (1UL << SYSCON_SYSPLLSTAT_LOCK_P)

#define PLL_CTRL_BYPASS_P                                                  15		// sys_pll150_ctrl
#define PLL_CTRL_BYPASS_FBDIV2_P                                           16
#define PLL_CTRL_UPLIMOFF_P                                                17
#define PLL_CTRL_BANDSEL_SSCGREG_N_P                                       18
#define PLL_CTRL_DIRECTI_P                                                 19
#define PLL_CTRL_DIRECTO_P                                                 20

#define PLL_CTRL_BYPASS                                                    (1 << PLL_CTRL_BYPASS_P)
#define PLL_CTRL_DIRECTI                                                   (1 << PLL_CTRL_DIRECTI_P)
#define PLL_CTRL_DIRECTO                                                   (1 << PLL_CTRL_DIRECTO_P)
#define PLL_CTRL_UPLIMOFF                                                  (1 << PLL_CTRL_UPLIMOFF_P)
#define PLL_CTRL_BANDSEL_SSCGREG_N                                         (1 << PLL_CTRL_BANDSEL_SSCGREG_N_P)
#define PLL_CTRL_BYPASS_FBDIV2                                             (1 << PLL_CTRL_BYPASS_FBDIV2_P)

// SSCG control[0]
// #define PLL_SSCG0_MDEC_VAL_P                                                0    // MDEC is in bits  16 downto 0
#define PLL_SSCG0_MREQ_P                                                   17
#define PLL_SSCG0_SEL_EXT_SSCG_N_P                                         18
#define PLL_SSCG0_SEL_EXT_SSCG_N                                           (1 << PLL_SSCG0_SEL_EXT_SSCG_N_P)
#define PLL_SSCG0_MREQ                                                     (1 << PLL_SSCG0_MREQ_P)

// SSCG control[1]
#define PLL_SSCG1_MD_REQ_P                                                 19
#define PLL_SSCG1_MOD_PD_SSCGCLK_N_P                                       28
#define PLL_SSCG1_DITHER_P                                                 29
#define PLL_SSCG1_MOD_PD_SSCGCLK_N                                         (1 << PLL_SSCG1_MOD_PD_SSCGCLK_N_P)
#define PLL_SSCG1_DITHER                                                   (1 << PLL_SSCG1_DITHER_P)
#define PLL_SSCG1_MD_REQ                                                   (1 << PLL_SSCG1_MD_REQ_P)

// PLL NDEC reg
#define PLL_NDEC_VAL_SET(value)                     (((unsigned long) (value) << PLL_NDEC_VAL_P) & PLL_NDEC_VAL_M)
#define PLL_NDEC_NREQ_P                                     10
#define PLL_NDEC_NREQ                                           (1 << PLL_NDEC_NREQ_P)

// PLL PDEC reg
#define PLL_PDEC_VAL_SET(value)                     (((unsigned long) (value) << PLL_PDEC_VAL_P) & PLL_PDEC_VAL_M)
#define PLL_PDEC_PREQ_P                                     7
#define PLL_PDEC_PREQ                                           (1 << PLL_PDEC_PREQ_P)

// SSCG control[0]
#define PLL_SSCG0_MDEC_VAL_SET(value)        (((unsigned long) (value) << PLL_SSCG0_MDEC_VAL_P) & PLL_SSCG0_MDEC_VAL_M)
#define PLL_SSCG0_MREQ_P                     17
#define PLL_SSCG0_MREQ                       (1 << PLL_SSCG0_MREQ_P)
#define PLL_SSCG0_SEL_EXT_SSCG_N_P           18
#define PLL_SSCG0_SEL_EXT_SSCG_N             (1 << PLL_SSCG0_SEL_EXT_SSCG_N_P)

// SSCG control[1]
#define PLL_SSCG1_MD_FRACT_P                                        0
#define PLL_SSCG1_MD_INT_P                                          11
#define PLL_SSCG1_MF_P                                              20
#define PLL_SSCG1_MC_P                                              26
#define PLL_SSCG1_MR_P                                              23

#define PLL_SSCG1_MD_FRACT_M                                        (0x7FFUL << PLL_SSCG1_MD_FRACT_P)
#define PLL_SSCG1_MD_INT_M                                          (0xFFUL << PLL_SSCG1_MD_INT_P)
#define PLL_SSCG1_MF_M                                              (0x7UL << PLL_SSCG1_MF_P)
#define PLL_SSCG1_MC_M                                              (0x3UL << PLL_SSCG1_MC_P)
#define PLL_SSCG1_MR_M                                              (0x7UL << PLL_SSCG1_MR_P)

#define PLL_SSCG1_MD_FRACT_SET(value)                               (((unsigned long) (value) << \
																	  PLL_SSCG1_MD_FRACT_P) & PLL_SSCG1_MD_FRACT_M)
#define PLL_SSCG1_MD_INT_SET(value)                                 (((unsigned long) (value) << \
																	  PLL_SSCG1_MD_INT_P)   & PLL_SSCG1_MD_INT_M)

// Middle of the range values for spread-spectrum
#define PLL0_SSCG_MF_FREQ_VALUE     4
#define PLL0_SSCG_MC_COMP_VALUE     2
#define PLL0_SSCG_MR_DEPTH_VALUE    4
#define PLL0_SSCG_DITHER_VALUE      0

#define PLL_MAX_N_DIV       0x100

/* Saved value of PLL output rate, computed whenever needed to save run-time
   computation on each call to retrive the PLL rate. */
static uint32_t curPllRate;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Find encoded NDEC value for raw N value, max N = NVALMAX */
static uint32_t pllEncodeN(uint32_t N)
{
	uint32_t x, i;

	/* Find NDec */
	switch (N) {
	case 0:
		x = 0x3FF;
		break;

	case 1:
		x = 0x302;
		break;

	case 2:
		x = 0x202;
		break;

	default:
		x = 0x080;
		for (i = N; i <= NVALMAX; i++) {
			x = (((x ^ (x >> 2) ^ (x >> 3) ^ (x >> 4)) & 1) << 7) | ((x >> 1) & 0x7F);
		}
		break;
	}

	return x & (PLL_NDEC_VAL_M >> PLL_NDEC_VAL_P);
}

/* Find decoded N value for raw NDEC value */
static uint32_t pllDecodeN(uint32_t NDEC)
{
	uint32_t n, x, i;

	/* Find NDec */
	switch (NDEC) {
	case 0x3FF:
		n = 0;
		break;

	case 0x302:
		n = 1;
		break;

	case 0x202:
		n = 2;
		break;

	default:
		x = 0x080;
		n = 0xFFFFFFFF;
		for (i = NVALMAX; ((i >= 3) && (n == 0xFFFFFFFF)); i--) {
			x = (((x ^ (x >> 2) ^ (x >> 3) ^ (x >> 4)) & 1) << 7) | ((x >> 1) & 0x7F);
			if ((x & (PLL_NDEC_VAL_M >> PLL_NDEC_VAL_P)) == NDEC) {
				/* Decoded value of NDEC */
				n = i;
			}
		}
		break;
	}

	return n;
}

/* Find encoded PDEC value for raw P value, max P = PVALMAX */
static uint32_t pllEncodeP(uint32_t P)
{
	uint32_t x, i;

	/* Find PDec */
	switch (P) {
	case 0:
		x = 0xFF;
		break;

	case 1:
		x = 0x62;
		break;

	case 2:
		x = 0x42;
		break;

	default:
		x = 0x10;
		for (i = P; i <= PVALMAX; i++) {
			x = (((x ^ (x >> 2)) & 1) << 4) | ((x >> 1) & 0xF);
		}
		break;
	}

	return x & (PLL_PDEC_VAL_M >> PLL_PDEC_VAL_P);
}

/* Find decoded P value for raw PDEC value */
static uint32_t pllDecodeP(uint32_t PDEC)
{
	uint32_t p, x, i;

	/* Find PDec */
	switch (PDEC) {
	case 0xFF:
		p = 0;
		break;

	case 0x62:
		p = 1;
		break;

	case 0x42:
		p = 2;
		break;

	default:
		x = 0x10;
		p = 0xFFFFFFFF;
		for (i = PVALMAX; ((i >= 3) && (p == 0xFFFFFFFF)); i--) {
			x = (((x ^ (x >> 2)) & 1) << 4) | ((x >> 1) & 0xF);
			if ((x & (PLL_PDEC_VAL_M >> PLL_PDEC_VAL_P)) == PDEC) {
				/* Decoded value of PDEC */
				p = i;
			}
		}
		break;
	}

	return p;
}

/* Find encoded MDEC value for raw M value, max M = MVALMAX */
static uint32_t pllEncodeM(uint32_t M)
{
	uint32_t i, x;

	/* Find MDec */
	switch (M) {
	case 0:
		x = 0x1FFFF;
		break;

	case 1:
		x = 0x18003;
		break;

	case 2:
		x = 0x10003;
		break;

	case 7:
		x = 0x1F;
		break;

	case 8:
		x = 0x3F;
		break;

	case 10:
		x = 0xFF;
		break;

	case 12:
		x = 0x3FF;
		break;

	default:
		x = 0x04000;
		for (i = M; i <= MVALMAX; i++) {
			x = (((x ^ (x >> 1)) & 1) << 14) | ((x >> 1) & 0x3FFF);
		}
		break;
	}

	return x & (PLL_SSCG0_MDEC_VAL_M >> PLL_SSCG0_MDEC_VAL_P);
}

/* Find decoded M value for raw MDEC value */
static uint32_t pllDecodeM(uint32_t MDEC)
{
	uint32_t m, i, x;

	/* Find MDec */
	switch (MDEC) {
	case 0x1FFFF:
		m = 0;
		break;

	case 0x18003:
		m = 1;
		break;

	case 0x10003:
		m = 2;
		break;

	case 0x1F:
		m = 7;
		break;

	case 0x3F:
		m = 8;
		break;

	case 0xFF:
		m = 10;
		break;

	case 0x3FF:
		m = 12;
		break;

	default:
		x = 0x04000;
		m = 0xFFFFFFFF;
		for (i = MVALMAX; ((i >= 3) && (m == 0xFFFFFFFF)); i--) {
			x = (((x ^ (x >> 1)) & 1) << 14) | ((x >> 1) & 0x3FFF);
			if ((x & (PLL_SSCG0_MDEC_VAL_M >> PLL_SSCG0_MDEC_VAL_P)) == MDEC) {
				/* Decoded value of MDEC */
				m = i;
			}
		}
		break;
	}

	return m;
}

/* Find SELP, SELI, and SELR values for raw M value, max M = MVALMAX */
static void pllFindSel(uint32_t M, bool bypassFBDIV2, uint32_t *pSelP, uint32_t *pSelI, uint32_t *pSelR)
{
	/* bandwidth: compute selP from Multiplier */
	if (M < 60) {
		*pSelP = (M >> 1) + 1;
	}
	else {
		*pSelP = PVALMAX - 1;
	}

	/* bandwidth: compute selI from Multiplier */
	if (M > 16384) {
		*pSelI = 1;
	}
	else if (M > 8192) {
		*pSelI = 2;
	}
	else if (M > 2048) {
		*pSelI = 4;
	}
	else if (M >= 501) {
		*pSelI = 8;
	}
	else if (M >= 60) {
		*pSelI = 4 * (1024 / (M + 9));
	}
	else {
		*pSelI = (M & 0x3C) + 4;
	}

	if (*pSelI > (SYSCON_SYSPLLCTRL_SELI_M >> SYSCON_SYSPLLCTRL_SELI_P)) {
		*pSelI = (SYSCON_SYSPLLCTRL_SELI_M >> SYSCON_SYSPLLCTRL_SELI_P);
	}

	*pSelR = 0;
}

/* Get predivider (N) from PLL NDEC setting */
uint32_t findPllPreDiv(uint32_t ctrlReg, uint32_t nDecReg)
{
	uint32_t preDiv = 1;

	/* Direct input is not used? */
	if ((ctrlReg & SYSCON_SYSPLLCTRL_DIRECTI) == 0) {
		/* Decode NDEC value to get (N) pre divider */
		preDiv = pllDecodeN(nDecReg & 0x3FF);
		if (preDiv == 0) {
			preDiv = 1;
		}
	}

	/* Adjusted by 1, directi is used to bypass */
	return preDiv;
}

/* Get postdivider (P) from PLL PDEC setting */
uint32_t findPllPostDiv(uint32_t ctrlReg, uint32_t pDecReg)
{
	uint32_t postDiv = 1;

	/* Direct input is not used? */
	if ((ctrlReg & SYS_PLL_DIRECTO) == 0) {
		/* Decode PDEC value to get (P) post divider */
		postDiv = 2 * pllDecodeP(pDecReg & 0x7F);
		if (postDiv == 0) {
			postDiv = 2;
		}
	}

	/* Adjusted by 1, directo is used to bypass */
	return postDiv;
}

/* Get multiplier (M) from PLL MDEC and BYPASS_FBDIV2 settings */
uint32_t findPllMMult(uint32_t ctrlReg, uint32_t mDecReg)
{
	uint32_t mMult = 1;

	/* Decode MDEC value to get (M) multiplier */
	mMult = pllDecodeM(mDecReg & 0x1FFFF);

	/* Extra multiply by 2 needed? */
	if ((ctrlReg & SYSCON_SYSPLLCTRL_BYPASS_FBDIV2) == 0) {
		mMult = mMult << 1;
	}

	if (mMult == 0) {
		mMult = 1;
	}

	return mMult;
}

static uint32_t FindGreatestCommonDivisor(uint32_t m, uint32_t n)
{
	uint32_t tmp;

	while (n != 0) {
		tmp = n;
		n = m % n;
		m = tmp;
	}

	return m;
}

/* Set PLL output based on desired output rate */
static PLL_ERROR_T Chip_Clock_GetPllConfig(uint32_t finHz, uint32_t foutHz, PLL_SETUP_T *pSetup,
										   bool useFeedbackDiv2, bool useSS)
{
	uint32_t nDivOutHz, fccoHz, multFccoDiv;
	uint32_t pllPreDivider, pllMultiplier, pllBypassFBDIV2, pllPostDivider;
	uint32_t pllDirectInput, pllDirectOutput;
	uint32_t pllSelP, pllSelI, pllSelR, bandsel, uplimoff;

	/* Baseline parameters (no input or output dividers) */
	pllPreDivider = 1;	/* 1 implies pre-divider will be disabled */
	pllPostDivider = 0;	/* 0 implies post-divider will be disabled */
	pllDirectOutput = 1;
	if (useFeedbackDiv2) {
		/* Using feedback divider for M, so disable bypass */
		pllBypassFBDIV2 = 0;
	}
	else {
		pllBypassFBDIV2 = 1;
	}
	multFccoDiv = (2 - pllBypassFBDIV2);

	/* Verify output rate parameter */
	if (foutHz > PLL_MAX_CCO_FREQ_MHZ) {
		/* Maximum PLL output with post divider=1 cannot go above this frequency */
		return PLL_ERROR_OUTPUT_TOO_HIGH;
	}
	if (foutHz < (PLL_MIN_CCO_FREQ_MHZ / (PVALMAX << 1))) {
		/* Minmum PLL output with maximum post divider cannot go below this frequency */
		return PLL_ERROR_OUTPUT_TOO_LOW;
	}

	/* If using SS mode, input clock needs to be between 2MHz and 4MHz */
	if (useSS) {
		/* Verify input rate parameter */
		if (finHz < PLL_MIN_IN_SSMODE) {
			/* Input clock into the PLL cannot be lower than this */
			return PLL_ERROR_INPUT_TOO_LOW;
		}

		/* PLL input in SS mode must be under 4MHz */
		pllPreDivider = finHz / ((PLL_MIN_IN_SSMODE + PLL_MAX_IN_SSMODE) / 2);
		if (pllPreDivider > NVALMAX) {
			return PLL_ERROR_INPUT_TOO_HIGH;
		}
	}
	else {
		/* Verify input rate parameter */
		if (finHz < PLL_LOWER_IN_LIMIT) {
			/* Input clock into the PLL cannot be lower than this */
			return PLL_ERROR_INPUT_TOO_LOW;
		}
	}

	/* Find the optimal CCO frequency for the output and input that
	   will keep it inside the PLL CCO range. This may require
	   tweaking the post-divider for the PLL. */
	fccoHz = foutHz;
	while (fccoHz < PLL_MIN_CCO_FREQ_MHZ) {
		/* CCO output is less than minimum CCO range, so the CCO output
		   needs to be bumped up and the post-divider is used to bring
		   the PLL output back down. */
		pllPostDivider++;
		if (pllPostDivider > PVALMAX) {
			return PLL_ERROR_OUTSIDE_INTLIMIT;
		}

		/* Target CCO goes up, PLL output goes down */
		fccoHz = foutHz * (pllPostDivider * 2);
		pllDirectOutput = 0;
	}

	/* Determine if a pre-divider is needed to get the best frequency */
	if ((finHz > PLL_LOWER_IN_LIMIT) && (fccoHz >= finHz) && (useSS == false)) {
		uint32_t a = FindGreatestCommonDivisor(fccoHz, (multFccoDiv * finHz));

		if (a > 20000) {
			a = (multFccoDiv * finHz) / a;
			if ((a != 0) && (a < PLL_MAX_N_DIV)) {
				pllPreDivider = a;
			}
		}
	}

	/* Bypass pre-divider hardware if pre-divider is 1 */
	if (pllPreDivider > 1) {
		pllDirectInput = 0;
	}
	else {
		pllDirectInput = 1;
	}

	/* Determine PLL multipler */
	nDivOutHz = (finHz / pllPreDivider);
	pllMultiplier = (fccoHz / nDivOutHz) / multFccoDiv;

	/* Find optimal values for filter */
	if (useSS == false) {
		/* Will bumping up M by 1 get us closer to the desired CCO frequency? */
		if ((nDivOutHz * ((multFccoDiv * pllMultiplier * 2) + 1)) < (fccoHz * 2)) {
			pllMultiplier++;
		}

		/* Setup filtering */
		pllFindSel(pllMultiplier, pllBypassFBDIV2, &pllSelP, &pllSelI, &pllSelR);
		bandsel = 1;
		uplimoff = 0;

		/* Get encoded value for M (mult) and use manual filter, disable SS mode */
		pSetup->SYSPLLSSCTRL[0] = (PLL_SSCG0_MDEC_VAL_SET(pllEncodeM(pllMultiplier)) |
								   (1 << PLL_SSCG0_SEL_EXT_SSCG_N_P));

		/* Power down SSC, not used */
		pSetup->SYSPLLSSCTRL[1] = PLL_SSCG1_MOD_PD_SSCGCLK_N;
	}
	else {
		uint64_t fc;

		/* Filtering will be handled by SSC */
		pllSelR = pllSelI = pllSelP = 0;
		bandsel = 0;
		uplimoff = 1;

		/* The PLL multiplier will get very close and slightly under the
		   desired target frequency. A small fractional component can be
		   added to fine tune the frequency upwards to the target. */
		fc = ((uint64_t) (fccoHz % (multFccoDiv * nDivOutHz)) << 11) / (multFccoDiv * nDivOutHz);

		/* MDEC set by SSC */
		pSetup->SYSPLLSSCTRL[0] = 0;

		/* Set multiplier */
		pSetup->SYSPLLSSCTRL[1] = PLL_SSCG1_MD_INT_SET(pllMultiplier) |
								  PLL_SSCG1_MD_FRACT_SET((uint32_t) fc);
	}

	/* Get encoded values for N (prediv) and P (postdiv) */
	pSetup->SYSPLLNDEC = PLL_NDEC_VAL_SET(pllEncodeN(pllPreDivider));
	pSetup->SYSPLLPDEC = PLL_PDEC_VAL_SET(pllEncodeP(pllPostDivider));

	/* PLL control */
	pSetup->SYSPLLCTRL =
		(pllSelR << SYSCON_SYSPLLCTRL_SELR_P) |					/* Filter coefficient */
		(pllSelI << SYSCON_SYSPLLCTRL_SELI_P) |					/* Filter coefficient */
		(pllSelP << SYSCON_SYSPLLCTRL_SELP_P) |					/* Filter coefficient */
		(0 << SYSCON_SYSPLLCTRL_BYPASS_P) |						/* PLL bypass mode disabled */
		(pllBypassFBDIV2 << SYSCON_SYSPLLCTRL_BYPASS_FBDIV2_P) |	/* Extra M / 2 divider? */
		(uplimoff << SYSCON_SYSPLLCTRL_UPLIMOFF_P) |			/* SS/fractional mode disabled */
		(bandsel << SYSCON_SYSPLLCTRL_BANDSEL_SSCGREG_N_P) |	/* Manual bandwidth selection enabled */
		(pllDirectInput << SYSCON_SYSPLLCTRL_DIRECTI_P) |		/* Bypass pre-divider? */
		(pllDirectOutput << SYSCON_SYSPLLCTRL_DIRECTO_P);		/* Bypass post-divider? */

	return PLL_ERROR_SUCCESS;
}

/* Update local PLL rate variable */
static void Chip_Clock_GetSystemPLLOutFromSetupUpdate(PLL_SETUP_T *pSetup)
{
	curPllRate = Chip_Clock_GetSystemPLLOutFromSetup(pSetup);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Return System PLL input clock rate */
uint32_t Chip_Clock_GetSystemPLLInClockRate(void)
{
	uint32_t clkRate = 0;

	switch ((CHIP_SYSCON_PLLCLKSRC_T) (LPC_SYSCON->SYSPLLCLKSEL & 0x3)) {
	case SYSCON_PLLCLKSRC_IRC:
		clkRate = Chip_Clock_GetIntOscRate();
		break;

	case SYSCON_PLLCLKSRC_CLKIN:
		clkRate = Chip_Clock_GetExtClockInRate();
		break;

	case SYSCON_PLLCLKSRC_WDTOSC:
		clkRate = Chip_Clock_GetWDTOSCRate();
		break;

	case SYSCON_PLLCLKSRC_RTC:
		clkRate = Chip_Clock_GetRTCOscRate();
		break;
	}

	return clkRate;
}

/* Return System PLL output clock rate from setup structure */
uint32_t Chip_Clock_GetSystemPLLOutFromSetup(PLL_SETUP_T *pSetup)
{
	uint32_t prediv, postdiv, mMult, inPllRate;
	uint64_t workRate;

	inPllRate = Chip_Clock_GetSystemPLLInClockRate();
	if ((pSetup->SYSPLLCTRL & SYSCON_SYSPLLCTRL_BYPASS) == 0) {
		/* PLL is not in bypass mode, get pre-divider, post-divider, and M divider */
		prediv = findPllPreDiv(pSetup->SYSPLLCTRL, pSetup->SYSPLLNDEC);
		postdiv = findPllPostDiv(pSetup->SYSPLLCTRL, pSetup->SYSPLLPDEC);

		/* Adjust input clock */
		inPllRate = inPllRate / prediv;

		/* If using the SS, use the multiplier */
		if (pSetup->SYSPLLSSCTRL[1] & PLL_SSCG1_MOD_PD_SSCGCLK_N) {
			/* MDEC used for rate */
			mMult = findPllMMult(pSetup->SYSPLLCTRL, pSetup->SYSPLLSSCTRL[0]);
			workRate = (uint64_t) inPllRate * (uint64_t) mMult;
		}
		else {
			uint64_t fract;

			/* SS multipler used for rate */
			mMult = (pSetup->SYSPLLSSCTRL[1] & PLL_SSCG1_MD_INT_M) >> PLL_SSCG1_MD_INT_P;
			workRate = (uint64_t) inPllRate * (uint64_t) mMult;

			/* Adjust by fractional */
			fract = (uint64_t) (pSetup->SYSPLLSSCTRL[1] & PLL_SSCG1_MD_FRACT_M) >> PLL_SSCG1_MD_FRACT_P;
			workRate = workRate + ((inPllRate * fract) / 0x800);
		}

		workRate = workRate / ((uint64_t) postdiv);
	}
	else {
		/* In bypass mode */
		workRate = (uint64_t) inPllRate;
	}

	return (uint32_t) workRate;
}

/* Return System PLL output clock rate */
uint32_t Chip_Clock_GetSystemPLLOutClockRate(bool recompute)
{
	PLL_SETUP_T Setup;
	uint32_t rate;

	if ((recompute == true) || (curPllRate == 0)) {
		Setup.SYSPLLCTRL = LPC_SYSCON->SYSPLLCTRL;
		Setup.SYSPLLNDEC = LPC_SYSCON->SYSPLLNDEC;
		Setup.SYSPLLPDEC = LPC_SYSCON->SYSPLLPDEC;
		Setup.SYSPLLSSCTRL[0] = LPC_SYSCON->SYSPLLSSCTRL[0];
		Setup.SYSPLLSSCTRL[1] = LPC_SYSCON->SYSPLLSSCTRL[1];

		Chip_Clock_GetSystemPLLOutFromSetupUpdate(&Setup);
	}

	rate = curPllRate;

	return rate;
}

/* Enables and disables PLL bypass mode */
void Chip_Clock_SetBypassPLL(bool bypass)
{
	if (bypass) {
		LPC_SYSCON->SYSPLLCTRL |= SYSCON_SYSPLLCTRL_BYPASS;
	}
	else {
		LPC_SYSCON->SYSPLLCTRL &= ~SYSCON_SYSPLLCTRL_BYPASS;
	}
}

/* Set PLL output based on the passed PLL setup data */
PLL_ERROR_T Chip_Clock_SetupPLLData(PLL_CONFIG_T *pControl, PLL_SETUP_T *pSetup)
{
	uint32_t inRate;
	bool useSS = (bool) ((pControl->flags & PLL_CONFIGFLAG_FORCENOFRACT) == 0);
	PLL_ERROR_T pllError;

	/* Determine input rate for the PLL */
	if ((pControl->flags & PLL_CONFIGFLAG_USEINRATE) != 0) {
		inRate = pControl->InputRate;
	}
	else {
		inRate = Chip_Clock_GetSystemPLLInClockRate();
	}

	/* PLL flag options */
	pllError = Chip_Clock_GetPllConfig(inRate, pControl->desiredRate, pSetup, false, useSS);
	if ((useSS) && (pllError == PLL_ERROR_SUCCESS)) {
		/* If using SS mode, then some tweaks are made to the generated setup */
		pSetup->SYSPLLSSCTRL[1] |= (uint32_t) pControl->ss_mf | (uint32_t) pControl->ss_mr |
								   (uint32_t) pControl->ss_mc;
		if (pControl->mfDither) {
			pSetup->SYSPLLSSCTRL[1] |= PLL_SSCG1_DITHER;
		}
	}

	return pllError;
}

/* Set PLL output from PLL setup structure */
PLL_ERROR_T Chip_Clock_SetupSystemPLLPrec(PLL_SETUP_T *pSetup)
{
	/* Power off PLL during setup changes */
	Chip_SYSCON_PowerDown(SYSCON_PDRUNCFG_PD_SYS_PLL);

	/* Write PLL setup data */
	LPC_SYSCON->SYSPLLCTRL = pSetup->SYSPLLCTRL;
	LPC_SYSCON->SYSPLLNDEC = pSetup->SYSPLLNDEC;
	LPC_SYSCON->SYSPLLNDEC = pSetup->SYSPLLNDEC | PLL_NDEC_NREQ;/* latch */
	LPC_SYSCON->SYSPLLPDEC = pSetup->SYSPLLPDEC;
	LPC_SYSCON->SYSPLLPDEC = pSetup->SYSPLLPDEC | PLL_PDEC_PREQ;/* latch */
	LPC_SYSCON->SYSPLLSSCTRL[0] = pSetup->SYSPLLSSCTRL[0];
	LPC_SYSCON->SYSPLLSSCTRL[0] = pSetup->SYSPLLSSCTRL[0] | PLL_SSCG0_MREQ;	/* latch */
	LPC_SYSCON->SYSPLLSSCTRL[1] = pSetup->SYSPLLSSCTRL[1];
	LPC_SYSCON->SYSPLLSSCTRL[1] = pSetup->SYSPLLSSCTRL[1] | PLL_SSCG1_MD_REQ;	/* latch */

	/* Flags for lock or power on */
	if ((pSetup->flags & (PLL_SETUPFLAG_POWERUP | PLL_SETUPFLAG_WAITLOCK)) != 0) {
		Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_SYS_PLL);
	}
	
	if ((pSetup->flags & PLL_SETUPFLAG_WAITLOCK) != 0) {
		while (Chip_Clock_IsSystemPLLLocked() == false) {}
	}

	/* Update current programmed PLL rate var */
	Chip_Clock_GetSystemPLLOutFromSetupUpdate(pSetup);

	/* System voltage adjustment, occurs prior to setting main system clock */
	if ((pSetup->flags & PLL_SETUPFLAG_ADGVOLT) != 0) {
		Chip_POWER_SetVoltage(POWER_LOW_POWER_MODE, curPllRate);
	}
	curPllRate = 0;

	return PLL_ERROR_SUCCESS;
}

/*
    Set System PLL clock based on the input frequency and multiplier

    Here is the configuration used by this function:
    - input divider -- set to 2
    - output -- direct (p-divider is not used)
    - direct in = off
    - direct out = on

    There is a subtle doubling of the "multiply_by" factor.
    There is a feedback clock that is not disabled.
    Because it halves the divider, it doubles the frequency.
    This is used to compensate for the input divider value.

 */

void Chip_Clock_SetupSystemPLL(uint32_t multiply_by, uint32_t input_freq)
{
	uint32_t cco_freq = input_freq * multiply_by;
	uint32_t pdec = 1;
	uint32_t selr;
	uint32_t seli;
	uint32_t selp;
	uint32_t mdec, ndec;

	uint32_t directo = SYS_PLL_DIRECTO;

	while (cco_freq < 75000000) {
		multiply_by <<= 1;	/* double value in each iteration */
		pdec <<= 1;			/* correspondingly double pdec to cancel effect of double msel */
		cco_freq = input_freq * multiply_by;
	}
	selr = 0;
    if(multiply_by < 60) {
        seli = (multiply_by & 0x3c) + 4;
        selp = (multiply_by >> 1) + 1;
    }
    else {
        selp = 31;
        if (multiply_by > 16384) {
            seli = 1;
        }
        else if(multiply_by > 8192) {
            seli = 2;
        }
        else if (multiply_by > 2048) {
            seli = 4;
        }
        else if (multiply_by >= 501) {
            seli = 8;
        }
        else {
            seli = 4*(1024/(multiply_by+9));
        }
    }

	if (pdec > 1) {
		directo = 0;	/* use post divider */
		pdec = pdec / 2;	/* Account for minus 1 encoding */
		/* Translate P value */
		pdec = (pdec == 1)  ? 0x62 :	/* 1  * 2 */
			   (pdec == 2)  ? 0x42 :	/* 2  * 2 */
			   (pdec == 4)  ? 0x02 :	/* 4  * 2 */
			   (pdec == 8)  ? 0x0b :	/* 8  * 2 */
			   (pdec == 16) ? 0x11 :	/* 16 * 2 */
			   (pdec == 32) ? 0x08 : 0x08;	/* 32 * 2 */
	}

    mdec = PLL_SSCG0_MDEC_VAL_SET(pllEncodeM(multiply_by));
	ndec = 0x302;	/* pre divide by 1 (hardcoded) */

	LPC_SYSCON->SYSPLLCTRL = SYS_PLL_BANDSEL | directo | SYS_PLL_BYPASSCCODIV2 | (selr << SYSCON_SYSPLLCTRL_SELR_P) |
							 (seli << SYSCON_SYSPLLCTRL_SELI_P) | (selp << SYSCON_SYSPLLCTRL_SELP_P);

	LPC_SYSCON->SYSPLLPDEC = pdec;									/* set PDEC value */
	LPC_SYSCON->SYSPLLPDEC = pdec | (1 << 7);						/* Assert PDEC reload request: load register into PLL */

	LPC_SYSCON->SYSPLLNDEC = ndec;									/* set NDEC value */
	LPC_SYSCON->SYSPLLNDEC = ndec | (1 << 10);						/* Assert NDEC reload request: load register into PLL */

	LPC_SYSCON->SYSPLLSSCTRL[0] = (1 << 18) | mdec;					/* set MDEC value*/
	LPC_SYSCON->SYSPLLSSCTRL[0] = (1 << 18) | (1 << 17) | mdec;		/* Load register into the PLL */

	curPllRate = 0;
}
