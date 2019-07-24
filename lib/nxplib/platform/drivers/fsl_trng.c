/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_trng.h"

#if defined(FSL_FEATURE_SOC_TRNG_COUNT) && FSL_FEATURE_SOC_TRNG_COUNT

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.trng"
#endif

/* Default values for user configuration structure.*/
#if (defined(KW40Z4_SERIES) || defined(KW41Z4_SERIES) || defined(KW31Z4_SERIES) || defined(KW21Z4_SERIES) || \
     defined(MCIMX7U5_M4_SERIES) || defined(KW36Z4_SERIES) || defined(KW37A4_SERIES) || defined(KW37Z4_SERIES) ||\
     defined(KW38A4_SERIES) || defined(KW38Z4_SERIES) || defined(KW39A4_SERIES))
#define TRNG_USER_CONFIG_DEFAULT_OSC_DIV kTRNG_RingOscDiv8
#elif (defined(KV56F24_SERIES) || defined(KV58F24_SERIES) || defined(KL28Z7_SERIES) || defined(KL81Z7_SERIES) || \
       defined(KL82Z7_SERIES))
#define TRNG_USER_CONFIG_DEFAULT_OSC_DIV kTRNG_RingOscDiv4
#elif defined(K81F25615_SERIES)
#define TRNG_USER_CONFIG_DEFAULT_OSC_DIV kTRNG_RingOscDiv2
#else
#define TRNG_USER_CONFIG_DEFAULT_OSC_DIV kTRNG_RingOscDiv0
#endif

#define TRNG_USER_CONFIG_DEFAULT_LOCK 0
#define TRNG_USER_CONFIG_DEFAULT_ENTROPY_DELAY 3200
#define TRNG_USER_CONFIG_DEFAULT_SAMPLE_SIZE 2500
#define TRNG_USER_CONFIG_DEFAULT_SPARSE_BIT_LIMIT 63
#define TRNG_USER_CONFIG_DEFAULT_RETRY_COUNT 1
#define TRNG_USER_CONFIG_DEFAULT_RUN_MAX_LIMIT 34

#define TRNG_USER_CONFIG_DEFAULT_MONOBIT_MAXIMUM 1384
#define TRNG_USER_CONFIG_DEFAULT_MONOBIT_MINIMUM (TRNG_USER_CONFIG_DEFAULT_MONOBIT_MAXIMUM - 268)
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT1_MAXIMUM 405
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT1_MINIMUM (TRNG_USER_CONFIG_DEFAULT_RUNBIT1_MAXIMUM - 178)
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT2_MAXIMUM 220
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT2_MINIMUM (TRNG_USER_CONFIG_DEFAULT_RUNBIT2_MAXIMUM - 122)
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT3_MAXIMUM 125
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT3_MINIMUM (TRNG_USER_CONFIG_DEFAULT_RUNBIT3_MAXIMUM - 88)
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT4_MAXIMUM 75
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT4_MINIMUM (TRNG_USER_CONFIG_DEFAULT_RUNBIT4_MAXIMUM - 64)
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT5_MAXIMUM 47
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT5_MINIMUM (TRNG_USER_CONFIG_DEFAULT_RUNBIT5_MAXIMUM - 46)
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT6PLUS_MAXIMUM 47
#define TRNG_USER_CONFIG_DEFAULT_RUNBIT6PLUS_MINIMUM (TRNG_USER_CONFIG_DEFAULT_RUNBIT6PLUS_MAXIMUM - 46)
#define TRNG_USER_CONFIG_DEFAULT_POKER_MAXIMUM 26912
#define TRNG_USER_CONFIG_DEFAULT_POKER_MINIMUM (TRNG_USER_CONFIG_DEFAULT_POKER_MAXIMUM - 2467)
#define TRNG_USER_CONFIG_DEFAULT_FREQUENCY_MAXIMUM 25600
#define TRNG_USER_CONFIG_DEFAULT_FREQUENCY_MINIMUM 1600

/*! @brief TRNG work mode */
typedef enum _trng_work_mode
{
    kTRNG_WorkModeRun     = 0U, /*!< Run Mode. */
    kTRNG_WorkModeProgram = 1U  /*!< Program Mode. */
} trng_work_mode_t;

/*! @brief TRNG statistical check type*/
typedef enum _trng_statistical_check
{
    kTRNG_StatisticalCheckMonobit =
        1U,                        /*!< Statistical check of number of ones/zero detected during entropy generation. */
    kTRNG_StatisticalCheckRunBit1, /*!< Statistical check of number of runs of length 1 detected during entropy
                                      generation. */
    kTRNG_StatisticalCheckRunBit2, /*!< Statistical check of number of runs of length 2 detected during entropy
                                      generation. */
    kTRNG_StatisticalCheckRunBit3, /*!< Statistical check of number of runs of length 3 detected during entropy
                                      generation. */
    kTRNG_StatisticalCheckRunBit4, /*!< Statistical check of number of runs of length 4 detected during entropy
                                      generation. */
    kTRNG_StatisticalCheckRunBit5, /*!< Statistical check of number of runs of length 5 detected during entropy
                                      generation. */
    kTRNG_StatisticalCheckRunBit6Plus,   /*!< Statistical check of number of runs of length 6 or more detected during
                                            entropy generation. */
    kTRNG_StatisticalCheckPoker,         /*!< Statistical check of "Poker Test". */
    kTRNG_StatisticalCheckFrequencyCount /*!< Statistical check of entropy sample frequency count. */
} trng_statistical_check_t;

/*******************************************************************************
 * TRNG_SCMISC - RNG Statistical Check Miscellaneous Register
 ******************************************************************************/
/*!
 * @name Register TRNG_SCMISC, field RTY_CT[19:16] (RW)
 *
 * RETRY COUNT. If a statistical check fails during the TRNG Entropy Generation,
 * the RTY_CT value indicates the number of times a retry should occur before
 * generating an error. This field is writable only if MCTL[PRGM] bit is 1. This
 * field will read zeroes if MCTL[PRGM] = 0. This field is cleared to 1h by writing
 * the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCMISC_RTY_CT field. */
#define TRNG_RD_SCMISC_RTY_CT(base) ((TRNG_SCMISC_REG(base) & TRNG_SCMISC_RTY_CT_MASK) >> TRNG_SCMISC_RTY_CT_SHIFT)

/*! @brief Set the RTY_CT field to a new value. */
#define TRNG_WR_SCMISC_RTY_CT(base, value) (TRNG_RMW_SCMISC(base, TRNG_SCMISC_RTY_CT_MASK, TRNG_SCMISC_RTY_CT(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SCML - RNG Statistical Check Monobit Limit Register
 ******************************************************************************/
/*!
 * @brief TRNG_SCML - RNG Statistical Check Monobit Limit Register (RW)
 *
 * Reset value: 0x010C0568U
 *
 * The RNG Statistical Check Monobit Limit Register defines the allowable
 * maximum and minimum number of ones/zero detected during entropy generation. To pass
 * the test, the number of ones/zeroes generated must be less than the programmed
 * maximum value, and the number of ones/zeroes generated must be greater than
 * (maximum - range). If this test fails, the Retry Counter in SCMISC will be
 * decremented, and a retry will occur if the Retry Count has not reached zero. If
 * the Retry Count has reached zero, an error will be generated. Note that this
 * offset (0xBASE_0620) is used as SCML only if MCTL[PRGM] is 1. If MCTL[PRGM] is 0,
 * this offset is used as SCMC readback register.
 */
/*!
 * @name Constants and macros for entire TRNG_SCML register
 */
/*@{*/
#define TRNG_SCML_REG(base) ((base)->SCML)
#define TRNG_RD_SCML(base) (TRNG_SCML_REG(base))
#define TRNG_WR_SCML(base, value) (TRNG_SCML_REG(base) = (value))
#define TRNG_RMW_SCML(base, mask, value) (TRNG_WR_SCML(base, (TRNG_RD_SCML(base) & ~(mask)) | (value)))
/*@}*/
/*!
 * @name Register TRNG_SCML, field MONO_MAX[15:0] (RW)
 *
 * Monobit Maximum Limit. Defines the maximum allowable count taken during
 * entropy generation. The number of ones/zeroes detected during entropy generation
 * must be less than MONO_MAX, else a retry or error will occur. This register is
 * cleared to 00056Bh (decimal 1387) by writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCML_MONO_MAX field. */
#define TRNG_RD_SCML_MONO_MAX(base) ((TRNG_SCML_REG(base) & TRNG_SCML_MONO_MAX_MASK) >> TRNG_SCML_MONO_MAX_SHIFT)

/*! @brief Set the MONO_MAX field to a new value. */
#define TRNG_WR_SCML_MONO_MAX(base, value) (TRNG_RMW_SCML(base, TRNG_SCML_MONO_MAX_MASK, TRNG_SCML_MONO_MAX(value)))
/*@}*/
/*!
 * @name Register TRNG_SCML, field MONO_RNG[31:16] (RW)
 *
 * Monobit Range. The number of ones/zeroes detected during entropy generation
 * must be greater than MONO_MAX - MONO_RNG, else a retry or error will occur.
 * This register is cleared to 000112h (decimal 274) by writing the MCTL[RST_DEF]
 * bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCML_MONO_RNG field. */
#define TRNG_RD_SCML_MONO_RNG(base) ((TRNG_SCML_REG(base) & TRNG_SCML_MONO_RNG_MASK) >> TRNG_SCML_MONO_RNG_SHIFT)

/*! @brief Set the MONO_RNG field to a new value. */
#define TRNG_WR_SCML_MONO_RNG(base, value) (TRNG_RMW_SCML(base, TRNG_SCML_MONO_RNG_MASK, TRNG_SCML_MONO_RNG(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SCR1L - RNG Statistical Check Run Length 1 Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_SCR1L - RNG Statistical Check Run Length 1 Limit Register (RW)
 *
 * Reset value: 0x00B20195U
 *
 * The RNG Statistical Check Run Length 1 Limit Register defines the allowable
 * maximum and minimum number of runs of length 1 detected during entropy
 * generation. To pass the test, the number of runs of length 1 (for samples of both 0
 * and 1) must be less than the programmed maximum value, and the number of runs of
 * length 1 must be greater than (maximum - range). If this test fails, the
 * Retry Counter in SCMISC will be decremented, and a retry will occur if the Retry
 * Count has not reached zero. If the Retry Count has reached zero, an error will
 * be generated. Note that this address (0xBASE_0624) is used as SCR1L only if
 * MCTL[PRGM] is 1. If MCTL[PRGM] is 0, this address is used as SCR1C readback
 * register.
 */
/*!
 * @name Constants and macros for entire TRNG_SCR1L register
 */
/*@{*/
#define TRNG_SCR1L_REG(base) ((base)->SCR1L)
#define TRNG_RD_SCR1L(base) (TRNG_SCR1L_REG(base))
#define TRNG_WR_SCR1L(base, value) (TRNG_SCR1L_REG(base) = (value))
#define TRNG_RMW_SCR1L(base, mask, value) (TRNG_WR_SCR1L(base, (TRNG_RD_SCR1L(base) & ~(mask)) | (value)))
/*@}*/

/*!
 * @name Register TRNG_SCR1L, field RUN1_MAX[14:0] (RW)
 *
 * Run Length 1 Maximum Limit. Defines the maximum allowable runs of length 1
 * (for both 0 and 1) detected during entropy generation. The number of runs of
 * length 1 detected during entropy generation must be less than RUN1_MAX, else a
 * retry or error will occur. This register is cleared to 01E5h (decimal 485) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR1L_RUN1_MAX field. */
#define TRNG_RD_SCR1L_RUN1_MAX(base) ((TRNG_SCR1L_REG(base) & TRNG_SCR1L_RUN1_MAX_MASK) >> TRNG_SCR1L_RUN1_MAX_SHIFT)

/*! @brief Set the RUN1_MAX field to a new value. */
#define TRNG_WR_SCR1L_RUN1_MAX(base, value) (TRNG_RMW_SCR1L(base, TRNG_SCR1L_RUN1_MAX_MASK, TRNG_SCR1L_RUN1_MAX(value)))
/*@}*/

/*!
 * @name Register TRNG_SCR1L, field RUN1_RNG[30:16] (RW)
 *
 * Run Length 1 Range. The number of runs of length 1 (for both 0 and 1)
 * detected during entropy generation must be greater than RUN1_MAX - RUN1_RNG, else a
 * retry or error will occur. This register is cleared to 0102h (decimal 258) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR1L_RUN1_RNG field. */
#define TRNG_RD_SCR1L_RUN1_RNG(base) ((TRNG_SCR1L_REG(base) & TRNG_SCR1L_RUN1_RNG_MASK) >> TRNG_SCR1L_RUN1_RNG_SHIFT)

/*! @brief Set the RUN1_RNG field to a new value. */
#define TRNG_WR_SCR1L_RUN1_RNG(base, value) (TRNG_RMW_SCR1L(base, TRNG_SCR1L_RUN1_RNG_MASK, TRNG_SCR1L_RUN1_RNG(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SCR2L - RNG Statistical Check Run Length 2 Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_SCR2L - RNG Statistical Check Run Length 2 Limit Register (RW)
 *
 * Reset value: 0x007A00DCU
 *
 * The RNG Statistical Check Run Length 2 Limit Register defines the allowable
 * maximum and minimum number of runs of length 2 detected during entropy
 * generation. To pass the test, the number of runs of length 2 (for samples of both 0
 * and 1) must be less than the programmed maximum value, and the number of runs of
 * length 2 must be greater than (maximum - range). If this test fails, the
 * Retry Counter in SCMISC will be decremented, and a retry will occur if the Retry
 * Count has not reached zero. If the Retry Count has reached zero, an error will
 * be generated. Note that this address (0xBASE_0628) is used as SCR2L only if
 * MCTL[PRGM] is 1. If MCTL[PRGM] is 0, this address is used as SCR2C readback
 * register.
 */
/*!
 * @name Constants and macros for entire TRNG_SCR2L register
 */
/*@{*/
#define TRNG_SCR2L_REG(base) ((base)->SCR2L)
#define TRNG_RD_SCR2L(base) (TRNG_SCR2L_REG(base))
#define TRNG_WR_SCR2L(base, value) (TRNG_SCR2L_REG(base) = (value))
#define TRNG_RMW_SCR2L(base, mask, value) (TRNG_WR_SCR2L(base, (TRNG_RD_SCR2L(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_SCR2L bitfields
 */

/*!
 * @name Register TRNG_SCR2L, field RUN2_MAX[13:0] (RW)
 *
 * Run Length 2 Maximum Limit. Defines the maximum allowable runs of length 2
 * (for both 0 and 1) detected during entropy generation. The number of runs of
 * length 2 detected during entropy generation must be less than RUN2_MAX, else a
 * retry or error will occur. This register is cleared to 00DCh (decimal 220) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR2L_RUN2_MAX field. */
#define TRNG_RD_SCR2L_RUN2_MAX(base) ((TRNG_SCR2L_REG(base) & TRNG_SCR2L_RUN2_MAX_MASK) >> TRNG_SCR2L_RUN2_MAX_SHIFT)

/*! @brief Set the RUN2_MAX field to a new value. */
#define TRNG_WR_SCR2L_RUN2_MAX(base, value) (TRNG_RMW_SCR2L(base, TRNG_SCR2L_RUN2_MAX_MASK, TRNG_SCR2L_RUN2_MAX(value)))
/*@}*/

/*!
 * @name Register TRNG_SCR2L, field RUN2_RNG[29:16] (RW)
 *
 * Run Length 2 Range. The number of runs of length 2 (for both 0 and 1)
 * detected during entropy generation must be greater than RUN2_MAX - RUN2_RNG, else a
 * retry or error will occur. This register is cleared to 007Ah (decimal 122) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR2L_RUN2_RNG field. */
#define TRNG_RD_SCR2L_RUN2_RNG(base) ((TRNG_SCR2L_REG(base) & TRNG_SCR2L_RUN2_RNG_MASK) >> TRNG_SCR2L_RUN2_RNG_SHIFT)

/*! @brief Set the RUN2_RNG field to a new value. */
#define TRNG_WR_SCR2L_RUN2_RNG(base, value) (TRNG_RMW_SCR2L(base, TRNG_SCR2L_RUN2_RNG_MASK, TRNG_SCR2L_RUN2_RNG(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SCR3L - RNG Statistical Check Run Length 3 Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_SCR3L - RNG Statistical Check Run Length 3 Limit Register (RW)
 *
 * Reset value: 0x0058007DU
 *
 * The RNG Statistical Check Run Length 3 Limit Register defines the allowable
 * maximum and minimum number of runs of length 3 detected during entropy
 * generation. To pass the test, the number of runs of length 3 (for samples of both 0
 * and 1) must be less than the programmed maximum value, and the number of runs of
 * length 3 must be greater than (maximum - range). If this test fails, the
 * Retry Counter in SCMISC will be decremented, and a retry will occur if the Retry
 * Count has not reached zero. If the Retry Count has reached zero, an error will
 * be generated. Note that this address (0xBASE_062C) is used as SCR3L only if
 * MCTL[PRGM] is 1. If MCTL[PRGM] is 0, this address is used as SCR3C readback
 * register.
 */
/*!
 * @name Constants and macros for entire TRNG_SCR3L register
 */
/*@{*/
#define TRNG_SCR3L_REG(base) ((base)->SCR3L)
#define TRNG_RD_SCR3L(base) (TRNG_SCR3L_REG(base))
#define TRNG_WR_SCR3L(base, value) (TRNG_SCR3L_REG(base) = (value))
#define TRNG_RMW_SCR3L(base, mask, value) (TRNG_WR_SCR3L(base, (TRNG_RD_SCR3L(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_SCR3L bitfields
 */

/*!
 * @name Register TRNG_SCR3L, field RUN3_MAX[12:0] (RW)
 *
 * Run Length 3 Maximum Limit. Defines the maximum allowable runs of length 3
 * (for both 0 and 1) detected during entropy generation. The number of runs of
 * length 3 detected during entropy generation must be less than RUN3_MAX, else a
 * retry or error will occur. This register is cleared to 007Dh (decimal 125) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR3L_RUN3_MAX field. */
#define TRNG_RD_SCR3L_RUN3_MAX(base) ((TRNG_SCR3L_REG(base) & TRNG_SCR3L_RUN3_MAX_MASK) >> TRNG_SCR3L_RUN3_MAX_SHIFT)

/*! @brief Set the RUN3_MAX field to a new value. */
#define TRNG_WR_SCR3L_RUN3_MAX(base, value) (TRNG_RMW_SCR3L(base, TRNG_SCR3L_RUN3_MAX_MASK, TRNG_SCR3L_RUN3_MAX(value)))
/*@}*/

/*!
 * @name Register TRNG_SCR3L, field RUN3_RNG[28:16] (RW)
 *
 * Run Length 3 Range. The number of runs of length 3 (for both 0 and 1)
 * detected during entropy generation must be greater than RUN3_MAX - RUN3_RNG, else a
 * retry or error will occur. This register is cleared to 0058h (decimal 88) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR3L_RUN3_RNG field. */
#define TRNG_RD_SCR3L_RUN3_RNG(base) ((TRNG_SCR3L_REG(base) & TRNG_SCR3L_RUN3_RNG_MASK) >> TRNG_SCR3L_RUN3_RNG_SHIFT)

/*! @brief Set the RUN3_RNG field to a new value. */
#define TRNG_WR_SCR3L_RUN3_RNG(base, value) (TRNG_RMW_SCR3L(base, TRNG_SCR3L_RUN3_RNG_MASK, TRNG_SCR3L_RUN3_RNG(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SCR4L - RNG Statistical Check Run Length 4 Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_SCR4L - RNG Statistical Check Run Length 4 Limit Register (RW)
 *
 * Reset value: 0x0040004BU
 *
 * The RNG Statistical Check Run Length 4 Limit Register defines the allowable
 * maximum and minimum number of runs of length 4 detected during entropy
 * generation. To pass the test, the number of runs of length 4 (for samples of both 0
 * and 1) must be less than the programmed maximum value, and the number of runs of
 * length 4 must be greater than (maximum - range). If this test fails, the
 * Retry Counter in SCMISC will be decremented, and a retry will occur if the Retry
 * Count has not reached zero. If the Retry Count has reached zero, an error will
 * be generated. Note that this address (0xBASE_0630) is used as SCR4L only if
 * MCTL[PRGM] is 1. If MCTL[PRGM] is 0, this address is used as SCR4C readback
 * register.
 */
/*!
 * @name Constants and macros for entire TRNG_SCR4L register
 */
/*@{*/
#define TRNG_SCR4L_REG(base) ((base)->SCR4L)
#define TRNG_RD_SCR4L(base) (TRNG_SCR4L_REG(base))
#define TRNG_WR_SCR4L(base, value) (TRNG_SCR4L_REG(base) = (value))
#define TRNG_RMW_SCR4L(base, mask, value) (TRNG_WR_SCR4L(base, (TRNG_RD_SCR4L(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_SCR4L bitfields
 */

/*!
 * @name Register TRNG_SCR4L, field RUN4_MAX[11:0] (RW)
 *
 * Run Length 4 Maximum Limit. Defines the maximum allowable runs of length 4
 * (for both 0 and 1) detected during entropy generation. The number of runs of
 * length 4 detected during entropy generation must be less than RUN4_MAX, else a
 * retry or error will occur. This register is cleared to 004Bh (decimal 75) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR4L_RUN4_MAX field. */
#define TRNG_RD_SCR4L_RUN4_MAX(base) ((TRNG_SCR4L_REG(base) & TRNG_SCR4L_RUN4_MAX_MASK) >> TRNG_SCR4L_RUN4_MAX_SHIFT)

/*! @brief Set the RUN4_MAX field to a new value. */
#define TRNG_WR_SCR4L_RUN4_MAX(base, value) (TRNG_RMW_SCR4L(base, TRNG_SCR4L_RUN4_MAX_MASK, TRNG_SCR4L_RUN4_MAX(value)))
/*@}*/

/*!
 * @name Register TRNG_SCR4L, field RUN4_RNG[27:16] (RW)
 *
 * Run Length 4 Range. The number of runs of length 4 (for both 0 and 1)
 * detected during entropy generation must be greater than RUN4_MAX - RUN4_RNG, else a
 * retry or error will occur. This register is cleared to 0040h (decimal 64) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR4L_RUN4_RNG field. */
#define TRNG_RD_SCR4L_RUN4_RNG(base) ((TRNG_SCR4L_REG(base) & TRNG_SCR4L_RUN4_RNG_MASK) >> TRNG_SCR4L_RUN4_RNG_SHIFT)

/*! @brief Set the RUN4_RNG field to a new value. */
#define TRNG_WR_SCR4L_RUN4_RNG(base, value) (TRNG_RMW_SCR4L(base, TRNG_SCR4L_RUN4_RNG_MASK, TRNG_SCR4L_RUN4_RNG(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SCR5L - RNG Statistical Check Run Length 5 Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_SCR5L - RNG Statistical Check Run Length 5 Limit Register (RW)
 *
 * Reset value: 0x002E002FU
 *
 * The RNG Statistical Check Run Length 5 Limit Register defines the allowable
 * maximum and minimum number of runs of length 5 detected during entropy
 * generation. To pass the test, the number of runs of length 5 (for samples of both 0
 * and 1) must be less than the programmed maximum value, and the number of runs of
 * length 5 must be greater than (maximum - range). If this test fails, the
 * Retry Counter in SCMISC will be decremented, and a retry will occur if the Retry
 * Count has not reached zero. If the Retry Count has reached zero, an error will
 * be generated. Note that this address (0xBASE_0634) is used as SCR5L only if
 * MCTL[PRGM] is 1. If MCTL[PRGM] is 0, this address is used as SCR5C readback
 * register.
 */
/*!
 * @name Constants and macros for entire TRNG_SCR5L register
 */
/*@{*/
#define TRNG_SCR5L_REG(base) ((base)->SCR5L)
#define TRNG_RD_SCR5L(base) (TRNG_SCR5L_REG(base))
#define TRNG_WR_SCR5L(base, value) (TRNG_SCR5L_REG(base) = (value))
#define TRNG_RMW_SCR5L(base, mask, value) (TRNG_WR_SCR5L(base, (TRNG_RD_SCR5L(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_SCR5L bitfields
 */

/*!
 * @name Register TRNG_SCR5L, field RUN5_MAX[10:0] (RW)
 *
 * Run Length 5 Maximum Limit. Defines the maximum allowable runs of length 5
 * (for both 0 and 1) detected during entropy generation. The number of runs of
 * length 5 detected during entropy generation must be less than RUN5_MAX, else a
 * retry or error will occur. This register is cleared to 002Fh (decimal 47) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR5L_RUN5_MAX field. */
#define TRNG_RD_SCR5L_RUN5_MAX(base) ((TRNG_SCR5L_REG(base) & TRNG_SCR5L_RUN5_MAX_MASK) >> TRNG_SCR5L_RUN5_MAX_SHIFT)

/*! @brief Set the RUN5_MAX field to a new value. */
#define TRNG_WR_SCR5L_RUN5_MAX(base, value) (TRNG_RMW_SCR5L(base, TRNG_SCR5L_RUN5_MAX_MASK, TRNG_SCR5L_RUN5_MAX(value)))
/*@}*/

/*!
 * @name Register TRNG_SCR5L, field RUN5_RNG[26:16] (RW)
 *
 * Run Length 5 Range. The number of runs of length 5 (for both 0 and 1)
 * detected during entropy generation must be greater than RUN5_MAX - RUN5_RNG, else a
 * retry or error will occur. This register is cleared to 002Eh (decimal 46) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR5L_RUN5_RNG field. */
#define TRNG_RD_SCR5L_RUN5_RNG(base) ((TRNG_SCR5L_REG(base) & TRNG_SCR5L_RUN5_RNG_MASK) >> TRNG_SCR5L_RUN5_RNG_SHIFT)

/*! @brief Set the RUN5_RNG field to a new value. */
#define TRNG_WR_SCR5L_RUN5_RNG(base, value) (TRNG_RMW_SCR5L(base, TRNG_SCR5L_RUN5_RNG_MASK, TRNG_SCR5L_RUN5_RNG(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SCR6PL - RNG Statistical Check Run Length 6+ Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_SCR6PL - RNG Statistical Check Run Length 6+ Limit Register (RW)
 *
 * Reset value: 0x002E002FU
 *
 * The RNG Statistical Check Run Length 6+ Limit Register defines the allowable
 * maximum and minimum number of runs of length 6 or more detected during entropy
 * generation. To pass the test, the number of runs of length 6 or more (for
 * samples of both 0 and 1) must be less than the programmed maximum value, and the
 * number of runs of length 6 or more must be greater than (maximum - range). If
 * this test fails, the Retry Counter in SCMISC will be decremented, and a retry
 * will occur if the Retry Count has not reached zero. If the Retry Count has
 * reached zero, an error will be generated. Note that this offset (0xBASE_0638) is
 * used as SCR6PL only if MCTL[PRGM] is 1. If MCTL[PRGM] is 0, this offset is
 * used as SCR6PC readback register.
 */
/*!
 * @name Constants and macros for entire TRNG_SCR6PL register
 */
/*@{*/
#define TRNG_SCR6PL_REG(base) ((base)->SCR6PL)
#define TRNG_RD_SCR6PL(base) (TRNG_SCR6PL_REG(base))
#define TRNG_WR_SCR6PL(base, value) (TRNG_SCR6PL_REG(base) = (value))
#define TRNG_RMW_SCR6PL(base, mask, value) (TRNG_WR_SCR6PL(base, (TRNG_RD_SCR6PL(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_SCR6PL bitfields
 */

/*!
 * @name Register TRNG_SCR6PL, field RUN6P_MAX[10:0] (RW)
 *
 * Run Length 6+ Maximum Limit. Defines the maximum allowable runs of length 6
 * or more (for both 0 and 1) detected during entropy generation. The number of
 * runs of length 6 or more detected during entropy generation must be less than
 * RUN6P_MAX, else a retry or error will occur. This register is cleared to 002Fh
 * (decimal 47) by writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR6PL_RUN6P_MAX field. */
#define TRNG_RD_SCR6PL_RUN6P_MAX(base) \
    ((TRNG_SCR6PL_REG(base) & TRNG_SCR6PL_RUN6P_MAX_MASK) >> TRNG_SCR6PL_RUN6P_MAX_SHIFT)

/*! @brief Set the RUN6P_MAX field to a new value. */
#define TRNG_WR_SCR6PL_RUN6P_MAX(base, value) \
    (TRNG_RMW_SCR6PL(base, TRNG_SCR6PL_RUN6P_MAX_MASK, TRNG_SCR6PL_RUN6P_MAX(value)))
/*@}*/

/*!
 * @name Register TRNG_SCR6PL, field RUN6P_RNG[26:16] (RW)
 *
 * Run Length 6+ Range. The number of runs of length 6 or more (for both 0 and
 * 1) detected during entropy generation must be greater than RUN6P_MAX -
 * RUN6P_RNG, else a retry or error will occur. This register is cleared to 002Eh
 * (decimal 46) by writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCR6PL_RUN6P_RNG field. */
#define TRNG_RD_SCR6PL_RUN6P_RNG(base) \
    ((TRNG_SCR6PL_REG(base) & TRNG_SCR6PL_RUN6P_RNG_MASK) >> TRNG_SCR6PL_RUN6P_RNG_SHIFT)

/*! @brief Set the RUN6P_RNG field to a new value. */
#define TRNG_WR_SCR6PL_RUN6P_RNG(base, value) \
    (TRNG_RMW_SCR6PL(base, TRNG_SCR6PL_RUN6P_RNG_MASK, TRNG_SCR6PL_RUN6P_RNG(value)))
/*@}*/

/*******************************************************************************
 * TRNG_PKRMAX - RNG Poker Maximum Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_PKRMAX - RNG Poker Maximum Limit Register (RW)
 *
 * Reset value: 0x00006920U
 *
 * The RNG Poker Maximum Limit Register defines Maximum Limit allowable during
 * the TRNG Statistical Check Poker Test. Note that this offset (0xBASE_060C) is
 * used as PKRMAX only if MCTL[PRGM] is 1. If MCTL[PRGM] is 0, this offset is used
 * as the PKRSQ readback register.
 */
/*!
 * @name Constants and macros for entire TRNG_PKRMAX register
 */
/*@{*/
#define TRNG_PKRMAX_REG(base) ((base)->PKRMAX)
#define TRNG_RD_PKRMAX(base) (TRNG_PKRMAX_REG(base))
#define TRNG_WR_PKRMAX(base, value) (TRNG_PKRMAX_REG(base) = (value))
#define TRNG_RMW_PKRMAX(base, mask, value) (TRNG_WR_PKRMAX(base, (TRNG_RD_PKRMAX(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_PKRMAX bitfields
 */

/*!
 * @name Register TRNG_PKRMAX, field PKR_MAX[23:0] (RW)
 *
 * Poker Maximum Limit. During the TRNG Statistical Checks, a "Poker Test" is
 * run which requires a maximum and minimum limit. The maximum allowable result is
 * programmed in the PKRMAX[PKR_MAX] register. This field is writable only if
 * MCTL[PRGM] bit is 1. This register is cleared to 006920h (decimal 26912) by
 * writing the MCTL[RST_DEF] bit to 1. Note that the PKRMAX and PKRRNG registers
 * combined are used to define the minimum allowable Poker result, which is PKR_MAX -
 * PKR_RNG + 1. Note that if MCTL[PRGM] bit is 0, this register address is used
 * to read the Poker Test Square Calculation result in register PKRSQ, as defined
 * in the following section.
 */
/*@{*/
/*! @brief Read current value of the TRNG_PKRMAX_PKR_MAX field. */
#define TRNG_RD_PKRMAX_PKR_MAX(base) ((TRNG_PKRMAX_REG(base) & TRNG_PKRMAX_PKR_MAX_MASK) >> TRNG_PKRMAX_PKR_MAX_SHIFT)

/*! @brief Set the PKR_MAX field to a new value. */
#define TRNG_WR_PKRMAX_PKR_MAX(base, value) \
    (TRNG_RMW_PKRMAX(base, TRNG_PKRMAX_PKR_MAX_MASK, TRNG_PKRMAX_PKR_MAX(value)))
/*@}*/

/*******************************************************************************
 * TRNG_PKRRNG - RNG Poker Range Register
 ******************************************************************************/

/*!
 * @brief TRNG_PKRRNG - RNG Poker Range Register (RW)
 *
 * Reset value: 0x000009A3U
 *
 * The RNG Poker Range Register defines the difference between the TRNG Poker
 * Maximum Limit and the minimum limit. These limits are used during the TRNG
 * Statistical Check Poker Test.
 */
/*!
 * @name Constants and macros for entire TRNG_PKRRNG register
 */
/*@{*/
#define TRNG_PKRRNG_REG(base) ((base)->PKRRNG)
#define TRNG_RD_PKRRNG(base) (TRNG_PKRRNG_REG(base))
#define TRNG_WR_PKRRNG(base, value) (TRNG_PKRRNG_REG(base) = (value))
#define TRNG_RMW_PKRRNG(base, mask, value) (TRNG_WR_PKRRNG(base, (TRNG_RD_PKRRNG(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_PKRRNG bitfields
 */

/*!
 * @name Register TRNG_PKRRNG, field PKR_RNG[15:0] (RW)
 *
 * Poker Range. During the TRNG Statistical Checks, a "Poker Test" is run which
 * requires a maximum and minimum limit. The maximum is programmed in the
 * RTPKRMAX[PKR_MAX] register, and the minimum is derived by subtracting the PKR_RNG
 * value from the programmed maximum value. This field is writable only if
 * MCTL[PRGM] bit is 1. This field will read zeroes if MCTL[PRGM] = 0. This field is
 * cleared to 09A3h (decimal 2467) by writing the MCTL[RST_DEF] bit to 1. Note that
 * the minimum allowable Poker result is PKR_MAX - PKR_RNG + 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_PKRRNG_PKR_RNG field. */
#define TRNG_RD_PKRRNG_PKR_RNG(base) ((TRNG_PKRRNG_REG(base) & TRNG_PKRRNG_PKR_RNG_MASK) >> TRNG_PKRRNG_PKR_RNG_SHIFT)

/*! @brief Set the PKR_RNG field to a new value. */
#define TRNG_WR_PKRRNG_PKR_RNG(base, value) \
    (TRNG_RMW_PKRRNG(base, TRNG_PKRRNG_PKR_RNG_MASK, TRNG_PKRRNG_PKR_RNG(value)))
/*@}*/

/*******************************************************************************
 * TRNG_FRQMAX - RNG Frequency Count Maximum Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_FRQMAX - RNG Frequency Count Maximum Limit Register (RW)
 *
 * Reset value: 0x00006400U
 *
 * The RNG Frequency Count Maximum Limit Register defines the maximum allowable
 * count taken by the Entropy sample counter during each Entropy sample. During
 * any sample period, if the count is greater than this programmed maximum, a
 * Frequency Count Fail is flagged in MCTL[FCT_FAIL] and an error is generated. Note
 * that this address (061C) is used as FRQMAX only if MCTL[PRGM] is 1. If
 * MCTL[PRGM] is 0, this address is used as FRQCNT readback register.
 */
/*!
 * @name Constants and macros for entire TRNG_FRQMAX register
 */
/*@{*/
#define TRNG_FRQMAX_REG(base) ((base)->FRQMAX)
#define TRNG_RD_FRQMAX(base) (TRNG_FRQMAX_REG(base))
#define TRNG_WR_FRQMAX(base, value) (TRNG_FRQMAX_REG(base) = (value))
#define TRNG_RMW_FRQMAX(base, mask, value) (TRNG_WR_FRQMAX(base, (TRNG_RD_FRQMAX(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_FRQMAX bitfields
 */

/*!
 * @name Register TRNG_FRQMAX, field FRQ_MAX[21:0] (RW)
 *
 * Frequency Counter Maximum Limit. Defines the maximum allowable count taken
 * during each entropy sample. This field is writable only if MCTL[PRGM] bit is 1.
 * This register is cleared to 000640h by writing the MCTL[RST_DEF] bit to 1.
 * Note that if MCTL[PRGM] bit is 0, this register address is used to read the
 * Frequency Count result in register FRQCNT, as defined in the following section.
 */
/*@{*/
/*! @brief Read current value of the TRNG_FRQMAX_FRQ_MAX field. */
#define TRNG_RD_FRQMAX_FRQ_MAX(base) ((TRNG_FRQMAX_REG(base) & TRNG_FRQMAX_FRQ_MAX_MASK) >> TRNG_FRQMAX_FRQ_MAX_SHIFT)

/*! @brief Set the FRQ_MAX field to a new value. */
#define TRNG_WR_FRQMAX_FRQ_MAX(base, value) \
    (TRNG_RMW_FRQMAX(base, TRNG_FRQMAX_FRQ_MAX_MASK, TRNG_FRQMAX_FRQ_MAX(value)))
/*@}*/

/*******************************************************************************
 * TRNG_FRQMIN - RNG Frequency Count Minimum Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_FRQMIN - RNG Frequency Count Minimum Limit Register (RW)
 *
 * Reset value: 0x00000640U
 *
 * The RNG Frequency Count Minimum Limit Register defines the minimum allowable
 * count taken by the Entropy sample counter during each Entropy sample. During
 * any sample period, if the count is less than this programmed minimum, a
 * Frequency Count Fail is flagged in MCTL[FCT_FAIL] and an error is generated.
 */
/*!
 * @name Constants and macros for entire TRNG_FRQMIN register
 */
/*@{*/
#define TRNG_FRQMIN_REG(base) ((base)->FRQMIN)
#define TRNG_RD_FRQMIN(base) (TRNG_FRQMIN_REG(base))
#define TRNG_WR_FRQMIN(base, value) (TRNG_FRQMIN_REG(base) = (value))
#define TRNG_RMW_FRQMIN(base, mask, value) (TRNG_WR_FRQMIN(base, (TRNG_RD_FRQMIN(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_FRQMIN bitfields
 */

/*!
 * @name Register TRNG_FRQMIN, field FRQ_MIN[21:0] (RW)
 *
 * Frequency Count Minimum Limit. Defines the minimum allowable count taken
 * during each entropy sample. This field is writable only if MCTL[PRGM] bit is 1.
 * This field will read zeroes if MCTL[PRGM] = 0. This field is cleared to 0000h64
 * by writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_FRQMIN_FRQ_MIN field. */
#define TRNG_RD_FRQMIN_FRQ_MIN(base) ((TRNG_FRQMIN_REG(base) & TRNG_FRQMIN_FRQ_MIN_MASK) >> TRNG_FRQMIN_FRQ_MIN_SHIFT)

/*! @brief Set the FRQ_MIN field to a new value. */
#define TRNG_WR_FRQMIN_FRQ_MIN(base, value) \
    (TRNG_RMW_FRQMIN(base, TRNG_FRQMIN_FRQ_MIN_MASK, TRNG_FRQMIN_FRQ_MIN(value)))
/*@}*/

/*******************************************************************************
 * TRNG_MCTL - RNG Miscellaneous Control Register
 ******************************************************************************/

/*!
 * @brief TRNG_MCTL - RNG Miscellaneous Control Register (RW)
 *
 * Reset value: 0x00012001U
 *
 * This register is intended to be used for programming, configuring and testing
 * the RNG. It is the main register to read/write, in order to enable Entropy
 * generation, to stop entropy generation and to block access to entropy registers.
 * This is done via the special TRNG_ACC and PRGM bits below. The RNG
 * Miscellaneous Control Register is a read/write register used to control the RNG's True
 * Random Number Generator (TRNG) access, operation and test. Note that in many
 * cases two RNG registers share the same address, and a particular register at the
 * shared address is selected based upon the value in the PRGM field of the MCTL
 * register.
 */
/*!
 * @name Constants and macros for entire TRNG_MCTL register
 */
/*@{*/
#define TRNG_MCTL_REG(base) ((base)->MCTL)
#define TRNG_RD_MCTL(base) (TRNG_MCTL_REG(base))
#define TRNG_WR_MCTL(base, value) (TRNG_MCTL_REG(base) = (value))
#define TRNG_RMW_MCTL(base, mask, value) (TRNG_WR_MCTL(base, (TRNG_RD_MCTL(base) & ~(mask)) | (value)))
/*@}*/

/*!
 * @name Register TRNG_MCTL, field FOR_SCLK[7] (RW)
 *
 * Force System Clock. If set, the system clock is used to operate the TRNG,
 * instead of the ring oscillator. This is for test use only, and indeterminate
 * results may occur. This bit is writable only if PRGM bit is 1, or PRGM bit is
 * being written to 1 simultaneously to writing this bit. This bit is cleared by
 * writing the RST_DEF bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_MCTL_FOR_SCLK field. */
