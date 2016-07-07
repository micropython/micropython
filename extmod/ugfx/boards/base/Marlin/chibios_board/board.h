/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012,2013 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for a generic STM32F4xx board.
 */

/*
 * Board identifier.
 */
#define BOARD_MARLIN_STM32F417
#define BOARD_NAME                  "Marlin STM32F417 board"

/*
 * Ethernet PHY type.
 */
#define MII_KSZ8051RNL_ID           0x00221550
#define BOARD_PHY_ID                MII_KSZ8051RNL_ID
#define BOARD_PHY_RMII

/*
 * Board oscillators-related settings.
 */
#define STM32_LSECLK                0
#define STM32_HSECLK                16000000
#define STM32_HSECLK_BYPASS

/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD                   300

/*
 * MCU type as defined in the ST header file stm32f4xx.h.
 */
#define STM32F4XX

/*
 * IO pins assignments.
 */
#define GPIOA_HEATER_0                    0
#define GPIOA_ETH_REFCLK                  1
#define GPIOA_ETH_MDIO                    2
#define GPIOA_HEATER_1                    3
#define GPIOA_BL                          4
#define GPIOA_CLK                         5
#define GPIOA_Y_B                         6
#define GPIOA_ETH_CRS_DV                  7
#define GPIOA_HBP                         8
#define GPIOA_INTERLOCK                   9
#define GPIOA_PWM_0                       10
#define GPIOA_HD_N                        11
#define GPIOA_HD_P                        12
#define GPIOA_SWDIO                       13
#define GPIOA_SWCLK                       14
#define GPIOA_X_B                         15

#define GPIOB_DIR_Z                       0
#define GPIOB_STEP_Z                      1
#define GPIOB_HOME_Y                      2
#define GPIOB_X_A                         3
#define GPIOB_MISO                        4
#define GPIOB_MOSI                        5
#define GPIOB_MB_UART1_TX                 6
#define GPIOB_MB_UART1_RX                 7
#define GPIOB_CAN_RX                      8
#define GPIOB_CAN_TX                      9
#define GPIOB_I2S_SCK                     10
#define GPIOB_NOT_USED_1                  11
#define GPIOB_I2S_WS                      12
#define GPIOB_VBUS_D                      13
#define GPIOB_DD_N                        14
#define GPIOB_DD_P                        15

#define GPIOC_ANALOG_1                    0
#define GPIOC_ETH_MDC                     1
#define GPIOC_HOME_Z                      2
#define GPIOC_I2S_SD                      3
#define GPIOC_ETH_RXD0                    4
#define GPIOC_ETH_RXD1                    5
#define GPIOC_SDI_ST                      6
#define GPIOC_Y_A                         7
#define GPIOC_SDIO_D0                     8
#define GPIOC_SDIO_D1                     9
#define GPIOC_SDIO_D2                     10
#define GPIOC_SDIO_D3                     11
#define GPIOC_SDIO_SK                     12
#define GPIOC_PIN13                       13
#define GPIOC_PIN14                       14
#define GPIOC_PIN15                       15

#define GPIOD_D2                          0
#define GPIOD_D3                          1
#define GPIOD_SDIO_CMD                    2
#define GPIOD_SDIO_CD_N                   3
#define GPIOD_RDN                         4
#define GPIOD_WRN                         5
#define GPIOD_STEPPER_EN_N                6
#define GPIOD_POWER_OUT_EN_N              7
#define GPIOD_D13                         8
#define GPIOD_D14                         9
#define GPIOD_D15                         10
#define GPIOD_P_CN                        11
#define GPIOD_HEATER_2                    12
#define GPIOD_PWM_1                       13
#define GPIOD_D0                          14
#define GPIOD_D1                          15

#define GPIOE_STEP_E2                     0
#define GPIOE_DIR_E2                      1
#define GPIOE_INT                         2
#define GPIOE_VMOT_EN                     3
#define GPIOE_LED_STS_1                   4
#define GPIOE_FAN_0                       5
#define GPIOE_FAN_1                       6
#define GPIOE_D4                          7
#define GPIOE_D5                          8
#define GPIOE_D6                          9
#define GPIOE_D7                          10
#define GPIOE_D8                          11
#define GPIOE_D9                          12
#define GPIOE_D10                         13
#define GPIOE_D11                         14
#define GPIOE_D12                         15

#define GPIOF_SDA                         0
#define GPIOF_SCL                         1
#define GPIOF_PRE_CHARGE                  2
#define GPIOF_VMOT_MEAS                   3
#define GPIOF_CURR_OUT                    4
#define GPIOF_VIN_MEAS                    5
#define GPIOF_TEMP_E2                     6
#define GPIOF_TEMP_E1                     7
#define GPIOF_TEMP_E0                     8
#define GPIOF_TEMP_HPB                    9
#define GPIOF_PIN10                       10
#define GPIOF_DIR_Y                       11
#define GPIOF_STEP_Y                      12
#define GPIOF_HOME_X                      13
#define GPIOF_MB_CS0_N                    14
#define GPIOF_MB_CS1_N                    15

#define GPIOG_DIR_X                       0
#define GPIOG_STEP_X                      1
#define GPIOG_STEP_E0                     2
#define GPIOG_DIR_E0                      3
#define GPIOG_STEP_E1                     4
#define GPIOG_DIR_E1                      5
#define GPIOG_LED_STS_2                   6
#define GPIOG_SCK_ST                      7
#define GPIOG_PIN8                        8
#define GPIOG_SDO_ST                      9
#define GPIOG_CS_N                        10
#define GPIOG_ETH_TXEN                    11
#define GPIOG_CS_ST_N                     12
#define GPIOG_ETH_TXD0                    13
#define GPIOG_ETH_TXD1                    14
#define GPIOG_PIN15                       15

#define GPIOH_PIN0                        0
#define GPIOH_PIN1                        1
#define GPIOH_PIN2                        2
#define GPIOH_PIN3                        3
#define GPIOH_PIN4                        4
#define GPIOH_PIN5                        5
#define GPIOH_PIN6                        6
#define GPIOH_PIN7                        7
#define GPIOH_PIN8                        8
#define GPIOH_PIN9                        9
#define GPIOH_PIN10                       10
#define GPIOH_PIN11                       11
#define GPIOH_PIN12                       12
#define GPIOH_PIN13                       13
#define GPIOH_PIN14                       14
#define GPIOH_PIN15                       15

#define GPIOI_PIN0                        0
#define GPIOI_PIN1                        1
#define GPIOI_PIN2                        2
#define GPIOI_PIN3                        3
#define GPIOI_PIN4                        4
#define GPIOI_PIN5                        5
#define GPIOI_PIN6                        6
#define GPIOI_PIN7                        7
#define GPIOI_PIN8                        8
#define GPIOI_PIN9                        9
#define GPIOI_PIN10                       10
#define GPIOI_PIN11                       11
#define GPIOI_PIN12                       12
#define GPIOI_PIN13                       13
#define GPIOI_PIN14                       14
#define GPIOI_PIN15                       15


/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 * Please refer to the STM32 Reference Manual for details.
 */
#define PIN_MODE_INPUT(n)           (0U << ((n) * 2))
#define PIN_MODE_OUTPUT(n)          (1U << ((n) * 2))
#define PIN_MODE_ALTERNATE(n)       (2U << ((n) * 2))
#define PIN_MODE_ANALOG(n)          (3U << ((n) * 2))
#define PIN_ODR_LOW(n)              (0U << (n))
#define PIN_ODR_HIGH(n)             (1U << (n))
#define PIN_OTYPE_PUSHPULL(n)       (0U << (n))
#define PIN_OTYPE_OPENDRAIN(n)      (1U << (n))
#define PIN_OSPEED_2M(n)            (0U << ((n) * 2))
#define PIN_OSPEED_25M(n)           (1U << ((n) * 2))
#define PIN_OSPEED_50M(n)           (2U << ((n) * 2))
#define PIN_OSPEED_100M(n)          (3U << ((n) * 2))
#define PIN_PUPDR_FLOATING(n)       (0U << ((n) * 2))
#define PIN_PUPDR_PULLUP(n)         (1U << ((n) * 2))
#define PIN_PUPDR_PULLDOWN(n)       (2U << ((n) * 2))
#define PIN_AFIO_AF(n, v)           ((v##U) << ((n % 8) * 4))

