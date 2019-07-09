/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>
#include "supervisor/port.h"
#include "boards/board.h"

#include "tick.h"
//#include "shared-bindings/rtc/__init__.h"

#include "stm32f4xx_hal.h"

#define LED3_Pin GPIO_PIN_2
#define LED3_GPIO_Port GPIOE
#define LED4_Pin GPIO_PIN_3
#define LED4_GPIO_Port GPIOE
#define DFSDM_DATIN3_Pin GPIO_PIN_4
#define DFSDM_DATIN3_GPIO_Port GPIOE
#define A0_Pin GPIO_PIN_0
#define A0_GPIO_Port GPIOF
#define LCD_BLCTRL_Pin GPIO_PIN_5
#define LCD_BLCTRL_GPIO_Port GPIOF
#define QSPI_BK1_IO3_Pin GPIO_PIN_6
#define QSPI_BK1_IO3_GPIO_Port GPIOF
#define QSPI_BK1_IO2_Pin GPIO_PIN_7
#define QSPI_BK1_IO2_GPIO_Port GPIOF
#define QSPI_BK1_IO0_Pin GPIO_PIN_8
#define QSPI_BK1_IO0_GPIO_Port GPIOF
#define QSPI_BK1_IO1_Pin GPIO_PIN_9
#define QSPI_BK1_IO1_GPIO_Port GPIOF
#define STLK_MCO_Pin GPIO_PIN_0
#define STLK_MCO_GPIO_Port GPIOH
#define DFSDM_CKOUT_Pin GPIO_PIN_2
#define DFSDM_CKOUT_GPIO_Port GPIOC
#define JOY_SEL_Pin GPIO_PIN_0
#define JOY_SEL_GPIO_Port GPIOA
#define STLINK_RX_Pin GPIO_PIN_2
#define STLINK_RX_GPIO_Port GPIOA
#define STLINK_TX_Pin GPIO_PIN_3
#define STLINK_TX_GPIO_Port GPIOA
#define CODEC_I2S3_WS_Pin GPIO_PIN_4
#define CODEC_I2S3_WS_GPIO_Port GPIOA
#define DFSDM_DATIN0_Pin GPIO_PIN_1
#define DFSDM_DATIN0_GPIO_Port GPIOB
#define QSPI_CLK_Pin GPIO_PIN_2
#define QSPI_CLK_GPIO_Port GPIOB
#define EXT_RESET_Pin GPIO_PIN_11
#define EXT_RESET_GPIO_Port GPIOF
#define CTP_RST_Pin GPIO_PIN_12
#define CTP_RST_GPIO_Port GPIOF
#define JOY_RIGHT_Pin GPIO_PIN_14
#define JOY_RIGHT_GPIO_Port GPIOF
#define JOY_LEFT_Pin GPIO_PIN_15
#define JOY_LEFT_GPIO_Port GPIOF
#define JOY_UP_Pin GPIO_PIN_0
#define JOY_UP_GPIO_Port GPIOG
#define JOY_DOWN_Pin GPIO_PIN_1
#define JOY_DOWN_GPIO_Port GPIOG
#define D4_Pin GPIO_PIN_7
#define D4_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_8
#define D5_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_10
#define D7_GPIO_Port GPIOE
#define D8_Pin GPIO_PIN_11
#define D8_GPIO_Port GPIOE
#define D9_Pin GPIO_PIN_12
#define D9_GPIO_Port GPIOE
#define D10_Pin GPIO_PIN_13
#define D10_GPIO_Port GPIOE
#define D11_Pin GPIO_PIN_14
#define D11_GPIO_Port GPIOE
#define D12_Pin GPIO_PIN_15
#define D12_GPIO_Port GPIOE
#define I2C2_SCL_Pin GPIO_PIN_10
#define I2C2_SCL_GPIO_Port GPIOB
#define M2_CKIN_Pin GPIO_PIN_11
#define M2_CKIN_GPIO_Port GPIOB
#define CODEC_I2S3_SCK_Pin GPIO_PIN_12
#define CODEC_I2S3_SCK_GPIO_Port GPIOB
#define D13_Pin GPIO_PIN_8
#define D13_GPIO_Port GPIOD
#define D14_Pin GPIO_PIN_9
#define D14_GPIO_Port GPIOD
#define D15_Pin GPIO_PIN_10
#define D15_GPIO_Port GPIOD
#define LCD_RESET_Pin GPIO_PIN_11
#define LCD_RESET_GPIO_Port GPIOD
#define D0_Pin GPIO_PIN_14
#define D0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_15
#define D1_GPIO_Port GPIOD
#define CODEC_INT_Pin GPIO_PIN_2
#define CODEC_INT_GPIO_Port GPIOG
#define LCD_TE_Pin GPIO_PIN_4
#define LCD_TE_GPIO_Port GPIOG
#define CTP_INT_Pin GPIO_PIN_5
#define CTP_INT_GPIO_Port GPIOG
#define QSPI_BK1_NCS_Pin GPIO_PIN_6
#define QSPI_BK1_NCS_GPIO_Port GPIOG
#define USB_OTGFS_OVRCR_Pin GPIO_PIN_7
#define USB_OTGFS_OVRCR_GPIO_Port GPIOG
#define USB_OTGFS_PPWR_EN_Pin GPIO_PIN_8
#define USB_OTGFS_PPWR_EN_GPIO_Port GPIOG
#define CODEC_I2S3_MCK_Pin GPIO_PIN_7
#define CODEC_I2S3_MCK_GPIO_Port GPIOC
#define uSD_D0_Pin GPIO_PIN_8
#define uSD_D0_GPIO_Port GPIOC
#define uSD_D1_Pin GPIO_PIN_9
#define uSD_D1_GPIO_Port GPIOC
#define M2_CKINA8_Pin GPIO_PIN_8
#define M2_CKINA8_GPIO_Port GPIOA
#define USB_OTGFS_VBUS_Pin GPIO_PIN_9
#define USB_OTGFS_VBUS_GPIO_Port GPIOA
#define USB_OTGFS_ID_Pin GPIO_PIN_10
#define USB_OTGFS_ID_GPIO_Port GPIOA
#define USB_OTGFS_DM_Pin GPIO_PIN_11
#define USB_OTGFS_DM_GPIO_Port GPIOA
#define USB_OTGFS_DP_Pin GPIO_PIN_12
#define USB_OTGFS_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define uSD_D2_Pin GPIO_PIN_10
#define uSD_D2_GPIO_Port GPIOC
#define uSD_D3_Pin GPIO_PIN_11
#define uSD_D3_GPIO_Port GPIOC
#define uSD_CLK_Pin GPIO_PIN_12
#define uSD_CLK_GPIO_Port GPIOC
#define D2_Pin GPIO_PIN_0
#define D2_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_1
#define D3_GPIO_Port GPIOD
#define uSD_CMD_Pin GPIO_PIN_2
#define uSD_CMD_GPIO_Port GPIOD
#define uSD_DETECT_Pin GPIO_PIN_3
#define uSD_DETECT_GPIO_Port GPIOD
#define FMC_NOE_Pin GPIO_PIN_4
#define FMC_NOE_GPIO_Port GPIOD
#define FMC_NWE_Pin GPIO_PIN_5
#define FMC_NWE_GPIO_Port GPIOD
#define FMC_NE1_Pin GPIO_PIN_7
#define FMC_NE1_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define CODEC_I2S3ext_SD_Pin GPIO_PIN_4
#define CODEC_I2S3ext_SD_GPIO_Port GPIOB
#define CODEC_I2S3_SD_Pin GPIO_PIN_5
#define CODEC_I2S3_SD_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_6
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB
#define I2C2_SDA_Pin GPIO_PIN_9
#define I2C2_SDA_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOE

//UART_HandleTypeDef huart2;

/*
#include "shared-module/gamepad/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/bleio/__init__.h"
#include "common-hal/busio/I2C.h"
#include "common-hal/busio/SPI.h"
#include "common-hal/busio/UART.h"
#include "common-hal/pulseio/PWMOut.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/rtc/RTC.h"

*/

//#include "shared-bindings/rtc/__init__.h"

static void power_warning_handler(void) {
    reset_into_safe_mode(BROWNOUT);
}

safe_mode_t port_init(void) {
	HAL_Init();

	//System clock init
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	/** Configure the main internal regulator output voltage 
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 72;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 3;
	RCC_OscInitStruct.PLL.PLLR = 2;
	/** Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                      |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S_APB1|RCC_PERIPHCLK_SDIO
	                      |RCC_PERIPHCLK_CLK48;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 50;
	PeriphClkInitStruct.PLLI2S.PLLI2SM = 4;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
	PeriphClkInitStruct.PLLI2S.PLLI2SQ = 2;
	PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
	PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48;
	PeriphClkInitStruct.PLLI2SSelection = RCC_PLLI2SCLKSOURCE_PLLSRC;
	PeriphClkInitStruct.I2sApb1ClockSelection = RCC_I2SAPB1CLKSOURCE_PLLI2S;
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);

	//GPIO Init
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE, LED3_Pin|LED4_Pin|LED1_Pin|LED2_Pin, GPIO_PIN_RESET);
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOF, LCD_BLCTRL_Pin|EXT_RESET_Pin|CTP_RST_Pin, GPIO_PIN_RESET);
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(USB_OTGFS_PPWR_EN_GPIO_Port, USB_OTGFS_PPWR_EN_Pin, GPIO_PIN_SET);
	/*Configure GPIO pins : LED3_Pin LED4_Pin LED1_Pin LED2_Pin */
	GPIO_InitStruct.Pin = LED3_Pin|LED4_Pin|LED1_Pin|LED2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	/*Configure GPIO pin : USB_OTGFS_PPWR_EN_Pin */
	GPIO_InitStruct.Pin = USB_OTGFS_PPWR_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(USB_OTGFS_PPWR_EN_GPIO_Port, &GPIO_InitStruct);

  	//turn on LED
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);


    tick_init();
    board_init(); 

    return NO_SAFE_MODE;
}

void reset_port(void) {

}

void reset_to_bootloader(void) {

}

void reset_cpu(void) {

}

extern uint32_t _ebss;
// Place the word to save just after our BSS section that gets blanked.
void port_set_saved_word(uint32_t value) {
    _ebss = value;
}

uint32_t port_get_saved_word(void) {
    return _ebss;
}

void HardFault_Handler(void) {

}
