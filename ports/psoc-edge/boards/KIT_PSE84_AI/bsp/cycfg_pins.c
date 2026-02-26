/*******************************************************************************
 * File Name: cycfg_pins.c
 *
 * Description:
 * Pin configuration
 * This file was automatically generated and should not be modified.
 * Configurator Backend 3.60.0
 * device-db 4.34.0.9502
 * mtb-dsl-pse8xxgp 1.2.0.895
 *
 *******************************************************************************
 * Copyright 2026 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#include "cycfg_pins.h"

const cy_stc_gpio_pin_config_t CYBSP_OSPI_RAM_CS_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_OSPI_RAM_CS_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_QSPI_D0_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_QSPI_D0_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_QSPI_D1_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_QSPI_D1_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_QSPI_D2_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_QSPI_D2_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_QSPI_D3_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_QSPI_D3_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_QSPI_CS_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_QSPI_CS_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I3C_SCL_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ,
    .hsiom = CYBSP_I3C_SCL_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I3C_SDA_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_CFGOUT3_STRONG_PULLUP_HIGHZ,
    .hsiom = CYBSP_I3C_SDA_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_OSPI_RAM_D0_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_OSPI_RAM_D0_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_OSPI_RAM_D1_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_OSPI_RAM_D1_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_OSPI_RAM_D2_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_OSPI_RAM_D2_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_OSPI_RAM_D3_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_OSPI_RAM_D3_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_OSPI_RAM_D4_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_OSPI_RAM_D4_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_OSPI_RAM_D5_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_OSPI_RAM_D5_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_OSPI_RAM_D6_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_OSPI_RAM_D6_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_OSPI_RAM_D7_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_OSPI_RAM_D7_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DEBUG_UART_RX_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DEBUG_UART_RX_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DEBUG_UART_TX_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_DEBUG_UART_TX_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_SW1_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_PULLUP,
    .hsiom = CYBSP_SW1_HSIOM,
    .intEdge = CY_GPIO_INTR_FALLING,
    .intMask = 1UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_EXT_CLK_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_ANALOG,
    .hsiom = CYBSP_EXT_CLK_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I2C_SCL_1V8_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_OD_DRIVESLOW,
    .hsiom = CYBSP_I2C_SCL_1V8_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I2C_SDA_1V8_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_OD_DRIVESLOW,
    .hsiom = CYBSP_I2C_SDA_1V8_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_PDM_CLK_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_PDM_CLK_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_PDM_DATA_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_PDM_DATA_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_SERIAL_INT_0_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_SERIAL_INT_0_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_SERIAL_INT_2_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_SERIAL_INT_2_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_BT_UART_RX_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_BT_UART_RX_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_BT_UART_TX_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_BT_UART_TX_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_BT_UART_CTS_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_BT_UART_CTS_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_BT_UART_RTS_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_BT_UART_RTS_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_BT_HOST_WAKE_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_BT_HOST_WAKE_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_USER_LED2_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_USER_LED2_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_BT_DEVICE_WAKE_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_BT_DEVICE_WAKE_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_USER_LED1_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_USER_LED1_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_BT_POWER_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_PULLUP,
    .hsiom = CYBSP_BT_POWER_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_PCLK_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_PCLK_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WIFI_WL_REG_ON_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_OD_DRIVESLOW,
    .hsiom = CYBSP_WIFI_WL_REG_ON_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WIFI_HOST_WAKE_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_OD_DRIVESLOW,
    .hsiom = CYBSP_WIFI_HOST_WAKE_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_CLK_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_WIFI_SDIO_CLK_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_D0_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_WIFI_SDIO_D0_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_D1_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_WIFI_SDIO_D1_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I2S_TX_FSYNC_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_I2S_TX_FSYNC_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_D2_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_WIFI_SDIO_D2_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_D3_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_WIFI_SDIO_D3_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_D0_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_D0_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_D1_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_D1_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_D2_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_D2_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_D3_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_D3_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_D4_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_D4_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_D5_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_D5_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_D6_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_D6_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_D7_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_D7_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I2C_SCL_3V3_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_OD_DRIVESLOW,
    .hsiom = CYBSP_I2C_SCL_3V3_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I2C_SDA_3V3_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_OD_DRIVESLOW,
    .hsiom = CYBSP_I2C_SDA_3V3_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_VSYNC_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_VSYNC_HSIOM,
    .intEdge = CY_GPIO_INTR_FALLING,
    .intMask = 1UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_HREF_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_DVP_CAM_HREF_HSIOM,
    .intEdge = CY_GPIO_INTR_FALLING,
    .intMask = 1UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_XCLK_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_DVP_CAM_XCLK_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_RESET_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_DVP_CAM_RESET_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_DVP_CAM_PWDN_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_DVP_CAM_PWDN_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WCO_OUT_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_ANALOG,
    .hsiom = CYBSP_WCO_OUT_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WCO_IN_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_ANALOG,
    .hsiom = CYBSP_WCO_IN_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_ECO_IN_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_ANALOG,
    .hsiom = CYBSP_ECO_IN_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_ECO_OUT_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_ANALOG,
    .hsiom = CYBSP_ECO_OUT_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_RADAR_INT_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_PULLDOWN_IN_OFF,
    .hsiom = CYBSP_RADAR_INT_HSIOM,
    .intEdge = CY_GPIO_INTR_RISING,
    .intMask = 1UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_LED_RGB_GREEN_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_LED_RGB_GREEN_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_LED_RGB_BLUE_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_LED_RGB_BLUE_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_LED_RGB_RED_config =
{
    .outVal = 0,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_LED_RGB_RED_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_RXRES_L_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_RXRES_L_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_WIFI_SDIO_CMD_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG,
    .hsiom = CYBSP_WIFI_SDIO_CMD_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I2S_TX_SD_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_I2S_TX_SD_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I2S_TX_SCK_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_I2S_TX_SCK_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_I2S_TX_MCK_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_I2S_TX_MCK_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_RSPI_MISO_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = CYBSP_RSPI_MISO_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_RSPI_MOSI_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_RSPI_MOSI_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_RSPI_CLK_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_RSPI_CLK_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};
const cy_stc_gpio_pin_config_t CYBSP_RSPI_CS_config =
{
    .outVal = 1,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom = CYBSP_RSPI_CS_HSIOM,
    .intEdge = CY_GPIO_INTR_DISABLE,
    .intMask = 0UL,
    .vtrip = CY_GPIO_VTRIP_CMOS,
    .slewRate = CY_GPIO_SLEW_FAST,
    .driveSel = CY_GPIO_DRIVE_1_2,
    .vregEn = 0UL,
    .ibufMode = 0UL,
    .vtripSel = 0UL,
    .vrefSel = 0UL,
    .vohSel = 0UL,
    .pullUpRes = CY_GPIO_PULLUP_RES_DISABLE,
    .nonSec = 1,
};

void init_cycfg_pins(void)
{
    Cy_GPIO_Pin_Init(CYBSP_OSPI_RAM_CS_PORT, CYBSP_OSPI_RAM_CS_PIN, &CYBSP_OSPI_RAM_CS_config);
    Cy_GPIO_Pin_Init(CYBSP_QSPI_D0_PORT, CYBSP_QSPI_D0_PIN, &CYBSP_QSPI_D0_config);
    Cy_GPIO_Pin_Init(CYBSP_QSPI_D1_PORT, CYBSP_QSPI_D1_PIN, &CYBSP_QSPI_D1_config);
    Cy_GPIO_Pin_Init(CYBSP_QSPI_D2_PORT, CYBSP_QSPI_D2_PIN, &CYBSP_QSPI_D2_config);
    Cy_GPIO_Pin_Init(CYBSP_QSPI_D3_PORT, CYBSP_QSPI_D3_PIN, &CYBSP_QSPI_D3_config);
    Cy_GPIO_Pin_Init(CYBSP_QSPI_CS_PORT, CYBSP_QSPI_CS_PIN, &CYBSP_QSPI_CS_config);
    Cy_GPIO_Pin_Init(CYBSP_I3C_SCL_PORT, CYBSP_I3C_SCL_PIN, &CYBSP_I3C_SCL_config);
    Cy_GPIO_Pin_Init(CYBSP_I3C_SDA_PORT, CYBSP_I3C_SDA_PIN, &CYBSP_I3C_SDA_config);
    Cy_GPIO_Pin_Init(CYBSP_OSPI_RAM_D0_PORT, CYBSP_OSPI_RAM_D0_PIN, &CYBSP_OSPI_RAM_D0_config);
    Cy_GPIO_Pin_Init(CYBSP_OSPI_RAM_D1_PORT, CYBSP_OSPI_RAM_D1_PIN, &CYBSP_OSPI_RAM_D1_config);
    Cy_GPIO_Pin_Init(CYBSP_OSPI_RAM_D2_PORT, CYBSP_OSPI_RAM_D2_PIN, &CYBSP_OSPI_RAM_D2_config);
    Cy_GPIO_Pin_Init(CYBSP_OSPI_RAM_D3_PORT, CYBSP_OSPI_RAM_D3_PIN, &CYBSP_OSPI_RAM_D3_config);
    Cy_GPIO_Pin_Init(CYBSP_OSPI_RAM_D4_PORT, CYBSP_OSPI_RAM_D4_PIN, &CYBSP_OSPI_RAM_D4_config);
    Cy_GPIO_Pin_Init(CYBSP_OSPI_RAM_D5_PORT, CYBSP_OSPI_RAM_D5_PIN, &CYBSP_OSPI_RAM_D5_config);
    Cy_GPIO_Pin_Init(CYBSP_OSPI_RAM_D6_PORT, CYBSP_OSPI_RAM_D6_PIN, &CYBSP_OSPI_RAM_D6_config);
    Cy_GPIO_Pin_Init(CYBSP_OSPI_RAM_D7_PORT, CYBSP_OSPI_RAM_D7_PIN, &CYBSP_OSPI_RAM_D7_config);
    Cy_GPIO_Pin_Init(CYBSP_DEBUG_UART_RX_PORT, CYBSP_DEBUG_UART_RX_PIN, &CYBSP_DEBUG_UART_RX_config);
    Cy_GPIO_Pin_Init(CYBSP_DEBUG_UART_TX_PORT, CYBSP_DEBUG_UART_TX_PIN, &CYBSP_DEBUG_UART_TX_config);
    Cy_GPIO_Pin_Init(CYBSP_SW1_PORT, CYBSP_SW1_PIN, &CYBSP_SW1_config);
    Cy_GPIO_Pin_Init(CYBSP_I2C_SCL_1V8_PORT, CYBSP_I2C_SCL_1V8_PIN, &CYBSP_I2C_SCL_1V8_config);
    Cy_GPIO_Pin_Init(CYBSP_I2C_SDA_1V8_PORT, CYBSP_I2C_SDA_1V8_PIN, &CYBSP_I2C_SDA_1V8_config);
    Cy_GPIO_Pin_Init(CYBSP_PDM_CLK_PORT, CYBSP_PDM_CLK_PIN, &CYBSP_PDM_CLK_config);
    Cy_GPIO_Pin_Init(CYBSP_PDM_DATA_PORT, CYBSP_PDM_DATA_PIN, &CYBSP_PDM_DATA_config);
    Cy_GPIO_Pin_Init(CYBSP_SERIAL_INT_0_PORT, CYBSP_SERIAL_INT_0_PIN, &CYBSP_SERIAL_INT_0_config);
    Cy_GPIO_Pin_Init(CYBSP_SERIAL_INT_2_PORT, CYBSP_SERIAL_INT_2_PIN, &CYBSP_SERIAL_INT_2_config);
    Cy_GPIO_Pin_Init(CYBSP_BT_UART_RX_PORT, CYBSP_BT_UART_RX_PIN, &CYBSP_BT_UART_RX_config);
    Cy_GPIO_Pin_Init(CYBSP_BT_UART_TX_PORT, CYBSP_BT_UART_TX_PIN, &CYBSP_BT_UART_TX_config);
    Cy_GPIO_Pin_Init(CYBSP_BT_UART_CTS_PORT, CYBSP_BT_UART_CTS_PIN, &CYBSP_BT_UART_CTS_config);
    Cy_GPIO_Pin_Init(CYBSP_BT_UART_RTS_PORT, CYBSP_BT_UART_RTS_PIN, &CYBSP_BT_UART_RTS_config);
    Cy_GPIO_Pin_Init(CYBSP_BT_HOST_WAKE_PORT, CYBSP_BT_HOST_WAKE_PIN, &CYBSP_BT_HOST_WAKE_config);
    Cy_GPIO_Pin_Init(CYBSP_USER_LED2_PORT, CYBSP_USER_LED2_PIN, &CYBSP_USER_LED2_config);
    Cy_GPIO_Pin_Init(CYBSP_BT_DEVICE_WAKE_PORT, CYBSP_BT_DEVICE_WAKE_PIN, &CYBSP_BT_DEVICE_WAKE_config);
    Cy_GPIO_Pin_Init(CYBSP_USER_LED1_PORT, CYBSP_USER_LED1_PIN, &CYBSP_USER_LED1_config);
    Cy_GPIO_Pin_Init(CYBSP_BT_POWER_PORT, CYBSP_BT_POWER_PIN, &CYBSP_BT_POWER_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_PCLK_PORT, CYBSP_DVP_CAM_PCLK_PIN, &CYBSP_DVP_CAM_PCLK_config);
    Cy_GPIO_Pin_Init(CYBSP_WIFI_WL_REG_ON_PORT, CYBSP_WIFI_WL_REG_ON_PIN, &CYBSP_WIFI_WL_REG_ON_config);
    Cy_GPIO_Pin_Init(CYBSP_WIFI_HOST_WAKE_PORT, CYBSP_WIFI_HOST_WAKE_PIN, &CYBSP_WIFI_HOST_WAKE_config);
    Cy_GPIO_Pin_Init(CYBSP_WIFI_SDIO_CLK_PORT, CYBSP_WIFI_SDIO_CLK_PIN, &CYBSP_WIFI_SDIO_CLK_config);
    Cy_GPIO_Pin_Init(CYBSP_WIFI_SDIO_D0_PORT, CYBSP_WIFI_SDIO_D0_PIN, &CYBSP_WIFI_SDIO_D0_config);
    Cy_GPIO_Pin_Init(CYBSP_WIFI_SDIO_D1_PORT, CYBSP_WIFI_SDIO_D1_PIN, &CYBSP_WIFI_SDIO_D1_config);
    Cy_GPIO_Pin_Init(CYBSP_I2S_TX_FSYNC_PORT, CYBSP_I2S_TX_FSYNC_PIN, &CYBSP_I2S_TX_FSYNC_config);
    Cy_GPIO_Pin_Init(CYBSP_WIFI_SDIO_D2_PORT, CYBSP_WIFI_SDIO_D2_PIN, &CYBSP_WIFI_SDIO_D2_config);
    Cy_GPIO_Pin_Init(CYBSP_WIFI_SDIO_D3_PORT, CYBSP_WIFI_SDIO_D3_PIN, &CYBSP_WIFI_SDIO_D3_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_D0_PORT, CYBSP_DVP_CAM_D0_PIN, &CYBSP_DVP_CAM_D0_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_D1_PORT, CYBSP_DVP_CAM_D1_PIN, &CYBSP_DVP_CAM_D1_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_D2_PORT, CYBSP_DVP_CAM_D2_PIN, &CYBSP_DVP_CAM_D2_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_D3_PORT, CYBSP_DVP_CAM_D3_PIN, &CYBSP_DVP_CAM_D3_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_D4_PORT, CYBSP_DVP_CAM_D4_PIN, &CYBSP_DVP_CAM_D4_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_D5_PORT, CYBSP_DVP_CAM_D5_PIN, &CYBSP_DVP_CAM_D5_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_D6_PORT, CYBSP_DVP_CAM_D6_PIN, &CYBSP_DVP_CAM_D6_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_D7_PORT, CYBSP_DVP_CAM_D7_PIN, &CYBSP_DVP_CAM_D7_config);
    Cy_GPIO_Pin_Init(CYBSP_I2C_SCL_3V3_PORT, CYBSP_I2C_SCL_3V3_PIN, &CYBSP_I2C_SCL_3V3_config);
    Cy_GPIO_Pin_Init(CYBSP_I2C_SDA_3V3_PORT, CYBSP_I2C_SDA_3V3_PIN, &CYBSP_I2C_SDA_3V3_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_VSYNC_PORT, CYBSP_DVP_CAM_VSYNC_PIN, &CYBSP_DVP_CAM_VSYNC_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_HREF_PORT, CYBSP_DVP_CAM_HREF_PIN, &CYBSP_DVP_CAM_HREF_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_XCLK_PORT, CYBSP_DVP_CAM_XCLK_PIN, &CYBSP_DVP_CAM_XCLK_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_RESET_PORT, CYBSP_DVP_CAM_RESET_PIN, &CYBSP_DVP_CAM_RESET_config);
    Cy_GPIO_Pin_Init(CYBSP_DVP_CAM_PWDN_PORT, CYBSP_DVP_CAM_PWDN_PIN, &CYBSP_DVP_CAM_PWDN_config);
    Cy_GPIO_Pin_Init(CYBSP_RADAR_INT_PORT, CYBSP_RADAR_INT_PIN, &CYBSP_RADAR_INT_config);
    Cy_GPIO_Pin_Init(CYBSP_LED_RGB_GREEN_PORT, CYBSP_LED_RGB_GREEN_PIN, &CYBSP_LED_RGB_GREEN_config);
    Cy_GPIO_Pin_Init(CYBSP_LED_RGB_BLUE_PORT, CYBSP_LED_RGB_BLUE_PIN, &CYBSP_LED_RGB_BLUE_config);
    Cy_GPIO_Pin_Init(CYBSP_LED_RGB_RED_PORT, CYBSP_LED_RGB_RED_PIN, &CYBSP_LED_RGB_RED_config);
    Cy_GPIO_Pin_Init(CYBSP_RXRES_L_PORT, CYBSP_RXRES_L_PIN, &CYBSP_RXRES_L_config);
    Cy_GPIO_Pin_Init(CYBSP_WIFI_SDIO_CMD_PORT, CYBSP_WIFI_SDIO_CMD_PIN, &CYBSP_WIFI_SDIO_CMD_config);
    Cy_GPIO_Pin_Init(CYBSP_I2S_TX_SD_PORT, CYBSP_I2S_TX_SD_PIN, &CYBSP_I2S_TX_SD_config);
    Cy_GPIO_Pin_Init(CYBSP_I2S_TX_SCK_PORT, CYBSP_I2S_TX_SCK_PIN, &CYBSP_I2S_TX_SCK_config);
    Cy_GPIO_Pin_Init(CYBSP_I2S_TX_MCK_PORT, CYBSP_I2S_TX_MCK_PIN, &CYBSP_I2S_TX_MCK_config);
    Cy_GPIO_Pin_Init(CYBSP_RSPI_MISO_PORT, CYBSP_RSPI_MISO_PIN, &CYBSP_RSPI_MISO_config);
    Cy_GPIO_Pin_Init(CYBSP_RSPI_MOSI_PORT, CYBSP_RSPI_MOSI_PIN, &CYBSP_RSPI_MOSI_config);
    Cy_GPIO_Pin_Init(CYBSP_RSPI_CLK_PORT, CYBSP_RSPI_CLK_PIN, &CYBSP_RSPI_CLK_config);
    Cy_GPIO_Pin_Init(CYBSP_RSPI_CS_PORT, CYBSP_RSPI_CS_PIN, &CYBSP_RSPI_CS_config);
}
