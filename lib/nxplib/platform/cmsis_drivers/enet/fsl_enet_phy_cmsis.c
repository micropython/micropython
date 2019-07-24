/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 * Copyright (c) 2016, Freescale Semiconductor, Inc. Not a Contribution.
 * Copyright 2016-2017 NXP. Not a Contribution.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fsl_enet_phy_cmsis.h"
#include "fsl_phy.h"

#define ARM_ETH_PHY_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

typedef const struct _cmsis_enet_phy_resource
{
    ENET_Type *base;           /*!< ENET peripheral base address. */
    uint32_t (*GetFreq)(void); /*!< Function to get frequency. */
    uint32_t phyAddr;          /*!< ENET PHY physical address. */
} cmsis_enet_phy_resource_t;

typedef struct _cmsis_enet_phy_state
{
    cmsis_enet_phy_resource_t *resource; /*!< Basic enet resource. */
    uint32_t cmsis_enet_phy_mode;        /*!< ENET MII interface work mode. */
} cmsis_enet_phy_state_t;

static const ARM_DRIVER_VERSION s_phyDriverVersion = {ARM_ETH_PHY_API_VERSION, ARM_ETH_PHY_DRV_VERSION};
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to enet clocks for each instance. */
extern clock_ip_name_t s_enetClock[FSL_FEATURE_SOC_ENET_COUNT];
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

extern uint32_t ENET_GetInstance(ENET_Type *base);

static ARM_DRIVER_VERSION PHYx_GetVersion(void)
{
    return s_phyDriverVersion;
}

static int32_t PHY_SetForcedSpeedDuplexMode(cmsis_enet_phy_state_t *ethPhy, uint32_t mode)
{
    int32_t result;
    uint32_t bsctlReg;
    uint32_t freq;

/* This API is used to set forced speed duplex mode. */
/* Set SMI first. */
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_EnableClock(s_enetClock[ENET_GetInstance(ethPhy->resource->base)]);
#endif
    freq = ethPhy->resource->GetFreq();
    ENET_SetSMI(ethPhy->resource->base, freq, false);

    /* Reset PHY. */
    result = PHY_Write(ethPhy->resource->base, ethPhy->resource->phyAddr, PHY_BASICCONTROL_REG, PHY_BCTL_RESET_MASK);
    if (result == kStatus_Success)
    {
        /* Set with the forced speed and duplex. */
        result = PHY_Read(ethPhy->resource->base, ethPhy->resource->phyAddr, PHY_BASICCONTROL_REG, &bsctlReg);
        if (result == kStatus_Success)
        {
            /* Build the control value. */
            bsctlReg &= ~(PHY_BCTL_DUPLEX_MASK | PHY_BCTL_SPEED_MASK | PHY_BCTL_AUTONEG_MASK);
            if (mode == ARM_ETH_PHY_DUPLEX_FULL)
            {
                bsctlReg |= PHY_BCTL_DUPLEX_MASK;
            }
            if (mode == ARM_ETH_PHY_SPEED_100M)
            {
                bsctlReg |= PHY_BCTL_SPEED_MASK;
            }
            result = PHY_Write(ethPhy->resource->base, ethPhy->resource->phyAddr, PHY_BASICCONTROL_REG, bsctlReg);
        }
    }

    return result;
}