/*
 * GPIOA setup:
 *
 * PA0  - HEATER_0                  (output pushpull 2mhz).
 * PA1  - ETH_REFCLK                (alternate 11).
 * PA2  - ETH_MDIO                  (alternate 11).
 * PA3  - HEATER_1                  (output pushpull 2mhz).
 * PA4  - BL                        (output pushpull 2mhz).
 * PA5  - CLK                       (alternate 5).
 * PA6  - Y_B                       (alternate 0).
 * PA7  - ETH_CRS_DV                (alternate 11).
 * PA8  - HBP                       (output pushpull 2mhz).
 * PA9  - INTERLOCK                 (input pull-up).
 * PA10 - PWM_0                     (output pushpull 2mhz).
 * PA11 - HD_N                      (alternate 6).
 * PA12 - HD_P                      (alternate 10).
 * PA13 - SWDIO                     (alternate 0).
 * PA14 - SWCLK                     (alternate 0).
 * PA15 - X_B                       (alternate 1).
 */
#define VAL_GPIOA_MODER             (PIN_MODE_ALTERNATE(GPIOA_HEATER_0) |   \
                                     PIN_MODE_ALTERNATE(GPIOA_ETH_REFCLK) | \
                                     PIN_MODE_ALTERNATE(GPIOA_ETH_MDIO) |   \
                                     PIN_MODE_ALTERNATE(GPIOA_HEATER_1) |   \
                                     PIN_MODE_OUTPUT(GPIOA_BL) |            \
                                     PIN_MODE_ALTERNATE(GPIOA_CLK) |        \
                                     PIN_MODE_ALTERNATE(GPIOA_Y_B) |        \
                                     PIN_MODE_ALTERNATE(GPIOA_ETH_CRS_DV) | \
                                     PIN_MODE_ALTERNATE(GPIOA_HBP) |        \
                                     PIN_MODE_INPUT(GPIOA_INTERLOCK) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_PWM_0) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_HD_N) |       \
                                     PIN_MODE_ALTERNATE(GPIOA_HD_P) |       \
                                     PIN_MODE_ALTERNATE(GPIOA_SWDIO) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_SWCLK) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_X_B))
#define VAL_GPIOA_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOA_HEATER_0) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOA_ETH_REFCLK) | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_ETH_MDIO) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOA_HEATER_1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOA_BL) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOA_CLK) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOA_Y_B) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOA_ETH_CRS_DV) | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_HBP) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOA_INTERLOCK) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_PWM_0) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_HD_N) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_HD_P) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWDIO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWCLK) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_X_B))
#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_2M(GPIOA_HEATER_0) |        \
                                     PIN_OSPEED_50M(GPIOA_ETH_REFCLK) |     \
                                     PIN_OSPEED_25M(GPIOA_ETH_MDIO) |       \
                                     PIN_OSPEED_2M(GPIOA_HEATER_1) |        \
                                     PIN_OSPEED_2M(GPIOA_BL) |              \
                                     PIN_OSPEED_100M(GPIOA_CLK) |            \
                                     PIN_OSPEED_50M(GPIOA_Y_B) |            \
                                     PIN_OSPEED_50M(GPIOA_ETH_CRS_DV) |     \
                                     PIN_OSPEED_2M(GPIOA_HBP) |             \
                                     PIN_OSPEED_50M(GPIOA_INTERLOCK) |      \
                                     PIN_OSPEED_2M(GPIOA_PWM_0) |           \
                                     PIN_OSPEED_50M(GPIOA_HD_N) |           \
                                     PIN_OSPEED_50M(GPIOA_HD_P) |           \
                                     PIN_OSPEED_50M(GPIOA_SWDIO) |          \
                                     PIN_OSPEED_50M(GPIOA_SWCLK) |          \
                                     PIN_OSPEED_50M(GPIOA_X_B))
#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_HEATER_0) |   \
                                     PIN_PUPDR_FLOATING(GPIOA_ETH_REFCLK) | \
                                     PIN_PUPDR_FLOATING(GPIOA_ETH_MDIO) |   \
                                     PIN_PUPDR_FLOATING(GPIOA_HEATER_1) |   \
                                     PIN_PUPDR_FLOATING(GPIOA_BL) |         \
                                     PIN_PUPDR_FLOATING(GPIOA_CLK) |        \
                                     PIN_PUPDR_FLOATING(GPIOA_Y_B) |        \
                                     PIN_PUPDR_FLOATING(GPIOA_ETH_CRS_DV) | \
                                     PIN_PUPDR_FLOATING(GPIOA_HBP) |        \
                                     PIN_PUPDR_PULLUP(GPIOA_INTERLOCK) |    \
                                     PIN_PUPDR_FLOATING(GPIOA_PWM_0) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_HD_N) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_HD_P) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_SWDIO) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_SWCLK) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_X_B))
#define VAL_GPIOA_ODR               (PIN_ODR_LOW(GPIOA_HEATER_0) |          \
                                     PIN_ODR_LOW(GPIOA_ETH_REFCLK) |        \
                                     PIN_ODR_LOW(GPIOA_ETH_MDIO) |          \
                                     PIN_ODR_LOW(GPIOA_HEATER_1) |          \
                                     PIN_ODR_LOW(GPIOA_BL) |                \
                                     PIN_ODR_LOW(GPIOA_CLK) |               \
                                     PIN_ODR_LOW(GPIOA_Y_B) |               \
                                     PIN_ODR_LOW(GPIOA_ETH_CRS_DV) |        \
                                     PIN_ODR_LOW(GPIOA_HBP) |               \
                                     PIN_ODR_LOW(GPIOA_INTERLOCK) |         \
                                     PIN_ODR_LOW(GPIOA_PWM_0) |             \
                                     PIN_ODR_LOW(GPIOA_HD_N) |              \
                                     PIN_ODR_LOW(GPIOA_HD_P) |              \
                                     PIN_ODR_LOW(GPIOA_SWDIO) |             \
                                     PIN_ODR_LOW(GPIOA_SWCLK) |             \
                                     PIN_ODR_LOW(GPIOA_X_B))
#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_HEATER_0, 2) |       \
                                     PIN_AFIO_AF(GPIOA_ETH_REFCLK, 11) |    \
                                     PIN_AFIO_AF(GPIOA_ETH_MDIO, 11) |      \
                                     PIN_AFIO_AF(GPIOA_HEATER_1, 2) |       \
                                     PIN_AFIO_AF(GPIOA_BL, 0) |             \
                                     PIN_AFIO_AF(GPIOA_CLK, 5) |            \
                                     PIN_AFIO_AF(GPIOA_Y_B, 0) |            \
                                     PIN_AFIO_AF(GPIOA_ETH_CRS_DV, 11))
#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_HBP, 1) |            \
                                     PIN_AFIO_AF(GPIOA_INTERLOCK, 0) |      \
                                     PIN_AFIO_AF(GPIOA_PWM_0, 1) |          \
                                     PIN_AFIO_AF(GPIOA_HD_N, 10) |           \
                                     PIN_AFIO_AF(GPIOA_HD_P, 10) |          \
                                     PIN_AFIO_AF(GPIOA_SWDIO, 0) |          \
                                     PIN_AFIO_AF(GPIOA_SWCLK, 0) |          \
                                     PIN_AFIO_AF(GPIOA_X_B, 1))

/*
 * GPIOB setup:
 *
 * PB0  - DIR_Z                     (output pushpull 2mhz).
 * PB1  - STEP_Z                    (output pushpull 2mhz).
 * PB2  - HOME_Y                    (input floating).
 * PB3  - X_A                       (alternate 1).
 * PB4  - MISO                      (alternate 5).
 * PB5  - MOSI                      (alternate 5).
 * PB6  - MB_SCL                    (alternate 7).
 * PB7  - MB_SDA                    (alternate 7).
 * PB8  - CAN_RX                    (input floating).
 * PB9  - CAN_TX                    (input floating).
 * PB10 - I2S_SCK                   (alternate 5).
 * PB11 - NOT_USED_1                (input floating).
 * PB12 - I2S_WS                    (alternate 5).
 * PB13 - VBUS_D                    (input pull-up).
 * PB14 - DD_N                      (alternate 12).
 * PB15 - DD_P                      (alternate 12).
 */
