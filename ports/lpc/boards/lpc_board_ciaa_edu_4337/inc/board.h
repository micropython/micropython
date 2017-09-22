/*
 * @brief NGX Xplorer 4330 board file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"
/* board_api.h is included at the bottom of this file after DEBUG setup */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_NGX_XPLORER_4330 LPC4330 NGX Xplorer board support software API functions
 * @ingroup LPCOPEN_43XX_BOARD_NGX4330
 * The board support software API functions provide some simple abstracted
 * functions used across multiple LPCOpen board examples. See @ref BOARD_COMMON_API
 * for the functions defined by this board support layer.<br>
 * @{
 */

/** @defgroup BOARD_NGX_XPLORER_4330_OPTIONS BOARD: LPC4330 NGX Xplorer board options
 * This board has options that configure its operation at build-time.<br>
 * @{
 */

/** Define DEBUG_ENABLE to enable IO via the DEBUGSTR, DEBUGOUT, and
    DEBUGIN macros. If not defined, DEBUG* functions will be optimized
   out of the code at build time.
 */
#define DEBUG_ENABLE

/** Define DEBUG_SEMIHOSTING along with DEBUG_ENABLE to enable IO support
    via semihosting. You may need to use a C library that supports
   semihosting with this option.
 */
//#define DEBUG_SEMIHOSTING

/** Board UART used for debug output and input using the DEBUG* macros. This
    is also the port used for Board_UARTPutChar, Board_UARTGetChar, and
   Board_UARTPutSTR functions. */
#define DEBUG_UART LPC_USART2

/**
 * @}
 */

/* Board name */
#define BOARD_CIAA_EDU_NXP_4337

/* Build for RMII interface */
#define USE_RMII
#define BOARD_ENET_PHY_ADDR    0x00

#define LED_3 2
#define LED_2 1
#define LED_1 0
#define LED_RED 3
#define LED_GREEN 4
#define LED_BLUE 5

/**
 * @brief  Sets up board specific I2C interface
 * @param  id  : I2C Peripheral ID (I2C0, I2C1)
 * @return Nothing
 */
void Board_I2C_Init(I2C_ID_T id);

/**
 * @brief  Sets up I2C Fast Plus mode
 * @param  id  : Must always be I2C0
 * @return Nothing
 * @note   This function must be called before calling
 *          Chip_I2C_SetClockRate() to set clock rates above
 *          normal range 100KHz to 400KHz. Only I2C0 supports
 *          this mode.
 */
STATIC INLINE void Board_I2C_EnableFastPlus(I2C_ID_T id)
{
   Chip_SCU_I2C0PinConfig(I2C0_FAST_MODE_PLUS);
}

/**
 * @brief  Disable I2C Fast Plus mode and enables default mode
 * @param  id  : Must always be I2C0
 * @return Nothing
 * @sa     Board_I2C_EnableFastPlus()
 */
STATIC INLINE void Board_I2C_DisableFastPlus(I2C_ID_T id)
{
   Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
}

/**
 * @brief  Initializes board specific GPIO Interrupt
 * @return Nothing
 */
void Board_GPIO_Int_Init(void);

/**
 * @brief  Initialize pin muxing for SSP interface
 * @param  pSSP    : Pointer to SSP interface to initialize
 * @return Nothing
 */
void Board_SSP_Init(LPC_SSP_T *pSSP);

/**
 * @brief  Returns the MAC address assigned to this board
 * @param  mcaddr : Pointer to 6-byte character array to populate with MAC address
 * @return Nothing
 */
void Board_ENET_GetMacADDR(uint8_t *mcaddr);

/**
 * @brief  Initialize pin muxing for a UART
 * @param  pUART   : Pointer to UART register block for UART pins to init
 * @return Nothing
 */
void Board_UART_Init(LPC_USART_T *pUART);

/**
 * @brief  Initialize pin muxing for SDMMC interface
 * @return Nothing
 */
void Board_SDMMC_Init(void);

/**
 * @brief  Initialize DAC
 * @param  pDAC    : Pointer to DAC register interface used on this board
 * @return Nothing
 */
void Board_DAC_Init(LPC_DAC_T *pDAC);

/**
 * @brief  Initialize ADC
 * @return Nothing
 */
STATIC INLINE void Board_ADC_Init(void){}

/**
 * @}
 */

#include "board_api.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */
