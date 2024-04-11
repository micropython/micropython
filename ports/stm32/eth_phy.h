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

#ifndef MICROPY_INCLUDED_STM32_PHY_H
#define MICROPY_INCLUDED_STM32_PYH_H

#if defined(MICROPY_HW_ETH_MDC)

// Common ETH PHY register definitions
#undef PHY_BCR
#define PHY_BCR                 (0x0000)
#define PHY_BCR_SOFT_RESET      (0x8000)
#define PHY_BCR_AUTONEG_EN      (0x1000)
#define PHY_BCR_POWER_DOWN      (0x0800U)

#undef PHY_BSR
#define PHY_BSR                 (0x0001)
#define PHY_BSR_LINK_STATUS     (0x0004)
#define PHY_BSR_AUTONEG_DONE    (0x0020)

#undef PHY_ANAR
#define PHY_ANAR                (0x0004)
#define PHY_ANAR_SPEED_10HALF   (0x0020)
#define PHY_ANAR_SPEED_10FULL   (0x0040)
#define PHY_ANAR_SPEED_100HALF  (0x0080)
#define PHY_ANAR_SPEED_100FULL  (0x0100)
#define PHY_ANAR_IEEE802_3      (0x0001)

#define PHY_SPEED_10HALF   (1)
#define PHY_SPEED_10FULL   (5)
#define PHY_SPEED_100HALF  (2)
#define PHY_SPEED_100FULL  (6)
#define PHY_DUPLEX         (4)

uint32_t eth_phy_read(uint32_t phy_addr, uint32_t reg);
void eth_phy_write(uint32_t phy_addr, uint32_t reg, uint32_t val);

int16_t eth_phy_lan87xx_get_link_status(uint32_t phy_addr);
int16_t eth_phy_dp838xx_get_link_status(uint32_t phy_addr);

#endif

#endif  // MICROPY_INCLUDED_STM32_PHY_H
