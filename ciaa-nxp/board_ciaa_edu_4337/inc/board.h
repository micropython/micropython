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
#define BOARD_ENET_PHY_ADDR	0x00

/* LCD interface defines */
#define LCD_SSP              LPC_SSP1
#define LCD_CDM_PORT         6
#define LCD_CMD_PIN          5
#define LCD_CMD_CFG          (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | SCU_MODE_FUNC0)
#define LCD_CMD_GPIO_PORT    3
#define LCD_CMD_GPIO_PIN     4
#define LCD_BIT_RATE         1000000 /* 1 MHz */

/* Audio Codec defines */
#define I2CDEV_WM8904_ADDR     (0x34 >> 1)
#define WM8904_I2C_BUS         I2C1
#define CODEC_LINE_IN          0 /* Mic */
#define AUDCFG_SAMPLE_RATE     16000

/* For USBLIB examples */
#define LEDS_LED1           0x01
#define LEDS_LED2           0x02
#define LEDS_LED3           0x04
#define LEDS_LED4           0x08
#define LEDS_NO_LEDS        0x00
#define BUTTONS_BUTTON1     0x01
#define JOY_UP              0x01
#define JOY_DOWN            0x02
#define JOY_LEFT            0x04
#define JOY_RIGHT           0x08
#define JOY_PRESS           0x10
#define NO_BUTTON_PRESSED   0x00

/*Define if use SDCARD for Mass Storage Example*/
// #define CFG_SDCARD

#define BUTTONS_BUTTON1_GPIO_PORT_NUM   0
#define BUTTONS_BUTTON1_GPIO_BIT_NUM    7
#define LED1_GPIO_PORT_NUM              1
#define LED1_GPIO_BIT_NUM               11
#define LED2_GPIO_PORT_NUM              1
#define LED2_GPIO_BIT_NUM               12

/* USB1 VBUS Enable GPIO pins */
#define USB1_VBUS_PORT_NUM          2
#define USB1_VBUS_PIN_NUM           5
#define USB1_VBUS_PIN_CFG           (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4)
#define USB1_VBUS_GPIO_PORT_NUM     5
#define USB1_VBUS_GPIO_BIT_NUM      5

/**
 * @brief	Sets up board specific I2C interface
 * @param	id	: I2C Peripheral ID (I2C0, I2C1)
 * @return	Nothing
 */
void Board_I2C_Init(I2C_ID_T id);

/**
 * @brief	Sets up I2C Fast Plus mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @note	This function must be called before calling
 *          Chip_I2C_SetClockRate() to set clock rates above
 *          normal range 100KHz to 400KHz. Only I2C0 supports
 *          this mode.
 */
STATIC INLINE void Board_I2C_EnableFastPlus(I2C_ID_T id)
{
	Chip_SCU_I2C0PinConfig(I2C0_FAST_MODE_PLUS);
}

/**
 * @brief	Disable I2C Fast Plus mode and enables default mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @sa		Board_I2C_EnableFastPlus()
 */
STATIC INLINE void Board_I2C_DisableFastPlus(I2C_ID_T id)
{
	Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
}

/**
 * @brief	Enable VBUS to USB1 port in Host mode
 * @return	Nothing
 * @sa		Board_USB1_DisableVbus()
 */
__STATIC_INLINE void Board_USB1_EnableVbus(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, USB1_VBUS_GPIO_PORT_NUM, USB1_VBUS_GPIO_BIT_NUM);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, USB1_VBUS_GPIO_PORT_NUM, USB1_VBUS_GPIO_BIT_NUM);
}

/**
 * @brief	Disable VBUS to USB1 port
 * @return	Nothing
 * @sa		Board_USB1_EnableVbus()
 */
__STATIC_INLINE void Board_USB1_DisableVbus(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, USB1_VBUS_GPIO_PORT_NUM, USB1_VBUS_GPIO_BIT_NUM);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, USB1_VBUS_GPIO_PORT_NUM, USB1_VBUS_GPIO_BIT_NUM);
}

/**
 * @brief	Initializes board specific GPIO Interrupt
 * @return	Nothing
 */
void Board_GPIO_Int_Init(void);

/**
 * @brief	Initialize pin muxing for SSP interface
 * @param	pSSP	: Pointer to SSP interface to initialize
 * @return	Nothing
 */
void Board_SSP_Init(LPC_SSP_T *pSSP);

/**
 * @brief	Returns the MAC address assigned to this board
 * @param	mcaddr : Pointer to 6-byte character array to populate with MAC address
 * @return	Nothing
 */
void Board_ENET_GetMacADDR(uint8_t *mcaddr);

/**
 * @brief	Initialize pin muxing for a UART
 * @param	pUART	: Pointer to UART register block for UART pins to init
 * @return	Nothing
 */
void Board_UART_Init(LPC_USART_T *pUART);

/**
 * @brief	Initialize pin muxing for SDMMC interface
 * @return	Nothing
 */
void Board_SDMMC_Init(void);

/**
 * @brief	Initialize button(s) interface on board
 * @return	Nothing
 */
void Board_Buttons_Init(void);

/**
 * @brief	Initialize joystick interface on board
 * @return	Nothing
 */
void Board_Joystick_Init(void);

/**
 * @brief	Returns joystick states on board
 * @return	Returns a JOY_* value, ir JOY_PRESS or JOY_UP
 */
uint8_t Joystick_GetStatus(void);

/**
 * @brief	Returns button(s) state on board
 * @return	Returns BUTTONS_BUTTON1 if button1 is pressed
 */
uint32_t Buttons_GetStatus (void);

/**
 * @brief	Initialize I2S interface for the board and UDA1380
 * @param	pI2S	: Pointer to I2S register interface used on this board
 * @param	micIn	: If 1 MIC will be used as input, if 0 LINE_IN will be used
 * @return	Nothing
 */
void Board_Audio_Init(LPC_I2S_T *pI2S, int micIn);

/**
 * @brief	Initialize DAC
 * @param	pDAC	: Pointer to DAC register interface used on this board
 * @return	Nothing
 */
void Board_DAC_Init(LPC_DAC_T *pDAC);

/**
 * @brief	Initialize ADC
 * @return	Nothing
 */
STATIC INLINE void Board_ADC_Init(void){}

/**
 * @brief	Initialize Pinmuxing for the LCD interface
 * @return	Nothing
 */
void Board_LCD_Init(void);

/**
 * @brief	Write given data to LCD module
 * @param	data	: data to be written
 * @param	size	: number of data items
 * @return	Nothing
 */
void Board_LCD_WriteData(const uint8_t *data, uint16_t size);

/**
 * @}
 */

#include "board_api.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */
