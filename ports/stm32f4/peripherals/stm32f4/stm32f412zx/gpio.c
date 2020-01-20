/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

/* GPIO PIN REFERENCE
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
*/

#include "stm32f4xx_hal.h"
#include "stm32f4/gpio.h"
#include "common-hal/microcontroller/Pin.h"

void stm32f4_peripherals_gpio_init(void) {
	//Enable all GPIO for now
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(USB_OTGFS_PPWR_EN_GPIO_Port, USB_OTGFS_PPWR_EN_Pin, GPIO_PIN_SET);

	//Configure LED pins
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	//Status LED chain
	stm32f4_peripherals_status_led(0,1);
	stm32f4_peripherals_status_led(1,0);
	stm32f4_peripherals_status_led(2,0);
	stm32f4_peripherals_status_led(3,0);

	//Never reset pins
	never_reset_pin_number(2,13); //PC13 anti tamp
    never_reset_pin_number(2,14); //PC14 OSC32_IN
    never_reset_pin_number(2,15); //PC15 OSC32_OUT
    never_reset_pin_number(0,13); //PA13 SWDIO
    never_reset_pin_number(0,14); //PA14 SWCLK
    //never_reset_pin_number(0,15); //PA15 JTDI
    //never_reset_pin_number(1,3); //PB3 JTDO
    //never_reset_pin_number(1,4); //PB4 JTRST

    // Port H is not included in GPIO port array
    // never_reset_pin_number(5,0); //PH0 JTDO   
    // never_reset_pin_number(5,1); //PH1 JTRST
}

//LEDs are inverted on F411 DISCO
void stm32f4_peripherals_status_led(uint8_t led, uint8_t state) {
	switch(led)
	{
		case 0: 	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, (state ^ 1));
				break;
		case 1: 	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, (state ^ 1));
				break;
		case 2: 	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, (state ^ 1));
				break;
		case 3: 	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, (state ^ 1));
				break;
		default: break;
	}
}


