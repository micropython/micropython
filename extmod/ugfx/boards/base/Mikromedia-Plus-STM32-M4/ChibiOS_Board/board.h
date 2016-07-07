/*
    ChibiOS - Copyright (C) 2006-2014 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for ST_MIKROMEDIA_PLUS_STM32 board.
 */

/*
 * Board identifier.
 */
#define BOARD_ST_MIKROMEDIA_PLUS_STM32
#define BOARD_NAME                  "ST_MIKROMEDIA_PLUS_STM32"

/*
 * Ethernet PHY type.
 */
#define BOARD_PHY_ID                MII_LAN8720_ID
#define BOARD_PHY_RMII

/*
 * Board oscillators-related settings.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                32768
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                25000000
#endif

/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD                   330

/*
 * MCU type as defined in the ST header.
 */
#define STM32F407xx

/*
 * IO pins assignments.
 */
#define GPIOA_TP_INT                0
#define GPIOA_LAN_SCL               1
#define GPIOA_LAN_MDIO              2
#define GPIOA_HDR_UART1_RX          3
#define GPIOA_HDR_AN0               4
#define GPIOA_SPI1_SCK              5
#define GPIOA_SPI1_MISO             6
#define GPIOA_LAN_CRS_DV            7
#define GPIOA_LAN_CLK               8
#define GPIOA_USB_VBUS              9
#define GPIOA_USB_ID                10
#define GPIOA_USB_DN                11
#define GPIOA_USB_DP                12
#define GPIOA_SWDIO                 13
#define GPIOA_SWCLK                 14
#define GPIOA_HDR_SPI1_SS           15

#define GPIOB_HDR_AN1               0
#define GPIOB_HDR_AN2               1
#define GPIOB_GND                   2
#define GPIOB_LED_G                 3
#define GPIOB_LED_B                 4
#define GPIOB_SPI1_MOSI             5
#define GPIOB_I2C1_SCL              6
#define GPIOB_I2C1_SDA              7
#define GPIOB_BUZZER                8
#define GPIOB_ACCEL_INT             9
#define GPIOB_RC5_TX                10
#define GPIOB_FLASH_CS              11
#define GPIOB_HDR_SPI2_SS           12
#define GPIOB_SPI2_SCK              13
#define GPIOB_SPI2_MISO             14
#define GPIOB_SPI2_MOSI             15

#define GPIOC_HDR_AN3               0
#define GPIOC_LAN_MDC               1
#define GPIOC_HDR_AN4               2
#define GPIOC_HDR_AN5               3
#define GPIOC_LAN_RXD0              4
#define GPIOC_LAN_RXD1              5
#define GPIOC_HDR_UART2_TX          6
#define GPIOC_HDR_UART2_RX          7
#define GPIOC_SD_DAT0               8
#define GPIOC_SD_DAT1               9
#define GPIOC_SD_DAT2               10
#define GPIOC_SD_DAT3               11
#define GPIOC_SD_CLK                12
#define GPIOC_HDR_GPIO2             13
#define GPIOC_OSC32_IN              14
#define GPIOC_OSC32_OUT             15

#define GPIOD_HDR_CAN_RX            0
#define GPIOD_HDR_CAN_TX            1
#define GPIOD_SD_CMD                2
#define GPIOD_SD_CD                 3
#define GPIOD_TFT_TE                4
#define GPIOD_HDR_UART1_TX          5
#define GPIOD_USB_PSW               6
#define GPIOD_USB_VBUS_ER           7
#define GPIOD_MP3_RST               8
#define GPIOD_MP3_DREQ              9
#define GPIOD_MP3_DCS               10
#define GPIOD_MP3_CS                11
#define GPIOD_HDR_PWM0              12
#define GPIOD_HDR_PWM1              13
#define GPIOD_HDR_PWM2              14
#define GPIOD_HDR_PWM3              15

#define GPIOE_HDR_INT0              0
#define GPIOE_HDR_INT1              1
#define GPIOE_HDR_INT2              2
#define GPIOE_HDR_INT3              3
#define GPIOE_HDR_GPIO1             4
#define GPIOE_HDR_PWM4              5
#define GPIOE_HDR_PWM5              6
#define GPIOE_TFT_GPIO0             7
#define GPIOE_TFT_D8                8
#define GPIOE_TFT_D9                9
#define GPIOE_TFT_D10               10
#define GPIOE_TFT_D11               11
#define GPIOE_TFT_D12               12
#define GPIOE_TFT_D13               13
#define GPIOE_TFT_D14               14
#define GPIOE_TFT_D15               15

#define GPIOF_HDR_I2C2_SDA          0
#define GPIOF_HDR_I2C2_SCL          1
#define GPIOF_PIN2                  2
#define GPIOF_DC_VBUS               3
#define GPIOF_BAT_STAT              4
#define GPIOF_LAN_INT               5
#define GPIOF_AN_BAT_VSENSE         6
#define GPIOF_PIN7                  7
#define GPIOF_AN_TEMP               8
#define GPIOF_AN_PD                 9
#define GPIOF_PIN10                 10
#define GPIOF_TFT_WR                11
#define GPIOF_TFT_RD                12
#define GPIOF_TFT_CS                13
#define GPIOF_TFT_RST               14
#define GPIOF_TFT_DC                15

#define GPIOG_TFT_D0                0
#define GPIOG_TFT_D1                1
#define GPIOG_TFT_D2                2
#define GPIOG_TFT_D3                3
#define GPIOG_TFT_D4                4
#define GPIOG_TFT_D5                5
#define GPIOG_TFT_D6                6
#define GPIOG_TFT_D7                7
#define GPIOG_LAN_RST               8
#define GPIOG_RF_CS                 9
#define GPIOG_RF_INT                10
#define GPIOG_LAN_TXEN              11
#define GPIOG_RF_CE                 12
#define GPIOG_LAN_TXD0              13
#define GPIOG_LAN_TXD1              14
#define GPIOG_LED_R                 15

#define GPIOH_OSC_IN                0
#define GPIOH_OSC_OUT               1
#define GPIOH_PIN2                  2
#define GPIOH_PIN3                  3
#define GPIOH_PIN4                  4
#define GPIOH_PIN5                  5
#define GPIOH_PIN6                  6
#define GPIOH_PIN7                  7
#define GPIOH_PIN8                  8
#define GPIOH_PIN9                  9
#define GPIOH_PIN10                 10
#define GPIOH_PIN11                 11
#define GPIOH_PIN12                 12
#define GPIOH_PIN13                 13
#define GPIOH_PIN14                 14
#define GPIOH_PIN15                 15

#define GPIOI_PIN0                  0
#define GPIOI_PIN1                  1
#define GPIOI_PIN2                  2
#define GPIOI_PIN3                  3
#define GPIOI_PIN4                  4
#define GPIOI_PIN5                  5
#define GPIOI_PIN6                  6
#define GPIOI_PIN7                  7
#define GPIOI_PIN8                  8
#define GPIOI_PIN9                  9
#define GPIOI_PIN10                 10
#define GPIOI_PIN11                 11
#define GPIOI_PIN12                 12
#define GPIOI_PIN13                 13
#define GPIOI_PIN14                 14
#define GPIOI_PIN15                 15

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
 * PA0  - TP_INT                    (input floating).
 * PA1  - LAN_SCL                   (alternate 11).
 * PA2  - LAN_MDIO                  (alternate 11).
 * PA3  - HDR_UART1_RX              (alternate 7).
 * PA4  - HDR_AN0                   (input pullup).
 * PA5  - SPI1_SCK                  (alternate 5).
 * PA6  - SPI1_MISO                 (alternate 5).
 * PA7  - LAN_CRS_DV                (alternate 11).
 * PA8  - LAN_CLK                   (alternate 0).
 * PA9  - USB_VBUS                  (input floating).
 * PA10 - USB_ID                    (alternate 10).
 * PA11 - USB_DN                    (alternate 10).
 * PA12 - USB_DP                    (alternate 10).
 * PA13 - SWDIO                     (alternate 0).
 * PA14 - SWCLK                     (alternate 0).
 * PA15 - HDR_SPI1_SS               (input pullup).
 */