#define VAL_GPIOB_MODER             (PIN_MODE_OUTPUT(GPIOB_DIR_Z) |         \
                                     PIN_MODE_OUTPUT(GPIOB_STEP_Z) |        \
                                     PIN_MODE_INPUT(GPIOB_HOME_Y) |         \
                                     PIN_MODE_ALTERNATE(GPIOB_X_A) |        \
                                     PIN_MODE_ALTERNATE(GPIOB_MISO) |       \
                                     PIN_MODE_ALTERNATE(GPIOB_MOSI) |       \
                                     PIN_MODE_ALTERNATE(GPIOB_MB_UART1_TX) |\
                                     PIN_MODE_ALTERNATE(GPIOB_MB_UART1_RX) |\
                                     PIN_MODE_INPUT(GPIOB_CAN_RX) |         \
                                     PIN_MODE_INPUT(GPIOB_CAN_TX) |         \
                                     PIN_MODE_ALTERNATE(GPIOB_I2S_SCK) |    \
                                     PIN_MODE_INPUT(GPIOB_NOT_USED_1) |     \
                                     PIN_MODE_ALTERNATE(GPIOB_I2S_WS) |     \
                                     PIN_MODE_INPUT(GPIOB_VBUS_D) |         \
                                     PIN_MODE_ALTERNATE(GPIOB_DD_N) |       \
                                     PIN_MODE_ALTERNATE(GPIOB_DD_P))
#define VAL_GPIOB_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOB_DIR_Z) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_STEP_Z) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_HOME_Y) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_X_A) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOB_MISO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_MOSI) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_MB_UART1_TX) |\
                                     PIN_OTYPE_OPENDRAIN(GPIOB_MB_UART1_RX) |\
                                     PIN_OTYPE_PUSHPULL(GPIOB_CAN_RX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_CAN_TX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_I2S_SCK) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_NOT_USED_1) | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_I2S_WS) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_VBUS_D) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_DD_N) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_DD_P))
#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_2M(GPIOB_DIR_Z) |           \
                                     PIN_OSPEED_2M(GPIOB_STEP_Z) |          \
                                     PIN_OSPEED_50M(GPIOB_HOME_Y) |         \
                                     PIN_OSPEED_50M(GPIOB_X_A) |            \
                                     PIN_OSPEED_100M(GPIOB_MISO) |           \
                                     PIN_OSPEED_100M(GPIOB_MOSI) |           \
                                     PIN_OSPEED_50M(GPIOB_MB_UART1_TX) |    \
                                     PIN_OSPEED_50M(GPIOB_MB_UART1_RX) |    \
                                     PIN_OSPEED_50M(GPIOB_CAN_RX) |         \
                                     PIN_OSPEED_50M(GPIOB_CAN_TX) |         \
                                     PIN_OSPEED_50M(GPIOB_I2S_SCK) |        \
                                     PIN_OSPEED_50M(GPIOB_NOT_USED_1) |     \
                                     PIN_OSPEED_50M(GPIOB_I2S_WS) |         \
                                     PIN_OSPEED_50M(GPIOB_VBUS_D) |         \
                                     PIN_OSPEED_50M(GPIOB_DD_N) |           \
                                     PIN_OSPEED_50M(GPIOB_DD_P))
#define VAL_GPIOB_PUPDR             (PIN_PUPDR_FLOATING(GPIOB_DIR_Z) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_STEP_Z) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_HOME_Y) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_X_A) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_MISO) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_MOSI) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_MB_UART1_TX) |\
                                     PIN_PUPDR_FLOATING(GPIOB_MB_UART1_RX) |\
                                     PIN_PUPDR_FLOATING(GPIOB_CAN_RX) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_CAN_TX) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_I2S_SCK) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_NOT_USED_1) | \
                                     PIN_PUPDR_FLOATING(GPIOB_I2S_WS) |     \
                                     PIN_PUPDR_PULLUP(GPIOB_VBUS_D) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_DD_N) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_DD_P))
#define VAL_GPIOB_ODR               (PIN_ODR_LOW(GPIOB_DIR_Z) |             \
                                     PIN_ODR_LOW(GPIOB_STEP_Z) |            \
                                     PIN_ODR_LOW(GPIOB_HOME_Y) |            \
                                     PIN_ODR_LOW(GPIOB_X_A) |               \
                                     PIN_ODR_LOW(GPIOB_MISO) |              \
                                     PIN_ODR_LOW(GPIOB_MOSI) |              \
                                     PIN_ODR_LOW(GPIOB_MB_UART1_TX) |       \
                                     PIN_ODR_LOW(GPIOB_MB_UART1_RX) |       \
                                     PIN_ODR_LOW(GPIOB_CAN_RX) |            \
                                     PIN_ODR_LOW(GPIOB_CAN_TX) |            \
                                     PIN_ODR_LOW(GPIOB_I2S_SCK) |           \
                                     PIN_ODR_LOW(GPIOB_NOT_USED_1) |        \
                                     PIN_ODR_LOW(GPIOB_I2S_WS) |            \
                                     PIN_ODR_LOW(GPIOB_VBUS_D) |            \
                                     PIN_ODR_LOW(GPIOB_DD_N) |              \
                                     PIN_ODR_LOW(GPIOB_DD_P))
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_DIR_Z, 0) |          \
                                     PIN_AFIO_AF(GPIOB_STEP_Z, 0) |         \
                                     PIN_AFIO_AF(GPIOB_HOME_Y, 0) |         \
                                     PIN_AFIO_AF(GPIOB_X_A, 1) |            \
                                     PIN_AFIO_AF(GPIOB_MISO, 5) |           \
                                     PIN_AFIO_AF(GPIOB_MOSI, 5) |           \
                                     PIN_AFIO_AF(GPIOB_MB_UART1_TX, 4) |    \
                                     PIN_AFIO_AF(GPIOB_MB_UART1_RX, 4))
#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_CAN_RX, 0) |         \
                                     PIN_AFIO_AF(GPIOB_CAN_TX, 0) |         \
                                     PIN_AFIO_AF(GPIOB_I2S_SCK, 5) |        \
                                     PIN_AFIO_AF(GPIOB_NOT_USED_1, 0) |     \
                                     PIN_AFIO_AF(GPIOB_I2S_WS, 5) |         \
                                     PIN_AFIO_AF(GPIOB_VBUS_D, 0) |         \
                                     PIN_AFIO_AF(GPIOB_DD_N, 12) |          \
                                     PIN_AFIO_AF(GPIOB_DD_P, 12))

/*
 * GPIOC setup:
 *
 * PC0  - ANALOG_1                  (input floating).
 * PC1  - ETH_MDC                   (alternate 11).
 * PC2  - HOME_Z                    (input floating).
 * PC3  - I2S_SD                    (alternate 5).
 * PC4  - ETH_RXD0                  (alternate 11).
 * PC5  - ETH_RXD1                  (alternate 11).
 * PC6  - SDI_ST                    (alternate 8).
 * PC7  - Y_A                       (alternate 2).
 * PC8  - SDIO_D0                   (alternate 12).
 * PC9  - SDIO_D1                   (alternate 12).
 * PC10 - SDIO_D2                   (alternate 12).
 * PC11 - SDIO_D3                   (alternate 12).
 * PC12 - SDIO_SK                   (alternate 12).
 * PC13 - P                         (input floating).
 * PC14 - P                         (input floating).
 * PC15 - P                         (input floating).
 */
#define VAL_GPIOC_MODER             (PIN_MODE_INPUT(GPIOC_ANALOG_1) |       \
                                     PIN_MODE_ALTERNATE(GPIOC_ETH_MDC) |    \
                                     PIN_MODE_INPUT(GPIOC_HOME_Z) |         \
                                     PIN_MODE_ALTERNATE(GPIOC_I2S_SD) |     \
                                     PIN_MODE_ALTERNATE(GPIOC_ETH_RXD0) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_ETH_RXD1) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_SDI_ST) |     \
                                     PIN_MODE_ALTERNATE(GPIOC_Y_A) |        \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_D0) |    \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_D1) |    \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_D2) |    \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_D3) |    \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_SK) |    \
                                     PIN_MODE_INPUT(GPIOC_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOC_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOC_PIN15))
