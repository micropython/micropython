/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2024 Robert Hammelrath
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

#include "py/mphal.h"
#include "eth_phy.h"

#if defined(MICROPY_HW_ETH_MDC)

#define PHY_SCSR_LAN87XX                (0x001f)
#define PHY_SCSR_LAN87XX_10M_Msk        (0x0004)
#define PHY_SCSR_LAN87XX_100M_Msk       (0x0008)
#define PHY_SCSR_LAN87XX_DUPLEX_Msk     (0x0010)

#define PHY_SCSR_DP838XX                (0x0010)
#define PHY_RECR_DP838XX                (0x0015)
#define PHY_SCSR_DP838XX_DUPLEX_Msk     (4)
#define PHY_SCSR_DP838XX_10M_Msk        (2)

#define PHY_RTL8211_DEFAULT_PAGE        (0xa42)
#define PHY_RTL8211_PAGSR_ADDR          (0x1f)
#define PHY_RTL8211_PHYSR_PAGE          (0xa43)
#define PHY_RTL8211_PHYSR_ADDR          (0x1a)
#define PHY_RTL8211_PHYSR_SPEED_Pos     (4)
#define PHY_RTL8211_PHYSR_SPEED_Msk     (3 << PHY_RTL8211_PHYSR_SPEED_Pos)
#define PHY_RTL8211_PHYSR_DUPLEX_Msk    (0x0008)
#define PHY_RTL8211_LCR_PAGE            (0xd04)
#define PHY_RTL8211_LCR_ADDR            (0x10)

void eth_phy_generic_init(uint32_t phy_addr) {
    // Reset the PHY.
    eth_phy_write(phy_addr, PHY_BCR, PHY_BCR_SOFT_RESET);
    mp_hal_delay_ms(50);
}

int16_t eth_phy_lan87xx_get_link_status(uint32_t phy_addr) {
    // Get the link mode & speed
    uint16_t scsr = eth_phy_read(phy_addr, PHY_SCSR_LAN87XX);
    int16_t status = 0;
    if (scsr & PHY_SCSR_LAN87XX_10M_Msk) {
        status |= PHY_SPEED_10HALF;
    }
    if (scsr & PHY_SCSR_LAN87XX_100M_Msk) {
        status |= PHY_SPEED_100HALF;
    }
    if (scsr & PHY_SCSR_LAN87XX_DUPLEX_Msk) {
        status |= PHY_DUPLEX;
    }
    return status;
}

int16_t eth_phy_dp838xx_get_link_status(uint32_t phy_addr) {
    int16_t scsr = 0;
    // Get the link mode & speed
    uint16_t temp = eth_phy_read(phy_addr, PHY_SCSR_DP838XX);
    scsr = (temp & PHY_SCSR_DP838XX_10M_Msk) ? PHY_SPEED_10HALF : PHY_SPEED_100HALF;
    if (temp & PHY_SCSR_DP838XX_DUPLEX_Msk) {
        scsr |= PHY_DUPLEX;
    }
    return scsr;
}

void eth_phy_rtl8211_init(uint32_t phy_addr) {
    // Perform generic PHY initialization.
    eth_phy_generic_init(phy_addr);

    // Configure LED0 output to show 10/100/1000 link speed, and activity.
    eth_phy_write(phy_addr, PHY_RTL8211_PAGSR_ADDR, PHY_RTL8211_LCR_PAGE);
    eth_phy_write(phy_addr, PHY_RTL8211_LCR_ADDR, 0x001b);
    eth_phy_write(phy_addr, PHY_RTL8211_PAGSR_ADDR, PHY_RTL8211_DEFAULT_PAGE);
}

int16_t eth_phy_rtl8211_get_link_status(uint32_t phy_addr) {
    // Get the link mode & speed
    eth_phy_write(phy_addr, PHY_RTL8211_PAGSR_ADDR, PHY_RTL8211_PHYSR_PAGE);
    int16_t physr = eth_phy_read(phy_addr, PHY_RTL8211_PHYSR_ADDR);
    eth_phy_write(phy_addr, PHY_RTL8211_PAGSR_ADDR, PHY_RTL8211_DEFAULT_PAGE);
    int16_t status = 0;
    switch ((physr & PHY_RTL8211_PHYSR_SPEED_Msk) >> PHY_RTL8211_PHYSR_SPEED_Pos) {
        case 0:
            status |= PHY_SPEED_10HALF;
            break;
        case 1:
            status |= PHY_SPEED_100HALF;
            break;
        case 2:
            status |= PHY_SPEED_1000HALF;
            break;
    }
    if (physr & PHY_RTL8211_PHYSR_DUPLEX_Msk) {
        status |= PHY_DUPLEX;
    }
    return status;
}

#endif