#define VAL_GPIOA_MODER             (PIN_MODE_INPUT(GPIOA_TP_INT) |         \
                                     PIN_MODE_ALTERNATE(GPIOA_LAN_SCL) |    \
                                     PIN_MODE_ALTERNATE(GPIOA_LAN_MDIO) |   \
                                     PIN_MODE_ALTERNATE(GPIOA_HDR_UART1_RX) |\
                                     PIN_MODE_INPUT(GPIOA_HDR_AN0) |        \
                                     PIN_MODE_ALTERNATE(GPIOA_SPI1_SCK) |   \
                                     PIN_MODE_ALTERNATE(GPIOA_SPI1_MISO) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_LAN_CRS_DV) | \
                                     PIN_MODE_ALTERNATE(GPIOA_LAN_CLK) |    \
                                     PIN_MODE_INPUT(GPIOA_USB_VBUS) |       \
                                     PIN_MODE_ALTERNATE(GPIOA_USB_ID) |     \
                                     PIN_MODE_ALTERNATE(GPIOA_USB_DN) |     \
                                     PIN_MODE_ALTERNATE(GPIOA_USB_DP) |     \
                                     PIN_MODE_ALTERNATE(GPIOA_SWDIO) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_SWCLK) |      \
                                     PIN_MODE_INPUT(GPIOA_HDR_SPI1_SS))
#define VAL_GPIOA_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOA_TP_INT) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_LAN_SCL) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_LAN_MDIO) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOA_HDR_UART1_RX) |\
                                     PIN_OTYPE_PUSHPULL(GPIOA_HDR_AN0) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SPI1_SCK) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SPI1_MISO) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_LAN_CRS_DV) | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_LAN_CLK) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USB_VBUS) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USB_ID) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USB_DN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USB_DP) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWDIO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWCLK) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_HDR_SPI1_SS))
#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_100M(GPIOA_TP_INT) |        \
                                     PIN_OSPEED_100M(GPIOA_LAN_SCL) |       \
                                     PIN_OSPEED_100M(GPIOA_LAN_MDIO) |      \
                                     PIN_OSPEED_100M(GPIOA_HDR_UART1_RX) |  \
                                     PIN_OSPEED_100M(GPIOA_HDR_AN0) |       \
                                     PIN_OSPEED_50M(GPIOA_SPI1_SCK) |       \
                                     PIN_OSPEED_50M(GPIOA_SPI1_MISO) |      \
                                     PIN_OSPEED_50M(GPIOA_LAN_CRS_DV) |     \
                                     PIN_OSPEED_100M(GPIOA_LAN_CLK) |       \
                                     PIN_OSPEED_100M(GPIOA_USB_VBUS) |      \
                                     PIN_OSPEED_100M(GPIOA_USB_ID) |        \
                                     PIN_OSPEED_100M(GPIOA_USB_DN) |        \
                                     PIN_OSPEED_100M(GPIOA_USB_DP) |        \
                                     PIN_OSPEED_100M(GPIOA_SWDIO) |         \
                                     PIN_OSPEED_100M(GPIOA_SWCLK) |         \
                                     PIN_OSPEED_100M(GPIOA_HDR_SPI1_SS))
#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_TP_INT) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_LAN_SCL) |    \
                                     PIN_PUPDR_PULLUP(GPIOA_LAN_MDIO) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_HDR_UART1_RX) |\
                                     PIN_PUPDR_PULLUP(GPIOA_HDR_AN0) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_SPI1_SCK) |   \
                                     PIN_PUPDR_FLOATING(GPIOA_SPI1_MISO) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_LAN_CRS_DV) | \
                                     PIN_PUPDR_FLOATING(GPIOA_LAN_CLK) |    \
                                     PIN_PUPDR_FLOATING(GPIOA_USB_VBUS) |   \
                                     PIN_PUPDR_FLOATING(GPIOA_USB_ID) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_USB_DN) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_USB_DP) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_SWDIO) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_SWCLK) |      \
                                     PIN_PUPDR_PULLUP(GPIOA_HDR_SPI1_SS))
#define VAL_GPIOA_ODR               (PIN_ODR_HIGH(GPIOA_TP_INT) |           \
                                     PIN_ODR_HIGH(GPIOA_LAN_SCL) |          \
                                     PIN_ODR_HIGH(GPIOA_LAN_MDIO) |         \
                                     PIN_ODR_HIGH(GPIOA_HDR_UART1_RX) |     \
                                     PIN_ODR_HIGH(GPIOA_HDR_AN0) |          \
                                     PIN_ODR_HIGH(GPIOA_SPI1_SCK) |         \
                                     PIN_ODR_HIGH(GPIOA_SPI1_MISO) |        \
                                     PIN_ODR_HIGH(GPIOA_LAN_CRS_DV) |       \
                                     PIN_ODR_HIGH(GPIOA_LAN_CLK) |          \
                                     PIN_ODR_HIGH(GPIOA_USB_VBUS) |         \
                                     PIN_ODR_HIGH(GPIOA_USB_ID) |           \
                                     PIN_ODR_HIGH(GPIOA_USB_DN) |           \
                                     PIN_ODR_HIGH(GPIOA_USB_DP) |           \
                                     PIN_ODR_HIGH(GPIOA_SWDIO) |            \
                                     PIN_ODR_HIGH(GPIOA_SWCLK) |            \
                                     PIN_ODR_HIGH(GPIOA_HDR_SPI1_SS))
#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_TP_INT, 0) |         \
                                     PIN_AFIO_AF(GPIOA_LAN_SCL, 11) |       \
                                     PIN_AFIO_AF(GPIOA_LAN_MDIO, 11) |      \
                                     PIN_AFIO_AF(GPIOA_HDR_UART1_RX, 7) |   \
                                     PIN_AFIO_AF(GPIOA_HDR_AN0, 0) |        \
                                     PIN_AFIO_AF(GPIOA_SPI1_SCK, 5) |       \
                                     PIN_AFIO_AF(GPIOA_SPI1_MISO, 5) |      \
                                     PIN_AFIO_AF(GPIOA_LAN_CRS_DV, 11))
#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_LAN_CLK, 0) |        \
                                     PIN_AFIO_AF(GPIOA_USB_VBUS, 0) |       \
                                     PIN_AFIO_AF(GPIOA_USB_ID, 10) |        \
                                     PIN_AFIO_AF(GPIOA_USB_DN, 10) |        \
                                     PIN_AFIO_AF(GPIOA_USB_DP, 10) |        \
                                     PIN_AFIO_AF(GPIOA_SWDIO, 0) |          \
                                     PIN_AFIO_AF(GPIOA_SWCLK, 0) |          \
                                     PIN_AFIO_AF(GPIOA_HDR_SPI1_SS, 0))

/*
 * GPIOB setup:
 *
 * PB0  - HDR_AN1                   (input pullup).
 * PB1  - HDR_AN2                   (input pullup).
 * PB2  - GND                       (input floating).
 * PB3  - LED_G                     (output pushpull maximum).
 * PB4  - LED_B                     (output pushpull maximum).
 * PB5  - SPI1_MOSI                 (alternate 5).
 * PB6  - I2C1_SCL                  (alternate 4).
 * PB7  - I2C1_SDA                  (alternate 4).
 * PB8  - BUZZER                    (output pushpull maximum).
 * PB9  - ACCEL_INT                 (input floating).
 * PB10 - RC5_TX                    (alternate 7).
 * PB11 - FLASH_CS                  (output pushpull maximum).
 * PB12 - HDR_SPI2_SS               (alternate 5).
 * PB13 - SPI2_SCK                  (alternate 5).
 * PB14 - SPI2_MISO                 (alternate 5).
 * PB15 - SPI2_MOSI                 (alternate 5).
 */