#define VAL_GPIOC_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOC_ANALOG_1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_ETH_MDC) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_HOME_Z) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_I2S_SD) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_ETH_RXD0) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_ETH_RXD1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDI_ST) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_Y_A) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D0) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D2) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_SK) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN15))
#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_50M(GPIOC_ANALOG_1) |       \
                                     PIN_OSPEED_50M(GPIOC_ETH_MDC) |        \
                                     PIN_OSPEED_50M(GPIOC_HOME_Z) |         \
                                     PIN_OSPEED_50M(GPIOC_I2S_SD) |         \
                                     PIN_OSPEED_50M(GPIOC_ETH_RXD0) |       \
                                     PIN_OSPEED_50M(GPIOC_ETH_RXD1) |       \
                                     PIN_OSPEED_50M(GPIOC_SDI_ST) |         \
                                     PIN_OSPEED_50M(GPIOC_Y_A) |            \
                                     PIN_OSPEED_50M(GPIOC_SDIO_D0) |        \
                                     PIN_OSPEED_50M(GPIOC_SDIO_D1) |        \
                                     PIN_OSPEED_50M(GPIOC_SDIO_D2) |        \
                                     PIN_OSPEED_50M(GPIOC_SDIO_D3) |        \
                                     PIN_OSPEED_50M(GPIOC_SDIO_SK) |        \
                                     PIN_OSPEED_50M(GPIOC_PIN13) |          \
                                     PIN_OSPEED_50M(GPIOC_PIN14) |          \
                                     PIN_OSPEED_50M(GPIOC_PIN15))
#define VAL_GPIOC_PUPDR             (PIN_PUPDR_FLOATING(GPIOC_ANALOG_1) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_ETH_MDC) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_HOME_Z) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_I2S_SD) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_ETH_RXD0) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_ETH_RXD1) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_SDI_ST) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_Y_A) |        \
                                     PIN_PUPDR_FLOATING(GPIOC_SDIO_D0) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_SDIO_D1) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_SDIO_D2) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_SDIO_D3) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_SDIO_SK) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_PIN13) |      \
                                     PIN_PUPDR_FLOATING(GPIOC_PIN14) |      \
                                     PIN_PUPDR_FLOATING(GPIOC_PIN15))
#define VAL_GPIOC_ODR               (PIN_ODR_LOW(GPIOC_ANALOG_1) |          \
                                     PIN_ODR_LOW(GPIOC_ETH_MDC) |           \
                                     PIN_ODR_LOW(GPIOC_HOME_Z) |            \
                                     PIN_ODR_LOW(GPIOC_I2S_SD) |            \
                                     PIN_ODR_LOW(GPIOC_ETH_RXD0) |          \
                                     PIN_ODR_LOW(GPIOC_ETH_RXD1) |          \
                                     PIN_ODR_LOW(GPIOC_SDI_ST) |            \
                                     PIN_ODR_LOW(GPIOC_Y_A) |               \
                                     PIN_ODR_LOW(GPIOC_SDIO_D0) |           \
                                     PIN_ODR_LOW(GPIOC_SDIO_D1) |           \
                                     PIN_ODR_LOW(GPIOC_SDIO_D2) |           \
                                     PIN_ODR_LOW(GPIOC_SDIO_D3) |           \
                                     PIN_ODR_LOW(GPIOC_SDIO_SK) |           \
                                     PIN_ODR_LOW(GPIOC_PIN13) |             \
                                     PIN_ODR_LOW(GPIOC_PIN14) |             \
                                     PIN_ODR_LOW(GPIOC_PIN15))
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOC_ANALOG_1, 0) |       \
                                     PIN_AFIO_AF(GPIOC_ETH_MDC, 11) |       \
                                     PIN_AFIO_AF(GPIOC_HOME_Z, 0) |         \
                                     PIN_AFIO_AF(GPIOC_I2S_SD, 5) |         \
                                     PIN_AFIO_AF(GPIOC_ETH_RXD0, 11) |      \
                                     PIN_AFIO_AF(GPIOC_ETH_RXD1, 11) |      \
                                     PIN_AFIO_AF(GPIOC_SDI_ST, 8) |         \
                                     PIN_AFIO_AF(GPIOC_Y_A, 2))
#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_SDIO_D0, 12) |       \
                                     PIN_AFIO_AF(GPIOC_SDIO_D1, 12) |       \
                                     PIN_AFIO_AF(GPIOC_SDIO_D2, 12) |       \
                                     PIN_AFIO_AF(GPIOC_SDIO_D3, 12) |       \
                                     PIN_AFIO_AF(GPIOC_SDIO_SK, 12) |       \
                                     PIN_AFIO_AF(GPIOC_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOC_PIN14, 0) |          \
                                     PIN_AFIO_AF(GPIOC_PIN15, 0))

/*
 * GPIOD setup:
 *
 * PD0  - D2                        (alternate 12).
 * PD1  - D3                        (alternate 12).
 * PD2  - SDIO_CMD                  (alternate 12).
 * PD3  - SDIO_CD_N                 (input floating).
 * PD4  - RDN                       (alternate 12).
 * PD5  - WRN                       (alternate 12).
 * PD6  - STEPPER_EN                (output pushpull 2mhz).
 * PD7  - POWER_OUT_EN_N            (output pushpull 2mhz).
 * PD8  - D13                       (alternate 12).
 * PD9  - D14                       (alternate 12).
 * PD10 - D15                       (alternate 12).
 * PD11 - P_CN                      (alternate 12).
 * PD12 - HEATER_2                  (output pushpull 2mhz).
 * PD13 - PWM_1                     (output pushpull 2mhz).
 * PD14 - D0                        (alternate 12).
 * PD15 - D1                        (alternate 12).
 */
#define VAL_GPIOD_MODER             (PIN_MODE_ALTERNATE(GPIOD_D2) |         \
                                     PIN_MODE_ALTERNATE(GPIOD_D3) |         \
                                     PIN_MODE_ALTERNATE(GPIOD_SDIO_CMD) |   \
                                     PIN_MODE_INPUT(GPIOD_SDIO_CD_N) |      \
                                     PIN_MODE_ALTERNATE(GPIOD_RDN) |        \
                                     PIN_MODE_ALTERNATE(GPIOD_WRN) |        \
                                     PIN_MODE_OUTPUT(GPIOD_STEPPER_EN_N) |  \
                                     PIN_MODE_OUTPUT(GPIOD_POWER_OUT_EN_N) |\
                                     PIN_MODE_ALTERNATE(GPIOD_D13) |        \
                                     PIN_MODE_ALTERNATE(GPIOD_D14) |        \
                                     PIN_MODE_ALTERNATE(GPIOD_D15) |        \
                                     PIN_MODE_ALTERNATE(GPIOD_P_CN) |       \
                                     PIN_MODE_ALTERNATE(GPIOD_HEATER_2) |   \
                                     PIN_MODE_ALTERNATE(GPIOD_PWM_1) |      \
                                     PIN_MODE_ALTERNATE(GPIOD_D0) |         \
                                     PIN_MODE_ALTERNATE(GPIOD_D1))
#define VAL_GPIOD_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOD_D2) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOD_D3) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SDIO_CMD) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SDIO_CD_N) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RDN) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOD_WRN) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOD_STEPPER_EN_N) |\
                                     PIN_OTYPE_PUSHPULL(GPIOD_POWER_OUT_EN_N) |\
                                     PIN_OTYPE_PUSHPULL(GPIOD_D13) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOD_D14) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOD_D15) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOD_P_CN) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_HEATER_2) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PWM_1) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_D0) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOD_D1))
#define VAL_GPIOD_OSPEEDR           (PIN_OSPEED_50M(GPIOD_D2) |             \
                                     PIN_OSPEED_50M(GPIOD_D3) |             \
                                     PIN_OSPEED_50M(GPIOD_SDIO_CMD) |       \
                                     PIN_OSPEED_50M(GPIOD_SDIO_CD_N) |      \
                                     PIN_OSPEED_50M(GPIOD_RDN) |            \
                                     PIN_OSPEED_50M(GPIOD_WRN) |            \
                                     PIN_OSPEED_2M(GPIOD_STEPPER_EN_N) |    \
                                     PIN_OSPEED_2M(GPIOD_POWER_OUT_EN_N) |  \
                                     PIN_OSPEED_50M(GPIOD_D13) |            \
                                     PIN_OSPEED_50M(GPIOD_D14) |            \
                                     PIN_OSPEED_50M(GPIOD_D15) |            \
                                     PIN_OSPEED_50M(GPIOD_P_CN) |           \
                                     PIN_OSPEED_2M(GPIOD_HEATER_2) |        \
                                     PIN_OSPEED_2M(GPIOD_PWM_1) |           \
                                     PIN_OSPEED_50M(GPIOD_D0) |             \
                                     PIN_OSPEED_50M(GPIOD_D1))
