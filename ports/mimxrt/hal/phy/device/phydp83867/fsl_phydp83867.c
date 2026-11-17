/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Andrew Leech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "fsl_phydp83867.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Defines the PHY DP83867 vendor defined registers. */
#define PHY_PHYSTS_REG          0x11U /*!< The PHY Status register. */

/*! @brief Defines the PHY DP83867 ID number. */
#define PHY_CONTROL_ID1         0x2000U /*!< The PHY ID1 (upper 16 bits). */
#define PHY_CONTROL_ID2         0xA231U /*!< The PHY ID2 (lower 16 bits). */
#define PHY_FULL_ID             0x2000A231U /*!< Full PHY ID. */

/*! @brief Defines the mask flag in PHYSTS register. */
#define PHY_PHYSTS_LINKSTATUS_MASK  0x0400U /*!< The PHY link status mask. */
#define PHY_PHYSTS_LINKSPEED_MASK   0xC000U /*!< The PHY link speed mask. */
#define PHY_PHYSTS_LINKDUPLEX_MASK  0x2000U /*!< The PHY link duplex mask. */
#define PHY_PHYSTS_LINKSPEED_SHIFT  14U     /*!< The link speed shift */

/*! @brief Link speed values from PHYSTS register. */
#define PHY_PHYSTS_LINKSPEED_10M    0U /*!< 10M link speed. */
#define PHY_PHYSTS_LINKSPEED_100M   1U /*!< 100M link speed. */
#define PHY_PHYSTS_LINKSPEED_1000M  2U /*!< 1000M link speed. */

/*! @brief Defines the timeout macro. */
#define PHY_READID_TIMEOUT_COUNT    1000U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

const phy_operations_t phydp83867_ops = {.phyInit = PHY_DP83867_Init,
                                         .phyWrite = PHY_DP83867_Write,
                                         .phyRead = PHY_DP83867_Read,
                                         .getAutoNegoStatus = PHY_DP83867_GetAutoNegotiationStatus,
                                         .getLinkStatus = PHY_DP83867_GetLinkStatus,
                                         .getLinkSpeedDuplex = PHY_DP83867_GetLinkSpeedDuplex,
                                         .setLinkSpeedDuplex = PHY_DP83867_SetLinkSpeedDuplex,
                                         .enableLoopback = PHY_DP83867_EnableLoopback};

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t PHY_DP83867_Init(phy_handle_t *handle, const phy_config_t *config) {
    uint32_t counter = PHY_READID_TIMEOUT_COUNT;
    status_t result;
    uint32_t regValue = 0U;


    /* Init MDIO interface. */
    MDIO_Init(handle->mdioHandle);

    /* Assign phy address. */
    handle->phyAddr = config->phyAddr;


    /* Check PHY ID. */
    do
    {
        result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_ID1_REG, &regValue);
        if (result != kStatus_Success) {
            return result;
        }
        counter--;
    } while ((regValue != PHY_CONTROL_ID1) && (counter != 0U));

    if (counter == 0U) {
        return kStatus_Fail;
    }


    /* Reset PHY. */
    result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, PHY_BCTL_RESET_MASK);
    if (result != kStatus_Success) {
        return result;
    }


    /* Wait for reset to complete */
    counter = PHY_READID_TIMEOUT_COUNT;
    do {
        result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, &regValue);
        if (result != kStatus_Success) {
            return result;
        }
        counter--;
    } while ((regValue & PHY_BCTL_RESET_MASK) && (counter != 0U));

    if (counter == 0U) {
        return kStatus_Fail;
    }


    if (config->autoNeg) {
        /* Set the auto-negotiation. */
        result =
            MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_AUTONEG_ADVERTISE_REG,
                PHY_100BASETX_FULLDUPLEX_MASK | PHY_100BASETX_HALFDUPLEX_MASK | PHY_10BASETX_FULLDUPLEX_MASK |
                PHY_10BASETX_HALFDUPLEX_MASK | PHY_IEEE802_3_SELECTOR_MASK);
        if (result == kStatus_Success) {
            result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_1000BASET_CONTROL_REG,
                PHY_1000BASET_FULLDUPLEX_MASK);
            if (result == kStatus_Success) {
                result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, &regValue);
                if (result == kStatus_Success) {
                    result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG,
                        (regValue | PHY_BCTL_AUTONEG_MASK | PHY_BCTL_RESTART_AUTONEG_MASK));
                }
            }
        }
    } else {
        /* Disable isolate mode */
        result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, &regValue);
        if (result != kStatus_Success) {
            return result;
        }
        regValue &= ~PHY_BCTL_ISOLATE_MASK;
        result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, regValue);
        if (result != kStatus_Success) {
            return result;
        }

        /* Disable the auto-negotiation and set user-defined speed/duplex configuration. */
        result = PHY_DP83867_SetLinkSpeedDuplex(handle, config->speed, config->duplex);
    }


    return result;
}

status_t PHY_DP83867_Write(phy_handle_t *handle, uint32_t phyReg, uint32_t data) {
    return MDIO_Write(handle->mdioHandle, handle->phyAddr, phyReg, data);
}