#define VAL_GPIOB_MODER             (PIN_MODE_INPUT(GPIOB_HDR_AN1) |        \
                                     PIN_MODE_INPUT(GPIOB_HDR_AN2) |        \
                                     PIN_MODE_INPUT(GPIOB_GND) |            \
                                     PIN_MODE_OUTPUT(GPIOB_LED_G) |         \
                                     PIN_MODE_OUTPUT(GPIOB_LED_B) |         \
                                     PIN_MODE_ALTERNATE(GPIOB_SPI1_MOSI) |  \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SCL) |   \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SDA) |   \
                                     PIN_MODE_OUTPUT(GPIOB_BUZZER) |        \
                                     PIN_MODE_INPUT(GPIOB_ACCEL_INT) |      \
                                     PIN_MODE_ALTERNATE(GPIOB_RC5_TX) |     \
                                     PIN_MODE_OUTPUT(GPIOB_FLASH_CS) |      \
                                     PIN_MODE_ALTERNATE(GPIOB_HDR_SPI2_SS) |\
                                     PIN_MODE_ALTERNATE(GPIOB_SPI2_SCK) |   \
                                     PIN_MODE_ALTERNATE(GPIOB_SPI2_MISO) |  \
                                     PIN_MODE_ALTERNATE(GPIOB_SPI2_MOSI))
#define VAL_GPIOB_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOB_HDR_AN1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_HDR_AN2) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_GND) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOB_LED_G) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_LED_B) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI1_MOSI) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOB_I2C1_SCL) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOB_I2C1_SDA) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOB_BUZZER) |     \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_ACCEL_INT) | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_RC5_TX) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_FLASH_CS) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOB_HDR_SPI2_SS) |\
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI2_SCK) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI2_MISO) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI2_MOSI))
#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_100M(GPIOB_HDR_AN1) |       \
                                     PIN_OSPEED_100M(GPIOB_HDR_AN2) |       \
                                     PIN_OSPEED_100M(GPIOB_GND) |           \
                                     PIN_OSPEED_100M(GPIOB_LED_G) |         \
                                     PIN_OSPEED_100M(GPIOB_LED_B) |         \
                                     PIN_OSPEED_100M(GPIOB_SPI1_MOSI) |     \
                                     PIN_OSPEED_100M(GPIOB_I2C1_SCL) |      \
                                     PIN_OSPEED_100M(GPIOB_I2C1_SDA) |      \
                                     PIN_OSPEED_100M(GPIOB_BUZZER) |        \
                                     PIN_OSPEED_100M(GPIOB_ACCEL_INT) |     \
                                     PIN_OSPEED_100M(GPIOB_RC5_TX) |        \
                                     PIN_OSPEED_100M(GPIOB_FLASH_CS) |      \
                                     PIN_OSPEED_100M(GPIOB_HDR_SPI2_SS) |   \
                                     PIN_OSPEED_100M(GPIOB_SPI2_SCK) |      \
                                     PIN_OSPEED_100M(GPIOB_SPI2_MISO) |     \
                                     PIN_OSPEED_100M(GPIOB_SPI2_MOSI))
#define VAL_GPIOB_PUPDR             (PIN_PUPDR_PULLUP(GPIOB_HDR_AN1) |      \
                                     PIN_PUPDR_PULLUP(GPIOB_HDR_AN2) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_GND) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_LED_G) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_LED_B) |      \
                                     PIN_PUPDR_PULLUP(GPIOB_SPI1_MOSI) |    \
                                     PIN_PUPDR_PULLUP(GPIOB_I2C1_SCL) |     \
                                     PIN_PUPDR_PULLUP(GPIOB_I2C1_SDA) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_BUZZER) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_ACCEL_INT) |  \
                                     PIN_PUPDR_FLOATING(GPIOB_RC5_TX) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_FLASH_CS) |   \
                                     PIN_PUPDR_PULLUP(GPIOB_HDR_SPI2_SS) |  \
                                     PIN_PUPDR_PULLUP(GPIOB_SPI2_SCK) |     \
                                     PIN_PUPDR_PULLUP(GPIOB_SPI2_MISO) |    \
                                     PIN_PUPDR_PULLUP(GPIOB_SPI2_MOSI))
#define VAL_GPIOB_ODR               (PIN_ODR_HIGH(GPIOB_HDR_AN1) |          \
                                     PIN_ODR_HIGH(GPIOB_HDR_AN2) |          \
                                     PIN_ODR_HIGH(GPIOB_GND) |              \
                                     PIN_ODR_LOW(GPIOB_LED_G) |             \
                                     PIN_ODR_LOW(GPIOB_LED_B) |             \
                                     PIN_ODR_HIGH(GPIOB_SPI1_MOSI) |        \
                                     PIN_ODR_HIGH(GPIOB_I2C1_SCL) |         \
                                     PIN_ODR_HIGH(GPIOB_I2C1_SDA) |         \
                                     PIN_ODR_HIGH(GPIOB_BUZZER) |           \
                                     PIN_ODR_HIGH(GPIOB_ACCEL_INT) |        \
                                     PIN_ODR_HIGH(GPIOB_RC5_TX) |           \
                                     PIN_ODR_HIGH(GPIOB_FLASH_CS) |         \
                                     PIN_ODR_HIGH(GPIOB_HDR_SPI2_SS) |      \
                                     PIN_ODR_HIGH(GPIOB_SPI2_SCK) |         \
                                     PIN_ODR_HIGH(GPIOB_SPI2_MISO) |        \
                                     PIN_ODR_HIGH(GPIOB_SPI2_MOSI))
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_HDR_AN1, 0) |        \
                                     PIN_AFIO_AF(GPIOB_HDR_AN2, 0) |        \
                                     PIN_AFIO_AF(GPIOB_GND, 0) |            \
                                     PIN_AFIO_AF(GPIOB_LED_G, 0) |          \
                                     PIN_AFIO_AF(GPIOB_LED_B, 0) |          \
                                     PIN_AFIO_AF(GPIOB_SPI1_MOSI, 5) |      \
                                     PIN_AFIO_AF(GPIOB_I2C1_SCL, 4) |       \
                                     PIN_AFIO_AF(GPIOB_I2C1_SDA, 4))
#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_BUZZER, 0) |         \
                                     PIN_AFIO_AF(GPIOB_ACCEL_INT, 0) |      \
                                     PIN_AFIO_AF(GPIOB_RC5_TX, 7) |         \
                                     PIN_AFIO_AF(GPIOB_FLASH_CS, 0) |       \
                                     PIN_AFIO_AF(GPIOB_HDR_SPI2_SS, 5) |    \
                                     PIN_AFIO_AF(GPIOB_SPI2_SCK, 5) |       \
                                     PIN_AFIO_AF(GPIOB_SPI2_MISO, 5) |      \
                                     PIN_AFIO_AF(GPIOB_SPI2_MOSI, 5))

/*
 * GPIOC setup:
 *
 * PC0  - HDR_AN3                   (output pushpull maximum).
 * PC1  - LAN_MDC                   (alternate 11).
 * PC2  - HDR_AN4                   (input pullup).
 * PC3  - HDR_AN5                   (input pullup).
 * PC4  - LAN_RXD0                  (alternate 11).
 * PC5  - LAN_RXD1                  (alternate 11).
 * PC6  - HDR_UART2_TX              (alternate 8).
 * PC7  - HDR_UART2_RX              (alternate 8).
 * PC8  - SD_DAT0                   (alternate 12).
 * PC9  - SD_DAT1                   (alternate 12).
 * PC10 - SD_DAT2                   (alternate 12).
 * PC11 - SD_DAT3                   (alternate 12).
 * PC12 - SD_CLK                    (alternate 12).
 * PC13 - HDR_GPIO2                 (input pullup).
 * PC14 - OSC32_IN                  (input pullup).
 * PC15 - OSC32_OUT                 (input pullup).
 */
#define VAL_GPIOC_MODER             (PIN_MODE_OUTPUT(GPIOC_HDR_AN3) |       \
                                     PIN_MODE_ALTERNATE(GPIOC_LAN_MDC) |    \
                                     PIN_MODE_INPUT(GPIOC_HDR_AN4) |        \
                                     PIN_MODE_INPUT(GPIOC_HDR_AN5) |        \
                                     PIN_MODE_ALTERNATE(GPIOC_LAN_RXD0) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_LAN_RXD1) |   \
                                     PIN_MODE_ALTERNATE(GPIOC_HDR_UART2_TX) |\
                                     PIN_MODE_ALTERNATE(GPIOC_HDR_UART2_RX) |\
                                     PIN_MODE_ALTERNATE(GPIOC_SD_DAT0) |    \
                                     PIN_MODE_ALTERNATE(GPIOC_SD_DAT1) |    \
                                     PIN_MODE_ALTERNATE(GPIOC_SD_DAT2) |    \
                                     PIN_MODE_ALTERNATE(GPIOC_SD_DAT3) |    \
                                     PIN_MODE_ALTERNATE(GPIOC_SD_CLK) |     \
                                     PIN_MODE_INPUT(GPIOC_HDR_GPIO2) |      \
                                     PIN_MODE_INPUT(GPIOC_OSC32_IN) |       \
                                     PIN_MODE_INPUT(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOC_HDR_AN3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_LAN_MDC) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_HDR_AN4) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_HDR_AN5) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_LAN_RXD0) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_LAN_RXD1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_HDR_UART2_TX) |\
                                     PIN_OTYPE_PUSHPULL(GPIOC_HDR_UART2_RX) |\
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_DAT0) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_DAT1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_DAT2) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_DAT3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SD_CLK) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_HDR_GPIO2) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_100M(GPIOC_HDR_AN3) |       \
                                     PIN_OSPEED_100M(GPIOC_LAN_MDC) |       \
                                     PIN_OSPEED_100M(GPIOC_HDR_AN4) |       \
                                     PIN_OSPEED_100M(GPIOC_HDR_AN5) |       \
                                     PIN_OSPEED_100M(GPIOC_LAN_RXD0) |      \
                                     PIN_OSPEED_100M(GPIOC_LAN_RXD1) |      \
                                     PIN_OSPEED_100M(GPIOC_HDR_UART2_TX) |  \
                                     PIN_OSPEED_100M(GPIOC_HDR_UART2_RX) |  \
                                     PIN_OSPEED_100M(GPIOC_SD_DAT0) |       \
                                     PIN_OSPEED_100M(GPIOC_SD_DAT1) |       \
                                     PIN_OSPEED_100M(GPIOC_SD_DAT2) |       \
                                     PIN_OSPEED_100M(GPIOC_SD_DAT3) |       \
                                     PIN_OSPEED_100M(GPIOC_SD_CLK) |        \
                                     PIN_OSPEED_100M(GPIOC_HDR_GPIO2) |     \
                                     PIN_OSPEED_100M(GPIOC_OSC32_IN) |      \
                                     PIN_OSPEED_100M(GPIOC_OSC32_OUT))
#define VAL_GPIOC_PUPDR             (PIN_PUPDR_PULLDOWN(GPIOC_HDR_AN3) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_LAN_MDC) |    \
                                     PIN_PUPDR_PULLUP(GPIOC_HDR_AN4) |      \
                                     PIN_PUPDR_PULLUP(GPIOC_HDR_AN5) |      \
                                     PIN_PUPDR_FLOATING(GPIOC_LAN_RXD0) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_LAN_RXD1) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_HDR_UART2_TX) |\
                                     PIN_PUPDR_FLOATING(GPIOC_HDR_UART2_RX) |\
                                     PIN_PUPDR_FLOATING(GPIOC_SD_DAT0) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_SD_DAT1) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_SD_DAT2) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_SD_DAT3) |    \
                                     PIN_PUPDR_FLOATING(GPIOC_SD_CLK) |     \
                                     PIN_PUPDR_PULLUP(GPIOC_HDR_GPIO2) |    \
                                     PIN_PUPDR_PULLUP(GPIOC_OSC32_IN) |     \
                                     PIN_PUPDR_PULLUP(GPIOC_OSC32_OUT))
#define VAL_GPIOC_ODR               (PIN_ODR_HIGH(GPIOC_HDR_AN3) |          \
                                     PIN_ODR_HIGH(GPIOC_LAN_MDC) |          \
                                     PIN_ODR_HIGH(GPIOC_HDR_AN4) |          \
                                     PIN_ODR_HIGH(GPIOC_HDR_AN5) |          \
                                     PIN_ODR_HIGH(GPIOC_LAN_RXD0) |         \
                                     PIN_ODR_HIGH(GPIOC_LAN_RXD1) |         \
                                     PIN_ODR_HIGH(GPIOC_HDR_UART2_TX) |     \
                                     PIN_ODR_HIGH(GPIOC_HDR_UART2_RX) |     \
                                     PIN_ODR_HIGH(GPIOC_SD_DAT0) |          \
                                     PIN_ODR_HIGH(GPIOC_SD_DAT1) |          \
                                     PIN_ODR_HIGH(GPIOC_SD_DAT2) |          \
                                     PIN_ODR_HIGH(GPIOC_SD_DAT3) |          \
                                     PIN_ODR_HIGH(GPIOC_SD_CLK) |           \
                                     PIN_ODR_HIGH(GPIOC_HDR_GPIO2) |        \
                                     PIN_ODR_HIGH(GPIOC_OSC32_IN) |         \
                                     PIN_ODR_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOC_HDR_AN3, 0) |        \
                                     PIN_AFIO_AF(GPIOC_LAN_MDC, 11) |       \
                                     PIN_AFIO_AF(GPIOC_HDR_AN4, 0) |        \
                                     PIN_AFIO_AF(GPIOC_HDR_AN5, 0) |        \
                                     PIN_AFIO_AF(GPIOC_LAN_RXD0, 11) |      \
                                     PIN_AFIO_AF(GPIOC_LAN_RXD1, 11) |      \
                                     PIN_AFIO_AF(GPIOC_HDR_UART2_TX, 8) |   \
                                     PIN_AFIO_AF(GPIOC_HDR_UART2_RX, 8))
#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_SD_DAT0, 12) |       \
                                     PIN_AFIO_AF(GPIOC_SD_DAT1, 12) |       \
                                     PIN_AFIO_AF(GPIOC_SD_DAT2, 12) |       \
                                     PIN_AFIO_AF(GPIOC_SD_DAT3, 12) |       \
                                     PIN_AFIO_AF(GPIOC_SD_CLK, 12) |        \
                                     PIN_AFIO_AF(GPIOC_HDR_GPIO2, 0) |      \
                                     PIN_AFIO_AF(GPIOC_OSC32_IN, 0) |       \
                                     PIN_AFIO_AF(GPIOC_OSC32_OUT, 0))

/*
 * GPIOD setup:
 *
 * PD0  - HDR_CAN_RX                (alternate 9).
 * PD1  - HDR_CAN_TX                (alternate 9).
 * PD2  - SD_CMD                    (alternate 12).
 * PD3  - SD_CD                     (output pushpull maximum).
 * PD4  - TFT_TE                    (input pullup).
 * PD5  - HDR_UART1_TX              (alternate 7).
 * PD6  - USB_PSW                   (output opendrain maximum).
 * PD7  - USB_VBUS_ER               (input floating).
 * PD8  - MP3_RST                   (output pushpull maximum).
 * PD9  - MP3_DREQ                  (input pullup).
 * PD10 - MP3_DCS                   (output pushpull maximum).
 * PD11 - MP3_CS                    (output pushpull maximum).
 * PD12 - HDR_PWM0                  (alternate 2).
 * PD13 - HDR_PWM1                  (output pushpull maximum).
 * PD14 - HDR_PWM2                  (alternate 2).
 * PD15 - HDR_PWM3                  (output pushpull maximum).
 */
#define VAL_GPIOD_MODER             (PIN_MODE_ALTERNATE(GPIOD_HDR_CAN_RX) | \
                                     PIN_MODE_ALTERNATE(GPIOD_HDR_CAN_TX) | \
                                     PIN_MODE_ALTERNATE(GPIOD_SD_CMD) |     \
                                     PIN_MODE_OUTPUT(GPIOD_SD_CD) |         \
                                     PIN_MODE_INPUT(GPIOD_TFT_TE) |         \
                                     PIN_MODE_ALTERNATE(GPIOD_HDR_UART1_TX) |\
                                     PIN_MODE_OUTPUT(GPIOD_USB_PSW) |       \
                                     PIN_MODE_INPUT(GPIOD_USB_VBUS_ER) |    \
                                     PIN_MODE_OUTPUT(GPIOD_MP3_RST) |       \
                                     PIN_MODE_INPUT(GPIOD_MP3_DREQ) |       \
                                     PIN_MODE_OUTPUT(GPIOD_MP3_DCS) |       \
                                     PIN_MODE_OUTPUT(GPIOD_MP3_CS) |        \
                                     PIN_MODE_ALTERNATE(GPIOD_HDR_PWM0) |   \
                                     PIN_MODE_OUTPUT(GPIOD_HDR_PWM1) |      \
                                     PIN_MODE_ALTERNATE(GPIOD_HDR_PWM2) |   \
                                     PIN_MODE_OUTPUT(GPIOD_HDR_PWM3))
