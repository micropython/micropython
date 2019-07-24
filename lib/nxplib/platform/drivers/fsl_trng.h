/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_TRNG_DRIVER_H_
#define _FSL_TRNG_DRIVER_H_

#include "fsl_common.h"

#if defined(FSL_FEATURE_SOC_TRNG_COUNT) && FSL_FEATURE_SOC_TRNG_COUNT

/*!
 * @addtogroup trng
 * @{
 */

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief TRNG driver version 2.0.4.
 *
 * Current version: 2.0.4
 *
 * Change log:
 * - version 2.0.4
 *   - Fix MISRA-2012 issues.
 * - Version 2.0.3
 *   - update TRNG_Init to restart entropy generation
 * - Version 2.0.2
 *   - fix MISRA issues
 * - Version 2.0.1
 *   - add support for KL8x and KL28Z
 *   - update default OSCDIV for K81 to divide by 2
 */
#define FSL_TRNG_DRIVER_VERSION (MAKE_VERSION(2, 0, 4))
/*@}*/

/*! @brief TRNG sample mode. Used by trng_config_t. */
typedef enum _trng_sample_mode
{
    kTRNG_SampleModeVonNeumann = 0U, /*!< Use von Neumann data in both Entropy shifter and Statistical Checker. */
    kTRNG_SampleModeRaw        = 1U, /*!< Use raw data into both Entropy shifter and Statistical Checker. */
    kTRNG_SampleModeVonNeumannRaw =
        2U /*!< Use von Neumann data in Entropy shifter. Use raw data into Statistical Checker. */
} trng_sample_mode_t;

/*! @brief TRNG clock mode. Used by trng_config_t. */
typedef enum _trng_clock_mode
{
    kTRNG_ClockModeRingOscillator = 0U, /*!< Ring oscillator is used to operate the TRNG (default). */
    kTRNG_ClockModeSystem         = 1U  /*!< System clock is used to operate the TRNG. This is for test use only, and
                                           indeterminate results may occur. */
} trng_clock_mode_t;

/*! @brief TRNG ring oscillator divide. Used by trng_config_t. */
typedef enum _trng_ring_osc_div
{
    kTRNG_RingOscDiv0 = 0U, /*!< Ring oscillator with no divide */
    kTRNG_RingOscDiv2 = 1U, /*!< Ring oscillator divided-by-2. */
    kTRNG_RingOscDiv4 = 2U, /*!< Ring oscillator divided-by-4. */
    kTRNG_RingOscDiv8 = 3U  /*!< Ring oscillator divided-by-8. */
} trng_ring_osc_div_t;

/*! @brief Data structure for definition of statistical check limits. Used by trng_config_t. */
typedef struct _trng_statistical_check_limit
{
    uint32_t maximum; /*!< Maximum limit.*/
    uint32_t minimum; /*!< Minimum limit.*/
} trng_statistical_check_limit_t;

/*!
 * @brief Data structure for the TRNG initialization
 *
 * This structure initializes the TRNG by calling the TRNG_Init() function.
 * It contains all TRNG configurations.
 */
typedef struct _trng_user_config
{
    bool lock;                      /*!< @brief Disable programmability of TRNG registers.  */
    trng_clock_mode_t clockMode;    /*!< @brief Clock mode used to operate TRNG.*/
    trng_ring_osc_div_t ringOscDiv; /*!< @brief Ring oscillator divide used by TRNG. */
    trng_sample_mode_t sampleMode;  /*!< @brief Sample mode of the TRNG ring oscillator. */
    /* Seed Control*/
    uint16_t
        entropyDelay; /*!< @brief Entropy Delay. Defines the length (in system clocks) of each Entropy sample taken. */
    uint16_t sampleSize; /*!< @brief Sample Size. Defines the total number of Entropy samples that will be taken during
                            Entropy generation. */
    uint16_t sparseBitLimit; /*!< @brief Sparse Bit Limit which defines the maximum number of
                              * consecutive samples that may be discarded before an error is generated.
                              * This limit is used only for during von Neumann sampling (enabled by
                              * TRNG_HAL_SetSampleMode()). Samples are discarded if two consecutive raw samples are both
                              * 0 or both 1. If this discarding occurs for a long period of time, it indicates that
                              * there is insufficient Entropy. */
    /* Statistical Check Parameters.*/
    uint8_t retryCount;      /*!< @brief Retry count. It defines the number of times a statistical check may fails
                              * during the TRNG Entropy Generation before generating an error. */
    uint8_t longRunMaxLimit; /*!< @brief Largest allowable number of consecutive samples of all 1, or all 0,
                              * that is allowed during the Entropy generation. */
    trng_statistical_check_limit_t monobitLimit; /*!< @brief Maximum and minimum limits for statistical check of number
                                                    of ones/zero detected during entropy generation. */
    trng_statistical_check_limit_t runBit1Limit; /*!< @brief Maximum and minimum limits for statistical check of number
                                                    of runs of length 1 detected during entropy generation. */
    trng_statistical_check_limit_t runBit2Limit; /*!< @brief Maximum and minimum limits for statistical check of number
                                                    of runs of length 2 detected during entropy generation. */
    trng_statistical_check_limit_t runBit3Limit; /*!< @brief Maximum and minimum limits for statistical check of number
                                                    of runs of length 3 detected during entropy generation. */
    trng_statistical_check_limit_t runBit4Limit; /*!< @brief Maximum and minimum limits for statistical check of number
                                                    of runs of length 4 detected during entropy generation. */
    trng_statistical_check_limit_t runBit5Limit; /*!< @brief Maximum and minimum limits for statistical check of number
                                                    of runs of length 5 detected during entropy generation. */
    trng_statistical_check_limit_t runBit6PlusLimit; /*!< @brief Maximum and minimum limits for statistical check of
                                                        number of runs of length 6 or more detected during entropy
                                                        generation. */
    trng_statistical_check_limit_t
        pokerLimit; /*!< @brief Maximum and minimum limits for statistical check of "Poker Test". */
    trng_statistical_check_limit_t frequencyCountLimit; /*!< @brief Maximum and minimum limits for statistical check of
                                                           entropy sample frequency count. */
} trng_config_t;

/*******************************************************************************
 * API
 *******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the user configuration structure to default values.
 *
 * This function initializes the configuration structure to default values. The default
 * values are as follows.
 * @code
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
 * @endcode
 *
 * @param user_config   User configuration structure.
 * @return If successful, returns the kStatus_TRNG_Success. Otherwise, it returns an error.
 */
status_t TRNG_GetDefaultConfig(trng_config_t *userConfig);

/*!
 * @brief Initializes the TRNG.
 *
 * This function initializes the TRNG.
 * When called, the TRNG entropy generation starts immediately.
 *
 * @param base  TRNG base address
 * @param userConfig    Pointer to the initialization configuration structure.
 * @return If successful, returns the kStatus_TRNG_Success. Otherwise, it returns an error.
 */
status_t TRNG_Init(TRNG_Type *base, const trng_config_t *userConfig);

/*!
 * @brief Shuts down the TRNG.
 *
 * This function shuts down the TRNG.
 *
 * @param base  TRNG base address.
 */
void TRNG_Deinit(TRNG_Type *base);

/*!
 * @brief Gets random data.
 *
 * This function gets random data from the TRNG.
 *
 * @param base  TRNG base address.
 * @param data  Pointer address used to store random data.
 * @param dataSize  Size of the buffer pointed by the data parameter.
 * @return random data
 */
status_t TRNG_GetRandomData(TRNG_Type *base, void *data, size_t dataSize);

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* FSL_FEATURE_SOC_TRNG_COUNT */
#endif /*_FSL_TRNG_H_*/
