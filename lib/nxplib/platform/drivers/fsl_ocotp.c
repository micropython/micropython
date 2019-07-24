/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ocotp.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.ocotp"
#endif

/* Wait time should be not less than 150ns . */
#define OCOTP_TIMING_WAIT_NS (uint64_t)150
/* Relex time should be not less than 100ns . */
#define OCOTP_TIMING_RELEX_NS (uint64_t)100
/* Program time should be rang from 9000ns~11000ns. */
#define OCOTP_TIMING_PROGRAM_NS (uint64_t)10000
/* Read time should be less than 40ns. */
#define OCOTP_TIMING_READ_NS (uint64_t)40

/* Unlock key is 0x3E77. */
#define OCOTP_WRITE_UNLOCK_KEY (0x3E77)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Set read timing configuration.
 *
 * @param base          OCOTP peripheral base addess.
 * @param timingConfig  configuration of timing.
 */
static void OCOTP_SetReadTiming(OCOTP_Type *base, ocotp_timing_t timingConfig);

/*!
 * @brief Set write timing configuration.
 *
 * @param base          OCOTP peripheral base addess.
 * @param timingConfig  configuration of timing.
 */
static void OCOTP_SetWriteTiming(OCOTP_Type *base, ocotp_timing_t timingConfig);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Timing configuration for OCOTP controller. */
ocotp_timing_t s_timingConfig;

/*******************************************************************************
 * Code
 *******************************************************************************/
/* Reload the shadow register. */
void OCOTP_ReloadShadowRegister(OCOTP_Type *base)
{
    assert(NULL != base);

    /* Make sure the OCOTP is ready, Overlapped accesses are not supported by the controller. */
    while (OCOTP_CheckBusyStatus(base))
    {
    }

    /* Clear access error status bit. */
    OCOTP_ClearErrorStatus(base);

    /* Set the read timing. */
    OCOTP_SetReadTiming(base, s_timingConfig);

    /* Wait for the OCOTP controller not busy. */
    while (OCOTP_CheckBusyStatus(base))
    {
    }

    /* Set reload bit. */
    base->CTRL_SET = OCOTP_CTRL_RELOAD_SHADOWS(1);

    /* Wait for the OCOTP controller not busy. */
    while (OCOTP_CheckBusyStatus(base))
    {
    }
    /* Wait for shadow register reload complete. this bit will be auto clear by OCOTP once operation is complete. */
    while (OCOTP_CTRL_RELOAD_SHADOWS_MASK == (base->CTRL & OCOTP_CTRL_RELOAD_SHADOWS_MASK))
    {
    }
}

static void OCOTP_SetReadTiming(OCOTP_Type *base, ocotp_timing_t timingConfig)
{
    uint32_t timingValue = base->TIMING;

    timingValue &= ~(OCOTP_TIMING_RELAX_MASK | OCOTP_TIMING_STROBE_READ_MASK | OCOTP_TIMING_WAIT_MASK);
    timingValue |= OCOTP_TIMING_RELAX(timingConfig.relax) | OCOTP_TIMING_STROBE_READ(timingConfig.strobe_read) |
                   OCOTP_TIMING_WAIT(timingConfig.wait);
    base->TIMING = timingValue;
}

static void OCOTP_SetWriteTiming(OCOTP_Type *base, ocotp_timing_t timingConfig)
{
    uint32_t timingValue = base->TIMING;

    timingValue &= ~(OCOTP_TIMING_RELAX_MASK | OCOTP_TIMING_STROBE_PROG_MASK | OCOTP_TIMING_WAIT_MASK);
    timingValue |= OCOTP_TIMING_RELAX(timingConfig.relax) | OCOTP_TIMING_STROBE_PROG(timingConfig.strobe_prog) |
                   OCOTP_TIMING_WAIT(timingConfig.wait);

    base->TIMING = timingValue;
}

/* Initializes OCOTP controller. */
void OCOTP_Init(OCOTP_Type *base, uint32_t srcClock_Hz)
{
    assert(NULL != base);
    assert(0UL != srcClock_Hz);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable OCOTP clock */
    CLOCK_EnableClock(kCLOCK_Ocotp);
#endif

    /* tWait time shoule be higher than OCOTP_TIMING_WAIT_NS. */
    s_timingConfig.wait = (OCOTP_TIMING_WAIT_NS * srcClock_Hz + 1000000000) / 1000000000 - 1;

    /* tRelax time shoule be higher than OCOTP_TIMING_RELEX_NS. */
    s_timingConfig.relax = (OCOTP_TIMING_RELEX_NS * srcClock_Hz + 1000000000) / 1000000000 - 1;

    /* tStrobe_prog time should be close to OCOTP_TIMING_PROGRAM_NS, only add half of 1000000000. */
    s_timingConfig.strobe_prog =
        (OCOTP_TIMING_PROGRAM_NS * srcClock_Hz + 500000000) / 1000000000 + 2 * (s_timingConfig.relax + 1) - 1;

    /* tStrobe_read time should be higher than OCOTP_TIMING_READ_NS. */
    s_timingConfig.strobe_read =
        (OCOTP_TIMING_READ_NS * srcClock_Hz + 1000000000) / 1000000000 + 2 * (s_timingConfig.relax + 1) - 1;
}