#define VAL_GPIOD_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOD_HDR_CAN_RX) | \
                                     PIN_OTYPE_PUSHPULL(GPIOD_HDR_CAN_TX) | \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SD_CMD) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SD_CD) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_TFT_TE) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_HDR_UART1_TX) |\
                                     PIN_OTYPE_OPENDRAIN(GPIOD_USB_PSW) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_USB_VBUS_ER) |\
                                     PIN_OTYPE_PUSHPULL(GPIOD_MP3_RST) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_MP3_DREQ) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_MP3_DCS) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_MP3_CS) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOD_HDR_PWM0) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_HDR_PWM1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_HDR_PWM2) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_HDR_PWM3))
#define VAL_GPIOD_OSPEEDR           (PIN_OSPEED_100M(GPIOD_HDR_CAN_RX) |    \
                                     PIN_OSPEED_100M(GPIOD_HDR_CAN_TX) |    \
                                     PIN_OSPEED_100M(GPIOD_SD_CMD) |        \
                                     PIN_OSPEED_100M(GPIOD_SD_CD) |         \
                                     PIN_OSPEED_100M(GPIOD_TFT_TE) |        \
                                     PIN_OSPEED_100M(GPIOD_HDR_UART1_TX) |  \
                                     PIN_OSPEED_100M(GPIOD_USB_PSW) |       \
                                     PIN_OSPEED_100M(GPIOD_USB_VBUS_ER) |   \
                                     PIN_OSPEED_100M(GPIOD_MP3_RST) |       \
                                     PIN_OSPEED_100M(GPIOD_MP3_DREQ) |      \
                                     PIN_OSPEED_100M(GPIOD_MP3_DCS) |       \
                                     PIN_OSPEED_100M(GPIOD_MP3_CS) |        \
                                     PIN_OSPEED_100M(GPIOD_HDR_PWM0) |      \
                                     PIN_OSPEED_100M(GPIOD_HDR_PWM1) |      \
                                     PIN_OSPEED_100M(GPIOD_HDR_PWM2) |      \
                                     PIN_OSPEED_100M(GPIOD_HDR_PWM3))
#define VAL_GPIOD_PUPDR             (PIN_PUPDR_FLOATING(GPIOD_HDR_CAN_RX) | \
                                     PIN_PUPDR_FLOATING(GPIOD_HDR_CAN_TX) | \
                                     PIN_PUPDR_FLOATING(GPIOD_SD_CMD) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_SD_CD) |      \
                                     PIN_PUPDR_PULLUP(GPIOD_TFT_TE) |       \
                                     PIN_PUPDR_FLOATING(GPIOD_HDR_UART1_TX) |\
                                     PIN_PUPDR_FLOATING(GPIOD_USB_PSW) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_USB_VBUS_ER) |\
                                     PIN_PUPDR_FLOATING(GPIOD_MP3_RST) |    \
                                     PIN_PUPDR_PULLUP(GPIOD_MP3_DREQ) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_MP3_DCS) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_MP3_CS) |     \
                                     PIN_PUPDR_FLOATING(GPIOD_HDR_PWM0) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_HDR_PWM1) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_HDR_PWM2) |   \
                                     PIN_PUPDR_PULLDOWN(GPIOD_HDR_PWM3))
#define VAL_GPIOD_ODR               (PIN_ODR_HIGH(GPIOD_HDR_CAN_RX) |       \
                                     PIN_ODR_HIGH(GPIOD_HDR_CAN_TX) |       \
                                     PIN_ODR_HIGH(GPIOD_SD_CMD) |           \
                                     PIN_ODR_HIGH(GPIOD_SD_CD) |            \
                                     PIN_ODR_HIGH(GPIOD_TFT_TE) |           \
                                     PIN_ODR_HIGH(GPIOD_HDR_UART1_TX) |     \
                                     PIN_ODR_HIGH(GPIOD_USB_PSW) |          \
                                     PIN_ODR_HIGH(GPIOD_USB_VBUS_ER) |      \
                                     PIN_ODR_LOW(GPIOD_MP3_RST) |           \
                                     PIN_ODR_HIGH(GPIOD_MP3_DREQ) |         \
                                     PIN_ODR_LOW(GPIOD_MP3_DCS) |           \
                                     PIN_ODR_HIGH(GPIOD_MP3_CS) |           \
                                     PIN_ODR_LOW(GPIOD_HDR_PWM0) |          \
                                     PIN_ODR_LOW(GPIOD_HDR_PWM1) |          \
                                     PIN_ODR_LOW(GPIOD_HDR_PWM2) |          \
                                     PIN_ODR_HIGH(GPIOD_HDR_PWM3))
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_HDR_CAN_RX, 9) |     \
                                     PIN_AFIO_AF(GPIOD_HDR_CAN_TX, 9) |     \
                                     PIN_AFIO_AF(GPIOD_SD_CMD, 12) |        \
                                     PIN_AFIO_AF(GPIOD_SD_CD, 0) |          \
                                     PIN_AFIO_AF(GPIOD_TFT_TE, 0) |         \
                                     PIN_AFIO_AF(GPIOD_HDR_UART1_TX, 7) |   \
                                     PIN_AFIO_AF(GPIOD_USB_PSW, 0) |        \
                                     PIN_AFIO_AF(GPIOD_USB_VBUS_ER, 0))
#define VAL_GPIOD_AFRH              (PIN_AFIO_AF(GPIOD_MP3_RST, 0) |        \
                                     PIN_AFIO_AF(GPIOD_MP3_DREQ, 0) |       \
                                     PIN_AFIO_AF(GPIOD_MP3_DCS, 0) |        \
                                     PIN_AFIO_AF(GPIOD_MP3_CS, 0) |         \
                                     PIN_AFIO_AF(GPIOD_HDR_PWM0, 2) |       \
                                     PIN_AFIO_AF(GPIOD_HDR_PWM1, 0) |       \
                                     PIN_AFIO_AF(GPIOD_HDR_PWM2, 2) |       \
                                     PIN_AFIO_AF(GPIOD_HDR_PWM3, 2))

/*
 * GPIOE setup:
 *
 * PE0  - HDR_INT0                  (input pullup).
 * PE1  - HDR_INT1                  (input pullup).
 * PE2  - HDR_INT2                  (input pullup).
 * PE3  - HDR_INT3                  (output pushpull maximum).
 * PE4  - HDR_GPIO1                 (input pullup).
 * PE5  - HDR_PWM4                  (alternate 3).
 * PE6  - HDR_PWM5                  (alternate 3).
 * PE7  - TFT_GPIO0                 (input floating).
 * PE8  - TFT_D8                    (input floating).
 * PE9  - TFT_D9                    (input floating).
 * PE10 - TFT_D10                   (input floating).
 * PE11 - TFT_D11                   (input floating).
 * PE12 - TFT_D12                   (input floating).
 * PE13 - TFT_D13                   (input floating).
 * PE14 - TFT_D14                   (input floating).
 * PE15 - TFT_D15                   (input floating).
 */
#define VAL_GPIOE_MODER             (PIN_MODE_INPUT(GPIOE_HDR_INT0) |       \
                                     PIN_MODE_INPUT(GPIOE_HDR_INT1) |       \
                                     PIN_MODE_INPUT(GPIOE_HDR_INT2) |       \
                                     PIN_MODE_OUTPUT(GPIOE_HDR_INT3) |      \
                                     PIN_MODE_INPUT(GPIOE_HDR_GPIO1) |      \
                                     PIN_MODE_ALTERNATE(GPIOE_HDR_PWM4) |   \
                                     PIN_MODE_ALTERNATE(GPIOE_HDR_PWM5) |   \
                                     PIN_MODE_INPUT(GPIOE_TFT_GPIO0) |      \
                                     PIN_MODE_INPUT(GPIOE_TFT_D8) |         \
                                     PIN_MODE_INPUT(GPIOE_TFT_D9) |         \
                                     PIN_MODE_INPUT(GPIOE_TFT_D10) |        \
                                     PIN_MODE_INPUT(GPIOE_TFT_D11) |        \
                                     PIN_MODE_INPUT(GPIOE_TFT_D12) |        \
                                     PIN_MODE_INPUT(GPIOE_TFT_D13) |        \
                                     PIN_MODE_INPUT(GPIOE_TFT_D14) |        \
                                     PIN_MODE_INPUT(GPIOE_TFT_D15))
#define VAL_GPIOE_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOE_HDR_INT0) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_HDR_INT1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_HDR_INT2) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_HDR_INT3) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_HDR_GPIO1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_HDR_PWM4) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_HDR_PWM5) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TFT_GPIO0) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TFT_D8) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TFT_D9) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TFT_D10) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TFT_D11) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TFT_D12) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TFT_D13) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TFT_D14) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TFT_D15))
#define VAL_GPIOE_OSPEEDR           (PIN_OSPEED_100M(GPIOE_HDR_INT0) |      \
                                     PIN_OSPEED_100M(GPIOE_HDR_INT1) |      \
                                     PIN_OSPEED_100M(GPIOE_HDR_INT2) |      \
                                     PIN_OSPEED_100M(GPIOE_HDR_INT3) |      \
                                     PIN_OSPEED_100M(GPIOE_HDR_GPIO1) |     \
                                     PIN_OSPEED_100M(GPIOE_HDR_PWM4) |      \
                                     PIN_OSPEED_100M(GPIOE_HDR_PWM5) |      \
                                     PIN_OSPEED_100M(GPIOE_TFT_GPIO0) |     \
                                     PIN_OSPEED_100M(GPIOE_TFT_D8) |        \
                                     PIN_OSPEED_100M(GPIOE_TFT_D9) |        \
                                     PIN_OSPEED_100M(GPIOE_TFT_D10) |       \
                                     PIN_OSPEED_100M(GPIOE_TFT_D11) |       \
                                     PIN_OSPEED_100M(GPIOE_TFT_D12) |       \
                                     PIN_OSPEED_100M(GPIOE_TFT_D13) |       \
                                     PIN_OSPEED_100M(GPIOE_TFT_D14) |       \
                                     PIN_OSPEED_100M(GPIOE_TFT_D15))
#define VAL_GPIOE_PUPDR             (PIN_PUPDR_PULLUP(GPIOE_HDR_INT0) |     \
                                     PIN_PUPDR_PULLUP(GPIOE_HDR_INT1) |     \
                                     PIN_PUPDR_PULLUP(GPIOE_HDR_INT2) |     \
                                     PIN_PUPDR_PULLDOWN(GPIOE_HDR_INT3) |   \
                                     PIN_PUPDR_PULLUP(GPIOE_HDR_GPIO1) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_HDR_PWM4) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_HDR_PWM5) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_TFT_GPIO0) |  \
                                     PIN_PUPDR_FLOATING(GPIOE_TFT_D8) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_TFT_D9) |     \
                                     PIN_PUPDR_FLOATING(GPIOE_TFT_D10) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_TFT_D11) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_TFT_D12) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_TFT_D13) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_TFT_D14) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_TFT_D15))
#define VAL_GPIOE_ODR               (PIN_ODR_HIGH(GPIOE_HDR_INT0) |         \
                                     PIN_ODR_HIGH(GPIOE_HDR_INT1) |         \
                                     PIN_ODR_HIGH(GPIOE_HDR_INT2) |         \
                                     PIN_ODR_HIGH(GPIOE_HDR_INT3) |         \
                                     PIN_ODR_HIGH(GPIOE_HDR_GPIO1) |        \
                                     PIN_ODR_HIGH(GPIOE_HDR_PWM4) |         \
                                     PIN_ODR_HIGH(GPIOE_HDR_PWM5) |         \
                                     PIN_ODR_HIGH(GPIOE_TFT_GPIO0) |        \
                                     PIN_ODR_HIGH(GPIOE_TFT_D8) |           \
                                     PIN_ODR_HIGH(GPIOE_TFT_D9) |           \
                                     PIN_ODR_HIGH(GPIOE_TFT_D10) |          \
                                     PIN_ODR_HIGH(GPIOE_TFT_D11) |          \
                                     PIN_ODR_HIGH(GPIOE_TFT_D12) |          \
                                     PIN_ODR_HIGH(GPIOE_TFT_D13) |          \
                                     PIN_ODR_HIGH(GPIOE_TFT_D14) |          \
                                     PIN_ODR_HIGH(GPIOE_TFT_D15))
#define VAL_GPIOE_AFRL              (PIN_AFIO_AF(GPIOE_HDR_INT0, 0) |       \
                                     PIN_AFIO_AF(GPIOE_HDR_INT1, 0) |       \
                                     PIN_AFIO_AF(GPIOE_HDR_INT2, 0) |       \
                                     PIN_AFIO_AF(GPIOE_HDR_INT3, 0) |       \
                                     PIN_AFIO_AF(GPIOE_HDR_GPIO1, 0) |      \
                                     PIN_AFIO_AF(GPIOE_HDR_PWM4, 3) |       \
                                     PIN_AFIO_AF(GPIOE_HDR_PWM5, 3) |       \
                                     PIN_AFIO_AF(GPIOE_TFT_GPIO0, 0))
#define VAL_GPIOE_AFRH              (PIN_AFIO_AF(GPIOE_TFT_D8, 0) |         \
                                     PIN_AFIO_AF(GPIOE_TFT_D9, 0) |         \
                                     PIN_AFIO_AF(GPIOE_TFT_D10, 0) |        \
                                     PIN_AFIO_AF(GPIOE_TFT_D11, 0) |        \
                                     PIN_AFIO_AF(GPIOE_TFT_D12, 0) |        \
                                     PIN_AFIO_AF(GPIOE_TFT_D13, 0) |        \
                                     PIN_AFIO_AF(GPIOE_TFT_D14, 0) |        \
                                     PIN_AFIO_AF(GPIOE_TFT_D15, 0))

/*
 * GPIOF setup:
 *
 * PF0  - HDR_I2C2_SDA              (alternate 4).
 * PF1  - HDR_I2C2_SCL              (alternate 4).
 * PF2  - PIN2                      (input floating).
 * PF3  - DC_VBUS                   (input floating).
 * PF4  - BAT_STAT                  (input floating).
 * PF5  - LAN_INT                   (input pullup).
 * PF6  - AN_BAT_VSENSE             (analog).
 * PF7  - PIN7                      (input floating).
 * PF8  - AN_TEMP                   (analog).
 * PF9  - AN_PD                     (analog).
 * PF10 - PIN10                     (input floating).
 * PF11 - TFT_WR                    (output pushpull maximum).
 * PF12 - TFT_RD                    (output pushpull maximum).
 * PF13 - TFT_CS                    (output pushpull maximum).
 * PF14 - TFT_RST                   (output pushpull maximum).
 * PF15 - TFT_DC                    (output pushpull maximum).
 */
#define VAL_GPIOF_MODER             (PIN_MODE_ALTERNATE(GPIOF_HDR_I2C2_SDA) |\
                                     PIN_MODE_ALTERNATE(GPIOF_HDR_I2C2_SCL) |\
                                     PIN_MODE_INPUT(GPIOF_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOF_DC_VBUS) |        \
                                     PIN_MODE_INPUT(GPIOF_BAT_STAT) |       \
                                     PIN_MODE_INPUT(GPIOF_LAN_INT) |        \
                                     PIN_MODE_ANALOG(GPIOF_AN_BAT_VSENSE) | \
                                     PIN_MODE_INPUT(GPIOF_PIN7) |           \
                                     PIN_MODE_ANALOG(GPIOF_AN_TEMP) |       \
                                     PIN_MODE_ANALOG(GPIOF_AN_PD) |         \
                                     PIN_MODE_INPUT(GPIOF_PIN10) |          \
                                     PIN_MODE_OUTPUT(GPIOF_TFT_WR) |        \
                                     PIN_MODE_OUTPUT(GPIOF_TFT_RD) |        \
                                     PIN_MODE_OUTPUT(GPIOF_TFT_CS) |        \
                                     PIN_MODE_OUTPUT(GPIOF_TFT_RST) |       \
                                     PIN_MODE_OUTPUT(GPIOF_TFT_DC))
#define VAL_GPIOF_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOF_HDR_I2C2_SDA) |\
                                     PIN_OTYPE_PUSHPULL(GPIOF_HDR_I2C2_SCL) |\
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_DC_VBUS) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_BAT_STAT) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOF_LAN_INT) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_AN_BAT_VSENSE) |\
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_AN_TEMP) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_AN_PD) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_TFT_WR) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_TFT_RD) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_TFT_CS) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOF_TFT_RST) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_TFT_DC))
#define VAL_GPIOF_OSPEEDR           (PIN_OSPEED_100M(GPIOF_HDR_I2C2_SDA) |  \
                                     PIN_OSPEED_100M(GPIOF_HDR_I2C2_SCL) |  \
                                     PIN_OSPEED_100M(GPIOF_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOF_DC_VBUS) |       \
                                     PIN_OSPEED_100M(GPIOF_BAT_STAT) |      \
                                     PIN_OSPEED_100M(GPIOF_LAN_INT) |       \
                                     PIN_OSPEED_100M(GPIOF_AN_BAT_VSENSE) | \
                                     PIN_OSPEED_100M(GPIOF_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOF_AN_TEMP) |       \
                                     PIN_OSPEED_100M(GPIOF_AN_PD) |         \
                                     PIN_OSPEED_100M(GPIOF_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOF_TFT_WR) |        \
                                     PIN_OSPEED_100M(GPIOF_TFT_RD) |        \
                                     PIN_OSPEED_100M(GPIOF_TFT_CS) |        \
                                     PIN_OSPEED_100M(GPIOF_TFT_RST) |       \
                                     PIN_OSPEED_100M(GPIOF_TFT_DC))
#define VAL_GPIOF_PUPDR             (PIN_PUPDR_PULLUP(GPIOF_HDR_I2C2_SDA) | \
                                     PIN_PUPDR_PULLUP(GPIOF_HDR_I2C2_SCL) | \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_DC_VBUS) |    \
                                     PIN_PUPDR_FLOATING(GPIOF_BAT_STAT) |   \
                                     PIN_PUPDR_PULLUP(GPIOF_LAN_INT) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_AN_BAT_VSENSE) |\
                                     PIN_PUPDR_FLOATING(GPIOF_PIN7) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_AN_TEMP) |    \
                                     PIN_PUPDR_FLOATING(GPIOF_AN_PD) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN10) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_TFT_WR) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_TFT_RD) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_TFT_CS) |     \
                                     PIN_PUPDR_FLOATING(GPIOF_TFT_RST) |    \
                                     PIN_PUPDR_FLOATING(GPIOF_TFT_DC))
#define VAL_GPIOF_ODR               (PIN_ODR_HIGH(GPIOF_HDR_I2C2_SDA) |     \
                                     PIN_ODR_HIGH(GPIOF_HDR_I2C2_SCL) |     \
                                     PIN_ODR_HIGH(GPIOF_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOF_DC_VBUS) |          \
                                     PIN_ODR_HIGH(GPIOF_BAT_STAT) |         \
                                     PIN_ODR_HIGH(GPIOF_LAN_INT) |          \
                                     PIN_ODR_HIGH(GPIOF_AN_BAT_VSENSE) |    \
                                     PIN_ODR_HIGH(GPIOF_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOF_AN_TEMP) |          \
                                     PIN_ODR_HIGH(GPIOF_AN_PD) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOF_TFT_WR) |           \
                                     PIN_ODR_HIGH(GPIOF_TFT_RD) |           \
                                     PIN_ODR_HIGH(GPIOF_TFT_CS) |           \
                                     PIN_ODR_LOW(GPIOF_TFT_RST) |           \
                                     PIN_ODR_HIGH(GPIOF_TFT_DC))
#define VAL_GPIOF_AFRL              (PIN_AFIO_AF(GPIOF_HDR_I2C2_SDA, 4) |   \
                                     PIN_AFIO_AF(GPIOF_HDR_I2C2_SCL, 4) |   \
                                     PIN_AFIO_AF(GPIOF_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOF_DC_VBUS, 0) |        \
                                     PIN_AFIO_AF(GPIOF_BAT_STAT, 0) |       \
                                     PIN_AFIO_AF(GPIOF_LAN_INT, 0) |        \
                                     PIN_AFIO_AF(GPIOF_AN_BAT_VSENSE, 0) |  \
                                     PIN_AFIO_AF(GPIOF_PIN7, 0))
#define VAL_GPIOF_AFRH              (PIN_AFIO_AF(GPIOF_AN_TEMP, 0) |        \
                                     PIN_AFIO_AF(GPIOF_AN_PD, 0) |          \
                                     PIN_AFIO_AF(GPIOF_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOF_TFT_WR, 0) |         \
                                     PIN_AFIO_AF(GPIOF_TFT_RD, 0) |         \
                                     PIN_AFIO_AF(GPIOF_TFT_CS, 0) |         \
                                     PIN_AFIO_AF(GPIOF_TFT_RST, 0) |        \
                                     PIN_AFIO_AF(GPIOF_TFT_DC, 0))

/*
 * GPIOG setup:
 *
 * PG0  - TFT_D0                    (input floating).
 * PG1  - TFT_D1                    (input floating).
 * PG2  - TFT_D2                    (input floating).
 * PG3  - TFT_D3                    (input floating).
 * PG4  - TFT_D4                    (input floating).
 * PG5  - TFT_D5                    (input floating).
 * PG6  - TFT_D6                    (input floating).
 * PG7  - TFT_D7                    (input floating).
 * PG8  - LAN_RST                   (output pushpull maximum).
 * PG9  - RF_CS                     (output pushpull maximum).
 * PG10 - RF_INT                    (input pullup).
 * PG11 - LAN_TXEN                  (alternate 11).
 * PG12 - RF_CE                     (output pushpull maximum).
 * PG13 - LAN_TXD0                  (alternate 11).
 * PG14 - LAN_TXD1                  (alternate 11).
 * PG15 - LED_R                     (output pushpull maximum).
 */
#define VAL_GPIOG_MODER             (PIN_MODE_INPUT(GPIOG_TFT_D0) |         \
                                     PIN_MODE_INPUT(GPIOG_TFT_D1) |         \
                                     PIN_MODE_INPUT(GPIOG_TFT_D2) |         \
                                     PIN_MODE_INPUT(GPIOG_TFT_D3) |         \
                                     PIN_MODE_INPUT(GPIOG_TFT_D4) |         \
                                     PIN_MODE_INPUT(GPIOG_TFT_D5) |         \
                                     PIN_MODE_INPUT(GPIOG_TFT_D6) |         \
                                     PIN_MODE_INPUT(GPIOG_TFT_D7) |         \
                                     PIN_MODE_OUTPUT(GPIOG_LAN_RST) |       \
                                     PIN_MODE_OUTPUT(GPIOG_RF_CS) |         \
                                     PIN_MODE_INPUT(GPIOG_RF_INT) |         \
                                     PIN_MODE_ALTERNATE(GPIOG_LAN_TXEN) |   \
                                     PIN_MODE_OUTPUT(GPIOG_RF_CE) |         \
                                     PIN_MODE_ALTERNATE(GPIOG_LAN_TXD0) |   \
                                     PIN_MODE_ALTERNATE(GPIOG_LAN_TXD1) |   \
                                     PIN_MODE_OUTPUT(GPIOG_LED_R))
