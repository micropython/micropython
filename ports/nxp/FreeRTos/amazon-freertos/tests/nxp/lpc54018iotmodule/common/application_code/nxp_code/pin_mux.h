/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/* USB0_VBUS (coord B12), U3[4]/J3[1]/P0_22-USB0_VBUS */
#define BOARD_INITPINS_USB0_VBUS_PERIPHERAL                               USBFSH   /*!< Device name: USBFSH */
#define BOARD_INITPINS_USB0_VBUS_SIGNAL                                 USB_VBUS   /*!< USBFSH signal: USB_VBUS */
#define BOARD_INITPINS_USB0_VBUS_PIN_NAME                              USB0_VBUS   /*!< Pin name */
#define BOARD_INITPINS_USB0_VBUS_LABEL             "U3[4]/J3[1]/P0_22-USB0_VBUS"   /*!< Label */
#define BOARD_INITPINS_USB0_VBUS_NAME                                "USB0_VBUS"   /*!< Identifier name */

/* FC0_RXD_SDA_MOSI (coord B13), U24[13]/P0_29-ISP_FC0_RXD */
#define BOARD_INITPINS_ISP_FC0_RXD_PERIPHERAL                          FLEXCOMM0   /*!< Device name: FLEXCOMM0 */
#define BOARD_INITPINS_ISP_FC0_RXD_SIGNAL                           RXD_SDA_MOSI   /*!< FLEXCOMM0 signal: RXD_SDA_MOSI */
#define BOARD_INITPINS_ISP_FC0_RXD_PIN_NAME                     FC0_RXD_SDA_MOSI   /*!< Pin name */
#define BOARD_INITPINS_ISP_FC0_RXD_LABEL             "U24[13]/P0_29-ISP_FC0_RXD"   /*!< Label */
#define BOARD_INITPINS_ISP_FC0_RXD_NAME                            "ISP_FC0_RXD"   /*!< Identifier name */

/* FC0_TXD_SCL_MISO (coord A2), U24[12]/P0_30-ISP_FC0_TXD */
#define BOARD_INITPINS_ISP_FC0_TXD_PERIPHERAL                          FLEXCOMM0   /*!< Device name: FLEXCOMM0 */
#define BOARD_INITPINS_ISP_FC0_TXD_SIGNAL                           TXD_SCL_MISO   /*!< FLEXCOMM0 signal: TXD_SCL_MISO */
#define BOARD_INITPINS_ISP_FC0_TXD_PIN_NAME                     FC0_TXD_SCL_MISO   /*!< Pin name */
#define BOARD_INITPINS_ISP_FC0_TXD_LABEL             "U24[12]/P0_30-ISP_FC0_TXD"   /*!< Label */
#define BOARD_INITPINS_ISP_FC0_TXD_NAME                            "ISP_FC0_TXD"   /*!< Identifier name */

/*!
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M4F */

/* FC8_SCK (coord D2), RP1[1]/U9[8]/P3_15-SD_WPn */
#define BOARD_INITGT202SHIELD_SD_WPn_PERIPHERAL                        FLEXCOMM8   /*!< Device name: FLEXCOMM8 */
#define BOARD_INITGT202SHIELD_SD_WPn_SIGNAL                                  SCK   /*!< FLEXCOMM8 signal: SCK */
#define BOARD_INITGT202SHIELD_SD_WPn_PIN_NAME                            FC8_SCK   /*!< Pin name */
#define BOARD_INITGT202SHIELD_SD_WPn_LABEL           "RP1[1]/U9[8]/P3_15-SD_WPn"   /*!< Label */
#define BOARD_INITGT202SHIELD_SD_WPn_NAME                               "SD_WPn"   /*!< Identifier name */

/* PIO3_3 (coord A13), PWRON */
#define BOARD_INITGT202SHIELD_PWRON_GPIO                                    GPIO   /*!< GPIO device name: GPIO */
#define BOARD_INITGT202SHIELD_PWRON_PORT                                      3U   /*!< PORT device index: 3 */
#define BOARD_INITGT202SHIELD_PWRON_GPIO_PIN                                  3U   /*!< PIO3 pin index: 3 */
#define BOARD_INITGT202SHIELD_PWRON_PIN_NAME                              PIO3_3   /*!< Pin name */
#define BOARD_INITGT202SHIELD_PWRON_LABEL                                "PWRON"   /*!< Label */
#define BOARD_INITGT202SHIELD_PWRON_NAME                                 "PWRON"   /*!< Identifier name */
#define BOARD_INITGT202SHIELD_PWRON_DIRECTION           kPIN_MUX_DirectionOutput   /*!< Direction */

/* PIO1_0 (coord N3), IRQ */
#define BOARD_INITGT202SHIELD_IRQ_GPIO                                      GPIO   /*!< GPIO device name: GPIO */
#define BOARD_INITGT202SHIELD_IRQ_PORT                                        1U   /*!< PORT device index: 1 */
#define BOARD_INITGT202SHIELD_IRQ_GPIO_PIN                                    0U   /*!< PIO1 pin index: 0 */
#define BOARD_INITGT202SHIELD_IRQ_PIN_NAME                                PIO1_0   /*!< Pin name */
#define BOARD_INITGT202SHIELD_IRQ_LABEL                                    "IRQ"   /*!< Label */
#define BOARD_INITGT202SHIELD_IRQ_NAME                                     "IRQ"   /*!< Identifier name */
#define BOARD_INITGT202SHIELD_IRQ_DIRECTION              kPIN_MUX_DirectionInput   /*!< Direction */

/*!
 *
 */
void BOARD_InitGT202Shield(void); /* Function assigned for the Cortex-M4F */
void BOARD_InitSPIFI(void); /* Function assigned for the Core #0 (ARM Cortex-M4) */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