#define VAL_GPIOD_PUPDR             (PIN_PUPDR_FLOATING(GPIOD_D2) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_D3) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_SDIO_CMD) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_SDIO_CD_N) |  \
                                     PIN_PUPDR_FLOATING(GPIOD_RDN) |        \
                                     PIN_PUPDR_FLOATING(GPIOD_WRN) |        \
                                     PIN_PUPDR_FLOATING(GPIOD_STEPPER_EN_N) |\
                                     PIN_PUPDR_FLOATING(GPIOD_POWER_OUT_EN_N) |\
                                     PIN_PUPDR_FLOATING(GPIOD_D13) |        \
                                     PIN_PUPDR_FLOATING(GPIOD_D14) |        \
                                     PIN_PUPDR_FLOATING(GPIOD_D15) |        \
                                     PIN_PUPDR_FLOATING(GPIOD_P_CN) |       \
                                     PIN_PUPDR_FLOATING(GPIOD_HEATER_2) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_PWM_1) |      \
                                     PIN_PUPDR_FLOATING(GPIOD_D0) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_D1))
#define VAL_GPIOD_ODR               (PIN_ODR_LOW(GPIOD_D2) |                \
                                     PIN_ODR_LOW(GPIOD_D3) |                \
                                     PIN_ODR_LOW(GPIOD_SDIO_CMD) |          \
                                     PIN_ODR_LOW(GPIOD_SDIO_CD_N) |         \
                                     PIN_ODR_LOW(GPIOD_RDN) |               \
                                     PIN_ODR_LOW(GPIOD_WRN) |               \
                                     PIN_ODR_LOW(GPIOD_STEPPER_EN_N) |      \
                                     PIN_ODR_HIGH(GPIOD_POWER_OUT_EN_N) |   \
                                     PIN_ODR_LOW(GPIOD_D13) |               \
                                     PIN_ODR_LOW(GPIOD_D14) |               \
                                     PIN_ODR_LOW(GPIOD_D15) |               \
                                     PIN_ODR_LOW(GPIOD_P_CN) |              \
                                     PIN_ODR_LOW(GPIOD_HEATER_2) |          \
                                     PIN_ODR_LOW(GPIOD_PWM_1) |             \
                                     PIN_ODR_LOW(GPIOD_D0) |                \
                                     PIN_ODR_LOW(GPIOD_D1))
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_D2, 12) |            \
                                     PIN_AFIO_AF(GPIOD_D3, 12) |            \
                                     PIN_AFIO_AF(GPIOD_SDIO_CMD, 12) |      \
                                     PIN_AFIO_AF(GPIOD_SDIO_CD_N, 0) |      \
                                     PIN_AFIO_AF(GPIOD_RDN, 12) |           \
                                     PIN_AFIO_AF(GPIOD_WRN, 12) |           \
                                     PIN_AFIO_AF(GPIOD_STEPPER_EN_N, 0) |   \
                                     PIN_AFIO_AF(GPIOD_POWER_OUT_EN_N, 0))
#define VAL_GPIOD_AFRH              (PIN_AFIO_AF(GPIOD_D13, 12) |           \
                                     PIN_AFIO_AF(GPIOD_D14, 12) |           \
                                     PIN_AFIO_AF(GPIOD_D15, 12) |           \
                                     PIN_AFIO_AF(GPIOD_P_CN, 12) |          \
                                     PIN_AFIO_AF(GPIOD_HEATER_2, 2) |       \
                                     PIN_AFIO_AF(GPIOD_PWM_1, 2) |          \
                                     PIN_AFIO_AF(GPIOD_D0, 12) |            \
                                     PIN_AFIO_AF(GPIOD_D1, 12))

/*
 * GPIOE setup:
 *
 * PE0  - STEP_E2                   (output pushpull 2mhz).
 * PE1  - DIR_E2                    (output pushpull 2mhz).
 * PE2  - INT                       (input pull-up).
 * PE3  - VMOT_EN                   (output pushpull 2mhz).
 * PE4  - LED_STS_1                 (output pushpull 2mhz).
 * PE5  - FAN_0                     (output pushpull 2mhz).
 * PE6  - FAN_1                     (output pushpull 2mhz).
 * PE7  - D4                        (alternate 12).
 * PE8  - D5                        (alternate 12).
 * PE9  - D6                        (alternate 12).
 * PE10 - D7                        (alternate 12).
 * PE11 - D8                        (alternate 12).
 * PE12 - D9                        (alternate 12).
 * PE13 - D10                       (alternate 12).
 * PE14 - D11                       (alternate 12).
 * PE15 - D12                       (alternate 12).
 */
#define VAL_GPIOE_MODER             (PIN_MODE_OUTPUT(GPIOE_STEP_E2) |       \
                                     PIN_MODE_OUTPUT(GPIOE_DIR_E2) |        \
                                     PIN_MODE_INPUT(GPIOE_INT) |            \
                                     PIN_MODE_OUTPUT(GPIOE_VMOT_EN) |       \
                                     PIN_MODE_OUTPUT(GPIOE_LED_STS_1) |     \
                                     PIN_MODE_ALTERNATE(GPIOE_FAN_0) |      \
                                     PIN_MODE_ALTERNATE(GPIOE_FAN_1) |      \
                                     PIN_MODE_ALTERNATE(GPIOE_D4) |         \
                                     PIN_MODE_ALTERNATE(GPIOE_D5) |         \
                                     PIN_MODE_ALTERNATE(GPIOE_D6) |         \
                                     PIN_MODE_ALTERNATE(GPIOE_D7) |         \
                                     PIN_MODE_ALTERNATE(GPIOE_D8) |         \
                                     PIN_MODE_ALTERNATE(GPIOE_D9) |         \
                                     PIN_MODE_ALTERNATE(GPIOE_D10) |        \
                                     PIN_MODE_ALTERNATE(GPIOE_D11) |        \
                                     PIN_MODE_ALTERNATE(GPIOE_D12))
#define VAL_GPIOE_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOE_STEP_E2) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_DIR_E2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_INT) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOE_VMOT_EN) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED_STS_1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FAN_0) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FAN_1) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D4) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D5) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D6) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D7) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D8) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D9) |         \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D10) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D11) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOE_D12))
#define VAL_GPIOE_OSPEEDR           (PIN_OSPEED_2M(GPIOE_STEP_E2) |         \
                                     PIN_OSPEED_2M(GPIOE_DIR_E2) |          \
                                     PIN_OSPEED_50M(GPIOE_INT) |            \
                                     PIN_OSPEED_2M(GPIOE_VMOT_EN) |         \
                                     PIN_OSPEED_2M(GPIOE_LED_STS_1) |       \
                                     PIN_OSPEED_2M(GPIOE_FAN_0) |           \
                                     PIN_OSPEED_2M(GPIOE_FAN_1) |           \
                                     PIN_OSPEED_50M(GPIOE_D4) |             \
                                     PIN_OSPEED_50M(GPIOE_D5) |             \
                                     PIN_OSPEED_50M(GPIOE_D6) |             \
                                     PIN_OSPEED_50M(GPIOE_D7) |             \
                                     PIN_OSPEED_50M(GPIOE_D8) |             \
                                     PIN_OSPEED_50M(GPIOE_D9) |             \
                                     PIN_OSPEED_50M(GPIOE_D10) |            \
                                     PIN_OSPEED_50M(GPIOE_D11) |            \
                                     PIN_OSPEED_50M(GPIOE_D12))
#define VAL_GPIOE_PUPDR             (PIN_PUPDR_FLOATING(GPIOE_STEP_E2) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_DIR_E2) |     \
                                     PIN_PUPDR_PULLUP(GPIOE_INT) |          \
                                     PIN_PUPDR_FLOATING(GPIOE_VMOT_EN) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_LED_STS_1) |  \
                                     PIN_PUPDR_FLOATING(GPIOE_FAN_0) |      \
                                     PIN_PUPDR_FLOATING(GPIOE_FAN_1) |      \
                                     PIN_PUPDR_FLOATING(GPIOE_D4) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_D5) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_D6) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_D7) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_D8) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_D9) |         \
                                     PIN_PUPDR_FLOATING(GPIOE_D10) |        \
                                     PIN_PUPDR_FLOATING(GPIOE_D11) |        \
                                     PIN_PUPDR_FLOATING(GPIOE_D12))
#define VAL_GPIOE_ODR               (PIN_ODR_LOW(GPIOE_STEP_E2) |           \
                                     PIN_ODR_LOW(GPIOE_DIR_E2) |            \
                                     PIN_ODR_LOW(GPIOE_INT) |               \
                                     PIN_ODR_LOW(GPIOE_VMOT_EN) |           \
                                     PIN_ODR_HIGH(GPIOE_LED_STS_1) |        \
                                     PIN_ODR_LOW(GPIOE_FAN_0) |             \
                                     PIN_ODR_LOW(GPIOE_FAN_1) |             \
                                     PIN_ODR_LOW(GPIOE_D4) |                \
                                     PIN_ODR_LOW(GPIOE_D5) |                \
                                     PIN_ODR_LOW(GPIOE_D6) |                \
                                     PIN_ODR_LOW(GPIOE_D7) |                \
                                     PIN_ODR_LOW(GPIOE_D8) |                \
                                     PIN_ODR_LOW(GPIOE_D9) |                \
                                     PIN_ODR_LOW(GPIOE_D10) |               \
                                     PIN_ODR_LOW(GPIOE_D11) |               \
                                     PIN_ODR_LOW(GPIOE_D12))
#define VAL_GPIOE_AFRL              (PIN_AFIO_AF(GPIOE_STEP_E2, 0) |        \
                                     PIN_AFIO_AF(GPIOE_DIR_E2, 0) |         \
                                     PIN_AFIO_AF(GPIOE_INT, 0) |            \
                                     PIN_AFIO_AF(GPIOE_VMOT_EN, 0) |        \
                                     PIN_AFIO_AF(GPIOE_LED_STS_1, 0) |      \
                                     PIN_AFIO_AF(GPIOE_FAN_0, 3) |          \
                                     PIN_AFIO_AF(GPIOE_FAN_1, 3) |          \
                                     PIN_AFIO_AF(GPIOE_D4, 12))
#define VAL_GPIOE_AFRH              (PIN_AFIO_AF(GPIOE_D5, 12) |            \
                                     PIN_AFIO_AF(GPIOE_D6, 12) |            \
                                     PIN_AFIO_AF(GPIOE_D7, 12) |            \
                                     PIN_AFIO_AF(GPIOE_D8, 12) |            \
                                     PIN_AFIO_AF(GPIOE_D9, 12) |            \
                                     PIN_AFIO_AF(GPIOE_D10, 12) |           \
                                     PIN_AFIO_AF(GPIOE_D11, 12) |           \
                                     PIN_AFIO_AF(GPIOE_D12, 12))

/*
 * GPIOF setup:
 *
 * PF0  - SDA                       (alternate 4).
 * PF1  - SCL                       (alternate 4).
 * PF2  - PRE_CHARGE                (output pushpull 2mhz).
 * PF3  - VMOT_MEAS                 (analog).
 * PF4  - CURR_OUT                  (analog).
 * PF5  - VIN_MEAS                  (analog).
 * PF6  - TEMP_E2                   (analog).
 * PF7  - TEMP_E1                   (analog).
 * PF8  - TEMP_E0                   (analog).
 * PF9  - TEMP_HPB                  (analog).
 * PF10 - P                         (input pull-up).
 * PF11 - DIR_Y                     (output pushpull 2mhz).
 * PF12 - STEP_Y                    (output pushpull 2mhz).
 * PF13 - HOME_X                    (input floating).
 * PF14 - MB_CS0_N                  (output pushpull 25mhz).
 * PF15 - MB_CS1_N                  (output pushpull 25mhz).
 */
#define VAL_GPIOF_MODER             (PIN_MODE_ALTERNATE(GPIOF_SDA) |        \
                                     PIN_MODE_ALTERNATE(GPIOF_SCL) |        \
                                     PIN_MODE_OUTPUT(GPIOF_PRE_CHARGE) |    \
                                     PIN_MODE_ANALOG(GPIOF_VMOT_MEAS) |     \
                                     PIN_MODE_ANALOG(GPIOF_CURR_OUT) |      \
                                     PIN_MODE_ANALOG(GPIOF_VIN_MEAS) |      \
                                     PIN_MODE_ANALOG(GPIOF_TEMP_E2) |       \
                                     PIN_MODE_ANALOG(GPIOF_TEMP_E1) |       \
                                     PIN_MODE_ANALOG(GPIOF_TEMP_E0) |       \
                                     PIN_MODE_ANALOG(GPIOF_TEMP_HPB) |      \
                                     PIN_MODE_INPUT(GPIOF_PIN10) |          \
                                     PIN_MODE_OUTPUT(GPIOF_DIR_Y) |         \
                                     PIN_MODE_OUTPUT(GPIOF_STEP_Y) |        \
                                     PIN_MODE_INPUT(GPIOF_HOME_X) |         \
                                     PIN_MODE_OUTPUT(GPIOF_MB_CS0_N) |      \
                                     PIN_MODE_OUTPUT(GPIOF_MB_CS1_N))
#define VAL_GPIOF_OTYPER            (PIN_OTYPE_OPENDRAIN(GPIOF_SDA) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOF_SCL) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PRE_CHARGE) | \
                                     PIN_OTYPE_PUSHPULL(GPIOF_VMOT_MEAS) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOF_CURR_OUT) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOF_VIN_MEAS) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOF_TEMP_E2) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_TEMP_E1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_TEMP_E0) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_TEMP_HPB) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_DIR_Y) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_STEP_Y) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_HOME_X) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_MB_CS0_N) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOF_MB_CS1_N))
#define VAL_GPIOF_OSPEEDR           (PIN_OSPEED_50M(GPIOF_SDA) |            \
                                     PIN_OSPEED_50M(GPIOF_SCL) |            \
                                     PIN_OSPEED_2M(GPIOF_PRE_CHARGE) |      \
                                     PIN_OSPEED_50M(GPIOF_VMOT_MEAS) |      \
                                     PIN_OSPEED_50M(GPIOF_CURR_OUT) |       \
                                     PIN_OSPEED_50M(GPIOF_VIN_MEAS) |       \
                                     PIN_OSPEED_50M(GPIOF_TEMP_E2) |        \
                                     PIN_OSPEED_50M(GPIOF_TEMP_E1) |        \
                                     PIN_OSPEED_50M(GPIOF_TEMP_E0) |        \
                                     PIN_OSPEED_50M(GPIOF_TEMP_HPB) |       \
                                     PIN_OSPEED_2M(GPIOF_PIN10) |           \
                                     PIN_OSPEED_2M(GPIOF_DIR_Y) |           \
                                     PIN_OSPEED_2M(GPIOF_STEP_Y) |          \
                                     PIN_OSPEED_50M(GPIOF_HOME_X) |         \
                                     PIN_OSPEED_100M(GPIOF_MB_CS0_N) |       \
                                     PIN_OSPEED_25M(GPIOF_MB_CS1_N))
#define VAL_GPIOF_PUPDR             (PIN_PUPDR_FLOATING(GPIOF_SDA) |        \
                                     PIN_PUPDR_FLOATING(GPIOF_SCL) |        \
                                     PIN_PUPDR_FLOATING(GPIOF_PRE_CHARGE) | \
                                     PIN_PUPDR_FLOATING(GPIOF_VMOT_MEAS) |  \
                                     PIN_PUPDR_FLOATING(GPIOF_CURR_OUT) |   \
                                     PIN_PUPDR_FLOATING(GPIOF_VIN_MEAS) |   \
                                     PIN_PUPDR_FLOATING(GPIOF_TEMP_E2) |    \
                                     PIN_PUPDR_FLOATING(GPIOF_TEMP_E1) |    \
                                     PIN_PUPDR_FLOATING(GPIOF_TEMP_E0) |    \
                                     PIN_PUPDR_FLOATING(GPIOF_TEMP_HPB) |   \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN10) |        \
                                     PIN_PUPDR_FLOATING(GPIOF_DIR_Y) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_STEP_Y) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_HOME_X) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_MB_CS0_N) |   \
                                     PIN_PUPDR_FLOATING(GPIOF_MB_CS1_N))
#define VAL_GPIOF_ODR               (PIN_ODR_HIGH(GPIOF_SDA) |              \
                                     PIN_ODR_HIGH(GPIOF_SCL) |              \
                                     PIN_ODR_LOW(GPIOF_PRE_CHARGE) |        \
                                     PIN_ODR_LOW(GPIOF_VMOT_MEAS) |         \
                                     PIN_ODR_LOW(GPIOF_CURR_OUT) |          \
                                     PIN_ODR_LOW(GPIOF_VIN_MEAS) |          \
                                     PIN_ODR_LOW(GPIOF_TEMP_E2) |           \
                                     PIN_ODR_LOW(GPIOF_TEMP_E1) |           \
                                     PIN_ODR_LOW(GPIOF_TEMP_E0) |           \
                                     PIN_ODR_LOW(GPIOF_TEMP_HPB) |          \
                                     PIN_ODR_LOW(GPIOF_PIN10) |             \
                                     PIN_ODR_LOW(GPIOF_DIR_Y) |             \
                                     PIN_ODR_LOW(GPIOF_STEP_Y) |            \
                                     PIN_ODR_LOW(GPIOF_HOME_X) |            \
                                     PIN_ODR_HIGH(GPIOF_MB_CS0_N) |          \
                                     PIN_ODR_LOW(GPIOF_MB_CS1_N))
#define VAL_GPIOF_AFRL              (PIN_AFIO_AF(GPIOF_SDA, 4) |            \
                                     PIN_AFIO_AF(GPIOF_SCL, 4) |            \
                                     PIN_AFIO_AF(GPIOF_PRE_CHARGE, 0) |     \
                                     PIN_AFIO_AF(GPIOF_VMOT_MEAS, 0) |      \
                                     PIN_AFIO_AF(GPIOF_CURR_OUT, 0) |       \
                                     PIN_AFIO_AF(GPIOF_VIN_MEAS, 0) |       \
                                     PIN_AFIO_AF(GPIOF_TEMP_E2, 0) |        \
                                     PIN_AFIO_AF(GPIOF_TEMP_E1, 0))
#define VAL_GPIOF_AFRH              (PIN_AFIO_AF(GPIOF_TEMP_E0, 0) |        \
                                     PIN_AFIO_AF(GPIOF_TEMP_HPB, 0) |       \
                                     PIN_AFIO_AF(GPIOF_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOF_DIR_Y, 0) |          \
                                     PIN_AFIO_AF(GPIOF_STEP_Y, 0) |         \
                                     PIN_AFIO_AF(GPIOF_HOME_X, 0) |         \
                                     PIN_AFIO_AF(GPIOF_MB_CS0_N, 0) |       \
                                     PIN_AFIO_AF(GPIOF_MB_CS1_N, 0))

/*
 * GPIOG setup:
 *
 * PG0  - DIR_X                     (output pushpull 2mhz).
 * PG1  - STEP_X                    (output pushpull 2mhz).
 * PG2  - STEP_E0                   (output pushpull 2mhz).
 * PG3  - DIR_E0                    (output pushpull 2mhz).
 * PG4  - STEP_E1                   (output pushpull 2mhz).
 * PG5  - DIR_E1                    (output pushpull 2mhz).
 * PG6  - LED_STS_2                 (output pushpull 2mhz).
 * PG7  - SCK_ST                    (alternate 8).
 * PG8  - P                         (input floating).
 * PG9  - SDO_ST                    (alternate 8).
 * PG10 - CS_N                      (input floating).
 * PG11 - ETH_TXEN                  (alternate 11).
 * PG12 - CS_ST_N                   (output pushpull 25mhz).
 * PG13 - ETH_TXD0                  (alternate 11).
 * PG14 - ETH_TXD1                  (alternate 11).
 * PG15 - P                         (input pull-up).
 */
#define VAL_GPIOG_MODER             (PIN_MODE_OUTPUT(GPIOG_DIR_X) |         \
                                     PIN_MODE_OUTPUT(GPIOG_STEP_X) |        \
                                     PIN_MODE_OUTPUT(GPIOG_STEP_E0) |       \
                                     PIN_MODE_OUTPUT(GPIOG_DIR_E0) |        \
                                     PIN_MODE_OUTPUT(GPIOG_STEP_E1) |       \
                                     PIN_MODE_OUTPUT(GPIOG_DIR_E1) |        \
                                     PIN_MODE_OUTPUT(GPIOG_LED_STS_2) |     \
                                     PIN_MODE_ALTERNATE(GPIOG_SCK_ST) |     \
                                     PIN_MODE_INPUT(GPIOG_PIN8) |           \
                                     PIN_MODE_ALTERNATE(GPIOG_SDO_ST) |     \
                                     PIN_MODE_INPUT(GPIOG_CS_N) |           \
                                     PIN_MODE_ALTERNATE(GPIOG_ETH_TXEN) |   \
                                     PIN_MODE_OUTPUT(GPIOG_CS_ST_N) |       \
                                     PIN_MODE_ALTERNATE(GPIOG_ETH_TXD0) |   \
                                     PIN_MODE_ALTERNATE(GPIOG_ETH_TXD1) |   \
                                     PIN_MODE_INPUT(GPIOG_PIN15))
#define VAL_GPIOG_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOG_DIR_X) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_STEP_X) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_STEP_E0) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOG_DIR_E0) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_STEP_E1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOG_DIR_E1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_LED_STS_2) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOG_SCK_ST) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_SDO_ST) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_CS_N) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_ETH_TXEN) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOG_CS_ST_N) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOG_ETH_TXD0) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOG_ETH_TXD1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN15))
#define VAL_GPIOG_OSPEEDR           (PIN_OSPEED_2M(GPIOG_DIR_X) |           \
                                     PIN_OSPEED_2M(GPIOG_STEP_X) |          \
                                     PIN_OSPEED_2M(GPIOG_STEP_E0) |         \
                                     PIN_OSPEED_2M(GPIOG_DIR_E0) |          \
                                     PIN_OSPEED_2M(GPIOG_STEP_E1) |         \
                                     PIN_OSPEED_2M(GPIOG_DIR_E1) |          \
                                     PIN_OSPEED_2M(GPIOG_LED_STS_2) |       \
                                     PIN_OSPEED_50M(GPIOG_SCK_ST) |         \
                                     PIN_OSPEED_50M(GPIOG_PIN8) |           \
                                     PIN_OSPEED_50M(GPIOG_SDO_ST) |         \
                                     PIN_OSPEED_50M(GPIOG_CS_N) |           \
                                     PIN_OSPEED_50M(GPIOG_ETH_TXEN) |       \
                                     PIN_OSPEED_25M(GPIOG_CS_ST_N) |        \
                                     PIN_OSPEED_50M(GPIOG_ETH_TXD0) |       \
                                     PIN_OSPEED_50M(GPIOG_ETH_TXD1) |       \
                                     PIN_OSPEED_2M(GPIOG_PIN15))
#define VAL_GPIOG_PUPDR             (PIN_PUPDR_FLOATING(GPIOG_DIR_X) |      \
                                     PIN_PUPDR_FLOATING(GPIOG_STEP_X) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_STEP_E0) |    \
                                     PIN_PUPDR_FLOATING(GPIOG_DIR_E0) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_STEP_E1) |    \
                                     PIN_PUPDR_FLOATING(GPIOG_DIR_E1) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_LED_STS_2) |  \
                                     PIN_PUPDR_FLOATING(GPIOG_SCK_ST) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN8) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_SDO_ST) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_CS_N) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_ETH_TXEN) |   \
                                     PIN_PUPDR_FLOATING(GPIOG_CS_ST_N) |    \
                                     PIN_PUPDR_FLOATING(GPIOG_ETH_TXD0) |   \
                                     PIN_PUPDR_FLOATING(GPIOG_ETH_TXD1) |   \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN15))
#define VAL_GPIOG_ODR               (PIN_ODR_LOW(GPIOG_DIR_X) |             \
                                     PIN_ODR_LOW(GPIOG_STEP_X) |            \
                                     PIN_ODR_LOW(GPIOG_STEP_E0) |           \
                                     PIN_ODR_LOW(GPIOG_DIR_E0) |            \
                                     PIN_ODR_LOW(GPIOG_STEP_E1) |           \
                                     PIN_ODR_LOW(GPIOG_DIR_E1) |            \
                                     PIN_ODR_LOW(GPIOG_LED_STS_2) |         \
                                     PIN_ODR_LOW(GPIOG_SCK_ST) |            \
                                     PIN_ODR_LOW(GPIOG_PIN8) |              \
                                     PIN_ODR_LOW(GPIOG_SDO_ST) |            \
                                     PIN_ODR_LOW(GPIOG_CS_N) |              \
                                     PIN_ODR_LOW(GPIOG_ETH_TXEN) |          \
                                     PIN_ODR_HIGH(GPIOG_CS_ST_N) |          \
                                     PIN_ODR_LOW(GPIOG_ETH_TXD0) |          \
                                     PIN_ODR_LOW(GPIOG_ETH_TXD1) |          \
                                     PIN_ODR_LOW(GPIOG_PIN15))
#define VAL_GPIOG_AFRL              (PIN_AFIO_AF(GPIOG_DIR_X, 0) |          \
                                     PIN_AFIO_AF(GPIOG_STEP_X, 0) |         \
                                     PIN_AFIO_AF(GPIOG_STEP_E0, 0) |        \
                                     PIN_AFIO_AF(GPIOG_DIR_E0, 0) |         \
                                     PIN_AFIO_AF(GPIOG_STEP_E1, 0) |        \
                                     PIN_AFIO_AF(GPIOG_DIR_E1, 0) |         \
                                     PIN_AFIO_AF(GPIOG_LED_STS_2, 0) |      \
                                     PIN_AFIO_AF(GPIOG_SCK_ST, 8))
#define VAL_GPIOG_AFRH              (PIN_AFIO_AF(GPIOG_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOG_SDO_ST, 8) |         \
                                     PIN_AFIO_AF(GPIOG_CS_N, 12) |          \
                                     PIN_AFIO_AF(GPIOG_ETH_TXEN, 11) |      \
                                     PIN_AFIO_AF(GPIOG_CS_ST_N, 0) |        \
                                     PIN_AFIO_AF(GPIOG_ETH_TXD0, 11) |      \
                                     PIN_AFIO_AF(GPIOG_ETH_TXD1, 11) |      \
                                     PIN_AFIO_AF(GPIOG_PIN15, 0))

/*
 * GPIOH setup:
 *
 * PH0  - P                         (input pull-up).
 * PH1  - P                         (input pull-up).
 * PH2  - P                         (input pull-up).
 * PH3  - P                         (input pull-up).
 * PH4  - P                         (input pull-up).
 * PH5  - P                         (input pull-up).
 * PH6  - P                         (input pull-up).
 * PH7  - P                         (input pull-up).
 * PH8  - P                         (input pull-up).
 * PH9  - P                         (input pull-up).
 * PH10 - P                         (input pull-up).
 * PH11 - P                         (input pull-up).
 * PH12 - P                         (input pull-up).
 * PH13 - P                         (input pull-up).
 * PH14 - P                         (input pull-up).
 * PH15 - P                         (input pull-up).
 */
#define VAL_GPIOH_MODER             (PIN_MODE_INPUT(GPIOH_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN15))
#define VAL_GPIOH_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOH_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN15))
#define VAL_GPIOH_OSPEEDR           (PIN_OSPEED_2M(GPIOH_PIN0) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN1) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN2) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN3) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN4) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN5) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN6) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN7) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN8) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN9) |            \
                                     PIN_OSPEED_2M(GPIOH_PIN10) |           \
                                     PIN_OSPEED_2M(GPIOH_PIN11) |           \
                                     PIN_OSPEED_2M(GPIOH_PIN12) |           \
                                     PIN_OSPEED_2M(GPIOH_PIN13) |           \
                                     PIN_OSPEED_2M(GPIOH_PIN14) |           \
                                     PIN_OSPEED_2M(GPIOH_PIN15))
#define VAL_GPIOH_PUPDR             (PIN_PUPDR_PULLUP(GPIOH_PIN0) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN1) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN4) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN5) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN6) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN7) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN15))
#define VAL_GPIOH_ODR               (PIN_ODR_LOW(GPIOH_PIN0) |              \
                                     PIN_ODR_LOW(GPIOH_PIN1) |              \
                                     PIN_ODR_LOW(GPIOH_PIN2) |              \
                                     PIN_ODR_LOW(GPIOH_PIN3) |              \
                                     PIN_ODR_LOW(GPIOH_PIN4) |              \
                                     PIN_ODR_LOW(GPIOH_PIN5) |              \
                                     PIN_ODR_LOW(GPIOH_PIN6) |              \
                                     PIN_ODR_LOW(GPIOH_PIN7) |              \
                                     PIN_ODR_LOW(GPIOH_PIN8) |              \
                                     PIN_ODR_LOW(GPIOH_PIN9) |              \
                                     PIN_ODR_LOW(GPIOH_PIN10) |             \
                                     PIN_ODR_LOW(GPIOH_PIN11) |             \
                                     PIN_ODR_LOW(GPIOH_PIN12) |             \
                                     PIN_ODR_LOW(GPIOH_PIN13) |             \
                                     PIN_ODR_LOW(GPIOH_PIN14) |             \
                                     PIN_ODR_LOW(GPIOH_PIN15))
#define VAL_GPIOH_AFRL              (PIN_AFIO_AF(GPIOH_PIN0, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN1, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN3, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN4, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN5, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN7, 0))
#define VAL_GPIOH_AFRH              (PIN_AFIO_AF(GPIOH_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN11, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN14, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN15, 0))

/*
 * GPIOI setup:
 *
 * PI0  - P                         (input pull-up).
 * PI1  - P                         (input pull-up).
 * PI2  - P                         (input pull-up).
 * PI3  - P                         (input pull-up).
 * PI4  - P                         (input pull-up).
 * PI5  - P                         (input pull-up).
 * PI6  - P                         (input pull-up).
 * PI7  - P                         (input pull-up).
 * PI8  - P                         (input pull-up).
 * PI9  - P                         (input pull-up).
 * PI10 - P                         (input pull-up).
 * PI11 - P                         (input pull-up).
 * PI12 - P                         (input pull-up).
 * PI13 - P                         (input pull-up).
 * PI14 - P                         (input pull-up).
 * PI15 - P                         (input pull-up).
 */
#define VAL_GPIOI_MODER             (PIN_MODE_INPUT(GPIOI_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN15))
#define VAL_GPIOI_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOI_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN15))
#define VAL_GPIOI_OSPEEDR           (PIN_OSPEED_2M(GPIOI_PIN0) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN1) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN2) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN3) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN4) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN5) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN6) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN7) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN8) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN9) |            \
                                     PIN_OSPEED_2M(GPIOI_PIN10) |           \
                                     PIN_OSPEED_2M(GPIOI_PIN11) |           \
                                     PIN_OSPEED_2M(GPIOI_PIN12) |           \
                                     PIN_OSPEED_2M(GPIOI_PIN13) |           \
                                     PIN_OSPEED_2M(GPIOI_PIN14) |           \
                                     PIN_OSPEED_2M(GPIOI_PIN15))
#define VAL_GPIOI_PUPDR             (PIN_PUPDR_PULLUP(GPIOI_PIN0) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN1) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN4) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN5) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN6) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN7) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN15))
#define VAL_GPIOI_ODR               (PIN_ODR_LOW(GPIOI_PIN0) |              \
                                     PIN_ODR_LOW(GPIOI_PIN1) |              \
                                     PIN_ODR_LOW(GPIOI_PIN2) |              \
                                     PIN_ODR_LOW(GPIOI_PIN3) |              \
                                     PIN_ODR_LOW(GPIOI_PIN4) |              \
                                     PIN_ODR_LOW(GPIOI_PIN5) |              \
                                     PIN_ODR_LOW(GPIOI_PIN6) |              \
                                     PIN_ODR_LOW(GPIOI_PIN7) |              \
                                     PIN_ODR_LOW(GPIOI_PIN8) |              \
                                     PIN_ODR_LOW(GPIOI_PIN9) |              \
                                     PIN_ODR_LOW(GPIOI_PIN10) |             \
                                     PIN_ODR_LOW(GPIOI_PIN11) |             \
                                     PIN_ODR_LOW(GPIOI_PIN12) |             \
                                     PIN_ODR_LOW(GPIOI_PIN13) |             \
                                     PIN_ODR_LOW(GPIOI_PIN14) |             \
                                     PIN_ODR_LOW(GPIOI_PIN15))
#define VAL_GPIOI_AFRL              (PIN_AFIO_AF(GPIOI_PIN0, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN1, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN3, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN4, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN5, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN7, 0))
#define VAL_GPIOI_AFRH              (PIN_AFIO_AF(GPIOI_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN11, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN14, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN15, 0))


#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