#define TRNG_RD_MCTL_FOR_SCLK(base) ((TRNG_MCTL_REG(base) & TRNG_MCTL_FOR_SCLK_MASK) >> TRNG_MCTL_FOR_SCLK_SHIFT)

/*! @brief Set the FOR_SCLK field to a new value. */
#define TRNG_WR_MCTL_FOR_SCLK(base, value) \
    (TRNG_RMW_MCTL(base, (TRNG_MCTL_FOR_SCLK_MASK | TRNG_MCTL_ERR_MASK), TRNG_MCTL_FOR_SCLK(value)))
/*@}*/

/*!
 * @name Register TRNG_MCTL, field OSC_DIV[3:2] (RW)
 *
 * Oscillator Divide. Determines the amount of dividing done to the ring
 * oscillator before it is used by the TRNG.This field is writable only if PRGM bit is
 * 1, or PRGM bit is being written to 1 simultaneously to writing this field. This
 * field is cleared to 00 by writing the RST_DEF bit to 1.
 *
 * Values:
 * - 0b00 - use ring oscillator with no divide
 * - 0b01 - use ring oscillator divided-by-2
 * - 0b10 - use ring oscillator divided-by-4
 * - 0b11 - use ring oscillator divided-by-8
 */
/*@{*/
/*! @brief Read current value of the TRNG_MCTL_OSC_DIV field. */
#define TRNG_RD_MCTL_OSC_DIV(base) ((TRNG_MCTL_REG(base) & TRNG_MCTL_OSC_DIV_MASK) >> TRNG_MCTL_OSC_DIV_SHIFT)

/*! @brief Set the OSC_DIV field to a new value. */
#define TRNG_WR_MCTL_OSC_DIV(base, value) \
    (TRNG_RMW_MCTL(base, (TRNG_MCTL_OSC_DIV_MASK | TRNG_MCTL_ERR_MASK), TRNG_MCTL_OSC_DIV(value)))
/*@}*/

/*!
 * @name Register TRNG_MCTL, field SAMP_MODE[1:0] (RW)
 *
 * Sample Mode. Determines the method of sampling the ring oscillator while
 * generating the Entropy value:This field is writable only if PRGM bit is 1, or PRGM
 * bit is being written to 1 simultaneously with writing this field. This field
 * is cleared to 01 by writing the RST_DEF bit to 1.
 *
 * Values:
 * - 0b00 - use Von Neumann data into both Entropy shifter and Statistical
 *     Checker
 * - 0b01 - use raw data into both Entropy shifter and Statistical Checker
 * - 0b10 - use Von Neumann data into Entropy shifter. Use raw data into
 *     Statistical Checker
 * - 0b11 - reserved.
 */
/*@{*/
/*! @brief Read current value of the TRNG_MCTL_SAMP_MODE field. */
#define TRNG_RD_MCTL_SAMP_MODE(base) ((TRNG_MCTL_REG(base) & TRNG_MCTL_SAMP_MODE_MASK) >> TRNG_MCTL_SAMP_MODE_SHIFT)

/*! @brief Set the SAMP_MODE field to a new value. */
#define TRNG_WR_MCTL_SAMP_MODE(base, value) \
    (TRNG_RMW_MCTL(base, (TRNG_MCTL_SAMP_MODE_MASK | TRNG_MCTL_ERR_MASK), TRNG_MCTL_SAMP_MODE(value)))
/*@}*/

/*!
 * @name Register TRNG_MCTL, field PRGM[16] (RW)
 *
 * Programming Mode Select. When this bit is 1, the TRNG is in Program Mode,
 * otherwise it is in Run Mode. No Entropy value will be generated while the TRNG is
 * in Program Mode. Note that different RNG registers are accessible at the same
 * address depending on whether PRGM is set to 1 or 0. This is noted in the RNG
 * register descriptions.
 */
/*@{*/
/*! @brief Read current value of the TRNG_MCTL_PRGM field. */
#define TRNG_RD_MCTL_PRGM(base) ((TRNG_MCTL_REG(base) & TRNG_MCTL_PRGM_MASK) >> TRNG_MCTL_PRGM_SHIFT)

/*! @brief Set the PRGM field to a new value. */
#define TRNG_WR_MCTL_PRGM(base, value) \
    (TRNG_RMW_MCTL(base, (TRNG_MCTL_PRGM_MASK | TRNG_MCTL_ERR_MASK), TRNG_MCTL_PRGM(value)))
/*@}*/

/*!
 * @name Register TRNG_MCTL, field RST_DEF[6] (WO)
 *
 * Reset Defaults. Writing a 1 to this bit clears various TRNG registers, and
 * bits within registers, to their default state. This bit is writable only if PRGM
 * bit is 1, or PRGM bit is being written to 1 simultaneously to writing this
 * bit. Reading this bit always produces a 0.
 */
/*@{*/
/*! @brief Set the RST_DEF field to a new value. */
#define TRNG_WR_MCTL_RST_DEF(base, value) \
    (TRNG_RMW_MCTL(base, (TRNG_MCTL_RST_DEF_MASK | TRNG_MCTL_ERR_MASK), TRNG_MCTL_RST_DEF(value)))
/*@}*/

#if !(defined(FSL_FEATURE_TRNG_HAS_NO_TRNG_ACC) && (FSL_FEATURE_TRNG_HAS_NO_TRNG_ACC > 0))
/*!
 * @name Register TRNG_MCTL, field TRNG_ACC[5] (RW)
 *
 * TRNG Access Mode. If this bit is set to 1, the TRNG will generate an Entropy
 * value that can be read via the ENT0-ENT15 registers. The Entropy value may be
 * read once the ENT VAL bit is asserted. Also see ENTa register descriptions
 * (For a = 0 to 15).
 */
/*@{*/
/*! @brief Read current value of the TRNG_MCTL_TRNG_ACC field. */
#define TRNG_RD_MCTL_TRNG_ACC(base) ((TRNG_MCTL_REG(base) & TRNG_MCTL_TRNG_ACC_MASK) >> TRNG_MCTL_TRNG_ACC_SHIFT)

/*! @brief Set the TRNG_ACC field to a new value. */
#define TRNG_WR_MCTL_TRNG_ACC(base, value) \
    (TRNG_RMW_MCTL(base, (TRNG_MCTL_TRNG_ACC_MASK | TRNG_MCTL_ERR_MASK), TRNG_MCTL_TRNG_ACC(value)))
/*@}*/
#endif

/*!
 * @name Register TRNG_MCTL, field TSTOP_OK[13] (RO)
 *
 * TRNG_OK_TO_STOP. Software should check that this bit is a 1 before
 * transitioning RNG to low power mode (RNG clock stopped). RNG turns on the TRNG
 * free-running ring oscillator whenever new entropy is being generated and turns off the
 * ring oscillator when entropy generation is complete. If the RNG clock is
 * stopped while the TRNG ring oscillator is running, the oscillator will continue
 * running even though the RNG clock is stopped. TSTOP_OK is asserted when the TRNG
 * ring oscillator is not running. and therefore it is ok to stop the RNG clock.
 */
/*@{*/
/*! @brief Read current value of the TRNG_MCTL_TSTOP_OK field. */
#define TRNG_RD_MCTL_TSTOP_OK(base) ((TRNG_MCTL_REG(base) & TRNG_MCTL_TSTOP_OK_MASK) >> TRNG_MCTL_TSTOP_OK_SHIFT)
/*@}*/

/*!
 * @name Register TRNG_MCTL, field ENT_VAL[10] (RO)
 *
 * Read only: Entropy Valid. Will assert only if TRNG ACC bit is set, and then
 * after an entropy value is generated. Will be cleared when ENT15 is read. (ENT0
 * through ENT14 should be read before reading ENT15).
 */
/*@{*/
/*! @brief Read current value of the TRNG_MCTL_ENT_VAL field. */
#define TRNG_RD_MCTL_ENT_VAL(base) ((TRNG_MCTL_REG(base) & TRNG_MCTL_ENT_VAL_MASK) >> TRNG_MCTL_ENT_VAL_SHIFT)
/*@}*/

/*!
 * @name Register TRNG_MCTL, field ERR[12] (W1C)
 *
 * Read: Error status. 1 = error detected. 0 = no error.Write: Write 1 to clear
 * errors. Writing 0 has no effect.
 */
/*@{*/
/*! @brief Read current value of the TRNG_MCTL_ERR field. */
#define TRNG_RD_MCTL_ERR(base) ((TRNG_MCTL_REG(base) & TRNG_MCTL_ERR_MASK) >> TRNG_MCTL_ERR_SHIFT)

/*! @brief Set the ERR field to a new value. */
#define TRNG_WR_MCTL_ERR(base, value) (TRNG_RMW_MCTL(base, TRNG_MCTL_ERR_MASK, TRNG_MCTL_ERR(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SDCTL - RNG Seed Control Register
 ******************************************************************************/

/*!
 * @brief TRNG_SDCTL - RNG Seed Control Register (RW)
 *
 * Reset value: 0x0C8009C4U
 *
 * The RNG Seed Control Register contains two fields. One field defines the
 * length (in system clocks) of each Entropy sample (ENT_DLY), and the other field
 * indicates the number of samples that will taken during each TRNG Entropy
 * generation (SAMP_SIZE).
 */
/*!
 * @name Constants and macros for entire TRNG_SDCTL register
 */
/*@{*/
#define TRNG_SDCTL_REG(base) ((base)->SDCTL)
#define TRNG_RD_SDCTL(base) (TRNG_SDCTL_REG(base))
#define TRNG_WR_SDCTL(base, value) (TRNG_SDCTL_REG(base) = (value))
#define TRNG_RMW_SDCTL(base, mask, value) (TRNG_WR_SDCTL(base, (TRNG_RD_SDCTL(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_SDCTL bitfields
 */

/*!
 * @name Register TRNG_SDCTL, field SAMP_SIZE[15:0] (RW)
 *
 * Sample Size. Defines the total number of Entropy samples that will be taken
 * during Entropy generation. This field is writable only if MCTL[PRGM] bit is 1.
 * This field will read zeroes if MCTL[PRGM] = 0. This field is cleared to 09C4h
 * (decimal 2500) by writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SDCTL_SAMP_SIZE field. */
