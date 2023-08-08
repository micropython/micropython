/***********************************************************************************************//**
 * \copyright
 * Copyright 2018-2022 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation
 *
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
 **************************************************************************************************/

#pragma once

#if defined(CY_USING_HAL)
#include "cyhal_pin_package.h"
#endif
#if defined(COMPONENT_CAT4) /* CAT4 does not have configurators so the BSP defines pins in a
                             * non-generated header */
#include "cybsp_pins.h"
#else
#include "cycfg.h"
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * \addtogroup group_bsp_pins Pin Mappings
 * \{
 * Macro definitions for common peripheral pins on the board.
 */

#if defined(CYBSP_USER_LED)
/**
 * \addtogroup group_bsp_pins_led LED Pins
 * \{
 * Pins connected to user LEDs on the board.
 */

#ifdef CYBSP_LED_RGB_RED
/** RGB LED - Red \def CYBSP_LED_RGB_RED
 */
#endif
#ifdef CYBSP_LED_RGB_GREEN
/** RGB LED - Green \def CYBSP_LED_RGB_GREEN
 */
#endif
#ifdef CYBSP_LED_RGB_BLUE
/** RGB LED - Blue \def CYBSP_LED_RGB_BLUE
 */
#endif
#ifdef CYBSP_USER_LED
/** User LED \def CYBSP_USER_LED
 */
#endif
#ifdef CYBSP_USER_LED1
/** User LED1 \def CYBSP_USER_LED1
 */
#endif
#ifdef CYBSP_USER_LED2
/** User LED2 \def CYBSP_USER_LED2
 */
#endif
#ifdef CYBSP_USER_LED3
/** User LED3 \def CYBSP_USER_LED3
 */
#endif
#ifdef CYBSP_USER_LED4
/** User LED 4 \def CYBSP_USER_LED4
 */
#endif
#ifdef CYBSP_USER_LED5
/** User LED 5 \def CYBSP_USER_LED5
 */
#endif
#ifdef CYBSP_USER_LED6
/** User LED 6 \def CYBSP_USER_LED6
 */
#endif
#ifdef CYBSP_USER_LED7
/** User LED 7 \def CYBSP_USER_LED7
 */
#endif
#ifdef CYBSP_USER_LED8
/** User LED 8 \def CYBSP_USER_LED8
 */
#endif
#ifdef CYBSP_USER_LED9
/** User LED 9 \def CYBSP_USER_LED9
 */
#endif
#ifdef CYBSP_USER_LED10
/** User LED 10 \def CYBSP_USER_LED10
 */
#endif
#ifdef CYBSP_LED1
/** LED 1 \def CYBSP_LED1
 */
#endif
#ifdef CYBSP_LED2
/** LED 2 \def CYBSP_LED2
 */
#endif
#ifdef CYBSP_LED3
/** LED 3 \def CYBSP_LED3
 */
#endif
#ifdef CYBSP_LED3_RGB_RED
/** LED 3: RGB LED - Red \def CYBSP_LED3_RGB_RED
 */
#endif
#ifdef CYBSP_LED3_RGB_GREEN
/** LED 3: RGB LED - Green \def CYBSP_LED3_RGB_GREEN
 */
#endif
#ifdef CYBSP_LED3_RGB_BLUE
/** LED 3: RGB LED - Blue \def CYBSP_LED3_RGB_BLUE
 */
#endif
#ifdef CYBSP_LED4
/** LED 4 \def CYBSP_LED4
 */
#endif
#ifdef CYBSP_LED5
/** LED 5 \def CYBSP_LED5
 */
#endif
#ifdef CYBSP_LED6
/** LED 6 \def CYBSP_LED6
 */
#endif
#ifdef CYBSP_LED7
/** LED 7 \def CYBSP_LED7
 */
#endif
#ifdef CYBSP_LED8
/** LED 8 \def CYBSP_LED8
 */
#endif
#ifdef CYBSP_LED9
/** LED 9 \def CYBSP_LED9
 */
#endif
#ifdef CYBSP_LED10
/** LED 10 \def CYBSP_LED10
 */
#endif
#ifdef CYBSP_LED11
/** LED 11 \def CYBSP_LED11
 */
#endif
#ifdef CYBSP_LED12
/** LED 12 \def CYBSP_LED12
 */
#endif
#ifdef CYBSP_LED13
/** LED 13 \def CYBSP_LED13
 */
#endif
#ifdef CYBSP_LED_SLD0
/** Slider LED 0 \def CYBSP_LED_SLD0
 */
#endif
#ifdef CYBSP_LED_SLD1
/** Slider LED 1 \def CYBSP_LED_SLD1
 */
#endif
#ifdef CYBSP_LED_SLD2
/** Slider LED 2 \def CYBSP_LED_SLD2
 */
#endif
#ifdef CYBSP_LED_SLD3
/** Slider LED 3 \def CYBSP_LED_SLD3
 */
#endif
#ifdef CYBSP_LED_SLD4
/** Slider LED 4 \def CYBSP_LED_SLD4
 */
#endif
#ifdef CYBSP_LED_SLD5
/** LED 10; Slider LED 5 \def CYBSP_LED_SLD5
 */
#endif
#ifdef CYBSP_LED_BTN0
/** Button LED 0 \def CYBSP_LED_BTN0
 */
#endif
#ifdef CYBSP_LED_BTN1
/** Button LED 1 \def CYBSP_LED_BTN1
 */
#endif
#ifdef CYBSP_LED_BTN2
/** Button LED 2 \def CYBSP_LED_BTN2
 */
#endif

/** \} group_bsp_pins_led */
#endif // defined(CYBSP_USER_LED)

#if defined(CYBSP_USER_BTN)
/**
 * \addtogroup group_bsp_pins_btn Button Pins
 * \{
 * Pins connected to user buttons on the board.
 */

#ifdef CYBSP_SW1
/** Switch 1 \def CYBSP_SW1
 */
#endif
#ifdef CYBSP_SW2
/** Switch 2 \def CYBSP_SW2
 */
#endif
#ifdef CYBSP_SW3
/** Switch 3 \def CYBSP_SW3
 */
#endif
#ifdef CYBSP_SW4
/** Switch 4 \def CYBSP_SW4
 */
#endif
#ifdef CYBSP_USER_BTN
/** User Button 1 \def CYBSP_USER_BTN
 */
#endif
#ifdef CYBSP_USER_BTN1
/** User Button 1 \def CYBSP_USER_BTN1
 */
#endif
#ifdef CYBSP_USER_BTN2
/** User Button 2 \def CYBSP_USER_BTN2
 */
#endif
#ifdef CYBSP_POTENTIOMETER_INPUT
/** Potentiometer input \def CYBSP_POTENTIOMETER_INPUT
 */
#endif

/** \} group_bsp_pins_btn */
#endif // defined(CYBSP_USER_BTN)

#if defined(CYBSP_DEBUG_UART_RX) || defined(CYBSP_SWDIO)
/**
 * \addtogroup group_bsp_pins_comm Communication Pins
 * \{
 * Pins associated with connections on the board for communication interfaces (UART/I2C/SPI/...)
 */

#ifdef CYBSP_DEBUG_UART_RX
/** Pin: UART RX \def CYBSP_DEBUG_UART_RX
 */
#endif
#ifdef CYBSP_DEBUG_UART_TX
/** Pin: UART TX \def CYBSP_DEBUG_UART_TX
 */
#endif
#ifdef CYBSP_I2C_SCL
/** Pin: I2C SCL \def CYBSP_I2C_SCL
 */
#endif
#ifdef CYBSP_I2C_SDA
/** Pin: I2C SDA \def CYBSP_I2C_SDA
 */
#endif
#ifdef CYBSP_SWDIO
/** Pin: SWDIO \def CYBSP_SWDIO
 */
#endif
#ifdef CYBSP_SWDCK
/** Pin: SWDCK \def CYBSP_SWDCK
 */
#endif
#ifdef CYBSP_SPI_MOSI
/** Pin: SPI MOSI \def CYBSP_SPI_MOSI
 */
#endif
#ifdef CYBSP_SPI_MISO
/** Pin: SPI MISO \def CYBSP_SPI_MISO
 */
#endif
#ifdef CYBSP_SPI_CLK
/** Pin: SPI CLK \def CYBSP_SPI_CLK
 */
#endif
#ifdef CYBSP_SPI_CS
/** Pin: SPI CS \def CYBSP_SPI_CS
 */
#endif
#ifdef CYBSP_SWO
/** Pin: SWO \def CYBSP_SWO
 */
#endif
#ifdef CYBSP_QSPI_SS
/** Pin: QUAD SPI SS \def CYBSP_QSPI_SS
 */
#endif
#ifdef CYBSP_QSPI_D3
/** Pin: QUAD SPI D3 \def CYBSP_QSPI_D3
 */
#endif
#ifdef CYBSP_QSPI_D2
/** Pin: QUAD SPI D2 \def CYBSP_QSPI_D2
 */
#endif
#ifdef CYBSP_QSPI_D1
/** Pin: QUAD SPI D1 \def CYBSP_QSPI_D1
 */
#endif
#ifdef CYBSP_QSPI_D0
/** Pin: QUAD SPI D0 \def CYBSP_QSPI_D0
 */
#endif
#ifdef CYBSP_QSPI_SCK
/** Pin: QUAD SPI SCK \def CYBSP_QSPI_SCK
 */
#endif
#ifdef CYBSP_WIFI_SDIO_D0
/** Pin: WIFI SDIO D0 \def CYBSP_WIFI_SDIO_D0
 */
#endif
#ifdef CYBSP_WIFI_SDIO_D1
/** Pin: WIFI SDIO D1 \def CYBSP_WIFI_SDIO_D1
 */
#endif
#ifdef CYBSP_WIFI_SDIO_D2
/** Pin: WIFI SDIO D2 \def CYBSP_WIFI_SDIO_D2
 */
#endif
#ifdef CYBSP_WIFI_SDIO_D3
/** Pin: WIFI SDIO D3 \def CYBSP_WIFI_SDIO_D3
 */
#endif
#ifdef CYBSP_WIFI_SDIO_CMD
/** Pin: WIFI SDIO CMD \def CYBSP_WIFI_SDIO_CMD
 */
#endif
#ifdef CYBSP_WIFI_SDIO_CLK
/** Pin: WIFI SDIO CLK \def CYBSP_WIFI_SDIO_CLK
 */
#endif
#ifdef CYBSP_WIFI_WL_REG_ON
/** Pin: WIFI ON \def CYBSP_WIFI_WL_REG_ON
 */
#endif
#ifdef CYBSP_WIFI_HOST_WAKE
/** Pin: WIFI Host Wakeup \def CYBSP_WIFI_HOST_WAKE
 */

/** WiFi host-wake GPIO drive mode */
#define CYBSP_WIFI_HOST_WAKE_GPIO_DM (CYHAL_GPIO_DRIVE_ANALOG)
/** WiFi host-wake IRQ event */
#define CYBSP_WIFI_HOST_WAKE_IRQ_EVENT (CYHAL_GPIO_IRQ_RISE)
#endif
#ifdef CYBSP_BT_UART_RX
/** Pin: BT UART RX \def CYBSP_BT_UART_RX
 */
#endif
#ifdef CYBSP_BT_UART_TX
/** Pin: BT UART TX \def CYBSP_BT_UART_TX
 */
#endif
#ifdef CYBSP_BT_UART_RTS
/** Pin: BT UART RTS \def CYBSP_BT_UART_RTS
 */
#endif
#ifdef CYBSP_BT_UART_CTS
/** Pin: BT UART CTS \def CYBSP_BT_UART_CTS
 */
#endif
#ifdef CYBSP_BT_POWER
/** Pin: BT Power \def CYBSP_BT_POWER
 */
#endif
#ifdef CYBSP_BT_HOST_WAKE
/** Pin: BT Host Wakeup \def CYBSP_BT_HOST_WAKE
 */
/** BT host-wake GPIO drive mode */
#define CYBSP_BT_HOST_WAKE_GPIO_DM (CYHAL_GPIO_DRIVE_NONE)
/** BT host wake IRQ event */
#define CYBSP_BT_HOST_WAKE_IRQ_EVENT (CYHAL_GPIO_IRQ_FALL)
#endif
#ifdef CYBSP_BT_DEVICE_WAKE
/** Pin: BT Device Wakeup \def CYBSP_BT_DEVICE_WAKE
 */
/** BT device wakeup GPIO drive mode */
#define CYBSP_BT_DEVICE_WAKE_GPIO_DM (CYHAL_GPIO_DRIVE_STRONG)
/** BT device wakeup polarity */
#define CYBSP_BT_DEVICE_WAKE_POLARITY (0u)
#endif
#ifdef CYBSP_PDM_CLK
/** Pin: PDM PCM CLK \def CYBSP_PDM_CLK
 */
#endif
#ifdef CYBSP_PDM_DATA
/** Pin PDM PCM DATA \def CYBSP_PDM_DATA
 */
#endif
#ifdef CYBSP_I2S_MCLK
/** Pin: I2S MCLK \def CYBSP_I2S_MCLK
 */
#endif
#ifdef CYBSP_I2S_TX_SCK
/** Pin: I2S TX SCK \def CYBSP_I2S_TX_SCK
 */
#endif
#ifdef CYBSP_I2S_TX_WS
/** Pin: I2S TX WS \def CYBSP_I2S_TX_WS
 */
#endif
#ifdef CYBSP_I2S_TX_DATA
/** Pin: I2S TX DATA \def CYBSP_I2S_TX_DATA
 */
#endif
#ifdef CYBSP_I2S_RX_SCK
/** Pin: I2S RX SCK \def CYBSP_I2S_RX_SCK
 */
#endif
#ifdef CYBSP_I2S_RX_WS
/** Pin: I2S RX WS \def CYBSP_I2S_RX_WS
 */
#endif
#ifdef CYBSP_I2S_RX_DATA
/** Pin: I2S RX DATA \def CYBSP_I2S_RX_DATA
 */
#endif
#ifdef CYBSP_DEBUG_UART_RTS
/** Pin: UART RX \def CYBSP_DEBUG_UART_RTS
 */
#endif
#ifdef CYBSP_DEBUG_UART_CTS
/** Pin: UART TX \def CYBSP_DEBUG_UART_CTS
 */
#endif
#ifdef CYBSP_UART_RX
/** Pin: UART RX \def CYBSP_UART_RX
 */
#endif
#ifdef CYBSP_UART_TX
/** Pin: UART TX \def CYBSP_UART_TX
 */
#endif
#ifdef CYBSP_TDO_SWO
/** Pin: \def CYBSP_TDO_SWO
 */
#endif
#ifdef CYBSP_TMS_SWDIO
/** Pin: \def CYBSP_TMS_SWDIO
 */
#endif
#ifdef CYBSP_SWCLK
/** Pin: \def CYBSP_SWCLK
 */
#endif

/** \} group_bsp_pins_comm */
#endif // defined(CYBSP_DEBUG_UART_RX) || defined(CYBSP_SWDIO)

#if defined(CYBSP_A0)
/**
 * \addtogroup group_bsp_pins_arduino Arduino Header Pins
 * \{
 * Pins mapped to the Arduino header on the board.
 */

#ifdef CYBSP_A0
/** Arduino A0 \def CYBSP_A0
 */
#endif
#ifdef CYBSP_A1
/** Arduino A1 \def CYBSP_A1
 */
#endif
#ifdef CYBSP_A2
/** Arduino A2 \def CYBSP_A2
 */
#endif
#ifdef CYBSP_A3
/** Arduino A3 \def CYBSP_A3
 */
#endif
#ifdef CYBSP_A4
/** Arduino A4 \def CYBSP_A4
 */
#endif
#ifdef CYBSP_A5
/** Arduino A5 \def CYBSP_A5
 */
#endif
#ifdef CYBSP_D0
/** Arduino D0 \def CYBSP_D0
 */
#endif
#ifdef CYBSP_D1
/** Arduino D1 \def CYBSP_D1
 */
#endif
#ifdef CYBSP_D2
/** Arduino D2 \def CYBSP_D2
 */
#endif
#ifdef CYBSP_D3
/** Arduino D3 \def CYBSP_D3
 */
#endif
#ifdef CYBSP_D4
/** Arduino D4 \def CYBSP_D4
 */
#endif
#ifdef CYBSP_D5
/** Arduino D5 \def CYBSP_D5
 */
#endif
#ifdef CYBSP_D6
/** Arduino D6 \def CYBSP_D6
 */
#endif
#ifdef CYBSP_D7
/** Arduino D7 \def CYBSP_D7
 */
#endif
#ifdef CYBSP_D8
/** Arduino D8 \def CYBSP_D8
 */
#endif
#ifdef CYBSP_D9
/** Arduino D9 \def CYBSP_D9
 */
#endif
#ifdef CYBSP_D10
/** Arduino D10 \def CYBSP_D10
 */
#endif
#ifdef CYBSP_D11
/** Arduino D11 \def CYBSP_D11
 */
#endif
#ifdef CYBSP_D12
/** Arduino D12 \def CYBSP_D12
 */
#endif
#ifdef CYBSP_D13
/** Arduino D13 \def CYBSP_D13
 */
#endif
#ifdef CYBSP_D14
/** Arduino D14 \def CYBSP_D14
 */
#endif
#ifdef CYBSP_D15
/** Arduino D15 \def CYBSP_D15
 */
#endif

/** \} group_bsp_pins_arduino */
#endif // defined(CYBSP_A0)

#if defined(CYBSP_J2_1)
/**
 * \addtogroup group_bsp_pins_j2 J2 Header Pins
 * \{
 * Pins mapped to the J2 header on the board.
 */

#ifdef CYBSP_J2_1
/** Infineon J2 Header pin 1 \def CYBSP_J2_1
 */
#endif
#ifdef CYBSP_J2_2
/** Infineon J2 Header pin 2 \def CYBSP_J2_2
 */
#endif
#ifdef CYBSP_J2_3
/** Infineon J2 Header pin 3 \def CYBSP_J2_3
 */
#endif
#ifdef CYBSP_J2_4
/** Infineon J2 Header pin 4 \def CYBSP_J2_4
 */
#endif
#ifdef CYBSP_J2_5
/** Infineon J2 Header pin 5 \def CYBSP_J2_5
 */
#endif
#ifdef CYBSP_J2_7
/** Infineon J2 Header pin 7 \def CYBSP_J2_7
 */
#endif
#ifdef CYBSP_J2_8
/** Infineon J2 Header pin 8 \def CYBSP_J2_8
 */
#endif
#ifdef CYBSP_J2_9
/** Infineon J2 Header pin 9 \def CYBSP_J2_9
 */
#endif
#ifdef CYBSP_J2_10
/** Infineon J2 Header pin 10 \def CYBSP_J2_10
 */
#endif
#ifdef CYBSP_J2_11
/** Infineon J2 Header pin 11 \def CYBSP_J2_11
 */
#endif
#ifdef CYBSP_J2_12
/** Infineon J2 Header pin 12 \def CYBSP_J2_12
 */
#endif
#ifdef CYBSP_J2_13
/** Infineon J2 Header pin 13 \def CYBSP_J2_13
 */
#endif
#ifdef CYBSP_J2_15
/** Infineon J2 Header pin 15 \def CYBSP_J2_15
 */
#endif
#ifdef CYBSP_J2_16
/** Infineon J2 Header pin 16 \def CYBSP_J2_16
 */
#endif
#ifdef CYBSP_J2_16
/** Infineon J2 Header pin 16 \def CYBSP_J2_16
 */
#endif
#ifdef CYBSP_J2_6
/** Infineon J2 Header pin 6 \def CYBSP_J2_6
 */
#endif
#ifdef CYBSP_J2_17
/** Infineon J2 Header pin 17 \def CYBSP_J2_17
 */
#endif
#ifdef CYBSP_J2_18
/** Infineon J2 Header pin 18 \def CYBSP_J2_18
 */
#endif
#ifdef CYBSP_J2_19
/** Infineon J2 Header pin 19 \def CYBSP_J2_19
 */
#endif
#ifdef CYBSP_J2_20
/** Infineon J2 Header pin 20 \def CYBSP_J2_20
 */
#endif
#ifdef CYBSP_J2_14
/** Infineon J2 Header pin 14 \def CYBSP_J2_14
 */
#endif

/** \} group_bsp_pins_j2 */
#endif // defined(CYBSP_J2_1)

#if defined(CYBSP_J6_1)
/**
 * \addtogroup group_bsp_pins_j6 J6 Header Pins
 * \{
 * Pins mapped to the J6 header on the board.
 */

#ifdef CYBSP_J6_1
/** Infineon J6 Header pin 1 \def CYBSP_J6_1
 */
#endif
#ifdef CYBSP_J6_2
/** Infineon J6 Header pin 2 \def CYBSP_J6_2
 */
#endif
#ifdef CYBSP_J6_3
/** Infineon J6 Header pin 3 \def CYBSP_J6_3
 */
#endif
#ifdef CYBSP_J6_4
/** Infineon J6 Header pin 4 \def CYBSP_J6_4
 */
#endif
#ifdef CYBSP_J6_5
/** Infineon J6 Header pin 5 \def CYBSP_J6_5
 */
#endif
#ifdef CYBSP_J6_6
/** Infineon J6 Header pin 6 \def CYBSP_J6_6
 */
#endif
#ifdef CYBSP_J6_7
/** Infineon J6 Header pin 7 \def CYBSP_J6_7
 */
#endif
#ifdef CYBSP_J6_8
/** Infineon J6 Header pin 8 \def CYBSP_J6_8
 */
#endif
#ifdef CYBSP_J6_9
/** Infineon J6 Header pin 9 \def CYBSP_J6_9
 */
#endif
#ifdef CYBSP_J6_10
/** Infineon J6 Header pin 10 \def CYBSP_J6_10
 */
#endif
#ifdef CYBSP_J6_11
/** Infineon J6 Header pin 11 \def CYBSP_J6_11
 */
#endif
#ifdef CYBSP_J6_12
/** Infineon J6 Header pin 12 \def CYBSP_J6_12
 */
#endif
#ifdef CYBSP_J6_13
/** Infineon J6 Header pin 13 \def CYBSP_J6_13
 */
#endif
#ifdef CYBSP_J6_14
/** Infineon J6 Header pin 14 \def CYBSP_J6_14
 */
#endif
#ifdef CYBSP_J6_15
/** Infineon J6 Header pin 15 \def CYBSP_J6_15
 */
#endif
#ifdef CYBSP_J6_16
/** Infineon J6 Header pin 16 \def CYBSP_J6_16
 */
#endif

/** \} group_bsp_pins_j6 */
#endif // defined(CYBSP_J6_1)

#if defined(CYBSP_CMOD) || defined(CYBSP_CINA) || defined(CYBSP_CINTA)
/**
 * \addtogroup group_bsp_pins_capsense Capsense
 * \{
 * Pins connected to CAPSENSE™ sensors on the board.
 */

#ifdef CYBSP_CSD_TX
/** Pin: CapSesnse TX \def CYBSP_CSD_TX
 */
#endif
#ifdef CYBSP_CINA
/** Pin: CapSesnse CINA \def CYBSP_CINA
 */
#endif
#ifdef CYBSP_CINTA
/** Pin: CapSesnse CINTA \def CYBSP_CINTA
 */
#endif
#ifdef CYBSP_CINB
/** Pin: CapSesnse CINB \def CYBSP_CINB
 */
#endif
#ifdef CYBSP_CINTB
/** Pin: CapSesnse CINTB \def CYBSP_CINTB
 */
#endif
#ifdef CYBSP_CMOD
/** Pin: CapSesnse CMOD \def CYBSP_CMOD
 */
#endif
#ifdef CYBSP_CSD_BTN0
/** Pin: CapSesnse Button 0 \def CYBSP_CSD_BTN0
 */
#endif
#ifdef CYBSP_CSD_BTN1
/** Pin: CapSesnse Button 1 \def CYBSP_CSD_BTN1
 */
#endif
#ifdef CYBSP_CSD_SLD0
/** Pin: CapSesnse Slider 0 \def CYBSP_CSD_SLD0
 */
#endif
#ifdef CYBSP_CSD_SLD1
/** Pin: CapSesnse Slider 1 \def CYBSP_CSD_SLD1
 */
#endif
#ifdef CYBSP_CSD_SLD2
/** Pin: CapSesnse Slider 2 \def CYBSP_CSD_SLD2
 */
#endif
#ifdef CYBSP_CSD_SLD3
/** Pin: CapSesnse Slider 3 \def CYBSP_CSD_SLD3
 */
#endif
#ifdef CYBSP_CSD_SLD4
/** Pin: CapSesnse Slider 4 \def CYBSP_CSD_SLD4
 */
#endif
#ifdef CYBSP_CSD_SLD5
/** Pin: CapSesnse Slider 5 \def CYBSP_CSD_SLD5
 */
#endif
#ifdef CYBSP_CSX_BTN_TX
/** Pin: CapSesnse Button TX \def CYBSP_CSX_BTN_TX
 */
#endif
#ifdef CYBSP_CSX_BTN0
/** Pin: CapSesnse Button 0 \def CYBSP_CSX_BTN0
 */
#endif
#ifdef CYBSP_CSX_BTN1
/** Pin: CapSesnse Button 1 \def CYBSP_CSX_BTN1
 */
#endif
#ifdef CYBSP_CSX_BTN2
/** Pin: CapSesnse Button 2 \def CYBSP_CSX_BTN2
 */
#endif

/** \} group_bsp_pins_capsense */
#endif // defined(CYBSP_CMOD) || defined(CYBSP_CINA) || defined(CYBSP_CINTA)

#if defined(CYBSP_WCO_IN)
/**
 * \addtogroup group_bsp_pins_wco WCO
 * \{
 * Pins connected to the WCO on the board.
 */
#ifdef CYBSP_WCO_IN
/** Pin: WCO input \def CYBSP_WCO_IN
 */
#endif
#ifdef CYBSP_WCO_OUT
/** Pin: WCO output \def CYBSP_WCO_OUT
 */
#endif

/** \} group_bsp_pins_wco */
#endif // defined(CYBSP_WCO_IN)

/** \} group_bsp_pins */

#if defined(__cplusplus)
}
#endif