status_t PHY_DP83867_Read(phy_handle_t *handle, uint32_t phyReg, uint32_t *dataPtr) {
    return MDIO_Read(handle->mdioHandle, handle->phyAddr, phyReg, dataPtr);
}

status_t PHY_DP83867_GetAutoNegotiationStatus(phy_handle_t *handle, bool *status) {
    assert(status);

    status_t result;
    uint32_t regValue;

    *status = false;

    /* Check auto negotiation complete. */
    result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICSTATUS_REG, &regValue);
    if (result == kStatus_Success) {
        if ((regValue & PHY_BSTATUS_AUTONEGCOMP_MASK) != 0U) {
            *status = true;
        }
    }
    return result;
}

status_t PHY_DP83867_GetLinkStatus(phy_handle_t *handle, bool *status) {
    assert(status);

    status_t result;
    uint32_t regValue;

    /* Read the PHY Status register. */
    result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_PHYSTS_REG, &regValue);
    if (result == kStatus_Success) {
        if ((PHY_PHYSTS_LINKSTATUS_MASK & regValue) != 0U) {
            /* Link up. */
            *status = true;
        } else {
            /* Link down. */
            *status = false;
        }
    }
    return result;
}

status_t PHY_DP83867_GetLinkSpeedDuplex(phy_handle_t *handle, phy_speed_t *speed, phy_duplex_t *duplex) {
    assert(!((speed == NULL) && (duplex == NULL)));

    status_t result;
    uint32_t regValue;

    /* Read the status register. */
    result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_PHYSTS_REG, &regValue);
    if (result == kStatus_Success) {
        if (speed != NULL) {
            switch ((regValue & PHY_PHYSTS_LINKSPEED_MASK) >> PHY_PHYSTS_LINKSPEED_SHIFT)
            {
                case PHY_PHYSTS_LINKSPEED_10M:
                    *speed = kPHY_Speed10M;
                    break;
                case PHY_PHYSTS_LINKSPEED_100M:
                    *speed = kPHY_Speed100M;
                    break;
                case PHY_PHYSTS_LINKSPEED_1000M:
                    *speed = kPHY_Speed1000M;
                    break;
                default:
                    *speed = kPHY_Speed10M;
                    break;
            }
        }

        if (duplex != NULL) {
            if ((regValue & PHY_PHYSTS_LINKDUPLEX_MASK) != 0U) {
                *duplex = kPHY_FullDuplex;
            } else {
                *duplex = kPHY_HalfDuplex;
            }
        }
    }
    return result;
}

status_t PHY_DP83867_SetLinkSpeedDuplex(phy_handle_t *handle, phy_speed_t speed, phy_duplex_t duplex) {
    status_t result;
    uint32_t regValue;

    result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, &regValue);
    if (result == kStatus_Success) {
        /* Disable the auto-negotiation and set according to user-defined configuration. */
        regValue &= ~PHY_BCTL_AUTONEG_MASK;
        if (speed == kPHY_Speed1000M) {
            regValue &= ~PHY_BCTL_SPEED0_MASK;
            regValue |= PHY_BCTL_SPEED1_MASK;
        } else if (speed == kPHY_Speed100M) {
            regValue |= PHY_BCTL_SPEED0_MASK;
            regValue &= ~PHY_BCTL_SPEED1_MASK;
        } else {
            regValue &= ~PHY_BCTL_SPEED0_MASK;
            regValue &= ~PHY_BCTL_SPEED1_MASK;
        }
        if (duplex == kPHY_FullDuplex) {
            regValue |= PHY_BCTL_DUPLEX_MASK;
        } else {
            regValue &= ~PHY_BCTL_DUPLEX_MASK;
        }
        result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, regValue);
    }
    return result;
}

status_t PHY_DP83867_EnableLoopback(phy_handle_t *handle, phy_loop_t mode, phy_speed_t speed, bool enable) {
    /* This PHY only supports local loopback. */
    assert(mode == kPHY_LocalLoop);

    status_t result;
    uint32_t regValue;

    /* Set the loop mode. */
    if (enable) {
        if (speed == kPHY_Speed1000M) {
            regValue = PHY_BCTL_SPEED1_MASK | PHY_BCTL_DUPLEX_MASK | PHY_BCTL_LOOP_MASK;
        } else if (speed == kPHY_Speed100M) {
            regValue = PHY_BCTL_SPEED0_MASK | PHY_BCTL_DUPLEX_MASK | PHY_BCTL_LOOP_MASK;
        } else {
            regValue = PHY_BCTL_DUPLEX_MASK | PHY_BCTL_LOOP_MASK;
        }
        result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, regValue);
    } else {
        /* First read the current status in control register. */
        result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, &regValue);
        if (result == kStatus_Success) {
            regValue &= ~PHY_BCTL_LOOP_MASK;
            result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG,
                (regValue | PHY_BCTL_RESTART_AUTONEG_MASK));
        }
    }
    return result;
}