#define TRNG_RD_SDCTL_SAMP_SIZE(base) ((TRNG_SDCTL_REG(base) & TRNG_SDCTL_SAMP_SIZE_MASK) >> TRNG_SDCTL_SAMP_SIZE_SHIFT)

/*! @brief Set the SAMP_SIZE field to a new value. */
#define TRNG_WR_SDCTL_SAMP_SIZE(base, value) \
    (TRNG_RMW_SDCTL(base, TRNG_SDCTL_SAMP_SIZE_MASK, TRNG_SDCTL_SAMP_SIZE(value)))
/*@}*/

/*!
 * @name Register TRNG_SDCTL, field ENT_DLY[31:16] (RW)
 *
 * Entropy Delay. Defines the length (in system clocks) of each Entropy sample
 * taken. This field is writable only if MCTL[PRGM] bit is 1. This field will read
 * zeroes if MCTL[PRGM] = 0. This field is cleared to 0C80h (decimal 3200) by
 * writing the MCTL[RST_DEF] bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SDCTL_ENT_DLY field. */
#define TRNG_RD_SDCTL_ENT_DLY(base) ((TRNG_SDCTL_REG(base) & TRNG_SDCTL_ENT_DLY_MASK) >> TRNG_SDCTL_ENT_DLY_SHIFT)

/*! @brief Set the ENT_DLY field to a new value. */
#define TRNG_WR_SDCTL_ENT_DLY(base, value) (TRNG_RMW_SDCTL(base, TRNG_SDCTL_ENT_DLY_MASK, TRNG_SDCTL_ENT_DLY(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SBLIM - RNG Sparse Bit Limit Register
 ******************************************************************************/

/*!
 * @brief TRNG_SBLIM - RNG Sparse Bit Limit Register (RW)
 *
 * Reset value: 0x0000003FU
 *
 * The RNG Sparse Bit Limit Register is used when Von Neumann sampling is
 * selected during Entropy Generation. It defines the maximum number of consecutive Von
 * Neumann samples which may be discarded before an error is generated. Note
 * that this address (0xBASE_0614) is used as SBLIM only if MCTL[PRGM] is 1. If
 * MCTL[PRGM] is 0, this address is used as TOTSAM readback register.
 */
/*!
 * @name Constants and macros for entire TRNG_SBLIM register
 */
/*@{*/
#define TRNG_SBLIM_REG(base) ((base)->SBLIM)
#define TRNG_RD_SBLIM(base) (TRNG_SBLIM_REG(base))
#define TRNG_WR_SBLIM(base, value) (TRNG_SBLIM_REG(base) = (value))
#define TRNG_RMW_SBLIM(base, mask, value) (TRNG_WR_SBLIM(base, (TRNG_RD_SBLIM(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_SBLIM bitfields
 */

/*!
 * @name Register TRNG_SBLIM, field SB_LIM[9:0] (RW)
 *
 * Sparse Bit Limit. During Von Neumann sampling (if enabled by MCTL[SAMP_MODE],
 * samples are discarded if two consecutive raw samples are both 0 or both 1. If
 * this discarding occurs for a long period of time, it indicates that there is
 * insufficient Entropy. The Sparse Bit Limit defines the maximum number of
 * consecutive samples that may be discarded before an error is generated. This field
 * is writable only if MCTL[PRGM] bit is 1. This register is cleared to 03hF by
 * writing the MCTL[RST_DEF] bit to 1. Note that if MCTL[PRGM] bit is 0, this
 * register address is used to read the Total Samples count in register TOTSAM, as
 * defined in the following section.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SBLIM_SB_LIM field. */
#define TRNG_RD_SBLIM_SB_LIM(base) ((TRNG_SBLIM_REG(base) & TRNG_SBLIM_SB_LIM_MASK) >> TRNG_SBLIM_SB_LIM_SHIFT)

/*! @brief Set the SB_LIM field to a new value. */
#define TRNG_WR_SBLIM_SB_LIM(base, value) (TRNG_RMW_SBLIM(base, TRNG_SBLIM_SB_LIM_MASK, TRNG_SBLIM_SB_LIM(value)))
/*@}*/

/*******************************************************************************
 * TRNG_SCMISC - RNG Statistical Check Miscellaneous Register
 ******************************************************************************/

/*!
 * @brief TRNG_SCMISC - RNG Statistical Check Miscellaneous Register (RW)
 *
 * Reset value: 0x0001001FU
 *
 * The RNG Statistical Check Miscellaneous Register contains the Long Run
 * Maximum Limit value and the Retry Count value. This register is accessible only when
 * the MCTL[PRGM] bit is 1, otherwise this register will read zeroes, and cannot
 * be written.
 */
/*!
 * @name Constants and macros for entire TRNG_SCMISC register
 */
/*@{*/
#define TRNG_SCMISC_REG(base) ((base)->SCMISC)
#define TRNG_RD_SCMISC(base) (TRNG_SCMISC_REG(base))
#define TRNG_WR_SCMISC(base, value) (TRNG_SCMISC_REG(base) = (value))
#define TRNG_RMW_SCMISC(base, mask, value) (TRNG_WR_SCMISC(base, (TRNG_RD_SCMISC(base) & ~(mask)) | (value)))
/*@}*/

/*
 * Constants & macros for individual TRNG_SCMISC bitfields
 */

/*!
 * @name Register TRNG_SCMISC, field LRUN_MAX[7:0] (RW)
 *
 * LONG RUN MAX LIMIT. This value is the largest allowable number of consecutive
 * samples of all 1, or all 0, that is allowed during the Entropy generation.
 * This field is writable only if MCTL[PRGM] bit is 1. This field will read zeroes
 * if MCTL[PRGM] = 0. This field is cleared to 22h by writing the MCTL[RST_DEF]
 * bit to 1.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SCMISC_LRUN_MAX field. */
#define TRNG_RD_SCMISC_LRUN_MAX(base) \
    ((TRNG_SCMISC_REG(base) & TRNG_SCMISC_LRUN_MAX_MASK) >> TRNG_SCMISC_LRUN_MAX_SHIFT)

/*! @brief Set the LRUN_MAX field to a new value. */
#define TRNG_WR_SCMISC_LRUN_MAX(base, value) \
    (TRNG_RMW_SCMISC(base, TRNG_SCMISC_LRUN_MAX_MASK, TRNG_SCMISC_LRUN_MAX(value)))
/*@}*/

/*******************************************************************************
 * TRNG_ENT - RNG TRNG Entropy Read Register
 ******************************************************************************/

/*!
 * @brief TRNG_ENT - RNG TRNG Entropy Read Register (RO)
 *
 * Reset value: 0x00000000U
 *
 * The RNG TRNG can be programmed to generate an entropy value that is readable
 * via the SkyBlue bus. To do this, set the MCTL[TRNG_ACC] bit to 1. Once the
 * entropy value has been generated, the MCTL[ENT_VAL] bit will be set to 1. At this
 * point, ENT0 through ENT15 may be read to retrieve the 512-bit entropy value.
 * Note that once ENT15 is read, the entropy value will be cleared and a new
 * value will begin generation, so it is important that ENT15 be read last. These
 * registers are readable only when MCTL[PRGM] = 0 (Run Mode), MCTL[TRNG_ACC] = 1
 * (TRNG access mode) and MCTL[ENT_VAL] = 1, otherwise zeroes will be read.
 */
/*!
 * @name Constants and macros for entire TRNG_ENT register
 */
/*@{*/
#define TRNG_ENT_REG(base, index) ((base)->ENT[index])
#define TRNG_RD_ENT(base, index) (TRNG_ENT_REG(base, index))
/*@}*/

/*******************************************************************************
 * TRNG_SEC_CFG - RNG Security Configuration Register
 ******************************************************************************/

/*!
 * @brief TRNG_SEC_CFG - RNG Security Configuration Register (RW)
 *
 * Reset value: 0x00000000U
 *
 * The RNG Security Configuration Register is a read/write register used to
 * control the test mode, programmability and state modes of the RNG. Many bits are
 * place holders for this version. More configurability will be added here. Clears
 * on asynchronous reset. For SA-TRNG releases before 2014/July/01, offsets 0xA0
 * to 0xAC used to be 0xB0 to 0xBC respectively. So, update newer tests that use
 * these registers, if hard coded.
 */
/*!
 * @name Constants and macros for entire TRNG_SEC_CFG register
 */
/*@{*/
#define TRNG_SEC_CFG_REG(base) ((base)->SEC_CFG)
#define TRNG_RD_SEC_CFG(base) (TRNG_SEC_CFG_REG(base))
#define TRNG_WR_SEC_CFG(base, value) (TRNG_SEC_CFG_REG(base) = (value))
#define TRNG_RMW_SEC_CFG(base, mask, value) (TRNG_WR_SEC_CFG(base, (TRNG_RD_SEC_CFG(base) & ~(mask)) | (value)))
/*@}*/

/*!
 * @name Register TRNG_SEC_CFG, field NO_PRGM[1] (RW)
 *
 * If set the TRNG registers cannot be programmed. That is, regardless of the
 * TRNG access mode in the SA-TRNG Miscellaneous Control Register.
 *
 * Values:
 * - 0b0 - Programability of registers controlled only by the RNG Miscellaneous
 *     Control Register's access mode bit.
 * - 0b1 - Overides RNG Miscellaneous Control Register access mode and prevents
 *     TRNG register programming.
 */
/*@{*/
/*! @brief Read current value of the TRNG_SEC_CFG_NO_PRGM field. */
#define TRNG_RD_SEC_CFG_NO_PRGM(base) \
    ((TRNG_SEC_CFG_REG(base) & TRNG_SEC_CFG_NO_PRGM_MASK) >> TRNG_SEC_CFG_NO_PRGM_SHIFT)

/*! @brief Set the NO_PRGM field to a new value. */
#define TRNG_WR_SEC_CFG_NO_PRGM(base, value) \
    (TRNG_RMW_SEC_CFG(base, TRNG_SEC_CFG_NO_PRGM_MASK, TRNG_SEC_CFG_NO_PRGM(value)))
/*@}*/

/*! @brief Array to map TRNG instance number to base pointer. */
static TRNG_Type *const s_trngBases[] = TRNG_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Clock array name */
static const clock_ip_name_t s_trngClock[] = TRNG_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Prototypes
 *******************************************************************************/