#define VAL_GPIOG_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOG_TFT_D0) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_TFT_D1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_TFT_D2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_TFT_D3) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_TFT_D4) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_TFT_D5) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_TFT_D6) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_TFT_D7) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_LAN_RST) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOG_RF_CS) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_RF_INT) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOG_LAN_TXEN) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOG_RF_CE) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_LAN_TXD0) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOG_LAN_TXD1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOG_LED_R))
#define VAL_GPIOG_OSPEEDR           (PIN_OSPEED_100M(GPIOG_TFT_D0) |        \
                                     PIN_OSPEED_100M(GPIOG_TFT_D1) |        \
                                     PIN_OSPEED_100M(GPIOG_TFT_D2) |        \
                                     PIN_OSPEED_100M(GPIOG_TFT_D3) |        \
                                     PIN_OSPEED_100M(GPIOG_TFT_D4) |        \
                                     PIN_OSPEED_100M(GPIOG_TFT_D5) |        \
                                     PIN_OSPEED_100M(GPIOG_TFT_D6) |        \
                                     PIN_OSPEED_100M(GPIOG_TFT_D7) |        \
                                     PIN_OSPEED_100M(GPIOG_LAN_RST) |       \
                                     PIN_OSPEED_100M(GPIOG_RF_CS) |         \
                                     PIN_OSPEED_100M(GPIOG_RF_INT) |        \
                                     PIN_OSPEED_100M(GPIOG_LAN_TXEN) |      \
                                     PIN_OSPEED_100M(GPIOG_RF_CE) |         \
                                     PIN_OSPEED_100M(GPIOG_LAN_TXD0) |      \
                                     PIN_OSPEED_100M(GPIOG_LAN_TXD1) |      \
                                     PIN_OSPEED_100M(GPIOG_LED_R))
#define VAL_GPIOG_PUPDR             (PIN_PUPDR_FLOATING(GPIOG_TFT_D0) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_TFT_D1) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_TFT_D2) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_TFT_D3) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_TFT_D4) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_TFT_D5) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_TFT_D6) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_TFT_D7) |     \
                                     PIN_PUPDR_FLOATING(GPIOG_LAN_RST) |    \
                                     PIN_PUPDR_FLOATING(GPIOG_RF_CS) |      \
                                     PIN_PUPDR_PULLUP(GPIOG_RF_INT) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_LAN_TXEN) |   \
                                     PIN_PUPDR_FLOATING(GPIOG_RF_CE) |      \
                                     PIN_PUPDR_FLOATING(GPIOG_LAN_TXD0) |   \
                                     PIN_PUPDR_FLOATING(GPIOG_LAN_TXD1) |   \
                                     PIN_PUPDR_FLOATING(GPIOG_LED_R))
#define VAL_GPIOG_ODR               (PIN_ODR_HIGH(GPIOG_TFT_D0) |           \
                                     PIN_ODR_HIGH(GPIOG_TFT_D1) |           \
                                     PIN_ODR_HIGH(GPIOG_TFT_D2) |           \
                                     PIN_ODR_HIGH(GPIOG_TFT_D3) |           \
                                     PIN_ODR_HIGH(GPIOG_TFT_D4) |           \
                                     PIN_ODR_HIGH(GPIOG_TFT_D5) |           \
                                     PIN_ODR_HIGH(GPIOG_TFT_D6) |           \
                                     PIN_ODR_HIGH(GPIOG_TFT_D7) |           \
                                     PIN_ODR_HIGH(GPIOG_LAN_RST) |          \
                                     PIN_ODR_HIGH(GPIOG_RF_CS) |            \
                                     PIN_ODR_HIGH(GPIOG_RF_INT) |           \
                                     PIN_ODR_HIGH(GPIOG_LAN_TXEN) |         \
                                     PIN_ODR_LOW(GPIOG_RF_CE) |             \
                                     PIN_ODR_HIGH(GPIOG_LAN_TXD0) |         \
                                     PIN_ODR_HIGH(GPIOG_LAN_TXD1) |         \
                                     PIN_ODR_LOW(GPIOG_LED_R))
#define VAL_GPIOG_AFRL              (PIN_AFIO_AF(GPIOG_TFT_D0, 0) |         \
                                     PIN_AFIO_AF(GPIOG_TFT_D1, 0) |         \
                                     PIN_AFIO_AF(GPIOG_TFT_D2, 0) |         \
                                     PIN_AFIO_AF(GPIOG_TFT_D3, 0) |         \
                                     PIN_AFIO_AF(GPIOG_TFT_D4, 0) |         \
                                     PIN_AFIO_AF(GPIOG_TFT_D5, 0) |         \
                                     PIN_AFIO_AF(GPIOG_TFT_D6, 0) |         \
                                     PIN_AFIO_AF(GPIOG_TFT_D7, 0))
#define VAL_GPIOG_AFRH              (PIN_AFIO_AF(GPIOG_LAN_RST, 11) |       \
                                     PIN_AFIO_AF(GPIOG_RF_CS, 0) |          \
                                     PIN_AFIO_AF(GPIOG_RF_INT, 0) |         \
                                     PIN_AFIO_AF(GPIOG_LAN_TXEN, 11) |      \
                                     PIN_AFIO_AF(GPIOG_RF_CE, 0) |          \
                                     PIN_AFIO_AF(GPIOG_LAN_TXD0, 11) |      \
                                     PIN_AFIO_AF(GPIOG_LAN_TXD1, 11) |      \
                                     PIN_AFIO_AF(GPIOG_LED_R, 0))

/*
 * GPIOH setup:
 *
 * PH0  - OSC_IN                    (input floating).
 * PH1  - OSC_OUT                   (input floating).
 * PH2  - PIN2                      (input floating).
 * PH3  - PIN3                      (input floating).
 * PH4  - PIN4                      (input floating).
 * PH5  - PIN5                      (input floating).
 * PH6  - PIN6                      (input floating).
 * PH7  - PIN7                      (input floating).
 * PH8  - PIN8                      (input floating).
 * PH9  - PIN9                      (input floating).
 * PH10 - PIN10                     (input floating).
 * PH11 - PIN11                     (input floating).
 * PH12 - PIN12                     (input floating).
 * PH13 - PIN13                     (input floating).
 * PH14 - PIN14                     (input floating).
 * PH15 - PIN15                     (input floating).
 */
#define VAL_GPIOH_MODER             (PIN_MODE_INPUT(GPIOH_OSC_IN) |         \
                                     PIN_MODE_INPUT(GPIOH_OSC_OUT) |        \
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
#define VAL_GPIOH_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT) |    \
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
#define VAL_GPIOH_OSPEEDR           (PIN_OSPEED_100M(GPIOH_OSC_IN) |        \
                                     PIN_OSPEED_100M(GPIOH_OSC_OUT) |       \
                                     PIN_OSPEED_100M(GPIOH_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN3) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN4) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN5) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN11) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN14) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN15))
#define VAL_GPIOH_PUPDR             (PIN_PUPDR_FLOATING(GPIOH_OSC_IN) |     \
                                     PIN_PUPDR_FLOATING(GPIOH_OSC_OUT) |    \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN3) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN4) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN5) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN6) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN7) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN8) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN9) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN10) |      \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN11) |      \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN12) |      \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN13) |      \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN14) |      \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN15))
#define VAL_GPIOH_ODR               (PIN_ODR_HIGH(GPIOH_OSC_IN) |           \
                                     PIN_ODR_HIGH(GPIOH_OSC_OUT) |          \
                                     PIN_ODR_HIGH(GPIOH_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN15))
#define VAL_GPIOH_AFRL              (PIN_AFIO_AF(GPIOH_OSC_IN, 0) |         \
                                     PIN_AFIO_AF(GPIOH_OSC_OUT, 0) |        \
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
 * PI0  - PIN0                      (input floating).
 * PI1  - PIN1                      (input floating).
 * PI2  - PIN2                      (input floating).
 * PI3  - PIN3                      (input floating).
 * PI4  - PIN4                      (input floating).
 * PI5  - PIN5                      (input floating).
 * PI6  - PIN6                      (input floating).
 * PI7  - PIN7                      (input floating).
 * PI8  - PIN8                      (input floating).
 * PI9  - PIN9                      (input floating).
 * PI10 - PIN10                     (input floating).
 * PI11 - PIN11                     (input floating).
 * PI12 - PIN12                     (input floating).
 * PI13 - PIN13                     (input floating).
 * PI14 - PIN14                     (input floating).
 * PI15 - PIN15                     (input floating).
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
#define VAL_GPIOI_OSPEEDR           (PIN_OSPEED_100M(GPIOI_PIN0) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN1) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN3) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN4) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN5) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN11) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN14) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN15))
#define VAL_GPIOI_PUPDR             (PIN_PUPDR_FLOATING(GPIOI_PIN0) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN1) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN3) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN4) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN5) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN6) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN7) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN8) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN9) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN10) |      \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN11) |      \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN12) |      \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN13) |      \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN14) |      \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN15))
#define VAL_GPIOI_ODR               (PIN_ODR_HIGH(GPIOI_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN15))
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