static void PHY_SetPowerDown(cmsis_enet_phy_resource_t *enet, bool down)
{
    uint32_t instance = ENET_GetInstance(enet->base);
    uint32_t data;
    uint32_t freq;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Set SMI first. */
    CLOCK_EnableClock(s_enetClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    freq = enet->GetFreq();
    ENET_SetSMI(enet->base, freq, false);

    if (down)
    {
        PHY_Read(enet->base, enet->phyAddr, PHY_BASICCONTROL_REG, &data);
        data |= (1U << 11);
        PHY_Write(enet->base, enet->phyAddr, PHY_BASICCONTROL_REG, data);
    }
    else
    {
        PHY_Read(enet->base, enet->phyAddr, PHY_BASICCONTROL_REG, &data);
        data &= ~(1U << 11);
        PHY_Write(enet->base, enet->phyAddr, PHY_BASICCONTROL_REG, data);
        PHY_Read(enet->base, enet->phyAddr, PHY_BASICCONTROL_REG, &data);
    }
}

#if (defined(ENET) && RTE_ENET) || (defined(ENET0) && RTE_ENET0)

/* User needs to provide the implementation for ENET_GetFreq
in the application for enabling according instance. */
extern uint32_t ENET0_GetFreq(void);
#if defined(ENET)
cmsis_enet_phy_resource_t ENETPHY0_Resource = {ENET, ENET0_GetFreq, RTE_ENET_PHY_ADDRESS};
#else
cmsis_enet_phy_resource_t ENETPHY0_Resource = {ENET0, ENET0_GetFreq, RTE_ENET_PHY_ADDRESS};
#endif
cmsis_enet_phy_state_t ENETPHY0_State = {&ENETPHY0_Resource, ARM_ETH_PHY_AUTO_NEGOTIATE};

int32_t PHY0_Initialize(ARM_ETH_PHY_Read_t fn_read, ARM_ETH_PHY_Write_t fn_write)
{
    return ARM_DRIVER_OK;
}

int32_t PHY0_UnInitialize(void)
{
    return ARM_DRIVER_OK;
}

int32_t PHY0_PowerControl(ARM_POWER_STATE state)
{
    int32_t status;

    switch (state)
    {
        case ARM_POWER_FULL:
        {
            status = PHY_Init(ENETPHY0_State.resource->base, ENETPHY0_State.resource->phyAddr,
                              ENETPHY0_State.resource->GetFreq());
            if ((status == kStatus_Success) || (status == kStatus_PHY_AutoNegotiateFail))
            {
                status = ARM_DRIVER_OK;
            }
            else
            {
                status = ARM_DRIVER_ERROR;
            }
        }
        break;
        case ARM_POWER_OFF:
        {
            PHY_SetPowerDown(ENETPHY0_State.resource, true);
            status = ARM_DRIVER_OK;
        }
        break;
        default:
            status = ARM_DRIVER_ERROR_UNSUPPORTED;
            break;
    }
    return status;
}

int32_t PHY0_SetInterface(uint32_t interface)
{
    /* The interface for PHY is fixed or controlled by JUMPER setting.
     * The interface setting for MAC is defined  by "RTE_ENET_RMII/RTE_ENET_MII"
     */
    if (interface == ARM_ETH_INTERFACE_SMII)
    {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    else
    {
        return ARM_DRIVER_OK;
    }
}

int32_t PHY0_SetMode(uint32_t mode)
{
    int32_t status = ARM_DRIVER_OK;

    /*!< Check input mode. */
    if ((mode == ARM_ETH_PHY_SPEED_1G) || (mode == ARM_ETH_PHY_ISOLATE))
    {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    if (mode == ARM_ETH_PHY_LOOPBACK)
    {
        status = PHY_EnableLoopback(ENETPHY0_State.resource->base, ENETPHY0_State.resource->phyAddr, kPHY_RemoteLoop,
                                    kPHY_Speed100M, true);
    }
    else if (mode == ARM_ETH_PHY_AUTO_NEGOTIATE)
    {
        status = PHY_Init(ENETPHY0_State.resource->base, ENETPHY0_State.resource->phyAddr,
                          ENETPHY0_State.resource->GetFreq());
    }
    else
    {
        status = PHY_SetForcedSpeedDuplexMode(&ENETPHY0_State, mode);
    }
    ENETPHY0_State.cmsis_enet_phy_mode = mode;

    return status;
}

ARM_ETH_LINK_STATE PHY0_GetLinkState(void)
{
    ARM_ETH_LINK_STATE state;
    bool linkUp = false;
    int32_t status;

    status = PHY_GetLinkStatus(ENETPHY0_Resource.base, ENETPHY0_Resource.phyAddr, &linkUp);
    if ((status == kStatus_Success) && linkUp)
    {
        state = ARM_ETH_LINK_UP;
    }
    else
    {
        state = ARM_ETH_LINK_DOWN;
    }
    return state;
}

ARM_ETH_LINK_INFO PHY0_GetLinkInfo(void)
{
    ARM_ETH_LINK_INFO linkInfo = {0};
    phy_speed_t speed;
    phy_duplex_t duplex;
    int32_t status;

    status = PHY_GetLinkSpeedDuplex(ENETPHY0_Resource.base, ENETPHY0_Resource.phyAddr, &speed, &duplex);
    if (status == kStatus_Success)
    {
        if (speed == kPHY_Speed10M)
        {
            linkInfo.speed = ARM_ETH_SPEED_10M;
        }
        else if (speed == kPHY_Speed100M)
        {
            linkInfo.speed = ARM_ETH_SPEED_100M;
        }
        else
        {
            linkInfo.speed = ARM_ETH_SPEED_1G;
        }

        if (duplex == kPHY_FullDuplex)
        {
            linkInfo.duplex = ARM_ETH_DUPLEX_FULL;
        }
        else
        {
            linkInfo.duplex = ARM_ETH_DUPLEX_HALF;
        }
    }
    return linkInfo;
}

ARM_DRIVER_ETH_PHY Driver_ETH_PHY0 = {PHYx_GetVersion,   PHY0_Initialize, PHY0_UnInitialize, PHY0_PowerControl,
                                      PHY0_SetInterface, PHY0_SetMode,    PHY0_GetLinkState, PHY0_GetLinkInfo};
#endif
