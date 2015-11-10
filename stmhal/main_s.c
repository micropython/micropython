/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include STM32_HAL_H
#include "usbd_def.h"
#include "usbd_desc.h"
#include "usbd_cdc_msc_hid.h"
#include "usbd_cdc_interface.h"
#include "usbd_msc_storage.h"


#include "systick.h"

TIM_HandleTypeDef TIM3_Handle;
#define USBD_CDC_POLLING_INTERVAL (10)

#define IRQ_PRI_TIM3 0x6
#define IRQ_SUBPRI_TIM3 0x0



void SystemClock_Config(void);
static uint32_t timer_get_source_freq(uint32_t tim_id);

void flash_error(int n){}



int vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
    return 0;
}


int snprintf(char *str, size_t size, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(str, size, fmt, ap);
    va_end(ap);
    return ret;
}


// Get the frequency (in Hz) of the source clock for the given timer.
// On STM32F405/407/415/417 there are 2 cases for how the clock freq is set.
// If the APB prescaler is 1, then the timer clock is equal to its respective
// APB clock.  Otherwise (APB prescaler > 1) the timer clock is twice its
// respective APB clock.  See DM00031020 Rev 4, page 115.
static uint32_t timer_get_source_freq(uint32_t tim_id) {
    uint32_t source;
    if (tim_id == 1 || (8 <= tim_id && tim_id <= 11)) {
        // TIM{1,8,9,10,11} are on APB2
        source = HAL_RCC_GetPCLK2Freq();
        if ((uint32_t)((RCC->CFGR & RCC_CFGR_PPRE2) >> 3) != RCC_HCLK_DIV1) {
            source *= 2;
        }
    } else {
        // TIM{2,3,4,5,6,7,12,13,14} are on APB1
        source = HAL_RCC_GetPCLK1Freq();
        if ((uint32_t)(RCC->CFGR & RCC_CFGR_PPRE1) != RCC_HCLK_DIV1) {
            source *= 2;
        }
    }
    return source;
}

// TIM3 is set-up for the USB CDC interface
void timer_tim3_init(void) {
    // set up the timer for USBD CDC
    __TIM3_CLK_ENABLE();

    TIM3_Handle.Instance = TIM3;
    TIM3_Handle.Init.Period = (USBD_CDC_POLLING_INTERVAL*1000) - 1; // TIM3 fires every USBD_CDC_POLLING_INTERVAL ms
    TIM3_Handle.Init.Prescaler = timer_get_source_freq(3) / 1000000 - 1; // TIM3 runs at 1MHz
    TIM3_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TIM3_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&TIM3_Handle);

    HAL_NVIC_SetPriority(TIM3_IRQn, IRQ_PRI_TIM3, IRQ_SUBPRI_TIM3);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    if (HAL_TIM_Base_Start(&TIM3_Handle) != HAL_OK) {
        /* Starting Error */
    }
}

void __attribute__((noreturn)) __fatal_error(const char *msg) {
    for (volatile uint delay = 0; delay < 10000000; delay++) {
    }
    while(1);
}

void nlr_jump_fail(void *val) {
    __fatal_error("");
}
/*
 * Defines from usb.h
 */
// Windows needs a different PID to distinguish different device configurations
#define USBD_VID         (0xf055)
#define USBD_PID_CDC_MSC (0x9800)
#define USBD_PID_CDC_HID (0x9801)
#define USBD_PID_CDC     (0x9802)
USBD_HandleTypeDef hUSBDDevice;
/*
 * Satisfy storage dependencies
 */
void storage_init(void){};
uint32_t storage_get_block_size(void){return 0;};
uint32_t storage_get_block_count(void){return 0;};
/*
 * Satisfy dependencies in disk
 */
/* This type MUST be 8 bit */
typedef unsigned char	BYTE;

/* Status of Disk Functions */
typedef BYTE	DSTATUS;
typedef unsigned short	WORD;
typedef unsigned int	UINT;
typedef unsigned long	DWORD;

typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count){ return RES_OK;};
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count){ return RES_OK;};
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff){ return RES_OK;};


bool pyb_usb_dev_init(uint16_t vid, uint16_t pid, usb_device_mode_t mode, USBD_HID_ModeInfoTypeDef *hid_info) {
	// only init USB once in the device's power-lifetime
	USBD_SetVIDPIDRelease(vid, pid, 0x0200, mode == USBD_MODE_CDC);
	if (USBD_SelectMode(mode, hid_info) != 0) {
		return false;
	}
	USBD_Init(&hUSBDDevice, (USBD_DescriptorsTypeDef*)&USBD_Descriptors, 0);
	USBD_RegisterClass(&hUSBDDevice, &USBD_CDC_MSC_HID);
	USBD_CDC_RegisterInterface(&hUSBDDevice, (USBD_CDC_ItfTypeDef*)&USBD_CDC_fops);
	USBD_MSC_RegisterStorage(&hUSBDDevice, (USBD_StorageTypeDef*)&USBD_FLASH_STORAGE_fops);
	USBD_Start(&hUSBDDevice);

    return true;
}

int main(void) {

    /* STM32F4xx HAL library initialization:
         - Configure the Flash prefetch, instruction and Data caches
         - Configure the Systick to generate an interrupt each 1 msec
         - Set NVIC Group Priority to 4
         - Global MSP (MCU Support Package) initialization
       */
    HAL_Init();

    // set the system clock to be HSE
    SystemClock_Config();

    timer_tim3_init();

    // init USB device to default setting if it was not already configured
	pyb_usb_dev_init(USBD_VID, USBD_PID_CDC_MSC, USBD_MODE_CDC, NULL);
    while (1)
    {
    	HAL_Delay(250);
    }
}
