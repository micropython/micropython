/*
 * @brief Common board API functions
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

#ifndef __BOARD_API_H_
#define __BOARD_API_H_

#include "lpc_types.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_COMMON_API BOARD: Common board functions
 * @ingroup BOARD_Common
 * This file contains common board definitions that are shared across
 * boards and devices. All of these functions do not need to be
 * implemented for a specific board, but if they are implemented, they
 * should use this API standard.
 * @{
 */

/**
 * @brief	Setup and initialize hardware prior to call to main()
 * @return	None
 * @note	Board_SystemInit() is called prior to the application and sets up system
 * clocking, memory, and any resources needed prior to the application
 * starting.
 */
void Board_SystemInit(void);

/**
 * @brief	Setup pin multiplexer per board schematics
 * @return	None
 * @note	Board_SetupMuxing() should be called from SystemInit() prior to application
 * main() is called. So that the PINs are set in proper state.
 */
void Board_SetupMuxing(void);

/**
 * @brief	Setup system clocking
 * @return	None
 * @note	This sets up board clocking.
 */
void Board_SetupClocking(void);

/**
 * @brief	Setup external system memory
 * @return	None
 * @note	This function is typically called after pin mux setup and clock setup and
 * sets up any external memory needed by the system (DRAM, SRAM, etc.). Not all
 * boards need this function.
 */
void Board_SetupExtMemory(void);

/**
 * @brief	Set up and initialize all required blocks and functions related to the board hardware.
 * @return	None
 */
void Board_Init(void);

/**
 * @brief	Initializes board UART for output, required for printf redirection
 * @return	None
 */
void Board_Debug_Init(void);

/**
 * @brief	Sends a single character on the UART, required for printf redirection
 * @param	ch	: character to send
 * @return	None
 */
void Board_UARTPutChar(char ch);

/**
 * @brief	Get a single character from the UART, required for scanf input
 * @return	EOF if not character was received, or character value
 */
int Board_UARTGetChar(void);

/**
 * @brief	Prints a string to the UART
 * @param	str	: Terminated string to output
 * @return	None
 */
void Board_UARTPutSTR(const char *str);

/**
 * @brief	Sets the state of a board LED to on or off
 * @param	LEDNumber	: LED number to set state for
 * @param	State		: true for on, false for off
 * @return	None
 */
void Board_LED_Set(uint8_t LEDNumber, bool State);

/**
 * @brief	Returns the current state of a board LED
 * @param	LEDNumber	: LED number to set state for
 * @return	true if the LED is on, otherwise false
 */
bool Board_LED_Test(uint8_t LEDNumber);

/**
 * @brief	Toggles the current state of a board LED
 * @param	LEDNumber	: LED number to change state for
 * @return	None
 */
void Board_LED_Toggle(uint8_t LEDNumber);

/**
 * @brief	Turn on Board LCD Backlight
 * @param	Intensity	: Backlight intensity (0 = off, >=1 = on)
 * @return	None
 * @note	On boards where a GPIO is used to control backlight on/off state, a '0' or '1'
 * value will turn off or on the backlight. On some boards, a non-0 value will
 * control backlight intensity via a PWN. For PWM systems, the intensity value
 * is a percentage value between 0 and 100%.
 */
void Board_SetLCDBacklight(uint8_t Intensity);

/**
 * @brief Function prototype for a MS delay function. Board layers or example code may
 *        define this function as needed.
 */
typedef void (*p_msDelay_func_t)(uint32_t);

/* The DEBUG* functions are selected based on system configuration.
   Code that uses the DEBUG* functions will have their I/O routed to
   the UART, semihosting, or nowhere. */
#if defined(DEBUG_ENABLE)
#if defined(DEBUG_SEMIHOSTING)
#define DEBUGINIT()
#define DEBUGOUT(...) printf(__VA_ARGS__)
#define DEBUGSTR(str) printf(str)
#define DEBUGIN() (int) EOF

#else
#define DEBUGINIT() Board_Debug_Init()
#define DEBUGOUT(...) printf(__VA_ARGS__)
#define DEBUGSTR(str) Board_UARTPutSTR(str)
#define DEBUGIN() Board_UARTGetChar()
#endif /* defined(DEBUG_SEMIHOSTING) */

#else
#define DEBUGINIT()
#define DEBUGOUT(...)
#define DEBUGSTR(str)
#define DEBUGIN() (int) EOF
#endif /* defined(DEBUG_ENABLE) */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_API_H_ */
