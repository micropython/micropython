/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_pmu.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.pmu"
#endif

/*!
 * name Status.
 * {
 */

uint32_t PMU_GetStatusFlags(PMU_Type *base)
{
    uint32_t ret = 0U;

    /* For 1P1. */
    if (PMU_REG_1P1_OK_VDD1P1_MASK == (PMU_REG_1P1_OK_VDD1P1_MASK & base->REG_1P1))
    {
        ret |= kPMU_1P1RegulatorOutputOK;
    }
    if (PMU_REG_1P1_BO_VDD1P1_MASK == (PMU_REG_1P1_BO_VDD1P1_MASK & base->REG_1P1))
    {
        ret |= kPMU_1P1BrownoutOnOutput;
    }

    /* For 3P0. */
    if (PMU_REG_3P0_OK_VDD3P0_MASK == (PMU_REG_3P0_OK_VDD3P0_MASK & base->REG_3P0))
    {
        ret |= kPMU_3P0RegulatorOutputOK;
    }
    if (PMU_REG_3P0_BO_VDD3P0_MASK == (PMU_REG_3P0_BO_VDD3P0_MASK & base->REG_3P0))
    {
        ret |= kPMU_3P0BrownoutOnOutput;
    }

    /* For 2P5. */
    if (PMU_REG_2P5_OK_VDD2P5_MASK == (PMU_REG_2P5_OK_VDD2P5_MASK & base->REG_2P5))
    {
        ret |= kPMU_2P5RegulatorOutputOK;
    }
    if (PMU_REG_2P5_BO_VDD2P5_MASK == (PMU_REG_2P5_BO_VDD2P5_MASK & base->REG_2P5))
    {
        ret |= kPMU_2P5BrownoutOnOutput;
    }

    return ret;
}