/* De-init OCOTP controller. */
void OCOTP_Deinit(OCOTP_Type *base)
{
    assert(NULL != base);

    s_timingConfig.wait        = 0UL;
    s_timingConfig.relax       = 0UL;
    s_timingConfig.strobe_prog = 0UL;
    s_timingConfig.strobe_read = 0UL;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable OCOTP clock */
    CLOCK_DisableClock(kCLOCK_Ocotp);
#endif
}

/* Read the fuse shadow register. */
uint32_t OCOTP_ReadFuseShadowRegister(OCOTP_Type *base, uint32_t address)
{
    assert(NULL != base);

    /* Make sure the OCOTP is ready, Overlapped accesses are not supported by the controller. */
    while (OCOTP_CheckBusyStatus(base))
    {
    }

    /* If ERROR bit was set, clear access error status bit. */
    if (OCOTP_CheckErrorStatus(base))
    {
        OCOTP_ClearErrorStatus(base);
    }

    /* Set the read timing. */
    OCOTP_SetReadTiming(base, s_timingConfig);

    /* Wait for busy bit is cleared. */
    while (OCOTP_CheckBusyStatus(base))
    {
    }

    /* Clear access error status bit. */
    if (OCOTP_CheckErrorStatus(base))
    {
        OCOTP_ClearErrorStatus(base);
    }

    /* Write requested address to register. */
    base->CTRL_CLR = OCOTP_CTRL_CLR_ADDR_MASK;
    base->CTRL_SET = OCOTP_CTRL_SET_ADDR(address);

    /* Set OCOTP auto read enable. */
    base->READ_CTRL = OCOTP_READ_CTRL_READ_FUSE_MASK;

    /* Wait for busy bit is cleared, and no error occurred on controller. */
    while (OCOTP_CheckBusyStatus(base))
    {
    }

    /* If ERROR bit was set, this may be mean that the accsee to the register was wrong. */
    if (OCOTP_CheckErrorStatus(base))
    {
        /* Clear access error status bit. */
        OCOTP_ClearErrorStatus(base);
    }

    /* Read the fuse data. */
    return (base->READ_FUSE_DATA);
}

/* Write the fuse shadow register. */
status_t OCOTP_WriteFuseShadowRegister(OCOTP_Type *base, uint32_t address, uint32_t data)
{
    assert(NULL != base);

    /* Make sure the OCOTP is ready, Overlapped accesses are not supported by the controller. */
    while (OCOTP_CheckBusyStatus(base))
    {
    }

    /* Clear access error status bit. */
    if (OCOTP_CheckErrorStatus(base))
    {
        OCOTP_ClearErrorStatus(base);
    }

    /* Set write timing for OCOTP controller. */
    OCOTP_SetWriteTiming(base, s_timingConfig);

    /* Wait for busy bit is cleared. */
    while (OCOTP_CheckBusyStatus(base))
    {
    }

    /* Clear access error status bit. */
    if (OCOTP_CheckErrorStatus(base))
    {
        OCOTP_ClearErrorStatus(base);
    }

    /* Write requested address and unlock key to register. */
    base->CTRL_CLR = OCOTP_CTRL_CLR_ADDR_MASK | OCOTP_CTRL_WR_UNLOCK_MASK;
    base->CTRL_SET = OCOTP_CTRL_SET_ADDR(address) | OCOTP_CTRL_WR_UNLOCK(OCOTP_WRITE_UNLOCK_KEY);

    /* Write data to register. */
    base->DATA = data;

    /* Wait for busy bit is cleared, and no error occurred on controller. */
    while (OCOTP_CheckBusyStatus(base))
    {
    }

    /* If ERROR bit was set, this may be mean that the accsee to the register was wrong. */
    if (OCOTP_CheckErrorStatus(base))
    {
        /* Clear access error status bit. */
        OCOTP_ClearErrorStatus(base);

        return kStatus_OCOTP_AccessError;
    }

    /* Reload the fuse register. */
    OCOTP_ReloadShadowRegister(base);

    return kStatus_Success;
}
