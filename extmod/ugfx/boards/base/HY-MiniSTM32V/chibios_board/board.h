/*
    ChibiOS/RT - Copyright (C) 2006-2015 Giovanni Di Sirio

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
 * Board file for HY-MiniSTM32V board from HAOYU (China).
 * www.powermcu.com or www.hotmcu.com.
 */

/*
 * Board identifier.
 */
#define BOARD_HY_MINI_STM32V
#define BOARD_NAME              "HY-MiniSTM32V"

/*
 * Board frequencies.
 */
#define STM32_LSECLK            32768
#define STM32_HSECLK            8000000

/*
 * MCU type, supported types are defined in ./os/hal/platforms/hal_lld.h.
 */
#define STM32F10X_HD

/*
 * IO pins assignments.
 */
 
#define GPIOA_PIN0 			0
#define GPIOA_PIN1			1
#define GPIOA_PIN2			2
#define GPIOA_PIN3			3
#define GPIOA_TP_CS			4
#define GPIOA_SPI_SCK 		5
#define GPIOA_SPI_MISO		6 
#define GPIOA_SPI_MOSI		7
#define GPIOA_PIN8			8
#define GPIOA_USART1_TX		9
#define GPIOA_USART1_RX		10
#define GPIOA_USB_DM 		11
#define GPIOA_USB_DP		12 
#define GPIOA_PIN13			13
#define GPIOA_PIN14			14
#define GPIOA_PIN15			15

#define GPIOB_LED1			0
#define GPIOB_LED2			1
#define GPIOB_USER_KEYB		2
#define GPIOB_PIN3			3
#define GPIOB_PIN4			4
#define GPIOB_BL_CNT		5
#define GPIOB_TP_IRQ		6
#define GPIOB_USB_EN		7
#define GPIOB_PIN8			8
#define GPIOB_PIN9			9
#define GPIOB_PIN10			10
#define GPIOB_PIN11			11
#define GPIOB_PIN12			12
#define GPIOB_PIN13			13
#define GPIOB_PIN14			14
#define GPIOB_PIN15			15

#define GPIOC_PIN0			0
#define GPIOC_PIN1			1
#define GPIOC_PIN2			2
#define GPIOC_PIN3			3
#define GPIOC_PIN4			4
#define GPIOC_PIN5			5
#define GPIOC_PIN6			6
#define GPIOC_PIN7			7
#define GPIOC_SDIO_D0		8
#define GPIOC_SDIO_D1		9
#define GPIOC_SDIO_D2		10
#define GPIOC_SDIO_D3		11
#define GPIOC_SDIO_CK		12
#define GPIOC_USER_KEYA		13
#define GPIOC_PIN14			14
#define GPIOC_PIN15			15

#define GPIOD_FSMC_D2		0
#define GPIOD_FSMC_D3		1
#define GPIOD_SDIO_CMD		2
#define GPIOD_SD_CD			3
#define GPIOD_LCD_RD		4
#define GPIOD_LCD_WR		5
#define GPIOD_PIN6			6
#define GPIOD_LCD_CS		7
#define GPIOD_FSMC_D13		8
#define GPIOD_FSMC_D14		9
#define GPIOD_FSMC_D15		10
#define GPIOD_LCD_RS		11
#define GPIOD_PIN12			12
#define GPIOD_PIN13			13
#define GPIOD_FSMC_D0		14
#define GPIOD_FSMC_D1		15

#define GPIOE_PIN0			0
#define GPIOE_PIN1			1
#define GPIOE_PIN2			2
#define GPIOE_PIN3			3
#define GPIOE_PIN4			4
#define GPIOE_PIN5			5
#define GPIOE_PIN6			6
#define GPIOE_FSMC_D4		7
#define GPIOE_FSMC_D5		8
#define GPIOE_FSMC_D6		9
#define GPIOE_FSMC_D7		10
#define GPIOE_FSMC_D8		11
#define GPIOE_FSMC_D9		12
#define GPIOE_FSMC_D10		13
#define GPIOE_FSMC_D11		14
#define GPIOE_FSMC_D12		15

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 *
 * The digits have the following meaning:
 *   0 - Analog input.
 *   1 - Push Pull output 10MHz.
 *   2 - Push Pull output 2MHz.
 *   3 - Push Pull output 50MHz.
 *   4 - Digital input.
 *   5 - Open Drain output 10MHz.
 *   6 - Open Drain output 2MHz.
 *   7 - Open Drain output 50MHz.
 *   8 - Digital input with PullUp or PullDown resistor depending on ODR.
 *   9 - Alternate Push Pull output 10MHz.
 *   A - Alternate Push Pull output 2MHz.
 *   B - Alternate Push Pull output 50MHz.
 *   C - Reserved.
 *   D - Alternate Open Drain output 10MHz.
 *   E - Alternate Open Drain output 2MHz.
 *   F - Alternate Open Drain output 50MHz.
 * Please refer to the STM32 Reference Manual for details.
 */