static status_t trng_ApplyUserConfig(TRNG_Type *base, const trng_config_t *userConfig);
static status_t trng_SetRetryCount(TRNG_Type *base, uint8_t retry_count);
static status_t trng_SetStatisticalCheckLimit(TRNG_Type *base,
                                              trng_statistical_check_t statistical_check,
                                              const trng_statistical_check_limit_t *limit);
static uint32_t trng_ReadEntropy(TRNG_Type *base, uint32_t index);
static uint32_t trng_GetInstance(TRNG_Type *base);

/*******************************************************************************
 * Code
 ******************************************************************************/

static uint32_t trng_GetInstance(TRNG_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_trngBases); instance++)
    {
        if (s_trngBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_trngBases));

    return instance;
}

/*FUNCTION*********************************************************************
 *
 * Function Name : TRNG_InitUserConfigDefault
 * Description   :  Initializes user configuration structure to default settings.
 *
 *END*************************************************************************/
/*!
 * brief Initializes the user configuration structure to default values.
 *
 * This function initializes the configuration structure to default values. The default
 * values are as follows.
 * code
 *     user_config->lock = 0;
 *     user_config->clockMode = kTRNG_ClockModeRingOscillator;
 *     user_config->ringOscDiv = kTRNG_RingOscDiv0;  Or  to other kTRNG_RingOscDiv[2|8] depending on the platform.
 *     user_config->sampleMode = kTRNG_SampleModeRaw;
 *     user_config->entropyDelay = 3200;
 *     user_config->sampleSize = 2500;
 *     user_config->sparseBitLimit = TRNG_USER_CONFIG_DEFAULT_SPARSE_BIT_LIMIT;
 *     user_config->retryCount = 63;
 *     user_config->longRunMaxLimit = 34;
 *     user_config->monobitLimit.maximum = 1384;
 *     user_config->monobitLimit.minimum = 1116;
 *     user_config->runBit1Limit.maximum = 405;
 *     user_config->runBit1Limit.minimum = 227;
 *     user_config->runBit2Limit.maximum = 220;
 *     user_config->runBit2Limit.minimum = 98;
 *     user_config->runBit3Limit.maximum = 125;
 *     user_config->runBit3Limit.minimum = 37;
 *     user_config->runBit4Limit.maximum = 75;
 *     user_config->runBit4Limit.minimum = 11;
 *     user_config->runBit5Limit.maximum = 47;
 *     user_config->runBit5Limit.minimum = 1;
 *     user_config->runBit6PlusLimit.maximum = 47;
 *     user_config->runBit6PlusLimit.minimum = 1;
 *     user_config->pokerLimit.maximum = 26912;
 *     user_config->pokerLimit.minimum = 24445;
 *     user_config->frequencyCountLimit.maximum = 25600;
 *     user_config->frequencyCountLimit.minimum = 1600;
 * endcode
 *
 * param user_config   User configuration structure.
 * return If successful, returns the kStatus_TRNG_Success. Otherwise, it returns an error.
 */
status_t TRNG_GetDefaultConfig(trng_config_t *userConfig)
{
    status_t result;

    if (userConfig != NULL)
    {
        userConfig->lock           = (bool)TRNG_USER_CONFIG_DEFAULT_LOCK;
        userConfig->clockMode      = kTRNG_ClockModeRingOscillator;
        userConfig->ringOscDiv     = TRNG_USER_CONFIG_DEFAULT_OSC_DIV;
        userConfig->sampleMode     = kTRNG_SampleModeRaw;
        userConfig->entropyDelay   = TRNG_USER_CONFIG_DEFAULT_ENTROPY_DELAY;
        userConfig->sampleSize     = TRNG_USER_CONFIG_DEFAULT_SAMPLE_SIZE;
        userConfig->sparseBitLimit = TRNG_USER_CONFIG_DEFAULT_SPARSE_BIT_LIMIT;

        /* Statistical Check Parameters.*/
        userConfig->retryCount      = TRNG_USER_CONFIG_DEFAULT_RETRY_COUNT;
        userConfig->longRunMaxLimit = TRNG_USER_CONFIG_DEFAULT_RUN_MAX_LIMIT;

        userConfig->monobitLimit.maximum        = TRNG_USER_CONFIG_DEFAULT_MONOBIT_MAXIMUM;
        userConfig->monobitLimit.minimum        = TRNG_USER_CONFIG_DEFAULT_MONOBIT_MINIMUM;
        userConfig->runBit1Limit.maximum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT1_MAXIMUM;
        userConfig->runBit1Limit.minimum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT1_MINIMUM;
        userConfig->runBit2Limit.maximum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT2_MAXIMUM;
        userConfig->runBit2Limit.minimum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT2_MINIMUM;
        userConfig->runBit3Limit.maximum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT3_MAXIMUM;
        userConfig->runBit3Limit.minimum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT3_MINIMUM;
        userConfig->runBit4Limit.maximum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT4_MAXIMUM;
        userConfig->runBit4Limit.minimum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT4_MINIMUM;
        userConfig->runBit5Limit.maximum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT5_MAXIMUM;
        userConfig->runBit5Limit.minimum        = TRNG_USER_CONFIG_DEFAULT_RUNBIT5_MINIMUM;
        userConfig->runBit6PlusLimit.maximum    = TRNG_USER_CONFIG_DEFAULT_RUNBIT6PLUS_MAXIMUM;
        userConfig->runBit6PlusLimit.minimum    = TRNG_USER_CONFIG_DEFAULT_RUNBIT6PLUS_MINIMUM;
        userConfig->pokerLimit.maximum          = TRNG_USER_CONFIG_DEFAULT_POKER_MAXIMUM;
        userConfig->pokerLimit.minimum          = TRNG_USER_CONFIG_DEFAULT_POKER_MINIMUM;
        userConfig->frequencyCountLimit.maximum = TRNG_USER_CONFIG_DEFAULT_FREQUENCY_MAXIMUM;
        userConfig->frequencyCountLimit.minimum = TRNG_USER_CONFIG_DEFAULT_FREQUENCY_MINIMUM;

        result = kStatus_Success;
    }
    else
    {
        result = kStatus_InvalidArgument;
    }

    return result;
}

/*!
 * @brief Sets the TRNG retry count.
 *
 * This function sets the retry counter which defines the number of times a
 * statistical check may fails during the TRNG Entropy Generation before
 * generating an error.
 */
static status_t trng_SetRetryCount(TRNG_Type *base, uint8_t retry_count)
{
    status_t status;

    if ((retry_count >= 1u) && (retry_count <= 15u))
    {
        /* Set retry count.*/
        TRNG_WR_SCMISC_RTY_CT(base, retry_count);
        status = kStatus_Success;
    }
    else
    {
        status = kStatus_InvalidArgument;
    }
    return status;
}

/*!
 * @brief Sets statistical check limits.
 *
 * This function is used to set minimum and maximum limits of statistical checks.
 *
 */
static status_t trng_SetStatisticalCheckLimit(TRNG_Type *base,
                                              trng_statistical_check_t statistical_check,
                                              const trng_statistical_check_limit_t *limit)
{
    uint32_t range;
    status_t status = kStatus_Success;

    if ((NULL != limit) && (limit->maximum > limit->minimum))
    {
        range = limit->maximum - limit->minimum; /* Registers use range instead of minimum value.*/

        if (statistical_check == kTRNG_StatisticalCheckMonobit) /* Allowable maximum and minimum number of ones/zero
                                                                   detected during entropy generation. */
        {
            if ((range <= 0xffffu) && (limit->maximum <= 0xffffu))
            {
                TRNG_WR_SCML_MONO_MAX(base, limit->maximum);
                TRNG_WR_SCML_MONO_RNG(base, range);
            }
            else
            {
                status = kStatus_InvalidArgument;
            }
        }
        else if (statistical_check == kTRNG_StatisticalCheckRunBit1) /* Allowable maximum and minimum number of runs of
                                                                        length 1 detected during entropy generation. */
        {
            if ((range <= 0x7fffu) && (limit->maximum <= 0x7fffu))
            {
                TRNG_WR_SCR1L_RUN1_MAX(base, limit->maximum);
                TRNG_WR_SCR1L_RUN1_RNG(base, range);
            }
            else
            {
                status = kStatus_InvalidArgument;
            }
        }
        else if (statistical_check == kTRNG_StatisticalCheckRunBit2) /* Allowable maximum and minimum number of runs of
                                                                        length 2 detected during entropy generation. */
        {
            if ((range <= 0x3fffu) && (limit->maximum <= 0x3fffu))
            {
                TRNG_WR_SCR2L_RUN2_MAX(base, limit->maximum);
                TRNG_WR_SCR2L_RUN2_RNG(base, range);
            }
            else
            {
                status = kStatus_InvalidArgument;
            }
        }
        else if (statistical_check == kTRNG_StatisticalCheckRunBit3) /* Allowable maximum and minimum number of runs of
                                                                        length 3 detected during entropy generation. */
        {
            if ((range <= 0x1fffu) && (limit->maximum <= 0x1fffu))
            {
                TRNG_WR_SCR3L_RUN3_MAX(base, limit->maximum);
                TRNG_WR_SCR3L_RUN3_RNG(base, range);
            }
            else
            {
                status = kStatus_InvalidArgument;
            }
        }
        else if (statistical_check == kTRNG_StatisticalCheckRunBit4) /* Allowable maximum and minimum number of runs of
                                                                        length 4 detected during entropy generation. */
        {
            if ((range <= 0xfffu) && (limit->maximum <= 0xfffu))
            {
                TRNG_WR_SCR4L_RUN4_MAX(base, limit->maximum);
                TRNG_WR_SCR4L_RUN4_RNG(base, range);
            }
            else
            {
                status = kStatus_InvalidArgument;
            }
        }
        else if (statistical_check == kTRNG_StatisticalCheckRunBit5) /* Allowable maximum and minimum number of runs of
                                                                        length 5 detected during entropy generation. */
        {
            if ((range <= 0x7ffu) && (limit->maximum <= 0x7ffu))
            {
                TRNG_WR_SCR5L_RUN5_MAX(base, limit->maximum);
                TRNG_WR_SCR5L_RUN5_RNG(base, range);
            }
            else
            {
                status = kStatus_InvalidArgument;
            }
        }
        else if (statistical_check == kTRNG_StatisticalCheckRunBit6Plus) /* Allowable maximum and minimum number of
                                                                            length 6 or more detected during entropy
                                                                            generation */
        {
            if ((range <= 0x7ffu) && (limit->maximum <= 0x7ffu))
            {
                TRNG_WR_SCR6PL_RUN6P_MAX(base, limit->maximum);
                TRNG_WR_SCR6PL_RUN6P_RNG(base, range);
            }
            else
            {
                status = kStatus_InvalidArgument;
            }
        }
        else if (statistical_check == kTRNG_StatisticalCheckPoker) /* Allowable maximum and minimum limit of "Poker
                                                                      Test" detected during entropy generation . */
        {
            if ((range <= 0xffffu) && (limit->maximum <= 0xffffffu))
            {
                TRNG_WR_PKRMAX_PKR_MAX(base, limit->maximum);
                TRNG_WR_PKRRNG_PKR_RNG(base, range);
            }
            else
            {
                status = kStatus_InvalidArgument;
            }
        }
        else if (statistical_check == kTRNG_StatisticalCheckFrequencyCount) /* Allowable maximum and minimum limit of
                                                                               entropy sample frquency count during
                                                                               entropy generation . */
        {
            if ((limit->minimum <= 0x3fffffu) && (limit->maximum <= 0x3fffffu))
            {
                TRNG_WR_FRQMAX_FRQ_MAX(base, limit->maximum);
                TRNG_WR_FRQMIN_FRQ_MIN(base, limit->minimum);
            }
            else
            {
                status = kStatus_InvalidArgument;
            }
        }
        else
        {
            status = kStatus_InvalidArgument;
        }
    }

    return status;
}

/*FUNCTION*********************************************************************
 *
 * Function Name : trng_ApplyUserConfig
 * Description   : Apply user configuration settings to TRNG module.
 *
 *END*************************************************************************/
static status_t trng_ApplyUserConfig(TRNG_Type *base, const trng_config_t *userConfig)
{
    status_t status;

    status = trng_SetRetryCount(base, userConfig->retryCount);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = trng_SetStatisticalCheckLimit(base, kTRNG_StatisticalCheckMonobit, &userConfig->monobitLimit);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = trng_SetStatisticalCheckLimit(base, kTRNG_StatisticalCheckRunBit1, &userConfig->runBit1Limit);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = trng_SetStatisticalCheckLimit(base, kTRNG_StatisticalCheckRunBit2, &userConfig->runBit2Limit);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = trng_SetStatisticalCheckLimit(base, kTRNG_StatisticalCheckRunBit3, &userConfig->runBit3Limit);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = trng_SetStatisticalCheckLimit(base, kTRNG_StatisticalCheckRunBit4, &userConfig->runBit4Limit);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = trng_SetStatisticalCheckLimit(base, kTRNG_StatisticalCheckRunBit5, &userConfig->runBit5Limit);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = trng_SetStatisticalCheckLimit(base, kTRNG_StatisticalCheckRunBit6Plus, &userConfig->runBit6PlusLimit);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = trng_SetStatisticalCheckLimit(base, kTRNG_StatisticalCheckPoker, &userConfig->pokerLimit);
    if (kStatus_Success != status)
    {
        return status;
    }

    status =
        trng_SetStatisticalCheckLimit(base, kTRNG_StatisticalCheckFrequencyCount, &userConfig->frequencyCountLimit);
    if (kStatus_Success != status)
    {
        return status;
    }

    TRNG_WR_MCTL_FOR_SCLK(base, userConfig->clockMode);
    TRNG_WR_MCTL_OSC_DIV(base, userConfig->ringOscDiv);
    TRNG_WR_MCTL_SAMP_MODE(base, userConfig->sampleMode);
    TRNG_WR_SDCTL_ENT_DLY(base, userConfig->entropyDelay);
    TRNG_WR_SDCTL_SAMP_SIZE(base, userConfig->sampleSize);
    TRNG_WR_SBLIM_SB_LIM(base, userConfig->sparseBitLimit);
    TRNG_WR_SCMISC_LRUN_MAX(base, userConfig->longRunMaxLimit);

    return status;
}

/*!
 * @brief Gets a entry data from the TRNG.
 *
 * This function gets an entropy data from TRNG.
 * Entropy data is spread over TRNG_ENT_COUNT registers.
 * Read register number is defined by index parameter.
 */
static uint32_t trng_ReadEntropy(TRNG_Type *base, uint32_t index)
{
    uint32_t data;

    index = index % TRNG_ENT_COUNT; /* This way we can use incremental index without limit control from application.*/

    data = TRNG_RD_ENT(base, index);

    if (index == (TRNG_ENT_COUNT - 1u))
    {
        /* Dummy read. Defect workaround.
         * TRNG could not clear ENT_VAL  flag automatically, application
         * had to do a dummy reading operation for anyone TRNG register
         * to clear it firstly, then to read the RTENT0 to RTENT15 again */
        index = TRNG_RD_ENT(base, 0);
    }

    return data;
}

/*!
 * brief Initializes the TRNG.
 *
 * This function initializes the TRNG.
 * When called, the TRNG entropy generation starts immediately.
 *
 * param base  TRNG base address
 * param userConfig    Pointer to the initialization configuration structure.
 * return If successful, returns the kStatus_TRNG_Success. Otherwise, it returns an error.
 */
status_t TRNG_Init(TRNG_Type *base, const trng_config_t *userConfig)
{
    status_t result;

    /* Check input parameters.*/
    if ((base != NULL) && (userConfig != NULL))
    {
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
        /* Enable the clock gate. */
        CLOCK_EnableClock(s_trngClock[trng_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

        /* Reset the registers of TRNG module to reset state. */
        /* Must be in program mode.*/
        TRNG_WR_MCTL_PRGM(base, kTRNG_WorkModeProgram);
        /* Reset Defaults.*/
        TRNG_WR_MCTL_RST_DEF(base, 1);

        /* Set configuration.*/
        if ((result = trng_ApplyUserConfig(base, userConfig)) == kStatus_Success)
        {
            /* Start entropy generation.*/
            /* Set to Run mode.*/
            TRNG_WR_MCTL_PRGM(base, kTRNG_WorkModeRun);
#if !(defined(FSL_FEATURE_TRNG_HAS_NO_TRNG_ACC) && (FSL_FEATURE_TRNG_HAS_NO_TRNG_ACC > 0))
            /* Enable TRNG Access Mode. To generate an Entropy
             * value that can be read via the true0-true15 registers.*/
            TRNG_WR_MCTL_TRNG_ACC(base, 1);
#endif /* !FSL_FEATURE_TRNG_HAS_NO_TRNG_ACC */

            (void)trng_ReadEntropy(base, (TRNG_ENT_COUNT - 1u));

            if (true == userConfig->lock) /* Disable programmability of TRNG registers. */
            {
                TRNG_WR_SEC_CFG_NO_PRGM(base, 1);
            }

            result = kStatus_Success;
        }
    }
    else
    {
        result = kStatus_InvalidArgument;
    }

    return result;
}

/*!
 * brief Shuts down the TRNG.
 *
 * This function shuts down the TRNG.
 *
 * param base  TRNG base address.
 */
void TRNG_Deinit(TRNG_Type *base)
{
    /* Check input parameters.*/
    if (NULL != base)
    {
        /* Move to program mode. Stop entropy generation.*/
        TRNG_WR_MCTL_PRGM(base, kTRNG_WorkModeProgram);

        /* Check before clock stop.
         TRNG turns on the TRNG free-running ring oscillator whenever new entropy
         is being generated and turns off the ring oscillator when entropy generation
         is complete. If the TRNG clock is stopped while the TRNG ring oscillator
         is running, the oscillator continues running though the RNG clock.
         is stopped. */
        while (TRNG_RD_MCTL_TSTOP_OK(base) == 0u)
        {
        }

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
        /* Disable Clock*/
        CLOCK_DisableClock(s_trngClock[trng_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    }
}

/*!
 * brief Gets random data.
 *
 * This function gets random data from the TRNG.
 *
 * param base  TRNG base address.
 * param data  Pointer address used to store random data.
 * param dataSize  Size of the buffer pointed by the data parameter.
 * return random data
 */
status_t TRNG_GetRandomData(TRNG_Type *base, void *data, size_t dataSize)
{
    status_t result = kStatus_Success;
    uint32_t random_32;
    uint8_t *random_p;
    uint32_t random_size;
    uint8_t *data_p = (uint8_t *)data;
    uint32_t i;
    uint32_t tmpValidFlag;
    uint32_t tmpErrorFlag;

    int index = 0;

    /* Check input parameters.*/
    if ((NULL != base) && (NULL != data) && (0U != dataSize))
    {
        do
        {
            /* Wait for Valid or Error flag*/
            tmpValidFlag = TRNG_RD_MCTL_ENT_VAL(base);
            tmpErrorFlag = TRNG_RD_MCTL_ERR(base);
            while (tmpValidFlag == 0u && tmpErrorFlag == 0u)
            {
                tmpValidFlag = TRNG_RD_MCTL_ENT_VAL(base);
                tmpErrorFlag = TRNG_RD_MCTL_ERR(base);
            }

            /* Check HW error.*/
            if (0U != TRNG_RD_MCTL_ERR(base))
            {
                result = kStatus_Fail; /* TRNG module error occurred */
                /* Clear error.*/
                TRNG_WR_MCTL_ERR(base, 1);
                break; /* No sense stay here.*/
            }

            /* Read Entropy.*/
            random_32 = trng_ReadEntropy(base, (uint32_t)index++);

            random_p = (uint8_t *)&random_32;

            if (dataSize < sizeof(random_32))
            {
                random_size = dataSize;
            }
            else
            {
                random_size = sizeof(random_32);
            }

            for (i = 0U; i < random_size; i++)
            {
                *data_p++ = *random_p++;
            }

            dataSize -= random_size;
        } while (dataSize > 0u);

        /* Start a new entropy generation.
        It is done by reading of the last entropy register.*/
        if (((unsigned)index % TRNG_ENT_COUNT) != (TRNG_ENT_COUNT - 1u))
        {
            (void)trng_ReadEntropy(base, (TRNG_ENT_COUNT - 1u));
        }
    }
    else
    {
        result = kStatus_InvalidArgument;
    }

    return result;
}

#endif /* FSL_FEATURE_SOC_TRNG_COUNT */
