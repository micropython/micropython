/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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


 /******************************************************************************
 IMPORTS
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "hw_memmap.h"
#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "rom_map.h"
#include "interrupt.h"
#include "systick.h"
#include "prcm.h"
#include "sdhost.h"
#include "pin.h"
#include "mpexception.h"
#include "telnet.h"
#include "pybuart.h"

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif


/******************************************************************************
 DECLARE CONSTANTS
 ******************************************************************************/
#define HAL_SDCARD_FREQUENCY_HZ             15000000        // 15MHz

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
#ifndef USE_FREERTOS
static void hal_TickInit (void);
#endif
#if MICROPY_HW_HAS_SDCARD
static void hal_EnableSdCard (void);
#endif

/******************************************************************************
 DECLARE LOCAL VARIABLES
 ******************************************************************************/
static volatile uint32_t HAL_tickCount;

/******************************************************************************
 DECLARE IMPORTED DATA
 ******************************************************************************/
extern void (* const g_pfnVectors[256])(void);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
 
void HAL_SystemInit (void) {
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);

    // in the case of a release image, these steps are already performed by
    // the bootloader so we can skip it and gain some code space
#ifndef NDEBUG
    MAP_IntMasterEnable();
    PRCMCC3200MCUInit();
#endif

#ifndef USE_FREERTOS
    hal_TickInit();
#endif
#if MICROPY_HW_HAS_SDCARD
    hal_EnableSdCard();
#endif
}

void HAL_SystemDeInit (void) {
}
 
void HAL_IncrementTick(void) {
    HAL_tickCount++;
}

uint32_t HAL_GetTick(void) {
    return HAL_tickCount;
}

void HAL_Delay(uint32_t delay) {
#ifdef USE_FREERTOS
    vTaskDelay (delay / portTICK_PERIOD_MS);
#else
    uint32_t start = HAL_tickCount;
    // Wraparound of tick is taken care of by 2's complement arithmetic.
    while (HAL_tickCount - start < delay) {
        // Enter sleep mode, waiting for (at least) the SysTick interrupt.
        __WFI();
    }
#endif
}

void mp_hal_set_interrupt_char (int c) {
    mpexception_set_interrupt_char (c);
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn(const char *str, uint32_t len) {
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        uart_tx_strn(MP_STATE_PORT(pyb_stdio_uart), str, len);
    }
    // and also to telnet
    if (telnet_is_active()) {
        telnet_tx_strn(str, len);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, uint32_t len) {
    // send stdout to UART
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        uart_tx_strn_cooked(MP_STATE_PORT(pyb_stdio_uart), str, len);
    }
    // and also to telnet
    if (telnet_is_active()) {
        telnet_tx_strn_cooked(str, len);
    }
}

int mp_hal_stdin_rx_chr(void) {
    for ( ;; ) {
        if (telnet_rx_any()) {
            return telnet_rx_char();
        }
        else if (MP_STATE_PORT(pyb_stdio_uart) != NULL && uart_rx_any(MP_STATE_PORT(pyb_stdio_uart))) {
            return uart_rx_char(MP_STATE_PORT(pyb_stdio_uart));
        }
        HAL_Delay(1);
    }
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/

#ifndef USE_FREERTOS
static void hal_TickInit (void) {
    HAL_tickCount = 0;
    MAP_SysTickIntRegister(HAL_IncrementTick);
    MAP_IntEnable(FAULT_SYSTICK);
    MAP_SysTickIntEnable();
    MAP_SysTickPeriodSet(HAL_FCPU_HZ / HAL_SYSTICK_PERIOD_US);
    // Force a reload of the SysTick counter register
    HWREG(NVIC_ST_CURRENT) = 0;
    MAP_SysTickEnable();
}
#endif

#if MICROPY_HW_HAS_SDCARD
static void hal_EnableSdCard (void) {
    // Configure PIN_06 for SDHOST0 SDHost_D0
    MAP_PinTypeSDHost(PIN_06, PIN_MODE_8);
    // Configure PIN_07 for SDHOST0 SDHost_CLK
    MAP_PinTypeSDHost(PIN_07, PIN_MODE_8);
    // Configure PIN_08 for SDHOST0 SDHost_CMD
    MAP_PinTypeSDHost(PIN_08, PIN_MODE_8);
    // Set the SD card clock as an output pin
    MAP_PinDirModeSet(PIN_07, PIN_DIR_MODE_OUT);
    // Enable SD peripheral clock
    MAP_PRCMPeripheralClkEnable(PRCM_SDHOST, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    // Reset MMCHS
    MAP_PRCMPeripheralReset(PRCM_SDHOST);
    // Configure MMCHS
    MAP_SDHostInit(SDHOST_BASE);
    // Configure the card clock
    MAP_SDHostSetExpClk(SDHOST_BASE, MAP_PRCMPeripheralClockGet(PRCM_SDHOST), HAL_SDCARD_FREQUENCY_HZ);
}
#endif