/*
 * Port A setup.
 * Everything input with pull-up except:
 * PA4  - Push Pull output 50MHz 			(TP_CS)
 * PA5  - Alternate Push Pull output 50MHz	(SPI_SCK)
 * PA6  - Digital input						(SPI_MISO) 
 * PA7  - Alternate Push Pull output 50MHz	(SPI_MOSI)
 * PA9  - Alternate Push Pull output 50MHz	(USART1_TX)
 * PA10  - Digital input					(USART1_RX)
 * PA11  - Digital input					(USB_DM)
 */
#define VAL_GPIOACRL            0xB4B38888      /*  PA7...PA0 */
#define VAL_GPIOACRH            0x888844B8      /* PA15...PA8 */
#define VAL_GPIOAODR            0xFFFFFFFF

/*
 * Port B setup.
 * Everything input with pull-up except:
 * PB0 - Push Pull output 50MHz 			(LED1)
 * PB1 - Push Pull output 50MHz 			(LED2)
 * PB2 - Digital input						(USER_KEYB)
 * PB5 - Alternate Push Pull output 50MHz	(BL_CNT)
 * PB6 - Digital input					    (TP_IRQ)
 * PB7 - Push Pull output 50MHz (init low)  (USB_EN)
 */

#define VAL_GPIOBCRL            0x34B88433      /*  PB7...PB0 */
#define VAL_GPIOBCRH            0x88888888      /* PB15...PB8 */
#define VAL_GPIOBODR            0xFFFF7FFC

/*
 * Port C setup.
 * Everything input with pull-up except:
 * PC8 - Digital input						(SDIO_D0)
 * PC9 - Digital input						(SDIO_D1)
 * PC10 - Digital input						(SDIO_D2)
 * PC11 - Digital input						(SDIO_D3)
 * PC12 - Digital input						(SDIO_CK)
 * PC13 - Digital input						(GPIOC_USER_KEYA)
 */
#define VAL_GPIOCCRL            0x88888888      /*  PC7...PC0 */
#define VAL_GPIOCCRH            0x88444444      /* PC15...PC8 */
#define VAL_GPIOCODR            0xFFFFFFFF

/*
 * Port D setup.
 * Everything input with pull-up except:
 * PD0 - Alternate Push Pull output 50MHz	(FSMC_D2)
 * PD1 - Alternate Push Pull output 50MHz	(FSMC_D1)
 * PD2 - Digital input						(SDIO_CMD)
 * PD3 - Digital input						(SD_CD)
 * PD4 - Alternate Push Pull output 50MHz	(LCD_RD)
 * PD5 - Alternate Push Pull output 50MHz	(LCD_WR)
 * PD7 - Alternate Push Pull output 50MHz	(LCD_CS)
 * PD8 - Alternate Push Pull output 50MHz	(FSMC_D13)
 * PD9 - Alternate Push Pull output 50MHz	(FSMC_D14)
 * PD10 - Alternate Push Pull output 50MHz	(FSMC_D15)
 * PD11 - Alternate Push Pull output 50MHz	(LCD_RS)
 * PD14 - Alternate Push Pull output 50MHz	(FSMC_D0)
 * PD15 - Alternate Push Pull output 50MHz	(FSMC_D1)
 */

#define VAL_GPIODCRL            0xB8BB44BB      /*  PD7...PD0 */
#define VAL_GPIODCRH            0xBB88BBBB      /* PD15...PD8 */
#define VAL_GPIODODR            0xFFFFFFFF

/*
 * Port E setup.
 * Everything input with pull-up except:
 * PE7 - Alternate Push Pull output 50MHz	(FSMC_D4)
 * PE8 - Alternate Push Pull output 50MHz	(FSMC_D5)
 * PE9 - Alternate Push Pull output 50MHz	(FSMC_D6)
 * PE10 - Alternate Push Pull output 50MHz	(FSMC_D7)
 * PE11 - Alternate Push Pull output 50MHz	(FSMC_D8)
 * PE12 - Alternate Push Pull output 50MHz	(FSMC_D9)
 * PE13 - Alternate Push Pull output 50MHz	(FSMC_D10)
 * PE14 - Alternate Push Pull output 50MHz	(FSMC_D11)
 * PE15 - Alternate Push Pull output 50MHz	(FSMC_D12)
 */

#define VAL_GPIOECRL            0xB8888888      /*  PE7...PE0 */
#define VAL_GPIOECRH            0xBBBBBBBB      /* PE15...PE8 */
#define VAL_GPIOEODR            0xFFFFFFFF

/*
 * Port F setup.
 * Everything input with pull-up
 */

#define VAL_GPIOFCRL            0x88888888      /*  PF7...PE0 */
#define VAL_GPIOFCRH            0x88888888      /* PF15...PE8 */
#define VAL_GPIOFODR            0xFFFFFFFF

/*
 * Port G setup.
 * Everything input with pull-up
 */

#define VAL_GPIOGCRL            0x88888888      /*  PG7...PE0 */
#define VAL_GPIOGCRH            0x88888888      /* PG15...PE8 */
#define VAL_GPIOGODR            0xFFFFFFFF

/*
 * USB bus activation macro, required by the USB driver.
 */
#define usb_lld_connect_bus(usbp) palClearPad(GPIOB, GPIOB_USB_EN)

/*
 * USB bus de-activation macro, required by the USB driver.
 */
#define usb_lld_disconnect_bus(usbp) palSetPad(GPIOB, GPIOB_USB_EN)

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
